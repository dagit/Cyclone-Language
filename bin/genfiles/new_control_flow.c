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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp67F;struct Cyc_NewControlFlow_CFStmtAnnot _tmp67E;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp67D;enclosee->annot=(void*)((_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D[0]=((_tmp67F.tag=Cyc_NewControlFlow_CFAnnot,((_tmp67F.f1=((_tmp67E.encloser=encloser,((_tmp67E.visited=0,_tmp67E)))),_tmp67F)))),_tmp67D))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};
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
 return _tmp5;}else{_LL3: _LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp685;const char*_tmp684;struct Cyc_Core_Impossible_exn_struct*_tmp683;(int)_throw((void*)((_tmp683=_cycalloc(sizeof(*_tmp683)),((_tmp683[0]=((_tmp685.tag=Cyc_Core_Impossible,((_tmp685.f1=((_tmp684="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp684,sizeof(char),37))),_tmp685)))),_tmp683)))));}}_LL0:;}
# 144
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp686;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp686=_region_malloc(env->r,sizeof(*_tmp686)),((_tmp686[0]=Cyc_CfFlowInfo_BottomFL(),_tmp686)));
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
_tmpA->visited=env->iteration_num;{
struct _tuple20 _tmp687;return(_tmp687.f1=_tmpA,((_tmp687.f2=_tmpB,_tmp687)));};}
# 178
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 186
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 193
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 195
struct Cyc_NewControlFlow_AnalEnv*_tmpE;unsigned int _tmpF;struct Cyc_Dict_Dict _tmp10;struct _tuple21*_tmpD=ckenv;_tmpE=_tmpD->f1;_tmpF=_tmpD->f2;_tmp10=_tmpD->f3;{
void*_tmp11=root;struct Cyc_Absyn_Vardecl*_tmp12;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11)->tag == 0){_LL6: _tmp12=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp11)->f1;_LL7:
# 198
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp10,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpE->fenv)->r,_tmp12->type)){
retry: {void*_tmp13=rval;void*_tmp14;switch(*((int*)_tmp13)){case 8: _LLB: _tmp14=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13)->f2;_LLC:
 rval=_tmp14;goto retry;case 7: _LLD: _LLE:
 goto _LL10;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp13)->f1 == Cyc_CfFlowInfo_NoneIL){_LLF: _LL10:
 goto _LL12;}else{goto _LL13;}case 0: _LL11: _LL12:
 goto _LLA;default: _LL13: _LL14:
# 207
{const char*_tmp68B;void*_tmp68A[1];struct Cyc_String_pa_PrintArg_struct _tmp689;(_tmp689.tag=0,((_tmp689.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp12->name)),((_tmp68A[0]=& _tmp689,Cyc_Tcutil_warn(_tmpF,((_tmp68B="alias-free pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp68B,sizeof(char),64))),_tag_dyneither(_tmp68A,sizeof(void*),1)))))));}
goto _LLA;}_LLA:;}}
# 211
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;};}
# 219
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp18=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp19=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp1A=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp19);
# 225
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp68C;struct _tuple19 _tmp1B=(_tmp68C.f1=flow,((_tmp68C.f2=_tmp1A,_tmp68C)));struct _tuple19 _tmp1C=_tmp1B;struct Cyc_Dict_Dict _tmp1D;struct Cyc_Dict_Dict _tmp1E;if(((_tmp1C.f1).ReachableFL).tag == 2){if(((_tmp1C.f2).ReachableFL).tag == 2){_LL16: _tmp1D=(((_tmp1C.f1).ReachableFL).val).f1;_tmp1E=(((_tmp1C.f2).ReachableFL).val).f1;_LL17: {
# 228
struct _tuple21 _tmp68D;struct _tuple21 _tmp1F=(_tmp68D.f1=env,((_tmp68D.f2=s->loc,((_tmp68D.f3=_tmp1E,_tmp68D)))));
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1F,_tmp1D);
goto _LL15;}}else{goto _LL18;}}else{_LL18: _LL19:
 goto _LL15;}_LL15:;}
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
union Cyc_CfFlowInfo_FlowInfo _tmp22=inflow;struct Cyc_Dict_Dict _tmp23;struct Cyc_List_List*_tmp24;if((_tmp22.BottomFL).tag == 1){_LL1B: _LL1C:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL1D: _tmp23=((_tmp22.ReachableFL).val).f1;_tmp24=((_tmp22.ReachableFL).val).f2;_LL1E:
# 259
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp690;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp68F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp25=(_tmp68F=_region_malloc(fenv->r,sizeof(*_tmp68F)),((_tmp68F[0]=((_tmp690.tag=0,((_tmp690.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp690)))),_tmp68F)));
void*_tmp26=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp693;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp692;_tmp26=(void*)((_tmp692=_region_malloc(fenv->r,sizeof(*_tmp692)),((_tmp692[0]=((_tmp693.tag=8,((_tmp693.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp693.f2=_tmp26,_tmp693)))))),_tmp692))));}
# 266
_tmp23=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp23,(void*)_tmp25,_tmp26);}
# 268
return Cyc_CfFlowInfo_ReachableFL(_tmp23,_tmp24);}_LL1A:;}
# 272
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 279
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2B=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp694;if(Cyc_Relations_consistent_relations(((_tmp694=_region_malloc(r,sizeof(*_tmp694)),((_tmp694->hd=_tmp2B,((_tmp694->tl=assume,_tmp694))))))))
return 0;}
# 284
return 1;}
# 287
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2D=e->r;void*_tmp2E=_tmp2D;struct Cyc_Absyn_Exp*_tmp2F;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E)->tag == 13){_LL20: _tmp2F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E)->f2;_LL21:
 return _tmp2F;}else{_LL22: _LL23:
 return e;}_LL1F:;}
# 294
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 296
union Cyc_CfFlowInfo_FlowInfo _tmp30=inflow;struct Cyc_List_List*_tmp31;if((_tmp30.BottomFL).tag == 1){_LL25: _LL26:
 return;}else{_LL27: _tmp31=((_tmp30.ReachableFL).val).f2;_LL28:
# 299
{void*_tmp32=Cyc_Tcutil_compress(t);void*_tmp33=_tmp32;struct Cyc_List_List*_tmp34;union Cyc_Absyn_AggrInfoU _tmp35;struct Cyc_List_List*_tmp36;switch(*((int*)_tmp33)){case 11: _LL2A: _tmp35=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33)->f1).aggr_info;_tmp36=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33)->f1).targs;_LL2B: {
# 301
struct Cyc_Absyn_Aggrdecl*_tmp37=Cyc_Absyn_get_known_aggrdecl(_tmp35);
struct Cyc_Absyn_Aggrfield*_tmp38=Cyc_Absyn_lookup_decl_field(_tmp37,f);
struct Cyc_Absyn_Exp*_tmp39=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp38))->requires_clause;
if(_tmp39 != 0){
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp3B=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp37->tvs,_tmp36),_tmp39);
# 308
if(!Cyc_NewControlFlow_relns_ok(r,_tmp31,Cyc_Relations_exp2relns(temp,_tmp3B))){
{const char*_tmp699;void*_tmp698[2];struct Cyc_String_pa_PrintArg_struct _tmp697;struct Cyc_String_pa_PrintArg_struct _tmp696;(_tmp696.tag=0,((_tmp696.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp697.tag=0,((_tmp697.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3B))),((_tmp698[0]=& _tmp697,((_tmp698[1]=& _tmp696,Cyc_CfFlowInfo_aerr(loc,((_tmp699="unable to prove %s, required to access %s",_tag_dyneither(_tmp699,sizeof(char),42))),_tag_dyneither(_tmp698,sizeof(void*),2)))))))))))));}
{const char*_tmp69C;void*_tmp69B;(_tmp69B=0,Cyc_fprintf(Cyc_stderr,((_tmp69C="  [recorded facts on path: ",_tag_dyneither(_tmp69C,sizeof(char),28))),_tag_dyneither(_tmp69B,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp31);{
const char*_tmp69F;void*_tmp69E;(_tmp69E=0,Cyc_fprintf(Cyc_stderr,((_tmp69F="]\n",_tag_dyneither(_tmp69F,sizeof(char),3))),_tag_dyneither(_tmp69E,sizeof(void*),0)));};}}
# 306
;_pop_region(temp);}
# 315
goto _LL29;}case 12: _LL2C: _tmp34=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33)->f2;_LL2D: {
# 317
struct Cyc_Absyn_Aggrfield*_tmp44=Cyc_Absyn_lookup_field(_tmp34,f);
struct Cyc_Absyn_Exp*_tmp45=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp44))->requires_clause;
if(_tmp45 != 0){
struct _RegionHandle _tmp46=_new_region("temp");struct _RegionHandle*temp=& _tmp46;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp31,Cyc_Relations_exp2relns(temp,_tmp45))){
{const char*_tmp6A4;void*_tmp6A3[2];struct Cyc_String_pa_PrintArg_struct _tmp6A2;struct Cyc_String_pa_PrintArg_struct _tmp6A1;(_tmp6A1.tag=0,((_tmp6A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A2.tag=0,((_tmp6A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp45))),((_tmp6A3[0]=& _tmp6A2,((_tmp6A3[1]=& _tmp6A1,Cyc_CfFlowInfo_aerr(loc,((_tmp6A4="unable to prove %s, required to access %s",_tag_dyneither(_tmp6A4,sizeof(char),42))),_tag_dyneither(_tmp6A3,sizeof(void*),2)))))))))))));}
{const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7="  [recorded facts on path: ",_tag_dyneither(_tmp6A7,sizeof(char),28))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp31);{
const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="]\n",_tag_dyneither(_tmp6AA,sizeof(char),3))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));};}
# 321
;_pop_region(temp);}
# 328
goto _LL29;}default: _LL2E: _LL2F:
 goto _LL29;}_LL29:;}
# 331
goto _LL24;}_LL24:;}
# 335
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4F=inflow;struct Cyc_Dict_Dict _tmp50;struct Cyc_List_List*_tmp51;if((_tmp4F.BottomFL).tag == 1){_LL31: _LL32:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL33: _tmp50=((_tmp4F.ReachableFL).val).f1;_tmp51=((_tmp4F.ReachableFL).val).f2;_LL34:
# 339
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp50,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp6AD;void*_tmp6AC;(_tmp6AC=0,Cyc_CfFlowInfo_aerr(loc,((_tmp6AD="expression may not be fully initialized",_tag_dyneither(_tmp6AD,sizeof(char),40))),_tag_dyneither(_tmp6AC,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp54=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp50,env->all_changed,r);
if(_tmp50.t == _tmp54.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp55=Cyc_CfFlowInfo_ReachableFL(_tmp54,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp55);
return _tmp55;};};}_LL30:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 349
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp6AE;struct _tuple0 _tmp56=(_tmp6AE.f1=Cyc_Tcutil_compress(t),((_tmp6AE.f2=r,_tmp6AE)));struct _tuple0 _tmp57=_tmp56;enum Cyc_Absyn_AggrKind _tmp58;struct Cyc_List_List*_tmp59;struct _dyneither_ptr _tmp5A;union Cyc_Absyn_AggrInfoU _tmp5B;struct Cyc_List_List*_tmp5C;struct _dyneither_ptr _tmp5D;struct Cyc_List_List*_tmp5E;struct _dyneither_ptr _tmp5F;struct Cyc_Absyn_Datatypefield*_tmp60;struct _dyneither_ptr _tmp61;void*_tmp62;switch(*((int*)_tmp57.f2)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL36: _LL37:
 return;}else{switch(*((int*)_tmp57.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL48;else{goto _LL48;}case 10: goto _LL48;case 11: goto _LL48;case 12: goto _LL48;case 5: goto _LL46;default: goto _LL48;}}case 0: _LL38: _LL39:
 return;case 7: _LL3A: _LL3B:
 return;case 8: _LL3C: _tmp62=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp62);return;default: switch(*((int*)_tmp57.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL3E: _tmp60=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp61=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL3F:
# 356
 if(_tmp60->typs == 0)
return;
_tmp5E=_tmp60->typs;_tmp5F=_tmp61;goto _LL41;}else{goto _LL48;}}else{goto _LL48;}case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL40: _tmp5E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57.f1)->f1;_tmp5F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL41: {
# 360
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5E);
{int i=0;for(0;i < sz;(i ++,_tmp5E=_tmp5E->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5E))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp5F,sizeof(void*),i)));}}
# 364
return;}}else{goto _LL48;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL42: _tmp5B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp57.f1)->f1).aggr_info;_tmp5C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp57.f1)->f1).targs;_tmp5D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL43: {
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
;_pop_region(temp);};}}else{goto _LL48;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->tag == 6){_LL44: _tmp58=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57.f1)->f1;_tmp59=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57.f1)->f2;_tmp5A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp57.f2)->f2;_LL45:
# 380
{int i=0;for(0;i < _get_dyneither_size(_tmp5A,sizeof(void*));(i ++,_tmp59=_tmp59->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp59))->hd)->type,((void**)_tmp5A.curr)[i]);}}
return;}else{goto _LL48;}case 5: _LL46: if(
Cyc_Tcutil_is_noalias_pointer(t,0)){_LL47:
{const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,Cyc_Tcutil_warn(loc,((_tmp6B1="argument may still contain alias-free pointers",_tag_dyneither(_tmp6B1,sizeof(char),47))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));}
return;}else{goto _LL48;}default: _LL48: _LL49:
 return;}}_LL35:;}
# 390
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp6A=inflow;struct Cyc_Dict_Dict _tmp6B;struct Cyc_List_List*_tmp6C;if((_tmp6A.BottomFL).tag == 1){_LL4B: _LL4C:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL4D: _tmp6B=((_tmp6A.ReachableFL).val).f1;_tmp6C=((_tmp6A.ReachableFL).val).f2;_LL4E:
# 394
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp6B4;void*_tmp6B3;(_tmp6B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6B4="noliveunique attribute requires alias-free pointer",_tag_dyneither(_tmp6B4,sizeof(char),51))),_tag_dyneither(_tmp6B3,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 398
void*_tmp6F=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp70=r;struct Cyc_CfFlowInfo_Place*_tmp71;void*_tmp72;switch(*((int*)_tmp70)){case 8: _LL50: _tmp72=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp70)->f2;_LL51:
 r=_tmp72;goto retry;case 5: _LL52: _tmp71=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp70)->f1;_LL53:
# 402
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6F,Cyc_CfFlowInfo_lookup_place(_tmp6B,_tmp71));
goto _LL4F;default: _LL54: _LL55:
# 405
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp6F)){
const char*_tmp6B7;void*_tmp6B6;(_tmp6B6=0,Cyc_Tcutil_warn(loc,((_tmp6B7="argument may contain live alias-free pointers",_tag_dyneither(_tmp6B7,sizeof(char),46))),_tag_dyneither(_tmp6B6,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL4F:;}{
# 409
struct Cyc_Dict_Dict _tmp75=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp6B,env->all_changed,r);
if(_tmp6B.t == _tmp75.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp76=Cyc_CfFlowInfo_ReachableFL(_tmp75,_tmp6C);
Cyc_NewControlFlow_update_tryflow(env,_tmp76);
return _tmp76;};};};}_LL4A:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 418
static struct _tuple23 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 425
if(es == 0){
struct _tuple23 _tmp6B8;return(_tmp6B8.f1=inflow,((_tmp6B8.f2=0,_tmp6B8)));}
if(es->tl == 0){
struct _tuple16 _tmp78=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp7A;void*_tmp7B;struct _tuple16 _tmp79=_tmp78;_tmp7A=_tmp79.f1;_tmp7B=_tmp79.f2;{
struct Cyc_List_List*_tmp6BB;struct _tuple23 _tmp6BA;return(_tmp6BA.f1=_tmp7A,((_tmp6BA.f2=((_tmp6BB=_region_malloc(rgn,sizeof(*_tmp6BB)),((_tmp6BB->hd=_tmp7B,((_tmp6BB->tl=0,_tmp6BB)))))),_tmp6BA)));};}{
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
{struct Cyc_Dict_Dict*_tmp6BC;env->all_changed=((_tmp6BC=_region_malloc(env->r,sizeof(*_tmp6BC)),((_tmp6BC[0]=(env->fenv)->mt_place_set,_tmp6BC))));}{
struct _tuple16 _tmp7F=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp81;void*_tmp82;struct _tuple16 _tmp80=_tmp7F;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;
outflow=_tmp81;
{struct Cyc_List_List*_tmp6BD;rvals=((_tmp6BD=_region_malloc(rgn,sizeof(*_tmp6BD)),((_tmp6BD->hd=_tmp82,((_tmp6BD->tl=0,_tmp6BD))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 448
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp6BE;env->all_changed=((_tmp6BE=_region_malloc(env->r,sizeof(*_tmp6BE)),((_tmp6BE[0]=(env->fenv)->mt_place_set,_tmp6BE))));}{
struct _tuple16 _tmp85=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp87;void*_tmp88;struct _tuple16 _tmp86=_tmp85;_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;
{struct Cyc_List_List*_tmp6BF;rvals=((_tmp6BF=_region_malloc(rgn,sizeof(*_tmp6BF)),((_tmp6BF->hd=_tmp88,((_tmp6BF->tl=rvals,_tmp6BF))))));}
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
struct Cyc_Dict_Dict*_tmp6C0;env->all_changed=((_tmp6C0=_region_malloc(env->r,sizeof(*_tmp6C0)),((_tmp6C0[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp6C0))));}
# 471
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 473
struct _tuple23 _tmp6C1;return(_tmp6C1.f1=outflow,((_tmp6C1.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp6C1)));};};}
# 478
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 481
struct _RegionHandle _tmp8C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8C;_push_region(rgn);
{struct _tuple23 _tmp8D=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 482
union Cyc_CfFlowInfo_FlowInfo _tmp8F;struct Cyc_List_List*_tmp90;struct _tuple23 _tmp8E=_tmp8D;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;
# 484
{union Cyc_CfFlowInfo_FlowInfo _tmp91=_tmp8F;struct Cyc_Dict_Dict _tmp92;if((_tmp91.ReachableFL).tag == 2){_LL57: _tmp92=((_tmp91.ReachableFL).val).f1;_LL58:
# 486
 for(0;_tmp90 != 0;(_tmp90=_tmp90->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp92,(void*)_tmp90->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp6C4;void*_tmp6C3;(_tmp6C3=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp6C4="expression may not be initialized",_tag_dyneither(_tmp6C4,sizeof(char),34))),_tag_dyneither(_tmp6C3,sizeof(void*),0)));}}
goto _LL56;}else{_LL59: _LL5A:
 goto _LL56;}_LL56:;}{
# 492
struct _tuple16 _tmp6C5;struct _tuple16 _tmp96=(_tmp6C5.f1=_tmp8F,((_tmp6C5.f2=(env->fenv)->unknown_all,_tmp6C5)));_npop_handler(0);return _tmp96;};}
# 482
;_pop_region(rgn);}
# 500
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 507
int needs_unconsume=0;
void*_tmp97=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp97,& needs_unconsume)){
struct Cyc_Core_Impossible_exn_struct _tmp6CB;const char*_tmp6CA;struct Cyc_Core_Impossible_exn_struct*_tmp6C9;(int)_throw((void*)((_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9[0]=((_tmp6CB.tag=Cyc_Core_Impossible,((_tmp6CB.f1=((_tmp6CA="consume_zero_ral",_tag_dyneither(_tmp6CA,sizeof(char),17))),_tmp6CB)))),_tmp6C9)))));}else{
# 512
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 516
return new_rval;}}}
# 529 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 535
union Cyc_CfFlowInfo_FlowInfo _tmp9B=outflow;struct Cyc_Dict_Dict _tmp9C;struct Cyc_List_List*_tmp9D;if((_tmp9B.BottomFL).tag == 1){_LL5C: _LL5D:
 return outflow;}else{_LL5E: _tmp9C=((_tmp9B.ReachableFL).val).f1;_tmp9D=((_tmp9B.ReachableFL).val).f2;_LL5F: {
# 538
union Cyc_CfFlowInfo_AbsLVal _tmp9E=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp9F=_tmp9E;struct Cyc_CfFlowInfo_Place*_tmpA0;if((_tmp9F.UnknownL).tag == 2){_LL61: _LL62:
# 542
 return outflow;}else{_LL63: _tmpA0=(_tmp9F.PlaceL).val;_LL64: {
# 546
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6CE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6CD;nzval=(void*)((_tmp6CD=_region_malloc((env->fenv)->r,sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CE.tag=8,((_tmp6CE.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp6CE.f2=nzval,_tmp6CE)))))),_tmp6CD))));}
# 550
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9C,_tmpA0,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpA3=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9C,env->all_changed,_tmpA0,nzval),_tmp9D);
# 555
return _tmpA3;};}}_LL60:;}}_LL5B:;}
# 564
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 567
union Cyc_CfFlowInfo_FlowInfo _tmpA4=outflow;struct Cyc_Dict_Dict _tmpA5;struct Cyc_List_List*_tmpA6;if((_tmpA4.BottomFL).tag == 1){_LL66: _LL67: {
struct _tuple19 _tmp6CF;return(_tmp6CF.f1=outflow,((_tmp6CF.f2=outflow,_tmp6CF)));}}else{_LL68: _tmpA5=((_tmpA4.ReachableFL).val).f1;_tmpA6=((_tmpA4.ReachableFL).val).f2;_LL69: {
# 570
union Cyc_CfFlowInfo_AbsLVal _tmpA8=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpA9=_tmpA8;struct Cyc_CfFlowInfo_Place*_tmpAA;if((_tmpA9.UnknownL).tag == 2){_LL6B: _LL6C: {
struct _tuple19 _tmp6D0;return(_tmp6D0.f1=outflow,((_tmp6D0.f2=outflow,_tmp6D0)));}}else{_LL6D: _tmpAA=(_tmpA9.PlaceL).val;_LL6E: {
# 573
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D2;nzval=(void*)((_tmp6D2=_region_malloc((env->fenv)->r,sizeof(*_tmp6D2)),((_tmp6D2[0]=((_tmp6D3.tag=8,((_tmp6D3.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp6D3.f2=nzval,_tmp6D3)))))),_tmp6D2))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6D6;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6D5;zval=(void*)((_tmp6D5=_region_malloc((env->fenv)->r,sizeof(*_tmp6D5)),((_tmp6D5[0]=((_tmp6D6.tag=8,((_tmp6D6.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp6D6.f2=zval,_tmp6D6)))))),_tmp6D5))));};}
# 579
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA5,_tmpAA,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA5,_tmpAA,e,zval);{
struct _tuple19 _tmp6D7;return
(_tmp6D7.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA5,env->all_changed,_tmpAA,nzval),_tmpA6),((_tmp6D7.f2=
# 584
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA5,env->all_changed,_tmpAA,zval),_tmpA6),_tmp6D7)));};}}_LL6A:;}}_LL65:;}
# 590
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 598
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 602
void*_tmpB3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpB4=_tmpB3;void*_tmpB5;union Cyc_Absyn_Constraint*_tmpB6;union Cyc_Absyn_Constraint*_tmpB7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->tag == 5){_LL70: _tmpB5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->f1).elt_typ;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->f1).ptr_atts).bounds;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB4)->f1).ptr_atts).zero_term;_LL71: {
# 604
union Cyc_CfFlowInfo_FlowInfo _tmpB8=f;struct Cyc_Dict_Dict _tmpB9;struct Cyc_List_List*_tmpBA;if((_tmpB8.BottomFL).tag == 1){_LL75: _LL76: {
# 606
struct _tuple16 _tmp6D8;return(_tmp6D8.f1=f,((_tmp6D8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_all),_tmp6D8)));}}else{_LL77: _tmpB9=((_tmpB8.ReachableFL).val).f1;_tmpBA=((_tmpB8.ReachableFL).val).f2;_LL78:
# 609
 if(Cyc_Tcutil_is_bound_one(_tmpB6)){
struct _tuple15 _tmpBC=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmpBE;struct Cyc_List_List*_tmpBF;struct _tuple15 _tmpBD=_tmpBC;_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;{
void*_tmpC0=_tmpBE;enum Cyc_CfFlowInfo_InitLevel _tmpC1;struct Cyc_CfFlowInfo_Place*_tmpC2;struct Cyc_Absyn_Vardecl*_tmpC3;void*_tmpC4;switch(*((int*)_tmpC0)){case 8: _LL7A: _tmpC3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;_tmpC4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f2;_LL7B: {
# 613
struct Cyc_Core_Impossible_exn_struct _tmp6DE;const char*_tmp6DD;struct Cyc_Core_Impossible_exn_struct*_tmp6DC;(int)_throw((void*)((_tmp6DC=_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DE.tag=Cyc_Core_Impossible,((_tmp6DE.f1=((_tmp6DD="named location in anal_derefR",_tag_dyneither(_tmp6DD,sizeof(char),30))),_tmp6DE)))),_tmp6DC)))));}case 1: _LL7C: _LL7D:
 goto _LL7F;case 2: _LL7E: _LL7F:
# 616
{void*_tmpC8=e->annot;void*_tmpC9=_tmpC8;struct Cyc_List_List*_tmpCA;if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpC9)->tag == Cyc_CfFlowInfo_NotZero){_LL89: _tmpCA=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpC9)->f1;_LL8A:
# 618
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpCA))goto _LL8C;
goto _LL88;}else{_LL8B: _LL8C:
# 622
{void*_tmpCB=e->r;void*_tmpCC=_tmpCB;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCC)->tag == 22){_LL8E: _LL8F:
# 624
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E1;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E0;e->annot=(void*)((_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0[0]=((_tmp6E1.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E1.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA),_tmp6E1)))),_tmp6E0))));}
goto _LL8D;}else{_LL90: _LL91:
# 627
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;}_LL8D:;}
# 630
goto _LL88;}_LL88:;}
# 632
goto _LL79;case 5: _LL80: _tmpC2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;_LL81: {
# 636
int possibly_null=0;
{void*_tmpCF=e->annot;void*_tmpD0=_tmpCF;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD2;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpD0)->tag == Cyc_CfFlowInfo_UnknownZ){_LL93: _tmpD2=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpD0)->f1;_LL94:
# 639
 possibly_null=1;
_tmpD1=_tmpD2;goto _LL96;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD0)->tag == Cyc_CfFlowInfo_NotZero){_LL95: _tmpD1=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpD0)->f1;_LL96:
# 642
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpD1))goto _LL98;
goto _LL92;}else{_LL97: _LL98:
# 646
{void*_tmpD3=e->r;void*_tmpD4=_tmpD3;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD4)->tag == 22){_LL9A: _LL9B:
# 648
 if(possibly_null){
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6E4;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6E3;e->annot=(void*)((_tmp6E3=_cycalloc(sizeof(*_tmp6E3)),((_tmp6E3[0]=((_tmp6E4.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6E4.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA),_tmp6E4)))),_tmp6E3))));}else{
# 651
struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6E7;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp6E6;e->annot=(void*)((_tmp6E6=_cycalloc(sizeof(*_tmp6E6)),((_tmp6E6[0]=((_tmp6E7.tag=Cyc_CfFlowInfo_NotZero,((_tmp6E7.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA),_tmp6E7)))),_tmp6E6))));}
goto _LL99;}else{_LL9C: _LL9D:
# 654
 if(possibly_null)
e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;else{
# 657
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;}
goto _LL99;}_LL99:;}
# 660
goto _LL92;}}_LL92:;}{
# 662
struct _tuple16 _tmp6E8;return(_tmp6E8.f1=f,((_tmp6E8.f2=Cyc_CfFlowInfo_lookup_place(_tmpB9,_tmpC2),_tmp6E8)));};}case 0: _LL82: _LL83:
# 664
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _tmp6E9;return(_tmp6E9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp6E9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_all),_tmp6E9)));};case 3: _LL84: _tmpC1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC0)->f1;_LL85:
# 667
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC1,_tmpBF);goto _LL87;default: _LL86: _LL87:
# 669
{void*_tmpDB=e->r;void*_tmpDC=_tmpDB;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC)->tag == 22){_LL9F: _LLA0:
# 671
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EC;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6EB;e->annot=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EC.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EC.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA),_tmp6EC)))),_tmp6EB))));}
goto _LL9E;}else{_LLA1: _LLA2:
# 674
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9E;}_LL9E:;}
# 677
goto _LL79;}_LL79:;};}else{
# 680
void*_tmpDF=e->annot;void*_tmpE0=_tmpDF;struct Cyc_List_List*_tmpE1;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE0)->tag == Cyc_CfFlowInfo_UnknownZ){_LLA4: _tmpE1=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpE0)->f1;_LLA5:
# 682
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpE1))goto _LLA7;
goto _LLA3;}else{_LLA6: _LLA7:
# 685
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6EF;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp6EE;e->annot=(void*)((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((_tmp6EF.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp6EF.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBA),_tmp6EF)))),_tmp6EE))));}
goto _LLA3;}_LLA3:;}{
# 689
enum Cyc_CfFlowInfo_InitLevel _tmpE4=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpB9,r);enum Cyc_CfFlowInfo_InitLevel _tmpE5=_tmpE4;switch(_tmpE5){case Cyc_CfFlowInfo_NoneIL: _LLA9: _LLAA: {
# 691
struct _tuple15 _tmpE6=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmpE8;struct _tuple15 _tmpE7=_tmpE6;_tmpE8=_tmpE7.f1;
{void*_tmpE9=_tmpE8;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpE9)->tag == 7){_LLB0: _LLB1:
# 694
{const char*_tmp6F2;void*_tmp6F1;(_tmp6F1=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp6F2="attempt to dereference a consumed alias-free pointer",_tag_dyneither(_tmp6F2,sizeof(char),53))),_tag_dyneither(_tmp6F1,sizeof(void*),0)));}
goto _LLAF;}else{_LLB2: _LLB3: {
# 697
const char*_tmp6F5;void*_tmp6F4;(_tmp6F4=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp6F5="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp6F5,sizeof(char),46))),_tag_dyneither(_tmp6F4,sizeof(void*),0)));}}_LLAF:;}
# 699
goto _LLAC;}case Cyc_CfFlowInfo_AllIL: _LLAB: _LLAC: {
# 701
struct _tuple16 _tmp6F6;return(_tmp6F6.f1=f,((_tmp6F6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_all),_tmp6F6)));}default: _LLAD: _LLAE: {
# 703
struct _tuple16 _tmp6F7;return(_tmp6F7.f1=f,((_tmp6F7.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpB5,0,(env->fenv)->unknown_none),_tmp6F7)));}}_LLA8:;};}_LL74:;}}else{_LL72: _LL73: {
# 706
struct Cyc_Core_Impossible_exn_struct _tmp6FD;const char*_tmp6FC;struct Cyc_Core_Impossible_exn_struct*_tmp6FB;(int)_throw((void*)((_tmp6FB=_cycalloc(sizeof(*_tmp6FB)),((_tmp6FB[0]=((_tmp6FD.tag=Cyc_Core_Impossible,((_tmp6FD.f1=((_tmp6FC="right deref of non-pointer-type",_tag_dyneither(_tmp6FC,sizeof(char),32))),_tmp6FD)))),_tmp6FB)))));}}_LL6F:;}
# 713
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 716
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 719
{void*_tmpF3=e1->r;void*_tmpF4=_tmpF3;void*_tmpF5;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF4)->tag == 1){_LLB5: _tmpF5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LLB6: {
# 721
struct Cyc_Absyn_Vardecl*_tmpF6=Cyc_Tcutil_nonesc_vardecl(_tmpF5);
if(_tmpF6 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpF6);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 727
goto _LLB4;}}else{_LLB7: _LLB8:
 goto _LLB4;}_LLB4:;}{
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
struct _tuple18 _tmpF7=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmpF9;struct _tuple18 _tmpF8=_tmpF7;_tmpF9=_tmpF8.f2;
{struct _tuple18 _tmp6FE;struct _tuple18 _tmpFA=(_tmp6FE.f1=inflow,((_tmp6FE.f2=_tmpF9,_tmp6FE)));struct _tuple18 _tmpFB=_tmpFA;struct Cyc_Dict_Dict _tmpFC;struct Cyc_List_List*_tmpFD;struct Cyc_CfFlowInfo_Place*_tmpFE;if(((_tmpFB.f1).ReachableFL).tag == 2){if(((_tmpFB.f2).PlaceL).tag == 1){_LLBA: _tmpFC=(((_tmpFB.f1).ReachableFL).val).f1;_tmpFD=(((_tmpFB.f1).ReachableFL).val).f2;_tmpFE=((_tmpFB.f2).PlaceL).val;_LLBB: {
# 753
void*_tmpFF=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 755
struct _tuple15 _tmp100=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);void*_tmp102;struct Cyc_List_List*_tmp103;struct _tuple15 _tmp101=_tmp100;_tmp102=_tmp101.f1;_tmp103=_tmp101.f2;
for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp701;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp700;_tmpFF=(void*)((_tmp700=_region_malloc((env->fenv)->r,sizeof(*_tmp700)),((_tmp700[0]=((_tmp701.tag=8,((_tmp701.f1=(struct Cyc_Absyn_Vardecl*)_tmp103->hd,((_tmp701.f2=_tmpFF,_tmp701)))))),_tmp700))));}
# 760
_tmpFC=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpFC,env->all_changed,_tmpFE,_tmpFF);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmpFC,_tmpFD);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 766
goto _LLB9;}}else{_LLBE: _LLBF:
# 769
{const char*_tmp704;void*_tmp703;(_tmp703=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp704="noconsume() parameters must be l-values",_tag_dyneither(_tmp704,sizeof(char),40))),_tag_dyneither(_tmp703,sizeof(void*),0)));}
goto _LLB9;}}else{_LLBC: _LLBD:
# 767
 goto _LLB9;}_LLB9:;}
# 772
return inflow;}
# 777
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 785
union Cyc_CfFlowInfo_FlowInfo _tmp109=outflow;struct Cyc_Dict_Dict _tmp10A;struct Cyc_List_List*_tmp10B;if((_tmp109.BottomFL).tag == 1){_LLC1: _LLC2: {
# 787
struct _tuple16 _tmp705;return(_tmp705.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp705.f2=rval,_tmp705)));}}else{_LLC3: _tmp10A=((_tmp109.ReachableFL).val).f1;_tmp10B=((_tmp109.ReachableFL).val).f2;_LLC4: {
# 789
union Cyc_CfFlowInfo_AbsLVal _tmp10D=lval;struct Cyc_CfFlowInfo_Place*_tmp10E;if((_tmp10D.PlaceL).tag == 1){_LLC6: _tmp10E=(_tmp10D.PlaceL).val;_LLC7: {
# 791
struct Cyc_Dict_Dict _tmp10F=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp10A,env->all_changed,_tmp10E,rval);
_tmp10B=Cyc_Relations_reln_assign_exp(fenv->r,_tmp10B,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp10F,_tmp10B);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 797
struct _tuple15 _tmp110=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp10A,_tmp10E));void*_tmp112;struct _tuple15 _tmp111=_tmp110;_tmp112=_tmp111.f1;{
void*_tmp113=_tmp112;switch(*((int*)_tmp113)){case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp113)->f1 == Cyc_CfFlowInfo_NoneIL){_LLCB: _LLCC:
 goto _LLCE;}else{goto _LLD1;}case 7: _LLCD: _LLCE:
 goto _LLD0;case 0: _LLCF: _LLD0:
 goto _LLCA;default: _LLD1: _LLD2:
# 803
{const char*_tmp708;void*_tmp707;(_tmp707=0,Cyc_Tcutil_warn(lexp->loc,((_tmp708="assignment may overwrite alias-free pointer(s)",_tag_dyneither(_tmp708,sizeof(char),47))),_tag_dyneither(_tmp707,sizeof(void*),0)));}
goto _LLCA;}_LLCA:;};}
# 808
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple16 _tmp709;return(_tmp709.f1=outflow,((_tmp709.f2=rval,_tmp709)));};}}else{_LLC8: _LLC9: {
# 811
struct _tuple16 _tmp70A;return(_tmp70A.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp70A.f2=rval,_tmp70A)));}}_LLC5:;}}_LLC0:;}
# 818
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 824
union Cyc_CfFlowInfo_FlowInfo _tmp118=f;struct Cyc_Dict_Dict _tmp119;struct Cyc_List_List*_tmp11A;if((_tmp118.BottomFL).tag == 1){_LLD4: _LLD5:
 return Cyc_CfFlowInfo_BottomFL();}else{_LLD6: _tmp119=((_tmp118.ReachableFL).val).f1;_tmp11A=((_tmp118.ReachableFL).val).f2;_LLD7:
# 829
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp710;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp70F;struct Cyc_CfFlowInfo_Place*_tmp70E;_tmp119=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp119,env->all_changed,(
(_tmp70E=_region_malloc(env->r,sizeof(*_tmp70E)),((_tmp70E->root=(void*)((_tmp710=_region_malloc(env->r,sizeof(*_tmp710)),((_tmp710[0]=((_tmp70F.tag=0,((_tmp70F.f1=vd,_tmp70F)))),_tmp710)))),((_tmp70E->fields=0,_tmp70E)))))),rval);}
# 832
_tmp11A=Cyc_Relations_reln_assign_var(env->r,_tmp11A,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp11E=Cyc_CfFlowInfo_ReachableFL(_tmp119,_tmp11A);
Cyc_NewControlFlow_update_tryflow(env,_tmp11E);
# 837
return _tmp11E;};}_LLD3:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 841
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 848
if(vds == 0)return inflow;{
# 851
struct Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp711;es=((_tmp711=_cycalloc(sizeof(*_tmp711)),((_tmp711->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),((_tmp711->tl=es,_tmp711))))));}}}
# 857
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp11F,fenv->unknown_all,pat_loc,name_locs);
# 859
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 863
struct _tuple16 _tmp121=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp123;void*_tmp124;struct _tuple16 _tmp122=_tmp121;_tmp123=_tmp122.f1;_tmp124=_tmp122.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp123,_tmp124);}}{
# 871
struct Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp125 != 0;_tmp125=_tmp125->tl){
struct _tuple24*_tmp126=(struct _tuple24*)_tmp125->hd;struct Cyc_Absyn_Vardecl**_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct _tuple24*_tmp127=_tmp126;_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;
if(_tmp128 != 0  && _tmp129 != 0){
if(_tmp129->topt == 0){
const char*_tmp715;void*_tmp714[1];struct Cyc_String_pa_PrintArg_struct _tmp713;(_tmp713.tag=0,((_tmp713.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp129)),((_tmp714[0]=& _tmp713,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp715="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp715,sizeof(char),41))),_tag_dyneither(_tmp714,sizeof(void*),1)))))));}{
# 885
struct Cyc_List_List _tmp716;struct Cyc_List_List l=(_tmp716.hd=_tmp129,((_tmp716.tl=0,_tmp716)));
union Cyc_CfFlowInfo_FlowInfo _tmp12D=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp12E=Cyc_NewControlFlow_anal_Lexp(env,_tmp12D,0,0,_tmp129);union Cyc_CfFlowInfo_AbsLVal _tmp130;struct _tuple18 _tmp12F=_tmp12E;_tmp130=_tmp12F.f2;{
struct _tuple16 _tmp131=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp12D,_tmp129);union Cyc_CfFlowInfo_FlowInfo _tmp133;void*_tmp134;struct _tuple16 _tmp132=_tmp131;_tmp133=_tmp132.f1;_tmp134=_tmp132.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp135=_tmp133;struct Cyc_Dict_Dict _tmp136;struct Cyc_List_List*_tmp137;if((_tmp135.ReachableFL).tag == 2){_LLD9: _tmp136=((_tmp135.ReachableFL).val).f1;_tmp137=((_tmp135.ReachableFL).val).f2;_LLDA:
# 891
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp138=_tmp130;struct Cyc_CfFlowInfo_Place*_tmp139;if((_tmp138.PlaceL).tag == 1){_LLDE: _tmp139=(_tmp138.PlaceL).val;_LLDF:
# 894
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp719;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp718;_tmp134=(void*)((_tmp718=_region_malloc(fenv->r,sizeof(*_tmp718)),((_tmp718[0]=((_tmp719.tag=8,((_tmp719.f1=*_tmp128,((_tmp719.f2=_tmp134,_tmp719)))))),_tmp718))));}{
# 897
void*_tmp13C=Cyc_CfFlowInfo_lookup_place(_tmp136,_tmp139);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp71C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp71B;_tmp13C=(void*)((_tmp71B=_region_malloc(fenv->r,sizeof(*_tmp71B)),((_tmp71B[0]=((_tmp71C.tag=8,((_tmp71C.f1=*_tmp128,((_tmp71C.f2=_tmp13C,_tmp71C)))))),_tmp71B))));}
_tmp136=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp136,env->all_changed,_tmp139,_tmp13C);
_tmp133=Cyc_CfFlowInfo_ReachableFL(_tmp136,_tmp137);
goto _LLDD;};}else{_LLE0: _LLE1:
# 904
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp129->topt)) && !
# 906
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp128)->type)){
# 908
const char*_tmp720;void*_tmp71F[1];struct Cyc_String_pa_PrintArg_struct _tmp71E;(_tmp71E.tag=0,((_tmp71E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp129)),((_tmp71F[0]=& _tmp71E,Cyc_CfFlowInfo_aerr(exp_loc,((_tmp720="aliased pattern expression not an l-value: %s",_tag_dyneither(_tmp720,sizeof(char),46))),_tag_dyneither(_tmp71F,sizeof(void*),1)))))));}}_LLDD:;}
# 916
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp133,*_tmp128,_tmp129,_tmp134,pat_loc);
goto _LLD8;}else{_LLDB: _LLDC:
# 919
 goto _LLD8;}_LLD8:;};};};}}
# 924
return inflow;};};}
# 927
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 929
void*_tmp143=e->r;void*_tmp144=_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp149;switch(*((int*)_tmp144)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp144)->f2)){case 4: _LLE3: _LLE4:
 goto _LLE6;case 3: _LLE5: _LLE6:
 goto _LLE8;case 5: _LLE7: _LLE8:
 return 1;default: goto _LLF3;}case 19: _LLE9: _tmp149=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LLEA:
 _tmp148=_tmp149;goto _LLEC;case 20: _LLEB: _tmp148=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LLEC:
 _tmp147=_tmp148;goto _LLEE;case 21: _LLED: _tmp147=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LLEE:
# 936
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp147,cast_ok);case 22: _LLEF: _tmp146=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp144)->f1;_LLF0: {
# 938
void*_tmp14A=Cyc_Tcutil_compress((void*)_check_null(_tmp146->topt));void*_tmp14B=_tmp14A;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14B)->tag == 10){_LLF6: _LLF7:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp146,cast_ok);}else{_LLF8: _LLF9:
 return 0;}_LLF5:;}case 13: _LLF1: _tmp145=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp144)->f2;_LLF2:
# 943
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp145,cast_ok);else{
return 0;}default: _LLF3: _LLF4:
 return 0;}_LLE2:;}
# 949
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp14C=*rop;union Cyc_Relations_RelnOp _tmp14D=_tmp14C;unsigned int _tmp14E;unsigned int _tmp14F;switch((_tmp14D.RParamNumelts).tag){case 5: _LLFB: _tmp14F=(_tmp14D.RParam).val;_LLFC: {
# 952
struct Cyc_Absyn_Exp*_tmp150=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp14F);
return Cyc_Relations_exp2relnop(_tmp150,rop);}case 6: _LLFD: _tmp14E=(_tmp14D.RParamNumelts).val;_LLFE: {
# 955
struct Cyc_Absyn_Exp*_tmp151=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp14E);
return Cyc_Relations_exp2relnop(Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp151,0),rop);}default: _LLFF: _LL100:
 return 1;}_LLFA:;}
# 961
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp152=rop;unsigned int _tmp153;unsigned int _tmp154;switch((_tmp152.RParamNumelts).tag){case 5: _LL102: _tmp154=(_tmp152.RParam).val;_LL103:
# 964
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp154));case 6: _LL104: _tmp153=(_tmp152.RParamNumelts).val;_LL105:
# 966
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp153));default: _LL106: _LL107:
 return Cyc_Relations_rop2string(rop);}_LL101:;}
# 971
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 974
union Cyc_CfFlowInfo_FlowInfo _tmp155=inflow;struct Cyc_Dict_Dict _tmp156;struct Cyc_List_List*_tmp157;if((_tmp155.BottomFL).tag == 1){_LL109: _LL10A:
 return;}else{_LL10B: _tmp156=((_tmp155.ReachableFL).val).f1;_tmp157=((_tmp155.ReachableFL).val).f2;_LL10C:
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
{const char*_tmp727;void*_tmp726[4];struct Cyc_String_pa_PrintArg_struct _tmp725;struct Cyc_String_pa_PrintArg_struct _tmp724;struct Cyc_String_pa_PrintArg_struct _tmp723;struct Cyc_String_pa_PrintArg_struct _tmp722;(_tmp722.tag=0,((_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp157)),((_tmp723.tag=0,((_tmp723.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp724.tag=0,((_tmp724.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp725.tag=0,((_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp726[0]=& _tmp725,((_tmp726[1]=& _tmp724,((_tmp726[2]=& _tmp723,((_tmp726[3]=& _tmp722,Cyc_Tcutil_terr(loc,((_tmp727="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)",_tag_dyneither(_tmp727,sizeof(char),77))),_tag_dyneither(_tmp726,sizeof(void*),4)))))))))))))))))))))))));}
break;}}
# 992
goto _LL108;}_LL108:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp7DA(unsigned int*_tmp7D9,unsigned int*_tmp7D8,void***_tmp7D6,struct Cyc_List_List**_tmp31F){for(*_tmp7D9=0;*_tmp7D9 < *_tmp7D8;(*_tmp7D9)++){
# 1585 "new_control_flow.cyc"
void*_tmp7D4;(*_tmp7D6)[*_tmp7D9]=(void*)((_tmp7D4=(void*)((struct Cyc_List_List*)_check_null(*_tmp31F))->hd,((*_tmp31F=(*_tmp31F)->tl,_tmp7D4))));}}
# 997 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1001
struct Cyc_CfFlowInfo_FlowEnv*_tmp160=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 1014
{union Cyc_CfFlowInfo_FlowInfo _tmp161=inflow;struct Cyc_Dict_Dict _tmp162;struct Cyc_List_List*_tmp163;if((_tmp161.BottomFL).tag == 1){_LL10E: _LL10F: {
struct _tuple16 _tmp728;return(_tmp728.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp728.f2=_tmp160->unknown_all,_tmp728)));}}else{_LL110: _tmp162=((_tmp161.ReachableFL).val).f1;_tmp163=((_tmp161.ReachableFL).val).f2;_LL111:
 d=_tmp162;relns=_tmp163;}_LL10D:;}
# 1029 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 1050 "new_control_flow.cyc"
struct _tuple18 _tmp165=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp167;union Cyc_CfFlowInfo_AbsLVal _tmp168;struct _tuple18 _tmp166=_tmp165;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;{
struct _tuple18 _tmp729;struct _tuple18 _tmp169=(_tmp729.f1=_tmp167,((_tmp729.f2=_tmp168,_tmp729)));struct _tuple18 _tmp16A=_tmp169;struct Cyc_Dict_Dict _tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_CfFlowInfo_Place*_tmp16D;if(((_tmp16A.f1).ReachableFL).tag == 2){if(((_tmp16A.f2).PlaceL).tag == 1){_LL113: _tmp16B=(((_tmp16A.f1).ReachableFL).val).f1;_tmp16C=(((_tmp16A.f1).ReachableFL).val).f2;_tmp16D=((_tmp16A.f2).PlaceL).val;_LL114: {
# 1053
void*_tmp16E=Cyc_CfFlowInfo_lookup_place(_tmp16B,_tmp16D);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp16E,& needs_unconsume)){
{const char*_tmp72C;void*_tmp72B;(_tmp72B=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp72C="expression attempts to copy a consumed alias-free value",_tag_dyneither(_tmp72C,sizeof(char),56))),_tag_dyneither(_tmp72B,sizeof(void*),0)));}{
struct _tuple16 _tmp72D;return(_tmp72D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp72D.f2=_tmp160->unknown_all,_tmp72D)));};}else{
# 1059
if(needs_unconsume){
# 1061
struct _tuple16 _tmp72E;return(_tmp72E.f1=_tmp167,((_tmp72E.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp160,_tmp16E),_tmp72E)));}else{
# 1064
void*_tmp173=Cyc_CfFlowInfo_make_unique_consumed(_tmp160,(void*)_check_null(e->topt),e,env->iteration_num,_tmp16E);
struct Cyc_Dict_Dict _tmp174=Cyc_CfFlowInfo_assign_place(_tmp160,e->loc,_tmp16B,env->all_changed,_tmp16D,_tmp173);
# 1076
struct _tuple16 _tmp72F;return(_tmp72F.f1=Cyc_CfFlowInfo_ReachableFL(_tmp174,_tmp16C),((_tmp72F.f2=_tmp16E,_tmp72F)));}}}}else{goto _LL115;}}else{_LL115: _LL116:
# 1078
 goto _LL112;}_LL112:;};}{
# 1081
void*_tmp177=e->r;void*_tmp178=_tmp177;struct Cyc_Absyn_Stmt*_tmp179;struct Cyc_Absyn_Exp*_tmp17A;void*_tmp17B;int _tmp17C;struct Cyc_Absyn_Vardecl*_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;int _tmp180;struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp182;enum Cyc_Absyn_AggrKind _tmp183;struct Cyc_List_List*_tmp184;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Exp*_tmp187;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp190;struct _dyneither_ptr*_tmp191;struct Cyc_Absyn_Exp*_tmp192;struct _dyneither_ptr*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct _dyneither_ptr*_tmp195;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp19B;int _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;void**_tmp19E;struct Cyc_Absyn_Exp*_tmp19F;int _tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;enum Cyc_Absyn_Primop _tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_Absyn_Datatypedecl*_tmp1AE;struct Cyc_Absyn_Vardecl*_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B0;struct Cyc_Absyn_Vardecl*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;switch(*((int*)_tmp178)){case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp178)->f4 == Cyc_Absyn_NonNull_to_Null){_LL118: _tmp1B5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL119: {
# 1085
struct _tuple16 _tmp1B6=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1B5);union Cyc_CfFlowInfo_FlowInfo _tmp1B8;void*_tmp1B9;struct _tuple16 _tmp1B7=_tmp1B6;_tmp1B8=_tmp1B7.f1;_tmp1B9=_tmp1B7.f2;{
struct _tuple16 _tmp1BA=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1B8,_tmp1B5,_tmp1B9);union Cyc_CfFlowInfo_FlowInfo _tmp1BC;void*_tmp1BD;struct _tuple16 _tmp1BB=_tmp1BA;_tmp1BC=_tmp1BB.f1;_tmp1BD=_tmp1BB.f2;{
struct _tuple16 _tmp730;return(_tmp730.f1=_tmp1BC,((_tmp730.f2=_tmp1B9,_tmp730)));};};}}else{_LL11A: _tmp1B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL11B:
# 1091
 _tmp1B3=_tmp1B4;goto _LL11D;}case 11: _LL11C: _tmp1B3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL11D:
 _tmp1B2=_tmp1B3;goto _LL11F;case 12: _LL11E: _tmp1B2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL11F:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1B2);case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp178)->f1).Int_c).tag){case 1: _LL120: _LL121:
# 1095
 goto _LL123;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp178)->f1).Int_c).val).f2 == 0){_LL122: _LL123: {
struct _tuple16 _tmp731;return(_tmp731.f1=inflow,((_tmp731.f2=_tmp160->zero,_tmp731)));}}else{_LL124: _LL125:
 goto _LL127;}default: _LL12A: _LL12B:
# 1101
 goto _LL12D;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)){case 2: _LL126: _LL127: {
# 1098
struct _tuple16 _tmp732;return(_tmp732.f1=inflow,((_tmp732.f2=_tmp160->notzeroall,_tmp732)));}case 1: _LL136: _LL137: {
# 1109
struct _tuple16 _tmp733;return(_tmp733.f1=inflow,((_tmp733.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all),_tmp733)));}case 3: _LL138: _tmp1B1=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)->f1;_LL139:
# 1112
 _tmp1B0=_tmp1B1;goto _LL13B;case 4: _LL13A: _tmp1B0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)->f1;_LL13B:
 _tmp1AF=_tmp1B0;goto _LL13D;case 5: _LL13C: _tmp1AF=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp178)->f2)->f1;_LL13D: {
# 1116
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp734;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp734.tag=0,((_tmp734.f1=_tmp1AF,_tmp734)));
struct _tuple16 _tmp735;return(_tmp735.f1=inflow,((_tmp735.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp735)));}default: _LL176: _LL177:
# 1723
 goto _LL179;}case 30: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp178)->f1 == 0){_LL128: _LL129:
# 1100
 goto _LL12B;}else{_LL164: _tmp1AD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1AE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL165:
# 1579
 _tmp186=_tmp1AD;goto _LL167;}case 17: _LL12C: _LL12D:
# 1102
 goto _LL12F;case 16: _LL12E: _LL12F:
 goto _LL131;case 18: _LL130: _LL131:
 goto _LL133;case 32: _LL132: _LL133:
 goto _LL135;case 31: _LL134: _LL135: {
struct _tuple16 _tmp736;return(_tmp736.f1=inflow,((_tmp736.f2=_tmp160->unknown_all,_tmp736)));}case 2: _LL13E: _tmp1AB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1AC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL13F: {
# 1123
struct _tuple16 _tmp1C5=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1AC);union Cyc_CfFlowInfo_FlowInfo _tmp1C7;void*_tmp1C8;struct _tuple16 _tmp1C6=_tmp1C5;_tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;{
struct _tuple16 _tmp737;return(_tmp737.f1=_tmp1C7,((_tmp737.f2=_tmp1C8,_tmp737)));};}case 4: _LL140: _tmp1AA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL141: {
# 1127
struct Cyc_List_List _tmp738;struct Cyc_List_List _tmp1CA=(_tmp738.hd=_tmp1AA,((_tmp738.tl=0,_tmp738)));
struct _tuple16 _tmp1CB=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1CA);union Cyc_CfFlowInfo_FlowInfo _tmp1CD;struct _tuple16 _tmp1CC=_tmp1CB;_tmp1CD=_tmp1CC.f1;{
struct _tuple18 _tmp1CE=Cyc_NewControlFlow_anal_Lexp(env,_tmp1CD,0,0,_tmp1AA);union Cyc_CfFlowInfo_AbsLVal _tmp1D0;struct _tuple18 _tmp1CF=_tmp1CE;_tmp1D0=_tmp1CF.f2;
{struct _tuple25 _tmp739;struct _tuple25 _tmp1D1=(_tmp739.f1=_tmp1D0,((_tmp739.f2=_tmp1CD,_tmp739)));struct _tuple25 _tmp1D2=_tmp1D1;struct Cyc_CfFlowInfo_Place*_tmp1D3;struct Cyc_Dict_Dict _tmp1D4;struct Cyc_List_List*_tmp1D5;if(((_tmp1D2.f1).PlaceL).tag == 1){if(((_tmp1D2.f2).ReachableFL).tag == 2){_LL181: _tmp1D3=((_tmp1D2.f1).PlaceL).val;_tmp1D4=(((_tmp1D2.f2).ReachableFL).val).f1;_tmp1D5=(((_tmp1D2.f2).ReachableFL).val).f2;_LL182:
# 1132
 _tmp1D5=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp1D5,_tmp1AA);
_tmp1CD=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp160,_tmp1AA->loc,_tmp1D4,env->all_changed,_tmp1D3,_tmp160->unknown_all),_tmp1D5);
# 1137
goto _LL180;}else{goto _LL183;}}else{_LL183: _LL184:
 goto _LL180;}_LL180:;}{
# 1140
struct _tuple16 _tmp73A;return(_tmp73A.f1=_tmp1CD,((_tmp73A.f2=_tmp160->unknown_all,_tmp73A)));};};}case 3: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f2 != 0){_LL142: _tmp1A8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL143:
# 1143
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp73D;void*_tmp73C;(_tmp73C=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp73D="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp73D,sizeof(char),62))),_tag_dyneither(_tmp73C,sizeof(void*),0)));}{
struct _tuple16 _tmp73E;return(_tmp73E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp73E.f2=_tmp160->unknown_all,_tmp73E)));};}{
# 1147
struct Cyc_List_List _tmp73F;struct Cyc_List_List _tmp1DC=(_tmp73F.hd=_tmp1A9,((_tmp73F.tl=0,_tmp73F)));
struct Cyc_List_List _tmp740;struct Cyc_List_List _tmp1DD=(_tmp740.hd=_tmp1A8,((_tmp740.tl=& _tmp1DC,_tmp740)));
struct _tuple16 _tmp1DE=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp1DD);union Cyc_CfFlowInfo_FlowInfo _tmp1E0;struct _tuple16 _tmp1DF=_tmp1DE;_tmp1E0=_tmp1DF.f1;{
struct _tuple18 _tmp1E1=Cyc_NewControlFlow_anal_Lexp(env,_tmp1E0,0,0,_tmp1A8);union Cyc_CfFlowInfo_AbsLVal _tmp1E3;struct _tuple18 _tmp1E2=_tmp1E1;_tmp1E3=_tmp1E2.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp1E4=_tmp1E0;struct Cyc_Dict_Dict _tmp1E5;struct Cyc_List_List*_tmp1E6;if((_tmp1E4.ReachableFL).tag == 2){_LL186: _tmp1E5=((_tmp1E4.ReachableFL).val).f1;_tmp1E6=((_tmp1E4.ReachableFL).val).f2;_LL187:
# 1153
{union Cyc_CfFlowInfo_AbsLVal _tmp1E7=_tmp1E3;struct Cyc_CfFlowInfo_Place*_tmp1E8;if((_tmp1E7.PlaceL).tag == 1){_LL18B: _tmp1E8=(_tmp1E7.PlaceL).val;_LL18C:
# 1157
 _tmp1E6=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp1E6,_tmp1A8);
_tmp1E5=Cyc_CfFlowInfo_assign_place(_tmp160,_tmp1A8->loc,_tmp1E5,env->all_changed,_tmp1E8,_tmp160->unknown_all);
# 1160
_tmp1E0=Cyc_CfFlowInfo_ReachableFL(_tmp1E5,_tmp1E6);
# 1164
goto _LL18A;}else{_LL18D: _LL18E:
# 1167
 goto _LL18A;}_LL18A:;}
# 1169
goto _LL185;}else{_LL188: _LL189:
 goto _LL185;}_LL185:;}{
# 1172
struct _tuple16 _tmp741;return(_tmp741.f1=_tmp1E0,((_tmp741.f2=_tmp160->unknown_all,_tmp741)));};};};}else{_LL144: _tmp1A6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL145:
# 1176
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp744;void*_tmp743;(_tmp743=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp744="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp744,sizeof(char),62))),_tag_dyneither(_tmp743,sizeof(void*),0)));}{
struct _tuple16 _tmp745;return(_tmp745.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp745.f2=_tmp160->unknown_all,_tmp745)));};}{
# 1180
struct Cyc_Dict_Dict*_tmp1EF=env->all_changed;
# 1182
{struct Cyc_Absyn_Exp*_tmp746[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp746[1]=_tmp1A7,((_tmp746[0]=_tmp1A6,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp746,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp747;env->all_changed=((_tmp747=_region_malloc(env->r,sizeof(*_tmp747)),((_tmp747[0]=_tmp160->mt_place_set,_tmp747))));}{
struct _tuple18 _tmp1F2=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1A6);union Cyc_CfFlowInfo_FlowInfo _tmp1F4;union Cyc_CfFlowInfo_AbsLVal _tmp1F5;struct _tuple18 _tmp1F3=_tmp1F2;_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;{
struct Cyc_Dict_Dict _tmp1F6=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp748;env->all_changed=((_tmp748=_region_malloc(env->r,sizeof(*_tmp748)),((_tmp748[0]=_tmp160->mt_place_set,_tmp748))));}{
struct _tuple16 _tmp1F8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A7);union Cyc_CfFlowInfo_FlowInfo _tmp1FA;void*_tmp1FB;struct _tuple16 _tmp1F9=_tmp1F8;_tmp1FA=_tmp1F9.f1;_tmp1FB=_tmp1F9.f2;{
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
inflow=_tmp1FE;};};};};}};}case 8: _LL146: _tmp1A4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL147: {
# 1211
struct _tuple16 _tmp1FF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1A4);union Cyc_CfFlowInfo_FlowInfo _tmp201;void*_tmp202;struct _tuple16 _tmp200=_tmp1FF;_tmp201=_tmp200.f1;_tmp202=_tmp200.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp201,_tmp1A5);}case 10: _LL148: _tmp1A3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL149: {
# 1215
struct _tuple16 _tmp203=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1A3);union Cyc_CfFlowInfo_FlowInfo _tmp205;void*_tmp206;struct _tuple16 _tmp204=_tmp203;_tmp205=_tmp204.f1;_tmp206=_tmp204.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1A3->loc,_tmp205,_tmp206);{
struct _tuple16 _tmp749;return(_tmp749.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp749.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all),_tmp749)));};}case 9: _LL14A: _tmp1A1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp1A2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL14B: {
# 1220
struct _RegionHandle _tmp208=_new_region("temp");struct _RegionHandle*temp=& _tmp208;_push_region(temp);
{struct Cyc_List_List*_tmp209=_tmp1A2;
struct Cyc_List_List*_tmp74A;struct _tuple23 _tmp20A=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp74A=_region_malloc(temp,sizeof(*_tmp74A)),((_tmp74A->hd=_tmp1A1,((_tmp74A->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1A2),_tmp74A)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp20C;struct Cyc_List_List*_tmp20D;struct _tuple23 _tmp20B=_tmp20A;_tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;{
# 1224
union Cyc_CfFlowInfo_FlowInfo _tmp20E=Cyc_NewControlFlow_use_Rval(env,_tmp1A1->loc,_tmp20C,(void*)((struct Cyc_List_List*)_check_null(_tmp20D))->hd);
_tmp20D=_tmp20D->tl;{
# 1227
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp20F=Cyc_Tcutil_compress((void*)_check_null(_tmp1A1->topt));void*_tmp210=_tmp20F;void*_tmp211;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->tag == 5){_LL190: _tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->f1).elt_typ;_LL191:
# 1234
{void*_tmp212=Cyc_Tcutil_compress(_tmp211);void*_tmp213=_tmp212;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp215;struct Cyc_List_List*_tmp216;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->tag == 9){_LL195: _tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->f1).attributes;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->f1).requires_relns;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213)->f1).ensures_relns;_LL196:
# 1236
 requires=_tmp215;
ensures=_tmp216;
for(0;_tmp214 != 0;_tmp214=_tmp214->tl){
# 1240
void*_tmp217=(void*)_tmp214->hd;void*_tmp218=_tmp217;int _tmp219;int _tmp21A;int _tmp21B;switch(*((int*)_tmp218)){case 20: _LL19A: _tmp21B=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp218)->f1;_LL19B:
# 1242
{struct Cyc_List_List*_tmp74B;init_params=((_tmp74B=_region_malloc(temp,sizeof(*_tmp74B)),((_tmp74B->hd=(void*)_tmp21B,((_tmp74B->tl=init_params,_tmp74B))))));}goto _LL199;case 21: _LL19C: _tmp21A=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp218)->f1;_LL19D:
# 1244
{struct Cyc_List_List*_tmp74C;nolive_unique_params=((_tmp74C=_region_malloc(temp,sizeof(*_tmp74C)),((_tmp74C->hd=(void*)_tmp21A,((_tmp74C->tl=nolive_unique_params,_tmp74C))))));}
goto _LL199;case 22: _LL19E: _tmp219=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp218)->f1;_LL19F:
# 1248
{struct Cyc_List_List*_tmp74D;noconsume_params=((_tmp74D=_region_malloc(temp,sizeof(*_tmp74D)),((_tmp74D->hd=(void*)_tmp219,((_tmp74D->tl=noconsume_params,_tmp74D))))));}
goto _LL199;default: _LL1A0: _LL1A1:
 goto _LL199;}_LL199:;}
# 1252
goto _LL194;}else{_LL197: _LL198: {
const char*_tmp750;void*_tmp74F;(_tmp74F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp750="anal_Rexp: bad function type",_tag_dyneither(_tmp750,sizeof(char),29))),_tag_dyneither(_tmp74F,sizeof(void*),0)));}}_LL194:;}
# 1255
goto _LL18F;}else{_LL192: _LL193: {
const char*_tmp753;void*_tmp752;(_tmp752=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp753="anal_Rexp: bad function type",_tag_dyneither(_tmp753,sizeof(char),29))),_tag_dyneither(_tmp752,sizeof(void*),0)));}}_LL18F:;}
# 1258
{int i=1;for(0;_tmp20D != 0;(((_tmp20D=_tmp20D->tl,_tmp1A2=((struct Cyc_List_List*)_check_null(_tmp1A2))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp223=_tmp20C;struct Cyc_Dict_Dict _tmp224;if((_tmp223.BottomFL).tag == 1){_LL1A3: _LL1A4:
 goto _LL1A2;}else{_LL1A5: _tmp224=((_tmp223.ReachableFL).val).f1;_LL1A6:
# 1263
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp224,(void*)_tmp20D->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp756;void*_tmp755;(_tmp755=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A2))->hd)->loc,((_tmp756="expression may not be initialized",_tag_dyneither(_tmp756,sizeof(char),34))),_tag_dyneither(_tmp755,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp227=_tmp20E;struct Cyc_Dict_Dict _tmp228;struct Cyc_List_List*_tmp229;if((_tmp227.BottomFL).tag == 1){_LL1A8: _LL1A9:
 goto _LL1A7;}else{_LL1AA: _tmp228=((_tmp227.ReachableFL).val).f1;_tmp229=((_tmp227.ReachableFL).val).f2;_LL1AB: {
# 1270
struct Cyc_Dict_Dict _tmp22A=Cyc_CfFlowInfo_escape_deref(_tmp160,_tmp228,env->all_changed,(void*)_tmp20D->hd);
{void*_tmp22B=(void*)_tmp20D->hd;void*_tmp22C=_tmp22B;struct Cyc_CfFlowInfo_Place*_tmp22D;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp22C)->tag == 5){_LL1AD: _tmp22D=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp22C)->f1;_LL1AE:
# 1273
{void*_tmp22E=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1A2))->hd)->topt));void*_tmp22F=_tmp22E;void*_tmp230;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F)->tag == 5){_LL1B2: _tmp230=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F)->f1).elt_typ;_LL1B3:
# 1275
 _tmp22A=Cyc_CfFlowInfo_assign_place(_tmp160,((struct Cyc_Absyn_Exp*)_tmp1A2->hd)->loc,_tmp22A,env->all_changed,_tmp22D,
# 1277
Cyc_CfFlowInfo_typ_to_absrval(_tmp160,_tmp230,0,_tmp160->esc_all));
# 1280
goto _LL1B1;}else{_LL1B4: _LL1B5: {
const char*_tmp759;void*_tmp758;(_tmp758=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp759="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp759,sizeof(char),39))),_tag_dyneither(_tmp758,sizeof(void*),0)));}}_LL1B1:;}
# 1283
goto _LL1AC;}else{_LL1AF: _LL1B0:
 goto _LL1AC;}_LL1AC:;}
# 1286
_tmp20E=Cyc_CfFlowInfo_ReachableFL(_tmp22A,_tmp229);
goto _LL1A7;}}_LL1A7:;}
# 1289
goto _LL1A2;}_LL1A2:;}else{
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
struct _tuple16 _tmp75A;struct _tuple16 _tmp234=(_tmp75A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp75A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all),_tmp75A)));_npop_handler(0);return _tmp234;}else{
# 1320
struct _tuple16 _tmp75B;struct _tuple16 _tmp236=(_tmp75B.f1=_tmp20E,((_tmp75B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all),_tmp75B)));_npop_handler(0);return _tmp236;}};};}
# 1221
;_pop_region(temp);}case 33: _LL14C: _tmp19C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).is_calloc;_tmp19D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).rgn;_tmp19E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).elt_type;_tmp19F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).num_elts;_tmp1A0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp178)->f1).fat_result;_LL14D: {
# 1323
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp75E;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp75D;void*root=(void*)((_tmp75D=_region_malloc(_tmp160->r,sizeof(*_tmp75D)),((_tmp75D[0]=((_tmp75E.tag=1,((_tmp75E.f1=_tmp19F,((_tmp75E.f2=(void*)_check_null(e->topt),_tmp75E)))))),_tmp75D))));
struct Cyc_CfFlowInfo_Place*_tmp75F;struct Cyc_CfFlowInfo_Place*place=(_tmp75F=_region_malloc(_tmp160->r,sizeof(*_tmp75F)),((_tmp75F->root=root,((_tmp75F->fields=0,_tmp75F)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp762;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp761;void*rval=(void*)((_tmp761=_region_malloc(_tmp160->r,sizeof(*_tmp761)),((_tmp761[0]=((_tmp762.tag=5,((_tmp762.f1=place,_tmp762)))),_tmp761))));
void*place_val;
if(_tmp1A0)place_val=_tmp160->notzeroall;else{
if(_tmp19C)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,*((void**)_check_null(_tmp19E)),0,_tmp160->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,*((void**)_check_null(_tmp19E)),0,_tmp160->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp19D != 0){
struct _RegionHandle _tmp238=_new_region("temp");struct _RegionHandle*temp=& _tmp238;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp763[2];struct _tuple23 _tmp239=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp763[1]=_tmp19F,((_tmp763[0]=_tmp19D,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp763,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1334
union Cyc_CfFlowInfo_FlowInfo _tmp23B;struct Cyc_List_List*_tmp23C;struct _tuple23 _tmp23A=_tmp239;_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;
# 1336
outflow=_tmp23B;}
# 1334
;_pop_region(temp);}else{
# 1339
struct _tuple16 _tmp23E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp19F);union Cyc_CfFlowInfo_FlowInfo _tmp240;struct _tuple16 _tmp23F=_tmp23E;_tmp240=_tmp23F.f1;
outflow=_tmp240;}{
# 1342
union Cyc_CfFlowInfo_FlowInfo _tmp241=outflow;struct Cyc_Dict_Dict _tmp242;struct Cyc_List_List*_tmp243;if((_tmp241.BottomFL).tag == 1){_LL1B7: _LL1B8: {
struct _tuple16 _tmp764;return(_tmp764.f1=outflow,((_tmp764.f2=rval,_tmp764)));}}else{_LL1B9: _tmp242=((_tmp241.ReachableFL).val).f1;_tmp243=((_tmp241.ReachableFL).val).f2;_LL1BA: {
# 1345
struct _tuple16 _tmp765;return(_tmp765.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp242,root,place_val),_tmp243),((_tmp765.f2=rval,_tmp765)));}}_LL1B6:;};};}case 34: _LL14E: _tmp19A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp19B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL14F: {
# 1349
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1355
struct _RegionHandle _tmp24B=_new_region("temp");struct _RegionHandle*temp=& _tmp24B;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp766[2];struct _tuple23 _tmp24C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp766[1]=_tmp19B,((_tmp766[0]=_tmp19A,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp766,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1356
union Cyc_CfFlowInfo_FlowInfo _tmp24E;struct Cyc_List_List*_tmp24F;struct _tuple23 _tmp24D=_tmp24C;_tmp24E=_tmp24D.f1;_tmp24F=_tmp24D.f2;
# 1358
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp24F))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp24F->tl))->hd;
outflow=_tmp24E;}{
# 1363
struct _tuple18 _tmp251=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp19A);union Cyc_CfFlowInfo_AbsLVal _tmp253;struct _tuple18 _tmp252=_tmp251;_tmp253=_tmp252.f2;{
struct _tuple18 _tmp254=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp19B);union Cyc_CfFlowInfo_AbsLVal _tmp256;struct _tuple18 _tmp255=_tmp254;_tmp256=_tmp255.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp257=outflow;struct Cyc_Dict_Dict _tmp258;struct Cyc_List_List*_tmp259;if((_tmp257.ReachableFL).tag == 2){_LL1BC: _tmp258=((_tmp257.ReachableFL).val).f1;_tmp259=((_tmp257.ReachableFL).val).f2;_LL1BD:
# 1367
{union Cyc_CfFlowInfo_AbsLVal _tmp25A=_tmp253;struct Cyc_CfFlowInfo_Place*_tmp25B;if((_tmp25A.PlaceL).tag == 1){_LL1C1: _tmp25B=(_tmp25A.PlaceL).val;_LL1C2:
# 1369
 _tmp258=Cyc_CfFlowInfo_assign_place(_tmp160,_tmp19A->loc,_tmp258,env->all_changed,_tmp25B,right_rval);
# 1371
goto _LL1C0;}else{_LL1C3: _LL1C4:
# 1376
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp258,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp769;void*_tmp768;(_tmp768=0,Cyc_Tcutil_terr(_tmp19B->loc,((_tmp769="expression may not be fully initialized",_tag_dyneither(_tmp769,sizeof(char),40))),_tag_dyneither(_tmp768,sizeof(void*),0)));}
goto _LL1C0;}_LL1C0:;}
# 1380
{union Cyc_CfFlowInfo_AbsLVal _tmp25E=_tmp256;struct Cyc_CfFlowInfo_Place*_tmp25F;if((_tmp25E.PlaceL).tag == 1){_LL1C6: _tmp25F=(_tmp25E.PlaceL).val;_LL1C7:
# 1382
 _tmp258=Cyc_CfFlowInfo_assign_place(_tmp160,_tmp19B->loc,_tmp258,env->all_changed,_tmp25F,left_rval);
# 1384
goto _LL1C5;}else{_LL1C8: _LL1C9:
# 1386
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp258,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp76C;void*_tmp76B;(_tmp76B=0,Cyc_Tcutil_terr(_tmp19A->loc,((_tmp76C="expression may not be fully initialized",_tag_dyneither(_tmp76C,sizeof(char),40))),_tag_dyneither(_tmp76B,sizeof(void*),0)));}
goto _LL1C5;}_LL1C5:;}
# 1391
_tmp259=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp259,_tmp19A);
_tmp259=Cyc_Relations_reln_kill_exp(_tmp160->r,_tmp259,_tmp19B);
# 1394
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp258,_tmp259);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL1BB;}else{_LL1BE: _LL1BF:
 goto _LL1BB;}_LL1BB:;}{
# 1401
struct _tuple16 _tmp76D;struct _tuple16 _tmp263=(_tmp76D.f1=outflow,((_tmp76D.f2=_tmp160->unknown_all,_tmp76D)));_npop_handler(0);return _tmp263;};};};
# 1355
;_pop_region(temp);}case 15: _LL150: _tmp198=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp199=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL151: {
# 1404
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp770;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp76F;void*root=(void*)((_tmp76F=_region_malloc(_tmp160->r,sizeof(*_tmp76F)),((_tmp76F[0]=((_tmp770.tag=1,((_tmp770.f1=_tmp199,((_tmp770.f2=(void*)_check_null(e->topt),_tmp770)))))),_tmp76F))));
struct Cyc_CfFlowInfo_Place*_tmp771;struct Cyc_CfFlowInfo_Place*place=(_tmp771=_region_malloc(_tmp160->r,sizeof(*_tmp771)),((_tmp771->root=root,((_tmp771->fields=0,_tmp771)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp774;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp773;void*rval=(void*)((_tmp773=_region_malloc(_tmp160->r,sizeof(*_tmp773)),((_tmp773[0]=((_tmp774.tag=5,((_tmp774.f1=place,_tmp774)))),_tmp773))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp198 != 0){
struct _RegionHandle _tmp264=_new_region("temp");struct _RegionHandle*temp=& _tmp264;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp775[2];struct _tuple23 _tmp265=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp775[1]=_tmp199,((_tmp775[0]=_tmp198,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp775,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1412
union Cyc_CfFlowInfo_FlowInfo _tmp267;struct Cyc_List_List*_tmp268;struct _tuple23 _tmp266=_tmp265;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;
# 1414
outflow=_tmp267;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp268))->tl))->hd;}
# 1412
;_pop_region(temp);}else{
# 1418
struct _tuple16 _tmp26A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp199);union Cyc_CfFlowInfo_FlowInfo _tmp26C;void*_tmp26D;struct _tuple16 _tmp26B=_tmp26A;_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;
outflow=_tmp26C;
place_val=_tmp26D;}{
# 1422
union Cyc_CfFlowInfo_FlowInfo _tmp26E=outflow;struct Cyc_Dict_Dict _tmp26F;struct Cyc_List_List*_tmp270;if((_tmp26E.BottomFL).tag == 1){_LL1CB: _LL1CC: {
struct _tuple16 _tmp776;return(_tmp776.f1=outflow,((_tmp776.f2=rval,_tmp776)));}}else{_LL1CD: _tmp26F=((_tmp26E.ReachableFL).val).f1;_tmp270=((_tmp26E.ReachableFL).val).f2;_LL1CE: {
# 1425
struct _tuple16 _tmp777;return(_tmp777.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp26F,root,place_val),_tmp270),((_tmp777.f2=rval,_tmp777)));}}_LL1CA:;};};}case 14: _LL152: _tmp197=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL153: {
# 1429
struct _tuple18 _tmp278=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp197);union Cyc_CfFlowInfo_FlowInfo _tmp27A;union Cyc_CfFlowInfo_AbsLVal _tmp27B;struct _tuple18 _tmp279=_tmp278;_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp27C=_tmp27B;struct Cyc_CfFlowInfo_Place*_tmp27D;if((_tmp27C.UnknownL).tag == 2){_LL1D0: _LL1D1: {
struct _tuple16 _tmp778;return(_tmp778.f1=_tmp27A,((_tmp778.f2=_tmp160->notzeroall,_tmp778)));}}else{_LL1D2: _tmp27D=(_tmp27C.PlaceL).val;_LL1D3: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp77E;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp77D;struct _tuple16 _tmp77C;return(_tmp77C.f1=_tmp27A,((_tmp77C.f2=(void*)((_tmp77E=_region_malloc(env->r,sizeof(*_tmp77E)),((_tmp77E[0]=((_tmp77D.tag=5,((_tmp77D.f1=_tmp27D,_tmp77D)))),_tmp77E)))),_tmp77C)));}}_LL1CF:;};}case 19: _LL154: _tmp196=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL155: {
# 1436
struct _tuple16 _tmp282=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp196);union Cyc_CfFlowInfo_FlowInfo _tmp284;void*_tmp285;struct _tuple16 _tmp283=_tmp282;_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp284,_tmp196,_tmp285);}case 20: _LL156: _tmp194=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp195=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL157: {
# 1440
struct _tuple16 _tmp286=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp194);union Cyc_CfFlowInfo_FlowInfo _tmp288;void*_tmp289;struct _tuple16 _tmp287=_tmp286;_tmp288=_tmp287.f1;_tmp289=_tmp287.f2;
if(_tmp194->topt == 0){
{const char*_tmp782;void*_tmp781[1];struct Cyc_String_pa_PrintArg_struct _tmp780;(_tmp780.tag=0,((_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp781[0]=& _tmp780,Cyc_fprintf(Cyc_stderr,((_tmp782="aggrmember exp %s\n",_tag_dyneither(_tmp782,sizeof(char),19))),_tag_dyneither(_tmp781,sizeof(void*),1)))))));}{
const char*_tmp786;void*_tmp785[1];struct Cyc_String_pa_PrintArg_struct _tmp784;(_tmp784.tag=0,((_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp194)),((_tmp785[0]=& _tmp784,Cyc_fprintf(Cyc_stderr,((_tmp786="oops! %s.topt = null!\n",_tag_dyneither(_tmp786,sizeof(char),23))),_tag_dyneither(_tmp785,sizeof(void*),1)))))));};}{
# 1445
void*_tmp290=(void*)_check_null(_tmp194->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp290)){
# 1448
struct _tuple16 _tmp787;return(_tmp787.f1=_tmp288,((_tmp787.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all),_tmp787)));}
# 1450
if(Cyc_Absyn_is_require_union_type(_tmp290))
Cyc_NewControlFlow_check_union_requires(_tmp194->loc,_tmp160->r,_tmp290,_tmp195,_tmp288);{
# 1453
struct _tuple15 _tmp292=Cyc_CfFlowInfo_unname_rval(_tmp160->r,_tmp289);void*_tmp294;struct _tuple15 _tmp293=_tmp292;_tmp294=_tmp293.f1;{
void*_tmp295=_tmp294;int _tmp296;int _tmp297;struct _dyneither_ptr _tmp298;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->tag == 6){_LL1D5: _tmp296=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->f1).is_union;_tmp297=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->f1).fieldnum;_tmp298=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp295)->f2;_LL1D6: {
# 1456
int _tmp299=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp194->topt),_tmp195);
if((_tmp296  && _tmp297 != - 1) && _tmp297 != _tmp299){
struct _tuple16 _tmp788;return(_tmp788.f1=_tmp288,((_tmp788.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),1,_tmp160->unknown_none),_tmp788)));}{
struct _tuple16 _tmp789;return(_tmp789.f1=_tmp288,((_tmp789.f2=*((void**)_check_dyneither_subscript(_tmp298,sizeof(void*),_tmp299)),_tmp789)));};}}else{_LL1D7: _LL1D8:
# 1461
{const char*_tmp78C;void*_tmp78B;(_tmp78B=0,Cyc_fprintf(Cyc_stderr,((_tmp78C="the bad rexp is :",_tag_dyneither(_tmp78C,sizeof(char),18))),_tag_dyneither(_tmp78B,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp294);
{const char*_tmp78F;void*_tmp78E;(_tmp78E=0,Cyc_fprintf(Cyc_stderr,((_tmp78F="\n",_tag_dyneither(_tmp78F,sizeof(char),2))),_tag_dyneither(_tmp78E,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp79C;const char*_tmp79B;void*_tmp79A[1];struct Cyc_String_pa_PrintArg_struct _tmp799;struct Cyc_Core_Impossible_exn_struct*_tmp798;(int)_throw((void*)((_tmp798=_cycalloc(sizeof(*_tmp798)),((_tmp798[0]=((_tmp79C.tag=Cyc_Core_Impossible,((_tmp79C.f1=(struct _dyneither_ptr)((_tmp799.tag=0,((_tmp799.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp79A[0]=& _tmp799,Cyc_aprintf(((_tmp79B="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp79B,sizeof(char),26))),_tag_dyneither(_tmp79A,sizeof(void*),1)))))))),_tmp79C)))),_tmp798)))));};}_LL1D4:;};};};}case 37: _LL158: _tmp192=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp193=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL159: {
# 1471
struct _tuple16 _tmp2A5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp192);union Cyc_CfFlowInfo_FlowInfo _tmp2A7;void*_tmp2A8;struct _tuple16 _tmp2A6=_tmp2A5;_tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;
# 1473
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp192->topt))){
struct _tuple16 _tmp79D;return(_tmp79D.f1=_tmp2A7,((_tmp79D.f2=_tmp160->unknown_all,_tmp79D)));}{
struct _tuple15 _tmp2AA=Cyc_CfFlowInfo_unname_rval(_tmp160->r,_tmp2A8);void*_tmp2AC;struct _tuple15 _tmp2AB=_tmp2AA;_tmp2AC=_tmp2AB.f1;{
void*_tmp2AD=_tmp2AC;int _tmp2AE;int _tmp2AF;struct _dyneither_ptr _tmp2B0;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->tag == 6){_LL1DA: _tmp2AE=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->f1).is_union;_tmp2AF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->f1).fieldnum;_tmp2B0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2AD)->f2;_LL1DB: {
# 1478
int _tmp2B1=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp192->topt),_tmp193);
if(_tmp2AE  && _tmp2AF != - 1){
if(_tmp2AF != _tmp2B1){
struct _tuple16 _tmp79E;return(_tmp79E.f1=_tmp2A7,((_tmp79E.f2=_tmp160->zero,_tmp79E)));}else{
# 1483
struct _tuple16 _tmp79F;return(_tmp79F.f1=_tmp2A7,((_tmp79F.f2=_tmp160->notzeroall,_tmp79F)));}}else{
# 1485
struct _tuple16 _tmp7A0;return(_tmp7A0.f1=_tmp2A7,((_tmp7A0.f2=_tmp160->unknown_all,_tmp7A0)));}}}else{_LL1DC: _LL1DD: {
# 1487
struct Cyc_Core_Impossible_exn_struct _tmp7AD;const char*_tmp7AC;void*_tmp7AB[1];struct Cyc_String_pa_PrintArg_struct _tmp7AA;struct Cyc_Core_Impossible_exn_struct*_tmp7A9;(int)_throw((void*)((_tmp7A9=_cycalloc(sizeof(*_tmp7A9)),((_tmp7A9[0]=((_tmp7AD.tag=Cyc_Core_Impossible,((_tmp7AD.f1=(struct _dyneither_ptr)((_tmp7AA.tag=0,((_tmp7AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7AB[0]=& _tmp7AA,Cyc_aprintf(((_tmp7AC="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp7AC,sizeof(char),26))),_tag_dyneither(_tmp7AB,sizeof(void*),1)))))))),_tmp7AD)))),_tmp7A9)))));}}_LL1D9:;};};}case 21: _LL15A: _tmp190=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp191=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL15B: {
# 1492
struct _tuple16 _tmp2BA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp190);union Cyc_CfFlowInfo_FlowInfo _tmp2BC;void*_tmp2BD;struct _tuple16 _tmp2BB=_tmp2BA;_tmp2BC=_tmp2BB.f1;_tmp2BD=_tmp2BB.f2;{
# 1495
struct _tuple16 _tmp2BE=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2BC,_tmp190,_tmp2BD);union Cyc_CfFlowInfo_FlowInfo _tmp2C0;void*_tmp2C1;struct _tuple16 _tmp2BF=_tmp2BE;_tmp2C0=_tmp2BF.f1;_tmp2C1=_tmp2BF.f2;{
# 1498
void*_tmp2C2=Cyc_Tcutil_compress((void*)_check_null(_tmp190->topt));void*_tmp2C3=_tmp2C2;void*_tmp2C4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C3)->tag == 5){_LL1DF: _tmp2C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C3)->f1).elt_typ;_LL1E0:
# 1500
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2C4)){
# 1502
struct _tuple16 _tmp7AE;return(_tmp7AE.f1=_tmp2C0,((_tmp7AE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all),_tmp7AE)));}
# 1504
if(Cyc_Absyn_is_require_union_type(_tmp2C4))
Cyc_NewControlFlow_check_union_requires(_tmp190->loc,_tmp160->r,_tmp2C4,_tmp191,_tmp2BC);{
# 1507
struct _tuple15 _tmp2C6=Cyc_CfFlowInfo_unname_rval(_tmp160->r,_tmp2C1);void*_tmp2C8;struct _tuple15 _tmp2C7=_tmp2C6;_tmp2C8=_tmp2C7.f1;{
void*_tmp2C9=_tmp2C8;int _tmp2CA;int _tmp2CB;struct _dyneither_ptr _tmp2CC;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->tag == 6){_LL1E4: _tmp2CA=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->f1).is_union;_tmp2CB=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->f1).fieldnum;_tmp2CC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C9)->f2;_LL1E5: {
# 1510
int _tmp2CD=Cyc_CfFlowInfo_get_field_index(_tmp2C4,_tmp191);
if((_tmp2CA  && _tmp2CB != - 1) && _tmp2CB != _tmp2CD){
struct _tuple16 _tmp7AF;return(_tmp7AF.f1=_tmp2C0,((_tmp7AF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),1,_tmp160->unknown_none),_tmp7AF)));}{
struct _tuple16 _tmp7B0;return(_tmp7B0.f1=_tmp2C0,((_tmp7B0.f2=*((void**)_check_dyneither_subscript(_tmp2CC,sizeof(void*),_tmp2CD)),_tmp7B0)));};}}else{_LL1E6: _LL1E7: {
struct Cyc_Core_Impossible_exn_struct _tmp7B6;const char*_tmp7B5;struct Cyc_Core_Impossible_exn_struct*_tmp7B4;(int)_throw((void*)((_tmp7B4=_cycalloc(sizeof(*_tmp7B4)),((_tmp7B4[0]=((_tmp7B6.tag=Cyc_Core_Impossible,((_tmp7B6.f1=((_tmp7B5="anal_Rexp: AggrArrow",_tag_dyneither(_tmp7B5,sizeof(char),21))),_tmp7B6)))),_tmp7B4)))));}}_LL1E3:;};};}else{_LL1E1: _LL1E2: {
# 1516
struct Cyc_Core_Impossible_exn_struct _tmp7BC;const char*_tmp7BB;struct Cyc_Core_Impossible_exn_struct*_tmp7BA;(int)_throw((void*)((_tmp7BA=_cycalloc(sizeof(*_tmp7BA)),((_tmp7BA[0]=((_tmp7BC.tag=Cyc_Core_Impossible,((_tmp7BC.f1=((_tmp7BB="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7BB,sizeof(char),25))),_tmp7BC)))),_tmp7BA)))));}}_LL1DE:;};};}case 5: _LL15C: _tmp18D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp18E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_tmp18F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL15D: {
# 1520
struct _tuple19 _tmp2D6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp18D);union Cyc_CfFlowInfo_FlowInfo _tmp2D8;union Cyc_CfFlowInfo_FlowInfo _tmp2D9;struct _tuple19 _tmp2D7=_tmp2D6;_tmp2D8=_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;{
struct _tuple16 _tmp2DA=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D8,_tmp18E);
struct _tuple16 _tmp2DB=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2D9,_tmp18F);
# 1524
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp160,env->all_changed,_tmp2DA,_tmp2DB);};}case 6: _LL15E: _tmp18B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp18C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL15F: {
# 1527
struct _tuple19 _tmp2DC=Cyc_NewControlFlow_anal_test(env,inflow,_tmp18B);union Cyc_CfFlowInfo_FlowInfo _tmp2DE;union Cyc_CfFlowInfo_FlowInfo _tmp2DF;struct _tuple19 _tmp2DD=_tmp2DC;_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;{
struct _tuple16 _tmp2E0=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2DE,_tmp18C);union Cyc_CfFlowInfo_FlowInfo _tmp2E2;void*_tmp2E3;struct _tuple16 _tmp2E1=_tmp2E0;_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;{
struct _tuple16 _tmp7BD;struct _tuple16 _tmp2E4=(_tmp7BD.f1=_tmp2E2,((_tmp7BD.f2=_tmp2E3,_tmp7BD)));
struct _tuple16 _tmp7BE;struct _tuple16 _tmp2E5=(_tmp7BE.f1=_tmp2DF,((_tmp7BE.f2=_tmp160->zero,_tmp7BE)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp160,env->all_changed,_tmp2E4,_tmp2E5);};};}case 7: _LL160: _tmp189=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp18A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL161: {
# 1534
struct _tuple19 _tmp2E8=Cyc_NewControlFlow_anal_test(env,inflow,_tmp189);union Cyc_CfFlowInfo_FlowInfo _tmp2EA;union Cyc_CfFlowInfo_FlowInfo _tmp2EB;struct _tuple19 _tmp2E9=_tmp2E8;_tmp2EA=_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;{
struct _tuple16 _tmp2EC=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2EB,_tmp18A);union Cyc_CfFlowInfo_FlowInfo _tmp2EE;void*_tmp2EF;struct _tuple16 _tmp2ED=_tmp2EC;_tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;{
struct _tuple16 _tmp7BF;struct _tuple16 _tmp2F0=(_tmp7BF.f1=_tmp2EE,((_tmp7BF.f2=_tmp2EF,_tmp7BF)));
struct _tuple16 _tmp7C0;struct _tuple16 _tmp2F1=(_tmp7C0.f1=_tmp2EA,((_tmp7C0.f2=_tmp160->notzeroall,_tmp7C0)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp160,env->all_changed,_tmp2F0,_tmp2F1);};};}case 22: _LL162: _tmp187=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp188=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL163: {
# 1541
struct _RegionHandle _tmp2F4=_new_region("temp");struct _RegionHandle*temp=& _tmp2F4;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp7C1[2];struct _tuple23 _tmp2F5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp7C1[1]=_tmp188,((_tmp7C1[0]=_tmp187,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp7C1,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1542
union Cyc_CfFlowInfo_FlowInfo _tmp2F7;struct Cyc_List_List*_tmp2F8;struct _tuple23 _tmp2F6=_tmp2F5;_tmp2F7=_tmp2F6.f1;_tmp2F8=_tmp2F6.f2;{
# 1546
union Cyc_CfFlowInfo_FlowInfo _tmp2F9=_tmp2F7;
{union Cyc_CfFlowInfo_FlowInfo _tmp2FA=_tmp2F7;struct Cyc_Dict_Dict _tmp2FB;struct Cyc_List_List*_tmp2FC;if((_tmp2FA.ReachableFL).tag == 2){_LL1E9: _tmp2FB=((_tmp2FA.ReachableFL).val).f1;_tmp2FC=((_tmp2FA.ReachableFL).val).f2;_LL1EA:
# 1551
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2FB,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2F8))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7C4;void*_tmp7C3;(_tmp7C3=0,Cyc_CfFlowInfo_aerr(_tmp188->loc,((_tmp7C4="expression may not be initialized",_tag_dyneither(_tmp7C4,sizeof(char),34))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2FF=Cyc_NewControlFlow_add_subscript_reln(_tmp160->r,_tmp2FC,_tmp187,_tmp188);
if(_tmp2FC != _tmp2FF)
_tmp2F9=Cyc_CfFlowInfo_ReachableFL(_tmp2FB,_tmp2FF);
goto _LL1E8;};}else{_LL1EB: _LL1EC:
 goto _LL1E8;}_LL1E8:;}{
# 1559
void*_tmp300=Cyc_Tcutil_compress((void*)_check_null(_tmp187->topt));void*_tmp301=_tmp300;union Cyc_Absyn_Constraint*_tmp302;struct Cyc_List_List*_tmp303;switch(*((int*)_tmp301)){case 10: _LL1EE: _tmp303=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp301)->f1;_LL1EF: {
# 1561
struct _tuple15 _tmp304=Cyc_CfFlowInfo_unname_rval(_tmp160->r,(void*)((struct Cyc_List_List*)_check_null(_tmp2F8))->hd);void*_tmp306;struct _tuple15 _tmp305=_tmp304;_tmp306=_tmp305.f1;{
void*_tmp307=_tmp306;struct _dyneither_ptr _tmp308;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp307)->tag == 6){_LL1F5: _tmp308=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp307)->f2;_LL1F6: {
# 1564
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp188)).f1;
struct _tuple16 _tmp7C5;struct _tuple16 _tmp30A=(_tmp7C5.f1=_tmp2F9,((_tmp7C5.f2=*((void**)_check_dyneither_subscript(_tmp308,sizeof(void*),(int)i)),_tmp7C5)));_npop_handler(0);return _tmp30A;}}else{_LL1F7: _LL1F8: {
struct Cyc_Core_Impossible_exn_struct _tmp7CB;const char*_tmp7CA;struct Cyc_Core_Impossible_exn_struct*_tmp7C9;(int)_throw((void*)((_tmp7C9=_cycalloc(sizeof(*_tmp7C9)),((_tmp7C9[0]=((_tmp7CB.tag=Cyc_Core_Impossible,((_tmp7CB.f1=((_tmp7CA="anal_Rexp: Subscript",_tag_dyneither(_tmp7CA,sizeof(char),21))),_tmp7CB)))),_tmp7C9)))));}}_LL1F4:;};}case 5: _LL1F0: _tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).bounds;_LL1F1: {
# 1569
struct _tuple16 _tmp30E=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2F7,_tmp187,(void*)((struct Cyc_List_List*)_check_null(_tmp2F8))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp310;void*_tmp311;struct _tuple16 _tmp30F=_tmp30E;_tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp312=_tmp310;if((_tmp312.BottomFL).tag == 1){_LL1FA: _LL1FB: {
struct _tuple16 _tmp7CC;struct _tuple16 _tmp314=(_tmp7CC.f1=_tmp310,((_tmp7CC.f2=_tmp311,_tmp7CC)));_npop_handler(0);return _tmp314;}}else{_LL1FC: _LL1FD: {
struct _tuple16 _tmp7CD;struct _tuple16 _tmp316=(_tmp7CD.f1=_tmp2F9,((_tmp7CD.f2=_tmp311,_tmp7CD)));_npop_handler(0);return _tmp316;}}_LL1F9:;};}default: _LL1F2: _LL1F3: {
# 1574
struct Cyc_Core_Impossible_exn_struct _tmp7D3;const char*_tmp7D2;struct Cyc_Core_Impossible_exn_struct*_tmp7D1;(int)_throw((void*)((_tmp7D1=_cycalloc(sizeof(*_tmp7D1)),((_tmp7D1[0]=((_tmp7D3.tag=Cyc_Core_Impossible,((_tmp7D3.f1=((_tmp7D2="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7D2,sizeof(char),33))),_tmp7D3)))),_tmp7D1)))));}}_LL1ED:;};};}
# 1542
;_pop_region(temp);}case 23: _LL166: _tmp186=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL167: {
# 1581
struct _RegionHandle _tmp31B=_new_region("temp");struct _RegionHandle*temp=& _tmp31B;_push_region(temp);
{struct _tuple23 _tmp31C=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp186,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp31E;struct Cyc_List_List*_tmp31F;struct _tuple23 _tmp31D=_tmp31C;_tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;{
unsigned int _tmp7D9;unsigned int _tmp7D8;struct _dyneither_ptr _tmp7D7;void**_tmp7D6;unsigned int _tmp7D5;struct _dyneither_ptr aggrdict=
(_tmp7D5=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp186),((_tmp7D6=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp7D5)),((_tmp7D7=_tag_dyneither(_tmp7D6,sizeof(void*),_tmp7D5),((((_tmp7D8=_tmp7D5,_tmp7DA(& _tmp7D9,& _tmp7D8,& _tmp7D6,& _tmp31F))),_tmp7D7)))))));
# 1589
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7E4;struct Cyc_CfFlowInfo_UnionRInfo _tmp7E3;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7E2;struct _tuple16 _tmp7E1;struct _tuple16 _tmp324=(_tmp7E1.f1=_tmp31E,((_tmp7E1.f2=(void*)((_tmp7E4=_region_malloc(env->r,sizeof(*_tmp7E4)),((_tmp7E4[0]=((_tmp7E2.tag=6,((_tmp7E2.f1=((_tmp7E3.is_union=0,((_tmp7E3.fieldnum=- 1,_tmp7E3)))),((_tmp7E2.f2=aggrdict,_tmp7E2)))))),_tmp7E4)))),_tmp7E1)));_npop_handler(0);return _tmp324;};}
# 1582
;_pop_region(temp);}case 29: _LL168: _tmp185=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_LL169: {
# 1592
struct Cyc_List_List*fs;
{void*_tmp32A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp32B=_tmp32A;struct Cyc_List_List*_tmp32C;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32B)->tag == 12){_LL1FF: _tmp32C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32B)->f2;_LL200:
 fs=_tmp32C;goto _LL1FE;}else{_LL201: _LL202: {
struct Cyc_Core_Impossible_exn_struct _tmp7EA;const char*_tmp7E9;struct Cyc_Core_Impossible_exn_struct*_tmp7E8;(int)_throw((void*)((_tmp7E8=_cycalloc(sizeof(*_tmp7E8)),((_tmp7E8[0]=((_tmp7EA.tag=Cyc_Core_Impossible,((_tmp7EA.f1=((_tmp7E9="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp7E9,sizeof(char),35))),_tmp7EA)))),_tmp7E8)))));}}_LL1FE:;}
# 1597
_tmp182=_tmp185;_tmp183=Cyc_Absyn_StructA;_tmp184=fs;goto _LL16B;}case 28: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4)->impl != 0){_LL16A: _tmp182=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_tmp183=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4)->kind;_tmp184=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp178)->f4)->impl)->fields;_LL16B: {
# 1599
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp330=_new_region("temp");struct _RegionHandle*temp=& _tmp330;_push_region(temp);
{struct _tuple23 _tmp331=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp182),1,1);
# 1601
union Cyc_CfFlowInfo_FlowInfo _tmp333;struct Cyc_List_List*_tmp334;struct _tuple23 _tmp332=_tmp331;_tmp333=_tmp332.f1;_tmp334=_tmp332.f2;{
# 1603
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp160,_tmp184,_tmp183 == Cyc_Absyn_UnionA,_tmp160->unknown_all);
# 1606
int field_no=-1;
{int i=0;for(0;_tmp334 != 0;(((_tmp334=_tmp334->tl,_tmp182=_tmp182->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp182))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp335=(void*)ds->hd;void*_tmp336=_tmp335;struct _dyneither_ptr*_tmp337;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp336)->tag == 0){_LL204: _LL205: {
struct Cyc_Core_Impossible_exn_struct _tmp7F0;const char*_tmp7EF;struct Cyc_Core_Impossible_exn_struct*_tmp7EE;(int)_throw((void*)((_tmp7EE=_cycalloc(sizeof(*_tmp7EE)),((_tmp7EE[0]=((_tmp7F0.tag=Cyc_Core_Impossible,((_tmp7F0.f1=((_tmp7EF="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp7EF,sizeof(char),22))),_tmp7F0)))),_tmp7EE)))));}}else{_LL206: _tmp337=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp336)->f1;_LL207:
# 1613
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp184,_tmp337);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp334->hd;
# 1616
if(_tmp183 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp33B=(*((struct _tuple26*)_tmp182->hd)).f2;
_tmp333=Cyc_NewControlFlow_use_Rval(env,_tmp33B->loc,_tmp333,(void*)_tmp334->hd);
# 1620
Cyc_NewControlFlow_check_union_requires(_tmp33B->loc,_tmp160->r,exp_type,_tmp337,_tmp333);}}_LL203:;}}}{
# 1623
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7F6;struct Cyc_CfFlowInfo_UnionRInfo _tmp7F5;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7F4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp33C=(_tmp7F4=_region_malloc(env->r,sizeof(*_tmp7F4)),((_tmp7F4[0]=((_tmp7F6.tag=6,((_tmp7F6.f1=((_tmp7F5.is_union=_tmp183 == Cyc_Absyn_UnionA,((_tmp7F5.fieldnum=field_no,_tmp7F5)))),((_tmp7F6.f2=aggrdict,_tmp7F6)))))),_tmp7F4)));
struct _tuple16 _tmp7F7;struct _tuple16 _tmp33E=(_tmp7F7.f1=_tmp333,((_tmp7F7.f2=(void*)_tmp33C,_tmp7F7)));_npop_handler(0);return _tmp33E;};};}
# 1601
;_pop_region(temp);}}else{goto _LL16C;}}else{_LL16C: _LL16D: {
# 1627
const char*_tmp7FB;void*_tmp7FA[1];struct Cyc_String_pa_PrintArg_struct _tmp7F9;(_tmp7F9.tag=0,((_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7FA[0]=& _tmp7F9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7FB="anal_Rexp:missing aggrdeclimpl in %s",_tag_dyneither(_tmp7FB,sizeof(char),37))),_tag_dyneither(_tmp7FA,sizeof(void*),1)))))));}}case 25: _LL16E: _tmp181=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL16F: {
# 1629
struct _RegionHandle _tmp345=_new_region("temp");struct _RegionHandle*temp=& _tmp345;_push_region(temp);
{struct Cyc_List_List*_tmp346=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp181);
struct _tuple23 _tmp347=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp346,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp349;struct Cyc_List_List*_tmp34A;struct _tuple23 _tmp348=_tmp347;_tmp349=_tmp348.f1;_tmp34A=_tmp348.f2;
for(0;_tmp34A != 0;(_tmp34A=_tmp34A->tl,_tmp346=_tmp346->tl)){
_tmp349=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp346))->hd)->loc,_tmp349,(void*)_tmp34A->hd);}{
struct _tuple16 _tmp7FC;struct _tuple16 _tmp34C=(_tmp7FC.f1=_tmp349,((_tmp7FC.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,(void*)_check_null(e->topt),0,_tmp160->unknown_all),_tmp7FC)));_npop_handler(0);return _tmp34C;};}
# 1630
;_pop_region(temp);}case 26: _LL170: _tmp17D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp17E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_tmp17F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_tmp180=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp178)->f4;_LL171: {
# 1638
struct _tuple16 _tmp34D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp17E);union Cyc_CfFlowInfo_FlowInfo _tmp34F;void*_tmp350;struct _tuple16 _tmp34E=_tmp34D;_tmp34F=_tmp34E.f1;_tmp350=_tmp34E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp351=_tmp34F;struct Cyc_Dict_Dict _tmp352;struct Cyc_List_List*_tmp353;if((_tmp351.BottomFL).tag == 1){_LL209: _LL20A: {
struct _tuple16 _tmp7FD;return(_tmp7FD.f1=_tmp34F,((_tmp7FD.f2=_tmp160->unknown_all,_tmp7FD)));}}else{_LL20B: _tmp352=((_tmp351.ReachableFL).val).f1;_tmp353=((_tmp351.ReachableFL).val).f2;_LL20C:
# 1642
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp352,_tmp350)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp800;void*_tmp7FF;(_tmp7FF=0,Cyc_CfFlowInfo_aerr(_tmp17E->loc,((_tmp800="expression may not be initialized",_tag_dyneither(_tmp800,sizeof(char),34))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}{
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
void*_tmp357=_tmp350;switch(*((int*)_tmp357)){case 0: _LL20E: _LL20F: {
struct _tuple16 _tmp801;return(_tmp801.f1=_tmp34F,((_tmp801.f2=_tmp160->unknown_all,_tmp801)));}case 2: _LL210: _LL211:
 goto _LL213;case 1: _LL212: _LL213:
 goto _LL215;case 5: _LL214: _LL215: {
# 1661
struct Cyc_List_List _tmp802;struct Cyc_List_List _tmp359=(_tmp802.hd=_tmp17D,((_tmp802.tl=0,_tmp802)));
_tmp34F=Cyc_NewControlFlow_add_vars(_tmp160,_tmp34F,& _tmp359,_tmp160->unknown_all,e->loc,0);{
# 1664
struct _tuple16 _tmp35A=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34F,_tmp17F);union Cyc_CfFlowInfo_FlowInfo _tmp35C;void*_tmp35D;struct _tuple16 _tmp35B=_tmp35A;_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp35E=_tmp35C;struct Cyc_Dict_Dict _tmp35F;if((_tmp35E.BottomFL).tag == 1){_LL219: _LL21A: {
struct _tuple16 _tmp803;return(_tmp803.f1=_tmp35C,((_tmp803.f2=_tmp160->unknown_all,_tmp803)));}}else{_LL21B: _tmp35F=((_tmp35E.ReachableFL).val).f1;_LL21C:
# 1668
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp35F,_tmp35D)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp806;void*_tmp805;(_tmp805=0,Cyc_CfFlowInfo_aerr(_tmp17E->loc,((_tmp806="expression may not be initialized",_tag_dyneither(_tmp806,sizeof(char),34))),_tag_dyneither(_tmp805,sizeof(void*),0)));}{
struct _tuple16 _tmp807;return(_tmp807.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp807.f2=_tmp160->unknown_all,_tmp807)));};}}_LL218:;}
# 1673
_tmp34F=_tmp35C;
goto _LL217;};}default: _LL216: _LL217:
# 1676
 while(1){
struct Cyc_List_List _tmp808;struct Cyc_List_List _tmp365=(_tmp808.hd=_tmp17D,((_tmp808.tl=0,_tmp808)));
_tmp34F=Cyc_NewControlFlow_add_vars(_tmp160,_tmp34F,& _tmp365,_tmp160->unknown_all,e->loc,0);{
struct _tuple16 _tmp366=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp34F,_tmp17F);union Cyc_CfFlowInfo_FlowInfo _tmp368;void*_tmp369;struct _tuple16 _tmp367=_tmp366;_tmp368=_tmp367.f1;_tmp369=_tmp367.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp36A=_tmp368;struct Cyc_Dict_Dict _tmp36B;if((_tmp36A.BottomFL).tag == 1){_LL21E: _LL21F:
 goto _LL21D;}else{_LL220: _tmp36B=((_tmp36A.ReachableFL).val).f1;_LL221:
# 1683
 if(Cyc_CfFlowInfo_initlevel(_tmp160,_tmp36B,_tmp369)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp80B;void*_tmp80A;(_tmp80A=0,Cyc_CfFlowInfo_aerr(_tmp17E->loc,((_tmp80B="expression may not be initialized",_tag_dyneither(_tmp80B,sizeof(char),34))),_tag_dyneither(_tmp80A,sizeof(void*),0)));}{
struct _tuple16 _tmp80C;return(_tmp80C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp80C.f2=_tmp160->unknown_all,_tmp80C)));};}}_LL21D:;}{
# 1688
union Cyc_CfFlowInfo_FlowInfo _tmp36F=Cyc_CfFlowInfo_join_flow(_tmp160,env->all_changed,_tmp34F,_tmp368);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp36F,_tmp34F))
break;
_tmp34F=_tmp36F;};};}{
# 1693
struct _tuple16 _tmp80D;return(_tmp80D.f1=_tmp34F,((_tmp80D.f2=_tmp160->unknown_all,_tmp80D)));};}_LL20D:;};};}_LL208:;};}case 27: _LL172: _tmp17A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_tmp17B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp178)->f2;_tmp17C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp178)->f3;_LL173: {
# 1699
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp810;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp80F;void*root=(void*)((_tmp80F=_region_malloc(_tmp160->r,sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp810.tag=1,((_tmp810.f1=_tmp17A,((_tmp810.f2=(void*)_check_null(e->topt),_tmp810)))))),_tmp80F))));
struct Cyc_CfFlowInfo_Place*_tmp811;struct Cyc_CfFlowInfo_Place*place=(_tmp811=_region_malloc(_tmp160->r,sizeof(*_tmp811)),((_tmp811->root=root,((_tmp811->fields=0,_tmp811)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp814;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp813;void*rval=(void*)((_tmp813=_region_malloc(_tmp160->r,sizeof(*_tmp813)),((_tmp813[0]=((_tmp814.tag=5,((_tmp814.f1=place,_tmp814)))),_tmp813))));
void*place_val;
# 1707
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp160,_tmp17B,0,_tmp160->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp372=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp17A);union Cyc_CfFlowInfo_FlowInfo _tmp374;struct _tuple16 _tmp373=_tmp372;_tmp374=_tmp373.f1;
outflow=_tmp374;{
union Cyc_CfFlowInfo_FlowInfo _tmp375=outflow;struct Cyc_Dict_Dict _tmp376;struct Cyc_List_List*_tmp377;if((_tmp375.BottomFL).tag == 1){_LL223: _LL224: {
struct _tuple16 _tmp815;return(_tmp815.f1=outflow,((_tmp815.f2=rval,_tmp815)));}}else{_LL225: _tmp376=((_tmp375.ReachableFL).val).f1;_tmp377=((_tmp375.ReachableFL).val).f2;_LL226: {
# 1715
struct _tuple16 _tmp816;return(_tmp816.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp376,root,place_val),_tmp377),((_tmp816.f2=rval,_tmp816)));}}_LL222:;};};};}case 36: _LL174: _tmp179=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp178)->f1;_LL175: {
# 1719
struct _tuple17 _tmp817;struct _tuple17 _tmp37F=(_tmp817.f1=(env->fenv)->unknown_all,((_tmp817.f2=copy_ctxt,_tmp817)));
union Cyc_CfFlowInfo_FlowInfo _tmp380=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp179,& _tmp37F);
struct _tuple16 _tmp818;return(_tmp818.f1=_tmp380,((_tmp818.f2=_tmp37F.f1,_tmp818)));}case 35: _LL178: _LL179:
# 1724
 goto _LL17B;case 24: _LL17A: _LL17B:
 goto _LL17D;case 38: _LL17C: _LL17D:
 goto _LL17F;default: _LL17E: _LL17F: {
# 1728
struct Cyc_Core_Impossible_exn_struct _tmp81E;const char*_tmp81D;struct Cyc_Core_Impossible_exn_struct*_tmp81C;(int)_throw((void*)((_tmp81C=_cycalloc(sizeof(*_tmp81C)),((_tmp81C[0]=((_tmp81E.tag=Cyc_Core_Impossible,((_tmp81E.f1=((_tmp81D="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp81D,sizeof(char),31))),_tmp81E)))),_tmp81C)))));}}_LL117:;};}
# 1738
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place,struct Cyc_List_List*flds){
# 1747
struct Cyc_CfFlowInfo_FlowEnv*_tmp386=env->fenv;
void*_tmp387=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp388=_tmp387;void*_tmp389;union Cyc_Absyn_Constraint*_tmp38A;union Cyc_Absyn_Constraint*_tmp38B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->tag == 5){_LL228: _tmp389=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->f1).elt_typ;_tmp38A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->f1).ptr_atts).bounds;_tmp38B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->f1).ptr_atts).zero_term;_LL229: {
# 1750
union Cyc_CfFlowInfo_FlowInfo _tmp38C=f;struct Cyc_Dict_Dict _tmp38D;struct Cyc_List_List*_tmp38E;if((_tmp38C.BottomFL).tag == 1){_LL22D: _LL22E: {
struct _tuple18 _tmp81F;return(_tmp81F.f1=f,((_tmp81F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp81F)));}}else{_LL22F: _tmp38D=((_tmp38C.ReachableFL).val).f1;_tmp38E=((_tmp38C.ReachableFL).val).f2;_LL230:
# 1754
 if(Cyc_Tcutil_is_bound_one(_tmp38A)){
struct _tuple15 _tmp390=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp392;struct Cyc_List_List*_tmp393;struct _tuple15 _tmp391=_tmp390;_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;
retry: {void*_tmp394=_tmp392;enum Cyc_CfFlowInfo_InitLevel _tmp395;void*_tmp396;struct Cyc_List_List*_tmp397;switch(*((int*)_tmp394)){case 8: _LL232: _LL233: {
# 1758
struct Cyc_Core_Impossible_exn_struct _tmp825;const char*_tmp824;struct Cyc_Core_Impossible_exn_struct*_tmp823;(int)_throw((void*)((_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823[0]=((_tmp825.tag=Cyc_Core_Impossible,((_tmp825.f1=((_tmp824="named location in anal_derefL",_tag_dyneither(_tmp824,sizeof(char),30))),_tmp825)))),_tmp823)))));}case 1: _LL234: _LL235:
 goto _LL237;case 2: _LL236: _LL237:
# 1761
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp828;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp827;e->annot=(void*)((_tmp827=_cycalloc(sizeof(*_tmp827)),((_tmp827[0]=((_tmp828.tag=Cyc_CfFlowInfo_NotZero,((_tmp828.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp828)))),_tmp827))));}goto _LL231;case 5: _LL238: _tmp396=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp394)->f1)->root;_tmp397=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp394)->f1)->fields;_LL239:
# 1764
 if(expanded_place){
# 1767
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp82B;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp82A;e->annot=(void*)((_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A[0]=((_tmp82B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp82B.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp82B)))),_tmp82A))));}else{
# 1770
void*_tmp39F=e->annot;void*_tmp3A0=_tmp39F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp3A0)->tag == Cyc_CfFlowInfo_UnknownZ){_LL241: _LL242:
# 1774
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp82E;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp82D;e->annot=(void*)((_tmp82D=_cycalloc(sizeof(*_tmp82D)),((_tmp82D[0]=((_tmp82E.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp82E.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp82E)))),_tmp82D))));}
goto _LL240;}else{_LL243: _LL244:
# 1777
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp831;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp830;e->annot=(void*)((_tmp830=_cycalloc(sizeof(*_tmp830)),((_tmp830[0]=((_tmp831.tag=Cyc_CfFlowInfo_NotZero,((_tmp831.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp831)))),_tmp830))));}
goto _LL240;}_LL240:;}{
# 1781
struct Cyc_CfFlowInfo_Place*_tmp834;struct _tuple18 _tmp833;return(_tmp833.f1=f,((_tmp833.f2=Cyc_CfFlowInfo_PlaceL(((_tmp834=_region_malloc(_tmp386->r,sizeof(*_tmp834)),((_tmp834->root=_tmp396,((_tmp834->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp386->r,_tmp397,flds),_tmp834))))))),_tmp833)));};case 0: _LL23A: _LL23B:
# 1783
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple18 _tmp835;return(_tmp835.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp835.f2=Cyc_CfFlowInfo_UnknownL(),_tmp835)));};case 3: _LL23C: _tmp395=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp394)->f1;_LL23D:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp395,_tmp393);goto _LL23F;default: _LL23E: _LL23F:
# 1788
 if(passthrough_consumes){
void*_tmp3A8=_tmp392;void*_tmp3A9;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A8)->tag == 7){_LL246: _tmp3A9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3A8)->f3;_LL247:
 _tmp392=_tmp3A9;goto retry;}else{_LL248: _LL249:
 goto _LL245;}_LL245:;}{
# 1794
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp838;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp837;e->annot=(void*)((_tmp837=_cycalloc(sizeof(*_tmp837)),((_tmp837[0]=((_tmp838.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp838.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp838)))),_tmp837))));};}_LL231:;}}else{
# 1798
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp83B;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp83A;e->annot=(void*)((_tmp83A=_cycalloc(sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp83B.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp38E),_tmp83B)))),_tmp83A))));}
if(Cyc_CfFlowInfo_initlevel(_tmp386,_tmp38D,r)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp3AE=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp3B0;struct _tuple15 _tmp3AF=_tmp3AE;_tmp3B0=_tmp3AF.f1;{
void*_tmp3B1=_tmp3B0;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3B1)->tag == 7){_LL24B: _LL24C:
# 1803
{const char*_tmp83E;void*_tmp83D;(_tmp83D=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp83E="attempt to dereference an alias-free that has already been copied",_tag_dyneither(_tmp83E,sizeof(char),66))),_tag_dyneither(_tmp83D,sizeof(void*),0)));}
goto _LL24A;}else{_LL24D: _LL24E:
# 1806
{const char*_tmp841;void*_tmp840;(_tmp840=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp841="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp841,sizeof(char),46))),_tag_dyneither(_tmp840,sizeof(void*),0)));}
goto _LL24A;}_LL24A:;};}{
# 1810
struct _tuple18 _tmp842;return(_tmp842.f1=f,((_tmp842.f2=Cyc_CfFlowInfo_UnknownL(),_tmp842)));};}_LL22C:;}}else{_LL22A: _LL22B: {
# 1812
struct Cyc_Core_Impossible_exn_struct _tmp848;const char*_tmp847;struct Cyc_Core_Impossible_exn_struct*_tmp846;(int)_throw((void*)((_tmp846=_cycalloc(sizeof(*_tmp846)),((_tmp846[0]=((_tmp848.tag=Cyc_Core_Impossible,((_tmp848.f1=((_tmp847="left deref of non-pointer-type",_tag_dyneither(_tmp847,sizeof(char),31))),_tmp848)))),_tmp846)))));}}_LL227:;}
# 1820
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1823
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3BA=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3BB=inflow;struct Cyc_Dict_Dict _tmp3BC;struct Cyc_List_List*_tmp3BD;if((_tmp3BB.BottomFL).tag == 1){_LL250: _LL251: {
struct _tuple18 _tmp849;return(_tmp849.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp849.f2=Cyc_CfFlowInfo_UnknownL(),_tmp849)));}}else{_LL252: _tmp3BC=((_tmp3BB.ReachableFL).val).f1;_tmp3BD=((_tmp3BB.ReachableFL).val).f2;_LL253:
# 1828
 d=_tmp3BC;}_LL24F:;}{
# 1832
void*_tmp3BF=e->r;void*_tmp3C0=_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C1;struct _dyneither_ptr*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C6;struct _dyneither_ptr*_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3C9;struct Cyc_Absyn_Vardecl*_tmp3CA;struct Cyc_Absyn_Vardecl*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;switch(*((int*)_tmp3C0)){case 13: _LL255: _tmp3CE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL256:
 _tmp3CD=_tmp3CE;goto _LL258;case 11: _LL257: _tmp3CD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_LL258:
 _tmp3CC=_tmp3CD;goto _LL25A;case 12: _LL259: _tmp3CC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_LL25A:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3CC,flds);case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)){case 3: _LL25B: _tmp3CB=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL25C:
# 1837
 _tmp3CA=_tmp3CB;goto _LL25E;case 4: _LL25D: _tmp3CA=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL25E:
 _tmp3C9=_tmp3CA;goto _LL260;case 5: _LL25F: _tmp3C9=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL260: {
# 1840
struct Cyc_CfFlowInfo_Place*_tmp853;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp852;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp851;struct _tuple18 _tmp850;return(_tmp850.f1=inflow,((_tmp850.f2=Cyc_CfFlowInfo_PlaceL(((_tmp853=_region_malloc(env->r,sizeof(*_tmp853)),((_tmp853->root=(void*)((_tmp851=_region_malloc(env->r,sizeof(*_tmp851)),((_tmp851[0]=((_tmp852.tag=0,((_tmp852.f1=_tmp3C9,_tmp852)))),_tmp851)))),((_tmp853->fields=flds,_tmp853))))))),_tmp850)));}case 1: _LL261: _tmp3C8=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->f1;_LL262: {
# 1842
struct _tuple18 _tmp854;return(_tmp854.f1=inflow,((_tmp854.f2=Cyc_CfFlowInfo_UnknownL(),_tmp854)));}default: goto _LL26B;}case 21: _LL263: _tmp3C6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_tmp3C7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL264:
# 1845
{void*_tmp3D4=Cyc_Tcutil_compress((void*)_check_null(_tmp3C6->topt));void*_tmp3D5=_tmp3D4;void*_tmp3D6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5)->tag == 5){_LL26E: _tmp3D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5)->f1).elt_typ;_LL26F:
# 1847
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3D6)){
Cyc_NewControlFlow_check_union_requires(_tmp3C6->loc,_tmp3BA->r,_tmp3D6,_tmp3C7,inflow);{
struct Cyc_List_List*_tmp855;flds=((_tmp855=_region_malloc(env->r,sizeof(*_tmp855)),((_tmp855->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp3D6,_tmp3C7),((_tmp855->tl=flds,_tmp855))))));};}
# 1851
goto _LL26D;}else{_LL270: _LL271: {
struct Cyc_Core_Impossible_exn_struct _tmp85B;const char*_tmp85A;struct Cyc_Core_Impossible_exn_struct*_tmp859;(int)_throw((void*)((_tmp859=_cycalloc(sizeof(*_tmp859)),((_tmp859[0]=((_tmp85B.tag=Cyc_Core_Impossible,((_tmp85B.f1=((_tmp85A="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp85A,sizeof(char),25))),_tmp85B)))),_tmp859)))));}}_LL26D:;}
# 1854
_tmp3C5=_tmp3C6;goto _LL266;case 19: _LL265: _tmp3C5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_LL266:
# 1857
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C5->topt),1)){
# 1859
struct _tuple18 _tmp3DB=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp3C5->topt),1),passthrough_consumes,_tmp3C5);
# 1859
union Cyc_CfFlowInfo_FlowInfo _tmp3DD;union Cyc_CfFlowInfo_AbsLVal _tmp3DE;struct _tuple18 _tmp3DC=_tmp3DB;_tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;{
# 1863
struct _tuple18 _tmp85C;struct _tuple18 _tmp3DF=(_tmp85C.f1=_tmp3DD,((_tmp85C.f2=_tmp3DE,_tmp85C)));struct _tuple18 _tmp3E0=_tmp3DF;struct Cyc_Dict_Dict _tmp3E1;struct Cyc_List_List*_tmp3E2;struct Cyc_CfFlowInfo_Place*_tmp3E3;if(((_tmp3E0.f1).ReachableFL).tag == 2){if(((_tmp3E0.f2).PlaceL).tag == 1){_LL273: _tmp3E1=(((_tmp3E0.f1).ReachableFL).val).f1;_tmp3E2=(((_tmp3E0.f1).ReachableFL).val).f2;_tmp3E3=((_tmp3E0.f2).PlaceL).val;_LL274: {
# 1865
void*_tmp3E4=Cyc_CfFlowInfo_lookup_place(_tmp3E1,_tmp3E3);
struct _tuple15 _tmp3E5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3E4);void*_tmp3E7;struct Cyc_List_List*_tmp3E8;struct _tuple15 _tmp3E6=_tmp3E5;_tmp3E7=_tmp3E6.f1;_tmp3E8=_tmp3E6.f2;{
void*_tmp3E9=_tmp3E7;struct Cyc_Absyn_Vardecl*_tmp3EA;void*_tmp3EB;switch(*((int*)_tmp3E9)){case 8: _LL278: _tmp3EA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E9)->f1;_tmp3EB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E9)->f2;_LL279: {
# 1869
struct Cyc_Core_Impossible_exn_struct _tmp862;const char*_tmp861;struct Cyc_Core_Impossible_exn_struct*_tmp860;(int)_throw((void*)((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860[0]=((_tmp862.tag=Cyc_Core_Impossible,((_tmp862.f1=((_tmp861="bad named location in anal_Lexp:deref",_tag_dyneither(_tmp861,sizeof(char),38))),_tmp862)))),_tmp860)))));}case 7: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3E9)->f3)->tag == 5){_LL27A: _LL27B:
 goto _LL27D;}else{goto _LL27E;}case 5: _LL27C: _LL27D:
# 1874
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3DD,_tmp3C5,_tmp3DD,_tmp3E4,passthrough_consumes,0,flds);default: _LL27E: _LL27F: {
# 1881
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3BA,_tmp3E1,_tmp3E7);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3BA->unknown_all: _tmp3BA->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp3BA,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp3C5->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp865;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp864;void*new_root=(void*)((_tmp864=_region_malloc(_tmp3BA->r,sizeof(*_tmp864)),((_tmp864[0]=((_tmp865.tag=1,((_tmp865.f1=e,((_tmp865.f2=(void*)_check_null(e->topt),_tmp865)))))),_tmp864))));
struct Cyc_CfFlowInfo_Place*_tmp866;struct Cyc_CfFlowInfo_Place*place=(_tmp866=_region_malloc(_tmp3BA->r,sizeof(*_tmp866)),((_tmp866->root=new_root,((_tmp866->fields=0,_tmp866)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp869;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp868;void*res=(void*)((_tmp868=_region_malloc(_tmp3BA->r,sizeof(*_tmp868)),((_tmp868[0]=((_tmp869.tag=5,((_tmp869.f1=place,_tmp869)))),_tmp868))));
for(0;_tmp3E8 != 0;_tmp3E8=_tmp3E8->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp86C;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp86B;res=(void*)((_tmp86B=_region_malloc(_tmp3BA->r,sizeof(*_tmp86B)),((_tmp86B[0]=((_tmp86C.tag=8,((_tmp86C.f1=(struct Cyc_Absyn_Vardecl*)_tmp3E8->hd,((_tmp86C.f2=res,_tmp86C)))))),_tmp86B))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp3E1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E1,new_root,new_rval);
_tmp3E1=Cyc_CfFlowInfo_assign_place(_tmp3BA,e->loc,_tmp3E1,env->all_changed,_tmp3E3,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp3F1=Cyc_CfFlowInfo_ReachableFL(_tmp3E1,_tmp3E2);
# 1899
return Cyc_NewControlFlow_anal_derefL(env,_tmp3F1,_tmp3C5,_tmp3F1,res,passthrough_consumes,1,flds);};}}_LL277:;};}}else{goto _LL275;}}else{_LL275: _LL276:
# 1902
 goto _LL272;}_LL272:;};}{
# 1905
struct _tuple16 _tmp3F8=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp3C5);union Cyc_CfFlowInfo_FlowInfo _tmp3FA;void*_tmp3FB;struct _tuple16 _tmp3F9=_tmp3F8;_tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3C5,_tmp3FA,_tmp3FB,passthrough_consumes,0,flds);};case 22: _LL267: _tmp3C3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_tmp3C4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL268: {
# 1909
void*_tmp3FC=Cyc_Tcutil_compress((void*)_check_null(_tmp3C3->topt));void*_tmp3FD=_tmp3FC;union Cyc_Absyn_Constraint*_tmp3FE;switch(*((int*)_tmp3FD)){case 10: _LL281: _LL282: {
# 1911
unsigned int _tmp3FF=(Cyc_Evexp_eval_const_uint_exp(_tmp3C4)).f1;
struct Cyc_List_List*_tmp86D;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3C3,((_tmp86D=_region_malloc(env->r,sizeof(*_tmp86D)),((_tmp86D->hd=(void*)_tmp3FF,((_tmp86D->tl=flds,_tmp86D)))))));}case 5: _LL283: _tmp3FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FD)->f1).ptr_atts).bounds;_LL284: {
# 1914
struct _RegionHandle _tmp401=_new_region("temp");struct _RegionHandle*temp=& _tmp401;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp86E[2];struct _tuple23 _tmp402=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp86E[1]=_tmp3C4,((_tmp86E[0]=_tmp3C3,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp86E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1915
union Cyc_CfFlowInfo_FlowInfo _tmp404;struct Cyc_List_List*_tmp405;struct _tuple23 _tmp403=_tmp402;_tmp404=_tmp403.f1;_tmp405=_tmp403.f2;{
# 1917
union Cyc_CfFlowInfo_FlowInfo _tmp406=_tmp404;
{union Cyc_CfFlowInfo_FlowInfo _tmp407=_tmp404;struct Cyc_Dict_Dict _tmp408;struct Cyc_List_List*_tmp409;if((_tmp407.ReachableFL).tag == 2){_LL288: _tmp408=((_tmp407.ReachableFL).val).f1;_tmp409=((_tmp407.ReachableFL).val).f2;_LL289:
# 1920
 if(Cyc_CfFlowInfo_initlevel(_tmp3BA,_tmp408,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp405))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp871;void*_tmp870;(_tmp870=0,Cyc_CfFlowInfo_aerr(_tmp3C4->loc,((_tmp871="expression may not be initialized",_tag_dyneither(_tmp871,sizeof(char),34))),_tag_dyneither(_tmp870,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp40C=Cyc_NewControlFlow_add_subscript_reln(_tmp3BA->r,_tmp409,_tmp3C3,_tmp3C4);
if(_tmp409 != _tmp40C)
_tmp406=Cyc_CfFlowInfo_ReachableFL(_tmp408,_tmp40C);
goto _LL287;};}else{_LL28A: _LL28B:
 goto _LL287;}_LL287:;}{
# 1928
struct _tuple18 _tmp40D=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp3C3,_tmp404,(void*)((struct Cyc_List_List*)_check_null(_tmp405))->hd,passthrough_consumes,0,flds);union Cyc_CfFlowInfo_FlowInfo _tmp40F;union Cyc_CfFlowInfo_AbsLVal _tmp410;struct _tuple18 _tmp40E=_tmp40D;_tmp40F=_tmp40E.f1;_tmp410=_tmp40E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp411=_tmp40F;if((_tmp411.BottomFL).tag == 1){_LL28D: _LL28E: {
struct _tuple18 _tmp872;struct _tuple18 _tmp413=(_tmp872.f1=_tmp40F,((_tmp872.f2=_tmp410,_tmp872)));_npop_handler(0);return _tmp413;}}else{_LL28F: _LL290: {
struct _tuple18 _tmp873;struct _tuple18 _tmp415=(_tmp873.f1=_tmp406,((_tmp873.f2=_tmp410,_tmp873)));_npop_handler(0);return _tmp415;}}_LL28C:;};};};}
# 1915
;_pop_region(temp);}default: _LL285: _LL286: {
# 1934
struct Cyc_Core_Impossible_exn_struct _tmp879;const char*_tmp878;struct Cyc_Core_Impossible_exn_struct*_tmp877;(int)_throw((void*)((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877[0]=((_tmp879.tag=Cyc_Core_Impossible,((_tmp879.f1=((_tmp878="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp878,sizeof(char),33))),_tmp879)))),_tmp877)))));}}_LL280:;}case 20: _LL269: _tmp3C1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_tmp3C2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2;_LL26A: {
# 1938
void*_tmp41A=(void*)_check_null(_tmp3C1->topt);
if(Cyc_Absyn_is_require_union_type(_tmp41A))
Cyc_NewControlFlow_check_union_requires(_tmp3C1->loc,_tmp3BA->r,_tmp41A,_tmp3C2,inflow);
# 1942
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp41A)){
struct _tuple18 _tmp87A;return(_tmp87A.f1=inflow,((_tmp87A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp87A)));}{
# 1945
struct Cyc_List_List*_tmp87B;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3C1,(
(_tmp87B=_region_malloc(env->r,sizeof(*_tmp87B)),((_tmp87B->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp41A,_tmp3C2),((_tmp87B->tl=flds,_tmp87B)))))));};}default: _LL26B: _LL26C: {
# 1948
struct _tuple18 _tmp87C;return(_tmp87C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp87C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp87C)));}}_LL254:;};}
# 1952
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1955
struct _tuple18 _tmp41E=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp420;union Cyc_CfFlowInfo_AbsLVal _tmp421;struct _tuple18 _tmp41F=_tmp41E;_tmp420=_tmp41F.f1;_tmp421=_tmp41F.f2;{
struct _tuple18 _tmp87D;return(_tmp87D.f1=_tmp420,((_tmp87D.f2=_tmp421,_tmp87D)));};}
# 1962
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1965
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp423=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp425;struct _tuple18 _tmp424=_tmp423;_tmp425=_tmp424.f1;
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
struct _tuple23 _tmp428=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp42A;struct Cyc_List_List*_tmp42B;struct _tuple23 _tmp429=_tmp428;_tmp42A=_tmp429.f1;_tmp42B=_tmp429.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp42B))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp42B->tl))->hd;
f=_tmp42A;}{
# 1995
union Cyc_CfFlowInfo_FlowInfo _tmp42C=f;struct Cyc_Dict_Dict _tmp42D;struct Cyc_List_List*_tmp42E;if((_tmp42C.BottomFL).tag == 1){_LL292: _LL293: {
struct _tuple19 _tmp87E;struct _tuple19 _tmp430=(_tmp87E.f1=f,((_tmp87E.f2=f,_tmp87E)));_npop_handler(0);return _tmp430;}}else{_LL294: _tmp42D=((_tmp42C.ReachableFL).val).f1;_tmp42E=((_tmp42C.ReachableFL).val).f2;_LL295: {
# 1998
struct Cyc_Absyn_Exp*_tmp431=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp432=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42D,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp881;void*_tmp880;(_tmp880=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp881="expression may not be initialized",_tag_dyneither(_tmp881,sizeof(char),34))),_tag_dyneither(_tmp880,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp42D,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp884;void*_tmp883;(_tmp883=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp884="expression may not be initialized",_tag_dyneither(_tmp884,sizeof(char),34))),_tag_dyneither(_tmp883,sizeof(void*),0)));}{
# 2005
union Cyc_CfFlowInfo_FlowInfo _tmp437=f;
union Cyc_CfFlowInfo_FlowInfo _tmp438=f;
# 2010
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp439=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);void*_tmp43B;struct Cyc_List_List*_tmp43C;struct _tuple15 _tmp43A=_tmp439;_tmp43B=_tmp43A.f1;_tmp43C=_tmp43A.f2;{
struct _tuple15 _tmp43D=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);void*_tmp43F;struct Cyc_List_List*_tmp440;struct _tuple15 _tmp43E=_tmp43D;_tmp43F=_tmp43E.f1;_tmp440=_tmp43E.f2;{
struct _tuple0 _tmp885;struct _tuple0 _tmp441=(_tmp885.f1=_tmp43B,((_tmp885.f2=_tmp43F,_tmp885)));struct _tuple0 _tmp442=_tmp441;enum Cyc_CfFlowInfo_InitLevel _tmp443;enum Cyc_CfFlowInfo_InitLevel _tmp444;switch(*((int*)_tmp442.f1)){case 3: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL297: _tmp444=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp442.f1)->f1;_LL298: {
# 2017
struct _tuple19 _tmp445=Cyc_NewControlFlow_splitzero(env,f,f,_tmp431,_tmp444,_tmp43C);union Cyc_CfFlowInfo_FlowInfo _tmp447;union Cyc_CfFlowInfo_FlowInfo _tmp448;struct _tuple19 _tmp446=_tmp445;_tmp447=_tmp446.f1;_tmp448=_tmp446.f2;
{enum Cyc_Absyn_Primop _tmp449=p;switch(_tmp449){case Cyc_Absyn_Eq: _LL2AC: _LL2AD:
 _tmp437=_tmp448;_tmp438=_tmp447;goto _LL2AB;case Cyc_Absyn_Neq: _LL2AE: _LL2AF:
 _tmp437=_tmp447;_tmp438=_tmp448;goto _LL2AB;default: _LL2B0: _LL2B1: {
struct Cyc_Core_Impossible_exn_struct _tmp88B;const char*_tmp88A;struct Cyc_Core_Impossible_exn_struct*_tmp889;(int)_throw((void*)((_tmp889=_cycalloc(sizeof(*_tmp889)),((_tmp889[0]=((_tmp88B.tag=Cyc_Core_Impossible,((_tmp88B.f1=((_tmp88A="anal_test, zero-split",_tag_dyneither(_tmp88A,sizeof(char),22))),_tmp88B)))),_tmp889)))));}}_LL2AB:;}
# 2023
goto _LL296;}}else{goto _LL2A9;}case 0: switch(*((int*)_tmp442.f2)){case 3: _LL299: _tmp443=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->f1;_LL29A: {
# 2025
struct _tuple19 _tmp44D=Cyc_NewControlFlow_splitzero(env,f,f,_tmp432,_tmp443,_tmp440);union Cyc_CfFlowInfo_FlowInfo _tmp44F;union Cyc_CfFlowInfo_FlowInfo _tmp450;struct _tuple19 _tmp44E=_tmp44D;_tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;
{enum Cyc_Absyn_Primop _tmp451=p;switch(_tmp451){case Cyc_Absyn_Eq: _LL2B3: _LL2B4:
 _tmp437=_tmp450;_tmp438=_tmp44F;goto _LL2B2;case Cyc_Absyn_Neq: _LL2B5: _LL2B6:
 _tmp437=_tmp44F;_tmp438=_tmp450;goto _LL2B2;default: _LL2B7: _LL2B8: {
struct Cyc_Core_Impossible_exn_struct _tmp891;const char*_tmp890;struct Cyc_Core_Impossible_exn_struct*_tmp88F;(int)_throw((void*)((_tmp88F=_cycalloc(sizeof(*_tmp88F)),((_tmp88F[0]=((_tmp891.tag=Cyc_Core_Impossible,((_tmp891.f1=((_tmp890="anal_test, zero-split",_tag_dyneither(_tmp890,sizeof(char),22))),_tmp891)))),_tmp88F)))));}}_LL2B2:;}
# 2031
goto _LL296;}case 0: _LL29B: _LL29C:
# 2033
 if(p == Cyc_Absyn_Eq)_tmp438=Cyc_CfFlowInfo_BottomFL();else{
_tmp437=Cyc_CfFlowInfo_BottomFL();}
goto _LL296;case 1: _LL29D: _LL29E:
 goto _LL2A0;case 2: _LL29F: _LL2A0:
 goto _LL2A2;case 5: _LL2A1: _LL2A2:
 goto _LL2A4;default: goto _LL2A9;}case 1: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL2A3: _LL2A4:
 goto _LL2A6;}else{goto _LL2A9;}case 2: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL2A5: _LL2A6:
 goto _LL2A8;}else{goto _LL2A9;}case 5: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp442.f2)->tag == 0){_LL2A7: _LL2A8:
# 2042
 if(p == Cyc_Absyn_Neq)_tmp438=Cyc_CfFlowInfo_BottomFL();else{
_tmp437=Cyc_CfFlowInfo_BottomFL();}
goto _LL296;}else{goto _LL2A9;}default: _LL2A9: _LL2AA:
 goto _LL296;}_LL296:;};};}
# 2053
{struct _tuple0 _tmp892;struct _tuple0 _tmp456=(_tmp892.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp431->topt)),((_tmp892.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp432->topt)),_tmp892)));struct _tuple0 _tmp457=_tmp456;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f1)->f1 == Cyc_Absyn_Unsigned){_LL2BA: _LL2BB:
 goto _LL2BD;}else{switch(*((int*)_tmp457.f2)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL2BC;else{goto _LL2C2;}case 19: goto _LL2C0;default: goto _LL2C2;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f1 == Cyc_Absyn_Unsigned){_LL2BC: _LL2BD:
 goto _LL2BF;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f1)->tag == 19)goto _LL2BE;else{goto _LL2C2;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f1)->tag == 19){_LL2BE: _LL2BF:
 goto _LL2C1;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp457.f2)->tag == 19){_LL2C0: _LL2C1:
 goto _LL2B9;}else{_LL2C2: _LL2C3: {
struct _tuple19 _tmp893;struct _tuple19 _tmp459=(_tmp893.f1=_tmp437,((_tmp893.f2=_tmp438,_tmp893)));_npop_handler(0);return _tmp459;}}}}}_LL2B9:;}
# 2062
{enum Cyc_Absyn_Primop _tmp45B=p;switch(_tmp45B){case Cyc_Absyn_Eq: _LL2C5: _LL2C6:
 goto _LL2C8;case Cyc_Absyn_Neq: _LL2C7: _LL2C8: goto _LL2CA;case Cyc_Absyn_Gt: _LL2C9: _LL2CA: goto _LL2CC;case Cyc_Absyn_Gte: _LL2CB: _LL2CC: goto _LL2CE;case Cyc_Absyn_Lt: _LL2CD: _LL2CE: goto _LL2D0;case Cyc_Absyn_Lte: _LL2CF: _LL2D0: goto _LL2C4;default: _LL2D1: _LL2D2: {
struct _tuple19 _tmp894;struct _tuple19 _tmp45D=(_tmp894.f1=_tmp437,((_tmp894.f2=_tmp438,_tmp894)));_npop_handler(0);return _tmp45D;}}_LL2C4:;}{
# 2067
struct _RegionHandle*_tmp45E=(env->fenv)->r;
struct _tuple13 _tmp45F=Cyc_Relations_primop2relation(_tmp431,p,_tmp432);struct Cyc_Absyn_Exp*_tmp461;enum Cyc_Relations_Relation _tmp462;struct Cyc_Absyn_Exp*_tmp463;struct _tuple13 _tmp460=_tmp45F;_tmp461=_tmp460.f1;_tmp462=_tmp460.f2;_tmp463=_tmp460.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2072
if(Cyc_Relations_exp2relnop(_tmp461,& n1) && Cyc_Relations_exp2relnop(_tmp463,& n2)){
# 2074
struct Cyc_List_List*_tmp464=Cyc_Relations_add_relation(_tmp45E,n1,_tmp462,n2,_tmp42E);
# 2078
struct Cyc_List_List*_tmp465=Cyc_Relations_add_relation(_tmp45E,n2,Cyc_Relations_flip_relation(_tmp462),n1,_tmp42E);
struct _tuple19 _tmp895;struct _tuple19 _tmp466=(_tmp895.f1=_tmp437,((_tmp895.f2=_tmp438,_tmp895)));struct _tuple19 _tmp467=_tmp466;struct Cyc_Dict_Dict _tmp468;struct Cyc_Dict_Dict _tmp469;struct Cyc_Dict_Dict _tmp46A;struct Cyc_Dict_Dict _tmp46B;if(((_tmp467.f1).ReachableFL).tag == 2){if(((_tmp467.f2).ReachableFL).tag == 2){_LL2D4: _tmp46A=(((_tmp467.f1).ReachableFL).val).f1;_tmp46B=(((_tmp467.f2).ReachableFL).val).f1;_LL2D5: {
# 2081
struct _tuple19 _tmp896;struct _tuple19 _tmp46D=(_tmp896.f1=Cyc_CfFlowInfo_ReachableFL(_tmp46A,_tmp464),((_tmp896.f2=Cyc_CfFlowInfo_ReachableFL(_tmp46B,_tmp465),_tmp896)));_npop_handler(0);return _tmp46D;}}else{_LL2D8: _tmp469=(((_tmp467.f1).ReachableFL).val).f1;_LL2D9: {
# 2085
struct _tuple19 _tmp897;struct _tuple19 _tmp471=(_tmp897.f1=Cyc_CfFlowInfo_ReachableFL(_tmp469,_tmp464),((_tmp897.f2=_tmp438,_tmp897)));_npop_handler(0);return _tmp471;}}}else{if(((_tmp467.f2).ReachableFL).tag == 2){_LL2D6: _tmp468=(((_tmp467.f2).ReachableFL).val).f1;_LL2D7: {
# 2083
struct _tuple19 _tmp898;struct _tuple19 _tmp46F=(_tmp898.f1=_tmp437,((_tmp898.f2=Cyc_CfFlowInfo_ReachableFL(_tmp468,_tmp465),_tmp898)));_npop_handler(0);return _tmp46F;}}else{_LL2DA: _LL2DB: {
# 2087
struct _tuple19 _tmp899;struct _tuple19 _tmp473=(_tmp899.f1=_tmp437,((_tmp899.f2=_tmp438,_tmp899)));_npop_handler(0);return _tmp473;}}}_LL2D3:;}else{
# 2090
struct _tuple19 _tmp89A;struct _tuple19 _tmp476=(_tmp89A.f1=_tmp437,((_tmp89A.f2=_tmp438,_tmp89A)));_npop_handler(0);return _tmp476;}};};};}}_LL291:;};
# 1987
;_pop_region(temp);}
# 2096
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2098
struct Cyc_CfFlowInfo_FlowEnv*_tmp477=env->fenv;
void*_tmp478=e->r;void*_tmp479=_tmp478;enum Cyc_Absyn_Primop _tmp47A;struct Cyc_List_List*_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*_tmp485;switch(*((int*)_tmp479)){case 5: _LL2DD: _tmp483=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp484=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_tmp485=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp479)->f3;_LL2DE: {
# 2101
struct _tuple19 _tmp486=Cyc_NewControlFlow_anal_test(env,inflow,_tmp483);union Cyc_CfFlowInfo_FlowInfo _tmp488;union Cyc_CfFlowInfo_FlowInfo _tmp489;struct _tuple19 _tmp487=_tmp486;_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;{
struct _tuple19 _tmp48A=Cyc_NewControlFlow_anal_test(env,_tmp488,_tmp484);union Cyc_CfFlowInfo_FlowInfo _tmp48C;union Cyc_CfFlowInfo_FlowInfo _tmp48D;struct _tuple19 _tmp48B=_tmp48A;_tmp48C=_tmp48B.f1;_tmp48D=_tmp48B.f2;{
struct _tuple19 _tmp48E=Cyc_NewControlFlow_anal_test(env,_tmp489,_tmp485);union Cyc_CfFlowInfo_FlowInfo _tmp490;union Cyc_CfFlowInfo_FlowInfo _tmp491;struct _tuple19 _tmp48F=_tmp48E;_tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;{
struct _tuple19 _tmp89B;return(_tmp89B.f1=Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp48C,_tmp490),((_tmp89B.f2=
Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp48D,_tmp491),_tmp89B)));};};};}case 6: _LL2DF: _tmp481=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp482=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL2E0: {
# 2107
struct _tuple19 _tmp493=Cyc_NewControlFlow_anal_test(env,inflow,_tmp481);union Cyc_CfFlowInfo_FlowInfo _tmp495;union Cyc_CfFlowInfo_FlowInfo _tmp496;struct _tuple19 _tmp494=_tmp493;_tmp495=_tmp494.f1;_tmp496=_tmp494.f2;{
struct _tuple19 _tmp497=Cyc_NewControlFlow_anal_test(env,_tmp495,_tmp482);union Cyc_CfFlowInfo_FlowInfo _tmp499;union Cyc_CfFlowInfo_FlowInfo _tmp49A;struct _tuple19 _tmp498=_tmp497;_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;{
struct _tuple19 _tmp89C;return(_tmp89C.f1=_tmp499,((_tmp89C.f2=Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp496,_tmp49A),_tmp89C)));};};}case 7: _LL2E1: _tmp47F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp480=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL2E2: {
# 2111
struct _tuple19 _tmp49C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47F);union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49F;struct _tuple19 _tmp49D=_tmp49C;_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{
struct _tuple19 _tmp4A0=Cyc_NewControlFlow_anal_test(env,_tmp49F,_tmp480);union Cyc_CfFlowInfo_FlowInfo _tmp4A2;union Cyc_CfFlowInfo_FlowInfo _tmp4A3;struct _tuple19 _tmp4A1=_tmp4A0;_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;{
struct _tuple19 _tmp89D;return(_tmp89D.f1=Cyc_CfFlowInfo_join_flow(_tmp477,env->all_changed,_tmp49E,_tmp4A2),((_tmp89D.f2=_tmp4A3,_tmp89D)));};};}case 8: _LL2E3: _tmp47D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp47E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL2E4: {
# 2115
struct _tuple16 _tmp4A5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp47D);union Cyc_CfFlowInfo_FlowInfo _tmp4A7;void*_tmp4A8;struct _tuple16 _tmp4A6=_tmp4A5;_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp4A7,_tmp47E);}case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2)->tl == 0){_LL2E5: _tmp47C=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2)->hd;_LL2E6: {
# 2118
struct _tuple19 _tmp4A9=Cyc_NewControlFlow_anal_test(env,inflow,_tmp47C);union Cyc_CfFlowInfo_FlowInfo _tmp4AB;union Cyc_CfFlowInfo_FlowInfo _tmp4AC;struct _tuple19 _tmp4AA=_tmp4A9;_tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;{
struct _tuple19 _tmp89E;return(_tmp89E.f1=_tmp4AC,((_tmp89E.f2=_tmp4AB,_tmp89E)));};}}else{goto _LL2E7;}}else{goto _LL2E7;}}else{_LL2E7: _tmp47A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f1;_tmp47B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp479)->f2;_LL2E8:
# 2121
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp47A,_tmp47B);}default: _LL2E9: _LL2EA: {
# 2125
struct _tuple16 _tmp4AE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp4B0;void*_tmp4B1;struct _tuple16 _tmp4AF=_tmp4AE;_tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B2=_tmp4B0;struct Cyc_Dict_Dict _tmp4B3;if((_tmp4B2.BottomFL).tag == 1){_LL2EC: _LL2ED: {
struct _tuple19 _tmp89F;return(_tmp89F.f1=_tmp4B0,((_tmp89F.f2=_tmp4B0,_tmp89F)));}}else{_LL2EE: _tmp4B3=((_tmp4B2.ReachableFL).val).f1;_LL2EF: {
# 2129
struct _tuple15 _tmp4B5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp4B1);void*_tmp4B7;struct Cyc_List_List*_tmp4B8;struct _tuple15 _tmp4B6=_tmp4B5;_tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;{
void*_tmp4B9=_tmp4B7;enum Cyc_CfFlowInfo_InitLevel _tmp4BA;struct Cyc_Absyn_Vardecl*_tmp4BB;void*_tmp4BC;switch(*((int*)_tmp4B9)){case 8: _LL2F1: _tmp4BB=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1;_tmp4BC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f2;_LL2F2: {
# 2132
struct Cyc_Core_Impossible_exn_struct _tmp8A5;const char*_tmp8A4;struct Cyc_Core_Impossible_exn_struct*_tmp8A3;(int)_throw((void*)((_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3[0]=((_tmp8A5.tag=Cyc_Core_Impossible,((_tmp8A5.f1=((_tmp8A4="anal_test: bad namedlocation",_tag_dyneither(_tmp8A4,sizeof(char),29))),_tmp8A5)))),_tmp8A3)))));}case 0: _LL2F3: _LL2F4: {
struct _tuple19 _tmp8A6;return(_tmp8A6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A6.f2=_tmp4B0,_tmp8A6)));}case 2: _LL2F5: _LL2F6:
 goto _LL2F8;case 1: _LL2F7: _LL2F8:
 goto _LL2FA;case 5: _LL2F9: _LL2FA: {
struct _tuple19 _tmp8A7;return(_tmp8A7.f1=_tmp4B0,((_tmp8A7.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8A7)));}case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1 == Cyc_CfFlowInfo_NoneIL){_LL2FB: _LL2FC:
 goto _LL2FE;}else{_LL301: _tmp4BA=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1;_LL302:
# 2142
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4B0,e,_tmp4BA,_tmp4B8);}case 4: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4B9)->f1 == Cyc_CfFlowInfo_NoneIL){_LL2FD: _LL2FE:
# 2138
 goto _LL300;}else{_LL303: _LL304: {
# 2143
struct _tuple19 _tmp8A8;return(_tmp8A8.f1=_tmp4B0,((_tmp8A8.f2=_tmp4B0,_tmp8A8)));}}case 7: _LL2FF: _LL300:
# 2140
{const char*_tmp8AB;void*_tmp8AA;(_tmp8AA=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8AB="expression may not be initialized",_tag_dyneither(_tmp8AB,sizeof(char),34))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}{
struct _tuple19 _tmp8AC;return(_tmp8AC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8AC.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8AC)));};default: _LL305: _LL306: {
# 2144
struct Cyc_Core_Impossible_exn_struct _tmp8B2;const char*_tmp8B1;struct Cyc_Core_Impossible_exn_struct*_tmp8B0;(int)_throw((void*)((_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B2.tag=Cyc_Core_Impossible,((_tmp8B2.f1=((_tmp8B1="anal_test",_tag_dyneither(_tmp8B1,sizeof(char),10))),_tmp8B2)))),_tmp8B0)))));}}_LL2F0:;};}}_LL2EB:;};}}_LL2DC:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2150
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2152
unsigned int _tmp4CA;struct Cyc_NewControlFlow_AnalEnv*_tmp4CB;struct Cyc_Dict_Dict _tmp4CC;struct _tuple27*_tmp4C9=ckenv;_tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f2;_tmp4CC=_tmp4C9->f3;{
void*_tmp4CD=root;struct Cyc_Absyn_Vardecl*_tmp4CE;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4CD)->tag == 0){_LL308: _tmp4CE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4CD)->f1;_LL309:
# 2155
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp4CB->fenv)->r,_tmp4CE->type)){
struct _tuple15 _tmp4CF=Cyc_CfFlowInfo_unname_rval((_tmp4CB->fenv)->r,rval);void*_tmp4D1;struct _tuple15 _tmp4D0=_tmp4CF;_tmp4D1=_tmp4D0.f1;{
void*_tmp4D2=_tmp4D1;switch(*((int*)_tmp4D2)){case 7: _LL30D: _LL30E:
 goto _LL310;case 0: _LL30F: _LL310:
 goto _LL312;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL311: _LL312:
 goto _LL30C;}else{goto _LL313;}default: _LL313: _LL314:
# 2162
{const char*_tmp8B6;void*_tmp8B5[1];struct Cyc_String_pa_PrintArg_struct _tmp8B4;(_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4CE->name)),((_tmp8B5[0]=& _tmp8B4,Cyc_Tcutil_warn(_tmp4CA,((_tmp8B6="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp8B6,sizeof(char),57))),_tag_dyneither(_tmp8B5,sizeof(void*),1)))))));}
goto _LL30C;}_LL30C:;};}
# 2166
goto _LL307;}else{_LL30A: _LL30B:
 goto _LL307;}_LL307:;};}
# 2171
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4D6=flow;struct Cyc_Dict_Dict _tmp4D7;if((_tmp4D6.BottomFL).tag == 1){_LL316: _LL317:
 return;}else{_LL318: _tmp4D7=((_tmp4D6.ReachableFL).val).f1;_LL319:
# 2175
{struct Cyc_List_List*_tmp4D8=env->param_roots;for(0;_tmp4D8 != 0;_tmp4D8=_tmp4D8->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D7,Cyc_CfFlowInfo_lookup_place(_tmp4D7,(struct Cyc_CfFlowInfo_Place*)_tmp4D8->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp8B9;void*_tmp8B8;(_tmp8B8=0,Cyc_CfFlowInfo_aerr(loc,((_tmp8B9="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp8B9,sizeof(char),76))),_tag_dyneither(_tmp8B8,sizeof(void*),0)));}}}
# 2179
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp8BA;struct _tuple27 _tmp4DB=(_tmp8BA.f1=loc,((_tmp8BA.f2=env,((_tmp8BA.f3=_tmp4D7,_tmp8BA)))));
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4DB,_tmp4D7);}
# 2183
return;}_LL315:;}
# 2192
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2195
struct Cyc_List_List*_tmp4DD=0;
struct Cyc_List_List*_tmp4DE=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4DF=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;struct _tuple24*_tmp4E0=_tmp4DF;_tmp4E1=_tmp4E0->f1;_tmp4E2=_tmp4E0->f2;
if((_tmp4E1 != 0  && _tmp4E2 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp4E2->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8BD;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8BC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4E3=(_tmp8BC=_region_malloc(frgn,sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BD.tag=0,((_tmp8BD.f1=*_tmp4E1,_tmp8BD)))),_tmp8BC)));
struct Cyc_CfFlowInfo_Place*_tmp8BE;struct Cyc_CfFlowInfo_Place*_tmp4E4=(_tmp8BE=_region_malloc(frgn,sizeof(*_tmp8BE)),((_tmp8BE->root=(void*)_tmp4E3,((_tmp8BE->fields=0,_tmp8BE)))));
{struct Cyc_List_List*_tmp8BF;_tmp4DD=((_tmp8BF=_region_malloc(frgn,sizeof(*_tmp8BF)),((_tmp8BF->hd=_tmp4E4,((_tmp8BF->tl=_tmp4DD,_tmp8BF))))));}{
struct Cyc_List_List*_tmp8C0;_tmp4DE=((_tmp8C0=_region_malloc(frgn,sizeof(*_tmp8C0)),((_tmp8C0->hd=_tmp4E2,((_tmp8C0->tl=_tmp4DE,_tmp8C0))))));};}}}{
# 2207
struct _tuple1 _tmp8C1;return(_tmp8C1.f1=_tmp4DD,((_tmp8C1.f2=_tmp4DE,_tmp8C1)));};}struct _tuple28{int f1;void*f2;};
# 2213
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2220
union Cyc_CfFlowInfo_FlowInfo _tmp4EB=flow;struct Cyc_Dict_Dict _tmp4EC;if((_tmp4EB.BottomFL).tag == 1){_LL31B: _LL31C: {
# 2222
const char*_tmp8C4;void*_tmp8C3;(_tmp8C3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8C4="noconsume_place_ok: flow became Bottom!",_tag_dyneither(_tmp8C4,sizeof(char),40))),_tag_dyneither(_tmp8C3,sizeof(void*),0)));}}else{_LL31D: _tmp4EC=((_tmp4EB.ReachableFL).val).f1;_LL31E: {
# 2229
struct Cyc_Absyn_Exp*_tmp4EF=Cyc_Absyn_exp_unsigned_one;
int _tmp4F0=0;
int _tmp4F1=1;
void*_tmp4F2=Cyc_CfFlowInfo_lookup_place(_tmp4EC,place);
void*_tmp4F3=_tmp4F2;
# 2241
int varok=0;
{void*_tmp4F4=_tmp4F2;struct Cyc_Absyn_Vardecl*_tmp4F5;void*_tmp4F6;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F4)->tag == 8){_LL320: _tmp4F5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F4)->f1;_tmp4F6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F4)->f2;_LL321:
# 2244
 if(vd == _tmp4F5){
_tmp4F3=_tmp4F6;
# 2247
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
# 2249
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4EF,_tmp4F1,_tmp4F3,& _tmp4F0)){
if(!do_unconsume){
const char*_tmp8C8;void*_tmp8C7[1];struct Cyc_String_pa_PrintArg_struct _tmp8C6;(_tmp8C6.tag=0,((_tmp8C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2253
Cyc_Absynpp_qvar2string(vd->name)),((_tmp8C7[0]=& _tmp8C6,Cyc_CfFlowInfo_aerr(loc,((_tmp8C8="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp8C8,sizeof(char),58))),_tag_dyneither(_tmp8C7,sizeof(void*),1)))))));}}else{
# 2256
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4EC,_tmp4F3)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume){
const char*_tmp8CC;void*_tmp8CB[1];struct Cyc_String_pa_PrintArg_struct _tmp8CA;(_tmp8CA.tag=0,((_tmp8CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2259
Cyc_Absynpp_qvar2string(vd->name)),((_tmp8CB[0]=& _tmp8CA,Cyc_CfFlowInfo_aerr(loc,((_tmp8CC="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp8CC,sizeof(char),84))),_tag_dyneither(_tmp8CB,sizeof(void*),1)))))));}else{
# 2261
varok=1;}}}else{
# 2264
varok=1;}}else{
# 2267
goto _LL323;}
goto _LL31F;}else{_LL322: _LL323:
# 2271
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type))
varok=1;else{
if(!do_unconsume){
const char*_tmp8D0;void*_tmp8CF[1];struct Cyc_String_pa_PrintArg_struct _tmp8CE;(_tmp8CE.tag=0,((_tmp8CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2276
Cyc_Absynpp_qvar2string(vd->name)),((_tmp8CF[0]=& _tmp8CE,Cyc_CfFlowInfo_aerr(loc,((_tmp8D0="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp8D0,sizeof(char),85))),_tag_dyneither(_tmp8CF,sizeof(void*),1)))))));}}
goto _LL31F;}_LL31F:;}{
# 2280
struct _tuple28 _tmp8D1;return(_tmp8D1.f1=varok,((_tmp8D1.f2=_tmp4F3,_tmp8D1)));};}}_LL31A:;}
# 2286
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
void*_tmp502;struct Cyc_List_List*_tmp503;struct Cyc_CfFlowInfo_Place*_tmp501=p;_tmp502=_tmp501->root;_tmp503=_tmp501->fields;
if(_tmp503 != 0){
const char*_tmp8D4;void*_tmp8D3;(_tmp8D3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8D4="unconsume_params: param to unconsume is non-variable",_tag_dyneither(_tmp8D4,sizeof(char),53))),_tag_dyneither(_tmp8D3,sizeof(void*),0)));}{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp506=_tmp502;struct Cyc_Absyn_Vardecl*_tmp507;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp506)->tag == 0){_LL325: _tmp507=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp506)->f1;_LL326:
 return _tmp507;}else{_LL327: _LL328: {
const char*_tmp8D7;void*_tmp8D6;(_tmp8D6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8D7="unconsume_params: root is not a varroot",_tag_dyneither(_tmp8D7,sizeof(char),40))),_tag_dyneither(_tmp8D6,sizeof(void*),0)));}}_LL324:;};}
# 2305 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2313
{union Cyc_CfFlowInfo_FlowInfo _tmp50A=flow;struct Cyc_Dict_Dict _tmp50B;if((_tmp50A.BottomFL).tag == 1){_LL32A: _LL32B:
 return flow;}else{_LL32C: _tmp50B=((_tmp50A.ReachableFL).val).f1;_LL32D: {
# 2320
struct _tuple18 _tmp50C=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);union Cyc_CfFlowInfo_FlowInfo _tmp50E;union Cyc_CfFlowInfo_AbsLVal _tmp50F;struct _tuple18 _tmp50D=_tmp50C;_tmp50E=_tmp50D.f1;_tmp50F=_tmp50D.f2;
# 2323
{union Cyc_CfFlowInfo_AbsLVal _tmp510=_tmp50F;struct Cyc_CfFlowInfo_Place*_tmp511;if((_tmp510.PlaceL).tag == 1){_LL32F: _tmp511=(_tmp510.PlaceL).val;_LL330: {
# 2327
void*_tmp512=Cyc_CfFlowInfo_lookup_place(_tmp50B,_tmp511);
{void*_tmp513=_tmp512;struct Cyc_Absyn_Vardecl*_tmp514;void*_tmp515;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp513)->tag == 8){_LL334: _tmp514=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp513)->f1;_tmp515=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp513)->f2;_LL335: {
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
struct _tuple16 _tmp8D9;struct _tuple16 _tmp8D8;struct _tuple16 _tmp516=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2353
(_tmp8D8.f1=_tmp50E,((_tmp8D8.f2=_tmp512,_tmp8D8)))),(
(_tmp8D9.f1=_tmp50E,((_tmp8D9.f2=ropt,_tmp8D9)))));
# 2350
union Cyc_CfFlowInfo_FlowInfo _tmp518;void*_tmp519;struct _tuple16 _tmp517=_tmp516;_tmp518=_tmp517.f1;_tmp519=_tmp517.f2;
# 2355
_tmp50E=_tmp518;new_rval=_tmp519;}else{
# 2360
new_rval=_tmp512;}}else{
# 2363
new_rval=_tmp515;}
# 2365
{union Cyc_CfFlowInfo_FlowInfo _tmp51C=_tmp50E;struct Cyc_Dict_Dict _tmp51D;struct Cyc_List_List*_tmp51E;if((_tmp51C.ReachableFL).tag == 2){_LL339: _tmp51D=((_tmp51C.ReachableFL).val).f1;_tmp51E=((_tmp51C.ReachableFL).val).f2;_LL33A:
# 2367
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp51D,env->all_changed,_tmp511,new_rval),_tmp51E);
# 2371
goto _LL338;}else{_LL33B: _LL33C: {
# 2373
const char*_tmp8DC;void*_tmp8DB;(_tmp8DB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8DC="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp8DC,sizeof(char),42))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}}_LL338:;}
# 2375
goto _LL333;}else{
# 2377
goto _LL337;}
goto _LL333;}}else{_LL336: _LL337:
# 2384
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
const char*_tmp8E0;void*_tmp8DF[1];struct Cyc_String_pa_PrintArg_struct _tmp8DE;(_tmp8DE.tag=0,((_tmp8DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp)),((_tmp8DF[0]=& _tmp8DE,Cyc_CfFlowInfo_aerr(loc,((_tmp8E0="aliased expression %s was overwritten",_tag_dyneither(_tmp8E0,sizeof(char),38))),_tag_dyneither(_tmp8DF,sizeof(void*),1)))))));}
# 2401 "new_control_flow.cyc"
goto _LL333;}_LL333:;}
# 2403
goto _LL32E;}}else{_LL331: _LL332:
# 2409
 goto _LL32E;}_LL32E:;}
# 2411
goto _LL329;}}_LL329:;}
# 2413
return flow;}
# 2426 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2432
{union Cyc_CfFlowInfo_FlowInfo _tmp524=flow;if((_tmp524.BottomFL).tag == 1){_LL33E: _LL33F:
 return flow;}else{_LL340: _LL341:
 goto _LL33D;}_LL33D:;}{
# 2436
int _tmp525=unconsume_exps != 0;
{struct Cyc_List_List*_tmp526=consumed_vals;for(0;_tmp526 != 0;
(_tmp526=_tmp526->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2443
struct Cyc_Absyn_Vardecl*_tmp527=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp526->hd);
struct _tuple28 _tmp8E1;struct _tuple28 _tmp528=
is_region_open?(_tmp8E1.f1=1,((_tmp8E1.f2=0,_tmp8E1))):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp526->hd,_tmp525,_tmp527,flow,loc);
# 2444
int _tmp52A;void*_tmp52B;struct _tuple28 _tmp529=_tmp528;_tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;
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
struct Cyc_Absyn_Switch_clause*_tmp52E=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp530;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Stmt*_tmp532;unsigned int _tmp533;struct Cyc_Absyn_Switch_clause*_tmp52F=_tmp52E;_tmp530=_tmp52F->pat_vars;_tmp531=_tmp52F->where_clause;_tmp532=_tmp52F->body;_tmp533=_tmp52F->loc;{
struct _tuple1 _tmp534=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp530))->v);struct Cyc_List_List*_tmp536;struct Cyc_List_List*_tmp537;struct _tuple1 _tmp535=_tmp534;_tmp536=_tmp535.f1;_tmp537=_tmp535.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp530->v,_tmp536 != 0,_tmp533,exp_loc);
# 2468
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp538=env->unique_pat_vars;
{struct _tuple29*_tmp8E4;struct Cyc_List_List*_tmp8E3;env->unique_pat_vars=(
(_tmp8E3=_region_malloc(env->r,sizeof(*_tmp8E3)),((_tmp8E3->hd=((_tmp8E4=_region_malloc(env->r,sizeof(*_tmp8E4)),((_tmp8E4->f1=0,((_tmp8E4->f2=_tmp532,((_tmp8E4->f3=_tmp536,((_tmp8E4->f4=_tmp537,_tmp8E4)))))))))),((_tmp8E3->tl=_tmp538,_tmp8E3))))));}
# 2473
if(_tmp531 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp531;
struct _tuple19 _tmp53B=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp53D;union Cyc_CfFlowInfo_FlowInfo _tmp53E;struct _tuple19 _tmp53C=_tmp53B;_tmp53D=_tmp53C.f1;_tmp53E=_tmp53C.f2;
inflow=_tmp53E;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp53D,_tmp532,0);}else{
# 2479
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp532,0);}
# 2481
env->unique_pat_vars=_tmp538;{
union Cyc_CfFlowInfo_FlowInfo _tmp53F=clause_outflow;if((_tmp53F.BottomFL).tag == 1){_LL343: _LL344:
 goto _LL342;}else{_LL345: _LL346:
# 2486
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp536,_tmp537,0,clause_outflow,_tmp533);
# 2488
if(scs->tl == 0)
return clause_outflow;else{
# 2493
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp8E7;void*_tmp8E6;(_tmp8E6=0,Cyc_CfFlowInfo_aerr(_tmp532->loc,((_tmp8E7="switch clause may implicitly fallthru",_tag_dyneither(_tmp8E7,sizeof(char),38))),_tag_dyneither(_tmp8E6,sizeof(void*),0)));}else{
# 2496
const char*_tmp8EA;void*_tmp8E9;(_tmp8E9=0,Cyc_Tcutil_warn(_tmp532->loc,((_tmp8EA="switch clause may implicitly fallthru",_tag_dyneither(_tmp8EA,sizeof(char),38))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}
# 2498
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2500
goto _LL342;}_LL342:;};};};}
# 2503
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2508
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2510
struct Cyc_NewControlFlow_AnalEnv*_tmp545;struct Cyc_Dict_Dict _tmp546;unsigned int _tmp547;struct _tuple30*_tmp544=vdenv;_tmp545=_tmp544->f1;_tmp546=_tmp544->f2;_tmp547=_tmp544->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp545->fenv)->r,vd->type)){
# 2513
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8EB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp8EB.tag=0,((_tmp8EB.f1=vd,_tmp8EB)));
# 2515
struct _tuple15 _tmp548=Cyc_CfFlowInfo_unname_rval((_tmp545->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp546,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2515
void*_tmp54A;struct _tuple15 _tmp549=_tmp548;_tmp54A=_tmp549.f1;{
# 2517
void*_tmp54B=_tmp54A;switch(*((int*)_tmp54B)){case 7: _LL348: _LL349:
 goto _LL34B;case 0: _LL34A: _LL34B:
 goto _LL34D;case 3: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54B)->f1 == Cyc_CfFlowInfo_NoneIL){_LL34C: _LL34D:
 goto _LL347;}else{goto _LL34E;}default: _LL34E: _LL34F:
# 2522
{const char*_tmp8EF;void*_tmp8EE[1];struct Cyc_String_pa_PrintArg_struct _tmp8ED;(_tmp8ED.tag=0,((_tmp8ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp8EE[0]=& _tmp8ED,Cyc_Tcutil_warn(_tmp547,((_tmp8EF="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp8EF,sizeof(char),68))),_tag_dyneither(_tmp8EE,sizeof(void*),1)))))));}
# 2524
goto _LL347;}_LL347:;};}}
# 2529
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2531
{union Cyc_CfFlowInfo_FlowInfo _tmp550=inflow;struct Cyc_Dict_Dict _tmp551;if((_tmp550.ReachableFL).tag == 2){_LL351: _tmp551=((_tmp550.ReachableFL).val).f1;_LL352: {
# 2533
struct _tuple30 _tmp8F0;struct _tuple30 _tmp552=(_tmp8F0.f1=env,((_tmp8F0.f2=_tmp551,((_tmp8F0.f3=decl->loc,_tmp8F0)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp553=env->fenv;
{void*_tmp554=decl->r;void*_tmp555=_tmp554;struct Cyc_List_List*_tmp556;struct Cyc_List_List*_tmp557;struct Cyc_Absyn_Vardecl*_tmp558;switch(*((int*)_tmp555)){case 0: _LL356: _tmp558=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp555)->f1;_LL357:
# 2537
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp552,_tmp558);
goto _LL355;case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp555)->f2 != 0){_LL358: _tmp557=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp555)->f2)->v;_LL359: {
# 2540
struct _tuple1 _tmp559=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp557);struct Cyc_List_List*_tmp55B;struct _tuple1 _tmp55A=_tmp559;_tmp55B=_tmp55A.f1;{
struct Cyc_List_List*_tmp55C=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp55B);
_tmp556=_tmp55C;goto _LL35B;};}}else{goto _LL35C;}case 3: _LL35A: _tmp556=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp555)->f1;_LL35B:
# 2544
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp552,_tmp556);
goto _LL355;default: _LL35C: _LL35D:
 goto _LL355;}_LL355:;}
# 2548
goto _LL350;}}else{_LL353: _LL354:
 goto _LL350;}_LL350:;}
# 2551
return;}
# 2557
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2560
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple29*_tmp55E=(struct _tuple29*)x->hd;int _tmp560;struct Cyc_Absyn_Stmt*_tmp561;struct Cyc_List_List*_tmp562;struct Cyc_List_List*_tmp563;struct _tuple29*_tmp55F=_tmp55E;_tmp560=_tmp55F->f1;_tmp561=_tmp55F->f2;_tmp562=_tmp55F->f3;_tmp563=_tmp55F->f4;
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
struct _tuple20 _tmp566=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp568;union Cyc_CfFlowInfo_FlowInfo*_tmp569;struct _tuple20 _tmp567=_tmp566;_tmp568=_tmp567.f1;_tmp569=_tmp567.f2;
inflow=*_tmp569;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp56A=env->fenv;
# 2613
void*_tmp56B=s->r;void*_tmp56C=_tmp56B;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Stmt*_tmp56E;struct Cyc_Absyn_Decl*_tmp56F;struct Cyc_Absyn_Stmt*_tmp570;struct Cyc_Absyn_Vardecl*_tmp571;struct Cyc_Absyn_Exp*_tmp572;unsigned int _tmp573;struct Cyc_Absyn_Stmt*_tmp574;struct Cyc_List_List*_tmp575;struct Cyc_Absyn_Exp*_tmp576;unsigned int _tmp577;struct Cyc_Absyn_Stmt*_tmp578;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_List_List*_tmp57A;void*_tmp57B;struct Cyc_Absyn_Exp*_tmp57C;struct Cyc_List_List*_tmp57D;void*_tmp57E;struct Cyc_Absyn_Stmt*_tmp57F;struct Cyc_Absyn_Stmt*_tmp580;struct Cyc_List_List*_tmp581;struct Cyc_Absyn_Switch_clause*_tmp582;struct Cyc_Absyn_Exp*_tmp583;struct Cyc_Absyn_Exp*_tmp584;struct Cyc_Absyn_Stmt*_tmp585;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Stmt*_tmp587;struct Cyc_Absyn_Stmt*_tmp588;struct Cyc_Absyn_Stmt*_tmp589;struct Cyc_Absyn_Exp*_tmp58A;struct Cyc_Absyn_Stmt*_tmp58B;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Stmt*_tmp58D;struct Cyc_Absyn_Stmt*_tmp58E;struct Cyc_Absyn_Exp*_tmp58F;struct Cyc_Absyn_Stmt*_tmp590;struct Cyc_Absyn_Stmt*_tmp591;struct Cyc_Absyn_Stmt*_tmp592;struct Cyc_Absyn_Stmt*_tmp593;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_Absyn_Stmt*_tmp596;switch(*((int*)_tmp56C)){case 0: _LL35F: _LL360:
 return inflow;case 6: if(((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1 == 0){_LL361: _LL362:
# 2619
{union Cyc_CfFlowInfo_FlowInfo _tmp597=inflow;if((_tmp597.ReachableFL).tag == 2){_LL38C: _LL38D:
# 2621
{void*_tmp598=Cyc_Tcutil_compress(env->return_type);void*_tmp599=_tmp598;switch(*((int*)_tmp599)){case 0: _LL391: _LL392:
 goto _LL390;case 7: _LL393: _LL394:
 goto _LL396;case 6: _LL395: _LL396:
# 2625
{const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,Cyc_Tcutil_warn(s->loc,((_tmp8F3="break may cause function not to return a value",_tag_dyneither(_tmp8F3,sizeof(char),47))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
goto _LL390;default: _LL397: _LL398:
# 2628
{const char*_tmp8F6;void*_tmp8F5;(_tmp8F5=0,Cyc_Tcutil_terr(s->loc,((_tmp8F6="break may cause function not to return a value",_tag_dyneither(_tmp8F6,sizeof(char),47))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}
goto _LL390;}_LL390:;}
# 2631
goto _LL38B;}else{_LL38E: _LL38F:
 goto _LL38B;}_LL38B:;}
# 2634
goto _LL364;}else{_LL375: _tmp596=((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL376:
# 2729
 _tmp580=_tmp596;goto _LL378;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1 == 0){_LL363: _LL364:
# 2636
 if(env->noreturn){
const char*_tmp8F9;void*_tmp8F8;(_tmp8F8=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8F9="`noreturn' function might return",_tag_dyneither(_tmp8F9,sizeof(char),33))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL365: _tmp595=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL366:
# 2642
 if(env->noreturn){
const char*_tmp8FC;void*_tmp8FB;(_tmp8FB=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp8FC="`noreturn' function might return",_tag_dyneither(_tmp8FC,sizeof(char),33))),_tag_dyneither(_tmp8FB,sizeof(void*),0)));}{
struct _tuple16 _tmp5A2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp595));union Cyc_CfFlowInfo_FlowInfo _tmp5A4;void*_tmp5A5;struct _tuple16 _tmp5A3=_tmp5A2;_tmp5A4=_tmp5A3.f1;_tmp5A5=_tmp5A3.f2;
_tmp5A4=Cyc_NewControlFlow_use_Rval(env,_tmp595->loc,_tmp5A4,_tmp5A5);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp5A4);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp5A4,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1: _LL367: _tmp594=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL368: {
# 2651
struct _tuple17*_tmp5A6=rval_opt;void**_tmp5A7;int _tmp5A8;if(_tmp5A6 != 0){_LL39A: _tmp5A7=(void**)& _tmp5A6->f1;_tmp5A8=_tmp5A6->f2;_LL39B: {
# 2653
struct _tuple16 _tmp5A9=Cyc_NewControlFlow_anal_Rexp(env,_tmp5A8,inflow,_tmp594);union Cyc_CfFlowInfo_FlowInfo _tmp5AB;void*_tmp5AC;struct _tuple16 _tmp5AA=_tmp5A9;_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;
*_tmp5A7=_tmp5AC;
return _tmp5AB;}}else{_LL39C: _LL39D:
# 2657
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp594)).f1;}_LL399:;}case 2: _LL369: _tmp592=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp593=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL36A:
# 2661
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp592,0),_tmp593,rval_opt);case 4: _LL36B: _tmp58F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp590=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_tmp591=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3;_LL36C: {
# 2664
struct _tuple19 _tmp5AD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp58F);union Cyc_CfFlowInfo_FlowInfo _tmp5AF;union Cyc_CfFlowInfo_FlowInfo _tmp5B0;struct _tuple19 _tmp5AE=_tmp5AD;_tmp5AF=_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;{
# 2671
union Cyc_CfFlowInfo_FlowInfo _tmp5B1=Cyc_NewControlFlow_anal_stmt(env,_tmp5B0,_tmp591,0);
union Cyc_CfFlowInfo_FlowInfo _tmp5B2=Cyc_NewControlFlow_anal_stmt(env,_tmp5AF,_tmp590,0);
return Cyc_CfFlowInfo_join_flow(_tmp56A,env->all_changed,_tmp5B2,_tmp5B1);};}case 5: _LL36D: _tmp58C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1).f1;_tmp58D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1).f2;_tmp58E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL36E: {
# 2679
struct _tuple20 _tmp5B3=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp58D);union Cyc_CfFlowInfo_FlowInfo*_tmp5B5;struct _tuple20 _tmp5B4=_tmp5B3;_tmp5B5=_tmp5B4.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B6=*_tmp5B5;
struct _tuple19 _tmp5B7=Cyc_NewControlFlow_anal_test(env,_tmp5B6,_tmp58C);union Cyc_CfFlowInfo_FlowInfo _tmp5B9;union Cyc_CfFlowInfo_FlowInfo _tmp5BA;struct _tuple19 _tmp5B8=_tmp5B7;_tmp5B9=_tmp5B8.f1;_tmp5BA=_tmp5B8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5BB=Cyc_NewControlFlow_anal_stmt(env,_tmp5B9,_tmp58E,0);
Cyc_NewControlFlow_update_flow(env,_tmp58D,_tmp5BB);
return _tmp5BA;};};}case 14: _LL36F: _tmp589=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp58A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f1;_tmp58B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f2;_LL370: {
# 2689
union Cyc_CfFlowInfo_FlowInfo _tmp5BC=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp589,0);
struct _tuple20 _tmp5BD=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5BC,_tmp58B);union Cyc_CfFlowInfo_FlowInfo*_tmp5BF;struct _tuple20 _tmp5BE=_tmp5BD;_tmp5BF=_tmp5BE.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C0=*_tmp5BF;
struct _tuple19 _tmp5C1=Cyc_NewControlFlow_anal_test(env,_tmp5C0,_tmp58A);union Cyc_CfFlowInfo_FlowInfo _tmp5C3;union Cyc_CfFlowInfo_FlowInfo _tmp5C4;struct _tuple19 _tmp5C2=_tmp5C1;_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;
Cyc_NewControlFlow_update_flow(env,_tmp589,_tmp5C3);
return _tmp5C4;};}case 9: _LL371: _tmp583=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp584=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f1;_tmp585=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2).f2;_tmp586=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3).f1;_tmp587=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3).f2;_tmp588=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp56C)->f4;_LL372: {
# 2698
union Cyc_CfFlowInfo_FlowInfo _tmp5C5=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp583)).f1;
struct _tuple20 _tmp5C6=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5C5,_tmp585);union Cyc_CfFlowInfo_FlowInfo*_tmp5C8;struct _tuple20 _tmp5C7=_tmp5C6;_tmp5C8=_tmp5C7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5C9=*_tmp5C8;
struct _tuple19 _tmp5CA=Cyc_NewControlFlow_anal_test(env,_tmp5C9,_tmp584);union Cyc_CfFlowInfo_FlowInfo _tmp5CC;union Cyc_CfFlowInfo_FlowInfo _tmp5CD;struct _tuple19 _tmp5CB=_tmp5CA;_tmp5CC=_tmp5CB.f1;_tmp5CD=_tmp5CB.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5CE=Cyc_NewControlFlow_anal_stmt(env,_tmp5CC,_tmp588,0);
struct _tuple20 _tmp5CF=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5CE,_tmp587);union Cyc_CfFlowInfo_FlowInfo*_tmp5D1;struct _tuple20 _tmp5D0=_tmp5CF;_tmp5D1=_tmp5D0.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp5D2=*_tmp5D1;
union Cyc_CfFlowInfo_FlowInfo _tmp5D3=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5D2,_tmp586)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp585,_tmp5D3);
return _tmp5CD;};};};}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2 != 0){_LL373: _tmp581=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp582=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL374: {
# 2710
struct _RegionHandle _tmp5D4=_new_region("temp");struct _RegionHandle*temp=& _tmp5D4;_push_region(temp);
{struct _tuple23 _tmp5D5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp581,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp5D7;struct Cyc_List_List*_tmp5D8;struct _tuple23 _tmp5D6=_tmp5D5;_tmp5D7=_tmp5D6.f1;_tmp5D8=_tmp5D6.f2;
# 2713
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,_tmp582->body);{
# 2715
struct Cyc_List_List*_tmp5D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp582->pat_vars))->v)).f1);
_tmp5D7=Cyc_NewControlFlow_add_vars(_tmp56A,_tmp5D7,_tmp5D9,_tmp56A->unknown_all,s->loc,0);
# 2718
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp582->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp5DA=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DD;struct _tuple24*_tmp5DB=_tmp5DA;_tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;
if(_tmp5DC != 0){
_tmp5D7=Cyc_NewControlFlow_do_initialize_var(_tmp56A,env,_tmp5D7,*_tmp5DC,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp581))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp5D8))->hd,s->loc);
_tmp5D8=_tmp5D8->tl;
_tmp581=_tmp581->tl;}}}
# 2726
Cyc_NewControlFlow_update_flow(env,_tmp582->body,_tmp5D7);{
union Cyc_CfFlowInfo_FlowInfo _tmp5DE=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5DE;};};}
# 2711
;_pop_region(temp);}}else{_LL389: _LL38A: {
# 2888
struct Cyc_Core_Impossible_exn_struct _tmp902;const char*_tmp901;struct Cyc_Core_Impossible_exn_struct*_tmp900;(int)_throw((void*)((_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900[0]=((_tmp902.tag=Cyc_Core_Impossible,((_tmp902.f1=((_tmp901="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp901,sizeof(char),56))),_tmp902)))),_tmp900)))));}}case 7: _LL377: _tmp580=((struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL378:
# 2730
 _tmp57F=_tmp580;goto _LL37A;case 8: _LL379: _tmp57F=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL37A:
# 2734
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp5DF=_tmp568->encloser;
struct Cyc_Absyn_Stmt*_tmp5E0=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp57F)))->encloser;
while(_tmp5E0 != _tmp5DF){
struct Cyc_Absyn_Stmt*_tmp5E1=(Cyc_NewControlFlow_get_stmt_annot(_tmp5DF))->encloser;
if(_tmp5E1 == _tmp5DF){
{const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp905="goto enters local scope or exception handler",_tag_dyneither(_tmp905,sizeof(char),45))),_tag_dyneither(_tmp904,sizeof(void*),0)));}
break;}
# 2743
_tmp5DF=_tmp5E1;}}
# 2747
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp57F));
# 2749
Cyc_NewControlFlow_update_flow(env,_tmp57F,inflow);
return Cyc_CfFlowInfo_BottomFL();case 10: _LL37B: _tmp57C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp57D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_tmp57E=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3;_LL37C:
# 2755
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp57D,_tmp57C->loc);case 15: _LL37D: _tmp579=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp57A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_tmp57B=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp56C)->f3;_LL37E: {
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
{union Cyc_CfFlowInfo_FlowInfo _tmp5E4=scs_outflow;if((_tmp5E4.BottomFL).tag == 1){_LL39F: _LL3A0:
 goto _LL39E;}else{_LL3A1: _LL3A2: {
const char*_tmp908;void*_tmp907;(_tmp907=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp908="last catch clause may implicitly fallthru",_tag_dyneither(_tmp908,sizeof(char),42))),_tag_dyneither(_tmp907,sizeof(void*),0)));}}_LL39E:;}
# 2777
outflow=s1_outflow;
# 2779
return outflow;};};}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f2 != 0){_LL37F: _tmp575=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f2)->v;_tmp576=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f3;_tmp577=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->loc;_tmp578=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL380: {
# 2789
struct _tuple1 _tmp5E7=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp575);struct Cyc_List_List*_tmp5E9;struct Cyc_List_List*_tmp5EA;struct _tuple1 _tmp5E8=_tmp5E7;_tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp56A,env,inflow,_tmp575,_tmp5E9 != 0,_tmp577,_tmp576->loc);{
struct Cyc_List_List*_tmp5EB=env->unique_pat_vars;
{struct _tuple29*_tmp90B;struct Cyc_List_List*_tmp90A;env->unique_pat_vars=(
(_tmp90A=_region_malloc(env->r,sizeof(*_tmp90A)),((_tmp90A->hd=((_tmp90B=_region_malloc(env->r,sizeof(*_tmp90B)),((_tmp90B->f1=0,((_tmp90B->f2=s,((_tmp90B->f3=_tmp5E9,((_tmp90B->f4=_tmp5EA,_tmp90B)))))))))),((_tmp90A->tl=_tmp5EB,_tmp90A))))));}
# 2797
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp578,rval_opt);
env->unique_pat_vars=_tmp5EB;
# 2801
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5E9,_tmp5EA,0,inflow,_tmp577);
# 2805
return inflow;};}}else{goto _LL383;}case 4: _LL381: _tmp571=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f2;_tmp572=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->r)->f4;_tmp573=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1)->loc;_tmp574=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;if(_tmp572 != 0){_LL382: {
# 2817
struct Cyc_List_List _tmp90C;struct Cyc_List_List l=(_tmp90C.hd=_tmp572,((_tmp90C.tl=0,_tmp90C)));
union Cyc_CfFlowInfo_FlowInfo _tmp5EE=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5EF=Cyc_NewControlFlow_anal_Lexp(env,_tmp5EE,0,0,_tmp572);union Cyc_CfFlowInfo_AbsLVal _tmp5F1;struct _tuple18 _tmp5F0=_tmp5EF;_tmp5F1=_tmp5F0.f2;{
struct _tuple16 _tmp5F2=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5EE,_tmp572);union Cyc_CfFlowInfo_FlowInfo _tmp5F4;struct _tuple16 _tmp5F3=_tmp5F2;_tmp5F4=_tmp5F3.f1;{
struct Cyc_List_List*_tmp5F5=0;
struct Cyc_List_List*_tmp5F6=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5F7=_tmp5F4;struct Cyc_Dict_Dict _tmp5F8;struct Cyc_List_List*_tmp5F9;if((_tmp5F7.ReachableFL).tag == 2){_LL3A4: _tmp5F8=((_tmp5F7.ReachableFL).val).f1;_tmp5F9=((_tmp5F7.ReachableFL).val).f2;_LL3A5:
# 2825
{union Cyc_CfFlowInfo_AbsLVal _tmp5FA=_tmp5F1;struct Cyc_CfFlowInfo_Place*_tmp5FB;if((_tmp5FA.PlaceL).tag == 1){_LL3A9: _tmp5FB=(_tmp5FA.PlaceL).val;_LL3AA: {
# 2829
void*_tmp5FC=Cyc_CfFlowInfo_lookup_place(_tmp5F8,_tmp5FB);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp90F;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp90E;_tmp5FC=(void*)((_tmp90E=_region_malloc(_tmp56A->r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=8,((_tmp90F.f1=_tmp571,((_tmp90F.f2=_tmp5FC,_tmp90F)))))),_tmp90E))));}
_tmp5F8=Cyc_CfFlowInfo_assign_place(_tmp56A,_tmp572->loc,_tmp5F8,env->all_changed,_tmp5FB,_tmp5FC);
# 2833
_tmp5F4=Cyc_CfFlowInfo_ReachableFL(_tmp5F8,_tmp5F9);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp912;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp911;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5FF=(_tmp911=_region_malloc(_tmp56A->r,sizeof(*_tmp911)),((_tmp911[0]=((_tmp912.tag=0,((_tmp912.f1=_tmp571,_tmp912)))),_tmp911)));
struct Cyc_CfFlowInfo_Place*_tmp913;struct Cyc_CfFlowInfo_Place*_tmp600=(_tmp913=_region_malloc(_tmp56A->r,sizeof(*_tmp913)),((_tmp913->root=(void*)_tmp5FF,((_tmp913->fields=0,_tmp913)))));
{struct Cyc_List_List*_tmp914;_tmp5F5=((_tmp914=_region_malloc(_tmp56A->r,sizeof(*_tmp914)),((_tmp914->hd=_tmp600,((_tmp914->tl=_tmp5F5,_tmp914))))));}
{struct Cyc_List_List*_tmp915;_tmp5F6=((_tmp915=_region_malloc(_tmp56A->r,sizeof(*_tmp915)),((_tmp915->hd=_tmp572,((_tmp915->tl=_tmp5F6,_tmp915))))));}
goto _LL3A8;};}}else{_LL3AB: _LL3AC:
# 2845
 goto _LL3A8;}_LL3A8:;}
# 2847
goto _LL3A3;}else{_LL3A6: _LL3A7:
# 2849
 goto _LL3A3;}_LL3A3:;}{
# 2852
struct Cyc_List_List _tmp916;struct Cyc_List_List _tmp606=(_tmp916.hd=_tmp571,((_tmp916.tl=0,_tmp916)));
_tmp5F4=Cyc_NewControlFlow_add_vars(_tmp56A,_tmp5F4,& _tmp606,_tmp56A->unknown_all,_tmp573,0);{
# 2855
struct Cyc_List_List*_tmp607=env->unique_pat_vars;
{struct _tuple29*_tmp919;struct Cyc_List_List*_tmp918;env->unique_pat_vars=(
(_tmp918=_region_malloc(env->r,sizeof(*_tmp918)),((_tmp918->hd=((_tmp919=_region_malloc(env->r,sizeof(*_tmp919)),((_tmp919->f1=1,((_tmp919->f2=s,((_tmp919->f3=_tmp5F5,((_tmp919->f4=_tmp5F6,_tmp919)))))))))),((_tmp918->tl=_tmp607,_tmp918))))));}
# 2861
_tmp5F4=Cyc_NewControlFlow_anal_stmt(env,_tmp5F4,_tmp574,rval_opt);
env->unique_pat_vars=_tmp607;
# 2865
_tmp5F4=Cyc_NewControlFlow_unconsume_params(env,_tmp5F5,_tmp5F6,1,_tmp5F4,_tmp573);
# 2869
return _tmp5F4;};};};};}}else{goto _LL383;}default: _LL383: _tmp56F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp570=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL384:
# 2872
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp56F),_tmp570,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp56F);
return outflow;}case 13: _LL385: _tmp56E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL386:
# 2877
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp56E,rval_opt);default: _LL387: _tmp56D=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL388: {
# 2880
struct _tuple16 _tmp60C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp56D);union Cyc_CfFlowInfo_FlowInfo _tmp60E;void*_tmp60F;struct _tuple16 _tmp60D=_tmp60C;_tmp60E=_tmp60D.f1;_tmp60F=_tmp60D.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp610=Cyc_NewControlFlow_use_Rval(env,_tmp56D->loc,_tmp60E,_tmp60F);
void*_tmp611=Cyc_Tcutil_compress((void*)_check_null(_tmp56D->topt));void*_tmp612=_tmp611;void*_tmp613;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp612)->tag == 15){_LL3AE: _tmp613=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp612)->f1;_LL3AF:
# 2884
 return Cyc_CfFlowInfo_kill_flow_region(_tmp56A,_tmp60E,_tmp613);}else{_LL3B0: _LL3B1: {
struct Cyc_Core_Impossible_exn_struct _tmp91F;const char*_tmp91E;struct Cyc_Core_Impossible_exn_struct*_tmp91D;(int)_throw((void*)((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D[0]=((_tmp91F.tag=Cyc_Core_Impossible,((_tmp91F.f1=((_tmp91E="anal_stmt -- reset_region",_tag_dyneither(_tmp91E,sizeof(char),26))),_tmp91F)))),_tmp91D)))));}}_LL3AD:;};}}_LL35E:;};}
# 2893
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2896
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp61A=env->fenv;
void*_tmp61B=decl->r;void*_tmp61C=_tmp61B;struct Cyc_Absyn_Tvar*_tmp61D;struct Cyc_Absyn_Vardecl*_tmp61E;int _tmp61F;struct Cyc_Absyn_Exp*_tmp620;struct Cyc_Absyn_Fndecl*_tmp621;struct Cyc_List_List*_tmp622;struct Cyc_Absyn_Vardecl*_tmp623;switch(*((int*)_tmp61C)){case 0: _LL3B3: _tmp623=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_LL3B4: {
# 2906
struct Cyc_List_List _tmp920;struct Cyc_List_List _tmp624=(_tmp920.hd=_tmp623,((_tmp920.tl=0,_tmp920)));
inflow=Cyc_NewControlFlow_add_vars(_tmp61A,inflow,& _tmp624,_tmp61A->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp625=_tmp623->initializer;
if(_tmp625 == 0)
return inflow;{
struct _tuple16 _tmp626=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp625);union Cyc_CfFlowInfo_FlowInfo _tmp628;void*_tmp629;struct _tuple16 _tmp627=_tmp626;_tmp628=_tmp627.f1;_tmp629=_tmp627.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp61A,env,_tmp628,_tmp623,_tmp625,_tmp629,decl->loc);};};}case 3: _LL3B5: _tmp622=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_LL3B6:
# 2915
 return Cyc_NewControlFlow_add_vars(_tmp61A,inflow,_tmp622,_tmp61A->unknown_none,decl->loc,0);case 1: _LL3B7: _tmp621=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_LL3B8:
# 2918
 Cyc_NewControlFlow_check_nested_fun(_tmp61A,inflow,_tmp621);{
void*t=(void*)_check_null(_tmp621->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp62B=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp621->fn_vardecl);
# 2924
struct Cyc_List_List*_tmp921;return Cyc_NewControlFlow_add_vars(_tmp61A,inflow,((_tmp921=_region_malloc(env->r,sizeof(*_tmp921)),((_tmp921->hd=_tmp62B,((_tmp921->tl=0,_tmp921)))))),_tmp61A->unknown_all,decl->loc,0);};case 4: _LL3B9: _tmp61D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f1;_tmp61E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f2;_tmp61F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f3;_tmp620=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61C)->f4;_LL3BA:
# 2927
 if(_tmp620 != 0){
struct Cyc_Core_Impossible_exn_struct _tmp927;const char*_tmp926;struct Cyc_Core_Impossible_exn_struct*_tmp925;(int)_throw((void*)((_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925[0]=((_tmp927.tag=Cyc_Core_Impossible,((_tmp927.f1=((_tmp926="found open expression in declaration!",_tag_dyneither(_tmp926,sizeof(char),38))),_tmp927)))),_tmp925)))));}{
struct Cyc_List_List _tmp928;struct Cyc_List_List _tmp630=(_tmp928.hd=_tmp61E,((_tmp928.tl=0,_tmp928)));
return Cyc_NewControlFlow_add_vars(_tmp61A,inflow,& _tmp630,_tmp61A->unknown_all,decl->loc,0);};default: _LL3BB: _LL3BC: {
# 2933
struct Cyc_Core_Impossible_exn_struct _tmp92E;const char*_tmp92D;struct Cyc_Core_Impossible_exn_struct*_tmp92C;(int)_throw((void*)((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92E.tag=Cyc_Core_Impossible,((_tmp92E.f1=((_tmp92D="anal_decl: unexpected decl variant",_tag_dyneither(_tmp92D,sizeof(char),35))),_tmp92E)))),_tmp92C)))));}}_LL3B2:;}
# 2941
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp635=_new_region("frgn");struct _RegionHandle*frgn=& _tmp635;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2943
;_pop_region(frgn);}
# 2948
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2953
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp636=r;unsigned int _tmp637;if((_tmp636.RParam).tag == 5){_LL3BE: _tmp637=(_tmp636.RParam).val;_LL3BF: {
# 2956
struct Cyc_Absyn_Vardecl*_tmp638=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp637);
if(!_tmp638->escapes)
return Cyc_Relations_RVar(_tmp638);
return r;}}else{_LL3C0: _LL3C1:
 return r;}_LL3BD:;}
# 2964
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2968
struct _RegionHandle*_tmp639=fenv->r;
unsigned int _tmp63A=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp63A,1);{
# 2974
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
union Cyc_CfFlowInfo_FlowInfo _tmp92F;struct _tuple14 _tmp63B=(_tmp92F=inflow,(((_tmp92F.ReachableFL).tag != 2?_throw_match(): 1,(_tmp92F.ReachableFL).val)));struct Cyc_Dict_Dict _tmp63D;struct Cyc_List_List*_tmp63E;struct _tuple14 _tmp63C=_tmp63B;_tmp63D=_tmp63C.f1;_tmp63E=_tmp63C.f2;{
# 2979
struct Cyc_List_List*_tmp63F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp640=fd->requires_relns;for(0;_tmp640 != 0;_tmp640=_tmp640->tl){
struct Cyc_Relations_Reln*_tmp641=(struct Cyc_Relations_Reln*)_tmp640->hd;
_tmp63E=Cyc_Relations_add_relation(_tmp639,Cyc_NewControlFlow_translate_rop(_tmp63F,_tmp641->rop1),_tmp641->relation,
Cyc_NewControlFlow_translate_rop(_tmp63F,_tmp641->rop2),_tmp63E);}}{
# 2988
struct Cyc_List_List*atts;
{void*_tmp642=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp643=_tmp642;struct Cyc_List_List*_tmp644;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp643)->tag == 9){_LL3C3: _tmp644=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp643)->f1).attributes;_LL3C4:
 atts=_tmp644;goto _LL3C2;}else{_LL3C5: _LL3C6: {
const char*_tmp932;void*_tmp931;(_tmp931=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp932="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp932,sizeof(char),50))),_tag_dyneither(_tmp931,sizeof(void*),0)));}}_LL3C2:;}
# 2995
for(0;atts != 0;atts=atts->tl){
void*_tmp647=(void*)atts->hd;void*_tmp648=_tmp647;int _tmp649;int _tmp64A;int _tmp64B;switch(*((int*)_tmp648)){case 21: _LL3C8: _tmp64B=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp648)->f1;_LL3C9: {
# 2998
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp64C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp64B - 1);
void*t=Cyc_Tcutil_compress(_tmp64C->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp64D=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp935;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp934;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp64E=(_tmp934=_region_malloc(_tmp639,sizeof(*_tmp934)),((_tmp934[0]=((_tmp935.tag=2,((_tmp935.f1=_tmp64B,((_tmp935.f2=t,_tmp935)))))),_tmp934)));
struct Cyc_CfFlowInfo_Place*_tmp936;struct Cyc_CfFlowInfo_Place*_tmp64F=(_tmp936=_region_malloc(_tmp639,sizeof(*_tmp936)),((_tmp936->root=(void*)_tmp64E,((_tmp936->fields=0,_tmp936)))));
_tmp63D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63D,(void*)_tmp64E,_tmp64D);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp93C;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp93B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp939;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp938;_tmp63D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63D,(void*)((_tmp938=_region_malloc(_tmp639,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=0,((_tmp939.f1=_tmp64C,_tmp939)))),_tmp938)))),(void*)((_tmp93B=_region_malloc(_tmp639,sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93C.tag=5,((_tmp93C.f1=_tmp64F,_tmp93C)))),_tmp93B)))));}
goto _LL3C7;}case 20: _LL3CA: _tmp64A=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp648)->f1;_LL3CB: {
# 3011
struct Cyc_Absyn_Vardecl*_tmp657=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp64A - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp657->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp93F;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp93E;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp658=(_tmp93E=_region_malloc(_tmp639,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=2,((_tmp93F.f1=_tmp64A,((_tmp93F.f2=elttype,_tmp93F)))))),_tmp93E)));
struct Cyc_CfFlowInfo_Place*_tmp940;struct Cyc_CfFlowInfo_Place*_tmp659=(_tmp940=_region_malloc(_tmp639,sizeof(*_tmp940)),((_tmp940->root=(void*)_tmp658,((_tmp940->fields=0,_tmp940)))));
_tmp63D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63D,(void*)_tmp658,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp946;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp945;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp943;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp942;_tmp63D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp63D,(void*)((_tmp942=_region_malloc(_tmp639,sizeof(*_tmp942)),((_tmp942[0]=((_tmp943.tag=0,((_tmp943.f1=_tmp657,_tmp943)))),_tmp942)))),(void*)((_tmp945=_region_malloc(_tmp639,sizeof(*_tmp945)),((_tmp945[0]=((_tmp946.tag=5,((_tmp946.f1=_tmp659,_tmp946)))),_tmp945)))));}
{struct Cyc_List_List*_tmp947;param_roots=((_tmp947=_region_malloc(_tmp639,sizeof(*_tmp947)),((_tmp947->hd=_tmp659,((_tmp947->tl=param_roots,_tmp947))))));}
goto _LL3C7;}case 22: _LL3CC: _tmp649=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp648)->f1;_LL3CD: {
# 3020
struct Cyc_Absyn_Vardecl*_tmp662=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp649 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp662->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp94A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp949;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp663=(_tmp949=_region_malloc(_tmp639,sizeof(*_tmp949)),((_tmp949[0]=((_tmp94A.tag=0,((_tmp94A.f1=_tmp662,_tmp94A)))),_tmp949)));
struct Cyc_CfFlowInfo_Place*_tmp94B;struct Cyc_CfFlowInfo_Place*_tmp664=(_tmp94B=_region_malloc(_tmp639,sizeof(*_tmp94B)),((_tmp94B->root=(void*)_tmp663,((_tmp94B->fields=0,_tmp94B)))));
struct Cyc_List_List*_tmp94C;noconsume_roots=((_tmp94C=_region_malloc(_tmp639,sizeof(*_tmp94C)),((_tmp94C->hd=_tmp664,((_tmp94C->tl=noconsume_roots,_tmp94C))))));}
# 3026
goto _LL3C7;}default: _LL3CE: _LL3CF:
 goto _LL3C7;}_LL3C7:;}
# 3029
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp63D,_tmp63E);{
# 3031
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp639,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp94D;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp94D=_region_malloc(_tmp639,sizeof(*_tmp94D)),((_tmp94D->r=_tmp639,((_tmp94D->fenv=fenv,((_tmp94D->iterate_again=1,((_tmp94D->iteration_num=0,((_tmp94D->in_try=0,((_tmp94D->tryflow=inflow,((_tmp94D->all_changed=0,((_tmp94D->noreturn=noreturn,((_tmp94D->return_type=fd->ret_type,((_tmp94D->unique_pat_vars=0,((_tmp94D->param_roots=param_roots,((_tmp94D->noconsume_params=noconsume_roots,((_tmp94D->flow_table=flow_table,((_tmp94D->return_relns=fd->ensures_relns,_tmp94D)))))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3041
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3044
union Cyc_CfFlowInfo_FlowInfo _tmp669=outflow;if((_tmp669.BottomFL).tag == 1){_LL3D1: _LL3D2:
 goto _LL3D0;}else{_LL3D3: _LL3D4:
# 3047
 Cyc_NewControlFlow_check_init_params(_tmp63A,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp63A);
# 3051
if(noreturn){
const char*_tmp950;void*_tmp94F;(_tmp94F=0,Cyc_CfFlowInfo_aerr(_tmp63A,((_tmp950="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp950,sizeof(char),46))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}else{
# 3054
void*_tmp66C=Cyc_Tcutil_compress(fd->ret_type);void*_tmp66D=_tmp66C;switch(*((int*)_tmp66D)){case 0: _LL3D6: _LL3D7:
 goto _LL3D5;case 7: _LL3D8: _LL3D9:
 goto _LL3DB;case 6: _LL3DA: _LL3DB:
# 3058
{const char*_tmp953;void*_tmp952;(_tmp952=0,Cyc_Tcutil_warn(_tmp63A,((_tmp953="function may not return a value",_tag_dyneither(_tmp953,sizeof(char),32))),_tag_dyneither(_tmp952,sizeof(void*),0)));}goto _LL3D5;default: _LL3DC: _LL3DD:
# 3060
{const char*_tmp956;void*_tmp955;(_tmp955=0,Cyc_CfFlowInfo_aerr(_tmp63A,((_tmp956="function may not return a value",_tag_dyneither(_tmp956,sizeof(char),32))),_tag_dyneither(_tmp955,sizeof(void*),0)));}goto _LL3D5;}_LL3D5:;}
# 3062
goto _LL3D0;}_LL3D0:;};};};};};}
# 3066
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp674=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp675=_tmp674;struct Cyc_List_List*_tmp676;struct Cyc_List_List*_tmp677;struct Cyc_List_List*_tmp678;struct Cyc_Absyn_Fndecl*_tmp679;switch(*((int*)_tmp675)){case 1: _LL3DF: _tmp679=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp675)->f1;_LL3E0:
 Cyc_NewControlFlow_check_fun(_tmp679);goto _LL3DE;case 11: _LL3E1: _tmp678=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp675)->f1;_LL3E2:
 _tmp677=_tmp678;goto _LL3E4;case 10: _LL3E3: _tmp677=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp675)->f2;_LL3E4:
 _tmp676=_tmp677;goto _LL3E6;case 9: _LL3E5: _tmp676=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp675)->f2;_LL3E6:
 Cyc_NewControlFlow_cf_check(_tmp676);goto _LL3DE;case 12: _LL3E7: _LL3E8:
 goto _LL3DE;default: _LL3E9: _LL3EA:
 goto _LL3DE;}_LL3DE:;};}}
