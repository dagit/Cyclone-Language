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
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&
      _curr != (unsigned char *)0)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
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

# 26 "cycboot.h"
 typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 119 "core.h"
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 131 "dict.h"
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
# 69 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 75
typedef void*Cyc_Tcpat_decision_opt_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 107
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
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 211
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 213
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 222
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 368
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 373
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 511
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 537
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 544
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 562
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 964
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1111
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1116
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1176
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_RgnOrder_RgnPO;
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 38 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 49
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 65
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 69
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 80
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 96 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 114
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};struct Cyc_Interface_I;
# 33 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 36
struct Cyc_Interface_I*Cyc_Interface_empty();
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 66 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 70
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo);
# 72
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 75
void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 87
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 33
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 44
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 49
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
typedef struct _tuple11*Cyc_declarator_list_t;
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 56
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 64
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 68
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};
# 80
typedef void*Cyc_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_pointer_quals_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 33 "tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 64
void*Cyc_Tcutil_compress(void*t);
# 94
int Cyc_Tcutil_is_function_type(void*t);
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 138
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 36 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 40
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 61 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 64
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 66
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 68
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 70
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 77
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 31 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);
# 34
typedef struct Cyc_List_List*Cyc_Binding_namespace_name_t;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};
# 39
typedef void*Cyc_Binding_nsdirective_t;struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 45
typedef struct Cyc_Binding_NSCtxt*Cyc_Binding_nsctxt_t;
# 47
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 46 "interface.cyc"
void Cyc_Lex_lex_init();
# 53
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Interface_xdatatypefielddecl_t;struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};
# 65
typedef struct Cyc_Interface_Ienv*Cyc_Interface_ienv_t;struct Cyc_Interface_I{struct Cyc_Interface_Ienv*imports;struct Cyc_Interface_Ienv*exports;struct Cyc_List_List*tds;};
# 74
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->aggrdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->datatypedecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->enumdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->typedefdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->vardecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0;});}
# 84
struct Cyc_Interface_I*Cyc_Interface_empty(){
return({struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->imports=Cyc_Interface_new_ienv();_tmp1->exports=Cyc_Interface_new_ienv();_tmp1->tds=0;_tmp1;});}
# 89
struct Cyc_Interface_I*Cyc_Interface_final(){
struct Cyc_Interface_I*_tmp2=Cyc_Interface_empty();
# 92
struct Cyc_Absyn_Datatypedecl*_tmp3=({struct Cyc_Absyn_Datatypedecl*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=*Cyc_Absyn_exn_tud;_tmp9;});
_tmp3->sc=Cyc_Absyn_Public;
(_tmp2->exports)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)((_tmp2->exports)->datatypedecls,Cyc_Absyn_exn_name,_tmp3);
# 96
{struct Cyc_List_List*_tmp4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp4 != 0;_tmp4=_tmp4->tl){
struct Cyc_Absyn_Datatypefield*_tmp5=({struct Cyc_Absyn_Datatypefield*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=*((struct Cyc_Absyn_Datatypefield*)_tmp4->hd);_tmp8;});
_tmp5->sc=Cyc_Absyn_Public;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp6=({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->base=_tmp3;_tmp7->field=_tmp5;_tmp7;});
(_tmp2->exports)->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)((_tmp2->exports)->xdatatypefielddecls,((struct Cyc_Absyn_Datatypefield*)_tmp4->hd)->name,_tmp6);};}}
# 121 "interface.cyc"
_tmp2->imports=_tmp2->exports;
return _tmp2;}
# 141 "interface.cyc"
static void Cyc_Interface_err(struct _dyneither_ptr msg){
({void*_tmpA=0;Cyc_Tcutil_terr(0,msg,_tag_dyneither(_tmpA,sizeof(void*),0));});}
# 144
static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmpC;_tmpC.tag=Cyc_Core_Invalid_argument;_tmpC.f1=s;_tmpC;});_tmpB;}));}
# 147
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n));({struct Cyc_String_pa_PrintArg_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);({void*_tmpD[3]={& _tmpF,& _tmp10,& _tmp11};Cyc_aprintf(({const char*_tmpE="fields of %s %s %s have never been defined";_tag_dyneither(_tmpE,sizeof(char),43);}),_tag_dyneither(_tmpD,sizeof(void*),3));});});});}));}
# 151
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n));({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);({void*_tmp12[2]={& _tmp14,& _tmp15};Cyc_aprintf(({const char*_tmp13="the body of %s function %s has never been defined";_tag_dyneither(_tmp13,sizeof(char),50);}),_tag_dyneither(_tmp12,sizeof(void*),2));});});}));}
# 157
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr)
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 161
Cyc_Absynpp_qvar2string(name2));({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);({struct Cyc_String_pa_PrintArg_struct _tmp19;_tmp19.tag=0;_tmp19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 160
Cyc_Absynpp_qvar2string(name1));({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);({void*_tmp16[4]={& _tmp18,& _tmp19,& _tmp1A,& _tmp1B};Cyc_aprintf(({const char*_tmp17="declaration of %s %s relies on static %s %s";_tag_dyneither(_tmp17,sizeof(char),44);}),_tag_dyneither(_tmp16,sizeof(void*),4));});});});});}));}
# 165
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr)
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 169
Cyc_Absynpp_qvar2string(name2));({struct Cyc_String_pa_PrintArg_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);({struct Cyc_String_pa_PrintArg_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 168
Cyc_Absynpp_qvar2string(name1));({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);({void*_tmp1C[4]={& _tmp1E,& _tmp1F,& _tmp20,& _tmp21};Cyc_aprintf(({const char*_tmp1D="declaration of %s %s relies on fields of abstract %s %s";_tag_dyneither(_tmp1D,sizeof(char),56);}),_tag_dyneither(_tmp1C,sizeof(void*),4));});});});});}));}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 180
typedef struct Cyc_Interface_Seen*Cyc_Interface_seen_t;
# 182
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){
return({struct Cyc_Interface_Seen*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->aggrs=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp22->datatypes=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp22;});}
# 188
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);
# 191
static int Cyc_Interface_check_public_type_list(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 193
int _tmp23=1;
for(0;l != 0;l=l->tl){
if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f(l->hd)))
_tmp23=0;}
return _tmp23;}
# 200
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _tmp24;_push_handler(& _tmp24);{int _tmp26=0;if(setjmp(_tmp24.handler))_tmp26=1;if(!_tmp26){{int _tmp27=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp27;};_pop_handler();}else{void*_tmp25=(void*)_exn_thrown;void*_tmp28=_tmp25;void*_tmp29;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp28)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp29=_tmp28;_LL4:(int)_rethrow(_tmp29);}_LL0:;}};}{
int _tmp2A=1;
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp2A);
if(d->impl != 0){
struct Cyc_List_List*_tmp2B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;for(0;_tmp2B != 0;_tmp2B=_tmp2B->tl){
if(!Cyc_Interface_check_public_type(ae,seen,({const char*_tmp2C="type";_tag_dyneither(_tmp2C,sizeof(char),5);}),d->name,((struct Cyc_Absyn_Aggrfield*)_tmp2B->hd)->type))
_tmp2A=0;}}
# 210
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp2A);
return _tmp2A;};}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 214
static void*Cyc_Interface_get_type1(struct _tuple20*x){
return(*x).f2;}
# 217
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f3;}
# 221
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _tmp2D;_push_handler(& _tmp2D);{int _tmp2F=0;if(setjmp(_tmp2D.handler))_tmp2F=1;if(!_tmp2F){{int _tmp30=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);return _tmp30;};_pop_handler();}else{void*_tmp2E=(void*)_exn_thrown;void*_tmp31=_tmp2E;void*_tmp32;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp31)->tag == Cyc_Dict_Absent){_LL6: _LL7:
 goto _LL5;}else{_LL8: _tmp32=_tmp31;_LL9:(int)_rethrow(_tmp32);}_LL5:;}};}{
int _tmp33=1;
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp33);
if(d->fields != 0){
struct Cyc_List_List*_tmp34=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp34 != 0;_tmp34=_tmp34->tl){
if(!((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,({const char*_tmp35="datatype";_tag_dyneither(_tmp35,sizeof(char),9);}),d->name,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)_tmp34->hd)->typs))
# 230
_tmp33=0;}}
# 232
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp33);
return _tmp33;};}
# 236
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 240
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn != 0)
return Cyc_Interface_check_public_type(ae,seen,_tag_dyneither(0,0,0),d->name,(void*)_check_null(d->defn));else{
return 1;}}
# 247
static int Cyc_Interface_check_public_vardecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
return Cyc_Interface_check_public_type(ae,seen,({const char*_tmp36="variable";_tag_dyneither(_tmp36,sizeof(char),9);}),d->name,d->type);}
# 251
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t){
void*_tmp37=Cyc_Tcutil_compress(t);void*_tmp38=_tmp37;struct Cyc_Absyn_Datatypedecl*_tmp72;struct Cyc_Absyn_Datatypefield*_tmp71;struct Cyc_List_List*_tmp70;struct Cyc_Absyn_Datatypedecl*_tmp6F;struct Cyc_List_List*_tmp6E;struct _tuple0*_tmp6D;union Cyc_Absyn_AggrInfoU _tmp6C;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6A;void*_tmp69;struct Cyc_List_List*_tmp68;struct Cyc_Absyn_VarargInfo*_tmp67;void*_tmp66;void*_tmp65;void*_tmp64;switch(*((int*)_tmp38)){case 5: _LLB: _tmp64=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38)->f1).elt_typ;_LLC:
 _tmp65=_tmp64;goto _LLE;case 8: _LLD: _tmp65=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38)->f1).elt_type;_LLE:
 _tmp66=_tmp65;goto _LL10;case 17: _LLF: _tmp66=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp38)->f4;if(_tmp66 != 0){_LL10:
# 256
 return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp66);}else{goto _LL1D;}case 9: _LL11: _tmp69=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38)->f1).ret_typ;_tmp68=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38)->f1).args;_tmp67=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38)->f1).cyc_varargs;_LL12: {
# 260
int b=((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple8*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,_tmp68) && 
Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp69);
if(_tmp67 != 0){
struct Cyc_Absyn_VarargInfo _tmp39=*_tmp67;struct Cyc_Absyn_VarargInfo _tmp3A=_tmp39;void*_tmp3B;_LL20: _tmp3B=_tmp3A.type;_LL21:;
b=Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp3B);}
# 266
return b;}case 10: _LL13: _tmp6A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38)->f1;_LL14:
# 269
 return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,_tmp6A);case 11: _LL15: _tmp6C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38)->f1).aggr_info;_tmp6B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38)->f1).targs;_LL16: {
# 272
struct Cyc_Absyn_Aggrdecl*_tmp3C=Cyc_Absyn_get_known_aggrdecl(_tmp6C);
if(_tmp3C->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,({const char*_tmp3D="type";_tag_dyneither(_tmp3D,sizeof(char),5);}),_tmp3C->name);
return 0;}
# 277
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp6B) && 
Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp3C);}case 13: _LL17: _tmp6D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38)->f1;_LL18: {
# 281
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _tmp3E;_push_handler(& _tmp3E);{int _tmp40=0;if(setjmp(_tmp3E.handler))_tmp40=1;if(!_tmp40){ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->enumdecls,_tmp6D);;_pop_handler();}else{void*_tmp3F=(void*)_exn_thrown;void*_tmp41=_tmp3F;void*_tmp45;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp41)->tag == Cyc_Dict_Absent){_LL23: _LL24:
# 284
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp6D));({void*_tmp42[1]={& _tmp44};Cyc_aprintf(({const char*_tmp43="check_public_type (can't find enum %s)";_tag_dyneither(_tmp43,sizeof(char),39);}),_tag_dyneither(_tmp42,sizeof(void*),1));});}));}else{_LL25: _tmp45=_tmp41;_LL26:(int)_rethrow(_tmp45);}_LL22:;}};}
# 287
if(ed->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,_tmp6D,({const char*_tmp46="enum";_tag_dyneither(_tmp46,sizeof(char),5);}),ed->name);
return 0;}
# 291
return 1;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38)->f1).datatype_info).KnownDatatype).tag == 2){_LL19: _tmp6F=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38)->f1).datatype_info).KnownDatatype).val;_tmp6E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38)->f1).targs;_LL1A: {
# 294
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp47;_push_handler(& _tmp47);{int _tmp49=0;if(setjmp(_tmp47.handler))_tmp49=1;if(!_tmp49){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp6F->name);;_pop_handler();}else{void*_tmp48=(void*)_exn_thrown;void*_tmp4A=_tmp48;void*_tmp4E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4A)->tag == Cyc_Dict_Absent){_LL28: _LL29:
# 297
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp6F->name));({void*_tmp4B[1]={& _tmp4D};Cyc_aprintf(({const char*_tmp4C="check_public_type (can't find datatype %s)";_tag_dyneither(_tmp4C,sizeof(char),43);}),_tag_dyneither(_tmp4B,sizeof(void*),1));});}));}else{_LL2A: _tmp4E=_tmp4A;_LL2B:(int)_rethrow(_tmp4E);}_LL27:;}};}
# 300
if(tud->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,({const char*_tmp4F="datatype";_tag_dyneither(_tmp4F,sizeof(char),9);}),tud->name);
return 0;}
# 304
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp6E) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}else{goto _LL1D;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).field_info).KnownDatatypefield).tag == 2){_LL1B: _tmp72=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).field_info).KnownDatatypefield).val).f1;_tmp71=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).field_info).KnownDatatypefield).val).f2;_tmp70=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38)->f1).targs;_LL1C: {
# 308
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp50;_push_handler(& _tmp50);{int _tmp52=0;if(setjmp(_tmp50.handler))_tmp52=1;if(!_tmp52){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp72->name);;_pop_handler();}else{void*_tmp51=(void*)_exn_thrown;void*_tmp53=_tmp51;void*_tmp57;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp53)->tag == Cyc_Dict_Absent){_LL2D: _LL2E:
# 311
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp72->name));({void*_tmp54[1]={& _tmp56};Cyc_aprintf(({const char*_tmp55="check_public_type (can't find datatype %s and search its fields)";_tag_dyneither(_tmp55,sizeof(char),65);}),_tag_dyneither(_tmp54,sizeof(void*),1));});}));}else{_LL2F: _tmp57=_tmp53;_LL30:(int)_rethrow(_tmp57);}_LL2C:;}};}
# 314
if(tud->fields == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0;_tmp5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tud->name));({void*_tmp58[1]={& _tmp5A};Cyc_aprintf(({const char*_tmp59="check_public_type (datatype %s has no fields)";_tag_dyneither(_tmp59,sizeof(char),46);}),_tag_dyneither(_tmp58,sizeof(void*),1));});}));{
# 318
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*_tmp5B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp5B != 0;_tmp5B=_tmp5B->tl){
if(Cyc_strptrcmp((*_tmp71->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp5B->hd)->name).f2)== 0){
tuf1=(struct Cyc_Absyn_Datatypefield*)_tmp5B->hd;
break;}}}
# 325
if(tuf1 == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp71->name));({void*_tmp5C[1]={& _tmp5E};Cyc_aprintf(({const char*_tmp5D="check_public_type (can't find datatypefield %s)";_tag_dyneither(_tmp5D,sizeof(char),48);}),_tag_dyneither(_tmp5C,sizeof(void*),1));});}));{
struct Cyc_Absyn_Datatypefield*tuf=tuf1;
# 330
if(tud->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,({const char*_tmp5F="datatype";_tag_dyneither(_tmp5F,sizeof(char),9);}),tud->name);
return 0;}
# 334
if(tud->sc == Cyc_Absyn_Abstract){
Cyc_Interface_abstract_err(obj,name,({const char*_tmp60="datatype";_tag_dyneither(_tmp60,sizeof(char),9);}),tud->name);
return 0;}
# 338
if(tuf->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0;_tmp63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp61[1]={& _tmp63};Cyc_aprintf(({const char*_tmp62="field %s of";_tag_dyneither(_tmp62,sizeof(char),12);}),_tag_dyneither(_tmp61,sizeof(void*),1));});}),tud->name);
return 0;}
# 343
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp70) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};}}else{goto _LL1D;}default: _LL1D: _LL1E:
# 346
 return 1;}_LLA:;}struct _tuple21{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Tcenv_Genv*f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*f6;};
# 352
static void Cyc_Interface_extract_aggrdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Aggrdecl**dp){
# 354
struct _tuple21*_tmp73=env;struct Cyc_Interface_Ienv*_tmp84;struct Cyc_Interface_Ienv*_tmp83;int _tmp82;struct Cyc_Tcenv_Genv*_tmp81;struct Cyc_Interface_Seen*_tmp80;_LL32: _tmp84=_tmp73->f1;_tmp83=_tmp73->f2;_tmp82=_tmp73->f3;_tmp81=_tmp73->f4;_tmp80=_tmp73->f5;_LL33:;{
struct Cyc_Absyn_Aggrdecl*_tmp74=*dp;
enum Cyc_Absyn_Scope _tmp75=_tmp74->sc;enum Cyc_Absyn_Scope _tmp76=_tmp75;switch(_tmp76){case Cyc_Absyn_Static: _LL35: _LL36:
# 358
 if(_tmp82  && _tmp74->impl == 0)
Cyc_Interface_fields_err(({const char*_tmp77="static";_tag_dyneither(_tmp77,sizeof(char),7);}),({const char*_tmp78="struct/union";_tag_dyneither(_tmp78,sizeof(char),13);}),_tmp74->name);
goto _LL34;case Cyc_Absyn_Abstract: _LL37: _LL38:
# 362
 if(_tmp74->impl == 0){
if(_tmp82)
Cyc_Interface_fields_err(({const char*_tmp79="abstract";_tag_dyneither(_tmp79,sizeof(char),9);}),({const char*_tmp7A="struct/union";_tag_dyneither(_tmp7A,sizeof(char),13);}),_tmp74->name);}else{
# 366
_tmp74=({struct Cyc_Absyn_Aggrdecl*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=*_tmp74;_tmp7B;});
_tmp74->impl=0;}
# 369
if(Cyc_Interface_check_public_aggrdecl(_tmp81,_tmp80,_tmp74))
_tmp83->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp83->aggrdecls,x,_tmp74);
goto _LL34;case Cyc_Absyn_Public: _LL39: _LL3A:
# 373
 if(_tmp74->impl == 0){
Cyc_Interface_fields_err(({const char*_tmp7C="public";_tag_dyneither(_tmp7C,sizeof(char),7);}),({const char*_tmp7D="struct/union";_tag_dyneither(_tmp7D,sizeof(char),13);}),_tmp74->name);
_tmp74=({struct Cyc_Absyn_Aggrdecl*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=*_tmp74;_tmp7E;});
_tmp74->sc=Cyc_Absyn_Abstract;}
# 378
if(Cyc_Interface_check_public_aggrdecl(_tmp81,_tmp80,_tmp74))
_tmp83->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp83->aggrdecls,x,_tmp74);
goto _LL34;case Cyc_Absyn_ExternC: _LL3B: _LL3C:
 goto _LL3E;case Cyc_Absyn_Extern: _LL3D: _LL3E:
# 383
 if(Cyc_Interface_check_public_aggrdecl(_tmp81,_tmp80,_tmp74))
_tmp84->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp84->aggrdecls,x,_tmp74);
goto _LL34;default: _LL3F: _LL40:
# 387
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp7F="add_aggrdecl";_tag_dyneither(_tmp7F,sizeof(char),13);}));
goto _LL34;}_LL34:;};}
# 391
static void Cyc_Interface_extract_enumdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Enumdecl**dp){
# 393
struct _tuple21*_tmp85=env;struct Cyc_Interface_Ienv*_tmp96;struct Cyc_Interface_Ienv*_tmp95;int _tmp94;struct Cyc_Tcenv_Genv*_tmp93;struct Cyc_Interface_Seen*_tmp92;_LL42: _tmp96=_tmp85->f1;_tmp95=_tmp85->f2;_tmp94=_tmp85->f3;_tmp93=_tmp85->f4;_tmp92=_tmp85->f5;_LL43:;{
struct Cyc_Absyn_Enumdecl*_tmp86=*dp;
enum Cyc_Absyn_Scope _tmp87=_tmp86->sc;enum Cyc_Absyn_Scope _tmp88=_tmp87;switch(_tmp88){case Cyc_Absyn_Static: _LL45: _LL46:
# 397
 if(_tmp94  && _tmp86->fields == 0)
Cyc_Interface_fields_err(({const char*_tmp89="static";_tag_dyneither(_tmp89,sizeof(char),7);}),({const char*_tmp8A="enum";_tag_dyneither(_tmp8A,sizeof(char),5);}),_tmp86->name);
goto _LL44;case Cyc_Absyn_Abstract: _LL47: _LL48:
# 401
 if(_tmp86->fields == 0){
if(_tmp94)
Cyc_Interface_fields_err(({const char*_tmp8B="abstract";_tag_dyneither(_tmp8B,sizeof(char),9);}),({const char*_tmp8C="enum";_tag_dyneither(_tmp8C,sizeof(char),5);}),_tmp86->name);}else{
# 405
_tmp86=({struct Cyc_Absyn_Enumdecl*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=*_tmp86;_tmp8D;});
_tmp86->fields=0;}
# 408
if(Cyc_Interface_check_public_enumdecl(_tmp93,_tmp92,_tmp86))
_tmp95->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp95->enumdecls,x,_tmp86);
goto _LL44;case Cyc_Absyn_Public: _LL49: _LL4A:
# 412
 if(_tmp86->fields == 0){
Cyc_Interface_fields_err(({const char*_tmp8E="public";_tag_dyneither(_tmp8E,sizeof(char),7);}),({const char*_tmp8F="enum";_tag_dyneither(_tmp8F,sizeof(char),5);}),_tmp86->name);
_tmp86=({struct Cyc_Absyn_Enumdecl*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=*_tmp86;_tmp90;});
_tmp86->sc=Cyc_Absyn_Abstract;}
# 417
if(Cyc_Interface_check_public_enumdecl(_tmp93,_tmp92,_tmp86))
_tmp95->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp95->enumdecls,x,_tmp86);
goto _LL44;case Cyc_Absyn_ExternC: _LL4B: _LL4C:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4D: _LL4E:
# 422
 if(Cyc_Interface_check_public_enumdecl(_tmp93,_tmp92,_tmp86))
_tmp96->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp96->enumdecls,x,_tmp86);
goto _LL44;default: _LL4F: _LL50:
# 426
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp91="add_enumdecl";_tag_dyneither(_tmp91,sizeof(char),13);}));
goto _LL44;}_LL44:;};}
# 431
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 433
struct Cyc_Interface_Ienv*env;
{enum Cyc_Absyn_Scope _tmp97=f->sc;enum Cyc_Absyn_Scope _tmp98=_tmp97;switch(_tmp98){case Cyc_Absyn_Static: _LL52: _LL53:
 return;case Cyc_Absyn_Extern: _LL54: _LL55:
 env=i->imports;goto _LL51;case Cyc_Absyn_Public: _LL56: _LL57:
 env=i->exports;goto _LL51;default: _LL58: _LL59:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp99="add_xdatatypefielddecl";_tag_dyneither(_tmp99,sizeof(char),23);}));}_LL51:;}
# 441
env->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(env->xdatatypefielddecls,f->name,({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->base=d;_tmp9A->field=f;_tmp9A;}));}
# 446
static void Cyc_Interface_extract_datatypedecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Datatypedecl**dp){
# 448
struct _tuple21*_tmp9B=env;struct Cyc_Interface_Ienv*_tmpB3;struct Cyc_Interface_Ienv*_tmpB2;int _tmpB1;struct Cyc_Tcenv_Genv*_tmpB0;struct Cyc_Interface_Seen*_tmpAF;struct Cyc_Interface_I*_tmpAE;_LL5B: _tmpB3=_tmp9B->f1;_tmpB2=_tmp9B->f2;_tmpB1=_tmp9B->f3;_tmpB0=_tmp9B->f4;_tmpAF=_tmp9B->f5;_tmpAE=_tmp9B->f6;_LL5C:;{
struct Cyc_Absyn_Datatypedecl*_tmp9C=*dp;
# 451
enum Cyc_Absyn_Scope _tmp9D=_tmp9C->sc;enum Cyc_Absyn_Scope _tmp9E=_tmp9D;switch(_tmp9E){case Cyc_Absyn_Static: _LL5E: _LL5F:
# 453
 if((!_tmp9C->is_extensible  && _tmpB1) && _tmp9C->fields == 0)
Cyc_Interface_fields_err(({const char*_tmp9F="static";_tag_dyneither(_tmp9F,sizeof(char),7);}),({const char*_tmpA0="datatype";_tag_dyneither(_tmpA0,sizeof(char),9);}),_tmp9C->name);
# 456
goto _LL5D;case Cyc_Absyn_Abstract: _LL60: _LL61:
# 458
 if(_tmp9C->fields == 0){
if(!_tmp9C->is_extensible  && _tmpB1)
Cyc_Interface_fields_err(({const char*_tmpA1="abstract";_tag_dyneither(_tmpA1,sizeof(char),9);}),({const char*_tmpA2="datatype";_tag_dyneither(_tmpA2,sizeof(char),9);}),_tmp9C->name);}else{
# 463
_tmp9C=({struct Cyc_Absyn_Datatypedecl*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=*_tmp9C;_tmpA3;});
_tmp9C->fields=0;}
# 466
if(Cyc_Interface_check_public_datatypedecl(_tmpB0,_tmpAF,_tmp9C))
_tmpB2->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB2->datatypedecls,x,_tmp9C);
goto _LL5D;case Cyc_Absyn_Public: _LL62: _LL63:
# 470
 _tmp9C=({struct Cyc_Absyn_Datatypedecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=*_tmp9C;_tmpA4;});
if(!_tmp9C->is_extensible  && _tmp9C->fields == 0){
Cyc_Interface_fields_err(({const char*_tmpA5="public";_tag_dyneither(_tmpA5,sizeof(char),7);}),({const char*_tmpA6="datatype";_tag_dyneither(_tmpA6,sizeof(char),9);}),_tmp9C->name);
_tmp9C->sc=Cyc_Absyn_Abstract;}
# 475
if(Cyc_Interface_check_public_datatypedecl(_tmpB0,_tmpAF,_tmp9C)){
if(_tmp9C->is_extensible  && _tmp9C->fields != 0){
struct Cyc_List_List*_tmpA7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->fields))->v;
_tmp9C->fields=0;{
struct Cyc_List_List*_tmpA8=_tmpA7;for(0;_tmpA8 != 0;_tmpA8=_tmpA8->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpAE,_tmp9C,(struct Cyc_Absyn_Datatypefield*)_tmpA8->hd);}};}
# 483
_tmpB2->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB2->datatypedecls,x,_tmp9C);}
# 485
goto _LL5D;case Cyc_Absyn_ExternC: _LL64: _LL65:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpA9="extract_datatypedecl";_tag_dyneither(_tmpA9,sizeof(char),21);}));case Cyc_Absyn_Extern: _LL66: _LL67:
# 488
 if(Cyc_Interface_check_public_datatypedecl(_tmpB0,_tmpAF,_tmp9C)){
if(_tmp9C->is_extensible  && _tmp9C->fields != 0){
_tmp9C=({struct Cyc_Absyn_Datatypedecl*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=*_tmp9C;_tmpAA;});{
struct Cyc_List_List*_tmpAB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->fields))->v;
_tmp9C->fields=0;{
struct Cyc_List_List*_tmpAC=_tmpAB;for(0;_tmpAC != 0;_tmpAC=_tmpAC->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpAE,_tmp9C,(struct Cyc_Absyn_Datatypefield*)_tmpAC->hd);}};};}
# 497
_tmpB3->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB3->datatypedecls,x,_tmp9C);}
# 499
goto _LL5D;default: _LL68: _LL69:
# 501
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpAD="add_datatypedecl";_tag_dyneither(_tmpAD,sizeof(char),17);}));
goto _LL5D;}_LL5D:;};}
# 506
static void Cyc_Interface_extract_typedef(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d){
# 508
struct _tuple21*_tmpB4=env;struct Cyc_Interface_Ienv*_tmpB7;struct Cyc_Tcenv_Genv*_tmpB6;struct Cyc_Interface_Seen*_tmpB5;_LL6B: _tmpB7=_tmpB4->f2;_tmpB6=_tmpB4->f4;_tmpB5=_tmpB4->f5;_LL6C:;
if(Cyc_Interface_check_public_typedefdecl(_tmpB6,_tmpB5,d))
_tmpB7->typedefdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpB7->typedefdecls,x,d);}struct _tuple22{void*f1;int f2;};
# 513
static void Cyc_Interface_extract_ordinarie(struct _tuple21*env,struct _tuple0*x,struct _tuple22*v){
# 515
struct _tuple21*_tmpB8=env;struct Cyc_Interface_Ienv*_tmpCB;struct Cyc_Interface_Ienv*_tmpCA;int _tmpC9;struct Cyc_Tcenv_Genv*_tmpC8;struct Cyc_Interface_Seen*_tmpC7;_LL6E: _tmpCB=_tmpB8->f1;_tmpCA=_tmpB8->f2;_tmpC9=_tmpB8->f3;_tmpC8=_tmpB8->f4;_tmpC7=_tmpB8->f5;_LL6F:;{
# 517
void*_tmpB9=(*v).f1;void*_tmpBA=_tmpB9;void*_tmpC6;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpBA)->tag == 0){_LL71: _tmpC6=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpBA)->f1;_LL72:
# 519
{void*_tmpBB=_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Fndecl*_tmpC4;switch(*((int*)_tmpBB)){case 2: _LL76: _tmpC4=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpBB)->f1;_LL77: {
# 521
struct Cyc_Absyn_Vardecl*_tmpBC=({struct Cyc_Absyn_Vardecl*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->sc=_tmpC4->sc;_tmpBD->name=_tmpC4->name;_tmpBD->tq=
# 524
Cyc_Absyn_empty_tqual(0);_tmpBD->type=(void*)_check_null(_tmpC4->cached_typ);_tmpBD->initializer=0;_tmpBD->rgn=0;_tmpBD->attributes=0;_tmpBD->escapes=0;_tmpBD;});
# 531
_tmpC9=0;
_tmpC5=_tmpBC;goto _LL79;}case 1: _LL78: _tmpC5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpBB)->f1;_LL79:
# 534
 if(_tmpC5->initializer != 0){
_tmpC5=({struct Cyc_Absyn_Vardecl*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=*_tmpC5;_tmpBE;});
_tmpC5->initializer=0;}
# 538
{enum Cyc_Absyn_Scope _tmpBF=_tmpC5->sc;enum Cyc_Absyn_Scope _tmpC0=_tmpBF;switch(_tmpC0){case Cyc_Absyn_Static: _LL7D: _LL7E:
# 540
 if(_tmpC9  && Cyc_Tcutil_is_function_type(_tmpC5->type))
Cyc_Interface_body_err(({const char*_tmpC1="static";_tag_dyneither(_tmpC1,sizeof(char),7);}),_tmpC5->name);
# 543
goto _LL7C;case Cyc_Absyn_Register: _LL7F: _LL80:
 goto _LL82;case Cyc_Absyn_Abstract: _LL81: _LL82:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpC2="extract_ordinarie";_tag_dyneither(_tmpC2,sizeof(char),18);}));case Cyc_Absyn_Public: _LL83: _LL84:
# 547
 if(_tmpC9  && Cyc_Tcutil_is_function_type(_tmpC5->type))
Cyc_Interface_body_err(({const char*_tmpC3="public";_tag_dyneither(_tmpC3,sizeof(char),7);}),_tmpC5->name);
# 550
if(Cyc_Interface_check_public_vardecl(_tmpC8,_tmpC7,_tmpC5))
_tmpCA->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpCA->vardecls,x,_tmpC5);
goto _LL7C;case Cyc_Absyn_ExternC: _LL85: _LL86:
 goto _LL88;default: _LL87: _LL88:
# 555
 if(Cyc_Interface_check_public_vardecl(_tmpC8,_tmpC7,_tmpC5))
_tmpCB->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpCB->vardecls,x,_tmpC5);
goto _LL7C;}_LL7C:;}
# 559
goto _LL75;default: _LL7A: _LL7B:
 goto _LL75;}_LL75:;}
# 562
goto _LL70;}else{_LL73: _LL74:
 goto _LL70;}_LL70:;};}struct _tuple23{void*f1;void*f2;};
# 567
static struct Cyc_List_List*Cyc_Interface_remove_decl_from_list(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*d){
if(l == 0)return 0;
{struct _tuple23 _tmpCC=({struct _tuple23 _tmpD5;_tmpD5.f1=d->r;_tmpD5.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmpD5;});struct _tuple23 _tmpCD=_tmpCC;struct Cyc_Absyn_Vardecl**_tmpD4;struct Cyc_Absyn_Vardecl*_tmpD3;struct Cyc_Absyn_Enumdecl*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_Absyn_Typedefdecl*_tmpD0;struct Cyc_Absyn_Typedefdecl*_tmpCF;switch(*((int*)_tmpCD.f1)){case 8: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCD.f2)->tag == 8){_LL8A: _tmpD0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCD.f1)->f1;_tmpCF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCD.f2)->f1;_LL8B:
# 571
 if(Cyc_Absyn_qvar_cmp(_tmpD0->name,_tmpCF->name)!= 0)goto _LL89;
return Cyc_Interface_remove_decl_from_list(l->tl,d);}else{goto _LL90;}case 7: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCD.f2)->tag == 7){_LL8C: _tmpD2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCD.f1)->f1;_tmpD1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCD.f2)->f1;_LL8D:
# 574
 if(Cyc_Absyn_qvar_cmp(_tmpD2->name,_tmpD1->name)!= 0)goto _LL89;
if(_tmpD2->sc == Cyc_Absyn_Extern)_tmpD2->sc=_tmpD1->sc;
return Cyc_Interface_remove_decl_from_list(l->tl,d);
goto _LL89;}else{goto _LL90;}case 0: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCD.f2)->tag == 0){_LL8E: _tmpD4=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCD.f1)->f1;_tmpD3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCD.f2)->f1;_LL8F:
# 579
 if(Cyc_Absyn_qvar_cmp((*_tmpD4)->name,_tmpD3->name)!= 0)goto _LL89;
if((*_tmpD4)->sc == Cyc_Absyn_Extern)*_tmpD4=({struct Cyc_Absyn_Vardecl*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=*_tmpD3;_tmpCE;});
return Cyc_Interface_remove_decl_from_list(l->tl,d);}else{goto _LL90;}default: _LL90: _LL91:
# 583
 goto _LL89;}_LL89:;}
# 585
return({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->hd=(struct Cyc_Absyn_Decl*)l->hd;_tmpD6->tl=Cyc_Interface_remove_decl_from_list(l->tl,d);_tmpD6;});}
# 588
static struct Cyc_List_List*Cyc_Interface_uniqify_decl_list(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
if(accum == 0)return({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=d;_tmpD7->tl=0;_tmpD7;});{
struct Cyc_List_List*l;
for(l=accum;l != 0;l=l->tl){
struct _tuple23 _tmpD8=({struct _tuple23 _tmpE6;_tmpE6.f1=d->r;_tmpE6.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmpE6;});struct _tuple23 _tmpD9=_tmpD8;struct _dyneither_ptr*_tmpE5;struct Cyc_List_List**_tmpE4;struct _dyneither_ptr*_tmpE3;struct Cyc_List_List*_tmpE2;struct Cyc_Absyn_Enumdecl*_tmpE1;struct Cyc_Absyn_Enumdecl*_tmpE0;struct Cyc_Absyn_Typedefdecl*_tmpDF;struct Cyc_Absyn_Typedefdecl*_tmpDE;struct Cyc_Absyn_Vardecl*_tmpDD;struct Cyc_Absyn_Vardecl**_tmpDC;switch(*((int*)_tmpD9.f1)){case 0: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->tag == 0){_LL93: _tmpDD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD9.f1)->f1;_tmpDC=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->f1;_LL94:
# 594
 if(Cyc_Absyn_qvar_cmp(_tmpDD->name,(*_tmpDC)->name)!= 0)goto _LL92;
if((*_tmpDC)->sc == Cyc_Absyn_Extern)*_tmpDC=({struct Cyc_Absyn_Vardecl*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=*_tmpDD;_tmpDA;});
return accum;}else{goto _LL9B;}case 8: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->tag == 8){_LL95: _tmpDF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD9.f1)->f1;_tmpDE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->f1;_LL96:
# 598
 if(Cyc_Absyn_qvar_cmp(_tmpDF->name,_tmpDE->name)!= 0)goto _LL92;
return accum;}else{goto _LL9B;}case 7: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->tag == 7){_LL97: _tmpE1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD9.f1)->f1;_tmpE0=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->f1;_LL98:
# 601
 if(Cyc_Absyn_qvar_cmp(_tmpE1->name,_tmpE0->name)!= 0)goto _LL92;
return accum;}else{goto _LL9B;}case 9: if(((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->tag == 9){_LL99: _tmpE5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD9.f1)->f1;_tmpE4=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD9.f1)->f2;_tmpE3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->f1;_tmpE2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD9.f2)->f2;_LL9A:
# 604
 if(Cyc_strptrcmp(_tmpE5,_tmpE3)!= 0)goto _LL92;{
struct Cyc_List_List*_tmpDB=_tmpE2;
for(0;_tmpDB != 0;_tmpDB=_tmpDB->tl){
*_tmpE4=Cyc_Interface_remove_decl_from_list(*_tmpE4,(struct Cyc_Absyn_Decl*)_tmpDB->hd);}
# 609
goto _LL92;};}else{goto _LL9B;}default: _LL9B: _LL9C:
# 611
 goto _LL92;}_LL92:;}
# 614
return({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->hd=d;_tmpE7->tl=accum;_tmpE7;});};}
# 617
static struct Cyc_List_List*Cyc_Interface_filterstatics(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
{void*_tmpE8=d->r;void*_tmpE9=_tmpE8;struct Cyc_List_List*_tmpFF;struct _dyneither_ptr*_tmpFE;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Typedefdecl*_tmpFC;struct Cyc_Absyn_Enumdecl*_tmpFB;struct Cyc_Absyn_Datatypedecl*_tmpFA;struct Cyc_Absyn_Aggrdecl*_tmpF9;struct Cyc_Absyn_Fndecl*_tmpF8;struct Cyc_Absyn_Vardecl*_tmpF7;switch(*((int*)_tmpE9)){case 0: _LL9E: _tmpF7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpE9)->f1;_LL9F:
# 620
 if(_tmpF7->sc == Cyc_Absyn_ExternC)return accum;
if(_tmpF7->sc == Cyc_Absyn_Static)return accum;{
struct Cyc_Absyn_Vardecl*_tmpEA=({struct Cyc_Absyn_Vardecl*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=*_tmpF7;_tmpEE;});
_tmpEA->initializer=0;
if(Cyc_Tcutil_is_function_type(_tmpEA->type) && _tmpEA->sc != Cyc_Absyn_Extern)
_tmpEA->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpED;_tmpED.tag=0;_tmpED.f1=_tmpEA;_tmpED;});_tmpEC;}),0);_tmpEB->tl=accum;_tmpEB;});};case 1: _LLA0: _tmpF8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpE9)->f1;_LLA1:
# 628
 if(_tmpF8->sc == Cyc_Absyn_Static)return accum;
if(_tmpF8->sc == Cyc_Absyn_ExternC)return accum;{
struct Cyc_Absyn_Vardecl*_tmpEF=Cyc_Absyn_new_vardecl(_tmpF8->name,(void*)_check_null(_tmpF8->cached_typ),0);
_tmpEF->sc=_tmpF8->sc;
return({struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=_tmpEF;_tmpF2;});_tmpF1;}),0);_tmpF0->tl=accum;_tmpF0;});};case 5: _LLA2: _tmpF9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpE9)->f1;_LLA3:
# 635
 if(_tmpF9->sc == Cyc_Absyn_ExternC)return accum;
goto _LL9D;case 6: _LLA4: _tmpFA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpE9)->f1;_LLA5:
# 639
 if(_tmpFA->sc == Cyc_Absyn_ExternC)return accum;
goto _LL9D;case 7: _LLA6: _tmpFB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpE9)->f1;_LLA7:
# 642
 if(_tmpFB->sc == Cyc_Absyn_Static)return accum;
if(_tmpFB->sc == Cyc_Absyn_ExternC)return accum;
goto _LL9D;case 8: _LLA8: _tmpFC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpE9)->f1;_LLA9:
# 646
 goto _LL9D;case 9: _LLAA: _tmpFE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpE9)->f1;_tmpFD=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpE9)->f2;_LLAB: {
# 648
struct Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,_tmpFD));
return({struct Cyc_List_List*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpF6;_tmpF6.tag=9;_tmpF6.f1=_tmpFE;_tmpF6.f2=_tmpF3;_tmpF6;});_tmpF5;}),0);_tmpF4->tl=accum;_tmpF4;});}case 10: _LLAC: _tmpFF=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpE9)->f2;_LLAD:
# 651
 return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,_tmpFF),accum);case 2: _LLAE: _LLAF:
 goto _LLB1;case 3: _LLB0: _LLB1:
 goto _LLB3;case 4: _LLB2: _LLB3:
 goto _LLB5;case 11: _LLB4: _LLB5:
 goto _LLB7;case 12: _LLB6: _LLB7:
 goto _LLB9;case 13: _LLB8: _LLB9:
 goto _LLBB;default: _LLBA: _LLBB:
 return accum;}_LL9D:;}
# 660
return({struct Cyc_List_List*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->hd=({struct Cyc_Absyn_Decl*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=*d;_tmp101;});_tmp100->tl=accum;_tmp100;});}struct _tuple24{struct Cyc_Interface_I*f1;int f2;struct Cyc_Tcenv_Genv*f3;struct Cyc_Interface_Seen*f4;};
# 663
static void Cyc_Interface_extract_f(struct _tuple24*env_f){
struct _tuple24*_tmp102=env_f;struct Cyc_Interface_I*_tmp108;int _tmp107;struct Cyc_Tcenv_Genv*_tmp106;struct Cyc_Interface_Seen*_tmp105;_LLBD: _tmp108=_tmp102->f1;_tmp107=_tmp102->f2;_tmp106=_tmp102->f3;_tmp105=_tmp102->f4;_LLBE:;{
struct _tuple21 _tmp103=({struct _tuple21 _tmp104;_tmp104.f1=_tmp108->imports;_tmp104.f2=_tmp108->exports;_tmp104.f3=_tmp107;_tmp104.f4=_tmp106;_tmp104.f5=_tmp105;_tmp104.f6=_tmp108;_tmp104;});
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmp103,_tmp106->aggrdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,& _tmp103,_tmp106->datatypedecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmp103,_tmp106->enumdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmp103,_tmp106->typedefs);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct _tuple22*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmp103,_tmp106->ordinaries);};}
# 673
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Tcenv_Genv*ae,int check_complete_defs,struct Cyc_List_List*tds){
struct _tuple24 env=({struct _tuple24 _tmp10A;_tmp10A.f1=
Cyc_Interface_empty();_tmp10A.f2=check_complete_defs;_tmp10A.f3=ae;_tmp10A.f4=Cyc_Interface_new_seen();_tmp10A;});
Cyc_Interface_extract_f(& env);{
struct Cyc_Interface_I*_tmp109=env.f1;
_tmp109->tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,tds));
_tmp109->tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,0,_tmp109->tds));
return _tmp109;};}
# 683
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae,struct Cyc_List_List*tds){
return Cyc_Interface_gen_extract(ae,1,tds);}
# 689
inline static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){
({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg2);({void*_tmp10B[1]={& _tmp10D};Cyc_Tcdecl_merr(0,msg1,({const char*_tmp10C="%s";_tag_dyneither(_tmp10C,sizeof(char),3);}),_tag_dyneither(_tmp10B,sizeof(void*),1));});});}struct _tuple25{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 693
static void Cyc_Interface_incl_dict_f(struct _tuple25*env,struct _tuple0*x,void*y1){
# 700
struct _tuple25*_tmp10E=env;int*_tmp11D;struct Cyc_Dict_Dict _tmp11C;int(*_tmp11B)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp11A;struct _dyneither_ptr*_tmp119;_LLC0: _tmp11D=(int*)& _tmp10E->f1;_tmp11C=_tmp10E->f2;_tmp11B=_tmp10E->f3;_tmp11A=_tmp10E->f4;_tmp119=_tmp10E->f5;_LLC1:;{
struct _handler_cons _tmp10F;_push_handler(& _tmp10F);{int _tmp111=0;if(setjmp(_tmp10F.handler))_tmp111=1;if(!_tmp111){
{void*_tmp112=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp11C,x);
if(!_tmp11B(y1,_tmp112,_tmp119))*_tmp11D=0;}
# 702
;_pop_handler();}else{void*_tmp110=(void*)_exn_thrown;void*_tmp113=_tmp110;void*_tmp118;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp113)->tag == Cyc_Dict_Absent){_LLC3: _LLC4:
# 706
 Cyc_Interface_check_err(_tmp119,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp117;_tmp117.tag=0;_tmp117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0;_tmp116.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11A);({void*_tmp114[2]={& _tmp116,& _tmp117};Cyc_aprintf(({const char*_tmp115="%s %s is missing";_tag_dyneither(_tmp115,sizeof(char),17);}),_tag_dyneither(_tmp114,sizeof(void*),2));});});}));
*_tmp11D=0;
goto _LLC2;}else{_LLC5: _tmp118=_tmp113;_LLC6:(int)_rethrow(_tmp118);}_LLC2:;}};};}
# 712
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 717
struct _tuple25 _tmp11E=({struct _tuple25 _tmp11F;_tmp11F.f1=1;_tmp11F.f2=dic2;_tmp11F.f3=incl_f;_tmp11F.f4=t;_tmp11F.f5=msg;_tmp11F;});
((void(*)(void(*f)(struct _tuple25*,struct _tuple0*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp11E,dic1);
return _tmp11E.f1;}
# 734 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmp120=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmp120 == 0)return 0;if(d0 != _tmp120){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmp121[1]={& _tmp123};Cyc_aprintf(({const char*_tmp122="declaration of type %s discloses too much information";_tag_dyneither(_tmp122,sizeof(char),54);}),_tag_dyneither(_tmp121,sizeof(void*),1));});}));return 0;}return 1;}
# 736
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmp124=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0,msg);if(_tmp124 == 0)return 0;if(d0 != _tmp124){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmp125[1]={& _tmp127};Cyc_aprintf(({const char*_tmp126="declaration of datatype %s discloses too much information";_tag_dyneither(_tmp126,sizeof(char),58);}),_tag_dyneither(_tmp125,sizeof(void*),1));});}));return 0;}return 1;}
# 738
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*_tmp128=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp128 == 0)return 0;if(d0 != _tmp128){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmp129[1]={& _tmp12B};Cyc_aprintf(({const char*_tmp12A="declaration of enum %s discloses too much information";_tag_dyneither(_tmp12A,sizeof(char),54);}),_tag_dyneither(_tmp129,sizeof(void*),1));});}));return 0;}return 1;}
# 740
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*_tmp12C=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp12C == 0)return 0;if(d0 != _tmp12C){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmp12D[1]={& _tmp12F};Cyc_aprintf(({const char*_tmp12E="declaration of variable %s discloses too much information";_tag_dyneither(_tmp12E,sizeof(char),58);}),_tag_dyneither(_tmp12D,sizeof(void*),1));});}));return 0;}return 1;}
# 742
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp130=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp130 == 0)return 0;if(d0 != _tmp130){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmp131[1]={& _tmp133};Cyc_aprintf(({const char*_tmp132="declaration of typedef %s discloses too much information";_tag_dyneither(_tmp132,sizeof(char),57);}),_tag_dyneither(_tmp131,sizeof(void*),1));});}));return 0;}return 1;}
# 744
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp134=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0,msg);if(_tmp134 == 0)return 0;if(d0 != _tmp134){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name));({void*_tmp135[1]={& _tmp137};Cyc_aprintf(({const char*_tmp136="declaration of xdatatypefield %s discloses too much information";_tag_dyneither(_tmp136,sizeof(char),64);}),_tag_dyneither(_tmp135,sizeof(void*),1));});}));return 0;}return 1;}
# 749
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);
# 758
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
int _tmp138=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,({const char*_tmp143="type";_tag_dyneither(_tmp143,sizeof(char),5);}),msg);
int _tmp139=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,Cyc_Interface_incl_datatypedecl,({const char*_tmp142="datatype";_tag_dyneither(_tmp142,sizeof(char),9);}),msg);
int _tmp13A=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,({const char*_tmp141="enum";_tag_dyneither(_tmp141,sizeof(char),5);}),msg);
# 763
int _tmp13B=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),Cyc_Tcdecl_merge_typedefdecl,({const char*_tmp140="typedef";_tag_dyneither(_tmp140,sizeof(char),8);}),msg)!= 0;
# 766
int _tmp13C=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,({const char*_tmp13F="variable";_tag_dyneither(_tmp13F,sizeof(char),9);}),msg);
int _tmp13D=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,({const char*_tmp13E="xdatatypefield";_tag_dyneither(_tmp13E,sizeof(char),15);}),msg);
return((((_tmp138  && _tmp139) && _tmp13A) && _tmp13B) && _tmp13C) && _tmp13D;}
# 771
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp144;_push_handler(& _tmp144);{int _tmp146=0;if(setjmp(_tmp144.handler))_tmp146=1;if(!_tmp146){
{int _tmp147=1;
struct _dyneither_ptr*msg=0;
# 776
if(info != 0)
msg=({struct _dyneither_ptr*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp149[2]={& _tmp14B,& _tmp14C};Cyc_aprintf(({const char*_tmp14A="checking inclusion of %s exports into %s exports,";_tag_dyneither(_tmp14A,sizeof(char),50);}),_tag_dyneither(_tmp149,sizeof(void*),2));});});});_tmp148;});
if(!Cyc_Interface_incl_ienv(i1->exports,i2->exports,msg))_tmp147=0;
# 780
if(info != 0)
msg=({struct _dyneither_ptr*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0;_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({void*_tmp14E[2]={& _tmp150,& _tmp151};Cyc_aprintf(({const char*_tmp14F="checking inclusion of %s imports into %s imports,";_tag_dyneither(_tmp14F,sizeof(char),50);}),_tag_dyneither(_tmp14E,sizeof(void*),2));});});});_tmp14D;});
if(!Cyc_Interface_incl_ienv(i2->imports,i1->imports,msg))_tmp147=0;{
# 784
int _tmp152=_tmp147;_npop_handler(0);return _tmp152;};}
# 773
;_pop_handler();}else{void*_tmp145=(void*)_exn_thrown;void*_tmp153=_tmp145;void*_tmp154;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp153)->tag == Cyc_Tcdecl_Incompatible){_LLC8: _LLC9:
# 786
 return 0;}else{_LLCA: _tmp154=_tmp153;_LLCB:(int)_rethrow(_tmp154);}_LLC7:;}};}struct _tuple26{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;void*(*f5)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr f6;struct _dyneither_ptr*f7;};
# 794
void Cyc_Interface_compat_merge_dict_f(struct _tuple26*env,struct _tuple0*x,void*y2){
# 801
struct _tuple26*_tmp155=env;int*_tmp16E;struct Cyc_Dict_Dict*_tmp16D;struct Cyc_Dict_Dict _tmp16C;struct Cyc_Dict_Dict _tmp16B;void*(*_tmp16A)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr _tmp169;struct _dyneither_ptr*_tmp168;_LLCD: _tmp16E=(int*)& _tmp155->f1;_tmp16D=(struct Cyc_Dict_Dict*)& _tmp155->f2;_tmp16C=_tmp155->f3;_tmp16B=_tmp155->f4;_tmp16A=_tmp155->f5;_tmp169=_tmp155->f6;_tmp168=_tmp155->f7;_LLCE:;{
void*y;
{struct _handler_cons _tmp156;_push_handler(& _tmp156);{int _tmp158=0;if(setjmp(_tmp156.handler))_tmp158=1;if(!_tmp158){
{void*_tmp159=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp16C,x);
# 807
void*_tmp15A=_tmp16A(_tmp159,y2,0,_tmp168);
if(!((unsigned int)_tmp15A)){
*_tmp16E=0;
_npop_handler(0);return;}
# 812
y=_tmp15A;}
# 804
;_pop_handler();}else{void*_tmp157=(void*)_exn_thrown;void*_tmp15B=_tmp157;void*_tmp15C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15B)->tag == Cyc_Dict_Absent){_LLD0: _LLD1:
# 815
 y=y2;
goto _LLCF;}else{_LLD2: _tmp15C=_tmp15B;_LLD3:(int)_rethrow(_tmp15C);}_LLCF:;}};}{
# 819
struct _handler_cons _tmp15D;_push_handler(& _tmp15D);{int _tmp15F=0;if(setjmp(_tmp15D.handler))_tmp15F=1;if(!_tmp15F){
{void*_tmp160=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp16B,x);
# 824
void*_tmp161=_tmp16A(_tmp160,y,0,_tmp168);
if(_tmp161 != _tmp160){
if((unsigned int)_tmp161)
Cyc_Interface_check_err(_tmp168,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0;_tmp165.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(x));({struct Cyc_String_pa_PrintArg_struct _tmp164;_tmp164.tag=0;_tmp164.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp169);({void*_tmp162[2]={& _tmp164,& _tmp165};Cyc_aprintf(({const char*_tmp163="abstract %s %s is being imported as non-abstract";_tag_dyneither(_tmp163,sizeof(char),49);}),_tag_dyneither(_tmp162,sizeof(void*),2));});});}));
*_tmp16E=0;}}
# 820
;_pop_handler();}else{void*_tmp15E=(void*)_exn_thrown;void*_tmp166=_tmp15E;void*_tmp167;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp166)->tag == Cyc_Dict_Absent){_LLD5: _LLD6:
# 833
 if(*_tmp16E)
*_tmp16D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp16D,x,y);
goto _LLD4;}else{_LLD7: _tmp167=_tmp166;_LLD8:(int)_rethrow(_tmp167);}_LLD4:;}};};};}
# 839
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 847
struct _tuple26 _tmp16F=({struct _tuple26 _tmp171;_tmp171.f1=1;_tmp171.f2=dic1;_tmp171.f3=dic1;_tmp171.f4=excl;_tmp171.f5=merge_f;_tmp171.f6=t;_tmp171.f7=msg;_tmp171;});
# 849
((void(*)(void(*f)(struct _tuple26*,struct _tuple0*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,& _tmp16F,dic2);
# 851
if(_tmp16F.f1)
return({struct Cyc_Dict_Dict*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=_tmp16F.f2;_tmp170;});
return 0;}
# 861
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp172=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,({const char*_tmp17E="type";_tag_dyneither(_tmp17E,sizeof(char),5);}),msg);
struct Cyc_Dict_Dict*_tmp173=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,({const char*_tmp17D="datatype";_tag_dyneither(_tmp17D,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp174=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,({const char*_tmp17C="enum";_tag_dyneither(_tmp17C,sizeof(char),5);}),msg);
struct Cyc_Dict_Dict*_tmp175=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,({const char*_tmp17B="typedef";_tag_dyneither(_tmp17B,sizeof(char),8);}),msg);
struct Cyc_Dict_Dict*_tmp176=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,({const char*_tmp17A="variable";_tag_dyneither(_tmp17A,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp177=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,({const char*_tmp179="xdatatypefield";_tag_dyneither(_tmp179,sizeof(char),15);}),msg);
if(((((!((unsigned int)_tmp172) || !((unsigned int)_tmp173)) || !((unsigned int)_tmp174)) || !((unsigned int)_tmp175)) || !((unsigned int)_tmp176)) || !((unsigned int)_tmp177))
return 0;
return({struct Cyc_Interface_Ienv*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->aggrdecls=*_tmp172;_tmp178->datatypedecls=*_tmp173;_tmp178->enumdecls=*_tmp174;_tmp178->typedefdecls=*_tmp175;_tmp178->vardecls=*_tmp176;_tmp178->xdatatypefielddecls=*_tmp177;_tmp178;});}struct _tuple27{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 874
void Cyc_Interface_disj_merge_dict_f(struct _tuple27*env,struct _tuple0*x,void*y){
# 877
struct _tuple27*_tmp17F=env;int*_tmp188;struct Cyc_Dict_Dict*_tmp187;struct Cyc_Dict_Dict _tmp186;struct _dyneither_ptr _tmp185;struct _dyneither_ptr*_tmp184;_LLDA: _tmp188=(int*)& _tmp17F->f1;_tmp187=(struct Cyc_Dict_Dict*)& _tmp17F->f2;_tmp186=_tmp17F->f3;_tmp185=_tmp17F->f4;_tmp184=_tmp17F->f5;_LLDB:;
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(_tmp186,x)){
Cyc_Interface_check_err(_tmp184,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(x));({struct Cyc_String_pa_PrintArg_struct _tmp182;_tmp182.tag=0;_tmp182.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp185);({void*_tmp180[2]={& _tmp182,& _tmp183};Cyc_aprintf(({const char*_tmp181="%s %s is exported more than once";_tag_dyneither(_tmp181,sizeof(char),33);}),_tag_dyneither(_tmp180,sizeof(void*),2));});});}));
*_tmp188=0;}else{
# 883
if(*_tmp188)*_tmp187=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp187,x,y);}}
# 887
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 891
struct _tuple27 _tmp189=({struct _tuple27 _tmp18B;_tmp18B.f1=1;_tmp18B.f2=dic1;_tmp18B.f3=dic1;_tmp18B.f4=t;_tmp18B.f5=msg;_tmp18B;});
((void(*)(void(*f)(struct _tuple27*,struct _tuple0*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp189,dic2);
if(_tmp189.f1)
return({struct Cyc_Dict_Dict*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A[0]=_tmp189.f2;_tmp18A;});
return 0;}
# 898
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp18C=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),Cyc_Tcdecl_merge_aggrdecl,({const char*_tmp198="type";_tag_dyneither(_tmp198,sizeof(char),5);}),msg);
# 902
struct Cyc_Dict_Dict*_tmp18D=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,({const char*_tmp197="datatype";_tag_dyneither(_tmp197,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp18E=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,ie2->enumdecls,({const char*_tmp196="enum";_tag_dyneither(_tmp196,sizeof(char),5);}),msg);
# 905
struct Cyc_Dict_Dict*_tmp18F=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),Cyc_Tcdecl_merge_typedefdecl,({const char*_tmp195="typedef";_tag_dyneither(_tmp195,sizeof(char),8);}),msg);
# 908
struct Cyc_Dict_Dict*_tmp190=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,ie2->vardecls,({const char*_tmp194="variable";_tag_dyneither(_tmp194,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp191=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,({const char*_tmp193="xdatatypefield";_tag_dyneither(_tmp193,sizeof(char),15);}),msg);
# 912
if(((((!((unsigned int)_tmp18C) || !((unsigned int)_tmp18D)) || !((unsigned int)_tmp18E)) || !((unsigned int)_tmp18F)) || !((unsigned int)_tmp190)) || !((unsigned int)_tmp191))
return 0;
return({struct Cyc_Interface_Ienv*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->aggrdecls=*_tmp18C;_tmp192->datatypedecls=*_tmp18D;_tmp192->enumdecls=*_tmp18E;_tmp192->typedefdecls=*_tmp18F;_tmp192->vardecls=*_tmp190;_tmp192->xdatatypefielddecls=*_tmp191;_tmp192;});}
# 918
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp199;_push_handler(& _tmp199);{int _tmp19B=0;if(setjmp(_tmp199.handler))_tmp19B=1;if(!_tmp19B){
{struct _dyneither_ptr*msg=0;
# 922
if(info != 0)
msg=({struct _dyneither_ptr*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp19F;_tmp19F.tag=0;_tmp19F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp19D[2]={& _tmp19F,& _tmp1A0};Cyc_aprintf(({const char*_tmp19E="merging exports of %s and %s,";_tag_dyneither(_tmp19E,sizeof(char),30);}),_tag_dyneither(_tmp19D,sizeof(void*),2));});});});_tmp19C;});{
# 926
struct Cyc_Interface_Ienv*_tmp1A1=Cyc_Interface_disj_merge_ienv(i1->exports,i2->exports,msg);
if(_tmp1A1 == 0){struct Cyc_Interface_I*_tmp1A2=0;_npop_handler(0);return _tmp1A2;}
# 929
if(info != 0)
msg=({struct _dyneither_ptr*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1A6;_tmp1A6.tag=0;_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp1A4[2]={& _tmp1A6,& _tmp1A7};Cyc_aprintf(({const char*_tmp1A5="merging imports of %s and %s,";_tag_dyneither(_tmp1A5,sizeof(char),30);}),_tag_dyneither(_tmp1A4,sizeof(void*),2));});});});_tmp1A3;});{
# 933
struct Cyc_Interface_Ienv*_tmp1A8=Cyc_Interface_compat_merge_ienv(i1->imports,i2->imports,_tmp1A1,msg);
if(_tmp1A8 == 0){struct Cyc_Interface_I*_tmp1A9=0;_npop_handler(0);return _tmp1A9;}{
# 936
struct Cyc_List_List*newtds=0;struct Cyc_List_List*l=i2->tds;
for(0;l != 0;l=l->tl){
newtds=({struct Cyc_List_List*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->hd=({struct Cyc_Absyn_Decl*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=*((struct Cyc_Absyn_Decl*)l->hd);_tmp1AB;});_tmp1AA->tl=newtds;_tmp1AA;});}
# 940
newtds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,newtds,i1->tds));{
struct Cyc_Interface_I*_tmp1AD=({struct Cyc_Interface_I*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->imports=_tmp1A8;_tmp1AC->exports=_tmp1A1;_tmp1AC->tds=newtds;_tmp1AC;});_npop_handler(0);return _tmp1AD;};};};};}
# 920
;_pop_handler();}else{void*_tmp19A=(void*)_exn_thrown;void*_tmp1AE=_tmp19A;void*_tmp1AF;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp1AE)->tag == Cyc_Tcdecl_Incompatible){_LLDD: _LLDE:
# 942
 return 0;}else{_LLDF: _tmp1AF=_tmp1AE;_LLE0:(int)_rethrow(_tmp1AF);}_LLDC:;}};}
# 945
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li == 0)return Cyc_Interface_empty();{
# 948
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo != 0)linfo=linfo->tl;
# 953
for(0;li != 0;li=li->tl){
struct Cyc_Interface_I*_tmp1B0=Cyc_Interface_merge(curr_i,(struct Cyc_Interface_I*)li->hd,
curr_info != 0  && linfo != 0?({struct _tuple10*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->f1=*curr_info;_tmp1B6->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp1B6;}): 0);
if(_tmp1B0 == 0)return 0;
curr_i=_tmp1B0;
if(curr_info != 0)
curr_info=linfo != 0?({struct _dyneither_ptr*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B5;_tmp1B5.tag=0;_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0;_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);({void*_tmp1B2[2]={& _tmp1B4,& _tmp1B5};Cyc_aprintf(({const char*_tmp1B3="%s+%s";_tag_dyneither(_tmp1B3,sizeof(char),6);}),_tag_dyneither(_tmp1B2,sizeof(void*),2));});});});_tmp1B1;}): 0;
# 961
if(linfo != 0)linfo=linfo->tl;}
# 963
return curr_i;};}
# 966
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la == 0)return Cyc_Interface_empty();{
# 969
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo != 0)linfo=linfo->tl;
# 974
for(0;la != 0;la=la->tl){
struct Cyc_Interface_I*_tmp1B7=Cyc_Interface_merge(curr_i,get(la->hd),
curr_info != 0  && linfo != 0?({struct _tuple10*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->f1=*curr_info;_tmp1BD->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp1BD;}): 0);
if(_tmp1B7 == 0)return 0;
curr_i=_tmp1B7;
if(curr_info != 0)
curr_info=linfo != 0?({struct _dyneither_ptr*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1BC;_tmp1BC.tag=0;_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));({struct Cyc_String_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);({void*_tmp1B9[2]={& _tmp1BB,& _tmp1BC};Cyc_aprintf(({const char*_tmp1BA="%s+%s";_tag_dyneither(_tmp1BA,sizeof(char),6);}),_tag_dyneither(_tmp1B9,sizeof(void*),2));});});});_tmp1B8;}): 0;
# 982
if(linfo != 0)linfo=linfo->tl;}
# 984
return curr_i;};}
# 990
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds){
struct Cyc_List_List*_tmp1BE=0;
for(0;tds != 0;tds=tds->tl){
struct _tuple0*qv;
struct Cyc_Absyn_Decl*_tmp1BF=(struct Cyc_Absyn_Decl*)tds->hd;
{void*_tmp1C0=_tmp1BF->r;void*_tmp1C1=_tmp1C0;struct Cyc_Absyn_Typedefdecl*_tmp1C8;struct Cyc_Absyn_Enumdecl*_tmp1C7;struct Cyc_Absyn_Datatypedecl*_tmp1C6;struct Cyc_Absyn_Aggrdecl*_tmp1C5;struct Cyc_Absyn_Fndecl*_tmp1C4;struct Cyc_Absyn_Vardecl*_tmp1C3;switch(*((int*)_tmp1C1)){case 0: _LLE2: _tmp1C3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1C1)->f1;_LLE3:
 qv=_tmp1C3->name;goto _LLE1;case 1: _LLE4: _tmp1C4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1C1)->f1;_LLE5:
 qv=_tmp1C4->name;goto _LLE1;case 5: _LLE6: _tmp1C5=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1C1)->f1;_LLE7:
 qv=_tmp1C5->name;goto _LLE1;case 6: _LLE8: _tmp1C6=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1C1)->f1;_LLE9:
 qv=_tmp1C6->name;goto _LLE1;case 7: _LLEA: _tmp1C7=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1C1)->f1;_LLEB:
 qv=_tmp1C7->name;goto _LLE1;case 8: _LLEC: _tmp1C8=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1C1)->f1;_LLED:
 qv=_tmp1C8->name;goto _LLE1;default: _LLEE: _LLEF:
 Cyc_Interface_err(({const char*_tmp1C2="bad decl form in Interface::add_namespace";_tag_dyneither(_tmp1C2,sizeof(char),42);}));return 0;}_LLE1:;}{
# 1004
struct Cyc_List_List*vs;
{union Cyc_Absyn_Nmspace _tmp1C9=(*qv).f1;union Cyc_Absyn_Nmspace _tmp1CA=_tmp1C9;struct Cyc_List_List*_tmp1CD;struct Cyc_List_List*_tmp1CC;switch((_tmp1CA.C_n).tag){case 2: _LLF1: _tmp1CC=(_tmp1CA.Abs_n).val;_LLF2:
 vs=_tmp1CC;goto _LLF0;case 3: _LLF3: _tmp1CD=(_tmp1CA.C_n).val;_LLF4:
 vs=_tmp1CD;goto _LLF0;default: _LLF5: _LLF6:
 Cyc_Interface_err(({const char*_tmp1CB="bad namespace in Interface::add_namespace";_tag_dyneither(_tmp1CB,sizeof(char),42);}));return 0;}_LLF0:;}
# 1010
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);
(*qv).f1=Cyc_Absyn_Rel_n(0);
for(0;vs != 0;vs=vs->tl){
_tmp1BF=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp1CF;_tmp1CF.tag=9;_tmp1CF.f1=(struct _dyneither_ptr*)vs->hd;_tmp1CF.f2=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=_tmp1BF;_tmp1D0->tl=0;_tmp1D0;});_tmp1CF;});_tmp1CE;}),_tmp1BF->loc);}
_tmp1BE=({struct Cyc_List_List*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->hd=_tmp1BF;_tmp1D1->tl=_tmp1BE;_tmp1D1;});};}
# 1016
_tmp1BE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1BE);
return _tmp1BE;}
# 1020
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1D4;_tmp1D4.tag=5;_tmp1D4.f1=d;_tmp1D4;});_tmp1D3;}),0);_tmp1D2->tl=tds;_tmp1D2;});}
# 1024
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 1026
d=({struct Cyc_Absyn_Aggrdecl*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=*d;_tmp1D5;});
d->impl=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1D8;_tmp1D8.tag=5;_tmp1D8.f1=d;_tmp1D8;});_tmp1D7;}),0);_tmp1D6->tl=tds;_tmp1D6;});}
# 1032
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1DB;_tmp1DB.tag=6;_tmp1DB.f1=d;_tmp1DB;});_tmp1DA;}),0);_tmp1D9->tl=tds;_tmp1D9;});}static char _tmp1DC[2]="_";
# 1036
static struct _dyneither_ptr Cyc_Interface_us={_tmp1DC,_tmp1DC,_tmp1DC + 2};
static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 1039
static struct _tuple20*Cyc_Interface_rewrite_datatypefield_type(struct _tuple20*x){
return({struct _tuple20*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->f1=Cyc_Absyn_empty_tqual(0);_tmp1DD->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1DF;_tmp1DF.tag=2;_tmp1DF.f1=({struct Cyc_Absyn_Tvar*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->name=Cyc_Interface_us_p;_tmp1E0->identity=- 1;_tmp1E0->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp1E0;});_tmp1DF;});_tmp1DE;});_tmp1DD;});}
# 1044
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
f=({struct Cyc_Absyn_Datatypefield*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1[0]=*f;_tmp1E1;});
f->typs=((struct Cyc_List_List*(*)(struct _tuple20*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);
return f;}
# 1050
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 1052
d=({struct Cyc_Absyn_Datatypedecl*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=*d;_tmp1E2;});
# 1054
if(d->fields != 0)d->fields=({struct Cyc_Core_Opt*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v);_tmp1E3;});
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1E6;_tmp1E6.tag=6;_tmp1E6.f1=d;_tmp1E6;});_tmp1E5;}),0);_tmp1E4->tl=tds;_tmp1E4;});}
# 1059
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1E9;_tmp1E9.tag=7;_tmp1E9.f1=d;_tmp1E9;});_tmp1E8;}),0);_tmp1E7->tl=tds;_tmp1E7;});}
# 1063
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 1065
d=({struct Cyc_Absyn_Enumdecl*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=*d;_tmp1EA;});
d->fields=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1ED;_tmp1ED.tag=7;_tmp1ED.f1=d;_tmp1ED;});_tmp1EC;}),0);_tmp1EB->tl=tds;_tmp1EB;});}
# 1071
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 1073
return({struct Cyc_List_List*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp1F0;_tmp1F0.tag=8;_tmp1F0.f1=d;_tmp1F0;});_tmp1EF;}),0);_tmp1EE->tl=tds;_tmp1EE;});}
# 1076
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _tuple0*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp1F3;_tmp1F3.tag=0;_tmp1F3.f1=d;_tmp1F3;});_tmp1F2;}),0);_tmp1F1->tl=tds;_tmp1F1;});}
# 1080
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1082
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1F4=d;struct Cyc_Absyn_Datatypedecl*_tmp1FC;struct Cyc_Absyn_Datatypefield*_tmp1FB;_LLF8: _tmp1FC=_tmp1F4->base;_tmp1FB=_tmp1F4->field;_LLF9:;
_tmp1FC=({struct Cyc_Absyn_Datatypedecl*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=*_tmp1FC;_tmp1F5;});
_tmp1FC->fields=({struct Cyc_Core_Opt*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->v=({struct Cyc_List_List*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->hd=_tmp1FB;_tmp1F7->tl=0;_tmp1F7;});_tmp1F6;});
_tmp1FC->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1FA;_tmp1FA.tag=6;_tmp1FA.f1=_tmp1FC;_tmp1FA;});_tmp1F9;}),0);_tmp1F8->tl=tds;_tmp1F8;});}
# 1089
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1091
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1FD=d;struct Cyc_Absyn_Datatypedecl*_tmp205;struct Cyc_Absyn_Datatypefield*_tmp204;_LLFB: _tmp205=_tmp1FD->base;_tmp204=_tmp1FD->field;_LLFC:;
_tmp205=({struct Cyc_Absyn_Datatypedecl*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE[0]=*_tmp205;_tmp1FE;});
_tmp204=Cyc_Interface_rewrite_datatypefield(_tmp204);
_tmp204->sc=Cyc_Absyn_Extern;
_tmp205->fields=({struct Cyc_Core_Opt*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->v=({struct Cyc_List_List*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->hd=_tmp204;_tmp200->tl=0;_tmp200;});_tmp1FF;});
_tmp205->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp203;_tmp203.tag=6;_tmp203.f1=_tmp205;_tmp203;});_tmp202;}),0);_tmp201->tl=tds;_tmp201;});}
# 1100
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp206=0;
_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp206);
_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp206);
_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp206);
# 1106
if(_tmp206 != 0){
_tmp206=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp206);
_tmp206=Cyc_Interface_add_namespace(_tmp206);
Cyc_Absynpp_decllist2file(_tmp206,f);}}
# 1113
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp207=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);
if(_tmp207 != 0){
_tmp207=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp207);
_tmp207=Cyc_Interface_add_namespace(_tmp207);
Cyc_Absynpp_decllist2file(_tmp207,f);}}
# 1122
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp208=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,0);
if(_tmp208 != 0){
_tmp208=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp208);
_tmp208=Cyc_Interface_add_namespace(_tmp208);
Cyc_Absynpp_decllist2file(_tmp208,f);}}
# 1131
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp209=0;
_tmp209=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp209);
_tmp209=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp209);
_tmp209=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp209);
_tmp209=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,_tmp209);
_tmp209=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp209);
# 1139
if(_tmp209 != 0){
_tmp209=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp209);
_tmp209=Cyc_Interface_add_namespace(_tmp209);
Cyc_Absynpp_decllist2file(_tmp209,f);}}
# 1149
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
({void*_tmp20A=0;Cyc_fprintf(f,({const char*_tmp20B="/****** needed (headers) ******/\n";_tag_dyneither(_tmp20B,sizeof(char),34);}),_tag_dyneither(_tmp20A,sizeof(void*),0));});
Cyc_Interface_print_ns_headers(f,i->imports);
# 1154
({void*_tmp20C=0;Cyc_fprintf(f,({const char*_tmp20D="\n/****** provided (headers) ******/\n";_tag_dyneither(_tmp20D,sizeof(char),37);}),_tag_dyneither(_tmp20C,sizeof(void*),0));});
Cyc_Interface_print_ns_headers(f,i->exports);
# 1159
({void*_tmp20E=0;Cyc_fprintf(f,({const char*_tmp20F="\n/****** needed (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp20F,sizeof(char),58);}),_tag_dyneither(_tmp20E,sizeof(void*),0));});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->imports);
# 1162
({void*_tmp210=0;Cyc_fprintf(f,({const char*_tmp211="\n/****** provided (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp211,sizeof(char),60);}),_tag_dyneither(_tmp210,sizeof(void*),0));});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->exports);
# 1166
({void*_tmp212=0;Cyc_fprintf(f,({const char*_tmp213="\n/****** provided (typedefs) ******/\n";_tag_dyneither(_tmp213,sizeof(char),38);}),_tag_dyneither(_tmp212,sizeof(void*),0));});
Cyc_Interface_print_ns_typedefs(f,i->exports);
# 1169
({void*_tmp214=0;Cyc_fprintf(f,({const char*_tmp215="\n/****** needed (declarations) ******/\n";_tag_dyneither(_tmp215,sizeof(char),40);}),_tag_dyneither(_tmp214,sizeof(void*),0));});
Cyc_Interface_print_ns_decls(f,i->imports);
# 1172
({void*_tmp216=0;Cyc_fprintf(f,({const char*_tmp217="\n/****** provided (declarations) ******/\n";_tag_dyneither(_tmp217,sizeof(char),42);}),_tag_dyneither(_tmp216,sizeof(void*),0));});
Cyc_Interface_print_ns_decls(f,i->exports);}
# 1179
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp218=Cyc_Parse_parse_file(f);
struct _RegionHandle _tmp219=_new_region("r");struct _RegionHandle*r=& _tmp219;_push_region(r);
Cyc_Binding_resolve_all(_tmp218);{
struct Cyc_Tcenv_Tenv*_tmp21A=Cyc_Tcenv_tc_init(r);
Cyc_Tc_tc(r,_tmp21A,0,_tmp218);{
struct Cyc_Interface_I*_tmp21B=Cyc_Interface_gen_extract(_tmp21A->ae,0,_tmp218);_npop_handler(0);return _tmp21B;};};
# 1183
;_pop_region(r);};}
# 1194
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1200
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}
