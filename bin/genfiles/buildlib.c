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
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1137 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1200
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
{const char*_tmp3B2;void*_tmp3B1;(_tmp3B1=0,Cyc_fprintf(Cyc_stderr,((_tmp3B2="Internal error: log file is NULL\n",_tag_dyneither(_tmp3B2,sizeof(char),34))),_tag_dyneither(_tmp3B1,sizeof(void*),0)));}
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
struct Cyc_Set_Set**_tmp3B3;Cyc_current_targets=((_tmp3B3=_cycalloc(sizeof(*_tmp3B3)),((_tmp3B3[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr),_tmp3B3))));}struct _tuple20{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
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
struct Cyc_Lexing_Error_exn_struct _tmp3B9;const char*_tmp3B8;struct Cyc_Lexing_Error_exn_struct*_tmp3B7;(int)_throw((void*)((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=((_tmp3B9.tag=Cyc_Lexing_Error,((_tmp3B9.f1=((_tmp3B8="empty token",_tag_dyneither(_tmp3B8,sizeof(char),12))),_tmp3B9)))),_tmp3B7)))));}else{
# 209
return lbuf->lex_last_action;}}else{
# 212
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 216
struct _tuple20*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7=lexstate;switch(_tmp7){case 0: _LL1: _LL2:
# 175 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
struct Cyc_Set_Set**_tmp3BA;Cyc_current_targets=((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp3BA))));}{
# 180
struct _tuple20*_tmp3BB;return(_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp3BB->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp3BB)))));};case 1: _LL3: _LL4:
# 183 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL5: _LL6:
# 185
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}{
# 189
struct Cyc_Lexing_Error_exn_struct _tmp3C1;const char*_tmp3C0;struct Cyc_Lexing_Error_exn_struct*_tmp3BF;(int)_throw((void*)((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF[0]=((_tmp3C1.tag=Cyc_Lexing_Error,((_tmp3C1.f1=((_tmp3C0="some action didn't return!",_tag_dyneither(_tmp3C0,sizeof(char),27))),_tmp3C1)))),_tmp3BF)))));};}
# 191
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpD=lexstate;switch(_tmpD){case 0: _LLA: _LLB:
# 189 "buildlib.cyl"
{struct _dyneither_ptr*_tmp3C2;Cyc_current_source=((_tmp3C2=_cycalloc(sizeof(*_tmp3C2)),((_tmp3C2[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3C2))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp3C3;Cyc_current_targets=((_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3C3))));}
Cyc_token(lexbuf);
return 0;case 1: _LLC: _LLD:
# 197
{struct _dyneither_ptr*_tmp3C4;Cyc_current_source=((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3C4))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp3C5;Cyc_current_targets=((_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3C5))));}
Cyc_args(lexbuf);
return 0;case 2: _LLE: _LLF:
# 205
{struct _dyneither_ptr*_tmp3C6;Cyc_current_source=((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3C6))));}
Cyc_current_args=0;
{struct Cyc_Set_Set**_tmp3C7;Cyc_current_targets=((_tmp3C7=_cycalloc(sizeof(*_tmp3C7)),((_tmp3C7[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3C7))));}
Cyc_token(lexbuf);
return 0;default: _LL10: _LL11:
# 211
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL9:;}{
# 214
struct Cyc_Lexing_Error_exn_struct _tmp3CD;const char*_tmp3CC;struct Cyc_Lexing_Error_exn_struct*_tmp3CB;(int)_throw((void*)((_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB[0]=((_tmp3CD.tag=Cyc_Lexing_Error,((_tmp3CD.f1=((_tmp3CC="some action didn't return!",_tag_dyneither(_tmp3CC,sizeof(char),27))),_tmp3CD)))),_tmp3CB)))));};}
# 216
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp17=lexstate;switch(_tmp17){case 0: _LL13: _LL14: {
# 214 "buildlib.cyl"
struct _dyneither_ptr*_tmp3CE;struct _dyneither_ptr*_tmp18=(_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3CE)));
{struct Cyc_List_List*_tmp3CF;Cyc_current_args=((_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF->hd=_tmp18,((_tmp3CF->tl=Cyc_current_args,_tmp3CF))))));}
return Cyc_args(lexbuf);}case 1: _LL15: _LL16: {
# 220
struct _dyneither_ptr*_tmp3D0;struct _dyneither_ptr*_tmp1B=(_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3D0)));
{struct Cyc_List_List*_tmp3D1;Cyc_current_args=((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1->hd=_tmp1B,((_tmp3D1->tl=Cyc_current_args,_tmp3D1))))));}
return Cyc_args(lexbuf);}case 2: _LL17: _LL18: {
# 226
struct _dyneither_ptr*_tmp3D2;struct _dyneither_ptr*_tmp1E=(_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3D2)));
{struct Cyc_List_List*_tmp3D3;Cyc_current_args=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->hd=_tmp1E,((_tmp3D3->tl=Cyc_current_args,_tmp3D3))))));}
return Cyc_token(lexbuf);}default: _LL19: _LL1A:
# 231
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL12:;}{
# 234
struct Cyc_Lexing_Error_exn_struct _tmp3D9;const char*_tmp3D8;struct Cyc_Lexing_Error_exn_struct*_tmp3D7;(int)_throw((void*)((_tmp3D7=_cycalloc(sizeof(*_tmp3D7)),((_tmp3D7[0]=((_tmp3D9.tag=Cyc_Lexing_Error,((_tmp3D9.f1=((_tmp3D8="some action didn't return!",_tag_dyneither(_tmp3D8,sizeof(char),27))),_tmp3D9)))),_tmp3D7)))));};}
# 236
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp24=lexstate;switch(_tmp24){case 0: _LL1C: _LL1D:
# 235 "buildlib.cyl"
{struct _dyneither_ptr*_tmp3DA;Cyc_add_target(((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3DA)))));}return Cyc_token(lexbuf);case 1: _LL1E: _LL1F:
# 238 "buildlib.cyl"
 return 0;case 2: _LL20: _LL21:
# 241 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3: _LL22: _LL23:
# 244 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL24: _LL25:
# 247 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5: _LL26: _LL27:
# 249
 return Cyc_token(lexbuf);case 6: _LL28: _LL29:
# 251
 return Cyc_token(lexbuf);case 7: _LL2A: _LL2B:
# 253
 return Cyc_token(lexbuf);case 8: _LL2C: _LL2D:
# 256 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9: _LL2E: _LL2F:
# 259 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10: _LL30: _LL31:
# 262 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11: _LL32: _LL33:
# 264
 return Cyc_token(lexbuf);case 12: _LL34: _LL35:
# 266
 return Cyc_token(lexbuf);case 13: _LL36: _LL37:
# 268
 return Cyc_token(lexbuf);case 14: _LL38: _LL39:
# 270
 return Cyc_token(lexbuf);case 15: _LL3A: _LL3B:
# 272
 return Cyc_token(lexbuf);case 16: _LL3C: _LL3D:
# 274
 return Cyc_token(lexbuf);case 17: _LL3E: _LL3F:
# 276
 return Cyc_token(lexbuf);case 18: _LL40: _LL41:
# 278
 return Cyc_token(lexbuf);case 19: _LL42: _LL43:
# 280
 return Cyc_token(lexbuf);case 20: _LL44: _LL45:
# 282
 return Cyc_token(lexbuf);case 21: _LL46: _LL47:
# 284
 return Cyc_token(lexbuf);case 22: _LL48: _LL49:
# 286
 return Cyc_token(lexbuf);case 23: _LL4A: _LL4B:
# 288
 return Cyc_token(lexbuf);case 24: _LL4C: _LL4D:
# 291 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25: _LL4E: _LL4F:
# 293
 return Cyc_token(lexbuf);case 26: _LL50: _LL51:
# 295
 return Cyc_token(lexbuf);case 27: _LL52: _LL53:
# 297
 return Cyc_token(lexbuf);case 28: _LL54: _LL55:
# 299
 return Cyc_token(lexbuf);case 29: _LL56: _LL57:
# 301
 return Cyc_token(lexbuf);case 30: _LL58: _LL59:
# 303
 return Cyc_token(lexbuf);case 31: _LL5A: _LL5B:
# 305
 return Cyc_token(lexbuf);case 32: _LL5C: _LL5D:
# 307
 return Cyc_token(lexbuf);case 33: _LL5E: _LL5F:
# 309
 return Cyc_token(lexbuf);case 34: _LL60: _LL61:
# 311
 return Cyc_token(lexbuf);case 35: _LL62: _LL63:
# 313
 return Cyc_token(lexbuf);case 36: _LL64: _LL65:
# 315
 return Cyc_token(lexbuf);case 37: _LL66: _LL67:
# 317
 return Cyc_token(lexbuf);case 38: _LL68: _LL69:
# 319
 return Cyc_token(lexbuf);case 39: _LL6A: _LL6B:
# 321
 return Cyc_token(lexbuf);case 40: _LL6C: _LL6D:
# 323
 return Cyc_token(lexbuf);case 41: _LL6E: _LL6F:
# 325
 return Cyc_token(lexbuf);case 42: _LL70: _LL71:
# 327
 return Cyc_token(lexbuf);case 43: _LL72: _LL73:
# 329
 return Cyc_token(lexbuf);case 44: _LL74: _LL75:
# 331
 return Cyc_token(lexbuf);case 45: _LL76: _LL77:
# 333
 return Cyc_token(lexbuf);case 46: _LL78: _LL79:
# 335
 return Cyc_token(lexbuf);case 47: _LL7A: _LL7B:
# 337
 return Cyc_token(lexbuf);case 48: _LL7C: _LL7D:
# 340 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL7E: _LL7F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL1B:;}{
# 344
struct Cyc_Lexing_Error_exn_struct _tmp3E0;const char*_tmp3DF;struct Cyc_Lexing_Error_exn_struct*_tmp3DE;(int)_throw((void*)((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE[0]=((_tmp3E0.tag=Cyc_Lexing_Error,((_tmp3E0.f1=((_tmp3DF="some action didn't return!",_tag_dyneither(_tmp3DF,sizeof(char),27))),_tmp3E0)))),_tmp3DE)))));};}
# 346
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp29=lexstate;switch(_tmp29){case 0: _LL81: _LL82:
# 345 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1: _LL83: _LL84:
# 346 "buildlib.cyl"
 return 0;case 2: _LL85: _LL86:
# 347 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3: _LL87: _LL88:
# 348 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4: _LL89: _LL8A:
# 351 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5: _LL8B: _LL8C:
# 354 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6: _LL8D: _LL8E:
# 356
 return Cyc_string(lexbuf);case 7: _LL8F: _LL90:
# 357 "buildlib.cyl"
 return 0;case 8: _LL91: _LL92:
# 358 "buildlib.cyl"
 return 0;case 9: _LL93: _LL94:
# 359 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL95: _LL96:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL80:;}{
# 363
struct Cyc_Lexing_Error_exn_struct _tmp3E6;const char*_tmp3E5;struct Cyc_Lexing_Error_exn_struct*_tmp3E4;(int)_throw((void*)((_tmp3E4=_cycalloc(sizeof(*_tmp3E4)),((_tmp3E4[0]=((_tmp3E6.tag=Cyc_Lexing_Error,((_tmp3E6.f1=((_tmp3E5="some action didn't return!",_tag_dyneither(_tmp3E5,sizeof(char),27))),_tmp3E6)))),_tmp3E4)))));};}
# 365
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp2D=lexstate;switch(_tmp2D){case 0: _LL98: _LL99:
# 368 "buildlib.cyl"
 return 0;case 1: _LL9A: _LL9B:
# 370
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL9C: _LL9D:
# 377 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp3E9;void*_tmp3E8;(_tmp3E8=0,Cyc_log(((_tmp3E9="Warning: declaration of malloc sidestepped\n",_tag_dyneither(_tmp3E9,sizeof(char),44))),_tag_dyneither(_tmp3E8,sizeof(void*),0)));}
return 1;case 3: _LL9E: _LL9F:
# 383 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,Cyc_log(((_tmp3EC="Warning: declaration of malloc sidestepped\n",_tag_dyneither(_tmp3EC,sizeof(char),44))),_tag_dyneither(_tmp3EB,sizeof(void*),0)));}
return 1;case 4: _LLA0: _LLA1:
# 389 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp3EF;void*_tmp3EE;(_tmp3EE=0,Cyc_log(((_tmp3EF="Warning: declaration of calloc sidestepped\n",_tag_dyneither(_tmp3EF,sizeof(char),44))),_tag_dyneither(_tmp3EE,sizeof(void*),0)));}
return 1;case 5: _LLA2: _LLA3:
# 395 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp3F2;void*_tmp3F1;(_tmp3F1=0,Cyc_log(((_tmp3F2="Warning: declaration of calloc sidestepped\n",_tag_dyneither(_tmp3F2,sizeof(char),44))),_tag_dyneither(_tmp3F1,sizeof(void*),0)));}
return 1;case 6: _LLA4: _LLA5:
# 399
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp3F5;void*_tmp3F4;(_tmp3F4=0,Cyc_log(((_tmp3F5="Warning: use of region sidestepped\n",_tag_dyneither(_tmp3F5,sizeof(char),36))),_tag_dyneither(_tmp3F4,sizeof(void*),0)));}
return 1;case 7: _LLA6: _LLA7:
# 403
{const char*_tmp3F8;void*_tmp3F7;(_tmp3F7=0,Cyc_log(((_tmp3F8="Warning: use of __extension__ deleted\n",_tag_dyneither(_tmp3F8,sizeof(char),39))),_tag_dyneither(_tmp3F7,sizeof(void*),0)));}
return 1;case 8: _LLA8: _LLA9:
# 408 "buildlib.cyl"
{const char*_tmp3FB;void*_tmp3FA;(_tmp3FA=0,Cyc_log(((_tmp3FB="Warning: use of mode HI deleted\n",_tag_dyneither(_tmp3FB,sizeof(char),33))),_tag_dyneither(_tmp3FA,sizeof(void*),0)));}
return 1;case 9: _LLAA: _LLAB:
# 411
{const char*_tmp3FE;void*_tmp3FD;(_tmp3FD=0,Cyc_log(((_tmp3FE="Warning: use of mode SI deleted\n",_tag_dyneither(_tmp3FE,sizeof(char),33))),_tag_dyneither(_tmp3FD,sizeof(void*),0)));}
return 1;case 10: _LLAC: _LLAD:
# 414
{const char*_tmp401;void*_tmp400;(_tmp400=0,Cyc_log(((_tmp401="Warning: use of mode QI deleted\n",_tag_dyneither(_tmp401,sizeof(char),33))),_tag_dyneither(_tmp400,sizeof(void*),0)));}
return 1;case 11: _LLAE: _LLAF:
# 417
{const char*_tmp404;void*_tmp403;(_tmp403=0,Cyc_log(((_tmp404="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp404,sizeof(char),33))),_tag_dyneither(_tmp403,sizeof(void*),0)));}
return 1;case 12: _LLB0: _LLB1:
# 420
{const char*_tmp407;void*_tmp406;(_tmp406=0,Cyc_log(((_tmp407="Warning: use of mode DI deleted\n",_tag_dyneither(_tmp407,sizeof(char),33))),_tag_dyneither(_tmp406,sizeof(void*),0)));}
return 1;case 13: _LLB2: _LLB3:
# 423
{const char*_tmp40A;void*_tmp409;(_tmp409=0,Cyc_log(((_tmp40A="Warning: use of mode word deleted\n",_tag_dyneither(_tmp40A,sizeof(char),35))),_tag_dyneither(_tmp409,sizeof(void*),0)));}
return 1;case 14: _LLB4: _LLB5:
# 426
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15: _LLB6: _LLB7:
# 428
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LLB8: _LLB9:
# 430
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17: _LLBA: _LLBB:
# 432
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LLBC: _LLBD:
# 437 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19: _LLBE: _LLBF:
# 439
 return 1;case 20: _LLC0: _LLC1:
# 441
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
{const char*_tmp40D;void*_tmp40C;(_tmp40C=0,Cyc_log(((_tmp40D="Warning: replacing use of __asm__ with 0\n",_tag_dyneither(_tmp40D,sizeof(char),42))),_tag_dyneither(_tmp40C,sizeof(void*),0)));}
return 1;case 21: _LLC2: _LLC3:
# 447
 Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default: _LLC4: _LLC5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL97:;}{
# 451
struct Cyc_Lexing_Error_exn_struct _tmp413;const char*_tmp412;struct Cyc_Lexing_Error_exn_struct*_tmp411;(int)_throw((void*)((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=((_tmp413.tag=Cyc_Lexing_Error,((_tmp413.f1=((_tmp412="some action didn't return!",_tag_dyneither(_tmp412,sizeof(char),27))),_tmp413)))),_tmp411)))));};}
# 453
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp4B=lexstate;switch(_tmp4B){case 0: _LLC7: _LLC8:
# 451 "buildlib.cyl"
 return 0;case 1: _LLC9: _LLCA:
# 453
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LLCB: _LLCC:
# 455
{const char*_tmp416;void*_tmp415;(_tmp415=0,Cyc_log(((_tmp416="Warning: unclosed string\n",_tag_dyneither(_tmp416,sizeof(char),26))),_tag_dyneither(_tmp415,sizeof(void*),0)));}
{const char*_tmp41A;void*_tmp419[1];struct Cyc_String_pa_PrintArg_struct _tmp418;(_tmp418.tag=0,((_tmp418.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp419[0]=& _tmp418,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp41A="%s",_tag_dyneither(_tmp41A,sizeof(char),3))),_tag_dyneither(_tmp419,sizeof(void*),1)))))));}return 1;case 3: _LLCD: _LLCE:
# 458
{const char*_tmp41E;void*_tmp41D[1];struct Cyc_String_pa_PrintArg_struct _tmp41C;(_tmp41C.tag=0,((_tmp41C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp41D[0]=& _tmp41C,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp41E="%s",_tag_dyneither(_tmp41E,sizeof(char),3))),_tag_dyneither(_tmp41D,sizeof(void*),1)))))));}return 1;case 4: _LLCF: _LLD0:
# 460
{const char*_tmp422;void*_tmp421[1];struct Cyc_String_pa_PrintArg_struct _tmp420;(_tmp420.tag=0,((_tmp420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp421[0]=& _tmp420,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp422="%s",_tag_dyneither(_tmp422,sizeof(char),3))),_tag_dyneither(_tmp421,sizeof(void*),1)))))));}return 1;case 5: _LLD1: _LLD2:
# 462
{const char*_tmp426;void*_tmp425[1];struct Cyc_String_pa_PrintArg_struct _tmp424;(_tmp424.tag=0,((_tmp424.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp425[0]=& _tmp424,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp426="%s",_tag_dyneither(_tmp426,sizeof(char),3))),_tag_dyneither(_tmp425,sizeof(void*),1)))))));}return 1;case 6: _LLD3: _LLD4:
# 464
{const char*_tmp42A;void*_tmp429[1];struct Cyc_String_pa_PrintArg_struct _tmp428;(_tmp428.tag=0,((_tmp428.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp429[0]=& _tmp428,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp42A="%s",_tag_dyneither(_tmp42A,sizeof(char),3))),_tag_dyneither(_tmp429,sizeof(void*),1)))))));}return 1;case 7: _LLD5: _LLD6:
# 466
{const char*_tmp42E;void*_tmp42D[1];struct Cyc_String_pa_PrintArg_struct _tmp42C;(_tmp42C.tag=0,((_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp42D[0]=& _tmp42C,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp42E="%s",_tag_dyneither(_tmp42E,sizeof(char),3))),_tag_dyneither(_tmp42D,sizeof(void*),1)))))));}return 1;case 8: _LLD7: _LLD8:
# 468
{const char*_tmp432;void*_tmp431[1];struct Cyc_String_pa_PrintArg_struct _tmp430;(_tmp430.tag=0,((_tmp430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp431[0]=& _tmp430,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp432="%s",_tag_dyneither(_tmp432,sizeof(char),3))),_tag_dyneither(_tmp431,sizeof(void*),1)))))));}return 1;default: _LLD9: _LLDA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LLC6:;}{
# 472
struct Cyc_Lexing_Error_exn_struct _tmp438;const char*_tmp437;struct Cyc_Lexing_Error_exn_struct*_tmp436;(int)_throw((void*)((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436[0]=((_tmp438.tag=Cyc_Lexing_Error,((_tmp438.f1=((_tmp437="some action didn't return!",_tag_dyneither(_tmp437,sizeof(char),27))),_tmp438)))),_tmp436)))));};}
# 474
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp66=lexstate;switch(_tmp66){case 0: _LLDC: _LLDD:
# 478 "buildlib.cyl"
 return 0;case 1: _LLDE: _LLDF:
# 480
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2: _LLE0: _LLE1:
# 484
 ++ Cyc_parens_to_match;
return 1;case 3: _LLE2: _LLE3:
# 487
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4: _LLE4: _LLE5:
# 490
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5: _LLE6: _LLE7:
# 493
 return 1;case 6: _LLE8: _LLE9:
# 495
 return 1;default: _LLEA: _LLEB:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LLDB:;}{
# 499
struct Cyc_Lexing_Error_exn_struct _tmp43E;const char*_tmp43D;struct Cyc_Lexing_Error_exn_struct*_tmp43C;(int)_throw((void*)((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=((_tmp43E.tag=Cyc_Lexing_Error,((_tmp43E.f1=((_tmp43D="some action didn't return!",_tag_dyneither(_tmp43D,sizeof(char),27))),_tmp43E)))),_tmp43C)))));};}
# 501
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp6A=lexstate;switch(_tmp6A){case 0: _LLED: _LLEE:
# 499 "buildlib.cyl"
{const char*_tmp441;void*_tmp440;(_tmp440=0,Cyc_log(((_tmp441="Warning: unclosed string\n",_tag_dyneither(_tmp441,sizeof(char),26))),_tag_dyneither(_tmp440,sizeof(void*),0)));}return 0;case 1: _LLEF: _LLF0:
# 501
 return 0;case 2: _LLF1: _LLF2:
# 503
{const char*_tmp444;void*_tmp443;(_tmp443=0,Cyc_log(((_tmp444="Warning: unclosed string\n",_tag_dyneither(_tmp444,sizeof(char),26))),_tag_dyneither(_tmp443,sizeof(void*),0)));}return 1;case 3: _LLF3: _LLF4:
# 505
 return 1;case 4: _LLF5: _LLF6:
# 507
 return 1;case 5: _LLF7: _LLF8:
# 509
 return 1;case 6: _LLF9: _LLFA:
# 511
 return 1;case 7: _LLFB: _LLFC:
# 513
 return 1;case 8: _LLFD: _LLFE:
# 515
 return 1;default: _LLFF: _LL100:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LLEC:;}{
# 519
struct Cyc_Lexing_Error_exn_struct _tmp44A;const char*_tmp449;struct Cyc_Lexing_Error_exn_struct*_tmp448;(int)_throw((void*)((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448[0]=((_tmp44A.tag=Cyc_Lexing_Error,((_tmp44A.f1=((_tmp449="some action didn't return!",_tag_dyneither(_tmp449,sizeof(char),27))),_tmp44A)))),_tmp448)))));};}
# 521
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp72=lexstate;switch(_tmp72){case 0: _LL102: _LL103:
# 519 "buildlib.cyl"
{const char*_tmp44D;void*_tmp44C;(_tmp44C=0,Cyc_log(((_tmp44D="Warning: unclosed comment\n",_tag_dyneither(_tmp44D,sizeof(char),27))),_tag_dyneither(_tmp44C,sizeof(void*),0)));}return 0;case 1: _LL104: _LL105:
# 521
 return 0;case 2: _LL106: _LL107:
# 523
 return 1;default: _LL108: _LL109:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL101:;}{
# 527
struct Cyc_Lexing_Error_exn_struct _tmp453;const char*_tmp452;struct Cyc_Lexing_Error_exn_struct*_tmp451;(int)_throw((void*)((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451[0]=((_tmp453.tag=Cyc_Lexing_Error,((_tmp453.f1=((_tmp452="some action didn't return!",_tag_dyneither(_tmp452,sizeof(char),27))),_tmp453)))),_tmp451)))));};}
# 529
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple21*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp78=lexstate;switch(_tmp78){case 0: _LL10B: _LL10C:
# 531 "buildlib.cyl"
{const char*_tmp454;Cyc_current_line=((_tmp454="#define ",_tag_dyneither(_tmp454,sizeof(char),9)));}
Cyc_suck_macroname(lexbuf);{
struct _tuple21*_tmp455;return(_tmp455=_cycalloc(sizeof(*_tmp455)),((_tmp455->f1=Cyc_current_line,((_tmp455->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp455)))));};case 1: _LL10D: _LL10E:
# 535
 return Cyc_suck_line(lexbuf);case 2: _LL10F: _LL110:
# 537
 return 0;default: _LL111: _LL112:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL10A:;}{
# 541
struct Cyc_Lexing_Error_exn_struct _tmp45B;const char*_tmp45A;struct Cyc_Lexing_Error_exn_struct*_tmp459;(int)_throw((void*)((_tmp459=_cycalloc(sizeof(*_tmp459)),((_tmp459[0]=((_tmp45B.tag=Cyc_Lexing_Error,((_tmp45B.f1=((_tmp45A="some action didn't return!",_tag_dyneither(_tmp45A,sizeof(char),27))),_tmp45B)))),_tmp459)))));};}
# 543
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7E=lexstate;if(_tmp7E == 0){_LL114: _LL115:
# 541 "buildlib.cyl"
{struct _dyneither_ptr*_tmp45C;Cyc_current_source=((_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp45C))));}
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL116: _LL117:
# 545
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL113:;}{
# 548
struct Cyc_Lexing_Error_exn_struct _tmp462;const char*_tmp461;struct Cyc_Lexing_Error_exn_struct*_tmp460;(int)_throw((void*)((_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460[0]=((_tmp462.tag=Cyc_Lexing_Error,((_tmp462.f1=((_tmp461="some action didn't return!",_tag_dyneither(_tmp461,sizeof(char),27))),_tmp462)))),_tmp460)))));};}
# 550
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp83=lexstate;if(_tmp83 == 0){_LL119: _LL11A:
# 548 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;}else{_LL11B: _LL11C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL118:;}{
# 552
struct Cyc_Lexing_Error_exn_struct _tmp468;const char*_tmp467;struct Cyc_Lexing_Error_exn_struct*_tmp466;(int)_throw((void*)((_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466[0]=((_tmp468.tag=Cyc_Lexing_Error,((_tmp468.f1=((_tmp467="some action didn't return!",_tag_dyneither(_tmp467,sizeof(char),27))),_tmp468)))),_tmp466)))));};}
# 554
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple23*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp87=lexstate;switch(_tmp87){case 0: _LL11E: _LL11F:
# 555 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL120: _LL121:
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
struct _tuple23*_tmp469;return(_tmp469=_cycalloc(sizeof(*_tmp469)),((_tmp469->f1=Cyc_current_headerfile,((_tmp469->f2=Cyc_current_symbols,((_tmp469->f3=Cyc_current_omit_symbols,((_tmp469->f4=Cyc_current_hstubs,((_tmp469->f5=Cyc_current_cstubs,((_tmp469->f6=Cyc_current_cycstubs,_tmp469)))))))))))));};case 2: _LL122: _LL123:
# 577
 return Cyc_spec(lexbuf);case 3: _LL124: _LL125:
# 579
 return 0;case 4: _LL126: _LL127:
# 581
{const char*_tmp46D;void*_tmp46C[1];struct Cyc_Int_pa_PrintArg_struct _tmp46B;(_tmp46B.tag=1,((_tmp46B.f1=(unsigned long)((int)
# 583
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp46C[0]=& _tmp46B,Cyc_fprintf(Cyc_stderr,((_tmp46D="Error in .cys file: expected header file name, found '%c' instead\n",_tag_dyneither(_tmp46D,sizeof(char),67))),_tag_dyneither(_tmp46C,sizeof(void*),1)))))));}
return 0;default: _LL128: _LL129:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL11D:;}{
# 588
struct Cyc_Lexing_Error_exn_struct _tmp473;const char*_tmp472;struct Cyc_Lexing_Error_exn_struct*_tmp471;(int)_throw((void*)((_tmp471=_cycalloc(sizeof(*_tmp471)),((_tmp471[0]=((_tmp473.tag=Cyc_Lexing_Error,((_tmp473.f1=((_tmp472="some action didn't return!",_tag_dyneither(_tmp472,sizeof(char),27))),_tmp473)))),_tmp471)))));};}
# 590
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp8F=lexstate;switch(_tmp8F){case 0: _LL12B: _LL12C:
# 588 "buildlib.cyl"
 return 0;case 1: _LL12D: _LL12E:
# 590
 return 0;case 2: _LL12F: _LL130:
# 592
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL131: _LL132:
# 597
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;case 4: _LL133: _LL134:
# 602
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp474;struct _tuple22*x=(_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp474->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp474)))));
{struct Cyc_List_List*_tmp475;Cyc_current_hstubs=((_tmp475=_cycalloc(sizeof(*_tmp475)),((_tmp475->hd=x,((_tmp475->tl=Cyc_current_hstubs,_tmp475))))));}
return 1;};case 5: _LL135: _LL136: {
# 610
struct _dyneither_ptr _tmp92=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp476;_dyneither_ptr_inplace_plus(& _tmp92,sizeof(char),(int)Cyc_strlen(((_tmp476="hstub",_tag_dyneither(_tmp476,sizeof(char),6)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp92,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp92,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp92;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp94=Cyc_substring((struct _dyneither_ptr)_tmp92,0,(unsigned long)((t.curr - _tmp92.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp477;struct _tuple22*x=(_tmp477=_cycalloc(sizeof(*_tmp477)),((_tmp477->f1=(struct _dyneither_ptr)_tmp94,((_tmp477->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp477)))));
{struct Cyc_List_List*_tmp478;Cyc_current_hstubs=((_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478->hd=x,((_tmp478->tl=Cyc_current_hstubs,_tmp478))))));}
return 1;};};};}case 6: _LL137: _LL138:
# 624
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp479;struct _tuple22*x=(_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp479->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp479)))));
{struct Cyc_List_List*_tmp47A;Cyc_current_cstubs=((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->hd=x,((_tmp47A->tl=Cyc_current_cstubs,_tmp47A))))));}
return 1;};case 7: _LL139: _LL13A: {
# 632
struct _dyneither_ptr _tmp99=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp47B;_dyneither_ptr_inplace_plus(& _tmp99,sizeof(char),(int)Cyc_strlen(((_tmp47B="cstub",_tag_dyneither(_tmp47B,sizeof(char),6)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp99,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp99,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp99;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp9B=Cyc_substring((struct _dyneither_ptr)_tmp99,0,(unsigned long)((t.curr - _tmp99.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp47C;struct _tuple22*x=(_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->f1=(struct _dyneither_ptr)_tmp9B,((_tmp47C->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp47C)))));
{struct Cyc_List_List*_tmp47D;Cyc_current_cstubs=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->hd=x,((_tmp47D->tl=Cyc_current_cstubs,_tmp47D))))));}
return 1;};};};}case 8: _LL13B: _LL13C:
# 646
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp47E;struct _tuple22*x=(_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp47E->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp47E)))));
{struct Cyc_List_List*_tmp47F;Cyc_current_cycstubs=((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F->hd=x,((_tmp47F->tl=Cyc_current_cycstubs,_tmp47F))))));}
return 1;};case 9: _LL13D: _LL13E: {
# 654
struct _dyneither_ptr _tmpA0=Cyc_Lexing_lexeme(lexbuf);
{const char*_tmp480;_dyneither_ptr_inplace_plus(& _tmpA0,sizeof(char),(int)Cyc_strlen(((_tmp480="cycstub",_tag_dyneither(_tmp480,sizeof(char),8)))));}
while(isspace((int)*((char*)_check_dyneither_subscript(_tmpA0,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmpA0,sizeof(char),1);}{
struct _dyneither_ptr t=_tmpA0;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmpA2=Cyc_substring((struct _dyneither_ptr)_tmpA0,0,(unsigned long)((t.curr - _tmpA0.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*_tmp481;struct _tuple22*x=(_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481->f1=(struct _dyneither_ptr)_tmpA2,((_tmp481->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp481)))));
{struct Cyc_List_List*_tmp482;Cyc_current_cycstubs=((_tmp482=_cycalloc(sizeof(*_tmp482)),((_tmp482->hd=x,((_tmp482->tl=Cyc_current_cycstubs,_tmp482))))));}
return 1;};};};}case 10: _LL13F: _LL140:
# 668
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*_tmp483;struct _dyneither_ptr*x=(_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp483)));
{struct Cyc_List_List*_tmp484;Cyc_current_cpp=((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484->hd=x,((_tmp484->tl=Cyc_current_cpp,_tmp484))))));}
return 1;};case 11: _LL141: _LL142:
# 675
 return 1;case 12: _LL143: _LL144:
# 677
 return 1;case 13: _LL145: _LL146:
# 679
{const char*_tmp488;void*_tmp487[1];struct Cyc_Int_pa_PrintArg_struct _tmp486;(_tmp486.tag=1,((_tmp486.f1=(unsigned long)((int)
# 681
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp487[0]=& _tmp486,Cyc_fprintf(Cyc_stderr,((_tmp488="Error in .cys file: expected command, found '%c' instead\n",_tag_dyneither(_tmp488,sizeof(char),58))),_tag_dyneither(_tmp487,sizeof(void*),1)))))));}
return 0;default: _LL147: _LL148:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL12A:;}{
# 686
struct Cyc_Lexing_Error_exn_struct _tmp48E;const char*_tmp48D;struct Cyc_Lexing_Error_exn_struct*_tmp48C;(int)_throw((void*)((_tmp48C=_cycalloc(sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp48E.tag=Cyc_Lexing_Error,((_tmp48E.f1=((_tmp48D="some action didn't return!",_tag_dyneither(_tmp48D,sizeof(char),27))),_tmp48E)))),_tmp48C)))));};}
# 688
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpAD=lexstate;switch(_tmpAD){case 0: _LL14A: _LL14B:
# 691 "buildlib.cyl"
{struct _dyneither_ptr*_tmp491;struct Cyc_List_List*_tmp490;Cyc_snarfed_symbols=((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490->hd=((_tmp491=_cycalloc(sizeof(*_tmp491)),((_tmp491[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp491)))),((_tmp490->tl=Cyc_snarfed_symbols,_tmp490))))));}
return 1;case 1: _LL14C: _LL14D:
# 694
 return 1;case 2: _LL14E: _LL14F:
# 696
 return 0;case 3: _LL150: _LL151:
# 698
{const char*_tmp494;void*_tmp493;(_tmp493=0,Cyc_fprintf(Cyc_stderr,((_tmp494="Error in .cys file: unexpected end-of-file\n",_tag_dyneither(_tmp494,sizeof(char),44))),_tag_dyneither(_tmp493,sizeof(void*),0)));}
# 700
return 0;case 4: _LL152: _LL153:
# 702
{const char*_tmp498;void*_tmp497[1];struct Cyc_Int_pa_PrintArg_struct _tmp496;(_tmp496.tag=1,((_tmp496.f1=(unsigned long)((int)
# 704
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp497[0]=& _tmp496,Cyc_fprintf(Cyc_stderr,((_tmp498="Error in .cys file: expected symbol, found '%c' instead\n",_tag_dyneither(_tmp498,sizeof(char),57))),_tag_dyneither(_tmp497,sizeof(void*),1)))))));}
return 0;default: _LL154: _LL155:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL149:;}{
# 709
struct Cyc_Lexing_Error_exn_struct _tmp49E;const char*_tmp49D;struct Cyc_Lexing_Error_exn_struct*_tmp49C;(int)_throw((void*)((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C[0]=((_tmp49E.tag=Cyc_Lexing_Error,((_tmp49E.f1=((_tmp49D="some action didn't return!",_tag_dyneither(_tmp49D,sizeof(char),27))),_tmp49E)))),_tmp49C)))));};}
# 711
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB8=lexstate;switch(_tmpB8){case 0: _LL157: _LL158:
# 715 "buildlib.cyl"
{const char*_tmp4A1;void*_tmp4A0;(_tmp4A0=0,Cyc_log(((_tmp4A1="Warning: unclosed brace\n",_tag_dyneither(_tmp4A1,sizeof(char),25))),_tag_dyneither(_tmp4A0,sizeof(void*),0)));}return 0;case 1: _LL159: _LL15A:
# 717
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2: _LL15B: _LL15C:
# 722
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3: _LL15D: _LL15E:
# 726
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LL15F: _LL160:
# 730
{const char*_tmp4A2;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp4A2="/*",_tag_dyneither(_tmp4A2,sizeof(char),3))));}
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LL161: _LL162:
# 734
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL163: _LL164:
# 737
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LL165: _LL166:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL156:;}{
# 742
struct Cyc_Lexing_Error_exn_struct _tmp4A8;const char*_tmp4A7;struct Cyc_Lexing_Error_exn_struct*_tmp4A6;(int)_throw((void*)((_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4A8.tag=Cyc_Lexing_Error,((_tmp4A8.f1=((_tmp4A7="some action didn't return!",_tag_dyneither(_tmp4A7,sizeof(char),27))),_tmp4A8)))),_tmp4A6)))));};}
# 744
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpBF=lexstate;switch(_tmpBF){case 0: _LL168: _LL169:
# 742 "buildlib.cyl"
{const char*_tmp4AB;void*_tmp4AA;(_tmp4AA=0,Cyc_log(((_tmp4AB="Warning: unclosed string\n",_tag_dyneither(_tmp4AB,sizeof(char),26))),_tag_dyneither(_tmp4AA,sizeof(void*),0)));}return 0;case 1: _LL16A: _LL16B:
# 744
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LL16C: _LL16D:
# 746
{const char*_tmp4AE;void*_tmp4AD;(_tmp4AD=0,Cyc_log(((_tmp4AE="Warning: unclosed string\n",_tag_dyneither(_tmp4AE,sizeof(char),26))),_tag_dyneither(_tmp4AD,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LL16E: _LL16F:
# 750
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 4: _LL170: _LL171:
# 753
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 5: _LL172: _LL173:
# 756
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL174: _LL175:
# 759
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 7: _LL176: _LL177:
# 762
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 8: _LL178: _LL179:
# 765
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL17A: _LL17B:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL167:;}{
# 770
struct Cyc_Lexing_Error_exn_struct _tmp4B4;const char*_tmp4B3;struct Cyc_Lexing_Error_exn_struct*_tmp4B2;(int)_throw((void*)((_tmp4B2=_cycalloc(sizeof(*_tmp4B2)),((_tmp4B2[0]=((_tmp4B4.tag=Cyc_Lexing_Error,((_tmp4B4.f1=((_tmp4B3="some action didn't return!",_tag_dyneither(_tmp4B3,sizeof(char),27))),_tmp4B4)))),_tmp4B2)))));};}
# 772
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC7=lexstate;switch(_tmpC7){case 0: _LL17D: _LL17E:
# 770 "buildlib.cyl"
{const char*_tmp4B7;void*_tmp4B6;(_tmp4B6=0,Cyc_log(((_tmp4B7="Warning: unclosed comment\n",_tag_dyneither(_tmp4B7,sizeof(char),27))),_tag_dyneither(_tmp4B6,sizeof(void*),0)));}return 0;case 1: _LL17F: _LL180:
# 772
{const char*_tmp4B8;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp4B8="*/",_tag_dyneither(_tmp4B8,sizeof(char),3))));}return 0;case 2: _LL181: _LL182:
# 774
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL183: _LL184:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL17C:;}{
# 779
struct Cyc_Lexing_Error_exn_struct _tmp4BE;const char*_tmp4BD;struct Cyc_Lexing_Error_exn_struct*_tmp4BC;(int)_throw((void*)((_tmp4BC=_cycalloc(sizeof(*_tmp4BC)),((_tmp4BC[0]=((_tmp4BE.tag=Cyc_Lexing_Error,((_tmp4BE.f1=((_tmp4BD="some action didn't return!",_tag_dyneither(_tmp4BD,sizeof(char),27))),_tmp4BE)))),_tmp4BC)))));};}
# 781
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 784 "buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 787
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpCE=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmpCF=_tmpCE;struct Cyc_List_List*_tmpD0;void*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct _dyneither_ptr*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;struct _dyneither_ptr*_tmpD6;void*_tmpD7;void*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;int _tmpDA;struct Cyc_Absyn_Exp*_tmpDB;void**_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;void*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_List_List*_tmpF3;struct _dyneither_ptr*_tmpF4;switch(*((int*)_tmpCF)){case 1: _LL186: _tmpF4=(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCF)->f1)->f2;_LL187:
# 791
 Cyc_add_target(_tmpF4);
return;case 2: _LL188: _tmpF3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL189:
# 794
 for(0;_tmpF3 != 0;_tmpF3=_tmpF3->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpF3->hd,dep);}
# 797
return;case 22: _LL18A: _tmpF1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpF2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL18B:
# 799
 _tmpEF=_tmpF1;_tmpF0=_tmpF2;goto _LL18D;case 8: _LL18C: _tmpEF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpF0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL18D:
# 801
 _tmpED=_tmpEF;_tmpEE=_tmpF0;goto _LL18F;case 3: _LL18E: _tmpED=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpEE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCF)->f3;_LL18F:
# 803
 Cyc_scan_exp(_tmpED,dep);
Cyc_scan_exp(_tmpEE,dep);
return;case 19: _LL190: _tmpEC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL191:
# 807
 _tmpEB=_tmpEC;goto _LL193;case 17: _LL192: _tmpEB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL193:
# 809
 _tmpEA=_tmpEB;goto _LL195;case 14: _LL194: _tmpEA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL195:
# 811
 _tmpE9=_tmpEA;goto _LL197;case 4: _LL196: _tmpE9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL197:
# 813
 Cyc_scan_exp(_tmpE9,dep);
return;case 5: _LL198: _tmpE6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_tmpE8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF)->f3;_LL199:
# 816
 Cyc_scan_exp(_tmpE6,dep);
Cyc_scan_exp(_tmpE7,dep);
Cyc_scan_exp(_tmpE8,dep);
return;case 6: _LL19A: _tmpE4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL19B:
 _tmpE2=_tmpE4;_tmpE3=_tmpE5;goto _LL19D;case 7: _LL19C: _tmpE2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL19D:
# 822
 Cyc_scan_exp(_tmpE2,dep);
Cyc_scan_exp(_tmpE3,dep);
return;case 9: _LL19E: _tmpE0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL19F:
# 826
 Cyc_scan_exp(_tmpE0,dep);
for(0;_tmpE1 != 0;_tmpE1=_tmpE1->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE1->hd,dep);}
# 830
return;case 13: _LL1A0: _tmpDE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpDF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1A1:
# 832
 Cyc_scan_type(_tmpDE,dep);
Cyc_scan_exp(_tmpDF,dep);
return;case 33: _LL1A2: _tmpDA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).is_calloc;_tmpDB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).rgn;_tmpDC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).elt_type;_tmpDD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).num_elts;_LL1A3:
# 836
 if(_tmpDB != 0)Cyc_scan_exp(_tmpDB,dep);
if(_tmpDC != 0)Cyc_scan_type(*_tmpDC,dep);
Cyc_scan_exp(_tmpDD,dep);
return;case 37: _LL1A4: _tmpD9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL1A5:
# 841
 Cyc_scan_exp(_tmpD9,dep);return;case 38: _LL1A6: _tmpD8=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL1A7:
 _tmpD7=_tmpD8;goto _LL1A9;case 16: _LL1A8: _tmpD7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL1A9:
# 844
 Cyc_scan_type(_tmpD7,dep);
return;case 20: _LL1AA: _tmpD5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpD6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1AB:
# 847
 _tmpD3=_tmpD5;_tmpD4=_tmpD6;goto _LL1AD;case 21: _LL1AC: _tmpD3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpD4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1AD:
# 849
 Cyc_scan_exp(_tmpD3,dep);
Cyc_add_target(_tmpD4);
return;case 18: _LL1AE: _tmpD1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpD2=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1AF:
# 853
 Cyc_scan_type(_tmpD1,dep);
# 855
{void*_tmpF5=(void*)((struct Cyc_List_List*)_check_null(_tmpD2))->hd;void*_tmpF6=_tmpF5;struct _dyneither_ptr*_tmpF7;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpF6)->tag == 0){_LL1D7: _tmpF7=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpF6)->f1;_LL1D8:
 Cyc_add_target(_tmpF7);goto _LL1D6;}else{_LL1D9: _LL1DA:
 goto _LL1D6;}_LL1D6:;}
# 859
return;case 0: _LL1B0: _LL1B1:
# 861
 return;case 35: _LL1B2: _tmpD0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1B3:
# 863
 for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){
struct _tuple24*_tmpF8=(struct _tuple24*)_tmpD0->hd;struct _tuple24*_tmpF9=_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;_LL1DC: _tmpFA=_tmpF9->f2;_LL1DD:
 Cyc_scan_exp(_tmpFA,dep);_LL1DB:;}
# 867
return;case 39: _LL1B4: _LL1B5:
 return;case 34: _LL1B6: _LL1B7:
# 870
{const char*_tmp4C1;void*_tmp4C0;(_tmp4C0=0,Cyc_fprintf(Cyc_stderr,((_tmp4C1="Error: unexpected Swap_e\n",_tag_dyneither(_tmp4C1,sizeof(char),26))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}
exit(1);return;case 36: _LL1B8: _LL1B9:
# 873
{const char*_tmp4C4;void*_tmp4C3;(_tmp4C3=0,Cyc_fprintf(Cyc_stderr,((_tmp4C4="Error: unexpected Stmt_e\n",_tag_dyneither(_tmp4C4,sizeof(char),26))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}
exit(1);return;case 10: _LL1BA: _LL1BB:
# 876
{const char*_tmp4C7;void*_tmp4C6;(_tmp4C6=0,Cyc_fprintf(Cyc_stderr,((_tmp4C7="Error: unexpected Throw_e\n",_tag_dyneither(_tmp4C7,sizeof(char),27))),_tag_dyneither(_tmp4C6,sizeof(void*),0)));}
exit(1);return;case 11: _LL1BC: _LL1BD:
# 879
{const char*_tmp4CA;void*_tmp4C9;(_tmp4C9=0,Cyc_fprintf(Cyc_stderr,((_tmp4CA="Error: unexpected NoInstantiate_e\n",_tag_dyneither(_tmp4CA,sizeof(char),35))),_tag_dyneither(_tmp4C9,sizeof(void*),0)));}
exit(1);return;case 12: _LL1BE: _LL1BF:
# 882
{const char*_tmp4CD;void*_tmp4CC;(_tmp4CC=0,Cyc_fprintf(Cyc_stderr,((_tmp4CD="Error: unexpected Instantiate_e\n",_tag_dyneither(_tmp4CD,sizeof(char),33))),_tag_dyneither(_tmp4CC,sizeof(void*),0)));}
exit(1);return;case 15: _LL1C0: _LL1C1:
# 885
{const char*_tmp4D0;void*_tmp4CF;(_tmp4CF=0,Cyc_fprintf(Cyc_stderr,((_tmp4D0="Error: unexpected New_e\n",_tag_dyneither(_tmp4D0,sizeof(char),25))),_tag_dyneither(_tmp4CF,sizeof(void*),0)));}
exit(1);return;case 23: _LL1C2: _LL1C3:
# 888
{const char*_tmp4D3;void*_tmp4D2;(_tmp4D2=0,Cyc_fprintf(Cyc_stderr,((_tmp4D3="Error: unexpected Tuple_e\n",_tag_dyneither(_tmp4D3,sizeof(char),27))),_tag_dyneither(_tmp4D2,sizeof(void*),0)));}
exit(1);return;case 24: _LL1C4: _LL1C5:
# 891
{const char*_tmp4D6;void*_tmp4D5;(_tmp4D5=0,Cyc_fprintf(Cyc_stderr,((_tmp4D6="Error: unexpected CompoundLit_e\n",_tag_dyneither(_tmp4D6,sizeof(char),33))),_tag_dyneither(_tmp4D5,sizeof(void*),0)));}
exit(1);return;case 25: _LL1C6: _LL1C7:
# 894
{const char*_tmp4D9;void*_tmp4D8;(_tmp4D8=0,Cyc_fprintf(Cyc_stderr,((_tmp4D9="Error: unexpected Array_e\n",_tag_dyneither(_tmp4D9,sizeof(char),27))),_tag_dyneither(_tmp4D8,sizeof(void*),0)));}
exit(1);return;case 26: _LL1C8: _LL1C9:
# 897
{const char*_tmp4DC;void*_tmp4DB;(_tmp4DB=0,Cyc_fprintf(Cyc_stderr,((_tmp4DC="Error: unexpected Comprehension_e\n",_tag_dyneither(_tmp4DC,sizeof(char),35))),_tag_dyneither(_tmp4DB,sizeof(void*),0)));}
exit(1);return;case 27: _LL1CA: _LL1CB:
# 900
{const char*_tmp4DF;void*_tmp4DE;(_tmp4DE=0,Cyc_fprintf(Cyc_stderr,((_tmp4DF="Error: unexpected ComprehensionNoinit_e\n",_tag_dyneither(_tmp4DF,sizeof(char),41))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}
exit(1);return;case 28: _LL1CC: _LL1CD:
# 903
{const char*_tmp4E2;void*_tmp4E1;(_tmp4E1=0,Cyc_fprintf(Cyc_stderr,((_tmp4E2="Error: unexpected Aggregate_e\n",_tag_dyneither(_tmp4E2,sizeof(char),31))),_tag_dyneither(_tmp4E1,sizeof(void*),0)));}
exit(1);return;case 29: _LL1CE: _LL1CF:
# 906
{const char*_tmp4E5;void*_tmp4E4;(_tmp4E4=0,Cyc_fprintf(Cyc_stderr,((_tmp4E5="Error: unexpected AnonStruct_e\n",_tag_dyneither(_tmp4E5,sizeof(char),32))),_tag_dyneither(_tmp4E4,sizeof(void*),0)));}
exit(1);return;case 30: _LL1D0: _LL1D1:
# 909
{const char*_tmp4E8;void*_tmp4E7;(_tmp4E7=0,Cyc_fprintf(Cyc_stderr,((_tmp4E8="Error: unexpected Datatype_e\n",_tag_dyneither(_tmp4E8,sizeof(char),30))),_tag_dyneither(_tmp4E7,sizeof(void*),0)));}
exit(1);return;case 31: _LL1D2: _LL1D3:
# 912
{const char*_tmp4EB;void*_tmp4EA;(_tmp4EA=0,Cyc_fprintf(Cyc_stderr,((_tmp4EB="Error: unexpected Enum_e\n",_tag_dyneither(_tmp4EB,sizeof(char),26))),_tag_dyneither(_tmp4EA,sizeof(void*),0)));}
exit(1);return;default: _LL1D4: _LL1D5:
# 915
{const char*_tmp4EE;void*_tmp4ED;(_tmp4ED=0,Cyc_fprintf(Cyc_stderr,((_tmp4EE="Error: unexpected AnonEnum_e\n",_tag_dyneither(_tmp4EE,sizeof(char),30))),_tag_dyneither(_tmp4ED,sizeof(void*),0)));}
exit(1);return;}_LL185:;}
# 920
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 925
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp11B=t;struct Cyc_Absyn_Datatypedecl*_tmp11C;struct Cyc_Absyn_Enumdecl*_tmp11D;struct Cyc_Absyn_Aggrdecl*_tmp11E;struct _dyneither_ptr*_tmp11F;struct _dyneither_ptr*_tmp120;union Cyc_Absyn_AggrInfoU _tmp121;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_FnInfo _tmp123;struct Cyc_Absyn_Exp*_tmp124;void*_tmp125;struct Cyc_Absyn_Exp*_tmp126;union Cyc_Absyn_Constraint*_tmp127;struct Cyc_Absyn_PtrInfo _tmp128;switch(*((int*)_tmp11B)){case 0: _LL1E1: _LL1E2:
 goto _LL1E4;case 6: _LL1E3: _LL1E4:
 goto _LL1E6;case 28: _LL1E5: _LL1E6:
 goto _LL1E8;case 7: _LL1E7: _LL1E8:
# 932
 return;case 5: _LL1E9: _tmp128=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B)->f1;_LL1EA:
# 935
 Cyc_scan_type(_tmp128.elt_typ,dep);
return;case 8: _LL1EB: _tmp125=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).elt_type;_tmp126=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).num_elts;_tmp127=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).zero_term;_LL1EC:
# 938
 Cyc_scan_type(_tmp125,dep);
Cyc_scan_exp_opt(_tmp126,dep);
return;case 27: _LL1ED: _tmp124=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp11B)->f1;_LL1EE:
# 942
 Cyc_scan_exp(_tmp124,dep);
return;case 9: _LL1EF: _tmp123=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11B)->f1;_LL1F0:
# 945
 Cyc_scan_type(_tmp123.ret_typ,dep);
{struct Cyc_List_List*_tmp129=_tmp123.args;for(0;_tmp129 != 0;_tmp129=_tmp129->tl){
struct _tuple8*_tmp12A=(struct _tuple8*)_tmp129->hd;struct _tuple8*_tmp12B=_tmp12A;void*_tmp12C;_LL220: _tmp12C=_tmp12B->f3;_LL221:
 Cyc_scan_type(_tmp12C,dep);_LL21F:;}}
# 950
if(_tmp123.cyc_varargs != 0)
Cyc_scan_type((_tmp123.cyc_varargs)->type,dep);
return;case 12: _LL1F1: _tmp122=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11B)->f2;_LL1F2:
# 954
 for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp122->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp122->hd)->width,dep);}
# 958
return;case 14: _LL1F3: _LL1F4:
# 960
 return;case 11: _LL1F5: _tmp121=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11B)->f1).aggr_info;_LL1F6: {
# 962
struct _tuple10 _tmp12D=Cyc_Absyn_aggr_kinded_name(_tmp121);struct _tuple10 _tmp12E=_tmp12D;struct _dyneither_ptr*_tmp12F;_LL225: _tmp12F=(_tmp12E.f2)->f2;_LL226:
 _tmp120=_tmp12F;goto _LL1F8;_LL224:;}case 13: _LL1F7: _tmp120=(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp11B)->f1)->f2;_LL1F8:
# 965
 _tmp11F=_tmp120;goto _LL1FA;case 17: _LL1F9: _tmp11F=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11B)->f1)->f2;_LL1FA:
# 967
 Cyc_add_target(_tmp11F);
return;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)){case 0: _LL1FB: _tmp11E=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)->f1;_LL1FC:
# 971
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp4F1;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4F0;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((_tmp4F0[0]=((_tmp4F1.tag=5,((_tmp4F1.f1=_tmp11E,_tmp4F1)))),_tmp4F0)))),0),dep);}{
struct _tuple0*_tmp132=_tmp11E->name;struct _tuple0*_tmp133=_tmp132;struct _dyneither_ptr*_tmp134;_LL22A: _tmp134=_tmp133->f2;_LL22B:
 Cyc_add_target(_tmp134);
return;_LL229:;};case 1: _LL1FD: _tmp11D=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)->f1;_LL1FE:
# 977
{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp4F4;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4F3;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp4F3=_cycalloc(sizeof(*_tmp4F3)),((_tmp4F3[0]=((_tmp4F4.tag=7,((_tmp4F4.f1=_tmp11D,_tmp4F4)))),_tmp4F3)))),0),dep);}{
struct _tuple0*_tmp137=_tmp11D->name;struct _tuple0*_tmp138=_tmp137;struct _dyneither_ptr*_tmp139;_LL22F: _tmp139=_tmp138->f2;_LL230:
 Cyc_add_target(_tmp139);
return;_LL22E:;};default: _LL1FF: _tmp11C=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)->f1;_LL200:
# 983
{const char*_tmp4F7;void*_tmp4F6;(_tmp4F6=0,Cyc_fprintf(Cyc_stderr,((_tmp4F7="Error: unexpected Datatype declaration\n",_tag_dyneither(_tmp4F7,sizeof(char),40))),_tag_dyneither(_tmp4F6,sizeof(void*),0)));}
exit(1);return;}case 1: _LL201: _LL202:
# 986
{const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,Cyc_fprintf(Cyc_stderr,((_tmp4FA="Error: unexpected Evar\n",_tag_dyneither(_tmp4FA,sizeof(char),24))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}
exit(1);return;case 2: _LL203: _LL204:
# 989
{const char*_tmp4FD;void*_tmp4FC;(_tmp4FC=0,Cyc_fprintf(Cyc_stderr,((_tmp4FD="Error: unexpected VarType\n",_tag_dyneither(_tmp4FD,sizeof(char),27))),_tag_dyneither(_tmp4FC,sizeof(void*),0)));}
exit(1);return;case 3: _LL205: _LL206:
# 992
{const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,Cyc_fprintf(Cyc_stderr,((_tmp500="Error: unexpected DatatypeType\n",_tag_dyneither(_tmp500,sizeof(char),32))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}
exit(1);return;case 4: _LL207: _LL208:
# 995
{const char*_tmp503;void*_tmp502;(_tmp502=0,Cyc_fprintf(Cyc_stderr,((_tmp503="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp503,sizeof(char),37))),_tag_dyneither(_tmp502,sizeof(void*),0)));}
exit(1);return;case 10: _LL209: _LL20A:
# 998
{const char*_tmp506;void*_tmp505;(_tmp505=0,Cyc_fprintf(Cyc_stderr,((_tmp506="Error: unexpected TupleType\n",_tag_dyneither(_tmp506,sizeof(char),29))),_tag_dyneither(_tmp505,sizeof(void*),0)));}
exit(1);return;case 15: _LL20B: _LL20C:
# 1001
{const char*_tmp509;void*_tmp508;(_tmp508=0,Cyc_fprintf(Cyc_stderr,((_tmp509="Error: unexpected RgnHandleType\n",_tag_dyneither(_tmp509,sizeof(char),33))),_tag_dyneither(_tmp508,sizeof(void*),0)));}
exit(1);return;case 16: _LL20D: _LL20E:
# 1004
{const char*_tmp50C;void*_tmp50B;(_tmp50B=0,Cyc_fprintf(Cyc_stderr,((_tmp50C="Error: unexpected DynRgnType\n",_tag_dyneither(_tmp50C,sizeof(char),30))),_tag_dyneither(_tmp50B,sizeof(void*),0)));}
exit(1);return;case 20: _LL20F: _LL210:
# 1007
{const char*_tmp50F;void*_tmp50E;(_tmp50E=0,Cyc_fprintf(Cyc_stderr,((_tmp50F="Error: unexpected HeapRgn\n",_tag_dyneither(_tmp50F,sizeof(char),27))),_tag_dyneither(_tmp50E,sizeof(void*),0)));}
exit(1);return;case 21: _LL211: _LL212:
# 1010
{const char*_tmp512;void*_tmp511;(_tmp511=0,Cyc_fprintf(Cyc_stderr,((_tmp512="Error: unexpected UniqueRgn\n",_tag_dyneither(_tmp512,sizeof(char),29))),_tag_dyneither(_tmp511,sizeof(void*),0)));}
exit(1);return;case 22: _LL213: _LL214:
# 1013
{const char*_tmp515;void*_tmp514;(_tmp514=0,Cyc_fprintf(Cyc_stderr,((_tmp515="Error: unexpected RefCntRgn\n",_tag_dyneither(_tmp515,sizeof(char),29))),_tag_dyneither(_tmp514,sizeof(void*),0)));}
exit(1);return;case 23: _LL215: _LL216:
# 1016
{const char*_tmp518;void*_tmp517;(_tmp517=0,Cyc_fprintf(Cyc_stderr,((_tmp518="Error: unexpected AccessEff\n",_tag_dyneither(_tmp518,sizeof(char),29))),_tag_dyneither(_tmp517,sizeof(void*),0)));}
exit(1);return;case 24: _LL217: _LL218:
# 1019
{const char*_tmp51B;void*_tmp51A;(_tmp51A=0,Cyc_fprintf(Cyc_stderr,((_tmp51B="Error: unexpected JoinEff\n",_tag_dyneither(_tmp51B,sizeof(char),27))),_tag_dyneither(_tmp51A,sizeof(void*),0)));}
exit(1);return;case 25: _LL219: _LL21A:
# 1022
{const char*_tmp51E;void*_tmp51D;(_tmp51D=0,Cyc_fprintf(Cyc_stderr,((_tmp51E="Error: unexpected RgnsEff\n",_tag_dyneither(_tmp51E,sizeof(char),27))),_tag_dyneither(_tmp51D,sizeof(void*),0)));}
exit(1);return;case 19: _LL21B: _LL21C:
# 1025
{const char*_tmp521;void*_tmp520;(_tmp520=0,Cyc_fprintf(Cyc_stderr,((_tmp521="Error: unexpected tag_t\n",_tag_dyneither(_tmp521,sizeof(char),25))),_tag_dyneither(_tmp520,sizeof(void*),0)));}
exit(1);return;default: _LL21D: _LL21E:
# 1028
{const char*_tmp524;void*_tmp523;(_tmp523=0,Cyc_fprintf(Cyc_stderr,((_tmp524="Error: unexpected valueof_t\n",_tag_dyneither(_tmp524,sizeof(char),29))),_tag_dyneither(_tmp523,sizeof(void*),0)));}
exit(1);return;}_LL1E0:;}
# 1033
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp15A=Cyc_current_targets;
struct _dyneither_ptr*_tmp15B=Cyc_current_source;
{struct Cyc_Set_Set**_tmp525;Cyc_current_targets=((_tmp525=_cycalloc(sizeof(*_tmp525)),((_tmp525[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp525))));}
{void*_tmp15D=d->r;void*_tmp15E=_tmp15D;struct Cyc_Absyn_Typedefdecl*_tmp15F;struct Cyc_Absyn_Enumdecl*_tmp160;struct Cyc_Absyn_Aggrdecl*_tmp161;struct Cyc_Absyn_Fndecl*_tmp162;struct Cyc_Absyn_Vardecl*_tmp163;switch(*((int*)_tmp15E)){case 0: _LL234: _tmp163=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL235: {
# 1039
struct _tuple0*_tmp164=_tmp163->name;struct _tuple0*_tmp165=_tmp164;struct _dyneither_ptr*_tmp166;_LL253: _tmp166=_tmp165->f2;_LL254:
 Cyc_current_source=_tmp166;
Cyc_scan_type(_tmp163->type,dep);
Cyc_scan_exp_opt(_tmp163->initializer,dep);
goto _LL233;_LL252:;}case 1: _LL236: _tmp162=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL237: {
# 1045
struct _tuple0*_tmp167=_tmp162->name;struct _tuple0*_tmp168=_tmp167;struct _dyneither_ptr*_tmp169;_LL258: _tmp169=_tmp168->f2;_LL259:
 Cyc_current_source=_tmp169;
Cyc_scan_type(_tmp162->ret_type,dep);
{struct Cyc_List_List*_tmp16A=_tmp162->args;for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
struct _tuple8*_tmp16B=(struct _tuple8*)_tmp16A->hd;struct _tuple8*_tmp16C=_tmp16B;void*_tmp16D;_LL25D: _tmp16D=_tmp16C->f3;_LL25E:
 Cyc_scan_type(_tmp16D,dep);_LL25C:;}}
# 1052
if(_tmp162->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp162->cyc_varargs))->type,dep);
if(_tmp162->is_inline){
const char*_tmp528;void*_tmp527;(_tmp527=0,Cyc_fprintf(Cyc_stderr,((_tmp528="Warning: ignoring inline function\n",_tag_dyneither(_tmp528,sizeof(char),35))),_tag_dyneither(_tmp527,sizeof(void*),0)));}
# 1057
goto _LL233;_LL257:;}case 5: _LL238: _tmp161=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL239: {
# 1059
struct _tuple0*_tmp170=_tmp161->name;struct _tuple0*_tmp171=_tmp170;struct _dyneither_ptr*_tmp172;_LL262: _tmp172=_tmp171->f2;_LL263:
 Cyc_current_source=_tmp172;
if((unsigned int)_tmp161->impl){
{struct Cyc_List_List*_tmp173=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp161->impl))->fields;for(0;_tmp173 != 0;_tmp173=_tmp173->tl){
struct Cyc_Absyn_Aggrfield*_tmp174=(struct Cyc_Absyn_Aggrfield*)_tmp173->hd;
Cyc_scan_type(_tmp174->type,dep);
Cyc_scan_exp_opt(_tmp174->width,dep);}}{
# 1069
struct Cyc_List_List*_tmp175=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp161->impl))->fields;for(0;_tmp175 != 0;_tmp175=_tmp175->tl){;}};}
# 1073
goto _LL233;_LL261:;}case 7: _LL23A: _tmp160=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL23B: {
# 1075
struct _tuple0*_tmp176=_tmp160->name;struct _tuple0*_tmp177=_tmp176;struct _dyneither_ptr*_tmp178;_LL267: _tmp178=_tmp177->f2;_LL268:
 Cyc_current_source=_tmp178;
if((unsigned int)_tmp160->fields){
{struct Cyc_List_List*_tmp179=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp160->fields))->v;for(0;_tmp179 != 0;_tmp179=_tmp179->tl){
struct Cyc_Absyn_Enumfield*_tmp17A=(struct Cyc_Absyn_Enumfield*)_tmp179->hd;
Cyc_scan_exp_opt(_tmp17A->tag,dep);}}{
# 1084
struct Cyc_List_List*_tmp17B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp160->fields))->v;for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){;}};}
# 1088
goto _LL233;_LL266:;}case 8: _LL23C: _tmp15F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL23D: {
# 1090
struct _tuple0*_tmp17C=_tmp15F->name;struct _tuple0*_tmp17D=_tmp17C;struct _dyneither_ptr*_tmp17E;_LL26C: _tmp17E=_tmp17D->f2;_LL26D:
 Cyc_current_source=_tmp17E;
if((unsigned int)_tmp15F->defn)
Cyc_scan_type((void*)_check_null(_tmp15F->defn),dep);
goto _LL233;_LL26B:;}case 4: _LL23E: _LL23F:
# 1096
{const char*_tmp52B;void*_tmp52A;(_tmp52A=0,Cyc_fprintf(Cyc_stderr,((_tmp52B="Error: unexpected region declaration",_tag_dyneither(_tmp52B,sizeof(char),37))),_tag_dyneither(_tmp52A,sizeof(void*),0)));}
exit(1);case 13: _LL240: _LL241:
# 1102
{const char*_tmp52E;void*_tmp52D;(_tmp52D=0,Cyc_fprintf(Cyc_stderr,((_tmp52E="Error: unexpected __cyclone_port_on__",_tag_dyneither(_tmp52E,sizeof(char),38))),_tag_dyneither(_tmp52D,sizeof(void*),0)));}
exit(1);case 14: _LL242: _LL243:
# 1105
{const char*_tmp531;void*_tmp530;(_tmp530=0,Cyc_fprintf(Cyc_stderr,((_tmp531="Error: unexpected __cyclone_port_off__",_tag_dyneither(_tmp531,sizeof(char),39))),_tag_dyneither(_tmp530,sizeof(void*),0)));}
exit(1);case 2: _LL244: _LL245:
# 1108
{const char*_tmp534;void*_tmp533;(_tmp533=0,Cyc_fprintf(Cyc_stderr,((_tmp534="Error: unexpected let declaration\n",_tag_dyneither(_tmp534,sizeof(char),35))),_tag_dyneither(_tmp533,sizeof(void*),0)));}
exit(1);case 6: _LL246: _LL247:
# 1111
{const char*_tmp537;void*_tmp536;(_tmp536=0,Cyc_fprintf(Cyc_stderr,((_tmp537="Error: unexpected datatype declaration\n",_tag_dyneither(_tmp537,sizeof(char),40))),_tag_dyneither(_tmp536,sizeof(void*),0)));}
exit(1);case 3: _LL248: _LL249:
# 1114
{const char*_tmp53A;void*_tmp539;(_tmp539=0,Cyc_fprintf(Cyc_stderr,((_tmp53A="Error: unexpected let declaration\n",_tag_dyneither(_tmp53A,sizeof(char),35))),_tag_dyneither(_tmp539,sizeof(void*),0)));}
exit(1);case 9: _LL24A: _LL24B:
# 1117
{const char*_tmp53D;void*_tmp53C;(_tmp53C=0,Cyc_fprintf(Cyc_stderr,((_tmp53D="Error: unexpected namespace declaration\n",_tag_dyneither(_tmp53D,sizeof(char),41))),_tag_dyneither(_tmp53C,sizeof(void*),0)));}
exit(1);case 10: _LL24C: _LL24D:
# 1120
{const char*_tmp540;void*_tmp53F;(_tmp53F=0,Cyc_fprintf(Cyc_stderr,((_tmp540="Error: unexpected using declaration\n",_tag_dyneither(_tmp540,sizeof(char),37))),_tag_dyneither(_tmp53F,sizeof(void*),0)));}
exit(1);case 11: _LL24E: _LL24F:
# 1123
{const char*_tmp543;void*_tmp542;(_tmp542=0,Cyc_fprintf(Cyc_stderr,((_tmp543="Error: unexpected extern \"C\" declaration\n",_tag_dyneither(_tmp543,sizeof(char),42))),_tag_dyneither(_tmp542,sizeof(void*),0)));}
exit(1);default: _LL250: _LL251:
# 1126
{const char*_tmp546;void*_tmp545;(_tmp545=0,Cyc_fprintf(Cyc_stderr,((_tmp546="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(_tmp546,sizeof(char),50))),_tag_dyneither(_tmp545,sizeof(void*),0)));}
exit(1);}_LL233:;}{
# 1134
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp193=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp194;_push_handler(& _tmp194);{int _tmp196=0;if(setjmp(_tmp194.handler))_tmp196=1;if(!_tmp196){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp193);;_pop_handler();}else{void*_tmp195=(void*)_exn_thrown;void*_tmp197=_tmp195;void*_tmp198;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp197)->tag == Cyc_Core_Not_found){_LL271: _LL272:
# 1139
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL270;}else{_LL273: _tmp198=_tmp197;_LL274:(int)_rethrow(_tmp198);}_LL270:;}};}{
# 1141
struct Cyc_Set_Set*_tmp199=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp193,_tmp199);
# 1144
Cyc_current_targets=_tmp15A;
Cyc_current_source=_tmp15B;};};}
# 1148
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1152
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp19A;_push_handler(& _tmp19A);{int _tmp19C=0;if(setjmp(_tmp19A.handler))_tmp19C=1;if(!_tmp19C){{struct Cyc_Set_Set*_tmp19D=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp19D;};_pop_handler();}else{void*_tmp19B=(void*)_exn_thrown;void*_tmp19E=_tmp19B;void*_tmp19F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp19E)->tag == Cyc_Core_Not_found){_LL276: _LL277:
# 1155
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL278: _tmp19F=_tmp19E;_LL279:(int)_rethrow(_tmp19F);}_LL275:;}};}
# 1159
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1170 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1A0=curr;
# 1176
struct _dyneither_ptr*_tmp1A1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1A0)> 0){
struct Cyc_Set_Set*_tmp1A2=emptyset;
struct Cyc_Iter_Iter _tmp1A3=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1A0);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1A3,& _tmp1A1)){
_tmp1A2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1A2,Cyc_find(t,_tmp1A1));}
_tmp1A0=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1A2,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp1A0);}
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
{const char*_tmp549;void*_tmp548;(_tmp548=0,Cyc_fprintf(Cyc_stderr,((_tmp549="Internal error: script file is NULL\n",_tag_dyneither(_tmp549,sizeof(char),37))),_tag_dyneither(_tmp548,sizeof(void*),0)));}
exit(1);}
# 1202
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1205
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT){
const char*_tmp54E;void*_tmp54D[2];struct Cyc_String_pa_PrintArg_struct _tmp54C;struct Cyc_String_pa_PrintArg_struct _tmp54B;(_tmp54B.tag=0,((_tmp54B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp54C.tag=0,((_tmp54C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp54D[0]=& _tmp54C,((_tmp54D[1]=& _tmp54B,Cyc_prscript(((_tmp54E="if ! test -e %s; then mkdir %s; fi\n",_tag_dyneither(_tmp54E,sizeof(char),36))),_tag_dyneither(_tmp54D,sizeof(void*),2)))))))))))));}else{
# 1212
unsigned short _tmp54F[0];int _tmp1AB=Cyc_open((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp54F,sizeof(unsigned short),0));
if(_tmp1AB == - 1){
if(mkdir((const char*)_untag_dyneither_ptr(d,sizeof(char),1),448)== - 1){
{const char*_tmp553;void*_tmp552[1];struct Cyc_String_pa_PrintArg_struct _tmp551;(_tmp551.tag=0,((_tmp551.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp552[0]=& _tmp551,Cyc_fprintf(Cyc_stderr,((_tmp553="Error: could not create directory %s\n",_tag_dyneither(_tmp553,sizeof(char),38))),_tag_dyneither(_tmp552,sizeof(void*),1)))))));}
return 1;}}else{
# 1219
close(_tmp1AB);}}
# 1221
return 0;}
# 1224
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1228
struct _dyneither_ptr _tmp1B0=Cyc_strdup((struct _dyneither_ptr)file);
# 1230
struct Cyc_List_List*_tmp1B1=0;
while(1){
_tmp1B0=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1B0);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1B0)== 0)break;{
struct _dyneither_ptr*_tmp556;struct Cyc_List_List*_tmp555;_tmp1B1=((_tmp555=_cycalloc(sizeof(*_tmp555)),((_tmp555->hd=((_tmp556=_cycalloc(sizeof(*_tmp556)),((_tmp556[0]=(struct _dyneither_ptr)_tmp1B0,_tmp556)))),((_tmp555->tl=_tmp1B1,_tmp555))))));};}
# 1237
for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp1B1->hd)))return 1;}
# 1240
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1247
static int Cyc_is_other_special(char c){
char _tmp1B4=c;switch(_tmp1B4){case 92: _LL27B: _LL27C:
 goto _LL27E;case 34: _LL27D: _LL27E:
 goto _LL280;case 59: _LL27F: _LL280:
 goto _LL282;case 38: _LL281: _LL282:
 goto _LL284;case 40: _LL283: _LL284:
 goto _LL286;case 41: _LL285: _LL286:
 goto _LL288;case 124: _LL287: _LL288:
 goto _LL28A;case 94: _LL289: _LL28A:
 goto _LL28C;case 60: _LL28B: _LL28C:
 goto _LL28E;case 62: _LL28D: _LL28E:
 goto _LL290;case 10: _LL28F: _LL290:
# 1262
 goto _LL292;case 9: _LL291: _LL292:
 return 1;default: _LL293: _LL294:
 return 0;}_LL27A:;}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp55F(unsigned int*_tmp55E,unsigned int*_tmp55D,char**_tmp55B){for(*_tmp55E=0;*_tmp55E < *_tmp55D;(*_tmp55E)++){(*_tmp55B)[*_tmp55E]=(char)'\000';}}
# 1268
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp1B5=Cyc_strlen((struct _dyneither_ptr)s);
# 1272
int _tmp1B6=0;
int _tmp1B7=0;
{int i=0;for(0;i < _tmp1B5;++ i){
char _tmp1B8=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp1B8 == '\'')++ _tmp1B6;else{
if(Cyc_is_other_special(_tmp1B8))++ _tmp1B7;}}}
# 1281
if(_tmp1B6 == 0  && _tmp1B7 == 0)
return s;
# 1285
if(_tmp1B6 == 0){
struct _dyneither_ptr*_tmp559;struct _dyneither_ptr*_tmp558[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp558[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp558[1]=((_tmp559=_cycalloc(sizeof(*_tmp559)),((_tmp559[0]=(struct _dyneither_ptr)s,_tmp559)))),((_tmp558[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp558,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 1289
unsigned long _tmp1BD=(_tmp1B5 + _tmp1B6)+ _tmp1B7;
unsigned int _tmp55E;unsigned int _tmp55D;struct _dyneither_ptr _tmp55C;char*_tmp55B;unsigned int _tmp55A;struct _dyneither_ptr s2=(_tmp55A=_tmp1BD + 1,((_tmp55B=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp55A + 1)),((_tmp55C=_tag_dyneither(_tmp55B,sizeof(char),_tmp55A + 1),((((_tmp55D=_tmp55A,((_tmp55F(& _tmp55E,& _tmp55D,& _tmp55B),_tmp55B[_tmp55D]=(char)0)))),_tmp55C)))))));
int _tmp1BE=0;
int _tmp1BF=0;
for(0;_tmp1BE < _tmp1B5;++ _tmp1BE){
char _tmp1C0=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp1BE));
if(_tmp1C0 == '\''  || Cyc_is_other_special(_tmp1C0)){
char _tmp562;char _tmp561;struct _dyneither_ptr _tmp560;(_tmp560=_dyneither_ptr_plus(s2,sizeof(char),_tmp1BF ++),((_tmp561=*((char*)_check_dyneither_subscript(_tmp560,sizeof(char),0)),((_tmp562='\\',((_get_dyneither_size(_tmp560,sizeof(char))== 1  && (_tmp561 == '\000'  && _tmp562 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp560.curr)=_tmp562)))))));}{
char _tmp565;char _tmp564;struct _dyneither_ptr _tmp563;(_tmp563=_dyneither_ptr_plus(s2,sizeof(char),_tmp1BF ++),((_tmp564=*((char*)_check_dyneither_subscript(_tmp563,sizeof(char),0)),((_tmp565=_tmp1C0,((_get_dyneither_size(_tmp563,sizeof(char))== 1  && (_tmp564 == '\000'  && _tmp565 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp563.curr)=_tmp565)))))));};}
# 1299
return(struct _dyneither_ptr)s2;};}
# 1301
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp566;return(_tmp566=_cycalloc(sizeof(*_tmp566)),((_tmp566[0]=Cyc_sh_escape_string(*sp),_tmp566)));}struct _tuple25{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1306
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1312
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1317
{const char*_tmp56C;void*_tmp56B[1];const char*_tmp56A;struct Cyc_String_pa_PrintArg_struct _tmp569;(_tmp569.tag=0,((_tmp569.f1=(struct _dyneither_ptr)(
(_tmp56A=filename,_tag_dyneither(_tmp56A,sizeof(char),_get_zero_arr_size_char((void*)_tmp56A,1)))),((_tmp56B[0]=& _tmp569,Cyc_fprintf(Cyc_stderr,((_tmp56C="********************************* %s...\n",_tag_dyneither(_tmp56C,sizeof(char),41))),_tag_dyneither(_tmp56B,sizeof(void*),1)))))));}
# 1320
if(!Cyc_gathering()){const char*_tmp572;void*_tmp571[1];const char*_tmp570;struct Cyc_String_pa_PrintArg_struct _tmp56F;(_tmp56F.tag=0,((_tmp56F.f1=(struct _dyneither_ptr)((_tmp570=filename,_tag_dyneither(_tmp570,sizeof(char),_get_zero_arr_size_char((void*)_tmp570,1)))),((_tmp571[0]=& _tmp56F,Cyc_log(((_tmp572="\n%s:\n",_tag_dyneither(_tmp572,sizeof(char),6))),_tag_dyneither(_tmp571,sizeof(void*),1)))))));}{
# 1332 "buildlib.cyl"
const char*_tmp573;struct _dyneither_ptr _tmp1D4=Cyc_Filename_basename(((_tmp573=filename,_tag_dyneither(_tmp573,sizeof(char),_get_zero_arr_size_char((void*)_tmp573,1)))));
const char*_tmp574;struct _dyneither_ptr _tmp1D5=Cyc_Filename_dirname(((_tmp574=filename,_tag_dyneither(_tmp574,sizeof(char),_get_zero_arr_size_char((void*)_tmp574,1)))));
struct _dyneither_ptr _tmp1D6=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp1D4);
const char*_tmp575;const char*_tmp1D7=(const char*)_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp1D6,((_tmp575=".iA",_tag_dyneither(_tmp575,sizeof(char),4)))),sizeof(char),1);
const char*_tmp57D;void*_tmp57C[1];struct Cyc_String_pa_PrintArg_struct _tmp57B;const char*_tmp579;void*_tmp578[1];struct Cyc_String_pa_PrintArg_struct _tmp577;const char*_tmp1D8=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp1D5,sizeof(char))== 0?(_tmp57B.tag=0,((_tmp57B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6),((_tmp57C[0]=& _tmp57B,Cyc_aprintf(((_tmp57D="%s.iB",_tag_dyneither(_tmp57D,sizeof(char),6))),_tag_dyneither(_tmp57C,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D5,(struct _dyneither_ptr)((_tmp577.tag=0,((_tmp577.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6),((_tmp578[0]=& _tmp577,Cyc_aprintf(((_tmp579="%s.iB",_tag_dyneither(_tmp579,sizeof(char),6))),_tag_dyneither(_tmp578,sizeof(void*),1))))))))),sizeof(char),1);
# 1340
const char*_tmp585;void*_tmp584[1];struct Cyc_String_pa_PrintArg_struct _tmp583;const char*_tmp581;void*_tmp580[1];struct Cyc_String_pa_PrintArg_struct _tmp57F;const char*_tmp1D9=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp1D5,sizeof(char))== 0?(_tmp583.tag=0,((_tmp583.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6),((_tmp584[0]=& _tmp583,Cyc_aprintf(((_tmp585="%s.iC",_tag_dyneither(_tmp585,sizeof(char),6))),_tag_dyneither(_tmp584,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D5,(struct _dyneither_ptr)((_tmp57F.tag=0,((_tmp57F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6),((_tmp580[0]=& _tmp57F,Cyc_aprintf(((_tmp581="%s.iC",_tag_dyneither(_tmp581,sizeof(char),6))),_tag_dyneither(_tmp580,sizeof(void*),1))))))))),sizeof(char),1);
# 1344
const char*_tmp58D;void*_tmp58C[1];struct Cyc_String_pa_PrintArg_struct _tmp58B;const char*_tmp589;void*_tmp588[1];struct Cyc_String_pa_PrintArg_struct _tmp587;const char*_tmp1DA=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp1D5,sizeof(char))== 0?(_tmp58B.tag=0,((_tmp58B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6),((_tmp58C[0]=& _tmp58B,Cyc_aprintf(((_tmp58D="%s.iD",_tag_dyneither(_tmp58D,sizeof(char),6))),_tag_dyneither(_tmp58C,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D5,(struct _dyneither_ptr)((_tmp587.tag=0,((_tmp587.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6),((_tmp588[0]=& _tmp587,Cyc_aprintf(((_tmp589="%s.iD",_tag_dyneither(_tmp589,sizeof(char),6))),_tag_dyneither(_tmp588,sizeof(void*),1))))))))),sizeof(char),1);
# 1349
{struct _handler_cons _tmp1DB;_push_handler(& _tmp1DB);{int _tmp1DD=0;if(setjmp(_tmp1DB.handler))_tmp1DD=1;if(!_tmp1DD){
# 1352
{const char*_tmp58E;if(Cyc_force_directory_prefixes(((_tmp58E=filename,_tag_dyneither(_tmp58E,sizeof(char),_get_zero_arr_size_char((void*)_tmp58E,1)))))){
int _tmp1DF=1;_npop_handler(0);return _tmp1DF;}}
# 1357
if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp594;void*_tmp593[1];const char*_tmp592;struct Cyc_String_pa_PrintArg_struct _tmp591;(_tmp591.tag=0,((_tmp591.f1=(struct _dyneither_ptr)((_tmp592=_tmp1D7,_tag_dyneither(_tmp592,sizeof(char),_get_zero_arr_size_char((void*)_tmp592,1)))),((_tmp593[0]=& _tmp591,Cyc_prscript(((_tmp594="cat >%s <<XXX\n",_tag_dyneither(_tmp594,sizeof(char),15))),_tag_dyneither(_tmp593,sizeof(void*),1)))))));}
{struct Cyc_List_List*_tmp1E4=Cyc_current_cpp;for(0;_tmp1E4 != 0;_tmp1E4=_tmp1E4->tl){
const char*_tmp598;void*_tmp597[1];struct Cyc_String_pa_PrintArg_struct _tmp596;(_tmp596.tag=0,((_tmp596.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp1E4->hd)),((_tmp597[0]=& _tmp596,Cyc_prscript(((_tmp598="%s",_tag_dyneither(_tmp598,sizeof(char),3))),_tag_dyneither(_tmp597,sizeof(void*),1)))))));}}
{const char*_tmp59E;void*_tmp59D[1];const char*_tmp59C;struct Cyc_String_pa_PrintArg_struct _tmp59B;(_tmp59B.tag=0,((_tmp59B.f1=(struct _dyneither_ptr)((_tmp59C=filename,_tag_dyneither(_tmp59C,sizeof(char),_get_zero_arr_size_char((void*)_tmp59C,1)))),((_tmp59D[0]=& _tmp59B,Cyc_prscript(((_tmp59E="#include <%s>\n",_tag_dyneither(_tmp59E,sizeof(char),15))),_tag_dyneither(_tmp59D,sizeof(void*),1)))))));}
{const char*_tmp5A1;void*_tmp5A0;(_tmp5A0=0,Cyc_prscript(((_tmp5A1="XXX\n",_tag_dyneither(_tmp5A1,sizeof(char),5))),_tag_dyneither(_tmp5A0,sizeof(void*),0)));}
{const char*_tmp5AA;void*_tmp5A9[2];const char*_tmp5A8;struct Cyc_String_pa_PrintArg_struct _tmp5A7;const char*_tmp5A6;struct Cyc_String_pa_PrintArg_struct _tmp5A5;(_tmp5A5.tag=0,((_tmp5A5.f1=(struct _dyneither_ptr)((_tmp5A6=_tmp1D7,_tag_dyneither(_tmp5A6,sizeof(char),_get_zero_arr_size_char((void*)_tmp5A6,1)))),((_tmp5A7.tag=0,((_tmp5A7.f1=(struct _dyneither_ptr)((_tmp5A8=_tmp1D8,_tag_dyneither(_tmp5A8,sizeof(char),_get_zero_arr_size_char((void*)_tmp5A8,1)))),((_tmp5A9[0]=& _tmp5A7,((_tmp5A9[1]=& _tmp5A5,Cyc_prscript(((_tmp5AA="$GCC -E -dM -o %s -x c %s && \\\n",_tag_dyneither(_tmp5AA,sizeof(char),32))),_tag_dyneither(_tmp5A9,sizeof(void*),2)))))))))))));}
{const char*_tmp5B3;void*_tmp5B2[2];const char*_tmp5B1;struct Cyc_String_pa_PrintArg_struct _tmp5B0;const char*_tmp5AF;struct Cyc_String_pa_PrintArg_struct _tmp5AE;(_tmp5AE.tag=0,((_tmp5AE.f1=(struct _dyneither_ptr)((_tmp5AF=_tmp1D7,_tag_dyneither(_tmp5AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp5AF,1)))),((_tmp5B0.tag=0,((_tmp5B0.f1=(struct _dyneither_ptr)((_tmp5B1=_tmp1D9,_tag_dyneither(_tmp5B1,sizeof(char),_get_zero_arr_size_char((void*)_tmp5B1,1)))),((_tmp5B2[0]=& _tmp5B0,((_tmp5B2[1]=& _tmp5AE,Cyc_prscript(((_tmp5B3="$GCC -E     -o %s -x c %s;\n",_tag_dyneither(_tmp5B3,sizeof(char),28))),_tag_dyneither(_tmp5B2,sizeof(void*),2)))))))))))));}{
const char*_tmp5B9;void*_tmp5B8[1];const char*_tmp5B7;struct Cyc_String_pa_PrintArg_struct _tmp5B6;(_tmp5B6.tag=0,((_tmp5B6.f1=(struct _dyneither_ptr)((_tmp5B7=_tmp1D7,_tag_dyneither(_tmp5B7,sizeof(char),_get_zero_arr_size_char((void*)_tmp5B7,1)))),((_tmp5B8[0]=& _tmp5B6,Cyc_prscript(((_tmp5B9="rm %s\n",_tag_dyneither(_tmp5B9,sizeof(char),7))),_tag_dyneither(_tmp5B8,sizeof(void*),1)))))));};}else{
# 1370
maybe=Cyc_fopen(_tmp1D7,"w");
if(!((unsigned int)maybe)){
{const char*_tmp5BF;void*_tmp5BE[1];const char*_tmp5BD;struct Cyc_String_pa_PrintArg_struct _tmp5BC;(_tmp5BC.tag=0,((_tmp5BC.f1=(struct _dyneither_ptr)((_tmp5BD=_tmp1D7,_tag_dyneither(_tmp5BD,sizeof(char),_get_zero_arr_size_char((void*)_tmp5BD,1)))),((_tmp5BE[0]=& _tmp5BC,Cyc_fprintf(Cyc_stderr,((_tmp5BF="Error: could not create file %s\n",_tag_dyneither(_tmp5BF,sizeof(char),33))),_tag_dyneither(_tmp5BE,sizeof(void*),1)))))));}{
int _tmp202=1;_npop_handler(0);return _tmp202;};}
# 1375
if(Cyc_verbose){
const char*_tmp5C5;void*_tmp5C4[1];const char*_tmp5C3;struct Cyc_String_pa_PrintArg_struct _tmp5C2;(_tmp5C2.tag=0,((_tmp5C2.f1=(struct _dyneither_ptr)((_tmp5C3=_tmp1D7,_tag_dyneither(_tmp5C3,sizeof(char),_get_zero_arr_size_char((void*)_tmp5C3,1)))),((_tmp5C4[0]=& _tmp5C2,Cyc_fprintf(Cyc_stderr,((_tmp5C5="Creating %s\n",_tag_dyneither(_tmp5C5,sizeof(char),13))),_tag_dyneither(_tmp5C4,sizeof(void*),1)))))));}
out_file=maybe;
{struct Cyc_List_List*_tmp207=Cyc_current_cpp;for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp207->hd),sizeof(char),1),out_file);}}
# 1381
{const char*_tmp5CB;void*_tmp5CA[1];const char*_tmp5C9;struct Cyc_String_pa_PrintArg_struct _tmp5C8;(_tmp5C8.tag=0,((_tmp5C8.f1=(struct _dyneither_ptr)((_tmp5C9=filename,_tag_dyneither(_tmp5C9,sizeof(char),_get_zero_arr_size_char((void*)_tmp5C9,1)))),((_tmp5CA[0]=& _tmp5C8,Cyc_fprintf(out_file,((_tmp5CB="#include <%s>\n",_tag_dyneither(_tmp5CB,sizeof(char),15))),_tag_dyneither(_tmp5CA,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp20C=Cstring_to_string(Ccomp);
const char*_tmp5D2;struct _dyneither_ptr*_tmp5D1;const char*_tmp5D0;struct Cyc_List_List*_tmp5CF;struct _dyneither_ptr _tmp20D=
Cyc_str_sepstr(((_tmp5CF=_cycalloc(sizeof(*_tmp5CF)),((_tmp5CF->hd=((_tmp5D1=_cycalloc(sizeof(*_tmp5D1)),((_tmp5D1[0]=(struct _dyneither_ptr)((_tmp5D0="",_tag_dyneither(_tmp5D0,sizeof(char),1))),_tmp5D1)))),((_tmp5CF->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp5CF)))))),(
(_tmp5D2=" ",_tag_dyneither(_tmp5D2,sizeof(char),2))));
const char*_tmp5DD;void*_tmp5DC[4];struct Cyc_String_pa_PrintArg_struct _tmp5DB;struct Cyc_String_pa_PrintArg_struct _tmp5DA;const char*_tmp5D9;struct Cyc_String_pa_PrintArg_struct _tmp5D8;const char*_tmp5D7;struct Cyc_String_pa_PrintArg_struct _tmp5D6;char*cmd=(char*)_untag_dyneither_ptr(((_tmp5D6.tag=0,((_tmp5D6.f1=(struct _dyneither_ptr)((_tmp5D7=_tmp1D7,_tag_dyneither(_tmp5D7,sizeof(char),_get_zero_arr_size_char((void*)_tmp5D7,1)))),((_tmp5D8.tag=0,((_tmp5D8.f1=(struct _dyneither_ptr)((_tmp5D9=_tmp1D8,_tag_dyneither(_tmp5D9,sizeof(char),_get_zero_arr_size_char((void*)_tmp5D9,1)))),((_tmp5DA.tag=0,((_tmp5DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20D),((_tmp5DB.tag=0,((_tmp5DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20C),((_tmp5DC[0]=& _tmp5DB,((_tmp5DC[1]=& _tmp5DA,((_tmp5DC[2]=& _tmp5D8,((_tmp5DC[3]=& _tmp5D6,Cyc_aprintf(((_tmp5DD="%s %s -E -dM -o %s -x c %s",_tag_dyneither(_tmp5DD,sizeof(char),27))),_tag_dyneither(_tmp5DC,sizeof(void*),4)))))))))))))))))))))))))),sizeof(char),1);
# 1390
if(Cyc_verbose){
const char*_tmp5E3;void*_tmp5E2[1];char*_tmp5E1;struct Cyc_String_pa_PrintArg_struct _tmp5E0;(_tmp5E0.tag=0,((_tmp5E0.f1=(struct _dyneither_ptr)((_tmp5E1=cmd,_tag_dyneither(_tmp5E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E1,1)))),((_tmp5E2[0]=& _tmp5E0,Cyc_fprintf(Cyc_stderr,((_tmp5E3="%s\n",_tag_dyneither(_tmp5E3,sizeof(char),4))),_tag_dyneither(_tmp5E2,sizeof(void*),1)))))));}
if(!system((const char*)cmd)){
# 1395
{const char*_tmp5EE;void*_tmp5ED[4];struct Cyc_String_pa_PrintArg_struct _tmp5EC;struct Cyc_String_pa_PrintArg_struct _tmp5EB;const char*_tmp5EA;struct Cyc_String_pa_PrintArg_struct _tmp5E9;const char*_tmp5E8;struct Cyc_String_pa_PrintArg_struct _tmp5E7;cmd=(char*)_untag_dyneither_ptr(((_tmp5E7.tag=0,((_tmp5E7.f1=(struct _dyneither_ptr)((_tmp5E8=_tmp1D7,_tag_dyneither(_tmp5E8,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E8,1)))),((_tmp5E9.tag=0,((_tmp5E9.f1=(struct _dyneither_ptr)((_tmp5EA=_tmp1D9,_tag_dyneither(_tmp5EA,sizeof(char),_get_zero_arr_size_char((void*)_tmp5EA,1)))),((_tmp5EB.tag=0,((_tmp5EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20D),((_tmp5EC.tag=0,((_tmp5EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20C),((_tmp5ED[0]=& _tmp5EC,((_tmp5ED[1]=& _tmp5EB,((_tmp5ED[2]=& _tmp5E9,((_tmp5ED[3]=& _tmp5E7,Cyc_aprintf(((_tmp5EE="%s %s -E -o %s -x c %s",_tag_dyneither(_tmp5EE,sizeof(char),23))),_tag_dyneither(_tmp5ED,sizeof(void*),4)))))))))))))))))))))))))),sizeof(char),1);}
# 1397
if(Cyc_verbose){
const char*_tmp5F4;void*_tmp5F3[1];char*_tmp5F2;struct Cyc_String_pa_PrintArg_struct _tmp5F1;(_tmp5F1.tag=0,((_tmp5F1.f1=(struct _dyneither_ptr)((_tmp5F2=cmd,_tag_dyneither(_tmp5F2,sizeof(char),_get_zero_arr_size_char((void*)_tmp5F2,1)))),((_tmp5F3[0]=& _tmp5F1,Cyc_fprintf(Cyc_stderr,((_tmp5F4="%s\n",_tag_dyneither(_tmp5F4,sizeof(char),4))),_tag_dyneither(_tmp5F3,sizeof(void*),1)))))));}
system((const char*)cmd);}};}}
# 1404
if(Cyc_gathering()){int _tmp22A=0;_npop_handler(0);return _tmp22A;}{
# 1407
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1D8,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp605;const char*_tmp604;void*_tmp603[1];const char*_tmp602;struct Cyc_String_pa_PrintArg_struct _tmp601;struct Cyc_NO_SUPPORT_exn_struct*_tmp600;(int)_throw((void*)((_tmp600=_cycalloc(sizeof(*_tmp600)),((_tmp600[0]=((_tmp605.tag=Cyc_NO_SUPPORT,((_tmp605.f1=(struct _dyneither_ptr)((_tmp601.tag=0,((_tmp601.f1=(struct _dyneither_ptr)((_tmp602=_tmp1D8,_tag_dyneither(_tmp602,sizeof(char),_get_zero_arr_size_char((void*)_tmp602,1)))),((_tmp603[0]=& _tmp601,Cyc_aprintf(((_tmp604="can't open macrosfile %s",_tag_dyneither(_tmp604,sizeof(char),25))),_tag_dyneither(_tmp603,sizeof(void*),1)))))))),_tmp605)))),_tmp600)))));}
# 1412
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp231=Cyc_Lexing_from_file(in_file);
struct _tuple20*entry;
while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp231))!= 0){
struct _tuple20*_tmp232=(struct _tuple20*)_check_null(entry);struct _tuple20*_tmp233=_tmp232;struct _dyneither_ptr*_tmp234;struct Cyc_Set_Set*_tmp235;_LL296: _tmp234=_tmp233->f1;_tmp235=_tmp233->f2;_LL297:
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp234,_tmp235);_LL295:;}
# 1421
Cyc_fclose(in_file);
# 1424
maybe=Cyc_fopen(_tmp1D9,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp616;const char*_tmp615;void*_tmp614[1];const char*_tmp613;struct Cyc_String_pa_PrintArg_struct _tmp612;struct Cyc_NO_SUPPORT_exn_struct*_tmp611;(int)_throw((void*)((_tmp611=_cycalloc(sizeof(*_tmp611)),((_tmp611[0]=((_tmp616.tag=Cyc_NO_SUPPORT,((_tmp616.f1=(struct _dyneither_ptr)((_tmp612.tag=0,((_tmp612.f1=(struct _dyneither_ptr)((_tmp613=_tmp1D9,_tag_dyneither(_tmp613,sizeof(char),_get_zero_arr_size_char((void*)_tmp613,1)))),((_tmp614[0]=& _tmp612,Cyc_aprintf(((_tmp615="can't open declsfile %s",_tag_dyneither(_tmp615,sizeof(char),24))),_tag_dyneither(_tmp614,sizeof(void*),1)))))))),_tmp616)))),_tmp611)))));}
# 1428
in_file=maybe;
_tmp231=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp1DA,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp23C=1;_npop_handler(0);return _tmp23C;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp231)){;}
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1438
maybe=Cyc_fopen(_tmp1DA,"r");
if(!((unsigned int)maybe)){int _tmp23D=1;_npop_handler(0);return _tmp23D;}
in_file=maybe;
{const char*_tmp617;Cyc_Position_reset_position(((_tmp617=_tmp1DA,_tag_dyneither(_tmp617,sizeof(char),_get_zero_arr_size_char((void*)_tmp617,1)))));}
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp23F=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1448
{struct Cyc_List_List*_tmp240=_tmp23F;for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp240->hd,t);}}{
# 1452
struct Cyc_Set_Set*_tmp241=Cyc_reachable(start_symbols,t);
# 1455
struct Cyc_List_List*_tmp242=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp243=_tmp23F;for(0;_tmp243 != 0;_tmp243=_tmp243->tl){
struct Cyc_Absyn_Decl*_tmp244=(struct Cyc_Absyn_Decl*)_tmp243->hd;
struct _dyneither_ptr*name;
{void*_tmp245=_tmp244->r;void*_tmp246=_tmp245;struct Cyc_Absyn_Typedefdecl*_tmp247;struct Cyc_Absyn_Enumdecl*_tmp248;struct Cyc_Absyn_Aggrdecl*_tmp249;struct Cyc_Absyn_Fndecl*_tmp24A;struct Cyc_Absyn_Vardecl*_tmp24B;switch(*((int*)_tmp246)){case 0: _LL29B: _tmp24B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL29C: {
# 1464
struct _tuple0*_tmp24C=_tmp24B->name;struct _tuple0*_tmp24D=_tmp24C;struct _dyneither_ptr*_tmp24E;_LL2BA: _tmp24E=_tmp24D->f2;_LL2BB:
 defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp24E);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp24E))name=0;else{
name=_tmp24E;}
goto _LL29A;_LL2B9:;}case 1: _LL29D: _tmp24A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL29E: {
# 1470
struct _tuple0*_tmp24F=_tmp24A->name;struct _tuple0*_tmp250=_tmp24F;struct _dyneither_ptr*_tmp251;_LL2BF: _tmp251=_tmp250->f2;_LL2C0:
 defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp251);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp251))name=0;else{
name=_tmp251;}
goto _LL29A;_LL2BE:;}case 5: _LL29F: _tmp249=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL2A0: {
# 1476
struct _tuple0*_tmp252=_tmp249->name;struct _tuple0*_tmp253=_tmp252;struct _dyneither_ptr*_tmp254;_LL2C4: _tmp254=_tmp253->f2;_LL2C5:
 name=_tmp254;
goto _LL29A;_LL2C3:;}case 7: _LL2A1: _tmp248=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL2A2: {
# 1480
struct _tuple0*_tmp255=_tmp248->name;struct _tuple0*_tmp256=_tmp255;struct _dyneither_ptr*_tmp257;_LL2C9: _tmp257=_tmp256->f2;_LL2CA:
 name=_tmp257;
# 1484
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,name)){
struct Cyc_List_List*_tmp618;_tmp242=((_tmp618=_cycalloc(sizeof(*_tmp618)),((_tmp618->hd=_tmp244,((_tmp618->tl=_tmp242,_tmp618))))));}else{
# 1487
if((unsigned int)_tmp248->fields){
struct Cyc_List_List*_tmp259=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp248->fields))->v;for(0;_tmp259 != 0;_tmp259=_tmp259->tl){
struct Cyc_Absyn_Enumfield*_tmp25A=(struct Cyc_Absyn_Enumfield*)_tmp259->hd;
struct _tuple0*_tmp25B=_tmp25A->name;struct _tuple0*_tmp25C=_tmp25B;struct _dyneither_ptr*_tmp25D;_LL2CE: _tmp25D=_tmp25C->f2;_LL2CF:
 if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,_tmp25D)){
{struct Cyc_List_List*_tmp619;_tmp242=((_tmp619=_cycalloc(sizeof(*_tmp619)),((_tmp619->hd=_tmp244,((_tmp619->tl=_tmp242,_tmp619))))));}
break;}_LL2CD:;}}}
# 1497
name=0;
goto _LL29A;_LL2C8:;}case 8: _LL2A3: _tmp247=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL2A4: {
# 1500
struct _tuple0*_tmp25F=_tmp247->name;struct _tuple0*_tmp260=_tmp25F;struct _dyneither_ptr*_tmp261;_LL2D3: _tmp261=_tmp260->f2;_LL2D4:
 name=_tmp261;
goto _LL29A;_LL2D2:;}case 13: _LL2A5: _LL2A6:
 goto _LL2A8;case 14: _LL2A7: _LL2A8:
 goto _LL2AA;case 2: _LL2A9: _LL2AA:
 goto _LL2AC;case 6: _LL2AB: _LL2AC:
 goto _LL2AE;case 3: _LL2AD: _LL2AE:
 goto _LL2B0;case 9: _LL2AF: _LL2B0:
 goto _LL2B2;case 10: _LL2B1: _LL2B2:
 goto _LL2B4;case 11: _LL2B3: _LL2B4:
 goto _LL2B6;case 12: _LL2B5: _LL2B6:
 goto _LL2B8;default: _LL2B7: _LL2B8:
# 1514
 name=0;
goto _LL29A;}_LL29A:;}
# 1517
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,name)){
struct Cyc_List_List*_tmp61A;_tmp242=((_tmp61A=_cycalloc(sizeof(*_tmp61A)),((_tmp61A->hd=_tmp244,((_tmp61A->tl=_tmp242,_tmp61A))))));}}}
# 1522
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp263=1;_npop_handler(0);return _tmp263;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
const char*_tmp620;void*_tmp61F[1];const char*_tmp61E;struct Cyc_String_pa_PrintArg_struct _tmp61D;struct _dyneither_ptr ifdefmacro=(_tmp61D.tag=0,((_tmp61D.f1=(struct _dyneither_ptr)((_tmp61E=filename,_tag_dyneither(_tmp61E,sizeof(char),_get_zero_arr_size_char((void*)_tmp61E,1)))),((_tmp61F[0]=& _tmp61D,Cyc_aprintf(((_tmp620="_%s_",_tag_dyneither(_tmp620,sizeof(char),5))),_tag_dyneither(_tmp61F,sizeof(void*),1)))))));
{int _tmp264=0;for(0;_tmp264 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp264){
if(((char*)ifdefmacro.curr)[_tmp264]== '.'  || ((char*)ifdefmacro.curr)[_tmp264]== '/'){
char _tmp623;char _tmp622;struct _dyneither_ptr _tmp621;(_tmp621=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp264),((_tmp622=*((char*)_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623='_',((_get_dyneither_size(_tmp621,sizeof(char))== 1  && (_tmp622 == '\000'  && _tmp623 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp621.curr)=_tmp623)))))));}else{
if(((char*)ifdefmacro.curr)[_tmp264]!= '_'  && ((char*)ifdefmacro.curr)[_tmp264]!= '/'){
char _tmp626;char _tmp625;struct _dyneither_ptr _tmp624;(_tmp624=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp264),((_tmp625=*((char*)_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626=(char)toupper((int)((char*)ifdefmacro.curr)[_tmp264]),((_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}}}}
# 1534
{const char*_tmp62B;void*_tmp62A[2];struct Cyc_String_pa_PrintArg_struct _tmp629;struct Cyc_String_pa_PrintArg_struct _tmp628;(_tmp628.tag=0,((_tmp628.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp629.tag=0,((_tmp629.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp62A[0]=& _tmp629,((_tmp62A[1]=& _tmp628,Cyc_fprintf(out_file,((_tmp62B="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp62B,sizeof(char),23))),_tag_dyneither(_tmp62A,sizeof(void*),2)))))))))))));}{
# 1541
struct Cyc_List_List*_tmp26F=0;
struct Cyc_List_List*_tmp270=0;
{struct Cyc_List_List*_tmp271=_tmp242;for(0;_tmp271 != 0;_tmp271=_tmp271->tl){
struct Cyc_Absyn_Decl*_tmp272=(struct Cyc_Absyn_Decl*)_tmp271->hd;
int _tmp273=0;
struct _dyneither_ptr*name;
{void*_tmp274=_tmp272->r;void*_tmp275=_tmp274;struct Cyc_Absyn_Typedefdecl*_tmp276;struct Cyc_Absyn_Enumdecl*_tmp277;struct Cyc_Absyn_Aggrdecl*_tmp278;struct Cyc_Absyn_Fndecl*_tmp279;struct Cyc_Absyn_Vardecl*_tmp27A;switch(*((int*)_tmp275)){case 0: _LL2D8: _tmp27A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2D9: {
# 1549
struct _tuple0*_tmp27B=_tmp27A->name;struct _tuple0*_tmp27C=_tmp27B;struct _dyneither_ptr*_tmp27D;_LL2F7: _tmp27D=_tmp27C->f2;_LL2F8:
 name=_tmp27D;
goto _LL2D7;_LL2F6:;}case 1: _LL2DA: _tmp279=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2DB: {
# 1553
struct _tuple0*_tmp27E=_tmp279->name;struct _tuple0*_tmp27F=_tmp27E;struct _dyneither_ptr*_tmp280;_LL2FC: _tmp280=_tmp27F->f2;_LL2FD:
 name=_tmp280;
goto _LL2D7;_LL2FB:;}case 5: _LL2DC: _tmp278=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2DD: {
# 1557
struct _tuple0*_tmp281=_tmp278->name;struct _tuple0*_tmp282=_tmp281;struct _dyneither_ptr*_tmp283;_LL301: _tmp283=_tmp282->f2;_LL302:
 name=_tmp283;
goto _LL2D7;_LL300:;}case 7: _LL2DE: _tmp277=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2DF: {
# 1561
struct _tuple0*_tmp284=_tmp277->name;struct _tuple0*_tmp285=_tmp284;struct _dyneither_ptr*_tmp286;_LL306: _tmp286=_tmp285->f2;_LL307:
 name=_tmp286;
goto _LL2D7;_LL305:;}case 8: _LL2E0: _tmp276=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2E1: {
# 1565
struct _tuple0*_tmp287=_tmp276->name;struct _tuple0*_tmp288=_tmp287;struct _dyneither_ptr*_tmp289;_LL30B: _tmp289=_tmp288->f2;_LL30C:
 name=_tmp289;
goto _LL2D7;_LL30A:;}case 4: _LL2E2: _LL2E3:
 goto _LL2E5;case 13: _LL2E4: _LL2E5:
# 1570
 goto _LL2E7;case 14: _LL2E6: _LL2E7:
 goto _LL2E9;case 2: _LL2E8: _LL2E9:
 goto _LL2EB;case 6: _LL2EA: _LL2EB:
 goto _LL2ED;case 3: _LL2EC: _LL2ED:
 goto _LL2EF;case 9: _LL2EE: _LL2EF:
 goto _LL2F1;case 10: _LL2F0: _LL2F1:
 goto _LL2F3;case 11: _LL2F2: _LL2F3:
 goto _LL2F5;default: _LL2F4: _LL2F5:
# 1579
 name=0;
goto _LL2D7;}_LL2D7:;}
# 1582
if(!((unsigned int)name) && !_tmp273)continue;
{struct Cyc_List_List*_tmp62C;_tmp26F=((_tmp62C=_cycalloc(sizeof(*_tmp62C)),((_tmp62C->hd=_tmp272,((_tmp62C->tl=_tmp26F,_tmp62C))))));}{
struct Cyc_List_List*_tmp62D;_tmp270=((_tmp62D=_cycalloc(sizeof(*_tmp62D)),((_tmp62D->hd=name,((_tmp62D->tl=_tmp270,_tmp62D))))));};}}
# 1588
{struct _handler_cons _tmp28C;_push_handler(& _tmp28C);{int _tmp28E=0;if(setjmp(_tmp28C.handler))_tmp28E=1;if(!_tmp28E){
{struct _RegionHandle _tmp28F=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp28F;_push_region(tc_rgn);
{struct Cyc_Tcenv_Tenv*_tmp290=Cyc_Tcenv_tc_init(tc_rgn);
Cyc_Tc_tc(tc_rgn,_tmp290,1,_tmp26F);}
# 1590
;_pop_region(tc_rgn);}
# 1589
;_pop_handler();}else{void*_tmp28D=(void*)_exn_thrown;void*_tmp291=_tmp28D;_LL310: _LL311:
# 1595
{struct Cyc_NO_SUPPORT_exn_struct _tmp633;const char*_tmp632;struct Cyc_NO_SUPPORT_exn_struct*_tmp631;(int)_throw((void*)((_tmp631=_cycalloc(sizeof(*_tmp631)),((_tmp631[0]=((_tmp633.tag=Cyc_NO_SUPPORT,((_tmp633.f1=((_tmp632="can't typecheck acquired declarations",_tag_dyneither(_tmp632,sizeof(char),38))),_tmp633)))),_tmp631)))));}
goto _LL30F;_LL30F:;}};}
# 1600
{struct _tuple25 _tmp634;struct _tuple25 _tmp295=(_tmp634.f1=_tmp26F,((_tmp634.f2=_tmp270,_tmp634)));struct _tuple25 _tmp296=_tmp295;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp298;_LL315: _tmp297=_tmp296.f1;_tmp298=_tmp296.f2;_LL316: for(0;
_tmp297 != 0  && _tmp298 != 0;(_tmp297=_tmp297->tl,_tmp298=_tmp298->tl)){
struct Cyc_Absyn_Decl*_tmp299=(struct Cyc_Absyn_Decl*)_tmp297->hd;
struct _dyneither_ptr*_tmp29A=(struct _dyneither_ptr*)_tmp298->hd;
int _tmp29B=0;
if(!((unsigned int)_tmp29A))
_tmp29B=1;
# 1609
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp29A){
{const char*_tmp638;void*_tmp637[1];struct Cyc_String_pa_PrintArg_struct _tmp636;ifdefmacro=((_tmp636.tag=0,((_tmp636.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp29A),((_tmp637[0]=& _tmp636,Cyc_aprintf(((_tmp638="_%s_def_",_tag_dyneither(_tmp638,sizeof(char),9))),_tag_dyneither(_tmp637,sizeof(void*),1))))))));}
{const char*_tmp63C;void*_tmp63B[1];struct Cyc_String_pa_PrintArg_struct _tmp63A;(_tmp63A.tag=0,((_tmp63A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp63B[0]=& _tmp63A,Cyc_fprintf(out_file,((_tmp63C="#ifndef %s\n",_tag_dyneither(_tmp63C,sizeof(char),12))),_tag_dyneither(_tmp63B,sizeof(void*),1)))))));}
{const char*_tmp640;void*_tmp63F[1];struct Cyc_String_pa_PrintArg_struct _tmp63E;(_tmp63E.tag=0,((_tmp63E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp63F[0]=& _tmp63E,Cyc_fprintf(out_file,((_tmp640="#define %s\n",_tag_dyneither(_tmp640,sizeof(char),12))),_tag_dyneither(_tmp63F,sizeof(void*),1)))))));}
# 1615
{struct Cyc_Absyn_Decl*_tmp641[1];Cyc_Absynpp_decllist2file(((_tmp641[0]=_tmp299,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp641,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}{
const char*_tmp644;void*_tmp643;(_tmp643=0,Cyc_fprintf(out_file,((_tmp644="#endif\n",_tag_dyneither(_tmp644,sizeof(char),8))),_tag_dyneither(_tmp643,sizeof(void*),0)));};}else{
# 1620
struct Cyc_Absyn_Decl*_tmp645[1];Cyc_Absynpp_decllist2file(((_tmp645[0]=_tmp299,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp645,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}_LL314:;}
# 1625
maybe=Cyc_fopen(_tmp1D8,"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp656;const char*_tmp655;void*_tmp654[1];const char*_tmp653;struct Cyc_String_pa_PrintArg_struct _tmp652;struct Cyc_NO_SUPPORT_exn_struct*_tmp651;(int)_throw((void*)((_tmp651=_cycalloc(sizeof(*_tmp651)),((_tmp651[0]=((_tmp656.tag=Cyc_NO_SUPPORT,((_tmp656.f1=(struct _dyneither_ptr)((_tmp652.tag=0,((_tmp652.f1=(struct _dyneither_ptr)((_tmp653=_tmp1D8,_tag_dyneither(_tmp653,sizeof(char),_get_zero_arr_size_char((void*)_tmp653,1)))),((_tmp654[0]=& _tmp652,Cyc_aprintf(((_tmp655="can't open macrosfile %s",_tag_dyneither(_tmp655,sizeof(char),25))),_tag_dyneither(_tmp654,sizeof(void*),1)))))))),_tmp656)))),_tmp651)))));}
# 1628
in_file=maybe;
_tmp231=Cyc_Lexing_from_file(in_file);{
struct _tuple21*entry2;
while((entry2=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp231))!= 0){
struct _tuple21*_tmp2B0=(struct _tuple21*)_check_null(entry2);struct _tuple21*_tmp2B1=_tmp2B0;struct _dyneither_ptr _tmp2B2;struct _dyneither_ptr*_tmp2B3;_LL31A: _tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f2;_LL31B:
 if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,_tmp2B3)){
{const char*_tmp65A;void*_tmp659[1];struct Cyc_String_pa_PrintArg_struct _tmp658;(_tmp658.tag=0,((_tmp658.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B3),((_tmp659[0]=& _tmp658,Cyc_fprintf(out_file,((_tmp65A="#ifndef %s\n",_tag_dyneither(_tmp65A,sizeof(char),12))),_tag_dyneither(_tmp659,sizeof(void*),1)))))));}
{const char*_tmp65E;void*_tmp65D[1];struct Cyc_String_pa_PrintArg_struct _tmp65C;(_tmp65C.tag=0,((_tmp65C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2B2),((_tmp65D[0]=& _tmp65C,Cyc_fprintf(out_file,((_tmp65E="%s\n",_tag_dyneither(_tmp65E,sizeof(char),4))),_tag_dyneither(_tmp65D,sizeof(void*),1)))))));}{
const char*_tmp661;void*_tmp660;(_tmp660=0,Cyc_fprintf(out_file,((_tmp661="#endif\n",_tag_dyneither(_tmp661,sizeof(char),8))),_tag_dyneither(_tmp660,sizeof(void*),0)));};}_LL319:;}
# 1639
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1642
if(hstubs != 0){
struct Cyc_List_List*_tmp2BC=hstubs;for(0;_tmp2BC != 0;_tmp2BC=_tmp2BC->tl){
struct _tuple22*_tmp2BD=(struct _tuple22*)_tmp2BC->hd;struct _tuple22*_tmp2BE=_tmp2BD;struct _dyneither_ptr _tmp2BF;struct _dyneither_ptr _tmp2C0;_LL31F: _tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BE->f2;_LL320: {
struct _dyneither_ptr*_tmp662;if((char*)_tmp2C0.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2BF.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp662=_cycalloc(sizeof(*_tmp662)),((_tmp662[0]=_tmp2BF,_tmp662)))))))
# 1648
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2C0,sizeof(char),1),out_file);else{
# 1650
const char*_tmp666;void*_tmp665[1];struct Cyc_String_pa_PrintArg_struct _tmp664;(_tmp664.tag=0,((_tmp664.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BF),((_tmp665[0]=& _tmp664,Cyc_log(((_tmp666="%s is not supported on this platform\n",_tag_dyneither(_tmp666,sizeof(char),38))),_tag_dyneither(_tmp665,sizeof(void*),1)))))));}}_LL31E:;}}
# 1653
{const char*_tmp669;void*_tmp668;(_tmp668=0,Cyc_fprintf(out_file,((_tmp669="#endif\n",_tag_dyneither(_tmp669,sizeof(char),8))),_tag_dyneither(_tmp668,sizeof(void*),0)));}
if(Cyc_do_setjmp){int _tmp2C7=0;_npop_handler(0);return _tmp2C7;}else{
Cyc_fclose(out_file);}
# 1658
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2C8=cstubs;for(0;_tmp2C8 != 0;_tmp2C8=_tmp2C8->tl){
struct _tuple22*_tmp2C9=(struct _tuple22*)_tmp2C8->hd;struct _tuple22*_tmp2CA=_tmp2C9;struct _dyneither_ptr _tmp2CB;struct _dyneither_ptr _tmp2CC;_LL324: _tmp2CB=_tmp2CA->f1;_tmp2CC=_tmp2CA->f2;_LL325: {
struct _dyneither_ptr*_tmp66A;if((char*)_tmp2CC.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2CB.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp66A=_cycalloc(sizeof(*_tmp66A)),((_tmp66A[0]=_tmp2CB,_tmp66A)))))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2CC,sizeof(char),1),out_file);}_LL323:;}};}
# 1669
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1671
{const char*_tmp670;void*_tmp66F[1];const char*_tmp66E;struct Cyc_String_pa_PrintArg_struct _tmp66D;(_tmp66D.tag=0,((_tmp66D.f1=(struct _dyneither_ptr)((_tmp66E=filename,_tag_dyneither(_tmp66E,sizeof(char),_get_zero_arr_size_char((void*)_tmp66E,1)))),((_tmp66F[0]=& _tmp66D,Cyc_fprintf(out_file,((_tmp670="#include <%s>\n\n",_tag_dyneither(_tmp670,sizeof(char),16))),_tag_dyneither(_tmp66F,sizeof(void*),1)))))));}
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2D2=cycstubs;for(0;_tmp2D2 != 0;_tmp2D2=_tmp2D2->tl){
struct _tuple22*_tmp2D3=(struct _tuple22*)_tmp2D2->hd;struct _tuple22*_tmp2D4=_tmp2D3;struct _dyneither_ptr _tmp2D5;struct _dyneither_ptr _tmp2D6;_LL329: _tmp2D5=_tmp2D4->f1;_tmp2D6=_tmp2D4->f2;_LL32A: {
struct _dyneither_ptr*_tmp671;if((char*)_tmp2D6.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2D5.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp671=_cycalloc(sizeof(*_tmp671)),((_tmp671[0]=_tmp2D5,_tmp671)))))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2D6,sizeof(char),1),out_file);}_LL328:;}}{
# 1680
const char*_tmp674;void*_tmp673;(_tmp673=0,Cyc_fprintf(out_file,((_tmp674="\n",_tag_dyneither(_tmp674,sizeof(char),2))),_tag_dyneither(_tmp673,sizeof(void*),0)));};}{
# 1683
int _tmp2DA=0;_npop_handler(0);return _tmp2DA;};};};};};};};};
# 1352
;_pop_handler();}else{void*_tmp1DC=(void*)_exn_thrown;void*_tmp2DF=_tmp1DC;struct _dyneither_ptr _tmp2E0;struct _dyneither_ptr _tmp2E1;struct _dyneither_ptr _tmp2E2;struct _dyneither_ptr _tmp2E3;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2DF)->tag == Cyc_Core_Impossible){_LL32E: _tmp2E3=((struct Cyc_Core_Impossible_exn_struct*)_tmp2DF)->f1;_LL32F:
# 1687
{const char*_tmp678;void*_tmp677[1];struct Cyc_String_pa_PrintArg_struct _tmp676;(_tmp676.tag=0,((_tmp676.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E3),((_tmp677[0]=& _tmp676,Cyc_fprintf(Cyc_stderr,((_tmp678="Got Core::Impossible(%s)\n",_tag_dyneither(_tmp678,sizeof(char),26))),_tag_dyneither(_tmp677,sizeof(void*),1)))))));}goto _LL32D;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2DF)->tag == Cyc_Dict_Absent){_LL330: _LL331:
# 1689
{const char*_tmp67B;void*_tmp67A;(_tmp67A=0,Cyc_fprintf(Cyc_stderr,((_tmp67B="Got Dict::Absent\n",_tag_dyneither(_tmp67B,sizeof(char),18))),_tag_dyneither(_tmp67A,sizeof(void*),0)));}goto _LL32D;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2DF)->tag == Cyc_Core_Failure){_LL332: _tmp2E2=((struct Cyc_Core_Failure_exn_struct*)_tmp2DF)->f1;_LL333:
# 1691
{const char*_tmp67F;void*_tmp67E[1];struct Cyc_String_pa_PrintArg_struct _tmp67D;(_tmp67D.tag=0,((_tmp67D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E2),((_tmp67E[0]=& _tmp67D,Cyc_fprintf(Cyc_stderr,((_tmp67F="Got Core::Failure(%s)\n",_tag_dyneither(_tmp67F,sizeof(char),23))),_tag_dyneither(_tmp67E,sizeof(void*),1)))))));}goto _LL32D;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2DF)->tag == Cyc_Core_Invalid_argument){_LL334: _tmp2E1=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2DF)->f1;_LL335:
# 1693
{const char*_tmp683;void*_tmp682[1];struct Cyc_String_pa_PrintArg_struct _tmp681;(_tmp681.tag=0,((_tmp681.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E1),((_tmp682[0]=& _tmp681,Cyc_fprintf(Cyc_stderr,((_tmp683="Got Invalid_argument(%s)\n",_tag_dyneither(_tmp683,sizeof(char),26))),_tag_dyneither(_tmp682,sizeof(void*),1)))))));}goto _LL32D;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2DF)->tag == Cyc_Core_Not_found){_LL336: _LL337:
# 1695
{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(Cyc_stderr,((_tmp686="Got Not_found\n",_tag_dyneither(_tmp686,sizeof(char),15))),_tag_dyneither(_tmp685,sizeof(void*),0)));}goto _LL32D;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2DF)->tag == Cyc_NO_SUPPORT){_LL338: _tmp2E0=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2DF)->f1;_LL339:
# 1697
{const char*_tmp68A;void*_tmp689[1];struct Cyc_String_pa_PrintArg_struct _tmp688;(_tmp688.tag=0,((_tmp688.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E0),((_tmp689[0]=& _tmp688,Cyc_fprintf(Cyc_stderr,((_tmp68A="No support because %s\n",_tag_dyneither(_tmp68A,sizeof(char),23))),_tag_dyneither(_tmp689,sizeof(void*),1)))))));}goto _LL32D;}else{_LL33A: _LL33B:
# 1699
{const char*_tmp68D;void*_tmp68C;(_tmp68C=0,Cyc_fprintf(Cyc_stderr,((_tmp68D="Got unknown exception\n",_tag_dyneither(_tmp68D,sizeof(char),23))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}goto _LL32D;}}}}}}_LL32D:;}};}
# 1704
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
{const char*_tmp693;void*_tmp692[1];const char*_tmp691;struct Cyc_String_pa_PrintArg_struct _tmp690;(_tmp690.tag=0,((_tmp690.f1=(struct _dyneither_ptr)((_tmp691=filename,_tag_dyneither(_tmp691,sizeof(char),_get_zero_arr_size_char((void*)_tmp691,1)))),((_tmp692[0]=& _tmp690,Cyc_fprintf(Cyc_stderr,((_tmp693="Error: could not create file %s\n",_tag_dyneither(_tmp693,sizeof(char),33))),_tag_dyneither(_tmp692,sizeof(void*),1)))))));}
return 1;}
# 1709
out_file=maybe;
{const char*_tmp699;void*_tmp698[1];const char*_tmp697;struct Cyc_String_pa_PrintArg_struct _tmp696;(_tmp696.tag=0,((_tmp696.f1=(struct _dyneither_ptr)(
# 1712
(_tmp697=filename,_tag_dyneither(_tmp697,sizeof(char),_get_zero_arr_size_char((void*)_tmp697,1)))),((_tmp698[0]=& _tmp696,Cyc_fprintf(out_file,((_tmp699="#error -- %s is not supported on this platform\n",_tag_dyneither(_tmp699,sizeof(char),48))),_tag_dyneither(_tmp698,sizeof(void*),1)))))));}
Cyc_fclose(out_file);
{const char*_tmp69F;void*_tmp69E[1];const char*_tmp69D;struct Cyc_String_pa_PrintArg_struct _tmp69C;(_tmp69C.tag=0,((_tmp69C.f1=(struct _dyneither_ptr)(
(_tmp69D=filename,_tag_dyneither(_tmp69D,sizeof(char),_get_zero_arr_size_char((void*)_tmp69D,1)))),((_tmp69E[0]=& _tmp69C,Cyc_fprintf(Cyc_stderr,((_tmp69F="Warning: %s will not be supported on this platform\n",_tag_dyneither(_tmp69F,sizeof(char),52))),_tag_dyneither(_tmp69E,sizeof(void*),1)))))));}
{const char*_tmp6A2;void*_tmp6A1;(_tmp6A1=0,Cyc_log(((_tmp6A2="Not supported on this platform\n",_tag_dyneither(_tmp6A2,sizeof(char),32))),_tag_dyneither(_tmp6A1,sizeof(void*),0)));}
# 1723
return 0;};}int Cyc_process_specfile(const char*file,const char*dir);static void _tmp6B3(unsigned int*_tmp6B2,unsigned int*_tmp6B1,char**_tmp6B0){for(*_tmp6B2=0;*_tmp6B2 < *_tmp6B1;(*_tmp6B2)++){(*_tmp6B0)[*_tmp6B2]=(char)'\000';}}
# 1727
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose){
const char*_tmp6A8;void*_tmp6A7[1];const char*_tmp6A6;struct Cyc_String_pa_PrintArg_struct _tmp6A5;(_tmp6A5.tag=0,((_tmp6A5.f1=(struct _dyneither_ptr)((_tmp6A6=file,_tag_dyneither(_tmp6A6,sizeof(char),_get_zero_arr_size_char((void*)_tmp6A6,1)))),((_tmp6A7[0]=& _tmp6A5,Cyc_fprintf(Cyc_stderr,((_tmp6A8="Processing %s\n",_tag_dyneither(_tmp6A8,sizeof(char),15))),_tag_dyneither(_tmp6A7,sizeof(void*),1)))))));}{
struct Cyc___cycFILE*_tmp31D=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp31D)){
{const char*_tmp6AE;void*_tmp6AD[1];const char*_tmp6AC;struct Cyc_String_pa_PrintArg_struct _tmp6AB;(_tmp6AB.tag=0,((_tmp6AB.f1=(struct _dyneither_ptr)((_tmp6AC=file,_tag_dyneither(_tmp6AC,sizeof(char),_get_zero_arr_size_char((void*)_tmp6AC,1)))),((_tmp6AD[0]=& _tmp6AB,Cyc_fprintf(Cyc_stderr,((_tmp6AE="Error: could not open %s\n",_tag_dyneither(_tmp6AE,sizeof(char),26))),_tag_dyneither(_tmp6AD,sizeof(void*),1)))))));}
return 1;}{
# 1735
struct Cyc___cycFILE*_tmp322=_tmp31D;
# 1739
char*_tmp6B8;unsigned int _tmp6B7;char*_tmp6B6;unsigned int _tmp6B5;unsigned int _tmp6B4;struct _dyneither_ptr buf=(_tmp6B8=((_tmp6B7=(unsigned int)1024,((_tmp6B6=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp6B7 + 1)),((((_tmp6B5=_tmp6B7,((_tmp6B3(& _tmp6B4,& _tmp6B5,& _tmp6B6),_tmp6B6[_tmp6B5]=(char)0)))),_tmp6B6)))))),_tag_dyneither(_tmp6B8,sizeof(char),_get_zero_arr_size_char((void*)_tmp6B8,(unsigned int)1024 + 1)));
struct _dyneither_ptr _tmp323=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir(dir)){
{const char*_tmp6BE;void*_tmp6BD[1];const char*_tmp6BC;struct Cyc_String_pa_PrintArg_struct _tmp6BB;(_tmp6BB.tag=0,((_tmp6BB.f1=(struct _dyneither_ptr)((_tmp6BC=dir,_tag_dyneither(_tmp6BC,sizeof(char),_get_zero_arr_size_char((void*)_tmp6BC,1)))),((_tmp6BD[0]=& _tmp6BB,Cyc_fprintf(Cyc_stderr,((_tmp6BE="Error: can't change directory to %s\n",_tag_dyneither(_tmp6BE,sizeof(char),37))),_tag_dyneither(_tmp6BD,sizeof(void*),1)))))));}
return 1;}}
# 1747
if(Cyc_mode == Cyc_GATHER){
# 1749
struct _dyneither_ptr _tmp328=Cstring_to_string(Ccomp);
const char*_tmp6C2;void*_tmp6C1[1];struct Cyc_String_pa_PrintArg_struct _tmp6C0;struct _dyneither_ptr _tmp329=(_tmp6C0.tag=0,((_tmp6C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp328),((_tmp6C1[0]=& _tmp6C0,Cyc_aprintf(((_tmp6C2="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp6C2,sizeof(char),36))),_tag_dyneither(_tmp6C1,sizeof(void*),1)))))));
if(Cyc_verbose){
const char*_tmp6C6;void*_tmp6C5[1];struct Cyc_String_pa_PrintArg_struct _tmp6C4;(_tmp6C4.tag=0,((_tmp6C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp329),((_tmp6C5[0]=& _tmp6C4,Cyc_fprintf(Cyc_stderr,((_tmp6C6="%s\n",_tag_dyneither(_tmp6C6,sizeof(char),4))),_tag_dyneither(_tmp6C5,sizeof(void*),1)))))));}
system((const char*)_untag_dyneither_ptr(_tmp329,sizeof(char),1));}{
# 1756
struct Cyc_Lexing_lexbuf*_tmp330=Cyc_Lexing_from_file(_tmp322);
struct _tuple23*entry;
while((entry=((struct _tuple23*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp330))!= 0){
struct _tuple23*_tmp331=(struct _tuple23*)_check_null(entry);struct _tuple23*_tmp332=_tmp331;struct _dyneither_ptr _tmp333;struct Cyc_List_List*_tmp334;struct Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp338;_LL33F: _tmp333=_tmp332->f1;_tmp334=_tmp332->f2;_tmp335=_tmp332->f3;_tmp336=_tmp332->f4;_tmp337=_tmp332->f5;_tmp338=_tmp332->f6;_LL340:
# 1761
 if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp333,sizeof(char),1),_tmp334,_tmp335,_tmp336,_tmp337,_tmp338))
# 1763
return 1;_LL33E:;}
# 1765
Cyc_fclose(_tmp322);
# 1767
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp323,sizeof(char),1)))){
{const char*_tmp6CA;void*_tmp6C9[1];struct Cyc_String_pa_PrintArg_struct _tmp6C8;(_tmp6C8.tag=0,((_tmp6C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp323),((_tmp6C9[0]=& _tmp6C8,Cyc_fprintf(Cyc_stderr,((_tmp6CA="Error: could not change directory to %s\n",_tag_dyneither(_tmp6CA,sizeof(char),41))),_tag_dyneither(_tmp6C9,sizeof(void*),1)))))));}
return 1;}}
# 1773
return 0;};};};}int Cyc_process_setjmp(const char*dir);static void _tmp6CF(unsigned int*_tmp6CE,unsigned int*_tmp6CD,char**_tmp6CC){for(*_tmp6CE=0;*_tmp6CE < *_tmp6CD;(*_tmp6CE)++){(*_tmp6CC)[*_tmp6CE]=(char)'\000';}}
# 1777
int Cyc_process_setjmp(const char*dir){
# 1780
char*_tmp6D4;unsigned int _tmp6D3;char*_tmp6D2;unsigned int _tmp6D1;unsigned int _tmp6D0;struct _dyneither_ptr buf=(_tmp6D4=((_tmp6D3=(unsigned int)1024,((_tmp6D2=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp6D3 + 1)),((((_tmp6D1=_tmp6D3,((_tmp6CF(& _tmp6D0,& _tmp6D1,& _tmp6D2),_tmp6D2[_tmp6D1]=(char)0)))),_tmp6D2)))))),_tag_dyneither(_tmp6D4,sizeof(char),_get_zero_arr_size_char((void*)_tmp6D4,(unsigned int)1024 + 1)));
struct _dyneither_ptr _tmp340=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(chdir(dir)){
{const char*_tmp6DA;void*_tmp6D9[1];const char*_tmp6D8;struct Cyc_String_pa_PrintArg_struct _tmp6D7;(_tmp6D7.tag=0,((_tmp6D7.f1=(struct _dyneither_ptr)((_tmp6D8=dir,_tag_dyneither(_tmp6D8,sizeof(char),_get_zero_arr_size_char((void*)_tmp6D8,1)))),((_tmp6D9[0]=& _tmp6D7,Cyc_fprintf(Cyc_stderr,((_tmp6DA="Error: can't change directory to %s\n",_tag_dyneither(_tmp6DA,sizeof(char),37))),_tag_dyneither(_tmp6D9,sizeof(void*),1)))))));}
return 1;}
# 1786
{struct _tuple22*_tmp6E4;const char*_tmp6E3;const char*_tmp6E2;struct _tuple22*_tmp6E1[1];struct _dyneither_ptr*_tmp6DB[1];if(Cyc_process_file("setjmp.h",((_tmp6DB[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6DB,sizeof(struct _dyneither_ptr*),1)))),0,(
(_tmp6E1[0]=((_tmp6E4=_cycalloc(sizeof(*_tmp6E4)),((_tmp6E4->f1=((_tmp6E3="setjmp",_tag_dyneither(_tmp6E3,sizeof(char),7))),((_tmp6E4->f2=((_tmp6E2="extern int setjmp(jmp_buf);\n",_tag_dyneither(_tmp6E2,sizeof(char),29))),_tmp6E4)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6E1,sizeof(struct _tuple22*),1)))),0,0))
# 1789
return 1;}
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp340,sizeof(char),1)))){
{const char*_tmp6E8;void*_tmp6E7[1];struct Cyc_String_pa_PrintArg_struct _tmp6E6;(_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp340),((_tmp6E7[0]=& _tmp6E6,Cyc_fprintf(Cyc_stderr,((_tmp6E8="Error: could not change directory to %s\n",_tag_dyneither(_tmp6E8,sizeof(char),41))),_tag_dyneither(_tmp6E7,sizeof(void*),1)))))));}
return 1;}
# 1794
return 0;}static char _tmp352[13]="BUILDLIB.OUT";
# 1798
static struct _dyneither_ptr Cyc_output_dir={_tmp352,_tmp352,_tmp352 + 13};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 1802
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
struct Cyc_List_List*_tmp6E9;Cyc_spec_files=((_tmp6E9=_cycalloc(sizeof(*_tmp6E9)),((_tmp6E9->hd=(const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp6E9->tl=Cyc_spec_files,_tmp6E9))))));}
# 1806
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 1810
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1813
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 1816
static void Cyc_add_cpparg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp6EC;struct Cyc_List_List*_tmp6EB;Cyc_cppargs=((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB->hd=((_tmp6EC=_cycalloc(sizeof(*_tmp6EC)),((_tmp6EC[0]=s,_tmp6EC)))),((_tmp6EB->tl=Cyc_cppargs,_tmp6EB))))));}
# 1819
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
{const char*_tmp6F0;void*_tmp6EF[1];struct Cyc_String_pa_PrintArg_struct _tmp6EE;(_tmp6EE.tag=0,((_tmp6EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp6EF[0]=& _tmp6EE,Cyc_fprintf(Cyc_stderr,((_tmp6F0="Unsupported option %s\n",_tag_dyneither(_tmp6F0,sizeof(char),23))),_tag_dyneither(_tmp6EF,sizeof(void*),1)))))));}
Cyc_badparse=1;}
# 1824
static void Cyc_set_machine(struct _dyneither_ptr s){
{const char*_tmp6F1;Cyc_add_cpparg(((_tmp6F1="-b",_tag_dyneither(_tmp6F1,sizeof(char),3))));}
Cyc_add_cpparg(s);}
# 1832
void GC_blacklist_warn_clear();struct _tuple26{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
GC_blacklist_warn_clear();{
# 1836
struct _tuple26*_tmp782;const char*_tmp781;const char*_tmp780;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp77F;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp77E;const char*_tmp77D;struct _tuple26*_tmp77C;const char*_tmp77B;const char*_tmp77A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp779;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp778;const char*_tmp777;struct _tuple26*_tmp776;const char*_tmp775;const char*_tmp774;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp773;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp772;const char*_tmp771;struct _tuple26*_tmp770;const char*_tmp76F;const char*_tmp76E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp76D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp76C;const char*_tmp76B;struct _tuple26*_tmp76A;const char*_tmp769;const char*_tmp768;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp767;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp766;const char*_tmp765;struct _tuple26*_tmp764;const char*_tmp763;const char*_tmp762;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp761;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp760;const char*_tmp75F;struct _tuple26*_tmp75E;const char*_tmp75D;const char*_tmp75C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp75B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp75A;const char*_tmp759;struct _tuple26*_tmp758;const char*_tmp757;const char*_tmp756;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp755;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp754;const char*_tmp753;struct _tuple26*_tmp752[8];struct Cyc_List_List*options=
(_tmp752[7]=(
# 1861
(_tmp758=_cycalloc(sizeof(*_tmp758)),((_tmp758->f1=((_tmp757="-",_tag_dyneither(_tmp757,sizeof(char),2))),((_tmp758->f2=1,((_tmp758->f3=((_tmp756="",_tag_dyneither(_tmp756,sizeof(char),1))),((_tmp758->f4=(void*)(
(_tmp754=_cycalloc(sizeof(*_tmp754)),((_tmp754[0]=((_tmp755.tag=1,((_tmp755.f1=Cyc_add_cpparg,_tmp755)))),_tmp754)))),((_tmp758->f5=(
(_tmp753="",_tag_dyneither(_tmp753,sizeof(char),1))),_tmp758)))))))))))),((_tmp752[6]=(
# 1858
(_tmp75E=_cycalloc(sizeof(*_tmp75E)),((_tmp75E->f1=((_tmp75D="-v",_tag_dyneither(_tmp75D,sizeof(char),3))),((_tmp75E->f2=0,((_tmp75E->f3=((_tmp75C="",_tag_dyneither(_tmp75C,sizeof(char),1))),((_tmp75E->f4=(void*)(
(_tmp75A=_cycalloc(sizeof(*_tmp75A)),((_tmp75A[0]=((_tmp75B.tag=3,((_tmp75B.f1=& Cyc_verbose,_tmp75B)))),_tmp75A)))),((_tmp75E->f5=(
(_tmp759="Verbose operation",_tag_dyneither(_tmp759,sizeof(char),18))),_tmp75E)))))))))))),((_tmp752[5]=(
# 1855
(_tmp764=_cycalloc(sizeof(*_tmp764)),((_tmp764->f1=((_tmp763="-b",_tag_dyneither(_tmp763,sizeof(char),3))),((_tmp764->f2=0,((_tmp764->f3=((_tmp762=" <machine>",_tag_dyneither(_tmp762,sizeof(char),11))),((_tmp764->f4=(void*)(
(_tmp760=_cycalloc(sizeof(*_tmp760)),((_tmp760[0]=((_tmp761.tag=5,((_tmp761.f1=Cyc_set_machine,_tmp761)))),_tmp760)))),((_tmp764->f5=(
(_tmp75F="Set the target machine for compilation to <machine>",_tag_dyneither(_tmp75F,sizeof(char),52))),_tmp764)))))))))))),((_tmp752[4]=(
# 1849
(_tmp76A=_cycalloc(sizeof(*_tmp76A)),((_tmp76A->f1=((_tmp769="-setjmp",_tag_dyneither(_tmp769,sizeof(char),8))),((_tmp76A->f2=0,((_tmp76A->f3=((_tmp768="",_tag_dyneither(_tmp768,sizeof(char),1))),((_tmp76A->f4=(void*)(
(_tmp766=_cycalloc(sizeof(*_tmp766)),((_tmp766[0]=((_tmp767.tag=3,((_tmp767.f1=& Cyc_do_setjmp,_tmp767)))),_tmp766)))),((_tmp76A->f5=(
# 1854
(_tmp765="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",_tag_dyneither(_tmp765,sizeof(char),186))),_tmp76A)))))))))))),((_tmp752[3]=(
# 1846
(_tmp770=_cycalloc(sizeof(*_tmp770)),((_tmp770->f1=((_tmp76F="-finish",_tag_dyneither(_tmp76F,sizeof(char),8))),((_tmp770->f2=0,((_tmp770->f3=((_tmp76E="",_tag_dyneither(_tmp76E,sizeof(char),1))),((_tmp770->f4=(void*)(
(_tmp76C=_cycalloc(sizeof(*_tmp76C)),((_tmp76C[0]=((_tmp76D.tag=0,((_tmp76D.f1=Cyc_set_FINISH,_tmp76D)))),_tmp76C)))),((_tmp770->f5=(
(_tmp76B="Produce Cyclone headers from pre-gathered C library info",_tag_dyneither(_tmp76B,sizeof(char),57))),_tmp770)))))))))))),((_tmp752[2]=(
# 1843
(_tmp776=_cycalloc(sizeof(*_tmp776)),((_tmp776->f1=((_tmp775="-gatherscript",_tag_dyneither(_tmp775,sizeof(char),14))),((_tmp776->f2=0,((_tmp776->f3=((_tmp774="",_tag_dyneither(_tmp774,sizeof(char),1))),((_tmp776->f4=(void*)(
(_tmp772=_cycalloc(sizeof(*_tmp772)),((_tmp772[0]=((_tmp773.tag=0,((_tmp773.f1=Cyc_set_GATHERSCRIPT,_tmp773)))),_tmp772)))),((_tmp776->f5=(
(_tmp771="Produce a script to gather C library info",_tag_dyneither(_tmp771,sizeof(char),42))),_tmp776)))))))))))),((_tmp752[1]=(
# 1840
(_tmp77C=_cycalloc(sizeof(*_tmp77C)),((_tmp77C->f1=((_tmp77B="-gather",_tag_dyneither(_tmp77B,sizeof(char),8))),((_tmp77C->f2=0,((_tmp77C->f3=((_tmp77A="",_tag_dyneither(_tmp77A,sizeof(char),1))),((_tmp77C->f4=(void*)(
(_tmp778=_cycalloc(sizeof(*_tmp778)),((_tmp778[0]=((_tmp779.tag=0,((_tmp779.f1=Cyc_set_GATHER,_tmp779)))),_tmp778)))),((_tmp77C->f5=(
(_tmp777="Gather C library info but don't produce Cyclone headers",_tag_dyneither(_tmp777,sizeof(char),56))),_tmp77C)))))))))))),((_tmp752[0]=(
# 1837
(_tmp782=_cycalloc(sizeof(*_tmp782)),((_tmp782->f1=((_tmp781="-d",_tag_dyneither(_tmp781,sizeof(char),3))),((_tmp782->f2=0,((_tmp782->f3=((_tmp780=" <file>",_tag_dyneither(_tmp780,sizeof(char),8))),((_tmp782->f4=(void*)(
(_tmp77E=_cycalloc(sizeof(*_tmp77E)),((_tmp77E[0]=((_tmp77F.tag=5,((_tmp77F.f1=Cyc_set_output_dir,_tmp77F)))),_tmp77E)))),((_tmp782->f5=(
(_tmp77D="Set the output directory to <file>",_tag_dyneither(_tmp77D,sizeof(char),35))),_tmp782)))))))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp752,sizeof(struct _tuple26*),8)))))))))))))))));
# 1865
{const char*_tmp783;Cyc_Arg_parse(options,Cyc_add_spec_file,Cyc_no_other,((_tmp783="Options:",_tag_dyneither(_tmp783,sizeof(char),9))),argv);}
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp784;Cyc_Arg_usage(options,(
# 1873
(_tmp784="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",_tag_dyneither(_tmp784,sizeof(char),59))));}
return 1;}
# 1877
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose){
const char*_tmp787;void*_tmp786;(_tmp786=0,Cyc_fprintf(Cyc_stderr,((_tmp787="Creating BUILDLIB.sh\n",_tag_dyneither(_tmp787,sizeof(char),22))),_tag_dyneither(_tmp786,sizeof(void*),0)));}
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
{const char*_tmp78A;void*_tmp789;(_tmp789=0,Cyc_fprintf(Cyc_stderr,((_tmp78A="Could not create file BUILDLIB.sh\n",_tag_dyneither(_tmp78A,sizeof(char),35))),_tag_dyneither(_tmp789,sizeof(void*),0)));}
exit(1);}
# 1885
{const char*_tmp78D;void*_tmp78C;(_tmp78C=0,Cyc_prscript(((_tmp78D="#!/bin/sh\n",_tag_dyneither(_tmp78D,sizeof(char),11))),_tag_dyneither(_tmp78C,sizeof(void*),0)));}{
const char*_tmp790;void*_tmp78F;(_tmp78F=0,Cyc_prscript(((_tmp790="GCC=\"gcc\"\n",_tag_dyneither(_tmp790,sizeof(char),11))),_tag_dyneither(_tmp78F,sizeof(void*),0)));};}
# 1890
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
{const char*_tmp794;void*_tmp793[1];struct Cyc_String_pa_PrintArg_struct _tmp792;(_tmp792.tag=0,((_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp793[0]=& _tmp792,Cyc_fprintf(Cyc_stderr,((_tmp794="Error: could not create directory %s\n",_tag_dyneither(_tmp794,sizeof(char),38))),_tag_dyneither(_tmp793,sizeof(void*),1)))))));}
return 1;}
# 1894
if(Cyc_verbose){
const char*_tmp798;void*_tmp797[1];struct Cyc_String_pa_PrintArg_struct _tmp796;(_tmp796.tag=0,((_tmp796.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp797[0]=& _tmp796,Cyc_fprintf(Cyc_stderr,((_tmp798="Output directory is %s\n",_tag_dyneither(_tmp798,sizeof(char),24))),_tag_dyneither(_tmp797,sizeof(void*),1)))))));}
# 1897
if(Cyc_mode == Cyc_GATHERSCRIPT){
{const char*_tmp79C;void*_tmp79B[1];struct Cyc_String_pa_PrintArg_struct _tmp79A;(_tmp79A.tag=0,((_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp79B[0]=& _tmp79A,Cyc_prscript(((_tmp79C="cd %s\n",_tag_dyneither(_tmp79C,sizeof(char),7))),_tag_dyneither(_tmp79B,sizeof(void*),1)))))));}{
const char*_tmp79F;void*_tmp79E;(_tmp79E=0,Cyc_prscript(((_tmp79F="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp79F,sizeof(char),38))),_tag_dyneither(_tmp79E,sizeof(void*),0)));};}
# 1902
if(!Cyc_gathering()){
# 1905
{const char*_tmp7A0;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp7A0="BUILDLIB.LOG",_tag_dyneither(_tmp7A0,sizeof(char),13)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_log_file)){
{const char*_tmp7A4;void*_tmp7A3[1];struct Cyc_String_pa_PrintArg_struct _tmp7A2;(_tmp7A2.tag=0,((_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp7A3[0]=& _tmp7A2,Cyc_fprintf(Cyc_stderr,((_tmp7A4="Error: could not create log file in directory %s\n",_tag_dyneither(_tmp7A4,sizeof(char),50))),_tag_dyneither(_tmp7A3,sizeof(void*),1)))))));}
return 1;}
# 1911
if(!Cyc_do_setjmp){
# 1913
{const char*_tmp7A5;Cyc_cstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp7A5="cstubs.c",_tag_dyneither(_tmp7A5,sizeof(char),9)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_cstubs_file)){
{const char*_tmp7A9;void*_tmp7A8[1];struct Cyc_String_pa_PrintArg_struct _tmp7A7;(_tmp7A7.tag=0,((_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp7A8[0]=& _tmp7A7,Cyc_fprintf(Cyc_stderr,((_tmp7A9="Error: could not create cstubs.c in directory %s\n",_tag_dyneither(_tmp7A9,sizeof(char),50))),_tag_dyneither(_tmp7A8,sizeof(void*),1)))))));}
return 1;}
# 1920
{const char*_tmp7AA;Cyc_cycstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp7AA="cycstubs.cyc",_tag_dyneither(_tmp7AA,sizeof(char),13)))),sizeof(char),1),"w");}
if(!((unsigned int)Cyc_cycstubs_file)){
{const char*_tmp7AE;void*_tmp7AD[1];struct Cyc_String_pa_PrintArg_struct _tmp7AC;(_tmp7AC.tag=0,((_tmp7AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp7AD[0]=& _tmp7AC,Cyc_fprintf(Cyc_stderr,((_tmp7AE="Error: could not create cycstubs.c in directory %s\n",_tag_dyneither(_tmp7AE,sizeof(char),52))),_tag_dyneither(_tmp7AD,sizeof(void*),1)))))));}
# 1925
return 1;}{
# 1927
const char*_tmp7B1;void*_tmp7B0;(_tmp7B0=0,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),((_tmp7B1="#include <core.h>\nusing Core;\n\n",_tag_dyneither(_tmp7B1,sizeof(char),32))),_tag_dyneither(_tmp7B0,sizeof(void*),0)));};}}{
# 1934
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 1940
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
{const char*_tmp7B4;void*_tmp7B3;(_tmp7B3=0,Cyc_fprintf(Cyc_stderr,((_tmp7B4="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp7B4,sizeof(char),22))),_tag_dyneither(_tmp7B3,sizeof(void*),0)));}
exit(1);}}}
# 1948
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 1951
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 1959
return 0;};};}
