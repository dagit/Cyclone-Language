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
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
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
struct _dyneither_ptr*_tmp3D6;struct Cyc_List_List*_tmp3D5;Cyc_ccargs=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->hd=((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=s,_tmp3D6)))),((_tmp3D5->tl=Cyc_ccargs,_tmp3D5))))));}
# 102
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp3D7;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D7="vc",_tag_dyneither(_tmp3D7,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp3D8;Cyc_add_ccarg(((_tmp3D8="-DVC_C",_tag_dyneither(_tmp3D8,sizeof(char),7))));};}else{
# 107
const char*_tmp3D9;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D9="gcc",_tag_dyneither(_tmp3D9,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp3DA;Cyc_add_ccarg(((_tmp3DA="-DGCC_C",_tag_dyneither(_tmp3DA,sizeof(char),8))));};}}}
# 115
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 123
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3DB;Cyc_output_file=((_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB[0]=s,_tmp3DB))));}
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
struct _dyneither_ptr*_tmp3DE;struct Cyc_List_List*_tmp3DD;Cyc_cppargs=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD->hd=((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE[0]=s,_tmp3DE)))),((_tmp3DD->tl=Cyc_cppargs,_tmp3DD))))));}
# 154
static void Cyc_add_cpp_and_ccarg(struct _dyneither_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 159
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3E2;void*_tmp3E1[1];struct Cyc_String_pa_PrintArg_struct _tmp3E0;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E0.tag=0,((_tmp3E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E1[0]=& _tmp3E0,Cyc_aprintf(((_tmp3E2="-iprefix %s",_tag_dyneither(_tmp3E2,sizeof(char),12))),_tag_dyneither(_tmp3E1,sizeof(void*),1)))))))));}
# 162
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3E6;void*_tmp3E5[1];struct Cyc_String_pa_PrintArg_struct _tmp3E4;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E5[0]=& _tmp3E4,Cyc_aprintf(((_tmp3E6="-iwithprefix %s",_tag_dyneither(_tmp3E6,sizeof(char),16))),_tag_dyneither(_tmp3E5,sizeof(void*),1)))))))));}
# 165
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3EA;void*_tmp3E9[1];struct Cyc_String_pa_PrintArg_struct _tmp3E8;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E8.tag=0,((_tmp3E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E9[0]=& _tmp3E8,Cyc_aprintf(((_tmp3EA="-iwithprefixbefore %s",_tag_dyneither(_tmp3EA,sizeof(char),22))),_tag_dyneither(_tmp3E9,sizeof(void*),1)))))))));}
# 168
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3EE;void*_tmp3ED[1];struct Cyc_String_pa_PrintArg_struct _tmp3EC;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3EC.tag=0,((_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3ED[0]=& _tmp3EC,Cyc_aprintf(((_tmp3EE="-isystem %s",_tag_dyneither(_tmp3EE,sizeof(char),12))),_tag_dyneither(_tmp3ED,sizeof(void*),1)))))))));}
# 171
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3F2;void*_tmp3F1[1];struct Cyc_String_pa_PrintArg_struct _tmp3F0;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3F0.tag=0,((_tmp3F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3F1[0]=& _tmp3F0,Cyc_aprintf(((_tmp3F2="-idirafter %s",_tag_dyneither(_tmp3F2,sizeof(char),14))),_tag_dyneither(_tmp3F1,sizeof(void*),1)))))))));}
# 175
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 178
Cyc_target_arch=s;
{const char*_tmp3F3;Cyc_add_ccarg(((_tmp3F3="-b",_tag_dyneither(_tmp3F3,sizeof(char),3))));}
Cyc_add_ccarg(s);}
# 183
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}
# 188
static void Cyc_print_version(){
{const char*_tmp3F7;void*_tmp3F6[1];struct Cyc_String_pa_PrintArg_struct _tmp3F5;(_tmp3F5.tag=0,((_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3F6[0]=& _tmp3F5,Cyc_printf(((_tmp3F7="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3F7,sizeof(char),34))),_tag_dyneither(_tmp3F6,sizeof(void*),1)))))));}
{const char*_tmp3FC;void*_tmp3FB[2];struct Cyc_String_pa_PrintArg_struct _tmp3FA;struct Cyc_String_pa_PrintArg_struct _tmp3F9;(_tmp3F9.tag=0,((_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3FA.tag=0,((_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3FB[0]=& _tmp3FA,((_tmp3FB[1]=& _tmp3F9,Cyc_printf(((_tmp3FC="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3FC,sizeof(char),39))),_tag_dyneither(_tmp3FB,sizeof(void*),2)))))))))))));}
{const char*_tmp400;void*_tmp3FF[1];struct Cyc_String_pa_PrintArg_struct _tmp3FE;(_tmp3FE.tag=0,((_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3FF[0]=& _tmp3FE,Cyc_printf(((_tmp400="Standard library directory: %s\n",_tag_dyneither(_tmp400,sizeof(char),32))),_tag_dyneither(_tmp3FF,sizeof(void*),1)))))));}
{const char*_tmp404;void*_tmp403[1];struct Cyc_String_pa_PrintArg_struct _tmp402;(_tmp402.tag=0,((_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp403[0]=& _tmp402,Cyc_printf(((_tmp404="Standard include directory: %s\n",_tag_dyneither(_tmp404,sizeof(char),32))),_tag_dyneither(_tmp403,sizeof(void*),1)))))));}
exit(0);}
# 197
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp27=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp27 <= 4)return 0;else{
const char*_tmp405;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp27 - 4)),((_tmp405=".cyc",_tag_dyneither(_tmp405,sizeof(char),5))))== 0;}}
# 203
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp29 <= 2)return;{
struct _dyneither_ptr _tmp2A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp29 - 2);
struct _dyneither_ptr*_tmp408;struct Cyc_List_List*_tmp407;Cyc_cyclone_exec_path=((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407->hd=((_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408[0]=_tmp2A,_tmp408)))),((_tmp407->tl=Cyc_cyclone_exec_path,_tmp407))))));};}
# 216
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp409;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp409="-lxml",_tag_dyneither(_tmp409,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 222
const char*_tmp40A;Cyc_add_ccarg(((_tmp40A="-lxml_a",_tag_dyneither(_tmp40A,sizeof(char),8))));}}else{
# 224
struct _dyneither_ptr*_tmp40D;struct Cyc_List_List*_tmp40C;Cyc_libargs=((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C->hd=((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D[0]=s,_tmp40D)))),((_tmp40C->tl=Cyc_libargs,_tmp40C))))));}}
# 227
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 231
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp40E;Cyc_add_ccarg(((_tmp40E="-save-temps",_tag_dyneither(_tmp40E,sizeof(char),12))));};}
# 236
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp40F;Cyc_add_cpparg(((_tmp40F="-M",_tag_dyneither(_tmp40F,sizeof(char),3))));};}
# 243
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp410;Cyc_dependencies_target=((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=s,_tmp410))));}
# 248
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp411;Cyc_add_ccarg(((_tmp411="-c",_tag_dyneither(_tmp411,sizeof(char),3))));};}
# 253
static void Cyc_set_nocppprecomp(){
const char*_tmp412;Cyc_add_cpp_and_ccarg(((_tmp412="-no-cpp-precomp",_tag_dyneither(_tmp412,sizeof(char),16))));}
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
{const char*_tmp413;Cyc_add_ccarg(((_tmp413="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp413,sizeof(char),18))));}
Cyc_set_elim_se();}
# 273
static void Cyc_set_noboundschecks(){
const char*_tmp414;Cyc_add_ccarg(((_tmp414="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp414,sizeof(char),23))));}
# 276
static void Cyc_set_nonullchecks(){
const char*_tmp415;Cyc_add_ccarg(((_tmp415="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp415,sizeof(char),21))));}
# 279
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 285
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp416;Cyc_add_ccarg(((_tmp416="-DNO_CYC_PREFIX",_tag_dyneither(_tmp416,sizeof(char),16))));};}
# 290
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp417;Cyc_add_ccarg(((_tmp417="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp417,sizeof(char),21))));}{
const char*_tmp418;Cyc_add_cpparg(((_tmp418="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp418,sizeof(char),21))));};}
# 296
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp419;Cyc_add_ccarg(((_tmp419="-pg",_tag_dyneither(_tmp419,sizeof(char),4))));};}
# 301
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp41A;Cyc_add_ccarg(((_tmp41A="-S",_tag_dyneither(_tmp41A,sizeof(char),3))));};}
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
const char*_tmp41B;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp41B="cyc",_tag_dyneither(_tmp41B,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp41C;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp41C="none",_tag_dyneither(_tmp41C,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 328
const char*_tmp420;void*_tmp41F[1];struct Cyc_String_pa_PrintArg_struct _tmp41E;(_tmp41E.tag=0,((_tmp41E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp41F[0]=& _tmp41E,Cyc_fprintf(Cyc_stderr,((_tmp420="Input type '%s' not supported\n",_tag_dyneither(_tmp420,sizeof(char),31))),_tag_dyneither(_tmp41F,sizeof(void*),1)))))));}}}
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
static void Cyc_add_other(struct _dyneither_ptr s){
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 349
{struct _dyneither_ptr*_tmp423;struct Cyc_List_List*_tmp422;Cyc_cyclone_files=((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422->hd=((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423[0]=s,_tmp423)))),((_tmp422->tl=Cyc_cyclone_files,_tmp422))))));}{
# 352
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp424;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp424=".c",_tag_dyneither(_tmp424,sizeof(char),3))));
Cyc_add_ccarg((struct _dyneither_ptr)_tmp49);};}else{
# 356
Cyc_add_ccarg(s);}}
# 359
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple21{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 370
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp4B=0;
int c;
int i;
char strname[256];
char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),"r");
# 378
if(spec_file == 0){
{const char*_tmp428;void*_tmp427[1];struct Cyc_String_pa_PrintArg_struct _tmp426;(_tmp426.tag=0,((_tmp426.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp427[0]=& _tmp426,Cyc_fprintf(Cyc_stderr,((_tmp428="Error opening spec file %s\n",_tag_dyneither(_tmp428,sizeof(char),28))),_tag_dyneither(_tmp427,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 384
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp42D;void*_tmp42C[2];struct Cyc_String_pa_PrintArg_struct _tmp42B;struct Cyc_Int_pa_PrintArg_struct _tmp42A;(_tmp42A.tag=1,((_tmp42A.f1=(unsigned long)c,((_tmp42B.tag=0,((_tmp42B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42C[0]=& _tmp42B,((_tmp42C[1]=& _tmp42A,Cyc_fprintf(Cyc_stderr,((_tmp42D="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp42D,sizeof(char),55))),_tag_dyneither(_tmp42C,sizeof(void*),2)))))))))))));}
# 393
Cyc_fflush(Cyc_stderr);}
# 395
goto CLEANUP_AND_RETURN;}
# 397
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == - 1){
{const char*_tmp431;void*_tmp430[1];struct Cyc_String_pa_PrintArg_struct _tmp42F;(_tmp42F.tag=0,((_tmp42F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp430[0]=& _tmp42F,Cyc_fprintf(Cyc_stderr,((_tmp431="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp431,sizeof(char),44))),_tag_dyneither(_tmp430,sizeof(void*),1)))))));}
# 405
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 408
if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';
break;}
# 412
strname[_check_known_subscript_notnull(256,i)]=(char)c;
++ i;
if(i >= 256){
{const char*_tmp435;void*_tmp434[1];struct Cyc_String_pa_PrintArg_struct _tmp433;(_tmp433.tag=0,((_tmp433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp434[0]=& _tmp433,Cyc_fprintf(Cyc_stderr,((_tmp435="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp435,sizeof(char),50))),_tag_dyneither(_tmp434,sizeof(void*),1)))))));}
# 418
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 422
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
break;}
# 427
if(c == '*'){
# 429
{struct _tuple21*_tmp442;struct _dyneither_ptr*_tmp441;const char*_tmp440;struct _dyneither_ptr*_tmp43F;struct Cyc_List_List*_tmp43E;_tmp4B=((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E->hd=((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442->f1=((_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp441)))),((_tmp442->f2=(
(_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp440="",_tag_dyneither(_tmp440,sizeof(char),1)))),_tmp43F)))),_tmp442)))))),((_tmp43E->tl=_tmp4B,_tmp43E))))));}
goto JUST_AFTER_STAR;}
# 434
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;
if(i >= 4096){
{const char*_tmp447;void*_tmp446[2];struct Cyc_String_pa_PrintArg_struct _tmp445;struct Cyc_String_pa_PrintArg_struct _tmp444;(_tmp444.tag=0,((_tmp444.f1=(struct _dyneither_ptr)
# 444
_tag_dyneither(strname,sizeof(char),256),((_tmp445.tag=0,((_tmp445.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp446[0]=& _tmp445,((_tmp446[1]=& _tmp444,Cyc_fprintf(Cyc_stderr,((_tmp447="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp447,sizeof(char),50))),_tag_dyneither(_tmp446,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 450
{struct _tuple21*_tmp450;struct _dyneither_ptr*_tmp44F;struct _dyneither_ptr*_tmp44E;struct Cyc_List_List*_tmp44D;_tmp4B=((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D->hd=((_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450->f1=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp44F)))),((_tmp450->f2=(
(_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp44E)))),_tmp450)))))),((_tmp44D->tl=_tmp4B,_tmp44D))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 455
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp4B;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp45A(struct _dyneither_ptr*_tmp6C,unsigned int*_tmp459,unsigned int*_tmp458,struct _dyneither_ptr**_tmp456){for(*_tmp459=0;*_tmp459 < *_tmp458;(*_tmp459)++){(*_tmp456)[*_tmp459]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)(*_tmp6C).curr)[(int)*_tmp459];}}
# 461
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
unsigned long _tmp66=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp67=0;
char buf[4096];
int i=0;
int j=0;
if(_tmp66 > 4096)goto DONE;
while(1){
# 471
while(1){
if(i >= _tmp66)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 477
j=0;
# 482
while(1){
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 488
++ i;
if(i >= _tmp66)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}else{
# 495
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}
# 498
++ i;}
# 500
if(j < 4096)
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 503
struct _dyneither_ptr*_tmp453;struct Cyc_List_List*_tmp452;_tmp67=((_tmp452=_cycalloc(sizeof(*_tmp452)),((_tmp452->hd=((_tmp453=_cycalloc(sizeof(*_tmp453)),((_tmp453[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp453)))),((_tmp452->tl=_tmp67,_tmp452))))));};}
# 505
DONE:
 _tmp67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67);
{struct Cyc_List_List*_tmp454;_tmp67=((_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp454->tl=_tmp67,_tmp454))))));}{
struct _dyneither_ptr _tmp6C=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp67);
unsigned int _tmp459;unsigned int _tmp458;struct _dyneither_ptr _tmp457;struct _dyneither_ptr*_tmp456;unsigned int _tmp455;struct _dyneither_ptr _tmp6D=(_tmp455=_get_dyneither_size(_tmp6C,sizeof(struct _dyneither_ptr*)),((_tmp456=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp455)),((_tmp457=_tag_dyneither(_tmp456,sizeof(struct _dyneither_ptr),_tmp455),((((_tmp458=_tmp455,_tmp45A(& _tmp6C,& _tmp459,& _tmp458,& _tmp456))),_tmp457)))))));
return _tmp6D;};};}
# 515
int Cyc_compile_failure=0;
# 517
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){{struct Cyc___cycFILE*_tmp75=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp75;};_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp77=_tmp73;void*_tmp78;_LL1:;_LL2:
# 521
 Cyc_compile_failure=1;
{const char*_tmp45F;void*_tmp45E[2];struct Cyc_String_pa_PrintArg_struct _tmp45D;struct Cyc_String_pa_PrintArg_struct _tmp45C;(_tmp45C.tag=0,((_tmp45C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp45D.tag=0,((_tmp45D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp45E[0]=& _tmp45D,((_tmp45E[1]=& _tmp45C,Cyc_fprintf(Cyc_stderr,((_tmp45F="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp45F,sizeof(char),29))),_tag_dyneither(_tmp45E,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL3: _tmp78=_tmp77;_LL4:(void)_rethrow(_tmp78);_LL0:;}};}
# 528
void CYCALLOCPROFILE_mark(const char*s);
# 530
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 534
const char*_tmp460;struct _dyneither_ptr explain_string=(_tmp460="",_tag_dyneither(_tmp460,sizeof(char),1));
struct Cyc_List_List*_tmp7D=0;
# 537
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 539
{struct _handler_cons _tmp7E;_push_handler(& _tmp7E);{int _tmp80=0;if(setjmp(_tmp7E.handler))_tmp80=1;if(!_tmp80){_tmp7D=f(env,tds);;_pop_handler();}else{void*_tmp7F=(void*)_exn_thrown;void*_tmp82=_tmp7F;void*_tmp83;void*_tmp84;_LL6: _tmp83=_tmp82;_LL7:
# 542
{const char*_tmp464;void*_tmp463[1];struct Cyc_String_pa_PrintArg_struct _tmp462;(_tmp462.tag=0,((_tmp462.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp463[0]=& _tmp462,Cyc_fprintf(Cyc_stderr,((_tmp464="COMPILER STAGE %s\n",_tag_dyneither(_tmp464,sizeof(char),19))),_tag_dyneither(_tmp463,sizeof(void*),1)))))));}
Cyc_Core_rethrow(_tmp83);_LL8: _tmp84=_tmp82;_LL9:(void)_rethrow(_tmp84);_LL5:;}};}
# 545
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
{const char*_tmp467;void*_tmp466;(_tmp466=0,Cyc_fprintf(Cyc_stderr,((_tmp467="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp467,sizeof(char),22))),_tag_dyneither(_tmp466,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
return _tmp7D;}else{
# 552
if(Cyc_v_r){
{const char*_tmp46B;void*_tmp46A[1];struct Cyc_String_pa_PrintArg_struct _tmp469;(_tmp469.tag=0,((_tmp469.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp46A[0]=& _tmp469,Cyc_fprintf(Cyc_stderr,((_tmp46B="%s completed.\n",_tag_dyneither(_tmp46B,sizeof(char),15))),_tag_dyneither(_tmp46A,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return _tmp7D;}}
# 557
return _tmp7D;}
# 560
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp8E;_push_handler(& _tmp8E);{int _tmp90=0;if(setjmp(_tmp8E.handler))_tmp90=1;if(!_tmp90){
ans=Cyc_Parse_parse_file(f);;_pop_handler();}else{void*_tmp8F=(void*)_exn_thrown;void*_tmp92=_tmp8F;void*_tmp94;void*_tmp95;_LLB: {struct Cyc_Position_Exit_exn_struct*_tmp93=(struct Cyc_Position_Exit_exn_struct*)_tmp92;if(_tmp93->tag != Cyc_Position_Exit)goto _LLD;}_LLC:
# 567
 Cyc_compile_failure=1;goto _LLA;_LLD: _tmp94=_tmp92;_LLE:
 Cyc_Core_rethrow(_tmp94);_LLF: _tmp95=_tmp92;_LL10:(void)_rethrow(_tmp95);_LLA:;}};}
# 570
Cyc_Lex_lex_init(1);
return ans;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 574
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 576
struct _tuple22 _tmp96=*tcenv;struct _RegionHandle*_tmp98;struct Cyc_Tcenv_Tenv*_tmp99;struct _tuple22 _tmp97=_tmp96;_tmp98=_tmp97.f1;_tmp99=_tmp97.f2;
Cyc_Tc_tc(_tmp98,_tmp99,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmp99,tds);
return tds;}
# 583
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 590
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct Cyc_Tcenv_Tenv*_tmp9B;struct Cyc___cycFILE*_tmp9C;struct Cyc___cycFILE*_tmp9D;struct _tuple23*_tmp9A=params;_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;_tmp9D=_tmp9A->f3;{
struct Cyc_Interface_I*_tmp9E=Cyc_Interface_extract(_tmp9B->ae);
if(_tmp9C == 0)
Cyc_Interface_save(_tmp9E,_tmp9D);else{
# 596
struct Cyc_Interface_I*_tmp9F=Cyc_Interface_parse(_tmp9C);
const char*_tmp470;const char*_tmp46F;struct _tuple20*_tmp46E;if(!Cyc_Interface_is_subinterface(_tmp9F,_tmp9E,((_tmp46E=_cycalloc(sizeof(*_tmp46E)),((_tmp46E->f1=((_tmp46F="written interface",_tag_dyneither(_tmp46F,sizeof(char),18))),((_tmp46E->f2=((_tmp470="maximal interface",_tag_dyneither(_tmp470,sizeof(char),18))),_tmp46E))))))))
Cyc_compile_failure=1;else{
# 600
Cyc_Interface_save(_tmp9F,_tmp9D);}}
# 602
return tds;};}
# 605
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
return Cyc_Toc_toc(tds);}
# 608
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 613
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 615
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 617
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 622
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 625
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{const char*_tmp473;void*_tmp472;(_tmp472=0,Cyc_fprintf(Cyc_stderr,((_tmp473="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp473,sizeof(char),53))),_tag_dyneither(_tmp472,sizeof(void*),0)));}
return 1;}{
# 629
const char*_tmp475;const char*_tmp474;struct Cyc___cycFILE*_tmpA5=Cyc_try_file_open(file,((_tmp474="r",_tag_dyneither(_tmp474,sizeof(char),2))),((_tmp475="internal compiler file",_tag_dyneither(_tmp475,sizeof(char),23))));
if(_tmpA5 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpAC=1024;unsigned int i;for(i=0;i < _tmpAC;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpA5);
if(n_read != 1024  && !Cyc_feof(_tmpA5)){
Cyc_compile_failure=1;
{const char*_tmp479;void*_tmp478[1];struct Cyc_String_pa_PrintArg_struct _tmp477;(_tmp477.tag=0,((_tmp477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp478[0]=& _tmp477,Cyc_fprintf(Cyc_stderr,((_tmp479="\nError: problem copying %s\n",_tag_dyneither(_tmp479,sizeof(char),28))),_tag_dyneither(_tmp478,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 642
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp47D;void*_tmp47C[1];struct Cyc_String_pa_PrintArg_struct _tmp47B;(_tmp47B.tag=0,((_tmp47B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp47C[0]=& _tmp47B,Cyc_fprintf(Cyc_stderr,((_tmp47D="\nError: problem copying %s\n",_tag_dyneither(_tmp47D,sizeof(char),28))),_tag_dyneither(_tmp47C,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 650
Cyc_fclose(_tmpA5);
return 0;};};}
# 655
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpAF=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpAF.expand_typedefs=!Cyc_noexpand_r;
_tmpAF.to_VC=Cyc_Cyclone_tovc_r;
_tmpAF.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpAF.generate_line_directives=Cyc_generate_line_directives_r;
_tmpAF.print_full_evars=Cyc_print_full_evars_r;
_tmpAF.print_all_tvars=Cyc_print_all_tvars_r;
_tmpAF.print_all_kinds=Cyc_print_all_kinds_r;
_tmpAF.print_all_effects=Cyc_print_all_effects_r;
# 666
if(Cyc_inline_functions_r){
const char*_tmp480;void*_tmp47F;(_tmp47F=0,Cyc_fprintf(out_file,((_tmp480="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp480,sizeof(char),27))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}
# 670
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r){
const char*_tmp483;void*_tmp482;(_tmp482=0,Cyc_fprintf(out_file,((_tmp483="#include <setjmp.h>\n",_tag_dyneither(_tmp483,sizeof(char),21))),_tag_dyneither(_tmp482,sizeof(void*),0)));}else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 676
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 678
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpAF);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 682
Cyc_Absyndump_set_params(& _tmpAF);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 685
Cyc_fflush(out_file);
return tds;}
# 691
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 699
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpB4=0;
unsigned long _tmpB5=Cyc_strlen((struct _dyneither_ptr)s);
# 704
while(_tmpB5 > 0){
struct _dyneither_ptr _tmpB6=Cyc_strchr(s,c);
if((char*)_tmpB6.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp486;struct Cyc_List_List*_tmp485;_tmpB4=((_tmp485=_cycalloc(sizeof(*_tmp485)),((_tmp485->hd=((_tmp486=_cycalloc(sizeof(*_tmp486)),((_tmp486[0]=s,_tmp486)))),((_tmp485->tl=_tmpB4,_tmp485))))));}
break;}else{
# 711
{struct _dyneither_ptr*_tmp489;struct Cyc_List_List*_tmp488;_tmpB4=((_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488->hd=((_tmp489=_cycalloc(sizeof(*_tmp489)),((_tmp489[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpB6).curr - s.curr)/ sizeof(char))),_tmp489)))),((_tmp488->tl=_tmpB4,_tmp488))))));}
_tmpB5 -=(((struct _dyneither_ptr)_tmpB6).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpB6,sizeof(char),1);}}
# 716
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB4);};}
# 720
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpBB;_push_handler(& _tmpBB);{int _tmpBD=0;if(setjmp(_tmpBB.handler))_tmpBD=1;if(!_tmpBD){{const char*_tmp48A;f=Cyc_file_open(file,((_tmp48A="r",_tag_dyneither(_tmp48A,sizeof(char),2))));};_pop_handler();}else{void*_tmpBC=(void*)_exn_thrown;void*_tmpC0=_tmpBC;void*_tmpC1;_LL12:;_LL13: goto _LL11;_LL14: _tmpC1=_tmpC0;_LL15:(void)_rethrow(_tmpC1);_LL11:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 728
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp48B;struct _dyneither_ptr tmp=(_tmp48B="",_tag_dyneither(_tmp48B,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC2=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC2.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC2)== 0)continue;
{const char*_tmp48C;_tmpC2=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC2,((_tmp48C=":",_tag_dyneither(_tmp48C,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC2,(struct _dyneither_ptr)tmp);}
# 736
return tmp;}
# 741
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC5=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC5.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC5)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpC5,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp48D;return(_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D[0]=s,_tmp48D)));}};}
# 749
return 0;}
# 752
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpC7=Cyc_find(dirs,file);
if(_tmpC7 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp492;void*_tmp491[2];struct Cyc_String_pa_PrintArg_struct _tmp490;struct Cyc_String_pa_PrintArg_struct _tmp48F;(_tmp48F.tag=0,((_tmp48F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp490.tag=0,((_tmp490.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp491[0]=& _tmp490,((_tmp491[1]=& _tmp48F,Cyc_fprintf(Cyc_stderr,((_tmp492="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp492,sizeof(char),56))),_tag_dyneither(_tmp491,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp4A2;const char*_tmp4A1;void*_tmp4A0[2];struct Cyc_String_pa_PrintArg_struct _tmp49F;struct Cyc_String_pa_PrintArg_struct _tmp49E;struct Cyc_Core_Failure_exn_struct*_tmp49D;(int)_throw((void*)((_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp4A2.tag=Cyc_Core_Failure,((_tmp4A2.f1=(struct _dyneither_ptr)((_tmp49E.tag=0,((_tmp49E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp49F.tag=0,((_tmp49F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp4A0[0]=& _tmp49F,((_tmp4A0[1]=& _tmp49E,Cyc_aprintf(((_tmp4A1="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp4A1,sizeof(char),56))),_tag_dyneither(_tmp4A0,sizeof(void*),2)))))))))))))),_tmp4A2)))),_tmp49D)))));};}
# 764
return*_tmpC7;}
# 769
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 771
struct Cyc_List_List*_tmpD2=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp4A5;struct Cyc_List_List*_tmp4A4;_tmpD2=((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4->hd=((_tmp4A5=_cycalloc(sizeof(*_tmp4A5)),((_tmp4A5[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4A5)))),((_tmp4A4->tl=_tmpD2,_tmp4A4))))));}
# 775
_tmpD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD2);
return _tmpD2;}
# 781
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 783
struct Cyc_List_List*_tmpD5=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp4A6;_tmpD5=((_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp4A6->tl=_tmpD5,_tmp4A6))))));}{
struct _dyneither_ptr*_tmp4A9;struct Cyc_List_List*_tmp4A8;_tmpD5=((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8->hd=((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4A9)))),((_tmp4A8->tl=_tmpD5,_tmp4A8))))));};}
# 788
_tmpD5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD5);
return _tmpD5;}
# 793
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
# 808
 goto _LL21;case '\t': _LL21:
 return 1;default: _LL22:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4B2(unsigned int*_tmp4B1,unsigned int*_tmp4B0,char**_tmp4AE){for(*_tmp4B1=0;*_tmp4B1 < *_tmp4B0;(*_tmp4B1)++){(*_tmp4AE)[*_tmp4B1]=(char)'\000';}}
# 815
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpD9=Cyc_strlen((struct _dyneither_ptr)s);
# 819
int _tmpDA=0;
int _tmpDB=0;
{int i=0;for(0;i < _tmpD9;++ i){
char _tmpDC=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpDC == '\'')++ _tmpDA;else{
if(Cyc_is_other_special(_tmpDC))++ _tmpDB;}}}
# 828
if(_tmpDA == 0  && _tmpDB == 0)
return s;
# 832
if(_tmpDA == 0){
struct _dyneither_ptr*_tmp4AC;struct _dyneither_ptr*_tmp4AB[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp4AB[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp4AB[1]=((_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC[0]=(struct _dyneither_ptr)s,_tmp4AC)))),((_tmp4AB[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4AB,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 836
unsigned long _tmpE1=(_tmpD9 + _tmpDA)+ _tmpDB;
unsigned int _tmp4B1;unsigned int _tmp4B0;struct _dyneither_ptr _tmp4AF;char*_tmp4AE;unsigned int _tmp4AD;struct _dyneither_ptr s2=(_tmp4AD=_tmpE1 + 1,((_tmp4AE=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4AD + 1)),((_tmp4AF=_tag_dyneither(_tmp4AE,sizeof(char),_tmp4AD + 1),((((_tmp4B0=_tmp4AD,((_tmp4B2(& _tmp4B1,& _tmp4B0,& _tmp4AE),_tmp4AE[_tmp4B0]=(char)0)))),_tmp4AF)))))));
int _tmpE2=0;
int _tmpE3=0;
for(0;_tmpE2 < _tmpD9;++ _tmpE2){
char _tmpE4=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpE2));
if(_tmpE4 == '\''  || Cyc_is_other_special(_tmpE4)){
char _tmp4B5;char _tmp4B4;struct _dyneither_ptr _tmp4B3;(_tmp4B3=_dyneither_ptr_plus(s2,sizeof(char),_tmpE3 ++),((_tmp4B4=*((char*)_check_dyneither_subscript(_tmp4B3,sizeof(char),0)),((_tmp4B5='\\',((_get_dyneither_size(_tmp4B3,sizeof(char))== 1  && (_tmp4B4 == '\000'  && _tmp4B5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4B3.curr)=_tmp4B5)))))));}{
char _tmp4B8;char _tmp4B7;struct _dyneither_ptr _tmp4B6;(_tmp4B6=_dyneither_ptr_plus(s2,sizeof(char),_tmpE3 ++),((_tmp4B7=*((char*)_check_dyneither_subscript(_tmp4B6,sizeof(char),0)),((_tmp4B8=_tmpE4,((_get_dyneither_size(_tmp4B6,sizeof(char))== 1  && (_tmp4B7 == '\000'  && _tmp4B8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4B6.curr)=_tmp4B8)))))));};}
# 846
return(struct _dyneither_ptr)s2;};}
# 848
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp4B9;return(_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9[0]=Cyc_sh_escape_string(*sp),_tmp4B9)));}
# 852
static void Cyc_process_file(struct _dyneither_ptr filename){
# 854
struct _dyneither_ptr _tmpF0=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp4BA;struct _dyneither_ptr _tmpF1=Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4BA=".cyp",_tag_dyneither(_tmp4BA,sizeof(char),5))));
const char*_tmp4BB;struct _dyneither_ptr _tmpF2=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4BB=".cyci",_tag_dyneither(_tmp4BB,sizeof(char),6))));
const char*_tmp4BC;struct _dyneither_ptr _tmpF3=Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4BC=".cycio",_tag_dyneither(_tmp4BC,sizeof(char),7))));
const char*_tmp4BD;struct _dyneither_ptr _tmpF4=Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4BD=".c",_tag_dyneither(_tmp4BD,sizeof(char),3))));
# 861
if(Cyc_v_r){const char*_tmp4C1;void*_tmp4C0[1];struct Cyc_String_pa_PrintArg_struct _tmp4BF;(_tmp4BF.tag=0,((_tmp4BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4C0[0]=& _tmp4BF,Cyc_fprintf(Cyc_stderr,((_tmp4C1="Compiling %s\n",_tag_dyneither(_tmp4C1,sizeof(char),14))),_tag_dyneither(_tmp4C0,sizeof(void*),1)))))));}{
# 864
const char*_tmp4C3;const char*_tmp4C2;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4C2="r",_tag_dyneither(_tmp4C2,sizeof(char),2))),((_tmp4C3="input file",_tag_dyneither(_tmp4C3,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 871
const char*_tmp4CA;struct _dyneither_ptr*_tmp4C9;const char*_tmp4C8;struct Cyc_List_List*_tmp4C7;struct _dyneither_ptr _tmpF8=
Cyc_str_sepstr(((_tmp4C7=_cycalloc(sizeof(*_tmp4C7)),((_tmp4C7->hd=((_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((_tmp4C9[0]=(struct _dyneither_ptr)((_tmp4C8="",_tag_dyneither(_tmp4C8,sizeof(char),1))),_tmp4C9)))),((_tmp4C7->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4C7)))))),(
(_tmp4CA=" ",_tag_dyneither(_tmp4CA,sizeof(char),2))));
# 881
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpF9=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp4CB;_tmpF9=Cyc_add_subdir(_tmpF9,((_tmp4CB="include",_tag_dyneither(_tmp4CB,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp4CE;struct Cyc_List_List*_tmp4CD;_tmpF9=((_tmp4CD=_cycalloc(sizeof(*_tmp4CD)),((_tmp4CD->hd=((_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((_tmp4CE[0]=def_inc_path,_tmp4CE)))),((_tmp4CD->tl=_tmpF9,_tmp4CD))))));}{
# 887
char*_tmpFD=getenv("CYCLONE_INCLUDE_PATH");
if(_tmpFD != 0){
char*_tmp4CF;_tmpF9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4CF=_tmpFD,_tag_dyneither(_tmp4CF,sizeof(char),_get_zero_arr_size_char((void*)_tmp4CF,1)))),':'),_tmpF9);}{
# 892
const char*_tmp4D6;struct _dyneither_ptr*_tmp4D5;const char*_tmp4D4;struct Cyc_List_List*_tmp4D3;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3->hd=((_tmp4D5=_cycalloc(sizeof(*_tmp4D5)),((_tmp4D5[0]=(struct _dyneither_ptr)((_tmp4D4="",_tag_dyneither(_tmp4D4,sizeof(char),1))),_tmp4D5)))),((_tmp4D3->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpF9),_tmp4D3)))))),(
(_tmp4D6=" -I",_tag_dyneither(_tmp4D6,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp4DA;void*_tmp4D9[1];struct Cyc_String_pa_PrintArg_struct _tmp4D8;ofile_string=(struct _dyneither_ptr)((_tmp4D8.tag=0,((_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4D9[0]=& _tmp4D8,Cyc_aprintf(((_tmp4DA=" > %s",_tag_dyneither(_tmp4DA,sizeof(char),6))),_tag_dyneither(_tmp4D9,sizeof(void*),1))))))));}else{
# 901
const char*_tmp4DB;ofile_string=((_tmp4DB="",_tag_dyneither(_tmp4DB,sizeof(char),1)));}}else{
# 903
const char*_tmp4DF;void*_tmp4DE[1];struct Cyc_String_pa_PrintArg_struct _tmp4DD;ofile_string=(struct _dyneither_ptr)((_tmp4DD.tag=0,((_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpF1)),((_tmp4DE[0]=& _tmp4DD,Cyc_aprintf(((_tmp4DF=" > %s",_tag_dyneither(_tmp4DF,sizeof(char),6))),_tag_dyneither(_tmp4DE,sizeof(void*),1))))))));}{
# 905
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 909
if(Cyc_dependencies_target == 0){
# 913
const char*_tmp4E0;fixup_string=((_tmp4E0=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4E0,sizeof(char),35)));}else{
# 918
const char*_tmp4E4;void*_tmp4E3[1];struct Cyc_String_pa_PrintArg_struct _tmp4E2;fixup_string=(struct _dyneither_ptr)(
(_tmp4E2.tag=0,((_tmp4E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4E3[0]=& _tmp4E2,Cyc_aprintf(((_tmp4E4=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4E4,sizeof(char),29))),_tag_dyneither(_tmp4E3,sizeof(void*),1))))))));}}else{
# 922
const char*_tmp4E5;fixup_string=((_tmp4E5="",_tag_dyneither(_tmp4E5,sizeof(char),1)));}{
# 924
const char*_tmp4EE;void*_tmp4ED[6];struct Cyc_String_pa_PrintArg_struct _tmp4EC;struct Cyc_String_pa_PrintArg_struct _tmp4EB;struct Cyc_String_pa_PrintArg_struct _tmp4EA;struct Cyc_String_pa_PrintArg_struct _tmp4E9;struct Cyc_String_pa_PrintArg_struct _tmp4E8;struct Cyc_String_pa_PrintArg_struct _tmp4E7;struct _dyneither_ptr _tmp10B=(_tmp4E7.tag=0,((_tmp4E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4E9.tag=0,((_tmp4E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 927
Cyc_sh_escape_string(filename)),((_tmp4EA.tag=0,((_tmp4EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4EB.tag=0,((_tmp4EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF8),((_tmp4EC.tag=0,((_tmp4EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4ED[0]=& _tmp4EC,((_tmp4ED[1]=& _tmp4EB,((_tmp4ED[2]=& _tmp4EA,((_tmp4ED[3]=& _tmp4E9,((_tmp4ED[4]=& _tmp4E8,((_tmp4ED[5]=& _tmp4E7,Cyc_aprintf(((_tmp4EE="%s %s%s %s%s%s",_tag_dyneither(_tmp4EE,sizeof(char),15))),_tag_dyneither(_tmp4ED,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 930
if(Cyc_v_r){const char*_tmp4F2;void*_tmp4F1[1];struct Cyc_String_pa_PrintArg_struct _tmp4F0;(_tmp4F0.tag=0,((_tmp4F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10B),((_tmp4F1[0]=& _tmp4F0,Cyc_fprintf(Cyc_stderr,((_tmp4F2="%s\n",_tag_dyneither(_tmp4F2,sizeof(char),4))),_tag_dyneither(_tmp4F1,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp10B,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp4F5;void*_tmp4F4;(_tmp4F4=0,Cyc_fprintf(Cyc_stderr,((_tmp4F5="\nError: preprocessing\n",_tag_dyneither(_tmp4F5,sizeof(char),23))),_tag_dyneither(_tmp4F4,sizeof(void*),0)));}
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
return;}
# 937
if(Cyc_stop_after_cpp_r)return;
# 940
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpF1);{
const char*_tmp4F7;const char*_tmp4F6;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF1,((_tmp4F6="r",_tag_dyneither(_tmp4F6,sizeof(char),2))),((_tmp4F7="file",_tag_dyneither(_tmp4F7,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 944
struct Cyc_List_List*tds=0;
# 947
{struct _handler_cons _tmp111;_push_handler(& _tmp111);{int _tmp113=0;if(setjmp(_tmp111.handler))_tmp113=1;if(!_tmp113){
{const char*_tmp4F8;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F8="parsing",_tag_dyneither(_tmp4F8,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp112=(void*)_exn_thrown;void*_tmp116=_tmp112;void*_tmp117;void*_tmp118;_LL25: _tmp117=_tmp116;_LL26:
# 951
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
Cyc_Core_rethrow(_tmp117);_LL27: _tmp118=_tmp116;_LL28:(void)_rethrow(_tmp118);_LL24:;}};}
# 955
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
return;}
# 961
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 966
struct _RegionHandle _tmp119=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp119;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp11A=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 971
struct _tuple22 _tmp4F9;struct _tuple22 _tmp11B=(_tmp4F9.f1=tc_rgn,((_tmp4F9.f2=_tmp11A,_tmp4F9)));
{const char*_tmp4FA;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FA="type checking",_tag_dyneither(_tmp4FA,sizeof(char),14))),Cyc_do_typecheck,& _tmp11B,tds);}
# 974
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
_npop_handler(0);return;}
# 978
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp4FB;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FB="control-flow checking",_tag_dyneither(_tmp4FB,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
_npop_handler(0);return;}
# 986
if(Cyc_generate_interface_r){
const char*_tmp4FE;const char*_tmp4FD;const char*_tmp4FC;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp4FC=(const char*)_untag_dyneither_ptr(_tmpF2,sizeof(char),1),_tag_dyneither(_tmp4FC,sizeof(char),_get_zero_arr_size_char((void*)_tmp4FC,1)))),((_tmp4FD="w",_tag_dyneither(_tmp4FD,sizeof(char),2))),((_tmp4FE="interface file",_tag_dyneither(_tmp4FE,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 991
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 998
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpF2,sizeof(char),1),"r");
const char*_tmp500;const char*_tmp4FF;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpF3,((_tmp4FF="w",_tag_dyneither(_tmp4FF,sizeof(char),2))),((_tmp500="interface object file",_tag_dyneither(_tmp500,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1005
Cyc_Position_reset_position(_tmpF2);{
struct _tuple23 _tmp501;struct _tuple23 _tmp121=(_tmp501.f1=_tmp11A,((_tmp501.f2=inter_file,((_tmp501.f3=inter_objfile,_tmp501)))));
{const char*_tmp502;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp502="interface checking",_tag_dyneither(_tmp502,sizeof(char),19))),Cyc_do_interface,& _tmp121,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1013
if(Cyc_cf_r)goto PRINTC;
# 1016
{const char*_tmp503;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp503="translation to C",_tag_dyneither(_tmp503,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
_npop_handler(0);return;}
# 1022
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1030
const char*_tmp504;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp504="post-pass to VC",_tag_dyneither(_tmp504,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1037
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp506;const char*_tmp505;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp505="w",_tag_dyneither(_tmp505,sizeof(char),2))),((_tmp506="output file",_tag_dyneither(_tmp506,sizeof(char),12))));}else{
# 1043
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp508;const char*_tmp507;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp507="w",_tag_dyneither(_tmp507,sizeof(char),2))),((_tmp508="output file",_tag_dyneither(_tmp508,sizeof(char),12))));}else{
# 1047
const char*_tmp50A;const char*_tmp509;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF4,((_tmp509="w",_tag_dyneither(_tmp509,sizeof(char),2))),((_tmp50A="output file",_tag_dyneither(_tmp50A,sizeof(char),12))));}}
# 1049
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1051
{struct _handler_cons _tmp12F;_push_handler(& _tmp12F);{int _tmp131=0;if(setjmp(_tmp12F.handler))_tmp131=1;if(!_tmp131){
if(!Cyc_noprint_r){
const char*_tmp50B;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp50B="printing",_tag_dyneither(_tmp50B,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1052
;_pop_handler();}else{void*_tmp130=(void*)_exn_thrown;void*_tmp134=_tmp130;void*_tmp135;void*_tmp136;_LL2A: _tmp135=_tmp134;_LL2B:
# 1056
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp50E;struct Cyc_List_List*_tmp50D;Cyc_cfiles=((_tmp50D=_cycalloc(sizeof(*_tmp50D)),((_tmp50D->hd=((_tmp50E=_cycalloc(sizeof(*_tmp50E)),((_tmp50E[0]=(struct _dyneither_ptr)_tmpF4,_tmp50E)))),((_tmp50D->tl=Cyc_cfiles,_tmp50D))))));}
Cyc_Core_rethrow(_tmp135);_LL2C: _tmp136=_tmp134;_LL2D:(void)_rethrow(_tmp136);_LL29:;}};}
# 1061
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp511;struct Cyc_List_List*_tmp510;Cyc_cfiles=((_tmp510=_cycalloc(sizeof(*_tmp510)),((_tmp510->hd=((_tmp511=_cycalloc(sizeof(*_tmp511)),((_tmp511[0]=(struct _dyneither_ptr)_tmpF4,_tmp511)))),((_tmp510->tl=Cyc_cfiles,_tmp510))))));};}
# 966
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp153[8]="<final>";
# 1066
static struct _dyneither_ptr Cyc_final_str={_tmp153,_tmp153,_tmp153 + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1069
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1073
struct _dyneither_ptr basename;
{struct _handler_cons _tmp154;_push_handler(& _tmp154);{int _tmp156=0;if(setjmp(_tmp154.handler))_tmp156=1;if(!_tmp156){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp155=(void*)_exn_thrown;void*_tmp158=_tmp155;void*_tmp15A;_LL2F: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp159=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp158;if(_tmp159->tag != Cyc_Core_Invalid_argument)goto _LL31;}_LL30:
# 1077
 basename=*n;goto _LL2E;_LL31: _tmp15A=_tmp158;_LL32:(void)_rethrow(_tmp15A);_LL2E:;}};}{
# 1079
const char*_tmp512;struct _dyneither_ptr _tmp15B=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp512=".cycio",_tag_dyneither(_tmp512,sizeof(char),7))));
const char*_tmp514;const char*_tmp513;struct Cyc___cycFILE*_tmp15C=Cyc_try_file_open((struct _dyneither_ptr)_tmp15B,((_tmp513="rb",_tag_dyneither(_tmp513,sizeof(char),3))),((_tmp514="interface object file",_tag_dyneither(_tmp514,sizeof(char),22))));
if(_tmp15C == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1086
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp15B);{
# 1088
struct Cyc_Interface_I*_tmp15D=Cyc_Interface_load(_tmp15C);
Cyc_file_close(_tmp15C);
return _tmp15D;};};};}
# 1093
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1097
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp51A(unsigned int*_tmp519,unsigned int*_tmp518,int**_tmp516){for(*_tmp519=0;*_tmp519 < *_tmp518;(*_tmp519)++){(*_tmp516)[*_tmp519]=(int)0;}}static void _tmp522(unsigned int*_tmp521,unsigned int*_tmp520,struct _dyneither_ptr**_tmp51E){for(*_tmp521=0;*_tmp521 < *_tmp520;(*_tmp521)++){(*_tmp51E)[*_tmp521]=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1134
_tag_dyneither(0,0,0));}}static void _tmp528(unsigned int*_tmp527,unsigned int*_tmp526,struct _dyneither_ptr**_tmp524){for(*_tmp527=0;*_tmp527 < *_tmp526;(*_tmp527)++){(*_tmp524)[*_tmp527]=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tag_dyneither(0,0,0));}}
# 1100
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1105
GC_blacklist_warn_clear();{
# 1107
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1112
struct _RegionHandle _tmp161=_new_region("r");struct _RegionHandle*r=& _tmp161;_push_region(r);{
# 1114
unsigned int _tmp519;unsigned int _tmp518;struct _dyneither_ptr _tmp517;int*_tmp516;unsigned int _tmp515;struct _dyneither_ptr _tmp162=(_tmp515=(unsigned int)argc,((_tmp516=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp515)),((_tmp517=_tag_dyneither(_tmp516,sizeof(int),_tmp515),((((_tmp518=_tmp515,_tmp51A(& _tmp519,& _tmp518,& _tmp516))),_tmp517)))))));
int _tmp163=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp51B;if(Cyc_strncmp(((_tmp51B="-B",_tag_dyneither(_tmp51B,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
++ _tmp163;}else{
# 1122
const char*_tmp51C;if(Cyc_strcmp(((_tmp51C="-b",_tag_dyneither(_tmp51C,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
++ _tmp163;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
++ _tmp163;}}}{
# 1134
unsigned int _tmp521;unsigned int _tmp520;struct _dyneither_ptr _tmp51F;struct _dyneither_ptr*_tmp51E;unsigned int _tmp51D;struct _dyneither_ptr _tmp166=(_tmp51D=(unsigned int)(_tmp163 + 1),((_tmp51E=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp51D)),((_tmp51F=_tag_dyneither(_tmp51E,sizeof(struct _dyneither_ptr),_tmp51D),((((_tmp520=_tmp51D,_tmp522(& _tmp521,& _tmp520,& _tmp51E))),_tmp51F)))))));
unsigned int _tmp527;unsigned int _tmp526;struct _dyneither_ptr _tmp525;struct _dyneither_ptr*_tmp524;unsigned int _tmp523;struct _dyneither_ptr _tmp167=(_tmp523=(unsigned int)(argc - _tmp163),((_tmp524=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp523)),((_tmp525=_tag_dyneither(_tmp524,sizeof(struct _dyneither_ptr),_tmp523),((((_tmp526=_tmp523,_tmp528(& _tmp527,& _tmp526,& _tmp524))),_tmp525)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1143
struct _tuple24*_tmpAC9;const char*_tmpAC8;const char*_tmpAC7;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpAC6;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpAC5;const char*_tmpAC4;struct _tuple24*_tmpAC3;const char*_tmpAC2;const char*_tmpAC1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpAC0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpABF;const char*_tmpABE;struct _tuple24*_tmpABD;const char*_tmpABC;const char*_tmpABB;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpABA;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpAB9;const char*_tmpAB8;struct _tuple24*_tmpAB7;const char*_tmpAB6;const char*_tmpAB5;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpAB4;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpAB3;const char*_tmpAB2;struct _tuple24*_tmpAB1;const char*_tmpAB0;const char*_tmpAAF;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpAAE;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpAAD;const char*_tmpAAC;struct _tuple24*_tmpAAB;const char*_tmpAAA;const char*_tmpAA9;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpAA8;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpAA7;const char*_tmpAA6;struct _tuple24*_tmpAA5;const char*_tmpAA4;const char*_tmpAA3;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpAA2;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpAA1;const char*_tmpAA0;struct _tuple24*_tmpA9F;const char*_tmpA9E;const char*_tmpA9D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA9C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA9B;const char*_tmpA9A;struct _tuple24*_tmpA99;const char*_tmpA98;const char*_tmpA97;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA96;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA95;const char*_tmpA94;struct _tuple24*_tmpA93;const char*_tmpA92;const char*_tmpA91;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA90;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA8F;const char*_tmpA8E;struct _tuple24*_tmpA8D;const char*_tmpA8C;const char*_tmpA8B;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA8A;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA89;const char*_tmpA88;struct _tuple24*_tmpA87;const char*_tmpA86;const char*_tmpA85;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA84;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA83;const char*_tmpA82;struct _tuple24*_tmpA81;const char*_tmpA80;const char*_tmpA7F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA7E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA7D;const char*_tmpA7C;struct _tuple24*_tmpA7B;const char*_tmpA7A;const char*_tmpA79;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA78;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA77;const char*_tmpA76;struct _tuple24*_tmpA75;const char*_tmpA74;const char*_tmpA73;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA72;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA71;const char*_tmpA70;struct _tuple24*_tmpA6F;const char*_tmpA6E;const char*_tmpA6D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA6C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA6B;const char*_tmpA6A;struct _tuple24*_tmpA69;const char*_tmpA68;const char*_tmpA67;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA66;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA65;const char*_tmpA64;struct _tuple24*_tmpA63;const char*_tmpA62;const char*_tmpA61;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA60;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA5F;const char*_tmpA5E;struct _tuple24*_tmpA5D;const char*_tmpA5C;const char*_tmpA5B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA5A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA59;const char*_tmpA58;struct _tuple24*_tmpA57;const char*_tmpA56;const char*_tmpA55;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA54;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA53;const char*_tmpA52;struct _tuple24*_tmpA51;const char*_tmpA50;const char*_tmpA4F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA4E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA4D;const char*_tmpA4C;struct _tuple24*_tmpA4B;const char*_tmpA4A;const char*_tmpA49;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA48;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA47;const char*_tmpA46;struct _tuple24*_tmpA45;const char*_tmpA44;const char*_tmpA43;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA42;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA41;const char*_tmpA40;struct _tuple24*_tmpA3F;const char*_tmpA3E;const char*_tmpA3D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA3C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA3B;const char*_tmpA3A;struct _tuple24*_tmpA39;const char*_tmpA38;const char*_tmpA37;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA36;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA35;const char*_tmpA34;struct _tuple24*_tmpA33;const char*_tmpA32;const char*_tmpA31;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA30;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA2F;const char*_tmpA2E;struct _tuple24*_tmpA2D;const char*_tmpA2C;const char*_tmpA2B;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA2A;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA29;const char*_tmpA28;struct _tuple24*_tmpA27;const char*_tmpA26;const char*_tmpA25;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA24;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA23;const char*_tmpA22;struct _tuple24*_tmpA21;const char*_tmpA20;const char*_tmpA1F;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA1E;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA1D;const char*_tmpA1C;struct _tuple24*_tmpA1B;const char*_tmpA1A;const char*_tmpA19;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA18;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA17;const char*_tmpA16;struct _tuple24*_tmpA15;const char*_tmpA14;const char*_tmpA13;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA12;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA11;const char*_tmpA10;struct _tuple24*_tmpA0F;const char*_tmpA0E;const char*_tmpA0D;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA0C;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA0B;const char*_tmpA0A;struct _tuple24*_tmpA09;const char*_tmpA08;const char*_tmpA07;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA06;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA05;const char*_tmpA04;struct _tuple24*_tmpA03;const char*_tmpA02;const char*_tmpA01;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA00;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9FF;const char*_tmp9FE;struct _tuple24*_tmp9FD;const char*_tmp9FC;const char*_tmp9FB;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9FA;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F9;const char*_tmp9F8;struct _tuple24*_tmp9F7;const char*_tmp9F6;const char*_tmp9F5;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F4;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F3;const char*_tmp9F2;struct _tuple24*_tmp9F1;const char*_tmp9F0;const char*_tmp9EF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9EE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9ED;const char*_tmp9EC;struct _tuple24*_tmp9EB;const char*_tmp9EA;const char*_tmp9E9;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E8;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E7;const char*_tmp9E6;struct _tuple24*_tmp9E5;const char*_tmp9E4;const char*_tmp9E3;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9E2;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9E1;const char*_tmp9E0;struct _tuple24*_tmp9DF;const char*_tmp9DE;const char*_tmp9DD;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9DC;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9DB;const char*_tmp9DA;struct _tuple24*_tmp9D9;const char*_tmp9D8;const char*_tmp9D7;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9D6;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9D5;const char*_tmp9D4;struct _tuple24*_tmp9D3;const char*_tmp9D2;const char*_tmp9D1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9D0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9CF;const char*_tmp9CE;struct _tuple24*_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9CA;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9C9;const char*_tmp9C8;struct _tuple24*_tmp9C7;const char*_tmp9C6;const char*_tmp9C5;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9C4;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9C3;const char*_tmp9C2;struct _tuple24*_tmp9C1;const char*_tmp9C0;const char*_tmp9BF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9BE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9BD;const char*_tmp9BC;struct _tuple24*_tmp9BB;const char*_tmp9BA;const char*_tmp9B9;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9B8;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9B7;const char*_tmp9B6;struct _tuple24*_tmp9B5;const char*_tmp9B4;const char*_tmp9B3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9B2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9B1;const char*_tmp9B0;struct _tuple24*_tmp9AF;const char*_tmp9AE;const char*_tmp9AD;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9AC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9AB;const char*_tmp9AA;struct _tuple24*_tmp9A9;const char*_tmp9A8;const char*_tmp9A7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A6;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A5;const char*_tmp9A4;struct _tuple24*_tmp9A3;const char*_tmp9A2;const char*_tmp9A1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp99F;const char*_tmp99E;struct _tuple24*_tmp99D;const char*_tmp99C;const char*_tmp99B;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp99A;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp999;const char*_tmp998;struct _tuple24*_tmp997;const char*_tmp996;const char*_tmp995;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp994;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp993;const char*_tmp992;struct _tuple24*_tmp991;const char*_tmp990;const char*_tmp98F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp98E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp98D;const char*_tmp98C;struct _tuple24*_tmp98B;const char*_tmp98A;const char*_tmp989;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp988;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp987;const char*_tmp986;struct _tuple24*_tmp985;const char*_tmp984;const char*_tmp983;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp982;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp981;const char*_tmp980;struct _tuple24*_tmp97F;const char*_tmp97E;const char*_tmp97D;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp97C;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp97B;const char*_tmp97A;struct _tuple24*_tmp979;const char*_tmp978;const char*_tmp977;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp976;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp975;const char*_tmp974;struct _tuple24*_tmp973;const char*_tmp972;const char*_tmp971;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp970;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp96F;const char*_tmp96E;struct _tuple24*_tmp96D;const char*_tmp96C;const char*_tmp96B;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp96A;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp969;const char*_tmp968;struct _tuple24*_tmp967;const char*_tmp966;const char*_tmp965;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp964;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp963;const char*_tmp962;struct _tuple24*_tmp961;const char*_tmp960;const char*_tmp95F;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp95E;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp95D;const char*_tmp95C;struct _tuple24*_tmp95B;const char*_tmp95A;const char*_tmp959;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp958;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp957;const char*_tmp956;struct _tuple24*_tmp955;const char*_tmp954;const char*_tmp953;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp952;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp951;const char*_tmp950;struct _tuple24*_tmp94F;const char*_tmp94E;const char*_tmp94D;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94C;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94B;const char*_tmp94A;struct _tuple24*_tmp949;const char*_tmp948;const char*_tmp947;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp946;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp945;const char*_tmp944;struct _tuple24*_tmp943;const char*_tmp942;const char*_tmp941;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp940;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp93F;const char*_tmp93E;struct _tuple24*_tmp93D;const char*_tmp93C;const char*_tmp93B;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp93A;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp939;const char*_tmp938;struct _tuple24*_tmp937;const char*_tmp936;const char*_tmp935;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp934;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp933;const char*_tmp932;struct _tuple24*_tmp931;const char*_tmp930;const char*_tmp92F;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp92E;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp92D;const char*_tmp92C;struct _tuple24*_tmp92B;const char*_tmp92A;const char*_tmp929;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp928;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp927;const char*_tmp926;struct _tuple24*_tmp925;const char*_tmp924;const char*_tmp923;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp922;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp921;const char*_tmp920;struct _tuple24*_tmp91F;const char*_tmp91E;const char*_tmp91D;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp91C;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp91B;const char*_tmp91A;struct _tuple24*_tmp919;const char*_tmp918;const char*_tmp917;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp916;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp915;const char*_tmp914;struct _tuple24*_tmp913;const char*_tmp912;const char*_tmp911;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp910;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp90F;const char*_tmp90E;struct _tuple24*_tmp90D;const char*_tmp90C;const char*_tmp90B;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp90A;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp909;const char*_tmp908;struct _tuple24*_tmp907;const char*_tmp906;const char*_tmp905;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp904;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp903;const char*_tmp902;struct _tuple24*_tmp901;const char*_tmp900;const char*_tmp8FF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8FE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8FD;const char*_tmp8FC;struct _tuple24*_tmp8FB;const char*_tmp8FA;const char*_tmp8F9;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8F8;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8F7;const char*_tmp8F6;struct _tuple24*_tmp8F5;const char*_tmp8F4;const char*_tmp8F3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8F2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8F1;const char*_tmp8F0;struct _tuple24*_tmp8EF;const char*_tmp8EE;const char*_tmp8ED;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8EC;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8EB;const char*_tmp8EA;struct _tuple24*_tmp8E9[80];struct Cyc_List_List*options=
(_tmp8E9[79]=(
# 1384
(_tmp8EF=_region_malloc(r,sizeof(*_tmp8EF)),((_tmp8EF->f1=((_tmp8EE="--parsestate",_tag_dyneither(_tmp8EE,sizeof(char),13))),((_tmp8EF->f2=0,((_tmp8EF->f3=((_tmp8ED="",_tag_dyneither(_tmp8ED,sizeof(char),1))),((_tmp8EF->f4=(void*)(
(_tmp8EB=_region_malloc(r,sizeof(*_tmp8EB)),((_tmp8EB[0]=((_tmp8EC.tag=3,((_tmp8EC.f1=& Cyc_ParseErrors_print_state_and_token,_tmp8EC)))),_tmp8EB)))),((_tmp8EF->f5=(
(_tmp8EA="print the parse state and token on a syntax error",_tag_dyneither(_tmp8EA,sizeof(char),50))),_tmp8EF)))))))))))),((_tmp8E9[78]=(
# 1381
(_tmp8F5=_region_malloc(r,sizeof(*_tmp8F5)),((_tmp8F5->f1=((_tmp8F4="--manyerrors",_tag_dyneither(_tmp8F4,sizeof(char),13))),((_tmp8F5->f2=0,((_tmp8F5->f3=((_tmp8F3="",_tag_dyneither(_tmp8F3,sizeof(char),1))),((_tmp8F5->f4=(void*)(
(_tmp8F1=_region_malloc(r,sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F2.tag=0,((_tmp8F2.f1=Cyc_set_many_errors,_tmp8F2)))),_tmp8F1)))),((_tmp8F5->f5=(
(_tmp8F0="don't stop after only a few errors.",_tag_dyneither(_tmp8F0,sizeof(char),36))),_tmp8F5)))))))))))),((_tmp8E9[77]=(
# 1378
(_tmp8FB=_region_malloc(r,sizeof(*_tmp8FB)),((_tmp8FB->f1=((_tmp8FA="--interface",_tag_dyneither(_tmp8FA,sizeof(char),12))),((_tmp8FB->f2=0,((_tmp8FB->f3=((_tmp8F9=" <file>",_tag_dyneither(_tmp8F9,sizeof(char),8))),((_tmp8FB->f4=(void*)(
(_tmp8F7=_region_malloc(r,sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8F8.tag=5,((_tmp8F8.f1=Cyc_set_specified_interface,_tmp8F8)))),_tmp8F7)))),((_tmp8FB->f5=(
(_tmp8F6="Set the interface file to be <file>.",_tag_dyneither(_tmp8F6,sizeof(char),37))),_tmp8FB)))))))))))),((_tmp8E9[76]=(
# 1375
(_tmp901=_region_malloc(r,sizeof(*_tmp901)),((_tmp901->f1=((_tmp900="--geninterface",_tag_dyneither(_tmp900,sizeof(char),15))),((_tmp901->f2=0,((_tmp901->f3=((_tmp8FF="",_tag_dyneither(_tmp8FF,sizeof(char),1))),((_tmp901->f4=(void*)(
(_tmp8FD=_region_malloc(r,sizeof(*_tmp8FD)),((_tmp8FD[0]=((_tmp8FE.tag=3,((_tmp8FE.f1=& Cyc_generate_interface_r,_tmp8FE)))),_tmp8FD)))),((_tmp901->f5=(
(_tmp8FC="Generate interface files",_tag_dyneither(_tmp8FC,sizeof(char),25))),_tmp901)))))))))))),((_tmp8E9[75]=(
# 1372
(_tmp907=_region_malloc(r,sizeof(*_tmp907)),((_tmp907->f1=((_tmp906="-idirafter",_tag_dyneither(_tmp906,sizeof(char),11))),((_tmp907->f2=0,((_tmp907->f3=((_tmp905="<dir>",_tag_dyneither(_tmp905,sizeof(char),6))),((_tmp907->f4=(void*)(
(_tmp903=_region_malloc(r,sizeof(*_tmp903)),((_tmp903[0]=((_tmp904.tag=5,((_tmp904.f1=Cyc_add_idirafter,_tmp904)))),_tmp903)))),((_tmp907->f5=(
(_tmp902="Add the directory to the second include path.",_tag_dyneither(_tmp902,sizeof(char),46))),_tmp907)))))))))))),((_tmp8E9[74]=(
# 1369
(_tmp90D=_region_malloc(r,sizeof(*_tmp90D)),((_tmp90D->f1=((_tmp90C="-isystem",_tag_dyneither(_tmp90C,sizeof(char),9))),((_tmp90D->f2=0,((_tmp90D->f3=((_tmp90B="<dir>",_tag_dyneither(_tmp90B,sizeof(char),6))),((_tmp90D->f4=(void*)(
(_tmp909=_region_malloc(r,sizeof(*_tmp909)),((_tmp909[0]=((_tmp90A.tag=5,((_tmp90A.f1=Cyc_add_isystem,_tmp90A)))),_tmp909)))),((_tmp90D->f5=(
(_tmp908="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp908,sizeof(char),90))),_tmp90D)))))))))))),((_tmp8E9[73]=(
# 1366
(_tmp913=_region_malloc(r,sizeof(*_tmp913)),((_tmp913->f1=((_tmp912="-iwithprefixbefore",_tag_dyneither(_tmp912,sizeof(char),19))),((_tmp913->f2=0,((_tmp913->f3=((_tmp911="<dir>",_tag_dyneither(_tmp911,sizeof(char),6))),((_tmp913->f4=(void*)(
(_tmp90F=_region_malloc(r,sizeof(*_tmp90F)),((_tmp90F[0]=((_tmp910.tag=5,((_tmp910.f1=Cyc_add_iwithprefixbefore,_tmp910)))),_tmp90F)))),((_tmp913->f5=(
(_tmp90E="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp90E,sizeof(char),45))),_tmp913)))))))))))),((_tmp8E9[72]=(
# 1363
(_tmp919=_region_malloc(r,sizeof(*_tmp919)),((_tmp919->f1=((_tmp918="-iwithprefix",_tag_dyneither(_tmp918,sizeof(char),13))),((_tmp919->f2=0,((_tmp919->f3=((_tmp917="<dir>",_tag_dyneither(_tmp917,sizeof(char),6))),((_tmp919->f4=(void*)(
(_tmp915=_region_malloc(r,sizeof(*_tmp915)),((_tmp915[0]=((_tmp916.tag=5,((_tmp916.f1=Cyc_add_iwithprefix,_tmp916)))),_tmp915)))),((_tmp919->f5=(
(_tmp914="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp914,sizeof(char),47))),_tmp919)))))))))))),((_tmp8E9[71]=(
# 1360
(_tmp91F=_region_malloc(r,sizeof(*_tmp91F)),((_tmp91F->f1=((_tmp91E="-iprefix",_tag_dyneither(_tmp91E,sizeof(char),9))),((_tmp91F->f2=0,((_tmp91F->f3=((_tmp91D="<prefix>",_tag_dyneither(_tmp91D,sizeof(char),9))),((_tmp91F->f4=(void*)(
(_tmp91B=_region_malloc(r,sizeof(*_tmp91B)),((_tmp91B[0]=((_tmp91C.tag=5,((_tmp91C.f1=Cyc_add_iprefix,_tmp91C)))),_tmp91B)))),((_tmp91F->f5=(
(_tmp91A="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp91A,sizeof(char),67))),_tmp91F)))))))))))),((_tmp8E9[70]=(
# 1357
(_tmp925=_region_malloc(r,sizeof(*_tmp925)),((_tmp925->f1=((_tmp924="-noregions",_tag_dyneither(_tmp924,sizeof(char),11))),((_tmp925->f2=0,((_tmp925->f3=((_tmp923="",_tag_dyneither(_tmp923,sizeof(char),1))),((_tmp925->f4=(void*)(
(_tmp921=_region_malloc(r,sizeof(*_tmp921)),((_tmp921[0]=((_tmp922.tag=3,((_tmp922.f1=& Cyc_Absyn_no_regions,_tmp922)))),_tmp921)))),((_tmp925->f5=(
(_tmp920="Generate code that doesn't use regions",_tag_dyneither(_tmp920,sizeof(char),39))),_tmp925)))))))))))),((_tmp8E9[69]=(
# 1354
(_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B->f1=((_tmp92A="-port",_tag_dyneither(_tmp92A,sizeof(char),6))),((_tmp92B->f2=0,((_tmp92B->f3=((_tmp929="",_tag_dyneither(_tmp929,sizeof(char),1))),((_tmp92B->f4=(void*)(
(_tmp927=_region_malloc(r,sizeof(*_tmp927)),((_tmp927[0]=((_tmp928.tag=0,((_tmp928.f1=Cyc_set_port_c_code,_tmp928)))),_tmp927)))),((_tmp92B->f5=(
(_tmp926="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp926,sizeof(char),38))),_tmp92B)))))))))))),((_tmp8E9[68]=(
# 1351
(_tmp931=_region_malloc(r,sizeof(*_tmp931)),((_tmp931->f1=((_tmp930="-detailedlocation",_tag_dyneither(_tmp930,sizeof(char),18))),((_tmp931->f2=0,((_tmp931->f3=((_tmp92F="",_tag_dyneither(_tmp92F,sizeof(char),1))),((_tmp931->f4=(void*)(
(_tmp92D=_region_malloc(r,sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92E.tag=4,((_tmp92E.f1=& Cyc_Position_use_gcc_style_location,_tmp92E)))),_tmp92D)))),((_tmp931->f5=(
(_tmp92C="Try to give more detailed location information for errors",_tag_dyneither(_tmp92C,sizeof(char),58))),_tmp931)))))))))))),((_tmp8E9[67]=(
# 1348
(_tmp937=_region_malloc(r,sizeof(*_tmp937)),((_tmp937->f1=((_tmp936="-noregister",_tag_dyneither(_tmp936,sizeof(char),12))),((_tmp937->f2=0,((_tmp937->f3=((_tmp935="",_tag_dyneither(_tmp935,sizeof(char),1))),((_tmp937->f4=(void*)(
(_tmp933=_region_malloc(r,sizeof(*_tmp933)),((_tmp933[0]=((_tmp934.tag=3,((_tmp934.f1=& Cyc_Parse_no_register,_tmp934)))),_tmp933)))),((_tmp937->f5=(
(_tmp932="Treat register storage class as public",_tag_dyneither(_tmp932,sizeof(char),39))),_tmp937)))))))))))),((_tmp8E9[66]=(
# 1345
(_tmp93D=_region_malloc(r,sizeof(*_tmp93D)),((_tmp93D->f1=((_tmp93C="-warnregioncoerce",_tag_dyneither(_tmp93C,sizeof(char),18))),((_tmp93D->f2=0,((_tmp93D->f3=((_tmp93B="",_tag_dyneither(_tmp93B,sizeof(char),1))),((_tmp93D->f4=(void*)(
(_tmp939=_region_malloc(r,sizeof(*_tmp939)),((_tmp939[0]=((_tmp93A.tag=3,((_tmp93A.f1=& Cyc_Tcutil_warn_region_coerce,_tmp93A)))),_tmp939)))),((_tmp93D->f5=(
(_tmp938="Warn when any region coercion is inserted",_tag_dyneither(_tmp938,sizeof(char),42))),_tmp93D)))))))))))),((_tmp8E9[65]=(
# 1342
(_tmp943=_region_malloc(r,sizeof(*_tmp943)),((_tmp943->f1=((_tmp942="-warnaliascoercion",_tag_dyneither(_tmp942,sizeof(char),19))),((_tmp943->f2=0,((_tmp943->f3=((_tmp941="",_tag_dyneither(_tmp941,sizeof(char),1))),((_tmp943->f4=(void*)(
(_tmp93F=_region_malloc(r,sizeof(*_tmp93F)),((_tmp93F[0]=((_tmp940.tag=3,((_tmp940.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp940)))),_tmp93F)))),((_tmp943->f5=(
(_tmp93E="Warn when any alias coercion is inserted",_tag_dyneither(_tmp93E,sizeof(char),41))),_tmp943)))))))))))),((_tmp8E9[64]=(
# 1339
(_tmp949=_region_malloc(r,sizeof(*_tmp949)),((_tmp949->f1=((_tmp948="-warnnullchecks",_tag_dyneither(_tmp948,sizeof(char),16))),((_tmp949->f2=0,((_tmp949->f3=((_tmp947="",_tag_dyneither(_tmp947,sizeof(char),1))),((_tmp949->f4=(void*)(
(_tmp945=_region_malloc(r,sizeof(*_tmp945)),((_tmp945[0]=((_tmp946.tag=3,((_tmp946.f1=& Cyc_Toc_warn_all_null_deref,_tmp946)))),_tmp945)))),((_tmp949->f5=(
(_tmp944="Warn when any null check can't be eliminated",_tag_dyneither(_tmp944,sizeof(char),45))),_tmp949)))))))))))),((_tmp8E9[63]=(
# 1336
(_tmp94F=_region_malloc(r,sizeof(*_tmp94F)),((_tmp94F->f1=((_tmp94E="-warnboundschecks",_tag_dyneither(_tmp94E,sizeof(char),18))),((_tmp94F->f2=0,((_tmp94F->f3=((_tmp94D="",_tag_dyneither(_tmp94D,sizeof(char),1))),((_tmp94F->f4=(void*)(
(_tmp94B=_region_malloc(r,sizeof(*_tmp94B)),((_tmp94B[0]=((_tmp94C.tag=3,((_tmp94C.f1=& Cyc_Toc_warn_bounds_checks,_tmp94C)))),_tmp94B)))),((_tmp94F->f5=(
(_tmp94A="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp94A,sizeof(char),44))),_tmp94F)))))))))))),((_tmp8E9[62]=(
# 1333
(_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955->f1=((_tmp954="-CI",_tag_dyneither(_tmp954,sizeof(char),4))),((_tmp955->f2=0,((_tmp955->f3=((_tmp953=" <file>",_tag_dyneither(_tmp953,sizeof(char),8))),((_tmp955->f4=(void*)(
(_tmp951=_region_malloc(r,sizeof(*_tmp951)),((_tmp951[0]=((_tmp952.tag=5,((_tmp952.f1=Cyc_set_cyc_include,_tmp952)))),_tmp951)))),((_tmp955->f5=(
(_tmp950="Set cyc_include.h to be <file>",_tag_dyneither(_tmp950,sizeof(char),31))),_tmp955)))))))))))),((_tmp8E9[61]=(
# 1330
(_tmp95B=_region_malloc(r,sizeof(*_tmp95B)),((_tmp95B->f1=((_tmp95A="-compile-for-boot",_tag_dyneither(_tmp95A,sizeof(char),18))),((_tmp95B->f2=0,((_tmp95B->f3=((_tmp959="",_tag_dyneither(_tmp959,sizeof(char),1))),((_tmp95B->f4=(void*)(
(_tmp957=_region_malloc(r,sizeof(*_tmp957)),((_tmp957[0]=((_tmp958.tag=3,((_tmp958.f1=& Cyc_Lex_compile_for_boot_r,_tmp958)))),_tmp957)))),((_tmp95B->f5=(
(_tmp956="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp956,sizeof(char),71))),_tmp95B)))))))))))),((_tmp8E9[60]=(
# 1327
(_tmp961=_region_malloc(r,sizeof(*_tmp961)),((_tmp961->f1=((_tmp960="-nocyc_setjmp",_tag_dyneither(_tmp960,sizeof(char),14))),((_tmp961->f2=0,((_tmp961->f3=((_tmp95F="",_tag_dyneither(_tmp95F,sizeof(char),1))),((_tmp961->f4=(void*)(
(_tmp95D=_region_malloc(r,sizeof(*_tmp95D)),((_tmp95D[0]=((_tmp95E.tag=3,((_tmp95E.f1=& Cyc_nocyc_setjmp_r,_tmp95E)))),_tmp95D)))),((_tmp961->f5=(
(_tmp95C="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp95C,sizeof(char),46))),_tmp961)))))))))))),((_tmp8E9[59]=(
# 1324
(_tmp967=_region_malloc(r,sizeof(*_tmp967)),((_tmp967->f1=((_tmp966="-printalleffects",_tag_dyneither(_tmp966,sizeof(char),17))),((_tmp967->f2=0,((_tmp967->f3=((_tmp965="",_tag_dyneither(_tmp965,sizeof(char),1))),((_tmp967->f4=(void*)(
(_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963[0]=((_tmp964.tag=3,((_tmp964.f1=& Cyc_print_all_effects_r,_tmp964)))),_tmp963)))),((_tmp967->f5=(
(_tmp962="Print effects for functions (type debugging)",_tag_dyneither(_tmp962,sizeof(char),45))),_tmp967)))))))))))),((_tmp8E9[58]=(
# 1321
(_tmp96D=_region_malloc(r,sizeof(*_tmp96D)),((_tmp96D->f1=((_tmp96C="-printfullevars",_tag_dyneither(_tmp96C,sizeof(char),16))),((_tmp96D->f2=0,((_tmp96D->f3=((_tmp96B="",_tag_dyneither(_tmp96B,sizeof(char),1))),((_tmp96D->f4=(void*)(
(_tmp969=_region_malloc(r,sizeof(*_tmp969)),((_tmp969[0]=((_tmp96A.tag=3,((_tmp96A.f1=& Cyc_print_full_evars_r,_tmp96A)))),_tmp969)))),((_tmp96D->f5=(
(_tmp968="Print full information for evars (type debugging)",_tag_dyneither(_tmp968,sizeof(char),50))),_tmp96D)))))))))))),((_tmp8E9[57]=(
# 1318
(_tmp973=_region_malloc(r,sizeof(*_tmp973)),((_tmp973->f1=((_tmp972="-printallkinds",_tag_dyneither(_tmp972,sizeof(char),15))),((_tmp973->f2=0,((_tmp973->f3=((_tmp971="",_tag_dyneither(_tmp971,sizeof(char),1))),((_tmp973->f4=(void*)(
(_tmp96F=_region_malloc(r,sizeof(*_tmp96F)),((_tmp96F[0]=((_tmp970.tag=3,((_tmp970.f1=& Cyc_print_all_kinds_r,_tmp970)))),_tmp96F)))),((_tmp973->f5=(
(_tmp96E="Always print kinds of type variables",_tag_dyneither(_tmp96E,sizeof(char),37))),_tmp973)))))))))))),((_tmp8E9[56]=(
# 1315
(_tmp979=_region_malloc(r,sizeof(*_tmp979)),((_tmp979->f1=((_tmp978="-printalltvars",_tag_dyneither(_tmp978,sizeof(char),15))),((_tmp979->f2=0,((_tmp979->f3=((_tmp977="",_tag_dyneither(_tmp977,sizeof(char),1))),((_tmp979->f4=(void*)(
(_tmp975=_region_malloc(r,sizeof(*_tmp975)),((_tmp975[0]=((_tmp976.tag=3,((_tmp976.f1=& Cyc_print_all_tvars_r,_tmp976)))),_tmp975)))),((_tmp979->f5=(
(_tmp974="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp974,sizeof(char),57))),_tmp979)))))))))))),((_tmp8E9[55]=(
# 1312
(_tmp97F=_region_malloc(r,sizeof(*_tmp97F)),((_tmp97F->f1=((_tmp97E="-noexpandtypedefs",_tag_dyneither(_tmp97E,sizeof(char),18))),((_tmp97F->f2=0,((_tmp97F->f3=((_tmp97D="",_tag_dyneither(_tmp97D,sizeof(char),1))),((_tmp97F->f4=(void*)(
(_tmp97B=_region_malloc(r,sizeof(*_tmp97B)),((_tmp97B[0]=((_tmp97C.tag=3,((_tmp97C.f1=& Cyc_noexpand_r,_tmp97C)))),_tmp97B)))),((_tmp97F->f5=(
(_tmp97A="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp97A,sizeof(char),41))),_tmp97F)))))))))))),((_tmp8E9[54]=(
# 1309
(_tmp985=_region_malloc(r,sizeof(*_tmp985)),((_tmp985->f1=((_tmp984="-noremoveunused",_tag_dyneither(_tmp984,sizeof(char),16))),((_tmp985->f2=0,((_tmp985->f3=((_tmp983="",_tag_dyneither(_tmp983,sizeof(char),1))),((_tmp985->f4=(void*)(
(_tmp981=_region_malloc(r,sizeof(*_tmp981)),((_tmp981[0]=((_tmp982.tag=3,((_tmp982.f1=& Cyc_noshake_r,_tmp982)))),_tmp981)))),((_tmp985->f5=(
(_tmp980="Don't remove externed variables that aren't used",_tag_dyneither(_tmp980,sizeof(char),49))),_tmp985)))))))))))),((_tmp8E9[53]=(
# 1306
(_tmp98B=_region_malloc(r,sizeof(*_tmp98B)),((_tmp98B->f1=((_tmp98A="-nogc",_tag_dyneither(_tmp98A,sizeof(char),6))),((_tmp98B->f2=0,((_tmp98B->f3=((_tmp989="",_tag_dyneither(_tmp989,sizeof(char),1))),((_tmp98B->f4=(void*)(
(_tmp987=_region_malloc(r,sizeof(*_tmp987)),((_tmp987[0]=((_tmp988.tag=3,((_tmp988.f1=& Cyc_nogc_r,_tmp988)))),_tmp987)))),((_tmp98B->f5=(
(_tmp986="Don't link in the garbage collector",_tag_dyneither(_tmp986,sizeof(char),36))),_tmp98B)))))))))))),((_tmp8E9[52]=(
# 1303
(_tmp991=_region_malloc(r,sizeof(*_tmp991)),((_tmp991->f1=((_tmp990="-nocyc",_tag_dyneither(_tmp990,sizeof(char),7))),((_tmp991->f2=0,((_tmp991->f3=((_tmp98F="",_tag_dyneither(_tmp98F,sizeof(char),1))),((_tmp991->f4=(void*)(
(_tmp98D=_region_malloc(r,sizeof(*_tmp98D)),((_tmp98D[0]=((_tmp98E.tag=0,((_tmp98E.f1=Cyc_set_nocyc,_tmp98E)))),_tmp98D)))),((_tmp991->f5=(
(_tmp98C="Don't add implicit namespace Cyc",_tag_dyneither(_tmp98C,sizeof(char),33))),_tmp991)))))))))))),((_tmp8E9[51]=(
# 1300
(_tmp997=_region_malloc(r,sizeof(*_tmp997)),((_tmp997->f1=((_tmp996="-no-cpp-precomp",_tag_dyneither(_tmp996,sizeof(char),16))),((_tmp997->f2=0,((_tmp997->f3=((_tmp995="",_tag_dyneither(_tmp995,sizeof(char),1))),((_tmp997->f4=(void*)(
(_tmp993=_region_malloc(r,sizeof(*_tmp993)),((_tmp993[0]=((_tmp994.tag=0,((_tmp994.f1=Cyc_set_nocppprecomp,_tmp994)))),_tmp993)))),((_tmp997->f5=(
(_tmp992="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp992,sizeof(char),40))),_tmp997)))))))))))),((_tmp8E9[50]=(
# 1297
(_tmp99D=_region_malloc(r,sizeof(*_tmp99D)),((_tmp99D->f1=((_tmp99C="-use-cpp",_tag_dyneither(_tmp99C,sizeof(char),9))),((_tmp99D->f2=0,((_tmp99D->f3=((_tmp99B="<path>",_tag_dyneither(_tmp99B,sizeof(char),7))),((_tmp99D->f4=(void*)(
(_tmp999=_region_malloc(r,sizeof(*_tmp999)),((_tmp999[0]=((_tmp99A.tag=5,((_tmp99A.f1=Cyc_set_cpp,_tmp99A)))),_tmp999)))),((_tmp99D->f5=(
(_tmp998="Indicate which preprocessor to use",_tag_dyneither(_tmp998,sizeof(char),35))),_tmp99D)))))))))))),((_tmp8E9[49]=(
# 1294
(_tmp9A3=_region_malloc(r,sizeof(*_tmp9A3)),((_tmp9A3->f1=((_tmp9A2="--inline-checks",_tag_dyneither(_tmp9A2,sizeof(char),16))),((_tmp9A3->f2=0,((_tmp9A3->f3=((_tmp9A1="",_tag_dyneither(_tmp9A1,sizeof(char),1))),((_tmp9A3->f4=(void*)(
(_tmp99F=_region_malloc(r,sizeof(*_tmp99F)),((_tmp99F[0]=((_tmp9A0.tag=0,((_tmp9A0.f1=Cyc_set_inline_functions,_tmp9A0)))),_tmp99F)))),((_tmp9A3->f5=(
(_tmp99E="Inline bounds checks instead of #define",_tag_dyneither(_tmp99E,sizeof(char),40))),_tmp9A3)))))))))))),((_tmp8E9[48]=(
# 1291
(_tmp9A9=_region_malloc(r,sizeof(*_tmp9A9)),((_tmp9A9->f1=((_tmp9A8="--noboundschecks",_tag_dyneither(_tmp9A8,sizeof(char),17))),((_tmp9A9->f2=0,((_tmp9A9->f3=((_tmp9A7="",_tag_dyneither(_tmp9A7,sizeof(char),1))),((_tmp9A9->f4=(void*)(
(_tmp9A5=_region_malloc(r,sizeof(*_tmp9A5)),((_tmp9A5[0]=((_tmp9A6.tag=0,((_tmp9A6.f1=Cyc_set_noboundschecks,_tmp9A6)))),_tmp9A5)))),((_tmp9A9->f5=(
(_tmp9A4="Disable bounds checks",_tag_dyneither(_tmp9A4,sizeof(char),22))),_tmp9A9)))))))))))),((_tmp8E9[47]=(
# 1288
(_tmp9AF=_region_malloc(r,sizeof(*_tmp9AF)),((_tmp9AF->f1=((_tmp9AE="--nonullchecks",_tag_dyneither(_tmp9AE,sizeof(char),15))),((_tmp9AF->f2=0,((_tmp9AF->f3=((_tmp9AD="",_tag_dyneither(_tmp9AD,sizeof(char),1))),((_tmp9AF->f4=(void*)(
(_tmp9AB=_region_malloc(r,sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AC.tag=0,((_tmp9AC.f1=Cyc_set_nonullchecks,_tmp9AC)))),_tmp9AB)))),((_tmp9AF->f5=(
(_tmp9AA="Disable null checks",_tag_dyneither(_tmp9AA,sizeof(char),20))),_tmp9AF)))))))))))),((_tmp8E9[46]=(
# 1285
(_tmp9B5=_region_malloc(r,sizeof(*_tmp9B5)),((_tmp9B5->f1=((_tmp9B4="--nochecks",_tag_dyneither(_tmp9B4,sizeof(char),11))),((_tmp9B5->f2=0,((_tmp9B5->f3=((_tmp9B3="",_tag_dyneither(_tmp9B3,sizeof(char),1))),((_tmp9B5->f4=(void*)(
(_tmp9B1=_region_malloc(r,sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B2.tag=0,((_tmp9B2.f1=Cyc_set_nochecks,_tmp9B2)))),_tmp9B1)))),((_tmp9B5->f5=(
(_tmp9B0="Disable bounds/null checks",_tag_dyneither(_tmp9B0,sizeof(char),27))),_tmp9B5)))))))))))),((_tmp8E9[45]=(
# 1282
(_tmp9BB=_region_malloc(r,sizeof(*_tmp9BB)),((_tmp9BB->f1=((_tmp9BA="--nolineno",_tag_dyneither(_tmp9BA,sizeof(char),11))),((_tmp9BB->f2=0,((_tmp9BB->f3=((_tmp9B9="",_tag_dyneither(_tmp9B9,sizeof(char),1))),((_tmp9BB->f4=(void*)(
(_tmp9B7=_region_malloc(r,sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=0,((_tmp9B8.f1=Cyc_clear_lineno,_tmp9B8)))),_tmp9B7)))),((_tmp9BB->f5=(
(_tmp9B6="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp9B6,sizeof(char),63))),_tmp9BB)))))))))))),((_tmp8E9[44]=(
# 1279
(_tmp9C1=_region_malloc(r,sizeof(*_tmp9C1)),((_tmp9C1->f1=((_tmp9C0="-save-c",_tag_dyneither(_tmp9C0,sizeof(char),8))),((_tmp9C1->f2=0,((_tmp9C1->f3=((_tmp9BF="",_tag_dyneither(_tmp9BF,sizeof(char),1))),((_tmp9C1->f4=(void*)(
(_tmp9BD=_region_malloc(r,sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=3,((_tmp9BE.f1=& Cyc_save_c_r,_tmp9BE)))),_tmp9BD)))),((_tmp9C1->f5=(
(_tmp9BC="Don't delete temporary C files",_tag_dyneither(_tmp9BC,sizeof(char),31))),_tmp9C1)))))))))))),((_tmp8E9[43]=(
# 1276
(_tmp9C7=_region_malloc(r,sizeof(*_tmp9C7)),((_tmp9C7->f1=((_tmp9C6="-save-temps",_tag_dyneither(_tmp9C6,sizeof(char),12))),((_tmp9C7->f2=0,((_tmp9C7->f3=((_tmp9C5="",_tag_dyneither(_tmp9C5,sizeof(char),1))),((_tmp9C7->f4=(void*)(
(_tmp9C3=_region_malloc(r,sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=0,((_tmp9C4.f1=Cyc_set_save_temps,_tmp9C4)))),_tmp9C3)))),((_tmp9C7->f5=(
(_tmp9C2="Don't delete temporary files",_tag_dyneither(_tmp9C2,sizeof(char),29))),_tmp9C7)))))))))))),((_tmp8E9[42]=(
# 1273
(_tmp9CD=_region_malloc(r,sizeof(*_tmp9CD)),((_tmp9CD->f1=((_tmp9CC="-c-comp",_tag_dyneither(_tmp9CC,sizeof(char),8))),((_tmp9CD->f2=0,((_tmp9CD->f3=((_tmp9CB=" <compiler>",_tag_dyneither(_tmp9CB,sizeof(char),12))),((_tmp9CD->f4=(void*)(
(_tmp9C9=_region_malloc(r,sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=5,((_tmp9CA.f1=Cyc_set_c_compiler,_tmp9CA)))),_tmp9C9)))),((_tmp9CD->f5=(
(_tmp9C8="Produce C output for the given compiler",_tag_dyneither(_tmp9C8,sizeof(char),40))),_tmp9CD)))))))))))),((_tmp8E9[41]=(
# 1270
(_tmp9D3=_region_malloc(r,sizeof(*_tmp9D3)),((_tmp9D3->f1=((_tmp9D2="-un-gcc",_tag_dyneither(_tmp9D2,sizeof(char),8))),((_tmp9D3->f2=0,((_tmp9D3->f3=((_tmp9D1="",_tag_dyneither(_tmp9D1,sizeof(char),1))),((_tmp9D3->f4=(void*)(
(_tmp9CF=_region_malloc(r,sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=0,((_tmp9D0.f1=Cyc_set_tovc,_tmp9D0)))),_tmp9CF)))),((_tmp9D3->f5=(
(_tmp9CE="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp9CE,sizeof(char),57))),_tmp9D3)))))))))))),((_tmp8E9[40]=(
# 1267
(_tmp9D9=_region_malloc(r,sizeof(*_tmp9D9)),((_tmp9D9->f1=((_tmp9D8="-no-elim-statement-expressions",_tag_dyneither(_tmp9D8,sizeof(char),31))),((_tmp9D9->f2=0,((_tmp9D9->f3=((_tmp9D7="",_tag_dyneither(_tmp9D7,sizeof(char),1))),((_tmp9D9->f4=(void*)(
(_tmp9D5=_region_malloc(r,sizeof(*_tmp9D5)),((_tmp9D5[0]=((_tmp9D6.tag=4,((_tmp9D6.f1=& Cyc_elim_se_r,_tmp9D6)))),_tmp9D5)))),((_tmp9D9->f5=(
(_tmp9D4="Do not avoid statement expressions in C output",_tag_dyneither(_tmp9D4,sizeof(char),47))),_tmp9D9)))))))))))),((_tmp8E9[39]=(
# 1263
(_tmp9DF=_region_malloc(r,sizeof(*_tmp9DF)),((_tmp9DF->f1=((_tmp9DE="-elim-statement-expressions",_tag_dyneither(_tmp9DE,sizeof(char),28))),((_tmp9DF->f2=0,((_tmp9DF->f3=((_tmp9DD="",_tag_dyneither(_tmp9DD,sizeof(char),1))),((_tmp9DF->f4=(void*)(
(_tmp9DB=_region_malloc(r,sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DC.tag=3,((_tmp9DC.f1=& Cyc_elim_se_r,_tmp9DC)))),_tmp9DB)))),((_tmp9DF->f5=(
# 1266
(_tmp9DA="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp9DA,sizeof(char),66))),_tmp9DF)))))))))))),((_tmp8E9[38]=(
# 1260
(_tmp9E5=_region_malloc(r,sizeof(*_tmp9E5)),((_tmp9E5->f1=((_tmp9E4="-up",_tag_dyneither(_tmp9E4,sizeof(char),4))),((_tmp9E5->f2=0,((_tmp9E5->f3=((_tmp9E3="",_tag_dyneither(_tmp9E3,sizeof(char),1))),((_tmp9E5->f4=(void*)(
(_tmp9E1=_region_malloc(r,sizeof(*_tmp9E1)),((_tmp9E1[0]=((_tmp9E2.tag=4,((_tmp9E2.f1=& Cyc_pp_r,_tmp9E2)))),_tmp9E1)))),((_tmp9E5->f5=(
(_tmp9E0="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp9E0,sizeof(char),55))),_tmp9E5)))))))))))),((_tmp8E9[37]=(
# 1257
(_tmp9EB=_region_malloc(r,sizeof(*_tmp9EB)),((_tmp9EB->f1=((_tmp9EA="-pp",_tag_dyneither(_tmp9EA,sizeof(char),4))),((_tmp9EB->f2=0,((_tmp9EB->f3=((_tmp9E9="",_tag_dyneither(_tmp9E9,sizeof(char),1))),((_tmp9EB->f4=(void*)(
(_tmp9E7=_region_malloc(r,sizeof(*_tmp9E7)),((_tmp9E7[0]=((_tmp9E8.tag=3,((_tmp9E8.f1=& Cyc_pp_r,_tmp9E8)))),_tmp9E7)))),((_tmp9EB->f5=(
(_tmp9E6="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp9E6,sizeof(char),47))),_tmp9EB)))))))))))),((_tmp8E9[36]=(
# 1254
(_tmp9F1=_region_malloc(r,sizeof(*_tmp9F1)),((_tmp9F1->f1=((_tmp9F0="-ic",_tag_dyneither(_tmp9F0,sizeof(char),4))),((_tmp9F1->f2=0,((_tmp9F1->f3=((_tmp9EF="",_tag_dyneither(_tmp9EF,sizeof(char),1))),((_tmp9F1->f4=(void*)(
(_tmp9ED=_region_malloc(r,sizeof(*_tmp9ED)),((_tmp9ED[0]=((_tmp9EE.tag=3,((_tmp9EE.f1=& Cyc_ic_r,_tmp9EE)))),_tmp9ED)))),((_tmp9F1->f5=(
(_tmp9EC="Activate the link-checker",_tag_dyneither(_tmp9EC,sizeof(char),26))),_tmp9F1)))))))))))),((_tmp8E9[35]=(
# 1251
(_tmp9F7=_region_malloc(r,sizeof(*_tmp9F7)),((_tmp9F7->f1=((_tmp9F6="-stopafter-toc",_tag_dyneither(_tmp9F6,sizeof(char),15))),((_tmp9F7->f2=0,((_tmp9F7->f3=((_tmp9F5="",_tag_dyneither(_tmp9F5,sizeof(char),1))),((_tmp9F7->f4=(void*)(
(_tmp9F3=_region_malloc(r,sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F4.tag=3,((_tmp9F4.f1=& Cyc_toc_r,_tmp9F4)))),_tmp9F3)))),((_tmp9F7->f5=(
(_tmp9F2="Stop after translation to C",_tag_dyneither(_tmp9F2,sizeof(char),28))),_tmp9F7)))))))))))),((_tmp8E9[34]=(
# 1248
(_tmp9FD=_region_malloc(r,sizeof(*_tmp9FD)),((_tmp9FD->f1=((_tmp9FC="-stopafter-cf",_tag_dyneither(_tmp9FC,sizeof(char),14))),((_tmp9FD->f2=0,((_tmp9FD->f3=((_tmp9FB="",_tag_dyneither(_tmp9FB,sizeof(char),1))),((_tmp9FD->f4=(void*)(
(_tmp9F9=_region_malloc(r,sizeof(*_tmp9F9)),((_tmp9F9[0]=((_tmp9FA.tag=3,((_tmp9FA.f1=& Cyc_cf_r,_tmp9FA)))),_tmp9F9)))),((_tmp9FD->f5=(
(_tmp9F8="Stop after control-flow checking",_tag_dyneither(_tmp9F8,sizeof(char),33))),_tmp9FD)))))))))))),((_tmp8E9[33]=(
# 1245
(_tmpA03=_region_malloc(r,sizeof(*_tmpA03)),((_tmpA03->f1=((_tmpA02="-noprint",_tag_dyneither(_tmpA02,sizeof(char),9))),((_tmpA03->f2=0,((_tmpA03->f3=((_tmpA01="",_tag_dyneither(_tmpA01,sizeof(char),1))),((_tmpA03->f4=(void*)(
(_tmp9FF=_region_malloc(r,sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmpA00.tag=3,((_tmpA00.f1=& Cyc_noprint_r,_tmpA00)))),_tmp9FF)))),((_tmpA03->f5=(
(_tmp9FE="Do not print output (when stopping early)",_tag_dyneither(_tmp9FE,sizeof(char),42))),_tmpA03)))))))))))),((_tmp8E9[32]=(
# 1242
(_tmpA09=_region_malloc(r,sizeof(*_tmpA09)),((_tmpA09->f1=((_tmpA08="-stopafter-tc",_tag_dyneither(_tmpA08,sizeof(char),14))),((_tmpA09->f2=0,((_tmpA09->f3=((_tmpA07="",_tag_dyneither(_tmpA07,sizeof(char),1))),((_tmpA09->f4=(void*)(
(_tmpA05=_region_malloc(r,sizeof(*_tmpA05)),((_tmpA05[0]=((_tmpA06.tag=3,((_tmpA06.f1=& Cyc_tc_r,_tmpA06)))),_tmpA05)))),((_tmpA09->f5=(
(_tmpA04="Stop after type checking",_tag_dyneither(_tmpA04,sizeof(char),25))),_tmpA09)))))))))))),((_tmp8E9[31]=(
# 1239
(_tmpA0F=_region_malloc(r,sizeof(*_tmpA0F)),((_tmpA0F->f1=((_tmpA0E="-stopafter-parse",_tag_dyneither(_tmpA0E,sizeof(char),17))),((_tmpA0F->f2=0,((_tmpA0F->f3=((_tmpA0D="",_tag_dyneither(_tmpA0D,sizeof(char),1))),((_tmpA0F->f4=(void*)(
(_tmpA0B=_region_malloc(r,sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0C.tag=3,((_tmpA0C.f1=& Cyc_parseonly_r,_tmpA0C)))),_tmpA0B)))),((_tmpA0F->f5=(
(_tmpA0A="Stop after parsing",_tag_dyneither(_tmpA0A,sizeof(char),19))),_tmpA0F)))))))))))),((_tmp8E9[30]=(
# 1236
(_tmpA15=_region_malloc(r,sizeof(*_tmpA15)),((_tmpA15->f1=((_tmpA14="-E",_tag_dyneither(_tmpA14,sizeof(char),3))),((_tmpA15->f2=0,((_tmpA15->f3=((_tmpA13="",_tag_dyneither(_tmpA13,sizeof(char),1))),((_tmpA15->f4=(void*)(
(_tmpA11=_region_malloc(r,sizeof(*_tmpA11)),((_tmpA11[0]=((_tmpA12.tag=3,((_tmpA12.f1=& Cyc_stop_after_cpp_r,_tmpA12)))),_tmpA11)))),((_tmpA15->f5=(
(_tmpA10="Stop after preprocessing",_tag_dyneither(_tmpA10,sizeof(char),25))),_tmpA15)))))))))))),((_tmp8E9[29]=(
# 1233
(_tmpA1B=_region_malloc(r,sizeof(*_tmpA1B)),((_tmpA1B->f1=((_tmpA1A="-Wall",_tag_dyneither(_tmpA1A,sizeof(char),6))),((_tmpA1B->f2=0,((_tmpA1B->f3=((_tmpA19="",_tag_dyneither(_tmpA19,sizeof(char),1))),((_tmpA1B->f4=(void*)(
(_tmpA17=_region_malloc(r,sizeof(*_tmpA17)),((_tmpA17[0]=((_tmpA18.tag=0,((_tmpA18.f1=Cyc_set_all_warnings,_tmpA18)))),_tmpA17)))),((_tmpA1B->f5=(
(_tmpA16="Turn on all warnings",_tag_dyneither(_tmpA16,sizeof(char),21))),_tmpA1B)))))))))))),((_tmp8E9[28]=(
# 1230
(_tmpA21=_region_malloc(r,sizeof(*_tmpA21)),((_tmpA21->f1=((_tmpA20="-Woverride",_tag_dyneither(_tmpA20,sizeof(char),11))),((_tmpA21->f2=0,((_tmpA21->f3=((_tmpA1F="",_tag_dyneither(_tmpA1F,sizeof(char),1))),((_tmpA21->f4=(void*)(
(_tmpA1D=_region_malloc(r,sizeof(*_tmpA1D)),((_tmpA1D[0]=((_tmpA1E.tag=3,((_tmpA1E.f1=& Cyc_Tcenv_warn_override,_tmpA1E)))),_tmpA1D)))),((_tmpA21->f5=(
(_tmpA1C="Warn when a local variable overrides an existing variable",_tag_dyneither(_tmpA1C,sizeof(char),58))),_tmpA21)))))))))))),((_tmp8E9[27]=(
# 1227
(_tmpA27=_region_malloc(r,sizeof(*_tmpA27)),((_tmpA27->f1=((_tmpA26="-Wlose-unique",_tag_dyneither(_tmpA26,sizeof(char),14))),((_tmpA27->f2=0,((_tmpA27->f3=((_tmpA25="",_tag_dyneither(_tmpA25,sizeof(char),1))),((_tmpA27->f4=(void*)(
(_tmpA23=_region_malloc(r,sizeof(*_tmpA23)),((_tmpA23[0]=((_tmpA24.tag=3,((_tmpA24.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmpA24)))),_tmpA23)))),((_tmpA27->f5=(
(_tmpA22="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmpA22,sizeof(char),49))),_tmpA27)))))))))))),((_tmp8E9[26]=(
# 1223
(_tmpA2D=_region_malloc(r,sizeof(*_tmpA2D)),((_tmpA2D->f1=((_tmpA2C="-b",_tag_dyneither(_tmpA2C,sizeof(char),3))),((_tmpA2D->f2=0,((_tmpA2D->f3=((_tmpA2B="<machine>",_tag_dyneither(_tmpA2B,sizeof(char),10))),((_tmpA2D->f4=(void*)(
(_tmpA29=_region_malloc(r,sizeof(*_tmpA29)),((_tmpA29[0]=((_tmpA2A.tag=5,((_tmpA2A.f1=Cyc_set_target_arch,_tmpA2A)))),_tmpA29)))),((_tmpA2D->f5=(
# 1226
(_tmpA28="Set target machine",_tag_dyneither(_tmpA28,sizeof(char),19))),_tmpA2D)))))))))))),((_tmp8E9[25]=(
# 1220
(_tmpA33=_region_malloc(r,sizeof(*_tmpA33)),((_tmpA33->f1=((_tmpA32="-MT",_tag_dyneither(_tmpA32,sizeof(char),4))),((_tmpA33->f2=0,((_tmpA33->f3=((_tmpA31=" <target>",_tag_dyneither(_tmpA31,sizeof(char),10))),((_tmpA33->f4=(void*)(
(_tmpA2F=_region_malloc(r,sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA30.tag=5,((_tmpA30.f1=Cyc_set_dependencies_target,_tmpA30)))),_tmpA2F)))),((_tmpA33->f5=(
(_tmpA2E="Give target for dependencies",_tag_dyneither(_tmpA2E,sizeof(char),29))),_tmpA33)))))))))))),((_tmp8E9[24]=(
# 1216
(_tmpA39=_region_malloc(r,sizeof(*_tmpA39)),((_tmpA39->f1=((_tmpA38="-MG",_tag_dyneither(_tmpA38,sizeof(char),4))),((_tmpA39->f2=0,((_tmpA39->f3=((_tmpA37="",_tag_dyneither(_tmpA37,sizeof(char),1))),((_tmpA39->f4=(void*)(
(_tmpA35=_region_malloc(r,sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=1,((_tmpA36.f1=Cyc_add_cpparg,_tmpA36)))),_tmpA35)))),((_tmpA39->f5=(
# 1219
(_tmpA34="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmpA34,sizeof(char),68))),_tmpA39)))))))))))),((_tmp8E9[23]=(
# 1213
(_tmpA3F=_region_malloc(r,sizeof(*_tmpA3F)),((_tmpA3F->f1=((_tmpA3E="-M",_tag_dyneither(_tmpA3E,sizeof(char),3))),((_tmpA3F->f2=0,((_tmpA3F->f3=((_tmpA3D="",_tag_dyneither(_tmpA3D,sizeof(char),1))),((_tmpA3F->f4=(void*)(
(_tmpA3B=_region_malloc(r,sizeof(*_tmpA3B)),((_tmpA3B[0]=((_tmpA3C.tag=0,((_tmpA3C.f1=Cyc_set_produce_dependencies,_tmpA3C)))),_tmpA3B)))),((_tmpA3F->f5=(
(_tmpA3A="Produce dependencies",_tag_dyneither(_tmpA3A,sizeof(char),21))),_tmpA3F)))))))))))),((_tmp8E9[22]=(
# 1210
(_tmpA45=_region_malloc(r,sizeof(*_tmpA45)),((_tmpA45->f1=((_tmpA44="-S",_tag_dyneither(_tmpA44,sizeof(char),3))),((_tmpA45->f2=0,((_tmpA45->f3=((_tmpA43="",_tag_dyneither(_tmpA43,sizeof(char),1))),((_tmpA45->f4=(void*)(
(_tmpA41=_region_malloc(r,sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=0,((_tmpA42.f1=Cyc_set_stop_after_asmfile,_tmpA42)))),_tmpA41)))),((_tmpA45->f5=(
(_tmpA40="Stop after producing assembly code",_tag_dyneither(_tmpA40,sizeof(char),35))),_tmpA45)))))))))))),((_tmp8E9[21]=(
# 1207
(_tmpA4B=_region_malloc(r,sizeof(*_tmpA4B)),((_tmpA4B->f1=((_tmpA4A="-pa",_tag_dyneither(_tmpA4A,sizeof(char),4))),((_tmpA4B->f2=0,((_tmpA4B->f3=((_tmpA49="",_tag_dyneither(_tmpA49,sizeof(char),1))),((_tmpA4B->f4=(void*)(
(_tmpA47=_region_malloc(r,sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=0,((_tmpA48.f1=Cyc_set_pa,_tmpA48)))),_tmpA47)))),((_tmpA4B->f5=(
(_tmpA46="Compile for profiling with aprof",_tag_dyneither(_tmpA46,sizeof(char),33))),_tmpA4B)))))))))))),((_tmp8E9[20]=(
# 1204
(_tmpA51=_region_malloc(r,sizeof(*_tmpA51)),((_tmpA51->f1=((_tmpA50="-pg",_tag_dyneither(_tmpA50,sizeof(char),4))),((_tmpA51->f2=0,((_tmpA51->f3=((_tmpA4F="",_tag_dyneither(_tmpA4F,sizeof(char),1))),((_tmpA51->f4=(void*)(
(_tmpA4D=_region_malloc(r,sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=0,((_tmpA4E.f1=Cyc_set_pg,_tmpA4E)))),_tmpA4D)))),((_tmpA51->f5=(
(_tmpA4C="Compile for profiling with gprof",_tag_dyneither(_tmpA4C,sizeof(char),33))),_tmpA51)))))))))))),((_tmp8E9[19]=(
# 1201
(_tmpA57=_region_malloc(r,sizeof(*_tmpA57)),((_tmpA57->f1=((_tmpA56="-p",_tag_dyneither(_tmpA56,sizeof(char),3))),((_tmpA57->f2=0,((_tmpA57->f3=((_tmpA55="",_tag_dyneither(_tmpA55,sizeof(char),1))),((_tmpA57->f4=(void*)(
(_tmpA53=_region_malloc(r,sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=1,((_tmpA54.f1=Cyc_add_ccarg,_tmpA54)))),_tmpA53)))),((_tmpA57->f5=(
(_tmpA52="Compile for profiling with prof",_tag_dyneither(_tmpA52,sizeof(char),32))),_tmpA57)))))))))))),((_tmp8E9[18]=(
# 1198
(_tmpA5D=_region_malloc(r,sizeof(*_tmpA5D)),((_tmpA5D->f1=((_tmpA5C="-g",_tag_dyneither(_tmpA5C,sizeof(char),3))),((_tmpA5D->f2=0,((_tmpA5D->f3=((_tmpA5B="",_tag_dyneither(_tmpA5B,sizeof(char),1))),((_tmpA5D->f4=(void*)(
(_tmpA59=_region_malloc(r,sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=1,((_tmpA5A.f1=Cyc_add_ccarg,_tmpA5A)))),_tmpA59)))),((_tmpA5D->f5=(
(_tmpA58="Compile for debugging",_tag_dyneither(_tmpA58,sizeof(char),22))),_tmpA5D)))))))))))),((_tmp8E9[17]=(
# 1195
(_tmpA63=_region_malloc(r,sizeof(*_tmpA63)),((_tmpA63->f1=((_tmpA62="-fno-builtin",_tag_dyneither(_tmpA62,sizeof(char),13))),((_tmpA63->f2=0,((_tmpA63->f3=((_tmpA61="",_tag_dyneither(_tmpA61,sizeof(char),1))),((_tmpA63->f4=(void*)(
(_tmpA5F=_region_malloc(r,sizeof(*_tmpA5F)),((_tmpA5F[0]=((_tmpA60.tag=1,((_tmpA60.f1=Cyc_add_cpp_and_ccarg,_tmpA60)))),_tmpA5F)))),((_tmpA63->f5=(
(_tmpA5E="Suppress builtin code sequence expansions (gcc option)",_tag_dyneither(_tmpA5E,sizeof(char),55))),_tmpA63)))))))))))),((_tmp8E9[16]=(
# 1192
(_tmpA69=_region_malloc(r,sizeof(*_tmpA69)),((_tmpA69->f1=((_tmpA68="-fomit-frame-pointer",_tag_dyneither(_tmpA68,sizeof(char),21))),((_tmpA69->f2=0,((_tmpA69->f3=((_tmpA67="",_tag_dyneither(_tmpA67,sizeof(char),1))),((_tmpA69->f4=(void*)(
(_tmpA65=_region_malloc(r,sizeof(*_tmpA65)),((_tmpA65[0]=((_tmpA66.tag=1,((_tmpA66.f1=Cyc_add_ccarg,_tmpA66)))),_tmpA65)))),((_tmpA69->f5=(
(_tmpA64="Omit frame pointer (gcc option)",_tag_dyneither(_tmpA64,sizeof(char),32))),_tmpA69)))))))))))),((_tmp8E9[15]=(
# 1189
(_tmpA6F=_region_malloc(r,sizeof(*_tmpA6F)),((_tmpA6F->f1=((_tmpA6E="-O3",_tag_dyneither(_tmpA6E,sizeof(char),4))),((_tmpA6F->f2=0,((_tmpA6F->f3=((_tmpA6D="",_tag_dyneither(_tmpA6D,sizeof(char),1))),((_tmpA6F->f4=(void*)(
(_tmpA6B=_region_malloc(r,sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6C.tag=1,((_tmpA6C.f1=Cyc_add_ccarg,_tmpA6C)))),_tmpA6B)))),((_tmpA6F->f5=(
(_tmpA6A="Even more optimization",_tag_dyneither(_tmpA6A,sizeof(char),23))),_tmpA6F)))))))))))),((_tmp8E9[14]=(
# 1186
(_tmpA75=_region_malloc(r,sizeof(*_tmpA75)),((_tmpA75->f1=((_tmpA74="-O2",_tag_dyneither(_tmpA74,sizeof(char),4))),((_tmpA75->f2=0,((_tmpA75->f3=((_tmpA73="",_tag_dyneither(_tmpA73,sizeof(char),1))),((_tmpA75->f4=(void*)(
(_tmpA71=_region_malloc(r,sizeof(*_tmpA71)),((_tmpA71[0]=((_tmpA72.tag=1,((_tmpA72.f1=Cyc_add_ccarg,_tmpA72)))),_tmpA71)))),((_tmpA75->f5=(
(_tmpA70="A higher level of optimization",_tag_dyneither(_tmpA70,sizeof(char),31))),_tmpA75)))))))))))),((_tmp8E9[13]=(
# 1183
(_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B->f1=((_tmpA7A="-O",_tag_dyneither(_tmpA7A,sizeof(char),3))),((_tmpA7B->f2=0,((_tmpA7B->f3=((_tmpA79="",_tag_dyneither(_tmpA79,sizeof(char),1))),((_tmpA7B->f4=(void*)(
(_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=1,((_tmpA78.f1=Cyc_add_ccarg,_tmpA78)))),_tmpA77)))),((_tmpA7B->f5=(
(_tmpA76="Optimize",_tag_dyneither(_tmpA76,sizeof(char),9))),_tmpA7B)))))))))))),((_tmp8E9[12]=(
# 1180
(_tmpA81=_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81->f1=((_tmpA80="-O0",_tag_dyneither(_tmpA80,sizeof(char),4))),((_tmpA81->f2=0,((_tmpA81->f3=((_tmpA7F="",_tag_dyneither(_tmpA7F,sizeof(char),1))),((_tmpA81->f4=(void*)(
(_tmpA7D=_region_malloc(r,sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7E.tag=1,((_tmpA7E.f1=Cyc_add_ccarg,_tmpA7E)))),_tmpA7D)))),((_tmpA81->f5=(
(_tmpA7C="Don't optimize",_tag_dyneither(_tmpA7C,sizeof(char),15))),_tmpA81)))))))))))),((_tmp8E9[11]=(
# 1177
(_tmpA87=_region_malloc(r,sizeof(*_tmpA87)),((_tmpA87->f1=((_tmpA86="-s",_tag_dyneither(_tmpA86,sizeof(char),3))),((_tmpA87->f2=0,((_tmpA87->f3=((_tmpA85="",_tag_dyneither(_tmpA85,sizeof(char),1))),((_tmpA87->f4=(void*)(
(_tmpA83=_region_malloc(r,sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA84.tag=1,((_tmpA84.f1=Cyc_add_ccarg,_tmpA84)))),_tmpA83)))),((_tmpA87->f5=(
(_tmpA82="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA82,sizeof(char),60))),_tmpA87)))))))))))),((_tmp8E9[10]=(
# 1174
(_tmpA8D=_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D->f1=((_tmpA8C="-x",_tag_dyneither(_tmpA8C,sizeof(char),3))),((_tmpA8D->f2=0,((_tmpA8D->f3=((_tmpA8B=" <language>",_tag_dyneither(_tmpA8B,sizeof(char),12))),((_tmpA8D->f4=(void*)(
(_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA8A.tag=5,((_tmpA8A.f1=Cyc_set_inputtype,_tmpA8A)))),_tmpA89)))),((_tmpA8D->f5=(
(_tmpA88="Specify <language> for the following input files",_tag_dyneither(_tmpA88,sizeof(char),49))),_tmpA8D)))))))))))),((_tmp8E9[9]=(
# 1171
(_tmpA93=_region_malloc(r,sizeof(*_tmpA93)),((_tmpA93->f1=((_tmpA92="-c",_tag_dyneither(_tmpA92,sizeof(char),3))),((_tmpA93->f2=0,((_tmpA93->f3=((_tmpA91="",_tag_dyneither(_tmpA91,sizeof(char),1))),((_tmpA93->f4=(void*)(
(_tmpA8F=_region_malloc(r,sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA90.tag=0,((_tmpA90.f1=Cyc_set_stop_after_objectfile,_tmpA90)))),_tmpA8F)))),((_tmpA93->f5=(
(_tmpA8E="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA8E,sizeof(char),61))),_tmpA93)))))))))))),((_tmp8E9[8]=(
# 1168
(_tmpA99=_region_malloc(r,sizeof(*_tmpA99)),((_tmpA99->f1=((_tmpA98="-m",_tag_dyneither(_tmpA98,sizeof(char),3))),((_tmpA99->f2=1,((_tmpA99->f3=((_tmpA97="<option>",_tag_dyneither(_tmpA97,sizeof(char),9))),((_tmpA99->f4=(void*)(
(_tmpA95=_region_malloc(r,sizeof(*_tmpA95)),((_tmpA95[0]=((_tmpA96.tag=1,((_tmpA96.f1=Cyc_add_marg,_tmpA96)))),_tmpA95)))),((_tmpA99->f5=(
(_tmpA94="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA94,sizeof(char),52))),_tmpA99)))))))))))),((_tmp8E9[7]=(
# 1165
(_tmpA9F=_region_malloc(r,sizeof(*_tmpA9F)),((_tmpA9F->f1=((_tmpA9E="-l",_tag_dyneither(_tmpA9E,sizeof(char),3))),((_tmpA9F->f2=1,((_tmpA9F->f3=((_tmpA9D="<libname>",_tag_dyneither(_tmpA9D,sizeof(char),10))),((_tmpA9F->f4=(void*)(
(_tmpA9B=_region_malloc(r,sizeof(*_tmpA9B)),((_tmpA9B[0]=((_tmpA9C.tag=1,((_tmpA9C.f1=Cyc_add_libarg,_tmpA9C)))),_tmpA9B)))),((_tmpA9F->f5=(
(_tmpA9A="Library file",_tag_dyneither(_tmpA9A,sizeof(char),13))),_tmpA9F)))))))))))),((_tmp8E9[6]=(
# 1162
(_tmpAA5=_region_malloc(r,sizeof(*_tmpAA5)),((_tmpAA5->f1=((_tmpAA4="-L",_tag_dyneither(_tmpAA4,sizeof(char),3))),((_tmpAA5->f2=1,((_tmpAA5->f3=((_tmpAA3="<dir>",_tag_dyneither(_tmpAA3,sizeof(char),6))),((_tmpAA5->f4=(void*)(
(_tmpAA1=_region_malloc(r,sizeof(*_tmpAA1)),((_tmpAA1[0]=((_tmpAA2.tag=1,((_tmpAA2.f1=Cyc_add_ccarg,_tmpAA2)))),_tmpAA1)))),((_tmpAA5->f5=(
(_tmpAA0="Add to the list of directories for -l",_tag_dyneither(_tmpAA0,sizeof(char),38))),_tmpAA5)))))))))))),((_tmp8E9[5]=(
# 1159
(_tmpAAB=_region_malloc(r,sizeof(*_tmpAAB)),((_tmpAAB->f1=((_tmpAAA="-I",_tag_dyneither(_tmpAAA,sizeof(char),3))),((_tmpAAB->f2=1,((_tmpAAB->f3=((_tmpAA9="<dir>",_tag_dyneither(_tmpAA9,sizeof(char),6))),((_tmpAAB->f4=(void*)(
(_tmpAA7=_region_malloc(r,sizeof(*_tmpAA7)),((_tmpAA7[0]=((_tmpAA8.tag=1,((_tmpAA8.f1=Cyc_add_cpparg,_tmpAA8)))),_tmpAA7)))),((_tmpAAB->f5=(
(_tmpAA6="Add to the list of directories to search for include files",_tag_dyneither(_tmpAA6,sizeof(char),59))),_tmpAAB)))))))))))),((_tmp8E9[4]=(
# 1156
(_tmpAB1=_region_malloc(r,sizeof(*_tmpAB1)),((_tmpAB1->f1=((_tmpAB0="-B",_tag_dyneither(_tmpAB0,sizeof(char),3))),((_tmpAB1->f2=1,((_tmpAB1->f3=((_tmpAAF="<file>",_tag_dyneither(_tmpAAF,sizeof(char),7))),((_tmpAB1->f4=(void*)(
(_tmpAAD=_region_malloc(r,sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAE.tag=1,((_tmpAAE.f1=Cyc_add_cyclone_exec_path,_tmpAAE)))),_tmpAAD)))),((_tmpAB1->f5=(
(_tmpAAC="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpAAC,sizeof(char),60))),_tmpAB1)))))))))))),((_tmp8E9[3]=(
# 1153
(_tmpAB7=_region_malloc(r,sizeof(*_tmpAB7)),((_tmpAB7->f1=((_tmpAB6="-D",_tag_dyneither(_tmpAB6,sizeof(char),3))),((_tmpAB7->f2=1,((_tmpAB7->f3=((_tmpAB5="<name>[=<value>]",_tag_dyneither(_tmpAB5,sizeof(char),17))),((_tmpAB7->f4=(void*)(
(_tmpAB3=_region_malloc(r,sizeof(*_tmpAB3)),((_tmpAB3[0]=((_tmpAB4.tag=1,((_tmpAB4.f1=Cyc_add_cpparg,_tmpAB4)))),_tmpAB3)))),((_tmpAB7->f5=(
(_tmpAB2="Pass definition to preprocessor",_tag_dyneither(_tmpAB2,sizeof(char),32))),_tmpAB7)))))))))))),((_tmp8E9[2]=(
# 1150
(_tmpABD=_region_malloc(r,sizeof(*_tmpABD)),((_tmpABD->f1=((_tmpABC="-o",_tag_dyneither(_tmpABC,sizeof(char),3))),((_tmpABD->f2=0,((_tmpABD->f3=((_tmpABB=" <file>",_tag_dyneither(_tmpABB,sizeof(char),8))),((_tmpABD->f4=(void*)(
(_tmpAB9=_region_malloc(r,sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABA.tag=5,((_tmpABA.f1=Cyc_set_output_file,_tmpABA)))),_tmpAB9)))),((_tmpABD->f5=(
(_tmpAB8="Set the output file name to <file>",_tag_dyneither(_tmpAB8,sizeof(char),35))),_tmpABD)))))))))))),((_tmp8E9[1]=(
# 1147
(_tmpAC3=_region_malloc(r,sizeof(*_tmpAC3)),((_tmpAC3->f1=((_tmpAC2="--version",_tag_dyneither(_tmpAC2,sizeof(char),10))),((_tmpAC3->f2=0,((_tmpAC3->f3=((_tmpAC1="",_tag_dyneither(_tmpAC1,sizeof(char),1))),((_tmpAC3->f4=(void*)(
(_tmpABF=_region_malloc(r,sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpAC0.tag=0,((_tmpAC0.f1=Cyc_print_version,_tmpAC0)))),_tmpABF)))),((_tmpAC3->f5=(
(_tmpABE="Print version information and exit",_tag_dyneither(_tmpABE,sizeof(char),35))),_tmpAC3)))))))))))),((_tmp8E9[0]=(
# 1144
(_tmpAC9=_region_malloc(r,sizeof(*_tmpAC9)),((_tmpAC9->f1=((_tmpAC8="-v",_tag_dyneither(_tmpAC8,sizeof(char),3))),((_tmpAC9->f2=0,((_tmpAC9->f3=((_tmpAC7="",_tag_dyneither(_tmpAC7,sizeof(char),1))),((_tmpAC9->f4=(void*)(
(_tmpAC5=_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5[0]=((_tmpAC6.tag=3,((_tmpAC6.f1=& Cyc_v_r,_tmpAC6)))),_tmpAC5)))),((_tmpAC9->f5=(
(_tmpAC4="Print compilation stages verbosely",_tag_dyneither(_tmpAC4,sizeof(char),35))),_tmpAC9)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp8E9,sizeof(struct _tuple24*),80)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1389
const char*_tmpACA;struct _dyneither_ptr optstring=(_tmpACA="Options:",_tag_dyneither(_tmpACA,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp166);
# 1394
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1402
char*_tmpACB;struct _dyneither_ptr cyclone_exec_prefix=(_tmpACB=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpACB,sizeof(char),_get_zero_arr_size_char((void*)_tmpACB,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmpACE;struct Cyc_List_List*_tmpACD;Cyc_cyclone_exec_path=((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->hd=((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=cyclone_exec_prefix,_tmpACE)))),((_tmpACD->tl=Cyc_cyclone_exec_path,_tmpACD))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmpAD4;const char*_tmpAD3;struct Cyc_List_List*_tmpAD2;Cyc_cyclone_exec_path=(
(_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->hd=((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpAD3="cyc-lib",_tag_dyneither(_tmpAD3,sizeof(char),8)))),_tmpAD4)))),((_tmpAD2->tl=Cyc_cyclone_exec_path,_tmpAD2))))));}
# 1410
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1418
const char*_tmpAD5;struct _dyneither_ptr _tmp16D=Cyc_do_find(cyclone_arch_path,((_tmpAD5="cycspecs",_tag_dyneither(_tmpAD5,sizeof(char),9))));
if(Cyc_v_r){const char*_tmpAD9;void*_tmpAD8[1];struct Cyc_String_pa_PrintArg_struct _tmpAD7;(_tmpAD7.tag=0,((_tmpAD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16D),((_tmpAD8[0]=& _tmpAD7,Cyc_fprintf(Cyc_stderr,((_tmpAD9="Reading from specs file %s\n",_tag_dyneither(_tmpAD9,sizeof(char),28))),_tag_dyneither(_tmpAD8,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp171=Cyc_read_specs(_tmp16D);
struct _dyneither_ptr _tmp172=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
{struct _dyneither_ptr _tmp176=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp171,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp172=Cyc_split_specs(_tmp176);}
# 1423
;_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp179=_tmp174;void*_tmp17B;_LL34: {struct Cyc_Core_Not_found_exn_struct*_tmp17A=(struct Cyc_Core_Not_found_exn_struct*)_tmp179;if(_tmp17A->tag != Cyc_Core_Not_found)goto _LL36;}_LL35:
# 1426
 goto _LL33;_LL36: _tmp17B=_tmp179;_LL37:(void)_rethrow(_tmp17B);_LL33:;}};}
# 1428
if((struct _dyneither_ptr*)_tmp172.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp172);}
# 1433
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp167);{
const char*_tmpADA;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpADA="",_tag_dyneither(_tmpADA,sizeof(char),1))))== 0){
# 1451 "cyclone.cyc"
const char*_tmp17D=Cyc_produce_dependencies?"":" -E";
const char*_tmpAE2;void*_tmpAE1[3];struct Cyc_String_pa_PrintArg_struct _tmpAE0;const char*_tmpADF;struct Cyc_String_pa_PrintArg_struct _tmpADE;struct Cyc_String_pa_PrintArg_struct _tmpADD;Cyc_set_cpp((struct _dyneither_ptr)((_tmpADD.tag=0,((_tmpADD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16D),((_tmpADE.tag=0,((_tmpADE.f1=(struct _dyneither_ptr)((_tmpADF=_tmp17D,_tag_dyneither(_tmpADF,sizeof(char),_get_zero_arr_size_char((void*)_tmpADF,1)))),((_tmpAE0.tag=0,((_tmpAE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAE1[0]=& _tmpAE0,((_tmpAE1[1]=& _tmpADE,((_tmpAE1[2]=& _tmpADD,Cyc_aprintf(((_tmpAE2="%s -w -x c%s -specs %s",_tag_dyneither(_tmpAE2,sizeof(char),23))),_tag_dyneither(_tmpAE1,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1455
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpAE5;void*_tmpAE4;(_tmpAE4=0,Cyc_fprintf(Cyc_stderr,((_tmpAE5="missing file\n",_tag_dyneither(_tmpAE5,sizeof(char),14))),_tag_dyneither(_tmpAE4,sizeof(void*),0)));}
exit(1);}
# 1464
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpAE6;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpAE6="cyc_setjmp.h",_tag_dyneither(_tmpAE6,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpAE7;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpAE7="cyc_include.h",_tag_dyneither(_tmpAE7,sizeof(char),14))));}
# 1470
{struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp378 != 0;_tmp378=_tmp378->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp378->hd));
if(Cyc_compile_failure){int _tmp379=1;_npop_handler(0);return _tmp379;}}}
# 1475
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp37A=0;_npop_handler(0);return _tmp37A;}
# 1481
if(Cyc_ccargs == 0){int _tmp37B=0;_npop_handler(0);return _tmp37B;}
# 1484
{const char*_tmpAE8;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpAE8="-L",_tag_dyneither(_tmpAE8,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1486
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpAE9;struct _dyneither_ptr _tmp37D=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpAE9=" ",_tag_dyneither(_tmpAE9,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpAEB;struct Cyc_List_List*_tmpAEA;struct _dyneither_ptr _tmp37E=Cyc_str_sepstr(((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpAEA->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpAEA)))))),((_tmpAEB=" ",_tag_dyneither(_tmpAEB,sizeof(char),2))));
# 1492
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpAED;const char*_tmpAEC;int _tmp37F=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1497
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAED=".a",_tag_dyneither(_tmpAED,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAEC=".lib",_tag_dyneither(_tmpAEC,sizeof(char),5))));
if(_tmp37F){
stdlib=0;{
const char*_tmpAEE;stdlib_string=(struct _dyneither_ptr)((_tmpAEE="",_tag_dyneither(_tmpAEE,sizeof(char),1)));};}else{
# 1505
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpAEF;libcyc_flag=((_tmpAEF="-lcyc_a",_tag_dyneither(_tmpAEF,sizeof(char),8)));}
{const char*_tmpAF0;nogc_filename=((_tmpAF0="nogc_a.a",_tag_dyneither(_tmpAF0,sizeof(char),9)));}{
const char*_tmpAF1;runtime_filename=((_tmpAF1="runtime_cyc_a.o",_tag_dyneither(_tmpAF1,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpAF2;libcyc_flag=((_tmpAF2="-lcyc_nocheck",_tag_dyneither(_tmpAF2,sizeof(char),14)));}
{const char*_tmpAF3;nogc_filename=((_tmpAF3="nogc.a",_tag_dyneither(_tmpAF3,sizeof(char),7)));}{
const char*_tmpAF4;runtime_filename=((_tmpAF4="runtime_cyc.o",_tag_dyneither(_tmpAF4,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpAF5;libcyc_flag=((_tmpAF5="-lcyc_pg",_tag_dyneither(_tmpAF5,sizeof(char),9)));}
{const char*_tmpAF6;runtime_filename=((_tmpAF6="runtime_cyc_pg.o",_tag_dyneither(_tmpAF6,sizeof(char),17)));}{
const char*_tmpAF7;nogc_filename=((_tmpAF7="nogc_pg.a",_tag_dyneither(_tmpAF7,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1523
{const char*_tmpAF8;libcyc_flag=((_tmpAF8="-lcycboot",_tag_dyneither(_tmpAF8,sizeof(char),10)));}
{const char*_tmpAF9;nogc_filename=((_tmpAF9="nogc.a",_tag_dyneither(_tmpAF9,sizeof(char),7)));}{
const char*_tmpAFA;runtime_filename=((_tmpAFA="runtime_cyc.o",_tag_dyneither(_tmpAFA,sizeof(char),14)));};}else{
# 1527
{const char*_tmpAFB;libcyc_flag=((_tmpAFB="-lcyc",_tag_dyneither(_tmpAFB,sizeof(char),6)));}
{const char*_tmpAFC;nogc_filename=((_tmpAFC="nogc.a",_tag_dyneither(_tmpAFC,sizeof(char),7)));}{
const char*_tmpAFD;runtime_filename=((_tmpAFD="runtime_cyc.o",_tag_dyneither(_tmpAFD,sizeof(char),14)));};}}}}
# 1531
{const char*_tmpAFE;gc_filename=((_tmpAFE="gc.a",_tag_dyneither(_tmpAFE,sizeof(char),5)));}{
# 1533
struct _dyneither_ptr _tmp391=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp392=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1540
stdlib=0;{
const char*_tmpB04;void*_tmpB03[3];struct Cyc_String_pa_PrintArg_struct _tmpB02;struct Cyc_String_pa_PrintArg_struct _tmpB01;struct Cyc_String_pa_PrintArg_struct _tmpB00;stdlib_string=(struct _dyneither_ptr)((_tmpB00.tag=0,((_tmpB00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp391),((_tmpB01.tag=0,((_tmpB01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpB02.tag=0,((_tmpB02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp392),((_tmpB03[0]=& _tmpB02,((_tmpB03[1]=& _tmpB01,((_tmpB03[2]=& _tmpB00,Cyc_aprintf(((_tmpB04=" %s %s %s",_tag_dyneither(_tmpB04,sizeof(char),10))),_tag_dyneither(_tmpB03,sizeof(void*),3))))))))))))))))))));};};}
# 1544
if(Cyc_ic_r){struct _handler_cons _tmp398;_push_handler(& _tmp398);{int _tmp39A=0;if(setjmp(_tmp398.handler))_tmp39A=1;if(!_tmp39A){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp37F){
struct Cyc_List_List*_tmpB05;_tmp39B=((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->hd=Cyc_final_strptr,((_tmpB05->tl=_tmp39B,_tmpB05))))));}{
# 1552
struct Cyc_Interface_I*_tmp39D=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp39B,_tmp39B);
if(_tmp39D == 0){
{const char*_tmpB08;void*_tmpB07;(_tmpB07=0,Cyc_fprintf(Cyc_stderr,((_tmpB08="Error: interfaces incompatible\n",_tag_dyneither(_tmpB08,sizeof(char),32))),_tag_dyneither(_tmpB07,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3A0=1;_npop_handler(1);return _tmp3A0;};}
# 1559
if(_tmp37F){
if(Cyc_output_file != 0){
const char*_tmpB0C;void*_tmpB0B[1];struct Cyc_String_pa_PrintArg_struct _tmpB0A;struct _dyneither_ptr _tmp3A1=(_tmpB0A.tag=0,((_tmpB0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpB0B[0]=& _tmpB0A,Cyc_aprintf(((_tmpB0C="%s.cycio",_tag_dyneither(_tmpB0C,sizeof(char),9))),_tag_dyneither(_tmpB0B,sizeof(void*),1)))))));
const char*_tmpB0E;const char*_tmpB0D;struct Cyc___cycFILE*_tmp3A2=Cyc_try_file_open((struct _dyneither_ptr)_tmp3A1,((_tmpB0D="wb",_tag_dyneither(_tmpB0D,sizeof(char),3))),((_tmpB0E="interface object file",_tag_dyneither(_tmpB0E,sizeof(char),22))));
if(_tmp3A2 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3A3=1;_npop_handler(1);return _tmp3A3;};}
# 1568
Cyc_Interface_save(_tmp39D,_tmp3A2);
Cyc_file_close(_tmp3A2);}}else{
# 1572
const char*_tmpB13;const char*_tmpB12;struct _tuple20*_tmpB11;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp39D,(
(_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11->f1=((_tmpB12="empty interface",_tag_dyneither(_tmpB12,sizeof(char),16))),((_tmpB11->f2=((_tmpB13="global interface",_tag_dyneither(_tmpB13,sizeof(char),17))),_tmpB11)))))))){
{const char*_tmpB16;void*_tmpB15;(_tmpB15=0,Cyc_fprintf(Cyc_stderr,((_tmpB16="Error: some objects are still undefined\n",_tag_dyneither(_tmpB16,sizeof(char),41))),_tag_dyneither(_tmpB15,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3AE=1;_npop_handler(1);return _tmp3AE;};}}};};
# 1545
;_pop_handler();}else{void*_tmp399=(void*)_exn_thrown;void*_tmp3B0=_tmp399;void*_tmp3B1;void*_tmp3B2;_LL39: _tmp3B1=_tmp3B0;_LL3A:
# 1582
{const char*_tmpB20;void*_tmpB1F[3];const char*_tmpB1E;struct Cyc_String_pa_PrintArg_struct _tmpB1D;const char*_tmpB1C;struct Cyc_String_pa_PrintArg_struct _tmpB1B;struct Cyc_Int_pa_PrintArg_struct _tmpB1A;(_tmpB1A.tag=1,((_tmpB1A.f1=(unsigned long)Cyc_Core_get_exn_lineno(),((_tmpB1B.tag=0,((_tmpB1B.f1=(struct _dyneither_ptr)((_tmpB1C=Cyc_Core_get_exn_filename(),_tag_dyneither(_tmpB1C,sizeof(char),_get_zero_arr_size_char((void*)_tmpB1C,1)))),((_tmpB1D.tag=0,((_tmpB1D.f1=(struct _dyneither_ptr)((_tmpB1E=Cyc_Core_get_exn_name(_tmp3B1),_tag_dyneither(_tmpB1E,sizeof(char),_get_zero_arr_size_char((void*)_tmpB1E,1)))),((_tmpB1F[0]=& _tmpB1D,((_tmpB1F[1]=& _tmpB1B,((_tmpB1F[2]=& _tmpB1A,Cyc_fprintf(Cyc_stderr,((_tmpB20="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",_tag_dyneither(_tmpB20,sizeof(char),131))),_tag_dyneither(_tmpB1F,sizeof(void*),3)))))))))))))))))));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3BA=1;_npop_handler(0);return _tmp3BA;};_LL3B: _tmp3B2=_tmp3B0;_LL3C:(void)_rethrow(_tmp3B2);_LL38:;}};}{
# 1588
const char*_tmpB21;struct _dyneither_ptr outfile_str=(_tmpB21="",_tag_dyneither(_tmpB21,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpB25;void*_tmpB24[1];struct Cyc_String_pa_PrintArg_struct _tmpB23;outfile_str=(struct _dyneither_ptr)((_tmpB23.tag=0,((_tmpB23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpB24[0]=& _tmpB23,Cyc_aprintf(((_tmpB25=" -o %s",_tag_dyneither(_tmpB25,sizeof(char),7))),_tag_dyneither(_tmpB24,sizeof(void*),1))))))));}{
# 1592
const char*_tmpB2D;void*_tmpB2C[5];struct Cyc_String_pa_PrintArg_struct _tmpB2B;struct Cyc_String_pa_PrintArg_struct _tmpB2A;struct Cyc_String_pa_PrintArg_struct _tmpB29;struct Cyc_String_pa_PrintArg_struct _tmpB28;struct Cyc_String_pa_PrintArg_struct _tmpB27;struct _dyneither_ptr _tmp3BE=
(_tmpB27.tag=0,((_tmpB27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37E),((_tmpB28.tag=0,((_tmpB28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpB29.tag=0,((_tmpB29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpB2A.tag=0,((_tmpB2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37D),((_tmpB2B.tag=0,((_tmpB2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpB2C[0]=& _tmpB2B,((_tmpB2C[1]=& _tmpB2A,((_tmpB2C[2]=& _tmpB29,((_tmpB2C[3]=& _tmpB28,((_tmpB2C[4]=& _tmpB27,Cyc_aprintf(((_tmpB2D="%s %s%s %s%s",_tag_dyneither(_tmpB2D,sizeof(char),13))),_tag_dyneither(_tmpB2C,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1595
if(Cyc_v_r){{const char*_tmpB31;void*_tmpB30[1];struct Cyc_String_pa_PrintArg_struct _tmpB2F;(_tmpB2F.tag=0,((_tmpB2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3BE),((_tmpB30[0]=& _tmpB2F,Cyc_fprintf(Cyc_stderr,((_tmpB31="%s\n",_tag_dyneither(_tmpB31,sizeof(char),4))),_tag_dyneither(_tmpB30,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp3BE,sizeof(char),1))!= 0){
{const char*_tmpB34;void*_tmpB33;(_tmpB33=0,Cyc_fprintf(Cyc_stderr,((_tmpB34="Error: C compiler failed\n",_tag_dyneither(_tmpB34,sizeof(char),26))),_tag_dyneither(_tmpB33,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3C4=1;_npop_handler(0);return _tmp3C4;};}
# 1602
Cyc_remove_cfiles();{
# 1604
int _tmp3C5=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3C5;};};};};};
# 1112 "cyclone.cyc"
;_pop_region(r);};}
