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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
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
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 107 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 55
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 333
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 350
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 364
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 444 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 498
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 505
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 525
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 551
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 558
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 576
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 740 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 912
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 939
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 943
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 947
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 957
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 962
void*Cyc_Absyn_compress_kb(void*);
# 967
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 969
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 972
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 974
extern void*Cyc_Absyn_sint_typ;
# 976
void*Cyc_Absyn_float_typ(int);
# 1024
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1091
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1099
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1104
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1117
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1120
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1123
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1127
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1132
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1134
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1141
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1150
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1155
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1161
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1186
int Cyc_Absyn_fntype_att(void*a);
# 1206
extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 122
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 144
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 309 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 74 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();
void Cyc_Lex_enter_extern_c();
void Cyc_Lex_leave_extern_c();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 96 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 98
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 101
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 110
return first;}}
# 115
int Cyc_Parse_no_register=0;struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 128
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 130
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
# 133
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 142
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 148
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;
# 152
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 156
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;struct _tuple11 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
{struct _tuple10*_tmp88C;declarators=((_tmp88C=_region_malloc(r,sizeof(*_tmp88C)),((_tmp88C->tl=declarators,((_tmp88C->hd=_tmp2,_tmp88C))))));}{
struct Cyc_List_List*_tmp88D;exprs=((_tmp88D=_region_malloc(r,sizeof(*_tmp88D)),((_tmp88D->hd=_tmp3,((_tmp88D->tl=exprs,_tmp88D))))));};}
# 163
*es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);
*decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 170
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 173
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 175
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 180
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 183
static void Cyc_Parse_err(struct _dyneither_ptr msg,unsigned int sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}
# 186
static void*Cyc_Parse_parse_abort(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 188
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_exn_struct _tmp890;struct Cyc_Position_Exit_exn_struct*_tmp88F;(int)_throw((void*)((_tmp88F=_cycalloc_atomic(sizeof(*_tmp88F)),((_tmp88F[0]=((_tmp890.tag=Cyc_Position_Exit,_tmp890)),_tmp88F)))));};}
# 191
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp895;void*_tmp894[2];struct Cyc_String_pa_PrintArg_struct _tmp893;struct Cyc_String_pa_PrintArg_struct _tmp892;(_tmp892.tag=0,((_tmp892.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp893.tag=0,((_tmp893.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp894[0]=& _tmp893,((_tmp894[1]=& _tmp892,Cyc_fprintf(Cyc_stderr,((_tmp895="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp895,sizeof(char),46))),_tag_dyneither(_tmp894,sizeof(void*),2)))))))))))));}
return;}
# 197
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 199
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp898;void*_tmp897;(_tmp897=0,Cyc_Tcutil_warn(loc,((_tmp898="qualifier on type is ignored",_tag_dyneither(_tmp898,sizeof(char),29))),_tag_dyneither(_tmp897,sizeof(void*),0)));};}
# 204
return _tmpE;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 207
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 213
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp11=(void*)pqs->hd;void*_tmp12=_tmp11;struct Cyc_Absyn_Exp*_tmp1A;void*_tmp1C;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp13=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp13->tag != 4)goto _LL3;}_LL2:
# 217
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp14=(struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp14->tag != 5)goto _LL5;}_LL4:
# 219
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp15=(struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp15->tag != 7)goto _LL7;}_LL6:
# 221
 nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp16=(struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp16->tag != 6)goto _LL9;}_LL8:
# 223
 nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp17=(struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp17->tag != 3)goto _LLB;}_LLA:
# 225
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp18=(struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp18->tag != 2)goto _LLD;}_LLC:
# 227
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp19->tag != 0)goto _LLF;else{_tmp1A=_tmp19->f1;}}_LLE:
# 229
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp89B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp89A;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp89A=_cycalloc(sizeof(*_tmp89A)),((_tmp89A[0]=((_tmp89B.tag=1,((_tmp89B.f1=_tmp1A,_tmp89B)))),_tmp89A)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 231
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 234
struct _tuple14 _tmp89C;return(_tmp89C.f1=nullable,((_tmp89C.f2=bound,((_tmp89C.f3=zeroterm,((_tmp89C.f4=rgn,_tmp89C)))))));};}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
struct _dyneither_ptr*_tmp8A9;const char*_tmp8A8;void*_tmp8A7[1];struct Cyc_Int_pa_PrintArg_struct _tmp8A6;struct _tuple0*_tmp8A5;return(_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5->f1=Cyc_Absyn_Rel_n(0),((_tmp8A5->f2=(
(_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9[0]=(struct _dyneither_ptr)((_tmp8A6.tag=1,((_tmp8A6.f1=(unsigned long)enum_counter ++,((_tmp8A7[0]=& _tmp8A6,Cyc_aprintf(((_tmp8A8="__anonymous_enum_%d__",_tag_dyneither(_tmp8A8,sizeof(char),22))),_tag_dyneither(_tmp8A7,sizeof(void*),1)))))))),_tmp8A9)))),_tmp8A5)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp8AA;Cyc_Parse_err(((_tmp8AA="bad type params in struct field",_tag_dyneither(_tmp8AA,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp8AB;Cyc_Parse_err(((_tmp8AB="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp8AB,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp8AC;return(_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC->name=(*_tmp26).f2,((_tmp8AC->tq=_tmp27,((_tmp8AC->type=_tmp28,((_tmp8AC->width=_tmp2B,((_tmp8AC->attributes=_tmp2A,((_tmp8AC->requires_clause=_tmp2C,_tmp8AC)))))))))))));};}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp8AD;return(_tmp8AD.Signed_spec=0,((_tmp8AD.Unsigned_spec=0,((_tmp8AD.Short_spec=0,((_tmp8AD.Long_spec=0,((_tmp8AD.Long_Long_spec=0,((_tmp8AD.Valid_type_spec=0,((_tmp8AD.Type_spec=Cyc_Absyn_sint_typ,((_tmp8AD.loc=loc,_tmp8AD)))))))))))))));}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp31=Cyc_Parse_empty_spec(loc);
_tmp31.Type_spec=t;
_tmp31.Valid_type_spec=1;
return _tmp31;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
_tmp32.Signed_spec=1;
return _tmp32;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp33=Cyc_Parse_empty_spec(loc);
_tmp33.Unsigned_spec=1;
return _tmp33;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp34=Cyc_Parse_empty_spec(loc);
_tmp34.Short_spec=1;
return _tmp34;}
# 294
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp35=Cyc_Parse_empty_spec(loc);
_tmp35.Long_spec=1;
return _tmp35;}
# 301
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 303
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 316 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp36=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp37=(struct _tuple8*)x->hd;struct _tuple8*_tmp38=_tmp37;struct _dyneither_ptr*_tmp39;void*_tmp3B;struct _dyneither_ptr _tmp3C;void**_tmp3F;_LL12: _tmp39=_tmp38->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3A->tag != 19)goto _LL14;else{_tmp3B=(void*)_tmp3A->f1;}};if(!(_tmp39 != 0))goto _LL14;_LL13:
# 321
{void*_tmp40=_tmp3B;void**_tmp42;_LL19: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40;if(_tmp41->tag != 1)goto _LL1B;else{_tmp42=(void**)((void**)& _tmp41->f2);}}_LL1A: {
# 325
struct Cyc_String_pa_PrintArg_struct _tmp8B5;void*_tmp8B4[1];const char*_tmp8B3;struct _dyneither_ptr*_tmp8B2;struct _dyneither_ptr*nm=(_tmp8B2=_cycalloc(sizeof(*_tmp8B2)),((_tmp8B2[0]=(struct _dyneither_ptr)((_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp8B4[0]=& _tmp8B5,Cyc_aprintf(((_tmp8B3="`%s",_tag_dyneither(_tmp8B3,sizeof(char),4))),_tag_dyneither(_tmp8B4,sizeof(void*),1)))))))),_tmp8B2)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8C4;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8C3;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8C2;struct Cyc_Absyn_Tvar*_tmp8C1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8C0;*_tmp42=(void*)((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C4.tag=2,((_tmp8C4.f1=((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1->name=nm,((_tmp8C1->identity=- 1,((_tmp8C1->kind=(void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C2.tag=0,((_tmp8C2.f1=& Cyc_Tcutil_ik,_tmp8C2)))),_tmp8C3)))),_tmp8C1)))))))),_tmp8C4)))),_tmp8C0))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 330
{struct _tuple18*_tmp8C7;struct Cyc_List_List*_tmp8C6;_tmp36=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6->hd=((_tmp8C7=_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7->f1=_tmp39,((_tmp8C7->f2=_tmp3B,_tmp8C7)))))),((_tmp8C6->tl=_tmp36,_tmp8C6))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 334
struct Cyc_String_pa_PrintArg_struct _tmp8CF;void*_tmp8CE[1];const char*_tmp8CD;struct _dyneither_ptr*_tmp8CC;struct _dyneither_ptr*nm=(_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),((_tmp8CC[0]=(struct _dyneither_ptr)((_tmp8CF.tag=0,((_tmp8CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp8CE[0]=& _tmp8CF,Cyc_aprintf(((_tmp8CD="`%s",_tag_dyneither(_tmp8CD,sizeof(char),4))),_tag_dyneither(_tmp8CE,sizeof(void*),1)))))))),_tmp8CC)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8DE;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8DD;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8DC;struct Cyc_Absyn_Tvar*_tmp8DB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8DA;*_tmp3F=(void*)((_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DE.tag=2,((_tmp8DE.f1=((_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB->name=nm,((_tmp8DB->identity=- 1,((_tmp8DB->kind=(void*)((_tmp8DD=_cycalloc(sizeof(*_tmp8DD)),((_tmp8DD[0]=((_tmp8DC.tag=0,((_tmp8DC.f1=& Cyc_Tcutil_rk,_tmp8DC)))),_tmp8DD)))),_tmp8DB)))))))),_tmp8DE)))),_tmp8DA))));}
goto _LL11;}_LL16:;_LL17:
 goto _LL11;_LL11:;}
# 340
return _tmp36;}
# 344
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp57=0;
for(0;x != 0;x=x->tl){
void*_tmp58=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp59=_tmp58;void*_tmp5B;_LL1E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59;if(_tmp5A->tag != 19)goto _LL20;else{_tmp5B=(void*)_tmp5A->f1;}}_LL1F:
# 349
{struct _tuple18*_tmp8E1;struct Cyc_List_List*_tmp8E0;_tmp57=((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0->hd=((_tmp8E1=_cycalloc(sizeof(*_tmp8E1)),((_tmp8E1->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8E1->f2=_tmp5B,_tmp8E1)))))),((_tmp8E0->tl=_tmp57,_tmp8E0))))));}goto _LL1D;_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 353
return _tmp57;}
# 357
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp5E=e->r;void*_tmp5F=_tmp5E;struct _dyneither_ptr*_tmp61;_LL23: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5F;if(_tmp60->tag != 1)goto _LL25;else{if((((_tmp60->f1)->f1).Rel_n).tag != 1)goto _LL25;if((struct Cyc_List_List*)(((_tmp60->f1)->f1).Rel_n).val != 0)goto _LL25;_tmp61=(_tmp60->f1)->f2;}}_LL24:
# 360
{struct Cyc_List_List*_tmp62=tags;for(0;_tmp62 != 0;_tmp62=_tmp62->tl){
struct _tuple18*_tmp63=(struct _tuple18*)_tmp62->hd;struct _dyneither_ptr*_tmp65;void*_tmp66;struct _tuple18*_tmp64=_tmp63;_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;
if(Cyc_strptrcmp(_tmp65,_tmp61)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8E4;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8E3;return Cyc_Absyn_new_exp((void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=38,((_tmp8E4.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8E4)))),_tmp8E3)))),e->loc);}}}
# 365
goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 368
return e;}
# 373
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp69=t;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_Exp*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;unsigned int _tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp69;if(_tmp6A->tag != 8)goto _LL2A;else{_tmp6B=(_tmp6A->f1).elt_type;_tmp6C=(_tmp6A->f1).tq;_tmp6D=(_tmp6A->f1).num_elts;_tmp6E=(_tmp6A->f1).zero_term;_tmp6F=(_tmp6A->f1).zt_loc;}}_LL29: {
# 376
struct Cyc_Absyn_Exp*nelts2=_tmp6D;
if(_tmp6D != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp6D);{
# 380
void*_tmp78=Cyc_Parse_substitute_tags(tags,_tmp6B);
if(_tmp6D != nelts2  || _tmp6B != _tmp78){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8EA;struct Cyc_Absyn_ArrayInfo _tmp8E9;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8E8;return(void*)((_tmp8E8=_cycalloc(sizeof(*_tmp8E8)),((_tmp8E8[0]=((_tmp8EA.tag=8,((_tmp8EA.f1=((_tmp8E9.elt_type=_tmp78,((_tmp8E9.tq=_tmp6C,((_tmp8E9.num_elts=nelts2,((_tmp8E9.zero_term=_tmp6E,((_tmp8E9.zt_loc=_tmp6F,_tmp8E9)))))))))),_tmp8EA)))),_tmp8E8))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 385
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 389
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8EF;union Cyc_Absyn_Constraint*_tmp8EE;_tmp7D=((_tmp8EE=_cycalloc(sizeof(*_tmp8EE)),(((_tmp8EE->Eq_constr).val=(void*)((_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8EF.tag=1,((_tmp8EF.f1=_tmp81,_tmp8EF)))),_tmp8F0)))),(((_tmp8EE->Eq_constr).tag=1,_tmp8EE))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 394
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8F6;struct Cyc_Absyn_PtrInfo _tmp8F5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8F4;return(void*)((_tmp8F4=_cycalloc(sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F6.tag=5,((_tmp8F6.f1=((_tmp8F5.elt_typ=_tmp7C,((_tmp8F5.elt_tq=_tmp72,((_tmp8F5.ptr_atts=(((_tmp8F5.ptr_atts).rgn=_tmp73,(((_tmp8F5.ptr_atts).nullable=_tmp74,(((_tmp8F5.ptr_atts).bounds=_tmp7D,(((_tmp8F5.ptr_atts).zero_term=_tmp76,(((_tmp8F5.ptr_atts).ptrloc=_tmp77,_tmp8F5.ptr_atts)))))))))),_tmp8F5)))))),_tmp8F6)))),_tmp8F4))));}
goto _LL27;}_LL2C:;_LL2D:
# 399
 goto _LL27;_LL27:;}
# 401
return t;}
# 406
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 413
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 415
struct _tuple19*_tmp8F7;return(_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7->f1=(*t).f2,((_tmp8F7->f2=(*t).f3,_tmp8F7)))));}
# 418
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8FA="bad declaration in `forarray' statement",_tag_dyneither(_tmp8FA,sizeof(char),40))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}_LL33:;}
# 425
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 434
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8FB;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8FB="`H",_tag_dyneither(_tmp8FB,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8FC;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8FC="`U",_tag_dyneither(_tmp8FC,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8FD;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8FD="`RC",_tag_dyneither(_tmp8FD,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 442
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp907;struct _dyneither_ptr*_tmp906;struct Cyc_Absyn_Tvar*_tmp905;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp904;return(void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904[0]=((_tmp907.tag=2,((_tmp907.f1=((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905->name=((_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906[0]=s,_tmp906)))),((_tmp905->identity=- 1,((_tmp905->kind=k,_tmp905)))))))),_tmp907)))),_tmp904))));}}}}
# 445
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp90A;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp909;k=(void*)((_tmp909=_cycalloc(sizeof(*_tmp909)),((_tmp909[0]=((_tmp90A.tag=1,((_tmp90A.f1=0,_tmp90A)))),_tmp909))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp90D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp90C;k=(void*)((_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C[0]=((_tmp90D.tag=2,((_tmp90D.f1=0,((_tmp90D.f2=_tmp9C,_tmp90D)))))),_tmp90C))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 452
struct Cyc_Absyn_Tvar*_tmp90E;return(_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E->name=t->name,((_tmp90E->identity=- 1,((_tmp90E->kind=k,_tmp90E)))))));};}
# 459
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp911;void*_tmp910;(_tmp910=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp911="expecting a list of type variables, not types",_tag_dyneither(_tmp911,sizeof(char),46))),_tag_dyneither(_tmp910,sizeof(void*),0)));}_LL44:;}
# 465
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp914;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp913;return(void*)((_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913[0]=((_tmp914.tag=2,((_tmp914.f1=pr,_tmp914)))),_tmp913))));}
# 470
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 473
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 475
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp917;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp916;*_tmpAD=(void*)((_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=2,((_tmp917.f1=0,((_tmp917.f2=k,_tmp917)))))),_tmp916))));}
return;_LL51:;_LL52:
 return;_LL4E:;}_LL4C:;_LL4D:
# 480
 return;_LL49:;}
# 485
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 491
if(tds == 0)return tms;
# 496
if(tms == 0)return 0;{
# 498
void*_tmpB3=(void*)tms->hd;void*_tmpB4=_tmpB3;void*_tmpB6;_LL54: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB4;if(_tmpB5->tag != 3)goto _LL56;else{_tmpB6=(void*)_tmpB5->f1;}}_LL55:
# 501
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 504
void*_tmpB7=_tmpB6;struct Cyc_List_List*_tmpBA;_LL59: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL5B;}_LL5A:
# 506
{const char*_tmp91A;void*_tmp919;(_tmp919=0,Cyc_Tcutil_warn(loc,((_tmp91A="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp91A,sizeof(char),93))),_tag_dyneither(_tmp919,sizeof(void*),0)));}
# 508
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp91D;void*_tmp91C;(_tmp91C=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp91D="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp91D,sizeof(char),73))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}{
# 514
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
struct Cyc_List_List*_tmpBF=tds;
for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
struct Cyc_Absyn_Decl*_tmpC0=(struct Cyc_Absyn_Decl*)_tmpBF->hd;
void*_tmpC1=_tmpC0->r;void*_tmpC2=_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC4;_LL5E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC2;if(_tmpC3->tag != 0)goto _LL60;else{_tmpC4=_tmpC3->f1;}}_LL5F:
# 521
 if(Cyc_zstrptrcmp((*_tmpC4->name).f2,(struct _dyneither_ptr*)_tmpBA->hd)!= 0)
continue;
if(_tmpC4->initializer != 0){
const char*_tmp920;void*_tmp91F;(_tmp91F=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp920="initializer found in parameter declaration",_tag_dyneither(_tmp920,sizeof(char),43))),_tag_dyneither(_tmp91F,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp923;void*_tmp922;(_tmp922=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp923="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp923,sizeof(char),47))),_tag_dyneither(_tmp922,sizeof(void*),0)));}
{struct _tuple8*_tmp926;struct Cyc_List_List*_tmp925;rev_new_params=(
(_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925->hd=((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->f1=(*_tmpC4->name).f2,((_tmp926->f2=_tmpC4->tq,((_tmp926->f3=_tmpC4->type,_tmp926)))))))),((_tmp925->tl=rev_new_params,_tmp925))))));}
# 530
goto L;_LL60:;_LL61: {
const char*_tmp929;void*_tmp928;(_tmp928=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp929="nonvariable declaration in parameter type",_tag_dyneither(_tmp929,sizeof(char),42))),_tag_dyneither(_tmp928,sizeof(void*),0)));}_LL5D:;}
# 534
L: if(_tmpBF == 0){
const char*_tmp92D;void*_tmp92C[1];struct Cyc_String_pa_PrintArg_struct _tmp92B;(_tmp92B.tag=0,((_tmp92B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp92C[0]=& _tmp92B,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp92D="%s is not given a type",_tag_dyneither(_tmp92D,sizeof(char),23))),_tag_dyneither(_tmp92C,sizeof(void*),1)))))));}}{
# 537
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp93C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp93B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp93A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp939;struct Cyc_List_List*_tmp938;return
(_tmp938=_region_malloc(yy,sizeof(*_tmp938)),((_tmp938->hd=(void*)((_tmp93C=_region_malloc(yy,sizeof(*_tmp93C)),((_tmp93C[0]=((_tmp939.tag=3,((_tmp939.f1=(void*)((_tmp93B=_region_malloc(yy,sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93A.tag=1,((_tmp93A.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp93A.f2=0,((_tmp93A.f3=0,((_tmp93A.f4=0,((_tmp93A.f5=0,((_tmp93A.f6=0,((_tmp93A.f7=0,_tmp93A)))))))))))))))),_tmp93B)))),_tmp939)))),_tmp93C)))),((_tmp938->tl=0,_tmp938)))));};};_LL58:;}
# 544
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp93D;return(_tmp93D=_region_malloc(yy,sizeof(*_tmp93D)),((_tmp93D->hd=(void*)tms->hd,((_tmp93D->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp93D)))));}_LL53:;};}
# 552
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 556
if(tds != 0){
struct Cyc_Parse_Declarator _tmp93E;d=((_tmp93E.id=d.id,((_tmp93E.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp93E))));}{
# 559
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 565
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 571
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmpD7=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));switch(_tmpD7){case Cyc_Parse_Extern_sc: _LL62:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL63:
 sc=Cyc_Absyn_Static;break;default: _LL64:
{const char*_tmp93F;Cyc_Parse_err(((_tmp93F="bad storage class on function",_tag_dyneither(_tmp93F,sizeof(char),30))),loc);}break;}}}{
# 578
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 583
if(_tmpDE != 0){
# 586
const char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_Tcutil_warn(loc,((_tmp942="bad type params, ignoring",_tag_dyneither(_tmp942,sizeof(char),26))),_tag_dyneither(_tmp941,sizeof(void*),0)));}{
# 588
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF0;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;_tmpED=(_tmpE3->f1).requires_clause;_tmpEE=(_tmpE3->f1).requires_relns;_tmpEF=(_tmpE3->f1).ensures_clause;_tmpF0=(_tmpE3->f1).ensures_relns;}}_LL68: {
# 592
struct Cyc_List_List*_tmpF1=0;
{struct Cyc_List_List*_tmpF2=_tmpE8;for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){
struct _tuple8*_tmpF3=(struct _tuple8*)_tmpF2->hd;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF7;struct _tuple8*_tmpF4=_tmpF3;_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;_tmpF7=_tmpF4->f3;
if(_tmpF5 == 0){
{const char*_tmp943;Cyc_Parse_err(((_tmp943="missing argument variable in function prototype",_tag_dyneither(_tmp943,sizeof(char),48))),loc);}{
struct _tuple8*_tmp946;struct Cyc_List_List*_tmp945;_tmpF1=((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945->hd=((_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp946->f2=_tmpF6,((_tmp946->f3=_tmpF7,_tmp946)))))))),((_tmp945->tl=_tmpF1,_tmp945))))));};}else{
# 599
struct _tuple8*_tmp949;struct Cyc_List_List*_tmp948;_tmpF1=((_tmp948=_cycalloc(sizeof(*_tmp948)),((_tmp948->hd=((_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949->f1=_tmpF5,((_tmp949->f2=_tmpF6,((_tmp949->f3=_tmpF7,_tmp949)))))))),((_tmp948->tl=_tmpF1,_tmp948))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp94A;return(_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A->sc=sc,((_tmp94A->name=d.id,((_tmp94A->tvs=_tmpE4,((_tmp94A->is_inline=is_inline,((_tmp94A->effect=_tmpE5,((_tmp94A->ret_tqual=_tmpE6,((_tmp94A->ret_type=_tmpE7,((_tmp94A->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpF1),((_tmp94A->c_varargs=_tmpE9,((_tmp94A->cyc_varargs=_tmpEA,((_tmp94A->rgn_po=_tmpEB,((_tmp94A->body=body,((_tmp94A->cached_typ=0,((_tmp94A->param_vardecls=0,((_tmp94A->fn_vardecl=0,((_tmp94A->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),((_tmp94A->requires_clause=_tmpED,((_tmp94A->requires_relns=0,((_tmp94A->ensures_clause=_tmpEF,((_tmp94A->ensures_relns=0,_tmp94A)))))))))))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
# 618
const char*_tmp94D;void*_tmp94C;(_tmp94C=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp94D="declarator is not a function prototype",_tag_dyneither(_tmp94D,sizeof(char),39))),_tag_dyneither(_tmp94C,sizeof(void*),0)));}_LL66:;};};};}static char _tmp101[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 622
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp101,_tmp101,_tmp101 + 76};static char _tmp102[87]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 624
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp102,_tmp102,_tmp102 + 87};static char _tmp103[74]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 626
static struct _dyneither_ptr Cyc_Parse_msg3={_tmp103,_tmp103,_tmp103 + 74};static char _tmp104[84]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 628
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp104,_tmp104,_tmp104 + 84};
# 635
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 638
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp94E;(_tmp94E=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp94E,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp94F;(_tmp94F=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp94F,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp950;(_tmp950=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp950,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp951;(_tmp951=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp951,sizeof(void*),0)));}
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 660
return s1;}
# 666
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 669
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 676
if(ts.Signed_spec  && ts.Unsigned_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 688
if(!seen_type){
if(!seen_sign  && !seen_size){
const char*_tmp954;void*_tmp953;(_tmp953=0,Cyc_Tcutil_warn(loc,((_tmp954="missing type within specifier",_tag_dyneither(_tmp954,sizeof(char),30))),_tag_dyneither(_tmp953,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 693
if(seen_sign){
void*_tmp10B=t;enum Cyc_Absyn_Sign _tmp10D;enum Cyc_Absyn_Size_of _tmp10E;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10B;if(_tmp10C->tag != 6)goto _LL6E;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LL6D:
# 696
 if(_tmp10D != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10E);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp955;Cyc_Parse_err(((_tmp955="sign specification on non-integral type",_tag_dyneither(_tmp955,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 701
if(seen_size){
void*_tmp110=t;enum Cyc_Absyn_Sign _tmp112;enum Cyc_Absyn_Size_of _tmp113;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp111=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp110;if(_tmp111->tag != 6)goto _LL73;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;}}_LL72:
# 704
 if(_tmp113 != sz)
t=Cyc_Absyn_int_typ(_tmp112,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp114=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp110;if(_tmp114->tag != 7)goto _LL75;}_LL74:
# 708
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp956;Cyc_Parse_err(((_tmp956="size qualifier on non-integral type",_tag_dyneither(_tmp956,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
# 712
return t;}
# 715
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 719
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp116=d.id;
struct _tuple13 _tmp117=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct Cyc_Absyn_Tqual _tmp119;void*_tmp11A;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11C;struct _tuple13 _tmp118=_tmp117;_tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;_tmp11B=_tmp118.f3;_tmp11C=_tmp118.f4;
# 725
if(ds->tl == 0){
struct _tuple15*_tmp959;struct Cyc_List_List*_tmp958;return(_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958->hd=((_tmp959=_region_malloc(r,sizeof(*_tmp959)),((_tmp959->f1=_tmp116,((_tmp959->f2=_tmp119,((_tmp959->f3=_tmp11A,((_tmp959->f4=_tmp11B,((_tmp959->f5=_tmp11C,_tmp959)))))))))))),((_tmp958->tl=0,_tmp958)))));}else{
# 728
struct _tuple15*_tmp95C;struct Cyc_List_List*_tmp95B;return(_tmp95B=_region_malloc(r,sizeof(*_tmp95B)),((_tmp95B->hd=((_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C->f1=_tmp116,((_tmp95C->f2=_tmp119,((_tmp95C->f3=_tmp11A,((_tmp95C->f4=_tmp11B,((_tmp95C->f5=_tmp11C,_tmp95C)))))))))))),((_tmp95B->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp95B)))));}};}
# 732
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 735
if(tms == 0){struct _tuple13 _tmp95D;return(_tmp95D.f1=tq,((_tmp95D.f2=t,((_tmp95D.f3=0,((_tmp95D.f4=atts,_tmp95D)))))));}{
void*_tmp122=(void*)tms->hd;void*_tmp123=_tmp122;union Cyc_Absyn_Constraint*_tmp125;unsigned int _tmp126;struct Cyc_Absyn_Exp*_tmp128;union Cyc_Absyn_Constraint*_tmp129;unsigned int _tmp12A;void*_tmp12C;struct Cyc_List_List*_tmp12E;unsigned int _tmp12F;struct Cyc_Absyn_PtrAtts _tmp131;struct Cyc_Absyn_Tqual _tmp132;unsigned int _tmp134;struct Cyc_List_List*_tmp135;_LL78: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp124=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp124->tag != 0)goto _LL7A;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL79:
# 738
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp125,_tmp126),atts,tms->tl);_LL7A: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp127=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp127->tag != 1)goto _LL7C;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;_tmp12A=_tmp127->f3;}}_LL7B:
# 741
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp128,_tmp129,_tmp12A),atts,tms->tl);_LL7C: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp12B=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12B->tag != 3)goto _LL7E;else{_tmp12C=(void*)_tmp12B->f1;}}_LL7D: {
# 744
void*_tmp136=_tmp12C;struct Cyc_List_List*_tmp138;int _tmp139;struct Cyc_Absyn_VarargInfo*_tmp13A;void*_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;unsigned int _tmp140;_LL85: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp137=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp136;if(_tmp137->tag != 1)goto _LL87;else{_tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;_tmp13B=(void*)_tmp137->f4;_tmp13C=_tmp137->f5;_tmp13D=_tmp137->f6;_tmp13E=_tmp137->f7;}}_LL86: {
# 746
struct Cyc_List_List*typvars=0;
# 748
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp95E;fn_atts=((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E->hd=(void*)as->hd,((_tmp95E->tl=fn_atts,_tmp95E))))));}else{
# 753
struct Cyc_List_List*_tmp95F;new_atts=((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F->hd=(void*)as->hd,((_tmp95F->tl=new_atts,_tmp95F))))));}}}
# 756
if(tms->tl != 0){
void*_tmp143=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp144=_tmp143;struct Cyc_List_List*_tmp146;_LL8A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp145=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp144;if(_tmp145->tag != 4)goto _LL8C;else{_tmp146=_tmp145->f1;}}_LL8B:
# 759
 typvars=_tmp146;
tms=tms->tl;
goto _LL89;_LL8C:;_LL8D:
# 763
 goto _LL89;_LL89:;}
# 767
if(((((!_tmp139  && _tmp13A == 0) && _tmp138 != 0) && _tmp138->tl == 0) && (*((struct _tuple8*)_tmp138->hd)).f1 == 0) && (*((struct _tuple8*)_tmp138->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 772
_tmp138=0;{
# 775
struct Cyc_List_List*_tmp147=Cyc_Parse_get_arg_tags(_tmp138);
# 777
if(_tmp147 != 0)
t=Cyc_Parse_substitute_tags(_tmp147,t);
t=Cyc_Parse_array2ptr(t,0);
# 782
{struct Cyc_List_List*_tmp148=_tmp138;for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
struct _tuple8*_tmp149=(struct _tuple8*)_tmp148->hd;struct _dyneither_ptr*_tmp14B;struct Cyc_Absyn_Tqual _tmp14C;void**_tmp14D;struct _tuple8*_tmp14A=_tmp149;_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=(void**)& _tmp14A->f3;
if(_tmp147 != 0)
*_tmp14D=Cyc_Parse_substitute_tags(_tmp147,*_tmp14D);
*_tmp14D=Cyc_Parse_array2ptr(*_tmp14D,1);}}
# 794
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp13B,tq,t,_tmp138,_tmp139,_tmp13A,_tmp13C,fn_atts,_tmp13D,_tmp13E),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL87: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp13F=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp136;if(_tmp13F->tag != 0)goto _LL84;else{_tmp140=_tmp13F->f2;}}_LL88: {
# 801
const char*_tmp962;void*_tmp961;(_tmp961=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp140,((_tmp962="function declaration without parameter types",_tag_dyneither(_tmp962,sizeof(char),45))),_tag_dyneither(_tmp961,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp12D=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12D->tag != 4)goto _LL80;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL7F:
# 808
 if(tms->tl == 0){
struct _tuple13 _tmp963;return(_tmp963.f1=tq,((_tmp963.f2=t,((_tmp963.f3=_tmp12E,((_tmp963.f4=atts,_tmp963)))))));}{
# 813
const char*_tmp966;void*_tmp965;(_tmp965=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12F,((_tmp966="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp966,sizeof(char),68))),_tag_dyneither(_tmp965,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp130=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp130->tag != 2)goto _LL82;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_LL81: {
# 816
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp96C;struct Cyc_Absyn_PtrInfo _tmp96B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp96A;return Cyc_Parse_apply_tms(_tmp132,(void*)((_tmp96A=_cycalloc(sizeof(*_tmp96A)),((_tmp96A[0]=((_tmp96C.tag=5,((_tmp96C.f1=((_tmp96B.elt_typ=t,((_tmp96B.elt_tq=tq,((_tmp96B.ptr_atts=_tmp131,_tmp96B)))))),_tmp96C)))),_tmp96A)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp133=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp133->tag != 5)goto _LL77;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL83:
# 821
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp135),tms->tl);_LL77:;};}
# 827
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 836
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp157;struct Cyc_Absyn_Tqual _tmp158;void*_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15B;struct _tuple15*_tmp156=t;_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;_tmp159=_tmp156->f3;_tmp15A=_tmp156->f4;_tmp15B=_tmp156->f5;
# 839
Cyc_Lex_register_typedef(_tmp157);{
# 841
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp15C=_tmp159;struct Cyc_Core_Opt*_tmp15E;_LL8F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15C;if(_tmp15D->tag != 1)goto _LL91;else{_tmp15E=_tmp15D->f1;}}_LL90:
# 845
 type=0;
if(_tmp15E == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp15E;}
goto _LL8E;_LL91:;_LL92:
 kind=0;type=_tmp159;goto _LL8E;_LL8E:;}{
# 851
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp972;struct Cyc_Absyn_Typedefdecl*_tmp971;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp970;return Cyc_Absyn_new_decl((void*)((_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970[0]=((_tmp972.tag=8,((_tmp972.f1=((_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971->name=_tmp157,((_tmp971->tvs=_tmp15A,((_tmp971->kind=kind,((_tmp971->defn=type,((_tmp971->atts=_tmp15B,((_tmp971->tq=_tmp158,((_tmp971->extern_c=0,_tmp971)))))))))))))))),_tmp972)))),_tmp970)))),loc);};};}
# 858
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp975;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp974;return Cyc_Absyn_new_stmt((void*)((_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=12,((_tmp975.f1=d,((_tmp975.f2=s,_tmp975)))))),_tmp974)))),d->loc);}
# 864
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 873
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 877
struct _RegionHandle _tmp164=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp164;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp166;struct Cyc_Parse_Type_specifier _tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Parse_Declaration_spec _tmp165=ds;_tmp166=_tmp165.tq;_tmp167=_tmp165.type_specs;_tmp168=_tmp165.attributes;
if(_tmp166.loc == 0)_tmp166.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp978;void*_tmp977;(_tmp977=0,Cyc_Tcutil_warn(loc,((_tmp978="inline qualifier on non-function definition",_tag_dyneither(_tmp978,sizeof(char),44))),_tag_dyneither(_tmp977,sizeof(void*),0)));}{
# 883
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp16B=*ds.sc;switch(_tmp16B){case Cyc_Parse_Typedef_sc: _LL93:
 istypedef=1;break;case Cyc_Parse_Extern_sc: _LL94:
 s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LL95:
 s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LL96:
 s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc: _LL97:
 s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LL98:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc: _LL99:
 s=Cyc_Absyn_Abstract;break;}}{
# 899
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 903
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 911
void*_tmp16C=Cyc_Parse_collapse_type_specifiers(_tmp167,loc);
if(declarators == 0){
# 915
void*_tmp16D=_tmp16C;struct Cyc_Absyn_Aggrdecl*_tmp170;struct Cyc_Absyn_Enumdecl*_tmp173;struct Cyc_Absyn_Datatypedecl*_tmp176;enum Cyc_Absyn_AggrKind _tmp178;struct _tuple0*_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Datatypedecl**_tmp17C;struct _tuple0*_tmp17E;int _tmp17F;struct Cyc_List_List*_tmp180;struct _tuple0*_tmp182;struct Cyc_List_List*_tmp184;_LL9C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16D;if(_tmp16E->tag != 26)goto _LL9E;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp16F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp16E->f1)->r;if(_tmp16F->tag != 0)goto _LL9E;else{_tmp170=_tmp16F->f1;}}}_LL9D:
# 917
 _tmp170->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp170->attributes,_tmp168);
_tmp170->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp97E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp97D;struct Cyc_List_List*_tmp97C;struct Cyc_List_List*_tmp188=(_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C->hd=Cyc_Absyn_new_decl((void*)((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=((_tmp97D.tag=5,((_tmp97D.f1=_tmp170,_tmp97D)))),_tmp97E)))),loc),((_tmp97C->tl=0,_tmp97C)))));_npop_handler(0);return _tmp188;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp171=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16D;if(_tmp171->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp172=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp171->f1)->r;if(_tmp172->tag != 1)goto _LLA0;else{_tmp173=_tmp172->f1;}}}_LL9F:
# 921
 if(_tmp168 != 0){const char*_tmp97F;Cyc_Parse_err(((_tmp97F="attributes on enum not supported",_tag_dyneither(_tmp97F,sizeof(char),33))),loc);}
_tmp173->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp985;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp984;struct Cyc_List_List*_tmp983;struct Cyc_List_List*_tmp18D=(_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983->hd=Cyc_Absyn_new_decl((void*)((_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985[0]=((_tmp984.tag=7,((_tmp984.f1=_tmp173,_tmp984)))),_tmp985)))),loc),((_tmp983->tl=0,_tmp983)))));_npop_handler(0);return _tmp18D;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16D;if(_tmp174->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp175=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp174->f1)->r;if(_tmp175->tag != 2)goto _LLA2;else{_tmp176=_tmp175->f1;}}}_LLA1:
# 925
 if(_tmp168 != 0){const char*_tmp986;Cyc_Parse_err(((_tmp986="attributes on datatypes not supported",_tag_dyneither(_tmp986,sizeof(char),38))),loc);}
_tmp176->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp98C;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp98B;struct Cyc_List_List*_tmp98A;struct Cyc_List_List*_tmp192=(_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A->hd=Cyc_Absyn_new_decl((void*)((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98B.tag=6,((_tmp98B.f1=_tmp176,_tmp98B)))),_tmp98C)))),loc),((_tmp98A->tl=0,_tmp98A)))));_npop_handler(0);return _tmp192;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16D;if(_tmp177->tag != 11)goto _LLA4;else{if((((_tmp177->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp178=((struct _tuple2)(((_tmp177->f1).aggr_info).UnknownAggr).val).f1;_tmp179=((struct _tuple2)(((_tmp177->f1).aggr_info).UnknownAggr).val).f2;_tmp17A=(_tmp177->f1).targs;}}_LLA3: {
# 929
struct Cyc_List_List*_tmp193=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17A);
struct Cyc_Absyn_Aggrdecl*_tmp98D;struct Cyc_Absyn_Aggrdecl*_tmp194=(_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D->kind=_tmp178,((_tmp98D->sc=s,((_tmp98D->name=_tmp179,((_tmp98D->tvs=_tmp193,((_tmp98D->impl=0,((_tmp98D->attributes=0,((_tmp98D->expected_mem_kind=0,_tmp98D)))))))))))))));
if(_tmp168 != 0){const char*_tmp98E;Cyc_Parse_err(((_tmp98E="bad attributes on type declaration",_tag_dyneither(_tmp98E,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp994;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp993;struct Cyc_List_List*_tmp992;struct Cyc_List_List*_tmp199=(_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992->hd=Cyc_Absyn_new_decl((void*)((_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994[0]=((_tmp993.tag=5,((_tmp993.f1=_tmp194,_tmp993)))),_tmp994)))),loc),((_tmp992->tl=0,_tmp992)))));_npop_handler(0);return _tmp199;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16D;if(_tmp17B->tag != 3)goto _LLA6;else{if((((_tmp17B->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp17C=(struct Cyc_Absyn_Datatypedecl**)(((_tmp17B->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 934
 if(_tmp168 != 0){const char*_tmp995;Cyc_Parse_err(((_tmp995="bad attributes on datatype",_tag_dyneither(_tmp995,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp99B;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp99A;struct Cyc_List_List*_tmp999;struct Cyc_List_List*_tmp19F=(_tmp999=_cycalloc(sizeof(*_tmp999)),((_tmp999->hd=Cyc_Absyn_new_decl((void*)((_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99A.tag=6,((_tmp99A.f1=*_tmp17C,_tmp99A)))),_tmp99B)))),loc),((_tmp999->tl=0,_tmp999)))));_npop_handler(0);return _tmp19F;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16D;if(_tmp17D->tag != 3)goto _LLA8;else{if((((_tmp17D->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp17E=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17D->f1).datatype_info).UnknownDatatype).val).name;_tmp17F=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17D->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp180=(_tmp17D->f1).targs;}}_LLA7: {
# 937
struct Cyc_List_List*_tmp1A0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp180);
struct Cyc_Absyn_Decl*_tmp1A1=Cyc_Absyn_datatype_decl(s,_tmp17E,_tmp1A0,0,_tmp17F,loc);
if(_tmp168 != 0){const char*_tmp99C;Cyc_Parse_err(((_tmp99C="bad attributes on datatype",_tag_dyneither(_tmp99C,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp1A4=(_tmp99D=_cycalloc(sizeof(*_tmp99D)),((_tmp99D->hd=_tmp1A1,((_tmp99D->tl=0,_tmp99D)))));_npop_handler(0);return _tmp1A4;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16D;if(_tmp181->tag != 13)goto _LLAA;else{_tmp182=_tmp181->f1;}}_LLA9: {
# 942
struct Cyc_Absyn_Enumdecl*_tmp99E;struct Cyc_Absyn_Enumdecl*_tmp1A5=(_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E->sc=s,((_tmp99E->name=_tmp182,((_tmp99E->fields=0,_tmp99E)))))));
if(_tmp168 != 0){const char*_tmp99F;Cyc_Parse_err(((_tmp99F="bad attributes on enum",_tag_dyneither(_tmp99F,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9A9;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp9A8;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9A7;struct Cyc_List_List*_tmp9A6;struct Cyc_List_List*_tmp1AB=(_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6->hd=((_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9->r=(void*)((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7[0]=((_tmp9A8.tag=7,((_tmp9A8.f1=_tmp1A5,_tmp9A8)))),_tmp9A7)))),((_tmp9A9->loc=loc,_tmp9A9)))))),((_tmp9A6->tl=0,_tmp9A6)))));_npop_handler(0);return _tmp1AB;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp183=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp16D;if(_tmp183->tag != 14)goto _LLAC;else{_tmp184=_tmp183->f1;}}_LLAB: {
# 948
struct Cyc_Core_Opt*_tmp9AC;struct Cyc_Absyn_Enumdecl*_tmp9AB;struct Cyc_Absyn_Enumdecl*_tmp1AD=(_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB->sc=s,((_tmp9AB->name=Cyc_Parse_gensym_enum(),((_tmp9AB->fields=((_tmp9AC=_cycalloc(sizeof(*_tmp9AC)),((_tmp9AC->v=_tmp184,_tmp9AC)))),_tmp9AB)))))));
if(_tmp168 != 0){const char*_tmp9AD;Cyc_Parse_err(((_tmp9AD="bad attributes on enum",_tag_dyneither(_tmp9AD,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9B7;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp9B6;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9B5;struct Cyc_List_List*_tmp9B4;struct Cyc_List_List*_tmp1B3=(_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4->hd=((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7->r=(void*)((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B6.tag=7,((_tmp9B6.f1=_tmp1AD,_tmp9B6)))),_tmp9B5)))),((_tmp9B7->loc=loc,_tmp9B7)))))),((_tmp9B4->tl=0,_tmp9B4)))));_npop_handler(0);return _tmp1B3;};}_LLAC:;_LLAD:
{const char*_tmp9B8;Cyc_Parse_err(((_tmp9B8="missing declarator",_tag_dyneither(_tmp9B8,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B7=0;_npop_handler(0);return _tmp1B7;};_LL9B:;}else{
# 955
struct Cyc_List_List*_tmp1B8=Cyc_Parse_apply_tmss(mkrgn,_tmp166,_tmp16C,declarators,_tmp168);
if(istypedef){
# 960
if(!exps_empty){
const char*_tmp9B9;Cyc_Parse_err(((_tmp9B9="initializer in typedef declaration",_tag_dyneither(_tmp9B9,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B8);
struct Cyc_List_List*_tmp1BA=decls;_npop_handler(0);return _tmp1BA;};}else{
# 966
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1BB=_tmp1B8;for(0;_tmp1BB != 0;(_tmp1BB=_tmp1BB->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1BC=(struct _tuple15*)_tmp1BB->hd;struct _tuple0*_tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;void*_tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C2;struct _tuple15*_tmp1BD=_tmp1BC;_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;_tmp1C0=_tmp1BD->f3;_tmp1C1=_tmp1BD->f4;_tmp1C2=_tmp1BD->f5;
if(_tmp1C1 != 0){
const char*_tmp9BC;void*_tmp9BB;(_tmp9BB=0,Cyc_Tcutil_warn(loc,((_tmp9BC="bad type params, ignoring",_tag_dyneither(_tmp9BC,sizeof(char),26))),_tag_dyneither(_tmp9BB,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp9BF;void*_tmp9BE;(_tmp9BE=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp9BF="unexpected NULL in parse!",_tag_dyneither(_tmp9BF,sizeof(char),26))),_tag_dyneither(_tmp9BE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C8=Cyc_Absyn_new_vardecl(_tmp1BE,_tmp1C0,_tmp1C7);
_tmp1C8->tq=_tmp1BF;
_tmp1C8->sc=s;
_tmp1C8->attributes=_tmp1C2;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9C5;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9C4;struct Cyc_Absyn_Decl*_tmp9C3;struct Cyc_Absyn_Decl*_tmp1C9=(_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3->r=(void*)((_tmp9C5=_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5[0]=((_tmp9C4.tag=0,((_tmp9C4.f1=_tmp1C8,_tmp9C4)))),_tmp9C5)))),((_tmp9C3->loc=loc,_tmp9C3)))));
struct Cyc_List_List*_tmp9C6;decls=((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6->hd=_tmp1C9,((_tmp9C6->tl=decls,_tmp9C6))))));};};}}{
# 981
struct Cyc_List_List*_tmp1CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1CE;};}}};};};};}
# 878
;_pop_region(mkrgn);}
# 988
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp1CF=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));switch(_tmp1CF){case 'A': _LLAE:
 return& Cyc_Tcutil_ak;case 'M': _LLAF:
 return& Cyc_Tcutil_mk;case 'B': _LLB0:
 return& Cyc_Tcutil_bk;case 'R': _LLB1:
 return& Cyc_Tcutil_rk;case 'E': _LLB2:
 return& Cyc_Tcutil_ek;case 'I': _LLB3:
 return& Cyc_Tcutil_ik;case 'U': _LLB4:
# 998
{char _tmp1D0=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1D0){case 'R': _LLB6:
 return& Cyc_Tcutil_urk;case 'A': _LLB7:
 return& Cyc_Tcutil_uak;case 'M': _LLB8:
 return& Cyc_Tcutil_umk;case 'B': _LLB9:
 return& Cyc_Tcutil_ubk;default: _LLBA:
 break;}}
# 1005
break;case 'T': _LLB5:
# 1007
{char _tmp1D1=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1D1){case 'R': _LLBD:
 return& Cyc_Tcutil_trk;case 'A': _LLBE:
 return& Cyc_Tcutil_tak;case 'M': _LLBF:
 return& Cyc_Tcutil_tmk;case 'B': _LLC0:
 return& Cyc_Tcutil_tbk;default: _LLC1:
 break;}}
# 1014
break;default: _LLBC:
 break;}}
# 1017
{const char*_tmp9CB;void*_tmp9CA[2];struct Cyc_String_pa_PrintArg_struct _tmp9C9;struct Cyc_Int_pa_PrintArg_struct _tmp9C8;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9C8.tag=1,((_tmp9C8.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9C9.tag=0,((_tmp9C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9CA[0]=& _tmp9C9,((_tmp9CA[1]=& _tmp9C8,Cyc_aprintf(((_tmp9CB="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9CB,sizeof(char),24))),_tag_dyneither(_tmp9CA,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1022
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1D6=e->r;void*_tmp1D7=_tmp1D6;int _tmp1D9;_LLC5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1D7;if(_tmp1D8->tag != 0)goto _LLC7;else{if(((_tmp1D8->f1).Int_c).tag != 5)goto _LLC7;_tmp1D9=((struct _tuple5)((_tmp1D8->f1).Int_c).val).f2;}}_LLC6:
 return _tmp1D9;_LLC7:;_LLC8:
# 1026
{const char*_tmp9CC;Cyc_Parse_err(((_tmp9CC="expecting integer constant",_tag_dyneither(_tmp9CC,sizeof(char),27))),loc);}
return 0;_LLC4:;}
# 1032
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1DB=e->r;void*_tmp1DC=_tmp1DB;struct _dyneither_ptr _tmp1DE;_LLCA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1DC;if(_tmp1DD->tag != 0)goto _LLCC;else{if(((_tmp1DD->f1).String_c).tag != 8)goto _LLCC;_tmp1DE=(struct _dyneither_ptr)((_tmp1DD->f1).String_c).val;}}_LLCB:
 return _tmp1DE;_LLCC:;_LLCD:
# 1036
{const char*_tmp9CD;Cyc_Parse_err(((_tmp9CD="expecting string constant",_tag_dyneither(_tmp9CD,sizeof(char),26))),loc);}
return _tag_dyneither(0,0,0);_LLC9:;}
# 1042
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1E0=x;int _tmp1E1;char _tmp1E2;long long _tmp1E3;_LLCF: if((_tmp1E0.Int_c).tag != 5)goto _LLD1;_tmp1E1=((struct _tuple5)(_tmp1E0.Int_c).val).f2;_LLD0:
 return(unsigned int)_tmp1E1;_LLD1: if((_tmp1E0.Char_c).tag != 2)goto _LLD3;_tmp1E2=((struct _tuple3)(_tmp1E0.Char_c).val).f2;_LLD2:
 return(unsigned int)_tmp1E2;_LLD3: if((_tmp1E0.LongLong_c).tag != 6)goto _LLD5;_tmp1E3=((struct _tuple6)(_tmp1E0.LongLong_c).val).f2;_LLD4: {
# 1047
unsigned long long y=(unsigned long long)_tmp1E3;
if(y > -1){
const char*_tmp9CE;Cyc_Parse_err(((_tmp9CE="integer constant too large",_tag_dyneither(_tmp9CE,sizeof(char),27))),loc);}
return(unsigned int)_tmp1E3;}_LLD5:;_LLD6:
# 1052
{const char*_tmp9D2;void*_tmp9D1[1];struct Cyc_String_pa_PrintArg_struct _tmp9D0;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9D0.tag=0,((_tmp9D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x)),((_tmp9D1[0]=& _tmp9D0,Cyc_aprintf(((_tmp9D2="expected integer constant but found %s",_tag_dyneither(_tmp9D2,sizeof(char),39))),_tag_dyneither(_tmp9D1,sizeof(void*),1)))))))),loc);}
return 0;_LLCE:;}
# 1058
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1E8=p->r;void*_tmp1E9=_tmp1E8;struct _tuple0*_tmp1EB;struct Cyc_Absyn_Vardecl*_tmp1ED;struct Cyc_Absyn_Pat*_tmp1F0;enum Cyc_Absyn_Sign _tmp1F3;int _tmp1F4;char _tmp1F6;struct _dyneither_ptr _tmp1F8;int _tmp1F9;struct _tuple0*_tmp1FB;struct Cyc_List_List*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FE;_LLD8: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1EA=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1EA->tag != 15)goto _LLDA;else{_tmp1EB=_tmp1EA->f1;}}_LLD9:
 return Cyc_Absyn_unknownid_exp(_tmp1EB,p->loc);_LLDA: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1EC=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1EC->tag != 3)goto _LLDC;else{_tmp1ED=_tmp1EC->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1EE=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1EC->f2)->r;if(_tmp1EE->tag != 0)goto _LLDC;};}}_LLDB:
# 1062
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1ED->name,p->loc),p->loc);_LLDC: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1EF=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1EF->tag != 6)goto _LLDE;else{_tmp1F0=_tmp1EF->f1;}}_LLDD:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1F0),p->loc);_LLDE: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1F1=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1F1->tag != 9)goto _LLE0;}_LLDF:
 return Cyc_Absyn_null_exp(p->loc);_LLE0: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1F2=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1F2->tag != 10)goto _LLE2;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;}}_LLE1:
 return Cyc_Absyn_int_exp(_tmp1F3,_tmp1F4,p->loc);_LLE2: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1F5=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1F5->tag != 11)goto _LLE4;else{_tmp1F6=_tmp1F5->f1;}}_LLE3:
 return Cyc_Absyn_char_exp(_tmp1F6,p->loc);_LLE4: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1F7=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1F7->tag != 12)goto _LLE6;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F7->f2;}}_LLE5:
 return Cyc_Absyn_float_exp(_tmp1F8,_tmp1F9,p->loc);_LLE6: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1FA=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1FA->tag != 16)goto _LLE8;else{_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FA->f2;if(_tmp1FA->f3 != 0)goto _LLE8;}}_LLE7: {
# 1069
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1FB,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1FC);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLE8: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1FD=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1E9;if(_tmp1FD->tag != 17)goto _LLEA;else{_tmp1FE=_tmp1FD->f1;}}_LLE9:
 return _tmp1FE;_LLEA:;_LLEB:
# 1074
{const char*_tmp9D3;Cyc_Parse_err(((_tmp9D3="cannot mix patterns and expressions in case",_tag_dyneither(_tmp9D3,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLD7:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp201[7]="cnst_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 7}};
# 1132
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;union Cyc_Absyn_Cnst _tmp203;_LLED: if((_tmp202.Int_tok).tag != 1)goto _LLEF;_tmp203=(union Cyc_Absyn_Cnst)(_tmp202.Int_tok).val;_LLEE:
# 1136
 yyzzz=_tmp203;
goto _LLEC;_LLEF:;_LLF0:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLEC:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.Int_tok).val=yy1,(((_tmp9D4.Int_tok).tag=1,_tmp9D4)));}static char _tmp206[5]="char";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 5}};
# 1133
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp207=yy1;char _tmp208;_LLF2: if((_tmp207.Char_tok).tag != 2)goto _LLF4;_tmp208=(char)(_tmp207.Char_tok).val;_LLF3:
# 1137
 yyzzz=_tmp208;
goto _LLF1;_LLF4:;_LLF5:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF1:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.Char_tok).val=yy1,(((_tmp9D5.Char_tok).tag=2,_tmp9D5)));}static char _tmp20B[13]="string_t<`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 13}};
# 1134
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct _dyneither_ptr _tmp20D;_LLF7: if((_tmp20C.String_tok).tag != 3)goto _LLF9;_tmp20D=(struct _dyneither_ptr)(_tmp20C.String_tok).val;_LLF8:
# 1138
 yyzzz=_tmp20D;
goto _LLF6;_LLF9:;_LLFA:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF6:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.String_tok).val=yy1,(((_tmp9D6.String_tok).tag=3,_tmp9D6)));}static char _tmp210[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 56}};
# 1137
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct _tuple21*_tmp212;_LLFC: if((_tmp211.YY1).tag != 9)goto _LLFE;_tmp212=(struct _tuple21*)(_tmp211.YY1).val;_LLFD:
# 1141
 yyzzz=_tmp212;
goto _LLFB;_LLFE:;_LLFF:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLFB:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY1).val=yy1,(((_tmp9D7.YY1).tag=9,_tmp9D7)));}static char _tmp215[19]="conref_t<bounds_t>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp215,_tmp215,_tmp215 + 19}};
# 1138
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp216=yy1;union Cyc_Absyn_Constraint*_tmp217;_LL101: if((_tmp216.YY2).tag != 10)goto _LL103;_tmp217=(union Cyc_Absyn_Constraint*)(_tmp216.YY2).val;_LL102:
# 1142
 yyzzz=_tmp217;
goto _LL100;_LL103:;_LL104:
(int)_throw((void*)& Cyc_yyfail_YY2);_LL100:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY2).val=yy1,(((_tmp9D8.YY2).tag=10,_tmp9D8)));}static char _tmp21A[28]="list_t<offsetof_field_t,`H>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 28}};
# 1139
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;struct Cyc_List_List*_tmp21C;_LL106: if((_tmp21B.YY3).tag != 11)goto _LL108;_tmp21C=(struct Cyc_List_List*)(_tmp21B.YY3).val;_LL107:
# 1143
 yyzzz=_tmp21C;
goto _LL105;_LL108:;_LL109:
(int)_throw((void*)& Cyc_yyfail_YY3);_LL105:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY3).val=yy1,(((_tmp9D9.YY3).tag=11,_tmp9D9)));}static char _tmp21F[6]="exp_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 6}};
# 1140
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_Absyn_Exp*_tmp221;_LL10B: if((_tmp220.Exp_tok).tag != 7)goto _LL10D;_tmp221=(struct Cyc_Absyn_Exp*)(_tmp220.Exp_tok).val;_LL10C:
# 1144
 yyzzz=_tmp221;
goto _LL10A;_LL10D:;_LL10E:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LL10A:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.Exp_tok).val=yy1,(((_tmp9DA.Exp_tok).tag=7,_tmp9DA)));}static char _tmp224[17]="list_t<exp_t,`H>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 17}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp225=yy1;struct Cyc_List_List*_tmp226;_LL110: if((_tmp225.YY4).tag != 12)goto _LL112;_tmp226=(struct Cyc_List_List*)(_tmp225.YY4).val;_LL111:
# 1152
 yyzzz=_tmp226;
goto _LL10F;_LL112:;_LL113:
(int)_throw((void*)& Cyc_yyfail_YY4);_LL10F:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY4).val=yy1,(((_tmp9DB.YY4).tag=12,_tmp9DB)));}static char _tmp229[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 47}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_List_List*_tmp22B;_LL115: if((_tmp22A.YY5).tag != 13)goto _LL117;_tmp22B=(struct Cyc_List_List*)(_tmp22A.YY5).val;_LL116:
# 1153
 yyzzz=_tmp22B;
goto _LL114;_LL117:;_LL118:
(int)_throw((void*)& Cyc_yyfail_YY5);_LL114:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY5).val=yy1,(((_tmp9DC.YY5).tag=13,_tmp9DC)));}static char _tmp22E[9]="primop_t";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 9}};
# 1150
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;enum Cyc_Absyn_Primop _tmp230;_LL11A: if((_tmp22F.YY6).tag != 14)goto _LL11C;_tmp230=(enum Cyc_Absyn_Primop)(_tmp22F.YY6).val;_LL11B:
# 1154
 yyzzz=_tmp230;
goto _LL119;_LL11C:;_LL11D:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL119:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY6).val=yy1,(((_tmp9DD.YY6).tag=14,_tmp9DD)));}static char _tmp233[19]="opt_t<primop_t,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 19}};
# 1151
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Core_Opt*_tmp235;_LL11F: if((_tmp234.YY7).tag != 15)goto _LL121;_tmp235=(struct Cyc_Core_Opt*)(_tmp234.YY7).val;_LL120:
# 1155
 yyzzz=_tmp235;
goto _LL11E;_LL121:;_LL122:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL11E:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY7).val=yy1,(((_tmp9DE.YY7).tag=15,_tmp9DE)));}static char _tmp238[7]="qvar_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 7}};
# 1152
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple0*_tmp23A;_LL124: if((_tmp239.QualId_tok).tag != 5)goto _LL126;_tmp23A=(struct _tuple0*)(_tmp239.QualId_tok).val;_LL125:
# 1156
 yyzzz=_tmp23A;
goto _LL123;_LL126:;_LL127:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL123:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.QualId_tok).val=yy1,(((_tmp9DF.QualId_tok).tag=5,_tmp9DF)));}static char _tmp23D[7]="stmt_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 7}};
# 1155
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct Cyc_Absyn_Stmt*_tmp23F;_LL129: if((_tmp23E.Stmt_tok).tag != 8)goto _LL12B;_tmp23F=(struct Cyc_Absyn_Stmt*)(_tmp23E.Stmt_tok).val;_LL12A:
# 1159
 yyzzz=_tmp23F;
goto _LL128;_LL12B:;_LL12C:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL128:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.Stmt_tok).val=yy1,(((_tmp9E0.Stmt_tok).tag=8,_tmp9E0)));}static char _tmp242[27]="list_t<switch_clause_t,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 27}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp243=yy1;struct Cyc_List_List*_tmp244;_LL12E: if((_tmp243.YY8).tag != 16)goto _LL130;_tmp244=(struct Cyc_List_List*)(_tmp243.YY8).val;_LL12F:
# 1163
 yyzzz=_tmp244;
goto _LL12D;_LL130:;_LL131:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL12D:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY8).val=yy1,(((_tmp9E1.YY8).tag=16,_tmp9E1)));}static char _tmp247[6]="pat_t";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 6}};
# 1160
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_Absyn_Pat*_tmp249;_LL133: if((_tmp248.YY9).tag != 17)goto _LL135;_tmp249=(struct Cyc_Absyn_Pat*)(_tmp248.YY9).val;_LL134:
# 1164
 yyzzz=_tmp249;
goto _LL132;_LL135:;_LL136:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL132:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY9).val=yy1,(((_tmp9E2.YY9).tag=17,_tmp9E2)));}static char _tmp24C[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 28}};
# 1165
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;struct _tuple22*_tmp24E;_LL138: if((_tmp24D.YY10).tag != 18)goto _LL13A;_tmp24E=(struct _tuple22*)(_tmp24D.YY10).val;_LL139:
# 1169
 yyzzz=_tmp24E;
goto _LL137;_LL13A:;_LL13B:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL137:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.YY10).val=yy1,(((_tmp9E3.YY10).tag=18,_tmp9E3)));}static char _tmp251[17]="list_t<pat_t,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 17}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_List_List*_tmp253;_LL13D: if((_tmp252.YY11).tag != 19)goto _LL13F;_tmp253=(struct Cyc_List_List*)(_tmp252.YY11).val;_LL13E:
# 1170
 yyzzz=_tmp253;
goto _LL13C;_LL13F:;_LL140:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL13C:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY11).val=yy1,(((_tmp9E4.YY11).tag=19,_tmp9E4)));}static char _tmp256[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 36}};
# 1167
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct _tuple23*_tmp258;_LL142: if((_tmp257.YY12).tag != 20)goto _LL144;_tmp258=(struct _tuple23*)(_tmp257.YY12).val;_LL143:
# 1171
 yyzzz=_tmp258;
goto _LL141;_LL144:;_LL145:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL141:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY12).val=yy1,(((_tmp9E5.YY12).tag=20,_tmp9E5)));}static char _tmp25B[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 47}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_List_List*_tmp25D;_LL147: if((_tmp25C.YY13).tag != 21)goto _LL149;_tmp25D=(struct Cyc_List_List*)(_tmp25C.YY13).val;_LL148:
# 1172
 yyzzz=_tmp25D;
goto _LL146;_LL149:;_LL14A:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL146:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY13).val=yy1,(((_tmp9E6.YY13).tag=21,_tmp9E6)));}static char _tmp260[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp260,_tmp260,_tmp260 + 58}};
# 1169
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp261=yy1;struct _tuple22*_tmp262;_LL14C: if((_tmp261.YY14).tag != 22)goto _LL14E;_tmp262=(struct _tuple22*)(_tmp261.YY14).val;_LL14D:
# 1173
 yyzzz=_tmp262;
goto _LL14B;_LL14E:;_LL14F:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL14B:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY14).val=yy1,(((_tmp9E7.YY14).tag=22,_tmp9E7)));}static char _tmp265[9]="fndecl_t";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 9}};
# 1170
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp266=yy1;struct Cyc_Absyn_Fndecl*_tmp267;_LL151: if((_tmp266.YY15).tag != 23)goto _LL153;_tmp267=(struct Cyc_Absyn_Fndecl*)(_tmp266.YY15).val;_LL152:
# 1174
 yyzzz=_tmp267;
goto _LL150;_LL153:;_LL154:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL150:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9E8;return((_tmp9E8.YY15).val=yy1,(((_tmp9E8.YY15).tag=23,_tmp9E8)));}static char _tmp26A[18]="list_t<decl_t,`H>";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 18}};
# 1171
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26B=yy1;struct Cyc_List_List*_tmp26C;_LL156: if((_tmp26B.YY16).tag != 24)goto _LL158;_tmp26C=(struct Cyc_List_List*)(_tmp26B.YY16).val;_LL157:
# 1175
 yyzzz=_tmp26C;
goto _LL155;_LL158:;_LL159:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL155:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.YY16).val=yy1,(((_tmp9E9.YY16).tag=24,_tmp9E9)));}static char _tmp26F[12]="decl_spec_t";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 12}};
# 1173
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp270=yy1;struct Cyc_Parse_Declaration_spec _tmp271;_LL15B: if((_tmp270.YY17).tag != 25)goto _LL15D;_tmp271=(struct Cyc_Parse_Declaration_spec)(_tmp270.YY17).val;_LL15C:
# 1177
 yyzzz=_tmp271;
goto _LL15A;_LL15D:;_LL15E:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL15A:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY17).val=yy1,(((_tmp9EA.YY17).tag=25,_tmp9EA)));}static char _tmp274[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 31}};
# 1174
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp275=yy1;struct _tuple11 _tmp276;_LL160: if((_tmp275.YY18).tag != 26)goto _LL162;_tmp276=(struct _tuple11)(_tmp275.YY18).val;_LL161:
# 1178
 yyzzz=_tmp276;
goto _LL15F;_LL162:;_LL163:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL15F:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY18).val=yy1,(((_tmp9EB.YY18).tag=26,_tmp9EB)));}static char _tmp279[23]="declarator_list_t<`yy>";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 23}};
# 1175
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp27A=yy1;struct _tuple12*_tmp27B;_LL165: if((_tmp27A.YY19).tag != 27)goto _LL167;_tmp27B=(struct _tuple12*)(_tmp27A.YY19).val;_LL166:
# 1179
 yyzzz=_tmp27B;
goto _LL164;_LL167:;_LL168:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL164:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY19).val=yy1,(((_tmp9EC.YY19).tag=27,_tmp9EC)));}static char _tmp27E[19]="storage_class_t@`H";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 19}};
# 1176
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp27F=yy1;enum Cyc_Parse_Storage_class*_tmp280;_LL16A: if((_tmp27F.YY20).tag != 28)goto _LL16C;_tmp280=(enum Cyc_Parse_Storage_class*)(_tmp27F.YY20).val;_LL16B:
# 1180
 yyzzz=_tmp280;
goto _LL169;_LL16C:;_LL16D:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL169:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY20).val=yy1,(((_tmp9ED.YY20).tag=28,_tmp9ED)));}static char _tmp283[17]="type_specifier_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp283,_tmp283,_tmp283 + 17}};
# 1177
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_Parse_Type_specifier _tmp285;_LL16F: if((_tmp284.YY21).tag != 29)goto _LL171;_tmp285=(struct Cyc_Parse_Type_specifier)(_tmp284.YY21).val;_LL170:
# 1181
 yyzzz=_tmp285;
goto _LL16E;_LL171:;_LL172:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL16E:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.YY21).val=yy1,(((_tmp9EE.YY21).tag=29,_tmp9EE)));}static char _tmp288[12]="aggr_kind_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 12}};
# 1179
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp289=yy1;enum Cyc_Absyn_AggrKind _tmp28A;_LL174: if((_tmp289.YY22).tag != 30)goto _LL176;_tmp28A=(enum Cyc_Absyn_AggrKind)(_tmp289.YY22).val;_LL175:
# 1183
 yyzzz=_tmp28A;
goto _LL173;_LL176:;_LL177:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL173:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9EF;return((_tmp9EF.YY22).val=yy1,(((_tmp9EF.YY22).tag=30,_tmp9EF)));}static char _tmp28D[8]="tqual_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 8}};
# 1180
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp28E=yy1;struct Cyc_Absyn_Tqual _tmp28F;_LL179: if((_tmp28E.YY23).tag != 31)goto _LL17B;_tmp28F=(struct Cyc_Absyn_Tqual)(_tmp28E.YY23).val;_LL17A:
# 1184
 yyzzz=_tmp28F;
goto _LL178;_LL17B:;_LL17C:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL178:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY23).val=yy1,(((_tmp9F0.YY23).tag=31,_tmp9F0)));}static char _tmp292[23]="list_t<aggrfield_t,`H>";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 23}};
# 1181
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp293=yy1;struct Cyc_List_List*_tmp294;_LL17E: if((_tmp293.YY24).tag != 32)goto _LL180;_tmp294=(struct Cyc_List_List*)(_tmp293.YY24).val;_LL17F:
# 1185
 yyzzz=_tmp294;
goto _LL17D;_LL180:;_LL181:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL17D:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY24).val=yy1,(((_tmp9F1.YY24).tag=32,_tmp9F1)));}static char _tmp297[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp297,_tmp297,_tmp297 + 34}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp298=yy1;struct Cyc_List_List*_tmp299;_LL183: if((_tmp298.YY25).tag != 33)goto _LL185;_tmp299=(struct Cyc_List_List*)(_tmp298.YY25).val;_LL184:
# 1186
 yyzzz=_tmp299;
goto _LL182;_LL185:;_LL186:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL182:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY25).val=yy1,(((_tmp9F2.YY25).tag=33,_tmp9F2)));}static char _tmp29C[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 33}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29D=yy1;struct Cyc_List_List*_tmp29E;_LL188: if((_tmp29D.YY26).tag != 34)goto _LL18A;_tmp29E=(struct Cyc_List_List*)(_tmp29D.YY26).val;_LL189:
# 1187
 yyzzz=_tmp29E;
goto _LL187;_LL18A:;_LL18B:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL187:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY26).val=yy1,(((_tmp9F3.YY26).tag=34,_tmp9F3)));}static char _tmp2A1[18]="declarator_t<`yy>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 18}};
# 1184
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp2A2=yy1;struct Cyc_Parse_Declarator _tmp2A3;_LL18D: if((_tmp2A2.YY27).tag != 35)goto _LL18F;_tmp2A3=(struct Cyc_Parse_Declarator)(_tmp2A2.YY27).val;_LL18E:
# 1188
 yyzzz=_tmp2A3;
goto _LL18C;_LL18F:;_LL190:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL18C:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY27).val=yy1,(((_tmp9F4.YY27).tag=35,_tmp9F4)));}static char _tmp2A6[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 45}};
# 1185
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp2A7=yy1;struct _tuple24*_tmp2A8;_LL192: if((_tmp2A7.YY28).tag != 36)goto _LL194;_tmp2A8=(struct _tuple24*)(_tmp2A7.YY28).val;_LL193:
# 1189
 yyzzz=_tmp2A8;
goto _LL191;_LL194:;_LL195:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL191:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9F5;return((_tmp9F5.YY28).val=yy1,(((_tmp9F5.YY28).tag=36,_tmp9F5)));}static char _tmp2AB[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 57}};
# 1186
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_List_List*_tmp2AD;_LL197: if((_tmp2AC.YY29).tag != 37)goto _LL199;_tmp2AD=(struct Cyc_List_List*)(_tmp2AC.YY29).val;_LL198:
# 1190
 yyzzz=_tmp2AD;
goto _LL196;_LL199:;_LL19A:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL196:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F6;return((_tmp9F6.YY29).val=yy1,(((_tmp9F6.YY29).tag=37,_tmp9F6)));}static char _tmp2B0[26]="abstractdeclarator_t<`yy>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 26}};
# 1187
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp2B1=yy1;struct Cyc_Parse_Abstractdeclarator _tmp2B2;_LL19C: if((_tmp2B1.YY30).tag != 38)goto _LL19E;_tmp2B2=(struct Cyc_Parse_Abstractdeclarator)(_tmp2B1.YY30).val;_LL19D:
# 1191
 yyzzz=_tmp2B2;
goto _LL19B;_LL19E:;_LL19F:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL19B:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY30).val=yy1,(((_tmp9F7.YY30).tag=38,_tmp9F7)));}static char _tmp2B5[5]="bool";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2B5,_tmp2B5,_tmp2B5 + 5}};
# 1188
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp2B6=yy1;int _tmp2B7;_LL1A1: if((_tmp2B6.YY31).tag != 39)goto _LL1A3;_tmp2B7=(int)(_tmp2B6.YY31).val;_LL1A2:
# 1192
 yyzzz=_tmp2B7;
goto _LL1A0;_LL1A3:;_LL1A4:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL1A0:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9F8;return((_tmp9F8.YY31).val=yy1,(((_tmp9F8.YY31).tag=39,_tmp9F8)));}static char _tmp2BA[8]="scope_t";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 8}};
# 1189
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp2BB=yy1;enum Cyc_Absyn_Scope _tmp2BC;_LL1A6: if((_tmp2BB.YY32).tag != 40)goto _LL1A8;_tmp2BC=(enum Cyc_Absyn_Scope)(_tmp2BB.YY32).val;_LL1A7:
# 1193
 yyzzz=_tmp2BC;
goto _LL1A5;_LL1A8:;_LL1A9:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL1A5:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9F9;return((_tmp9F9.YY32).val=yy1,(((_tmp9F9.YY32).tag=40,_tmp9F9)));}static char _tmp2BF[16]="datatypefield_t";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 16}};
# 1190
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp2C0=yy1;struct Cyc_Absyn_Datatypefield*_tmp2C1;_LL1AB: if((_tmp2C0.YY33).tag != 41)goto _LL1AD;_tmp2C1=(struct Cyc_Absyn_Datatypefield*)(_tmp2C0.YY33).val;_LL1AC:
# 1194
 yyzzz=_tmp2C1;
goto _LL1AA;_LL1AD:;_LL1AE:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL1AA:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9FA;return((_tmp9FA.YY33).val=yy1,(((_tmp9FA.YY33).tag=41,_tmp9FA)));}static char _tmp2C4[27]="list_t<datatypefield_t,`H>";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 27}};
# 1191
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C5=yy1;struct Cyc_List_List*_tmp2C6;_LL1B0: if((_tmp2C5.YY34).tag != 42)goto _LL1B2;_tmp2C6=(struct Cyc_List_List*)(_tmp2C5.YY34).val;_LL1B1:
# 1195
 yyzzz=_tmp2C6;
goto _LL1AF;_LL1B2:;_LL1B3:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL1AF:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FB;return((_tmp9FB.YY34).val=yy1,(((_tmp9FB.YY34).tag=42,_tmp9FB)));}static char _tmp2C9[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1190 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 41}};
# 1192
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2CA=yy1;struct _tuple25 _tmp2CB;_LL1B5: if((_tmp2CA.YY35).tag != 43)goto _LL1B7;_tmp2CB=(struct _tuple25)(_tmp2CA.YY35).val;_LL1B6:
# 1196
 yyzzz=_tmp2CB;
goto _LL1B4;_LL1B7:;_LL1B8:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL1B4:;}
# 1200
return yyzzz;}
# 1202
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9FC;return((_tmp9FC.YY35).val=yy1,(((_tmp9FC.YY35).tag=43,_tmp9FC)));}static char _tmp2CE[17]="list_t<var_t,`H>";
# 1191 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 17}};
# 1193
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_List_List*_tmp2D0;_LL1BA: if((_tmp2CF.YY36).tag != 44)goto _LL1BC;_tmp2D0=(struct Cyc_List_List*)(_tmp2CF.YY36).val;_LL1BB:
# 1197
 yyzzz=_tmp2D0;
goto _LL1B9;_LL1BC:;_LL1BD:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B9:;}
# 1201
return yyzzz;}
# 1203
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FD;return((_tmp9FD.YY36).val=yy1,(((_tmp9FD.YY36).tag=44,_tmp9FD)));}static char _tmp2D3[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1192 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 31}};
# 1194
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2D4=yy1;struct _tuple8*_tmp2D5;_LL1BF: if((_tmp2D4.YY37).tag != 45)goto _LL1C1;_tmp2D5=(struct _tuple8*)(_tmp2D4.YY37).val;_LL1C0:
# 1198
 yyzzz=_tmp2D5;
goto _LL1BE;_LL1C1:;_LL1C2:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1BE:;}
# 1202
return yyzzz;}
# 1204
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9FE;return((_tmp9FE.YY37).val=yy1,(((_tmp9FE.YY37).tag=45,_tmp9FE)));}static char _tmp2D8[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1193 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2D8,_tmp2D8,_tmp2D8 + 42}};
# 1195
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D9=yy1;struct Cyc_List_List*_tmp2DA;_LL1C4: if((_tmp2D9.YY38).tag != 46)goto _LL1C6;_tmp2DA=(struct Cyc_List_List*)(_tmp2D9.YY38).val;_LL1C5:
# 1199
 yyzzz=_tmp2DA;
goto _LL1C3;_LL1C6:;_LL1C7:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1C3:;}
# 1203
return yyzzz;}
# 1205
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FF;return((_tmp9FF.YY38).val=yy1,(((_tmp9FF.YY38).tag=46,_tmp9FF)));}static char _tmp2DD[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1194 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 115}};
# 1196
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2DE=yy1;struct _tuple26*_tmp2DF;_LL1C9: if((_tmp2DE.YY39).tag != 47)goto _LL1CB;_tmp2DF=(struct _tuple26*)(_tmp2DE.YY39).val;_LL1CA:
# 1200
 yyzzz=_tmp2DF;
goto _LL1C8;_LL1CB:;_LL1CC:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1C8:;}
# 1204
return yyzzz;}
# 1206
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){union Cyc_YYSTYPE _tmpA00;return((_tmpA00.YY39).val=yy1,(((_tmpA00.YY39).tag=47,_tmpA00)));}static char _tmp2E2[18]="list_t<type_t,`H>";
# 1195 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 18}};
# 1197
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E3=yy1;struct Cyc_List_List*_tmp2E4;_LL1CE: if((_tmp2E3.YY40).tag != 48)goto _LL1D0;_tmp2E4=(struct Cyc_List_List*)(_tmp2E3.YY40).val;_LL1CF:
# 1201
 yyzzz=_tmp2E4;
goto _LL1CD;_LL1D0:;_LL1D1:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1CD:;}
# 1205
return yyzzz;}
# 1207
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA01;return((_tmpA01.YY40).val=yy1,(((_tmpA01.YY40).tag=48,_tmpA01)));}static char _tmp2E7[24]="list_t<designator_t,`H>";
# 1197 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 24}};
# 1199
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E8=yy1;struct Cyc_List_List*_tmp2E9;_LL1D3: if((_tmp2E8.YY41).tag != 49)goto _LL1D5;_tmp2E9=(struct Cyc_List_List*)(_tmp2E8.YY41).val;_LL1D4:
# 1203
 yyzzz=_tmp2E9;
goto _LL1D2;_LL1D5:;_LL1D6:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1D2:;}
# 1207
return yyzzz;}
# 1209
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY41).val=yy1,(((_tmpA02.YY41).tag=49,_tmpA02)));}static char _tmp2EC[13]="designator_t";
# 1198 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 13}};
# 1200
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2ED=yy1;void*_tmp2EE;_LL1D8: if((_tmp2ED.YY42).tag != 50)goto _LL1DA;_tmp2EE=(void*)(_tmp2ED.YY42).val;_LL1D9:
# 1204
 yyzzz=_tmp2EE;
goto _LL1D7;_LL1DA:;_LL1DB:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1D7:;}
# 1208
return yyzzz;}
# 1210
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmpA03;return((_tmpA03.YY42).val=yy1,(((_tmpA03.YY42).tag=50,_tmpA03)));}static char _tmp2F1[7]="kind_t";
# 1199 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 7}};
# 1201
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2F2=yy1;struct Cyc_Absyn_Kind*_tmp2F3;_LL1DD: if((_tmp2F2.YY43).tag != 51)goto _LL1DF;_tmp2F3=(struct Cyc_Absyn_Kind*)(_tmp2F2.YY43).val;_LL1DE:
# 1205
 yyzzz=_tmp2F3;
goto _LL1DC;_LL1DF:;_LL1E0:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1DC:;}
# 1209
return yyzzz;}
# 1211
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmpA04;return((_tmpA04.YY43).val=yy1,(((_tmpA04.YY43).tag=51,_tmpA04)));}static char _tmp2F6[7]="type_t";
# 1200 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,_tmp2F6 + 7}};
# 1202
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2F7=yy1;void*_tmp2F8;_LL1E2: if((_tmp2F7.YY44).tag != 52)goto _LL1E4;_tmp2F8=(void*)(_tmp2F7.YY44).val;_LL1E3:
# 1206
 yyzzz=_tmp2F8;
goto _LL1E1;_LL1E4:;_LL1E5:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1E1:;}
# 1210
return yyzzz;}
# 1212
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmpA05;return((_tmpA05.YY44).val=yy1,(((_tmpA05.YY44).tag=52,_tmpA05)));}static char _tmp2FB[23]="list_t<attribute_t,`H>";
# 1201 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2FB,_tmp2FB,_tmp2FB + 23}};
# 1203
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2FC=yy1;struct Cyc_List_List*_tmp2FD;_LL1E7: if((_tmp2FC.YY45).tag != 53)goto _LL1E9;_tmp2FD=(struct Cyc_List_List*)(_tmp2FC.YY45).val;_LL1E8:
# 1207
 yyzzz=_tmp2FD;
goto _LL1E6;_LL1E9:;_LL1EA:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1E6:;}
# 1211
return yyzzz;}
# 1213
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA06;return((_tmpA06.YY45).val=yy1,(((_tmpA06.YY45).tag=53,_tmpA06)));}static char _tmp300[12]="attribute_t";
# 1202 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp300,_tmp300,_tmp300 + 12}};
# 1204
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp301=yy1;void*_tmp302;_LL1EC: if((_tmp301.YY46).tag != 54)goto _LL1EE;_tmp302=(void*)(_tmp301.YY46).val;_LL1ED:
# 1208
 yyzzz=_tmp302;
goto _LL1EB;_LL1EE:;_LL1EF:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1EB:;}
# 1212
return yyzzz;}
# 1214
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmpA07;return((_tmpA07.YY46).val=yy1,(((_tmpA07.YY46).tag=54,_tmpA07)));}static char _tmp305[12]="enumfield_t";
# 1203 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp305,_tmp305,_tmp305 + 12}};
# 1205
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp306=yy1;struct Cyc_Absyn_Enumfield*_tmp307;_LL1F1: if((_tmp306.YY47).tag != 55)goto _LL1F3;_tmp307=(struct Cyc_Absyn_Enumfield*)(_tmp306.YY47).val;_LL1F2:
# 1209
 yyzzz=_tmp307;
goto _LL1F0;_LL1F3:;_LL1F4:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1F0:;}
# 1213
return yyzzz;}
# 1215
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmpA08;return((_tmpA08.YY47).val=yy1,(((_tmpA08.YY47).tag=55,_tmpA08)));}static char _tmp30A[23]="list_t<enumfield_t,`H>";
# 1204 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp30A,_tmp30A,_tmp30A + 23}};
# 1206
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp30B=yy1;struct Cyc_List_List*_tmp30C;_LL1F6: if((_tmp30B.YY48).tag != 56)goto _LL1F8;_tmp30C=(struct Cyc_List_List*)(_tmp30B.YY48).val;_LL1F7:
# 1210
 yyzzz=_tmp30C;
goto _LL1F5;_LL1F8:;_LL1F9:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1F5:;}
# 1214
return yyzzz;}
# 1216
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA09;return((_tmpA09.YY48).val=yy1,(((_tmpA09.YY48).tag=56,_tmpA09)));}static char _tmp30F[11]="type_opt_t";
# 1205 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp30F,_tmp30F,_tmp30F + 11}};
# 1207
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp310=yy1;void*_tmp311;_LL1FB: if((_tmp310.YY49).tag != 57)goto _LL1FD;_tmp311=(void*)(_tmp310.YY49).val;_LL1FC:
# 1211
 yyzzz=_tmp311;
goto _LL1FA;_LL1FD:;_LL1FE:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1FA:;}
# 1215
return yyzzz;}
# 1217
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmpA0A;return((_tmpA0A.YY49).val=yy1,(((_tmpA0A.YY49).tag=57,_tmpA0A)));}static char _tmp314[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1206 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp314,_tmp314,_tmp314 + 31}};
# 1208
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp315=yy1;struct Cyc_List_List*_tmp316;_LL200: if((_tmp315.YY50).tag != 58)goto _LL202;_tmp316=(struct Cyc_List_List*)(_tmp315.YY50).val;_LL201:
# 1212
 yyzzz=_tmp316;
goto _LL1FF;_LL202:;_LL203:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1FF:;}
# 1216
return yyzzz;}
# 1218
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0B;return((_tmpA0B.YY50).val=yy1,(((_tmpA0B.YY50).tag=58,_tmpA0B)));}static char _tmp319[15]="conref_t<bool>";
# 1207 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp319,_tmp319,_tmp319 + 15}};
# 1209
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp31A=yy1;union Cyc_Absyn_Constraint*_tmp31B;_LL205: if((_tmp31A.YY51).tag != 59)goto _LL207;_tmp31B=(union Cyc_Absyn_Constraint*)(_tmp31A.YY51).val;_LL206:
# 1213
 yyzzz=_tmp31B;
goto _LL204;_LL207:;_LL208:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL204:;}
# 1217
return yyzzz;}
# 1219
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmpA0C;return((_tmpA0C.YY51).val=yy1,(((_tmpA0C.YY51).tag=59,_tmpA0C)));}static char _tmp31E[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1208 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp31E,_tmp31E,_tmp31E + 45}};
# 1210
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp31F=yy1;struct Cyc_List_List*_tmp320;_LL20A: if((_tmp31F.YY52).tag != 60)goto _LL20C;_tmp320=(struct Cyc_List_List*)(_tmp31F.YY52).val;_LL20B:
# 1214
 yyzzz=_tmp320;
goto _LL209;_LL20C:;_LL20D:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL209:;}
# 1218
return yyzzz;}
# 1220
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0D;return((_tmpA0D.YY52).val=yy1,(((_tmpA0D.YY52).tag=60,_tmpA0D)));}static char _tmp323[20]="pointer_qual_t<`yy>";
# 1209 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp323,_tmp323,_tmp323 + 20}};
# 1211
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp324=yy1;void*_tmp325;_LL20F: if((_tmp324.YY53).tag != 61)goto _LL211;_tmp325=(void*)(_tmp324.YY53).val;_LL210:
# 1215
 yyzzz=_tmp325;
goto _LL20E;_LL211:;_LL212:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL20E:;}
# 1219
return yyzzz;}
# 1221
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA0E;return((_tmpA0E.YY53).val=yy1,(((_tmpA0E.YY53).tag=61,_tmpA0E)));}static char _tmp328[21]="pointer_quals_t<`yy>";
# 1210 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp328,_tmp328,_tmp328 + 21}};
# 1212
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp329=yy1;struct Cyc_List_List*_tmp32A;_LL214: if((_tmp329.YY54).tag != 62)goto _LL216;_tmp32A=(struct Cyc_List_List*)(_tmp329.YY54).val;_LL215:
# 1216
 yyzzz=_tmp32A;
goto _LL213;_LL216:;_LL217:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL213:;}
# 1220
return yyzzz;}
# 1222
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0F;return((_tmpA0F.YY54).val=yy1,(((_tmpA0F.YY54).tag=62,_tmpA0F)));}static char _tmp32D[21]="$(bool,string_t<`H>)";
# 1211 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp32D,_tmp32D,_tmp32D + 21}};
# 1213
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp32E=yy1;struct _tuple20 _tmp32F;_LL219: if((_tmp32E.Asm_tok).tag != 6)goto _LL21B;_tmp32F=(struct _tuple20)(_tmp32E.Asm_tok).val;_LL21A:
# 1217
 yyzzz=_tmp32F;
goto _LL218;_LL21B:;_LL21C:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL218:;}
# 1221
return yyzzz;}
# 1223
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmpA10;return((_tmpA10.Asm_tok).val=yy1,(((_tmpA10.Asm_tok).tag=6,_tmpA10)));}static char _tmp332[10]="exp_opt_t";
# 1212 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp332,_tmp332,_tmp332 + 10}};
# 1214
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp333=yy1;struct Cyc_Absyn_Exp*_tmp334;_LL21E: if((_tmp333.YY55).tag != 63)goto _LL220;_tmp334=(struct Cyc_Absyn_Exp*)(_tmp333.YY55).val;_LL21F:
# 1218
 yyzzz=_tmp334;
goto _LL21D;_LL220:;_LL221:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL21D:;}
# 1222
return yyzzz;}
# 1224
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmpA11;return((_tmpA11.YY55).val=yy1,(((_tmpA11.YY55).tag=63,_tmpA11)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1236
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1240
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmpA12;return(_tmpA12.timestamp=0,((_tmpA12.first_line=0,((_tmpA12.first_column=0,((_tmpA12.last_line=0,((_tmpA12.last_column=0,_tmpA12)))))))));}
# 1243
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1254 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp337[2]="$";static char _tmp338[6]="error";static char _tmp339[12]="$undefined.";static char _tmp33A[5]="AUTO";static char _tmp33B[9]="REGISTER";static char _tmp33C[7]="STATIC";static char _tmp33D[7]="EXTERN";static char _tmp33E[8]="TYPEDEF";static char _tmp33F[5]="VOID";static char _tmp340[5]="CHAR";static char _tmp341[6]="SHORT";static char _tmp342[4]="INT";static char _tmp343[5]="LONG";static char _tmp344[6]="FLOAT";static char _tmp345[7]="DOUBLE";static char _tmp346[7]="SIGNED";static char _tmp347[9]="UNSIGNED";static char _tmp348[6]="CONST";static char _tmp349[9]="VOLATILE";static char _tmp34A[9]="RESTRICT";static char _tmp34B[7]="STRUCT";static char _tmp34C[6]="UNION";static char _tmp34D[5]="CASE";static char _tmp34E[8]="DEFAULT";static char _tmp34F[7]="INLINE";static char _tmp350[7]="SIZEOF";static char _tmp351[9]="OFFSETOF";static char _tmp352[3]="IF";static char _tmp353[5]="ELSE";static char _tmp354[7]="SWITCH";static char _tmp355[6]="WHILE";static char _tmp356[3]="DO";static char _tmp357[4]="FOR";static char _tmp358[5]="GOTO";static char _tmp359[9]="CONTINUE";static char _tmp35A[6]="BREAK";static char _tmp35B[7]="RETURN";static char _tmp35C[5]="ENUM";static char _tmp35D[7]="TYPEOF";static char _tmp35E[8]="NULL_kw";static char _tmp35F[4]="LET";static char _tmp360[6]="THROW";static char _tmp361[4]="TRY";static char _tmp362[6]="CATCH";static char _tmp363[7]="EXPORT";static char _tmp364[4]="NEW";static char _tmp365[9]="ABSTRACT";static char _tmp366[9]="FALLTHRU";static char _tmp367[6]="USING";static char _tmp368[10]="NAMESPACE";static char _tmp369[9]="DATATYPE";static char _tmp36A[7]="MALLOC";static char _tmp36B[8]="RMALLOC";static char _tmp36C[7]="CALLOC";static char _tmp36D[8]="RCALLOC";static char _tmp36E[5]="SWAP";static char _tmp36F[9]="REGION_T";static char _tmp370[6]="TAG_T";static char _tmp371[7]="REGION";static char _tmp372[5]="RNEW";static char _tmp373[8]="REGIONS";static char _tmp374[13]="RESET_REGION";static char _tmp375[7]="PORTON";static char _tmp376[8]="PORTOFF";static char _tmp377[12]="DYNREGION_T";static char _tmp378[8]="NUMELTS";static char _tmp379[8]="VALUEOF";static char _tmp37A[10]="VALUEOF_T";static char _tmp37B[9]="TAGCHECK";static char _tmp37C[13]="NUMELTS_QUAL";static char _tmp37D[10]="THIN_QUAL";static char _tmp37E[9]="FAT_QUAL";static char _tmp37F[13]="NOTNULL_QUAL";static char _tmp380[14]="NULLABLE_QUAL";static char _tmp381[14]="REQUIRES_QUAL";static char _tmp382[13]="ENSURES_QUAL";static char _tmp383[12]="REGION_QUAL";static char _tmp384[16]="NOZEROTERM_QUAL";static char _tmp385[14]="ZEROTERM_QUAL";static char _tmp386[12]="TAGGED_QUAL";static char _tmp387[16]="EXTENSIBLE_QUAL";static char _tmp388[15]="RESETABLE_QUAL";static char _tmp389[7]="PTR_OP";static char _tmp38A[7]="INC_OP";static char _tmp38B[7]="DEC_OP";static char _tmp38C[8]="LEFT_OP";static char _tmp38D[9]="RIGHT_OP";static char _tmp38E[6]="LE_OP";static char _tmp38F[6]="GE_OP";static char _tmp390[6]="EQ_OP";static char _tmp391[6]="NE_OP";static char _tmp392[7]="AND_OP";static char _tmp393[6]="OR_OP";static char _tmp394[11]="MUL_ASSIGN";static char _tmp395[11]="DIV_ASSIGN";static char _tmp396[11]="MOD_ASSIGN";static char _tmp397[11]="ADD_ASSIGN";static char _tmp398[11]="SUB_ASSIGN";static char _tmp399[12]="LEFT_ASSIGN";static char _tmp39A[13]="RIGHT_ASSIGN";static char _tmp39B[11]="AND_ASSIGN";static char _tmp39C[11]="XOR_ASSIGN";static char _tmp39D[10]="OR_ASSIGN";static char _tmp39E[9]="ELLIPSIS";static char _tmp39F[11]="LEFT_RIGHT";static char _tmp3A0[12]="COLON_COLON";static char _tmp3A1[11]="IDENTIFIER";static char _tmp3A2[17]="INTEGER_CONSTANT";static char _tmp3A3[7]="STRING";static char _tmp3A4[8]="WSTRING";static char _tmp3A5[19]="CHARACTER_CONSTANT";static char _tmp3A6[20]="WCHARACTER_CONSTANT";static char _tmp3A7[18]="FLOATING_CONSTANT";static char _tmp3A8[9]="TYPE_VAR";static char _tmp3A9[13]="TYPEDEF_NAME";static char _tmp3AA[16]="QUAL_IDENTIFIER";static char _tmp3AB[18]="QUAL_TYPEDEF_NAME";static char _tmp3AC[10]="ATTRIBUTE";static char _tmp3AD[4]="ASM";static char _tmp3AE[4]="';'";static char _tmp3AF[4]="'{'";static char _tmp3B0[4]="'}'";static char _tmp3B1[4]="','";static char _tmp3B2[4]="'='";static char _tmp3B3[4]="'<'";static char _tmp3B4[4]="'>'";static char _tmp3B5[4]="'('";static char _tmp3B6[4]="')'";static char _tmp3B7[4]="'_'";static char _tmp3B8[4]="'$'";static char _tmp3B9[4]="':'";static char _tmp3BA[4]="'.'";static char _tmp3BB[4]="'['";static char _tmp3BC[4]="']'";static char _tmp3BD[4]="'*'";static char _tmp3BE[4]="'@'";static char _tmp3BF[4]="'?'";static char _tmp3C0[4]="'+'";static char _tmp3C1[4]="'|'";static char _tmp3C2[4]="'^'";static char _tmp3C3[4]="'&'";static char _tmp3C4[4]="'-'";static char _tmp3C5[4]="'/'";static char _tmp3C6[4]="'%'";static char _tmp3C7[4]="'~'";static char _tmp3C8[4]="'!'";static char _tmp3C9[5]="prog";static char _tmp3CA[17]="translation_unit";static char _tmp3CB[16]="extern_c_action";static char _tmp3CC[13]="end_extern_c";static char _tmp3CD[12]="export_list";static char _tmp3CE[19]="export_list_values";static char _tmp3CF[21]="external_declaration";static char _tmp3D0[15]="optional_comma";static char _tmp3D1[20]="function_definition";static char _tmp3D2[21]="function_definition2";static char _tmp3D3[13]="using_action";static char _tmp3D4[15]="unusing_action";static char _tmp3D5[17]="namespace_action";static char _tmp3D6[19]="unnamespace_action";static char _tmp3D7[12]="declaration";static char _tmp3D8[19]="resetable_qual_opt";static char _tmp3D9[17]="declaration_list";static char _tmp3DA[23]="declaration_specifiers";static char _tmp3DB[24]="storage_class_specifier";static char _tmp3DC[15]="attributes_opt";static char _tmp3DD[11]="attributes";static char _tmp3DE[15]="attribute_list";static char _tmp3DF[10]="attribute";static char _tmp3E0[15]="type_specifier";static char _tmp3E1[25]="type_specifier_notypedef";static char _tmp3E2[5]="kind";static char _tmp3E3[15]="type_qualifier";static char _tmp3E4[15]="enum_specifier";static char _tmp3E5[11]="enum_field";static char _tmp3E6[22]="enum_declaration_list";static char _tmp3E7[26]="struct_or_union_specifier";static char _tmp3E8[16]="type_params_opt";static char _tmp3E9[16]="struct_or_union";static char _tmp3EA[24]="struct_declaration_list";static char _tmp3EB[25]="struct_declaration_list0";static char _tmp3EC[21]="init_declarator_list";static char _tmp3ED[22]="init_declarator_list0";static char _tmp3EE[16]="init_declarator";static char _tmp3EF[19]="struct_declaration";static char _tmp3F0[25]="specifier_qualifier_list";static char _tmp3F1[35]="notypedef_specifier_qualifier_list";static char _tmp3F2[23]="struct_declarator_list";static char _tmp3F3[24]="struct_declarator_list0";static char _tmp3F4[18]="struct_declarator";static char _tmp3F5[20]="requires_clause_opt";static char _tmp3F6[19]="ensures_clause_opt";static char _tmp3F7[19]="datatype_specifier";static char _tmp3F8[14]="qual_datatype";static char _tmp3F9[19]="datatypefield_list";static char _tmp3FA[20]="datatypefield_scope";static char _tmp3FB[14]="datatypefield";static char _tmp3FC[11]="declarator";static char _tmp3FD[23]="declarator_withtypedef";static char _tmp3FE[18]="direct_declarator";static char _tmp3FF[30]="direct_declarator_withtypedef";static char _tmp400[8]="pointer";static char _tmp401[12]="one_pointer";static char _tmp402[14]="pointer_quals";static char _tmp403[13]="pointer_qual";static char _tmp404[23]="pointer_null_and_bound";static char _tmp405[14]="pointer_bound";static char _tmp406[18]="zeroterm_qual_opt";static char _tmp407[8]="rgn_opt";static char _tmp408[11]="tqual_list";static char _tmp409[20]="parameter_type_list";static char _tmp40A[9]="type_var";static char _tmp40B[16]="optional_effect";static char _tmp40C[19]="optional_rgn_order";static char _tmp40D[10]="rgn_order";static char _tmp40E[16]="optional_inject";static char _tmp40F[11]="effect_set";static char _tmp410[14]="atomic_effect";static char _tmp411[11]="region_set";static char _tmp412[15]="parameter_list";static char _tmp413[22]="parameter_declaration";static char _tmp414[16]="identifier_list";static char _tmp415[17]="identifier_list0";static char _tmp416[12]="initializer";static char _tmp417[18]="array_initializer";static char _tmp418[17]="initializer_list";static char _tmp419[12]="designation";static char _tmp41A[16]="designator_list";static char _tmp41B[11]="designator";static char _tmp41C[10]="type_name";static char _tmp41D[14]="any_type_name";static char _tmp41E[15]="type_name_list";static char _tmp41F[20]="abstract_declarator";static char _tmp420[27]="direct_abstract_declarator";static char _tmp421[10]="statement";static char _tmp422[18]="labeled_statement";static char _tmp423[21]="expression_statement";static char _tmp424[19]="compound_statement";static char _tmp425[16]="block_item_list";static char _tmp426[20]="selection_statement";static char _tmp427[15]="switch_clauses";static char _tmp428[20]="iteration_statement";static char _tmp429[15]="jump_statement";static char _tmp42A[12]="exp_pattern";static char _tmp42B[20]="conditional_pattern";static char _tmp42C[19]="logical_or_pattern";static char _tmp42D[20]="logical_and_pattern";static char _tmp42E[21]="inclusive_or_pattern";static char _tmp42F[21]="exclusive_or_pattern";static char _tmp430[12]="and_pattern";static char _tmp431[17]="equality_pattern";static char _tmp432[19]="relational_pattern";static char _tmp433[14]="shift_pattern";static char _tmp434[17]="additive_pattern";static char _tmp435[23]="multiplicative_pattern";static char _tmp436[13]="cast_pattern";static char _tmp437[14]="unary_pattern";static char _tmp438[16]="postfix_pattern";static char _tmp439[16]="primary_pattern";static char _tmp43A[8]="pattern";static char _tmp43B[19]="tuple_pattern_list";static char _tmp43C[20]="tuple_pattern_list0";static char _tmp43D[14]="field_pattern";static char _tmp43E[19]="field_pattern_list";static char _tmp43F[20]="field_pattern_list0";static char _tmp440[11]="expression";static char _tmp441[22]="assignment_expression";static char _tmp442[20]="assignment_operator";static char _tmp443[23]="conditional_expression";static char _tmp444[20]="constant_expression";static char _tmp445[22]="logical_or_expression";static char _tmp446[23]="logical_and_expression";static char _tmp447[24]="inclusive_or_expression";static char _tmp448[24]="exclusive_or_expression";static char _tmp449[15]="and_expression";static char _tmp44A[20]="equality_expression";static char _tmp44B[22]="relational_expression";static char _tmp44C[17]="shift_expression";static char _tmp44D[20]="additive_expression";static char _tmp44E[26]="multiplicative_expression";static char _tmp44F[16]="cast_expression";static char _tmp450[17]="unary_expression";static char _tmp451[15]="unary_operator";static char _tmp452[19]="postfix_expression";static char _tmp453[17]="field_expression";static char _tmp454[19]="primary_expression";static char _tmp455[25]="argument_expression_list";static char _tmp456[26]="argument_expression_list0";static char _tmp457[9]="constant";static char _tmp458[20]="qual_opt_identifier";static char _tmp459[17]="qual_opt_typedef";static char _tmp45A[18]="struct_union_name";static char _tmp45B[11]="field_name";static char _tmp45C[12]="right_angle";
# 1608 "parse.y"
static struct _dyneither_ptr Cyc_yytname[294]={{_tmp337,_tmp337,_tmp337 + 2},{_tmp338,_tmp338,_tmp338 + 6},{_tmp339,_tmp339,_tmp339 + 12},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 7},{_tmp33D,_tmp33D,_tmp33D + 7},{_tmp33E,_tmp33E,_tmp33E + 8},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 5},{_tmp341,_tmp341,_tmp341 + 6},{_tmp342,_tmp342,_tmp342 + 4},{_tmp343,_tmp343,_tmp343 + 5},{_tmp344,_tmp344,_tmp344 + 6},{_tmp345,_tmp345,_tmp345 + 7},{_tmp346,_tmp346,_tmp346 + 7},{_tmp347,_tmp347,_tmp347 + 9},{_tmp348,_tmp348,_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 9},{_tmp34A,_tmp34A,_tmp34A + 9},{_tmp34B,_tmp34B,_tmp34B + 7},{_tmp34C,_tmp34C,_tmp34C + 6},{_tmp34D,_tmp34D,_tmp34D + 5},{_tmp34E,_tmp34E,_tmp34E + 8},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 7},{_tmp351,_tmp351,_tmp351 + 9},{_tmp352,_tmp352,_tmp352 + 3},{_tmp353,_tmp353,_tmp353 + 5},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 6},{_tmp356,_tmp356,_tmp356 + 3},{_tmp357,_tmp357,_tmp357 + 4},{_tmp358,_tmp358,_tmp358 + 5},{_tmp359,_tmp359,_tmp359 + 9},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 7},{_tmp35C,_tmp35C,_tmp35C + 5},{_tmp35D,_tmp35D,_tmp35D + 7},{_tmp35E,_tmp35E,_tmp35E + 8},{_tmp35F,_tmp35F,_tmp35F + 4},{_tmp360,_tmp360,_tmp360 + 6},{_tmp361,_tmp361,_tmp361 + 4},{_tmp362,_tmp362,_tmp362 + 6},{_tmp363,_tmp363,_tmp363 + 7},{_tmp364,_tmp364,_tmp364 + 4},{_tmp365,_tmp365,_tmp365 + 9},{_tmp366,_tmp366,_tmp366 + 9},{_tmp367,_tmp367,_tmp367 + 6},{_tmp368,_tmp368,_tmp368 + 10},{_tmp369,_tmp369,_tmp369 + 9},{_tmp36A,_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,_tmp36B + 8},{_tmp36C,_tmp36C,_tmp36C + 7},{_tmp36D,_tmp36D,_tmp36D + 8},{_tmp36E,_tmp36E,_tmp36E + 5},{_tmp36F,_tmp36F,_tmp36F + 9},{_tmp370,_tmp370,_tmp370 + 6},{_tmp371,_tmp371,_tmp371 + 7},{_tmp372,_tmp372,_tmp372 + 5},{_tmp373,_tmp373,_tmp373 + 8},{_tmp374,_tmp374,_tmp374 + 13},{_tmp375,_tmp375,_tmp375 + 7},{_tmp376,_tmp376,_tmp376 + 8},{_tmp377,_tmp377,_tmp377 + 12},{_tmp378,_tmp378,_tmp378 + 8},{_tmp379,_tmp379,_tmp379 + 8},{_tmp37A,_tmp37A,_tmp37A + 10},{_tmp37B,_tmp37B,_tmp37B + 9},{_tmp37C,_tmp37C,_tmp37C + 13},{_tmp37D,_tmp37D,_tmp37D + 10},{_tmp37E,_tmp37E,_tmp37E + 9},{_tmp37F,_tmp37F,_tmp37F + 13},{_tmp380,_tmp380,_tmp380 + 14},{_tmp381,_tmp381,_tmp381 + 14},{_tmp382,_tmp382,_tmp382 + 13},{_tmp383,_tmp383,_tmp383 + 12},{_tmp384,_tmp384,_tmp384 + 16},{_tmp385,_tmp385,_tmp385 + 14},{_tmp386,_tmp386,_tmp386 + 12},{_tmp387,_tmp387,_tmp387 + 16},{_tmp388,_tmp388,_tmp388 + 15},{_tmp389,_tmp389,_tmp389 + 7},{_tmp38A,_tmp38A,_tmp38A + 7},{_tmp38B,_tmp38B,_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 8},{_tmp38D,_tmp38D,_tmp38D + 9},{_tmp38E,_tmp38E,_tmp38E + 6},{_tmp38F,_tmp38F,_tmp38F + 6},{_tmp390,_tmp390,_tmp390 + 6},{_tmp391,_tmp391,_tmp391 + 6},{_tmp392,_tmp392,_tmp392 + 7},{_tmp393,_tmp393,_tmp393 + 6},{_tmp394,_tmp394,_tmp394 + 11},{_tmp395,_tmp395,_tmp395 + 11},{_tmp396,_tmp396,_tmp396 + 11},{_tmp397,_tmp397,_tmp397 + 11},{_tmp398,_tmp398,_tmp398 + 11},{_tmp399,_tmp399,_tmp399 + 12},{_tmp39A,_tmp39A,_tmp39A + 13},{_tmp39B,_tmp39B,_tmp39B + 11},{_tmp39C,_tmp39C,_tmp39C + 11},{_tmp39D,_tmp39D,_tmp39D + 10},{_tmp39E,_tmp39E,_tmp39E + 9},{_tmp39F,_tmp39F,_tmp39F + 11},{_tmp3A0,_tmp3A0,_tmp3A0 + 12},{_tmp3A1,_tmp3A1,_tmp3A1 + 11},{_tmp3A2,_tmp3A2,_tmp3A2 + 17},{_tmp3A3,_tmp3A3,_tmp3A3 + 7},{_tmp3A4,_tmp3A4,_tmp3A4 + 8},{_tmp3A5,_tmp3A5,_tmp3A5 + 19},{_tmp3A6,_tmp3A6,_tmp3A6 + 20},{_tmp3A7,_tmp3A7,_tmp3A7 + 18},{_tmp3A8,_tmp3A8,_tmp3A8 + 9},{_tmp3A9,_tmp3A9,_tmp3A9 + 13},{_tmp3AA,_tmp3AA,_tmp3AA + 16},{_tmp3AB,_tmp3AB,_tmp3AB + 18},{_tmp3AC,_tmp3AC,_tmp3AC + 10},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 4},{_tmp3B1,_tmp3B1,_tmp3B1 + 4},{_tmp3B2,_tmp3B2,_tmp3B2 + 4},{_tmp3B3,_tmp3B3,_tmp3B3 + 4},{_tmp3B4,_tmp3B4,_tmp3B4 + 4},{_tmp3B5,_tmp3B5,_tmp3B5 + 4},{_tmp3B6,_tmp3B6,_tmp3B6 + 4},{_tmp3B7,_tmp3B7,_tmp3B7 + 4},{_tmp3B8,_tmp3B8,_tmp3B8 + 4},{_tmp3B9,_tmp3B9,_tmp3B9 + 4},{_tmp3BA,_tmp3BA,_tmp3BA + 4},{_tmp3BB,_tmp3BB,_tmp3BB + 4},{_tmp3BC,_tmp3BC,_tmp3BC + 4},{_tmp3BD,_tmp3BD,_tmp3BD + 4},{_tmp3BE,_tmp3BE,_tmp3BE + 4},{_tmp3BF,_tmp3BF,_tmp3BF + 4},{_tmp3C0,_tmp3C0,_tmp3C0 + 4},{_tmp3C1,_tmp3C1,_tmp3C1 + 4},{_tmp3C2,_tmp3C2,_tmp3C2 + 4},{_tmp3C3,_tmp3C3,_tmp3C3 + 4},{_tmp3C4,_tmp3C4,_tmp3C4 + 4},{_tmp3C5,_tmp3C5,_tmp3C5 + 4},{_tmp3C6,_tmp3C6,_tmp3C6 + 4},{_tmp3C7,_tmp3C7,_tmp3C7 + 4},{_tmp3C8,_tmp3C8,_tmp3C8 + 4},{_tmp3C9,_tmp3C9,_tmp3C9 + 5},{_tmp3CA,_tmp3CA,_tmp3CA + 17},{_tmp3CB,_tmp3CB,_tmp3CB + 16},{_tmp3CC,_tmp3CC,_tmp3CC + 13},{_tmp3CD,_tmp3CD,_tmp3CD + 12},{_tmp3CE,_tmp3CE,_tmp3CE + 19},{_tmp3CF,_tmp3CF,_tmp3CF + 21},{_tmp3D0,_tmp3D0,_tmp3D0 + 15},{_tmp3D1,_tmp3D1,_tmp3D1 + 20},{_tmp3D2,_tmp3D2,_tmp3D2 + 21},{_tmp3D3,_tmp3D3,_tmp3D3 + 13},{_tmp3D4,_tmp3D4,_tmp3D4 + 15},{_tmp3D5,_tmp3D5,_tmp3D5 + 17},{_tmp3D6,_tmp3D6,_tmp3D6 + 19},{_tmp3D7,_tmp3D7,_tmp3D7 + 12},{_tmp3D8,_tmp3D8,_tmp3D8 + 19},{_tmp3D9,_tmp3D9,_tmp3D9 + 17},{_tmp3DA,_tmp3DA,_tmp3DA + 23},{_tmp3DB,_tmp3DB,_tmp3DB + 24},{_tmp3DC,_tmp3DC,_tmp3DC + 15},{_tmp3DD,_tmp3DD,_tmp3DD + 11},{_tmp3DE,_tmp3DE,_tmp3DE + 15},{_tmp3DF,_tmp3DF,_tmp3DF + 10},{_tmp3E0,_tmp3E0,_tmp3E0 + 15},{_tmp3E1,_tmp3E1,_tmp3E1 + 25},{_tmp3E2,_tmp3E2,_tmp3E2 + 5},{_tmp3E3,_tmp3E3,_tmp3E3 + 15},{_tmp3E4,_tmp3E4,_tmp3E4 + 15},{_tmp3E5,_tmp3E5,_tmp3E5 + 11},{_tmp3E6,_tmp3E6,_tmp3E6 + 22},{_tmp3E7,_tmp3E7,_tmp3E7 + 26},{_tmp3E8,_tmp3E8,_tmp3E8 + 16},{_tmp3E9,_tmp3E9,_tmp3E9 + 16},{_tmp3EA,_tmp3EA,_tmp3EA + 24},{_tmp3EB,_tmp3EB,_tmp3EB + 25},{_tmp3EC,_tmp3EC,_tmp3EC + 21},{_tmp3ED,_tmp3ED,_tmp3ED + 22},{_tmp3EE,_tmp3EE,_tmp3EE + 16},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{_tmp3F0,_tmp3F0,_tmp3F0 + 25},{_tmp3F1,_tmp3F1,_tmp3F1 + 35},{_tmp3F2,_tmp3F2,_tmp3F2 + 23},{_tmp3F3,_tmp3F3,_tmp3F3 + 24},{_tmp3F4,_tmp3F4,_tmp3F4 + 18},{_tmp3F5,_tmp3F5,_tmp3F5 + 20},{_tmp3F6,_tmp3F6,_tmp3F6 + 19},{_tmp3F7,_tmp3F7,_tmp3F7 + 19},{_tmp3F8,_tmp3F8,_tmp3F8 + 14},{_tmp3F9,_tmp3F9,_tmp3F9 + 19},{_tmp3FA,_tmp3FA,_tmp3FA + 20},{_tmp3FB,_tmp3FB,_tmp3FB + 14},{_tmp3FC,_tmp3FC,_tmp3FC + 11},{_tmp3FD,_tmp3FD,_tmp3FD + 23},{_tmp3FE,_tmp3FE,_tmp3FE + 18},{_tmp3FF,_tmp3FF,_tmp3FF + 30},{_tmp400,_tmp400,_tmp400 + 8},{_tmp401,_tmp401,_tmp401 + 12},{_tmp402,_tmp402,_tmp402 + 14},{_tmp403,_tmp403,_tmp403 + 13},{_tmp404,_tmp404,_tmp404 + 23},{_tmp405,_tmp405,_tmp405 + 14},{_tmp406,_tmp406,_tmp406 + 18},{_tmp407,_tmp407,_tmp407 + 8},{_tmp408,_tmp408,_tmp408 + 11},{_tmp409,_tmp409,_tmp409 + 20},{_tmp40A,_tmp40A,_tmp40A + 9},{_tmp40B,_tmp40B,_tmp40B + 16},{_tmp40C,_tmp40C,_tmp40C + 19},{_tmp40D,_tmp40D,_tmp40D + 10},{_tmp40E,_tmp40E,_tmp40E + 16},{_tmp40F,_tmp40F,_tmp40F + 11},{_tmp410,_tmp410,_tmp410 + 14},{_tmp411,_tmp411,_tmp411 + 11},{_tmp412,_tmp412,_tmp412 + 15},{_tmp413,_tmp413,_tmp413 + 22},{_tmp414,_tmp414,_tmp414 + 16},{_tmp415,_tmp415,_tmp415 + 17},{_tmp416,_tmp416,_tmp416 + 12},{_tmp417,_tmp417,_tmp417 + 18},{_tmp418,_tmp418,_tmp418 + 17},{_tmp419,_tmp419,_tmp419 + 12},{_tmp41A,_tmp41A,_tmp41A + 16},{_tmp41B,_tmp41B,_tmp41B + 11},{_tmp41C,_tmp41C,_tmp41C + 10},{_tmp41D,_tmp41D,_tmp41D + 14},{_tmp41E,_tmp41E,_tmp41E + 15},{_tmp41F,_tmp41F,_tmp41F + 20},{_tmp420,_tmp420,_tmp420 + 27},{_tmp421,_tmp421,_tmp421 + 10},{_tmp422,_tmp422,_tmp422 + 18},{_tmp423,_tmp423,_tmp423 + 21},{_tmp424,_tmp424,_tmp424 + 19},{_tmp425,_tmp425,_tmp425 + 16},{_tmp426,_tmp426,_tmp426 + 20},{_tmp427,_tmp427,_tmp427 + 15},{_tmp428,_tmp428,_tmp428 + 20},{_tmp429,_tmp429,_tmp429 + 15},{_tmp42A,_tmp42A,_tmp42A + 12},{_tmp42B,_tmp42B,_tmp42B + 20},{_tmp42C,_tmp42C,_tmp42C + 19},{_tmp42D,_tmp42D,_tmp42D + 20},{_tmp42E,_tmp42E,_tmp42E + 21},{_tmp42F,_tmp42F,_tmp42F + 21},{_tmp430,_tmp430,_tmp430 + 12},{_tmp431,_tmp431,_tmp431 + 17},{_tmp432,_tmp432,_tmp432 + 19},{_tmp433,_tmp433,_tmp433 + 14},{_tmp434,_tmp434,_tmp434 + 17},{_tmp435,_tmp435,_tmp435 + 23},{_tmp436,_tmp436,_tmp436 + 13},{_tmp437,_tmp437,_tmp437 + 14},{_tmp438,_tmp438,_tmp438 + 16},{_tmp439,_tmp439,_tmp439 + 16},{_tmp43A,_tmp43A,_tmp43A + 8},{_tmp43B,_tmp43B,_tmp43B + 19},{_tmp43C,_tmp43C,_tmp43C + 20},{_tmp43D,_tmp43D,_tmp43D + 14},{_tmp43E,_tmp43E,_tmp43E + 19},{_tmp43F,_tmp43F,_tmp43F + 20},{_tmp440,_tmp440,_tmp440 + 11},{_tmp441,_tmp441,_tmp441 + 22},{_tmp442,_tmp442,_tmp442 + 20},{_tmp443,_tmp443,_tmp443 + 23},{_tmp444,_tmp444,_tmp444 + 20},{_tmp445,_tmp445,_tmp445 + 22},{_tmp446,_tmp446,_tmp446 + 23},{_tmp447,_tmp447,_tmp447 + 24},{_tmp448,_tmp448,_tmp448 + 24},{_tmp449,_tmp449,_tmp449 + 15},{_tmp44A,_tmp44A,_tmp44A + 20},{_tmp44B,_tmp44B,_tmp44B + 22},{_tmp44C,_tmp44C,_tmp44C + 17},{_tmp44D,_tmp44D,_tmp44D + 20},{_tmp44E,_tmp44E,_tmp44E + 26},{_tmp44F,_tmp44F,_tmp44F + 16},{_tmp450,_tmp450,_tmp450 + 17},{_tmp451,_tmp451,_tmp451 + 15},{_tmp452,_tmp452,_tmp452 + 19},{_tmp453,_tmp453,_tmp453 + 17},{_tmp454,_tmp454,_tmp454 + 19},{_tmp455,_tmp455,_tmp455 + 25},{_tmp456,_tmp456,_tmp456 + 26},{_tmp457,_tmp457,_tmp457 + 9},{_tmp458,_tmp458,_tmp458 + 20},{_tmp459,_tmp459,_tmp459 + 17},{_tmp45A,_tmp45A,_tmp45A + 18},{_tmp45B,_tmp45B,_tmp45B + 11},{_tmp45C,_tmp45C,_tmp45C + 12}};
# 1663
static short Cyc_yyr1[528]={0,146,147,147,147,147,147,147,147,147,147,147,148,149,150,150,151,151,151,152,152,152,153,153,154,154,154,154,155,155,156,157,158,159,160,160,160,160,160,160,160,161,161,162,162,163,163,163,163,163,163,163,163,163,163,164,164,164,164,164,164,164,165,165,166,167,167,168,168,168,168,169,169,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,171,172,172,172,173,173,173,174,174,175,175,175,176,176,176,176,176,177,177,178,178,179,179,180,180,181,182,182,183,183,184,185,185,185,185,185,185,186,186,186,186,186,186,187,188,188,189,189,189,189,190,190,191,191,192,192,192,193,193,194,194,194,194,195,195,195,196,196,197,197,198,198,199,199,199,199,199,199,199,199,199,199,200,200,200,200,200,200,200,200,200,200,200,201,201,202,203,203,204,204,204,204,204,204,204,204,205,205,205,206,206,207,207,207,208,208,208,209,209,210,210,210,210,211,211,212,212,213,213,214,214,215,215,216,216,217,217,217,217,218,218,219,219,220,220,220,221,222,222,223,223,224,224,224,224,224,225,225,225,225,226,226,227,227,228,228,229,229,230,230,230,230,230,231,231,232,232,232,233,233,233,233,233,233,233,233,233,233,233,234,234,234,234,234,234,234,235,236,236,237,237,238,238,238,238,238,238,238,238,239,239,239,239,239,239,240,240,240,240,240,240,241,241,241,241,241,241,241,241,241,241,241,241,241,241,242,242,242,242,242,242,242,242,243,244,244,245,245,246,246,247,247,248,248,249,249,250,250,250,251,251,251,251,251,252,252,252,253,253,253,254,254,254,254,255,255,256,256,256,256,256,256,257,258,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,260,260,260,261,261,262,262,263,263,263,264,264,265,265,266,266,266,267,267,267,267,267,267,267,267,267,267,267,268,268,268,268,268,268,268,269,270,270,271,271,272,272,273,273,274,274,275,275,275,276,276,276,276,276,277,277,277,278,278,278,279,279,279,279,280,280,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,282,282,282,283,283,283,283,283,283,283,283,283,283,283,284,284,284,284,285,285,285,285,285,285,285,285,285,285,286,287,287,288,288,288,288,288,289,289,290,290,291,291,292,292,293,293};
# 1719
static short Cyc_yyr2[528]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1775
static short Cyc_yydefact[1070]={0,21,55,56,57,58,60,73,74,75,76,77,78,79,80,81,98,99,100,116,117,51,0,0,0,61,0,0,154,91,95,0,0,0,0,0,0,42,518,220,520,519,521,0,0,87,0,206,206,205,1,0,0,19,0,0,20,0,0,45,53,47,71,49,82,83,0,85,0,41,165,0,190,193,86,169,114,59,58,52,0,102,0,517,0,518,513,514,515,516,114,0,381,0,0,0,0,243,0,383,384,30,32,0,0,0,0,0,0,0,155,0,0,0,0,0,0,0,203,204,0,2,0,0,0,0,0,34,0,122,123,125,46,54,48,50,118,522,523,114,114,41,43,41,0,23,0,222,0,178,166,191,0,197,198,201,202,0,200,199,211,193,0,72,59,106,0,104,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,502,503,481,0,0,0,0,0,484,482,483,0,409,411,425,433,435,437,439,441,443,446,451,454,457,461,0,463,485,501,500,518,392,0,0,0,0,0,393,391,37,0,0,114,0,0,0,132,128,130,263,265,0,0,9,10,0,0,114,524,525,221,97,0,0,170,88,241,0,238,0,0,3,0,5,0,0,0,35,0,0,41,23,0,119,120,145,113,0,152,0,0,0,0,0,0,0,0,0,0,0,0,518,294,296,0,304,298,0,302,286,287,288,0,289,290,291,0,44,23,125,22,24,270,0,228,244,0,0,224,222,0,208,0,0,0,213,62,212,194,0,107,103,0,0,0,471,0,427,461,0,428,429,0,0,0,0,0,0,0,0,0,464,465,41,0,0,0,467,468,466,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,415,416,417,418,419,420,421,422,423,424,414,0,469,0,491,492,0,0,0,505,0,114,385,0,0,406,518,0,0,0,0,259,402,407,0,404,0,382,399,400,0,397,0,245,0,0,0,0,266,0,236,133,138,134,136,129,131,222,0,272,264,273,527,526,0,90,94,0,92,96,112,68,67,0,65,171,222,240,167,272,242,179,180,0,89,207,13,0,31,0,33,0,39,0,0,124,126,247,246,23,25,109,121,0,0,0,140,141,148,0,114,114,160,0,0,0,0,0,518,0,41,0,333,334,335,0,0,337,0,0,41,0,305,299,125,303,297,295,26,0,177,229,0,0,0,235,223,230,148,0,0,0,224,176,210,209,172,208,0,0,214,63,115,108,432,105,101,0,0,0,518,248,253,0,0,0,0,0,0,0,0,0,0,0,0,0,504,511,0,510,410,434,0,436,438,440,442,444,445,449,450,447,448,452,453,455,456,458,459,460,413,412,490,487,0,489,0,0,0,395,396,262,0,403,257,260,390,0,258,387,0,394,36,0,388,0,267,0,139,135,137,0,224,0,208,0,274,0,222,0,285,269,0,114,0,0,0,132,0,114,0,222,0,189,168,239,0,7,0,4,6,0,0,27,0,144,127,145,0,0,143,224,153,162,161,0,0,156,0,0,0,312,0,0,0,0,0,0,332,336,0,0,0,0,300,293,41,28,271,222,0,232,0,0,150,225,0,148,228,216,173,195,196,214,192,470,0,0,249,0,254,473,0,0,0,0,477,480,0,0,0,0,0,462,507,0,0,488,486,0,0,0,261,405,408,398,401,389,268,237,148,0,275,276,208,0,0,224,208,0,93,224,518,0,64,66,0,181,0,0,224,0,208,0,0,8,0,0,142,0,146,118,151,163,160,160,0,0,0,0,0,0,0,0,0,0,0,0,0,312,338,0,0,301,29,224,0,233,231,0,174,0,150,224,0,215,497,0,496,0,250,255,0,0,0,0,430,431,490,489,509,493,0,512,426,506,508,0,386,150,148,278,284,148,0,277,208,118,0,69,182,188,148,0,187,183,208,15,0,16,0,38,0,0,0,159,158,306,312,0,0,0,0,0,0,340,341,343,345,347,349,351,353,356,361,364,367,371,373,379,380,0,41,309,318,0,0,0,0,0,0,0,0,0,0,339,292,218,234,0,226,175,217,222,472,0,0,256,474,0,0,479,478,494,0,281,150,150,148,279,0,0,150,148,184,14,17,0,0,147,111,0,0,0,312,0,377,0,0,374,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,375,312,0,320,0,0,0,328,0,0,0,0,0,0,311,0,0,224,499,498,0,0,0,495,280,283,150,110,0,185,150,18,40,164,307,308,0,0,0,0,312,314,344,0,346,348,350,352,354,355,359,360,357,358,362,363,365,366,368,369,370,0,313,319,321,322,0,330,329,0,324,0,0,0,149,227,219,0,0,0,0,282,0,186,310,376,0,372,315,0,41,323,331,325,326,0,252,251,475,0,70,0,342,312,316,327,0,378,317,476,0,0,0};
# 1885
static short Cyc_yydefgoto[148]={1067,50,51,463,644,844,52,305,53,289,54,465,55,467,56,57,143,58,59,536,230,449,450,231,62,244,232,64,165,166,65,163,66,266,267,128,129,130,268,233,431,481,482,483,656,796,67,68,661,662,663,69,484,70,454,71,72,160,161,73,118,532,320,700,617,74,618,526,691,518,522,523,425,313,252,96,97,548,473,549,405,406,407,234,306,307,619,437,292,293,294,295,296,297,777,298,299,860,861,862,863,864,865,866,867,868,869,870,871,872,873,874,875,408,416,417,409,410,411,300,195,387,196,541,197,198,199,200,201,202,203,204,205,206,207,208,209,210,803,211,564,565,212,213,76,845,412,441};
# 1903
static short Cyc_yypact[1070]={2947,- -32768,- -32768,- -32768,- -32768,- 55,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3400,163,- 14,901,- -32768,161,36,- -32768,34,88,174,176,122,178,252,256,- -32768,- -32768,90,- -32768,- -32768,- -32768,204,700,233,228,239,239,- -32768,- -32768,273,2810,- -32768,292,342,- -32768,337,740,3400,3400,3400,- -32768,3400,- -32768,- -32768,684,- -32768,161,3319,301,128,353,1426,- -32768,- -32768,310,349,391,- -32768,161,401,6290,- -32768,1335,250,- -32768,- -32768,- -32768,- -32768,310,6290,- -32768,405,457,1335,454,419,458,- -32768,133,- -32768,- -32768,3548,3548,2810,2810,3548,6290,755,- -32768,- 15,461,579,485,- 15,3918,6290,- -32768,- -32768,3083,- -32768,2810,3083,2810,3083,- 8,- -32768,505,492,- -32768,3238,- -32768,- -32768,- -32768,- -32768,3918,- -32768,- -32768,310,93,1679,- -32768,3319,740,524,3548,3474,4641,- -32768,301,- -32768,525,- -32768,- -32768,- -32768,- -32768,533,- -32768,- -32768,- 24,1426,3548,- -32768,- -32768,523,548,550,161,6531,553,6290,6435,571,573,580,591,596,608,620,633,6579,6579,- -32768,- -32768,- -32768,2389,646,6627,6627,6627,- -32768,- -32768,- -32768,215,- -32768,- -32768,- 72,703,663,669,670,272,132,422,240,- 85,- -32768,1292,6627,232,- 20,- -32768,693,56,- -32768,1335,81,2636,286,963,710,- -32768,- -32768,713,6290,310,963,707,3696,3918,3992,3918,716,- -32768,66,66,- -32768,- -32768,60,331,310,- -32768,- -32768,- -32768,- -32768,33,720,- -32768,- -32768,859,378,- -32768,735,746,- -32768,752,- -32768,756,303,770,- -32768,579,4738,3319,524,767,3918,- -32768,1222,766,161,769,764,270,788,4049,790,786,774,800,4835,4049,206,798,795,- -32768,- -32768,801,1822,1822,740,1822,- -32768,- -32768,- -32768,809,- -32768,- -32768,- -32768,325,- -32768,524,808,- -32768,- -32768,796,80,831,- -32768,57,811,815,379,812,659,813,6290,3548,- -32768,824,- -32768,- -32768,80,161,- -32768,6290,826,2389,- -32768,3918,- -32768,- -32768,4156,- -32768,856,6290,6290,6290,6290,6290,6290,3918,1032,2389,- -32768,- -32768,1965,823,395,6290,- -32768,- -32768,- -32768,6290,- -32768,6627,6290,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6290,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6290,- -32768,- 15,- -32768,- -32768,4932,- 15,6290,- -32768,827,310,- -32768,830,832,- -32768,54,- 15,6290,1335,76,- -32768,- -32768,- -32768,835,836,822,- -32768,- -32768,- -32768,833,839,1335,- -32768,479,2636,840,3548,- -32768,842,846,- -32768,3992,3992,3992,- -32768,- -32768,1304,5029,202,- -32768,592,- -32768,- -32768,57,- -32768,- -32768,3548,- -32768,- -32768,849,- -32768,845,850,858,- -32768,1112,- -32768,628,225,- -32768,- -32768,- -32768,3918,- -32768,- -32768,- -32768,2673,- -32768,2810,- -32768,2810,- -32768,866,853,- -32768,- -32768,- -32768,- -32768,524,- -32768,- -32768,- -32768,1049,6290,862,860,- -32768,- 34,274,310,310,737,6290,6290,861,864,6290,867,959,2251,871,- -32768,- -32768,- -32768,513,955,- -32768,5126,6290,2108,2527,- -32768,- -32768,3238,- -32768,- -32768,- -32768,- -32768,3548,- -32768,- -32768,3918,873,3770,- -32768,- -32768,863,927,57,879,3844,815,- -32768,- -32768,- -32768,- -32768,659,882,99,765,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,883,892,909,887,- -32768,- -32768,648,4738,893,900,904,906,398,896,905,495,916,910,6483,- -32768,- -32768,917,911,- -32768,703,4,663,669,670,272,132,132,422,422,422,422,240,240,- 85,- 85,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,918,- -32768,71,3548,4544,3918,- -32768,- -32768,903,- -32768,- -32768,- -32768,- -32768,2773,- -32768,- -32768,1171,- -32768,- -32768,925,- -32768,184,- -32768,3918,- -32768,- -32768,- -32768,921,815,922,659,919,592,3548,3622,5223,- -32768,- -32768,66,310,6387,923,33,3157,926,310,3548,3474,5320,- -32768,628,- -32768,936,- -32768,2810,- -32768,- -32768,931,952,- -32768,1098,- -32768,- -32768,1222,935,6290,- -32768,815,- -32768,- -32768,- -32768,941,161,520,410,412,6290,753,423,4049,938,5417,5514,530,- -32768,- -32768,947,958,953,440,1822,- -32768,3319,- -32768,796,960,3548,- -32768,961,57,1006,- -32768,962,927,216,- -32768,- -32768,- -32768,- -32768,765,- -32768,964,261,966,- -32768,4253,- -32768,- -32768,6290,1060,6290,6435,- -32768,- -32768,- 15,- 15,964,969,4350,- -32768,- -32768,6290,6290,- -32768,- -32768,80,666,992,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,927,972,- -32768,- -32768,659,80,975,815,659,971,- -32768,815,983,979,- -32768,- -32768,980,- -32768,80,981,815,982,659,977,647,- -32768,6290,994,- -32768,6290,- -32768,3918,- -32768,989,124,737,4049,997,991,6723,1004,998,4049,6290,5611,568,5708,569,5805,753,- -32768,1017,1026,- -32768,- -32768,815,192,- -32768,- -32768,1020,- -32768,1035,1006,815,3918,- -32768,- -32768,341,- -32768,6290,- -32768,- -32768,4738,1024,1027,1030,- -32768,856,1029,1033,- -32768,- -32768,702,- -32768,- -32768,- -32768,- -32768,4544,- -32768,1006,927,- -32768,- -32768,927,1043,- -32768,659,3918,1047,- -32768,- -32768,- -32768,927,1044,- -32768,- -32768,659,- -32768,1036,585,442,- -32768,1045,1038,3918,- -32768,- -32768,1145,753,1057,6675,1052,2527,6627,1050,- -32768,- 36,- -32768,1090,1048,691,750,173,778,334,145,- -32768,- -32768,- -32768,- -32768,1091,6627,1965,- -32768,- -32768,477,4049,489,5902,4049,493,5999,6096,613,1066,- -32768,- -32768,- -32768,- -32768,6290,1067,- -32768,- -32768,960,- -32768,295,53,- -32768,- -32768,3918,1163,- -32768,- -32768,- -32768,4447,- -32768,1006,1006,927,- -32768,1072,1073,1006,927,- -32768,- -32768,- -32768,755,1077,- -32768,- -32768,503,4049,1076,753,2389,- -32768,3918,1071,- -32768,1536,6627,6290,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6627,6290,- -32768,753,1080,- -32768,4049,4049,519,- -32768,4049,4049,541,4049,544,6193,- -32768,1074,57,815,- -32768,- -32768,2527,1079,1081,- -32768,- -32768,- -32768,1006,- -32768,1093,- -32768,1006,- -32768,- -32768,- -32768,- -32768,- -32768,1082,1084,1087,6627,753,- -32768,703,118,663,669,669,272,132,132,422,422,422,422,240,240,- 85,- 85,- -32768,- -32768,- -32768,169,- -32768,- -32768,- -32768,- -32768,4049,- -32768,- -32768,4049,- -32768,4049,4049,559,- -32768,- -32768,- -32768,1096,758,1094,3918,- -32768,1095,- -32768,- -32768,964,261,- -32768,- -32768,6290,1536,- -32768,- -32768,- -32768,- -32768,4049,- -32768,- -32768,- -32768,1103,- -32768,354,- -32768,753,- -32768,- -32768,1109,- -32768,- -32768,- -32768,1223,1237,- -32768};
# 2013
static short Cyc_yypgoto[148]={- -32768,105,- -32768,- -32768,- -32768,316,- -32768,- 240,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 51,- -32768,- 128,18,- -32768,- -32768,0,610,- -32768,127,- 207,1128,13,- -32768,- -32768,- 135,- -32768,538,1210,- 724,- -32768,- -32768,- -32768,990,986,435,415,- -32768,- -32768,601,3,- 712,- -32768,- -32768,111,- -32768,- -32768,242,- 167,1184,- 353,333,- -32768,1097,- -32768,- -32768,1208,- 267,- -32768,558,- 111,- 159,- 142,- 420,287,565,575,- 418,- 473,- 100,- 417,- 140,- -32768,- 249,- 155,- 553,- 170,- -32768,865,- 175,69,- 107,- 105,- 360,- 108,- -32768,- -32768,- 46,- 244,- -32768,- 274,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,97,1039,- -32768,662,848,- -32768,332,701,- -32768,- 162,- 392,- 153,- 348,- 343,- 355,907,- 351,- 325,- 338,- 302,- 277,- 158,539,497,930,236,- -32768,- 357,- -32768,320,326,5,2,- 109,26};
# 2035
static short Cyc_yytable[6869]={60,321,245,264,570,312,245,314,567,331,572,348,598,63,472,569,251,334,142,335,356,60,627,145,429,476,575,576,577,578,351,352,353,327,63,590,311,573,574,79,654,726,641,849,113,508,509,688,511,372,447,388,60,77,426,323,937,373,374,60,60,60,514,60,357,63,579,580,139,60,149,138,63,63,63,622,63,132,133,134,142,135,63,453,395,265,897,144,651,39,290,242,301,581,582,622,655,302,259,243,938,685,640,560,319,60,60,692,695,916,641,241,82,911,138,396,260,519,63,63,60,98,60,60,60,60,354,61,436,659,660,60,640,63,722,63,63,63,63,448,735,60,102,60,63,456,438,678,61,144,149,521,438,543,63,544,63,121,103,291,216,144,216,550,540,818,438,557,495,559,39,528,235,236,502,354,239,520,217,61,217,215,443,977,- 524,439,61,61,61,539,61,439,222,354,399,111,61,440,737,600,981,982,515,440,724,439,986,403,404,400,237,238,104,301,583,584,585,162,475,364,365,429,429,429,271,254,698,255,256,257,258,428,61,61,38,649,440,767,290,290,354,290,540,41,430,- 157,107,61,1046,61,61,61,61,226,44,458,366,367,61,227,945,946,442,766,60,444,696,38,61,38,61,1038,19,20,458,1040,41,63,41,953,588,521,144,80,591,634,114,954,955,60,60,354,60,105,597,106,290,947,948,1047,131,706,63,63,108,63,110,291,291,774,291,734,634,398,389,390,391,415,540,894,537,440,517,830,415,503,75,833,433,440,112,38,504,516,434,310,839,354,115,40,41,42,355,- 222,99,426,- 222,60,81,538,100,452,101,739,116,247,216,434,392,117,63,362,363,393,394,291,521,242,802,- 244,75,893,- 244,848,217,243,38,370,75,898,38,371,682,899,75,41,303,535,40,41,42,61,137,120,140,126,490,75,681,491,479,242,975,719,99,151,167,251,354,243,100,122,123,413,194,99,61,61,43,61,727,100,468,219,550,146,469,147,428,428,428,75,75,148,162,137,789,626,426,75,240,430,430,430,513,672,75,354,75,75,75,75,633,354,639,680,807,635,445,142,458,124,125,60,683,60,752,60,900,- 12,75,951,901,827,61,952,63,831,63,650,63,1064,743,752,458,901,725,47,48,49,458,841,611,540,167,756,60,757,310,164,459,527,599,972,303,460,60,368,369,63,60,890,628,742,144,607,741,354,349,63,354,168,562,63,144,711,755,690,144,754,521,220,354,510,354,808,99,772,99,773,99,224,100,791,100,354,100,99,550,699,778,250,799,100,1033,692,812,420,813,903,820,681,354,221,354,915,435,788,643,924,645,269,646,223,807,920,457,714,390,391,929,225,250,455,684,1002,1003,246,75,999,61,1004,61,804,61,457,1001,487,608,354,492,354,485,540,959,814,815,1007,1008,1009,1010,354,248,501,262,354,961,75,1005,1006,965,392,626,61,261,459,715,394,218,290,990,301,675,61,958,354,790,61,635,770,639,354,771,304,60,324,1023,1011,1012,784,167,317,354,808,746,458,993,63,458,318,349,807,998,354,853,427,354,432,1026,325,880,1028,555,326,1013,1014,349,270,272,330,60,354,60,934,1019,38,1052,884,887,568,354,354,997,63,41,63,798,336,291,337,144,935,269,732,922,44,338,923,329,43,332,332,699,47,48,49,623,339,624,957,345,346,340,1045,625,99,592,332,332,332,978,100,970,540,341,354,530,531,99,825,808,99,659,660,100,43,342,100,332,761,927,821,636,38,637,792,994,974,995,343,638,40,41,42,421,138,435,828,843,704,705,61,350,1061,960,775,776,964,457,446,837,457,16,17,18,455,1065,822,823,75,38,75,976,75,358,1015,1016,1017,40,41,42,359,1034,1060,136,457,38,61,360,61,361,457,485,397,521,41,418,43,253,419,991,664,665,909,910,668,44,290,673,912,941,942,913,423,47,48,49,679,1044,943,944,918,433,614,615,616,38,451,434,316,47,48,49,1021,1022,41,461,1024,1025,127,1027,38,1056,949,950,332,44,462,250,40,41,42,876,464,47,48,49,466,60,1054,354,851,852,470,1059,290,486,250,477,488,489,63,497,498,250,332,291,332,332,332,332,332,332,332,332,332,332,332,332,332,332,332,332,332,493,1048,496,983,1049,499,1050,1051,987,99,505,506,99,507,138,100,512,263,100,440,804,594,60,517,524,529,83,43,250,332,1062,525,533,542,356,63,561,593,604,250,291,595,602,596,603,457,605,606,250,612,474,38,485,610,613,629,75,630,647,40,41,42,457,631,648,457,632,652,653,485,667,452,485,666,769,670,674,434,84,47,48,49,290,669,676,686,689,654,83,781,783,61,693,85,86,697,701,87,88,89,702,703,41,- 524,534,332,707,90,708,712,657,658,709,91,710,92,93,717,713,721,474,94,728,551,552,553,554,95,556,716,720,723,733,60,736,738,750,563,740,753,84,566,760,762,763,250,63,765,768,61,779,291,414,785,427,214,86,83,250,87,88,89,586,786,41,310,787,795,793,90,718,810,137,797,587,91,805,92,93,563,846,99,816,94,824,826,332,100,829,95,832,834,835,836,838,840,842,881,883,847,886,850,889,854,855,879,7,8,9,10,11,12,13,14,15,16,17,18,19,20,878,621,891,902,38,86,183,184,87,88,89,892,895,41,896,22,23,904,906,905,917,38,907,921,344,926,908,187,28,40,41,42,43,747,29,30,914,919,925,928,61,479,33,930,933,34,936,939,956,47,48,49,940,971,979,973,219,35,36,984,332,985,989,992,996,1020,1039,1031,269,1041,38,563,1036,1037,563,1043,83,1042,40,41,42,308,963,1053,38,967,969,1055,1057,1068,479,39,40,41,42,43,1063,310,47,48,49,250,1066,1069,452,988,45,46,751,249,434,109,47,48,49,137,332,474,471,478,764,150,119,801,322,800,1032,332,84,349,794,730,422,571,269,609,1000,601,877,558,731,0,0,214,86,1058,0,87,88,89,0,250,41,0,1018,0,0,90,0,0,0,474,0,91,0,92,93,0,1030,0,332,94,0,0,0,1035,0,95,7,8,9,10,11,12,13,14,15,16,17,18,19,20,745,0,38,0,0,749,0,0,0,0,40,41,42,759,0,22,23,0,0,0,0,375,479,0,0,0,480,0,28,0,47,48,49,0,29,30,0,0,0,0,0,563,33,0,0,34,0,0,83,0,0,0,0,0,0,0,0,35,36,376,377,378,379,380,381,382,383,384,385,932,0,0,332,0,0,0,0,0,0,0,474,308,0,809,0,811,0,0,0,386,332,39,40,474,42,43,819,310,0,0,84,0,0,0,433,0,45,46,332,0,434,0,47,48,49,214,86,0,0,87,88,89,0,0,41,0,0,0,0,90,0,0,0,0,0,91,0,92,93,0,0,0,0,94,0,0,0,0,0,95,332,0,332,332,332,332,332,332,332,332,332,332,332,332,332,332,332,332,332,152,153,154,155,156,0,0,157,158,159,0,0,0,0,474,0,0,0,0,0,0,0,0,0,0,0,0,0,0,474,0,0,0,0,0,0,0,0,0,0,332,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,775,776,21,169,170,273,0,274,275,276,277,278,279,280,281,22,23,83,24,171,282,0,0,172,25,283,0,332,28,173,174,175,176,0,29,30,0,177,0,284,0,0,33,178,179,34,180,0,0,0,0,0,0,474,0,0,0,35,36,37,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,285,86,183,184,87,88,89,39,40,41,42,43,185,286,141,- 312,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,169,170,273,0,274,275,276,277,278,279,280,281,22,23,83,24,171,282,0,0,172,25,283,0,0,28,173,174,175,176,0,29,30,0,177,0,284,0,0,33,178,179,34,180,0,0,0,0,0,0,0,0,0,0,35,36,37,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,285,86,183,184,87,88,89,39,40,41,42,43,185,286,141,287,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,169,170,273,0,274,275,276,277,278,279,280,281,22,23,83,24,171,282,0,0,172,25,283,0,0,28,173,174,175,176,0,29,30,- 41,177,0,284,0,0,33,178,179,34,180,0,0,0,0,0,0,0,0,0,0,35,36,37,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,285,86,183,184,87,88,89,39,40,41,42,43,185,286,141,0,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,169,170,273,0,274,275,276,277,278,279,280,281,22,23,83,24,171,282,0,0,172,25,283,0,0,28,173,174,175,176,0,29,30,0,177,0,284,0,0,33,178,179,34,180,0,0,0,0,0,0,0,0,0,0,35,36,37,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,285,86,183,184,87,88,89,39,40,41,42,43,185,286,141,0,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,169,170,273,0,274,275,276,277,278,279,280,281,22,23,83,24,171,282,0,0,172,25,283,0,0,28,173,174,175,176,0,29,30,0,177,0,284,0,0,33,178,179,34,180,0,0,0,0,0,0,0,0,0,0,35,36,37,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,494,86,183,184,87,88,89,39,40,41,42,43,185,286,141,0,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,169,170,0,0,0,0,0,0,0,0,0,0,22,23,83,24,171,0,0,0,172,25,0,0,0,28,173,174,175,176,0,29,30,0,177,0,0,0,0,33,178,179,34,180,0,0,0,0,0,0,0,0,0,0,35,36,37,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,39,40,41,42,43,185,671,0,0,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,22,23,83,0,171,0,0,0,172,0,0,0,0,28,173,174,175,176,0,29,30,0,177,0,0,0,0,33,178,179,34,180,0,0,0,0,0,0,0,0,0,0,35,36,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,39,40,41,42,43,185,0,347,0,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,22,23,83,0,171,0,0,0,172,0,0,0,0,28,173,174,175,176,0,29,30,0,177,0,0,0,0,33,178,179,34,180,0,0,0,0,0,0,0,0,0,0,35,36,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,39,40,41,42,43,185,0,0,0,0,0,0,0,186,0,45,288,0,0,0,0,188,0,0,189,0,0,190,191,0,0,192,193,- 11,1,83,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,0,24,0,0,0,642,0,25,0,26,27,28,0,0,0,84,0,29,30,- 41,0,0,0,31,32,33,0,401,34,0,402,86,0,0,87,88,89,0,243,41,35,36,37,0,90,0,0,0,0,0,91,0,92,93,0,403,404,0,94,0,0,0,0,0,95,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,83,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,0,24,0,0,0,0,0,25,0,26,27,28,0,0,0,84,0,29,30,- 41,0,0,0,31,32,33,0,729,34,0,402,86,0,0,87,88,89,0,243,41,35,36,37,0,90,0,0,0,0,0,91,0,92,93,0,403,404,0,94,0,0,0,0,0,95,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,0,24,0,0,0,0,0,25,0,26,27,28,0,0,0,0,0,29,30,- 41,0,0,0,31,32,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,0,24,0,0,0,0,0,25,0,26,27,28,0,0,0,0,0,29,30,- 41,0,0,0,31,32,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,23,39,40,41,42,43,0,0,0,- 11,0,0,28,0,44,0,45,46,29,30,0,0,47,48,49,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,38,0,0,0,0,0,0,39,40,41,42,43,22,23,0,24,0,0,0,0,479,25,45,46,0,28,0,0,47,48,49,29,30,- 41,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,37,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,23,141,24,0,263,0,0,0,25,45,46,0,28,0,0,0,0,0,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,37,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,23,141,0,0,0,0,0,0,25,45,46,0,28,0,0,0,0,0,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,39,40,0,42,43,0,0,0,0,0,0,28,0,0,0,45,46,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,308,0,0,309,0,0,0,0,22,23,39,40,0,42,43,0,310,0,0,0,0,28,0,0,0,45,46,29,30,0,0,228,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,39,40,0,42,43,0,0,229,0,0,0,28,0,0,0,45,46,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,308,0,0,0,0,0,0,0,22,23,39,40,0,42,43,0,310,0,0,0,0,28,0,0,0,45,46,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,39,40,0,42,43,0,0,0,424,0,0,28,0,0,0,45,46,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,39,40,0,42,43,0,0,0,687,0,0,28,0,0,0,45,46,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,694,0,0,0,0,0,0,0,22,23,39,40,0,42,43,0,0,0,0,0,0,28,0,0,0,45,46,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,39,40,0,42,43,0,0,0,0,0,0,28,0,0,0,45,46,29,30,0,0,0,0,0,0,33,0,0,34,0,0,0,0,0,0,0,0,0,0,0,35,36,0,169,170,273,0,274,275,276,277,278,279,280,281,0,0,83,0,171,282,0,0,172,0,283,0,0,0,173,174,175,176,0,39,0,0,177,43,284,0,0,0,178,179,0,180,0,0,45,46,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,494,86,183,184,87,88,89,0,0,41,0,0,185,286,141,0,0,0,0,0,186,0,0,187,0,0,169,170,188,0,0,189,0,545,190,191,0,0,192,193,83,0,171,0,0,0,172,0,0,0,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,546,86,183,184,87,88,89,0,243,41,0,0,185,0,333,547,169,170,0,0,186,0,0,187,0,403,404,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,546,86,183,184,87,88,89,0,243,41,0,0,185,0,333,806,169,170,0,0,186,0,0,187,0,403,404,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,546,86,183,184,87,88,89,0,243,41,0,0,185,0,333,817,169,170,0,0,186,0,0,187,0,403,404,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,546,86,183,184,87,88,89,0,243,41,0,0,185,0,333,980,169,170,0,0,186,0,0,187,0,403,404,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,546,86,183,184,87,88,89,0,243,41,0,0,185,0,333,0,169,170,0,0,186,0,0,187,0,403,404,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,0,0,187,0,0,0,315,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,333,0,169,170,0,0,186,0,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,500,0,0,169,170,0,0,186,0,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,589,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,0,0,187,0,0,0,620,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,677,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,0,0,187,0,0,0,744,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,0,0,187,0,0,0,758,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,780,0,0,169,170,0,0,186,0,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,782,0,0,169,170,0,0,186,0,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,882,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,885,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,888,0,0,169,170,0,0,186,0,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,962,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,966,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,968,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,1029,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,181,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,169,170,0,0,186,0,0,187,0,0,0,0,188,0,83,189,171,0,190,191,172,0,192,193,0,0,173,174,175,176,0,0,0,0,177,0,0,0,0,0,178,179,0,180,0,0,0,0,169,170,0,0,0,0,0,0,0,0,181,182,0,0,83,0,0,0,0,0,0,0,0,0,0,0,173,174,175,176,0,0,0,748,86,183,184,87,88,89,178,179,41,180,0,185,0,0,169,170,0,0,0,186,0,0,187,0,181,182,0,188,83,0,189,0,0,190,191,0,0,192,193,0,173,174,175,176,0,0,0,38,86,183,184,87,88,89,178,179,41,180,0,185,0,333,169,170,0,0,0,186,0,0,187,0,181,182,0,188,83,0,189,0,0,190,191,0,0,192,193,0,173,174,175,176,0,0,0,38,86,183,184,87,88,89,178,179,41,180,0,185,0,718,169,170,0,0,0,186,0,0,187,0,181,182,0,188,83,0,189,0,0,190,191,0,0,192,193,0,173,174,175,176,0,0,0,38,86,183,184,87,88,89,178,179,41,180,0,185,0,0,169,170,0,0,0,328,0,0,187,0,181,182,0,188,83,0,189,0,0,190,191,0,0,192,193,0,173,174,175,176,0,0,0,38,86,183,184,87,88,89,178,179,41,180,0,185,0,0,169,170,0,0,0,344,0,0,187,0,181,182,0,188,83,0,189,0,0,190,191,0,0,192,193,0,173,174,175,176,0,0,0,38,86,183,184,87,88,89,178,179,41,180,0,185,0,0,856,857,0,0,0,186,0,0,187,0,181,182,0,188,83,0,189,0,0,190,191,0,0,192,193,0,0,0,0,0,0,0,0,38,86,183,184,87,88,89,0,0,41,0,0,185,0,0,0,0,0,0,0,931,0,0,187,0,0,0,0,188,0,0,189,0,84,190,191,0,0,192,193,0,0,0,0,0,0,0,0,214,86,0,0,87,88,89,0,0,41,0,0,0,0,90,0,0,0,0,0,858,0,92,93,0,0,0,0,94,0,0,859,0,0,95,191,0,0,192,193};
# 2725
static short Cyc_yycheck[6869]={0,160,111,131,359,147,115,147,356,171,361,186,404,0,263,358,116,172,69,172,92,21,440,69,231,265,364,365,366,367,188,189,190,168,21,392,147,362,363,21,74,594,459,767,44,289,290,520,292,134,17,209,52,108,229,162,92,142,143,59,60,61,302,63,136,52,368,369,66,69,70,66,59,60,61,435,63,59,60,61,131,63,69,250,104,131,798,69,480,113,141,106,143,370,371,455,130,143,106,114,136,518,455,347,128,105,106,525,528,833,527,109,126,825,109,135,124,60,105,106,120,24,122,123,124,125,122,0,233,5,6,131,485,120,130,122,123,124,125,106,613,141,106,143,131,250,86,504,21,131,150,310,86,328,141,330,143,52,124,141,106,143,106,333,326,718,86,342,276,344,113,313,103,104,282,122,107,120,124,52,124,84,122,130,130,125,59,60,61,324,63,125,95,122,113,105,69,137,618,123,912,913,122,137,133,125,918,131,132,128,105,106,124,264,372,373,374,124,264,87,88,428,429,430,131,120,127,122,123,124,125,231,105,106,106,475,137,657,289,290,122,292,404,115,231,121,124,120,130,122,123,124,125,120,126,250,124,125,131,126,87,88,236,655,264,239,533,106,141,106,143,983,20,21,269,987,115,264,115,134,389,440,264,120,393,452,44,142,143,289,290,122,292,119,403,119,347,124,125,130,58,550,289,290,126,292,50,289,290,666,292,127,479,216,82,83,84,220,480,127,320,137,106,743,227,119,0,747,126,137,126,106,126,307,132,119,756,122,105,114,115,116,127,127,24,520,130,347,22,323,24,126,26,620,126,113,106,132,126,120,347,89,90,131,132,347,525,106,107,119,44,791,122,765,124,114,106,137,52,799,106,141,510,800,58,115,144,318,114,115,116,264,66,120,68,58,126,71,506,129,126,106,107,561,84,72,80,507,122,114,84,119,120,127,82,95,289,290,117,292,595,95,119,91,594,124,123,126,428,429,430,105,106,132,124,109,680,437,613,113,108,428,429,430,119,496,120,122,122,123,124,125,452,122,454,506,705,452,127,510,455,119,120,463,510,465,633,467,127,120,144,137,131,740,347,141,463,744,465,479,467,127,624,650,479,131,593,134,135,136,485,758,423,655,168,637,496,637,119,108,122,122,405,895,262,127,506,85,86,496,510,785,443,624,496,418,623,122,186,506,122,120,127,510,506,127,637,524,510,636,689,126,122,291,122,705,216,127,218,127,220,122,216,685,218,122,220,227,718,536,127,116,694,227,974,973,711,225,711,808,722,669,122,106,122,832,233,127,463,127,465,136,467,119,823,842,250,82,83,84,854,123,147,250,515,940,941,126,262,937,463,942,465,702,467,269,939,271,119,122,274,122,269,765,127,714,715,945,946,947,948,122,127,281,122,122,127,291,943,944,127,126,622,496,119,122,131,132,90,680,127,682,119,506,878,122,682,510,633,119,640,122,122,119,644,122,127,949,950,119,324,126,122,823,628,650,930,644,653,126,328,910,936,122,772,230,122,232,127,121,778,127,340,123,951,952,344,139,140,126,680,122,682,858,958,106,127,119,119,357,122,122,936,680,115,682,693,126,680,126,682,859,267,606,119,126,126,122,169,117,171,172,699,134,135,136,124,126,126,877,181,182,126,997,132,405,394,188,189,190,905,405,119,895,126,122,77,78,418,736,910,421,5,6,418,117,126,421,209,644,850,725,124,106,126,686,931,899,933,126,132,114,115,116,226,760,433,741,121,121,122,644,126,1047,882,22,23,885,452,241,754,455,17,18,19,452,1060,121,122,463,106,465,901,467,91,953,954,955,114,115,116,138,977,1047,120,479,106,680,139,682,140,485,479,120,973,115,106,117,117,106,928,489,490,121,122,493,126,878,496,826,139,140,829,126,134,135,136,505,996,89,90,838,126,428,429,430,106,127,132,148,134,135,136,961,962,115,121,965,966,119,968,106,1037,85,86,326,126,121,433,114,115,116,775,121,134,135,136,121,878,121,122,770,771,113,1046,936,120,452,121,120,126,878,106,119,459,356,878,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,126,1023,126,914,1026,119,1028,1029,919,603,126,130,606,126,923,603,121,123,606,137,1043,397,936,106,127,127,39,117,507,404,1052,130,133,121,92,936,127,124,130,518,936,125,121,125,122,633,127,122,527,121,263,106,633,127,122,120,644,126,106,114,115,116,650,127,125,653,122,119,122,650,120,126,653,126,662,30,119,132,91,134,135,136,1047,130,43,126,137,74,39,671,672,878,127,106,107,127,127,110,111,112,122,106,115,130,317,480,127,120,122,127,486,487,122,126,122,128,129,121,127,122,333,134,133,336,337,338,339,140,341,127,127,127,121,1047,127,127,127,350,133,127,91,354,120,126,106,624,1047,126,121,936,126,1047,103,120,633,106,107,39,637,110,111,112,375,119,115,119,127,75,121,120,120,25,760,125,387,126,124,128,129,392,762,775,127,134,106,127,561,775,127,140,133,122,127,127,127,127,133,779,780,119,782,126,784,120,127,121,8,9,10,11,12,13,14,15,16,17,18,19,20,21,130,434,119,805,106,107,108,109,110,111,112,119,126,115,113,37,38,127,122,126,107,106,127,121,126,121,127,129,50,114,115,116,117,629,56,57,127,127,127,28,1047,126,64,120,126,67,130,91,91,134,135,136,138,121,25,122,858,79,80,121,655,122,119,121,127,119,107,127,767,121,106,504,127,126,507,122,39,127,114,115,116,103,884,121,106,887,888,127,127,0,126,113,114,115,116,117,127,119,134,135,136,800,127,0,126,923,128,129,632,115,132,35,134,135,136,923,711,550,262,267,653,71,48,699,161,694,973,722,91,931,689,603,227,360,833,421,938,406,775,343,103,- 1,- 1,106,107,1043,- 1,110,111,112,- 1,850,115,- 1,956,- 1,- 1,120,- 1,- 1,- 1,594,- 1,126,- 1,128,129,- 1,970,- 1,765,134,- 1,- 1,- 1,977,- 1,140,8,9,10,11,12,13,14,15,16,17,18,19,20,21,625,- 1,106,- 1,- 1,630,- 1,- 1,- 1,- 1,114,115,116,638,- 1,37,38,- 1,- 1,- 1,- 1,55,126,- 1,- 1,- 1,130,- 1,50,- 1,134,135,136,- 1,56,57,- 1,- 1,- 1,- 1,- 1,666,64,- 1,- 1,67,- 1,- 1,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,93,94,95,96,97,98,99,100,101,102,856,- 1,- 1,859,- 1,- 1,- 1,- 1,- 1,- 1,- 1,705,103,- 1,708,- 1,710,- 1,- 1,- 1,123,877,113,114,718,116,117,721,119,- 1,- 1,91,- 1,- 1,- 1,126,- 1,128,129,895,- 1,132,- 1,134,135,136,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,937,- 1,939,940,941,942,943,944,945,946,947,948,949,950,951,952,953,954,955,69,70,71,72,73,- 1,- 1,76,77,78,- 1,- 1,- 1,- 1,808,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,823,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,996,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,- 1,- 1,45,46,47,- 1,1046,50,51,52,53,54,- 1,56,57,- 1,59,- 1,61,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,910,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,- 1,- 1,45,46,47,- 1,- 1,50,51,52,53,54,- 1,56,57,- 1,59,- 1,61,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,- 1,- 1,45,46,47,- 1,- 1,50,51,52,53,54,- 1,56,57,58,59,- 1,61,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,- 1,- 1,45,46,47,- 1,- 1,50,51,52,53,54,- 1,56,57,- 1,59,- 1,61,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,- 1,- 1,45,46,47,- 1,- 1,50,51,52,53,54,- 1,56,57,- 1,59,- 1,61,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,45,46,- 1,- 1,- 1,50,51,52,53,54,- 1,56,57,- 1,59,- 1,- 1,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,- 1,56,57,- 1,59,- 1,- 1,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,45,- 1,- 1,- 1,- 1,50,51,52,53,54,- 1,56,57,- 1,59,- 1,- 1,- 1,- 1,64,65,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,39,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,46,- 1,48,49,50,- 1,- 1,- 1,91,- 1,56,57,58,- 1,- 1,- 1,62,63,64,- 1,103,67,- 1,106,107,- 1,- 1,110,111,112,- 1,114,115,79,80,81,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,39,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,- 1,- 1,46,- 1,48,49,50,- 1,- 1,- 1,91,- 1,56,57,58,- 1,- 1,- 1,62,63,64,- 1,103,67,- 1,106,107,- 1,- 1,110,111,112,- 1,114,115,79,80,81,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,- 1,- 1,46,- 1,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,62,63,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,- 1,- 1,46,- 1,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,62,63,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,38,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,50,- 1,126,- 1,128,129,56,57,- 1,- 1,134,135,136,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,37,38,- 1,40,- 1,- 1,- 1,- 1,126,46,128,129,- 1,50,- 1,- 1,134,135,136,56,57,58,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,38,120,40,- 1,123,- 1,- 1,- 1,46,128,129,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,38,120,- 1,- 1,- 1,- 1,- 1,- 1,46,128,129,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,119,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,60,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,- 1,120,- 1,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,119,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,- 1,- 1,121,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,- 1,- 1,121,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,113,114,- 1,116,117,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,128,129,56,57,- 1,- 1,- 1,- 1,- 1,- 1,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,39,- 1,41,42,- 1,- 1,45,- 1,47,- 1,- 1,- 1,51,52,53,54,- 1,113,- 1,- 1,59,117,61,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,128,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,25,26,134,- 1,- 1,137,- 1,32,140,141,- 1,- 1,144,145,39,- 1,41,- 1,- 1,- 1,45,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,25,26,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,25,26,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,25,26,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,121,25,26,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,114,115,- 1,- 1,118,- 1,120,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,25,26,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,39,137,41,- 1,140,141,45,- 1,144,145,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,- 1,- 1,- 1,65,66,- 1,68,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,106,107,108,109,110,111,112,65,66,115,68,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,83,84,- 1,134,39,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,51,52,53,54,- 1,- 1,- 1,106,107,108,109,110,111,112,65,66,115,68,- 1,118,- 1,120,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,83,84,- 1,134,39,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,51,52,53,54,- 1,- 1,- 1,106,107,108,109,110,111,112,65,66,115,68,- 1,118,- 1,120,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,83,84,- 1,134,39,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,51,52,53,54,- 1,- 1,- 1,106,107,108,109,110,111,112,65,66,115,68,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,83,84,- 1,134,39,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,51,52,53,54,- 1,- 1,- 1,106,107,108,109,110,111,112,65,66,115,68,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,83,84,- 1,134,39,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,51,52,53,54,- 1,- 1,- 1,106,107,108,109,110,111,112,65,66,115,68,- 1,118,- 1,- 1,25,26,- 1,- 1,- 1,126,- 1,- 1,129,- 1,83,84,- 1,134,39,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,91,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp51B[8]="stdcall";static char _tmp51C[6]="cdecl";static char _tmp51D[9]="fastcall";static char _tmp51E[9]="noreturn";static char _tmp51F[6]="const";static char _tmp520[8]="aligned";static char _tmp521[7]="packed";static char _tmp522[7]="shared";static char _tmp523[7]="unused";static char _tmp524[5]="weak";static char _tmp525[10]="dllimport";static char _tmp526[10]="dllexport";static char _tmp527[23]="no_instrument_function";static char _tmp528[12]="constructor";static char _tmp529[11]="destructor";static char _tmp52A[22]="no_check_memory_usage";static char _tmp52B[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA1A(union Cyc_YYSTYPE*yylval,unsigned int*_tmpA19,unsigned int*_tmpA18,union Cyc_YYSTYPE**_tmpA17){for(*_tmpA19=0;*_tmpA19 < *_tmpA18;(*_tmpA19)++){(*_tmpA17)[*_tmpA19]=(union Cyc_YYSTYPE)*yylval;}}static void _tmpA1F(unsigned int*_tmpA1E,unsigned int*_tmpA1D,struct Cyc_Yyltype**_tmpA1C){for(*_tmpA1E=0;*_tmpA1E < *_tmpA1D;(*_tmpA1E)++){(*_tmpA1C)[*_tmpA1E]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmpA26(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmpA25,unsigned int*_tmpA24,short**_tmpA22){for(*_tmpA25=0;*_tmpA25 < *_tmpA24;(*_tmpA25)++){(*_tmpA22)[*_tmpA25]=(short)(
# 216
*_tmpA25 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmpA25)): 0);}}static void _tmpA2C(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmpA2B,unsigned int*_tmpA2A,union Cyc_YYSTYPE**_tmpA28){for(*_tmpA2B=0;*_tmpA2B < *_tmpA2A;(*_tmpA2B)++){(*_tmpA28)[*_tmpA2B]=(union Cyc_YYSTYPE)(
# 219
*_tmpA2B <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmpA2B)):*yylval);}}static void _tmpA32(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmpA31,unsigned int*_tmpA30,struct Cyc_Yyltype**_tmpA2E){for(*_tmpA31=0;*_tmpA31 < *_tmpA30;(*_tmpA31)++){(*_tmpA2E)[*_tmpA31]=(struct Cyc_Yyltype)(
# 222
*_tmpA31 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmpA31)):
 Cyc_yynewloc());}}static void _tmpF47(unsigned int*_tmpF46,unsigned int*_tmpF45,char**_tmpF43){for(*_tmpF46=0;*_tmpF46 < *_tmpF45;(*_tmpF46)++){(*_tmpF43)[*_tmpF46]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp45E=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp45E;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmpA13;union Cyc_YYSTYPE yylval=((_tmpA13.YYINITIALSVAL).val=0,(((_tmpA13.YYINITIALSVAL).tag=64,_tmpA13)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmpA15;unsigned int _tmpA14;struct _dyneither_ptr yyss=(_tmpA14=200,((_tmpA15=_region_calloc(yyregion,sizeof(short),_tmpA14),_tag_dyneither(_tmpA15,sizeof(short),_tmpA14))));
# 158
int yyvsp_offset;
unsigned int _tmpA19;unsigned int _tmpA18;union Cyc_YYSTYPE*_tmpA17;unsigned int _tmpA16;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmpA16=(unsigned int)200,((_tmpA17=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA16)),((((_tmpA18=_tmpA16,_tmpA1A(& yylval,& _tmpA19,& _tmpA18,& _tmpA17))),_tmpA17)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmpA1E;unsigned int _tmpA1D;struct Cyc_Yyltype*_tmpA1C;unsigned int _tmpA1B;struct _dyneither_ptr yyls=_tag_dyneither(((_tmpA1B=(unsigned int)200,((_tmpA1C=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA1B)),((((_tmpA1D=_tmpA1B,_tmpA1F(& _tmpA1E,& _tmpA1D,& _tmpA1C))),_tmpA1C)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 193
yyssp_offset=-1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
{const char*_tmpA20;Cyc_yyerror(((_tmpA20="parser stack overflow",_tag_dyneither(_tmpA20,sizeof(char),22))),yystate,yychar);}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmpA25;unsigned int _tmpA24;struct _dyneither_ptr _tmpA23;short*_tmpA22;unsigned int _tmpA21;struct _dyneither_ptr yyss1=(_tmpA21=(unsigned int)yystacksize,((_tmpA22=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmpA21)),((_tmpA23=_tag_dyneither(_tmpA22,sizeof(short),_tmpA21),((((_tmpA24=_tmpA21,_tmpA26(& yyssp_offset,& yyss,& _tmpA25,& _tmpA24,& _tmpA22))),_tmpA23)))))));
# 217
unsigned int _tmpA2B;unsigned int _tmpA2A;struct _dyneither_ptr _tmpA29;union Cyc_YYSTYPE*_tmpA28;unsigned int _tmpA27;struct _dyneither_ptr yyvs1=
(_tmpA27=(unsigned int)yystacksize,((_tmpA28=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA27)),((_tmpA29=_tag_dyneither(_tmpA28,sizeof(union Cyc_YYSTYPE),_tmpA27),((((_tmpA2A=_tmpA27,_tmpA2C(& yyvs,& yyssp_offset,& yylval,& _tmpA2B,& _tmpA2A,& _tmpA28))),_tmpA29)))))));
# 221
unsigned int _tmpA31;unsigned int _tmpA30;struct _dyneither_ptr _tmpA2F;struct Cyc_Yyltype*_tmpA2E;unsigned int _tmpA2D;struct _dyneither_ptr yyls1=(_tmpA2D=(unsigned int)yystacksize,((_tmpA2E=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA2D)),((_tmpA2F=_tag_dyneither(_tmpA2E,sizeof(struct Cyc_Yyltype),_tmpA2D),((((_tmpA30=_tmpA2D,_tmpA32(& yyssp_offset,& yyls,& _tmpA31,& _tmpA30,& _tmpA2E))),_tmpA2F)))))));
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1070,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 294;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6868) || Cyc_yycheck[_check_known_subscript_notnull(6869,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6869,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1069){
int _tmp46C=0;_npop_handler(0);return _tmp46C;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1070,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(528,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL222: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1219 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1222
break;}case 2: _LL223: {
# 1224
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1225 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp46D;_push_handler(& _tmp46D);{int _tmp46F=0;if(setjmp(_tmp46D.handler))_tmp46F=1;if(!_tmp46F){
x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp46E=(void*)_exn_thrown;void*_tmp471=_tmp46E;void*_tmp473;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp472=(struct Cyc_Core_Failure_exn_struct*)_tmp471;if(_tmp472->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
# 1230
 x=0;goto _LL225;_LL228: _tmp473=_tmp471;_LL229:(void)_rethrow(_tmp473);_LL225:;}};}
# 1232
{struct _handler_cons _tmp474;_push_handler(& _tmp474);{int _tmp476=0;if(setjmp(_tmp474.handler))_tmp476=1;if(!_tmp476){
y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp475=(void*)_exn_thrown;void*_tmp478=_tmp475;void*_tmp47A;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp479=(struct Cyc_Core_Failure_exn_struct*)_tmp478;if(_tmp479->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
# 1236
 y=0;goto _LL22A;_LL22D: _tmp47A=_tmp478;_LL22E:(void)_rethrow(_tmp47A);_LL22A:;}};}
# 1238
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1240
break;}case 3: _LL224: {
# 1242
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1244
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1243 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA3C;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA3B;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA3A;struct Cyc_List_List*_tmpA39;yyval=Cyc_YY16(((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39->hd=((_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C->r=(void*)((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=10,((_tmpA3B.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA3B.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA3B)))))),_tmpA3A)))),((_tmpA3C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA3C)))))),((_tmpA39->tl=0,_tmpA39)))))));}
Cyc_Lex_leave_using();
# 1246
break;}case 4: _LL22F: {
# 1248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1247 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA46;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA45;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA44;struct Cyc_List_List*_tmpA43;yyval=Cyc_YY16(((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->hd=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->r=(void*)((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=((_tmpA45.tag=10,((_tmpA45.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA45.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA45)))))),_tmpA44)))),((_tmpA46->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA46)))))),((_tmpA43->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA43)))))));}
break;}case 5: _LL230: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA55;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA54;struct _dyneither_ptr*_tmpA53;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA52;struct Cyc_List_List*_tmpA51;yyval=Cyc_YY16(((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->hd=((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55->r=(void*)((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA54.tag=9,((_tmpA54.f1=((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA53)))),((_tmpA54.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA54)))))),_tmpA52)))),((_tmpA55->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA55)))))),((_tmpA51->tl=0,_tmpA51)))))));}
Cyc_Lex_leave_namespace();
# 1253
break;}case 6: _LL231: {
# 1255
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1255 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp488;_push_handler(& _tmp488);{int _tmp48A=0;if(setjmp(_tmp488.handler))_tmp48A=1;if(!_tmp48A){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp489=(void*)_exn_thrown;void*_tmp48C=_tmp489;void*_tmp48E;_LL234: {struct Cyc_Core_Failure_exn_struct*_tmp48D=(struct Cyc_Core_Failure_exn_struct*)_tmp48C;if(_tmp48D->tag != Cyc_Core_Failure)goto _LL236;}_LL235:
{const char*_tmpA56;nspace=((_tmpA56="",_tag_dyneither(_tmpA56,sizeof(char),1)));}goto _LL233;_LL236: _tmp48E=_tmp48C;_LL237:(void)_rethrow(_tmp48E);_LL233:;}};}
# 1260
{struct _handler_cons _tmp490;_push_handler(& _tmp490);{int _tmp492=0;if(setjmp(_tmp490.handler))_tmp492=1;if(!_tmp492){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp491=(void*)_exn_thrown;void*_tmp494=_tmp491;void*_tmp496;_LL239: {struct Cyc_Core_Failure_exn_struct*_tmp495=(struct Cyc_Core_Failure_exn_struct*)_tmp494;if(_tmp495->tag != Cyc_Core_Failure)goto _LL23B;}_LL23A:
 x=0;goto _LL238;_LL23B: _tmp496=_tmp494;_LL23C:(void)_rethrow(_tmp496);_LL238:;}};}
# 1263
{struct _handler_cons _tmp497;_push_handler(& _tmp497);{int _tmp499=0;if(setjmp(_tmp497.handler))_tmp499=1;if(!_tmp499){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp498=(void*)_exn_thrown;void*_tmp49B=_tmp498;void*_tmp49D;_LL23E: {struct Cyc_Core_Failure_exn_struct*_tmp49C=(struct Cyc_Core_Failure_exn_struct*)_tmp49B;if(_tmp49C->tag != Cyc_Core_Failure)goto _LL240;}_LL23F:
 y=0;goto _LL23D;_LL240: _tmp49D=_tmp49B;_LL241:(void)_rethrow(_tmp49D);_LL23D:;}};}
# 1266
{struct Cyc_Absyn_Decl*_tmpA65;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA64;struct _dyneither_ptr*_tmpA63;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA62;struct Cyc_List_List*_tmpA61;yyval=Cyc_YY16(((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->hd=((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->r=(void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA64.tag=9,((_tmpA64.f1=((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63[0]=nspace,_tmpA63)))),((_tmpA64.f2=x,_tmpA64)))))),_tmpA62)))),((_tmpA65->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA65)))))),((_tmpA61->tl=y,_tmpA61)))))));}
# 1268
break;}case 7: _LL232: {
# 1270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1269 "parse.y"
int _tmp4A3=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp4A3){
struct Cyc_Absyn_Decl*_tmpA6F;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA6E;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA6D;struct Cyc_List_List*_tmpA6C;yyval=Cyc_YY16(((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->hd=((_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->r=(void*)((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6E.tag=11,((_tmpA6E.f1=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA6E)))),_tmpA6D)))),((_tmpA6F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA6F)))))),((_tmpA6C->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA6C)))))));}else{
# 1273
struct Cyc_Absyn_Decl*_tmpA79;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA78;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA77;struct Cyc_List_List*_tmpA76;yyval=Cyc_YY16(((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->r=(void*)((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=12,((_tmpA78.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA78.f2=0,_tmpA78)))))),_tmpA77)))),((_tmpA79->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA79)))))),((_tmpA76->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA76)))))));}
# 1275
break;}case 8: _LL242: {
# 1277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1276 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0])){
const char*_tmpA7A;Cyc_Parse_err(((_tmpA7A="expecting \"C include\"",_tag_dyneither(_tmpA7A,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
{struct Cyc_Absyn_Decl*_tmpA84;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA83;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA82;struct Cyc_List_List*_tmpA81;yyval=Cyc_YY16(((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81->hd=((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->r=(void*)((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=12,((_tmpA83.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA83.f2=exs,_tmpA83)))))),_tmpA82)))),((_tmpA84->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA84)))))),((_tmpA81->tl=Cyc_yyget_YY16(yyyyvsp[5]),_tmpA81)))))));}
# 1281
break;};}case 9: _LL243: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1282 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA87;struct Cyc_List_List*_tmpA86;yyval=Cyc_YY16(((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->hd=((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA87->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA87)))))),((_tmpA86->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA86)))))));}
break;}case 10: _LL244: {
# 1285
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1287
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1284 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA8A;struct Cyc_List_List*_tmpA89;yyval=Cyc_YY16(((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89->hd=((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA8A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA8A)))))),((_tmpA89->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA89)))))));}
break;}case 11: _LL245:
# 1287
 yyval=Cyc_YY16(0);
break;case 12: _LL246: {
# 1290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1290 "parse.y"
struct _dyneither_ptr _tmp4B5=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
{const char*_tmpA8B;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B5,((_tmpA8B="C",_tag_dyneither(_tmpA8B,sizeof(char),2))))== 0)
yyval=Cyc_YY31(0);else{
const char*_tmpA8C;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B5,((_tmpA8C="C include",_tag_dyneither(_tmpA8C,sizeof(char),10))))== 0)
yyval=Cyc_YY31(1);else{
# 1297
{const char*_tmpA8D;Cyc_Parse_err(((_tmpA8D="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA8D,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY31(1);}}}
# 1301
break;}case 13: _LL247: {
# 1303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1304 "parse.y"
Cyc_Lex_leave_extern_c();
break;}case 14: _LL248: {
# 1307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1308 "parse.y"
yyval=yyyyvsp[2];
break;}case 15: _LL249: {
# 1311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1309 "parse.y"
yyval=Cyc_YY52(0);
break;}case 16: _LL24A: {
# 1312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1313 "parse.y"
{struct _tuple27*_tmpA90;struct Cyc_List_List*_tmpA8F;yyval=Cyc_YY52(((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA90->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA90->f3=0,_tmpA90)))))))),((_tmpA8F->tl=0,_tmpA8F)))))));}
break;}case 17: _LL24B: {
# 1316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1314 "parse.y"
{struct _tuple27*_tmpA93;struct Cyc_List_List*_tmpA92;yyval=Cyc_YY52(((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->hd=((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA93->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA93->f3=0,_tmpA93)))))))),((_tmpA92->tl=0,_tmpA92)))))));}
break;}case 18: _LL24C: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1316 "parse.y"
{struct _tuple27*_tmpA96;struct Cyc_List_List*_tmpA95;yyval=Cyc_YY52(((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->hd=((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA96->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA96->f3=0,_tmpA96)))))))),((_tmpA95->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA95)))))));}
break;}case 19: _LL24D: {
# 1319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1320 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA9C;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA9B;struct Cyc_List_List*_tmpA9A;yyval=Cyc_YY16(((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->hd=Cyc_Absyn_new_decl((void*)((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9B.tag=1,((_tmpA9B.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpA9B)))),_tmpA9C)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA9A->tl=0,_tmpA9A)))))));}
break;}case 20: _LL24E: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1321 "parse.y"
yyval=yyyyvsp[0];
break;}case 21: _LL24F: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1322 "parse.y"
yyval=Cyc_YY16(0);
break;}case 24: _LL250: {
# 1325
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1331 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),0,Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL251: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1333 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C2=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C2,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL252: {
# 1337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1346 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1]),Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL253: {
# 1349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1348 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C3=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C3,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 28: _LL254: {
# 1352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1356 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C4=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C4,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 29: _LL255: {
# 1360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1359 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C5=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C5,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 30: _LL256: {
# 1363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1364 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 31: _LL257: {
# 1367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1367 "parse.y"
Cyc_Lex_leave_using();
break;}case 32: _LL258: {
# 1370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1370 "parse.y"
{struct _dyneither_ptr*_tmpA9D;Cyc_Lex_enter_namespace(((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA9D)))));}yyval=yyyyvsp[1];
break;}case 33: _LL259: {
# 1373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1373 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 34: _LL25A: {
# 1376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp4C7=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),0,(unsigned int)_tmp4C7,(unsigned int)_tmp4C7));
break;}case 35: _LL25B: {
# 1383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1382 "parse.y"
int _tmp4C8=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp4C8));
break;}case 36: _LL25C: {
# 1386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1386 "parse.y"
{struct Cyc_List_List*_tmpA9E;yyval=Cyc_YY16(((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA9E->tl=0,_tmpA9E)))))));}
break;}case 37: _LL25D: {
# 1389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1388 "parse.y"
struct Cyc_List_List*_tmp4CA=0;
{struct Cyc_List_List*_tmp4CB=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp4CB != 0;_tmp4CB=_tmp4CB->tl){
struct _dyneither_ptr*_tmp4CC=(struct _dyneither_ptr*)_tmp4CB->hd;
struct _tuple0*_tmpA9F;struct _tuple0*qv=(_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->f1=Cyc_Absyn_Rel_n(0),((_tmpA9F->f2=_tmp4CC,_tmpA9F)))));
struct Cyc_Absyn_Vardecl*_tmp4CD=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpAA0;_tmp4CA=((_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->hd=_tmp4CD,((_tmpAA0->tl=_tmp4CA,_tmpAA0))))));}}
# 1395
_tmp4CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4CA);
{struct Cyc_List_List*_tmpAA1;yyval=Cyc_YY16(((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->hd=Cyc_Absyn_letv_decl(_tmp4CA,(unsigned int)(yyyylsp[0]).first_line),((_tmpAA1->tl=0,_tmpAA1)))))));}
# 1398
break;}case 38: _LL25E: {
# 1400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1401 "parse.y"
struct _dyneither_ptr _tmp4D1=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1403
{const char*_tmpAA2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D1,((_tmpAA2="`H",_tag_dyneither(_tmpAA2,sizeof(char),3))))== 0){
const char*_tmpAA5;void*_tmpAA4;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAA4=0,Cyc_aprintf(((_tmpAA5="bad occurrence of heap region",_tag_dyneither(_tmpAA5,sizeof(char),30))),_tag_dyneither(_tmpAA4,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpAA6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D1,((_tmpAA6="`U",_tag_dyneither(_tmpAA6,sizeof(char),3))))== 0){
const char*_tmpAA9;void*_tmpAA8;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAA8=0,Cyc_aprintf(((_tmpAA9="bad occurrence of unique region",_tag_dyneither(_tmpAA9,sizeof(char),32))),_tag_dyneither(_tmpAA8,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAAC;struct Cyc_Absyn_Tvar*_tmpAAB;struct Cyc_Absyn_Tvar*tv=(_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->name=((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=_tmp4D1,_tmpAAC)))),((_tmpAAB->identity=- 1,((_tmpAAB->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAAB)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAAF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAAE;void*t=(void*)((_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=2,((_tmpAAF.f1=tv,_tmpAAF)))),_tmpAAE))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAB5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAB4;struct _dyneither_ptr*_tmpAB2;struct _tuple0*_tmpAB1;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1->f1=Cyc_Absyn_Loc_n,((_tmpAB1->f2=((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpAB2)))),_tmpAB1)))))),(void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=15,((_tmpAB5.f1=t,_tmpAB5)))),_tmpAB4)))),0);
{struct Cyc_List_List*_tmpAB6;yyval=Cyc_YY16(((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAB6->tl=0,_tmpAB6)))))));}
# 1412
break;};}case 39: _LL25F: {
# 1414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1414 "parse.y"
int _tmp4E1=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp4E2=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpAB7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E2,((_tmpAB7="H",_tag_dyneither(_tmpAB7,sizeof(char),2))))== 0){
const char*_tmpAB8;Cyc_Parse_err(((_tmpAB8="bad occurrence of heap region `H",_tag_dyneither(_tmpAB8,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpAB9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E2,((_tmpAB9="U",_tag_dyneither(_tmpAB9,sizeof(char),2))))== 0){
const char*_tmpABA;Cyc_Parse_err(((_tmpABA="bad occurrence of unique region `U",_tag_dyneither(_tmpABA,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpAC7;const char*_tmpAC6;void*_tmpAC5[1];struct Cyc_String_pa_PrintArg_struct _tmpAC4;struct Cyc_Absyn_Tvar*_tmpAC3;struct Cyc_Absyn_Tvar*tv=(_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->name=((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=(struct _dyneither_ptr)((_tmpAC4.tag=0,((_tmpAC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4E2),((_tmpAC5[0]=& _tmpAC4,Cyc_aprintf(((_tmpAC6="`%s",_tag_dyneither(_tmpAC6,sizeof(char),4))),_tag_dyneither(_tmpAC5,sizeof(void*),1)))))))),_tmpAC7)))),((_tmpAC3->identity=- 1,((_tmpAC3->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAC3)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpACA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC9;void*t=(void*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=2,((_tmpACA.f1=tv,_tmpACA)))),_tmpAC9))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAD0;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpACF;struct _dyneither_ptr*_tmpACD;struct _tuple0*_tmpACC;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->f1=Cyc_Absyn_Loc_n,((_tmpACC->f2=((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=_tmp4E2,_tmpACD)))),_tmpACC)))))),(void*)((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=15,((_tmpAD0.f1=t,_tmpAD0)))),_tmpACF)))),0);
# 1425
{struct Cyc_List_List*_tmpAD1;yyval=Cyc_YY16(((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4E1,0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAD1->tl=0,_tmpAD1)))))));}
# 1427
break;};}case 40: _LL260: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1429 "parse.y"
struct _dyneither_ptr _tmp4F3=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4F4=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4F5=Cyc_yyget_Exp_tok(yyyyvsp[6]);
{const char*_tmpAD2;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4F4,((_tmpAD2="open",_tag_dyneither(_tmpAD2,sizeof(char),5))))!= 0){const char*_tmpAD3;Cyc_Parse_err(((_tmpAD3="expecting `open'",_tag_dyneither(_tmpAD3,sizeof(char),17))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAE0;const char*_tmpADF;void*_tmpADE[1];struct Cyc_String_pa_PrintArg_struct _tmpADD;struct Cyc_Absyn_Tvar*_tmpADC;struct Cyc_Absyn_Tvar*tv=(_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->name=((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0[0]=(struct _dyneither_ptr)((_tmpADD.tag=0,((_tmpADD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4F3),((_tmpADE[0]=& _tmpADD,Cyc_aprintf(((_tmpADF="`%s",_tag_dyneither(_tmpADF,sizeof(char),4))),_tag_dyneither(_tmpADE,sizeof(void*),1)))))))),_tmpAE0)))),((_tmpADC->identity=- 1,((_tmpADC->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpADC)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAE3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAE2;void*t=(void*)((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2[0]=((_tmpAE3.tag=2,((_tmpAE3.f1=tv,_tmpAE3)))),_tmpAE2))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAE9;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAE8;struct _dyneither_ptr*_tmpAE6;struct _tuple0*_tmpAE5;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->f1=Cyc_Absyn_Loc_n,((_tmpAE5->f2=((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=_tmp4F3,_tmpAE6)))),_tmpAE5)))))),(void*)((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE9.tag=15,((_tmpAE9.f1=t,_tmpAE9)))),_tmpAE8)))),0);
# 1438
{struct Cyc_List_List*_tmpAEA;yyval=Cyc_YY16(((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4F5,(unsigned int)(yyyylsp[0]).first_line),((_tmpAEA->tl=0,_tmpAEA)))))));}
# 1440
break;};}case 41: _LL261:
# 1442
 yyval=Cyc_YY31(0);
break;case 42: _LL262: {
# 1445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1444 "parse.y"
yyval=Cyc_YY31(1);
break;}case 43: _LL263: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449 "parse.y"
yyval=yyyyvsp[0];
break;}case 44: _LL264: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1451 "parse.y"
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1])));
break;}case 45: _LL265: {
# 1454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpAEB;yyval=Cyc_YY17(((_tmpAEB.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAEB.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAEB.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAEB.is_inline=0,((_tmpAEB.attributes=0,_tmpAEB)))))))))));}
break;}case 46: _LL266: {
# 1461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1460 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp505=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp505.sc != 0){
const char*_tmpAEE;void*_tmpAED;(_tmpAED=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAEE="Only one storage class is allowed in a declaration (missing ';' or ','?)",_tag_dyneither(_tmpAEE,sizeof(char),73))),_tag_dyneither(_tmpAED,sizeof(void*),0)));}
# 1464
{struct Cyc_Parse_Declaration_spec _tmpAEF;yyval=Cyc_YY17(((_tmpAEF.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAEF.tq=_tmp505.tq,((_tmpAEF.type_specs=_tmp505.type_specs,((_tmpAEF.is_inline=_tmp505.is_inline,((_tmpAEF.attributes=_tmp505.attributes,_tmpAEF)))))))))));}
# 1468
break;}case 47: _LL267: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1469 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAF0;yyval=Cyc_YY17(((_tmpAF0.sc=0,((_tmpAF0.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAF0.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAF0.is_inline=0,((_tmpAF0.attributes=0,_tmpAF0)))))))))));}
break;}case 48: _LL268: {
# 1473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1472 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50A=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAF1;yyval=Cyc_YY17(((_tmpAF1.sc=_tmp50A.sc,((_tmpAF1.tq=_tmp50A.tq,((_tmpAF1.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp50A.type_specs,Cyc_yyget_YY21(yyyyvsp[0])),((_tmpAF1.is_inline=_tmp50A.is_inline,((_tmpAF1.attributes=_tmp50A.attributes,_tmpAF1)))))))))));}
# 1478
break;}case 49: _LL269: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1479 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAF2;yyval=Cyc_YY17(((_tmpAF2.sc=0,((_tmpAF2.tq=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAF2.type_specs=Cyc_Parse_empty_spec(0),((_tmpAF2.is_inline=0,((_tmpAF2.attributes=0,_tmpAF2)))))))))));}
break;}case 50: _LL26A: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1481 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50D=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAF3;yyval=Cyc_YY17(((_tmpAF3.sc=_tmp50D.sc,((_tmpAF3.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp50D.tq),((_tmpAF3.type_specs=_tmp50D.type_specs,((_tmpAF3.is_inline=_tmp50D.is_inline,((_tmpAF3.attributes=_tmp50D.attributes,_tmpAF3)))))))))));}
# 1486
break;}case 51: _LL26B: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1487 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAF4;yyval=Cyc_YY17(((_tmpAF4.sc=0,((_tmpAF4.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAF4.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAF4.is_inline=1,((_tmpAF4.attributes=0,_tmpAF4)))))))))));}
break;}case 52: _LL26C: {
# 1491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1490 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp510=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAF5;yyval=Cyc_YY17(((_tmpAF5.sc=_tmp510.sc,((_tmpAF5.tq=_tmp510.tq,((_tmpAF5.type_specs=_tmp510.type_specs,((_tmpAF5.is_inline=1,((_tmpAF5.attributes=_tmp510.attributes,_tmpAF5)))))))))));}
# 1494
break;}case 53: _LL26D: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1495 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAF6;yyval=Cyc_YY17(((_tmpAF6.sc=0,((_tmpAF6.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAF6.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAF6.is_inline=0,((_tmpAF6.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAF6)))))))))));}
break;}case 54: _LL26E: {
# 1499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1498 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp513=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAF7;yyval=Cyc_YY17(((_tmpAF7.sc=_tmp513.sc,((_tmpAF7.tq=_tmp513.tq,((_tmpAF7.type_specs=_tmp513.type_specs,((_tmpAF7.is_inline=_tmp513.is_inline,((_tmpAF7.attributes=
# 1501
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp513.attributes),_tmpAF7)))))))))));}
break;}case 55: _LL26F: {
# 1504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1505 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
break;}case 56: _LL270: {
# 1509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1507 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
break;}case 57: _LL271: {
# 1511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1509 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
break;}case 58: _LL272: {
# 1513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1511 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
break;}case 59: _LL273: {
# 1515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1514 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpAF8;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAF8="C",_tag_dyneither(_tmpAF8,sizeof(char),2))))!= 0){
const char*_tmpAF9;Cyc_Parse_err(((_tmpAF9="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAF9,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY20(& externC_sc);
# 1519
break;}case 60: _LL274: {
# 1521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1519 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
break;}case 61: _LL275: {
# 1523
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1522 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
break;}case 62: _LL276:
# 1526
 yyval=Cyc_YY45(0);
break;case 63: _LL277: {
# 1529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1529 "parse.y"
yyval=yyyyvsp[0];
break;}case 64: _LL278: {
# 1532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1534 "parse.y"
yyval=yyyyvsp[3];
break;}case 65: _LL279: {
# 1537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1538 "parse.y"
{struct Cyc_List_List*_tmpAFA;yyval=Cyc_YY45(((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAFA->tl=0,_tmpAFA)))))));}
break;}case 66: _LL27A: {
# 1541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1539 "parse.y"
{struct Cyc_List_List*_tmpAFB;yyval=Cyc_YY45(((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAFB->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAFB)))))));}
break;}case 67: _LL27B: {
# 1542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1544 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple28 att_map[17]={{{_tmp51B,_tmp51B,_tmp51B + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp51C,_tmp51C,_tmp51C + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51D,_tmp51D,_tmp51D + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp51E,_tmp51E,_tmp51E + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp51F,_tmp51F,_tmp51F + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp520,_tmp520,_tmp520 + 8},(void*)& att_aligned},{{_tmp521,_tmp521,_tmp521 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp522,_tmp522,_tmp522 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp523,_tmp523,_tmp523 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp524,_tmp524,_tmp524 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp525,_tmp525,_tmp525 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp526,_tmp526,_tmp526 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp527,_tmp527,_tmp527 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp528,_tmp528,_tmp528 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp529,_tmp529,_tmp529 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp52A,_tmp52A,_tmp52A + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp52B,_tmp52B,_tmp52B + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1564
struct _dyneither_ptr _tmp519=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1566
if((((_get_dyneither_size(_tmp519,sizeof(char))> 4  && ((const char*)_tmp519.curr)[0]== '_') && ((const char*)_tmp519.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp519,sizeof(char),(int)(_get_dyneither_size(_tmp519,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp519,sizeof(char),(int)(_get_dyneither_size(_tmp519,sizeof(char))- 3)))== '_')
# 1568
_tmp519=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp519,2,_get_dyneither_size(_tmp519,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp519,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1575
if(i == 17){
{const char*_tmpAFC;Cyc_Parse_err(((_tmpAFC="unrecognized attribute",_tag_dyneither(_tmpAFC,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1580
break;};}case 68: _LL27C: {
# 1582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1580 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;}case 69: _LL27D: {
# 1583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1582 "parse.y"
struct _dyneither_ptr _tmp52D=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp52E=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAFE;const char*_tmpAFD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpAFE="aligned",_tag_dyneither(_tmpAFE,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpAFD="__aligned__",_tag_dyneither(_tmpAFD,sizeof(char),12))))== 0){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpB01;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpB00;a=(void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=6,((_tmpB01.f1=_tmp52E,_tmpB01)))),_tmpB00))));}else{
const char*_tmpB03;const char*_tmpB02;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB03="section",_tag_dyneither(_tmpB03,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB02="__section__",_tag_dyneither(_tmpB02,sizeof(char),12))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp52E);
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpB06;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpB05;a=(void*)((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05[0]=((_tmpB06.tag=8,((_tmpB06.f1=str,_tmpB06)))),_tmpB05))));}else{
const char*_tmpB07;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB07="__mode__",_tag_dyneither(_tmpB07,sizeof(char),9))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp52E);
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpB0A;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpB09;a=(void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0A.tag=24,((_tmpB0A.f1=str,_tmpB0A)))),_tmpB09))));}else{
const char*_tmpB0B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB0B="alias",_tag_dyneither(_tmpB0B,sizeof(char),6))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp52E);
struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmpB0E;struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmpB0D;a=(void*)((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D[0]=((_tmpB0E.tag=25,((_tmpB0E.f1=str,_tmpB0E)))),_tmpB0D))));}else{
# 1598
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp52E);
const char*_tmpB10;const char*_tmpB0F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB10="regparm",_tag_dyneither(_tmpB10,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB0F="__regparm__",_tag_dyneither(_tmpB0F,sizeof(char),12))))== 0){
if(n < 0  || n > 3){
const char*_tmpB11;Cyc_Parse_err(((_tmpB11="regparm requires value between 0 and 3",_tag_dyneither(_tmpB11,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpB14;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpB13;a=(void*)((_tmpB13=_cycalloc_atomic(sizeof(*_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=0,((_tmpB14.f1=n,_tmpB14)))),_tmpB13))));};}else{
const char*_tmpB16;const char*_tmpB15;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB16="initializes",_tag_dyneither(_tmpB16,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB15="__initializes__",_tag_dyneither(_tmpB15,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpB19;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpB18;a=(void*)((_tmpB18=_cycalloc_atomic(sizeof(*_tmpB18)),((_tmpB18[0]=((_tmpB19.tag=20,((_tmpB19.f1=n,_tmpB19)))),_tmpB18))));}else{
const char*_tmpB1B;const char*_tmpB1A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB1B="noliveunique",_tag_dyneither(_tmpB1B,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB1A="__noliveunique__",_tag_dyneither(_tmpB1A,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpB1E;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpB1D;a=(void*)((_tmpB1D=_cycalloc_atomic(sizeof(*_tmpB1D)),((_tmpB1D[0]=((_tmpB1E.tag=21,((_tmpB1E.f1=n,_tmpB1E)))),_tmpB1D))));}else{
const char*_tmpB20;const char*_tmpB1F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB20="noconsume",_tag_dyneither(_tmpB20,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52D,((_tmpB1F="__noconsume__",_tag_dyneither(_tmpB1F,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpB23;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpB22;a=(void*)((_tmpB22=_cycalloc_atomic(sizeof(*_tmpB22)),((_tmpB22[0]=((_tmpB23.tag=22,((_tmpB23.f1=n,_tmpB23)))),_tmpB22))));}else{
# 1610
{const char*_tmpB24;Cyc_Parse_err(((_tmpB24="unrecognized attribute",_tag_dyneither(_tmpB24,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}}
# 1614
yyval=Cyc_YY46(a);
# 1616
break;}case 70: _LL27E: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1617 "parse.y"
struct _dyneither_ptr _tmp54F=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp550=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp551=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_Int_tok(yyyyvsp[4]));
unsigned int _tmp552=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[6]).first_line,Cyc_yyget_Int_tok(yyyyvsp[6]));
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpB26;const char*_tmpB25;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54F,((_tmpB26="format",_tag_dyneither(_tmpB26,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp54F,((_tmpB25="__format__",_tag_dyneither(_tmpB25,sizeof(char),11))))== 0){
const char*_tmpB27;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp550,((_tmpB27="printf",_tag_dyneither(_tmpB27,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB2A;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB29;a=(void*)((_tmpB29=_cycalloc_atomic(sizeof(*_tmpB29)),((_tmpB29[0]=((_tmpB2A.tag=19,((_tmpB2A.f1=Cyc_Absyn_Printf_ft,((_tmpB2A.f2=(int)_tmp551,((_tmpB2A.f3=(int)_tmp552,_tmpB2A)))))))),_tmpB29))));}else{
const char*_tmpB2B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp550,((_tmpB2B="scanf",_tag_dyneither(_tmpB2B,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB2E;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB2D;a=(void*)((_tmpB2D=_cycalloc_atomic(sizeof(*_tmpB2D)),((_tmpB2D[0]=((_tmpB2E.tag=19,((_tmpB2E.f1=Cyc_Absyn_Scanf_ft,((_tmpB2E.f2=(int)_tmp551,((_tmpB2E.f3=(int)_tmp552,_tmpB2E)))))))),_tmpB2D))));}else{
# 1628
const char*_tmpB2F;Cyc_Parse_err(((_tmpB2F="unrecognized format type",_tag_dyneither(_tmpB2F,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1630
const char*_tmpB30;Cyc_Parse_err(((_tmpB30="unrecognized attribute",_tag_dyneither(_tmpB30,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);
# 1633
break;}case 71: _LL27F: {
# 1635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1642 "parse.y"
yyval=yyyyvsp[0];
break;}case 72: _LL280: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1644 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB33;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB32;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB33.tag=17,((_tmpB33.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB33.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpB33.f3=0,((_tmpB33.f4=0,_tmpB33)))))))))),_tmpB32)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 73: _LL281: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1648 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL282: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1649 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL283: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL284: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL285: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1652 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL286: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1653 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL287: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL288: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL289: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL28A: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657 "parse.y"
yyval=yyyyvsp[0];
break;}case 83: _LL28B: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1658 "parse.y"
yyval=yyyyvsp[0];
break;}case 84: _LL28C: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1661 "parse.y"
{struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpB36;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB35;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB36.tag=27,((_tmpB36.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB36)))),_tmpB35)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 85: _LL28D: {
# 1664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1663 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL28E: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1665 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 87: _LL28F: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1667 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 88: _LL290: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1669 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 89: _LL291: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1671 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB39;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB38;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB39.tag=10,((_tmpB39.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpB39)))),_tmpB38)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1674
break;}case 90: _LL292: {
# 1676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1675 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB3C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB3B;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((_tmpB3C.tag=15,((_tmpB3C.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB3C)))),_tmpB3B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL293: {
# 1678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1677 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB3F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB3E;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB3E=_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E[0]=((_tmpB3F.tag=15,((_tmpB3F.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpB3F)))),_tmpB3E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1679
break;}case 92: _LL294: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1680 "parse.y"
void*_tmp567=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB42;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB41;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41[0]=((_tmpB42.tag=16,((_tmpB42.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB42.f2=_tmp567,_tmpB42)))))),_tmpB41)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1683
break;}case 93: _LL295: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1684 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB45;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB44;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB44=_cycalloc(sizeof(*_tmpB44)),((_tmpB44[0]=((_tmpB45.tag=16,((_tmpB45.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB45.f2=Cyc_yyget_YY44(yyyyvsp[4]),_tmpB45)))))),_tmpB44)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL296: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1686 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB48;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB47;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=((_tmpB48.tag=19,((_tmpB48.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB48)))),_tmpB47)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL297: {
# 1689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1688 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB4B;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB4A;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A[0]=((_tmpB4B.tag=19,((_tmpB4B.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB4B)))),_tmpB4A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 96: _LL298: {
# 1691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1690 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB4E;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB4D;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB4E.tag=18,((_tmpB4E.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB4E)))),_tmpB4D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 97: _LL299: {
# 1693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 98: _LL29A: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1700 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpB4F;yyval=Cyc_YY23(((_tmpB4F.print_const=1,((_tmpB4F.q_volatile=0,((_tmpB4F.q_restrict=0,((_tmpB4F.real_const=1,((_tmpB4F.loc=loc,_tmpB4F)))))))))));}
break;}case 99: _LL29B: {
# 1704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1702 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB50;yyval=Cyc_YY23(((_tmpB50.print_const=0,((_tmpB50.q_volatile=1,((_tmpB50.q_restrict=0,((_tmpB50.real_const=0,((_tmpB50.loc=0,_tmpB50)))))))))));}
break;}case 100: _LL29C: {
# 1705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1703 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB51;yyval=Cyc_YY23(((_tmpB51.print_const=0,((_tmpB51.q_volatile=0,((_tmpB51.q_restrict=1,((_tmpB51.real_const=0,((_tmpB51.loc=0,_tmpB51)))))))))));}
break;}case 101: _LL29D: {
# 1706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB60;struct Cyc_Absyn_Enumdecl*_tmpB5F;struct Cyc_Core_Opt*_tmpB5E;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB5D;struct Cyc_Absyn_TypeDecl*_tmpB5C;struct Cyc_Absyn_TypeDecl*_tmp575=(_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->r=(void*)((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60[0]=((_tmpB5D.tag=1,((_tmpB5D.f1=((_tmpB5F=_cycalloc(sizeof(*_tmpB5F)),((_tmpB5F->sc=Cyc_Absyn_Public,((_tmpB5F->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB5F->fields=((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpB5E)))),_tmpB5F)))))))),_tmpB5D)))),_tmpB60)))),((_tmpB5C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB5C)))));
# 1711
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB63;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB62;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62[0]=((_tmpB63.tag=26,((_tmpB63.f1=_tmp575,((_tmpB63.f2=0,_tmpB63)))))),_tmpB62)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1713
break;}case 102: _LL29E: {
# 1715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1714 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB66;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB65;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB65=_cycalloc(sizeof(*_tmpB65)),((_tmpB65[0]=((_tmpB66.tag=13,((_tmpB66.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB66.f2=0,_tmpB66)))))),_tmpB65)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL29F: {
# 1717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1716 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB69;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB68;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68[0]=((_tmpB69.tag=14,((_tmpB69.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB69)))),_tmpB68)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 104: _LL2A0: {
# 1719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB6A;yyval=Cyc_YY47(((_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB6A->tag=0,((_tmpB6A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB6A)))))))));}
break;}case 105: _LL2A1: {
# 1725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1724 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB6B;yyval=Cyc_YY47(((_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB6B->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB6B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB6B)))))))));}
break;}case 106: _LL2A2: {
# 1727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1728 "parse.y"
{struct Cyc_List_List*_tmpB6C;yyval=Cyc_YY48(((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB6C->tl=0,_tmpB6C)))))));}
break;}case 107: _LL2A3: {
# 1731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1729 "parse.y"
{struct Cyc_List_List*_tmpB6D;yyval=Cyc_YY48(((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB6D->tl=0,_tmpB6D)))))));}
break;}case 108: _LL2A4: {
# 1732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1730 "parse.y"
{struct Cyc_List_List*_tmpB6E;yyval=Cyc_YY48(((_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB6E->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB6E)))))));}
break;}case 109: _LL2A5: {
# 1733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB71;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB70;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70[0]=((_tmpB71.tag=12,((_tmpB71.f1=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB71.f2=Cyc_yyget_YY24(yyyyvsp[2]),_tmpB71)))))),_tmpB70)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 110: _LL2A6: {
# 1739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1742 "parse.y"
struct Cyc_List_List*_tmp588=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp589=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp58A=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp588,
Cyc_Absyn_aggrdecl_impl(_tmp589,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY24(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1747
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB74;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB73;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73[0]=((_tmpB74.tag=26,((_tmpB74.f1=_tmp58A,((_tmpB74.f2=0,_tmpB74)))))),_tmpB73)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1749
break;}case 111: _LL2A7: {
# 1751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1753 "parse.y"
struct Cyc_List_List*_tmp58D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp58E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp58F=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp58D,
Cyc_Absyn_aggrdecl_impl(_tmp58E,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY24(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1758
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB77;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB76;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB77.tag=26,((_tmpB77.f1=_tmp58F,((_tmpB77.f2=0,_tmpB77)))))),_tmpB76)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1760
break;}case 112: _LL2A8: {
# 1762
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1761 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB81;struct Cyc_Core_Opt*_tmpB80;struct Cyc_Absyn_AggrInfo _tmpB7F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB7E;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB81.tag=11,((_tmpB81.f1=((_tmpB7F.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB80=_cycalloc_atomic(sizeof(*_tmpB80)),((_tmpB80->v=(void*)1,_tmpB80))))),((_tmpB7F.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpB7F)))),_tmpB81)))),_tmpB7E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1764
break;}case 113: _LL2A9: {
# 1766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1765 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB87;struct Cyc_Absyn_AggrInfo _tmpB86;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB85;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB87.tag=11,((_tmpB87.f1=((_tmpB86.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB86.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB86)))),_tmpB87)))),_tmpB85)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1768
break;}case 114: _LL2AA:
# 1770
 yyval=Cyc_YY40(0);
break;case 115: _LL2AB: {
# 1773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1774 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
break;}case 116: _LL2AC: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1778 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
break;}case 117: _LL2AD: {
# 1781
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1783
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1779 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
break;}case 118: _LL2AE:
# 1782
 yyval=Cyc_YY24(0);
break;case 119: _LL2AF: {
# 1785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1786 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp599=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp599 != 0;_tmp599=_tmp599->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp599->hd,decls);}}{
# 1790
struct Cyc_List_List*_tmp59A=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp59A != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp59A,decls);
yyval=Cyc_YY24(decls);
# 1795
break;};}case 120: _LL2B0: {
# 1797
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB88;yyval=Cyc_YY25(((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->hd=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB88->tl=0,_tmpB88)))))));}
break;}case 121: _LL2B1: {
# 1803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1802 "parse.y"
{struct Cyc_List_List*_tmpB89;yyval=Cyc_YY25(((_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->hd=Cyc_yyget_YY24(yyyyvsp[1]),((_tmpB89->tl=Cyc_yyget_YY25(yyyyvsp[0]),_tmpB89)))))));}
break;}case 122: _LL2B2: {
# 1805
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1806 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
break;}case 123: _LL2B3: {
# 1809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB8A;yyval=Cyc_YY19(((_tmpB8A=_region_malloc(yyr,sizeof(*_tmpB8A)),((_tmpB8A->tl=0,((_tmpB8A->hd=Cyc_yyget_YY18(yyyyvsp[0]),_tmpB8A)))))));}
break;}case 124: _LL2B4: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1814 "parse.y"
{struct _tuple12*_tmpB8B;yyval=Cyc_YY19(((_tmpB8B=_region_malloc(yyr,sizeof(*_tmpB8B)),((_tmpB8B->tl=Cyc_yyget_YY19(yyyyvsp[0]),((_tmpB8B->hd=Cyc_yyget_YY18(yyyyvsp[2]),_tmpB8B)))))));}
break;}case 125: _LL2B5: {
# 1817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1819 "parse.y"
{struct _tuple11 _tmpB8C;yyval=Cyc_YY18(((_tmpB8C.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB8C.f2=0,_tmpB8C)))));}
break;}case 126: _LL2B6: {
# 1822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1821 "parse.y"
{struct _tuple11 _tmpB8D;yyval=Cyc_YY18(((_tmpB8D.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB8D.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB8D)))));}
break;}case 127: _LL2B7: {
# 1824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1827 "parse.y"
struct _RegionHandle _tmp5A1=_new_region("temp");struct _RegionHandle*temp=& _tmp5A1;_push_region(temp);{
struct _tuple25 _tmp5A2=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp5A4;struct Cyc_Parse_Type_specifier _tmp5A5;struct Cyc_List_List*_tmp5A6;struct _tuple25 _tmp5A3=_tmp5A2;_tmp5A4=_tmp5A3.f1;_tmp5A5=_tmp5A3.f2;_tmp5A6=_tmp5A3.f3;
if(_tmp5A4.loc == 0)_tmp5A4.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp5A7=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp5A7 != 0;_tmp5A7=_tmp5A7->tl){
struct _tuple24*_tmp5A8=(struct _tuple24*)_tmp5A7->hd;struct Cyc_Parse_Declarator _tmp5AA;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AC;struct _tuple24*_tmp5A9=_tmp5A8;_tmp5AA=_tmp5A9->f1;_tmp5AB=_tmp5A9->f2;_tmp5AC=_tmp5A9->f3;
{struct _tuple10*_tmpB8E;decls=((_tmpB8E=_region_malloc(temp,sizeof(*_tmpB8E)),((_tmpB8E->tl=decls,((_tmpB8E->hd=_tmp5AA,_tmpB8E))))));}{
struct _tuple16*_tmpB91;struct Cyc_List_List*_tmpB90;widths_and_reqs=(
(_tmpB90=_region_malloc(temp,sizeof(*_tmpB90)),((_tmpB90->hd=((_tmpB91=_region_malloc(temp,sizeof(*_tmpB91)),((_tmpB91->f1=_tmp5AB,((_tmpB91->f2=_tmp5AC,_tmpB91)))))),((_tmpB90->tl=widths_and_reqs,_tmpB90))))));};}}
# 1838
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp5B0=Cyc_Parse_speclist2typ(_tmp5A5,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp5B1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp5A4,_tmp5B0,decls,_tmp5A6),widths_and_reqs);
# 1844
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp5B1));};};}
# 1847
_npop_handler(0);break;
# 1827
;_pop_region(temp);}case 128: _LL2B8: {
# 1849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1855 "parse.y"
{struct _tuple25 _tmpB92;yyval=Cyc_YY35(((_tmpB92.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB92.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB92.f3=0,_tmpB92)))))));}
break;}case 129: _LL2B9: {
# 1858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1857 "parse.y"
struct _tuple25 _tmp5B3=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB93;yyval=Cyc_YY35(((_tmpB93.f1=_tmp5B3.f1,((_tmpB93.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5B3.f2),((_tmpB93.f3=_tmp5B3.f3,_tmpB93)))))));}
break;}case 130: _LL2BA: {
# 1860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1859 "parse.y"
{struct _tuple25 _tmpB94;yyval=Cyc_YY35(((_tmpB94.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB94.f2=Cyc_Parse_empty_spec(0),((_tmpB94.f3=0,_tmpB94)))))));}
break;}case 131: _LL2BB: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1861 "parse.y"
struct _tuple25 _tmp5B6=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB95;yyval=Cyc_YY35(((_tmpB95.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5B6.f1),((_tmpB95.f2=_tmp5B6.f2,((_tmpB95.f3=_tmp5B6.f3,_tmpB95)))))));}
break;}case 132: _LL2BC: {
# 1865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1864 "parse.y"
{struct _tuple25 _tmpB96;yyval=Cyc_YY35(((_tmpB96.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB96.f2=Cyc_Parse_empty_spec(0),((_tmpB96.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB96)))))));}
break;}case 133: _LL2BD: {
# 1867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1866 "parse.y"
struct _tuple25 _tmp5B9=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB97;yyval=Cyc_YY35(((_tmpB97.f1=_tmp5B9.f1,((_tmpB97.f2=_tmp5B9.f2,((_tmpB97.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5B9.f3),_tmpB97)))))));}
break;}case 134: _LL2BE: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB98;yyval=Cyc_YY35(((_tmpB98.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB98.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB98.f3=0,_tmpB98)))))));}
break;}case 135: _LL2BF: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1874 "parse.y"
struct _tuple25 _tmp5BC=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB99;yyval=Cyc_YY35(((_tmpB99.f1=_tmp5BC.f1,((_tmpB99.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5BC.f2),((_tmpB99.f3=_tmp5BC.f3,_tmpB99)))))));}
break;}case 136: _LL2C0: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1876 "parse.y"
{struct _tuple25 _tmpB9A;yyval=Cyc_YY35(((_tmpB9A.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB9A.f2=Cyc_Parse_empty_spec(0),((_tmpB9A.f3=0,_tmpB9A)))))));}
break;}case 137: _LL2C1: {
# 1879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1878 "parse.y"
struct _tuple25 _tmp5BF=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB9B;yyval=Cyc_YY35(((_tmpB9B.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5BF.f1),((_tmpB9B.f2=_tmp5BF.f2,((_tmpB9B.f3=_tmp5BF.f3,_tmpB9B)))))));}
break;}case 138: _LL2C2: {
# 1882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1881 "parse.y"
{struct _tuple25 _tmpB9C;yyval=Cyc_YY35(((_tmpB9C.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB9C.f2=Cyc_Parse_empty_spec(0),((_tmpB9C.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB9C)))))));}
break;}case 139: _LL2C3: {
# 1884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1883 "parse.y"
struct _tuple25 _tmp5C2=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB9D;yyval=Cyc_YY35(((_tmpB9D.f1=_tmp5C2.f1,((_tmpB9D.f2=_tmp5C2.f2,((_tmpB9D.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5C2.f3),_tmpB9D)))))));}
break;}case 140: _LL2C4: {
# 1886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1887 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
break;}case 141: _LL2C5: {
# 1890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB9E;yyval=Cyc_YY29(((_tmpB9E=_region_malloc(yyr,sizeof(*_tmpB9E)),((_tmpB9E->hd=Cyc_yyget_YY28(yyyyvsp[0]),((_tmpB9E->tl=0,_tmpB9E)))))));}
break;}case 142: _LL2C6: {
# 1896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1895 "parse.y"
{struct Cyc_List_List*_tmpB9F;yyval=Cyc_YY29(((_tmpB9F=_region_malloc(yyr,sizeof(*_tmpB9F)),((_tmpB9F->hd=Cyc_yyget_YY28(yyyyvsp[2]),((_tmpB9F->tl=Cyc_yyget_YY29(yyyyvsp[0]),_tmpB9F)))))));}
break;}case 143: _LL2C7: {
# 1898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1900 "parse.y"
{struct _tuple24*_tmpBA0;yyval=Cyc_YY28(((_tmpBA0=_region_malloc(yyr,sizeof(*_tmpBA0)),((_tmpBA0->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpBA0->f2=0,((_tmpBA0->f3=Cyc_yyget_YY55(yyyyvsp[1]),_tmpBA0)))))))));}
break;}case 144: _LL2C8: {
# 1903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1904 "parse.y"
{struct Cyc_Parse_Declarator _tmpBA6;struct _tuple0*_tmpBA5;struct _tuple24*_tmpBA4;yyval=Cyc_YY28(((_tmpBA4=_region_malloc(yyr,sizeof(*_tmpBA4)),((_tmpBA4->f1=((_tmpBA6.id=((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5->f1=Cyc_Absyn_Rel_n(0),((_tmpBA5->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpBA5)))))),((_tmpBA6.tms=0,_tmpBA6)))),((_tmpBA4->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpBA4->f3=0,_tmpBA4)))))))));}
# 1906
break;}case 145: _LL2C9:
# 1910
{struct Cyc_Parse_Declarator _tmpBAC;struct _tuple0*_tmpBAB;struct _tuple24*_tmpBAA;yyval=Cyc_YY28(((_tmpBAA=_region_malloc(yyr,sizeof(*_tmpBAA)),((_tmpBAA->f1=((_tmpBAC.id=((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB->f1=Cyc_Absyn_Rel_n(0),((_tmpBAB->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpBAB)))))),((_tmpBAC.tms=0,_tmpBAC)))),((_tmpBAA->f2=0,((_tmpBAA->f3=0,_tmpBAA)))))))));}
# 1912
break;case 146: _LL2CA: {
# 1914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1912 "parse.y"
{struct _tuple24*_tmpBAD;yyval=Cyc_YY28(((_tmpBAD=_region_malloc(yyr,sizeof(*_tmpBAD)),((_tmpBAD->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpBAD->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBAD->f3=0,_tmpBAD)))))))));}
break;}case 147: _LL2CB: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1916 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 148: _LL2CC:
# 1919
 yyval=Cyc_YY55(0);
break;case 149: _LL2CD: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1921 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 150: _LL2CE:
# 1924
 yyval=Cyc_YY55(0);
break;case 151: _LL2CF: {
# 1927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1928 "parse.y"
int _tmp5D0=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp5D1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpBAE;struct Cyc_Absyn_TypeDecl*_tmp5D2=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5D1,((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpBAE)))),_tmp5D0,(unsigned int)(yyyylsp[0]).first_line);
# 1932
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpBB1;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB0;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=((_tmpBB1.tag=26,((_tmpBB1.f1=_tmp5D2,((_tmpBB1.f2=0,_tmpBB1)))))),_tmpBB0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1934
break;}case 152: _LL2D0: {
# 1936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1935 "parse.y"
int _tmp5D6=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpBBB;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBBA;struct Cyc_Absyn_DatatypeInfo _tmpBB9;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpBB8;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8[0]=((_tmpBBB.tag=3,((_tmpBBB.f1=((_tmpBB9.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpBBA.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBBA.is_extensible=_tmp5D6,_tmpBBA))))),((_tmpBB9.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpBB9)))),_tmpBBB)))),_tmpBB8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1938
break;}case 153: _LL2D1: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1939 "parse.y"
int _tmp5DB=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpBC5;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBC4;struct Cyc_Absyn_DatatypeFieldInfo _tmpBC3;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpBC2;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((_tmpBC2[0]=((_tmpBC5.tag=4,((_tmpBC5.f1=((_tmpBC3.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpBC4.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBC4.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpBC4.is_extensible=_tmp5DB,_tmpBC4))))))),((_tmpBC3.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpBC3)))),_tmpBC5)))),_tmpBC2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1944
break;}case 154: _LL2D2: {
# 1946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1947 "parse.y"
yyval=Cyc_YY31(0);
break;}case 155: _LL2D3: {
# 1950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1948 "parse.y"
yyval=Cyc_YY31(1);
break;}case 156: _LL2D4: {
# 1951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1952 "parse.y"
{struct Cyc_List_List*_tmpBC6;yyval=Cyc_YY34(((_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBC6->tl=0,_tmpBC6)))))));}
break;}case 157: _LL2D5: {
# 1955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1953 "parse.y"
{struct Cyc_List_List*_tmpBC7;yyval=Cyc_YY34(((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBC7->tl=0,_tmpBC7)))))));}
break;}case 158: _LL2D6: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1954 "parse.y"
{struct Cyc_List_List*_tmpBC8;yyval=Cyc_YY34(((_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBC8->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBC8)))))));}
break;}case 159: _LL2D7: {
# 1957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1955 "parse.y"
{struct Cyc_List_List*_tmpBC9;yyval=Cyc_YY34(((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBC9->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBC9)))))));}
break;}case 160: _LL2D8:
# 1958
 yyval=Cyc_YY32(Cyc_Absyn_Public);
break;case 161: _LL2D9: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1960 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
break;}case 162: _LL2DA: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1961 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
break;}case 163: _LL2DB: {
# 1964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1965 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpBCA;yyval=Cyc_YY33(((_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBCA->typs=0,((_tmpBCA->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBCA->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBCA)))))))))));}
break;}case 164: _LL2DC: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1967 "parse.y"
struct Cyc_List_List*_tmp5E5=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpBCB;yyval=Cyc_YY33(((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBCB->typs=_tmp5E5,((_tmpBCB->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBCB->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBCB)))))))))));}
break;}case 165: _LL2DD: {
# 1971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1973 "parse.y"
yyval=yyyyvsp[0];
break;}case 166: _LL2DE: {
# 1976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1975 "parse.y"
struct Cyc_Parse_Declarator _tmp5E7=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBCC;yyval=Cyc_YY27(((_tmpBCC.id=_tmp5E7.id,((_tmpBCC.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5E7.tms),_tmpBCC)))));}
break;}case 167: _LL2DF: {
# 1979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 168: _LL2E0: {
# 1985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1984 "parse.y"
struct Cyc_Parse_Declarator _tmp5E9=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBCD;yyval=Cyc_YY27(((_tmpBCD.id=_tmp5E9.id,((_tmpBCD.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5E9.tms),_tmpBCD)))));}
break;}case 169: _LL2E1: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1990 "parse.y"
{struct Cyc_Parse_Declarator _tmpBCE;yyval=Cyc_YY27(((_tmpBCE.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBCE.tms=0,_tmpBCE)))));}
break;}case 170: _LL2E2: {
# 1993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1992 "parse.y"
yyval=yyyyvsp[1];
break;}case 171: _LL2E3: {
# 1995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1996 "parse.y"
struct Cyc_Parse_Declarator _tmp5EC=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBD4;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBD3;struct Cyc_List_List*_tmpBD2;_tmp5EC.tms=((_tmpBD2=_region_malloc(yyr,sizeof(*_tmpBD2)),((_tmpBD2->hd=(void*)((_tmpBD4=_region_malloc(yyr,sizeof(*_tmpBD4)),((_tmpBD4[0]=((_tmpBD3.tag=5,((_tmpBD3.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBD3.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBD3)))))),_tmpBD4)))),((_tmpBD2->tl=_tmp5EC.tms,_tmpBD2))))));}
yyval=yyyyvsp[2];
# 2000
break;}case 172: _LL2E4: {
# 2002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2001 "parse.y"
{struct Cyc_List_List*_tmpBDE;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBDD;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBDC;struct Cyc_Parse_Declarator _tmpBDB;yyval=Cyc_YY27(((_tmpBDB.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBDB.tms=((_tmpBDE=_region_malloc(yyr,sizeof(*_tmpBDE)),((_tmpBDE->hd=(void*)((_tmpBDC=_region_malloc(yyr,sizeof(*_tmpBDC)),((_tmpBDC[0]=((_tmpBDD.tag=0,((_tmpBDD.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBDD.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBDD)))))),_tmpBDC)))),((_tmpBDE->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBDE)))))),_tmpBDB)))));}
break;}case 173: _LL2E5: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2003 "parse.y"
{struct Cyc_List_List*_tmpBE8;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBE7;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBE6;struct Cyc_Parse_Declarator _tmpBE5;yyval=Cyc_YY27(((_tmpBE5.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBE5.tms=(
(_tmpBE8=_region_malloc(yyr,sizeof(*_tmpBE8)),((_tmpBE8->hd=(void*)((_tmpBE6=_region_malloc(yyr,sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE7.tag=1,((_tmpBE7.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBE7.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBE7.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBE7)))))))),_tmpBE6)))),((_tmpBE8->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBE8)))))),_tmpBE5)))));}
break;}case 174: _LL2E6: {
# 2007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2006 "parse.y"
struct _tuple26*_tmp5F8=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp5FA;int _tmp5FB;struct Cyc_Absyn_VarargInfo*_tmp5FC;void*_tmp5FD;struct Cyc_List_List*_tmp5FE;struct _tuple26*_tmp5F9=_tmp5F8;_tmp5FA=_tmp5F9->f1;_tmp5FB=_tmp5F9->f2;_tmp5FC=_tmp5F9->f3;_tmp5FD=_tmp5F9->f4;_tmp5FE=_tmp5F9->f5;{
struct Cyc_Absyn_Exp*_tmp5FF=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp600=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_List_List*_tmpBFD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBFC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBFB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBFA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBF9;struct Cyc_Parse_Declarator _tmpBF8;yyval=Cyc_YY27(((_tmpBF8.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBF8.tms=((_tmpBFD=_region_malloc(yyr,sizeof(*_tmpBFD)),((_tmpBFD->hd=(void*)((_tmpBF9=_region_malloc(yyr,sizeof(*_tmpBF9)),((_tmpBF9[0]=((_tmpBFC.tag=3,((_tmpBFC.f1=(void*)((_tmpBFA=_region_malloc(yyr,sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFB.tag=1,((_tmpBFB.f1=_tmp5FA,((_tmpBFB.f2=_tmp5FB,((_tmpBFB.f3=_tmp5FC,((_tmpBFB.f4=_tmp5FD,((_tmpBFB.f5=_tmp5FE,((_tmpBFB.f6=_tmp5FF,((_tmpBFB.f7=_tmp600,_tmpBFB)))))))))))))))),_tmpBFA)))),_tmpBFC)))),_tmpBF9)))),((_tmpBFD->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBFD)))))),_tmpBF8)))));}
# 2011
break;};}case 175: _LL2E7: {
# 2013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2012 "parse.y"
{struct Cyc_List_List*_tmpC12;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC11;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC10;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC0F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC0E;struct Cyc_Parse_Declarator _tmpC0D;yyval=Cyc_YY27(((_tmpC0D.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC0D.tms=(
(_tmpC12=_region_malloc(yyr,sizeof(*_tmpC12)),((_tmpC12->hd=(void*)((_tmpC0E=_region_malloc(yyr,sizeof(*_tmpC0E)),((_tmpC0E[0]=((_tmpC11.tag=3,((_tmpC11.f1=(void*)((_tmpC0F=_region_malloc(yyr,sizeof(*_tmpC0F)),((_tmpC0F[0]=((_tmpC10.tag=1,((_tmpC10.f1=0,((_tmpC10.f2=0,((_tmpC10.f3=0,((_tmpC10.f4=
# 2015
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC10.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC10.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC10.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC10)))))))))))))))),_tmpC0F)))),_tmpC11)))),_tmpC0E)))),((_tmpC12->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC12)))))),_tmpC0D)))));}
# 2018
break;}case 176: _LL2E8: {
# 2020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2019 "parse.y"
{struct Cyc_List_List*_tmpC27;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC26;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC25;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC24;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC23;struct Cyc_Parse_Declarator _tmpC22;yyval=Cyc_YY27(((_tmpC22.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC22.tms=((_tmpC27=_region_malloc(yyr,sizeof(*_tmpC27)),((_tmpC27->hd=(void*)((_tmpC23=_region_malloc(yyr,sizeof(*_tmpC23)),((_tmpC23[0]=((_tmpC26.tag=3,((_tmpC26.f1=(void*)((_tmpC24=_region_malloc(yyr,sizeof(*_tmpC24)),((_tmpC24[0]=((_tmpC25.tag=0,((_tmpC25.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC25.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC25)))))),_tmpC24)))),_tmpC26)))),_tmpC23)))),((_tmpC27->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC27)))))),_tmpC22)))));}
break;}case 177: _LL2E9: {
# 2022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2022 "parse.y"
struct Cyc_List_List*_tmp613=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpC31;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC30;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC2F;struct Cyc_Parse_Declarator _tmpC2E;yyval=Cyc_YY27(((_tmpC2E.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC2E.tms=((_tmpC31=_region_malloc(yyr,sizeof(*_tmpC31)),((_tmpC31->hd=(void*)((_tmpC2F=_region_malloc(yyr,sizeof(*_tmpC2F)),((_tmpC2F[0]=((_tmpC30.tag=4,((_tmpC30.f1=_tmp613,((_tmpC30.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC30.f3=0,_tmpC30)))))))),_tmpC2F)))),((_tmpC31->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC31)))))),_tmpC2E)))));}
# 2025
break;}case 178: _LL2EA: {
# 2027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2026 "parse.y"
{struct Cyc_List_List*_tmpC3B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC3A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC39;struct Cyc_Parse_Declarator _tmpC38;yyval=Cyc_YY27(((_tmpC38.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC38.tms=((_tmpC3B=_region_malloc(yyr,sizeof(*_tmpC3B)),((_tmpC3B->hd=(void*)((_tmpC39=_region_malloc(yyr,sizeof(*_tmpC39)),((_tmpC39[0]=((_tmpC3A.tag=5,((_tmpC3A.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC3A.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC3A)))))),_tmpC39)))),((_tmpC3B->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC3B)))))),_tmpC38)))));}
# 2029
break;}case 179: _LL2EB: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2040 "parse.y"
{struct Cyc_Parse_Declarator _tmpC3C;yyval=Cyc_YY27(((_tmpC3C.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC3C.tms=0,_tmpC3C)))));}
break;}case 180: _LL2EC: {
# 2043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2042 "parse.y"
{struct Cyc_Parse_Declarator _tmpC3D;yyval=Cyc_YY27(((_tmpC3D.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC3D.tms=0,_tmpC3D)))));}
break;}case 181: _LL2ED: {
# 2045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2044 "parse.y"
yyval=yyyyvsp[1];
break;}case 182: _LL2EE: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2048 "parse.y"
struct Cyc_Parse_Declarator _tmp61E=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC43;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC42;struct Cyc_List_List*_tmpC41;_tmp61E.tms=((_tmpC41=_region_malloc(yyr,sizeof(*_tmpC41)),((_tmpC41->hd=(void*)((_tmpC43=_region_malloc(yyr,sizeof(*_tmpC43)),((_tmpC43[0]=((_tmpC42.tag=5,((_tmpC42.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC42.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC42)))))),_tmpC43)))),((_tmpC41->tl=_tmp61E.tms,_tmpC41))))));}
yyval=yyyyvsp[2];
# 2052
break;}case 183: _LL2EF: {
# 2054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2053 "parse.y"
struct Cyc_Parse_Declarator _tmp622=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC4D;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC4C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC4B;struct Cyc_Parse_Declarator _tmpC4A;yyval=Cyc_YY27(((_tmpC4A.id=_tmp622.id,((_tmpC4A.tms=(
(_tmpC4D=_region_malloc(yyr,sizeof(*_tmpC4D)),((_tmpC4D->hd=(void*)((_tmpC4B=_region_malloc(yyr,sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4C.tag=0,((_tmpC4C.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpC4C.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC4C)))))),_tmpC4B)))),((_tmpC4D->tl=_tmp622.tms,_tmpC4D)))))),_tmpC4A)))));}
break;}case 184: _LL2F0: {
# 2058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2057 "parse.y"
struct Cyc_Parse_Declarator _tmp627=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC57;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC56;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC55;struct Cyc_Parse_Declarator _tmpC54;yyval=Cyc_YY27(((_tmpC54.id=_tmp627.id,((_tmpC54.tms=(
(_tmpC57=_region_malloc(yyr,sizeof(*_tmpC57)),((_tmpC57->hd=(void*)((_tmpC55=_region_malloc(yyr,sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=1,((_tmpC56.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpC56.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC56.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC56)))))))),_tmpC55)))),((_tmpC57->tl=_tmp627.tms,_tmpC57)))))),_tmpC54)))));}
# 2061
break;}case 185: _LL2F1: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2062 "parse.y"
struct _tuple26*_tmp62C=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp62E;int _tmp62F;struct Cyc_Absyn_VarargInfo*_tmp630;void*_tmp631;struct Cyc_List_List*_tmp632;struct _tuple26*_tmp62D=_tmp62C;_tmp62E=_tmp62D->f1;_tmp62F=_tmp62D->f2;_tmp630=_tmp62D->f3;_tmp631=_tmp62D->f4;_tmp632=_tmp62D->f5;{
struct Cyc_Absyn_Exp*_tmp633=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp634=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp635=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC6C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC6B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC6A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC69;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC68;struct Cyc_Parse_Declarator _tmpC67;yyval=Cyc_YY27(((_tmpC67.id=_tmp635.id,((_tmpC67.tms=((_tmpC6C=_region_malloc(yyr,sizeof(*_tmpC6C)),((_tmpC6C->hd=(void*)((_tmpC68=_region_malloc(yyr,sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC6B.tag=3,((_tmpC6B.f1=(void*)((_tmpC69=_region_malloc(yyr,sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=1,((_tmpC6A.f1=_tmp62E,((_tmpC6A.f2=_tmp62F,((_tmpC6A.f3=_tmp630,((_tmpC6A.f4=_tmp631,((_tmpC6A.f5=_tmp632,((_tmpC6A.f6=_tmp633,((_tmpC6A.f7=_tmp634,_tmpC6A)))))))))))))))),_tmpC69)))),_tmpC6B)))),_tmpC68)))),((_tmpC6C->tl=_tmp635.tms,_tmpC6C)))))),_tmpC67)))));}
# 2068
break;};}case 186: _LL2F2: {
# 2070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2069 "parse.y"
struct Cyc_Parse_Declarator _tmp63C=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC81;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC80;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC7F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC7E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC7D;struct Cyc_Parse_Declarator _tmpC7C;yyval=Cyc_YY27(((_tmpC7C.id=_tmp63C.id,((_tmpC7C.tms=(
(_tmpC81=_region_malloc(yyr,sizeof(*_tmpC81)),((_tmpC81->hd=(void*)((_tmpC7D=_region_malloc(yyr,sizeof(*_tmpC7D)),((_tmpC7D[0]=((_tmpC80.tag=3,((_tmpC80.f1=(void*)((_tmpC7E=_region_malloc(yyr,sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7F.tag=1,((_tmpC7F.f1=0,((_tmpC7F.f2=0,((_tmpC7F.f3=0,((_tmpC7F.f4=
# 2073
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC7F.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC7F.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC7F.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC7F)))))))))))))))),_tmpC7E)))),_tmpC80)))),_tmpC7D)))),((_tmpC81->tl=_tmp63C.tms,_tmpC81)))))),_tmpC7C)))));}
# 2076
break;}case 187: _LL2F3: {
# 2078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2077 "parse.y"
struct Cyc_Parse_Declarator _tmp643=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC96;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC95;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC94;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC93;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC92;struct Cyc_Parse_Declarator _tmpC91;yyval=Cyc_YY27(((_tmpC91.id=_tmp643.id,((_tmpC91.tms=((_tmpC96=_region_malloc(yyr,sizeof(*_tmpC96)),((_tmpC96->hd=(void*)((_tmpC92=_region_malloc(yyr,sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC95.tag=3,((_tmpC95.f1=(void*)((_tmpC93=_region_malloc(yyr,sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC94.tag=0,((_tmpC94.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC94.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC94)))))),_tmpC93)))),_tmpC95)))),_tmpC92)))),((_tmpC96->tl=_tmp643.tms,_tmpC96)))))),_tmpC91)))));}
break;}case 188: _LL2F4: {
# 2081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2081 "parse.y"
struct Cyc_List_List*_tmp64A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp64B=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpCA0;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC9F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC9E;struct Cyc_Parse_Declarator _tmpC9D;yyval=Cyc_YY27(((_tmpC9D.id=_tmp64B.id,((_tmpC9D.tms=((_tmpCA0=_region_malloc(yyr,sizeof(*_tmpCA0)),((_tmpCA0->hd=(void*)((_tmpC9E=_region_malloc(yyr,sizeof(*_tmpC9E)),((_tmpC9E[0]=((_tmpC9F.tag=4,((_tmpC9F.f1=_tmp64A,((_tmpC9F.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC9F.f3=0,_tmpC9F)))))))),_tmpC9E)))),((_tmpCA0->tl=_tmp64B.tms,_tmpCA0)))))),_tmpC9D)))));}
# 2085
break;}case 189: _LL2F5: {
# 2087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2086 "parse.y"
struct Cyc_Parse_Declarator _tmp650=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpCAA;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpCA9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpCA8;struct Cyc_Parse_Declarator _tmpCA7;yyval=Cyc_YY27(((_tmpCA7.id=_tmp650.id,((_tmpCA7.tms=((_tmpCAA=_region_malloc(yyr,sizeof(*_tmpCAA)),((_tmpCAA->hd=(void*)((_tmpCA8=_region_malloc(yyr,sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=5,((_tmpCA9.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpCA9.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpCA9)))))),_tmpCA8)))),((_tmpCAA->tl=_tmp650.tms,_tmpCAA)))))),_tmpCA7)))));}
# 2089
break;}case 190: _LL2F6: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2099 "parse.y"
yyval=yyyyvsp[0];
break;}case 191: _LL2F7: {
# 2102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2100 "parse.y"
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 192: _LL2F8: {
# 2103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2104 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpCB0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpCAF;struct Cyc_List_List*_tmpCAE;ans=((_tmpCAE=_region_malloc(yyr,sizeof(*_tmpCAE)),((_tmpCAE->hd=(void*)((_tmpCB0=_region_malloc(yyr,sizeof(*_tmpCB0)),((_tmpCB0[0]=((_tmpCAF.tag=5,((_tmpCAF.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpCAF.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpCAF)))))),_tmpCB0)))),((_tmpCAE->tl=ans,_tmpCAE))))));}{
# 2108
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp658=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp65A;union Cyc_Absyn_Constraint*_tmp65B;union Cyc_Absyn_Constraint*_tmp65C;struct _tuple21 _tmp659=_tmp658;_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;_tmp65C=_tmp659.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpCB1;ptrloc=((_tmpCB1=_cycalloc_atomic(sizeof(*_tmpCB1)),((_tmpCB1->ptr_loc=_tmp65A,((_tmpCB1->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpCB1->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpCB1))))))));}{
# 2113
struct _tuple14 _tmp65E=Cyc_Parse_collapse_pointer_quals(_tmp65A,_tmp65B,_tmp65C,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp660;union Cyc_Absyn_Constraint*_tmp661;union Cyc_Absyn_Constraint*_tmp662;void*_tmp663;struct _tuple14 _tmp65F=_tmp65E;_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;_tmp662=_tmp65F.f3;_tmp663=_tmp65F.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpCBB;struct Cyc_Absyn_PtrAtts _tmpCBA;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpCB9;struct Cyc_List_List*_tmpCB8;ans=((_tmpCB8=_region_malloc(yyr,sizeof(*_tmpCB8)),((_tmpCB8->hd=(void*)((_tmpCBB=_region_malloc(yyr,sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCB9.tag=2,((_tmpCB9.f1=((_tmpCBA.rgn=_tmp663,((_tmpCBA.nullable=_tmp660,((_tmpCBA.bounds=_tmp661,((_tmpCBA.zero_term=_tmp662,((_tmpCBA.ptrloc=ptrloc,_tmpCBA)))))))))),((_tmpCB9.f2=Cyc_yyget_YY23(yyyyvsp[4]),_tmpCB9)))))),_tmpCBB)))),((_tmpCB8->tl=ans,_tmpCB8))))));}
yyval=Cyc_YY26(ans);
# 2117
break;};};}case 193: _LL2F9:
# 2119
 yyval=Cyc_YY54(0);
break;case 194: _LL2FA: {
# 2122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2120 "parse.y"
{struct Cyc_List_List*_tmpCBC;yyval=Cyc_YY54(((_tmpCBC=_region_malloc(yyr,sizeof(*_tmpCBC)),((_tmpCBC->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpCBC->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpCBC)))))));}
break;}case 195: _LL2FB: {
# 2123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2125 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpCBF;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpCBE;yyval=Cyc_YY53((void*)((_tmpCBE=_region_malloc(yyr,sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCBF.tag=0,((_tmpCBF.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpCBF)))),_tmpCBE)))));}
break;}case 196: _LL2FC: {
# 2128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2127 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpCC2;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpCC1;yyval=Cyc_YY53((void*)((_tmpCC1=_region_malloc(yyr,sizeof(*_tmpCC1)),((_tmpCC1[0]=((_tmpCC2.tag=1,((_tmpCC2.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCC2)))),_tmpCC1)))));}
break;}case 197: _LL2FD: {
# 2130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2129 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpCC5;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpCC4;yyval=Cyc_YY53((void*)((_tmpCC4=_region_malloc(yyr,sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC5.tag=2,_tmpCC5)),_tmpCC4)))));}
break;}case 198: _LL2FE: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2131 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpCC8;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpCC7;yyval=Cyc_YY53((void*)((_tmpCC7=_region_malloc(yyr,sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=3,_tmpCC8)),_tmpCC7)))));}
break;}case 199: _LL2FF: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2133 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCCB;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCCA;yyval=Cyc_YY53((void*)((_tmpCCA=_region_malloc(yyr,sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCCB.tag=4,_tmpCCB)),_tmpCCA)))));}
break;}case 200: _LL300: {
# 2136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2135 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCCE;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCCD;yyval=Cyc_YY53((void*)((_tmpCCD=_region_malloc(yyr,sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCE.tag=5,_tmpCCE)),_tmpCCD)))));}
break;}case 201: _LL301: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2137 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpCD1;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpCD0;yyval=Cyc_YY53((void*)((_tmpCD0=_region_malloc(yyr,sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=6,_tmpCD1)),_tmpCD0)))));}
break;}case 202: _LL302: {
# 2140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2139 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpCD4;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpCD3;yyval=Cyc_YY53((void*)((_tmpCD3=_region_malloc(yyr,sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=7,_tmpCD4)),_tmpCD3)))));}
break;}case 203: _LL303: {
# 2142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2145 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCD5;yyval=Cyc_YY1(((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->f1=loc,((_tmpCD5->f2=Cyc_Absyn_true_conref,((_tmpCD5->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCD5)))))))));}
# 2148
break;}case 204: _LL304: {
# 2150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2149 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCD6;yyval=Cyc_YY1(((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->f1=loc,((_tmpCD6->f2=Cyc_Absyn_false_conref,((_tmpCD6->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCD6)))))))));}
# 2152
break;}case 205: _LL305: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2153 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCD7;yyval=Cyc_YY1(((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->f1=loc,((_tmpCD7->f2=Cyc_Absyn_true_conref,((_tmpCD7->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpCD7)))))))));}
# 2156
break;}case 206: _LL306:
# 2158
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 207: _LL307: {
# 2161
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2159 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpCDA;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCD9;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=((_tmpCDA.tag=1,((_tmpCDA.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpCDA)))),_tmpCD9))))));}
break;}case 208: _LL308:
# 2162
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 209: _LL309: {
# 2165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2163 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;}case 210: _LL30A: {
# 2166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2164 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
break;}case 211: _LL30B:
# 2167
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 212: _LL30C: {
# 2170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2170 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 213: _LL30D: {
# 2173
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2171 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 214: _LL30E:
# 2174
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 215: _LL30F: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2176 "parse.y"
yyval=Cyc_YY23(Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),Cyc_yyget_YY23(yyyyvsp[1])));
break;}case 216: _LL310: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2181 "parse.y"
{struct _tuple26*_tmpCDB;yyval=Cyc_YY39(((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCDB->f2=0,((_tmpCDB->f3=0,((_tmpCDB->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpCDB->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpCDB)))))))))))));}
break;}case 217: _LL311: {
# 2184
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2186
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2183 "parse.y"
{struct _tuple26*_tmpCDC;yyval=Cyc_YY39(((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCDC->f2=1,((_tmpCDC->f3=0,((_tmpCDC->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCDC->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCDC)))))))))))));}
break;}case 218: _LL312: {
# 2186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2186 "parse.y"
struct _tuple8*_tmp680=Cyc_yyget_YY37(yyyyvsp[2]);struct _dyneither_ptr*_tmp682;struct Cyc_Absyn_Tqual _tmp683;void*_tmp684;struct _tuple8*_tmp681=_tmp680;_tmp682=_tmp681->f1;_tmp683=_tmp681->f2;_tmp684=_tmp681->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCDD;struct Cyc_Absyn_VarargInfo*_tmp685=(_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->name=_tmp682,((_tmpCDD->tq=_tmp683,((_tmpCDD->type=_tmp684,((_tmpCDD->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpCDD)))))))));
{struct _tuple26*_tmpCDE;yyval=Cyc_YY39(((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->f1=0,((_tmpCDE->f2=0,((_tmpCDE->f3=_tmp685,((_tmpCDE->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCDE->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCDE)))))))))))));}
# 2190
break;};}case 219: _LL313: {
# 2192
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2192 "parse.y"
struct _tuple8*_tmp688=Cyc_yyget_YY37(yyyyvsp[4]);struct _dyneither_ptr*_tmp68A;struct Cyc_Absyn_Tqual _tmp68B;void*_tmp68C;struct _tuple8*_tmp689=_tmp688;_tmp68A=_tmp689->f1;_tmp68B=_tmp689->f2;_tmp68C=_tmp689->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCDF;struct Cyc_Absyn_VarargInfo*_tmp68D=(_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->name=_tmp68A,((_tmpCDF->tq=_tmp68B,((_tmpCDF->type=_tmp68C,((_tmpCDF->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpCDF)))))))));
{struct _tuple26*_tmpCE0;yyval=Cyc_YY39(((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCE0->f2=0,((_tmpCE0->f3=_tmp68D,((_tmpCE0->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpCE0->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpCE0)))))))))))));}
# 2196
break;};}case 220: _LL314: {
# 2198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2200 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCE3;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCE2;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2[0]=((_tmpCE3.tag=1,((_tmpCE3.f1=0,_tmpCE3)))),_tmpCE2))))));}
break;}case 221: _LL315: {
# 2203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2201 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
break;}case 222: _LL316:
# 2204
 yyval=Cyc_YY49(0);
break;case 223: _LL317: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2205 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCE6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCE5;yyval=Cyc_YY49((void*)((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5[0]=((_tmpCE6.tag=24,((_tmpCE6.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCE6)))),_tmpCE5)))));}
break;}case 224: _LL318:
# 2208
 yyval=Cyc_YY50(0);
break;case 225: _LL319: {
# 2211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2210 "parse.y"
yyval=yyyyvsp[1];
break;}case 226: _LL31A: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2218 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCE9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCE8;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp694=(_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=((_tmpCE9.tag=2,((_tmpCE9.f1=0,((_tmpCE9.f2=& Cyc_Tcutil_trk,_tmpCE9)))))),_tmpCE8)));
struct _dyneither_ptr _tmp695=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp696=Cyc_Parse_id2type(_tmp695,(void*)_tmp694);
{struct _tuple29*_tmpCF3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCF1;struct Cyc_List_List*_tmpCF0;yyval=Cyc_YY50(((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->f1=(void*)((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=24,((_tmpCF2.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCF2)))),_tmpCF1)))),((_tmpCF3->f2=_tmp696,_tmpCF3)))))),((_tmpCF0->tl=0,_tmpCF0)))))));}
# 2223
break;}case 227: _LL31B: {
# 2225
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2227
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2225 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCF6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCF5;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp69D=(_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF6.tag=2,((_tmpCF6.f1=0,((_tmpCF6.f2=& Cyc_Tcutil_trk,_tmpCF6)))))),_tmpCF5)));
struct _dyneither_ptr _tmp69E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp69F=Cyc_Parse_id2type(_tmp69E,(void*)_tmp69D);
{struct _tuple29*_tmpD00;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCFF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCFE;struct Cyc_List_List*_tmpCFD;yyval=Cyc_YY50(((_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD->hd=((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->f1=(void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=24,((_tmpCFF.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCFF)))),_tmpCFE)))),((_tmpD00->f2=_tmp69F,_tmpD00)))))),((_tmpCFD->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCFD)))))));}
# 2230
break;}case 228: _LL31C:
# 2232
 yyval=Cyc_YY31(0);
break;case 229: _LL31D: {
# 2235
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2237
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2236 "parse.y"
{const char*_tmpD01;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpD01="inject",_tag_dyneither(_tmpD01,sizeof(char),7))))!= 0){
const char*_tmpD02;Cyc_Parse_err(((_tmpD02="missing type in function declaration",_tag_dyneither(_tmpD02,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY31(1);
# 2240
break;}case 230: _LL31E: {
# 2242
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2244
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2243 "parse.y"
yyval=yyyyvsp[0];
break;}case 231: _LL31F: {
# 2246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2244 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
break;}case 232: _LL320: {
# 2247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2248 "parse.y"
yyval=Cyc_YY40(0);
break;}case 233: _LL321: {
# 2251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2249 "parse.y"
yyval=yyyyvsp[1];
break;}case 234: _LL322: {
# 2252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2251 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD08;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD07;struct Cyc_List_List*_tmpD06;yyval=Cyc_YY40(((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->hd=(void*)((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08[0]=((_tmpD07.tag=25,((_tmpD07.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD07)))),_tmpD08)))),((_tmpD06->tl=0,_tmpD06)))))));}
break;}case 235: _LL323: {
# 2254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2253 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpD09;yyval=Cyc_YY40(((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD09->tl=0,_tmpD09)))))));}
# 2256
break;}case 236: _LL324: {
# 2258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2273 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD0F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD0E;struct Cyc_List_List*_tmpD0D;yyval=Cyc_YY40(((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->hd=(void*)((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F[0]=((_tmpD0E.tag=23,((_tmpD0E.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpD0E)))),_tmpD0F)))),((_tmpD0D->tl=0,_tmpD0D)))))));}
break;}case 237: _LL325: {
# 2276
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2278
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2275 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD15;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD14;struct Cyc_List_List*_tmpD13;yyval=Cyc_YY40(((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->hd=(void*)((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=((_tmpD14.tag=23,((_tmpD14.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpD14)))),_tmpD15)))),((_tmpD13->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD13)))))));}
break;}case 238: _LL326: {
# 2278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD16;yyval=Cyc_YY38(((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpD16->tl=0,_tmpD16)))))));}
break;}case 239: _LL327: {
# 2284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2283 "parse.y"
{struct Cyc_List_List*_tmpD17;yyval=Cyc_YY38(((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpD17->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpD17)))))));}
break;}case 240: _LL328: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp6B4=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6B6;struct Cyc_Parse_Type_specifier _tmp6B7;struct Cyc_List_List*_tmp6B8;struct _tuple25 _tmp6B5=_tmp6B4;_tmp6B6=_tmp6B5.f1;_tmp6B7=_tmp6B5.f2;_tmp6B8=_tmp6B5.f3;
if(_tmp6B6.loc == 0)_tmp6B6.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp6B9=Cyc_yyget_YY27(yyyyvsp[1]);struct _tuple0*_tmp6BB;struct Cyc_List_List*_tmp6BC;struct Cyc_Parse_Declarator _tmp6BA=_tmp6B9;_tmp6BB=_tmp6BA.id;_tmp6BC=_tmp6BA.tms;{
void*_tmp6BD=Cyc_Parse_speclist2typ(_tmp6B7,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp6BE=Cyc_Parse_apply_tms(_tmp6B6,_tmp6BD,_tmp6B8,_tmp6BC);struct Cyc_Absyn_Tqual _tmp6C0;void*_tmp6C1;struct Cyc_List_List*_tmp6C2;struct Cyc_List_List*_tmp6C3;struct _tuple13 _tmp6BF=_tmp6BE;_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;_tmp6C2=_tmp6BF.f3;_tmp6C3=_tmp6BF.f4;
if(_tmp6C2 != 0){
const char*_tmpD18;Cyc_Parse_err(((_tmpD18="parameter with bad type params",_tag_dyneither(_tmpD18,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6BB)){
const char*_tmpD19;Cyc_Parse_err(((_tmpD19="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpD19,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp6BB).f2;
if(_tmp6C3 != 0){
const char*_tmpD1C;void*_tmpD1B;(_tmpD1B=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD1C="extra attributes on parameter, ignoring",_tag_dyneither(_tmpD1C,sizeof(char),40))),_tag_dyneither(_tmpD1B,sizeof(void*),0)));}
{struct _tuple8*_tmpD1D;yyval=Cyc_YY37(((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->f1=idopt,((_tmpD1D->f2=_tmp6C0,((_tmpD1D->f3=_tmp6C1,_tmpD1D)))))))));}
# 2303
break;};};};}case 241: _LL329: {
# 2305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2304 "parse.y"
struct _tuple25 _tmp6C9=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6CB;struct Cyc_Parse_Type_specifier _tmp6CC;struct Cyc_List_List*_tmp6CD;struct _tuple25 _tmp6CA=_tmp6C9;_tmp6CB=_tmp6CA.f1;_tmp6CC=_tmp6CA.f2;_tmp6CD=_tmp6CA.f3;
if(_tmp6CB.loc == 0)_tmp6CB.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6CE=Cyc_Parse_speclist2typ(_tmp6CC,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6CD != 0){
const char*_tmpD20;void*_tmpD1F;(_tmpD1F=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD20="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD20,sizeof(char),38))),_tag_dyneither(_tmpD1F,sizeof(void*),0)));}
{struct _tuple8*_tmpD21;yyval=Cyc_YY37(((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->f1=0,((_tmpD21->f2=_tmp6CB,((_tmpD21->f3=_tmp6CE,_tmpD21)))))))));}
# 2311
break;};}case 242: _LL32A: {
# 2313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2312 "parse.y"
struct _tuple25 _tmp6D2=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6D4;struct Cyc_Parse_Type_specifier _tmp6D5;struct Cyc_List_List*_tmp6D6;struct _tuple25 _tmp6D3=_tmp6D2;_tmp6D4=_tmp6D3.f1;_tmp6D5=_tmp6D3.f2;_tmp6D6=_tmp6D3.f3;
if(_tmp6D4.loc == 0)_tmp6D4.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6D7=Cyc_Parse_speclist2typ(_tmp6D5,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6D8=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6D9=Cyc_Parse_apply_tms(_tmp6D4,_tmp6D7,_tmp6D6,_tmp6D8);struct Cyc_Absyn_Tqual _tmp6DB;void*_tmp6DC;struct Cyc_List_List*_tmp6DD;struct Cyc_List_List*_tmp6DE;struct _tuple13 _tmp6DA=_tmp6D9;_tmp6DB=_tmp6DA.f1;_tmp6DC=_tmp6DA.f2;_tmp6DD=_tmp6DA.f3;_tmp6DE=_tmp6DA.f4;
if(_tmp6DD != 0){
const char*_tmpD24;void*_tmpD23;(_tmpD23=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD24="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpD24,sizeof(char),49))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}
# 2320
if(_tmp6DE != 0){
const char*_tmpD27;void*_tmpD26;(_tmpD26=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD27="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD27,sizeof(char),38))),_tag_dyneither(_tmpD26,sizeof(void*),0)));}
{struct _tuple8*_tmpD28;yyval=Cyc_YY37(((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->f1=0,((_tmpD28->f2=_tmp6DB,((_tmpD28->f3=_tmp6DC,_tmpD28)))))))));}
# 2324
break;};}case 243: _LL32B: {
# 2326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2328 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
break;}case 244: _LL32C: {
# 2331
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2333
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2332 "parse.y"
{struct _dyneither_ptr*_tmpD2B;struct Cyc_List_List*_tmpD2A;yyval=Cyc_YY36(((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->hd=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD2B)))),((_tmpD2A->tl=0,_tmpD2A)))))));}
break;}case 245: _LL32D: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2334 "parse.y"
{struct _dyneither_ptr*_tmpD2E;struct Cyc_List_List*_tmpD2D;yyval=Cyc_YY36(((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->hd=((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD2E)))),((_tmpD2D->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpD2D)))))));}
break;}case 246: _LL32E: {
# 2337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2338 "parse.y"
yyval=yyyyvsp[0];
break;}case 247: _LL32F: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2339 "parse.y"
yyval=yyyyvsp[0];
break;}case 248: _LL330: {
# 2342
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2344 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD31;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD30;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((_tmpD31.tag=35,((_tmpD31.f1=0,((_tmpD31.f2=0,_tmpD31)))))),_tmpD30)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 249: _LL331: {
# 2347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2346 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD34;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD33;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=35,((_tmpD34.f1=0,((_tmpD34.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD34)))))),_tmpD33)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 250: _LL332: {
# 2349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2348 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD37;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD36;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD37.tag=35,((_tmpD37.f1=0,((_tmpD37.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD37)))))),_tmpD36)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 251: _LL333: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2350 "parse.y"
struct _dyneither_ptr*_tmpD3A;struct _tuple0*_tmpD39;struct Cyc_Absyn_Vardecl*_tmp6EE=Cyc_Absyn_new_vardecl(((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->f1=Cyc_Absyn_Loc_n,((_tmpD39->f2=((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD3A)))),_tmpD39)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2353
(_tmp6EE->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpD3D;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD3C;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3D.tag=26,((_tmpD3D.f1=_tmp6EE,((_tmpD3D.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD3D.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]),((_tmpD3D.f4=0,_tmpD3D)))))))))),_tmpD3C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2356
break;}case 252: _LL334: {
# 2358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2357 "parse.y"
void*_tmp6F3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpD40;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD3F;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD40.tag=27,((_tmpD40.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD40.f2=_tmp6F3,((_tmpD40.f3=0,_tmpD40)))))))),_tmpD3F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2360
break;}case 253: _LL335: {
# 2362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD43;struct Cyc_List_List*_tmpD42;yyval=Cyc_YY5(((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->hd=((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->f1=0,((_tmpD43->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]),_tmpD43)))))),((_tmpD42->tl=0,_tmpD42)))))));}
break;}case 254: _LL336: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2367 "parse.y"
{struct _tuple30*_tmpD46;struct Cyc_List_List*_tmpD45;yyval=Cyc_YY5(((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->hd=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpD46->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD46)))))),((_tmpD45->tl=0,_tmpD45)))))));}
break;}case 255: _LL337: {
# 2370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2369 "parse.y"
{struct _tuple30*_tmpD49;struct Cyc_List_List*_tmpD48;yyval=Cyc_YY5(((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->f1=0,((_tmpD49->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpD49)))))),((_tmpD48->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD48)))))));}
break;}case 256: _LL338: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2371 "parse.y"
{struct _tuple30*_tmpD4C;struct Cyc_List_List*_tmpD4B;yyval=Cyc_YY5(((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->hd=((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpD4C->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]),_tmpD4C)))))),((_tmpD4B->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD4B)))))));}
break;}case 257: _LL339: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2375 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
break;}case 258: _LL33A: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2376 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD56;struct _dyneither_ptr*_tmpD55;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD54;struct Cyc_List_List*_tmpD53;yyval=Cyc_YY41(((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->hd=(void*)((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56[0]=((_tmpD54.tag=1,((_tmpD54.f1=((_tmpD55=_cycalloc(sizeof(*_tmpD55)),((_tmpD55[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD55)))),_tmpD54)))),_tmpD56)))),((_tmpD53->tl=0,_tmpD53)))))));}
break;}case 259: _LL33B: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2381 "parse.y"
{struct Cyc_List_List*_tmpD57;yyval=Cyc_YY41(((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpD57->tl=0,_tmpD57)))))));}
break;}case 260: _LL33C: {
# 2384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2382 "parse.y"
{struct Cyc_List_List*_tmpD58;yyval=Cyc_YY41(((_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpD58->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpD58)))))));}
break;}case 261: _LL33D: {
# 2385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2386 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD5B;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD5A;yyval=Cyc_YY42((void*)((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A[0]=((_tmpD5B.tag=0,((_tmpD5B.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD5B)))),_tmpD5A)))));}
break;}case 262: _LL33E: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2387 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD61;struct _dyneither_ptr*_tmpD60;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD5F;yyval=Cyc_YY42((void*)((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD61.tag=1,((_tmpD61.f1=((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD60)))),_tmpD61)))),_tmpD5F)))));}
break;}case 263: _LL33F: {
# 2390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2392 "parse.y"
struct _tuple25 _tmp709=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp70B;struct Cyc_Parse_Type_specifier _tmp70C;struct Cyc_List_List*_tmp70D;struct _tuple25 _tmp70A=_tmp709;_tmp70B=_tmp70A.f1;_tmp70C=_tmp70A.f2;_tmp70D=_tmp70A.f3;{
void*_tmp70E=Cyc_Parse_speclist2typ(_tmp70C,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp70D != 0){
const char*_tmpD64;void*_tmpD63;(_tmpD63=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD64="ignoring attributes in type",_tag_dyneither(_tmpD64,sizeof(char),28))),_tag_dyneither(_tmpD63,sizeof(void*),0)));}
{struct _tuple8*_tmpD65;yyval=Cyc_YY37(((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65->f1=0,((_tmpD65->f2=_tmp70B,((_tmpD65->f3=_tmp70E,_tmpD65)))))))));}
# 2398
break;};}case 264: _LL340: {
# 2400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2399 "parse.y"
struct _tuple25 _tmp712=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp714;struct Cyc_Parse_Type_specifier _tmp715;struct Cyc_List_List*_tmp716;struct _tuple25 _tmp713=_tmp712;_tmp714=_tmp713.f1;_tmp715=_tmp713.f2;_tmp716=_tmp713.f3;{
void*_tmp717=Cyc_Parse_speclist2typ(_tmp715,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp718=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp719=Cyc_Parse_apply_tms(_tmp714,_tmp717,_tmp716,_tmp718);
if(_tmp719.f3 != 0){
# 2405
const char*_tmpD68;void*_tmpD67;(_tmpD67=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD68="bad type params, ignoring",_tag_dyneither(_tmpD68,sizeof(char),26))),_tag_dyneither(_tmpD67,sizeof(void*),0)));}
if(_tmp719.f4 != 0){
const char*_tmpD6B;void*_tmpD6A;(_tmpD6A=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD6B="bad specifiers, ignoring",_tag_dyneither(_tmpD6B,sizeof(char),25))),_tag_dyneither(_tmpD6A,sizeof(void*),0)));}
{struct _tuple8*_tmpD6C;yyval=Cyc_YY37(((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->f1=0,((_tmpD6C->f2=_tmp719.f1,((_tmpD6C->f3=_tmp719.f2,_tmpD6C)))))))));}
# 2410
break;};}case 265: _LL341: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2413 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 266: _LL342: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2414 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD6F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD6E;yyval=Cyc_YY44((void*)((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD6F.tag=24,((_tmpD6F.f1=0,_tmpD6F)))),_tmpD6E)))));}
break;}case 267: _LL343: {
# 2417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2415 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD72;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD71;yyval=Cyc_YY44((void*)((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD72.tag=24,((_tmpD72.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpD72)))),_tmpD71)))));}
break;}case 268: _LL344: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2416 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD75;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD74;yyval=Cyc_YY44((void*)((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74[0]=((_tmpD75.tag=25,((_tmpD75.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD75)))),_tmpD74)))));}
break;}case 269: _LL345: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2417 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD7B;struct Cyc_List_List*_tmpD7A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD79;yyval=Cyc_YY44((void*)((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79[0]=((_tmpD7B.tag=24,((_tmpD7B.f1=((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD7A->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD7A)))))),_tmpD7B)))),_tmpD79)))));}
break;}case 270: _LL346: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD7C;yyval=Cyc_YY40(((_tmpD7C=_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD7C->tl=0,_tmpD7C)))))));}
break;}case 271: _LL347: {
# 2426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2424 "parse.y"
{struct Cyc_List_List*_tmpD7D;yyval=Cyc_YY40(((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD7D->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD7D)))))));}
break;}case 272: _LL348: {
# 2427
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2429 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD7E;yyval=Cyc_YY30(((_tmpD7E.tms=Cyc_yyget_YY26(yyyyvsp[0]),_tmpD7E)));}
break;}case 273: _LL349: {
# 2432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2431 "parse.y"
yyval=yyyyvsp[0];
break;}case 274: _LL34A: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2433 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD7F;yyval=Cyc_YY30(((_tmpD7F.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpD7F)));}
break;}case 275: _LL34B: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2438 "parse.y"
yyval=yyyyvsp[1];
break;}case 276: _LL34C: {
# 2441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2440 "parse.y"
{struct Cyc_List_List*_tmpD89;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD88;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD87;struct Cyc_Parse_Abstractdeclarator _tmpD86;yyval=Cyc_YY30(((_tmpD86.tms=((_tmpD89=_region_malloc(yyr,sizeof(*_tmpD89)),((_tmpD89->hd=(void*)((_tmpD87=_region_malloc(yyr,sizeof(*_tmpD87)),((_tmpD87[0]=((_tmpD88.tag=0,((_tmpD88.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD88.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD88)))))),_tmpD87)))),((_tmpD89->tl=0,_tmpD89)))))),_tmpD86)));}
break;}case 277: _LL34D: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2442 "parse.y"
{struct Cyc_List_List*_tmpD93;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD92;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD91;struct Cyc_Parse_Abstractdeclarator _tmpD90;yyval=Cyc_YY30(((_tmpD90.tms=((_tmpD93=_region_malloc(yyr,sizeof(*_tmpD93)),((_tmpD93->hd=(void*)((_tmpD91=_region_malloc(yyr,sizeof(*_tmpD91)),((_tmpD91[0]=((_tmpD92.tag=0,((_tmpD92.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD92.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD92)))))),_tmpD91)))),((_tmpD93->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD93)))))),_tmpD90)));}
break;}case 278: _LL34E: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2444 "parse.y"
{struct Cyc_List_List*_tmpD9D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD9C;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD9B;struct Cyc_Parse_Abstractdeclarator _tmpD9A;yyval=Cyc_YY30(((_tmpD9A.tms=((_tmpD9D=_region_malloc(yyr,sizeof(*_tmpD9D)),((_tmpD9D->hd=(void*)((_tmpD9B=_region_malloc(yyr,sizeof(*_tmpD9B)),((_tmpD9B[0]=((_tmpD9C.tag=1,((_tmpD9C.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpD9C.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD9C.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD9C)))))))),_tmpD9B)))),((_tmpD9D->tl=0,_tmpD9D)))))),_tmpD9A)));}
break;}case 279: _LL34F: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2446 "parse.y"
{struct Cyc_List_List*_tmpDA7;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpDA6;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpDA5;struct Cyc_Parse_Abstractdeclarator _tmpDA4;yyval=Cyc_YY30(((_tmpDA4.tms=((_tmpDA7=_region_malloc(yyr,sizeof(*_tmpDA7)),((_tmpDA7->hd=(void*)((_tmpDA5=_region_malloc(yyr,sizeof(*_tmpDA5)),((_tmpDA5[0]=((_tmpDA6.tag=1,((_tmpDA6.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpDA6.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpDA6.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpDA6)))))))),_tmpDA5)))),((_tmpDA7->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDA7)))))),_tmpDA4)));}
# 2449
break;}case 280: _LL350: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2450 "parse.y"
{struct Cyc_List_List*_tmpDBC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDBB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDBA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDB9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDB8;struct Cyc_Parse_Abstractdeclarator _tmpDB7;yyval=Cyc_YY30(((_tmpDB7.tms=((_tmpDBC=_region_malloc(yyr,sizeof(*_tmpDBC)),((_tmpDBC->hd=(void*)((_tmpDB8=_region_malloc(yyr,sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDBB.tag=3,((_tmpDBB.f1=(void*)((_tmpDB9=_region_malloc(yyr,sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=1,((_tmpDBA.f1=0,((_tmpDBA.f2=0,((_tmpDBA.f3=0,((_tmpDBA.f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpDBA.f5=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpDBA.f6=Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDBA.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDBA)))))))))))))))),_tmpDB9)))),_tmpDBB)))),_tmpDB8)))),((_tmpDBC->tl=0,_tmpDBC)))))),_tmpDB7)));}
# 2452
break;}case 281: _LL351: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2453 "parse.y"
struct _tuple26*_tmp742=Cyc_yyget_YY39(yyyyvsp[1]);struct Cyc_List_List*_tmp744;int _tmp745;struct Cyc_Absyn_VarargInfo*_tmp746;void*_tmp747;struct Cyc_List_List*_tmp748;struct _tuple26*_tmp743=_tmp742;_tmp744=_tmp743->f1;_tmp745=_tmp743->f2;_tmp746=_tmp743->f3;_tmp747=_tmp743->f4;_tmp748=_tmp743->f5;
{struct Cyc_List_List*_tmpDD1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDD0;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDCF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDCE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDCD;struct Cyc_Parse_Abstractdeclarator _tmpDCC;yyval=Cyc_YY30(((_tmpDCC.tms=((_tmpDD1=_region_malloc(yyr,sizeof(*_tmpDD1)),((_tmpDD1->hd=(void*)((_tmpDCD=_region_malloc(yyr,sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDD0.tag=3,((_tmpDD0.f1=(void*)((_tmpDCE=_region_malloc(yyr,sizeof(*_tmpDCE)),((_tmpDCE[0]=((_tmpDCF.tag=1,((_tmpDCF.f1=_tmp744,((_tmpDCF.f2=_tmp745,((_tmpDCF.f3=_tmp746,((_tmpDCF.f4=_tmp747,((_tmpDCF.f5=_tmp748,((_tmpDCF.f6=Cyc_yyget_YY55(yyyyvsp[3]),((_tmpDCF.f7=Cyc_yyget_YY55(yyyyvsp[4]),_tmpDCF)))))))))))))))),_tmpDCE)))),_tmpDD0)))),_tmpDCD)))),((_tmpDD1->tl=0,_tmpDD1)))))),_tmpDCC)));}
# 2456
break;}case 282: _LL352: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2457 "parse.y"
{struct Cyc_List_List*_tmpDE6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDE5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDE4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDE3;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDE2;struct Cyc_Parse_Abstractdeclarator _tmpDE1;yyval=Cyc_YY30(((_tmpDE1.tms=((_tmpDE6=_region_malloc(yyr,sizeof(*_tmpDE6)),((_tmpDE6->hd=(void*)((_tmpDE2=_region_malloc(yyr,sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE5.tag=3,((_tmpDE5.f1=(void*)((_tmpDE3=_region_malloc(yyr,sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE4.tag=1,((_tmpDE4.f1=0,((_tmpDE4.f2=0,((_tmpDE4.f3=0,((_tmpDE4.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpDE4.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpDE4.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpDE4.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpDE4)))))))))))))))),_tmpDE3)))),_tmpDE5)))),_tmpDE2)))),((_tmpDE6->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDE6)))))),_tmpDE1)));}
# 2460
break;}case 283: _LL353: {
# 2462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2461 "parse.y"
struct _tuple26*_tmp755=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp757;int _tmp758;struct Cyc_Absyn_VarargInfo*_tmp759;void*_tmp75A;struct Cyc_List_List*_tmp75B;struct _tuple26*_tmp756=_tmp755;_tmp757=_tmp756->f1;_tmp758=_tmp756->f2;_tmp759=_tmp756->f3;_tmp75A=_tmp756->f4;_tmp75B=_tmp756->f5;
{struct Cyc_List_List*_tmpDFB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDFA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDF9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDF8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDF7;struct Cyc_Parse_Abstractdeclarator _tmpDF6;yyval=Cyc_YY30(((_tmpDF6.tms=((_tmpDFB=_region_malloc(yyr,sizeof(*_tmpDFB)),((_tmpDFB->hd=(void*)((_tmpDF7=_region_malloc(yyr,sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDFA.tag=3,((_tmpDFA.f1=(void*)((_tmpDF8=_region_malloc(yyr,sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDF9.tag=1,((_tmpDF9.f1=_tmp757,((_tmpDF9.f2=_tmp758,((_tmpDF9.f3=_tmp759,((_tmpDF9.f4=_tmp75A,((_tmpDF9.f5=_tmp75B,((_tmpDF9.f6=
Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDF9.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDF9)))))))))))))))),_tmpDF8)))),_tmpDFA)))),_tmpDF7)))),((_tmpDFB->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDFB)))))),_tmpDF6)));}
# 2465
break;}case 284: _LL354: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2467 "parse.y"
struct Cyc_List_List*_tmp762=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpE05;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpE04;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpE03;struct Cyc_Parse_Abstractdeclarator _tmpE02;yyval=Cyc_YY30(((_tmpE02.tms=((_tmpE05=_region_malloc(yyr,sizeof(*_tmpE05)),((_tmpE05->hd=(void*)((_tmpE03=_region_malloc(yyr,sizeof(*_tmpE03)),((_tmpE03[0]=((_tmpE04.tag=4,((_tmpE04.f1=_tmp762,((_tmpE04.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpE04.f3=0,_tmpE04)))))))),_tmpE03)))),((_tmpE05->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpE05)))))),_tmpE02)));}
# 2471
break;}case 285: _LL355: {
# 2473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2472 "parse.y"
{struct Cyc_List_List*_tmpE0F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpE0E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpE0D;struct Cyc_Parse_Abstractdeclarator _tmpE0C;yyval=Cyc_YY30(((_tmpE0C.tms=((_tmpE0F=_region_malloc(yyr,sizeof(*_tmpE0F)),((_tmpE0F->hd=(void*)((_tmpE0D=_region_malloc(yyr,sizeof(*_tmpE0D)),((_tmpE0D[0]=((_tmpE0E.tag=5,((_tmpE0E.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpE0E.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpE0E)))))),_tmpE0D)))),((_tmpE0F->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpE0F)))))),_tmpE0C)));}
# 2474
break;}case 286: _LL356: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2478 "parse.y"
yyval=yyyyvsp[0];
break;}case 287: _LL357: {
# 2481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2479 "parse.y"
yyval=yyyyvsp[0];
break;}case 288: _LL358: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2480 "parse.y"
yyval=yyyyvsp[0];
break;}case 289: _LL359: {
# 2483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2481 "parse.y"
yyval=yyyyvsp[0];
break;}case 290: _LL35A: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2482 "parse.y"
yyval=yyyyvsp[0];
break;}case 291: _LL35B: {
# 2485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2483 "parse.y"
yyval=yyyyvsp[0];
break;}case 292: _LL35C: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2486 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpE12;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpE11;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE12.tag=16,((_tmpE12.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpE12)))),_tmpE11)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 293: _LL35D: {
# 2489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE18;struct _dyneither_ptr*_tmpE17;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE16;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16[0]=((_tmpE18.tag=13,((_tmpE18.f1=((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE17)))),((_tmpE18.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]),_tmpE18)))))),_tmpE16)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 294: _LL35E: {
# 2495
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2497
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2496 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL35F: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2497 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 296: _LL360: {
# 2500
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2502 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 297: _LL361: {
# 2505
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2507
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2503 "parse.y"
yyval=yyyyvsp[1];
break;}case 298: _LL362: {
# 2506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2508 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 299: _LL363: {
# 2511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2509 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
break;}case 300: _LL364: {
# 2512
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2514
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2510 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE1E;struct _dyneither_ptr*_tmpE1D;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE1C;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1E.tag=13,((_tmpE1E.f1=((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE1D)))),((_tmpE1E.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpE1E)))))),_tmpE1C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2512
break;}case 301: _LL365: {
# 2514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2512 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE24;struct _dyneither_ptr*_tmpE23;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE22;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22[0]=((_tmpE24.tag=13,((_tmpE24.f1=((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE23)))),((_tmpE24.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3])),_tmpE24)))))),_tmpE22)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2514
break;}case 302: _LL366: {
# 2516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2514 "parse.y"
yyval=yyyyvsp[0];
break;}case 303: _LL367: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2515 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL368: {
# 2518
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2516 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE27;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE26;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26[0]=((_tmpE27.tag=1,((_tmpE27.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE27)))),_tmpE26)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 305: _LL369: {
# 2520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2519 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE2A;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE29;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29[0]=((_tmpE2A.tag=1,((_tmpE2A.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE2A)))),_tmpE29)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));}
break;}case 306: _LL36A: {
# 2522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2524 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL36B: {
# 2527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2526 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 308: _LL36C: {
# 2529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 309: _LL36D: {
# 2535
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2537
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2535 "parse.y"
struct Cyc_Absyn_Exp*_tmp77A=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp77A,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 310: _LL36E: {
# 2539
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2539 "parse.y"
struct Cyc_Absyn_Exp*_tmp77B=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp77B,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2542
break;}case 311: _LL36F: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2545 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 312: _LL370:
# 2548
 yyval=Cyc_YY8(0);
break;case 313: _LL371: {
# 2551
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2563 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE2D;struct Cyc_List_List*_tmpE2C;yyval=Cyc_YY8(((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->hd=((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpE2D->pat_vars=0,((_tmpE2D->where_clause=0,((_tmpE2D->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]),((_tmpE2D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE2D)))))))))))),((_tmpE2C->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpE2C)))))));}
break;}case 314: _LL372: {
# 2568
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2567 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE30;struct Cyc_List_List*_tmpE2F;yyval=Cyc_YY8(((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->hd=((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE30->pat_vars=0,((_tmpE30->where_clause=0,((_tmpE30->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpE30->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE30)))))))))))),((_tmpE2F->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpE2F)))))));}
break;}case 315: _LL373: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2571 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE33;struct Cyc_List_List*_tmpE32;yyval=Cyc_YY8(((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32->hd=((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE33->pat_vars=0,((_tmpE33->where_clause=0,((_tmpE33->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]),((_tmpE33->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE33)))))))))))),((_tmpE32->tl=Cyc_yyget_YY8(yyyyvsp[4]),_tmpE32)))))));}
break;}case 316: _LL374: {
# 2574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2573 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE36;struct Cyc_List_List*_tmpE35;yyval=Cyc_YY8(((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35->hd=((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE36->pat_vars=0,((_tmpE36->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE36->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpE36->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE36)))))))))))),((_tmpE35->tl=
Cyc_yyget_YY8(yyyyvsp[5]),_tmpE35)))))));}
break;}case 317: _LL375: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2577 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE39;struct Cyc_List_List*_tmpE38;yyval=Cyc_YY8(((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->hd=((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE39->pat_vars=0,((_tmpE39->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE39->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]),((_tmpE39->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE39)))))))))))),((_tmpE38->tl=Cyc_yyget_YY8(yyyyvsp[6]),_tmpE38)))))));}
break;}case 318: _LL376: {
# 2580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2584 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL377: {
# 2587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2588 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL378: {
# 2591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2592 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL379: {
# 2596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2595 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL37A: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2598 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL37B: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2601 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL37C: {
# 2605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2604 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL37D: {
# 2608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2607 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL37E: {
# 2611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2610 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL37F: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2613 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 328: _LL380: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2616 "parse.y"
struct Cyc_List_List*_tmp786=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp787=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp786,_tmp787));
# 2621
break;}case 329: _LL381: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2622 "parse.y"
struct Cyc_List_List*_tmp788=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp789=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp788,_tmp789));
# 2627
break;}case 330: _LL382: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2628 "parse.y"
struct Cyc_List_List*_tmp78A=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp78B=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp78A,_tmp78B));
# 2633
break;}case 331: _LL383: {
# 2635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2634 "parse.y"
struct Cyc_List_List*_tmp78C=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp78D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp78C,_tmp78D));
# 2639
break;}case 332: _LL384: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpE3A;yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE3A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 333: _LL385: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2645 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL386: {
# 2648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2646 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL387: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2647 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL388: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2648 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL389: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2650 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 338: _LL38A: {
# 2653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2651 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 339: _LL38B: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2653 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 340: _LL38C: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2662 "parse.y"
yyval=yyyyvsp[0];
break;}case 341: _LL38D: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2665 "parse.y"
yyval=yyyyvsp[0];
break;}case 342: _LL38E: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2667 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 343: _LL38F: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2670 "parse.y"
yyval=yyyyvsp[0];
break;}case 344: _LL390: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2672 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 345: _LL391: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2675 "parse.y"
yyval=yyyyvsp[0];
break;}case 346: _LL392: {
# 2678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2677 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL393: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2680 "parse.y"
yyval=yyyyvsp[0];
break;}case 348: _LL394: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2682 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL395: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2685 "parse.y"
yyval=yyyyvsp[0];
break;}case 350: _LL396: {
# 2688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2687 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL397: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2690 "parse.y"
yyval=yyyyvsp[0];
break;}case 352: _LL398: {
# 2693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2692 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL399: {
# 2695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2695 "parse.y"
yyval=yyyyvsp[0];
break;}case 354: _LL39A: {
# 2698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2697 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL39B: {
# 2700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2699 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL39C: {
# 2702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2702 "parse.y"
yyval=yyyyvsp[0];
break;}case 357: _LL39D: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2704 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL39E: {
# 2707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2706 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL39F: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2708 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL3A0: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2710 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL3A1: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2713 "parse.y"
yyval=yyyyvsp[0];
break;}case 362: _LL3A2: {
# 2716
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2718
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2715 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL3A3: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2717 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL3A4: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2720 "parse.y"
yyval=yyyyvsp[0];
break;}case 365: _LL3A5: {
# 2723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2722 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL3A6: {
# 2725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2724 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL3A7: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2727 "parse.y"
yyval=yyyyvsp[0];
break;}case 368: _LL3A8: {
# 2730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2729 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL3A9: {
# 2732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2731 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 370: _LL3AA: {
# 2734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2733 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 371: _LL3AB: {
# 2736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2736 "parse.y"
yyval=yyyyvsp[0];
break;}case 372: _LL3AC: {
# 2739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2738 "parse.y"
void*_tmp78F=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp78F,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2741
break;}case 373: _LL3AD: {
# 2743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2744 "parse.y"
yyval=yyyyvsp[0];
break;}case 374: _LL3AE: {
# 2747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2747 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 375: _LL3AF: {
# 2750
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2749 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 376: _LL3B0: {
# 2752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2751 "parse.y"
void*_tmp790=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp790,(unsigned int)(yyyylsp[0]).first_line)));
# 2754
break;}case 377: _LL3B1: {
# 2756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2755 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 378: _LL3B2: {
# 2758
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2757 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 379: _LL3B3: {
# 2760
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2762 "parse.y"
yyval=yyyyvsp[0];
break;}case 380: _LL3B4: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2771 "parse.y"
yyval=yyyyvsp[0];
break;}case 381: _LL3B5: {
# 2774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2776 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 382: _LL3B6: {
# 2779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2778 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 383: _LL3B7: {
# 2781
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2783
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2780 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp791=e->r;void*_tmp792=_tmp791;enum Cyc_Absyn_Sign _tmp794;char _tmp795;enum Cyc_Absyn_Sign _tmp797;short _tmp798;enum Cyc_Absyn_Sign _tmp79A;int _tmp79B;struct _dyneither_ptr _tmp79D;int _tmp79E;_LL3BA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp793=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp793->tag != 0)goto _LL3BC;else{if(((_tmp793->f1).Char_c).tag != 2)goto _LL3BC;_tmp794=((struct _tuple3)((_tmp793->f1).Char_c).val).f1;_tmp795=((struct _tuple3)((_tmp793->f1).Char_c).val).f2;}}_LL3BB:
# 2784
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE3D;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpE3C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE3C=_cycalloc_atomic(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3D.tag=11,((_tmpE3D.f1=_tmp795,_tmpE3D)))),_tmpE3C)))),e->loc));}goto _LL3B9;_LL3BC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp796=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp796->tag != 0)goto _LL3BE;else{if(((_tmp796->f1).Short_c).tag != 4)goto _LL3BE;_tmp797=((struct _tuple4)((_tmp796->f1).Short_c).val).f1;_tmp798=((struct _tuple4)((_tmp796->f1).Short_c).val).f2;}}_LL3BD:
# 2786
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE40;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE3F;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE3F=_cycalloc_atomic(sizeof(*_tmpE3F)),((_tmpE3F[0]=((_tmpE40.tag=10,((_tmpE40.f1=_tmp797,((_tmpE40.f2=(int)_tmp798,_tmpE40)))))),_tmpE3F)))),e->loc));}goto _LL3B9;_LL3BE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp799=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp799->tag != 0)goto _LL3C0;else{if(((_tmp799->f1).Int_c).tag != 5)goto _LL3C0;_tmp79A=((struct _tuple5)((_tmp799->f1).Int_c).val).f1;_tmp79B=((struct _tuple5)((_tmp799->f1).Int_c).val).f2;}}_LL3BF:
# 2788
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE43;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE42;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE42=_cycalloc_atomic(sizeof(*_tmpE42)),((_tmpE42[0]=((_tmpE43.tag=10,((_tmpE43.f1=_tmp79A,((_tmpE43.f2=_tmp79B,_tmpE43)))))),_tmpE42)))),e->loc));}goto _LL3B9;_LL3C0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp79C->tag != 0)goto _LL3C2;else{if(((_tmp79C->f1).Float_c).tag != 7)goto _LL3C2;_tmp79D=((struct _tuple7)((_tmp79C->f1).Float_c).val).f1;_tmp79E=((struct _tuple7)((_tmp79C->f1).Float_c).val).f2;}}_LL3C1:
# 2790
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE46;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpE45;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45[0]=((_tmpE46.tag=12,((_tmpE46.f1=_tmp79D,((_tmpE46.f2=_tmp79E,_tmpE46)))))),_tmpE45)))),e->loc));}goto _LL3B9;_LL3C2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp79F->tag != 0)goto _LL3C4;else{if(((_tmp79F->f1).Null_c).tag != 1)goto _LL3C4;}}_LL3C3:
# 2792
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3B9;_LL3C4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp7A0->tag != 0)goto _LL3C6;else{if(((_tmp7A0->f1).String_c).tag != 8)goto _LL3C6;}}_LL3C5:
# 2794
{const char*_tmpE47;Cyc_Parse_err(((_tmpE47="strings cannot occur within patterns",_tag_dyneither(_tmpE47,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B9;_LL3C6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp7A1->tag != 0)goto _LL3C8;else{if(((_tmp7A1->f1).Wstring_c).tag != 9)goto _LL3C8;}}_LL3C7:
# 2796
{const char*_tmpE48;Cyc_Parse_err(((_tmpE48="strings cannot occur within patterns",_tag_dyneither(_tmpE48,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B9;_LL3C8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp792;if(_tmp7A2->tag != 0)goto _LL3CA;else{if(((_tmp7A2->f1).LongLong_c).tag != 6)goto _LL3CA;}}_LL3C9:
# 2798
{const char*_tmpE49;Cyc_Parse_unimp(((_tmpE49="long long's in patterns",_tag_dyneither(_tmpE49,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B9;_LL3CA:;_LL3CB: {
# 2800
const char*_tmpE4A;Cyc_Parse_err(((_tmpE4A="bad constant in case",_tag_dyneither(_tmpE4A,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3B9:;}
# 2803
break;}case 384: _LL3B8: {
# 2805
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2804 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE4D;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE4C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4D.tag=15,((_tmpE4D.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE4D)))),_tmpE4C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 385: _LL3CC: {
# 2807
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2809
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2806 "parse.y"
{const char*_tmpE4E;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE4E="as",_tag_dyneither(_tmpE4E,sizeof(char),3))))!= 0){
const char*_tmpE4F;Cyc_Parse_err(((_tmpE4F="expecting `as'",_tag_dyneither(_tmpE4F,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE59;struct _dyneither_ptr*_tmpE58;struct _tuple0*_tmpE57;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE56;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56[0]=((_tmpE59.tag=1,((_tmpE59.f1=Cyc_Absyn_new_vardecl(((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57->f1=Cyc_Absyn_Loc_n,((_tmpE57->f2=((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE58)))),_tmpE57)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE59.f2=
Cyc_yyget_YY9(yyyyvsp[2]),_tmpE59)))))),_tmpE56)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2811
break;}case 386: _LL3CD: {
# 2813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2812 "parse.y"
{const char*_tmpE5A;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE5A="alias",_tag_dyneither(_tmpE5A,sizeof(char),6))))!= 0){
const char*_tmpE5B;Cyc_Parse_err(((_tmpE5B="expecting `alias'",_tag_dyneither(_tmpE5B,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp7B9=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE63;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE62;struct _dyneither_ptr*_tmpE61;struct Cyc_Absyn_Tvar*_tmpE60;struct Cyc_Absyn_Tvar*tv=(_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60->name=((_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE61)))),((_tmpE60->identity=- 1,((_tmpE60->kind=(void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE62.tag=0,((_tmpE62.f1=& Cyc_Tcutil_rk,_tmpE62)))),_tmpE63)))),_tmpE60)))))));
struct _dyneither_ptr*_tmpE66;struct _tuple0*_tmpE65;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65->f1=Cyc_Absyn_Loc_n,((_tmpE65->f2=((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE66)))),_tmpE65)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE69;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE68;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE69.tag=2,((_tmpE69.f1=tv,((_tmpE69.f2=vd,_tmpE69)))))),_tmpE68)))),(unsigned int)_tmp7B9));}
# 2820
break;};}case 387: _LL3CE: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2821 "parse.y"
struct _tuple22 _tmp7C2=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7C4;int _tmp7C5;struct _tuple22 _tmp7C3=_tmp7C2;_tmp7C4=_tmp7C3.f1;_tmp7C5=_tmp7C3.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE6C;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE6B;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=((_tmpE6C.tag=5,((_tmpE6C.f1=_tmp7C4,((_tmpE6C.f2=_tmp7C5,_tmpE6C)))))),_tmpE6B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2824
break;}case 388: _LL3CF: {
# 2826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2825 "parse.y"
struct _tuple22 _tmp7C8=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7CA;int _tmp7CB;struct _tuple22 _tmp7C9=_tmp7C8;_tmp7CA=_tmp7C9.f1;_tmp7CB=_tmp7C9.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE6F;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE6E;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=((_tmpE6F.tag=16,((_tmpE6F.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE6F.f2=_tmp7CA,((_tmpE6F.f3=_tmp7CB,_tmpE6F)))))))),_tmpE6E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2828
break;}case 389: _LL3D0: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2829 "parse.y"
struct _tuple22 _tmp7CE=*Cyc_yyget_YY14(yyyyvsp[3]);struct Cyc_List_List*_tmp7D0;int _tmp7D1;struct _tuple22 _tmp7CF=_tmp7CE;_tmp7D0=_tmp7CF.f1;_tmp7D1=_tmp7CF.f2;{
struct Cyc_List_List*_tmp7D2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE75;struct Cyc_Absyn_AggrInfo*_tmpE74;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE73;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73[0]=((_tmpE75.tag=7,((_tmpE75.f1=((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE74->targs=0,_tmpE74)))))),((_tmpE75.f2=_tmp7D2,((_tmpE75.f3=_tmp7D0,((_tmpE75.f4=_tmp7D1,_tmpE75)))))))))),_tmpE73)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2834
break;};}case 390: _LL3D1: {
# 2836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2835 "parse.y"
struct _tuple22 _tmp7D6=*Cyc_yyget_YY14(yyyyvsp[2]);struct Cyc_List_List*_tmp7D8;int _tmp7D9;struct _tuple22 _tmp7D7=_tmp7D6;_tmp7D8=_tmp7D7.f1;_tmp7D9=_tmp7D7.f2;{
struct Cyc_List_List*_tmp7DA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE78;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE77;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77[0]=((_tmpE78.tag=7,((_tmpE78.f1=0,((_tmpE78.f2=_tmp7DA,((_tmpE78.f3=_tmp7D8,((_tmpE78.f4=_tmp7D9,_tmpE78)))))))))),_tmpE77)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2839
break;};}case 391: _LL3D2: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2840 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE7B;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE7A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=((_tmpE7B.tag=6,((_tmpE7B.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE7B)))),_tmpE7A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 392: _LL3D3: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2842 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE85;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE84;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE83;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE82;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=((_tmpE85.tag=6,((_tmpE85.f1=Cyc_Absyn_new_pat((void*)((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=((_tmpE84.tag=6,((_tmpE84.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE84)))),_tmpE83)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE85)))),_tmpE82)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 393: _LL3D4: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2844 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE8F;struct _dyneither_ptr*_tmpE8E;struct _tuple0*_tmpE8D;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE8C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C[0]=((_tmpE8F.tag=3,((_tmpE8F.f1=Cyc_Absyn_new_vardecl(((_tmpE8D=_cycalloc(sizeof(*_tmpE8D)),((_tmpE8D->f1=Cyc_Absyn_Loc_n,((_tmpE8D->f2=((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE8E)))),_tmpE8D)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE8F.f2=
# 2846
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE8F)))))),_tmpE8C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2848
break;}case 394: _LL3D5: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2849 "parse.y"
{const char*_tmpE90;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE90="as",_tag_dyneither(_tmpE90,sizeof(char),3))))!= 0){
const char*_tmpE91;Cyc_Parse_err(((_tmpE91="expecting `as'",_tag_dyneither(_tmpE91,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE9B;struct _dyneither_ptr*_tmpE9A;struct _tuple0*_tmpE99;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE98;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98[0]=((_tmpE9B.tag=3,((_tmpE9B.f1=Cyc_Absyn_new_vardecl(((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99->f1=Cyc_Absyn_Loc_n,((_tmpE99->f2=((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE9A)))),_tmpE99)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE9B.f2=
# 2853
Cyc_yyget_YY9(yyyyvsp[3]),_tmpE9B)))))),_tmpE98)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2855
break;}case 395: _LL3D6: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2856 "parse.y"
void*_tmp7ED=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEAC;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEAB;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEAA;struct _dyneither_ptr*_tmpEA9;struct _tuple0*_tmpEA8;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpEA7;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7[0]=((_tmpEAC.tag=4,((_tmpEAC.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7ED),((_tmpEAC.f2=
Cyc_Absyn_new_vardecl(((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->f1=Cyc_Absyn_Loc_n,((_tmpEA8->f2=((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEA9)))),_tmpEA8)))))),(void*)(
(_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=((_tmpEAB.tag=19,((_tmpEAB.f1=_tmp7ED,_tmpEAB)))),_tmpEAA)))),0),_tmpEAC)))))),_tmpEA7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2861
break;}case 396: _LL3D7: {
# 2863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2862 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7F4=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEC8;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEC7;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEC6;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEC5;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEC4;struct _dyneither_ptr*_tmpEC3;struct _tuple0*_tmpEC2;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpEC1;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1[0]=((_tmpEC8.tag=4,((_tmpEC8.f1=_tmp7F4,((_tmpEC8.f2=
Cyc_Absyn_new_vardecl(((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2->f1=Cyc_Absyn_Loc_n,((_tmpEC2->f2=((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEC3)))),_tmpEC2)))))),(void*)(
(_tmpEC4=_cycalloc(sizeof(*_tmpEC4)),((_tmpEC4[0]=((_tmpEC7.tag=19,((_tmpEC7.f1=(void*)((_tmpEC5=_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC6.tag=2,((_tmpEC6.f1=_tmp7F4,_tmpEC6)))),_tmpEC5)))),_tmpEC7)))),_tmpEC4)))),0),_tmpEC8)))))),_tmpEC1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2867
break;}case 397: _LL3D8: {
# 2869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2870 "parse.y"
{struct _tuple22*_tmpEC9;yyval=Cyc_YY10(((_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpEC9->f2=0,_tmpEC9)))))));}
break;}case 398: _LL3D9: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2871 "parse.y"
{struct _tuple22*_tmpECA;yyval=Cyc_YY10(((_tmpECA=_cycalloc(sizeof(*_tmpECA)),((_tmpECA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpECA->f2=1,_tmpECA)))))));}
break;}case 399: _LL3DA: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2872 "parse.y"
{struct _tuple22*_tmpECB;yyval=Cyc_YY10(((_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB->f1=0,((_tmpECB->f2=1,_tmpECB)))))));}
break;}case 400: _LL3DB: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2877 "parse.y"
{struct Cyc_List_List*_tmpECC;yyval=Cyc_YY11(((_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC->hd=Cyc_yyget_YY9(yyyyvsp[0]),((_tmpECC->tl=0,_tmpECC)))))));}
break;}case 401: _LL3DC: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2879 "parse.y"
{struct Cyc_List_List*_tmpECD;yyval=Cyc_YY11(((_tmpECD=_cycalloc(sizeof(*_tmpECD)),((_tmpECD->hd=Cyc_yyget_YY9(yyyyvsp[2]),((_tmpECD->tl=Cyc_yyget_YY11(yyyyvsp[0]),_tmpECD)))))));}
break;}case 402: _LL3DD: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2884 "parse.y"
{struct _tuple23*_tmpECE;yyval=Cyc_YY12(((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE->f1=0,((_tmpECE->f2=Cyc_yyget_YY9(yyyyvsp[0]),_tmpECE)))))));}
break;}case 403: _LL3DE: {
# 2887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2886 "parse.y"
{struct _tuple23*_tmpECF;yyval=Cyc_YY12(((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpECF->f2=Cyc_yyget_YY9(yyyyvsp[1]),_tmpECF)))))));}
break;}case 404: _LL3DF: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2889 "parse.y"
{struct _tuple22*_tmpED0;yyval=Cyc_YY14(((_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpED0->f2=0,_tmpED0)))))));}
break;}case 405: _LL3E0: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2890 "parse.y"
{struct _tuple22*_tmpED1;yyval=Cyc_YY14(((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpED1->f2=1,_tmpED1)))))));}
break;}case 406: _LL3E1: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2891 "parse.y"
{struct _tuple22*_tmpED2;yyval=Cyc_YY14(((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2->f1=0,((_tmpED2->f2=1,_tmpED2)))))));}
break;}case 407: _LL3E2: {
# 2894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2896 "parse.y"
{struct Cyc_List_List*_tmpED3;yyval=Cyc_YY13(((_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpED3->tl=0,_tmpED3)))))));}
break;}case 408: _LL3E3: {
# 2899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2898 "parse.y"
{struct Cyc_List_List*_tmpED4;yyval=Cyc_YY13(((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpED4->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpED4)))))));}
break;}case 409: _LL3E4: {
# 2901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 410: _LL3E5: {
# 2907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2906 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 411: _LL3E6: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2911 "parse.y"
yyval=yyyyvsp[0];
break;}case 412: _LL3E7: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2913 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 413: _LL3E8: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2915 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 414: _LL3E9: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2919 "parse.y"
yyval=Cyc_YY7(0);
break;}case 415: _LL3EA: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2920 "parse.y"
{struct Cyc_Core_Opt*_tmpED5;yyval=Cyc_YY7(((_tmpED5=_cycalloc_atomic(sizeof(*_tmpED5)),((_tmpED5->v=(void*)Cyc_Absyn_Times,_tmpED5)))));}
break;}case 416: _LL3EB: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2921 "parse.y"
{struct Cyc_Core_Opt*_tmpED6;yyval=Cyc_YY7(((_tmpED6=_cycalloc_atomic(sizeof(*_tmpED6)),((_tmpED6->v=(void*)Cyc_Absyn_Div,_tmpED6)))));}
break;}case 417: _LL3EC: {
# 2924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2922 "parse.y"
{struct Cyc_Core_Opt*_tmpED7;yyval=Cyc_YY7(((_tmpED7=_cycalloc_atomic(sizeof(*_tmpED7)),((_tmpED7->v=(void*)Cyc_Absyn_Mod,_tmpED7)))));}
break;}case 418: _LL3ED: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2923 "parse.y"
{struct Cyc_Core_Opt*_tmpED8;yyval=Cyc_YY7(((_tmpED8=_cycalloc_atomic(sizeof(*_tmpED8)),((_tmpED8->v=(void*)Cyc_Absyn_Plus,_tmpED8)))));}
break;}case 419: _LL3EE: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2924 "parse.y"
{struct Cyc_Core_Opt*_tmpED9;yyval=Cyc_YY7(((_tmpED9=_cycalloc_atomic(sizeof(*_tmpED9)),((_tmpED9->v=(void*)Cyc_Absyn_Minus,_tmpED9)))));}
break;}case 420: _LL3EF: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2925 "parse.y"
{struct Cyc_Core_Opt*_tmpEDA;yyval=Cyc_YY7(((_tmpEDA=_cycalloc_atomic(sizeof(*_tmpEDA)),((_tmpEDA->v=(void*)Cyc_Absyn_Bitlshift,_tmpEDA)))));}
break;}case 421: _LL3F0: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2926 "parse.y"
{struct Cyc_Core_Opt*_tmpEDB;yyval=Cyc_YY7(((_tmpEDB=_cycalloc_atomic(sizeof(*_tmpEDB)),((_tmpEDB->v=(void*)Cyc_Absyn_Bitlrshift,_tmpEDB)))));}
break;}case 422: _LL3F1: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2927 "parse.y"
{struct Cyc_Core_Opt*_tmpEDC;yyval=Cyc_YY7(((_tmpEDC=_cycalloc_atomic(sizeof(*_tmpEDC)),((_tmpEDC->v=(void*)Cyc_Absyn_Bitand,_tmpEDC)))));}
break;}case 423: _LL3F2: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2928 "parse.y"
{struct Cyc_Core_Opt*_tmpEDD;yyval=Cyc_YY7(((_tmpEDD=_cycalloc_atomic(sizeof(*_tmpEDD)),((_tmpEDD->v=(void*)Cyc_Absyn_Bitxor,_tmpEDD)))));}
break;}case 424: _LL3F3: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2929 "parse.y"
{struct Cyc_Core_Opt*_tmpEDE;yyval=Cyc_YY7(((_tmpEDE=_cycalloc_atomic(sizeof(*_tmpEDE)),((_tmpEDE->v=(void*)Cyc_Absyn_Bitor,_tmpEDE)))));}
break;}case 425: _LL3F4: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2934 "parse.y"
yyval=yyyyvsp[0];
break;}case 426: _LL3F5: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2936 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3F6: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2939 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3F7: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2942 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3F8: {
# 2945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2944 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3F9: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2946 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3FA: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2948 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 432: _LL3FB: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2952 "parse.y"
yyval=yyyyvsp[0];
break;}case 433: _LL3FC: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2956 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3FD: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2958 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3FE: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2962 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3FF: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2964 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL400: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2968 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL401: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2970 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL402: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2974 "parse.y"
yyval=yyyyvsp[0];
break;}case 440: _LL403: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2976 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL404: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2980 "parse.y"
yyval=yyyyvsp[0];
break;}case 442: _LL405: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2982 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 443: _LL406: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2986 "parse.y"
yyval=yyyyvsp[0];
break;}case 444: _LL407: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2988 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL408: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2990 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL409: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2994 "parse.y"
yyval=yyyyvsp[0];
break;}case 447: _LL40A: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2996 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL40B: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2998 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL40C: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3000 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL40D: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3002 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL40E: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3006 "parse.y"
yyval=yyyyvsp[0];
break;}case 452: _LL40F: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3008 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL410: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3010 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 454: _LL411: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3014 "parse.y"
yyval=yyyyvsp[0];
break;}case 455: _LL412: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3016 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL413: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3018 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL414: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3022 "parse.y"
yyval=yyyyvsp[0];
break;}case 458: _LL415: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3024 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL416: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3026 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 460: _LL417: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3028 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL418: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3032 "parse.y"
yyval=yyyyvsp[0];
break;}case 462: _LL419: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3034 "parse.y"
void*_tmp813=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp813,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 3037
break;}case 463: _LL41A: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3040 "parse.y"
yyval=yyyyvsp[0];
break;}case 464: _LL41B: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3041 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL41C: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3042 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL41D: {
# 3045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3043 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 467: _LL41E: {
# 3046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3044 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL41F: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3045 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 469: _LL420: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3046 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 470: _LL421: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3048 "parse.y"
void*_tmp814=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp814,(unsigned int)(yyyylsp[0]).first_line));
# 3051
break;}case 471: _LL422: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3051 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 472: _LL423: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3053 "parse.y"
void*_tmp815=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp815,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3056
break;}case 473: _LL424: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3058 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEE4;struct Cyc_Absyn_MallocInfo _tmpEE3;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEE2;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEE2=_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2[0]=((_tmpEE4.tag=33,((_tmpEE4.f1=((_tmpEE3.is_calloc=0,((_tmpEE3.rgn=0,((_tmpEE3.elt_type=0,((_tmpEE3.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEE3.fat_result=0,_tmpEE3)))))))))),_tmpEE4)))),_tmpEE2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3060
break;}case 474: _LL425: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3061 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEEA;struct Cyc_Absyn_MallocInfo _tmpEE9;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEE8;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=((_tmpEEA.tag=33,((_tmpEEA.f1=((_tmpEE9.is_calloc=0,((_tmpEE9.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEE9.elt_type=0,((_tmpEE9.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEE9.fat_result=0,_tmpEE9)))))))))),_tmpEEA)))),_tmpEE8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3063
break;}case 475: _LL426: {
# 3065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3064 "parse.y"
void*_tmp81C=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEF4;void**_tmpEF3;struct Cyc_Absyn_MallocInfo _tmpEF2;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEF1;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1[0]=((_tmpEF4.tag=33,((_tmpEF4.f1=((_tmpEF2.is_calloc=1,((_tmpEF2.rgn=0,((_tmpEF2.elt_type=((_tmpEF3=_cycalloc(sizeof(*_tmpEF3)),((_tmpEF3[0]=_tmp81C,_tmpEF3)))),((_tmpEF2.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEF2.fat_result=0,_tmpEF2)))))))))),_tmpEF4)))),_tmpEF1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3067
break;}case 476: _LL427: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3069 "parse.y"
void*_tmp821=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEFE;void**_tmpEFD;struct Cyc_Absyn_MallocInfo _tmpEFC;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEFB;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEFB=_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB[0]=((_tmpEFE.tag=33,((_tmpEFE.f1=((_tmpEFC.is_calloc=1,((_tmpEFC.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEFC.elt_type=((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD[0]=_tmp821,_tmpEFD)))),((_tmpEFC.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEFC.fat_result=0,_tmpEFC)))))))))),_tmpEFE)))),_tmpEFB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3072
break;}case 477: _LL428: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3073 "parse.y"
{struct Cyc_Absyn_Exp*_tmpEFF[1];yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpEFF[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEFF,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 478: _LL429: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3075 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF05;struct _dyneither_ptr*_tmpF04;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpF03;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03[0]=((_tmpF05.tag=37,((_tmpF05.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF05.f2=((_tmpF04=_cycalloc(sizeof(*_tmpF04)),((_tmpF04[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpF04)))),_tmpF05)))))),_tmpF03)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 479: _LL42A: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3077 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF0B;struct _dyneither_ptr*_tmpF0A;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpF09;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((_tmpF09[0]=((_tmpF0B.tag=37,((_tmpF0B.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpF0B.f2=((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpF0A)))),_tmpF0B)))))),_tmpF09)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 480: _LL42B: {
# 3080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3079 "parse.y"
void*_tmp82D=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp82D,(unsigned int)(yyyylsp[0]).first_line));
break;}case 481: _LL42C: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3082 "parse.y"
struct _tuple20 _tmp82E=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp830;struct _dyneither_ptr _tmp831;struct _tuple20 _tmp82F=_tmp82E;_tmp830=_tmp82F.f1;_tmp831=_tmp82F.f2;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp830,_tmp831,(unsigned int)(yyyylsp[0]).first_line));
break;}case 482: _LL42D: {
# 3086
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3088
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3087 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
break;}case 483: _LL42E: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3088 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
break;}case 484: _LL42F: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3089 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
break;}case 485: _LL430: {
# 3092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3094 "parse.y"
yyval=yyyyvsp[0];
break;}case 486: _LL431: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3096 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 487: _LL432: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3098 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL433: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3100 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 489: _LL434: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3102 "parse.y"
{struct _dyneither_ptr*_tmpF0C;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF0C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 490: _LL435: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3104 "parse.y"
{struct _dyneither_ptr*_tmpF0D;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF0D=_cycalloc(sizeof(*_tmpF0D)),((_tmpF0D[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF0D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 491: _LL436: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3106 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 492: _LL437: {
# 3109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3108 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 493: _LL438: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3110 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF10;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF0F;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF0F=_cycalloc(sizeof(*_tmpF0F)),((_tmpF0F[0]=((_tmpF10.tag=24,((_tmpF10.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF10.f2=0,_tmpF10)))))),_tmpF0F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 494: _LL439: {
# 3113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3112 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF13;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF12;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=((_tmpF13.tag=24,((_tmpF13.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF13.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpF13)))))),_tmpF12)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 495: _LL43A: {
# 3115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3114 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF16;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF15;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF16.tag=24,((_tmpF16.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF16.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpF16)))))),_tmpF15)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 496: _LL43B: {
# 3117
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3119 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF20;struct _dyneither_ptr*_tmpF1F;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF1E;struct Cyc_List_List*_tmpF1D;yyval=Cyc_YY3(((_tmpF1D=_cycalloc(sizeof(*_tmpF1D)),((_tmpF1D->hd=(void*)((_tmpF20=_cycalloc(sizeof(*_tmpF20)),((_tmpF20[0]=((_tmpF1E.tag=0,((_tmpF1E.f1=((_tmpF1F=_cycalloc(sizeof(*_tmpF1F)),((_tmpF1F[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF1F)))),_tmpF1E)))),_tmpF20)))),((_tmpF1D->tl=0,_tmpF1D)))))));}
break;}case 497: _LL43C: {
# 3122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3122 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF26;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF25;struct Cyc_List_List*_tmpF24;yyval=Cyc_YY3(((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((_tmpF24->hd=(void*)((_tmpF26=_cycalloc_atomic(sizeof(*_tmpF26)),((_tmpF26[0]=((_tmpF25.tag=1,((_tmpF25.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_Int_tok(yyyyvsp[0])),_tmpF25)))),_tmpF26)))),((_tmpF24->tl=0,_tmpF24)))))));}
break;}case 498: _LL43D: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3124 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF30;struct _dyneither_ptr*_tmpF2F;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF2E;struct Cyc_List_List*_tmpF2D;yyval=Cyc_YY3(((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D->hd=(void*)((_tmpF30=_cycalloc(sizeof(*_tmpF30)),((_tmpF30[0]=((_tmpF2E.tag=0,((_tmpF2E.f1=((_tmpF2F=_cycalloc(sizeof(*_tmpF2F)),((_tmpF2F[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF2F)))),_tmpF2E)))),_tmpF30)))),((_tmpF2D->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF2D)))))));}
break;}case 499: _LL43E: {
# 3127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3127 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF36;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF35;struct Cyc_List_List*_tmpF34;yyval=Cyc_YY3(((_tmpF34=_cycalloc(sizeof(*_tmpF34)),((_tmpF34->hd=(void*)((_tmpF36=_cycalloc_atomic(sizeof(*_tmpF36)),((_tmpF36[0]=((_tmpF35.tag=1,((_tmpF35.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_Int_tok(yyyyvsp[2])),_tmpF35)))),_tmpF36)))),((_tmpF34->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF34)))))));}
break;}case 500: _LL43F: {
# 3130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL440: {
# 3136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3135 "parse.y"
yyval=yyyyvsp[0];
break;}case 502: _LL441: {
# 3138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3137 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL442: {
# 3140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3139 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 504: _LL443: {
# 3142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3141 "parse.y"
yyval=yyyyvsp[1];
break;}case 505: _LL444: {
# 3144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3146 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 506: _LL445: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3148 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 507: _LL446: {
# 3151
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3153
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3151 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 508: _LL447: {
# 3154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3154 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF39;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF38;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF38=_cycalloc(sizeof(*_tmpF38)),((_tmpF38[0]=((_tmpF39.tag=28,((_tmpF39.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF39.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpF39.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3])),((_tmpF39.f4=0,_tmpF39)))))))))),_tmpF38)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 509: _LL448: {
# 3157
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3159
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3157 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 510: _LL449: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3161 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
break;}case 511: _LL44A: {
# 3164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpF3A;yyval=Cyc_YY4(((_tmpF3A=_cycalloc(sizeof(*_tmpF3A)),((_tmpF3A->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF3A->tl=0,_tmpF3A)))))));}
break;}case 512: _LL44B: {
# 3170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3169 "parse.y"
{struct Cyc_List_List*_tmpF3B;yyval=Cyc_YY4(((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF3B->tl=Cyc_yyget_YY4(yyyyvsp[0]),_tmpF3B)))))));}
break;}case 513: _LL44C: {
# 3172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_const_exp(Cyc_yyget_Int_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 514: _LL44D: {
# 3178
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3176 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 515: _LL44E: {
# 3179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3177 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 516: _LL44F: {
# 3180
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3179 "parse.y"
struct _dyneither_ptr _tmp84C=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp84C);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp84C,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3187
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp84C,i,(unsigned int)(yyyylsp[0]).first_line));
# 3189
break;}case 517: _LL450: {
# 3191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3190 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 518: _LL451: {
# 3193
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3195
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3194 "parse.y"
{struct _dyneither_ptr*_tmpF3E;struct _tuple0*_tmpF3D;yyval=Cyc_QualId_tok(((_tmpF3D=_cycalloc(sizeof(*_tmpF3D)),((_tmpF3D->f1=Cyc_Absyn_Rel_n(0),((_tmpF3D->f2=((_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF3E)))),_tmpF3D)))))));}
break;}case 519: _LL452: {
# 3197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3195 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL453: {
# 3198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3198 "parse.y"
{struct _dyneither_ptr*_tmpF41;struct _tuple0*_tmpF40;yyval=Cyc_QualId_tok(((_tmpF40=_cycalloc(sizeof(*_tmpF40)),((_tmpF40->f1=Cyc_Absyn_Rel_n(0),((_tmpF40->f2=((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF41)))),_tmpF40)))))));}
break;}case 521: _LL454: {
# 3201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3199 "parse.y"
yyval=yyyyvsp[0];
break;}case 522: _LL455: {
# 3202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3204 "parse.y"
yyval=yyyyvsp[0];
break;}case 523: _LL456: {
# 3207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3205 "parse.y"
yyval=yyyyvsp[0];
break;}case 524: _LL457: {
# 3208
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3208 "parse.y"
yyval=yyyyvsp[0];
break;}case 525: _LL458: {
# 3211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3209 "parse.y"
yyval=yyyyvsp[0];
break;}case 526: _LL459: {
# 3212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3214 "parse.y"
break;}case 527: _LL45A: {
# 3216
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3218
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3214 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL45B:
# 3219
 break;}
# 371 "cyc_bison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 374
yylsp_offset -=yylen;
# 388 "cyc_bison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 391
++ yylsp_offset;
if(yylen == 0){
# 394
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 401
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 411
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(528,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(148,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6868) && Cyc_yycheck[_check_known_subscript_notnull(6869,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6869,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(148,yyn - 146)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1070,yystate)];
# 431
if(yyn > - 32768  && yyn < 6868){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 294 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6869,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(294,x)])+ 15,count ++);}
{unsigned int _tmpF46;unsigned int _tmpF45;struct _dyneither_ptr _tmpF44;char*_tmpF43;unsigned int _tmpF42;msg=((_tmpF42=(unsigned int)(sze + 15),((_tmpF43=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpF42 + 1)),((_tmpF44=_tag_dyneither(_tmpF43,sizeof(char),_tmpF42 + 1),((((_tmpF45=_tmpF42,((_tmpF47(& _tmpF46,& _tmpF45,& _tmpF43),_tmpF43[_tmpF45]=(char)0)))),_tmpF44))))))));}
{const char*_tmpF48;Cyc_strcpy(msg,((_tmpF48="parse error",_tag_dyneither(_tmpF48,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 294 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6869,x + yyn)]== x){
# 453
{const char*_tmpF4A;const char*_tmpF49;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpF4A=", expecting `",_tag_dyneither(_tmpF4A,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpF49=" or `",_tag_dyneither(_tmpF49,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(294,x)]);
{const char*_tmpF4B;Cyc_strcat(msg,((_tmpF4B="'",_tag_dyneither(_tmpF4B,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
const char*_tmpF4C;Cyc_yyerror(((_tmpF4C="parse error",_tag_dyneither(_tmpF4C,sizeof(char),12))),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp85A=1;_npop_handler(0);return _tmp85A;}
# 485
yychar=-2;}
# 491
yyerrstatus=3;
# 493
goto yyerrhandle;
# 495
yyerrdefault:
# 505 "cyc_bison.simple"
 yyerrpop:
# 507
 if(yyssp_offset == 0){int _tmp85B=1;_npop_handler(0);return _tmp85B;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1070,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6868) || Cyc_yycheck[_check_known_subscript_notnull(6869,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6869,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1069){
int _tmp85C=0;_npop_handler(0);return _tmp85C;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3217 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp866=v;union Cyc_Absyn_Cnst _tmp867;char _tmp868;struct _dyneither_ptr _tmp869;union Cyc_Absyn_Nmspace _tmp86A;struct _dyneither_ptr*_tmp86B;struct Cyc_Absyn_Exp*_tmp86C;struct Cyc_Absyn_Stmt*_tmp86D;_LL45E: if((_tmp866.Int_tok).tag != 1)goto _LL460;_tmp867=(union Cyc_Absyn_Cnst)(_tmp866.Int_tok).val;_LL45F:
{const char*_tmpF50;void*_tmpF4F[1];struct Cyc_String_pa_PrintArg_struct _tmpF4E;(_tmpF4E.tag=0,((_tmpF4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp867)),((_tmpF4F[0]=& _tmpF4E,Cyc_fprintf(Cyc_stderr,((_tmpF50="%s",_tag_dyneither(_tmpF50,sizeof(char),3))),_tag_dyneither(_tmpF4F,sizeof(void*),1)))))));}goto _LL45D;_LL460: if((_tmp866.Char_tok).tag != 2)goto _LL462;_tmp868=(char)(_tmp866.Char_tok).val;_LL461:
{const char*_tmpF54;void*_tmpF53[1];struct Cyc_Int_pa_PrintArg_struct _tmpF52;(_tmpF52.tag=1,((_tmpF52.f1=(unsigned long)((int)_tmp868),((_tmpF53[0]=& _tmpF52,Cyc_fprintf(Cyc_stderr,((_tmpF54="%c",_tag_dyneither(_tmpF54,sizeof(char),3))),_tag_dyneither(_tmpF53,sizeof(void*),1)))))));}goto _LL45D;_LL462: if((_tmp866.String_tok).tag != 3)goto _LL464;_tmp869=(struct _dyneither_ptr)(_tmp866.String_tok).val;_LL463:
{const char*_tmpF58;void*_tmpF57[1];struct Cyc_String_pa_PrintArg_struct _tmpF56;(_tmpF56.tag=0,((_tmpF56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp869),((_tmpF57[0]=& _tmpF56,Cyc_fprintf(Cyc_stderr,((_tmpF58="\"%s\"",_tag_dyneither(_tmpF58,sizeof(char),5))),_tag_dyneither(_tmpF57,sizeof(void*),1)))))));}goto _LL45D;_LL464: if((_tmp866.QualId_tok).tag != 5)goto _LL466;_tmp86A=((struct _tuple0*)(_tmp866.QualId_tok).val)->f1;_tmp86B=((struct _tuple0*)(_tmp866.QualId_tok).val)->f2;_LL465: {
# 3223
struct Cyc_List_List*_tmp877=0;
{union Cyc_Absyn_Nmspace _tmp878=_tmp86A;struct Cyc_List_List*_tmp879;struct Cyc_List_List*_tmp87A;struct Cyc_List_List*_tmp87B;_LL46D: if((_tmp878.Rel_n).tag != 1)goto _LL46F;_tmp879=(struct Cyc_List_List*)(_tmp878.Rel_n).val;_LL46E:
 _tmp877=_tmp879;goto _LL46C;_LL46F: if((_tmp878.Abs_n).tag != 2)goto _LL471;_tmp87A=(struct Cyc_List_List*)(_tmp878.Abs_n).val;_LL470:
 _tmp877=_tmp87A;goto _LL46C;_LL471: if((_tmp878.C_n).tag != 3)goto _LL473;_tmp87B=(struct Cyc_List_List*)(_tmp878.C_n).val;_LL472:
 _tmp877=_tmp87B;goto _LL46C;_LL473: if((_tmp878.Loc_n).tag != 4)goto _LL46C;_LL474:
 goto _LL46C;_LL46C:;}
# 3230
for(0;_tmp877 != 0;_tmp877=_tmp877->tl){
const char*_tmpF5C;void*_tmpF5B[1];struct Cyc_String_pa_PrintArg_struct _tmpF5A;(_tmpF5A.tag=0,((_tmpF5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp877->hd)),((_tmpF5B[0]=& _tmpF5A,Cyc_fprintf(Cyc_stderr,((_tmpF5C="%s::",_tag_dyneither(_tmpF5C,sizeof(char),5))),_tag_dyneither(_tmpF5B,sizeof(void*),1)))))));}
{const char*_tmpF60;void*_tmpF5F[1];struct Cyc_String_pa_PrintArg_struct _tmpF5E;(_tmpF5E.tag=0,((_tmpF5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp86B),((_tmpF5F[0]=& _tmpF5E,Cyc_fprintf(Cyc_stderr,((_tmpF60="%s::",_tag_dyneither(_tmpF60,sizeof(char),5))),_tag_dyneither(_tmpF5F,sizeof(void*),1)))))));}
goto _LL45D;}_LL466: if((_tmp866.Exp_tok).tag != 7)goto _LL468;_tmp86C=(struct Cyc_Absyn_Exp*)(_tmp866.Exp_tok).val;_LL467:
{const char*_tmpF64;void*_tmpF63[1];struct Cyc_String_pa_PrintArg_struct _tmpF62;(_tmpF62.tag=0,((_tmpF62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp86C)),((_tmpF63[0]=& _tmpF62,Cyc_fprintf(Cyc_stderr,((_tmpF64="%s",_tag_dyneither(_tmpF64,sizeof(char),3))),_tag_dyneither(_tmpF63,sizeof(void*),1)))))));}goto _LL45D;_LL468: if((_tmp866.Stmt_tok).tag != 8)goto _LL46A;_tmp86D=(struct Cyc_Absyn_Stmt*)(_tmp866.Stmt_tok).val;_LL469:
{const char*_tmpF68;void*_tmpF67[1];struct Cyc_String_pa_PrintArg_struct _tmpF66;(_tmpF66.tag=0,((_tmpF66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp86D)),((_tmpF67[0]=& _tmpF66,Cyc_fprintf(Cyc_stderr,((_tmpF68="%s",_tag_dyneither(_tmpF68,sizeof(char),3))),_tag_dyneither(_tmpF67,sizeof(void*),1)))))));}goto _LL45D;_LL46A:;_LL46B:
{const char*_tmpF6B;void*_tmpF6A;(_tmpF6A=0,Cyc_fprintf(Cyc_stderr,((_tmpF6B="?",_tag_dyneither(_tmpF6B,sizeof(char),2))),_tag_dyneither(_tmpF6A,sizeof(void*),0)));}goto _LL45D;_LL45D:;}
# 3241
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp88A=_new_region("yyr");struct _RegionHandle*yyr=& _tmp88A;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp88B=Cyc_Parse_parse_result;_npop_handler(0);return _tmp88B;};
# 3244
;_pop_region(yyr);};}
