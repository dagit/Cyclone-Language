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
# 270 "core.h"
void Cyc_Core_rethrow(void*);
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
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 981 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1163
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1165
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1177
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1181
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1183
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
# 43 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 54
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 71
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 75
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 86
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 121 "tcenv.h"
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
# 249 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 281
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 291
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 342
int Cyc_Tcutil_is_noreturn(void*);
# 360
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 364
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 367
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
# 30 "jump_analysis.h"
typedef struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_table_t;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 45 "jump_analysis.h"
typedef struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_anal_res_t;
# 47
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
# 33 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 38
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
# 75
typedef void*Cyc_Tcpat_pat_test_t;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};
# 83
typedef void*Cyc_Tcpat_access_t;struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};
# 93
typedef struct Cyc_Tcpat_PathNode*Cyc_Tcpat_path_node_t;
# 95
typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_path_t;struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};
# 103
typedef struct Cyc_Tcpat_Rhs*Cyc_Tcpat_rhs_t;
# 105
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 120
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct Cyc_PP_Ppstate;
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
# 63 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
# 67
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};
# 74
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};
# 109 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 117
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
# 119
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 126
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 131
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp5;}else{_LL3: _LL4:
 s->annot=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));_tmp2[0]=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp3;_tmp3.tag=Cyc_NewControlFlow_CFAnnot;_tmp3.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;_tmp4.visited=0;_tmp4;});_tmp3;});_tmp2;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 138
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp6=_region_malloc(env->r,sizeof(*_tmp6));_tmp6[0]=Cyc_CfFlowInfo_BottomFL();_tmp6;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 145
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 148
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 156
*_tmp8=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmp8);
# 162
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp9;_tmp9.f1=_tmp7;_tmp9.f2=_tmp8;_tmp9;});}
# 172
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 180
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 187
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 189
struct _tuple21*_tmpA=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp14;unsigned int _tmp13;struct Cyc_Dict_Dict _tmp12;_LL6: _tmp14=_tmpA->f1;_tmp13=_tmpA->f2;_tmp12=_tmpA->f3;_LL7:;{
void*_tmpB=root;struct Cyc_Absyn_Vardecl*_tmp11;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->tag == 0){_LL9: _tmp11=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->f1;_LLA:
# 192
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp14->fenv)->r,_tmp11->type)){
retry: {void*_tmpC=rval;void*_tmp10;switch(*((int*)_tmpC)){case 8: _LLE: _tmp10=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC)->f2;_LLF:
 rval=_tmp10;goto retry;case 7: _LL10: _LL11:
 goto _LL13;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL12: _LL13:
 goto _LL15;}else{goto _LL16;}case 0: _LL14: _LL15:
 goto _LLD;default: _LL16: _LL17:
# 201
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11->name));({void*_tmpD[1]={& _tmpF};Cyc_Tcutil_warn(_tmp13,({const char*_tmpE="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpE,sizeof(char),64);}),_tag_dyneither(_tmpD,sizeof(void*),1));});});
goto _LLD;}_LLD:;}}
# 205
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}
# 213
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp15=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp16=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp17=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp16);
# 219
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp18=({struct _tuple19 _tmp1E;_tmp1E.f1=flow;_tmp1E.f2=_tmp17;_tmp1E;});struct _tuple19 _tmp19=_tmp18;struct Cyc_Dict_Dict _tmp1D;struct Cyc_Dict_Dict _tmp1C;if(((_tmp19.f1).ReachableFL).tag == 2){if(((_tmp19.f2).ReachableFL).tag == 2){_LL19: _tmp1D=(((_tmp19.f1).ReachableFL).val).f1;_tmp1C=(((_tmp19.f2).ReachableFL).val).f1;_LL1A: {
# 222
struct _tuple21 _tmp1A=({struct _tuple21 _tmp1B;_tmp1B.f1=env;_tmp1B.f2=s->loc;_tmp1B.f3=_tmp1C;_tmp1B;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1A,_tmp1D);
goto _LL18;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 228
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp17,*_tmp16)){
# 235
*_tmp16=_tmp17;
# 239
if(_tmp15->visited == env->iteration_num)
# 241
env->iterate_again=1;}}
# 246
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 250
union Cyc_CfFlowInfo_FlowInfo _tmp1F=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp26;if((_tmp1F.BottomFL).tag == 1){_LL1E: _LL1F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL20: _tmp27=((_tmp1F.ReachableFL).val).f1;_tmp26=((_tmp1F.ReachableFL).val).f2;_LL21:
# 253
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp24=_region_malloc(fenv->r,sizeof(*_tmp24));_tmp24[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp25;});_tmp24;});
void*_tmp21=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
_tmp21=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp22=_region_malloc(fenv->r,sizeof(*_tmp22));_tmp22[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp23;_tmp23.tag=8;_tmp23.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp23.f2=_tmp21;_tmp23;});_tmp22;});
# 260
_tmp27=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp20,_tmp21);}
# 262
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp26);}_LL1D:;}
# 266
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 273
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp28=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp29=_region_malloc(r,sizeof(*_tmp29));_tmp29->hd=_tmp28;_tmp29->tl=assume;_tmp29;})))
return 0;}
# 278
return 1;}
# 281
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2A=e->r;void*_tmp2B=_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->tag == 13){_LL23: _tmp2C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->f2;_LL24:
 return _tmp2C;}else{_LL25: _LL26:
 return e;}_LL22:;}
# 288
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 290
union Cyc_CfFlowInfo_FlowInfo _tmp2D=inflow;struct Cyc_List_List*_tmp4B;if((_tmp2D.BottomFL).tag == 1){_LL28: _LL29:
 return;}else{_LL2A: _tmp4B=((_tmp2D.ReachableFL).val).f2;_LL2B:
# 293
{void*_tmp2E=Cyc_Tcutil_compress(t);void*_tmp2F=_tmp2E;struct Cyc_List_List*_tmp4A;union Cyc_Absyn_AggrInfoU _tmp49;struct Cyc_List_List*_tmp48;switch(*((int*)_tmp2F)){case 11: _LL2D: _tmp49=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).aggr_info;_tmp48=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).targs;_LL2E: {
# 295
struct Cyc_Absyn_Aggrdecl*_tmp30=Cyc_Absyn_get_known_aggrdecl(_tmp49);
struct Cyc_Absyn_Aggrfield*_tmp31=Cyc_Absyn_lookup_decl_field(_tmp30,f);
struct Cyc_Absyn_Exp*_tmp32=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp31))->requires_clause;
if(_tmp32 != 0){
struct _RegionHandle _tmp33=_new_region("temp");struct _RegionHandle*temp=& _tmp33;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp34=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp30->tvs,_tmp48),_tmp32);
# 302
if(!Cyc_NewControlFlow_relns_ok(r,_tmp4B,Cyc_Relations_exp2relns(temp,_tmp34))){
({struct Cyc_String_pa_PrintArg_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp34)));({void*_tmp35[2]={& _tmp37,& _tmp38};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp36="unable to prove %s, required to access %s";_tag_dyneither(_tmp36,sizeof(char),42);}),_tag_dyneither(_tmp35,sizeof(void*),2));});});});
({void*_tmp39=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3A="  [recorded facts on path: ";_tag_dyneither(_tmp3A,sizeof(char),28);}),_tag_dyneither(_tmp39,sizeof(void*),0));});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp3B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3C="]\n";_tag_dyneither(_tmp3C,sizeof(char),3);}),_tag_dyneither(_tmp3B,sizeof(void*),0));});}}
# 300
;_pop_region(temp);}
# 309
goto _LL2C;}case 12: _LL2F: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F)->f2;_LL30: {
# 311
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_field(_tmp4A,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp4B,Cyc_Relations_exp2relns(temp,_tmp3E))){
({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp42;_tmp42.tag=0;_tmp42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3E)));({void*_tmp40[2]={& _tmp42,& _tmp43};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp41="unable to prove %s, required to access %s";_tag_dyneither(_tmp41,sizeof(char),42);}),_tag_dyneither(_tmp40,sizeof(void*),2));});});});
({void*_tmp44=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp45="  [recorded facts on path: ";_tag_dyneither(_tmp45,sizeof(char),28);}),_tag_dyneither(_tmp44,sizeof(void*),0));});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp46=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp47="]\n";_tag_dyneither(_tmp47,sizeof(char),3);}),_tag_dyneither(_tmp46,sizeof(void*),0));});}
# 315
;_pop_region(temp);}
# 322
goto _LL2C;}default: _LL31: _LL32:
 goto _LL2C;}_LL2C:;}
# 325
goto _LL27;}_LL27:;}
# 329
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4C=inflow;struct Cyc_Dict_Dict _tmp52;struct Cyc_List_List*_tmp51;if((_tmp4C.BottomFL).tag == 1){_LL34: _LL35:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL36: _tmp52=((_tmp4C.ReachableFL).val).f1;_tmp51=((_tmp4C.ReachableFL).val).f2;_LL37:
# 333
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp52,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4D=0;Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4E="expression may not be fully initialized";_tag_dyneither(_tmp4E,sizeof(char),40);}),_tag_dyneither(_tmp4D,sizeof(void*),0));});{
struct Cyc_Dict_Dict _tmp4F=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp52,env->all_changed,r);
if(_tmp52.t == _tmp4F.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp50=Cyc_CfFlowInfo_ReachableFL(_tmp4F,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp50);
return _tmp50;};};}_LL33:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 343
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp53=({struct _tuple0 _tmp66;_tmp66.f1=Cyc_Tcutil_compress(t);_tmp66.f2=r;_tmp66;});struct _tuple0 _tmp54=_tmp53;enum Cyc_Absyn_AggrKind _tmp65;struct Cyc_List_List*_tmp64;struct _dyneither_ptr _tmp63;union Cyc_Absyn_AggrInfoU _tmp62;struct Cyc_List_List*_tmp61;struct _dyneither_ptr _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;struct Cyc_Absyn_Datatypefield*_tmp5D;struct _dyneither_ptr _tmp5C;void*_tmp5B;switch(*((int*)_tmp54.f2)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL39: _LL3A:
 return;}else{switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL4B;else{goto _LL4B;}case 10: goto _LL4B;case 11: goto _LL4B;case 12: goto _LL4B;case 5: goto _LL49;default: goto _LL4B;}}case 0: _LL3B: _LL3C:
 return;case 7: _LL3D: _LL3E:
 return;case 8: _LL3F: _tmp5B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL40:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp5B);return;default: switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL41: _tmp5D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp5C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL42:
# 350
 if(_tmp5D->typs == 0)
return;
_tmp5F=_tmp5D->typs;_tmp5E=_tmp5C;goto _LL44;}else{goto _LL4B;}}else{goto _LL4B;}case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL43: _tmp5F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL44: {
# 354
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5F);
{int i=0;for(0;i < sz;(i ++,_tmp5F=_tmp5F->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp5E,sizeof(void*),i)));}}
# 358
return;}}else{goto _LL4B;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL45: _tmp62=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).aggr_info;_tmp61=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).targs;_tmp60=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL46: {
# 361
struct Cyc_Absyn_Aggrdecl*_tmp55=Cyc_Absyn_get_known_aggrdecl(_tmp62);
if(_tmp55->impl == 0)return;{
struct Cyc_List_List*_tmp56=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55->impl))->fields;
struct _RegionHandle _tmp57=_new_region("temp");struct _RegionHandle*temp=& _tmp57;_push_region(temp);
{struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp55->tvs,_tmp61);
{int i=0;for(0;i < _get_dyneither_size(_tmp60,sizeof(void*));(i ++,_tmp56=_tmp56->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp56))->hd)->type;
if(_tmp58 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp58,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp60.curr)[i]);}}
# 371
_npop_handler(0);return;}
# 365
;_pop_region(temp);};}}else{goto _LL4B;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL47: _tmp65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp64=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f2;_tmp63=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL48:
# 374
{int i=0;for(0;i < _get_dyneither_size(_tmp63,sizeof(void*));(i ++,_tmp64=_tmp64->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp64))->hd)->type,((void**)_tmp63.curr)[i]);}}
return;}else{goto _LL4B;}case 5: _LL49: if(
Cyc_Tcutil_is_noalias_pointer(t,0)){_LL4A:
({void*_tmp59=0;Cyc_Tcutil_warn(loc,({const char*_tmp5A="argument may still contain alias-free pointers";_tag_dyneither(_tmp5A,sizeof(char),47);}),_tag_dyneither(_tmp59,sizeof(void*),0));});
return;}else{goto _LL4B;}default: _LL4B: _LL4C:
 return;}}_LL38:;}
# 384
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp67=inflow;struct Cyc_Dict_Dict _tmp73;struct Cyc_List_List*_tmp72;if((_tmp67.BottomFL).tag == 1){_LL4E: _LL4F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL50: _tmp73=((_tmp67.ReachableFL).val).f1;_tmp72=((_tmp67.ReachableFL).val).f2;_LL51:
# 388
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp68=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp69="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp69,sizeof(char),51);}),_tag_dyneither(_tmp68,sizeof(void*),0));});
return Cyc_CfFlowInfo_BottomFL();}{
# 392
void*_tmp6A=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp6B=r;struct Cyc_CfFlowInfo_Place*_tmp6F;void*_tmp6E;switch(*((int*)_tmp6B)){case 8: _LL53: _tmp6E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f2;_LL54:
 r=_tmp6E;goto retry;case 5: _LL55: _tmp6F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL56:
# 396
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6A,Cyc_CfFlowInfo_lookup_place(_tmp73,_tmp6F));
goto _LL52;default: _LL57: _LL58:
# 399
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp6A))
({void*_tmp6C=0;Cyc_Tcutil_warn(loc,({const char*_tmp6D="argument may contain live alias-free pointers";_tag_dyneither(_tmp6D,sizeof(char),46);}),_tag_dyneither(_tmp6C,sizeof(void*),0));});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL52:;}{
# 403
struct Cyc_Dict_Dict _tmp70=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp73,env->all_changed,r);
if(_tmp73.t == _tmp70.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp71=Cyc_CfFlowInfo_ReachableFL(_tmp70,_tmp72);
Cyc_NewControlFlow_update_tryflow(env,_tmp71);
return _tmp71;};};};}_LL4D:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 412
static struct _tuple23 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 419
if(es == 0)
return({struct _tuple23 _tmp74;_tmp74.f1=inflow;_tmp74.f2=0;_tmp74;});
if(es->tl == 0){
struct _tuple16 _tmp75=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp76=_tmp75;union Cyc_CfFlowInfo_FlowInfo _tmp7A;void*_tmp79;_LL5A: _tmp7A=_tmp76.f1;_tmp79=_tmp76.f2;_LL5B:;
return({struct _tuple23 _tmp77;_tmp77.f1=_tmp7A;_tmp77.f2=({struct Cyc_List_List*_tmp78=_region_malloc(rgn,sizeof(*_tmp78));_tmp78->hd=_tmp79;_tmp78->tl=0;_tmp78;});_tmp77;});}{
# 425
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
# 432
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
do{
this_all_changed=(env->fenv)->mt_place_set;
# 436
env->all_changed=({struct Cyc_Dict_Dict*_tmp7B=_region_malloc(env->r,sizeof(*_tmp7B));_tmp7B[0]=(env->fenv)->mt_place_set;_tmp7B;});{
struct _tuple16 _tmp7C=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp7D=_tmp7C;union Cyc_CfFlowInfo_FlowInfo _tmp86;void*_tmp85;_LL5D: _tmp86=_tmp7D.f1;_tmp85=_tmp7D.f2;_LL5E:;
outflow=_tmp86;
rvals=({struct Cyc_List_List*_tmp7E=_region_malloc(rgn,sizeof(*_tmp7E));_tmp7E->hd=_tmp85;_tmp7E->tl=0;_tmp7E;});
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 442
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
env->all_changed=({struct Cyc_Dict_Dict*_tmp7F=_region_malloc(env->r,sizeof(*_tmp7F));_tmp7F[0]=(env->fenv)->mt_place_set;_tmp7F;});{
struct _tuple16 _tmp80=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);struct _tuple16 _tmp81=_tmp80;union Cyc_CfFlowInfo_FlowInfo _tmp84;void*_tmp83;_LL60: _tmp84=_tmp81.f1;_tmp83=_tmp81.f2;_LL61:;
rvals=({struct Cyc_List_List*_tmp82=_region_malloc(rgn,sizeof(*_tmp82));_tmp82->hd=_tmp83;_tmp82->tl=rvals;_tmp82;});
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp84,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 448
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 451
old_inflow=inflow;
# 454
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 459
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 463
env->all_changed=({struct Cyc_Dict_Dict*_tmp87=_region_malloc(env->r,sizeof(*_tmp87));_tmp87[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0);_tmp87;});}
# 465
Cyc_NewControlFlow_update_tryflow(env,outflow);
# 467
return({struct _tuple23 _tmp88;_tmp88.f1=outflow;_tmp88.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp88;});};}
# 472
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 475
struct _RegionHandle _tmp89=_new_region("rgn");struct _RegionHandle*rgn=& _tmp89;_push_region(rgn);
{struct _tuple23 _tmp8A=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 476
struct _tuple23 _tmp8B=_tmp8A;union Cyc_CfFlowInfo_FlowInfo _tmp93;struct Cyc_List_List*_tmp92;_LL63: _tmp93=_tmp8B.f1;_tmp92=_tmp8B.f2;_LL64:;
# 478
{union Cyc_CfFlowInfo_FlowInfo _tmp8C=_tmp93;struct Cyc_Dict_Dict _tmp8F;if((_tmp8C.ReachableFL).tag == 2){_LL66: _tmp8F=((_tmp8C.ReachableFL).val).f1;_LL67:
# 480
 for(0;_tmp92 != 0;(_tmp92=_tmp92->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp8F,(void*)_tmp92->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp8D=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp8E="expression may not be initialized";_tag_dyneither(_tmp8E,sizeof(char),34);}),_tag_dyneither(_tmp8D,sizeof(void*),0));});}
goto _LL65;}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}{
# 486
struct _tuple16 _tmp91=({struct _tuple16 _tmp90;_tmp90.f1=_tmp93;_tmp90.f2=(env->fenv)->unknown_all;_tmp90;});_npop_handler(0);return _tmp91;};}
# 476
;_pop_region(rgn);}
# 494
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 501
int needs_unconsume=0;
void*_tmp94=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp94,& needs_unconsume))
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Core_Impossible_exn_struct _tmp96;_tmp96.tag=Cyc_Core_Impossible;_tmp96.f1=({const char*_tmp97="consume_zero_ral";_tag_dyneither(_tmp97,sizeof(char),17);});_tmp96;});_tmp95;}));else{
# 506
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 510
return new_rval;}}}
# 523 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 529
union Cyc_CfFlowInfo_FlowInfo _tmp98=outflow;struct Cyc_Dict_Dict _tmpA0;struct Cyc_List_List*_tmp9F;if((_tmp98.BottomFL).tag == 1){_LL6B: _LL6C:
 return outflow;}else{_LL6D: _tmpA0=((_tmp98.ReachableFL).val).f1;_tmp9F=((_tmp98.ReachableFL).val).f2;_LL6E: {
# 532
union Cyc_CfFlowInfo_AbsLVal _tmp99=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp9A=_tmp99;struct Cyc_CfFlowInfo_Place*_tmp9E;if((_tmp9A.UnknownL).tag == 2){_LL70: _LL71:
# 536
 return outflow;}else{_LL72: _tmp9E=(_tmp9A.PlaceL).val;_LL73: {
# 540
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9B=_region_malloc((env->fenv)->r,sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9C;_tmp9C.tag=8;_tmp9C.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp9C.f2=nzval;_tmp9C;});_tmp9B;});}
# 544
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA0,_tmp9E,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmp9D=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA0,env->all_changed,_tmp9E,nzval),_tmp9F);
# 549
return _tmp9D;};}}_LL6F:;}}_LL6A:;}
# 558
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 561
union Cyc_CfFlowInfo_FlowInfo _tmpA1=outflow;struct Cyc_Dict_Dict _tmpAD;struct Cyc_List_List*_tmpAC;if((_tmpA1.BottomFL).tag == 1){_LL75: _LL76:
 return({struct _tuple19 _tmpA2;_tmpA2.f1=outflow;_tmpA2.f2=outflow;_tmpA2;});}else{_LL77: _tmpAD=((_tmpA1.ReachableFL).val).f1;_tmpAC=((_tmpA1.ReachableFL).val).f2;_LL78: {
# 564
union Cyc_CfFlowInfo_AbsLVal _tmpA3=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpA4=_tmpA3;struct Cyc_CfFlowInfo_Place*_tmpAB;if((_tmpA4.UnknownL).tag == 2){_LL7A: _LL7B:
 return({struct _tuple19 _tmpA5;_tmpA5.f1=outflow;_tmpA5.f2=outflow;_tmpA5;});}else{_LL7C: _tmpAB=(_tmpA4.PlaceL).val;_LL7D: {
# 567
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpA6=_region_malloc((env->fenv)->r,sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpA7;_tmpA7.tag=8;_tmpA7.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmpA7.f2=nzval;_tmpA7;});_tmpA6;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpA8=_region_malloc((env->fenv)->r,sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpA9;_tmpA9.tag=8;_tmpA9.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmpA9.f2=zval;_tmpA9;});_tmpA8;});}
# 573
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpAD,_tmpAB,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpAD,_tmpAB,e,zval);
return({struct _tuple19 _tmpAA;_tmpAA.f1=
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAD,env->all_changed,_tmpAB,nzval),_tmpAC);_tmpAA.f2=
# 578
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpAD,env->all_changed,_tmpAB,zval),_tmpAC);_tmpAA;});}}_LL79:;}}_LL74:;}
# 584
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 592
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 596
void*_tmpB0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpB1=_tmpB0;void*_tmpEF;union Cyc_Absyn_Constraint*_tmpEE;union Cyc_Absyn_Constraint*_tmpED;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->tag == 5){_LL7F: _tmpEF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->f1).elt_typ;_tmpEE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->f1).ptr_atts).bounds;_tmpED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->f1).ptr_atts).zero_term;_LL80: {
# 598
union Cyc_CfFlowInfo_FlowInfo _tmpB2=f;struct Cyc_Dict_Dict _tmpE9;struct Cyc_List_List*_tmpE8;if((_tmpB2.BottomFL).tag == 1){_LL84: _LL85:
# 600
 return({struct _tuple16 _tmpB3;_tmpB3.f1=f;_tmpB3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpEF,0,(env->fenv)->unknown_all);_tmpB3;});}else{_LL86: _tmpE9=((_tmpB2.ReachableFL).val).f1;_tmpE8=((_tmpB2.ReachableFL).val).f2;_LL87:
# 603
 if(Cyc_Tcutil_is_bound_one(_tmpEE)){
struct _tuple15 _tmpB4=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpB5=_tmpB4;void*_tmpD6;struct Cyc_List_List*_tmpD5;_LL89: _tmpD6=_tmpB5.f1;_tmpD5=_tmpB5.f2;_LL8A:;{
void*_tmpB6=_tmpD6;enum Cyc_CfFlowInfo_InitLevel _tmpD4;struct Cyc_CfFlowInfo_Place*_tmpD3;struct Cyc_Absyn_Vardecl*_tmpD2;void*_tmpD1;switch(*((int*)_tmpB6)){case 8: _LL8C: _tmpD2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_tmpD1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f2;_LL8D:
# 607
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Core_Impossible_exn_struct _tmpB8;_tmpB8.tag=Cyc_Core_Impossible;_tmpB8.f1=({const char*_tmpB9="named location in anal_derefR";_tag_dyneither(_tmpB9,sizeof(char),30);});_tmpB8;});_tmpB7;}));case 1: _LL8E: _LL8F:
 goto _LL91;case 2: _LL90: _LL91:
# 610
{void*_tmpBA=e->annot;void*_tmpBB=_tmpBA;struct Cyc_List_List*_tmpC0;if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpBB)->tag == Cyc_CfFlowInfo_NotZero){_LL9B: _tmpC0=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpBB)->f1;_LL9C:
# 612
 if(!Cyc_Relations_same_relns(_tmpE8,_tmpC0))goto _LL9E;
goto _LL9A;}else{_LL9D: _LL9E:
# 616
{void*_tmpBC=e->r;void*_tmpBD=_tmpBC;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD)->tag == 22){_LLA0: _LLA1:
# 618
 e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpBF;_tmpBF.tag=Cyc_CfFlowInfo_NotZero;_tmpBF.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpE8);_tmpBF;});_tmpBE;});
goto _LL9F;}else{_LLA2: _LLA3:
# 621
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL9F;}_LL9F:;}
# 624
goto _LL9A;}_LL9A:;}
# 626
goto _LL8B;case 5: _LL92: _tmpD3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LL93: {
# 630
int possibly_null=0;
{void*_tmpC1=e->annot;void*_tmpC2=_tmpC1;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpC9;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC2)->tag == Cyc_CfFlowInfo_UnknownZ){_LLA5: _tmpC9=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC2)->f1;_LLA6:
# 633
 possibly_null=1;
_tmpCA=_tmpC9;goto _LLA8;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpC2)->tag == Cyc_CfFlowInfo_NotZero){_LLA7: _tmpCA=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpC2)->f1;_LLA8:
# 636
 if(!Cyc_Relations_same_relns(_tmpE8,_tmpCA))goto _LLAA;
goto _LLA4;}else{_LLA9: _LLAA:
# 640
{void*_tmpC3=e->r;void*_tmpC4=_tmpC3;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC4)->tag == 22){_LLAC: _LLAD:
# 642
 if(possibly_null)
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpC6;_tmpC6.tag=Cyc_CfFlowInfo_UnknownZ;_tmpC6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpE8);_tmpC6;});_tmpC5;});else{
# 645
e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpC8;_tmpC8.tag=Cyc_CfFlowInfo_NotZero;_tmpC8.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpE8);_tmpC8;});_tmpC7;});}
goto _LLAB;}else{_LLAE: _LLAF:
# 648
 if(possibly_null)
e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;else{
# 651
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;}
goto _LLAB;}_LLAB:;}
# 654
goto _LLA4;}}_LLA4:;}
# 656
return({struct _tuple16 _tmpCB;_tmpCB.f1=f;_tmpCB.f2=Cyc_CfFlowInfo_lookup_place(_tmpE9,_tmpD3);_tmpCB;});}case 0: _LL94: _LL95:
# 658
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpCC;_tmpCC.f1=Cyc_CfFlowInfo_BottomFL();_tmpCC.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpEF,0,(env->fenv)->unknown_all);_tmpCC;});case 3: _LL96: _tmpD4=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LL97:
# 661
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpD4,_tmpD5);goto _LL99;default: _LL98: _LL99:
# 663
{void*_tmpCD=e->r;void*_tmpCE=_tmpCD;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCE)->tag == 22){_LLB1: _LLB2:
# 665
 e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpD0;_tmpD0.tag=Cyc_CfFlowInfo_UnknownZ;_tmpD0.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpE8);_tmpD0;});_tmpCF;});
goto _LLB0;}else{_LLB3: _LLB4:
# 668
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LLB0;}_LLB0:;}
# 671
goto _LL8B;}_LL8B:;};}else{
# 674
void*_tmpD7=e->annot;void*_tmpD8=_tmpD7;struct Cyc_List_List*_tmpDB;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpD8)->tag == Cyc_CfFlowInfo_UnknownZ){_LLB6: _tmpDB=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpD8)->f1;_LLB7:
# 676
 if(!Cyc_Relations_same_relns(_tmpE8,_tmpDB))goto _LLB9;
goto _LLB5;}else{_LLB8: _LLB9:
# 679
 e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpDA;_tmpDA.tag=Cyc_CfFlowInfo_UnknownZ;_tmpDA.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpE8);_tmpDA;});_tmpD9;});
goto _LLB5;}_LLB5:;}{
# 683
enum Cyc_CfFlowInfo_InitLevel _tmpDC=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpE9,r);enum Cyc_CfFlowInfo_InitLevel _tmpDD=_tmpDC;switch(_tmpDD){case Cyc_CfFlowInfo_NoneIL: _LLBB: _LLBC: {
# 685
struct _tuple15 _tmpDE=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpDF=_tmpDE;void*_tmpE5;_LLC2: _tmpE5=_tmpDF.f1;_LLC3:;
{void*_tmpE0=_tmpE5;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE0)->tag == 7){_LLC5: _LLC6:
# 688
({void*_tmpE1=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmpE2="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpE2,sizeof(char),53);}),_tag_dyneither(_tmpE1,sizeof(void*),0));});
goto _LLC4;}else{_LLC7: _LLC8:
# 691
({void*_tmpE3=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmpE4="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpE4,sizeof(char),46);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});}_LLC4:;}
# 693
goto _LLBE;}case Cyc_CfFlowInfo_AllIL: _LLBD: _LLBE:
# 695
 return({struct _tuple16 _tmpE6;_tmpE6.f1=f;_tmpE6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpEF,0,(env->fenv)->unknown_all);_tmpE6;});default: _LLBF: _LLC0:
# 697
 return({struct _tuple16 _tmpE7;_tmpE7.f1=f;_tmpE7.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpEF,0,(env->fenv)->unknown_none);_tmpE7;});}_LLBA:;};}_LL83:;}}else{_LL81: _LL82:
# 700
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Core_Impossible_exn_struct _tmpEB;_tmpEB.tag=Cyc_Core_Impossible;_tmpEB.f1=({const char*_tmpEC="right deref of non-pointer-type";_tag_dyneither(_tmpEC,sizeof(char),32);});_tmpEB;});_tmpEA;}));}_LL7E:;}
# 707
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 710
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 713
{void*_tmpF0=e1->r;void*_tmpF1=_tmpF0;void*_tmpF3;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF1)->tag == 1){_LLCA: _tmpF3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LLCB: {
# 715
struct Cyc_Absyn_Vardecl*_tmpF2=Cyc_Tcutil_nonesc_vardecl(_tmpF3);
if(_tmpF2 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpF2);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 721
goto _LLC9;}}else{_LLCC: _LLCD:
 goto _LLC9;}_LLC9:;}{
# 725
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 731
return relns;};}
# 739
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 744
struct _tuple18 _tmpF4=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpF5=_tmpF4;union Cyc_CfFlowInfo_AbsLVal _tmp105;_LLCF: _tmp105=_tmpF5.f2;_LLD0:;
{struct _tuple18 _tmpF6=({struct _tuple18 _tmp104;_tmp104.f1=inflow;_tmp104.f2=_tmp105;_tmp104;});struct _tuple18 _tmpF7=_tmpF6;struct Cyc_Dict_Dict _tmp103;struct Cyc_List_List*_tmp102;struct Cyc_CfFlowInfo_Place*_tmp101;if(((_tmpF7.f1).ReachableFL).tag == 2){if(((_tmpF7.f2).PlaceL).tag == 1){_LLD2: _tmp103=(((_tmpF7.f1).ReachableFL).val).f1;_tmp102=(((_tmpF7.f1).ReachableFL).val).f2;_tmp101=((_tmpF7.f2).PlaceL).val;_LLD3: {
# 747
void*_tmpF8=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 749
struct _tuple15 _tmpF9=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);struct _tuple15 _tmpFA=_tmpF9;void*_tmpFE;struct Cyc_List_List*_tmpFD;_LLD9: _tmpFE=_tmpFA.f1;_tmpFD=_tmpFA.f2;_LLDA:;
for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){
_tmpF8=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFB=_region_malloc((env->fenv)->r,sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpFC;_tmpFC.tag=8;_tmpFC.f1=(struct Cyc_Absyn_Vardecl*)_tmpFD->hd;_tmpFC.f2=_tmpF8;_tmpFC;});_tmpFB;});}
# 754
_tmp103=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp103,env->all_changed,_tmp101,_tmpF8);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp103,_tmp102);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 760
goto _LLD1;}}else{_LLD6: _LLD7:
# 763
({void*_tmpFF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp100="noconsume() parameters must be l-values";_tag_dyneither(_tmp100,sizeof(char),40);}),_tag_dyneither(_tmpFF,sizeof(void*),0));});
goto _LLD1;}}else{_LLD4: _LLD5:
# 761
 goto _LLD1;}_LLD1:;}
# 766
return inflow;}
# 771
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 779
union Cyc_CfFlowInfo_FlowInfo _tmp106=outflow;struct Cyc_Dict_Dict _tmp114;struct Cyc_List_List*_tmp113;if((_tmp106.BottomFL).tag == 1){_LLDC: _LLDD:
# 781
 return({struct _tuple16 _tmp107;_tmp107.f1=Cyc_CfFlowInfo_BottomFL();_tmp107.f2=rval;_tmp107;});}else{_LLDE: _tmp114=((_tmp106.ReachableFL).val).f1;_tmp113=((_tmp106.ReachableFL).val).f2;_LLDF: {
# 783
union Cyc_CfFlowInfo_AbsLVal _tmp108=lval;struct Cyc_CfFlowInfo_Place*_tmp112;if((_tmp108.PlaceL).tag == 1){_LLE1: _tmp112=(_tmp108.PlaceL).val;_LLE2: {
# 785
struct Cyc_Dict_Dict _tmp109=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp114,env->all_changed,_tmp112,rval);
_tmp113=Cyc_Relations_reln_assign_exp(fenv->r,_tmp113,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp109,_tmp113);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 791
struct _tuple15 _tmp10A=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp114,_tmp112));struct _tuple15 _tmp10B=_tmp10A;void*_tmp10F;_LLE6: _tmp10F=_tmp10B.f1;_LLE7:;{
void*_tmp10C=_tmp10F;switch(*((int*)_tmp10C)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp10C)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE9: _LLEA:
 goto _LLEC;}else{goto _LLEF;}case 7: _LLEB: _LLEC:
 goto _LLEE;case 0: _LLED: _LLEE:
 goto _LLE8;default: _LLEF: _LLF0:
# 797
({void*_tmp10D=0;Cyc_Tcutil_warn(lexp->loc,({const char*_tmp10E="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmp10E,sizeof(char),47);}),_tag_dyneither(_tmp10D,sizeof(void*),0));});
goto _LLE8;}_LLE8:;};}
# 802
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmp110;_tmp110.f1=outflow;_tmp110.f2=rval;_tmp110;});}}else{_LLE3: _LLE4:
# 805
 return({struct _tuple16 _tmp111;_tmp111.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp111.f2=rval;_tmp111;});}_LLE0:;}}_LLDB:;}
# 812
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 818
union Cyc_CfFlowInfo_FlowInfo _tmp115=f;struct Cyc_Dict_Dict _tmp11B;struct Cyc_List_List*_tmp11A;if((_tmp115.BottomFL).tag == 1){_LLF2: _LLF3:
 return Cyc_CfFlowInfo_BottomFL();}else{_LLF4: _tmp11B=((_tmp115.ReachableFL).val).f1;_tmp11A=((_tmp115.ReachableFL).val).f2;_LLF5:
# 823
 _tmp11B=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp11B,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp116=_region_malloc(env->r,sizeof(*_tmp116));_tmp116->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp117=_region_malloc(env->r,sizeof(*_tmp117));_tmp117[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp118;_tmp118.tag=0;_tmp118.f1=vd;_tmp118;});_tmp117;});_tmp116->fields=0;_tmp116;}),rval);
# 826
_tmp11A=Cyc_Relations_reln_assign_var(env->r,_tmp11A,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp119=Cyc_CfFlowInfo_ReachableFL(_tmp11B,_tmp11A);
Cyc_NewControlFlow_update_tryflow(env,_tmp119);
# 831
return _tmp119;};}_LLF1:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 835
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 842
if(vds == 0)return inflow;{
# 845
struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)es=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp11D->tl=es;_tmp11D;});}}
# 851
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11C,fenv->unknown_all,pat_loc,name_locs);
# 853
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 857
struct _tuple16 _tmp11E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp11F=_tmp11E;union Cyc_CfFlowInfo_FlowInfo _tmp121;void*_tmp120;_LLF7: _tmp121=_tmp11F.f1;_tmp120=_tmp11F.f2;_LLF8:;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp121,_tmp120);}}{
# 865
struct Cyc_List_List*_tmp122=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
struct _tuple24*_tmp123=(struct _tuple24*)_tmp122->hd;struct _tuple24*_tmp124=_tmp123;struct Cyc_Absyn_Vardecl**_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;_LLFA: _tmp13F=_tmp124->f1;_tmp13E=_tmp124->f2;_LLFB:;
if(_tmp13F != 0  && _tmp13E != 0){
if(_tmp13E->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp13E));({void*_tmp125[1]={& _tmp127};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp126="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp126,sizeof(char),41);}),_tag_dyneither(_tmp125,sizeof(void*),1));});});{
# 879
struct Cyc_List_List l=({struct Cyc_List_List _tmp13D;_tmp13D.hd=_tmp13E;_tmp13D.tl=0;_tmp13D;});
union Cyc_CfFlowInfo_FlowInfo _tmp128=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp129=Cyc_NewControlFlow_anal_Lexp(env,_tmp128,0,0,_tmp13E);struct _tuple18 _tmp12A=_tmp129;union Cyc_CfFlowInfo_AbsLVal _tmp13C;_LLFD: _tmp13C=_tmp12A.f2;_LLFE:;{
struct _tuple16 _tmp12B=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp128,_tmp13E);struct _tuple16 _tmp12C=_tmp12B;union Cyc_CfFlowInfo_FlowInfo _tmp13B;void*_tmp13A;_LL100: _tmp13B=_tmp12C.f1;_tmp13A=_tmp12C.f2;_LL101:;{
union Cyc_CfFlowInfo_FlowInfo _tmp12D=_tmp13B;struct Cyc_Dict_Dict _tmp139;struct Cyc_List_List*_tmp138;if((_tmp12D.ReachableFL).tag == 2){_LL103: _tmp139=((_tmp12D.ReachableFL).val).f1;_tmp138=((_tmp12D.ReachableFL).val).f2;_LL104:
# 885
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp12E=_tmp13C;struct Cyc_CfFlowInfo_Place*_tmp137;if((_tmp12E.PlaceL).tag == 1){_LL108: _tmp137=(_tmp12E.PlaceL).val;_LL109:
# 888
 _tmp13A=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp12F=_region_malloc(fenv->r,sizeof(*_tmp12F));_tmp12F[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp130;_tmp130.tag=8;_tmp130.f1=*_tmp13F;_tmp130.f2=_tmp13A;_tmp130;});_tmp12F;});{
# 891
void*_tmp131=Cyc_CfFlowInfo_lookup_place(_tmp139,_tmp137);
_tmp131=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp132=_region_malloc(fenv->r,sizeof(*_tmp132));_tmp132[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp133;_tmp133.tag=8;_tmp133.f1=*_tmp13F;_tmp133.f2=_tmp131;_tmp133;});_tmp132;});
_tmp139=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp139,env->all_changed,_tmp137,_tmp131);
_tmp13B=Cyc_CfFlowInfo_ReachableFL(_tmp139,_tmp138);
goto _LL107;};}else{_LL10A: _LL10B:
# 898
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp13E->topt)) && !
# 900
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp13F)->type))
# 902
({struct Cyc_String_pa_PrintArg_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp13E));({void*_tmp134[1]={& _tmp136};Cyc_CfFlowInfo_aerr(exp_loc,({const char*_tmp135="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp135,sizeof(char),46);}),_tag_dyneither(_tmp134,sizeof(void*),1));});});}_LL107:;}
# 910
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp13B,*_tmp13F,_tmp13E,_tmp13A,pat_loc);
goto _LL102;}else{_LL105: _LL106:
# 913
 goto _LL102;}_LL102:;};};};}}
# 918
return inflow;};};}
# 921
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 923
void*_tmp140=e->r;void*_tmp141=_tmp140;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp144;switch(*((int*)_tmp141)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp141)->f1)){case 4: _LL10D: _LL10E:
 goto _LL110;case 3: _LL10F: _LL110:
 goto _LL112;case 5: _LL111: _LL112:
 return 1;default: goto _LL11D;}case 19: _LL113: _tmp144=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL114:
 _tmp145=_tmp144;goto _LL116;case 20: _LL115: _tmp145=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL116:
 _tmp146=_tmp145;goto _LL118;case 21: _LL117: _tmp146=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL118:
# 930
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp146,cast_ok);case 22: _LL119: _tmp147=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL11A: {
# 932
void*_tmp142=Cyc_Tcutil_compress((void*)_check_null(_tmp147->topt));void*_tmp143=_tmp142;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->tag == 10){_LL120: _LL121:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp147,cast_ok);}else{_LL122: _LL123:
 return 0;}_LL11F:;}case 13: _LL11B: _tmp148=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_LL11C:
# 937
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp148,cast_ok);else{
return 0;}default: _LL11D: _LL11E:
 return 0;}_LL10C:;}
# 943
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp149=*rop;union Cyc_Relations_RelnOp _tmp14A=_tmp149;unsigned int _tmp14E;unsigned int _tmp14D;switch((_tmp14A.RParamNumelts).tag){case 5: _LL125: _tmp14D=(_tmp14A.RParam).val;_LL126: {
# 946
struct Cyc_Absyn_Exp*_tmp14B=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp14D);
return Cyc_Relations_exp2relnop(_tmp14B,rop);}case 6: _LL127: _tmp14E=(_tmp14A.RParamNumelts).val;_LL128: {
# 949
struct Cyc_Absyn_Exp*_tmp14C=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp14E);
return Cyc_Relations_exp2relnop(Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp14C,0),rop);}default: _LL129: _LL12A:
 return 1;}_LL124:;}
# 955
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp14F=rop;unsigned int _tmp151;unsigned int _tmp150;switch((_tmp14F.RParamNumelts).tag){case 5: _LL12C: _tmp150=(_tmp14F.RParam).val;_LL12D:
# 958
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp150));case 6: _LL12E: _tmp151=(_tmp14F.RParamNumelts).val;_LL12F:
# 960
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp151));default: _LL130: _LL131:
 return Cyc_Relations_rop2string(rop);}_LL12B:;}
# 965
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 968
union Cyc_CfFlowInfo_FlowInfo _tmp152=inflow;struct Cyc_Dict_Dict _tmp15C;struct Cyc_List_List*_tmp15B;if((_tmp152.BottomFL).tag == 1){_LL133: _LL134:
 return;}else{_LL135: _tmp15C=((_tmp152.ReachableFL).val).f1;_tmp15B=((_tmp152.ReachableFL).val).f2;_LL136:
# 971
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp153=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp153->rop1;
union Cyc_Relations_RelnOp rop2=_tmp153->rop2;
enum Cyc_Relations_Relation _tmp154=Cyc_Relations_flip_relation(_tmp153->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(env->r,rop2,_tmp154,rop1,_tmp15B))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp153->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp15B));({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0;_tmp158.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0;_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp155[4]={& _tmp157,& _tmp158,& _tmp159,& _tmp15A};Cyc_Tcutil_terr(loc,({const char*_tmp156="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp156,sizeof(char),77);}),_tag_dyneither(_tmp155,sizeof(void*),4));});});});});});
break;}}
# 986
goto _LL132;}_LL132:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 991
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 995
struct Cyc_CfFlowInfo_FlowEnv*_tmp15D=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 1008
{union Cyc_CfFlowInfo_FlowInfo _tmp15E=inflow;struct Cyc_Dict_Dict _tmp161;struct Cyc_List_List*_tmp160;if((_tmp15E.BottomFL).tag == 1){_LL138: _LL139:
 return({struct _tuple16 _tmp15F;_tmp15F.f1=Cyc_CfFlowInfo_BottomFL();_tmp15F.f2=_tmp15D->unknown_all;_tmp15F;});}else{_LL13A: _tmp161=((_tmp15E.ReachableFL).val).f1;_tmp160=((_tmp15E.ReachableFL).val).f2;_LL13B:
 d=_tmp161;relns=_tmp160;}_LL137:;}
# 1023 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 1044 "new_control_flow.cyc"
struct _tuple18 _tmp162=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp163=_tmp162;union Cyc_CfFlowInfo_FlowInfo _tmp173;union Cyc_CfFlowInfo_AbsLVal _tmp172;_LL13D: _tmp173=_tmp163.f1;_tmp172=_tmp163.f2;_LL13E:;{
struct _tuple18 _tmp164=({struct _tuple18 _tmp171;_tmp171.f1=_tmp173;_tmp171.f2=_tmp172;_tmp171;});struct _tuple18 _tmp165=_tmp164;struct Cyc_Dict_Dict _tmp170;struct Cyc_List_List*_tmp16F;struct Cyc_CfFlowInfo_Place*_tmp16E;if(((_tmp165.f1).ReachableFL).tag == 2){if(((_tmp165.f2).PlaceL).tag == 1){_LL140: _tmp170=(((_tmp165.f1).ReachableFL).val).f1;_tmp16F=(((_tmp165.f1).ReachableFL).val).f2;_tmp16E=((_tmp165.f2).PlaceL).val;_LL141: {
# 1047
void*_tmp166=Cyc_CfFlowInfo_lookup_place(_tmp170,_tmp16E);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp166,& needs_unconsume)){
({void*_tmp167=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp168="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp168,sizeof(char),56);}),_tag_dyneither(_tmp167,sizeof(void*),0));});
return({struct _tuple16 _tmp169;_tmp169.f1=Cyc_CfFlowInfo_BottomFL();_tmp169.f2=_tmp15D->unknown_all;_tmp169;});}else{
# 1053
if(needs_unconsume)
# 1055
return({struct _tuple16 _tmp16A;_tmp16A.f1=_tmp173;_tmp16A.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp15D,_tmp166);_tmp16A;});else{
# 1058
void*_tmp16B=Cyc_CfFlowInfo_make_unique_consumed(_tmp15D,(void*)_check_null(e->topt),e,env->iteration_num,_tmp166);
struct Cyc_Dict_Dict _tmp16C=Cyc_CfFlowInfo_assign_place(_tmp15D,e->loc,_tmp170,env->all_changed,_tmp16E,_tmp16B);
# 1070
return({struct _tuple16 _tmp16D;_tmp16D.f1=Cyc_CfFlowInfo_ReachableFL(_tmp16C,_tmp16F);_tmp16D.f2=_tmp166;_tmp16D;});}}}}else{goto _LL142;}}else{_LL142: _LL143:
# 1072
 goto _LL13F;}_LL13F:;};}{
# 1075
void*_tmp174=e->r;void*_tmp175=_tmp174;struct Cyc_Absyn_Stmt*_tmp382;struct Cyc_Absyn_Exp*_tmp381;void*_tmp380;int _tmp37F;struct Cyc_Absyn_Vardecl*_tmp37E;struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*_tmp37C;int _tmp37B;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*_tmp379;enum Cyc_Absyn_AggrKind _tmp378;struct Cyc_List_List*_tmp377;struct Cyc_List_List*_tmp376;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_Absyn_Exp*_tmp371;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*_tmp36F;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*_tmp36C;struct Cyc_Absyn_Exp*_tmp36B;struct _dyneither_ptr*_tmp36A;struct Cyc_Absyn_Exp*_tmp369;struct _dyneither_ptr*_tmp368;struct Cyc_Absyn_Exp*_tmp367;struct _dyneither_ptr*_tmp366;struct Cyc_Absyn_Exp*_tmp365;struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Absyn_Exp*_tmp361;struct Cyc_Absyn_Exp*_tmp360;int _tmp35F;struct Cyc_Absyn_Exp*_tmp35E;void**_tmp35D;struct Cyc_Absyn_Exp*_tmp35C;int _tmp35B;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_List_List*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Exp*_tmp352;struct Cyc_Absyn_Exp*_tmp351;enum Cyc_Absyn_Primop _tmp350;struct Cyc_List_List*_tmp34F;struct Cyc_List_List*_tmp34E;struct Cyc_Absyn_Datatypedecl*_tmp34D;struct Cyc_Absyn_Vardecl*_tmp34C;struct Cyc_Absyn_Vardecl*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_Absyn_Exp*_tmp348;struct Cyc_Absyn_Exp*_tmp347;struct Cyc_Absyn_Exp*_tmp346;switch(*((int*)_tmp175)){case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp175)->f4 == Cyc_Absyn_NonNull_to_Null){_LL145: _tmp346=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL146: {
# 1079
struct _tuple16 _tmp176=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp346);struct _tuple16 _tmp177=_tmp176;union Cyc_CfFlowInfo_FlowInfo _tmp17E;void*_tmp17D;_LL1AE: _tmp17E=_tmp177.f1;_tmp17D=_tmp177.f2;_LL1AF:;{
struct _tuple16 _tmp178=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp17E,_tmp346,_tmp17D);struct _tuple16 _tmp179=_tmp178;union Cyc_CfFlowInfo_FlowInfo _tmp17C;void*_tmp17B;_LL1B1: _tmp17C=_tmp179.f1;_tmp17B=_tmp179.f2;_LL1B2:;
return({struct _tuple16 _tmp17A;_tmp17A.f1=_tmp17C;_tmp17A.f2=_tmp17D;_tmp17A;});};}}else{_LL147: _tmp347=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL148:
# 1085
 _tmp348=_tmp347;goto _LL14A;}case 11: _LL149: _tmp348=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL14A:
 _tmp349=_tmp348;goto _LL14C;case 12: _LL14B: _tmp349=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL14C:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp349);case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp175)->f1).Int_c).tag){case 1: _LL14D: _LL14E:
# 1089
 goto _LL150;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp175)->f1).Int_c).val).f2 == 0){_LL14F: _LL150:
 return({struct _tuple16 _tmp17F;_tmp17F.f1=inflow;_tmp17F.f2=_tmp15D->zero;_tmp17F;});}else{_LL151: _LL152:
 goto _LL154;}default: _LL157: _LL158:
# 1095
 goto _LL15A;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp175)->f1)){case 2: _LL153: _LL154:
# 1092
 return({struct _tuple16 _tmp180;_tmp180.f1=inflow;_tmp180.f2=_tmp15D->notzeroall;_tmp180;});case 1: _LL163: _LL164:
# 1103
 return({struct _tuple16 _tmp182;_tmp182.f1=inflow;_tmp182.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),0,_tmp15D->unknown_all);_tmp182;});case 3: _LL165: _tmp34A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp175)->f1)->f1;_LL166:
# 1106
 _tmp34B=_tmp34A;goto _LL168;case 4: _LL167: _tmp34B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp175)->f1)->f1;_LL168:
 _tmp34C=_tmp34B;goto _LL16A;case 5: _LL169: _tmp34C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp175)->f1)->f1;_LL16A: {
# 1110
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp184;_tmp184.tag=0;_tmp184.f1=_tmp34C;_tmp184;});
return({struct _tuple16 _tmp183;_tmp183.f1=inflow;_tmp183.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);_tmp183;});}default: _LL1A3: _LL1A4:
# 1717
 goto _LL1A6;}case 30: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp175)->f1 == 0){_LL155: _LL156:
# 1094
 goto _LL158;}else{_LL191: _tmp34E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp34D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL192:
# 1573
 _tmp375=_tmp34E;goto _LL194;}case 17: _LL159: _LL15A:
# 1096
 goto _LL15C;case 16: _LL15B: _LL15C:
 goto _LL15E;case 18: _LL15D: _LL15E:
 goto _LL160;case 32: _LL15F: _LL160:
 goto _LL162;case 31: _LL161: _LL162:
 return({struct _tuple16 _tmp181;_tmp181.f1=inflow;_tmp181.f2=_tmp15D->unknown_all;_tmp181;});case 2: _LL16B: _tmp350=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp34F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL16C: {
# 1117
struct _tuple16 _tmp185=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp34F);struct _tuple16 _tmp186=_tmp185;union Cyc_CfFlowInfo_FlowInfo _tmp189;void*_tmp188;_LL1B4: _tmp189=_tmp186.f1;_tmp188=_tmp186.f2;_LL1B5:;
return({struct _tuple16 _tmp187;_tmp187.f1=_tmp189;_tmp187.f2=_tmp188;_tmp187;});}case 4: _LL16D: _tmp351=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL16E: {
# 1121
struct Cyc_List_List _tmp18A=({struct Cyc_List_List _tmp198;_tmp198.hd=_tmp351;_tmp198.tl=0;_tmp198;});
struct _tuple16 _tmp18B=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp18A);struct _tuple16 _tmp18C=_tmp18B;union Cyc_CfFlowInfo_FlowInfo _tmp197;_LL1B7: _tmp197=_tmp18C.f1;_LL1B8:;{
struct _tuple18 _tmp18D=Cyc_NewControlFlow_anal_Lexp(env,_tmp197,0,0,_tmp351);struct _tuple18 _tmp18E=_tmp18D;union Cyc_CfFlowInfo_AbsLVal _tmp196;_LL1BA: _tmp196=_tmp18E.f2;_LL1BB:;
{struct _tuple25 _tmp18F=({struct _tuple25 _tmp194;_tmp194.f1=_tmp196;_tmp194.f2=_tmp197;_tmp194;});struct _tuple25 _tmp190=_tmp18F;struct Cyc_CfFlowInfo_Place*_tmp193;struct Cyc_Dict_Dict _tmp192;struct Cyc_List_List*_tmp191;if(((_tmp190.f1).PlaceL).tag == 1){if(((_tmp190.f2).ReachableFL).tag == 2){_LL1BD: _tmp193=((_tmp190.f1).PlaceL).val;_tmp192=(((_tmp190.f2).ReachableFL).val).f1;_tmp191=(((_tmp190.f2).ReachableFL).val).f2;_LL1BE:
# 1126
 _tmp191=Cyc_Relations_reln_kill_exp(_tmp15D->r,_tmp191,_tmp351);
_tmp197=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp15D,_tmp351->loc,_tmp192,env->all_changed,_tmp193,_tmp15D->unknown_all),_tmp191);
# 1131
goto _LL1BC;}else{goto _LL1BF;}}else{_LL1BF: _LL1C0:
 goto _LL1BC;}_LL1BC:;}
# 1134
return({struct _tuple16 _tmp195;_tmp195.f1=_tmp197;_tmp195.f2=_tmp15D->unknown_all;_tmp195;});};}case 3: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp175)->f2 != 0){_LL16F: _tmp353=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp352=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp175)->f3;_LL170:
# 1137
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
({void*_tmp199=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp19A="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp19A,sizeof(char),62);}),_tag_dyneither(_tmp199,sizeof(void*),0));});
return({struct _tuple16 _tmp19B;_tmp19B.f1=Cyc_CfFlowInfo_BottomFL();_tmp19B.f2=_tmp15D->unknown_all;_tmp19B;});}{
# 1141
struct Cyc_List_List _tmp19C=({struct Cyc_List_List _tmp1AB;_tmp1AB.hd=_tmp352;_tmp1AB.tl=0;_tmp1AB;});
struct Cyc_List_List _tmp19D=({struct Cyc_List_List _tmp1AA;_tmp1AA.hd=_tmp353;_tmp1AA.tl=& _tmp19C;_tmp1AA;});
struct _tuple16 _tmp19E=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp19D);struct _tuple16 _tmp19F=_tmp19E;union Cyc_CfFlowInfo_FlowInfo _tmp1A9;_LL1C2: _tmp1A9=_tmp19F.f1;_LL1C3:;{
struct _tuple18 _tmp1A0=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A9,0,0,_tmp353);struct _tuple18 _tmp1A1=_tmp1A0;union Cyc_CfFlowInfo_AbsLVal _tmp1A8;_LL1C5: _tmp1A8=_tmp1A1.f2;_LL1C6:;
{union Cyc_CfFlowInfo_FlowInfo _tmp1A2=_tmp1A9;struct Cyc_Dict_Dict _tmp1A6;struct Cyc_List_List*_tmp1A5;if((_tmp1A2.ReachableFL).tag == 2){_LL1C8: _tmp1A6=((_tmp1A2.ReachableFL).val).f1;_tmp1A5=((_tmp1A2.ReachableFL).val).f2;_LL1C9:
# 1147
{union Cyc_CfFlowInfo_AbsLVal _tmp1A3=_tmp1A8;struct Cyc_CfFlowInfo_Place*_tmp1A4;if((_tmp1A3.PlaceL).tag == 1){_LL1CD: _tmp1A4=(_tmp1A3.PlaceL).val;_LL1CE:
# 1151
 _tmp1A5=Cyc_Relations_reln_kill_exp(_tmp15D->r,_tmp1A5,_tmp353);
_tmp1A6=Cyc_CfFlowInfo_assign_place(_tmp15D,_tmp353->loc,_tmp1A6,env->all_changed,_tmp1A4,_tmp15D->unknown_all);
# 1154
_tmp1A9=Cyc_CfFlowInfo_ReachableFL(_tmp1A6,_tmp1A5);
# 1158
goto _LL1CC;}else{_LL1CF: _LL1D0:
# 1161
 goto _LL1CC;}_LL1CC:;}
# 1163
goto _LL1C7;}else{_LL1CA: _LL1CB:
 goto _LL1C7;}_LL1C7:;}
# 1166
return({struct _tuple16 _tmp1A7;_tmp1A7.f1=_tmp1A9;_tmp1A7.f2=_tmp15D->unknown_all;_tmp1A7;});};};}else{_LL171: _tmp355=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp354=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp175)->f3;_LL172:
# 1170
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
({void*_tmp1AC=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp1AD="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1AD,sizeof(char),62);}),_tag_dyneither(_tmp1AC,sizeof(void*),0));});
return({struct _tuple16 _tmp1AE;_tmp1AE.f1=Cyc_CfFlowInfo_BottomFL();_tmp1AE.f2=_tmp15D->unknown_all;_tmp1AE;});}{
# 1174
struct Cyc_Dict_Dict*_tmp1AF=env->all_changed;
# 1176
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,({struct Cyc_Absyn_Exp*_tmp1B0[2];_tmp1B0[1]=_tmp354;_tmp1B0[0]=_tmp355;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp1B0,sizeof(struct Cyc_Absyn_Exp*),2));}));
while(1){
env->all_changed=({struct Cyc_Dict_Dict*_tmp1B1=_region_malloc(env->r,sizeof(*_tmp1B1));_tmp1B1[0]=_tmp15D->mt_place_set;_tmp1B1;});{
struct _tuple18 _tmp1B2=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp355);struct _tuple18 _tmp1B3=_tmp1B2;union Cyc_CfFlowInfo_FlowInfo _tmp1BE;union Cyc_CfFlowInfo_AbsLVal _tmp1BD;_LL1D2: _tmp1BE=_tmp1B3.f1;_tmp1BD=_tmp1B3.f2;_LL1D3:;{
struct Cyc_Dict_Dict _tmp1B4=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
env->all_changed=({struct Cyc_Dict_Dict*_tmp1B5=_region_malloc(env->r,sizeof(*_tmp1B5));_tmp1B5[0]=_tmp15D->mt_place_set;_tmp1B5;});{
struct _tuple16 _tmp1B6=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp354);struct _tuple16 _tmp1B7=_tmp1B6;union Cyc_CfFlowInfo_FlowInfo _tmp1BC;void*_tmp1BB;_LL1D5: _tmp1BC=_tmp1B7.f1;_tmp1BB=_tmp1B7.f2;_LL1D6:;{
struct Cyc_Dict_Dict _tmp1B8=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp1B9=Cyc_CfFlowInfo_after_flow(_tmp15D,& _tmp1B4,_tmp1BE,_tmp1BC,_tmp1B4,_tmp1B8);
# 1187
union Cyc_CfFlowInfo_FlowInfo _tmp1BA=Cyc_CfFlowInfo_join_flow(_tmp15D,_tmp1AF,inflow,_tmp1B9);
# 1190
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1BA,inflow)){
if(_tmp1AF == 0)
env->all_changed=0;else{
# 1194
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1AF,
Cyc_CfFlowInfo_union_place_set(_tmp1B4,_tmp1B8,0),0);}
# 1197
return Cyc_NewControlFlow_do_assign(_tmp15D,env,_tmp1B9,_tmp355,_tmp1BD,_tmp354,_tmp1BB,e->loc);}
# 1200
inflow=_tmp1BA;};};};};}};}case 8: _LL173: _tmp357=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp356=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL174: {
# 1205
struct _tuple16 _tmp1BF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp357);struct _tuple16 _tmp1C0=_tmp1BF;union Cyc_CfFlowInfo_FlowInfo _tmp1C2;void*_tmp1C1;_LL1D8: _tmp1C2=_tmp1C0.f1;_tmp1C1=_tmp1C0.f2;_LL1D9:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1C2,_tmp356);}case 10: _LL175: _tmp358=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL176: {
# 1209
struct _tuple16 _tmp1C3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp358);struct _tuple16 _tmp1C4=_tmp1C3;union Cyc_CfFlowInfo_FlowInfo _tmp1C7;void*_tmp1C6;_LL1DB: _tmp1C7=_tmp1C4.f1;_tmp1C6=_tmp1C4.f2;_LL1DC:;
Cyc_NewControlFlow_use_Rval(env,_tmp358->loc,_tmp1C7,_tmp1C6);
return({struct _tuple16 _tmp1C5;_tmp1C5.f1=Cyc_CfFlowInfo_BottomFL();_tmp1C5.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),0,_tmp15D->unknown_all);_tmp1C5;});}case 9: _LL177: _tmp35A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp359=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL178: {
# 1214
struct _RegionHandle _tmp1C8=_new_region("temp");struct _RegionHandle*temp=& _tmp1C8;_push_region(temp);
{struct Cyc_List_List*_tmp1C9=_tmp359;
struct _tuple23 _tmp1CA=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_List_List*_tmp1F7=_region_malloc(temp,sizeof(*_tmp1F7));_tmp1F7->hd=_tmp35A;_tmp1F7->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp359);_tmp1F7;}),0,1);struct _tuple23 _tmp1CB=_tmp1CA;union Cyc_CfFlowInfo_FlowInfo _tmp1F6;struct Cyc_List_List*_tmp1F5;_LL1DE: _tmp1F6=_tmp1CB.f1;_tmp1F5=_tmp1CB.f2;_LL1DF:;{
# 1218
union Cyc_CfFlowInfo_FlowInfo _tmp1CC=Cyc_NewControlFlow_use_Rval(env,_tmp35A->loc,_tmp1F6,(void*)((struct Cyc_List_List*)_check_null(_tmp1F5))->hd);
_tmp1F5=_tmp1F5->tl;{
# 1221
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1CD=Cyc_Tcutil_compress((void*)_check_null(_tmp35A->topt));void*_tmp1CE=_tmp1CD;void*_tmp1E0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CE)->tag == 5){_LL1E1: _tmp1E0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CE)->f1).elt_typ;_LL1E2:
# 1228
{void*_tmp1CF=Cyc_Tcutil_compress(_tmp1E0);void*_tmp1D0=_tmp1CF;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DB;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->tag == 9){_LL1E6: _tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->f1).attributes;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->f1).requires_relns;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->f1).ensures_relns;_LL1E7:
# 1230
 requires=_tmp1DC;
ensures=_tmp1DB;
for(0;_tmp1DD != 0;_tmp1DD=_tmp1DD->tl){
# 1234
void*_tmp1D1=(void*)_tmp1DD->hd;void*_tmp1D2=_tmp1D1;int _tmp1D8;int _tmp1D7;int _tmp1D6;switch(*((int*)_tmp1D2)){case 20: _LL1EB: _tmp1D6=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1D2)->f1;_LL1EC:
# 1236
 init_params=({struct Cyc_List_List*_tmp1D3=_region_malloc(temp,sizeof(*_tmp1D3));_tmp1D3->hd=(void*)_tmp1D6;_tmp1D3->tl=init_params;_tmp1D3;});goto _LL1EA;case 21: _LL1ED: _tmp1D7=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1D2)->f1;_LL1EE:
# 1238
 nolive_unique_params=({struct Cyc_List_List*_tmp1D4=_region_malloc(temp,sizeof(*_tmp1D4));_tmp1D4->hd=(void*)_tmp1D7;_tmp1D4->tl=nolive_unique_params;_tmp1D4;});
goto _LL1EA;case 22: _LL1EF: _tmp1D8=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1D2)->f1;_LL1F0:
# 1242
 noconsume_params=({struct Cyc_List_List*_tmp1D5=_region_malloc(temp,sizeof(*_tmp1D5));_tmp1D5->hd=(void*)_tmp1D8;_tmp1D5->tl=noconsume_params;_tmp1D5;});
goto _LL1EA;default: _LL1F1: _LL1F2:
 goto _LL1EA;}_LL1EA:;}
# 1246
goto _LL1E5;}else{_LL1E8: _LL1E9:
({void*_tmp1D9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1DA="anal_Rexp: bad function type";_tag_dyneither(_tmp1DA,sizeof(char),29);}),_tag_dyneither(_tmp1D9,sizeof(void*),0));});}_LL1E5:;}
# 1249
goto _LL1E0;}else{_LL1E3: _LL1E4:
({void*_tmp1DE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1DF="anal_Rexp: bad function type";_tag_dyneither(_tmp1DF,sizeof(char),29);}),_tag_dyneither(_tmp1DE,sizeof(void*),0));});}_LL1E0:;}
# 1252
{int i=1;for(0;_tmp1F5 != 0;(((_tmp1F5=_tmp1F5->tl,_tmp359=((struct Cyc_List_List*)_check_null(_tmp359))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1E1=_tmp1F6;struct Cyc_Dict_Dict _tmp1F0;if((_tmp1E1.BottomFL).tag == 1){_LL1F4: _LL1F5:
 goto _LL1F3;}else{_LL1F6: _tmp1F0=((_tmp1E1.ReachableFL).val).f1;_LL1F7:
# 1257
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1F0,(void*)_tmp1F5->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp1E2=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp359))->hd)->loc,({const char*_tmp1E3="expression may not be initialized";_tag_dyneither(_tmp1E3,sizeof(char),34);}),_tag_dyneither(_tmp1E2,sizeof(void*),0));});
{union Cyc_CfFlowInfo_FlowInfo _tmp1E4=_tmp1CC;struct Cyc_Dict_Dict _tmp1EF;struct Cyc_List_List*_tmp1EE;if((_tmp1E4.BottomFL).tag == 1){_LL1F9: _LL1FA:
 goto _LL1F8;}else{_LL1FB: _tmp1EF=((_tmp1E4.ReachableFL).val).f1;_tmp1EE=((_tmp1E4.ReachableFL).val).f2;_LL1FC: {
# 1264
struct Cyc_Dict_Dict _tmp1E5=Cyc_CfFlowInfo_escape_deref(_tmp15D,_tmp1EF,env->all_changed,(void*)_tmp1F5->hd);
{void*_tmp1E6=(void*)_tmp1F5->hd;void*_tmp1E7=_tmp1E6;struct Cyc_CfFlowInfo_Place*_tmp1ED;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E7)->tag == 5){_LL1FE: _tmp1ED=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E7)->f1;_LL1FF:
# 1267
{void*_tmp1E8=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp359))->hd)->topt));void*_tmp1E9=_tmp1E8;void*_tmp1EC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E9)->tag == 5){_LL203: _tmp1EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E9)->f1).elt_typ;_LL204:
# 1269
 _tmp1E5=Cyc_CfFlowInfo_assign_place(_tmp15D,((struct Cyc_Absyn_Exp*)_tmp359->hd)->loc,_tmp1E5,env->all_changed,_tmp1ED,
# 1271
Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,_tmp1EC,0,_tmp15D->esc_all));
# 1274
goto _LL202;}else{_LL205: _LL206:
({void*_tmp1EA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1EB="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1EB,sizeof(char),39);}),_tag_dyneither(_tmp1EA,sizeof(void*),0));});}_LL202:;}
# 1277
goto _LL1FD;}else{_LL200: _LL201:
 goto _LL1FD;}_LL1FD:;}
# 1280
_tmp1CC=Cyc_CfFlowInfo_ReachableFL(_tmp1E5,_tmp1EE);
goto _LL1F8;}}_LL1F8:;}
# 1283
goto _LL1F3;}_LL1F3:;}else{
# 1286
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1291
_tmp1CC=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp359))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp359))->hd)->topt),_tmp1CC,(void*)_tmp1F5->hd);else{
# 1301
_tmp1CC=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp359))->hd)->loc,_tmp1CC,(void*)_tmp1F5->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp359->hd)->topt),0))
_tmp1CC=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp1CC,(struct Cyc_Absyn_Exp*)_tmp359->hd,((struct Cyc_Absyn_Exp*)_tmp359->hd)->loc,(void*)_tmp1F5->hd);}}}}
# 1308
Cyc_NewControlFlow_check_fn_requires(env,_tmp1CC,_tmp1C9,requires,e->loc);
# 1311
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp35A->topt))){
struct _tuple16 _tmp1F2=({struct _tuple16 _tmp1F1;_tmp1F1.f1=Cyc_CfFlowInfo_BottomFL();_tmp1F1.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),0,_tmp15D->unknown_all);_tmp1F1;});_npop_handler(0);return _tmp1F2;}else{
# 1314
struct _tuple16 _tmp1F4=({struct _tuple16 _tmp1F3;_tmp1F3.f1=_tmp1CC;_tmp1F3.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),0,_tmp15D->unknown_all);_tmp1F3;});_npop_handler(0);return _tmp1F4;}};};}
# 1215
;_pop_region(temp);}case 33: _LL179: _tmp35F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp175)->f1).is_calloc;_tmp35E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp175)->f1).rgn;_tmp35D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp175)->f1).elt_type;_tmp35C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp175)->f1).num_elts;_tmp35B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp175)->f1).fat_result;_LL17A: {
# 1317
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp209=_region_malloc(_tmp15D->r,sizeof(*_tmp209));_tmp209[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp20A;_tmp20A.tag=1;_tmp20A.f1=_tmp35C;_tmp20A.f2=(void*)_check_null(e->topt);_tmp20A;});_tmp209;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp208=_region_malloc(_tmp15D->r,sizeof(*_tmp208));_tmp208->root=root;_tmp208->fields=0;_tmp208;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp206=_region_malloc(_tmp15D->r,sizeof(*_tmp206));_tmp206[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp207;_tmp207.tag=5;_tmp207.f1=place;_tmp207;});_tmp206;});
void*place_val;
if(_tmp35B)place_val=_tmp15D->notzeroall;else{
if(_tmp35F)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,*((void**)_check_null(_tmp35D)),0,_tmp15D->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,*((void**)_check_null(_tmp35D)),0,_tmp15D->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp35E != 0){
struct _RegionHandle _tmp1F8=_new_region("temp");struct _RegionHandle*temp=& _tmp1F8;_push_region(temp);
{struct _tuple23 _tmp1F9=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp1FD[2];_tmp1FD[1]=_tmp35C;_tmp1FD[0]=_tmp35E;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp1FD,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);struct _tuple23 _tmp1FA=_tmp1F9;union Cyc_CfFlowInfo_FlowInfo _tmp1FC;struct Cyc_List_List*_tmp1FB;_LL208: _tmp1FC=_tmp1FA.f1;_tmp1FB=_tmp1FA.f2;_LL209:;
# 1330
outflow=_tmp1FC;}
# 1328
;_pop_region(temp);}else{
# 1333
struct _tuple16 _tmp1FE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp35C);struct _tuple16 _tmp1FF=_tmp1FE;union Cyc_CfFlowInfo_FlowInfo _tmp200;_LL20B: _tmp200=_tmp1FF.f1;_LL20C:;
outflow=_tmp200;}{
# 1336
union Cyc_CfFlowInfo_FlowInfo _tmp201=outflow;struct Cyc_Dict_Dict _tmp205;struct Cyc_List_List*_tmp204;if((_tmp201.BottomFL).tag == 1){_LL20E: _LL20F:
 return({struct _tuple16 _tmp202;_tmp202.f1=outflow;_tmp202.f2=rval;_tmp202;});}else{_LL210: _tmp205=((_tmp201.ReachableFL).val).f1;_tmp204=((_tmp201.ReachableFL).val).f2;_LL211:
# 1339
 return({struct _tuple16 _tmp203;_tmp203.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp205,root,place_val),_tmp204);_tmp203.f2=rval;_tmp203;});}_LL20D:;};};}case 34: _LL17B: _tmp361=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp360=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL17C: {
# 1343
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1349
struct _RegionHandle _tmp20B=_new_region("temp");struct _RegionHandle*temp=& _tmp20B;_push_region(temp);{
struct _tuple23 _tmp20C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp210[2];_tmp210[1]=_tmp360;_tmp210[0]=_tmp361;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp210,sizeof(struct Cyc_Absyn_Exp*),2));}),0,0);struct _tuple23 _tmp20D=_tmp20C;union Cyc_CfFlowInfo_FlowInfo _tmp20F;struct Cyc_List_List*_tmp20E;_LL213: _tmp20F=_tmp20D.f1;_tmp20E=_tmp20D.f2;_LL214:;
# 1352
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp20E))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp20E->tl))->hd;
outflow=_tmp20F;}{
# 1357
struct _tuple18 _tmp211=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp361);struct _tuple18 _tmp212=_tmp211;union Cyc_CfFlowInfo_AbsLVal _tmp223;_LL216: _tmp223=_tmp212.f2;_LL217:;{
struct _tuple18 _tmp213=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp360);struct _tuple18 _tmp214=_tmp213;union Cyc_CfFlowInfo_AbsLVal _tmp222;_LL219: _tmp222=_tmp214.f2;_LL21A:;
{union Cyc_CfFlowInfo_FlowInfo _tmp215=outflow;struct Cyc_Dict_Dict _tmp21F;struct Cyc_List_List*_tmp21E;if((_tmp215.ReachableFL).tag == 2){_LL21C: _tmp21F=((_tmp215.ReachableFL).val).f1;_tmp21E=((_tmp215.ReachableFL).val).f2;_LL21D:
# 1361
{union Cyc_CfFlowInfo_AbsLVal _tmp216=_tmp223;struct Cyc_CfFlowInfo_Place*_tmp219;if((_tmp216.PlaceL).tag == 1){_LL221: _tmp219=(_tmp216.PlaceL).val;_LL222:
# 1363
 _tmp21F=Cyc_CfFlowInfo_assign_place(_tmp15D,_tmp361->loc,_tmp21F,env->all_changed,_tmp219,right_rval);
# 1365
goto _LL220;}else{_LL223: _LL224:
# 1370
 if(Cyc_CfFlowInfo_initlevel(_tmp15D,_tmp21F,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp217=0;Cyc_Tcutil_terr(_tmp360->loc,({const char*_tmp218="expression may not be fully initialized";_tag_dyneither(_tmp218,sizeof(char),40);}),_tag_dyneither(_tmp217,sizeof(void*),0));});
goto _LL220;}_LL220:;}
# 1374
{union Cyc_CfFlowInfo_AbsLVal _tmp21A=_tmp222;struct Cyc_CfFlowInfo_Place*_tmp21D;if((_tmp21A.PlaceL).tag == 1){_LL226: _tmp21D=(_tmp21A.PlaceL).val;_LL227:
# 1376
 _tmp21F=Cyc_CfFlowInfo_assign_place(_tmp15D,_tmp360->loc,_tmp21F,env->all_changed,_tmp21D,left_rval);
# 1378
goto _LL225;}else{_LL228: _LL229:
# 1380
 if(Cyc_CfFlowInfo_initlevel(_tmp15D,_tmp21F,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp21B=0;Cyc_Tcutil_terr(_tmp361->loc,({const char*_tmp21C="expression may not be fully initialized";_tag_dyneither(_tmp21C,sizeof(char),40);}),_tag_dyneither(_tmp21B,sizeof(void*),0));});
goto _LL225;}_LL225:;}
# 1385
_tmp21E=Cyc_Relations_reln_kill_exp(_tmp15D->r,_tmp21E,_tmp361);
_tmp21E=Cyc_Relations_reln_kill_exp(_tmp15D->r,_tmp21E,_tmp360);
# 1388
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp21F,_tmp21E);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL21B;}else{_LL21E: _LL21F:
 goto _LL21B;}_LL21B:;}{
# 1395
struct _tuple16 _tmp221=({struct _tuple16 _tmp220;_tmp220.f1=outflow;_tmp220.f2=_tmp15D->unknown_all;_tmp220;});_npop_handler(0);return _tmp221;};};};
# 1349
;_pop_region(temp);}case 15: _LL17D: _tmp363=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp362=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL17E: {
# 1398
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp236=_region_malloc(_tmp15D->r,sizeof(*_tmp236));_tmp236[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp237;_tmp237.tag=1;_tmp237.f1=_tmp362;_tmp237.f2=(void*)_check_null(e->topt);_tmp237;});_tmp236;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp235=_region_malloc(_tmp15D->r,sizeof(*_tmp235));_tmp235->root=root;_tmp235->fields=0;_tmp235;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp233=_region_malloc(_tmp15D->r,sizeof(*_tmp233));_tmp233[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp234;_tmp234.tag=5;_tmp234.f1=place;_tmp234;});_tmp233;});
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp363 != 0){
struct _RegionHandle _tmp224=_new_region("temp");struct _RegionHandle*temp=& _tmp224;_push_region(temp);
{struct _tuple23 _tmp225=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp229[2];_tmp229[1]=_tmp362;_tmp229[0]=_tmp363;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp229,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);struct _tuple23 _tmp226=_tmp225;union Cyc_CfFlowInfo_FlowInfo _tmp228;struct Cyc_List_List*_tmp227;_LL22B: _tmp228=_tmp226.f1;_tmp227=_tmp226.f2;_LL22C:;
# 1408
outflow=_tmp228;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp227))->tl))->hd;}
# 1406
;_pop_region(temp);}else{
# 1412
struct _tuple16 _tmp22A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp362);struct _tuple16 _tmp22B=_tmp22A;union Cyc_CfFlowInfo_FlowInfo _tmp22D;void*_tmp22C;_LL22E: _tmp22D=_tmp22B.f1;_tmp22C=_tmp22B.f2;_LL22F:;
outflow=_tmp22D;
place_val=_tmp22C;}{
# 1416
union Cyc_CfFlowInfo_FlowInfo _tmp22E=outflow;struct Cyc_Dict_Dict _tmp232;struct Cyc_List_List*_tmp231;if((_tmp22E.BottomFL).tag == 1){_LL231: _LL232:
 return({struct _tuple16 _tmp22F;_tmp22F.f1=outflow;_tmp22F.f2=rval;_tmp22F;});}else{_LL233: _tmp232=((_tmp22E.ReachableFL).val).f1;_tmp231=((_tmp22E.ReachableFL).val).f2;_LL234:
# 1419
 return({struct _tuple16 _tmp230;_tmp230.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp232,root,place_val),_tmp231);_tmp230.f2=rval;_tmp230;});}_LL230:;};};}case 14: _LL17F: _tmp364=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL180: {
# 1423
struct _tuple18 _tmp238=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp364);struct _tuple18 _tmp239=_tmp238;union Cyc_CfFlowInfo_FlowInfo _tmp241;union Cyc_CfFlowInfo_AbsLVal _tmp240;_LL236: _tmp241=_tmp239.f1;_tmp240=_tmp239.f2;_LL237:;{
union Cyc_CfFlowInfo_AbsLVal _tmp23A=_tmp240;struct Cyc_CfFlowInfo_Place*_tmp23F;if((_tmp23A.UnknownL).tag == 2){_LL239: _LL23A:
 return({struct _tuple16 _tmp23B;_tmp23B.f1=_tmp241;_tmp23B.f2=_tmp15D->notzeroall;_tmp23B;});}else{_LL23B: _tmp23F=(_tmp23A.PlaceL).val;_LL23C:
 return({struct _tuple16 _tmp23C;_tmp23C.f1=_tmp241;_tmp23C.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp23D=_region_malloc(env->r,sizeof(*_tmp23D));_tmp23D[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp23E;_tmp23E.tag=5;_tmp23E.f1=_tmp23F;_tmp23E;});_tmp23D;});_tmp23C;});}_LL238:;};}case 19: _LL181: _tmp365=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL182: {
# 1430
struct _tuple16 _tmp242=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp365);struct _tuple16 _tmp243=_tmp242;union Cyc_CfFlowInfo_FlowInfo _tmp245;void*_tmp244;_LL23E: _tmp245=_tmp243.f1;_tmp244=_tmp243.f2;_LL23F:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp245,_tmp365,_tmp244);}case 20: _LL183: _tmp367=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp366=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL184: {
# 1434
struct _tuple16 _tmp246=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp367);struct _tuple16 _tmp247=_tmp246;union Cyc_CfFlowInfo_FlowInfo _tmp264;void*_tmp263;_LL241: _tmp264=_tmp247.f1;_tmp263=_tmp247.f2;_LL242:;
if(_tmp367->topt == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp248[1]={& _tmp24A};Cyc_fprintf(Cyc_stderr,({const char*_tmp249="aggrmember exp %s\n";_tag_dyneither(_tmp249,sizeof(char),19);}),_tag_dyneither(_tmp248,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp24D;_tmp24D.tag=0;_tmp24D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp367));({void*_tmp24B[1]={& _tmp24D};Cyc_fprintf(Cyc_stderr,({const char*_tmp24C="oops! %s.topt = null!\n";_tag_dyneither(_tmp24C,sizeof(char),23);}),_tag_dyneither(_tmp24B,sizeof(void*),1));});});}{
# 1439
void*_tmp24E=(void*)_check_null(_tmp367->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp24E))
# 1442
return({struct _tuple16 _tmp24F;_tmp24F.f1=_tmp264;_tmp24F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),0,_tmp15D->unknown_all);_tmp24F;});
# 1444
if(Cyc_Absyn_is_require_union_type(_tmp24E))
Cyc_NewControlFlow_check_union_requires(_tmp367->loc,_tmp15D->r,_tmp24E,_tmp366,_tmp264);{
# 1447
struct _tuple15 _tmp250=Cyc_CfFlowInfo_unname_rval(_tmp15D->r,_tmp263);struct _tuple15 _tmp251=_tmp250;void*_tmp262;_LL244: _tmp262=_tmp251.f1;_LL245:;{
void*_tmp252=_tmp262;int _tmp261;int _tmp260;struct _dyneither_ptr _tmp25F;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->tag == 6){_LL247: _tmp261=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->f1).is_union;_tmp260=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->f1).fieldnum;_tmp25F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp252)->f2;_LL248: {
# 1450
int _tmp253=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp367->topt),_tmp366);
if((_tmp261  && _tmp260 != - 1) && _tmp260 != _tmp253)
return({struct _tuple16 _tmp254;_tmp254.f1=_tmp264;_tmp254.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),1,_tmp15D->unknown_none);_tmp254;});
return({struct _tuple16 _tmp255;_tmp255.f1=_tmp264;_tmp255.f2=*((void**)_check_dyneither_subscript(_tmp25F,sizeof(void*),_tmp253));_tmp255;});}}else{_LL249: _LL24A:
# 1455
({void*_tmp256=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp257="the bad rexp is :";_tag_dyneither(_tmp257,sizeof(char),18);}),_tag_dyneither(_tmp256,sizeof(void*),0));});
Cyc_CfFlowInfo_print_absrval(_tmp262);
({void*_tmp258=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp259="\n";_tag_dyneither(_tmp259,sizeof(char),2);}),_tag_dyneither(_tmp258,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Core_Impossible_exn_struct _tmp25B;_tmp25B.tag=Cyc_Core_Impossible;_tmp25B.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp25C[1]={& _tmp25E};Cyc_aprintf(({const char*_tmp25D="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp25D,sizeof(char),26);}),_tag_dyneither(_tmp25C,sizeof(void*),1));});});_tmp25B;});_tmp25A;}));}_LL246:;};};};}case 37: _LL185: _tmp369=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp368=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL186: {
# 1465
struct _tuple16 _tmp265=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp369);struct _tuple16 _tmp266=_tmp265;union Cyc_CfFlowInfo_FlowInfo _tmp279;void*_tmp278;_LL24C: _tmp279=_tmp266.f1;_tmp278=_tmp266.f2;_LL24D:;
# 1467
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp369->topt)))
return({struct _tuple16 _tmp267;_tmp267.f1=_tmp279;_tmp267.f2=_tmp15D->unknown_all;_tmp267;});{
struct _tuple15 _tmp268=Cyc_CfFlowInfo_unname_rval(_tmp15D->r,_tmp278);struct _tuple15 _tmp269=_tmp268;void*_tmp277;_LL24F: _tmp277=_tmp269.f1;_LL250:;{
void*_tmp26A=_tmp277;int _tmp276;int _tmp275;struct _dyneither_ptr _tmp274;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26A)->tag == 6){_LL252: _tmp276=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26A)->f1).is_union;_tmp275=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26A)->f1).fieldnum;_tmp274=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26A)->f2;_LL253: {
# 1472
int _tmp26B=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp369->topt),_tmp368);
if(_tmp276  && _tmp275 != - 1){
if(_tmp275 != _tmp26B)
return({struct _tuple16 _tmp26C;_tmp26C.f1=_tmp279;_tmp26C.f2=_tmp15D->zero;_tmp26C;});else{
# 1477
return({struct _tuple16 _tmp26D;_tmp26D.f1=_tmp279;_tmp26D.f2=_tmp15D->notzeroall;_tmp26D;});}}else{
# 1479
return({struct _tuple16 _tmp26E;_tmp26E.f1=_tmp279;_tmp26E.f2=_tmp15D->unknown_all;_tmp26E;});}}}else{_LL254: _LL255:
# 1481
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Core_Impossible_exn_struct _tmp270;_tmp270.tag=Cyc_Core_Impossible;_tmp270.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp273;_tmp273.tag=0;_tmp273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp271[1]={& _tmp273};Cyc_aprintf(({const char*_tmp272="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp272,sizeof(char),26);}),_tag_dyneither(_tmp271,sizeof(void*),1));});});_tmp270;});_tmp26F;}));}_LL251:;};};}case 21: _LL187: _tmp36B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp36A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL188: {
# 1486
struct _tuple16 _tmp27A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp36B);struct _tuple16 _tmp27B=_tmp27A;union Cyc_CfFlowInfo_FlowInfo _tmp295;void*_tmp294;_LL257: _tmp295=_tmp27B.f1;_tmp294=_tmp27B.f2;_LL258:;{
# 1489
struct _tuple16 _tmp27C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp295,_tmp36B,_tmp294);struct _tuple16 _tmp27D=_tmp27C;union Cyc_CfFlowInfo_FlowInfo _tmp293;void*_tmp292;_LL25A: _tmp293=_tmp27D.f1;_tmp292=_tmp27D.f2;_LL25B:;{
# 1492
void*_tmp27E=Cyc_Tcutil_compress((void*)_check_null(_tmp36B->topt));void*_tmp27F=_tmp27E;void*_tmp291;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27F)->tag == 5){_LL25D: _tmp291=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27F)->f1).elt_typ;_LL25E:
# 1494
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp291))
# 1496
return({struct _tuple16 _tmp280;_tmp280.f1=_tmp293;_tmp280.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),0,_tmp15D->unknown_all);_tmp280;});
# 1498
if(Cyc_Absyn_is_require_union_type(_tmp291))
Cyc_NewControlFlow_check_union_requires(_tmp36B->loc,_tmp15D->r,_tmp291,_tmp36A,_tmp295);{
# 1501
struct _tuple15 _tmp281=Cyc_CfFlowInfo_unname_rval(_tmp15D->r,_tmp292);struct _tuple15 _tmp282=_tmp281;void*_tmp28D;_LL262: _tmp28D=_tmp282.f1;_LL263:;{
void*_tmp283=_tmp28D;int _tmp28C;int _tmp28B;struct _dyneither_ptr _tmp28A;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp283)->tag == 6){_LL265: _tmp28C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp283)->f1).is_union;_tmp28B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp283)->f1).fieldnum;_tmp28A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp283)->f2;_LL266: {
# 1504
int _tmp284=Cyc_CfFlowInfo_get_field_index(_tmp291,_tmp36A);
if((_tmp28C  && _tmp28B != - 1) && _tmp28B != _tmp284)
return({struct _tuple16 _tmp285;_tmp285.f1=_tmp293;_tmp285.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),1,_tmp15D->unknown_none);_tmp285;});
return({struct _tuple16 _tmp286;_tmp286.f1=_tmp293;_tmp286.f2=*((void**)_check_dyneither_subscript(_tmp28A,sizeof(void*),_tmp284));_tmp286;});}}else{_LL267: _LL268:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287[0]=({struct Cyc_Core_Impossible_exn_struct _tmp288;_tmp288.tag=Cyc_Core_Impossible;_tmp288.f1=({const char*_tmp289="anal_Rexp: AggrArrow";_tag_dyneither(_tmp289,sizeof(char),21);});_tmp288;});_tmp287;}));}_LL264:;};};}else{_LL25F: _LL260:
# 1510
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_Core_Impossible_exn_struct _tmp28F;_tmp28F.tag=Cyc_Core_Impossible;_tmp28F.f1=({const char*_tmp290="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp290,sizeof(char),25);});_tmp28F;});_tmp28E;}));}_LL25C:;};};}case 5: _LL189: _tmp36E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp36D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_tmp36C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp175)->f3;_LL18A: {
# 1514
struct _tuple19 _tmp296=Cyc_NewControlFlow_anal_test(env,inflow,_tmp36E);struct _tuple19 _tmp297=_tmp296;union Cyc_CfFlowInfo_FlowInfo _tmp29B;union Cyc_CfFlowInfo_FlowInfo _tmp29A;_LL26A: _tmp29B=_tmp297.f1;_tmp29A=_tmp297.f2;_LL26B:;{
struct _tuple16 _tmp298=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp29B,_tmp36D);
struct _tuple16 _tmp299=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp29A,_tmp36C);
# 1518
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp15D,env->all_changed,_tmp298,_tmp299);};}case 6: _LL18B: _tmp370=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp36F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL18C: {
# 1521
struct _tuple19 _tmp29C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp370);struct _tuple19 _tmp29D=_tmp29C;union Cyc_CfFlowInfo_FlowInfo _tmp2A7;union Cyc_CfFlowInfo_FlowInfo _tmp2A6;_LL26D: _tmp2A7=_tmp29D.f1;_tmp2A6=_tmp29D.f2;_LL26E:;{
struct _tuple16 _tmp29E=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2A7,_tmp36F);struct _tuple16 _tmp29F=_tmp29E;union Cyc_CfFlowInfo_FlowInfo _tmp2A5;void*_tmp2A4;_LL270: _tmp2A5=_tmp29F.f1;_tmp2A4=_tmp29F.f2;_LL271:;{
struct _tuple16 _tmp2A0=({struct _tuple16 _tmp2A3;_tmp2A3.f1=_tmp2A5;_tmp2A3.f2=_tmp2A4;_tmp2A3;});
struct _tuple16 _tmp2A1=({struct _tuple16 _tmp2A2;_tmp2A2.f1=_tmp2A6;_tmp2A2.f2=_tmp15D->zero;_tmp2A2;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp15D,env->all_changed,_tmp2A0,_tmp2A1);};};}case 7: _LL18D: _tmp372=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp371=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL18E: {
# 1528
struct _tuple19 _tmp2A8=Cyc_NewControlFlow_anal_test(env,inflow,_tmp372);struct _tuple19 _tmp2A9=_tmp2A8;union Cyc_CfFlowInfo_FlowInfo _tmp2B3;union Cyc_CfFlowInfo_FlowInfo _tmp2B2;_LL273: _tmp2B3=_tmp2A9.f1;_tmp2B2=_tmp2A9.f2;_LL274:;{
struct _tuple16 _tmp2AA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2B2,_tmp371);struct _tuple16 _tmp2AB=_tmp2AA;union Cyc_CfFlowInfo_FlowInfo _tmp2B1;void*_tmp2B0;_LL276: _tmp2B1=_tmp2AB.f1;_tmp2B0=_tmp2AB.f2;_LL277:;{
struct _tuple16 _tmp2AC=({struct _tuple16 _tmp2AF;_tmp2AF.f1=_tmp2B1;_tmp2AF.f2=_tmp2B0;_tmp2AF;});
struct _tuple16 _tmp2AD=({struct _tuple16 _tmp2AE;_tmp2AE.f1=_tmp2B3;_tmp2AE.f2=_tmp15D->notzeroall;_tmp2AE;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp15D,env->all_changed,_tmp2AC,_tmp2AD);};};}case 22: _LL18F: _tmp374=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp373=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL190: {
# 1535
struct _RegionHandle _tmp2B4=_new_region("temp");struct _RegionHandle*temp=& _tmp2B4;_push_region(temp);
{struct _tuple23 _tmp2B5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp2DA[2];_tmp2DA[1]=_tmp373;_tmp2DA[0]=_tmp374;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp2DA,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);struct _tuple23 _tmp2B6=_tmp2B5;union Cyc_CfFlowInfo_FlowInfo _tmp2D9;struct Cyc_List_List*_tmp2D8;_LL279: _tmp2D9=_tmp2B6.f1;_tmp2D8=_tmp2B6.f2;_LL27A:;{
# 1540
union Cyc_CfFlowInfo_FlowInfo _tmp2B7=_tmp2D9;
{union Cyc_CfFlowInfo_FlowInfo _tmp2B8=_tmp2D9;struct Cyc_Dict_Dict _tmp2BD;struct Cyc_List_List*_tmp2BC;if((_tmp2B8.ReachableFL).tag == 2){_LL27C: _tmp2BD=((_tmp2B8.ReachableFL).val).f1;_tmp2BC=((_tmp2B8.ReachableFL).val).f2;_LL27D:
# 1545
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2BD,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2D8))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2B9=0;Cyc_CfFlowInfo_aerr(_tmp373->loc,({const char*_tmp2BA="expression may not be initialized";_tag_dyneither(_tmp2BA,sizeof(char),34);}),_tag_dyneither(_tmp2B9,sizeof(void*),0));});{
struct Cyc_List_List*_tmp2BB=Cyc_NewControlFlow_add_subscript_reln(_tmp15D->r,_tmp2BC,_tmp374,_tmp373);
if(_tmp2BC != _tmp2BB)
_tmp2B7=Cyc_CfFlowInfo_ReachableFL(_tmp2BD,_tmp2BB);
goto _LL27B;};}else{_LL27E: _LL27F:
 goto _LL27B;}_LL27B:;}{
# 1553
void*_tmp2BE=Cyc_Tcutil_compress((void*)_check_null(_tmp374->topt));void*_tmp2BF=_tmp2BE;union Cyc_Absyn_Constraint*_tmp2D7;struct Cyc_List_List*_tmp2D6;switch(*((int*)_tmp2BF)){case 10: _LL281: _tmp2D6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2BF)->f1;_LL282: {
# 1555
struct _tuple15 _tmp2C0=Cyc_CfFlowInfo_unname_rval(_tmp15D->r,(void*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd);struct _tuple15 _tmp2C1=_tmp2C0;void*_tmp2C9;_LL288: _tmp2C9=_tmp2C1.f1;_LL289:;{
void*_tmp2C2=_tmp2C9;struct _dyneither_ptr _tmp2C8;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C2)->tag == 6){_LL28B: _tmp2C8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C2)->f2;_LL28C: {
# 1558
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp373)).f1;
struct _tuple16 _tmp2C4=({struct _tuple16 _tmp2C3;_tmp2C3.f1=_tmp2B7;_tmp2C3.f2=*((void**)_check_dyneither_subscript(_tmp2C8,sizeof(void*),(int)i));_tmp2C3;});_npop_handler(0);return _tmp2C4;}}else{_LL28D: _LL28E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2C6;_tmp2C6.tag=Cyc_Core_Impossible;_tmp2C6.f1=({const char*_tmp2C7="anal_Rexp: Subscript";_tag_dyneither(_tmp2C7,sizeof(char),21);});_tmp2C6;});_tmp2C5;}));}_LL28A:;};}case 5: _LL283: _tmp2D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).bounds;_LL284: {
# 1563
struct _tuple16 _tmp2CA=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2D9,_tmp374,(void*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd);struct _tuple16 _tmp2CB=_tmp2CA;union Cyc_CfFlowInfo_FlowInfo _tmp2D2;void*_tmp2D1;_LL290: _tmp2D2=_tmp2CB.f1;_tmp2D1=_tmp2CB.f2;_LL291:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2CC=_tmp2D2;if((_tmp2CC.BottomFL).tag == 1){_LL293: _LL294: {
struct _tuple16 _tmp2CE=({struct _tuple16 _tmp2CD;_tmp2CD.f1=_tmp2D2;_tmp2CD.f2=_tmp2D1;_tmp2CD;});_npop_handler(0);return _tmp2CE;}}else{_LL295: _LL296: {
struct _tuple16 _tmp2D0=({struct _tuple16 _tmp2CF;_tmp2CF.f1=_tmp2B7;_tmp2CF.f2=_tmp2D1;_tmp2CF;});_npop_handler(0);return _tmp2D0;}}_LL292:;};}default: _LL285: _LL286:
# 1568
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2D4;_tmp2D4.tag=Cyc_Core_Impossible;_tmp2D4.f1=({const char*_tmp2D5="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp2D5,sizeof(char),33);});_tmp2D4;});_tmp2D3;}));}_LL280:;};};}
# 1536
;_pop_region(temp);}case 23: _LL193: _tmp375=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL194: {
# 1575
struct _RegionHandle _tmp2DB=_new_region("temp");struct _RegionHandle*temp=& _tmp2DB;_push_region(temp);
{struct _tuple23 _tmp2DC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp375,1,1);struct _tuple23 _tmp2DD=_tmp2DC;union Cyc_CfFlowInfo_FlowInfo _tmp2E9;struct Cyc_List_List*_tmp2E8;_LL298: _tmp2E9=_tmp2DD.f1;_tmp2E8=_tmp2DD.f2;_LL299:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp2E3=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp375);void**_tmp2E4=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2E3));struct _dyneither_ptr _tmp2E7=_tag_dyneither(_tmp2E4,sizeof(void*),_tmp2E3);{unsigned int _tmp2E5=_tmp2E3;unsigned int i;for(i=0;i < _tmp2E5;i ++){_tmp2E4[i]=(void*)({
void*_tmp2E6=(void*)((struct Cyc_List_List*)_check_null(_tmp2E8))->hd;
_tmp2E8=_tmp2E8->tl;
_tmp2E6;});}}_tmp2E7;});
# 1583
struct _tuple16 _tmp2E2=({struct _tuple16 _tmp2DE;_tmp2DE.f1=_tmp2E9;_tmp2DE.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DF=_region_malloc(env->r,sizeof(*_tmp2DF));_tmp2DF[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E0;_tmp2E0.tag=6;_tmp2E0.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E1;_tmp2E1.is_union=0;_tmp2E1.fieldnum=- 1;_tmp2E1;});_tmp2E0.f2=aggrdict;_tmp2E0;});_tmp2DF;});_tmp2DE;});_npop_handler(0);return _tmp2E2;};}
# 1576
;_pop_region(temp);}case 29: _LL195: _tmp376=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_LL196: {
# 1586
struct Cyc_List_List*fs;
{void*_tmp2EA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2EB=_tmp2EA;struct Cyc_List_List*_tmp2EF;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2EB)->tag == 12){_LL29B: _tmp2EF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2EB)->f2;_LL29C:
 fs=_tmp2EF;goto _LL29A;}else{_LL29D: _LL29E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2ED;_tmp2ED.tag=Cyc_Core_Impossible;_tmp2ED.f1=({const char*_tmp2EE="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp2EE,sizeof(char),35);});_tmp2ED;});_tmp2EC;}));}_LL29A:;}
# 1591
_tmp379=_tmp376;_tmp378=Cyc_Absyn_StructA;_tmp377=fs;goto _LL198;}case 28: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp175)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp175)->f4)->impl != 0){_LL197: _tmp379=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp175)->f3;_tmp378=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp175)->f4)->kind;_tmp377=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp175)->f4)->impl)->fields;_LL198: {
# 1593
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp2F0=_new_region("temp");struct _RegionHandle*temp=& _tmp2F0;_push_region(temp);
{struct _tuple23 _tmp2F1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp379),1,1);
# 1595
struct _tuple23 _tmp2F2=_tmp2F1;union Cyc_CfFlowInfo_FlowInfo _tmp301;struct Cyc_List_List*_tmp300;_LL2A0: _tmp301=_tmp2F2.f1;_tmp300=_tmp2F2.f2;_LL2A1:;{
# 1597
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp15D,_tmp377,_tmp378 == Cyc_Absyn_UnionA,_tmp15D->unknown_all);
# 1600
int field_no=-1;
{int i=0;for(0;_tmp300 != 0;(((_tmp300=_tmp300->tl,_tmp379=_tmp379->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp379))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2F3=(void*)ds->hd;void*_tmp2F4=_tmp2F3;struct _dyneither_ptr*_tmp2F9;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2F4)->tag == 0){_LL2A3: _LL2A4:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2F6;_tmp2F6.tag=Cyc_Core_Impossible;_tmp2F6.f1=({const char*_tmp2F7="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2F7,sizeof(char),22);});_tmp2F6;});_tmp2F5;}));}else{_LL2A5: _tmp2F9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2F4)->f1;_LL2A6:
# 1607
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp377,_tmp2F9);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp300->hd;
# 1610
if(_tmp378 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp2F8=(*((struct _tuple26*)_tmp379->hd)).f2;
_tmp301=Cyc_NewControlFlow_use_Rval(env,_tmp2F8->loc,_tmp301,(void*)_tmp300->hd);
# 1614
Cyc_NewControlFlow_check_union_requires(_tmp2F8->loc,_tmp15D->r,exp_type,_tmp2F9,_tmp301);}}_LL2A2:;}}}{
# 1617
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2FA=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2FD=_region_malloc(env->r,sizeof(*_tmp2FD));_tmp2FD[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2FE;_tmp2FE.tag=6;_tmp2FE.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2FF;_tmp2FF.is_union=_tmp378 == Cyc_Absyn_UnionA;_tmp2FF.fieldnum=field_no;_tmp2FF;});_tmp2FE.f2=aggrdict;_tmp2FE;});_tmp2FD;});
struct _tuple16 _tmp2FC=({struct _tuple16 _tmp2FB;_tmp2FB.f1=_tmp301;_tmp2FB.f2=(void*)_tmp2FA;_tmp2FB;});_npop_handler(0);return _tmp2FC;};};}
# 1595
;_pop_region(temp);}}else{goto _LL199;}}else{_LL199: _LL19A:
# 1621
({struct Cyc_String_pa_PrintArg_struct _tmp304;_tmp304.tag=0;_tmp304.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp302[1]={& _tmp304};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp303="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp303,sizeof(char),37);}),_tag_dyneither(_tmp302,sizeof(void*),1));});});}case 25: _LL19B: _tmp37A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL19C: {
# 1623
struct _RegionHandle _tmp305=_new_region("temp");struct _RegionHandle*temp=& _tmp305;_push_region(temp);
{struct Cyc_List_List*_tmp306=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp37A);
struct _tuple23 _tmp307=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp306,1,1);struct _tuple23 _tmp308=_tmp307;union Cyc_CfFlowInfo_FlowInfo _tmp30C;struct Cyc_List_List*_tmp30B;_LL2A8: _tmp30C=_tmp308.f1;_tmp30B=_tmp308.f2;_LL2A9:;
for(0;_tmp30B != 0;(_tmp30B=_tmp30B->tl,_tmp306=_tmp306->tl)){
_tmp30C=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp306))->hd)->loc,_tmp30C,(void*)_tmp30B->hd);}{
struct _tuple16 _tmp30A=({struct _tuple16 _tmp309;_tmp309.f1=_tmp30C;_tmp309.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,(void*)_check_null(e->topt),0,_tmp15D->unknown_all);_tmp309;});_npop_handler(0);return _tmp30A;};}
# 1624
;_pop_region(temp);}case 26: _LL19D: _tmp37E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp37D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_tmp37C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp175)->f3;_tmp37B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp175)->f4;_LL19E: {
# 1632
struct _tuple16 _tmp30D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp37D);struct _tuple16 _tmp30E=_tmp30D;union Cyc_CfFlowInfo_FlowInfo _tmp331;void*_tmp330;_LL2AB: _tmp331=_tmp30E.f1;_tmp330=_tmp30E.f2;_LL2AC:;{
union Cyc_CfFlowInfo_FlowInfo _tmp30F=_tmp331;struct Cyc_Dict_Dict _tmp32F;struct Cyc_List_List*_tmp32E;if((_tmp30F.BottomFL).tag == 1){_LL2AE: _LL2AF:
 return({struct _tuple16 _tmp310;_tmp310.f1=_tmp331;_tmp310.f2=_tmp15D->unknown_all;_tmp310;});}else{_LL2B0: _tmp32F=((_tmp30F.ReachableFL).val).f1;_tmp32E=((_tmp30F.ReachableFL).val).f2;_LL2B1:
# 1636
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp32F,_tmp330)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp311=0;Cyc_CfFlowInfo_aerr(_tmp37D->loc,({const char*_tmp312="expression may not be initialized";_tag_dyneither(_tmp312,sizeof(char),34);}),_tag_dyneither(_tmp311,sizeof(void*),0));});{
# 1640
struct Cyc_List_List*new_relns=_tmp32E;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp37E);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp37D,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp32E);
# 1646
if(_tmp32E != new_relns)
_tmp331=Cyc_CfFlowInfo_ReachableFL(_tmp32F,new_relns);{
# 1650
void*_tmp313=_tmp330;switch(*((int*)_tmp313)){case 0: _LL2B3: _LL2B4:
 return({struct _tuple16 _tmp314;_tmp314.f1=_tmp331;_tmp314.f2=_tmp15D->unknown_all;_tmp314;});case 2: _LL2B5: _LL2B6:
 goto _LL2B8;case 1: _LL2B7: _LL2B8:
 goto _LL2BA;case 5: _LL2B9: _LL2BA: {
# 1655
struct Cyc_List_List _tmp315=({struct Cyc_List_List _tmp320;_tmp320.hd=_tmp37E;_tmp320.tl=0;_tmp320;});
_tmp331=Cyc_NewControlFlow_add_vars(_tmp15D,_tmp331,& _tmp315,_tmp15D->unknown_all,e->loc,0);{
# 1658
struct _tuple16 _tmp316=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp331,_tmp37C);struct _tuple16 _tmp317=_tmp316;union Cyc_CfFlowInfo_FlowInfo _tmp31F;void*_tmp31E;_LL2BE: _tmp31F=_tmp317.f1;_tmp31E=_tmp317.f2;_LL2BF:;
{union Cyc_CfFlowInfo_FlowInfo _tmp318=_tmp31F;struct Cyc_Dict_Dict _tmp31D;if((_tmp318.BottomFL).tag == 1){_LL2C1: _LL2C2:
 return({struct _tuple16 _tmp319;_tmp319.f1=_tmp31F;_tmp319.f2=_tmp15D->unknown_all;_tmp319;});}else{_LL2C3: _tmp31D=((_tmp318.ReachableFL).val).f1;_LL2C4:
# 1662
 if(Cyc_CfFlowInfo_initlevel(_tmp15D,_tmp31D,_tmp31E)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp31A=0;Cyc_CfFlowInfo_aerr(_tmp37D->loc,({const char*_tmp31B="expression may not be initialized";_tag_dyneither(_tmp31B,sizeof(char),34);}),_tag_dyneither(_tmp31A,sizeof(void*),0));});
return({struct _tuple16 _tmp31C;_tmp31C.f1=Cyc_CfFlowInfo_BottomFL();_tmp31C.f2=_tmp15D->unknown_all;_tmp31C;});}}_LL2C0:;}
# 1667
_tmp331=_tmp31F;
goto _LL2BC;};}default: _LL2BB: _LL2BC:
# 1670
 while(1){
struct Cyc_List_List _tmp321=({struct Cyc_List_List _tmp32C;_tmp32C.hd=_tmp37E;_tmp32C.tl=0;_tmp32C;});
_tmp331=Cyc_NewControlFlow_add_vars(_tmp15D,_tmp331,& _tmp321,_tmp15D->unknown_all,e->loc,0);{
struct _tuple16 _tmp322=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp331,_tmp37C);struct _tuple16 _tmp323=_tmp322;union Cyc_CfFlowInfo_FlowInfo _tmp32B;void*_tmp32A;_LL2C6: _tmp32B=_tmp323.f1;_tmp32A=_tmp323.f2;_LL2C7:;
{union Cyc_CfFlowInfo_FlowInfo _tmp324=_tmp32B;struct Cyc_Dict_Dict _tmp328;if((_tmp324.BottomFL).tag == 1){_LL2C9: _LL2CA:
 goto _LL2C8;}else{_LL2CB: _tmp328=((_tmp324.ReachableFL).val).f1;_LL2CC:
# 1677
 if(Cyc_CfFlowInfo_initlevel(_tmp15D,_tmp328,_tmp32A)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp325=0;Cyc_CfFlowInfo_aerr(_tmp37D->loc,({const char*_tmp326="expression may not be initialized";_tag_dyneither(_tmp326,sizeof(char),34);}),_tag_dyneither(_tmp325,sizeof(void*),0));});
return({struct _tuple16 _tmp327;_tmp327.f1=Cyc_CfFlowInfo_BottomFL();_tmp327.f2=_tmp15D->unknown_all;_tmp327;});}}_LL2C8:;}{
# 1682
union Cyc_CfFlowInfo_FlowInfo _tmp329=Cyc_CfFlowInfo_join_flow(_tmp15D,env->all_changed,_tmp331,_tmp32B);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp329,_tmp331))
break;
_tmp331=_tmp329;};};}
# 1687
return({struct _tuple16 _tmp32D;_tmp32D.f1=_tmp331;_tmp32D.f2=_tmp15D->unknown_all;_tmp32D;});}_LL2B2:;};};}_LL2AD:;};}case 27: _LL19F: _tmp381=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_tmp380=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp175)->f2;_tmp37F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp175)->f3;_LL1A0: {
# 1693
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp33D=_region_malloc(_tmp15D->r,sizeof(*_tmp33D));_tmp33D[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp33E;_tmp33E.tag=1;_tmp33E.f1=_tmp381;_tmp33E.f2=(void*)_check_null(e->topt);_tmp33E;});_tmp33D;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp33C=_region_malloc(_tmp15D->r,sizeof(*_tmp33C));_tmp33C->root=root;_tmp33C->fields=0;_tmp33C;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp33A=_region_malloc(_tmp15D->r,sizeof(*_tmp33A));_tmp33A[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp33B;_tmp33B.tag=5;_tmp33B.f1=place;_tmp33B;});_tmp33A;});
void*place_val;
# 1701
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp15D,_tmp380,0,_tmp15D->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp332=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp381);struct _tuple16 _tmp333=_tmp332;union Cyc_CfFlowInfo_FlowInfo _tmp339;_LL2CE: _tmp339=_tmp333.f1;_LL2CF:;
outflow=_tmp339;{
union Cyc_CfFlowInfo_FlowInfo _tmp334=outflow;struct Cyc_Dict_Dict _tmp338;struct Cyc_List_List*_tmp337;if((_tmp334.BottomFL).tag == 1){_LL2D1: _LL2D2:
 return({struct _tuple16 _tmp335;_tmp335.f1=outflow;_tmp335.f2=rval;_tmp335;});}else{_LL2D3: _tmp338=((_tmp334.ReachableFL).val).f1;_tmp337=((_tmp334.ReachableFL).val).f2;_LL2D4:
# 1709
 return({struct _tuple16 _tmp336;_tmp336.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp338,root,place_val),_tmp337);_tmp336.f2=rval;_tmp336;});}_LL2D0:;};};};}case 36: _LL1A1: _tmp382=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp175)->f1;_LL1A2: {
# 1713
struct _tuple17 _tmp33F=({struct _tuple17 _tmp342;_tmp342.f1=(env->fenv)->unknown_all;_tmp342.f2=copy_ctxt;_tmp342;});
union Cyc_CfFlowInfo_FlowInfo _tmp340=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp382,& _tmp33F);
return({struct _tuple16 _tmp341;_tmp341.f1=_tmp340;_tmp341.f2=_tmp33F.f1;_tmp341;});}case 35: _LL1A5: _LL1A6:
# 1718
 goto _LL1A8;case 24: _LL1A7: _LL1A8:
 goto _LL1AA;case 38: _LL1A9: _LL1AA:
 goto _LL1AC;default: _LL1AB: _LL1AC:
# 1722
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343[0]=({struct Cyc_Core_Impossible_exn_struct _tmp344;_tmp344.tag=Cyc_Core_Impossible;_tmp344.f1=({const char*_tmp345="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp345,sizeof(char),31);});_tmp344;});_tmp343;}));}_LL144:;};}
# 1732
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place,struct Cyc_List_List*flds){
# 1741
struct Cyc_CfFlowInfo_FlowEnv*_tmp383=env->fenv;
void*_tmp384=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp385=_tmp384;void*_tmp3B6;union Cyc_Absyn_Constraint*_tmp3B5;union Cyc_Absyn_Constraint*_tmp3B4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp385)->tag == 5){_LL2D6: _tmp3B6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp385)->f1).elt_typ;_tmp3B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp385)->f1).ptr_atts).bounds;_tmp3B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp385)->f1).ptr_atts).zero_term;_LL2D7: {
# 1744
union Cyc_CfFlowInfo_FlowInfo _tmp386=f;struct Cyc_Dict_Dict _tmp3B0;struct Cyc_List_List*_tmp3AF;if((_tmp386.BottomFL).tag == 1){_LL2DB: _LL2DC:
 return({struct _tuple18 _tmp387;_tmp387.f1=f;_tmp387.f2=Cyc_CfFlowInfo_UnknownL();_tmp387;});}else{_LL2DD: _tmp3B0=((_tmp386.ReachableFL).val).f1;_tmp3AF=((_tmp386.ReachableFL).val).f2;_LL2DE:
# 1748
 if(Cyc_Tcutil_is_bound_one(_tmp3B5)){
struct _tuple15 _tmp388=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmp389=_tmp388;void*_tmp3A3;struct Cyc_List_List*_tmp3A2;_LL2E0: _tmp3A3=_tmp389.f1;_tmp3A2=_tmp389.f2;_LL2E1:;
retry: {void*_tmp38A=_tmp3A3;enum Cyc_CfFlowInfo_InitLevel _tmp3A1;void*_tmp3A0;struct Cyc_List_List*_tmp39F;switch(*((int*)_tmp38A)){case 8: _LL2E3: _LL2E4:
# 1752
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B[0]=({struct Cyc_Core_Impossible_exn_struct _tmp38C;_tmp38C.tag=Cyc_Core_Impossible;_tmp38C.f1=({const char*_tmp38D="named location in anal_derefL";_tag_dyneither(_tmp38D,sizeof(char),30);});_tmp38C;});_tmp38B;}));case 1: _LL2E5: _LL2E6:
 goto _LL2E8;case 2: _LL2E7: _LL2E8:
# 1755
 e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp38F;_tmp38F.tag=Cyc_CfFlowInfo_NotZero;_tmp38F.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3AF);_tmp38F;});_tmp38E;});goto _LL2E2;case 5: _LL2E9: _tmp3A0=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38A)->f1)->root;_tmp39F=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38A)->f1)->fields;_LL2EA:
# 1758
 if(expanded_place)
# 1761
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp391;_tmp391.tag=Cyc_CfFlowInfo_UnknownZ;_tmp391.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3AF);_tmp391;});_tmp390;});else{
# 1764
void*_tmp392=e->annot;void*_tmp393=_tmp392;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp393)->tag == Cyc_CfFlowInfo_UnknownZ){_LL2F2: _LL2F3:
# 1768
 e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp395;_tmp395.tag=Cyc_CfFlowInfo_UnknownZ;_tmp395.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3AF);_tmp395;});_tmp394;});
goto _LL2F1;}else{_LL2F4: _LL2F5:
# 1771
 e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp397;_tmp397.tag=Cyc_CfFlowInfo_NotZero;_tmp397.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3AF);_tmp397;});_tmp396;});
goto _LL2F1;}_LL2F1:;}
# 1775
return({struct _tuple18 _tmp398;_tmp398.f1=f;_tmp398.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp399=_region_malloc(_tmp383->r,sizeof(*_tmp399));_tmp399->root=_tmp3A0;_tmp399->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp383->r,_tmp39F,flds);_tmp399;}));_tmp398;});case 0: _LL2EB: _LL2EC:
# 1777
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp39A;_tmp39A.f1=Cyc_CfFlowInfo_BottomFL();_tmp39A.f2=Cyc_CfFlowInfo_UnknownL();_tmp39A;});case 3: _LL2ED: _tmp3A1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp38A)->f1;_LL2EE:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3A1,_tmp3A2);goto _LL2F0;default: _LL2EF: _LL2F0:
# 1782
 if(passthrough_consumes){
void*_tmp39B=_tmp3A3;void*_tmp39C;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp39B)->tag == 7){_LL2F7: _tmp39C=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp39B)->f3;_LL2F8:
 _tmp3A3=_tmp39C;goto retry;}else{_LL2F9: _LL2FA:
 goto _LL2F6;}_LL2F6:;}
# 1788
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp39E;_tmp39E.tag=Cyc_CfFlowInfo_UnknownZ;_tmp39E.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3AF);_tmp39E;});_tmp39D;});}_LL2E2:;}}else{
# 1792
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3A5;_tmp3A5.tag=Cyc_CfFlowInfo_UnknownZ;_tmp3A5.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3AF);_tmp3A5;});_tmp3A4;});}
if(Cyc_CfFlowInfo_initlevel(_tmp383,_tmp3B0,r)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp3A6=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmp3A7=_tmp3A6;void*_tmp3AD;_LL2FC: _tmp3AD=_tmp3A7.f1;_LL2FD:;{
void*_tmp3A8=_tmp3AD;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A8)->tag == 7){_LL2FF: _LL300:
# 1797
({void*_tmp3A9=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp3AA="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp3AA,sizeof(char),66);}),_tag_dyneither(_tmp3A9,sizeof(void*),0));});
goto _LL2FE;}else{_LL301: _LL302:
# 1800
({void*_tmp3AB=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp3AC="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp3AC,sizeof(char),46);}),_tag_dyneither(_tmp3AB,sizeof(void*),0));});
goto _LL2FE;}_LL2FE:;};}
# 1804
return({struct _tuple18 _tmp3AE;_tmp3AE.f1=f;_tmp3AE.f2=Cyc_CfFlowInfo_UnknownL();_tmp3AE;});}_LL2DA:;}}else{_LL2D8: _LL2D9:
# 1806
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3B2;_tmp3B2.tag=Cyc_Core_Impossible;_tmp3B2.f1=({const char*_tmp3B3="left deref of non-pointer-type";_tag_dyneither(_tmp3B3,sizeof(char),31);});_tmp3B2;});_tmp3B1;}));}_LL2D5:;}
# 1814
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1817
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3B7=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3B8=inflow;struct Cyc_Dict_Dict _tmp3BB;struct Cyc_List_List*_tmp3BA;if((_tmp3B8.BottomFL).tag == 1){_LL304: _LL305:
 return({struct _tuple18 _tmp3B9;_tmp3B9.f1=Cyc_CfFlowInfo_BottomFL();_tmp3B9.f2=Cyc_CfFlowInfo_UnknownL();_tmp3B9;});}else{_LL306: _tmp3BB=((_tmp3B8.ReachableFL).val).f1;_tmp3BA=((_tmp3B8.ReachableFL).val).f2;_LL307:
# 1822
 d=_tmp3BB;}_LL303:;}{
# 1826
void*_tmp3BC=e->r;void*_tmp3BD=_tmp3BC;struct Cyc_Absyn_Exp*_tmp41A;struct _dyneither_ptr*_tmp419;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*_tmp417;struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp415;struct _dyneither_ptr*_tmp414;struct Cyc_Absyn_Vardecl*_tmp413;struct Cyc_Absyn_Vardecl*_tmp412;struct Cyc_Absyn_Vardecl*_tmp411;struct Cyc_Absyn_Vardecl*_tmp410;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Exp*_tmp40D;switch(*((int*)_tmp3BD)){case 13: _LL309: _tmp40D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD)->f2;_LL30A:
 _tmp40E=_tmp40D;goto _LL30C;case 11: _LL30B: _tmp40E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL30C:
 _tmp40F=_tmp40E;goto _LL30E;case 12: _LL30D: _tmp40F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL30E:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp40F,flds);case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1)){case 3: _LL30F: _tmp410=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1)->f1;_LL310:
# 1831
 _tmp411=_tmp410;goto _LL312;case 4: _LL311: _tmp411=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1)->f1;_LL312:
 _tmp412=_tmp411;goto _LL314;case 5: _LL313: _tmp412=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1)->f1;_LL314:
# 1834
 return({struct _tuple18 _tmp3BE;_tmp3BE.f1=inflow;_tmp3BE.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3BF=_region_malloc(env->r,sizeof(*_tmp3BF));_tmp3BF->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3C0=_region_malloc(env->r,sizeof(*_tmp3C0));_tmp3C0[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3C1;_tmp3C1.tag=0;_tmp3C1.f1=_tmp412;_tmp3C1;});_tmp3C0;});_tmp3BF->fields=flds;_tmp3BF;}));_tmp3BE;});case 1: _LL315: _tmp413=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1)->f1;_LL316:
# 1836
 return({struct _tuple18 _tmp3C2;_tmp3C2.f1=inflow;_tmp3C2.f2=Cyc_CfFlowInfo_UnknownL();_tmp3C2;});default: goto _LL31F;}case 21: _LL317: _tmp415=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_tmp414=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3BD)->f2;_LL318:
# 1839
{void*_tmp3C3=Cyc_Tcutil_compress((void*)_check_null(_tmp415->topt));void*_tmp3C4=_tmp3C3;void*_tmp3C9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->tag == 5){_LL322: _tmp3C9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C4)->f1).elt_typ;_LL323:
# 1841
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3C9)){
Cyc_NewControlFlow_check_union_requires(_tmp415->loc,_tmp3B7->r,_tmp3C9,_tmp414,inflow);
flds=({struct Cyc_List_List*_tmp3C5=_region_malloc(env->r,sizeof(*_tmp3C5));_tmp3C5->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3C9,_tmp414);_tmp3C5->tl=flds;_tmp3C5;});}
# 1845
goto _LL321;}else{_LL324: _LL325:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3C7;_tmp3C7.tag=Cyc_Core_Impossible;_tmp3C7.f1=({const char*_tmp3C8="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3C8,sizeof(char),25);});_tmp3C7;});_tmp3C6;}));}_LL321:;}
# 1848
_tmp416=_tmp415;goto _LL31A;case 19: _LL319: _tmp416=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL31A:
# 1851
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp416->topt),1)){
# 1853
struct _tuple18 _tmp3CA=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp416->topt),1),passthrough_consumes,_tmp416);
# 1853
struct _tuple18 _tmp3CB=_tmp3CA;union Cyc_CfFlowInfo_FlowInfo _tmp3E6;union Cyc_CfFlowInfo_AbsLVal _tmp3E5;_LL327: _tmp3E6=_tmp3CB.f1;_tmp3E5=_tmp3CB.f2;_LL328:;{
# 1857
struct _tuple18 _tmp3CC=({struct _tuple18 _tmp3E4;_tmp3E4.f1=_tmp3E6;_tmp3E4.f2=_tmp3E5;_tmp3E4;});struct _tuple18 _tmp3CD=_tmp3CC;struct Cyc_Dict_Dict _tmp3E3;struct Cyc_List_List*_tmp3E2;struct Cyc_CfFlowInfo_Place*_tmp3E1;if(((_tmp3CD.f1).ReachableFL).tag == 2){if(((_tmp3CD.f2).PlaceL).tag == 1){_LL32A: _tmp3E3=(((_tmp3CD.f1).ReachableFL).val).f1;_tmp3E2=(((_tmp3CD.f1).ReachableFL).val).f2;_tmp3E1=((_tmp3CD.f2).PlaceL).val;_LL32B: {
# 1859
void*_tmp3CE=Cyc_CfFlowInfo_lookup_place(_tmp3E3,_tmp3E1);
struct _tuple15 _tmp3CF=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3CE);struct _tuple15 _tmp3D0=_tmp3CF;void*_tmp3E0;struct Cyc_List_List*_tmp3DF;_LL32F: _tmp3E0=_tmp3D0.f1;_tmp3DF=_tmp3D0.f2;_LL330:;{
void*_tmp3D1=_tmp3E0;struct Cyc_Absyn_Vardecl*_tmp3DE;void*_tmp3DD;switch(*((int*)_tmp3D1)){case 8: _LL332: _tmp3DE=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3D1)->f1;_tmp3DD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3D1)->f2;_LL333:
# 1863
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3D3;_tmp3D3.tag=Cyc_Core_Impossible;_tmp3D3.f1=({const char*_tmp3D4="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3D4,sizeof(char),38);});_tmp3D3;});_tmp3D2;}));case 7: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3D1)->f3)->tag == 5){_LL334: _LL335:
 goto _LL337;}else{goto _LL338;}case 5: _LL336: _LL337:
# 1868
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3E6,_tmp416,_tmp3E6,_tmp3CE,passthrough_consumes,0,flds);default: _LL338: _LL339: {
# 1875
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3B7,_tmp3E3,_tmp3E0);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3B7->unknown_all: _tmp3B7->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3B7,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp416->topt)),0,leaf);
void*new_root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3DB=_region_malloc(_tmp3B7->r,sizeof(*_tmp3DB));_tmp3DB[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp3DC;_tmp3DC.tag=1;_tmp3DC.f1=e;_tmp3DC.f2=(void*)_check_null(e->topt);_tmp3DC;});_tmp3DB;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp3DA=_region_malloc(_tmp3B7->r,sizeof(*_tmp3DA));_tmp3DA->root=new_root;_tmp3DA->fields=0;_tmp3DA;});
void*res=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3D8=_region_malloc(_tmp3B7->r,sizeof(*_tmp3D8));_tmp3D8[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp3D9;_tmp3D9.tag=5;_tmp3D9.f1=place;_tmp3D9;});_tmp3D8;});
for(0;_tmp3DF != 0;_tmp3DF=_tmp3DF->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3D5=_region_malloc(_tmp3B7->r,sizeof(*_tmp3D5));_tmp3D5[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3D6;_tmp3D6.tag=8;_tmp3D6.f1=(struct Cyc_Absyn_Vardecl*)_tmp3DF->hd;_tmp3D6.f2=res;_tmp3D6;});_tmp3D5;});}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3E3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E3,new_root,new_rval);
_tmp3E3=Cyc_CfFlowInfo_assign_place(_tmp3B7,e->loc,_tmp3E3,env->all_changed,_tmp3E1,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3D7=Cyc_CfFlowInfo_ReachableFL(_tmp3E3,_tmp3E2);
# 1893
return Cyc_NewControlFlow_anal_derefL(env,_tmp3D7,_tmp416,_tmp3D7,res,passthrough_consumes,1,flds);};}}_LL331:;};}}else{goto _LL32C;}}else{_LL32C: _LL32D:
# 1896
 goto _LL329;}_LL329:;};}{
# 1899
struct _tuple16 _tmp3E7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp416);struct _tuple16 _tmp3E8=_tmp3E7;union Cyc_CfFlowInfo_FlowInfo _tmp3EA;void*_tmp3E9;_LL33B: _tmp3EA=_tmp3E8.f1;_tmp3E9=_tmp3E8.f2;_LL33C:;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp416,_tmp3EA,_tmp3E9,passthrough_consumes,0,flds);};case 22: _LL31B: _tmp418=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_tmp417=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3BD)->f2;_LL31C: {
# 1903
void*_tmp3EB=Cyc_Tcutil_compress((void*)_check_null(_tmp418->topt));void*_tmp3EC=_tmp3EB;union Cyc_Absyn_Constraint*_tmp408;switch(*((int*)_tmp3EC)){case 10: _LL33E: _LL33F: {
# 1905
unsigned int _tmp3ED=(Cyc_Evexp_eval_const_uint_exp(_tmp417)).f1;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp418,({struct Cyc_List_List*_tmp3EE=_region_malloc(env->r,sizeof(*_tmp3EE));_tmp3EE->hd=(void*)_tmp3ED;_tmp3EE->tl=flds;_tmp3EE;}));}case 5: _LL340: _tmp408=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EC)->f1).ptr_atts).bounds;_LL341: {
# 1908
struct _RegionHandle _tmp3EF=_new_region("temp");struct _RegionHandle*temp=& _tmp3EF;_push_region(temp);
{struct _tuple23 _tmp3F0=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp404[2];_tmp404[1]=_tmp417;_tmp404[0]=_tmp418;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp404,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);struct _tuple23 _tmp3F1=_tmp3F0;union Cyc_CfFlowInfo_FlowInfo _tmp403;struct Cyc_List_List*_tmp402;_LL345: _tmp403=_tmp3F1.f1;_tmp402=_tmp3F1.f2;_LL346:;{
# 1911
union Cyc_CfFlowInfo_FlowInfo _tmp3F2=_tmp403;
{union Cyc_CfFlowInfo_FlowInfo _tmp3F3=_tmp403;struct Cyc_Dict_Dict _tmp3F8;struct Cyc_List_List*_tmp3F7;if((_tmp3F3.ReachableFL).tag == 2){_LL348: _tmp3F8=((_tmp3F3.ReachableFL).val).f1;_tmp3F7=((_tmp3F3.ReachableFL).val).f2;_LL349:
# 1914
 if(Cyc_CfFlowInfo_initlevel(_tmp3B7,_tmp3F8,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp402))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp3F4=0;Cyc_CfFlowInfo_aerr(_tmp417->loc,({const char*_tmp3F5="expression may not be initialized";_tag_dyneither(_tmp3F5,sizeof(char),34);}),_tag_dyneither(_tmp3F4,sizeof(void*),0));});{
struct Cyc_List_List*_tmp3F6=Cyc_NewControlFlow_add_subscript_reln(_tmp3B7->r,_tmp3F7,_tmp418,_tmp417);
if(_tmp3F7 != _tmp3F6)
_tmp3F2=Cyc_CfFlowInfo_ReachableFL(_tmp3F8,_tmp3F6);
goto _LL347;};}else{_LL34A: _LL34B:
 goto _LL347;}_LL347:;}{
# 1922
struct _tuple18 _tmp3F9=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp418,_tmp403,(void*)((struct Cyc_List_List*)_check_null(_tmp402))->hd,passthrough_consumes,0,flds);struct _tuple18 _tmp3FA=_tmp3F9;union Cyc_CfFlowInfo_FlowInfo _tmp401;union Cyc_CfFlowInfo_AbsLVal _tmp400;_LL34D: _tmp401=_tmp3FA.f1;_tmp400=_tmp3FA.f2;_LL34E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3FB=_tmp401;if((_tmp3FB.BottomFL).tag == 1){_LL350: _LL351: {
struct _tuple18 _tmp3FD=({struct _tuple18 _tmp3FC;_tmp3FC.f1=_tmp401;_tmp3FC.f2=_tmp400;_tmp3FC;});_npop_handler(0);return _tmp3FD;}}else{_LL352: _LL353: {
struct _tuple18 _tmp3FF=({struct _tuple18 _tmp3FE;_tmp3FE.f1=_tmp3F2;_tmp3FE.f2=_tmp400;_tmp3FE;});_npop_handler(0);return _tmp3FF;}}_LL34F:;};};};}
# 1909
;_pop_region(temp);}default: _LL342: _LL343:
# 1928
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405[0]=({struct Cyc_Core_Impossible_exn_struct _tmp406;_tmp406.tag=Cyc_Core_Impossible;_tmp406.f1=({const char*_tmp407="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp407,sizeof(char),33);});_tmp406;});_tmp405;}));}_LL33D:;}case 20: _LL31D: _tmp41A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_tmp419=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3BD)->f2;_LL31E: {
# 1932
void*_tmp409=(void*)_check_null(_tmp41A->topt);
if(Cyc_Absyn_is_require_union_type(_tmp409))
Cyc_NewControlFlow_check_union_requires(_tmp41A->loc,_tmp3B7->r,_tmp409,_tmp419,inflow);
# 1936
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp409))
return({struct _tuple18 _tmp40A;_tmp40A.f1=inflow;_tmp40A.f2=Cyc_CfFlowInfo_UnknownL();_tmp40A;});
# 1939
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp41A,({struct Cyc_List_List*_tmp40B=_region_malloc(env->r,sizeof(*_tmp40B));_tmp40B->hd=(void*)
Cyc_CfFlowInfo_get_field_index(_tmp409,_tmp419);_tmp40B->tl=flds;_tmp40B;}));}default: _LL31F: _LL320:
# 1942
 return({struct _tuple18 _tmp40C;_tmp40C.f1=Cyc_CfFlowInfo_BottomFL();_tmp40C.f2=Cyc_CfFlowInfo_UnknownL();_tmp40C;});}_LL308:;};}
# 1946
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1949
struct _tuple18 _tmp41B=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp41C=_tmp41B;union Cyc_CfFlowInfo_FlowInfo _tmp41F;union Cyc_CfFlowInfo_AbsLVal _tmp41E;_LL355: _tmp41F=_tmp41C.f1;_tmp41E=_tmp41C.f2;_LL356:;
return({struct _tuple18 _tmp41D;_tmp41D.f1=_tmp41F;_tmp41D.f2=_tmp41E;_tmp41D;});}
# 1956
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1959
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp420=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp421=_tmp420;union Cyc_CfFlowInfo_FlowInfo _tmp422;_LL358: _tmp422=_tmp421.f1;_LL359:;
inflow=_tmp422;}}}
# 1967
return inflow;}
# 1973
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1976
struct Cyc_CfFlowInfo_FlowEnv*_tmp423=env->fenv;
# 1979
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp424=_new_region("temp");struct _RegionHandle*temp=& _tmp424;_push_region(temp);{
struct _tuple23 _tmp425=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);struct _tuple23 _tmp426=_tmp425;union Cyc_CfFlowInfo_FlowInfo _tmp428;struct Cyc_List_List*_tmp427;_LL35B: _tmp428=_tmp426.f1;_tmp427=_tmp426.f2;_LL35C:;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp427))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp427->tl))->hd;
f=_tmp428;}{
# 1989
union Cyc_CfFlowInfo_FlowInfo _tmp429=f;struct Cyc_Dict_Dict _tmp473;struct Cyc_List_List*_tmp472;if((_tmp429.BottomFL).tag == 1){_LL35E: _LL35F: {
struct _tuple19 _tmp42B=({struct _tuple19 _tmp42A;_tmp42A.f1=f;_tmp42A.f2=f;_tmp42A;});_npop_handler(0);return _tmp42B;}}else{_LL360: _tmp473=((_tmp429.ReachableFL).val).f1;_tmp472=((_tmp429.ReachableFL).val).f2;_LL361: {
# 1992
struct Cyc_Absyn_Exp*_tmp42C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp42D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp473,r1)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp42E=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,({const char*_tmp42F="expression may not be initialized";_tag_dyneither(_tmp42F,sizeof(char),34);}),_tag_dyneither(_tmp42E,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp473,r2)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp430=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,({const char*_tmp431="expression may not be initialized";_tag_dyneither(_tmp431,sizeof(char),34);}),_tag_dyneither(_tmp430,sizeof(void*),0));});{
# 1999
union Cyc_CfFlowInfo_FlowInfo _tmp432=f;
union Cyc_CfFlowInfo_FlowInfo _tmp433=f;
# 2004
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp434=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);struct _tuple15 _tmp435=_tmp434;void*_tmp450;struct Cyc_List_List*_tmp44F;_LL363: _tmp450=_tmp435.f1;_tmp44F=_tmp435.f2;_LL364:;{
struct _tuple15 _tmp436=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);struct _tuple15 _tmp437=_tmp436;void*_tmp44E;struct Cyc_List_List*_tmp44D;_LL366: _tmp44E=_tmp437.f1;_tmp44D=_tmp437.f2;_LL367:;{
struct _tuple0 _tmp438=({struct _tuple0 _tmp44C;_tmp44C.f1=_tmp450;_tmp44C.f2=_tmp44E;_tmp44C;});struct _tuple0 _tmp439=_tmp438;enum Cyc_CfFlowInfo_InitLevel _tmp44B;enum Cyc_CfFlowInfo_InitLevel _tmp44A;switch(*((int*)_tmp439.f1)){case 3: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2)->tag == 0){_LL369: _tmp44A=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp439.f1)->f1;_LL36A: {
# 2011
struct _tuple19 _tmp43A=Cyc_NewControlFlow_splitzero(env,f,f,_tmp42C,_tmp44A,_tmp44F);struct _tuple19 _tmp43B=_tmp43A;union Cyc_CfFlowInfo_FlowInfo _tmp441;union Cyc_CfFlowInfo_FlowInfo _tmp440;_LL37E: _tmp441=_tmp43B.f1;_tmp440=_tmp43B.f2;_LL37F:;
{enum Cyc_Absyn_Primop _tmp43C=p;switch(_tmp43C){case Cyc_Absyn_Eq: _LL381: _LL382:
 _tmp432=_tmp440;_tmp433=_tmp441;goto _LL380;case Cyc_Absyn_Neq: _LL383: _LL384:
 _tmp432=_tmp441;_tmp433=_tmp440;goto _LL380;default: _LL385: _LL386:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D[0]=({struct Cyc_Core_Impossible_exn_struct _tmp43E;_tmp43E.tag=Cyc_Core_Impossible;_tmp43E.f1=({const char*_tmp43F="anal_test, zero-split";_tag_dyneither(_tmp43F,sizeof(char),22);});_tmp43E;});_tmp43D;}));}_LL380:;}
# 2017
goto _LL368;}}else{goto _LL37B;}case 0: switch(*((int*)_tmp439.f2)){case 3: _LL36B: _tmp44B=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp439.f2)->f1;_LL36C: {
# 2019
struct _tuple19 _tmp442=Cyc_NewControlFlow_splitzero(env,f,f,_tmp42D,_tmp44B,_tmp44D);struct _tuple19 _tmp443=_tmp442;union Cyc_CfFlowInfo_FlowInfo _tmp449;union Cyc_CfFlowInfo_FlowInfo _tmp448;_LL388: _tmp449=_tmp443.f1;_tmp448=_tmp443.f2;_LL389:;
{enum Cyc_Absyn_Primop _tmp444=p;switch(_tmp444){case Cyc_Absyn_Eq: _LL38B: _LL38C:
 _tmp432=_tmp448;_tmp433=_tmp449;goto _LL38A;case Cyc_Absyn_Neq: _LL38D: _LL38E:
 _tmp432=_tmp449;_tmp433=_tmp448;goto _LL38A;default: _LL38F: _LL390:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_Core_Impossible_exn_struct _tmp446;_tmp446.tag=Cyc_Core_Impossible;_tmp446.f1=({const char*_tmp447="anal_test, zero-split";_tag_dyneither(_tmp447,sizeof(char),22);});_tmp446;});_tmp445;}));}_LL38A:;}
# 2025
goto _LL368;}case 0: _LL36D: _LL36E:
# 2027
 if(p == Cyc_Absyn_Eq)_tmp433=Cyc_CfFlowInfo_BottomFL();else{
_tmp432=Cyc_CfFlowInfo_BottomFL();}
goto _LL368;case 1: _LL36F: _LL370:
 goto _LL372;case 2: _LL371: _LL372:
 goto _LL374;case 5: _LL373: _LL374:
 goto _LL376;default: goto _LL37B;}case 1: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2)->tag == 0){_LL375: _LL376:
 goto _LL378;}else{goto _LL37B;}case 2: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2)->tag == 0){_LL377: _LL378:
 goto _LL37A;}else{goto _LL37B;}case 5: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp439.f2)->tag == 0){_LL379: _LL37A:
# 2036
 if(p == Cyc_Absyn_Neq)_tmp433=Cyc_CfFlowInfo_BottomFL();else{
_tmp432=Cyc_CfFlowInfo_BottomFL();}
goto _LL368;}else{goto _LL37B;}default: _LL37B: _LL37C:
 goto _LL368;}_LL368:;};};}
# 2047
{struct _tuple0 _tmp451=({struct _tuple0 _tmp455;_tmp455.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp42C->topt));_tmp455.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp42D->topt));_tmp455;});struct _tuple0 _tmp452=_tmp451;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452.f1)->f1 == Cyc_Absyn_Unsigned){_LL392: _LL393:
 goto _LL395;}else{switch(*((int*)_tmp452.f2)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL394;else{goto _LL39A;}case 19: goto _LL398;default: goto _LL39A;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452.f2)->f1 == Cyc_Absyn_Unsigned){_LL394: _LL395:
 goto _LL397;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp452.f1)->tag == 19)goto _LL396;else{goto _LL39A;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp452.f1)->tag == 19){_LL396: _LL397:
 goto _LL399;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp452.f2)->tag == 19){_LL398: _LL399:
 goto _LL391;}else{_LL39A: _LL39B: {
struct _tuple19 _tmp454=({struct _tuple19 _tmp453;_tmp453.f1=_tmp432;_tmp453.f2=_tmp433;_tmp453;});_npop_handler(0);return _tmp454;}}}}}_LL391:;}
# 2056
{enum Cyc_Absyn_Primop _tmp456=p;switch(_tmp456){case Cyc_Absyn_Eq: _LL39D: _LL39E:
 goto _LL3A0;case Cyc_Absyn_Neq: _LL39F: _LL3A0: goto _LL3A2;case Cyc_Absyn_Gt: _LL3A1: _LL3A2: goto _LL3A4;case Cyc_Absyn_Gte: _LL3A3: _LL3A4: goto _LL3A6;case Cyc_Absyn_Lt: _LL3A5: _LL3A6: goto _LL3A8;case Cyc_Absyn_Lte: _LL3A7: _LL3A8: goto _LL39C;default: _LL3A9: _LL3AA: {
struct _tuple19 _tmp458=({struct _tuple19 _tmp457;_tmp457.f1=_tmp432;_tmp457.f2=_tmp433;_tmp457;});_npop_handler(0);return _tmp458;}}_LL39C:;}{
# 2061
struct _RegionHandle*_tmp459=(env->fenv)->r;
struct _tuple13 _tmp45A=Cyc_Relations_primop2relation(_tmp42C,p,_tmp42D);struct _tuple13 _tmp45B=_tmp45A;struct Cyc_Absyn_Exp*_tmp471;enum Cyc_Relations_Relation _tmp470;struct Cyc_Absyn_Exp*_tmp46F;_LL3AC: _tmp471=_tmp45B.f1;_tmp470=_tmp45B.f2;_tmp46F=_tmp45B.f3;_LL3AD:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2066
if(Cyc_Relations_exp2relnop(_tmp471,& n1) && Cyc_Relations_exp2relnop(_tmp46F,& n2)){
# 2068
struct Cyc_List_List*_tmp45C=Cyc_Relations_add_relation(_tmp459,n1,_tmp470,n2,_tmp472);
# 2072
struct Cyc_List_List*_tmp45D=Cyc_Relations_add_relation(_tmp459,n2,Cyc_Relations_flip_relation(_tmp470),n1,_tmp472);
struct _tuple19 _tmp45E=({struct _tuple19 _tmp46C;_tmp46C.f1=_tmp432;_tmp46C.f2=_tmp433;_tmp46C;});struct _tuple19 _tmp45F=_tmp45E;struct Cyc_Dict_Dict _tmp46B;struct Cyc_Dict_Dict _tmp46A;struct Cyc_Dict_Dict _tmp469;struct Cyc_Dict_Dict _tmp468;if(((_tmp45F.f1).ReachableFL).tag == 2){if(((_tmp45F.f2).ReachableFL).tag == 2){_LL3AF: _tmp469=(((_tmp45F.f1).ReachableFL).val).f1;_tmp468=(((_tmp45F.f2).ReachableFL).val).f1;_LL3B0: {
# 2075
struct _tuple19 _tmp461=({struct _tuple19 _tmp460;_tmp460.f1=Cyc_CfFlowInfo_ReachableFL(_tmp469,_tmp45C);_tmp460.f2=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp45D);_tmp460;});_npop_handler(0);return _tmp461;}}else{_LL3B3: _tmp46A=(((_tmp45F.f1).ReachableFL).val).f1;_LL3B4: {
# 2079
struct _tuple19 _tmp465=({struct _tuple19 _tmp464;_tmp464.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46A,_tmp45C);_tmp464.f2=_tmp433;_tmp464;});_npop_handler(0);return _tmp465;}}}else{if(((_tmp45F.f2).ReachableFL).tag == 2){_LL3B1: _tmp46B=(((_tmp45F.f2).ReachableFL).val).f1;_LL3B2: {
# 2077
struct _tuple19 _tmp463=({struct _tuple19 _tmp462;_tmp462.f1=_tmp432;_tmp462.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46B,_tmp45D);_tmp462;});_npop_handler(0);return _tmp463;}}else{_LL3B5: _LL3B6: {
# 2081
struct _tuple19 _tmp467=({struct _tuple19 _tmp466;_tmp466.f1=_tmp432;_tmp466.f2=_tmp433;_tmp466;});_npop_handler(0);return _tmp467;}}}_LL3AE:;}else{
# 2084
struct _tuple19 _tmp46E=({struct _tuple19 _tmp46D;_tmp46D.f1=_tmp432;_tmp46D.f2=_tmp433;_tmp46D;});_npop_handler(0);return _tmp46E;}};};};}}_LL35D:;};
# 1981
;_pop_region(temp);}
# 2090
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2092
struct Cyc_CfFlowInfo_FlowEnv*_tmp474=env->fenv;
void*_tmp475=e->r;void*_tmp476=_tmp475;enum Cyc_Absyn_Primop _tmp4C5;struct Cyc_List_List*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BA;switch(*((int*)_tmp476)){case 5: _LL3B8: _tmp4BC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp476)->f1;_tmp4BB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp476)->f2;_tmp4BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp476)->f3;_LL3B9: {
# 2095
struct _tuple19 _tmp477=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4BC);struct _tuple19 _tmp478=_tmp477;union Cyc_CfFlowInfo_FlowInfo _tmp483;union Cyc_CfFlowInfo_FlowInfo _tmp482;_LL3C7: _tmp483=_tmp478.f1;_tmp482=_tmp478.f2;_LL3C8:;{
struct _tuple19 _tmp479=Cyc_NewControlFlow_anal_test(env,_tmp483,_tmp4BB);struct _tuple19 _tmp47A=_tmp479;union Cyc_CfFlowInfo_FlowInfo _tmp481;union Cyc_CfFlowInfo_FlowInfo _tmp480;_LL3CA: _tmp481=_tmp47A.f1;_tmp480=_tmp47A.f2;_LL3CB:;{
struct _tuple19 _tmp47B=Cyc_NewControlFlow_anal_test(env,_tmp482,_tmp4BA);struct _tuple19 _tmp47C=_tmp47B;union Cyc_CfFlowInfo_FlowInfo _tmp47F;union Cyc_CfFlowInfo_FlowInfo _tmp47E;_LL3CD: _tmp47F=_tmp47C.f1;_tmp47E=_tmp47C.f2;_LL3CE:;
return({struct _tuple19 _tmp47D;_tmp47D.f1=Cyc_CfFlowInfo_join_flow(_tmp474,env->all_changed,_tmp481,_tmp47F);_tmp47D.f2=
Cyc_CfFlowInfo_join_flow(_tmp474,env->all_changed,_tmp480,_tmp47E);_tmp47D;});};};}case 6: _LL3BA: _tmp4BE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp476)->f1;_tmp4BD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp476)->f2;_LL3BB: {
# 2101
struct _tuple19 _tmp484=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4BE);struct _tuple19 _tmp485=_tmp484;union Cyc_CfFlowInfo_FlowInfo _tmp48C;union Cyc_CfFlowInfo_FlowInfo _tmp48B;_LL3D0: _tmp48C=_tmp485.f1;_tmp48B=_tmp485.f2;_LL3D1:;{
struct _tuple19 _tmp486=Cyc_NewControlFlow_anal_test(env,_tmp48C,_tmp4BD);struct _tuple19 _tmp487=_tmp486;union Cyc_CfFlowInfo_FlowInfo _tmp48A;union Cyc_CfFlowInfo_FlowInfo _tmp489;_LL3D3: _tmp48A=_tmp487.f1;_tmp489=_tmp487.f2;_LL3D4:;
return({struct _tuple19 _tmp488;_tmp488.f1=_tmp48A;_tmp488.f2=Cyc_CfFlowInfo_join_flow(_tmp474,env->all_changed,_tmp48B,_tmp489);_tmp488;});};}case 7: _LL3BC: _tmp4C0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp476)->f1;_tmp4BF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp476)->f2;_LL3BD: {
# 2105
struct _tuple19 _tmp48D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4C0);struct _tuple19 _tmp48E=_tmp48D;union Cyc_CfFlowInfo_FlowInfo _tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp494;_LL3D6: _tmp495=_tmp48E.f1;_tmp494=_tmp48E.f2;_LL3D7:;{
struct _tuple19 _tmp48F=Cyc_NewControlFlow_anal_test(env,_tmp494,_tmp4BF);struct _tuple19 _tmp490=_tmp48F;union Cyc_CfFlowInfo_FlowInfo _tmp493;union Cyc_CfFlowInfo_FlowInfo _tmp492;_LL3D9: _tmp493=_tmp490.f1;_tmp492=_tmp490.f2;_LL3DA:;
return({struct _tuple19 _tmp491;_tmp491.f1=Cyc_CfFlowInfo_join_flow(_tmp474,env->all_changed,_tmp495,_tmp493);_tmp491.f2=_tmp492;_tmp491;});};}case 8: _LL3BE: _tmp4C2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp476)->f1;_tmp4C1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp476)->f2;_LL3BF: {
# 2109
struct _tuple16 _tmp496=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4C2);struct _tuple16 _tmp497=_tmp496;union Cyc_CfFlowInfo_FlowInfo _tmp499;void*_tmp498;_LL3DC: _tmp499=_tmp497.f1;_tmp498=_tmp497.f2;_LL3DD:;
return Cyc_NewControlFlow_anal_test(env,_tmp499,_tmp4C1);}case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp476)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp476)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp476)->f2)->tl == 0){_LL3C0: _tmp4C3=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp476)->f2)->hd;_LL3C1: {
# 2112
struct _tuple19 _tmp49A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4C3);struct _tuple19 _tmp49B=_tmp49A;union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49D;_LL3DF: _tmp49E=_tmp49B.f1;_tmp49D=_tmp49B.f2;_LL3E0:;
return({struct _tuple19 _tmp49C;_tmp49C.f1=_tmp49D;_tmp49C.f2=_tmp49E;_tmp49C;});}}else{goto _LL3C2;}}else{goto _LL3C2;}}else{_LL3C2: _tmp4C5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp476)->f1;_tmp4C4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp476)->f2;_LL3C3:
# 2115
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4C5,_tmp4C4);}default: _LL3C4: _LL3C5: {
# 2119
struct _tuple16 _tmp49F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp4A0=_tmp49F;union Cyc_CfFlowInfo_FlowInfo _tmp4B9;void*_tmp4B8;_LL3E2: _tmp4B9=_tmp4A0.f1;_tmp4B8=_tmp4A0.f2;_LL3E3:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4A1=_tmp4B9;struct Cyc_Dict_Dict _tmp4B7;if((_tmp4A1.BottomFL).tag == 1){_LL3E5: _LL3E6:
 return({struct _tuple19 _tmp4A2;_tmp4A2.f1=_tmp4B9;_tmp4A2.f2=_tmp4B9;_tmp4A2;});}else{_LL3E7: _tmp4B7=((_tmp4A1.ReachableFL).val).f1;_LL3E8: {
# 2123
struct _tuple15 _tmp4A3=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp4B8);struct _tuple15 _tmp4A4=_tmp4A3;void*_tmp4B6;struct Cyc_List_List*_tmp4B5;_LL3EA: _tmp4B6=_tmp4A4.f1;_tmp4B5=_tmp4A4.f2;_LL3EB:;{
void*_tmp4A5=_tmp4B6;enum Cyc_CfFlowInfo_InitLevel _tmp4B4;struct Cyc_Absyn_Vardecl*_tmp4B3;void*_tmp4B2;switch(*((int*)_tmp4A5)){case 8: _LL3ED: _tmp4B3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4A5)->f1;_tmp4B2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4A5)->f2;_LL3EE:
# 2126
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4A7;_tmp4A7.tag=Cyc_Core_Impossible;_tmp4A7.f1=({const char*_tmp4A8="anal_test: bad namedlocation";_tag_dyneither(_tmp4A8,sizeof(char),29);});_tmp4A7;});_tmp4A6;}));case 0: _LL3EF: _LL3F0:
 return({struct _tuple19 _tmp4A9;_tmp4A9.f1=Cyc_CfFlowInfo_BottomFL();_tmp4A9.f2=_tmp4B9;_tmp4A9;});case 2: _LL3F1: _LL3F2:
 goto _LL3F4;case 1: _LL3F3: _LL3F4:
 goto _LL3F6;case 5: _LL3F5: _LL3F6:
 return({struct _tuple19 _tmp4AA;_tmp4AA.f1=_tmp4B9;_tmp4AA.f2=Cyc_CfFlowInfo_BottomFL();_tmp4AA;});case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4A5)->f1 == Cyc_CfFlowInfo_NoneIL){_LL3F7: _LL3F8:
 goto _LL3FA;}else{_LL3FD: _tmp4B4=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4A5)->f1;_LL3FE:
# 2136
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4B9,e,_tmp4B4,_tmp4B5);}case 4: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4A5)->f1 == Cyc_CfFlowInfo_NoneIL){_LL3F9: _LL3FA:
# 2132
 goto _LL3FC;}else{_LL3FF: _LL400:
# 2137
 return({struct _tuple19 _tmp4AE;_tmp4AE.f1=_tmp4B9;_tmp4AE.f2=_tmp4B9;_tmp4AE;});}case 7: _LL3FB: _LL3FC:
# 2134
({void*_tmp4AB=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp4AC="expression may not be initialized";_tag_dyneither(_tmp4AC,sizeof(char),34);}),_tag_dyneither(_tmp4AB,sizeof(void*),0));});
return({struct _tuple19 _tmp4AD;_tmp4AD.f1=Cyc_CfFlowInfo_BottomFL();_tmp4AD.f2=Cyc_CfFlowInfo_BottomFL();_tmp4AD;});default: _LL401: _LL402:
# 2138
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4B0;_tmp4B0.tag=Cyc_Core_Impossible;_tmp4B0.f1=({const char*_tmp4B1="anal_test";_tag_dyneither(_tmp4B1,sizeof(char),10);});_tmp4B0;});_tmp4AF;}));}_LL3EC:;};}}_LL3E4:;};}}_LL3B7:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2144
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2146
struct _tuple27*_tmp4C6=ckenv;unsigned int _tmp4D2;struct Cyc_NewControlFlow_AnalEnv*_tmp4D1;struct Cyc_Dict_Dict _tmp4D0;_LL404: _tmp4D2=_tmp4C6->f1;_tmp4D1=_tmp4C6->f2;_tmp4D0=_tmp4C6->f3;_LL405:;{
void*_tmp4C7=root;struct Cyc_Absyn_Vardecl*_tmp4CF;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4C7)->tag == 0){_LL407: _tmp4CF=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4C7)->f1;_LL408:
# 2149
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4D1->fenv)->r,_tmp4CF->type)){
struct _tuple15 _tmp4C8=Cyc_CfFlowInfo_unname_rval((_tmp4D1->fenv)->r,rval);struct _tuple15 _tmp4C9=_tmp4C8;void*_tmp4CE;_LL40C: _tmp4CE=_tmp4C9.f1;_LL40D:;{
void*_tmp4CA=_tmp4CE;switch(*((int*)_tmp4CA)){case 7: _LL40F: _LL410:
 goto _LL412;case 0: _LL411: _LL412:
 goto _LL414;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4CA)->f1 == Cyc_CfFlowInfo_NoneIL){_LL413: _LL414:
 goto _LL40E;}else{goto _LL415;}default: _LL415: _LL416:
# 2156
({struct Cyc_String_pa_PrintArg_struct _tmp4CD;_tmp4CD.tag=0;_tmp4CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4CF->name));({void*_tmp4CB[1]={& _tmp4CD};Cyc_Tcutil_warn(_tmp4D2,({const char*_tmp4CC="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4CC,sizeof(char),57);}),_tag_dyneither(_tmp4CB,sizeof(void*),1));});});
goto _LL40E;}_LL40E:;};}
# 2160
goto _LL406;}else{_LL409: _LL40A:
 goto _LL406;}_LL406:;};}
# 2165
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4D3=flow;struct Cyc_Dict_Dict _tmp4D9;if((_tmp4D3.BottomFL).tag == 1){_LL418: _LL419:
 return;}else{_LL41A: _tmp4D9=((_tmp4D3.ReachableFL).val).f1;_LL41B:
# 2169
{struct Cyc_List_List*_tmp4D4=env->param_roots;for(0;_tmp4D4 != 0;_tmp4D4=_tmp4D4->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D9,Cyc_CfFlowInfo_lookup_place(_tmp4D9,(struct Cyc_CfFlowInfo_Place*)_tmp4D4->hd))!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4D5=0;Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4D6="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4D6,sizeof(char),76);}),_tag_dyneither(_tmp4D5,sizeof(void*),0));});}}
# 2173
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4D7=({struct _tuple27 _tmp4D8;_tmp4D8.f1=loc;_tmp4D8.f2=env;_tmp4D8.f3=_tmp4D9;_tmp4D8;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4D7,_tmp4D9);}
# 2177
return;}_LL417:;}
# 2186
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2189
struct Cyc_List_List*_tmp4DA=0;
struct Cyc_List_List*_tmp4DB=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4DC=(struct _tuple24*)x->hd;struct _tuple24*_tmp4DD=_tmp4DC;struct Cyc_Absyn_Vardecl**_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;_LL41D: _tmp4E6=_tmp4DD->f1;_tmp4E5=_tmp4DD->f2;_LL41E:;
if((_tmp4E6 != 0  && _tmp4E5 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp4E5->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4DE=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E3=_region_malloc(frgn,sizeof(*_tmp4E3));_tmp4E3[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp4E4;_tmp4E4.tag=0;_tmp4E4.f1=*_tmp4E6;_tmp4E4;});_tmp4E3;});
struct Cyc_CfFlowInfo_Place*_tmp4DF=({struct Cyc_CfFlowInfo_Place*_tmp4E2=_region_malloc(frgn,sizeof(*_tmp4E2));_tmp4E2->root=(void*)_tmp4DE;_tmp4E2->fields=0;_tmp4E2;});
_tmp4DA=({struct Cyc_List_List*_tmp4E0=_region_malloc(frgn,sizeof(*_tmp4E0));_tmp4E0->hd=_tmp4DF;_tmp4E0->tl=_tmp4DA;_tmp4E0;});
_tmp4DB=({struct Cyc_List_List*_tmp4E1=_region_malloc(frgn,sizeof(*_tmp4E1));_tmp4E1->hd=_tmp4E5;_tmp4E1->tl=_tmp4DB;_tmp4E1;});}}}
# 2201
return({struct _tuple1 _tmp4E7;_tmp4E7.f1=_tmp4DA;_tmp4E7.f2=_tmp4DB;_tmp4E7;});}struct _tuple28{int f1;void*f2;};
# 2207
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2214
union Cyc_CfFlowInfo_FlowInfo _tmp4E8=flow;struct Cyc_Dict_Dict _tmp4FD;if((_tmp4E8.BottomFL).tag == 1){_LL420: _LL421:
# 2216
({void*_tmp4E9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4EA="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp4EA,sizeof(char),40);}),_tag_dyneither(_tmp4E9,sizeof(void*),0));});}else{_LL422: _tmp4FD=((_tmp4E8.ReachableFL).val).f1;_LL423: {
# 2223
struct Cyc_Absyn_Exp*_tmp4EB=Cyc_Absyn_exp_unsigned_one;
int _tmp4EC=0;
int _tmp4ED=1;
void*_tmp4EE=Cyc_CfFlowInfo_lookup_place(_tmp4FD,place);
void*_tmp4EF=_tmp4EE;
# 2235
int varok=0;
{void*_tmp4F0=_tmp4EE;struct Cyc_Absyn_Vardecl*_tmp4FB;void*_tmp4FA;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F0)->tag == 8){_LL425: _tmp4FB=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F0)->f1;_tmp4FA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F0)->f2;_LL426:
# 2238
 if(vd == _tmp4FB){
_tmp4EF=_tmp4FA;
# 2241
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
# 2243
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4EB,_tmp4ED,_tmp4EF,& _tmp4EC)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0;_tmp4F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2247
Cyc_Absynpp_qvar2string(vd->name));({void*_tmp4F1[1]={& _tmp4F3};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4F2="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp4F2,sizeof(char),58);}),_tag_dyneither(_tmp4F1,sizeof(void*),1));});});}else{
# 2250
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4FD,_tmp4EF)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0;_tmp4F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2253
Cyc_Absynpp_qvar2string(vd->name));({void*_tmp4F4[1]={& _tmp4F6};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4F5="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp4F5,sizeof(char),84);}),_tag_dyneither(_tmp4F4,sizeof(void*),1));});});else{
# 2255
varok=1;}}}else{
# 2258
varok=1;}}else{
# 2261
goto _LL428;}
goto _LL424;}else{_LL427: _LL428:
# 2265
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0;_tmp4F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2270
Cyc_Absynpp_qvar2string(vd->name));({void*_tmp4F7[1]={& _tmp4F9};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4F8="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp4F8,sizeof(char),85);}),_tag_dyneither(_tmp4F7,sizeof(void*),1));});});}
goto _LL424;}_LL424:;}
# 2274
return({struct _tuple28 _tmp4FC;_tmp4FC.f1=varok;_tmp4FC.f2=_tmp4EF;_tmp4FC;});}}_LL41F:;}
# 2280
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp4FE=p;void*_tmp506;struct Cyc_List_List*_tmp505;_LL42A: _tmp506=_tmp4FE->root;_tmp505=_tmp4FE->fields;_LL42B:;
if(_tmp505 != 0)
({void*_tmp4FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp500="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp500,sizeof(char),53);}),_tag_dyneither(_tmp4FF,sizeof(void*),0));});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp501=_tmp506;struct Cyc_Absyn_Vardecl*_tmp504;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp501)->tag == 0){_LL42D: _tmp504=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp501)->f1;_LL42E:
 return _tmp504;}else{_LL42F: _LL430:
({void*_tmp502=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp503="unconsume_params: root is not a varroot";_tag_dyneither(_tmp503,sizeof(char),40);}),_tag_dyneither(_tmp502,sizeof(void*),0));});}_LL42C:;};}
# 2299 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2307
{union Cyc_CfFlowInfo_FlowInfo _tmp507=flow;struct Cyc_Dict_Dict _tmp520;if((_tmp507.BottomFL).tag == 1){_LL432: _LL433:
 return flow;}else{_LL434: _tmp520=((_tmp507.ReachableFL).val).f1;_LL435: {
# 2314
struct _tuple18 _tmp508=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp509=_tmp508;union Cyc_CfFlowInfo_FlowInfo _tmp51F;union Cyc_CfFlowInfo_AbsLVal _tmp51E;_LL437: _tmp51F=_tmp509.f1;_tmp51E=_tmp509.f2;_LL438:;
# 2317
{union Cyc_CfFlowInfo_AbsLVal _tmp50A=_tmp51E;struct Cyc_CfFlowInfo_Place*_tmp51D;if((_tmp50A.PlaceL).tag == 1){_LL43A: _tmp51D=(_tmp50A.PlaceL).val;_LL43B: {
# 2321
void*_tmp50B=Cyc_CfFlowInfo_lookup_place(_tmp520,_tmp51D);
{void*_tmp50C=_tmp50B;struct Cyc_Absyn_Vardecl*_tmp51C;void*_tmp51B;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50C)->tag == 8){_LL43F: _tmp51C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50C)->f1;_tmp51B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50C)->f2;_LL440: {
# 2327
void*new_rval;
if(_tmp51C == vd){
# 2331
if(varok){
# 2333
_tmp50B=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp51B);
# 2338
if(ropt != 0){
# 2344
struct _tuple16 _tmp50D=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,({struct _tuple16 _tmp511;_tmp511.f1=_tmp51F;_tmp511.f2=_tmp50B;_tmp511;}),({struct _tuple16 _tmp512;_tmp512.f1=_tmp51F;_tmp512.f2=ropt;_tmp512;}));
# 2344
struct _tuple16 _tmp50E=_tmp50D;union Cyc_CfFlowInfo_FlowInfo _tmp510;void*_tmp50F;_LL444: _tmp510=_tmp50E.f1;_tmp50F=_tmp50E.f2;_LL445:;
# 2349
_tmp51F=_tmp510;new_rval=_tmp50F;}else{
# 2354
new_rval=_tmp50B;}}else{
# 2357
new_rval=_tmp51B;}
# 2359
{union Cyc_CfFlowInfo_FlowInfo _tmp513=_tmp51F;struct Cyc_Dict_Dict _tmp517;struct Cyc_List_List*_tmp516;if((_tmp513.ReachableFL).tag == 2){_LL447: _tmp517=((_tmp513.ReachableFL).val).f1;_tmp516=((_tmp513.ReachableFL).val).f2;_LL448:
# 2361
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp517,env->all_changed,_tmp51D,new_rval),_tmp516);
# 2365
goto _LL446;}else{_LL449: _LL44A:
# 2367
({void*_tmp514=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp515="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp515,sizeof(char),42);}),_tag_dyneither(_tmp514,sizeof(void*),0));});}_LL446:;}
# 2369
goto _LL43E;}else{
# 2371
goto _LL442;}
goto _LL43E;}}else{_LL441: _LL442:
# 2378
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0;_tmp51A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));({void*_tmp518[1]={& _tmp51A};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp519="aliased expression %s was overwritten";_tag_dyneither(_tmp519,sizeof(char),38);}),_tag_dyneither(_tmp518,sizeof(void*),1));});});
# 2395 "new_control_flow.cyc"
goto _LL43E;}_LL43E:;}
# 2397
goto _LL439;}}else{_LL43C: _LL43D:
# 2403
 goto _LL439;}_LL439:;}
# 2405
goto _LL431;}}_LL431:;}
# 2407
return flow;}
# 2420 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2426
{union Cyc_CfFlowInfo_FlowInfo _tmp521=flow;if((_tmp521.BottomFL).tag == 1){_LL44C: _LL44D:
 return flow;}else{_LL44E: _LL44F:
 goto _LL44B;}_LL44B:;}{
# 2430
int _tmp522=unconsume_exps != 0;
{struct Cyc_List_List*_tmp523=consumed_vals;for(0;_tmp523 != 0;
(_tmp523=_tmp523->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2437
struct Cyc_Absyn_Vardecl*_tmp524=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp523->hd);
struct _tuple28 _tmp525=
is_region_open?({struct _tuple28 _tmp529;_tmp529.f1=1;_tmp529.f2=0;_tmp529;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp523->hd,_tmp522,_tmp524,flow,loc);
# 2438
struct _tuple28 _tmp526=_tmp525;int _tmp528;void*_tmp527;_LL451: _tmp528=_tmp526.f1;_tmp527=_tmp526.f2;_LL452:;
# 2444
if(_tmp522)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,_tmp524,_tmp528,_tmp527,flow,loc);}}
# 2447
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2451
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2454
struct Cyc_CfFlowInfo_FlowEnv*_tmp52A=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp52B=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp52C=_tmp52B;struct Cyc_Core_Opt*_tmp540;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Stmt*_tmp53E;unsigned int _tmp53D;_LL454: _tmp540=_tmp52C->pat_vars;_tmp53F=_tmp52C->where_clause;_tmp53E=_tmp52C->body;_tmp53D=_tmp52C->loc;_LL455:;{
struct _tuple1 _tmp52D=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp540))->v);struct _tuple1 _tmp52E=_tmp52D;struct Cyc_List_List*_tmp53C;struct Cyc_List_List*_tmp53B;_LL457: _tmp53C=_tmp52E.f1;_tmp53B=_tmp52E.f2;_LL458:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp540->v,_tmp53C != 0,_tmp53D,exp_loc);
# 2462
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp52F=env->unique_pat_vars;
# 2465
if(Cyc_Tcpat_has_vars(_tmp540))
env->unique_pat_vars=({struct Cyc_List_List*_tmp530=_region_malloc(env->r,sizeof(*_tmp530));_tmp530->hd=({struct _tuple29*_tmp531=_region_malloc(env->r,sizeof(*_tmp531));_tmp531->f1=0;_tmp531->f2=_tmp53E;_tmp531->f3=_tmp53C;_tmp531->f4=_tmp53B;_tmp531;});_tmp530->tl=_tmp52F;_tmp530;});
# 2470
if(_tmp53F != 0){
struct Cyc_Absyn_Exp*wexp=_tmp53F;
struct _tuple19 _tmp532=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp533=_tmp532;union Cyc_CfFlowInfo_FlowInfo _tmp535;union Cyc_CfFlowInfo_FlowInfo _tmp534;_LL45A: _tmp535=_tmp533.f1;_tmp534=_tmp533.f2;_LL45B:;
inflow=_tmp534;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp535,_tmp53E,0);}else{
# 2476
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp53E,0);}
# 2478
env->unique_pat_vars=_tmp52F;{
union Cyc_CfFlowInfo_FlowInfo _tmp536=clause_outflow;if((_tmp536.BottomFL).tag == 1){_LL45D: _LL45E:
 goto _LL45C;}else{_LL45F: _LL460:
# 2483
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp53C,_tmp53B,0,clause_outflow,_tmp53D);
# 2485
if(scs->tl == 0)
return clause_outflow;else{
# 2490
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp537=0;Cyc_CfFlowInfo_aerr(_tmp53E->loc,({const char*_tmp538="switch clause may implicitly fallthru";_tag_dyneither(_tmp538,sizeof(char),38);}),_tag_dyneither(_tmp537,sizeof(void*),0));});else{
# 2493
({void*_tmp539=0;Cyc_Tcutil_warn(_tmp53E->loc,({const char*_tmp53A="switch clause may implicitly fallthru";_tag_dyneither(_tmp53A,sizeof(char),38);}),_tag_dyneither(_tmp539,sizeof(void*),0));});}
# 2495
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2497
goto _LL45C;}_LL45C:;};};};}
# 2500
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2505
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2507
struct _tuple30*_tmp541=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp54C;struct Cyc_Dict_Dict _tmp54B;unsigned int _tmp54A;_LL462: _tmp54C=_tmp541->f1;_tmp54B=_tmp541->f2;_tmp54A=_tmp541->f3;_LL463:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp54C->fenv)->r,vd->type)){
# 2510
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp549;_tmp549.tag=0;_tmp549.f1=vd;_tmp549;});
# 2512
struct _tuple15 _tmp542=Cyc_CfFlowInfo_unname_rval((_tmp54C->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp54B,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2512
struct _tuple15 _tmp543=_tmp542;void*_tmp548;_LL465: _tmp548=_tmp543.f1;_LL466:;{
# 2514
void*_tmp544=_tmp548;switch(*((int*)_tmp544)){case 7: _LL468: _LL469:
 goto _LL46B;case 0: _LL46A: _LL46B:
 goto _LL46D;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp544)->f1 == Cyc_CfFlowInfo_NoneIL){_LL46C: _LL46D:
 goto _LL467;}else{goto _LL46E;}default: _LL46E: _LL46F:
# 2519
({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0;_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));({void*_tmp545[1]={& _tmp547};Cyc_Tcutil_warn(_tmp54A,({const char*_tmp546="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp546,sizeof(char),68);}),_tag_dyneither(_tmp545,sizeof(void*),1));});});
# 2521
goto _LL467;}_LL467:;};}}
# 2526
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2528
{union Cyc_CfFlowInfo_FlowInfo _tmp54D=inflow;struct Cyc_Dict_Dict _tmp55A;if((_tmp54D.ReachableFL).tag == 2){_LL471: _tmp55A=((_tmp54D.ReachableFL).val).f1;_LL472: {
# 2530
struct _tuple30 _tmp54E=({struct _tuple30 _tmp559;_tmp559.f1=env;_tmp559.f2=_tmp55A;_tmp559.f3=decl->loc;_tmp559;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp54F=env->fenv;
{void*_tmp550=decl->r;void*_tmp551=_tmp550;struct Cyc_List_List*_tmp558;struct Cyc_List_List*_tmp557;struct Cyc_Absyn_Vardecl*_tmp556;switch(*((int*)_tmp551)){case 0: _LL476: _tmp556=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp551)->f1;_LL477:
# 2534
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp54E,_tmp556);
goto _LL475;case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp551)->f2 != 0){_LL478: _tmp557=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp551)->f2)->v;_LL479: {
# 2537
struct _tuple1 _tmp552=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp557);struct _tuple1 _tmp553=_tmp552;struct Cyc_List_List*_tmp555;_LL47F: _tmp555=_tmp553.f1;_LL480:;{
struct Cyc_List_List*_tmp554=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp555);
_tmp558=_tmp554;goto _LL47B;};}}else{goto _LL47C;}case 3: _LL47A: _tmp558=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp551)->f1;_LL47B:
# 2541
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp54E,_tmp558);
goto _LL475;default: _LL47C: _LL47D:
 goto _LL475;}_LL475:;}
# 2545
goto _LL470;}}else{_LL473: _LL474:
 goto _LL470;}_LL470:;}
# 2548
return;}
# 2554
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2558
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp55B=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp55C=_tmp55B;int _tmp560;struct Cyc_Absyn_Stmt*_tmp55F;struct Cyc_List_List*_tmp55E;struct Cyc_List_List*_tmp55D;_LL482: _tmp560=_tmp55C->f1;_tmp55F=_tmp55C->f2;_tmp55E=_tmp55C->f3;_tmp55D=_tmp55C->f4;_LL483:;
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp55E,_tmp55D,_tmp560,inflow,dest->loc);}}
# 2563
return inflow;}
# 2569
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2572
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp561=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp562=_tmp561;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp611;union Cyc_CfFlowInfo_FlowInfo*_tmp610;_LL485: _tmp611=_tmp562.f1;_tmp610=_tmp562.f2;_LL486:;
inflow=*_tmp610;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp563=env->fenv;
# 2579
void*_tmp564=s->r;void*_tmp565=_tmp564;struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Stmt*_tmp60E;struct Cyc_Absyn_Decl*_tmp60D;struct Cyc_Absyn_Stmt*_tmp60C;struct Cyc_Absyn_Vardecl*_tmp60B;struct Cyc_Absyn_Exp*_tmp60A;unsigned int _tmp609;struct Cyc_Absyn_Stmt*_tmp608;struct Cyc_List_List*_tmp607;struct Cyc_Absyn_Exp*_tmp606;unsigned int _tmp605;struct Cyc_Absyn_Stmt*_tmp604;struct Cyc_Absyn_Stmt*_tmp603;struct Cyc_List_List*_tmp602;void*_tmp601;struct Cyc_Absyn_Exp*_tmp600;struct Cyc_List_List*_tmp5FF;void*_tmp5FE;struct Cyc_List_List*_tmp5FD;struct Cyc_Absyn_Switch_clause*_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Stmt*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Stmt*_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Stmt*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_Absyn_Stmt*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Stmt*_tmp5F1;struct Cyc_Absyn_Stmt*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5EF;struct Cyc_Absyn_Stmt*_tmp5EE;struct Cyc_Absyn_Stmt*_tmp5ED;struct Cyc_Absyn_Stmt*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5E9;switch(*((int*)_tmp565)){case 0: _LL488: _LL489:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp565)->f1 == 0){_LL48A: _LL48B:
# 2583
 if(env->noreturn)
({void*_tmp566=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp567="`noreturn' function might return";_tag_dyneither(_tmp567,sizeof(char),33);}),_tag_dyneither(_tmp566,sizeof(void*),0));});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL48C: _tmp5E9=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_LL48D:
# 2589
 if(env->noreturn)
({void*_tmp568=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp569="`noreturn' function might return";_tag_dyneither(_tmp569,sizeof(char),33);}),_tag_dyneither(_tmp568,sizeof(void*),0));});{
struct _tuple16 _tmp56A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5E9));struct _tuple16 _tmp56B=_tmp56A;union Cyc_CfFlowInfo_FlowInfo _tmp56D;void*_tmp56C;_LL4B3: _tmp56D=_tmp56B.f1;_tmp56C=_tmp56B.f2;_LL4B4:;
_tmp56D=Cyc_NewControlFlow_use_Rval(env,_tmp5E9->loc,_tmp56D,_tmp56C);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp56D);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp56D,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1: _LL48E: _tmp5EA=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_LL48F: {
# 2598
struct _tuple17*_tmp56E=rval_opt;void**_tmp574;int _tmp573;if(_tmp56E != 0){_LL4B6: _tmp574=(void**)& _tmp56E->f1;_tmp573=_tmp56E->f2;_LL4B7: {
# 2600
struct _tuple16 _tmp56F=Cyc_NewControlFlow_anal_Rexp(env,_tmp573,inflow,_tmp5EA);struct _tuple16 _tmp570=_tmp56F;union Cyc_CfFlowInfo_FlowInfo _tmp572;void*_tmp571;_LL4BB: _tmp572=_tmp570.f1;_tmp571=_tmp570.f2;_LL4BC:;
*_tmp574=_tmp571;
return _tmp572;}}else{_LL4B8: _LL4B9:
# 2604
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5EA)).f1;}_LL4B5:;}case 2: _LL490: _tmp5EC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5EB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL491:
# 2608
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5EC,0),_tmp5EB,rval_opt);case 4: _LL492: _tmp5EF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5EE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_tmp5ED=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp565)->f3;_LL493: {
# 2611
struct _tuple19 _tmp575=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5EF);struct _tuple19 _tmp576=_tmp575;union Cyc_CfFlowInfo_FlowInfo _tmp57A;union Cyc_CfFlowInfo_FlowInfo _tmp579;_LL4BE: _tmp57A=_tmp576.f1;_tmp579=_tmp576.f2;_LL4BF:;{
# 2618
union Cyc_CfFlowInfo_FlowInfo _tmp577=Cyc_NewControlFlow_anal_stmt(env,_tmp579,_tmp5ED,0);
union Cyc_CfFlowInfo_FlowInfo _tmp578=Cyc_NewControlFlow_anal_stmt(env,_tmp57A,_tmp5EE,0);
return Cyc_CfFlowInfo_join_flow(_tmp563,env->all_changed,_tmp578,_tmp577);};}case 5: _LL494: _tmp5F2=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp565)->f1).f1;_tmp5F1=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp565)->f1).f2;_tmp5F0=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL495: {
# 2626
struct _tuple20 _tmp57B=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5F1);struct _tuple20 _tmp57C=_tmp57B;union Cyc_CfFlowInfo_FlowInfo*_tmp583;_LL4C1: _tmp583=_tmp57C.f2;_LL4C2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp57D=*_tmp583;
struct _tuple19 _tmp57E=Cyc_NewControlFlow_anal_test(env,_tmp57D,_tmp5F2);struct _tuple19 _tmp57F=_tmp57E;union Cyc_CfFlowInfo_FlowInfo _tmp582;union Cyc_CfFlowInfo_FlowInfo _tmp581;_LL4C4: _tmp582=_tmp57F.f1;_tmp581=_tmp57F.f2;_LL4C5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp580=Cyc_NewControlFlow_anal_stmt(env,_tmp582,_tmp5F0,0);
Cyc_NewControlFlow_update_flow(env,_tmp5F1,_tmp580);
return _tmp581;};};}case 14: _LL496: _tmp5F5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5F4=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp565)->f2).f1;_tmp5F3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp565)->f2).f2;_LL497: {
# 2636
union Cyc_CfFlowInfo_FlowInfo _tmp584=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5F5,0);
struct _tuple20 _tmp585=Cyc_NewControlFlow_pre_stmt_check(env,_tmp584,_tmp5F3);struct _tuple20 _tmp586=_tmp585;union Cyc_CfFlowInfo_FlowInfo*_tmp58C;_LL4C7: _tmp58C=_tmp586.f2;_LL4C8:;{
union Cyc_CfFlowInfo_FlowInfo _tmp587=*_tmp58C;
struct _tuple19 _tmp588=Cyc_NewControlFlow_anal_test(env,_tmp587,_tmp5F4);struct _tuple19 _tmp589=_tmp588;union Cyc_CfFlowInfo_FlowInfo _tmp58B;union Cyc_CfFlowInfo_FlowInfo _tmp58A;_LL4CA: _tmp58B=_tmp589.f1;_tmp58A=_tmp589.f2;_LL4CB:;
Cyc_NewControlFlow_update_flow(env,_tmp5F5,_tmp58B);
return _tmp58A;};}case 9: _LL498: _tmp5FB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5FA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f2).f1;_tmp5F9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f2).f2;_tmp5F8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f3).f1;_tmp5F7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f3).f2;_tmp5F6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp565)->f4;_LL499: {
# 2645
union Cyc_CfFlowInfo_FlowInfo _tmp58D=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5FB)).f1;
struct _tuple20 _tmp58E=Cyc_NewControlFlow_pre_stmt_check(env,_tmp58D,_tmp5F9);struct _tuple20 _tmp58F=_tmp58E;union Cyc_CfFlowInfo_FlowInfo*_tmp59B;_LL4CD: _tmp59B=_tmp58F.f2;_LL4CE:;{
union Cyc_CfFlowInfo_FlowInfo _tmp590=*_tmp59B;
struct _tuple19 _tmp591=Cyc_NewControlFlow_anal_test(env,_tmp590,_tmp5FA);struct _tuple19 _tmp592=_tmp591;union Cyc_CfFlowInfo_FlowInfo _tmp59A;union Cyc_CfFlowInfo_FlowInfo _tmp599;_LL4D0: _tmp59A=_tmp592.f1;_tmp599=_tmp592.f2;_LL4D1:;{
union Cyc_CfFlowInfo_FlowInfo _tmp593=Cyc_NewControlFlow_anal_stmt(env,_tmp59A,_tmp5F6,0);
struct _tuple20 _tmp594=Cyc_NewControlFlow_pre_stmt_check(env,_tmp593,_tmp5F7);struct _tuple20 _tmp595=_tmp594;union Cyc_CfFlowInfo_FlowInfo*_tmp598;_LL4D3: _tmp598=_tmp595.f2;_LL4D4:;{
union Cyc_CfFlowInfo_FlowInfo _tmp596=*_tmp598;
union Cyc_CfFlowInfo_FlowInfo _tmp597=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp596,_tmp5F8)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5F9,_tmp597);
return _tmp599;};};};}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp565)->f2 != 0){_LL49A: _tmp5FD=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5FC=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL49B: {
# 2657
struct _RegionHandle _tmp59C=_new_region("temp");struct _RegionHandle*temp=& _tmp59C;_push_region(temp);
{struct _tuple23 _tmp59D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp5FD,1,1);struct _tuple23 _tmp59E=_tmp59D;union Cyc_CfFlowInfo_FlowInfo _tmp5A6;struct Cyc_List_List*_tmp5A5;_LL4D6: _tmp5A6=_tmp59E.f1;_tmp5A5=_tmp59E.f2;_LL4D7:;
# 2660
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp5FC->body);{
# 2662
struct Cyc_List_List*_tmp59F=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5FC->pat_vars))->v)).f1);
_tmp5A6=Cyc_NewControlFlow_add_vars(_tmp563,_tmp5A6,_tmp59F,_tmp563->unknown_all,s->loc,0);
# 2665
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5FC->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp5A0=(struct _tuple24*)x->hd;struct _tuple24*_tmp5A1=_tmp5A0;struct Cyc_Absyn_Vardecl**_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A2;_LL4D9: _tmp5A3=_tmp5A1->f1;_tmp5A2=_tmp5A1->f2;_LL4DA:;
if(_tmp5A3 != 0){
_tmp5A6=Cyc_NewControlFlow_do_initialize_var(_tmp563,env,_tmp5A6,*_tmp5A3,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5FD))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5A5))->hd,s->loc);
_tmp5A5=_tmp5A5->tl;
_tmp5FD=_tmp5FD->tl;}}}
# 2673
Cyc_NewControlFlow_update_flow(env,_tmp5FC->body,_tmp5A6);{
union Cyc_CfFlowInfo_FlowInfo _tmp5A4=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5A4;};};}
# 2658
;_pop_region(temp);}}else{_LL4B0: _LL4B1:
# 2852
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6[0]=({struct Cyc_Core_Impossible_exn_struct _tmp5E7;_tmp5E7.tag=Cyc_Core_Impossible;_tmp5E7.f1=({const char*_tmp5E8="anal_stmt -- bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp5E8,sizeof(char),56);});_tmp5E7;});_tmp5E6;}));}case 6: _LL49C: _LL49D:
# 2680
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp5A7=inflow;if((_tmp5A7.ReachableFL).tag == 2){_LL4DC: _LL4DD:
# 2683
{void*_tmp5A8=Cyc_Tcutil_compress(env->return_type);void*_tmp5A9=_tmp5A8;switch(*((int*)_tmp5A9)){case 0: _LL4E1: _LL4E2:
 goto _LL4E0;case 7: _LL4E3: _LL4E4:
 goto _LL4E6;case 6: _LL4E5: _LL4E6:
# 2687
({void*_tmp5AA=0;Cyc_Tcutil_warn(s->loc,({const char*_tmp5AB="break may cause function not to return a value";_tag_dyneither(_tmp5AB,sizeof(char),47);}),_tag_dyneither(_tmp5AA,sizeof(void*),0));});
goto _LL4E0;default: _LL4E7: _LL4E8:
# 2690
({void*_tmp5AC=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp5AD="break may cause function not to return a value";_tag_dyneither(_tmp5AD,sizeof(char),47);}),_tag_dyneither(_tmp5AC,sizeof(void*),0));});
goto _LL4E0;}_LL4E0:;}
# 2693
goto _LL4DB;}else{_LL4DE: _LL4DF:
 goto _LL4DB;}_LL4DB:;}
# 2696
if(env->noreturn)
({void*_tmp5AE=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp5AF="`noreturn' function might return";_tag_dyneither(_tmp5AF,sizeof(char),33);}),_tag_dyneither(_tmp5AE,sizeof(void*),0));});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2702
goto _LL49F;case 7: _LL49E: _LL49F:
 goto _LL4A1;case 8: _LL4A0: _LL4A1: {
# 2706
struct Cyc_Absyn_Stmt*_tmp5B0=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp5B0 == 0)
({void*_tmp5B1=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp5B2="jump has no target (should be impossible)";_tag_dyneither(_tmp5B2,sizeof(char),42);}),_tag_dyneither(_tmp5B1,sizeof(void*),0));});
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp5B0));
# 2711
Cyc_NewControlFlow_update_flow(env,_tmp5B0,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _LL4A2: _tmp600=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp5FF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_tmp5FE=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp565)->f3;_LL4A3:
# 2717
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp5FF,_tmp600->loc);case 15: _LL4A4: _tmp603=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp602=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_tmp601=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp565)->f3;_LL4A5: {
# 2722
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp603,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2730
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2733
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp602,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5B3=scs_outflow;if((_tmp5B3.BottomFL).tag == 1){_LL4EA: _LL4EB:
 goto _LL4E9;}else{_LL4EC: _LL4ED:
({void*_tmp5B4=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp5B5="last catch clause may implicitly fallthru";_tag_dyneither(_tmp5B5,sizeof(char),42);}),_tag_dyneither(_tmp5B4,sizeof(void*),0));});}_LL4E9:;}
# 2739
outflow=s1_outflow;
# 2741
return outflow;};};}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->r)->f2 != 0){_LL4A6: _tmp607=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->r)->f2)->v;_tmp606=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->r)->f3;_tmp605=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->loc;_tmp604=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL4A7: {
# 2751
struct _tuple1 _tmp5B6=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp607);struct _tuple1 _tmp5B7=_tmp5B6;struct Cyc_List_List*_tmp5BC;struct Cyc_List_List*_tmp5BB;_LL4EF: _tmp5BC=_tmp5B7.f1;_tmp5BB=_tmp5B7.f2;_LL4F0:;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp563,env,inflow,_tmp607,_tmp5BC != 0,_tmp605,_tmp606->loc);{
struct Cyc_List_List*_tmp5B8=env->unique_pat_vars;
# 2755
env->unique_pat_vars=({struct Cyc_List_List*_tmp5B9=_region_malloc(env->r,sizeof(*_tmp5B9));_tmp5B9->hd=({struct _tuple29*_tmp5BA=_region_malloc(env->r,sizeof(*_tmp5BA));_tmp5BA->f1=0;_tmp5BA->f2=s;_tmp5BA->f3=_tmp5BC;_tmp5BA->f4=_tmp5BB;_tmp5BA;});_tmp5B9->tl=_tmp5B8;_tmp5B9;});
# 2760
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp604,rval_opt);
env->unique_pat_vars=_tmp5B8;
# 2764
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5BC,_tmp5BB,0,inflow,_tmp605);
# 2768
return inflow;};}}else{goto _LL4AA;}case 4: _LL4A8: _tmp60B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->r)->f2;_tmp60A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->r)->f4;_tmp609=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1)->loc;_tmp608=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;if(_tmp60A != 0){_LL4A9: {
# 2780
struct Cyc_List_List l=({struct Cyc_List_List _tmp5DA;_tmp5DA.hd=_tmp60A;_tmp5DA.tl=0;_tmp5DA;});
union Cyc_CfFlowInfo_FlowInfo _tmp5BD=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5BE=Cyc_NewControlFlow_anal_Lexp(env,_tmp5BD,0,0,_tmp60A);struct _tuple18 _tmp5BF=_tmp5BE;union Cyc_CfFlowInfo_AbsLVal _tmp5D9;_LL4F2: _tmp5D9=_tmp5BF.f2;_LL4F3:;{
struct _tuple16 _tmp5C0=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5BD,_tmp60A);struct _tuple16 _tmp5C1=_tmp5C0;union Cyc_CfFlowInfo_FlowInfo _tmp5D8;_LL4F5: _tmp5D8=_tmp5C1.f1;_LL4F6:;{
struct Cyc_List_List*_tmp5C2=0;
struct Cyc_List_List*_tmp5C3=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5C4=_tmp5D8;struct Cyc_Dict_Dict _tmp5D2;struct Cyc_List_List*_tmp5D1;if((_tmp5C4.ReachableFL).tag == 2){_LL4F8: _tmp5D2=((_tmp5C4.ReachableFL).val).f1;_tmp5D1=((_tmp5C4.ReachableFL).val).f2;_LL4F9:
# 2788
{union Cyc_CfFlowInfo_AbsLVal _tmp5C5=_tmp5D9;struct Cyc_CfFlowInfo_Place*_tmp5D0;if((_tmp5C5.PlaceL).tag == 1){_LL4FD: _tmp5D0=(_tmp5C5.PlaceL).val;_LL4FE: {
# 2792
void*_tmp5C6=Cyc_CfFlowInfo_lookup_place(_tmp5D2,_tmp5D0);
_tmp5C6=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C7=_region_malloc(_tmp563->r,sizeof(*_tmp5C7));_tmp5C7[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5C8;_tmp5C8.tag=8;_tmp5C8.f1=_tmp60B;_tmp5C8.f2=_tmp5C6;_tmp5C8;});_tmp5C7;});
_tmp5D2=Cyc_CfFlowInfo_assign_place(_tmp563,_tmp60A->loc,_tmp5D2,env->all_changed,_tmp5D0,_tmp5C6);
# 2796
_tmp5D8=Cyc_CfFlowInfo_ReachableFL(_tmp5D2,_tmp5D1);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5C9=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5CE=_region_malloc(_tmp563->r,sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5CF;_tmp5CF.tag=0;_tmp5CF.f1=_tmp60B;_tmp5CF;});_tmp5CE;});
struct Cyc_CfFlowInfo_Place*_tmp5CA=({struct Cyc_CfFlowInfo_Place*_tmp5CD=_region_malloc(_tmp563->r,sizeof(*_tmp5CD));_tmp5CD->root=(void*)_tmp5C9;_tmp5CD->fields=0;_tmp5CD;});
_tmp5C2=({struct Cyc_List_List*_tmp5CB=_region_malloc(_tmp563->r,sizeof(*_tmp5CB));_tmp5CB->hd=_tmp5CA;_tmp5CB->tl=_tmp5C2;_tmp5CB;});
_tmp5C3=({struct Cyc_List_List*_tmp5CC=_region_malloc(_tmp563->r,sizeof(*_tmp5CC));_tmp5CC->hd=_tmp60A;_tmp5CC->tl=_tmp5C3;_tmp5CC;});
goto _LL4FC;};}}else{_LL4FF: _LL500:
# 2808
 goto _LL4FC;}_LL4FC:;}
# 2810
goto _LL4F7;}else{_LL4FA: _LL4FB:
# 2812
 goto _LL4F7;}_LL4F7:;}{
# 2815
struct Cyc_List_List _tmp5D3=({struct Cyc_List_List _tmp5D7;_tmp5D7.hd=_tmp60B;_tmp5D7.tl=0;_tmp5D7;});
_tmp5D8=Cyc_NewControlFlow_add_vars(_tmp563,_tmp5D8,& _tmp5D3,_tmp563->unknown_all,_tmp609,0);{
# 2819
struct Cyc_List_List*_tmp5D4=env->unique_pat_vars;
env->unique_pat_vars=({struct Cyc_List_List*_tmp5D5=_region_malloc(env->r,sizeof(*_tmp5D5));_tmp5D5->hd=({struct _tuple29*_tmp5D6=_region_malloc(env->r,sizeof(*_tmp5D6));_tmp5D6->f1=1;_tmp5D6->f2=s;_tmp5D6->f3=_tmp5C2;_tmp5D6->f4=_tmp5C3;_tmp5D6;});_tmp5D5->tl=_tmp5D4;_tmp5D5;});
# 2825
_tmp5D8=Cyc_NewControlFlow_anal_stmt(env,_tmp5D8,_tmp608,rval_opt);
env->unique_pat_vars=_tmp5D4;
# 2829
_tmp5D8=Cyc_NewControlFlow_unconsume_params(env,_tmp5C2,_tmp5C3,1,_tmp5D8,_tmp609);
# 2833
return _tmp5D8;};};};};}}else{goto _LL4AA;}default: _LL4AA: _tmp60D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_tmp60C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL4AB:
# 2836
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp60D),_tmp60C,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp60D);
return outflow;}case 13: _LL4AC: _tmp60E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp565)->f2;_LL4AD:
# 2841
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp60E,rval_opt);default: _LL4AE: _tmp60F=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp565)->f1;_LL4AF: {
# 2844
struct _tuple16 _tmp5DB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp60F);struct _tuple16 _tmp5DC=_tmp5DB;union Cyc_CfFlowInfo_FlowInfo _tmp5E5;void*_tmp5E4;_LL502: _tmp5E5=_tmp5DC.f1;_tmp5E4=_tmp5DC.f2;_LL503:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5DD=Cyc_NewControlFlow_use_Rval(env,_tmp60F->loc,_tmp5E5,_tmp5E4);
void*_tmp5DE=Cyc_Tcutil_compress((void*)_check_null(_tmp60F->topt));void*_tmp5DF=_tmp5DE;void*_tmp5E3;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5DF)->tag == 15){_LL505: _tmp5E3=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5DF)->f1;_LL506:
# 2848
 return Cyc_CfFlowInfo_kill_flow_region(_tmp563,_tmp5E5,_tmp5E3);}else{_LL507: _LL508:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0[0]=({struct Cyc_Core_Impossible_exn_struct _tmp5E1;_tmp5E1.tag=Cyc_Core_Impossible;_tmp5E1.f1=({const char*_tmp5E2="anal_stmt -- reset_region";_tag_dyneither(_tmp5E2,sizeof(char),26);});_tmp5E1;});_tmp5E0;}));}_LL504:;};}}_LL487:;};}
# 2857
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2861
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp612=env->fenv;
void*_tmp613=decl->r;void*_tmp614=_tmp613;struct Cyc_Absyn_Tvar*_tmp62C;struct Cyc_Absyn_Vardecl*_tmp62B;int _tmp62A;struct Cyc_Absyn_Exp*_tmp629;struct Cyc_Absyn_Fndecl*_tmp628;struct Cyc_List_List*_tmp627;struct Cyc_Absyn_Vardecl*_tmp626;switch(*((int*)_tmp614)){case 0: _LL50A: _tmp626=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp614)->f1;_LL50B: {
# 2871
struct Cyc_List_List _tmp615=({struct Cyc_List_List _tmp61B;_tmp61B.hd=_tmp626;_tmp61B.tl=0;_tmp61B;});
inflow=Cyc_NewControlFlow_add_vars(_tmp612,inflow,& _tmp615,_tmp612->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp616=_tmp626->initializer;
if(_tmp616 == 0)
return inflow;{
struct _tuple16 _tmp617=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp616);struct _tuple16 _tmp618=_tmp617;union Cyc_CfFlowInfo_FlowInfo _tmp61A;void*_tmp619;_LL515: _tmp61A=_tmp618.f1;_tmp619=_tmp618.f2;_LL516:;
return Cyc_NewControlFlow_do_initialize_var(_tmp612,env,_tmp61A,_tmp626,_tmp616,_tmp619,decl->loc);};};}case 3: _LL50C: _tmp627=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp614)->f1;_LL50D:
# 2880
 return Cyc_NewControlFlow_add_vars(_tmp612,inflow,_tmp627,_tmp612->unknown_none,decl->loc,0);case 1: _LL50E: _tmp628=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp614)->f1;_LL50F:
# 2883
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp612,inflow,_tmp628);{
void*t=(void*)_check_null(_tmp628->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp61C=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp628->fn_vardecl);
# 2889
return Cyc_NewControlFlow_add_vars(_tmp612,inflow,({struct Cyc_List_List*_tmp61D=_region_malloc(env->r,sizeof(*_tmp61D));_tmp61D->hd=_tmp61C;_tmp61D->tl=0;_tmp61D;}),_tmp612->unknown_all,decl->loc,0);};case 4: _LL510: _tmp62C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp614)->f1;_tmp62B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp614)->f2;_tmp62A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp614)->f3;_tmp629=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp614)->f4;_LL511:
# 2892
 if(_tmp629 != 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E[0]=({struct Cyc_Core_Impossible_exn_struct _tmp61F;_tmp61F.tag=Cyc_Core_Impossible;_tmp61F.f1=({const char*_tmp620="found open expression in declaration!";_tag_dyneither(_tmp620,sizeof(char),38);});_tmp61F;});_tmp61E;}));{
struct Cyc_List_List _tmp621=({struct Cyc_List_List _tmp622;_tmp622.hd=_tmp62B;_tmp622.tl=0;_tmp622;});
return Cyc_NewControlFlow_add_vars(_tmp612,inflow,& _tmp621,_tmp612->unknown_all,decl->loc,0);};default: _LL512: _LL513:
# 2898
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623[0]=({struct Cyc_Core_Impossible_exn_struct _tmp624;_tmp624.tag=Cyc_Core_Impossible;_tmp624.f1=({const char*_tmp625="anal_decl: unexpected decl variant";_tag_dyneither(_tmp625,sizeof(char),35);});_tmp624;});_tmp623;}));}_LL509:;}
# 2906
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp62D;_push_handler(& _tmp62D);{int _tmp62F=0;if(setjmp(_tmp62D.handler))_tmp62F=1;if(!_tmp62F){
{struct _RegionHandle _tmp630=_new_region("frgn");struct _RegionHandle*frgn=& _tmp630;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(tables,fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2909
;_pop_region(frgn);}
# 2908
;_pop_handler();}else{void*_tmp62E=(void*)_exn_thrown;void*_tmp631=_tmp62E;void*_tmp634;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp631)->tag == Cyc_Dict_Absent){_LL518: _LL519:
# 2915
 if(Cyc_Position_num_errors > 0)
goto _LL517;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp632=_cycalloc_atomic(sizeof(*_tmp632));_tmp632[0]=({struct Cyc_Dict_Absent_exn_struct _tmp633;_tmp633.tag=Cyc_Dict_Absent;_tmp633;});_tmp632;}));}}else{_LL51A: _tmp634=_tmp631;_LL51B:(int)_rethrow(_tmp634);}_LL517:;}};}
# 2921
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2926
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp635=r;unsigned int _tmp637;if((_tmp635.RParam).tag == 5){_LL51D: _tmp637=(_tmp635.RParam).val;_LL51E: {
# 2929
struct Cyc_Absyn_Vardecl*_tmp636=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp637);
if(!_tmp636->escapes)
return Cyc_Relations_RVar(_tmp636);
return r;}}else{_LL51F: _LL520:
 return r;}_LL51C:;}
# 2937
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2941
struct _RegionHandle*_tmp638=fenv->r;
unsigned int _tmp639=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp639,1);{
# 2947
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp63A=({union Cyc_CfFlowInfo_FlowInfo _tmp672=inflow;if((_tmp672.ReachableFL).tag != 2)_throw_match();(_tmp672.ReachableFL).val;});struct _tuple14 _tmp63B=_tmp63A;struct Cyc_Dict_Dict _tmp671;struct Cyc_List_List*_tmp670;_LL522: _tmp671=_tmp63B.f1;_tmp670=_tmp63B.f2;_LL523:;{
# 2952
struct Cyc_List_List*_tmp63C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp63D=fd->requires_relns;for(0;_tmp63D != 0;_tmp63D=_tmp63D->tl){
struct Cyc_Relations_Reln*_tmp63E=(struct Cyc_Relations_Reln*)_tmp63D->hd;
_tmp670=Cyc_Relations_add_relation(_tmp638,Cyc_NewControlFlow_translate_rop(_tmp63C,_tmp63E->rop1),_tmp63E->relation,
Cyc_NewControlFlow_translate_rop(_tmp63C,_tmp63E->rop2),_tmp670);}}{
# 2961
struct Cyc_List_List*atts;
{void*_tmp63F=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp640=_tmp63F;struct Cyc_List_List*_tmp643;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp640)->tag == 9){_LL525: _tmp643=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp640)->f1).attributes;_LL526:
 atts=_tmp643;goto _LL524;}else{_LL527: _LL528:
({void*_tmp641=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp642="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp642,sizeof(char),50);}),_tag_dyneither(_tmp641,sizeof(void*),0));});}_LL524:;}
# 2968
for(0;atts != 0;atts=atts->tl){
void*_tmp644=(void*)atts->hd;void*_tmp645=_tmp644;int _tmp665;int _tmp664;int _tmp663;switch(*((int*)_tmp645)){case 21: _LL52A: _tmp663=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp645)->f1;_LL52B: {
# 2971
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp646=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp663 - 1);
void*t=Cyc_Tcutil_compress(_tmp646->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp647=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp648=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp64F=_region_malloc(_tmp638,sizeof(*_tmp64F));_tmp64F[0]=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp650;_tmp650.tag=2;_tmp650.f1=_tmp663;_tmp650.f2=t;_tmp650;});_tmp64F;});
struct Cyc_CfFlowInfo_Place*_tmp649=({struct Cyc_CfFlowInfo_Place*_tmp64E=_region_malloc(_tmp638,sizeof(*_tmp64E));_tmp64E->root=(void*)_tmp648;_tmp64E->fields=0;_tmp64E;});
_tmp671=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp671,(void*)_tmp648,_tmp647);
_tmp671=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp671,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp64A=_region_malloc(_tmp638,sizeof(*_tmp64A));_tmp64A[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp64B;_tmp64B.tag=0;_tmp64B.f1=_tmp646;_tmp64B;});_tmp64A;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp64C=_region_malloc(_tmp638,sizeof(*_tmp64C));_tmp64C[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp64D;_tmp64D.tag=5;_tmp64D.f1=_tmp649;_tmp64D;});_tmp64C;}));
goto _LL529;}case 20: _LL52C: _tmp664=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp645)->f1;_LL52D: {
# 2984
struct Cyc_Absyn_Vardecl*_tmp651=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp664 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp651->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp652=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65A=_region_malloc(_tmp638,sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp65B;_tmp65B.tag=2;_tmp65B.f1=_tmp664;_tmp65B.f2=elttype;_tmp65B;});_tmp65A;});
struct Cyc_CfFlowInfo_Place*_tmp653=({struct Cyc_CfFlowInfo_Place*_tmp659=_region_malloc(_tmp638,sizeof(*_tmp659));_tmp659->root=(void*)_tmp652;_tmp659->fields=0;_tmp659;});
_tmp671=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp671,(void*)_tmp652,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
_tmp671=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp671,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp654=_region_malloc(_tmp638,sizeof(*_tmp654));_tmp654[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp655;_tmp655.tag=0;_tmp655.f1=_tmp651;_tmp655;});_tmp654;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp656=_region_malloc(_tmp638,sizeof(*_tmp656));_tmp656[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp657;_tmp657.tag=5;_tmp657.f1=_tmp653;_tmp657;});_tmp656;}));
param_roots=({struct Cyc_List_List*_tmp658=_region_malloc(_tmp638,sizeof(*_tmp658));_tmp658->hd=_tmp653;_tmp658->tl=param_roots;_tmp658;});
goto _LL529;}case 22: _LL52E: _tmp665=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp645)->f1;_LL52F: {
# 2993
struct Cyc_Absyn_Vardecl*_tmp65C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp665 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp65C->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp65D=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp661=_region_malloc(_tmp638,sizeof(*_tmp661));_tmp661[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp662;_tmp662.tag=0;_tmp662.f1=_tmp65C;_tmp662;});_tmp661;});
struct Cyc_CfFlowInfo_Place*_tmp65E=({struct Cyc_CfFlowInfo_Place*_tmp660=_region_malloc(_tmp638,sizeof(*_tmp660));_tmp660->root=(void*)_tmp65D;_tmp660->fields=0;_tmp660;});
noconsume_roots=({struct Cyc_List_List*_tmp65F=_region_malloc(_tmp638,sizeof(*_tmp65F));_tmp65F->hd=_tmp65E;_tmp65F->tl=noconsume_roots;_tmp65F;});}
# 2999
goto _LL529;}default: _LL530: _LL531:
 goto _LL529;}_LL529:;}
# 3002
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp671,_tmp670);{
# 3004
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp638,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp66F=_region_malloc(_tmp638,sizeof(*_tmp66F));_tmp66F->all_tables=tables;_tmp66F->succ_table=
# 3009
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp66F->pat_pop_table=
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp66F->r=_tmp638;_tmp66F->fenv=fenv;_tmp66F->iterate_again=1;_tmp66F->iteration_num=0;_tmp66F->in_try=0;_tmp66F->tryflow=inflow;_tmp66F->all_changed=0;_tmp66F->noreturn=noreturn;_tmp66F->return_type=fd->ret_type;_tmp66F->unique_pat_vars=0;_tmp66F->param_roots=param_roots;_tmp66F->noconsume_params=noconsume_roots;_tmp66F->flow_table=flow_table;_tmp66F->return_relns=fd->ensures_relns;_tmp66F;});
# 3014
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3020
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3023
union Cyc_CfFlowInfo_FlowInfo _tmp666=outflow;if((_tmp666.BottomFL).tag == 1){_LL533: _LL534:
 goto _LL532;}else{_LL535: _LL536:
# 3026
 Cyc_NewControlFlow_check_init_params(_tmp639,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp639);
# 3030
if(noreturn)
({void*_tmp667=0;Cyc_CfFlowInfo_aerr(_tmp639,({const char*_tmp668="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp668,sizeof(char),46);}),_tag_dyneither(_tmp667,sizeof(void*),0));});else{
# 3033
void*_tmp669=Cyc_Tcutil_compress(fd->ret_type);void*_tmp66A=_tmp669;switch(*((int*)_tmp66A)){case 0: _LL538: _LL539:
 goto _LL537;case 7: _LL53A: _LL53B:
 goto _LL53D;case 6: _LL53C: _LL53D:
# 3037
({void*_tmp66B=0;Cyc_Tcutil_warn(_tmp639,({const char*_tmp66C="function may not return a value";_tag_dyneither(_tmp66C,sizeof(char),32);}),_tag_dyneither(_tmp66B,sizeof(void*),0));});goto _LL537;default: _LL53E: _LL53F:
# 3039
({void*_tmp66D=0;Cyc_CfFlowInfo_aerr(_tmp639,({const char*_tmp66E="function may not return a value";_tag_dyneither(_tmp66E,sizeof(char),32);}),_tag_dyneither(_tmp66D,sizeof(void*),0));});goto _LL537;}_LL537:;}
# 3041
goto _LL532;}_LL532:;};};};};};}
# 3045
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp673=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp674=_tmp673;struct Cyc_List_List*_tmp678;struct Cyc_List_List*_tmp677;struct Cyc_List_List*_tmp676;struct Cyc_Absyn_Fndecl*_tmp675;switch(*((int*)_tmp674)){case 1: _LL541: _tmp675=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp674)->f1;_LL542:
 Cyc_NewControlFlow_check_fun(tables,_tmp675);goto _LL540;case 11: _LL543: _tmp676=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp674)->f1;_LL544:
 _tmp677=_tmp676;goto _LL546;case 10: _LL545: _tmp677=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp674)->f2;_LL546:
 _tmp678=_tmp677;goto _LL548;case 9: _LL547: _tmp678=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp674)->f2;_LL548:
 Cyc_NewControlFlow_cf_check(tables,_tmp678);goto _LL540;case 12: _LL549: _LL54A:
 goto _LL540;default: _LL54B: _LL54C:
 goto _LL540;}_LL540:;};}}
