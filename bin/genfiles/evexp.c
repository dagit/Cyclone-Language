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
# 552
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1041 "absyn.h"
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 101
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 139 "tcenv.h"
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
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple10{unsigned int f1;int f2;};
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
union Cyc_Absyn_Cnst _tmp0=cn;enum Cyc_Absyn_Sign _tmp1;short _tmp2;enum Cyc_Absyn_Sign _tmp3;char _tmp4;switch((_tmp0.Short_c).tag){case 2: _LL1: _tmp3=((_tmp0.Char_c).val).f1;_tmp4=((_tmp0.Char_c).val).f2;_LL2:
 return Cyc_Absyn_Int_c(_tmp3,(int)_tmp4);case 4: _LL3: _tmp1=((_tmp0.Short_c).val).f1;_tmp2=((_tmp0.Short_c).val).f2;_LL4:
# 47
 return Cyc_Absyn_Int_c(_tmp1,(int)_tmp2);default: _LL5: _LL6:
 return cn;}_LL0:;}
# 52
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp5=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp7;int _tmp8;struct _tuple11 _tmp6=_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;
if(!_tmp8){
struct _tuple10 _tmp133;return(_tmp133.f1=0,((_tmp133.f2=0,_tmp133)));}{
union Cyc_Absyn_Cnst _tmpA=Cyc_Evexp_promote_const(_tmp7);union Cyc_Absyn_Cnst _tmpB=_tmpA;long long _tmpC;int _tmpD;switch((_tmpB.LongLong_c).tag){case 5: _LL8: _tmpD=((_tmpB.Int_c).val).f2;_LL9: {
struct _tuple10 _tmp134;return(_tmp134.f1=(unsigned int)_tmpD,((_tmp134.f2=1,_tmp134)));}case 6: _LLA: _tmpC=((_tmpB.LongLong_c).val).f2;_LLB: {
# 60
unsigned long long y=(unsigned long long)_tmpC >> 32;
if(y != -1  && y != 0){
struct _tuple10 _tmp135;return(_tmp135.f1=0,((_tmp135.f2=0,_tmp135)));}else{
struct _tuple10 _tmp136;return(_tmp136.f1=(unsigned int)_tmpC,((_tmp136.f2=1,_tmp136)));}}default: _LLC: _LLD:
{const char*_tmp139;void*_tmp138;(_tmp138=0,Cyc_Tcutil_terr(e->loc,((_tmp139="expecting unsigned int",_tag_dyneither(_tmp139,sizeof(char),23))),_tag_dyneither(_tmp138,sizeof(void*),0)));}{struct _tuple10 _tmp13A;return(_tmp13A.f1=0,((_tmp13A.f2=1,_tmp13A)));};}_LL7:;};}struct _tuple12{int f1;int f2;};
# 68
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp14=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp16;int _tmp17;struct _tuple11 _tmp15=_tmp14;_tmp16=_tmp15.f1;_tmp17=_tmp15.f2;
if(!_tmp17){
struct _tuple12 _tmp13B;return(_tmp13B.f1=0,((_tmp13B.f2=0,_tmp13B)));}{
union Cyc_Absyn_Cnst _tmp19=Cyc_Evexp_promote_const(_tmp16);union Cyc_Absyn_Cnst _tmp1A=_tmp19;long long _tmp1B;int _tmp1C;switch((_tmp1A.Null_c).tag){case 5: _LLF: _tmp1C=((_tmp1A.Int_c).val).f2;_LL10: {
struct _tuple12 _tmp13C;return(_tmp13C.f1=_tmp1C != 0,((_tmp13C.f2=1,_tmp13C)));}case 6: _LL11: _tmp1B=((_tmp1A.LongLong_c).val).f2;_LL12: {
struct _tuple12 _tmp13D;return(_tmp13D.f1=_tmp1B != 0,((_tmp13D.f2=1,_tmp13D)));}case 1: _LL13: _LL14: {
struct _tuple12 _tmp13E;return(_tmp13E.f1=0,((_tmp13E.f2=0,_tmp13E)));}default: _LL15: _LL16:
{const char*_tmp141;void*_tmp140;(_tmp140=0,Cyc_Tcutil_terr(e->loc,((_tmp141="expecting bool",_tag_dyneither(_tmp141,sizeof(char),15))),_tag_dyneither(_tmp140,sizeof(void*),0)));}{struct _tuple12 _tmp142;return(_tmp142.f1=0,((_tmp142.f2=0,_tmp142)));};}_LLE:;};}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 80
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp23=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp25;int _tmp26;struct _tuple11 _tmp24=_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;
if(!_tmp26){
struct _tuple11 _tmp143;return(_tmp143.f1=_tmp25,((_tmp143.f2=0,_tmp143)));}
{struct _tuple13 _tmp144;struct _tuple13 _tmp28=(_tmp144.f1=p,((_tmp144.f2=_tmp25,_tmp144)));struct _tuple13 _tmp29=_tmp28;int _tmp2A;enum Cyc_Absyn_Sign _tmp2B;int _tmp2C;enum Cyc_Absyn_Sign _tmp2D;int _tmp2E;switch(_tmp29.f1){case Cyc_Absyn_Plus: _LL18: _LL19:
 goto _LL17;case Cyc_Absyn_Minus: if(((_tmp29.f2).Int_c).tag == 5){_LL1A: _tmp2D=(((_tmp29.f2).Int_c).val).f1;_tmp2E=(((_tmp29.f2).Int_c).val).f2;_LL1B:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- _tmp2E);goto _LL17;}else{goto _LL22;}case Cyc_Absyn_Bitnot: if(((_tmp29.f2).Int_c).tag == 5){_LL1C: _tmp2B=(((_tmp29.f2).Int_c).val).f1;_tmp2C=(((_tmp29.f2).Int_c).val).f2;_LL1D:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ _tmp2C);goto _LL17;}else{goto _LL22;}case Cyc_Absyn_Not: switch(((_tmp29.f2).Null_c).tag){case 5: _LL1E: _tmp2A=(((_tmp29.f2).Int_c).val).f2;_LL1F:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,_tmp2A == 0?1: 0);goto _LL17;case 1: _LL20: _LL21:
 _tmp25=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL17;default: goto _LL22;}default: _LL22: _LL23:
{const char*_tmp147;void*_tmp146;(_tmp146=0,Cyc_Tcutil_terr(e->loc,((_tmp147="bad constant expression",_tag_dyneither(_tmp147,sizeof(char),24))),_tag_dyneither(_tmp146,sizeof(void*),0)));}goto _LL17;}_LL17:;}{
# 92
struct _tuple11 _tmp148;return(_tmp148.f1=_tmp25,((_tmp148.f2=1,_tmp148)));};}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 96
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp33=Cyc_Evexp_eval_const_exp(e1);union Cyc_Absyn_Cnst _tmp35;int _tmp36;struct _tuple11 _tmp34=_tmp33;_tmp35=_tmp34.f1;_tmp36=_tmp34.f2;{
struct _tuple11 _tmp37=Cyc_Evexp_eval_const_exp(e2);union Cyc_Absyn_Cnst _tmp39;int _tmp3A;struct _tuple11 _tmp38=_tmp37;_tmp39=_tmp38.f1;_tmp3A=_tmp38.f2;
if(!_tmp36  || !_tmp3A){
struct _tuple11 _tmp149;return(_tmp149.f1=_tmp35,((_tmp149.f2=0,_tmp149)));}
_tmp35=Cyc_Evexp_promote_const(_tmp35);
_tmp39=Cyc_Evexp_promote_const(_tmp39);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp3C=_tmp35;enum Cyc_Absyn_Sign _tmp3D;int _tmp3E;if((_tmp3C.Int_c).tag == 5){_LL25: _tmp3D=((_tmp3C.Int_c).val).f1;_tmp3E=((_tmp3C.Int_c).val).f2;_LL26:
 s1=_tmp3D;i1=_tmp3E;goto _LL24;}else{_LL27: _LL28:
{const char*_tmp14C;void*_tmp14B;(_tmp14B=0,Cyc_Tcutil_terr(e1->loc,((_tmp14C="bad constant expression",_tag_dyneither(_tmp14C,sizeof(char),24))),_tag_dyneither(_tmp14B,sizeof(void*),0)));}{struct _tuple11 _tmp14D;return(_tmp14D.f1=_tmp35,((_tmp14D.f2=1,_tmp14D)));};}_LL24:;}
# 109
{union Cyc_Absyn_Cnst _tmp42=_tmp39;enum Cyc_Absyn_Sign _tmp43;int _tmp44;if((_tmp42.Int_c).tag == 5){_LL2A: _tmp43=((_tmp42.Int_c).val).f1;_tmp44=((_tmp42.Int_c).val).f2;_LL2B:
 s2=_tmp43;i2=_tmp44;goto _LL29;}else{_LL2C: _LL2D:
{const char*_tmp150;void*_tmp14F;(_tmp14F=0,Cyc_Tcutil_terr(e2->loc,((_tmp150="bad constant expression",_tag_dyneither(_tmp150,sizeof(char),24))),_tag_dyneither(_tmp14F,sizeof(void*),0)));}{struct _tuple11 _tmp151;return(_tmp151.f1=_tmp35,((_tmp151.f2=1,_tmp151)));};}_LL29:;}
# 113
{enum Cyc_Absyn_Primop _tmp48=p;switch(_tmp48){case Cyc_Absyn_Div: _LL2F: _LL30:
 goto _LL32;case Cyc_Absyn_Mod: _LL31: _LL32:
# 116
 if(i2 == 0){
{const char*_tmp154;void*_tmp153;(_tmp153=0,Cyc_Tcutil_terr(e2->loc,((_tmp154="division by zero in constant expression",_tag_dyneither(_tmp154,sizeof(char),40))),_tag_dyneither(_tmp153,sizeof(void*),0)));}{
struct _tuple11 _tmp155;return(_tmp155.f1=_tmp35,((_tmp155.f2=1,_tmp155)));};}
# 120
goto _LL2E;default: _LL33: _LL34:
 goto _LL2E;}_LL2E:;}{
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
{struct _tuple14 _tmp156;struct _tuple14 _tmp4C=(_tmp156.f1=p,((_tmp156.f2=has_u_arg,_tmp156)));struct _tuple14 _tmp4D=_tmp4C;switch(_tmp4D.f1){case Cyc_Absyn_Plus: switch(_tmp4D.f2){case 0: _LL36: _LL37:
 i3=i1 + i2;use_i3=1;goto _LL35;case 1: _LL40: _LL41:
# 138
 u3=u1 + u2;use_u3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Times: switch(_tmp4D.f2){case 0: _LL38: _LL39:
# 134
 i3=i1 * i2;use_i3=1;goto _LL35;case 1: _LL42: _LL43:
# 139
 u3=u1 * u2;use_u3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Minus: switch(_tmp4D.f2){case 0: _LL3A: _LL3B:
# 135
 i3=i1 - i2;use_i3=1;goto _LL35;case 1: _LL44: _LL45:
# 140
 u3=u1 - u2;use_u3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Div: switch(_tmp4D.f2){case 0: _LL3C: _LL3D:
# 136
 i3=i1 / i2;use_i3=1;goto _LL35;case 1: _LL46: _LL47:
# 141
 u3=u1 / u2;use_u3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Mod: switch(_tmp4D.f2){case 0: _LL3E: _LL3F:
# 137
 i3=i1 % i2;use_i3=1;goto _LL35;case 1: _LL48: _LL49:
# 142
 u3=u1 % u2;use_u3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Eq: _LL4A: _LL4B:
 b3=i1 == i2;use_b3=1;goto _LL35;case Cyc_Absyn_Neq: _LL4C: _LL4D:
 b3=i1 != i2;use_b3=1;goto _LL35;case Cyc_Absyn_Gt: switch(_tmp4D.f2){case 0: _LL4E: _LL4F:
 b3=i1 > i2;use_b3=1;goto _LL35;case 1: _LL56: _LL57:
# 149
 b3=u1 > u2;use_b3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Lt: switch(_tmp4D.f2){case 0: _LL50: _LL51:
# 146
 b3=i1 < i2;use_b3=1;goto _LL35;case 1: _LL58: _LL59:
# 150
 b3=u1 < u2;use_b3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Gte: switch(_tmp4D.f2){case 0: _LL52: _LL53:
# 147
 b3=i1 >= i2;use_b3=1;goto _LL35;case 1: _LL5A: _LL5B:
# 151
 b3=u1 >= u2;use_b3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Lte: switch(_tmp4D.f2){case 0: _LL54: _LL55:
# 148
 b3=i1 <= i2;use_b3=1;goto _LL35;case 1: _LL5C: _LL5D:
# 152
 b3=u1 <= u2;use_b3=1;goto _LL35;default: goto _LL6A;}case Cyc_Absyn_Bitand: _LL5E: _LL5F:
 u3=u1 & u2;use_u3=1;goto _LL35;case Cyc_Absyn_Bitor: _LL60: _LL61:
 u3=u1 | u2;use_u3=1;goto _LL35;case Cyc_Absyn_Bitxor: _LL62: _LL63:
 u3=u1 ^ u2;use_u3=1;goto _LL35;case Cyc_Absyn_Bitlshift: _LL64: _LL65:
 u3=u1 << u2;use_u3=1;goto _LL35;case Cyc_Absyn_Bitlrshift: _LL66: _LL67:
 u3=u1 >> u2;use_u3=1;goto _LL35;case Cyc_Absyn_Bitarshift: _LL68: _LL69:
{const char*_tmp159;void*_tmp158;(_tmp158=0,Cyc_Tcutil_terr(e1->loc,((_tmp159=">>> NOT IMPLEMENTED",_tag_dyneither(_tmp159,sizeof(char),20))),_tag_dyneither(_tmp158,sizeof(void*),0)));}goto _LL6B;default: _LL6A: _LL6B:
{const char*_tmp15C;void*_tmp15B;(_tmp15B=0,Cyc_Tcutil_terr(e1->loc,((_tmp15C="bad constant expression",_tag_dyneither(_tmp15C,sizeof(char),24))),_tag_dyneither(_tmp15B,sizeof(void*),0)));}{struct _tuple11 _tmp15D;return(_tmp15D.f1=_tmp35,((_tmp15D.f2=1,_tmp15D)));};}_LL35:;}
# 161
if(use_i3){struct _tuple11 _tmp15E;return(_tmp15E.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3),((_tmp15E.f2=1,_tmp15E)));}
if(use_u3){struct _tuple11 _tmp15F;return(_tmp15F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3),((_tmp15F.f2=1,_tmp15F)));}
if(use_b3){struct _tuple11 _tmp160;return(_tmp160.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0),((_tmp160.f2=1,_tmp160)));}{
const char*_tmp163;void*_tmp162;(_tmp162=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp163="Evexp::eval_const_binop",_tag_dyneither(_tmp163,sizeof(char),24))),_tag_dyneither(_tmp162,sizeof(void*),0)));};};};};}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 170
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 ans;
{void*_tmp59=e->r;void*_tmp5A=_tmp59;struct Cyc_Absyn_Enumfield*_tmp5B;struct Cyc_Absyn_Enumfield*_tmp5C;void*_tmp5D;struct Cyc_Absyn_Exp*_tmp5E;void*_tmp5F;enum Cyc_Absyn_Primop _tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_Exp*_tmp62;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_Absyn_Exp*_tmp66;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp68;union Cyc_Absyn_Cnst _tmp69;switch(*((int*)_tmp5A)){case 1: _LL6D: _LL6E: {
# 174
struct _tuple11 _tmp164;return(_tmp164.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp164.f2=0,_tmp164)));}case 0: _LL6F: _tmp69=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL70: {
struct _tuple11 _tmp165;return(_tmp165.f1=_tmp69,((_tmp165.f2=1,_tmp165)));}case 5: _LL71: _tmp66=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp67=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_tmp68=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL72: {
# 177
struct _tuple12 _tmp6C=Cyc_Evexp_eval_const_bool_exp(_tmp66);int _tmp6E;int _tmp6F;struct _tuple12 _tmp6D=_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
if(!_tmp6F){
Cyc_Evexp_eval_const_exp(_tmp67);
Cyc_Evexp_eval_const_exp(_tmp68);{
struct _tuple11 _tmp166;return(_tmp166.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp166.f2=0,_tmp166)));};}
# 183
ans=_tmp6E?Cyc_Evexp_eval_const_exp(_tmp67): Cyc_Evexp_eval_const_exp(_tmp68);
goto _LL6C;}case 6: _LL73: _tmp64=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp65=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL74: {
# 186
struct _tuple12 _tmp71=Cyc_Evexp_eval_const_bool_exp(_tmp64);int _tmp73;int _tmp74;struct _tuple12 _tmp72=_tmp71;_tmp73=_tmp72.f1;_tmp74=_tmp72.f2;
if(!_tmp74){
Cyc_Evexp_eval_const_exp(_tmp65);{
struct _tuple11 _tmp167;return(_tmp167.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp167.f2=0,_tmp167)));};}
# 191
if(_tmp73)ans=Cyc_Evexp_eval_const_exp(_tmp65);else{
struct _tuple11 _tmp168;ans=((_tmp168.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp168.f2=1,_tmp168))));}
goto _LL6C;}case 7: _LL75: _tmp62=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp63=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL76: {
# 195
struct _tuple12 _tmp77=Cyc_Evexp_eval_const_bool_exp(_tmp62);int _tmp79;int _tmp7A;struct _tuple12 _tmp78=_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;
if(!_tmp7A){
Cyc_Evexp_eval_const_exp(_tmp63);{
struct _tuple11 _tmp169;return(_tmp169.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp169.f2=0,_tmp169)));};}
# 200
if(!_tmp79)ans=Cyc_Evexp_eval_const_exp(_tmp63);else{
struct _tuple11 _tmp16A;ans=((_tmp16A.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1),((_tmp16A.f2=1,_tmp16A))));}
goto _LL6C;}case 2: _LL77: _tmp60=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp61=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL78:
# 204
 if(_tmp61 == 0){
{const char*_tmp16D;void*_tmp16C;(_tmp16C=0,Cyc_Tcutil_terr(e->loc,((_tmp16D="bad static expression (no args to primop)",_tag_dyneither(_tmp16D,sizeof(char),42))),_tag_dyneither(_tmp16C,sizeof(void*),0)));}{
struct _tuple11 _tmp16E;return(_tmp16E.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp16E.f2=1,_tmp16E)));};}
# 208
if(_tmp61->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(_tmp60,(struct Cyc_Absyn_Exp*)_tmp61->hd);
goto _LL6C;}
# 212
if(((struct Cyc_List_List*)_check_null(_tmp61->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(_tmp60,(struct Cyc_Absyn_Exp*)_tmp61->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp61->tl))->hd);
goto _LL6C;}
# 216
{const char*_tmp171;void*_tmp170;(_tmp170=0,Cyc_Tcutil_terr(e->loc,((_tmp171="bad static expression (too many args to primop)",_tag_dyneither(_tmp171,sizeof(char),48))),_tag_dyneither(_tmp170,sizeof(void*),0)));}{
struct _tuple11 _tmp172;return(_tmp172.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp172.f2=1,_tmp172)));};case 38: _LL79: _tmp5F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL7A:
# 220
{void*_tmp83=Cyc_Tcutil_compress(_tmp5F);void*_tmp84=_tmp83;struct Cyc_Absyn_Exp*_tmp85;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp84)->tag == 18){_LL8A: _tmp85=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp84)->f1;_LL8B:
# 222
 e->r=_tmp85->r;
return Cyc_Evexp_eval_const_exp(_tmp85);}else{_LL8C: _LL8D:
 goto _LL89;}_LL89:;}
# 226
goto _LL7C;case 16: _LL7B: _LL7C:
 goto _LL7E;case 17: _LL7D: _LL7E:
 goto _LL80;case 18: _LL7F: _LL80:
{struct _tuple11 _tmp173;ans=((_tmp173.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp173.f2=0,_tmp173))));}goto _LL6C;case 13: _LL81: _tmp5D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp5E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL82:
# 232
 ans=Cyc_Evexp_eval_const_exp(_tmp5E);
if(ans.f2){
struct _tuple15 _tmp174;struct _tuple15 _tmp87=(_tmp174.f1=Cyc_Tcutil_compress(_tmp5D),((_tmp174.f2=ans.f1,_tmp174)));struct _tuple15 _tmp88=_tmp87;void*_tmp89;enum Cyc_Absyn_Sign _tmp8A;int _tmp8B;void*_tmp8C;enum Cyc_Absyn_Sign _tmp8D;short _tmp8E;void*_tmp8F;enum Cyc_Absyn_Sign _tmp90;char _tmp91;enum Cyc_Absyn_Sign _tmp92;enum Cyc_Absyn_Size_of _tmp93;enum Cyc_Absyn_Sign _tmp94;int _tmp95;enum Cyc_Absyn_Sign _tmp96;enum Cyc_Absyn_Size_of _tmp97;enum Cyc_Absyn_Sign _tmp98;short _tmp99;enum Cyc_Absyn_Sign _tmp9A;enum Cyc_Absyn_Size_of _tmp9B;enum Cyc_Absyn_Sign _tmp9C;char _tmp9D;switch(*((int*)_tmp88.f1)){case 6: switch(((_tmp88.f2).Int_c).tag){case 2: _LL8F: _tmp9A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp88.f1)->f1;_tmp9B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp88.f1)->f2;_tmp9C=(((_tmp88.f2).Char_c).val).f1;_tmp9D=(((_tmp88.f2).Char_c).val).f2;_LL90:
# 236
 _tmp96=_tmp9A;_tmp97=_tmp9B;_tmp98=_tmp9C;_tmp99=(short)_tmp9D;goto _LL92;case 4: _LL91: _tmp96=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp88.f1)->f1;_tmp97=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp88.f1)->f2;_tmp98=(((_tmp88.f2).Short_c).val).f1;_tmp99=(((_tmp88.f2).Short_c).val).f2;_LL92:
 _tmp92=_tmp96;_tmp93=_tmp97;_tmp94=_tmp98;_tmp95=(int)_tmp99;goto _LL94;case 5: _LL93: _tmp92=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp88.f1)->f1;_tmp93=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp88.f1)->f2;_tmp94=(((_tmp88.f2).Int_c).val).f1;_tmp95=(((_tmp88.f2).Int_c).val).f2;_LL94:
# 239
 if(_tmp92 != _tmp94)
ans.f1=Cyc_Absyn_Int_c(_tmp92,_tmp95);
goto _LL8E;default: goto _LL9B;}case 19: switch(((_tmp88.f2).Int_c).tag){case 2: _LL95: _tmp8F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp88.f1)->f1;_tmp90=(((_tmp88.f2).Char_c).val).f1;_tmp91=(((_tmp88.f2).Char_c).val).f2;_LL96:
# 243
 _tmp8C=_tmp8F;_tmp8D=_tmp90;_tmp8E=(short)_tmp91;goto _LL98;case 4: _LL97: _tmp8C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp88.f1)->f1;_tmp8D=(((_tmp88.f2).Short_c).val).f1;_tmp8E=(((_tmp88.f2).Short_c).val).f2;_LL98:
 _tmp89=_tmp8C;_tmp8A=_tmp8D;_tmp8B=(int)_tmp8E;goto _LL9A;case 5: _LL99: _tmp89=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp88.f1)->f1;_tmp8A=(((_tmp88.f2).Int_c).val).f1;_tmp8B=(((_tmp88.f2).Int_c).val).f2;_LL9A:
# 246
 if(_tmp8B < 0){
const char*_tmp177;void*_tmp176;(_tmp176=0,Cyc_Tcutil_terr(e->loc,((_tmp177="cannot cast negative number to a tag type",_tag_dyneither(_tmp177,sizeof(char),42))),_tag_dyneither(_tmp176,sizeof(void*),0)));}
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp17A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp179;Cyc_Tcutil_unify(_tmp89,(void*)((_tmp179=_cycalloc(sizeof(*_tmp179)),((_tmp179[0]=((_tmp17A.tag=18,((_tmp17A.f1=Cyc_Absyn_const_exp(ans.f1,0),_tmp17A)))),_tmp179)))));}
ans.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp8B);
goto _LL8E;default: goto _LL9B;}default: _LL9B: _LL9C:
{const char*_tmp17E;void*_tmp17D[1];struct Cyc_String_pa_PrintArg_struct _tmp17C;(_tmp17C.tag=0,((_tmp17C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp5D)),((_tmp17D[0]=& _tmp17C,Cyc_Tcutil_terr(e->loc,((_tmp17E="cannot cast to %s",_tag_dyneither(_tmp17E,sizeof(char),18))),_tag_dyneither(_tmp17D,sizeof(void*),1)))))));}goto _LL8E;}_LL8E:;}
# 253
goto _LL6C;case 32: _LL83: _tmp5C=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL84:
 _tmp5B=_tmp5C;goto _LL86;case 31: _LL85: _tmp5B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL86:
 ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmp5B))->tag));goto _LL6C;default: _LL87: _LL88:
# 257
{const char*_tmp182;void*_tmp181[1];struct Cyc_String_pa_PrintArg_struct _tmp180;(_tmp180.tag=0,((_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp181[0]=& _tmp180,Cyc_Tcutil_terr(e->loc,((_tmp182="bad static expression %s",_tag_dyneither(_tmp182,sizeof(char),25))),_tag_dyneither(_tmp181,sizeof(void*),1)))))));}{
struct _tuple11 _tmp183;return(_tmp183.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp183.f2=1,_tmp183)));};}_LL6C:;}
# 260
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmpAA=ans.f1;
{union Cyc_Absyn_Cnst _tmpAB=_tmpAA;if((_tmpAB.Int_c).tag == 5)switch(((_tmpAB.Int_c).val).f1){case Cyc_Absyn_Unsigned: switch(((_tmpAB.Int_c).val).f2){case 0: _LL9E: _LL9F: {
# 265
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
# 267
c=(void*)& uzero;
goto _LL9D;}case 1: _LLA0: _LLA1: {
# 270
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 272
c=(void*)& uone;
goto _LL9D;}default: goto _LLA6;}case Cyc_Absyn_Signed: switch(((_tmpAB.Int_c).val).f2){case 0: _LLA2: _LLA3: {
# 275
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 277
c=(void*)& szero;
goto _LL9D;}case 1: _LLA4: _LLA5: {
# 280
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
# 282
c=(void*)& sone;
goto _LL9D;}default: goto _LLA6;}default: goto _LLA6;}else{_LLA6: _LLA7:
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp186;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp185;c=(void*)((_tmp185=_cycalloc(sizeof(*_tmp185)),((_tmp185[0]=((_tmp186.tag=0,((_tmp186.f1=_tmpAA,_tmp186)))),_tmp185))));}goto _LL9D;}_LL9D:;}
# 286
e->r=c;}
# 288
return ans;}
# 291
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmpB2=e->r;void*_tmpB3=_tmpB2;struct Cyc_Absyn_Exp*_tmpB4;enum Cyc_Absyn_Primop _tmpB5;struct Cyc_List_List*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;switch(*((int*)_tmpB3)){case 32: _LLA9: _LLAA:
 goto _LLAC;case 31: _LLAB: _LLAC:
 goto _LLAE;case 16: _LLAD: _LLAE:
 goto _LLB0;case 17: _LLAF: _LLB0:
 goto _LLB2;case 18: _LLB1: _LLB2:
 goto _LLB4;case 0: _LLB3: _LLB4:
 return 1;case 5: _LLB5: _tmpBB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpBC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpBD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LLB6:
# 300
 return(Cyc_Evexp_c_can_eval(_tmpBB) && Cyc_Evexp_c_can_eval(_tmpBC)) && Cyc_Evexp_c_can_eval(_tmpBD);case 6: _LLB7: _tmpB9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpBA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LLB8:
 _tmpB7=_tmpB9;_tmpB8=_tmpBA;goto _LLBA;case 7: _LLB9: _tmpB7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpB8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LLBA:
# 303
 return Cyc_Evexp_c_can_eval(_tmpB7) && Cyc_Evexp_c_can_eval(_tmpB8);case 2: _LLBB: _tmpB5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpB6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LLBC:
# 305
 for(0;_tmpB6 != 0;_tmpB6=_tmpB6->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmpB6->hd))return 0;}
return 1;case 38: _LLBD: _LLBE:
 return 0;case 13: _LLBF: _tmpB4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LLC0:
 return Cyc_Evexp_c_can_eval(_tmpB4);default: _LLC1: _LLC2:
 return 0;}_LLA8:;}
# 314
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmpBE=e->r;void*_tmpBF=_tmpBE;switch(*((int*)_tmpBF)){case 0: _LLC4: _LLC5:
 return 1;case 5: _LLC6: _LLC7:
 return 2;case 2: _LLC8: _LLC9:
 return 3;case 16: _LLCA: _LLCB:
 goto _LLCD;case 17: _LLCC: _LLCD:
 return 4;case 18: _LLCE: _LLCF:
 return 5;case 13: _LLD0: _LLD1:
 return 6;case 6: _LLD2: _LLD3:
 return 7;case 7: _LLD4: _LLD5:
 return 8;case 38: _LLD6: _LLD7:
 return 9;default: _LLD8: _LLD9:
# 327
{const char*_tmp18A;void*_tmp189[1];struct Cyc_String_pa_PrintArg_struct _tmp188;(_tmp188.tag=0,((_tmp188.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp189[0]=& _tmp188,Cyc_Tcutil_terr(e->loc,((_tmp18A="bad static expression %s",_tag_dyneither(_tmp18A,sizeof(char),25))),_tag_dyneither(_tmp189,sizeof(void*),1)))))));}return 0;}_LLC3:;}struct _tuple16{void*f1;void*f2;};
# 331
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmpC3=Cyc_Evexp_eval_const_uint_exp(e1);unsigned int _tmpC5;int _tmpC6;struct _tuple10 _tmpC4=_tmpC3;_tmpC5=_tmpC4.f1;_tmpC6=_tmpC4.f2;{
struct _tuple10 _tmpC7=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmpC9;int _tmpCA;struct _tuple10 _tmpC8=_tmpC7;_tmpC9=_tmpC8.f1;_tmpCA=_tmpC8.f2;
if(_tmpC6  && _tmpCA)
return(int)(_tmpC5 - _tmpC9);{
int _tmpCB=Cyc_Evexp_const_exp_case_number(e1);
int _tmpCC=Cyc_Evexp_const_exp_case_number(e2);
if(_tmpCB != _tmpCC)
return _tmpCB - _tmpCC;{
struct _tuple16 _tmp18B;struct _tuple16 _tmpCD=(_tmp18B.f1=e1->r,((_tmp18B.f2=e2->r,_tmp18B)));struct _tuple16 _tmpCE=_tmpCD;void*_tmpCF;void*_tmpD0;void*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;void*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;void*_tmpD5;struct Cyc_List_List*_tmpD6;void*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;void*_tmpDC;void*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;void*_tmpDF;void*_tmpE0;enum Cyc_Absyn_Primop _tmpE1;struct Cyc_List_List*_tmpE2;enum Cyc_Absyn_Primop _tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;switch(*((int*)_tmpCE.f1)){case 5: if(((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->tag == 5){_LLDB: _tmpED=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpEE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f2;_tmpEF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f3;_tmpF0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_tmpF1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f2;_tmpF2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f3;_LLDC: {
# 345
int _tmpF3=Cyc_Evexp_const_exp_cmp(_tmpEF,_tmpF2);
if(_tmpF3 != 0)return _tmpF3;
_tmpE9=_tmpED;_tmpEA=_tmpEE;_tmpEB=_tmpF0;_tmpEC=_tmpF1;goto _LLDE;}}else{goto _LLF1;}case 6: if(((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->tag == 6){_LLDD: _tmpE9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpEA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f2;_tmpEB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_tmpEC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f2;_LLDE:
 _tmpE5=_tmpE9;_tmpE6=_tmpEA;_tmpE7=_tmpEB;_tmpE8=_tmpEC;goto _LLE0;}else{goto _LLF1;}case 7: if(((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->tag == 7){_LLDF: _tmpE5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpE6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f2;_tmpE7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_tmpE8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f2;_LLE0: {
# 350
int _tmpF4=Cyc_Evexp_const_exp_cmp(_tmpE5,_tmpE7);
if(_tmpF4 != 0)return _tmpF4;
return Cyc_Evexp_const_exp_cmp(_tmpE6,_tmpE8);}}else{goto _LLF1;}case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->tag == 2){_LLE1: _tmpE1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpE2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f2;_tmpE3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_tmpE4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f2;_LLE2: {
# 354
int _tmpF5=(int)_tmpE1 - (int)_tmpE3;
if(_tmpF5 != 0)return _tmpF5;
for(0;_tmpE2 != 0  && _tmpE4 != 0;(_tmpE2=_tmpE2->tl,_tmpE4=_tmpE4->tl)){
int _tmpF6=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmpE2->hd,(struct Cyc_Absyn_Exp*)_tmpE4->hd);
if(_tmpF6 != 0)
return _tmpF6;}
# 361
return 0;}}else{goto _LLF1;}case 16: switch(*((int*)_tmpCE.f2)){case 16: _LLE3: _tmpDF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpE0=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_LLE4:
 return Cyc_Tcutil_typecmp(_tmpDF,_tmpE0);case 17: _LLE5: _tmpDD=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpDE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_LLE6: {
# 364
void*_tmpF7=_tmpDE->topt;
if(_tmpF7 == 0){
const char*_tmp18E;void*_tmp18D;(_tmp18D=0,Cyc_Tcutil_terr(e2->loc,((_tmp18E="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp18E,sizeof(char),51))),_tag_dyneither(_tmp18D,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp(_tmpDD,(void*)_check_null(_tmpF7));}default: goto _LLF1;}case 17: switch(*((int*)_tmpCE.f2)){case 16: _LLE7: _tmpDB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpDC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_LLE8: {
# 369
void*_tmpFA=_tmpDB->topt;
if(_tmpFA == 0){
const char*_tmp191;void*_tmp190;(_tmp190=0,Cyc_Tcutil_terr(e1->loc,((_tmp191="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp191,sizeof(char),51))),_tag_dyneither(_tmp190,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmpFA),_tmpDC);}case 17: _LLE9: _tmpD9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpDA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_LLEA: {
# 374
void*_tmpFD=_tmpD9->topt;
void*_tmpFE=_tmpDA->topt;
if(_tmpFD == 0){
const char*_tmp194;void*_tmp193;(_tmp193=0,Cyc_Tcutil_terr(e1->loc,((_tmp194="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp194,sizeof(char),51))),_tag_dyneither(_tmp193,sizeof(void*),0)));}
if(_tmpFE == 0){
const char*_tmp197;void*_tmp196;(_tmp196=0,Cyc_Tcutil_terr(e2->loc,((_tmp197="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp197,sizeof(char),51))),_tag_dyneither(_tmp196,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmpFD),(void*)_check_null(_tmpFE));}default: goto _LLF1;}case 18: if(((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->tag == 18){_LLEB: _tmpD5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpD6=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f2;_tmpD7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_tmpD8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f2;_LLEC: {
# 382
int _tmp103=Cyc_Tcutil_typecmp(_tmpD5,_tmpD7);
if(_tmp103 != 0)return _tmp103;{
int _tmp104=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD6);
int _tmp105=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD8);
if(_tmp104 < _tmp105)return - 1;
if(_tmp105 < _tmp104)return 1;
for(0;_tmpD6 != 0  && _tmpD8 != 0;(_tmpD6=_tmpD6->tl,_tmpD8=_tmpD8->tl)){
struct _tuple16 _tmp198;struct _tuple16 _tmp106=(_tmp198.f1=(void*)_tmpD6->hd,((_tmp198.f2=(void*)_tmpD8->hd,_tmp198)));struct _tuple16 _tmp107=_tmp106;unsigned int _tmp108;unsigned int _tmp109;struct _dyneither_ptr*_tmp10A;struct _dyneither_ptr*_tmp10B;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp107.f1)->tag == 0){if(((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp107.f2)->tag == 1){_LLF4: _LLF5:
 return - 1;}else{_LLF8: _tmp10A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp107.f1)->f1;_tmp10B=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp107.f2)->f1;_LLF9: {
# 393
int c=Cyc_strptrcmp(_tmp10A,_tmp10B);
if(c != 0)return c;
goto _LLF3;}}}else{if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp107.f2)->tag == 0){_LLF6: _LLF7:
# 391
 return 1;}else{_LLFA: _tmp108=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp107.f1)->f1;_tmp109=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp107.f2)->f1;_LLFB: {
# 397
int c=(int)(_tmp108 - _tmp109);
if(c != 0)return c;
goto _LLF3;}}}_LLF3:;}
# 401
return 0;};}}else{goto _LLF1;}case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->tag == 13){_LLED: _tmpD1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpD2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f2;_tmpD3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_tmpD4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f2;_LLEE: {
# 403
int _tmp10D=Cyc_Tcutil_typecmp(_tmpD1,_tmpD3);
if(_tmp10D != 0)return _tmp10D;
return Cyc_Evexp_const_exp_cmp(_tmpD2,_tmpD4);}}else{goto _LLF1;}case 38: if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->tag == 38){_LLEF: _tmpCF=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpCE.f1)->f1;_tmpD0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpCE.f2)->f1;_LLF0:
# 407
 if(Cyc_Tcutil_unify(_tmpCF,_tmpD0))return 0;
return Cyc_Tcutil_typecmp(_tmpCF,_tmpD0);}else{goto _LLF1;}default: _LLF1: _LLF2: {
const char*_tmp19B;void*_tmp19A;(_tmp19A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp19B="Evexp::const_exp_cmp, unexpected case",_tag_dyneither(_tmp19B,sizeof(char),38))),_tag_dyneither(_tmp19A,sizeof(void*),0)));}}_LLDA:;};};};}
# 412
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 415
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmp111=Cyc_Evexp_eval_const_uint_exp(e1);unsigned int _tmp113;int _tmp114;struct _tuple10 _tmp112=_tmp111;_tmp113=_tmp112.f1;_tmp114=_tmp112.f2;{
struct _tuple10 _tmp115=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp117;int _tmp118;struct _tuple10 _tmp116=_tmp115;_tmp117=_tmp116.f1;_tmp118=_tmp116.f2;
if(_tmp114  && _tmp118)
return _tmp113 <= _tmp117;
return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 425
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp119=Cyc_Tcutil_compress(t);void*_tmp11A=_tmp119;struct Cyc_Absyn_Typedefdecl*_tmp11B;struct Cyc_List_List*_tmp11C;union Cyc_Absyn_AggrInfoU _tmp11D;struct Cyc_List_List*_tmp11E;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_List_List*_tmp121;struct Cyc_Absyn_Datatypefield*_tmp122;struct Cyc_Absyn_Tvar*_tmp123;switch(*((int*)_tmp11A)){case 0: _LLFD: _LLFE:
 return 0;case 2: _LLFF: _tmp123=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp11A)->f1;_LL100: {
# 429
enum Cyc_Absyn_KindQual _tmp124=(Cyc_Tcutil_tvar_kind(_tmp123,& Cyc_Tcutil_bk))->kind;enum Cyc_Absyn_KindQual _tmp125=_tmp124;if(_tmp125 == Cyc_Absyn_BoxKind){_LL13C: _LL13D:
 return 1;}else{_LL13E: _LL13F:
 return 0;}_LL13B:;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp11A)->f1 != 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp11A)->f1)->v)->kind == Cyc_Absyn_BoxKind){_LL101: _LL102:
# 433
 return 1;}else{goto _LL103;}}else{_LL103: _LL104:
 return 0;}case 3: _LL105: _LL106:
 return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp11A)->f1).field_info).KnownDatatypefield).tag == 2){_LL107: _tmp122=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp11A)->f1).field_info).KnownDatatypefield).val).f2;_LL108:
# 437
 _tmp121=_tmp122->typs;goto _LL10A;}else{_LL10B: _LL10C:
# 444
 return 0;}case 10: _LL109: _tmp121=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11A)->f1;_LL10A:
# 439
 for(0;_tmp121 != 0;_tmp121=_tmp121->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmp121->hd)).f2))
return 0;}
return 1;case 5: _LL10D: _LL10E:
# 447
 return 1;case 19: _LL10F: _LL110:
# 449
 goto _LL112;case 6: _LL111: _LL112:
 goto _LL114;case 7: _LL113: _LL114:
 return 1;case 8: _LL115: _tmp11F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11A)->f1).elt_type;_tmp120=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11A)->f1).num_elts;_LL116:
# 453
 return _tmp120 != 0;case 28: _LL117: _LL118:
 return 1;case 9: _LL119: _LL11A:
 return 0;case 15: _LL11B: _LL11C:
 return 1;case 16: _LL11D: _LL11E:
 return 1;case 11: _LL11F: _tmp11D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11A)->f1).aggr_info;_tmp11E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11A)->f1).targs;_LL120: {
# 459
struct Cyc_Absyn_Aggrdecl*ad;
{union Cyc_Absyn_AggrInfoU _tmp126=_tmp11D;struct Cyc_Absyn_Aggrdecl*_tmp127;if((_tmp126.UnknownAggr).tag == 1){_LL141: _LL142: {
const char*_tmp19E;void*_tmp19D;(_tmp19D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp19E="szof on unchecked StructType",_tag_dyneither(_tmp19E,sizeof(char),29))),_tag_dyneither(_tmp19D,sizeof(void*),0)));}}else{_LL143: _tmp127=*(_tmp126.KnownAggr).val;_LL144:
 ad=_tmp127;}_LL140:;}
# 464
if(ad->impl == 0)
return 0;
{
struct _RegionHandle _tmp12A=_new_region("temp");struct _RegionHandle*temp=& _tmp12A;_push_region(temp);
{struct Cyc_List_List*_tmp12B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,_tmp11E);
{struct Cyc_List_List*_tmp12C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,_tmp12B,((struct Cyc_Absyn_Aggrfield*)_tmp12C->hd)->type))){int _tmp12D=0;_npop_handler(0);return _tmp12D;}}}{
int _tmp12E=1;_npop_handler(0);return _tmp12E;};}
# 468
;_pop_region(temp);}
# 473
_tmp11C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(ad))->impl))->fields;goto _LL122;}case 12: _LL121: _tmp11C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11A)->f2;_LL122:
# 475
 for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)_tmp11C->hd)->type))
return 0;}
return 1;case 13: _LL123: _LL124:
 goto _LL126;case 14: _LL125: _LL126:
 return 1;case 26: _LL127: _LL128:
 goto _LL12A;case 20: _LL129: _LL12A:
 goto _LL12C;case 22: _LL12B: _LL12C:
 goto _LL12E;case 21: _LL12D: _LL12E:
 goto _LL130;case 24: _LL12F: _LL130:
 goto _LL132;case 23: _LL131: _LL132:
 goto _LL134;case 18: _LL133: _LL134:
 goto _LL136;case 27: _LL135: _LL136:
 goto _LL138;case 25: _LL137: _LL138:
 return 0;default: _LL139: _tmp11B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11A)->f3;_LL13A:
# 492
 if(_tmp11B == 0  || _tmp11B->kind == 0){
struct Cyc_String_pa_PrintArg_struct _tmp1A6;void*_tmp1A5[1];const char*_tmp1A4;void*_tmp1A3;(_tmp1A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp1A6.tag=0,((_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1A5[0]=& _tmp1A6,Cyc_aprintf(((_tmp1A4="szof typedeftype %s",_tag_dyneither(_tmp1A4,sizeof(char),20))),_tag_dyneither(_tmp1A5,sizeof(void*),1)))))))),_tag_dyneither(_tmp1A3,sizeof(void*),0)));}
return((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp11B->kind))->v)->kind == Cyc_Absyn_BoxKind;}_LLFC:;}
