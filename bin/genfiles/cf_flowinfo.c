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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 121 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 400
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 147 "dict.h"
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 195
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 215 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 225
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
# 232
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 432 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 912 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 921
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 939
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1177
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1181
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 43 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 54
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 71
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 75
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 86
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 121 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 71
void*Cyc_Tcutil_compress(void*t);
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 281 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 322
int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 107
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 109
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 111
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 43 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 54
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 65
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 67
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 72
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 81
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 121
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 123
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 125
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 128
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 166
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 169
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 172
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple12{void*f1;struct Cyc_List_List*f2;};
struct _tuple12 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 180
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 185
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 206 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 210
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 215
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2);
# 221
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 226
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233
extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 237
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
# 239
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 36 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 39
Cyc_CfFlowInfo_anal_error=1;
Cyc_Position_post_error(Cyc_Position_mk_err(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 52 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 56
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _tmp3;(_tmp3.PlaceL).val=x;(_tmp3.PlaceL).tag=1;_tmp3;});}
# 59
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp4;(_tmp4.UnknownL).val=0;(_tmp4.UnknownL).tag=2;_tmp4;});}
# 62
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union Cyc_CfFlowInfo_FlowInfo _tmp5;(_tmp5.BottomFL).val=0;(_tmp5.BottomFL).tag=1;_tmp5;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp6;(_tmp6.ReachableFL).val=({struct _tuple11 _tmp7;_tmp7.f1=fd;_tmp7.f2=r;_tmp7;});(_tmp6.ReachableFL).tag=2;_tmp6;});}
# 68
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->r=r;_tmp8->zero=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1C=_region_malloc(r,sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp1D;_tmp1D.tag=0;_tmp1D;});_tmp1C;});_tmp8->notzeroall=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1A=_region_malloc(r,sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp1B;_tmp1B.tag=1;_tmp1B;});_tmp1A;});_tmp8->notzerothis=(void*)({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp18=_region_malloc(r,sizeof(*_tmp18));_tmp18[0]=({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp19;_tmp19.tag=2;_tmp19;});_tmp18;});_tmp8->unknown_none=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp16=_region_malloc(r,sizeof(*_tmp16));_tmp16[0]=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp17;_tmp17.tag=3;_tmp17.f1=Cyc_CfFlowInfo_NoneIL;_tmp17;});_tmp16;});_tmp8->unknown_this=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp14=_region_malloc(r,sizeof(*_tmp14));_tmp14[0]=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp15;_tmp15.tag=3;_tmp15.f1=Cyc_CfFlowInfo_ThisIL;_tmp15;});_tmp14;});_tmp8->unknown_all=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp12=_region_malloc(r,sizeof(*_tmp12));_tmp12[0]=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp13;_tmp13.tag=3;_tmp13.f1=Cyc_CfFlowInfo_AllIL;_tmp13;});_tmp12;});_tmp8->esc_none=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp10=_region_malloc(r,sizeof(*_tmp10));_tmp10[0]=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp11;_tmp11.tag=4;_tmp11.f1=Cyc_CfFlowInfo_NoneIL;_tmp11;});_tmp10;});_tmp8->esc_this=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpE=_region_malloc(r,sizeof(*_tmpE));_tmpE[0]=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpF;_tmpF.tag=4;_tmpF.f1=Cyc_CfFlowInfo_ThisIL;_tmpF;});_tmpE;});_tmp8->esc_all=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpC=_region_malloc(r,sizeof(*_tmpC));_tmpC[0]=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpD;_tmpD.tag=4;_tmpD.f1=Cyc_CfFlowInfo_AllIL;_tmpD;});_tmpC;});_tmp8->mt_flowdict=
# 84
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp);_tmp8->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp);_tmp8->dummy_place=({struct Cyc_CfFlowInfo_Place*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmpB;_tmpB.tag=1;_tmpB.f1=& dummy_exp;_tmpB.f2=(void*)& Cyc_Absyn_VoidType_val;_tmpB;});_tmpA;});_tmp9->fields=0;_tmp9;});_tmp8;});}
# 92
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc){
if(set != 0){
# 95
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))
return 1;else{
# 98
*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}
# 100
return 0;}
# 104
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp1F=*place;struct Cyc_CfFlowInfo_Place _tmp20=_tmp1F;void*_tmp29;struct Cyc_List_List*_tmp28;_LL1: _tmp29=_tmp20.root;_tmp28=_tmp20.fields;_LL2:;{
void*_tmp21=_tmp29;struct Cyc_Absyn_Vardecl*_tmp27;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21)->tag == 0){_LL4: _tmp27=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21)->f1;_LL5:
# 108
 if(_tmp28 == 0)
return Cyc_Absynpp_qvar2string(_tmp27->name);else{
# 111
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27->name));({void*_tmp22[1]={& _tmp24};Cyc_aprintf(({const char*_tmp23="reachable from %s";_tag_dyneither(_tmp23,sizeof(char),18);}),_tag_dyneither(_tmp22,sizeof(void*),1));});});}}else{_LL6: _LL7:
({void*_tmp25=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26="error locations not for VarRoots";_tag_dyneither(_tmp26,sizeof(char),33);}),_tag_dyneither(_tmp25,sizeof(void*),0));});}_LL3:;};}
# 117
static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,unsigned int loc1,unsigned int loc2){
# 121
return loc1;}
# 126
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){
# 133
int _tmp2A=0;
struct Cyc_Dict_Dict _tmp2B=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,s2);
# 138
return _tmp2B;}struct _tuple14{void*f1;unsigned int f2;};
# 141
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct _RegionHandle _tmp2F=_new_region("temp");struct _RegionHandle*temp=& _tmp2F;_push_region(temp);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple14 _tmp30=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp30)){
void*_tmp31=_tmp30.f1;
if(!((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp31)){
# 153
int _tmp32=0;_npop_handler(1);return _tmp32;}}{
# 156
int _tmp33=1;_npop_handler(1);return _tmp33;};}
# 145
;_pop_region(temp);}
# 144
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp34=_tmp2D;void*_tmp35;_LL9: _tmp35=_tmp34;_LLA:
# 160
 return 1;_LL8:;}};};}
# 164
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}
# 169
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp36=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp36,f)== 0)return n;
n=n + 1;}
# 176
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Core_Impossible_exn_struct _tmp38;_tmp38.tag=Cyc_Core_Impossible;_tmp38.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp39[1]={& _tmp3B};Cyc_aprintf(({const char*_tmp3A="get_field_index_fs failed: %s";_tag_dyneither(_tmp3A,sizeof(char),30);}),_tag_dyneither(_tmp39,sizeof(void*),1));});});_tmp38;});_tmp37;}));}
# 179
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp3C=Cyc_Tcutil_compress(t);void*_tmp3D=_tmp3C;struct Cyc_List_List*_tmp45;union Cyc_Absyn_AggrInfoU _tmp44;switch(*((int*)_tmp3D)){case 11: _LLE: _tmp44=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3D)->f1).aggr_info;_LLF: {
# 182
struct Cyc_List_List*_tmp3E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp44))->impl))->fields;
_tmp45=_tmp3E;goto _LL11;}case 12: _LL10: _tmp45=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D)->f2;_LL11:
# 185
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp45,f);default: _LL12: _LL13:
# 187
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Core_Impossible_exn_struct _tmp40;_tmp40.tag=Cyc_Core_Impossible;_tmp40.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp41[1]={& _tmp43};Cyc_aprintf(({const char*_tmp42="get_field_index failed: %s";_tag_dyneither(_tmp42,sizeof(char),27);}),_tag_dyneither(_tmp41,sizeof(void*),1));});});_tmp40;});_tmp3F;}));}_LLD:;}
# 192
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp46=({struct _tuple0 _tmp4E;_tmp4E.f1=r1;_tmp4E.f2=r2;_tmp4E;});struct _tuple0 _tmp47=_tmp46;int _tmp4D;int _tmp4C;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Exp*_tmp4A;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Vardecl*_tmp48;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f1)->tag == 0){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 0){_LL15: _tmp49=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f1)->f1;_tmp48=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f2)->f1;_LL16:
 return(int)_tmp49 - (int)_tmp48;}else{_LL17: _LL18:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 0){_LL19: _LL1A:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f1)->tag == 1){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 1){_LL1B: _tmp4B=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f1)->f1;_tmp4A=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f2)->f1;_LL1C:
 return(int)_tmp4B - (int)_tmp4A;}else{_LL1D: _LL1E:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 1){_LL1F: _LL20:
 return 1;}else{_LL21: _tmp4D=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp47.f1)->f1;_tmp4C=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp47.f2)->f1;_LL22:
 return _tmp4D - _tmp4C;}}}}_LL14:;};}
# 206
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 208
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 211
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp4F=*p1;struct Cyc_CfFlowInfo_Place _tmp50=_tmp4F;void*_tmp56;struct Cyc_List_List*_tmp55;_LL24: _tmp56=_tmp50.root;_tmp55=_tmp50.fields;_LL25:;{
struct Cyc_CfFlowInfo_Place _tmp51=*p2;struct Cyc_CfFlowInfo_Place _tmp52=_tmp51;void*_tmp54;struct Cyc_List_List*_tmp53;_LL27: _tmp54=_tmp52.root;_tmp53=_tmp52.fields;_LL28:;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp56,_tmp54);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp55,_tmp53);};};};}
# 223
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
struct Cyc_CfFlowInfo_Place _tmp57=*p;struct Cyc_CfFlowInfo_Place _tmp58=_tmp57;void*_tmp7C;struct Cyc_List_List*_tmp7B;_LL2A: _tmp7C=_tmp58.root;_tmp7B=_tmp58.fields;_LL2B:;
{void*_tmp59=_tmp7C;int _tmp6B;struct Cyc_Absyn_Exp*_tmp6A;struct Cyc_Absyn_Vardecl*_tmp69;switch(*((int*)_tmp59)){case 0: _LL2D: _tmp69=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp59)->f1;_LL2E:
 sl=({struct Cyc_List_List*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->hd=({struct _dyneither_ptr*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp69->name).f2);({void*_tmp5C[1]={& _tmp5E};Cyc_aprintf(({const char*_tmp5D="%s";_tag_dyneither(_tmp5D,sizeof(char),3);}),_tag_dyneither(_tmp5C,sizeof(void*),1));});});_tmp5B;});_tmp5A->tl=sl;_tmp5A;});goto _LL2C;case 1: _LL2F: _tmp6A=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp59)->f1;_LL30:
 sl=({struct Cyc_List_List*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->hd=({struct _dyneither_ptr*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Int_pa_PrintArg_struct _tmp63;_tmp63.tag=1;_tmp63.f1=(unsigned long)((int)_tmp6A);({void*_tmp61[1]={& _tmp63};Cyc_aprintf(({const char*_tmp62="mpt%d";_tag_dyneither(_tmp62,sizeof(char),6);}),_tag_dyneither(_tmp61,sizeof(void*),1));});});_tmp60;});_tmp5F->tl=sl;_tmp5F;});goto _LL2C;default: _LL31: _tmp6B=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp59)->f1;_LL32:
 sl=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=({struct _dyneither_ptr*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Int_pa_PrintArg_struct _tmp68;_tmp68.tag=1;_tmp68.f1=(unsigned long)_tmp6B;({void*_tmp66[1]={& _tmp68};Cyc_aprintf(({const char*_tmp67="param%d";_tag_dyneither(_tmp67,sizeof(char),8);}),_tag_dyneither(_tmp66,sizeof(void*),1));});});_tmp65;});_tmp64->tl=sl;_tmp64;});goto _LL2C;}_LL2C:;}
# 231
for(0;_tmp7B != 0;_tmp7B=_tmp7B->tl){
sl=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=({struct _dyneither_ptr*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Int_pa_PrintArg_struct _tmp70;_tmp70.tag=1;_tmp70.f1=(unsigned long)((int)_tmp7B->hd);({void*_tmp6E[1]={& _tmp70};Cyc_aprintf(({const char*_tmp6F="%d";_tag_dyneither(_tmp6F,sizeof(char),3);}),_tag_dyneither(_tmp6E,sizeof(void*),1));});});_tmp6D;});_tmp6C->tl=sl;_tmp6C;});}{
struct _dyneither_ptr*_tmp71=({struct _dyneither_ptr*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_String_pa_PrintArg_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct _dyneither_ptr)({const char*_tmp7A="";_tag_dyneither(_tmp7A,sizeof(char),1);});({void*_tmp77[1]={& _tmp79};Cyc_aprintf(({const char*_tmp78="%s";_tag_dyneither(_tmp78,sizeof(char),3);}),_tag_dyneither(_tmp77,sizeof(void*),1));});});_tmp76;});
for(0;sl != 0;sl=sl->tl){
*_tmp71=({struct Cyc_String_pa_PrintArg_struct _tmp75;_tmp75.tag=0;_tmp75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp71);({struct Cyc_String_pa_PrintArg_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd));({void*_tmp72[2]={& _tmp74,& _tmp75};Cyc_aprintf(({const char*_tmp73="%s.%s";_tag_dyneither(_tmp73,sizeof(char),6);}),_tag_dyneither(_tmp72,sizeof(void*),2));});});});}
return _tmp71;};}
# 249 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);
# 254
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 260
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
struct _dyneither_ptr d=({unsigned int _tmp81=sz;void**_tmp82=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp81));struct _dyneither_ptr _tmp84=_tag_dyneither(_tmp82,sizeof(void*),_tmp81);{unsigned int _tmp83=_tmp81;unsigned int i;for(i=0;i < _tmp83;i ++){_tmp82[i]=(void*)fenv->unknown_all;}}_tmp84;});
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp7D=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_tmp7E=_tmp7D;struct _dyneither_ptr*_tmp80;void*_tmp7F;_LL34: _tmp80=_tmp7E->name;_tmp7F=_tmp7E->type;_LL35:;
if(_get_dyneither_size(*_tmp80,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp7F,leafval);}}
# 268
return d;}struct _tuple15{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 272
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){
# 276
struct _tuple15*_tmp85=env;struct _RegionHandle*_tmp8B;struct Cyc_List_List*_tmp8A;_LL37: _tmp8B=_tmp85->f1;_tmp8A=_tmp85->f2;_LL38:;{
void*_tmp86=Cyc_Tcutil_rsubstitute(_tmp8B,_tmp8A,f->type);
struct Cyc_Absyn_Exp*_tmp87=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp88=_tmp87 == 0?0: Cyc_Tcutil_rsubsexp(_tmp8B,_tmp8A,_tmp87);
return({struct Cyc_Absyn_Aggrfield*_tmp89=_region_malloc(_tmp8B,sizeof(*_tmp89));_tmp89->name=f->name;_tmp89->tq=f->tq;_tmp89->type=_tmp86;_tmp89->width=f->width;_tmp89->attributes=f->attributes;_tmp89->requires_clause=_tmp88;_tmp89;});};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 287
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 293
struct _RegionHandle _tmp8C=_new_region("temp");struct _RegionHandle*temp=& _tmp8C;_push_region(temp);
# 297
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp8D=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8E=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp8F=Cyc_Tcutil_tvar_kind(_tmp8D,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp90=_tmp8F;switch(((struct Cyc_Absyn_Kind*)_tmp90)->kind){case Cyc_Absyn_RgnKind: _LL3A: _LL3B:
 goto _LL3D;case Cyc_Absyn_EffKind: _LL3C: _LL3D:
 continue;default: _LL3E: _LL3F:
 goto _LL39;}_LL39:;}
# 306
inst=({struct Cyc_List_List*_tmp91=_region_malloc(temp,sizeof(*_tmp91));_tmp91->hd=({struct _tuple16*_tmp92=_region_malloc(temp,sizeof(*_tmp92));_tmp92->f1=_tmp8D;_tmp92->f2=_tmp8E;_tmp92;});_tmp91->tl=inst;_tmp91;});}
# 308
if(inst != 0){
struct _tuple15 _tmp93=({struct _tuple15 _tmp95;_tmp95.f1=temp;_tmp95.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);_tmp95;});
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp93,fs);
struct _dyneither_ptr _tmp94=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp94;}else{
# 313
struct _dyneither_ptr _tmp96=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp96;}}
# 297
;_pop_region(temp);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 317
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 323
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 325
{void*_tmp97=Cyc_Tcutil_compress(t);void*_tmp98=_tmp97;union Cyc_Absyn_Constraint*_tmpB2;void*_tmpB1;void*_tmpB0;union Cyc_Absyn_Constraint*_tmpAF;enum Cyc_Absyn_AggrKind _tmpAE;struct Cyc_List_List*_tmpAD;union Cyc_Absyn_AggrInfoU _tmpAC;struct Cyc_List_List*_tmpAB;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Datatypefield*_tmpA9;switch(*((int*)_tmp98)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp98)->f1).field_info).KnownDatatypefield).tag == 2){_LL41: _tmpA9=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp98)->f1).field_info).KnownDatatypefield).val).f2;_LL42:
# 327
 if(_tmpA9->typs == 0)
return leafval;
_tmpAA=_tmpA9->typs;goto _LL44;}else{goto _LL4F;}case 10: _LL43: _tmpAA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp98)->f1;_LL44: {
# 331
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAA);
# 334
struct _dyneither_ptr d=({unsigned int _tmp9C=sz;void**_tmp9D=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp9C));struct _dyneither_ptr _tmp9F=_tag_dyneither(_tmp9D,sizeof(void*),_tmp9C);{unsigned int _tmp9E=_tmp9C;unsigned int i;for(i=0;i < _tmp9E;i ++){_tmp9D[i]=(void*)fenv->unknown_all;}}_tmp9F;});
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpAA))->hd)).f2,leafval);
_tmpAA=_tmpAA->tl;}}
# 339
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp99=_region_malloc(fenv->r,sizeof(*_tmp99));_tmp99[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp9A;_tmp9A.tag=6;_tmp9A.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp9B;_tmp9B.is_union=0;_tmp9B.fieldnum=- 1;_tmp9B;});_tmp9A.f2=d;_tmp9A;});_tmp99;});}case 11: _LL45: _tmpAC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp98)->f1).aggr_info;_tmpAB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp98)->f1).targs;_LL46: {
# 341
struct Cyc_Absyn_Aggrdecl*_tmpA0=Cyc_Absyn_get_known_aggrdecl(_tmpAC);
if(_tmpA0->impl == 0)
goto _LL40;{
struct Cyc_List_List*_tmpA1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA0->impl))->fields;
if(_tmpAB == 0){_tmpAE=_tmpA0->kind;_tmpAD=_tmpA1;goto _LL48;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpA2=_region_malloc(fenv->r,sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpA3;_tmpA3.tag=6;_tmpA3.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmpA4;_tmpA4.is_union=_tmpA0->kind == Cyc_Absyn_UnionA;_tmpA4.fieldnum=- 1;_tmpA4;});_tmpA3.f2=
Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpA0->tvs,_tmpAB,_tmpA1,_tmpA0->kind == Cyc_Absyn_UnionA,leafval);_tmpA3;});_tmpA2;});};}case 12: _LL47: _tmpAE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp98)->f1;_tmpAD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp98)->f2;_LL48:
# 352
 return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpA5=_region_malloc(fenv->r,sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpA6;_tmpA6.tag=6;_tmpA6.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmpA7;_tmpA7.is_union=_tmpAE == Cyc_Absyn_UnionA;_tmpA7.fieldnum=- 1;_tmpA7;});_tmpA6.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpAD,_tmpAE == Cyc_Absyn_UnionA,leafval);_tmpA6;});_tmpA5;});case 8: _LL49: _tmpB0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp98)->f1).elt_type;_tmpAF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp98)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAF)){_LL4A:
# 356
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpB0)?fenv->unknown_all: leafval;}else{goto _LL4F;}case 19: _LL4B: _tmpB1=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp98)->f1;_LL4C:
 return leafval;case 5: _LL4D: _tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98)->f1).ptr_atts).nullable;if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB2)){_LL4E:
{void*_tmpA8=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA8)->tag == 3)switch(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA8)->f1){case Cyc_CfFlowInfo_ThisIL: _LL52: _LL53:
 return fenv->notzerothis;case Cyc_CfFlowInfo_AllIL: _LL54: _LL55:
 return fenv->notzeroall;default: goto _LL56;}else{_LL56: _LL57:
 goto _LL51;}_LL51:;}
# 364
goto _LL40;}else{goto _LL4F;}default: _LL4F: _LL50:
 goto _LL40;}_LL40:;}
# 368
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 371
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 377
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpB3=r;void*_tmpBB;int _tmpBA;int _tmpB9;struct _dyneither_ptr _tmpB8;struct Cyc_Absyn_Exp*_tmpB7;int _tmpB6;void*_tmpB5;switch(*((int*)_tmpB3)){case 7: _LL59: _tmpB7=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_tmpB6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_tmpB5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f3;_LL5A:
# 380
 if(_tmpB7 == e  && _tmpB6 == env_iteration){
*needs_unconsume=1;
return 0;}
# 384
return 1;case 6: _LL5B: _tmpBA=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1).is_union;_tmpB9=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1).fieldnum;_tmpB8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_LL5C:
# 386
 if(!_tmpBA  || _tmpB9 == - 1){
unsigned int _tmpB4=_get_dyneither_size(_tmpB8,sizeof(void*));
{int i=0;for(0;i < _tmpB4;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmpB8.curr)[i],needs_unconsume))
return 1;}}
# 392
return 0;}else{
# 395
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpB8,sizeof(void*),_tmpB9)),needs_unconsume);}case 8: _LL5D: _tmpBB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_LL5E:
# 397
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpBB,needs_unconsume);default: _LL5F: _LL60:
 return 0;}_LL58:;}
# 404
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpBC=r;struct Cyc_Absyn_Vardecl*_tmpCD;void*_tmpCC;struct Cyc_CfFlowInfo_UnionRInfo _tmpCB;struct _dyneither_ptr _tmpCA;struct Cyc_Absyn_Exp*_tmpC9;int _tmpC8;void*_tmpC7;switch(*((int*)_tmpBC)){case 7: _LL62: _tmpC9=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f1;_tmpC8=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f2;_tmpC7=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f3;_LL63:
# 407
 return _tmpC7;case 6: _LL64: _tmpCB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f1;_tmpCA=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f2;_LL65: {
# 409
unsigned int _tmpBD=_get_dyneither_size(_tmpCA,sizeof(void*));
int change=0;
struct _dyneither_ptr d2=({unsigned int _tmpC0=_tmpBD;void**_tmpC1=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmpC0));struct _dyneither_ptr _tmpC3=_tag_dyneither(_tmpC1,sizeof(void*),_tmpC0);{unsigned int _tmpC2=_tmpC0;unsigned int i;for(i=0;i < _tmpC2;i ++){_tmpC1[i]=(void*)((void**)_tmpCA.curr)[(int)i];}}_tmpC3;});
{int i=0;for(0;i < _tmpBD;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmpCA.curr)[i]);
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= ((void**)_tmpCA.curr)[i])
change=1;}}
# 417
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpBE=_region_malloc(fenv->r,sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpBF;_tmpBF.tag=6;_tmpBF.f1=_tmpCB;_tmpBF.f2=d2;_tmpBF;});_tmpBE;});else{
return r;}}case 8: _LL66: _tmpCD=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f1;_tmpCC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f2;_LL67: {
# 421
void*_tmpC4=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpCC);
if(_tmpC4 != _tmpCC)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC5=_region_malloc(fenv->r,sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpC6;_tmpC6.tag=8;_tmpC6.f1=_tmpCD;_tmpC6.f2=_tmpC4;_tmpC6;});_tmpC5;});else{
return r;}}default: _LL68: _LL69:
 return r;}_LL61:;}
# 431
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmpCE=({struct _tuple0 _tmpF3;_tmpF3.f1=Cyc_Tcutil_compress(t);_tmpF3.f2=r;_tmpF3;});struct _tuple0 _tmpCF=_tmpCE;enum Cyc_Absyn_AggrKind _tmpF2;struct Cyc_List_List*_tmpF1;struct Cyc_CfFlowInfo_UnionRInfo _tmpF0;struct _dyneither_ptr _tmpEF;union Cyc_Absyn_AggrInfoU _tmpEE;struct Cyc_CfFlowInfo_UnionRInfo _tmpED;struct _dyneither_ptr _tmpEC;struct Cyc_List_List*_tmpEB;struct Cyc_CfFlowInfo_UnionRInfo _tmpEA;struct _dyneither_ptr _tmpE9;struct Cyc_Absyn_Vardecl*_tmpE8;void*_tmpE7;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 8){_LL6B: _tmpE8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpE7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL6C: {
# 434
void*_tmpD0=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpE7);
if(_tmpD0 != _tmpE7)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpD1=_region_malloc(fenv->r,sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpD2;_tmpD2.tag=8;_tmpD2.f1=_tmpE8;_tmpD2.f2=_tmpD0;_tmpD2;});_tmpD1;});else{
return r;}}}else{switch(*((int*)_tmpCF.f1)){case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 6){_LL6D: _tmpEB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCF.f1)->f1;_tmpEA=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpE9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL6E: {
# 438
unsigned int _tmpD3=_get_dyneither_size(_tmpE9,sizeof(void*));
struct _dyneither_ptr d2=({unsigned int _tmpD6=_tmpD3;void**_tmpD7=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmpD6));struct _dyneither_ptr _tmpD9=_tag_dyneither(_tmpD7,sizeof(void*),_tmpD6);{unsigned int _tmpD8=_tmpD6;unsigned int i;for(i=0;i < _tmpD8;i ++){_tmpD7[i]=(void*)fenv->unknown_all;}}_tmpD9;});
{int i=0;for(0;i < _tmpD3;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpEB))->hd)).f2,consumer,iteration,((void**)_tmpE9.curr)[i]);
_tmpEB=_tmpEB->tl;}}
# 444
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD4=_region_malloc(fenv->r,sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpD5;_tmpD5.tag=6;_tmpD5.f1=_tmpEA;_tmpD5.f2=d2;_tmpD5;});_tmpD4;});}}else{goto _LL73;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 6){_LL6F: _tmpEE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCF.f1)->f1).aggr_info;_tmpED=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpEC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL70: {
# 446
struct Cyc_Absyn_Aggrdecl*_tmpDA=Cyc_Absyn_get_known_aggrdecl(_tmpEE);
if(_tmpDA->impl == 0)return r;
_tmpF2=_tmpDA->kind;_tmpF1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDA->impl))->fields;_tmpF0=_tmpED;_tmpEF=_tmpEC;goto _LL72;}}else{goto _LL73;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 6){_LL71: _tmpF2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCF.f1)->f1;_tmpF1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCF.f1)->f2;_tmpF0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpEF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL72: {
# 450
struct _dyneither_ptr d2=({unsigned int _tmpE1=_get_dyneither_size(_tmpEF,sizeof(void*));void**_tmpE2=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmpE1));struct _dyneither_ptr _tmpE4=_tag_dyneither(_tmpE2,sizeof(void*),_tmpE1);{unsigned int _tmpE3=_tmpE1;unsigned int i;for(i=0;i < _tmpE3;i ++){_tmpE2[i]=(void*)((void**)_tmpEF.curr)[(int)i];}}_tmpE4;});
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF1);
{int i=0;for(0;i < sz;(i ++,_tmpF1=_tmpF1->tl)){
struct Cyc_Absyn_Aggrfield*_tmpDB=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpF1))->hd;struct Cyc_Absyn_Aggrfield*_tmpDC=_tmpDB;struct _dyneither_ptr*_tmpDE;void*_tmpDD;_LL76: _tmpDE=_tmpDC->name;_tmpDD=_tmpDC->type;_LL77:;
if(_get_dyneither_size(*_tmpDE,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpDD,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpEF,sizeof(void*),i)));}}
# 457
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpDF=_region_malloc(fenv->r,sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpE0;_tmpE0.tag=6;_tmpE0.f1=_tmpF0;_tmpE0.f2=d2;_tmpE0;});_tmpDF;});}}else{goto _LL73;}default: _LL73: _LL74:
# 459
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE5=_region_malloc(fenv->r,sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmpE6;_tmpE6.tag=7;_tmpE6.f1=consumer;_tmpE6.f2=iteration;_tmpE6.f3=r;_tmpE6;});_tmpE5;});else{
return r;}}}_LL6A:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 465
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 468
struct _RegionHandle _tmpF4=_new_region("r");struct _RegionHandle*r=& _tmpF4;_push_region(r);
{struct _tuple18 elem=({struct _tuple18 _tmp100;_tmp100.f1=place;_tmp100.f2=0;_tmp100;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmpF5=elem.f1;
struct Cyc_CfFlowInfo_Place _tmpF6=*place;struct Cyc_CfFlowInfo_Place _tmpF7=_tmpF6;void*_tmpFE;struct Cyc_List_List*_tmpFD;_LL79: _tmpFE=_tmpF7.root;_tmpFD=_tmpF7.fields;_LL7A:;{
struct Cyc_CfFlowInfo_Place _tmpF8=*_tmpF5;struct Cyc_CfFlowInfo_Place _tmpF9=_tmpF8;void*_tmpFC;struct Cyc_List_List*_tmpFB;_LL7C: _tmpFC=_tmpF9.root;_tmpFB=_tmpF9.fields;_LL7D:;
if(Cyc_CfFlowInfo_root_cmp(_tmpFE,_tmpFC)!= 0)
continue;
for(0;_tmpFD != 0  && _tmpFB != 0;(_tmpFD=_tmpFD->tl,_tmpFB=_tmpFB->tl)){
if((int)_tmpFD->hd != (int)_tmpFB->hd)break;}
if(_tmpFD == 0){
int _tmpFA=1;_npop_handler(0);return _tmpFA;}};}{
# 482
int _tmpFF=0;_npop_handler(0);return _tmpFF;};}
# 469
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 495
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 497
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 500
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
pile->places=({struct Cyc_List_List*_tmp101=_region_malloc(pile->rgn,sizeof(*_tmp101));_tmp101->hd=place;_tmp101->tl=pile->places;_tmp101;});}
# 503
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp102=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp107;struct _dyneither_ptr _tmp106;struct Cyc_CfFlowInfo_Place*_tmp105;void*_tmp104;void*_tmp103;switch(*((int*)_tmp102)){case 7: _LL7F: _tmp103=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp102)->f3;_LL80:
 Cyc_CfFlowInfo_add_places(pile,_tmp103);return;case 8: _LL81: _tmp104=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102)->f2;_LL82:
 Cyc_CfFlowInfo_add_places(pile,_tmp104);return;case 5: _LL83: _tmp105=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp102)->f1;_LL84:
 Cyc_CfFlowInfo_add_place(pile,_tmp105);return;case 6: _LL85: _tmp107=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102)->f1;_tmp106=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102)->f2;_LL86:
# 509
{int i=0;for(0;i < _get_dyneither_size(_tmp106,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp106.curr)[i]);}}
return;default: _LL87: _LL88:
 return;}_LL7E:;}
# 519
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 521
void*_tmp108=old_val;struct Cyc_Absyn_Vardecl*_tmp11A;void*_tmp119;struct Cyc_Absyn_Exp*_tmp118;int _tmp117;void*_tmp116;int _tmp115;struct _dyneither_ptr _tmp114;switch(*((int*)_tmp108)){case 6: _LL8A: _tmp115=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp108)->f1).is_union;_tmp114=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp108)->f2;_LL8B: {
# 523
struct _dyneither_ptr d2=({unsigned int _tmp10C=_get_dyneither_size(_tmp114,sizeof(void*));void**_tmp10D=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp10C));struct _dyneither_ptr _tmp10F=_tag_dyneither(_tmp10D,sizeof(void*),_tmp10C);{unsigned int _tmp10E=_tmp10C;unsigned int i;for(i=0;i < _tmp10E;i ++){_tmp10D[i]=(void*)
Cyc_CfFlowInfo_insert_place_inner(r,new_val,((void**)_tmp114.curr)[(int)i]);}}_tmp10F;});
# 527
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp109=_region_malloc(r,sizeof(*_tmp109));_tmp109[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp10A;_tmp10A.tag=6;_tmp10A.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp10B;_tmp10B.is_union=_tmp115;_tmp10B.fieldnum=- 1;_tmp10B;});_tmp10A.f2=d2;_tmp10A;});_tmp109;});}case 7: _LL8C: _tmp118=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp108)->f1;_tmp117=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp108)->f2;_tmp116=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp108)->f3;_LL8D:
# 529
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp110=_region_malloc(r,sizeof(*_tmp110));_tmp110[0]=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp111;_tmp111.tag=7;_tmp111.f1=_tmp118;_tmp111.f2=_tmp117;_tmp111.f3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp116);_tmp111;});_tmp110;});case 8: _LL8E: _tmp11A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp108)->f1;_tmp119=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp108)->f2;_LL8F:
# 531
 return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp112=_region_malloc(r,sizeof(*_tmp112));_tmp112[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp113;_tmp113.tag=8;_tmp113.f1=_tmp11A;_tmp113.f2=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp119);_tmp113;});_tmp112;});default: _LL90: _LL91:
 return new_val;}_LL89:;}
# 538
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 540
void*_tmp11B=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp11B == rval)return d;{
struct _dyneither_ptr res=({unsigned int _tmp11C=_get_dyneither_size(d,sizeof(void*));void**_tmp11D=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp11C));struct _dyneither_ptr _tmp11F=_tag_dyneither(_tmp11D,sizeof(void*),_tmp11C);{unsigned int _tmp11E=_tmp11C;unsigned int i;for(i=0;i < _tmp11E;i ++){_tmp11D[i]=(void*)((void**)d.curr)[(int)i];}}_tmp11F;});
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple19{struct Cyc_List_List*f1;void*f2;};
# 552
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 556
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple19 _tmp120=({struct _tuple19 _tmp133;_tmp133.f1=fs;_tmp133.f2=old_val;_tmp133;});struct _tuple19 _tmp121=_tmp120;struct Cyc_Absyn_Vardecl*_tmp132;void*_tmp131;int _tmp130;struct Cyc_List_List*_tmp12F;int _tmp12E;struct _dyneither_ptr _tmp12D;if(_tmp121.f1 != 0)switch(*((int*)_tmp121.f2)){case 6: _LL93: _tmp130=(int)(_tmp121.f1)->hd;_tmp12F=(_tmp121.f1)->tl;_tmp12E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f1).is_union;_tmp12D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f2;_LL94: {
# 560
void*_tmp122=Cyc_CfFlowInfo_insert_place_outer(r,_tmp12F,*((void**)_check_dyneither_subscript(_tmp12D,sizeof(void*),_tmp130)),new_val);
struct _dyneither_ptr _tmp123=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp12D,_tmp130,_tmp122);
if((void**)_tmp123.curr == (void**)_tmp12D.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp124=_region_malloc(r,sizeof(*_tmp124));_tmp124[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp125;_tmp125.tag=6;_tmp125.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp126;_tmp126.is_union=_tmp12E;_tmp126.fieldnum=- 1;_tmp126;});_tmp125.f2=_tmp123;_tmp125;});_tmp124;});}}case 8: goto _LL95;default: goto _LL97;}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->tag == 8){_LL95: _tmp132=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f1;_tmp131=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f2;_LL96: {
# 565
void*_tmp127=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp131,new_val);
if(_tmp127 == _tmp131)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp128=_region_malloc(r,sizeof(*_tmp128));_tmp128[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp129;_tmp129.tag=8;_tmp129.f1=_tmp132;_tmp129.f2=_tmp127;_tmp129;});_tmp128;});}}}else{_LL97: _LL98:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=({struct Cyc_Core_Impossible_exn_struct _tmp12B;_tmp12B.tag=Cyc_Core_Impossible;_tmp12B.f1=({const char*_tmp12C="bad insert place";_tag_dyneither(_tmp12C,sizeof(char),17);});_tmp12B;});_tmp12A;}));}}_LL92:;};}
# 575
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 579
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp134=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp134,0);{
void*oldval;void*newval;
{struct _handler_cons _tmp135;_push_handler(& _tmp135);{int _tmp137=0;if(setjmp(_tmp135.handler))_tmp137=1;if(!_tmp137){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp134);;_pop_handler();}else{void*_tmp136=(void*)_exn_thrown;void*_tmp138=_tmp136;void*_tmp139;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp138)->tag == Cyc_Dict_Absent){_LL9A: _LL9B:
 continue;}else{_LL9C: _tmp139=_tmp138;_LL9D:(int)_rethrow(_tmp139);}_LL99:;}};}
{enum Cyc_CfFlowInfo_InitLevel _tmp13A=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _tmp13B=_tmp13A;switch(_tmp13B){case Cyc_CfFlowInfo_AllIL: _LL9F: _LLA0:
 newval=fenv->esc_all;goto _LL9E;case Cyc_CfFlowInfo_ThisIL: _LLA1: _LLA2:
 newval=fenv->esc_this;goto _LL9E;default: _LLA3: _LLA4:
 newval=fenv->esc_none;goto _LL9E;}_LL9E:;}
# 591
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmp13C=*_tmp134;struct Cyc_CfFlowInfo_Place _tmp13D=_tmp13C;void*_tmp13F;struct Cyc_List_List*_tmp13E;_LLA6: _tmp13F=_tmp13D.root;_tmp13E=_tmp13D.fields;_LLA7:;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp13F,
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp13E,
# 596
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp13F),newval));};};}
# 599
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 611
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp140=r;void*_tmp146;enum Cyc_CfFlowInfo_InitLevel _tmp145;enum Cyc_CfFlowInfo_InitLevel _tmp144;switch(*((int*)_tmp140)){case 3: _LLA9: _tmp144=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp140)->f1;_LLAA:
 return _tmp144;case 4: _LLAB: _tmp145=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp140)->f1;_LLAC:
 return _tmp145;case 0: _LLAD: _LLAE:
 goto _LLB0;case 1: _LLAF: _LLB0:
 return Cyc_CfFlowInfo_AllIL;case 2: _LLB1: _LLB2:
 return Cyc_CfFlowInfo_ThisIL;case 7: _LLB3: _tmp146=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp140)->f3;_LLB4:
 return Cyc_CfFlowInfo_NoneIL;default: _LLB5: _LLB6:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141[0]=({struct Cyc_Core_Impossible_exn_struct _tmp142;_tmp142.tag=Cyc_Core_Impossible;_tmp142.f1=({const char*_tmp143="initlevel_approx";_tag_dyneither(_tmp143,sizeof(char),17);});_tmp142;});_tmp141;}));}_LLA8:;}
# 622
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 624
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp147=r;struct Cyc_CfFlowInfo_Place*_tmp150;struct _dyneither_ptr _tmp14F;int _tmp14E;int _tmp14D;struct _dyneither_ptr _tmp14C;void*_tmp14B;switch(*((int*)_tmp147)){case 8: _LLB8: _tmp14B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp147)->f2;_LLB9:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp14B,acc);case 6: _LLBA: _tmp14E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f1).is_union;_tmp14D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f1).fieldnum;_tmp14C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f2;if(_tmp14E){_LLBB: {
# 632
unsigned int _tmp148=_get_dyneither_size(_tmp14C,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp14D == - 1){
int i=0;for(0;i < _tmp148;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp14C.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 642
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp14C,sizeof(void*),_tmp14D)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LLB7;}}else{_LLBC: _tmp14F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f2;_LLBD: {
# 646
unsigned int _tmp149=_get_dyneither_size(_tmp14F,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp149;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp14F.curr)[i],this_ans);}}
goto _LLB7;}}case 5: _LLBE: _tmp150=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp147)->f1;_LLBF:
# 652
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp150))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 655
env->seen=({struct Cyc_List_List*_tmp14A=_region_malloc(env->e,sizeof(*_tmp14A));_tmp14A->hd=_tmp150;_tmp14A->tl=env->seen;_tmp14A;});
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp150),Cyc_CfFlowInfo_AllIL);
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 661
goto _LLB7;default: _LLC0: _LLC1:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);}_LLB7:;}
# 664
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 668
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp151=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp152=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp153;_tmp153.e=_tmp151;_tmp153.d=d;_tmp153.seen=0;_tmp153;});
return Cyc_CfFlowInfo_initlevel_rec(& _tmp152,r,Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};
# 675
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp154=*place;struct Cyc_CfFlowInfo_Place _tmp155=_tmp154;void*_tmp163;struct Cyc_List_List*_tmp162;_LLC3: _tmp163=_tmp155.root;_tmp162=_tmp155.fields;_LLC4:;{
void*_tmp156=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp163);
for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
retry: {struct _tuple20 _tmp157=({struct _tuple20 _tmp161;_tmp161.f1=_tmp156;_tmp161.f2=(int)_tmp162->hd;_tmp161;});struct _tuple20 _tmp158=_tmp157;struct Cyc_CfFlowInfo_UnionRInfo _tmp160;struct _dyneither_ptr _tmp15F;int _tmp15E;void*_tmp15D;int _tmp15C;switch(*((int*)_tmp158.f1)){case 8: _LLC6: _tmp15D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp158.f1)->f2;_tmp15C=_tmp158.f2;_LLC7:
# 681
 _tmp156=_tmp15D;goto retry;case 6: _LLC8: _tmp160=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp158.f1)->f1;_tmp15F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp158.f1)->f2;_tmp15E=_tmp158.f2;_LLC9:
# 683
 _tmp156=*((void**)_check_dyneither_subscript(_tmp15F,sizeof(void*),_tmp15E));
goto _LLC5;default: _LLCA: _LLCB:
# 689
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[0]=({struct Cyc_Core_Impossible_exn_struct _tmp15A;_tmp15A.tag=Cyc_Core_Impossible;_tmp15A.f1=({const char*_tmp15B="bad lookup_place";_tag_dyneither(_tmp15B,sizeof(char),17);});_tmp15A;});_tmp159;}));}_LLC5:;}}
# 691
return _tmp156;};}
# 694
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp164=rval;int _tmp169;int _tmp168;struct _dyneither_ptr _tmp167;void*_tmp166;void*_tmp165;switch(*((int*)_tmp164)){case 4: _LLCD: _LLCE:
 return 0;case 7: _LLCF: _tmp165=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp164)->f3;_LLD0:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp165);case 8: _LLD1: _tmp166=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp164)->f2;_LLD2:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp166);case 6: _LLD3: _tmp169=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164)->f1).is_union;_tmp168=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164)->f1).fieldnum;_tmp167=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164)->f2;_LLD4:
# 700
 if(_tmp169  && _tmp168 != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp167,sizeof(void*),_tmp168)));else{
# 703
unsigned int sz=_get_dyneither_size(_tmp167,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp167.curr)[i]))return 0;}}
return 1;}default: _LLD5: _LLD6:
# 708
 return 1;}_LLCC:;}
# 711
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 716
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp16A=fenv->r;
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp16B=_region_malloc(_tmp16A,sizeof(*_tmp16B));_tmp16B->rgn=_tmp16A;_tmp16B->places=0;_tmp16B;});
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};
# 734
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 739
struct _tuple0 _tmp16C=({struct _tuple0 _tmp183;_tmp183.f1=oldval;_tmp183.f2=newval;_tmp183;});struct _tuple0 _tmp16D=_tmp16C;enum Cyc_CfFlowInfo_InitLevel _tmp182;struct Cyc_CfFlowInfo_UnionRInfo _tmp181;struct _dyneither_ptr _tmp180;struct Cyc_CfFlowInfo_UnionRInfo _tmp17F;struct _dyneither_ptr _tmp17E;struct Cyc_CfFlowInfo_Place*_tmp17D;struct Cyc_Absyn_Vardecl*_tmp17C;void*_tmp17B;void*_tmp17A;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->tag == 8){_LLD8: _tmp17A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->f2;_LLD9:
# 741
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp17A,newval);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->tag == 8){_LLDA: _tmp17C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_tmp17B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f2;_LLDB: {
# 743
void*_tmp16E=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp17B);
if(_tmp16E == _tmp17B)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp16F=_region_malloc((env->fenv)->r,sizeof(*_tmp16F));_tmp16F[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp170;_tmp170.tag=8;_tmp170.f1=_tmp17C;_tmp170.f2=_tmp16E;_tmp170;});_tmp16F;});}}}else{switch(*((int*)_tmp16D.f1)){case 4: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->tag == 5){_LLDC: _tmp17D=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_LLDD:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp17D);goto _LLDF;}else{_LLDE: _LLDF:
# 748
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp171=0;Cyc_CfFlowInfo_aerr(env->loc,({const char*_tmp172="assignment puts possibly-uninitialized data in an escaped location";_tag_dyneither(_tmp172,sizeof(char),67);}),_tag_dyneither(_tmp171,sizeof(void*),0));});
# 751
return(env->fenv)->esc_all;}case 6: switch(*((int*)_tmp16D.f2)){case 6: _LLE0: _tmp181=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->f1;_tmp180=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->f2;_tmp17F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_tmp17E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f2;_LLE1: {
# 753
struct _dyneither_ptr new_d=({unsigned int _tmp175=
_get_dyneither_size(_tmp180,sizeof(void*));void**_tmp176=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp175));struct _dyneither_ptr _tmp178=_tag_dyneither(_tmp176,sizeof(void*),_tmp175);{unsigned int _tmp177=_tmp175;unsigned int i;for(i=0;i < _tmp177;i ++){_tmp176[i]=(void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp180.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp17E,sizeof(void*),(int)i)));}}_tmp178;});
# 757
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp180,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp180.curr)[i]){
change=1;break;}}}
# 762
if(!change){
if(!_tmp181.is_union)return oldval;
new_d=_tmp180;}else{
# 767
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp180,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp17E,sizeof(void*),i))){
change=1;break;}}}
# 772
if(!change){
if(!_tmp181.is_union)return newval;
new_d=_tmp17E;}}
# 777
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp173=_region_malloc((env->fenv)->r,sizeof(*_tmp173));_tmp173[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp174;_tmp174.tag=6;_tmp174.f1=_tmp17F;_tmp174.f2=new_d;_tmp174;});_tmp173;});}case 4: goto _LLE2;default: goto _LLE4;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->tag == 4){_LLE2: _tmp182=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_LLE3: {
# 779
enum Cyc_CfFlowInfo_InitLevel _tmp179=_tmp182;switch(_tmp179){case Cyc_CfFlowInfo_NoneIL: _LLE7: _LLE8:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLE9: _LLEA:
 return(env->fenv)->unknown_this;default: _LLEB: _LLEC:
 return(env->fenv)->unknown_all;}_LLE6:;}}else{_LLE4: _LLE5:
# 784
 return newval;}}}}_LLD7:;}
# 790
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 794
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 799
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple19 _tmp184=({struct _tuple19 _tmp1A1;_tmp1A1.f1=fs;_tmp1A1.f2=oldval;_tmp1A1;});struct _tuple19 _tmp185=_tmp184;int _tmp1A0;struct Cyc_List_List*_tmp19F;int _tmp19E;int _tmp19D;struct _dyneither_ptr _tmp19C;struct Cyc_Absyn_Vardecl*_tmp19B;void*_tmp19A;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->tag == 8){_LLEE: _tmp19B=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f1;_tmp19A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f2;_LLEF: {
# 802
void*_tmp186=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp19A,newval);
if(_tmp186 == _tmp19A)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp187=_region_malloc((env->fenv)->r,sizeof(*_tmp187));_tmp187[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp188;_tmp188.tag=8;_tmp188.f1=_tmp19B;_tmp188.f2=_tmp186;_tmp188;});_tmp187;});}}}else{if(_tmp185.f1 != 0){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->tag == 6){_LLF0: _tmp1A0=(int)(_tmp185.f1)->hd;_tmp19F=(_tmp185.f1)->tl;_tmp19E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f1).is_union;_tmp19D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f1).fieldnum;_tmp19C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f2;_LLF1: {
# 806
void*_tmp189=Cyc_CfFlowInfo_assign_place_outer(env,_tmp19F,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp19C,sizeof(void*),_tmp1A0)),newval);
# 808
struct _dyneither_ptr _tmp18A=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp19C,_tmp1A0,_tmp189);
if((void**)_tmp18A.curr == (void**)_tmp19C.curr  && (!_tmp19E  || _tmp19D == _tmp1A0))return oldval;
_tmp19C=_tmp18A;
# 813
if(_tmp19E){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp19C,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp1A0){
struct _dyneither_ptr _tmp18B=
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp19C,i,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp19C,sizeof(void*),i))));
# 823
if((void**)_tmp18B.curr != (void**)_tmp19C.curr){
_tmp19C=_tmp18B;
changed=1;}}}}
# 831
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp18C=env->root_place;struct Cyc_CfFlowInfo_Place*_tmp18D=_tmp18C;void*_tmp193;struct Cyc_List_List*_tmp192;_LLF5: _tmp193=_tmp18D->root;_tmp192=_tmp18D->fields;_LLF6:;{
struct Cyc_List_List*_tmp18E=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,({int*_tmp191=_cycalloc_atomic(sizeof(*_tmp191));_tmp191[0]=fs_prefix;_tmp191;}),_tmp192);
struct Cyc_CfFlowInfo_Place*_tmp18F=({struct Cyc_CfFlowInfo_Place*_tmp190=_region_malloc((env->pile)->rgn,sizeof(*_tmp190));_tmp190->root=_tmp193;_tmp190->fields=_tmp18E;_tmp190;});
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp18F,0);};}}
# 838
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp194=_region_malloc((env->fenv)->r,sizeof(*_tmp194));_tmp194[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp195;_tmp195.tag=6;_tmp195.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp196;_tmp196.is_union=_tmp19E;_tmp196.fieldnum=_tmp1A0;_tmp196;});_tmp195.f2=_tmp19C;_tmp195;});_tmp194;});}}else{goto _LLF2;}}else{_LLF2: _LLF3:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197[0]=({struct Cyc_Core_Impossible_exn_struct _tmp198;_tmp198.tag=Cyc_Core_Impossible;_tmp198.f1=({const char*_tmp199="bad assign place";_tag_dyneither(_tmp199,sizeof(char),17);});_tmp198;});_tmp197;}));}}_LLED:;};}
# 842
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 853
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1A2=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp1A3=place;void*_tmp1A7;struct Cyc_List_List*_tmp1A6;_LLF8: _tmp1A7=_tmp1A3->root;_tmp1A6=_tmp1A3->fields;_LLF9:;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp1A4;_tmp1A4.fenv=fenv;_tmp1A4.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1A5=_region_malloc(_tmp1A2,sizeof(*_tmp1A5));_tmp1A5->rgn=_tmp1A2;_tmp1A5->places=0;_tmp1A5;});_tmp1A4.d=d;_tmp1A4.loc=loc;_tmp1A4.root_place=place;_tmp1A4.all_changed=all_changed;_tmp1A4;});
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1A6,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1A7),r);
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1A7,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 869
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 879
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 881
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 891 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1A8=(env->fenv)->r;
# 895
{struct _tuple0 _tmp1A9=({struct _tuple0 _tmp1E4;_tmp1E4.f1=r1;_tmp1E4.f2=r2;_tmp1E4;});struct _tuple0 _tmp1AA=_tmp1A9;int _tmp1E3;int _tmp1E2;struct _dyneither_ptr _tmp1E1;int _tmp1E0;int _tmp1DF;struct _dyneither_ptr _tmp1DE;struct Cyc_CfFlowInfo_Place*_tmp1DD;struct Cyc_CfFlowInfo_Place*_tmp1DC;struct Cyc_CfFlowInfo_Place*_tmp1DB;struct Cyc_CfFlowInfo_Place*_tmp1DA;struct Cyc_CfFlowInfo_Place*_tmp1D9;struct Cyc_CfFlowInfo_Place*_tmp1D8;struct Cyc_CfFlowInfo_Place*_tmp1D7;struct Cyc_CfFlowInfo_Place*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D5;int _tmp1D4;void*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D2;int _tmp1D1;void*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1CF;int _tmp1CE;void*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;int _tmp1CB;void*_tmp1CA;struct Cyc_Absyn_Vardecl*_tmp1C9;void*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1C7;void*_tmp1C6;struct Cyc_Absyn_Vardecl*_tmp1C5;void*_tmp1C4;struct Cyc_Absyn_Vardecl*_tmp1C3;void*_tmp1C2;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 8){_LLFB: _tmp1C5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1C4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1C3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1C2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_LLFC:
# 898
 if(_tmp1C5 == _tmp1C3)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1AB=_region_malloc(_tmp1A8,sizeof(*_tmp1AB));_tmp1AB[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp1AC;_tmp1AC.tag=8;_tmp1AC.f1=_tmp1C5;_tmp1AC.f2=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C4,_tmp1C2);_tmp1AC;});_tmp1AB;});else{
# 901
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C4,_tmp1C2);}}else{_LLFD: _tmp1C7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1C6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_LLFE:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C6,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 8){_LLFF: _tmp1C9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1C8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_LL100:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1C8);}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->tag == 7){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 7){_LL101: _tmp1CF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1CE=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1CD=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f3;_tmp1CC=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1CB=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_tmp1CA=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f3;_LL102:
# 906
 if(_tmp1CF == _tmp1CC)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1AD=_region_malloc(_tmp1A8,sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1AE;_tmp1AE.tag=7;_tmp1AE.f1=_tmp1CF;_tmp1AE.f2=_tmp1CE > _tmp1CB?_tmp1CE: _tmp1CB;_tmp1AE.f3=
Cyc_CfFlowInfo_join_absRval(env,a,_tmp1CD,_tmp1CA);_tmp1AE;});_tmp1AD;});
{void*_tmp1AF=_tmp1CD;struct Cyc_CfFlowInfo_Place*_tmp1B0;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AF)->tag == 5){_LL11E: _tmp1B0=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AF)->f1;_LL11F:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B0);goto _LL11D;}else{_LL120: _LL121:
 goto _LL11D;}_LL11D:;}
# 913
{void*_tmp1B1=_tmp1CA;struct Cyc_CfFlowInfo_Place*_tmp1B2;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1B1)->tag == 5){_LL123: _tmp1B2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1B1)->f1;_LL124:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B2);goto _LL122;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}
# 917
goto _LLFA;}else{_LL103: _tmp1D2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1D1=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1D0=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f3;_LL104:
# 919
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1B3=_region_malloc(_tmp1A8,sizeof(*_tmp1B3));_tmp1B3[0]=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1B4;_tmp1B4.tag=7;_tmp1B4.f1=_tmp1D2;_tmp1B4.f2=_tmp1D1;_tmp1B4.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1D0,r2);_tmp1B4;});_tmp1B3;});}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 7){_LL105: _tmp1D5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1D4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_tmp1D3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f3;_LL106:
# 921
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1B5=_region_malloc(_tmp1A8,sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1B6;_tmp1B6.tag=7;_tmp1B6.f1=_tmp1D5;_tmp1B6.f2=_tmp1D4;_tmp1B6.f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1D3);_tmp1B6;});_tmp1B5;});}else{switch(*((int*)_tmp1AA.f1)){case 5: switch(*((int*)_tmp1AA.f2)){case 5: _LL107: _tmp1D7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1D6=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL108:
# 924
 if(Cyc_CfFlowInfo_place_cmp(_tmp1D7,_tmp1D6)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1D7);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1D6);
goto _LLFA;case 2: _LL109: _tmp1D8=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_LL10A:
# 929
 _tmp1DB=_tmp1D8;goto _LL10C;case 1: _LL10D: _tmp1D9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_LL10E:
# 935
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1D9);{
enum Cyc_CfFlowInfo_InitLevel _tmp1B7=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _tmp1B8=_tmp1B7;if(_tmp1B8 == Cyc_CfFlowInfo_AllIL){_LL128: _LL129:
 return(env->fenv)->notzeroall;}else{_LL12A: _LL12B:
 return(env->fenv)->notzerothis;}_LL127:;};default: _LL111: _tmp1DA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_LL112:
# 947
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1DA);goto _LLFA;}case 2: switch(*((int*)_tmp1AA.f2)){case 5: _LL10B: _tmp1DB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL10C:
# 931
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1DB);
return(env->fenv)->notzerothis;case 1: _LL117: _LL118:
# 952
 return(env->fenv)->notzerothis;default: goto _LL11B;}case 1: switch(*((int*)_tmp1AA.f2)){case 5: _LL10F: _tmp1DC=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL110:
# 941
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1DC);{
enum Cyc_CfFlowInfo_InitLevel _tmp1B9=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _tmp1BA=_tmp1B9;if(_tmp1BA == Cyc_CfFlowInfo_AllIL){_LL12D: _LL12E:
 return(env->fenv)->notzeroall;}else{_LL12F: _LL130:
 return(env->fenv)->notzerothis;}_LL12C:;};case 2: _LL115: _LL116:
# 951
 goto _LL118;default: goto _LL11B;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 5){_LL113: _tmp1DD=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL114:
# 948
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1DD);goto _LLFA;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 6){_LL119: _tmp1E3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1).is_union;_tmp1E2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1).fieldnum;_tmp1E1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1E0=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1).is_union;_tmp1DF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1).fieldnum;_tmp1DE=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_LL11A: {
# 955
struct _dyneither_ptr new_d=({unsigned int _tmp1BE=
_get_dyneither_size(_tmp1E1,sizeof(void*));void**_tmp1BF=(void**)_region_malloc(_tmp1A8,_check_times(sizeof(void*),_tmp1BE));struct _dyneither_ptr _tmp1C1=_tag_dyneither(_tmp1BF,sizeof(void*),_tmp1BE);{unsigned int _tmp1C0=_tmp1BE;unsigned int i;for(i=0;i < _tmp1C0;i ++){_tmp1BF[i]=(void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1E1.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1DE,sizeof(void*),(int)i)));}}_tmp1C1;});
# 960
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1E1,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1E1.curr)[i]){
change=1;break;}}}
# 965
if(!change){
if(!_tmp1E3)return r1;
new_d=_tmp1E1;}else{
# 970
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1E1,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1DE,sizeof(void*),i))){
change=1;break;}}}
# 975
if(!change){
if(!_tmp1E3)return r2;
new_d=_tmp1DE;}}
# 980
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1BB=_region_malloc(_tmp1A8,sizeof(*_tmp1BB));_tmp1BB[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp1BC;_tmp1BC.tag=6;_tmp1BC.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp1BD;_tmp1BD.is_union=_tmp1E3;_tmp1BD.fieldnum=_tmp1E2 == _tmp1DF?_tmp1E2: - 1;_tmp1BD;});_tmp1BC.f2=new_d;_tmp1BC;});_tmp1BB;});}}else{goto _LL11B;}}else{_LL11B: _LL11C:
# 982
 goto _LLFA;}}}}}}}_LLFA:;}{
# 984
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp1E5=({struct _tuple0 _tmp1ED;_tmp1ED.f1=r1;_tmp1ED.f2=r2;_tmp1ED;});struct _tuple0 _tmp1E6=_tmp1E5;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1E6.f1)->tag == 4){_LL132: _LL133:
 goto _LL135;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1E6.f2)->tag == 4){_LL134: _LL135: {
# 989
struct _tuple21 _tmp1E7=({struct _tuple21 _tmp1E9;_tmp1E9.f1=il1;_tmp1E9.f2=il2;_tmp1E9;});struct _tuple21 _tmp1E8=_tmp1E7;if(_tmp1E8.f2 == Cyc_CfFlowInfo_NoneIL){_LL139: _LL13A:
 goto _LL13C;}else{if(_tmp1E8.f1 == Cyc_CfFlowInfo_NoneIL){_LL13B: _LL13C: return(env->fenv)->esc_none;}else{if(_tmp1E8.f2 == Cyc_CfFlowInfo_ThisIL){_LL13D: _LL13E:
 goto _LL140;}else{if(_tmp1E8.f1 == Cyc_CfFlowInfo_ThisIL){_LL13F: _LL140: return(env->fenv)->esc_this;}else{_LL141: _LL142:
 return(env->fenv)->esc_all;}}}}_LL138:;}}else{_LL136: _LL137: {
# 995
struct _tuple21 _tmp1EA=({struct _tuple21 _tmp1EC;_tmp1EC.f1=il1;_tmp1EC.f2=il2;_tmp1EC;});struct _tuple21 _tmp1EB=_tmp1EA;if(_tmp1EB.f2 == Cyc_CfFlowInfo_NoneIL){_LL144: _LL145:
 goto _LL147;}else{if(_tmp1EB.f1 == Cyc_CfFlowInfo_NoneIL){_LL146: _LL147: return(env->fenv)->unknown_none;}else{if(_tmp1EB.f2 == Cyc_CfFlowInfo_ThisIL){_LL148: _LL149:
 goto _LL14B;}else{if(_tmp1EB.f1 == Cyc_CfFlowInfo_ThisIL){_LL14A: _LL14B: return(env->fenv)->unknown_this;}else{_LL14C: _LL14D:
 return(env->fenv)->unknown_all;}}}}_LL143:;}}}_LL131:;};};}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 1003
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1006
struct _tuple22 _tmp1EE=({struct _tuple22 _tmp1FA;_tmp1FA.f1=f1;_tmp1FA.f2=f2;_tmp1FA;});struct _tuple22 _tmp1EF=_tmp1EE;struct Cyc_Dict_Dict _tmp1F9;struct Cyc_List_List*_tmp1F8;struct Cyc_Dict_Dict _tmp1F7;struct Cyc_List_List*_tmp1F6;if(((_tmp1EF.f1).BottomFL).tag == 1){_LL14F: _LL150:
 return f2;}else{if(((_tmp1EF.f2).BottomFL).tag == 1){_LL151: _LL152:
 return f1;}else{_LL153: _tmp1F9=(((_tmp1EF.f1).ReachableFL).val).f1;_tmp1F8=(((_tmp1EF.f1).ReachableFL).val).f2;_tmp1F7=(((_tmp1EF.f2).ReachableFL).val).f1;_tmp1F6=(((_tmp1EF.f2).ReachableFL).val).f2;_LL154:
# 1012
 if(_tmp1F9.t == _tmp1F7.t  && _tmp1F8 == _tmp1F6)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp1F0=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp1F1=({struct Cyc_CfFlowInfo_JoinEnv _tmp1F4;_tmp1F4.fenv=fenv;_tmp1F4.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1F5=_region_malloc(_tmp1F0,sizeof(*_tmp1F5));_tmp1F5->rgn=_tmp1F0;_tmp1F5->places=0;_tmp1F5;});_tmp1F4.d1=_tmp1F9;_tmp1F4.d2=_tmp1F7;_tmp1F4;});
struct Cyc_Dict_Dict _tmp1F2=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp1F1,_tmp1F9,_tmp1F7);
struct Cyc_List_List*_tmp1F3=Cyc_Relations_join_relns(fenv->r,_tmp1F8,_tmp1F6);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp1F1.pile,all_changed,_tmp1F2),_tmp1F3);};}}_LL14E:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1024
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2){
# 1031
struct _tuple13 _tmp1FB=pr1;union Cyc_CfFlowInfo_FlowInfo _tmp214;void*_tmp213;_LL156: _tmp214=_tmp1FB.f1;_tmp213=_tmp1FB.f2;_LL157:;{
struct _tuple13 _tmp1FC=pr2;union Cyc_CfFlowInfo_FlowInfo _tmp212;void*_tmp211;_LL159: _tmp212=_tmp1FC.f1;_tmp211=_tmp1FC.f2;_LL15A:;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp214,_tmp212);
struct _tuple23 _tmp1FD=({struct _tuple23 _tmp210;_tmp210.f1=_tmp214;_tmp210.f2=_tmp212;_tmp210.f3=outflow;_tmp210;});struct _tuple23 _tmp1FE=_tmp1FD;struct Cyc_Dict_Dict _tmp20F;struct Cyc_Dict_Dict _tmp20E;struct Cyc_Dict_Dict _tmp20D;struct Cyc_List_List*_tmp20C;if(((_tmp1FE.f1).BottomFL).tag == 1){_LL15C: _LL15D:
 return({struct _tuple13 _tmp1FF;_tmp1FF.f1=outflow;_tmp1FF.f2=_tmp211;_tmp1FF;});}else{if(((_tmp1FE.f2).BottomFL).tag == 1){_LL15E: _LL15F:
 return({struct _tuple13 _tmp200;_tmp200.f1=outflow;_tmp200.f2=_tmp213;_tmp200;});}else{if(((_tmp1FE.f3).ReachableFL).tag == 2){_LL160: _tmp20F=(((_tmp1FE.f1).ReachableFL).val).f1;_tmp20E=(((_tmp1FE.f2).ReachableFL).val).f1;_tmp20D=(((_tmp1FE.f3).ReachableFL).val).f1;_tmp20C=(((_tmp1FE.f3).ReachableFL).val).f2;_LL161:
# 1039
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp213,_tmp211))return({struct _tuple13 _tmp201;_tmp201.f1=outflow;_tmp201.f2=_tmp211;_tmp201;});
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp211,_tmp213))return({struct _tuple13 _tmp202;_tmp202.f1=outflow;_tmp202.f2=_tmp213;_tmp202;});{
struct _RegionHandle*_tmp203=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp204=({struct Cyc_CfFlowInfo_JoinEnv _tmp207;_tmp207.fenv=fenv;_tmp207.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp208=_region_malloc(_tmp203,sizeof(*_tmp208));_tmp208->rgn=_tmp203;_tmp208->places=0;_tmp208;});_tmp207.d1=_tmp20F;_tmp207.d2=_tmp20E;_tmp207;});
void*_tmp205=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp204,0,_tmp213,_tmp211);
return({struct _tuple13 _tmp206;_tmp206.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp204.pile,all_changed,_tmp20D),_tmp20C);_tmp206.f2=_tmp205;_tmp206;});};}else{_LL162: _LL163:
# 1047
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209[0]=({struct Cyc_Core_Impossible_exn_struct _tmp20A;_tmp20A.tag=Cyc_Core_Impossible;_tmp20A.f1=({const char*_tmp20B="join_flow_and_rval: BottomFL outflow";_tag_dyneither(_tmp20B,sizeof(char),37);});_tmp20A;});_tmp209;}));}}}_LL15B:;};};}
# 1051
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1060
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1065
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1073
struct _tuple0 _tmp215=({struct _tuple0 _tmp22E;_tmp22E.f1=r1;_tmp22E.f2=r2;_tmp22E;});struct _tuple0 _tmp216=_tmp215;int _tmp22D;int _tmp22C;struct _dyneither_ptr _tmp22B;int _tmp22A;int _tmp229;struct _dyneither_ptr _tmp228;struct Cyc_Absyn_Vardecl*_tmp227;void*_tmp226;struct Cyc_Absyn_Vardecl*_tmp225;void*_tmp224;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->tag == 7){_LL165: _LL166:
# 1075
 goto _LL168;}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 7){_LL167: _LL168:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 8){_LL169: _tmp227=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f1;_tmp226=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f2;_tmp225=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f1;_tmp224=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f2;_LL16A:
# 1078
 if(_tmp227 == _tmp225){
void*_tmp217=Cyc_CfFlowInfo_after_absRval(env,_tmp226,_tmp224);
if(_tmp217 == _tmp226)return r1;
if(_tmp217 == _tmp224)return r2;
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp218=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp218));_tmp218[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp219;_tmp219.tag=8;_tmp219.f1=_tmp227;_tmp219.f2=_tmp217;_tmp219;});_tmp218;});}else{
# 1084
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}}else{_LL16D: _LL16E:
# 1087
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 8){_LL16B: _LL16C:
# 1085
 goto _LL16E;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 6){_LL16F: _tmp22D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f1).is_union;_tmp22C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f1).fieldnum;_tmp22B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f2;_tmp22A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f1).is_union;_tmp229=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f1).fieldnum;_tmp228=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f2;_LL170: {
# 1090
struct _dyneither_ptr new_d=({unsigned int _tmp21D=
# 1092
_get_dyneither_size(_tmp22B,sizeof(void*));void**_tmp21E=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp21D));struct _dyneither_ptr _tmp220=_tag_dyneither(_tmp21E,sizeof(void*),_tmp21D);{unsigned int _tmp21F=_tmp21D;unsigned int i;for(i=0;i < _tmp21F;i ++){_tmp21E[i]=(void*)Cyc_CfFlowInfo_after_absRval_d(env,(int)i,((void**)_tmp22B.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp228,sizeof(void*),(int)i)));}}_tmp220;});
# 1097
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp22B,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp22B.curr)[i]){
change=1;break;}}}
# 1102
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp22B,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp228,sizeof(void*),i))){
change=1;break;}}}
# 1109
if(!change)
return r2;
# 1113
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp21A=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp21A));_tmp21A[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp21B;_tmp21B.tag=6;_tmp21B.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp21C;_tmp21C.is_union=_tmp22D;_tmp21C.fieldnum=_tmp22C == _tmp229?_tmp22C: - 1;_tmp21C;});_tmp21B.f2=new_d;_tmp21B;});_tmp21A;});}}else{goto _LL171;}}else{_LL171: _LL172:
# 1121
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221[0]=({struct Cyc_Core_Impossible_exn_struct _tmp222;_tmp222.tag=Cyc_Core_Impossible;_tmp222.f1=({const char*_tmp223="after_absRval -- non-aggregates!";_tag_dyneither(_tmp223,sizeof(char),33);});_tmp222;});_tmp221;}));}}}}}_LL164:;};}
# 1124
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1127
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp22F=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp230=env->changed;
*env->last_field_cell=({struct Cyc_List_List*_tmp231=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp231));_tmp231->hd=(void*)key;_tmp231->tl=0;_tmp231;});
# 1133
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp232=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp22F;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp230;
return _tmp232;};};}
# 1140
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1142
if(r1 == r2)return r1;
*env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp233;_tmp233.root=root;_tmp233.fields=0;_tmp233;});
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1149
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1155
struct _tuple22 _tmp234=({struct _tuple22 _tmp241;_tmp241.f1=f1;_tmp241.f2=f2;_tmp241;});struct _tuple22 _tmp235=_tmp234;struct Cyc_Dict_Dict _tmp240;struct Cyc_List_List*_tmp23F;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*_tmp23D;if(((_tmp235.f1).BottomFL).tag == 1){_LL174: _LL175:
 goto _LL177;}else{if(((_tmp235.f2).BottomFL).tag == 1){_LL176: _LL177:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL178: _tmp240=(((_tmp235.f1).ReachableFL).val).f1;_tmp23F=(((_tmp235.f1).ReachableFL).val).f2;_tmp23E=(((_tmp235.f2).ReachableFL).val).f1;_tmp23D=(((_tmp235.f2).ReachableFL).val).f2;_LL179:
# 1159
 if(_tmp240.t == _tmp23E.t  && _tmp23F == _tmp23D)return f1;{
# 1162
struct _RegionHandle*_tmp236=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp237=fenv->dummy_place;
struct Cyc_CfFlowInfo_AfterEnv _tmp238=({struct Cyc_CfFlowInfo_AfterEnv _tmp23B;_tmp23B.joinenv=({(_tmp23B.joinenv).fenv=fenv;(_tmp23B.joinenv).pile=({struct Cyc_CfFlowInfo_EscPile*_tmp23C=_region_malloc(_tmp236,sizeof(*_tmp23C));_tmp23C->rgn=_tmp236;_tmp23C->places=0;_tmp23C;});(_tmp23B.joinenv).d1=_tmp240;(_tmp23B.joinenv).d2=_tmp23E;_tmp23B.joinenv;});_tmp23B.chg1=chg1;_tmp23B.chg2=chg2;_tmp23B.curr_place=_tmp237;_tmp23B.last_field_cell=& _tmp237->fields;_tmp23B.changed=Cyc_CfFlowInfo_Neither;_tmp23B;});
# 1168
struct Cyc_Dict_Dict _tmp239=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp238,_tmp240,_tmp23E);
struct Cyc_List_List*_tmp23A=Cyc_Relations_join_relns(fenv->r,_tmp23F,_tmp23D);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp238.joinenv).pile,all_changed,_tmp239),_tmp23A);};}}_LL173:;}
# 1177
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1181
{struct _tuple0 _tmp242=({struct _tuple0 _tmp250;_tmp250.f1=r1;_tmp250.f2=r2;_tmp250;});struct _tuple0 _tmp243=_tmp242;int _tmp24F;int _tmp24E;struct _dyneither_ptr _tmp24D;int _tmp24C;int _tmp24B;struct _dyneither_ptr _tmp24A;struct Cyc_CfFlowInfo_Place*_tmp249;struct Cyc_CfFlowInfo_Place*_tmp248;struct Cyc_Absyn_Vardecl*_tmp247;void*_tmp246;struct Cyc_Absyn_Vardecl*_tmp245;void*_tmp244;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 8){_LL17B: _tmp247=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1;_tmp246=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f2;_tmp245=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1;_tmp244=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f2;_LL17C:
# 1183
 return _tmp247 == _tmp245  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp246,_tmp244);}else{_LL17D: _LL17E:
 goto _LL180;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 8){_LL17F: _LL180:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->tag == 5){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 5){_LL181: _tmp249=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1;_tmp248=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1;_LL182:
 return Cyc_CfFlowInfo_place_cmp(_tmp249,_tmp248)== 0;}else{_LL183: _LL184:
 goto _LL186;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 5){_LL185: _LL186:
 return 0;}else{switch(*((int*)_tmp243.f1)){case 6: switch(*((int*)_tmp243.f2)){case 6: _LL187: _tmp24F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1).is_union;_tmp24E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1).fieldnum;_tmp24D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f2;_tmp24C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1).is_union;_tmp24B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1).fieldnum;_tmp24A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f2;_LL188:
# 1191
 if(_tmp24F  && _tmp24E != _tmp24B)return 0;
if((void**)_tmp24D.curr == (void**)_tmp24A.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp24D,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp24D.curr)[i],*((void**)_check_dyneither_subscript(_tmp24A,sizeof(void*),i))))return 0;}}
return 1;case 2: goto _LL18B;case 0: goto _LL18D;case 1: goto _LL18F;default: goto _LL195;}case 1: switch(*((int*)_tmp243.f2)){case 2: _LL189: _LL18A:
 return 1;case 0: goto _LL18D;case 1: goto _LL18F;default: goto _LL195;}default: switch(*((int*)_tmp243.f2)){case 2: _LL18B: _LL18C:
 return 0;case 0: _LL18D: _LL18E:
 goto _LL190;case 1: _LL18F: _LL190:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->tag == 4){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 4){_LL191: _LL192:
 goto _LL17A;}else{_LL193: _LL194:
 return 0;}}else{_LL195: _LL196:
 goto _LL17A;}}}}}}}_LL17A:;}{
# 1204
struct _tuple21 _tmp251=({struct _tuple21 _tmp253;_tmp253.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp253.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp253;});struct _tuple21 _tmp252=_tmp251;if(_tmp252.f1 == Cyc_CfFlowInfo_AllIL)switch(_tmp252.f2){case Cyc_CfFlowInfo_AllIL: _LL198: _LL199:
 return 1;case Cyc_CfFlowInfo_NoneIL: goto _LL19A;default: goto _LL19E;}else{if(_tmp252.f2 == Cyc_CfFlowInfo_NoneIL){_LL19A: _LL19B:
 return 1;}else{if(_tmp252.f1 == Cyc_CfFlowInfo_NoneIL){_LL19C: _LL19D:
 return 0;}else{if(_tmp252.f2 == Cyc_CfFlowInfo_ThisIL){_LL19E: _LL19F:
 return 1;}else{_LL1A0: _LL1A1:
 return 0;}}}}_LL197:;};}
# 1219
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1221
struct _tuple22 _tmp254=({struct _tuple22 _tmp25A;_tmp25A.f1=f1;_tmp25A.f2=f2;_tmp25A;});struct _tuple22 _tmp255=_tmp254;struct Cyc_Dict_Dict _tmp259;struct Cyc_List_List*_tmp258;struct Cyc_Dict_Dict _tmp257;struct Cyc_List_List*_tmp256;if(((_tmp255.f1).BottomFL).tag == 1){_LL1A3: _LL1A4:
 return 1;}else{if(((_tmp255.f2).BottomFL).tag == 1){_LL1A5: _LL1A6:
 return 0;}else{_LL1A7: _tmp259=(((_tmp255.f1).ReachableFL).val).f1;_tmp258=(((_tmp255.f1).ReachableFL).val).f2;_tmp257=(((_tmp255.f2).ReachableFL).val).f1;_tmp256=(((_tmp255.f2).ReachableFL).val).f2;_LL1A8:
# 1225
 if(_tmp259.t == _tmp257.t  && _tmp258 == _tmp256)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp259,_tmp257) && 
Cyc_Relations_relns_approx(_tmp258,_tmp256);}}_LL1A2:;}
# 1231
struct _tuple12 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv){
# 1233
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp25B=rv;struct Cyc_Absyn_Vardecl*_tmp25E;void*_tmp25D;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp25B)->tag == 8){_LL1AA: _tmp25E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp25B)->f1;_tmp25D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp25B)->f2;_LL1AB:
# 1238
 names=({struct Cyc_List_List*_tmp25C=_region_malloc(rgn,sizeof(*_tmp25C));_tmp25C->hd=_tmp25E;_tmp25C->tl=names;_tmp25C;});rv=_tmp25D;goto _LL1A9;}else{_LL1AC: _LL1AD:
# 1240
 done=1;goto _LL1A9;}_LL1A9:;}
# 1243
return({struct _tuple12 _tmp25F;_tmp25F.f1=rv;_tmp25F.f2=names;_tmp25F;});}
# 1246
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp260=il;switch(_tmp260){case Cyc_CfFlowInfo_NoneIL: _LL1AF: _LL1B0:
({void*_tmp261=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp262="uninitialized";_tag_dyneither(_tmp262,sizeof(char),14);}),_tag_dyneither(_tmp261,sizeof(void*),0));});goto _LL1AE;case Cyc_CfFlowInfo_ThisIL: _LL1B1: _LL1B2:
({void*_tmp263=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp264="this-initialized";_tag_dyneither(_tmp264,sizeof(char),17);}),_tag_dyneither(_tmp263,sizeof(void*),0));});goto _LL1AE;default: _LL1B3: _LL1B4:
({void*_tmp265=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp266="all-initialized";_tag_dyneither(_tmp266,sizeof(char),16);}),_tag_dyneither(_tmp265,sizeof(void*),0));});goto _LL1AE;}_LL1AE:;}
# 1254
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp267=root;struct Cyc_Absyn_Exp*_tmp273;void*_tmp272;struct Cyc_Absyn_Vardecl*_tmp271;switch(*((int*)_tmp267)){case 0: _LL1B6: _tmp271=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp267)->f1;_LL1B7:
# 1257
({struct Cyc_String_pa_PrintArg_struct _tmp26A;_tmp26A.tag=0;_tmp26A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp271->name));({void*_tmp268[1]={& _tmp26A};Cyc_fprintf(Cyc_stderr,({const char*_tmp269="Root(%s)";_tag_dyneither(_tmp269,sizeof(char),9);}),_tag_dyneither(_tmp268,sizeof(void*),1));});});goto _LL1B5;case 1: _LL1B8: _tmp273=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp267)->f1;_tmp272=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp267)->f2;_LL1B9:
# 1259
({struct Cyc_String_pa_PrintArg_struct _tmp26E;_tmp26E.tag=0;_tmp26E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp272));({struct Cyc_String_pa_PrintArg_struct _tmp26D;_tmp26D.tag=0;_tmp26D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp273));({void*_tmp26B[2]={& _tmp26D,& _tmp26E};Cyc_fprintf(Cyc_stderr,({const char*_tmp26C="MallocPt(%s,%s)";_tag_dyneither(_tmp26C,sizeof(char),16);}),_tag_dyneither(_tmp26B,sizeof(void*),2));});});});goto _LL1B5;default: _LL1BA: _LL1BB:
# 1262
({void*_tmp26F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp270="InitParam(_,_)";_tag_dyneither(_tmp270,sizeof(char),15);}),_tag_dyneither(_tmp26F,sizeof(void*),0));});goto _LL1B5;}_LL1B5:;}
# 1266
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp274=p->fields;
while(_tmp274 != 0){
({struct Cyc_Int_pa_PrintArg_struct _tmp277;_tmp277.tag=1;_tmp277.f1=(unsigned long)((int)_tmp274->hd);({void*_tmp275[1]={& _tmp277};Cyc_fprintf(Cyc_stderr,({const char*_tmp276=".%d";_tag_dyneither(_tmp276,sizeof(char),4);}),_tag_dyneither(_tmp275,sizeof(void*),1));});});
_tmp274=_tmp274->tl;}
# 1273
({void*_tmp278=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp279=" ";_tag_dyneither(_tmp279,sizeof(char),2);}),_tag_dyneither(_tmp278,sizeof(void*),0));});};}
# 1276
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp27A=1;
while(x != 0){
if(!_tmp27A){({void*_tmp27B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp27C=", ";_tag_dyneither(_tmp27C,sizeof(char),3);}),_tag_dyneither(_tmp27B,sizeof(void*),0));});_tmp27A=0;}
pr(x->hd);
x=x->tl;}
# 1283
({void*_tmp27D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp27E="\n";_tag_dyneither(_tmp27E,sizeof(char),2);}),_tag_dyneither(_tmp27D,sizeof(void*),0));});}
# 1286
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp27F=_new_region("r");struct _RegionHandle*r=& _tmp27F;_push_region(r);
{struct _handler_cons _tmp280;_push_handler(& _tmp280);{int _tmp282=0;if(setjmp(_tmp280.handler))_tmp282=1;if(!_tmp282){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple14 _tmp283=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
({void*_tmp284=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp285="{ ";_tag_dyneither(_tmp285,sizeof(char),3);}),_tag_dyneither(_tmp284,sizeof(void*),0));});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp283)){
void*_tmp286=_tmp283.f1;
if(!first){
({void*_tmp287=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp288=", ";_tag_dyneither(_tmp288,sizeof(char),3);}),_tag_dyneither(_tmp287,sizeof(void*),0));});
first=0;}
# 1299
pr(_tmp286);}
# 1301
({void*_tmp289=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp28A="}\n";_tag_dyneither(_tmp28A,sizeof(char),3);}),_tag_dyneither(_tmp289,sizeof(void*),0));});}
# 1289
;_pop_handler();}else{void*_tmp281=(void*)_exn_thrown;void*_tmp28B=_tmp281;void*_tmp28E;_LL1BD: _tmp28E=_tmp28B;_LL1BE:
# 1304
({void*_tmp28C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp28D="{ }\n";_tag_dyneither(_tmp28D,sizeof(char),5);}),_tag_dyneither(_tmp28C,sizeof(void*),0));});
_npop_handler(0);return;_LL1BC:;}};}
# 1288
;_pop_region(r);}
# 1310
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp28F=rval;struct Cyc_Absyn_Vardecl*_tmp2C2;void*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;int _tmp2BF;void*_tmp2BE;int _tmp2BD;int _tmp2BC;struct _dyneither_ptr _tmp2BB;struct Cyc_CfFlowInfo_Place*_tmp2BA;enum Cyc_CfFlowInfo_InitLevel _tmp2B9;enum Cyc_CfFlowInfo_InitLevel _tmp2B8;switch(*((int*)_tmp28F)){case 0: _LL1C2: _LL1C3:
({void*_tmp290=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp291="Zero";_tag_dyneither(_tmp291,sizeof(char),5);}),_tag_dyneither(_tmp290,sizeof(void*),0));});goto _LL1C1;case 1: _LL1C4: _LL1C5:
({void*_tmp292=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp293="NotZeroAll";_tag_dyneither(_tmp293,sizeof(char),11);}),_tag_dyneither(_tmp292,sizeof(void*),0));});goto _LL1C1;case 2: _LL1C6: _LL1C7:
({void*_tmp294=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp295="NotZeroThis";_tag_dyneither(_tmp295,sizeof(char),12);}),_tag_dyneither(_tmp294,sizeof(void*),0));});goto _LL1C1;case 3: _LL1C8: _tmp2B8=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_LL1C9:
({void*_tmp296=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp297="Unknown(";_tag_dyneither(_tmp297,sizeof(char),9);}),_tag_dyneither(_tmp296,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(_tmp2B8);
({void*_tmp298=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp299=")";_tag_dyneither(_tmp299,sizeof(char),2);}),_tag_dyneither(_tmp298,sizeof(void*),0));});goto _LL1C1;case 4: _LL1CA: _tmp2B9=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_LL1CB:
({void*_tmp29A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29B="Esc(";_tag_dyneither(_tmp29B,sizeof(char),5);}),_tag_dyneither(_tmp29A,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(_tmp2B9);
({void*_tmp29C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29D=")";_tag_dyneither(_tmp29D,sizeof(char),2);}),_tag_dyneither(_tmp29C,sizeof(void*),0));});goto _LL1C1;case 5: _LL1CC: _tmp2BA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_LL1CD:
({void*_tmp29E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29F="AddrOf(";_tag_dyneither(_tmp29F,sizeof(char),8);}),_tag_dyneither(_tmp29E,sizeof(void*),0));});Cyc_CfFlowInfo_print_place(_tmp2BA);
({void*_tmp2A0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A1=")";_tag_dyneither(_tmp2A1,sizeof(char),2);}),_tag_dyneither(_tmp2A0,sizeof(void*),0));});goto _LL1C1;case 6: _LL1CE: _tmp2BD=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1).is_union;_tmp2BC=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1).fieldnum;_tmp2BB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f2;_LL1CF:
# 1322
 if(_tmp2BD){
({void*_tmp2A2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A3="AggrUnion{";_tag_dyneither(_tmp2A3,sizeof(char),11);}),_tag_dyneither(_tmp2A2,sizeof(void*),0));});
if(_tmp2BC != - 1)
({struct Cyc_Int_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=1;_tmp2A6.f1=(unsigned long)_tmp2BC;({void*_tmp2A4[1]={& _tmp2A6};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A5="tag == %d;";_tag_dyneither(_tmp2A5,sizeof(char),11);}),_tag_dyneither(_tmp2A4,sizeof(void*),1));});});}else{
# 1327
({void*_tmp2A7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A8="AggrStruct{";_tag_dyneither(_tmp2A8,sizeof(char),12);}),_tag_dyneither(_tmp2A7,sizeof(void*),0));});}
{int i=0;for(0;i < _get_dyneither_size(_tmp2BB,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp2BB.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp2BB,sizeof(void*)))({void*_tmp2A9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2AA=",";_tag_dyneither(_tmp2AA,sizeof(char),2);}),_tag_dyneither(_tmp2A9,sizeof(void*),0));});}}
# 1332
({void*_tmp2AB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2AC="}";_tag_dyneither(_tmp2AC,sizeof(char),2);}),_tag_dyneither(_tmp2AB,sizeof(void*),0));});
goto _LL1C1;case 7: _LL1D0: _tmp2C0=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_tmp2BF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f2;_tmp2BE=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f3;_LL1D1:
# 1335
({struct Cyc_Int_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=1;_tmp2B0.f1=(unsigned long)_tmp2BF;({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2C0));({void*_tmp2AD[2]={& _tmp2AF,& _tmp2B0};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AE="[Consumed(%s,%d,";_tag_dyneither(_tmp2AE,sizeof(char),17);}),_tag_dyneither(_tmp2AD,sizeof(void*),2));});});});
Cyc_CfFlowInfo_print_absrval(_tmp2BE);({void*_tmp2B1=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2B2=")]";_tag_dyneither(_tmp2B2,sizeof(char),3);}),_tag_dyneither(_tmp2B1,sizeof(void*),0));});
goto _LL1C1;default: _LL1D2: _tmp2C2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_tmp2C1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f2;_LL1D3:
# 1339
({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C2->name));({void*_tmp2B3[1]={& _tmp2B5};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B4="[NamedLocation(%s,";_tag_dyneither(_tmp2B4,sizeof(char),19);}),_tag_dyneither(_tmp2B3,sizeof(void*),1));});});
Cyc_CfFlowInfo_print_absrval(_tmp2C1);({void*_tmp2B6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2B7=")]";_tag_dyneither(_tmp2B7,sizeof(char),3);}),_tag_dyneither(_tmp2B6,sizeof(void*),0));});
goto _LL1C1;}_LL1C1:;}
# 1345
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp2C3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C4="    ";_tag_dyneither(_tmp2C4,sizeof(char),5);}),_tag_dyneither(_tmp2C3,sizeof(void*),0));});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp2C5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C6=" --> ";_tag_dyneither(_tmp2C6,sizeof(char),6);}),_tag_dyneither(_tmp2C5,sizeof(void*),0));});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp2C7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C8="\n";_tag_dyneither(_tmp2C8,sizeof(char),2);}),_tag_dyneither(_tmp2C7,sizeof(void*),0));});}
# 1353
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1357
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp2C9=f;struct Cyc_Dict_Dict _tmp2D3;struct Cyc_List_List*_tmp2D2;if((_tmp2C9.BottomFL).tag == 1){_LL1D5: _LL1D6:
({void*_tmp2CA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2CB="  BottomFL\n";_tag_dyneither(_tmp2CB,sizeof(char),12);}),_tag_dyneither(_tmp2CA,sizeof(void*),0));});goto _LL1D4;}else{_LL1D7: _tmp2D3=((_tmp2C9.ReachableFL).val).f1;_tmp2D2=((_tmp2C9.ReachableFL).val).f2;_LL1D8:
# 1361
({void*_tmp2CC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2CD="  ReachableFL:\n";_tag_dyneither(_tmp2CD,sizeof(char),16);}),_tag_dyneither(_tmp2CC,sizeof(void*),0));});
Cyc_CfFlowInfo_print_flowdict(_tmp2D3);
({void*_tmp2CE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2CF="\n  Relations: ";_tag_dyneither(_tmp2CF,sizeof(char),15);}),_tag_dyneither(_tmp2CE,sizeof(void*),0));});
Cyc_Relations_print_relns(Cyc_stderr,_tmp2D2);
({void*_tmp2D0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D1="\n";_tag_dyneither(_tmp2D1,sizeof(char),2);}),_tag_dyneither(_tmp2D0,sizeof(void*),0));});
goto _LL1D4;}_LL1D4:;}
# 1378 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_tmp2D4=Cyc_Tcutil_compress(t);void*_tmp2D5=_tmp2D4;void*_tmp2E9;void*_tmp2E8;void*_tmp2E7;void*_tmp2E6;void*_tmp2E5;void*_tmp2E4;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E2;void*_tmp2E1;void*_tmp2E0;struct Cyc_List_List*_tmp2DF;struct Cyc_List_List*_tmp2DE;struct Cyc_List_List*_tmp2DD;struct Cyc_List_List*_tmp2DC;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Tvar*_tmp2DA;switch(*((int*)_tmp2D5)){case 26: _LL1DA: _LL1DB:
 goto _LL1DD;case 0: _LL1DC: _LL1DD:
 goto _LL1DF;case 6: _LL1DE: _LL1DF:
 goto _LL1E1;case 7: _LL1E0: _LL1E1:
 goto _LL1E3;case 13: _LL1E2: _LL1E3:
 goto _LL1E5;case 14: _LL1E4: _LL1E5:
 goto _LL1E7;case 19: _LL1E6: _LL1E7:
 goto _LL1E9;case 20: _LL1E8: _LL1E9:
 goto _LL1EB;case 28: _LL1EA: _LL1EB:
 goto _LL1ED;case 1: _LL1EC: _LL1ED:
 return 0;case 21: _LL1EE: _LL1EF: {
# 1391
void*_tmp2D6=rgn;if(((struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp2D6)->tag == 0){_LL215: _LL216:
 return 1;}else{_LL217: _LL218:
 return 0;}_LL214:;}case 22: _LL1F0: _LL1F1: {
# 1396
void*_tmp2D7=rgn;if(((struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp2D7)->tag == 1){_LL21A: _LL21B:
 return 1;}else{_LL21C: _LL21D:
 return 0;}_LL219:;}case 2: _LL1F2: _tmp2DA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D5)->f1;_LL1F3: {
# 1401
void*_tmp2D8=rgn;struct Cyc_Absyn_Tvar*_tmp2D9;switch(*((int*)_tmp2D8)){case 1: _LL21F: _LL220:
 goto _LL222;case 0: _LL221: _LL222:
 return 0;default: _LL223: _tmp2D9=((struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp2D8)->f1;_LL224:
 return Cyc_Absyn_tvar_cmp(_tmp2DA,_tmp2D9)== 0;}_LL21E:;}case 3: _LL1F4: _tmp2DB=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1F5:
# 1414 "cf_flowinfo.cyc"
 _tmp2DC=_tmp2DB;goto _LL1F7;case 17: _LL1F6: _tmp2DC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5)->f2;_LL1F7:
 _tmp2DD=_tmp2DC;goto _LL1F9;case 11: _LL1F8: _tmp2DD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1F9:
 _tmp2DE=_tmp2DD;goto _LL1FB;case 4: _LL1FA: _tmp2DE=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1FB:
 _tmp2DF=_tmp2DE;goto _LL1FD;case 24: _LL1FC: _tmp2DF=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D5)->f1;_LL1FD:
# 1419
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp2DF);case 5: _LL1FE: _tmp2E1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5)->f1).elt_typ;_tmp2E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5)->f1).ptr_atts).rgn;_LL1FF:
# 1421
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp2E0) || Cyc_CfFlowInfo_contains_region(rgn,_tmp2E1);case 9: _LL200: _LL201:
 return 0;case 10: _LL202: _tmp2E2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D5)->f1;_LL203:
# 1424
 for(0;_tmp2E2 != 0;_tmp2E2=_tmp2E2->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple17*)_tmp2E2->hd)).f2))return 1;}
return 0;case 12: _LL204: _tmp2E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5)->f2;_LL205:
# 1428
 for(0;_tmp2E3 != 0;_tmp2E3=_tmp2E3->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp2E3->hd)->type))return 1;}
return 0;case 8: _LL206: _tmp2E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5)->f1).elt_type;_LL207:
 _tmp2E5=_tmp2E4;goto _LL209;case 23: _LL208: _tmp2E5=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D5)->f1;_LL209:
 _tmp2E6=_tmp2E5;goto _LL20B;case 25: _LL20A: _tmp2E6=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D5)->f1;_LL20B:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp2E6);case 16: _LL20C: _tmp2E8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D5)->f1;_tmp2E7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D5)->f2;_LL20D:
# 1435
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp2E8) || Cyc_CfFlowInfo_contains_region(rgn,_tmp2E7);case 15: _LL20E: _tmp2E9=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D5)->f1;_LL20F:
# 1438
 return 0;case 18: _LL210: _LL211:
 return 0;default: _LL212: _LL213:
 return 0;}_LL1D9:;}struct _tuple24{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1445
static void Cyc_CfFlowInfo_kill_root(struct _tuple24*env,void*root,void*rval){
struct _tuple24*_tmp2EA=env;struct Cyc_CfFlowInfo_FlowEnv*_tmp2F0;struct Cyc_Dict_Dict*_tmp2EF;void*_tmp2EE;_LL226: _tmp2F0=_tmp2EA->f1;_tmp2EF=(struct Cyc_Dict_Dict*)& _tmp2EA->f2;_tmp2EE=_tmp2EA->f3;_LL227:;{
void*_tmp2EB=root;void*_tmp2ED;struct Cyc_Absyn_Vardecl*_tmp2EC;switch(*((int*)_tmp2EB)){case 0: _LL229: _tmp2EC=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2EB)->f1;_LL22A:
# 1450
 if(Cyc_CfFlowInfo_contains_region(_tmp2EE,_tmp2EC->type))
# 1452
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp2F0,_tmp2EC->type,0,_tmp2F0->unknown_none);
*_tmp2EF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp2EF,root,rval);
goto _LL228;case 1: _LL22B: _tmp2ED=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2EB)->f2;_LL22C:
# 1457
 if(!Cyc_CfFlowInfo_contains_region(_tmp2EE,_tmp2ED))
# 1459
*_tmp2EF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp2EF,root,rval);
goto _LL228;default: _LL22D: _LL22E:
# 1463
 goto _LL228;}_LL228:;};}
# 1468
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1470
void*_tmp2F1=Cyc_Tcutil_compress(rgn);void*_tmp2F2=_tmp2F1;struct Cyc_Absyn_Tvar*_tmp2FA;switch(*((int*)_tmp2F2)){case 2: _LL230: _tmp2FA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2F2)->f1;_LL231: {
# 1472
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=({struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp2F4;_tmp2F4.tag=2;_tmp2F4.f1=_tmp2FA;_tmp2F4;});
void*v2=(void*)& v;
struct _tuple24 env=({struct _tuple24 _tmp2F3;_tmp2F3.f1=fenv;_tmp2F3.f2=fenv->mt_flowdict;_tmp2F3.f3=v2;_tmp2F3;});
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}case 21: _LL232: _LL233: {
# 1479
struct _tuple24 env=({struct _tuple24 _tmp2F5;_tmp2F5.f1=fenv;_tmp2F5.f2=fenv->mt_flowdict;_tmp2F5.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val;_tmp2F5;});
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}case 22: _LL234: _LL235: {
# 1484
struct _tuple24 env=({struct _tuple24 _tmp2F6;_tmp2F6.f1=fenv;_tmp2F6.f2=fenv->mt_flowdict;_tmp2F6.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val;_tmp2F6;});
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}default: _LL236: _LL237:
# 1488
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2F8;_tmp2F8.tag=Cyc_Core_Impossible;_tmp2F8.f1=({const char*_tmp2F9="kill_flowdict_region";_tag_dyneither(_tmp2F9,sizeof(char),21);});_tmp2F8;});_tmp2F7;}));}_LL22F:;}
# 1493
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1496
return 0;}
# 1501
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp2FB=f;struct Cyc_Dict_Dict _tmp2FF;struct Cyc_List_List*_tmp2FE;if((_tmp2FB.BottomFL).tag == 1){_LL239: _LL23A:
 return f;}else{_LL23B: _tmp2FF=((_tmp2FB.ReachableFL).val).f1;_tmp2FE=((_tmp2FB.ReachableFL).val).f2;_LL23C: {
# 1505
struct Cyc_Dict_Dict _tmp2FC=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp2FF,rgn);
struct Cyc_List_List*_tmp2FD=Cyc_CfFlowInfo_kill_relns_region(_tmp2FE,rgn);
# 1508
return Cyc_CfFlowInfo_ReachableFL(_tmp2FC,_tmp2FD);}}_LL238:;}
