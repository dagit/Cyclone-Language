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
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 110
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 453 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 937 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 946
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 964
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1202
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1206
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
# 284 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 325
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
# 40
Cyc_CfFlowInfo_anal_error=1;
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 53 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 57
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _tmp300;return((_tmp300.PlaceL).val=x,(((_tmp300.PlaceL).tag=1,_tmp300)));}
# 60
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp301;return((_tmp301.UnknownL).val=0,(((_tmp301.UnknownL).tag=2,_tmp301)));}
# 63
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp302;return((_tmp302.BottomFL).val=0,(((_tmp302.BottomFL).tag=1,_tmp302)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
struct _tuple11 _tmp305;union Cyc_CfFlowInfo_FlowInfo _tmp304;return((_tmp304.ReachableFL).val=((_tmp305.f1=fd,((_tmp305.f2=r,_tmp305)))),(((_tmp304.ReachableFL).tag=2,_tmp304)));}
# 69
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp33C;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp33B;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp33A;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp339;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp338;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp337;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp336;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp335;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp334;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp333;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp332;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp331;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp330;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp32F;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp32E;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp32D;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp32C;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp32B;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp32A;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp329;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp328;struct Cyc_CfFlowInfo_FlowEnv*_tmp327;return(_tmp327=_region_malloc(r,sizeof(*_tmp327)),((_tmp327->r=r,((_tmp327->zero=(void*)(
# 76
(_tmp329=_region_malloc(r,sizeof(*_tmp329)),((_tmp329[0]=((_tmp328.tag=0,_tmp328)),_tmp329)))),((_tmp327->notzeroall=(void*)(
(_tmp32B=_region_malloc(r,sizeof(*_tmp32B)),((_tmp32B[0]=((_tmp32A.tag=1,_tmp32A)),_tmp32B)))),((_tmp327->notzerothis=(void*)(
(_tmp32D=_region_malloc(r,sizeof(*_tmp32D)),((_tmp32D[0]=((_tmp32C.tag=2,_tmp32C)),_tmp32D)))),((_tmp327->unknown_none=(void*)(
(_tmp32F=_region_malloc(r,sizeof(*_tmp32F)),((_tmp32F[0]=((_tmp32E.tag=3,((_tmp32E.f1=Cyc_CfFlowInfo_NoneIL,_tmp32E)))),_tmp32F)))),((_tmp327->unknown_this=(void*)(
(_tmp331=_region_malloc(r,sizeof(*_tmp331)),((_tmp331[0]=((_tmp330.tag=3,((_tmp330.f1=Cyc_CfFlowInfo_ThisIL,_tmp330)))),_tmp331)))),((_tmp327->unknown_all=(void*)(
(_tmp333=_region_malloc(r,sizeof(*_tmp333)),((_tmp333[0]=((_tmp332.tag=3,((_tmp332.f1=Cyc_CfFlowInfo_AllIL,_tmp332)))),_tmp333)))),((_tmp327->esc_none=(void*)(
(_tmp335=_region_malloc(r,sizeof(*_tmp335)),((_tmp335[0]=((_tmp334.tag=4,((_tmp334.f1=Cyc_CfFlowInfo_NoneIL,_tmp334)))),_tmp335)))),((_tmp327->esc_this=(void*)(
(_tmp337=_region_malloc(r,sizeof(*_tmp337)),((_tmp337[0]=((_tmp336.tag=4,((_tmp336.f1=Cyc_CfFlowInfo_ThisIL,_tmp336)))),_tmp337)))),((_tmp327->esc_all=(void*)(
(_tmp339=_region_malloc(r,sizeof(*_tmp339)),((_tmp339[0]=((_tmp338.tag=4,((_tmp338.f1=Cyc_CfFlowInfo_AllIL,_tmp338)))),_tmp339)))),((_tmp327->mt_flowdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp327->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp327->dummy_place=(
(_tmp33C=_region_malloc(r,sizeof(*_tmp33C)),((_tmp33C->root=(void*)((_tmp33A=_region_malloc(r,sizeof(*_tmp33A)),((_tmp33A[0]=((_tmp33B.tag=1,((_tmp33B.f1=& dummy_exp,((_tmp33B.f2=(void*)& Cyc_Absyn_VoidType_val,_tmp33B)))))),_tmp33A)))),((_tmp33C->fields=0,_tmp33C)))))),_tmp327)))))))))))))))))))))))))));}
# 93
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc){
if(set != 0){
# 96
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))
return 1;else{
# 99
*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}
# 101
return 0;}
# 105
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp1F=*place;struct Cyc_CfFlowInfo_Place _tmp20=_tmp1F;void*_tmp21;struct Cyc_List_List*_tmp22;_LL1: _tmp21=_tmp20.root;_tmp22=_tmp20.fields;_LL2:;{
void*_tmp23=_tmp21;struct Cyc_Absyn_Vardecl*_tmp24;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp23)->tag == 0){_LL4: _tmp24=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp23)->f1;_LL5:
# 109
 if(_tmp22 == 0)
return Cyc_Absynpp_qvar2string(_tmp24->name);else{
# 112
const char*_tmp340;void*_tmp33F[1];struct Cyc_String_pa_PrintArg_struct _tmp33E;return(struct _dyneither_ptr)((_tmp33E.tag=0,((_tmp33E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp33F[0]=& _tmp33E,Cyc_aprintf(((_tmp340="reachable from %s",_tag_dyneither(_tmp340,sizeof(char),18))),_tag_dyneither(_tmp33F,sizeof(void*),1))))))));}}else{_LL6: _LL7: {
const char*_tmp343;void*_tmp342;(_tmp342=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp343="error locations not for VarRoots",_tag_dyneither(_tmp343,sizeof(char),33))),_tag_dyneither(_tmp342,sizeof(void*),0)));}}_LL3:;};}
# 118
static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,unsigned int loc1,unsigned int loc2){
# 122
return loc1;}
# 127
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){
# 134
int _tmp2A=0;
struct Cyc_Dict_Dict _tmp2B=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2A,s1,s2);
# 139
return _tmp2B;}struct _tuple14{void*f1;unsigned int f2;};
# 142
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct _RegionHandle _tmp2F=_new_region("temp");struct _RegionHandle*temp=& _tmp2F;_push_region(temp);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple14 _tmp30=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp30)){
void*_tmp31=_tmp30.f1;
if(!((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp31)){
# 154
int _tmp32=0;_npop_handler(1);return _tmp32;}}{
# 157
int _tmp33=1;_npop_handler(1);return _tmp33;};}
# 146
;_pop_region(temp);}
# 145
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp34=_tmp2D;void*_tmp35;_LL9: _tmp35=_tmp34;_LLA:
# 161
 return 1;_LL8:;}};};}
# 165
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}
# 170
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp36=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp36,f)== 0)return n;
n=n + 1;}{
# 177
struct Cyc_Core_Impossible_exn_struct _tmp350;const char*_tmp34F;void*_tmp34E[1];struct Cyc_String_pa_PrintArg_struct _tmp34D;struct Cyc_Core_Impossible_exn_struct*_tmp34C;(int)_throw((void*)((_tmp34C=_cycalloc(sizeof(*_tmp34C)),((_tmp34C[0]=((_tmp350.tag=Cyc_Core_Impossible,((_tmp350.f1=(struct _dyneither_ptr)((_tmp34D.tag=0,((_tmp34D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp34E[0]=& _tmp34D,Cyc_aprintf(((_tmp34F="get_field_index_fs failed: %s",_tag_dyneither(_tmp34F,sizeof(char),30))),_tag_dyneither(_tmp34E,sizeof(void*),1)))))))),_tmp350)))),_tmp34C)))));};}
# 180
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp3C=Cyc_Tcutil_compress(t);void*_tmp3D=_tmp3C;struct Cyc_List_List*_tmp3E;union Cyc_Absyn_AggrInfoU _tmp3F;switch(*((int*)_tmp3D)){case 11: _LLE: _tmp3F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3D)->f1).aggr_info;_LLF: {
# 183
struct Cyc_List_List*_tmp40=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp3F))->impl))->fields;
_tmp3E=_tmp40;goto _LL11;}case 12: _LL10: _tmp3E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D)->f2;_LL11:
# 186
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp3E,f);default: _LL12: _LL13: {
# 188
struct Cyc_Core_Impossible_exn_struct _tmp35D;const char*_tmp35C;void*_tmp35B[1];struct Cyc_String_pa_PrintArg_struct _tmp35A;struct Cyc_Core_Impossible_exn_struct*_tmp359;(int)_throw((void*)((_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359[0]=((_tmp35D.tag=Cyc_Core_Impossible,((_tmp35D.f1=(struct _dyneither_ptr)((_tmp35A.tag=0,((_tmp35A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp35B[0]=& _tmp35A,Cyc_aprintf(((_tmp35C="get_field_index failed: %s",_tag_dyneither(_tmp35C,sizeof(char),27))),_tag_dyneither(_tmp35B,sizeof(void*),1)))))))),_tmp35D)))),_tmp359)))));}}_LLD:;}
# 193
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp35E;struct _tuple0 _tmp46=(_tmp35E.f1=r1,((_tmp35E.f2=r2,_tmp35E)));struct _tuple0 _tmp47=_tmp46;int _tmp48;int _tmp49;struct Cyc_Absyn_Exp*_tmp4A;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;struct Cyc_Absyn_Vardecl*_tmp4D;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f1)->tag == 0){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 0){_LL15: _tmp4C=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f1)->f1;_tmp4D=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f2)->f1;_LL16:
 return(int)_tmp4C - (int)_tmp4D;}else{_LL17: _LL18:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 0){_LL19: _LL1A:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f1)->tag == 1){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 1){_LL1B: _tmp4A=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f1)->f1;_tmp4B=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f2)->f1;_LL1C:
 return(int)_tmp4A - (int)_tmp4B;}else{_LL1D: _LL1E:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp47.f2)->tag == 1){_LL1F: _LL20:
 return 1;}else{_LL21: _tmp48=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp47.f1)->f1;_tmp49=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp47.f2)->f1;_LL22:
 return _tmp48 - _tmp49;}}}}_LL14:;};}
# 207
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 209
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 212
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp4F=*p1;struct Cyc_CfFlowInfo_Place _tmp50=_tmp4F;void*_tmp51;struct Cyc_List_List*_tmp52;_LL24: _tmp51=_tmp50.root;_tmp52=_tmp50.fields;_LL25:;{
struct Cyc_CfFlowInfo_Place _tmp53=*p2;struct Cyc_CfFlowInfo_Place _tmp54=_tmp53;void*_tmp55;struct Cyc_List_List*_tmp56;_LL27: _tmp55=_tmp54.root;_tmp56=_tmp54.fields;_LL28:;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp51,_tmp55);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp52,_tmp56);};};};}
# 224
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
struct Cyc_CfFlowInfo_Place _tmp57=*p;struct Cyc_CfFlowInfo_Place _tmp58=_tmp57;void*_tmp59;struct Cyc_List_List*_tmp5A;_LL2A: _tmp59=_tmp58.root;_tmp5A=_tmp58.fields;_LL2B:;
{void*_tmp5B=_tmp59;int _tmp5C;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Vardecl*_tmp5E;switch(*((int*)_tmp5B)){case 0: _LL2D: _tmp5E=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp5B)->f1;_LL2E:
{struct _dyneither_ptr*_tmp36B;const char*_tmp36A;void*_tmp369[1];struct Cyc_String_pa_PrintArg_struct _tmp368;struct Cyc_List_List*_tmp367;sl=((_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367->hd=((_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B[0]=((_tmp368.tag=0,((_tmp368.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp5E->name).f2),((_tmp369[0]=& _tmp368,Cyc_aprintf(((_tmp36A="%s",_tag_dyneither(_tmp36A,sizeof(char),3))),_tag_dyneither(_tmp369,sizeof(void*),1)))))))),_tmp36B)))),((_tmp367->tl=sl,_tmp367))))));}goto _LL2C;case 1: _LL2F: _tmp5D=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp5B)->f1;_LL30:
{struct _dyneither_ptr*_tmp378;const char*_tmp377;void*_tmp376[1];struct Cyc_Int_pa_PrintArg_struct _tmp375;struct Cyc_List_List*_tmp374;sl=((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374->hd=((_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378[0]=((_tmp375.tag=1,((_tmp375.f1=(unsigned long)((int)_tmp5D),((_tmp376[0]=& _tmp375,Cyc_aprintf(((_tmp377="mpt%d",_tag_dyneither(_tmp377,sizeof(char),6))),_tag_dyneither(_tmp376,sizeof(void*),1)))))))),_tmp378)))),((_tmp374->tl=sl,_tmp374))))));}goto _LL2C;default: _LL31: _tmp5C=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp5B)->f1;_LL32:
{struct _dyneither_ptr*_tmp385;const char*_tmp384;void*_tmp383[1];struct Cyc_Int_pa_PrintArg_struct _tmp382;struct Cyc_List_List*_tmp381;sl=((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381->hd=((_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385[0]=((_tmp382.tag=1,((_tmp382.f1=(unsigned long)_tmp5C,((_tmp383[0]=& _tmp382,Cyc_aprintf(((_tmp384="param%d",_tag_dyneither(_tmp384,sizeof(char),8))),_tag_dyneither(_tmp383,sizeof(void*),1)))))))),_tmp385)))),((_tmp381->tl=sl,_tmp381))))));}goto _LL2C;}_LL2C:;}
# 232
for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){
struct _dyneither_ptr*_tmp392;const char*_tmp391;void*_tmp390[1];struct Cyc_Int_pa_PrintArg_struct _tmp38F;struct Cyc_List_List*_tmp38E;sl=((_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E->hd=((_tmp392=_cycalloc(sizeof(*_tmp392)),((_tmp392[0]=((_tmp38F.tag=1,((_tmp38F.f1=(unsigned long)((int)_tmp5A->hd),((_tmp390[0]=& _tmp38F,Cyc_aprintf(((_tmp391="%d",_tag_dyneither(_tmp391,sizeof(char),3))),_tag_dyneither(_tmp390,sizeof(void*),1)))))))),_tmp392)))),((_tmp38E->tl=sl,_tmp38E))))));}{
struct Cyc_String_pa_PrintArg_struct _tmp39D;const char*_tmp39C;void*_tmp39B[1];const char*_tmp39A;struct _dyneither_ptr*_tmp399;struct _dyneither_ptr*_tmp73=(_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=((_tmp39D.tag=0,((_tmp39D.f1=(struct _dyneither_ptr)((_tmp39C="",_tag_dyneither(_tmp39C,sizeof(char),1))),((_tmp39B[0]=& _tmp39D,Cyc_aprintf(((_tmp39A="%s",_tag_dyneither(_tmp39A,sizeof(char),3))),_tag_dyneither(_tmp39B,sizeof(void*),1)))))))),_tmp399)));
for(0;sl != 0;sl=sl->tl){
const char*_tmp3A2;void*_tmp3A1[2];struct Cyc_String_pa_PrintArg_struct _tmp3A0;struct Cyc_String_pa_PrintArg_struct _tmp39F;*_tmp73=((_tmp39F.tag=0,((_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp73),((_tmp3A0.tag=0,((_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp3A1[0]=& _tmp3A0,((_tmp3A1[1]=& _tmp39F,Cyc_aprintf(((_tmp3A2="%s.%s",_tag_dyneither(_tmp3A2,sizeof(char),6))),_tag_dyneither(_tmp3A1,sizeof(void*),2))))))))))))));}
return _tmp73;};}
# 250 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp3A8(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp3A7,unsigned int*_tmp3A6,void***_tmp3A4){for(*_tmp3A7=0;*_tmp3A7 < *_tmp3A6;(*_tmp3A7)++){(*_tmp3A4)[*_tmp3A7]=(void*)(*fenv)->unknown_all;}}
# 255
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 261
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp3A7;unsigned int _tmp3A6;struct _dyneither_ptr _tmp3A5;void**_tmp3A4;unsigned int _tmp3A3;struct _dyneither_ptr d=(_tmp3A3=sz,((_tmp3A4=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp3A3)),((_tmp3A5=_tag_dyneither(_tmp3A4,sizeof(void*),_tmp3A3),((((_tmp3A6=_tmp3A3,_tmp3A8(& fenv,& _tmp3A7,& _tmp3A6,& _tmp3A4))),_tmp3A5)))))));
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp7D=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_tmp7E=_tmp7D;struct _dyneither_ptr*_tmp7F;void*_tmp80;_LL34: _tmp7F=_tmp7E->name;_tmp80=_tmp7E->type;_LL35:;
if(_get_dyneither_size(*_tmp7F,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp80,leafval);}}
# 269
return d;}struct _tuple15{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 273
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){
# 277
struct _tuple15*_tmp85=env;struct _RegionHandle*_tmp86;struct Cyc_List_List*_tmp87;_LL37: _tmp86=_tmp85->f1;_tmp87=_tmp85->f2;_LL38:;{
void*_tmp88=Cyc_Tcutil_rsubstitute(_tmp86,_tmp87,f->type);
struct Cyc_Absyn_Exp*_tmp89=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp8A=_tmp89 == 0?0: Cyc_Tcutil_rsubsexp(_tmp86,_tmp87,_tmp89);
struct Cyc_Absyn_Aggrfield*_tmp3A9;return(_tmp3A9=_region_malloc(_tmp86,sizeof(*_tmp3A9)),((_tmp3A9->name=f->name,((_tmp3A9->tq=f->tq,((_tmp3A9->type=_tmp88,((_tmp3A9->width=f->width,((_tmp3A9->attributes=f->attributes,((_tmp3A9->requires_clause=_tmp8A,_tmp3A9)))))))))))));};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 288
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 294
struct _RegionHandle _tmp8C=_new_region("temp");struct _RegionHandle*temp=& _tmp8C;_push_region(temp);
# 298
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp8D=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8E=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp8F=Cyc_Tcutil_tvar_kind(_tmp8D,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp90=_tmp8F;switch(((struct Cyc_Absyn_Kind*)_tmp90)->kind){case Cyc_Absyn_RgnKind: _LL3A: _LL3B:
 goto _LL3D;case Cyc_Absyn_EffKind: _LL3C: _LL3D:
 continue;default: _LL3E: _LL3F:
 goto _LL39;}_LL39:;}{
# 307
struct _tuple16*_tmp3AC;struct Cyc_List_List*_tmp3AB;inst=((_tmp3AB=_region_malloc(temp,sizeof(*_tmp3AB)),((_tmp3AB->hd=((_tmp3AC=_region_malloc(temp,sizeof(*_tmp3AC)),((_tmp3AC->f1=_tmp8D,((_tmp3AC->f2=_tmp8E,_tmp3AC)))))),((_tmp3AB->tl=inst,_tmp3AB))))));};}
# 309
if(inst != 0){
struct _tuple15 _tmp3AD;struct _tuple15 _tmp93=(_tmp3AD.f1=temp,((_tmp3AD.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp3AD)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp93,fs);
struct _dyneither_ptr _tmp94=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp94;}else{
# 314
struct _dyneither_ptr _tmp96=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp96;}}
# 298
;_pop_region(temp);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp3B3(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp3B2,unsigned int*_tmp3B1,void***_tmp3AF){for(*_tmp3B2=0;*_tmp3B2 < *_tmp3B1;(*_tmp3B2)++){(*_tmp3AF)[*_tmp3B2]=(void*)(*fenv)->unknown_all;}}
# 318
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 324
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 326
{void*_tmp97=Cyc_Tcutil_compress(t);void*_tmp98=_tmp97;union Cyc_Absyn_Constraint*_tmp99;void*_tmp9A;void*_tmp9B;union Cyc_Absyn_Constraint*_tmp9C;enum Cyc_Absyn_AggrKind _tmp9D;struct Cyc_List_List*_tmp9E;union Cyc_Absyn_AggrInfoU _tmp9F;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmpA1;struct Cyc_Absyn_Datatypefield*_tmpA2;switch(*((int*)_tmp98)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp98)->f1).field_info).KnownDatatypefield).tag == 2){_LL41: _tmpA2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp98)->f1).field_info).KnownDatatypefield).val).f2;_LL42:
# 328
 if(_tmpA2->typs == 0)
return leafval;
_tmpA1=_tmpA2->typs;goto _LL44;}else{goto _LL4F;}case 10: _LL43: _tmpA1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp98)->f1;_LL44: {
# 332
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA1);
# 335
unsigned int _tmp3B2;unsigned int _tmp3B1;struct _dyneither_ptr _tmp3B0;void**_tmp3AF;unsigned int _tmp3AE;struct _dyneither_ptr d=(_tmp3AE=sz,((_tmp3AF=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp3AE)),((_tmp3B0=_tag_dyneither(_tmp3AF,sizeof(void*),_tmp3AE),((((_tmp3B1=_tmp3AE,_tmp3B3(& fenv,& _tmp3B2,& _tmp3B1,& _tmp3AF))),_tmp3B0)))))));
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpA1))->hd)).f2,leafval);
_tmpA1=_tmpA1->tl;}}{
# 340
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp3B9;struct Cyc_CfFlowInfo_UnionRInfo _tmp3B8;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3B7;return(void*)((_tmp3B7=_region_malloc(fenv->r,sizeof(*_tmp3B7)),((_tmp3B7[0]=((_tmp3B9.tag=6,((_tmp3B9.f1=((_tmp3B8.is_union=0,((_tmp3B8.fieldnum=- 1,_tmp3B8)))),((_tmp3B9.f2=d,_tmp3B9)))))),_tmp3B7))));};}case 11: _LL45: _tmp9F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp98)->f1).aggr_info;_tmpA0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp98)->f1).targs;_LL46: {
# 342
struct Cyc_Absyn_Aggrdecl*_tmpAA=Cyc_Absyn_get_known_aggrdecl(_tmp9F);
if(_tmpAA->impl == 0)
goto _LL40;{
struct Cyc_List_List*_tmpAB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA->impl))->fields;
if(_tmpA0 == 0){_tmp9D=_tmpAA->kind;_tmp9E=_tmpAB;goto _LL48;}{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp3BF;struct Cyc_CfFlowInfo_UnionRInfo _tmp3BE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3BD;return(void*)((_tmp3BD=_region_malloc(fenv->r,sizeof(*_tmp3BD)),((_tmp3BD[0]=(
(_tmp3BF.tag=6,((_tmp3BF.f1=((_tmp3BE.is_union=_tmpAA->kind == Cyc_Absyn_UnionA,((_tmp3BE.fieldnum=- 1,_tmp3BE)))),((_tmp3BF.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpAA->tvs,_tmpA0,_tmpAB,_tmpAA->kind == Cyc_Absyn_UnionA,leafval),_tmp3BF)))))),_tmp3BD))));};};}case 12: _LL47: _tmp9D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp98)->f1;_tmp9E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp98)->f2;_LL48: {
# 353
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp3C5;struct Cyc_CfFlowInfo_UnionRInfo _tmp3C4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3C3;return(void*)((_tmp3C3=_region_malloc(fenv->r,sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C5.tag=6,((_tmp3C5.f1=((_tmp3C4.is_union=_tmp9D == Cyc_Absyn_UnionA,((_tmp3C4.fieldnum=- 1,_tmp3C4)))),((_tmp3C5.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmp9E,_tmp9D == Cyc_Absyn_UnionA,leafval),_tmp3C5)))))),_tmp3C3))));}case 8: _LL49: _tmp9B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp98)->f1).elt_type;_tmp9C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp98)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9C)){_LL4A:
# 357
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmp9B)?fenv->unknown_all: leafval;}else{goto _LL4F;}case 19: _LL4B: _tmp9A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp98)->f1;_LL4C:
 return leafval;case 5: _LL4D: _tmp99=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98)->f1).ptr_atts).nullable;if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp99)){_LL4E:
{void*_tmpB2=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB2)->tag == 3)switch(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB2)->f1){case Cyc_CfFlowInfo_ThisIL: _LL52: _LL53:
 return fenv->notzerothis;case Cyc_CfFlowInfo_AllIL: _LL54: _LL55:
 return fenv->notzeroall;default: goto _LL56;}else{_LL56: _LL57:
 goto _LL51;}_LL51:;}
# 365
goto _LL40;}else{goto _LL4F;}default: _LL4F: _LL50:
 goto _LL40;}_LL40:;}
# 369
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 372
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 378
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpB3=r;void*_tmpB4;int _tmpB5;int _tmpB6;struct _dyneither_ptr _tmpB7;struct Cyc_Absyn_Exp*_tmpB8;int _tmpB9;void*_tmpBA;switch(*((int*)_tmpB3)){case 7: _LL59: _tmpB8=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_tmpB9=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_tmpBA=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f3;_LL5A:
# 381
 if(_tmpB8 == e  && _tmpB9 == env_iteration){
*needs_unconsume=1;
return 0;}
# 385
return 1;case 6: _LL5B: _tmpB5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1).is_union;_tmpB6=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1).fieldnum;_tmpB7=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_LL5C:
# 387
 if(!_tmpB5  || _tmpB6 == - 1){
unsigned int _tmpBB=_get_dyneither_size(_tmpB7,sizeof(void*));
{int i=0;for(0;i < _tmpBB;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmpB7.curr)[i],needs_unconsume))
return 1;}}
# 393
return 0;}else{
# 396
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpB7,sizeof(void*),_tmpB6)),needs_unconsume);}case 8: _LL5D: _tmpB4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_LL5E:
# 398
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpB4,needs_unconsume);default: _LL5F: _LL60:
 return 0;}_LL58:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp3CB(struct _dyneither_ptr*_tmpC0,unsigned int*_tmp3CA,unsigned int*_tmp3C9,void***_tmp3C7){for(*_tmp3CA=0;*_tmp3CA < *_tmp3C9;(*_tmp3CA)++){(*_tmp3C7)[*_tmp3CA]=(void*)((void**)(*_tmpC0).curr)[(int)*_tmp3CA];}}
# 405
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpBC=r;struct Cyc_Absyn_Vardecl*_tmpBD;void*_tmpBE;struct Cyc_CfFlowInfo_UnionRInfo _tmpBF;struct _dyneither_ptr _tmpC0;struct Cyc_Absyn_Exp*_tmpC1;int _tmpC2;void*_tmpC3;switch(*((int*)_tmpBC)){case 7: _LL62: _tmpC1=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f1;_tmpC2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f2;_tmpC3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f3;_LL63:
# 408
 return _tmpC3;case 6: _LL64: _tmpBF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f1;_tmpC0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f2;_LL65: {
# 410
unsigned int _tmpC4=_get_dyneither_size(_tmpC0,sizeof(void*));
int change=0;
unsigned int _tmp3CA;unsigned int _tmp3C9;struct _dyneither_ptr _tmp3C8;void**_tmp3C7;unsigned int _tmp3C6;struct _dyneither_ptr d2=(_tmp3C6=_tmpC4,((_tmp3C7=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp3C6)),((_tmp3C8=_tag_dyneither(_tmp3C7,sizeof(void*),_tmp3C6),((((_tmp3C9=_tmp3C6,_tmp3CB(& _tmpC0,& _tmp3CA,& _tmp3C9,& _tmp3C7))),_tmp3C8)))))));
{int i=0;for(0;i < _tmpC4;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmpC0.curr)[i]);
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= ((void**)_tmpC0.curr)[i])
change=1;}}
# 418
if(change){
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp3CE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3CD;return(void*)((_tmp3CD=_region_malloc(fenv->r,sizeof(*_tmp3CD)),((_tmp3CD[0]=((_tmp3CE.tag=6,((_tmp3CE.f1=_tmpBF,((_tmp3CE.f2=d2,_tmp3CE)))))),_tmp3CD))));}else{
return r;}}case 8: _LL66: _tmpBD=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f1;_tmpBE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpBC)->f2;_LL67: {
# 422
void*_tmpCB=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpBE);
if(_tmpCB != _tmpBE){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3D1;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3D0;return(void*)((_tmp3D0=_region_malloc(fenv->r,sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D1.tag=8,((_tmp3D1.f1=_tmpBD,((_tmp3D1.f2=_tmpCB,_tmp3D1)))))),_tmp3D0))));}else{
return r;}}default: _LL68: _LL69:
 return r;}_LL61:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp3DB(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp3DA,unsigned int*_tmp3D9,void***_tmp3D7){for(*_tmp3DA=0;*_tmp3DA < *_tmp3D9;(*_tmp3DA)++){(*_tmp3D7)[*_tmp3DA]=(void*)(*fenv)->unknown_all;}}static void _tmp3E4(struct _dyneither_ptr*_tmpD3,unsigned int*_tmp3E3,unsigned int*_tmp3E2,void***_tmp3E0){for(*_tmp3E3=0;*_tmp3E3 < *_tmp3E2;(*_tmp3E3)++){(*_tmp3E0)[*_tmp3E3]=(void*)((void**)(*_tmpD3).curr)[(int)*_tmp3E3];}}
# 432
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmp3D2;struct _tuple0 _tmpCE=(_tmp3D2.f1=Cyc_Tcutil_compress(t),((_tmp3D2.f2=r,_tmp3D2)));struct _tuple0 _tmpCF=_tmpCE;enum Cyc_Absyn_AggrKind _tmpD0;struct Cyc_List_List*_tmpD1;struct Cyc_CfFlowInfo_UnionRInfo _tmpD2;struct _dyneither_ptr _tmpD3;union Cyc_Absyn_AggrInfoU _tmpD4;struct Cyc_CfFlowInfo_UnionRInfo _tmpD5;struct _dyneither_ptr _tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_CfFlowInfo_UnionRInfo _tmpD8;struct _dyneither_ptr _tmpD9;struct Cyc_Absyn_Vardecl*_tmpDA;void*_tmpDB;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 8){_LL6B: _tmpDA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpDB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL6C: {
# 435
void*_tmpDC=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpDB);
if(_tmpDC != _tmpDB){struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3D5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3D4;return(void*)((_tmp3D4=_region_malloc(fenv->r,sizeof(*_tmp3D4)),((_tmp3D4[0]=((_tmp3D5.tag=8,((_tmp3D5.f1=_tmpDA,((_tmp3D5.f2=_tmpDC,_tmp3D5)))))),_tmp3D4))));}else{
return r;}}}else{switch(*((int*)_tmpCF.f1)){case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 6){_LL6D: _tmpD7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCF.f1)->f1;_tmpD8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpD9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL6E: {
# 439
unsigned int _tmpDF=_get_dyneither_size(_tmpD9,sizeof(void*));
unsigned int _tmp3DA;unsigned int _tmp3D9;struct _dyneither_ptr _tmp3D8;void**_tmp3D7;unsigned int _tmp3D6;struct _dyneither_ptr d2=(_tmp3D6=_tmpDF,((_tmp3D7=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp3D6)),((_tmp3D8=_tag_dyneither(_tmp3D7,sizeof(void*),_tmp3D6),((((_tmp3D9=_tmp3D6,_tmp3DB(& fenv,& _tmp3DA,& _tmp3D9,& _tmp3D7))),_tmp3D8)))))));
{int i=0;for(0;i < _tmpDF;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpD7))->hd)).f2,consumer,iteration,((void**)_tmpD9.curr)[i]);
_tmpD7=_tmpD7->tl;}}{
# 445
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp3DE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3DD;return(void*)((_tmp3DD=_region_malloc(fenv->r,sizeof(*_tmp3DD)),((_tmp3DD[0]=((_tmp3DE.tag=6,((_tmp3DE.f1=_tmpD8,((_tmp3DE.f2=d2,_tmp3DE)))))),_tmp3DD))));};}}else{goto _LL73;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 6){_LL6F: _tmpD4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCF.f1)->f1).aggr_info;_tmpD5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpD6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL70: {
# 447
struct Cyc_Absyn_Aggrdecl*_tmpE6=Cyc_Absyn_get_known_aggrdecl(_tmpD4);
if(_tmpE6->impl == 0)return r;
_tmpD0=_tmpE6->kind;_tmpD1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE6->impl))->fields;_tmpD2=_tmpD5;_tmpD3=_tmpD6;goto _LL72;}}else{goto _LL73;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->tag == 6){_LL71: _tmpD0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCF.f1)->f1;_tmpD1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCF.f1)->f2;_tmpD2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f1;_tmpD3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCF.f2)->f2;_LL72: {
# 451
unsigned int _tmp3E3;unsigned int _tmp3E2;struct _dyneither_ptr _tmp3E1;void**_tmp3E0;unsigned int _tmp3DF;struct _dyneither_ptr d2=(_tmp3DF=_get_dyneither_size(_tmpD3,sizeof(void*)),((_tmp3E0=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp3DF)),((_tmp3E1=_tag_dyneither(_tmp3E0,sizeof(void*),_tmp3DF),((((_tmp3E2=_tmp3DF,_tmp3E4(& _tmpD3,& _tmp3E3,& _tmp3E2,& _tmp3E0))),_tmp3E1)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD1);
{int i=0;for(0;i < sz;(i ++,_tmpD1=_tmpD1->tl)){
struct Cyc_Absyn_Aggrfield*_tmpE7=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpD1))->hd;struct Cyc_Absyn_Aggrfield*_tmpE8=_tmpE7;struct _dyneither_ptr*_tmpE9;void*_tmpEA;_LL76: _tmpE9=_tmpE8->name;_tmpEA=_tmpE8->type;_LL77:;
if(_get_dyneither_size(*_tmpE9,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpEA,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpD3,sizeof(void*),i)));}}{
# 458
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp3E7;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3E6;return(void*)((_tmp3E6=_region_malloc(fenv->r,sizeof(*_tmp3E6)),((_tmp3E6[0]=((_tmp3E7.tag=6,((_tmp3E7.f1=_tmpD2,((_tmp3E7.f2=d2,_tmp3E7)))))),_tmp3E6))));};}}else{goto _LL73;}default: _LL73: _LL74:
# 460
 if(Cyc_Tcutil_is_noalias_pointer(t,0)){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp3EA;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3E9;return(void*)((_tmp3E9=_region_malloc(fenv->r,sizeof(*_tmp3E9)),((_tmp3E9[0]=((_tmp3EA.tag=7,((_tmp3EA.f1=consumer,((_tmp3EA.f2=iteration,((_tmp3EA.f3=r,_tmp3EA)))))))),_tmp3E9))));}else{
return r;}}}_LL6A:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 466
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 469
struct _RegionHandle _tmpF4=_new_region("r");struct _RegionHandle*r=& _tmpF4;_push_region(r);
{struct _tuple18 _tmp3EB;struct _tuple18 elem=(_tmp3EB.f1=place,((_tmp3EB.f2=0,_tmp3EB)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmpF5=elem.f1;
struct Cyc_CfFlowInfo_Place _tmpF6=*place;struct Cyc_CfFlowInfo_Place _tmpF7=_tmpF6;void*_tmpF8;struct Cyc_List_List*_tmpF9;_LL79: _tmpF8=_tmpF7.root;_tmpF9=_tmpF7.fields;_LL7A:;{
struct Cyc_CfFlowInfo_Place _tmpFA=*_tmpF5;struct Cyc_CfFlowInfo_Place _tmpFB=_tmpFA;void*_tmpFC;struct Cyc_List_List*_tmpFD;_LL7C: _tmpFC=_tmpFB.root;_tmpFD=_tmpFB.fields;_LL7D:;
if(Cyc_CfFlowInfo_root_cmp(_tmpF8,_tmpFC)!= 0)
continue;
for(0;_tmpF9 != 0  && _tmpFD != 0;(_tmpF9=_tmpF9->tl,_tmpFD=_tmpFD->tl)){
if((int)_tmpF9->hd != (int)_tmpFD->hd)break;}
if(_tmpF9 == 0){
int _tmpFE=1;_npop_handler(0);return _tmpFE;}};}{
# 483
int _tmpFF=0;_npop_handler(0);return _tmpFF;};}
# 470
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 496
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 498
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 501
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){
struct Cyc_List_List*_tmp3EC;pile->places=((_tmp3EC=_region_malloc(pile->rgn,sizeof(*_tmp3EC)),((_tmp3EC->hd=place,((_tmp3EC->tl=pile->places,_tmp3EC))))));}}
# 504
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp102=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp103;struct _dyneither_ptr _tmp104;struct Cyc_CfFlowInfo_Place*_tmp105;void*_tmp106;void*_tmp107;switch(*((int*)_tmp102)){case 7: _LL7F: _tmp107=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp102)->f3;_LL80:
 Cyc_CfFlowInfo_add_places(pile,_tmp107);return;case 8: _LL81: _tmp106=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102)->f2;_LL82:
 Cyc_CfFlowInfo_add_places(pile,_tmp106);return;case 5: _LL83: _tmp105=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp102)->f1;_LL84:
 Cyc_CfFlowInfo_add_place(pile,_tmp105);return;case 6: _LL85: _tmp103=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102)->f1;_tmp104=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102)->f2;_LL86:
# 510
{int i=0;for(0;i < _get_dyneither_size(_tmp104,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp104.curr)[i]);}}
return;default: _LL87: _LL88:
 return;}_LL7E:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp3F2(struct _RegionHandle**r,void**new_val,unsigned int*_tmp3F1,unsigned int*_tmp3F0,void***_tmp3EE,struct _dyneither_ptr*_tmp10F){for(*_tmp3F1=0;*_tmp3F1 < *_tmp3F0;(*_tmp3F1)++){(*_tmp3EE)[*_tmp3F1]=(void*)
# 525
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp10F).curr)[(int)*_tmp3F1]);}}
# 520
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 522
void*_tmp108=old_val;struct Cyc_Absyn_Vardecl*_tmp109;void*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;int _tmp10C;void*_tmp10D;int _tmp10E;struct _dyneither_ptr _tmp10F;switch(*((int*)_tmp108)){case 6: _LL8A: _tmp10E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp108)->f1).is_union;_tmp10F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp108)->f2;_LL8B: {
# 524
unsigned int _tmp3F1;unsigned int _tmp3F0;struct _dyneither_ptr _tmp3EF;void**_tmp3EE;unsigned int _tmp3ED;struct _dyneither_ptr d2=(_tmp3ED=_get_dyneither_size(_tmp10F,sizeof(void*)),((_tmp3EE=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp3ED)),((_tmp3EF=_tag_dyneither(_tmp3EE,sizeof(void*),_tmp3ED),((((_tmp3F0=_tmp3ED,_tmp3F2(& r,& new_val,& _tmp3F1,& _tmp3F0,& _tmp3EE,& _tmp10F))),_tmp3EF)))))));
# 528
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp3F8;struct Cyc_CfFlowInfo_UnionRInfo _tmp3F7;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3F6;return(void*)((_tmp3F6=_region_malloc(r,sizeof(*_tmp3F6)),((_tmp3F6[0]=((_tmp3F8.tag=6,((_tmp3F8.f1=((_tmp3F7.is_union=_tmp10E,((_tmp3F7.fieldnum=- 1,_tmp3F7)))),((_tmp3F8.f2=d2,_tmp3F8)))))),_tmp3F6))));}case 7: _LL8C: _tmp10B=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp108)->f1;_tmp10C=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp108)->f2;_tmp10D=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp108)->f3;_LL8D: {
# 530
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp3FB;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp3FA;return(void*)((_tmp3FA=_region_malloc(r,sizeof(*_tmp3FA)),((_tmp3FA[0]=((_tmp3FB.tag=7,((_tmp3FB.f1=_tmp10B,((_tmp3FB.f2=_tmp10C,((_tmp3FB.f3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp10D),_tmp3FB)))))))),_tmp3FA))));}case 8: _LL8E: _tmp109=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp108)->f1;_tmp10A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp108)->f2;_LL8F: {
# 532
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3FE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3FD;return(void*)((_tmp3FD=_region_malloc(r,sizeof(*_tmp3FD)),((_tmp3FD[0]=((_tmp3FE.tag=8,((_tmp3FE.f1=_tmp109,((_tmp3FE.f2=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp10A),_tmp3FE)))))),_tmp3FD))));}default: _LL90: _LL91:
 return new_val;}_LL89:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp404(struct _dyneither_ptr*d,unsigned int*_tmp403,unsigned int*_tmp402,void***_tmp400){for(*_tmp403=0;*_tmp403 < *_tmp402;(*_tmp403)++){(*_tmp400)[*_tmp403]=(void*)((void**)(*d).curr)[(int)*_tmp403];}}
# 539
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 541
void*_tmp11B=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp11B == rval)return d;{
unsigned int _tmp403;unsigned int _tmp402;struct _dyneither_ptr _tmp401;void**_tmp400;unsigned int _tmp3FF;struct _dyneither_ptr res=(_tmp3FF=_get_dyneither_size(d,sizeof(void*)),((_tmp400=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp3FF)),((_tmp401=_tag_dyneither(_tmp400,sizeof(void*),_tmp3FF),((((_tmp402=_tmp3FF,_tmp404(& d,& _tmp403,& _tmp402,& _tmp400))),_tmp401)))))));
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple19{struct Cyc_List_List*f1;void*f2;};
# 553
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 557
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple19 _tmp405;struct _tuple19 _tmp120=(_tmp405.f1=fs,((_tmp405.f2=old_val,_tmp405)));struct _tuple19 _tmp121=_tmp120;struct Cyc_Absyn_Vardecl*_tmp122;void*_tmp123;int _tmp124;struct Cyc_List_List*_tmp125;int _tmp126;struct _dyneither_ptr _tmp127;if(_tmp121.f1 != 0)switch(*((int*)_tmp121.f2)){case 6: _LL93: _tmp124=(int)(_tmp121.f1)->hd;_tmp125=(_tmp121.f1)->tl;_tmp126=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f1).is_union;_tmp127=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f2;_LL94: {
# 561
void*_tmp128=Cyc_CfFlowInfo_insert_place_outer(r,_tmp125,*((void**)_check_dyneither_subscript(_tmp127,sizeof(void*),_tmp124)),new_val);
struct _dyneither_ptr _tmp129=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp127,_tmp124,_tmp128);
if((void**)_tmp129.curr == (void**)_tmp127.curr)return old_val;else{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp40B;struct Cyc_CfFlowInfo_UnionRInfo _tmp40A;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp409;return(void*)((_tmp409=_region_malloc(r,sizeof(*_tmp409)),((_tmp409[0]=((_tmp40B.tag=6,((_tmp40B.f1=((_tmp40A.is_union=_tmp126,((_tmp40A.fieldnum=- 1,_tmp40A)))),((_tmp40B.f2=_tmp129,_tmp40B)))))),_tmp409))));}}case 8: goto _LL95;default: goto _LL97;}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->tag == 8){_LL95: _tmp122=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f1;_tmp123=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp121.f2)->f2;_LL96: {
# 566
void*_tmp12D=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp123,new_val);
if(_tmp12D == _tmp123)return old_val;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp40E;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp40D;return(void*)((_tmp40D=_region_malloc(r,sizeof(*_tmp40D)),((_tmp40D[0]=((_tmp40E.tag=8,((_tmp40E.f1=_tmp122,((_tmp40E.f2=_tmp12D,_tmp40E)))))),_tmp40D))));}}}else{_LL97: _LL98: {
struct Cyc_Core_Impossible_exn_struct _tmp414;const char*_tmp413;struct Cyc_Core_Impossible_exn_struct*_tmp412;(int)_throw((void*)((_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412[0]=((_tmp414.tag=Cyc_Core_Impossible,((_tmp414.f1=((_tmp413="bad insert place",_tag_dyneither(_tmp413,sizeof(char),17))),_tmp414)))),_tmp412)))));}}}_LL92:;};}
# 576
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 580
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
# 592
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmp13C=*_tmp134;struct Cyc_CfFlowInfo_Place _tmp13D=_tmp13C;void*_tmp13E;struct Cyc_List_List*_tmp13F;_LLA6: _tmp13E=_tmp13D.root;_tmp13F=_tmp13D.fields;_LLA7:;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp13E,
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp13F,
# 597
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp13E),newval));};};}
# 600
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 612
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp140=r;void*_tmp141;enum Cyc_CfFlowInfo_InitLevel _tmp142;enum Cyc_CfFlowInfo_InitLevel _tmp143;switch(*((int*)_tmp140)){case 3: _LLA9: _tmp143=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp140)->f1;_LLAA:
 return _tmp143;case 4: _LLAB: _tmp142=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp140)->f1;_LLAC:
 return _tmp142;case 0: _LLAD: _LLAE:
 goto _LLB0;case 1: _LLAF: _LLB0:
 return Cyc_CfFlowInfo_AllIL;case 2: _LLB1: _LLB2:
 return Cyc_CfFlowInfo_ThisIL;case 7: _LLB3: _tmp141=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp140)->f3;_LLB4:
 return Cyc_CfFlowInfo_NoneIL;default: _LLB5: _LLB6: {
struct Cyc_Core_Impossible_exn_struct _tmp41A;const char*_tmp419;struct Cyc_Core_Impossible_exn_struct*_tmp418;(int)_throw((void*)((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418[0]=((_tmp41A.tag=Cyc_Core_Impossible,((_tmp41A.f1=((_tmp419="initlevel_approx",_tag_dyneither(_tmp419,sizeof(char),17))),_tmp41A)))),_tmp418)))));}}_LLA8:;}
# 623
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 625
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp147=r;struct Cyc_CfFlowInfo_Place*_tmp148;struct _dyneither_ptr _tmp149;int _tmp14A;int _tmp14B;struct _dyneither_ptr _tmp14C;void*_tmp14D;switch(*((int*)_tmp147)){case 8: _LLB8: _tmp14D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp147)->f2;_LLB9:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp14D,acc);case 6: _LLBA: _tmp14A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f1).is_union;_tmp14B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f1).fieldnum;_tmp14C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f2;if(_tmp14A){_LLBB: {
# 633
unsigned int _tmp14E=_get_dyneither_size(_tmp14C,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp14B == - 1){
int i=0;for(0;i < _tmp14E;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp14C.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 643
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp14C,sizeof(void*),_tmp14B)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LLB7;}}else{_LLBC: _tmp149=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp147)->f2;_LLBD: {
# 647
unsigned int _tmp14F=_get_dyneither_size(_tmp149,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp14F;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp149.curr)[i],this_ans);}}
goto _LLB7;}}case 5: _LLBE: _tmp148=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp147)->f1;_LLBF:
# 653
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp148))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 656
{struct Cyc_List_List*_tmp41B;env->seen=((_tmp41B=_region_malloc(env->e,sizeof(*_tmp41B)),((_tmp41B->hd=_tmp148,((_tmp41B->tl=env->seen,_tmp41B))))));}
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp148),Cyc_CfFlowInfo_AllIL);
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 662
goto _LLB7;default: _LLC0: _LLC1:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);}_LLB7:;}
# 665
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 669
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp151=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp41C;struct Cyc_CfFlowInfo_InitlevelEnv _tmp152=(_tmp41C.e=_tmp151,((_tmp41C.d=d,((_tmp41C.seen=0,_tmp41C)))));
return Cyc_CfFlowInfo_initlevel_rec(& _tmp152,r,Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};
# 676
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp154=*place;struct Cyc_CfFlowInfo_Place _tmp155=_tmp154;void*_tmp156;struct Cyc_List_List*_tmp157;_LLC3: _tmp156=_tmp155.root;_tmp157=_tmp155.fields;_LLC4:;{
void*_tmp158=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp156);
for(0;_tmp157 != 0;_tmp157=_tmp157->tl){
retry: {struct _tuple20 _tmp41D;struct _tuple20 _tmp159=(_tmp41D.f1=_tmp158,((_tmp41D.f2=(int)_tmp157->hd,_tmp41D)));struct _tuple20 _tmp15A=_tmp159;struct Cyc_CfFlowInfo_UnionRInfo _tmp15B;struct _dyneither_ptr _tmp15C;int _tmp15D;void*_tmp15E;int _tmp15F;switch(*((int*)_tmp15A.f1)){case 8: _LLC6: _tmp15E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15A.f1)->f2;_tmp15F=_tmp15A.f2;_LLC7:
# 682
 _tmp158=_tmp15E;goto retry;case 6: _LLC8: _tmp15B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp15A.f1)->f1;_tmp15C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp15A.f1)->f2;_tmp15D=_tmp15A.f2;_LLC9:
# 684
 _tmp158=*((void**)_check_dyneither_subscript(_tmp15C,sizeof(void*),_tmp15D));
goto _LLC5;default: _LLCA: _LLCB: {
# 690
struct Cyc_Core_Impossible_exn_struct _tmp423;const char*_tmp422;struct Cyc_Core_Impossible_exn_struct*_tmp421;(int)_throw((void*)((_tmp421=_cycalloc(sizeof(*_tmp421)),((_tmp421[0]=((_tmp423.tag=Cyc_Core_Impossible,((_tmp423.f1=((_tmp422="bad lookup_place",_tag_dyneither(_tmp422,sizeof(char),17))),_tmp423)))),_tmp421)))));}}_LLC5:;}}
# 692
return _tmp158;};}
# 695
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp164=rval;int _tmp165;int _tmp166;struct _dyneither_ptr _tmp167;void*_tmp168;void*_tmp169;switch(*((int*)_tmp164)){case 4: _LLCD: _LLCE:
 return 0;case 7: _LLCF: _tmp169=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp164)->f3;_LLD0:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp169);case 8: _LLD1: _tmp168=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp164)->f2;_LLD2:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp168);case 6: _LLD3: _tmp165=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164)->f1).is_union;_tmp166=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164)->f1).fieldnum;_tmp167=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp164)->f2;_LLD4:
# 701
 if(_tmp165  && _tmp166 != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp167,sizeof(void*),_tmp166)));else{
# 704
unsigned int sz=_get_dyneither_size(_tmp167,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp167.curr)[i]))return 0;}}
return 1;}default: _LLD5: _LLD6:
# 709
 return 1;}_LLCC:;}
# 712
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 717
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp16A=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp424;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp424=_region_malloc(_tmp16A,sizeof(*_tmp424)),((_tmp424->rgn=_tmp16A,((_tmp424->places=0,_tmp424)))));
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp431(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp430,unsigned int*_tmp42F,void***_tmp42D,struct _dyneither_ptr*_tmp172,struct _dyneither_ptr*_tmp170){for(*_tmp430=0;*_tmp430 < *_tmp42F;(*_tmp430)++){(*_tmp42D)[*_tmp430]=(void*)
# 755
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp170).curr)[(int)*_tmp430],*((void**)_check_dyneither_subscript(*_tmp172,sizeof(void*),(int)*_tmp430)));}}
# 735
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 740
struct _tuple0 _tmp425;struct _tuple0 _tmp16C=(_tmp425.f1=oldval,((_tmp425.f2=newval,_tmp425)));struct _tuple0 _tmp16D=_tmp16C;enum Cyc_CfFlowInfo_InitLevel _tmp16E;struct Cyc_CfFlowInfo_UnionRInfo _tmp16F;struct _dyneither_ptr _tmp170;struct Cyc_CfFlowInfo_UnionRInfo _tmp171;struct _dyneither_ptr _tmp172;struct Cyc_CfFlowInfo_Place*_tmp173;struct Cyc_Absyn_Vardecl*_tmp174;void*_tmp175;void*_tmp176;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->tag == 8){_LLD8: _tmp176=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->f2;_LLD9:
# 742
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp176,newval);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->tag == 8){_LLDA: _tmp174=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_tmp175=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f2;_LLDB: {
# 744
void*_tmp177=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp175);
if(_tmp177 == _tmp175)return newval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp428;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp427;return(void*)((_tmp427=_region_malloc((env->fenv)->r,sizeof(*_tmp427)),((_tmp427[0]=((_tmp428.tag=8,((_tmp428.f1=_tmp174,((_tmp428.f2=_tmp177,_tmp428)))))),_tmp427))));}}}else{switch(*((int*)_tmp16D.f1)){case 4: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->tag == 5){_LLDC: _tmp173=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_LLDD:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp173);goto _LLDF;}else{_LLDE: _LLDF:
# 749
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp42B;void*_tmp42A;(_tmp42A=0,Cyc_CfFlowInfo_aerr(env->loc,((_tmp42B="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp42B,sizeof(char),67))),_tag_dyneither(_tmp42A,sizeof(void*),0)));}
# 752
return(env->fenv)->esc_all;}case 6: switch(*((int*)_tmp16D.f2)){case 6: _LLE0: _tmp16F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->f1;_tmp170=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f1)->f2;_tmp171=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_tmp172=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f2;_LLE1: {
# 754
unsigned int _tmp430;unsigned int _tmp42F;struct _dyneither_ptr _tmp42E;void**_tmp42D;unsigned int _tmp42C;struct _dyneither_ptr new_d=
(_tmp42C=_get_dyneither_size(_tmp170,sizeof(void*)),((_tmp42D=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp42C)),((_tmp42E=_tag_dyneither(_tmp42D,sizeof(void*),_tmp42C),((((_tmp42F=_tmp42C,_tmp431(& env,& _tmp430,& _tmp42F,& _tmp42D,& _tmp172,& _tmp170))),_tmp42E)))))));
# 758
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp170,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp170.curr)[i]){
change=1;break;}}}
# 763
if(!change){
if(!_tmp16F.is_union)return oldval;
new_d=_tmp170;}else{
# 768
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp170,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp172,sizeof(void*),i))){
change=1;break;}}}
# 773
if(!change){
if(!_tmp16F.is_union)return newval;
new_d=_tmp172;}}{
# 778
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp434;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp433;return(void*)((_tmp433=_region_malloc((env->fenv)->r,sizeof(*_tmp433)),((_tmp433[0]=((_tmp434.tag=6,((_tmp434.f1=_tmp171,((_tmp434.f2=new_d,_tmp434)))))),_tmp433))));};}case 4: goto _LLE2;default: goto _LLE4;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->tag == 4){_LLE2: _tmp16E=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16D.f2)->f1;_LLE3: {
# 780
enum Cyc_CfFlowInfo_InitLevel _tmp182=_tmp16E;switch(_tmp182){case Cyc_CfFlowInfo_NoneIL: _LLE7: _LLE8:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLE9: _LLEA:
 return(env->fenv)->unknown_this;default: _LLEB: _LLEC:
 return(env->fenv)->unknown_all;}_LLE6:;}}else{_LLE4: _LLE5:
# 785
 return newval;}}}}_LLD7:;}
# 791
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 795
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 800
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple19 _tmp435;struct _tuple19 _tmp184=(_tmp435.f1=fs,((_tmp435.f2=oldval,_tmp435)));struct _tuple19 _tmp185=_tmp184;int _tmp186;struct Cyc_List_List*_tmp187;int _tmp188;int _tmp189;struct _dyneither_ptr _tmp18A;struct Cyc_Absyn_Vardecl*_tmp18B;void*_tmp18C;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->tag == 8){_LLEE: _tmp18B=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f1;_tmp18C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f2;_LLEF: {
# 803
void*_tmp18D=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp18C,newval);
if(_tmp18D == _tmp18C)return oldval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp438;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp437;return(void*)((_tmp437=_region_malloc((env->fenv)->r,sizeof(*_tmp437)),((_tmp437[0]=((_tmp438.tag=8,((_tmp438.f1=_tmp18B,((_tmp438.f2=_tmp18D,_tmp438)))))),_tmp437))));}}}else{if(_tmp185.f1 != 0){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->tag == 6){_LLF0: _tmp186=(int)(_tmp185.f1)->hd;_tmp187=(_tmp185.f1)->tl;_tmp188=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f1).is_union;_tmp189=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f1).fieldnum;_tmp18A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp185.f2)->f2;_LLF1: {
# 807
void*_tmp190=Cyc_CfFlowInfo_assign_place_outer(env,_tmp187,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp18A,sizeof(void*),_tmp186)),newval);
# 809
struct _dyneither_ptr _tmp191=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp18A,_tmp186,_tmp190);
if((void**)_tmp191.curr == (void**)_tmp18A.curr  && (!_tmp188  || _tmp189 == _tmp186))return oldval;
_tmp18A=_tmp191;
# 814
if(_tmp188){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp18A,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp186){
struct _dyneither_ptr _tmp192=
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp18A,i,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp18A,sizeof(void*),i))));
# 824
if((void**)_tmp192.curr != (void**)_tmp18A.curr){
_tmp18A=_tmp192;
changed=1;}}}}
# 832
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp193=env->root_place;struct Cyc_CfFlowInfo_Place*_tmp194=_tmp193;void*_tmp195;struct Cyc_List_List*_tmp196;_LLF5: _tmp195=_tmp194->root;_tmp196=_tmp194->fields;_LLF6:;{
int*_tmp439;struct Cyc_List_List*_tmp197=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp439=_cycalloc_atomic(sizeof(*_tmp439)),((_tmp439[0]=fs_prefix,_tmp439)))),_tmp196);
struct Cyc_CfFlowInfo_Place*_tmp43A;struct Cyc_CfFlowInfo_Place*_tmp198=(_tmp43A=_region_malloc((env->pile)->rgn,sizeof(*_tmp43A)),((_tmp43A->root=_tmp195,((_tmp43A->fields=_tmp197,_tmp43A)))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp198,0);};}}{
# 839
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp440;struct Cyc_CfFlowInfo_UnionRInfo _tmp43F;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp43E;return(void*)((_tmp43E=_region_malloc((env->fenv)->r,sizeof(*_tmp43E)),((_tmp43E[0]=((_tmp440.tag=6,((_tmp440.f1=((_tmp43F.is_union=_tmp188,((_tmp43F.fieldnum=_tmp186,_tmp43F)))),((_tmp440.f2=_tmp18A,_tmp440)))))),_tmp43E))));};}}else{goto _LLF2;}}else{_LLF2: _LLF3: {
struct Cyc_Core_Impossible_exn_struct _tmp446;const char*_tmp445;struct Cyc_Core_Impossible_exn_struct*_tmp444;(int)_throw((void*)((_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=((_tmp446.tag=Cyc_Core_Impossible,((_tmp446.f1=((_tmp445="bad assign place",_tag_dyneither(_tmp445,sizeof(char),17))),_tmp446)))),_tmp444)))));}}}_LLED:;};}
# 843
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 854
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1A2=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp1A3=place;void*_tmp1A4;struct Cyc_List_List*_tmp1A5;_LLF8: _tmp1A4=_tmp1A3->root;_tmp1A5=_tmp1A3->fields;_LLF9:;{
struct Cyc_CfFlowInfo_EscPile*_tmp449;struct Cyc_CfFlowInfo_AssignEnv _tmp448;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp448.fenv=fenv,((_tmp448.pile=((_tmp449=_region_malloc(_tmp1A2,sizeof(*_tmp449)),((_tmp449->rgn=_tmp1A2,((_tmp449->places=0,_tmp449)))))),((_tmp448.d=d,((_tmp448.loc=loc,((_tmp448.root_place=place,((_tmp448.all_changed=all_changed,_tmp448)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1A5,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1A4),r);
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1A4,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 870
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 880
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 882
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp45C(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp45B,unsigned int*_tmp45A,void***_tmp458,struct _dyneither_ptr*_tmp1B0,struct _dyneither_ptr*_tmp1AD){for(*_tmp45B=0;*_tmp45B < *_tmp45A;(*_tmp45B)++){(*_tmp458)[*_tmp45B]=(void*)
# 957 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1AD).curr)[(int)*_tmp45B],*((void**)_check_dyneither_subscript(*_tmp1B0,sizeof(void*),(int)*_tmp45B)));}}
# 892
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1A8=(env->fenv)->r;
# 896
{struct _tuple0 _tmp44A;struct _tuple0 _tmp1A9=(_tmp44A.f1=r1,((_tmp44A.f2=r2,_tmp44A)));struct _tuple0 _tmp1AA=_tmp1A9;int _tmp1AB;int _tmp1AC;struct _dyneither_ptr _tmp1AD;int _tmp1AE;int _tmp1AF;struct _dyneither_ptr _tmp1B0;struct Cyc_CfFlowInfo_Place*_tmp1B1;struct Cyc_CfFlowInfo_Place*_tmp1B2;struct Cyc_CfFlowInfo_Place*_tmp1B3;struct Cyc_CfFlowInfo_Place*_tmp1B4;struct Cyc_CfFlowInfo_Place*_tmp1B5;struct Cyc_CfFlowInfo_Place*_tmp1B6;struct Cyc_CfFlowInfo_Place*_tmp1B7;struct Cyc_CfFlowInfo_Place*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;int _tmp1BA;void*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BC;int _tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;int _tmp1C0;void*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;int _tmp1C3;void*_tmp1C4;struct Cyc_Absyn_Vardecl*_tmp1C5;void*_tmp1C6;struct Cyc_Absyn_Vardecl*_tmp1C7;void*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1C9;void*_tmp1CA;struct Cyc_Absyn_Vardecl*_tmp1CB;void*_tmp1CC;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 8){_LLFB: _tmp1C9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1CA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1CB=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1CC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_LLFC:
# 899
 if(_tmp1C9 == _tmp1CB){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp44D;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp44C;return(void*)((_tmp44C=_region_malloc(_tmp1A8,sizeof(*_tmp44C)),((_tmp44C[0]=((_tmp44D.tag=8,((_tmp44D.f1=_tmp1C9,((_tmp44D.f2=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1CA,_tmp1CC),_tmp44D)))))),_tmp44C))));}else{
# 902
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1CA,_tmp1CC);}}else{_LLFD: _tmp1C7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1C8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_LLFE:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C8,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 8){_LLFF: _tmp1C5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1C6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_LL100:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1C6);}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->tag == 7){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 7){_LL101: _tmp1BF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1C0=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1C1=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f3;_tmp1C2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1C3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_tmp1C4=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f3;_LL102:
# 907
 if(_tmp1BF == _tmp1C2){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp450;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp44F;return(void*)((_tmp44F=_region_malloc(_tmp1A8,sizeof(*_tmp44F)),((_tmp44F[0]=((_tmp450.tag=7,((_tmp450.f1=_tmp1BF,((_tmp450.f2=_tmp1C0 > _tmp1C3?_tmp1C0: _tmp1C3,((_tmp450.f3=
Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C1,_tmp1C4),_tmp450)))))))),_tmp44F))));}
{void*_tmp1D1=_tmp1C1;struct Cyc_CfFlowInfo_Place*_tmp1D2;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1D1)->tag == 5){_LL11E: _tmp1D2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1D1)->f1;_LL11F:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1D2);goto _LL11D;}else{_LL120: _LL121:
 goto _LL11D;}_LL11D:;}
# 914
{void*_tmp1D3=_tmp1C4;struct Cyc_CfFlowInfo_Place*_tmp1D4;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1D3)->tag == 5){_LL123: _tmp1D4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1D3)->f1;_LL124:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1D4);goto _LL122;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}
# 918
goto _LLFA;}else{_LL103: _tmp1BC=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1BD=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1BE=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f3;_LL104: {
# 920
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp453;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp452;return(void*)((_tmp452=_region_malloc(_tmp1A8,sizeof(*_tmp452)),((_tmp452[0]=((_tmp453.tag=7,((_tmp453.f1=_tmp1BC,((_tmp453.f2=_tmp1BD,((_tmp453.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1BE,r2),_tmp453)))))))),_tmp452))));}}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 7){_LL105: _tmp1B9=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_tmp1BA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_tmp1BB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f3;_LL106: {
# 922
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp456;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp455;return(void*)((_tmp455=_region_malloc(_tmp1A8,sizeof(*_tmp455)),((_tmp455[0]=((_tmp456.tag=7,((_tmp456.f1=_tmp1B9,((_tmp456.f2=_tmp1BA,((_tmp456.f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1BB),_tmp456)))))))),_tmp455))));}}else{switch(*((int*)_tmp1AA.f1)){case 5: switch(*((int*)_tmp1AA.f2)){case 5: _LL107: _tmp1B7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_tmp1B8=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL108:
# 925
 if(Cyc_CfFlowInfo_place_cmp(_tmp1B7,_tmp1B8)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1B7);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1B8);
goto _LLFA;case 2: _LL109: _tmp1B6=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_LL10A:
# 930
 _tmp1B3=_tmp1B6;goto _LL10C;case 1: _LL10D: _tmp1B5=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_LL10E:
# 936
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B5);{
enum Cyc_CfFlowInfo_InitLevel _tmp1D9=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _tmp1DA=_tmp1D9;if(_tmp1DA == Cyc_CfFlowInfo_AllIL){_LL128: _LL129:
 return(env->fenv)->notzeroall;}else{_LL12A: _LL12B:
 return(env->fenv)->notzerothis;}_LL127:;};default: _LL111: _tmp1B4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1;_LL112:
# 948
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B4);goto _LLFA;}case 2: switch(*((int*)_tmp1AA.f2)){case 5: _LL10B: _tmp1B3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL10C:
# 932
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B3);
return(env->fenv)->notzerothis;case 1: _LL117: _LL118:
# 953
 return(env->fenv)->notzerothis;default: goto _LL11B;}case 1: switch(*((int*)_tmp1AA.f2)){case 5: _LL10F: _tmp1B2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL110:
# 942
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B2);{
enum Cyc_CfFlowInfo_InitLevel _tmp1DB=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _tmp1DC=_tmp1DB;if(_tmp1DC == Cyc_CfFlowInfo_AllIL){_LL12D: _LL12E:
 return(env->fenv)->notzeroall;}else{_LL12F: _LL130:
 return(env->fenv)->notzerothis;}_LL12C:;};case 2: _LL115: _LL116:
# 952
 goto _LL118;default: goto _LL11B;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 5){_LL113: _tmp1B1=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1;_LL114:
# 949
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B1);goto _LLFA;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->tag == 6){_LL119: _tmp1AB=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1).is_union;_tmp1AC=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f1).fieldnum;_tmp1AD=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f1)->f2;_tmp1AE=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1).is_union;_tmp1AF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f1).fieldnum;_tmp1B0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AA.f2)->f2;_LL11A: {
# 956
unsigned int _tmp45B;unsigned int _tmp45A;struct _dyneither_ptr _tmp459;void**_tmp458;unsigned int _tmp457;struct _dyneither_ptr new_d=
(_tmp457=_get_dyneither_size(_tmp1AD,sizeof(void*)),((_tmp458=(void**)_region_malloc(_tmp1A8,_check_times(sizeof(void*),_tmp457)),((_tmp459=_tag_dyneither(_tmp458,sizeof(void*),_tmp457),((((_tmp45A=_tmp457,_tmp45C(& env,& _tmp45B,& _tmp45A,& _tmp458,& _tmp1B0,& _tmp1AD))),_tmp459)))))));
# 961
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1AD,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1AD.curr)[i]){
change=1;break;}}}
# 966
if(!change){
if(!_tmp1AB)return r1;
new_d=_tmp1AD;}else{
# 971
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1AD,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1B0,sizeof(void*),i))){
change=1;break;}}}
# 976
if(!change){
if(!_tmp1AB)return r2;
new_d=_tmp1B0;}}{
# 981
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp462;struct Cyc_CfFlowInfo_UnionRInfo _tmp461;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp460;return(void*)((_tmp460=_region_malloc(_tmp1A8,sizeof(*_tmp460)),((_tmp460[0]=((_tmp462.tag=6,((_tmp462.f1=((_tmp461.is_union=_tmp1AB,((_tmp461.fieldnum=_tmp1AC == _tmp1AF?_tmp1AC: - 1,_tmp461)))),((_tmp462.f2=new_d,_tmp462)))))),_tmp460))));};}}else{goto _LL11B;}}else{_LL11B: _LL11C:
# 983
 goto _LLFA;}}}}}}}_LLFA:;}{
# 985
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp463;struct _tuple0 _tmp1E5=(_tmp463.f1=r1,((_tmp463.f2=r2,_tmp463)));struct _tuple0 _tmp1E6=_tmp1E5;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1E6.f1)->tag == 4){_LL132: _LL133:
 goto _LL135;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1E6.f2)->tag == 4){_LL134: _LL135: {
# 990
struct _tuple21 _tmp464;struct _tuple21 _tmp1E7=(_tmp464.f1=il1,((_tmp464.f2=il2,_tmp464)));struct _tuple21 _tmp1E8=_tmp1E7;if(_tmp1E8.f2 == Cyc_CfFlowInfo_NoneIL){_LL139: _LL13A:
 goto _LL13C;}else{if(_tmp1E8.f1 == Cyc_CfFlowInfo_NoneIL){_LL13B: _LL13C: return(env->fenv)->esc_none;}else{if(_tmp1E8.f2 == Cyc_CfFlowInfo_ThisIL){_LL13D: _LL13E:
 goto _LL140;}else{if(_tmp1E8.f1 == Cyc_CfFlowInfo_ThisIL){_LL13F: _LL140: return(env->fenv)->esc_this;}else{_LL141: _LL142:
 return(env->fenv)->esc_all;}}}}_LL138:;}}else{_LL136: _LL137: {
# 996
struct _tuple21 _tmp465;struct _tuple21 _tmp1EA=(_tmp465.f1=il1,((_tmp465.f2=il2,_tmp465)));struct _tuple21 _tmp1EB=_tmp1EA;if(_tmp1EB.f2 == Cyc_CfFlowInfo_NoneIL){_LL144: _LL145:
 goto _LL147;}else{if(_tmp1EB.f1 == Cyc_CfFlowInfo_NoneIL){_LL146: _LL147: return(env->fenv)->unknown_none;}else{if(_tmp1EB.f2 == Cyc_CfFlowInfo_ThisIL){_LL148: _LL149:
 goto _LL14B;}else{if(_tmp1EB.f1 == Cyc_CfFlowInfo_ThisIL){_LL14A: _LL14B: return(env->fenv)->unknown_this;}else{_LL14C: _LL14D:
 return(env->fenv)->unknown_all;}}}}_LL143:;}}}_LL131:;};};}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 1004
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1007
struct _tuple22 _tmp466;struct _tuple22 _tmp1EE=(_tmp466.f1=f1,((_tmp466.f2=f2,_tmp466)));struct _tuple22 _tmp1EF=_tmp1EE;struct Cyc_Dict_Dict _tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_Dict_Dict _tmp1F2;struct Cyc_List_List*_tmp1F3;if(((_tmp1EF.f1).BottomFL).tag == 1){_LL14F: _LL150:
 return f2;}else{if(((_tmp1EF.f2).BottomFL).tag == 1){_LL151: _LL152:
 return f1;}else{_LL153: _tmp1F0=(((_tmp1EF.f1).ReachableFL).val).f1;_tmp1F1=(((_tmp1EF.f1).ReachableFL).val).f2;_tmp1F2=(((_tmp1EF.f2).ReachableFL).val).f1;_tmp1F3=(((_tmp1EF.f2).ReachableFL).val).f2;_LL154:
# 1013
 if(_tmp1F0.t == _tmp1F2.t  && _tmp1F1 == _tmp1F3)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp1F4=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp469;struct Cyc_CfFlowInfo_JoinEnv _tmp468;struct Cyc_CfFlowInfo_JoinEnv _tmp1F5=(_tmp468.fenv=fenv,((_tmp468.pile=((_tmp469=_region_malloc(_tmp1F4,sizeof(*_tmp469)),((_tmp469->rgn=_tmp1F4,((_tmp469->places=0,_tmp469)))))),((_tmp468.d1=_tmp1F0,((_tmp468.d2=_tmp1F2,_tmp468)))))));
struct Cyc_Dict_Dict _tmp1F6=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp1F5,_tmp1F0,_tmp1F2);
struct Cyc_List_List*_tmp1F7=Cyc_Relations_join_relns(fenv->r,_tmp1F1,_tmp1F3);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp1F5.pile,all_changed,_tmp1F6),_tmp1F7);};}}_LL14E:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1025
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2){
# 1032
struct _tuple13 _tmp1FB=pr1;union Cyc_CfFlowInfo_FlowInfo _tmp1FC;void*_tmp1FD;_LL156: _tmp1FC=_tmp1FB.f1;_tmp1FD=_tmp1FB.f2;_LL157:;{
struct _tuple13 _tmp1FE=pr2;union Cyc_CfFlowInfo_FlowInfo _tmp1FF;void*_tmp200;_LL159: _tmp1FF=_tmp1FE.f1;_tmp200=_tmp1FE.f2;_LL15A:;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp1FC,_tmp1FF);
struct _tuple23 _tmp46A;struct _tuple23 _tmp201=(_tmp46A.f1=_tmp1FC,((_tmp46A.f2=_tmp1FF,((_tmp46A.f3=outflow,_tmp46A)))));struct _tuple23 _tmp202=_tmp201;struct Cyc_Dict_Dict _tmp203;struct Cyc_Dict_Dict _tmp204;struct Cyc_Dict_Dict _tmp205;struct Cyc_List_List*_tmp206;if(((_tmp202.f1).BottomFL).tag == 1){_LL15C: _LL15D: {
struct _tuple13 _tmp46B;return(_tmp46B.f1=outflow,((_tmp46B.f2=_tmp200,_tmp46B)));}}else{if(((_tmp202.f2).BottomFL).tag == 1){_LL15E: _LL15F: {
struct _tuple13 _tmp46C;return(_tmp46C.f1=outflow,((_tmp46C.f2=_tmp1FD,_tmp46C)));}}else{if(((_tmp202.f3).ReachableFL).tag == 2){_LL160: _tmp203=(((_tmp202.f1).ReachableFL).val).f1;_tmp204=(((_tmp202.f2).ReachableFL).val).f1;_tmp205=(((_tmp202.f3).ReachableFL).val).f1;_tmp206=(((_tmp202.f3).ReachableFL).val).f2;_LL161:
# 1040
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1FD,_tmp200)){struct _tuple13 _tmp46D;return(_tmp46D.f1=outflow,((_tmp46D.f2=_tmp200,_tmp46D)));}
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp200,_tmp1FD)){struct _tuple13 _tmp46E;return(_tmp46E.f1=outflow,((_tmp46E.f2=_tmp1FD,_tmp46E)));}{
struct _RegionHandle*_tmp20B=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp471;struct Cyc_CfFlowInfo_JoinEnv _tmp470;struct Cyc_CfFlowInfo_JoinEnv _tmp20C=(_tmp470.fenv=fenv,((_tmp470.pile=((_tmp471=_region_malloc(_tmp20B,sizeof(*_tmp471)),((_tmp471->rgn=_tmp20B,((_tmp471->places=0,_tmp471)))))),((_tmp470.d1=_tmp203,((_tmp470.d2=_tmp204,_tmp470)))))));
void*_tmp20D=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp20C,0,_tmp1FD,_tmp200);
struct _tuple13 _tmp472;return(_tmp472.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp20C.pile,all_changed,_tmp205),_tmp206),((_tmp472.f2=_tmp20D,_tmp472)));};}else{_LL162: _LL163: {
# 1048
struct Cyc_Core_Impossible_exn_struct _tmp478;const char*_tmp477;struct Cyc_Core_Impossible_exn_struct*_tmp476;(int)_throw((void*)((_tmp476=_cycalloc(sizeof(*_tmp476)),((_tmp476[0]=((_tmp478.tag=Cyc_Core_Impossible,((_tmp478.f1=((_tmp477="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp477,sizeof(char),37))),_tmp478)))),_tmp476)))));}}}}_LL15B:;};};}
# 1052
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp482(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp481,unsigned int*_tmp480,void***_tmp47E,struct _dyneither_ptr*_tmp21C,struct _dyneither_ptr*_tmp219){for(*_tmp481=0;*_tmp481 < *_tmp480;(*_tmp481)++){(*_tmp47E)[*_tmp481]=(void*)
# 1093
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp481,((void**)(*_tmp219).curr)[(int)*_tmp481],*((void**)_check_dyneither_subscript(*_tmp21C,sizeof(void*),(int)*_tmp481)));}}
# 1053
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1061
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1066
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1074
struct _tuple0 _tmp479;struct _tuple0 _tmp215=(_tmp479.f1=r1,((_tmp479.f2=r2,_tmp479)));struct _tuple0 _tmp216=_tmp215;int _tmp217;int _tmp218;struct _dyneither_ptr _tmp219;int _tmp21A;int _tmp21B;struct _dyneither_ptr _tmp21C;struct Cyc_Absyn_Vardecl*_tmp21D;void*_tmp21E;struct Cyc_Absyn_Vardecl*_tmp21F;void*_tmp220;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->tag == 7){_LL165: _LL166:
# 1076
 goto _LL168;}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 7){_LL167: _LL168:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 8){_LL169: _tmp21D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f1;_tmp21E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f2;_tmp21F=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f1;_tmp220=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f2;_LL16A:
# 1079
 if(_tmp21D == _tmp21F){
void*_tmp221=Cyc_CfFlowInfo_after_absRval(env,_tmp21E,_tmp220);
if(_tmp221 == _tmp21E)return r1;
if(_tmp221 == _tmp220)return r2;{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp47C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp47B;return(void*)((_tmp47B=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp47B)),((_tmp47B[0]=((_tmp47C.tag=8,((_tmp47C.f1=_tmp21D,((_tmp47C.f2=_tmp221,_tmp47C)))))),_tmp47B))));};}else{
# 1085
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}}else{_LL16D: _LL16E:
# 1088
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 8){_LL16B: _LL16C:
# 1086
 goto _LL16E;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->tag == 6){_LL16F: _tmp217=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f1).is_union;_tmp218=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f1).fieldnum;_tmp219=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f1)->f2;_tmp21A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f1).is_union;_tmp21B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f1).fieldnum;_tmp21C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp216.f2)->f2;_LL170: {
# 1091
unsigned int _tmp481;unsigned int _tmp480;struct _dyneither_ptr _tmp47F;void**_tmp47E;unsigned int _tmp47D;struct _dyneither_ptr new_d=
(_tmp47D=
_get_dyneither_size(_tmp219,sizeof(void*)),((_tmp47E=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp47D)),((_tmp47F=_tag_dyneither(_tmp47E,sizeof(void*),_tmp47D),((((_tmp480=_tmp47D,_tmp482(& env,& _tmp481,& _tmp480,& _tmp47E,& _tmp21C,& _tmp219))),_tmp47F)))))));
# 1098
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp219,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp219.curr)[i]){
change=1;break;}}}
# 1103
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp219,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp21C,sizeof(void*),i))){
change=1;break;}}}
# 1110
if(!change)
return r2;{
# 1114
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp488;struct Cyc_CfFlowInfo_UnionRInfo _tmp487;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp486;return(void*)((_tmp486=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp486)),((_tmp486[0]=((_tmp488.tag=6,((_tmp488.f1=((_tmp487.is_union=_tmp217,((_tmp487.fieldnum=_tmp218 == _tmp21B?_tmp218: - 1,_tmp487)))),((_tmp488.f2=new_d,_tmp488)))))),_tmp486))));};}}else{goto _LL171;}}else{_LL171: _LL172: {
# 1122
struct Cyc_Core_Impossible_exn_struct _tmp48E;const char*_tmp48D;struct Cyc_Core_Impossible_exn_struct*_tmp48C;(int)_throw((void*)((_tmp48C=_cycalloc(sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp48E.tag=Cyc_Core_Impossible,((_tmp48E.f1=((_tmp48D="after_absRval -- non-aggregates!",_tag_dyneither(_tmp48D,sizeof(char),33))),_tmp48E)))),_tmp48C)))));}}}}}}_LL164:;};}
# 1125
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1128
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp22F=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp230=env->changed;
{struct Cyc_List_List*_tmp48F;*env->last_field_cell=((_tmp48F=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp48F)),((_tmp48F->hd=(void*)key,((_tmp48F->tl=0,_tmp48F))))));}
# 1134
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp232=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp22F;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp230;
return _tmp232;};};}
# 1141
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1143
if(r1 == r2)return r1;
{struct Cyc_CfFlowInfo_Place _tmp490;*env->curr_place=((_tmp490.root=root,((_tmp490.fields=0,_tmp490))));}
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1150
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1156
struct _tuple22 _tmp491;struct _tuple22 _tmp234=(_tmp491.f1=f1,((_tmp491.f2=f2,_tmp491)));struct _tuple22 _tmp235=_tmp234;struct Cyc_Dict_Dict _tmp236;struct Cyc_List_List*_tmp237;struct Cyc_Dict_Dict _tmp238;struct Cyc_List_List*_tmp239;if(((_tmp235.f1).BottomFL).tag == 1){_LL174: _LL175:
 goto _LL177;}else{if(((_tmp235.f2).BottomFL).tag == 1){_LL176: _LL177:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL178: _tmp236=(((_tmp235.f1).ReachableFL).val).f1;_tmp237=(((_tmp235.f1).ReachableFL).val).f2;_tmp238=(((_tmp235.f2).ReachableFL).val).f1;_tmp239=(((_tmp235.f2).ReachableFL).val).f2;_LL179:
# 1160
 if(_tmp236.t == _tmp238.t  && _tmp237 == _tmp239)return f1;{
# 1163
struct _RegionHandle*_tmp23A=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp23B=fenv->dummy_place;
struct Cyc_CfFlowInfo_EscPile*_tmp495;struct Cyc_CfFlowInfo_AfterEnv _tmp494;struct Cyc_CfFlowInfo_AfterEnv _tmp23C=(_tmp494.joinenv=(((_tmp494.joinenv).fenv=fenv,(((_tmp494.joinenv).pile=((_tmp495=_region_malloc(_tmp23A,sizeof(*_tmp495)),((_tmp495->rgn=_tmp23A,((_tmp495->places=0,_tmp495)))))),(((_tmp494.joinenv).d1=_tmp236,(((_tmp494.joinenv).d2=_tmp238,_tmp494.joinenv)))))))),((_tmp494.chg1=chg1,((_tmp494.chg2=chg2,((_tmp494.curr_place=_tmp23B,((_tmp494.last_field_cell=& _tmp23B->fields,((_tmp494.changed=Cyc_CfFlowInfo_Neither,_tmp494)))))))))));
# 1169
struct Cyc_Dict_Dict _tmp23D=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp23C,_tmp236,_tmp238);
struct Cyc_List_List*_tmp23E=Cyc_Relations_join_relns(fenv->r,_tmp237,_tmp239);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp23C.joinenv).pile,all_changed,_tmp23D),_tmp23E);};}}_LL173:;}
# 1178
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1182
{struct _tuple0 _tmp496;struct _tuple0 _tmp242=(_tmp496.f1=r1,((_tmp496.f2=r2,_tmp496)));struct _tuple0 _tmp243=_tmp242;int _tmp244;int _tmp245;struct _dyneither_ptr _tmp246;int _tmp247;int _tmp248;struct _dyneither_ptr _tmp249;struct Cyc_CfFlowInfo_Place*_tmp24A;struct Cyc_CfFlowInfo_Place*_tmp24B;struct Cyc_Absyn_Vardecl*_tmp24C;void*_tmp24D;struct Cyc_Absyn_Vardecl*_tmp24E;void*_tmp24F;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 8){_LL17B: _tmp24C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1;_tmp24D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f2;_tmp24E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1;_tmp24F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f2;_LL17C:
# 1184
 return _tmp24C == _tmp24E  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp24D,_tmp24F);}else{_LL17D: _LL17E:
 goto _LL180;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 8){_LL17F: _LL180:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->tag == 5){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 5){_LL181: _tmp24A=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1;_tmp24B=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1;_LL182:
 return Cyc_CfFlowInfo_place_cmp(_tmp24A,_tmp24B)== 0;}else{_LL183: _LL184:
 goto _LL186;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 5){_LL185: _LL186:
 return 0;}else{switch(*((int*)_tmp243.f1)){case 6: switch(*((int*)_tmp243.f2)){case 6: _LL187: _tmp244=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1).is_union;_tmp245=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f1).fieldnum;_tmp246=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->f2;_tmp247=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1).is_union;_tmp248=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f1).fieldnum;_tmp249=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->f2;_LL188:
# 1192
 if(_tmp244  && _tmp245 != _tmp248)return 0;
if((void**)_tmp246.curr == (void**)_tmp249.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp246,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp246.curr)[i],*((void**)_check_dyneither_subscript(_tmp249,sizeof(void*),i))))return 0;}}
return 1;case 2: goto _LL18B;case 0: goto _LL18D;case 1: goto _LL18F;default: goto _LL195;}case 1: switch(*((int*)_tmp243.f2)){case 2: _LL189: _LL18A:
 return 1;case 0: goto _LL18D;case 1: goto _LL18F;default: goto _LL195;}default: switch(*((int*)_tmp243.f2)){case 2: _LL18B: _LL18C:
 return 0;case 0: _LL18D: _LL18E:
 goto _LL190;case 1: _LL18F: _LL190:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp243.f1)->tag == 4){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp243.f2)->tag == 4){_LL191: _LL192:
 goto _LL17A;}else{_LL193: _LL194:
 return 0;}}else{_LL195: _LL196:
 goto _LL17A;}}}}}}}_LL17A:;}{
# 1205
struct _tuple21 _tmp497;struct _tuple21 _tmp251=(_tmp497.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp497.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp497)));struct _tuple21 _tmp252=_tmp251;if(_tmp252.f1 == Cyc_CfFlowInfo_AllIL)switch(_tmp252.f2){case Cyc_CfFlowInfo_AllIL: _LL198: _LL199:
 return 1;case Cyc_CfFlowInfo_NoneIL: goto _LL19A;default: goto _LL19E;}else{if(_tmp252.f2 == Cyc_CfFlowInfo_NoneIL){_LL19A: _LL19B:
 return 1;}else{if(_tmp252.f1 == Cyc_CfFlowInfo_NoneIL){_LL19C: _LL19D:
 return 0;}else{if(_tmp252.f2 == Cyc_CfFlowInfo_ThisIL){_LL19E: _LL19F:
 return 1;}else{_LL1A0: _LL1A1:
 return 0;}}}}_LL197:;};}
# 1220
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1222
struct _tuple22 _tmp498;struct _tuple22 _tmp254=(_tmp498.f1=f1,((_tmp498.f2=f2,_tmp498)));struct _tuple22 _tmp255=_tmp254;struct Cyc_Dict_Dict _tmp256;struct Cyc_List_List*_tmp257;struct Cyc_Dict_Dict _tmp258;struct Cyc_List_List*_tmp259;if(((_tmp255.f1).BottomFL).tag == 1){_LL1A3: _LL1A4:
 return 1;}else{if(((_tmp255.f2).BottomFL).tag == 1){_LL1A5: _LL1A6:
 return 0;}else{_LL1A7: _tmp256=(((_tmp255.f1).ReachableFL).val).f1;_tmp257=(((_tmp255.f1).ReachableFL).val).f2;_tmp258=(((_tmp255.f2).ReachableFL).val).f1;_tmp259=(((_tmp255.f2).ReachableFL).val).f2;_LL1A8:
# 1226
 if(_tmp256.t == _tmp258.t  && _tmp257 == _tmp259)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp256,_tmp258) && 
Cyc_Relations_relns_approx(_tmp257,_tmp259);}}_LL1A2:;}
# 1232
struct _tuple12 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv){
# 1234
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp25B=rv;struct Cyc_Absyn_Vardecl*_tmp25C;void*_tmp25D;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp25B)->tag == 8){_LL1AA: _tmp25C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp25B)->f1;_tmp25D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp25B)->f2;_LL1AB:
# 1239
{struct Cyc_List_List*_tmp499;names=((_tmp499=_region_malloc(rgn,sizeof(*_tmp499)),((_tmp499->hd=_tmp25C,((_tmp499->tl=names,_tmp499))))));}rv=_tmp25D;goto _LL1A9;}else{_LL1AC: _LL1AD:
# 1241
 done=1;goto _LL1A9;}_LL1A9:;}{
# 1244
struct _tuple12 _tmp49A;return(_tmp49A.f1=rv,((_tmp49A.f2=names,_tmp49A)));};}
# 1247
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp260=il;switch(_tmp260){case Cyc_CfFlowInfo_NoneIL: _LL1AF: _LL1B0:
{const char*_tmp49D;void*_tmp49C;(_tmp49C=0,Cyc_fprintf(Cyc_stderr,((_tmp49D="uninitialized",_tag_dyneither(_tmp49D,sizeof(char),14))),_tag_dyneither(_tmp49C,sizeof(void*),0)));}goto _LL1AE;case Cyc_CfFlowInfo_ThisIL: _LL1B1: _LL1B2:
{const char*_tmp4A0;void*_tmp49F;(_tmp49F=0,Cyc_fprintf(Cyc_stderr,((_tmp4A0="this-initialized",_tag_dyneither(_tmp4A0,sizeof(char),17))),_tag_dyneither(_tmp49F,sizeof(void*),0)));}goto _LL1AE;default: _LL1B3: _LL1B4:
{const char*_tmp4A3;void*_tmp4A2;(_tmp4A2=0,Cyc_fprintf(Cyc_stderr,((_tmp4A3="all-initialized",_tag_dyneither(_tmp4A3,sizeof(char),16))),_tag_dyneither(_tmp4A2,sizeof(void*),0)));}goto _LL1AE;}_LL1AE:;}
# 1255
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp267=root;struct Cyc_Absyn_Exp*_tmp268;void*_tmp269;struct Cyc_Absyn_Vardecl*_tmp26A;switch(*((int*)_tmp267)){case 0: _LL1B6: _tmp26A=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp267)->f1;_LL1B7:
# 1258
{const char*_tmp4A7;void*_tmp4A6[1];struct Cyc_String_pa_PrintArg_struct _tmp4A5;(_tmp4A5.tag=0,((_tmp4A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp26A->name)),((_tmp4A6[0]=& _tmp4A5,Cyc_fprintf(Cyc_stderr,((_tmp4A7="Root(%s)",_tag_dyneither(_tmp4A7,sizeof(char),9))),_tag_dyneither(_tmp4A6,sizeof(void*),1)))))));}goto _LL1B5;case 1: _LL1B8: _tmp268=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp267)->f1;_tmp269=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp267)->f2;_LL1B9:
# 1260
{const char*_tmp4AC;void*_tmp4AB[2];struct Cyc_String_pa_PrintArg_struct _tmp4AA;struct Cyc_String_pa_PrintArg_struct _tmp4A9;(_tmp4A9.tag=0,((_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp269)),((_tmp4AA.tag=0,((_tmp4AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp268)),((_tmp4AB[0]=& _tmp4AA,((_tmp4AB[1]=& _tmp4A9,Cyc_fprintf(Cyc_stderr,((_tmp4AC="MallocPt(%s,%s)",_tag_dyneither(_tmp4AC,sizeof(char),16))),_tag_dyneither(_tmp4AB,sizeof(void*),2)))))))))))));}goto _LL1B5;default: _LL1BA: _LL1BB:
# 1263
{const char*_tmp4AF;void*_tmp4AE;(_tmp4AE=0,Cyc_fprintf(Cyc_stderr,((_tmp4AF="InitParam(_,_)",_tag_dyneither(_tmp4AF,sizeof(char),15))),_tag_dyneither(_tmp4AE,sizeof(void*),0)));}goto _LL1B5;}_LL1B5:;}
# 1267
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp274=p->fields;
while(_tmp274 != 0){
{const char*_tmp4B3;void*_tmp4B2[1];struct Cyc_Int_pa_PrintArg_struct _tmp4B1;(_tmp4B1.tag=1,((_tmp4B1.f1=(unsigned long)((int)_tmp274->hd),((_tmp4B2[0]=& _tmp4B1,Cyc_fprintf(Cyc_stderr,((_tmp4B3=".%d",_tag_dyneither(_tmp4B3,sizeof(char),4))),_tag_dyneither(_tmp4B2,sizeof(void*),1)))))));}
_tmp274=_tmp274->tl;}{
# 1274
const char*_tmp4B6;void*_tmp4B5;(_tmp4B5=0,Cyc_fprintf(Cyc_stderr,((_tmp4B6=" ",_tag_dyneither(_tmp4B6,sizeof(char),2))),_tag_dyneither(_tmp4B5,sizeof(void*),0)));};};}
# 1277
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp27A=1;
while(x != 0){
if(!_tmp27A){{const char*_tmp4B9;void*_tmp4B8;(_tmp4B8=0,Cyc_fprintf(Cyc_stderr,((_tmp4B9=", ",_tag_dyneither(_tmp4B9,sizeof(char),3))),_tag_dyneither(_tmp4B8,sizeof(void*),0)));}_tmp27A=0;}
pr(x->hd);
x=x->tl;}{
# 1284
const char*_tmp4BC;void*_tmp4BB;(_tmp4BB=0,Cyc_fprintf(Cyc_stderr,((_tmp4BC="\n",_tag_dyneither(_tmp4BC,sizeof(char),2))),_tag_dyneither(_tmp4BB,sizeof(void*),0)));};}
# 1287
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp27F=_new_region("r");struct _RegionHandle*r=& _tmp27F;_push_region(r);
{struct _handler_cons _tmp280;_push_handler(& _tmp280);{int _tmp282=0;if(setjmp(_tmp280.handler))_tmp282=1;if(!_tmp282){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple14 _tmp283=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
{const char*_tmp4BF;void*_tmp4BE;(_tmp4BE=0,Cyc_fprintf(Cyc_stderr,((_tmp4BF="{ ",_tag_dyneither(_tmp4BF,sizeof(char),3))),_tag_dyneither(_tmp4BE,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp283)){
void*_tmp286=_tmp283.f1;
if(!first){
{const char*_tmp4C2;void*_tmp4C1;(_tmp4C1=0,Cyc_fprintf(Cyc_stderr,((_tmp4C2=", ",_tag_dyneither(_tmp4C2,sizeof(char),3))),_tag_dyneither(_tmp4C1,sizeof(void*),0)));}
first=0;}
# 1300
pr(_tmp286);}{
# 1302
const char*_tmp4C5;void*_tmp4C4;(_tmp4C4=0,Cyc_fprintf(Cyc_stderr,((_tmp4C5="}\n",_tag_dyneither(_tmp4C5,sizeof(char),3))),_tag_dyneither(_tmp4C4,sizeof(void*),0)));};}
# 1290
;_pop_handler();}else{void*_tmp281=(void*)_exn_thrown;void*_tmp28B=_tmp281;void*_tmp28C;_LL1BD: _tmp28C=_tmp28B;_LL1BE:
# 1305
{const char*_tmp4C8;void*_tmp4C7;(_tmp4C7=0,Cyc_fprintf(Cyc_stderr,((_tmp4C8="{ }\n",_tag_dyneither(_tmp4C8,sizeof(char),5))),_tag_dyneither(_tmp4C7,sizeof(void*),0)));}
_npop_handler(0);return;_LL1BC:;}};}
# 1289
;_pop_region(r);}
# 1311
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp28F=rval;struct Cyc_Absyn_Vardecl*_tmp290;void*_tmp291;struct Cyc_Absyn_Exp*_tmp292;int _tmp293;void*_tmp294;int _tmp295;int _tmp296;struct _dyneither_ptr _tmp297;struct Cyc_CfFlowInfo_Place*_tmp298;enum Cyc_CfFlowInfo_InitLevel _tmp299;enum Cyc_CfFlowInfo_InitLevel _tmp29A;switch(*((int*)_tmp28F)){case 0: _LL1C2: _LL1C3:
{const char*_tmp4CB;void*_tmp4CA;(_tmp4CA=0,Cyc_fprintf(Cyc_stderr,((_tmp4CB="Zero",_tag_dyneither(_tmp4CB,sizeof(char),5))),_tag_dyneither(_tmp4CA,sizeof(void*),0)));}goto _LL1C1;case 1: _LL1C4: _LL1C5:
{const char*_tmp4CE;void*_tmp4CD;(_tmp4CD=0,Cyc_fprintf(Cyc_stderr,((_tmp4CE="NotZeroAll",_tag_dyneither(_tmp4CE,sizeof(char),11))),_tag_dyneither(_tmp4CD,sizeof(void*),0)));}goto _LL1C1;case 2: _LL1C6: _LL1C7:
{const char*_tmp4D1;void*_tmp4D0;(_tmp4D0=0,Cyc_fprintf(Cyc_stderr,((_tmp4D1="NotZeroThis",_tag_dyneither(_tmp4D1,sizeof(char),12))),_tag_dyneither(_tmp4D0,sizeof(void*),0)));}goto _LL1C1;case 3: _LL1C8: _tmp29A=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_LL1C9:
{const char*_tmp4D4;void*_tmp4D3;(_tmp4D3=0,Cyc_fprintf(Cyc_stderr,((_tmp4D4="Unknown(",_tag_dyneither(_tmp4D4,sizeof(char),9))),_tag_dyneither(_tmp4D3,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp29A);
{const char*_tmp4D7;void*_tmp4D6;(_tmp4D6=0,Cyc_fprintf(Cyc_stderr,((_tmp4D7=")",_tag_dyneither(_tmp4D7,sizeof(char),2))),_tag_dyneither(_tmp4D6,sizeof(void*),0)));}goto _LL1C1;case 4: _LL1CA: _tmp299=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_LL1CB:
{const char*_tmp4DA;void*_tmp4D9;(_tmp4D9=0,Cyc_fprintf(Cyc_stderr,((_tmp4DA="Esc(",_tag_dyneither(_tmp4DA,sizeof(char),5))),_tag_dyneither(_tmp4D9,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp299);
{const char*_tmp4DD;void*_tmp4DC;(_tmp4DC=0,Cyc_fprintf(Cyc_stderr,((_tmp4DD=")",_tag_dyneither(_tmp4DD,sizeof(char),2))),_tag_dyneither(_tmp4DC,sizeof(void*),0)));}goto _LL1C1;case 5: _LL1CC: _tmp298=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_LL1CD:
{const char*_tmp4E0;void*_tmp4DF;(_tmp4DF=0,Cyc_fprintf(Cyc_stderr,((_tmp4E0="AddrOf(",_tag_dyneither(_tmp4E0,sizeof(char),8))),_tag_dyneither(_tmp4DF,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp298);
{const char*_tmp4E3;void*_tmp4E2;(_tmp4E2=0,Cyc_fprintf(Cyc_stderr,((_tmp4E3=")",_tag_dyneither(_tmp4E3,sizeof(char),2))),_tag_dyneither(_tmp4E2,sizeof(void*),0)));}goto _LL1C1;case 6: _LL1CE: _tmp295=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1).is_union;_tmp296=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1).fieldnum;_tmp297=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f2;_LL1CF:
# 1323
 if(_tmp295){
{const char*_tmp4E6;void*_tmp4E5;(_tmp4E5=0,Cyc_fprintf(Cyc_stderr,((_tmp4E6="AggrUnion{",_tag_dyneither(_tmp4E6,sizeof(char),11))),_tag_dyneither(_tmp4E5,sizeof(void*),0)));}
if(_tmp296 != - 1){
const char*_tmp4EA;void*_tmp4E9[1];struct Cyc_Int_pa_PrintArg_struct _tmp4E8;(_tmp4E8.tag=1,((_tmp4E8.f1=(unsigned long)_tmp296,((_tmp4E9[0]=& _tmp4E8,Cyc_fprintf(Cyc_stderr,((_tmp4EA="tag == %d;",_tag_dyneither(_tmp4EA,sizeof(char),11))),_tag_dyneither(_tmp4E9,sizeof(void*),1)))))));}}else{
# 1328
const char*_tmp4ED;void*_tmp4EC;(_tmp4EC=0,Cyc_fprintf(Cyc_stderr,((_tmp4ED="AggrStruct{",_tag_dyneither(_tmp4ED,sizeof(char),12))),_tag_dyneither(_tmp4EC,sizeof(void*),0)));}
{int i=0;for(0;i < _get_dyneither_size(_tmp297,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp297.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp297,sizeof(void*))){const char*_tmp4F0;void*_tmp4EF;(_tmp4EF=0,Cyc_fprintf(Cyc_stderr,((_tmp4F0=",",_tag_dyneither(_tmp4F0,sizeof(char),2))),_tag_dyneither(_tmp4EF,sizeof(void*),0)));}}}
# 1333
{const char*_tmp4F3;void*_tmp4F2;(_tmp4F2=0,Cyc_fprintf(Cyc_stderr,((_tmp4F3="}",_tag_dyneither(_tmp4F3,sizeof(char),2))),_tag_dyneither(_tmp4F2,sizeof(void*),0)));}
goto _LL1C1;case 7: _LL1D0: _tmp292=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_tmp293=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f2;_tmp294=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f3;_LL1D1:
# 1336
{const char*_tmp4F8;void*_tmp4F7[2];struct Cyc_String_pa_PrintArg_struct _tmp4F6;struct Cyc_Int_pa_PrintArg_struct _tmp4F5;(_tmp4F5.tag=1,((_tmp4F5.f1=(unsigned long)_tmp293,((_tmp4F6.tag=0,((_tmp4F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp292)),((_tmp4F7[0]=& _tmp4F6,((_tmp4F7[1]=& _tmp4F5,Cyc_fprintf(Cyc_stderr,((_tmp4F8="[Consumed(%s,%d,",_tag_dyneither(_tmp4F8,sizeof(char),17))),_tag_dyneither(_tmp4F7,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp294);{const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,Cyc_fprintf(Cyc_stderr,((_tmp4FB=")]",_tag_dyneither(_tmp4FB,sizeof(char),3))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}
goto _LL1C1;default: _LL1D2: _tmp290=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f1;_tmp291=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28F)->f2;_LL1D3:
# 1340
{const char*_tmp4FF;void*_tmp4FE[1];struct Cyc_String_pa_PrintArg_struct _tmp4FD;(_tmp4FD.tag=0,((_tmp4FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp290->name)),((_tmp4FE[0]=& _tmp4FD,Cyc_fprintf(Cyc_stderr,((_tmp4FF="[NamedLocation(%s,",_tag_dyneither(_tmp4FF,sizeof(char),19))),_tag_dyneither(_tmp4FE,sizeof(void*),1)))))));}
Cyc_CfFlowInfo_print_absrval(_tmp291);{const char*_tmp502;void*_tmp501;(_tmp501=0,Cyc_fprintf(Cyc_stderr,((_tmp502=")]",_tag_dyneither(_tmp502,sizeof(char),3))),_tag_dyneither(_tmp501,sizeof(void*),0)));}
goto _LL1C1;}_LL1C1:;}
# 1346
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
{const char*_tmp505;void*_tmp504;(_tmp504=0,Cyc_fprintf(Cyc_stderr,((_tmp505="    ",_tag_dyneither(_tmp505,sizeof(char),5))),_tag_dyneither(_tmp504,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);
{const char*_tmp508;void*_tmp507;(_tmp507=0,Cyc_fprintf(Cyc_stderr,((_tmp508=" --> ",_tag_dyneither(_tmp508,sizeof(char),6))),_tag_dyneither(_tmp507,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp50B;void*_tmp50A;(_tmp50A=0,Cyc_fprintf(Cyc_stderr,((_tmp50B="\n",_tag_dyneither(_tmp50B,sizeof(char),2))),_tag_dyneither(_tmp50A,sizeof(void*),0)));};}
# 1354
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1358
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp2C9=f;struct Cyc_Dict_Dict _tmp2CA;struct Cyc_List_List*_tmp2CB;if((_tmp2C9.BottomFL).tag == 1){_LL1D5: _LL1D6:
{const char*_tmp50E;void*_tmp50D;(_tmp50D=0,Cyc_fprintf(Cyc_stderr,((_tmp50E="  BottomFL\n",_tag_dyneither(_tmp50E,sizeof(char),12))),_tag_dyneither(_tmp50D,sizeof(void*),0)));}goto _LL1D4;}else{_LL1D7: _tmp2CA=((_tmp2C9.ReachableFL).val).f1;_tmp2CB=((_tmp2C9.ReachableFL).val).f2;_LL1D8:
# 1362
{const char*_tmp511;void*_tmp510;(_tmp510=0,Cyc_fprintf(Cyc_stderr,((_tmp511="  ReachableFL:\n",_tag_dyneither(_tmp511,sizeof(char),16))),_tag_dyneither(_tmp510,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp2CA);
{const char*_tmp514;void*_tmp513;(_tmp513=0,Cyc_fprintf(Cyc_stderr,((_tmp514="\n  Relations: ",_tag_dyneither(_tmp514,sizeof(char),15))),_tag_dyneither(_tmp513,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp2CB);
{const char*_tmp517;void*_tmp516;(_tmp516=0,Cyc_fprintf(Cyc_stderr,((_tmp517="\n",_tag_dyneither(_tmp517,sizeof(char),2))),_tag_dyneither(_tmp516,sizeof(void*),0)));}
goto _LL1D4;}_LL1D4:;}
# 1379 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_tmp2D4=Cyc_Tcutil_compress(t);void*_tmp2D5=_tmp2D4;void*_tmp2D6;void*_tmp2D7;void*_tmp2D8;void*_tmp2D9;void*_tmp2DA;void*_tmp2DB;struct Cyc_List_List*_tmp2DC;struct Cyc_List_List*_tmp2DD;void*_tmp2DE;void*_tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_List_List*_tmp2E1;struct Cyc_List_List*_tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_Absyn_Tvar*_tmp2E5;switch(*((int*)_tmp2D5)){case 26: _LL1DA: _LL1DB:
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
# 1392
void*_tmp2E6=rgn;if(((struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp2E6)->tag == 0){_LL215: _LL216:
 return 1;}else{_LL217: _LL218:
 return 0;}_LL214:;}case 22: _LL1F0: _LL1F1: {
# 1397
void*_tmp2E7=rgn;if(((struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp2E7)->tag == 1){_LL21A: _LL21B:
 return 1;}else{_LL21C: _LL21D:
 return 0;}_LL219:;}case 2: _LL1F2: _tmp2E5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D5)->f1;_LL1F3: {
# 1402
void*_tmp2E8=rgn;struct Cyc_Absyn_Tvar*_tmp2E9;switch(*((int*)_tmp2E8)){case 1: _LL21F: _LL220:
 goto _LL222;case 0: _LL221: _LL222:
 return 0;default: _LL223: _tmp2E9=((struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp2E8)->f1;_LL224:
 return Cyc_Absyn_tvar_cmp(_tmp2E5,_tmp2E9)== 0;}_LL21E:;}case 3: _LL1F4: _tmp2E4=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1F5:
# 1415 "cf_flowinfo.cyc"
 _tmp2E3=_tmp2E4;goto _LL1F7;case 17: _LL1F6: _tmp2E3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5)->f2;_LL1F7:
 _tmp2E2=_tmp2E3;goto _LL1F9;case 11: _LL1F8: _tmp2E2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1F9:
 _tmp2E1=_tmp2E2;goto _LL1FB;case 4: _LL1FA: _tmp2E1=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1FB:
 _tmp2E0=_tmp2E1;goto _LL1FD;case 24: _LL1FC: _tmp2E0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D5)->f1;_LL1FD:
# 1420
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp2E0);case 5: _LL1FE: _tmp2DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5)->f1).elt_typ;_tmp2DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5)->f1).ptr_atts).rgn;_LL1FF:
# 1422
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp2DF) || Cyc_CfFlowInfo_contains_region(rgn,_tmp2DE);case 9: _LL200: _LL201:
 return 0;case 10: _LL202: _tmp2DD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D5)->f1;_LL203:
# 1425
 for(0;_tmp2DD != 0;_tmp2DD=_tmp2DD->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple17*)_tmp2DD->hd)).f2))return 1;}
return 0;case 12: _LL204: _tmp2DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5)->f2;_LL205:
# 1429
 for(0;_tmp2DC != 0;_tmp2DC=_tmp2DC->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp2DC->hd)->type))return 1;}
return 0;case 8: _LL206: _tmp2DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5)->f1).elt_type;_LL207:
 _tmp2DA=_tmp2DB;goto _LL209;case 23: _LL208: _tmp2DA=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D5)->f1;_LL209:
 _tmp2D9=_tmp2DA;goto _LL20B;case 25: _LL20A: _tmp2D9=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D5)->f1;_LL20B:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp2D9);case 16: _LL20C: _tmp2D7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D5)->f1;_tmp2D8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D5)->f2;_LL20D:
# 1436
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp2D7) || Cyc_CfFlowInfo_contains_region(rgn,_tmp2D8);case 15: _LL20E: _tmp2D6=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D5)->f1;_LL20F:
# 1439
 return 0;case 18: _LL210: _LL211:
 return 0;default: _LL212: _LL213:
 return 0;}_LL1D9:;}struct _tuple24{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1446
static void Cyc_CfFlowInfo_kill_root(struct _tuple24*env,void*root,void*rval){
struct _tuple24*_tmp2EA=env;struct Cyc_CfFlowInfo_FlowEnv*_tmp2EB;struct Cyc_Dict_Dict*_tmp2EC;void*_tmp2ED;_LL226: _tmp2EB=_tmp2EA->f1;_tmp2EC=(struct Cyc_Dict_Dict*)& _tmp2EA->f2;_tmp2ED=_tmp2EA->f3;_LL227:;{
void*_tmp2EE=root;void*_tmp2EF;struct Cyc_Absyn_Vardecl*_tmp2F0;switch(*((int*)_tmp2EE)){case 0: _LL229: _tmp2F0=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2EE)->f1;_LL22A:
# 1451
 if(Cyc_CfFlowInfo_contains_region(_tmp2ED,_tmp2F0->type))
# 1453
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp2EB,_tmp2F0->type,0,_tmp2EB->unknown_none);
*_tmp2EC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp2EC,root,rval);
goto _LL228;case 1: _LL22B: _tmp2EF=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2EE)->f2;_LL22C:
# 1458
 if(!Cyc_CfFlowInfo_contains_region(_tmp2ED,_tmp2EF))
# 1460
*_tmp2EC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp2EC,root,rval);
goto _LL228;default: _LL22D: _LL22E:
# 1464
 goto _LL228;}_LL228:;};}
# 1469
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1471
void*_tmp2F1=Cyc_Tcutil_compress(rgn);void*_tmp2F2=_tmp2F1;struct Cyc_Absyn_Tvar*_tmp2F3;switch(*((int*)_tmp2F2)){case 2: _LL230: _tmp2F3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2F2)->f1;_LL231: {
# 1473
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp518;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp518.tag=2,((_tmp518.f1=_tmp2F3,_tmp518)));
void*v2=(void*)& v;
struct _tuple24 _tmp519;struct _tuple24 env=(_tmp519.f1=fenv,((_tmp519.f2=fenv->mt_flowdict,((_tmp519.f3=v2,_tmp519)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}case 21: _LL232: _LL233: {
# 1480
struct _tuple24 _tmp51A;struct _tuple24 env=(_tmp51A.f1=fenv,((_tmp51A.f2=fenv->mt_flowdict,((_tmp51A.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp51A)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}case 22: _LL234: _LL235: {
# 1485
struct _tuple24 _tmp51B;struct _tuple24 env=(_tmp51B.f1=fenv,((_tmp51B.f2=fenv->mt_flowdict,((_tmp51B.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp51B)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}default: _LL236: _LL237: {
# 1489
struct Cyc_Core_Impossible_exn_struct _tmp521;const char*_tmp520;struct Cyc_Core_Impossible_exn_struct*_tmp51F;(int)_throw((void*)((_tmp51F=_cycalloc(sizeof(*_tmp51F)),((_tmp51F[0]=((_tmp521.tag=Cyc_Core_Impossible,((_tmp521.f1=((_tmp520="kill_flowdict_region",_tag_dyneither(_tmp520,sizeof(char),21))),_tmp521)))),_tmp51F)))));}}_LL22F:;}
# 1494
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1497
return 0;}
# 1502
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp2FB=f;struct Cyc_Dict_Dict _tmp2FC;struct Cyc_List_List*_tmp2FD;if((_tmp2FB.BottomFL).tag == 1){_LL239: _LL23A:
 return f;}else{_LL23B: _tmp2FC=((_tmp2FB.ReachableFL).val).f1;_tmp2FD=((_tmp2FB.ReachableFL).val).f2;_LL23C: {
# 1506
struct Cyc_Dict_Dict _tmp2FE=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp2FC,rgn);
struct Cyc_List_List*_tmp2FF=Cyc_CfFlowInfo_kill_relns_region(_tmp2FD,rgn);
# 1509
return Cyc_CfFlowInfo_ReachableFL(_tmp2FE,_tmp2FF);}}_LL238:;}
