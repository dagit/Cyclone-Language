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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 450 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466
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
# 944 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 968
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 982
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 984
extern void*Cyc_Absyn_sint_typ;
# 990
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1023
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1137
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1142
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1171
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1202
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 176
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 70 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 74
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo);
# 76
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 79
void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 36 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
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
typedef struct Cyc_Yyltype Cyc_yyltype;
# 40 "tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 101
int Cyc_Tcutil_is_function_type(void*t);
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 66 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 73
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 82
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 38 "interface.cyc"
void Cyc_Lex_lex_init();
# 45
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Interface_xdatatypefielddecl_t;struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};
# 57
typedef struct Cyc_Interface_Ienv*Cyc_Interface_ienv_t;struct Cyc_Interface_I{struct Cyc_Dict_Dict imports;struct Cyc_Dict_Dict exports;};
# 65
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
struct Cyc_Interface_Ienv*_tmp22C;return(_tmp22C=_cycalloc(sizeof(*_tmp22C)),((_tmp22C->aggrdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp22C->datatypedecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp22C->enumdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp22C->typedefdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp22C->vardecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp22C->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),_tmp22C)))))))))))));}
# 76
static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){
static struct Cyc_Interface_Ienv*ie=0;
if(ie == 0)
ie=Cyc_Interface_new_ienv();
return ie;}
# 83
static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(void*ignored){
return Cyc_Interface_new_ienv();}
# 87
static struct Cyc_Interface_I*Cyc_Interface_skel2i(struct Cyc_Dict_Dict skel){
struct Cyc_Interface_I*_tmp22D;return(_tmp22D=_cycalloc(sizeof(*_tmp22D)),((_tmp22D->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),((_tmp22D->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),_tmp22D)))));}
# 91
struct Cyc_Interface_I*Cyc_Interface_empty(){
struct Cyc_Interface_I*_tmp22E;return(_tmp22E=_cycalloc(sizeof(*_tmp22E)),((_tmp22E->imports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),((_tmp22E->exports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),_tmp22E)))));}
# 97
struct Cyc_Interface_I*Cyc_Interface_final(){
struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();
# 100
struct Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();
struct Cyc_Absyn_Datatypedecl*_tmp22F;struct Cyc_Absyn_Datatypedecl*_tmp5=(_tmp22F=_cycalloc(sizeof(*_tmp22F)),((_tmp22F[0]=*Cyc_Absyn_exn_tud,_tmp22F)));
_tmp5->sc=Cyc_Absyn_Public;
_tmp4->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,_tmp5);
{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
struct Cyc_Absyn_Datatypefield*_tmp230;struct Cyc_Absyn_Datatypefield*_tmp7=(_tmp230=_cycalloc(sizeof(*_tmp230)),((_tmp230[0]=*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd),_tmp230)));
_tmp7->sc=Cyc_Absyn_Public;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp231;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp8=(_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231->base=_tmp5,((_tmp231->field=_tmp7,_tmp231)))));
_tmp4->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(_tmp4->xdatatypefielddecls,(*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd)->name).f2,_tmp8);};}}
# 111
_tmp3->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,0,_tmp4);{
# 113
struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();
struct _dyneither_ptr*_tmp239;const char*_tmp238;union Cyc_Absyn_Nmspace _tmp237;struct _tuple0*_tmp236;struct _tuple0*qmain=(_tmp236=_cycalloc(sizeof(*_tmp236)),((_tmp236->f1=(((_tmp237.Abs_n).val=0,(((_tmp237.Abs_n).tag=2,_tmp237)))),((_tmp236->f2=((_tmp239=_cycalloc(sizeof(*_tmp239)),((_tmp239[0]=(struct _dyneither_ptr)((_tmp238="main",_tag_dyneither(_tmp238,sizeof(char),5))),_tmp239)))),_tmp236)))));
struct Cyc_List_List*_tmp241;struct _tuple8*_tmp240;struct _tuple8*_tmp23F;struct Cyc_List_List*_tmp23E;struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(qmain,
Cyc_Absyn_function_typ(0,0,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_sint_typ,(
(_tmp23E=_cycalloc(sizeof(*_tmp23E)),((_tmp23E->hd=((_tmp23F=_cycalloc(sizeof(*_tmp23F)),((_tmp23F->f1=0,((_tmp23F->f2=Cyc_Absyn_empty_tqual(0),((_tmp23F->f3=Cyc_Absyn_uint_typ,_tmp23F)))))))),((_tmp23E->tl=(
(_tmp241=_cycalloc(sizeof(*_tmp241)),((_tmp241->hd=((_tmp240=_cycalloc(sizeof(*_tmp240)),((_tmp240->f1=0,((_tmp240->f2=Cyc_Absyn_empty_tqual(0),((_tmp240->f3=
Cyc_Absyn_dyneither_typ(Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
# 121
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),(void*)& Cyc_Absyn_HeapRgn_val,
# 123
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp240)))))))),((_tmp241->tl=0,_tmp241)))))),_tmp23E)))))),0,0,0,0,0,0),0);
# 127
_tmpC->sc=Cyc_Absyn_Extern;
_tmpB->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpB->vardecls,(*qmain).f2,_tmpC);
_tmp3->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,0,_tmpB);
# 131
return _tmp3;};}
# 151 "interface.cyc"
static void Cyc_Interface_err(struct _dyneither_ptr msg){
void*_tmp242;(_tmp242=0,Cyc_Tcutil_terr(0,msg,_tag_dyneither(_tmp242,sizeof(void*),0)));}
# 154
static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){
struct Cyc_Core_Invalid_argument_exn_struct _tmp245;struct Cyc_Core_Invalid_argument_exn_struct*_tmp244;(int)_throw((void*)((_tmp244=_cycalloc(sizeof(*_tmp244)),((_tmp244[0]=((_tmp245.tag=Cyc_Core_Invalid_argument,((_tmp245.f1=s,_tmp245)))),_tmp244)))));}
# 157
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){
const char*_tmp24B;void*_tmp24A[3];struct Cyc_String_pa_PrintArg_struct _tmp249;struct Cyc_String_pa_PrintArg_struct _tmp248;struct Cyc_String_pa_PrintArg_struct _tmp247;Cyc_Interface_err((struct _dyneither_ptr)((_tmp247.tag=0,((_tmp247.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n)),((_tmp248.tag=0,((_tmp248.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp249.tag=0,((_tmp249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((_tmp24A[0]=& _tmp249,((_tmp24A[1]=& _tmp248,((_tmp24A[2]=& _tmp247,Cyc_aprintf(((_tmp24B="fields of %s %s %s have never been defined",_tag_dyneither(_tmp24B,sizeof(char),43))),_tag_dyneither(_tmp24A,sizeof(void*),3)))))))))))))))))))));}
# 161
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){
const char*_tmp250;void*_tmp24F[2];struct Cyc_String_pa_PrintArg_struct _tmp24E;struct Cyc_String_pa_PrintArg_struct _tmp24D;Cyc_Interface_err((struct _dyneither_ptr)((_tmp24D.tag=0,((_tmp24D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n)),((_tmp24E.tag=0,((_tmp24E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((_tmp24F[0]=& _tmp24E,((_tmp24F[1]=& _tmp24D,Cyc_aprintf(((_tmp250="the body of %s function %s has never been defined",_tag_dyneither(_tmp250,sizeof(char),50))),_tag_dyneither(_tmp24F,sizeof(void*),2)))))))))))))));}
# 167
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr){
const char*_tmp257;void*_tmp256[4];struct Cyc_String_pa_PrintArg_struct _tmp255;struct Cyc_String_pa_PrintArg_struct _tmp254;struct Cyc_String_pa_PrintArg_struct _tmp253;struct Cyc_String_pa_PrintArg_struct _tmp252;Cyc_Interface_err((struct _dyneither_ptr)((_tmp252.tag=0,((_tmp252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 171
Cyc_Absynpp_qvar2string(name2)),((_tmp253.tag=0,((_tmp253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp254.tag=0,((_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 170
Cyc_Absynpp_qvar2string(name1)),((_tmp255.tag=0,((_tmp255.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((_tmp256[0]=& _tmp255,((_tmp256[1]=& _tmp254,((_tmp256[2]=& _tmp253,((_tmp256[3]=& _tmp252,Cyc_aprintf(((_tmp257="declaration of %s %s relies on static %s %s",_tag_dyneither(_tmp257,sizeof(char),44))),_tag_dyneither(_tmp256,sizeof(void*),4)))))))))))))))))))))))))));}}
# 175
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr){
const char*_tmp25E;void*_tmp25D[4];struct Cyc_String_pa_PrintArg_struct _tmp25C;struct Cyc_String_pa_PrintArg_struct _tmp25B;struct Cyc_String_pa_PrintArg_struct _tmp25A;struct Cyc_String_pa_PrintArg_struct _tmp259;Cyc_Interface_err((struct _dyneither_ptr)((_tmp259.tag=0,((_tmp259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 179
Cyc_Absynpp_qvar2string(name2)),((_tmp25A.tag=0,((_tmp25A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp25B.tag=0,((_tmp25B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 178
Cyc_Absynpp_qvar2string(name1)),((_tmp25C.tag=0,((_tmp25C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((_tmp25D[0]=& _tmp25C,((_tmp25D[1]=& _tmp25B,((_tmp25D[2]=& _tmp25A,((_tmp25D[3]=& _tmp259,Cyc_aprintf(((_tmp25E="declaration of %s %s relies on fields of abstract %s %s",_tag_dyneither(_tmp25E,sizeof(char),56))),_tag_dyneither(_tmp25D,sizeof(void*),4)))))))))))))))))))))))))));}}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 190
typedef struct Cyc_Interface_Seen*Cyc_Interface_seen_t;
# 192
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){
struct Cyc_Interface_Seen*_tmp25F;return(_tmp25F=_cycalloc(sizeof(*_tmp25F)),((_tmp25F->aggrs=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp25F->datatypes=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),_tmp25F)))));}
# 198
typedef struct Cyc_Dict_Dict Cyc_Interface_aenv_t;
# 200
static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);
# 203
static int Cyc_Interface_check_public_type_list(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 205
int _tmp2F=1;
for(0;l != 0;l=l->tl){
if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f(l->hd)))
_tmp2F=0;}
return _tmp2F;}
# 211
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _tmp30;_push_handler(& _tmp30);{int _tmp32=0;if(setjmp(_tmp30.handler))_tmp32=1;if(!_tmp32){{int _tmp33=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp33;};_pop_handler();}else{void*_tmp31=(void*)_exn_thrown;void*_tmp34=_tmp31;void*_tmp35;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp34)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp35=_tmp34;_LL4:(int)_rethrow(_tmp35);}_LL0:;}};}{
int _tmp36=1;
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp36);
if(d->impl != 0){
struct Cyc_List_List*_tmp37=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;for(0;_tmp37 != 0;_tmp37=_tmp37->tl){
const char*_tmp260;if(!Cyc_Interface_check_public_type(ae,seen,((_tmp260="type",_tag_dyneither(_tmp260,sizeof(char),5))),d->name,((struct Cyc_Absyn_Aggrfield*)_tmp37->hd)->type))
_tmp36=0;}}
# 221
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp36);
return _tmp36;};}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 225
static void*Cyc_Interface_get_type1(struct _tuple20*x){
return(*x).f2;}
# 228
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f3;}
# 232
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(_tmp39.handler))_tmp3B=1;if(!_tmp3B){{int _tmp3C=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);return _tmp3C;};_pop_handler();}else{void*_tmp3A=(void*)_exn_thrown;void*_tmp3D=_tmp3A;void*_tmp3E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3D)->tag == Cyc_Dict_Absent){_LL6: _LL7:
 goto _LL5;}else{_LL8: _tmp3E=_tmp3D;_LL9:(int)_rethrow(_tmp3E);}_LL5:;}};}{
int _tmp3F=1;
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp3F);
if(d->fields != 0){
struct Cyc_List_List*_tmp40=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){
const char*_tmp261;if(!((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,((_tmp261="datatype",_tag_dyneither(_tmp261,sizeof(char),9))),d->name,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)_tmp40->hd)->typs))
# 241
_tmp3F=0;}}
# 243
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp3F);
return _tmp3F;};}
# 247
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 251
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn != 0)
return Cyc_Interface_check_public_type(ae,seen,_tag_dyneither(0,0,0),d->name,(void*)_check_null(d->defn));else{
return 1;}}
# 258
static int Cyc_Interface_check_public_vardecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
const char*_tmp262;return Cyc_Interface_check_public_type(ae,seen,((_tmp262="variable",_tag_dyneither(_tmp262,sizeof(char),9))),d->name,d->type);}
# 262
static struct Cyc_List_List*Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace ns){
union Cyc_Absyn_Nmspace _tmp43=ns;struct Cyc_List_List*_tmp44;struct Cyc_List_List*_tmp45;switch((_tmp43.C_n).tag){case 2: _LLB: _tmp45=(_tmp43.Abs_n).val;_LLC:
 return _tmp45;case 3: _LLD: _tmp44=(_tmp43.C_n).val;_LLE:
 return _tmp44;default: _LLF: _LL10: {
const char*_tmp263;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp263="get_abs_ns",_tag_dyneither(_tmp263,sizeof(char),11))));}}_LLA:;}
# 270
static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t){
void*_tmp47=Cyc_Tcutil_compress(t);void*_tmp48=_tmp47;struct Cyc_Absyn_Datatypedecl*_tmp49;struct Cyc_Absyn_Datatypefield*_tmp4A;struct Cyc_List_List*_tmp4B;struct Cyc_Absyn_Datatypedecl*_tmp4C;struct Cyc_List_List*_tmp4D;struct _tuple0*_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;void*_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_Absyn_VarargInfo*_tmp54;void*_tmp55;void*_tmp56;void*_tmp57;switch(*((int*)_tmp48)){case 5: _LL12: _tmp57=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).elt_typ;_LL13:
 _tmp56=_tmp57;goto _LL15;case 8: _LL14: _tmp56=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).elt_type;_LL15:
 _tmp55=_tmp56;goto _LL17;case 17: _LL16: _tmp55=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp48)->f4;if(_tmp55 != 0){_LL17:
# 275
 return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp55);}else{goto _LL24;}case 9: _LL18: _tmp52=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48)->f1).ret_typ;_tmp53=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48)->f1).args;_tmp54=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48)->f1).cyc_varargs;_LL19: {
# 279
int b=((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple8*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,_tmp53) && 
Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp52);
if(_tmp54 != 0){
struct Cyc_Absyn_VarargInfo _tmp58=*_tmp54;struct Cyc_Absyn_VarargInfo _tmp59=_tmp58;void*_tmp5A;_LL27: _tmp5A=_tmp59.type;_LL28:
 b=Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp5A);_LL26:;}
# 285
return b;}case 10: _LL1A: _tmp51=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp48)->f1;_LL1B:
# 288
 return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,_tmp51);case 11: _LL1C: _tmp4F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48)->f1).aggr_info;_tmp50=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48)->f1).targs;_LL1D: {
# 291
struct Cyc_Absyn_Aggrdecl*_tmp5B=Cyc_Absyn_get_known_aggrdecl(_tmp4F);
if(_tmp5B->sc == Cyc_Absyn_Static){
{const char*_tmp264;Cyc_Interface_static_err(obj,name,((_tmp264="type",_tag_dyneither(_tmp264,sizeof(char),5))),_tmp5B->name);}
return 0;}
# 296
return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp50) && 
Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp5B);}case 13: _LL1E: _tmp4E=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp48)->f1;_LL1F: {
# 300
struct _tuple0*_tmp5D=_tmp4E;union Cyc_Absyn_Nmspace _tmp5E;struct _dyneither_ptr*_tmp5F;_LL2C: _tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;_LL2D: {
struct Cyc_List_List*_tmp60=Cyc_Interface_get_abs_ns(_tmp5E);
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _tmp61;_push_handler(& _tmp61);{int _tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!_tmp63){
{struct Cyc_Tcenv_Genv*_tmp64=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp60);
ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp64->enumdecls,_tmp5F);}
# 304
;_pop_handler();}else{void*_tmp62=(void*)_exn_thrown;void*_tmp65=_tmp62;void*_tmp66;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp65)->tag == Cyc_Dict_Absent){_LL31: _LL32: {
# 308
const char*_tmp268;void*_tmp267[1];struct Cyc_String_pa_PrintArg_struct _tmp266;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp266.tag=0,((_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4E)),((_tmp267[0]=& _tmp266,Cyc_aprintf(((_tmp268="check_public_type (can't find enum %s)",_tag_dyneither(_tmp268,sizeof(char),39))),_tag_dyneither(_tmp267,sizeof(void*),1)))))))));}}else{_LL33: _tmp66=_tmp65;_LL34:(int)_rethrow(_tmp66);}_LL30:;}};}
# 311
if(ed->sc == Cyc_Absyn_Static){
{const char*_tmp269;Cyc_Interface_static_err(obj,_tmp4E,((_tmp269="enum",_tag_dyneither(_tmp269,sizeof(char),5))),ed->name);}
return 0;}
# 315
return 1;}_LL2B:;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp48)->f1).datatype_info).KnownDatatype).tag == 2){_LL20: _tmp4C=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp48)->f1).datatype_info).KnownDatatype).val;_tmp4D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp48)->f1).targs;_LL21: {
# 318
struct _tuple0*_tmp6B=_tmp4C->name;struct _tuple0*_tmp6C=_tmp6B;union Cyc_Absyn_Nmspace _tmp6D;struct _dyneither_ptr*_tmp6E;_LL36: _tmp6D=_tmp6C->f1;_tmp6E=_tmp6C->f2;_LL37: {
struct Cyc_List_List*_tmp6F=Cyc_Interface_get_abs_ns(_tmp6D);
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp70;_push_handler(& _tmp70);{int _tmp72=0;if(setjmp(_tmp70.handler))_tmp72=1;if(!_tmp72){
{struct Cyc_Tcenv_Genv*_tmp73=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp6F);
tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp73->datatypedecls,_tmp6E);}
# 322
;_pop_handler();}else{void*_tmp71=(void*)_exn_thrown;void*_tmp74=_tmp71;void*_tmp75;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp74)->tag == Cyc_Dict_Absent){_LL3B: _LL3C: {
# 325
const char*_tmp26D;void*_tmp26C[1];struct Cyc_String_pa_PrintArg_struct _tmp26B;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp26B.tag=0,((_tmp26B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4C->name)),((_tmp26C[0]=& _tmp26B,Cyc_aprintf(((_tmp26D="check_public_type (can't find datatype %s)",_tag_dyneither(_tmp26D,sizeof(char),43))),_tag_dyneither(_tmp26C,sizeof(void*),1)))))))));}}else{_LL3D: _tmp75=_tmp74;_LL3E:(int)_rethrow(_tmp75);}_LL3A:;}};}
# 328
if(tud->sc == Cyc_Absyn_Static){
{const char*_tmp26E;Cyc_Interface_static_err(obj,name,((_tmp26E="datatype",_tag_dyneither(_tmp26E,sizeof(char),9))),tud->name);}
return 0;}
# 332
return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp4D) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}_LL35:;}}else{goto _LL24;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp48)->f1).field_info).KnownDatatypefield).tag == 2){_LL22: _tmp49=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp48)->f1).field_info).KnownDatatypefield).val).f1;_tmp4A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp48)->f1).field_info).KnownDatatypefield).val).f2;_tmp4B=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp48)->f1).targs;_LL23: {
# 336
struct _tuple0*_tmp7A=_tmp49->name;struct _tuple0*_tmp7B=_tmp7A;union Cyc_Absyn_Nmspace _tmp7C;struct _dyneither_ptr*_tmp7D;_LL40: _tmp7C=_tmp7B->f1;_tmp7D=_tmp7B->f2;_LL41: {
struct Cyc_List_List*_tmp7E=Cyc_Interface_get_abs_ns(_tmp7C);
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){
{struct Cyc_Tcenv_Genv*_tmp82=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp7E);
tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp82->datatypedecls,_tmp7D);}
# 340
;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*_tmp83=_tmp80;void*_tmp84;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp83)->tag == Cyc_Dict_Absent){_LL45: _LL46: {
# 344
const char*_tmp272;void*_tmp271[1];struct Cyc_String_pa_PrintArg_struct _tmp270;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp270.tag=0,((_tmp270.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp49->name)),((_tmp271[0]=& _tmp270,Cyc_aprintf(((_tmp272="check_public_type (can't find datatype %s and search its fields)",_tag_dyneither(_tmp272,sizeof(char),65))),_tag_dyneither(_tmp271,sizeof(void*),1)))))))));}}else{_LL47: _tmp84=_tmp83;_LL48:(int)_rethrow(_tmp84);}_LL44:;}};}
# 347
if(tud->fields == 0){
const char*_tmp276;void*_tmp275[1];struct Cyc_String_pa_PrintArg_struct _tmp274;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp274.tag=0,((_tmp274.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tud->name)),((_tmp275[0]=& _tmp274,Cyc_aprintf(((_tmp276="check_public_type (datatype %s has no fields)",_tag_dyneither(_tmp276,sizeof(char),46))),_tag_dyneither(_tmp275,sizeof(void*),1)))))))));}{
# 351
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*_tmp8B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){
if(Cyc_strptrcmp((*_tmp4A->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp8B->hd)->name).f2)== 0){
tuf1=(struct Cyc_Absyn_Datatypefield*)_tmp8B->hd;
break;}}}
# 358
if(tuf1 == 0){
const char*_tmp27A;void*_tmp279[1];struct Cyc_String_pa_PrintArg_struct _tmp278;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp278.tag=0,((_tmp278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4A->name)),((_tmp279[0]=& _tmp278,Cyc_aprintf(((_tmp27A="check_public_type (can't find datatypefield %s)",_tag_dyneither(_tmp27A,sizeof(char),48))),_tag_dyneither(_tmp279,sizeof(void*),1)))))))));}{
struct Cyc_Absyn_Datatypefield*tuf=tuf1;
# 363
if(tud->sc == Cyc_Absyn_Static){
{const char*_tmp27B;Cyc_Interface_static_err(obj,name,((_tmp27B="datatype",_tag_dyneither(_tmp27B,sizeof(char),9))),tud->name);}
return 0;}
# 367
if(tud->sc == Cyc_Absyn_Abstract){
{const char*_tmp27C;Cyc_Interface_abstract_err(obj,name,((_tmp27C="datatype",_tag_dyneither(_tmp27C,sizeof(char),9))),tud->name);}
return 0;}
# 371
if(tuf->sc == Cyc_Absyn_Static){
{const char*_tmp280;void*_tmp27F[1];struct Cyc_String_pa_PrintArg_struct _tmp27E;Cyc_Interface_static_err(obj,name,(struct _dyneither_ptr)(
(_tmp27E.tag=0,((_tmp27E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp27F[0]=& _tmp27E,Cyc_aprintf(((_tmp280="field %s of",_tag_dyneither(_tmp280,sizeof(char),12))),_tag_dyneither(_tmp27F,sizeof(void*),1)))))))),tud->name);}
return 0;}
# 376
return((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp4B) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};}_LL3F:;}}else{goto _LL24;}default: _LL24: _LL25:
# 379
 return 1;}_LL11:;}struct _tuple21{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Dict_Dict f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*f6;};
# 385
static void Cyc_Interface_extract_aggrdecl(struct _tuple21*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp){
# 387
struct _tuple21*_tmp94=env;struct Cyc_Interface_Ienv*_tmp95;struct Cyc_Interface_Ienv*_tmp96;int _tmp97;struct Cyc_Dict_Dict _tmp98;struct Cyc_Interface_Seen*_tmp99;_LL4A: _tmp95=_tmp94->f1;_tmp96=_tmp94->f2;_tmp97=_tmp94->f3;_tmp98=_tmp94->f4;_tmp99=_tmp94->f5;_LL4B: {
struct Cyc_Absyn_Aggrdecl*_tmp9A=*dp;
enum Cyc_Absyn_Scope _tmp9B=_tmp9A->sc;enum Cyc_Absyn_Scope _tmp9C=_tmp9B;switch(_tmp9C){case Cyc_Absyn_Static: _LL4F: _LL50:
# 391
 if(_tmp97  && _tmp9A->impl == 0){
const char*_tmp282;const char*_tmp281;Cyc_Interface_fields_err(((_tmp281="static",_tag_dyneither(_tmp281,sizeof(char),7))),((_tmp282="struct/union",_tag_dyneither(_tmp282,sizeof(char),13))),_tmp9A->name);}
goto _LL4E;case Cyc_Absyn_Abstract: _LL51: _LL52:
# 395
 if(_tmp9A->impl == 0){
if(_tmp97){
const char*_tmp284;const char*_tmp283;Cyc_Interface_fields_err(((_tmp283="abstract",_tag_dyneither(_tmp283,sizeof(char),9))),((_tmp284="struct/union",_tag_dyneither(_tmp284,sizeof(char),13))),_tmp9A->name);}}else{
# 399
{struct Cyc_Absyn_Aggrdecl*_tmp285;_tmp9A=((_tmp285=_cycalloc(sizeof(*_tmp285)),((_tmp285[0]=*_tmp9A,_tmp285))));}
_tmp9A->impl=0;}
# 402
if(Cyc_Interface_check_public_aggrdecl(_tmp98,_tmp99,_tmp9A))
_tmp96->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp96->aggrdecls,x,_tmp9A);
goto _LL4E;case Cyc_Absyn_Public: _LL53: _LL54:
# 406
 if(_tmp9A->impl == 0){
{const char*_tmp287;const char*_tmp286;Cyc_Interface_fields_err(((_tmp286="public",_tag_dyneither(_tmp286,sizeof(char),7))),((_tmp287="struct/union",_tag_dyneither(_tmp287,sizeof(char),13))),_tmp9A->name);}
{struct Cyc_Absyn_Aggrdecl*_tmp288;_tmp9A=((_tmp288=_cycalloc(sizeof(*_tmp288)),((_tmp288[0]=*_tmp9A,_tmp288))));}
_tmp9A->sc=Cyc_Absyn_Abstract;}
# 411
if(Cyc_Interface_check_public_aggrdecl(_tmp98,_tmp99,_tmp9A))
_tmp96->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp96->aggrdecls,x,_tmp9A);
goto _LL4E;case Cyc_Absyn_ExternC: _LL55: _LL56:
 goto _LL58;case Cyc_Absyn_Extern: _LL57: _LL58:
# 416
 if(Cyc_Interface_check_public_aggrdecl(_tmp98,_tmp99,_tmp9A))
_tmp95->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp95->aggrdecls,x,_tmp9A);
goto _LL4E;default: _LL59: _LL5A:
# 420
{const char*_tmp289;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp289="add_aggrdecl",_tag_dyneither(_tmp289,sizeof(char),13))));}
goto _LL4E;}_LL4E:;}_LL49:;}
# 424
static void Cyc_Interface_extract_enumdecl(struct _tuple21*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl**dp){
# 426
struct _tuple21*_tmpA6=env;struct Cyc_Interface_Ienv*_tmpA7;struct Cyc_Interface_Ienv*_tmpA8;int _tmpA9;struct Cyc_Dict_Dict _tmpAA;struct Cyc_Interface_Seen*_tmpAB;_LL5C: _tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;_tmpA9=_tmpA6->f3;_tmpAA=_tmpA6->f4;_tmpAB=_tmpA6->f5;_LL5D: {
struct Cyc_Absyn_Enumdecl*_tmpAC=*dp;
enum Cyc_Absyn_Scope _tmpAD=_tmpAC->sc;enum Cyc_Absyn_Scope _tmpAE=_tmpAD;switch(_tmpAE){case Cyc_Absyn_Static: _LL61: _LL62:
# 430
 if(_tmpA9  && _tmpAC->fields == 0){
const char*_tmp28B;const char*_tmp28A;Cyc_Interface_fields_err(((_tmp28A="static",_tag_dyneither(_tmp28A,sizeof(char),7))),((_tmp28B="enum",_tag_dyneither(_tmp28B,sizeof(char),5))),_tmpAC->name);}
goto _LL60;case Cyc_Absyn_Abstract: _LL63: _LL64:
# 434
 if(_tmpAC->fields == 0){
if(_tmpA9){
const char*_tmp28D;const char*_tmp28C;Cyc_Interface_fields_err(((_tmp28C="abstract",_tag_dyneither(_tmp28C,sizeof(char),9))),((_tmp28D="enum",_tag_dyneither(_tmp28D,sizeof(char),5))),_tmpAC->name);}}else{
# 438
{struct Cyc_Absyn_Enumdecl*_tmp28E;_tmpAC=((_tmp28E=_cycalloc(sizeof(*_tmp28E)),((_tmp28E[0]=*_tmpAC,_tmp28E))));}
_tmpAC->fields=0;}
# 441
if(Cyc_Interface_check_public_enumdecl(_tmpAA,_tmpAB,_tmpAC))
_tmpA8->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpA8->enumdecls,x,_tmpAC);
goto _LL60;case Cyc_Absyn_Public: _LL65: _LL66:
# 445
 if(_tmpAC->fields == 0){
{const char*_tmp290;const char*_tmp28F;Cyc_Interface_fields_err(((_tmp28F="public",_tag_dyneither(_tmp28F,sizeof(char),7))),((_tmp290="enum",_tag_dyneither(_tmp290,sizeof(char),5))),_tmpAC->name);}
{struct Cyc_Absyn_Enumdecl*_tmp291;_tmpAC=((_tmp291=_cycalloc(sizeof(*_tmp291)),((_tmp291[0]=*_tmpAC,_tmp291))));}
_tmpAC->sc=Cyc_Absyn_Abstract;}
# 450
if(Cyc_Interface_check_public_enumdecl(_tmpAA,_tmpAB,_tmpAC))
_tmpA8->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpA8->enumdecls,x,_tmpAC);
goto _LL60;case Cyc_Absyn_ExternC: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Extern: _LL69: _LL6A:
# 455
 if(Cyc_Interface_check_public_enumdecl(_tmpAA,_tmpAB,_tmpAC))
_tmpA7->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpA7->enumdecls,x,_tmpAC);
goto _LL60;default: _LL6B: _LL6C:
# 459
{const char*_tmp292;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp292="add_enumdecl",_tag_dyneither(_tmp292,sizeof(char),13))));}
goto _LL60;}_LL60:;}_LL5B:;}
# 464
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 466
struct Cyc_List_List*_tmpB8=0;
{union Cyc_Absyn_Nmspace _tmpB9=(*f->name).f1;union Cyc_Absyn_Nmspace _tmpBA=_tmpB9;struct Cyc_List_List*_tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_List_List*_tmpBD;switch((_tmpBA.C_n).tag){case 1: _LL6E: _tmpBD=(_tmpBA.Rel_n).val;_LL6F:
 _tmpBC=_tmpBD;goto _LL71;case 2: _LL70: _tmpBC=(_tmpBA.Abs_n).val;_LL71:
 _tmpB8=_tmpBC;goto _LL6D;case 3: _LL72: _tmpBB=(_tmpBA.C_n).val;_LL73:
 _tmpB8=_tmpBB;goto _LL6D;default: _LL74: _LL75:
 goto _LL6D;}_LL6D:;}{
# 473
struct Cyc_Dict_Dict*dict;
{enum Cyc_Absyn_Scope _tmpBE=f->sc;enum Cyc_Absyn_Scope _tmpBF=_tmpBE;switch(_tmpBF){case Cyc_Absyn_Static: _LL77: _LL78:
 return;case Cyc_Absyn_Extern: _LL79: _LL7A:
 dict=& i->imports;goto _LL76;case Cyc_Absyn_Public: _LL7B: _LL7C:
 dict=& i->exports;goto _LL76;default: _LL7D: _LL7E: {
const char*_tmp293;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp293="add_xdatatypefielddecl",_tag_dyneither(_tmp293,sizeof(char),23))));}}_LL76:;}{
# 481
struct Cyc_Interface_Ienv*env;
{struct _handler_cons _tmpC1;_push_handler(& _tmpC1);{int _tmpC3=0;if(setjmp(_tmpC1.handler))_tmpC3=1;if(!_tmpC3){
env=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpB8);;_pop_handler();}else{void*_tmpC2=(void*)_exn_thrown;void*_tmpC4=_tmpC2;void*_tmpC5;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC4)->tag == Cyc_Dict_Absent){_LL80: _LL81:
# 486
 env=Cyc_Interface_new_ienv();
*dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*dict,_tmpB8,env);
goto _LL7F;}else{_LL82: _tmpC5=_tmpC4;_LL83:(int)_rethrow(_tmpC5);}_LL7F:;}};}{
# 490
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp294;env->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(env->xdatatypefielddecls,(*f->name).f2,((_tmp294=_cycalloc(sizeof(*_tmp294)),((_tmp294->base=d,((_tmp294->field=f,_tmp294)))))));};};};}
# 494
static void Cyc_Interface_extract_datatypedecl(struct _tuple21*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl**dp){
# 496
struct _tuple21*_tmpC7=env;struct Cyc_Interface_Ienv*_tmpC8;struct Cyc_Interface_Ienv*_tmpC9;int _tmpCA;struct Cyc_Dict_Dict _tmpCB;struct Cyc_Interface_Seen*_tmpCC;struct Cyc_Interface_I*_tmpCD;_LL85: _tmpC8=_tmpC7->f1;_tmpC9=_tmpC7->f2;_tmpCA=_tmpC7->f3;_tmpCB=_tmpC7->f4;_tmpCC=_tmpC7->f5;_tmpCD=_tmpC7->f6;_LL86: {
struct Cyc_Absyn_Datatypedecl*_tmpCE=*dp;
# 499
enum Cyc_Absyn_Scope _tmpCF=_tmpCE->sc;enum Cyc_Absyn_Scope _tmpD0=_tmpCF;switch(_tmpD0){case Cyc_Absyn_Static: _LL8A: _LL8B:
# 501
 if((!_tmpCE->is_extensible  && _tmpCA) && _tmpCE->fields == 0){
const char*_tmp296;const char*_tmp295;Cyc_Interface_fields_err(((_tmp295="static",_tag_dyneither(_tmp295,sizeof(char),7))),((_tmp296="datatype",_tag_dyneither(_tmp296,sizeof(char),9))),_tmpCE->name);}
# 504
goto _LL89;case Cyc_Absyn_Abstract: _LL8C: _LL8D:
# 506
 if(_tmpCE->fields == 0){
if(!_tmpCE->is_extensible  && _tmpCA){
const char*_tmp298;const char*_tmp297;Cyc_Interface_fields_err(((_tmp297="abstract",_tag_dyneither(_tmp297,sizeof(char),9))),((_tmp298="datatype",_tag_dyneither(_tmp298,sizeof(char),9))),_tmpCE->name);}}else{
# 511
{struct Cyc_Absyn_Datatypedecl*_tmp299;_tmpCE=((_tmp299=_cycalloc(sizeof(*_tmp299)),((_tmp299[0]=*_tmpCE,_tmp299))));}
_tmpCE->fields=0;}
# 514
if(Cyc_Interface_check_public_datatypedecl(_tmpCB,_tmpCC,_tmpCE))
_tmpC9->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpC9->datatypedecls,x,_tmpCE);
goto _LL89;case Cyc_Absyn_Public: _LL8E: _LL8F:
# 518
{struct Cyc_Absyn_Datatypedecl*_tmp29A;_tmpCE=((_tmp29A=_cycalloc(sizeof(*_tmp29A)),((_tmp29A[0]=*_tmpCE,_tmp29A))));}
if(!_tmpCE->is_extensible  && _tmpCE->fields == 0){
{const char*_tmp29C;const char*_tmp29B;Cyc_Interface_fields_err(((_tmp29B="public",_tag_dyneither(_tmp29B,sizeof(char),7))),((_tmp29C="datatype",_tag_dyneither(_tmp29C,sizeof(char),9))),_tmpCE->name);}
_tmpCE->sc=Cyc_Absyn_Abstract;}
# 523
if(Cyc_Interface_check_public_datatypedecl(_tmpCB,_tmpCC,_tmpCE)){
if(_tmpCE->is_extensible  && _tmpCE->fields != 0){
struct Cyc_List_List*_tmpD9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpCE->fields))->v;
_tmpCE->fields=0;{
struct Cyc_List_List*_tmpDA=_tmpD9;for(0;_tmpDA != 0;_tmpDA=_tmpDA->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpCD,_tmpCE,(struct Cyc_Absyn_Datatypefield*)_tmpDA->hd);}};}
# 531
_tmpC9->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpC9->datatypedecls,x,_tmpCE);}
# 533
goto _LL89;case Cyc_Absyn_ExternC: _LL90: _LL91: {
const char*_tmp29D;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp29D="extract_datatypedecl",_tag_dyneither(_tmp29D,sizeof(char),21))));}case Cyc_Absyn_Extern: _LL92: _LL93:
# 536
 if(Cyc_Interface_check_public_datatypedecl(_tmpCB,_tmpCC,_tmpCE)){
if(_tmpCE->is_extensible  && _tmpCE->fields != 0){
{struct Cyc_Absyn_Datatypedecl*_tmp29E;_tmpCE=((_tmp29E=_cycalloc(sizeof(*_tmp29E)),((_tmp29E[0]=*_tmpCE,_tmp29E))));}{
struct Cyc_List_List*_tmpDD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpCE->fields))->v;
_tmpCE->fields=0;{
struct Cyc_List_List*_tmpDE=_tmpDD;for(0;_tmpDE != 0;_tmpDE=_tmpDE->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpCD,_tmpCE,(struct Cyc_Absyn_Datatypefield*)_tmpDE->hd);}};};}
# 545
_tmpC8->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpC8->datatypedecls,x,_tmpCE);}
# 547
goto _LL89;default: _LL94: _LL95:
# 549
{const char*_tmp29F;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp29F="add_datatypedecl",_tag_dyneither(_tmp29F,sizeof(char),17))));}
goto _LL89;}_LL89:;}_LL84:;}
# 554
static void Cyc_Interface_extract_typedef(struct _tuple21*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d){
# 556
struct _tuple21*_tmpE0=env;struct Cyc_Interface_Ienv*_tmpE1;struct Cyc_Dict_Dict _tmpE2;struct Cyc_Interface_Seen*_tmpE3;_LL97: _tmpE1=_tmpE0->f2;_tmpE2=_tmpE0->f4;_tmpE3=_tmpE0->f5;_LL98:
 if(Cyc_Interface_check_public_typedefdecl(_tmpE2,_tmpE3,d))
_tmpE1->typedefdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpE1->typedefdecls,x,d);_LL96:;}struct _tuple22{void*f1;int f2;};
# 561
static void Cyc_Interface_extract_ordinarie(struct _tuple21*env,struct _dyneither_ptr*x,struct _tuple22*v){
# 563
struct _tuple21*_tmpE4=env;struct Cyc_Interface_Ienv*_tmpE5;struct Cyc_Interface_Ienv*_tmpE6;int _tmpE7;struct Cyc_Dict_Dict _tmpE8;struct Cyc_Interface_Seen*_tmpE9;_LL9C: _tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;_tmpE7=_tmpE4->f3;_tmpE8=_tmpE4->f4;_tmpE9=_tmpE4->f5;_LL9D: {
void*_tmpEA=(*v).f1;
# 566
void*_tmpEB=_tmpEA;void*_tmpEC;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpEB)->tag == 0){_LLA1: _tmpEC=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpEB)->f1;_LLA2:
# 568
{void*_tmpED=_tmpEC;struct Cyc_Absyn_Vardecl*_tmpEE;struct Cyc_Absyn_Fndecl*_tmpEF;switch(*((int*)_tmpED)){case 2: _LLA6: _tmpEF=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpED)->f1;_LLA7: {
# 570
struct Cyc_Absyn_Vardecl*_tmp2A0;struct Cyc_Absyn_Vardecl*_tmpF0=(_tmp2A0=_cycalloc(sizeof(*_tmp2A0)),((_tmp2A0->sc=_tmpEF->sc,((_tmp2A0->name=_tmpEF->name,((_tmp2A0->tq=
# 573
Cyc_Absyn_empty_tqual(0),((_tmp2A0->type=(void*)_check_null(_tmpEF->cached_typ),((_tmp2A0->initializer=0,((_tmp2A0->rgn=0,((_tmp2A0->attributes=0,((_tmp2A0->escapes=0,_tmp2A0)))))))))))))))));
# 580
_tmpE7=0;
_tmpEE=_tmpF0;goto _LLA9;}case 1: _LLA8: _tmpEE=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpED)->f1;_LLA9:
# 583
 if(_tmpEE->initializer != 0){
{struct Cyc_Absyn_Vardecl*_tmp2A1;_tmpEE=((_tmp2A1=_cycalloc(sizeof(*_tmp2A1)),((_tmp2A1[0]=*_tmpEE,_tmp2A1))));}
_tmpEE->initializer=0;}
# 587
{enum Cyc_Absyn_Scope _tmpF3=_tmpEE->sc;enum Cyc_Absyn_Scope _tmpF4=_tmpF3;switch(_tmpF4){case Cyc_Absyn_Static: _LLAD: _LLAE:
# 589
 if(_tmpE7  && Cyc_Tcutil_is_function_type(_tmpEE->type)){
const char*_tmp2A2;Cyc_Interface_body_err(((_tmp2A2="static",_tag_dyneither(_tmp2A2,sizeof(char),7))),_tmpEE->name);}
# 592
goto _LLAC;case Cyc_Absyn_Register: _LLAF: _LLB0:
 goto _LLB2;case Cyc_Absyn_Abstract: _LLB1: _LLB2: {
const char*_tmp2A3;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2A3="extract_ordinarie",_tag_dyneither(_tmp2A3,sizeof(char),18))));}case Cyc_Absyn_Public: _LLB3: _LLB4:
# 596
 if(_tmpE7  && Cyc_Tcutil_is_function_type(_tmpEE->type)){
const char*_tmp2A4;Cyc_Interface_body_err(((_tmp2A4="public",_tag_dyneither(_tmp2A4,sizeof(char),7))),_tmpEE->name);}
# 599
if(Cyc_Interface_check_public_vardecl(_tmpE8,_tmpE9,_tmpEE))
_tmpE6->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpE6->vardecls,x,_tmpEE);
goto _LLAC;case Cyc_Absyn_ExternC: _LLB5: _LLB6:
 goto _LLB8;default: _LLB7: _LLB8:
# 604
 if(Cyc_Interface_check_public_vardecl(_tmpE8,_tmpE9,_tmpEE))
_tmpE5->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpE5->vardecls,x,_tmpEE);
goto _LLAC;}_LLAC:;}
# 608
goto _LLA5;default: _LLAA: _LLAB:
 goto _LLA5;}_LLA5:;}
# 611
goto _LLA0;}else{_LLA3: _LLA4:
 goto _LLA0;}_LLA0:;}_LL9B:;}struct _tuple23{struct Cyc_Interface_I*f1;int f2;struct Cyc_Dict_Dict f3;struct Cyc_Interface_Seen*f4;};
# 616
static void Cyc_Interface_extract_f(struct _tuple23*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge){
# 618
struct _tuple23*_tmpF8=env_f;struct Cyc_Interface_I*_tmpF9;int _tmpFA;struct Cyc_Dict_Dict _tmpFB;struct Cyc_Interface_Seen*_tmpFC;_LLBA: _tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;_tmpFB=_tmpF8->f3;_tmpFC=_tmpF8->f4;_LLBB: {
struct _tuple21 _tmp2A5;struct _tuple21 _tmpFD=(_tmp2A5.f1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpF9->imports,ns),((_tmp2A5.f2=
((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpF9->exports,ns),((_tmp2A5.f3=_tmpFA,((_tmp2A5.f4=_tmpFB,((_tmp2A5.f5=_tmpFC,((_tmp2A5.f6=_tmpF9,_tmp2A5)))))))))));
# 624
((void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmpFD,ge->aggrdecls);
((void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,& _tmpFD,ge->datatypedecls);
((void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmpFD,ge->enumdecls);
((void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmpFD,ge->typedefs);
((void(*)(void(*f)(struct _tuple21*,struct _dyneither_ptr*,struct _tuple22*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmpFD,ge->ordinaries);}_LLB9:;}
# 631
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Dict_Dict ae,int check_complete_defs){
# 633
struct _tuple23 _tmp2A6;struct _tuple23 env=
(_tmp2A6.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict skel))Cyc_Interface_skel2i)(ae),((_tmp2A6.f2=check_complete_defs,((_tmp2A6.f3=ae,((_tmp2A6.f4=Cyc_Interface_new_seen(),_tmp2A6)))))));
((void(*)(void(*f)(struct _tuple23*,struct Cyc_List_List*,struct Cyc_Tcenv_Genv*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_f,& env,ae);
return env.f1;}
# 639
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae){
return Cyc_Interface_gen_extract(ae,1);}
# 645
inline static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){
const char*_tmp2AA;void*_tmp2A9[1];struct Cyc_String_pa_PrintArg_struct _tmp2A8;(_tmp2A8.tag=0,((_tmp2A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg2),((_tmp2A9[0]=& _tmp2A8,Cyc_Tcdecl_merr(0,msg1,((_tmp2AA="%s",_tag_dyneither(_tmp2AA,sizeof(char),3))),_tag_dyneither(_tmp2A9,sizeof(void*),1)))))));}struct _tuple24{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 649
static void Cyc_Interface_incl_dict_f(struct _tuple24*env,struct _dyneither_ptr*x,void*y1){
# 656
struct _tuple24*_tmp103=env;int*_tmp104;struct Cyc_Dict_Dict _tmp105;int(*_tmp106)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp107;struct _dyneither_ptr*_tmp108;_LLBF: _tmp104=(int*)& _tmp103->f1;_tmp105=_tmp103->f2;_tmp106=_tmp103->f3;_tmp107=_tmp103->f4;_tmp108=_tmp103->f5;_LLC0: {
struct _handler_cons _tmp109;_push_handler(& _tmp109);{int _tmp10B=0;if(setjmp(_tmp109.handler))_tmp10B=1;if(!_tmp10B){
{void*_tmp10C=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp105,x);
if(!_tmp106(y1,_tmp10C,_tmp108))*_tmp104=0;}
# 658
;_pop_handler();}else{void*_tmp10A=(void*)_exn_thrown;void*_tmp10D=_tmp10A;void*_tmp10E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp10D)->tag == Cyc_Dict_Absent){_LLC4: _LLC5:
# 662
{const char*_tmp2AF;void*_tmp2AE[2];struct Cyc_String_pa_PrintArg_struct _tmp2AD;struct Cyc_String_pa_PrintArg_struct _tmp2AC;Cyc_Interface_check_err(_tmp108,(struct _dyneither_ptr)((_tmp2AC.tag=0,((_tmp2AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp2AD.tag=0,((_tmp2AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107),((_tmp2AE[0]=& _tmp2AD,((_tmp2AE[1]=& _tmp2AC,Cyc_aprintf(((_tmp2AF="%s %s is missing",_tag_dyneither(_tmp2AF,sizeof(char),17))),_tag_dyneither(_tmp2AE,sizeof(void*),2)))))))))))))));}
*_tmp104=0;
goto _LLC3;}else{_LLC6: _tmp10E=_tmp10D;_LLC7:(int)_rethrow(_tmp10E);}_LLC3:;}};}_LLBE:;}
# 668
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 673
struct _tuple24 _tmp2B0;struct _tuple24 _tmp113=(_tmp2B0.f1=1,((_tmp2B0.f2=dic2,((_tmp2B0.f3=incl_f,((_tmp2B0.f4=t,((_tmp2B0.f5=msg,_tmp2B0)))))))));
((void(*)(void(*f)(struct _tuple24*,struct _dyneither_ptr*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp113,dic1);
return _tmp113.f1;}
# 691 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmp115=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmp115 == 0)return 0;if(d0 != _tmp115){{const char*_tmp2B4;void*_tmp2B3[1];struct Cyc_String_pa_PrintArg_struct _tmp2B2;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2B2.tag=0,((_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2B3[0]=& _tmp2B2,Cyc_aprintf(((_tmp2B4="declaration of type %s discloses too much information",_tag_dyneither(_tmp2B4,sizeof(char),54))),_tag_dyneither(_tmp2B3,sizeof(void*),1)))))))));}return 0;}return 1;}
# 693
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmp119=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0,msg);if(_tmp119 == 0)return 0;if(d0 != _tmp119){{const char*_tmp2B8;void*_tmp2B7[1];struct Cyc_String_pa_PrintArg_struct _tmp2B6;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2B6.tag=0,((_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2B7[0]=& _tmp2B6,Cyc_aprintf(((_tmp2B8="declaration of datatype %s discloses too much information",_tag_dyneither(_tmp2B8,sizeof(char),58))),_tag_dyneither(_tmp2B7,sizeof(void*),1)))))))));}return 0;}return 1;}
# 695
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*_tmp11D=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp11D == 0)return 0;if(d0 != _tmp11D){{const char*_tmp2BC;void*_tmp2BB[1];struct Cyc_String_pa_PrintArg_struct _tmp2BA;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2BA.tag=0,((_tmp2BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2BB[0]=& _tmp2BA,Cyc_aprintf(((_tmp2BC="declaration of enum %s discloses too much information",_tag_dyneither(_tmp2BC,sizeof(char),54))),_tag_dyneither(_tmp2BB,sizeof(void*),1)))))))));}return 0;}return 1;}
# 697
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*_tmp121=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp121 == 0)return 0;if(d0 != _tmp121){{const char*_tmp2C0;void*_tmp2BF[1];struct Cyc_String_pa_PrintArg_struct _tmp2BE;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2BE.tag=0,((_tmp2BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2BF[0]=& _tmp2BE,Cyc_aprintf(((_tmp2C0="declaration of variable %s discloses too much information",_tag_dyneither(_tmp2C0,sizeof(char),58))),_tag_dyneither(_tmp2BF,sizeof(void*),1)))))))));}return 0;}return 1;}
# 699
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp125=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp125 == 0)return 0;if(d0 != _tmp125){{const char*_tmp2C4;void*_tmp2C3[1];struct Cyc_String_pa_PrintArg_struct _tmp2C2;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2C2.tag=0,((_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2C3[0]=& _tmp2C2,Cyc_aprintf(((_tmp2C4="declaration of typedef %s discloses too much information",_tag_dyneither(_tmp2C4,sizeof(char),57))),_tag_dyneither(_tmp2C3,sizeof(void*),1)))))))));}return 0;}return 1;}
# 701
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp129=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0,msg);if(_tmp129 == 0)return 0;if(d0 != _tmp129){{const char*_tmp2C8;void*_tmp2C7[1];struct Cyc_String_pa_PrintArg_struct _tmp2C6;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2C6.tag=0,((_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name)),((_tmp2C7[0]=& _tmp2C6,Cyc_aprintf(((_tmp2C8="declaration of xdatatypefield %s discloses too much information",_tag_dyneither(_tmp2C8,sizeof(char),64))),_tag_dyneither(_tmp2C7,sizeof(void*),1)))))))));}return 0;}return 1;}
# 706
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);
# 715
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
const char*_tmp2C9;int _tmp12D=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,((_tmp2C9="type",_tag_dyneither(_tmp2C9,sizeof(char),5))),msg);
const char*_tmp2CA;int _tmp12E=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,Cyc_Interface_incl_datatypedecl,((_tmp2CA="datatype",_tag_dyneither(_tmp2CA,sizeof(char),9))),msg);
const char*_tmp2CB;int _tmp12F=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,((_tmp2CB="enum",_tag_dyneither(_tmp2CB,sizeof(char),5))),msg);
# 720
const char*_tmp2CC;int _tmp130=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,(
(_tmp2CC="typedef",_tag_dyneither(_tmp2CC,sizeof(char),8))),msg)!= 0;
const char*_tmp2CD;int _tmp131=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,((_tmp2CD="variable",_tag_dyneither(_tmp2CD,sizeof(char),9))),msg);
const char*_tmp2CE;int _tmp132=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,((_tmp2CE="xdatatypefield",_tag_dyneither(_tmp2CE,sizeof(char),15))),msg);
return((((_tmp12D  && _tmp12E) && _tmp12F) && _tmp130) && _tmp131) && _tmp132;}struct _tuple25{int f1;struct Cyc_Dict_Dict f2;struct _dyneither_ptr*f3;};
# 728
static void Cyc_Interface_incl_ns_f(struct _tuple25*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){
# 733
struct _tuple25*_tmp139=env;int*_tmp13A;struct Cyc_Dict_Dict _tmp13B;struct _dyneither_ptr*_tmp13C;_LLC9: _tmp13A=(int*)& _tmp139->f1;_tmp13B=_tmp139->f2;_tmp13C=_tmp139->f3;_LLCA: {
# 735
struct Cyc_Interface_Ienv*ie2;
# 738
{struct _handler_cons _tmp13D;_push_handler(& _tmp13D);{int _tmp13F=0;if(setjmp(_tmp13D.handler))_tmp13F=1;if(!_tmp13F){ie2=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp13B,ns);;_pop_handler();}else{void*_tmp13E=(void*)_exn_thrown;void*_tmp140=_tmp13E;void*_tmp141;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp140)->tag == Cyc_Dict_Absent){_LLCE: _LLCF:
 ie2=Cyc_Interface_lazy_new_ienv();goto _LLCD;}else{_LLD0: _tmp141=_tmp140;_LLD1:(int)_rethrow(_tmp141);}_LLCD:;}};}
# 741
if(!Cyc_Interface_incl_ienv(ie1,ie2,_tmp13C))*_tmp13A=0;}_LLC8:;}
# 744
static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){
# 747
struct _tuple25 _tmp2CF;struct _tuple25 _tmp142=(_tmp2CF.f1=1,((_tmp2CF.f2=dic2,((_tmp2CF.f3=msg,_tmp2CF)))));
((void(*)(void(*f)(struct _tuple25*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,& _tmp142,dic1);
return _tmp142.f1;}
# 752
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp144;_push_handler(& _tmp144);{int _tmp146=0;if(setjmp(_tmp144.handler))_tmp146=1;if(!_tmp146){
{int _tmp147=1;
struct _dyneither_ptr*msg=0;
# 757
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp2D9;struct Cyc_String_pa_PrintArg_struct _tmp2D8;void*_tmp2D7[2];const char*_tmp2D6;struct _dyneither_ptr*_tmp2D5;msg=((_tmp2D5=_cycalloc(sizeof(*_tmp2D5)),((_tmp2D5[0]=(struct _dyneither_ptr)((_tmp2D9.tag=0,((_tmp2D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp2D8.tag=0,((_tmp2D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp2D7[0]=& _tmp2D8,((_tmp2D7[1]=& _tmp2D9,Cyc_aprintf(((_tmp2D6="checking inclusion of %s exports into %s exports,",_tag_dyneither(_tmp2D6,sizeof(char),50))),_tag_dyneither(_tmp2D7,sizeof(void*),2)))))))))))))),_tmp2D5))));}
if(!Cyc_Interface_incl_ns(i1->exports,i2->exports,msg))_tmp147=0;
# 761
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp2E3;struct Cyc_String_pa_PrintArg_struct _tmp2E2;void*_tmp2E1[2];const char*_tmp2E0;struct _dyneither_ptr*_tmp2DF;msg=((_tmp2DF=_cycalloc(sizeof(*_tmp2DF)),((_tmp2DF[0]=(struct _dyneither_ptr)((_tmp2E3.tag=0,((_tmp2E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp2E2.tag=0,((_tmp2E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp2E1[0]=& _tmp2E2,((_tmp2E1[1]=& _tmp2E3,Cyc_aprintf(((_tmp2E0="checking inclusion of %s imports into %s imports,",_tag_dyneither(_tmp2E0,sizeof(char),50))),_tag_dyneither(_tmp2E1,sizeof(void*),2)))))))))))))),_tmp2DF))));}
if(!Cyc_Interface_incl_ns(i2->imports,i1->imports,msg))_tmp147=0;{
# 765
int _tmp152=_tmp147;_npop_handler(0);return _tmp152;};}
# 754
;_pop_handler();}else{void*_tmp145=(void*)_exn_thrown;void*_tmp153=_tmp145;void*_tmp154;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp153)->tag == Cyc_Tcdecl_Incompatible){_LLD3: _LLD4:
# 767
 return 0;}else{_LLD5: _tmp154=_tmp153;_LLD6:(int)_rethrow(_tmp154);}_LLD2:;}};}struct _tuple26{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;void*(*f5)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr f6;struct _dyneither_ptr*f7;};
# 775
void Cyc_Interface_compat_merge_dict_f(struct _tuple26*env,struct _dyneither_ptr*x,void*y2){
# 782
struct _tuple26*_tmp155=env;int*_tmp156;struct Cyc_Dict_Dict*_tmp157;struct Cyc_Dict_Dict _tmp158;struct Cyc_Dict_Dict _tmp159;void*(*_tmp15A)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr _tmp15B;struct _dyneither_ptr*_tmp15C;_LLD8: _tmp156=(int*)& _tmp155->f1;_tmp157=(struct Cyc_Dict_Dict*)& _tmp155->f2;_tmp158=_tmp155->f3;_tmp159=_tmp155->f4;_tmp15A=_tmp155->f5;_tmp15B=_tmp155->f6;_tmp15C=_tmp155->f7;_LLD9: {
void*y;
{struct _handler_cons _tmp15D;_push_handler(& _tmp15D);{int _tmp15F=0;if(setjmp(_tmp15D.handler))_tmp15F=1;if(!_tmp15F){
{void*_tmp160=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp158,x);
# 788
void*_tmp161=_tmp15A(_tmp160,y2,0,_tmp15C);
if(!((unsigned int)_tmp161)){
*_tmp156=0;
_npop_handler(0);return;}
# 793
y=_tmp161;}
# 785
;_pop_handler();}else{void*_tmp15E=(void*)_exn_thrown;void*_tmp162=_tmp15E;void*_tmp163;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp162)->tag == Cyc_Dict_Absent){_LLDD: _LLDE:
# 796
 y=y2;
goto _LLDC;}else{_LLDF: _tmp163=_tmp162;_LLE0:(int)_rethrow(_tmp163);}_LLDC:;}};}{
# 800
struct _handler_cons _tmp164;_push_handler(& _tmp164);{int _tmp166=0;if(setjmp(_tmp164.handler))_tmp166=1;if(!_tmp166){
{void*_tmp167=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp159,x);
# 805
void*_tmp168=_tmp15A(_tmp167,y,0,_tmp15C);
if(_tmp168 != _tmp167){
if((unsigned int)_tmp168){
const char*_tmp2E8;void*_tmp2E7[2];struct Cyc_String_pa_PrintArg_struct _tmp2E6;struct Cyc_String_pa_PrintArg_struct _tmp2E5;Cyc_Interface_check_err(_tmp15C,(struct _dyneither_ptr)((_tmp2E5.tag=0,((_tmp2E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp2E6.tag=0,((_tmp2E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp15B),((_tmp2E7[0]=& _tmp2E6,((_tmp2E7[1]=& _tmp2E5,Cyc_aprintf(((_tmp2E8="abstract %s %s is being imported as non-abstract",_tag_dyneither(_tmp2E8,sizeof(char),49))),_tag_dyneither(_tmp2E7,sizeof(void*),2)))))))))))))));}
# 810
*_tmp156=0;}}
# 801
;_pop_handler();}else{void*_tmp165=(void*)_exn_thrown;void*_tmp16D=_tmp165;void*_tmp16E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16D)->tag == Cyc_Dict_Absent){_LLE2: _LLE3:
# 814
 if(*_tmp156)
*_tmp157=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp157,x,y);
goto _LLE1;}else{_LLE4: _tmp16E=_tmp16D;_LLE5:(int)_rethrow(_tmp16E);}_LLE1:;}};};}_LLD7:;}
# 820
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 828
struct _tuple26 _tmp2E9;struct _tuple26 _tmp16F=(_tmp2E9.f1=1,((_tmp2E9.f2=dic1,((_tmp2E9.f3=dic1,((_tmp2E9.f4=excl,((_tmp2E9.f5=merge_f,((_tmp2E9.f6=t,((_tmp2E9.f7=msg,_tmp2E9)))))))))))));
# 830
((void(*)(void(*f)(struct _tuple26*,struct _dyneither_ptr*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,& _tmp16F,dic2);
# 832
if(_tmp16F.f1){
struct Cyc_Dict_Dict*_tmp2EA;return(_tmp2EA=_cycalloc(sizeof(*_tmp2EA)),((_tmp2EA[0]=_tmp16F.f2,_tmp2EA)));}
return 0;}
# 842
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*msg){
const char*_tmp2EB;struct Cyc_Dict_Dict*_tmp172=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,((_tmp2EB="type",_tag_dyneither(_tmp2EB,sizeof(char),5))),msg);
const char*_tmp2EC;struct Cyc_Dict_Dict*_tmp173=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,((_tmp2EC="datatype",_tag_dyneither(_tmp2EC,sizeof(char),9))),msg);
const char*_tmp2ED;struct Cyc_Dict_Dict*_tmp174=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,((_tmp2ED="enum",_tag_dyneither(_tmp2ED,sizeof(char),5))),msg);
const char*_tmp2EE;struct Cyc_Dict_Dict*_tmp175=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,((_tmp2EE="typedef",_tag_dyneither(_tmp2EE,sizeof(char),8))),msg);
const char*_tmp2EF;struct Cyc_Dict_Dict*_tmp176=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,((_tmp2EF="variable",_tag_dyneither(_tmp2EF,sizeof(char),9))),msg);
const char*_tmp2F0;struct Cyc_Dict_Dict*_tmp177=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,((_tmp2F0="xdatatypefield",_tag_dyneither(_tmp2F0,sizeof(char),15))),msg);
if(((((!((unsigned int)_tmp172) || !((unsigned int)_tmp173)) || !((unsigned int)_tmp174)) || !((unsigned int)_tmp175)) || !((unsigned int)_tmp176)) || !((unsigned int)_tmp177))
return 0;{
struct Cyc_Interface_Ienv*_tmp2F1;return(_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1->aggrdecls=*_tmp172,((_tmp2F1->datatypedecls=*_tmp173,((_tmp2F1->enumdecls=*_tmp174,((_tmp2F1->typedefdecls=*_tmp175,((_tmp2F1->vardecls=*_tmp176,((_tmp2F1->xdatatypefielddecls=*_tmp177,_tmp2F1)))))))))))));};}struct _tuple27{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;struct _dyneither_ptr*f5;};
# 854
void Cyc_Interface_compat_merge_ns_f(struct _tuple27*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){
# 860
struct _tuple27*_tmp17F=env;int*_tmp180;struct Cyc_Dict_Dict*_tmp181;struct Cyc_Dict_Dict _tmp182;struct Cyc_Dict_Dict _tmp183;struct _dyneither_ptr*_tmp184;_LLE7: _tmp180=(int*)& _tmp17F->f1;_tmp181=(struct Cyc_Dict_Dict*)& _tmp17F->f2;_tmp182=_tmp17F->f3;_tmp183=_tmp17F->f4;_tmp184=_tmp17F->f5;_LLE8: {
# 862
struct Cyc_Interface_Ienv*ie1;struct Cyc_Interface_Ienv*iexcl;
# 865
{struct _handler_cons _tmp185;_push_handler(& _tmp185);{int _tmp187=0;if(setjmp(_tmp185.handler))_tmp187=1;if(!_tmp187){iexcl=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp183,ns);;_pop_handler();}else{void*_tmp186=(void*)_exn_thrown;void*_tmp188=_tmp186;void*_tmp189;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp188)->tag == Cyc_Dict_Absent){_LLEC: _LLED:
 iexcl=Cyc_Interface_lazy_new_ienv();goto _LLEB;}else{_LLEE: _tmp189=_tmp188;_LLEF:(int)_rethrow(_tmp189);}_LLEB:;}};}
# 868
{struct _handler_cons _tmp18A;_push_handler(& _tmp18A);{int _tmp18C=0;if(setjmp(_tmp18A.handler))_tmp18C=1;if(!_tmp18C){
ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp182,ns);{
struct Cyc_Interface_Ienv*_tmp18D=Cyc_Interface_compat_merge_ienv(Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp184);
if(_tmp18D == 0){
*_tmp180=0;
_npop_handler(0);return;}
# 875
ie1=_tmp18D;};
# 869
;_pop_handler();}else{void*_tmp18B=(void*)_exn_thrown;void*_tmp18E=_tmp18B;void*_tmp18F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp18E)->tag == Cyc_Dict_Absent){_LLF1: _LLF2:
# 876
 ie1=Cyc_Interface_lazy_new_ienv();goto _LLF0;}else{_LLF3: _tmp18F=_tmp18E;_LLF4:(int)_rethrow(_tmp18F);}_LLF0:;}};}{
# 878
struct Cyc_Interface_Ienv*_tmp190=Cyc_Interface_compat_merge_ienv(ie1,ie2,iexcl,_tmp184);
# 880
if(_tmp190 == 0)
*_tmp180=0;else{
# 883
if(*_tmp180)
*_tmp181=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp181,ns,_tmp190);}};}_LLE6:;}
# 887
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct _dyneither_ptr*msg){
# 892
struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
struct _tuple27 _tmp2F2;struct _tuple27 _tmp191=(_tmp2F2.f1=1,((_tmp2F2.f2=d,((_tmp2F2.f3=dic1,((_tmp2F2.f4=excl,((_tmp2F2.f5=msg,_tmp2F2)))))))));
# 895
((void(*)(void(*f)(struct _tuple27*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,& _tmp191,dic2);
# 897
if(_tmp191.f1){
struct Cyc_Dict_Dict*_tmp2F3;return(_tmp2F3=_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3[0]=_tmp191.f2,_tmp2F3)));}
return 0;}struct _tuple28{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 903
void Cyc_Interface_disj_merge_dict_f(struct _tuple28*env,struct _dyneither_ptr*x,void*y){
# 906
struct _tuple28*_tmp194=env;int*_tmp195;struct Cyc_Dict_Dict*_tmp196;struct Cyc_Dict_Dict _tmp197;struct _dyneither_ptr _tmp198;struct _dyneither_ptr*_tmp199;_LLF6: _tmp195=(int*)& _tmp194->f1;_tmp196=(struct Cyc_Dict_Dict*)& _tmp194->f2;_tmp197=_tmp194->f3;_tmp198=_tmp194->f4;_tmp199=_tmp194->f5;_LLF7:
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp197,x)){
{const char*_tmp2F8;void*_tmp2F7[2];struct Cyc_String_pa_PrintArg_struct _tmp2F6;struct Cyc_String_pa_PrintArg_struct _tmp2F5;Cyc_Interface_check_err(_tmp199,(struct _dyneither_ptr)((_tmp2F5.tag=0,((_tmp2F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp2F6.tag=0,((_tmp2F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp198),((_tmp2F7[0]=& _tmp2F6,((_tmp2F7[1]=& _tmp2F5,Cyc_aprintf(((_tmp2F8="%s %s is exported more than once",_tag_dyneither(_tmp2F8,sizeof(char),33))),_tag_dyneither(_tmp2F7,sizeof(void*),2)))))))))))))));}
*_tmp195=0;}else{
# 911
if(*_tmp195)*_tmp196=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp196,x,y);}_LLF5:;}
# 915
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 919
struct _tuple28 _tmp2F9;struct _tuple28 _tmp19E=(_tmp2F9.f1=1,((_tmp2F9.f2=dic1,((_tmp2F9.f3=dic1,((_tmp2F9.f4=t,((_tmp2F9.f5=msg,_tmp2F9)))))))));
((void(*)(void(*f)(struct _tuple28*,struct _dyneither_ptr*,void*),struct _tuple28*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp19E,dic2);
if(_tmp19E.f1){
struct Cyc_Dict_Dict*_tmp2FA;return(_tmp2FA=_cycalloc(sizeof(*_tmp2FA)),((_tmp2FA[0]=_tmp19E.f2,_tmp2FA)));}
return 0;}
# 926
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
const char*_tmp2FB;struct Cyc_Dict_Dict*_tmp1A1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_aggrdecl,(
(_tmp2FB="type",_tag_dyneither(_tmp2FB,sizeof(char),5))),msg);
const char*_tmp2FC;struct Cyc_Dict_Dict*_tmp1A2=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,((_tmp2FC="datatype",_tag_dyneither(_tmp2FC,sizeof(char),9))),msg);
const char*_tmp2FD;struct Cyc_Dict_Dict*_tmp1A3=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,ie2->enumdecls,((_tmp2FD="enum",_tag_dyneither(_tmp2FD,sizeof(char),5))),msg);
# 933
const char*_tmp2FE;struct Cyc_Dict_Dict*_tmp1A4=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,(
(_tmp2FE="typedef",_tag_dyneither(_tmp2FE,sizeof(char),8))),msg);
const char*_tmp2FF;struct Cyc_Dict_Dict*_tmp1A5=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,ie2->vardecls,((_tmp2FF="variable",_tag_dyneither(_tmp2FF,sizeof(char),9))),msg);
const char*_tmp300;struct Cyc_Dict_Dict*_tmp1A6=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,(
(_tmp300="xdatatypefield",_tag_dyneither(_tmp300,sizeof(char),15))),msg);
# 940
if(((((!((unsigned int)_tmp1A1) || !((unsigned int)_tmp1A2)) || !((unsigned int)_tmp1A3)) || !((unsigned int)_tmp1A4)) || !((unsigned int)_tmp1A5)) || !((unsigned int)_tmp1A6))
return 0;{
struct Cyc_Interface_Ienv*_tmp301;return(_tmp301=_cycalloc(sizeof(*_tmp301)),((_tmp301->aggrdecls=*_tmp1A1,((_tmp301->datatypedecls=*_tmp1A2,((_tmp301->enumdecls=*_tmp1A3,((_tmp301->typedefdecls=*_tmp1A4,((_tmp301->vardecls=*_tmp1A5,((_tmp301->xdatatypefielddecls=*_tmp1A6,_tmp301)))))))))))));};}struct _tuple29{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr*f4;};
# 945
void Cyc_Interface_disj_merge_ns_f(struct _tuple29*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){
# 948
struct _tuple29*_tmp1AE=env;int*_tmp1AF;struct Cyc_Dict_Dict*_tmp1B0;struct Cyc_Dict_Dict _tmp1B1;struct _dyneither_ptr*_tmp1B2;_LLFB: _tmp1AF=(int*)& _tmp1AE->f1;_tmp1B0=(struct Cyc_Dict_Dict*)& _tmp1AE->f2;_tmp1B1=_tmp1AE->f3;_tmp1B2=_tmp1AE->f4;_LLFC: {
# 950
struct Cyc_Interface_Ienv*ie1;
# 952
{struct _handler_cons _tmp1B3;_push_handler(& _tmp1B3);{int _tmp1B5=0;if(setjmp(_tmp1B3.handler))_tmp1B5=1;if(!_tmp1B5){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp1B1,ns);;_pop_handler();}else{void*_tmp1B4=(void*)_exn_thrown;void*_tmp1B6=_tmp1B4;void*_tmp1B7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B6)->tag == Cyc_Dict_Absent){_LL100: _LL101:
 ie1=Cyc_Interface_lazy_new_ienv();goto _LLFF;}else{_LL102: _tmp1B7=_tmp1B6;_LL103:(int)_rethrow(_tmp1B7);}_LLFF:;}};}{
# 955
struct Cyc_Interface_Ienv*_tmp1B8=Cyc_Interface_disj_merge_ienv(ie1,ie2,_tmp1B2);
# 957
if(_tmp1B8 == 0)
*_tmp1AF=0;else{
# 960
if(*_tmp1AF)
*_tmp1B0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp1B0,ns,_tmp1B8);}};}_LLFA:;}
# 964
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){
# 968
struct _tuple29 _tmp302;struct _tuple29 _tmp1B9=(_tmp302.f1=1,((_tmp302.f2=dic1,((_tmp302.f3=dic1,((_tmp302.f4=msg,_tmp302)))))));
((void(*)(void(*f)(struct _tuple29*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple29*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_ns_f,& _tmp1B9,dic2);
if(_tmp1B9.f1){
struct Cyc_Dict_Dict*_tmp303;return(_tmp303=_cycalloc(sizeof(*_tmp303)),((_tmp303[0]=_tmp1B9.f2,_tmp303)));}
return 0;}
# 976
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp1BC;_push_handler(& _tmp1BC);{int _tmp1BE=0;if(setjmp(_tmp1BC.handler))_tmp1BE=1;if(!_tmp1BE){
{struct _dyneither_ptr*msg=0;
# 980
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp30D;struct Cyc_String_pa_PrintArg_struct _tmp30C;void*_tmp30B[2];const char*_tmp30A;struct _dyneither_ptr*_tmp309;msg=((_tmp309=_cycalloc(sizeof(*_tmp309)),((_tmp309[0]=(struct _dyneither_ptr)((_tmp30D.tag=0,((_tmp30D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp30C.tag=0,((_tmp30C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp30B[0]=& _tmp30C,((_tmp30B[1]=& _tmp30D,Cyc_aprintf(((_tmp30A="merging exports of %s and %s,",_tag_dyneither(_tmp30A,sizeof(char),30))),_tag_dyneither(_tmp30B,sizeof(void*),2)))))))))))))),_tmp309))));}{
# 984
struct Cyc_Dict_Dict*_tmp1C4=Cyc_Interface_disj_merge_ns(i1->exports,i2->exports,msg);
if(_tmp1C4 == 0){struct Cyc_Interface_I*_tmp1C5=0;_npop_handler(0);return _tmp1C5;}
# 987
if(info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp317;struct Cyc_String_pa_PrintArg_struct _tmp316;void*_tmp315[2];const char*_tmp314;struct _dyneither_ptr*_tmp313;msg=((_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313[0]=(struct _dyneither_ptr)((_tmp317.tag=0,((_tmp317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp316.tag=0,((_tmp316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp315[0]=& _tmp316,((_tmp315[1]=& _tmp317,Cyc_aprintf(((_tmp314="merging imports of %s and %s,",_tag_dyneither(_tmp314,sizeof(char),30))),_tag_dyneither(_tmp315,sizeof(void*),2)))))))))))))),_tmp313))));}{
# 991
struct Cyc_Dict_Dict*_tmp1CB=Cyc_Interface_compat_merge_ns(i1->imports,i2->imports,*_tmp1C4,msg);
if(_tmp1CB == 0){struct Cyc_Interface_I*_tmp1CC=0;_npop_handler(0);return _tmp1CC;}{
# 994
struct Cyc_Interface_I*_tmp318;struct Cyc_Interface_I*_tmp1CE=(_tmp318=_cycalloc(sizeof(*_tmp318)),((_tmp318->imports=*_tmp1CB,((_tmp318->exports=*_tmp1C4,_tmp318)))));_npop_handler(0);return _tmp1CE;};};};}
# 978
;_pop_handler();}else{void*_tmp1BD=(void*)_exn_thrown;void*_tmp1CF=_tmp1BD;void*_tmp1D0;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp1CF)->tag == Cyc_Tcdecl_Incompatible){_LL105: _LL106:
# 995
 return 0;}else{_LL107: _tmp1D0=_tmp1CF;_LL108:(int)_rethrow(_tmp1D0);}_LL104:;}};}
# 998
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li == 0)return Cyc_Interface_empty();{
# 1001
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo != 0)linfo=linfo->tl;
# 1006
for(0;li != 0;li=li->tl){
struct _tuple10*_tmp319;struct Cyc_Interface_I*_tmp1D1=Cyc_Interface_merge(curr_i,(struct Cyc_Interface_I*)li->hd,
curr_info != 0  && linfo != 0?(_tmp319=_cycalloc(sizeof(*_tmp319)),((_tmp319->f1=*curr_info,((_tmp319->f2=*((struct _dyneither_ptr*)linfo->hd),_tmp319))))): 0);
if(_tmp1D1 == 0)return 0;
curr_i=_tmp1D1;
if(curr_info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp323;struct Cyc_String_pa_PrintArg_struct _tmp322;void*_tmp321[2];const char*_tmp320;struct _dyneither_ptr*_tmp31F;curr_info=linfo != 0?(_tmp31F=_cycalloc(sizeof(*_tmp31F)),((_tmp31F[0]=(struct _dyneither_ptr)((_tmp323.tag=0,((_tmp323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((_tmp322.tag=0,((_tmp322.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info),((_tmp321[0]=& _tmp322,((_tmp321[1]=& _tmp323,Cyc_aprintf(((_tmp320="%s+%s",_tag_dyneither(_tmp320,sizeof(char),6))),_tag_dyneither(_tmp321,sizeof(void*),2)))))))))))))),_tmp31F))): 0;}
# 1014
if(linfo != 0)linfo=linfo->tl;}
# 1016
return curr_i;};}
# 1019
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la == 0)return Cyc_Interface_empty();{
# 1022
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo != 0)linfo=linfo->tl;
# 1027
for(0;la != 0;la=la->tl){
struct _tuple10*_tmp324;struct Cyc_Interface_I*_tmp1D8=Cyc_Interface_merge(curr_i,get(la->hd),
curr_info != 0  && linfo != 0?(_tmp324=_cycalloc(sizeof(*_tmp324)),((_tmp324->f1=*curr_info,((_tmp324->f2=*((struct _dyneither_ptr*)linfo->hd),_tmp324))))): 0);
if(_tmp1D8 == 0)return 0;
curr_i=_tmp1D8;
if(curr_info != 0){
struct Cyc_String_pa_PrintArg_struct _tmp32E;struct Cyc_String_pa_PrintArg_struct _tmp32D;void*_tmp32C[2];const char*_tmp32B;struct _dyneither_ptr*_tmp32A;curr_info=linfo != 0?(_tmp32A=_cycalloc(sizeof(*_tmp32A)),((_tmp32A[0]=(struct _dyneither_ptr)((_tmp32E.tag=0,((_tmp32E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((_tmp32D.tag=0,((_tmp32D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info),((_tmp32C[0]=& _tmp32D,((_tmp32C[1]=& _tmp32E,Cyc_aprintf(((_tmp32B="%s+%s",_tag_dyneither(_tmp32B,sizeof(char),6))),_tag_dyneither(_tmp32C,sizeof(void*),2)))))))))))))),_tmp32A))): 0;}
# 1035
if(linfo != 0)linfo=linfo->tl;}
# 1037
return curr_i;};}
# 1042
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds,struct Cyc_List_List*ns){
if(ns == 0)return tds;{
struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp334;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp333;struct Cyc_List_List*_tmp332;return(_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332->hd=Cyc_Absyn_new_decl((void*)((_tmp334=_cycalloc(sizeof(*_tmp334)),((_tmp334[0]=((_tmp333.tag=9,((_tmp333.f1=(struct _dyneither_ptr*)ns->hd,((_tmp333.f2=Cyc_Interface_add_namespace(tds,ns->tl),_tmp333)))))),_tmp334)))),0),((_tmp332->tl=0,_tmp332)))));};}
# 1048
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp33A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp339;struct Cyc_List_List*_tmp338;return(_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338->hd=Cyc_Absyn_new_decl((void*)((_tmp33A=_cycalloc(sizeof(*_tmp33A)),((_tmp33A[0]=((_tmp339.tag=5,((_tmp339.f1=d,_tmp339)))),_tmp33A)))),0),((_tmp338->tl=tds,_tmp338)))));}
# 1052
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 1054
{struct Cyc_Absyn_Aggrdecl*_tmp33B;d=((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((_tmp33B[0]=*d,_tmp33B))));}
d->impl=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp341;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp340;struct Cyc_List_List*_tmp33F;return(_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F->hd=Cyc_Absyn_new_decl((void*)((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341[0]=((_tmp340.tag=5,((_tmp340.f1=d,_tmp340)))),_tmp341)))),0),((_tmp33F->tl=tds,_tmp33F)))));};}
# 1060
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp347;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp346;struct Cyc_List_List*_tmp345;return(_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345->hd=Cyc_Absyn_new_decl((void*)((_tmp347=_cycalloc(sizeof(*_tmp347)),((_tmp347[0]=((_tmp346.tag=6,((_tmp346.f1=d,_tmp346)))),_tmp347)))),0),((_tmp345->tl=tds,_tmp345)))));}static char _tmp1EC[2]="_";
# 1064
static struct _dyneither_ptr Cyc_Interface_us={_tmp1EC,_tmp1EC,_tmp1EC + 2};
static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 1067
static struct _tuple20*Cyc_Interface_rewrite_datatypefield_type(struct _tuple20*x){
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp351;struct Cyc_Absyn_Tvar*_tmp350;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp34F;struct _tuple20*_tmp34E;return(_tmp34E=_cycalloc(sizeof(*_tmp34E)),((_tmp34E->f1=Cyc_Absyn_empty_tqual(0),((_tmp34E->f2=(void*)(
(_tmp351=_cycalloc(sizeof(*_tmp351)),((_tmp351[0]=((_tmp34F.tag=2,((_tmp34F.f1=((_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350->name=Cyc_Interface_us_p,((_tmp350->identity=- 1,((_tmp350->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp350)))))))),_tmp34F)))),_tmp351)))),_tmp34E)))));}
# 1072
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
{struct Cyc_Absyn_Datatypefield*_tmp352;f=((_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352[0]=*f,_tmp352))));}
f->typs=((struct Cyc_List_List*(*)(struct _tuple20*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);
return f;}
# 1078
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 1080
{struct Cyc_Absyn_Datatypedecl*_tmp353;d=((_tmp353=_cycalloc(sizeof(*_tmp353)),((_tmp353[0]=*d,_tmp353))));}
# 1082
if(d->fields != 0){struct Cyc_Core_Opt*_tmp354;d->fields=((_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v),_tmp354))));}
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp35A;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp359;struct Cyc_List_List*_tmp358;return(_tmp358=_cycalloc(sizeof(*_tmp358)),((_tmp358->hd=Cyc_Absyn_new_decl((void*)((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=((_tmp359.tag=6,((_tmp359.f1=d,_tmp359)))),_tmp35A)))),0),((_tmp358->tl=tds,_tmp358)))));};}
# 1087
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp360;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp35F;struct Cyc_List_List*_tmp35E;return(_tmp35E=_cycalloc(sizeof(*_tmp35E)),((_tmp35E->hd=Cyc_Absyn_new_decl((void*)((_tmp360=_cycalloc(sizeof(*_tmp360)),((_tmp360[0]=((_tmp35F.tag=7,((_tmp35F.f1=d,_tmp35F)))),_tmp360)))),0),((_tmp35E->tl=tds,_tmp35E)))));}
# 1091
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 1093
{struct Cyc_Absyn_Enumdecl*_tmp361;d=((_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361[0]=*d,_tmp361))));}
d->fields=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp367;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp366;struct Cyc_List_List*_tmp365;return(_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365->hd=Cyc_Absyn_new_decl((void*)((_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367[0]=((_tmp366.tag=7,((_tmp366.f1=d,_tmp366)))),_tmp367)))),0),((_tmp365->tl=tds,_tmp365)))));};}
# 1099
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 1101
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp36D;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp36C;struct Cyc_List_List*_tmp36B;return(_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B->hd=Cyc_Absyn_new_decl((void*)((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D[0]=((_tmp36C.tag=8,((_tmp36C.f1=d,_tmp36C)))),_tmp36D)))),0),((_tmp36B->tl=tds,_tmp36B)))));}
# 1104
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp373;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp372;struct Cyc_List_List*_tmp371;return(_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371->hd=Cyc_Absyn_new_decl((void*)((_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373[0]=((_tmp372.tag=0,((_tmp372.f1=d,_tmp372)))),_tmp373)))),0),((_tmp371->tl=tds,_tmp371)))));}
# 1108
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1110
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp204=d;struct Cyc_Absyn_Datatypedecl*_tmp205;struct Cyc_Absyn_Datatypefield*_tmp206;_LL10A: _tmp205=_tmp204->base;_tmp206=_tmp204->field;_LL10B:
{struct Cyc_Absyn_Datatypedecl*_tmp374;_tmp205=((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374[0]=*_tmp205,_tmp374))));}
{struct Cyc_List_List*_tmp377;struct Cyc_Core_Opt*_tmp376;_tmp205->fields=((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376->v=((_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377->hd=_tmp206,((_tmp377->tl=0,_tmp377)))))),_tmp376))));}
_tmp205->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp37D;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp37C;struct Cyc_List_List*_tmp37B;return(_tmp37B=_cycalloc(sizeof(*_tmp37B)),((_tmp37B->hd=Cyc_Absyn_new_decl((void*)((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=((_tmp37C.tag=6,((_tmp37C.f1=_tmp205,_tmp37C)))),_tmp37D)))),0),((_tmp37B->tl=tds,_tmp37B)))));};_LL109:;}
# 1117
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1119
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp20D=d;struct Cyc_Absyn_Datatypedecl*_tmp20E;struct Cyc_Absyn_Datatypefield*_tmp20F;_LL10F: _tmp20E=_tmp20D->base;_tmp20F=_tmp20D->field;_LL110:
{struct Cyc_Absyn_Datatypedecl*_tmp37E;_tmp20E=((_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E[0]=*_tmp20E,_tmp37E))));}
_tmp20F=Cyc_Interface_rewrite_datatypefield(_tmp20F);
_tmp20F->sc=Cyc_Absyn_Extern;
{struct Cyc_List_List*_tmp381;struct Cyc_Core_Opt*_tmp380;_tmp20E->fields=((_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380->v=((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381->hd=_tmp20F,((_tmp381->tl=0,_tmp381)))))),_tmp380))));}
_tmp20E->sc=Cyc_Absyn_Extern;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp387;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp386;struct Cyc_List_List*_tmp385;return(_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385->hd=Cyc_Absyn_new_decl((void*)((_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387[0]=((_tmp386.tag=6,((_tmp386.f1=_tmp20E,_tmp386)))),_tmp387)))),0),((_tmp385->tl=tds,_tmp385)))));};_LL10E:;}
# 1128
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp216=0;
_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp216);
_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp216);
_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp216);
# 1134
if(_tmp216 != 0){
_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp216);
_tmp216=Cyc_Interface_add_namespace(_tmp216,ns);
Cyc_Absynpp_decllist2file(_tmp216,f);}}
# 1141
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
# 1143
struct Cyc_List_List*_tmp217=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);
if(_tmp217 != 0){
_tmp217=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp217);
_tmp217=Cyc_Interface_add_namespace(_tmp217,ns);
Cyc_Absynpp_decllist2file(_tmp217,f);}}
# 1151
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,0);
if(_tmp218 != 0){
_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp218);
_tmp218=Cyc_Interface_add_namespace(_tmp218,ns);
Cyc_Absynpp_decllist2file(_tmp218,f);}}
# 1160
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp219=0;
_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp219);
_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp219);
_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp219);
_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,_tmp219);
_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp219);
# 1168
if(_tmp219 != 0){
_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp219);
_tmp219=Cyc_Interface_add_namespace(_tmp219,ns);
Cyc_Absynpp_decllist2file(_tmp219,f);}}
# 1178
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
{const char*_tmp38A;void*_tmp389;(_tmp389=0,Cyc_fprintf(f,((_tmp38A="/****** needed (headers) ******/\n",_tag_dyneither(_tmp38A,sizeof(char),34))),_tag_dyneither(_tmp389,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,f,i->imports);
# 1183
{const char*_tmp38D;void*_tmp38C;(_tmp38C=0,Cyc_fprintf(f,((_tmp38D="\n/****** provided (headers) ******/\n",_tag_dyneither(_tmp38D,sizeof(char),37))),_tag_dyneither(_tmp38C,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,f,i->exports);
# 1188
{const char*_tmp390;void*_tmp38F;(_tmp38F=0,Cyc_fprintf(f,((_tmp390="\n/****** needed (headers of xdatatypefielddecls) ******/\n",_tag_dyneither(_tmp390,sizeof(char),58))),_tag_dyneither(_tmp38F,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,f,i->imports);
# 1191
{const char*_tmp393;void*_tmp392;(_tmp392=0,Cyc_fprintf(f,((_tmp393="\n/****** provided (headers of xdatatypefielddecls) ******/\n",_tag_dyneither(_tmp393,sizeof(char),60))),_tag_dyneither(_tmp392,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,f,i->exports);
# 1195
{const char*_tmp396;void*_tmp395;(_tmp395=0,Cyc_fprintf(f,((_tmp396="\n/****** provided (typedefs) ******/\n",_tag_dyneither(_tmp396,sizeof(char),38))),_tag_dyneither(_tmp395,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,f,i->exports);
# 1198
{const char*_tmp399;void*_tmp398;(_tmp398=0,Cyc_fprintf(f,((_tmp399="\n/****** needed (declarations) ******/\n",_tag_dyneither(_tmp399,sizeof(char),40))),_tag_dyneither(_tmp398,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,f,i->imports);
# 1201
{const char*_tmp39C;void*_tmp39B;(_tmp39B=0,Cyc_fprintf(f,((_tmp39C="\n/****** provided (declarations) ******/\n",_tag_dyneither(_tmp39C,sizeof(char),42))),_tag_dyneither(_tmp39B,sizeof(void*),0)));}
((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,f,i->exports);}
# 1208
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp228=Cyc_Parse_parse_file(f);
struct _RegionHandle _tmp229=_new_region("r");struct _RegionHandle*r=& _tmp229;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp22A=Cyc_Tcenv_tc_init(r);
Cyc_Tc_tc(r,_tmp22A,0,_tmp228);{
struct Cyc_Interface_I*_tmp22B=Cyc_Interface_gen_extract(_tmp22A->ae,0);_npop_handler(0);return _tmp22B;};}
# 1212
;_pop_region(r);};}
# 1222
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1228
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}
