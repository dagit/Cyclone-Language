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
# 1216 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1218
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
struct _dyneither_ptr*_tmp399;struct Cyc_List_List*_tmp398;Cyc_ccargs=((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398->hd=((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=s,_tmp399)))),((_tmp398->tl=Cyc_ccargs,_tmp398))))));}
# 102
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp39A;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp39A="vc",_tag_dyneither(_tmp39A,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp39B;Cyc_add_ccarg(((_tmp39B="-DVC_C",_tag_dyneither(_tmp39B,sizeof(char),7))));};}else{
# 107
const char*_tmp39C;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp39C="gcc",_tag_dyneither(_tmp39C,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp39D;Cyc_add_ccarg(((_tmp39D="-DGCC_C",_tag_dyneither(_tmp39D,sizeof(char),8))));};}}}
# 115
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 123
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp39E;Cyc_output_file=((_tmp39E=_cycalloc(sizeof(*_tmp39E)),((_tmp39E[0]=s,_tmp39E))));}
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
struct _dyneither_ptr*_tmp3A1;struct Cyc_List_List*_tmp3A0;Cyc_cppargs=((_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0->hd=((_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((_tmp3A1[0]=s,_tmp3A1)))),((_tmp3A0->tl=Cyc_cppargs,_tmp3A0))))));}
# 154
static void Cyc_add_cpp_and_ccarg(struct _dyneither_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 159
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3A5;void*_tmp3A4[1];struct Cyc_String_pa_PrintArg_struct _tmp3A3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3A3.tag=0,((_tmp3A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3A4[0]=& _tmp3A3,Cyc_aprintf(((_tmp3A5="-iprefix %s",_tag_dyneither(_tmp3A5,sizeof(char),12))),_tag_dyneither(_tmp3A4,sizeof(void*),1)))))))));}
# 162
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3A9;void*_tmp3A8[1];struct Cyc_String_pa_PrintArg_struct _tmp3A7;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3A7.tag=0,((_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3A8[0]=& _tmp3A7,Cyc_aprintf(((_tmp3A9="-iwithprefix %s",_tag_dyneither(_tmp3A9,sizeof(char),16))),_tag_dyneither(_tmp3A8,sizeof(void*),1)))))))));}
# 165
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3AD;void*_tmp3AC[1];struct Cyc_String_pa_PrintArg_struct _tmp3AB;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3AB.tag=0,((_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3AC[0]=& _tmp3AB,Cyc_aprintf(((_tmp3AD="-iwithprefixbefore %s",_tag_dyneither(_tmp3AD,sizeof(char),22))),_tag_dyneither(_tmp3AC,sizeof(void*),1)))))))));}
# 168
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3B1;void*_tmp3B0[1];struct Cyc_String_pa_PrintArg_struct _tmp3AF;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3AF.tag=0,((_tmp3AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3B0[0]=& _tmp3AF,Cyc_aprintf(((_tmp3B1="-isystem %s",_tag_dyneither(_tmp3B1,sizeof(char),12))),_tag_dyneither(_tmp3B0,sizeof(void*),1)))))))));}
# 171
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3B5;void*_tmp3B4[1];struct Cyc_String_pa_PrintArg_struct _tmp3B3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3B3.tag=0,((_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3B4[0]=& _tmp3B3,Cyc_aprintf(((_tmp3B5="-idirafter %s",_tag_dyneither(_tmp3B5,sizeof(char),14))),_tag_dyneither(_tmp3B4,sizeof(void*),1)))))))));}
# 175
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 178
Cyc_target_arch=s;
{const char*_tmp3B6;Cyc_add_ccarg(((_tmp3B6="-b",_tag_dyneither(_tmp3B6,sizeof(char),3))));}
Cyc_add_ccarg(s);}
# 183
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}
# 188
static void Cyc_print_version(){
{const char*_tmp3BA;void*_tmp3B9[1];struct Cyc_String_pa_PrintArg_struct _tmp3B8;(_tmp3B8.tag=0,((_tmp3B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3B9[0]=& _tmp3B8,Cyc_printf(((_tmp3BA="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3BA,sizeof(char),34))),_tag_dyneither(_tmp3B9,sizeof(void*),1)))))));}
{const char*_tmp3BF;void*_tmp3BE[2];struct Cyc_String_pa_PrintArg_struct _tmp3BD;struct Cyc_String_pa_PrintArg_struct _tmp3BC;(_tmp3BC.tag=0,((_tmp3BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3BD.tag=0,((_tmp3BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3BE[0]=& _tmp3BD,((_tmp3BE[1]=& _tmp3BC,Cyc_printf(((_tmp3BF="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3BF,sizeof(char),39))),_tag_dyneither(_tmp3BE,sizeof(void*),2)))))))))))));}
{const char*_tmp3C3;void*_tmp3C2[1];struct Cyc_String_pa_PrintArg_struct _tmp3C1;(_tmp3C1.tag=0,((_tmp3C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3C2[0]=& _tmp3C1,Cyc_printf(((_tmp3C3="Standard library directory: %s\n",_tag_dyneither(_tmp3C3,sizeof(char),32))),_tag_dyneither(_tmp3C2,sizeof(void*),1)))))));}
{const char*_tmp3C7;void*_tmp3C6[1];struct Cyc_String_pa_PrintArg_struct _tmp3C5;(_tmp3C5.tag=0,((_tmp3C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp3C6[0]=& _tmp3C5,Cyc_printf(((_tmp3C7="Standard include directory: %s\n",_tag_dyneither(_tmp3C7,sizeof(char),32))),_tag_dyneither(_tmp3C6,sizeof(void*),1)))))));}
exit(0);}
# 197
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp27=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp27 <= 4)return 0;else{
const char*_tmp3C8;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp27 - 4)),((_tmp3C8=".cyc",_tag_dyneither(_tmp3C8,sizeof(char),5))))== 0;}}
# 203
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp29 <= 2)return;{
struct _dyneither_ptr _tmp2A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp29 - 2);
struct _dyneither_ptr*_tmp3CB;struct Cyc_List_List*_tmp3CA;Cyc_cyclone_exec_path=((_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA->hd=((_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB[0]=_tmp2A,_tmp3CB)))),((_tmp3CA->tl=Cyc_cyclone_exec_path,_tmp3CA))))));};}
# 216
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp3CC;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3CC="-lxml",_tag_dyneither(_tmp3CC,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 222
const char*_tmp3CD;Cyc_add_ccarg(((_tmp3CD="-lxml_a",_tag_dyneither(_tmp3CD,sizeof(char),8))));}}else{
# 224
struct _dyneither_ptr*_tmp3D0;struct Cyc_List_List*_tmp3CF;Cyc_libargs=((_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF->hd=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=s,_tmp3D0)))),((_tmp3CF->tl=Cyc_libargs,_tmp3CF))))));}}
# 227
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 231
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp3D1;Cyc_add_ccarg(((_tmp3D1="-save-temps",_tag_dyneither(_tmp3D1,sizeof(char),12))));};}
# 236
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp3D2;Cyc_add_cpparg(((_tmp3D2="-M",_tag_dyneither(_tmp3D2,sizeof(char),3))));};}
# 243
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3D3;Cyc_dependencies_target=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3[0]=s,_tmp3D3))));}
# 248
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp3D4;Cyc_add_ccarg(((_tmp3D4="-c",_tag_dyneither(_tmp3D4,sizeof(char),3))));};}
# 253
static void Cyc_set_nocppprecomp(){
const char*_tmp3D5;Cyc_add_cpp_and_ccarg(((_tmp3D5="-no-cpp-precomp",_tag_dyneither(_tmp3D5,sizeof(char),16))));}
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
{const char*_tmp3D6;Cyc_add_ccarg(((_tmp3D6="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3D6,sizeof(char),18))));}
Cyc_set_elim_se();}
# 273
static void Cyc_set_noboundschecks(){
const char*_tmp3D7;Cyc_add_ccarg(((_tmp3D7="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp3D7,sizeof(char),23))));}
# 276
static void Cyc_set_nonullchecks(){
const char*_tmp3D8;Cyc_add_ccarg(((_tmp3D8="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp3D8,sizeof(char),21))));}
# 279
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 285
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp3D9;Cyc_add_ccarg(((_tmp3D9="-DNO_CYC_PREFIX",_tag_dyneither(_tmp3D9,sizeof(char),16))));};}
# 290
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp3DA;Cyc_add_ccarg(((_tmp3DA="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3DA,sizeof(char),21))));}{
const char*_tmp3DB;Cyc_add_cpparg(((_tmp3DB="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3DB,sizeof(char),21))));};}
# 296
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp3DC;Cyc_add_ccarg(((_tmp3DC="-pg",_tag_dyneither(_tmp3DC,sizeof(char),4))));};}
# 301
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp3DD;Cyc_add_ccarg(((_tmp3DD="-S",_tag_dyneither(_tmp3DD,sizeof(char),3))));};}
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
const char*_tmp3DE;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3DE="cyc",_tag_dyneither(_tmp3DE,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp3DF;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3DF="none",_tag_dyneither(_tmp3DF,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 328
const char*_tmp3E3;void*_tmp3E2[1];struct Cyc_String_pa_PrintArg_struct _tmp3E1;(_tmp3E1.tag=0,((_tmp3E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E2[0]=& _tmp3E1,Cyc_fprintf(Cyc_stderr,((_tmp3E3="Input type '%s' not supported\n",_tag_dyneither(_tmp3E3,sizeof(char),31))),_tag_dyneither(_tmp3E2,sizeof(void*),1)))))));}}}
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
{struct _dyneither_ptr*_tmp3E6;struct Cyc_List_List*_tmp3E5;Cyc_cyclone_files=((_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5->hd=((_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6[0]=s,_tmp3E6)))),((_tmp3E5->tl=Cyc_cyclone_files,_tmp3E5))))));}{
# 358
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp3E7;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp3E7=".c",_tag_dyneither(_tmp3E7,sizeof(char),3))));
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
{const char*_tmp3EB;void*_tmp3EA[1];struct Cyc_String_pa_PrintArg_struct _tmp3E9;(_tmp3E9.tag=0,((_tmp3E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3EA[0]=& _tmp3E9,Cyc_fprintf(Cyc_stderr,((_tmp3EB="Error opening spec file %s\n",_tag_dyneither(_tmp3EB,sizeof(char),28))),_tag_dyneither(_tmp3EA,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 426
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp3F0;void*_tmp3EF[2];struct Cyc_String_pa_PrintArg_struct _tmp3EE;struct Cyc_Int_pa_PrintArg_struct _tmp3ED;(_tmp3ED.tag=1,((_tmp3ED.f1=(unsigned long)c,((_tmp3EE.tag=0,((_tmp3EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3EF[0]=& _tmp3EE,((_tmp3EF[1]=& _tmp3ED,Cyc_fprintf(Cyc_stderr,((_tmp3F0="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp3F0,sizeof(char),55))),_tag_dyneither(_tmp3EF,sizeof(void*),2)))))))))))));}
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
{const char*_tmp3F4;void*_tmp3F3[1];struct Cyc_String_pa_PrintArg_struct _tmp3F2;(_tmp3F2.tag=0,((_tmp3F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3F3[0]=& _tmp3F2,Cyc_fprintf(Cyc_stderr,((_tmp3F4="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp3F4,sizeof(char),44))),_tag_dyneither(_tmp3F3,sizeof(void*),1)))))));}
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
{const char*_tmp3F8;void*_tmp3F7[1];struct Cyc_String_pa_PrintArg_struct _tmp3F6;(_tmp3F6.tag=0,((_tmp3F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3F7[0]=& _tmp3F6,Cyc_fprintf(Cyc_stderr,((_tmp3F8="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp3F8,sizeof(char),50))),_tag_dyneither(_tmp3F7,sizeof(void*),1)))))));}
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
{struct _tuple21*_tmp405;struct _dyneither_ptr*_tmp404;const char*_tmp403;struct _dyneither_ptr*_tmp402;struct Cyc_List_List*_tmp401;_tmp56=((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->hd=((_tmp405=_cycalloc(sizeof(*_tmp405)),((_tmp405->f1=((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp404)))),((_tmp405->f2=(
(_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp403="",_tag_dyneither(_tmp403,sizeof(char),1)))),_tmp402)))),_tmp405)))))),((_tmp401->tl=_tmp56,_tmp401))))));}
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
{const char*_tmp40A;void*_tmp409[2];struct Cyc_String_pa_PrintArg_struct _tmp408;struct Cyc_String_pa_PrintArg_struct _tmp407;(_tmp407.tag=0,((_tmp407.f1=(struct _dyneither_ptr)
# 486
_tag_dyneither(strname,sizeof(char),256),((_tmp408.tag=0,((_tmp408.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp409[0]=& _tmp408,((_tmp409[1]=& _tmp407,Cyc_fprintf(Cyc_stderr,((_tmp40A="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp40A,sizeof(char),50))),_tag_dyneither(_tmp409,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 492
{struct _tuple21*_tmp413;struct _dyneither_ptr*_tmp412;struct _dyneither_ptr*_tmp411;struct Cyc_List_List*_tmp410;_tmp56=((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->f1=((_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp412)))),((_tmp413->f2=(
(_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp411)))),_tmp413)))))),((_tmp410->tl=_tmp56,_tmp410))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 497
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp56;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp41D(struct _dyneither_ptr*_tmp77,unsigned int*_tmp41C,unsigned int*_tmp41B,struct _dyneither_ptr**_tmp419){for(*_tmp41C=0;*_tmp41C < *_tmp41B;(*_tmp41C)++){(*_tmp419)[*_tmp41C]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)(*_tmp77).curr)[(int)*_tmp41C];}}
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
struct _dyneither_ptr*_tmp416;struct Cyc_List_List*_tmp415;_tmp72=((_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415->hd=((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp416)))),((_tmp415->tl=_tmp72,_tmp415))))));};}
# 547
DONE:
 _tmp72=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp72);
{struct Cyc_List_List*_tmp417;_tmp72=((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp417->tl=_tmp72,_tmp417))))));}{
struct _dyneither_ptr _tmp77=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp72);
unsigned int _tmp41C;unsigned int _tmp41B;struct _dyneither_ptr _tmp41A;struct _dyneither_ptr*_tmp419;unsigned int _tmp418;struct _dyneither_ptr _tmp78=(_tmp418=_get_dyneither_size(_tmp77,sizeof(struct _dyneither_ptr*)),((_tmp419=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp418)),((_tmp41A=_tag_dyneither(_tmp419,sizeof(struct _dyneither_ptr),_tmp418),((((_tmp41B=_tmp418,_tmp41D(& _tmp77,& _tmp41C,& _tmp41B,& _tmp419))),_tmp41A)))))));
return _tmp78;};};}
# 557
int Cyc_compile_failure=0;
# 559
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp7D;_push_handler(& _tmp7D);{int _tmp7F=0;if(setjmp(_tmp7D.handler))_tmp7F=1;if(!_tmp7F){{struct Cyc___cycFILE*_tmp80=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp80;};_pop_handler();}else{void*_tmp7E=(void*)_exn_thrown;void*_tmp81=_tmp7E;_LL1: _LL2:
# 563
 Cyc_compile_failure=1;
{const char*_tmp422;void*_tmp421[2];struct Cyc_String_pa_PrintArg_struct _tmp420;struct Cyc_String_pa_PrintArg_struct _tmp41F;(_tmp41F.tag=0,((_tmp41F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp420.tag=0,((_tmp420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp421[0]=& _tmp420,((_tmp421[1]=& _tmp41F,Cyc_fprintf(Cyc_stderr,((_tmp422="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp422,sizeof(char),29))),_tag_dyneither(_tmp421,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL0:;}};}
# 570
void CYCALLOCPROFILE_mark(const char*s);
# 572
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 576
const char*_tmp423;struct _dyneither_ptr explain_string=(_tmp423="",_tag_dyneither(_tmp423,sizeof(char),1));
struct Cyc_List_List*_tmp86=0;
# 579
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 581
{struct _handler_cons _tmp87;_push_handler(& _tmp87);{int _tmp89=0;if(setjmp(_tmp87.handler))_tmp89=1;if(!_tmp89){_tmp86=f(env,tds);;_pop_handler();}else{void*_tmp88=(void*)_exn_thrown;void*_tmp8A=_tmp88;void*_tmp8B;_LL6: _tmp8B=_tmp8A;_LL7:
# 584
{const char*_tmp427;void*_tmp426[1];struct Cyc_String_pa_PrintArg_struct _tmp425;(_tmp425.tag=0,((_tmp425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp426[0]=& _tmp425,Cyc_fprintf(Cyc_stderr,((_tmp427="COMPILER STAGE %s\n",_tag_dyneither(_tmp427,sizeof(char),19))),_tag_dyneither(_tmp426,sizeof(void*),1)))))));}
Cyc_Core_rethrow(_tmp8B);_LL5:;}};}
# 587
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
{const char*_tmp42A;void*_tmp429;(_tmp429=0,Cyc_fprintf(Cyc_stderr,((_tmp42A="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp42A,sizeof(char),22))),_tag_dyneither(_tmp429,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
return _tmp86;}else{
# 594
if(Cyc_v_r){
{const char*_tmp42E;void*_tmp42D[1];struct Cyc_String_pa_PrintArg_struct _tmp42C;(_tmp42C.tag=0,((_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp42D[0]=& _tmp42C,Cyc_fprintf(Cyc_stderr,((_tmp42E="%s completed.\n",_tag_dyneither(_tmp42E,sizeof(char),15))),_tag_dyneither(_tmp42D,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return _tmp86;}}
# 599
return _tmp86;}
# 602
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp95;_push_handler(& _tmp95);{int _tmp97=0;if(setjmp(_tmp95.handler))_tmp97=1;if(!_tmp97){
ans=Cyc_Parse_parse_file(f);;_pop_handler();}else{void*_tmp96=(void*)_exn_thrown;void*_tmp98=_tmp96;void*_tmp99;if(((struct Cyc_Position_Exit_exn_struct*)_tmp98)->tag == Cyc_Position_Exit){_LLB: _LLC:
# 609
 Cyc_compile_failure=1;goto _LLA;}else{_LLD: _tmp99=_tmp98;_LLE:
 Cyc_Core_rethrow(_tmp99);}_LLA:;}};}
# 612
Cyc_Lex_lex_init(1);
return ans;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 616
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 618
struct _tuple22 _tmp9A=*tcenv;struct _tuple22 _tmp9B=_tmp9A;struct _RegionHandle*_tmp9C;struct Cyc_Tcenv_Tenv*_tmp9D;_LL12: _tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;_LL13:;
Cyc_Tc_tc(_tmp9C,_tmp9D,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmp9D,tds);
return tds;}
# 625
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 632
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct _tuple23*_tmp9E=params;struct Cyc_Tcenv_Tenv*_tmp9F;struct Cyc___cycFILE*_tmpA0;struct Cyc___cycFILE*_tmpA1;_LL15: _tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;_tmpA1=_tmp9E->f3;_LL16:;{
struct Cyc_Interface_I*_tmpA2=Cyc_Interface_extract(_tmp9F->ae);
if(_tmpA0 == 0)
Cyc_Interface_save(_tmpA2,_tmpA1);else{
# 638
struct Cyc_Interface_I*_tmpA3=Cyc_Interface_parse(_tmpA0);
const char*_tmp433;const char*_tmp432;struct _tuple20*_tmp431;if(!Cyc_Interface_is_subinterface(_tmpA3,_tmpA2,((_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431->f1=((_tmp432="written interface",_tag_dyneither(_tmp432,sizeof(char),18))),((_tmp431->f2=((_tmp433="maximal interface",_tag_dyneither(_tmp433,sizeof(char),18))),_tmp431))))))))
Cyc_compile_failure=1;else{
# 642
Cyc_Interface_save(_tmpA3,_tmpA1);}}
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
{const char*_tmp436;void*_tmp435;(_tmp435=0,Cyc_fprintf(Cyc_stderr,((_tmp436="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp436,sizeof(char),53))),_tag_dyneither(_tmp435,sizeof(void*),0)));}
return 1;}{
# 671
const char*_tmp438;const char*_tmp437;struct Cyc___cycFILE*_tmpA9=Cyc_try_file_open(file,((_tmp437="r",_tag_dyneither(_tmp437,sizeof(char),2))),((_tmp438="internal compiler file",_tag_dyneither(_tmp438,sizeof(char),23))));
if(_tmpA9 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpB0=1024;unsigned int i;for(i=0;i < _tmpB0;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpA9);
if(n_read != 1024  && !Cyc_feof(_tmpA9)){
Cyc_compile_failure=1;
{const char*_tmp43C;void*_tmp43B[1];struct Cyc_String_pa_PrintArg_struct _tmp43A;(_tmp43A.tag=0,((_tmp43A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp43B[0]=& _tmp43A,Cyc_fprintf(Cyc_stderr,((_tmp43C="\nError: problem copying %s\n",_tag_dyneither(_tmp43C,sizeof(char),28))),_tag_dyneither(_tmp43B,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 684
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp440;void*_tmp43F[1];struct Cyc_String_pa_PrintArg_struct _tmp43E;(_tmp43E.tag=0,((_tmp43E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp43F[0]=& _tmp43E,Cyc_fprintf(Cyc_stderr,((_tmp440="\nError: problem copying %s\n",_tag_dyneither(_tmp440,sizeof(char),28))),_tag_dyneither(_tmp43F,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 692
Cyc_fclose(_tmpA9);
return 0;};};}
# 697
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpB3=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpB3.expand_typedefs=!Cyc_noexpand_r;
_tmpB3.to_VC=Cyc_Cyclone_tovc_r;
_tmpB3.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpB3.generate_line_directives=Cyc_generate_line_directives_r;
_tmpB3.print_full_evars=Cyc_print_full_evars_r;
_tmpB3.print_all_tvars=Cyc_print_all_tvars_r;
_tmpB3.print_all_kinds=Cyc_print_all_kinds_r;
_tmpB3.print_all_effects=Cyc_print_all_effects_r;
# 708
if(Cyc_inline_functions_r){
const char*_tmp443;void*_tmp442;(_tmp442=0,Cyc_fprintf(out_file,((_tmp443="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp443,sizeof(char),27))),_tag_dyneither(_tmp442,sizeof(void*),0)));}
# 712
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r){
const char*_tmp446;void*_tmp445;(_tmp445=0,Cyc_fprintf(out_file,((_tmp446="#include <setjmp.h>\n",_tag_dyneither(_tmp446,sizeof(char),21))),_tag_dyneither(_tmp445,sizeof(void*),0)));}else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 718
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 720
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpB3);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 724
Cyc_Absyndump_set_params(& _tmpB3);
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
struct Cyc_List_List*_tmpB8=0;
unsigned long _tmpB9=Cyc_strlen((struct _dyneither_ptr)s);
# 746
while(_tmpB9 > 0){
struct _dyneither_ptr _tmpBA=Cyc_strchr(s,c);
if((char*)_tmpBA.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp449;struct Cyc_List_List*_tmp448;_tmpB8=((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448->hd=((_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449[0]=s,_tmp449)))),((_tmp448->tl=_tmpB8,_tmp448))))));}
break;}else{
# 753
{struct _dyneither_ptr*_tmp44C;struct Cyc_List_List*_tmp44B;_tmpB8=((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B->hd=((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpBA).curr - s.curr)/ sizeof(char))),_tmp44C)))),((_tmp44B->tl=_tmpB8,_tmp44B))))));}
_tmpB9 -=(((struct _dyneither_ptr)_tmpBA).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpBA,sizeof(char),1);}}
# 758
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB8);};}
# 762
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpBF;_push_handler(& _tmpBF);{int _tmpC1=0;if(setjmp(_tmpBF.handler))_tmpC1=1;if(!_tmpC1){{const char*_tmp44D;f=Cyc_file_open(file,((_tmp44D="r",_tag_dyneither(_tmp44D,sizeof(char),2))));};_pop_handler();}else{void*_tmpC0=(void*)_exn_thrown;void*_tmpC3=_tmpC0;_LL18: _LL19: goto _LL17;_LL17:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 770
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp44E;struct _dyneither_ptr tmp=(_tmp44E="",_tag_dyneither(_tmp44E,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC4=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC4.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC4)== 0)continue;
{const char*_tmp44F;_tmpC4=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC4,((_tmp44F=":",_tag_dyneither(_tmp44F,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC4,(struct _dyneither_ptr)tmp);}
# 778
return tmp;}
# 783
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC7=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC7.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC7)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpC7,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp450;return(_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450[0]=s,_tmp450)));}};}
# 791
return 0;}
# 794
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpC9=Cyc_find(dirs,file);
if(_tmpC9 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp455;void*_tmp454[2];struct Cyc_String_pa_PrintArg_struct _tmp453;struct Cyc_String_pa_PrintArg_struct _tmp452;(_tmp452.tag=0,((_tmp452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp453.tag=0,((_tmp453.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp454[0]=& _tmp453,((_tmp454[1]=& _tmp452,Cyc_fprintf(Cyc_stderr,((_tmp455="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp455,sizeof(char),56))),_tag_dyneither(_tmp454,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp465;const char*_tmp464;void*_tmp463[2];struct Cyc_String_pa_PrintArg_struct _tmp462;struct Cyc_String_pa_PrintArg_struct _tmp461;struct Cyc_Core_Failure_exn_struct*_tmp460;(int)_throw((void*)((_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460[0]=((_tmp465.tag=Cyc_Core_Failure,((_tmp465.f1=(struct _dyneither_ptr)((_tmp461.tag=0,((_tmp461.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp462.tag=0,((_tmp462.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp463[0]=& _tmp462,((_tmp463[1]=& _tmp461,Cyc_aprintf(((_tmp464="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp464,sizeof(char),56))),_tag_dyneither(_tmp463,sizeof(void*),2)))))))))))))),_tmp465)))),_tmp460)))));};}
# 806
return*_tmpC9;}
# 811
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 813
struct Cyc_List_List*_tmpD4=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp468;struct Cyc_List_List*_tmp467;_tmpD4=((_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467->hd=((_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp468)))),((_tmp467->tl=_tmpD4,_tmp467))))));}
# 817
_tmpD4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);
return _tmpD4;}
# 823
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 825
struct Cyc_List_List*_tmpD7=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp469;_tmpD7=((_tmp469=_cycalloc(sizeof(*_tmp469)),((_tmp469->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp469->tl=_tmpD7,_tmp469))))));}{
struct _dyneither_ptr*_tmp46C;struct Cyc_List_List*_tmp46B;_tmpD7=((_tmp46B=_cycalloc(sizeof(*_tmp46B)),((_tmp46B->hd=((_tmp46C=_cycalloc(sizeof(*_tmp46C)),((_tmp46C[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp46C)))),((_tmp46B->tl=_tmpD7,_tmp46B))))));};}
# 830
_tmpD7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD7);
return _tmpD7;}
# 835
static int Cyc_is_other_special(char c){
char _tmpDB=c;switch(_tmpDB){case 92: _LL1D: _LL1E:
 goto _LL20;case 34: _LL1F: _LL20:
 goto _LL22;case 59: _LL21: _LL22:
 goto _LL24;case 38: _LL23: _LL24:
 goto _LL26;case 40: _LL25: _LL26:
 goto _LL28;case 41: _LL27: _LL28:
 goto _LL2A;case 124: _LL29: _LL2A:
 goto _LL2C;case 94: _LL2B: _LL2C:
 goto _LL2E;case 60: _LL2D: _LL2E:
 goto _LL30;case 62: _LL2F: _LL30:
 goto _LL32;case 10: _LL31: _LL32:
# 850
 goto _LL34;case 9: _LL33: _LL34:
 return 1;default: _LL35: _LL36:
 return 0;}_LL1C:;}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp475(unsigned int*_tmp474,unsigned int*_tmp473,char**_tmp471){for(*_tmp474=0;*_tmp474 < *_tmp473;(*_tmp474)++){(*_tmp471)[*_tmp474]=(char)'\000';}}
# 857
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpDC=Cyc_strlen((struct _dyneither_ptr)s);
# 861
int _tmpDD=0;
int _tmpDE=0;
{int i=0;for(0;i < _tmpDC;++ i){
char _tmpDF=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpDF == '\'')++ _tmpDD;else{
if(Cyc_is_other_special(_tmpDF))++ _tmpDE;}}}
# 870
if(_tmpDD == 0  && _tmpDE == 0)
return s;
# 874
if(_tmpDD == 0){
struct _dyneither_ptr*_tmp46F;struct _dyneither_ptr*_tmp46E[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp46E[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp46E[1]=((_tmp46F=_cycalloc(sizeof(*_tmp46F)),((_tmp46F[0]=(struct _dyneither_ptr)s,_tmp46F)))),((_tmp46E[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46E,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 878
unsigned long _tmpE4=(_tmpDC + _tmpDD)+ _tmpDE;
unsigned int _tmp474;unsigned int _tmp473;struct _dyneither_ptr _tmp472;char*_tmp471;unsigned int _tmp470;struct _dyneither_ptr s2=(_tmp470=_tmpE4 + 1,((_tmp471=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp470 + 1)),((_tmp472=_tag_dyneither(_tmp471,sizeof(char),_tmp470 + 1),((((_tmp473=_tmp470,((_tmp475(& _tmp474,& _tmp473,& _tmp471),_tmp471[_tmp473]=(char)0)))),_tmp472)))))));
int _tmpE5=0;
int _tmpE6=0;
for(0;_tmpE5 < _tmpDC;++ _tmpE5){
char _tmpE7=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpE5));
if(_tmpE7 == '\''  || Cyc_is_other_special(_tmpE7)){
char _tmp478;char _tmp477;struct _dyneither_ptr _tmp476;(_tmp476=_dyneither_ptr_plus(s2,sizeof(char),_tmpE6 ++),((_tmp477=*((char*)_check_dyneither_subscript(_tmp476,sizeof(char),0)),((_tmp478='\\',((_get_dyneither_size(_tmp476,sizeof(char))== 1  && (_tmp477 == '\000'  && _tmp478 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp476.curr)=_tmp478)))))));}{
char _tmp47B;char _tmp47A;struct _dyneither_ptr _tmp479;(_tmp479=_dyneither_ptr_plus(s2,sizeof(char),_tmpE6 ++),((_tmp47A=*((char*)_check_dyneither_subscript(_tmp479,sizeof(char),0)),((_tmp47B=_tmpE7,((_get_dyneither_size(_tmp479,sizeof(char))== 1  && (_tmp47A == '\000'  && _tmp47B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp479.curr)=_tmp47B)))))));};}
# 888
return(struct _dyneither_ptr)s2;};}
# 890
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp47C;return(_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C[0]=Cyc_sh_escape_string(*sp),_tmp47C)));}
# 894
static void Cyc_process_file(struct _dyneither_ptr filename){
# 896
struct _dyneither_ptr _tmpF3=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp47D;struct _dyneither_ptr _tmpF4=Cyc_strconcat((struct _dyneither_ptr)_tmpF3,((_tmp47D=".cyp",_tag_dyneither(_tmp47D,sizeof(char),5))));
const char*_tmp47E;struct _dyneither_ptr _tmpF5=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpF3,((_tmp47E=".cyci",_tag_dyneither(_tmp47E,sizeof(char),6))));
const char*_tmp47F;struct _dyneither_ptr _tmpF6=Cyc_strconcat((struct _dyneither_ptr)_tmpF3,((_tmp47F=".cycio",_tag_dyneither(_tmp47F,sizeof(char),7))));
const char*_tmp480;struct _dyneither_ptr _tmpF7=Cyc_strconcat((struct _dyneither_ptr)_tmpF3,((_tmp480=".c",_tag_dyneither(_tmp480,sizeof(char),3))));
# 903
if(Cyc_v_r){const char*_tmp484;void*_tmp483[1];struct Cyc_String_pa_PrintArg_struct _tmp482;(_tmp482.tag=0,((_tmp482.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp483[0]=& _tmp482,Cyc_fprintf(Cyc_stderr,((_tmp484="Compiling %s\n",_tag_dyneither(_tmp484,sizeof(char),14))),_tag_dyneither(_tmp483,sizeof(void*),1)))))));}{
# 906
const char*_tmp486;const char*_tmp485;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp485="r",_tag_dyneither(_tmp485,sizeof(char),2))),((_tmp486="input file",_tag_dyneither(_tmp486,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 913
const char*_tmp48D;struct _dyneither_ptr*_tmp48C;const char*_tmp48B;struct Cyc_List_List*_tmp48A;struct _dyneither_ptr _tmpFB=
Cyc_str_sepstr(((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A->hd=((_tmp48C=_cycalloc(sizeof(*_tmp48C)),((_tmp48C[0]=(struct _dyneither_ptr)((_tmp48B="",_tag_dyneither(_tmp48B,sizeof(char),1))),_tmp48C)))),((_tmp48A->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp48A)))))),(
(_tmp48D=" ",_tag_dyneither(_tmp48D,sizeof(char),2))));
# 923
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpFC=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp48E;_tmpFC=Cyc_add_subdir(_tmpFC,((_tmp48E="include",_tag_dyneither(_tmp48E,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp491;struct Cyc_List_List*_tmp490;_tmpFC=((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490->hd=((_tmp491=_cycalloc(sizeof(*_tmp491)),((_tmp491[0]=def_inc_path,_tmp491)))),((_tmp490->tl=_tmpFC,_tmp490))))));}{
# 929
char*_tmp100=getenv("CYCLONE_INCLUDE_PATH");
if(_tmp100 != 0){
char*_tmp492;_tmpFC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp492=_tmp100,_tag_dyneither(_tmp492,sizeof(char),_get_zero_arr_size_char((void*)_tmp492,1)))),':'),_tmpFC);}{
# 934
const char*_tmp499;struct _dyneither_ptr*_tmp498;const char*_tmp497;struct Cyc_List_List*_tmp496;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->hd=((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498[0]=(struct _dyneither_ptr)((_tmp497="",_tag_dyneither(_tmp497,sizeof(char),1))),_tmp498)))),((_tmp496->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpFC),_tmp496)))))),(
(_tmp499=" -I",_tag_dyneither(_tmp499,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp49D;void*_tmp49C[1];struct Cyc_String_pa_PrintArg_struct _tmp49B;ofile_string=(struct _dyneither_ptr)((_tmp49B.tag=0,((_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp49C[0]=& _tmp49B,Cyc_aprintf(((_tmp49D=" > %s",_tag_dyneither(_tmp49D,sizeof(char),6))),_tag_dyneither(_tmp49C,sizeof(void*),1))))))));}else{
# 943
const char*_tmp49E;ofile_string=((_tmp49E="",_tag_dyneither(_tmp49E,sizeof(char),1)));}}else{
# 945
const char*_tmp4A2;void*_tmp4A1[1];struct Cyc_String_pa_PrintArg_struct _tmp4A0;ofile_string=(struct _dyneither_ptr)((_tmp4A0.tag=0,((_tmp4A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpF4)),((_tmp4A1[0]=& _tmp4A0,Cyc_aprintf(((_tmp4A2=" > %s",_tag_dyneither(_tmp4A2,sizeof(char),6))),_tag_dyneither(_tmp4A1,sizeof(void*),1))))))));}{
# 947
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 951
if(Cyc_dependencies_target == 0){
# 955
const char*_tmp4A3;fixup_string=((_tmp4A3=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4A3,sizeof(char),35)));}else{
# 960
const char*_tmp4A7;void*_tmp4A6[1];struct Cyc_String_pa_PrintArg_struct _tmp4A5;fixup_string=(struct _dyneither_ptr)(
(_tmp4A5.tag=0,((_tmp4A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4A6[0]=& _tmp4A5,Cyc_aprintf(((_tmp4A7=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4A7,sizeof(char),29))),_tag_dyneither(_tmp4A6,sizeof(void*),1))))))));}}else{
# 964
const char*_tmp4A8;fixup_string=((_tmp4A8="",_tag_dyneither(_tmp4A8,sizeof(char),1)));}{
# 966
const char*_tmp4B1;void*_tmp4B0[6];struct Cyc_String_pa_PrintArg_struct _tmp4AF;struct Cyc_String_pa_PrintArg_struct _tmp4AE;struct Cyc_String_pa_PrintArg_struct _tmp4AD;struct Cyc_String_pa_PrintArg_struct _tmp4AC;struct Cyc_String_pa_PrintArg_struct _tmp4AB;struct Cyc_String_pa_PrintArg_struct _tmp4AA;struct _dyneither_ptr _tmp10E=(_tmp4AA.tag=0,((_tmp4AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4AB.tag=0,((_tmp4AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4AC.tag=0,((_tmp4AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 969
Cyc_sh_escape_string(filename)),((_tmp4AD.tag=0,((_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4AE.tag=0,((_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFB),((_tmp4AF.tag=0,((_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4B0[0]=& _tmp4AF,((_tmp4B0[1]=& _tmp4AE,((_tmp4B0[2]=& _tmp4AD,((_tmp4B0[3]=& _tmp4AC,((_tmp4B0[4]=& _tmp4AB,((_tmp4B0[5]=& _tmp4AA,Cyc_aprintf(((_tmp4B1="%s %s%s %s%s%s",_tag_dyneither(_tmp4B1,sizeof(char),15))),_tag_dyneither(_tmp4B0,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 972
if(Cyc_v_r){const char*_tmp4B5;void*_tmp4B4[1];struct Cyc_String_pa_PrintArg_struct _tmp4B3;(_tmp4B3.tag=0,((_tmp4B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10E),((_tmp4B4[0]=& _tmp4B3,Cyc_fprintf(Cyc_stderr,((_tmp4B5="%s\n",_tag_dyneither(_tmp4B5,sizeof(char),4))),_tag_dyneither(_tmp4B4,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp10E,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp4B8;void*_tmp4B7;(_tmp4B7=0,Cyc_fprintf(Cyc_stderr,((_tmp4B8="\nError: preprocessing\n",_tag_dyneither(_tmp4B8,sizeof(char),23))),_tag_dyneither(_tmp4B7,sizeof(void*),0)));}
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpF4);
return;}
# 979
if(Cyc_stop_after_cpp_r)return;
# 982
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpF4);{
const char*_tmp4BA;const char*_tmp4B9;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF4,((_tmp4B9="r",_tag_dyneither(_tmp4B9,sizeof(char),2))),((_tmp4BA="file",_tag_dyneither(_tmp4BA,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 986
struct Cyc_List_List*tds=0;
# 989
{struct _handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=0;if(setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){
{const char*_tmp4BB;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4BB="parsing",_tag_dyneither(_tmp4BB,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp115=(void*)_exn_thrown;void*_tmp118=_tmp115;void*_tmp119;_LL38: _tmp119=_tmp118;_LL39:
# 993
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpF4);
Cyc_Core_rethrow(_tmp119);_LL37:;}};}
# 997
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF4);
return;}
# 1003
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 1008
struct _RegionHandle _tmp11A=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11A;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp11B=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 1013
struct _tuple22 _tmp4BC;struct _tuple22 _tmp11C=(_tmp4BC.f1=tc_rgn,((_tmp4BC.f2=_tmp11B,_tmp4BC)));
{const char*_tmp4BD;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4BD="type checking",_tag_dyneither(_tmp4BD,sizeof(char),14))),Cyc_do_typecheck,& _tmp11C,tds);}
# 1016
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF4);
_npop_handler(0);return;}
# 1020
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp4BE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4BE="control-flow checking",_tag_dyneither(_tmp4BE,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF4);
_npop_handler(0);return;}
# 1028
if(Cyc_generate_interface_r){
const char*_tmp4C1;const char*_tmp4C0;const char*_tmp4BF;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp4BF=(const char*)_untag_dyneither_ptr(_tmpF5,sizeof(char),1),_tag_dyneither(_tmp4BF,sizeof(char),_get_zero_arr_size_char((void*)_tmp4BF,1)))),((_tmp4C0="w",_tag_dyneither(_tmp4C0,sizeof(char),2))),((_tmp4C1="interface file",_tag_dyneither(_tmp4C1,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 1033
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 1040
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpF5,sizeof(char),1),"r");
const char*_tmp4C3;const char*_tmp4C2;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpF6,((_tmp4C2="w",_tag_dyneither(_tmp4C2,sizeof(char),2))),((_tmp4C3="interface object file",_tag_dyneither(_tmp4C3,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1047
Cyc_Position_reset_position(_tmpF5);{
struct _tuple23 _tmp4C4;struct _tuple23 _tmp122=(_tmp4C4.f1=_tmp11B,((_tmp4C4.f2=inter_file,((_tmp4C4.f3=inter_objfile,_tmp4C4)))));
{const char*_tmp4C5;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4C5="interface checking",_tag_dyneither(_tmp4C5,sizeof(char),19))),Cyc_do_interface,& _tmp122,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1055
if(Cyc_cf_r)goto PRINTC;
# 1058
{const char*_tmp4C6;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4C6="translation to C",_tag_dyneither(_tmp4C6,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF4);
_npop_handler(0);return;}
# 1064
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpF4);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1072
const char*_tmp4C7;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4C7="post-pass to VC",_tag_dyneither(_tmp4C7,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1079
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp4C9;const char*_tmp4C8;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4C8="w",_tag_dyneither(_tmp4C8,sizeof(char),2))),((_tmp4C9="output file",_tag_dyneither(_tmp4C9,sizeof(char),12))));}else{
# 1085
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp4CB;const char*_tmp4CA;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4CA="w",_tag_dyneither(_tmp4CA,sizeof(char),2))),((_tmp4CB="output file",_tag_dyneither(_tmp4CB,sizeof(char),12))));}else{
# 1089
const char*_tmp4CD;const char*_tmp4CC;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF7,((_tmp4CC="w",_tag_dyneither(_tmp4CC,sizeof(char),2))),((_tmp4CD="output file",_tag_dyneither(_tmp4CD,sizeof(char),12))));}}
# 1091
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1093
{struct _handler_cons _tmp130;_push_handler(& _tmp130);{int _tmp132=0;if(setjmp(_tmp130.handler))_tmp132=1;if(!_tmp132){
if(!Cyc_noprint_r){
const char*_tmp4CE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4CE="printing",_tag_dyneither(_tmp4CE,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1094
;_pop_handler();}else{void*_tmp131=(void*)_exn_thrown;void*_tmp134=_tmp131;void*_tmp135;_LL3D: _tmp135=_tmp134;_LL3E:
# 1098
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp4D1;struct Cyc_List_List*_tmp4D0;Cyc_cfiles=((_tmp4D0=_cycalloc(sizeof(*_tmp4D0)),((_tmp4D0->hd=((_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((_tmp4D1[0]=(struct _dyneither_ptr)_tmpF7,_tmp4D1)))),((_tmp4D0->tl=Cyc_cfiles,_tmp4D0))))));}
Cyc_Core_rethrow(_tmp135);_LL3C:;}};}
# 1103
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp4D4;struct Cyc_List_List*_tmp4D3;Cyc_cfiles=((_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3->hd=((_tmp4D4=_cycalloc(sizeof(*_tmp4D4)),((_tmp4D4[0]=(struct _dyneither_ptr)_tmpF7,_tmp4D4)))),((_tmp4D3->tl=Cyc_cfiles,_tmp4D3))))));};}
# 1008
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp152[8]="<final>";
# 1108
static struct _dyneither_ptr Cyc_final_str={_tmp152,_tmp152,_tmp152 + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1111
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1115
struct _dyneither_ptr basename;
{struct _handler_cons _tmp153;_push_handler(& _tmp153);{int _tmp155=0;if(setjmp(_tmp153.handler))_tmp155=1;if(!_tmp155){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp154=(void*)_exn_thrown;void*_tmp156=_tmp154;void*_tmp157;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp156)->tag == Cyc_Core_Invalid_argument){_LL42: _LL43:
# 1119
 basename=*n;goto _LL41;}else{_LL44: _tmp157=_tmp156;_LL45:(int)_rethrow(_tmp157);}_LL41:;}};}{
# 1121
const char*_tmp4D5;struct _dyneither_ptr _tmp158=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp4D5=".cycio",_tag_dyneither(_tmp4D5,sizeof(char),7))));
const char*_tmp4D7;const char*_tmp4D6;struct Cyc___cycFILE*_tmp159=Cyc_try_file_open((struct _dyneither_ptr)_tmp158,((_tmp4D6="rb",_tag_dyneither(_tmp4D6,sizeof(char),3))),((_tmp4D7="interface object file",_tag_dyneither(_tmp4D7,sizeof(char),22))));
if(_tmp159 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1128
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp158);{
# 1130
struct Cyc_Interface_I*_tmp15A=Cyc_Interface_load(_tmp159);
Cyc_file_close(_tmp159);
return _tmp15A;};};};}
# 1135
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1139
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp4DD(unsigned int*_tmp4DC,unsigned int*_tmp4DB,int**_tmp4D9){for(*_tmp4DC=0;*_tmp4DC < *_tmp4DB;(*_tmp4DC)++){(*_tmp4D9)[*_tmp4DC]=(int)0;}}static void _tmp4E5(unsigned int*_tmp4E4,unsigned int*_tmp4E3,struct _dyneither_ptr**_tmp4E1){for(*_tmp4E4=0;*_tmp4E4 < *_tmp4E3;(*_tmp4E4)++){(*_tmp4E1)[*_tmp4E4]=(struct _dyneither_ptr)
# 1176
_tag_dyneither(0,0,0);}}static void _tmp4EB(unsigned int*_tmp4EA,unsigned int*_tmp4E9,struct _dyneither_ptr**_tmp4E7){for(*_tmp4EA=0;*_tmp4EA < *_tmp4E9;(*_tmp4EA)++){(*_tmp4E7)[*_tmp4EA]=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);}}
# 1142
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1147
GC_blacklist_warn_clear();{
# 1149
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1154
struct _RegionHandle _tmp15E=_new_region("r");struct _RegionHandle*r=& _tmp15E;_push_region(r);{
# 1156
unsigned int _tmp4DC;unsigned int _tmp4DB;struct _dyneither_ptr _tmp4DA;int*_tmp4D9;unsigned int _tmp4D8;struct _dyneither_ptr _tmp15F=(_tmp4D8=(unsigned int)argc,((_tmp4D9=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp4D8)),((_tmp4DA=_tag_dyneither(_tmp4D9,sizeof(int),_tmp4D8),((((_tmp4DB=_tmp4D8,_tmp4DD(& _tmp4DC,& _tmp4DB,& _tmp4D9))),_tmp4DA)))))));
int _tmp160=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp4DE;if(Cyc_strncmp(((_tmp4DE="-B",_tag_dyneither(_tmp4DE,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp15F,sizeof(int),i))=1;
++ _tmp160;}else{
# 1164
const char*_tmp4DF;if(Cyc_strcmp(((_tmp4DF="-b",_tag_dyneither(_tmp4DF,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp15F,sizeof(int),i))=1;
++ _tmp160;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp15F,sizeof(int),i))=1;
++ _tmp160;}}}{
# 1176
unsigned int _tmp4E4;unsigned int _tmp4E3;struct _dyneither_ptr _tmp4E2;struct _dyneither_ptr*_tmp4E1;unsigned int _tmp4E0;struct _dyneither_ptr _tmp163=(_tmp4E0=(unsigned int)(_tmp160 + 1),((_tmp4E1=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4E0)),((_tmp4E2=_tag_dyneither(_tmp4E1,sizeof(struct _dyneither_ptr),_tmp4E0),((((_tmp4E3=_tmp4E0,_tmp4E5(& _tmp4E4,& _tmp4E3,& _tmp4E1))),_tmp4E2)))))));
unsigned int _tmp4EA;unsigned int _tmp4E9;struct _dyneither_ptr _tmp4E8;struct _dyneither_ptr*_tmp4E7;unsigned int _tmp4E6;struct _dyneither_ptr _tmp164=(_tmp4E6=(unsigned int)(argc - _tmp160),((_tmp4E7=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4E6)),((_tmp4E8=_tag_dyneither(_tmp4E7,sizeof(struct _dyneither_ptr),_tmp4E6),((((_tmp4E9=_tmp4E6,_tmp4EB(& _tmp4EA,& _tmp4E9,& _tmp4E7))),_tmp4E8)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp163,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp164,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp15F,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp163,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp164,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1185
struct _tuple24*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E6;const char*_tmp9E5;struct _tuple24*_tmp9E4;const char*_tmp9E3;const char*_tmp9E2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9E1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9E0;const char*_tmp9DF;struct _tuple24*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9DB;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9DA;const char*_tmp9D9;struct _tuple24*_tmp9D8;const char*_tmp9D7;const char*_tmp9D6;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9D5;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9D4;const char*_tmp9D3;struct _tuple24*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9CF;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9CE;const char*_tmp9CD;struct _tuple24*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9C9;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9C8;const char*_tmp9C7;struct _tuple24*_tmp9C6;const char*_tmp9C5;const char*_tmp9C4;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9C3;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9C2;const char*_tmp9C1;struct _tuple24*_tmp9C0;const char*_tmp9BF;const char*_tmp9BE;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9BD;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9BC;const char*_tmp9BB;struct _tuple24*_tmp9BA;const char*_tmp9B9;const char*_tmp9B8;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9B7;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9B6;const char*_tmp9B5;struct _tuple24*_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9B1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9B0;const char*_tmp9AF;struct _tuple24*_tmp9AE;const char*_tmp9AD;const char*_tmp9AC;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9AB;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9AA;const char*_tmp9A9;struct _tuple24*_tmp9A8;const char*_tmp9A7;const char*_tmp9A6;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A5;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A4;const char*_tmp9A3;struct _tuple24*_tmp9A2;const char*_tmp9A1;const char*_tmp9A0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp99F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp99E;const char*_tmp99D;struct _tuple24*_tmp99C;const char*_tmp99B;const char*_tmp99A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp999;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp998;const char*_tmp997;struct _tuple24*_tmp996;const char*_tmp995;const char*_tmp994;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp993;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp992;const char*_tmp991;struct _tuple24*_tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp98D;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp98C;const char*_tmp98B;struct _tuple24*_tmp98A;const char*_tmp989;const char*_tmp988;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp987;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp986;const char*_tmp985;struct _tuple24*_tmp984;const char*_tmp983;const char*_tmp982;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp981;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp980;const char*_tmp97F;struct _tuple24*_tmp97E;const char*_tmp97D;const char*_tmp97C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp97B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp97A;const char*_tmp979;struct _tuple24*_tmp978;const char*_tmp977;const char*_tmp976;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp975;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp974;const char*_tmp973;struct _tuple24*_tmp972;const char*_tmp971;const char*_tmp970;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp96F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp96E;const char*_tmp96D;struct _tuple24*_tmp96C;const char*_tmp96B;const char*_tmp96A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp969;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp968;const char*_tmp967;struct _tuple24*_tmp966;const char*_tmp965;const char*_tmp964;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp963;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp962;const char*_tmp961;struct _tuple24*_tmp960;const char*_tmp95F;const char*_tmp95E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp95D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp95C;const char*_tmp95B;struct _tuple24*_tmp95A;const char*_tmp959;const char*_tmp958;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp957;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp956;const char*_tmp955;struct _tuple24*_tmp954;const char*_tmp953;const char*_tmp952;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp951;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp950;const char*_tmp94F;struct _tuple24*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94A;const char*_tmp949;struct _tuple24*_tmp948;const char*_tmp947;const char*_tmp946;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp945;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp944;const char*_tmp943;struct _tuple24*_tmp942;const char*_tmp941;const char*_tmp940;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp93F;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp93E;const char*_tmp93D;struct _tuple24*_tmp93C;const char*_tmp93B;const char*_tmp93A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp939;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp938;const char*_tmp937;struct _tuple24*_tmp936;const char*_tmp935;const char*_tmp934;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp933;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp932;const char*_tmp931;struct _tuple24*_tmp930;const char*_tmp92F;const char*_tmp92E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp92D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp92C;const char*_tmp92B;struct _tuple24*_tmp92A;const char*_tmp929;const char*_tmp928;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp927;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp926;const char*_tmp925;struct _tuple24*_tmp924;const char*_tmp923;const char*_tmp922;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp921;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp920;const char*_tmp91F;struct _tuple24*_tmp91E;const char*_tmp91D;const char*_tmp91C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp91B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp91A;const char*_tmp919;struct _tuple24*_tmp918;const char*_tmp917;const char*_tmp916;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp915;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp914;const char*_tmp913;struct _tuple24*_tmp912;const char*_tmp911;const char*_tmp910;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp90F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp90E;const char*_tmp90D;struct _tuple24*_tmp90C;const char*_tmp90B;const char*_tmp90A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp909;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp908;const char*_tmp907;struct _tuple24*_tmp906;const char*_tmp905;const char*_tmp904;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp903;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp902;const char*_tmp901;struct _tuple24*_tmp900;const char*_tmp8FF;const char*_tmp8FE;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8FD;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8FC;const char*_tmp8FB;struct _tuple24*_tmp8FA;const char*_tmp8F9;const char*_tmp8F8;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8F7;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8F6;const char*_tmp8F5;struct _tuple24*_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8F1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8F0;const char*_tmp8EF;struct _tuple24*_tmp8EE;const char*_tmp8ED;const char*_tmp8EC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8EB;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8EA;const char*_tmp8E9;struct _tuple24*_tmp8E8;const char*_tmp8E7;const char*_tmp8E6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8E5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8E4;const char*_tmp8E3;struct _tuple24*_tmp8E2;const char*_tmp8E1;const char*_tmp8E0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8DF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8DE;const char*_tmp8DD;struct _tuple24*_tmp8DC;const char*_tmp8DB;const char*_tmp8DA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D8;const char*_tmp8D7;struct _tuple24*_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D2;const char*_tmp8D1;struct _tuple24*_tmp8D0;const char*_tmp8CF;const char*_tmp8CE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8CD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8CC;const char*_tmp8CB;struct _tuple24*_tmp8CA;const char*_tmp8C9;const char*_tmp8C8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8C7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8C6;const char*_tmp8C5;struct _tuple24*_tmp8C4;const char*_tmp8C3;const char*_tmp8C2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8C1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8C0;const char*_tmp8BF;struct _tuple24*_tmp8BE;const char*_tmp8BD;const char*_tmp8BC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8BB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8BA;const char*_tmp8B9;struct _tuple24*_tmp8B8;const char*_tmp8B7;const char*_tmp8B6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8B5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8B4;const char*_tmp8B3;struct _tuple24*_tmp8B2;const char*_tmp8B1;const char*_tmp8B0;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8AF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8AE;const char*_tmp8AD;struct _tuple24*_tmp8AC;const char*_tmp8AB;const char*_tmp8AA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8A9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8A8;const char*_tmp8A7;struct _tuple24*_tmp8A6;const char*_tmp8A5;const char*_tmp8A4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8A3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8A2;const char*_tmp8A1;struct _tuple24*_tmp8A0;const char*_tmp89F;const char*_tmp89E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp89D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp89C;const char*_tmp89B;struct _tuple24*_tmp89A;const char*_tmp899;const char*_tmp898;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp897;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp896;const char*_tmp895;struct _tuple24*_tmp894;const char*_tmp893;const char*_tmp892;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp891;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp890;const char*_tmp88F;struct _tuple24*_tmp88E;const char*_tmp88D;const char*_tmp88C;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp88B;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp88A;const char*_tmp889;struct _tuple24*_tmp888;const char*_tmp887;const char*_tmp886;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp885;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp884;const char*_tmp883;struct _tuple24*_tmp882;const char*_tmp881;const char*_tmp880;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp87F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp87E;const char*_tmp87D;struct _tuple24*_tmp87C;const char*_tmp87B;const char*_tmp87A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp879;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp878;const char*_tmp877;struct _tuple24*_tmp876;const char*_tmp875;const char*_tmp874;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp873;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp872;const char*_tmp871;struct _tuple24*_tmp870;const char*_tmp86F;const char*_tmp86E;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp86D;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp86C;const char*_tmp86B;struct _tuple24*_tmp86A;const char*_tmp869;const char*_tmp868;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp867;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp866;const char*_tmp865;struct _tuple24*_tmp864;const char*_tmp863;const char*_tmp862;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp861;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp860;const char*_tmp85F;struct _tuple24*_tmp85E;const char*_tmp85D;const char*_tmp85C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp85B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp85A;const char*_tmp859;struct _tuple24*_tmp858;const char*_tmp857;const char*_tmp856;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp855;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp854;const char*_tmp853;struct _tuple24*_tmp852;const char*_tmp851;const char*_tmp850;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp84F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp84E;const char*_tmp84D;struct _tuple24*_tmp84C;const char*_tmp84B;const char*_tmp84A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp849;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp848;const char*_tmp847;struct _tuple24*_tmp846;const char*_tmp845;const char*_tmp844;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp843;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp842;const char*_tmp841;struct _tuple24*_tmp840[71];struct Cyc_List_List*options=
(_tmp840[70]=(
# 1399
(_tmp846=_region_malloc(r,sizeof(*_tmp846)),((_tmp846->f1=((_tmp845="-known-gcc-flags-only",_tag_dyneither(_tmp845,sizeof(char),22))),((_tmp846->f2=0,((_tmp846->f3=((_tmp844="",_tag_dyneither(_tmp844,sizeof(char),1))),((_tmp846->f4=(void*)(
(_tmp842=_region_malloc(r,sizeof(*_tmp842)),((_tmp842[0]=((_tmp843.tag=0,((_tmp843.f1=Cyc_noassume_gcc_flag,_tmp843)))),_tmp842)))),((_tmp846->f5=(
(_tmp841="do not assume that unknown flags should be passed to gcc",_tag_dyneither(_tmp841,sizeof(char),57))),_tmp846)))))))))))),((_tmp840[69]=(
# 1396
(_tmp84C=_region_malloc(r,sizeof(*_tmp84C)),((_tmp84C->f1=((_tmp84B="--parsestate",_tag_dyneither(_tmp84B,sizeof(char),13))),((_tmp84C->f2=0,((_tmp84C->f3=((_tmp84A="",_tag_dyneither(_tmp84A,sizeof(char),1))),((_tmp84C->f4=(void*)(
(_tmp848=_region_malloc(r,sizeof(*_tmp848)),((_tmp848[0]=((_tmp849.tag=3,((_tmp849.f1=& Cyc_ParseErrors_print_state_and_token,_tmp849)))),_tmp848)))),((_tmp84C->f5=(
(_tmp847="print the parse state and token on a syntax error",_tag_dyneither(_tmp847,sizeof(char),50))),_tmp84C)))))))))))),((_tmp840[68]=(
# 1393
(_tmp852=_region_malloc(r,sizeof(*_tmp852)),((_tmp852->f1=((_tmp851="--manyerrors",_tag_dyneither(_tmp851,sizeof(char),13))),((_tmp852->f2=0,((_tmp852->f3=((_tmp850="",_tag_dyneither(_tmp850,sizeof(char),1))),((_tmp852->f4=(void*)(
(_tmp84E=_region_malloc(r,sizeof(*_tmp84E)),((_tmp84E[0]=((_tmp84F.tag=0,((_tmp84F.f1=Cyc_set_many_errors,_tmp84F)))),_tmp84E)))),((_tmp852->f5=(
(_tmp84D="don't stop after only a few errors.",_tag_dyneither(_tmp84D,sizeof(char),36))),_tmp852)))))))))))),((_tmp840[67]=(
# 1390
(_tmp858=_region_malloc(r,sizeof(*_tmp858)),((_tmp858->f1=((_tmp857="--interface",_tag_dyneither(_tmp857,sizeof(char),12))),((_tmp858->f2=0,((_tmp858->f3=((_tmp856=" <file>",_tag_dyneither(_tmp856,sizeof(char),8))),((_tmp858->f4=(void*)(
(_tmp854=_region_malloc(r,sizeof(*_tmp854)),((_tmp854[0]=((_tmp855.tag=5,((_tmp855.f1=Cyc_set_specified_interface,_tmp855)))),_tmp854)))),((_tmp858->f5=(
(_tmp853="Set the interface file to be <file>.",_tag_dyneither(_tmp853,sizeof(char),37))),_tmp858)))))))))))),((_tmp840[66]=(
# 1387
(_tmp85E=_region_malloc(r,sizeof(*_tmp85E)),((_tmp85E->f1=((_tmp85D="--geninterface",_tag_dyneither(_tmp85D,sizeof(char),15))),((_tmp85E->f2=0,((_tmp85E->f3=((_tmp85C="",_tag_dyneither(_tmp85C,sizeof(char),1))),((_tmp85E->f4=(void*)(
(_tmp85A=_region_malloc(r,sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85B.tag=3,((_tmp85B.f1=& Cyc_generate_interface_r,_tmp85B)))),_tmp85A)))),((_tmp85E->f5=(
(_tmp859="Generate interface files",_tag_dyneither(_tmp859,sizeof(char),25))),_tmp85E)))))))))))),((_tmp840[65]=(
# 1384
(_tmp864=_region_malloc(r,sizeof(*_tmp864)),((_tmp864->f1=((_tmp863="-idirafter",_tag_dyneither(_tmp863,sizeof(char),11))),((_tmp864->f2=0,((_tmp864->f3=((_tmp862="<dir>",_tag_dyneither(_tmp862,sizeof(char),6))),((_tmp864->f4=(void*)(
(_tmp860=_region_malloc(r,sizeof(*_tmp860)),((_tmp860[0]=((_tmp861.tag=5,((_tmp861.f1=Cyc_add_idirafter,_tmp861)))),_tmp860)))),((_tmp864->f5=(
(_tmp85F="Add the directory to the second include path.",_tag_dyneither(_tmp85F,sizeof(char),46))),_tmp864)))))))))))),((_tmp840[64]=(
# 1381
(_tmp86A=_region_malloc(r,sizeof(*_tmp86A)),((_tmp86A->f1=((_tmp869="-isystem",_tag_dyneither(_tmp869,sizeof(char),9))),((_tmp86A->f2=0,((_tmp86A->f3=((_tmp868="<dir>",_tag_dyneither(_tmp868,sizeof(char),6))),((_tmp86A->f4=(void*)(
(_tmp866=_region_malloc(r,sizeof(*_tmp866)),((_tmp866[0]=((_tmp867.tag=5,((_tmp867.f1=Cyc_add_isystem,_tmp867)))),_tmp866)))),((_tmp86A->f5=(
(_tmp865="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp865,sizeof(char),90))),_tmp86A)))))))))))),((_tmp840[63]=(
# 1378
(_tmp870=_region_malloc(r,sizeof(*_tmp870)),((_tmp870->f1=((_tmp86F="-iwithprefixbefore",_tag_dyneither(_tmp86F,sizeof(char),19))),((_tmp870->f2=0,((_tmp870->f3=((_tmp86E="<dir>",_tag_dyneither(_tmp86E,sizeof(char),6))),((_tmp870->f4=(void*)(
(_tmp86C=_region_malloc(r,sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86D.tag=5,((_tmp86D.f1=Cyc_add_iwithprefixbefore,_tmp86D)))),_tmp86C)))),((_tmp870->f5=(
(_tmp86B="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp86B,sizeof(char),45))),_tmp870)))))))))))),((_tmp840[62]=(
# 1375
(_tmp876=_region_malloc(r,sizeof(*_tmp876)),((_tmp876->f1=((_tmp875="-iwithprefix",_tag_dyneither(_tmp875,sizeof(char),13))),((_tmp876->f2=0,((_tmp876->f3=((_tmp874="<dir>",_tag_dyneither(_tmp874,sizeof(char),6))),((_tmp876->f4=(void*)(
(_tmp872=_region_malloc(r,sizeof(*_tmp872)),((_tmp872[0]=((_tmp873.tag=5,((_tmp873.f1=Cyc_add_iwithprefix,_tmp873)))),_tmp872)))),((_tmp876->f5=(
(_tmp871="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp871,sizeof(char),47))),_tmp876)))))))))))),((_tmp840[61]=(
# 1372
(_tmp87C=_region_malloc(r,sizeof(*_tmp87C)),((_tmp87C->f1=((_tmp87B="-iprefix",_tag_dyneither(_tmp87B,sizeof(char),9))),((_tmp87C->f2=0,((_tmp87C->f3=((_tmp87A="<prefix>",_tag_dyneither(_tmp87A,sizeof(char),9))),((_tmp87C->f4=(void*)(
(_tmp878=_region_malloc(r,sizeof(*_tmp878)),((_tmp878[0]=((_tmp879.tag=5,((_tmp879.f1=Cyc_add_iprefix,_tmp879)))),_tmp878)))),((_tmp87C->f5=(
(_tmp877="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp877,sizeof(char),67))),_tmp87C)))))))))))),((_tmp840[60]=(
# 1369
(_tmp882=_region_malloc(r,sizeof(*_tmp882)),((_tmp882->f1=((_tmp881="-noregions",_tag_dyneither(_tmp881,sizeof(char),11))),((_tmp882->f2=0,((_tmp882->f3=((_tmp880="",_tag_dyneither(_tmp880,sizeof(char),1))),((_tmp882->f4=(void*)(
(_tmp87E=_region_malloc(r,sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp87F.tag=3,((_tmp87F.f1=& Cyc_Absyn_no_regions,_tmp87F)))),_tmp87E)))),((_tmp882->f5=(
(_tmp87D="Generate code that doesn't use regions",_tag_dyneither(_tmp87D,sizeof(char),39))),_tmp882)))))))))))),((_tmp840[59]=(
# 1366
(_tmp888=_region_malloc(r,sizeof(*_tmp888)),((_tmp888->f1=((_tmp887="-port",_tag_dyneither(_tmp887,sizeof(char),6))),((_tmp888->f2=0,((_tmp888->f3=((_tmp886="",_tag_dyneither(_tmp886,sizeof(char),1))),((_tmp888->f4=(void*)(
(_tmp884=_region_malloc(r,sizeof(*_tmp884)),((_tmp884[0]=((_tmp885.tag=0,((_tmp885.f1=Cyc_set_port_c_code,_tmp885)))),_tmp884)))),((_tmp888->f5=(
(_tmp883="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp883,sizeof(char),38))),_tmp888)))))))))))),((_tmp840[58]=(
# 1363
(_tmp88E=_region_malloc(r,sizeof(*_tmp88E)),((_tmp88E->f1=((_tmp88D="-detailedlocation",_tag_dyneither(_tmp88D,sizeof(char),18))),((_tmp88E->f2=0,((_tmp88E->f3=((_tmp88C="",_tag_dyneither(_tmp88C,sizeof(char),1))),((_tmp88E->f4=(void*)(
(_tmp88A=_region_malloc(r,sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp88B.tag=4,((_tmp88B.f1=& Cyc_Position_use_gcc_style_location,_tmp88B)))),_tmp88A)))),((_tmp88E->f5=(
(_tmp889="Try to give more detailed location information for errors",_tag_dyneither(_tmp889,sizeof(char),58))),_tmp88E)))))))))))),((_tmp840[57]=(
# 1360
(_tmp894=_region_malloc(r,sizeof(*_tmp894)),((_tmp894->f1=((_tmp893="-noregister",_tag_dyneither(_tmp893,sizeof(char),12))),((_tmp894->f2=0,((_tmp894->f3=((_tmp892="",_tag_dyneither(_tmp892,sizeof(char),1))),((_tmp894->f4=(void*)(
(_tmp890=_region_malloc(r,sizeof(*_tmp890)),((_tmp890[0]=((_tmp891.tag=3,((_tmp891.f1=& Cyc_Parse_no_register,_tmp891)))),_tmp890)))),((_tmp894->f5=(
(_tmp88F="Treat register storage class as public",_tag_dyneither(_tmp88F,sizeof(char),39))),_tmp894)))))))))))),((_tmp840[56]=(
# 1357
(_tmp89A=_region_malloc(r,sizeof(*_tmp89A)),((_tmp89A->f1=((_tmp899="-warnregioncoerce",_tag_dyneither(_tmp899,sizeof(char),18))),((_tmp89A->f2=0,((_tmp89A->f3=((_tmp898="",_tag_dyneither(_tmp898,sizeof(char),1))),((_tmp89A->f4=(void*)(
(_tmp896=_region_malloc(r,sizeof(*_tmp896)),((_tmp896[0]=((_tmp897.tag=3,((_tmp897.f1=& Cyc_Tcutil_warn_region_coerce,_tmp897)))),_tmp896)))),((_tmp89A->f5=(
(_tmp895="Warn when any region coercion is inserted",_tag_dyneither(_tmp895,sizeof(char),42))),_tmp89A)))))))))))),((_tmp840[55]=(
# 1354
(_tmp8A0=_region_malloc(r,sizeof(*_tmp8A0)),((_tmp8A0->f1=((_tmp89F="-warnaliascoercion",_tag_dyneither(_tmp89F,sizeof(char),19))),((_tmp8A0->f2=0,((_tmp8A0->f3=((_tmp89E="",_tag_dyneither(_tmp89E,sizeof(char),1))),((_tmp8A0->f4=(void*)(
(_tmp89C=_region_malloc(r,sizeof(*_tmp89C)),((_tmp89C[0]=((_tmp89D.tag=3,((_tmp89D.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp89D)))),_tmp89C)))),((_tmp8A0->f5=(
(_tmp89B="Warn when any alias coercion is inserted",_tag_dyneither(_tmp89B,sizeof(char),41))),_tmp8A0)))))))))))),((_tmp840[54]=(
# 1351
(_tmp8A6=_region_malloc(r,sizeof(*_tmp8A6)),((_tmp8A6->f1=((_tmp8A5="-warnnullchecks",_tag_dyneither(_tmp8A5,sizeof(char),16))),((_tmp8A6->f2=0,((_tmp8A6->f3=((_tmp8A4="",_tag_dyneither(_tmp8A4,sizeof(char),1))),((_tmp8A6->f4=(void*)(
(_tmp8A2=_region_malloc(r,sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A3.tag=3,((_tmp8A3.f1=& Cyc_Toc_warn_all_null_deref,_tmp8A3)))),_tmp8A2)))),((_tmp8A6->f5=(
(_tmp8A1="Warn when any null check can't be eliminated",_tag_dyneither(_tmp8A1,sizeof(char),45))),_tmp8A6)))))))))))),((_tmp840[53]=(
# 1348
(_tmp8AC=_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC->f1=((_tmp8AB="-warnboundschecks",_tag_dyneither(_tmp8AB,sizeof(char),18))),((_tmp8AC->f2=0,((_tmp8AC->f3=((_tmp8AA="",_tag_dyneither(_tmp8AA,sizeof(char),1))),((_tmp8AC->f4=(void*)(
(_tmp8A8=_region_malloc(r,sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=3,((_tmp8A9.f1=& Cyc_Toc_warn_bounds_checks,_tmp8A9)))),_tmp8A8)))),((_tmp8AC->f5=(
(_tmp8A7="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp8A7,sizeof(char),44))),_tmp8AC)))))))))))),((_tmp840[52]=(
# 1345
(_tmp8B2=_region_malloc(r,sizeof(*_tmp8B2)),((_tmp8B2->f1=((_tmp8B1="-CI",_tag_dyneither(_tmp8B1,sizeof(char),4))),((_tmp8B2->f2=0,((_tmp8B2->f3=((_tmp8B0=" <file>",_tag_dyneither(_tmp8B0,sizeof(char),8))),((_tmp8B2->f4=(void*)(
(_tmp8AE=_region_malloc(r,sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8AF.tag=5,((_tmp8AF.f1=Cyc_set_cyc_include,_tmp8AF)))),_tmp8AE)))),((_tmp8B2->f5=(
(_tmp8AD="Set cyc_include.h to be <file>",_tag_dyneither(_tmp8AD,sizeof(char),31))),_tmp8B2)))))))))))),((_tmp840[51]=(
# 1342
(_tmp8B8=_region_malloc(r,sizeof(*_tmp8B8)),((_tmp8B8->f1=((_tmp8B7="-compile-for-boot",_tag_dyneither(_tmp8B7,sizeof(char),18))),((_tmp8B8->f2=0,((_tmp8B8->f3=((_tmp8B6="",_tag_dyneither(_tmp8B6,sizeof(char),1))),((_tmp8B8->f4=(void*)(
(_tmp8B4=_region_malloc(r,sizeof(*_tmp8B4)),((_tmp8B4[0]=((_tmp8B5.tag=3,((_tmp8B5.f1=& Cyc_Lex_compile_for_boot_r,_tmp8B5)))),_tmp8B4)))),((_tmp8B8->f5=(
(_tmp8B3="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp8B3,sizeof(char),71))),_tmp8B8)))))))))))),((_tmp840[50]=(
# 1339
(_tmp8BE=_region_malloc(r,sizeof(*_tmp8BE)),((_tmp8BE->f1=((_tmp8BD="-nocyc_setjmp",_tag_dyneither(_tmp8BD,sizeof(char),14))),((_tmp8BE->f2=0,((_tmp8BE->f3=((_tmp8BC="",_tag_dyneither(_tmp8BC,sizeof(char),1))),((_tmp8BE->f4=(void*)(
(_tmp8BA=_region_malloc(r,sizeof(*_tmp8BA)),((_tmp8BA[0]=((_tmp8BB.tag=3,((_tmp8BB.f1=& Cyc_nocyc_setjmp_r,_tmp8BB)))),_tmp8BA)))),((_tmp8BE->f5=(
(_tmp8B9="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp8B9,sizeof(char),46))),_tmp8BE)))))))))))),((_tmp840[49]=(
# 1336
(_tmp8C4=_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4->f1=((_tmp8C3="-printalleffects",_tag_dyneither(_tmp8C3,sizeof(char),17))),((_tmp8C4->f2=0,((_tmp8C4->f3=((_tmp8C2="",_tag_dyneither(_tmp8C2,sizeof(char),1))),((_tmp8C4->f4=(void*)(
(_tmp8C0=_region_malloc(r,sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=3,((_tmp8C1.f1=& Cyc_print_all_effects_r,_tmp8C1)))),_tmp8C0)))),((_tmp8C4->f5=(
(_tmp8BF="Print effects for functions (type debugging)",_tag_dyneither(_tmp8BF,sizeof(char),45))),_tmp8C4)))))))))))),((_tmp840[48]=(
# 1333
(_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA->f1=((_tmp8C9="-printfullevars",_tag_dyneither(_tmp8C9,sizeof(char),16))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C8="",_tag_dyneither(_tmp8C8,sizeof(char),1))),((_tmp8CA->f4=(void*)(
(_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=3,((_tmp8C7.f1=& Cyc_print_full_evars_r,_tmp8C7)))),_tmp8C6)))),((_tmp8CA->f5=(
(_tmp8C5="Print full information for evars (type debugging)",_tag_dyneither(_tmp8C5,sizeof(char),50))),_tmp8CA)))))))))))),((_tmp840[47]=(
# 1330
(_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CF="-printallkinds",_tag_dyneither(_tmp8CF,sizeof(char),15))),((_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CE="",_tag_dyneither(_tmp8CE,sizeof(char),1))),((_tmp8D0->f4=(void*)(
(_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=3,((_tmp8CD.f1=& Cyc_print_all_kinds_r,_tmp8CD)))),_tmp8CC)))),((_tmp8D0->f5=(
(_tmp8CB="Always print kinds of type variables",_tag_dyneither(_tmp8CB,sizeof(char),37))),_tmp8D0)))))))))))),((_tmp840[46]=(
# 1327
(_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6->f1=((_tmp8D5="-printalltvars",_tag_dyneither(_tmp8D5,sizeof(char),15))),((_tmp8D6->f2=0,((_tmp8D6->f3=((_tmp8D4="",_tag_dyneither(_tmp8D4,sizeof(char),1))),((_tmp8D6->f4=(void*)(
(_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=3,((_tmp8D3.f1=& Cyc_print_all_tvars_r,_tmp8D3)))),_tmp8D2)))),((_tmp8D6->f5=(
(_tmp8D1="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp8D1,sizeof(char),57))),_tmp8D6)))))))))))),((_tmp840[45]=(
# 1324
(_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8DB="-noexpandtypedefs",_tag_dyneither(_tmp8DB,sizeof(char),18))),((_tmp8DC->f2=0,((_tmp8DC->f3=((_tmp8DA="",_tag_dyneither(_tmp8DA,sizeof(char),1))),((_tmp8DC->f4=(void*)(
(_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=3,((_tmp8D9.f1=& Cyc_noexpand_r,_tmp8D9)))),_tmp8D8)))),((_tmp8DC->f5=(
(_tmp8D7="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp8D7,sizeof(char),41))),_tmp8DC)))))))))))),((_tmp840[44]=(
# 1321
(_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2->f1=((_tmp8E1="-noremoveunused",_tag_dyneither(_tmp8E1,sizeof(char),16))),((_tmp8E2->f2=0,((_tmp8E2->f3=((_tmp8E0="",_tag_dyneither(_tmp8E0,sizeof(char),1))),((_tmp8E2->f4=(void*)(
(_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=3,((_tmp8DF.f1=& Cyc_noshake_r,_tmp8DF)))),_tmp8DE)))),((_tmp8E2->f5=(
(_tmp8DD="Don't remove externed variables that aren't used",_tag_dyneither(_tmp8DD,sizeof(char),49))),_tmp8E2)))))))))))),((_tmp840[43]=(
# 1318
(_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8->f1=((_tmp8E7="-nogc",_tag_dyneither(_tmp8E7,sizeof(char),6))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E6="",_tag_dyneither(_tmp8E6,sizeof(char),1))),((_tmp8E8->f4=(void*)(
(_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=3,((_tmp8E5.f1=& Cyc_nogc_r,_tmp8E5)))),_tmp8E4)))),((_tmp8E8->f5=(
(_tmp8E3="Don't link in the garbage collector",_tag_dyneither(_tmp8E3,sizeof(char),36))),_tmp8E8)))))))))))),((_tmp840[42]=(
# 1315
(_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE->f1=((_tmp8ED="-nocyc",_tag_dyneither(_tmp8ED,sizeof(char),7))),((_tmp8EE->f2=0,((_tmp8EE->f3=((_tmp8EC="",_tag_dyneither(_tmp8EC,sizeof(char),1))),((_tmp8EE->f4=(void*)(
(_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=0,((_tmp8EB.f1=Cyc_set_nocyc,_tmp8EB)))),_tmp8EA)))),((_tmp8EE->f5=(
(_tmp8E9="Don't add implicit namespace Cyc",_tag_dyneither(_tmp8E9,sizeof(char),33))),_tmp8EE)))))))))))),((_tmp840[41]=(
# 1312
(_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=((_tmp8F3="-no-cpp-precomp",_tag_dyneither(_tmp8F3,sizeof(char),16))),((_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F2="",_tag_dyneither(_tmp8F2,sizeof(char),1))),((_tmp8F4->f4=(void*)(
(_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=0,((_tmp8F1.f1=Cyc_set_nocppprecomp,_tmp8F1)))),_tmp8F0)))),((_tmp8F4->f5=(
(_tmp8EF="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp8EF,sizeof(char),40))),_tmp8F4)))))))))))),((_tmp840[40]=(
# 1309
(_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA->f1=((_tmp8F9="-use-cpp",_tag_dyneither(_tmp8F9,sizeof(char),9))),((_tmp8FA->f2=0,((_tmp8FA->f3=((_tmp8F8="<path>",_tag_dyneither(_tmp8F8,sizeof(char),7))),((_tmp8FA->f4=(void*)(
(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=5,((_tmp8F7.f1=Cyc_set_cpp,_tmp8F7)))),_tmp8F6)))),((_tmp8FA->f5=(
(_tmp8F5="Indicate which preprocessor to use",_tag_dyneither(_tmp8F5,sizeof(char),35))),_tmp8FA)))))))))))),((_tmp840[39]=(
# 1306
(_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900->f1=((_tmp8FF="--inline-checks",_tag_dyneither(_tmp8FF,sizeof(char),16))),((_tmp900->f2=0,((_tmp900->f3=((_tmp8FE="",_tag_dyneither(_tmp8FE,sizeof(char),1))),((_tmp900->f4=(void*)(
(_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=0,((_tmp8FD.f1=Cyc_set_inline_functions,_tmp8FD)))),_tmp8FC)))),((_tmp900->f5=(
(_tmp8FB="Inline bounds checks instead of #define",_tag_dyneither(_tmp8FB,sizeof(char),40))),_tmp900)))))))))))),((_tmp840[38]=(
# 1303
(_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp905="--noboundschecks",_tag_dyneither(_tmp905,sizeof(char),17))),((_tmp906->f2=0,((_tmp906->f3=((_tmp904="",_tag_dyneither(_tmp904,sizeof(char),1))),((_tmp906->f4=(void*)(
(_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=0,((_tmp903.f1=Cyc_set_noboundschecks,_tmp903)))),_tmp902)))),((_tmp906->f5=(
(_tmp901="Disable bounds checks",_tag_dyneither(_tmp901,sizeof(char),22))),_tmp906)))))))))))),((_tmp840[37]=(
# 1300
(_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C->f1=((_tmp90B="--nonullchecks",_tag_dyneither(_tmp90B,sizeof(char),15))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp90A="",_tag_dyneither(_tmp90A,sizeof(char),1))),((_tmp90C->f4=(void*)(
(_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=0,((_tmp909.f1=Cyc_set_nonullchecks,_tmp909)))),_tmp908)))),((_tmp90C->f5=(
(_tmp907="Disable null checks",_tag_dyneither(_tmp907,sizeof(char),20))),_tmp90C)))))))))))),((_tmp840[36]=(
# 1297
(_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912->f1=((_tmp911="--nochecks",_tag_dyneither(_tmp911,sizeof(char),11))),((_tmp912->f2=0,((_tmp912->f3=((_tmp910="",_tag_dyneither(_tmp910,sizeof(char),1))),((_tmp912->f4=(void*)(
(_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=0,((_tmp90F.f1=Cyc_set_nochecks,_tmp90F)))),_tmp90E)))),((_tmp912->f5=(
(_tmp90D="Disable bounds/null checks",_tag_dyneither(_tmp90D,sizeof(char),27))),_tmp912)))))))))))),((_tmp840[35]=(
# 1294
(_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp917="--nolineno",_tag_dyneither(_tmp917,sizeof(char),11))),((_tmp918->f2=0,((_tmp918->f3=((_tmp916="",_tag_dyneither(_tmp916,sizeof(char),1))),((_tmp918->f4=(void*)(
(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=0,((_tmp915.f1=Cyc_clear_lineno,_tmp915)))),_tmp914)))),((_tmp918->f5=(
(_tmp913="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp913,sizeof(char),63))),_tmp918)))))))))))),((_tmp840[34]=(
# 1291
(_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((_tmp91D="-save-c",_tag_dyneither(_tmp91D,sizeof(char),8))),((_tmp91E->f2=0,((_tmp91E->f3=((_tmp91C="",_tag_dyneither(_tmp91C,sizeof(char),1))),((_tmp91E->f4=(void*)(
(_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=3,((_tmp91B.f1=& Cyc_save_c_r,_tmp91B)))),_tmp91A)))),((_tmp91E->f5=(
(_tmp919="Don't delete temporary C files",_tag_dyneither(_tmp919,sizeof(char),31))),_tmp91E)))))))))))),((_tmp840[33]=(
# 1288
(_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924->f1=((_tmp923="-save-temps",_tag_dyneither(_tmp923,sizeof(char),12))),((_tmp924->f2=0,((_tmp924->f3=((_tmp922="",_tag_dyneither(_tmp922,sizeof(char),1))),((_tmp924->f4=(void*)(
(_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=0,((_tmp921.f1=Cyc_set_save_temps,_tmp921)))),_tmp920)))),((_tmp924->f5=(
(_tmp91F="Don't delete temporary files",_tag_dyneither(_tmp91F,sizeof(char),29))),_tmp924)))))))))))),((_tmp840[32]=(
# 1285
(_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->f1=((_tmp929="-c-comp",_tag_dyneither(_tmp929,sizeof(char),8))),((_tmp92A->f2=0,((_tmp92A->f3=((_tmp928=" <compiler>",_tag_dyneither(_tmp928,sizeof(char),12))),((_tmp92A->f4=(void*)(
(_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=5,((_tmp927.f1=Cyc_set_c_compiler,_tmp927)))),_tmp926)))),((_tmp92A->f5=(
(_tmp925="Produce C output for the given compiler",_tag_dyneither(_tmp925,sizeof(char),40))),_tmp92A)))))))))))),((_tmp840[31]=(
# 1282
(_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->f1=((_tmp92F="-un-gcc",_tag_dyneither(_tmp92F,sizeof(char),8))),((_tmp930->f2=0,((_tmp930->f3=((_tmp92E="",_tag_dyneither(_tmp92E,sizeof(char),1))),((_tmp930->f4=(void*)(
(_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92D.tag=0,((_tmp92D.f1=Cyc_set_tovc,_tmp92D)))),_tmp92C)))),((_tmp930->f5=(
(_tmp92B="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp92B,sizeof(char),57))),_tmp930)))))))))))),((_tmp840[30]=(
# 1279
(_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->f1=((_tmp935="-no-elim-statement-expressions",_tag_dyneither(_tmp935,sizeof(char),31))),((_tmp936->f2=0,((_tmp936->f3=((_tmp934="",_tag_dyneither(_tmp934,sizeof(char),1))),((_tmp936->f4=(void*)(
(_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=4,((_tmp933.f1=& Cyc_elim_se_r,_tmp933)))),_tmp932)))),((_tmp936->f5=(
(_tmp931="Do not avoid statement expressions in C output",_tag_dyneither(_tmp931,sizeof(char),47))),_tmp936)))))))))))),((_tmp840[29]=(
# 1275
(_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B="-elim-statement-expressions",_tag_dyneither(_tmp93B,sizeof(char),28))),((_tmp93C->f2=0,((_tmp93C->f3=((_tmp93A="",_tag_dyneither(_tmp93A,sizeof(char),1))),((_tmp93C->f4=(void*)(
(_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=3,((_tmp939.f1=& Cyc_elim_se_r,_tmp939)))),_tmp938)))),((_tmp93C->f5=(
# 1278
(_tmp937="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp937,sizeof(char),66))),_tmp93C)))))))))))),((_tmp840[28]=(
# 1272
(_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942->f1=((_tmp941="-up",_tag_dyneither(_tmp941,sizeof(char),4))),((_tmp942->f2=0,((_tmp942->f3=((_tmp940="",_tag_dyneither(_tmp940,sizeof(char),1))),((_tmp942->f4=(void*)(
(_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=4,((_tmp93F.f1=& Cyc_pp_r,_tmp93F)))),_tmp93E)))),((_tmp942->f5=(
(_tmp93D="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp93D,sizeof(char),55))),_tmp942)))))))))))),((_tmp840[27]=(
# 1269
(_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948->f1=((_tmp947="-pp",_tag_dyneither(_tmp947,sizeof(char),4))),((_tmp948->f2=0,((_tmp948->f3=((_tmp946="",_tag_dyneither(_tmp946,sizeof(char),1))),((_tmp948->f4=(void*)(
(_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=3,((_tmp945.f1=& Cyc_pp_r,_tmp945)))),_tmp944)))),((_tmp948->f5=(
(_tmp943="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp943,sizeof(char),47))),_tmp948)))))))))))),((_tmp840[26]=(
# 1266
(_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->f1=((_tmp94D="-ic",_tag_dyneither(_tmp94D,sizeof(char),4))),((_tmp94E->f2=0,((_tmp94E->f3=((_tmp94C="",_tag_dyneither(_tmp94C,sizeof(char),1))),((_tmp94E->f4=(void*)(
(_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=3,((_tmp94B.f1=& Cyc_ic_r,_tmp94B)))),_tmp94A)))),((_tmp94E->f5=(
(_tmp949="Activate the link-checker",_tag_dyneither(_tmp949,sizeof(char),26))),_tmp94E)))))))))))),((_tmp840[25]=(
# 1263
(_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->f1=((_tmp953="-stopafter-toc",_tag_dyneither(_tmp953,sizeof(char),15))),((_tmp954->f2=0,((_tmp954->f3=((_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),((_tmp954->f4=(void*)(
(_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=3,((_tmp951.f1=& Cyc_toc_r,_tmp951)))),_tmp950)))),((_tmp954->f5=(
(_tmp94F="Stop after translation to C",_tag_dyneither(_tmp94F,sizeof(char),28))),_tmp954)))))))))))),((_tmp840[24]=(
# 1260
(_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A->f1=((_tmp959="-stopafter-cf",_tag_dyneither(_tmp959,sizeof(char),14))),((_tmp95A->f2=0,((_tmp95A->f3=((_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),((_tmp95A->f4=(void*)(
(_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=3,((_tmp957.f1=& Cyc_cf_r,_tmp957)))),_tmp956)))),((_tmp95A->f5=(
(_tmp955="Stop after control-flow checking",_tag_dyneither(_tmp955,sizeof(char),33))),_tmp95A)))))))))))),((_tmp840[23]=(
# 1257
(_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960->f1=((_tmp95F="-noprint",_tag_dyneither(_tmp95F,sizeof(char),9))),((_tmp960->f2=0,((_tmp960->f3=((_tmp95E="",_tag_dyneither(_tmp95E,sizeof(char),1))),((_tmp960->f4=(void*)(
(_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95D.tag=3,((_tmp95D.f1=& Cyc_noprint_r,_tmp95D)))),_tmp95C)))),((_tmp960->f5=(
(_tmp95B="Do not print output (when stopping early)",_tag_dyneither(_tmp95B,sizeof(char),42))),_tmp960)))))))))))),((_tmp840[22]=(
# 1254
(_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966->f1=((_tmp965="-stopafter-tc",_tag_dyneither(_tmp965,sizeof(char),14))),((_tmp966->f2=0,((_tmp966->f3=((_tmp964="",_tag_dyneither(_tmp964,sizeof(char),1))),((_tmp966->f4=(void*)(
(_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=3,((_tmp963.f1=& Cyc_tc_r,_tmp963)))),_tmp962)))),((_tmp966->f5=(
(_tmp961="Stop after type checking",_tag_dyneither(_tmp961,sizeof(char),25))),_tmp966)))))))))))),((_tmp840[21]=(
# 1251
(_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=((_tmp96B="-stopafter-parse",_tag_dyneither(_tmp96B,sizeof(char),17))),((_tmp96C->f2=0,((_tmp96C->f3=((_tmp96A="",_tag_dyneither(_tmp96A,sizeof(char),1))),((_tmp96C->f4=(void*)(
(_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=3,((_tmp969.f1=& Cyc_parseonly_r,_tmp969)))),_tmp968)))),((_tmp96C->f5=(
(_tmp967="Stop after parsing",_tag_dyneither(_tmp967,sizeof(char),19))),_tmp96C)))))))))))),((_tmp840[20]=(
# 1248
(_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972->f1=((_tmp971="-E",_tag_dyneither(_tmp971,sizeof(char),3))),((_tmp972->f2=0,((_tmp972->f3=((_tmp970="",_tag_dyneither(_tmp970,sizeof(char),1))),((_tmp972->f4=(void*)(
(_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=3,((_tmp96F.f1=& Cyc_stop_after_cpp_r,_tmp96F)))),_tmp96E)))),((_tmp972->f5=(
(_tmp96D="Stop after preprocessing",_tag_dyneither(_tmp96D,sizeof(char),25))),_tmp972)))))))))))),((_tmp840[19]=(
# 1245
(_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->f1=((_tmp977="-Wall",_tag_dyneither(_tmp977,sizeof(char),6))),((_tmp978->f2=0,((_tmp978->f3=((_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp978->f4=(void*)(
(_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=0,((_tmp975.f1=Cyc_set_all_warnings,_tmp975)))),_tmp974)))),((_tmp978->f5=(
(_tmp973="Turn on all warnings",_tag_dyneither(_tmp973,sizeof(char),21))),_tmp978)))))))))))),((_tmp840[18]=(
# 1242
(_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E->f1=((_tmp97D="-Woverride",_tag_dyneither(_tmp97D,sizeof(char),11))),((_tmp97E->f2=0,((_tmp97E->f3=((_tmp97C="",_tag_dyneither(_tmp97C,sizeof(char),1))),((_tmp97E->f4=(void*)(
(_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97B.tag=3,((_tmp97B.f1=& Cyc_Tcenv_warn_override,_tmp97B)))),_tmp97A)))),((_tmp97E->f5=(
(_tmp979="Warn when a local variable overrides an existing variable",_tag_dyneither(_tmp979,sizeof(char),58))),_tmp97E)))))))))))),((_tmp840[17]=(
# 1239
(_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984->f1=((_tmp983="-Wlose-unique",_tag_dyneither(_tmp983,sizeof(char),14))),((_tmp984->f2=0,((_tmp984->f3=((_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),((_tmp984->f4=(void*)(
(_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=3,((_tmp981.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp981)))),_tmp980)))),((_tmp984->f5=(
(_tmp97F="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmp97F,sizeof(char),49))),_tmp984)))))))))))),((_tmp840[16]=(
# 1235
(_tmp98A=_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A->f1=((_tmp989="-b",_tag_dyneither(_tmp989,sizeof(char),3))),((_tmp98A->f2=0,((_tmp98A->f3=((_tmp988="<machine>",_tag_dyneither(_tmp988,sizeof(char),10))),((_tmp98A->f4=(void*)(
(_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=5,((_tmp987.f1=Cyc_set_target_arch,_tmp987)))),_tmp986)))),((_tmp98A->f5=(
# 1238
(_tmp985="Set target machine",_tag_dyneither(_tmp985,sizeof(char),19))),_tmp98A)))))))))))),((_tmp840[15]=(
# 1232
(_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=((_tmp98F="-MT",_tag_dyneither(_tmp98F,sizeof(char),4))),((_tmp990->f2=0,((_tmp990->f3=((_tmp98E=" <target>",_tag_dyneither(_tmp98E,sizeof(char),10))),((_tmp990->f4=(void*)(
(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=5,((_tmp98D.f1=Cyc_set_dependencies_target,_tmp98D)))),_tmp98C)))),((_tmp990->f5=(
(_tmp98B="Give target for dependencies",_tag_dyneither(_tmp98B,sizeof(char),29))),_tmp990)))))))))))),((_tmp840[14]=(
# 1228
(_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=((_tmp995="-MG",_tag_dyneither(_tmp995,sizeof(char),4))),((_tmp996->f2=0,((_tmp996->f3=((_tmp994="",_tag_dyneither(_tmp994,sizeof(char),1))),((_tmp996->f4=(void*)(
(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=1,((_tmp993.f1=Cyc_add_cpparg,_tmp993)))),_tmp992)))),((_tmp996->f5=(
# 1231
(_tmp991="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmp991,sizeof(char),68))),_tmp996)))))))))))),((_tmp840[13]=(
# 1225
(_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->f1=((_tmp99B="-M",_tag_dyneither(_tmp99B,sizeof(char),3))),((_tmp99C->f2=0,((_tmp99C->f3=((_tmp99A="",_tag_dyneither(_tmp99A,sizeof(char),1))),((_tmp99C->f4=(void*)(
(_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=0,((_tmp999.f1=Cyc_set_produce_dependencies,_tmp999)))),_tmp998)))),((_tmp99C->f5=(
(_tmp997="Produce dependencies",_tag_dyneither(_tmp997,sizeof(char),21))),_tmp99C)))))))))))),((_tmp840[12]=(
# 1222
(_tmp9A2=_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2->f1=((_tmp9A1="-S",_tag_dyneither(_tmp9A1,sizeof(char),3))),((_tmp9A2->f2=0,((_tmp9A2->f3=((_tmp9A0="",_tag_dyneither(_tmp9A0,sizeof(char),1))),((_tmp9A2->f4=(void*)(
(_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=0,((_tmp99F.f1=Cyc_set_stop_after_asmfile,_tmp99F)))),_tmp99E)))),((_tmp9A2->f5=(
(_tmp99D="Stop after producing assembly code",_tag_dyneither(_tmp99D,sizeof(char),35))),_tmp9A2)))))))))))),((_tmp840[11]=(
# 1219
(_tmp9A8=_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8->f1=((_tmp9A7="-pa",_tag_dyneither(_tmp9A7,sizeof(char),4))),((_tmp9A8->f2=0,((_tmp9A8->f3=((_tmp9A6="",_tag_dyneither(_tmp9A6,sizeof(char),1))),((_tmp9A8->f4=(void*)(
(_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=0,((_tmp9A5.f1=Cyc_set_pa,_tmp9A5)))),_tmp9A4)))),((_tmp9A8->f5=(
(_tmp9A3="Compile for profiling with aprof",_tag_dyneither(_tmp9A3,sizeof(char),33))),_tmp9A8)))))))))))),((_tmp840[10]=(
# 1216
(_tmp9AE=_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE->f1=((_tmp9AD="-x",_tag_dyneither(_tmp9AD,sizeof(char),3))),((_tmp9AE->f2=0,((_tmp9AE->f3=((_tmp9AC=" <language>",_tag_dyneither(_tmp9AC,sizeof(char),12))),((_tmp9AE->f4=(void*)(
(_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=5,((_tmp9AB.f1=Cyc_set_inputtype,_tmp9AB)))),_tmp9AA)))),((_tmp9AE->f5=(
(_tmp9A9="Specify <language> for the following input files",_tag_dyneither(_tmp9A9,sizeof(char),49))),_tmp9AE)))))))))))),((_tmp840[9]=(
# 1213
(_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->f1=((_tmp9B3="-c",_tag_dyneither(_tmp9B3,sizeof(char),3))),((_tmp9B4->f2=0,((_tmp9B4->f3=((_tmp9B2="",_tag_dyneither(_tmp9B2,sizeof(char),1))),((_tmp9B4->f4=(void*)(
(_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=0,((_tmp9B1.f1=Cyc_set_stop_after_objectfile,_tmp9B1)))),_tmp9B0)))),((_tmp9B4->f5=(
(_tmp9AF="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmp9AF,sizeof(char),61))),_tmp9B4)))))))))))),((_tmp840[8]=(
# 1210
(_tmp9BA=_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA->f1=((_tmp9B9="-m",_tag_dyneither(_tmp9B9,sizeof(char),3))),((_tmp9BA->f2=1,((_tmp9BA->f3=((_tmp9B8="<option>",_tag_dyneither(_tmp9B8,sizeof(char),9))),((_tmp9BA->f4=(void*)(
(_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=1,((_tmp9B7.f1=Cyc_add_marg,_tmp9B7)))),_tmp9B6)))),((_tmp9BA->f5=(
(_tmp9B5="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmp9B5,sizeof(char),52))),_tmp9BA)))))))))))),((_tmp840[7]=(
# 1207
(_tmp9C0=_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0->f1=((_tmp9BF="-l",_tag_dyneither(_tmp9BF,sizeof(char),3))),((_tmp9C0->f2=1,((_tmp9C0->f3=((_tmp9BE="<libname>",_tag_dyneither(_tmp9BE,sizeof(char),10))),((_tmp9C0->f4=(void*)(
(_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=1,((_tmp9BD.f1=Cyc_add_libarg,_tmp9BD)))),_tmp9BC)))),((_tmp9C0->f5=(
(_tmp9BB="Library file",_tag_dyneither(_tmp9BB,sizeof(char),13))),_tmp9C0)))))))))))),((_tmp840[6]=(
# 1204
(_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->f1=((_tmp9C5="-L",_tag_dyneither(_tmp9C5,sizeof(char),3))),((_tmp9C6->f2=1,((_tmp9C6->f3=((_tmp9C4="<dir>",_tag_dyneither(_tmp9C4,sizeof(char),6))),((_tmp9C6->f4=(void*)(
(_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=1,((_tmp9C3.f1=Cyc_add_ccarg,_tmp9C3)))),_tmp9C2)))),((_tmp9C6->f5=(
(_tmp9C1="Add to the list of directories for -l",_tag_dyneither(_tmp9C1,sizeof(char),38))),_tmp9C6)))))))))))),((_tmp840[5]=(
# 1201
(_tmp9CC=_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC->f1=((_tmp9CB="-I",_tag_dyneither(_tmp9CB,sizeof(char),3))),((_tmp9CC->f2=1,((_tmp9CC->f3=((_tmp9CA="<dir>",_tag_dyneither(_tmp9CA,sizeof(char),6))),((_tmp9CC->f4=(void*)(
(_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=1,((_tmp9C9.f1=Cyc_add_cpparg,_tmp9C9)))),_tmp9C8)))),((_tmp9CC->f5=(
(_tmp9C7="Add to the list of directories to search for include files",_tag_dyneither(_tmp9C7,sizeof(char),59))),_tmp9CC)))))))))))),((_tmp840[4]=(
# 1198
(_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2->f1=((_tmp9D1="-B",_tag_dyneither(_tmp9D1,sizeof(char),3))),((_tmp9D2->f2=1,((_tmp9D2->f3=((_tmp9D0="<file>",_tag_dyneither(_tmp9D0,sizeof(char),7))),((_tmp9D2->f4=(void*)(
(_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=1,((_tmp9CF.f1=Cyc_add_cyclone_exec_path,_tmp9CF)))),_tmp9CE)))),((_tmp9D2->f5=(
(_tmp9CD="Add to the list of directories to search for compiler files",_tag_dyneither(_tmp9CD,sizeof(char),60))),_tmp9D2)))))))))))),((_tmp840[3]=(
# 1195
(_tmp9D8=_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8->f1=((_tmp9D7="-D",_tag_dyneither(_tmp9D7,sizeof(char),3))),((_tmp9D8->f2=1,((_tmp9D8->f3=((_tmp9D6="<name>[=<value>]",_tag_dyneither(_tmp9D6,sizeof(char),17))),((_tmp9D8->f4=(void*)(
(_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=1,((_tmp9D5.f1=Cyc_add_cpparg,_tmp9D5)))),_tmp9D4)))),((_tmp9D8->f5=(
(_tmp9D3="Pass definition to preprocessor",_tag_dyneither(_tmp9D3,sizeof(char),32))),_tmp9D8)))))))))))),((_tmp840[2]=(
# 1192
(_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->f1=((_tmp9DD="-o",_tag_dyneither(_tmp9DD,sizeof(char),3))),((_tmp9DE->f2=0,((_tmp9DE->f3=((_tmp9DC=" <file>",_tag_dyneither(_tmp9DC,sizeof(char),8))),((_tmp9DE->f4=(void*)(
(_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=5,((_tmp9DB.f1=Cyc_set_output_file,_tmp9DB)))),_tmp9DA)))),((_tmp9DE->f5=(
(_tmp9D9="Set the output file name to <file>",_tag_dyneither(_tmp9D9,sizeof(char),35))),_tmp9DE)))))))))))),((_tmp840[1]=(
# 1189
(_tmp9E4=_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4->f1=((_tmp9E3="--version",_tag_dyneither(_tmp9E3,sizeof(char),10))),((_tmp9E4->f2=0,((_tmp9E4->f3=((_tmp9E2="",_tag_dyneither(_tmp9E2,sizeof(char),1))),((_tmp9E4->f4=(void*)(
(_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=0,((_tmp9E1.f1=Cyc_print_version,_tmp9E1)))),_tmp9E0)))),((_tmp9E4->f5=(
(_tmp9DF="Print version information and exit",_tag_dyneither(_tmp9DF,sizeof(char),35))),_tmp9E4)))))))))))),((_tmp840[0]=(
# 1186
(_tmp9EA=_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA->f1=((_tmp9E9="-v",_tag_dyneither(_tmp9E9,sizeof(char),3))),((_tmp9EA->f2=0,((_tmp9EA->f3=((_tmp9E8="",_tag_dyneither(_tmp9E8,sizeof(char),1))),((_tmp9EA->f4=(void*)(
(_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=3,((_tmp9E7.f1=& Cyc_v_r,_tmp9E7)))),_tmp9E6)))),((_tmp9EA->f5=(
(_tmp9E5="Print compilation stages verbosely",_tag_dyneither(_tmp9E5,sizeof(char),35))),_tmp9EA)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp840,sizeof(struct _tuple24*),71)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1404
const char*_tmp9EB;struct _dyneither_ptr optstring=(_tmp9EB="Options:",_tag_dyneither(_tmp9EB,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp163);
# 1409
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1417
char*_tmp9EC;struct _dyneither_ptr cyclone_exec_prefix=(_tmp9EC=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmp9EC,sizeof(char),_get_zero_arr_size_char((void*)_tmp9EC,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmp9EF;struct Cyc_List_List*_tmp9EE;Cyc_cyclone_exec_path=((_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE->hd=((_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF[0]=cyclone_exec_prefix,_tmp9EF)))),((_tmp9EE->tl=Cyc_cyclone_exec_path,_tmp9EE))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmp9F5;const char*_tmp9F4;struct Cyc_List_List*_tmp9F3;Cyc_cyclone_exec_path=(
(_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3->hd=((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmp9F4="cyc-lib",_tag_dyneither(_tmp9F4,sizeof(char),8)))),_tmp9F5)))),((_tmp9F3->tl=Cyc_cyclone_exec_path,_tmp9F3))))));}
# 1425
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1433
const char*_tmp9F6;struct _dyneither_ptr _tmp16A=Cyc_do_find(cyclone_arch_path,((_tmp9F6="cycspecs",_tag_dyneither(_tmp9F6,sizeof(char),9))));
if(Cyc_v_r){const char*_tmp9FA;void*_tmp9F9[1];struct Cyc_String_pa_PrintArg_struct _tmp9F8;(_tmp9F8.tag=0,((_tmp9F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16A),((_tmp9F9[0]=& _tmp9F8,Cyc_fprintf(Cyc_stderr,((_tmp9FA="Reading from specs file %s\n",_tag_dyneither(_tmp9FA,sizeof(char),28))),_tag_dyneither(_tmp9F9,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp16E=Cyc_read_specs(_tmp16A);
struct _dyneither_ptr _tmp16F=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp170;_push_handler(& _tmp170);{int _tmp172=0;if(setjmp(_tmp170.handler))_tmp172=1;if(!_tmp172){
{struct _dyneither_ptr _tmp173=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp16E,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp16F=Cyc_split_specs(_tmp173);}
# 1438
;_pop_handler();}else{void*_tmp171=(void*)_exn_thrown;void*_tmp175=_tmp171;void*_tmp176;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp175)->tag == Cyc_Core_Not_found){_LL47: _LL48:
# 1441
 goto _LL46;}else{_LL49: _tmp176=_tmp175;_LL4A:(int)_rethrow(_tmp176);}_LL46:;}};}
# 1443
if((struct _dyneither_ptr*)_tmp16F.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp16F);}
# 1448
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp164);{
const char*_tmp9FB;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmp9FB="",_tag_dyneither(_tmp9FB,sizeof(char),1))))== 0){
# 1466 "cyclone.cyc"
const char*_tmp178=Cyc_produce_dependencies?"":" -E";
const char*_tmpA03;void*_tmpA02[3];struct Cyc_String_pa_PrintArg_struct _tmpA01;const char*_tmpA00;struct Cyc_String_pa_PrintArg_struct _tmp9FF;struct Cyc_String_pa_PrintArg_struct _tmp9FE;Cyc_set_cpp((struct _dyneither_ptr)((_tmp9FE.tag=0,((_tmp9FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16A),((_tmp9FF.tag=0,((_tmp9FF.f1=(struct _dyneither_ptr)((_tmpA00=_tmp178,_tag_dyneither(_tmpA00,sizeof(char),_get_zero_arr_size_char((void*)_tmpA00,1)))),((_tmpA01.tag=0,((_tmpA01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA02[0]=& _tmpA01,((_tmpA02[1]=& _tmp9FF,((_tmpA02[2]=& _tmp9FE,Cyc_aprintf(((_tmpA03="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA03,sizeof(char),23))),_tag_dyneither(_tmpA02,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1470
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpA06;void*_tmpA05;(_tmpA05=0,Cyc_fprintf(Cyc_stderr,((_tmpA06="missing file\n",_tag_dyneither(_tmpA06,sizeof(char),14))),_tag_dyneither(_tmpA05,sizeof(void*),0)));}
exit(1);}
# 1479
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpA07;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpA07="cyc_setjmp.h",_tag_dyneither(_tmpA07,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpA08;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpA08="cyc_include.h",_tag_dyneither(_tmpA08,sizeof(char),14))));}
# 1485
{struct Cyc_List_List*_tmp33D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp33D != 0;_tmp33D=_tmp33D->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp33D->hd));
if(Cyc_compile_failure){int _tmp33E=1;_npop_handler(0);return _tmp33E;}}}
# 1490
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp33F=0;_npop_handler(0);return _tmp33F;}
# 1496
if(Cyc_ccargs == 0){int _tmp340=0;_npop_handler(0);return _tmp340;}
# 1499
{const char*_tmpA09;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpA09="-L",_tag_dyneither(_tmpA09,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1501
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpA0A;struct _dyneither_ptr _tmp342=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpA0A=" ",_tag_dyneither(_tmpA0A,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpA0C;struct Cyc_List_List*_tmpA0B;struct _dyneither_ptr _tmp343=Cyc_str_sepstr(((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpA0B->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpA0B)))))),((_tmpA0C=" ",_tag_dyneither(_tmpA0C,sizeof(char),2))));
# 1507
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpA0E;const char*_tmpA0D;int _tmp344=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1512
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA0E=".a",_tag_dyneither(_tmpA0E,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA0D=".lib",_tag_dyneither(_tmpA0D,sizeof(char),5))));
if(_tmp344){
stdlib=0;{
const char*_tmpA0F;stdlib_string=(struct _dyneither_ptr)((_tmpA0F="",_tag_dyneither(_tmpA0F,sizeof(char),1)));};}else{
# 1520
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpA10;libcyc_flag=((_tmpA10="-lcyc_a",_tag_dyneither(_tmpA10,sizeof(char),8)));}
{const char*_tmpA11;nogc_filename=((_tmpA11="nogc_a.a",_tag_dyneither(_tmpA11,sizeof(char),9)));}{
const char*_tmpA12;runtime_filename=((_tmpA12="runtime_cyc_a.o",_tag_dyneither(_tmpA12,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpA13;libcyc_flag=((_tmpA13="-lcyc_nocheck",_tag_dyneither(_tmpA13,sizeof(char),14)));}
{const char*_tmpA14;nogc_filename=((_tmpA14="nogc.a",_tag_dyneither(_tmpA14,sizeof(char),7)));}{
const char*_tmpA15;runtime_filename=((_tmpA15="runtime_cyc.o",_tag_dyneither(_tmpA15,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpA16;libcyc_flag=((_tmpA16="-lcyc_pg",_tag_dyneither(_tmpA16,sizeof(char),9)));}
{const char*_tmpA17;runtime_filename=((_tmpA17="runtime_cyc_pg.o",_tag_dyneither(_tmpA17,sizeof(char),17)));}{
const char*_tmpA18;nogc_filename=((_tmpA18="nogc_pg.a",_tag_dyneither(_tmpA18,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1538
{const char*_tmpA19;libcyc_flag=((_tmpA19="-lcycboot",_tag_dyneither(_tmpA19,sizeof(char),10)));}
{const char*_tmpA1A;nogc_filename=((_tmpA1A="nogc.a",_tag_dyneither(_tmpA1A,sizeof(char),7)));}{
const char*_tmpA1B;runtime_filename=((_tmpA1B="runtime_cyc.o",_tag_dyneither(_tmpA1B,sizeof(char),14)));};}else{
# 1542
{const char*_tmpA1C;libcyc_flag=((_tmpA1C="-lcyc",_tag_dyneither(_tmpA1C,sizeof(char),6)));}
{const char*_tmpA1D;nogc_filename=((_tmpA1D="nogc.a",_tag_dyneither(_tmpA1D,sizeof(char),7)));}{
const char*_tmpA1E;runtime_filename=((_tmpA1E="runtime_cyc.o",_tag_dyneither(_tmpA1E,sizeof(char),14)));};}}}}
# 1546
{const char*_tmpA1F;gc_filename=((_tmpA1F="gc.a",_tag_dyneither(_tmpA1F,sizeof(char),5)));}{
# 1548
struct _dyneither_ptr _tmp356=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp357=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1555
stdlib=0;{
const char*_tmpA25;void*_tmpA24[3];struct Cyc_String_pa_PrintArg_struct _tmpA23;struct Cyc_String_pa_PrintArg_struct _tmpA22;struct Cyc_String_pa_PrintArg_struct _tmpA21;stdlib_string=(struct _dyneither_ptr)((_tmpA21.tag=0,((_tmpA21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp356),((_tmpA22.tag=0,((_tmpA22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpA23.tag=0,((_tmpA23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp357),((_tmpA24[0]=& _tmpA23,((_tmpA24[1]=& _tmpA22,((_tmpA24[2]=& _tmpA21,Cyc_aprintf(((_tmpA25=" %s %s %s",_tag_dyneither(_tmpA25,sizeof(char),10))),_tag_dyneither(_tmpA24,sizeof(void*),3))))))))))))))))))));};};}
# 1559
if(Cyc_ic_r){struct _handler_cons _tmp35D;_push_handler(& _tmp35D);{int _tmp35F=0;if(setjmp(_tmp35D.handler))_tmp35F=1;if(!_tmp35F){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp360=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp344){
struct Cyc_List_List*_tmpA26;_tmp360=((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26->hd=Cyc_final_strptr,((_tmpA26->tl=_tmp360,_tmpA26))))));}{
# 1567
struct Cyc_Interface_I*_tmp362=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp360,_tmp360);
if(_tmp362 == 0){
{const char*_tmpA29;void*_tmpA28;(_tmpA28=0,Cyc_fprintf(Cyc_stderr,((_tmpA29="Error: interfaces incompatible\n",_tag_dyneither(_tmpA29,sizeof(char),32))),_tag_dyneither(_tmpA28,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp365=1;_npop_handler(1);return _tmp365;};}
# 1574
if(_tmp344){
if(Cyc_output_file != 0){
const char*_tmpA2D;void*_tmpA2C[1];struct Cyc_String_pa_PrintArg_struct _tmpA2B;struct _dyneither_ptr _tmp366=(_tmpA2B.tag=0,((_tmpA2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA2C[0]=& _tmpA2B,Cyc_aprintf(((_tmpA2D="%s.cycio",_tag_dyneither(_tmpA2D,sizeof(char),9))),_tag_dyneither(_tmpA2C,sizeof(void*),1)))))));
const char*_tmpA2F;const char*_tmpA2E;struct Cyc___cycFILE*_tmp367=Cyc_try_file_open((struct _dyneither_ptr)_tmp366,((_tmpA2E="wb",_tag_dyneither(_tmpA2E,sizeof(char),3))),((_tmpA2F="interface object file",_tag_dyneither(_tmpA2F,sizeof(char),22))));
if(_tmp367 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp368=1;_npop_handler(1);return _tmp368;};}
# 1583
Cyc_Interface_save(_tmp362,_tmp367);
Cyc_file_close(_tmp367);}}else{
# 1587
const char*_tmpA34;const char*_tmpA33;struct _tuple20*_tmpA32;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp362,(
(_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->f1=((_tmpA33="empty interface",_tag_dyneither(_tmpA33,sizeof(char),16))),((_tmpA32->f2=((_tmpA34="global interface",_tag_dyneither(_tmpA34,sizeof(char),17))),_tmpA32)))))))){
{const char*_tmpA37;void*_tmpA36;(_tmpA36=0,Cyc_fprintf(Cyc_stderr,((_tmpA37="Error: some objects are still undefined\n",_tag_dyneither(_tmpA37,sizeof(char),41))),_tag_dyneither(_tmpA36,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp373=1;_npop_handler(1);return _tmp373;};}}};};
# 1560
;_pop_handler();}else{void*_tmp35E=(void*)_exn_thrown;void*_tmp374=_tmp35E;void*_tmp375;_LL4C: _tmp375=_tmp374;_LL4D:
# 1597
{const char*_tmpA41;void*_tmpA40[3];const char*_tmpA3F;struct Cyc_String_pa_PrintArg_struct _tmpA3E;const char*_tmpA3D;struct Cyc_String_pa_PrintArg_struct _tmpA3C;struct Cyc_Int_pa_PrintArg_struct _tmpA3B;(_tmpA3B.tag=1,((_tmpA3B.f1=(unsigned long)Cyc_Core_get_exn_lineno(),((_tmpA3C.tag=0,((_tmpA3C.f1=(struct _dyneither_ptr)((_tmpA3D=Cyc_Core_get_exn_filename(),_tag_dyneither(_tmpA3D,sizeof(char),_get_zero_arr_size_char((void*)_tmpA3D,1)))),((_tmpA3E.tag=0,((_tmpA3E.f1=(struct _dyneither_ptr)((_tmpA3F=Cyc_Core_get_exn_name(_tmp375),_tag_dyneither(_tmpA3F,sizeof(char),_get_zero_arr_size_char((void*)_tmpA3F,1)))),((_tmpA40[0]=& _tmpA3E,((_tmpA40[1]=& _tmpA3C,((_tmpA40[2]=& _tmpA3B,Cyc_fprintf(Cyc_stderr,((_tmpA41="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",_tag_dyneither(_tmpA41,sizeof(char),131))),_tag_dyneither(_tmpA40,sizeof(void*),3)))))))))))))))))));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp37D=1;_npop_handler(0);return _tmp37D;};_LL4B:;}};}{
# 1603
const char*_tmpA42;struct _dyneither_ptr outfile_str=(_tmpA42="",_tag_dyneither(_tmpA42,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpA46;void*_tmpA45[1];struct Cyc_String_pa_PrintArg_struct _tmpA44;outfile_str=(struct _dyneither_ptr)((_tmpA44.tag=0,((_tmpA44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA45[0]=& _tmpA44,Cyc_aprintf(((_tmpA46=" -o %s",_tag_dyneither(_tmpA46,sizeof(char),7))),_tag_dyneither(_tmpA45,sizeof(void*),1))))))));}{
# 1607
const char*_tmpA4E;void*_tmpA4D[5];struct Cyc_String_pa_PrintArg_struct _tmpA4C;struct Cyc_String_pa_PrintArg_struct _tmpA4B;struct Cyc_String_pa_PrintArg_struct _tmpA4A;struct Cyc_String_pa_PrintArg_struct _tmpA49;struct Cyc_String_pa_PrintArg_struct _tmpA48;struct _dyneither_ptr _tmp381=
(_tmpA48.tag=0,((_tmpA48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp343),((_tmpA49.tag=0,((_tmpA49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpA4A.tag=0,((_tmpA4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpA4B.tag=0,((_tmpA4B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp342),((_tmpA4C.tag=0,((_tmpA4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA4D[0]=& _tmpA4C,((_tmpA4D[1]=& _tmpA4B,((_tmpA4D[2]=& _tmpA4A,((_tmpA4D[3]=& _tmpA49,((_tmpA4D[4]=& _tmpA48,Cyc_aprintf(((_tmpA4E="%s %s%s %s%s",_tag_dyneither(_tmpA4E,sizeof(char),13))),_tag_dyneither(_tmpA4D,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1610
if(Cyc_v_r){{const char*_tmpA52;void*_tmpA51[1];struct Cyc_String_pa_PrintArg_struct _tmpA50;(_tmpA50.tag=0,((_tmpA50.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp381),((_tmpA51[0]=& _tmpA50,Cyc_fprintf(Cyc_stderr,((_tmpA52="%s\n",_tag_dyneither(_tmpA52,sizeof(char),4))),_tag_dyneither(_tmpA51,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp381,sizeof(char),1))!= 0){
{const char*_tmpA55;void*_tmpA54;(_tmpA54=0,Cyc_fprintf(Cyc_stderr,((_tmpA55="Error: C compiler failed\n",_tag_dyneither(_tmpA55,sizeof(char),26))),_tag_dyneither(_tmpA54,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp387=1;_npop_handler(0);return _tmp387;};}
# 1617
Cyc_remove_cfiles();{
# 1619
int _tmp388=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp388;};};};};};
# 1154 "cyclone.cyc"
;_pop_region(r);};}
