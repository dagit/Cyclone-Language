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
# 69 "core.h"
void exit(int);
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;
# 38
int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 310
int toupper(int);
# 318
int system(const char*);
# 321
int mkdir(const char*pathname,unsigned short mode);
# 324
int close(int);
int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);
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
# 288 "core.h"
struct _dyneither_ptr Cstring_to_string(char*);
# 299
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
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 49
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 70
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);struct Cyc_Relations_Reln;
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1131 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1194
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 36
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
typedef struct _tuple11*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Dict_T;
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
# 119 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 139
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 36 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 81 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 89
extern char*Ccomp;
# 91
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
# 94
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 98
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 101
if(Cyc_log_file == 0){
{const char*_tmp425;void*_tmp424;(_tmp424=0,Cyc_fprintf(Cyc_stderr,((_tmp425="Internal error: log file is NULL\n",_tag_dyneither(_tmp425,sizeof(char),34))),_tag_dyneither(_tmp424,sizeof(void*),0)));}
exit(1);}{
# 105
int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return _tmp2;};}
# 110
static struct _dyneither_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _dyneither_ptr*sptr){
struct Cyc_Set_Set**_tmp426;Cyc_current_targets=((_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr),_tmp426))));}struct _tuple20{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 117
typedef struct _tuple20*Cyc_line_t;
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 124
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple21{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 131
typedef struct _tuple21*Cyc_suck_line_t;
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple22{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 139
typedef struct _tuple22*Cyc_ifdefined_t;struct _tuple23{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};
# 141
typedef struct _tuple23*Cyc_spec_t;
# 143
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 161
int Cyc_numdef=0;
# 163
static struct Cyc_List_List*Cyc_cppargs=0;
# 166
const int Cyc_lex_base[425]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 22,1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 20,292,296,300,313,321,326,- 15,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 17,422,421,415,436,433,449,427,449,453,441,445,436,436,- 19,444,438,442,453,464,447,449,482,489,490,4,6,21,491,492,505,504,516,516,524,554,24,556,557,24,20,570,571,53,627,644,- 13,647,594,596,583,584,598,598,605,606,1,677,623,624,654,647,654,658,659,623,624,639,645,648,703,704,705,654,655,710,720,721,679,680,735,737,765,713,714,770,797,798,746,747,803,814,815,- 12,762,763,1030,- 21,1165,762,763,760,773,772,767,769,772,773,771,827,1144,819,820,818,832,1258,1170,871,872,862,864,862,875,1370,893,894,892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,1150,- 2,945,- 4,974,998,1028,980,1010,1023,1448,981,2320,2363,994,1055,993,996,2433,994,1057,- 36,- 42,- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,- 39,- 48,2583,2612,1467,1081,1091,1563,2622,2652,1582,2281,2685,2716,2754,1083,1093,2824,2862,1118,1157,1195,1206,1198,1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1048,2902,1112,1117,2128,1119,1124,1125,1133,1134,1138,1146,1147,1191,2975,3059,- 23,- 16,- 18,2239,1192,- 24,- 41,- 38,- 35,- 26,1394,3141,2,3224,1191,15,1170,1174,1175,1173,1171,1185,1255};
const int Cyc_lex_backtrk[425]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[425]={- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3481]={0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,19,45,45,45,45,45,22,45,0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,224,22,415,415,415,415,415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,31,217,218,221,415,130,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,413,413,413,413,413,413,413,413,413,413,121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,60,413,61,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,69,70,129,34,34,34,34,34,34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,325,326,104,327,328,329,330,331,331,331,331,331,331,331,331,331,332,77,333,334,335,104,19,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,307,338,37,308,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,309,88,95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,192,193,26,35,35,35,35,35,35,35,35,178,172,163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,150,78,151,152,153,154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,159,143,144,145,160,146,161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,113,113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,169,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,175,97,97,104,104,97,176,104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,80,21,97,182,104,183,184,185,186,112,187,188,189,190,191,271,194,105,195,80,196,112,112,91,197,112,198,199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,21,112,200,201,202,206,207,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,208,209,210,211,106,212,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,219,220,92,222,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,225,- 1,226,- 1,227,228,99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,35,35,35,35,35,99,235,236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,243,240,50,241,244,245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,137,137,137,137,137,137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,259,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,102,100,261,262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,33,269,270,272,273,288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,282,36,284,285,286,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,291,44,292,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,44,411,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,410,408,78,- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,351,383,346,341,78,343,344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,79,79,79,79,79,79,79,79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,294,155,80,287,287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,278,80,270,223,275,275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,79,79,79,79,79,275,100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,294,419,292,316,316,277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,292,0,316,0,0,316,0,21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,0,0,0,92,0,300,412,0,412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,412,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,378,0,0,0,0,357,0,357,0,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,358,358,358,358,358,358,358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,125,125,125,125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,- 1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,405,405,405,0,126,127,127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,136,136,136,136,136,0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,138,312,312,312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,136,136,136,136,136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,314,314,314,314,314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,314,314,314,314,314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,314,0,0,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,314,314,314,314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,353,353,353,353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,0,0,0,0,0,356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,356,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,359,359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,352,93,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,359,359,359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,93,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,0,0,0,0,373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,0,0,354,0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,378,378,378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,368,368,368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,360,93,0,0,0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,368,368,368,368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,370,0,0,390,390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,369,0,0,0,0,0,0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,395,0,0,0,0,0,0,0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,402,0,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,413,413,413,413,413,413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,0,0,0,0,413,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3481]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,308,311,41,41,45,45,41,417,45,- 1,- 1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,204,10,38,213,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,216,217,220,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,17,66,142,27,161,7,73,73,163,164,73,142,165,166,167,11,11,11,11,11,11,11,11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,111,168,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,169,88,170,171,172,173,174,96,96,104,104,96,175,104,105,105,176,178,105,179,88,180,13,13,80,80,13,73,80,16,96,181,104,182,183,184,185,105,186,187,188,189,190,192,193,13,194,80,195,112,112,4,196,112,197,198,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,199,200,201,205,206,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,207,208,209,210,13,211,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,212,14,214,215,32,32,32,32,32,32,32,32,32,32,92,92,218,219,92,221,14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,222,92,224,103,225,110,226,227,99,99,228,14,99,229,230,231,232,32,32,32,32,32,32,128,128,128,128,128,128,128,128,99,234,235,236,238,111,237,130,15,15,237,239,15,101,101,240,241,101,242,237,14,237,243,244,245,14,14,246,247,248,249,15,14,250,251,252,101,134,134,134,134,134,134,134,134,253,254,15,15,15,15,15,15,15,15,15,15,237,255,256,257,92,258,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,259,99,260,261,15,262,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,263,264,15,265,266,101,267,33,33,33,33,33,33,33,33,33,33,268,269,271,272,276,277,278,33,33,33,33,33,33,137,137,137,137,137,137,137,137,279,280,281,282,283,284,285,42,42,42,42,42,42,42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,286,288,289,290,42,291,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,294,295,296,15,297,298,299,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,302,303,304,44,320,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,322,323,78,324,273,273,325,329,273,312,312,312,312,312,312,312,312,332,326,334,337,78,335,335,323,139,139,273,344,139,140,140,324,327,140,273,326,78,78,78,78,78,78,78,78,78,78,347,139,327,327,377,386,140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,333,333,333,338,78,324,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,139,388,79,287,287,140,389,287,391,318,139,318,318,392,393,140,355,356,365,366,140,79,394,395,275,275,287,396,275,293,293,338,318,293,287,397,398,79,79,79,79,79,79,79,79,79,79,275,355,356,365,366,293,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,399,406,416,370,79,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,275,370,89,292,292,293,418,292,309,309,275,419,309,420,421,293,371,422,372,373,324,89,423,424,316,316,292,- 1,316,- 1,- 1,309,- 1,309,292,- 1,- 1,89,89,89,89,89,89,89,89,89,89,316,371,316,372,373,- 1,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,300,300,90,- 1,300,305,305,- 1,- 1,305,315,315,315,315,315,315,315,315,- 1,- 1,- 1,90,- 1,300,412,- 1,412,412,305,- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,90,90,90,90,90,90,90,412,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,- 1,328,97,328,328,328,328,328,328,328,328,328,328,- 1,- 1,- 1,- 1,354,- 1,354,- 1,97,354,354,354,354,354,354,354,354,354,354,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,360,- 1,360,- 1,98,360,360,360,360,360,360,360,360,360,360,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,390,- 1,- 1,126,126,126,126,126,126,390,390,390,390,390,390,390,390,- 1,123,127,127,127,127,127,127,127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,127,127,127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,136,136,136,136,136,136,136,136,136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,310,310,310,310,310,310,310,310,405,405,405,405,405,405,405,405,- 1,136,136,136,136,136,136,- 1,313,313,313,313,313,313,313,313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,- 1,313,313,313,313,313,313,- 1,- 1,310,361,361,361,361,361,361,361,361,361,361,- 1,- 1,314,314,314,314,314,314,314,314,314,314,310,313,313,313,313,313,313,314,314,314,314,314,314,- 1,- 1,330,- 1,330,330,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,314,314,314,314,314,314,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,330,331,- 1,331,331,331,331,331,331,331,331,331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,342,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,352,352,352,352,352,352,352,352,352,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,- 1,353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,352,358,- 1,- 1,- 1,- 1,353,358,- 1,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,353,- 1,- 1,- 1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,- 1,359,353,358,- 1,- 1,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,- 1,359,362,363,- 1,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,- 1,- 1,- 1,- 1,- 1,364,363,364,364,364,364,364,364,364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,378,378,378,378,378,378,378,378,378,- 1,- 1,- 1,364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,- 1,- 1,- 1,- 1,- 1,378,364,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,367,367,367,367,367,378,378,- 1,- 1,- 1,- 1,- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,368,368,368,368,368,368,368,368,368,- 1,367,367,367,367,367,367,368,368,368,368,368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,387,387,387,387,387,387,387,387,- 1,368,368,368,368,368,368,387,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,387,- 1,387,- 1,387,- 1,387,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,413,- 1,- 1,413,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 173
int state;int base;int backtrk;
int c;
state=start_state;
# 177
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 181
state=(- state)- 1;}
# 183
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(425,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(425,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 191
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 195
c=256;}}else{
# 197
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 200
if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
# 203
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){
struct Cyc_Lexing_Error_exn_struct _tmp42C;const char*_tmp42B;struct Cyc_Lexing_Error_exn_struct*_tmp42A;(int)_throw((void*)((_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A[0]=((_tmp42C.tag=Cyc_Lexing_Error,((_tmp42C.f1=((_tmp42B="empty token",_tag_dyneither(_tmp42B,sizeof(char),12))),_tmp42C)))),_tmp42A)))));}else{
# 209
return lbuf->lex_last_action;}}else{
# 212
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 216
struct _tuple20*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL0:
# 175 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
struct Cyc_Set_Set**_tmp42D;Cyc_current_targets=((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp42D))));}{
# 180
struct _tuple20*_tmp42E;return(_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp42E->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp42E)))));};case 1: _LL1:
# 183 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL2:
# 185
 return 0;default: _LL3:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}{
# 189
struct Cyc_Lexing_Error_exn_struct _tmp434;const char*_tmp433;struct Cyc_Lexing_Error_exn_struct*_tmp432;(int)_throw((void*)((_tmp432=_cycalloc(sizeof(*_tmp432)),((_tmp432[0]=((_tmp434.tag=Cyc_Lexing_Error,((_tmp434.f1=((_tmp433="some action didn't return!",_tag_dyneither(_tmp433,sizeof(char),27))),_tmp434)))),_tmp432)))));};}
# 191
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL5:
# 189 "buildlib.cyl"
{struct _dyneither_ptr*_tmp435;Cyc_current_source=((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp435))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp436;Cyc_current_targets=((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp436))));}
Cyc_token(lexbuf);
return 0;case 1: _LL6:
# 197
{struct _dyneither_ptr*_tmp437;Cyc_current_source=((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp437))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp438;Cyc_current_targets=((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp438))));}
Cyc_args(lexbuf);
return 0;case 2: _LL7:
# 205
{struct _dyneither_ptr*_tmp439;Cyc_current_source=((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp439))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp43A;Cyc_current_targets=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp43A))));}
Cyc_token(lexbuf);
return 0;default: _LL8:
# 211
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{
# 214
struct Cyc_Lexing_Error_exn_struct _tmp440;const char*_tmp43F;struct Cyc_Lexing_Error_exn_struct*_tmp43E;(int)_throw((void*)((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E[0]=((_tmp440.tag=Cyc_Lexing_Error,((_tmp440.f1=((_tmp43F="some action didn't return!",_tag_dyneither(_tmp43F,sizeof(char),27))),_tmp440)))),_tmp43E)))));};}
# 216
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLA: {
# 214 "buildlib.cyl"
struct _dyneither_ptr*_tmp441;struct _dyneither_ptr*_tmp15=(_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp441)));
{struct Cyc_List_List*_tmp442;Cyc_current_args=((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442->hd=_tmp15,((_tmp442->tl=Cyc_current_args,_tmp442))))));}
return Cyc_args(lexbuf);}case 1: _LLB: {
# 220
struct _dyneither_ptr*_tmp443;struct _dyneither_ptr*_tmp18=(_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp443)));
{struct Cyc_List_List*_tmp444;Cyc_current_args=((_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444->hd=_tmp18,((_tmp444->tl=Cyc_current_args,_tmp444))))));}
return Cyc_args(lexbuf);}case 2: _LLC: {
# 226
struct _dyneither_ptr*_tmp445;struct _dyneither_ptr*_tmp1B=(_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp445)));
{struct Cyc_List_List*_tmp446;Cyc_current_args=((_tmp446=_cycalloc(sizeof(*_tmp446)),((_tmp446->hd=_tmp1B,((_tmp446->tl=Cyc_current_args,_tmp446))))));}
return Cyc_token(lexbuf);}default: _LLD:
# 231
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}{
# 234
struct Cyc_Lexing_Error_exn_struct _tmp44C;const char*_tmp44B;struct Cyc_Lexing_Error_exn_struct*_tmp44A;(int)_throw((void*)((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=((_tmp44C.tag=Cyc_Lexing_Error,((_tmp44C.f1=((_tmp44B="some action didn't return!",_tag_dyneither(_tmp44B,sizeof(char),27))),_tmp44C)))),_tmp44A)))));};}
# 236
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLF:
# 235 "buildlib.cyl"
{struct _dyneither_ptr*_tmp44D;Cyc_add_target(((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp44D)))));}return Cyc_token(lexbuf);case 1: _LL10:
# 238 "buildlib.cyl"
 return 0;case 2: _LL11:
# 241 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3: _LL12:
# 244 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL13:
# 247 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5: _LL14:
# 249
 return Cyc_token(lexbuf);case 6: _LL15:
# 251
 return Cyc_token(lexbuf);case 7: _LL16:
# 253
 return Cyc_token(lexbuf);case 8: _LL17:
# 256 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9: _LL18:
# 259 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10: _LL19:
# 262 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11: _LL1A:
# 264
 return Cyc_token(lexbuf);case 12: _LL1B:
# 266
 return Cyc_token(lexbuf);case 13: _LL1C:
# 268
 return Cyc_token(lexbuf);case 14: _LL1D:
# 270
 return Cyc_token(lexbuf);case 15: _LL1E:
# 272
 return Cyc_token(lexbuf);case 16: _LL1F:
# 274
 return Cyc_token(lexbuf);case 17: _LL20:
# 276
 return Cyc_token(lexbuf);case 18: _LL21:
# 278
 return Cyc_token(lexbuf);case 19: _LL22:
# 280
 return Cyc_token(lexbuf);case 20: _LL23:
# 282
 return Cyc_token(lexbuf);case 21: _LL24:
# 284
 return Cyc_token(lexbuf);case 22: _LL25:
# 286
 return Cyc_token(lexbuf);case 23: _LL26:
# 288
 return Cyc_token(lexbuf);case 24: _LL27:
# 291 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25: _LL28:
# 293
 return Cyc_token(lexbuf);case 26: _LL29:
# 295
 return Cyc_token(lexbuf);case 27: _LL2A:
# 297
 return Cyc_token(lexbuf);case 28: _LL2B:
# 299
 return Cyc_token(lexbuf);case 29: _LL2C:
# 301
 return Cyc_token(lexbuf);case 30: _LL2D:
# 303
 return Cyc_token(lexbuf);case 31: _LL2E:
# 305
 return Cyc_token(lexbuf);case 32: _LL2F:
# 307
 return Cyc_token(lexbuf);case 33: _LL30:
# 309
 return Cyc_token(lexbuf);case 34: _LL31:
# 311
 return Cyc_token(lexbuf);case 35: _LL32:
# 313
 return Cyc_token(lexbuf);case 36: _LL33:
# 315
 return Cyc_token(lexbuf);case 37: _LL34:
# 317
 return Cyc_token(lexbuf);case 38: _LL35:
# 319
 return Cyc_token(lexbuf);case 39: _LL36:
# 321
 return Cyc_token(lexbuf);case 40: _LL37:
# 323
 return Cyc_token(lexbuf);case 41: _LL38:
# 325
 return Cyc_token(lexbuf);case 42: _LL39:
# 327
 return Cyc_token(lexbuf);case 43: _LL3A:
# 329
 return Cyc_token(lexbuf);case 44: _LL3B:
# 331
 return Cyc_token(lexbuf);case 45: _LL3C:
# 333
 return Cyc_token(lexbuf);case 46: _LL3D:
# 335
 return Cyc_token(lexbuf);case 47: _LL3E:
# 337
 return Cyc_token(lexbuf);case 48: _LL3F:
# 340 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL40:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}{
# 344
struct Cyc_Lexing_Error_exn_struct _tmp453;const char*_tmp452;struct Cyc_Lexing_Error_exn_struct*_tmp451;(int)_throw((void*)((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451[0]=((_tmp453.tag=Cyc_Lexing_Error,((_tmp453.f1=((_tmp452="some action didn't return!",_tag_dyneither(_tmp452,sizeof(char),27))),_tmp453)))),_tmp451)))));};}
# 346
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL42:
# 345 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1: _LL43:
# 346 "buildlib.cyl"
 return 0;case 2: _LL44:
# 347 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3: _LL45:
# 348 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4: _LL46:
# 351 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5: _LL47:
# 354 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6: _LL48:
# 356
 return Cyc_string(lexbuf);case 7: _LL49:
# 357 "buildlib.cyl"
 return 0;case 8: _LL4A:
# 358 "buildlib.cyl"
 return 0;case 9: _LL4B:
# 359 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL4C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}{
# 363
struct Cyc_Lexing_Error_exn_struct _tmp459;const char*_tmp458;struct Cyc_Lexing_Error_exn_struct*_tmp457;(int)_throw((void*)((_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457[0]=((_tmp459.tag=Cyc_Lexing_Error,((_tmp459.f1=((_tmp458="some action didn't return!",_tag_dyneither(_tmp458,sizeof(char),27))),_tmp459)))),_tmp457)))));};}
# 365
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL4E:
# 368 "buildlib.cyl"
 return 0;case 1: _LL4F:
# 370
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL50:
# 377 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp45C;void*_tmp45B;(_tmp45B=0,Cyc_log(((_tmp45C="Warning: declaration of malloc sidestepped\n",_tag_dyneither(_tmp45C,sizeof(char),44))),_tag_dyneither(_tmp45B,sizeof(void*),0)));}
return 1;case 3: _LL51:
# 383 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp45F;void*_tmp45E;(_tmp45E=0,Cyc_log(((_tmp45F="Warning: declaration of malloc sidestepped\n",_tag_dyneither(_tmp45F,sizeof(char),44))),_tag_dyneither(_tmp45E,sizeof(void*),0)));}
return 1;case 4: _LL52:
# 389 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp462;void*_tmp461;(_tmp461=0,Cyc_log(((_tmp462="Warning: declaration of calloc sidestepped\n",_tag_dyneither(_tmp462,sizeof(char),44))),_tag_dyneither(_tmp461,sizeof(void*),0)));}
return 1;case 5: _LL53:
# 395 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp465;void*_tmp464;(_tmp464=0,Cyc_log(((_tmp465="Warning: declaration of calloc sidestepped\n",_tag_dyneither(_tmp465,sizeof(char),44))),_tag_dyneither(_tmp464,sizeof(void*),0)));}
return 1;case 6: _LL54:
# 399
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp468;void*_tmp467;(_tmp467=0,Cyc_log(((_tmp468="Warning: use of region sidestepped\n",_tag_dyneither(_tmp468,sizeof(char),36))),_tag_dyneither(_tmp467,sizeof(void*),0)));}
return 1;case 7: _LL55:
# 403
{const char*_tmp46B;void*_tmp46A;(_tmp46A=0,Cyc_log(((_tmp46B="Warning: use of __extension__ deleted\n",_tag_dyneither(_tmp46B,sizeof(char),39))),_tag_dyneither(_tmp46A,sizeof(void*),0)));}
return 1;case 8: _LL56:
# 408 "buildlib.cyl"
{const char*_tmp46E;void*_tmp46D;(_tmp46D=0,Cyc_log(((_tmp46E="Warning: use of mode HI deleted\n",_tag_dyneither(_tmp46E,sizeof(char),33))),_tag_dyneither(_tmp46D,sizeof(void*),0)));}
return 1;case 9: _LL57:
# 411
{const char*_tmp471;void*_tmp470;(_tmp470=0,Cyc_log(((_tmp471="Warning: use of mode SI deleted\n",_tag_dyneither(_tmp471,sizeof(char),33))),_tag_dyneither(_tmp470,sizeof(void*),0)));}
return 1;case 10: _LL58:
# 414
{const char*_tmp474;void*_tmp473;(_tmp473=0,Cyc_log(((_tmp474="Warning: use of mode QI deleted\n",_tag_dyneither(_tmp474,sizeof(char),33))),_tag_dyneither(_tmp473,sizeof(void*),0)));}
return 1;case 11: _LL59:
# 417
{const char*_tmp477;void*_tmp476;(_tmp476=0,Cyc_log(((_tmp477="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp477,sizeof(char),33))),_tag_dyneither(_tmp476,sizeof(void*),0)));}
return 1;case 12: _LL5A:
# 420
{const char*_tmp47A;void*_tmp479;(_tmp479=0,Cyc_log(((_tmp47A="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp47A,sizeof(char),33))),_tag_dyneither(_tmp479,sizeof(void*),0)));}
return 1;case 13: _LL5B:
# 423
{const char*_tmp47D;void*_tmp47C;(_tmp47C=0,Cyc_log(((_tmp47D="Warning: use of mode word deleted\n",_tag_dyneither(_tmp47D,sizeof(char),35))),_tag_dyneither(_tmp47C,sizeof(void*),0)));}
return 1;case 14: _LL5C:
# 426
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15: _LL5D:
# 428
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LL5E:
# 430
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17: _LL5F:
# 432
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LL60:
# 437 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19: _LL61:
# 439
 return 1;case 20: _LL62:
# 441
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp480;void*_tmp47F;(_tmp47F=0,Cyc_log(((_tmp480="Warning: replacing use of __asm__ with 0\n",_tag_dyneither(_tmp480,sizeof(char),42))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}
return 1;case 21: _LL63:
# 447
 Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default: _LL64:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{
# 451
struct Cyc_Lexing_Error_exn_struct _tmp486;const char*_tmp485;struct Cyc_Lexing_Error_exn_struct*_tmp484;(int)_throw((void*)((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484[0]=((_tmp486.tag=Cyc_Lexing_Error,((_tmp486.f1=((_tmp485="some action didn't return!",_tag_dyneither(_tmp485,sizeof(char),27))),_tmp486)))),_tmp484)))));};}
# 453
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL66:
# 451 "buildlib.cyl"
 return 0;case 1: _LL67:
# 453
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL68:
# 455
{const char*_tmp489;void*_tmp488;(_tmp488=0,Cyc_log(((_tmp489="Warning: unclosed string\n",_tag_dyneither(_tmp489,sizeof(char),26))),_tag_dyneither(_tmp488,sizeof(void*),0)));}
{const char*_tmp48D;void*_tmp48C[1];struct Cyc_String_pa_PrintArg_struct _tmp48B;(_tmp48B.tag=0,((_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp48C[0]=& _tmp48B,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp48D="%s",_tag_dyneither(_tmp48D,sizeof(char),3))),_tag_dyneither(_tmp48C,sizeof(void*),1)))))));}return 1;case 3: _LL69:
# 458
{const char*_tmp491;void*_tmp490[1];struct Cyc_String_pa_PrintArg_struct _tmp48F;(_tmp48F.tag=0,((_tmp48F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp490[0]=& _tmp48F,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp491="%s",_tag_dyneither(_tmp491,sizeof(char),3))),_tag_dyneither(_tmp490,sizeof(void*),1)))))));}return 1;case 4: _LL6A:
# 460
{const char*_tmp495;void*_tmp494[1];struct Cyc_String_pa_PrintArg_struct _tmp493;(_tmp493.tag=0,((_tmp493.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp494[0]=& _tmp493,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp495="%s",_tag_dyneither(_tmp495,sizeof(char),3))),_tag_dyneither(_tmp494,sizeof(void*),1)))))));}return 1;case 5: _LL6B:
# 462
{const char*_tmp499;void*_tmp498[1];struct Cyc_String_pa_PrintArg_struct _tmp497;(_tmp497.tag=0,((_tmp497.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp498[0]=& _tmp497,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp499="%s",_tag_dyneither(_tmp499,sizeof(char),3))),_tag_dyneither(_tmp498,sizeof(void*),1)))))));}return 1;case 6: _LL6C:
# 464
{const char*_tmp49D;void*_tmp49C[1];struct Cyc_String_pa_PrintArg_struct _tmp49B;(_tmp49B.tag=0,((_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp49C[0]=& _tmp49B,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp49D="%s",_tag_dyneither(_tmp49D,sizeof(char),3))),_tag_dyneither(_tmp49C,sizeof(void*),1)))))));}return 1;case 7: _LL6D:
# 466
{const char*_tmp4A1;void*_tmp4A0[1];struct Cyc_String_pa_PrintArg_struct _tmp49F;(_tmp49F.tag=0,((_tmp49F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp4A0[0]=& _tmp49F,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp4A1="%s",_tag_dyneither(_tmp4A1,sizeof(char),3))),_tag_dyneither(_tmp4A0,sizeof(void*),1)))))));}return 1;case 8: _LL6E:
# 468
{const char*_tmp4A5;void*_tmp4A4[1];struct Cyc_String_pa_PrintArg_struct _tmp4A3;(_tmp4A3.tag=0,((_tmp4A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp4A4[0]=& _tmp4A3,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp4A5="%s",_tag_dyneither(_tmp4A5,sizeof(char),3))),_tag_dyneither(_tmp4A4,sizeof(void*),1)))))));}return 1;default: _LL6F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{
# 472
struct Cyc_Lexing_Error_exn_struct _tmp4AB;const char*_tmp4AA;struct Cyc_Lexing_Error_exn_struct*_tmp4A9;(int)_throw((void*)((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4AB.tag=Cyc_Lexing_Error,((_tmp4AB.f1=((_tmp4AA="some action didn't return!",_tag_dyneither(_tmp4AA,sizeof(char),27))),_tmp4AB)))),_tmp4A9)))));};}
# 474
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL71:
# 478 "buildlib.cyl"
 return 0;case 1: _LL72:
# 480
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2: _LL73:
# 484
 ++ Cyc_parens_to_match;
return 1;case 3: _LL74:
# 487
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4: _LL75:
# 490
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5: _LL76:
# 493
 return 1;case 6: _LL77:
# 495
 return 1;default: _LL78:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}{
# 499
struct Cyc_Lexing_Error_exn_struct _tmp4B1;const char*_tmp4B0;struct Cyc_Lexing_Error_exn_struct*_tmp4AF;(int)_throw((void*)((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF[0]=((_tmp4B1.tag=Cyc_Lexing_Error,((_tmp4B1.f1=((_tmp4B0="some action didn't return!",_tag_dyneither(_tmp4B0,sizeof(char),27))),_tmp4B1)))),_tmp4AF)))));};}
# 501
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL7A:
# 499 "buildlib.cyl"
{const char*_tmp4B4;void*_tmp4B3;(_tmp4B3=0,Cyc_log(((_tmp4B4="Warning: unclosed string\n",_tag_dyneither(_tmp4B4,sizeof(char),26))),_tag_dyneither(_tmp4B3,sizeof(void*),0)));}return 0;case 1: _LL7B:
# 501
 return 0;case 2: _LL7C:
# 503
{const char*_tmp4B7;void*_tmp4B6;(_tmp4B6=0,Cyc_log(((_tmp4B7="Warning: unclosed string\n",_tag_dyneither(_tmp4B7,sizeof(char),26))),_tag_dyneither(_tmp4B6,sizeof(void*),0)));}return 1;case 3: _LL7D:
# 505
 return 1;case 4: _LL7E:
# 507
 return 1;case 5: _LL7F:
# 509
 return 1;case 6: _LL80:
# 511
 return 1;case 7: _LL81:
# 513
 return 1;case 8: _LL82:
# 515
 return 1;default: _LL83:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}{
# 519
struct Cyc_Lexing_Error_exn_struct _tmp4BD;const char*_tmp4BC;struct Cyc_Lexing_Error_exn_struct*_tmp4BB;(int)_throw((void*)((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB[0]=((_tmp4BD.tag=Cyc_Lexing_Error,((_tmp4BD.f1=((_tmp4BC="some action didn't return!",_tag_dyneither(_tmp4BC,sizeof(char),27))),_tmp4BD)))),_tmp4BB)))));};}
# 521
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL85:
# 519 "buildlib.cyl"
{const char*_tmp4C0;void*_tmp4BF;(_tmp4BF=0,Cyc_log(((_tmp4C0="Warning: unclosed comment\n",_tag_dyneither(_tmp4C0,sizeof(char),27))),_tag_dyneither(_tmp4BF,sizeof(void*),0)));}return 0;case 1: _LL86:
# 521
 return 0;case 2: _LL87:
# 523
 return 1;default: _LL88:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}{
# 527
struct Cyc_Lexing_Error_exn_struct _tmp4C6;const char*_tmp4C5;struct Cyc_Lexing_Error_exn_struct*_tmp4C4;(int)_throw((void*)((_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4[0]=((_tmp4C6.tag=Cyc_Lexing_Error,((_tmp4C6.f1=((_tmp4C5="some action didn't return!",_tag_dyneither(_tmp4C5,sizeof(char),27))),_tmp4C6)))),_tmp4C4)))));};}
# 529
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple21*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL8A:
# 531 "buildlib.cyl"
{const char*_tmp4C7;Cyc_current_line=((_tmp4C7="#define ",_tag_dyneither(_tmp4C7,sizeof(char),9)));}
Cyc_suck_macroname(lexbuf);{
struct _tuple21*_tmp4C8;return(_tmp4C8=_cycalloc(sizeof(*_tmp4C8)),((_tmp4C8->f1=Cyc_current_line,((_tmp4C8->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp4C8)))));};case 1: _LL8B:
# 535
 return Cyc_suck_line(lexbuf);case 2: _LL8C:
# 537
 return 0;default: _LL8D:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}{
# 541
struct Cyc_Lexing_Error_exn_struct _tmp4CE;const char*_tmp4CD;struct Cyc_Lexing_Error_exn_struct*_tmp4CC;(int)_throw((void*)((_tmp4CC=_cycalloc(sizeof(*_tmp4CC)),((_tmp4CC[0]=((_tmp4CE.tag=Cyc_Lexing_Error,((_tmp4CE.f1=((_tmp4CD="some action didn't return!",_tag_dyneither(_tmp4CD,sizeof(char),27))),_tmp4CE)))),_tmp4CC)))));};}
# 543
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL8F:
# 541 "buildlib.cyl"
{struct _dyneither_ptr*_tmp4CF;Cyc_current_source=((_tmp4CF=_cycalloc(sizeof(*_tmp4CF)),((_tmp4CF[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp4CF))));}
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL90:
# 545
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{
# 548
struct Cyc_Lexing_Error_exn_struct _tmp4D5;const char*_tmp4D4;struct Cyc_Lexing_Error_exn_struct*_tmp4D3;(int)_throw((void*)((_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3[0]=((_tmp4D5.tag=Cyc_Lexing_Error,((_tmp4D5.f1=((_tmp4D4="some action didn't return!",_tag_dyneither(_tmp4D4,sizeof(char),27))),_tmp4D5)))),_tmp4D3)))));};}
# 550
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL92:
# 548 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL93:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}{
# 552
struct Cyc_Lexing_Error_exn_struct _tmp4DB;const char*_tmp4DA;struct Cyc_Lexing_Error_exn_struct*_tmp4D9;(int)_throw((void*)((_tmp4D9=_cycalloc(sizeof(*_tmp4D9)),((_tmp4D9[0]=((_tmp4DB.tag=Cyc_Lexing_Error,((_tmp4DB.f1=((_tmp4DA="some action didn't return!",_tag_dyneither(_tmp4DA,sizeof(char),27))),_tmp4DB)))),_tmp4D9)))));};}
# 554
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple23*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL95:
# 555 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL96:
# 557
 Cyc_current_headerfile=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));
Cyc_current_symbols=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{
struct _tuple23*_tmp4DC;return(_tmp4DC=_cycalloc(sizeof(*_tmp4DC)),((_tmp4DC->f1=Cyc_current_headerfile,((_tmp4DC->f2=Cyc_current_symbols,((_tmp4DC->f3=Cyc_current_omit_symbols,((_tmp4DC->f4=Cyc_current_hstubs,((_tmp4DC->f5=Cyc_current_cstubs,((_tmp4DC->f6=Cyc_current_cycstubs,_tmp4DC)))))))))))));};case 2: _LL97:
# 577
 return Cyc_spec(lexbuf);case 3: _LL98:
# 579
 return 0;case 4: _LL99:
# 581
{const char*_tmp4E0;void*_tmp4DF[1];struct Cyc_Int_pa_PrintArg_struct _tmp4DE;(_tmp4DE.tag=1,((_tmp4DE.f1=(unsigned long)((int)
# 583
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4DF[0]=& _tmp4DE,Cyc_fprintf(Cyc_stderr,((_tmp4E0="Error in .cys file: expected header file name, found '%c' instead\n",_tag_dyneither(_tmp4E0,sizeof(char),67))),_tag_dyneither(_tmp4DF,sizeof(void*),1)))))));}
return 0;default: _LL9A:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}{
# 588
struct Cyc_Lexing_Error_exn_struct _tmp4E6;const char*_tmp4E5;struct Cyc_Lexing_Error_exn_struct*_tmp4E4;(int)_throw((void*)((_tmp4E4=_cycalloc(sizeof(*_tmp4E4)),((_tmp4E4[0]=((_tmp4E6.tag=Cyc_Lexing_Error,((_tmp4E6.f1=((_tmp4E5="some action didn't return!",_tag_dyneither(_tmp4E5,sizeof(char),27))),_tmp4E6)))),_tmp4E4)))));};}
# 590
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL9C:
# 588 "buildlib.cyl"
 return 0;case 1: _LL9D:
# 590
 return 0;case 2: _LL9E:
# 592
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9F:
# 597
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;case 4: _LLA0:
# 602
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp4E7;struct _tuple22*x=(_tmp4E7=_cycalloc(sizeof(*_tmp4E7)),((_tmp4E7->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4E7->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4E7)))));
{struct Cyc_List_List*_tmp4E8;Cyc_current_hstubs=((_tmp4E8=_cycalloc(sizeof(*_tmp4E8)),((_tmp4E8->hd=x,((_tmp4E8->tl=Cyc_current_hstubs,_tmp4E8))))));}
return 1;};case 5: _LLA1: {
# 610
struct _dyneither_ptr _tmp83=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp4E9;_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)Cyc_strlen(((_tmp4E9="hstub",_tag_dyneither(_tmp4E9,sizeof(char),6)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp83,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp83;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp85=Cyc_substring((struct _dyneither_ptr)_tmp83,0,(unsigned long)((t.curr - _tmp83.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp4EA;struct _tuple22*x=(_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA->f1=(struct _dyneither_ptr)_tmp85,((_tmp4EA->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4EA)))));
{struct Cyc_List_List*_tmp4EB;Cyc_current_hstubs=((_tmp4EB=_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB->hd=x,((_tmp4EB->tl=Cyc_current_hstubs,_tmp4EB))))));}
return 1;};};};}case 6: _LLA2:
# 624
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp4EC;struct _tuple22*x=(_tmp4EC=_cycalloc(sizeof(*_tmp4EC)),((_tmp4EC->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4EC->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4EC)))));
{struct Cyc_List_List*_tmp4ED;Cyc_current_cstubs=((_tmp4ED=_cycalloc(sizeof(*_tmp4ED)),((_tmp4ED->hd=x,((_tmp4ED->tl=Cyc_current_cstubs,_tmp4ED))))));}
return 1;};case 7: _LLA3: {
# 632
struct _dyneither_ptr _tmp8A=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp4EE;_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),(int)Cyc_strlen(((_tmp4EE="cstub",_tag_dyneither(_tmp4EE,sizeof(char),6)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp8A,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp8A;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp8C=Cyc_substring((struct _dyneither_ptr)_tmp8A,0,(unsigned long)((t.curr - _tmp8A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp4EF;struct _tuple22*x=(_tmp4EF=_cycalloc(sizeof(*_tmp4EF)),((_tmp4EF->f1=(struct _dyneither_ptr)_tmp8C,((_tmp4EF->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4EF)))));
{struct Cyc_List_List*_tmp4F0;Cyc_current_cstubs=((_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((_tmp4F0->hd=x,((_tmp4F0->tl=Cyc_current_cstubs,_tmp4F0))))));}
return 1;};};};}case 8: _LLA4:
# 646
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp4F1;struct _tuple22*x=(_tmp4F1=_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4F1->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4F1)))));
{struct Cyc_List_List*_tmp4F2;Cyc_current_cycstubs=((_tmp4F2=_cycalloc(sizeof(*_tmp4F2)),((_tmp4F2->hd=x,((_tmp4F2->tl=Cyc_current_cycstubs,_tmp4F2))))));}
return 1;};case 9: _LLA5: {
# 654
struct _dyneither_ptr _tmp91=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp4F3;_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),(int)Cyc_strlen(((_tmp4F3="cycstub",_tag_dyneither(_tmp4F3,sizeof(char),8)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp91,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp91;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp93=Cyc_substring((struct _dyneither_ptr)_tmp91,0,(unsigned long)((t.curr - _tmp91.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp4F4;struct _tuple22*x=(_tmp4F4=_cycalloc(sizeof(*_tmp4F4)),((_tmp4F4->f1=(struct _dyneither_ptr)_tmp93,((_tmp4F4->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4F4)))));
{struct Cyc_List_List*_tmp4F5;Cyc_current_cycstubs=((_tmp4F5=_cycalloc(sizeof(*_tmp4F5)),((_tmp4F5->hd=x,((_tmp4F5->tl=Cyc_current_cycstubs,_tmp4F5))))));}
return 1;};};};}case 10: _LLA6:
# 668
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*_tmp4F6;struct _dyneither_ptr*x=(_tmp4F6=_cycalloc(sizeof(*_tmp4F6)),((_tmp4F6[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4F6)));
{struct Cyc_List_List*_tmp4F7;Cyc_current_cpp=((_tmp4F7=_cycalloc(sizeof(*_tmp4F7)),((_tmp4F7->hd=x,((_tmp4F7->tl=Cyc_current_cpp,_tmp4F7))))));}
return 1;};case 11: _LLA7:
# 675
 return 1;case 12: _LLA8:
# 677
 return 1;case 13: _LLA9:
# 679
{const char*_tmp4FB;void*_tmp4FA[1];struct Cyc_Int_pa_PrintArg_struct _tmp4F9;(_tmp4F9.tag=1,((_tmp4F9.f1=(unsigned long)((int)
# 681
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4FA[0]=& _tmp4F9,Cyc_fprintf(Cyc_stderr,((_tmp4FB="Error in .cys file: expected command, found '%c' instead\n",_tag_dyneither(_tmp4FB,sizeof(char),58))),_tag_dyneither(_tmp4FA,sizeof(void*),1)))))));}
return 0;default: _LLAA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}{
# 686
struct Cyc_Lexing_Error_exn_struct _tmp501;const char*_tmp500;struct Cyc_Lexing_Error_exn_struct*_tmp4FF;(int)_throw((void*)((_tmp4FF=_cycalloc(sizeof(*_tmp4FF)),((_tmp4FF[0]=((_tmp501.tag=Cyc_Lexing_Error,((_tmp501.f1=((_tmp500="some action didn't return!",_tag_dyneither(_tmp500,sizeof(char),27))),_tmp501)))),_tmp4FF)))));};}
# 688
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLAC:
# 691 "buildlib.cyl"
{struct _dyneither_ptr*_tmp504;struct Cyc_List_List*_tmp503;Cyc_snarfed_symbols=((_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503->hd=((_tmp504=_cycalloc(sizeof(*_tmp504)),((_tmp504[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp504)))),((_tmp503->tl=Cyc_snarfed_symbols,_tmp503))))));}
return 1;case 1: _LLAD:
# 694
 return 1;case 2: _LLAE:
# 696
 return 0;case 3: _LLAF:
# 698
{const char*_tmp507;void*_tmp506;(_tmp506=0,Cyc_fprintf(Cyc_stderr,((_tmp507="Error in .cys file: unexpected end-of-file\n",_tag_dyneither(_tmp507,sizeof(char),44))),_tag_dyneither(_tmp506,sizeof(void*),0)));}
# 700
return 0;case 4: _LLB0:
# 702
{const char*_tmp50B;void*_tmp50A[1];struct Cyc_Int_pa_PrintArg_struct _tmp509;(_tmp509.tag=1,((_tmp509.f1=(unsigned long)((int)
# 704
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp50A[0]=& _tmp509,Cyc_fprintf(Cyc_stderr,((_tmp50B="Error in .cys file: expected symbol, found '%c' instead\n",_tag_dyneither(_tmp50B,sizeof(char),57))),_tag_dyneither(_tmp50A,sizeof(void*),1)))))));}
return 0;default: _LLB1:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}{
# 709
struct Cyc_Lexing_Error_exn_struct _tmp511;const char*_tmp510;struct Cyc_Lexing_Error_exn_struct*_tmp50F;(int)_throw((void*)((_tmp50F=_cycalloc(sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp511.tag=Cyc_Lexing_Error,((_tmp511.f1=((_tmp510="some action didn't return!",_tag_dyneither(_tmp510,sizeof(char),27))),_tmp511)))),_tmp50F)))));};}
# 711
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLB3:
# 715 "buildlib.cyl"
{const char*_tmp514;void*_tmp513;(_tmp513=0,Cyc_log(((_tmp514="Warning: unclosed brace\n",_tag_dyneither(_tmp514,sizeof(char),25))),_tag_dyneither(_tmp513,sizeof(void*),0)));}return 0;case 1: _LLB4:
# 717
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2: _LLB5:
# 722
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3: _LLB6:
# 726
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLB7:
# 730
{const char*_tmp515;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp515="/*",_tag_dyneither(_tmp515,sizeof(char),3))));}
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LLB8:
# 734
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LLB9:
# 737
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLBA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}{
# 742
struct Cyc_Lexing_Error_exn_struct _tmp51B;const char*_tmp51A;struct Cyc_Lexing_Error_exn_struct*_tmp519;(int)_throw((void*)((_tmp519=_cycalloc(sizeof(*_tmp519)),((_tmp519[0]=((_tmp51B.tag=Cyc_Lexing_Error,((_tmp51B.f1=((_tmp51A="some action didn't return!",_tag_dyneither(_tmp51A,sizeof(char),27))),_tmp51B)))),_tmp519)))));};}
# 744
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLBC:
# 742 "buildlib.cyl"
{const char*_tmp51E;void*_tmp51D;(_tmp51D=0,Cyc_log(((_tmp51E="Warning: unclosed string\n",_tag_dyneither(_tmp51E,sizeof(char),26))),_tag_dyneither(_tmp51D,sizeof(void*),0)));}return 0;case 1: _LLBD:
# 744
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLBE:
# 746
{const char*_tmp521;void*_tmp520;(_tmp520=0,Cyc_log(((_tmp521="Warning: unclosed string\n",_tag_dyneither(_tmp521,sizeof(char),26))),_tag_dyneither(_tmp520,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LLBF:
# 750
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 4: _LLC0:
# 753
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 5: _LLC1:
# 756
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LLC2:
# 759
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 7: _LLC3:
# 762
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 8: _LLC4:
# 765
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LLC5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}{
# 770
struct Cyc_Lexing_Error_exn_struct _tmp527;const char*_tmp526;struct Cyc_Lexing_Error_exn_struct*_tmp525;(int)_throw((void*)((_tmp525=_cycalloc(sizeof(*_tmp525)),((_tmp525[0]=((_tmp527.tag=Cyc_Lexing_Error,((_tmp527.f1=((_tmp526="some action didn't return!",_tag_dyneither(_tmp526,sizeof(char),27))),_tmp527)))),_tmp525)))));};}
# 772
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC7:
# 770 "buildlib.cyl"
{const char*_tmp52A;void*_tmp529;(_tmp529=0,Cyc_log(((_tmp52A="Warning: unclosed comment\n",_tag_dyneither(_tmp52A,sizeof(char),27))),_tag_dyneither(_tmp529,sizeof(void*),0)));}return 0;case 1: _LLC8:
# 772
{const char*_tmp52B;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp52B="*/",_tag_dyneither(_tmp52B,sizeof(char),3))));}return 0;case 2: _LLC9:
# 774
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LLCA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}{
# 779
struct Cyc_Lexing_Error_exn_struct _tmp531;const char*_tmp530;struct Cyc_Lexing_Error_exn_struct*_tmp52F;(int)_throw((void*)((_tmp52F=_cycalloc(sizeof(*_tmp52F)),((_tmp52F[0]=((_tmp531.tag=Cyc_Lexing_Error,((_tmp531.f1=((_tmp530="some action didn't return!",_tag_dyneither(_tmp530,sizeof(char),27))),_tmp531)))),_tmp52F)))));};}
# 781
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 784 "buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 787
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpBB=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmpBC=_tmpBB;struct _dyneither_ptr*_tmpBE;struct Cyc_List_List*_tmpC0;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_List_List*_tmpDE;void*_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;int _tmpE3;struct Cyc_Absyn_Exp*_tmpE4;void**_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE8;void*_tmpEA;void*_tmpEC;struct Cyc_Absyn_Exp*_tmpEE;struct _dyneither_ptr*_tmpEF;struct Cyc_Absyn_Exp*_tmpF1;struct _dyneither_ptr*_tmpF2;void*_tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF8;_LLCD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpBD->tag != 1)goto _LLCF;else{_tmpBE=(_tmpBD->f1)->f2;}}_LLCE:
# 791
 Cyc_add_target(_tmpBE);
return;_LLCF: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpBF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpBF->tag != 2)goto _LLD1;else{_tmpC0=_tmpBF->f2;}}_LLD0:
# 794
 for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC0->hd,dep);}
# 797
return;_LLD1: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpC1->tag != 22)goto _LLD3;else{_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;}}_LLD2:
# 799
 _tmpC5=_tmpC2;_tmpC6=_tmpC3;goto _LLD4;_LLD3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC4=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpC4->tag != 8)goto _LLD5;else{_tmpC5=_tmpC4->f1;_tmpC6=_tmpC4->f2;}}_LLD4:
# 801
 _tmpC8=_tmpC5;_tmpC9=_tmpC6;goto _LLD6;_LLD5: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC7=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpC7->tag != 3)goto _LLD7;else{_tmpC8=_tmpC7->f1;_tmpC9=_tmpC7->f3;}}_LLD6:
# 803
 Cyc_scan_exp(_tmpC8,dep);
Cyc_scan_exp(_tmpC9,dep);
return;_LLD7: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpCA->tag != 19)goto _LLD9;else{_tmpCB=_tmpCA->f1;}}_LLD8:
# 807
 _tmpCD=_tmpCB;goto _LLDA;_LLD9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCC=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpCC->tag != 17)goto _LLDB;else{_tmpCD=_tmpCC->f1;}}_LLDA:
# 809
 _tmpCF=_tmpCD;goto _LLDC;_LLDB: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpCE=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpCE->tag != 14)goto _LLDD;else{_tmpCF=_tmpCE->f1;}}_LLDC:
# 811
 _tmpD1=_tmpCF;goto _LLDE;_LLDD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpD0=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpD0->tag != 4)goto _LLDF;else{_tmpD1=_tmpD0->f1;}}_LLDE:
# 813
 Cyc_scan_exp(_tmpD1,dep);
return;_LLDF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD2=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpD2->tag != 5)goto _LLE1;else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD2->f2;_tmpD5=_tmpD2->f3;}}_LLE0:
# 816
 Cyc_scan_exp(_tmpD3,dep);
Cyc_scan_exp(_tmpD4,dep);
Cyc_scan_exp(_tmpD5,dep);
return;_LLE1: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpD6=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpD6->tag != 6)goto _LLE3;else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD6->f2;}}_LLE2:
 _tmpDA=_tmpD7;_tmpDB=_tmpD8;goto _LLE4;_LLE3: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpD9=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpD9->tag != 7)goto _LLE5;else{_tmpDA=_tmpD9->f1;_tmpDB=_tmpD9->f2;}}_LLE4:
# 822
 Cyc_scan_exp(_tmpDA,dep);
Cyc_scan_exp(_tmpDB,dep);
return;_LLE5: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDC=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpDC->tag != 9)goto _LLE7;else{_tmpDD=_tmpDC->f1;_tmpDE=_tmpDC->f2;}}_LLE6:
# 826
 Cyc_scan_exp(_tmpDD,dep);
for(0;_tmpDE != 0;_tmpDE=_tmpDE->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDE->hd,dep);}
# 830
return;_LLE7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpDF->tag != 13)goto _LLE9;else{_tmpE0=(void*)_tmpDF->f1;_tmpE1=_tmpDF->f2;}}_LLE8:
# 832
 Cyc_scan_type(_tmpE0,dep);
Cyc_scan_exp(_tmpE1,dep);
return;_LLE9: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpE2->tag != 33)goto _LLEB;else{_tmpE3=(_tmpE2->f1).is_calloc;_tmpE4=(_tmpE2->f1).rgn;_tmpE5=(_tmpE2->f1).elt_type;_tmpE6=(_tmpE2->f1).num_elts;}}_LLEA:
# 836
 if(_tmpE4 != 0)Cyc_scan_exp(_tmpE4,dep);
if(_tmpE5 != 0)Cyc_scan_type(*_tmpE5,dep);
Cyc_scan_exp(_tmpE6,dep);
return;_LLEB: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE7=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpE7->tag != 37)goto _LLED;else{_tmpE8=_tmpE7->f1;}}_LLEC:
# 841
 Cyc_scan_exp(_tmpE8,dep);return;_LLED: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpE9=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpE9->tag != 38)goto _LLEF;else{_tmpEA=(void*)_tmpE9->f1;}}_LLEE:
 _tmpEC=_tmpEA;goto _LLF0;_LLEF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpEB->tag != 16)goto _LLF1;else{_tmpEC=(void*)_tmpEB->f1;}}_LLF0:
# 844
 Cyc_scan_type(_tmpEC,dep);
return;_LLF1: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpED->tag != 20)goto _LLF3;else{_tmpEE=_tmpED->f1;_tmpEF=_tmpED->f2;}}_LLF2:
# 847
 _tmpF1=_tmpEE;_tmpF2=_tmpEF;goto _LLF4;_LLF3: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpF0->tag != 21)goto _LLF5;else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}}_LLF4:
# 849
 Cyc_scan_exp(_tmpF1,dep);
Cyc_add_target(_tmpF2);
return;_LLF5: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpF3->tag != 18)goto _LLF7;else{_tmpF4=(void*)_tmpF3->f1;_tmpF5=_tmpF3->f2;}}_LLF6:
# 853
 Cyc_scan_type(_tmpF4,dep);
# 855
{void*_tmp10A=(void*)((struct Cyc_List_List*)_check_null(_tmpF5))->hd;void*_tmp10B=_tmp10A;struct _dyneither_ptr*_tmp10D;_LL11E: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp10C=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp10B;if(_tmp10C->tag != 0)goto _LL120;else{_tmp10D=_tmp10C->f1;}}_LL11F:
 Cyc_add_target(_tmp10D);goto _LL11D;_LL120: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp10E=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp10B;if(_tmp10E->tag != 1)goto _LL11D;}_LL121:
 goto _LL11D;_LL11D:;}
# 859
return;_LLF7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpF6->tag != 0)goto _LLF9;}_LLF8:
# 861
 return;_LLF9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpF7->tag != 35)goto _LLFB;else{_tmpF8=_tmpF7->f2;}}_LLFA:
# 863
 for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){
struct _tuple24*_tmp10F=(struct _tuple24*)_tmpF8->hd;struct Cyc_Absyn_Exp*_tmp111;struct _tuple24*_tmp110=_tmp10F;_tmp111=_tmp110->f2;
Cyc_scan_exp(_tmp111,dep);}
# 867
return;_LLFB: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpF9->tag != 39)goto _LLFD;}_LLFC:
 return;_LLFD: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmpFA=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpFA->tag != 34)goto _LLFF;}_LLFE:
# 870
{const char*_tmp534;void*_tmp533;(_tmp533=0,Cyc_fprintf(Cyc_stderr,((_tmp534="Error: unexpected Swap_e\n",_tag_dyneither(_tmp534,sizeof(char),26))),_tag_dyneither(_tmp533,sizeof(void*),0)));}
exit(1);return;_LLFF: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpFB->tag != 36)goto _LL101;}_LL100:
# 873
{const char*_tmp537;void*_tmp536;(_tmp536=0,Cyc_fprintf(Cyc_stderr,((_tmp537="Error: unexpected Stmt_e\n",_tag_dyneither(_tmp537,sizeof(char),26))),_tag_dyneither(_tmp536,sizeof(void*),0)));}
exit(1);return;_LL101: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpFC->tag != 10)goto _LL103;}_LL102:
# 876
{const char*_tmp53A;void*_tmp539;(_tmp539=0,Cyc_fprintf(Cyc_stderr,((_tmp53A="Error: unexpected Throw_e\n",_tag_dyneither(_tmp53A,sizeof(char),27))),_tag_dyneither(_tmp539,sizeof(void*),0)));}
exit(1);return;_LL103: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpFD->tag != 11)goto _LL105;}_LL104:
# 879
{const char*_tmp53D;void*_tmp53C;(_tmp53C=0,Cyc_fprintf(Cyc_stderr,((_tmp53D="Error: unexpected NoInstantiate_e\n",_tag_dyneither(_tmp53D,sizeof(char),35))),_tag_dyneither(_tmp53C,sizeof(void*),0)));}
exit(1);return;_LL105: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpFE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpFE->tag != 12)goto _LL107;}_LL106:
# 882
{const char*_tmp540;void*_tmp53F;(_tmp53F=0,Cyc_fprintf(Cyc_stderr,((_tmp540="Error: unexpected Instantiate_e\n",_tag_dyneither(_tmp540,sizeof(char),33))),_tag_dyneither(_tmp53F,sizeof(void*),0)));}
exit(1);return;_LL107: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmpFF->tag != 15)goto _LL109;}_LL108:
# 885
{const char*_tmp543;void*_tmp542;(_tmp542=0,Cyc_fprintf(Cyc_stderr,((_tmp543="Error: unexpected New_e\n",_tag_dyneither(_tmp543,sizeof(char),25))),_tag_dyneither(_tmp542,sizeof(void*),0)));}
exit(1);return;_LL109: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp100->tag != 23)goto _LL10B;}_LL10A:
# 888
{const char*_tmp546;void*_tmp545;(_tmp545=0,Cyc_fprintf(Cyc_stderr,((_tmp546="Error: unexpected Tuple_e\n",_tag_dyneither(_tmp546,sizeof(char),27))),_tag_dyneither(_tmp545,sizeof(void*),0)));}
exit(1);return;_LL10B: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp101->tag != 24)goto _LL10D;}_LL10C:
# 891
{const char*_tmp549;void*_tmp548;(_tmp548=0,Cyc_fprintf(Cyc_stderr,((_tmp549="Error: unexpected CompoundLit_e\n",_tag_dyneither(_tmp549,sizeof(char),33))),_tag_dyneither(_tmp548,sizeof(void*),0)));}
exit(1);return;_LL10D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp102->tag != 25)goto _LL10F;}_LL10E:
# 894
{const char*_tmp54C;void*_tmp54B;(_tmp54B=0,Cyc_fprintf(Cyc_stderr,((_tmp54C="Error: unexpected Array_e\n",_tag_dyneither(_tmp54C,sizeof(char),27))),_tag_dyneither(_tmp54B,sizeof(void*),0)));}
exit(1);return;_LL10F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp103->tag != 26)goto _LL111;}_LL110:
# 897
{const char*_tmp54F;void*_tmp54E;(_tmp54E=0,Cyc_fprintf(Cyc_stderr,((_tmp54F="Error: unexpected Comprehension_e\n",_tag_dyneither(_tmp54F,sizeof(char),35))),_tag_dyneither(_tmp54E,sizeof(void*),0)));}
exit(1);return;_LL111: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp104->tag != 27)goto _LL113;}_LL112:
# 900
{const char*_tmp552;void*_tmp551;(_tmp551=0,Cyc_fprintf(Cyc_stderr,((_tmp552="Error: unexpected ComprehensionNoinit_e\n",_tag_dyneither(_tmp552,sizeof(char),41))),_tag_dyneither(_tmp551,sizeof(void*),0)));}
exit(1);return;_LL113: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp105=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp105->tag != 28)goto _LL115;}_LL114:
# 903
{const char*_tmp555;void*_tmp554;(_tmp554=0,Cyc_fprintf(Cyc_stderr,((_tmp555="Error: unexpected Aggregate_e\n",_tag_dyneither(_tmp555,sizeof(char),31))),_tag_dyneither(_tmp554,sizeof(void*),0)));}
exit(1);return;_LL115: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp106->tag != 29)goto _LL117;}_LL116:
# 906
{const char*_tmp558;void*_tmp557;(_tmp557=0,Cyc_fprintf(Cyc_stderr,((_tmp558="Error: unexpected AnonStruct_e\n",_tag_dyneither(_tmp558,sizeof(char),32))),_tag_dyneither(_tmp557,sizeof(void*),0)));}
exit(1);return;_LL117: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp107=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp107->tag != 30)goto _LL119;}_LL118:
# 909
{const char*_tmp55B;void*_tmp55A;(_tmp55A=0,Cyc_fprintf(Cyc_stderr,((_tmp55B="Error: unexpected Datatype_e\n",_tag_dyneither(_tmp55B,sizeof(char),30))),_tag_dyneither(_tmp55A,sizeof(void*),0)));}
exit(1);return;_LL119: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp108=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp108->tag != 31)goto _LL11B;}_LL11A:
# 912
{const char*_tmp55E;void*_tmp55D;(_tmp55D=0,Cyc_fprintf(Cyc_stderr,((_tmp55E="Error: unexpected Enum_e\n",_tag_dyneither(_tmp55E,sizeof(char),26))),_tag_dyneither(_tmp55D,sizeof(void*),0)));}
exit(1);return;_LL11B: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp109=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBC;if(_tmp109->tag != 32)goto _LLCC;}_LL11C:
# 915
{const char*_tmp561;void*_tmp560;(_tmp560=0,Cyc_fprintf(Cyc_stderr,((_tmp561="Error: unexpected AnonEnum_e\n",_tag_dyneither(_tmp561,sizeof(char),30))),_tag_dyneither(_tmp560,sizeof(void*),0)));}
exit(1);return;_LLCC:;}
# 920
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 925
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp132=t;struct Cyc_Absyn_PtrInfo _tmp138;void*_tmp13A;struct Cyc_Absyn_Exp*_tmp13B;union Cyc_Absyn_Constraint*_tmp13C;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_FnInfo _tmp140;struct Cyc_List_List*_tmp142;union Cyc_Absyn_AggrInfoU _tmp145;struct _dyneither_ptr*_tmp147;struct _dyneither_ptr*_tmp149;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct Cyc_Absyn_Enumdecl*_tmp14F;struct Cyc_Absyn_Datatypedecl*_tmp152;_LL123: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp133=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp132;if(_tmp133->tag != 0)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp134=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp132;if(_tmp134->tag != 6)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp135=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp132;if(_tmp135->tag != 28)goto _LL129;}_LL128:
 goto _LL12A;_LL129: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp136=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp132;if(_tmp136->tag != 7)goto _LL12B;}_LL12A:
# 932
 return;_LL12B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp137=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132;if(_tmp137->tag != 5)goto _LL12D;else{_tmp138=_tmp137->f1;}}_LL12C:
# 935
 Cyc_scan_type(_tmp138.elt_typ,dep);
return;_LL12D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp139=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp132;if(_tmp139->tag != 8)goto _LL12F;else{_tmp13A=(_tmp139->f1).elt_type;_tmp13B=(_tmp139->f1).num_elts;_tmp13C=(_tmp139->f1).zero_term;}}_LL12E:
# 938
 Cyc_scan_type(_tmp13A,dep);
Cyc_scan_exp_opt(_tmp13B,dep);
return;_LL12F: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp132;if(_tmp13D->tag != 27)goto _LL131;else{_tmp13E=_tmp13D->f1;}}_LL130:
# 942
 Cyc_scan_exp(_tmp13E,dep);
return;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp132;if(_tmp13F->tag != 9)goto _LL133;else{_tmp140=_tmp13F->f1;}}_LL132:
# 945
 Cyc_scan_type(_tmp140.ret_typ,dep);
{struct Cyc_List_List*_tmp162=_tmp140.args;for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
struct _tuple8*_tmp163=(struct _tuple8*)_tmp162->hd;void*_tmp165;struct _tuple8*_tmp164=_tmp163;_tmp165=_tmp164->f3;
Cyc_scan_type(_tmp165,dep);}}
# 950
if(_tmp140.cyc_varargs != 0)
Cyc_scan_type((_tmp140.cyc_varargs)->type,dep);
return;_LL133: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp132;if(_tmp141->tag != 12)goto _LL135;else{_tmp142=_tmp141->f2;}}_LL134:
# 954
 for(0;_tmp142 != 0;_tmp142=_tmp142->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp142->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp142->hd)->width,dep);}
# 958
return;_LL135: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp132;if(_tmp143->tag != 14)goto _LL137;}_LL136:
# 960
 return;_LL137: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp144=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp132;if(_tmp144->tag != 11)goto _LL139;else{_tmp145=(_tmp144->f1).aggr_info;}}_LL138: {
# 962
struct _tuple10 _tmp166=Cyc_Absyn_aggr_kinded_name(_tmp145);struct _dyneither_ptr*_tmp168;struct _tuple10 _tmp167=_tmp166;_tmp168=(_tmp167.f2)->f2;
_tmp147=_tmp168;goto _LL13A;}_LL139: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp132;if(_tmp146->tag != 13)goto _LL13B;else{_tmp147=(_tmp146->f1)->f2;}}_LL13A:
# 965
 _tmp149=_tmp147;goto _LL13C;_LL13B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp148=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp132;if(_tmp148->tag != 17)goto _LL13D;else{_tmp149=(_tmp148->f1)->f2;}}_LL13C:
# 967
 Cyc_add_target(_tmp149);
return;_LL13D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp14A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp132;if(_tmp14A->tag != 26)goto _LL13F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp14B=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp14A->f1)->r;if(_tmp14B->tag != 0)goto _LL13F;else{_tmp14C=_tmp14B->f1;}}}_LL13E:
# 971
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp564;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp563;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp563=_cycalloc(sizeof(*_tmp563)),((_tmp563[0]=((_tmp564.tag=5,((_tmp564.f1=_tmp14C,_tmp564)))),_tmp563)))),0),dep);}{
struct _tuple0*_tmp16B=_tmp14C->name;struct _dyneither_ptr*_tmp16D;struct _tuple0*_tmp16C=_tmp16B;_tmp16D=_tmp16C->f2;
Cyc_add_target(_tmp16D);
return;};_LL13F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp14D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp132;if(_tmp14D->tag != 26)goto _LL141;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp14E=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp14D->f1)->r;if(_tmp14E->tag != 1)goto _LL141;else{_tmp14F=_tmp14E->f1;}}}_LL140:
# 977
{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp567;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp566;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp566=_cycalloc(sizeof(*_tmp566)),((_tmp566[0]=((_tmp567.tag=7,((_tmp567.f1=_tmp14F,_tmp567)))),_tmp566)))),0),dep);}{
struct _tuple0*_tmp170=_tmp14F->name;struct _dyneither_ptr*_tmp172;struct _tuple0*_tmp171=_tmp170;_tmp172=_tmp171->f2;
Cyc_add_target(_tmp172);
return;};_LL141: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp132;if(_tmp150->tag != 26)goto _LL143;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp151=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp150->f1)->r;if(_tmp151->tag != 2)goto _LL143;else{_tmp152=_tmp151->f1;}}}_LL142:
# 983
{const char*_tmp56A;void*_tmp569;(_tmp569=0,Cyc_fprintf(Cyc_stderr,((_tmp56A="Error: unexpected Datatype declaration\n",_tag_dyneither(_tmp56A,sizeof(char),40))),_tag_dyneither(_tmp569,sizeof(void*),0)));}
exit(1);return;_LL143: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp153=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp132;if(_tmp153->tag != 1)goto _LL145;}_LL144:
# 986
{const char*_tmp56D;void*_tmp56C;(_tmp56C=0,Cyc_fprintf(Cyc_stderr,((_tmp56D="Error: unexpected Evar\n",_tag_dyneither(_tmp56D,sizeof(char),24))),_tag_dyneither(_tmp56C,sizeof(void*),0)));}
exit(1);return;_LL145: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp154=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp132;if(_tmp154->tag != 2)goto _LL147;}_LL146:
# 989
{const char*_tmp570;void*_tmp56F;(_tmp56F=0,Cyc_fprintf(Cyc_stderr,((_tmp570="Error: unexpected VarType\n",_tag_dyneither(_tmp570,sizeof(char),27))),_tag_dyneither(_tmp56F,sizeof(void*),0)));}
exit(1);return;_LL147: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp132;if(_tmp155->tag != 3)goto _LL149;}_LL148:
# 992
{const char*_tmp573;void*_tmp572;(_tmp572=0,Cyc_fprintf(Cyc_stderr,((_tmp573="Error: unexpected DatatypeType\n",_tag_dyneither(_tmp573,sizeof(char),32))),_tag_dyneither(_tmp572,sizeof(void*),0)));}
exit(1);return;_LL149: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp156=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp132;if(_tmp156->tag != 4)goto _LL14B;}_LL14A:
# 995
{const char*_tmp576;void*_tmp575;(_tmp575=0,Cyc_fprintf(Cyc_stderr,((_tmp576="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp576,sizeof(char),37))),_tag_dyneither(_tmp575,sizeof(void*),0)));}
exit(1);return;_LL14B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp132;if(_tmp157->tag != 10)goto _LL14D;}_LL14C:
# 998
{const char*_tmp579;void*_tmp578;(_tmp578=0,Cyc_fprintf(Cyc_stderr,((_tmp579="Error: unexpected TupleType\n",_tag_dyneither(_tmp579,sizeof(char),29))),_tag_dyneither(_tmp578,sizeof(void*),0)));}
exit(1);return;_LL14D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp132;if(_tmp158->tag != 15)goto _LL14F;}_LL14E:
# 1001
{const char*_tmp57C;void*_tmp57B;(_tmp57B=0,Cyc_fprintf(Cyc_stderr,((_tmp57C="Error: unexpected RgnHandleType\n",_tag_dyneither(_tmp57C,sizeof(char),33))),_tag_dyneither(_tmp57B,sizeof(void*),0)));}
exit(1);return;_LL14F: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp132;if(_tmp159->tag != 16)goto _LL151;}_LL150:
# 1004
{const char*_tmp57F;void*_tmp57E;(_tmp57E=0,Cyc_fprintf(Cyc_stderr,((_tmp57F="Error: unexpected DynRgnType\n",_tag_dyneither(_tmp57F,sizeof(char),30))),_tag_dyneither(_tmp57E,sizeof(void*),0)));}
exit(1);return;_LL151: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp132;if(_tmp15A->tag != 20)goto _LL153;}_LL152:
# 1007
{const char*_tmp582;void*_tmp581;(_tmp581=0,Cyc_fprintf(Cyc_stderr,((_tmp582="Error: unexpected HeapRgn\n",_tag_dyneither(_tmp582,sizeof(char),27))),_tag_dyneither(_tmp581,sizeof(void*),0)));}
exit(1);return;_LL153: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp132;if(_tmp15B->tag != 21)goto _LL155;}_LL154:
# 1010
{const char*_tmp585;void*_tmp584;(_tmp584=0,Cyc_fprintf(Cyc_stderr,((_tmp585="Error: unexpected UniqueRgn\n",_tag_dyneither(_tmp585,sizeof(char),29))),_tag_dyneither(_tmp584,sizeof(void*),0)));}
exit(1);return;_LL155: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp15C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp132;if(_tmp15C->tag != 22)goto _LL157;}_LL156:
# 1013
{const char*_tmp588;void*_tmp587;(_tmp587=0,Cyc_fprintf(Cyc_stderr,((_tmp588="Error: unexpected RefCntRgn\n",_tag_dyneither(_tmp588,sizeof(char),29))),_tag_dyneither(_tmp587,sizeof(void*),0)));}
exit(1);return;_LL157: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp132;if(_tmp15D->tag != 23)goto _LL159;}_LL158:
# 1016
{const char*_tmp58B;void*_tmp58A;(_tmp58A=0,Cyc_fprintf(Cyc_stderr,((_tmp58B="Error: unexpected AccessEff\n",_tag_dyneither(_tmp58B,sizeof(char),29))),_tag_dyneither(_tmp58A,sizeof(void*),0)));}
exit(1);return;_LL159: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp15E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp132;if(_tmp15E->tag != 24)goto _LL15B;}_LL15A:
# 1019
{const char*_tmp58E;void*_tmp58D;(_tmp58D=0,Cyc_fprintf(Cyc_stderr,((_tmp58E="Error: unexpected JoinEff\n",_tag_dyneither(_tmp58E,sizeof(char),27))),_tag_dyneither(_tmp58D,sizeof(void*),0)));}
exit(1);return;_LL15B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp132;if(_tmp15F->tag != 25)goto _LL15D;}_LL15C:
# 1022
{const char*_tmp591;void*_tmp590;(_tmp590=0,Cyc_fprintf(Cyc_stderr,((_tmp591="Error: unexpected RgnsEff\n",_tag_dyneither(_tmp591,sizeof(char),27))),_tag_dyneither(_tmp590,sizeof(void*),0)));}
exit(1);return;_LL15D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp132;if(_tmp160->tag != 19)goto _LL15F;}_LL15E:
# 1025
{const char*_tmp594;void*_tmp593;(_tmp593=0,Cyc_fprintf(Cyc_stderr,((_tmp594="Error: unexpected tag_t\n",_tag_dyneither(_tmp594,sizeof(char),25))),_tag_dyneither(_tmp593,sizeof(void*),0)));}
exit(1);return;_LL15F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp132;if(_tmp161->tag != 18)goto _LL122;}_LL160:
# 1028
{const char*_tmp597;void*_tmp596;(_tmp596=0,Cyc_fprintf(Cyc_stderr,((_tmp597="Error: unexpected valueof_t\n",_tag_dyneither(_tmp597,sizeof(char),29))),_tag_dyneither(_tmp596,sizeof(void*),0)));}
exit(1);return;_LL122:;}
# 1033
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp193=Cyc_current_targets;
struct _dyneither_ptr*_tmp194=Cyc_current_source;
{struct Cyc_Set_Set**_tmp598;Cyc_current_targets=((_tmp598=_cycalloc(sizeof(*_tmp598)),((_tmp598[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp598))));}
{void*_tmp196=d->r;void*_tmp197=_tmp196;struct Cyc_Absyn_Vardecl*_tmp199;struct Cyc_Absyn_Fndecl*_tmp19B;struct Cyc_Absyn_Aggrdecl*_tmp19D;struct Cyc_Absyn_Enumdecl*_tmp19F;struct Cyc_Absyn_Typedefdecl*_tmp1A1;_LL162: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp198=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp198->tag != 0)goto _LL164;else{_tmp199=_tmp198->f1;}}_LL163: {
# 1039
struct _tuple0*_tmp1AC=_tmp199->name;struct _dyneither_ptr*_tmp1AE;struct _tuple0*_tmp1AD=_tmp1AC;_tmp1AE=_tmp1AD->f2;
Cyc_current_source=_tmp1AE;
Cyc_scan_type(_tmp199->type,dep);
Cyc_scan_exp_opt(_tmp199->initializer,dep);
goto _LL161;}_LL164: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp19A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp19A->tag != 1)goto _LL166;else{_tmp19B=_tmp19A->f1;}}_LL165: {
# 1045
struct _tuple0*_tmp1AF=_tmp19B->name;struct _dyneither_ptr*_tmp1B1;struct _tuple0*_tmp1B0=_tmp1AF;_tmp1B1=_tmp1B0->f2;
Cyc_current_source=_tmp1B1;
Cyc_scan_type(_tmp19B->ret_type,dep);
{struct Cyc_List_List*_tmp1B2=_tmp19B->args;for(0;_tmp1B2 != 0;_tmp1B2=_tmp1B2->tl){
struct _tuple8*_tmp1B3=(struct _tuple8*)_tmp1B2->hd;void*_tmp1B5;struct _tuple8*_tmp1B4=_tmp1B3;_tmp1B5=_tmp1B4->f3;
Cyc_scan_type(_tmp1B5,dep);}}
# 1052
if(_tmp19B->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp19B->cyc_varargs))->type,dep);
if(_tmp19B->is_inline){
const char*_tmp59B;void*_tmp59A;(_tmp59A=0,Cyc_fprintf(Cyc_stderr,((_tmp59B="Warning: ignoring inline function\n",_tag_dyneither(_tmp59B,sizeof(char),35))),_tag_dyneither(_tmp59A,sizeof(void*),0)));}
# 1057
goto _LL161;}_LL166: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp19C=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp19C->tag != 5)goto _LL168;else{_tmp19D=_tmp19C->f1;}}_LL167: {
# 1059
struct _tuple0*_tmp1B8=_tmp19D->name;struct _dyneither_ptr*_tmp1BA;struct _tuple0*_tmp1B9=_tmp1B8;_tmp1BA=_tmp1B9->f2;
Cyc_current_source=_tmp1BA;
if((unsigned int)_tmp19D->impl){
{struct Cyc_List_List*_tmp1BB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp19D->impl))->fields;for(0;_tmp1BB != 0;_tmp1BB=_tmp1BB->tl){
struct Cyc_Absyn_Aggrfield*_tmp1BC=(struct Cyc_Absyn_Aggrfield*)_tmp1BB->hd;
Cyc_scan_type(_tmp1BC->type,dep);
Cyc_scan_exp_opt(_tmp1BC->width,dep);}}{
# 1069
struct Cyc_List_List*_tmp1BD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp19D->impl))->fields;for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){;}};}
# 1073
goto _LL161;}_LL168: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp19E=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp19E->tag != 7)goto _LL16A;else{_tmp19F=_tmp19E->f1;}}_LL169: {
# 1075
struct _tuple0*_tmp1BE=_tmp19F->name;struct _dyneither_ptr*_tmp1C0;struct _tuple0*_tmp1BF=_tmp1BE;_tmp1C0=_tmp1BF->f2;
Cyc_current_source=_tmp1C0;
if((unsigned int)_tmp19F->fields){
{struct Cyc_List_List*_tmp1C1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp19F->fields))->v;for(0;_tmp1C1 != 0;_tmp1C1=_tmp1C1->tl){
struct Cyc_Absyn_Enumfield*_tmp1C2=(struct Cyc_Absyn_Enumfield*)_tmp1C1->hd;
Cyc_scan_exp_opt(_tmp1C2->tag,dep);}}{
# 1084
struct Cyc_List_List*_tmp1C3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp19F->fields))->v;for(0;_tmp1C3 != 0;_tmp1C3=_tmp1C3->tl){;}};}
# 1088
goto _LL161;}_LL16A: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1A0=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A0->tag != 8)goto _LL16C;else{_tmp1A1=_tmp1A0->f1;}}_LL16B: {
# 1090
struct _tuple0*_tmp1C4=_tmp1A1->name;struct _dyneither_ptr*_tmp1C6;struct _tuple0*_tmp1C5=_tmp1C4;_tmp1C6=_tmp1C5->f2;
Cyc_current_source=_tmp1C6;
if((unsigned int)_tmp1A1->defn)
Cyc_scan_type((void*)_check_null(_tmp1A1->defn),dep);
goto _LL161;}_LL16C: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp1A2=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A2->tag != 4)goto _LL16E;}_LL16D:
# 1096
{const char*_tmp59E;void*_tmp59D;(_tmp59D=0,Cyc_fprintf(Cyc_stderr,((_tmp59E="Error: unexpected region declaration",_tag_dyneither(_tmp59E,sizeof(char),37))),_tag_dyneither(_tmp59D,sizeof(void*),0)));}
exit(1);_LL16E: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp1A3=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A3->tag != 13)goto _LL170;}_LL16F:
# 1102
{const char*_tmp5A1;void*_tmp5A0;(_tmp5A0=0,Cyc_fprintf(Cyc_stderr,((_tmp5A1="Error: unexpected __cyclone_port_on__",_tag_dyneither(_tmp5A1,sizeof(char),38))),_tag_dyneither(_tmp5A0,sizeof(void*),0)));}
exit(1);_LL170: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp1A4=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A4->tag != 14)goto _LL172;}_LL171:
# 1105
{const char*_tmp5A4;void*_tmp5A3;(_tmp5A3=0,Cyc_fprintf(Cyc_stderr,((_tmp5A4="Error: unexpected __cyclone_port_off__",_tag_dyneither(_tmp5A4,sizeof(char),39))),_tag_dyneither(_tmp5A3,sizeof(void*),0)));}
exit(1);_LL172: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp1A5=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A5->tag != 2)goto _LL174;}_LL173:
# 1108
{const char*_tmp5A7;void*_tmp5A6;(_tmp5A6=0,Cyc_fprintf(Cyc_stderr,((_tmp5A7="Error: unexpected let declaration\n",_tag_dyneither(_tmp5A7,sizeof(char),35))),_tag_dyneither(_tmp5A6,sizeof(void*),0)));}
exit(1);_LL174: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1A6=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A6->tag != 6)goto _LL176;}_LL175:
# 1111
{const char*_tmp5AA;void*_tmp5A9;(_tmp5A9=0,Cyc_fprintf(Cyc_stderr,((_tmp5AA="Error: unexpected datatype declaration\n",_tag_dyneither(_tmp5AA,sizeof(char),40))),_tag_dyneither(_tmp5A9,sizeof(void*),0)));}
exit(1);_LL176: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp1A7=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A7->tag != 3)goto _LL178;}_LL177:
# 1114
{const char*_tmp5AD;void*_tmp5AC;(_tmp5AC=0,Cyc_fprintf(Cyc_stderr,((_tmp5AD="Error: unexpected let declaration\n",_tag_dyneither(_tmp5AD,sizeof(char),35))),_tag_dyneither(_tmp5AC,sizeof(void*),0)));}
exit(1);_LL178: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp1A8=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A8->tag != 9)goto _LL17A;}_LL179:
# 1117
{const char*_tmp5B0;void*_tmp5AF;(_tmp5AF=0,Cyc_fprintf(Cyc_stderr,((_tmp5B0="Error: unexpected namespace declaration\n",_tag_dyneither(_tmp5B0,sizeof(char),41))),_tag_dyneither(_tmp5AF,sizeof(void*),0)));}
exit(1);_LL17A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp1A9=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1A9->tag != 10)goto _LL17C;}_LL17B:
# 1120
{const char*_tmp5B3;void*_tmp5B2;(_tmp5B2=0,Cyc_fprintf(Cyc_stderr,((_tmp5B3="Error: unexpected using declaration\n",_tag_dyneither(_tmp5B3,sizeof(char),37))),_tag_dyneither(_tmp5B2,sizeof(void*),0)));}
exit(1);_LL17C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp1AA=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1AA->tag != 11)goto _LL17E;}_LL17D:
# 1123
{const char*_tmp5B6;void*_tmp5B5;(_tmp5B5=0,Cyc_fprintf(Cyc_stderr,((_tmp5B6="Error: unexpected extern \"C\" declaration\n",_tag_dyneither(_tmp5B6,sizeof(char),42))),_tag_dyneither(_tmp5B5,sizeof(void*),0)));}
exit(1);_LL17E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp1AB=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp197;if(_tmp1AB->tag != 12)goto _LL161;}_LL17F:
# 1126
{const char*_tmp5B9;void*_tmp5B8;(_tmp5B8=0,Cyc_fprintf(Cyc_stderr,((_tmp5B9="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(_tmp5B9,sizeof(char),50))),_tag_dyneither(_tmp5B8,sizeof(void*),0)));}
exit(1);_LL161:;}{
# 1134
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp1DB=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp1DC;_push_handler(& _tmp1DC);{int _tmp1DE=0;if(setjmp(_tmp1DC.handler))_tmp1DE=1;if(!_tmp1DE){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp1DB);;_pop_handler();}else{void*_tmp1DD=(void*)_exn_thrown;void*_tmp1E0=_tmp1DD;void*_tmp1E2;_LL181: {struct Cyc_Core_Not_found_exn_struct*_tmp1E1=(struct Cyc_Core_Not_found_exn_struct*)_tmp1E0;if(_tmp1E1->tag != Cyc_Core_Not_found)goto _LL183;}_LL182:
# 1139
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL180;_LL183: _tmp1E2=_tmp1E0;_LL184:(void)_rethrow(_tmp1E2);_LL180:;}};}{
# 1141
struct Cyc_Set_Set*_tmp1E3=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp1DB,_tmp1E3);
# 1144
Cyc_current_targets=_tmp193;
Cyc_current_source=_tmp194;};};}
# 1148
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1152
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp1E4;_push_handler(& _tmp1E4);{int _tmp1E6=0;if(setjmp(_tmp1E4.handler))_tmp1E6=1;if(!_tmp1E6){{struct Cyc_Set_Set*_tmp1E7=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp1E7;};_pop_handler();}else{void*_tmp1E5=(void*)_exn_thrown;void*_tmp1E9=_tmp1E5;void*_tmp1EB;_LL186: {struct Cyc_Core_Not_found_exn_struct*_tmp1EA=(struct Cyc_Core_Not_found_exn_struct*)_tmp1E9;if(_tmp1EA->tag != Cyc_Core_Not_found)goto _LL188;}_LL187:
# 1155
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL188: _tmp1EB=_tmp1E9;_LL189:(void)_rethrow(_tmp1EB);_LL185:;}};}
# 1159
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1170 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1EC=curr;
# 1176
struct _dyneither_ptr*_tmp1ED=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1EC)> 0){
struct Cyc_Set_Set*_tmp1EE=emptyset;
struct Cyc_Iter_Iter _tmp1EF=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1EC);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1EF,& _tmp1ED)){
_tmp1EE=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1EE,Cyc_find(t,_tmp1ED));}
_tmp1EC=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1EE,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp1EC);}
# 1185
return curr;};}
# 1188
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1194
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1198
if(Cyc_script_file == 0){
{const char*_tmp5BC;void*_tmp5BB;(_tmp5BB=0,Cyc_fprintf(Cyc_stderr,((_tmp5BC="Internal error: script file is NULL\n",_tag_dyneither(_tmp5BC,sizeof(char),37))),_tag_dyneither(_tmp5BB,sizeof(void*),0)));}
exit(1);}
# 1202
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1205
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT){
const char*_tmp5C1;void*_tmp5C0[2];struct Cyc_String_pa_PrintArg_struct _tmp5BF;struct Cyc_String_pa_PrintArg_struct _tmp5BE;(_tmp5BE.tag=0,((_tmp5BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp5BF.tag=0,((_tmp5BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp5C0[0]=& _tmp5BF,((_tmp5C0[1]=& _tmp5BE,Cyc_prscript(((_tmp5C1="if ! test -e %s; then mkdir %s; fi\n",_tag_dyneither(_tmp5C1,sizeof(char),36))),_tag_dyneither(_tmp5C0,sizeof(void*),2)))))))))))));}else{
# 1212
unsigned short _tmp5C2[0];int _tmp1F7=Cyc_open((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp5C2,sizeof(unsigned short),0));
if(_tmp1F7 == - 1){
if(mkdir((const char*)_untag_dyneither_ptr(d,sizeof(char),1),448)== - 1){
{const char*_tmp5C6;void*_tmp5C5[1];struct Cyc_String_pa_PrintArg_struct _tmp5C4;(_tmp5C4.tag=0,((_tmp5C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp5C5[0]=& _tmp5C4,Cyc_fprintf(Cyc_stderr,((_tmp5C6="Error: could not create directory %s\n",_tag_dyneither(_tmp5C6,sizeof(char),38))),_tag_dyneither(_tmp5C5,sizeof(void*),1)))))));}
return 1;}}else{
# 1219
close(_tmp1F7);}}
# 1221
return 0;}
# 1224
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1228
struct _dyneither_ptr _tmp1FC=Cyc_strdup((struct _dyneither_ptr)file);
# 1230
struct Cyc_List_List*_tmp1FD=0;
while(1){
_tmp1FC=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1FC);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1FC)== 0)break;{
struct _dyneither_ptr*_tmp5C9;struct Cyc_List_List*_tmp5C8;_tmp1FD=((_tmp5C8=_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8->hd=((_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9[0]=(struct _dyneither_ptr)_tmp1FC,_tmp5C9)))),((_tmp5C8->tl=_tmp1FD,_tmp5C8))))));};}
# 1237
for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp1FD->hd)))return 1;}
# 1240
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1247
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL18A:
 goto _LL18B;case '"': _LL18B:
 goto _LL18C;case ';': _LL18C:
 goto _LL18D;case '&': _LL18D:
 goto _LL18E;case '(': _LL18E:
 goto _LL18F;case ')': _LL18F:
 goto _LL190;case '|': _LL190:
 goto _LL191;case '^': _LL191:
 goto _LL192;case '<': _LL192:
 goto _LL193;case '>': _LL193:
 goto _LL194;case '\n': _LL194:
# 1262
 goto _LL195;case '\t': _LL195:
 return 1;default: _LL196:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp5D2(unsigned int*_tmp5D1,unsigned int*_tmp5D0,char**_tmp5CE){for(*_tmp5D1=0;*_tmp5D1 < *_tmp5D0;(*_tmp5D1)++){(*_tmp5CE)[*_tmp5D1]=(char)'\000';}}
# 1268
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp200=Cyc_strlen((struct _dyneither_ptr)s);
# 1272
int _tmp201=0;
int _tmp202=0;
{int i=0;for(0;i < _tmp200;++ i){
char _tmp203=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp203 == '\'')++ _tmp201;else{
if(Cyc_is_other_special(_tmp203))++ _tmp202;}}}
# 1281
if(_tmp201 == 0  && _tmp202 == 0)
return s;
# 1285
if(_tmp201 == 0){
struct _dyneither_ptr*_tmp5CC;struct _dyneither_ptr*_tmp5CB[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp5CB[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp5CB[1]=((_tmp5CC=_cycalloc(sizeof(*_tmp5CC)),((_tmp5CC[0]=(struct _dyneither_ptr)s,_tmp5CC)))),((_tmp5CB[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5CB,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 1289
unsigned long _tmp208=(_tmp200 + _tmp201)+ _tmp202;
unsigned int _tmp5D1;unsigned int _tmp5D0;struct _dyneither_ptr _tmp5CF;char*_tmp5CE;unsigned int _tmp5CD;struct _dyneither_ptr s2=(_tmp5CD=_tmp208 + 1,((_tmp5CE=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp5CD + 1)),((_tmp5CF=_tag_dyneither(_tmp5CE,sizeof(char),_tmp5CD + 1),((((_tmp5D0=_tmp5CD,((_tmp5D2(& _tmp5D1,& _tmp5D0,& _tmp5CE),_tmp5CE[_tmp5D0]=(char)0)))),_tmp5CF)))))));
int _tmp209=0;
int _tmp20A=0;
for(0;_tmp209 < _tmp200;++ _tmp209){
char _tmp20B=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp209));
if(_tmp20B == '\''  || Cyc_is_other_special(_tmp20B)){
char _tmp5D5;char _tmp5D4;struct _dyneither_ptr _tmp5D3;(_tmp5D3=_dyneither_ptr_plus(s2,sizeof(char),_tmp20A ++),((_tmp5D4=*((char*)_check_dyneither_subscript(_tmp5D3,sizeof(char),0)),((_tmp5D5='\\',((_get_dyneither_size(_tmp5D3,sizeof(char))== 1  && (_tmp5D4 == '\000'  && _tmp5D5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D3.curr)=_tmp5D5)))))));}{
char _tmp5D8;char _tmp5D7;struct _dyneither_ptr _tmp5D6;(_tmp5D6=_dyneither_ptr_plus(s2,sizeof(char),_tmp20A ++),((_tmp5D7=*((char*)_check_dyneither_subscript(_tmp5D6,sizeof(char),0)),((_tmp5D8=_tmp20B,((_get_dyneither_size(_tmp5D6,sizeof(char))== 1  && (_tmp5D7 == '\000'  && _tmp5D8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D6.curr)=_tmp5D8)))))));};}
# 1299
return(struct _dyneither_ptr)s2;};}
# 1301
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp5D9;return(_tmp5D9=_cycalloc(sizeof(*_tmp5D9)),((_tmp5D9[0]=Cyc_sh_escape_string(*sp),_tmp5D9)));}struct _tuple25{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1306
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1312
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1317
{const char*_tmp5DF;void*_tmp5DE[1];const char*_tmp5DD;struct Cyc_String_pa_PrintArg_struct _tmp5DC;(_tmp5DC.tag=0,((_tmp5DC.f1=(struct _dyneither_ptr)(
(_tmp5DD=filename,_tag_dyneither(_tmp5DD,sizeof(char),_get_zero_arr_size_char((void*)_tmp5DD,1)))),((_tmp5DE[0]=& _tmp5DC,Cyc_fprintf(Cyc_stderr,((_tmp5DF="********************************* %s...\n",_tag_dyneither(_tmp5DF,sizeof(char),41))),_tag_dyneither(_tmp5DE,sizeof(void*),1)))))));}
# 1320
if(!Cyc_gathering()){const char*_tmp5E5;void*_tmp5E4[1];const char*_tmp5E3;struct Cyc_String_pa_PrintArg_struct _tmp5E2;(_tmp5E2.tag=0,((_tmp5E2.f1=(struct _dyneither_ptr)((_tmp5E3=filename,_tag_dyneither(_tmp5E3,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E3,1)))),((_tmp5E4[0]=& _tmp5E2,Cyc_log(((_tmp5E5="\n%s:\n",_tag_dyneither(_tmp5E5,sizeof(char),6))),_tag_dyneither(_tmp5E4,sizeof(void*),1)))))));}{
# 1332 "buildlib.cyl"
const char*_tmp5E6;struct _dyneither_ptr _tmp21F=Cyc_Filename_basename(((_tmp5E6=filename,_tag_dyneither(_tmp5E6,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E6,1)))));
const char*_tmp5E7;struct _dyneither_ptr _tmp220=Cyc_Filename_dirname(((_tmp5E7=filename,_tag_dyneither(_tmp5E7,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E7,1)))));
struct _dyneither_ptr _tmp221=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp21F);
const char*_tmp5E8;const char*_tmp222=(const char*)_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp221,((_tmp5E8=".iA",_tag_dyneither(_tmp5E8,sizeof(char),4)))),sizeof(char),1);
const char*_tmp5F0;void*_tmp5EF[1];struct Cyc_String_pa_PrintArg_struct _tmp5EE;const char*_tmp5EC;void*_tmp5EB[1];struct Cyc_String_pa_PrintArg_struct _tmp5EA;const char*_tmp223=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp220,sizeof(char))== 0?(_tmp5EE.tag=0,((_tmp5EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp221),((_tmp5EF[0]=& _tmp5EE,Cyc_aprintf(((_tmp5F0="%s.iB",_tag_dyneither(_tmp5F0,sizeof(char),6))),_tag_dyneither(_tmp5EF,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp220,(struct _dyneither_ptr)((_tmp5EA.tag=0,((_tmp5EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp221),((_tmp5EB[0]=& _tmp5EA,Cyc_aprintf(((_tmp5EC="%s.iB",_tag_dyneither(_tmp5EC,sizeof(char),6))),_tag_dyneither(_tmp5EB,sizeof(void*),1))))))))),sizeof(char),1);
# 1340
const char*_tmp5F8;void*_tmp5F7[1];struct Cyc_String_pa_PrintArg_struct _tmp5F6;const char*_tmp5F4;void*_tmp5F3[1];struct Cyc_String_pa_PrintArg_struct _tmp5F2;const char*_tmp224=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp220,sizeof(char))== 0?(_tmp5F6.tag=0,((_tmp5F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp221),((_tmp5F7[0]=& _tmp5F6,Cyc_aprintf(((_tmp5F8="%s.iC",_tag_dyneither(_tmp5F8,sizeof(char),6))),_tag_dyneither(_tmp5F7,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp220,(struct _dyneither_ptr)((_tmp5F2.tag=0,((_tmp5F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp221),((_tmp5F3[0]=& _tmp5F2,Cyc_aprintf(((_tmp5F4="%s.iC",_tag_dyneither(_tmp5F4,sizeof(char),6))),_tag_dyneither(_tmp5F3,sizeof(void*),1))))))))),sizeof(char),1);
# 1344
const char*_tmp600;void*_tmp5FF[1];struct Cyc_String_pa_PrintArg_struct _tmp5FE;const char*_tmp5FC;void*_tmp5FB[1];struct Cyc_String_pa_PrintArg_struct _tmp5FA;const char*_tmp225=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp220,sizeof(char))== 0?(_tmp5FE.tag=0,((_tmp5FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp221),((_tmp5FF[0]=& _tmp5FE,Cyc_aprintf(((_tmp600="%s.iD",_tag_dyneither(_tmp600,sizeof(char),6))),_tag_dyneither(_tmp5FF,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp220,(struct _dyneither_ptr)((_tmp5FA.tag=0,((_tmp5FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp221),((_tmp5FB[0]=& _tmp5FA,Cyc_aprintf(((_tmp5FC="%s.iD",_tag_dyneither(_tmp5FC,sizeof(char),6))),_tag_dyneither(_tmp5FB,sizeof(void*),1))))))))),sizeof(char),1);
# 1349
{struct _handler_cons _tmp226;_push_handler(& _tmp226);{int _tmp228=0;if(setjmp(_tmp226.handler))_tmp228=1;if(!_tmp228){
# 1352
{const char*_tmp601;if(Cyc_force_directory_prefixes(((_tmp601=filename,_tag_dyneither(_tmp601,sizeof(char),_get_zero_arr_size_char((void*)_tmp601,1)))))){
int _tmp22A=1;_npop_handler(0);return _tmp22A;}}
# 1357
if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp607;void*_tmp606[1];const char*_tmp605;struct Cyc_String_pa_PrintArg_struct _tmp604;(_tmp604.tag=0,((_tmp604.f1=(struct _dyneither_ptr)((_tmp605=_tmp222,_tag_dyneither(_tmp605,sizeof(char),_get_zero_arr_size_char((void*)_tmp605,1)))),((_tmp606[0]=& _tmp604,Cyc_prscript(((_tmp607="cat >%s <<XXX\n",_tag_dyneither(_tmp607,sizeof(char),15))),_tag_dyneither(_tmp606,sizeof(void*),1)))))));}
{struct Cyc_List_List*_tmp22F=Cyc_current_cpp;for(0;_tmp22F != 0;_tmp22F=_tmp22F->tl){
const char*_tmp60B;void*_tmp60A[1];struct Cyc_String_pa_PrintArg_struct _tmp609;(_tmp609.tag=0,((_tmp609.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp22F->hd)),((_tmp60A[0]=& _tmp609,Cyc_prscript(((_tmp60B="%s",_tag_dyneither(_tmp60B,sizeof(char),3))),_tag_dyneither(_tmp60A,sizeof(void*),1)))))));}}
{const char*_tmp611;void*_tmp610[1];const char*_tmp60F;struct Cyc_String_pa_PrintArg_struct _tmp60E;(_tmp60E.tag=0,((_tmp60E.f1=(struct _dyneither_ptr)((_tmp60F=filename,_tag_dyneither(_tmp60F,sizeof(char),_get_zero_arr_size_char((void*)_tmp60F,1)))),((_tmp610[0]=& _tmp60E,Cyc_prscript(((_tmp611="#include <%s>\n",_tag_dyneither(_tmp611,sizeof(char),15))),_tag_dyneither(_tmp610,sizeof(void*),1)))))));}
{const char*_tmp614;void*_tmp613;(_tmp613=0,Cyc_prscript(((_tmp614="XXX\n",_tag_dyneither(_tmp614,sizeof(char),5))),_tag_dyneither(_tmp613,sizeof(void*),0)));}
{const char*_tmp61D;void*_tmp61C[2];const char*_tmp61B;struct Cyc_String_pa_PrintArg_struct _tmp61A;const char*_tmp619;struct Cyc_String_pa_PrintArg_struct _tmp618;(_tmp618.tag=0,((_tmp618.f1=(struct _dyneither_ptr)((_tmp619=_tmp222,_tag_dyneither(_tmp619,sizeof(char),_get_zero_arr_size_char((void*)_tmp619,1)))),((_tmp61A.tag=0,((_tmp61A.f1=(struct _dyneither_ptr)((_tmp61B=_tmp223,_tag_dyneither(_tmp61B,sizeof(char),_get_zero_arr_size_char((void*)_tmp61B,1)))),((_tmp61C[0]=& _tmp61A,((_tmp61C[1]=& _tmp618,Cyc_prscript(((_tmp61D="$GCC -E -dM -o %s -x c %s && \\\n",_tag_dyneither(_tmp61D,sizeof(char),32))),_tag_dyneither(_tmp61C,sizeof(void*),2)))))))))))));}
{const char*_tmp626;void*_tmp625[2];const char*_tmp624;struct Cyc_String_pa_PrintArg_struct _tmp623;const char*_tmp622;struct Cyc_String_pa_PrintArg_struct _tmp621;(_tmp621.tag=0,((_tmp621.f1=(struct _dyneither_ptr)((_tmp622=_tmp222,_tag_dyneither(_tmp622,sizeof(char),_get_zero_arr_size_char((void*)_tmp622,1)))),((_tmp623.tag=0,((_tmp623.f1=(struct _dyneither_ptr)((_tmp624=_tmp224,_tag_dyneither(_tmp624,sizeof(char),_get_zero_arr_size_char((void*)_tmp624,1)))),((_tmp625[0]=& _tmp623,((_tmp625[1]=& _tmp621,Cyc_prscript(((_tmp626="$GCC -E     -o %s -x c %s;\n",_tag_dyneither(_tmp626,sizeof(char),28))),_tag_dyneither(_tmp625,sizeof(void*),2)))))))))))));}{
const char*_tmp62C;void*_tmp62B[1];const char*_tmp62A;struct Cyc_String_pa_PrintArg_struct _tmp629;(_tmp629.tag=0,((_tmp629.f1=(struct _dyneither_ptr)((_tmp62A=_tmp222,_tag_dyneither(_tmp62A,sizeof(char),_get_zero_arr_size_char((void*)_tmp62A,1)))),((_tmp62B[0]=& _tmp629,Cyc_prscript(((_tmp62C="rm %s\n",_tag_dyneither(_tmp62C,sizeof(char),7))),_tag_dyneither(_tmp62B,sizeof(void*),1)))))));};}else{
# 1370
maybe=Cyc_fopen(_tmp222,"w");
if(!((unsigned int)maybe)){
{const char*_tmp632;void*_tmp631[1];const char*_tmp630;struct Cyc_String_pa_PrintArg_struct _tmp62F;(_tmp62F.tag=0,((_tmp62F.f1=(struct _dyneither_ptr)((_tmp630=_tmp222,_tag_dyneither(_tmp630,sizeof(char),_get_zero_arr_size_char((void*)_tmp630,1)))),((_tmp631[0]=& _tmp62F,Cyc_fprintf(Cyc_stderr,((_tmp632="Error: could not create file %s\n",_tag_dyneither(_tmp632,sizeof(char),33))),_tag_dyneither(_tmp631,sizeof(void*),1)))))));}{
int _tmp24D=1;_npop_handler(0);return _tmp24D;};}
# 1375
if(Cyc_verbose){
const char*_tmp638;void*_tmp637[1];const char*_tmp636;struct Cyc_String_pa_PrintArg_struct _tmp635;(_tmp635.tag=0,((_tmp635.f1=(struct _dyneither_ptr)((_tmp636=_tmp222,_tag_dyneither(_tmp636,sizeof(char),_get_zero_arr_size_char((void*)_tmp636,1)))),((_tmp637[0]=& _tmp635,Cyc_fprintf(Cyc_stderr,((_tmp638="Creating %s\n",_tag_dyneither(_tmp638,sizeof(char),13))),_tag_dyneither(_tmp637,sizeof(void*),1)))))));}
out_file=maybe;
{struct Cyc_List_List*_tmp252=Cyc_current_cpp;for(0;_tmp252 != 0;_tmp252=_tmp252->tl){
Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp252->hd),sizeof(char),1),out_file);}}
# 1381
{const char*_tmp63E;void*_tmp63D[1];const char*_tmp63C;struct Cyc_String_pa_PrintArg_struct _tmp63B;(_tmp63B.tag=0,((_tmp63B.f1=(struct _dyneither_ptr)((_tmp63C=filename,_tag_dyneither(_tmp63C,sizeof(char),_get_zero_arr_size_char((void*)_tmp63C,1)))),((_tmp63D[0]=& _tmp63B,Cyc_fprintf(out_file,((_tmp63E="#include <%s>\n",_tag_dyneither(_tmp63E,sizeof(char),15))),_tag_dyneither(_tmp63D,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp257=Cstring_to_string(Ccomp);
const char*_tmp645;struct _dyneither_ptr*_tmp644;const char*_tmp643;struct Cyc_List_List*_tmp642;struct _dyneither_ptr _tmp258=
Cyc_str_sepstr(((_tmp642=_cycalloc(sizeof(*_tmp642)),((_tmp642->hd=((_tmp644=_cycalloc(sizeof(*_tmp644)),((_tmp644[0]=(struct _dyneither_ptr)((_tmp643="",_tag_dyneither(_tmp643,sizeof(char),1))),_tmp644)))),((_tmp642->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp642)))))),(
(_tmp645=" ",_tag_dyneither(_tmp645,sizeof(char),2))));
const char*_tmp650;void*_tmp64F[4];struct Cyc_String_pa_PrintArg_struct _tmp64E;struct Cyc_String_pa_PrintArg_struct _tmp64D;const char*_tmp64C;struct Cyc_String_pa_PrintArg_struct _tmp64B;const char*_tmp64A;struct Cyc_String_pa_PrintArg_struct _tmp649;char*cmd=(char*)_untag_dyneither_ptr(((_tmp649.tag=0,((_tmp649.f1=(struct _dyneither_ptr)((_tmp64A=_tmp222,_tag_dyneither(_tmp64A,sizeof(char),_get_zero_arr_size_char((void*)_tmp64A,1)))),((_tmp64B.tag=0,((_tmp64B.f1=(struct _dyneither_ptr)((_tmp64C=_tmp223,_tag_dyneither(_tmp64C,sizeof(char),_get_zero_arr_size_char((void*)_tmp64C,1)))),((_tmp64D.tag=0,((_tmp64D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp258),((_tmp64E.tag=0,((_tmp64E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp257),((_tmp64F[0]=& _tmp64E,((_tmp64F[1]=& _tmp64D,((_tmp64F[2]=& _tmp64B,((_tmp64F[3]=& _tmp649,Cyc_aprintf(((_tmp650="%s %s -E -dM -o %s -x c %s",_tag_dyneither(_tmp650,sizeof(char),27))),_tag_dyneither(_tmp64F,sizeof(void*),4)))))))))))))))))))))))))),sizeof(char),1);
# 1390
if(Cyc_verbose){
const char*_tmp656;void*_tmp655[1];char*_tmp654;struct Cyc_String_pa_PrintArg_struct _tmp653;(_tmp653.tag=0,((_tmp653.f1=(struct _dyneither_ptr)((_tmp654=cmd,_tag_dyneither(_tmp654,sizeof(char),_get_zero_arr_size_char((void*)_tmp654,1)))),((_tmp655[0]=& _tmp653,Cyc_fprintf(Cyc_stderr,((_tmp656="%s\n",_tag_dyneither(_tmp656,sizeof(char),4))),_tag_dyneither(_tmp655,sizeof(void*),1)))))));}
if(!system((const char*)cmd)){
# 1395
{const char*_tmp661;void*_tmp660[4];struct Cyc_String_pa_PrintArg_struct _tmp65F;struct Cyc_String_pa_PrintArg_struct _tmp65E;const char*_tmp65D;struct Cyc_String_pa_PrintArg_struct _tmp65C;const char*_tmp65B;struct Cyc_String_pa_PrintArg_struct _tmp65A;cmd=(char*)_untag_dyneither_ptr(((_tmp65A.tag=0,((_tmp65A.f1=(struct _dyneither_ptr)((_tmp65B=_tmp222,_tag_dyneither(_tmp65B,sizeof(char),_get_zero_arr_size_char((void*)_tmp65B,1)))),((_tmp65C.tag=0,((_tmp65C.f1=(struct _dyneither_ptr)((_tmp65D=_tmp224,_tag_dyneither(_tmp65D,sizeof(char),_get_zero_arr_size_char((void*)_tmp65D,1)))),((_tmp65E.tag=0,((_tmp65E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp258),((_tmp65F.tag=0,((_tmp65F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp257),((_tmp660[0]=& _tmp65F,((_tmp660[1]=& _tmp65E,((_tmp660[2]=& _tmp65C,((_tmp660[3]=& _tmp65A,Cyc_aprintf(((_tmp661="%s %s -E -o %s -x c %s",_tag_dyneither(_tmp661,sizeof(char),23))),_tag_dyneither(_tmp660,sizeof(void*),4)))))))))))))))))))))))))),sizeof(char),1);}
# 1397
if(Cyc_verbose){
const char*_tmp667;void*_tmp666[1];char*_tmp665;struct Cyc_String_pa_PrintArg_struct _tmp664;(_tmp664.tag=0,((_tmp664.f1=(struct _dyneither_ptr)((_tmp665=cmd,_tag_dyneither(_tmp665,sizeof(char),_get_zero_arr_size_char((void*)_tmp665,1)))),((_tmp666[0]=& _tmp664,Cyc_fprintf(Cyc_stderr,((_tmp667="%s\n",_tag_dyneither(_tmp667,sizeof(char),4))),_tag_dyneither(_tmp666,sizeof(void*),1)))))));}
system((const char*)cmd);}};}}
# 1404
if(Cyc_gathering()){int _tmp275=0;_npop_handler(0);return _tmp275;}{
# 1407
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp223,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp678;const char*_tmp677;void*_tmp676[1];const char*_tmp675;struct Cyc_String_pa_PrintArg_struct _tmp674;struct Cyc_NO_SUPPORT_exn_struct*_tmp673;(int)_throw((void*)((_tmp673=_cycalloc(sizeof(*_tmp673)),((_tmp673[0]=((_tmp678.tag=Cyc_NO_SUPPORT,((_tmp678.f1=(struct _dyneither_ptr)((_tmp674.tag=0,((_tmp674.f1=(struct _dyneither_ptr)((_tmp675=_tmp223,_tag_dyneither(_tmp675,sizeof(char),_get_zero_arr_size_char((void*)_tmp675,1)))),((_tmp676[0]=& _tmp674,Cyc_aprintf(((_tmp677="can't open macrosfile %s",_tag_dyneither(_tmp677,sizeof(char),25))),_tag_dyneither(_tmp676,sizeof(void*),1)))))))),_tmp678)))),_tmp673)))));}
# 1412
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp27C=Cyc_Lexing_from_file(in_file);
struct _tuple20*entry;
while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp27C))!= 0){
struct _tuple20*_tmp27D=(struct _tuple20*)_check_null(entry);struct _dyneither_ptr*_tmp27F;struct Cyc_Set_Set*_tmp280;struct _tuple20*_tmp27E=_tmp27D;_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp27F,_tmp280);}
# 1421
Cyc_fclose(in_file);
# 1424
maybe=Cyc_fopen(_tmp224,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp689;const char*_tmp688;void*_tmp687[1];const char*_tmp686;struct Cyc_String_pa_PrintArg_struct _tmp685;struct Cyc_NO_SUPPORT_exn_struct*_tmp684;(int)_throw((void*)((_tmp684=_cycalloc(sizeof(*_tmp684)),((_tmp684[0]=((_tmp689.tag=Cyc_NO_SUPPORT,((_tmp689.f1=(struct _dyneither_ptr)((_tmp685.tag=0,((_tmp685.f1=(struct _dyneither_ptr)((_tmp686=_tmp224,_tag_dyneither(_tmp686,sizeof(char),_get_zero_arr_size_char((void*)_tmp686,1)))),((_tmp687[0]=& _tmp685,Cyc_aprintf(((_tmp688="can't open declsfile %s",_tag_dyneither(_tmp688,sizeof(char),24))),_tag_dyneither(_tmp687,sizeof(void*),1)))))))),_tmp689)))),_tmp684)))));}
# 1428
in_file=maybe;
_tmp27C=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp225,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp287=1;_npop_handler(0);return _tmp287;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp27C)){;}
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1438
maybe=Cyc_fopen(_tmp225,"r");
if(!((unsigned int)maybe)){int _tmp288=1;_npop_handler(0);return _tmp288;}
in_file=maybe;
{const char*_tmp68A;Cyc_Position_reset_position(((_tmp68A=_tmp225,_tag_dyneither(_tmp68A,sizeof(char),_get_zero_arr_size_char((void*)_tmp68A,1)))));}
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp28A=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1448
{struct Cyc_List_List*_tmp28B=_tmp28A;for(0;_tmp28B != 0;_tmp28B=_tmp28B->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp28B->hd,t);}}{
# 1452
struct Cyc_Set_Set*_tmp28C=Cyc_reachable(start_symbols,t);
# 1455
struct Cyc_List_List*_tmp28D=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp28E=_tmp28A;for(0;_tmp28E != 0;_tmp28E=_tmp28E->tl){
struct Cyc_Absyn_Decl*_tmp28F=(struct Cyc_Absyn_Decl*)_tmp28E->hd;
struct _dyneither_ptr*name;
{void*_tmp290=_tmp28F->r;void*_tmp291=_tmp290;struct Cyc_Absyn_Vardecl*_tmp293;struct Cyc_Absyn_Fndecl*_tmp295;struct Cyc_Absyn_Aggrdecl*_tmp297;struct Cyc_Absyn_Enumdecl*_tmp299;struct Cyc_Absyn_Typedefdecl*_tmp29B;_LL199: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp292=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp292->tag != 0)goto _LL19B;else{_tmp293=_tmp292->f1;}}_LL19A: {
# 1464
struct _tuple0*_tmp2A6=_tmp293->name;struct _dyneither_ptr*_tmp2A8;struct _tuple0*_tmp2A7=_tmp2A6;_tmp2A8=_tmp2A7->f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2A8);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp2A8))name=0;else{
name=_tmp2A8;}
goto _LL198;}_LL19B: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp294=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp294->tag != 1)goto _LL19D;else{_tmp295=_tmp294->f1;}}_LL19C: {
# 1470
struct _tuple0*_tmp2A9=_tmp295->name;struct _dyneither_ptr*_tmp2AB;struct _tuple0*_tmp2AA=_tmp2A9;_tmp2AB=_tmp2AA->f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2AB);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp2AB))name=0;else{
name=_tmp2AB;}
goto _LL198;}_LL19D: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp296=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp296->tag != 5)goto _LL19F;else{_tmp297=_tmp296->f1;}}_LL19E: {
# 1476
struct _tuple0*_tmp2AC=_tmp297->name;struct _dyneither_ptr*_tmp2AE;struct _tuple0*_tmp2AD=_tmp2AC;_tmp2AE=_tmp2AD->f2;
name=_tmp2AE;
goto _LL198;}_LL19F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp298=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp298->tag != 7)goto _LL1A1;else{_tmp299=_tmp298->f1;}}_LL1A0: {
# 1480
struct _tuple0*_tmp2AF=_tmp299->name;struct _dyneither_ptr*_tmp2B1;struct _tuple0*_tmp2B0=_tmp2AF;_tmp2B1=_tmp2B0->f2;
name=_tmp2B1;
# 1484
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp28C,name)){
struct Cyc_List_List*_tmp68B;_tmp28D=((_tmp68B=_cycalloc(sizeof(*_tmp68B)),((_tmp68B->hd=_tmp28F,((_tmp68B->tl=_tmp28D,_tmp68B))))));}else{
# 1487
if((unsigned int)_tmp299->fields){
struct Cyc_List_List*_tmp2B3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp299->fields))->v;for(0;_tmp2B3 != 0;_tmp2B3=_tmp2B3->tl){
struct Cyc_Absyn_Enumfield*_tmp2B4=(struct Cyc_Absyn_Enumfield*)_tmp2B3->hd;
struct _tuple0*_tmp2B5=_tmp2B4->name;struct _dyneither_ptr*_tmp2B7;struct _tuple0*_tmp2B6=_tmp2B5;_tmp2B7=_tmp2B6->f2;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp28C,_tmp2B7)){
{struct Cyc_List_List*_tmp68C;_tmp28D=((_tmp68C=_cycalloc(sizeof(*_tmp68C)),((_tmp68C->hd=_tmp28F,((_tmp68C->tl=_tmp28D,_tmp68C))))));}
break;}}}}
# 1497
name=0;
goto _LL198;}_LL1A1: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp29A=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp29A->tag != 8)goto _LL1A3;else{_tmp29B=_tmp29A->f1;}}_LL1A2: {
# 1500
struct _tuple0*_tmp2B9=_tmp29B->name;struct _dyneither_ptr*_tmp2BB;struct _tuple0*_tmp2BA=_tmp2B9;_tmp2BB=_tmp2BA->f2;
name=_tmp2BB;
goto _LL198;}_LL1A3: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp29C=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp29C->tag != 13)goto _LL1A5;}_LL1A4:
 goto _LL1A6;_LL1A5: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp29D=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp29D->tag != 14)goto _LL1A7;}_LL1A6:
 goto _LL1A8;_LL1A7: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp29E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp29E->tag != 2)goto _LL1A9;}_LL1A8:
 goto _LL1AA;_LL1A9: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp29F=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp29F->tag != 6)goto _LL1AB;}_LL1AA:
 goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp2A0=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp2A0->tag != 3)goto _LL1AD;}_LL1AC:
 goto _LL1AE;_LL1AD: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp2A1=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp2A1->tag != 9)goto _LL1AF;}_LL1AE:
 goto _LL1B0;_LL1AF: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp2A2=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp2A2->tag != 10)goto _LL1B1;}_LL1B0:
 goto _LL1B2;_LL1B1: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp2A3=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp2A3->tag != 11)goto _LL1B3;}_LL1B2:
 goto _LL1B4;_LL1B3: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp2A4=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp2A4->tag != 12)goto _LL1B5;}_LL1B4:
 goto _LL1B6;_LL1B5: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp2A5=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp291;if(_tmp2A5->tag != 4)goto _LL198;}_LL1B6:
# 1514
 name=0;
goto _LL198;_LL198:;}
# 1517
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp28C,name)){
struct Cyc_List_List*_tmp68D;_tmp28D=((_tmp68D=_cycalloc(sizeof(*_tmp68D)),((_tmp68D->hd=_tmp28F,((_tmp68D->tl=_tmp28D,_tmp68D))))));}}}
# 1522
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp2BD=1;_npop_handler(0);return _tmp2BD;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
const char*_tmp693;void*_tmp692[1];const char*_tmp691;struct Cyc_String_pa_PrintArg_struct _tmp690;struct _dyneither_ptr ifdefmacro=(_tmp690.tag=0,((_tmp690.f1=(struct _dyneither_ptr)((_tmp691=filename,_tag_dyneither(_tmp691,sizeof(char),_get_zero_arr_size_char((void*)_tmp691,1)))),((_tmp692[0]=& _tmp690,Cyc_aprintf(((_tmp693="_%s_",_tag_dyneither(_tmp693,sizeof(char),5))),_tag_dyneither(_tmp692,sizeof(void*),1)))))));
{int _tmp2BE=0;for(0;_tmp2BE < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp2BE){
if(((char*)ifdefmacro.curr)[_tmp2BE]== '.'  || ((char*)ifdefmacro.curr)[_tmp2BE]== '/'){
char _tmp696;char _tmp695;struct _dyneither_ptr _tmp694;(_tmp694=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp2BE),((_tmp695=*((char*)_check_dyneither_subscript(_tmp694,sizeof(char),0)),((_tmp696='_',((_get_dyneither_size(_tmp694,sizeof(char))== 1  && (_tmp695 == '\000'  && _tmp696 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp694.curr)=_tmp696)))))));}else{
if(((char*)ifdefmacro.curr)[_tmp2BE]!= '_'  && ((char*)ifdefmacro.curr)[_tmp2BE]!= '/'){
char _tmp699;char _tmp698;struct _dyneither_ptr _tmp697;(_tmp697=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp2BE),((_tmp698=*((char*)_check_dyneither_subscript(_tmp697,sizeof(char),0)),((_tmp699=(char)toupper((int)((char*)ifdefmacro.curr)[_tmp2BE]),((_get_dyneither_size(_tmp697,sizeof(char))== 1  && (_tmp698 == '\000'  && _tmp699 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp697.curr)=_tmp699)))))));}}}}
# 1534
{const char*_tmp69E;void*_tmp69D[2];struct Cyc_String_pa_PrintArg_struct _tmp69C;struct Cyc_String_pa_PrintArg_struct _tmp69B;(_tmp69B.tag=0,((_tmp69B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp69C.tag=0,((_tmp69C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp69D[0]=& _tmp69C,((_tmp69D[1]=& _tmp69B,Cyc_fprintf(out_file,((_tmp69E="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp69E,sizeof(char),23))),_tag_dyneither(_tmp69D,sizeof(void*),2)))))))))))));}{
# 1541
struct Cyc_List_List*_tmp2C9=0;
struct Cyc_List_List*_tmp2CA=0;
{struct Cyc_List_List*_tmp2CB=_tmp28D;for(0;_tmp2CB != 0;_tmp2CB=_tmp2CB->tl){
struct Cyc_Absyn_Decl*_tmp2CC=(struct Cyc_Absyn_Decl*)_tmp2CB->hd;
int _tmp2CD=0;
struct _dyneither_ptr*name;
{void*_tmp2CE=_tmp2CC->r;void*_tmp2CF=_tmp2CE;struct Cyc_Absyn_Vardecl*_tmp2D1;struct Cyc_Absyn_Fndecl*_tmp2D3;struct Cyc_Absyn_Aggrdecl*_tmp2D5;struct Cyc_Absyn_Enumdecl*_tmp2D7;struct Cyc_Absyn_Typedefdecl*_tmp2D9;_LL1B8: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2D0=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2D0->tag != 0)goto _LL1BA;else{_tmp2D1=_tmp2D0->f1;}}_LL1B9: {
# 1549
struct _tuple0*_tmp2E4=_tmp2D1->name;struct _dyneither_ptr*_tmp2E6;struct _tuple0*_tmp2E5=_tmp2E4;_tmp2E6=_tmp2E5->f2;
name=_tmp2E6;
goto _LL1B7;}_LL1BA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp2D2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2D2->tag != 1)goto _LL1BC;else{_tmp2D3=_tmp2D2->f1;}}_LL1BB: {
# 1553
struct _tuple0*_tmp2E7=_tmp2D3->name;struct _dyneither_ptr*_tmp2E9;struct _tuple0*_tmp2E8=_tmp2E7;_tmp2E9=_tmp2E8->f2;
name=_tmp2E9;
goto _LL1B7;}_LL1BC: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp2D4=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2D4->tag != 5)goto _LL1BE;else{_tmp2D5=_tmp2D4->f1;}}_LL1BD: {
# 1557
struct _tuple0*_tmp2EA=_tmp2D5->name;struct _dyneither_ptr*_tmp2EC;struct _tuple0*_tmp2EB=_tmp2EA;_tmp2EC=_tmp2EB->f2;
name=_tmp2EC;
goto _LL1B7;}_LL1BE: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp2D6=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2D6->tag != 7)goto _LL1C0;else{_tmp2D7=_tmp2D6->f1;}}_LL1BF: {
# 1561
struct _tuple0*_tmp2ED=_tmp2D7->name;struct _dyneither_ptr*_tmp2EF;struct _tuple0*_tmp2EE=_tmp2ED;_tmp2EF=_tmp2EE->f2;
name=_tmp2EF;
goto _LL1B7;}_LL1C0: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp2D8=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2D8->tag != 8)goto _LL1C2;else{_tmp2D9=_tmp2D8->f1;}}_LL1C1: {
# 1565
struct _tuple0*_tmp2F0=_tmp2D9->name;struct _dyneither_ptr*_tmp2F2;struct _tuple0*_tmp2F1=_tmp2F0;_tmp2F2=_tmp2F1->f2;
name=_tmp2F2;
goto _LL1B7;}_LL1C2: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp2DA=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2DA->tag != 4)goto _LL1C4;}_LL1C3:
 goto _LL1C5;_LL1C4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp2DB=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2DB->tag != 13)goto _LL1C6;}_LL1C5:
# 1570
 goto _LL1C7;_LL1C6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp2DC=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2DC->tag != 14)goto _LL1C8;}_LL1C7:
 goto _LL1C9;_LL1C8: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp2DD=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2DD->tag != 2)goto _LL1CA;}_LL1C9:
 goto _LL1CB;_LL1CA: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2DE=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2DE->tag != 6)goto _LL1CC;}_LL1CB:
 goto _LL1CD;_LL1CC: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp2DF=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2DF->tag != 3)goto _LL1CE;}_LL1CD:
 goto _LL1CF;_LL1CE: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp2E0=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2E0->tag != 9)goto _LL1D0;}_LL1CF:
 goto _LL1D1;_LL1D0: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp2E1=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2E1->tag != 10)goto _LL1D2;}_LL1D1:
 goto _LL1D3;_LL1D2: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp2E2=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2E2->tag != 11)goto _LL1D4;}_LL1D3:
 goto _LL1D5;_LL1D4: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp2E3=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CF;if(_tmp2E3->tag != 12)goto _LL1B7;}_LL1D5:
# 1579
 name=0;
goto _LL1B7;_LL1B7:;}
# 1582
if(!((unsigned int)name) && !_tmp2CD)continue;
{struct Cyc_List_List*_tmp69F;_tmp2C9=((_tmp69F=_cycalloc(sizeof(*_tmp69F)),((_tmp69F->hd=_tmp2CC,((_tmp69F->tl=_tmp2C9,_tmp69F))))));}{
struct Cyc_List_List*_tmp6A0;_tmp2CA=((_tmp6A0=_cycalloc(sizeof(*_tmp6A0)),((_tmp6A0->hd=name,((_tmp6A0->tl=_tmp2CA,_tmp6A0))))));};}}
# 1588
{struct _handler_cons _tmp2F5;_push_handler(& _tmp2F5);{int _tmp2F7=0;if(setjmp(_tmp2F5.handler))_tmp2F7=1;if(!_tmp2F7){
{struct _RegionHandle _tmp2F8=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp2F8;_push_region(tc_rgn);
{struct Cyc_Tcenv_Tenv*_tmp2F9=Cyc_Tcenv_tc_init(tc_rgn);
Cyc_Tc_tc(tc_rgn,_tmp2F9,1,_tmp2C9);}
# 1590
;_pop_region(tc_rgn);}
# 1589
;_pop_handler();}else{void*_tmp2F6=(void*)_exn_thrown;void*_tmp2FB=_tmp2F6;void*_tmp2FC;_LL1D7:;_LL1D8:
# 1595
{struct Cyc_NO_SUPPORT_exn_struct _tmp6A6;const char*_tmp6A5;struct Cyc_NO_SUPPORT_exn_struct*_tmp6A4;(int)_throw((void*)((_tmp6A4=_cycalloc(sizeof(*_tmp6A4)),((_tmp6A4[0]=((_tmp6A6.tag=Cyc_NO_SUPPORT,((_tmp6A6.f1=((_tmp6A5="can't typecheck acquired declarations",_tag_dyneither(_tmp6A5,sizeof(char),38))),_tmp6A6)))),_tmp6A4)))));}
goto _LL1D6;_LL1D9: _tmp2FC=_tmp2FB;_LL1DA:(void)_rethrow(_tmp2FC);_LL1D6:;}};}
# 1600
{struct _tuple25 _tmp6A7;struct _tuple25 _tmp300=(_tmp6A7.f1=_tmp2C9,((_tmp6A7.f2=_tmp2CA,_tmp6A7)));struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp303;struct _tuple25 _tmp301=_tmp300;_tmp302=_tmp301.f1;_tmp303=_tmp301.f2;for(0;
_tmp302 != 0  && _tmp303 != 0;(_tmp302=_tmp302->tl,_tmp303=_tmp303->tl)){
struct Cyc_Absyn_Decl*_tmp304=(struct Cyc_Absyn_Decl*)_tmp302->hd;
struct _dyneither_ptr*_tmp305=(struct _dyneither_ptr*)_tmp303->hd;
int _tmp306=0;
if(!((unsigned int)_tmp305))
_tmp306=1;
# 1609
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp305){
{const char*_tmp6AB;void*_tmp6AA[1];struct Cyc_String_pa_PrintArg_struct _tmp6A9;ifdefmacro=((_tmp6A9.tag=0,((_tmp6A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp305),((_tmp6AA[0]=& _tmp6A9,Cyc_aprintf(((_tmp6AB="_%s_def_",_tag_dyneither(_tmp6AB,sizeof(char),9))),_tag_dyneither(_tmp6AA,sizeof(void*),1))))))));}
{const char*_tmp6AF;void*_tmp6AE[1];struct Cyc_String_pa_PrintArg_struct _tmp6AD;(_tmp6AD.tag=0,((_tmp6AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp6AE[0]=& _tmp6AD,Cyc_fprintf(out_file,((_tmp6AF="#ifndef %s\n",_tag_dyneither(_tmp6AF,sizeof(char),12))),_tag_dyneither(_tmp6AE,sizeof(void*),1)))))));}
{const char*_tmp6B3;void*_tmp6B2[1];struct Cyc_String_pa_PrintArg_struct _tmp6B1;(_tmp6B1.tag=0,((_tmp6B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp6B2[0]=& _tmp6B1,Cyc_fprintf(out_file,((_tmp6B3="#define %s\n",_tag_dyneither(_tmp6B3,sizeof(char),12))),_tag_dyneither(_tmp6B2,sizeof(void*),1)))))));}
# 1615
{struct Cyc_Absyn_Decl*_tmp6B4[1];Cyc_Absynpp_decllist2file(((_tmp6B4[0]=_tmp304,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6B4,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}{
const char*_tmp6B7;void*_tmp6B6;(_tmp6B6=0,Cyc_fprintf(out_file,((_tmp6B7="#endif\n",_tag_dyneither(_tmp6B7,sizeof(char),8))),_tag_dyneither(_tmp6B6,sizeof(void*),0)));};}else{
# 1620
struct Cyc_Absyn_Decl*_tmp6B8[1];Cyc_Absynpp_decllist2file(((_tmp6B8[0]=_tmp304,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6B8,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}
# 1625
maybe=Cyc_fopen(_tmp223,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp6C9;const char*_tmp6C8;void*_tmp6C7[1];const char*_tmp6C6;struct Cyc_String_pa_PrintArg_struct _tmp6C5;struct Cyc_NO_SUPPORT_exn_struct*_tmp6C4;(int)_throw((void*)((_tmp6C4=_cycalloc(sizeof(*_tmp6C4)),((_tmp6C4[0]=((_tmp6C9.tag=Cyc_NO_SUPPORT,((_tmp6C9.f1=(struct _dyneither_ptr)((_tmp6C5.tag=0,((_tmp6C5.f1=(struct _dyneither_ptr)((_tmp6C6=_tmp223,_tag_dyneither(_tmp6C6,sizeof(char),_get_zero_arr_size_char((void*)_tmp6C6,1)))),((_tmp6C7[0]=& _tmp6C5,Cyc_aprintf(((_tmp6C8="can't open macrosfile %s",_tag_dyneither(_tmp6C8,sizeof(char),25))),_tag_dyneither(_tmp6C7,sizeof(void*),1)))))))),_tmp6C9)))),_tmp6C4)))));}
# 1628
in_file=maybe;
_tmp27C=Cyc_Lexing_from_file(in_file);{
struct _tuple21*entry2;
while((entry2=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp27C))!= 0){
struct _tuple21*_tmp31B=(struct _tuple21*)_check_null(entry2);struct _dyneither_ptr _tmp31D;struct _dyneither_ptr*_tmp31E;struct _tuple21*_tmp31C=_tmp31B;_tmp31D=_tmp31C->f1;_tmp31E=_tmp31C->f2;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp28C,_tmp31E)){
{const char*_tmp6CD;void*_tmp6CC[1];struct Cyc_String_pa_PrintArg_struct _tmp6CB;(_tmp6CB.tag=0,((_tmp6CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp31E),((_tmp6CC[0]=& _tmp6CB,Cyc_fprintf(out_file,((_tmp6CD="#ifndef %s\n",_tag_dyneither(_tmp6CD,sizeof(char),12))),_tag_dyneither(_tmp6CC,sizeof(void*),1)))))));}
{const char*_tmp6D1;void*_tmp6D0[1];struct Cyc_String_pa_PrintArg_struct _tmp6CF;(_tmp6CF.tag=0,((_tmp6CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31D),((_tmp6D0[0]=& _tmp6CF,Cyc_fprintf(out_file,((_tmp6D1="%s\n",_tag_dyneither(_tmp6D1,sizeof(char),4))),_tag_dyneither(_tmp6D0,sizeof(void*),1)))))));}{
const char*_tmp6D4;void*_tmp6D3;(_tmp6D3=0,Cyc_fprintf(out_file,((_tmp6D4="#endif\n",_tag_dyneither(_tmp6D4,sizeof(char),8))),_tag_dyneither(_tmp6D3,sizeof(void*),0)));};}}
# 1639
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1642
if(hstubs != 0){
struct Cyc_List_List*_tmp327=hstubs;for(0;_tmp327 != 0;_tmp327=_tmp327->tl){
struct _tuple22*_tmp328=(struct _tuple22*)_tmp327->hd;struct _dyneither_ptr _tmp32A;struct _dyneither_ptr _tmp32B;struct _tuple22*_tmp329=_tmp328;_tmp32A=_tmp329->f1;_tmp32B=_tmp329->f2;{
struct _dyneither_ptr*_tmp6D5;if((char*)_tmp32B.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp32A.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp6D5=_cycalloc(sizeof(*_tmp6D5)),((_tmp6D5[0]=_tmp32A,_tmp6D5)))))))
# 1648
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp32B,sizeof(char),1),out_file);else{
# 1650
const char*_tmp6D9;void*_tmp6D8[1];struct Cyc_String_pa_PrintArg_struct _tmp6D7;(_tmp6D7.tag=0,((_tmp6D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp6D8[0]=& _tmp6D7,Cyc_log(((_tmp6D9="%s is not supported on this platform\n",_tag_dyneither(_tmp6D9,sizeof(char),38))),_tag_dyneither(_tmp6D8,sizeof(void*),1)))))));}};}}
# 1653
{const char*_tmp6DC;void*_tmp6DB;(_tmp6DB=0,Cyc_fprintf(out_file,((_tmp6DC="#endif\n",_tag_dyneither(_tmp6DC,sizeof(char),8))),_tag_dyneither(_tmp6DB,sizeof(void*),0)));}
if(Cyc_do_setjmp){int _tmp332=0;_npop_handler(0);return _tmp332;}else{
Cyc_fclose(out_file);}
# 1658
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp333=cstubs;for(0;_tmp333 != 0;_tmp333=_tmp333->tl){
struct _tuple22*_tmp334=(struct _tuple22*)_tmp333->hd;struct _dyneither_ptr _tmp336;struct _dyneither_ptr _tmp337;struct _tuple22*_tmp335=_tmp334;_tmp336=_tmp335->f1;_tmp337=_tmp335->f2;{
struct _dyneither_ptr*_tmp6DD;if((char*)_tmp337.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp336.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD[0]=_tmp336,_tmp6DD)))))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp337,sizeof(char),1),out_file);};}};}
# 1669
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1671
{const char*_tmp6E3;void*_tmp6E2[1];const char*_tmp6E1;struct Cyc_String_pa_PrintArg_struct _tmp6E0;(_tmp6E0.tag=0,((_tmp6E0.f1=(struct _dyneither_ptr)((_tmp6E1=filename,_tag_dyneither(_tmp6E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp6E1,1)))),((_tmp6E2[0]=& _tmp6E0,Cyc_fprintf(out_file,((_tmp6E3="#include <%s>\n\n",_tag_dyneither(_tmp6E3,sizeof(char),16))),_tag_dyneither(_tmp6E2,sizeof(void*),1)))))));}
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp33D=cycstubs;for(0;_tmp33D != 0;_tmp33D=_tmp33D->tl){
struct _tuple22*_tmp33E=(struct _tuple22*)_tmp33D->hd;struct _dyneither_ptr _tmp340;struct _dyneither_ptr _tmp341;struct _tuple22*_tmp33F=_tmp33E;_tmp340=_tmp33F->f1;_tmp341=_tmp33F->f2;{
struct _dyneither_ptr*_tmp6E4;if((char*)_tmp341.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp340.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp6E4=_cycalloc(sizeof(*_tmp6E4)),((_tmp6E4[0]=_tmp340,_tmp6E4)))))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp341,sizeof(char),1),out_file);};}}{
# 1680
const char*_tmp6E7;void*_tmp6E6;(_tmp6E6=0,Cyc_fprintf(out_file,((_tmp6E7="\n",_tag_dyneither(_tmp6E7,sizeof(char),2))),_tag_dyneither(_tmp6E6,sizeof(void*),0)));};}{
# 1683
int _tmp345=0;_npop_handler(0);return _tmp345;};};};};};};};};
# 1352
;_pop_handler();}else{void*_tmp227=(void*)_exn_thrown;void*_tmp34B=_tmp227;struct _dyneither_ptr _tmp34D;struct _dyneither_ptr _tmp350;struct _dyneither_ptr _tmp352;struct _dyneither_ptr _tmp355;void*_tmp356;_LL1DC: {struct Cyc_Core_Impossible_exn_struct*_tmp34C=(struct Cyc_Core_Impossible_exn_struct*)_tmp34B;if(_tmp34C->tag != Cyc_Core_Impossible)goto _LL1DE;else{_tmp34D=_tmp34C->f1;}}_LL1DD:
# 1687
{const char*_tmp6EB;void*_tmp6EA[1];struct Cyc_String_pa_PrintArg_struct _tmp6E9;(_tmp6E9.tag=0,((_tmp6E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34D),((_tmp6EA[0]=& _tmp6E9,Cyc_fprintf(Cyc_stderr,((_tmp6EB="Got Core::Impossible(%s)\n",_tag_dyneither(_tmp6EB,sizeof(char),26))),_tag_dyneither(_tmp6EA,sizeof(void*),1)))))));}goto _LL1DB;_LL1DE: {struct Cyc_Dict_Absent_exn_struct*_tmp34E=(struct Cyc_Dict_Absent_exn_struct*)_tmp34B;if(_tmp34E->tag != Cyc_Dict_Absent)goto _LL1E0;}_LL1DF:
# 1689
{const char*_tmp6EE;void*_tmp6ED;(_tmp6ED=0,Cyc_fprintf(Cyc_stderr,((_tmp6EE="Got Dict::Absent\n",_tag_dyneither(_tmp6EE,sizeof(char),18))),_tag_dyneither(_tmp6ED,sizeof(void*),0)));}goto _LL1DB;_LL1E0: {struct Cyc_Core_Failure_exn_struct*_tmp34F=(struct Cyc_Core_Failure_exn_struct*)_tmp34B;if(_tmp34F->tag != Cyc_Core_Failure)goto _LL1E2;else{_tmp350=_tmp34F->f1;}}_LL1E1:
# 1691
{const char*_tmp6F2;void*_tmp6F1[1];struct Cyc_String_pa_PrintArg_struct _tmp6F0;(_tmp6F0.tag=0,((_tmp6F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp350),((_tmp6F1[0]=& _tmp6F0,Cyc_fprintf(Cyc_stderr,((_tmp6F2="Got Core::Failure(%s)\n",_tag_dyneither(_tmp6F2,sizeof(char),23))),_tag_dyneither(_tmp6F1,sizeof(void*),1)))))));}goto _LL1DB;_LL1E2: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp351=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp34B;if(_tmp351->tag != Cyc_Core_Invalid_argument)goto _LL1E4;else{_tmp352=_tmp351->f1;}}_LL1E3:
# 1693
{const char*_tmp6F6;void*_tmp6F5[1];struct Cyc_String_pa_PrintArg_struct _tmp6F4;(_tmp6F4.tag=0,((_tmp6F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp352),((_tmp6F5[0]=& _tmp6F4,Cyc_fprintf(Cyc_stderr,((_tmp6F6="Got Invalid_argument(%s)\n",_tag_dyneither(_tmp6F6,sizeof(char),26))),_tag_dyneither(_tmp6F5,sizeof(void*),1)))))));}goto _LL1DB;_LL1E4: {struct Cyc_Core_Not_found_exn_struct*_tmp353=(struct Cyc_Core_Not_found_exn_struct*)_tmp34B;if(_tmp353->tag != Cyc_Core_Not_found)goto _LL1E6;}_LL1E5:
# 1695
{const char*_tmp6F9;void*_tmp6F8;(_tmp6F8=0,Cyc_fprintf(Cyc_stderr,((_tmp6F9="Got Not_found\n",_tag_dyneither(_tmp6F9,sizeof(char),15))),_tag_dyneither(_tmp6F8,sizeof(void*),0)));}goto _LL1DB;_LL1E6: {struct Cyc_NO_SUPPORT_exn_struct*_tmp354=(struct Cyc_NO_SUPPORT_exn_struct*)_tmp34B;if(_tmp354->tag != Cyc_NO_SUPPORT)goto _LL1E8;else{_tmp355=_tmp354->f1;}}_LL1E7:
# 1697
{const char*_tmp6FD;void*_tmp6FC[1];struct Cyc_String_pa_PrintArg_struct _tmp6FB;(_tmp6FB.tag=0,((_tmp6FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp355),((_tmp6FC[0]=& _tmp6FB,Cyc_fprintf(Cyc_stderr,((_tmp6FD="No support because %s\n",_tag_dyneither(_tmp6FD,sizeof(char),23))),_tag_dyneither(_tmp6FC,sizeof(void*),1)))))));}goto _LL1DB;_LL1E8:;_LL1E9:
# 1699
{const char*_tmp700;void*_tmp6FF;(_tmp6FF=0,Cyc_fprintf(Cyc_stderr,((_tmp700="Got unknown exception\n",_tag_dyneither(_tmp700,sizeof(char),23))),_tag_dyneither(_tmp6FF,sizeof(void*),0)));}goto _LL1DB;_LL1EA: _tmp356=_tmp34B;_LL1EB:(void)_rethrow(_tmp356);_LL1DB:;}};}
# 1704
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
{const char*_tmp706;void*_tmp705[1];const char*_tmp704;struct Cyc_String_pa_PrintArg_struct _tmp703;(_tmp703.tag=0,((_tmp703.f1=(struct _dyneither_ptr)((_tmp704=filename,_tag_dyneither(_tmp704,sizeof(char),_get_zero_arr_size_char((void*)_tmp704,1)))),((_tmp705[0]=& _tmp703,Cyc_fprintf(Cyc_stderr,((_tmp706="Error: could not create file %s\n",_tag_dyneither(_tmp706,sizeof(char),33))),_tag_dyneither(_tmp705,sizeof(void*),1)))))));}
return 1;}
# 1709
out_file=maybe;
{const char*_tmp70C;void*_tmp70B[1];const char*_tmp70A;struct Cyc_String_pa_PrintArg_struct _tmp709;(_tmp709.tag=0,((_tmp709.f1=(struct _dyneither_ptr)(
# 1712
(_tmp70A=filename,_tag_dyneither(_tmp70A,sizeof(char),_get_zero_arr_size_char((void*)_tmp70A,1)))),((_tmp70B[0]=& _tmp709,Cyc_fprintf(out_file,((_tmp70C="#error -- %s is not supported on this platform\n",_tag_dyneither(_tmp70C,sizeof(char),48))),_tag_dyneither(_tmp70B,sizeof(void*),1)))))));}
Cyc_fclose(out_file);
{const char*_tmp712;void*_tmp711[1];const char*_tmp710;struct Cyc_String_pa_PrintArg_struct _tmp70F;(_tmp70F.tag=0,((_tmp70F.f1=(struct _dyneither_ptr)(
(_tmp710=filename,_tag_dyneither(_tmp710,sizeof(char),_get_zero_arr_size_char((void*)_tmp710,1)))),((_tmp711[0]=& _tmp70F,Cyc_fprintf(Cyc_stderr,((_tmp712="Warning: %s will not be supported on this platform\n",_tag_dyneither(_tmp712,sizeof(char),52))),_tag_dyneither(_tmp711,sizeof(void*),1)))))));}
{const char*_tmp715;void*_tmp714;(_tmp714=0,Cyc_log(((_tmp715="Not supported on this platform\n",_tag_dyneither(_tmp715,sizeof(char),32))),_tag_dyneither(_tmp714,sizeof(void*),0)));}
# 1723
return 0;};}int Cyc_process_specfile(const char*file,const char*dir);static void _tmp726(unsigned int*_tmp725,unsigned int*_tmp724,char**_tmp723){for(*_tmp725=0;*_tmp725 < *_tmp724;(*_tmp725)++){(*_tmp723)[*_tmp725]=(char)'\000';}}
# 1727
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose){
const char*_tmp71B;void*_tmp71A[1];const char*_tmp719;struct Cyc_String_pa_PrintArg_struct _tmp718;(_tmp718.tag=0,((_tmp718.f1=(struct _dyneither_ptr)((_tmp719=file,_tag_dyneither(_tmp719,sizeof(char),_get_zero_arr_size_char((void*)_tmp719,1)))),((_tmp71A[0]=& _tmp718,Cyc_fprintf(Cyc_stderr,((_tmp71B="Processing %s\n",_tag_dyneither(_tmp71B,sizeof(char),15))),_tag_dyneither(_tmp71A,sizeof(void*),1)))))));}{
struct Cyc___cycFILE*_tmp390=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp390)){
{const char*_tmp721;void*_tmp720[1];const char*_tmp71F;struct Cyc_String_pa_PrintArg_struct _tmp71E;(_tmp71E.tag=0,((_tmp71E.f1=(struct _dyneither_ptr)((_tmp71F=file,_tag_dyneither(_tmp71F,sizeof(char),_get_zero_arr_size_char((void*)_tmp71F,1)))),((_tmp720[0]=& _tmp71E,Cyc_fprintf(Cyc_stderr,((_tmp721="Error: could not open %s\n",_tag_dyneither(_tmp721,sizeof(char),26))),_tag_dyneither(_tmp720,sizeof(void*),1)))))));}
return 1;}{
# 1735
struct Cyc___cycFILE*_tmp395=_tmp390;
# 1739
char*_tmp72B;unsigned int _tmp72A;char*_tmp729;unsigned int _tmp728;unsigned int _tmp727;struct _dyneither_ptr buf=(_tmp72B=((_tmp72A=(unsigned int)1024,((_tmp729=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp72A + 1)),((((_tmp728=_tmp72A,((_tmp726(& _tmp727,& _tmp728,& _tmp729),_tmp729[_tmp728]=(char)0)))),_tmp729)))))),_tag_dyneither(_tmp72B,sizeof(char),_get_zero_arr_size_char((void*)_tmp72B,(unsigned int)1024 + 1)));
struct _dyneither_ptr _tmp396=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir(dir)){
{const char*_tmp731;void*_tmp730[1];const char*_tmp72F;struct Cyc_String_pa_PrintArg_struct _tmp72E;(_tmp72E.tag=0,((_tmp72E.f1=(struct _dyneither_ptr)((_tmp72F=dir,_tag_dyneither(_tmp72F,sizeof(char),_get_zero_arr_size_char((void*)_tmp72F,1)))),((_tmp730[0]=& _tmp72E,Cyc_fprintf(Cyc_stderr,((_tmp731="Error: can't change directory to %s\n",_tag_dyneither(_tmp731,sizeof(char),37))),_tag_dyneither(_tmp730,sizeof(void*),1)))))));}
return 1;}}
# 1747
if(Cyc_mode == Cyc_GATHER){
# 1749
struct _dyneither_ptr _tmp39B=Cstring_to_string(Ccomp);
const char*_tmp735;void*_tmp734[1];struct Cyc_String_pa_PrintArg_struct _tmp733;struct _dyneither_ptr _tmp39C=(_tmp733.tag=0,((_tmp733.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39B),((_tmp734[0]=& _tmp733,Cyc_aprintf(((_tmp735="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp735,sizeof(char),36))),_tag_dyneither(_tmp734,sizeof(void*),1)))))));
if(Cyc_verbose){
const char*_tmp739;void*_tmp738[1];struct Cyc_String_pa_PrintArg_struct _tmp737;(_tmp737.tag=0,((_tmp737.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39C),((_tmp738[0]=& _tmp737,Cyc_fprintf(Cyc_stderr,((_tmp739="%s\n",_tag_dyneither(_tmp739,sizeof(char),4))),_tag_dyneither(_tmp738,sizeof(void*),1)))))));}
system((const char*)_untag_dyneither_ptr(_tmp39C,sizeof(char),1));}{
# 1756
struct Cyc_Lexing_lexbuf*_tmp3A3=Cyc_Lexing_from_file(_tmp395);
struct _tuple23*entry;
while((entry=((struct _tuple23*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp3A3))!= 0){
struct _tuple23*_tmp3A4=(struct _tuple23*)_check_null(entry);struct _dyneither_ptr _tmp3A6;struct Cyc_List_List*_tmp3A7;struct Cyc_List_List*_tmp3A8;struct Cyc_List_List*_tmp3A9;struct Cyc_List_List*_tmp3AA;struct Cyc_List_List*_tmp3AB;struct _tuple23*_tmp3A5=_tmp3A4;_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;_tmp3A8=_tmp3A5->f3;_tmp3A9=_tmp3A5->f4;_tmp3AA=_tmp3A5->f5;_tmp3AB=_tmp3A5->f6;
# 1761
if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp3A6,sizeof(char),1),_tmp3A7,_tmp3A8,_tmp3A9,_tmp3AA,_tmp3AB))
# 1763
return 1;}
# 1765
Cyc_fclose(_tmp395);
# 1767
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp396,sizeof(char),1)))){
{const char*_tmp73D;void*_tmp73C[1];struct Cyc_String_pa_PrintArg_struct _tmp73B;(_tmp73B.tag=0,((_tmp73B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp396),((_tmp73C[0]=& _tmp73B,Cyc_fprintf(Cyc_stderr,((_tmp73D="Error: could not change directory to %s\n",_tag_dyneither(_tmp73D,sizeof(char),41))),_tag_dyneither(_tmp73C,sizeof(void*),1)))))));}
return 1;}}
# 1773
return 0;};};};}int Cyc_process_setjmp(const char*dir);static void _tmp742(unsigned int*_tmp741,unsigned int*_tmp740,char**_tmp73F){for(*_tmp741=0;*_tmp741 < *_tmp740;(*_tmp741)++){(*_tmp73F)[*_tmp741]=(char)'\000';}}
# 1777
int Cyc_process_setjmp(const char*dir){
# 1780
char*_tmp747;unsigned int _tmp746;char*_tmp745;unsigned int _tmp744;unsigned int _tmp743;struct _dyneither_ptr buf=(_tmp747=((_tmp746=(unsigned int)1024,((_tmp745=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp746 + 1)),((((_tmp744=_tmp746,((_tmp742(& _tmp743,& _tmp744,& _tmp745),_tmp745[_tmp744]=(char)0)))),_tmp745)))))),_tag_dyneither(_tmp747,sizeof(char),_get_zero_arr_size_char((void*)_tmp747,(unsigned int)1024 + 1)));
struct _dyneither_ptr _tmp3B3=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(chdir(dir)){
{const char*_tmp74D;void*_tmp74C[1];const char*_tmp74B;struct Cyc_String_pa_PrintArg_struct _tmp74A;(_tmp74A.tag=0,((_tmp74A.f1=(struct _dyneither_ptr)((_tmp74B=dir,_tag_dyneither(_tmp74B,sizeof(char),_get_zero_arr_size_char((void*)_tmp74B,1)))),((_tmp74C[0]=& _tmp74A,Cyc_fprintf(Cyc_stderr,((_tmp74D="Error: can't change directory to %s\n",_tag_dyneither(_tmp74D,sizeof(char),37))),_tag_dyneither(_tmp74C,sizeof(void*),1)))))));}
return 1;}
# 1786
{struct _tuple22*_tmp757;const char*_tmp756;const char*_tmp755;struct _tuple22*_tmp754[1];struct _dyneither_ptr*_tmp74E[1];if(Cyc_process_file("setjmp.h",((_tmp74E[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp74E,sizeof(struct _dyneither_ptr*),1)))),0,(
(_tmp754[0]=((_tmp757=_cycalloc(sizeof(*_tmp757)),((_tmp757->f1=((_tmp756="setjmp",_tag_dyneither(_tmp756,sizeof(char),7))),((_tmp757->f2=((_tmp755="extern int setjmp(jmp_buf);\n",_tag_dyneither(_tmp755,sizeof(char),29))),_tmp757)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp754,sizeof(struct _tuple22*),1)))),0,0))
# 1789
return 1;}
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp3B3,sizeof(char),1)))){
{const char*_tmp75B;void*_tmp75A[1];struct Cyc_String_pa_PrintArg_struct _tmp759;(_tmp759.tag=0,((_tmp759.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B3),((_tmp75A[0]=& _tmp759,Cyc_fprintf(Cyc_stderr,((_tmp75B="Error: could not change directory to %s\n",_tag_dyneither(_tmp75B,sizeof(char),41))),_tag_dyneither(_tmp75A,sizeof(void*),1)))))));}
return 1;}
# 1794
return 0;}static char _tmp3C5[13]="BUILDLIB.OUT";
# 1798
static struct _dyneither_ptr Cyc_output_dir={_tmp3C5,_tmp3C5,_tmp3C5 + 13};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 1802
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
struct Cyc_List_List*_tmp75C;Cyc_spec_files=((_tmp75C=_cycalloc(sizeof(*_tmp75C)),((_tmp75C->hd=(const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp75C->tl=Cyc_spec_files,_tmp75C))))));}
# 1806
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 1809
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1812
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 1815
static void Cyc_add_cpparg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp75F;struct Cyc_List_List*_tmp75E;Cyc_cppargs=((_tmp75E=_cycalloc(sizeof(*_tmp75E)),((_tmp75E->hd=((_tmp75F=_cycalloc(sizeof(*_tmp75F)),((_tmp75F[0]=s,_tmp75F)))),((_tmp75E->tl=Cyc_cppargs,_tmp75E))))));}
# 1818
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
{const char*_tmp763;void*_tmp762[1];struct Cyc_String_pa_PrintArg_struct _tmp761;(_tmp761.tag=0,((_tmp761.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp762[0]=& _tmp761,Cyc_fprintf(Cyc_stderr,((_tmp763="Unsupported option %s\n",_tag_dyneither(_tmp763,sizeof(char),23))),_tag_dyneither(_tmp762,sizeof(void*),1)))))));}
Cyc_badparse=1;}
# 1823
static void Cyc_set_machine(struct _dyneither_ptr s){
{const char*_tmp764;Cyc_add_cpparg(((_tmp764="-b",_tag_dyneither(_tmp764,sizeof(char),3))));}
Cyc_add_cpparg(s);}
# 1831
void GC_blacklist_warn_clear();struct _tuple26{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
GC_blacklist_warn_clear();{
# 1835
struct _tuple26*_tmp7F5;const char*_tmp7F4;const char*_tmp7F3;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp7F2;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp7F1;const char*_tmp7F0;struct _tuple26*_tmp7EF;const char*_tmp7EE;const char*_tmp7ED;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp7EC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp7EB;const char*_tmp7EA;struct _tuple26*_tmp7E9;const char*_tmp7E8;const char*_tmp7E7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp7E6;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp7E5;const char*_tmp7E4;struct _tuple26*_tmp7E3;const char*_tmp7E2;const char*_tmp7E1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp7E0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp7DF;const char*_tmp7DE;struct _tuple26*_tmp7DD;const char*_tmp7DC;const char*_tmp7DB;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp7DA;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp7D9;const char*_tmp7D8;struct _tuple26*_tmp7D7;const char*_tmp7D6;const char*_tmp7D5;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp7D4;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp7D3;const char*_tmp7D2;struct _tuple26*_tmp7D1;const char*_tmp7D0;const char*_tmp7CF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp7CE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp7CD;const char*_tmp7CC;struct _tuple26*_tmp7CB;const char*_tmp7CA;const char*_tmp7C9;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp7C8;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp7C7;const char*_tmp7C6;struct _tuple26*_tmp7C5[8];struct Cyc_List_List*options=
(_tmp7C5[7]=(
# 1860
(_tmp7CB=_cycalloc(sizeof(*_tmp7CB)),((_tmp7CB->f1=((_tmp7CA="-",_tag_dyneither(_tmp7CA,sizeof(char),2))),((_tmp7CB->f2=1,((_tmp7CB->f3=((_tmp7C9="",_tag_dyneither(_tmp7C9,sizeof(char),1))),((_tmp7CB->f4=(void*)(
(_tmp7C7=_cycalloc(sizeof(*_tmp7C7)),((_tmp7C7[0]=((_tmp7C8.tag=1,((_tmp7C8.f1=Cyc_add_cpparg,_tmp7C8)))),_tmp7C7)))),((_tmp7CB->f5=(
(_tmp7C6="",_tag_dyneither(_tmp7C6,sizeof(char),1))),_tmp7CB)))))))))))),((_tmp7C5[6]=(
# 1857
(_tmp7D1=_cycalloc(sizeof(*_tmp7D1)),((_tmp7D1->f1=((_tmp7D0="-v",_tag_dyneither(_tmp7D0,sizeof(char),3))),((_tmp7D1->f2=0,((_tmp7D1->f3=((_tmp7CF="",_tag_dyneither(_tmp7CF,sizeof(char),1))),((_tmp7D1->f4=(void*)(
(_tmp7CD=_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD[0]=((_tmp7CE.tag=3,((_tmp7CE.f1=& Cyc_verbose,_tmp7CE)))),_tmp7CD)))),((_tmp7D1->f5=(
(_tmp7CC="Verbose operation",_tag_dyneither(_tmp7CC,sizeof(char),18))),_tmp7D1)))))))))))),((_tmp7C5[5]=(
# 1854
(_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7->f1=((_tmp7D6="-b",_tag_dyneither(_tmp7D6,sizeof(char),3))),((_tmp7D7->f2=0,((_tmp7D7->f3=((_tmp7D5=" <machine>",_tag_dyneither(_tmp7D5,sizeof(char),11))),((_tmp7D7->f4=(void*)(
(_tmp7D3=_cycalloc(sizeof(*_tmp7D3)),((_tmp7D3[0]=((_tmp7D4.tag=5,((_tmp7D4.f1=Cyc_set_machine,_tmp7D4)))),_tmp7D3)))),((_tmp7D7->f5=(
(_tmp7D2="Set the target machine for compilation to <machine>",_tag_dyneither(_tmp7D2,sizeof(char),52))),_tmp7D7)))))))))))),((_tmp7C5[4]=(
# 1848
(_tmp7DD=_cycalloc(sizeof(*_tmp7DD)),((_tmp7DD->f1=((_tmp7DC="-setjmp",_tag_dyneither(_tmp7DC,sizeof(char),8))),((_tmp7DD->f2=0,((_tmp7DD->f3=((_tmp7DB="",_tag_dyneither(_tmp7DB,sizeof(char),1))),((_tmp7DD->f4=(void*)(
(_tmp7D9=_cycalloc(sizeof(*_tmp7D9)),((_tmp7D9[0]=((_tmp7DA.tag=3,((_tmp7DA.f1=& Cyc_do_setjmp,_tmp7DA)))),_tmp7D9)))),((_tmp7DD->f5=(
# 1853
(_tmp7D8="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",_tag_dyneither(_tmp7D8,sizeof(char),186))),_tmp7DD)))))))))))),((_tmp7C5[3]=(
# 1845
(_tmp7E3=_cycalloc(sizeof(*_tmp7E3)),((_tmp7E3->f1=((_tmp7E2="-finish",_tag_dyneither(_tmp7E2,sizeof(char),8))),((_tmp7E3->f2=0,((_tmp7E3->f3=((_tmp7E1="",_tag_dyneither(_tmp7E1,sizeof(char),1))),((_tmp7E3->f4=(void*)(
(_tmp7DF=_cycalloc(sizeof(*_tmp7DF)),((_tmp7DF[0]=((_tmp7E0.tag=0,((_tmp7E0.f1=Cyc_set_FINISH,_tmp7E0)))),_tmp7DF)))),((_tmp7E3->f5=(
(_tmp7DE="Produce Cyclone headers from pre-gathered C library info",_tag_dyneither(_tmp7DE,sizeof(char),57))),_tmp7E3)))))))))))),((_tmp7C5[2]=(
# 1842
(_tmp7E9=_cycalloc(sizeof(*_tmp7E9)),((_tmp7E9->f1=((_tmp7E8="-gatherscript",_tag_dyneither(_tmp7E8,sizeof(char),14))),((_tmp7E9->f2=0,((_tmp7E9->f3=((_tmp7E7="",_tag_dyneither(_tmp7E7,sizeof(char),1))),((_tmp7E9->f4=(void*)(
(_tmp7E5=_cycalloc(sizeof(*_tmp7E5)),((_tmp7E5[0]=((_tmp7E6.tag=0,((_tmp7E6.f1=Cyc_set_GATHERSCRIPT,_tmp7E6)))),_tmp7E5)))),((_tmp7E9->f5=(
(_tmp7E4="Produce a script to gather C library info",_tag_dyneither(_tmp7E4,sizeof(char),42))),_tmp7E9)))))))))))),((_tmp7C5[1]=(
# 1839
(_tmp7EF=_cycalloc(sizeof(*_tmp7EF)),((_tmp7EF->f1=((_tmp7EE="-gather",_tag_dyneither(_tmp7EE,sizeof(char),8))),((_tmp7EF->f2=0,((_tmp7EF->f3=((_tmp7ED="",_tag_dyneither(_tmp7ED,sizeof(char),1))),((_tmp7EF->f4=(void*)(
(_tmp7EB=_cycalloc(sizeof(*_tmp7EB)),((_tmp7EB[0]=((_tmp7EC.tag=0,((_tmp7EC.f1=Cyc_set_GATHER,_tmp7EC)))),_tmp7EB)))),((_tmp7EF->f5=(
(_tmp7EA="Gather C library info but don't produce Cyclone headers",_tag_dyneither(_tmp7EA,sizeof(char),56))),_tmp7EF)))))))))))),((_tmp7C5[0]=(
# 1836
(_tmp7F5=_cycalloc(sizeof(*_tmp7F5)),((_tmp7F5->f1=((_tmp7F4="-d",_tag_dyneither(_tmp7F4,sizeof(char),3))),((_tmp7F5->f2=0,((_tmp7F5->f3=((_tmp7F3=" <file>",_tag_dyneither(_tmp7F3,sizeof(char),8))),((_tmp7F5->f4=(void*)(
(_tmp7F1=_cycalloc(sizeof(*_tmp7F1)),((_tmp7F1[0]=((_tmp7F2.tag=5,((_tmp7F2.f1=Cyc_set_output_dir,_tmp7F2)))),_tmp7F1)))),((_tmp7F5->f5=(
(_tmp7F0="Set the output directory to <file>",_tag_dyneither(_tmp7F0,sizeof(char),35))),_tmp7F5)))))))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7C5,sizeof(struct _tuple26*),8)))))))))))))))));
# 1864
{const char*_tmp7F6;Cyc_Arg_parse(options,Cyc_add_spec_file,((_tmp7F6="Options:",_tag_dyneither(_tmp7F6,sizeof(char),9))),argv);}
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp7F7;Cyc_Arg_usage(options,(
# 1872
(_tmp7F7="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",_tag_dyneither(_tmp7F7,sizeof(char),59))));}
return 1;}
# 1876
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose){
const char*_tmp7FA;void*_tmp7F9;(_tmp7F9=0,Cyc_fprintf(Cyc_stderr,((_tmp7FA="Creating BUILDLIB.sh\n",_tag_dyneither(_tmp7FA,sizeof(char),22))),_tag_dyneither(_tmp7F9,sizeof(void*),0)));}
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
{const char*_tmp7FD;void*_tmp7FC;(_tmp7FC=0,Cyc_fprintf(Cyc_stderr,((_tmp7FD="Could not create file BUILDLIB.sh\n",_tag_dyneither(_tmp7FD,sizeof(char),35))),_tag_dyneither(_tmp7FC,sizeof(void*),0)));}
exit(1);}
# 1884
{const char*_tmp800;void*_tmp7FF;(_tmp7FF=0,Cyc_prscript(((_tmp800="#!/bin/sh\n",_tag_dyneither(_tmp800,sizeof(char),11))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}{
const char*_tmp803;void*_tmp802;(_tmp802=0,Cyc_prscript(((_tmp803="GCC=\"gcc\"\n",_tag_dyneither(_tmp803,sizeof(char),11))),_tag_dyneither(_tmp802,sizeof(void*),0)));};}
# 1889
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
{const char*_tmp807;void*_tmp806[1];struct Cyc_String_pa_PrintArg_struct _tmp805;(_tmp805.tag=0,((_tmp805.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp806[0]=& _tmp805,Cyc_fprintf(Cyc_stderr,((_tmp807="Error: could not create directory %s\n",_tag_dyneither(_tmp807,sizeof(char),38))),_tag_dyneither(_tmp806,sizeof(void*),1)))))));}
return 1;}
# 1893
if(Cyc_verbose){
const char*_tmp80B;void*_tmp80A[1];struct Cyc_String_pa_PrintArg_struct _tmp809;(_tmp809.tag=0,((_tmp809.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp80A[0]=& _tmp809,Cyc_fprintf(Cyc_stderr,((_tmp80B="Output directory is %s\n",_tag_dyneither(_tmp80B,sizeof(char),24))),_tag_dyneither(_tmp80A,sizeof(void*),1)))))));}
# 1896
if(Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp80F;void*_tmp80E[1];struct Cyc_String_pa_PrintArg_struct _tmp80D;(_tmp80D.tag=0,((_tmp80D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp80E[0]=& _tmp80D,Cyc_prscript(((_tmp80F="cd %s\n",_tag_dyneither(_tmp80F,sizeof(char),7))),_tag_dyneither(_tmp80E,sizeof(void*),1)))))));}{
const char*_tmp812;void*_tmp811;(_tmp811=0,Cyc_prscript(((_tmp812="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp812,sizeof(char),38))),_tag_dyneither(_tmp811,sizeof(void*),0)));};}
# 1901
if(!Cyc_gathering()){
# 1904
{const char*_tmp813;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp813="BUILDLIB.LOG",_tag_dyneither(_tmp813,sizeof(char),13)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_log_file)){
{const char*_tmp817;void*_tmp816[1];struct Cyc_String_pa_PrintArg_struct _tmp815;(_tmp815.tag=0,((_tmp815.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp816[0]=& _tmp815,Cyc_fprintf(Cyc_stderr,((_tmp817="Error: could not create log file in directory %s\n",_tag_dyneither(_tmp817,sizeof(char),50))),_tag_dyneither(_tmp816,sizeof(void*),1)))))));}
return 1;}
# 1910
if(!Cyc_do_setjmp){
# 1912
{const char*_tmp818;Cyc_cstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp818="cstubs.c",_tag_dyneither(_tmp818,sizeof(char),9)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_cstubs_file)){
{const char*_tmp81C;void*_tmp81B[1];struct Cyc_String_pa_PrintArg_struct _tmp81A;(_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp81B[0]=& _tmp81A,Cyc_fprintf(Cyc_stderr,((_tmp81C="Error: could not create cstubs.c in directory %s\n",_tag_dyneither(_tmp81C,sizeof(char),50))),_tag_dyneither(_tmp81B,sizeof(void*),1)))))));}
return 1;}
# 1919
{const char*_tmp81D;Cyc_cycstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp81D="cycstubs.cyc",_tag_dyneither(_tmp81D,sizeof(char),13)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_cycstubs_file)){
{const char*_tmp821;void*_tmp820[1];struct Cyc_String_pa_PrintArg_struct _tmp81F;(_tmp81F.tag=0,((_tmp81F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp820[0]=& _tmp81F,Cyc_fprintf(Cyc_stderr,((_tmp821="Error: could not create cycstubs.c in directory %s\n",_tag_dyneither(_tmp821,sizeof(char),52))),_tag_dyneither(_tmp820,sizeof(void*),1)))))));}
# 1924
return 1;}{
# 1926
const char*_tmp824;void*_tmp823;(_tmp823=0,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),((_tmp824="#include <core.h>\nusing Core;\n\n",_tag_dyneither(_tmp824,sizeof(char),32))),_tag_dyneither(_tmp823,sizeof(void*),0)));};}}{
# 1933
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 1939
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
{const char*_tmp827;void*_tmp826;(_tmp826=0,Cyc_fprintf(Cyc_stderr,((_tmp827="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp827,sizeof(char),22))),_tag_dyneither(_tmp826,sizeof(void*),0)));}
exit(1);}}}
# 1947
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 1950
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 1958
return 0;};};}
