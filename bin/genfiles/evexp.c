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
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Relations_Reln;
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 541
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1024 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 137 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 71
void*Cyc_Tcutil_compress(void*t);
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 163
int Cyc_Tcutil_typecmp(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple10{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{union Cyc_Absyn_Cnst f1;int f2;};
# 41 "evexp.cyc"
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 43
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;enum Cyc_Absyn_Sign _tmp1;char _tmp2;enum Cyc_Absyn_Sign _tmp3;short _tmp4;_LL1: if((_tmp0.Char_c).tag != 2)goto _LL3;_tmp1=((struct _tuple3)(_tmp0.Char_c).val).f1;_tmp2=((struct _tuple3)(_tmp0.Char_c).val).f2;_LL2:
 return Cyc_Absyn_Int_c(_tmp1,(int)_tmp2);_LL3: if((_tmp0.Short_c).tag != 4)goto _LL5;_tmp3=((struct _tuple4)(_tmp0.Short_c).val).f1;_tmp4=((struct _tuple4)(_tmp0.Short_c).val).f2;_LL4:
# 47
 return Cyc_Absyn_Int_c(_tmp3,(int)_tmp4);_LL5:;_LL6:
 return cn;_LL0:;}
# 52
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp5=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp7;int _tmp8;struct _tuple11 _tmp6=_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;
if(!_tmp8){
struct _tuple10 _tmp196;return(_tmp196.f1=0,((_tmp196.f2=0,_tmp196)));}{
union Cyc_Absyn_Cnst _tmpA=Cyc_Evexp_promote_const(_tmp7);union Cyc_Absyn_Cnst _tmpB=_tmpA;int _tmpC;long long _tmpD;_LL8: if((_tmpB.Int_c).tag != 5)goto _LLA;_tmpC=((struct _tuple5)(_tmpB.Int_c).val).f2;_LL9: {
struct _tuple10 _tmp197;return(_tmp197.f1=(unsigned int)_tmpC,((_tmp197.f2=1,_tmp197)));}_LLA: if((_tmpB.LongLong_c).tag != 6)goto _LLC;_tmpD=((struct _tuple6)(_tmpB.LongLong_c).val).f2;_LLB: {
# 60
unsigned long long y=(unsigned long long)_tmpD >> 32;
if(y != -1  && y != 0){
struct _tuple10 _tmp198;return(_tmp198.f1=0,((_tmp198.f2=0,_tmp198)));}else{
struct _tuple10 _tmp199;return(_tmp199.f1=(unsigned int)_tmpD,((_tmp199.f2=1,_tmp199)));}}_LLC:;_LLD:
{const char*_tmp19C;void*_tmp19B;(_tmp19B=0,Cyc_Tcutil_terr(e->loc,((_tmp19C="expecting unsigned int",_tag_dyneither(_tmp19C,sizeof(char),23))),_tag_dyneither(_tmp19B,sizeof(void*),0)));}{struct _tuple10 _tmp19D;return(_tmp19D.f1=0,((_tmp19D.f2=1,_tmp19D)));};_LL7:;};}struct _tuple12{int f1;int f2;};
# 68
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp14=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp16;int _tmp17;struct _tuple11 _tmp15=_tmp14;_tmp16=_tmp15.f1;_tmp17=_tmp15.f2;
if(!_tmp17){
struct _tuple12 _tmp19E;return(_tmp19E.f1=0,((_tmp19E.f2=0,_tmp19E)));}{
union Cyc_Absyn_Cnst _tmp19=Cyc_Evexp_promote_const(_tmp16);union Cyc_Absyn_Cnst _tmp1A=_tmp19;int _tmp1B;long long _tmp1C;_LLF: if((_tmp1A.Int_c).tag != 5)goto _LL11;_tmp1B=((struct _tuple5)(_tmp1A.Int_c).val).f2;_LL10: {
struct _tuple12 _tmp19F;return(_tmp19F.f1=_tmp1B != 0,((_tmp19F.f2=1,_tmp19F)));}_LL11: if((_tmp1A.LongLong_c).tag != 6)goto _LL13;_tmp1C=((struct _tuple6)(_tmp1A.LongLong_c).val).f2;_LL12: {
struct _tuple12 _tmp1A0;return(_tmp1A0.f1=_tmp1C != 0,((_tmp1A0.f2=1,_tmp1A0)));}_LL13: if((_tmp1A.Null_c).tag != 1)goto _LL15;_LL14: {
struct _tuple12 _tmp1A1;return(_tmp1A1.f1=0,((_tmp1A1.f2=0,_tmp1A1)));}_LL15:;_LL16:
{const char*_tmp1A4;void*_tmp1A3;(_tmp1A3=0,Cyc_Tcutil_terr(e->loc,((_tmp1A4="expecting bool",_tag_dyneither(_tmp1A4,sizeof(char),15))),_tag_dyneither(_tmp1A3,sizeof(void*),0)));}{struct _tuple12 _tmp1A5;return(_tmp1A5.f1=0,((_tmp1A5.f2=0,_tmp1A5)));};_LLE:;};}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 80
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp23=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp25;int _tmp26;struct _tuple11 _tmp24=_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;
if(!_tmp26){
struct _tuple11 _tmp1A6;return(_tmp1A6.f1=_tmp25,((_tmp1A6.f2=0,_tmp1A6)));}
{struct _tuple13 _tmp1A7;struct _tuple13 _tmp28=(_tmp1A7.f1=p,((_tmp1A7.f2=_tmp25,_tmp1A7)));struct _tuple13 _tmp29=_tmp28;enum Cyc_Absyn_Sign _tmp2A;int _tmp2B;enum Cyc_Absyn_Sign _tmp2C;int _tmp2D;int _tmp2E;_LL18: if(_tmp29.f1 != Cyc_Absyn_Plus)goto _LL1A;_LL19:
 goto _LL17;_LL1A: if(_tmp29.f1 != Cyc_Absyn_Minus)goto _LL1C;if(((_tmp29.f2).Int_c).tag != 5)goto _LL1C;_tmp2A=((struct _tuple5)((_tmp29.f2).Int_c).val).f1;_tmp2B=((struct _tuple5)((_tmp29.f2).Int_c).val).f2;_LL1B:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- _tmp2B);goto _LL17;_LL1C: if(_tmp29.f1 != Cyc_Absyn_Bitnot)goto _LL1E;if(((_tmp29.f2).Int_c).tag != 5)goto _LL1E;_tmp2C=((struct _tuple5)((_tmp29.f2).Int_c).val).f1;_tmp2D=((struct _tuple5)((_tmp29.f2).Int_c).val).f2;_LL1D:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ _tmp2D);goto _LL17;_LL1E: if(_tmp29.f1 != Cyc_Absyn_Not)goto _LL20;if(((_tmp29.f2).Int_c).tag != 5)goto _LL20;_tmp2E=((struct _tuple5)((_tmp29.f2).Int_c).val).f2;_LL1F:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,_tmp2E == 0?1: 0);goto _LL17;_LL20: if(_tmp29.f1 != Cyc_Absyn_Not)goto _LL22;if(((_tmp29.f2).Null_c).tag != 1)goto _LL22;_LL21:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL17;_LL22:;_LL23:
{const char*_tmp1AA;void*_tmp1A9;(_tmp1A9=0,Cyc_Tcutil_terr(e->loc,((_tmp1AA="bad constant expression",_tag_dyneither(_tmp1AA,sizeof(char),24))),_tag_dyneither(_tmp1A9,sizeof(void*),0)));}goto _LL17;_LL17:;}{
# 92
struct _tuple11 _tmp1AB;return(_tmp1AB.f1=_tmp25,((_tmp1AB.f2=1,_tmp1AB)));};}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 96
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp33=Cyc_Evexp_eval_const_exp(e1);union Cyc_Absyn_Cnst _tmp35;int _tmp36;struct _tuple11 _tmp34=_tmp33;_tmp35=_tmp34.f1;_tmp36=_tmp34.f2;{
struct _tuple11 _tmp37=Cyc_Evexp_eval_const_exp(e2);union Cyc_Absyn_Cnst _tmp39;int _tmp3A;struct _tuple11 _tmp38=_tmp37;_tmp39=_tmp38.f1;_tmp3A=_tmp38.f2;
if(!_tmp36  || !_tmp3A){
struct _tuple11 _tmp1AC;return(_tmp1AC.f1=_tmp35,((_tmp1AC.f2=0,_tmp1AC)));}
_tmp35=Cyc_Evexp_promote_const(_tmp35);
_tmp39=Cyc_Evexp_promote_const(_tmp39);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp3C=_tmp35;enum Cyc_Absyn_Sign _tmp3D;int _tmp3E;_LL25: if((_tmp3C.Int_c).tag != 5)goto _LL27;_tmp3D=((struct _tuple5)(_tmp3C.Int_c).val).f1;_tmp3E=((struct _tuple5)(_tmp3C.Int_c).val).f2;_LL26:
 s1=_tmp3D;i1=_tmp3E;goto _LL24;_LL27:;_LL28:
{const char*_tmp1AF;void*_tmp1AE;(_tmp1AE=0,Cyc_Tcutil_terr(e1->loc,((_tmp1AF="bad constant expression",_tag_dyneither(_tmp1AF,sizeof(char),24))),_tag_dyneither(_tmp1AE,sizeof(void*),0)));}{struct _tuple11 _tmp1B0;return(_tmp1B0.f1=_tmp35,((_tmp1B0.f2=1,_tmp1B0)));};_LL24:;}
# 109
{union Cyc_Absyn_Cnst _tmp42=_tmp39;enum Cyc_Absyn_Sign _tmp43;int _tmp44;_LL2A: if((_tmp42.Int_c).tag != 5)goto _LL2C;_tmp43=((struct _tuple5)(_tmp42.Int_c).val).f1;_tmp44=((struct _tuple5)(_tmp42.Int_c).val).f2;_LL2B:
 s2=_tmp43;i2=_tmp44;goto _LL29;_LL2C:;_LL2D:
{const char*_tmp1B3;void*_tmp1B2;(_tmp1B2=0,Cyc_Tcutil_terr(e2->loc,((_tmp1B3="bad constant expression",_tag_dyneither(_tmp1B3,sizeof(char),24))),_tag_dyneither(_tmp1B2,sizeof(void*),0)));}{struct _tuple11 _tmp1B4;return(_tmp1B4.f1=_tmp35,((_tmp1B4.f2=1,_tmp1B4)));};_LL29:;}
# 113
switch(p){case Cyc_Absyn_Div: _LL2E:
 goto _LL2F;case Cyc_Absyn_Mod: _LL2F:
# 116
 if(i2 == 0){
{const char*_tmp1B7;void*_tmp1B6;(_tmp1B6=0,Cyc_Tcutil_terr(e2->loc,((_tmp1B7="division by zero in constant expression",_tag_dyneither(_tmp1B7,sizeof(char),40))),_tag_dyneither(_tmp1B6,sizeof(void*),0)));}{
struct _tuple11 _tmp1B8;return(_tmp1B8.f1=_tmp35,((_tmp1B8.f2=1,_tmp1B8)));};}
# 120
break;default: _LL30:
 break;}{
# 123
int has_u_arg=s1 == Cyc_Absyn_Unsigned  || s2 == Cyc_Absyn_Unsigned;
unsigned int u1=(unsigned int)i1;
unsigned int u2=(unsigned int)i2;
int i3=0;
unsigned int u3=0;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple14 _tmp1B9;struct _tuple14 _tmp4B=(_tmp1B9.f1=p,((_tmp1B9.f2=has_u_arg,_tmp1B9)));struct _tuple14 _tmp4C=_tmp4B;_LL33: if(_tmp4C.f1 != Cyc_Absyn_Plus)goto _LL35;if(_tmp4C.f2 != 0)goto _LL35;_LL34:
 i3=i1 + i2;use_i3=1;goto _LL32;_LL35: if(_tmp4C.f1 != Cyc_Absyn_Times)goto _LL37;if(_tmp4C.f2 != 0)goto _LL37;_LL36:
 i3=i1 * i2;use_i3=1;goto _LL32;_LL37: if(_tmp4C.f1 != Cyc_Absyn_Minus)goto _LL39;if(_tmp4C.f2 != 0)goto _LL39;_LL38:
 i3=i1 - i2;use_i3=1;goto _LL32;_LL39: if(_tmp4C.f1 != Cyc_Absyn_Div)goto _LL3B;if(_tmp4C.f2 != 0)goto _LL3B;_LL3A:
 i3=i1 / i2;use_i3=1;goto _LL32;_LL3B: if(_tmp4C.f1 != Cyc_Absyn_Mod)goto _LL3D;if(_tmp4C.f2 != 0)goto _LL3D;_LL3C:
 i3=i1 % i2;use_i3=1;goto _LL32;_LL3D: if(_tmp4C.f1 != Cyc_Absyn_Plus)goto _LL3F;if(_tmp4C.f2 != 1)goto _LL3F;_LL3E:
 u3=u1 + u2;use_u3=1;goto _LL32;_LL3F: if(_tmp4C.f1 != Cyc_Absyn_Times)goto _LL41;if(_tmp4C.f2 != 1)goto _LL41;_LL40:
 u3=u1 * u2;use_u3=1;goto _LL32;_LL41: if(_tmp4C.f1 != Cyc_Absyn_Minus)goto _LL43;if(_tmp4C.f2 != 1)goto _LL43;_LL42:
 u3=u1 - u2;use_u3=1;goto _LL32;_LL43: if(_tmp4C.f1 != Cyc_Absyn_Div)goto _LL45;if(_tmp4C.f2 != 1)goto _LL45;_LL44:
 u3=u1 / u2;use_u3=1;goto _LL32;_LL45: if(_tmp4C.f1 != Cyc_Absyn_Mod)goto _LL47;if(_tmp4C.f2 != 1)goto _LL47;_LL46:
 u3=u1 % u2;use_u3=1;goto _LL32;_LL47: if(_tmp4C.f1 != Cyc_Absyn_Eq)goto _LL49;_LL48:
 b3=i1 == i2;use_b3=1;goto _LL32;_LL49: if(_tmp4C.f1 != Cyc_Absyn_Neq)goto _LL4B;_LL4A:
 b3=i1 != i2;use_b3=1;goto _LL32;_LL4B: if(_tmp4C.f1 != Cyc_Absyn_Gt)goto _LL4D;if(_tmp4C.f2 != 0)goto _LL4D;_LL4C:
 b3=i1 > i2;use_b3=1;goto _LL32;_LL4D: if(_tmp4C.f1 != Cyc_Absyn_Lt)goto _LL4F;if(_tmp4C.f2 != 0)goto _LL4F;_LL4E:
 b3=i1 < i2;use_b3=1;goto _LL32;_LL4F: if(_tmp4C.f1 != Cyc_Absyn_Gte)goto _LL51;if(_tmp4C.f2 != 0)goto _LL51;_LL50:
 b3=i1 >= i2;use_b3=1;goto _LL32;_LL51: if(_tmp4C.f1 != Cyc_Absyn_Lte)goto _LL53;if(_tmp4C.f2 != 0)goto _LL53;_LL52:
 b3=i1 <= i2;use_b3=1;goto _LL32;_LL53: if(_tmp4C.f1 != Cyc_Absyn_Gt)goto _LL55;if(_tmp4C.f2 != 1)goto _LL55;_LL54:
 b3=u1 > u2;use_b3=1;goto _LL32;_LL55: if(_tmp4C.f1 != Cyc_Absyn_Lt)goto _LL57;if(_tmp4C.f2 != 1)goto _LL57;_LL56:
 b3=u1 < u2;use_b3=1;goto _LL32;_LL57: if(_tmp4C.f1 != Cyc_Absyn_Gte)goto _LL59;if(_tmp4C.f2 != 1)goto _LL59;_LL58:
 b3=u1 >= u2;use_b3=1;goto _LL32;_LL59: if(_tmp4C.f1 != Cyc_Absyn_Lte)goto _LL5B;if(_tmp4C.f2 != 1)goto _LL5B;_LL5A:
 b3=u1 <= u2;use_b3=1;goto _LL32;_LL5B: if(_tmp4C.f1 != Cyc_Absyn_Bitand)goto _LL5D;_LL5C:
 u3=u1 & u2;use_u3=1;goto _LL32;_LL5D: if(_tmp4C.f1 != Cyc_Absyn_Bitor)goto _LL5F;_LL5E:
 u3=u1 | u2;use_u3=1;goto _LL32;_LL5F: if(_tmp4C.f1 != Cyc_Absyn_Bitxor)goto _LL61;_LL60:
 u3=u1 ^ u2;use_u3=1;goto _LL32;_LL61: if(_tmp4C.f1 != Cyc_Absyn_Bitlshift)goto _LL63;_LL62:
 u3=u1 << u2;use_u3=1;goto _LL32;_LL63: if(_tmp4C.f1 != Cyc_Absyn_Bitlrshift)goto _LL65;_LL64:
 u3=u1 >> u2;use_u3=1;goto _LL32;_LL65: if(_tmp4C.f1 != Cyc_Absyn_Bitarshift)goto _LL67;_LL66:
{const char*_tmp1BC;void*_tmp1BB;(_tmp1BB=0,Cyc_Tcutil_terr(e1->loc,((_tmp1BC=">>> NOT IMPLEMENTED",_tag_dyneither(_tmp1BC,sizeof(char),20))),_tag_dyneither(_tmp1BB,sizeof(void*),0)));}goto _LL68;_LL67:;_LL68:
{const char*_tmp1BF;void*_tmp1BE;(_tmp1BE=0,Cyc_Tcutil_terr(e1->loc,((_tmp1BF="bad constant expression",_tag_dyneither(_tmp1BF,sizeof(char),24))),_tag_dyneither(_tmp1BE,sizeof(void*),0)));}{struct _tuple11 _tmp1C0;return(_tmp1C0.f1=_tmp35,((_tmp1C0.f2=1,_tmp1C0)));};_LL32:;}
# 161
if(use_i3){struct _tuple11 _tmp1C1;return(_tmp1C1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3),((_tmp1C1.f2=1,_tmp1C1)));}
if(use_u3){struct _tuple11 _tmp1C2;return(_tmp1C2.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3),((_tmp1C2.f2=1,_tmp1C2)));}
if(use_b3){struct _tuple11 _tmp1C3;return(_tmp1C3.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0),((_tmp1C3.f2=1,_tmp1C3)));}{
const char*_tmp1C6;void*_tmp1C5;(_tmp1C5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1C6="Evexp::eval_const_binop",_tag_dyneither(_tmp1C6,sizeof(char),24))),_tag_dyneither(_tmp1C5,sizeof(void*),0)));};};};};}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 170
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 ans;
{void*_tmp58=e->r;void*_tmp59=_tmp58;union Cyc_Absyn_Cnst _tmp5C;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_Absyn_Exp*_tmp5F;struct Cyc_Absyn_Exp*_tmp60;struct Cyc_Absyn_Exp*_tmp62;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_Absyn_Exp*_tmp66;enum Cyc_Absyn_Primop _tmp68;struct Cyc_List_List*_tmp69;void*_tmp6B;void*_tmp70;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Enumfield*_tmp73;struct Cyc_Absyn_Enumfield*_tmp75;_LL6A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp5A->tag != 1)goto _LL6C;}_LL6B: {
# 174
struct _tuple11 _tmp1C7;return(_tmp1C7.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C7.f2=0,_tmp1C7)));}_LL6C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp5B->tag != 0)goto _LL6E;else{_tmp5C=_tmp5B->f1;}}_LL6D: {
struct _tuple11 _tmp1C8;return(_tmp1C8.f1=_tmp5C,((_tmp1C8.f2=1,_tmp1C8)));}_LL6E: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp5D->tag != 5)goto _LL70;else{_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;_tmp60=_tmp5D->f3;}}_LL6F: {
# 177
struct _tuple12 _tmp78=Cyc_Evexp_eval_const_bool_exp(_tmp5E);int _tmp7A;int _tmp7B;struct _tuple12 _tmp79=_tmp78;_tmp7A=_tmp79.f1;_tmp7B=_tmp79.f2;
if(!_tmp7B){
Cyc_Evexp_eval_const_exp(_tmp5F);
Cyc_Evexp_eval_const_exp(_tmp60);{
struct _tuple11 _tmp1C9;return(_tmp1C9.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C9.f2=0,_tmp1C9)));};}
# 183
ans=_tmp7A?Cyc_Evexp_eval_const_exp(_tmp5F): Cyc_Evexp_eval_const_exp(_tmp60);
goto _LL69;}_LL70: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp61=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp61->tag != 6)goto _LL72;else{_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;}}_LL71: {
# 186
struct _tuple12 _tmp7D=Cyc_Evexp_eval_const_bool_exp(_tmp62);int _tmp7F;int _tmp80;struct _tuple12 _tmp7E=_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;
if(!_tmp80){
Cyc_Evexp_eval_const_exp(_tmp63);{
struct _tuple11 _tmp1CA;return(_tmp1CA.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1CA.f2=0,_tmp1CA)));};}
# 191
if(_tmp7F)ans=Cyc_Evexp_eval_const_exp(_tmp63);else{
struct _tuple11 _tmp1CB;ans=((_tmp1CB.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1CB.f2=1,_tmp1CB))));}
goto _LL69;}_LL72: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp64=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp64->tag != 7)goto _LL74;else{_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;}}_LL73: {
# 195
struct _tuple12 _tmp83=Cyc_Evexp_eval_const_bool_exp(_tmp65);int _tmp85;int _tmp86;struct _tuple12 _tmp84=_tmp83;_tmp85=_tmp84.f1;_tmp86=_tmp84.f2;
if(!_tmp86){
Cyc_Evexp_eval_const_exp(_tmp66);{
struct _tuple11 _tmp1CC;return(_tmp1CC.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1CC.f2=0,_tmp1CC)));};}
# 200
if(!_tmp85)ans=Cyc_Evexp_eval_const_exp(_tmp66);else{
struct _tuple11 _tmp1CD;ans=((_tmp1CD.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1),((_tmp1CD.f2=1,_tmp1CD))));}
goto _LL69;}_LL74: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp67=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp67->tag != 2)goto _LL76;else{_tmp68=_tmp67->f1;_tmp69=_tmp67->f2;}}_LL75:
# 204
 if(_tmp69 == 0){
{const char*_tmp1D0;void*_tmp1CF;(_tmp1CF=0,Cyc_Tcutil_terr(e->loc,((_tmp1D0="bad static expression (no args to primop)",_tag_dyneither(_tmp1D0,sizeof(char),42))),_tag_dyneither(_tmp1CF,sizeof(void*),0)));}{
struct _tuple11 _tmp1D1;return(_tmp1D1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1D1.f2=1,_tmp1D1)));};}
# 208
if(_tmp69->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(_tmp68,(struct Cyc_Absyn_Exp*)_tmp69->hd);
goto _LL69;}
# 212
if(((struct Cyc_List_List*)_check_null(_tmp69->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(_tmp68,(struct Cyc_Absyn_Exp*)_tmp69->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp69->tl))->hd);
goto _LL69;}
# 216
{const char*_tmp1D4;void*_tmp1D3;(_tmp1D3=0,Cyc_Tcutil_terr(e->loc,((_tmp1D4="bad static expression (too many args to primop)",_tag_dyneither(_tmp1D4,sizeof(char),48))),_tag_dyneither(_tmp1D3,sizeof(void*),0)));}{
struct _tuple11 _tmp1D5;return(_tmp1D5.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1D5.f2=1,_tmp1D5)));};_LL76: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp6A->tag != 38)goto _LL78;else{_tmp6B=(void*)_tmp6A->f1;}}_LL77:
# 220
{void*_tmp8F=Cyc_Tcutil_compress(_tmp6B);void*_tmp90=_tmp8F;struct Cyc_Absyn_Exp*_tmp92;_LL87: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp90;if(_tmp91->tag != 18)goto _LL89;else{_tmp92=_tmp91->f1;}}_LL88:
# 222
 e->r=_tmp92->r;
return Cyc_Evexp_eval_const_exp(_tmp92);_LL89:;_LL8A:
 goto _LL86;_LL86:;}
# 226
goto _LL79;_LL78: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp6C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp6C->tag != 16)goto _LL7A;}_LL79:
 goto _LL7B;_LL7A: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp6D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp6D->tag != 17)goto _LL7C;}_LL7B:
 goto _LL7D;_LL7C: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp6E=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp6E->tag != 18)goto _LL7E;}_LL7D:
{struct _tuple11 _tmp1D6;ans=((_tmp1D6.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1D6.f2=0,_tmp1D6))));}goto _LL69;_LL7E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp6F->tag != 13)goto _LL80;else{_tmp70=(void*)_tmp6F->f1;_tmp71=_tmp6F->f2;}}_LL7F:
# 232
 ans=Cyc_Evexp_eval_const_exp(_tmp71);
if(ans.f2){
struct _tuple15 _tmp1D7;struct _tuple15 _tmp94=(_tmp1D7.f1=Cyc_Tcutil_compress(_tmp70),((_tmp1D7.f2=ans.f1,_tmp1D7)));struct _tuple15 _tmp95=_tmp94;enum Cyc_Absyn_Sign _tmp97;enum Cyc_Absyn_Size_of _tmp98;enum Cyc_Absyn_Sign _tmp99;char _tmp9A;enum Cyc_Absyn_Sign _tmp9C;enum Cyc_Absyn_Size_of _tmp9D;enum Cyc_Absyn_Sign _tmp9E;short _tmp9F;enum Cyc_Absyn_Sign _tmpA1;enum Cyc_Absyn_Size_of _tmpA2;enum Cyc_Absyn_Sign _tmpA3;int _tmpA4;void*_tmpA6;enum Cyc_Absyn_Sign _tmpA7;char _tmpA8;void*_tmpAA;enum Cyc_Absyn_Sign _tmpAB;short _tmpAC;void*_tmpAE;enum Cyc_Absyn_Sign _tmpAF;int _tmpB0;_LL8C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp95.f1;if(_tmp96->tag != 6)goto _LL8E;else{_tmp97=_tmp96->f1;_tmp98=_tmp96->f2;}}if(((_tmp95.f2).Char_c).tag != 2)goto _LL8E;_tmp99=((struct _tuple3)((_tmp95.f2).Char_c).val).f1;_tmp9A=((struct _tuple3)((_tmp95.f2).Char_c).val).f2;_LL8D:
# 236
 _tmp9C=_tmp97;_tmp9D=_tmp98;_tmp9E=_tmp99;_tmp9F=(short)_tmp9A;goto _LL8F;_LL8E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp95.f1;if(_tmp9B->tag != 6)goto _LL90;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}if(((_tmp95.f2).Short_c).tag != 4)goto _LL90;_tmp9E=((struct _tuple4)((_tmp95.f2).Short_c).val).f1;_tmp9F=((struct _tuple4)((_tmp95.f2).Short_c).val).f2;_LL8F:
 _tmpA1=_tmp9C;_tmpA2=_tmp9D;_tmpA3=_tmp9E;_tmpA4=(int)_tmp9F;goto _LL91;_LL90:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp95.f1;if(_tmpA0->tag != 6)goto _LL92;else{_tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;}}if(((_tmp95.f2).Int_c).tag != 5)goto _LL92;_tmpA3=((struct _tuple5)((_tmp95.f2).Int_c).val).f1;_tmpA4=((struct _tuple5)((_tmp95.f2).Int_c).val).f2;_LL91:
# 239
 if(_tmpA1 != _tmpA3)
ans.f1=Cyc_Absyn_Int_c(_tmpA1,_tmpA4);
goto _LL8B;_LL92:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp95.f1;if(_tmpA5->tag != 19)goto _LL94;else{_tmpA6=(void*)_tmpA5->f1;}}if(((_tmp95.f2).Char_c).tag != 2)goto _LL94;_tmpA7=((struct _tuple3)((_tmp95.f2).Char_c).val).f1;_tmpA8=((struct _tuple3)((_tmp95.f2).Char_c).val).f2;_LL93:
# 243
 _tmpAA=_tmpA6;_tmpAB=_tmpA7;_tmpAC=(short)_tmpA8;goto _LL95;_LL94:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp95.f1;if(_tmpA9->tag != 19)goto _LL96;else{_tmpAA=(void*)_tmpA9->f1;}}if(((_tmp95.f2).Short_c).tag != 4)goto _LL96;_tmpAB=((struct _tuple4)((_tmp95.f2).Short_c).val).f1;_tmpAC=((struct _tuple4)((_tmp95.f2).Short_c).val).f2;_LL95:
 _tmpAE=_tmpAA;_tmpAF=_tmpAB;_tmpB0=(int)_tmpAC;goto _LL97;_LL96:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp95.f1;if(_tmpAD->tag != 19)goto _LL98;else{_tmpAE=(void*)_tmpAD->f1;}}if(((_tmp95.f2).Int_c).tag != 5)goto _LL98;_tmpAF=((struct _tuple5)((_tmp95.f2).Int_c).val).f1;_tmpB0=((struct _tuple5)((_tmp95.f2).Int_c).val).f2;_LL97:
# 246
 if(_tmpB0 < 0){
const char*_tmp1DA;void*_tmp1D9;(_tmp1D9=0,Cyc_Tcutil_terr(e->loc,((_tmp1DA="cannot cast negative number to a tag type",_tag_dyneither(_tmp1DA,sizeof(char),42))),_tag_dyneither(_tmp1D9,sizeof(void*),0)));}
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1DD;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1DC;Cyc_Tcutil_unify(_tmpAE,(void*)((_tmp1DC=_cycalloc(sizeof(*_tmp1DC)),((_tmp1DC[0]=((_tmp1DD.tag=18,((_tmp1DD.f1=Cyc_Absyn_const_exp(ans.f1,0),_tmp1DD)))),_tmp1DC)))));}
ans.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmpB0);
goto _LL8B;_LL98:;_LL99:
{const char*_tmp1E1;void*_tmp1E0[1];struct Cyc_String_pa_PrintArg_struct _tmp1DF;(_tmp1DF.tag=0,((_tmp1DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp70)),((_tmp1E0[0]=& _tmp1DF,Cyc_Tcutil_terr(e->loc,((_tmp1E1="cannot cast to %s",_tag_dyneither(_tmp1E1,sizeof(char),18))),_tag_dyneither(_tmp1E0,sizeof(void*),1)))))));}goto _LL8B;_LL8B:;}
# 253
goto _LL69;_LL80: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp72=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp72->tag != 32)goto _LL82;else{_tmp73=_tmp72->f3;}}_LL81:
 _tmp75=_tmp73;goto _LL83;_LL82: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp74=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp59;if(_tmp74->tag != 31)goto _LL84;else{_tmp75=_tmp74->f3;}}_LL83:
 ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmp75))->tag));goto _LL69;_LL84:;_LL85:
# 257
{const char*_tmp1E5;void*_tmp1E4[1];struct Cyc_String_pa_PrintArg_struct _tmp1E3;(_tmp1E3.tag=0,((_tmp1E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1E4[0]=& _tmp1E3,Cyc_Tcutil_terr(e->loc,((_tmp1E5="bad static expression %s",_tag_dyneither(_tmp1E5,sizeof(char),25))),_tag_dyneither(_tmp1E4,sizeof(void*),1)))))));}{
struct _tuple11 _tmp1E6;return(_tmp1E6.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1E6.f2=1,_tmp1E6)));};_LL69:;}
# 260
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmpBD=ans.f1;
{union Cyc_Absyn_Cnst _tmpBE=_tmpBD;_LL9B: if((_tmpBE.Int_c).tag != 5)goto _LL9D;if(((struct _tuple5)(_tmpBE.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL9D;if(((struct _tuple5)(_tmpBE.Int_c).val).f2 != 0)goto _LL9D;_LL9C: {
# 265
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
# 267
c=(void*)& uzero;
goto _LL9A;}_LL9D: if((_tmpBE.Int_c).tag != 5)goto _LL9F;if(((struct _tuple5)(_tmpBE.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL9F;if(((struct _tuple5)(_tmpBE.Int_c).val).f2 != 1)goto _LL9F;_LL9E: {
# 270
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 272
c=(void*)& uone;
goto _LL9A;}_LL9F: if((_tmpBE.Int_c).tag != 5)goto _LLA1;if(((struct _tuple5)(_tmpBE.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LLA1;if(((struct _tuple5)(_tmpBE.Int_c).val).f2 != 0)goto _LLA1;_LLA0: {
# 275
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 277
c=(void*)& szero;
goto _LL9A;}_LLA1: if((_tmpBE.Int_c).tag != 5)goto _LLA3;if(((struct _tuple5)(_tmpBE.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LLA3;if(((struct _tuple5)(_tmpBE.Int_c).val).f2 != 1)goto _LLA3;_LLA2: {
# 280
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
# 282
c=(void*)& sone;
goto _LL9A;}_LLA3:;_LLA4:
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp1E9;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1E8;c=(void*)((_tmp1E8=_cycalloc(sizeof(*_tmp1E8)),((_tmp1E8[0]=((_tmp1E9.tag=0,((_tmp1E9.f1=_tmpBD,_tmp1E9)))),_tmp1E8))));}goto _LL9A;_LL9A:;}
# 286
e->r=c;}
# 288
return ans;}
# 291
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmpC5=e->r;void*_tmpC6=_tmpC5;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;enum Cyc_Absyn_Primop _tmpD8;struct Cyc_List_List*_tmpD9;struct Cyc_Absyn_Exp*_tmpDC;_LLA6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC7=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpC7->tag != 32)goto _LLA8;}_LLA7:
 goto _LLA9;_LLA8: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC8=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpC8->tag != 31)goto _LLAA;}_LLA9:
 goto _LLAB;_LLAA: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC9=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpC9->tag != 16)goto _LLAC;}_LLAB:
 goto _LLAD;_LLAC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCA=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpCA->tag != 17)goto _LLAE;}_LLAD:
 goto _LLAF;_LLAE: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCB=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpCB->tag != 18)goto _LLB0;}_LLAF:
 goto _LLB1;_LLB0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpCC->tag != 0)goto _LLB2;}_LLB1:
 return 1;_LLB2: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCD=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpCD->tag != 5)goto _LLB4;else{_tmpCE=_tmpCD->f1;_tmpCF=_tmpCD->f2;_tmpD0=_tmpCD->f3;}}_LLB3:
# 300
 return(Cyc_Evexp_c_can_eval(_tmpCE) && Cyc_Evexp_c_can_eval(_tmpCF)) && Cyc_Evexp_c_can_eval(_tmpD0);_LLB4: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpD1=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpD1->tag != 6)goto _LLB6;else{_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;}}_LLB5:
 _tmpD5=_tmpD2;_tmpD6=_tmpD3;goto _LLB7;_LLB6: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpD4->tag != 7)goto _LLB8;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;}}_LLB7:
# 303
 return Cyc_Evexp_c_can_eval(_tmpD5) && Cyc_Evexp_c_can_eval(_tmpD6);_LLB8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpD7->tag != 2)goto _LLBA;else{_tmpD8=_tmpD7->f1;_tmpD9=_tmpD7->f2;}}_LLB9:
# 305
 for(0;_tmpD9 != 0;_tmpD9=_tmpD9->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmpD9->hd))return 0;}
return 1;_LLBA: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpDA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpDA->tag != 38)goto _LLBC;}_LLBB:
 return 0;_LLBC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC6;if(_tmpDB->tag != 13)goto _LLBE;else{_tmpDC=_tmpDB->f2;}}_LLBD:
 return Cyc_Evexp_c_can_eval(_tmpDC);_LLBE:;_LLBF:
 return 0;_LLA5:;}
# 314
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmpDD=e->r;void*_tmpDE=_tmpDD;_LLC1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpDF->tag != 0)goto _LLC3;}_LLC2:
 return 1;_LLC3: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE0->tag != 5)goto _LLC5;}_LLC4:
 return 2;_LLC5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE1->tag != 2)goto _LLC7;}_LLC6:
 return 3;_LLC7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE2->tag != 16)goto _LLC9;}_LLC8:
 goto _LLCA;_LLC9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE3->tag != 17)goto _LLCB;}_LLCA:
 return 4;_LLCB: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE4->tag != 18)goto _LLCD;}_LLCC:
 return 5;_LLCD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE5->tag != 13)goto _LLCF;}_LLCE:
 return 6;_LLCF: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE6->tag != 6)goto _LLD1;}_LLD0:
 return 7;_LLD1: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpE7=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE7->tag != 7)goto _LLD3;}_LLD2:
 return 8;_LLD3: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE8->tag != 38)goto _LLD5;}_LLD4:
 return 9;_LLD5:;_LLD6:
# 327
{const char*_tmp1ED;void*_tmp1EC[1];struct Cyc_String_pa_PrintArg_struct _tmp1EB;(_tmp1EB.tag=0,((_tmp1EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1EC[0]=& _tmp1EB,Cyc_Tcutil_terr(e->loc,((_tmp1ED="bad static expression %s",_tag_dyneither(_tmp1ED,sizeof(char),25))),_tag_dyneither(_tmp1EC,sizeof(void*),1)))))));}return 0;_LLC0:;}struct _tuple16{void*f1;void*f2;};
# 331
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmpEC=Cyc_Evexp_eval_const_uint_exp(e1);unsigned int _tmpEE;int _tmpEF;struct _tuple10 _tmpED=_tmpEC;_tmpEE=_tmpED.f1;_tmpEF=_tmpED.f2;{
struct _tuple10 _tmpF0=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmpF2;int _tmpF3;struct _tuple10 _tmpF1=_tmpF0;_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;
if(_tmpEF  && _tmpF3)
return(int)(_tmpEE - _tmpF2);{
int _tmpF4=Cyc_Evexp_const_exp_case_number(e1);
int _tmpF5=Cyc_Evexp_const_exp_case_number(e2);
if(_tmpF4 != _tmpF5)
return _tmpF4 - _tmpF5;{
struct _tuple16 _tmp1EE;struct _tuple16 _tmpF6=(_tmp1EE.f1=e1->r,((_tmp1EE.f2=e2->r,_tmp1EE)));struct _tuple16 _tmpF7=_tmpF6;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;enum Cyc_Absyn_Primop _tmp10D;struct Cyc_List_List*_tmp10E;enum Cyc_Absyn_Primop _tmp110;struct Cyc_List_List*_tmp111;void*_tmp113;void*_tmp115;void*_tmp117;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11B;void*_tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp121;void*_tmp123;struct Cyc_List_List*_tmp124;void*_tmp126;struct Cyc_List_List*_tmp127;void*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;void*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;void*_tmp12F;void*_tmp131;_LLD8:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmpF8->tag != 5)goto _LLDA;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;_tmpFB=_tmpF8->f3;}}{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmpFC->tag != 5)goto _LLDA;else{_tmpFD=_tmpFC->f1;_tmpFE=_tmpFC->f2;_tmpFF=_tmpFC->f3;}};_LLD9: {
# 345
int _tmp132=Cyc_Evexp_const_exp_cmp(_tmpFB,_tmpFF);
if(_tmp132 != 0)return _tmp132;
_tmp101=_tmpF9;_tmp102=_tmpFA;_tmp104=_tmpFD;_tmp105=_tmpFE;goto _LLDB;}_LLDA:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp100->tag != 6)goto _LLDC;else{_tmp101=_tmp100->f1;_tmp102=_tmp100->f2;}}{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp103->tag != 6)goto _LLDC;else{_tmp104=_tmp103->f1;_tmp105=_tmp103->f2;}};_LLDB:
 _tmp107=_tmp101;_tmp108=_tmp102;_tmp10A=_tmp104;_tmp10B=_tmp105;goto _LLDD;_LLDC:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp106->tag != 7)goto _LLDE;else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}}{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp109=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp109->tag != 7)goto _LLDE;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}};_LLDD: {
# 350
int _tmp133=Cyc_Evexp_const_exp_cmp(_tmp107,_tmp10A);
if(_tmp133 != 0)return _tmp133;
return Cyc_Evexp_const_exp_cmp(_tmp108,_tmp10B);}_LLDE:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp10C->tag != 2)goto _LLE0;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp10F->tag != 2)goto _LLE0;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;}};_LLDF: {
# 354
int _tmp134=(int)_tmp10D - (int)_tmp110;
if(_tmp134 != 0)return _tmp134;
for(0;_tmp10E != 0  && _tmp111 != 0;(_tmp10E=_tmp10E->tl,_tmp111=_tmp111->tl)){
int _tmp135=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmp10E->hd,(struct Cyc_Absyn_Exp*)_tmp111->hd);
if(_tmp135 != 0)
return _tmp135;}
# 361
return 0;}_LLE0:{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp112=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp112->tag != 16)goto _LLE2;else{_tmp113=(void*)_tmp112->f1;}}{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp114->tag != 16)goto _LLE2;else{_tmp115=(void*)_tmp114->f1;}};_LLE1:
 return Cyc_Tcutil_typecmp(_tmp113,_tmp115);_LLE2:{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp116->tag != 16)goto _LLE4;else{_tmp117=(void*)_tmp116->f1;}}{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp118=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp118->tag != 17)goto _LLE4;else{_tmp119=_tmp118->f1;}};_LLE3: {
# 364
void*_tmp136=_tmp119->topt;
if(_tmp136 == 0){
const char*_tmp1F1;void*_tmp1F0;(_tmp1F0=0,Cyc_Tcutil_terr(e2->loc,((_tmp1F1="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1F1,sizeof(char),51))),_tag_dyneither(_tmp1F0,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp(_tmp117,(void*)_check_null(_tmp136));}_LLE4:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp11A->tag != 17)goto _LLE6;else{_tmp11B=_tmp11A->f1;}}{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp11C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp11C->tag != 16)goto _LLE6;else{_tmp11D=(void*)_tmp11C->f1;}};_LLE5: {
# 369
void*_tmp139=_tmp11B->topt;
if(_tmp139 == 0){
const char*_tmp1F4;void*_tmp1F3;(_tmp1F3=0,Cyc_Tcutil_terr(e1->loc,((_tmp1F4="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1F4,sizeof(char),51))),_tag_dyneither(_tmp1F3,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp139),_tmp11D);}_LLE6:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp11E->tag != 17)goto _LLE8;else{_tmp11F=_tmp11E->f1;}}{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp120=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp120->tag != 17)goto _LLE8;else{_tmp121=_tmp120->f1;}};_LLE7: {
# 374
void*_tmp13C=_tmp11F->topt;
void*_tmp13D=_tmp121->topt;
if(_tmp13C == 0){
const char*_tmp1F7;void*_tmp1F6;(_tmp1F6=0,Cyc_Tcutil_terr(e1->loc,((_tmp1F7="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1F7,sizeof(char),51))),_tag_dyneither(_tmp1F6,sizeof(void*),0)));}
if(_tmp13D == 0){
const char*_tmp1FA;void*_tmp1F9;(_tmp1F9=0,Cyc_Tcutil_terr(e2->loc,((_tmp1FA="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1FA,sizeof(char),51))),_tag_dyneither(_tmp1F9,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp13C),(void*)_check_null(_tmp13D));}_LLE8:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp122->tag != 18)goto _LLEA;else{_tmp123=(void*)_tmp122->f1;_tmp124=_tmp122->f2;}}{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp125->tag != 18)goto _LLEA;else{_tmp126=(void*)_tmp125->f1;_tmp127=_tmp125->f2;}};_LLE9: {
# 382
int _tmp142=Cyc_Tcutil_typecmp(_tmp123,_tmp126);
if(_tmp142 != 0)return _tmp142;{
int _tmp143=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp124);
int _tmp144=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp127);
if(_tmp143 < _tmp144)return - 1;
if(_tmp144 < _tmp143)return 1;
for(0;_tmp124 != 0  && _tmp127 != 0;(_tmp124=_tmp124->tl,_tmp127=_tmp127->tl)){
struct _tuple16 _tmp1FB;struct _tuple16 _tmp145=(_tmp1FB.f1=(void*)_tmp124->hd,((_tmp1FB.f2=(void*)_tmp127->hd,_tmp1FB)));struct _tuple16 _tmp146=_tmp145;struct _dyneither_ptr*_tmp14C;struct _dyneither_ptr*_tmp14E;unsigned int _tmp150;unsigned int _tmp152;_LLF1:{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp147=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp146.f1;if(_tmp147->tag != 0)goto _LLF3;}{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp148=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp146.f2;if(_tmp148->tag != 1)goto _LLF3;};_LLF2:
 return - 1;_LLF3:{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp149=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp146.f1;if(_tmp149->tag != 1)goto _LLF5;}{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp14A=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp146.f2;if(_tmp14A->tag != 0)goto _LLF5;};_LLF4:
 return 1;_LLF5:{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp14B=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp146.f1;if(_tmp14B->tag != 0)goto _LLF7;else{_tmp14C=_tmp14B->f1;}}{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp14D=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp146.f2;if(_tmp14D->tag != 0)goto _LLF7;else{_tmp14E=_tmp14D->f1;}};_LLF6: {
# 393
int c=Cyc_strptrcmp(_tmp14C,_tmp14E);
if(c != 0)return c;
goto _LLF0;}_LLF7:{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp14F=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp146.f1;if(_tmp14F->tag != 1)goto _LLF0;else{_tmp150=_tmp14F->f1;}}{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp151=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp146.f2;if(_tmp151->tag != 1)goto _LLF0;else{_tmp152=_tmp151->f1;}};_LLF8: {
# 397
int c=(int)(_tmp150 - _tmp152);
if(c != 0)return c;
goto _LLF0;}_LLF0:;}
# 401
return 0;};}_LLEA:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp128->tag != 13)goto _LLEC;else{_tmp129=(void*)_tmp128->f1;_tmp12A=_tmp128->f2;}}{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp12B->tag != 13)goto _LLEC;else{_tmp12C=(void*)_tmp12B->f1;_tmp12D=_tmp12B->f2;}};_LLEB: {
# 403
int _tmp154=Cyc_Tcutil_typecmp(_tmp129,_tmp12C);
if(_tmp154 != 0)return _tmp154;
return Cyc_Evexp_const_exp_cmp(_tmp12A,_tmp12D);}_LLEC:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF7.f1;if(_tmp12E->tag != 38)goto _LLEE;else{_tmp12F=(void*)_tmp12E->f1;}}{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF7.f2;if(_tmp130->tag != 38)goto _LLEE;else{_tmp131=(void*)_tmp130->f1;}};_LLED:
# 407
 if(Cyc_Tcutil_unify(_tmp12F,_tmp131))return 0;
return Cyc_Tcutil_typecmp(_tmp12F,_tmp131);_LLEE:;_LLEF: {
const char*_tmp1FE;void*_tmp1FD;(_tmp1FD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1FE="Evexp::const_exp_cmp, unexpected case",_tag_dyneither(_tmp1FE,sizeof(char),38))),_tag_dyneither(_tmp1FD,sizeof(void*),0)));}_LLD7:;};};};}
# 412
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 415
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmp158=Cyc_Evexp_eval_const_uint_exp(e1);unsigned int _tmp15A;int _tmp15B;struct _tuple10 _tmp159=_tmp158;_tmp15A=_tmp159.f1;_tmp15B=_tmp159.f2;{
struct _tuple10 _tmp15C=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp15E;int _tmp15F;struct _tuple10 _tmp15D=_tmp15C;_tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;
if(_tmp15B  && _tmp15F)
return _tmp15A <= _tmp15E;
return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 425
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp160=Cyc_Tcutil_compress(t);void*_tmp161=_tmp160;struct Cyc_Absyn_Tvar*_tmp164;struct Cyc_Absyn_Datatypefield*_tmp169;struct Cyc_List_List*_tmp16B;void*_tmp172;struct Cyc_Absyn_Exp*_tmp173;union Cyc_Absyn_AggrInfoU _tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17B;struct Cyc_Absyn_Typedefdecl*_tmp187;_LLFA: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp161;if(_tmp162->tag != 0)goto _LLFC;}_LLFB:
 return 0;_LLFC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp161;if(_tmp163->tag != 2)goto _LLFE;else{_tmp164=_tmp163->f1;}}_LLFD: {
# 429
enum Cyc_Absyn_KindQual _tmp188=(Cyc_Tcutil_tvar_kind(_tmp164,& Cyc_Tcutil_bk))->kind;switch(_tmp188){case Cyc_Absyn_BoxKind: _LL134:
 return 1;default: _LL135:
 return 0;}}_LLFE: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp161;if(_tmp165->tag != 1)goto _LL100;else{if(_tmp165->f1 == 0)goto _LL100;if(((struct Cyc_Absyn_Kind*)(_tmp165->f1)->v)->kind != Cyc_Absyn_BoxKind)goto _LL100;}}_LLFF:
# 433
 return 1;_LL100: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp161;if(_tmp166->tag != 1)goto _LL102;}_LL101:
 return 0;_LL102: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp161;if(_tmp167->tag != 3)goto _LL104;}_LL103:
 return 1;_LL104: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp161;if(_tmp168->tag != 4)goto _LL106;else{if((((_tmp168->f1).field_info).KnownDatatypefield).tag != 2)goto _LL106;_tmp169=((struct _tuple1)(((_tmp168->f1).field_info).KnownDatatypefield).val).f2;}}_LL105:
# 437
 _tmp16B=_tmp169->typs;goto _LL107;_LL106: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp161;if(_tmp16A->tag != 10)goto _LL108;else{_tmp16B=_tmp16A->f1;}}_LL107:
# 439
 for(0;_tmp16B != 0;_tmp16B=_tmp16B->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmp16B->hd)).f2))
return 0;}
return 1;_LL108: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp161;if(_tmp16C->tag != 4)goto _LL10A;}_LL109:
# 444
 return 0;_LL10A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp161;if(_tmp16D->tag != 5)goto _LL10C;}_LL10B:
# 447
 return 1;_LL10C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp161;if(_tmp16E->tag != 19)goto _LL10E;}_LL10D:
# 449
 goto _LL10F;_LL10E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp161;if(_tmp16F->tag != 6)goto _LL110;}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp161;if(_tmp170->tag != 7)goto _LL112;}_LL111:
 return 1;_LL112: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp171=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp161;if(_tmp171->tag != 8)goto _LL114;else{_tmp172=(_tmp171->f1).elt_type;_tmp173=(_tmp171->f1).num_elts;}}_LL113:
# 453
 return _tmp173 != 0;_LL114: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp161;if(_tmp174->tag != 9)goto _LL116;}_LL115:
 return 0;_LL116: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp161;if(_tmp175->tag != 15)goto _LL118;}_LL117:
 return 1;_LL118: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp161;if(_tmp176->tag != 16)goto _LL11A;}_LL119:
 return 1;_LL11A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp161;if(_tmp177->tag != 11)goto _LL11C;else{_tmp178=(_tmp177->f1).aggr_info;_tmp179=(_tmp177->f1).targs;}}_LL11B: {
# 458
struct Cyc_Absyn_Aggrdecl*ad;
{union Cyc_Absyn_AggrInfoU _tmp189=_tmp178;struct Cyc_Absyn_Aggrdecl*_tmp18A;_LL138: if((_tmp189.UnknownAggr).tag != 1)goto _LL13A;_LL139: {
const char*_tmp201;void*_tmp200;(_tmp200=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp201="szof on unchecked StructType",_tag_dyneither(_tmp201,sizeof(char),29))),_tag_dyneither(_tmp200,sizeof(void*),0)));}_LL13A: if((_tmp189.KnownAggr).tag != 2)goto _LL137;_tmp18A=*((struct Cyc_Absyn_Aggrdecl**)(_tmp189.KnownAggr).val);_LL13B:
 ad=_tmp18A;_LL137:;}
# 463
if(ad->impl == 0)
return 0;
{
struct _RegionHandle _tmp18D=_new_region("temp");struct _RegionHandle*temp=& _tmp18D;_push_region(temp);
{struct Cyc_List_List*_tmp18E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,_tmp179);
{struct Cyc_List_List*_tmp18F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp18F != 0;_tmp18F=_tmp18F->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,_tmp18E,((struct Cyc_Absyn_Aggrfield*)_tmp18F->hd)->type))){int _tmp190=0;_npop_handler(0);return _tmp190;}}}{
int _tmp191=1;_npop_handler(0);return _tmp191;};}
# 467
;_pop_region(temp);}
# 472
_tmp17B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(ad))->impl))->fields;goto _LL11D;}_LL11C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp161;if(_tmp17A->tag != 12)goto _LL11E;else{_tmp17B=_tmp17A->f2;}}_LL11D:
# 474
 for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)_tmp17B->hd)->type))
return 0;}
return 1;_LL11E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp161;if(_tmp17C->tag != 13)goto _LL120;}_LL11F:
 goto _LL121;_LL120: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp161;if(_tmp17D->tag != 14)goto _LL122;}_LL121:
 return 1;_LL122: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp161;if(_tmp17E->tag != 26)goto _LL124;}_LL123:
 goto _LL125;_LL124: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp161;if(_tmp17F->tag != 20)goto _LL126;}_LL125:
 goto _LL127;_LL126: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp161;if(_tmp180->tag != 22)goto _LL128;}_LL127:
 goto _LL129;_LL128: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp161;if(_tmp181->tag != 21)goto _LL12A;}_LL129:
 goto _LL12B;_LL12A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp161;if(_tmp182->tag != 24)goto _LL12C;}_LL12B:
 goto _LL12D;_LL12C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp183=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp161;if(_tmp183->tag != 23)goto _LL12E;}_LL12D:
 goto _LL12F;_LL12E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp161;if(_tmp184->tag != 18)goto _LL130;}_LL12F:
 goto _LL131;_LL130: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp161;if(_tmp185->tag != 25)goto _LL132;}_LL131:
 return 0;_LL132: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp161;if(_tmp186->tag != 17)goto _LLF9;else{_tmp187=_tmp186->f3;}}_LL133:
# 490
 if(_tmp187 == 0  || _tmp187->kind == 0){
struct Cyc_String_pa_PrintArg_struct _tmp209;void*_tmp208[1];const char*_tmp207;void*_tmp206;(_tmp206=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp209.tag=0,((_tmp209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp208[0]=& _tmp209,Cyc_aprintf(((_tmp207="szof typedeftype %s",_tag_dyneither(_tmp207,sizeof(char),20))),_tag_dyneither(_tmp208,sizeof(void*),1)))))))),_tag_dyneither(_tmp206,sizeof(void*),0)));}
return((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp187->kind))->v)->kind == Cyc_Absyn_BoxKind;_LLF9:;}
