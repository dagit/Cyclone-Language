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
typedef void*Cyc_sarg_t;
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 233
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_ukey();
# 241
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion*k);
# 251
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));
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
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 127
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 73
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 107 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err_lex(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 928 "absyn.h"
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 36 "parse.h"
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
typedef struct Cyc_Yyltype Cyc_yyltype;
# 83
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 43 "lex.cyl"
void Cyc_yyerror(struct _dyneither_ptr s,int state,int token);
# 47
int Cyc_Lex_compile_for_boot_r=0;
# 50
static int Cyc_Lex_in_extern_c=0;
static struct Cyc_List_List*Cyc_Lex_prev_extern_c=0;
void Cyc_Lex_enter_extern_c(){
struct Cyc_List_List*x=0;
_swap_word(& x,& Cyc_Lex_prev_extern_c);
{struct Cyc_List_List*_tmp17A;Cyc_Lex_prev_extern_c=((_tmp17A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp17A)),((_tmp17A->hd=(void*)Cyc_Lex_in_extern_c,((_tmp17A->tl=x,_tmp17A))))));}
Cyc_Lex_in_extern_c=1;}
# 58
void Cyc_Lex_leave_extern_c(){
struct Cyc_List_List*x=0;
_swap_word(& x,& Cyc_Lex_prev_extern_c);
Cyc_Lex_in_extern_c=(int)((struct Cyc_List_List*)_check_null(x))->hd;
Cyc_Lex_prev_extern_c=((struct Cyc_List_List*)_check_null(x))->tl;
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(x,sizeof(struct Cyc_List_List),1));}struct Cyc_Lex_Trie;
# 67
typedef struct Cyc_Lex_Trie*Cyc_Lex_trie_t;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple19{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple19 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 73
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){union Cyc_Lex_TrieChildren _tmp17B;return((_tmp17B.Zero).val=0,(((_tmp17B.Zero).tag=1,_tmp17B)));}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
struct _tuple19 _tmp17E;union Cyc_Lex_TrieChildren _tmp17D;return((_tmp17D.One).val=((_tmp17E.f1=i,((_tmp17E.f2=t,_tmp17E)))),(((_tmp17D.One).tag=2,_tmp17D)));}
# 77
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
union Cyc_Lex_TrieChildren _tmp17F;return((_tmp17F.Many).val=ts,(((_tmp17F.Many).tag=3,_tmp17F)));}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};
# 88
static int Cyc_Lex_num_kws=0;struct Cyc_Lex_KeyWordInfo{int token_index;int is_c_keyword;};
# 90
static struct _dyneither_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_Lex_DynTrie{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;};
# 97
typedef struct Cyc_Lex_DynTrie*Cyc_Lex_dyn_trie_t;struct Cyc_Lex_DynTrieSymbols{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 104
typedef struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_dyn_trie_symbols_t;
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 108
static int Cyc_Lex_comment_depth=0;
# 111
static union Cyc_Absyn_Cnst Cyc_Lex_token_int={.Int_c={5,{Cyc_Absyn_Signed,0}}};static char _tmp5[8]="*bogus*";
static struct _dyneither_ptr Cyc_Lex_bogus_string={_tmp5,_tmp5,_tmp5 + 8};
static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};
# 116
static char Cyc_Lex_token_char='\000';static char _tmp6[1]="";
static struct _dyneither_ptr Cyc_Lex_token_string={_tmp6,_tmp6,_tmp6 + 1};
static struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static char _tmp7[1]="";
static struct _tuple12 Cyc_Lex_token_asm={0,{_tmp7,_tmp7,_tmp7 + 1}};
# 121
static int Cyc_Lex_runaway_start=0;
static int Cyc_Lex_paren_depth=0;
# 124
static void Cyc_Lex_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lexing_lexeme_start(lb);
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}
# 128
static void Cyc_Lex_runaway_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lex_runaway_start;
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}
# 132
static char Cyc_Lex_cnst2char(union Cyc_Absyn_Cnst cnst,struct Cyc_Lexing_lexbuf*lb){
union Cyc_Absyn_Cnst _tmp8=cnst;int _tmp9;long long _tmpA;char _tmpB;_LL1: if((_tmp8.Int_c).tag != 5)goto _LL3;_tmp9=((struct _tuple5)(_tmp8.Int_c).val).f2;_LL2:
 return(char)_tmp9;_LL3: if((_tmp8.LongLong_c).tag != 6)goto _LL5;_tmpA=((struct _tuple6)(_tmp8.LongLong_c).val).f2;_LL4:
 return(char)_tmpA;_LL5: if((_tmp8.Char_c).tag != 2)goto _LL7;_tmpB=((struct _tuple3)(_tmp8.Char_c).val).f2;_LL6:
 return _tmpB;_LL7:;_LL8:
{const char*_tmp180;Cyc_Lex_err(((_tmp180="couldn't convert constant to char!",_tag_dyneither(_tmp180,sizeof(char),35))),lb);}
return'\000';_LL0:;}struct _tuple20{struct _dyneither_ptr f1;short f2;int f3;};static char _tmpD[14]="__attribute__";static char _tmpE[9]="abstract";static char _tmpF[5]="auto";static char _tmp10[6]="break";static char _tmp11[7]="calloc";static char _tmp12[5]="case";static char _tmp13[6]="catch";static char _tmp14[5]="char";static char _tmp15[6]="const";static char _tmp16[10]="__const__";static char _tmp17[9]="continue";static char _tmp18[9]="datatype";static char _tmp19[8]="default";static char _tmp1A[3]="do";static char _tmp1B[7]="double";static char _tmp1C[12]="dynregion_t";static char _tmp1D[5]="else";static char _tmp1E[5]="enum";static char _tmp1F[7]="export";static char _tmp20[7]="extern";static char _tmp21[9]="fallthru";static char _tmp22[6]="float";static char _tmp23[4]="for";static char _tmp24[5]="goto";static char _tmp25[3]="if";static char _tmp26[7]="inline";static char _tmp27[11]="__inline__";static char _tmp28[4]="int";static char _tmp29[4]="let";static char _tmp2A[5]="long";static char _tmp2B[7]="malloc";static char _tmp2C[10]="namespace";static char _tmp2D[4]="new";static char _tmp2E[5]="NULL";static char _tmp2F[8]="numelts";static char _tmp30[9]="offsetof";static char _tmp31[20]="__cyclone_port_on__";static char _tmp32[21]="__cyclone_port_off__";static char _tmp33[8]="rcalloc";static char _tmp34[9]="region_t";static char _tmp35[7]="region";static char _tmp36[8]="regions";static char _tmp37[9]="register";static char _tmp38[13]="reset_region";static char _tmp39[9]="restrict";static char _tmp3A[7]="return";static char _tmp3B[8]="rmalloc";static char _tmp3C[5]="rnew";static char _tmp3D[6]="short";static char _tmp3E[7]="signed";static char _tmp3F[11]="__signed__";static char _tmp40[7]="sizeof";static char _tmp41[7]="static";static char _tmp42[7]="struct";static char _tmp43[7]="switch";static char _tmp44[9]="tagcheck";static char _tmp45[6]="tag_t";static char _tmp46[6]="throw";static char _tmp47[4]="try";static char _tmp48[8]="typedef";static char _tmp49[6]="union";static char _tmp4A[9]="unsigned";static char _tmp4B[13]="__unsigned__";static char _tmp4C[6]="using";static char _tmp4D[8]="valueof";static char _tmp4E[10]="valueof_t";static char _tmp4F[5]="void";static char _tmp50[9]="volatile";static char _tmp51[13]="__volatile__";static char _tmp52[6]="while";
# 145
static struct _tuple20 Cyc_Lex_rw_array[70]={{{_tmpD,_tmpD,_tmpD + 14},371,1},{{_tmpE,_tmpE,_tmpE + 9},300,0},{{_tmpF,_tmpF,_tmpF + 5},258,1},{{_tmp10,_tmp10,_tmp10 + 6},290,1},{{_tmp11,_tmp11,_tmp11 + 7},307,0},{{_tmp12,_tmp12,_tmp12 + 5},277,1},{{_tmp13,_tmp13,_tmp13 + 6},297,1},{{_tmp14,_tmp14,_tmp14 + 5},264,1},{{_tmp15,_tmp15,_tmp15 + 6},272,1},{{_tmp16,_tmp16,_tmp16 + 10},272,1},{{_tmp17,_tmp17,_tmp17 + 9},289,1},{{_tmp18,_tmp18,_tmp18 + 9},304,0},{{_tmp19,_tmp19,_tmp19 + 8},278,1},{{_tmp1A,_tmp1A,_tmp1A + 3},286,1},{{_tmp1B,_tmp1B,_tmp1B + 7},269,1},{{_tmp1C,_tmp1C,_tmp1C + 12},318,0},{{_tmp1D,_tmp1D,_tmp1D + 5},283,1},{{_tmp1E,_tmp1E,_tmp1E + 5},292,1},{{_tmp1F,_tmp1F,_tmp1F + 7},298,0},{{_tmp20,_tmp20,_tmp20 + 7},261,1},{{_tmp21,_tmp21,_tmp21 + 9},301,0},{{_tmp22,_tmp22,_tmp22 + 6},268,1},{{_tmp23,_tmp23,_tmp23 + 4},287,1},{{_tmp24,_tmp24,_tmp24 + 5},288,1},{{_tmp25,_tmp25,_tmp25 + 3},282,1},{{_tmp26,_tmp26,_tmp26 + 7},279,1},{{_tmp27,_tmp27,_tmp27 + 11},279,1},{{_tmp28,_tmp28,_tmp28 + 4},266,1},{{_tmp29,_tmp29,_tmp29 + 4},294,0},{{_tmp2A,_tmp2A,_tmp2A + 5},267,1},{{_tmp2B,_tmp2B,_tmp2B + 7},305,0},{{_tmp2C,_tmp2C,_tmp2C + 10},303,0},{{_tmp2D,_tmp2D,_tmp2D + 4},299,0},{{_tmp2E,_tmp2E,_tmp2E + 5},293,0},{{_tmp2F,_tmp2F,_tmp2F + 8},319,0},{{_tmp30,_tmp30,_tmp30 + 9},281,1},{{_tmp31,_tmp31,_tmp31 + 20},316,0},{{_tmp32,_tmp32,_tmp32 + 21},317,0},{{_tmp33,_tmp33,_tmp33 + 8},308,0},{{_tmp34,_tmp34,_tmp34 + 9},310,0},{{_tmp35,_tmp35,_tmp35 + 7},312,0},{{_tmp36,_tmp36,_tmp36 + 8},314,0},{{_tmp37,_tmp37,_tmp37 + 9},259,1},{{_tmp38,_tmp38,_tmp38 + 13},315,0},{{_tmp39,_tmp39,_tmp39 + 9},274,1},{{_tmp3A,_tmp3A,_tmp3A + 7},291,1},{{_tmp3B,_tmp3B,_tmp3B + 8},306,0},{{_tmp3C,_tmp3C,_tmp3C + 5},313,0},{{_tmp3D,_tmp3D,_tmp3D + 6},265,1},{{_tmp3E,_tmp3E,_tmp3E + 7},270,1},{{_tmp3F,_tmp3F,_tmp3F + 11},270,1},{{_tmp40,_tmp40,_tmp40 + 7},280,1},{{_tmp41,_tmp41,_tmp41 + 7},260,1},{{_tmp42,_tmp42,_tmp42 + 7},275,1},{{_tmp43,_tmp43,_tmp43 + 7},284,1},{{_tmp44,_tmp44,_tmp44 + 9},322,0},{{_tmp45,_tmp45,_tmp45 + 6},311,0},{{_tmp46,_tmp46,_tmp46 + 6},295,0},{{_tmp47,_tmp47,_tmp47 + 4},296,0},{{_tmp48,_tmp48,_tmp48 + 8},262,1},{{_tmp49,_tmp49,_tmp49 + 6},276,1},{{_tmp4A,_tmp4A,_tmp4A + 9},271,1},{{_tmp4B,_tmp4B,_tmp4B + 13},271,1},{{_tmp4C,_tmp4C,_tmp4C + 6},302,0},{{_tmp4D,_tmp4D,_tmp4D + 8},320,0},{{_tmp4E,_tmp4E,_tmp4E + 10},321,0},{{_tmp4F,_tmp4F,_tmp4F + 5},263,1},{{_tmp50,_tmp50,_tmp50 + 9},273,1},{{_tmp51,_tmp51,_tmp51 + 13},273,1},{{_tmp52,_tmp52,_tmp52 + 6},285,1}};
# 219
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned int i=0;for(0;i < 70;++ i){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)
++ sum;}}
return sum;}
# 228
static int Cyc_Lex_trie_char(int c){
# 230
if(c >= 95)return c - 59;else{
if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len);static void _tmp186(unsigned int*_tmp185,unsigned int*_tmp184,struct Cyc_Lex_Trie***_tmp183){for(*_tmp185=0;*_tmp185 < *_tmp184;(*_tmp185)++){(*_tmp183)[*_tmp185]=(struct Cyc_Lex_Trie*)0;}}
# 235
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){
# 237
int i=offset;
int last=(offset + len)- 1;
while(i <= last){
union Cyc_Lex_TrieChildren _tmp53=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _tmp54=_tmp53;struct Cyc_Lex_Trie**_tmp55;int _tmp56;struct Cyc_Lex_Trie*_tmp57;_LLA: if((_tmp54.Many).tag != 3)goto _LLC;_tmp55=(struct Cyc_Lex_Trie**)(_tmp54.Many).val;_LLB: {
# 243
int ch=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
if(_tmp55[_check_known_subscript_notnull(64,ch)]== 0){
struct Cyc_Lex_Trie*_tmp181;_tmp55[_check_known_subscript_notnull(64,ch)]=((_tmp181=_region_malloc(r,sizeof(*_tmp181)),((_tmp181->children=Cyc_Lex_Zero(),((_tmp181->shared_str=0,_tmp181))))));}
t=_tmp55[_check_known_subscript_notnull(64,ch)];
++ i;
goto _LL9;}_LLC: if((_tmp54.One).tag != 2)goto _LLE;_tmp56=((struct _tuple19)(_tmp54.One).val).f1;_tmp57=((struct _tuple19)(_tmp54.One).val).f2;_LLD:
# 251
 if(_tmp56 == *((const char*)_check_dyneither_subscript(buff,sizeof(char),i)))
t=_tmp57;else{
# 254
unsigned int _tmp185;unsigned int _tmp184;struct Cyc_Lex_Trie**_tmp183;unsigned int _tmp182;struct Cyc_Lex_Trie**_tmp59=(_tmp182=64,((_tmp183=(struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp182)),((((_tmp184=_tmp182,_tmp186(& _tmp185,& _tmp184,& _tmp183))),_tmp183)))));
_tmp59[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp56))]=_tmp57;{
int _tmp5A=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
{struct Cyc_Lex_Trie*_tmp187;_tmp59[_tmp5A]=((_tmp187=_region_malloc(r,sizeof(*_tmp187)),((_tmp187->children=Cyc_Lex_Zero(),((_tmp187->shared_str=0,_tmp187))))));}
t->children=Cyc_Lex_Many(_tmp59);
t=_tmp59[_tmp5A];};}
# 261
++ i;
goto _LL9;_LLE: if((_tmp54.Zero).tag != 1)goto _LL9;_LLF:
# 265
 while(i <= last){
struct Cyc_Lex_Trie*_tmp188;struct Cyc_Lex_Trie*_tmp5F=(_tmp188=_region_malloc(r,sizeof(*_tmp188)),((_tmp188->children=Cyc_Lex_Zero(),((_tmp188->shared_str=0,_tmp188)))));
t->children=Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),_tmp5F);
t=_tmp5F;}
# 270
return t;_LL9:;}
# 273
return t;}struct _tuple21{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;struct _dyneither_ptr f3;int f4;int f5;};
# 276
static int Cyc_Lex_str_index_body(struct _RegionHandle*d,struct _tuple21*env){
# 279
struct _tuple21 _tmp61=*env;struct Cyc_Lex_Trie*_tmp63;struct Cyc_Xarray_Xarray*_tmp64;struct _dyneither_ptr _tmp65;int _tmp66;int _tmp67;struct _tuple21 _tmp62=_tmp61;_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;_tmp65=_tmp62.f3;_tmp66=_tmp62.f4;_tmp67=_tmp62.f5;{
struct Cyc_Lex_Trie*_tmp68=Cyc_Lex_trie_lookup(d,_tmp63,_tmp65,_tmp66,_tmp67);
# 282
if(_tmp68->shared_str == 0){
struct _dyneither_ptr _tmp69=Cyc_Core_new_string((unsigned int)(_tmp67 + 1));
Cyc_zstrncpy(_dyneither_ptr_decrease_size(_tmp69,sizeof(char),1),(struct _dyneither_ptr)_dyneither_ptr_plus(_tmp65,sizeof(char),_tmp66),(unsigned long)_tmp67);{
struct _dyneither_ptr*_tmp189;int ans=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp64,((_tmp189=_cycalloc(sizeof(*_tmp189)),((_tmp189[0]=(struct _dyneither_ptr)_tmp69,_tmp189)))));
_tmp68->shared_str=ans;};}
# 288
return _tmp68->shared_str;};}
# 291
static int Cyc_Lex_str_index(struct _dyneither_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols*idt=0;
_swap_word(& idt,& Cyc_Lex_ids_trie);{
struct Cyc_Lex_DynTrieSymbols _tmp6B=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Core_DynamicRegion*_tmp6D;struct Cyc_Lex_Trie*_tmp6E;struct Cyc_Xarray_Xarray*_tmp6F;struct Cyc_Lex_DynTrieSymbols _tmp6C=_tmp6B;_tmp6D=_tmp6C.dyn;_tmp6E=_tmp6C.t;_tmp6F=_tmp6C.symbols;{
struct _tuple21 _tmp18A;struct _tuple21 _tmp70=(_tmp18A.f1=_tmp6E,((_tmp18A.f2=_tmp6F,((_tmp18A.f3=buff,((_tmp18A.f4=offset,((_tmp18A.f5=len,_tmp18A)))))))));
int _tmp71=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple21*arg,int(*body)(struct _RegionHandle*h,struct _tuple21*arg)))Cyc_Core_open_region)(_tmp6D,& _tmp70,Cyc_Lex_str_index_body);
{struct Cyc_Lex_DynTrieSymbols _tmp18B;*idt=((_tmp18B.dyn=(struct Cyc_Core_DynamicRegion*)_tmp6D,((_tmp18B.t=(struct Cyc_Lex_Trie*)_tmp6E,((_tmp18B.symbols=(struct Cyc_Xarray_Xarray*)_tmp6F,_tmp18B))))));}
_swap_word(& idt,& Cyc_Lex_ids_trie);
return _tmp71;};};}
# 302
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}struct _tuple22{struct Cyc_Lex_Trie*f1;struct _dyneither_ptr f2;};
# 308
static int Cyc_Lex_insert_typedef_body(struct _RegionHandle*h,struct _tuple22*arg){
# 310
struct _tuple22 _tmp74=*arg;struct Cyc_Lex_Trie*_tmp76;struct _dyneither_ptr _tmp77;struct _tuple22 _tmp75=_tmp74;_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;{
struct Cyc_Lex_Trie*_tmp78=Cyc_Lex_trie_lookup(h,_tmp76,_tmp77,0,(int)(_get_dyneither_size(_tmp77,sizeof(char))- 1));
((struct Cyc_Lex_Trie*)_check_null(_tmp78))->shared_str=1;
return 0;};}
# 317
static void Cyc_Lex_insert_typedef(struct _dyneither_ptr*sptr){
struct _dyneither_ptr _tmp79=*sptr;
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmp7A=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmp7C;struct Cyc_Lex_Trie*_tmp7D;struct Cyc_Lex_DynTrie _tmp7B=_tmp7A;_tmp7C=_tmp7B.dyn;_tmp7D=_tmp7B.t;{
struct _tuple22 _tmp18C;struct _tuple22 _tmp7E=(_tmp18C.f1=_tmp7D,((_tmp18C.f2=_tmp79,_tmp18C)));
((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple22*arg,int(*body)(struct _RegionHandle*h,struct _tuple22*arg)))Cyc_Core_open_region)(_tmp7C,& _tmp7E,Cyc_Lex_insert_typedef_body);
{struct Cyc_Lex_DynTrie _tmp18D;*tdefs=((_tmp18D.dyn=(struct Cyc_Core_DynamicRegion*)_tmp7C,((_tmp18D.t=(struct Cyc_Lex_Trie*)_tmp7D,_tmp18D))));}
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);
return;};};}struct _tuple23{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;int f3;};
# 329
static struct _dyneither_ptr*Cyc_Lex_get_symbol_body(struct _RegionHandle*dyn,struct _tuple23*env){
struct _tuple23 _tmp81=*env;struct Cyc_Lex_Trie*_tmp83;struct Cyc_Xarray_Xarray*_tmp84;int _tmp85;struct _tuple23 _tmp82=_tmp81;_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;_tmp85=_tmp82.f3;
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp84,_tmp85);}
# 334
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols*idt=0;
_swap_word(& idt,& Cyc_Lex_ids_trie);{
struct Cyc_Lex_DynTrieSymbols _tmp86=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Core_DynamicRegion*_tmp88;struct Cyc_Lex_Trie*_tmp89;struct Cyc_Xarray_Xarray*_tmp8A;struct Cyc_Lex_DynTrieSymbols _tmp87=_tmp86;_tmp88=_tmp87.dyn;_tmp89=_tmp87.t;_tmp8A=_tmp87.symbols;{
struct _tuple23 _tmp18E;struct _tuple23 _tmp8B=(_tmp18E.f1=_tmp89,((_tmp18E.f2=_tmp8A,((_tmp18E.f3=symbol_num,_tmp18E)))));
struct _dyneither_ptr*_tmp8C=((struct _dyneither_ptr*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple23*arg,struct _dyneither_ptr*(*body)(struct _RegionHandle*h,struct _tuple23*arg)))Cyc_Core_open_region)(_tmp88,& _tmp8B,Cyc_Lex_get_symbol_body);
{struct Cyc_Lex_DynTrieSymbols _tmp18F;*idt=((_tmp18F.dyn=(struct Cyc_Core_DynamicRegion*)_tmp88,((_tmp18F.t=(struct Cyc_Lex_Trie*)_tmp89,((_tmp18F.symbols=(struct Cyc_Xarray_Xarray*)_tmp8A,_tmp18F))))));}
_swap_word(& idt,& Cyc_Lex_ids_trie);
return _tmp8C;};};}
# 353 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if('0' <= c  && c <= '9')return c - '0';else{
if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{
if('A' <= c  && c <= 'F')return(10 + c)- 'A';else{
struct Cyc_Core_Invalid_argument_exn_struct _tmp195;const char*_tmp194;struct Cyc_Core_Invalid_argument_exn_struct*_tmp193;(int)_throw((void*)((_tmp193=_cycalloc(sizeof(*_tmp193)),((_tmp193[0]=((_tmp195.tag=Cyc_Core_Invalid_argument,((_tmp195.f1=((_tmp194="string to integer conversion",_tag_dyneither(_tmp194,sizeof(char),29))),_tmp195)))),_tmp193)))));}}}}
# 361
static union Cyc_Absyn_Cnst Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
enum Cyc_Absyn_Sign sn=Cyc_Absyn_None;
start +=lbuf->lex_start_pos;{
struct _dyneither_ptr buff=_dyneither_ptr_plus(lbuf->lex_buffer,sizeof(char),start);
int end2=lbuf->lex_curr_pos - end;
int len=end2 - start;
enum Cyc_Absyn_Size_of size=Cyc_Absyn_Int_sz;
int declared_size=0;
union Cyc_Absyn_Cnst res;
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'l'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'L')){
# 372
len -=1;
declared_size=1;
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'l'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'L')){
len -=1;
size=Cyc_Absyn_LongLong_sz;}}
# 379
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'u'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'U')){
len -=1;
sn=Cyc_Absyn_Unsigned;}
# 383
if(sn == Cyc_Absyn_Unsigned){
unsigned long long n=(unsigned long long)0;
{int i=0;for(0;i < len;++ i){
n=n * base + (unsigned long long)Cyc_Lex_int_of_char(*((char*)_check_dyneither_subscript(buff,sizeof(char),i)));}}
if(n > (unsigned long long)-1){
if(declared_size  && size == Cyc_Absyn_Int_sz){
const char*_tmp196;Cyc_Lex_err(((_tmp196="integer constant too large",_tag_dyneither(_tmp196,sizeof(char),27))),lbuf);}
size=Cyc_Absyn_LongLong_sz;}
# 392
if(size == Cyc_Absyn_Int_sz){
struct _tuple5 _tmp199;union Cyc_Absyn_Cnst _tmp198;res=(((_tmp198.Int_c).val=((_tmp199.f1=sn,((_tmp199.f2=(int)((unsigned int)n),_tmp199)))),(((_tmp198.Int_c).tag=5,_tmp198))));}else{
# 395
struct _tuple6 _tmp19C;union Cyc_Absyn_Cnst _tmp19B;res=(((_tmp19B.LongLong_c).val=((_tmp19C.f1=sn,((_tmp19C.f2=(long long)n,_tmp19C)))),(((_tmp19B.LongLong_c).tag=6,_tmp19B))));}}else{
# 398
long long n=(long long)0;
{int i=0;for(0;i < len;++ i){
n=n * base + (long long)Cyc_Lex_int_of_char(*((char*)_check_dyneither_subscript(buff,sizeof(char),i)));}}{
unsigned long long x=(unsigned long long)n >> 32;
if(x != -1  && x != 0){
if(declared_size  && size == Cyc_Absyn_Int_sz){
const char*_tmp19D;Cyc_Lex_err(((_tmp19D="integer constant too large",_tag_dyneither(_tmp19D,sizeof(char),27))),lbuf);}
size=Cyc_Absyn_LongLong_sz;}
# 407
if(size == Cyc_Absyn_Int_sz){
struct _tuple5 _tmp1A0;union Cyc_Absyn_Cnst _tmp19F;res=(((_tmp19F.Int_c).val=((_tmp1A0.f1=sn,((_tmp1A0.f2=(int)n,_tmp1A0)))),(((_tmp19F.Int_c).tag=5,_tmp19F))));}else{
# 410
struct _tuple6 _tmp1A3;union Cyc_Absyn_Cnst _tmp1A2;res=(((_tmp1A2.LongLong_c).val=((_tmp1A3.f1=sn,((_tmp1A3.f2=n,_tmp1A3)))),(((_tmp1A2.LongLong_c).tag=6,_tmp1A2))));}};}
# 413
return res;};}
# 418
char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dyneither_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11)};
int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(char c);static void _tmp1A9(int*sz,unsigned int*_tmp1A8,unsigned int*_tmp1A7,char**_tmp1A5){for(*_tmp1A8=0;*_tmp1A8 < *_tmp1A7;(*_tmp1A8)++){(*_tmp1A5)[*_tmp1A8]=(char)(
# 426
*_tmp1A8 < *sz?*((char*)_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)*_tmp1A8)):'\000');}}
# 421
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
unsigned int _tmp1A8;unsigned int _tmp1A7;struct _dyneither_ptr _tmp1A6;char*_tmp1A5;unsigned int _tmp1A4;struct _dyneither_ptr str=(_tmp1A4=(unsigned int)newsz,((_tmp1A5=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp1A4 + 1)),((_tmp1A6=_tag_dyneither(_tmp1A5,sizeof(char),_tmp1A4 + 1),((((_tmp1A7=_tmp1A4,((_tmp1A9(& sz,& _tmp1A8,& _tmp1A7,& _tmp1A5),_tmp1A5[_tmp1A7]=(char)0)))),_tmp1A6)))))));
Cyc_Lex_string_buffer=str;};}
# 429
{char _tmp1AC;char _tmp1AB;struct _dyneither_ptr _tmp1AA;(_tmp1AA=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos),((_tmp1AB=*((char*)_check_dyneither_subscript(_tmp1AA,sizeof(char),0)),((_tmp1AC=c,((_get_dyneither_size(_tmp1AA,sizeof(char))== 1  && (_tmp1AB == '\000'  && _tmp1AC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1AA.curr)=_tmp1AC)))))));}
++ Cyc_Lex_string_pos;}
# 432
void Cyc_Lex_store_string(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));}}
# 437
struct _dyneither_ptr Cyc_Lex_get_stored_string(){
struct _dyneither_ptr str=Cyc_substring((struct _dyneither_ptr)Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*namespaces;};
# 450
typedef struct Cyc_Lex_Ldecls*Cyc_Lex_ldecls_t;struct Cyc_Lex_Lvis{struct Cyc_List_List*current_namespace;struct Cyc_List_List*imported_namespaces;};
# 457
typedef struct Cyc_Lex_Lvis*Cyc_Lex_lvis_t;struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};
# 465
typedef struct Cyc_Lex_Lstate*Cyc_Lex_lstate_t;
# 467
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 469
static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis*_tmp1AD;struct Cyc_Lex_Lvis*_tmpA3=(_tmp1AD=_cycalloc(sizeof(*_tmp1AD)),((_tmp1AD->current_namespace=0,((_tmp1AD->imported_namespaces=0,_tmp1AD)))));
# 472
struct Cyc_List_List*_tmp1AE;struct Cyc_List_List*_tmpA4=(_tmp1AE=_cycalloc(sizeof(*_tmp1AE)),((_tmp1AE->hd=_tmpA3,((_tmp1AE->tl=0,_tmp1AE)))));
struct Cyc_Lex_Ldecls*_tmp1AF;struct Cyc_Dict_Dict init_decls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),0,(
# 476
(_tmp1AF=_cycalloc(sizeof(*_tmp1AF)),((_tmp1AF->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp1AF->namespaces=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp1AF)))))));
struct Cyc_Lex_Lstate*_tmp1B2;struct Cyc_Core_Opt*_tmp1B1;Cyc_Lex_lstate=((_tmp1B1=_cycalloc(sizeof(*_tmp1B1)),((_tmp1B1->v=((_tmp1B2=_cycalloc(sizeof(*_tmp1B2)),((_tmp1B2->lstack=_tmpA4,((_tmp1B2->decls=init_decls,_tmp1B2)))))),_tmp1B1))));}
# 481
static struct Cyc_List_List*Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){
# 483
struct _dyneither_ptr*n=(struct _dyneither_ptr*)ns->hd;
# 485
{struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
# 488
struct Cyc_List_List*_tmp1B3;struct Cyc_List_List*x=(_tmp1B3=_cycalloc(sizeof(*_tmp1B3)),((_tmp1B3->hd=lv->current_namespace,((_tmp1B3->tl=lv->imported_namespaces,_tmp1B3)))));
# 490
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
# 493
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,n))
# 495
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)x->hd,ns);}}}
# 498
{const char*_tmp1B9;void*_tmp1B8[1];const char*_tmp1B7;struct Cyc_String_pa_PrintArg_struct _tmp1B6;Cyc_yyerror((struct _dyneither_ptr)((_tmp1B6.tag=0,((_tmp1B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr(ns,((_tmp1B7="::",_tag_dyneither(_tmp1B7,sizeof(char),3))))),((_tmp1B8[0]=& _tmp1B6,Cyc_aprintf(((_tmp1B9="undeclared namespace %s",_tag_dyneither(_tmp1B9,sizeof(char),24))),_tag_dyneither(_tmp1B8,sizeof(void*),1)))))))),- 1,- 1);}
return 0;}
# 502
static struct Cyc_Lex_Trie*Cyc_Lex_empty_trie(struct _RegionHandle*d,int dummy){
struct Cyc_Lex_Trie*_tmp1BA;return(_tmp1BA=_region_malloc(d,sizeof(*_tmp1BA)),((_tmp1BA->children=Cyc_Lex_Zero(),((_tmp1BA->shared_str=0,_tmp1BA)))));}
# 506
static void Cyc_Lex_recompute_typedefs(){
# 508
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmpB0=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmpB2;struct Cyc_Lex_Trie*_tmpB3;struct Cyc_Lex_DynTrie _tmpB1=_tmpB0;_tmpB2=_tmpB1.dyn;_tmpB3=_tmpB1.t;
Cyc_Core_free_ukey(_tmpB2);{
struct Cyc_Core_NewDynamicRegion _tmpB4=Cyc_Core_new_ukey();struct Cyc_Core_DynamicRegion*_tmpB6;struct Cyc_Core_NewDynamicRegion _tmpB5=_tmpB4;_tmpB6=_tmpB5.key;{
struct Cyc_Lex_Trie*t2=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpB6,0,Cyc_Lex_empty_trie);
{struct Cyc_Lex_DynTrie _tmp1BB;*tdefs=((_tmp1BB.dyn=(struct Cyc_Core_DynamicRegion*)_tmpB6,((_tmp1BB.t=(struct Cyc_Lex_Trie*)t2,_tmp1BB))));}
_swap_word(& Cyc_Lex_typedefs_trie,& tdefs);{
# 518
struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*x=(_tmp1BC=_cycalloc(sizeof(*_tmp1BC)),((_tmp1BC->hd=lv->current_namespace,((_tmp1BC->tl=lv->imported_namespaces,_tmp1BC)))));
# 522
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}};};};};}
# 530
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);
struct _handler_cons _tmpB9;_push_handler(& _tmpB9);{int _tmpBB=0;if(setjmp(_tmpB9.handler))_tmpBB=1;if(!_tmpBB){
{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);
int _tmpBC=((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);_npop_handler(0);return _tmpBC;}
# 533
;_pop_handler();}else{void*_tmpBA=(void*)_exn_thrown;void*_tmpBE=_tmpBA;void*_tmpC0;_LL11: {struct Cyc_Dict_Absent_exn_struct*_tmpBF=(struct Cyc_Dict_Absent_exn_struct*)_tmpBE;if(_tmpBF->tag != Cyc_Dict_Absent)goto _LL13;}_LL12:
# 536
 return 0;_LL13: _tmpC0=_tmpBE;_LL14:(void)_rethrow(_tmpC0);_LL10:;}};}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Lex_Trie*f2;struct _dyneither_ptr f3;};
# 540
static int Cyc_Lex_is_typedef_body(struct _RegionHandle*d,struct _tuple24*env){
# 543
struct _tuple24 _tmpC1=*env;struct Cyc_List_List*_tmpC3;struct Cyc_Lex_Trie*_tmpC4;struct _dyneither_ptr _tmpC5;struct _tuple24 _tmpC2=_tmpC1;_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;{
int len=(int)(_get_dyneither_size(_tmpC5,sizeof(char))- 1);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmpC6=((struct Cyc_Lex_Trie*)_check_null(_tmpC4))->children;union Cyc_Lex_TrieChildren _tmpC7=_tmpC6;int _tmpC8;struct Cyc_Lex_Trie*_tmpC9;struct Cyc_Lex_Trie*_tmpCA;struct Cyc_Lex_Trie**_tmpCB;_LL16: if((_tmpC7.Zero).tag != 1)goto _LL18;_LL17:
 return 0;_LL18: if((_tmpC7.One).tag != 2)goto _LL1A;_tmpC8=((struct _tuple19)(_tmpC7.One).val).f1;_tmpC9=((struct _tuple19)(_tmpC7.One).val).f2;if(!(_tmpC8 != *((const char*)_check_dyneither_subscript(_tmpC5,sizeof(char),i))))goto _LL1A;_LL19:
 return 0;_LL1A: if((_tmpC7.One).tag != 2)goto _LL1C;_tmpCA=((struct _tuple19)(_tmpC7.One).val).f2;_LL1B:
 _tmpC4=_tmpCA;goto _LL15;_LL1C: if((_tmpC7.Many).tag != 3)goto _LL15;_tmpCB=(struct Cyc_Lex_Trie**)(_tmpC7.Many).val;_LL1D: {
# 551
struct Cyc_Lex_Trie*_tmpCC=_tmpCB[_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(_tmpC5,sizeof(char),i))))];
if(_tmpCC == 0)
return 0;
_tmpC4=_tmpCC;
goto _LL15;}_LL15:;}}
# 557
return((struct Cyc_Lex_Trie*)_check_null(_tmpC4))->shared_str;};}
# 559
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 564
struct _dyneither_ptr _tmpCD=*v;
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmpCE=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmpD0;struct Cyc_Lex_Trie*_tmpD1;struct Cyc_Lex_DynTrie _tmpCF=_tmpCE;_tmpD0=_tmpCF.dyn;_tmpD1=_tmpCF.t;{
struct _tuple24 _tmp1BD;struct _tuple24 _tmpD2=(_tmp1BD.f1=ns,((_tmp1BD.f2=_tmpD1,((_tmp1BD.f3=_tmpCD,_tmp1BD)))));
int _tmpD3=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple24*arg,int(*body)(struct _RegionHandle*h,struct _tuple24*arg)))Cyc_Core_open_region)(_tmpD0,& _tmpD2,Cyc_Lex_is_typedef_body);
{struct Cyc_Lex_DynTrie _tmp1BE;*tdefs=((_tmp1BE.dyn=(struct Cyc_Core_DynamicRegion*)_tmpD0,((_tmp1BE.t=(struct Cyc_Lex_Trie*)_tmpD1,_tmp1BE))));}
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);
return _tmpD3;};};};}
# 575
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,((_tmp1BF=_cycalloc(sizeof(*_tmp1BF)),((_tmp1BF->hd=s,((_tmp1BF->tl=0,_tmp1BF)))))));
# 579
{struct Cyc_Lex_Lvis*_tmp1C2;struct Cyc_List_List*_tmp1C1;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(
(_tmp1C1=_cycalloc(sizeof(*_tmp1C1)),((_tmp1C1->hd=((_tmp1C2=_cycalloc(sizeof(*_tmp1C2)),((_tmp1C2->current_namespace=new_ns,((_tmp1C2->imported_namespaces=0,_tmp1C2)))))),((_tmp1C1->tl=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack,_tmp1C1))))));}{
# 585
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns);
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,s)){
{struct Cyc_Lex_Ldecls*_tmp1C3;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns,(
# 589
(_tmp1C3=_cycalloc(sizeof(*_tmp1C3)),((_tmp1C3->typedefs=ld->typedefs,((_tmp1C3->namespaces=
# 591
((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(ld->namespaces,s),_tmp1C3)))))));}{
# 593
struct Cyc_Lex_Ldecls*_tmp1C4;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns,(
# 595
(_tmp1C4=_cycalloc(sizeof(*_tmp1C4)),((_tmp1C4->typedefs=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp1C4->namespaces=
# 598
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp1C4)))))));};}
# 604
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns))->typedefs);};}
# 606
void Cyc_Lex_leave_namespace(){
# 608
((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}
# 611
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmpDB=(*q).f1;union Cyc_Absyn_Nmspace _tmpDC=_tmpDB;struct Cyc_List_List*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;_LL1F: if((_tmpDC.Loc_n).tag != 4)goto _LL21;_LL20:
# 615
{struct Cyc_List_List*_tmp1C5;ns=((_tmp1C5=_cycalloc(sizeof(*_tmp1C5)),((_tmp1C5->hd=(*q).f2,((_tmp1C5->tl=0,_tmp1C5))))));}
goto _LL1E;_LL21: if((_tmpDC.Rel_n).tag != 1)goto _LL23;_tmpDD=(struct Cyc_List_List*)(_tmpDC.Rel_n).val;_LL22:
 _tmpDE=_tmpDD;goto _LL24;_LL23: if((_tmpDC.C_n).tag != 3)goto _LL25;_tmpDE=(struct Cyc_List_List*)(_tmpDC.C_n).val;_LL24:
 _tmpDF=_tmpDE;goto _LL26;_LL25: if((_tmpDC.Abs_n).tag != 2)goto _LL1E;_tmpDF=(struct Cyc_List_List*)(_tmpDC.Abs_n).val;_LL26:
# 620
{struct Cyc_List_List*_tmp1C6;ns=(struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpDF,((_tmp1C6=_cycalloc(sizeof(*_tmp1C6)),((_tmp1C6->hd=(*q).f2,((_tmp1C6->tl=0,_tmp1C6))))))));}
goto _LL1E;_LL1E:;}{
# 624
struct Cyc_List_List*_tmpE2=Cyc_Lex_get_absolute_namespace(ns);
struct Cyc_List_List*_tmpE3=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
# 627
{struct Cyc_List_List*_tmp1C7;((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((_tmp1C7=_cycalloc(sizeof(*_tmp1C7)),((_tmp1C7->hd=_tmpE2,((_tmp1C7->tl=_tmpE3,_tmp1C7))))));}
# 631
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,_tmpE2))->typedefs);};}
# 633
void Cyc_Lex_leave_using(){
struct Cyc_List_List*_tmpE5=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((struct Cyc_List_List*)_check_null(_tmpE5))->tl;
# 638
Cyc_Lex_recompute_typedefs();}
# 641
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 643
struct Cyc_List_List*_tmpE6=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpE7=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls;
struct Cyc_Lex_Ldecls*_tmpE8=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpE7,_tmpE6);
struct Cyc_Lex_Ldecls*_tmp1C8;struct Cyc_Lex_Ldecls*_tmpE9=(_tmp1C8=_cycalloc(sizeof(*_tmp1C8)),((_tmp1C8->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpE8->typedefs,(*q).f2),((_tmp1C8->namespaces=_tmpE8->namespaces,_tmp1C8)))));
# 648
((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpE7,_tmpE6,_tmpE9);
# 652
Cyc_Lex_insert_typedef((*q).f2);}
# 655
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 659
if(symbol_num <= Cyc_Lex_num_kws){
# 661
if(!Cyc_Lex_in_extern_c  || (*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).is_c_keyword){
short res=(short)(*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).token_index;
return res;}}{
# 667
struct _dyneither_ptr*_tmpEB=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*_tmpEB;
# 671
if(Cyc_Lex_is_typedef(0,_tmpEB))
return 368;
# 675
return 360;};}
# 678
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _dyneither_ptr s=lbuf->lex_buffer;
# 683
struct _dyneither_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 686
while(i < end){
int start=i;
for(0;i < end  && *((char*)_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){
;}
if(start == i){
struct Cyc_Core_Impossible_exn_struct _tmp1CE;const char*_tmp1CD;struct Cyc_Core_Impossible_exn_struct*_tmp1CC;(int)_throw((void*)((_tmp1CC=_cycalloc(sizeof(*_tmp1CC)),((_tmp1CC[0]=((_tmp1CE.tag=Cyc_Core_Impossible,((_tmp1CE.f1=((_tmp1CD="bad namespace",_tag_dyneither(_tmp1CD,sizeof(char),14))),_tmp1CE)))),_tmp1CC)))));}{
int vlen=i - start;
if(v != 0){
struct Cyc_List_List*_tmp1CF;rev_vs=((_tmp1CF=_cycalloc(sizeof(*_tmp1CF)),((_tmp1CF->hd=v,((_tmp1CF->tl=rev_vs,_tmp1CF))))));}
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));
i +=2;};}
# 698
if(v == 0){
struct Cyc_Core_Impossible_exn_struct _tmp1D5;const char*_tmp1D4;struct Cyc_Core_Impossible_exn_struct*_tmp1D3;(int)_throw((void*)((_tmp1D3=_cycalloc(sizeof(*_tmp1D3)),((_tmp1D3[0]=((_tmp1D5.tag=Cyc_Core_Impossible,((_tmp1D5.f1=((_tmp1D4="bad namespace",_tag_dyneither(_tmp1D4,sizeof(char),14))),_tmp1D5)))),_tmp1D3)))));}{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 703
{const char*_tmp1D6;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1D6="Cyc",_tag_dyneither(_tmp1D6,sizeof(char),4))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1D7;Cyc_Lex_token_qvar=((_tmp1D7=_cycalloc(sizeof(*_tmp1D7)),((_tmp1D7->f1=Cyc_Absyn_Abs_n(vs,0),((_tmp1D7->f2=v,_tmp1D7))))));};}else{
# 707
const char*_tmp1D8;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1D8="C",_tag_dyneither(_tmp1D8,sizeof(char),2))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1D9;Cyc_Lex_token_qvar=((_tmp1D9=_cycalloc(sizeof(*_tmp1D9)),((_tmp1D9->f1=Cyc_Absyn_Abs_n(vs,1),((_tmp1D9->f2=v,_tmp1D9))))));};}else{
# 711
struct _tuple0*_tmp1DA;Cyc_Lex_token_qvar=((_tmp1DA=_cycalloc(sizeof(*_tmp1DA)),((_tmp1DA->f1=Cyc_Absyn_Rel_n(vs),((_tmp1DA->f2=v,_tmp1DA))))));}}}
if(Cyc_Lex_is_typedef(vs,v))
return 370;
return 369;};}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned int starting_line;struct _dyneither_ptr filename;struct _dyneither_ptr linenumpos;unsigned int linenumpos_offset;};
# 727
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_t;
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_opt_t;
# 730
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 732
static int Cyc_Lex_linenumber=1;
# 735
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _dyneither_ptr filename,unsigned int starting_line,struct Cyc_Lex_PosInfo*next){
# 739
unsigned int*_tmp1DC;unsigned int _tmp1DB;struct _dyneither_ptr linenumpos=(_tmp1DB=10,((_tmp1DC=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1DB),_tag_dyneither(_tmp1DC,sizeof(unsigned int),_tmp1DB))));
*((unsigned int*)_check_dyneither_subscript(linenumpos,sizeof(unsigned int),0))=(unsigned int)Cyc_yylloc.first_line;{
struct Cyc_Lex_PosInfo*_tmp1DD;return(_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD)),((_tmp1DD->next=next,((_tmp1DD->starting_line=starting_line,((_tmp1DD->filename=(struct _dyneither_ptr)filename,((_tmp1DD->linenumpos=linenumpos,((_tmp1DD->linenumpos_offset=1,_tmp1DD)))))))))));};}
# 748
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0){struct Cyc_Core_Impossible_exn_struct _tmp1E3;const char*_tmp1E2;struct Cyc_Core_Impossible_exn_struct*_tmp1E1;(int)_throw((void*)((_tmp1E1=_cycalloc(sizeof(*_tmp1E1)),((_tmp1E1[0]=((_tmp1E3.tag=Cyc_Core_Impossible,((_tmp1E3.f1=((_tmp1E2="empty position info!",_tag_dyneither(_tmp1E2,sizeof(char),21))),_tmp1E3)))),_tmp1E1)))));}{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _dyneither_ptr linenums=_tag_dyneither(0,0,0);
_swap_dyneither(& p->linenumpos,& linenums);{
unsigned int offset=p->linenumpos_offset;
unsigned int n=_get_dyneither_size(linenums,sizeof(unsigned int));
# 756
if(offset >= n){
# 758
unsigned int*_tmp1E5;unsigned int _tmp1E4;struct _dyneither_ptr newlinenums=(_tmp1E4=n * 2,((_tmp1E5=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1E4),_tag_dyneither(_tmp1E5,sizeof(unsigned int),_tmp1E4))));
{unsigned int i=0;for(0;i < n;++ i){
*((unsigned int*)_check_dyneither_subscript(newlinenums,sizeof(unsigned int),(int)i))=((unsigned int*)linenums.curr)[(int)i];}}
_swap_dyneither(& linenums,& newlinenums);
# 763
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(newlinenums);}
# 766
*((unsigned int*)_check_dyneither_subscript(linenums,sizeof(unsigned int),(int)offset))=(unsigned int)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + 1;
++ Cyc_Lex_linenumber;
_swap_dyneither(& p->linenumpos,& linenums);};};}
# 773
static void Cyc_Lex_process_directive(struct _dyneither_ptr line){
int i;
char buf[100];
const char*_tmp1E6;struct _dyneither_ptr filename=(_tmp1E6="",_tag_dyneither(_tmp1E6,sizeof(char),1));
{const char*_tmp1EB;void*_tmp1EA[2];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1E9;struct Cyc_CharPtr_sa_ScanfArg_struct _tmp1E8;if(((_tmp1E8.tag=7,((_tmp1E8.f1=_tag_dyneither(buf,sizeof(char),100),((_tmp1E9.tag=2,((_tmp1E9.f1=& i,((_tmp1EA[0]=& _tmp1E9,((_tmp1EA[1]=& _tmp1E8,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1EB="# %d \"%s",_tag_dyneither(_tmp1EB,sizeof(char),9))),_tag_dyneither(_tmp1EA,sizeof(void*),2))))))))))))))== 2){
if(Cyc_Lex_compile_for_boot_r){
# 781
int i=(int)(Cyc_strlen(_tag_dyneither(buf,sizeof(char),100))- 1);
int last_slash=-1;
while(i >= 0){
if(buf[_check_known_subscript_notnull(100,i)]== '/'){last_slash=i;break;}
-- i;}{
# 787
const char*_tmp1EF;void*_tmp1EE[1];struct Cyc_String_pa_PrintArg_struct _tmp1ED;filename=(struct _dyneither_ptr)((_tmp1ED.tag=0,((_tmp1ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(_tag_dyneither(buf,sizeof(char),100),sizeof(char),last_slash),sizeof(char),1)),((_tmp1EE[0]=& _tmp1ED,Cyc_aprintf(((_tmp1EF="\"%s",_tag_dyneither(_tmp1EF,sizeof(char),4))),_tag_dyneither(_tmp1EE,sizeof(void*),1))))))));};}else{
# 789
const char*_tmp1F3;void*_tmp1F2[1];struct Cyc_String_pa_PrintArg_struct _tmp1F1;filename=(struct _dyneither_ptr)((_tmp1F1.tag=0,((_tmp1F1.f1=(struct _dyneither_ptr)_tag_dyneither(buf,sizeof(char),100),((_tmp1F2[0]=& _tmp1F1,Cyc_aprintf(((_tmp1F3="\"%s",_tag_dyneither(_tmp1F3,sizeof(char),4))),_tag_dyneither(_tmp1F2,sizeof(void*),1))))))));}
if((Cyc_Lex_linenumber == i  && Cyc_Lex_pos_info != 0) && Cyc_strcmp((struct _dyneither_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _dyneither_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
const char*_tmp1F7;void*_tmp1F6[1];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1F5;if(((_tmp1F5.tag=2,((_tmp1F5.f1=& i,((_tmp1F6[0]=& _tmp1F5,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1F7="# %d",_tag_dyneither(_tmp1F7,sizeof(char),5))),_tag_dyneither(_tmp1F6,sizeof(void*),1))))))))== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 797
++ Cyc_Lex_linenumber;
return;}}}
# 800
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned int)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple25{struct _dyneither_ptr f1;unsigned int f2;};
# 807
struct _tuple25 Cyc_Lex_xlate_pos(unsigned int char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p != 0;p=p->next){
unsigned int first_char_offset=*((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),0));
if(char_offset < first_char_offset  && p->next != 0)continue;{
# 813
unsigned int base=0;
unsigned int size=p->linenumpos_offset;
while(size > 1){
int half=(int)(size / 2);
int mid=(int)(base + half);
if(char_offset > *((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),mid))){
base=base + half;
size=size - half;}else{
# 822
size=(unsigned int)half;}}{
# 825
struct _tuple25 _tmp1F8;return(_tmp1F8.f1=p->filename,((_tmp1F8.f2=p->starting_line + base,_tmp1F8)));};};}}{
# 827
struct _tuple25 _tmp1F9;return(_tmp1F9.f1=_tag_dyneither(0,0,0),((_tmp1F9.f2=0,_tmp1F9)));};}
# 832
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);
# 845
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 847
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);
Cyc_yylloc.first_line=(yyllocptr->first_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf));
Cyc_yylloc.last_line=(yyllocptr->last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf));
switch(ans){case 360: _LL27:
# 852
{union Cyc_YYSTYPE _tmp1FA;*yylval=(((_tmp1FA.String_tok).val=Cyc_Lex_token_string,(((_tmp1FA.String_tok).tag=3,_tmp1FA))));}break;case 368: _LL28:
# 854
{union Cyc_YYSTYPE _tmp1FB;*yylval=(((_tmp1FB.String_tok).val=Cyc_Lex_token_string,(((_tmp1FB.String_tok).tag=3,_tmp1FB))));}break;case 369: _LL29:
# 856
{union Cyc_YYSTYPE _tmp1FC;*yylval=(((_tmp1FC.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1FC.QualId_tok).tag=5,_tmp1FC))));}break;case 370: _LL2A:
# 858
{union Cyc_YYSTYPE _tmp1FD;*yylval=(((_tmp1FD.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1FD.QualId_tok).tag=5,_tmp1FD))));}break;case 367: _LL2B:
# 860
{union Cyc_YYSTYPE _tmp1FE;*yylval=(((_tmp1FE.String_tok).val=Cyc_Lex_token_string,(((_tmp1FE.String_tok).tag=3,_tmp1FE))));}break;case 361: _LL2C:
# 862
{union Cyc_YYSTYPE _tmp1FF;*yylval=(((_tmp1FF.Int_tok).val=Cyc_Lex_token_int,(((_tmp1FF.Int_tok).tag=1,_tmp1FF))));}break;case 364: _LL2D:
# 864
{union Cyc_YYSTYPE _tmp200;*yylval=(((_tmp200.Char_tok).val=Cyc_Lex_token_char,(((_tmp200.Char_tok).tag=2,_tmp200))));}break;case 365: _LL2E:
# 866
{union Cyc_YYSTYPE _tmp201;*yylval=(((_tmp201.String_tok).val=Cyc_Lex_token_string,(((_tmp201.String_tok).tag=3,_tmp201))));}break;case 366: _LL2F:
# 868
{union Cyc_YYSTYPE _tmp202;*yylval=(((_tmp202.String_tok).val=Cyc_Lex_token_string,(((_tmp202.String_tok).tag=3,_tmp202))));}break;case 362: _LL30:
# 870
{union Cyc_YYSTYPE _tmp203;*yylval=(((_tmp203.String_tok).val=Cyc_Lex_token_string,(((_tmp203.String_tok).tag=3,_tmp203))));}break;case 363: _LL31:
# 872
{union Cyc_YYSTYPE _tmp204;*yylval=(((_tmp204.String_tok).val=Cyc_Lex_token_string,(((_tmp204.String_tok).tag=3,_tmp204))));}break;case 372: _LL32:
# 874
{union Cyc_YYSTYPE _tmp205;*yylval=(((_tmp205.Asm_tok).val=Cyc_Lex_token_asm,(((_tmp205.Asm_tok).tag=6,_tmp205))));}break;default: _LL33:
 break;}
# 877
return ans;}
# 884
const int Cyc_Lex_lex_base[321]={0,24,117,119,123,132,134,139,150,157,- 8,- 7,4,- 1,5,- 4,- 5,2,- 6,- 2,6,- 3,7,- 7,163,- 1,28,20,27,40,23,38,40,49,201,36,44,50,68,50,64,63,76,83,84,207,183,10,12,21,- 15,- 16,11,- 1,215,- 17,- 11,- 10,234,- 12,- 9,- 4,- 5,299,354,242,- 13,- 16,- 3,- 2,12,2,- 16,- 17,13,- 1,423,- 18,14,250,496,519,288,- 14,15,- 1,16,574,332,647,722,377,- 27,74,83,72,84,102,86,74,89,85,101,91,98,797,118,117,113,872,121,126,128,947,129,124,114,133,119,139,139,130,139,1022,138,131,145,133,1097,139,141,1172,137,163,144,154,149,144,158,1247,177,177,168,176,1322,169,205,194,198,194,210,200,206,1397,199,210,211,1472,204,1547,206,206,222,214,211,222,230,222,231,1622,217,230,244,231,1697,- 62,380,- 41,17,30,- 44,347,31,332,404,32,315,316,449,398,1774,1817,301,355,302,310,- 7,1887,1976,313,2051,2109,2184,357,- 61,- 25,- 31,2259,336,2334,2409,2467,2542,2617,376,2692,2767,2842,2917,2992,3067,3142,3217,- 26,- 45,- 46,374,- 29,376,- 34,- 28,- 37,380,- 38,3292,3321,590,406,371,372,373,600,3331,3361,1855,1784,3394,3425,3463,409,384,3533,3571,1768,385,408,402,432,425,1770,446,448,442,1771,452,482,- 9,492,- 43,431,- 23,- 42,18,396,3503,- 36,- 19,- 21,- 35,- 20,- 22,1,3611,2,407,1878,410,411,424,425,426,427,428,429,430,3684,3768,- 59,- 53,- 52,- 51,- 50,- 49,- 48,- 47,- 54,- 57,- 58,3641,431,- 55,- 56,- 60,- 30,- 24,525,- 39,19,- 15,- 41,500};
const int Cyc_Lex_lex_backtrk[321]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,7,- 1,- 1,7,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,5,3,4,- 1,- 1,14,- 1,17,- 1,- 1,- 1,12,- 1,- 1,- 1,- 1,- 1,13,12,- 1,- 1,- 1,- 1,1,3,- 1,- 1,15,- 1,18,- 1,1,13,- 1,14,13,- 1,1,- 1,14,17,12,- 1,13,12,- 1,26,26,26,26,26,26,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,- 1,- 1,20,- 1,- 1,- 1,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,24,- 1,- 1,18,- 1,- 1,- 1,- 1,- 1,- 1,- 1,19,- 1,- 1,- 1,- 1,23,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,22,- 1,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,25,- 1,39,- 1,40,61,- 1,61,61,61,61,61,61,61,61,61,8,10,61,61,61,61,- 1,2,2,61,61,61,2,61,- 1,- 1,- 1,2,- 1,2,0,- 1,4,4,- 1,5,3,3,3,3,3,3,1,- 1,- 1,- 1,32,- 1,31,- 1,- 1,- 1,- 1,- 1,12,10,- 1,10,10,10,10,- 1,11,12,- 1,- 1,12,8,9,8,8,- 1,7,7,7,7,7,8,8,9,9,9,9,8,8,8,- 1,8,- 1,- 1,- 1,- 1,41,- 1,12,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,39};
const int Cyc_Lex_lex_default[321]={175,92,50,61,72,61,50,46,- 1,10,0,0,- 1,0,20,0,0,- 1,0,0,20,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,46,- 1,- 1,- 1,0,0,- 1,0,- 1,0,0,0,- 1,0,0,0,0,- 1,- 1,- 1,0,0,0,0,- 1,- 1,0,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,0,315,- 1,- 1,281,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,269,0,0,- 1,- 1,- 1,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,315,0,- 1,0,0,- 1};
const int Cyc_Lex_lex_trans[4025]={0,0,0,0,0,0,0,0,0,176,177,176,176,178,23,21,21,21,22,22,18,67,19,55,19,19,67,319,271,316,0,0,176,179,180,181,25,182,183,184,312,311,185,186,19,187,188,189,190,191,191,191,191,191,191,191,191,191,192,19,193,194,195,25,196,197,197,197,197,197,197,197,197,197,197,197,198,197,197,197,197,197,197,197,197,197,197,197,197,197,197,318,314,280,199,200,201,202,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,35,203,93,94,51,68,69,86,28,84,73,95,29,74,30,96,31,97,68,69,32,51,70,98,52,33,18,34,68,47,85,36,37,53,75,38,24,24,24,24,24,68,39,40,11,53,41,12,42,24,24,24,24,24,43,44,45,158,48,24,132,160,114,49,99,100,133,25,13,14,- 1,161,24,- 1,15,16,106,101,102,103,25,17,104,107,105,71,87,34,34,34,34,34,76,45,45,45,45,45,110,108,109,111,- 1,54,112,113,129,- 1,124,115,34,118,119,120,121,122,45,123,19,116,125,117,126,127,21,128,56,130,131,140,145,57,136,204,137,26,146,138,- 1,- 1,58,58,58,58,58,58,58,58,134,135,139,141,142,143,144,59,154,- 1,27,65,65,65,65,65,65,65,65,66,66,66,66,66,66,66,66,82,82,82,82,82,82,82,82,147,60,148,149,150,151,19,21,152,153,155,61,156,157,159,170,162,163,164,62,165,166,167,18,168,23,169,10,171,63,83,83,83,83,83,83,83,83,172,173,174,64,64,64,64,64,64,64,64,64,64,316,77,231,317,276,232,83,64,64,64,64,64,64,313,55,226,55,223,- 1,279,277,278,77,91,91,91,91,91,91,91,91,- 1,320,55,320,320,92,211,21,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,320,18,- 1,228,67,229,205,64,64,64,64,64,64,66,66,66,66,66,66,66,66,19,211,227,78,230,233,- 1,268,271,275,- 1,272,269,310,239,56,306,305,64,64,64,64,64,64,59,56,270,257,255,56,304,303,302,301,300,299,298,307,79,79,79,79,79,79,79,79,239,56,206,240,0,10,258,66,10,0,0,56,56,257,255,60,273,282,274,274,274,274,274,274,274,274,274,274,0,60,320,60,320,320,0,240,57,10,258,0,10,21,61,261,56,57,62,60,57,265,0,0,0,320,18,0,316,0,23,317,10,60,60,60,80,81,81,81,81,81,81,81,81,81,81,261,0,57,0,266,57,265,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,266,0,0,0,0,0,0,81,81,81,81,81,81,266,0,0,81,81,81,81,81,81,0,0,0,0,- 1,0,0,0,0,56,266,0,0,0,57,0,0,81,81,81,81,81,81,88,88,88,88,88,88,88,88,0,0,0,241,0,241,0,59,242,242,242,242,242,242,242,242,242,242,242,242,242,242,242,242,242,242,242,242,0,0,- 1,0,0,0,0,0,60,0,0,0,0,19,21,0,0,0,61,0,0,0,0,0,0,0,62,0,0,- 1,18,0,23,0,10,0,89,90,90,90,90,90,90,90,90,90,90,0,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,- 1,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,19,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,0,0,0,0,19,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,10,10,10,10,10,10,10,10,10,10,0,0,0,0,0,0,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0,10,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,25,25,25,25,25,25,25,25,25,25,0,0,0,0,0,0,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,0,0,0,0,25,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,62,62,62,62,62,62,62,62,62,62,0,0,0,0,0,0,0,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,0,0,0,0,62,0,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,59,59,59,59,59,59,59,59,59,59,0,0,0,0,0,0,0,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,0,0,0,0,59,0,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,59,18,18,18,18,18,18,18,18,18,18,0,0,0,0,0,0,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,0,0,0,0,18,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,23,23,23,23,23,23,23,23,23,23,0,0,0,0,0,0,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,0,0,0,0,23,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,56,56,56,56,56,56,56,56,56,56,0,0,0,0,0,0,0,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,0,0,0,0,56,0,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,21,21,21,21,21,21,21,21,21,21,0,0,0,0,0,0,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,0,0,0,0,21,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,57,57,57,57,57,57,57,57,57,57,0,0,0,0,0,0,0,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,0,0,0,0,57,0,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,57,60,60,60,60,60,60,60,60,60,60,0,0,0,0,0,0,0,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,0,0,0,0,60,0,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,61,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,66,66,66,66,66,66,66,66,66,66,0,0,0,0,0,0,0,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,0,0,0,0,66,0,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,234,0,247,247,247,247,247,247,247,247,248,248,246,246,246,246,246,246,246,246,246,246,0,236,256,0,262,267,0,0,249,0,0,10,0,57,266,0,0,250,0,0,251,234,0,235,235,235,235,235,235,235,235,235,235,236,256,0,262,267,0,0,249,0,0,10,236,57,266,0,0,250,0,237,251,0,0,0,245,0,245,0,238,246,246,246,246,246,246,246,246,246,246,0,0,0,0,307,236,0,0,0,0,0,0,237,308,308,308,308,308,308,308,308,238,207,207,207,207,207,207,207,207,207,207,208,0,0,0,0,0,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,0,0,0,0,207,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,224,0,0,0,0,225,0,0,0,0,0,0,0,0,207,207,207,207,207,207,207,207,207,207,208,0,0,0,0,0,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,0,0,0,0,207,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,217,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,0,0,0,0,215,0,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,207,207,207,207,207,207,207,207,207,207,208,0,0,0,0,0,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,0,0,0,0,207,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,209,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,208,0,0,0,0,0,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,0,0,0,0,207,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,208,0,0,0,0,0,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,0,0,0,0,207,0,207,207,207,207,207,207,207,207,207,207,207,207,210,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,208,0,0,0,0,0,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,0,0,0,0,207,0,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,0,0,0,0,213,0,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,214,0,0,0,0,0,0,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,0,0,0,0,212,0,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,213,213,213,213,213,213,213,213,213,213,0,0,0,0,0,0,0,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,0,0,0,0,213,0,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,215,215,215,215,215,215,215,215,215,215,0,0,0,0,0,0,0,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,0,0,0,0,215,0,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,216,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,216,0,218,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,216,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,219,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,216,0,216,216,216,216,216,216,216,216,216,216,216,216,220,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,221,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,222,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,0,216,0,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,243,243,243,243,243,243,243,243,243,243,0,0,0,0,0,0,0,0,0,0,0,244,66,0,0,0,0,234,66,235,235,235,235,235,235,235,235,235,235,242,242,242,242,242,242,242,242,242,242,0,236,0,0,244,66,0,0,237,0,0,66,59,0,0,0,0,238,59,0,243,243,243,243,243,243,243,243,243,243,0,0,0,236,0,0,0,0,0,0,237,244,66,0,59,0,0,0,66,238,59,0,0,246,246,246,246,246,246,246,246,246,246,0,0,0,0,0,0,0,0,0,0,244,66,66,0,0,0,0,66,66,234,0,247,247,247,247,247,247,247,247,248,248,0,0,0,0,0,0,0,0,0,0,0,236,0,66,0,0,0,0,263,66,0,0,0,0,0,0,234,264,248,248,248,248,248,248,248,248,248,248,0,0,0,0,0,236,0,0,0,0,0,236,263,0,0,0,0,0,259,0,0,264,0,0,0,0,0,260,0,0,274,274,274,274,274,274,274,274,274,274,0,0,0,236,0,0,0,0,0,0,259,244,66,0,0,0,0,0,66,260,252,252,252,252,252,252,252,252,252,252,0,0,0,0,0,0,0,252,252,252,252,252,252,244,66,0,0,0,0,0,66,0,0,0,0,0,0,0,252,252,252,252,252,252,252,252,252,252,0,252,252,252,252,252,252,252,252,252,252,252,252,0,0,0,283,0,253,0,0,284,0,0,0,0,0,254,0,0,285,285,285,285,285,285,285,285,0,252,252,252,252,252,252,286,0,0,0,0,253,307,0,0,0,0,0,0,0,254,309,309,309,309,309,309,309,309,0,0,0,0,0,0,287,0,0,0,0,288,289,0,0,0,290,0,0,0,0,0,0,0,291,0,0,0,292,0,293,0,294,0,295,296,296,296,296,296,296,296,296,296,296,0,0,0,0,0,0,0,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,0,0,0,0,0,0,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,297,0,0,0,0,0,0,0,0,296,296,296,296,296,296,296,296,296,296,0,0,0,0,0,0,0,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,0,0,0,0,0,0,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[4025]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,12,14,20,22,14,20,47,52,70,74,78,84,86,178,272,317,- 1,- 1,0,0,0,0,71,0,0,0,281,283,0,0,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,0,0,0,49,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,179,182,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,0,1,1,2,3,3,2,27,3,4,1,28,4,29,1,30,1,5,5,31,6,5,1,6,32,7,33,3,7,3,35,36,2,4,37,8,8,8,8,8,5,38,39,9,6,40,9,41,24,24,24,24,24,42,43,44,94,7,8,95,93,96,7,98,99,95,8,9,9,46,93,24,46,9,9,97,100,101,102,24,9,103,97,104,5,2,34,34,34,34,34,4,45,45,45,45,45,106,107,108,110,46,6,111,112,115,46,116,114,34,117,118,119,120,121,45,122,34,114,124,114,125,126,45,127,54,129,130,134,132,54,135,0,136,24,132,137,14,20,54,54,54,54,54,54,54,54,133,133,138,140,141,142,143,54,145,1,24,58,58,58,58,58,58,58,58,65,65,65,65,65,65,65,65,79,79,79,79,79,79,79,79,146,54,147,148,149,150,54,54,151,152,154,54,155,156,158,160,161,162,163,54,164,165,166,54,167,54,168,54,170,54,82,82,82,82,82,82,82,82,171,172,173,63,63,63,63,63,63,63,63,63,63,181,186,192,181,187,192,194,63,63,63,63,63,63,183,195,195,2,199,3,186,187,187,4,88,88,88,88,88,88,88,88,5,176,6,176,176,183,208,7,63,63,63,63,63,63,64,64,64,64,64,64,64,64,64,64,176,9,184,193,193,193,203,64,64,64,64,64,64,91,91,91,91,91,91,91,91,76,214,226,76,228,232,46,189,269,273,184,269,189,284,238,239,286,287,64,64,64,64,64,64,76,240,189,250,254,76,288,289,290,291,292,293,294,309,76,76,76,76,76,76,76,76,238,239,203,237,- 1,255,249,76,256,- 1,- 1,240,237,250,254,249,188,184,188,188,188,188,188,188,188,188,188,188,- 1,257,320,258,320,320,- 1,237,76,255,249,- 1,256,76,76,260,237,261,76,249,262,264,- 1,- 1,- 1,320,76,- 1,315,- 1,76,315,76,257,76,258,76,80,80,80,80,80,80,80,80,80,80,260,- 1,261,- 1,265,262,264,80,80,80,80,80,80,81,81,81,81,81,81,81,81,81,81,267,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,265,- 1,- 1,80,80,80,80,80,80,- 1,- 1,- 1,- 1,181,- 1,- 1,- 1,- 1,87,267,- 1,- 1,- 1,87,- 1,- 1,81,81,81,81,81,81,87,87,87,87,87,87,87,87,- 1,- 1,- 1,236,- 1,236,- 1,87,236,236,236,236,236,236,236,236,236,236,241,241,241,241,241,241,241,241,241,241,- 1,- 1,184,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,87,87,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,269,87,- 1,87,- 1,87,- 1,87,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,- 1,- 1,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,90,90,90,90,90,90,90,90,- 1,315,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,105,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,128,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,131,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,139,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,144,144,144,144,144,144,144,144,144,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,- 1,- 1,- 1,- 1,144,- 1,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,153,153,153,153,153,153,153,153,153,153,- 1,- 1,- 1,- 1,- 1,- 1,- 1,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,- 1,- 1,- 1,- 1,153,- 1,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,157,157,157,157,157,157,157,157,157,157,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,- 1,- 1,- 1,- 1,157,- 1,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,- 1,- 1,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,159,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,169,169,169,169,169,169,169,169,169,169,- 1,- 1,- 1,- 1,- 1,- 1,- 1,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,- 1,- 1,- 1,- 1,169,- 1,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,174,174,174,174,174,174,174,174,174,174,- 1,- 1,- 1,- 1,- 1,- 1,- 1,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,- 1,- 1,- 1,- 1,174,- 1,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,190,- 1,190,190,190,190,190,190,190,190,190,190,245,245,245,245,245,245,245,245,245,245,- 1,190,253,- 1,259,263,- 1,- 1,190,- 1,- 1,253,- 1,259,263,- 1,- 1,190,- 1,- 1,190,191,- 1,191,191,191,191,191,191,191,191,191,191,190,253,- 1,259,263,- 1,- 1,190,- 1,- 1,253,191,259,263,- 1,- 1,190,- 1,191,190,- 1,- 1,- 1,244,- 1,244,- 1,191,244,244,244,244,244,244,244,244,244,244,- 1,- 1,- 1,- 1,285,191,- 1,- 1,- 1,- 1,- 1,- 1,191,285,285,285,285,285,285,285,285,191,197,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,- 1,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,197,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,198,- 1,- 1,- 1,- 1,198,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,- 1,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,198,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,- 1,- 1,- 1,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,200,- 1,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,201,- 1,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,202,202,202,202,202,202,202,202,202,202,202,- 1,- 1,- 1,- 1,- 1,- 1,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,- 1,- 1,- 1,- 1,202,- 1,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,202,207,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,- 1,- 1,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,207,- 1,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,209,209,209,209,209,209,209,209,209,209,209,- 1,- 1,- 1,- 1,- 1,- 1,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,- 1,- 1,- 1,- 1,209,- 1,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,210,210,210,210,210,210,210,210,210,210,210,- 1,- 1,- 1,- 1,- 1,- 1,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,- 1,- 1,- 1,- 1,210,- 1,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,- 1,- 1,- 1,- 1,211,- 1,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,211,212,212,212,212,212,212,212,212,212,212,212,- 1,- 1,- 1,- 1,- 1,- 1,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,- 1,- 1,- 1,- 1,212,- 1,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,213,213,213,213,213,213,213,213,213,213,- 1,- 1,- 1,- 1,- 1,- 1,- 1,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,- 1,- 1,- 1,- 1,213,- 1,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,215,215,215,215,215,215,215,215,215,215,- 1,- 1,- 1,- 1,- 1,- 1,- 1,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,- 1,- 1,- 1,- 1,215,- 1,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,215,216,216,216,216,216,216,216,216,216,216,- 1,- 1,- 1,- 1,- 1,- 1,- 1,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,- 1,- 1,- 1,- 1,216,- 1,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,216,217,217,217,217,217,217,217,217,217,217,- 1,- 1,- 1,- 1,- 1,- 1,- 1,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,- 1,- 1,- 1,- 1,217,- 1,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,218,218,218,218,218,218,218,218,218,218,- 1,- 1,- 1,- 1,- 1,- 1,- 1,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,- 1,- 1,- 1,- 1,218,- 1,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,219,219,219,219,219,219,219,219,219,219,- 1,- 1,- 1,- 1,- 1,- 1,- 1,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,- 1,- 1,- 1,- 1,219,- 1,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,220,220,220,220,220,220,220,220,220,220,- 1,- 1,- 1,- 1,- 1,- 1,- 1,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,- 1,- 1,- 1,- 1,220,- 1,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,221,221,221,221,221,221,221,221,221,221,- 1,- 1,- 1,- 1,- 1,- 1,- 1,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,- 1,- 1,- 1,- 1,221,- 1,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,- 1,- 1,- 1,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,222,- 1,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,234,234,234,234,234,234,234,234,234,234,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,234,234,- 1,- 1,- 1,- 1,235,234,235,235,235,235,235,235,235,235,235,235,242,242,242,242,242,242,242,242,242,242,- 1,235,- 1,- 1,234,234,- 1,- 1,235,- 1,- 1,234,242,- 1,- 1,- 1,- 1,235,242,- 1,243,243,243,243,243,243,243,243,243,243,- 1,- 1,- 1,235,- 1,- 1,- 1,- 1,- 1,- 1,235,243,243,- 1,242,- 1,- 1,- 1,243,235,242,- 1,- 1,246,246,246,246,246,246,246,246,246,246,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,243,243,246,- 1,- 1,- 1,- 1,243,246,247,- 1,247,247,247,247,247,247,247,247,247,247,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,247,- 1,246,- 1,- 1,- 1,- 1,247,246,- 1,- 1,- 1,- 1,- 1,- 1,248,247,248,248,248,248,248,248,248,248,248,248,- 1,- 1,- 1,- 1,- 1,247,- 1,- 1,- 1,- 1,- 1,248,247,- 1,- 1,- 1,- 1,- 1,248,- 1,- 1,247,- 1,- 1,- 1,- 1,- 1,248,- 1,- 1,274,274,274,274,274,274,274,274,274,274,- 1,- 1,- 1,248,- 1,- 1,- 1,- 1,- 1,- 1,248,274,274,- 1,- 1,- 1,- 1,- 1,274,248,251,251,251,251,251,251,251,251,251,251,- 1,- 1,- 1,- 1,- 1,- 1,- 1,251,251,251,251,251,251,274,274,- 1,- 1,- 1,- 1,- 1,274,- 1,- 1,- 1,- 1,- 1,- 1,- 1,252,252,252,252,252,252,252,252,252,252,- 1,251,251,251,251,251,251,252,252,252,252,252,252,- 1,- 1,- 1,282,- 1,252,- 1,- 1,282,- 1,- 1,- 1,- 1,- 1,252,- 1,- 1,282,282,282,282,282,282,282,282,- 1,252,252,252,252,252,252,282,- 1,- 1,- 1,- 1,252,308,- 1,- 1,- 1,- 1,- 1,- 1,- 1,252,308,308,308,308,308,308,308,308,- 1,- 1,- 1,- 1,- 1,- 1,282,- 1,- 1,- 1,- 1,282,282,- 1,- 1,- 1,282,- 1,- 1,- 1,- 1,- 1,- 1,- 1,282,- 1,- 1,- 1,282,- 1,282,- 1,282,- 1,282,295,295,295,295,295,295,295,295,295,295,- 1,- 1,- 1,- 1,- 1,- 1,- 1,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,- 1,- 1,- 1,- 1,- 1,- 1,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,296,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,296,296,296,296,296,296,296,296,296,296,- 1,- 1,- 1,- 1,- 1,- 1,- 1,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,- 1,- 1,- 1,- 1,- 1,- 1,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 891
int state;int base;int backtrk;
int c;
state=start_state;
# 895
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 899
state=(- state)- 1;}
# 901
while(1){
base=Cyc_Lex_lex_base[_check_known_subscript_notnull(321,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(321,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 909
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 913
c=256;}}else{
# 915
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 918
if(Cyc_Lex_lex_check[_check_known_subscript_notnull(4025,base + c)]== state)
state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(4025,base + c)];else{
# 921
state=Cyc_Lex_lex_default[_check_known_subscript_notnull(321,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){
struct Cyc_Lexing_Error_exn_struct _tmp20B;const char*_tmp20A;struct Cyc_Lexing_Error_exn_struct*_tmp209;(int)_throw((void*)((_tmp209=_cycalloc(sizeof(*_tmp209)),((_tmp209[0]=((_tmp20B.tag=Cyc_Lexing_Error,((_tmp20B.f1=((_tmp20A="empty token",_tag_dyneither(_tmp20A,sizeof(char),12))),_tmp20B)))),_tmp209)))));}else{
# 927
return lbuf->lex_last_action;}}else{
# 930
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 934
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL35:
# 887 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 1: _LL36:
# 888 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 2: _LL37:
# 891 "lex.cyl"
 return(int)Cyc_Lex_process_id(lexbuf);case 3: _LL38:
# 893
 return(int)Cyc_Lex_process_id(lexbuf);case 4: _LL39:
# 897 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 5: _LL3A:
# 900 "lex.cyl"
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 367;case 6: _LL3B:
# 903
 return Cyc_Lex_cyc_qual(lexbuf);case 7: _LL3C:
# 907 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 361;case 8: _LL3D:
# 911 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 361;case 9: _LL3E:
# 918 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 361;case 10: _LL3F:
# 922 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 361;case 11: _LL40:
# 926 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 366;case 12: _LL41:
# 930 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 366;case 13: _LL42:
# 933
 return 343;case 14: _LL43:
# 934 "lex.cyl"
 return 344;case 15: _LL44:
# 935 "lex.cyl"
 return 341;case 16: _LL45:
# 936 "lex.cyl"
 return 342;case 17: _LL46:
# 937 "lex.cyl"
 return 337;case 18: _LL47:
# 938 "lex.cyl"
 return 338;case 19: _LL48:
# 939 "lex.cyl"
 return 350;case 20: _LL49:
# 940 "lex.cyl"
 return 351;case 21: _LL4A:
# 941 "lex.cyl"
 return 347;case 22: _LL4B:
# 942 "lex.cyl"
 return 348;case 23: _LL4C:
# 943 "lex.cyl"
 return 349;case 24: _LL4D:
# 944 "lex.cyl"
 return 356;case 25: _LL4E:
# 945 "lex.cyl"
 return 355;case 26: _LL4F:
# 946 "lex.cyl"
 return 354;case 27: _LL50:
# 947 "lex.cyl"
 return 352;case 28: _LL51:
# 948 "lex.cyl"
 return 353;case 29: _LL52:
# 949 "lex.cyl"
 return 345;case 30: _LL53:
# 950 "lex.cyl"
 return 346;case 31: _LL54:
# 951 "lex.cyl"
 return 339;case 32: _LL55:
# 953
 return 340;case 33: _LL56:
# 954 "lex.cyl"
 return 358;case 34: _LL57:
# 955 "lex.cyl"
 return 336;case 35: _LL58:
# 956 "lex.cyl"
 return 357;case 36: _LL59:
# 957 "lex.cyl"
 return 359;case 37: _LL5A:
# 959
 return 309;case 38: _LL5B:
# 962 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 39: _LL5C:
# 963 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 40: _LL5D:
# 964 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 41: _LL5E:
# 965 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 42: _LL5F:
# 966 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 43: _LL60:
# 972 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_strng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 362;case 44: _LL61:
# 980 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 363;case 45: _LL62:
# 988 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 365;case 46: _LL63:
# 995
 Cyc_Lex_token_char='\a';return 364;case 47: _LL64:
# 996 "lex.cyl"
 Cyc_Lex_token_char='\b';return 364;case 48: _LL65:
# 997 "lex.cyl"
 Cyc_Lex_token_char='\f';return 364;case 49: _LL66:
# 998 "lex.cyl"
 Cyc_Lex_token_char='\n';return 364;case 50: _LL67:
# 999 "lex.cyl"
 Cyc_Lex_token_char='\r';return 364;case 51: _LL68:
# 1000 "lex.cyl"
 Cyc_Lex_token_char='\t';return 364;case 52: _LL69:
# 1001 "lex.cyl"
 Cyc_Lex_token_char='\v';return 364;case 53: _LL6A:
# 1002 "lex.cyl"
 Cyc_Lex_token_char='\\';return 364;case 54: _LL6B:
# 1003 "lex.cyl"
 Cyc_Lex_token_char='\'';return 364;case 55: _LL6C:
# 1004 "lex.cyl"
 Cyc_Lex_token_char='"';return 364;case 56: _LL6D:
# 1005 "lex.cyl"
 Cyc_Lex_token_char='?';return 364;case 57: _LL6E:
# 1008 "lex.cyl"
 Cyc_Lex_token_char=Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,2,1,8),lexbuf);
return 364;case 58: _LL6F:
# 1013 "lex.cyl"
 Cyc_Lex_token_char=Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,3,1,16),lexbuf);
return 364;case 59: _LL70:
# 1017
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 364;case 60: _LL71:
# 1021
 return - 1;case 61: _LL72:
# 1023
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL73:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}{
# 1027
struct Cyc_Lexing_Error_exn_struct _tmp211;const char*_tmp210;struct Cyc_Lexing_Error_exn_struct*_tmp20F;(int)_throw((void*)((_tmp20F=_cycalloc(sizeof(*_tmp20F)),((_tmp20F[0]=((_tmp211.tag=Cyc_Lexing_Error,((_tmp211.f1=((_tmp210="some action didn't return!",_tag_dyneither(_tmp210,sizeof(char),27))),_tmp211)))),_tmp20F)))));};}
# 1029
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_cyc_qual_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL75:
# 1030 "lex.cyl"
{const char*_tmp212;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp212="taggedZ",_tag_dyneither(_tmp212,sizeof(char),8))));}return(int)'@';case 1: _LL76:
# 1032
{const char*_tmp213;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp213="zerotermZ",_tag_dyneither(_tmp213,sizeof(char),10))));}return(int)'@';case 2: _LL77:
# 1034
{const char*_tmp214;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp214="nozerotermZ",_tag_dyneither(_tmp214,sizeof(char),12))));}return(int)'@';case 3: _LL78:
# 1036
{const char*_tmp215;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp215="extensibleZ",_tag_dyneither(_tmp215,sizeof(char),12))));}return(int)'@';case 4: _LL79:
# 1038
{const char*_tmp216;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp216="resetableZ",_tag_dyneither(_tmp216,sizeof(char),11))));}return(int)'@';case 5: _LL7A:
# 1040
{const char*_tmp217;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp217="regionZ",_tag_dyneither(_tmp217,sizeof(char),8))));}return(int)'@';case 6: _LL7B:
# 1042
{const char*_tmp218;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp218="numeltsZ",_tag_dyneither(_tmp218,sizeof(char),9))));}return(int)'@';case 7: _LL7C:
# 1044
{const char*_tmp219;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp219="thinZ",_tag_dyneither(_tmp219,sizeof(char),6))));}return(int)'@';case 8: _LL7D:
# 1046
{const char*_tmp21A;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp21A="fatZ",_tag_dyneither(_tmp21A,sizeof(char),5))));}return(int)'@';case 9: _LL7E:
# 1048
{const char*_tmp21B;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp21B="notnullZ",_tag_dyneither(_tmp21B,sizeof(char),9))));}return(int)'@';case 10: _LL7F:
# 1050
{const char*_tmp21C;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp21C="nullableZ",_tag_dyneither(_tmp21C,sizeof(char),10))));}return(int)'@';case 11: _LL80:
# 1052
{const char*_tmp21D;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp21D="requiresZ",_tag_dyneither(_tmp21D,sizeof(char),10))));}return(int)'@';case 12: _LL81:
# 1054
{const char*_tmp21E;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp21E="ensuresZ",_tag_dyneither(_tmp21E,sizeof(char),9))));}return(int)'@';case 13: _LL82:
# 1055 "lex.cyl"
 return 333;case 14: _LL83:
# 1056 "lex.cyl"
 return 332;case 15: _LL84:
# 1057 "lex.cyl"
 return 331;case 16: _LL85:
# 1058 "lex.cyl"
 return 334;case 17: _LL86:
# 1059 "lex.cyl"
 return 335;case 18: _LL87:
# 1060 "lex.cyl"
 return 330;case 19: _LL88:
# 1061 "lex.cyl"
 return 323;case 20: _LL89:
# 1062 "lex.cyl"
 return 324;case 21: _LL8A:
# 1063 "lex.cyl"
 return 325;case 22: _LL8B:
# 1064 "lex.cyl"
 return 326;case 23: _LL8C:
# 1065 "lex.cyl"
 return 327;case 24: _LL8D:
# 1066 "lex.cyl"
 return 328;case 25: _LL8E:
# 1067 "lex.cyl"
 return 329;case 26: _LL8F:
# 1068 "lex.cyl"
 lexbuf->lex_curr_pos -=1;return(int)'@';default: _LL90:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_cyc_qual_rec(lexbuf,lexstate);}{
# 1072
struct Cyc_Lexing_Error_exn_struct _tmp224;const char*_tmp223;struct Cyc_Lexing_Error_exn_struct*_tmp222;(int)_throw((void*)((_tmp222=_cycalloc(sizeof(*_tmp222)),((_tmp222[0]=((_tmp224.tag=Cyc_Lexing_Error,((_tmp224.f1=((_tmp223="some action didn't return!",_tag_dyneither(_tmp223,sizeof(char),27))),_tmp224)))),_tmp222)))));};}
# 1074
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_cyc_qual_rec(lexbuf,1);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL92:
# 1071 "lex.cyl"
 return 0;case 1: _LL93:
# 1073
 Cyc_Lex_store_string_char('\a');return 1;case 2: _LL94:
# 1074 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3: _LL95:
# 1075 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4: _LL96:
# 1076 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5: _LL97:
# 1077 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6: _LL98:
# 1078 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7: _LL99:
# 1079 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8: _LL9A:
# 1080 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9: _LL9B:
# 1081 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10: _LL9C:
# 1082 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11: _LL9D:
# 1083 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12: _LL9E:
# 1086 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13: _LL9F:
# 1089 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14: _LLA0:
# 1091
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15: _LLA1:
# 1093
 Cyc_Lex_inc_linenumber();{const char*_tmp225;Cyc_Lex_runaway_err(((_tmp225="wide character ends in newline",_tag_dyneither(_tmp225,sizeof(char),31))),lexbuf);}return 0;case 16: _LLA2:
# 1094 "lex.cyl"
{const char*_tmp226;Cyc_Lex_runaway_err(((_tmp226="unterminated wide character",_tag_dyneither(_tmp226,sizeof(char),28))),lexbuf);}return 0;case 17: _LLA3:
# 1095 "lex.cyl"
{const char*_tmp227;Cyc_Lex_err(((_tmp227="bad character following backslash in wide character",_tag_dyneither(_tmp227,sizeof(char),52))),lexbuf);}return 1;default: _LLA4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}{
# 1099
struct Cyc_Lexing_Error_exn_struct _tmp22D;const char*_tmp22C;struct Cyc_Lexing_Error_exn_struct*_tmp22B;(int)_throw((void*)((_tmp22B=_cycalloc(sizeof(*_tmp22B)),((_tmp22B[0]=((_tmp22D.tag=Cyc_Lexing_Error,((_tmp22D.f1=((_tmp22C="some action didn't return!",_tag_dyneither(_tmp22C,sizeof(char),27))),_tmp22D)))),_tmp22B)))));};}
# 1101
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,2);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLA6:
# 1100 "lex.cyl"
 return 1;case 1: _LLA7:
# 1101 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2: _LLA8:
# 1102 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3: _LLA9:
# 1104
 lexbuf->lex_curr_pos -=1;return 0;default: _LLAA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}{
# 1108
struct Cyc_Lexing_Error_exn_struct _tmp233;const char*_tmp232;struct Cyc_Lexing_Error_exn_struct*_tmp231;(int)_throw((void*)((_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231[0]=((_tmp233.tag=Cyc_Lexing_Error,((_tmp233.f1=((_tmp232="some action didn't return!",_tag_dyneither(_tmp232,sizeof(char),27))),_tmp233)))),_tmp231)))));};}
# 1110
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,3);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLAC:
# 1108 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1: _LLAD:
# 1109 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2: _LLAE:
# 1110 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3: _LLAF:
# 1111 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4: _LLB0:
# 1112 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5: _LLB1:
# 1113 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 6: _LLB2:
# 1114 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7: _LLB3:
# 1115 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 8: _LLB4:
# 1116 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9: _LLB5:
# 1117 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10: _LLB6:
# 1118 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11: _LLB7:
# 1119 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12: _LLB8:
# 1120 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13: _LLB9:
# 1123 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,1,0,8),lexbuf));
return 1;case 14: _LLBA:
# 1128 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lex_cnst2char(Cyc_Lex_intconst(lexbuf,2,0,16),lexbuf));
return 1;case 15: _LLBB:
# 1133
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 16: _LLBC:
# 1135 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp234;Cyc_Lex_runaway_err(((_tmp234="string ends in newline",_tag_dyneither(_tmp234,sizeof(char),23))),lexbuf);}
return 0;case 17: _LLBD:
# 1139 "lex.cyl"
{const char*_tmp235;Cyc_Lex_runaway_err(((_tmp235="unterminated string",_tag_dyneither(_tmp235,sizeof(char),20))),lexbuf);}
return 0;case 18: _LLBE:
# 1142 "lex.cyl"
{const char*_tmp236;Cyc_Lex_err(((_tmp236="bad character following backslash in string",_tag_dyneither(_tmp236,sizeof(char),44))),lexbuf);}
return 1;default: _LLBF:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}{
# 1147
struct Cyc_Lexing_Error_exn_struct _tmp23C;const char*_tmp23B;struct Cyc_Lexing_Error_exn_struct*_tmp23A;(int)_throw((void*)((_tmp23A=_cycalloc(sizeof(*_tmp23A)),((_tmp23A[0]=((_tmp23C.tag=Cyc_Lexing_Error,((_tmp23C.f1=((_tmp23B="some action didn't return!",_tag_dyneither(_tmp23B,sizeof(char),27))),_tmp23C)))),_tmp23A)))));};}
# 1149
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,4);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC1:
# 1152 "lex.cyl"
{const char*_tmp23D;Cyc_Lex_store_string(((_tmp23D="\" L\"",_tag_dyneither(_tmp23D,sizeof(char),5))));}return 1;case 1: _LLC2:
# 1153 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2: _LLC3:
# 1154 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3: _LLC4:
# 1156
 lexbuf->lex_curr_pos -=1;return 0;default: _LLC5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}{
# 1160
struct Cyc_Lexing_Error_exn_struct _tmp243;const char*_tmp242;struct Cyc_Lexing_Error_exn_struct*_tmp241;(int)_throw((void*)((_tmp241=_cycalloc(sizeof(*_tmp241)),((_tmp241[0]=((_tmp243.tag=Cyc_Lexing_Error,((_tmp243.f1=((_tmp242="some action didn't return!",_tag_dyneither(_tmp242,sizeof(char),27))),_tmp243)))),_tmp241)))));};}
# 1162
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,5);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC7:
# 1159 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1: _LLC8:
# 1160 "lex.cyl"
{const char*_tmp244;Cyc_Lex_store_string(((_tmp244="\\a",_tag_dyneither(_tmp244,sizeof(char),3))));}return 1;case 2: _LLC9:
# 1161 "lex.cyl"
{const char*_tmp245;Cyc_Lex_store_string(((_tmp245="\\b",_tag_dyneither(_tmp245,sizeof(char),3))));}return 1;case 3: _LLCA:
# 1162 "lex.cyl"
{const char*_tmp246;Cyc_Lex_store_string(((_tmp246="\\f",_tag_dyneither(_tmp246,sizeof(char),3))));}return 1;case 4: _LLCB:
# 1163 "lex.cyl"
{const char*_tmp247;Cyc_Lex_store_string(((_tmp247="\\n",_tag_dyneither(_tmp247,sizeof(char),3))));}return 1;case 5: _LLCC:
# 1164 "lex.cyl"
{const char*_tmp248;Cyc_Lex_store_string(((_tmp248="\\r",_tag_dyneither(_tmp248,sizeof(char),3))));}return 1;case 6: _LLCD:
# 1165 "lex.cyl"
{const char*_tmp249;Cyc_Lex_store_string(((_tmp249="\\t",_tag_dyneither(_tmp249,sizeof(char),3))));}return 1;case 7: _LLCE:
# 1166 "lex.cyl"
{const char*_tmp24A;Cyc_Lex_store_string(((_tmp24A="\\v",_tag_dyneither(_tmp24A,sizeof(char),3))));}return 1;case 8: _LLCF:
# 1167 "lex.cyl"
{const char*_tmp24B;Cyc_Lex_store_string(((_tmp24B="\\\\",_tag_dyneither(_tmp24B,sizeof(char),3))));}return 1;case 9: _LLD0:
# 1168 "lex.cyl"
{const char*_tmp24C;Cyc_Lex_store_string(((_tmp24C="\\'",_tag_dyneither(_tmp24C,sizeof(char),3))));}return 1;case 10: _LLD1:
# 1169 "lex.cyl"
{const char*_tmp24D;Cyc_Lex_store_string(((_tmp24D="\\\"",_tag_dyneither(_tmp24D,sizeof(char),3))));}return 1;case 11: _LLD2:
# 1170 "lex.cyl"
{const char*_tmp24E;Cyc_Lex_store_string(((_tmp24E="\\?",_tag_dyneither(_tmp24E,sizeof(char),3))));}return 1;case 12: _LLD3:
# 1173 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 13: _LLD4:
# 1178 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 14: _LLD5:
# 1183
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 15: _LLD6:
# 1185 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp24F;Cyc_Lex_runaway_err(((_tmp24F="string ends in newline",_tag_dyneither(_tmp24F,sizeof(char),23))),lexbuf);}
return 0;case 16: _LLD7:
# 1189 "lex.cyl"
{const char*_tmp250;Cyc_Lex_runaway_err(((_tmp250="unterminated string",_tag_dyneither(_tmp250,sizeof(char),20))),lexbuf);}
return 0;case 17: _LLD8:
# 1192 "lex.cyl"
{const char*_tmp251;Cyc_Lex_err(((_tmp251="bad character following backslash in string",_tag_dyneither(_tmp251,sizeof(char),44))),lexbuf);}
return 1;default: _LLD9:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}{
# 1197
struct Cyc_Lexing_Error_exn_struct _tmp257;const char*_tmp256;struct Cyc_Lexing_Error_exn_struct*_tmp255;(int)_throw((void*)((_tmp255=_cycalloc(sizeof(*_tmp255)),((_tmp255[0]=((_tmp257.tag=Cyc_Lexing_Error,((_tmp257.f1=((_tmp256="some action didn't return!",_tag_dyneither(_tmp256,sizeof(char),27))),_tmp257)))),_tmp255)))));};}
# 1199
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,6);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLDB:
# 1195 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1: _LLDC:
# 1196 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2: _LLDD:
# 1201 "lex.cyl"
{const char*_tmp258;Cyc_Lex_runaway_err(((_tmp258="unterminated comment",_tag_dyneither(_tmp258,sizeof(char),21))),lexbuf);}
return 0;case 3: _LLDE:
# 1204 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4: _LLDF:
# 1205 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5: _LLE0:
# 1206 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6: _LLE1:
# 1207 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LLE2:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}{
# 1211
struct Cyc_Lexing_Error_exn_struct _tmp25E;const char*_tmp25D;struct Cyc_Lexing_Error_exn_struct*_tmp25C;(int)_throw((void*)((_tmp25C=_cycalloc(sizeof(*_tmp25C)),((_tmp25C[0]=((_tmp25E.tag=Cyc_Lexing_Error,((_tmp25E.f1=((_tmp25D="some action didn't return!",_tag_dyneither(_tmp25D,sizeof(char),27))),_tmp25E)))),_tmp25C)))));};}
# 1213
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,7);}
int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE4:
# 1214 "lex.cyl"
 Cyc_Lex_token_asm.f1=0;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 372;case 1: _LLE5:
# 1224 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 372;case 2: _LLE6:
# 1234 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 372;default: _LLE7:
# 1242
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}{
# 1245
struct Cyc_Lexing_Error_exn_struct _tmp264;const char*_tmp263;struct Cyc_Lexing_Error_exn_struct*_tmp262;(int)_throw((void*)((_tmp262=_cycalloc(sizeof(*_tmp262)),((_tmp262[0]=((_tmp264.tag=Cyc_Lexing_Error,((_tmp264.f1=((_tmp263="some action didn't return!",_tag_dyneither(_tmp263,sizeof(char),27))),_tmp264)))),_tmp262)))));};}
# 1247
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,8);}
int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE9:
# 1244 "lex.cyl"
 Cyc_Lex_store_string_char('"');
while(Cyc_Lex_strng(lexbuf)){;}
Cyc_Lex_store_string_char('"');
return 1;case 1: _LLEA:
# 1248 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_comment(lexbuf);
Cyc_Lex_store_string_char(' ');
return 1;case 2: _LLEB:
# 1252 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_store_string_char('\n');return 1;case 3: _LLEC:
# 1254 "lex.cyl"
 Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return 1;case 4: _LLED:
# 1255 "lex.cyl"
 Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{
# 1258
Cyc_Lex_store_string_char(')');
return 1;}case 5: _LLEE:
# 1261 "lex.cyl"
{const char*_tmp265;Cyc_Lex_runaway_err(((_tmp265="unterminated asm",_tag_dyneither(_tmp265,sizeof(char),17))),lexbuf);}return 0;case 6: _LLEF:
# 1262 "lex.cyl"
 Cyc_Lex_inc_linenumber();Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 7: _LLF0:
# 1263 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLF1:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}{
# 1267
struct Cyc_Lexing_Error_exn_struct _tmp26B;const char*_tmp26A;struct Cyc_Lexing_Error_exn_struct*_tmp269;(int)_throw((void*)((_tmp269=_cycalloc(sizeof(*_tmp269)),((_tmp269[0]=((_tmp26B.tag=Cyc_Lexing_Error,((_tmp26B.f1=((_tmp26A="some action didn't return!",_tag_dyneither(_tmp26A,sizeof(char),27))),_tmp26B)))),_tmp269)))));};}
# 1269
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,9);}
# 1272 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}
# 1277
static struct Cyc_Xarray_Xarray*Cyc_Lex_empty_xarray(struct _RegionHandle*id_rgn,int dummy){
const char*_tmp26E;struct _dyneither_ptr*_tmp26D;struct Cyc_Xarray_Xarray*symbols=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,((_tmp26D=_cycalloc(sizeof(*_tmp26D)),((_tmp26D[0]=(struct _dyneither_ptr)((_tmp26E="",_tag_dyneither(_tmp26E,sizeof(char),1))),_tmp26D)))));
# 1281
((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);
return symbols;}void Cyc_Lex_lex_init(int include_cyclone_keywords);static void _tmp277(unsigned int*_tmp276,unsigned int*_tmp275,struct Cyc_Lex_KeyWordInfo**_tmp273){for(*_tmp276=0;*_tmp276 < *_tmp275;(*_tmp276)++){struct Cyc_Lex_KeyWordInfo _tmp271;(*_tmp273)[*_tmp276]=(struct Cyc_Lex_KeyWordInfo)(
# 1320
(_tmp271.token_index=0,((_tmp271.is_c_keyword=0,_tmp271))));}}
# 1285
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1287
Cyc_Lex_in_extern_c=0;{
struct Cyc_List_List*x=0;
_swap_word(& Cyc_Lex_prev_extern_c,& x);
while(x != 0){
struct Cyc_List_List*t=((struct Cyc_List_List*)_check_null(x))->tl;
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(x,sizeof(struct Cyc_List_List),1));
x=t;}
# 1296
if(Cyc_Lex_ids_trie != 0){
struct Cyc_Lex_DynTrieSymbols*idt=0;
_swap_word(& idt,& Cyc_Lex_ids_trie);{
struct Cyc_Lex_DynTrieSymbols _tmp163=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Core_DynamicRegion*_tmp165;struct Cyc_Lex_DynTrieSymbols _tmp164=_tmp163;_tmp165=_tmp164.dyn;
Cyc_Core_free_ukey(_tmp165);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(idt,sizeof(struct Cyc_Lex_DynTrieSymbols),1));};}
# 1303
if(Cyc_Lex_typedefs_trie != 0){
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmp166=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmp168;struct Cyc_Lex_DynTrie _tmp167=_tmp166;_tmp168=_tmp167.dyn;
Cyc_Core_free_ukey(_tmp168);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(tdefs,sizeof(struct Cyc_Lex_DynTrie),1));};}{
# 1311
struct Cyc_Core_NewDynamicRegion _tmp169=Cyc_Core_new_ukey();struct Cyc_Core_DynamicRegion*_tmp16B;struct Cyc_Core_NewDynamicRegion _tmp16A=_tmp169;_tmp16B=_tmp16A.key;{
struct Cyc_Lex_Trie*_tmp16C=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp16B,0,Cyc_Lex_empty_trie);
struct Cyc_Xarray_Xarray*_tmp16D=((struct Cyc_Xarray_Xarray*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Xarray_Xarray*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp16B,0,Cyc_Lex_empty_xarray);
{struct Cyc_Lex_DynTrieSymbols*_tmp26F;Cyc_Lex_ids_trie=((_tmp26F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp26F)),((_tmp26F->dyn=(struct Cyc_Core_DynamicRegion*)_tmp16B,((_tmp26F->t=(struct Cyc_Lex_Trie*)_tmp16C,((_tmp26F->symbols=(struct Cyc_Xarray_Xarray*)_tmp16D,_tmp26F))))))));}{
# 1316
struct Cyc_Core_NewDynamicRegion _tmp16F=Cyc_Core_new_ukey();struct Cyc_Core_DynamicRegion*_tmp171;struct Cyc_Core_NewDynamicRegion _tmp170=_tmp16F;_tmp171=_tmp170.key;{
struct Cyc_Lex_Trie*t=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp171,0,Cyc_Lex_empty_trie);
{struct Cyc_Lex_DynTrie*_tmp270;Cyc_Lex_typedefs_trie=((_tmp270=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp270)),((_tmp270->dyn=(struct Cyc_Core_DynamicRegion*)_tmp171,((_tmp270->t=(struct Cyc_Lex_Trie*)t,_tmp270))))));}
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
{unsigned int _tmp276;unsigned int _tmp275;struct _dyneither_ptr _tmp274;struct Cyc_Lex_KeyWordInfo*_tmp273;unsigned int _tmp272;Cyc_Lex_kw_nums=((_tmp272=(unsigned int)Cyc_Lex_num_kws,((_tmp273=(struct Cyc_Lex_KeyWordInfo*)_cycalloc_atomic(_check_times(sizeof(struct Cyc_Lex_KeyWordInfo),_tmp272)),((_tmp274=_tag_dyneither(_tmp273,sizeof(struct Cyc_Lex_KeyWordInfo),_tmp272),((((_tmp275=_tmp272,_tmp277(& _tmp276,& _tmp275,& _tmp273))),_tmp274))))))));}{
unsigned int i=0;
unsigned int rwsze=70;
{unsigned int j=0;for(0;j < rwsze;++ j){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){
struct _dyneither_ptr _tmp178=(Cyc_Lex_rw_array[(int)j]).f1;
Cyc_Lex_str_index(_tmp178,0,(int)Cyc_strlen((struct _dyneither_ptr)_tmp178));
{struct Cyc_Lex_KeyWordInfo _tmp278;*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),(int)i))=((_tmp278.token_index=(int)(Cyc_Lex_rw_array[(int)j]).f2,((_tmp278.is_c_keyword=(Cyc_Lex_rw_array[(int)j]).f3,_tmp278))));}
++ i;}}}
# 1331
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};};};};};};}
