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
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 429 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 919 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 943
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 957
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 959
extern void*Cyc_Absyn_sint_typ;
# 965
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 998
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1112
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1117
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1146
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1177
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
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 85
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 101 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 119
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae);struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
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
# 35 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 37
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 48
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 53
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
typedef struct _tuple11*Cyc_declarator_list_t;
# 56
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 60
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 68
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 72
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};
# 84
typedef void*Cyc_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_pointer_quals_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
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
# 25 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);
# 46 "interface.cyc"
void Cyc_Lex_lex_init();
# 53
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Interface_xdatatypefielddecl_t;struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};
# 65
typedef struct Cyc_Interface_Ienv*Cyc_Interface_ienv_t;struct Cyc_Interface_I{struct Cyc_Interface_Ienv*imports;struct Cyc_Interface_Ienv*exports;};
# 73
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->aggrdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->datatypedecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->enumdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->typedefdecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->vardecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0;});}
# 83
struct Cyc_Interface_I*Cyc_Interface_empty(){
return({struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->imports=Cyc_Interface_new_ienv();_tmp1->exports=Cyc_Interface_new_ienv();_tmp1;});}
# 88
struct Cyc_Interface_I*Cyc_Interface_final(){
struct Cyc_Interface_I*_tmp2=Cyc_Interface_empty();
# 91
struct Cyc_Absyn_Datatypedecl*_tmp3=({struct Cyc_Absyn_Datatypedecl*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=*Cyc_Absyn_exn_tud;_tmp12;});
_tmp3->sc=Cyc_Absyn_Public;
(_tmp2->exports)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)((_tmp2->exports)->datatypedecls,Cyc_Absyn_exn_name,_tmp3);
# 95
{struct Cyc_List_List*_tmp4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp4 != 0;_tmp4=_tmp4->tl){
struct Cyc_Absyn_Datatypefield*_tmp5=({struct Cyc_Absyn_Datatypefield*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=*((struct Cyc_Absyn_Datatypefield*)_tmp4->hd);_tmp8;});
_tmp5->sc=Cyc_Absyn_Public;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp6=({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->base=_tmp3;_tmp7->field=_tmp5;_tmp7;});
(_tmp2->exports)->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)((_tmp2->exports)->xdatatypefielddecls,((struct Cyc_Absyn_Datatypefield*)_tmp4->hd)->name,_tmp6);};}}{
# 103
struct _tuple0*qmain=({struct _tuple0*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->f1=({union Cyc_Absyn_Nmspace _tmp11;(_tmp11.Abs_n).val=0;(_tmp11.Abs_n).tag=2;_tmp11;});_tmpE->f2=({struct _dyneither_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=(struct _dyneither_ptr)({const char*_tmp10="main";_tag_dyneither(_tmp10,sizeof(char),5);});_tmpF;});_tmpE;});
struct Cyc_Absyn_Vardecl*_tmp9=Cyc_Absyn_new_vardecl(qmain,
Cyc_Absyn_function_typ(0,0,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_sint_typ,({struct Cyc_List_List*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->hd=({struct _tuple8*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->f1=0;_tmpD->f2=
Cyc_Absyn_empty_tqual(0);_tmpD->f3=Cyc_Absyn_uint_typ;_tmpD;});_tmpA->tl=({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->hd=({struct _tuple8*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->f1=0;_tmpC->f2=
Cyc_Absyn_empty_tqual(0);_tmpC->f3=
Cyc_Absyn_dyneither_typ(Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
# 110
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),(void*)& Cyc_Absyn_HeapRgn_val,
# 112
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmpC;});_tmpB->tl=0;_tmpB;});_tmpA;}),0,0,0,0,0,0),0);
# 116
_tmp9->sc=Cyc_Absyn_Extern;
(_tmp2->exports)->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)((_tmp2->exports)->vardecls,qmain,_tmp9);
# 119
_tmp2->imports=_tmp2->exports;
return _tmp2;};}
# 139 "interface.cyc"
static void Cyc_Interface_err(struct _dyneither_ptr msg){
({void*_tmp13=0;Cyc_Tcutil_terr(0,msg,_tag_dyneither(_tmp13,sizeof(void*),0));});}
# 142
static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp15;_tmp15.tag=Cyc_Core_Invalid_argument;_tmp15.f1=s;_tmp15;});_tmp14;}));}
# 145
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n));({struct Cyc_String_pa_PrintArg_struct _tmp19;_tmp19.tag=0;_tmp19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);({void*_tmp16[3]={& _tmp18,& _tmp19,& _tmp1A};Cyc_aprintf(({const char*_tmp17="fields of %s %s %s have never been defined";_tag_dyneither(_tmp17,sizeof(char),43);}),_tag_dyneither(_tmp16,sizeof(void*),3));});});});}));}
# 149
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(n));({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);({void*_tmp1B[2]={& _tmp1D,& _tmp1E};Cyc_aprintf(({const char*_tmp1C="the body of %s function %s has never been defined";_tag_dyneither(_tmp1C,sizeof(char),50);}),_tag_dyneither(_tmp1B,sizeof(void*),2));});});}));}
# 155
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr)
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 159
Cyc_Absynpp_qvar2string(name2));({struct Cyc_String_pa_PrintArg_struct _tmp23;_tmp23.tag=0;_tmp23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 158
Cyc_Absynpp_qvar2string(name1));({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);({void*_tmp1F[4]={& _tmp21,& _tmp22,& _tmp23,& _tmp24};Cyc_aprintf(({const char*_tmp20="declaration of %s %s relies on static %s %s";_tag_dyneither(_tmp20,sizeof(char),44);}),_tag_dyneither(_tmp1F,sizeof(void*),4));});});});});}));}
# 163
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if((char*)obj1.curr != (char*)(_tag_dyneither(0,0,0)).curr)
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 167
Cyc_Absynpp_qvar2string(name2));({struct Cyc_String_pa_PrintArg_struct _tmp29;_tmp29.tag=0;_tmp29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 166
Cyc_Absynpp_qvar2string(name1));({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);({void*_tmp25[4]={& _tmp27,& _tmp28,& _tmp29,& _tmp2A};Cyc_aprintf(({const char*_tmp26="declaration of %s %s relies on fields of abstract %s %s";_tag_dyneither(_tmp26,sizeof(char),56);}),_tag_dyneither(_tmp25,sizeof(void*),4));});});});});}));}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 178
typedef struct Cyc_Interface_Seen*Cyc_Interface_seen_t;
# 180
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){
return({struct Cyc_Interface_Seen*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->aggrs=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp2B->datatypes=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp2B;});}
# 186
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);
# 189
static int Cyc_Interface_check_public_type_list(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 191
int _tmp2C=1;
for(0;l != 0;l=l->tl){
if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f(l->hd)))
_tmp2C=0;}
return _tmp2C;}
# 198
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _tmp2D;_push_handler(& _tmp2D);{int _tmp2F=0;if(setjmp(_tmp2D.handler))_tmp2F=1;if(!_tmp2F){{int _tmp30=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp30;};_pop_handler();}else{void*_tmp2E=(void*)_exn_thrown;void*_tmp31=_tmp2E;void*_tmp32;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp31)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp32=_tmp31;_LL4:(int)_rethrow(_tmp32);}_LL0:;}};}{
int _tmp33=1;
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp33);
if(d->impl != 0){
struct Cyc_List_List*_tmp34=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;for(0;_tmp34 != 0;_tmp34=_tmp34->tl){
if(!Cyc_Interface_check_public_type(ae,seen,({const char*_tmp35="type";_tag_dyneither(_tmp35,sizeof(char),5);}),d->name,((struct Cyc_Absyn_Aggrfield*)_tmp34->hd)->type))
_tmp33=0;}}
# 208
seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp33);
return _tmp33;};}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 212
static void*Cyc_Interface_get_type1(struct _tuple20*x){
return(*x).f2;}
# 215
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f3;}
# 219
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _tmp36;_push_handler(& _tmp36);{int _tmp38=0;if(setjmp(_tmp36.handler))_tmp38=1;if(!_tmp38){{int _tmp39=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);return _tmp39;};_pop_handler();}else{void*_tmp37=(void*)_exn_thrown;void*_tmp3A=_tmp37;void*_tmp3B;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3A)->tag == Cyc_Dict_Absent){_LL6: _LL7:
 goto _LL5;}else{_LL8: _tmp3B=_tmp3A;_LL9:(int)_rethrow(_tmp3B);}_LL5:;}};}{
int _tmp3C=1;
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp3C);
if(d->fields != 0){
struct Cyc_List_List*_tmp3D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp3D != 0;_tmp3D=_tmp3D->tl){
if(!((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,({const char*_tmp3E="datatype";_tag_dyneither(_tmp3E,sizeof(char),9);}),d->name,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)_tmp3D->hd)->typs))
# 228
_tmp3C=0;}}
# 230
seen->datatypes=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp3C);
return _tmp3C;};}
# 234
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 238
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn != 0)
return Cyc_Interface_check_public_type(ae,seen,_tag_dyneither(0,0,0),d->name,(void*)_check_null(d->defn));else{
return 1;}}
# 245
static int Cyc_Interface_check_public_vardecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
return Cyc_Interface_check_public_type(ae,seen,({const char*_tmp3F="variable";_tag_dyneither(_tmp3F,sizeof(char),9);}),d->name,d->type);}
# 249
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t){
void*_tmp40=Cyc_Tcutil_compress(t);void*_tmp41=_tmp40;struct Cyc_Absyn_Datatypedecl*_tmp7B;struct Cyc_Absyn_Datatypefield*_tmp7A;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_Datatypedecl*_tmp78;struct Cyc_List_List*_tmp77;struct _tuple0*_tmp76;union Cyc_Absyn_AggrInfoU _tmp75;struct Cyc_List_List*_tmp74;struct Cyc_List_List*_tmp73;void*_tmp72;struct Cyc_List_List*_tmp71;struct Cyc_Absyn_VarargInfo*_tmp70;void*_tmp6F;void*_tmp6E;void*_tmp6D;switch(*((int*)_tmp41)){case 5: _LLB: _tmp6D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->f1).elt_typ;_LLC:
 _tmp6E=_tmp6D;goto _LLE;case 8: _LLD: _tmp6E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp41)->f1).elt_type;_LLE:
 _tmp6F=_tmp6E;goto _LL10;case 17: _LLF: _tmp6F=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp41)->f4;if(_tmp6F != 0){_LL10:
# 254
 return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp6F);}else{goto _LL1D;}case 9: _LL11: _tmp72=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp41)->f1).ret_typ;_tmp71=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp41)->f1).args;_tmp70=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp41)->f1).cyc_varargs;_LL12: {
# 258
int b=((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple8*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,_tmp71) && 
Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp72);
if(_tmp70 != 0){
struct Cyc_Absyn_VarargInfo _tmp42=*_tmp70;struct Cyc_Absyn_VarargInfo _tmp43=_tmp42;void*_tmp44;_LL20: _tmp44=_tmp43.type;_LL21:;
b=Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp44);}
# 264
return b;}case 10: _LL13: _tmp73=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp41)->f1;_LL14:
# 267
 return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,_tmp73);case 11: _LL15: _tmp75=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41)->f1).aggr_info;_tmp74=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41)->f1).targs;_LL16: {
# 270
struct Cyc_Absyn_Aggrdecl*_tmp45=Cyc_Absyn_get_known_aggrdecl(_tmp75);
if(_tmp45->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,({const char*_tmp46="type";_tag_dyneither(_tmp46,sizeof(char),5);}),_tmp45->name);
return 0;}
# 275
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp74) && 
Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp45);}case 13: _LL17: _tmp76=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp41)->f1;_LL18: {
# 279
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _tmp47;_push_handler(& _tmp47);{int _tmp49=0;if(setjmp(_tmp47.handler))_tmp49=1;if(!_tmp49){ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->enumdecls,_tmp76);;_pop_handler();}else{void*_tmp48=(void*)_exn_thrown;void*_tmp4A=_tmp48;void*_tmp4E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4A)->tag == Cyc_Dict_Absent){_LL23: _LL24:
# 282
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp76));({void*_tmp4B[1]={& _tmp4D};Cyc_aprintf(({const char*_tmp4C="check_public_type (can't find enum %s)";_tag_dyneither(_tmp4C,sizeof(char),39);}),_tag_dyneither(_tmp4B,sizeof(void*),1));});}));}else{_LL25: _tmp4E=_tmp4A;_LL26:(int)_rethrow(_tmp4E);}_LL22:;}};}
# 285
if(ed->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,_tmp76,({const char*_tmp4F="enum";_tag_dyneither(_tmp4F,sizeof(char),5);}),ed->name);
return 0;}
# 289
return 1;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp41)->f1).datatype_info).KnownDatatype).tag == 2){_LL19: _tmp78=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp41)->f1).datatype_info).KnownDatatype).val;_tmp77=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp41)->f1).targs;_LL1A: {
# 292
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp50;_push_handler(& _tmp50);{int _tmp52=0;if(setjmp(_tmp50.handler))_tmp52=1;if(!_tmp52){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp78->name);;_pop_handler();}else{void*_tmp51=(void*)_exn_thrown;void*_tmp53=_tmp51;void*_tmp57;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp53)->tag == Cyc_Dict_Absent){_LL28: _LL29:
# 295
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp78->name));({void*_tmp54[1]={& _tmp56};Cyc_aprintf(({const char*_tmp55="check_public_type (can't find datatype %s)";_tag_dyneither(_tmp55,sizeof(char),43);}),_tag_dyneither(_tmp54,sizeof(void*),1));});}));}else{_LL2A: _tmp57=_tmp53;_LL2B:(int)_rethrow(_tmp57);}_LL27:;}};}
# 298
if(tud->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,({const char*_tmp58="datatype";_tag_dyneither(_tmp58,sizeof(char),9);}),tud->name);
return 0;}
# 302
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp77) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}else{goto _LL1D;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp41)->f1).field_info).KnownDatatypefield).tag == 2){_LL1B: _tmp7B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp41)->f1).field_info).KnownDatatypefield).val).f1;_tmp7A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp41)->f1).field_info).KnownDatatypefield).val).f2;_tmp79=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp41)->f1).targs;_LL1C: {
# 306
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp59;_push_handler(& _tmp59);{int _tmp5B=0;if(setjmp(_tmp59.handler))_tmp5B=1;if(!_tmp5B){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp7B->name);;_pop_handler();}else{void*_tmp5A=(void*)_exn_thrown;void*_tmp5C=_tmp5A;void*_tmp60;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp5C)->tag == Cyc_Dict_Absent){_LL2D: _LL2E:
# 309
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B->name));({void*_tmp5D[1]={& _tmp5F};Cyc_aprintf(({const char*_tmp5E="check_public_type (can't find datatype %s and search its fields)";_tag_dyneither(_tmp5E,sizeof(char),65);}),_tag_dyneither(_tmp5D,sizeof(void*),1));});}));}else{_LL2F: _tmp60=_tmp5C;_LL30:(int)_rethrow(_tmp60);}_LL2C:;}};}
# 312
if(tud->fields == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0;_tmp63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tud->name));({void*_tmp61[1]={& _tmp63};Cyc_aprintf(({const char*_tmp62="check_public_type (datatype %s has no fields)";_tag_dyneither(_tmp62,sizeof(char),46);}),_tag_dyneither(_tmp61,sizeof(void*),1));});}));{
# 316
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*_tmp64=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp64 != 0;_tmp64=_tmp64->tl){
if(Cyc_strptrcmp((*_tmp7A->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp64->hd)->name).f2)== 0){
tuf1=(struct Cyc_Absyn_Datatypefield*)_tmp64->hd;
break;}}}
# 323
if(tuf1 == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A->name));({void*_tmp65[1]={& _tmp67};Cyc_aprintf(({const char*_tmp66="check_public_type (can't find datatypefield %s)";_tag_dyneither(_tmp66,sizeof(char),48);}),_tag_dyneither(_tmp65,sizeof(void*),1));});}));{
struct Cyc_Absyn_Datatypefield*tuf=tuf1;
# 328
if(tud->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,({const char*_tmp68="datatype";_tag_dyneither(_tmp68,sizeof(char),9);}),tud->name);
return 0;}
# 332
if(tud->sc == Cyc_Absyn_Abstract){
Cyc_Interface_abstract_err(obj,name,({const char*_tmp69="datatype";_tag_dyneither(_tmp69,sizeof(char),9);}),tud->name);
return 0;}
# 336
if(tuf->sc == Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp6A[1]={& _tmp6C};Cyc_aprintf(({const char*_tmp6B="field %s of";_tag_dyneither(_tmp6B,sizeof(char),12);}),_tag_dyneither(_tmp6A,sizeof(void*),1));});}),tud->name);
return 0;}
# 341
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp79) && 
Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};}}else{goto _LL1D;}default: _LL1D: _LL1E:
# 344
 return 1;}_LLA:;}struct _tuple21{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Tcenv_Genv*f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*f6;};
# 350
static void Cyc_Interface_extract_aggrdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Aggrdecl**dp){
# 352
struct _tuple21*_tmp7C=env;struct Cyc_Interface_Ienv*_tmp8D;struct Cyc_Interface_Ienv*_tmp8C;int _tmp8B;struct Cyc_Tcenv_Genv*_tmp8A;struct Cyc_Interface_Seen*_tmp89;_LL32: _tmp8D=_tmp7C->f1;_tmp8C=_tmp7C->f2;_tmp8B=_tmp7C->f3;_tmp8A=_tmp7C->f4;_tmp89=_tmp7C->f5;_LL33:;{
struct Cyc_Absyn_Aggrdecl*_tmp7D=*dp;
enum Cyc_Absyn_Scope _tmp7E=_tmp7D->sc;enum Cyc_Absyn_Scope _tmp7F=_tmp7E;switch(_tmp7F){case Cyc_Absyn_Static: _LL35: _LL36:
# 356
 if(_tmp8B  && _tmp7D->impl == 0)
Cyc_Interface_fields_err(({const char*_tmp80="static";_tag_dyneither(_tmp80,sizeof(char),7);}),({const char*_tmp81="struct/union";_tag_dyneither(_tmp81,sizeof(char),13);}),_tmp7D->name);
goto _LL34;case Cyc_Absyn_Abstract: _LL37: _LL38:
# 360
 if(_tmp7D->impl == 0){
if(_tmp8B)
Cyc_Interface_fields_err(({const char*_tmp82="abstract";_tag_dyneither(_tmp82,sizeof(char),9);}),({const char*_tmp83="struct/union";_tag_dyneither(_tmp83,sizeof(char),13);}),_tmp7D->name);}else{
# 364
_tmp7D=({struct Cyc_Absyn_Aggrdecl*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=*_tmp7D;_tmp84;});
_tmp7D->impl=0;}
# 367
if(Cyc_Interface_check_public_aggrdecl(_tmp8A,_tmp89,_tmp7D))
_tmp8C->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp8C->aggrdecls,x,_tmp7D);
goto _LL34;case Cyc_Absyn_Public: _LL39: _LL3A:
# 371
 if(_tmp7D->impl == 0){
Cyc_Interface_fields_err(({const char*_tmp85="public";_tag_dyneither(_tmp85,sizeof(char),7);}),({const char*_tmp86="struct/union";_tag_dyneither(_tmp86,sizeof(char),13);}),_tmp7D->name);
_tmp7D=({struct Cyc_Absyn_Aggrdecl*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=*_tmp7D;_tmp87;});
_tmp7D->sc=Cyc_Absyn_Abstract;}
# 376
if(Cyc_Interface_check_public_aggrdecl(_tmp8A,_tmp89,_tmp7D))
_tmp8C->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp8C->aggrdecls,x,_tmp7D);
goto _LL34;case Cyc_Absyn_ExternC: _LL3B: _LL3C:
 goto _LL3E;case Cyc_Absyn_Extern: _LL3D: _LL3E:
# 381
 if(Cyc_Interface_check_public_aggrdecl(_tmp8A,_tmp89,_tmp7D))
_tmp8D->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp8D->aggrdecls,x,_tmp7D);
goto _LL34;default: _LL3F: _LL40:
# 385
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp88="add_aggrdecl";_tag_dyneither(_tmp88,sizeof(char),13);}));
goto _LL34;}_LL34:;};}
# 389
static void Cyc_Interface_extract_enumdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Enumdecl**dp){
# 391
struct _tuple21*_tmp8E=env;struct Cyc_Interface_Ienv*_tmp9F;struct Cyc_Interface_Ienv*_tmp9E;int _tmp9D;struct Cyc_Tcenv_Genv*_tmp9C;struct Cyc_Interface_Seen*_tmp9B;_LL42: _tmp9F=_tmp8E->f1;_tmp9E=_tmp8E->f2;_tmp9D=_tmp8E->f3;_tmp9C=_tmp8E->f4;_tmp9B=_tmp8E->f5;_LL43:;{
struct Cyc_Absyn_Enumdecl*_tmp8F=*dp;
enum Cyc_Absyn_Scope _tmp90=_tmp8F->sc;enum Cyc_Absyn_Scope _tmp91=_tmp90;switch(_tmp91){case Cyc_Absyn_Static: _LL45: _LL46:
# 395
 if(_tmp9D  && _tmp8F->fields == 0)
Cyc_Interface_fields_err(({const char*_tmp92="static";_tag_dyneither(_tmp92,sizeof(char),7);}),({const char*_tmp93="enum";_tag_dyneither(_tmp93,sizeof(char),5);}),_tmp8F->name);
goto _LL44;case Cyc_Absyn_Abstract: _LL47: _LL48:
# 399
 if(_tmp8F->fields == 0){
if(_tmp9D)
Cyc_Interface_fields_err(({const char*_tmp94="abstract";_tag_dyneither(_tmp94,sizeof(char),9);}),({const char*_tmp95="enum";_tag_dyneither(_tmp95,sizeof(char),5);}),_tmp8F->name);}else{
# 403
_tmp8F=({struct Cyc_Absyn_Enumdecl*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=*_tmp8F;_tmp96;});
_tmp8F->fields=0;}
# 406
if(Cyc_Interface_check_public_enumdecl(_tmp9C,_tmp9B,_tmp8F))
_tmp9E->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp9E->enumdecls,x,_tmp8F);
goto _LL44;case Cyc_Absyn_Public: _LL49: _LL4A:
# 410
 if(_tmp8F->fields == 0){
Cyc_Interface_fields_err(({const char*_tmp97="public";_tag_dyneither(_tmp97,sizeof(char),7);}),({const char*_tmp98="enum";_tag_dyneither(_tmp98,sizeof(char),5);}),_tmp8F->name);
_tmp8F=({struct Cyc_Absyn_Enumdecl*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=*_tmp8F;_tmp99;});
_tmp8F->sc=Cyc_Absyn_Abstract;}
# 415
if(Cyc_Interface_check_public_enumdecl(_tmp9C,_tmp9B,_tmp8F))
_tmp9E->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp9E->enumdecls,x,_tmp8F);
goto _LL44;case Cyc_Absyn_ExternC: _LL4B: _LL4C:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4D: _LL4E:
# 420
 if(Cyc_Interface_check_public_enumdecl(_tmp9C,_tmp9B,_tmp8F))
_tmp9F->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp9F->enumdecls,x,_tmp8F);
goto _LL44;default: _LL4F: _LL50:
# 424
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp9A="add_enumdecl";_tag_dyneither(_tmp9A,sizeof(char),13);}));
goto _LL44;}_LL44:;};}
# 429
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 431
struct Cyc_Interface_Ienv*env;
{enum Cyc_Absyn_Scope _tmpA0=f->sc;enum Cyc_Absyn_Scope _tmpA1=_tmpA0;switch(_tmpA1){case Cyc_Absyn_Static: _LL52: _LL53:
 return;case Cyc_Absyn_Extern: _LL54: _LL55:
 env=i->imports;goto _LL51;case Cyc_Absyn_Public: _LL56: _LL57:
 env=i->exports;goto _LL51;default: _LL58: _LL59:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpA2="add_xdatatypefielddecl";_tag_dyneither(_tmpA2,sizeof(char),23);}));}_LL51:;}
# 439
env->xdatatypefielddecls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(env->xdatatypefielddecls,f->name,({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->base=d;_tmpA3->field=f;_tmpA3;}));}
# 444
static void Cyc_Interface_extract_datatypedecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Datatypedecl**dp){
# 446
struct _tuple21*_tmpA4=env;struct Cyc_Interface_Ienv*_tmpBC;struct Cyc_Interface_Ienv*_tmpBB;int _tmpBA;struct Cyc_Tcenv_Genv*_tmpB9;struct Cyc_Interface_Seen*_tmpB8;struct Cyc_Interface_I*_tmpB7;_LL5B: _tmpBC=_tmpA4->f1;_tmpBB=_tmpA4->f2;_tmpBA=_tmpA4->f3;_tmpB9=_tmpA4->f4;_tmpB8=_tmpA4->f5;_tmpB7=_tmpA4->f6;_LL5C:;{
struct Cyc_Absyn_Datatypedecl*_tmpA5=*dp;
# 449
enum Cyc_Absyn_Scope _tmpA6=_tmpA5->sc;enum Cyc_Absyn_Scope _tmpA7=_tmpA6;switch(_tmpA7){case Cyc_Absyn_Static: _LL5E: _LL5F:
# 451
 if((!_tmpA5->is_extensible  && _tmpBA) && _tmpA5->fields == 0)
Cyc_Interface_fields_err(({const char*_tmpA8="static";_tag_dyneither(_tmpA8,sizeof(char),7);}),({const char*_tmpA9="datatype";_tag_dyneither(_tmpA9,sizeof(char),9);}),_tmpA5->name);
# 454
goto _LL5D;case Cyc_Absyn_Abstract: _LL60: _LL61:
# 456
 if(_tmpA5->fields == 0){
if(!_tmpA5->is_extensible  && _tmpBA)
Cyc_Interface_fields_err(({const char*_tmpAA="abstract";_tag_dyneither(_tmpAA,sizeof(char),9);}),({const char*_tmpAB="datatype";_tag_dyneither(_tmpAB,sizeof(char),9);}),_tmpA5->name);}else{
# 461
_tmpA5=({struct Cyc_Absyn_Datatypedecl*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=*_tmpA5;_tmpAC;});
_tmpA5->fields=0;}
# 464
if(Cyc_Interface_check_public_datatypedecl(_tmpB9,_tmpB8,_tmpA5))
_tmpBB->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpBB->datatypedecls,x,_tmpA5);
goto _LL5D;case Cyc_Absyn_Public: _LL62: _LL63:
# 468
 _tmpA5=({struct Cyc_Absyn_Datatypedecl*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=*_tmpA5;_tmpAD;});
if(!_tmpA5->is_extensible  && _tmpA5->fields == 0){
Cyc_Interface_fields_err(({const char*_tmpAE="public";_tag_dyneither(_tmpAE,sizeof(char),7);}),({const char*_tmpAF="datatype";_tag_dyneither(_tmpAF,sizeof(char),9);}),_tmpA5->name);
_tmpA5->sc=Cyc_Absyn_Abstract;}
# 473
if(Cyc_Interface_check_public_datatypedecl(_tmpB9,_tmpB8,_tmpA5)){
if(_tmpA5->is_extensible  && _tmpA5->fields != 0){
struct Cyc_List_List*_tmpB0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpA5->fields))->v;
_tmpA5->fields=0;{
struct Cyc_List_List*_tmpB1=_tmpB0;for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpB7,_tmpA5,(struct Cyc_Absyn_Datatypefield*)_tmpB1->hd);}};}
# 481
_tmpBB->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpBB->datatypedecls,x,_tmpA5);}
# 483
goto _LL5D;case Cyc_Absyn_ExternC: _LL64: _LL65:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpB2="extract_datatypedecl";_tag_dyneither(_tmpB2,sizeof(char),21);}));case Cyc_Absyn_Extern: _LL66: _LL67:
# 486
 if(Cyc_Interface_check_public_datatypedecl(_tmpB9,_tmpB8,_tmpA5)){
if(_tmpA5->is_extensible  && _tmpA5->fields != 0){
_tmpA5=({struct Cyc_Absyn_Datatypedecl*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=*_tmpA5;_tmpB3;});{
struct Cyc_List_List*_tmpB4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpA5->fields))->v;
_tmpA5->fields=0;{
struct Cyc_List_List*_tmpB5=_tmpB4;for(0;_tmpB5 != 0;_tmpB5=_tmpB5->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpB7,_tmpA5,(struct Cyc_Absyn_Datatypefield*)_tmpB5->hd);}};};}
# 495
_tmpBC->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpBC->datatypedecls,x,_tmpA5);}
# 497
goto _LL5D;default: _LL68: _LL69:
# 499
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpB6="add_datatypedecl";_tag_dyneither(_tmpB6,sizeof(char),17);}));
goto _LL5D;}_LL5D:;};}
# 504
static void Cyc_Interface_extract_typedef(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d){
# 506
struct _tuple21*_tmpBD=env;struct Cyc_Interface_Ienv*_tmpC0;struct Cyc_Tcenv_Genv*_tmpBF;struct Cyc_Interface_Seen*_tmpBE;_LL6B: _tmpC0=_tmpBD->f2;_tmpBF=_tmpBD->f4;_tmpBE=_tmpBD->f5;_LL6C:;
if(Cyc_Interface_check_public_typedefdecl(_tmpBF,_tmpBE,d))
_tmpC0->typedefdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpC0->typedefdecls,x,d);}struct _tuple22{void*f1;int f2;};
# 511
static void Cyc_Interface_extract_ordinarie(struct _tuple21*env,struct _tuple0*x,struct _tuple22*v){
# 513
struct _tuple21*_tmpC1=env;struct Cyc_Interface_Ienv*_tmpD4;struct Cyc_Interface_Ienv*_tmpD3;int _tmpD2;struct Cyc_Tcenv_Genv*_tmpD1;struct Cyc_Interface_Seen*_tmpD0;_LL6E: _tmpD4=_tmpC1->f1;_tmpD3=_tmpC1->f2;_tmpD2=_tmpC1->f3;_tmpD1=_tmpC1->f4;_tmpD0=_tmpC1->f5;_LL6F:;{
# 515
void*_tmpC2=(*v).f1;void*_tmpC3=_tmpC2;void*_tmpCF;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC3)->tag == 0){_LL71: _tmpCF=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC3)->f1;_LL72:
# 517
{void*_tmpC4=_tmpCF;struct Cyc_Absyn_Vardecl*_tmpCE;struct Cyc_Absyn_Fndecl*_tmpCD;switch(*((int*)_tmpC4)){case 2: _LL76: _tmpCD=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC4)->f1;_LL77: {
# 519
struct Cyc_Absyn_Vardecl*_tmpC5=({struct Cyc_Absyn_Vardecl*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->sc=_tmpCD->sc;_tmpC6->name=_tmpCD->name;_tmpC6->tq=
# 522
Cyc_Absyn_empty_tqual(0);_tmpC6->type=(void*)_check_null(_tmpCD->cached_typ);_tmpC6->initializer=0;_tmpC6->rgn=0;_tmpC6->attributes=0;_tmpC6->escapes=0;_tmpC6;});
# 529
_tmpD2=0;
_tmpCE=_tmpC5;goto _LL79;}case 1: _LL78: _tmpCE=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC4)->f1;_LL79:
# 532
 if(_tmpCE->initializer != 0){
_tmpCE=({struct Cyc_Absyn_Vardecl*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=*_tmpCE;_tmpC7;});
_tmpCE->initializer=0;}
# 536
{enum Cyc_Absyn_Scope _tmpC8=_tmpCE->sc;enum Cyc_Absyn_Scope _tmpC9=_tmpC8;switch(_tmpC9){case Cyc_Absyn_Static: _LL7D: _LL7E:
# 538
 if(_tmpD2  && Cyc_Tcutil_is_function_type(_tmpCE->type))
Cyc_Interface_body_err(({const char*_tmpCA="static";_tag_dyneither(_tmpCA,sizeof(char),7);}),_tmpCE->name);
# 541
goto _LL7C;case Cyc_Absyn_Register: _LL7F: _LL80:
 goto _LL82;case Cyc_Absyn_Abstract: _LL81: _LL82:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpCB="extract_ordinarie";_tag_dyneither(_tmpCB,sizeof(char),18);}));case Cyc_Absyn_Public: _LL83: _LL84:
# 545
 if(_tmpD2  && Cyc_Tcutil_is_function_type(_tmpCE->type))
Cyc_Interface_body_err(({const char*_tmpCC="public";_tag_dyneither(_tmpCC,sizeof(char),7);}),_tmpCE->name);
# 548
if(Cyc_Interface_check_public_vardecl(_tmpD1,_tmpD0,_tmpCE))
_tmpD3->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpD3->vardecls,x,_tmpCE);
goto _LL7C;case Cyc_Absyn_ExternC: _LL85: _LL86:
 goto _LL88;default: _LL87: _LL88:
# 553
 if(Cyc_Interface_check_public_vardecl(_tmpD1,_tmpD0,_tmpCE))
_tmpD4->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpD4->vardecls,x,_tmpCE);
goto _LL7C;}_LL7C:;}
# 557
goto _LL75;default: _LL7A: _LL7B:
 goto _LL75;}_LL75:;}
# 560
goto _LL70;}else{_LL73: _LL74:
 goto _LL70;}_LL70:;};}struct _tuple23{struct Cyc_Interface_I*f1;int f2;struct Cyc_Tcenv_Genv*f3;struct Cyc_Interface_Seen*f4;};
# 565
static void Cyc_Interface_extract_f(struct _tuple23*env_f){
struct _tuple23*_tmpD5=env_f;struct Cyc_Interface_I*_tmpDB;int _tmpDA;struct Cyc_Tcenv_Genv*_tmpD9;struct Cyc_Interface_Seen*_tmpD8;_LL8A: _tmpDB=_tmpD5->f1;_tmpDA=_tmpD5->f2;_tmpD9=_tmpD5->f3;_tmpD8=_tmpD5->f4;_LL8B:;{
struct _tuple21 _tmpD6=({struct _tuple21 _tmpD7;_tmpD7.f1=_tmpDB->imports;_tmpD7.f2=_tmpDB->exports;_tmpD7.f3=_tmpDA;_tmpD7.f4=_tmpD9;_tmpD7.f5=_tmpD8;_tmpD7.f6=_tmpDB;_tmpD7;});
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmpD6,_tmpD9->aggrdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,& _tmpD6,_tmpD9->datatypedecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmpD6,_tmpD9->enumdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmpD6,_tmpD9->typedefs);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct _tuple22*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmpD6,_tmpD9->ordinaries);};}
# 575
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Tcenv_Genv*ae,int check_complete_defs){
struct _tuple23 env=({struct _tuple23 _tmpDC;_tmpDC.f1=
Cyc_Interface_empty();_tmpDC.f2=check_complete_defs;_tmpDC.f3=ae;_tmpDC.f4=Cyc_Interface_new_seen();_tmpDC;});
Cyc_Interface_extract_f(& env);
return env.f1;}
# 582
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae){
return Cyc_Interface_gen_extract(ae,1);}
# 588
inline static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){
({struct Cyc_String_pa_PrintArg_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg2);({void*_tmpDD[1]={& _tmpDF};Cyc_Tcdecl_merr(0,msg1,({const char*_tmpDE="%s";_tag_dyneither(_tmpDE,sizeof(char),3);}),_tag_dyneither(_tmpDD,sizeof(void*),1));});});}struct _tuple24{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 592
static void Cyc_Interface_incl_dict_f(struct _tuple24*env,struct _tuple0*x,void*y1){
# 599
struct _tuple24*_tmpE0=env;int*_tmpEF;struct Cyc_Dict_Dict _tmpEE;int(*_tmpED)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr _tmpEC;struct _dyneither_ptr*_tmpEB;_LL8D: _tmpEF=(int*)& _tmpE0->f1;_tmpEE=_tmpE0->f2;_tmpED=_tmpE0->f3;_tmpEC=_tmpE0->f4;_tmpEB=_tmpE0->f5;_LL8E:;{
struct _handler_cons _tmpE1;_push_handler(& _tmpE1);{int _tmpE3=0;if(setjmp(_tmpE1.handler))_tmpE3=1;if(!_tmpE3){
{void*_tmpE4=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmpEE,x);
if(!_tmpED(y1,_tmpE4,_tmpEB))*_tmpEF=0;}
# 601
;_pop_handler();}else{void*_tmpE2=(void*)_exn_thrown;void*_tmpE5=_tmpE2;void*_tmpEA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpE5)->tag == Cyc_Dict_Absent){_LL90: _LL91:
# 605
 Cyc_Interface_check_err(_tmpEB,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));({struct Cyc_String_pa_PrintArg_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEC);({void*_tmpE6[2]={& _tmpE8,& _tmpE9};Cyc_aprintf(({const char*_tmpE7="%s %s is missing";_tag_dyneither(_tmpE7,sizeof(char),17);}),_tag_dyneither(_tmpE6,sizeof(void*),2));});});}));
*_tmpEF=0;
goto _LL8F;}else{_LL92: _tmpEA=_tmpE5;_LL93:(int)_rethrow(_tmpEA);}_LL8F:;}};};}
# 611
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 616
struct _tuple24 _tmpF0=({struct _tuple24 _tmpF1;_tmpF1.f1=1;_tmpF1.f2=dic2;_tmpF1.f3=incl_f;_tmpF1.f4=t;_tmpF1.f5=msg;_tmpF1;});
((void(*)(void(*f)(struct _tuple24*,struct _tuple0*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmpF0,dic1);
return _tmpF0.f1;}
# 633 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmpF2=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmpF2 == 0)return 0;if(d0 != _tmpF2){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmpF3[1]={& _tmpF5};Cyc_aprintf(({const char*_tmpF4="declaration of type %s discloses too much information";_tag_dyneither(_tmpF4,sizeof(char),54);}),_tag_dyneither(_tmpF3,sizeof(void*),1));});}));return 0;}return 1;}
# 635
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmpF6=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0,msg);if(_tmpF6 == 0)return 0;if(d0 != _tmpF6){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmpF7[1]={& _tmpF9};Cyc_aprintf(({const char*_tmpF8="declaration of datatype %s discloses too much information";_tag_dyneither(_tmpF8,sizeof(char),58);}),_tag_dyneither(_tmpF7,sizeof(void*),1));});}));return 0;}return 1;}
# 637
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*_tmpFA=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmpFA == 0)return 0;if(d0 != _tmpFA){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmpFB[1]={& _tmpFD};Cyc_aprintf(({const char*_tmpFC="declaration of enum %s discloses too much information";_tag_dyneither(_tmpFC,sizeof(char),54);}),_tag_dyneither(_tmpFB,sizeof(void*),1));});}));return 0;}return 1;}
# 639
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*_tmpFE=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmpFE == 0)return 0;if(d0 != _tmpFE){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmpFF[1]={& _tmp101};Cyc_aprintf(({const char*_tmp100="declaration of variable %s discloses too much information";_tag_dyneither(_tmp100,sizeof(char),58);}),_tag_dyneither(_tmpFF,sizeof(void*),1));});}));return 0;}return 1;}
# 641
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp102=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp102 == 0)return 0;if(d0 != _tmp102){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp105;_tmp105.tag=0;_tmp105.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));({void*_tmp103[1]={& _tmp105};Cyc_aprintf(({const char*_tmp104="declaration of typedef %s discloses too much information";_tag_dyneither(_tmp104,sizeof(char),57);}),_tag_dyneither(_tmp103,sizeof(void*),1));});}));return 0;}return 1;}
# 643
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp106=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0,msg);if(_tmp106 == 0)return 0;if(d0 != _tmp106){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp109;_tmp109.tag=0;_tmp109.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name));({void*_tmp107[1]={& _tmp109};Cyc_aprintf(({const char*_tmp108="declaration of xdatatypefield %s discloses too much information";_tag_dyneither(_tmp108,sizeof(char),64);}),_tag_dyneither(_tmp107,sizeof(void*),1));});}));return 0;}return 1;}
# 648
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);
# 657
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
int _tmp10A=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,({const char*_tmp115="type";_tag_dyneither(_tmp115,sizeof(char),5);}),msg);
int _tmp10B=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,Cyc_Interface_incl_datatypedecl,({const char*_tmp114="datatype";_tag_dyneither(_tmp114,sizeof(char),9);}),msg);
int _tmp10C=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,({const char*_tmp113="enum";_tag_dyneither(_tmp113,sizeof(char),5);}),msg);
# 662
int _tmp10D=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),Cyc_Tcdecl_merge_typedefdecl,({const char*_tmp112="typedef";_tag_dyneither(_tmp112,sizeof(char),8);}),msg)!= 0;
# 665
int _tmp10E=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,({const char*_tmp111="variable";_tag_dyneither(_tmp111,sizeof(char),9);}),msg);
int _tmp10F=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,({const char*_tmp110="xdatatypefield";_tag_dyneither(_tmp110,sizeof(char),15);}),msg);
return((((_tmp10A  && _tmp10B) && _tmp10C) && _tmp10D) && _tmp10E) && _tmp10F;}
# 670
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp116;_push_handler(& _tmp116);{int _tmp118=0;if(setjmp(_tmp116.handler))_tmp118=1;if(!_tmp118){
{int _tmp119=1;
struct _dyneither_ptr*msg=0;
# 675
if(info != 0)
msg=({struct _dyneither_ptr*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp11B[2]={& _tmp11D,& _tmp11E};Cyc_aprintf(({const char*_tmp11C="checking inclusion of %s exports into %s exports,";_tag_dyneither(_tmp11C,sizeof(char),50);}),_tag_dyneither(_tmp11B,sizeof(void*),2));});});});_tmp11A;});
if(!Cyc_Interface_incl_ienv(i1->exports,i2->exports,msg))_tmp119=0;
# 679
if(info != 0)
msg=({struct _dyneither_ptr*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0;_tmp122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({void*_tmp120[2]={& _tmp122,& _tmp123};Cyc_aprintf(({const char*_tmp121="checking inclusion of %s imports into %s imports,";_tag_dyneither(_tmp121,sizeof(char),50);}),_tag_dyneither(_tmp120,sizeof(void*),2));});});});_tmp11F;});
if(!Cyc_Interface_incl_ienv(i2->imports,i1->imports,msg))_tmp119=0;{
# 683
int _tmp124=_tmp119;_npop_handler(0);return _tmp124;};}
# 672
;_pop_handler();}else{void*_tmp117=(void*)_exn_thrown;void*_tmp125=_tmp117;void*_tmp126;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp125)->tag == Cyc_Tcdecl_Incompatible){_LL95: _LL96:
# 685
 return 0;}else{_LL97: _tmp126=_tmp125;_LL98:(int)_rethrow(_tmp126);}_LL94:;}};}struct _tuple25{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;void*(*f5)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr f6;struct _dyneither_ptr*f7;};
# 693
void Cyc_Interface_compat_merge_dict_f(struct _tuple25*env,struct _tuple0*x,void*y2){
# 700
struct _tuple25*_tmp127=env;int*_tmp140;struct Cyc_Dict_Dict*_tmp13F;struct Cyc_Dict_Dict _tmp13E;struct Cyc_Dict_Dict _tmp13D;void*(*_tmp13C)(void*,void*,unsigned int,struct _dyneither_ptr*);struct _dyneither_ptr _tmp13B;struct _dyneither_ptr*_tmp13A;_LL9A: _tmp140=(int*)& _tmp127->f1;_tmp13F=(struct Cyc_Dict_Dict*)& _tmp127->f2;_tmp13E=_tmp127->f3;_tmp13D=_tmp127->f4;_tmp13C=_tmp127->f5;_tmp13B=_tmp127->f6;_tmp13A=_tmp127->f7;_LL9B:;{
void*y;
{struct _handler_cons _tmp128;_push_handler(& _tmp128);{int _tmp12A=0;if(setjmp(_tmp128.handler))_tmp12A=1;if(!_tmp12A){
{void*_tmp12B=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp13E,x);
# 706
void*_tmp12C=_tmp13C(_tmp12B,y2,0,_tmp13A);
if(!((unsigned int)_tmp12C)){
*_tmp140=0;
_npop_handler(0);return;}
# 711
y=_tmp12C;}
# 703
;_pop_handler();}else{void*_tmp129=(void*)_exn_thrown;void*_tmp12D=_tmp129;void*_tmp12E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp12D)->tag == Cyc_Dict_Absent){_LL9D: _LL9E:
# 714
 y=y2;
goto _LL9C;}else{_LL9F: _tmp12E=_tmp12D;_LLA0:(int)_rethrow(_tmp12E);}_LL9C:;}};}{
# 718
struct _handler_cons _tmp12F;_push_handler(& _tmp12F);{int _tmp131=0;if(setjmp(_tmp12F.handler))_tmp131=1;if(!_tmp131){
{void*_tmp132=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp13D,x);
# 723
void*_tmp133=_tmp13C(_tmp132,y,0,_tmp13A);
if(_tmp133 != _tmp132){
if((unsigned int)_tmp133)
Cyc_Interface_check_err(_tmp13A,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(x));({struct Cyc_String_pa_PrintArg_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp13B);({void*_tmp134[2]={& _tmp136,& _tmp137};Cyc_aprintf(({const char*_tmp135="abstract %s %s is being imported as non-abstract";_tag_dyneither(_tmp135,sizeof(char),49);}),_tag_dyneither(_tmp134,sizeof(void*),2));});});}));
*_tmp140=0;}}
# 719
;_pop_handler();}else{void*_tmp130=(void*)_exn_thrown;void*_tmp138=_tmp130;void*_tmp139;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp138)->tag == Cyc_Dict_Absent){_LLA2: _LLA3:
# 732
 if(*_tmp140)
*_tmp13F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp13F,x,y);
goto _LLA1;}else{_LLA4: _tmp139=_tmp138;_LLA5:(int)_rethrow(_tmp139);}_LLA1:;}};};};}
# 738
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 746
struct _tuple25 _tmp141=({struct _tuple25 _tmp143;_tmp143.f1=1;_tmp143.f2=dic1;_tmp143.f3=dic1;_tmp143.f4=excl;_tmp143.f5=merge_f;_tmp143.f6=t;_tmp143.f7=msg;_tmp143;});
# 748
((void(*)(void(*f)(struct _tuple25*,struct _tuple0*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,& _tmp141,dic2);
# 750
if(_tmp141.f1)
return({struct Cyc_Dict_Dict*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142[0]=_tmp141.f2;_tmp142;});
return 0;}
# 760
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp144=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,({const char*_tmp150="type";_tag_dyneither(_tmp150,sizeof(char),5);}),msg);
struct Cyc_Dict_Dict*_tmp145=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,({const char*_tmp14F="datatype";_tag_dyneither(_tmp14F,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp146=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,({const char*_tmp14E="enum";_tag_dyneither(_tmp14E,sizeof(char),5);}),msg);
struct Cyc_Dict_Dict*_tmp147=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,({const char*_tmp14D="typedef";_tag_dyneither(_tmp14D,sizeof(char),8);}),msg);
struct Cyc_Dict_Dict*_tmp148=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,({const char*_tmp14C="variable";_tag_dyneither(_tmp14C,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp149=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,({const char*_tmp14B="xdatatypefield";_tag_dyneither(_tmp14B,sizeof(char),15);}),msg);
if(((((!((unsigned int)_tmp144) || !((unsigned int)_tmp145)) || !((unsigned int)_tmp146)) || !((unsigned int)_tmp147)) || !((unsigned int)_tmp148)) || !((unsigned int)_tmp149))
return 0;
return({struct Cyc_Interface_Ienv*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->aggrdecls=*_tmp144;_tmp14A->datatypedecls=*_tmp145;_tmp14A->enumdecls=*_tmp146;_tmp14A->typedefdecls=*_tmp147;_tmp14A->vardecls=*_tmp148;_tmp14A->xdatatypefielddecls=*_tmp149;_tmp14A;});}struct _tuple26{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 773
void Cyc_Interface_disj_merge_dict_f(struct _tuple26*env,struct _tuple0*x,void*y){
# 776
struct _tuple26*_tmp151=env;int*_tmp15A;struct Cyc_Dict_Dict*_tmp159;struct Cyc_Dict_Dict _tmp158;struct _dyneither_ptr _tmp157;struct _dyneither_ptr*_tmp156;_LLA7: _tmp15A=(int*)& _tmp151->f1;_tmp159=(struct Cyc_Dict_Dict*)& _tmp151->f2;_tmp158=_tmp151->f3;_tmp157=_tmp151->f4;_tmp156=_tmp151->f5;_LLA8:;
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(_tmp158,x)){
Cyc_Interface_check_err(_tmp156,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(x));({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp157);({void*_tmp152[2]={& _tmp154,& _tmp155};Cyc_aprintf(({const char*_tmp153="%s %s is exported more than once";_tag_dyneither(_tmp153,sizeof(char),33);}),_tag_dyneither(_tmp152,sizeof(void*),2));});});}));
*_tmp15A=0;}else{
# 782
if(*_tmp15A)*_tmp159=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp159,x,y);}}
# 786
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 790
struct _tuple26 _tmp15B=({struct _tuple26 _tmp15D;_tmp15D.f1=1;_tmp15D.f2=dic1;_tmp15D.f3=dic1;_tmp15D.f4=t;_tmp15D.f5=msg;_tmp15D;});
((void(*)(void(*f)(struct _tuple26*,struct _tuple0*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp15B,dic2);
if(_tmp15B.f1)
return({struct Cyc_Dict_Dict*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=_tmp15B.f2;_tmp15C;});
return 0;}
# 797
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp15E=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),Cyc_Tcdecl_merge_aggrdecl,({const char*_tmp16A="type";_tag_dyneither(_tmp16A,sizeof(char),5);}),msg);
# 801
struct Cyc_Dict_Dict*_tmp15F=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->datatypedecls,ie2->datatypedecls,({const char*_tmp169="datatype";_tag_dyneither(_tmp169,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp160=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,ie2->enumdecls,({const char*_tmp168="enum";_tag_dyneither(_tmp168,sizeof(char),5);}),msg);
# 804
struct Cyc_Dict_Dict*_tmp161=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned int,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),Cyc_Tcdecl_merge_typedefdecl,({const char*_tmp167="typedef";_tag_dyneither(_tmp167,sizeof(char),8);}),msg);
# 807
struct Cyc_Dict_Dict*_tmp162=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,ie2->vardecls,({const char*_tmp166="variable";_tag_dyneither(_tmp166,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp163=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,({const char*_tmp165="xdatatypefield";_tag_dyneither(_tmp165,sizeof(char),15);}),msg);
# 811
if(((((!((unsigned int)_tmp15E) || !((unsigned int)_tmp15F)) || !((unsigned int)_tmp160)) || !((unsigned int)_tmp161)) || !((unsigned int)_tmp162)) || !((unsigned int)_tmp163))
return 0;
return({struct Cyc_Interface_Ienv*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->aggrdecls=*_tmp15E;_tmp164->datatypedecls=*_tmp15F;_tmp164->enumdecls=*_tmp160;_tmp164->typedefdecls=*_tmp161;_tmp164->vardecls=*_tmp162;_tmp164->xdatatypefielddecls=*_tmp163;_tmp164;});}
# 817
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp16B;_push_handler(& _tmp16B);{int _tmp16D=0;if(setjmp(_tmp16B.handler))_tmp16D=1;if(!_tmp16D){
{struct _dyneither_ptr*msg=0;
# 821
if(info != 0)
msg=({struct _dyneither_ptr*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0;_tmp171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp16F[2]={& _tmp171,& _tmp172};Cyc_aprintf(({const char*_tmp170="merging exports of %s and %s,";_tag_dyneither(_tmp170,sizeof(char),30);}),_tag_dyneither(_tmp16F,sizeof(void*),2));});});});_tmp16E;});{
# 825
struct Cyc_Interface_Ienv*_tmp173=Cyc_Interface_disj_merge_ienv(i1->exports,i2->exports,msg);
if(_tmp173 == 0){struct Cyc_Interface_I*_tmp174=0;_npop_handler(0);return _tmp174;}
# 828
if(info != 0)
msg=({struct _dyneither_ptr*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);({void*_tmp176[2]={& _tmp178,& _tmp179};Cyc_aprintf(({const char*_tmp177="merging imports of %s and %s,";_tag_dyneither(_tmp177,sizeof(char),30);}),_tag_dyneither(_tmp176,sizeof(void*),2));});});});_tmp175;});{
# 832
struct Cyc_Interface_Ienv*_tmp17A=Cyc_Interface_compat_merge_ienv(i1->imports,i2->imports,_tmp173,msg);
if(_tmp17A == 0){struct Cyc_Interface_I*_tmp17B=0;_npop_handler(0);return _tmp17B;}{
# 835
struct Cyc_Interface_I*_tmp17D=({struct Cyc_Interface_I*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->imports=_tmp17A;_tmp17C->exports=_tmp173;_tmp17C;});_npop_handler(0);return _tmp17D;};};};}
# 819
;_pop_handler();}else{void*_tmp16C=(void*)_exn_thrown;void*_tmp17E=_tmp16C;void*_tmp17F;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp17E)->tag == Cyc_Tcdecl_Incompatible){_LLAA: _LLAB:
# 836
 return 0;}else{_LLAC: _tmp17F=_tmp17E;_LLAD:(int)_rethrow(_tmp17F);}_LLA9:;}};}
# 839
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li == 0)return Cyc_Interface_empty();{
# 842
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo != 0)linfo=linfo->tl;
# 847
for(0;li != 0;li=li->tl){
struct Cyc_Interface_I*_tmp180=Cyc_Interface_merge(curr_i,(struct Cyc_Interface_I*)li->hd,
curr_info != 0  && linfo != 0?({struct _tuple10*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->f1=*curr_info;_tmp186->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp186;}): 0);
if(_tmp180 == 0)return 0;
curr_i=_tmp180;
if(curr_info != 0)
curr_info=linfo != 0?({struct _dyneither_ptr*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp185;_tmp185.tag=0;_tmp185.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));({struct Cyc_String_pa_PrintArg_struct _tmp184;_tmp184.tag=0;_tmp184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);({void*_tmp182[2]={& _tmp184,& _tmp185};Cyc_aprintf(({const char*_tmp183="%s+%s";_tag_dyneither(_tmp183,sizeof(char),6);}),_tag_dyneither(_tmp182,sizeof(void*),2));});});});_tmp181;}): 0;
# 855
if(linfo != 0)linfo=linfo->tl;}
# 857
return curr_i;};}
# 860
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la == 0)return Cyc_Interface_empty();{
# 863
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo != 0)linfo=linfo->tl;
# 868
for(0;la != 0;la=la->tl){
struct Cyc_Interface_I*_tmp187=Cyc_Interface_merge(curr_i,get(la->hd),
curr_info != 0  && linfo != 0?({struct _tuple10*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->f1=*curr_info;_tmp18D->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp18D;}): 0);
if(_tmp187 == 0)return 0;
curr_i=_tmp187;
if(curr_info != 0)
curr_info=linfo != 0?({struct _dyneither_ptr*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);({void*_tmp189[2]={& _tmp18B,& _tmp18C};Cyc_aprintf(({const char*_tmp18A="%s+%s";_tag_dyneither(_tmp18A,sizeof(char),6);}),_tag_dyneither(_tmp189,sizeof(void*),2));});});});_tmp188;}): 0;
# 876
if(linfo != 0)linfo=linfo->tl;}
# 878
return curr_i;};}
# 884
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds){
struct Cyc_List_List*_tmp18E=0;
for(0;tds != 0;tds=tds->tl){
struct _tuple0*qv;
struct Cyc_Absyn_Decl*_tmp18F=(struct Cyc_Absyn_Decl*)tds->hd;
{void*_tmp190=_tmp18F->r;void*_tmp191=_tmp190;struct Cyc_Absyn_Typedefdecl*_tmp198;struct Cyc_Absyn_Enumdecl*_tmp197;struct Cyc_Absyn_Datatypedecl*_tmp196;struct Cyc_Absyn_Aggrdecl*_tmp195;struct Cyc_Absyn_Fndecl*_tmp194;struct Cyc_Absyn_Vardecl*_tmp193;switch(*((int*)_tmp191)){case 0: _LLAF: _tmp193=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp191)->f1;_LLB0:
 qv=_tmp193->name;goto _LLAE;case 1: _LLB1: _tmp194=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp191)->f1;_LLB2:
 qv=_tmp194->name;goto _LLAE;case 5: _LLB3: _tmp195=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp191)->f1;_LLB4:
 qv=_tmp195->name;goto _LLAE;case 6: _LLB5: _tmp196=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp191)->f1;_LLB6:
 qv=_tmp196->name;goto _LLAE;case 7: _LLB7: _tmp197=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp191)->f1;_LLB8:
 qv=_tmp197->name;goto _LLAE;case 8: _LLB9: _tmp198=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp191)->f1;_LLBA:
 qv=_tmp198->name;goto _LLAE;default: _LLBB: _LLBC:
 Cyc_Interface_err(({const char*_tmp192="bad decl form in Interface::add_namespace";_tag_dyneither(_tmp192,sizeof(char),42);}));return 0;}_LLAE:;}{
# 898
struct Cyc_List_List*vs;
{union Cyc_Absyn_Nmspace _tmp199=(*qv).f1;union Cyc_Absyn_Nmspace _tmp19A=_tmp199;struct Cyc_List_List*_tmp19D;struct Cyc_List_List*_tmp19C;switch((_tmp19A.C_n).tag){case 2: _LLBE: _tmp19C=(_tmp19A.Abs_n).val;_LLBF:
 vs=_tmp19C;goto _LLBD;case 3: _LLC0: _tmp19D=(_tmp19A.C_n).val;_LLC1:
 vs=_tmp19D;goto _LLBD;default: _LLC2: _LLC3:
 Cyc_Interface_err(({const char*_tmp19B="bad namespace in Interface::add_namespace";_tag_dyneither(_tmp19B,sizeof(char),42);}));return 0;}_LLBD:;}
# 904
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);
(*qv).f1=Cyc_Absyn_Rel_n(0);
for(0;vs != 0;vs=vs->tl){
_tmp18F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E[0]=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp19F;_tmp19F.tag=9;_tmp19F.f1=(struct _dyneither_ptr*)vs->hd;_tmp19F.f2=({struct Cyc_List_List*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->hd=_tmp18F;_tmp1A0->tl=0;_tmp1A0;});_tmp19F;});_tmp19E;}),_tmp18F->loc);}
_tmp18E=({struct Cyc_List_List*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->hd=_tmp18F;_tmp1A1->tl=_tmp18E;_tmp1A1;});};}
# 910
_tmp18E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp18E);
return _tmp18E;}
# 914
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1A4;_tmp1A4.tag=5;_tmp1A4.f1=d;_tmp1A4;});_tmp1A3;}),0);_tmp1A2->tl=tds;_tmp1A2;});}
# 918
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 920
d=({struct Cyc_Absyn_Aggrdecl*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5[0]=*d;_tmp1A5;});
d->impl=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1A8;_tmp1A8.tag=5;_tmp1A8.f1=d;_tmp1A8;});_tmp1A7;}),0);_tmp1A6->tl=tds;_tmp1A6;});}
# 926
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1AB;_tmp1AB.tag=6;_tmp1AB.f1=d;_tmp1AB;});_tmp1AA;}),0);_tmp1A9->tl=tds;_tmp1A9;});}static char _tmp1AC[2]="_";
# 930
static struct _dyneither_ptr Cyc_Interface_us={_tmp1AC,_tmp1AC,_tmp1AC + 2};
static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 933
static struct _tuple20*Cyc_Interface_rewrite_datatypefield_type(struct _tuple20*x){
return({struct _tuple20*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD->f1=Cyc_Absyn_empty_tqual(0);_tmp1AD->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1AF;_tmp1AF.tag=2;_tmp1AF.f1=({struct Cyc_Absyn_Tvar*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->name=Cyc_Interface_us_p;_tmp1B0->identity=- 1;_tmp1B0->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp1B0;});_tmp1AF;});_tmp1AE;});_tmp1AD;});}
# 938
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
f=({struct Cyc_Absyn_Datatypefield*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1[0]=*f;_tmp1B1;});
f->typs=((struct Cyc_List_List*(*)(struct _tuple20*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);
return f;}
# 944
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 946
d=({struct Cyc_Absyn_Datatypedecl*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[0]=*d;_tmp1B2;});
# 948
if(d->fields != 0)d->fields=({struct Cyc_Core_Opt*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v);_tmp1B3;});
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1B6;_tmp1B6.tag=6;_tmp1B6.f1=d;_tmp1B6;});_tmp1B5;}),0);_tmp1B4->tl=tds;_tmp1B4;});}
# 953
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1B9;_tmp1B9.tag=7;_tmp1B9.f1=d;_tmp1B9;});_tmp1B8;}),0);_tmp1B7->tl=tds;_tmp1B7;});}
# 957
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 959
d=({struct Cyc_Absyn_Enumdecl*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA[0]=*d;_tmp1BA;});
d->fields=0;
if(d->sc != Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp1BD;_tmp1BD.tag=7;_tmp1BD.f1=d;_tmp1BD;});_tmp1BC;}),0);_tmp1BB->tl=tds;_tmp1BB;});}
# 965
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 967
return({struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp1C0;_tmp1C0.tag=8;_tmp1C0.f1=d;_tmp1C0;});_tmp1BF;}),0);_tmp1BE->tl=tds;_tmp1BE;});}
# 970
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _tuple0*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=d;_tmp1C3;});_tmp1C2;}),0);_tmp1C1->tl=tds;_tmp1C1;});}
# 974
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 976
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1C4=d;struct Cyc_Absyn_Datatypedecl*_tmp1CC;struct Cyc_Absyn_Datatypefield*_tmp1CB;_LLC5: _tmp1CC=_tmp1C4->base;_tmp1CB=_tmp1C4->field;_LLC6:;
_tmp1CC=({struct Cyc_Absyn_Datatypedecl*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5[0]=*_tmp1CC;_tmp1C5;});
_tmp1CC->fields=({struct Cyc_Core_Opt*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->v=({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=_tmp1CB;_tmp1C7->tl=0;_tmp1C7;});_tmp1C6;});
_tmp1CC->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1CA;_tmp1CA.tag=6;_tmp1CA.f1=_tmp1CC;_tmp1CA;});_tmp1C9;}),0);_tmp1C8->tl=tds;_tmp1C8;});}
# 983
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 985
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1CD=d;struct Cyc_Absyn_Datatypedecl*_tmp1D5;struct Cyc_Absyn_Datatypefield*_tmp1D4;_LLC8: _tmp1D5=_tmp1CD->base;_tmp1D4=_tmp1CD->field;_LLC9:;
_tmp1D5=({struct Cyc_Absyn_Datatypedecl*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=*_tmp1D5;_tmp1CE;});
_tmp1D4=Cyc_Interface_rewrite_datatypefield(_tmp1D4);
_tmp1D4->sc=Cyc_Absyn_Extern;
_tmp1D5->fields=({struct Cyc_Core_Opt*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->v=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=_tmp1D4;_tmp1D0->tl=0;_tmp1D0;});_tmp1CF;});
_tmp1D5->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp1D3;_tmp1D3.tag=6;_tmp1D3.f1=_tmp1D5;_tmp1D3;});_tmp1D2;}),0);_tmp1D1->tl=tds;_tmp1D1;});}
# 994
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1D6=0;
_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp1D6);
_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp1D6);
_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp1D6);
# 1000
if(_tmp1D6 != 0){
_tmp1D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1D6);
_tmp1D6=Cyc_Interface_add_namespace(_tmp1D6);
Cyc_Absynpp_decllist2file(_tmp1D6,f);}}
# 1007
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);
if(_tmp1D7 != 0){
_tmp1D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1D7);
_tmp1D7=Cyc_Interface_add_namespace(_tmp1D7);
Cyc_Absynpp_decllist2file(_tmp1D7,f);}}
# 1016
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1D8=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,0);
if(_tmp1D8 != 0){
_tmp1D8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1D8);
_tmp1D8=Cyc_Interface_add_namespace(_tmp1D8);
Cyc_Absynpp_decllist2file(_tmp1D8,f);}}
# 1025
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1D9=0;
_tmp1D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp1D9);
_tmp1D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp1D9);
_tmp1D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp1D9);
_tmp1D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,_tmp1D9);
_tmp1D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp1D9);
# 1033
if(_tmp1D9 != 0){
_tmp1D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1D9);
_tmp1D9=Cyc_Interface_add_namespace(_tmp1D9);
Cyc_Absynpp_decllist2file(_tmp1D9,f);}}
# 1043
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
({void*_tmp1DA=0;Cyc_fprintf(f,({const char*_tmp1DB="/****** needed (headers) ******/\n";_tag_dyneither(_tmp1DB,sizeof(char),34);}),_tag_dyneither(_tmp1DA,sizeof(void*),0));});
Cyc_Interface_print_ns_headers(f,i->imports);
# 1048
({void*_tmp1DC=0;Cyc_fprintf(f,({const char*_tmp1DD="\n/****** provided (headers) ******/\n";_tag_dyneither(_tmp1DD,sizeof(char),37);}),_tag_dyneither(_tmp1DC,sizeof(void*),0));});
Cyc_Interface_print_ns_headers(f,i->exports);
# 1053
({void*_tmp1DE=0;Cyc_fprintf(f,({const char*_tmp1DF="\n/****** needed (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp1DF,sizeof(char),58);}),_tag_dyneither(_tmp1DE,sizeof(void*),0));});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->imports);
# 1056
({void*_tmp1E0=0;Cyc_fprintf(f,({const char*_tmp1E1="\n/****** provided (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp1E1,sizeof(char),60);}),_tag_dyneither(_tmp1E0,sizeof(void*),0));});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->exports);
# 1060
({void*_tmp1E2=0;Cyc_fprintf(f,({const char*_tmp1E3="\n/****** provided (typedefs) ******/\n";_tag_dyneither(_tmp1E3,sizeof(char),38);}),_tag_dyneither(_tmp1E2,sizeof(void*),0));});
Cyc_Interface_print_ns_typedefs(f,i->exports);
# 1063
({void*_tmp1E4=0;Cyc_fprintf(f,({const char*_tmp1E5="\n/****** needed (declarations) ******/\n";_tag_dyneither(_tmp1E5,sizeof(char),40);}),_tag_dyneither(_tmp1E4,sizeof(void*),0));});
Cyc_Interface_print_ns_decls(f,i->imports);
# 1066
({void*_tmp1E6=0;Cyc_fprintf(f,({const char*_tmp1E7="\n/****** provided (declarations) ******/\n";_tag_dyneither(_tmp1E7,sizeof(char),42);}),_tag_dyneither(_tmp1E6,sizeof(void*),0));});
Cyc_Interface_print_ns_decls(f,i->exports);}
# 1073
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp1E8=Cyc_Parse_parse_file(f);
struct _RegionHandle _tmp1E9=_new_region("r");struct _RegionHandle*r=& _tmp1E9;_push_region(r);
Cyc_Binding_resolve_all(_tmp1E8);{
struct Cyc_Tcenv_Tenv*_tmp1EA=Cyc_Tcenv_tc_init(r);
Cyc_Tc_tc(r,_tmp1EA,0,_tmp1E8);{
struct Cyc_Interface_I*_tmp1EB=Cyc_Interface_gen_extract(_tmp1EA->ae,0);_npop_handler(0);return _tmp1EB;};};
# 1077
;_pop_region(r);};}
# 1088
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1094
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}
