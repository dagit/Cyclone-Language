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
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
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
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
void Cyc_file_close(struct Cyc___cycFILE*);
# 300
int isspace(int);
# 313
char*getenv(const char*);
# 318
int system(const char*);
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
# 270 "core.h"
void Cyc_Core_rethrow(void*);
# 274
const char*Cyc_Core_get_exn_name(void*);
# 276
const char*Cyc_Core_get_exn_filename();
# 279
int Cyc_Core_get_exn_lineno();
# 288
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 364
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 118
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;
# 63
extern int Cyc_Position_max_errors;
# 65
int Cyc_Position_error_p();struct Cyc_Relations_Reln;
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
# 1210 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1212
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 36
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 31 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 232
extern int Cyc_Tcenv_warn_override;
# 44 "tcutil.h"
void Cyc_Tcutil_flush_warnings();extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 93
extern int Cyc_Tcutil_warn_alias_coerce;
# 96
extern int Cyc_Tcutil_warn_region_coerce;
# 34 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 36
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 40
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
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
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 121
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 128
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 35 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple20*info);
# 76 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 42 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 45
extern int Cyc_ParseErrors_print_state_and_token;
# 48
extern int Cyc_Lex_compile_for_boot_r;
void Cyc_Lex_pos_init();
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 53
int Cyc_Toc_warn_bounds_checks;
int Cyc_Toc_warn_all_null_deref;
# 63
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
# 94
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3AB;struct Cyc_List_List*_tmp3AA;Cyc_ccargs=((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA->hd=((_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB[0]=s,_tmp3AB)))),((_tmp3AA->tl=Cyc_ccargs,_tmp3AA))))));}
# 102
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp3AC;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3AC="vc",_tag_dyneither(_tmp3AC,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp3AD;Cyc_add_ccarg(((_tmp3AD="-DVC_C",_tag_dyneither(_tmp3AD,sizeof(char),7))));};}else{
# 107
const char*_tmp3AE;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3AE="gcc",_tag_dyneither(_tmp3AE,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp3AF;Cyc_add_ccarg(((_tmp3AF="-DGCC_C",_tag_dyneither(_tmp3AF,sizeof(char),8))));};}}}
# 115
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 123
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3B0;Cyc_output_file=((_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0[0]=s,_tmp3B0))));}
# 128
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
Cyc_specified_interface=s;}
# 137
extern char*Cdef_inc_path;
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Ccomp;
extern char*Cversion;static char _tmp7[1]="";
# 144
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s){
Cyc_cpp=s;}
# 149
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3B3;struct Cyc_List_List*_tmp3B2;Cyc_cppargs=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->hd=((_tmp3B3=_cycalloc(sizeof(*_tmp3B3)),((_tmp3B3[0]=s,_tmp3B3)))),((_tmp3B2->tl=Cyc_cppargs,_tmp3B2))))));}
# 154
static void Cyc_add_cpp_and_ccarg(struct _dyneither_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 159
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3B7;void*_tmp3B6[1];struct Cyc_String_pa_PrintArg_struct _tmp3B5;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3B5.tag=0,((_tmp3B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3B6[0]=& _tmp3B5,Cyc_aprintf(((_tmp3B7="-iprefix %s",_tag_dyneither(_tmp3B7,sizeof(char),12))),_tag_dyneither(_tmp3B6,sizeof(void*),1)))))))));}
# 162
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3BB;void*_tmp3BA[1];struct Cyc_String_pa_PrintArg_struct _tmp3B9;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3B9.tag=0,((_tmp3B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3BA[0]=& _tmp3B9,Cyc_aprintf(((_tmp3BB="-iwithprefix %s",_tag_dyneither(_tmp3BB,sizeof(char),16))),_tag_dyneither(_tmp3BA,sizeof(void*),1)))))))));}
# 165
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3BF;void*_tmp3BE[1];struct Cyc_String_pa_PrintArg_struct _tmp3BD;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3BD.tag=0,((_tmp3BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3BE[0]=& _tmp3BD,Cyc_aprintf(((_tmp3BF="-iwithprefixbefore %s",_tag_dyneither(_tmp3BF,sizeof(char),22))),_tag_dyneither(_tmp3BE,sizeof(void*),1)))))))));}
# 168
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3C3;void*_tmp3C2[1];struct Cyc_String_pa_PrintArg_struct _tmp3C1;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C1.tag=0,((_tmp3C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C2[0]=& _tmp3C1,Cyc_aprintf(((_tmp3C3="-isystem %s",_tag_dyneither(_tmp3C3,sizeof(char),12))),_tag_dyneither(_tmp3C2,sizeof(void*),1)))))))));}
# 171
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3C7;void*_tmp3C6[1];struct Cyc_String_pa_PrintArg_struct _tmp3C5;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C5.tag=0,((_tmp3C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C6[0]=& _tmp3C5,Cyc_aprintf(((_tmp3C7="-idirafter %s",_tag_dyneither(_tmp3C7,sizeof(char),14))),_tag_dyneither(_tmp3C6,sizeof(void*),1)))))))));}
# 175
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 178
Cyc_target_arch=s;
{const char*_tmp3C8;Cyc_add_ccarg(((_tmp3C8="-b",_tag_dyneither(_tmp3C8,sizeof(char),3))));}
Cyc_add_ccarg(s);}
# 183
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}
# 188
static void Cyc_print_version(){
{const char*_tmp3CC;void*_tmp3CB[1];struct Cyc_String_pa_PrintArg_struct _tmp3CA;(_tmp3CA.tag=0,((_tmp3CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3CB[0]=& _tmp3CA,Cyc_printf(((_tmp3CC="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3CC,sizeof(char),34))),_tag_dyneither(_tmp3CB,sizeof(void*),1)))))));}
{const char*_tmp3D1;void*_tmp3D0[2];struct Cyc_String_pa_PrintArg_struct _tmp3CF;struct Cyc_String_pa_PrintArg_struct _tmp3CE;(_tmp3CE.tag=0,((_tmp3CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3CF.tag=0,((_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3D0[0]=& _tmp3CF,((_tmp3D0[1]=& _tmp3CE,Cyc_printf(((_tmp3D1="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3D1,sizeof(char),39))),_tag_dyneither(_tmp3D0,sizeof(void*),2)))))))))))));}
{const char*_tmp3D5;void*_tmp3D4[1];struct Cyc_String_pa_PrintArg_struct _tmp3D3;(_tmp3D3.tag=0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3D4[0]=& _tmp3D3,Cyc_printf(((_tmp3D5="Standard library directory: %s\n",_tag_dyneither(_tmp3D5,sizeof(char),32))),_tag_dyneither(_tmp3D4,sizeof(void*),1)))))));}
{const char*_tmp3D9;void*_tmp3D8[1];struct Cyc_String_pa_PrintArg_struct _tmp3D7;(_tmp3D7.tag=0,((_tmp3D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp3D8[0]=& _tmp3D7,Cyc_printf(((_tmp3D9="Standard include directory: %s\n",_tag_dyneither(_tmp3D9,sizeof(char),32))),_tag_dyneither(_tmp3D8,sizeof(void*),1)))))));}
exit(0);}
# 197
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp27=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp27 <= 4)return 0;else{
const char*_tmp3DA;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp27 - 4)),((_tmp3DA=".cyc",_tag_dyneither(_tmp3DA,sizeof(char),5))))== 0;}}
# 203
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp29 <= 2)return;{
struct _dyneither_ptr _tmp2A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp29 - 2);
struct _dyneither_ptr*_tmp3DD;struct Cyc_List_List*_tmp3DC;Cyc_cyclone_exec_path=((_tmp3DC=_cycalloc(sizeof(*_tmp3DC)),((_tmp3DC->hd=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD[0]=_tmp2A,_tmp3DD)))),((_tmp3DC->tl=Cyc_cyclone_exec_path,_tmp3DC))))));};}
# 216
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp3DE;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3DE="-lxml",_tag_dyneither(_tmp3DE,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 222
const char*_tmp3DF;Cyc_add_ccarg(((_tmp3DF="-lxml_a",_tag_dyneither(_tmp3DF,sizeof(char),8))));}}else{
# 224
struct _dyneither_ptr*_tmp3E2;struct Cyc_List_List*_tmp3E1;Cyc_libargs=((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1->hd=((_tmp3E2=_cycalloc(sizeof(*_tmp3E2)),((_tmp3E2[0]=s,_tmp3E2)))),((_tmp3E1->tl=Cyc_libargs,_tmp3E1))))));}}
# 227
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 231
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp3E3;Cyc_add_ccarg(((_tmp3E3="-save-temps",_tag_dyneither(_tmp3E3,sizeof(char),12))));};}
# 236
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp3E4;Cyc_add_cpparg(((_tmp3E4="-M",_tag_dyneither(_tmp3E4,sizeof(char),3))));};}
# 243
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3E5;Cyc_dependencies_target=((_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5[0]=s,_tmp3E5))));}
# 248
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp3E6;Cyc_add_ccarg(((_tmp3E6="-c",_tag_dyneither(_tmp3E6,sizeof(char),3))));};}
# 253
static void Cyc_set_nocppprecomp(){
const char*_tmp3E7;Cyc_add_cpp_and_ccarg(((_tmp3E7="-no-cpp-precomp",_tag_dyneither(_tmp3E7,sizeof(char),16))));}
# 257
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 261
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 264
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 268
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
{const char*_tmp3E8;Cyc_add_ccarg(((_tmp3E8="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3E8,sizeof(char),18))));}
Cyc_set_elim_se();}
# 273
static void Cyc_set_noboundschecks(){
const char*_tmp3E9;Cyc_add_ccarg(((_tmp3E9="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp3E9,sizeof(char),23))));}
# 276
static void Cyc_set_nonullchecks(){
const char*_tmp3EA;Cyc_add_ccarg(((_tmp3EA="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp3EA,sizeof(char),21))));}
# 279
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 285
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp3EB;Cyc_add_ccarg(((_tmp3EB="-DNO_CYC_PREFIX",_tag_dyneither(_tmp3EB,sizeof(char),16))));};}
# 290
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp3EC;Cyc_add_ccarg(((_tmp3EC="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3EC,sizeof(char),21))));}{
const char*_tmp3ED;Cyc_add_cpparg(((_tmp3ED="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3ED,sizeof(char),21))));};}
# 296
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp3EE;Cyc_add_ccarg(((_tmp3EE="-pg",_tag_dyneither(_tmp3EE,sizeof(char),4))));};}
# 301
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp3EF;Cyc_add_ccarg(((_tmp3EF="-S",_tag_dyneither(_tmp3EF,sizeof(char),3))));};}
# 306
static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;
Cyc_Tc_aggressive_warn=1;}
# 316
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 320
typedef enum Cyc_inputtype Cyc_inputtype_t;
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
const char*_tmp3F0;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3F0="cyc",_tag_dyneither(_tmp3F0,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp3F1;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3F1="none",_tag_dyneither(_tmp3F1,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 328
const char*_tmp3F5;void*_tmp3F4[1];struct Cyc_String_pa_PrintArg_struct _tmp3F3;(_tmp3F3.tag=0,((_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3F4[0]=& _tmp3F3,Cyc_fprintf(Cyc_stderr,((_tmp3F5="Input type '%s' not supported\n",_tag_dyneither(_tmp3F5,sizeof(char),31))),_tag_dyneither(_tmp3F4,sizeof(void*),1)))))));}}}
# 332
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp44=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 338
struct _dyneither_ptr _tmp45=Cyc_strlen((struct _dyneither_ptr)_tmp43)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp43,(struct _dyneither_ptr)_tmp44): _tmp44;
return(struct _dyneither_ptr)_tmp45;}
# 344
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 346
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _dyneither_ptr s){
# 349
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
# 353
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 355
{struct _dyneither_ptr*_tmp3F8;struct Cyc_List_List*_tmp3F7;Cyc_cyclone_files=((_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7->hd=((_tmp3F8=_cycalloc(sizeof(*_tmp3F8)),((_tmp3F8[0]=s,_tmp3F8)))),((_tmp3F7->tl=Cyc_cyclone_files,_tmp3F7))))));}{
# 358
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp3F9;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp3F9=".c",_tag_dyneither(_tmp3F9,sizeof(char),3))));
Cyc_add_ccarg((struct _dyneither_ptr)_tmp49);};}else{
# 362
Cyc_add_ccarg(s);}}}
# 367
static int Cyc_assume_gcc_flag=1;static char _tmp4B[3]="-s";static char _tmp4C[3]="-O";static char _tmp4D[4]="-O0";static char _tmp4E[4]="-O2";static char _tmp4F[4]="-O3";static char _tmp50[21]="-fomit-frame-pointer";static char _tmp51[13]="-fno-builtin";static char _tmp52[3]="-g";static char _tmp53[4]="-pg";static char _tmp54[3]="-p";static char _tmp55[8]="-static";
static int Cyc_add_other_flag(struct _dyneither_ptr s){
static struct {struct _dyneither_ptr flag;int optargs;}known_gcc_flags[11]={{.flag={_tmp4B,_tmp4B,_tmp4B + 3},.optargs=0},{.flag={_tmp4C,_tmp4C,_tmp4C + 3},.optargs=0},{.flag={_tmp4D,_tmp4D,_tmp4D + 4},.optargs=0},{.flag={_tmp4E,_tmp4E,_tmp4E + 4},.optargs=0},{.flag={_tmp4F,_tmp4F,_tmp4F + 4},.optargs=0},{.flag={_tmp50,_tmp50,_tmp50 + 21},.optargs=0},{.flag={_tmp51,_tmp51,_tmp51 + 13},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 3},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 4},.optargs=0},{.flag={_tmp54,_tmp54,_tmp54 + 3},.optargs=0},{.flag={_tmp55,_tmp55,_tmp55 + 8},.optargs=0}};
# 382
if(Cyc_assume_gcc_flag)
Cyc_add_ccarg(s);else{
# 385
{int i=0;for(0;i < 11;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)(known_gcc_flags[_check_known_subscript_notnull(11,i)]).flag,(struct _dyneither_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(known_gcc_flags[_check_known_subscript_notnull(11,i)]).optargs;
break;}}}
# 392
return 0;}
# 394
return 1;}
# 397
static void Cyc_noassume_gcc_flag(){
Cyc_assume_gcc_flag=0;}
# 401
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple21{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 412
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp56=0;
int c;
int i;
char strname[256];
char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),"r");
# 420
if(spec_file == 0){
{const char*_tmp3FD;void*_tmp3FC[1];struct Cyc_String_pa_PrintArg_struct _tmp3FB;(_tmp3FB.tag=0,((_tmp3FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3FC[0]=& _tmp3FB,Cyc_fprintf(Cyc_stderr,((_tmp3FD="Error opening spec file %s\n",_tag_dyneither(_tmp3FD,sizeof(char),28))),_tag_dyneither(_tmp3FC,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 426
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp402;void*_tmp401[2];struct Cyc_String_pa_PrintArg_struct _tmp400;struct Cyc_Int_pa_PrintArg_struct _tmp3FF;(_tmp3FF.tag=1,((_tmp3FF.f1=(unsigned long)c,((_tmp400.tag=0,((_tmp400.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp401[0]=& _tmp400,((_tmp401[1]=& _tmp3FF,Cyc_fprintf(Cyc_stderr,((_tmp402="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp402,sizeof(char),55))),_tag_dyneither(_tmp401,sizeof(void*),2)))))))))))));}
# 435
Cyc_fflush(Cyc_stderr);}
# 437
goto CLEANUP_AND_RETURN;}
# 439
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == - 1){
{const char*_tmp406;void*_tmp405[1];struct Cyc_String_pa_PrintArg_struct _tmp404;(_tmp404.tag=0,((_tmp404.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp405[0]=& _tmp404,Cyc_fprintf(Cyc_stderr,((_tmp406="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp406,sizeof(char),44))),_tag_dyneither(_tmp405,sizeof(void*),1)))))));}
# 447
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 450
if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';
break;}
# 454
strname[_check_known_subscript_notnull(256,i)]=(char)c;
++ i;
if(i >= 256){
{const char*_tmp40A;void*_tmp409[1];struct Cyc_String_pa_PrintArg_struct _tmp408;(_tmp408.tag=0,((_tmp408.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp409[0]=& _tmp408,Cyc_fprintf(Cyc_stderr,((_tmp40A="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp40A,sizeof(char),50))),_tag_dyneither(_tmp409,sizeof(void*),1)))))));}
# 460
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 464
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
break;}
# 469
if(c == '*'){
# 471
{struct _tuple21*_tmp417;struct _dyneither_ptr*_tmp416;const char*_tmp415;struct _dyneither_ptr*_tmp414;struct Cyc_List_List*_tmp413;_tmp56=((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->hd=((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417->f1=((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp416)))),((_tmp417->f2=(
(_tmp414=_cycalloc(sizeof(*_tmp414)),((_tmp414[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp415="",_tag_dyneither(_tmp415,sizeof(char),1)))),_tmp414)))),_tmp417)))))),((_tmp413->tl=_tmp56,_tmp413))))));}
goto JUST_AFTER_STAR;}
# 476
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;
if(i >= 4096){
{const char*_tmp41C;void*_tmp41B[2];struct Cyc_String_pa_PrintArg_struct _tmp41A;struct Cyc_String_pa_PrintArg_struct _tmp419;(_tmp419.tag=0,((_tmp419.f1=(struct _dyneither_ptr)
# 486
_tag_dyneither(strname,sizeof(char),256),((_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp41B[0]=& _tmp41A,((_tmp41B[1]=& _tmp419,Cyc_fprintf(Cyc_stderr,((_tmp41C="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp41C,sizeof(char),50))),_tag_dyneither(_tmp41B,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 492
{struct _tuple21*_tmp425;struct _dyneither_ptr*_tmp424;struct _dyneither_ptr*_tmp423;struct Cyc_List_List*_tmp422;_tmp56=((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422->hd=((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425->f1=((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp424)))),((_tmp425->f2=(
(_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp423)))),_tmp425)))))),((_tmp422->tl=_tmp56,_tmp422))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 497
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp56;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp42F(struct _dyneither_ptr*_tmp77,unsigned int*_tmp42E,unsigned int*_tmp42D,struct _dyneither_ptr**_tmp42B){for(*_tmp42E=0;*_tmp42E < *_tmp42D;(*_tmp42E)++){(*_tmp42B)[*_tmp42E]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)(*_tmp77).curr)[(int)*_tmp42E];}}
# 503
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
unsigned long _tmp71=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp72=0;
char buf[4096];
int i=0;
int j=0;
if(_tmp71 > 4096)goto DONE;
while(1){
# 513
while(1){
if(i >= _tmp71)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 519
j=0;
# 524
while(1){
if(i >= _tmp71)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 530
++ i;
if(i >= _tmp71)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}else{
# 537
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}
# 540
++ i;}
# 542
if(j < 4096)
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 545
struct _dyneither_ptr*_tmp428;struct Cyc_List_List*_tmp427;_tmp72=((_tmp427=_cycalloc(sizeof(*_tmp427)),((_tmp427->hd=((_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp428)))),((_tmp427->tl=_tmp72,_tmp427))))));};}
# 547
DONE:
 _tmp72=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp72);
{struct Cyc_List_List*_tmp429;_tmp72=((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp429->tl=_tmp72,_tmp429))))));}{
struct _dyneither_ptr _tmp77=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp72);
unsigned int _tmp42E;unsigned int _tmp42D;struct _dyneither_ptr _tmp42C;struct _dyneither_ptr*_tmp42B;unsigned int _tmp42A;struct _dyneither_ptr _tmp78=(_tmp42A=_get_dyneither_size(_tmp77,sizeof(struct _dyneither_ptr*)),((_tmp42B=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp42A)),((_tmp42C=_tag_dyneither(_tmp42B,sizeof(struct _dyneither_ptr),_tmp42A),((((_tmp42D=_tmp42A,_tmp42F(& _tmp77,& _tmp42E,& _tmp42D,& _tmp42B))),_tmp42C)))))));
return _tmp78;};};}
# 557
int Cyc_compile_failure=0;
# 559
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp7D;_push_handler(& _tmp7D);{int _tmp7F=0;if(setjmp(_tmp7D.handler))_tmp7F=1;if(!_tmp7F){{struct Cyc___cycFILE*_tmp80=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp80;};_pop_handler();}else{void*_tmp7E=(void*)_exn_thrown;void*_tmp82=_tmp7E;void*_tmp83;_LL1:;_LL2:
# 563
 Cyc_compile_failure=1;
{const char*_tmp434;void*_tmp433[2];struct Cyc_String_pa_PrintArg_struct _tmp432;struct Cyc_String_pa_PrintArg_struct _tmp431;(_tmp431.tag=0,((_tmp431.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp432.tag=0,((_tmp432.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp433[0]=& _tmp432,((_tmp433[1]=& _tmp431,Cyc_fprintf(Cyc_stderr,((_tmp434="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp434,sizeof(char),29))),_tag_dyneither(_tmp433,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL3: _tmp83=_tmp82;_LL4:(void)_rethrow(_tmp83);_LL0:;}};}
# 570
void CYCALLOCPROFILE_mark(const char*s);
# 572
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 576
const char*_tmp435;struct _dyneither_ptr explain_string=(_tmp435="",_tag_dyneither(_tmp435,sizeof(char),1));
struct Cyc_List_List*_tmp88=0;
# 579
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 581
{struct _handler_cons _tmp89;_push_handler(& _tmp89);{int _tmp8B=0;if(setjmp(_tmp89.handler))_tmp8B=1;if(!_tmp8B){_tmp88=f(env,tds);;_pop_handler();}else{void*_tmp8A=(void*)_exn_thrown;void*_tmp8D=_tmp8A;void*_tmp8E;void*_tmp8F;_LL6: _tmp8E=_tmp8D;_LL7:
# 584
{const char*_tmp439;void*_tmp438[1];struct Cyc_String_pa_PrintArg_struct _tmp437;(_tmp437.tag=0,((_tmp437.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp438[0]=& _tmp437,Cyc_fprintf(Cyc_stderr,((_tmp439="COMPILER STAGE %s\n",_tag_dyneither(_tmp439,sizeof(char),19))),_tag_dyneither(_tmp438,sizeof(void*),1)))))));}
Cyc_Core_rethrow(_tmp8E);_LL8: _tmp8F=_tmp8D;_LL9:(void)_rethrow(_tmp8F);_LL5:;}};}
# 587
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
{const char*_tmp43C;void*_tmp43B;(_tmp43B=0,Cyc_fprintf(Cyc_stderr,((_tmp43C="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp43C,sizeof(char),22))),_tag_dyneither(_tmp43B,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
return _tmp88;}else{
# 594
if(Cyc_v_r){
{const char*_tmp440;void*_tmp43F[1];struct Cyc_String_pa_PrintArg_struct _tmp43E;(_tmp43E.tag=0,((_tmp43E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp43F[0]=& _tmp43E,Cyc_fprintf(Cyc_stderr,((_tmp440="%s completed.\n",_tag_dyneither(_tmp440,sizeof(char),15))),_tag_dyneither(_tmp43F,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return _tmp88;}}
# 599
return _tmp88;}
# 602
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp99;_push_handler(& _tmp99);{int _tmp9B=0;if(setjmp(_tmp99.handler))_tmp9B=1;if(!_tmp9B){
ans=Cyc_Parse_parse_file(f);;_pop_handler();}else{void*_tmp9A=(void*)_exn_thrown;void*_tmp9D=_tmp9A;void*_tmp9F;void*_tmpA0;_LLB: {struct Cyc_Position_Exit_exn_struct*_tmp9E=(struct Cyc_Position_Exit_exn_struct*)_tmp9D;if(_tmp9E->tag != Cyc_Position_Exit)goto _LLD;}_LLC:
# 609
 Cyc_compile_failure=1;goto _LLA;_LLD: _tmp9F=_tmp9D;_LLE:
 Cyc_Core_rethrow(_tmp9F);_LLF: _tmpA0=_tmp9D;_LL10:(void)_rethrow(_tmpA0);_LLA:;}};}
# 612
Cyc_Lex_lex_init(1);
return ans;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 616
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 618
struct _tuple22 _tmpA1=*tcenv;struct _RegionHandle*_tmpA3;struct Cyc_Tcenv_Tenv*_tmpA4;struct _tuple22 _tmpA2=_tmpA1;_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;
Cyc_Tc_tc(_tmpA3,_tmpA4,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmpA4,tds);
return tds;}
# 625
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 632
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct Cyc_Tcenv_Tenv*_tmpA6;struct Cyc___cycFILE*_tmpA7;struct Cyc___cycFILE*_tmpA8;struct _tuple23*_tmpA5=params;_tmpA6=_tmpA5->f1;_tmpA7=_tmpA5->f2;_tmpA8=_tmpA5->f3;{
struct Cyc_Interface_I*_tmpA9=Cyc_Interface_extract(_tmpA6->ae);
if(_tmpA7 == 0)
Cyc_Interface_save(_tmpA9,_tmpA8);else{
# 638
struct Cyc_Interface_I*_tmpAA=Cyc_Interface_parse(_tmpA7);
const char*_tmp445;const char*_tmp444;struct _tuple20*_tmp443;if(!Cyc_Interface_is_subinterface(_tmpAA,_tmpA9,((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->f1=((_tmp444="written interface",_tag_dyneither(_tmp444,sizeof(char),18))),((_tmp443->f2=((_tmp445="maximal interface",_tag_dyneither(_tmp445,sizeof(char),18))),_tmp443))))))))
Cyc_compile_failure=1;else{
# 642
Cyc_Interface_save(_tmpAA,_tmpA8);}}
# 644
return tds;};}
# 647
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
return Cyc_Toc_toc(tds);}
# 650
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 655
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 657
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 659
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 664
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 667
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{const char*_tmp448;void*_tmp447;(_tmp447=0,Cyc_fprintf(Cyc_stderr,((_tmp448="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp448,sizeof(char),53))),_tag_dyneither(_tmp447,sizeof(void*),0)));}
return 1;}{
# 671
const char*_tmp44A;const char*_tmp449;struct Cyc___cycFILE*_tmpB0=Cyc_try_file_open(file,((_tmp449="r",_tag_dyneither(_tmp449,sizeof(char),2))),((_tmp44A="internal compiler file",_tag_dyneither(_tmp44A,sizeof(char),23))));
if(_tmpB0 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpB7=1024;unsigned int i;for(i=0;i < _tmpB7;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpB0);
if(n_read != 1024  && !Cyc_feof(_tmpB0)){
Cyc_compile_failure=1;
{const char*_tmp44E;void*_tmp44D[1];struct Cyc_String_pa_PrintArg_struct _tmp44C;(_tmp44C.tag=0,((_tmp44C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp44D[0]=& _tmp44C,Cyc_fprintf(Cyc_stderr,((_tmp44E="\nError: problem copying %s\n",_tag_dyneither(_tmp44E,sizeof(char),28))),_tag_dyneither(_tmp44D,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 684
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp452;void*_tmp451[1];struct Cyc_String_pa_PrintArg_struct _tmp450;(_tmp450.tag=0,((_tmp450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp451[0]=& _tmp450,Cyc_fprintf(Cyc_stderr,((_tmp452="\nError: problem copying %s\n",_tag_dyneither(_tmp452,sizeof(char),28))),_tag_dyneither(_tmp451,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 692
Cyc_fclose(_tmpB0);
return 0;};};}
# 697
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpBA=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpBA.expand_typedefs=!Cyc_noexpand_r;
_tmpBA.to_VC=Cyc_Cyclone_tovc_r;
_tmpBA.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpBA.generate_line_directives=Cyc_generate_line_directives_r;
_tmpBA.print_full_evars=Cyc_print_full_evars_r;
_tmpBA.print_all_tvars=Cyc_print_all_tvars_r;
_tmpBA.print_all_kinds=Cyc_print_all_kinds_r;
_tmpBA.print_all_effects=Cyc_print_all_effects_r;
# 708
if(Cyc_inline_functions_r){
const char*_tmp455;void*_tmp454;(_tmp454=0,Cyc_fprintf(out_file,((_tmp455="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp455,sizeof(char),27))),_tag_dyneither(_tmp454,sizeof(void*),0)));}
# 712
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r){
const char*_tmp458;void*_tmp457;(_tmp457=0,Cyc_fprintf(out_file,((_tmp458="#include <setjmp.h>\n",_tag_dyneither(_tmp458,sizeof(char),21))),_tag_dyneither(_tmp457,sizeof(void*),0)));}else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 718
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 720
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpBA);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 724
Cyc_Absyndump_set_params(& _tmpBA);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 727
Cyc_fflush(out_file);
return tds;}
# 733
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 741
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpBF=0;
unsigned long _tmpC0=Cyc_strlen((struct _dyneither_ptr)s);
# 746
while(_tmpC0 > 0){
struct _dyneither_ptr _tmpC1=Cyc_strchr(s,c);
if((char*)_tmpC1.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp45B;struct Cyc_List_List*_tmp45A;_tmpBF=((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((_tmp45A->hd=((_tmp45B=_cycalloc(sizeof(*_tmp45B)),((_tmp45B[0]=s,_tmp45B)))),((_tmp45A->tl=_tmpBF,_tmp45A))))));}
break;}else{
# 753
{struct _dyneither_ptr*_tmp45E;struct Cyc_List_List*_tmp45D;_tmpBF=((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D->hd=((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpC1).curr - s.curr)/ sizeof(char))),_tmp45E)))),((_tmp45D->tl=_tmpBF,_tmp45D))))));}
_tmpC0 -=(((struct _dyneither_ptr)_tmpC1).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpC1,sizeof(char),1);}}
# 758
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBF);};}
# 762
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpC6;_push_handler(& _tmpC6);{int _tmpC8=0;if(setjmp(_tmpC6.handler))_tmpC8=1;if(!_tmpC8){{const char*_tmp45F;f=Cyc_file_open(file,((_tmp45F="r",_tag_dyneither(_tmp45F,sizeof(char),2))));};_pop_handler();}else{void*_tmpC7=(void*)_exn_thrown;void*_tmpCB=_tmpC7;void*_tmpCC;_LL12:;_LL13: goto _LL11;_LL14: _tmpCC=_tmpCB;_LL15:(void)_rethrow(_tmpCC);_LL11:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 770
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp460;struct _dyneither_ptr tmp=(_tmp460="",_tag_dyneither(_tmp460,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpCD=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpCD.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCD)== 0)continue;
{const char*_tmp461;_tmpCD=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCD,((_tmp461=":",_tag_dyneither(_tmp461,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCD,(struct _dyneither_ptr)tmp);}
# 778
return tmp;}
# 783
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpD0=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpD0.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpD0)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpD0,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp462;return(_tmp462=_cycalloc(sizeof(*_tmp462)),((_tmp462[0]=s,_tmp462)));}};}
# 791
return 0;}
# 794
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpD2=Cyc_find(dirs,file);
if(_tmpD2 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp467;void*_tmp466[2];struct Cyc_String_pa_PrintArg_struct _tmp465;struct Cyc_String_pa_PrintArg_struct _tmp464;(_tmp464.tag=0,((_tmp464.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp465.tag=0,((_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp466[0]=& _tmp465,((_tmp466[1]=& _tmp464,Cyc_fprintf(Cyc_stderr,((_tmp467="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp467,sizeof(char),56))),_tag_dyneither(_tmp466,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp477;const char*_tmp476;void*_tmp475[2];struct Cyc_String_pa_PrintArg_struct _tmp474;struct Cyc_String_pa_PrintArg_struct _tmp473;struct Cyc_Core_Failure_exn_struct*_tmp472;(int)_throw((void*)((_tmp472=_cycalloc(sizeof(*_tmp472)),((_tmp472[0]=((_tmp477.tag=Cyc_Core_Failure,((_tmp477.f1=(struct _dyneither_ptr)((_tmp473.tag=0,((_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp474.tag=0,((_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp475[0]=& _tmp474,((_tmp475[1]=& _tmp473,Cyc_aprintf(((_tmp476="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp476,sizeof(char),56))),_tag_dyneither(_tmp475,sizeof(void*),2)))))))))))))),_tmp477)))),_tmp472)))));};}
# 806
return*_tmpD2;}
# 811
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 813
struct Cyc_List_List*_tmpDD=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp47A;struct Cyc_List_List*_tmp479;_tmpDD=((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479->hd=((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp47A)))),((_tmp479->tl=_tmpDD,_tmp479))))));}
# 817
_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD);
return _tmpDD;}
# 823
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 825
struct Cyc_List_List*_tmpE0=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp47B;_tmpE0=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp47B->tl=_tmpE0,_tmp47B))))));}{
struct _dyneither_ptr*_tmp47E;struct Cyc_List_List*_tmp47D;_tmpE0=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->hd=((_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp47E)))),((_tmp47D->tl=_tmpE0,_tmp47D))))));};}
# 830
_tmpE0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE0);
return _tmpE0;}
# 835
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL16:
 goto _LL17;case '"': _LL17:
 goto _LL18;case ';': _LL18:
 goto _LL19;case '&': _LL19:
 goto _LL1A;case '(': _LL1A:
 goto _LL1B;case ')': _LL1B:
 goto _LL1C;case '|': _LL1C:
 goto _LL1D;case '^': _LL1D:
 goto _LL1E;case '<': _LL1E:
 goto _LL1F;case '>': _LL1F:
 goto _LL20;case '\n': _LL20:
# 850
 goto _LL21;case '\t': _LL21:
 return 1;default: _LL22:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp487(unsigned int*_tmp486,unsigned int*_tmp485,char**_tmp483){for(*_tmp486=0;*_tmp486 < *_tmp485;(*_tmp486)++){(*_tmp483)[*_tmp486]=(char)'\000';}}
# 857
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpE4=Cyc_strlen((struct _dyneither_ptr)s);
# 861
int _tmpE5=0;
int _tmpE6=0;
{int i=0;for(0;i < _tmpE4;++ i){
char _tmpE7=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpE7 == '\'')++ _tmpE5;else{
if(Cyc_is_other_special(_tmpE7))++ _tmpE6;}}}
# 870
if(_tmpE5 == 0  && _tmpE6 == 0)
return s;
# 874
if(_tmpE5 == 0){
struct _dyneither_ptr*_tmp481;struct _dyneither_ptr*_tmp480[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp480[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp480[1]=((_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481[0]=(struct _dyneither_ptr)s,_tmp481)))),((_tmp480[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp480,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 878
unsigned long _tmpEC=(_tmpE4 + _tmpE5)+ _tmpE6;
unsigned int _tmp486;unsigned int _tmp485;struct _dyneither_ptr _tmp484;char*_tmp483;unsigned int _tmp482;struct _dyneither_ptr s2=(_tmp482=_tmpEC + 1,((_tmp483=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp482 + 1)),((_tmp484=_tag_dyneither(_tmp483,sizeof(char),_tmp482 + 1),((((_tmp485=_tmp482,((_tmp487(& _tmp486,& _tmp485,& _tmp483),_tmp483[_tmp485]=(char)0)))),_tmp484)))))));
int _tmpED=0;
int _tmpEE=0;
for(0;_tmpED < _tmpE4;++ _tmpED){
char _tmpEF=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpED));
if(_tmpEF == '\''  || Cyc_is_other_special(_tmpEF)){
char _tmp48A;char _tmp489;struct _dyneither_ptr _tmp488;(_tmp488=_dyneither_ptr_plus(s2,sizeof(char),_tmpEE ++),((_tmp489=*((char*)_check_dyneither_subscript(_tmp488,sizeof(char),0)),((_tmp48A='\\',((_get_dyneither_size(_tmp488,sizeof(char))== 1  && (_tmp489 == '\000'  && _tmp48A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp488.curr)=_tmp48A)))))));}{
char _tmp48D;char _tmp48C;struct _dyneither_ptr _tmp48B;(_tmp48B=_dyneither_ptr_plus(s2,sizeof(char),_tmpEE ++),((_tmp48C=*((char*)_check_dyneither_subscript(_tmp48B,sizeof(char),0)),((_tmp48D=_tmpEF,((_get_dyneither_size(_tmp48B,sizeof(char))== 1  && (_tmp48C == '\000'  && _tmp48D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp48B.curr)=_tmp48D)))))));};}
# 888
return(struct _dyneither_ptr)s2;};}
# 890
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp48E;return(_tmp48E=_cycalloc(sizeof(*_tmp48E)),((_tmp48E[0]=Cyc_sh_escape_string(*sp),_tmp48E)));}
# 894
static void Cyc_process_file(struct _dyneither_ptr filename){
# 896
struct _dyneither_ptr _tmpFB=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp48F;struct _dyneither_ptr _tmpFC=Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp48F=".cyp",_tag_dyneither(_tmp48F,sizeof(char),5))));
const char*_tmp490;struct _dyneither_ptr _tmpFD=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp490=".cyci",_tag_dyneither(_tmp490,sizeof(char),6))));
const char*_tmp491;struct _dyneither_ptr _tmpFE=Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp491=".cycio",_tag_dyneither(_tmp491,sizeof(char),7))));
const char*_tmp492;struct _dyneither_ptr _tmpFF=Cyc_strconcat((struct _dyneither_ptr)_tmpFB,((_tmp492=".c",_tag_dyneither(_tmp492,sizeof(char),3))));
# 903
if(Cyc_v_r){const char*_tmp496;void*_tmp495[1];struct Cyc_String_pa_PrintArg_struct _tmp494;(_tmp494.tag=0,((_tmp494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp495[0]=& _tmp494,Cyc_fprintf(Cyc_stderr,((_tmp496="Compiling %s\n",_tag_dyneither(_tmp496,sizeof(char),14))),_tag_dyneither(_tmp495,sizeof(void*),1)))))));}{
# 906
const char*_tmp498;const char*_tmp497;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp497="r",_tag_dyneither(_tmp497,sizeof(char),2))),((_tmp498="input file",_tag_dyneither(_tmp498,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 913
const char*_tmp49F;struct _dyneither_ptr*_tmp49E;const char*_tmp49D;struct Cyc_List_List*_tmp49C;struct _dyneither_ptr _tmp103=
Cyc_str_sepstr(((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C->hd=((_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E[0]=(struct _dyneither_ptr)((_tmp49D="",_tag_dyneither(_tmp49D,sizeof(char),1))),_tmp49E)))),((_tmp49C->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp49C)))))),(
(_tmp49F=" ",_tag_dyneither(_tmp49F,sizeof(char),2))));
# 923
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmp104=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp4A0;_tmp104=Cyc_add_subdir(_tmp104,((_tmp4A0="include",_tag_dyneither(_tmp4A0,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp4A3;struct Cyc_List_List*_tmp4A2;_tmp104=((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2->hd=((_tmp4A3=_cycalloc(sizeof(*_tmp4A3)),((_tmp4A3[0]=def_inc_path,_tmp4A3)))),((_tmp4A2->tl=_tmp104,_tmp4A2))))));}{
# 929
char*_tmp108=getenv("CYCLONE_INCLUDE_PATH");
if(_tmp108 != 0){
char*_tmp4A4;_tmp104=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4A4=_tmp108,_tag_dyneither(_tmp4A4,sizeof(char),_get_zero_arr_size_char((void*)_tmp4A4,1)))),':'),_tmp104);}{
# 934
const char*_tmp4AB;struct _dyneither_ptr*_tmp4AA;const char*_tmp4A9;struct Cyc_List_List*_tmp4A8;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8->hd=((_tmp4AA=_cycalloc(sizeof(*_tmp4AA)),((_tmp4AA[0]=(struct _dyneither_ptr)((_tmp4A9="",_tag_dyneither(_tmp4A9,sizeof(char),1))),_tmp4AA)))),((_tmp4A8->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmp104),_tmp4A8)))))),(
(_tmp4AB=" -I",_tag_dyneither(_tmp4AB,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp4AF;void*_tmp4AE[1];struct Cyc_String_pa_PrintArg_struct _tmp4AD;ofile_string=(struct _dyneither_ptr)((_tmp4AD.tag=0,((_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4AE[0]=& _tmp4AD,Cyc_aprintf(((_tmp4AF=" > %s",_tag_dyneither(_tmp4AF,sizeof(char),6))),_tag_dyneither(_tmp4AE,sizeof(void*),1))))))));}else{
# 943
const char*_tmp4B0;ofile_string=((_tmp4B0="",_tag_dyneither(_tmp4B0,sizeof(char),1)));}}else{
# 945
const char*_tmp4B4;void*_tmp4B3[1];struct Cyc_String_pa_PrintArg_struct _tmp4B2;ofile_string=(struct _dyneither_ptr)((_tmp4B2.tag=0,((_tmp4B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpFC)),((_tmp4B3[0]=& _tmp4B2,Cyc_aprintf(((_tmp4B4=" > %s",_tag_dyneither(_tmp4B4,sizeof(char),6))),_tag_dyneither(_tmp4B3,sizeof(void*),1))))))));}{
# 947
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 951
if(Cyc_dependencies_target == 0){
# 955
const char*_tmp4B5;fixup_string=((_tmp4B5=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4B5,sizeof(char),35)));}else{
# 960
const char*_tmp4B9;void*_tmp4B8[1];struct Cyc_String_pa_PrintArg_struct _tmp4B7;fixup_string=(struct _dyneither_ptr)(
(_tmp4B7.tag=0,((_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4B8[0]=& _tmp4B7,Cyc_aprintf(((_tmp4B9=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4B9,sizeof(char),29))),_tag_dyneither(_tmp4B8,sizeof(void*),1))))))));}}else{
# 964
const char*_tmp4BA;fixup_string=((_tmp4BA="",_tag_dyneither(_tmp4BA,sizeof(char),1)));}{
# 966
const char*_tmp4C3;void*_tmp4C2[6];struct Cyc_String_pa_PrintArg_struct _tmp4C1;struct Cyc_String_pa_PrintArg_struct _tmp4C0;struct Cyc_String_pa_PrintArg_struct _tmp4BF;struct Cyc_String_pa_PrintArg_struct _tmp4BE;struct Cyc_String_pa_PrintArg_struct _tmp4BD;struct Cyc_String_pa_PrintArg_struct _tmp4BC;struct _dyneither_ptr _tmp116=(_tmp4BC.tag=0,((_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4BD.tag=0,((_tmp4BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4BE.tag=0,((_tmp4BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 969
Cyc_sh_escape_string(filename)),((_tmp4BF.tag=0,((_tmp4BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4C0.tag=0,((_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp103),((_tmp4C1.tag=0,((_tmp4C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4C2[0]=& _tmp4C1,((_tmp4C2[1]=& _tmp4C0,((_tmp4C2[2]=& _tmp4BF,((_tmp4C2[3]=& _tmp4BE,((_tmp4C2[4]=& _tmp4BD,((_tmp4C2[5]=& _tmp4BC,Cyc_aprintf(((_tmp4C3="%s %s%s %s%s%s",_tag_dyneither(_tmp4C3,sizeof(char),15))),_tag_dyneither(_tmp4C2,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 972
if(Cyc_v_r){const char*_tmp4C7;void*_tmp4C6[1];struct Cyc_String_pa_PrintArg_struct _tmp4C5;(_tmp4C5.tag=0,((_tmp4C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp116),((_tmp4C6[0]=& _tmp4C5,Cyc_fprintf(Cyc_stderr,((_tmp4C7="%s\n",_tag_dyneither(_tmp4C7,sizeof(char),4))),_tag_dyneither(_tmp4C6,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp116,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp4CA;void*_tmp4C9;(_tmp4C9=0,Cyc_fprintf(Cyc_stderr,((_tmp4CA="\nError: preprocessing\n",_tag_dyneither(_tmp4CA,sizeof(char),23))),_tag_dyneither(_tmp4C9,sizeof(void*),0)));}
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
return;}
# 979
if(Cyc_stop_after_cpp_r)return;
# 982
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpFC);{
const char*_tmp4CC;const char*_tmp4CB;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFC,((_tmp4CB="r",_tag_dyneither(_tmp4CB,sizeof(char),2))),((_tmp4CC="file",_tag_dyneither(_tmp4CC,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 986
struct Cyc_List_List*tds=0;
# 989
{struct _handler_cons _tmp11C;_push_handler(& _tmp11C);{int _tmp11E=0;if(setjmp(_tmp11C.handler))_tmp11E=1;if(!_tmp11E){
{const char*_tmp4CD;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4CD="parsing",_tag_dyneither(_tmp4CD,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp11D=(void*)_exn_thrown;void*_tmp121=_tmp11D;void*_tmp122;void*_tmp123;_LL25: _tmp122=_tmp121;_LL26:
# 993
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
Cyc_Core_rethrow(_tmp122);_LL27: _tmp123=_tmp121;_LL28:(void)_rethrow(_tmp123);_LL24:;}};}
# 997
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
return;}
# 1003
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 1008
struct _RegionHandle _tmp124=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp124;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp125=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 1013
struct _tuple22 _tmp4CE;struct _tuple22 _tmp126=(_tmp4CE.f1=tc_rgn,((_tmp4CE.f2=_tmp125,_tmp4CE)));
{const char*_tmp4CF;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4CF="type checking",_tag_dyneither(_tmp4CF,sizeof(char),14))),Cyc_do_typecheck,& _tmp126,tds);}
# 1016
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
_npop_handler(0);return;}
# 1020
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp4D0;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4D0="control-flow checking",_tag_dyneither(_tmp4D0,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
_npop_handler(0);return;}
# 1028
if(Cyc_generate_interface_r){
const char*_tmp4D3;const char*_tmp4D2;const char*_tmp4D1;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp4D1=(const char*)_untag_dyneither_ptr(_tmpFD,sizeof(char),1),_tag_dyneither(_tmp4D1,sizeof(char),_get_zero_arr_size_char((void*)_tmp4D1,1)))),((_tmp4D2="w",_tag_dyneither(_tmp4D2,sizeof(char),2))),((_tmp4D3="interface file",_tag_dyneither(_tmp4D3,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 1033
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 1040
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpFD,sizeof(char),1),"r");
const char*_tmp4D5;const char*_tmp4D4;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpFE,((_tmp4D4="w",_tag_dyneither(_tmp4D4,sizeof(char),2))),((_tmp4D5="interface object file",_tag_dyneither(_tmp4D5,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1047
Cyc_Position_reset_position(_tmpFD);{
struct _tuple23 _tmp4D6;struct _tuple23 _tmp12C=(_tmp4D6.f1=_tmp125,((_tmp4D6.f2=inter_file,((_tmp4D6.f3=inter_objfile,_tmp4D6)))));
{const char*_tmp4D7;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4D7="interface checking",_tag_dyneither(_tmp4D7,sizeof(char),19))),Cyc_do_interface,& _tmp12C,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1055
if(Cyc_cf_r)goto PRINTC;
# 1058
{const char*_tmp4D8;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4D8="translation to C",_tag_dyneither(_tmp4D8,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
_npop_handler(0);return;}
# 1064
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpFC);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1072
const char*_tmp4D9;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4D9="post-pass to VC",_tag_dyneither(_tmp4D9,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1079
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp4DB;const char*_tmp4DA;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4DA="w",_tag_dyneither(_tmp4DA,sizeof(char),2))),((_tmp4DB="output file",_tag_dyneither(_tmp4DB,sizeof(char),12))));}else{
# 1085
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp4DD;const char*_tmp4DC;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4DC="w",_tag_dyneither(_tmp4DC,sizeof(char),2))),((_tmp4DD="output file",_tag_dyneither(_tmp4DD,sizeof(char),12))));}else{
# 1089
const char*_tmp4DF;const char*_tmp4DE;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFF,((_tmp4DE="w",_tag_dyneither(_tmp4DE,sizeof(char),2))),((_tmp4DF="output file",_tag_dyneither(_tmp4DF,sizeof(char),12))));}}
# 1091
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1093
{struct _handler_cons _tmp13A;_push_handler(& _tmp13A);{int _tmp13C=0;if(setjmp(_tmp13A.handler))_tmp13C=1;if(!_tmp13C){
if(!Cyc_noprint_r){
const char*_tmp4E0;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4E0="printing",_tag_dyneither(_tmp4E0,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1094
;_pop_handler();}else{void*_tmp13B=(void*)_exn_thrown;void*_tmp13F=_tmp13B;void*_tmp140;void*_tmp141;_LL2A: _tmp140=_tmp13F;_LL2B:
# 1098
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp4E3;struct Cyc_List_List*_tmp4E2;Cyc_cfiles=((_tmp4E2=_cycalloc(sizeof(*_tmp4E2)),((_tmp4E2->hd=((_tmp4E3=_cycalloc(sizeof(*_tmp4E3)),((_tmp4E3[0]=(struct _dyneither_ptr)_tmpFF,_tmp4E3)))),((_tmp4E2->tl=Cyc_cfiles,_tmp4E2))))));}
Cyc_Core_rethrow(_tmp140);_LL2C: _tmp141=_tmp13F;_LL2D:(void)_rethrow(_tmp141);_LL29:;}};}
# 1103
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp4E6;struct Cyc_List_List*_tmp4E5;Cyc_cfiles=((_tmp4E5=_cycalloc(sizeof(*_tmp4E5)),((_tmp4E5->hd=((_tmp4E6=_cycalloc(sizeof(*_tmp4E6)),((_tmp4E6[0]=(struct _dyneither_ptr)_tmpFF,_tmp4E6)))),((_tmp4E5->tl=Cyc_cfiles,_tmp4E5))))));};}
# 1008
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp15E[8]="<final>";
# 1108
static struct _dyneither_ptr Cyc_final_str={_tmp15E,_tmp15E,_tmp15E + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1111
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1115
struct _dyneither_ptr basename;
{struct _handler_cons _tmp15F;_push_handler(& _tmp15F);{int _tmp161=0;if(setjmp(_tmp15F.handler))_tmp161=1;if(!_tmp161){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp160=(void*)_exn_thrown;void*_tmp163=_tmp160;void*_tmp165;_LL2F: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp164=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp163;if(_tmp164->tag != Cyc_Core_Invalid_argument)goto _LL31;}_LL30:
# 1119
 basename=*n;goto _LL2E;_LL31: _tmp165=_tmp163;_LL32:(void)_rethrow(_tmp165);_LL2E:;}};}{
# 1121
const char*_tmp4E7;struct _dyneither_ptr _tmp166=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp4E7=".cycio",_tag_dyneither(_tmp4E7,sizeof(char),7))));
const char*_tmp4E9;const char*_tmp4E8;struct Cyc___cycFILE*_tmp167=Cyc_try_file_open((struct _dyneither_ptr)_tmp166,((_tmp4E8="rb",_tag_dyneither(_tmp4E8,sizeof(char),3))),((_tmp4E9="interface object file",_tag_dyneither(_tmp4E9,sizeof(char),22))));
if(_tmp167 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1128
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp166);{
# 1130
struct Cyc_Interface_I*_tmp168=Cyc_Interface_load(_tmp167);
Cyc_file_close(_tmp167);
return _tmp168;};};};}
# 1135
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1139
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp4EF(unsigned int*_tmp4EE,unsigned int*_tmp4ED,int**_tmp4EB){for(*_tmp4EE=0;*_tmp4EE < *_tmp4ED;(*_tmp4EE)++){(*_tmp4EB)[*_tmp4EE]=(int)0;}}static void _tmp4F7(unsigned int*_tmp4F6,unsigned int*_tmp4F5,struct _dyneither_ptr**_tmp4F3){for(*_tmp4F6=0;*_tmp4F6 < *_tmp4F5;(*_tmp4F6)++){(*_tmp4F3)[*_tmp4F6]=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1176
_tag_dyneither(0,0,0));}}static void _tmp4FD(unsigned int*_tmp4FC,unsigned int*_tmp4FB,struct _dyneither_ptr**_tmp4F9){for(*_tmp4FC=0;*_tmp4FC < *_tmp4FB;(*_tmp4FC)++){(*_tmp4F9)[*_tmp4FC]=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tag_dyneither(0,0,0));}}
# 1142
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1147
GC_blacklist_warn_clear();{
# 1149
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1154
struct _RegionHandle _tmp16C=_new_region("r");struct _RegionHandle*r=& _tmp16C;_push_region(r);{
# 1156
unsigned int _tmp4EE;unsigned int _tmp4ED;struct _dyneither_ptr _tmp4EC;int*_tmp4EB;unsigned int _tmp4EA;struct _dyneither_ptr _tmp16D=(_tmp4EA=(unsigned int)argc,((_tmp4EB=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp4EA)),((_tmp4EC=_tag_dyneither(_tmp4EB,sizeof(int),_tmp4EA),((((_tmp4ED=_tmp4EA,_tmp4EF(& _tmp4EE,& _tmp4ED,& _tmp4EB))),_tmp4EC)))))));
int _tmp16E=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp4F0;if(Cyc_strncmp(((_tmp4F0="-B",_tag_dyneither(_tmp4F0,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i))=1;
++ _tmp16E;}else{
# 1164
const char*_tmp4F1;if(Cyc_strcmp(((_tmp4F1="-b",_tag_dyneither(_tmp4F1,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i))=1;
++ _tmp16E;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i))=1;
++ _tmp16E;}}}{
# 1176
unsigned int _tmp4F6;unsigned int _tmp4F5;struct _dyneither_ptr _tmp4F4;struct _dyneither_ptr*_tmp4F3;unsigned int _tmp4F2;struct _dyneither_ptr _tmp171=(_tmp4F2=(unsigned int)(_tmp16E + 1),((_tmp4F3=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4F2)),((_tmp4F4=_tag_dyneither(_tmp4F3,sizeof(struct _dyneither_ptr),_tmp4F2),((((_tmp4F5=_tmp4F2,_tmp4F7(& _tmp4F6,& _tmp4F5,& _tmp4F3))),_tmp4F4)))))));
unsigned int _tmp4FC;unsigned int _tmp4FB;struct _dyneither_ptr _tmp4FA;struct _dyneither_ptr*_tmp4F9;unsigned int _tmp4F8;struct _dyneither_ptr _tmp172=(_tmp4F8=(unsigned int)(argc - _tmp16E),((_tmp4F9=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4F8)),((_tmp4FA=_tag_dyneither(_tmp4F9,sizeof(struct _dyneither_ptr),_tmp4F8),((((_tmp4FB=_tmp4F8,_tmp4FD(& _tmp4FC,& _tmp4FB,& _tmp4F9))),_tmp4FA)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp171,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp172,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp16D,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp171,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp172,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1185
struct _tuple24*_tmp9FC;const char*_tmp9FB;const char*_tmp9FA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F8;const char*_tmp9F7;struct _tuple24*_tmp9F6;const char*_tmp9F5;const char*_tmp9F4;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9F3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9F2;const char*_tmp9F1;struct _tuple24*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9ED;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9EC;const char*_tmp9EB;struct _tuple24*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9E7;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9E6;const char*_tmp9E5;struct _tuple24*_tmp9E4;const char*_tmp9E3;const char*_tmp9E2;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9E1;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9E0;const char*_tmp9DF;struct _tuple24*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9DB;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9DA;const char*_tmp9D9;struct _tuple24*_tmp9D8;const char*_tmp9D7;const char*_tmp9D6;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9D5;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9D4;const char*_tmp9D3;struct _tuple24*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9CF;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9CE;const char*_tmp9CD;struct _tuple24*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9C9;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9C8;const char*_tmp9C7;struct _tuple24*_tmp9C6;const char*_tmp9C5;const char*_tmp9C4;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9C3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9C2;const char*_tmp9C1;struct _tuple24*_tmp9C0;const char*_tmp9BF;const char*_tmp9BE;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9BD;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9BC;const char*_tmp9BB;struct _tuple24*_tmp9BA;const char*_tmp9B9;const char*_tmp9B8;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9B7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9B6;const char*_tmp9B5;struct _tuple24*_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9B1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9B0;const char*_tmp9AF;struct _tuple24*_tmp9AE;const char*_tmp9AD;const char*_tmp9AC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9AB;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9AA;const char*_tmp9A9;struct _tuple24*_tmp9A8;const char*_tmp9A7;const char*_tmp9A6;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9A5;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9A4;const char*_tmp9A3;struct _tuple24*_tmp9A2;const char*_tmp9A1;const char*_tmp9A0;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp99F;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp99E;const char*_tmp99D;struct _tuple24*_tmp99C;const char*_tmp99B;const char*_tmp99A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp999;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp998;const char*_tmp997;struct _tuple24*_tmp996;const char*_tmp995;const char*_tmp994;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp993;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp992;const char*_tmp991;struct _tuple24*_tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp98D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp98C;const char*_tmp98B;struct _tuple24*_tmp98A;const char*_tmp989;const char*_tmp988;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp987;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp986;const char*_tmp985;struct _tuple24*_tmp984;const char*_tmp983;const char*_tmp982;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp981;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp980;const char*_tmp97F;struct _tuple24*_tmp97E;const char*_tmp97D;const char*_tmp97C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp97B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp97A;const char*_tmp979;struct _tuple24*_tmp978;const char*_tmp977;const char*_tmp976;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp975;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp974;const char*_tmp973;struct _tuple24*_tmp972;const char*_tmp971;const char*_tmp970;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp96F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp96E;const char*_tmp96D;struct _tuple24*_tmp96C;const char*_tmp96B;const char*_tmp96A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp969;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp968;const char*_tmp967;struct _tuple24*_tmp966;const char*_tmp965;const char*_tmp964;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp963;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp962;const char*_tmp961;struct _tuple24*_tmp960;const char*_tmp95F;const char*_tmp95E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp95D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp95C;const char*_tmp95B;struct _tuple24*_tmp95A;const char*_tmp959;const char*_tmp958;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp957;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp956;const char*_tmp955;struct _tuple24*_tmp954;const char*_tmp953;const char*_tmp952;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp951;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp950;const char*_tmp94F;struct _tuple24*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94A;const char*_tmp949;struct _tuple24*_tmp948;const char*_tmp947;const char*_tmp946;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp945;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp944;const char*_tmp943;struct _tuple24*_tmp942;const char*_tmp941;const char*_tmp940;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp93F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp93E;const char*_tmp93D;struct _tuple24*_tmp93C;const char*_tmp93B;const char*_tmp93A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp939;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp938;const char*_tmp937;struct _tuple24*_tmp936;const char*_tmp935;const char*_tmp934;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp933;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp932;const char*_tmp931;struct _tuple24*_tmp930;const char*_tmp92F;const char*_tmp92E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp92D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp92C;const char*_tmp92B;struct _tuple24*_tmp92A;const char*_tmp929;const char*_tmp928;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp927;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp926;const char*_tmp925;struct _tuple24*_tmp924;const char*_tmp923;const char*_tmp922;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp921;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp920;const char*_tmp91F;struct _tuple24*_tmp91E;const char*_tmp91D;const char*_tmp91C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp91B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp91A;const char*_tmp919;struct _tuple24*_tmp918;const char*_tmp917;const char*_tmp916;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp915;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp914;const char*_tmp913;struct _tuple24*_tmp912;const char*_tmp911;const char*_tmp910;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp90F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp90E;const char*_tmp90D;struct _tuple24*_tmp90C;const char*_tmp90B;const char*_tmp90A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp909;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp908;const char*_tmp907;struct _tuple24*_tmp906;const char*_tmp905;const char*_tmp904;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp903;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp902;const char*_tmp901;struct _tuple24*_tmp900;const char*_tmp8FF;const char*_tmp8FE;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8FD;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8FC;const char*_tmp8FB;struct _tuple24*_tmp8FA;const char*_tmp8F9;const char*_tmp8F8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8F7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8F6;const char*_tmp8F5;struct _tuple24*_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8F1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8F0;const char*_tmp8EF;struct _tuple24*_tmp8EE;const char*_tmp8ED;const char*_tmp8EC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8EB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8EA;const char*_tmp8E9;struct _tuple24*_tmp8E8;const char*_tmp8E7;const char*_tmp8E6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8E5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8E4;const char*_tmp8E3;struct _tuple24*_tmp8E2;const char*_tmp8E1;const char*_tmp8E0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8DF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8DE;const char*_tmp8DD;struct _tuple24*_tmp8DC;const char*_tmp8DB;const char*_tmp8DA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D8;const char*_tmp8D7;struct _tuple24*_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D2;const char*_tmp8D1;struct _tuple24*_tmp8D0;const char*_tmp8CF;const char*_tmp8CE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8CD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8CC;const char*_tmp8CB;struct _tuple24*_tmp8CA;const char*_tmp8C9;const char*_tmp8C8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8C7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8C6;const char*_tmp8C5;struct _tuple24*_tmp8C4;const char*_tmp8C3;const char*_tmp8C2;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8C1;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8C0;const char*_tmp8BF;struct _tuple24*_tmp8BE;const char*_tmp8BD;const char*_tmp8BC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8BB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8BA;const char*_tmp8B9;struct _tuple24*_tmp8B8;const char*_tmp8B7;const char*_tmp8B6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8B5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8B4;const char*_tmp8B3;struct _tuple24*_tmp8B2;const char*_tmp8B1;const char*_tmp8B0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8AF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8AE;const char*_tmp8AD;struct _tuple24*_tmp8AC;const char*_tmp8AB;const char*_tmp8AA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8A9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8A8;const char*_tmp8A7;struct _tuple24*_tmp8A6;const char*_tmp8A5;const char*_tmp8A4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8A3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8A2;const char*_tmp8A1;struct _tuple24*_tmp8A0;const char*_tmp89F;const char*_tmp89E;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp89D;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp89C;const char*_tmp89B;struct _tuple24*_tmp89A;const char*_tmp899;const char*_tmp898;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp897;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp896;const char*_tmp895;struct _tuple24*_tmp894;const char*_tmp893;const char*_tmp892;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp891;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp890;const char*_tmp88F;struct _tuple24*_tmp88E;const char*_tmp88D;const char*_tmp88C;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp88B;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp88A;const char*_tmp889;struct _tuple24*_tmp888;const char*_tmp887;const char*_tmp886;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp885;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp884;const char*_tmp883;struct _tuple24*_tmp882;const char*_tmp881;const char*_tmp880;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp87F;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp87E;const char*_tmp87D;struct _tuple24*_tmp87C;const char*_tmp87B;const char*_tmp87A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp879;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp878;const char*_tmp877;struct _tuple24*_tmp876;const char*_tmp875;const char*_tmp874;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp873;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp872;const char*_tmp871;struct _tuple24*_tmp870;const char*_tmp86F;const char*_tmp86E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp86D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp86C;const char*_tmp86B;struct _tuple24*_tmp86A;const char*_tmp869;const char*_tmp868;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp867;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp866;const char*_tmp865;struct _tuple24*_tmp864;const char*_tmp863;const char*_tmp862;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp861;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp860;const char*_tmp85F;struct _tuple24*_tmp85E;const char*_tmp85D;const char*_tmp85C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp85B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp85A;const char*_tmp859;struct _tuple24*_tmp858;const char*_tmp857;const char*_tmp856;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp855;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp854;const char*_tmp853;struct _tuple24*_tmp852[71];struct Cyc_List_List*options=
(_tmp852[70]=(
# 1399
(_tmp858=_region_malloc(r,sizeof(*_tmp858)),((_tmp858->f1=((_tmp857="-known-gcc-flags-only",_tag_dyneither(_tmp857,sizeof(char),22))),((_tmp858->f2=0,((_tmp858->f3=((_tmp856="",_tag_dyneither(_tmp856,sizeof(char),1))),((_tmp858->f4=(void*)(
(_tmp854=_region_malloc(r,sizeof(*_tmp854)),((_tmp854[0]=((_tmp855.tag=0,((_tmp855.f1=Cyc_noassume_gcc_flag,_tmp855)))),_tmp854)))),((_tmp858->f5=(
(_tmp853="do not assume that unknown flags should be passed to gcc",_tag_dyneither(_tmp853,sizeof(char),57))),_tmp858)))))))))))),((_tmp852[69]=(
# 1396
(_tmp85E=_region_malloc(r,sizeof(*_tmp85E)),((_tmp85E->f1=((_tmp85D="--parsestate",_tag_dyneither(_tmp85D,sizeof(char),13))),((_tmp85E->f2=0,((_tmp85E->f3=((_tmp85C="",_tag_dyneither(_tmp85C,sizeof(char),1))),((_tmp85E->f4=(void*)(
(_tmp85A=_region_malloc(r,sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85B.tag=3,((_tmp85B.f1=& Cyc_ParseErrors_print_state_and_token,_tmp85B)))),_tmp85A)))),((_tmp85E->f5=(
(_tmp859="print the parse state and token on a syntax error",_tag_dyneither(_tmp859,sizeof(char),50))),_tmp85E)))))))))))),((_tmp852[68]=(
# 1393
(_tmp864=_region_malloc(r,sizeof(*_tmp864)),((_tmp864->f1=((_tmp863="--manyerrors",_tag_dyneither(_tmp863,sizeof(char),13))),((_tmp864->f2=0,((_tmp864->f3=((_tmp862="",_tag_dyneither(_tmp862,sizeof(char),1))),((_tmp864->f4=(void*)(
(_tmp860=_region_malloc(r,sizeof(*_tmp860)),((_tmp860[0]=((_tmp861.tag=0,((_tmp861.f1=Cyc_set_many_errors,_tmp861)))),_tmp860)))),((_tmp864->f5=(
(_tmp85F="don't stop after only a few errors.",_tag_dyneither(_tmp85F,sizeof(char),36))),_tmp864)))))))))))),((_tmp852[67]=(
# 1390
(_tmp86A=_region_malloc(r,sizeof(*_tmp86A)),((_tmp86A->f1=((_tmp869="--interface",_tag_dyneither(_tmp869,sizeof(char),12))),((_tmp86A->f2=0,((_tmp86A->f3=((_tmp868=" <file>",_tag_dyneither(_tmp868,sizeof(char),8))),((_tmp86A->f4=(void*)(
(_tmp866=_region_malloc(r,sizeof(*_tmp866)),((_tmp866[0]=((_tmp867.tag=5,((_tmp867.f1=Cyc_set_specified_interface,_tmp867)))),_tmp866)))),((_tmp86A->f5=(
(_tmp865="Set the interface file to be <file>.",_tag_dyneither(_tmp865,sizeof(char),37))),_tmp86A)))))))))))),((_tmp852[66]=(
# 1387
(_tmp870=_region_malloc(r,sizeof(*_tmp870)),((_tmp870->f1=((_tmp86F="--geninterface",_tag_dyneither(_tmp86F,sizeof(char),15))),((_tmp870->f2=0,((_tmp870->f3=((_tmp86E="",_tag_dyneither(_tmp86E,sizeof(char),1))),((_tmp870->f4=(void*)(
(_tmp86C=_region_malloc(r,sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86D.tag=3,((_tmp86D.f1=& Cyc_generate_interface_r,_tmp86D)))),_tmp86C)))),((_tmp870->f5=(
(_tmp86B="Generate interface files",_tag_dyneither(_tmp86B,sizeof(char),25))),_tmp870)))))))))))),((_tmp852[65]=(
# 1384
(_tmp876=_region_malloc(r,sizeof(*_tmp876)),((_tmp876->f1=((_tmp875="-idirafter",_tag_dyneither(_tmp875,sizeof(char),11))),((_tmp876->f2=0,((_tmp876->f3=((_tmp874="<dir>",_tag_dyneither(_tmp874,sizeof(char),6))),((_tmp876->f4=(void*)(
(_tmp872=_region_malloc(r,sizeof(*_tmp872)),((_tmp872[0]=((_tmp873.tag=5,((_tmp873.f1=Cyc_add_idirafter,_tmp873)))),_tmp872)))),((_tmp876->f5=(
(_tmp871="Add the directory to the second include path.",_tag_dyneither(_tmp871,sizeof(char),46))),_tmp876)))))))))))),((_tmp852[64]=(
# 1381
(_tmp87C=_region_malloc(r,sizeof(*_tmp87C)),((_tmp87C->f1=((_tmp87B="-isystem",_tag_dyneither(_tmp87B,sizeof(char),9))),((_tmp87C->f2=0,((_tmp87C->f3=((_tmp87A="<dir>",_tag_dyneither(_tmp87A,sizeof(char),6))),((_tmp87C->f4=(void*)(
(_tmp878=_region_malloc(r,sizeof(*_tmp878)),((_tmp878[0]=((_tmp879.tag=5,((_tmp879.f1=Cyc_add_isystem,_tmp879)))),_tmp878)))),((_tmp87C->f5=(
(_tmp877="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp877,sizeof(char),90))),_tmp87C)))))))))))),((_tmp852[63]=(
# 1378
(_tmp882=_region_malloc(r,sizeof(*_tmp882)),((_tmp882->f1=((_tmp881="-iwithprefixbefore",_tag_dyneither(_tmp881,sizeof(char),19))),((_tmp882->f2=0,((_tmp882->f3=((_tmp880="<dir>",_tag_dyneither(_tmp880,sizeof(char),6))),((_tmp882->f4=(void*)(
(_tmp87E=_region_malloc(r,sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp87F.tag=5,((_tmp87F.f1=Cyc_add_iwithprefixbefore,_tmp87F)))),_tmp87E)))),((_tmp882->f5=(
(_tmp87D="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp87D,sizeof(char),45))),_tmp882)))))))))))),((_tmp852[62]=(
# 1375
(_tmp888=_region_malloc(r,sizeof(*_tmp888)),((_tmp888->f1=((_tmp887="-iwithprefix",_tag_dyneither(_tmp887,sizeof(char),13))),((_tmp888->f2=0,((_tmp888->f3=((_tmp886="<dir>",_tag_dyneither(_tmp886,sizeof(char),6))),((_tmp888->f4=(void*)(
(_tmp884=_region_malloc(r,sizeof(*_tmp884)),((_tmp884[0]=((_tmp885.tag=5,((_tmp885.f1=Cyc_add_iwithprefix,_tmp885)))),_tmp884)))),((_tmp888->f5=(
(_tmp883="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp883,sizeof(char),47))),_tmp888)))))))))))),((_tmp852[61]=(
# 1372
(_tmp88E=_region_malloc(r,sizeof(*_tmp88E)),((_tmp88E->f1=((_tmp88D="-iprefix",_tag_dyneither(_tmp88D,sizeof(char),9))),((_tmp88E->f2=0,((_tmp88E->f3=((_tmp88C="<prefix>",_tag_dyneither(_tmp88C,sizeof(char),9))),((_tmp88E->f4=(void*)(
(_tmp88A=_region_malloc(r,sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp88B.tag=5,((_tmp88B.f1=Cyc_add_iprefix,_tmp88B)))),_tmp88A)))),((_tmp88E->f5=(
(_tmp889="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp889,sizeof(char),67))),_tmp88E)))))))))))),((_tmp852[60]=(
# 1369
(_tmp894=_region_malloc(r,sizeof(*_tmp894)),((_tmp894->f1=((_tmp893="-noregions",_tag_dyneither(_tmp893,sizeof(char),11))),((_tmp894->f2=0,((_tmp894->f3=((_tmp892="",_tag_dyneither(_tmp892,sizeof(char),1))),((_tmp894->f4=(void*)(
(_tmp890=_region_malloc(r,sizeof(*_tmp890)),((_tmp890[0]=((_tmp891.tag=3,((_tmp891.f1=& Cyc_Absyn_no_regions,_tmp891)))),_tmp890)))),((_tmp894->f5=(
(_tmp88F="Generate code that doesn't use regions",_tag_dyneither(_tmp88F,sizeof(char),39))),_tmp894)))))))))))),((_tmp852[59]=(
# 1366
(_tmp89A=_region_malloc(r,sizeof(*_tmp89A)),((_tmp89A->f1=((_tmp899="-port",_tag_dyneither(_tmp899,sizeof(char),6))),((_tmp89A->f2=0,((_tmp89A->f3=((_tmp898="",_tag_dyneither(_tmp898,sizeof(char),1))),((_tmp89A->f4=(void*)(
(_tmp896=_region_malloc(r,sizeof(*_tmp896)),((_tmp896[0]=((_tmp897.tag=0,((_tmp897.f1=Cyc_set_port_c_code,_tmp897)))),_tmp896)))),((_tmp89A->f5=(
(_tmp895="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp895,sizeof(char),38))),_tmp89A)))))))))))),((_tmp852[58]=(
# 1363
(_tmp8A0=_region_malloc(r,sizeof(*_tmp8A0)),((_tmp8A0->f1=((_tmp89F="-detailedlocation",_tag_dyneither(_tmp89F,sizeof(char),18))),((_tmp8A0->f2=0,((_tmp8A0->f3=((_tmp89E="",_tag_dyneither(_tmp89E,sizeof(char),1))),((_tmp8A0->f4=(void*)(
(_tmp89C=_region_malloc(r,sizeof(*_tmp89C)),((_tmp89C[0]=((_tmp89D.tag=4,((_tmp89D.f1=& Cyc_Position_use_gcc_style_location,_tmp89D)))),_tmp89C)))),((_tmp8A0->f5=(
(_tmp89B="Try to give more detailed location information for errors",_tag_dyneither(_tmp89B,sizeof(char),58))),_tmp8A0)))))))))))),((_tmp852[57]=(
# 1360
(_tmp8A6=_region_malloc(r,sizeof(*_tmp8A6)),((_tmp8A6->f1=((_tmp8A5="-noregister",_tag_dyneither(_tmp8A5,sizeof(char),12))),((_tmp8A6->f2=0,((_tmp8A6->f3=((_tmp8A4="",_tag_dyneither(_tmp8A4,sizeof(char),1))),((_tmp8A6->f4=(void*)(
(_tmp8A2=_region_malloc(r,sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A3.tag=3,((_tmp8A3.f1=& Cyc_Parse_no_register,_tmp8A3)))),_tmp8A2)))),((_tmp8A6->f5=(
(_tmp8A1="Treat register storage class as public",_tag_dyneither(_tmp8A1,sizeof(char),39))),_tmp8A6)))))))))))),((_tmp852[56]=(
# 1357
(_tmp8AC=_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC->f1=((_tmp8AB="-warnregioncoerce",_tag_dyneither(_tmp8AB,sizeof(char),18))),((_tmp8AC->f2=0,((_tmp8AC->f3=((_tmp8AA="",_tag_dyneither(_tmp8AA,sizeof(char),1))),((_tmp8AC->f4=(void*)(
(_tmp8A8=_region_malloc(r,sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=3,((_tmp8A9.f1=& Cyc_Tcutil_warn_region_coerce,_tmp8A9)))),_tmp8A8)))),((_tmp8AC->f5=(
(_tmp8A7="Warn when any region coercion is inserted",_tag_dyneither(_tmp8A7,sizeof(char),42))),_tmp8AC)))))))))))),((_tmp852[55]=(
# 1354
(_tmp8B2=_region_malloc(r,sizeof(*_tmp8B2)),((_tmp8B2->f1=((_tmp8B1="-warnaliascoercion",_tag_dyneither(_tmp8B1,sizeof(char),19))),((_tmp8B2->f2=0,((_tmp8B2->f3=((_tmp8B0="",_tag_dyneither(_tmp8B0,sizeof(char),1))),((_tmp8B2->f4=(void*)(
(_tmp8AE=_region_malloc(r,sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8AF.tag=3,((_tmp8AF.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp8AF)))),_tmp8AE)))),((_tmp8B2->f5=(
(_tmp8AD="Warn when any alias coercion is inserted",_tag_dyneither(_tmp8AD,sizeof(char),41))),_tmp8B2)))))))))))),((_tmp852[54]=(
# 1351
(_tmp8B8=_region_malloc(r,sizeof(*_tmp8B8)),((_tmp8B8->f1=((_tmp8B7="-warnnullchecks",_tag_dyneither(_tmp8B7,sizeof(char),16))),((_tmp8B8->f2=0,((_tmp8B8->f3=((_tmp8B6="",_tag_dyneither(_tmp8B6,sizeof(char),1))),((_tmp8B8->f4=(void*)(
(_tmp8B4=_region_malloc(r,sizeof(*_tmp8B4)),((_tmp8B4[0]=((_tmp8B5.tag=3,((_tmp8B5.f1=& Cyc_Toc_warn_all_null_deref,_tmp8B5)))),_tmp8B4)))),((_tmp8B8->f5=(
(_tmp8B3="Warn when any null check can't be eliminated",_tag_dyneither(_tmp8B3,sizeof(char),45))),_tmp8B8)))))))))))),((_tmp852[53]=(
# 1348
(_tmp8BE=_region_malloc(r,sizeof(*_tmp8BE)),((_tmp8BE->f1=((_tmp8BD="-warnboundschecks",_tag_dyneither(_tmp8BD,sizeof(char),18))),((_tmp8BE->f2=0,((_tmp8BE->f3=((_tmp8BC="",_tag_dyneither(_tmp8BC,sizeof(char),1))),((_tmp8BE->f4=(void*)(
(_tmp8BA=_region_malloc(r,sizeof(*_tmp8BA)),((_tmp8BA[0]=((_tmp8BB.tag=3,((_tmp8BB.f1=& Cyc_Toc_warn_bounds_checks,_tmp8BB)))),_tmp8BA)))),((_tmp8BE->f5=(
(_tmp8B9="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp8B9,sizeof(char),44))),_tmp8BE)))))))))))),((_tmp852[52]=(
# 1345
(_tmp8C4=_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4->f1=((_tmp8C3="-CI",_tag_dyneither(_tmp8C3,sizeof(char),4))),((_tmp8C4->f2=0,((_tmp8C4->f3=((_tmp8C2=" <file>",_tag_dyneither(_tmp8C2,sizeof(char),8))),((_tmp8C4->f4=(void*)(
(_tmp8C0=_region_malloc(r,sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=5,((_tmp8C1.f1=Cyc_set_cyc_include,_tmp8C1)))),_tmp8C0)))),((_tmp8C4->f5=(
(_tmp8BF="Set cyc_include.h to be <file>",_tag_dyneither(_tmp8BF,sizeof(char),31))),_tmp8C4)))))))))))),((_tmp852[51]=(
# 1342
(_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA->f1=((_tmp8C9="-compile-for-boot",_tag_dyneither(_tmp8C9,sizeof(char),18))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C8="",_tag_dyneither(_tmp8C8,sizeof(char),1))),((_tmp8CA->f4=(void*)(
(_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=3,((_tmp8C7.f1=& Cyc_Lex_compile_for_boot_r,_tmp8C7)))),_tmp8C6)))),((_tmp8CA->f5=(
(_tmp8C5="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp8C5,sizeof(char),71))),_tmp8CA)))))))))))),((_tmp852[50]=(
# 1339
(_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CF="-nocyc_setjmp",_tag_dyneither(_tmp8CF,sizeof(char),14))),((_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CE="",_tag_dyneither(_tmp8CE,sizeof(char),1))),((_tmp8D0->f4=(void*)(
(_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=3,((_tmp8CD.f1=& Cyc_nocyc_setjmp_r,_tmp8CD)))),_tmp8CC)))),((_tmp8D0->f5=(
(_tmp8CB="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp8CB,sizeof(char),46))),_tmp8D0)))))))))))),((_tmp852[49]=(
# 1336
(_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6->f1=((_tmp8D5="-printalleffects",_tag_dyneither(_tmp8D5,sizeof(char),17))),((_tmp8D6->f2=0,((_tmp8D6->f3=((_tmp8D4="",_tag_dyneither(_tmp8D4,sizeof(char),1))),((_tmp8D6->f4=(void*)(
(_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=3,((_tmp8D3.f1=& Cyc_print_all_effects_r,_tmp8D3)))),_tmp8D2)))),((_tmp8D6->f5=(
(_tmp8D1="Print effects for functions (type debugging)",_tag_dyneither(_tmp8D1,sizeof(char),45))),_tmp8D6)))))))))))),((_tmp852[48]=(
# 1333
(_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8DB="-printfullevars",_tag_dyneither(_tmp8DB,sizeof(char),16))),((_tmp8DC->f2=0,((_tmp8DC->f3=((_tmp8DA="",_tag_dyneither(_tmp8DA,sizeof(char),1))),((_tmp8DC->f4=(void*)(
(_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=3,((_tmp8D9.f1=& Cyc_print_full_evars_r,_tmp8D9)))),_tmp8D8)))),((_tmp8DC->f5=(
(_tmp8D7="Print full information for evars (type debugging)",_tag_dyneither(_tmp8D7,sizeof(char),50))),_tmp8DC)))))))))))),((_tmp852[47]=(
# 1330
(_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2->f1=((_tmp8E1="-printallkinds",_tag_dyneither(_tmp8E1,sizeof(char),15))),((_tmp8E2->f2=0,((_tmp8E2->f3=((_tmp8E0="",_tag_dyneither(_tmp8E0,sizeof(char),1))),((_tmp8E2->f4=(void*)(
(_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=3,((_tmp8DF.f1=& Cyc_print_all_kinds_r,_tmp8DF)))),_tmp8DE)))),((_tmp8E2->f5=(
(_tmp8DD="Always print kinds of type variables",_tag_dyneither(_tmp8DD,sizeof(char),37))),_tmp8E2)))))))))))),((_tmp852[46]=(
# 1327
(_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8->f1=((_tmp8E7="-printalltvars",_tag_dyneither(_tmp8E7,sizeof(char),15))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E6="",_tag_dyneither(_tmp8E6,sizeof(char),1))),((_tmp8E8->f4=(void*)(
(_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=3,((_tmp8E5.f1=& Cyc_print_all_tvars_r,_tmp8E5)))),_tmp8E4)))),((_tmp8E8->f5=(
(_tmp8E3="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp8E3,sizeof(char),57))),_tmp8E8)))))))))))),((_tmp852[45]=(
# 1324
(_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE->f1=((_tmp8ED="-noexpandtypedefs",_tag_dyneither(_tmp8ED,sizeof(char),18))),((_tmp8EE->f2=0,((_tmp8EE->f3=((_tmp8EC="",_tag_dyneither(_tmp8EC,sizeof(char),1))),((_tmp8EE->f4=(void*)(
(_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=3,((_tmp8EB.f1=& Cyc_noexpand_r,_tmp8EB)))),_tmp8EA)))),((_tmp8EE->f5=(
(_tmp8E9="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp8E9,sizeof(char),41))),_tmp8EE)))))))))))),((_tmp852[44]=(
# 1321
(_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=((_tmp8F3="-noremoveunused",_tag_dyneither(_tmp8F3,sizeof(char),16))),((_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F2="",_tag_dyneither(_tmp8F2,sizeof(char),1))),((_tmp8F4->f4=(void*)(
(_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=3,((_tmp8F1.f1=& Cyc_noshake_r,_tmp8F1)))),_tmp8F0)))),((_tmp8F4->f5=(
(_tmp8EF="Don't remove externed variables that aren't used",_tag_dyneither(_tmp8EF,sizeof(char),49))),_tmp8F4)))))))))))),((_tmp852[43]=(
# 1318
(_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA->f1=((_tmp8F9="-nogc",_tag_dyneither(_tmp8F9,sizeof(char),6))),((_tmp8FA->f2=0,((_tmp8FA->f3=((_tmp8F8="",_tag_dyneither(_tmp8F8,sizeof(char),1))),((_tmp8FA->f4=(void*)(
(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=3,((_tmp8F7.f1=& Cyc_nogc_r,_tmp8F7)))),_tmp8F6)))),((_tmp8FA->f5=(
(_tmp8F5="Don't link in the garbage collector",_tag_dyneither(_tmp8F5,sizeof(char),36))),_tmp8FA)))))))))))),((_tmp852[42]=(
# 1315
(_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900->f1=((_tmp8FF="-nocyc",_tag_dyneither(_tmp8FF,sizeof(char),7))),((_tmp900->f2=0,((_tmp900->f3=((_tmp8FE="",_tag_dyneither(_tmp8FE,sizeof(char),1))),((_tmp900->f4=(void*)(
(_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=0,((_tmp8FD.f1=Cyc_set_nocyc,_tmp8FD)))),_tmp8FC)))),((_tmp900->f5=(
(_tmp8FB="Don't add implicit namespace Cyc",_tag_dyneither(_tmp8FB,sizeof(char),33))),_tmp900)))))))))))),((_tmp852[41]=(
# 1312
(_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp905="-no-cpp-precomp",_tag_dyneither(_tmp905,sizeof(char),16))),((_tmp906->f2=0,((_tmp906->f3=((_tmp904="",_tag_dyneither(_tmp904,sizeof(char),1))),((_tmp906->f4=(void*)(
(_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=0,((_tmp903.f1=Cyc_set_nocppprecomp,_tmp903)))),_tmp902)))),((_tmp906->f5=(
(_tmp901="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp901,sizeof(char),40))),_tmp906)))))))))))),((_tmp852[40]=(
# 1309
(_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C->f1=((_tmp90B="-use-cpp",_tag_dyneither(_tmp90B,sizeof(char),9))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp90A="<path>",_tag_dyneither(_tmp90A,sizeof(char),7))),((_tmp90C->f4=(void*)(
(_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=5,((_tmp909.f1=Cyc_set_cpp,_tmp909)))),_tmp908)))),((_tmp90C->f5=(
(_tmp907="Indicate which preprocessor to use",_tag_dyneither(_tmp907,sizeof(char),35))),_tmp90C)))))))))))),((_tmp852[39]=(
# 1306
(_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912->f1=((_tmp911="--inline-checks",_tag_dyneither(_tmp911,sizeof(char),16))),((_tmp912->f2=0,((_tmp912->f3=((_tmp910="",_tag_dyneither(_tmp910,sizeof(char),1))),((_tmp912->f4=(void*)(
(_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=0,((_tmp90F.f1=Cyc_set_inline_functions,_tmp90F)))),_tmp90E)))),((_tmp912->f5=(
(_tmp90D="Inline bounds checks instead of #define",_tag_dyneither(_tmp90D,sizeof(char),40))),_tmp912)))))))))))),((_tmp852[38]=(
# 1303
(_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp917="--noboundschecks",_tag_dyneither(_tmp917,sizeof(char),17))),((_tmp918->f2=0,((_tmp918->f3=((_tmp916="",_tag_dyneither(_tmp916,sizeof(char),1))),((_tmp918->f4=(void*)(
(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=0,((_tmp915.f1=Cyc_set_noboundschecks,_tmp915)))),_tmp914)))),((_tmp918->f5=(
(_tmp913="Disable bounds checks",_tag_dyneither(_tmp913,sizeof(char),22))),_tmp918)))))))))))),((_tmp852[37]=(
# 1300
(_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((_tmp91D="--nonullchecks",_tag_dyneither(_tmp91D,sizeof(char),15))),((_tmp91E->f2=0,((_tmp91E->f3=((_tmp91C="",_tag_dyneither(_tmp91C,sizeof(char),1))),((_tmp91E->f4=(void*)(
(_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=0,((_tmp91B.f1=Cyc_set_nonullchecks,_tmp91B)))),_tmp91A)))),((_tmp91E->f5=(
(_tmp919="Disable null checks",_tag_dyneither(_tmp919,sizeof(char),20))),_tmp91E)))))))))))),((_tmp852[36]=(
# 1297
(_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924->f1=((_tmp923="--nochecks",_tag_dyneither(_tmp923,sizeof(char),11))),((_tmp924->f2=0,((_tmp924->f3=((_tmp922="",_tag_dyneither(_tmp922,sizeof(char),1))),((_tmp924->f4=(void*)(
(_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=0,((_tmp921.f1=Cyc_set_nochecks,_tmp921)))),_tmp920)))),((_tmp924->f5=(
(_tmp91F="Disable bounds/null checks",_tag_dyneither(_tmp91F,sizeof(char),27))),_tmp924)))))))))))),((_tmp852[35]=(
# 1294
(_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->f1=((_tmp929="--nolineno",_tag_dyneither(_tmp929,sizeof(char),11))),((_tmp92A->f2=0,((_tmp92A->f3=((_tmp928="",_tag_dyneither(_tmp928,sizeof(char),1))),((_tmp92A->f4=(void*)(
(_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=0,((_tmp927.f1=Cyc_clear_lineno,_tmp927)))),_tmp926)))),((_tmp92A->f5=(
(_tmp925="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp925,sizeof(char),63))),_tmp92A)))))))))))),((_tmp852[34]=(
# 1291
(_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->f1=((_tmp92F="-save-c",_tag_dyneither(_tmp92F,sizeof(char),8))),((_tmp930->f2=0,((_tmp930->f3=((_tmp92E="",_tag_dyneither(_tmp92E,sizeof(char),1))),((_tmp930->f4=(void*)(
(_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92D.tag=3,((_tmp92D.f1=& Cyc_save_c_r,_tmp92D)))),_tmp92C)))),((_tmp930->f5=(
(_tmp92B="Don't delete temporary C files",_tag_dyneither(_tmp92B,sizeof(char),31))),_tmp930)))))))))))),((_tmp852[33]=(
# 1288
(_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->f1=((_tmp935="-save-temps",_tag_dyneither(_tmp935,sizeof(char),12))),((_tmp936->f2=0,((_tmp936->f3=((_tmp934="",_tag_dyneither(_tmp934,sizeof(char),1))),((_tmp936->f4=(void*)(
(_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=0,((_tmp933.f1=Cyc_set_save_temps,_tmp933)))),_tmp932)))),((_tmp936->f5=(
(_tmp931="Don't delete temporary files",_tag_dyneither(_tmp931,sizeof(char),29))),_tmp936)))))))))))),((_tmp852[32]=(
# 1285
(_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B="-c-comp",_tag_dyneither(_tmp93B,sizeof(char),8))),((_tmp93C->f2=0,((_tmp93C->f3=((_tmp93A=" <compiler>",_tag_dyneither(_tmp93A,sizeof(char),12))),((_tmp93C->f4=(void*)(
(_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=5,((_tmp939.f1=Cyc_set_c_compiler,_tmp939)))),_tmp938)))),((_tmp93C->f5=(
(_tmp937="Produce C output for the given compiler",_tag_dyneither(_tmp937,sizeof(char),40))),_tmp93C)))))))))))),((_tmp852[31]=(
# 1282
(_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942->f1=((_tmp941="-un-gcc",_tag_dyneither(_tmp941,sizeof(char),8))),((_tmp942->f2=0,((_tmp942->f3=((_tmp940="",_tag_dyneither(_tmp940,sizeof(char),1))),((_tmp942->f4=(void*)(
(_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=0,((_tmp93F.f1=Cyc_set_tovc,_tmp93F)))),_tmp93E)))),((_tmp942->f5=(
(_tmp93D="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp93D,sizeof(char),57))),_tmp942)))))))))))),((_tmp852[30]=(
# 1279
(_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948->f1=((_tmp947="-no-elim-statement-expressions",_tag_dyneither(_tmp947,sizeof(char),31))),((_tmp948->f2=0,((_tmp948->f3=((_tmp946="",_tag_dyneither(_tmp946,sizeof(char),1))),((_tmp948->f4=(void*)(
(_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=4,((_tmp945.f1=& Cyc_elim_se_r,_tmp945)))),_tmp944)))),((_tmp948->f5=(
(_tmp943="Do not avoid statement expressions in C output",_tag_dyneither(_tmp943,sizeof(char),47))),_tmp948)))))))))))),((_tmp852[29]=(
# 1275
(_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->f1=((_tmp94D="-elim-statement-expressions",_tag_dyneither(_tmp94D,sizeof(char),28))),((_tmp94E->f2=0,((_tmp94E->f3=((_tmp94C="",_tag_dyneither(_tmp94C,sizeof(char),1))),((_tmp94E->f4=(void*)(
(_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=3,((_tmp94B.f1=& Cyc_elim_se_r,_tmp94B)))),_tmp94A)))),((_tmp94E->f5=(
# 1278
(_tmp949="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp949,sizeof(char),66))),_tmp94E)))))))))))),((_tmp852[28]=(
# 1272
(_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->f1=((_tmp953="-up",_tag_dyneither(_tmp953,sizeof(char),4))),((_tmp954->f2=0,((_tmp954->f3=((_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),((_tmp954->f4=(void*)(
(_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=4,((_tmp951.f1=& Cyc_pp_r,_tmp951)))),_tmp950)))),((_tmp954->f5=(
(_tmp94F="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp94F,sizeof(char),55))),_tmp954)))))))))))),((_tmp852[27]=(
# 1269
(_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A->f1=((_tmp959="-pp",_tag_dyneither(_tmp959,sizeof(char),4))),((_tmp95A->f2=0,((_tmp95A->f3=((_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),((_tmp95A->f4=(void*)(
(_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=3,((_tmp957.f1=& Cyc_pp_r,_tmp957)))),_tmp956)))),((_tmp95A->f5=(
(_tmp955="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp955,sizeof(char),47))),_tmp95A)))))))))))),((_tmp852[26]=(
# 1266
(_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960->f1=((_tmp95F="-ic",_tag_dyneither(_tmp95F,sizeof(char),4))),((_tmp960->f2=0,((_tmp960->f3=((_tmp95E="",_tag_dyneither(_tmp95E,sizeof(char),1))),((_tmp960->f4=(void*)(
(_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95D.tag=3,((_tmp95D.f1=& Cyc_ic_r,_tmp95D)))),_tmp95C)))),((_tmp960->f5=(
(_tmp95B="Activate the link-checker",_tag_dyneither(_tmp95B,sizeof(char),26))),_tmp960)))))))))))),((_tmp852[25]=(
# 1263
(_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966->f1=((_tmp965="-stopafter-toc",_tag_dyneither(_tmp965,sizeof(char),15))),((_tmp966->f2=0,((_tmp966->f3=((_tmp964="",_tag_dyneither(_tmp964,sizeof(char),1))),((_tmp966->f4=(void*)(
(_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=3,((_tmp963.f1=& Cyc_toc_r,_tmp963)))),_tmp962)))),((_tmp966->f5=(
(_tmp961="Stop after translation to C",_tag_dyneither(_tmp961,sizeof(char),28))),_tmp966)))))))))))),((_tmp852[24]=(
# 1260
(_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=((_tmp96B="-stopafter-cf",_tag_dyneither(_tmp96B,sizeof(char),14))),((_tmp96C->f2=0,((_tmp96C->f3=((_tmp96A="",_tag_dyneither(_tmp96A,sizeof(char),1))),((_tmp96C->f4=(void*)(
(_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=3,((_tmp969.f1=& Cyc_cf_r,_tmp969)))),_tmp968)))),((_tmp96C->f5=(
(_tmp967="Stop after control-flow checking",_tag_dyneither(_tmp967,sizeof(char),33))),_tmp96C)))))))))))),((_tmp852[23]=(
# 1257
(_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972->f1=((_tmp971="-noprint",_tag_dyneither(_tmp971,sizeof(char),9))),((_tmp972->f2=0,((_tmp972->f3=((_tmp970="",_tag_dyneither(_tmp970,sizeof(char),1))),((_tmp972->f4=(void*)(
(_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=3,((_tmp96F.f1=& Cyc_noprint_r,_tmp96F)))),_tmp96E)))),((_tmp972->f5=(
(_tmp96D="Do not print output (when stopping early)",_tag_dyneither(_tmp96D,sizeof(char),42))),_tmp972)))))))))))),((_tmp852[22]=(
# 1254
(_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->f1=((_tmp977="-stopafter-tc",_tag_dyneither(_tmp977,sizeof(char),14))),((_tmp978->f2=0,((_tmp978->f3=((_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp978->f4=(void*)(
(_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=3,((_tmp975.f1=& Cyc_tc_r,_tmp975)))),_tmp974)))),((_tmp978->f5=(
(_tmp973="Stop after type checking",_tag_dyneither(_tmp973,sizeof(char),25))),_tmp978)))))))))))),((_tmp852[21]=(
# 1251
(_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E->f1=((_tmp97D="-stopafter-parse",_tag_dyneither(_tmp97D,sizeof(char),17))),((_tmp97E->f2=0,((_tmp97E->f3=((_tmp97C="",_tag_dyneither(_tmp97C,sizeof(char),1))),((_tmp97E->f4=(void*)(
(_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97B.tag=3,((_tmp97B.f1=& Cyc_parseonly_r,_tmp97B)))),_tmp97A)))),((_tmp97E->f5=(
(_tmp979="Stop after parsing",_tag_dyneither(_tmp979,sizeof(char),19))),_tmp97E)))))))))))),((_tmp852[20]=(
# 1248
(_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984->f1=((_tmp983="-E",_tag_dyneither(_tmp983,sizeof(char),3))),((_tmp984->f2=0,((_tmp984->f3=((_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),((_tmp984->f4=(void*)(
(_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=3,((_tmp981.f1=& Cyc_stop_after_cpp_r,_tmp981)))),_tmp980)))),((_tmp984->f5=(
(_tmp97F="Stop after preprocessing",_tag_dyneither(_tmp97F,sizeof(char),25))),_tmp984)))))))))))),((_tmp852[19]=(
# 1245
(_tmp98A=_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A->f1=((_tmp989="-Wall",_tag_dyneither(_tmp989,sizeof(char),6))),((_tmp98A->f2=0,((_tmp98A->f3=((_tmp988="",_tag_dyneither(_tmp988,sizeof(char),1))),((_tmp98A->f4=(void*)(
(_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=0,((_tmp987.f1=Cyc_set_all_warnings,_tmp987)))),_tmp986)))),((_tmp98A->f5=(
(_tmp985="Turn on all warnings",_tag_dyneither(_tmp985,sizeof(char),21))),_tmp98A)))))))))))),((_tmp852[18]=(
# 1242
(_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=((_tmp98F="-Woverride",_tag_dyneither(_tmp98F,sizeof(char),11))),((_tmp990->f2=0,((_tmp990->f3=((_tmp98E="",_tag_dyneither(_tmp98E,sizeof(char),1))),((_tmp990->f4=(void*)(
(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=3,((_tmp98D.f1=& Cyc_Tcenv_warn_override,_tmp98D)))),_tmp98C)))),((_tmp990->f5=(
(_tmp98B="Warn when a local variable overrides an existing variable",_tag_dyneither(_tmp98B,sizeof(char),58))),_tmp990)))))))))))),((_tmp852[17]=(
# 1239
(_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=((_tmp995="-Wlose-unique",_tag_dyneither(_tmp995,sizeof(char),14))),((_tmp996->f2=0,((_tmp996->f3=((_tmp994="",_tag_dyneither(_tmp994,sizeof(char),1))),((_tmp996->f4=(void*)(
(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=3,((_tmp993.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp993)))),_tmp992)))),((_tmp996->f5=(
(_tmp991="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmp991,sizeof(char),49))),_tmp996)))))))))))),((_tmp852[16]=(
# 1235
(_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->f1=((_tmp99B="-b",_tag_dyneither(_tmp99B,sizeof(char),3))),((_tmp99C->f2=0,((_tmp99C->f3=((_tmp99A="<machine>",_tag_dyneither(_tmp99A,sizeof(char),10))),((_tmp99C->f4=(void*)(
(_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=5,((_tmp999.f1=Cyc_set_target_arch,_tmp999)))),_tmp998)))),((_tmp99C->f5=(
# 1238
(_tmp997="Set target machine",_tag_dyneither(_tmp997,sizeof(char),19))),_tmp99C)))))))))))),((_tmp852[15]=(
# 1232
(_tmp9A2=_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2->f1=((_tmp9A1="-MT",_tag_dyneither(_tmp9A1,sizeof(char),4))),((_tmp9A2->f2=0,((_tmp9A2->f3=((_tmp9A0=" <target>",_tag_dyneither(_tmp9A0,sizeof(char),10))),((_tmp9A2->f4=(void*)(
(_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=5,((_tmp99F.f1=Cyc_set_dependencies_target,_tmp99F)))),_tmp99E)))),((_tmp9A2->f5=(
(_tmp99D="Give target for dependencies",_tag_dyneither(_tmp99D,sizeof(char),29))),_tmp9A2)))))))))))),((_tmp852[14]=(
# 1228
(_tmp9A8=_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8->f1=((_tmp9A7="-MG",_tag_dyneither(_tmp9A7,sizeof(char),4))),((_tmp9A8->f2=0,((_tmp9A8->f3=((_tmp9A6="",_tag_dyneither(_tmp9A6,sizeof(char),1))),((_tmp9A8->f4=(void*)(
(_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=1,((_tmp9A5.f1=Cyc_add_cpparg,_tmp9A5)))),_tmp9A4)))),((_tmp9A8->f5=(
# 1231
(_tmp9A3="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmp9A3,sizeof(char),68))),_tmp9A8)))))))))))),((_tmp852[13]=(
# 1225
(_tmp9AE=_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE->f1=((_tmp9AD="-M",_tag_dyneither(_tmp9AD,sizeof(char),3))),((_tmp9AE->f2=0,((_tmp9AE->f3=((_tmp9AC="",_tag_dyneither(_tmp9AC,sizeof(char),1))),((_tmp9AE->f4=(void*)(
(_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=0,((_tmp9AB.f1=Cyc_set_produce_dependencies,_tmp9AB)))),_tmp9AA)))),((_tmp9AE->f5=(
(_tmp9A9="Produce dependencies",_tag_dyneither(_tmp9A9,sizeof(char),21))),_tmp9AE)))))))))))),((_tmp852[12]=(
# 1222
(_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->f1=((_tmp9B3="-S",_tag_dyneither(_tmp9B3,sizeof(char),3))),((_tmp9B4->f2=0,((_tmp9B4->f3=((_tmp9B2="",_tag_dyneither(_tmp9B2,sizeof(char),1))),((_tmp9B4->f4=(void*)(
(_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=0,((_tmp9B1.f1=Cyc_set_stop_after_asmfile,_tmp9B1)))),_tmp9B0)))),((_tmp9B4->f5=(
(_tmp9AF="Stop after producing assembly code",_tag_dyneither(_tmp9AF,sizeof(char),35))),_tmp9B4)))))))))))),((_tmp852[11]=(
# 1219
(_tmp9BA=_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA->f1=((_tmp9B9="-pa",_tag_dyneither(_tmp9B9,sizeof(char),4))),((_tmp9BA->f2=0,((_tmp9BA->f3=((_tmp9B8="",_tag_dyneither(_tmp9B8,sizeof(char),1))),((_tmp9BA->f4=(void*)(
(_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=0,((_tmp9B7.f1=Cyc_set_pa,_tmp9B7)))),_tmp9B6)))),((_tmp9BA->f5=(
(_tmp9B5="Compile for profiling with aprof",_tag_dyneither(_tmp9B5,sizeof(char),33))),_tmp9BA)))))))))))),((_tmp852[10]=(
# 1216
(_tmp9C0=_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0->f1=((_tmp9BF="-x",_tag_dyneither(_tmp9BF,sizeof(char),3))),((_tmp9C0->f2=0,((_tmp9C0->f3=((_tmp9BE=" <language>",_tag_dyneither(_tmp9BE,sizeof(char),12))),((_tmp9C0->f4=(void*)(
(_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=5,((_tmp9BD.f1=Cyc_set_inputtype,_tmp9BD)))),_tmp9BC)))),((_tmp9C0->f5=(
(_tmp9BB="Specify <language> for the following input files",_tag_dyneither(_tmp9BB,sizeof(char),49))),_tmp9C0)))))))))))),((_tmp852[9]=(
# 1213
(_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->f1=((_tmp9C5="-c",_tag_dyneither(_tmp9C5,sizeof(char),3))),((_tmp9C6->f2=0,((_tmp9C6->f3=((_tmp9C4="",_tag_dyneither(_tmp9C4,sizeof(char),1))),((_tmp9C6->f4=(void*)(
(_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=0,((_tmp9C3.f1=Cyc_set_stop_after_objectfile,_tmp9C3)))),_tmp9C2)))),((_tmp9C6->f5=(
(_tmp9C1="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmp9C1,sizeof(char),61))),_tmp9C6)))))))))))),((_tmp852[8]=(
# 1210
(_tmp9CC=_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC->f1=((_tmp9CB="-m",_tag_dyneither(_tmp9CB,sizeof(char),3))),((_tmp9CC->f2=1,((_tmp9CC->f3=((_tmp9CA="<option>",_tag_dyneither(_tmp9CA,sizeof(char),9))),((_tmp9CC->f4=(void*)(
(_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=1,((_tmp9C9.f1=Cyc_add_marg,_tmp9C9)))),_tmp9C8)))),((_tmp9CC->f5=(
(_tmp9C7="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmp9C7,sizeof(char),52))),_tmp9CC)))))))))))),((_tmp852[7]=(
# 1207
(_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2->f1=((_tmp9D1="-l",_tag_dyneither(_tmp9D1,sizeof(char),3))),((_tmp9D2->f2=1,((_tmp9D2->f3=((_tmp9D0="<libname>",_tag_dyneither(_tmp9D0,sizeof(char),10))),((_tmp9D2->f4=(void*)(
(_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=1,((_tmp9CF.f1=Cyc_add_libarg,_tmp9CF)))),_tmp9CE)))),((_tmp9D2->f5=(
(_tmp9CD="Library file",_tag_dyneither(_tmp9CD,sizeof(char),13))),_tmp9D2)))))))))))),((_tmp852[6]=(
# 1204
(_tmp9D8=_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8->f1=((_tmp9D7="-L",_tag_dyneither(_tmp9D7,sizeof(char),3))),((_tmp9D8->f2=1,((_tmp9D8->f3=((_tmp9D6="<dir>",_tag_dyneither(_tmp9D6,sizeof(char),6))),((_tmp9D8->f4=(void*)(
(_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=1,((_tmp9D5.f1=Cyc_add_ccarg,_tmp9D5)))),_tmp9D4)))),((_tmp9D8->f5=(
(_tmp9D3="Add to the list of directories for -l",_tag_dyneither(_tmp9D3,sizeof(char),38))),_tmp9D8)))))))))))),((_tmp852[5]=(
# 1201
(_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->f1=((_tmp9DD="-I",_tag_dyneither(_tmp9DD,sizeof(char),3))),((_tmp9DE->f2=1,((_tmp9DE->f3=((_tmp9DC="<dir>",_tag_dyneither(_tmp9DC,sizeof(char),6))),((_tmp9DE->f4=(void*)(
(_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=1,((_tmp9DB.f1=Cyc_add_cpparg,_tmp9DB)))),_tmp9DA)))),((_tmp9DE->f5=(
(_tmp9D9="Add to the list of directories to search for include files",_tag_dyneither(_tmp9D9,sizeof(char),59))),_tmp9DE)))))))))))),((_tmp852[4]=(
# 1198
(_tmp9E4=_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4->f1=((_tmp9E3="-B",_tag_dyneither(_tmp9E3,sizeof(char),3))),((_tmp9E4->f2=1,((_tmp9E4->f3=((_tmp9E2="<file>",_tag_dyneither(_tmp9E2,sizeof(char),7))),((_tmp9E4->f4=(void*)(
(_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=1,((_tmp9E1.f1=Cyc_add_cyclone_exec_path,_tmp9E1)))),_tmp9E0)))),((_tmp9E4->f5=(
(_tmp9DF="Add to the list of directories to search for compiler files",_tag_dyneither(_tmp9DF,sizeof(char),60))),_tmp9E4)))))))))))),((_tmp852[3]=(
# 1195
(_tmp9EA=_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA->f1=((_tmp9E9="-D",_tag_dyneither(_tmp9E9,sizeof(char),3))),((_tmp9EA->f2=1,((_tmp9EA->f3=((_tmp9E8="<name>[=<value>]",_tag_dyneither(_tmp9E8,sizeof(char),17))),((_tmp9EA->f4=(void*)(
(_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=1,((_tmp9E7.f1=Cyc_add_cpparg,_tmp9E7)))),_tmp9E6)))),((_tmp9EA->f5=(
(_tmp9E5="Pass definition to preprocessor",_tag_dyneither(_tmp9E5,sizeof(char),32))),_tmp9EA)))))))))))),((_tmp852[2]=(
# 1192
(_tmp9F0=_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0->f1=((_tmp9EF="-o",_tag_dyneither(_tmp9EF,sizeof(char),3))),((_tmp9F0->f2=0,((_tmp9F0->f3=((_tmp9EE=" <file>",_tag_dyneither(_tmp9EE,sizeof(char),8))),((_tmp9F0->f4=(void*)(
(_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=5,((_tmp9ED.f1=Cyc_set_output_file,_tmp9ED)))),_tmp9EC)))),((_tmp9F0->f5=(
(_tmp9EB="Set the output file name to <file>",_tag_dyneither(_tmp9EB,sizeof(char),35))),_tmp9F0)))))))))))),((_tmp852[1]=(
# 1189
(_tmp9F6=_region_malloc(r,sizeof(*_tmp9F6)),((_tmp9F6->f1=((_tmp9F5="--version",_tag_dyneither(_tmp9F5,sizeof(char),10))),((_tmp9F6->f2=0,((_tmp9F6->f3=((_tmp9F4="",_tag_dyneither(_tmp9F4,sizeof(char),1))),((_tmp9F6->f4=(void*)(
(_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=0,((_tmp9F3.f1=Cyc_print_version,_tmp9F3)))),_tmp9F2)))),((_tmp9F6->f5=(
(_tmp9F1="Print version information and exit",_tag_dyneither(_tmp9F1,sizeof(char),35))),_tmp9F6)))))))))))),((_tmp852[0]=(
# 1186
(_tmp9FC=_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC->f1=((_tmp9FB="-v",_tag_dyneither(_tmp9FB,sizeof(char),3))),((_tmp9FC->f2=0,((_tmp9FC->f3=((_tmp9FA="",_tag_dyneither(_tmp9FA,sizeof(char),1))),((_tmp9FC->f4=(void*)(
(_tmp9F8=_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=3,((_tmp9F9.f1=& Cyc_v_r,_tmp9F9)))),_tmp9F8)))),((_tmp9FC->f5=(
(_tmp9F7="Print compilation stages verbosely",_tag_dyneither(_tmp9F7,sizeof(char),35))),_tmp9FC)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp852,sizeof(struct _tuple24*),71)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1404
const char*_tmp9FD;struct _dyneither_ptr optstring=(_tmp9FD="Options:",_tag_dyneither(_tmp9FD,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp171);
# 1409
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1417
char*_tmp9FE;struct _dyneither_ptr cyclone_exec_prefix=(_tmp9FE=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmp9FE,sizeof(char),_get_zero_arr_size_char((void*)_tmp9FE,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmpA01;struct Cyc_List_List*_tmpA00;Cyc_cyclone_exec_path=((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->hd=((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01[0]=cyclone_exec_prefix,_tmpA01)))),((_tmpA00->tl=Cyc_cyclone_exec_path,_tmpA00))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmpA07;const char*_tmpA06;struct Cyc_List_List*_tmpA05;Cyc_cyclone_exec_path=(
(_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->hd=((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA06="cyc-lib",_tag_dyneither(_tmpA06,sizeof(char),8)))),_tmpA07)))),((_tmpA05->tl=Cyc_cyclone_exec_path,_tmpA05))))));}
# 1425
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1433
const char*_tmpA08;struct _dyneither_ptr _tmp178=Cyc_do_find(cyclone_arch_path,((_tmpA08="cycspecs",_tag_dyneither(_tmpA08,sizeof(char),9))));
if(Cyc_v_r){const char*_tmpA0C;void*_tmpA0B[1];struct Cyc_String_pa_PrintArg_struct _tmpA0A;(_tmpA0A.tag=0,((_tmpA0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp178),((_tmpA0B[0]=& _tmpA0A,Cyc_fprintf(Cyc_stderr,((_tmpA0C="Reading from specs file %s\n",_tag_dyneither(_tmpA0C,sizeof(char),28))),_tag_dyneither(_tmpA0B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp17C=Cyc_read_specs(_tmp178);
struct _dyneither_ptr _tmp17D=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp17E;_push_handler(& _tmp17E);{int _tmp180=0;if(setjmp(_tmp17E.handler))_tmp180=1;if(!_tmp180){
{struct _dyneither_ptr _tmp181=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp17C,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp17D=Cyc_split_specs(_tmp181);}
# 1438
;_pop_handler();}else{void*_tmp17F=(void*)_exn_thrown;void*_tmp184=_tmp17F;void*_tmp186;_LL34: {struct Cyc_Core_Not_found_exn_struct*_tmp185=(struct Cyc_Core_Not_found_exn_struct*)_tmp184;if(_tmp185->tag != Cyc_Core_Not_found)goto _LL36;}_LL35:
# 1441
 goto _LL33;_LL36: _tmp186=_tmp184;_LL37:(void)_rethrow(_tmp186);_LL33:;}};}
# 1443
if((struct _dyneither_ptr*)_tmp17D.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp17D);}
# 1448
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp172);{
const char*_tmpA0D;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpA0D="",_tag_dyneither(_tmpA0D,sizeof(char),1))))== 0){
# 1466 "cyclone.cyc"
const char*_tmp188=Cyc_produce_dependencies?"":" -E";
const char*_tmpA15;void*_tmpA14[3];struct Cyc_String_pa_PrintArg_struct _tmpA13;const char*_tmpA12;struct Cyc_String_pa_PrintArg_struct _tmpA11;struct Cyc_String_pa_PrintArg_struct _tmpA10;Cyc_set_cpp((struct _dyneither_ptr)((_tmpA10.tag=0,((_tmpA10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp178),((_tmpA11.tag=0,((_tmpA11.f1=(struct _dyneither_ptr)((_tmpA12=_tmp188,_tag_dyneither(_tmpA12,sizeof(char),_get_zero_arr_size_char((void*)_tmpA12,1)))),((_tmpA13.tag=0,((_tmpA13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA14[0]=& _tmpA13,((_tmpA14[1]=& _tmpA11,((_tmpA14[2]=& _tmpA10,Cyc_aprintf(((_tmpA15="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA15,sizeof(char),23))),_tag_dyneither(_tmpA14,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1470
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpA18;void*_tmpA17;(_tmpA17=0,Cyc_fprintf(Cyc_stderr,((_tmpA18="missing file\n",_tag_dyneither(_tmpA18,sizeof(char),14))),_tag_dyneither(_tmpA17,sizeof(void*),0)));}
exit(1);}
# 1479
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpA19;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpA19="cyc_setjmp.h",_tag_dyneither(_tmpA19,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpA1A;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpA1A="cyc_include.h",_tag_dyneither(_tmpA1A,sizeof(char),14))));}
# 1485
{struct Cyc_List_List*_tmp34D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp34D != 0;_tmp34D=_tmp34D->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp34D->hd));
if(Cyc_compile_failure){int _tmp34E=1;_npop_handler(0);return _tmp34E;}}}
# 1490
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp34F=0;_npop_handler(0);return _tmp34F;}
# 1496
if(Cyc_ccargs == 0){int _tmp350=0;_npop_handler(0);return _tmp350;}
# 1499
{const char*_tmpA1B;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpA1B="-L",_tag_dyneither(_tmpA1B,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1501
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpA1C;struct _dyneither_ptr _tmp352=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpA1C=" ",_tag_dyneither(_tmpA1C,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpA1E;struct Cyc_List_List*_tmpA1D;struct _dyneither_ptr _tmp353=Cyc_str_sepstr(((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpA1D->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpA1D)))))),((_tmpA1E=" ",_tag_dyneither(_tmpA1E,sizeof(char),2))));
# 1507
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpA20;const char*_tmpA1F;int _tmp354=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1512
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA20=".a",_tag_dyneither(_tmpA20,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA1F=".lib",_tag_dyneither(_tmpA1F,sizeof(char),5))));
if(_tmp354){
stdlib=0;{
const char*_tmpA21;stdlib_string=(struct _dyneither_ptr)((_tmpA21="",_tag_dyneither(_tmpA21,sizeof(char),1)));};}else{
# 1520
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpA22;libcyc_flag=((_tmpA22="-lcyc_a",_tag_dyneither(_tmpA22,sizeof(char),8)));}
{const char*_tmpA23;nogc_filename=((_tmpA23="nogc_a.a",_tag_dyneither(_tmpA23,sizeof(char),9)));}{
const char*_tmpA24;runtime_filename=((_tmpA24="runtime_cyc_a.o",_tag_dyneither(_tmpA24,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpA25;libcyc_flag=((_tmpA25="-lcyc_nocheck",_tag_dyneither(_tmpA25,sizeof(char),14)));}
{const char*_tmpA26;nogc_filename=((_tmpA26="nogc.a",_tag_dyneither(_tmpA26,sizeof(char),7)));}{
const char*_tmpA27;runtime_filename=((_tmpA27="runtime_cyc.o",_tag_dyneither(_tmpA27,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpA28;libcyc_flag=((_tmpA28="-lcyc_pg",_tag_dyneither(_tmpA28,sizeof(char),9)));}
{const char*_tmpA29;runtime_filename=((_tmpA29="runtime_cyc_pg.o",_tag_dyneither(_tmpA29,sizeof(char),17)));}{
const char*_tmpA2A;nogc_filename=((_tmpA2A="nogc_pg.a",_tag_dyneither(_tmpA2A,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1538
{const char*_tmpA2B;libcyc_flag=((_tmpA2B="-lcycboot",_tag_dyneither(_tmpA2B,sizeof(char),10)));}
{const char*_tmpA2C;nogc_filename=((_tmpA2C="nogc.a",_tag_dyneither(_tmpA2C,sizeof(char),7)));}{
const char*_tmpA2D;runtime_filename=((_tmpA2D="runtime_cyc.o",_tag_dyneither(_tmpA2D,sizeof(char),14)));};}else{
# 1542
{const char*_tmpA2E;libcyc_flag=((_tmpA2E="-lcyc",_tag_dyneither(_tmpA2E,sizeof(char),6)));}
{const char*_tmpA2F;nogc_filename=((_tmpA2F="nogc.a",_tag_dyneither(_tmpA2F,sizeof(char),7)));}{
const char*_tmpA30;runtime_filename=((_tmpA30="runtime_cyc.o",_tag_dyneither(_tmpA30,sizeof(char),14)));};}}}}
# 1546
{const char*_tmpA31;gc_filename=((_tmpA31="gc.a",_tag_dyneither(_tmpA31,sizeof(char),5)));}{
# 1548
struct _dyneither_ptr _tmp366=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp367=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1555
stdlib=0;{
const char*_tmpA37;void*_tmpA36[3];struct Cyc_String_pa_PrintArg_struct _tmpA35;struct Cyc_String_pa_PrintArg_struct _tmpA34;struct Cyc_String_pa_PrintArg_struct _tmpA33;stdlib_string=(struct _dyneither_ptr)((_tmpA33.tag=0,((_tmpA33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp366),((_tmpA34.tag=0,((_tmpA34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpA35.tag=0,((_tmpA35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp367),((_tmpA36[0]=& _tmpA35,((_tmpA36[1]=& _tmpA34,((_tmpA36[2]=& _tmpA33,Cyc_aprintf(((_tmpA37=" %s %s %s",_tag_dyneither(_tmpA37,sizeof(char),10))),_tag_dyneither(_tmpA36,sizeof(void*),3))))))))))))))))))));};};}
# 1559
if(Cyc_ic_r){struct _handler_cons _tmp36D;_push_handler(& _tmp36D);{int _tmp36F=0;if(setjmp(_tmp36D.handler))_tmp36F=1;if(!_tmp36F){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp370=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp354){
struct Cyc_List_List*_tmpA38;_tmp370=((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38->hd=Cyc_final_strptr,((_tmpA38->tl=_tmp370,_tmpA38))))));}{
# 1567
struct Cyc_Interface_I*_tmp372=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp370,_tmp370);
if(_tmp372 == 0){
{const char*_tmpA3B;void*_tmpA3A;(_tmpA3A=0,Cyc_fprintf(Cyc_stderr,((_tmpA3B="Error: interfaces incompatible\n",_tag_dyneither(_tmpA3B,sizeof(char),32))),_tag_dyneither(_tmpA3A,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp375=1;_npop_handler(1);return _tmp375;};}
# 1574
if(_tmp354){
if(Cyc_output_file != 0){
const char*_tmpA3F;void*_tmpA3E[1];struct Cyc_String_pa_PrintArg_struct _tmpA3D;struct _dyneither_ptr _tmp376=(_tmpA3D.tag=0,((_tmpA3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA3E[0]=& _tmpA3D,Cyc_aprintf(((_tmpA3F="%s.cycio",_tag_dyneither(_tmpA3F,sizeof(char),9))),_tag_dyneither(_tmpA3E,sizeof(void*),1)))))));
const char*_tmpA41;const char*_tmpA40;struct Cyc___cycFILE*_tmp377=Cyc_try_file_open((struct _dyneither_ptr)_tmp376,((_tmpA40="wb",_tag_dyneither(_tmpA40,sizeof(char),3))),((_tmpA41="interface object file",_tag_dyneither(_tmpA41,sizeof(char),22))));
if(_tmp377 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp378=1;_npop_handler(1);return _tmp378;};}
# 1583
Cyc_Interface_save(_tmp372,_tmp377);
Cyc_file_close(_tmp377);}}else{
# 1587
const char*_tmpA46;const char*_tmpA45;struct _tuple20*_tmpA44;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp372,(
(_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->f1=((_tmpA45="empty interface",_tag_dyneither(_tmpA45,sizeof(char),16))),((_tmpA44->f2=((_tmpA46="global interface",_tag_dyneither(_tmpA46,sizeof(char),17))),_tmpA44)))))))){
{const char*_tmpA49;void*_tmpA48;(_tmpA48=0,Cyc_fprintf(Cyc_stderr,((_tmpA49="Error: some objects are still undefined\n",_tag_dyneither(_tmpA49,sizeof(char),41))),_tag_dyneither(_tmpA48,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp383=1;_npop_handler(1);return _tmp383;};}}};};
# 1560
;_pop_handler();}else{void*_tmp36E=(void*)_exn_thrown;void*_tmp385=_tmp36E;void*_tmp386;void*_tmp387;_LL39: _tmp386=_tmp385;_LL3A:
# 1597
{const char*_tmpA53;void*_tmpA52[3];const char*_tmpA51;struct Cyc_String_pa_PrintArg_struct _tmpA50;const char*_tmpA4F;struct Cyc_String_pa_PrintArg_struct _tmpA4E;struct Cyc_Int_pa_PrintArg_struct _tmpA4D;(_tmpA4D.tag=1,((_tmpA4D.f1=(unsigned long)Cyc_Core_get_exn_lineno(),((_tmpA4E.tag=0,((_tmpA4E.f1=(struct _dyneither_ptr)((_tmpA4F=Cyc_Core_get_exn_filename(),_tag_dyneither(_tmpA4F,sizeof(char),_get_zero_arr_size_char((void*)_tmpA4F,1)))),((_tmpA50.tag=0,((_tmpA50.f1=(struct _dyneither_ptr)((_tmpA51=Cyc_Core_get_exn_name(_tmp386),_tag_dyneither(_tmpA51,sizeof(char),_get_zero_arr_size_char((void*)_tmpA51,1)))),((_tmpA52[0]=& _tmpA50,((_tmpA52[1]=& _tmpA4E,((_tmpA52[2]=& _tmpA4D,Cyc_fprintf(Cyc_stderr,((_tmpA53="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",_tag_dyneither(_tmpA53,sizeof(char),131))),_tag_dyneither(_tmpA52,sizeof(void*),3)))))))))))))))))));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp38F=1;_npop_handler(0);return _tmp38F;};_LL3B: _tmp387=_tmp385;_LL3C:(void)_rethrow(_tmp387);_LL38:;}};}{
# 1603
const char*_tmpA54;struct _dyneither_ptr outfile_str=(_tmpA54="",_tag_dyneither(_tmpA54,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpA58;void*_tmpA57[1];struct Cyc_String_pa_PrintArg_struct _tmpA56;outfile_str=(struct _dyneither_ptr)((_tmpA56.tag=0,((_tmpA56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA57[0]=& _tmpA56,Cyc_aprintf(((_tmpA58=" -o %s",_tag_dyneither(_tmpA58,sizeof(char),7))),_tag_dyneither(_tmpA57,sizeof(void*),1))))))));}{
# 1607
const char*_tmpA60;void*_tmpA5F[5];struct Cyc_String_pa_PrintArg_struct _tmpA5E;struct Cyc_String_pa_PrintArg_struct _tmpA5D;struct Cyc_String_pa_PrintArg_struct _tmpA5C;struct Cyc_String_pa_PrintArg_struct _tmpA5B;struct Cyc_String_pa_PrintArg_struct _tmpA5A;struct _dyneither_ptr _tmp393=
(_tmpA5A.tag=0,((_tmpA5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp353),((_tmpA5B.tag=0,((_tmpA5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpA5C.tag=0,((_tmpA5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpA5D.tag=0,((_tmpA5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp352),((_tmpA5E.tag=0,((_tmpA5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA5F[0]=& _tmpA5E,((_tmpA5F[1]=& _tmpA5D,((_tmpA5F[2]=& _tmpA5C,((_tmpA5F[3]=& _tmpA5B,((_tmpA5F[4]=& _tmpA5A,Cyc_aprintf(((_tmpA60="%s %s%s %s%s",_tag_dyneither(_tmpA60,sizeof(char),13))),_tag_dyneither(_tmpA5F,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1610
if(Cyc_v_r){{const char*_tmpA64;void*_tmpA63[1];struct Cyc_String_pa_PrintArg_struct _tmpA62;(_tmpA62.tag=0,((_tmpA62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp393),((_tmpA63[0]=& _tmpA62,Cyc_fprintf(Cyc_stderr,((_tmpA64="%s\n",_tag_dyneither(_tmpA64,sizeof(char),4))),_tag_dyneither(_tmpA63,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp393,sizeof(char),1))!= 0){
{const char*_tmpA67;void*_tmpA66;(_tmpA66=0,Cyc_fprintf(Cyc_stderr,((_tmpA67="Error: C compiler failed\n",_tag_dyneither(_tmpA67,sizeof(char),26))),_tag_dyneither(_tmpA66,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp399=1;_npop_handler(0);return _tmp399;};}
# 1617
Cyc_remove_cfiles();{
# 1619
int _tmp39A=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp39A;};};};};};
# 1154 "cyclone.cyc"
;_pop_region(r);};}
