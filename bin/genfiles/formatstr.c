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
# 290 "cycboot.h"
int isdigit(int);
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
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 134 "string.h"
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Relations_Reln;
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 951 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 953
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 967
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 976
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 981
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;
# 983
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
# 985
void*Cyc_Absyn_float_typ(int);
# 1017
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1022
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 156
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 40 "tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;char f5;int f6;};
# 44 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _dyneither_ptr s,int i){
# 48
unsigned long _tmp0=Cyc_strlen((struct _dyneither_ptr)s);
if(i < 0  || i >= _tmp0)return 0;{
# 52
struct Cyc_List_List*_tmp1=0;
char c=' ';
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmp2=c;switch(_tmp2){case 43: _LL1: _LL2:
 goto _LL4;case 45: _LL3: _LL4:
 goto _LL6;case 32: _LL5: _LL6:
 goto _LL8;case 35: _LL7: _LL8:
 goto _LLA;case 48: _LL9: _LLA:
 _tmp1=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=(void*)((int)c);_tmp3->tl=_tmp1;_tmp3;});continue;default: _LLB: _LLC:
 goto _LL0;}_LL0:;}
# 64
break;}
# 66
if(i >= _tmp0)return 0;
_tmp1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1);{
# 70
struct Cyc_List_List*_tmp4=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '*'){
_tmp4=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=(void*)((int)c);_tmp5->tl=_tmp4;_tmp5;});
++ i;}else{
# 76
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(isdigit((int)c))_tmp4=({struct Cyc_List_List*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->hd=(void*)((int)c);_tmp6->tl=_tmp4;_tmp6;});else{
break;}}}
# 82
if(i >= _tmp0)return 0;
_tmp4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4);{
# 86
struct Cyc_List_List*_tmp7=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '.'){
_tmp7=({struct Cyc_List_List*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->hd=(void*)((int)c);_tmp8->tl=_tmp7;_tmp8;});
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '*'){
_tmp7=({struct Cyc_List_List*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->hd=(void*)((int)c);_tmp9->tl=_tmp7;_tmp9;});
++ i;}else{
# 97
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(isdigit((int)c))_tmp7=({struct Cyc_List_List*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->hd=(void*)((int)c);_tmpA->tl=_tmp7;_tmpA;});else{
break;}}}}
# 103
if(i >= _tmp0)return 0;
_tmp7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7);{
# 108
struct Cyc_List_List*_tmpB=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmpC=c;switch(_tmpC){case 104: _LLE: _LLF:
# 112
 _tmpB=({struct Cyc_List_List*_tmpD=_region_malloc(r,sizeof(*_tmpD));_tmpD->hd=(void*)((int)c);_tmpD->tl=_tmpB;_tmpD;});
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == 'h'){_tmpB=({struct Cyc_List_List*_tmpE=_region_malloc(r,sizeof(*_tmpE));_tmpE->hd=(void*)((int)c);_tmpE->tl=_tmpB;_tmpE;});++ i;}
goto _LLD;case 108: _LL10: _LL11:
# 119
 _tmpB=({struct Cyc_List_List*_tmpF=_region_malloc(r,sizeof(*_tmpF));_tmpF->hd=(void*)((int)c);_tmpF->tl=_tmpB;_tmpF;});
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == 'l'){_tmpB=({struct Cyc_List_List*_tmp10=_region_malloc(r,sizeof(*_tmp10));_tmp10->hd=(void*)((int)c);_tmp10->tl=_tmpB;_tmp10;});++ i;}
goto _LLD;case 106: _LL12: _LL13:
 goto _LL15;case 122: _LL14: _LL15:
 goto _LL17;case 116: _LL16: _LL17:
 goto _LL19;case 76: _LL18: _LL19:
# 129
 _tmpB=({struct Cyc_List_List*_tmp11=_region_malloc(r,sizeof(*_tmp11));_tmp11->hd=(void*)((int)c);_tmp11->tl=_tmpB;_tmp11;});
++ i;
goto _LLD;default: _LL1A: _LL1B:
 goto _LLD;}_LLD:;}
# 134
if(i >= _tmp0)return 0;
_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB);
# 138
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmp12=c;switch(_tmp12){case 100: _LL1D: _LL1E:
 goto _LL20;case 105: _LL1F: _LL20:
 goto _LL22;case 111: _LL21: _LL22:
 goto _LL24;case 117: _LL23: _LL24:
 goto _LL26;case 120: _LL25: _LL26:
 goto _LL28;case 88: _LL27: _LL28:
 goto _LL2A;case 102: _LL29: _LL2A:
 goto _LL2C;case 70: _LL2B: _LL2C:
 goto _LL2E;case 101: _LL2D: _LL2E:
 goto _LL30;case 69: _LL2F: _LL30:
 goto _LL32;case 103: _LL31: _LL32:
 goto _LL34;case 71: _LL33: _LL34:
 goto _LL36;case 97: _LL35: _LL36:
 goto _LL38;case 65: _LL37: _LL38:
 goto _LL3A;case 99: _LL39: _LL3A:
 goto _LL3C;case 115: _LL3B: _LL3C:
 goto _LL3E;case 112: _LL3D: _LL3E:
 goto _LL40;case 110: _LL3F: _LL40:
 goto _LL42;case 37: _LL41: _LL42:
 goto _LL1C;default: _LL43: _LL44:
 return 0;}_LL1C:;}
# 170 "formatstr.cyc"
return({struct Cyc_Core_Opt*_tmp13=_region_malloc(r,sizeof(*_tmp13));_tmp13->v=({struct _tuple10*_tmp14=_region_malloc(r,sizeof(*_tmp14));_tmp14->f1=_tmp1;_tmp14->f2=_tmp4;_tmp14->f3=_tmp7;_tmp14->f4=_tmpB;_tmp14->f5=c;_tmp14->f6=i + 1;_tmp14;});_tmp13;});};};};};}
# 173
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 176
unsigned long _tmp15=Cyc_strlen((struct _dyneither_ptr)s);
struct Cyc_List_List*_tmp16=0;
int i;
struct _RegionHandle _tmp17=_new_region("temp");struct _RegionHandle*temp=& _tmp17;_push_region(temp);
for(i=0;i < _tmp15;++ i){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmp18=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 1);
if(_tmp18 == 0){
({void*_tmp19=0;Cyc_Tcutil_terr(loc,({const char*_tmp1A="bad format string";_tag_dyneither(_tmp1A,sizeof(char),18);}),_tag_dyneither(_tmp19,sizeof(void*),0));});{
struct Cyc_List_List*_tmp1B=0;_npop_handler(0);return _tmp1B;};}{
# 187
struct _tuple10*_tmp1C=(struct _tuple10*)_tmp18->v;struct _tuple10*_tmp1D=_tmp1C;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1F;struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp21;char _tmp22;int _tmp23;_LL46: _tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;_tmp20=_tmp1D->f3;_tmp21=_tmp1D->f4;_tmp22=_tmp1D->f5;_tmp23=_tmp1D->f6;_LL47:;
i=_tmp23 - 1;
{struct Cyc_List_List*_tmp24=_tmp21;int _tmp25;if(_tmp24 != 0){if(((struct Cyc_List_List*)_tmp24)->tl == 0){_LL49: _tmp25=(int)_tmp24->hd;if(
(_tmp25 == 'j'  || _tmp25 == 'z') || _tmp25 == 't'){_LL4A:
# 193
({struct Cyc_Int_pa_PrintArg_struct _tmp28;_tmp28.tag=1;_tmp28.f1=(unsigned long)_tmp25;({void*_tmp26[1]={& _tmp28};Cyc_Tcutil_terr(loc,({const char*_tmp27="length modifier '%c' is not supported";_tag_dyneither(_tmp27,sizeof(char),38);}),_tag_dyneither(_tmp26,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp29=0;_npop_handler(0);return _tmp29;};}else{goto _LL4B;}}else{goto _LL4B;}}else{_LL4B: _LL4C:
 goto _LL48;}_LL48:;}
# 197
{struct Cyc_List_List*_tmp2A=_tmp1F;int _tmp2B;if(_tmp2A != 0){if(((struct Cyc_List_List*)_tmp2A)->tl == 0){_LL4E: _tmp2B=(int)_tmp2A->hd;if(_tmp2B == '*'){_LL4F:
 _tmp16=({struct Cyc_List_List*_tmp2C=_region_malloc(r,sizeof(*_tmp2C));_tmp2C->hd=Cyc_Absyn_sint_typ;_tmp2C->tl=_tmp16;_tmp2C;});goto _LL4D;}else{goto _LL50;}}else{goto _LL50;}}else{_LL50: _LL51:
 goto _LL4D;}_LL4D:;}
# 201
{struct Cyc_List_List*_tmp2D=_tmp20;int _tmp2E;int _tmp2F;if(_tmp2D != 0){if(((struct Cyc_List_List*)_tmp2D)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp2D)->tl)->tl == 0){_LL53: _tmp2E=(int)_tmp2D->hd;_tmp2F=(int)(_tmp2D->tl)->hd;if(
_tmp2E == '.'  && _tmp2F == '*'){_LL54:
 _tmp16=({struct Cyc_List_List*_tmp30=_region_malloc(r,sizeof(*_tmp30));_tmp30->hd=Cyc_Absyn_sint_typ;_tmp30->tl=_tmp16;_tmp30;});goto _LL52;}else{goto _LL55;}}else{goto _LL55;}}else{goto _LL55;}}else{_LL55: _LL56:
 goto _LL52;}_LL52:;}{
# 206
void*t;
char _tmp31=_tmp22;switch(_tmp31){case 100: _LL58: _LL59:
 goto _LL5B;case 105: _LL5A: _LL5B:
# 210
{struct Cyc_List_List*f=_tmp1E;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'){
({struct Cyc_Int_pa_PrintArg_struct _tmp34;_tmp34.tag=1;_tmp34.f1=(unsigned long)((int)_tmp22);({void*_tmp32[1]={& _tmp34};Cyc_Tcutil_terr(loc,({const char*_tmp33="flag '#' is not valid with %%%c";_tag_dyneither(_tmp33,sizeof(char),32);}),_tag_dyneither(_tmp32,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp35=0;_npop_handler(0);return _tmp35;};}}}
# 215
{struct Cyc_List_List*_tmp36=_tmp21;int _tmp37;int _tmp38;int _tmp39;int _tmp3A;if(_tmp36 == 0){_LL81: _LL82:
 t=Cyc_Absyn_sint_typ;goto _LL80;}else{if(((struct Cyc_List_List*)_tmp36)->tl == 0){_LL83: _tmp3A=(int)_tmp36->hd;if(_tmp3A == 'l'){_LL84:
 t=Cyc_Absyn_slong_typ;goto _LL80;}else{_LL85: _tmp39=(int)_tmp36->hd;if(_tmp39 == 'h'){_LL86:
 t=Cyc_Absyn_sshort_typ;goto _LL80;}else{goto _LL89;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp36)->tl)->tl == 0){_LL87: _tmp37=(int)_tmp36->hd;_tmp38=(int)(_tmp36->tl)->hd;if(
_tmp37 == 'h'  && _tmp38 == 'h'){_LL88:
 t=Cyc_Absyn_schar_typ;goto _LL80;}else{goto _LL89;}}else{_LL89: _LL8A:
# 222
({struct Cyc_Int_pa_PrintArg_struct _tmp3E;_tmp3E.tag=1;_tmp3E.f1=(unsigned long)((int)_tmp22);({struct Cyc_String_pa_PrintArg_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp21));({void*_tmp3B[2]={& _tmp3D,& _tmp3E};Cyc_Tcutil_terr(loc,({const char*_tmp3C="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp3C,sizeof(char),46);}),_tag_dyneither(_tmp3B,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmp3F=0;_npop_handler(0);return _tmp3F;};}}}_LL80:;}
# 226
_tmp16=({struct Cyc_List_List*_tmp40=_region_malloc(r,sizeof(*_tmp40));_tmp40->hd=t;_tmp40->tl=_tmp16;_tmp40;});
goto _LL57;case 117: _LL5C: _LL5D:
# 229
{struct Cyc_List_List*f=_tmp1E;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'){
({void*_tmp41=0;Cyc_Tcutil_terr(loc,({const char*_tmp42="Flag '#' not valid with %%u";_tag_dyneither(_tmp42,sizeof(char),28);}),_tag_dyneither(_tmp41,sizeof(void*),0));});{
struct Cyc_List_List*_tmp43=0;_npop_handler(0);return _tmp43;};}}}
# 234
goto _LL5F;case 111: _LL5E: _LL5F:
 goto _LL61;case 120: _LL60: _LL61:
 goto _LL63;case 88: _LL62: _LL63:
# 238
{struct Cyc_List_List*_tmp44=_tmp21;int _tmp45;int _tmp46;int _tmp47;int _tmp48;if(_tmp44 == 0){_LL8C: _LL8D:
 t=Cyc_Absyn_uint_typ;goto _LL8B;}else{if(((struct Cyc_List_List*)_tmp44)->tl == 0){_LL8E: _tmp48=(int)_tmp44->hd;if(_tmp48 == 'l'){_LL8F:
 t=Cyc_Absyn_ulong_typ;goto _LL8B;}else{_LL90: _tmp47=(int)_tmp44->hd;if(_tmp47 == 'h'){_LL91:
 t=Cyc_Absyn_ushort_typ;goto _LL8B;}else{goto _LL94;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp44)->tl)->tl == 0){_LL92: _tmp45=(int)_tmp44->hd;_tmp46=(int)(_tmp44->tl)->hd;if(
_tmp45 == 'h'  && _tmp46 == 'h'){_LL93:
 t=Cyc_Absyn_uchar_typ;goto _LL8B;}else{goto _LL94;}}else{_LL94: _LL95:
# 246
({struct Cyc_Int_pa_PrintArg_struct _tmp4C;_tmp4C.tag=1;_tmp4C.f1=(unsigned long)((int)_tmp22);({struct Cyc_String_pa_PrintArg_struct _tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp21));({void*_tmp49[2]={& _tmp4B,& _tmp4C};Cyc_Tcutil_terr(loc,({const char*_tmp4A="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp4A,sizeof(char),46);}),_tag_dyneither(_tmp49,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmp4D=0;_npop_handler(0);return _tmp4D;};}}}_LL8B:;}
# 250
_tmp16=({struct Cyc_List_List*_tmp4E=_region_malloc(r,sizeof(*_tmp4E));_tmp4E->hd=t;_tmp4E->tl=_tmp16;_tmp4E;});
goto _LL57;case 102: _LL64: _LL65:
 goto _LL67;case 70: _LL66: _LL67:
 goto _LL69;case 101: _LL68: _LL69:
 goto _LL6B;case 69: _LL6A: _LL6B:
 goto _LL6D;case 103: _LL6C: _LL6D:
 goto _LL6F;case 71: _LL6E: _LL6F:
 goto _LL71;case 97: _LL70: _LL71:
 goto _LL73;case 65: _LL72: _LL73:
# 266
{struct Cyc_List_List*_tmp4F=_tmp21;int _tmp50;if(_tmp4F == 0){_LL97: _LL98:
 _tmp16=({struct Cyc_List_List*_tmp51=_region_malloc(r,sizeof(*_tmp51));_tmp51->hd=Cyc_Absyn_float_typ(1);_tmp51->tl=_tmp16;_tmp51;});goto _LL96;}else{if(((struct Cyc_List_List*)_tmp4F)->tl == 0){_LL99: _tmp50=(int)_tmp4F->hd;if(_tmp50 == 'l'){_LL9A:
# 269
 _tmp16=({struct Cyc_List_List*_tmp52=_region_malloc(r,sizeof(*_tmp52));_tmp52->hd=Cyc_Absyn_float_typ(2);_tmp52->tl=_tmp16;_tmp52;});goto _LL96;}else{goto _LL9B;}}else{_LL9B: _LL9C:
# 271
({struct Cyc_Int_pa_PrintArg_struct _tmp56;_tmp56.tag=1;_tmp56.f1=(unsigned long)((int)_tmp22);({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp21));({void*_tmp53[2]={& _tmp55,& _tmp56};Cyc_Tcutil_terr(loc,({const char*_tmp54="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp54,sizeof(char),46);}),_tag_dyneither(_tmp53,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmp57=0;_npop_handler(0);return _tmp57;};}}_LL96:;}
# 275
goto _LL57;case 99: _LL74: _LL75:
# 277
{struct Cyc_List_List*f=_tmp1E;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'  || (int)f->hd == '0'){
({struct Cyc_Int_pa_PrintArg_struct _tmp5A;_tmp5A.tag=1;_tmp5A.f1=(unsigned long)((int)f->hd);({void*_tmp58[1]={& _tmp5A};Cyc_Tcutil_terr(loc,({const char*_tmp59="flag '%c' not allowed with %%c";_tag_dyneither(_tmp59,sizeof(char),31);}),_tag_dyneither(_tmp58,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp5B=0;_npop_handler(0);return _tmp5B;};}}}
# 284
if(_tmp21 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp21));({void*_tmp5C[1]={& _tmp5E};Cyc_Tcutil_terr(loc,({const char*_tmp5D="length modifier '%s' not allowed with %%c";_tag_dyneither(_tmp5D,sizeof(char),42);}),_tag_dyneither(_tmp5C,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp5F=0;_npop_handler(0);return _tmp5F;};}
# 289
if(_tmp20 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp20));({void*_tmp60[1]={& _tmp62};Cyc_Tcutil_terr(loc,({const char*_tmp61="precision '%s' not allowed with %%c";_tag_dyneither(_tmp61,sizeof(char),36);}),_tag_dyneither(_tmp60,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp63=0;_npop_handler(0);return _tmp63;};}
# 294
_tmp16=({struct Cyc_List_List*_tmp64=_region_malloc(r,sizeof(*_tmp64));_tmp64->hd=Cyc_Absyn_sint_typ;_tmp64->tl=_tmp16;_tmp64;});
goto _LL57;case 115: _LL76: _LL77:
# 298
{struct Cyc_List_List*f=_tmp1E;for(0;f != 0;f=f->tl){
if((int)f->hd != '-'){
({void*_tmp65=0;Cyc_Tcutil_terr(loc,({const char*_tmp66="a flag not allowed with %%s";_tag_dyneither(_tmp66,sizeof(char),28);}),_tag_dyneither(_tmp65,sizeof(void*),0));});{
struct Cyc_List_List*_tmp67=0;_npop_handler(0);return _tmp67;};}}}
# 305
if(_tmp21 != 0){
({void*_tmp68=0;Cyc_Tcutil_terr(loc,({const char*_tmp69="length modifiers not allowed with %%s";_tag_dyneither(_tmp69,sizeof(char),38);}),_tag_dyneither(_tmp68,sizeof(void*),0));});{
struct Cyc_List_List*_tmp6A=0;_npop_handler(0);return _tmp6A;};}{
# 311
void*ptr=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,
Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp6C;})),
Cyc_Absyn_const_tqual(0),Cyc_Absyn_false_conref);
_tmp16=({struct Cyc_List_List*_tmp6B=_region_malloc(r,sizeof(*_tmp6B));_tmp6B->hd=ptr;_tmp6B->tl=_tmp16;_tmp6B;});
goto _LL57;};case 112: _LL78: _LL79:
# 319
 _tmp16=({struct Cyc_List_List*_tmp6D=_region_malloc(r,sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absyn_uint_typ;_tmp6D->tl=_tmp16;_tmp6D;});
goto _LL57;case 110: _LL7A: _LL7B:
# 322
{struct Cyc_List_List*f=_tmp1E;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'  || (int)f->hd == '0'){
({struct Cyc_Int_pa_PrintArg_struct _tmp70;_tmp70.tag=1;_tmp70.f1=(unsigned long)((int)f->hd);({void*_tmp6E[1]={& _tmp70};Cyc_Tcutil_terr(loc,({const char*_tmp6F="flag '%c' not allowed with %%n";_tag_dyneither(_tmp6F,sizeof(char),31);}),_tag_dyneither(_tmp6E,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp71=0;_npop_handler(0);return _tmp71;};}}}
# 327
if(_tmp20 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp20));({void*_tmp72[1]={& _tmp74};Cyc_Tcutil_terr(loc,({const char*_tmp73="precision '%s' not allowed with %%n";_tag_dyneither(_tmp73,sizeof(char),36);}),_tag_dyneither(_tmp72,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp75=0;_npop_handler(0);return _tmp75;};}{
# 332
struct Cyc_Core_Opt*_tmp76=({struct Cyc_Core_Opt*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->v=Cyc_Tcenv_lookup_type_vars(te);_tmp82;});
{struct Cyc_List_List*_tmp77=_tmp21;int _tmp78;int _tmp79;int _tmp7A;int _tmp7B;if(_tmp77 == 0){_LL9E: _LL9F:
 t=Cyc_Absyn_sint_typ;goto _LL9D;}else{if(((struct Cyc_List_List*)_tmp77)->tl == 0){_LLA0: _tmp7B=(int)_tmp77->hd;if(_tmp7B == 'l'){_LLA1:
# 336
 t=Cyc_Absyn_ulong_typ;goto _LL9D;}else{_LLA2: _tmp7A=(int)_tmp77->hd;if(_tmp7A == 'h'){_LLA3:
 t=Cyc_Absyn_sshort_typ;goto _LL9D;}else{goto _LLA6;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp77)->tl)->tl == 0){_LLA4: _tmp78=(int)_tmp77->hd;_tmp79=(int)(_tmp77->tl)->hd;if(
_tmp78 == 'h'  && _tmp79 == 'h'){_LLA5:
 t=Cyc_Absyn_schar_typ;goto _LL9D;}else{goto _LLA6;}}else{_LLA6: _LLA7:
# 341
({struct Cyc_Int_pa_PrintArg_struct _tmp7F;_tmp7F.tag=1;_tmp7F.f1=(unsigned long)((int)_tmp22);({struct Cyc_String_pa_PrintArg_struct _tmp7E;_tmp7E.tag=0;_tmp7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp21));({void*_tmp7C[2]={& _tmp7E,& _tmp7F};Cyc_Tcutil_terr(loc,({const char*_tmp7D="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp7D,sizeof(char),46);}),_tag_dyneither(_tmp7C,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmp80=0;_npop_handler(0);return _tmp80;};}}}_LL9D:;}
# 345
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmp76),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmp16=({struct Cyc_List_List*_tmp81=_region_malloc(r,sizeof(*_tmp81));_tmp81->hd=t;_tmp81->tl=_tmp16;_tmp81;});
goto _LL57;};case 37: _LL7C: _LL7D:
# 349
 if(_tmp1E != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1E));({void*_tmp83[1]={& _tmp85};Cyc_Tcutil_terr(loc,({const char*_tmp84="flags '%s' not allowed with %%%%";_tag_dyneither(_tmp84,sizeof(char),33);}),_tag_dyneither(_tmp83,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp86=0;_npop_handler(0);return _tmp86;};}
# 353
if(_tmp1F != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1F));({void*_tmp87[1]={& _tmp89};Cyc_Tcutil_terr(loc,({const char*_tmp88="width '%s' not allowed with %%%%";_tag_dyneither(_tmp88,sizeof(char),33);}),_tag_dyneither(_tmp87,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp8A=0;_npop_handler(0);return _tmp8A;};}
# 357
if(_tmp20 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp20));({void*_tmp8B[1]={& _tmp8D};Cyc_Tcutil_terr(loc,({const char*_tmp8C="precision '%s' not allowed with %%%%";_tag_dyneither(_tmp8C,sizeof(char),37);}),_tag_dyneither(_tmp8B,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp8E=0;_npop_handler(0);return _tmp8E;};}
# 361
if(_tmp21 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp21));({void*_tmp8F[1]={& _tmp91};Cyc_Tcutil_terr(loc,({const char*_tmp90="length modifier '%s' not allowed with %%%%";_tag_dyneither(_tmp90,sizeof(char),43);}),_tag_dyneither(_tmp8F,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp92=0;_npop_handler(0);return _tmp92;};}
# 365
goto _LL57;default: _LL7E: _LL7F: {
# 367
struct Cyc_List_List*_tmp93=0;_npop_handler(0);return _tmp93;}}_LL57:;};};};}{
# 370
struct Cyc_List_List*_tmp94=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16);_npop_handler(0);return _tmp94;};
# 180
;_pop_region(temp);}struct _tuple11{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};
# 381 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _dyneither_ptr s,int i){
# 383
unsigned long _tmp95=Cyc_strlen((struct _dyneither_ptr)s);
if(i < 0  || i >= _tmp95)return 0;{
# 386
int _tmp96=0;
char _tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp97 == '*'){
_tmp96=1;
++ i;
if(i >= _tmp95)return 0;}{
# 394
struct Cyc_List_List*_tmp98=0;
for(0;i < _tmp95;++ i){
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(isdigit((int)_tmp97))_tmp98=({struct Cyc_List_List*_tmp99=_region_malloc(r,sizeof(*_tmp99));_tmp99->hd=(void*)((int)_tmp97);_tmp99->tl=_tmp98;_tmp99;});else{
break;}}
# 400
if(i >= _tmp95)return 0;
_tmp98=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp98);{
# 405
struct Cyc_List_List*_tmp9A=0;
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmp9B=_tmp97;switch(_tmp9B){case 104: _LLA9: _LLAA:
# 409
 _tmp9A=({struct Cyc_List_List*_tmp9C=_region_malloc(r,sizeof(*_tmp9C));_tmp9C->hd=(void*)((int)_tmp97);_tmp9C->tl=_tmp9A;_tmp9C;});
++ i;
if(i >= _tmp95)return 0;
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp97 == 'h'){_tmp9A=({struct Cyc_List_List*_tmp9D=_region_malloc(r,sizeof(*_tmp9D));_tmp9D->hd=(void*)((int)_tmp97);_tmp9D->tl=_tmp9A;_tmp9D;});++ i;}
goto _LLA8;case 108: _LLAB: _LLAC:
# 416
 _tmp9A=({struct Cyc_List_List*_tmp9E=_region_malloc(r,sizeof(*_tmp9E));_tmp9E->hd=(void*)((int)_tmp97);_tmp9E->tl=_tmp9A;_tmp9E;});
++ i;
if(i >= _tmp95)return 0;
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp97 == 'l'){_tmp9A=({struct Cyc_List_List*_tmp9F=_region_malloc(r,sizeof(*_tmp9F));_tmp9F->hd=(void*)((int)_tmp97);_tmp9F->tl=_tmp9A;_tmp9F;});++ i;}
goto _LLA8;case 106: _LLAD: _LLAE:
 goto _LLB0;case 122: _LLAF: _LLB0:
 goto _LLB2;case 116: _LLB1: _LLB2:
 goto _LLB4;case 76: _LLB3: _LLB4:
# 426
 _tmp9A=({struct Cyc_List_List*_tmpA0=_region_malloc(r,sizeof(*_tmpA0));_tmpA0->hd=(void*)((int)_tmp97);_tmpA0->tl=_tmp9A;_tmpA0;});
++ i;
goto _LLA8;default: _LLB5: _LLB6:
 goto _LLA8;}_LLA8:;}
# 431
if(i >= _tmp95)return 0;
_tmp9A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9A);
# 435
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmpA1=_tmp97;switch(_tmpA1){case 100: _LLB8: _LLB9:
 goto _LLBB;case 105: _LLBA: _LLBB:
 goto _LLBD;case 111: _LLBC: _LLBD:
 goto _LLBF;case 117: _LLBE: _LLBF:
 goto _LLC1;case 120: _LLC0: _LLC1:
 goto _LLC3;case 88: _LLC2: _LLC3:
 goto _LLC5;case 102: _LLC4: _LLC5:
 goto _LLC7;case 70: _LLC6: _LLC7:
 goto _LLC9;case 101: _LLC8: _LLC9:
 goto _LLCB;case 69: _LLCA: _LLCB:
 goto _LLCD;case 103: _LLCC: _LLCD:
 goto _LLCF;case 71: _LLCE: _LLCF:
 goto _LLD1;case 97: _LLD0: _LLD1:
 goto _LLD3;case 65: _LLD2: _LLD3:
 goto _LLD5;case 99: _LLD4: _LLD5:
 goto _LLD7;case 115: _LLD6: _LLD7:
 goto _LLD9;case 112: _LLD8: _LLD9:
 goto _LLDB;case 110: _LLDA: _LLDB:
 goto _LLDD;case 37: _LLDC: _LLDD:
 goto _LLB7;default: _LLDE: _LLDF:
 return 0;}_LLB7:;}
# 458
return({struct Cyc_Core_Opt*_tmpA2=_region_malloc(r,sizeof(*_tmpA2));_tmpA2->v=({struct _tuple11*_tmpA3=_region_malloc(r,sizeof(*_tmpA3));_tmpA3->f1=_tmp96;_tmpA3->f2=_tmp98;_tmpA3->f3=_tmp9A;_tmpA3->f4=_tmp97;_tmpA3->f5=i + 1;_tmpA3;});_tmpA2;});};};};}
# 460
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 463
unsigned long _tmpA4=Cyc_strlen((struct _dyneither_ptr)s);
struct Cyc_List_List*_tmpA5=0;
int i;
struct _RegionHandle _tmpA6=_new_region("temp");struct _RegionHandle*temp=& _tmpA6;_push_region(temp);
for(i=0;i < _tmpA4;++ i){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmpA7=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);
if(_tmpA7 == 0){
({void*_tmpA8=0;Cyc_Tcutil_terr(loc,({const char*_tmpA9="bad format string";_tag_dyneither(_tmpA9,sizeof(char),18);}),_tag_dyneither(_tmpA8,sizeof(void*),0));});{
struct Cyc_List_List*_tmpAA=0;_npop_handler(0);return _tmpAA;};}{
# 474
struct _tuple11*_tmpAB=(struct _tuple11*)_tmpA7->v;struct _tuple11*_tmpAC=_tmpAB;int _tmpAD;struct Cyc_List_List*_tmpAE;struct Cyc_List_List*_tmpAF;char _tmpB0;int _tmpB1;_LLE1: _tmpAD=_tmpAC->f1;_tmpAE=_tmpAC->f2;_tmpAF=_tmpAC->f3;_tmpB0=_tmpAC->f4;_tmpB1=_tmpAC->f5;_LLE2:;
i=_tmpB1 - 1;
{struct Cyc_List_List*_tmpB2=_tmpAF;int _tmpB3;if(_tmpB2 != 0){if(((struct Cyc_List_List*)_tmpB2)->tl == 0){_LLE4: _tmpB3=(int)_tmpB2->hd;if(
(_tmpB3 == 'j'  || _tmpB3 == 'z') || _tmpB3 == 't'){_LLE5:
# 479
({struct Cyc_Int_pa_PrintArg_struct _tmpB6;_tmpB6.tag=1;_tmpB6.f1=(unsigned long)_tmpB3;({void*_tmpB4[1]={& _tmpB6};Cyc_Tcutil_terr(loc,({const char*_tmpB5="length modifier '%c' is not supported";_tag_dyneither(_tmpB5,sizeof(char),38);}),_tag_dyneither(_tmpB4,sizeof(void*),1));});});{
struct Cyc_List_List*_tmpB7=0;_npop_handler(0);return _tmpB7;};}else{goto _LLE6;}}else{goto _LLE6;}}else{_LLE6: _LLE7:
 goto _LLE3;}_LLE3:;}
# 483
if(_tmpAD)continue;{
void*t;
char _tmpB8=_tmpB0;switch(_tmpB8){case 100: _LLE9: _LLEA:
 goto _LLEC;case 105: _LLEB: _LLEC: {
# 488
struct Cyc_Core_Opt*_tmpB9=({struct Cyc_Core_Opt*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC5;});
{struct Cyc_List_List*_tmpBA=_tmpAF;int _tmpBB;int _tmpBC;int _tmpBD;int _tmpBE;if(_tmpBA == 0){_LL114: _LL115:
 t=Cyc_Absyn_sint_typ;goto _LL113;}else{if(((struct Cyc_List_List*)_tmpBA)->tl == 0){_LL116: _tmpBE=(int)_tmpBA->hd;if(_tmpBE == 'l'){_LL117:
 t=Cyc_Absyn_slong_typ;goto _LL113;}else{_LL118: _tmpBD=(int)_tmpBA->hd;if(_tmpBD == 'h'){_LL119:
 t=Cyc_Absyn_sshort_typ;goto _LL113;}else{goto _LL11C;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpBA)->tl)->tl == 0){_LL11A: _tmpBB=(int)_tmpBA->hd;_tmpBC=(int)(_tmpBA->tl)->hd;if(
_tmpBB == 'h'  && _tmpBC == 'h'){_LL11B: t=Cyc_Absyn_schar_typ;goto _LL113;}else{goto _LL11C;}}else{_LL11C: _LL11D:
# 495
({struct Cyc_Int_pa_PrintArg_struct _tmpC2;_tmpC2.tag=1;_tmpC2.f1=(unsigned long)((int)_tmpB0);({struct Cyc_String_pa_PrintArg_struct _tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpAF));({void*_tmpBF[2]={& _tmpC1,& _tmpC2};Cyc_Tcutil_terr(loc,({const char*_tmpC0="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpC0,sizeof(char),46);}),_tag_dyneither(_tmpBF,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmpC3=0;_npop_handler(0);return _tmpC3;};}}}_LL113:;}
# 499
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpB9),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmpC4=_region_malloc(r,sizeof(*_tmpC4));_tmpC4->hd=t;_tmpC4->tl=_tmpA5;_tmpC4;});
goto _LLE8;}case 117: _LLED: _LLEE:
 goto _LLF0;case 111: _LLEF: _LLF0:
 goto _LLF2;case 120: _LLF1: _LLF2:
 goto _LLF4;case 88: _LLF3: _LLF4: {
# 506
struct Cyc_Core_Opt*_tmpC6=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpD2;});
{struct Cyc_List_List*_tmpC7=_tmpAF;int _tmpC8;int _tmpC9;int _tmpCA;int _tmpCB;if(_tmpC7 == 0){_LL11F: _LL120:
 t=Cyc_Absyn_uint_typ;goto _LL11E;}else{if(((struct Cyc_List_List*)_tmpC7)->tl == 0){_LL121: _tmpCB=(int)_tmpC7->hd;if(_tmpCB == 'l'){_LL122:
 t=Cyc_Absyn_ulong_typ;goto _LL11E;}else{_LL123: _tmpCA=(int)_tmpC7->hd;if(_tmpCA == 'h'){_LL124:
 t=Cyc_Absyn_ushort_typ;goto _LL11E;}else{goto _LL127;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpC7)->tl)->tl == 0){_LL125: _tmpC8=(int)_tmpC7->hd;_tmpC9=(int)(_tmpC7->tl)->hd;if(
_tmpC8 == 'h'  && _tmpC9 == 'h'){_LL126: t=Cyc_Absyn_uchar_typ;goto _LL11E;}else{goto _LL127;}}else{_LL127: _LL128:
# 513
({struct Cyc_Int_pa_PrintArg_struct _tmpCF;_tmpCF.tag=1;_tmpCF.f1=(unsigned long)((int)_tmpB0);({struct Cyc_String_pa_PrintArg_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpAF));({void*_tmpCC[2]={& _tmpCE,& _tmpCF};Cyc_Tcutil_terr(loc,({const char*_tmpCD="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpCD,sizeof(char),46);}),_tag_dyneither(_tmpCC,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmpD0=0;_npop_handler(0);return _tmpD0;};}}}_LL11E:;}
# 517
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpC6),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));_tmpD1->hd=t;_tmpD1->tl=_tmpA5;_tmpD1;});
goto _LLE8;}case 102: _LLF5: _LLF6:
 goto _LLF8;case 70: _LLF7: _LLF8:
 goto _LLFA;case 101: _LLF9: _LLFA:
 goto _LLFC;case 69: _LLFB: _LLFC:
 goto _LLFE;case 103: _LLFD: _LLFE:
 goto _LL100;case 71: _LLFF: _LL100:
 goto _LL102;case 97: _LL101: _LL102:
 goto _LL104;case 65: _LL103: _LL104: {
# 528
struct Cyc_Core_Opt*_tmpD3=({struct Cyc_Core_Opt*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->v=Cyc_Tcenv_lookup_type_vars(te);_tmpDC;});
{struct Cyc_List_List*_tmpD4=_tmpAF;int _tmpD5;if(_tmpD4 == 0){_LL12A: _LL12B:
 t=Cyc_Absyn_float_typ(0);goto _LL129;}else{if(((struct Cyc_List_List*)_tmpD4)->tl == 0){_LL12C: _tmpD5=(int)_tmpD4->hd;if(_tmpD5 == 'l'){_LL12D:
# 532
 t=Cyc_Absyn_float_typ(1);goto _LL129;}else{goto _LL12E;}}else{_LL12E: _LL12F:
# 534
({struct Cyc_Int_pa_PrintArg_struct _tmpD9;_tmpD9.tag=1;_tmpD9.f1=(unsigned long)((int)_tmpB0);({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpAF));({void*_tmpD6[2]={& _tmpD8,& _tmpD9};Cyc_Tcutil_terr(loc,({const char*_tmpD7="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpD7,sizeof(char),46);}),_tag_dyneither(_tmpD6,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmpDA=0;_npop_handler(0);return _tmpDA;};}}_LL129:;}
# 538
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpD3),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmpDB=_region_malloc(r,sizeof(*_tmpDB));_tmpDB->hd=t;_tmpDB->tl=_tmpA5;_tmpDB;});
goto _LLE8;}case 99: _LL105: _LL106: {
# 543
struct Cyc_Core_Opt*_tmpDD=({struct Cyc_Core_Opt*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->v=Cyc_Tcenv_lookup_type_vars(te);_tmpDF;});
_tmpA5=({struct Cyc_List_List*_tmpDE=_region_malloc(r,sizeof(*_tmpDE));_tmpDE->hd=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpDD),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmpDE->tl=_tmpA5;_tmpDE;});
# 547
goto _LLE8;}case 115: _LL107: _LL108: {
# 549
struct Cyc_Core_Opt*_tmpE0=({struct Cyc_Core_Opt*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpE2;});
# 551
void*ptr=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpE0),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmpE1=_region_malloc(r,sizeof(*_tmpE1));_tmpE1->hd=ptr;_tmpE1->tl=_tmpA5;_tmpE1;});
goto _LLE8;}case 91: _LL109: _LL10A:
 goto _LL10C;case 112: _LL10B: _LL10C:
# 557
({struct Cyc_Int_pa_PrintArg_struct _tmpE5;_tmpE5.tag=1;_tmpE5.f1=(unsigned long)((int)_tmpB0);({void*_tmpE3[1]={& _tmpE5};Cyc_Tcutil_terr(loc,({const char*_tmpE4="%%%c is not supported";_tag_dyneither(_tmpE4,sizeof(char),22);}),_tag_dyneither(_tmpE3,sizeof(void*),1));});});{
struct Cyc_List_List*_tmpE6=0;_npop_handler(0);return _tmpE6;};case 110: _LL10D: _LL10E: {
# 560
struct Cyc_Core_Opt*_tmpE7=({struct Cyc_Core_Opt*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->v=Cyc_Tcenv_lookup_type_vars(te);_tmpF3;});
{struct Cyc_List_List*_tmpE8=_tmpAF;int _tmpE9;int _tmpEA;int _tmpEB;int _tmpEC;if(_tmpE8 == 0){_LL131: _LL132:
 t=Cyc_Absyn_sint_typ;goto _LL130;}else{if(((struct Cyc_List_List*)_tmpE8)->tl == 0){_LL133: _tmpEC=(int)_tmpE8->hd;if(_tmpEC == 'l'){_LL134:
 t=Cyc_Absyn_ulong_typ;goto _LL130;}else{_LL135: _tmpEB=(int)_tmpE8->hd;if(_tmpEB == 'h'){_LL136:
 t=Cyc_Absyn_sshort_typ;goto _LL130;}else{goto _LL139;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpE8)->tl)->tl == 0){_LL137: _tmpE9=(int)_tmpE8->hd;_tmpEA=(int)(_tmpE8->tl)->hd;if(
_tmpE9 == 'h'  && _tmpEA == 'h'){_LL138: t=Cyc_Absyn_schar_typ;goto _LL130;}else{goto _LL139;}}else{_LL139: _LL13A:
# 567
({struct Cyc_Int_pa_PrintArg_struct _tmpF0;_tmpF0.tag=1;_tmpF0.f1=(unsigned long)((int)_tmpB0);({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpAF));({void*_tmpED[2]={& _tmpEF,& _tmpF0};Cyc_Tcutil_terr(loc,({const char*_tmpEE="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpEE,sizeof(char),46);}),_tag_dyneither(_tmpED,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmpF1=0;_npop_handler(0);return _tmpF1;};}}}_LL130:;}
# 571
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpE7),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
_tmpA5=({struct Cyc_List_List*_tmpF2=_region_malloc(r,sizeof(*_tmpF2));_tmpF2->hd=t;_tmpF2->tl=_tmpA5;_tmpF2;});
goto _LLE8;}case 37: _LL10F: _LL110:
# 575
 if(_tmpAD){
({void*_tmpF4=0;Cyc_Tcutil_terr(loc,({const char*_tmpF5="Assignment suppression (*) is not allowed with %%%%";_tag_dyneither(_tmpF5,sizeof(char),52);}),_tag_dyneither(_tmpF4,sizeof(void*),0));});{
struct Cyc_List_List*_tmpF6=0;_npop_handler(0);return _tmpF6;};}
# 579
if(_tmpAE != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpAE));({void*_tmpF7[1]={& _tmpF9};Cyc_Tcutil_terr(loc,({const char*_tmpF8="width '%s' not allowed with %%%%";_tag_dyneither(_tmpF8,sizeof(char),33);}),_tag_dyneither(_tmpF7,sizeof(void*),1));});});{
struct Cyc_List_List*_tmpFA=0;_npop_handler(0);return _tmpFA;};}
# 583
if(_tmpAF != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpAF));({void*_tmpFB[1]={& _tmpFD};Cyc_Tcutil_terr(loc,({const char*_tmpFC="length modifier '%s' not allowed with %%%%";_tag_dyneither(_tmpFC,sizeof(char),43);}),_tag_dyneither(_tmpFB,sizeof(void*),1));});});{
struct Cyc_List_List*_tmpFE=0;_npop_handler(0);return _tmpFE;};}
# 587
goto _LLE8;default: _LL111: _LL112: {
# 589
struct Cyc_List_List*_tmpFF=0;_npop_handler(0);return _tmpFF;}}_LLE8:;};};};}{
# 593
struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA5);_npop_handler(0);return _tmp100;};
# 466
;_pop_region(temp);}
