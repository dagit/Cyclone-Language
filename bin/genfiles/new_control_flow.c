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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 461 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 464
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 528
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 554
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 561
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 579
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1000 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1182
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1184
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1196
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1200
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1202
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 106
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 252 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 345
int Cyc_Tcutil_is_noreturn(void*);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 42
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 65
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 69
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 71
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 77
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 85
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 88
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 93
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 96
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 101
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 103
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 105
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 111
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 114
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 122
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
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
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 81
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 121
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 123
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 128
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 166
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 169
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 172
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple15{void*f1;struct Cyc_List_List*f2;};
struct _tuple15 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 180
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 183
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 185
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 206 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 210
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 215
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple16 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple16 pr1,struct _tuple16 pr2);
# 221
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 226
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 35
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 70 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 67 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
# 71
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 79
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 84
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp77E;struct Cyc_NewControlFlow_CFStmtAnnot _tmp77D;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp77C;enclosee->annot=(void*)((_tmp77C=_cycalloc(sizeof(*_tmp77C)),((_tmp77C[0]=((_tmp77E.tag=Cyc_NewControlFlow_CFAnnot,((_tmp77E.f1=((_tmp77D.encloser=encloser,((_tmp77D.visited=0,_tmp77D)))),_tmp77E)))),_tmp77C))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};
# 117 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 125
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
# 127
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 134
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 137
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;void*_tmp4=_tmp3;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp5=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp4;if(_tmp5->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp5->f1;}}_LL2:
 return _tmp6;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp784;const char*_tmp783;struct Cyc_Core_Impossible_exn_struct*_tmp782;(int)_throw((void*)((_tmp782=_cycalloc(sizeof(*_tmp782)),((_tmp782[0]=((_tmp784.tag=Cyc_Core_Impossible,((_tmp784.f1=((_tmp783="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp783,sizeof(char),37))),_tmp784)))),_tmp782)))));}_LL0:;}
# 144
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp785;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp785=_region_malloc(env->r,sizeof(*_tmp785)),((_tmp785[0]=Cyc_CfFlowInfo_BottomFL(),_tmp785)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 151
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 154
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 162
*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);
# 168
_tmpB->visited=env->iteration_num;{
struct _tuple20 _tmp786;return(_tmp786.f1=_tmpB,((_tmp786.f2=_tmpC,_tmp786)));};}
# 178
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 186
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 193
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 195
struct Cyc_NewControlFlow_AnalEnv*_tmpF;unsigned int _tmp10;struct Cyc_Dict_Dict _tmp11;struct _tuple21*_tmpE=ckenv;_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;_tmp11=_tmpE->f3;{
void*_tmp12=root;struct Cyc_Absyn_Vardecl*_tmp14;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp13=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp12;if(_tmp13->tag != 0)goto _LL8;else{_tmp14=_tmp13->f1;}}_LL7:
# 198
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp11,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpF->fenv)->r,_tmp14->type)){
retry: {void*_tmp15=rval;void*_tmp17;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp16=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp16->tag != 8)goto _LLD;else{_tmp17=(void*)_tmp16->f2;}}_LLC:
 rval=_tmp17;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp18->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp19->tag != 3)goto _LL11;else{if(_tmp19->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp1A->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 207
{const char*_tmp78A;void*_tmp789[1];struct Cyc_String_pa_PrintArg_struct _tmp788;(_tmp788.tag=0,((_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14->name)),((_tmp789[0]=& _tmp788,Cyc_Tcutil_warn(_tmp10,((_tmp78A="alias-free pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp78A,sizeof(char),64))),_tag_dyneither(_tmp789,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 211
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 219
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1F);
# 225
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp78B;struct _tuple19 _tmp21=(_tmp78B.f1=flow,((_tmp78B.f2=_tmp20,_tmp78B)));struct _tuple19 _tmp22=_tmp21;struct Cyc_Dict_Dict _tmp23;struct Cyc_Dict_Dict _tmp24;_LL16: if(((_tmp22.f1).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple14)((_tmp22.f1).ReachableFL).val).f1;if(((_tmp22.f2).ReachableFL).tag != 2)goto _LL18;_tmp24=((struct _tuple14)((_tmp22.f2).ReachableFL).val).f1;_LL17: {
# 228
struct _tuple21 _tmp78C;struct _tuple21 _tmp25=(_tmp78C.f1=env,((_tmp78C.f2=s->loc,((_tmp78C.f3=_tmp24,_tmp78C)))));
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp25,_tmp23);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 234
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*_tmp1F)){
# 241
*_tmp1F=_tmp20;
# 245
if(_tmp1E->visited == env->iteration_num)
# 247
env->iterate_again=1;}}
# 252
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 256
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_Dict_Dict _tmp29;struct Cyc_List_List*_tmp2A;_LL1B: if((_tmp28.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp28.ReachableFL).tag != 2)goto _LL1A;_tmp29=((struct _tuple14)(_tmp28.ReachableFL).val).f1;_tmp2A=((struct _tuple14)(_tmp28.ReachableFL).val).f2;_LL1E:
# 259
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp78F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp78E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2B=(_tmp78E=_region_malloc(fenv->r,sizeof(*_tmp78E)),((_tmp78E[0]=((_tmp78F.tag=0,((_tmp78F.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp78F)))),_tmp78E)));
void*_tmp2C=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp792;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp791;_tmp2C=(void*)((_tmp791=_region_malloc(fenv->r,sizeof(*_tmp791)),((_tmp791[0]=((_tmp792.tag=8,((_tmp792.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp792.f2=_tmp2C,_tmp792)))))),_tmp791))));}
# 266
_tmp29=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29,(void*)_tmp2B,_tmp2C);}
# 268
return Cyc_CfFlowInfo_ReachableFL(_tmp29,_tmp2A);_LL1A:;}
# 272
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 279
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp31=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp793;if(Cyc_Relations_consistent_relations(((_tmp793=_region_malloc(r,sizeof(*_tmp793)),((_tmp793->hd=_tmp31,((_tmp793->tl=assume,_tmp793))))))))
return 0;}
# 284
return 1;}
# 287
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp33=e->r;void*_tmp34=_tmp33;struct Cyc_Absyn_Exp*_tmp36;_LL20: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp35=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp34;if(_tmp35->tag != 13)goto _LL22;else{_tmp36=_tmp35->f2;}}_LL21:
 return _tmp36;_LL22:;_LL23:
 return e;_LL1F:;}
# 294
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 296
union Cyc_CfFlowInfo_FlowInfo _tmp37=inflow;struct Cyc_List_List*_tmp38;_LL25: if((_tmp37.BottomFL).tag != 1)goto _LL27;_LL26:
 return;_LL27: if((_tmp37.ReachableFL).tag != 2)goto _LL24;_tmp38=((struct _tuple14)(_tmp37.ReachableFL).val).f2;_LL28:
# 299
{void*_tmp39=Cyc_Tcutil_compress(t);void*_tmp3A=_tmp39;union Cyc_Absyn_AggrInfoU _tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3F;_LL2A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3B->tag != 11)goto _LL2C;else{_tmp3C=(_tmp3B->f1).aggr_info;_tmp3D=(_tmp3B->f1).targs;}}_LL2B: {
# 301
struct Cyc_Absyn_Aggrdecl*_tmp40=Cyc_Absyn_get_known_aggrdecl(_tmp3C);
struct Cyc_Absyn_Aggrfield*_tmp41=Cyc_Absyn_lookup_decl_field(_tmp40,f);
struct Cyc_Absyn_Exp*_tmp42=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp41))->requires_clause;
if(_tmp42 != 0){
struct _RegionHandle _tmp43=_new_region("temp");struct _RegionHandle*temp=& _tmp43;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp44=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp40->tvs,_tmp3D),_tmp42);
# 308
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp44))){
{const char*_tmp798;void*_tmp797[2];struct Cyc_String_pa_PrintArg_struct _tmp796;struct Cyc_String_pa_PrintArg_struct _tmp795;(_tmp795.tag=0,((_tmp795.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp796.tag=0,((_tmp796.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp44))),((_tmp797[0]=& _tmp796,((_tmp797[1]=& _tmp795,Cyc_CfFlowInfo_aerr(loc,((_tmp798="unable to prove %s, required to access %s",_tag_dyneither(_tmp798,sizeof(char),42))),_tag_dyneither(_tmp797,sizeof(void*),2)))))))))))));}
{const char*_tmp79B;void*_tmp79A;(_tmp79A=0,Cyc_fprintf(Cyc_stderr,((_tmp79B="  [recorded facts on path: ",_tag_dyneither(_tmp79B,sizeof(char),28))),_tag_dyneither(_tmp79A,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp79E;void*_tmp79D;(_tmp79D=0,Cyc_fprintf(Cyc_stderr,((_tmp79E="]\n",_tag_dyneither(_tmp79E,sizeof(char),3))),_tag_dyneither(_tmp79D,sizeof(void*),0)));};}}
# 306
;_pop_region(temp);}
# 315
goto _LL29;}_LL2C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3E->tag != 12)goto _LL2E;else{_tmp3F=_tmp3E->f2;}}_LL2D: {
# 317
struct Cyc_Absyn_Aggrfield*_tmp4D=Cyc_Absyn_lookup_field(_tmp3F,f);
struct Cyc_Absyn_Exp*_tmp4E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp4D))->requires_clause;
if(_tmp4E != 0){
struct _RegionHandle _tmp4F=_new_region("temp");struct _RegionHandle*temp=& _tmp4F;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp4E))){
{const char*_tmp7A3;void*_tmp7A2[2];struct Cyc_String_pa_PrintArg_struct _tmp7A1;struct Cyc_String_pa_PrintArg_struct _tmp7A0;(_tmp7A0.tag=0,((_tmp7A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7A1.tag=0,((_tmp7A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4E))),((_tmp7A2[0]=& _tmp7A1,((_tmp7A2[1]=& _tmp7A0,Cyc_CfFlowInfo_aerr(loc,((_tmp7A3="unable to prove %s, required to access %s",_tag_dyneither(_tmp7A3,sizeof(char),42))),_tag_dyneither(_tmp7A2,sizeof(void*),2)))))))))))));}
{const char*_tmp7A6;void*_tmp7A5;(_tmp7A5=0,Cyc_fprintf(Cyc_stderr,((_tmp7A6="  [recorded facts on path: ",_tag_dyneither(_tmp7A6,sizeof(char),28))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp7A9;void*_tmp7A8;(_tmp7A8=0,Cyc_fprintf(Cyc_stderr,((_tmp7A9="]\n",_tag_dyneither(_tmp7A9,sizeof(char),3))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));};}
# 321
;_pop_region(temp);}
# 328
goto _LL29;}_LL2E:;_LL2F:
 goto _LL29;_LL29:;}
# 331
goto _LL24;_LL24:;}
# 335
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp58=inflow;struct Cyc_Dict_Dict _tmp59;struct Cyc_List_List*_tmp5A;_LL31: if((_tmp58.BottomFL).tag != 1)goto _LL33;_LL32:
 return Cyc_CfFlowInfo_BottomFL();_LL33: if((_tmp58.ReachableFL).tag != 2)goto _LL30;_tmp59=((struct _tuple14)(_tmp58.ReachableFL).val).f1;_tmp5A=((struct _tuple14)(_tmp58.ReachableFL).val).f2;_LL34:
# 339
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp59,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp7AC;void*_tmp7AB;(_tmp7AB=0,Cyc_CfFlowInfo_aerr(loc,((_tmp7AC="expression may not be fully initialized",_tag_dyneither(_tmp7AC,sizeof(char),40))),_tag_dyneither(_tmp7AB,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp5D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp59,env->all_changed,r);
if(_tmp59.t == _tmp5D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5E=Cyc_CfFlowInfo_ReachableFL(_tmp5D,_tmp5A);
Cyc_NewControlFlow_update_tryflow(env,_tmp5E);
return _tmp5E;};};_LL30:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 349
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp7AD;struct _tuple0 _tmp5F=(_tmp7AD.f1=Cyc_Tcutil_compress(t),((_tmp7AD.f2=r,_tmp7AD)));struct _tuple0 _tmp60=_tmp5F;void*_tmp65;struct Cyc_Absyn_Datatypefield*_tmp67;struct _dyneither_ptr _tmp69;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;union Cyc_Absyn_AggrInfoU _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;enum Cyc_Absyn_AggrKind _tmp74;struct Cyc_List_List*_tmp75;struct _dyneither_ptr _tmp77;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp61=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp61->tag != 3)goto _LL38;else{if(_tmp61->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp62=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp62->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp63=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp63->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp64=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp64->tag != 8)goto _LL3E;else{_tmp65=(void*)_tmp64->f2;}}_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp65);return;_LL3E:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp60.f1;if(_tmp66->tag != 4)goto _LL40;else{if((((_tmp66->f1).field_info).KnownDatatypefield).tag != 2)goto _LL40;_tmp67=((struct _tuple3)(((_tmp66->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp68=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp68->tag != 6)goto _LL40;else{_tmp69=_tmp68->f2;}};_LL3F:
# 356
 if(_tmp67->typs == 0)
return;
_tmp6B=_tmp67->typs;_tmp6D=_tmp69;goto _LL41;_LL40:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6A->tag != 10)goto _LL42;else{_tmp6B=_tmp6A->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp6C->tag != 6)goto _LL42;else{_tmp6D=_tmp6C->f2;}};_LL41: {
# 360
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B);
{int i=0;for(0;i < sz;(i ++,_tmp6B=_tmp6B->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6B))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp6D,sizeof(void*),i)));}}
# 364
return;}_LL42:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6E->tag != 11)goto _LL44;else{_tmp6F=(_tmp6E->f1).aggr_info;_tmp70=(_tmp6E->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp71=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp71->tag != 6)goto _LL44;else{_tmp72=_tmp71->f2;}};_LL43: {
# 367
struct Cyc_Absyn_Aggrdecl*_tmp79=Cyc_Absyn_get_known_aggrdecl(_tmp6F);
if(_tmp79->impl == 0)return;{
struct Cyc_List_List*_tmp7A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79->impl))->fields;
struct _RegionHandle _tmp7B=_new_region("temp");struct _RegionHandle*temp=& _tmp7B;_push_region(temp);
{struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp79->tvs,_tmp70);
{int i=0;for(0;i < _get_dyneither_size(_tmp72,sizeof(void*));(i ++,_tmp7A=_tmp7A->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp7A))->hd)->type;
if(_tmp7C != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp7C,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp72.curr)[i]);}}
# 377
_npop_handler(0);return;}
# 371
;_pop_region(temp);};}_LL44:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp73->tag != 12)goto _LL46;else{_tmp74=_tmp73->f1;_tmp75=_tmp73->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp76=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp76->tag != 6)goto _LL46;else{_tmp77=_tmp76->f2;}};_LL45:
# 380
{int i=0;for(0;i < _get_dyneither_size(_tmp77,sizeof(void*));(i ++,_tmp75=_tmp75->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp75))->hd)->type,((void**)_tmp77.curr)[i]);}}
return;_LL46:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60.f1;if(_tmp78->tag != 5)goto _LL48;}if(!
Cyc_Tcutil_is_noalias_pointer(t,0))goto _LL48;_LL47:
{const char*_tmp7B0;void*_tmp7AF;(_tmp7AF=0,Cyc_Tcutil_warn(loc,((_tmp7B0="argument may still contain alias-free pointers",_tag_dyneither(_tmp7B0,sizeof(char),47))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 390
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp80=inflow;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*_tmp82;_LL4B: if((_tmp80.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp80.ReachableFL).tag != 2)goto _LL4A;_tmp81=((struct _tuple14)(_tmp80.ReachableFL).val).f1;_tmp82=((struct _tuple14)(_tmp80.ReachableFL).val).f2;_LL4E:
# 394
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp7B3;void*_tmp7B2;(_tmp7B2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B3="noliveunique attribute requires alias-free pointer",_tag_dyneither(_tmp7B3,sizeof(char),51))),_tag_dyneither(_tmp7B2,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 398
void*_tmp85=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp86=r;void*_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8A;_LL50: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp87=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp87->tag != 8)goto _LL52;else{_tmp88=(void*)_tmp87->f2;}}_LL51:
 r=_tmp88;goto retry;_LL52: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp89=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp89->tag != 5)goto _LL54;else{_tmp8A=_tmp89->f1;}}_LL53:
# 402
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp85,Cyc_CfFlowInfo_lookup_place(_tmp81,_tmp8A));
goto _LL4F;_LL54:;_LL55:
# 405
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp85)){
const char*_tmp7B6;void*_tmp7B5;(_tmp7B5=0,Cyc_Tcutil_warn(loc,((_tmp7B6="argument may contain live alias-free pointers",_tag_dyneither(_tmp7B6,sizeof(char),46))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 409
struct Cyc_Dict_Dict _tmp8D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp81,env->all_changed,r);
if(_tmp81.t == _tmp8D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp8E=Cyc_CfFlowInfo_ReachableFL(_tmp8D,_tmp82);
Cyc_NewControlFlow_update_tryflow(env,_tmp8E);
return _tmp8E;};};};_LL4A:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 418
static struct _tuple23 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 425
if(es == 0){
struct _tuple23 _tmp7B7;return(_tmp7B7.f1=inflow,((_tmp7B7.f2=0,_tmp7B7)));}
if(es->tl == 0){
struct _tuple16 _tmp90=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple16 _tmp91=_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{
struct Cyc_List_List*_tmp7BA;struct _tuple23 _tmp7B9;return(_tmp7B9.f1=_tmp92,((_tmp7B9.f2=((_tmp7BA=_region_malloc(rgn,sizeof(*_tmp7BA)),((_tmp7BA->hd=_tmp93,((_tmp7BA->tl=0,_tmp7BA)))))),_tmp7B9)));};}{
# 431
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
# 438
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
do{
this_all_changed=(env->fenv)->mt_place_set;
# 442
{struct Cyc_Dict_Dict*_tmp7BB;env->all_changed=((_tmp7BB=_region_malloc(env->r,sizeof(*_tmp7BB)),((_tmp7BB[0]=(env->fenv)->mt_place_set,_tmp7BB))));}{
struct _tuple16 _tmp97=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp99;void*_tmp9A;struct _tuple16 _tmp98=_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
outflow=_tmp99;
{struct Cyc_List_List*_tmp7BC;rvals=((_tmp7BC=_region_malloc(rgn,sizeof(*_tmp7BC)),((_tmp7BC->hd=_tmp9A,((_tmp7BC->tl=0,_tmp7BC))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 448
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp7BD;env->all_changed=((_tmp7BD=_region_malloc(env->r,sizeof(*_tmp7BD)),((_tmp7BD[0]=(env->fenv)->mt_place_set,_tmp7BD))));}{
struct _tuple16 _tmp9D=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp9F;void*_tmpA0;struct _tuple16 _tmp9E=_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;
{struct Cyc_List_List*_tmp7BE;rvals=((_tmp7BE=_region_malloc(rgn,sizeof(*_tmp7BE)),((_tmp7BE->hd=_tmpA0,((_tmp7BE->tl=rvals,_tmp7BE))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp9F,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 454
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 457
old_inflow=inflow;
# 460
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 465
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 469
struct Cyc_Dict_Dict*_tmp7BF;env->all_changed=((_tmp7BF=_region_malloc(env->r,sizeof(*_tmp7BF)),((_tmp7BF[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp7BF))));}
# 471
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 473
struct _tuple23 _tmp7C0;return(_tmp7C0.f1=outflow,((_tmp7C0.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp7C0)));};};}
# 478
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 481
struct _RegionHandle _tmpA4=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA4;_push_region(rgn);
{struct _tuple23 _tmpA5=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 482
union Cyc_CfFlowInfo_FlowInfo _tmpA7;struct Cyc_List_List*_tmpA8;struct _tuple23 _tmpA6=_tmpA5;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;
# 484
{union Cyc_CfFlowInfo_FlowInfo _tmpA9=_tmpA7;struct Cyc_Dict_Dict _tmpAA;_LL57: if((_tmpA9.ReachableFL).tag != 2)goto _LL59;_tmpAA=((struct _tuple14)(_tmpA9.ReachableFL).val).f1;_LL58:
# 486
 for(0;_tmpA8 != 0;(_tmpA8=_tmpA8->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpAA,(void*)_tmpA8->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7C3;void*_tmp7C2;(_tmp7C2=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp7C3="expression may not be initialized",_tag_dyneither(_tmp7C3,sizeof(char),34))),_tag_dyneither(_tmp7C2,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA9.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 492
struct _tuple16 _tmp7C4;struct _tuple16 _tmpAE=(_tmp7C4.f1=_tmpA7,((_tmp7C4.f2=(env->fenv)->unknown_all,_tmp7C4)));_npop_handler(0);return _tmpAE;};}
# 482
;_pop_region(rgn);}
# 500
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 507
int needs_unconsume=0;
void*_tmpAF=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmpAF,& needs_unconsume)){
struct Cyc_Core_Impossible_exn_struct _tmp7CA;const char*_tmp7C9;struct Cyc_Core_Impossible_exn_struct*_tmp7C8;(int)_throw((void*)((_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7CA.tag=Cyc_Core_Impossible,((_tmp7CA.f1=((_tmp7C9="consume_zero_ral",_tag_dyneither(_tmp7C9,sizeof(char),17))),_tmp7CA)))),_tmp7C8)))));}else{
# 512
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 516
return new_rval;}}}
# 529 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 535
union Cyc_CfFlowInfo_FlowInfo _tmpB3=outflow;struct Cyc_Dict_Dict _tmpB4;struct Cyc_List_List*_tmpB5;_LL5C: if((_tmpB3.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpB3.ReachableFL).tag != 2)goto _LL5B;_tmpB4=((struct _tuple14)(_tmpB3.ReachableFL).val).f1;_tmpB5=((struct _tuple14)(_tmpB3.ReachableFL).val).f2;_LL5F: {
# 538
union Cyc_CfFlowInfo_AbsLVal _tmpB6=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB7=_tmpB6;struct Cyc_CfFlowInfo_Place*_tmpB8;_LL61: if((_tmpB7.UnknownL).tag != 2)goto _LL63;_LL62:
# 542
 return outflow;_LL63: if((_tmpB7.PlaceL).tag != 1)goto _LL60;_tmpB8=(struct Cyc_CfFlowInfo_Place*)(_tmpB7.PlaceL).val;_LL64: {
# 546
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7CD;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7CC;nzval=(void*)((_tmp7CC=_region_malloc((env->fenv)->r,sizeof(*_tmp7CC)),((_tmp7CC[0]=((_tmp7CD.tag=8,((_tmp7CD.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp7CD.f2=nzval,_tmp7CD)))))),_tmp7CC))));}
# 550
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpB4,_tmpB8,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpBB=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpB4,env->all_changed,_tmpB8,nzval),_tmpB5);
# 555
return _tmpBB;};}_LL60:;}_LL5B:;}
# 564
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 567
union Cyc_CfFlowInfo_FlowInfo _tmpBC=outflow;struct Cyc_Dict_Dict _tmpBD;struct Cyc_List_List*_tmpBE;_LL66: if((_tmpBC.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple19 _tmp7CE;return(_tmp7CE.f1=outflow,((_tmp7CE.f2=outflow,_tmp7CE)));}_LL68: if((_tmpBC.ReachableFL).tag != 2)goto _LL65;_tmpBD=((struct _tuple14)(_tmpBC.ReachableFL).val).f1;_tmpBE=((struct _tuple14)(_tmpBC.ReachableFL).val).f2;_LL69: {
# 570
union Cyc_CfFlowInfo_AbsLVal _tmpC0=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpC1=_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpC2;_LL6B: if((_tmpC1.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple19 _tmp7CF;return(_tmp7CF.f1=outflow,((_tmp7CF.f2=outflow,_tmp7CF)));}_LL6D: if((_tmpC1.PlaceL).tag != 1)goto _LL6A;_tmpC2=(struct Cyc_CfFlowInfo_Place*)(_tmpC1.PlaceL).val;_LL6E: {
# 573
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7D2;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7D1;nzval=(void*)((_tmp7D1=_region_malloc((env->fenv)->r,sizeof(*_tmp7D1)),((_tmp7D1[0]=((_tmp7D2.tag=8,((_tmp7D2.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp7D2.f2=nzval,_tmp7D2)))))),_tmp7D1))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7D5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7D4;zval=(void*)((_tmp7D4=_region_malloc((env->fenv)->r,sizeof(*_tmp7D4)),((_tmp7D4[0]=((_tmp7D5.tag=8,((_tmp7D5.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp7D5.f2=zval,_tmp7D5)))))),_tmp7D4))));};}
# 579
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,zval);{
struct _tuple19 _tmp7D6;return
(_tmp7D6.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,nzval),_tmpBE),((_tmp7D6.f2=
# 584
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,zval),_tmpBE),_tmp7D6)));};}_LL6A:;}_LL65:;}
# 590
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 598
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 602
void*_tmpCB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpCC=_tmpCB;void*_tmpCE;union Cyc_Absyn_Constraint*_tmpCF;union Cyc_Absyn_Constraint*_tmpD0;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC;if(_tmpCD->tag != 5)goto _LL72;else{_tmpCE=(_tmpCD->f1).elt_typ;_tmpCF=((_tmpCD->f1).ptr_atts).bounds;_tmpD0=((_tmpCD->f1).ptr_atts).zero_term;}}_LL71: {
# 604
union Cyc_CfFlowInfo_FlowInfo _tmpD1=f;struct Cyc_Dict_Dict _tmpD2;struct Cyc_List_List*_tmpD3;_LL75: if((_tmpD1.BottomFL).tag != 1)goto _LL77;_LL76: {
# 606
struct _tuple16 _tmp7D7;return(_tmp7D7.f1=f,((_tmp7D7.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7D7)));}_LL77: if((_tmpD1.ReachableFL).tag != 2)goto _LL74;_tmpD2=((struct _tuple14)(_tmpD1.ReachableFL).val).f1;_tmpD3=((struct _tuple14)(_tmpD1.ReachableFL).val).f2;_LL78:
# 609
 if(Cyc_Tcutil_is_bound_one(_tmpCF)){
struct _tuple15 _tmpD5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmpD7;struct Cyc_List_List*_tmpD8;struct _tuple15 _tmpD6=_tmpD5;_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{
void*_tmpD9=_tmpD7;struct Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDC;struct Cyc_CfFlowInfo_Place*_tmpE0;enum Cyc_CfFlowInfo_InitLevel _tmpE3;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDA->tag != 8)goto _LL7C;else{_tmpDB=_tmpDA->f1;_tmpDC=(void*)_tmpDA->f2;}}_LL7B: {
# 613
struct Cyc_Core_Impossible_exn_struct _tmp7DD;const char*_tmp7DC;struct Cyc_Core_Impossible_exn_struct*_tmp7DB;(int)_throw((void*)((_tmp7DB=_cycalloc(sizeof(*_tmp7DB)),((_tmp7DB[0]=((_tmp7DD.tag=Cyc_Core_Impossible,((_tmp7DD.f1=((_tmp7DC="named location in anal_derefR",_tag_dyneither(_tmp7DC,sizeof(char),30))),_tmp7DD)))),_tmp7DB)))));}_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpDD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDD->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpDE=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDE->tag != 2)goto _LL80;}_LL7F:
# 616
{void*_tmpE7=e->annot;void*_tmpE8=_tmpE7;struct Cyc_List_List*_tmpEA;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpE9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpE8;if(_tmpE9->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpEA=_tmpE9->f1;}}_LL8A:
# 618
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpEA))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 622
{void*_tmpEB=e->r;void*_tmpEC=_tmpEB;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC;if(_tmpED->tag != 22)goto _LL90;}_LL8F:
# 624
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7E0;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7DF;e->annot=(void*)((_tmp7DF=_cycalloc(sizeof(*_tmp7DF)),((_tmp7DF[0]=((_tmp7E0.tag=Cyc_CfFlowInfo_NotZero,((_tmp7E0.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7E0)))),_tmp7DF))));}
goto _LL8D;_LL90:;_LL91:
# 627
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 630
goto _LL88;_LL88:;}
# 632
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpDF=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDF->tag != 5)goto _LL82;else{_tmpE0=_tmpDF->f1;}}_LL81: {
# 636
int possibly_null=0;
{void*_tmpF0=e->annot;void*_tmpF1=_tmpF0;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;_LL93: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpF2=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpF1;if(_tmpF2->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL95;else{_tmpF3=_tmpF2->f1;}}_LL94:
# 639
 possibly_null=1;
_tmpF5=_tmpF3;goto _LL96;_LL95: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpF4=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpF1;if(_tmpF4->tag != Cyc_CfFlowInfo_NotZero)goto _LL97;else{_tmpF5=_tmpF4->f1;}}_LL96:
# 642
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpF5))goto _LL98;
goto _LL92;_LL97:;_LL98:
# 646
{void*_tmpF6=e->r;void*_tmpF7=_tmpF6;_LL9A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF7;if(_tmpF8->tag != 22)goto _LL9C;}_LL9B:
# 648
 if(possibly_null){
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7E3;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7E2;e->annot=(void*)((_tmp7E2=_cycalloc(sizeof(*_tmp7E2)),((_tmp7E2[0]=((_tmp7E3.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7E3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7E3)))),_tmp7E2))));}else{
# 651
struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7E6;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7E5;e->annot=(void*)((_tmp7E5=_cycalloc(sizeof(*_tmp7E5)),((_tmp7E5[0]=((_tmp7E6.tag=Cyc_CfFlowInfo_NotZero,((_tmp7E6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7E6)))),_tmp7E5))));}
goto _LL99;_LL9C:;_LL9D:
# 654
 if(possibly_null)
e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;else{
# 657
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;}
goto _LL99;_LL99:;}
# 660
goto _LL92;_LL92:;}{
# 662
struct _tuple16 _tmp7E7;return(_tmp7E7.f1=f,((_tmp7E7.f2=Cyc_CfFlowInfo_lookup_place(_tmpD2,_tmpE0),_tmp7E7)));};}_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpE1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE1->tag != 0)goto _LL84;}_LL83:
# 664
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _tmp7E8;return(_tmp7E8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7E8)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE2->tag != 3)goto _LL86;else{_tmpE3=_tmpE2->f1;}}_LL85:
# 667
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpE3,_tmpD8);goto _LL87;_LL86:;_LL87:
# 669
{void*_tmpFF=e->r;void*_tmp100=_tmpFF;_LL9F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp100;if(_tmp101->tag != 22)goto _LLA1;}_LLA0:
# 671
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7EB;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7EA;e->annot=(void*)((_tmp7EA=_cycalloc(sizeof(*_tmp7EA)),((_tmp7EA[0]=((_tmp7EB.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7EB.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7EB)))),_tmp7EA))));}
goto _LL9E;_LLA1:;_LLA2:
# 674
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9E;_LL9E:;}
# 677
goto _LL79;_LL79:;};}else{
# 680
void*_tmp104=e->annot;void*_tmp105=_tmp104;struct Cyc_List_List*_tmp107;_LLA4: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp106=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp105;if(_tmp106->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA6;else{_tmp107=_tmp106->f1;}}_LLA5:
# 682
 if(!Cyc_Relations_same_relns(_tmpD3,_tmp107))goto _LLA7;
goto _LLA3;_LLA6:;_LLA7:
# 685
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7EE;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7ED;e->annot=(void*)((_tmp7ED=_cycalloc(sizeof(*_tmp7ED)),((_tmp7ED[0]=((_tmp7EE.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7EE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7EE)))),_tmp7ED))));}
goto _LLA3;_LLA3:;}{
# 689
enum Cyc_CfFlowInfo_InitLevel _tmp10A=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD2,r);switch(_tmp10A){case Cyc_CfFlowInfo_NoneIL: _LLA8: {
# 691
struct _tuple15 _tmp10B=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp10D;struct _tuple15 _tmp10C=_tmp10B;_tmp10D=_tmp10C.f1;
{void*_tmp10E=_tmp10D;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp10F->tag != 7)goto _LLAD;}_LLAC:
# 694
{const char*_tmp7F1;void*_tmp7F0;(_tmp7F0=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7F1="attempt to dereference a consumed alias-free pointer",_tag_dyneither(_tmp7F1,sizeof(char),53))),_tag_dyneither(_tmp7F0,sizeof(void*),0)));}
goto _LLAA;_LLAD:;_LLAE: {
# 697
const char*_tmp7F4;void*_tmp7F3;(_tmp7F3=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7F4="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp7F4,sizeof(char),46))),_tag_dyneither(_tmp7F3,sizeof(void*),0)));}_LLAA:;}
# 699
goto _LLA9;}case Cyc_CfFlowInfo_AllIL: _LLA9: {
# 701
struct _tuple16 _tmp7F5;return(_tmp7F5.f1=f,((_tmp7F5.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7F5)));}case Cyc_CfFlowInfo_ThisIL: _LLAF: {
# 703
struct _tuple16 _tmp7F6;return(_tmp7F6.f1=f,((_tmp7F6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_none),_tmp7F6)));}}};_LL74:;}_LL72:;_LL73: {
# 706
struct Cyc_Core_Impossible_exn_struct _tmp7FC;const char*_tmp7FB;struct Cyc_Core_Impossible_exn_struct*_tmp7FA;(int)_throw((void*)((_tmp7FA=_cycalloc(sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FC.tag=Cyc_Core_Impossible,((_tmp7FC.f1=((_tmp7FB="right deref of non-pointer-type",_tag_dyneither(_tmp7FB,sizeof(char),32))),_tmp7FC)))),_tmp7FA)))));}_LL6F:;}
# 713
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 716
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 719
{void*_tmp119=e1->r;void*_tmp11A=_tmp119;void*_tmp11C;_LLB2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11A;if(_tmp11B->tag != 1)goto _LLB4;else{_tmp11C=(void*)_tmp11B->f2;}}_LLB3: {
# 721
struct Cyc_Absyn_Vardecl*_tmp11D=Cyc_Tcutil_nonesc_vardecl(_tmp11C);
if(_tmp11D != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmp11D);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 727
goto _LLB1;}_LLB4:;_LLB5:
 goto _LLB1;_LLB1:;}{
# 731
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 737
return relns;};}
# 745
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 750
struct _tuple18 _tmp11E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmp120;struct _tuple18 _tmp11F=_tmp11E;_tmp120=_tmp11F.f2;
{struct _tuple18 _tmp7FD;struct _tuple18 _tmp121=(_tmp7FD.f1=inflow,((_tmp7FD.f2=_tmp120,_tmp7FD)));struct _tuple18 _tmp122=_tmp121;struct Cyc_Dict_Dict _tmp123;struct Cyc_List_List*_tmp124;struct Cyc_CfFlowInfo_Place*_tmp125;_LLB7: if(((_tmp122.f1).ReachableFL).tag != 2)goto _LLB9;_tmp123=((struct _tuple14)((_tmp122.f1).ReachableFL).val).f1;_tmp124=((struct _tuple14)((_tmp122.f1).ReachableFL).val).f2;if(((_tmp122.f2).PlaceL).tag != 1)goto _LLB9;_tmp125=(struct Cyc_CfFlowInfo_Place*)((_tmp122.f2).PlaceL).val;_LLB8: {
# 753
void*_tmp126=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 755
struct _tuple15 _tmp127=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);void*_tmp129;struct Cyc_List_List*_tmp12A;struct _tuple15 _tmp128=_tmp127;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;
for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp800;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7FF;_tmp126=(void*)((_tmp7FF=_region_malloc((env->fenv)->r,sizeof(*_tmp7FF)),((_tmp7FF[0]=((_tmp800.tag=8,((_tmp800.f1=(struct Cyc_Absyn_Vardecl*)_tmp12A->hd,((_tmp800.f2=_tmp126,_tmp800)))))),_tmp7FF))));}
# 760
_tmp123=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp123,env->all_changed,_tmp125,_tmp126);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp123,_tmp124);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 766
goto _LLB6;}_LLB9: if(((_tmp122.f1).BottomFL).tag != 1)goto _LLBB;_LLBA:
 goto _LLB6;_LLBB:;_LLBC:
# 769
{const char*_tmp803;void*_tmp802;(_tmp802=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp803="noconsume() parameters must be l-values",_tag_dyneither(_tmp803,sizeof(char),40))),_tag_dyneither(_tmp802,sizeof(void*),0)));}
goto _LLB6;_LLB6:;}
# 772
return inflow;}
# 777
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 785
union Cyc_CfFlowInfo_FlowInfo _tmp130=outflow;struct Cyc_Dict_Dict _tmp131;struct Cyc_List_List*_tmp132;_LLBE: if((_tmp130.BottomFL).tag != 1)goto _LLC0;_LLBF: {
# 787
struct _tuple16 _tmp804;return(_tmp804.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp804.f2=rval,_tmp804)));}_LLC0: if((_tmp130.ReachableFL).tag != 2)goto _LLBD;_tmp131=((struct _tuple14)(_tmp130.ReachableFL).val).f1;_tmp132=((struct _tuple14)(_tmp130.ReachableFL).val).f2;_LLC1: {
# 789
union Cyc_CfFlowInfo_AbsLVal _tmp134=lval;struct Cyc_CfFlowInfo_Place*_tmp135;_LLC3: if((_tmp134.PlaceL).tag != 1)goto _LLC5;_tmp135=(struct Cyc_CfFlowInfo_Place*)(_tmp134.PlaceL).val;_LLC4: {
# 791
struct Cyc_Dict_Dict _tmp136=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp131,env->all_changed,_tmp135,rval);
_tmp132=Cyc_Relations_reln_assign_exp(fenv->r,_tmp132,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp136,_tmp132);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 797
struct _tuple15 _tmp137=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp131,_tmp135));void*_tmp139;struct _tuple15 _tmp138=_tmp137;_tmp139=_tmp138.f1;{
void*_tmp13A=_tmp139;_LLC8: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp13B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13B->tag != 3)goto _LLCA;else{if(_tmp13B->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCA;}}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp13C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13C->tag != 7)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp13D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13D->tag != 0)goto _LLCE;}_LLCD:
 goto _LLC7;_LLCE:;_LLCF:
# 803
{const char*_tmp807;void*_tmp806;(_tmp806=0,Cyc_Tcutil_warn(lexp->loc,((_tmp807="assignment may overwrite alias-free pointer(s)",_tag_dyneither(_tmp807,sizeof(char),47))),_tag_dyneither(_tmp806,sizeof(void*),0)));}
goto _LLC7;_LLC7:;};}
# 808
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple16 _tmp808;return(_tmp808.f1=outflow,((_tmp808.f2=rval,_tmp808)));};}_LLC5: if((_tmp134.UnknownL).tag != 2)goto _LLC2;_LLC6: {
# 811
struct _tuple16 _tmp809;return(_tmp809.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp809.f2=rval,_tmp809)));}_LLC2:;}_LLBD:;}
# 818
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 824
union Cyc_CfFlowInfo_FlowInfo _tmp142=f;struct Cyc_Dict_Dict _tmp143;struct Cyc_List_List*_tmp144;_LLD1: if((_tmp142.BottomFL).tag != 1)goto _LLD3;_LLD2:
 return Cyc_CfFlowInfo_BottomFL();_LLD3: if((_tmp142.ReachableFL).tag != 2)goto _LLD0;_tmp143=((struct _tuple14)(_tmp142.ReachableFL).val).f1;_tmp144=((struct _tuple14)(_tmp142.ReachableFL).val).f2;_LLD4:
# 829
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp80F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp80E;struct Cyc_CfFlowInfo_Place*_tmp80D;_tmp143=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp143,env->all_changed,(
(_tmp80D=_region_malloc(env->r,sizeof(*_tmp80D)),((_tmp80D->root=(void*)((_tmp80F=_region_malloc(env->r,sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp80E.tag=0,((_tmp80E.f1=vd,_tmp80E)))),_tmp80F)))),((_tmp80D->fields=0,_tmp80D)))))),rval);}
# 832
_tmp144=Cyc_Relations_reln_assign_var(env->r,_tmp144,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp148=Cyc_CfFlowInfo_ReachableFL(_tmp143,_tmp144);
Cyc_NewControlFlow_update_tryflow(env,_tmp148);
# 837
return _tmp148;};_LLD0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 841
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 848
if(vds == 0)return inflow;{
# 851
struct Cyc_List_List*_tmp149=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp810;es=((_tmp810=_cycalloc(sizeof(*_tmp810)),((_tmp810->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),((_tmp810->tl=es,_tmp810))))));}}}
# 857
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp149,fenv->unknown_all,pat_loc,name_locs);
# 859
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 863
struct _tuple16 _tmp14B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp14D;void*_tmp14E;struct _tuple16 _tmp14C=_tmp14B;_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp14D,_tmp14E);}}{
# 871
struct Cyc_List_List*_tmp14F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp14F != 0;_tmp14F=_tmp14F->tl){
struct _tuple24*_tmp150=(struct _tuple24*)_tmp14F->hd;struct Cyc_Absyn_Vardecl**_tmp152;struct Cyc_Absyn_Exp*_tmp153;struct _tuple24*_tmp151=_tmp150;_tmp152=_tmp151->f1;_tmp153=_tmp151->f2;
if(_tmp152 != 0  && _tmp153 != 0){
if(_tmp153->topt == 0){
const char*_tmp814;void*_tmp813[1];struct Cyc_String_pa_PrintArg_struct _tmp812;(_tmp812.tag=0,((_tmp812.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp153)),((_tmp813[0]=& _tmp812,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp814="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp814,sizeof(char),41))),_tag_dyneither(_tmp813,sizeof(void*),1)))))));}{
# 885
struct Cyc_List_List _tmp815;struct Cyc_List_List l=(_tmp815.hd=_tmp153,((_tmp815.tl=0,_tmp815)));
union Cyc_CfFlowInfo_FlowInfo _tmp157=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp158=Cyc_NewControlFlow_anal_Lexp(env,_tmp157,0,0,_tmp153);union Cyc_CfFlowInfo_AbsLVal _tmp15A;struct _tuple18 _tmp159=_tmp158;_tmp15A=_tmp159.f2;{
struct _tuple16 _tmp15B=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp157,_tmp153);union Cyc_CfFlowInfo_FlowInfo _tmp15D;void*_tmp15E;struct _tuple16 _tmp15C=_tmp15B;_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp15F=_tmp15D;struct Cyc_Dict_Dict _tmp160;struct Cyc_List_List*_tmp161;_LLD6: if((_tmp15F.ReachableFL).tag != 2)goto _LLD8;_tmp160=((struct _tuple14)(_tmp15F.ReachableFL).val).f1;_tmp161=((struct _tuple14)(_tmp15F.ReachableFL).val).f2;_LLD7:
# 891
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp162=_tmp15A;struct Cyc_CfFlowInfo_Place*_tmp163;_LLDB: if((_tmp162.PlaceL).tag != 1)goto _LLDD;_tmp163=(struct Cyc_CfFlowInfo_Place*)(_tmp162.PlaceL).val;_LLDC:
# 894
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp818;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp817;_tmp15E=(void*)((_tmp817=_region_malloc(fenv->r,sizeof(*_tmp817)),((_tmp817[0]=((_tmp818.tag=8,((_tmp818.f1=*_tmp152,((_tmp818.f2=_tmp15E,_tmp818)))))),_tmp817))));}{
# 897
void*_tmp166=Cyc_CfFlowInfo_lookup_place(_tmp160,_tmp163);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp81B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp81A;_tmp166=(void*)((_tmp81A=_region_malloc(fenv->r,sizeof(*_tmp81A)),((_tmp81A[0]=((_tmp81B.tag=8,((_tmp81B.f1=*_tmp152,((_tmp81B.f2=_tmp166,_tmp81B)))))),_tmp81A))));}
_tmp160=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp160,env->all_changed,_tmp163,_tmp166);
_tmp15D=Cyc_CfFlowInfo_ReachableFL(_tmp160,_tmp161);
goto _LLDA;};_LLDD:;_LLDE:
# 904
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp153->topt)) && !
# 906
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp152)->type)){
# 908
const char*_tmp81F;void*_tmp81E[1];struct Cyc_String_pa_PrintArg_struct _tmp81D;(_tmp81D.tag=0,((_tmp81D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp153)),((_tmp81E[0]=& _tmp81D,Cyc_CfFlowInfo_aerr(exp_loc,((_tmp81F="aliased pattern expression not an l-value: %s",_tag_dyneither(_tmp81F,sizeof(char),46))),_tag_dyneither(_tmp81E,sizeof(void*),1)))))));}_LLDA:;}
# 916
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp15D,*_tmp152,_tmp153,_tmp15E,pat_loc);
goto _LLD5;_LLD8:;_LLD9:
# 919
 goto _LLD5;_LLD5:;};};};}}
# 924
return inflow;};};}
# 927
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 929
void*_tmp16D=e->r;void*_tmp16E=_tmp16D;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17E;_LLE0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp16F->tag != 1)goto _LLE2;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp170=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp16F->f2);if(_tmp170->tag != 4)goto _LLE2;}}_LLE1:
 goto _LLE3;_LLE2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp171->tag != 1)goto _LLE4;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp172=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp171->f2);if(_tmp172->tag != 3)goto _LLE4;}}_LLE3:
 goto _LLE5;_LLE4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp173->tag != 1)goto _LLE6;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp174=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp173->f2);if(_tmp174->tag != 5)goto _LLE6;}}_LLE5:
 return 1;_LLE6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp175->tag != 19)goto _LLE8;else{_tmp176=_tmp175->f1;}}_LLE7:
 _tmp178=_tmp176;goto _LLE9;_LLE8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp177->tag != 20)goto _LLEA;else{_tmp178=_tmp177->f1;}}_LLE9:
 _tmp17A=_tmp178;goto _LLEB;_LLEA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp179->tag != 21)goto _LLEC;else{_tmp17A=_tmp179->f1;}}_LLEB:
# 936
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17A,cast_ok);_LLEC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp17B->tag != 22)goto _LLEE;else{_tmp17C=_tmp17B->f1;}}_LLED: {
# 938
void*_tmp17F=Cyc_Tcutil_compress((void*)_check_null(_tmp17C->topt));void*_tmp180=_tmp17F;_LLF3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp180;if(_tmp181->tag != 10)goto _LLF5;}_LLF4:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17C,cast_ok);_LLF5:;_LLF6:
 return 0;_LLF2:;}_LLEE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp17D->tag != 13)goto _LLF0;else{_tmp17E=_tmp17D->f2;}}_LLEF:
# 943
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17E,cast_ok);else{
return 0;}_LLF0:;_LLF1:
 return 0;_LLDF:;}
# 949
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp182=*rop;union Cyc_Relations_RelnOp _tmp183=_tmp182;unsigned int _tmp184;unsigned int _tmp185;_LLF8: if((_tmp183.RParam).tag != 5)goto _LLFA;_tmp184=(unsigned int)(_tmp183.RParam).val;_LLF9: {
# 952
struct Cyc_Absyn_Exp*_tmp186=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp184);
return Cyc_Relations_exp2relnop(_tmp186,rop);}_LLFA: if((_tmp183.RParamNumelts).tag != 6)goto _LLFC;_tmp185=(unsigned int)(_tmp183.RParamNumelts).val;_LLFB: {
# 955
struct Cyc_Absyn_Exp*_tmp187=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp185);
return Cyc_Relations_exp2relnop(Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp187,0),rop);}_LLFC:;_LLFD:
 return 1;_LLF7:;}
# 961
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp188=rop;unsigned int _tmp189;unsigned int _tmp18A;_LLFF: if((_tmp188.RParam).tag != 5)goto _LL101;_tmp189=(unsigned int)(_tmp188.RParam).val;_LL100:
# 964
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp189));_LL101: if((_tmp188.RParamNumelts).tag != 6)goto _LL103;_tmp18A=(unsigned int)(_tmp188.RParamNumelts).val;_LL102:
# 966
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp18A));_LL103:;_LL104:
 return Cyc_Relations_rop2string(rop);_LLFE:;}
# 971
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 974
union Cyc_CfFlowInfo_FlowInfo _tmp18B=inflow;struct Cyc_Dict_Dict _tmp18C;struct Cyc_List_List*_tmp18D;_LL106: if((_tmp18B.BottomFL).tag != 1)goto _LL108;_LL107:
 return;_LL108: if((_tmp18B.ReachableFL).tag != 2)goto _LL105;_tmp18C=((struct _tuple14)(_tmp18B.ReachableFL).val).f1;_tmp18D=((struct _tuple14)(_tmp18B.ReachableFL).val).f2;_LL109:
# 977
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp18E=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp18E->rop1;
union Cyc_Relations_RelnOp rop2=_tmp18E->rop2;
enum Cyc_Relations_Relation _tmp18F=Cyc_Relations_flip_relation(_tmp18E->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(env->r,rop2,_tmp18F,rop1,_tmp18D))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp18E->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
{const char*_tmp826;void*_tmp825[4];struct Cyc_String_pa_PrintArg_struct _tmp824;struct Cyc_String_pa_PrintArg_struct _tmp823;struct Cyc_String_pa_PrintArg_struct _tmp822;struct Cyc_String_pa_PrintArg_struct _tmp821;(_tmp821.tag=0,((_tmp821.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp18D)),((_tmp822.tag=0,((_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp823.tag=0,((_tmp823.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp824.tag=0,((_tmp824.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp825[0]=& _tmp824,((_tmp825[1]=& _tmp823,((_tmp825[2]=& _tmp822,((_tmp825[3]=& _tmp821,Cyc_Tcutil_terr(loc,((_tmp826="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)",_tag_dyneither(_tmp826,sizeof(char),77))),_tag_dyneither(_tmp825,sizeof(void*),4)))))))))))))))))))))))));}
break;}}
# 992
goto _LL105;_LL105:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp8D9(unsigned int*_tmp8D8,unsigned int*_tmp8D7,void***_tmp8D5,struct Cyc_List_List**_tmp39D){for(*_tmp8D8=0;*_tmp8D8 < *_tmp8D7;(*_tmp8D8)++){
# 1585 "new_control_flow.cyc"
void*_tmp8D3;(*_tmp8D5)[*_tmp8D8]=(void*)((_tmp8D3=(void*)((struct Cyc_List_List*)_check_null(*_tmp39D))->hd,((*_tmp39D=(*_tmp39D)->tl,_tmp8D3))));}}
# 997 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1001
struct Cyc_CfFlowInfo_FlowEnv*_tmp196=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 1014
{union Cyc_CfFlowInfo_FlowInfo _tmp197=inflow;struct Cyc_Dict_Dict _tmp198;struct Cyc_List_List*_tmp199;_LL10B: if((_tmp197.BottomFL).tag != 1)goto _LL10D;_LL10C: {
struct _tuple16 _tmp827;return(_tmp827.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp827.f2=_tmp196->unknown_all,_tmp827)));}_LL10D: if((_tmp197.ReachableFL).tag != 2)goto _LL10A;_tmp198=((struct _tuple14)(_tmp197.ReachableFL).val).f1;_tmp199=((struct _tuple14)(_tmp197.ReachableFL).val).f2;_LL10E:
 d=_tmp198;relns=_tmp199;_LL10A:;}
# 1029 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 1050 "new_control_flow.cyc"
struct _tuple18 _tmp19B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp19D;union Cyc_CfFlowInfo_AbsLVal _tmp19E;struct _tuple18 _tmp19C=_tmp19B;_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;{
struct _tuple18 _tmp828;struct _tuple18 _tmp19F=(_tmp828.f1=_tmp19D,((_tmp828.f2=_tmp19E,_tmp828)));struct _tuple18 _tmp1A0=_tmp19F;struct Cyc_Dict_Dict _tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_CfFlowInfo_Place*_tmp1A3;_LL110: if(((_tmp1A0.f1).ReachableFL).tag != 2)goto _LL112;_tmp1A1=((struct _tuple14)((_tmp1A0.f1).ReachableFL).val).f1;_tmp1A2=((struct _tuple14)((_tmp1A0.f1).ReachableFL).val).f2;if(((_tmp1A0.f2).PlaceL).tag != 1)goto _LL112;_tmp1A3=(struct Cyc_CfFlowInfo_Place*)((_tmp1A0.f2).PlaceL).val;_LL111: {
# 1053
void*_tmp1A4=Cyc_CfFlowInfo_lookup_place(_tmp1A1,_tmp1A3);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp1A4,& needs_unconsume)){
{const char*_tmp82B;void*_tmp82A;(_tmp82A=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp82B="expression attempts to copy a consumed alias-free value",_tag_dyneither(_tmp82B,sizeof(char),56))),_tag_dyneither(_tmp82A,sizeof(void*),0)));}{
struct _tuple16 _tmp82C;return(_tmp82C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp82C.f2=_tmp196->unknown_all,_tmp82C)));};}else{
# 1059
if(needs_unconsume){
# 1061
struct _tuple16 _tmp82D;return(_tmp82D.f1=_tmp19D,((_tmp82D.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp196,_tmp1A4),_tmp82D)));}else{
# 1064
void*_tmp1A9=Cyc_CfFlowInfo_make_unique_consumed(_tmp196,(void*)_check_null(e->topt),e,env->iteration_num,_tmp1A4);
struct Cyc_Dict_Dict _tmp1AA=Cyc_CfFlowInfo_assign_place(_tmp196,e->loc,_tmp1A1,env->all_changed,_tmp1A3,_tmp1A9);
# 1076
struct _tuple16 _tmp82E;return(_tmp82E.f1=Cyc_CfFlowInfo_ReachableFL(_tmp1AA,_tmp1A2),((_tmp82E.f2=_tmp1A4,_tmp82E)));}}}_LL112:;_LL113:
# 1078
 goto _LL10F;_LL10F:;};}{
# 1081
void*_tmp1AD=e->r;void*_tmp1AE=_tmp1AD;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Vardecl*_tmp1C7;struct Cyc_Absyn_Vardecl*_tmp1CA;struct Cyc_Absyn_Vardecl*_tmp1CD;enum Cyc_Absyn_Primop _tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_List_List*_tmp1E0;int _tmp1E2;struct Cyc_Absyn_Exp*_tmp1E3;void**_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;int _tmp1E6;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F2;struct _dyneither_ptr*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F5;struct _dyneither_ptr*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F8;struct _dyneither_ptr*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_Absyn_Exp*_tmp202;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_List_List*_tmp208;struct Cyc_Absyn_Datatypedecl*_tmp209;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20D;struct Cyc_List_List*_tmp20F;enum Cyc_Absyn_AggrKind _tmp210;struct Cyc_List_List*_tmp211;struct Cyc_List_List*_tmp214;struct Cyc_Absyn_Vardecl*_tmp216;struct Cyc_Absyn_Exp*_tmp217;struct Cyc_Absyn_Exp*_tmp218;int _tmp219;struct Cyc_Absyn_Exp*_tmp21B;void*_tmp21C;int _tmp21D;struct Cyc_Absyn_Stmt*_tmp21F;_LL115: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1AF->tag != 13)goto _LL117;else{_tmp1B0=_tmp1AF->f2;if(_tmp1AF->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL117;}}_LL116: {
# 1085
struct _tuple16 _tmp226=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1B0);union Cyc_CfFlowInfo_FlowInfo _tmp228;void*_tmp229;struct _tuple16 _tmp227=_tmp226;_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;{
struct _tuple16 _tmp22A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp228,_tmp1B0,_tmp229);union Cyc_CfFlowInfo_FlowInfo _tmp22C;void*_tmp22D;struct _tuple16 _tmp22B=_tmp22A;_tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;{
struct _tuple16 _tmp82F;return(_tmp82F.f1=_tmp22C,((_tmp82F.f2=_tmp229,_tmp82F)));};};}_LL117: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1B1->tag != 13)goto _LL119;else{_tmp1B2=_tmp1B1->f2;}}_LL118:
# 1091
 _tmp1B4=_tmp1B2;goto _LL11A;_LL119: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1B3=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1B3->tag != 11)goto _LL11B;else{_tmp1B4=_tmp1B3->f1;}}_LL11A:
 _tmp1B6=_tmp1B4;goto _LL11C;_LL11B: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1B5->tag != 12)goto _LL11D;else{_tmp1B6=_tmp1B5->f1;}}_LL11C:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1B6);_LL11D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1B7->tag != 0)goto _LL11F;else{if(((_tmp1B7->f1).Null_c).tag != 1)goto _LL11F;}}_LL11E:
# 1095
 goto _LL120;_LL11F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1B8->tag != 0)goto _LL121;else{if(((_tmp1B8->f1).Int_c).tag != 5)goto _LL121;if(((struct _tuple7)((_tmp1B8->f1).Int_c).val).f2 != 0)goto _LL121;}}_LL120: {
struct _tuple16 _tmp830;return(_tmp830.f1=inflow,((_tmp830.f2=_tmp196->zero,_tmp830)));}_LL121: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1B9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1B9->tag != 0)goto _LL123;else{if(((_tmp1B9->f1).Int_c).tag != 5)goto _LL123;}}_LL122:
 goto _LL124;_LL123: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1BA->tag != 1)goto _LL125;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1BB=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp1BA->f2);if(_tmp1BB->tag != 2)goto _LL125;}}_LL124: {
struct _tuple16 _tmp831;return(_tmp831.f1=inflow,((_tmp831.f2=_tmp196->notzeroall,_tmp831)));}_LL125: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1BC->tag != 30)goto _LL127;else{if(_tmp1BC->f1 != 0)goto _LL127;}}_LL126:
# 1100
 goto _LL128;_LL127: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1BD->tag != 0)goto _LL129;}_LL128:
 goto _LL12A;_LL129: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1BE->tag != 17)goto _LL12B;}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1BF=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1BF->tag != 16)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1C0->tag != 18)goto _LL12F;}_LL12E:
 goto _LL130;_LL12F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1C1=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1C1->tag != 32)goto _LL131;}_LL130:
 goto _LL132;_LL131: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1C2->tag != 31)goto _LL133;}_LL132: {
struct _tuple16 _tmp832;return(_tmp832.f1=inflow,((_tmp832.f2=_tmp196->unknown_all,_tmp832)));}_LL133: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1C3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1C3->tag != 1)goto _LL135;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1C4=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp1C3->f2);if(_tmp1C4->tag != 1)goto _LL135;}}_LL134: {
# 1109
struct _tuple16 _tmp833;return(_tmp833.f1=inflow,((_tmp833.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),0,_tmp196->unknown_all),_tmp833)));}_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1C5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1C5->tag != 1)goto _LL137;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1C6=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp1C5->f2);if(_tmp1C6->tag != 3)goto _LL137;else{_tmp1C7=_tmp1C6->f1;}}}_LL136:
# 1112
 _tmp1CA=_tmp1C7;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1C8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1C8->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1C9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp1C8->f2);if(_tmp1C9->tag != 4)goto _LL139;else{_tmp1CA=_tmp1C9->f1;}}}_LL138:
 _tmp1CD=_tmp1CA;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1CB->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1CC=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp1CB->f2);if(_tmp1CC->tag != 5)goto _LL13B;else{_tmp1CD=_tmp1CC->f1;}}}_LL13A: {
# 1116
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp834;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp834.tag=0,((_tmp834.f1=_tmp1CD,_tmp834)));
struct _tuple16 _tmp835;return(_tmp835.f1=inflow,((_tmp835.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp835)));}_LL13B: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1CE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1CE->tag != 2)goto _LL13D;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CE->f2;}}_LL13C: {
# 1123
struct _tuple16 _tmp235=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1D0);union Cyc_CfFlowInfo_FlowInfo _tmp237;void*_tmp238;struct _tuple16 _tmp236=_tmp235;_tmp237=_tmp236.f1;_tmp238=_tmp236.f2;{
struct _tuple16 _tmp836;return(_tmp836.f1=_tmp237,((_tmp836.f2=_tmp238,_tmp836)));};}_LL13D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1D1=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1D1->tag != 4)goto _LL13F;else{_tmp1D2=_tmp1D1->f1;}}_LL13E: {
# 1127
struct Cyc_List_List _tmp837;struct Cyc_List_List _tmp23A=(_tmp837.hd=_tmp1D2,((_tmp837.tl=0,_tmp837)));
struct _tuple16 _tmp23B=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp23A);union Cyc_CfFlowInfo_FlowInfo _tmp23D;struct _tuple16 _tmp23C=_tmp23B;_tmp23D=_tmp23C.f1;{
struct _tuple18 _tmp23E=Cyc_NewControlFlow_anal_Lexp(env,_tmp23D,0,0,_tmp1D2);union Cyc_CfFlowInfo_AbsLVal _tmp240;struct _tuple18 _tmp23F=_tmp23E;_tmp240=_tmp23F.f2;
{struct _tuple25 _tmp838;struct _tuple25 _tmp241=(_tmp838.f1=_tmp240,((_tmp838.f2=_tmp23D,_tmp838)));struct _tuple25 _tmp242=_tmp241;struct Cyc_CfFlowInfo_Place*_tmp243;struct Cyc_Dict_Dict _tmp244;struct Cyc_List_List*_tmp245;_LL17E: if(((_tmp242.f1).PlaceL).tag != 1)goto _LL180;_tmp243=(struct Cyc_CfFlowInfo_Place*)((_tmp242.f1).PlaceL).val;if(((_tmp242.f2).ReachableFL).tag != 2)goto _LL180;_tmp244=((struct _tuple14)((_tmp242.f2).ReachableFL).val).f1;_tmp245=((struct _tuple14)((_tmp242.f2).ReachableFL).val).f2;_LL17F:
# 1132
 _tmp245=Cyc_Relations_reln_kill_exp(_tmp196->r,_tmp245,_tmp1D2);
_tmp23D=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp196,_tmp1D2->loc,_tmp244,env->all_changed,_tmp243,_tmp196->unknown_all),_tmp245);
# 1137
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}{
# 1140
struct _tuple16 _tmp839;return(_tmp839.f1=_tmp23D,((_tmp839.f2=_tmp196->unknown_all,_tmp839)));};};}_LL13F: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1D3=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1D3->tag != 3)goto _LL141;else{_tmp1D4=_tmp1D3->f1;if(_tmp1D3->f2 == 0)goto _LL141;_tmp1D5=_tmp1D3->f3;}}_LL140:
# 1143
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp83C;void*_tmp83B;(_tmp83B=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp83C="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp83C,sizeof(char),62))),_tag_dyneither(_tmp83B,sizeof(void*),0)));}{
struct _tuple16 _tmp83D;return(_tmp83D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp83D.f2=_tmp196->unknown_all,_tmp83D)));};}{
# 1147
struct Cyc_List_List _tmp83E;struct Cyc_List_List _tmp24C=(_tmp83E.hd=_tmp1D5,((_tmp83E.tl=0,_tmp83E)));
struct Cyc_List_List _tmp83F;struct Cyc_List_List _tmp24D=(_tmp83F.hd=_tmp1D4,((_tmp83F.tl=& _tmp24C,_tmp83F)));
struct _tuple16 _tmp24E=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp24D);union Cyc_CfFlowInfo_FlowInfo _tmp250;struct _tuple16 _tmp24F=_tmp24E;_tmp250=_tmp24F.f1;{
struct _tuple18 _tmp251=Cyc_NewControlFlow_anal_Lexp(env,_tmp250,0,0,_tmp1D4);union Cyc_CfFlowInfo_AbsLVal _tmp253;struct _tuple18 _tmp252=_tmp251;_tmp253=_tmp252.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp254=_tmp250;struct Cyc_Dict_Dict _tmp255;struct Cyc_List_List*_tmp256;_LL183: if((_tmp254.ReachableFL).tag != 2)goto _LL185;_tmp255=((struct _tuple14)(_tmp254.ReachableFL).val).f1;_tmp256=((struct _tuple14)(_tmp254.ReachableFL).val).f2;_LL184:
# 1153
{union Cyc_CfFlowInfo_AbsLVal _tmp257=_tmp253;struct Cyc_CfFlowInfo_Place*_tmp258;_LL188: if((_tmp257.PlaceL).tag != 1)goto _LL18A;_tmp258=(struct Cyc_CfFlowInfo_Place*)(_tmp257.PlaceL).val;_LL189:
# 1157
 _tmp256=Cyc_Relations_reln_kill_exp(_tmp196->r,_tmp256,_tmp1D4);
_tmp255=Cyc_CfFlowInfo_assign_place(_tmp196,_tmp1D4->loc,_tmp255,env->all_changed,_tmp258,_tmp196->unknown_all);
# 1160
_tmp250=Cyc_CfFlowInfo_ReachableFL(_tmp255,_tmp256);
# 1164
goto _LL187;_LL18A: if((_tmp257.UnknownL).tag != 2)goto _LL187;_LL18B:
# 1167
 goto _LL187;_LL187:;}
# 1169
goto _LL182;_LL185:;_LL186:
 goto _LL182;_LL182:;}{
# 1172
struct _tuple16 _tmp840;return(_tmp840.f1=_tmp250,((_tmp840.f2=_tmp196->unknown_all,_tmp840)));};};};_LL141: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1D6->tag != 3)goto _LL143;else{_tmp1D7=_tmp1D6->f1;if(_tmp1D6->f2 != 0)goto _LL143;_tmp1D8=_tmp1D6->f3;}}_LL142:
# 1176
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp843;void*_tmp842;(_tmp842=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp843="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp843,sizeof(char),62))),_tag_dyneither(_tmp842,sizeof(void*),0)));}{
struct _tuple16 _tmp844;return(_tmp844.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp844.f2=_tmp196->unknown_all,_tmp844)));};}{
# 1180
struct Cyc_Dict_Dict*_tmp25F=env->all_changed;
# 1182
{struct Cyc_Absyn_Exp*_tmp845[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp845[1]=_tmp1D8,((_tmp845[0]=_tmp1D7,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp845,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp846;env->all_changed=((_tmp846=_region_malloc(env->r,sizeof(*_tmp846)),((_tmp846[0]=_tmp196->mt_place_set,_tmp846))));}{
struct _tuple18 _tmp262=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1D7);union Cyc_CfFlowInfo_FlowInfo _tmp264;union Cyc_CfFlowInfo_AbsLVal _tmp265;struct _tuple18 _tmp263=_tmp262;_tmp264=_tmp263.f1;_tmp265=_tmp263.f2;{
struct Cyc_Dict_Dict _tmp266=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp847;env->all_changed=((_tmp847=_region_malloc(env->r,sizeof(*_tmp847)),((_tmp847[0]=_tmp196->mt_place_set,_tmp847))));}{
struct _tuple16 _tmp268=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D8);union Cyc_CfFlowInfo_FlowInfo _tmp26A;void*_tmp26B;struct _tuple16 _tmp269=_tmp268;_tmp26A=_tmp269.f1;_tmp26B=_tmp269.f2;{
struct Cyc_Dict_Dict _tmp26C=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp26D=Cyc_CfFlowInfo_after_flow(_tmp196,& _tmp266,_tmp264,_tmp26A,_tmp266,_tmp26C);
# 1193
union Cyc_CfFlowInfo_FlowInfo _tmp26E=Cyc_CfFlowInfo_join_flow(_tmp196,_tmp25F,inflow,_tmp26D);
# 1196
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp26E,inflow)){
if(_tmp25F == 0)
env->all_changed=0;else{
# 1200
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp25F,
Cyc_CfFlowInfo_union_place_set(_tmp266,_tmp26C,0),0);}
# 1203
return Cyc_NewControlFlow_do_assign(_tmp196,env,_tmp26D,_tmp1D7,_tmp265,_tmp1D8,_tmp26B,e->loc);}
# 1206
inflow=_tmp26E;};};};};}};_LL143: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1D9=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1D9->tag != 8)goto _LL145;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}}_LL144: {
# 1211
struct _tuple16 _tmp26F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DA);union Cyc_CfFlowInfo_FlowInfo _tmp271;void*_tmp272;struct _tuple16 _tmp270=_tmp26F;_tmp271=_tmp270.f1;_tmp272=_tmp270.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp271,_tmp1DB);}_LL145: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1DC=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1DC->tag != 10)goto _LL147;else{_tmp1DD=_tmp1DC->f1;}}_LL146: {
# 1215
struct _tuple16 _tmp273=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1DD);union Cyc_CfFlowInfo_FlowInfo _tmp275;void*_tmp276;struct _tuple16 _tmp274=_tmp273;_tmp275=_tmp274.f1;_tmp276=_tmp274.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1DD->loc,_tmp275,_tmp276);{
struct _tuple16 _tmp848;return(_tmp848.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp848.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),0,_tmp196->unknown_all),_tmp848)));};}_LL147: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1DE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1DE->tag != 9)goto _LL149;else{_tmp1DF=_tmp1DE->f1;_tmp1E0=_tmp1DE->f2;}}_LL148: {
# 1220
struct _RegionHandle _tmp278=_new_region("temp");struct _RegionHandle*temp=& _tmp278;_push_region(temp);
{struct Cyc_List_List*_tmp279=_tmp1E0;
struct Cyc_List_List*_tmp849;struct _tuple23 _tmp27A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp849=_region_malloc(temp,sizeof(*_tmp849)),((_tmp849->hd=_tmp1DF,((_tmp849->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1E0),_tmp849)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp27C;struct Cyc_List_List*_tmp27D;struct _tuple23 _tmp27B=_tmp27A;_tmp27C=_tmp27B.f1;_tmp27D=_tmp27B.f2;{
# 1224
union Cyc_CfFlowInfo_FlowInfo _tmp27E=Cyc_NewControlFlow_use_Rval(env,_tmp1DF->loc,_tmp27C,(void*)((struct Cyc_List_List*)_check_null(_tmp27D))->hd);
_tmp27D=_tmp27D->tl;{
# 1227
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp27F=Cyc_Tcutil_compress((void*)_check_null(_tmp1DF->topt));void*_tmp280=_tmp27F;void*_tmp282;_LL18D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp281=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp280;if(_tmp281->tag != 5)goto _LL18F;else{_tmp282=(_tmp281->f1).elt_typ;}}_LL18E:
# 1234
{void*_tmp283=Cyc_Tcutil_compress(_tmp282);void*_tmp284=_tmp283;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp287;struct Cyc_List_List*_tmp288;_LL192: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp285=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp284;if(_tmp285->tag != 9)goto _LL194;else{_tmp286=(_tmp285->f1).attributes;_tmp287=(_tmp285->f1).requires_relns;_tmp288=(_tmp285->f1).ensures_relns;}}_LL193:
# 1236
 requires=_tmp287;
ensures=_tmp288;
for(0;_tmp286 != 0;_tmp286=_tmp286->tl){
# 1240
void*_tmp289=(void*)_tmp286->hd;void*_tmp28A=_tmp289;int _tmp28C;int _tmp28E;int _tmp290;_LL197: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp28B=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp28A;if(_tmp28B->tag != 20)goto _LL199;else{_tmp28C=_tmp28B->f1;}}_LL198:
# 1242
{struct Cyc_List_List*_tmp84A;init_params=((_tmp84A=_region_malloc(temp,sizeof(*_tmp84A)),((_tmp84A->hd=(void*)_tmp28C,((_tmp84A->tl=init_params,_tmp84A))))));}goto _LL196;_LL199: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp28D=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp28A;if(_tmp28D->tag != 21)goto _LL19B;else{_tmp28E=_tmp28D->f1;}}_LL19A:
# 1244
{struct Cyc_List_List*_tmp84B;nolive_unique_params=((_tmp84B=_region_malloc(temp,sizeof(*_tmp84B)),((_tmp84B->hd=(void*)_tmp28E,((_tmp84B->tl=nolive_unique_params,_tmp84B))))));}
goto _LL196;_LL19B: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp28F=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp28A;if(_tmp28F->tag != 22)goto _LL19D;else{_tmp290=_tmp28F->f1;}}_LL19C:
# 1248
{struct Cyc_List_List*_tmp84C;noconsume_params=((_tmp84C=_region_malloc(temp,sizeof(*_tmp84C)),((_tmp84C->hd=(void*)_tmp290,((_tmp84C->tl=noconsume_params,_tmp84C))))));}
goto _LL196;_LL19D:;_LL19E:
 goto _LL196;_LL196:;}
# 1252
goto _LL191;_LL194:;_LL195: {
const char*_tmp84F;void*_tmp84E;(_tmp84E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp84F="anal_Rexp: bad function type",_tag_dyneither(_tmp84F,sizeof(char),29))),_tag_dyneither(_tmp84E,sizeof(void*),0)));}_LL191:;}
# 1255
goto _LL18C;_LL18F:;_LL190: {
const char*_tmp852;void*_tmp851;(_tmp851=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp852="anal_Rexp: bad function type",_tag_dyneither(_tmp852,sizeof(char),29))),_tag_dyneither(_tmp851,sizeof(void*),0)));}_LL18C:;}
# 1258
{int i=1;for(0;_tmp27D != 0;(((_tmp27D=_tmp27D->tl,_tmp1E0=((struct Cyc_List_List*)_check_null(_tmp1E0))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp298=_tmp27C;struct Cyc_Dict_Dict _tmp299;_LL1A0: if((_tmp298.BottomFL).tag != 1)goto _LL1A2;_LL1A1:
 goto _LL19F;_LL1A2: if((_tmp298.ReachableFL).tag != 2)goto _LL19F;_tmp299=((struct _tuple14)(_tmp298.ReachableFL).val).f1;_LL1A3:
# 1263
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp299,(void*)_tmp27D->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp855;void*_tmp854;(_tmp854=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E0))->hd)->loc,((_tmp855="expression may not be initialized",_tag_dyneither(_tmp855,sizeof(char),34))),_tag_dyneither(_tmp854,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp29C=_tmp27E;struct Cyc_Dict_Dict _tmp29D;struct Cyc_List_List*_tmp29E;_LL1A5: if((_tmp29C.BottomFL).tag != 1)goto _LL1A7;_LL1A6:
 goto _LL1A4;_LL1A7: if((_tmp29C.ReachableFL).tag != 2)goto _LL1A4;_tmp29D=((struct _tuple14)(_tmp29C.ReachableFL).val).f1;_tmp29E=((struct _tuple14)(_tmp29C.ReachableFL).val).f2;_LL1A8: {
# 1270
struct Cyc_Dict_Dict _tmp29F=Cyc_CfFlowInfo_escape_deref(_tmp196,_tmp29D,env->all_changed,(void*)_tmp27D->hd);
{void*_tmp2A0=(void*)_tmp27D->hd;void*_tmp2A1=_tmp2A0;struct Cyc_CfFlowInfo_Place*_tmp2A3;_LL1AA: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2A2=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2A1;if(_tmp2A2->tag != 5)goto _LL1AC;else{_tmp2A3=_tmp2A2->f1;}}_LL1AB:
# 1273
{void*_tmp2A4=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E0))->hd)->topt));void*_tmp2A5=_tmp2A4;void*_tmp2A7;_LL1AF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A5;if(_tmp2A6->tag != 5)goto _LL1B1;else{_tmp2A7=(_tmp2A6->f1).elt_typ;}}_LL1B0:
# 1275
 _tmp29F=Cyc_CfFlowInfo_assign_place(_tmp196,((struct Cyc_Absyn_Exp*)_tmp1E0->hd)->loc,_tmp29F,env->all_changed,_tmp2A3,
# 1277
Cyc_CfFlowInfo_typ_to_absrval(_tmp196,_tmp2A7,0,_tmp196->esc_all));
# 1280
goto _LL1AE;_LL1B1:;_LL1B2: {
const char*_tmp858;void*_tmp857;(_tmp857=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp858="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp858,sizeof(char),39))),_tag_dyneither(_tmp857,sizeof(void*),0)));}_LL1AE:;}
# 1283
goto _LL1A9;_LL1AC:;_LL1AD:
 goto _LL1A9;_LL1A9:;}
# 1286
_tmp27E=Cyc_CfFlowInfo_ReachableFL(_tmp29F,_tmp29E);
goto _LL1A4;}_LL1A4:;}
# 1289
goto _LL19F;_LL19F:;}else{
# 1292
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1297
_tmp27E=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E0))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E0))->hd)->topt),_tmp27E,(void*)_tmp27D->hd);else{
# 1307
_tmp27E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E0))->hd)->loc,_tmp27E,(void*)_tmp27D->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp1E0->hd)->topt),0))
_tmp27E=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp27E,(struct Cyc_Absyn_Exp*)_tmp1E0->hd,((struct Cyc_Absyn_Exp*)_tmp1E0->hd)->loc,(void*)_tmp27D->hd);}}}}
# 1314
Cyc_NewControlFlow_check_fn_requires(env,_tmp27E,_tmp279,requires,e->loc);
# 1317
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1DF->topt))){
struct _tuple16 _tmp859;struct _tuple16 _tmp2AB=(_tmp859.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp859.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),0,_tmp196->unknown_all),_tmp859)));_npop_handler(0);return _tmp2AB;}else{
# 1320
struct _tuple16 _tmp85A;struct _tuple16 _tmp2AD=(_tmp85A.f1=_tmp27E,((_tmp85A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),0,_tmp196->unknown_all),_tmp85A)));_npop_handler(0);return _tmp2AD;}};};}
# 1221
;_pop_region(temp);}_LL149: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1E1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1E1->tag != 33)goto _LL14B;else{_tmp1E2=(_tmp1E1->f1).is_calloc;_tmp1E3=(_tmp1E1->f1).rgn;_tmp1E4=(_tmp1E1->f1).elt_type;_tmp1E5=(_tmp1E1->f1).num_elts;_tmp1E6=(_tmp1E1->f1).fat_result;}}_LL14A: {
# 1323
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp85D;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp85C;void*root=(void*)((_tmp85C=_region_malloc(_tmp196->r,sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp85D.tag=1,((_tmp85D.f1=_tmp1E5,((_tmp85D.f2=(void*)_check_null(e->topt),_tmp85D)))))),_tmp85C))));
struct Cyc_CfFlowInfo_Place*_tmp85E;struct Cyc_CfFlowInfo_Place*place=(_tmp85E=_region_malloc(_tmp196->r,sizeof(*_tmp85E)),((_tmp85E->root=root,((_tmp85E->fields=0,_tmp85E)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp861;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp860;void*rval=(void*)((_tmp860=_region_malloc(_tmp196->r,sizeof(*_tmp860)),((_tmp860[0]=((_tmp861.tag=5,((_tmp861.f1=place,_tmp861)))),_tmp860))));
void*place_val;
if(_tmp1E6)place_val=_tmp196->notzeroall;else{
if(_tmp1E2)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,*((void**)_check_null(_tmp1E4)),0,_tmp196->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,*((void**)_check_null(_tmp1E4)),0,_tmp196->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1E3 != 0){
struct _RegionHandle _tmp2AF=_new_region("temp");struct _RegionHandle*temp=& _tmp2AF;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp862[2];struct _tuple23 _tmp2B0=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp862[1]=_tmp1E5,((_tmp862[0]=_tmp1E3,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp862,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1334
union Cyc_CfFlowInfo_FlowInfo _tmp2B2;struct Cyc_List_List*_tmp2B3;struct _tuple23 _tmp2B1=_tmp2B0;_tmp2B2=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;
# 1336
outflow=_tmp2B2;}
# 1334
;_pop_region(temp);}else{
# 1339
struct _tuple16 _tmp2B5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1E5);union Cyc_CfFlowInfo_FlowInfo _tmp2B7;struct _tuple16 _tmp2B6=_tmp2B5;_tmp2B7=_tmp2B6.f1;
outflow=_tmp2B7;}{
# 1342
union Cyc_CfFlowInfo_FlowInfo _tmp2B8=outflow;struct Cyc_Dict_Dict _tmp2B9;struct Cyc_List_List*_tmp2BA;_LL1B4: if((_tmp2B8.BottomFL).tag != 1)goto _LL1B6;_LL1B5: {
struct _tuple16 _tmp863;return(_tmp863.f1=outflow,((_tmp863.f2=rval,_tmp863)));}_LL1B6: if((_tmp2B8.ReachableFL).tag != 2)goto _LL1B3;_tmp2B9=((struct _tuple14)(_tmp2B8.ReachableFL).val).f1;_tmp2BA=((struct _tuple14)(_tmp2B8.ReachableFL).val).f2;_LL1B7: {
# 1345
struct _tuple16 _tmp864;return(_tmp864.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2B9,root,place_val),_tmp2BA),((_tmp864.f2=rval,_tmp864)));}_LL1B3:;};};}_LL14B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1E7=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1E7->tag != 34)goto _LL14D;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}}_LL14C: {
# 1349
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1355
struct _RegionHandle _tmp2C2=_new_region("temp");struct _RegionHandle*temp=& _tmp2C2;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp865[2];struct _tuple23 _tmp2C3=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp865[1]=_tmp1E9,((_tmp865[0]=_tmp1E8,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp865,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1356
union Cyc_CfFlowInfo_FlowInfo _tmp2C5;struct Cyc_List_List*_tmp2C6;struct _tuple23 _tmp2C4=_tmp2C3;_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;
# 1358
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2C6))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2C6->tl))->hd;
outflow=_tmp2C5;}{
# 1363
struct _tuple18 _tmp2C8=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1E8);union Cyc_CfFlowInfo_AbsLVal _tmp2CA;struct _tuple18 _tmp2C9=_tmp2C8;_tmp2CA=_tmp2C9.f2;{
struct _tuple18 _tmp2CB=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1E9);union Cyc_CfFlowInfo_AbsLVal _tmp2CD;struct _tuple18 _tmp2CC=_tmp2CB;_tmp2CD=_tmp2CC.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp2CE=outflow;struct Cyc_Dict_Dict _tmp2CF;struct Cyc_List_List*_tmp2D0;_LL1B9: if((_tmp2CE.ReachableFL).tag != 2)goto _LL1BB;_tmp2CF=((struct _tuple14)(_tmp2CE.ReachableFL).val).f1;_tmp2D0=((struct _tuple14)(_tmp2CE.ReachableFL).val).f2;_LL1BA:
# 1367
{union Cyc_CfFlowInfo_AbsLVal _tmp2D1=_tmp2CA;struct Cyc_CfFlowInfo_Place*_tmp2D2;_LL1BE: if((_tmp2D1.PlaceL).tag != 1)goto _LL1C0;_tmp2D2=(struct Cyc_CfFlowInfo_Place*)(_tmp2D1.PlaceL).val;_LL1BF:
# 1369
 _tmp2CF=Cyc_CfFlowInfo_assign_place(_tmp196,_tmp1E8->loc,_tmp2CF,env->all_changed,_tmp2D2,right_rval);
# 1371
goto _LL1BD;_LL1C0: if((_tmp2D1.UnknownL).tag != 2)goto _LL1BD;_LL1C1:
# 1376
 if(Cyc_CfFlowInfo_initlevel(_tmp196,_tmp2CF,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp868;void*_tmp867;(_tmp867=0,Cyc_Tcutil_terr(_tmp1E9->loc,((_tmp868="expression may not be fully initialized",_tag_dyneither(_tmp868,sizeof(char),40))),_tag_dyneither(_tmp867,sizeof(void*),0)));}
goto _LL1BD;_LL1BD:;}
# 1380
{union Cyc_CfFlowInfo_AbsLVal _tmp2D5=_tmp2CD;struct Cyc_CfFlowInfo_Place*_tmp2D6;_LL1C3: if((_tmp2D5.PlaceL).tag != 1)goto _LL1C5;_tmp2D6=(struct Cyc_CfFlowInfo_Place*)(_tmp2D5.PlaceL).val;_LL1C4:
# 1382
 _tmp2CF=Cyc_CfFlowInfo_assign_place(_tmp196,_tmp1E9->loc,_tmp2CF,env->all_changed,_tmp2D6,left_rval);
# 1384
goto _LL1C2;_LL1C5: if((_tmp2D5.UnknownL).tag != 2)goto _LL1C2;_LL1C6:
# 1386
 if(Cyc_CfFlowInfo_initlevel(_tmp196,_tmp2CF,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp86B;void*_tmp86A;(_tmp86A=0,Cyc_Tcutil_terr(_tmp1E8->loc,((_tmp86B="expression may not be fully initialized",_tag_dyneither(_tmp86B,sizeof(char),40))),_tag_dyneither(_tmp86A,sizeof(void*),0)));}
goto _LL1C2;_LL1C2:;}
# 1391
_tmp2D0=Cyc_Relations_reln_kill_exp(_tmp196->r,_tmp2D0,_tmp1E8);
_tmp2D0=Cyc_Relations_reln_kill_exp(_tmp196->r,_tmp2D0,_tmp1E9);
# 1394
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2CF,_tmp2D0);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL1B8;_LL1BB:;_LL1BC:
 goto _LL1B8;_LL1B8:;}{
# 1401
struct _tuple16 _tmp86C;struct _tuple16 _tmp2DA=(_tmp86C.f1=outflow,((_tmp86C.f2=_tmp196->unknown_all,_tmp86C)));_npop_handler(0);return _tmp2DA;};};};
# 1355
;_pop_region(temp);}_LL14D: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1EA=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1EA->tag != 15)goto _LL14F;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;}}_LL14E: {
# 1404
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp86F;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp86E;void*root=(void*)((_tmp86E=_region_malloc(_tmp196->r,sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp86F.tag=1,((_tmp86F.f1=_tmp1EC,((_tmp86F.f2=(void*)_check_null(e->topt),_tmp86F)))))),_tmp86E))));
struct Cyc_CfFlowInfo_Place*_tmp870;struct Cyc_CfFlowInfo_Place*place=(_tmp870=_region_malloc(_tmp196->r,sizeof(*_tmp870)),((_tmp870->root=root,((_tmp870->fields=0,_tmp870)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp873;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp872;void*rval=(void*)((_tmp872=_region_malloc(_tmp196->r,sizeof(*_tmp872)),((_tmp872[0]=((_tmp873.tag=5,((_tmp873.f1=place,_tmp873)))),_tmp872))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1EB != 0){
struct _RegionHandle _tmp2DB=_new_region("temp");struct _RegionHandle*temp=& _tmp2DB;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp874[2];struct _tuple23 _tmp2DC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp874[1]=_tmp1EC,((_tmp874[0]=_tmp1EB,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp874,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1412
union Cyc_CfFlowInfo_FlowInfo _tmp2DE;struct Cyc_List_List*_tmp2DF;struct _tuple23 _tmp2DD=_tmp2DC;_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;
# 1414
outflow=_tmp2DE;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2DF))->tl))->hd;}
# 1412
;_pop_region(temp);}else{
# 1418
struct _tuple16 _tmp2E1=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1EC);union Cyc_CfFlowInfo_FlowInfo _tmp2E3;void*_tmp2E4;struct _tuple16 _tmp2E2=_tmp2E1;_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;
outflow=_tmp2E3;
place_val=_tmp2E4;}{
# 1422
union Cyc_CfFlowInfo_FlowInfo _tmp2E5=outflow;struct Cyc_Dict_Dict _tmp2E6;struct Cyc_List_List*_tmp2E7;_LL1C8: if((_tmp2E5.BottomFL).tag != 1)goto _LL1CA;_LL1C9: {
struct _tuple16 _tmp875;return(_tmp875.f1=outflow,((_tmp875.f2=rval,_tmp875)));}_LL1CA: if((_tmp2E5.ReachableFL).tag != 2)goto _LL1C7;_tmp2E6=((struct _tuple14)(_tmp2E5.ReachableFL).val).f1;_tmp2E7=((struct _tuple14)(_tmp2E5.ReachableFL).val).f2;_LL1CB: {
# 1425
struct _tuple16 _tmp876;return(_tmp876.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2E6,root,place_val),_tmp2E7),((_tmp876.f2=rval,_tmp876)));}_LL1C7:;};};}_LL14F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1ED=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1ED->tag != 14)goto _LL151;else{_tmp1EE=_tmp1ED->f1;}}_LL150: {
# 1429
struct _tuple18 _tmp2EF=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1EE);union Cyc_CfFlowInfo_FlowInfo _tmp2F1;union Cyc_CfFlowInfo_AbsLVal _tmp2F2;struct _tuple18 _tmp2F0=_tmp2EF;_tmp2F1=_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp2F3=_tmp2F2;struct Cyc_CfFlowInfo_Place*_tmp2F4;_LL1CD: if((_tmp2F3.UnknownL).tag != 2)goto _LL1CF;_LL1CE: {
struct _tuple16 _tmp877;return(_tmp877.f1=_tmp2F1,((_tmp877.f2=_tmp196->notzeroall,_tmp877)));}_LL1CF: if((_tmp2F3.PlaceL).tag != 1)goto _LL1CC;_tmp2F4=(struct Cyc_CfFlowInfo_Place*)(_tmp2F3.PlaceL).val;_LL1D0: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp87D;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp87C;struct _tuple16 _tmp87B;return(_tmp87B.f1=_tmp2F1,((_tmp87B.f2=(void*)((_tmp87D=_region_malloc(env->r,sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp87C.tag=5,((_tmp87C.f1=_tmp2F4,_tmp87C)))),_tmp87D)))),_tmp87B)));}_LL1CC:;};}_LL151: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1EF=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1EF->tag != 19)goto _LL153;else{_tmp1F0=_tmp1EF->f1;}}_LL152: {
# 1436
struct _tuple16 _tmp2F9=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1F0);union Cyc_CfFlowInfo_FlowInfo _tmp2FB;void*_tmp2FC;struct _tuple16 _tmp2FA=_tmp2F9;_tmp2FB=_tmp2FA.f1;_tmp2FC=_tmp2FA.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2FB,_tmp1F0,_tmp2FC);}_LL153: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1F1=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1F1->tag != 20)goto _LL155;else{_tmp1F2=_tmp1F1->f1;_tmp1F3=_tmp1F1->f2;}}_LL154: {
# 1440
struct _tuple16 _tmp2FD=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1F2);union Cyc_CfFlowInfo_FlowInfo _tmp2FF;void*_tmp300;struct _tuple16 _tmp2FE=_tmp2FD;_tmp2FF=_tmp2FE.f1;_tmp300=_tmp2FE.f2;
if(_tmp1F2->topt == 0){
{const char*_tmp881;void*_tmp880[1];struct Cyc_String_pa_PrintArg_struct _tmp87F;(_tmp87F.tag=0,((_tmp87F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp880[0]=& _tmp87F,Cyc_fprintf(Cyc_stderr,((_tmp881="aggrmember exp %s\n",_tag_dyneither(_tmp881,sizeof(char),19))),_tag_dyneither(_tmp880,sizeof(void*),1)))))));}{
const char*_tmp885;void*_tmp884[1];struct Cyc_String_pa_PrintArg_struct _tmp883;(_tmp883.tag=0,((_tmp883.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1F2)),((_tmp884[0]=& _tmp883,Cyc_fprintf(Cyc_stderr,((_tmp885="oops! %s.topt = null!\n",_tag_dyneither(_tmp885,sizeof(char),23))),_tag_dyneither(_tmp884,sizeof(void*),1)))))));};}{
# 1445
void*_tmp307=(void*)_check_null(_tmp1F2->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp307)){
# 1448
struct _tuple16 _tmp886;return(_tmp886.f1=_tmp2FF,((_tmp886.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),0,_tmp196->unknown_all),_tmp886)));}
# 1450
if(Cyc_Absyn_is_require_union_type(_tmp307))
Cyc_NewControlFlow_check_union_requires(_tmp1F2->loc,_tmp196->r,_tmp307,_tmp1F3,_tmp2FF);{
# 1453
struct _tuple15 _tmp309=Cyc_CfFlowInfo_unname_rval(_tmp196->r,_tmp300);void*_tmp30B;struct _tuple15 _tmp30A=_tmp309;_tmp30B=_tmp30A.f1;{
void*_tmp30C=_tmp30B;int _tmp30E;int _tmp30F;struct _dyneither_ptr _tmp310;_LL1D2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30C;if(_tmp30D->tag != 6)goto _LL1D4;else{_tmp30E=(_tmp30D->f1).is_union;_tmp30F=(_tmp30D->f1).fieldnum;_tmp310=_tmp30D->f2;}}_LL1D3: {
# 1456
int _tmp311=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1F2->topt),_tmp1F3);
if((_tmp30E  && _tmp30F != - 1) && _tmp30F != _tmp311){
struct _tuple16 _tmp887;return(_tmp887.f1=_tmp2FF,((_tmp887.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),1,_tmp196->unknown_none),_tmp887)));}{
struct _tuple16 _tmp888;return(_tmp888.f1=_tmp2FF,((_tmp888.f2=*((void**)_check_dyneither_subscript(_tmp310,sizeof(void*),_tmp311)),_tmp888)));};}_LL1D4:;_LL1D5:
# 1461
{const char*_tmp88B;void*_tmp88A;(_tmp88A=0,Cyc_fprintf(Cyc_stderr,((_tmp88B="the bad rexp is :",_tag_dyneither(_tmp88B,sizeof(char),18))),_tag_dyneither(_tmp88A,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp30B);
{const char*_tmp88E;void*_tmp88D;(_tmp88D=0,Cyc_fprintf(Cyc_stderr,((_tmp88E="\n",_tag_dyneither(_tmp88E,sizeof(char),2))),_tag_dyneither(_tmp88D,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp89B;const char*_tmp89A;void*_tmp899[1];struct Cyc_String_pa_PrintArg_struct _tmp898;struct Cyc_Core_Impossible_exn_struct*_tmp897;(int)_throw((void*)((_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897[0]=((_tmp89B.tag=Cyc_Core_Impossible,((_tmp89B.f1=(struct _dyneither_ptr)((_tmp898.tag=0,((_tmp898.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp899[0]=& _tmp898,Cyc_aprintf(((_tmp89A="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp89A,sizeof(char),26))),_tag_dyneither(_tmp899,sizeof(void*),1)))))))),_tmp89B)))),_tmp897)))));};_LL1D1:;};};};}_LL155: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1F4=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1F4->tag != 37)goto _LL157;else{_tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;}}_LL156: {
# 1471
struct _tuple16 _tmp31D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1F5);union Cyc_CfFlowInfo_FlowInfo _tmp31F;void*_tmp320;struct _tuple16 _tmp31E=_tmp31D;_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;
# 1473
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1F5->topt))){
struct _tuple16 _tmp89C;return(_tmp89C.f1=_tmp31F,((_tmp89C.f2=_tmp196->unknown_all,_tmp89C)));}{
struct _tuple15 _tmp322=Cyc_CfFlowInfo_unname_rval(_tmp196->r,_tmp320);void*_tmp324;struct _tuple15 _tmp323=_tmp322;_tmp324=_tmp323.f1;{
void*_tmp325=_tmp324;int _tmp327;int _tmp328;struct _dyneither_ptr _tmp329;_LL1D7: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp326=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp325;if(_tmp326->tag != 6)goto _LL1D9;else{_tmp327=(_tmp326->f1).is_union;_tmp328=(_tmp326->f1).fieldnum;_tmp329=_tmp326->f2;}}_LL1D8: {
# 1478
int _tmp32A=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1F5->topt),_tmp1F6);
if(_tmp327  && _tmp328 != - 1){
if(_tmp328 != _tmp32A){
struct _tuple16 _tmp89D;return(_tmp89D.f1=_tmp31F,((_tmp89D.f2=_tmp196->zero,_tmp89D)));}else{
# 1483
struct _tuple16 _tmp89E;return(_tmp89E.f1=_tmp31F,((_tmp89E.f2=_tmp196->notzeroall,_tmp89E)));}}else{
# 1485
struct _tuple16 _tmp89F;return(_tmp89F.f1=_tmp31F,((_tmp89F.f2=_tmp196->unknown_all,_tmp89F)));}}_LL1D9:;_LL1DA: {
# 1487
struct Cyc_Core_Impossible_exn_struct _tmp8AC;const char*_tmp8AB;void*_tmp8AA[1];struct Cyc_String_pa_PrintArg_struct _tmp8A9;struct Cyc_Core_Impossible_exn_struct*_tmp8A8;(int)_throw((void*)((_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8AC.tag=Cyc_Core_Impossible,((_tmp8AC.f1=(struct _dyneither_ptr)((_tmp8A9.tag=0,((_tmp8A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp8AA[0]=& _tmp8A9,Cyc_aprintf(((_tmp8AB="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp8AB,sizeof(char),26))),_tag_dyneither(_tmp8AA,sizeof(void*),1)))))))),_tmp8AC)))),_tmp8A8)))));}_LL1D6:;};};}_LL157: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1F7=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1F7->tag != 21)goto _LL159;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F7->f2;}}_LL158: {
# 1492
struct _tuple16 _tmp333=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1F8);union Cyc_CfFlowInfo_FlowInfo _tmp335;void*_tmp336;struct _tuple16 _tmp334=_tmp333;_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;{
# 1495
struct _tuple16 _tmp337=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp335,_tmp1F8,_tmp336);union Cyc_CfFlowInfo_FlowInfo _tmp339;void*_tmp33A;struct _tuple16 _tmp338=_tmp337;_tmp339=_tmp338.f1;_tmp33A=_tmp338.f2;{
# 1498
void*_tmp33B=Cyc_Tcutil_compress((void*)_check_null(_tmp1F8->topt));void*_tmp33C=_tmp33B;void*_tmp33E;_LL1DC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33C;if(_tmp33D->tag != 5)goto _LL1DE;else{_tmp33E=(_tmp33D->f1).elt_typ;}}_LL1DD:
# 1500
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp33E)){
# 1502
struct _tuple16 _tmp8AD;return(_tmp8AD.f1=_tmp339,((_tmp8AD.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),0,_tmp196->unknown_all),_tmp8AD)));}
# 1504
if(Cyc_Absyn_is_require_union_type(_tmp33E))
Cyc_NewControlFlow_check_union_requires(_tmp1F8->loc,_tmp196->r,_tmp33E,_tmp1F9,_tmp335);{
# 1507
struct _tuple15 _tmp340=Cyc_CfFlowInfo_unname_rval(_tmp196->r,_tmp33A);void*_tmp342;struct _tuple15 _tmp341=_tmp340;_tmp342=_tmp341.f1;{
void*_tmp343=_tmp342;int _tmp345;int _tmp346;struct _dyneither_ptr _tmp347;_LL1E1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp344=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp343;if(_tmp344->tag != 6)goto _LL1E3;else{_tmp345=(_tmp344->f1).is_union;_tmp346=(_tmp344->f1).fieldnum;_tmp347=_tmp344->f2;}}_LL1E2: {
# 1510
int _tmp348=Cyc_CfFlowInfo_get_field_index(_tmp33E,_tmp1F9);
if((_tmp345  && _tmp346 != - 1) && _tmp346 != _tmp348){
struct _tuple16 _tmp8AE;return(_tmp8AE.f1=_tmp339,((_tmp8AE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),1,_tmp196->unknown_none),_tmp8AE)));}{
struct _tuple16 _tmp8AF;return(_tmp8AF.f1=_tmp339,((_tmp8AF.f2=*((void**)_check_dyneither_subscript(_tmp347,sizeof(void*),_tmp348)),_tmp8AF)));};}_LL1E3:;_LL1E4: {
struct Cyc_Core_Impossible_exn_struct _tmp8B5;const char*_tmp8B4;struct Cyc_Core_Impossible_exn_struct*_tmp8B3;(int)_throw((void*)((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=((_tmp8B5.tag=Cyc_Core_Impossible,((_tmp8B5.f1=((_tmp8B4="anal_Rexp: AggrArrow",_tag_dyneither(_tmp8B4,sizeof(char),21))),_tmp8B5)))),_tmp8B3)))));}_LL1E0:;};};_LL1DE:;_LL1DF: {
# 1516
struct Cyc_Core_Impossible_exn_struct _tmp8BB;const char*_tmp8BA;struct Cyc_Core_Impossible_exn_struct*_tmp8B9;(int)_throw((void*)((_tmp8B9=_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9[0]=((_tmp8BB.tag=Cyc_Core_Impossible,((_tmp8BB.f1=((_tmp8BA="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8BA,sizeof(char),25))),_tmp8BB)))),_tmp8B9)))));}_LL1DB:;};};}_LL159: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1FA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1FA->tag != 5)goto _LL15B;else{_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FA->f2;_tmp1FD=_tmp1FA->f3;}}_LL15A: {
# 1520
struct _tuple19 _tmp351=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1FB);union Cyc_CfFlowInfo_FlowInfo _tmp353;union Cyc_CfFlowInfo_FlowInfo _tmp354;struct _tuple19 _tmp352=_tmp351;_tmp353=_tmp352.f1;_tmp354=_tmp352.f2;{
struct _tuple16 _tmp355=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp353,_tmp1FC);
struct _tuple16 _tmp356=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp354,_tmp1FD);
# 1524
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp196,env->all_changed,_tmp355,_tmp356);};}_LL15B: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1FE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp1FE->tag != 6)goto _LL15D;else{_tmp1FF=_tmp1FE->f1;_tmp200=_tmp1FE->f2;}}_LL15C: {
# 1527
struct _tuple19 _tmp357=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1FF);union Cyc_CfFlowInfo_FlowInfo _tmp359;union Cyc_CfFlowInfo_FlowInfo _tmp35A;struct _tuple19 _tmp358=_tmp357;_tmp359=_tmp358.f1;_tmp35A=_tmp358.f2;{
struct _tuple16 _tmp35B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp359,_tmp200);union Cyc_CfFlowInfo_FlowInfo _tmp35D;void*_tmp35E;struct _tuple16 _tmp35C=_tmp35B;_tmp35D=_tmp35C.f1;_tmp35E=_tmp35C.f2;{
struct _tuple16 _tmp8BC;struct _tuple16 _tmp35F=(_tmp8BC.f1=_tmp35D,((_tmp8BC.f2=_tmp35E,_tmp8BC)));
struct _tuple16 _tmp8BD;struct _tuple16 _tmp360=(_tmp8BD.f1=_tmp35A,((_tmp8BD.f2=_tmp196->zero,_tmp8BD)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp196,env->all_changed,_tmp35F,_tmp360);};};}_LL15D: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp201=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp201->tag != 7)goto _LL15F;else{_tmp202=_tmp201->f1;_tmp203=_tmp201->f2;}}_LL15E: {
# 1534
struct _tuple19 _tmp363=Cyc_NewControlFlow_anal_test(env,inflow,_tmp202);union Cyc_CfFlowInfo_FlowInfo _tmp365;union Cyc_CfFlowInfo_FlowInfo _tmp366;struct _tuple19 _tmp364=_tmp363;_tmp365=_tmp364.f1;_tmp366=_tmp364.f2;{
struct _tuple16 _tmp367=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp366,_tmp203);union Cyc_CfFlowInfo_FlowInfo _tmp369;void*_tmp36A;struct _tuple16 _tmp368=_tmp367;_tmp369=_tmp368.f1;_tmp36A=_tmp368.f2;{
struct _tuple16 _tmp8BE;struct _tuple16 _tmp36B=(_tmp8BE.f1=_tmp369,((_tmp8BE.f2=_tmp36A,_tmp8BE)));
struct _tuple16 _tmp8BF;struct _tuple16 _tmp36C=(_tmp8BF.f1=_tmp365,((_tmp8BF.f2=_tmp196->notzeroall,_tmp8BF)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp196,env->all_changed,_tmp36B,_tmp36C);};};}_LL15F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp204=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp204->tag != 22)goto _LL161;else{_tmp205=_tmp204->f1;_tmp206=_tmp204->f2;}}_LL160: {
# 1541
struct _RegionHandle _tmp36F=_new_region("temp");struct _RegionHandle*temp=& _tmp36F;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp8C0[2];struct _tuple23 _tmp370=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp8C0[1]=_tmp206,((_tmp8C0[0]=_tmp205,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp8C0,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1542
union Cyc_CfFlowInfo_FlowInfo _tmp372;struct Cyc_List_List*_tmp373;struct _tuple23 _tmp371=_tmp370;_tmp372=_tmp371.f1;_tmp373=_tmp371.f2;{
# 1546
union Cyc_CfFlowInfo_FlowInfo _tmp374=_tmp372;
{union Cyc_CfFlowInfo_FlowInfo _tmp375=_tmp372;struct Cyc_Dict_Dict _tmp376;struct Cyc_List_List*_tmp377;_LL1E6: if((_tmp375.ReachableFL).tag != 2)goto _LL1E8;_tmp376=((struct _tuple14)(_tmp375.ReachableFL).val).f1;_tmp377=((struct _tuple14)(_tmp375.ReachableFL).val).f2;_LL1E7:
# 1551
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp376,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp373))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,Cyc_CfFlowInfo_aerr(_tmp206->loc,((_tmp8C3="expression may not be initialized",_tag_dyneither(_tmp8C3,sizeof(char),34))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp37A=Cyc_NewControlFlow_add_subscript_reln(_tmp196->r,_tmp377,_tmp205,_tmp206);
if(_tmp377 != _tmp37A)
_tmp374=Cyc_CfFlowInfo_ReachableFL(_tmp376,_tmp37A);
goto _LL1E5;};_LL1E8:;_LL1E9:
 goto _LL1E5;_LL1E5:;}{
# 1559
void*_tmp37B=Cyc_Tcutil_compress((void*)_check_null(_tmp205->topt));void*_tmp37C=_tmp37B;struct Cyc_List_List*_tmp37E;union Cyc_Absyn_Constraint*_tmp380;_LL1EB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp37C;if(_tmp37D->tag != 10)goto _LL1ED;else{_tmp37E=_tmp37D->f1;}}_LL1EC: {
# 1561
struct _tuple15 _tmp381=Cyc_CfFlowInfo_unname_rval(_tmp196->r,(void*)((struct Cyc_List_List*)_check_null(_tmp373))->hd);void*_tmp383;struct _tuple15 _tmp382=_tmp381;_tmp383=_tmp382.f1;{
void*_tmp384=_tmp383;struct _dyneither_ptr _tmp386;_LL1F2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp385=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp384;if(_tmp385->tag != 6)goto _LL1F4;else{_tmp386=_tmp385->f2;}}_LL1F3: {
# 1564
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp206)).f1;
struct _tuple16 _tmp8C4;struct _tuple16 _tmp388=(_tmp8C4.f1=_tmp374,((_tmp8C4.f2=*((void**)_check_dyneither_subscript(_tmp386,sizeof(void*),(int)i)),_tmp8C4)));_npop_handler(0);return _tmp388;}_LL1F4:;_LL1F5: {
struct Cyc_Core_Impossible_exn_struct _tmp8CA;const char*_tmp8C9;struct Cyc_Core_Impossible_exn_struct*_tmp8C8;(int)_throw((void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8CA.tag=Cyc_Core_Impossible,((_tmp8CA.f1=((_tmp8C9="anal_Rexp: Subscript",_tag_dyneither(_tmp8C9,sizeof(char),21))),_tmp8CA)))),_tmp8C8)))));}_LL1F1:;};}_LL1ED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C;if(_tmp37F->tag != 5)goto _LL1EF;else{_tmp380=((_tmp37F->f1).ptr_atts).bounds;}}_LL1EE: {
# 1569
struct _tuple16 _tmp38C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp372,_tmp205,(void*)((struct Cyc_List_List*)_check_null(_tmp373))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp38E;void*_tmp38F;struct _tuple16 _tmp38D=_tmp38C;_tmp38E=_tmp38D.f1;_tmp38F=_tmp38D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp390=_tmp38E;_LL1F7: if((_tmp390.BottomFL).tag != 1)goto _LL1F9;_LL1F8: {
struct _tuple16 _tmp8CB;struct _tuple16 _tmp392=(_tmp8CB.f1=_tmp38E,((_tmp8CB.f2=_tmp38F,_tmp8CB)));_npop_handler(0);return _tmp392;}_LL1F9:;_LL1FA: {
struct _tuple16 _tmp8CC;struct _tuple16 _tmp394=(_tmp8CC.f1=_tmp374,((_tmp8CC.f2=_tmp38F,_tmp8CC)));_npop_handler(0);return _tmp394;}_LL1F6:;};}_LL1EF:;_LL1F0: {
# 1574
struct Cyc_Core_Impossible_exn_struct _tmp8D2;const char*_tmp8D1;struct Cyc_Core_Impossible_exn_struct*_tmp8D0;(int)_throw((void*)((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=((_tmp8D2.tag=Cyc_Core_Impossible,((_tmp8D2.f1=((_tmp8D1="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp8D1,sizeof(char),33))),_tmp8D2)))),_tmp8D0)))));}_LL1EA:;};};}
# 1542
;_pop_region(temp);}_LL161: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp207=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp207->tag != 30)goto _LL163;else{_tmp208=_tmp207->f1;_tmp209=_tmp207->f2;}}_LL162:
# 1579
 _tmp20B=_tmp208;goto _LL164;_LL163: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp20A=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp20A->tag != 23)goto _LL165;else{_tmp20B=_tmp20A->f1;}}_LL164: {
# 1581
struct _RegionHandle _tmp399=_new_region("temp");struct _RegionHandle*temp=& _tmp399;_push_region(temp);
{struct _tuple23 _tmp39A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp20B,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp39C;struct Cyc_List_List*_tmp39D;struct _tuple23 _tmp39B=_tmp39A;_tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;{
unsigned int _tmp8D8;unsigned int _tmp8D7;struct _dyneither_ptr _tmp8D6;void**_tmp8D5;unsigned int _tmp8D4;struct _dyneither_ptr aggrdict=
(_tmp8D4=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp20B),((_tmp8D5=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp8D4)),((_tmp8D6=_tag_dyneither(_tmp8D5,sizeof(void*),_tmp8D4),((((_tmp8D7=_tmp8D4,_tmp8D9(& _tmp8D8,& _tmp8D7,& _tmp8D5,& _tmp39D))),_tmp8D6)))))));
# 1589
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8E3;struct Cyc_CfFlowInfo_UnionRInfo _tmp8E2;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8E1;struct _tuple16 _tmp8E0;struct _tuple16 _tmp3A2=(_tmp8E0.f1=_tmp39C,((_tmp8E0.f2=(void*)((_tmp8E3=_region_malloc(env->r,sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E1.tag=6,((_tmp8E1.f1=((_tmp8E2.is_union=0,((_tmp8E2.fieldnum=- 1,_tmp8E2)))),((_tmp8E1.f2=aggrdict,_tmp8E1)))))),_tmp8E3)))),_tmp8E0)));_npop_handler(0);return _tmp3A2;};}
# 1582
;_pop_region(temp);}_LL165: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp20C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp20C->tag != 29)goto _LL167;else{_tmp20D=_tmp20C->f2;}}_LL166: {
# 1592
struct Cyc_List_List*fs;
{void*_tmp3A8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3A9=_tmp3A8;struct Cyc_List_List*_tmp3AB;_LL1FC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3AA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AA->tag != 12)goto _LL1FE;else{_tmp3AB=_tmp3AA->f2;}}_LL1FD:
 fs=_tmp3AB;goto _LL1FB;_LL1FE:;_LL1FF: {
struct Cyc_Core_Impossible_exn_struct _tmp8E9;const char*_tmp8E8;struct Cyc_Core_Impossible_exn_struct*_tmp8E7;(int)_throw((void*)((_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E9.tag=Cyc_Core_Impossible,((_tmp8E9.f1=((_tmp8E8="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp8E8,sizeof(char),35))),_tmp8E9)))),_tmp8E7)))));}_LL1FB:;}
# 1597
_tmp20F=_tmp20D;_tmp210=Cyc_Absyn_StructA;_tmp211=fs;goto _LL168;}_LL167: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp20E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp20E->tag != 28)goto _LL169;else{_tmp20F=_tmp20E->f3;if(_tmp20E->f4 == 0)goto _LL169;_tmp210=(_tmp20E->f4)->kind;if((_tmp20E->f4)->impl == 0)goto _LL169;_tmp211=((_tmp20E->f4)->impl)->fields;}}_LL168: {
# 1599
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp3AF=_new_region("temp");struct _RegionHandle*temp=& _tmp3AF;_push_region(temp);
{struct _tuple23 _tmp3B0=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp20F),1,1);
# 1601
union Cyc_CfFlowInfo_FlowInfo _tmp3B2;struct Cyc_List_List*_tmp3B3;struct _tuple23 _tmp3B1=_tmp3B0;_tmp3B2=_tmp3B1.f1;_tmp3B3=_tmp3B1.f2;{
# 1603
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp196,_tmp211,_tmp210 == Cyc_Absyn_UnionA,_tmp196->unknown_all);
# 1606
int field_no=-1;
{int i=0;for(0;_tmp3B3 != 0;(((_tmp3B3=_tmp3B3->tl,_tmp20F=_tmp20F->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp20F))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp3B4=(void*)ds->hd;void*_tmp3B5=_tmp3B4;struct _dyneither_ptr*_tmp3B8;_LL201: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3B6=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3B5;if(_tmp3B6->tag != 0)goto _LL203;}_LL202: {
struct Cyc_Core_Impossible_exn_struct _tmp8EF;const char*_tmp8EE;struct Cyc_Core_Impossible_exn_struct*_tmp8ED;(int)_throw((void*)((_tmp8ED=_cycalloc(sizeof(*_tmp8ED)),((_tmp8ED[0]=((_tmp8EF.tag=Cyc_Core_Impossible,((_tmp8EF.f1=((_tmp8EE="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp8EE,sizeof(char),22))),_tmp8EF)))),_tmp8ED)))));}_LL203: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3B7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B5;if(_tmp3B7->tag != 1)goto _LL200;else{_tmp3B8=_tmp3B7->f1;}}_LL204:
# 1613
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp211,_tmp3B8);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp3B3->hd;
# 1616
if(_tmp210 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp3BC=(*((struct _tuple26*)_tmp20F->hd)).f2;
_tmp3B2=Cyc_NewControlFlow_use_Rval(env,_tmp3BC->loc,_tmp3B2,(void*)_tmp3B3->hd);
# 1620
Cyc_NewControlFlow_check_union_requires(_tmp3BC->loc,_tmp196->r,exp_type,_tmp3B8,_tmp3B2);}_LL200:;}}}{
# 1623
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8F5;struct Cyc_CfFlowInfo_UnionRInfo _tmp8F4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8F3;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3BD=(_tmp8F3=_region_malloc(env->r,sizeof(*_tmp8F3)),((_tmp8F3[0]=((_tmp8F5.tag=6,((_tmp8F5.f1=((_tmp8F4.is_union=_tmp210 == Cyc_Absyn_UnionA,((_tmp8F4.fieldnum=field_no,_tmp8F4)))),((_tmp8F5.f2=aggrdict,_tmp8F5)))))),_tmp8F3)));
struct _tuple16 _tmp8F6;struct _tuple16 _tmp3BF=(_tmp8F6.f1=_tmp3B2,((_tmp8F6.f2=(void*)_tmp3BD,_tmp8F6)));_npop_handler(0);return _tmp3BF;};};}
# 1601
;_pop_region(temp);}_LL169: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp212=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp212->tag != 28)goto _LL16B;}_LL16A: {
# 1627
struct Cyc_Core_Impossible_exn_struct _tmp8FC;const char*_tmp8FB;struct Cyc_Core_Impossible_exn_struct*_tmp8FA;(int)_throw((void*)((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA[0]=((_tmp8FC.tag=Cyc_Core_Impossible,((_tmp8FC.f1=((_tmp8FB="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp8FB,sizeof(char),31))),_tmp8FC)))),_tmp8FA)))));}_LL16B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp213=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp213->tag != 25)goto _LL16D;else{_tmp214=_tmp213->f1;}}_LL16C: {
# 1629
struct _RegionHandle _tmp3C6=_new_region("temp");struct _RegionHandle*temp=& _tmp3C6;_push_region(temp);
{struct Cyc_List_List*_tmp3C7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp214);
struct _tuple23 _tmp3C8=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3C7,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp3CA;struct Cyc_List_List*_tmp3CB;struct _tuple23 _tmp3C9=_tmp3C8;_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;
for(0;_tmp3CB != 0;(_tmp3CB=_tmp3CB->tl,_tmp3C7=_tmp3C7->tl)){
_tmp3CA=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C7))->hd)->loc,_tmp3CA,(void*)_tmp3CB->hd);}{
struct _tuple16 _tmp8FD;struct _tuple16 _tmp3CD=(_tmp8FD.f1=_tmp3CA,((_tmp8FD.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,(void*)_check_null(e->topt),0,_tmp196->unknown_all),_tmp8FD)));_npop_handler(0);return _tmp3CD;};}
# 1630
;_pop_region(temp);}_LL16D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp215=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp215->tag != 26)goto _LL16F;else{_tmp216=_tmp215->f1;_tmp217=_tmp215->f2;_tmp218=_tmp215->f3;_tmp219=_tmp215->f4;}}_LL16E: {
# 1638
struct _tuple16 _tmp3CE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp217);union Cyc_CfFlowInfo_FlowInfo _tmp3D0;void*_tmp3D1;struct _tuple16 _tmp3CF=_tmp3CE;_tmp3D0=_tmp3CF.f1;_tmp3D1=_tmp3CF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3D2=_tmp3D0;struct Cyc_Dict_Dict _tmp3D3;struct Cyc_List_List*_tmp3D4;_LL206: if((_tmp3D2.BottomFL).tag != 1)goto _LL208;_LL207: {
struct _tuple16 _tmp8FE;return(_tmp8FE.f1=_tmp3D0,((_tmp8FE.f2=_tmp196->unknown_all,_tmp8FE)));}_LL208: if((_tmp3D2.ReachableFL).tag != 2)goto _LL205;_tmp3D3=((struct _tuple14)(_tmp3D2.ReachableFL).val).f1;_tmp3D4=((struct _tuple14)(_tmp3D2.ReachableFL).val).f2;_LL209:
# 1642
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3D3,_tmp3D1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp901;void*_tmp900;(_tmp900=0,Cyc_CfFlowInfo_aerr(_tmp217->loc,((_tmp901="expression may not be initialized",_tag_dyneither(_tmp901,sizeof(char),34))),_tag_dyneither(_tmp900,sizeof(void*),0)));}{
# 1646
struct Cyc_List_List*new_relns=_tmp3D4;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp216);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp217,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp3D4);
# 1652
if(_tmp3D4 != new_relns)
_tmp3D0=Cyc_CfFlowInfo_ReachableFL(_tmp3D3,new_relns);{
# 1656
void*_tmp3D8=_tmp3D1;_LL20B: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3D9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3D8;if(_tmp3D9->tag != 0)goto _LL20D;}_LL20C: {
struct _tuple16 _tmp902;return(_tmp902.f1=_tmp3D0,((_tmp902.f2=_tmp196->unknown_all,_tmp902)));}_LL20D: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3DA=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3D8;if(_tmp3DA->tag != 2)goto _LL20F;}_LL20E:
 goto _LL210;_LL20F: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3DB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3D8;if(_tmp3DB->tag != 1)goto _LL211;}_LL210:
 goto _LL212;_LL211: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3DC=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3D8;if(_tmp3DC->tag != 5)goto _LL213;}_LL212: {
# 1661
struct Cyc_List_List _tmp903;struct Cyc_List_List _tmp3DE=(_tmp903.hd=_tmp216,((_tmp903.tl=0,_tmp903)));
_tmp3D0=Cyc_NewControlFlow_add_vars(_tmp196,_tmp3D0,& _tmp3DE,_tmp196->unknown_all,e->loc,0);{
# 1664
struct _tuple16 _tmp3DF=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3D0,_tmp218);union Cyc_CfFlowInfo_FlowInfo _tmp3E1;void*_tmp3E2;struct _tuple16 _tmp3E0=_tmp3DF;_tmp3E1=_tmp3E0.f1;_tmp3E2=_tmp3E0.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3E3=_tmp3E1;struct Cyc_Dict_Dict _tmp3E4;_LL216: if((_tmp3E3.BottomFL).tag != 1)goto _LL218;_LL217: {
struct _tuple16 _tmp904;return(_tmp904.f1=_tmp3E1,((_tmp904.f2=_tmp196->unknown_all,_tmp904)));}_LL218: if((_tmp3E3.ReachableFL).tag != 2)goto _LL215;_tmp3E4=((struct _tuple14)(_tmp3E3.ReachableFL).val).f1;_LL219:
# 1668
 if(Cyc_CfFlowInfo_initlevel(_tmp196,_tmp3E4,_tmp3E2)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp907;void*_tmp906;(_tmp906=0,Cyc_CfFlowInfo_aerr(_tmp217->loc,((_tmp907="expression may not be initialized",_tag_dyneither(_tmp907,sizeof(char),34))),_tag_dyneither(_tmp906,sizeof(void*),0)));}{
struct _tuple16 _tmp908;return(_tmp908.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp908.f2=_tmp196->unknown_all,_tmp908)));};}_LL215:;}
# 1673
_tmp3D0=_tmp3E1;
goto _LL214;};}_LL213:;_LL214:
# 1676
 while(1){
struct Cyc_List_List _tmp909;struct Cyc_List_List _tmp3EA=(_tmp909.hd=_tmp216,((_tmp909.tl=0,_tmp909)));
_tmp3D0=Cyc_NewControlFlow_add_vars(_tmp196,_tmp3D0,& _tmp3EA,_tmp196->unknown_all,e->loc,0);{
struct _tuple16 _tmp3EB=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3D0,_tmp218);union Cyc_CfFlowInfo_FlowInfo _tmp3ED;void*_tmp3EE;struct _tuple16 _tmp3EC=_tmp3EB;_tmp3ED=_tmp3EC.f1;_tmp3EE=_tmp3EC.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3EF=_tmp3ED;struct Cyc_Dict_Dict _tmp3F0;_LL21B: if((_tmp3EF.BottomFL).tag != 1)goto _LL21D;_LL21C:
 goto _LL21A;_LL21D: if((_tmp3EF.ReachableFL).tag != 2)goto _LL21A;_tmp3F0=((struct _tuple14)(_tmp3EF.ReachableFL).val).f1;_LL21E:
# 1683
 if(Cyc_CfFlowInfo_initlevel(_tmp196,_tmp3F0,_tmp3EE)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp90C;void*_tmp90B;(_tmp90B=0,Cyc_CfFlowInfo_aerr(_tmp217->loc,((_tmp90C="expression may not be initialized",_tag_dyneither(_tmp90C,sizeof(char),34))),_tag_dyneither(_tmp90B,sizeof(void*),0)));}{
struct _tuple16 _tmp90D;return(_tmp90D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp90D.f2=_tmp196->unknown_all,_tmp90D)));};}_LL21A:;}{
# 1688
union Cyc_CfFlowInfo_FlowInfo _tmp3F4=Cyc_CfFlowInfo_join_flow(_tmp196,env->all_changed,_tmp3D0,_tmp3ED);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3F4,_tmp3D0))
break;
_tmp3D0=_tmp3F4;};};}{
# 1693
struct _tuple16 _tmp90E;return(_tmp90E.f1=_tmp3D0,((_tmp90E.f2=_tmp196->unknown_all,_tmp90E)));};_LL20A:;};};_LL205:;};}_LL16F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp21A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp21A->tag != 27)goto _LL171;else{_tmp21B=_tmp21A->f1;_tmp21C=(void*)_tmp21A->f2;_tmp21D=_tmp21A->f3;}}_LL170: {
# 1699
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp911;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp910;void*root=(void*)((_tmp910=_region_malloc(_tmp196->r,sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=1,((_tmp911.f1=_tmp21B,((_tmp911.f2=(void*)_check_null(e->topt),_tmp911)))))),_tmp910))));
struct Cyc_CfFlowInfo_Place*_tmp912;struct Cyc_CfFlowInfo_Place*place=(_tmp912=_region_malloc(_tmp196->r,sizeof(*_tmp912)),((_tmp912->root=root,((_tmp912->fields=0,_tmp912)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp915;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp914;void*rval=(void*)((_tmp914=_region_malloc(_tmp196->r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=5,((_tmp915.f1=place,_tmp915)))),_tmp914))));
void*place_val;
# 1707
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp196,_tmp21C,0,_tmp196->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp3F7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp21B);union Cyc_CfFlowInfo_FlowInfo _tmp3F9;struct _tuple16 _tmp3F8=_tmp3F7;_tmp3F9=_tmp3F8.f1;
outflow=_tmp3F9;{
union Cyc_CfFlowInfo_FlowInfo _tmp3FA=outflow;struct Cyc_Dict_Dict _tmp3FB;struct Cyc_List_List*_tmp3FC;_LL220: if((_tmp3FA.BottomFL).tag != 1)goto _LL222;_LL221: {
struct _tuple16 _tmp916;return(_tmp916.f1=outflow,((_tmp916.f2=rval,_tmp916)));}_LL222: if((_tmp3FA.ReachableFL).tag != 2)goto _LL21F;_tmp3FB=((struct _tuple14)(_tmp3FA.ReachableFL).val).f1;_tmp3FC=((struct _tuple14)(_tmp3FA.ReachableFL).val).f2;_LL223: {
# 1715
struct _tuple16 _tmp917;return(_tmp917.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3FB,root,place_val),_tmp3FC),((_tmp917.f2=rval,_tmp917)));}_LL21F:;};};};}_LL171: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp21E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp21E->tag != 36)goto _LL173;else{_tmp21F=_tmp21E->f1;}}_LL172: {
# 1719
struct _tuple17 _tmp918;struct _tuple17 _tmp404=(_tmp918.f1=(env->fenv)->unknown_all,((_tmp918.f2=copy_ctxt,_tmp918)));
union Cyc_CfFlowInfo_FlowInfo _tmp405=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp21F,& _tmp404);
struct _tuple16 _tmp919;return(_tmp919.f1=_tmp405,((_tmp919.f2=_tmp404.f1,_tmp919)));}_LL173: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp220=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp220->tag != 1)goto _LL175;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp221=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp220->f2);if(_tmp221->tag != 0)goto _LL175;}}_LL174:
# 1723
 goto _LL176;_LL175: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp222=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp222->tag != 35)goto _LL177;}_LL176:
 goto _LL178;_LL177: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp223=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp223->tag != 24)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp224=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp224->tag != 38)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp225=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp1AE;if(_tmp225->tag != 39)goto _LL114;}_LL17C: {
# 1728
struct Cyc_Core_Impossible_exn_struct _tmp91F;const char*_tmp91E;struct Cyc_Core_Impossible_exn_struct*_tmp91D;(int)_throw((void*)((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D[0]=((_tmp91F.tag=Cyc_Core_Impossible,((_tmp91F.f1=((_tmp91E="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp91E,sizeof(char),31))),_tmp91F)))),_tmp91D)))));}_LL114:;};}
# 1738
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place,struct Cyc_List_List*flds){
# 1747
struct Cyc_CfFlowInfo_FlowEnv*_tmp40B=env->fenv;
void*_tmp40C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp40D=_tmp40C;void*_tmp40F;union Cyc_Absyn_Constraint*_tmp410;union Cyc_Absyn_Constraint*_tmp411;_LL225: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40D;if(_tmp40E->tag != 5)goto _LL227;else{_tmp40F=(_tmp40E->f1).elt_typ;_tmp410=((_tmp40E->f1).ptr_atts).bounds;_tmp411=((_tmp40E->f1).ptr_atts).zero_term;}}_LL226: {
# 1750
union Cyc_CfFlowInfo_FlowInfo _tmp412=f;struct Cyc_Dict_Dict _tmp413;struct Cyc_List_List*_tmp414;_LL22A: if((_tmp412.BottomFL).tag != 1)goto _LL22C;_LL22B: {
struct _tuple18 _tmp920;return(_tmp920.f1=f,((_tmp920.f2=Cyc_CfFlowInfo_UnknownL(),_tmp920)));}_LL22C: if((_tmp412.ReachableFL).tag != 2)goto _LL229;_tmp413=((struct _tuple14)(_tmp412.ReachableFL).val).f1;_tmp414=((struct _tuple14)(_tmp412.ReachableFL).val).f2;_LL22D:
# 1754
 if(Cyc_Tcutil_is_bound_one(_tmp410)){
struct _tuple15 _tmp416=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp418;struct Cyc_List_List*_tmp419;struct _tuple15 _tmp417=_tmp416;_tmp418=_tmp417.f1;_tmp419=_tmp417.f2;
retry: {void*_tmp41A=_tmp418;void*_tmp41F;struct Cyc_List_List*_tmp420;enum Cyc_CfFlowInfo_InitLevel _tmp423;_LL22F: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp41B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp41A;if(_tmp41B->tag != 8)goto _LL231;}_LL230: {
# 1758
struct Cyc_Core_Impossible_exn_struct _tmp926;const char*_tmp925;struct Cyc_Core_Impossible_exn_struct*_tmp924;(int)_throw((void*)((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924[0]=((_tmp926.tag=Cyc_Core_Impossible,((_tmp926.f1=((_tmp925="named location in anal_derefL",_tag_dyneither(_tmp925,sizeof(char),30))),_tmp926)))),_tmp924)))));}_LL231: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp41C=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp41A;if(_tmp41C->tag != 1)goto _LL233;}_LL232:
 goto _LL234;_LL233: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp41D=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp41A;if(_tmp41D->tag != 2)goto _LL235;}_LL234:
# 1761
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp929;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp928;e->annot=(void*)((_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928[0]=((_tmp929.tag=Cyc_CfFlowInfo_NotZero,((_tmp929.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp414),_tmp929)))),_tmp928))));}goto _LL22E;_LL235: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp41E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp41A;if(_tmp41E->tag != 5)goto _LL237;else{_tmp41F=(_tmp41E->f1)->root;_tmp420=(_tmp41E->f1)->fields;}}_LL236:
# 1764
 if(expanded_place){
# 1767
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp92C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp92B;e->annot=(void*)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=((_tmp92C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp92C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp414),_tmp92C)))),_tmp92B))));}else{
# 1770
void*_tmp42B=e->annot;void*_tmp42C=_tmp42B;_LL23E: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp42D=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp42C;if(_tmp42D->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL240;}_LL23F:
# 1774
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp92F;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp92E;e->annot=(void*)((_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92F.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp92F.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp414),_tmp92F)))),_tmp92E))));}
goto _LL23D;_LL240:;_LL241:
# 1777
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp932;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp931;e->annot=(void*)((_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931[0]=((_tmp932.tag=Cyc_CfFlowInfo_NotZero,((_tmp932.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp414),_tmp932)))),_tmp931))));}
goto _LL23D;_LL23D:;}{
# 1781
struct Cyc_CfFlowInfo_Place*_tmp935;struct _tuple18 _tmp934;return(_tmp934.f1=f,((_tmp934.f2=Cyc_CfFlowInfo_PlaceL(((_tmp935=_region_malloc(_tmp40B->r,sizeof(*_tmp935)),((_tmp935->root=_tmp41F,((_tmp935->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp40B->r,_tmp420,flds),_tmp935))))))),_tmp934)));};_LL237: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp421=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41A;if(_tmp421->tag != 0)goto _LL239;}_LL238:
# 1783
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple18 _tmp936;return(_tmp936.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp936.f2=Cyc_CfFlowInfo_UnknownL(),_tmp936)));};_LL239: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp422=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp41A;if(_tmp422->tag != 3)goto _LL23B;else{_tmp423=_tmp422->f1;}}_LL23A:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp423,_tmp419);goto _LL23C;_LL23B:;_LL23C:
# 1788
 if(passthrough_consumes){
void*_tmp435=_tmp418;void*_tmp437;_LL243: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp436=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp435;if(_tmp436->tag != 7)goto _LL245;else{_tmp437=(void*)_tmp436->f3;}}_LL244:
 _tmp418=_tmp437;goto retry;_LL245:;_LL246:
 goto _LL242;_LL242:;}{
# 1794
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp939;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp938;e->annot=(void*)((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp939.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp414),_tmp939)))),_tmp938))));};_LL22E:;}}else{
# 1798
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp93C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp93B;e->annot=(void*)((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp93C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp414),_tmp93C)))),_tmp93B))));}
if(Cyc_CfFlowInfo_initlevel(_tmp40B,_tmp413,r)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp43C=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp43E;struct _tuple15 _tmp43D=_tmp43C;_tmp43E=_tmp43D.f1;{
void*_tmp43F=_tmp43E;_LL248: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp440=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp43F;if(_tmp440->tag != 7)goto _LL24A;}_LL249:
# 1803
{const char*_tmp93F;void*_tmp93E;(_tmp93E=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp93F="attempt to dereference an alias-free that has already been copied",_tag_dyneither(_tmp93F,sizeof(char),66))),_tag_dyneither(_tmp93E,sizeof(void*),0)));}
goto _LL247;_LL24A:;_LL24B:
# 1806
{const char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp942="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp942,sizeof(char),46))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
goto _LL247;_LL247:;};}{
# 1810
struct _tuple18 _tmp943;return(_tmp943.f1=f,((_tmp943.f2=Cyc_CfFlowInfo_UnknownL(),_tmp943)));};_LL229:;}_LL227:;_LL228: {
# 1812
struct Cyc_Core_Impossible_exn_struct _tmp949;const char*_tmp948;struct Cyc_Core_Impossible_exn_struct*_tmp947;(int)_throw((void*)((_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947[0]=((_tmp949.tag=Cyc_Core_Impossible,((_tmp949.f1=((_tmp948="left deref of non-pointer-type",_tag_dyneither(_tmp948,sizeof(char),31))),_tmp949)))),_tmp947)))));}_LL224:;}
# 1820
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1823
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp449=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp44A=inflow;struct Cyc_Dict_Dict _tmp44B;struct Cyc_List_List*_tmp44C;_LL24D: if((_tmp44A.BottomFL).tag != 1)goto _LL24F;_LL24E: {
struct _tuple18 _tmp94A;return(_tmp94A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp94A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp94A)));}_LL24F: if((_tmp44A.ReachableFL).tag != 2)goto _LL24C;_tmp44B=((struct _tuple14)(_tmp44A.ReachableFL).val).f1;_tmp44C=((struct _tuple14)(_tmp44A.ReachableFL).val).f2;_LL250:
# 1828
 d=_tmp44B;_LL24C:;}{
# 1832
void*_tmp44E=e->r;void*_tmp44F=_tmp44E;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Vardecl*_tmp458;struct Cyc_Absyn_Vardecl*_tmp45B;struct Cyc_Absyn_Vardecl*_tmp45E;struct Cyc_Absyn_Vardecl*_tmp461;struct Cyc_Absyn_Exp*_tmp463;struct _dyneither_ptr*_tmp464;struct Cyc_Absyn_Exp*_tmp466;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp469;struct Cyc_Absyn_Exp*_tmp46B;struct _dyneither_ptr*_tmp46C;_LL252: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp450=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp450->tag != 13)goto _LL254;else{_tmp451=_tmp450->f2;}}_LL253:
 _tmp453=_tmp451;goto _LL255;_LL254: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp452=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp452->tag != 11)goto _LL256;else{_tmp453=_tmp452->f1;}}_LL255:
 _tmp455=_tmp453;goto _LL257;_LL256: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp454=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp454->tag != 12)goto _LL258;else{_tmp455=_tmp454->f1;}}_LL257:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp455,flds);_LL258: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp456=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp456->tag != 1)goto _LL25A;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp457=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp456->f2);if(_tmp457->tag != 3)goto _LL25A;else{_tmp458=_tmp457->f1;}}}_LL259:
# 1837
 _tmp45B=_tmp458;goto _LL25B;_LL25A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp459=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp459->tag != 1)goto _LL25C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp45A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp459->f2);if(_tmp45A->tag != 4)goto _LL25C;else{_tmp45B=_tmp45A->f1;}}}_LL25B:
 _tmp45E=_tmp45B;goto _LL25D;_LL25C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp45C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp45C->tag != 1)goto _LL25E;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp45D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp45C->f2);if(_tmp45D->tag != 5)goto _LL25E;else{_tmp45E=_tmp45D->f1;}}}_LL25D: {
# 1840
struct Cyc_CfFlowInfo_Place*_tmp954;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp953;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp952;struct _tuple18 _tmp951;return(_tmp951.f1=inflow,((_tmp951.f2=Cyc_CfFlowInfo_PlaceL(((_tmp954=_region_malloc(env->r,sizeof(*_tmp954)),((_tmp954->root=(void*)((_tmp952=_region_malloc(env->r,sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=0,((_tmp953.f1=_tmp45E,_tmp953)))),_tmp952)))),((_tmp954->fields=flds,_tmp954))))))),_tmp951)));}_LL25E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp45F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp45F->tag != 1)goto _LL260;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp460=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp45F->f2);if(_tmp460->tag != 1)goto _LL260;else{_tmp461=_tmp460->f1;}}}_LL25F: {
# 1842
struct _tuple18 _tmp955;return(_tmp955.f1=inflow,((_tmp955.f2=Cyc_CfFlowInfo_UnknownL(),_tmp955)));}_LL260: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp462=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp462->tag != 21)goto _LL262;else{_tmp463=_tmp462->f1;_tmp464=_tmp462->f2;}}_LL261:
# 1845
{void*_tmp472=Cyc_Tcutil_compress((void*)_check_null(_tmp463->topt));void*_tmp473=_tmp472;void*_tmp475;_LL26B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp473;if(_tmp474->tag != 5)goto _LL26D;else{_tmp475=(_tmp474->f1).elt_typ;}}_LL26C:
# 1847
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp475)){
Cyc_NewControlFlow_check_union_requires(_tmp463->loc,_tmp449->r,_tmp475,_tmp464,inflow);{
struct Cyc_List_List*_tmp956;flds=((_tmp956=_region_malloc(env->r,sizeof(*_tmp956)),((_tmp956->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp475,_tmp464),((_tmp956->tl=flds,_tmp956))))));};}
# 1851
goto _LL26A;_LL26D:;_LL26E: {
struct Cyc_Core_Impossible_exn_struct _tmp95C;const char*_tmp95B;struct Cyc_Core_Impossible_exn_struct*_tmp95A;(int)_throw((void*)((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A[0]=((_tmp95C.tag=Cyc_Core_Impossible,((_tmp95C.f1=((_tmp95B="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp95B,sizeof(char),25))),_tmp95C)))),_tmp95A)))));}_LL26A:;}
# 1854
_tmp466=_tmp463;goto _LL263;_LL262: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp465=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp465->tag != 19)goto _LL264;else{_tmp466=_tmp465->f1;}}_LL263:
# 1857
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp466->topt),1)){
# 1859
struct _tuple18 _tmp47A=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp466->topt),1),passthrough_consumes,_tmp466);
# 1859
union Cyc_CfFlowInfo_FlowInfo _tmp47C;union Cyc_CfFlowInfo_AbsLVal _tmp47D;struct _tuple18 _tmp47B=_tmp47A;_tmp47C=_tmp47B.f1;_tmp47D=_tmp47B.f2;{
# 1863
struct _tuple18 _tmp95D;struct _tuple18 _tmp47E=(_tmp95D.f1=_tmp47C,((_tmp95D.f2=_tmp47D,_tmp95D)));struct _tuple18 _tmp47F=_tmp47E;struct Cyc_Dict_Dict _tmp480;struct Cyc_List_List*_tmp481;struct Cyc_CfFlowInfo_Place*_tmp482;_LL270: if(((_tmp47F.f1).ReachableFL).tag != 2)goto _LL272;_tmp480=((struct _tuple14)((_tmp47F.f1).ReachableFL).val).f1;_tmp481=((struct _tuple14)((_tmp47F.f1).ReachableFL).val).f2;if(((_tmp47F.f2).PlaceL).tag != 1)goto _LL272;_tmp482=(struct Cyc_CfFlowInfo_Place*)((_tmp47F.f2).PlaceL).val;_LL271: {
# 1865
void*_tmp483=Cyc_CfFlowInfo_lookup_place(_tmp480,_tmp482);
struct _tuple15 _tmp484=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp483);void*_tmp486;struct Cyc_List_List*_tmp487;struct _tuple15 _tmp485=_tmp484;_tmp486=_tmp485.f1;_tmp487=_tmp485.f2;{
void*_tmp488=_tmp486;struct Cyc_Absyn_Vardecl*_tmp48A;void*_tmp48B;_LL275: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp489=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp488;if(_tmp489->tag != 8)goto _LL277;else{_tmp48A=_tmp489->f1;_tmp48B=(void*)_tmp489->f2;}}_LL276: {
# 1869
struct Cyc_Core_Impossible_exn_struct _tmp963;const char*_tmp962;struct Cyc_Core_Impossible_exn_struct*_tmp961;(int)_throw((void*)((_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961[0]=((_tmp963.tag=Cyc_Core_Impossible,((_tmp963.f1=((_tmp962="bad named location in anal_Lexp:deref",_tag_dyneither(_tmp962,sizeof(char),38))),_tmp963)))),_tmp961)))));}_LL277: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp48C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp488;if(_tmp48C->tag != 7)goto _LL279;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp48D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp48C->f3);if(_tmp48D->tag != 5)goto _LL279;}}_LL278:
 goto _LL27A;_LL279: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp48E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp488;if(_tmp48E->tag != 5)goto _LL27B;}_LL27A:
# 1874
 return Cyc_NewControlFlow_anal_derefL(env,_tmp47C,_tmp466,_tmp47C,_tmp483,passthrough_consumes,0,flds);_LL27B:;_LL27C: {
# 1881
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp449,_tmp480,_tmp486);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp449->unknown_all: _tmp449->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp449,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp466->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp966;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp965;void*new_root=(void*)((_tmp965=_region_malloc(_tmp449->r,sizeof(*_tmp965)),((_tmp965[0]=((_tmp966.tag=1,((_tmp966.f1=e,((_tmp966.f2=(void*)_check_null(e->topt),_tmp966)))))),_tmp965))));
struct Cyc_CfFlowInfo_Place*_tmp967;struct Cyc_CfFlowInfo_Place*place=(_tmp967=_region_malloc(_tmp449->r,sizeof(*_tmp967)),((_tmp967->root=new_root,((_tmp967->fields=0,_tmp967)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp96A;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp969;void*res=(void*)((_tmp969=_region_malloc(_tmp449->r,sizeof(*_tmp969)),((_tmp969[0]=((_tmp96A.tag=5,((_tmp96A.f1=place,_tmp96A)))),_tmp969))));
for(0;_tmp487 != 0;_tmp487=_tmp487->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp96D;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp96C;res=(void*)((_tmp96C=_region_malloc(_tmp449->r,sizeof(*_tmp96C)),((_tmp96C[0]=((_tmp96D.tag=8,((_tmp96D.f1=(struct Cyc_Absyn_Vardecl*)_tmp487->hd,((_tmp96D.f2=res,_tmp96D)))))),_tmp96C))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp480=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp480,new_root,new_rval);
_tmp480=Cyc_CfFlowInfo_assign_place(_tmp449,e->loc,_tmp480,env->all_changed,_tmp482,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp494=Cyc_CfFlowInfo_ReachableFL(_tmp480,_tmp481);
# 1899
return Cyc_NewControlFlow_anal_derefL(env,_tmp494,_tmp466,_tmp494,res,passthrough_consumes,1,flds);};}_LL274:;};}_LL272:;_LL273:
# 1902
 goto _LL26F;_LL26F:;};}{
# 1905
struct _tuple16 _tmp49B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp466);union Cyc_CfFlowInfo_FlowInfo _tmp49D;void*_tmp49E;struct _tuple16 _tmp49C=_tmp49B;_tmp49D=_tmp49C.f1;_tmp49E=_tmp49C.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp466,_tmp49D,_tmp49E,passthrough_consumes,0,flds);};_LL264: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp467=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp467->tag != 22)goto _LL266;else{_tmp468=_tmp467->f1;_tmp469=_tmp467->f2;}}_LL265: {
# 1909
void*_tmp49F=Cyc_Tcutil_compress((void*)_check_null(_tmp468->topt));void*_tmp4A0=_tmp49F;union Cyc_Absyn_Constraint*_tmp4A3;_LL27E: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4A0;if(_tmp4A1->tag != 10)goto _LL280;}_LL27F: {
# 1911
unsigned int _tmp4A4=(Cyc_Evexp_eval_const_uint_exp(_tmp469)).f1;
struct Cyc_List_List*_tmp96E;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp468,((_tmp96E=_region_malloc(env->r,sizeof(*_tmp96E)),((_tmp96E->hd=(void*)((int)_tmp4A4),((_tmp96E->tl=flds,_tmp96E)))))));}_LL280: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0;if(_tmp4A2->tag != 5)goto _LL282;else{_tmp4A3=((_tmp4A2->f1).ptr_atts).bounds;}}_LL281: {
# 1914
struct _RegionHandle _tmp4A6=_new_region("temp");struct _RegionHandle*temp=& _tmp4A6;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp96F[2];struct _tuple23 _tmp4A7=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp96F[1]=_tmp469,((_tmp96F[0]=_tmp468,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp96F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1915
union Cyc_CfFlowInfo_FlowInfo _tmp4A9;struct Cyc_List_List*_tmp4AA;struct _tuple23 _tmp4A8=_tmp4A7;_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;{
# 1917
union Cyc_CfFlowInfo_FlowInfo _tmp4AB=_tmp4A9;
{union Cyc_CfFlowInfo_FlowInfo _tmp4AC=_tmp4A9;struct Cyc_Dict_Dict _tmp4AD;struct Cyc_List_List*_tmp4AE;_LL285: if((_tmp4AC.ReachableFL).tag != 2)goto _LL287;_tmp4AD=((struct _tuple14)(_tmp4AC.ReachableFL).val).f1;_tmp4AE=((struct _tuple14)(_tmp4AC.ReachableFL).val).f2;_LL286:
# 1920
 if(Cyc_CfFlowInfo_initlevel(_tmp449,_tmp4AD,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4AA))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp972;void*_tmp971;(_tmp971=0,Cyc_CfFlowInfo_aerr(_tmp469->loc,((_tmp972="expression may not be initialized",_tag_dyneither(_tmp972,sizeof(char),34))),_tag_dyneither(_tmp971,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp4B1=Cyc_NewControlFlow_add_subscript_reln(_tmp449->r,_tmp4AE,_tmp468,_tmp469);
if(_tmp4AE != _tmp4B1)
_tmp4AB=Cyc_CfFlowInfo_ReachableFL(_tmp4AD,_tmp4B1);
goto _LL284;};_LL287:;_LL288:
 goto _LL284;_LL284:;}{
# 1928
struct _tuple18 _tmp4B2=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp468,_tmp4A9,(void*)((struct Cyc_List_List*)_check_null(_tmp4AA))->hd,passthrough_consumes,0,flds);union Cyc_CfFlowInfo_FlowInfo _tmp4B4;union Cyc_CfFlowInfo_AbsLVal _tmp4B5;struct _tuple18 _tmp4B3=_tmp4B2;_tmp4B4=_tmp4B3.f1;_tmp4B5=_tmp4B3.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B6=_tmp4B4;_LL28A: if((_tmp4B6.BottomFL).tag != 1)goto _LL28C;_LL28B: {
struct _tuple18 _tmp973;struct _tuple18 _tmp4B8=(_tmp973.f1=_tmp4B4,((_tmp973.f2=_tmp4B5,_tmp973)));_npop_handler(0);return _tmp4B8;}_LL28C:;_LL28D: {
struct _tuple18 _tmp974;struct _tuple18 _tmp4BA=(_tmp974.f1=_tmp4AB,((_tmp974.f2=_tmp4B5,_tmp974)));_npop_handler(0);return _tmp4BA;}_LL289:;};};};}
# 1915
;_pop_region(temp);}_LL282:;_LL283: {
# 1934
struct Cyc_Core_Impossible_exn_struct _tmp97A;const char*_tmp979;struct Cyc_Core_Impossible_exn_struct*_tmp978;(int)_throw((void*)((_tmp978=_cycalloc(sizeof(*_tmp978)),((_tmp978[0]=((_tmp97A.tag=Cyc_Core_Impossible,((_tmp97A.f1=((_tmp979="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp979,sizeof(char),33))),_tmp97A)))),_tmp978)))));}_LL27D:;}_LL266: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp46A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp44F;if(_tmp46A->tag != 20)goto _LL268;else{_tmp46B=_tmp46A->f1;_tmp46C=_tmp46A->f2;}}_LL267: {
# 1938
void*_tmp4BF=(void*)_check_null(_tmp46B->topt);
if(Cyc_Absyn_is_require_union_type(_tmp4BF))
Cyc_NewControlFlow_check_union_requires(_tmp46B->loc,_tmp449->r,_tmp4BF,_tmp46C,inflow);
# 1942
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp4BF)){
struct _tuple18 _tmp97B;return(_tmp97B.f1=inflow,((_tmp97B.f2=Cyc_CfFlowInfo_UnknownL(),_tmp97B)));}{
# 1945
struct Cyc_List_List*_tmp97C;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp46B,(
(_tmp97C=_region_malloc(env->r,sizeof(*_tmp97C)),((_tmp97C->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp4BF,_tmp46C),((_tmp97C->tl=flds,_tmp97C)))))));};}_LL268:;_LL269: {
# 1948
struct _tuple18 _tmp97D;return(_tmp97D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp97D.f2=Cyc_CfFlowInfo_UnknownL(),_tmp97D)));}_LL251:;};}
# 1952
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1955
struct _tuple18 _tmp4C3=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp4C5;union Cyc_CfFlowInfo_AbsLVal _tmp4C6;struct _tuple18 _tmp4C4=_tmp4C3;_tmp4C5=_tmp4C4.f1;_tmp4C6=_tmp4C4.f2;{
struct _tuple18 _tmp97E;return(_tmp97E.f1=_tmp4C5,((_tmp97E.f2=_tmp4C6,_tmp97E)));};}
# 1962
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1965
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp4C8=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp4CA;struct _tuple18 _tmp4C9=_tmp4C8;_tmp4CA=_tmp4C9.f1;
inflow=_tmp4CA;}}}
# 1973
return inflow;}
# 1979
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1982
struct Cyc_CfFlowInfo_FlowEnv*_tmp4CB=env->fenv;
# 1985
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp4CC=_new_region("temp");struct _RegionHandle*temp=& _tmp4CC;_push_region(temp);{
struct _tuple23 _tmp4CD=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp4CF;struct Cyc_List_List*_tmp4D0;struct _tuple23 _tmp4CE=_tmp4CD;_tmp4CF=_tmp4CE.f1;_tmp4D0=_tmp4CE.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp4D0))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp4D0->tl))->hd;
f=_tmp4CF;}{
# 1995
union Cyc_CfFlowInfo_FlowInfo _tmp4D1=f;struct Cyc_Dict_Dict _tmp4D2;struct Cyc_List_List*_tmp4D3;_LL28F: if((_tmp4D1.BottomFL).tag != 1)goto _LL291;_LL290: {
struct _tuple19 _tmp97F;struct _tuple19 _tmp4D5=(_tmp97F.f1=f,((_tmp97F.f2=f,_tmp97F)));_npop_handler(0);return _tmp4D5;}_LL291: if((_tmp4D1.ReachableFL).tag != 2)goto _LL28E;_tmp4D2=((struct _tuple14)(_tmp4D1.ReachableFL).val).f1;_tmp4D3=((struct _tuple14)(_tmp4D1.ReachableFL).val).f2;_LL292: {
# 1998
struct Cyc_Absyn_Exp*_tmp4D6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp4D7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D2,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp982;void*_tmp981;(_tmp981=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp982="expression may not be initialized",_tag_dyneither(_tmp982,sizeof(char),34))),_tag_dyneither(_tmp981,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D2,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp985;void*_tmp984;(_tmp984=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp985="expression may not be initialized",_tag_dyneither(_tmp985,sizeof(char),34))),_tag_dyneither(_tmp984,sizeof(void*),0)));}{
# 2005
union Cyc_CfFlowInfo_FlowInfo _tmp4DC=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4DD=f;
# 2010
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp4DE=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);void*_tmp4E0;struct Cyc_List_List*_tmp4E1;struct _tuple15 _tmp4DF=_tmp4DE;_tmp4E0=_tmp4DF.f1;_tmp4E1=_tmp4DF.f2;{
struct _tuple15 _tmp4E2=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);void*_tmp4E4;struct Cyc_List_List*_tmp4E5;struct _tuple15 _tmp4E3=_tmp4E2;_tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;{
struct _tuple0 _tmp986;struct _tuple0 _tmp4E6=(_tmp986.f1=_tmp4E0,((_tmp986.f2=_tmp4E4,_tmp986)));struct _tuple0 _tmp4E7=_tmp4E6;enum Cyc_CfFlowInfo_InitLevel _tmp4E9;enum Cyc_CfFlowInfo_InitLevel _tmp4ED;_LL294:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4E8=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4E8->tag != 3)goto _LL296;else{_tmp4E9=_tmp4E8->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4EA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4EA->tag != 0)goto _LL296;};_LL295: {
# 2017
struct _tuple19 _tmp4FC=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4D6,_tmp4E9,_tmp4E1);union Cyc_CfFlowInfo_FlowInfo _tmp4FE;union Cyc_CfFlowInfo_FlowInfo _tmp4FF;struct _tuple19 _tmp4FD=_tmp4FC;_tmp4FE=_tmp4FD.f1;_tmp4FF=_tmp4FD.f2;
switch(p){case Cyc_Absyn_Eq: _LL2A8:
 _tmp4DC=_tmp4FF;_tmp4DD=_tmp4FE;break;case Cyc_Absyn_Neq: _LL2A9:
 _tmp4DC=_tmp4FE;_tmp4DD=_tmp4FF;break;default: _LL2AA: {
struct Cyc_Core_Impossible_exn_struct _tmp98C;const char*_tmp98B;struct Cyc_Core_Impossible_exn_struct*_tmp98A;(int)_throw((void*)((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98C.tag=Cyc_Core_Impossible,((_tmp98C.f1=((_tmp98B="anal_test, zero-split",_tag_dyneither(_tmp98B,sizeof(char),22))),_tmp98C)))),_tmp98A)))));}}
# 2023
goto _LL293;}_LL296:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4EB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4EB->tag != 0)goto _LL298;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4EC=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4EC->tag != 3)goto _LL298;else{_tmp4ED=_tmp4EC->f1;}};_LL297: {
# 2025
struct _tuple19 _tmp503=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4D7,_tmp4ED,_tmp4E5);union Cyc_CfFlowInfo_FlowInfo _tmp505;union Cyc_CfFlowInfo_FlowInfo _tmp506;struct _tuple19 _tmp504=_tmp503;_tmp505=_tmp504.f1;_tmp506=_tmp504.f2;
switch(p){case Cyc_Absyn_Eq: _LL2AC:
 _tmp4DC=_tmp506;_tmp4DD=_tmp505;break;case Cyc_Absyn_Neq: _LL2AD:
 _tmp4DC=_tmp505;_tmp4DD=_tmp506;break;default: _LL2AE: {
struct Cyc_Core_Impossible_exn_struct _tmp992;const char*_tmp991;struct Cyc_Core_Impossible_exn_struct*_tmp990;(int)_throw((void*)((_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990[0]=((_tmp992.tag=Cyc_Core_Impossible,((_tmp992.f1=((_tmp991="anal_test, zero-split",_tag_dyneither(_tmp991,sizeof(char),22))),_tmp992)))),_tmp990)))));}}
# 2031
goto _LL293;}_LL298:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4EE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4EE->tag != 0)goto _LL29A;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4EF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4EF->tag != 0)goto _LL29A;};_LL299:
# 2033
 if(p == Cyc_Absyn_Eq)_tmp4DD=Cyc_CfFlowInfo_BottomFL();else{
_tmp4DC=Cyc_CfFlowInfo_BottomFL();}
goto _LL293;_LL29A:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4F0=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4F0->tag != 0)goto _LL29C;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4F1=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4F1->tag != 1)goto _LL29C;};_LL29B:
 goto _LL29D;_LL29C:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4F2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4F2->tag != 0)goto _LL29E;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4F3=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4F3->tag != 2)goto _LL29E;};_LL29D:
 goto _LL29F;_LL29E:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4F4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4F4->tag != 0)goto _LL2A0;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4F5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4F5->tag != 5)goto _LL2A0;};_LL29F:
 goto _LL2A1;_LL2A0:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4F6=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4F6->tag != 1)goto _LL2A2;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4F7=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4F7->tag != 0)goto _LL2A2;};_LL2A1:
 goto _LL2A3;_LL2A2:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4F8=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4F8->tag != 2)goto _LL2A4;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4F9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4F9->tag != 0)goto _LL2A4;};_LL2A3:
 goto _LL2A5;_LL2A4:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4FA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f1;if(_tmp4FA->tag != 5)goto _LL2A6;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4FB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4E7.f2;if(_tmp4FB->tag != 0)goto _LL2A6;};_LL2A5:
# 2042
 if(p == Cyc_Absyn_Neq)_tmp4DD=Cyc_CfFlowInfo_BottomFL();else{
_tmp4DC=Cyc_CfFlowInfo_BottomFL();}
goto _LL293;_LL2A6:;_LL2A7:
 goto _LL293;_LL293:;};};}
# 2053
{struct _tuple0 _tmp993;struct _tuple0 _tmp50B=(_tmp993.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp4D6->topt)),((_tmp993.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp4D7->topt)),_tmp993)));struct _tuple0 _tmp50C=_tmp50B;_LL2B1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50C.f1;if(_tmp50D->tag != 6)goto _LL2B3;else{if(_tmp50D->f1 != Cyc_Absyn_Unsigned)goto _LL2B3;}}_LL2B2:
 goto _LL2B4;_LL2B3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp50E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50C.f2;if(_tmp50E->tag != 6)goto _LL2B5;else{if(_tmp50E->f1 != Cyc_Absyn_Unsigned)goto _LL2B5;}}_LL2B4:
 goto _LL2B6;_LL2B5: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp50C.f1;if(_tmp50F->tag != 19)goto _LL2B7;}_LL2B6:
 goto _LL2B8;_LL2B7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp50C.f2;if(_tmp510->tag != 19)goto _LL2B9;}_LL2B8:
 goto _LL2B0;_LL2B9:;_LL2BA: {
struct _tuple19 _tmp994;struct _tuple19 _tmp512=(_tmp994.f1=_tmp4DC,((_tmp994.f2=_tmp4DD,_tmp994)));_npop_handler(0);return _tmp512;}_LL2B0:;}
# 2062
switch(p){case Cyc_Absyn_Eq: _LL2BB:
 goto _LL2BC;case Cyc_Absyn_Neq: _LL2BC: goto _LL2BD;case Cyc_Absyn_Gt: _LL2BD: goto _LL2BE;case Cyc_Absyn_Gte: _LL2BE: goto _LL2BF;case Cyc_Absyn_Lt: _LL2BF: goto _LL2C0;case Cyc_Absyn_Lte: _LL2C0: break;default: _LL2C1: {
struct _tuple19 _tmp995;struct _tuple19 _tmp515=(_tmp995.f1=_tmp4DC,((_tmp995.f2=_tmp4DD,_tmp995)));_npop_handler(0);return _tmp515;}}{
# 2067
struct _RegionHandle*_tmp516=(env->fenv)->r;
struct _tuple13 _tmp517=Cyc_Relations_primop2relation(_tmp4D6,p,_tmp4D7);struct Cyc_Absyn_Exp*_tmp519;enum Cyc_Relations_Relation _tmp51A;struct Cyc_Absyn_Exp*_tmp51B;struct _tuple13 _tmp518=_tmp517;_tmp519=_tmp518.f1;_tmp51A=_tmp518.f2;_tmp51B=_tmp518.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2072
if(Cyc_Relations_exp2relnop(_tmp519,& n1) && Cyc_Relations_exp2relnop(_tmp51B,& n2)){
# 2074
struct Cyc_List_List*_tmp51C=Cyc_Relations_add_relation(_tmp516,n1,_tmp51A,n2,_tmp4D3);
# 2078
struct Cyc_List_List*_tmp51D=Cyc_Relations_add_relation(_tmp516,n2,Cyc_Relations_flip_relation(_tmp51A),n1,_tmp4D3);
struct _tuple19 _tmp996;struct _tuple19 _tmp51E=(_tmp996.f1=_tmp4DC,((_tmp996.f2=_tmp4DD,_tmp996)));struct _tuple19 _tmp51F=_tmp51E;struct Cyc_Dict_Dict _tmp520;struct Cyc_Dict_Dict _tmp521;struct Cyc_Dict_Dict _tmp522;struct Cyc_Dict_Dict _tmp523;_LL2C4: if(((_tmp51F.f1).ReachableFL).tag != 2)goto _LL2C6;_tmp520=((struct _tuple14)((_tmp51F.f1).ReachableFL).val).f1;if(((_tmp51F.f2).ReachableFL).tag != 2)goto _LL2C6;_tmp521=((struct _tuple14)((_tmp51F.f2).ReachableFL).val).f1;_LL2C5: {
# 2081
struct _tuple19 _tmp997;struct _tuple19 _tmp525=(_tmp997.f1=Cyc_CfFlowInfo_ReachableFL(_tmp520,_tmp51C),((_tmp997.f2=Cyc_CfFlowInfo_ReachableFL(_tmp521,_tmp51D),_tmp997)));_npop_handler(0);return _tmp525;}_LL2C6: if(((_tmp51F.f2).ReachableFL).tag != 2)goto _LL2C8;_tmp522=((struct _tuple14)((_tmp51F.f2).ReachableFL).val).f1;_LL2C7: {
# 2083
struct _tuple19 _tmp998;struct _tuple19 _tmp527=(_tmp998.f1=_tmp4DC,((_tmp998.f2=Cyc_CfFlowInfo_ReachableFL(_tmp522,_tmp51D),_tmp998)));_npop_handler(0);return _tmp527;}_LL2C8: if(((_tmp51F.f1).ReachableFL).tag != 2)goto _LL2CA;_tmp523=((struct _tuple14)((_tmp51F.f1).ReachableFL).val).f1;_LL2C9: {
# 2085
struct _tuple19 _tmp999;struct _tuple19 _tmp529=(_tmp999.f1=Cyc_CfFlowInfo_ReachableFL(_tmp523,_tmp51C),((_tmp999.f2=_tmp4DD,_tmp999)));_npop_handler(0);return _tmp529;}_LL2CA:;_LL2CB: {
# 2087
struct _tuple19 _tmp99A;struct _tuple19 _tmp52B=(_tmp99A.f1=_tmp4DC,((_tmp99A.f2=_tmp4DD,_tmp99A)));_npop_handler(0);return _tmp52B;}_LL2C3:;}else{
# 2090
struct _tuple19 _tmp99B;struct _tuple19 _tmp52E=(_tmp99B.f1=_tmp4DC,((_tmp99B.f2=_tmp4DD,_tmp99B)));_npop_handler(0);return _tmp52E;}};};};}_LL28E:;};
# 1987
;_pop_region(temp);}
# 2096
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2098
struct Cyc_CfFlowInfo_FlowEnv*_tmp52F=env->fenv;
void*_tmp530=e->r;void*_tmp531=_tmp530;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp538;struct Cyc_Absyn_Exp*_tmp53A;struct Cyc_Absyn_Exp*_tmp53B;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp540;enum Cyc_Absyn_Primop _tmp542;struct Cyc_List_List*_tmp543;_LL2CD: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp532=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp531;if(_tmp532->tag != 5)goto _LL2CF;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;_tmp535=_tmp532->f3;}}_LL2CE: {
# 2101
struct _tuple19 _tmp544=Cyc_NewControlFlow_anal_test(env,inflow,_tmp533);union Cyc_CfFlowInfo_FlowInfo _tmp546;union Cyc_CfFlowInfo_FlowInfo _tmp547;struct _tuple19 _tmp545=_tmp544;_tmp546=_tmp545.f1;_tmp547=_tmp545.f2;{
struct _tuple19 _tmp548=Cyc_NewControlFlow_anal_test(env,_tmp546,_tmp534);union Cyc_CfFlowInfo_FlowInfo _tmp54A;union Cyc_CfFlowInfo_FlowInfo _tmp54B;struct _tuple19 _tmp549=_tmp548;_tmp54A=_tmp549.f1;_tmp54B=_tmp549.f2;{
struct _tuple19 _tmp54C=Cyc_NewControlFlow_anal_test(env,_tmp547,_tmp535);union Cyc_CfFlowInfo_FlowInfo _tmp54E;union Cyc_CfFlowInfo_FlowInfo _tmp54F;struct _tuple19 _tmp54D=_tmp54C;_tmp54E=_tmp54D.f1;_tmp54F=_tmp54D.f2;{
struct _tuple19 _tmp99C;return(_tmp99C.f1=Cyc_CfFlowInfo_join_flow(_tmp52F,env->all_changed,_tmp54A,_tmp54E),((_tmp99C.f2=
Cyc_CfFlowInfo_join_flow(_tmp52F,env->all_changed,_tmp54B,_tmp54F),_tmp99C)));};};};}_LL2CF: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp536=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp531;if(_tmp536->tag != 6)goto _LL2D1;else{_tmp537=_tmp536->f1;_tmp538=_tmp536->f2;}}_LL2D0: {
# 2107
struct _tuple19 _tmp551=Cyc_NewControlFlow_anal_test(env,inflow,_tmp537);union Cyc_CfFlowInfo_FlowInfo _tmp553;union Cyc_CfFlowInfo_FlowInfo _tmp554;struct _tuple19 _tmp552=_tmp551;_tmp553=_tmp552.f1;_tmp554=_tmp552.f2;{
struct _tuple19 _tmp555=Cyc_NewControlFlow_anal_test(env,_tmp553,_tmp538);union Cyc_CfFlowInfo_FlowInfo _tmp557;union Cyc_CfFlowInfo_FlowInfo _tmp558;struct _tuple19 _tmp556=_tmp555;_tmp557=_tmp556.f1;_tmp558=_tmp556.f2;{
struct _tuple19 _tmp99D;return(_tmp99D.f1=_tmp557,((_tmp99D.f2=Cyc_CfFlowInfo_join_flow(_tmp52F,env->all_changed,_tmp554,_tmp558),_tmp99D)));};};}_LL2D1: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp539=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp531;if(_tmp539->tag != 7)goto _LL2D3;else{_tmp53A=_tmp539->f1;_tmp53B=_tmp539->f2;}}_LL2D2: {
# 2111
struct _tuple19 _tmp55A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp53A);union Cyc_CfFlowInfo_FlowInfo _tmp55C;union Cyc_CfFlowInfo_FlowInfo _tmp55D;struct _tuple19 _tmp55B=_tmp55A;_tmp55C=_tmp55B.f1;_tmp55D=_tmp55B.f2;{
struct _tuple19 _tmp55E=Cyc_NewControlFlow_anal_test(env,_tmp55D,_tmp53B);union Cyc_CfFlowInfo_FlowInfo _tmp560;union Cyc_CfFlowInfo_FlowInfo _tmp561;struct _tuple19 _tmp55F=_tmp55E;_tmp560=_tmp55F.f1;_tmp561=_tmp55F.f2;{
struct _tuple19 _tmp99E;return(_tmp99E.f1=Cyc_CfFlowInfo_join_flow(_tmp52F,env->all_changed,_tmp55C,_tmp560),((_tmp99E.f2=_tmp561,_tmp99E)));};};}_LL2D3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp53C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp531;if(_tmp53C->tag != 8)goto _LL2D5;else{_tmp53D=_tmp53C->f1;_tmp53E=_tmp53C->f2;}}_LL2D4: {
# 2115
struct _tuple16 _tmp563=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp53D);union Cyc_CfFlowInfo_FlowInfo _tmp565;void*_tmp566;struct _tuple16 _tmp564=_tmp563;_tmp565=_tmp564.f1;_tmp566=_tmp564.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp565,_tmp53E);}_LL2D5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp53F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp531;if(_tmp53F->tag != 2)goto _LL2D7;else{if(_tmp53F->f1 != Cyc_Absyn_Not)goto _LL2D7;if(_tmp53F->f2 == 0)goto _LL2D7;_tmp540=(struct Cyc_Absyn_Exp*)(_tmp53F->f2)->hd;if((_tmp53F->f2)->tl != 0)goto _LL2D7;}}_LL2D6: {
# 2118
struct _tuple19 _tmp567=Cyc_NewControlFlow_anal_test(env,inflow,_tmp540);union Cyc_CfFlowInfo_FlowInfo _tmp569;union Cyc_CfFlowInfo_FlowInfo _tmp56A;struct _tuple19 _tmp568=_tmp567;_tmp569=_tmp568.f1;_tmp56A=_tmp568.f2;{
struct _tuple19 _tmp99F;return(_tmp99F.f1=_tmp56A,((_tmp99F.f2=_tmp569,_tmp99F)));};}_LL2D7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp531;if(_tmp541->tag != 2)goto _LL2D9;else{_tmp542=_tmp541->f1;_tmp543=_tmp541->f2;}}_LL2D8:
# 2121
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp542,_tmp543);_LL2D9:;_LL2DA: {
# 2125
struct _tuple16 _tmp56C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp56E;void*_tmp56F;struct _tuple16 _tmp56D=_tmp56C;_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp570=_tmp56E;struct Cyc_Dict_Dict _tmp571;_LL2DC: if((_tmp570.BottomFL).tag != 1)goto _LL2DE;_LL2DD: {
struct _tuple19 _tmp9A0;return(_tmp9A0.f1=_tmp56E,((_tmp9A0.f2=_tmp56E,_tmp9A0)));}_LL2DE: if((_tmp570.ReachableFL).tag != 2)goto _LL2DB;_tmp571=((struct _tuple14)(_tmp570.ReachableFL).val).f1;_LL2DF: {
# 2129
struct _tuple15 _tmp573=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp56F);void*_tmp575;struct Cyc_List_List*_tmp576;struct _tuple15 _tmp574=_tmp573;_tmp575=_tmp574.f1;_tmp576=_tmp574.f2;{
void*_tmp577=_tmp575;struct Cyc_Absyn_Vardecl*_tmp579;void*_tmp57A;enum Cyc_CfFlowInfo_InitLevel _tmp583;_LL2E1: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp578=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp578->tag != 8)goto _LL2E3;else{_tmp579=_tmp578->f1;_tmp57A=(void*)_tmp578->f2;}}_LL2E2: {
# 2132
struct Cyc_Core_Impossible_exn_struct _tmp9A6;const char*_tmp9A5;struct Cyc_Core_Impossible_exn_struct*_tmp9A4;(int)_throw((void*)((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A6.tag=Cyc_Core_Impossible,((_tmp9A6.f1=((_tmp9A5="anal_test: bad namedlocation",_tag_dyneither(_tmp9A5,sizeof(char),29))),_tmp9A6)))),_tmp9A4)))));}_LL2E3: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp57B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57B->tag != 0)goto _LL2E5;}_LL2E4: {
struct _tuple19 _tmp9A7;return(_tmp9A7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp9A7.f2=_tmp56E,_tmp9A7)));}_LL2E5: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp57C=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57C->tag != 2)goto _LL2E7;}_LL2E6:
 goto _LL2E8;_LL2E7: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp57D=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57D->tag != 1)goto _LL2E9;}_LL2E8:
 goto _LL2EA;_LL2E9: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp57E=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57E->tag != 5)goto _LL2EB;}_LL2EA: {
struct _tuple19 _tmp9A8;return(_tmp9A8.f1=_tmp56E,((_tmp9A8.f2=Cyc_CfFlowInfo_BottomFL(),_tmp9A8)));}_LL2EB: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp57F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp57F->tag != 3)goto _LL2ED;else{if(_tmp57F->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2ED;}}_LL2EC:
 goto _LL2EE;_LL2ED: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp580=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp580->tag != 4)goto _LL2EF;else{if(_tmp580->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2EF;}}_LL2EE:
 goto _LL2F0;_LL2EF: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp581=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp581->tag != 7)goto _LL2F1;}_LL2F0:
# 2140
{const char*_tmp9AB;void*_tmp9AA;(_tmp9AA=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp9AB="expression may not be initialized",_tag_dyneither(_tmp9AB,sizeof(char),34))),_tag_dyneither(_tmp9AA,sizeof(void*),0)));}{
struct _tuple19 _tmp9AC;return(_tmp9AC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp9AC.f2=Cyc_CfFlowInfo_BottomFL(),_tmp9AC)));};_LL2F1: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp582=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp582->tag != 3)goto _LL2F3;else{_tmp583=_tmp582->f1;}}_LL2F2:
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp56E,e,_tmp583,_tmp576);_LL2F3: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp584=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp584->tag != 4)goto _LL2F5;}_LL2F4: {
struct _tuple19 _tmp9AD;return(_tmp9AD.f1=_tmp56E,((_tmp9AD.f2=_tmp56E,_tmp9AD)));}_LL2F5: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp585=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp577;if(_tmp585->tag != 6)goto _LL2E0;}_LL2F6: {
struct Cyc_Core_Impossible_exn_struct _tmp9B3;const char*_tmp9B2;struct Cyc_Core_Impossible_exn_struct*_tmp9B1;(int)_throw((void*)((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B3.tag=Cyc_Core_Impossible,((_tmp9B3.f1=((_tmp9B2="anal_test",_tag_dyneither(_tmp9B2,sizeof(char),10))),_tmp9B3)))),_tmp9B1)))));}_LL2E0:;};}_LL2DB:;};}_LL2CC:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2150
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2152
unsigned int _tmp593;struct Cyc_NewControlFlow_AnalEnv*_tmp594;struct Cyc_Dict_Dict _tmp595;struct _tuple27*_tmp592=ckenv;_tmp593=_tmp592->f1;_tmp594=_tmp592->f2;_tmp595=_tmp592->f3;{
void*_tmp596=root;struct Cyc_Absyn_Vardecl*_tmp598;_LL2F8: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp597=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp596;if(_tmp597->tag != 0)goto _LL2FA;else{_tmp598=_tmp597->f1;}}_LL2F9:
# 2155
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp594->fenv)->r,_tmp598->type)){
struct _tuple15 _tmp599=Cyc_CfFlowInfo_unname_rval((_tmp594->fenv)->r,rval);void*_tmp59B;struct _tuple15 _tmp59A=_tmp599;_tmp59B=_tmp59A.f1;{
void*_tmp59C=_tmp59B;_LL2FD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp59D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp59C;if(_tmp59D->tag != 7)goto _LL2FF;}_LL2FE:
 goto _LL300;_LL2FF: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp59E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp59C;if(_tmp59E->tag != 0)goto _LL301;}_LL300:
 goto _LL302;_LL301: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp59F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp59C;if(_tmp59F->tag != 3)goto _LL303;else{if(_tmp59F->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL303;}}_LL302:
 goto _LL2FC;_LL303:;_LL304:
# 2162
{const char*_tmp9B7;void*_tmp9B6[1];struct Cyc_String_pa_PrintArg_struct _tmp9B5;(_tmp9B5.tag=0,((_tmp9B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp598->name)),((_tmp9B6[0]=& _tmp9B5,Cyc_Tcutil_warn(_tmp593,((_tmp9B7="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp9B7,sizeof(char),57))),_tag_dyneither(_tmp9B6,sizeof(void*),1)))))));}
goto _LL2FC;_LL2FC:;};}
# 2166
goto _LL2F7;_LL2FA:;_LL2FB:
 goto _LL2F7;_LL2F7:;};}
# 2171
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp5A3=flow;struct Cyc_Dict_Dict _tmp5A4;_LL306: if((_tmp5A3.BottomFL).tag != 1)goto _LL308;_LL307:
 return;_LL308: if((_tmp5A3.ReachableFL).tag != 2)goto _LL305;_tmp5A4=((struct _tuple14)(_tmp5A3.ReachableFL).val).f1;_LL309:
# 2175
{struct Cyc_List_List*_tmp5A5=env->param_roots;for(0;_tmp5A5 != 0;_tmp5A5=_tmp5A5->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5A4,Cyc_CfFlowInfo_lookup_place(_tmp5A4,(struct Cyc_CfFlowInfo_Place*)_tmp5A5->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp9BA;void*_tmp9B9;(_tmp9B9=0,Cyc_CfFlowInfo_aerr(loc,((_tmp9BA="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp9BA,sizeof(char),76))),_tag_dyneither(_tmp9B9,sizeof(void*),0)));}}}
# 2179
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp9BB;struct _tuple27 _tmp5A8=(_tmp9BB.f1=loc,((_tmp9BB.f2=env,((_tmp9BB.f3=_tmp5A4,_tmp9BB)))));
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp5A8,_tmp5A4);}
# 2183
return;_LL305:;}
# 2192
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2195
struct Cyc_List_List*_tmp5AA=0;
struct Cyc_List_List*_tmp5AB=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp5AC=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AF;struct _tuple24*_tmp5AD=_tmp5AC;_tmp5AE=_tmp5AD->f1;_tmp5AF=_tmp5AD->f2;
if((_tmp5AE != 0  && _tmp5AF != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp5AF->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9BE;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9BD;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5B0=(_tmp9BD=_region_malloc(frgn,sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=0,((_tmp9BE.f1=*_tmp5AE,_tmp9BE)))),_tmp9BD)));
struct Cyc_CfFlowInfo_Place*_tmp9BF;struct Cyc_CfFlowInfo_Place*_tmp5B1=(_tmp9BF=_region_malloc(frgn,sizeof(*_tmp9BF)),((_tmp9BF->root=(void*)_tmp5B0,((_tmp9BF->fields=0,_tmp9BF)))));
{struct Cyc_List_List*_tmp9C0;_tmp5AA=((_tmp9C0=_region_malloc(frgn,sizeof(*_tmp9C0)),((_tmp9C0->hd=_tmp5B1,((_tmp9C0->tl=_tmp5AA,_tmp9C0))))));}{
struct Cyc_List_List*_tmp9C1;_tmp5AB=((_tmp9C1=_region_malloc(frgn,sizeof(*_tmp9C1)),((_tmp9C1->hd=_tmp5AF,((_tmp9C1->tl=_tmp5AB,_tmp9C1))))));};}}}{
# 2207
struct _tuple1 _tmp9C2;return(_tmp9C2.f1=_tmp5AA,((_tmp9C2.f2=_tmp5AB,_tmp9C2)));};}struct _tuple28{int f1;void*f2;};
# 2213
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2220
union Cyc_CfFlowInfo_FlowInfo _tmp5B8=flow;struct Cyc_Dict_Dict _tmp5B9;_LL30B: if((_tmp5B8.BottomFL).tag != 1)goto _LL30D;_LL30C: {
# 2222
const char*_tmp9C5;void*_tmp9C4;(_tmp9C4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C5="noconsume_place_ok: flow became Bottom!",_tag_dyneither(_tmp9C5,sizeof(char),40))),_tag_dyneither(_tmp9C4,sizeof(void*),0)));}_LL30D: if((_tmp5B8.ReachableFL).tag != 2)goto _LL30A;_tmp5B9=((struct _tuple14)(_tmp5B8.ReachableFL).val).f1;_LL30E: {
# 2229
struct Cyc_Absyn_Exp*_tmp5BC=Cyc_Absyn_exp_unsigned_one;
int _tmp5BD=0;
int _tmp5BE=1;
void*_tmp5BF=Cyc_CfFlowInfo_lookup_place(_tmp5B9,place);
void*_tmp5C0=_tmp5BF;
# 2241
int varok=0;
{void*_tmp5C1=_tmp5BF;struct Cyc_Absyn_Vardecl*_tmp5C3;void*_tmp5C4;_LL310: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C1;if(_tmp5C2->tag != 8)goto _LL312;else{_tmp5C3=_tmp5C2->f1;_tmp5C4=(void*)_tmp5C2->f2;}}_LL311:
# 2244
 if(vd == _tmp5C3){
_tmp5C0=_tmp5C4;
# 2247
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
# 2249
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp5BC,_tmp5BE,_tmp5C0,& _tmp5BD)){
if(!do_unconsume){
const char*_tmp9C9;void*_tmp9C8[1];struct Cyc_String_pa_PrintArg_struct _tmp9C7;(_tmp9C7.tag=0,((_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2253
Cyc_Absynpp_qvar2string(vd->name)),((_tmp9C8[0]=& _tmp9C7,Cyc_CfFlowInfo_aerr(loc,((_tmp9C9="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp9C9,sizeof(char),58))),_tag_dyneither(_tmp9C8,sizeof(void*),1)))))));}}else{
# 2256
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5B9,_tmp5C0)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume){
const char*_tmp9CD;void*_tmp9CC[1];struct Cyc_String_pa_PrintArg_struct _tmp9CB;(_tmp9CB.tag=0,((_tmp9CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2259
Cyc_Absynpp_qvar2string(vd->name)),((_tmp9CC[0]=& _tmp9CB,Cyc_CfFlowInfo_aerr(loc,((_tmp9CD="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp9CD,sizeof(char),84))),_tag_dyneither(_tmp9CC,sizeof(void*),1)))))));}else{
# 2261
varok=1;}}}else{
# 2264
varok=1;}}else{
# 2267
goto _LL313;}
goto _LL30F;_LL312:;_LL313:
# 2271
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type))
varok=1;else{
if(!do_unconsume){
const char*_tmp9D1;void*_tmp9D0[1];struct Cyc_String_pa_PrintArg_struct _tmp9CF;(_tmp9CF.tag=0,((_tmp9CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2276
Cyc_Absynpp_qvar2string(vd->name)),((_tmp9D0[0]=& _tmp9CF,Cyc_CfFlowInfo_aerr(loc,((_tmp9D1="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp9D1,sizeof(char),85))),_tag_dyneither(_tmp9D0,sizeof(void*),1)))))));}}
goto _LL30F;_LL30F:;}{
# 2280
struct _tuple28 _tmp9D2;return(_tmp9D2.f1=varok,((_tmp9D2.f2=_tmp5C0,_tmp9D2)));};}_LL30A:;}
# 2286
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
void*_tmp5D0;struct Cyc_List_List*_tmp5D1;struct Cyc_CfFlowInfo_Place*_tmp5CF=p;_tmp5D0=_tmp5CF->root;_tmp5D1=_tmp5CF->fields;
if(_tmp5D1 != 0){
const char*_tmp9D5;void*_tmp9D4;(_tmp9D4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9D5="unconsume_params: param to unconsume is non-variable",_tag_dyneither(_tmp9D5,sizeof(char),53))),_tag_dyneither(_tmp9D4,sizeof(void*),0)));}{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp5D4=_tmp5D0;struct Cyc_Absyn_Vardecl*_tmp5D6;_LL315: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5D5=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp5D4;if(_tmp5D5->tag != 0)goto _LL317;else{_tmp5D6=_tmp5D5->f1;}}_LL316:
 return _tmp5D6;_LL317:;_LL318: {
const char*_tmp9D8;void*_tmp9D7;(_tmp9D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9D8="unconsume_params: root is not a varroot",_tag_dyneither(_tmp9D8,sizeof(char),40))),_tag_dyneither(_tmp9D7,sizeof(void*),0)));}_LL314:;};}
# 2305 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2313
{union Cyc_CfFlowInfo_FlowInfo _tmp5D9=flow;struct Cyc_Dict_Dict _tmp5DA;_LL31A: if((_tmp5D9.BottomFL).tag != 1)goto _LL31C;_LL31B:
 return flow;_LL31C: if((_tmp5D9.ReachableFL).tag != 2)goto _LL319;_tmp5DA=((struct _tuple14)(_tmp5D9.ReachableFL).val).f1;_LL31D: {
# 2320
struct _tuple18 _tmp5DB=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);union Cyc_CfFlowInfo_FlowInfo _tmp5DD;union Cyc_CfFlowInfo_AbsLVal _tmp5DE;struct _tuple18 _tmp5DC=_tmp5DB;_tmp5DD=_tmp5DC.f1;_tmp5DE=_tmp5DC.f2;
# 2323
{union Cyc_CfFlowInfo_AbsLVal _tmp5DF=_tmp5DE;struct Cyc_CfFlowInfo_Place*_tmp5E0;_LL31F: if((_tmp5DF.PlaceL).tag != 1)goto _LL321;_tmp5E0=(struct Cyc_CfFlowInfo_Place*)(_tmp5DF.PlaceL).val;_LL320: {
# 2327
void*_tmp5E1=Cyc_CfFlowInfo_lookup_place(_tmp5DA,_tmp5E0);
{void*_tmp5E2=_tmp5E1;struct Cyc_Absyn_Vardecl*_tmp5E4;void*_tmp5E5;_LL324: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5E3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5E2;if(_tmp5E3->tag != 8)goto _LL326;else{_tmp5E4=_tmp5E3->f1;_tmp5E5=(void*)_tmp5E3->f2;}}_LL325: {
# 2333
void*new_rval;
if(_tmp5E4 == vd){
# 2337
if(varok){
# 2339
_tmp5E1=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp5E5);
# 2344
if(ropt != 0){
# 2350
struct _tuple16 _tmp9DA;struct _tuple16 _tmp9D9;struct _tuple16 _tmp5E6=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2353
(_tmp9D9.f1=_tmp5DD,((_tmp9D9.f2=_tmp5E1,_tmp9D9)))),(
(_tmp9DA.f1=_tmp5DD,((_tmp9DA.f2=ropt,_tmp9DA)))));
# 2350
union Cyc_CfFlowInfo_FlowInfo _tmp5E8;void*_tmp5E9;struct _tuple16 _tmp5E7=_tmp5E6;_tmp5E8=_tmp5E7.f1;_tmp5E9=_tmp5E7.f2;
# 2355
_tmp5DD=_tmp5E8;new_rval=_tmp5E9;}else{
# 2360
new_rval=_tmp5E1;}}else{
# 2363
new_rval=_tmp5E5;}
# 2365
{union Cyc_CfFlowInfo_FlowInfo _tmp5EC=_tmp5DD;struct Cyc_Dict_Dict _tmp5ED;struct Cyc_List_List*_tmp5EE;_LL329: if((_tmp5EC.ReachableFL).tag != 2)goto _LL32B;_tmp5ED=((struct _tuple14)(_tmp5EC.ReachableFL).val).f1;_tmp5EE=((struct _tuple14)(_tmp5EC.ReachableFL).val).f2;_LL32A:
# 2367
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp5ED,env->all_changed,_tmp5E0,new_rval),_tmp5EE);
# 2371
goto _LL328;_LL32B:;_LL32C: {
# 2373
const char*_tmp9DD;void*_tmp9DC;(_tmp9DC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9DD="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp9DD,sizeof(char),42))),_tag_dyneither(_tmp9DC,sizeof(void*),0)));}_LL328:;}
# 2375
goto _LL323;}else{
# 2377
goto _LL327;}
goto _LL323;}_LL326:;_LL327:
# 2384
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
const char*_tmp9E1;void*_tmp9E0[1];struct Cyc_String_pa_PrintArg_struct _tmp9DF;(_tmp9DF.tag=0,((_tmp9DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp)),((_tmp9E0[0]=& _tmp9DF,Cyc_CfFlowInfo_aerr(loc,((_tmp9E1="aliased expression %s was overwritten",_tag_dyneither(_tmp9E1,sizeof(char),38))),_tag_dyneither(_tmp9E0,sizeof(void*),1)))))));}
# 2401 "new_control_flow.cyc"
goto _LL323;_LL323:;}
# 2403
goto _LL31E;}_LL321:;_LL322:
# 2409
 goto _LL31E;_LL31E:;}
# 2411
goto _LL319;}_LL319:;}
# 2413
return flow;}
# 2426 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2432
{union Cyc_CfFlowInfo_FlowInfo _tmp5F4=flow;_LL32E: if((_tmp5F4.BottomFL).tag != 1)goto _LL330;_LL32F:
 return flow;_LL330:;_LL331:
 goto _LL32D;_LL32D:;}{
# 2436
int _tmp5F5=unconsume_exps != 0;
{struct Cyc_List_List*_tmp5F6=consumed_vals;for(0;_tmp5F6 != 0;
(_tmp5F6=_tmp5F6->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2443
struct Cyc_Absyn_Vardecl*_tmp5F7=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp5F6->hd);
struct _tuple28 _tmp9E2;struct _tuple28 _tmp5F8=
is_region_open?(_tmp9E2.f1=1,((_tmp9E2.f2=0,_tmp9E2))):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp5F6->hd,_tmp5F5,_tmp5F7,flow,loc);
# 2444
int _tmp5FA;void*_tmp5FB;struct _tuple28 _tmp5F9=_tmp5F8;_tmp5FA=_tmp5F9.f1;_tmp5FB=_tmp5F9.f2;
# 2450
if(_tmp5F5)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,_tmp5F7,_tmp5FA,_tmp5FB,flow,loc);}}
# 2453
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2457
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2460
struct Cyc_CfFlowInfo_FlowEnv*_tmp5FD=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp5FE=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp600;struct Cyc_Absyn_Exp*_tmp601;struct Cyc_Absyn_Stmt*_tmp602;unsigned int _tmp603;struct Cyc_Absyn_Switch_clause*_tmp5FF=_tmp5FE;_tmp600=_tmp5FF->pat_vars;_tmp601=_tmp5FF->where_clause;_tmp602=_tmp5FF->body;_tmp603=_tmp5FF->loc;{
struct _tuple1 _tmp604=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp600))->v);struct Cyc_List_List*_tmp606;struct Cyc_List_List*_tmp607;struct _tuple1 _tmp605=_tmp604;_tmp606=_tmp605.f1;_tmp607=_tmp605.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp600->v,_tmp606 != 0,_tmp603,exp_loc);
# 2468
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp608=env->unique_pat_vars;
{struct _tuple29*_tmp9E5;struct Cyc_List_List*_tmp9E4;env->unique_pat_vars=(
(_tmp9E4=_region_malloc(env->r,sizeof(*_tmp9E4)),((_tmp9E4->hd=((_tmp9E5=_region_malloc(env->r,sizeof(*_tmp9E5)),((_tmp9E5->f1=0,((_tmp9E5->f2=_tmp602,((_tmp9E5->f3=_tmp606,((_tmp9E5->f4=_tmp607,_tmp9E5)))))))))),((_tmp9E4->tl=_tmp608,_tmp9E4))))));}
# 2473
if(_tmp601 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp601;
struct _tuple19 _tmp60B=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp60D;union Cyc_CfFlowInfo_FlowInfo _tmp60E;struct _tuple19 _tmp60C=_tmp60B;_tmp60D=_tmp60C.f1;_tmp60E=_tmp60C.f2;
inflow=_tmp60E;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp60D,_tmp602,0);}else{
# 2479
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp602,0);}
# 2481
env->unique_pat_vars=_tmp608;{
union Cyc_CfFlowInfo_FlowInfo _tmp60F=clause_outflow;_LL333: if((_tmp60F.BottomFL).tag != 1)goto _LL335;_LL334:
 goto _LL332;_LL335:;_LL336:
# 2486
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp606,_tmp607,0,clause_outflow,_tmp603);
# 2488
if(scs->tl == 0)
return clause_outflow;else{
# 2493
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp9E8;void*_tmp9E7;(_tmp9E7=0,Cyc_CfFlowInfo_aerr(_tmp602->loc,((_tmp9E8="switch clause may implicitly fallthru",_tag_dyneither(_tmp9E8,sizeof(char),38))),_tag_dyneither(_tmp9E7,sizeof(void*),0)));}else{
# 2496
const char*_tmp9EB;void*_tmp9EA;(_tmp9EA=0,Cyc_Tcutil_warn(_tmp602->loc,((_tmp9EB="switch clause may implicitly fallthru",_tag_dyneither(_tmp9EB,sizeof(char),38))),_tag_dyneither(_tmp9EA,sizeof(void*),0)));}
# 2498
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2500
goto _LL332;_LL332:;};};};}
# 2503
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2508
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2510
struct Cyc_NewControlFlow_AnalEnv*_tmp615;struct Cyc_Dict_Dict _tmp616;unsigned int _tmp617;struct _tuple30*_tmp614=vdenv;_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;_tmp617=_tmp614->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp615->fenv)->r,vd->type)){
# 2513
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9EC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp9EC.tag=0,((_tmp9EC.f1=vd,_tmp9EC)));
# 2515
struct _tuple15 _tmp618=Cyc_CfFlowInfo_unname_rval((_tmp615->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp616,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2515
void*_tmp61A;struct _tuple15 _tmp619=_tmp618;_tmp61A=_tmp619.f1;{
# 2517
void*_tmp61B=_tmp61A;_LL338: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp61C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp61B;if(_tmp61C->tag != 7)goto _LL33A;}_LL339:
 goto _LL33B;_LL33A: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp61D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp61B;if(_tmp61D->tag != 0)goto _LL33C;}_LL33B:
 goto _LL33D;_LL33C: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp61E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp61B;if(_tmp61E->tag != 3)goto _LL33E;else{if(_tmp61E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL33E;}}_LL33D:
 goto _LL337;_LL33E:;_LL33F:
# 2522
{const char*_tmp9F0;void*_tmp9EF[1];struct Cyc_String_pa_PrintArg_struct _tmp9EE;(_tmp9EE.tag=0,((_tmp9EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp9EF[0]=& _tmp9EE,Cyc_Tcutil_warn(_tmp617,((_tmp9F0="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp9F0,sizeof(char),68))),_tag_dyneither(_tmp9EF,sizeof(void*),1)))))));}
# 2524
goto _LL337;_LL337:;};}}
# 2529
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2531
{union Cyc_CfFlowInfo_FlowInfo _tmp623=inflow;struct Cyc_Dict_Dict _tmp624;_LL341: if((_tmp623.ReachableFL).tag != 2)goto _LL343;_tmp624=((struct _tuple14)(_tmp623.ReachableFL).val).f1;_LL342: {
# 2533
struct _tuple30 _tmp9F1;struct _tuple30 _tmp625=(_tmp9F1.f1=env,((_tmp9F1.f2=_tmp624,((_tmp9F1.f3=decl->loc,_tmp9F1)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp626=env->fenv;
{void*_tmp627=decl->r;void*_tmp628=_tmp627;struct Cyc_Absyn_Vardecl*_tmp62A;struct Cyc_List_List*_tmp62C;struct Cyc_List_List*_tmp62E;_LL346: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp629=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp628;if(_tmp629->tag != 0)goto _LL348;else{_tmp62A=_tmp629->f1;}}_LL347:
# 2537
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp625,_tmp62A);
goto _LL345;_LL348: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp62B=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp628;if(_tmp62B->tag != 2)goto _LL34A;else{if(_tmp62B->f2 == 0)goto _LL34A;_tmp62C=(struct Cyc_List_List*)(_tmp62B->f2)->v;}}_LL349: {
# 2540
struct _tuple1 _tmp62F=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp62C);struct Cyc_List_List*_tmp631;struct _tuple1 _tmp630=_tmp62F;_tmp631=_tmp630.f1;{
struct Cyc_List_List*_tmp632=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp631);
_tmp62E=_tmp632;goto _LL34B;};}_LL34A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp62D=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp628;if(_tmp62D->tag != 3)goto _LL34C;else{_tmp62E=_tmp62D->f1;}}_LL34B:
# 2544
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp625,_tmp62E);
goto _LL345;_LL34C:;_LL34D:
 goto _LL345;_LL345:;}
# 2548
goto _LL340;}_LL343:;_LL344:
 goto _LL340;_LL340:;}
# 2551
return;}
# 2557
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2560
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple29*_tmp634=(struct _tuple29*)x->hd;int _tmp636;struct Cyc_Absyn_Stmt*_tmp637;struct Cyc_List_List*_tmp638;struct Cyc_List_List*_tmp639;struct _tuple29*_tmp635=_tmp634;_tmp636=_tmp635->f1;_tmp637=_tmp635->f2;_tmp638=_tmp635->f3;_tmp639=_tmp635->f4;
keep_going=0;{
# 2578 "new_control_flow.cyc"
struct Cyc_Absyn_Stmt*_tmp63A=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
# 2581
while(_tmp63A != _tmp637){
struct Cyc_Absyn_Stmt*_tmp63B=(Cyc_NewControlFlow_get_stmt_annot(_tmp63A))->encloser;
if(_tmp63B == _tmp63A){
# 2586
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp638,_tmp639,_tmp636,inflow,dest->loc);
keep_going=1;
break;}
# 2590
_tmp63A=_tmp63B;}};}}
# 2597
return inflow;}
# 2603
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2606
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp63C=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp63E;union Cyc_CfFlowInfo_FlowInfo*_tmp63F;struct _tuple20 _tmp63D=_tmp63C;_tmp63E=_tmp63D.f1;_tmp63F=_tmp63D.f2;
inflow=*_tmp63F;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp640=env->fenv;
# 2613
void*_tmp641=s->r;void*_tmp642=_tmp641;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*_tmp649;struct Cyc_Absyn_Stmt*_tmp64B;struct Cyc_Absyn_Stmt*_tmp64C;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Absyn_Stmt*_tmp64F;struct Cyc_Absyn_Stmt*_tmp650;struct Cyc_Absyn_Exp*_tmp652;struct Cyc_Absyn_Stmt*_tmp653;struct Cyc_Absyn_Stmt*_tmp654;struct Cyc_Absyn_Stmt*_tmp656;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Stmt*_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp65B;struct Cyc_Absyn_Stmt*_tmp65C;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_Absyn_Stmt*_tmp65E;struct Cyc_Absyn_Stmt*_tmp65F;struct Cyc_List_List*_tmp661;struct Cyc_Absyn_Switch_clause*_tmp662;struct Cyc_Absyn_Stmt*_tmp664;struct Cyc_Absyn_Stmt*_tmp666;struct Cyc_Absyn_Stmt*_tmp668;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_List_List*_tmp66B;struct Cyc_Absyn_Stmt*_tmp66D;struct Cyc_List_List*_tmp66E;struct Cyc_List_List*_tmp671;struct Cyc_Absyn_Exp*_tmp672;unsigned int _tmp673;struct Cyc_Absyn_Stmt*_tmp674;struct Cyc_Absyn_Vardecl*_tmp677;struct Cyc_Absyn_Exp*_tmp678;unsigned int _tmp679;struct Cyc_Absyn_Stmt*_tmp67A;struct Cyc_Absyn_Decl*_tmp67C;struct Cyc_Absyn_Stmt*_tmp67D;struct Cyc_Absyn_Stmt*_tmp67F;struct Cyc_Absyn_Exp*_tmp681;_LL34F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp643=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp643->tag != 0)goto _LL351;}_LL350:
 return inflow;_LL351: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp644=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp644->tag != 6)goto _LL353;else{if(_tmp644->f1 != 0)goto _LL353;}}_LL352:
# 2619
{union Cyc_CfFlowInfo_FlowInfo _tmp682=inflow;_LL37C: if((_tmp682.ReachableFL).tag != 2)goto _LL37E;_LL37D:
# 2621
{void*_tmp683=Cyc_Tcutil_compress(env->return_type);void*_tmp684=_tmp683;_LL381: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp685=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp684;if(_tmp685->tag != 0)goto _LL383;}_LL382:
 goto _LL380;_LL383: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp686=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp684;if(_tmp686->tag != 7)goto _LL385;}_LL384:
 goto _LL386;_LL385: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp687=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp684;if(_tmp687->tag != 6)goto _LL387;}_LL386:
# 2625
{const char*_tmp9F4;void*_tmp9F3;(_tmp9F3=0,Cyc_Tcutil_warn(s->loc,((_tmp9F4="break may cause function not to return a value",_tag_dyneither(_tmp9F4,sizeof(char),47))),_tag_dyneither(_tmp9F3,sizeof(void*),0)));}
goto _LL380;_LL387:;_LL388:
# 2628
{const char*_tmp9F7;void*_tmp9F6;(_tmp9F6=0,Cyc_Tcutil_terr(s->loc,((_tmp9F7="break may cause function not to return a value",_tag_dyneither(_tmp9F7,sizeof(char),47))),_tag_dyneither(_tmp9F6,sizeof(void*),0)));}
goto _LL380;_LL380:;}
# 2631
goto _LL37B;_LL37E:;_LL37F:
 goto _LL37B;_LL37B:;}
# 2634
goto _LL354;_LL353: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp645=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp645->tag != 3)goto _LL355;else{if(_tmp645->f1 != 0)goto _LL355;}}_LL354:
# 2636
 if(env->noreturn){
const char*_tmp9FA;void*_tmp9F9;(_tmp9F9=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9FA="`noreturn' function might return",_tag_dyneither(_tmp9FA,sizeof(char),33))),_tag_dyneither(_tmp9F9,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL355: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp646=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp646->tag != 3)goto _LL357;else{_tmp647=_tmp646->f1;}}_LL356:
# 2642
 if(env->noreturn){
const char*_tmp9FD;void*_tmp9FC;(_tmp9FC=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9FD="`noreturn' function might return",_tag_dyneither(_tmp9FD,sizeof(char),33))),_tag_dyneither(_tmp9FC,sizeof(void*),0)));}{
struct _tuple16 _tmp690=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp647));union Cyc_CfFlowInfo_FlowInfo _tmp692;void*_tmp693;struct _tuple16 _tmp691=_tmp690;_tmp692=_tmp691.f1;_tmp693=_tmp691.f2;
_tmp692=Cyc_NewControlFlow_use_Rval(env,_tmp647->loc,_tmp692,_tmp693);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp692);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp692,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL357: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp648=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp648->tag != 1)goto _LL359;else{_tmp649=_tmp648->f1;}}_LL358: {
# 2651
struct _tuple17*_tmp694=rval_opt;void**_tmp695;int _tmp696;_LL38A: if(_tmp694 == 0)goto _LL38C;_tmp695=(void**)& _tmp694->f1;_tmp696=_tmp694->f2;_LL38B: {
# 2653
struct _tuple16 _tmp697=Cyc_NewControlFlow_anal_Rexp(env,_tmp696,inflow,_tmp649);union Cyc_CfFlowInfo_FlowInfo _tmp699;void*_tmp69A;struct _tuple16 _tmp698=_tmp697;_tmp699=_tmp698.f1;_tmp69A=_tmp698.f2;
*_tmp695=_tmp69A;
return _tmp699;}_LL38C: if(_tmp694 != 0)goto _LL389;_LL38D:
# 2657
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp649)).f1;_LL389:;}_LL359: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp64A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp64A->tag != 2)goto _LL35B;else{_tmp64B=_tmp64A->f1;_tmp64C=_tmp64A->f2;}}_LL35A:
# 2661
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp64B,0),_tmp64C,rval_opt);_LL35B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp64D=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp64D->tag != 4)goto _LL35D;else{_tmp64E=_tmp64D->f1;_tmp64F=_tmp64D->f2;_tmp650=_tmp64D->f3;}}_LL35C: {
# 2664
struct _tuple19 _tmp69B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp64E);union Cyc_CfFlowInfo_FlowInfo _tmp69D;union Cyc_CfFlowInfo_FlowInfo _tmp69E;struct _tuple19 _tmp69C=_tmp69B;_tmp69D=_tmp69C.f1;_tmp69E=_tmp69C.f2;{
# 2671
union Cyc_CfFlowInfo_FlowInfo _tmp69F=Cyc_NewControlFlow_anal_stmt(env,_tmp69E,_tmp650,0);
union Cyc_CfFlowInfo_FlowInfo _tmp6A0=Cyc_NewControlFlow_anal_stmt(env,_tmp69D,_tmp64F,0);
return Cyc_CfFlowInfo_join_flow(_tmp640,env->all_changed,_tmp6A0,_tmp69F);};}_LL35D: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp651=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp651->tag != 5)goto _LL35F;else{_tmp652=(_tmp651->f1).f1;_tmp653=(_tmp651->f1).f2;_tmp654=_tmp651->f2;}}_LL35E: {
# 2679
struct _tuple20 _tmp6A1=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp653);union Cyc_CfFlowInfo_FlowInfo*_tmp6A3;struct _tuple20 _tmp6A2=_tmp6A1;_tmp6A3=_tmp6A2.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6A4=*_tmp6A3;
struct _tuple19 _tmp6A5=Cyc_NewControlFlow_anal_test(env,_tmp6A4,_tmp652);union Cyc_CfFlowInfo_FlowInfo _tmp6A7;union Cyc_CfFlowInfo_FlowInfo _tmp6A8;struct _tuple19 _tmp6A6=_tmp6A5;_tmp6A7=_tmp6A6.f1;_tmp6A8=_tmp6A6.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6A9=Cyc_NewControlFlow_anal_stmt(env,_tmp6A7,_tmp654,0);
Cyc_NewControlFlow_update_flow(env,_tmp653,_tmp6A9);
return _tmp6A8;};};}_LL35F: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp655=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp655->tag != 14)goto _LL361;else{_tmp656=_tmp655->f1;_tmp657=(_tmp655->f2).f1;_tmp658=(_tmp655->f2).f2;}}_LL360: {
# 2689
union Cyc_CfFlowInfo_FlowInfo _tmp6AA=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp656,0);
struct _tuple20 _tmp6AB=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6AA,_tmp658);union Cyc_CfFlowInfo_FlowInfo*_tmp6AD;struct _tuple20 _tmp6AC=_tmp6AB;_tmp6AD=_tmp6AC.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6AE=*_tmp6AD;
struct _tuple19 _tmp6AF=Cyc_NewControlFlow_anal_test(env,_tmp6AE,_tmp657);union Cyc_CfFlowInfo_FlowInfo _tmp6B1;union Cyc_CfFlowInfo_FlowInfo _tmp6B2;struct _tuple19 _tmp6B0=_tmp6AF;_tmp6B1=_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;
Cyc_NewControlFlow_update_flow(env,_tmp656,_tmp6B1);
return _tmp6B2;};}_LL361: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp659=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp659->tag != 9)goto _LL363;else{_tmp65A=_tmp659->f1;_tmp65B=(_tmp659->f2).f1;_tmp65C=(_tmp659->f2).f2;_tmp65D=(_tmp659->f3).f1;_tmp65E=(_tmp659->f3).f2;_tmp65F=_tmp659->f4;}}_LL362: {
# 2698
union Cyc_CfFlowInfo_FlowInfo _tmp6B3=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp65A)).f1;
struct _tuple20 _tmp6B4=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6B3,_tmp65C);union Cyc_CfFlowInfo_FlowInfo*_tmp6B6;struct _tuple20 _tmp6B5=_tmp6B4;_tmp6B6=_tmp6B5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6B7=*_tmp6B6;
struct _tuple19 _tmp6B8=Cyc_NewControlFlow_anal_test(env,_tmp6B7,_tmp65B);union Cyc_CfFlowInfo_FlowInfo _tmp6BA;union Cyc_CfFlowInfo_FlowInfo _tmp6BB;struct _tuple19 _tmp6B9=_tmp6B8;_tmp6BA=_tmp6B9.f1;_tmp6BB=_tmp6B9.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6BC=Cyc_NewControlFlow_anal_stmt(env,_tmp6BA,_tmp65F,0);
struct _tuple20 _tmp6BD=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6BC,_tmp65E);union Cyc_CfFlowInfo_FlowInfo*_tmp6BF;struct _tuple20 _tmp6BE=_tmp6BD;_tmp6BF=_tmp6BE.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6C0=*_tmp6BF;
union Cyc_CfFlowInfo_FlowInfo _tmp6C1=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp6C0,_tmp65D)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp65C,_tmp6C1);
return _tmp6BB;};};};}_LL363: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp660=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp660->tag != 11)goto _LL365;else{_tmp661=_tmp660->f1;if(_tmp660->f2 == 0)goto _LL365;_tmp662=*_tmp660->f2;}}_LL364: {
# 2710
struct _RegionHandle _tmp6C2=_new_region("temp");struct _RegionHandle*temp=& _tmp6C2;_push_region(temp);
{struct _tuple23 _tmp6C3=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp661,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp6C5;struct Cyc_List_List*_tmp6C6;struct _tuple23 _tmp6C4=_tmp6C3;_tmp6C5=_tmp6C4.f1;_tmp6C6=_tmp6C4.f2;
# 2713
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,_tmp662->body);{
# 2715
struct Cyc_List_List*_tmp6C7=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp662->pat_vars))->v)).f1);
_tmp6C5=Cyc_NewControlFlow_add_vars(_tmp640,_tmp6C5,_tmp6C7,_tmp640->unknown_all,s->loc,0);
# 2718
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp662->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp6C8=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp6CA;struct Cyc_Absyn_Exp*_tmp6CB;struct _tuple24*_tmp6C9=_tmp6C8;_tmp6CA=_tmp6C9->f1;_tmp6CB=_tmp6C9->f2;
if(_tmp6CA != 0){
_tmp6C5=Cyc_NewControlFlow_do_initialize_var(_tmp640,env,_tmp6C5,*_tmp6CA,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp661))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6C6))->hd,s->loc);
_tmp6C6=_tmp6C6->tl;
_tmp661=_tmp661->tl;}}}
# 2726
Cyc_NewControlFlow_update_flow(env,_tmp662->body,_tmp6C5);{
union Cyc_CfFlowInfo_FlowInfo _tmp6CC=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp6CC;};};}
# 2711
;_pop_region(temp);}_LL365: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp663=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp663->tag != 6)goto _LL367;else{_tmp664=_tmp663->f1;}}_LL366:
# 2729
 _tmp666=_tmp664;goto _LL368;_LL367: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp665=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp665->tag != 7)goto _LL369;else{_tmp666=_tmp665->f1;}}_LL368:
 _tmp668=_tmp666;goto _LL36A;_LL369: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp667=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp667->tag != 8)goto _LL36B;else{_tmp668=_tmp667->f2;}}_LL36A:
# 2734
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp6CD=_tmp63E->encloser;
struct Cyc_Absyn_Stmt*_tmp6CE=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp668)))->encloser;
while(_tmp6CE != _tmp6CD){
struct Cyc_Absyn_Stmt*_tmp6CF=(Cyc_NewControlFlow_get_stmt_annot(_tmp6CD))->encloser;
if(_tmp6CF == _tmp6CD){
{const char*_tmpA00;void*_tmp9FF;(_tmp9FF=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmpA00="goto enters local scope or exception handler",_tag_dyneither(_tmpA00,sizeof(char),45))),_tag_dyneither(_tmp9FF,sizeof(void*),0)));}
break;}
# 2743
_tmp6CD=_tmp6CF;}}
# 2747
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp668));
# 2749
Cyc_NewControlFlow_update_flow(env,_tmp668,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL36B: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp669=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp669->tag != 10)goto _LL36D;else{_tmp66A=_tmp669->f1;_tmp66B=_tmp669->f2;}}_LL36C:
# 2755
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp66B,_tmp66A->loc);_LL36D: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp66C=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp66C->tag != 15)goto _LL36F;else{_tmp66D=_tmp66C->f1;_tmp66E=_tmp66C->f2;}}_LL36E: {
# 2760
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp66D,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2768
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2771
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp66E,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp6D2=scs_outflow;_LL38F: if((_tmp6D2.BottomFL).tag != 1)goto _LL391;_LL390:
 goto _LL38E;_LL391:;_LL392: {
const char*_tmpA03;void*_tmpA02;(_tmpA02=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmpA03="last catch clause may implicitly fallthru",_tag_dyneither(_tmpA03,sizeof(char),42))),_tag_dyneither(_tmpA02,sizeof(void*),0)));}_LL38E:;}
# 2777
outflow=s1_outflow;
# 2779
return outflow;};};}_LL36F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp66F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp66F->tag != 12)goto _LL371;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp670=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp66F->f1)->r;if(_tmp670->tag != 2)goto _LL371;else{if(_tmp670->f2 == 0)goto _LL371;_tmp671=(struct Cyc_List_List*)(_tmp670->f2)->v;_tmp672=_tmp670->f3;}}_tmp673=(_tmp66F->f1)->loc;_tmp674=_tmp66F->f2;}}_LL370: {
# 2789
struct _tuple1 _tmp6D5=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp671);struct Cyc_List_List*_tmp6D7;struct Cyc_List_List*_tmp6D8;struct _tuple1 _tmp6D6=_tmp6D5;_tmp6D7=_tmp6D6.f1;_tmp6D8=_tmp6D6.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp640,env,inflow,_tmp671,_tmp6D7 != 0,_tmp673,_tmp672->loc);{
struct Cyc_List_List*_tmp6D9=env->unique_pat_vars;
{struct _tuple29*_tmpA06;struct Cyc_List_List*_tmpA05;env->unique_pat_vars=(
(_tmpA05=_region_malloc(env->r,sizeof(*_tmpA05)),((_tmpA05->hd=((_tmpA06=_region_malloc(env->r,sizeof(*_tmpA06)),((_tmpA06->f1=0,((_tmpA06->f2=s,((_tmpA06->f3=_tmp6D7,((_tmpA06->f4=_tmp6D8,_tmpA06)))))))))),((_tmpA05->tl=_tmp6D9,_tmpA05))))));}
# 2797
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp674,rval_opt);
env->unique_pat_vars=_tmp6D9;
# 2801
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp6D7,_tmp6D8,0,inflow,_tmp673);
# 2805
return inflow;};}_LL371:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp675=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp675->tag != 12)goto _LL373;else{{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp676=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(_tmp675->f1)->r;if(_tmp676->tag != 4)goto _LL373;else{_tmp677=_tmp676->f2;_tmp678=_tmp676->f4;}}_tmp679=(_tmp675->f1)->loc;_tmp67A=_tmp675->f2;}}if(!(_tmp678 != 0))goto _LL373;_LL372: {
# 2817
struct Cyc_List_List _tmpA07;struct Cyc_List_List l=(_tmpA07.hd=_tmp678,((_tmpA07.tl=0,_tmpA07)));
union Cyc_CfFlowInfo_FlowInfo _tmp6DC=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp6DD=Cyc_NewControlFlow_anal_Lexp(env,_tmp6DC,0,0,_tmp678);union Cyc_CfFlowInfo_AbsLVal _tmp6DF;struct _tuple18 _tmp6DE=_tmp6DD;_tmp6DF=_tmp6DE.f2;{
struct _tuple16 _tmp6E0=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp6DC,_tmp678);union Cyc_CfFlowInfo_FlowInfo _tmp6E2;struct _tuple16 _tmp6E1=_tmp6E0;_tmp6E2=_tmp6E1.f1;{
struct Cyc_List_List*_tmp6E3=0;
struct Cyc_List_List*_tmp6E4=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp6E5=_tmp6E2;struct Cyc_Dict_Dict _tmp6E6;struct Cyc_List_List*_tmp6E7;_LL394: if((_tmp6E5.ReachableFL).tag != 2)goto _LL396;_tmp6E6=((struct _tuple14)(_tmp6E5.ReachableFL).val).f1;_tmp6E7=((struct _tuple14)(_tmp6E5.ReachableFL).val).f2;_LL395:
# 2825
{union Cyc_CfFlowInfo_AbsLVal _tmp6E8=_tmp6DF;struct Cyc_CfFlowInfo_Place*_tmp6E9;_LL399: if((_tmp6E8.PlaceL).tag != 1)goto _LL39B;_tmp6E9=(struct Cyc_CfFlowInfo_Place*)(_tmp6E8.PlaceL).val;_LL39A: {
# 2829
void*_tmp6EA=Cyc_CfFlowInfo_lookup_place(_tmp6E6,_tmp6E9);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpA0A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpA09;_tmp6EA=(void*)((_tmpA09=_region_malloc(_tmp640->r,sizeof(*_tmpA09)),((_tmpA09[0]=((_tmpA0A.tag=8,((_tmpA0A.f1=_tmp677,((_tmpA0A.f2=_tmp6EA,_tmpA0A)))))),_tmpA09))));}
_tmp6E6=Cyc_CfFlowInfo_assign_place(_tmp640,_tmp678->loc,_tmp6E6,env->all_changed,_tmp6E9,_tmp6EA);
# 2833
_tmp6E2=Cyc_CfFlowInfo_ReachableFL(_tmp6E6,_tmp6E7);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA0D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA0C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6ED=(_tmpA0C=_region_malloc(_tmp640->r,sizeof(*_tmpA0C)),((_tmpA0C[0]=((_tmpA0D.tag=0,((_tmpA0D.f1=_tmp677,_tmpA0D)))),_tmpA0C)));
struct Cyc_CfFlowInfo_Place*_tmpA0E;struct Cyc_CfFlowInfo_Place*_tmp6EE=(_tmpA0E=_region_malloc(_tmp640->r,sizeof(*_tmpA0E)),((_tmpA0E->root=(void*)_tmp6ED,((_tmpA0E->fields=0,_tmpA0E)))));
{struct Cyc_List_List*_tmpA0F;_tmp6E3=((_tmpA0F=_region_malloc(_tmp640->r,sizeof(*_tmpA0F)),((_tmpA0F->hd=_tmp6EE,((_tmpA0F->tl=_tmp6E3,_tmpA0F))))));}
{struct Cyc_List_List*_tmpA10;_tmp6E4=((_tmpA10=_region_malloc(_tmp640->r,sizeof(*_tmpA10)),((_tmpA10->hd=_tmp678,((_tmpA10->tl=_tmp6E4,_tmpA10))))));}
goto _LL398;};}_LL39B:;_LL39C:
# 2845
 goto _LL398;_LL398:;}
# 2847
goto _LL393;_LL396:;_LL397:
# 2849
 goto _LL393;_LL393:;}{
# 2852
struct Cyc_List_List _tmpA11;struct Cyc_List_List _tmp6F4=(_tmpA11.hd=_tmp677,((_tmpA11.tl=0,_tmpA11)));
_tmp6E2=Cyc_NewControlFlow_add_vars(_tmp640,_tmp6E2,& _tmp6F4,_tmp640->unknown_all,_tmp679,0);{
# 2855
struct Cyc_List_List*_tmp6F5=env->unique_pat_vars;
{struct _tuple29*_tmpA14;struct Cyc_List_List*_tmpA13;env->unique_pat_vars=(
(_tmpA13=_region_malloc(env->r,sizeof(*_tmpA13)),((_tmpA13->hd=((_tmpA14=_region_malloc(env->r,sizeof(*_tmpA14)),((_tmpA14->f1=1,((_tmpA14->f2=s,((_tmpA14->f3=_tmp6E3,((_tmpA14->f4=_tmp6E4,_tmpA14)))))))))),((_tmpA13->tl=_tmp6F5,_tmpA13))))));}
# 2861
_tmp6E2=Cyc_NewControlFlow_anal_stmt(env,_tmp6E2,_tmp67A,rval_opt);
env->unique_pat_vars=_tmp6F5;
# 2865
_tmp6E2=Cyc_NewControlFlow_unconsume_params(env,_tmp6E3,_tmp6E4,1,_tmp6E2,_tmp679);
# 2869
return _tmp6E2;};};};};}_LL373: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp67B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp67B->tag != 12)goto _LL375;else{_tmp67C=_tmp67B->f1;_tmp67D=_tmp67B->f2;}}_LL374:
# 2872
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp67C),_tmp67D,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp67C);
return outflow;_LL375: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp67E=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp67E->tag != 13)goto _LL377;else{_tmp67F=_tmp67E->f2;}}_LL376:
# 2877
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp67F,rval_opt);_LL377: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp680=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp642;if(_tmp680->tag != 16)goto _LL379;else{_tmp681=_tmp680->f1;}}_LL378: {
# 2880
struct _tuple16 _tmp6FA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp681);union Cyc_CfFlowInfo_FlowInfo _tmp6FC;void*_tmp6FD;struct _tuple16 _tmp6FB=_tmp6FA;_tmp6FC=_tmp6FB.f1;_tmp6FD=_tmp6FB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6FE=Cyc_NewControlFlow_use_Rval(env,_tmp681->loc,_tmp6FC,_tmp6FD);
void*_tmp6FF=Cyc_Tcutil_compress((void*)_check_null(_tmp681->topt));void*_tmp700=_tmp6FF;void*_tmp702;_LL39E: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp701=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp700;if(_tmp701->tag != 15)goto _LL3A0;else{_tmp702=(void*)_tmp701->f1;}}_LL39F:
# 2884
 return Cyc_CfFlowInfo_kill_flow_region(_tmp640,_tmp6FC,_tmp702);_LL3A0:;_LL3A1: {
struct Cyc_Core_Impossible_exn_struct _tmpA1A;const char*_tmpA19;struct Cyc_Core_Impossible_exn_struct*_tmpA18;(int)_throw((void*)((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18[0]=((_tmpA1A.tag=Cyc_Core_Impossible,((_tmpA1A.f1=((_tmpA19="anal_stmt -- reset_region",_tag_dyneither(_tmpA19,sizeof(char),26))),_tmpA1A)))),_tmpA18)))));}_LL39D:;};}_LL379:;_LL37A: {
# 2888
struct Cyc_Core_Impossible_exn_struct _tmpA20;const char*_tmpA1F;struct Cyc_Core_Impossible_exn_struct*_tmpA1E;(int)_throw((void*)((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=((_tmpA20.tag=Cyc_Core_Impossible,((_tmpA20.f1=((_tmpA1F="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmpA1F,sizeof(char),56))),_tmpA20)))),_tmpA1E)))));}_LL34E:;};}
# 2893
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2896
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp709=env->fenv;
void*_tmp70A=decl->r;void*_tmp70B=_tmp70A;struct Cyc_Absyn_Vardecl*_tmp70D;struct Cyc_List_List*_tmp70F;struct Cyc_Absyn_Fndecl*_tmp711;struct Cyc_Absyn_Tvar*_tmp713;struct Cyc_Absyn_Vardecl*_tmp714;int _tmp715;struct Cyc_Absyn_Exp*_tmp716;_LL3A3: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp70C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp70B;if(_tmp70C->tag != 0)goto _LL3A5;else{_tmp70D=_tmp70C->f1;}}_LL3A4: {
# 2906
struct Cyc_List_List _tmpA21;struct Cyc_List_List _tmp717=(_tmpA21.hd=_tmp70D,((_tmpA21.tl=0,_tmpA21)));
inflow=Cyc_NewControlFlow_add_vars(_tmp709,inflow,& _tmp717,_tmp709->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp718=_tmp70D->initializer;
if(_tmp718 == 0)
return inflow;{
struct _tuple16 _tmp719=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp718);union Cyc_CfFlowInfo_FlowInfo _tmp71B;void*_tmp71C;struct _tuple16 _tmp71A=_tmp719;_tmp71B=_tmp71A.f1;_tmp71C=_tmp71A.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp709,env,_tmp71B,_tmp70D,_tmp718,_tmp71C,decl->loc);};};}_LL3A5: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp70E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp70B;if(_tmp70E->tag != 3)goto _LL3A7;else{_tmp70F=_tmp70E->f1;}}_LL3A6:
# 2915
 return Cyc_NewControlFlow_add_vars(_tmp709,inflow,_tmp70F,_tmp709->unknown_none,decl->loc,0);_LL3A7: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp710=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp70B;if(_tmp710->tag != 1)goto _LL3A9;else{_tmp711=_tmp710->f1;}}_LL3A8:
# 2918
 Cyc_NewControlFlow_check_nested_fun(_tmp709,inflow,_tmp711);{
void*t=(void*)_check_null(_tmp711->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp71E=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp711->fn_vardecl);
# 2924
struct Cyc_List_List*_tmpA22;return Cyc_NewControlFlow_add_vars(_tmp709,inflow,((_tmpA22=_region_malloc(env->r,sizeof(*_tmpA22)),((_tmpA22->hd=_tmp71E,((_tmpA22->tl=0,_tmpA22)))))),_tmp709->unknown_all,decl->loc,0);};_LL3A9: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp712=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp70B;if(_tmp712->tag != 4)goto _LL3AB;else{_tmp713=_tmp712->f1;_tmp714=_tmp712->f2;_tmp715=_tmp712->f3;_tmp716=_tmp712->f4;}}_LL3AA:
# 2927
 if(_tmp716 != 0){
struct Cyc_Core_Impossible_exn_struct _tmpA28;const char*_tmpA27;struct Cyc_Core_Impossible_exn_struct*_tmpA26;(int)_throw((void*)((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26[0]=((_tmpA28.tag=Cyc_Core_Impossible,((_tmpA28.f1=((_tmpA27="found open expression in declaration!",_tag_dyneither(_tmpA27,sizeof(char),38))),_tmpA28)))),_tmpA26)))));}{
struct Cyc_List_List _tmpA29;struct Cyc_List_List _tmp723=(_tmpA29.hd=_tmp714,((_tmpA29.tl=0,_tmpA29)));
return Cyc_NewControlFlow_add_vars(_tmp709,inflow,& _tmp723,_tmp709->unknown_all,decl->loc,0);};_LL3AB:;_LL3AC: {
# 2933
struct Cyc_Core_Impossible_exn_struct _tmpA2F;const char*_tmpA2E;struct Cyc_Core_Impossible_exn_struct*_tmpA2D;(int)_throw((void*)((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=((_tmpA2F.tag=Cyc_Core_Impossible,((_tmpA2F.f1=((_tmpA2E="anal_decl: unexpected decl variant",_tag_dyneither(_tmpA2E,sizeof(char),35))),_tmpA2F)))),_tmpA2D)))));}_LL3A2:;}
# 2941
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp728=_new_region("frgn");struct _RegionHandle*frgn=& _tmp728;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2943
;_pop_region(frgn);}
# 2948
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2953
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp729=r;unsigned int _tmp72A;_LL3AE: if((_tmp729.RParam).tag != 5)goto _LL3B0;_tmp72A=(unsigned int)(_tmp729.RParam).val;_LL3AF: {
# 2956
struct Cyc_Absyn_Vardecl*_tmp72B=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp72A);
if(!_tmp72B->escapes)
return Cyc_Relations_RVar(_tmp72B);
return r;}_LL3B0:;_LL3B1:
 return r;_LL3AD:;}
# 2964
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2968
struct _RegionHandle*_tmp72C=fenv->r;
unsigned int _tmp72D=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp72D,1);{
# 2974
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
union Cyc_CfFlowInfo_FlowInfo _tmpA30;struct _tuple14 _tmp72E=(_tmpA30=inflow,(((_tmpA30.ReachableFL).tag != 2?_throw_match(): 1,(_tmpA30.ReachableFL).val)));struct Cyc_Dict_Dict _tmp730;struct Cyc_List_List*_tmp731;struct _tuple14 _tmp72F=_tmp72E;_tmp730=_tmp72F.f1;_tmp731=_tmp72F.f2;{
# 2979
struct Cyc_List_List*_tmp732=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp733=fd->requires_relns;for(0;_tmp733 != 0;_tmp733=_tmp733->tl){
struct Cyc_Relations_Reln*_tmp734=(struct Cyc_Relations_Reln*)_tmp733->hd;
_tmp731=Cyc_Relations_add_relation(_tmp72C,Cyc_NewControlFlow_translate_rop(_tmp732,_tmp734->rop1),_tmp734->relation,
Cyc_NewControlFlow_translate_rop(_tmp732,_tmp734->rop2),_tmp731);}}{
# 2988
struct Cyc_List_List*atts;
{void*_tmp735=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp736=_tmp735;struct Cyc_List_List*_tmp738;_LL3B3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp737=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp736;if(_tmp737->tag != 9)goto _LL3B5;else{_tmp738=(_tmp737->f1).attributes;}}_LL3B4:
 atts=_tmp738;goto _LL3B2;_LL3B5:;_LL3B6: {
const char*_tmpA33;void*_tmpA32;(_tmpA32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA33="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmpA33,sizeof(char),50))),_tag_dyneither(_tmpA32,sizeof(void*),0)));}_LL3B2:;}
# 2995
for(0;atts != 0;atts=atts->tl){
void*_tmp73B=(void*)atts->hd;void*_tmp73C=_tmp73B;int _tmp73E;int _tmp740;int _tmp742;_LL3B8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp73D=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp73C;if(_tmp73D->tag != 21)goto _LL3BA;else{_tmp73E=_tmp73D->f1;}}_LL3B9: {
# 2998
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp743=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp73E - 1);
void*t=Cyc_Tcutil_compress(_tmp743->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp744=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmpA36;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmpA35;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp745=(_tmpA35=_region_malloc(_tmp72C,sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=2,((_tmpA36.f1=_tmp73E,((_tmpA36.f2=t,_tmpA36)))))),_tmpA35)));
struct Cyc_CfFlowInfo_Place*_tmpA37;struct Cyc_CfFlowInfo_Place*_tmp746=(_tmpA37=_region_malloc(_tmp72C,sizeof(*_tmpA37)),((_tmpA37->root=(void*)_tmp745,((_tmpA37->fields=0,_tmpA37)))));
_tmp730=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp730,(void*)_tmp745,_tmp744);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmpA3D;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA3C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA3A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA39;_tmp730=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp730,(void*)((_tmpA39=_region_malloc(_tmp72C,sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA3A.tag=0,((_tmpA3A.f1=_tmp743,_tmpA3A)))),_tmpA39)))),(void*)((_tmpA3C=_region_malloc(_tmp72C,sizeof(*_tmpA3C)),((_tmpA3C[0]=((_tmpA3D.tag=5,((_tmpA3D.f1=_tmp746,_tmpA3D)))),_tmpA3C)))));}
goto _LL3B7;}_LL3BA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp73F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp73C;if(_tmp73F->tag != 20)goto _LL3BC;else{_tmp740=_tmp73F->f1;}}_LL3BB: {
# 3011
struct Cyc_Absyn_Vardecl*_tmp74E=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp740 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp74E->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmpA40;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmpA3F;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp74F=(_tmpA3F=_region_malloc(_tmp72C,sizeof(*_tmpA3F)),((_tmpA3F[0]=((_tmpA40.tag=2,((_tmpA40.f1=_tmp740,((_tmpA40.f2=elttype,_tmpA40)))))),_tmpA3F)));
struct Cyc_CfFlowInfo_Place*_tmpA41;struct Cyc_CfFlowInfo_Place*_tmp750=(_tmpA41=_region_malloc(_tmp72C,sizeof(*_tmpA41)),((_tmpA41->root=(void*)_tmp74F,((_tmpA41->fields=0,_tmpA41)))));
_tmp730=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp730,(void*)_tmp74F,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmpA47;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA46;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA44;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA43;_tmp730=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp730,(void*)((_tmpA43=_region_malloc(_tmp72C,sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=0,((_tmpA44.f1=_tmp74E,_tmpA44)))),_tmpA43)))),(void*)((_tmpA46=_region_malloc(_tmp72C,sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=5,((_tmpA47.f1=_tmp750,_tmpA47)))),_tmpA46)))));}
{struct Cyc_List_List*_tmpA48;param_roots=((_tmpA48=_region_malloc(_tmp72C,sizeof(*_tmpA48)),((_tmpA48->hd=_tmp750,((_tmpA48->tl=param_roots,_tmpA48))))));}
goto _LL3B7;}_LL3BC: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp741=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp73C;if(_tmp741->tag != 22)goto _LL3BE;else{_tmp742=_tmp741->f1;}}_LL3BD: {
# 3020
struct Cyc_Absyn_Vardecl*_tmp759=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp742 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp759->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA4B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA4A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp75A=(_tmpA4A=_region_malloc(_tmp72C,sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=0,((_tmpA4B.f1=_tmp759,_tmpA4B)))),_tmpA4A)));
struct Cyc_CfFlowInfo_Place*_tmpA4C;struct Cyc_CfFlowInfo_Place*_tmp75B=(_tmpA4C=_region_malloc(_tmp72C,sizeof(*_tmpA4C)),((_tmpA4C->root=(void*)_tmp75A,((_tmpA4C->fields=0,_tmpA4C)))));
struct Cyc_List_List*_tmpA4D;noconsume_roots=((_tmpA4D=_region_malloc(_tmp72C,sizeof(*_tmpA4D)),((_tmpA4D->hd=_tmp75B,((_tmpA4D->tl=noconsume_roots,_tmpA4D))))));}
# 3026
goto _LL3B7;}_LL3BE:;_LL3BF:
 goto _LL3B7;_LL3B7:;}
# 3029
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp730,_tmp731);{
# 3031
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp72C,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmpA4E;struct Cyc_NewControlFlow_AnalEnv*env=(_tmpA4E=_region_malloc(_tmp72C,sizeof(*_tmpA4E)),((_tmpA4E->r=_tmp72C,((_tmpA4E->fenv=fenv,((_tmpA4E->iterate_again=1,((_tmpA4E->iteration_num=0,((_tmpA4E->in_try=0,((_tmpA4E->tryflow=inflow,((_tmpA4E->all_changed=0,((_tmpA4E->noreturn=noreturn,((_tmpA4E->return_type=fd->ret_type,((_tmpA4E->unique_pat_vars=0,((_tmpA4E->param_roots=param_roots,((_tmpA4E->noconsume_params=noconsume_roots,((_tmpA4E->flow_table=flow_table,((_tmpA4E->return_relns=fd->ensures_relns,_tmpA4E)))))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3041
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3044
union Cyc_CfFlowInfo_FlowInfo _tmp760=outflow;_LL3C1: if((_tmp760.BottomFL).tag != 1)goto _LL3C3;_LL3C2:
 goto _LL3C0;_LL3C3:;_LL3C4:
# 3047
 Cyc_NewControlFlow_check_init_params(_tmp72D,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp72D);
# 3051
if(noreturn){
const char*_tmpA51;void*_tmpA50;(_tmpA50=0,Cyc_CfFlowInfo_aerr(_tmp72D,((_tmpA51="`noreturn' function might (implicitly) return",_tag_dyneither(_tmpA51,sizeof(char),46))),_tag_dyneither(_tmpA50,sizeof(void*),0)));}else{
# 3054
void*_tmp763=Cyc_Tcutil_compress(fd->ret_type);void*_tmp764=_tmp763;_LL3C6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp765=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp764;if(_tmp765->tag != 0)goto _LL3C8;}_LL3C7:
 goto _LL3C5;_LL3C8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp766=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp764;if(_tmp766->tag != 7)goto _LL3CA;}_LL3C9:
 goto _LL3CB;_LL3CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp767=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp764;if(_tmp767->tag != 6)goto _LL3CC;}_LL3CB:
# 3058
{const char*_tmpA54;void*_tmpA53;(_tmpA53=0,Cyc_Tcutil_warn(_tmp72D,((_tmpA54="function may not return a value",_tag_dyneither(_tmpA54,sizeof(char),32))),_tag_dyneither(_tmpA53,sizeof(void*),0)));}goto _LL3C5;_LL3CC:;_LL3CD:
# 3060
{const char*_tmpA57;void*_tmpA56;(_tmpA56=0,Cyc_CfFlowInfo_aerr(_tmp72D,((_tmpA57="function may not return a value",_tag_dyneither(_tmpA57,sizeof(char),32))),_tag_dyneither(_tmpA56,sizeof(void*),0)));}goto _LL3C5;_LL3C5:;}
# 3062
goto _LL3C0;_LL3C0:;};};};};};}
# 3066
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp76E=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp76F=_tmp76E;struct Cyc_Absyn_Fndecl*_tmp771;struct Cyc_List_List*_tmp773;struct Cyc_List_List*_tmp775;struct Cyc_List_List*_tmp777;_LL3CF: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp770=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp76F;if(_tmp770->tag != 1)goto _LL3D1;else{_tmp771=_tmp770->f1;}}_LL3D0:
 Cyc_NewControlFlow_check_fun(_tmp771);goto _LL3CE;_LL3D1: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp772=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp76F;if(_tmp772->tag != 11)goto _LL3D3;else{_tmp773=_tmp772->f1;}}_LL3D2:
 _tmp775=_tmp773;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp774=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp76F;if(_tmp774->tag != 10)goto _LL3D5;else{_tmp775=_tmp774->f2;}}_LL3D4:
 _tmp777=_tmp775;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp776=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp76F;if(_tmp776->tag != 9)goto _LL3D7;else{_tmp777=_tmp776->f2;}}_LL3D6:
 Cyc_NewControlFlow_cf_check(_tmp777);goto _LL3CE;_LL3D7: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp778=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp76F;if(_tmp778->tag != 12)goto _LL3D9;}_LL3D8:
 goto _LL3CE;_LL3D9:;_LL3DA:
 goto _LL3CE;_LL3CE:;};}}
