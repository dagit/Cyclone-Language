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
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 110
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1006 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1188
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1190
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1202
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1206
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1208
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
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_PP_Ppstate;
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
enclosee->annot=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;});}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};
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
void*_tmp3=s->annot;void*_tmp4=_tmp3;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp4)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp4)->f1;_LL2:
 return _tmp5;}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_exn_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";_tag_dyneither(_tmp8,sizeof(char),37);});_tmp7;});_tmp6;}));}_LL0:;}
# 144
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[0]=Cyc_CfFlowInfo_BottomFL();_tmp9;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 151
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 154
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpB=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 162
*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpB);
# 168
_tmpA->visited=env->iteration_num;
return({struct _tuple20 _tmpC;_tmpC.f1=_tmpA;_tmpC.f2=_tmpB;_tmpC;});}
# 178
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 186
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 193
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 195
struct _tuple21*_tmpD=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;_LL6: _tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;_LL7:;{
void*_tmp11=root;struct Cyc_Absyn_Vardecl*_tmp12;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11)->tag == 0){_LL9: _tmp12=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11)->f1;_LLA:
# 198
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp10,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpE->fenv)->r,_tmp12->type)){
retry: {void*_tmp13=rval;void*_tmp14;switch(*((int*)_tmp13)){case 8: _LLE: _tmp14=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13)->f2;_LLF:
 rval=_tmp14;goto retry;case 7: _LL10: _LL11:
 goto _LL13;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp13)->f1 == Cyc_CfFlowInfo_NoneIL){_LL12: _LL13:
 goto _LL15;}else{goto _LL16;}case 0: _LL14: _LL15:
 goto _LLD;default: _LL16: _LL17:
# 207
({struct Cyc_String_pa_PrintArg_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp12->name));({void*_tmp15[1]={& _tmp17};Cyc_Tcutil_warn(_tmpF,({const char*_tmp16="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmp16,sizeof(char),64);}),_tag_dyneither(_tmp15,sizeof(void*),1));});});
goto _LLD;}_LLD:;}}
# 211
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}
# 219
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp18=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp19=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1A=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp19);
# 225
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp1B=({struct _tuple19 _tmp21;_tmp21.f1=flow;_tmp21.f2=_tmp1A;_tmp21;});struct _tuple19 _tmp1C=_tmp1B;struct Cyc_Dict_Dict _tmp1D;struct Cyc_Dict_Dict _tmp1E;if(((_tmp1C.f1).ReachableFL).tag == 2){if(((_tmp1C.f2).ReachableFL).tag == 2){_LL19: _tmp1D=(((_tmp1C.f1).ReachableFL).val).f1;_tmp1E=(((_tmp1C.f2).ReachableFL).val).f1;_LL1A: {
# 228
struct _tuple21 _tmp1F=({struct _tuple21 _tmp20;_tmp20.f1=env;_tmp20.f2=s->loc;_tmp20.f3=_tmp1E;_tmp20;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1F,_tmp1D);
goto _LL18;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 234
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1A,*_tmp19)){
# 241
*_tmp19=_tmp1A;
# 245
if(_tmp18->visited == env->iteration_num)
# 247
env->iterate_again=1;}}
# 252
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 256
union Cyc_CfFlowInfo_FlowInfo _tmp22=inflow;struct Cyc_Dict_Dict _tmp23;struct Cyc_List_List*_tmp24;if((_tmp22.BottomFL).tag == 1){_LL1E: _LL1F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL20: _tmp23=((_tmp22.ReachableFL).val).f1;_tmp24=((_tmp22.ReachableFL).val).f2;_LL21:
# 259
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp25=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp29=_region_malloc(fenv->r,sizeof(*_tmp29));_tmp29[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp2A;});_tmp29;});
void*_tmp26=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
_tmp26=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp27=_region_malloc(fenv->r,sizeof(*_tmp27));_tmp27[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp28;_tmp28.tag=8;_tmp28.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp28.f2=_tmp26;_tmp28;});_tmp27;});
# 266
_tmp23=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp23,(void*)_tmp25,_tmp26);}
# 268
return Cyc_CfFlowInfo_ReachableFL(_tmp23,_tmp24);}_LL1D:;}
# 272
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 279
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2B=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp2C=_region_malloc(r,sizeof(*_tmp2C));_tmp2C->hd=_tmp2B;_tmp2C->tl=assume;_tmp2C;})))
return 0;}
# 284
return 1;}
# 287
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2D=e->r;void*_tmp2E=_tmp2D;struct Cyc_Absyn_Exp*_tmp2F;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E)->tag == 13){_LL23: _tmp2F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E)->f2;_LL24:
 return _tmp2F;}else{_LL25: _LL26:
 return e;}_LL22:;}
# 294
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 296
union Cyc_CfFlowInfo_FlowInfo _tmp30=inflow;struct Cyc_List_List*_tmp31;if((_tmp30.BottomFL).tag == 1){_LL28: _LL29:
 return;}else{_LL2A: _tmp31=((_tmp30.ReachableFL).val).f2;_LL2B:
# 299
{void*_tmp32=Cyc_Tcutil_compress(t);void*_tmp33=_tmp32;struct Cyc_List_List*_tmp34;union Cyc_Absyn_AggrInfoU _tmp35;struct Cyc_List_List*_tmp36;switch(*((int*)_tmp33)){case 11: _LL2D: _tmp35=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33)->f1).aggr_info;_tmp36=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33)->f1).targs;_LL2E: {
# 301
struct Cyc_Absyn_Aggrdecl*_tmp37=Cyc_Absyn_get_known_aggrdecl(_tmp35);
struct Cyc_Absyn_Aggrfield*_tmp38=Cyc_Absyn_lookup_decl_field(_tmp37,f);
struct Cyc_Absyn_Exp*_tmp39=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp38))->requires_clause;
if(_tmp39 != 0){
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp3B=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp37->tvs,_tmp36),_tmp39);
# 308
if(!Cyc_NewControlFlow_relns_ok(r,_tmp31,Cyc_Relations_exp2relns(temp,_tmp3B))){
({struct Cyc_String_pa_PrintArg_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3B)));({void*_tmp3C[2]={& _tmp3E,& _tmp3F};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp3D="unable to prove %s, required to access %s";_tag_dyneither(_tmp3D,sizeof(char),42);}),_tag_dyneither(_tmp3C,sizeof(void*),2));});});});
({void*_tmp40=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp41="  [recorded facts on path: ";_tag_dyneither(_tmp41,sizeof(char),28);}),_tag_dyneither(_tmp40,sizeof(void*),0));});
Cyc_Relations_print_relns(Cyc_stderr,_tmp31);
({void*_tmp42=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp43="]\n";_tag_dyneither(_tmp43,sizeof(char),3);}),_tag_dyneither(_tmp42,sizeof(void*),0));});}}
# 306
;_pop_region(temp);}
# 315
goto _LL2C;}case 12: _LL2F: _tmp34=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33)->f2;_LL30: {
# 317
struct Cyc_Absyn_Aggrfield*_tmp44=Cyc_Absyn_lookup_field(_tmp34,f);
struct Cyc_Absyn_Exp*_tmp45=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp44))->requires_clause;
if(_tmp45 != 0){
struct _RegionHandle _tmp46=_new_region("temp");struct _RegionHandle*temp=& _tmp46;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp31,Cyc_Relations_exp2relns(temp,_tmp45))){
({struct Cyc_String_pa_PrintArg_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp49;_tmp49.tag=0;_tmp49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp45)));({void*_tmp47[2]={& _tmp49,& _tmp4A};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp48="unable to prove %s, required to access %s";_tag_dyneither(_tmp48,sizeof(char),42);}),_tag_dyneither(_tmp47,sizeof(void*),2));});});});
({void*_tmp4B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp4C="  [recorded facts on path: ";_tag_dyneither(_tmp4C,sizeof(char),28);}),_tag_dyneither(_tmp4B,sizeof(void*),0));});
Cyc_Relations_print_relns(Cyc_stderr,_tmp31);
({void*_tmp4D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp4E="]\n";_tag_dyneither(_tmp4E,sizeof(char),3);}),_tag_dyneither(_tmp4D,sizeof(void*),0));});}
# 321
;_pop_region(temp);}
# 328
goto _LL2C;}default: _LL31: _LL32:
 goto _LL2C;}_LL2C:;}
# 331
goto _LL27;}_LL27:;}
# 335
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4F=inflow;struct Cyc_Dict_Dict _tmp50;struct Cyc_List_List*_tmp51;if((_tmp4F.BottomFL).tag == 1){_LL34: _LL35:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL36: _tmp50=((_tmp4F.ReachableFL).val).f1;_tmp51=((_tmp4F.ReachableFL).val).f2;_LL37:
# 339
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp50,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp52=0;Cyc_CfFlowInfo_aerr(loc,({const char*_tmp53="expression may not be fully initialized";_tag_dyneither(_tmp53,sizeof(char),40);}),_tag_dyneither(_tmp52,sizeof(void*),0));});{
struct Cyc_Dict_Dict _tmp54=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp50,env->all_changed,r);
if(_tmp50.t == _tmp54.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp55=Cyc_CfFlowInfo_ReachableFL(_tmp54,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp55);
return _tmp55;};};}_LL33:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 349
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp56=({struct _tuple0 _tmp69;_tmp69.f1=Cyc_Tcutil_compress(t);_tmp69.f2=r;_tmp69;});struct _tuple0 _tmp57=_tmp56;enum Cyc_Absyn_AggrKind _tmp58;struct Cyc_List_List*_tmp59;struct _dyneither_ptr _tmp5A;union Cyc_Absyn_AggrInfoU _tmp5B;struct Cyc_List_List*_tmp5C;struct _dyneither_ptr _tmp5D;struct Cyc_List_List*_tmp5E;struct _dyneither_ptr _tmp5F;struct Cyc_Absyn_Datatypefield*_tmp60;struct _dyneither_ptr _tmp61;void*_tmp62;switch(*((int*)_tmp57.f2)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL39: _LL3A:
 return;}else{switch(*((int*)_tmp57.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL4B;else{goto _LL4B;}case 10: goto _LL4B;case 11: goto _LL4B;case 12: goto _LL4B;case 5: goto _LL49;default: goto _LL4B;}}case 0: _LL3B: _LL3C:
 return;case 7: _LL3D: _LL3E:
 return;case 8: _LL3F: _tmp62=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL40:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp62);return;default: switch(*((int*)_tmp57.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL41: _tmp60=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp61=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL42:
# 356
 if(_tmp60->typs == 0)
return;
_tmp5E=_tmp60->typs;_tmp5F=_tmp61;goto _LL44;}else{goto _LL4B;}}else{goto _LL4B;}case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL43: _tmp5E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57.f1)->f1;_tmp5F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL44: {
# 360
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5E);
{int i=0;for(0;i < sz;(i ++,_tmp5E=_tmp5E->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5E))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp5F,sizeof(void*),i)));}}
# 364
return;}}else{goto _LL4B;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL45: _tmp5B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp57.f1)->f1).aggr_info;_tmp5C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp57.f1)->f1).targs;_tmp5D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL46: {
# 367
struct Cyc_Absyn_Aggrdecl*_tmp63=Cyc_Absyn_get_known_aggrdecl(_tmp5B);
if(_tmp63->impl == 0)return;{
struct Cyc_List_List*_tmp64=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->fields;
struct _RegionHandle _tmp65=_new_region("temp");struct _RegionHandle*temp=& _tmp65;_push_region(temp);
{struct Cyc_List_List*_tmp66=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp63->tvs,_tmp5C);
{int i=0;for(0;i < _get_dyneither_size(_tmp5D,sizeof(void*));(i ++,_tmp64=_tmp64->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp64))->hd)->type;
if(_tmp66 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp66,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp5D.curr)[i]);}}
# 377
_npop_handler(0);return;}
# 371
;_pop_region(temp);};}}else{goto _LL4B;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL47: _tmp58=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57.f1)->f1;_tmp59=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57.f1)->f2;_tmp5A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL48:
# 380
{int i=0;for(0;i < _get_dyneither_size(_tmp5A,sizeof(void*));(i ++,_tmp59=_tmp59->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp59))->hd)->type,((void**)_tmp5A.curr)[i]);}}
return;}else{goto _LL4B;}case 5: _LL49: if(
Cyc_Tcutil_is_noalias_pointer(t,0)){_LL4A:
({void*_tmp67=0;Cyc_Tcutil_warn(loc,({const char*_tmp68="argument may still contain alias-free pointers";_tag_dyneither(_tmp68,sizeof(char),47);}),_tag_dyneither(_tmp67,sizeof(void*),0));});
return;}else{goto _LL4B;}default: _LL4B: _LL4C:
 return;}}_LL38:;}
# 390
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp6A=inflow;struct Cyc_Dict_Dict _tmp6B;struct Cyc_List_List*_tmp6C;if((_tmp6A.BottomFL).tag == 1){_LL4E: _LL4F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL50: _tmp6B=((_tmp6A.ReachableFL).val).f1;_tmp6C=((_tmp6A.ReachableFL).val).f2;_LL51:
# 394
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp6D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6E="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp6E,sizeof(char),51);}),_tag_dyneither(_tmp6D,sizeof(void*),0));});
return Cyc_CfFlowInfo_BottomFL();}{
# 398
void*_tmp6F=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp70=r;struct Cyc_CfFlowInfo_Place*_tmp71;void*_tmp72;switch(*((int*)_tmp70)){case 8: _LL53: _tmp72=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp70)->f2;_LL54:
 r=_tmp72;goto retry;case 5: _LL55: _tmp71=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp70)->f1;_LL56:
# 402
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6F,Cyc_CfFlowInfo_lookup_place(_tmp6B,_tmp71));
goto _LL52;default: _LL57: _LL58:
# 405
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp6F))
({void*_tmp73=0;Cyc_Tcutil_warn(loc,({const char*_tmp74="argument may contain live alias-free pointers";_tag_dyneither(_tmp74,sizeof(char),46);}),_tag_dyneither(_tmp73,sizeof(void*),0));});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL52:;}{
# 409
struct Cyc_Dict_Dict _tmp75=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp6B,env->all_changed,r);
if(_tmp6B.t == _tmp75.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp76=Cyc_CfFlowInfo_ReachableFL(_tmp75,_tmp6C);
Cyc_NewControlFlow_update_tryflow(env,_tmp76);
return _tmp76;};};};}_LL4D:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 418
static struct _tuple23 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 425
if(es == 0)
return({struct _tuple23 _tmp77;_tmp77.f1=inflow;_tmp77.f2=0;_tmp77;});
if(es->tl == 0){
struct _tuple16 _tmp78=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp79=_tmp78;union Cyc_CfFlowInfo_FlowInfo _tmp7A;void*_tmp7B;_LL5A: _tmp7A=_tmp79.f1;_tmp7B=_tmp79.f2;_LL5B:;
return({struct _tuple23 _tmp7C;_tmp7C.f1=_tmp7A;_tmp7C.f2=({struct Cyc_List_List*_tmp7D=_region_malloc(rgn,sizeof(*_tmp7D));_tmp7D->hd=_tmp7B;_tmp7D->tl=0;_tmp7D;});_tmp7C;});}{
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
env->all_changed=({struct Cyc_Dict_Dict*_tmp7E=_region_malloc(env->r,sizeof(*_tmp7E));_tmp7E[0]=(env->fenv)->mt_place_set;_tmp7E;});{
struct _tuple16 _tmp7F=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp80=_tmp7F;union Cyc_CfFlowInfo_FlowInfo _tmp81;void*_tmp82;_LL5D: _tmp81=_tmp80.f1;_tmp82=_tmp80.f2;_LL5E:;
outflow=_tmp81;
rvals=({struct Cyc_List_List*_tmp83=_region_malloc(rgn,sizeof(*_tmp83));_tmp83->hd=_tmp82;_tmp83->tl=0;_tmp83;});
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 448
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
env->all_changed=({struct Cyc_Dict_Dict*_tmp84=_region_malloc(env->r,sizeof(*_tmp84));_tmp84[0]=(env->fenv)->mt_place_set;_tmp84;});{
struct _tuple16 _tmp85=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);struct _tuple16 _tmp86=_tmp85;union Cyc_CfFlowInfo_FlowInfo _tmp87;void*_tmp88;_LL60: _tmp87=_tmp86.f1;_tmp88=_tmp86.f2;_LL61:;
rvals=({struct Cyc_List_List*_tmp89=_region_malloc(rgn,sizeof(*_tmp89));_tmp89->hd=_tmp88;_tmp89->tl=rvals;_tmp89;});
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp87,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
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
env->all_changed=({struct Cyc_Dict_Dict*_tmp8A=_region_malloc(env->r,sizeof(*_tmp8A));_tmp8A[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0);_tmp8A;});}
# 471
Cyc_NewControlFlow_update_tryflow(env,outflow);
# 473
return({struct _tuple23 _tmp8B;_tmp8B.f1=outflow;_tmp8B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp8B;});};}
# 478
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 481
struct _RegionHandle _tmp8C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8C;_push_region(rgn);
{struct _tuple23 _tmp8D=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 482
struct _tuple23 _tmp8E=_tmp8D;union Cyc_CfFlowInfo_FlowInfo _tmp8F;struct Cyc_List_List*_tmp90;_LL63: _tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;_LL64:;
# 484
{union Cyc_CfFlowInfo_FlowInfo _tmp91=_tmp8F;struct Cyc_Dict_Dict _tmp92;if((_tmp91.ReachableFL).tag == 2){_LL66: _tmp92=((_tmp91.ReachableFL).val).f1;_LL67:
# 486
 for(0;_tmp90 != 0;(_tmp90=_tmp90->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)_tmp90->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp93=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp94="expression may not be initialized";_tag_dyneither(_tmp94,sizeof(char),34);}),_tag_dyneither(_tmp93,sizeof(void*),0));});}
goto _LL65;}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}{
# 492
struct _tuple16 _tmp96=({struct _tuple16 _tmp95;_tmp95.f1=_tmp8F;_tmp95.f2=(env->fenv)->unknown_all;_tmp95;});_npop_handler(0);return _tmp96;};}
# 482
;_pop_region(rgn);}
# 500
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 507
int needs_unconsume=0;
void*_tmp97=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp97,& needs_unconsume))
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Core_Impossible_exn_struct _tmp99;_tmp99.tag=Cyc_Core_Impossible;_tmp99.f1=({const char*_tmp9A="consume_zero_ral";_tag_dyneither(_tmp9A,sizeof(char),17);});_tmp99;});_tmp98;}));else{
# 512
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 516
return new_rval;}}}
# 529 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 535
union Cyc_CfFlowInfo_FlowInfo _tmp9B=outflow;struct Cyc_Dict_Dict _tmp9C;struct Cyc_List_List*_tmp9D;if((_tmp9B.BottomFL).tag == 1){_LL6B: _LL6C:
 return outflow;}else{_LL6D: _tmp9C=((_tmp9B.ReachableFL).val).f1;_tmp9D=((_tmp9B.ReachableFL).val).f2;_LL6E: {
# 538
union Cyc_CfFlowInfo_AbsLVal _tmp9E=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp9F=_tmp9E;struct Cyc_CfFlowInfo_Place*_tmpA0;if((_tmp9F.UnknownL).tag == 2){_LL70: _LL71:
# 542
 return outflow;}else{_LL72: _tmpA0=(_tmp9F.PlaceL).val;_LL73: {
# 546
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpA1=_region_malloc((env->fenv)->r,sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpA2;_tmpA2.tag=8;_tmpA2.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmpA2.f2=nzval;_tmpA2;});_tmpA1;});}
# 550
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9C,_tmpA0,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpA3=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9C,env->all_changed,_tmpA0,nzval),_tmp9D);
# 555
return _tmpA3;};}}_LL6F:;}}_LL6A:;}
# 564
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 567
union Cyc_CfFlowInfo_FlowInfo _tmpA4=outflow;struct Cyc_Dict_Dict _tmpA5;struct Cyc_List_List*_tmpA6;if((_tmpA4.BottomFL).tag == 1){_LL75: _LL76:
 return({struct _tuple19 _tmpA7;_tmpA7.f1=outflow;_tmpA7.f2=outflow;_tmpA7;});}else{_LL77: _tmpA5=((_tmpA4.ReachableFL).val).f1;_tmpA6=((_tmpA4.ReachableFL).val).f2;_LL78: {
# 570
union Cyc_CfFlowInfo_AbsLVal _tmpA8=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpA9=_tmpA8;struct Cyc_CfFlowInfo_Place*_tmpAA;if((_tmpA9.UnknownL).tag == 2){_LL7A: _LL7B:
 return({struct _tuple19 _tmpAB;_tmpAB.f1=outflow;_tmpAB.f2=outflow;_tmpAB;});}else{_LL7C: _tmpAA=(_tmpA9.PlaceL).val;_LL7D: {
# 573
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpAC=_region_malloc((env->fenv)->r,sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpAD;_tmpAD.tag=8;_tmpAD.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmpAD.f2=nzval;_tmpAD;});_tmpAC;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpAE=_region_malloc((env->fenv)->r,sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpAF;_tmpAF.tag=8;_tmpAF.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmpAF.f2=zval;_tmpAF;});_tmpAE;});}
# 579
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA5,_tmpAA,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA5,_tmpAA,e,zval);
return({struct _tuple19 _tmpB0;_tmpB0.f1=
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA5,env->all_changed,_tmpAA,nzval),_tmpA6);_tmpB0.f2=
# 584
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA5,env->all_changed,_tmpAA,zval),_tmpA6);_tmpB0;});}}_LL79:;}}_LL74:;}
# 590
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 598
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 602
void*_tmpB3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpB4=_tmpB3;void*_tmpB5;union Cyc_Absyn_Constraint*_tmpB6;union Cyc_Absyn_Constraint*_tmpB7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->tag == 5){_LL7F: _tmpB5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->f1).elt_typ;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->f1).ptr_atts).bounds;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->f1).ptr_atts).zero_term;_LL80: {
# 604
union Cyc_CfFlowInfo_FlowInfo _tmpB8=f;struct Cyc_Dict_Dict _tmpB9;struct Cyc_List_List*_tmpBA;if((_tmpB8.BottomFL).tag == 1){_LL84: _LL85:
# 606
 return({struct _tuple16 _tmpBB;_tmpBB.f1=f;_tmpBB.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_all);_tmpBB;});}else{_LL86: _tmpB9=((_tmpB8.ReachableFL).val).f1;_tmpBA=((_tmpB8.ReachableFL).val).f2;_LL87:
# 609
 if(Cyc_Tcutil_is_bound_one(_tmpB6)){
struct _tuple15 _tmpBC=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpBD=_tmpBC;void*_tmpBE;struct Cyc_List_List*_tmpBF;_LL89: _tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;_LL8A:;{
void*_tmpC0=_tmpBE;enum Cyc_CfFlowInfo_InitLevel _tmpC1;struct Cyc_CfFlowInfo_Place*_tmpC2;struct Cyc_Absyn_Vardecl*_tmpC3;void*_tmpC4;switch(*((int*)_tmpC0)){case 8: _LL8C: _tmpC3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;_tmpC4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f2;_LL8D:
# 613
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Core_Impossible_exn_struct _tmpC6;_tmpC6.tag=Cyc_Core_Impossible;_tmpC6.f1=({const char*_tmpC7="named location in anal_derefR";_tag_dyneither(_tmpC7,sizeof(char),30);});_tmpC6;});_tmpC5;}));case 1: _LL8E: _LL8F:
 goto _LL91;case 2: _LL90: _LL91:
# 616
{void*_tmpC8=e->annot;void*_tmpC9=_tmpC8;struct Cyc_List_List*_tmpCA;if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpC9)->tag == Cyc_CfFlowInfo_NotZero){_LL9B: _tmpCA=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpC9)->f1;_LL9C:
# 618
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpCA))goto _LL9E;
goto _LL9A;}else{_LL9D: _LL9E:
# 622
{void*_tmpCB=e->r;void*_tmpCC=_tmpCB;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCC)->tag == 22){_LLA0: _LLA1:
# 624
 e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpCE;_tmpCE.tag=Cyc_CfFlowInfo_NotZero;_tmpCE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA);_tmpCE;});_tmpCD;});
goto _LL9F;}else{_LLA2: _LLA3:
# 627
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL9F;}_LL9F:;}
# 630
goto _LL9A;}_LL9A:;}
# 632
goto _LL8B;case 5: _LL92: _tmpC2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;_LL93: {
# 636
int possibly_null=0;
{void*_tmpCF=e->annot;void*_tmpD0=_tmpCF;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD2;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpD0)->tag == Cyc_CfFlowInfo_UnknownZ){_LLA5: _tmpD2=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpD0)->f1;_LLA6:
# 639
 possibly_null=1;
_tmpD1=_tmpD2;goto _LLA8;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD0)->tag == Cyc_CfFlowInfo_NotZero){_LLA7: _tmpD1=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD0)->f1;_LLA8:
# 642
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpD1))goto _LLAA;
goto _LLA4;}else{_LLA9: _LLAA:
# 646
{void*_tmpD3=e->r;void*_tmpD4=_tmpD3;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD4)->tag == 22){_LLAC: _LLAD:
# 648
 if(possibly_null)
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpD6;_tmpD6.tag=Cyc_CfFlowInfo_UnknownZ;_tmpD6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA);_tmpD6;});_tmpD5;});else{
# 651
e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpD8;_tmpD8.tag=Cyc_CfFlowInfo_NotZero;_tmpD8.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA);_tmpD8;});_tmpD7;});}
goto _LLAB;}else{_LLAE: _LLAF:
# 654
 if(possibly_null)
e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;else{
# 657
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;}
goto _LLAB;}_LLAB:;}
# 660
goto _LLA4;}}_LLA4:;}
# 662
return({struct _tuple16 _tmpD9;_tmpD9.f1=f;_tmpD9.f2=Cyc_CfFlowInfo_lookup_place(_tmpB9,_tmpC2);_tmpD9;});}case 0: _LL94: _LL95:
# 664
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpDA;_tmpDA.f1=Cyc_CfFlowInfo_BottomFL();_tmpDA.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_all);_tmpDA;});case 3: _LL96: _tmpC1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;_LL97:
# 667
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC1,_tmpBF);goto _LL99;default: _LL98: _LL99:
# 669
{void*_tmpDB=e->r;void*_tmpDC=_tmpDB;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC)->tag == 22){_LLB1: _LLB2:
# 671
 e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpDE;_tmpDE.tag=Cyc_CfFlowInfo_UnknownZ;_tmpDE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA);_tmpDE;});_tmpDD;});
goto _LLB0;}else{_LLB3: _LLB4:
# 674
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LLB0;}_LLB0:;}
# 677
goto _LL8B;}_LL8B:;};}else{
# 680
void*_tmpDF=e->annot;void*_tmpE0=_tmpDF;struct Cyc_List_List*_tmpE1;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE0)->tag == Cyc_CfFlowInfo_UnknownZ){_LLB6: _tmpE1=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE0)->f1;_LLB7:
# 682
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpE1))goto _LLB9;
goto _LLB5;}else{_LLB8: _LLB9:
# 685
 e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpE3;_tmpE3.tag=Cyc_CfFlowInfo_UnknownZ;_tmpE3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA);_tmpE3;});_tmpE2;});
goto _LLB5;}_LLB5:;}{
# 689
enum Cyc_CfFlowInfo_InitLevel _tmpE4=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpB9,r);enum Cyc_CfFlowInfo_InitLevel _tmpE5=_tmpE4;switch(_tmpE5){case Cyc_CfFlowInfo_NoneIL: _LLBB: _LLBC: {
# 691
struct _tuple15 _tmpE6=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpE7=_tmpE6;void*_tmpE8;_LLC2: _tmpE8=_tmpE7.f1;_LLC3:;
{void*_tmpE9=_tmpE8;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE9)->tag == 7){_LLC5: _LLC6:
# 694
({void*_tmpEA=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmpEB="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpEB,sizeof(char),53);}),_tag_dyneither(_tmpEA,sizeof(void*),0));});
goto _LLC4;}else{_LLC7: _LLC8:
# 697
({void*_tmpEC=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmpED="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpED,sizeof(char),46);}),_tag_dyneither(_tmpEC,sizeof(void*),0));});}_LLC4:;}
# 699
goto _LLBE;}case Cyc_CfFlowInfo_AllIL: _LLBD: _LLBE:
# 701
 return({struct _tuple16 _tmpEE;_tmpEE.f1=f;_tmpEE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_all);_tmpEE;});default: _LLBF: _LLC0:
# 703
 return({struct _tuple16 _tmpEF;_tmpEF.f1=f;_tmpEF.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_none);_tmpEF;});}_LLBA:;};}_LL83:;}}else{_LL81: _LL82:
# 706
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Core_Impossible_exn_struct _tmpF1;_tmpF1.tag=Cyc_Core_Impossible;_tmpF1.f1=({const char*_tmpF2="right deref of non-pointer-type";_tag_dyneither(_tmpF2,sizeof(char),32);});_tmpF1;});_tmpF0;}));}_LL7E:;}
# 713
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 716
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 719
{void*_tmpF3=e1->r;void*_tmpF4=_tmpF3;void*_tmpF5;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF4)->tag == 1){_LLCA: _tmpF5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LLCB: {
# 721
struct Cyc_Absyn_Vardecl*_tmpF6=Cyc_Tcutil_nonesc_vardecl(_tmpF5);
if(_tmpF6 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpF6);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 727
goto _LLC9;}}else{_LLCC: _LLCD:
 goto _LLC9;}_LLC9:;}{
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
struct _tuple18 _tmpF7=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpF8=_tmpF7;union Cyc_CfFlowInfo_AbsLVal _tmpF9;_LLCF: _tmpF9=_tmpF8.f2;_LLD0:;
{struct _tuple18 _tmpFA=({struct _tuple18 _tmp108;_tmp108.f1=inflow;_tmp108.f2=_tmpF9;_tmp108;});struct _tuple18 _tmpFB=_tmpFA;struct Cyc_Dict_Dict _tmpFC;struct Cyc_List_List*_tmpFD;struct Cyc_CfFlowInfo_Place*_tmpFE;if(((_tmpFB.f1).ReachableFL).tag == 2){if(((_tmpFB.f2).PlaceL).tag == 1){_LLD2: _tmpFC=(((_tmpFB.f1).ReachableFL).val).f1;_tmpFD=(((_tmpFB.f1).ReachableFL).val).f2;_tmpFE=((_tmpFB.f2).PlaceL).val;_LLD3: {
# 753
void*_tmpFF=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 755
struct _tuple15 _tmp100=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);struct _tuple15 _tmp101=_tmp100;void*_tmp102;struct Cyc_List_List*_tmp103;_LLD9: _tmp102=_tmp101.f1;_tmp103=_tmp101.f2;_LLDA:;
for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
_tmpFF=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp104=_region_malloc((env->fenv)->r,sizeof(*_tmp104));_tmp104[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp105;_tmp105.tag=8;_tmp105.f1=(struct Cyc_Absyn_Vardecl*)_tmp103->hd;_tmp105.f2=_tmpFF;_tmp105;});_tmp104;});}
# 760
_tmpFC=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpFC,env->all_changed,_tmpFE,_tmpFF);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpFC,_tmpFD);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 766
goto _LLD1;}}else{_LLD6: _LLD7:
# 769
({void*_tmp106=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp107="noconsume() parameters must be l-values";_tag_dyneither(_tmp107,sizeof(char),40);}),_tag_dyneither(_tmp106,sizeof(void*),0));});
goto _LLD1;}}else{_LLD4: _LLD5:
# 767
 goto _LLD1;}_LLD1:;}
# 772
return inflow;}
# 777
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 785
union Cyc_CfFlowInfo_FlowInfo _tmp109=outflow;struct Cyc_Dict_Dict _tmp10A;struct Cyc_List_List*_tmp10B;if((_tmp109.BottomFL).tag == 1){_LLDC: _LLDD:
# 787
 return({struct _tuple16 _tmp10C;_tmp10C.f1=Cyc_CfFlowInfo_BottomFL();_tmp10C.f2=rval;_tmp10C;});}else{_LLDE: _tmp10A=((_tmp109.ReachableFL).val).f1;_tmp10B=((_tmp109.ReachableFL).val).f2;_LLDF: {
# 789
union Cyc_CfFlowInfo_AbsLVal _tmp10D=lval;struct Cyc_CfFlowInfo_Place*_tmp10E;if((_tmp10D.PlaceL).tag == 1){_LLE1: _tmp10E=(_tmp10D.PlaceL).val;_LLE2: {
# 791
struct Cyc_Dict_Dict _tmp10F=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp10A,env->all_changed,_tmp10E,rval);
_tmp10B=Cyc_Relations_reln_assign_exp(fenv->r,_tmp10B,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp10F,_tmp10B);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 797
struct _tuple15 _tmp110=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp10A,_tmp10E));struct _tuple15 _tmp111=_tmp110;void*_tmp112;_LLE6: _tmp112=_tmp111.f1;_LLE7:;{
void*_tmp113=_tmp112;switch(*((int*)_tmp113)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp113)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE9: _LLEA:
 goto _LLEC;}else{goto _LLEF;}case 7: _LLEB: _LLEC:
 goto _LLEE;case 0: _LLED: _LLEE:
 goto _LLE8;default: _LLEF: _LLF0:
# 803
({void*_tmp114=0;Cyc_Tcutil_warn(lexp->loc,({const char*_tmp115="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmp115,sizeof(char),47);}),_tag_dyneither(_tmp114,sizeof(void*),0));});
goto _LLE8;}_LLE8:;};}
# 808
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmp116;_tmp116.f1=outflow;_tmp116.f2=rval;_tmp116;});}}else{_LLE3: _LLE4:
# 811
 return({struct _tuple16 _tmp117;_tmp117.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp117.f2=rval;_tmp117;});}_LLE0:;}}_LLDB:;}
# 818
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 824
union Cyc_CfFlowInfo_FlowInfo _tmp118=f;struct Cyc_Dict_Dict _tmp119;struct Cyc_List_List*_tmp11A;if((_tmp118.BottomFL).tag == 1){_LLF2: _LLF3:
 return Cyc_CfFlowInfo_BottomFL();}else{_LLF4: _tmp119=((_tmp118.ReachableFL).val).f1;_tmp11A=((_tmp118.ReachableFL).val).f2;_LLF5:
# 829
 _tmp119=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp119,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp11B=_region_malloc(env->r,sizeof(*_tmp11B));_tmp11B->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp11C=_region_malloc(env->r,sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=vd;_tmp11D;});_tmp11C;});_tmp11B->fields=0;_tmp11B;}),rval);
# 832
_tmp11A=Cyc_Relations_reln_assign_var(env->r,_tmp11A,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp11E=Cyc_CfFlowInfo_ReachableFL(_tmp119,_tmp11A);
Cyc_NewControlFlow_update_tryflow(env,_tmp11E);
# 837
return _tmp11E;};}_LLF1:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 841
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 848
if(vds == 0)return inflow;{
# 851
struct Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)es=({struct Cyc_List_List*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp120->tl=es;_tmp120;});}}
# 857
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11F,fenv->unknown_all,pat_loc,name_locs);
# 859
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 863
struct _tuple16 _tmp121=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp122=_tmp121;union Cyc_CfFlowInfo_FlowInfo _tmp123;void*_tmp124;_LLF7: _tmp123=_tmp122.f1;_tmp124=_tmp122.f2;_LLF8:;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp123,_tmp124);}}{
# 871
struct Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp125 != 0;_tmp125=_tmp125->tl){
struct _tuple24*_tmp126=(struct _tuple24*)_tmp125->hd;struct _tuple24*_tmp127=_tmp126;struct Cyc_Absyn_Vardecl**_tmp128;struct Cyc_Absyn_Exp*_tmp129;_LLFA: _tmp128=_tmp127->f1;_tmp129=_tmp127->f2;_LLFB:;
if(_tmp128 != 0  && _tmp129 != 0){
if(_tmp129->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp129));({void*_tmp12A[1]={& _tmp12C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp12B="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp12B,sizeof(char),41);}),_tag_dyneither(_tmp12A,sizeof(void*),1));});});{
# 885
struct Cyc_List_List l=({struct Cyc_List_List _tmp142;_tmp142.hd=_tmp129;_tmp142.tl=0;_tmp142;});
union Cyc_CfFlowInfo_FlowInfo _tmp12D=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp12E=Cyc_NewControlFlow_anal_Lexp(env,_tmp12D,0,0,_tmp129);struct _tuple18 _tmp12F=_tmp12E;union Cyc_CfFlowInfo_AbsLVal _tmp130;_LLFD: _tmp130=_tmp12F.f2;_LLFE:;{
struct _tuple16 _tmp131=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp12D,_tmp129);struct _tuple16 _tmp132=_tmp131;union Cyc_CfFlowInfo_FlowInfo _tmp133;void*_tmp134;_LL100: _tmp133=_tmp132.f1;_tmp134=_tmp132.f2;_LL101:;{
union Cyc_CfFlowInfo_FlowInfo _tmp135=_tmp133;struct Cyc_Dict_Dict _tmp136;struct Cyc_List_List*_tmp137;if((_tmp135.ReachableFL).tag == 2){_LL103: _tmp136=((_tmp135.ReachableFL).val).f1;_tmp137=((_tmp135.ReachableFL).val).f2;_LL104:
# 891
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp138=_tmp130;struct Cyc_CfFlowInfo_Place*_tmp139;if((_tmp138.PlaceL).tag == 1){_LL108: _tmp139=(_tmp138.PlaceL).val;_LL109:
# 894
 _tmp134=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp13A=_region_malloc(fenv->r,sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp13B;_tmp13B.tag=8;_tmp13B.f1=*_tmp128;_tmp13B.f2=_tmp134;_tmp13B;});_tmp13A;});{
# 897
void*_tmp13C=Cyc_CfFlowInfo_lookup_place(_tmp136,_tmp139);
_tmp13C=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp13D=_region_malloc(fenv->r,sizeof(*_tmp13D));_tmp13D[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp13E;_tmp13E.tag=8;_tmp13E.f1=*_tmp128;_tmp13E.f2=_tmp13C;_tmp13E;});_tmp13D;});
_tmp136=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp136,env->all_changed,_tmp139,_tmp13C);
_tmp133=Cyc_CfFlowInfo_ReachableFL(_tmp136,_tmp137);
goto _LL107;};}else{_LL10A: _LL10B:
# 904
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp129->topt)) && !
# 906
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp128)->type))
# 908
({struct Cyc_String_pa_PrintArg_struct _tmp141;_tmp141.tag=0;_tmp141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp129));({void*_tmp13F[1]={& _tmp141};Cyc_CfFlowInfo_aerr(exp_loc,({const char*_tmp140="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp140,sizeof(char),46);}),_tag_dyneither(_tmp13F,sizeof(void*),1));});});}_LL107:;}
# 916
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp133,*_tmp128,_tmp129,_tmp134,pat_loc);
goto _LL102;}else{_LL105: _LL106:
# 919
 goto _LL102;}_LL102:;};};};}}
# 924
return inflow;};};}
# 927
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 929
void*_tmp143=e->r;void*_tmp144=_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp149;switch(*((int*)_tmp144)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp144)->f2)){case 4: _LL10D: _LL10E:
 goto _LL110;case 3: _LL10F: _LL110:
 goto _LL112;case 5: _LL111: _LL112:
 return 1;default: goto _LL11D;}case 19: _LL113: _tmp149=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LL114:
 _tmp148=_tmp149;goto _LL116;case 20: _LL115: _tmp148=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LL116:
 _tmp147=_tmp148;goto _LL118;case 21: _LL117: _tmp147=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LL118:
# 936
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp147,cast_ok);case 22: _LL119: _tmp146=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LL11A: {
# 938
void*_tmp14A=Cyc_Tcutil_compress((void*)_check_null(_tmp146->topt));void*_tmp14B=_tmp14A;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14B)->tag == 10){_LL120: _LL121:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp146,cast_ok);}else{_LL122: _LL123:
 return 0;}_LL11F:;}case 13: _LL11B: _tmp145=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp144)->f2;_LL11C:
# 943
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp145,cast_ok);else{
return 0;}default: _LL11D: _LL11E:
 return 0;}_LL10C:;}
# 949
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp14C=*rop;union Cyc_Relations_RelnOp _tmp14D=_tmp14C;unsigned int _tmp14E;unsigned int _tmp14F;switch((_tmp14D.RParamNumelts).tag){case 5: _LL125: _tmp14F=(_tmp14D.RParam).val;_LL126: {
# 952
struct Cyc_Absyn_Exp*_tmp150=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp14F);
return Cyc_Relations_exp2relnop(_tmp150,rop);}case 6: _LL127: _tmp14E=(_tmp14D.RParamNumelts).val;_LL128: {
# 955
struct Cyc_Absyn_Exp*_tmp151=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp14E);
return Cyc_Relations_exp2relnop(Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp151,0),rop);}default: _LL129: _LL12A:
 return 1;}_LL124:;}
# 961
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp152=rop;unsigned int _tmp153;unsigned int _tmp154;switch((_tmp152.RParamNumelts).tag){case 5: _LL12C: _tmp154=(_tmp152.RParam).val;_LL12D:
# 964
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp154));case 6: _LL12E: _tmp153=(_tmp152.RParamNumelts).val;_LL12F:
# 966
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp153));default: _LL130: _LL131:
 return Cyc_Relations_rop2string(rop);}_LL12B:;}
# 971
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 974
union Cyc_CfFlowInfo_FlowInfo _tmp155=inflow;struct Cyc_Dict_Dict _tmp156;struct Cyc_List_List*_tmp157;if((_tmp155.BottomFL).tag == 1){_LL133: _LL134:
 return;}else{_LL135: _tmp156=((_tmp155.ReachableFL).val).f1;_tmp157=((_tmp155.ReachableFL).val).f2;_LL136:
# 977
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp158=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp158->rop1;
union Cyc_Relations_RelnOp rop2=_tmp158->rop2;
enum Cyc_Relations_Relation _tmp159=Cyc_Relations_flip_relation(_tmp158->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(env->r,rop2,_tmp159,rop1,_tmp157))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp158->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp157));({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp15A[4]={& _tmp15C,& _tmp15D,& _tmp15E,& _tmp15F};Cyc_Tcutil_terr(loc,({const char*_tmp15B="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp15B,sizeof(char),77);}),_tag_dyneither(_tmp15A,sizeof(void*),4));});});});});});
break;}}
# 992
goto _LL132;}_LL132:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 997
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1001
struct Cyc_CfFlowInfo_FlowEnv*_tmp160=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 1014
{union Cyc_CfFlowInfo_FlowInfo _tmp161=inflow;struct Cyc_Dict_Dict _tmp162;struct Cyc_List_List*_tmp163;if((_tmp161.BottomFL).tag == 1){_LL138: _LL139:
 return({struct _tuple16 _tmp164;_tmp164.f1=Cyc_CfFlowInfo_BottomFL();_tmp164.f2=_tmp160->unknown_all;_tmp164;});}else{_LL13A: _tmp162=((_tmp161.ReachableFL).val).f1;_tmp163=((_tmp161.ReachableFL).val).f2;_LL13B:
 d=_tmp162;relns=_tmp163;}_LL137:;}
# 1029 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 1050 "new_control_flow.cyc"
struct _tuple18 _tmp165=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp166=_tmp165;union Cyc_CfFlowInfo_FlowInfo _tmp167;union Cyc_CfFlowInfo_AbsLVal _tmp168;_LL13D: _tmp167=_tmp166.f1;_tmp168=_tmp166.f2;_LL13E:;{
struct _tuple18 _tmp169=({struct _tuple18 _tmp176;_tmp176.f1=_tmp167;_tmp176.f2=_tmp168;_tmp176;});struct _tuple18 _tmp16A=_tmp169;struct Cyc_Dict_Dict _tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_CfFlowInfo_Place*_tmp16D;if(((_tmp16A.f1).ReachableFL).tag == 2){if(((_tmp16A.f2).PlaceL).tag == 1){_LL140: _tmp16B=(((_tmp16A.f1).ReachableFL).val).f1;_tmp16C=(((_tmp16A.f1).ReachableFL).val).f2;_tmp16D=((_tmp16A.f2).PlaceL).val;_LL141: {
# 1053
void*_tmp16E=Cyc_CfFlowInfo_lookup_place(_tmp16B,_tmp16D);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp16E,& needs_unconsume)){
({void*_tmp16F=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp170="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp170,sizeof(char),56);}),_tag_dyneither(_tmp16F,sizeof(void*),0));});
return({struct _tuple16 _tmp171;_tmp171.f1=Cyc_CfFlowInfo_BottomFL();_tmp171.f2=_tmp160->unknown_all;_tmp171;});}else{
# 1059
if(needs_unconsume)
# 1061
return({struct _tuple16 _tmp172;_tmp172.f1=_tmp167;_tmp172.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp160,_tmp16E);_tmp172;});else{
# 1064
void*_tmp173=Cyc_CfFlowInfo_make_unique_consumed(_tmp160,(void*)_check_null(e->topt),e,env->iteration_num,_tmp16E);
struct Cyc_Dict_Dict _tmp174=Cyc_CfFlowInfo_assign_place(_tmp160,e->loc,_tmp16B,env->all_changed,_tmp16D,_tmp173);
# 1076
return({struct _tuple16 _tmp175;_tmp175.f1=Cyc_CfFlowInfo_ReachableFL(_tmp174,_tmp16C);_tmp175.f2=_tmp16E;_tmp175;});}}}}else{goto _LL142;}}else{_LL142: _LL143:
# 1078
 goto _LL13F;}_LL13F:;};}{
# 1081
void*_tmp177=e->r;void*_tmp178=_tmp177;struct Cyc_Absyn_Stmt*_tmp179;struct Cyc_Absyn_Exp*_tmp17A;void*_tmp17B;int _tmp17C;struct Cyc_Absyn_Vardecl*_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;int _tmp180;struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp182;enum Cyc_Absyn_AggrKind _tmp183;struct Cyc_List_List*_tmp184;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Exp*_tmp187;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp190;struct _dyneither_ptr*_tmp191;struct Cyc_Absyn_Exp*_tmp192;struct _dyneither_ptr*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct _dyneither_ptr*_tmp195;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;int _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;void**_tmp19E;struct Cyc_Absyn_Exp*_tmp19F;int _tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;enum Cyc_Absyn_Primop _tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_Absyn_Datatypedecl*_tmp1AE;struct Cyc_Absyn_Vardecl*_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B0;struct Cyc_Absyn_Vardecl*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;switch(*((int*)_tmp178)){case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp178)->f4 == Cyc_Absyn_NonNull_to_Null){_LL145: _tmp1B5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL146: {
# 1085
struct _tuple16 _tmp1B6=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1B5);struct _tuple16 _tmp1B7=_tmp1B6;union Cyc_CfFlowInfo_FlowInfo _tmp1B8;void*_tmp1B9;_LL1AE: _tmp1B8=_tmp1B7.f1;_tmp1B9=_tmp1B7.f2;_LL1AF:;{
struct _tuple16 _tmp1BA=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1B8,_tmp1B5,_tmp1B9);struct _tuple16 _tmp1BB=_tmp1BA;union Cyc_CfFlowInfo_FlowInfo _tmp1BC;void*_tmp1BD;_LL1B1: _tmp1BC=_tmp1BB.f1;_tmp1BD=_tmp1BB.f2;_LL1B2:;
return({struct _tuple16 _tmp1BE;_tmp1BE.f1=_tmp1BC;_tmp1BE.f2=_tmp1B9;_tmp1BE;});};}}else{_LL147: _tmp1B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL148:
# 1091
 _tmp1B3=_tmp1B4;goto _LL14A;}case 11: _LL149: _tmp1B3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL14A:
 _tmp1B2=_tmp1B3;goto _LL14C;case 12: _LL14B: _tmp1B2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL14C:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1B2);case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp178)->f1).Int_c).tag){case 1: _LL14D: _LL14E:
# 1095
 goto _LL150;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp178)->f1).Int_c).val).f2 == 0){_LL14F: _LL150:
 return({struct _tuple16 _tmp1BF;_tmp1BF.f1=inflow;_tmp1BF.f2=_tmp160->zero;_tmp1BF;});}else{_LL151: _LL152:
 goto _LL154;}default: _LL157: _LL158:
# 1101
 goto _LL15A;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)){case 2: _LL153: _LL154:
# 1098
 return({struct _tuple16 _tmp1C0;_tmp1C0.f1=inflow;_tmp1C0.f2=_tmp160->notzeroall;_tmp1C0;});case 1: _LL163: _LL164:
# 1109
 return({struct _tuple16 _tmp1C2;_tmp1C2.f1=inflow;_tmp1C2.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all);_tmp1C2;});case 3: _LL165: _tmp1B1=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)->f1;_LL166:
# 1112
 _tmp1B0=_tmp1B1;goto _LL168;case 4: _LL167: _tmp1B0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)->f1;_LL168:
 _tmp1AF=_tmp1B0;goto _LL16A;case 5: _LL169: _tmp1AF=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)->f1;_LL16A: {
# 1116
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp1C4;_tmp1C4.tag=0;_tmp1C4.f1=_tmp1AF;_tmp1C4;});
return({struct _tuple16 _tmp1C3;_tmp1C3.f1=inflow;_tmp1C3.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);_tmp1C3;});}default: _LL1A3: _LL1A4:
# 1723
 goto _LL1A6;}case 30: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp178)->f1 == 0){_LL155: _LL156:
# 1100
 goto _LL158;}else{_LL191: _tmp1AD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1AE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL192:
# 1579
 _tmp186=_tmp1AD;goto _LL194;}case 17: _LL159: _LL15A:
# 1102
 goto _LL15C;case 16: _LL15B: _LL15C:
 goto _LL15E;case 18: _LL15D: _LL15E:
 goto _LL160;case 32: _LL15F: _LL160:
 goto _LL162;case 31: _LL161: _LL162:
 return({struct _tuple16 _tmp1C1;_tmp1C1.f1=inflow;_tmp1C1.f2=_tmp160->unknown_all;_tmp1C1;});case 2: _LL16B: _tmp1AB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1AC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL16C: {
# 1123
struct _tuple16 _tmp1C5=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1AC);struct _tuple16 _tmp1C6=_tmp1C5;union Cyc_CfFlowInfo_FlowInfo _tmp1C7;void*_tmp1C8;_LL1B4: _tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;_LL1B5:;
return({struct _tuple16 _tmp1C9;_tmp1C9.f1=_tmp1C7;_tmp1C9.f2=_tmp1C8;_tmp1C9;});}case 4: _LL16D: _tmp1AA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL16E: {
# 1127
struct Cyc_List_List _tmp1CA=({struct Cyc_List_List _tmp1D8;_tmp1D8.hd=_tmp1AA;_tmp1D8.tl=0;_tmp1D8;});
struct _tuple16 _tmp1CB=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1CA);struct _tuple16 _tmp1CC=_tmp1CB;union Cyc_CfFlowInfo_FlowInfo _tmp1CD;_LL1B7: _tmp1CD=_tmp1CC.f1;_LL1B8:;{
struct _tuple18 _tmp1CE=Cyc_NewControlFlow_anal_Lexp(env,_tmp1CD,0,0,_tmp1AA);struct _tuple18 _tmp1CF=_tmp1CE;union Cyc_CfFlowInfo_AbsLVal _tmp1D0;_LL1BA: _tmp1D0=_tmp1CF.f2;_LL1BB:;
{struct _tuple25 _tmp1D1=({struct _tuple25 _tmp1D6;_tmp1D6.f1=_tmp1D0;_tmp1D6.f2=_tmp1CD;_tmp1D6;});struct _tuple25 _tmp1D2=_tmp1D1;struct Cyc_CfFlowInfo_Place*_tmp1D3;struct Cyc_Dict_Dict _tmp1D4;struct Cyc_List_List*_tmp1D5;if(((_tmp1D2.f1).PlaceL).tag == 1){if(((_tmp1D2.f2).ReachableFL).tag == 2){_LL1BD: _tmp1D3=((_tmp1D2.f1).PlaceL).val;_tmp1D4=(((_tmp1D2.f2).ReachableFL).val).f1;_tmp1D5=(((_tmp1D2.f2).ReachableFL).val).f2;_LL1BE:
# 1132
 _tmp1D5=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp1D5,_tmp1AA);
_tmp1CD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp160,_tmp1AA->loc,_tmp1D4,env->all_changed,_tmp1D3,_tmp160->unknown_all),_tmp1D5);
# 1137
goto _LL1BC;}else{goto _LL1BF;}}else{_LL1BF: _LL1C0:
 goto _LL1BC;}_LL1BC:;}
# 1140
return({struct _tuple16 _tmp1D7;_tmp1D7.f1=_tmp1CD;_tmp1D7.f2=_tmp160->unknown_all;_tmp1D7;});};}case 3: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f2 != 0){_LL16F: _tmp1A8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL170:
# 1143
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
({void*_tmp1D9=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp1DA="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1DA,sizeof(char),62);}),_tag_dyneither(_tmp1D9,sizeof(void*),0));});
return({struct _tuple16 _tmp1DB;_tmp1DB.f1=Cyc_CfFlowInfo_BottomFL();_tmp1DB.f2=_tmp160->unknown_all;_tmp1DB;});}{
# 1147
struct Cyc_List_List _tmp1DC=({struct Cyc_List_List _tmp1EB;_tmp1EB.hd=_tmp1A9;_tmp1EB.tl=0;_tmp1EB;});
struct Cyc_List_List _tmp1DD=({struct Cyc_List_List _tmp1EA;_tmp1EA.hd=_tmp1A8;_tmp1EA.tl=& _tmp1DC;_tmp1EA;});
struct _tuple16 _tmp1DE=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1DD);struct _tuple16 _tmp1DF=_tmp1DE;union Cyc_CfFlowInfo_FlowInfo _tmp1E0;_LL1C2: _tmp1E0=_tmp1DF.f1;_LL1C3:;{
struct _tuple18 _tmp1E1=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E0,0,0,_tmp1A8);struct _tuple18 _tmp1E2=_tmp1E1;union Cyc_CfFlowInfo_AbsLVal _tmp1E3;_LL1C5: _tmp1E3=_tmp1E2.f2;_LL1C6:;
{union Cyc_CfFlowInfo_FlowInfo _tmp1E4=_tmp1E0;struct Cyc_Dict_Dict _tmp1E5;struct Cyc_List_List*_tmp1E6;if((_tmp1E4.ReachableFL).tag == 2){_LL1C8: _tmp1E5=((_tmp1E4.ReachableFL).val).f1;_tmp1E6=((_tmp1E4.ReachableFL).val).f2;_LL1C9:
# 1153
{union Cyc_CfFlowInfo_AbsLVal _tmp1E7=_tmp1E3;struct Cyc_CfFlowInfo_Place*_tmp1E8;if((_tmp1E7.PlaceL).tag == 1){_LL1CD: _tmp1E8=(_tmp1E7.PlaceL).val;_LL1CE:
# 1157
 _tmp1E6=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp1E6,_tmp1A8);
_tmp1E5=Cyc_CfFlowInfo_assign_place(_tmp160,_tmp1A8->loc,_tmp1E5,env->all_changed,_tmp1E8,_tmp160->unknown_all);
# 1160
_tmp1E0=Cyc_CfFlowInfo_ReachableFL(_tmp1E5,_tmp1E6);
# 1164
goto _LL1CC;}else{_LL1CF: _LL1D0:
# 1167
 goto _LL1CC;}_LL1CC:;}
# 1169
goto _LL1C7;}else{_LL1CA: _LL1CB:
 goto _LL1C7;}_LL1C7:;}
# 1172
return({struct _tuple16 _tmp1E9;_tmp1E9.f1=_tmp1E0;_tmp1E9.f2=_tmp160->unknown_all;_tmp1E9;});};};}else{_LL171: _tmp1A6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL172:
# 1176
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
({void*_tmp1EC=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp1ED="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1ED,sizeof(char),62);}),_tag_dyneither(_tmp1EC,sizeof(void*),0));});
return({struct _tuple16 _tmp1EE;_tmp1EE.f1=Cyc_CfFlowInfo_BottomFL();_tmp1EE.f2=_tmp160->unknown_all;_tmp1EE;});}{
# 1180
struct Cyc_Dict_Dict*_tmp1EF=env->all_changed;
# 1182
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,({struct Cyc_Absyn_Exp*_tmp1F0[2];_tmp1F0[1]=_tmp1A7;_tmp1F0[0]=_tmp1A6;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp1F0,sizeof(struct Cyc_Absyn_Exp*),2));}));
while(1){
env->all_changed=({struct Cyc_Dict_Dict*_tmp1F1=_region_malloc(env->r,sizeof(*_tmp1F1));_tmp1F1[0]=_tmp160->mt_place_set;_tmp1F1;});{
struct _tuple18 _tmp1F2=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1A6);struct _tuple18 _tmp1F3=_tmp1F2;union Cyc_CfFlowInfo_FlowInfo _tmp1F4;union Cyc_CfFlowInfo_AbsLVal _tmp1F5;_LL1D2: _tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;_LL1D3:;{
struct Cyc_Dict_Dict _tmp1F6=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
env->all_changed=({struct Cyc_Dict_Dict*_tmp1F7=_region_malloc(env->r,sizeof(*_tmp1F7));_tmp1F7[0]=_tmp160->mt_place_set;_tmp1F7;});{
struct _tuple16 _tmp1F8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A7);struct _tuple16 _tmp1F9=_tmp1F8;union Cyc_CfFlowInfo_FlowInfo _tmp1FA;void*_tmp1FB;_LL1D5: _tmp1FA=_tmp1F9.f1;_tmp1FB=_tmp1F9.f2;_LL1D6:;{
struct Cyc_Dict_Dict _tmp1FC=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp1FD=Cyc_CfFlowInfo_after_flow(_tmp160,& _tmp1F6,_tmp1F4,_tmp1FA,_tmp1F6,_tmp1FC);
# 1193
union Cyc_CfFlowInfo_FlowInfo _tmp1FE=Cyc_CfFlowInfo_join_flow(_tmp160,_tmp1EF,inflow,_tmp1FD);
# 1196
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1FE,inflow)){
if(_tmp1EF == 0)
env->all_changed=0;else{
# 1200
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1EF,
Cyc_CfFlowInfo_union_place_set(_tmp1F6,_tmp1FC,0),0);}
# 1203
return Cyc_NewControlFlow_do_assign(_tmp160,env,_tmp1FD,_tmp1A6,_tmp1F5,_tmp1A7,_tmp1FB,e->loc);}
# 1206
inflow=_tmp1FE;};};};};}};}case 8: _LL173: _tmp1A4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL174: {
# 1211
struct _tuple16 _tmp1FF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A4);struct _tuple16 _tmp200=_tmp1FF;union Cyc_CfFlowInfo_FlowInfo _tmp201;void*_tmp202;_LL1D8: _tmp201=_tmp200.f1;_tmp202=_tmp200.f2;_LL1D9:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp201,_tmp1A5);}case 10: _LL175: _tmp1A3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL176: {
# 1215
struct _tuple16 _tmp203=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A3);struct _tuple16 _tmp204=_tmp203;union Cyc_CfFlowInfo_FlowInfo _tmp205;void*_tmp206;_LL1DB: _tmp205=_tmp204.f1;_tmp206=_tmp204.f2;_LL1DC:;
Cyc_NewControlFlow_use_Rval(env,_tmp1A3->loc,_tmp205,_tmp206);
return({struct _tuple16 _tmp207;_tmp207.f1=Cyc_CfFlowInfo_BottomFL();_tmp207.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all);_tmp207;});}case 9: _LL177: _tmp1A1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL178: {
# 1220
struct _RegionHandle _tmp208=_new_region("temp");struct _RegionHandle*temp=& _tmp208;_push_region(temp);
{struct Cyc_List_List*_tmp209=_tmp1A2;
struct _tuple23 _tmp20A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_List_List*_tmp237=_region_malloc(temp,sizeof(*_tmp237));_tmp237->hd=_tmp1A1;_tmp237->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1A2);_tmp237;}),0,1);struct _tuple23 _tmp20B=_tmp20A;union Cyc_CfFlowInfo_FlowInfo _tmp20C;struct Cyc_List_List*_tmp20D;_LL1DE: _tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;_LL1DF:;{
# 1224
union Cyc_CfFlowInfo_FlowInfo _tmp20E=Cyc_NewControlFlow_use_Rval(env,_tmp1A1->loc,_tmp20C,(void*)((struct Cyc_List_List*)_check_null(_tmp20D))->hd);
_tmp20D=_tmp20D->tl;{
# 1227
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp20F=Cyc_Tcutil_compress((void*)_check_null(_tmp1A1->topt));void*_tmp210=_tmp20F;void*_tmp211;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->tag == 5){_LL1E1: _tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->f1).elt_typ;_LL1E2:
# 1234
{void*_tmp212=Cyc_Tcutil_compress(_tmp211);void*_tmp213=_tmp212;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp215;struct Cyc_List_List*_tmp216;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->tag == 9){_LL1E6: _tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->f1).attributes;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->f1).requires_relns;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->f1).ensures_relns;_LL1E7:
# 1236
 requires=_tmp215;
ensures=_tmp216;
for(0;_tmp214 != 0;_tmp214=_tmp214->tl){
# 1240
void*_tmp217=(void*)_tmp214->hd;void*_tmp218=_tmp217;int _tmp219;int _tmp21A;int _tmp21B;switch(*((int*)_tmp218)){case 20: _LL1EB: _tmp21B=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp218)->f1;_LL1EC:
# 1242
 init_params=({struct Cyc_List_List*_tmp21C=_region_malloc(temp,sizeof(*_tmp21C));_tmp21C->hd=(void*)_tmp21B;_tmp21C->tl=init_params;_tmp21C;});goto _LL1EA;case 21: _LL1ED: _tmp21A=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp218)->f1;_LL1EE:
# 1244
 nolive_unique_params=({struct Cyc_List_List*_tmp21D=_region_malloc(temp,sizeof(*_tmp21D));_tmp21D->hd=(void*)_tmp21A;_tmp21D->tl=nolive_unique_params;_tmp21D;});
goto _LL1EA;case 22: _LL1EF: _tmp219=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp218)->f1;_LL1F0:
# 1248
 noconsume_params=({struct Cyc_List_List*_tmp21E=_region_malloc(temp,sizeof(*_tmp21E));_tmp21E->hd=(void*)_tmp219;_tmp21E->tl=noconsume_params;_tmp21E;});
goto _LL1EA;default: _LL1F1: _LL1F2:
 goto _LL1EA;}_LL1EA:;}
# 1252
goto _LL1E5;}else{_LL1E8: _LL1E9:
({void*_tmp21F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp220="anal_Rexp: bad function type";_tag_dyneither(_tmp220,sizeof(char),29);}),_tag_dyneither(_tmp21F,sizeof(void*),0));});}_LL1E5:;}
# 1255
goto _LL1E0;}else{_LL1E3: _LL1E4:
({void*_tmp221=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp222="anal_Rexp: bad function type";_tag_dyneither(_tmp222,sizeof(char),29);}),_tag_dyneither(_tmp221,sizeof(void*),0));});}_LL1E0:;}
# 1258
{int i=1;for(0;_tmp20D != 0;(((_tmp20D=_tmp20D->tl,_tmp1A2=((struct Cyc_List_List*)_check_null(_tmp1A2))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp223=_tmp20C;struct Cyc_Dict_Dict _tmp224;if((_tmp223.BottomFL).tag == 1){_LL1F4: _LL1F5:
 goto _LL1F3;}else{_LL1F6: _tmp224=((_tmp223.ReachableFL).val).f1;_LL1F7:
# 1263
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp224,(void*)_tmp20D->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp225=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A2))->hd)->loc,({const char*_tmp226="expression may not be initialized";_tag_dyneither(_tmp226,sizeof(char),34);}),_tag_dyneither(_tmp225,sizeof(void*),0));});
{union Cyc_CfFlowInfo_FlowInfo _tmp227=_tmp20E;struct Cyc_Dict_Dict _tmp228;struct Cyc_List_List*_tmp229;if((_tmp227.BottomFL).tag == 1){_LL1F9: _LL1FA:
 goto _LL1F8;}else{_LL1FB: _tmp228=((_tmp227.ReachableFL).val).f1;_tmp229=((_tmp227.ReachableFL).val).f2;_LL1FC: {
# 1270
struct Cyc_Dict_Dict _tmp22A=Cyc_CfFlowInfo_escape_deref(_tmp160,_tmp228,env->all_changed,(void*)_tmp20D->hd);
{void*_tmp22B=(void*)_tmp20D->hd;void*_tmp22C=_tmp22B;struct Cyc_CfFlowInfo_Place*_tmp22D;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp22C)->tag == 5){_LL1FE: _tmp22D=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp22C)->f1;_LL1FF:
# 1273
{void*_tmp22E=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A2))->hd)->topt));void*_tmp22F=_tmp22E;void*_tmp230;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F)->tag == 5){_LL203: _tmp230=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F)->f1).elt_typ;_LL204:
# 1275
 _tmp22A=Cyc_CfFlowInfo_assign_place(_tmp160,((struct Cyc_Absyn_Exp*)_tmp1A2->hd)->loc,_tmp22A,env->all_changed,_tmp22D,
# 1277
Cyc_CfFlowInfo_typ_to_absrval(_tmp160,_tmp230,0,_tmp160->esc_all));
# 1280
goto _LL202;}else{_LL205: _LL206:
({void*_tmp231=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp232="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp232,sizeof(char),39);}),_tag_dyneither(_tmp231,sizeof(void*),0));});}_LL202:;}
# 1283
goto _LL1FD;}else{_LL200: _LL201:
 goto _LL1FD;}_LL1FD:;}
# 1286
_tmp20E=Cyc_CfFlowInfo_ReachableFL(_tmp22A,_tmp229);
goto _LL1F8;}}_LL1F8:;}
# 1289
goto _LL1F3;}_LL1F3:;}else{
# 1292
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1297
_tmp20E=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A2))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A2))->hd)->topt),_tmp20E,(void*)_tmp20D->hd);else{
# 1307
_tmp20E=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A2))->hd)->loc,_tmp20E,(void*)_tmp20D->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp1A2->hd)->topt),0))
_tmp20E=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp20E,(struct Cyc_Absyn_Exp*)_tmp1A2->hd,((struct Cyc_Absyn_Exp*)_tmp1A2->hd)->loc,(void*)_tmp20D->hd);}}}}
# 1314
Cyc_NewControlFlow_check_fn_requires(env,_tmp20E,_tmp209,requires,e->loc);
# 1317
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1A1->topt))){
struct _tuple16 _tmp234=({struct _tuple16 _tmp233;_tmp233.f1=Cyc_CfFlowInfo_BottomFL();_tmp233.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all);_tmp233;});_npop_handler(0);return _tmp234;}else{
# 1320
struct _tuple16 _tmp236=({struct _tuple16 _tmp235;_tmp235.f1=_tmp20E;_tmp235.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all);_tmp235;});_npop_handler(0);return _tmp236;}};};}
# 1221
;_pop_region(temp);}case 33: _LL179: _tmp19C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).is_calloc;_tmp19D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).rgn;_tmp19E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).elt_type;_tmp19F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).num_elts;_tmp1A0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).fat_result;_LL17A: {
# 1323
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp249=_region_malloc(_tmp160->r,sizeof(*_tmp249));_tmp249[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp24A;_tmp24A.tag=1;_tmp24A.f1=_tmp19F;_tmp24A.f2=(void*)_check_null(e->topt);_tmp24A;});_tmp249;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp248=_region_malloc(_tmp160->r,sizeof(*_tmp248));_tmp248->root=root;_tmp248->fields=0;_tmp248;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp246=_region_malloc(_tmp160->r,sizeof(*_tmp246));_tmp246[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp247;_tmp247.tag=5;_tmp247.f1=place;_tmp247;});_tmp246;});
void*place_val;
if(_tmp1A0)place_val=_tmp160->notzeroall;else{
if(_tmp19C)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,*((void**)_check_null(_tmp19E)),0,_tmp160->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,*((void**)_check_null(_tmp19E)),0,_tmp160->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp19D != 0){
struct _RegionHandle _tmp238=_new_region("temp");struct _RegionHandle*temp=& _tmp238;_push_region(temp);
{struct _tuple23 _tmp239=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp23D[2];_tmp23D[1]=_tmp19F;_tmp23D[0]=_tmp19D;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp23D,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);struct _tuple23 _tmp23A=_tmp239;union Cyc_CfFlowInfo_FlowInfo _tmp23B;struct Cyc_List_List*_tmp23C;_LL208: _tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;_LL209:;
# 1336
outflow=_tmp23B;}
# 1334
;_pop_region(temp);}else{
# 1339
struct _tuple16 _tmp23E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp19F);struct _tuple16 _tmp23F=_tmp23E;union Cyc_CfFlowInfo_FlowInfo _tmp240;_LL20B: _tmp240=_tmp23F.f1;_LL20C:;
outflow=_tmp240;}{
# 1342
union Cyc_CfFlowInfo_FlowInfo _tmp241=outflow;struct Cyc_Dict_Dict _tmp242;struct Cyc_List_List*_tmp243;if((_tmp241.BottomFL).tag == 1){_LL20E: _LL20F:
 return({struct _tuple16 _tmp244;_tmp244.f1=outflow;_tmp244.f2=rval;_tmp244;});}else{_LL210: _tmp242=((_tmp241.ReachableFL).val).f1;_tmp243=((_tmp241.ReachableFL).val).f2;_LL211:
# 1345
 return({struct _tuple16 _tmp245;_tmp245.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp242,root,place_val),_tmp243);_tmp245.f2=rval;_tmp245;});}_LL20D:;};};}case 34: _LL17B: _tmp19A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp19B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL17C: {
# 1349
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1355
struct _RegionHandle _tmp24B=_new_region("temp");struct _RegionHandle*temp=& _tmp24B;_push_region(temp);{
struct _tuple23 _tmp24C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp250[2];_tmp250[1]=_tmp19B;_tmp250[0]=_tmp19A;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp250,sizeof(struct Cyc_Absyn_Exp*),2));}),0,0);struct _tuple23 _tmp24D=_tmp24C;union Cyc_CfFlowInfo_FlowInfo _tmp24E;struct Cyc_List_List*_tmp24F;_LL213: _tmp24E=_tmp24D.f1;_tmp24F=_tmp24D.f2;_LL214:;
# 1358
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp24F))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp24F->tl))->hd;
outflow=_tmp24E;}{
# 1363
struct _tuple18 _tmp251=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp19A);struct _tuple18 _tmp252=_tmp251;union Cyc_CfFlowInfo_AbsLVal _tmp253;_LL216: _tmp253=_tmp252.f2;_LL217:;{
struct _tuple18 _tmp254=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp19B);struct _tuple18 _tmp255=_tmp254;union Cyc_CfFlowInfo_AbsLVal _tmp256;_LL219: _tmp256=_tmp255.f2;_LL21A:;
{union Cyc_CfFlowInfo_FlowInfo _tmp257=outflow;struct Cyc_Dict_Dict _tmp258;struct Cyc_List_List*_tmp259;if((_tmp257.ReachableFL).tag == 2){_LL21C: _tmp258=((_tmp257.ReachableFL).val).f1;_tmp259=((_tmp257.ReachableFL).val).f2;_LL21D:
# 1367
{union Cyc_CfFlowInfo_AbsLVal _tmp25A=_tmp253;struct Cyc_CfFlowInfo_Place*_tmp25B;if((_tmp25A.PlaceL).tag == 1){_LL221: _tmp25B=(_tmp25A.PlaceL).val;_LL222:
# 1369
 _tmp258=Cyc_CfFlowInfo_assign_place(_tmp160,_tmp19A->loc,_tmp258,env->all_changed,_tmp25B,right_rval);
# 1371
goto _LL220;}else{_LL223: _LL224:
# 1376
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp258,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp25C=0;Cyc_Tcutil_terr(_tmp19B->loc,({const char*_tmp25D="expression may not be fully initialized";_tag_dyneither(_tmp25D,sizeof(char),40);}),_tag_dyneither(_tmp25C,sizeof(void*),0));});
goto _LL220;}_LL220:;}
# 1380
{union Cyc_CfFlowInfo_AbsLVal _tmp25E=_tmp256;struct Cyc_CfFlowInfo_Place*_tmp25F;if((_tmp25E.PlaceL).tag == 1){_LL226: _tmp25F=(_tmp25E.PlaceL).val;_LL227:
# 1382
 _tmp258=Cyc_CfFlowInfo_assign_place(_tmp160,_tmp19B->loc,_tmp258,env->all_changed,_tmp25F,left_rval);
# 1384
goto _LL225;}else{_LL228: _LL229:
# 1386
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp258,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp260=0;Cyc_Tcutil_terr(_tmp19A->loc,({const char*_tmp261="expression may not be fully initialized";_tag_dyneither(_tmp261,sizeof(char),40);}),_tag_dyneither(_tmp260,sizeof(void*),0));});
goto _LL225;}_LL225:;}
# 1391
_tmp259=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp259,_tmp19A);
_tmp259=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp259,_tmp19B);
# 1394
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp258,_tmp259);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL21B;}else{_LL21E: _LL21F:
 goto _LL21B;}_LL21B:;}{
# 1401
struct _tuple16 _tmp263=({struct _tuple16 _tmp262;_tmp262.f1=outflow;_tmp262.f2=_tmp160->unknown_all;_tmp262;});_npop_handler(0);return _tmp263;};};};
# 1355
;_pop_region(temp);}case 15: _LL17D: _tmp198=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp199=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL17E: {
# 1404
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp276=_region_malloc(_tmp160->r,sizeof(*_tmp276));_tmp276[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp277;_tmp277.tag=1;_tmp277.f1=_tmp199;_tmp277.f2=(void*)_check_null(e->topt);_tmp277;});_tmp276;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp275=_region_malloc(_tmp160->r,sizeof(*_tmp275));_tmp275->root=root;_tmp275->fields=0;_tmp275;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp273=_region_malloc(_tmp160->r,sizeof(*_tmp273));_tmp273[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp274;_tmp274.tag=5;_tmp274.f1=place;_tmp274;});_tmp273;});
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp198 != 0){
struct _RegionHandle _tmp264=_new_region("temp");struct _RegionHandle*temp=& _tmp264;_push_region(temp);
{struct _tuple23 _tmp265=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp269[2];_tmp269[1]=_tmp199;_tmp269[0]=_tmp198;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp269,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);struct _tuple23 _tmp266=_tmp265;union Cyc_CfFlowInfo_FlowInfo _tmp267;struct Cyc_List_List*_tmp268;_LL22B: _tmp267=_tmp266.f1;_tmp268=_tmp266.f2;_LL22C:;
# 1414
outflow=_tmp267;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp268))->tl))->hd;}
# 1412
;_pop_region(temp);}else{
# 1418
struct _tuple16 _tmp26A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp199);struct _tuple16 _tmp26B=_tmp26A;union Cyc_CfFlowInfo_FlowInfo _tmp26C;void*_tmp26D;_LL22E: _tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;_LL22F:;
outflow=_tmp26C;
place_val=_tmp26D;}{
# 1422
union Cyc_CfFlowInfo_FlowInfo _tmp26E=outflow;struct Cyc_Dict_Dict _tmp26F;struct Cyc_List_List*_tmp270;if((_tmp26E.BottomFL).tag == 1){_LL231: _LL232:
 return({struct _tuple16 _tmp271;_tmp271.f1=outflow;_tmp271.f2=rval;_tmp271;});}else{_LL233: _tmp26F=((_tmp26E.ReachableFL).val).f1;_tmp270=((_tmp26E.ReachableFL).val).f2;_LL234:
# 1425
 return({struct _tuple16 _tmp272;_tmp272.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp26F,root,place_val),_tmp270);_tmp272.f2=rval;_tmp272;});}_LL230:;};};}case 14: _LL17F: _tmp197=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL180: {
# 1429
struct _tuple18 _tmp278=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp197);struct _tuple18 _tmp279=_tmp278;union Cyc_CfFlowInfo_FlowInfo _tmp27A;union Cyc_CfFlowInfo_AbsLVal _tmp27B;_LL236: _tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_LL237:;{
union Cyc_CfFlowInfo_AbsLVal _tmp27C=_tmp27B;struct Cyc_CfFlowInfo_Place*_tmp27D;if((_tmp27C.UnknownL).tag == 2){_LL239: _LL23A:
 return({struct _tuple16 _tmp27E;_tmp27E.f1=_tmp27A;_tmp27E.f2=_tmp160->notzeroall;_tmp27E;});}else{_LL23B: _tmp27D=(_tmp27C.PlaceL).val;_LL23C:
 return({struct _tuple16 _tmp27F;_tmp27F.f1=_tmp27A;_tmp27F.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp280=_region_malloc(env->r,sizeof(*_tmp280));_tmp280[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp281;_tmp281.tag=5;_tmp281.f1=_tmp27D;_tmp281;});_tmp280;});_tmp27F;});}_LL238:;};}case 19: _LL181: _tmp196=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL182: {
# 1436
struct _tuple16 _tmp282=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp196);struct _tuple16 _tmp283=_tmp282;union Cyc_CfFlowInfo_FlowInfo _tmp284;void*_tmp285;_LL23E: _tmp284=_tmp283.f1;_tmp285=_tmp283.f2;_LL23F:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp284,_tmp196,_tmp285);}case 20: _LL183: _tmp194=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp195=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL184: {
# 1440
struct _tuple16 _tmp286=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp194);struct _tuple16 _tmp287=_tmp286;union Cyc_CfFlowInfo_FlowInfo _tmp288;void*_tmp289;_LL241: _tmp288=_tmp287.f1;_tmp289=_tmp287.f2;_LL242:;
if(_tmp194->topt == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp28C;_tmp28C.tag=0;_tmp28C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp28A[1]={& _tmp28C};Cyc_fprintf(Cyc_stderr,({const char*_tmp28B="aggrmember exp %s\n";_tag_dyneither(_tmp28B,sizeof(char),19);}),_tag_dyneither(_tmp28A,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp28F;_tmp28F.tag=0;_tmp28F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp194));({void*_tmp28D[1]={& _tmp28F};Cyc_fprintf(Cyc_stderr,({const char*_tmp28E="oops! %s.topt = null!\n";_tag_dyneither(_tmp28E,sizeof(char),23);}),_tag_dyneither(_tmp28D,sizeof(void*),1));});});}{
# 1445
void*_tmp290=(void*)_check_null(_tmp194->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp290))
# 1448
return({struct _tuple16 _tmp291;_tmp291.f1=_tmp288;_tmp291.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all);_tmp291;});
# 1450
if(Cyc_Absyn_is_require_union_type(_tmp290))
Cyc_NewControlFlow_check_union_requires(_tmp194->loc,_tmp160->r,_tmp290,_tmp195,_tmp288);{
# 1453
struct _tuple15 _tmp292=Cyc_CfFlowInfo_unname_rval(_tmp160->r,_tmp289);struct _tuple15 _tmp293=_tmp292;void*_tmp294;_LL244: _tmp294=_tmp293.f1;_LL245:;{
void*_tmp295=_tmp294;int _tmp296;int _tmp297;struct _dyneither_ptr _tmp298;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->tag == 6){_LL247: _tmp296=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->f1).is_union;_tmp297=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->f1).fieldnum;_tmp298=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->f2;_LL248: {
# 1456
int _tmp299=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp194->topt),_tmp195);
if((_tmp296  && _tmp297 != - 1) && _tmp297 != _tmp299)
return({struct _tuple16 _tmp29A;_tmp29A.f1=_tmp288;_tmp29A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),1,_tmp160->unknown_none);_tmp29A;});
return({struct _tuple16 _tmp29B;_tmp29B.f1=_tmp288;_tmp29B.f2=*((void**)_check_dyneither_subscript(_tmp298,sizeof(void*),_tmp299));_tmp29B;});}}else{_LL249: _LL24A:
# 1461
({void*_tmp29C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29D="the bad rexp is :";_tag_dyneither(_tmp29D,sizeof(char),18);}),_tag_dyneither(_tmp29C,sizeof(void*),0));});
Cyc_CfFlowInfo_print_absrval(_tmp294);
({void*_tmp29E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29F="\n";_tag_dyneither(_tmp29F,sizeof(char),2);}),_tag_dyneither(_tmp29E,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2A1;_tmp2A1.tag=Cyc_Core_Impossible;_tmp2A1.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp2A2[1]={& _tmp2A4};Cyc_aprintf(({const char*_tmp2A3="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp2A3,sizeof(char),26);}),_tag_dyneither(_tmp2A2,sizeof(void*),1));});});_tmp2A1;});_tmp2A0;}));}_LL246:;};};};}case 37: _LL185: _tmp192=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp193=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL186: {
# 1471
struct _tuple16 _tmp2A5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp192);struct _tuple16 _tmp2A6=_tmp2A5;union Cyc_CfFlowInfo_FlowInfo _tmp2A7;void*_tmp2A8;_LL24C: _tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;_LL24D:;
# 1473
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp192->topt)))
return({struct _tuple16 _tmp2A9;_tmp2A9.f1=_tmp2A7;_tmp2A9.f2=_tmp160->unknown_all;_tmp2A9;});{
struct _tuple15 _tmp2AA=Cyc_CfFlowInfo_unname_rval(_tmp160->r,_tmp2A8);struct _tuple15 _tmp2AB=_tmp2AA;void*_tmp2AC;_LL24F: _tmp2AC=_tmp2AB.f1;_LL250:;{
void*_tmp2AD=_tmp2AC;int _tmp2AE;int _tmp2AF;struct _dyneither_ptr _tmp2B0;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->tag == 6){_LL252: _tmp2AE=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->f1).is_union;_tmp2AF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->f1).fieldnum;_tmp2B0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->f2;_LL253: {
# 1478
int _tmp2B1=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp192->topt),_tmp193);
if(_tmp2AE  && _tmp2AF != - 1){
if(_tmp2AF != _tmp2B1)
return({struct _tuple16 _tmp2B2;_tmp2B2.f1=_tmp2A7;_tmp2B2.f2=_tmp160->zero;_tmp2B2;});else{
# 1483
return({struct _tuple16 _tmp2B3;_tmp2B3.f1=_tmp2A7;_tmp2B3.f2=_tmp160->notzeroall;_tmp2B3;});}}else{
# 1485
return({struct _tuple16 _tmp2B4;_tmp2B4.f1=_tmp2A7;_tmp2B4.f2=_tmp160->unknown_all;_tmp2B4;});}}}else{_LL254: _LL255:
# 1487
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2B6;_tmp2B6.tag=Cyc_Core_Impossible;_tmp2B6.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp2B7[1]={& _tmp2B9};Cyc_aprintf(({const char*_tmp2B8="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp2B8,sizeof(char),26);}),_tag_dyneither(_tmp2B7,sizeof(void*),1));});});_tmp2B6;});_tmp2B5;}));}_LL251:;};};}case 21: _LL187: _tmp190=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp191=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL188: {
# 1492
struct _tuple16 _tmp2BA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp190);struct _tuple16 _tmp2BB=_tmp2BA;union Cyc_CfFlowInfo_FlowInfo _tmp2BC;void*_tmp2BD;_LL257: _tmp2BC=_tmp2BB.f1;_tmp2BD=_tmp2BB.f2;_LL258:;{
# 1495
struct _tuple16 _tmp2BE=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2BC,_tmp190,_tmp2BD);struct _tuple16 _tmp2BF=_tmp2BE;union Cyc_CfFlowInfo_FlowInfo _tmp2C0;void*_tmp2C1;_LL25A: _tmp2C0=_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;_LL25B:;{
# 1498
void*_tmp2C2=Cyc_Tcutil_compress((void*)_check_null(_tmp190->topt));void*_tmp2C3=_tmp2C2;void*_tmp2C4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C3)->tag == 5){_LL25D: _tmp2C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C3)->f1).elt_typ;_LL25E:
# 1500
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2C4))
# 1502
return({struct _tuple16 _tmp2C5;_tmp2C5.f1=_tmp2C0;_tmp2C5.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all);_tmp2C5;});
# 1504
if(Cyc_Absyn_is_require_union_type(_tmp2C4))
Cyc_NewControlFlow_check_union_requires(_tmp190->loc,_tmp160->r,_tmp2C4,_tmp191,_tmp2BC);{
# 1507
struct _tuple15 _tmp2C6=Cyc_CfFlowInfo_unname_rval(_tmp160->r,_tmp2C1);struct _tuple15 _tmp2C7=_tmp2C6;void*_tmp2C8;_LL262: _tmp2C8=_tmp2C7.f1;_LL263:;{
void*_tmp2C9=_tmp2C8;int _tmp2CA;int _tmp2CB;struct _dyneither_ptr _tmp2CC;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->tag == 6){_LL265: _tmp2CA=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->f1).is_union;_tmp2CB=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->f1).fieldnum;_tmp2CC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->f2;_LL266: {
# 1510
int _tmp2CD=Cyc_CfFlowInfo_get_field_index(_tmp2C4,_tmp191);
if((_tmp2CA  && _tmp2CB != - 1) && _tmp2CB != _tmp2CD)
return({struct _tuple16 _tmp2CE;_tmp2CE.f1=_tmp2C0;_tmp2CE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),1,_tmp160->unknown_none);_tmp2CE;});
return({struct _tuple16 _tmp2CF;_tmp2CF.f1=_tmp2C0;_tmp2CF.f2=*((void**)_check_dyneither_subscript(_tmp2CC,sizeof(void*),_tmp2CD));_tmp2CF;});}}else{_LL267: _LL268:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2D1;_tmp2D1.tag=Cyc_Core_Impossible;_tmp2D1.f1=({const char*_tmp2D2="anal_Rexp: AggrArrow";_tag_dyneither(_tmp2D2,sizeof(char),21);});_tmp2D1;});_tmp2D0;}));}_LL264:;};};}else{_LL25F: _LL260:
# 1516
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2D4;_tmp2D4.tag=Cyc_Core_Impossible;_tmp2D4.f1=({const char*_tmp2D5="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp2D5,sizeof(char),25);});_tmp2D4;});_tmp2D3;}));}_LL25C:;};};}case 5: _LL189: _tmp18D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp18E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_tmp18F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL18A: {
# 1520
struct _tuple19 _tmp2D6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp18D);struct _tuple19 _tmp2D7=_tmp2D6;union Cyc_CfFlowInfo_FlowInfo _tmp2D8;union Cyc_CfFlowInfo_FlowInfo _tmp2D9;_LL26A: _tmp2D8=_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;_LL26B:;{
struct _tuple16 _tmp2DA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D8,_tmp18E);
struct _tuple16 _tmp2DB=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D9,_tmp18F);
# 1524
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp160,env->all_changed,_tmp2DA,_tmp2DB);};}case 6: _LL18B: _tmp18B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp18C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL18C: {
# 1527
struct _tuple19 _tmp2DC=Cyc_NewControlFlow_anal_test(env,inflow,_tmp18B);struct _tuple19 _tmp2DD=_tmp2DC;union Cyc_CfFlowInfo_FlowInfo _tmp2DE;union Cyc_CfFlowInfo_FlowInfo _tmp2DF;_LL26D: _tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;_LL26E:;{
struct _tuple16 _tmp2E0=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DE,_tmp18C);struct _tuple16 _tmp2E1=_tmp2E0;union Cyc_CfFlowInfo_FlowInfo _tmp2E2;void*_tmp2E3;_LL270: _tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;_LL271:;{
struct _tuple16 _tmp2E4=({struct _tuple16 _tmp2E7;_tmp2E7.f1=_tmp2E2;_tmp2E7.f2=_tmp2E3;_tmp2E7;});
struct _tuple16 _tmp2E5=({struct _tuple16 _tmp2E6;_tmp2E6.f1=_tmp2DF;_tmp2E6.f2=_tmp160->zero;_tmp2E6;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp160,env->all_changed,_tmp2E4,_tmp2E5);};};}case 7: _LL18D: _tmp189=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp18A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL18E: {
# 1534
struct _tuple19 _tmp2E8=Cyc_NewControlFlow_anal_test(env,inflow,_tmp189);struct _tuple19 _tmp2E9=_tmp2E8;union Cyc_CfFlowInfo_FlowInfo _tmp2EA;union Cyc_CfFlowInfo_FlowInfo _tmp2EB;_LL273: _tmp2EA=_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;_LL274:;{
struct _tuple16 _tmp2EC=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2EB,_tmp18A);struct _tuple16 _tmp2ED=_tmp2EC;union Cyc_CfFlowInfo_FlowInfo _tmp2EE;void*_tmp2EF;_LL276: _tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;_LL277:;{
struct _tuple16 _tmp2F0=({struct _tuple16 _tmp2F3;_tmp2F3.f1=_tmp2EE;_tmp2F3.f2=_tmp2EF;_tmp2F3;});
struct _tuple16 _tmp2F1=({struct _tuple16 _tmp2F2;_tmp2F2.f1=_tmp2EA;_tmp2F2.f2=_tmp160->notzeroall;_tmp2F2;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp160,env->all_changed,_tmp2F0,_tmp2F1);};};}case 22: _LL18F: _tmp187=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp188=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL190: {
# 1541
struct _RegionHandle _tmp2F4=_new_region("temp");struct _RegionHandle*temp=& _tmp2F4;_push_region(temp);
{struct _tuple23 _tmp2F5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp31A[2];_tmp31A[1]=_tmp188;_tmp31A[0]=_tmp187;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp31A,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);struct _tuple23 _tmp2F6=_tmp2F5;union Cyc_CfFlowInfo_FlowInfo _tmp2F7;struct Cyc_List_List*_tmp2F8;_LL279: _tmp2F7=_tmp2F6.f1;_tmp2F8=_tmp2F6.f2;_LL27A:;{
# 1546
union Cyc_CfFlowInfo_FlowInfo _tmp2F9=_tmp2F7;
{union Cyc_CfFlowInfo_FlowInfo _tmp2FA=_tmp2F7;struct Cyc_Dict_Dict _tmp2FB;struct Cyc_List_List*_tmp2FC;if((_tmp2FA.ReachableFL).tag == 2){_LL27C: _tmp2FB=((_tmp2FA.ReachableFL).val).f1;_tmp2FC=((_tmp2FA.ReachableFL).val).f2;_LL27D:
# 1551
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2FB,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2F8))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2FD=0;Cyc_CfFlowInfo_aerr(_tmp188->loc,({const char*_tmp2FE="expression may not be initialized";_tag_dyneither(_tmp2FE,sizeof(char),34);}),_tag_dyneither(_tmp2FD,sizeof(void*),0));});{
struct Cyc_List_List*_tmp2FF=Cyc_NewControlFlow_add_subscript_reln(_tmp160->r,_tmp2FC,_tmp187,_tmp188);
if(_tmp2FC != _tmp2FF)
_tmp2F9=Cyc_CfFlowInfo_ReachableFL(_tmp2FB,_tmp2FF);
goto _LL27B;};}else{_LL27E: _LL27F:
 goto _LL27B;}_LL27B:;}{
# 1559
void*_tmp300=Cyc_Tcutil_compress((void*)_check_null(_tmp187->topt));void*_tmp301=_tmp300;union Cyc_Absyn_Constraint*_tmp302;struct Cyc_List_List*_tmp303;switch(*((int*)_tmp301)){case 10: _LL281: _tmp303=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp301)->f1;_LL282: {
# 1561
struct _tuple15 _tmp304=Cyc_CfFlowInfo_unname_rval(_tmp160->r,(void*)((struct Cyc_List_List*)_check_null(_tmp2F8))->hd);struct _tuple15 _tmp305=_tmp304;void*_tmp306;_LL288: _tmp306=_tmp305.f1;_LL289:;{
void*_tmp307=_tmp306;struct _dyneither_ptr _tmp308;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp307)->tag == 6){_LL28B: _tmp308=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp307)->f2;_LL28C: {
# 1564
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp188)).f1;
struct _tuple16 _tmp30A=({struct _tuple16 _tmp309;_tmp309.f1=_tmp2F9;_tmp309.f2=*((void**)_check_dyneither_subscript(_tmp308,sizeof(void*),(int)i));_tmp309;});_npop_handler(0);return _tmp30A;}}else{_LL28D: _LL28E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({struct Cyc_Core_Impossible_exn_struct _tmp30C;_tmp30C.tag=Cyc_Core_Impossible;_tmp30C.f1=({const char*_tmp30D="anal_Rexp: Subscript";_tag_dyneither(_tmp30D,sizeof(char),21);});_tmp30C;});_tmp30B;}));}_LL28A:;};}case 5: _LL283: _tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).bounds;_LL284: {
# 1569
struct _tuple16 _tmp30E=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F7,_tmp187,(void*)((struct Cyc_List_List*)_check_null(_tmp2F8))->hd);struct _tuple16 _tmp30F=_tmp30E;union Cyc_CfFlowInfo_FlowInfo _tmp310;void*_tmp311;_LL290: _tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;_LL291:;{
union Cyc_CfFlowInfo_FlowInfo _tmp312=_tmp310;if((_tmp312.BottomFL).tag == 1){_LL293: _LL294: {
struct _tuple16 _tmp314=({struct _tuple16 _tmp313;_tmp313.f1=_tmp310;_tmp313.f2=_tmp311;_tmp313;});_npop_handler(0);return _tmp314;}}else{_LL295: _LL296: {
struct _tuple16 _tmp316=({struct _tuple16 _tmp315;_tmp315.f1=_tmp2F9;_tmp315.f2=_tmp311;_tmp315;});_npop_handler(0);return _tmp316;}}_LL292:;};}default: _LL285: _LL286:
# 1574
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317[0]=({struct Cyc_Core_Impossible_exn_struct _tmp318;_tmp318.tag=Cyc_Core_Impossible;_tmp318.f1=({const char*_tmp319="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp319,sizeof(char),33);});_tmp318;});_tmp317;}));}_LL280:;};};}
# 1542
;_pop_region(temp);}case 23: _LL193: _tmp186=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL194: {
# 1581
struct _RegionHandle _tmp31B=_new_region("temp");struct _RegionHandle*temp=& _tmp31B;_push_region(temp);
{struct _tuple23 _tmp31C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp186,1,1);struct _tuple23 _tmp31D=_tmp31C;union Cyc_CfFlowInfo_FlowInfo _tmp31E;struct Cyc_List_List*_tmp31F;_LL298: _tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;_LL299:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp325=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp186);void**_tmp326=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp325));struct _dyneither_ptr _tmp329=_tag_dyneither(_tmp326,sizeof(void*),_tmp325);{unsigned int _tmp327=_tmp325;unsigned int i;for(i=0;i < _tmp327;i ++){_tmp326[i]=(void*)({
void*_tmp328=(void*)((struct Cyc_List_List*)_check_null(_tmp31F))->hd;
_tmp31F=_tmp31F->tl;
_tmp328;});}}_tmp329;});
# 1589
struct _tuple16 _tmp324=({struct _tuple16 _tmp320;_tmp320.f1=_tmp31E;_tmp320.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp321=_region_malloc(env->r,sizeof(*_tmp321));_tmp321[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp322;_tmp322.tag=6;_tmp322.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp323;_tmp323.is_union=0;_tmp323.fieldnum=- 1;_tmp323;});_tmp322.f2=aggrdict;_tmp322;});_tmp321;});_tmp320;});_npop_handler(0);return _tmp324;};}
# 1582
;_pop_region(temp);}case 29: _LL195: _tmp185=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL196: {
# 1592
struct Cyc_List_List*fs;
{void*_tmp32A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp32B=_tmp32A;struct Cyc_List_List*_tmp32C;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32B)->tag == 12){_LL29B: _tmp32C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32B)->f2;_LL29C:
 fs=_tmp32C;goto _LL29A;}else{_LL29D: _LL29E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=({struct Cyc_Core_Impossible_exn_struct _tmp32E;_tmp32E.tag=Cyc_Core_Impossible;_tmp32E.f1=({const char*_tmp32F="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp32F,sizeof(char),35);});_tmp32E;});_tmp32D;}));}_LL29A:;}
# 1597
_tmp182=_tmp185;_tmp183=Cyc_Absyn_StructA;_tmp184=fs;goto _LL198;}case 28: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4)->impl != 0){_LL197: _tmp182=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_tmp183=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4)->kind;_tmp184=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4)->impl)->fields;_LL198: {
# 1599
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp330=_new_region("temp");struct _RegionHandle*temp=& _tmp330;_push_region(temp);
{struct _tuple23 _tmp331=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp182),1,1);
# 1601
struct _tuple23 _tmp332=_tmp331;union Cyc_CfFlowInfo_FlowInfo _tmp333;struct Cyc_List_List*_tmp334;_LL2A0: _tmp333=_tmp332.f1;_tmp334=_tmp332.f2;_LL2A1:;{
# 1603
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp160,_tmp184,_tmp183 == Cyc_Absyn_UnionA,_tmp160->unknown_all);
# 1606
int field_no=-1;
{int i=0;for(0;_tmp334 != 0;(((_tmp334=_tmp334->tl,_tmp182=_tmp182->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp182))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp335=(void*)ds->hd;void*_tmp336=_tmp335;struct _dyneither_ptr*_tmp337;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp336)->tag == 0){_LL2A3: _LL2A4:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338[0]=({struct Cyc_Core_Impossible_exn_struct _tmp339;_tmp339.tag=Cyc_Core_Impossible;_tmp339.f1=({const char*_tmp33A="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp33A,sizeof(char),22);});_tmp339;});_tmp338;}));}else{_LL2A5: _tmp337=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp336)->f1;_LL2A6:
# 1613
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp184,_tmp337);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp334->hd;
# 1616
if(_tmp183 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp33B=(*((struct _tuple26*)_tmp182->hd)).f2;
_tmp333=Cyc_NewControlFlow_use_Rval(env,_tmp33B->loc,_tmp333,(void*)_tmp334->hd);
# 1620
Cyc_NewControlFlow_check_union_requires(_tmp33B->loc,_tmp160->r,exp_type,_tmp337,_tmp333);}}_LL2A2:;}}}{
# 1623
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp33C=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp33F=_region_malloc(env->r,sizeof(*_tmp33F));_tmp33F[0]=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp340;_tmp340.tag=6;_tmp340.f1=({struct Cyc_CfFlowInfo_UnionRInfo _tmp341;_tmp341.is_union=_tmp183 == Cyc_Absyn_UnionA;_tmp341.fieldnum=field_no;_tmp341;});_tmp340.f2=aggrdict;_tmp340;});_tmp33F;});
struct _tuple16 _tmp33E=({struct _tuple16 _tmp33D;_tmp33D.f1=_tmp333;_tmp33D.f2=(void*)_tmp33C;_tmp33D;});_npop_handler(0);return _tmp33E;};};}
# 1601
;_pop_region(temp);}}else{goto _LL199;}}else{_LL199: _LL19A:
# 1627
({struct Cyc_String_pa_PrintArg_struct _tmp344;_tmp344.tag=0;_tmp344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp342[1]={& _tmp344};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp343="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp343,sizeof(char),37);}),_tag_dyneither(_tmp342,sizeof(void*),1));});});}case 25: _LL19B: _tmp181=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL19C: {
# 1629
struct _RegionHandle _tmp345=_new_region("temp");struct _RegionHandle*temp=& _tmp345;_push_region(temp);
{struct Cyc_List_List*_tmp346=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp181);
struct _tuple23 _tmp347=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp346,1,1);struct _tuple23 _tmp348=_tmp347;union Cyc_CfFlowInfo_FlowInfo _tmp349;struct Cyc_List_List*_tmp34A;_LL2A8: _tmp349=_tmp348.f1;_tmp34A=_tmp348.f2;_LL2A9:;
for(0;_tmp34A != 0;(_tmp34A=_tmp34A->tl,_tmp346=_tmp346->tl)){
_tmp349=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp346))->hd)->loc,_tmp349,(void*)_tmp34A->hd);}{
struct _tuple16 _tmp34C=({struct _tuple16 _tmp34B;_tmp34B.f1=_tmp349;_tmp34B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all);_tmp34B;});_npop_handler(0);return _tmp34C;};}
# 1630
;_pop_region(temp);}case 26: _LL19D: _tmp17D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp17E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_tmp17F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_tmp180=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f4;_LL19E: {
# 1638
struct _tuple16 _tmp34D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp17E);struct _tuple16 _tmp34E=_tmp34D;union Cyc_CfFlowInfo_FlowInfo _tmp34F;void*_tmp350;_LL2AB: _tmp34F=_tmp34E.f1;_tmp350=_tmp34E.f2;_LL2AC:;{
union Cyc_CfFlowInfo_FlowInfo _tmp351=_tmp34F;struct Cyc_Dict_Dict _tmp352;struct Cyc_List_List*_tmp353;if((_tmp351.BottomFL).tag == 1){_LL2AE: _LL2AF:
 return({struct _tuple16 _tmp354;_tmp354.f1=_tmp34F;_tmp354.f2=_tmp160->unknown_all;_tmp354;});}else{_LL2B0: _tmp352=((_tmp351.ReachableFL).val).f1;_tmp353=((_tmp351.ReachableFL).val).f2;_LL2B1:
# 1642
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp352,_tmp350)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp355=0;Cyc_CfFlowInfo_aerr(_tmp17E->loc,({const char*_tmp356="expression may not be initialized";_tag_dyneither(_tmp356,sizeof(char),34);}),_tag_dyneither(_tmp355,sizeof(void*),0));});{
# 1646
struct Cyc_List_List*new_relns=_tmp353;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp17D);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp17E,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp353);
# 1652
if(_tmp353 != new_relns)
_tmp34F=Cyc_CfFlowInfo_ReachableFL(_tmp352,new_relns);{
# 1656
void*_tmp357=_tmp350;switch(*((int*)_tmp357)){case 0: _LL2B3: _LL2B4:
 return({struct _tuple16 _tmp358;_tmp358.f1=_tmp34F;_tmp358.f2=_tmp160->unknown_all;_tmp358;});case 2: _LL2B5: _LL2B6:
 goto _LL2B8;case 1: _LL2B7: _LL2B8:
 goto _LL2BA;case 5: _LL2B9: _LL2BA: {
# 1661
struct Cyc_List_List _tmp359=({struct Cyc_List_List _tmp364;_tmp364.hd=_tmp17D;_tmp364.tl=0;_tmp364;});
_tmp34F=Cyc_NewControlFlow_add_vars(_tmp160,_tmp34F,& _tmp359,_tmp160->unknown_all,e->loc,0);{
# 1664
struct _tuple16 _tmp35A=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34F,_tmp17F);struct _tuple16 _tmp35B=_tmp35A;union Cyc_CfFlowInfo_FlowInfo _tmp35C;void*_tmp35D;_LL2BE: _tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;_LL2BF:;
{union Cyc_CfFlowInfo_FlowInfo _tmp35E=_tmp35C;struct Cyc_Dict_Dict _tmp35F;if((_tmp35E.BottomFL).tag == 1){_LL2C1: _LL2C2:
 return({struct _tuple16 _tmp360;_tmp360.f1=_tmp35C;_tmp360.f2=_tmp160->unknown_all;_tmp360;});}else{_LL2C3: _tmp35F=((_tmp35E.ReachableFL).val).f1;_LL2C4:
# 1668
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp35F,_tmp35D)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp361=0;Cyc_CfFlowInfo_aerr(_tmp17E->loc,({const char*_tmp362="expression may not be initialized";_tag_dyneither(_tmp362,sizeof(char),34);}),_tag_dyneither(_tmp361,sizeof(void*),0));});
return({struct _tuple16 _tmp363;_tmp363.f1=Cyc_CfFlowInfo_BottomFL();_tmp363.f2=_tmp160->unknown_all;_tmp363;});}}_LL2C0:;}
# 1673
_tmp34F=_tmp35C;
goto _LL2BC;};}default: _LL2BB: _LL2BC:
# 1676
 while(1){
struct Cyc_List_List _tmp365=({struct Cyc_List_List _tmp370;_tmp370.hd=_tmp17D;_tmp370.tl=0;_tmp370;});
_tmp34F=Cyc_NewControlFlow_add_vars(_tmp160,_tmp34F,& _tmp365,_tmp160->unknown_all,e->loc,0);{
struct _tuple16 _tmp366=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34F,_tmp17F);struct _tuple16 _tmp367=_tmp366;union Cyc_CfFlowInfo_FlowInfo _tmp368;void*_tmp369;_LL2C6: _tmp368=_tmp367.f1;_tmp369=_tmp367.f2;_LL2C7:;
{union Cyc_CfFlowInfo_FlowInfo _tmp36A=_tmp368;struct Cyc_Dict_Dict _tmp36B;if((_tmp36A.BottomFL).tag == 1){_LL2C9: _LL2CA:
 goto _LL2C8;}else{_LL2CB: _tmp36B=((_tmp36A.ReachableFL).val).f1;_LL2CC:
# 1683
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp36B,_tmp369)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp36C=0;Cyc_CfFlowInfo_aerr(_tmp17E->loc,({const char*_tmp36D="expression may not be initialized";_tag_dyneither(_tmp36D,sizeof(char),34);}),_tag_dyneither(_tmp36C,sizeof(void*),0));});
return({struct _tuple16 _tmp36E;_tmp36E.f1=Cyc_CfFlowInfo_BottomFL();_tmp36E.f2=_tmp160->unknown_all;_tmp36E;});}}_LL2C8:;}{
# 1688
union Cyc_CfFlowInfo_FlowInfo _tmp36F=Cyc_CfFlowInfo_join_flow(_tmp160,env->all_changed,_tmp34F,_tmp368);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp36F,_tmp34F))
break;
_tmp34F=_tmp36F;};};}
# 1693
return({struct _tuple16 _tmp371;_tmp371.f1=_tmp34F;_tmp371.f2=_tmp160->unknown_all;_tmp371;});}_LL2B2:;};};}_LL2AD:;};}case 27: _LL19F: _tmp17A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp17B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_tmp17C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL1A0: {
# 1699
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp37D=_region_malloc(_tmp160->r,sizeof(*_tmp37D));_tmp37D[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp37E;_tmp37E.tag=1;_tmp37E.f1=_tmp17A;_tmp37E.f2=(void*)_check_null(e->topt);_tmp37E;});_tmp37D;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp37C=_region_malloc(_tmp160->r,sizeof(*_tmp37C));_tmp37C->root=root;_tmp37C->fields=0;_tmp37C;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp37A=_region_malloc(_tmp160->r,sizeof(*_tmp37A));_tmp37A[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp37B;_tmp37B.tag=5;_tmp37B.f1=place;_tmp37B;});_tmp37A;});
void*place_val;
# 1707
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,_tmp17B,0,_tmp160->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp372=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp17A);struct _tuple16 _tmp373=_tmp372;union Cyc_CfFlowInfo_FlowInfo _tmp374;_LL2CE: _tmp374=_tmp373.f1;_LL2CF:;
outflow=_tmp374;{
union Cyc_CfFlowInfo_FlowInfo _tmp375=outflow;struct Cyc_Dict_Dict _tmp376;struct Cyc_List_List*_tmp377;if((_tmp375.BottomFL).tag == 1){_LL2D1: _LL2D2:
 return({struct _tuple16 _tmp378;_tmp378.f1=outflow;_tmp378.f2=rval;_tmp378;});}else{_LL2D3: _tmp376=((_tmp375.ReachableFL).val).f1;_tmp377=((_tmp375.ReachableFL).val).f2;_LL2D4:
# 1715
 return({struct _tuple16 _tmp379;_tmp379.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp376,root,place_val),_tmp377);_tmp379.f2=rval;_tmp379;});}_LL2D0:;};};};}case 36: _LL1A1: _tmp179=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL1A2: {
# 1719
struct _tuple17 _tmp37F=({struct _tuple17 _tmp382;_tmp382.f1=(env->fenv)->unknown_all;_tmp382.f2=copy_ctxt;_tmp382;});
union Cyc_CfFlowInfo_FlowInfo _tmp380=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp179,& _tmp37F);
return({struct _tuple16 _tmp381;_tmp381.f1=_tmp380;_tmp381.f2=_tmp37F.f1;_tmp381;});}case 35: _LL1A5: _LL1A6:
# 1724
 goto _LL1A8;case 24: _LL1A7: _LL1A8:
 goto _LL1AA;case 38: _LL1A9: _LL1AA:
 goto _LL1AC;default: _LL1AB: _LL1AC:
# 1728
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_Core_Impossible_exn_struct _tmp384;_tmp384.tag=Cyc_Core_Impossible;_tmp384.f1=({const char*_tmp385="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp385,sizeof(char),31);});_tmp384;});_tmp383;}));}_LL144:;};}
# 1738
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place,struct Cyc_List_List*flds){
# 1747
struct Cyc_CfFlowInfo_FlowEnv*_tmp386=env->fenv;
void*_tmp387=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp388=_tmp387;void*_tmp389;union Cyc_Absyn_Constraint*_tmp38A;union Cyc_Absyn_Constraint*_tmp38B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->tag == 5){_LL2D6: _tmp389=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->f1).elt_typ;_tmp38A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->f1).ptr_atts).bounds;_tmp38B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->f1).ptr_atts).zero_term;_LL2D7: {
# 1750
union Cyc_CfFlowInfo_FlowInfo _tmp38C=f;struct Cyc_Dict_Dict _tmp38D;struct Cyc_List_List*_tmp38E;if((_tmp38C.BottomFL).tag == 1){_LL2DB: _LL2DC:
 return({struct _tuple18 _tmp38F;_tmp38F.f1=f;_tmp38F.f2=Cyc_CfFlowInfo_UnknownL();_tmp38F;});}else{_LL2DD: _tmp38D=((_tmp38C.ReachableFL).val).f1;_tmp38E=((_tmp38C.ReachableFL).val).f2;_LL2DE:
# 1754
 if(Cyc_Tcutil_is_bound_one(_tmp38A)){
struct _tuple15 _tmp390=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmp391=_tmp390;void*_tmp392;struct Cyc_List_List*_tmp393;_LL2E0: _tmp392=_tmp391.f1;_tmp393=_tmp391.f2;_LL2E1:;
retry: {void*_tmp394=_tmp392;enum Cyc_CfFlowInfo_InitLevel _tmp395;void*_tmp396;struct Cyc_List_List*_tmp397;switch(*((int*)_tmp394)){case 8: _LL2E3: _LL2E4:
# 1758
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=({struct Cyc_Core_Impossible_exn_struct _tmp399;_tmp399.tag=Cyc_Core_Impossible;_tmp399.f1=({const char*_tmp39A="named location in anal_derefL";_tag_dyneither(_tmp39A,sizeof(char),30);});_tmp399;});_tmp398;}));case 1: _LL2E5: _LL2E6:
 goto _LL2E8;case 2: _LL2E7: _LL2E8:
# 1761
 e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp39C;_tmp39C.tag=Cyc_CfFlowInfo_NotZero;_tmp39C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E);_tmp39C;});_tmp39B;});goto _LL2E2;case 5: _LL2E9: _tmp396=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp394)->f1)->root;_tmp397=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp394)->f1)->fields;_LL2EA:
# 1764
 if(expanded_place)
# 1767
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp39E;_tmp39E.tag=Cyc_CfFlowInfo_UnknownZ;_tmp39E.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E);_tmp39E;});_tmp39D;});else{
# 1770
void*_tmp39F=e->annot;void*_tmp3A0=_tmp39F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp3A0)->tag == Cyc_CfFlowInfo_UnknownZ){_LL2F2: _LL2F3:
# 1774
 e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3A2;_tmp3A2.tag=Cyc_CfFlowInfo_UnknownZ;_tmp3A2.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E);_tmp3A2;});_tmp3A1;});
goto _LL2F1;}else{_LL2F4: _LL2F5:
# 1777
 e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp3A4;_tmp3A4.tag=Cyc_CfFlowInfo_NotZero;_tmp3A4.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E);_tmp3A4;});_tmp3A3;});
goto _LL2F1;}_LL2F1:;}
# 1781
return({struct _tuple18 _tmp3A5;_tmp3A5.f1=f;_tmp3A5.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3A6=_region_malloc(_tmp386->r,sizeof(*_tmp3A6));_tmp3A6->root=_tmp396;_tmp3A6->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp386->r,_tmp397,flds);_tmp3A6;}));_tmp3A5;});case 0: _LL2EB: _LL2EC:
# 1783
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp3A7;_tmp3A7.f1=Cyc_CfFlowInfo_BottomFL();_tmp3A7.f2=Cyc_CfFlowInfo_UnknownL();_tmp3A7;});case 3: _LL2ED: _tmp395=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp394)->f1;_LL2EE:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp395,_tmp393);goto _LL2F0;default: _LL2EF: _LL2F0:
# 1788
 if(passthrough_consumes){
void*_tmp3A8=_tmp392;void*_tmp3A9;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A8)->tag == 7){_LL2F7: _tmp3A9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A8)->f3;_LL2F8:
 _tmp392=_tmp3A9;goto retry;}else{_LL2F9: _LL2FA:
 goto _LL2F6;}_LL2F6:;}
# 1794
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3AB;_tmp3AB.tag=Cyc_CfFlowInfo_UnknownZ;_tmp3AB.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E);_tmp3AB;});_tmp3AA;});}_LL2E2:;}}else{
# 1798
e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC[0]=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3AD;_tmp3AD.tag=Cyc_CfFlowInfo_UnknownZ;_tmp3AD.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E);_tmp3AD;});_tmp3AC;});}
if(Cyc_CfFlowInfo_initlevel(_tmp386,_tmp38D,r)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp3AE=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmp3AF=_tmp3AE;void*_tmp3B0;_LL2FC: _tmp3B0=_tmp3AF.f1;_LL2FD:;{
void*_tmp3B1=_tmp3B0;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3B1)->tag == 7){_LL2FF: _LL300:
# 1803
({void*_tmp3B2=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp3B3="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp3B3,sizeof(char),66);}),_tag_dyneither(_tmp3B2,sizeof(void*),0));});
goto _LL2FE;}else{_LL301: _LL302:
# 1806
({void*_tmp3B4=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp3B5="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp3B5,sizeof(char),46);}),_tag_dyneither(_tmp3B4,sizeof(void*),0));});
goto _LL2FE;}_LL2FE:;};}
# 1810
return({struct _tuple18 _tmp3B6;_tmp3B6.f1=f;_tmp3B6.f2=Cyc_CfFlowInfo_UnknownL();_tmp3B6;});}_LL2DA:;}}else{_LL2D8: _LL2D9:
# 1812
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3B8;_tmp3B8.tag=Cyc_Core_Impossible;_tmp3B8.f1=({const char*_tmp3B9="left deref of non-pointer-type";_tag_dyneither(_tmp3B9,sizeof(char),31);});_tmp3B8;});_tmp3B7;}));}_LL2D5:;}
# 1820
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1823
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3BA=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3BB=inflow;struct Cyc_Dict_Dict _tmp3BC;struct Cyc_List_List*_tmp3BD;if((_tmp3BB.BottomFL).tag == 1){_LL304: _LL305:
 return({struct _tuple18 _tmp3BE;_tmp3BE.f1=Cyc_CfFlowInfo_BottomFL();_tmp3BE.f2=Cyc_CfFlowInfo_UnknownL();_tmp3BE;});}else{_LL306: _tmp3BC=((_tmp3BB.ReachableFL).val).f1;_tmp3BD=((_tmp3BB.ReachableFL).val).f2;_LL307:
# 1828
 d=_tmp3BC;}_LL303:;}{
# 1832
void*_tmp3BF=e->r;void*_tmp3C0=_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C1;struct _dyneither_ptr*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C6;struct _dyneither_ptr*_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3C9;struct Cyc_Absyn_Vardecl*_tmp3CA;struct Cyc_Absyn_Vardecl*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;switch(*((int*)_tmp3C0)){case 13: _LL309: _tmp3CE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL30A:
 _tmp3CD=_tmp3CE;goto _LL30C;case 11: _LL30B: _tmp3CD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_LL30C:
 _tmp3CC=_tmp3CD;goto _LL30E;case 12: _LL30D: _tmp3CC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_LL30E:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CC,flds);case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)){case 3: _LL30F: _tmp3CB=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL310:
# 1837
 _tmp3CA=_tmp3CB;goto _LL312;case 4: _LL311: _tmp3CA=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL312:
 _tmp3C9=_tmp3CA;goto _LL314;case 5: _LL313: _tmp3C9=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL314:
# 1840
 return({struct _tuple18 _tmp3CF;_tmp3CF.f1=inflow;_tmp3CF.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3D0=_region_malloc(env->r,sizeof(*_tmp3D0));_tmp3D0->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3D1=_region_malloc(env->r,sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3D2;_tmp3D2.tag=0;_tmp3D2.f1=_tmp3C9;_tmp3D2;});_tmp3D1;});_tmp3D0->fields=flds;_tmp3D0;}));_tmp3CF;});case 1: _LL315: _tmp3C8=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL316:
# 1842
 return({struct _tuple18 _tmp3D3;_tmp3D3.f1=inflow;_tmp3D3.f2=Cyc_CfFlowInfo_UnknownL();_tmp3D3;});default: goto _LL31F;}case 21: _LL317: _tmp3C6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_tmp3C7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL318:
# 1845
{void*_tmp3D4=Cyc_Tcutil_compress((void*)_check_null(_tmp3C6->topt));void*_tmp3D5=_tmp3D4;void*_tmp3D6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5)->tag == 5){_LL322: _tmp3D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5)->f1).elt_typ;_LL323:
# 1847
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3D6)){
Cyc_NewControlFlow_check_union_requires(_tmp3C6->loc,_tmp3BA->r,_tmp3D6,_tmp3C7,inflow);
flds=({struct Cyc_List_List*_tmp3D7=_region_malloc(env->r,sizeof(*_tmp3D7));_tmp3D7->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3D6,_tmp3C7);_tmp3D7->tl=flds;_tmp3D7;});}
# 1851
goto _LL321;}else{_LL324: _LL325:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3D9;_tmp3D9.tag=Cyc_Core_Impossible;_tmp3D9.f1=({const char*_tmp3DA="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3DA,sizeof(char),25);});_tmp3D9;});_tmp3D8;}));}_LL321:;}
# 1854
_tmp3C5=_tmp3C6;goto _LL31A;case 19: _LL319: _tmp3C5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_LL31A:
# 1857
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C5->topt),1)){
# 1859
struct _tuple18 _tmp3DB=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C5->topt),1),passthrough_consumes,_tmp3C5);
# 1859
struct _tuple18 _tmp3DC=_tmp3DB;union Cyc_CfFlowInfo_FlowInfo _tmp3DD;union Cyc_CfFlowInfo_AbsLVal _tmp3DE;_LL327: _tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;_LL328:;{
# 1863
struct _tuple18 _tmp3DF=({struct _tuple18 _tmp3F7;_tmp3F7.f1=_tmp3DD;_tmp3F7.f2=_tmp3DE;_tmp3F7;});struct _tuple18 _tmp3E0=_tmp3DF;struct Cyc_Dict_Dict _tmp3E1;struct Cyc_List_List*_tmp3E2;struct Cyc_CfFlowInfo_Place*_tmp3E3;if(((_tmp3E0.f1).ReachableFL).tag == 2){if(((_tmp3E0.f2).PlaceL).tag == 1){_LL32A: _tmp3E1=(((_tmp3E0.f1).ReachableFL).val).f1;_tmp3E2=(((_tmp3E0.f1).ReachableFL).val).f2;_tmp3E3=((_tmp3E0.f2).PlaceL).val;_LL32B: {
# 1865
void*_tmp3E4=Cyc_CfFlowInfo_lookup_place(_tmp3E1,_tmp3E3);
struct _tuple15 _tmp3E5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3E4);struct _tuple15 _tmp3E6=_tmp3E5;void*_tmp3E7;struct Cyc_List_List*_tmp3E8;_LL32F: _tmp3E7=_tmp3E6.f1;_tmp3E8=_tmp3E6.f2;_LL330:;{
void*_tmp3E9=_tmp3E7;struct Cyc_Absyn_Vardecl*_tmp3EA;void*_tmp3EB;switch(*((int*)_tmp3E9)){case 8: _LL332: _tmp3EA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E9)->f1;_tmp3EB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E9)->f2;_LL333:
# 1869
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3ED;_tmp3ED.tag=Cyc_Core_Impossible;_tmp3ED.f1=({const char*_tmp3EE="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3EE,sizeof(char),38);});_tmp3ED;});_tmp3EC;}));case 7: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3E9)->f3)->tag == 5){_LL334: _LL335:
 goto _LL337;}else{goto _LL338;}case 5: _LL336: _LL337:
# 1874
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3DD,_tmp3C5,_tmp3DD,_tmp3E4,passthrough_consumes,0,flds);default: _LL338: _LL339: {
# 1881
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3BA,_tmp3E1,_tmp3E7);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3BA->unknown_all: _tmp3BA->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3BA,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3C5->topt)),0,leaf);
void*new_root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3F5=_region_malloc(_tmp3BA->r,sizeof(*_tmp3F5));_tmp3F5[0]=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp3F6;_tmp3F6.tag=1;_tmp3F6.f1=e;_tmp3F6.f2=(void*)_check_null(e->topt);_tmp3F6;});_tmp3F5;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp3F4=_region_malloc(_tmp3BA->r,sizeof(*_tmp3F4));_tmp3F4->root=new_root;_tmp3F4->fields=0;_tmp3F4;});
void*res=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3F2=_region_malloc(_tmp3BA->r,sizeof(*_tmp3F2));_tmp3F2[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp3F3;_tmp3F3.tag=5;_tmp3F3.f1=place;_tmp3F3;});_tmp3F2;});
for(0;_tmp3E8 != 0;_tmp3E8=_tmp3E8->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3EF=_region_malloc(_tmp3BA->r,sizeof(*_tmp3EF));_tmp3EF[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3F0;_tmp3F0.tag=8;_tmp3F0.f1=(struct Cyc_Absyn_Vardecl*)_tmp3E8->hd;_tmp3F0.f2=res;_tmp3F0;});_tmp3EF;});}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3E1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E1,new_root,new_rval);
_tmp3E1=Cyc_CfFlowInfo_assign_place(_tmp3BA,e->loc,_tmp3E1,env->all_changed,_tmp3E3,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3F1=Cyc_CfFlowInfo_ReachableFL(_tmp3E1,_tmp3E2);
# 1899
return Cyc_NewControlFlow_anal_derefL(env,_tmp3F1,_tmp3C5,_tmp3F1,res,passthrough_consumes,1,flds);};}}_LL331:;};}}else{goto _LL32C;}}else{_LL32C: _LL32D:
# 1902
 goto _LL329;}_LL329:;};}{
# 1905
struct _tuple16 _tmp3F8=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3C5);struct _tuple16 _tmp3F9=_tmp3F8;union Cyc_CfFlowInfo_FlowInfo _tmp3FA;void*_tmp3FB;_LL33B: _tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;_LL33C:;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3C5,_tmp3FA,_tmp3FB,passthrough_consumes,0,flds);};case 22: _LL31B: _tmp3C3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_tmp3C4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL31C: {
# 1909
void*_tmp3FC=Cyc_Tcutil_compress((void*)_check_null(_tmp3C3->topt));void*_tmp3FD=_tmp3FC;union Cyc_Absyn_Constraint*_tmp3FE;switch(*((int*)_tmp3FD)){case 10: _LL33E: _LL33F: {
# 1911
unsigned int _tmp3FF=(Cyc_Evexp_eval_const_uint_exp(_tmp3C4)).f1;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3C3,({struct Cyc_List_List*_tmp400=_region_malloc(env->r,sizeof(*_tmp400));_tmp400->hd=(void*)_tmp3FF;_tmp400->tl=flds;_tmp400;}));}case 5: _LL340: _tmp3FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FD)->f1).ptr_atts).bounds;_LL341: {
# 1914
struct _RegionHandle _tmp401=_new_region("temp");struct _RegionHandle*temp=& _tmp401;_push_region(temp);
{struct _tuple23 _tmp402=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp416[2];_tmp416[1]=_tmp3C4;_tmp416[0]=_tmp3C3;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp416,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);struct _tuple23 _tmp403=_tmp402;union Cyc_CfFlowInfo_FlowInfo _tmp404;struct Cyc_List_List*_tmp405;_LL345: _tmp404=_tmp403.f1;_tmp405=_tmp403.f2;_LL346:;{
# 1917
union Cyc_CfFlowInfo_FlowInfo _tmp406=_tmp404;
{union Cyc_CfFlowInfo_FlowInfo _tmp407=_tmp404;struct Cyc_Dict_Dict _tmp408;struct Cyc_List_List*_tmp409;if((_tmp407.ReachableFL).tag == 2){_LL348: _tmp408=((_tmp407.ReachableFL).val).f1;_tmp409=((_tmp407.ReachableFL).val).f2;_LL349:
# 1920
 if(Cyc_CfFlowInfo_initlevel(_tmp3BA,_tmp408,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp405))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp40A=0;Cyc_CfFlowInfo_aerr(_tmp3C4->loc,({const char*_tmp40B="expression may not be initialized";_tag_dyneither(_tmp40B,sizeof(char),34);}),_tag_dyneither(_tmp40A,sizeof(void*),0));});{
struct Cyc_List_List*_tmp40C=Cyc_NewControlFlow_add_subscript_reln(_tmp3BA->r,_tmp409,_tmp3C3,_tmp3C4);
if(_tmp409 != _tmp40C)
_tmp406=Cyc_CfFlowInfo_ReachableFL(_tmp408,_tmp40C);
goto _LL347;};}else{_LL34A: _LL34B:
 goto _LL347;}_LL347:;}{
# 1928
struct _tuple18 _tmp40D=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3C3,_tmp404,(void*)((struct Cyc_List_List*)_check_null(_tmp405))->hd,passthrough_consumes,0,flds);struct _tuple18 _tmp40E=_tmp40D;union Cyc_CfFlowInfo_FlowInfo _tmp40F;union Cyc_CfFlowInfo_AbsLVal _tmp410;_LL34D: _tmp40F=_tmp40E.f1;_tmp410=_tmp40E.f2;_LL34E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp411=_tmp40F;if((_tmp411.BottomFL).tag == 1){_LL350: _LL351: {
struct _tuple18 _tmp413=({struct _tuple18 _tmp412;_tmp412.f1=_tmp40F;_tmp412.f2=_tmp410;_tmp412;});_npop_handler(0);return _tmp413;}}else{_LL352: _LL353: {
struct _tuple18 _tmp415=({struct _tuple18 _tmp414;_tmp414.f1=_tmp406;_tmp414.f2=_tmp410;_tmp414;});_npop_handler(0);return _tmp415;}}_LL34F:;};};};}
# 1915
;_pop_region(temp);}default: _LL342: _LL343:
# 1934
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Core_Impossible_exn_struct _tmp418;_tmp418.tag=Cyc_Core_Impossible;_tmp418.f1=({const char*_tmp419="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp419,sizeof(char),33);});_tmp418;});_tmp417;}));}_LL33D:;}case 20: _LL31D: _tmp3C1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_tmp3C2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL31E: {
# 1938
void*_tmp41A=(void*)_check_null(_tmp3C1->topt);
if(Cyc_Absyn_is_require_union_type(_tmp41A))
Cyc_NewControlFlow_check_union_requires(_tmp3C1->loc,_tmp3BA->r,_tmp41A,_tmp3C2,inflow);
# 1942
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp41A))
return({struct _tuple18 _tmp41B;_tmp41B.f1=inflow;_tmp41B.f2=Cyc_CfFlowInfo_UnknownL();_tmp41B;});
# 1945
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3C1,({struct Cyc_List_List*_tmp41C=_region_malloc(env->r,sizeof(*_tmp41C));_tmp41C->hd=(void*)
Cyc_CfFlowInfo_get_field_index(_tmp41A,_tmp3C2);_tmp41C->tl=flds;_tmp41C;}));}default: _LL31F: _LL320:
# 1948
 return({struct _tuple18 _tmp41D;_tmp41D.f1=Cyc_CfFlowInfo_BottomFL();_tmp41D.f2=Cyc_CfFlowInfo_UnknownL();_tmp41D;});}_LL308:;};}
# 1952
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1955
struct _tuple18 _tmp41E=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp41F=_tmp41E;union Cyc_CfFlowInfo_FlowInfo _tmp420;union Cyc_CfFlowInfo_AbsLVal _tmp421;_LL355: _tmp420=_tmp41F.f1;_tmp421=_tmp41F.f2;_LL356:;
return({struct _tuple18 _tmp422;_tmp422.f1=_tmp420;_tmp422.f2=_tmp421;_tmp422;});}
# 1962
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1965
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp423=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp424=_tmp423;union Cyc_CfFlowInfo_FlowInfo _tmp425;_LL358: _tmp425=_tmp424.f1;_LL359:;
inflow=_tmp425;}}}
# 1973
return inflow;}
# 1979
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1982
struct Cyc_CfFlowInfo_FlowEnv*_tmp426=env->fenv;
# 1985
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp427=_new_region("temp");struct _RegionHandle*temp=& _tmp427;_push_region(temp);{
struct _tuple23 _tmp428=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);struct _tuple23 _tmp429=_tmp428;union Cyc_CfFlowInfo_FlowInfo _tmp42A;struct Cyc_List_List*_tmp42B;_LL35B: _tmp42A=_tmp429.f1;_tmp42B=_tmp429.f2;_LL35C:;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp42B))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp42B->tl))->hd;
f=_tmp42A;}{
# 1995
union Cyc_CfFlowInfo_FlowInfo _tmp42C=f;struct Cyc_Dict_Dict _tmp42D;struct Cyc_List_List*_tmp42E;if((_tmp42C.BottomFL).tag == 1){_LL35E: _LL35F: {
struct _tuple19 _tmp430=({struct _tuple19 _tmp42F;_tmp42F.f1=f;_tmp42F.f2=f;_tmp42F;});_npop_handler(0);return _tmp430;}}else{_LL360: _tmp42D=((_tmp42C.ReachableFL).val).f1;_tmp42E=((_tmp42C.ReachableFL).val).f2;_LL361: {
# 1998
struct Cyc_Absyn_Exp*_tmp431=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp432=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42D,r1)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp433=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,({const char*_tmp434="expression may not be initialized";_tag_dyneither(_tmp434,sizeof(char),34);}),_tag_dyneither(_tmp433,sizeof(void*),0));});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42D,r2)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp435=0;Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,({const char*_tmp436="expression may not be initialized";_tag_dyneither(_tmp436,sizeof(char),34);}),_tag_dyneither(_tmp435,sizeof(void*),0));});{
# 2005
union Cyc_CfFlowInfo_FlowInfo _tmp437=f;
union Cyc_CfFlowInfo_FlowInfo _tmp438=f;
# 2010
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp439=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);struct _tuple15 _tmp43A=_tmp439;void*_tmp43B;struct Cyc_List_List*_tmp43C;_LL363: _tmp43B=_tmp43A.f1;_tmp43C=_tmp43A.f2;_LL364:;{
struct _tuple15 _tmp43D=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);struct _tuple15 _tmp43E=_tmp43D;void*_tmp43F;struct Cyc_List_List*_tmp440;_LL366: _tmp43F=_tmp43E.f1;_tmp440=_tmp43E.f2;_LL367:;{
struct _tuple0 _tmp441=({struct _tuple0 _tmp455;_tmp455.f1=_tmp43B;_tmp455.f2=_tmp43F;_tmp455;});struct _tuple0 _tmp442=_tmp441;enum Cyc_CfFlowInfo_InitLevel _tmp443;enum Cyc_CfFlowInfo_InitLevel _tmp444;switch(*((int*)_tmp442.f1)){case 3: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL369: _tmp444=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp442.f1)->f1;_LL36A: {
# 2017
struct _tuple19 _tmp445=Cyc_NewControlFlow_splitzero(env,f,f,_tmp431,_tmp444,_tmp43C);struct _tuple19 _tmp446=_tmp445;union Cyc_CfFlowInfo_FlowInfo _tmp447;union Cyc_CfFlowInfo_FlowInfo _tmp448;_LL37E: _tmp447=_tmp446.f1;_tmp448=_tmp446.f2;_LL37F:;
{enum Cyc_Absyn_Primop _tmp449=p;switch(_tmp449){case Cyc_Absyn_Eq: _LL381: _LL382:
 _tmp437=_tmp448;_tmp438=_tmp447;goto _LL380;case Cyc_Absyn_Neq: _LL383: _LL384:
 _tmp437=_tmp447;_tmp438=_tmp448;goto _LL380;default: _LL385: _LL386:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=({struct Cyc_Core_Impossible_exn_struct _tmp44B;_tmp44B.tag=Cyc_Core_Impossible;_tmp44B.f1=({const char*_tmp44C="anal_test, zero-split";_tag_dyneither(_tmp44C,sizeof(char),22);});_tmp44B;});_tmp44A;}));}_LL380:;}
# 2023
goto _LL368;}}else{goto _LL37B;}case 0: switch(*((int*)_tmp442.f2)){case 3: _LL36B: _tmp443=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->f1;_LL36C: {
# 2025
struct _tuple19 _tmp44D=Cyc_NewControlFlow_splitzero(env,f,f,_tmp432,_tmp443,_tmp440);struct _tuple19 _tmp44E=_tmp44D;union Cyc_CfFlowInfo_FlowInfo _tmp44F;union Cyc_CfFlowInfo_FlowInfo _tmp450;_LL388: _tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;_LL389:;
{enum Cyc_Absyn_Primop _tmp451=p;switch(_tmp451){case Cyc_Absyn_Eq: _LL38B: _LL38C:
 _tmp437=_tmp450;_tmp438=_tmp44F;goto _LL38A;case Cyc_Absyn_Neq: _LL38D: _LL38E:
 _tmp437=_tmp44F;_tmp438=_tmp450;goto _LL38A;default: _LL38F: _LL390:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452[0]=({struct Cyc_Core_Impossible_exn_struct _tmp453;_tmp453.tag=Cyc_Core_Impossible;_tmp453.f1=({const char*_tmp454="anal_test, zero-split";_tag_dyneither(_tmp454,sizeof(char),22);});_tmp453;});_tmp452;}));}_LL38A:;}
# 2031
goto _LL368;}case 0: _LL36D: _LL36E:
# 2033
 if(p == Cyc_Absyn_Eq)_tmp438=Cyc_CfFlowInfo_BottomFL();else{
_tmp437=Cyc_CfFlowInfo_BottomFL();}
goto _LL368;case 1: _LL36F: _LL370:
 goto _LL372;case 2: _LL371: _LL372:
 goto _LL374;case 5: _LL373: _LL374:
 goto _LL376;default: goto _LL37B;}case 1: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL375: _LL376:
 goto _LL378;}else{goto _LL37B;}case 2: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL377: _LL378:
 goto _LL37A;}else{goto _LL37B;}case 5: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL379: _LL37A:
# 2042
 if(p == Cyc_Absyn_Neq)_tmp438=Cyc_CfFlowInfo_BottomFL();else{
_tmp437=Cyc_CfFlowInfo_BottomFL();}
goto _LL368;}else{goto _LL37B;}default: _LL37B: _LL37C:
 goto _LL368;}_LL368:;};};}
# 2053
{struct _tuple0 _tmp456=({struct _tuple0 _tmp45A;_tmp45A.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp431->topt));_tmp45A.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp432->topt));_tmp45A;});struct _tuple0 _tmp457=_tmp456;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f1)->f1 == Cyc_Absyn_Unsigned){_LL392: _LL393:
 goto _LL395;}else{switch(*((int*)_tmp457.f2)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL394;else{goto _LL39A;}case 19: goto _LL398;default: goto _LL39A;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f1 == Cyc_Absyn_Unsigned){_LL394: _LL395:
 goto _LL397;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f1)->tag == 19)goto _LL396;else{goto _LL39A;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f1)->tag == 19){_LL396: _LL397:
 goto _LL399;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f2)->tag == 19){_LL398: _LL399:
 goto _LL391;}else{_LL39A: _LL39B: {
struct _tuple19 _tmp459=({struct _tuple19 _tmp458;_tmp458.f1=_tmp437;_tmp458.f2=_tmp438;_tmp458;});_npop_handler(0);return _tmp459;}}}}}_LL391:;}
# 2062
{enum Cyc_Absyn_Primop _tmp45B=p;switch(_tmp45B){case Cyc_Absyn_Eq: _LL39D: _LL39E:
 goto _LL3A0;case Cyc_Absyn_Neq: _LL39F: _LL3A0: goto _LL3A2;case Cyc_Absyn_Gt: _LL3A1: _LL3A2: goto _LL3A4;case Cyc_Absyn_Gte: _LL3A3: _LL3A4: goto _LL3A6;case Cyc_Absyn_Lt: _LL3A5: _LL3A6: goto _LL3A8;case Cyc_Absyn_Lte: _LL3A7: _LL3A8: goto _LL39C;default: _LL3A9: _LL3AA: {
struct _tuple19 _tmp45D=({struct _tuple19 _tmp45C;_tmp45C.f1=_tmp437;_tmp45C.f2=_tmp438;_tmp45C;});_npop_handler(0);return _tmp45D;}}_LL39C:;}{
# 2067
struct _RegionHandle*_tmp45E=(env->fenv)->r;
struct _tuple13 _tmp45F=Cyc_Relations_primop2relation(_tmp431,p,_tmp432);struct _tuple13 _tmp460=_tmp45F;struct Cyc_Absyn_Exp*_tmp461;enum Cyc_Relations_Relation _tmp462;struct Cyc_Absyn_Exp*_tmp463;_LL3AC: _tmp461=_tmp460.f1;_tmp462=_tmp460.f2;_tmp463=_tmp460.f3;_LL3AD:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2072
if(Cyc_Relations_exp2relnop(_tmp461,& n1) && Cyc_Relations_exp2relnop(_tmp463,& n2)){
# 2074
struct Cyc_List_List*_tmp464=Cyc_Relations_add_relation(_tmp45E,n1,_tmp462,n2,_tmp42E);
# 2078
struct Cyc_List_List*_tmp465=Cyc_Relations_add_relation(_tmp45E,n2,Cyc_Relations_flip_relation(_tmp462),n1,_tmp42E);
struct _tuple19 _tmp466=({struct _tuple19 _tmp474;_tmp474.f1=_tmp437;_tmp474.f2=_tmp438;_tmp474;});struct _tuple19 _tmp467=_tmp466;struct Cyc_Dict_Dict _tmp468;struct Cyc_Dict_Dict _tmp469;struct Cyc_Dict_Dict _tmp46A;struct Cyc_Dict_Dict _tmp46B;if(((_tmp467.f1).ReachableFL).tag == 2){if(((_tmp467.f2).ReachableFL).tag == 2){_LL3AF: _tmp46A=(((_tmp467.f1).ReachableFL).val).f1;_tmp46B=(((_tmp467.f2).ReachableFL).val).f1;_LL3B0: {
# 2081
struct _tuple19 _tmp46D=({struct _tuple19 _tmp46C;_tmp46C.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46A,_tmp464);_tmp46C.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46B,_tmp465);_tmp46C;});_npop_handler(0);return _tmp46D;}}else{_LL3B3: _tmp469=(((_tmp467.f1).ReachableFL).val).f1;_LL3B4: {
# 2085
struct _tuple19 _tmp471=({struct _tuple19 _tmp470;_tmp470.f1=Cyc_CfFlowInfo_ReachableFL(_tmp469,_tmp464);_tmp470.f2=_tmp438;_tmp470;});_npop_handler(0);return _tmp471;}}}else{if(((_tmp467.f2).ReachableFL).tag == 2){_LL3B1: _tmp468=(((_tmp467.f2).ReachableFL).val).f1;_LL3B2: {
# 2083
struct _tuple19 _tmp46F=({struct _tuple19 _tmp46E;_tmp46E.f1=_tmp437;_tmp46E.f2=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp465);_tmp46E;});_npop_handler(0);return _tmp46F;}}else{_LL3B5: _LL3B6: {
# 2087
struct _tuple19 _tmp473=({struct _tuple19 _tmp472;_tmp472.f1=_tmp437;_tmp472.f2=_tmp438;_tmp472;});_npop_handler(0);return _tmp473;}}}_LL3AE:;}else{
# 2090
struct _tuple19 _tmp476=({struct _tuple19 _tmp475;_tmp475.f1=_tmp437;_tmp475.f2=_tmp438;_tmp475;});_npop_handler(0);return _tmp476;}};};};}}_LL35D:;};
# 1987
;_pop_region(temp);}
# 2096
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2098
struct Cyc_CfFlowInfo_FlowEnv*_tmp477=env->fenv;
void*_tmp478=e->r;void*_tmp479=_tmp478;enum Cyc_Absyn_Primop _tmp47A;struct Cyc_List_List*_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*_tmp485;switch(*((int*)_tmp479)){case 5: _LL3B8: _tmp483=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp484=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_tmp485=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp479)->f3;_LL3B9: {
# 2101
struct _tuple19 _tmp486=Cyc_NewControlFlow_anal_test(env,inflow,_tmp483);struct _tuple19 _tmp487=_tmp486;union Cyc_CfFlowInfo_FlowInfo _tmp488;union Cyc_CfFlowInfo_FlowInfo _tmp489;_LL3C7: _tmp488=_tmp487.f1;_tmp489=_tmp487.f2;_LL3C8:;{
struct _tuple19 _tmp48A=Cyc_NewControlFlow_anal_test(env,_tmp488,_tmp484);struct _tuple19 _tmp48B=_tmp48A;union Cyc_CfFlowInfo_FlowInfo _tmp48C;union Cyc_CfFlowInfo_FlowInfo _tmp48D;_LL3CA: _tmp48C=_tmp48B.f1;_tmp48D=_tmp48B.f2;_LL3CB:;{
struct _tuple19 _tmp48E=Cyc_NewControlFlow_anal_test(env,_tmp489,_tmp485);struct _tuple19 _tmp48F=_tmp48E;union Cyc_CfFlowInfo_FlowInfo _tmp490;union Cyc_CfFlowInfo_FlowInfo _tmp491;_LL3CD: _tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;_LL3CE:;
return({struct _tuple19 _tmp492;_tmp492.f1=Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp48C,_tmp490);_tmp492.f2=
Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp48D,_tmp491);_tmp492;});};};}case 6: _LL3BA: _tmp481=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp482=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL3BB: {
# 2107
struct _tuple19 _tmp493=Cyc_NewControlFlow_anal_test(env,inflow,_tmp481);struct _tuple19 _tmp494=_tmp493;union Cyc_CfFlowInfo_FlowInfo _tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp496;_LL3D0: _tmp495=_tmp494.f1;_tmp496=_tmp494.f2;_LL3D1:;{
struct _tuple19 _tmp497=Cyc_NewControlFlow_anal_test(env,_tmp495,_tmp482);struct _tuple19 _tmp498=_tmp497;union Cyc_CfFlowInfo_FlowInfo _tmp499;union Cyc_CfFlowInfo_FlowInfo _tmp49A;_LL3D3: _tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;_LL3D4:;
return({struct _tuple19 _tmp49B;_tmp49B.f1=_tmp499;_tmp49B.f2=Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp496,_tmp49A);_tmp49B;});};}case 7: _LL3BC: _tmp47F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp480=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL3BD: {
# 2111
struct _tuple19 _tmp49C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47F);struct _tuple19 _tmp49D=_tmp49C;union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49F;_LL3D6: _tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;_LL3D7:;{
struct _tuple19 _tmp4A0=Cyc_NewControlFlow_anal_test(env,_tmp49F,_tmp480);struct _tuple19 _tmp4A1=_tmp4A0;union Cyc_CfFlowInfo_FlowInfo _tmp4A2;union Cyc_CfFlowInfo_FlowInfo _tmp4A3;_LL3D9: _tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;_LL3DA:;
return({struct _tuple19 _tmp4A4;_tmp4A4.f1=Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp49E,_tmp4A2);_tmp4A4.f2=_tmp4A3;_tmp4A4;});};}case 8: _LL3BE: _tmp47D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp47E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL3BF: {
# 2115
struct _tuple16 _tmp4A5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp47D);struct _tuple16 _tmp4A6=_tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A7;void*_tmp4A8;_LL3DC: _tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;_LL3DD:;
return Cyc_NewControlFlow_anal_test(env,_tmp4A7,_tmp47E);}case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2)->tl == 0){_LL3C0: _tmp47C=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2)->hd;_LL3C1: {
# 2118
struct _tuple19 _tmp4A9=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47C);struct _tuple19 _tmp4AA=_tmp4A9;union Cyc_CfFlowInfo_FlowInfo _tmp4AB;union Cyc_CfFlowInfo_FlowInfo _tmp4AC;_LL3DF: _tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;_LL3E0:;
return({struct _tuple19 _tmp4AD;_tmp4AD.f1=_tmp4AC;_tmp4AD.f2=_tmp4AB;_tmp4AD;});}}else{goto _LL3C2;}}else{goto _LL3C2;}}else{_LL3C2: _tmp47A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp47B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL3C3:
# 2121
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp47A,_tmp47B);}default: _LL3C4: _LL3C5: {
# 2125
struct _tuple16 _tmp4AE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp4AF=_tmp4AE;union Cyc_CfFlowInfo_FlowInfo _tmp4B0;void*_tmp4B1;_LL3E2: _tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;_LL3E3:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B2=_tmp4B0;struct Cyc_Dict_Dict _tmp4B3;if((_tmp4B2.BottomFL).tag == 1){_LL3E5: _LL3E6:
 return({struct _tuple19 _tmp4B4;_tmp4B4.f1=_tmp4B0;_tmp4B4.f2=_tmp4B0;_tmp4B4;});}else{_LL3E7: _tmp4B3=((_tmp4B2.ReachableFL).val).f1;_LL3E8: {
# 2129
struct _tuple15 _tmp4B5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp4B1);struct _tuple15 _tmp4B6=_tmp4B5;void*_tmp4B7;struct Cyc_List_List*_tmp4B8;_LL3EA: _tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;_LL3EB:;{
void*_tmp4B9=_tmp4B7;enum Cyc_CfFlowInfo_InitLevel _tmp4BA;struct Cyc_Absyn_Vardecl*_tmp4BB;void*_tmp4BC;switch(*((int*)_tmp4B9)){case 8: _LL3ED: _tmp4BB=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1;_tmp4BC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f2;_LL3EE:
# 2132
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4BE;_tmp4BE.tag=Cyc_Core_Impossible;_tmp4BE.f1=({const char*_tmp4BF="anal_test: bad namedlocation";_tag_dyneither(_tmp4BF,sizeof(char),29);});_tmp4BE;});_tmp4BD;}));case 0: _LL3EF: _LL3F0:
 return({struct _tuple19 _tmp4C0;_tmp4C0.f1=Cyc_CfFlowInfo_BottomFL();_tmp4C0.f2=_tmp4B0;_tmp4C0;});case 2: _LL3F1: _LL3F2:
 goto _LL3F4;case 1: _LL3F3: _LL3F4:
 goto _LL3F6;case 5: _LL3F5: _LL3F6:
 return({struct _tuple19 _tmp4C1;_tmp4C1.f1=_tmp4B0;_tmp4C1.f2=Cyc_CfFlowInfo_BottomFL();_tmp4C1;});case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1 == Cyc_CfFlowInfo_NoneIL){_LL3F7: _LL3F8:
 goto _LL3FA;}else{_LL3FD: _tmp4BA=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1;_LL3FE:
# 2142
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4B0,e,_tmp4BA,_tmp4B8);}case 4: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1 == Cyc_CfFlowInfo_NoneIL){_LL3F9: _LL3FA:
# 2138
 goto _LL3FC;}else{_LL3FF: _LL400:
# 2143
 return({struct _tuple19 _tmp4C5;_tmp4C5.f1=_tmp4B0;_tmp4C5.f2=_tmp4B0;_tmp4C5;});}case 7: _LL3FB: _LL3FC:
# 2140
({void*_tmp4C2=0;Cyc_CfFlowInfo_aerr(e->loc,({const char*_tmp4C3="expression may not be initialized";_tag_dyneither(_tmp4C3,sizeof(char),34);}),_tag_dyneither(_tmp4C2,sizeof(void*),0));});
return({struct _tuple19 _tmp4C4;_tmp4C4.f1=Cyc_CfFlowInfo_BottomFL();_tmp4C4.f2=Cyc_CfFlowInfo_BottomFL();_tmp4C4;});default: _LL401: _LL402:
# 2144
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4C7;_tmp4C7.tag=Cyc_Core_Impossible;_tmp4C7.f1=({const char*_tmp4C8="anal_test";_tag_dyneither(_tmp4C8,sizeof(char),10);});_tmp4C7;});_tmp4C6;}));}_LL3EC:;};}}_LL3E4:;};}}_LL3B7:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2150
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2152
struct _tuple27*_tmp4C9=ckenv;unsigned int _tmp4CA;struct Cyc_NewControlFlow_AnalEnv*_tmp4CB;struct Cyc_Dict_Dict _tmp4CC;_LL404: _tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f2;_tmp4CC=_tmp4C9->f3;_LL405:;{
void*_tmp4CD=root;struct Cyc_Absyn_Vardecl*_tmp4CE;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4CD)->tag == 0){_LL407: _tmp4CE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4CD)->f1;_LL408:
# 2155
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4CB->fenv)->r,_tmp4CE->type)){
struct _tuple15 _tmp4CF=Cyc_CfFlowInfo_unname_rval((_tmp4CB->fenv)->r,rval);struct _tuple15 _tmp4D0=_tmp4CF;void*_tmp4D1;_LL40C: _tmp4D1=_tmp4D0.f1;_LL40D:;{
void*_tmp4D2=_tmp4D1;switch(*((int*)_tmp4D2)){case 7: _LL40F: _LL410:
 goto _LL412;case 0: _LL411: _LL412:
 goto _LL414;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL413: _LL414:
 goto _LL40E;}else{goto _LL415;}default: _LL415: _LL416:
# 2162
({struct Cyc_String_pa_PrintArg_struct _tmp4D5;_tmp4D5.tag=0;_tmp4D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4CE->name));({void*_tmp4D3[1]={& _tmp4D5};Cyc_Tcutil_warn(_tmp4CA,({const char*_tmp4D4="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4D4,sizeof(char),57);}),_tag_dyneither(_tmp4D3,sizeof(void*),1));});});
goto _LL40E;}_LL40E:;};}
# 2166
goto _LL406;}else{_LL409: _LL40A:
 goto _LL406;}_LL406:;};}
# 2171
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4D6=flow;struct Cyc_Dict_Dict _tmp4D7;if((_tmp4D6.BottomFL).tag == 1){_LL418: _LL419:
 return;}else{_LL41A: _tmp4D7=((_tmp4D6.ReachableFL).val).f1;_LL41B:
# 2175
{struct Cyc_List_List*_tmp4D8=env->param_roots;for(0;_tmp4D8 != 0;_tmp4D8=_tmp4D8->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D7,Cyc_CfFlowInfo_lookup_place(_tmp4D7,(struct Cyc_CfFlowInfo_Place*)_tmp4D8->hd))!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4D9=0;Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4DA="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4DA,sizeof(char),76);}),_tag_dyneither(_tmp4D9,sizeof(void*),0));});}}
# 2179
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4DB=({struct _tuple27 _tmp4DC;_tmp4DC.f1=loc;_tmp4DC.f2=env;_tmp4DC.f3=_tmp4D7;_tmp4DC;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4DB,_tmp4D7);}
# 2183
return;}_LL417:;}
# 2192
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2195
struct Cyc_List_List*_tmp4DD=0;
struct Cyc_List_List*_tmp4DE=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4DF=(struct _tuple24*)x->hd;struct _tuple24*_tmp4E0=_tmp4DF;struct Cyc_Absyn_Vardecl**_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;_LL41D: _tmp4E1=_tmp4E0->f1;_tmp4E2=_tmp4E0->f2;_LL41E:;
if((_tmp4E1 != 0  && _tmp4E2 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp4E2->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E3=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E8=_region_malloc(frgn,sizeof(*_tmp4E8));_tmp4E8[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp4E9;_tmp4E9.tag=0;_tmp4E9.f1=*_tmp4E1;_tmp4E9;});_tmp4E8;});
struct Cyc_CfFlowInfo_Place*_tmp4E4=({struct Cyc_CfFlowInfo_Place*_tmp4E7=_region_malloc(frgn,sizeof(*_tmp4E7));_tmp4E7->root=(void*)_tmp4E3;_tmp4E7->fields=0;_tmp4E7;});
_tmp4DD=({struct Cyc_List_List*_tmp4E5=_region_malloc(frgn,sizeof(*_tmp4E5));_tmp4E5->hd=_tmp4E4;_tmp4E5->tl=_tmp4DD;_tmp4E5;});
_tmp4DE=({struct Cyc_List_List*_tmp4E6=_region_malloc(frgn,sizeof(*_tmp4E6));_tmp4E6->hd=_tmp4E2;_tmp4E6->tl=_tmp4DE;_tmp4E6;});}}}
# 2207
return({struct _tuple1 _tmp4EA;_tmp4EA.f1=_tmp4DD;_tmp4EA.f2=_tmp4DE;_tmp4EA;});}struct _tuple28{int f1;void*f2;};
# 2213
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2220
union Cyc_CfFlowInfo_FlowInfo _tmp4EB=flow;struct Cyc_Dict_Dict _tmp4EC;if((_tmp4EB.BottomFL).tag == 1){_LL420: _LL421:
# 2222
({void*_tmp4ED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4EE="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp4EE,sizeof(char),40);}),_tag_dyneither(_tmp4ED,sizeof(void*),0));});}else{_LL422: _tmp4EC=((_tmp4EB.ReachableFL).val).f1;_LL423: {
# 2229
struct Cyc_Absyn_Exp*_tmp4EF=Cyc_Absyn_exp_unsigned_one;
int _tmp4F0=0;
int _tmp4F1=1;
void*_tmp4F2=Cyc_CfFlowInfo_lookup_place(_tmp4EC,place);
void*_tmp4F3=_tmp4F2;
# 2241
int varok=0;
{void*_tmp4F4=_tmp4F2;struct Cyc_Absyn_Vardecl*_tmp4F5;void*_tmp4F6;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F4)->tag == 8){_LL425: _tmp4F5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F4)->f1;_tmp4F6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F4)->f2;_LL426:
# 2244
 if(vd == _tmp4F5){
_tmp4F3=_tmp4F6;
# 2247
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
# 2249
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4EF,_tmp4F1,_tmp4F3,& _tmp4F0)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0;_tmp4F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2253
Cyc_Absynpp_qvar2string(vd->name));({void*_tmp4F7[1]={& _tmp4F9};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4F8="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp4F8,sizeof(char),58);}),_tag_dyneither(_tmp4F7,sizeof(void*),1));});});}else{
# 2256
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4EC,_tmp4F3)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0;_tmp4FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2259
Cyc_Absynpp_qvar2string(vd->name));({void*_tmp4FA[1]={& _tmp4FC};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4FB="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp4FB,sizeof(char),84);}),_tag_dyneither(_tmp4FA,sizeof(void*),1));});});else{
# 2261
varok=1;}}}else{
# 2264
varok=1;}}else{
# 2267
goto _LL428;}
goto _LL424;}else{_LL427: _LL428:
# 2271
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0;_tmp4FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2276
Cyc_Absynpp_qvar2string(vd->name));({void*_tmp4FD[1]={& _tmp4FF};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp4FE="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp4FE,sizeof(char),85);}),_tag_dyneither(_tmp4FD,sizeof(void*),1));});});}
goto _LL424;}_LL424:;}
# 2280
return({struct _tuple28 _tmp500;_tmp500.f1=varok;_tmp500.f2=_tmp4F3;_tmp500;});}}_LL41F:;}
# 2286
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp501=p;void*_tmp502;struct Cyc_List_List*_tmp503;_LL42A: _tmp502=_tmp501->root;_tmp503=_tmp501->fields;_LL42B:;
if(_tmp503 != 0)
({void*_tmp504=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp505="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp505,sizeof(char),53);}),_tag_dyneither(_tmp504,sizeof(void*),0));});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp506=_tmp502;struct Cyc_Absyn_Vardecl*_tmp507;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp506)->tag == 0){_LL42D: _tmp507=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp506)->f1;_LL42E:
 return _tmp507;}else{_LL42F: _LL430:
({void*_tmp508=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp509="unconsume_params: root is not a varroot";_tag_dyneither(_tmp509,sizeof(char),40);}),_tag_dyneither(_tmp508,sizeof(void*),0));});}_LL42C:;};}
# 2305 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2313
{union Cyc_CfFlowInfo_FlowInfo _tmp50A=flow;struct Cyc_Dict_Dict _tmp50B;if((_tmp50A.BottomFL).tag == 1){_LL432: _LL433:
 return flow;}else{_LL434: _tmp50B=((_tmp50A.ReachableFL).val).f1;_LL435: {
# 2320
struct _tuple18 _tmp50C=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp50D=_tmp50C;union Cyc_CfFlowInfo_FlowInfo _tmp50E;union Cyc_CfFlowInfo_AbsLVal _tmp50F;_LL437: _tmp50E=_tmp50D.f1;_tmp50F=_tmp50D.f2;_LL438:;
# 2323
{union Cyc_CfFlowInfo_AbsLVal _tmp510=_tmp50F;struct Cyc_CfFlowInfo_Place*_tmp511;if((_tmp510.PlaceL).tag == 1){_LL43A: _tmp511=(_tmp510.PlaceL).val;_LL43B: {
# 2327
void*_tmp512=Cyc_CfFlowInfo_lookup_place(_tmp50B,_tmp511);
{void*_tmp513=_tmp512;struct Cyc_Absyn_Vardecl*_tmp514;void*_tmp515;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp513)->tag == 8){_LL43F: _tmp514=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp513)->f1;_tmp515=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp513)->f2;_LL440: {
# 2333
void*new_rval;
if(_tmp514 == vd){
# 2337
if(varok){
# 2339
_tmp512=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp515);
# 2344
if(ropt != 0){
# 2350
struct _tuple16 _tmp516=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,({struct _tuple16 _tmp51A;_tmp51A.f1=_tmp50E;_tmp51A.f2=_tmp512;_tmp51A;}),({struct _tuple16 _tmp51B;_tmp51B.f1=_tmp50E;_tmp51B.f2=ropt;_tmp51B;}));
# 2350
struct _tuple16 _tmp517=_tmp516;union Cyc_CfFlowInfo_FlowInfo _tmp518;void*_tmp519;_LL444: _tmp518=_tmp517.f1;_tmp519=_tmp517.f2;_LL445:;
# 2355
_tmp50E=_tmp518;new_rval=_tmp519;}else{
# 2360
new_rval=_tmp512;}}else{
# 2363
new_rval=_tmp515;}
# 2365
{union Cyc_CfFlowInfo_FlowInfo _tmp51C=_tmp50E;struct Cyc_Dict_Dict _tmp51D;struct Cyc_List_List*_tmp51E;if((_tmp51C.ReachableFL).tag == 2){_LL447: _tmp51D=((_tmp51C.ReachableFL).val).f1;_tmp51E=((_tmp51C.ReachableFL).val).f2;_LL448:
# 2367
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp51D,env->all_changed,_tmp511,new_rval),_tmp51E);
# 2371
goto _LL446;}else{_LL449: _LL44A:
# 2373
({void*_tmp51F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp520="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp520,sizeof(char),42);}),_tag_dyneither(_tmp51F,sizeof(void*),0));});}_LL446:;}
# 2375
goto _LL43E;}else{
# 2377
goto _LL442;}
goto _LL43E;}}else{_LL441: _LL442:
# 2384
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0;_tmp523.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));({void*_tmp521[1]={& _tmp523};Cyc_CfFlowInfo_aerr(loc,({const char*_tmp522="aliased expression %s was overwritten";_tag_dyneither(_tmp522,sizeof(char),38);}),_tag_dyneither(_tmp521,sizeof(void*),1));});});
# 2401 "new_control_flow.cyc"
goto _LL43E;}_LL43E:;}
# 2403
goto _LL439;}}else{_LL43C: _LL43D:
# 2409
 goto _LL439;}_LL439:;}
# 2411
goto _LL431;}}_LL431:;}
# 2413
return flow;}
# 2426 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2432
{union Cyc_CfFlowInfo_FlowInfo _tmp524=flow;if((_tmp524.BottomFL).tag == 1){_LL44C: _LL44D:
 return flow;}else{_LL44E: _LL44F:
 goto _LL44B;}_LL44B:;}{
# 2436
int _tmp525=unconsume_exps != 0;
{struct Cyc_List_List*_tmp526=consumed_vals;for(0;_tmp526 != 0;
(_tmp526=_tmp526->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2443
struct Cyc_Absyn_Vardecl*_tmp527=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp526->hd);
struct _tuple28 _tmp528=
is_region_open?({struct _tuple28 _tmp52C;_tmp52C.f1=1;_tmp52C.f2=0;_tmp52C;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp526->hd,_tmp525,_tmp527,flow,loc);
# 2444
struct _tuple28 _tmp529=_tmp528;int _tmp52A;void*_tmp52B;_LL451: _tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;_LL452:;
# 2450
if(_tmp525)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,_tmp527,_tmp52A,_tmp52B,flow,loc);}}
# 2453
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2457
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2460
struct Cyc_CfFlowInfo_FlowEnv*_tmp52D=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp52E=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp52F=_tmp52E;struct Cyc_Core_Opt*_tmp530;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Stmt*_tmp532;unsigned int _tmp533;_LL454: _tmp530=_tmp52F->pat_vars;_tmp531=_tmp52F->where_clause;_tmp532=_tmp52F->body;_tmp533=_tmp52F->loc;_LL455:;{
struct _tuple1 _tmp534=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp530))->v);struct _tuple1 _tmp535=_tmp534;struct Cyc_List_List*_tmp536;struct Cyc_List_List*_tmp537;_LL457: _tmp536=_tmp535.f1;_tmp537=_tmp535.f2;_LL458:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp530->v,_tmp536 != 0,_tmp533,exp_loc);
# 2468
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp538=env->unique_pat_vars;
env->unique_pat_vars=({struct Cyc_List_List*_tmp539=_region_malloc(env->r,sizeof(*_tmp539));_tmp539->hd=({struct _tuple29*_tmp53A=_region_malloc(env->r,sizeof(*_tmp53A));_tmp53A->f1=0;_tmp53A->f2=_tmp532;_tmp53A->f3=_tmp536;_tmp53A->f4=_tmp537;_tmp53A;});_tmp539->tl=_tmp538;_tmp539;});
# 2473
if(_tmp531 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp531;
struct _tuple19 _tmp53B=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp53C=_tmp53B;union Cyc_CfFlowInfo_FlowInfo _tmp53D;union Cyc_CfFlowInfo_FlowInfo _tmp53E;_LL45A: _tmp53D=_tmp53C.f1;_tmp53E=_tmp53C.f2;_LL45B:;
inflow=_tmp53E;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp53D,_tmp532,0);}else{
# 2479
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp532,0);}
# 2481
env->unique_pat_vars=_tmp538;{
union Cyc_CfFlowInfo_FlowInfo _tmp53F=clause_outflow;if((_tmp53F.BottomFL).tag == 1){_LL45D: _LL45E:
 goto _LL45C;}else{_LL45F: _LL460:
# 2486
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp536,_tmp537,0,clause_outflow,_tmp533);
# 2488
if(scs->tl == 0)
return clause_outflow;else{
# 2493
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp540=0;Cyc_CfFlowInfo_aerr(_tmp532->loc,({const char*_tmp541="switch clause may implicitly fallthru";_tag_dyneither(_tmp541,sizeof(char),38);}),_tag_dyneither(_tmp540,sizeof(void*),0));});else{
# 2496
({void*_tmp542=0;Cyc_Tcutil_warn(_tmp532->loc,({const char*_tmp543="switch clause may implicitly fallthru";_tag_dyneither(_tmp543,sizeof(char),38);}),_tag_dyneither(_tmp542,sizeof(void*),0));});}
# 2498
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2500
goto _LL45C;}_LL45C:;};};};}
# 2503
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2508
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2510
struct _tuple30*_tmp544=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp545;struct Cyc_Dict_Dict _tmp546;unsigned int _tmp547;_LL462: _tmp545=_tmp544->f1;_tmp546=_tmp544->f2;_tmp547=_tmp544->f3;_LL463:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp545->fenv)->r,vd->type)){
# 2513
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp54F;_tmp54F.tag=0;_tmp54F.f1=vd;_tmp54F;});
# 2515
struct _tuple15 _tmp548=Cyc_CfFlowInfo_unname_rval((_tmp545->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp546,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2515
struct _tuple15 _tmp549=_tmp548;void*_tmp54A;_LL465: _tmp54A=_tmp549.f1;_LL466:;{
# 2517
void*_tmp54B=_tmp54A;switch(*((int*)_tmp54B)){case 7: _LL468: _LL469:
 goto _LL46B;case 0: _LL46A: _LL46B:
 goto _LL46D;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54B)->f1 == Cyc_CfFlowInfo_NoneIL){_LL46C: _LL46D:
 goto _LL467;}else{goto _LL46E;}default: _LL46E: _LL46F:
# 2522
({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0;_tmp54E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));({void*_tmp54C[1]={& _tmp54E};Cyc_Tcutil_warn(_tmp547,({const char*_tmp54D="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp54D,sizeof(char),68);}),_tag_dyneither(_tmp54C,sizeof(void*),1));});});
# 2524
goto _LL467;}_LL467:;};}}
# 2529
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2531
{union Cyc_CfFlowInfo_FlowInfo _tmp550=inflow;struct Cyc_Dict_Dict _tmp551;if((_tmp550.ReachableFL).tag == 2){_LL471: _tmp551=((_tmp550.ReachableFL).val).f1;_LL472: {
# 2533
struct _tuple30 _tmp552=({struct _tuple30 _tmp55D;_tmp55D.f1=env;_tmp55D.f2=_tmp551;_tmp55D.f3=decl->loc;_tmp55D;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp553=env->fenv;
{void*_tmp554=decl->r;void*_tmp555=_tmp554;struct Cyc_List_List*_tmp556;struct Cyc_List_List*_tmp557;struct Cyc_Absyn_Vardecl*_tmp558;switch(*((int*)_tmp555)){case 0: _LL476: _tmp558=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp555)->f1;_LL477:
# 2537
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp552,_tmp558);
goto _LL475;case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp555)->f2 != 0){_LL478: _tmp557=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp555)->f2)->v;_LL479: {
# 2540
struct _tuple1 _tmp559=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp557);struct _tuple1 _tmp55A=_tmp559;struct Cyc_List_List*_tmp55B;_LL47F: _tmp55B=_tmp55A.f1;_LL480:;{
struct Cyc_List_List*_tmp55C=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp55B);
_tmp556=_tmp55C;goto _LL47B;};}}else{goto _LL47C;}case 3: _LL47A: _tmp556=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp555)->f1;_LL47B:
# 2544
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp552,_tmp556);
goto _LL475;default: _LL47C: _LL47D:
 goto _LL475;}_LL475:;}
# 2548
goto _LL470;}}else{_LL473: _LL474:
 goto _LL470;}_LL470:;}
# 2551
return;}
# 2557
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2560
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple29*_tmp55E=(struct _tuple29*)x->hd;struct _tuple29*_tmp55F=_tmp55E;int _tmp560;struct Cyc_Absyn_Stmt*_tmp561;struct Cyc_List_List*_tmp562;struct Cyc_List_List*_tmp563;_LL482: _tmp560=_tmp55F->f1;_tmp561=_tmp55F->f2;_tmp562=_tmp55F->f3;_tmp563=_tmp55F->f4;_LL483:;
keep_going=0;{
# 2578 "new_control_flow.cyc"
struct Cyc_Absyn_Stmt*_tmp564=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
# 2581
while(_tmp564 != _tmp561){
struct Cyc_Absyn_Stmt*_tmp565=(Cyc_NewControlFlow_get_stmt_annot(_tmp564))->encloser;
if(_tmp565 == _tmp564){
# 2586
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp562,_tmp563,_tmp560,inflow,dest->loc);
keep_going=1;
break;}
# 2590
_tmp564=_tmp565;}};}}
# 2597
return inflow;}
# 2603
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2606
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp566=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp567=_tmp566;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp568;union Cyc_CfFlowInfo_FlowInfo*_tmp569;_LL485: _tmp568=_tmp567.f1;_tmp569=_tmp567.f2;_LL486:;
inflow=*_tmp569;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp56A=env->fenv;
# 2613
void*_tmp56B=s->r;void*_tmp56C=_tmp56B;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Stmt*_tmp56E;struct Cyc_Absyn_Decl*_tmp56F;struct Cyc_Absyn_Stmt*_tmp570;struct Cyc_Absyn_Vardecl*_tmp571;struct Cyc_Absyn_Exp*_tmp572;unsigned int _tmp573;struct Cyc_Absyn_Stmt*_tmp574;struct Cyc_List_List*_tmp575;struct Cyc_Absyn_Exp*_tmp576;unsigned int _tmp577;struct Cyc_Absyn_Stmt*_tmp578;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_List_List*_tmp57A;void*_tmp57B;struct Cyc_Absyn_Exp*_tmp57C;struct Cyc_List_List*_tmp57D;void*_tmp57E;struct Cyc_Absyn_Stmt*_tmp57F;struct Cyc_Absyn_Stmt*_tmp580;struct Cyc_List_List*_tmp581;struct Cyc_Absyn_Switch_clause*_tmp582;struct Cyc_Absyn_Exp*_tmp583;struct Cyc_Absyn_Exp*_tmp584;struct Cyc_Absyn_Stmt*_tmp585;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Stmt*_tmp587;struct Cyc_Absyn_Stmt*_tmp588;struct Cyc_Absyn_Stmt*_tmp589;struct Cyc_Absyn_Exp*_tmp58A;struct Cyc_Absyn_Stmt*_tmp58B;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Stmt*_tmp58D;struct Cyc_Absyn_Stmt*_tmp58E;struct Cyc_Absyn_Exp*_tmp58F;struct Cyc_Absyn_Stmt*_tmp590;struct Cyc_Absyn_Stmt*_tmp591;struct Cyc_Absyn_Stmt*_tmp592;struct Cyc_Absyn_Stmt*_tmp593;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_Absyn_Stmt*_tmp596;switch(*((int*)_tmp56C)){case 0: _LL488: _LL489:
 return inflow;case 6: if(((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1 == 0){_LL48A: _LL48B:
# 2619
{union Cyc_CfFlowInfo_FlowInfo _tmp597=inflow;if((_tmp597.ReachableFL).tag == 2){_LL4B5: _LL4B6:
# 2621
{void*_tmp598=Cyc_Tcutil_compress(env->return_type);void*_tmp599=_tmp598;switch(*((int*)_tmp599)){case 0: _LL4BA: _LL4BB:
 goto _LL4B9;case 7: _LL4BC: _LL4BD:
 goto _LL4BF;case 6: _LL4BE: _LL4BF:
# 2625
({void*_tmp59A=0;Cyc_Tcutil_warn(s->loc,({const char*_tmp59B="break may cause function not to return a value";_tag_dyneither(_tmp59B,sizeof(char),47);}),_tag_dyneither(_tmp59A,sizeof(void*),0));});
goto _LL4B9;default: _LL4C0: _LL4C1:
# 2628
({void*_tmp59C=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp59D="break may cause function not to return a value";_tag_dyneither(_tmp59D,sizeof(char),47);}),_tag_dyneither(_tmp59C,sizeof(void*),0));});
goto _LL4B9;}_LL4B9:;}
# 2631
goto _LL4B4;}else{_LL4B7: _LL4B8:
 goto _LL4B4;}_LL4B4:;}
# 2634
goto _LL48D;}else{_LL49E: _tmp596=((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL49F:
# 2729
 _tmp580=_tmp596;goto _LL4A1;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1 == 0){_LL48C: _LL48D:
# 2636
 if(env->noreturn)
({void*_tmp59E=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp59F="`noreturn' function might return";_tag_dyneither(_tmp59F,sizeof(char),33);}),_tag_dyneither(_tmp59E,sizeof(void*),0));});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL48E: _tmp595=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL48F:
# 2642
 if(env->noreturn)
({void*_tmp5A0=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp5A1="`noreturn' function might return";_tag_dyneither(_tmp5A1,sizeof(char),33);}),_tag_dyneither(_tmp5A0,sizeof(void*),0));});{
struct _tuple16 _tmp5A2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp595));struct _tuple16 _tmp5A3=_tmp5A2;union Cyc_CfFlowInfo_FlowInfo _tmp5A4;void*_tmp5A5;_LL4C3: _tmp5A4=_tmp5A3.f1;_tmp5A5=_tmp5A3.f2;_LL4C4:;
_tmp5A4=Cyc_NewControlFlow_use_Rval(env,_tmp595->loc,_tmp5A4,_tmp5A5);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5A4);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp5A4,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1: _LL490: _tmp594=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL491: {
# 2651
struct _tuple17*_tmp5A6=rval_opt;void**_tmp5A7;int _tmp5A8;if(_tmp5A6 != 0){_LL4C6: _tmp5A7=(void**)& _tmp5A6->f1;_tmp5A8=_tmp5A6->f2;_LL4C7: {
# 2653
struct _tuple16 _tmp5A9=Cyc_NewControlFlow_anal_Rexp(env,_tmp5A8,inflow,_tmp594);struct _tuple16 _tmp5AA=_tmp5A9;union Cyc_CfFlowInfo_FlowInfo _tmp5AB;void*_tmp5AC;_LL4CB: _tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;_LL4CC:;
*_tmp5A7=_tmp5AC;
return _tmp5AB;}}else{_LL4C8: _LL4C9:
# 2657
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp594)).f1;}_LL4C5:;}case 2: _LL492: _tmp592=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp593=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL493:
# 2661
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp592,0),_tmp593,rval_opt);case 4: _LL494: _tmp58F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp590=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_tmp591=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3;_LL495: {
# 2664
struct _tuple19 _tmp5AD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp58F);struct _tuple19 _tmp5AE=_tmp5AD;union Cyc_CfFlowInfo_FlowInfo _tmp5AF;union Cyc_CfFlowInfo_FlowInfo _tmp5B0;_LL4CE: _tmp5AF=_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;_LL4CF:;{
# 2671
union Cyc_CfFlowInfo_FlowInfo _tmp5B1=Cyc_NewControlFlow_anal_stmt(env,_tmp5B0,_tmp591,0);
union Cyc_CfFlowInfo_FlowInfo _tmp5B2=Cyc_NewControlFlow_anal_stmt(env,_tmp5AF,_tmp590,0);
return Cyc_CfFlowInfo_join_flow(_tmp56A,env->all_changed,_tmp5B2,_tmp5B1);};}case 5: _LL496: _tmp58C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1).f1;_tmp58D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1).f2;_tmp58E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL497: {
# 2679
struct _tuple20 _tmp5B3=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp58D);struct _tuple20 _tmp5B4=_tmp5B3;union Cyc_CfFlowInfo_FlowInfo*_tmp5B5;_LL4D1: _tmp5B5=_tmp5B4.f2;_LL4D2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B6=*_tmp5B5;
struct _tuple19 _tmp5B7=Cyc_NewControlFlow_anal_test(env,_tmp5B6,_tmp58C);struct _tuple19 _tmp5B8=_tmp5B7;union Cyc_CfFlowInfo_FlowInfo _tmp5B9;union Cyc_CfFlowInfo_FlowInfo _tmp5BA;_LL4D4: _tmp5B9=_tmp5B8.f1;_tmp5BA=_tmp5B8.f2;_LL4D5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5BB=Cyc_NewControlFlow_anal_stmt(env,_tmp5B9,_tmp58E,0);
Cyc_NewControlFlow_update_flow(env,_tmp58D,_tmp5BB);
return _tmp5BA;};};}case 14: _LL498: _tmp589=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp58A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f1;_tmp58B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f2;_LL499: {
# 2689
union Cyc_CfFlowInfo_FlowInfo _tmp5BC=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp589,0);
struct _tuple20 _tmp5BD=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5BC,_tmp58B);struct _tuple20 _tmp5BE=_tmp5BD;union Cyc_CfFlowInfo_FlowInfo*_tmp5BF;_LL4D7: _tmp5BF=_tmp5BE.f2;_LL4D8:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C0=*_tmp5BF;
struct _tuple19 _tmp5C1=Cyc_NewControlFlow_anal_test(env,_tmp5C0,_tmp58A);struct _tuple19 _tmp5C2=_tmp5C1;union Cyc_CfFlowInfo_FlowInfo _tmp5C3;union Cyc_CfFlowInfo_FlowInfo _tmp5C4;_LL4DA: _tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;_LL4DB:;
Cyc_NewControlFlow_update_flow(env,_tmp589,_tmp5C3);
return _tmp5C4;};}case 9: _LL49A: _tmp583=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp584=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f1;_tmp585=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f2;_tmp586=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3).f1;_tmp587=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3).f2;_tmp588=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f4;_LL49B: {
# 2698
union Cyc_CfFlowInfo_FlowInfo _tmp5C5=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp583)).f1;
struct _tuple20 _tmp5C6=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5C5,_tmp585);struct _tuple20 _tmp5C7=_tmp5C6;union Cyc_CfFlowInfo_FlowInfo*_tmp5C8;_LL4DD: _tmp5C8=_tmp5C7.f2;_LL4DE:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C9=*_tmp5C8;
struct _tuple19 _tmp5CA=Cyc_NewControlFlow_anal_test(env,_tmp5C9,_tmp584);struct _tuple19 _tmp5CB=_tmp5CA;union Cyc_CfFlowInfo_FlowInfo _tmp5CC;union Cyc_CfFlowInfo_FlowInfo _tmp5CD;_LL4E0: _tmp5CC=_tmp5CB.f1;_tmp5CD=_tmp5CB.f2;_LL4E1:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CE=Cyc_NewControlFlow_anal_stmt(env,_tmp5CC,_tmp588,0);
struct _tuple20 _tmp5CF=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5CE,_tmp587);struct _tuple20 _tmp5D0=_tmp5CF;union Cyc_CfFlowInfo_FlowInfo*_tmp5D1;_LL4E3: _tmp5D1=_tmp5D0.f2;_LL4E4:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D2=*_tmp5D1;
union Cyc_CfFlowInfo_FlowInfo _tmp5D3=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5D2,_tmp586)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp585,_tmp5D3);
return _tmp5CD;};};};}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2 != 0){_LL49C: _tmp581=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp582=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL49D: {
# 2710
struct _RegionHandle _tmp5D4=_new_region("temp");struct _RegionHandle*temp=& _tmp5D4;_push_region(temp);
{struct _tuple23 _tmp5D5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp581,1,1);struct _tuple23 _tmp5D6=_tmp5D5;union Cyc_CfFlowInfo_FlowInfo _tmp5D7;struct Cyc_List_List*_tmp5D8;_LL4E6: _tmp5D7=_tmp5D6.f1;_tmp5D8=_tmp5D6.f2;_LL4E7:;
# 2713
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,_tmp582->body);{
# 2715
struct Cyc_List_List*_tmp5D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp582->pat_vars))->v)).f1);
_tmp5D7=Cyc_NewControlFlow_add_vars(_tmp56A,_tmp5D7,_tmp5D9,_tmp56A->unknown_all,s->loc,0);
# 2718
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp582->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp5DA=(struct _tuple24*)x->hd;struct _tuple24*_tmp5DB=_tmp5DA;struct Cyc_Absyn_Vardecl**_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DD;_LL4E9: _tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;_LL4EA:;
if(_tmp5DC != 0){
_tmp5D7=Cyc_NewControlFlow_do_initialize_var(_tmp56A,env,_tmp5D7,*_tmp5DC,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp581))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5D8))->hd,s->loc);
_tmp5D8=_tmp5D8->tl;
_tmp581=_tmp581->tl;}}}
# 2726
Cyc_NewControlFlow_update_flow(env,_tmp582->body,_tmp5D7);{
union Cyc_CfFlowInfo_FlowInfo _tmp5DE=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5DE;};};}
# 2711
;_pop_region(temp);}}else{_LL4B2: _LL4B3:
# 2888
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617[0]=({struct Cyc_Core_Impossible_exn_struct _tmp618;_tmp618.tag=Cyc_Core_Impossible;_tmp618.f1=({const char*_tmp619="anal_stmt -- bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp619,sizeof(char),56);});_tmp618;});_tmp617;}));}case 7: _LL4A0: _tmp580=((struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL4A1:
# 2730
 _tmp57F=_tmp580;goto _LL4A3;case 8: _LL4A2: _tmp57F=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL4A3:
# 2734
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5DF=_tmp568->encloser;
struct Cyc_Absyn_Stmt*_tmp5E0=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp57F)))->encloser;
while(_tmp5E0 != _tmp5DF){
struct Cyc_Absyn_Stmt*_tmp5E1=(Cyc_NewControlFlow_get_stmt_annot(_tmp5DF))->encloser;
if(_tmp5E1 == _tmp5DF){
({void*_tmp5E2=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp5E3="goto enters local scope or exception handler";_tag_dyneither(_tmp5E3,sizeof(char),45);}),_tag_dyneither(_tmp5E2,sizeof(void*),0));});
break;}
# 2743
_tmp5DF=_tmp5E1;}}
# 2747
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp57F));
# 2749
Cyc_NewControlFlow_update_flow(env,_tmp57F,inflow);
return Cyc_CfFlowInfo_BottomFL();case 10: _LL4A4: _tmp57C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp57D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_tmp57E=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3;_LL4A5:
# 2755
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp57D,_tmp57C->loc);case 15: _LL4A6: _tmp579=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp57A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_tmp57B=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3;_LL4A7: {
# 2760
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp579,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2768
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2771
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp57A,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5E4=scs_outflow;if((_tmp5E4.BottomFL).tag == 1){_LL4EC: _LL4ED:
 goto _LL4EB;}else{_LL4EE: _LL4EF:
({void*_tmp5E5=0;Cyc_CfFlowInfo_aerr(s->loc,({const char*_tmp5E6="last catch clause may implicitly fallthru";_tag_dyneither(_tmp5E6,sizeof(char),42);}),_tag_dyneither(_tmp5E5,sizeof(void*),0));});}_LL4EB:;}
# 2777
outflow=s1_outflow;
# 2779
return outflow;};};}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f2 != 0){_LL4A8: _tmp575=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f2)->v;_tmp576=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f3;_tmp577=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->loc;_tmp578=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL4A9: {
# 2789
struct _tuple1 _tmp5E7=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp575);struct _tuple1 _tmp5E8=_tmp5E7;struct Cyc_List_List*_tmp5E9;struct Cyc_List_List*_tmp5EA;_LL4F1: _tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;_LL4F2:;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp56A,env,inflow,_tmp575,_tmp5E9 != 0,_tmp577,_tmp576->loc);{
struct Cyc_List_List*_tmp5EB=env->unique_pat_vars;
env->unique_pat_vars=({struct Cyc_List_List*_tmp5EC=_region_malloc(env->r,sizeof(*_tmp5EC));_tmp5EC->hd=({struct _tuple29*_tmp5ED=_region_malloc(env->r,sizeof(*_tmp5ED));_tmp5ED->f1=0;_tmp5ED->f2=s;_tmp5ED->f3=_tmp5E9;_tmp5ED->f4=_tmp5EA;_tmp5ED;});_tmp5EC->tl=_tmp5EB;_tmp5EC;});
# 2797
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp578,rval_opt);
env->unique_pat_vars=_tmp5EB;
# 2801
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5E9,_tmp5EA,0,inflow,_tmp577);
# 2805
return inflow;};}}else{goto _LL4AC;}case 4: _LL4AA: _tmp571=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f2;_tmp572=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f4;_tmp573=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->loc;_tmp574=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;if(_tmp572 != 0){_LL4AB: {
# 2817
struct Cyc_List_List l=({struct Cyc_List_List _tmp60B;_tmp60B.hd=_tmp572;_tmp60B.tl=0;_tmp60B;});
union Cyc_CfFlowInfo_FlowInfo _tmp5EE=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5EF=Cyc_NewControlFlow_anal_Lexp(env,_tmp5EE,0,0,_tmp572);struct _tuple18 _tmp5F0=_tmp5EF;union Cyc_CfFlowInfo_AbsLVal _tmp5F1;_LL4F4: _tmp5F1=_tmp5F0.f2;_LL4F5:;{
struct _tuple16 _tmp5F2=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5EE,_tmp572);struct _tuple16 _tmp5F3=_tmp5F2;union Cyc_CfFlowInfo_FlowInfo _tmp5F4;_LL4F7: _tmp5F4=_tmp5F3.f1;_LL4F8:;{
struct Cyc_List_List*_tmp5F5=0;
struct Cyc_List_List*_tmp5F6=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5F7=_tmp5F4;struct Cyc_Dict_Dict _tmp5F8;struct Cyc_List_List*_tmp5F9;if((_tmp5F7.ReachableFL).tag == 2){_LL4FA: _tmp5F8=((_tmp5F7.ReachableFL).val).f1;_tmp5F9=((_tmp5F7.ReachableFL).val).f2;_LL4FB:
# 2825
{union Cyc_CfFlowInfo_AbsLVal _tmp5FA=_tmp5F1;struct Cyc_CfFlowInfo_Place*_tmp5FB;if((_tmp5FA.PlaceL).tag == 1){_LL4FF: _tmp5FB=(_tmp5FA.PlaceL).val;_LL500: {
# 2829
void*_tmp5FC=Cyc_CfFlowInfo_lookup_place(_tmp5F8,_tmp5FB);
_tmp5FC=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5FD=_region_malloc(_tmp56A->r,sizeof(*_tmp5FD));_tmp5FD[0]=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5FE;_tmp5FE.tag=8;_tmp5FE.f1=_tmp571;_tmp5FE.f2=_tmp5FC;_tmp5FE;});_tmp5FD;});
_tmp5F8=Cyc_CfFlowInfo_assign_place(_tmp56A,_tmp572->loc,_tmp5F8,env->all_changed,_tmp5FB,_tmp5FC);
# 2833
_tmp5F4=Cyc_CfFlowInfo_ReachableFL(_tmp5F8,_tmp5F9);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5FF=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp604=_region_malloc(_tmp56A->r,sizeof(*_tmp604));_tmp604[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp605;_tmp605.tag=0;_tmp605.f1=_tmp571;_tmp605;});_tmp604;});
struct Cyc_CfFlowInfo_Place*_tmp600=({struct Cyc_CfFlowInfo_Place*_tmp603=_region_malloc(_tmp56A->r,sizeof(*_tmp603));_tmp603->root=(void*)_tmp5FF;_tmp603->fields=0;_tmp603;});
_tmp5F5=({struct Cyc_List_List*_tmp601=_region_malloc(_tmp56A->r,sizeof(*_tmp601));_tmp601->hd=_tmp600;_tmp601->tl=_tmp5F5;_tmp601;});
_tmp5F6=({struct Cyc_List_List*_tmp602=_region_malloc(_tmp56A->r,sizeof(*_tmp602));_tmp602->hd=_tmp572;_tmp602->tl=_tmp5F6;_tmp602;});
goto _LL4FE;};}}else{_LL501: _LL502:
# 2845
 goto _LL4FE;}_LL4FE:;}
# 2847
goto _LL4F9;}else{_LL4FC: _LL4FD:
# 2849
 goto _LL4F9;}_LL4F9:;}{
# 2852
struct Cyc_List_List _tmp606=({struct Cyc_List_List _tmp60A;_tmp60A.hd=_tmp571;_tmp60A.tl=0;_tmp60A;});
_tmp5F4=Cyc_NewControlFlow_add_vars(_tmp56A,_tmp5F4,& _tmp606,_tmp56A->unknown_all,_tmp573,0);{
# 2855
struct Cyc_List_List*_tmp607=env->unique_pat_vars;
env->unique_pat_vars=({struct Cyc_List_List*_tmp608=_region_malloc(env->r,sizeof(*_tmp608));_tmp608->hd=({struct _tuple29*_tmp609=_region_malloc(env->r,sizeof(*_tmp609));_tmp609->f1=1;_tmp609->f2=s;_tmp609->f3=_tmp5F5;_tmp609->f4=_tmp5F6;_tmp609;});_tmp608->tl=_tmp607;_tmp608;});
# 2861
_tmp5F4=Cyc_NewControlFlow_anal_stmt(env,_tmp5F4,_tmp574,rval_opt);
env->unique_pat_vars=_tmp607;
# 2865
_tmp5F4=Cyc_NewControlFlow_unconsume_params(env,_tmp5F5,_tmp5F6,1,_tmp5F4,_tmp573);
# 2869
return _tmp5F4;};};};};}}else{goto _LL4AC;}default: _LL4AC: _tmp56F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp570=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL4AD:
# 2872
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp56F),_tmp570,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp56F);
return outflow;}case 13: _LL4AE: _tmp56E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL4AF:
# 2877
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp56E,rval_opt);default: _LL4B0: _tmp56D=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL4B1: {
# 2880
struct _tuple16 _tmp60C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp56D);struct _tuple16 _tmp60D=_tmp60C;union Cyc_CfFlowInfo_FlowInfo _tmp60E;void*_tmp60F;_LL504: _tmp60E=_tmp60D.f1;_tmp60F=_tmp60D.f2;_LL505:;{
union Cyc_CfFlowInfo_FlowInfo _tmp610=Cyc_NewControlFlow_use_Rval(env,_tmp56D->loc,_tmp60E,_tmp60F);
void*_tmp611=Cyc_Tcutil_compress((void*)_check_null(_tmp56D->topt));void*_tmp612=_tmp611;void*_tmp613;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp612)->tag == 15){_LL507: _tmp613=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp612)->f1;_LL508:
# 2884
 return Cyc_CfFlowInfo_kill_flow_region(_tmp56A,_tmp60E,_tmp613);}else{_LL509: _LL50A:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614[0]=({struct Cyc_Core_Impossible_exn_struct _tmp615;_tmp615.tag=Cyc_Core_Impossible;_tmp615.f1=({const char*_tmp616="anal_stmt -- reset_region";_tag_dyneither(_tmp616,sizeof(char),26);});_tmp615;});_tmp614;}));}_LL506:;};}}_LL487:;};}
# 2893
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2896
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp61A=env->fenv;
void*_tmp61B=decl->r;void*_tmp61C=_tmp61B;struct Cyc_Absyn_Tvar*_tmp61D;struct Cyc_Absyn_Vardecl*_tmp61E;int _tmp61F;struct Cyc_Absyn_Exp*_tmp620;struct Cyc_Absyn_Fndecl*_tmp621;struct Cyc_List_List*_tmp622;struct Cyc_Absyn_Vardecl*_tmp623;switch(*((int*)_tmp61C)){case 0: _LL50C: _tmp623=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_LL50D: {
# 2906
struct Cyc_List_List _tmp624=({struct Cyc_List_List _tmp62A;_tmp62A.hd=_tmp623;_tmp62A.tl=0;_tmp62A;});
inflow=Cyc_NewControlFlow_add_vars(_tmp61A,inflow,& _tmp624,_tmp61A->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp625=_tmp623->initializer;
if(_tmp625 == 0)
return inflow;{
struct _tuple16 _tmp626=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp625);struct _tuple16 _tmp627=_tmp626;union Cyc_CfFlowInfo_FlowInfo _tmp628;void*_tmp629;_LL517: _tmp628=_tmp627.f1;_tmp629=_tmp627.f2;_LL518:;
return Cyc_NewControlFlow_do_initialize_var(_tmp61A,env,_tmp628,_tmp623,_tmp625,_tmp629,decl->loc);};};}case 3: _LL50E: _tmp622=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_LL50F:
# 2915
 return Cyc_NewControlFlow_add_vars(_tmp61A,inflow,_tmp622,_tmp61A->unknown_none,decl->loc,0);case 1: _LL510: _tmp621=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_LL511:
# 2918
 Cyc_NewControlFlow_check_nested_fun(_tmp61A,inflow,_tmp621);{
void*t=(void*)_check_null(_tmp621->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp62B=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp621->fn_vardecl);
# 2924
return Cyc_NewControlFlow_add_vars(_tmp61A,inflow,({struct Cyc_List_List*_tmp62C=_region_malloc(env->r,sizeof(*_tmp62C));_tmp62C->hd=_tmp62B;_tmp62C->tl=0;_tmp62C;}),_tmp61A->unknown_all,decl->loc,0);};case 4: _LL512: _tmp61D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_tmp61E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f2;_tmp61F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f3;_tmp620=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f4;_LL513:
# 2927
 if(_tmp620 != 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D[0]=({struct Cyc_Core_Impossible_exn_struct _tmp62E;_tmp62E.tag=Cyc_Core_Impossible;_tmp62E.f1=({const char*_tmp62F="found open expression in declaration!";_tag_dyneither(_tmp62F,sizeof(char),38);});_tmp62E;});_tmp62D;}));{
struct Cyc_List_List _tmp630=({struct Cyc_List_List _tmp631;_tmp631.hd=_tmp61E;_tmp631.tl=0;_tmp631;});
return Cyc_NewControlFlow_add_vars(_tmp61A,inflow,& _tmp630,_tmp61A->unknown_all,decl->loc,0);};default: _LL514: _LL515:
# 2933
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632[0]=({struct Cyc_Core_Impossible_exn_struct _tmp633;_tmp633.tag=Cyc_Core_Impossible;_tmp633.f1=({const char*_tmp634="anal_decl: unexpected decl variant";_tag_dyneither(_tmp634,sizeof(char),35);});_tmp633;});_tmp632;}));}_LL50B:;}
# 2941
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp635;_push_handler(& _tmp635);{int _tmp637=0;if(setjmp(_tmp635.handler))_tmp637=1;if(!_tmp637){
{struct _RegionHandle _tmp638=_new_region("frgn");struct _RegionHandle*frgn=& _tmp638;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2944
;_pop_region(frgn);}
# 2943
;_pop_handler();}else{void*_tmp636=(void*)_exn_thrown;void*_tmp639=_tmp636;void*_tmp63A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp639)->tag == Cyc_Dict_Absent){_LL51A: _LL51B:
# 2951
 if(Cyc_Position_num_errors > 0)
goto _LL519;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp63B=_cycalloc_atomic(sizeof(*_tmp63B));_tmp63B[0]=({struct Cyc_Dict_Absent_exn_struct _tmp63C;_tmp63C.tag=Cyc_Dict_Absent;_tmp63C;});_tmp63B;}));}}else{_LL51C: _tmp63A=_tmp639;_LL51D:(int)_rethrow(_tmp63A);}_LL519:;}};}
# 2957
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2962
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp63D=r;unsigned int _tmp63E;if((_tmp63D.RParam).tag == 5){_LL51F: _tmp63E=(_tmp63D.RParam).val;_LL520: {
# 2965
struct Cyc_Absyn_Vardecl*_tmp63F=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp63E);
if(!_tmp63F->escapes)
return Cyc_Relations_RVar(_tmp63F);
return r;}}else{_LL521: _LL522:
 return r;}_LL51E:;}
# 2973
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2977
struct _RegionHandle*_tmp640=fenv->r;
unsigned int _tmp641=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp641,1);{
# 2983
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp642=({union Cyc_CfFlowInfo_FlowInfo _tmp67A=inflow;if((_tmp67A.ReachableFL).tag != 2)_throw_match();(_tmp67A.ReachableFL).val;});struct _tuple14 _tmp643=_tmp642;struct Cyc_Dict_Dict _tmp644;struct Cyc_List_List*_tmp645;_LL524: _tmp644=_tmp643.f1;_tmp645=_tmp643.f2;_LL525:;{
# 2988
struct Cyc_List_List*_tmp646=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp647=fd->requires_relns;for(0;_tmp647 != 0;_tmp647=_tmp647->tl){
struct Cyc_Relations_Reln*_tmp648=(struct Cyc_Relations_Reln*)_tmp647->hd;
_tmp645=Cyc_Relations_add_relation(_tmp640,Cyc_NewControlFlow_translate_rop(_tmp646,_tmp648->rop1),_tmp648->relation,
Cyc_NewControlFlow_translate_rop(_tmp646,_tmp648->rop2),_tmp645);}}{
# 2997
struct Cyc_List_List*atts;
{void*_tmp649=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp64A=_tmp649;struct Cyc_List_List*_tmp64B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp64A)->tag == 9){_LL527: _tmp64B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp64A)->f1).attributes;_LL528:
 atts=_tmp64B;goto _LL526;}else{_LL529: _LL52A:
({void*_tmp64C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp64D="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp64D,sizeof(char),50);}),_tag_dyneither(_tmp64C,sizeof(void*),0));});}_LL526:;}
# 3004
for(0;atts != 0;atts=atts->tl){
void*_tmp64E=(void*)atts->hd;void*_tmp64F=_tmp64E;int _tmp650;int _tmp651;int _tmp652;switch(*((int*)_tmp64F)){case 21: _LL52C: _tmp652=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp64F)->f1;_LL52D: {
# 3007
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp653=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp652 - 1);
void*t=Cyc_Tcutil_compress(_tmp653->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp654=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp655=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65C=_region_malloc(_tmp640,sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp65D;_tmp65D.tag=2;_tmp65D.f1=_tmp652;_tmp65D.f2=t;_tmp65D;});_tmp65C;});
struct Cyc_CfFlowInfo_Place*_tmp656=({struct Cyc_CfFlowInfo_Place*_tmp65B=_region_malloc(_tmp640,sizeof(*_tmp65B));_tmp65B->root=(void*)_tmp655;_tmp65B->fields=0;_tmp65B;});
_tmp644=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp644,(void*)_tmp655,_tmp654);
_tmp644=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp644,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp657=_region_malloc(_tmp640,sizeof(*_tmp657));_tmp657[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp658;_tmp658.tag=0;_tmp658.f1=_tmp653;_tmp658;});_tmp657;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp659=_region_malloc(_tmp640,sizeof(*_tmp659));_tmp659[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp65A;_tmp65A.tag=5;_tmp65A.f1=_tmp656;_tmp65A;});_tmp659;}));
goto _LL52B;}case 20: _LL52E: _tmp651=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp64F)->f1;_LL52F: {
# 3020
struct Cyc_Absyn_Vardecl*_tmp65E=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp651 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp65E->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65F=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp667=_region_malloc(_tmp640,sizeof(*_tmp667));_tmp667[0]=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp668;_tmp668.tag=2;_tmp668.f1=_tmp651;_tmp668.f2=elttype;_tmp668;});_tmp667;});
struct Cyc_CfFlowInfo_Place*_tmp660=({struct Cyc_CfFlowInfo_Place*_tmp666=_region_malloc(_tmp640,sizeof(*_tmp666));_tmp666->root=(void*)_tmp65F;_tmp666->fields=0;_tmp666;});
_tmp644=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp644,(void*)_tmp65F,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
_tmp644=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp644,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp661=_region_malloc(_tmp640,sizeof(*_tmp661));_tmp661[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp662;_tmp662.tag=0;_tmp662.f1=_tmp65E;_tmp662;});_tmp661;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp663=_region_malloc(_tmp640,sizeof(*_tmp663));_tmp663[0]=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp664;_tmp664.tag=5;_tmp664.f1=_tmp660;_tmp664;});_tmp663;}));
param_roots=({struct Cyc_List_List*_tmp665=_region_malloc(_tmp640,sizeof(*_tmp665));_tmp665->hd=_tmp660;_tmp665->tl=param_roots;_tmp665;});
goto _LL52B;}case 22: _LL530: _tmp650=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp64F)->f1;_LL531: {
# 3029
struct Cyc_Absyn_Vardecl*_tmp669=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp650 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp669->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp66A=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp66E=_region_malloc(_tmp640,sizeof(*_tmp66E));_tmp66E[0]=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp66F;_tmp66F.tag=0;_tmp66F.f1=_tmp669;_tmp66F;});_tmp66E;});
struct Cyc_CfFlowInfo_Place*_tmp66B=({struct Cyc_CfFlowInfo_Place*_tmp66D=_region_malloc(_tmp640,sizeof(*_tmp66D));_tmp66D->root=(void*)_tmp66A;_tmp66D->fields=0;_tmp66D;});
noconsume_roots=({struct Cyc_List_List*_tmp66C=_region_malloc(_tmp640,sizeof(*_tmp66C));_tmp66C->hd=_tmp66B;_tmp66C->tl=noconsume_roots;_tmp66C;});}
# 3035
goto _LL52B;}default: _LL532: _LL533:
 goto _LL52B;}_LL52B:;}
# 3038
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp644,_tmp645);{
# 3040
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp640,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp679=_region_malloc(_tmp640,sizeof(*_tmp679));_tmp679->r=_tmp640;_tmp679->fenv=fenv;_tmp679->iterate_again=1;_tmp679->iteration_num=0;_tmp679->in_try=0;_tmp679->tryflow=inflow;_tmp679->all_changed=0;_tmp679->noreturn=noreturn;_tmp679->return_type=fd->ret_type;_tmp679->unique_pat_vars=0;_tmp679->param_roots=param_roots;_tmp679->noconsume_params=noconsume_roots;_tmp679->flow_table=flow_table;_tmp679->return_relns=fd->ensures_relns;_tmp679;});
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3050
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3053
union Cyc_CfFlowInfo_FlowInfo _tmp670=outflow;if((_tmp670.BottomFL).tag == 1){_LL535: _LL536:
 goto _LL534;}else{_LL537: _LL538:
# 3056
 Cyc_NewControlFlow_check_init_params(_tmp641,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp641);
# 3060
if(noreturn)
({void*_tmp671=0;Cyc_CfFlowInfo_aerr(_tmp641,({const char*_tmp672="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp672,sizeof(char),46);}),_tag_dyneither(_tmp671,sizeof(void*),0));});else{
# 3063
void*_tmp673=Cyc_Tcutil_compress(fd->ret_type);void*_tmp674=_tmp673;switch(*((int*)_tmp674)){case 0: _LL53A: _LL53B:
 goto _LL539;case 7: _LL53C: _LL53D:
 goto _LL53F;case 6: _LL53E: _LL53F:
# 3067
({void*_tmp675=0;Cyc_Tcutil_warn(_tmp641,({const char*_tmp676="function may not return a value";_tag_dyneither(_tmp676,sizeof(char),32);}),_tag_dyneither(_tmp675,sizeof(void*),0));});goto _LL539;default: _LL540: _LL541:
# 3069
({void*_tmp677=0;Cyc_CfFlowInfo_aerr(_tmp641,({const char*_tmp678="function may not return a value";_tag_dyneither(_tmp678,sizeof(char),32);}),_tag_dyneither(_tmp677,sizeof(void*),0));});goto _LL539;}_LL539:;}
# 3071
goto _LL534;}_LL534:;};};};};};}
# 3075
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp67B=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp67C=_tmp67B;struct Cyc_List_List*_tmp67D;struct Cyc_List_List*_tmp67E;struct Cyc_List_List*_tmp67F;struct Cyc_Absyn_Fndecl*_tmp680;switch(*((int*)_tmp67C)){case 1: _LL543: _tmp680=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp67C)->f1;_LL544:
 Cyc_NewControlFlow_check_fun(_tmp680);goto _LL542;case 11: _LL545: _tmp67F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp67C)->f1;_LL546:
 _tmp67E=_tmp67F;goto _LL548;case 10: _LL547: _tmp67E=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp67C)->f2;_LL548:
 _tmp67D=_tmp67E;goto _LL54A;case 9: _LL549: _tmp67D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp67C)->f2;_LL54A:
 Cyc_NewControlFlow_cf_check(_tmp67D);goto _LL542;case 12: _LL54B: _LL54C:
 goto _LL542;default: _LL54D: _LL54E:
 goto _LL542;}_LL542:;};}}
