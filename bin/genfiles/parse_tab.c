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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 445 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 461
typedef void*Cyc_Absyn_funcparams_t;
# 464
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 500
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 507
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 528
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 554
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 561
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 579
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 744 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 916
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 943
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 947
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 951
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 961
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 966
void*Cyc_Absyn_compress_kb(void*);
# 971
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 973
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 976
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 978
extern void*Cyc_Absyn_sint_typ;
# 980
void*Cyc_Absyn_float_typ(int);
# 1028
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1079
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
# 1095
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1108
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1121
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1124
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1127
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1131
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1136
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1138
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1145
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1154
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1159
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1165
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1190
int Cyc_Absyn_fntype_att(void*a);
# 1210
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
{struct _tuple10*_tmp895;declarators=((_tmp895=_region_malloc(r,sizeof(*_tmp895)),((_tmp895->tl=declarators,((_tmp895->hd=_tmp2,_tmp895))))));}{
struct Cyc_List_List*_tmp896;exprs=((_tmp896=_region_malloc(r,sizeof(*_tmp896)),((_tmp896->hd=_tmp3,((_tmp896->tl=exprs,_tmp896))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp899;struct Cyc_Position_Exit_exn_struct*_tmp898;(int)_throw((void*)((_tmp898=_cycalloc_atomic(sizeof(*_tmp898)),((_tmp898[0]=((_tmp899.tag=Cyc_Position_Exit,_tmp899)),_tmp898)))));};}
# 191
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp89E;void*_tmp89D[2];struct Cyc_String_pa_PrintArg_struct _tmp89C;struct Cyc_String_pa_PrintArg_struct _tmp89B;(_tmp89B.tag=0,((_tmp89B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp89D[0]=& _tmp89C,((_tmp89D[1]=& _tmp89B,Cyc_fprintf(Cyc_stderr,((_tmp89E="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp89E,sizeof(char),46))),_tag_dyneither(_tmp89D,sizeof(void*),2)))))))))))));}
return;}
# 197
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 199
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp8A1;void*_tmp8A0;(_tmp8A0=0,Cyc_Tcutil_warn(loc,((_tmp8A1="qualifier on type is ignored",_tag_dyneither(_tmp8A1,sizeof(char),29))),_tag_dyneither(_tmp8A0,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8A4;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8A3;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3[0]=((_tmp8A4.tag=1,((_tmp8A4.f1=_tmp1A,_tmp8A4)))),_tmp8A3)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 231
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 234
struct _tuple14 _tmp8A5;return(_tmp8A5.f1=nullable,((_tmp8A5.f2=bound,((_tmp8A5.f3=zeroterm,((_tmp8A5.f4=rgn,_tmp8A5)))))));};}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
struct _dyneither_ptr*_tmp8B2;const char*_tmp8B1;void*_tmp8B0[1];struct Cyc_Int_pa_PrintArg_struct _tmp8AF;struct _tuple0*_tmp8AE;return(_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE->f1=Cyc_Absyn_Rel_n(0),((_tmp8AE->f2=(
(_tmp8B2=_cycalloc(sizeof(*_tmp8B2)),((_tmp8B2[0]=(struct _dyneither_ptr)((_tmp8AF.tag=1,((_tmp8AF.f1=(unsigned long)enum_counter ++,((_tmp8B0[0]=& _tmp8AF,Cyc_aprintf(((_tmp8B1="__anonymous_enum_%d__",_tag_dyneither(_tmp8B1,sizeof(char),22))),_tag_dyneither(_tmp8B0,sizeof(void*),1)))))))),_tmp8B2)))),_tmp8AE)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp8B3;Cyc_Parse_err(((_tmp8B3="bad type params in struct field",_tag_dyneither(_tmp8B3,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp8B4;Cyc_Parse_err(((_tmp8B4="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp8B4,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp8B5;return(_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5->name=(*_tmp26).f2,((_tmp8B5->tq=_tmp27,((_tmp8B5->type=_tmp28,((_tmp8B5->width=_tmp2B,((_tmp8B5->attributes=_tmp2A,((_tmp8B5->requires_clause=_tmp2C,_tmp8B5)))))))))))));};}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp8B6;return(_tmp8B6.Signed_spec=0,((_tmp8B6.Unsigned_spec=0,((_tmp8B6.Short_spec=0,((_tmp8B6.Long_spec=0,((_tmp8B6.Long_Long_spec=0,((_tmp8B6.Valid_type_spec=0,((_tmp8B6.Type_spec=Cyc_Absyn_sint_typ,((_tmp8B6.loc=loc,_tmp8B6)))))))))))))));}
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
struct Cyc_String_pa_PrintArg_struct _tmp8BE;void*_tmp8BD[1];const char*_tmp8BC;struct _dyneither_ptr*_tmp8BB;struct _dyneither_ptr*nm=(_tmp8BB=_cycalloc(sizeof(*_tmp8BB)),((_tmp8BB[0]=(struct _dyneither_ptr)((_tmp8BE.tag=0,((_tmp8BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp8BD[0]=& _tmp8BE,Cyc_aprintf(((_tmp8BC="`%s",_tag_dyneither(_tmp8BC,sizeof(char),4))),_tag_dyneither(_tmp8BD,sizeof(void*),1)))))))),_tmp8BB)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8CD;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8CC;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8CB;struct Cyc_Absyn_Tvar*_tmp8CA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8C9;*_tmp42=(void*)((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8CD.tag=2,((_tmp8CD.f1=((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA->name=nm,((_tmp8CA->identity=- 1,((_tmp8CA->kind=(void*)((_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CB.tag=0,((_tmp8CB.f1=& Cyc_Tcutil_ik,_tmp8CB)))),_tmp8CC)))),_tmp8CA)))))))),_tmp8CD)))),_tmp8C9))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 330
{struct _tuple18*_tmp8D0;struct Cyc_List_List*_tmp8CF;_tmp36=((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF->hd=((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0->f1=_tmp39,((_tmp8D0->f2=_tmp3B,_tmp8D0)))))),((_tmp8CF->tl=_tmp36,_tmp8CF))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 334
struct Cyc_String_pa_PrintArg_struct _tmp8D8;void*_tmp8D7[1];const char*_tmp8D6;struct _dyneither_ptr*_tmp8D5;struct _dyneither_ptr*nm=(_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5[0]=(struct _dyneither_ptr)((_tmp8D8.tag=0,((_tmp8D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp8D7[0]=& _tmp8D8,Cyc_aprintf(((_tmp8D6="`%s",_tag_dyneither(_tmp8D6,sizeof(char),4))),_tag_dyneither(_tmp8D7,sizeof(void*),1)))))))),_tmp8D5)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8E7;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8E6;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8E5;struct Cyc_Absyn_Tvar*_tmp8E4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8E3;*_tmp3F=(void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E7.tag=2,((_tmp8E7.f1=((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4->name=nm,((_tmp8E4->identity=- 1,((_tmp8E4->kind=(void*)((_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E5.tag=0,((_tmp8E5.f1=& Cyc_Tcutil_rk,_tmp8E5)))),_tmp8E6)))),_tmp8E4)))))))),_tmp8E7)))),_tmp8E3))));}
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
{struct _tuple18*_tmp8EA;struct Cyc_List_List*_tmp8E9;_tmp57=((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9->hd=((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8EA->f2=_tmp5B,_tmp8EA)))))),((_tmp8E9->tl=_tmp57,_tmp8E9))))));}goto _LL1D;_LL20:;_LL21:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8ED;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8EC;return Cyc_Absyn_new_exp((void*)((_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=38,((_tmp8ED.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8ED)))),_tmp8EC)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8F3;struct Cyc_Absyn_ArrayInfo _tmp8F2;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F1;return(void*)((_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F3.tag=8,((_tmp8F3.f1=((_tmp8F2.elt_type=_tmp78,((_tmp8F2.tq=_tmp6C,((_tmp8F2.num_elts=nelts2,((_tmp8F2.zero_term=_tmp6E,((_tmp8F2.zt_loc=_tmp6F,_tmp8F2)))))))))),_tmp8F3)))),_tmp8F1))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 385
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 389
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8F8;union Cyc_Absyn_Constraint*_tmp8F7;_tmp7D=((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),(((_tmp8F7->Eq_constr).val=(void*)((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=((_tmp8F8.tag=1,((_tmp8F8.f1=_tmp81,_tmp8F8)))),_tmp8F9)))),(((_tmp8F7->Eq_constr).tag=1,_tmp8F7))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 394
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8FF;struct Cyc_Absyn_PtrInfo _tmp8FE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8FD;return(void*)((_tmp8FD=_cycalloc(sizeof(*_tmp8FD)),((_tmp8FD[0]=((_tmp8FF.tag=5,((_tmp8FF.f1=((_tmp8FE.elt_typ=_tmp7C,((_tmp8FE.elt_tq=_tmp72,((_tmp8FE.ptr_atts=(((_tmp8FE.ptr_atts).rgn=_tmp73,(((_tmp8FE.ptr_atts).nullable=_tmp74,(((_tmp8FE.ptr_atts).bounds=_tmp7D,(((_tmp8FE.ptr_atts).zero_term=_tmp76,(((_tmp8FE.ptr_atts).ptrloc=_tmp77,_tmp8FE.ptr_atts)))))))))),_tmp8FE)))))),_tmp8FF)))),_tmp8FD))));}
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
struct _tuple19*_tmp900;return(_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900->f1=(*t).f2,((_tmp900->f2=(*t).f3,_tmp900)))));}
# 418
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp903;void*_tmp902;(_tmp902=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp903="bad declaration in `forarray' statement",_tag_dyneither(_tmp903,sizeof(char),40))),_tag_dyneither(_tmp902,sizeof(void*),0)));}_LL33:;}
# 425
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 434
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp904;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp904="`H",_tag_dyneither(_tmp904,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp905;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp905="`U",_tag_dyneither(_tmp905,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp906;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp906="`RC",_tag_dyneither(_tmp906,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 442
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp910;struct _dyneither_ptr*_tmp90F;struct Cyc_Absyn_Tvar*_tmp90E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp90D;return(void*)((_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp910.tag=2,((_tmp910.f1=((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E->name=((_tmp90F=_cycalloc(sizeof(*_tmp90F)),((_tmp90F[0]=s,_tmp90F)))),((_tmp90E->identity=- 1,((_tmp90E->kind=k,_tmp90E)))))))),_tmp910)))),_tmp90D))));}}}}
# 445
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp913;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp912;k=(void*)((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((_tmp913.tag=1,((_tmp913.f1=0,_tmp913)))),_tmp912))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp916;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp915;k=(void*)((_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915[0]=((_tmp916.tag=2,((_tmp916.f1=0,((_tmp916.f2=_tmp9C,_tmp916)))))),_tmp915))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 452
struct Cyc_Absyn_Tvar*_tmp917;return(_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917->name=t->name,((_tmp917->identity=- 1,((_tmp917->kind=k,_tmp917)))))));};}
# 459
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp91A;void*_tmp919;(_tmp919=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp91A="expecting a list of type variables, not types",_tag_dyneither(_tmp91A,sizeof(char),46))),_tag_dyneither(_tmp919,sizeof(void*),0)));}_LL44:;}
# 465
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp91D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp91C;return(void*)((_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91D.tag=2,((_tmp91D.f1=pr,_tmp91D)))),_tmp91C))));}
# 470
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 473
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 475
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp920;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp91F;*_tmpAD=(void*)((_tmp91F=_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=((_tmp920.tag=2,((_tmp920.f1=0,((_tmp920.f2=k,_tmp920)))))),_tmp91F))));}
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
{const char*_tmp923;void*_tmp922;(_tmp922=0,Cyc_Tcutil_warn(loc,((_tmp923="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp923,sizeof(char),93))),_tag_dyneither(_tmp922,sizeof(void*),0)));}
# 508
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp926;void*_tmp925;(_tmp925=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp926="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp926,sizeof(char),73))),_tag_dyneither(_tmp925,sizeof(void*),0)));}{
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
const char*_tmp929;void*_tmp928;(_tmp928=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp929="initializer found in parameter declaration",_tag_dyneither(_tmp929,sizeof(char),43))),_tag_dyneither(_tmp928,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp92C;void*_tmp92B;(_tmp92B=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp92C="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp92C,sizeof(char),47))),_tag_dyneither(_tmp92B,sizeof(void*),0)));}
{struct _tuple8*_tmp92F;struct Cyc_List_List*_tmp92E;rev_new_params=(
(_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E->hd=((_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F->f1=(*_tmpC4->name).f2,((_tmp92F->f2=_tmpC4->tq,((_tmp92F->f3=_tmpC4->type,_tmp92F)))))))),((_tmp92E->tl=rev_new_params,_tmp92E))))));}
# 530
goto L;_LL60:;_LL61: {
const char*_tmp932;void*_tmp931;(_tmp931=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp932="nonvariable declaration in parameter type",_tag_dyneither(_tmp932,sizeof(char),42))),_tag_dyneither(_tmp931,sizeof(void*),0)));}_LL5D:;}
# 534
L: if(_tmpBF == 0){
const char*_tmp936;void*_tmp935[1];struct Cyc_String_pa_PrintArg_struct _tmp934;(_tmp934.tag=0,((_tmp934.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp935[0]=& _tmp934,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp936="%s is not given a type",_tag_dyneither(_tmp936,sizeof(char),23))),_tag_dyneither(_tmp935,sizeof(void*),1)))))));}}{
# 537
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp945;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp944;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp943;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp942;struct Cyc_List_List*_tmp941;return
(_tmp941=_region_malloc(yy,sizeof(*_tmp941)),((_tmp941->hd=(void*)((_tmp945=_region_malloc(yy,sizeof(*_tmp945)),((_tmp945[0]=((_tmp942.tag=3,((_tmp942.f1=(void*)((_tmp944=_region_malloc(yy,sizeof(*_tmp944)),((_tmp944[0]=((_tmp943.tag=1,((_tmp943.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp943.f2=0,((_tmp943.f3=0,((_tmp943.f4=0,((_tmp943.f5=0,((_tmp943.f6=0,((_tmp943.f7=0,_tmp943)))))))))))))))),_tmp944)))),_tmp942)))),_tmp945)))),((_tmp941->tl=0,_tmp941)))));};};_LL58:;}
# 544
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp946;return(_tmp946=_region_malloc(yy,sizeof(*_tmp946)),((_tmp946->hd=(void*)tms->hd,((_tmp946->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp946)))));}_LL53:;};}
# 552
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 556
if(tds != 0){
struct Cyc_Parse_Declarator _tmp947;d=((_tmp947.id=d.id,((_tmp947.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp947))));}{
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
{const char*_tmp948;Cyc_Parse_err(((_tmp948="bad storage class on function",_tag_dyneither(_tmp948,sizeof(char),30))),loc);}break;}}}{
# 578
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 583
if(_tmpDE != 0){
# 586
const char*_tmp94B;void*_tmp94A;(_tmp94A=0,Cyc_Tcutil_warn(loc,((_tmp94B="bad type params, ignoring",_tag_dyneither(_tmp94B,sizeof(char),26))),_tag_dyneither(_tmp94A,sizeof(void*),0)));}{
# 588
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF0;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;_tmpED=(_tmpE3->f1).requires_clause;_tmpEE=(_tmpE3->f1).requires_relns;_tmpEF=(_tmpE3->f1).ensures_clause;_tmpF0=(_tmpE3->f1).ensures_relns;}}_LL68: {
# 592
struct Cyc_List_List*_tmpF1=0;
{struct Cyc_List_List*_tmpF2=_tmpE8;for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){
struct _tuple8*_tmpF3=(struct _tuple8*)_tmpF2->hd;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF7;struct _tuple8*_tmpF4=_tmpF3;_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;_tmpF7=_tmpF4->f3;
if(_tmpF5 == 0){
{const char*_tmp94C;Cyc_Parse_err(((_tmp94C="missing argument variable in function prototype",_tag_dyneither(_tmp94C,sizeof(char),48))),loc);}{
struct _tuple8*_tmp94F;struct Cyc_List_List*_tmp94E;_tmpF1=((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E->hd=((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp94F->f2=_tmpF6,((_tmp94F->f3=_tmpF7,_tmp94F)))))))),((_tmp94E->tl=_tmpF1,_tmp94E))))));};}else{
# 599
struct _tuple8*_tmp952;struct Cyc_List_List*_tmp951;_tmpF1=((_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951->hd=((_tmp952=_cycalloc(sizeof(*_tmp952)),((_tmp952->f1=_tmpF5,((_tmp952->f2=_tmpF6,((_tmp952->f3=_tmpF7,_tmp952)))))))),((_tmp951->tl=_tmpF1,_tmp951))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp953;return(_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953->sc=sc,((_tmp953->name=d.id,((_tmp953->tvs=_tmpE4,((_tmp953->is_inline=is_inline,((_tmp953->effect=_tmpE5,((_tmp953->ret_tqual=_tmpE6,((_tmp953->ret_type=_tmpE7,((_tmp953->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpF1),((_tmp953->c_varargs=_tmpE9,((_tmp953->cyc_varargs=_tmpEA,((_tmp953->rgn_po=_tmpEB,((_tmp953->body=body,((_tmp953->cached_typ=0,((_tmp953->param_vardecls=0,((_tmp953->fn_vardecl=0,((_tmp953->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),((_tmp953->requires_clause=_tmpED,((_tmp953->requires_relns=0,((_tmp953->ensures_clause=_tmpEF,((_tmp953->ensures_relns=0,_tmp953)))))))))))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
# 618
const char*_tmp956;void*_tmp955;(_tmp955=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp956="declarator is not a function prototype",_tag_dyneither(_tmp956,sizeof(char),39))),_tag_dyneither(_tmp955,sizeof(void*),0)));}_LL66:;};};};}static char _tmp101[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
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
void*_tmp957;(_tmp957=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp957,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp958;(_tmp958=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp958,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp959;(_tmp959=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp959,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp95A;(_tmp95A=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp95A,sizeof(void*),0)));}
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
const char*_tmp95D;void*_tmp95C;(_tmp95C=0,Cyc_Tcutil_warn(loc,((_tmp95D="missing type within specifier",_tag_dyneither(_tmp95D,sizeof(char),30))),_tag_dyneither(_tmp95C,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 693
if(seen_sign){
void*_tmp10B=t;enum Cyc_Absyn_Sign _tmp10D;enum Cyc_Absyn_Size_of _tmp10E;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10B;if(_tmp10C->tag != 6)goto _LL6E;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LL6D:
# 696
 if(_tmp10D != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10E);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp95E;Cyc_Parse_err(((_tmp95E="sign specification on non-integral type",_tag_dyneither(_tmp95E,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 701
if(seen_size){
void*_tmp110=t;enum Cyc_Absyn_Sign _tmp112;enum Cyc_Absyn_Size_of _tmp113;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp111=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp110;if(_tmp111->tag != 6)goto _LL73;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;}}_LL72:
# 704
 if(_tmp113 != sz)
t=Cyc_Absyn_int_typ(_tmp112,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp114=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp110;if(_tmp114->tag != 7)goto _LL75;}_LL74:
# 708
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp95F;Cyc_Parse_err(((_tmp95F="size qualifier on non-integral type",_tag_dyneither(_tmp95F,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
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
struct _tuple15*_tmp962;struct Cyc_List_List*_tmp961;return(_tmp961=_region_malloc(r,sizeof(*_tmp961)),((_tmp961->hd=((_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962->f1=_tmp116,((_tmp962->f2=_tmp119,((_tmp962->f3=_tmp11A,((_tmp962->f4=_tmp11B,((_tmp962->f5=_tmp11C,_tmp962)))))))))))),((_tmp961->tl=0,_tmp961)))));}else{
# 728
struct _tuple15*_tmp965;struct Cyc_List_List*_tmp964;return(_tmp964=_region_malloc(r,sizeof(*_tmp964)),((_tmp964->hd=((_tmp965=_region_malloc(r,sizeof(*_tmp965)),((_tmp965->f1=_tmp116,((_tmp965->f2=_tmp119,((_tmp965->f3=_tmp11A,((_tmp965->f4=_tmp11B,((_tmp965->f5=_tmp11C,_tmp965)))))))))))),((_tmp964->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp964)))));}};}
# 732
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 735
if(tms == 0){struct _tuple13 _tmp966;return(_tmp966.f1=tq,((_tmp966.f2=t,((_tmp966.f3=0,((_tmp966.f4=atts,_tmp966)))))));}{
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
struct Cyc_List_List*_tmp967;fn_atts=((_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967->hd=(void*)as->hd,((_tmp967->tl=fn_atts,_tmp967))))));}else{
# 753
struct Cyc_List_List*_tmp968;new_atts=((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968->hd=(void*)as->hd,((_tmp968->tl=new_atts,_tmp968))))));}}}
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
const char*_tmp96B;void*_tmp96A;(_tmp96A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp140,((_tmp96B="function declaration without parameter types",_tag_dyneither(_tmp96B,sizeof(char),45))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp12D=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12D->tag != 4)goto _LL80;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL7F:
# 808
 if(tms->tl == 0){
struct _tuple13 _tmp96C;return(_tmp96C.f1=tq,((_tmp96C.f2=t,((_tmp96C.f3=_tmp12E,((_tmp96C.f4=atts,_tmp96C)))))));}{
# 813
const char*_tmp96F;void*_tmp96E;(_tmp96E=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12F,((_tmp96F="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp96F,sizeof(char),68))),_tag_dyneither(_tmp96E,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp130=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp130->tag != 2)goto _LL82;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_LL81: {
# 816
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp975;struct Cyc_Absyn_PtrInfo _tmp974;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp973;return Cyc_Parse_apply_tms(_tmp132,(void*)((_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973[0]=((_tmp975.tag=5,((_tmp975.f1=((_tmp974.elt_typ=t,((_tmp974.elt_tq=tq,((_tmp974.ptr_atts=_tmp131,_tmp974)))))),_tmp975)))),_tmp973)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp133=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp133->tag != 5)goto _LL77;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL83:
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
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp97B;struct Cyc_Absyn_Typedefdecl*_tmp97A;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp979;return Cyc_Absyn_new_decl((void*)((_tmp979=_cycalloc(sizeof(*_tmp979)),((_tmp979[0]=((_tmp97B.tag=8,((_tmp97B.f1=((_tmp97A=_cycalloc(sizeof(*_tmp97A)),((_tmp97A->name=_tmp157,((_tmp97A->tvs=_tmp15A,((_tmp97A->kind=kind,((_tmp97A->defn=type,((_tmp97A->atts=_tmp15B,((_tmp97A->tq=_tmp158,((_tmp97A->extern_c=0,_tmp97A)))))))))))))))),_tmp97B)))),_tmp979)))),loc);};};}
# 858
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp97E;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp97D;return Cyc_Absyn_new_stmt((void*)((_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D[0]=((_tmp97E.tag=12,((_tmp97E.f1=d,((_tmp97E.f2=s,_tmp97E)))))),_tmp97D)))),d->loc);}
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
const char*_tmp981;void*_tmp980;(_tmp980=0,Cyc_Tcutil_warn(loc,((_tmp981="inline qualifier on non-function definition",_tag_dyneither(_tmp981,sizeof(char),44))),_tag_dyneither(_tmp980,sizeof(void*),0)));}{
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
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp987;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp986;struct Cyc_List_List*_tmp985;struct Cyc_List_List*_tmp188=(_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985->hd=Cyc_Absyn_new_decl((void*)((_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987[0]=((_tmp986.tag=5,((_tmp986.f1=_tmp170,_tmp986)))),_tmp987)))),loc),((_tmp985->tl=0,_tmp985)))));_npop_handler(0);return _tmp188;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp171=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16D;if(_tmp171->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp172=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp171->f1)->r;if(_tmp172->tag != 1)goto _LLA0;else{_tmp173=_tmp172->f1;}}}_LL9F:
# 921
 if(_tmp168 != 0){const char*_tmp988;Cyc_Parse_err(((_tmp988="attributes on enum not supported",_tag_dyneither(_tmp988,sizeof(char),33))),loc);}
_tmp173->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp98E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp98D;struct Cyc_List_List*_tmp98C;struct Cyc_List_List*_tmp18D=(_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C->hd=Cyc_Absyn_new_decl((void*)((_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98D.tag=7,((_tmp98D.f1=_tmp173,_tmp98D)))),_tmp98E)))),loc),((_tmp98C->tl=0,_tmp98C)))));_npop_handler(0);return _tmp18D;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16D;if(_tmp174->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp175=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp174->f1)->r;if(_tmp175->tag != 2)goto _LLA2;else{_tmp176=_tmp175->f1;}}}_LLA1:
# 925
 if(_tmp168 != 0){const char*_tmp98F;Cyc_Parse_err(((_tmp98F="attributes on datatypes not supported",_tag_dyneither(_tmp98F,sizeof(char),38))),loc);}
_tmp176->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp995;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp994;struct Cyc_List_List*_tmp993;struct Cyc_List_List*_tmp192=(_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993->hd=Cyc_Absyn_new_decl((void*)((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995[0]=((_tmp994.tag=6,((_tmp994.f1=_tmp176,_tmp994)))),_tmp995)))),loc),((_tmp993->tl=0,_tmp993)))));_npop_handler(0);return _tmp192;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16D;if(_tmp177->tag != 11)goto _LLA4;else{if((((_tmp177->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp178=((struct _tuple2)(((_tmp177->f1).aggr_info).UnknownAggr).val).f1;_tmp179=((struct _tuple2)(((_tmp177->f1).aggr_info).UnknownAggr).val).f2;_tmp17A=(_tmp177->f1).targs;}}_LLA3: {
# 929
struct Cyc_List_List*_tmp193=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17A);
struct Cyc_Absyn_Aggrdecl*_tmp996;struct Cyc_Absyn_Aggrdecl*_tmp194=(_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996->kind=_tmp178,((_tmp996->sc=s,((_tmp996->name=_tmp179,((_tmp996->tvs=_tmp193,((_tmp996->impl=0,((_tmp996->attributes=0,((_tmp996->expected_mem_kind=0,_tmp996)))))))))))))));
if(_tmp168 != 0){const char*_tmp997;Cyc_Parse_err(((_tmp997="bad attributes on type declaration",_tag_dyneither(_tmp997,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp99D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp99C;struct Cyc_List_List*_tmp99B;struct Cyc_List_List*_tmp199=(_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B->hd=Cyc_Absyn_new_decl((void*)((_tmp99D=_cycalloc(sizeof(*_tmp99D)),((_tmp99D[0]=((_tmp99C.tag=5,((_tmp99C.f1=_tmp194,_tmp99C)))),_tmp99D)))),loc),((_tmp99B->tl=0,_tmp99B)))));_npop_handler(0);return _tmp199;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16D;if(_tmp17B->tag != 3)goto _LLA6;else{if((((_tmp17B->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp17C=(struct Cyc_Absyn_Datatypedecl**)(((_tmp17B->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 934
 if(_tmp168 != 0){const char*_tmp99E;Cyc_Parse_err(((_tmp99E="bad attributes on datatype",_tag_dyneither(_tmp99E,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9A4;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp9A3;struct Cyc_List_List*_tmp9A2;struct Cyc_List_List*_tmp19F=(_tmp9A2=_cycalloc(sizeof(*_tmp9A2)),((_tmp9A2->hd=Cyc_Absyn_new_decl((void*)((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A3.tag=6,((_tmp9A3.f1=*_tmp17C,_tmp9A3)))),_tmp9A4)))),loc),((_tmp9A2->tl=0,_tmp9A2)))));_npop_handler(0);return _tmp19F;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16D;if(_tmp17D->tag != 3)goto _LLA8;else{if((((_tmp17D->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp17E=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17D->f1).datatype_info).UnknownDatatype).val).name;_tmp17F=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17D->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp180=(_tmp17D->f1).targs;}}_LLA7: {
# 937
struct Cyc_List_List*_tmp1A0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp180);
struct Cyc_Absyn_Decl*_tmp1A1=Cyc_Absyn_datatype_decl(s,_tmp17E,_tmp1A0,0,_tmp17F,loc);
if(_tmp168 != 0){const char*_tmp9A5;Cyc_Parse_err(((_tmp9A5="bad attributes on datatype",_tag_dyneither(_tmp9A5,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp9A6;struct Cyc_List_List*_tmp1A4=(_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6->hd=_tmp1A1,((_tmp9A6->tl=0,_tmp9A6)))));_npop_handler(0);return _tmp1A4;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16D;if(_tmp181->tag != 13)goto _LLAA;else{_tmp182=_tmp181->f1;}}_LLA9: {
# 942
struct Cyc_Absyn_Enumdecl*_tmp9A7;struct Cyc_Absyn_Enumdecl*_tmp1A5=(_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7->sc=s,((_tmp9A7->name=_tmp182,((_tmp9A7->fields=0,_tmp9A7)))))));
if(_tmp168 != 0){const char*_tmp9A8;Cyc_Parse_err(((_tmp9A8="bad attributes on enum",_tag_dyneither(_tmp9A8,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9B2;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp9B1;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9B0;struct Cyc_List_List*_tmp9AF;struct Cyc_List_List*_tmp1AB=(_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->hd=((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2->r=(void*)((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=7,((_tmp9B1.f1=_tmp1A5,_tmp9B1)))),_tmp9B0)))),((_tmp9B2->loc=loc,_tmp9B2)))))),((_tmp9AF->tl=0,_tmp9AF)))));_npop_handler(0);return _tmp1AB;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp183=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp16D;if(_tmp183->tag != 14)goto _LLAC;else{_tmp184=_tmp183->f1;}}_LLAB: {
# 948
struct Cyc_Core_Opt*_tmp9B5;struct Cyc_Absyn_Enumdecl*_tmp9B4;struct Cyc_Absyn_Enumdecl*_tmp1AD=(_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4->sc=s,((_tmp9B4->name=Cyc_Parse_gensym_enum(),((_tmp9B4->fields=((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5->v=_tmp184,_tmp9B5)))),_tmp9B4)))))));
if(_tmp168 != 0){const char*_tmp9B6;Cyc_Parse_err(((_tmp9B6="bad attributes on enum",_tag_dyneither(_tmp9B6,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9C0;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp9BF;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9BE;struct Cyc_List_List*_tmp9BD;struct Cyc_List_List*_tmp1B3=(_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD->hd=((_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0->r=(void*)((_tmp9BE=_cycalloc(sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9BF.tag=7,((_tmp9BF.f1=_tmp1AD,_tmp9BF)))),_tmp9BE)))),((_tmp9C0->loc=loc,_tmp9C0)))))),((_tmp9BD->tl=0,_tmp9BD)))));_npop_handler(0);return _tmp1B3;};}_LLAC:;_LLAD:
{const char*_tmp9C1;Cyc_Parse_err(((_tmp9C1="missing declarator",_tag_dyneither(_tmp9C1,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B7=0;_npop_handler(0);return _tmp1B7;};_LL9B:;}else{
# 955
struct Cyc_List_List*_tmp1B8=Cyc_Parse_apply_tmss(mkrgn,_tmp166,_tmp16C,declarators,_tmp168);
if(istypedef){
# 960
if(!exps_empty){
const char*_tmp9C2;Cyc_Parse_err(((_tmp9C2="initializer in typedef declaration",_tag_dyneither(_tmp9C2,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B8);
struct Cyc_List_List*_tmp1BA=decls;_npop_handler(0);return _tmp1BA;};}else{
# 966
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1BB=_tmp1B8;for(0;_tmp1BB != 0;(_tmp1BB=_tmp1BB->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1BC=(struct _tuple15*)_tmp1BB->hd;struct _tuple0*_tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;void*_tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C2;struct _tuple15*_tmp1BD=_tmp1BC;_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;_tmp1C0=_tmp1BD->f3;_tmp1C1=_tmp1BD->f4;_tmp1C2=_tmp1BD->f5;
if(_tmp1C1 != 0){
const char*_tmp9C5;void*_tmp9C4;(_tmp9C4=0,Cyc_Tcutil_warn(loc,((_tmp9C5="bad type params, ignoring",_tag_dyneither(_tmp9C5,sizeof(char),26))),_tag_dyneither(_tmp9C4,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp9C8;void*_tmp9C7;(_tmp9C7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp9C8="unexpected NULL in parse!",_tag_dyneither(_tmp9C8,sizeof(char),26))),_tag_dyneither(_tmp9C7,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C8=Cyc_Absyn_new_vardecl(_tmp1BE,_tmp1C0,_tmp1C7);
_tmp1C8->tq=_tmp1BF;
_tmp1C8->sc=s;
_tmp1C8->attributes=_tmp1C2;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9CE;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9CD;struct Cyc_Absyn_Decl*_tmp9CC;struct Cyc_Absyn_Decl*_tmp1C9=(_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC->r=(void*)((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CD.tag=0,((_tmp9CD.f1=_tmp1C8,_tmp9CD)))),_tmp9CE)))),((_tmp9CC->loc=loc,_tmp9CC)))));
struct Cyc_List_List*_tmp9CF;decls=((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF->hd=_tmp1C9,((_tmp9CF->tl=decls,_tmp9CF))))));};};}}{
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
{const char*_tmp9D4;void*_tmp9D3[2];struct Cyc_String_pa_PrintArg_struct _tmp9D2;struct Cyc_Int_pa_PrintArg_struct _tmp9D1;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9D1.tag=1,((_tmp9D1.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9D2.tag=0,((_tmp9D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9D3[0]=& _tmp9D2,((_tmp9D3[1]=& _tmp9D1,Cyc_aprintf(((_tmp9D4="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9D4,sizeof(char),24))),_tag_dyneither(_tmp9D3,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1022
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1D6=e->r;void*_tmp1D7=_tmp1D6;int _tmp1D9;_LLC5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1D7;if(_tmp1D8->tag != 0)goto _LLC7;else{if(((_tmp1D8->f1).Int_c).tag != 5)goto _LLC7;_tmp1D9=((struct _tuple5)((_tmp1D8->f1).Int_c).val).f2;}}_LLC6:
 return _tmp1D9;_LLC7:;_LLC8:
# 1026
{const char*_tmp9D5;Cyc_Parse_err(((_tmp9D5="expecting integer constant",_tag_dyneither(_tmp9D5,sizeof(char),27))),loc);}
return 0;_LLC4:;}
# 1032
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1DB=e->r;void*_tmp1DC=_tmp1DB;struct _dyneither_ptr _tmp1DE;_LLCA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1DC;if(_tmp1DD->tag != 0)goto _LLCC;else{if(((_tmp1DD->f1).String_c).tag != 8)goto _LLCC;_tmp1DE=(struct _dyneither_ptr)((_tmp1DD->f1).String_c).val;}}_LLCB:
 return _tmp1DE;_LLCC:;_LLCD:
# 1036
{const char*_tmp9D6;Cyc_Parse_err(((_tmp9D6="expecting string constant",_tag_dyneither(_tmp9D6,sizeof(char),26))),loc);}
return _tag_dyneither(0,0,0);_LLC9:;}
# 1042
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1E0=x;int _tmp1E1;char _tmp1E2;long long _tmp1E3;_LLCF: if((_tmp1E0.Int_c).tag != 5)goto _LLD1;_tmp1E1=((struct _tuple5)(_tmp1E0.Int_c).val).f2;_LLD0:
 return(unsigned int)_tmp1E1;_LLD1: if((_tmp1E0.Char_c).tag != 2)goto _LLD3;_tmp1E2=((struct _tuple3)(_tmp1E0.Char_c).val).f2;_LLD2:
 return(unsigned int)_tmp1E2;_LLD3: if((_tmp1E0.LongLong_c).tag != 6)goto _LLD5;_tmp1E3=((struct _tuple6)(_tmp1E0.LongLong_c).val).f2;_LLD4: {
# 1047
unsigned long long y=(unsigned long long)_tmp1E3;
if(y > -1){
const char*_tmp9D7;Cyc_Parse_err(((_tmp9D7="integer constant too large",_tag_dyneither(_tmp9D7,sizeof(char),27))),loc);}
return(unsigned int)_tmp1E3;}_LLD5:;_LLD6:
# 1052
{const char*_tmp9DB;void*_tmp9DA[1];struct Cyc_String_pa_PrintArg_struct _tmp9D9;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9D9.tag=0,((_tmp9D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x)),((_tmp9DA[0]=& _tmp9D9,Cyc_aprintf(((_tmp9DB="expected integer constant but found %s",_tag_dyneither(_tmp9DB,sizeof(char),39))),_tag_dyneither(_tmp9DA,sizeof(void*),1)))))))),loc);}
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
{const char*_tmp9DC;Cyc_Parse_err(((_tmp9DC="cannot mix patterns and expressions in case",_tag_dyneither(_tmp9DC,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLD7:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp201[7]="cnst_t";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 7}};
# 1133
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;union Cyc_Absyn_Cnst _tmp203;_LLED: if((_tmp202.Int_tok).tag != 1)goto _LLEF;_tmp203=(union Cyc_Absyn_Cnst)(_tmp202.Int_tok).val;_LLEE:
# 1137
 yyzzz=_tmp203;
goto _LLEC;_LLEF:;_LLF0:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLEC:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.Int_tok).val=yy1,(((_tmp9DD.Int_tok).tag=1,_tmp9DD)));}static char _tmp206[5]="char";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 5}};
# 1134
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp207=yy1;char _tmp208;_LLF2: if((_tmp207.Char_tok).tag != 2)goto _LLF4;_tmp208=(char)(_tmp207.Char_tok).val;_LLF3:
# 1138
 yyzzz=_tmp208;
goto _LLF1;_LLF4:;_LLF5:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF1:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.Char_tok).val=yy1,(((_tmp9DE.Char_tok).tag=2,_tmp9DE)));}static char _tmp20B[13]="string_t<`H>";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 13}};
# 1135
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct _dyneither_ptr _tmp20D;_LLF7: if((_tmp20C.String_tok).tag != 3)goto _LLF9;_tmp20D=(struct _dyneither_ptr)(_tmp20C.String_tok).val;_LLF8:
# 1139
 yyzzz=_tmp20D;
goto _LLF6;_LLF9:;_LLFA:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF6:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.String_tok).val=yy1,(((_tmp9DF.String_tok).tag=3,_tmp9DF)));}static char _tmp210[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 56}};
# 1138
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct _tuple21*_tmp212;_LLFC: if((_tmp211.YY1).tag != 9)goto _LLFE;_tmp212=(struct _tuple21*)(_tmp211.YY1).val;_LLFD:
# 1142
 yyzzz=_tmp212;
goto _LLFB;_LLFE:;_LLFF:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLFB:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY1).val=yy1,(((_tmp9E0.YY1).tag=9,_tmp9E0)));}static char _tmp215[19]="conref_t<bounds_t>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp215,_tmp215,_tmp215 + 19}};
# 1139
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp216=yy1;union Cyc_Absyn_Constraint*_tmp217;_LL101: if((_tmp216.YY2).tag != 10)goto _LL103;_tmp217=(union Cyc_Absyn_Constraint*)(_tmp216.YY2).val;_LL102:
# 1143
 yyzzz=_tmp217;
goto _LL100;_LL103:;_LL104:
(int)_throw((void*)& Cyc_yyfail_YY2);_LL100:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY2).val=yy1,(((_tmp9E1.YY2).tag=10,_tmp9E1)));}static char _tmp21A[28]="list_t<offsetof_field_t,`H>";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 28}};
# 1140
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;struct Cyc_List_List*_tmp21C;_LL106: if((_tmp21B.YY3).tag != 11)goto _LL108;_tmp21C=(struct Cyc_List_List*)(_tmp21B.YY3).val;_LL107:
# 1144
 yyzzz=_tmp21C;
goto _LL105;_LL108:;_LL109:
(int)_throw((void*)& Cyc_yyfail_YY3);_LL105:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY3).val=yy1,(((_tmp9E2.YY3).tag=11,_tmp9E2)));}static char _tmp21F[6]="exp_t";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 6}};
# 1141
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_Absyn_Exp*_tmp221;_LL10B: if((_tmp220.Exp_tok).tag != 7)goto _LL10D;_tmp221=(struct Cyc_Absyn_Exp*)(_tmp220.Exp_tok).val;_LL10C:
# 1145
 yyzzz=_tmp221;
goto _LL10A;_LL10D:;_LL10E:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LL10A:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.Exp_tok).val=yy1,(((_tmp9E3.Exp_tok).tag=7,_tmp9E3)));}static char _tmp224[17]="list_t<exp_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 17}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp225=yy1;struct Cyc_List_List*_tmp226;_LL110: if((_tmp225.YY4).tag != 12)goto _LL112;_tmp226=(struct Cyc_List_List*)(_tmp225.YY4).val;_LL111:
# 1153
 yyzzz=_tmp226;
goto _LL10F;_LL112:;_LL113:
(int)_throw((void*)& Cyc_yyfail_YY4);_LL10F:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY4).val=yy1,(((_tmp9E4.YY4).tag=12,_tmp9E4)));}static char _tmp229[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 47}};
# 1150
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_List_List*_tmp22B;_LL115: if((_tmp22A.YY5).tag != 13)goto _LL117;_tmp22B=(struct Cyc_List_List*)(_tmp22A.YY5).val;_LL116:
# 1154
 yyzzz=_tmp22B;
goto _LL114;_LL117:;_LL118:
(int)_throw((void*)& Cyc_yyfail_YY5);_LL114:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY5).val=yy1,(((_tmp9E5.YY5).tag=13,_tmp9E5)));}static char _tmp22E[9]="primop_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 9}};
# 1151
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;enum Cyc_Absyn_Primop _tmp230;_LL11A: if((_tmp22F.YY6).tag != 14)goto _LL11C;_tmp230=(enum Cyc_Absyn_Primop)(_tmp22F.YY6).val;_LL11B:
# 1155
 yyzzz=_tmp230;
goto _LL119;_LL11C:;_LL11D:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL119:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY6).val=yy1,(((_tmp9E6.YY6).tag=14,_tmp9E6)));}static char _tmp233[19]="opt_t<primop_t,`H>";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 19}};
# 1152
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Core_Opt*_tmp235;_LL11F: if((_tmp234.YY7).tag != 15)goto _LL121;_tmp235=(struct Cyc_Core_Opt*)(_tmp234.YY7).val;_LL120:
# 1156
 yyzzz=_tmp235;
goto _LL11E;_LL121:;_LL122:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL11E:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY7).val=yy1,(((_tmp9E7.YY7).tag=15,_tmp9E7)));}static char _tmp238[7]="qvar_t";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 7}};
# 1153
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple0*_tmp23A;_LL124: if((_tmp239.QualId_tok).tag != 5)goto _LL126;_tmp23A=(struct _tuple0*)(_tmp239.QualId_tok).val;_LL125:
# 1157
 yyzzz=_tmp23A;
goto _LL123;_LL126:;_LL127:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL123:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9E8;return((_tmp9E8.QualId_tok).val=yy1,(((_tmp9E8.QualId_tok).tag=5,_tmp9E8)));}static char _tmp23D[7]="stmt_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 7}};
# 1156
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct Cyc_Absyn_Stmt*_tmp23F;_LL129: if((_tmp23E.Stmt_tok).tag != 8)goto _LL12B;_tmp23F=(struct Cyc_Absyn_Stmt*)(_tmp23E.Stmt_tok).val;_LL12A:
# 1160
 yyzzz=_tmp23F;
goto _LL128;_LL12B:;_LL12C:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL128:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.Stmt_tok).val=yy1,(((_tmp9E9.Stmt_tok).tag=8,_tmp9E9)));}static char _tmp242[27]="list_t<switch_clause_t,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 27}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp243=yy1;struct Cyc_List_List*_tmp244;_LL12E: if((_tmp243.YY8).tag != 16)goto _LL130;_tmp244=(struct Cyc_List_List*)(_tmp243.YY8).val;_LL12F:
# 1164
 yyzzz=_tmp244;
goto _LL12D;_LL130:;_LL131:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL12D:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY8).val=yy1,(((_tmp9EA.YY8).tag=16,_tmp9EA)));}static char _tmp247[6]="pat_t";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 6}};
# 1161
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_Absyn_Pat*_tmp249;_LL133: if((_tmp248.YY9).tag != 17)goto _LL135;_tmp249=(struct Cyc_Absyn_Pat*)(_tmp248.YY9).val;_LL134:
# 1165
 yyzzz=_tmp249;
goto _LL132;_LL135:;_LL136:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL132:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY9).val=yy1,(((_tmp9EB.YY9).tag=17,_tmp9EB)));}static char _tmp24C[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 28}};
# 1166
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;struct _tuple22*_tmp24E;_LL138: if((_tmp24D.YY10).tag != 18)goto _LL13A;_tmp24E=(struct _tuple22*)(_tmp24D.YY10).val;_LL139:
# 1170
 yyzzz=_tmp24E;
goto _LL137;_LL13A:;_LL13B:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL137:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY10).val=yy1,(((_tmp9EC.YY10).tag=18,_tmp9EC)));}static char _tmp251[17]="list_t<pat_t,`H>";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 17}};
# 1167
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_List_List*_tmp253;_LL13D: if((_tmp252.YY11).tag != 19)goto _LL13F;_tmp253=(struct Cyc_List_List*)(_tmp252.YY11).val;_LL13E:
# 1171
 yyzzz=_tmp253;
goto _LL13C;_LL13F:;_LL140:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL13C:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY11).val=yy1,(((_tmp9ED.YY11).tag=19,_tmp9ED)));}static char _tmp256[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 36}};
# 1168
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct _tuple23*_tmp258;_LL142: if((_tmp257.YY12).tag != 20)goto _LL144;_tmp258=(struct _tuple23*)(_tmp257.YY12).val;_LL143:
# 1172
 yyzzz=_tmp258;
goto _LL141;_LL144:;_LL145:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL141:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.YY12).val=yy1,(((_tmp9EE.YY12).tag=20,_tmp9EE)));}static char _tmp25B[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 47}};
# 1169
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_List_List*_tmp25D;_LL147: if((_tmp25C.YY13).tag != 21)goto _LL149;_tmp25D=(struct Cyc_List_List*)(_tmp25C.YY13).val;_LL148:
# 1173
 yyzzz=_tmp25D;
goto _LL146;_LL149:;_LL14A:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL146:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EF;return((_tmp9EF.YY13).val=yy1,(((_tmp9EF.YY13).tag=21,_tmp9EF)));}static char _tmp260[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp260,_tmp260,_tmp260 + 58}};
# 1170
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp261=yy1;struct _tuple22*_tmp262;_LL14C: if((_tmp261.YY14).tag != 22)goto _LL14E;_tmp262=(struct _tuple22*)(_tmp261.YY14).val;_LL14D:
# 1174
 yyzzz=_tmp262;
goto _LL14B;_LL14E:;_LL14F:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL14B:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY14).val=yy1,(((_tmp9F0.YY14).tag=22,_tmp9F0)));}static char _tmp265[9]="fndecl_t";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 9}};
# 1171
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp266=yy1;struct Cyc_Absyn_Fndecl*_tmp267;_LL151: if((_tmp266.YY15).tag != 23)goto _LL153;_tmp267=(struct Cyc_Absyn_Fndecl*)(_tmp266.YY15).val;_LL152:
# 1175
 yyzzz=_tmp267;
goto _LL150;_LL153:;_LL154:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL150:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY15).val=yy1,(((_tmp9F1.YY15).tag=23,_tmp9F1)));}static char _tmp26A[18]="list_t<decl_t,`H>";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 18}};
# 1172
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26B=yy1;struct Cyc_List_List*_tmp26C;_LL156: if((_tmp26B.YY16).tag != 24)goto _LL158;_tmp26C=(struct Cyc_List_List*)(_tmp26B.YY16).val;_LL157:
# 1176
 yyzzz=_tmp26C;
goto _LL155;_LL158:;_LL159:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL155:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY16).val=yy1,(((_tmp9F2.YY16).tag=24,_tmp9F2)));}static char _tmp26F[12]="decl_spec_t";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 12}};
# 1174
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp270=yy1;struct Cyc_Parse_Declaration_spec _tmp271;_LL15B: if((_tmp270.YY17).tag != 25)goto _LL15D;_tmp271=(struct Cyc_Parse_Declaration_spec)(_tmp270.YY17).val;_LL15C:
# 1178
 yyzzz=_tmp271;
goto _LL15A;_LL15D:;_LL15E:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL15A:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY17).val=yy1,(((_tmp9F3.YY17).tag=25,_tmp9F3)));}static char _tmp274[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 31}};
# 1175
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp275=yy1;struct _tuple11 _tmp276;_LL160: if((_tmp275.YY18).tag != 26)goto _LL162;_tmp276=(struct _tuple11)(_tmp275.YY18).val;_LL161:
# 1179
 yyzzz=_tmp276;
goto _LL15F;_LL162:;_LL163:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL15F:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY18).val=yy1,(((_tmp9F4.YY18).tag=26,_tmp9F4)));}static char _tmp279[23]="declarator_list_t<`yy>";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 23}};
# 1176
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp27A=yy1;struct _tuple12*_tmp27B;_LL165: if((_tmp27A.YY19).tag != 27)goto _LL167;_tmp27B=(struct _tuple12*)(_tmp27A.YY19).val;_LL166:
# 1180
 yyzzz=_tmp27B;
goto _LL164;_LL167:;_LL168:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL164:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9F5;return((_tmp9F5.YY19).val=yy1,(((_tmp9F5.YY19).tag=27,_tmp9F5)));}static char _tmp27E[19]="storage_class_t@`H";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 19}};
# 1177
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp27F=yy1;enum Cyc_Parse_Storage_class*_tmp280;_LL16A: if((_tmp27F.YY20).tag != 28)goto _LL16C;_tmp280=(enum Cyc_Parse_Storage_class*)(_tmp27F.YY20).val;_LL16B:
# 1181
 yyzzz=_tmp280;
goto _LL169;_LL16C:;_LL16D:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL169:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9F6;return((_tmp9F6.YY20).val=yy1,(((_tmp9F6.YY20).tag=28,_tmp9F6)));}static char _tmp283[17]="type_specifier_t";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp283,_tmp283,_tmp283 + 17}};
# 1178
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_Parse_Type_specifier _tmp285;_LL16F: if((_tmp284.YY21).tag != 29)goto _LL171;_tmp285=(struct Cyc_Parse_Type_specifier)(_tmp284.YY21).val;_LL170:
# 1182
 yyzzz=_tmp285;
goto _LL16E;_LL171:;_LL172:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL16E:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY21).val=yy1,(((_tmp9F7.YY21).tag=29,_tmp9F7)));}static char _tmp288[12]="aggr_kind_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 12}};
# 1180
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp289=yy1;enum Cyc_Absyn_AggrKind _tmp28A;_LL174: if((_tmp289.YY22).tag != 30)goto _LL176;_tmp28A=(enum Cyc_Absyn_AggrKind)(_tmp289.YY22).val;_LL175:
# 1184
 yyzzz=_tmp28A;
goto _LL173;_LL176:;_LL177:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL173:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9F8;return((_tmp9F8.YY22).val=yy1,(((_tmp9F8.YY22).tag=30,_tmp9F8)));}static char _tmp28D[8]="tqual_t";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 8}};
# 1181
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp28E=yy1;struct Cyc_Absyn_Tqual _tmp28F;_LL179: if((_tmp28E.YY23).tag != 31)goto _LL17B;_tmp28F=(struct Cyc_Absyn_Tqual)(_tmp28E.YY23).val;_LL17A:
# 1185
 yyzzz=_tmp28F;
goto _LL178;_LL17B:;_LL17C:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL178:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9F9;return((_tmp9F9.YY23).val=yy1,(((_tmp9F9.YY23).tag=31,_tmp9F9)));}static char _tmp292[23]="list_t<aggrfield_t,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 23}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp293=yy1;struct Cyc_List_List*_tmp294;_LL17E: if((_tmp293.YY24).tag != 32)goto _LL180;_tmp294=(struct Cyc_List_List*)(_tmp293.YY24).val;_LL17F:
# 1186
 yyzzz=_tmp294;
goto _LL17D;_LL180:;_LL181:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL17D:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FA;return((_tmp9FA.YY24).val=yy1,(((_tmp9FA.YY24).tag=32,_tmp9FA)));}static char _tmp297[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp297,_tmp297,_tmp297 + 34}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp298=yy1;struct Cyc_List_List*_tmp299;_LL183: if((_tmp298.YY25).tag != 33)goto _LL185;_tmp299=(struct Cyc_List_List*)(_tmp298.YY25).val;_LL184:
# 1187
 yyzzz=_tmp299;
goto _LL182;_LL185:;_LL186:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL182:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FB;return((_tmp9FB.YY25).val=yy1,(((_tmp9FB.YY25).tag=33,_tmp9FB)));}static char _tmp29C[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 33}};
# 1184
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29D=yy1;struct Cyc_List_List*_tmp29E;_LL188: if((_tmp29D.YY26).tag != 34)goto _LL18A;_tmp29E=(struct Cyc_List_List*)(_tmp29D.YY26).val;_LL189:
# 1188
 yyzzz=_tmp29E;
goto _LL187;_LL18A:;_LL18B:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL187:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FC;return((_tmp9FC.YY26).val=yy1,(((_tmp9FC.YY26).tag=34,_tmp9FC)));}static char _tmp2A1[18]="declarator_t<`yy>";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 18}};
# 1185
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp2A2=yy1;struct Cyc_Parse_Declarator _tmp2A3;_LL18D: if((_tmp2A2.YY27).tag != 35)goto _LL18F;_tmp2A3=(struct Cyc_Parse_Declarator)(_tmp2A2.YY27).val;_LL18E:
# 1189
 yyzzz=_tmp2A3;
goto _LL18C;_LL18F:;_LL190:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL18C:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9FD;return((_tmp9FD.YY27).val=yy1,(((_tmp9FD.YY27).tag=35,_tmp9FD)));}static char _tmp2A6[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 45}};
# 1186
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp2A7=yy1;struct _tuple24*_tmp2A8;_LL192: if((_tmp2A7.YY28).tag != 36)goto _LL194;_tmp2A8=(struct _tuple24*)(_tmp2A7.YY28).val;_LL193:
# 1190
 yyzzz=_tmp2A8;
goto _LL191;_LL194:;_LL195:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL191:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9FE;return((_tmp9FE.YY28).val=yy1,(((_tmp9FE.YY28).tag=36,_tmp9FE)));}static char _tmp2AB[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 57}};
# 1187
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_List_List*_tmp2AD;_LL197: if((_tmp2AC.YY29).tag != 37)goto _LL199;_tmp2AD=(struct Cyc_List_List*)(_tmp2AC.YY29).val;_LL198:
# 1191
 yyzzz=_tmp2AD;
goto _LL196;_LL199:;_LL19A:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL196:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FF;return((_tmp9FF.YY29).val=yy1,(((_tmp9FF.YY29).tag=37,_tmp9FF)));}static char _tmp2B0[26]="abstractdeclarator_t<`yy>";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 26}};
# 1188
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp2B1=yy1;struct Cyc_Parse_Abstractdeclarator _tmp2B2;_LL19C: if((_tmp2B1.YY30).tag != 38)goto _LL19E;_tmp2B2=(struct Cyc_Parse_Abstractdeclarator)(_tmp2B1.YY30).val;_LL19D:
# 1192
 yyzzz=_tmp2B2;
goto _LL19B;_LL19E:;_LL19F:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL19B:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmpA00;return((_tmpA00.YY30).val=yy1,(((_tmpA00.YY30).tag=38,_tmpA00)));}static char _tmp2B5[5]="bool";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2B5,_tmp2B5,_tmp2B5 + 5}};
# 1189
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp2B6=yy1;int _tmp2B7;_LL1A1: if((_tmp2B6.YY31).tag != 39)goto _LL1A3;_tmp2B7=(int)(_tmp2B6.YY31).val;_LL1A2:
# 1193
 yyzzz=_tmp2B7;
goto _LL1A0;_LL1A3:;_LL1A4:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL1A0:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmpA01;return((_tmpA01.YY31).val=yy1,(((_tmpA01.YY31).tag=39,_tmpA01)));}static char _tmp2BA[8]="scope_t";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 8}};
# 1190
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp2BB=yy1;enum Cyc_Absyn_Scope _tmp2BC;_LL1A6: if((_tmp2BB.YY32).tag != 40)goto _LL1A8;_tmp2BC=(enum Cyc_Absyn_Scope)(_tmp2BB.YY32).val;_LL1A7:
# 1194
 yyzzz=_tmp2BC;
goto _LL1A5;_LL1A8:;_LL1A9:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL1A5:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY32).val=yy1,(((_tmpA02.YY32).tag=40,_tmpA02)));}static char _tmp2BF[16]="datatypefield_t";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 16}};
# 1191
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp2C0=yy1;struct Cyc_Absyn_Datatypefield*_tmp2C1;_LL1AB: if((_tmp2C0.YY33).tag != 41)goto _LL1AD;_tmp2C1=(struct Cyc_Absyn_Datatypefield*)(_tmp2C0.YY33).val;_LL1AC:
# 1195
 yyzzz=_tmp2C1;
goto _LL1AA;_LL1AD:;_LL1AE:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL1AA:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmpA03;return((_tmpA03.YY33).val=yy1,(((_tmpA03.YY33).tag=41,_tmpA03)));}static char _tmp2C4[27]="list_t<datatypefield_t,`H>";
# 1190 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 27}};
# 1192
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C5=yy1;struct Cyc_List_List*_tmp2C6;_LL1B0: if((_tmp2C5.YY34).tag != 42)goto _LL1B2;_tmp2C6=(struct Cyc_List_List*)(_tmp2C5.YY34).val;_LL1B1:
# 1196
 yyzzz=_tmp2C6;
goto _LL1AF;_LL1B2:;_LL1B3:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL1AF:;}
# 1200
return yyzzz;}
# 1202
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA04;return((_tmpA04.YY34).val=yy1,(((_tmpA04.YY34).tag=42,_tmpA04)));}static char _tmp2C9[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1191 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 41}};
# 1193
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2CA=yy1;struct _tuple25 _tmp2CB;_LL1B5: if((_tmp2CA.YY35).tag != 43)goto _LL1B7;_tmp2CB=(struct _tuple25)(_tmp2CA.YY35).val;_LL1B6:
# 1197
 yyzzz=_tmp2CB;
goto _LL1B4;_LL1B7:;_LL1B8:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL1B4:;}
# 1201
return yyzzz;}
# 1203
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){union Cyc_YYSTYPE _tmpA05;return((_tmpA05.YY35).val=yy1,(((_tmpA05.YY35).tag=43,_tmpA05)));}static char _tmp2CE[17]="list_t<var_t,`H>";
# 1192 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 17}};
# 1194
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_List_List*_tmp2D0;_LL1BA: if((_tmp2CF.YY36).tag != 44)goto _LL1BC;_tmp2D0=(struct Cyc_List_List*)(_tmp2CF.YY36).val;_LL1BB:
# 1198
 yyzzz=_tmp2D0;
goto _LL1B9;_LL1BC:;_LL1BD:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B9:;}
# 1202
return yyzzz;}
# 1204
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA06;return((_tmpA06.YY36).val=yy1,(((_tmpA06.YY36).tag=44,_tmpA06)));}static char _tmp2D3[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1193 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 31}};
# 1195
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2D4=yy1;struct _tuple8*_tmp2D5;_LL1BF: if((_tmp2D4.YY37).tag != 45)goto _LL1C1;_tmp2D5=(struct _tuple8*)(_tmp2D4.YY37).val;_LL1C0:
# 1199
 yyzzz=_tmp2D5;
goto _LL1BE;_LL1C1:;_LL1C2:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1BE:;}
# 1203
return yyzzz;}
# 1205
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmpA07;return((_tmpA07.YY37).val=yy1,(((_tmpA07.YY37).tag=45,_tmpA07)));}static char _tmp2D8[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1194 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2D8,_tmp2D8,_tmp2D8 + 42}};
# 1196
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D9=yy1;struct Cyc_List_List*_tmp2DA;_LL1C4: if((_tmp2D9.YY38).tag != 46)goto _LL1C6;_tmp2DA=(struct Cyc_List_List*)(_tmp2D9.YY38).val;_LL1C5:
# 1200
 yyzzz=_tmp2DA;
goto _LL1C3;_LL1C6:;_LL1C7:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1C3:;}
# 1204
return yyzzz;}
# 1206
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA08;return((_tmpA08.YY38).val=yy1,(((_tmpA08.YY38).tag=46,_tmpA08)));}static char _tmp2DD[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1195 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 115}};
# 1197
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2DE=yy1;struct _tuple26*_tmp2DF;_LL1C9: if((_tmp2DE.YY39).tag != 47)goto _LL1CB;_tmp2DF=(struct _tuple26*)(_tmp2DE.YY39).val;_LL1CA:
# 1201
 yyzzz=_tmp2DF;
goto _LL1C8;_LL1CB:;_LL1CC:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1C8:;}
# 1205
return yyzzz;}
# 1207
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){union Cyc_YYSTYPE _tmpA09;return((_tmpA09.YY39).val=yy1,(((_tmpA09.YY39).tag=47,_tmpA09)));}static char _tmp2E2[18]="list_t<type_t,`H>";
# 1196 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 18}};
# 1198
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E3=yy1;struct Cyc_List_List*_tmp2E4;_LL1CE: if((_tmp2E3.YY40).tag != 48)goto _LL1D0;_tmp2E4=(struct Cyc_List_List*)(_tmp2E3.YY40).val;_LL1CF:
# 1202
 yyzzz=_tmp2E4;
goto _LL1CD;_LL1D0:;_LL1D1:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1CD:;}
# 1206
return yyzzz;}
# 1208
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0A;return((_tmpA0A.YY40).val=yy1,(((_tmpA0A.YY40).tag=48,_tmpA0A)));}static char _tmp2E7[24]="list_t<designator_t,`H>";
# 1198 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 24}};
# 1200
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E8=yy1;struct Cyc_List_List*_tmp2E9;_LL1D3: if((_tmp2E8.YY41).tag != 49)goto _LL1D5;_tmp2E9=(struct Cyc_List_List*)(_tmp2E8.YY41).val;_LL1D4:
# 1204
 yyzzz=_tmp2E9;
goto _LL1D2;_LL1D5:;_LL1D6:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1D2:;}
# 1208
return yyzzz;}
# 1210
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0B;return((_tmpA0B.YY41).val=yy1,(((_tmpA0B.YY41).tag=49,_tmpA0B)));}static char _tmp2EC[13]="designator_t";
# 1199 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 13}};
# 1201
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2ED=yy1;void*_tmp2EE;_LL1D8: if((_tmp2ED.YY42).tag != 50)goto _LL1DA;_tmp2EE=(void*)(_tmp2ED.YY42).val;_LL1D9:
# 1205
 yyzzz=_tmp2EE;
goto _LL1D7;_LL1DA:;_LL1DB:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1D7:;}
# 1209
return yyzzz;}
# 1211
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmpA0C;return((_tmpA0C.YY42).val=yy1,(((_tmpA0C.YY42).tag=50,_tmpA0C)));}static char _tmp2F1[7]="kind_t";
# 1200 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 7}};
# 1202
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2F2=yy1;struct Cyc_Absyn_Kind*_tmp2F3;_LL1DD: if((_tmp2F2.YY43).tag != 51)goto _LL1DF;_tmp2F3=(struct Cyc_Absyn_Kind*)(_tmp2F2.YY43).val;_LL1DE:
# 1206
 yyzzz=_tmp2F3;
goto _LL1DC;_LL1DF:;_LL1E0:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1DC:;}
# 1210
return yyzzz;}
# 1212
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmpA0D;return((_tmpA0D.YY43).val=yy1,(((_tmpA0D.YY43).tag=51,_tmpA0D)));}static char _tmp2F6[7]="type_t";
# 1201 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,_tmp2F6 + 7}};
# 1203
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2F7=yy1;void*_tmp2F8;_LL1E2: if((_tmp2F7.YY44).tag != 52)goto _LL1E4;_tmp2F8=(void*)(_tmp2F7.YY44).val;_LL1E3:
# 1207
 yyzzz=_tmp2F8;
goto _LL1E1;_LL1E4:;_LL1E5:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1E1:;}
# 1211
return yyzzz;}
# 1213
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmpA0E;return((_tmpA0E.YY44).val=yy1,(((_tmpA0E.YY44).tag=52,_tmpA0E)));}static char _tmp2FB[23]="list_t<attribute_t,`H>";
# 1202 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2FB,_tmp2FB,_tmp2FB + 23}};
# 1204
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2FC=yy1;struct Cyc_List_List*_tmp2FD;_LL1E7: if((_tmp2FC.YY45).tag != 53)goto _LL1E9;_tmp2FD=(struct Cyc_List_List*)(_tmp2FC.YY45).val;_LL1E8:
# 1208
 yyzzz=_tmp2FD;
goto _LL1E6;_LL1E9:;_LL1EA:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1E6:;}
# 1212
return yyzzz;}
# 1214
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA0F;return((_tmpA0F.YY45).val=yy1,(((_tmpA0F.YY45).tag=53,_tmpA0F)));}static char _tmp300[12]="attribute_t";
# 1203 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp300,_tmp300,_tmp300 + 12}};
# 1205
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp301=yy1;void*_tmp302;_LL1EC: if((_tmp301.YY46).tag != 54)goto _LL1EE;_tmp302=(void*)(_tmp301.YY46).val;_LL1ED:
# 1209
 yyzzz=_tmp302;
goto _LL1EB;_LL1EE:;_LL1EF:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1EB:;}
# 1213
return yyzzz;}
# 1215
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmpA10;return((_tmpA10.YY46).val=yy1,(((_tmpA10.YY46).tag=54,_tmpA10)));}static char _tmp305[12]="enumfield_t";
# 1204 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp305,_tmp305,_tmp305 + 12}};
# 1206
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp306=yy1;struct Cyc_Absyn_Enumfield*_tmp307;_LL1F1: if((_tmp306.YY47).tag != 55)goto _LL1F3;_tmp307=(struct Cyc_Absyn_Enumfield*)(_tmp306.YY47).val;_LL1F2:
# 1210
 yyzzz=_tmp307;
goto _LL1F0;_LL1F3:;_LL1F4:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1F0:;}
# 1214
return yyzzz;}
# 1216
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmpA11;return((_tmpA11.YY47).val=yy1,(((_tmpA11.YY47).tag=55,_tmpA11)));}static char _tmp30A[23]="list_t<enumfield_t,`H>";
# 1205 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp30A,_tmp30A,_tmp30A + 23}};
# 1207
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp30B=yy1;struct Cyc_List_List*_tmp30C;_LL1F6: if((_tmp30B.YY48).tag != 56)goto _LL1F8;_tmp30C=(struct Cyc_List_List*)(_tmp30B.YY48).val;_LL1F7:
# 1211
 yyzzz=_tmp30C;
goto _LL1F5;_LL1F8:;_LL1F9:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1F5:;}
# 1215
return yyzzz;}
# 1217
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA12;return((_tmpA12.YY48).val=yy1,(((_tmpA12.YY48).tag=56,_tmpA12)));}static char _tmp30F[11]="type_opt_t";
# 1206 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp30F,_tmp30F,_tmp30F + 11}};
# 1208
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp310=yy1;void*_tmp311;_LL1FB: if((_tmp310.YY49).tag != 57)goto _LL1FD;_tmp311=(void*)(_tmp310.YY49).val;_LL1FC:
# 1212
 yyzzz=_tmp311;
goto _LL1FA;_LL1FD:;_LL1FE:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1FA:;}
# 1216
return yyzzz;}
# 1218
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmpA13;return((_tmpA13.YY49).val=yy1,(((_tmpA13.YY49).tag=57,_tmpA13)));}static char _tmp314[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1207 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp314,_tmp314,_tmp314 + 31}};
# 1209
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp315=yy1;struct Cyc_List_List*_tmp316;_LL200: if((_tmp315.YY50).tag != 58)goto _LL202;_tmp316=(struct Cyc_List_List*)(_tmp315.YY50).val;_LL201:
# 1213
 yyzzz=_tmp316;
goto _LL1FF;_LL202:;_LL203:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1FF:;}
# 1217
return yyzzz;}
# 1219
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA14;return((_tmpA14.YY50).val=yy1,(((_tmpA14.YY50).tag=58,_tmpA14)));}static char _tmp319[15]="conref_t<bool>";
# 1208 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp319,_tmp319,_tmp319 + 15}};
# 1210
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp31A=yy1;union Cyc_Absyn_Constraint*_tmp31B;_LL205: if((_tmp31A.YY51).tag != 59)goto _LL207;_tmp31B=(union Cyc_Absyn_Constraint*)(_tmp31A.YY51).val;_LL206:
# 1214
 yyzzz=_tmp31B;
goto _LL204;_LL207:;_LL208:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL204:;}
# 1218
return yyzzz;}
# 1220
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmpA15;return((_tmpA15.YY51).val=yy1,(((_tmpA15.YY51).tag=59,_tmpA15)));}static char _tmp31E[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1209 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp31E,_tmp31E,_tmp31E + 45}};
# 1211
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp31F=yy1;struct Cyc_List_List*_tmp320;_LL20A: if((_tmp31F.YY52).tag != 60)goto _LL20C;_tmp320=(struct Cyc_List_List*)(_tmp31F.YY52).val;_LL20B:
# 1215
 yyzzz=_tmp320;
goto _LL209;_LL20C:;_LL20D:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL209:;}
# 1219
return yyzzz;}
# 1221
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA16;return((_tmpA16.YY52).val=yy1,(((_tmpA16.YY52).tag=60,_tmpA16)));}static char _tmp323[20]="pointer_qual_t<`yy>";
# 1210 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp323,_tmp323,_tmp323 + 20}};
# 1212
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp324=yy1;void*_tmp325;_LL20F: if((_tmp324.YY53).tag != 61)goto _LL211;_tmp325=(void*)(_tmp324.YY53).val;_LL210:
# 1216
 yyzzz=_tmp325;
goto _LL20E;_LL211:;_LL212:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL20E:;}
# 1220
return yyzzz;}
# 1222
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA17;return((_tmpA17.YY53).val=yy1,(((_tmpA17.YY53).tag=61,_tmpA17)));}static char _tmp328[21]="pointer_quals_t<`yy>";
# 1211 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp328,_tmp328,_tmp328 + 21}};
# 1213
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp329=yy1;struct Cyc_List_List*_tmp32A;_LL214: if((_tmp329.YY54).tag != 62)goto _LL216;_tmp32A=(struct Cyc_List_List*)(_tmp329.YY54).val;_LL215:
# 1217
 yyzzz=_tmp32A;
goto _LL213;_LL216:;_LL217:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL213:;}
# 1221
return yyzzz;}
# 1223
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA18;return((_tmpA18.YY54).val=yy1,(((_tmpA18.YY54).tag=62,_tmpA18)));}static char _tmp32D[21]="$(bool,string_t<`H>)";
# 1212 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp32D,_tmp32D,_tmp32D + 21}};
# 1214
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp32E=yy1;struct _tuple20 _tmp32F;_LL219: if((_tmp32E.Asm_tok).tag != 6)goto _LL21B;_tmp32F=(struct _tuple20)(_tmp32E.Asm_tok).val;_LL21A:
# 1218
 yyzzz=_tmp32F;
goto _LL218;_LL21B:;_LL21C:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL218:;}
# 1222
return yyzzz;}
# 1224
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmpA19;return((_tmpA19.Asm_tok).val=yy1,(((_tmpA19.Asm_tok).tag=6,_tmpA19)));}static char _tmp332[10]="exp_opt_t";
# 1213 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp332,_tmp332,_tmp332 + 10}};
# 1215
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp333=yy1;struct Cyc_Absyn_Exp*_tmp334;_LL21E: if((_tmp333.YY55).tag != 63)goto _LL220;_tmp334=(struct Cyc_Absyn_Exp*)(_tmp333.YY55).val;_LL21F:
# 1219
 yyzzz=_tmp334;
goto _LL21D;_LL220:;_LL221:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL21D:;}
# 1223
return yyzzz;}
# 1225
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmpA1A;return((_tmpA1A.YY55).val=yy1,(((_tmpA1A.YY55).tag=63,_tmpA1A)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1237
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1241
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmpA1B;return(_tmpA1B.timestamp=0,((_tmpA1B.first_line=0,((_tmpA1B.first_column=0,((_tmpA1B.last_line=0,((_tmpA1B.last_column=0,_tmpA1B)))))))));}
# 1244
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1255 "parse.y"
static short Cyc_yytranslate[376]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,147,2,2,131,145,142,2,128,129,136,139,124,143,133,144,2,2,2,2,2,2,2,2,2,2,132,121,126,125,127,138,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,134,2,135,141,130,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,122,140,123,146,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};static char _tmp337[2]="$";static char _tmp338[6]="error";static char _tmp339[12]="$undefined.";static char _tmp33A[5]="AUTO";static char _tmp33B[9]="REGISTER";static char _tmp33C[7]="STATIC";static char _tmp33D[7]="EXTERN";static char _tmp33E[8]="TYPEDEF";static char _tmp33F[5]="VOID";static char _tmp340[5]="CHAR";static char _tmp341[6]="SHORT";static char _tmp342[4]="INT";static char _tmp343[5]="LONG";static char _tmp344[6]="FLOAT";static char _tmp345[7]="DOUBLE";static char _tmp346[7]="SIGNED";static char _tmp347[9]="UNSIGNED";static char _tmp348[6]="CONST";static char _tmp349[9]="VOLATILE";static char _tmp34A[9]="RESTRICT";static char _tmp34B[7]="STRUCT";static char _tmp34C[6]="UNION";static char _tmp34D[5]="CASE";static char _tmp34E[8]="DEFAULT";static char _tmp34F[7]="INLINE";static char _tmp350[7]="SIZEOF";static char _tmp351[9]="OFFSETOF";static char _tmp352[3]="IF";static char _tmp353[5]="ELSE";static char _tmp354[7]="SWITCH";static char _tmp355[6]="WHILE";static char _tmp356[3]="DO";static char _tmp357[4]="FOR";static char _tmp358[5]="GOTO";static char _tmp359[9]="CONTINUE";static char _tmp35A[6]="BREAK";static char _tmp35B[7]="RETURN";static char _tmp35C[5]="ENUM";static char _tmp35D[7]="TYPEOF";static char _tmp35E[16]="BUILTIN_VA_LIST";static char _tmp35F[8]="NULL_kw";static char _tmp360[4]="LET";static char _tmp361[6]="THROW";static char _tmp362[4]="TRY";static char _tmp363[6]="CATCH";static char _tmp364[7]="EXPORT";static char _tmp365[4]="NEW";static char _tmp366[9]="ABSTRACT";static char _tmp367[9]="FALLTHRU";static char _tmp368[6]="USING";static char _tmp369[10]="NAMESPACE";static char _tmp36A[9]="DATATYPE";static char _tmp36B[7]="MALLOC";static char _tmp36C[8]="RMALLOC";static char _tmp36D[15]="RMALLOC_INLINE";static char _tmp36E[7]="CALLOC";static char _tmp36F[8]="RCALLOC";static char _tmp370[5]="SWAP";static char _tmp371[9]="REGION_T";static char _tmp372[6]="TAG_T";static char _tmp373[7]="REGION";static char _tmp374[5]="RNEW";static char _tmp375[8]="REGIONS";static char _tmp376[13]="RESET_REGION";static char _tmp377[7]="PORTON";static char _tmp378[8]="PORTOFF";static char _tmp379[12]="DYNREGION_T";static char _tmp37A[8]="NUMELTS";static char _tmp37B[8]="VALUEOF";static char _tmp37C[10]="VALUEOF_T";static char _tmp37D[9]="TAGCHECK";static char _tmp37E[13]="NUMELTS_QUAL";static char _tmp37F[10]="THIN_QUAL";static char _tmp380[9]="FAT_QUAL";static char _tmp381[13]="NOTNULL_QUAL";static char _tmp382[14]="NULLABLE_QUAL";static char _tmp383[14]="REQUIRES_QUAL";static char _tmp384[13]="ENSURES_QUAL";static char _tmp385[12]="REGION_QUAL";static char _tmp386[16]="NOZEROTERM_QUAL";static char _tmp387[14]="ZEROTERM_QUAL";static char _tmp388[12]="TAGGED_QUAL";static char _tmp389[16]="EXTENSIBLE_QUAL";static char _tmp38A[15]="RESETABLE_QUAL";static char _tmp38B[7]="PTR_OP";static char _tmp38C[7]="INC_OP";static char _tmp38D[7]="DEC_OP";static char _tmp38E[8]="LEFT_OP";static char _tmp38F[9]="RIGHT_OP";static char _tmp390[6]="LE_OP";static char _tmp391[6]="GE_OP";static char _tmp392[6]="EQ_OP";static char _tmp393[6]="NE_OP";static char _tmp394[7]="AND_OP";static char _tmp395[6]="OR_OP";static char _tmp396[11]="MUL_ASSIGN";static char _tmp397[11]="DIV_ASSIGN";static char _tmp398[11]="MOD_ASSIGN";static char _tmp399[11]="ADD_ASSIGN";static char _tmp39A[11]="SUB_ASSIGN";static char _tmp39B[12]="LEFT_ASSIGN";static char _tmp39C[13]="RIGHT_ASSIGN";static char _tmp39D[11]="AND_ASSIGN";static char _tmp39E[11]="XOR_ASSIGN";static char _tmp39F[10]="OR_ASSIGN";static char _tmp3A0[9]="ELLIPSIS";static char _tmp3A1[11]="LEFT_RIGHT";static char _tmp3A2[12]="COLON_COLON";static char _tmp3A3[11]="IDENTIFIER";static char _tmp3A4[17]="INTEGER_CONSTANT";static char _tmp3A5[7]="STRING";static char _tmp3A6[8]="WSTRING";static char _tmp3A7[19]="CHARACTER_CONSTANT";static char _tmp3A8[20]="WCHARACTER_CONSTANT";static char _tmp3A9[18]="FLOATING_CONSTANT";static char _tmp3AA[9]="TYPE_VAR";static char _tmp3AB[13]="TYPEDEF_NAME";static char _tmp3AC[16]="QUAL_IDENTIFIER";static char _tmp3AD[18]="QUAL_TYPEDEF_NAME";static char _tmp3AE[10]="ATTRIBUTE";static char _tmp3AF[4]="ASM";static char _tmp3B0[4]="';'";static char _tmp3B1[4]="'{'";static char _tmp3B2[4]="'}'";static char _tmp3B3[4]="','";static char _tmp3B4[4]="'='";static char _tmp3B5[4]="'<'";static char _tmp3B6[4]="'>'";static char _tmp3B7[4]="'('";static char _tmp3B8[4]="')'";static char _tmp3B9[4]="'_'";static char _tmp3BA[4]="'$'";static char _tmp3BB[4]="':'";static char _tmp3BC[4]="'.'";static char _tmp3BD[4]="'['";static char _tmp3BE[4]="']'";static char _tmp3BF[4]="'*'";static char _tmp3C0[4]="'@'";static char _tmp3C1[4]="'?'";static char _tmp3C2[4]="'+'";static char _tmp3C3[4]="'|'";static char _tmp3C4[4]="'^'";static char _tmp3C5[4]="'&'";static char _tmp3C6[4]="'-'";static char _tmp3C7[4]="'/'";static char _tmp3C8[4]="'%'";static char _tmp3C9[4]="'~'";static char _tmp3CA[4]="'!'";static char _tmp3CB[5]="prog";static char _tmp3CC[17]="translation_unit";static char _tmp3CD[16]="extern_c_action";static char _tmp3CE[13]="end_extern_c";static char _tmp3CF[12]="export_list";static char _tmp3D0[19]="export_list_values";static char _tmp3D1[21]="external_declaration";static char _tmp3D2[15]="optional_comma";static char _tmp3D3[20]="function_definition";static char _tmp3D4[21]="function_definition2";static char _tmp3D5[13]="using_action";static char _tmp3D6[15]="unusing_action";static char _tmp3D7[17]="namespace_action";static char _tmp3D8[19]="unnamespace_action";static char _tmp3D9[12]="declaration";static char _tmp3DA[19]="resetable_qual_opt";static char _tmp3DB[17]="declaration_list";static char _tmp3DC[23]="declaration_specifiers";static char _tmp3DD[24]="storage_class_specifier";static char _tmp3DE[15]="attributes_opt";static char _tmp3DF[11]="attributes";static char _tmp3E0[15]="attribute_list";static char _tmp3E1[10]="attribute";static char _tmp3E2[15]="type_specifier";static char _tmp3E3[25]="type_specifier_notypedef";static char _tmp3E4[5]="kind";static char _tmp3E5[15]="type_qualifier";static char _tmp3E6[15]="enum_specifier";static char _tmp3E7[11]="enum_field";static char _tmp3E8[22]="enum_declaration_list";static char _tmp3E9[26]="struct_or_union_specifier";static char _tmp3EA[16]="type_params_opt";static char _tmp3EB[16]="struct_or_union";static char _tmp3EC[24]="struct_declaration_list";static char _tmp3ED[25]="struct_declaration_list0";static char _tmp3EE[21]="init_declarator_list";static char _tmp3EF[22]="init_declarator_list0";static char _tmp3F0[16]="init_declarator";static char _tmp3F1[19]="struct_declaration";static char _tmp3F2[25]="specifier_qualifier_list";static char _tmp3F3[35]="notypedef_specifier_qualifier_list";static char _tmp3F4[23]="struct_declarator_list";static char _tmp3F5[24]="struct_declarator_list0";static char _tmp3F6[18]="struct_declarator";static char _tmp3F7[20]="requires_clause_opt";static char _tmp3F8[19]="ensures_clause_opt";static char _tmp3F9[19]="datatype_specifier";static char _tmp3FA[14]="qual_datatype";static char _tmp3FB[19]="datatypefield_list";static char _tmp3FC[20]="datatypefield_scope";static char _tmp3FD[14]="datatypefield";static char _tmp3FE[11]="declarator";static char _tmp3FF[23]="declarator_withtypedef";static char _tmp400[18]="direct_declarator";static char _tmp401[30]="direct_declarator_withtypedef";static char _tmp402[8]="pointer";static char _tmp403[12]="one_pointer";static char _tmp404[14]="pointer_quals";static char _tmp405[13]="pointer_qual";static char _tmp406[23]="pointer_null_and_bound";static char _tmp407[14]="pointer_bound";static char _tmp408[18]="zeroterm_qual_opt";static char _tmp409[8]="rgn_opt";static char _tmp40A[11]="tqual_list";static char _tmp40B[20]="parameter_type_list";static char _tmp40C[9]="type_var";static char _tmp40D[16]="optional_effect";static char _tmp40E[19]="optional_rgn_order";static char _tmp40F[10]="rgn_order";static char _tmp410[16]="optional_inject";static char _tmp411[11]="effect_set";static char _tmp412[14]="atomic_effect";static char _tmp413[11]="region_set";static char _tmp414[15]="parameter_list";static char _tmp415[22]="parameter_declaration";static char _tmp416[16]="identifier_list";static char _tmp417[17]="identifier_list0";static char _tmp418[12]="initializer";static char _tmp419[18]="array_initializer";static char _tmp41A[17]="initializer_list";static char _tmp41B[12]="designation";static char _tmp41C[16]="designator_list";static char _tmp41D[11]="designator";static char _tmp41E[10]="type_name";static char _tmp41F[14]="any_type_name";static char _tmp420[15]="type_name_list";static char _tmp421[20]="abstract_declarator";static char _tmp422[27]="direct_abstract_declarator";static char _tmp423[10]="statement";static char _tmp424[18]="labeled_statement";static char _tmp425[21]="expression_statement";static char _tmp426[19]="compound_statement";static char _tmp427[16]="block_item_list";static char _tmp428[20]="selection_statement";static char _tmp429[15]="switch_clauses";static char _tmp42A[20]="iteration_statement";static char _tmp42B[15]="jump_statement";static char _tmp42C[12]="exp_pattern";static char _tmp42D[20]="conditional_pattern";static char _tmp42E[19]="logical_or_pattern";static char _tmp42F[20]="logical_and_pattern";static char _tmp430[21]="inclusive_or_pattern";static char _tmp431[21]="exclusive_or_pattern";static char _tmp432[12]="and_pattern";static char _tmp433[17]="equality_pattern";static char _tmp434[19]="relational_pattern";static char _tmp435[14]="shift_pattern";static char _tmp436[17]="additive_pattern";static char _tmp437[23]="multiplicative_pattern";static char _tmp438[13]="cast_pattern";static char _tmp439[14]="unary_pattern";static char _tmp43A[16]="postfix_pattern";static char _tmp43B[16]="primary_pattern";static char _tmp43C[8]="pattern";static char _tmp43D[19]="tuple_pattern_list";static char _tmp43E[20]="tuple_pattern_list0";static char _tmp43F[14]="field_pattern";static char _tmp440[19]="field_pattern_list";static char _tmp441[20]="field_pattern_list0";static char _tmp442[11]="expression";static char _tmp443[22]="assignment_expression";static char _tmp444[20]="assignment_operator";static char _tmp445[23]="conditional_expression";static char _tmp446[20]="constant_expression";static char _tmp447[22]="logical_or_expression";static char _tmp448[23]="logical_and_expression";static char _tmp449[24]="inclusive_or_expression";static char _tmp44A[24]="exclusive_or_expression";static char _tmp44B[15]="and_expression";static char _tmp44C[20]="equality_expression";static char _tmp44D[22]="relational_expression";static char _tmp44E[17]="shift_expression";static char _tmp44F[20]="additive_expression";static char _tmp450[26]="multiplicative_expression";static char _tmp451[16]="cast_expression";static char _tmp452[17]="unary_expression";static char _tmp453[15]="unary_operator";static char _tmp454[19]="postfix_expression";static char _tmp455[17]="field_expression";static char _tmp456[19]="primary_expression";static char _tmp457[25]="argument_expression_list";static char _tmp458[26]="argument_expression_list0";static char _tmp459[9]="constant";static char _tmp45A[20]="qual_opt_identifier";static char _tmp45B[17]="qual_opt_typedef";static char _tmp45C[18]="struct_union_name";static char _tmp45D[11]="field_name";static char _tmp45E[12]="right_angle";
# 1609 "parse.y"
static struct _dyneither_ptr Cyc_yytname[296]={{_tmp337,_tmp337,_tmp337 + 2},{_tmp338,_tmp338,_tmp338 + 6},{_tmp339,_tmp339,_tmp339 + 12},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 7},{_tmp33D,_tmp33D,_tmp33D + 7},{_tmp33E,_tmp33E,_tmp33E + 8},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 5},{_tmp341,_tmp341,_tmp341 + 6},{_tmp342,_tmp342,_tmp342 + 4},{_tmp343,_tmp343,_tmp343 + 5},{_tmp344,_tmp344,_tmp344 + 6},{_tmp345,_tmp345,_tmp345 + 7},{_tmp346,_tmp346,_tmp346 + 7},{_tmp347,_tmp347,_tmp347 + 9},{_tmp348,_tmp348,_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 9},{_tmp34A,_tmp34A,_tmp34A + 9},{_tmp34B,_tmp34B,_tmp34B + 7},{_tmp34C,_tmp34C,_tmp34C + 6},{_tmp34D,_tmp34D,_tmp34D + 5},{_tmp34E,_tmp34E,_tmp34E + 8},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 7},{_tmp351,_tmp351,_tmp351 + 9},{_tmp352,_tmp352,_tmp352 + 3},{_tmp353,_tmp353,_tmp353 + 5},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 6},{_tmp356,_tmp356,_tmp356 + 3},{_tmp357,_tmp357,_tmp357 + 4},{_tmp358,_tmp358,_tmp358 + 5},{_tmp359,_tmp359,_tmp359 + 9},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 7},{_tmp35C,_tmp35C,_tmp35C + 5},{_tmp35D,_tmp35D,_tmp35D + 7},{_tmp35E,_tmp35E,_tmp35E + 16},{_tmp35F,_tmp35F,_tmp35F + 8},{_tmp360,_tmp360,_tmp360 + 4},{_tmp361,_tmp361,_tmp361 + 6},{_tmp362,_tmp362,_tmp362 + 4},{_tmp363,_tmp363,_tmp363 + 6},{_tmp364,_tmp364,_tmp364 + 7},{_tmp365,_tmp365,_tmp365 + 4},{_tmp366,_tmp366,_tmp366 + 9},{_tmp367,_tmp367,_tmp367 + 9},{_tmp368,_tmp368,_tmp368 + 6},{_tmp369,_tmp369,_tmp369 + 10},{_tmp36A,_tmp36A,_tmp36A + 9},{_tmp36B,_tmp36B,_tmp36B + 7},{_tmp36C,_tmp36C,_tmp36C + 8},{_tmp36D,_tmp36D,_tmp36D + 15},{_tmp36E,_tmp36E,_tmp36E + 7},{_tmp36F,_tmp36F,_tmp36F + 8},{_tmp370,_tmp370,_tmp370 + 5},{_tmp371,_tmp371,_tmp371 + 9},{_tmp372,_tmp372,_tmp372 + 6},{_tmp373,_tmp373,_tmp373 + 7},{_tmp374,_tmp374,_tmp374 + 5},{_tmp375,_tmp375,_tmp375 + 8},{_tmp376,_tmp376,_tmp376 + 13},{_tmp377,_tmp377,_tmp377 + 7},{_tmp378,_tmp378,_tmp378 + 8},{_tmp379,_tmp379,_tmp379 + 12},{_tmp37A,_tmp37A,_tmp37A + 8},{_tmp37B,_tmp37B,_tmp37B + 8},{_tmp37C,_tmp37C,_tmp37C + 10},{_tmp37D,_tmp37D,_tmp37D + 9},{_tmp37E,_tmp37E,_tmp37E + 13},{_tmp37F,_tmp37F,_tmp37F + 10},{_tmp380,_tmp380,_tmp380 + 9},{_tmp381,_tmp381,_tmp381 + 13},{_tmp382,_tmp382,_tmp382 + 14},{_tmp383,_tmp383,_tmp383 + 14},{_tmp384,_tmp384,_tmp384 + 13},{_tmp385,_tmp385,_tmp385 + 12},{_tmp386,_tmp386,_tmp386 + 16},{_tmp387,_tmp387,_tmp387 + 14},{_tmp388,_tmp388,_tmp388 + 12},{_tmp389,_tmp389,_tmp389 + 16},{_tmp38A,_tmp38A,_tmp38A + 15},{_tmp38B,_tmp38B,_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 7},{_tmp38D,_tmp38D,_tmp38D + 7},{_tmp38E,_tmp38E,_tmp38E + 8},{_tmp38F,_tmp38F,_tmp38F + 9},{_tmp390,_tmp390,_tmp390 + 6},{_tmp391,_tmp391,_tmp391 + 6},{_tmp392,_tmp392,_tmp392 + 6},{_tmp393,_tmp393,_tmp393 + 6},{_tmp394,_tmp394,_tmp394 + 7},{_tmp395,_tmp395,_tmp395 + 6},{_tmp396,_tmp396,_tmp396 + 11},{_tmp397,_tmp397,_tmp397 + 11},{_tmp398,_tmp398,_tmp398 + 11},{_tmp399,_tmp399,_tmp399 + 11},{_tmp39A,_tmp39A,_tmp39A + 11},{_tmp39B,_tmp39B,_tmp39B + 12},{_tmp39C,_tmp39C,_tmp39C + 13},{_tmp39D,_tmp39D,_tmp39D + 11},{_tmp39E,_tmp39E,_tmp39E + 11},{_tmp39F,_tmp39F,_tmp39F + 10},{_tmp3A0,_tmp3A0,_tmp3A0 + 9},{_tmp3A1,_tmp3A1,_tmp3A1 + 11},{_tmp3A2,_tmp3A2,_tmp3A2 + 12},{_tmp3A3,_tmp3A3,_tmp3A3 + 11},{_tmp3A4,_tmp3A4,_tmp3A4 + 17},{_tmp3A5,_tmp3A5,_tmp3A5 + 7},{_tmp3A6,_tmp3A6,_tmp3A6 + 8},{_tmp3A7,_tmp3A7,_tmp3A7 + 19},{_tmp3A8,_tmp3A8,_tmp3A8 + 20},{_tmp3A9,_tmp3A9,_tmp3A9 + 18},{_tmp3AA,_tmp3AA,_tmp3AA + 9},{_tmp3AB,_tmp3AB,_tmp3AB + 13},{_tmp3AC,_tmp3AC,_tmp3AC + 16},{_tmp3AD,_tmp3AD,_tmp3AD + 18},{_tmp3AE,_tmp3AE,_tmp3AE + 10},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 4},{_tmp3B1,_tmp3B1,_tmp3B1 + 4},{_tmp3B2,_tmp3B2,_tmp3B2 + 4},{_tmp3B3,_tmp3B3,_tmp3B3 + 4},{_tmp3B4,_tmp3B4,_tmp3B4 + 4},{_tmp3B5,_tmp3B5,_tmp3B5 + 4},{_tmp3B6,_tmp3B6,_tmp3B6 + 4},{_tmp3B7,_tmp3B7,_tmp3B7 + 4},{_tmp3B8,_tmp3B8,_tmp3B8 + 4},{_tmp3B9,_tmp3B9,_tmp3B9 + 4},{_tmp3BA,_tmp3BA,_tmp3BA + 4},{_tmp3BB,_tmp3BB,_tmp3BB + 4},{_tmp3BC,_tmp3BC,_tmp3BC + 4},{_tmp3BD,_tmp3BD,_tmp3BD + 4},{_tmp3BE,_tmp3BE,_tmp3BE + 4},{_tmp3BF,_tmp3BF,_tmp3BF + 4},{_tmp3C0,_tmp3C0,_tmp3C0 + 4},{_tmp3C1,_tmp3C1,_tmp3C1 + 4},{_tmp3C2,_tmp3C2,_tmp3C2 + 4},{_tmp3C3,_tmp3C3,_tmp3C3 + 4},{_tmp3C4,_tmp3C4,_tmp3C4 + 4},{_tmp3C5,_tmp3C5,_tmp3C5 + 4},{_tmp3C6,_tmp3C6,_tmp3C6 + 4},{_tmp3C7,_tmp3C7,_tmp3C7 + 4},{_tmp3C8,_tmp3C8,_tmp3C8 + 4},{_tmp3C9,_tmp3C9,_tmp3C9 + 4},{_tmp3CA,_tmp3CA,_tmp3CA + 4},{_tmp3CB,_tmp3CB,_tmp3CB + 5},{_tmp3CC,_tmp3CC,_tmp3CC + 17},{_tmp3CD,_tmp3CD,_tmp3CD + 16},{_tmp3CE,_tmp3CE,_tmp3CE + 13},{_tmp3CF,_tmp3CF,_tmp3CF + 12},{_tmp3D0,_tmp3D0,_tmp3D0 + 19},{_tmp3D1,_tmp3D1,_tmp3D1 + 21},{_tmp3D2,_tmp3D2,_tmp3D2 + 15},{_tmp3D3,_tmp3D3,_tmp3D3 + 20},{_tmp3D4,_tmp3D4,_tmp3D4 + 21},{_tmp3D5,_tmp3D5,_tmp3D5 + 13},{_tmp3D6,_tmp3D6,_tmp3D6 + 15},{_tmp3D7,_tmp3D7,_tmp3D7 + 17},{_tmp3D8,_tmp3D8,_tmp3D8 + 19},{_tmp3D9,_tmp3D9,_tmp3D9 + 12},{_tmp3DA,_tmp3DA,_tmp3DA + 19},{_tmp3DB,_tmp3DB,_tmp3DB + 17},{_tmp3DC,_tmp3DC,_tmp3DC + 23},{_tmp3DD,_tmp3DD,_tmp3DD + 24},{_tmp3DE,_tmp3DE,_tmp3DE + 15},{_tmp3DF,_tmp3DF,_tmp3DF + 11},{_tmp3E0,_tmp3E0,_tmp3E0 + 15},{_tmp3E1,_tmp3E1,_tmp3E1 + 10},{_tmp3E2,_tmp3E2,_tmp3E2 + 15},{_tmp3E3,_tmp3E3,_tmp3E3 + 25},{_tmp3E4,_tmp3E4,_tmp3E4 + 5},{_tmp3E5,_tmp3E5,_tmp3E5 + 15},{_tmp3E6,_tmp3E6,_tmp3E6 + 15},{_tmp3E7,_tmp3E7,_tmp3E7 + 11},{_tmp3E8,_tmp3E8,_tmp3E8 + 22},{_tmp3E9,_tmp3E9,_tmp3E9 + 26},{_tmp3EA,_tmp3EA,_tmp3EA + 16},{_tmp3EB,_tmp3EB,_tmp3EB + 16},{_tmp3EC,_tmp3EC,_tmp3EC + 24},{_tmp3ED,_tmp3ED,_tmp3ED + 25},{_tmp3EE,_tmp3EE,_tmp3EE + 21},{_tmp3EF,_tmp3EF,_tmp3EF + 22},{_tmp3F0,_tmp3F0,_tmp3F0 + 16},{_tmp3F1,_tmp3F1,_tmp3F1 + 19},{_tmp3F2,_tmp3F2,_tmp3F2 + 25},{_tmp3F3,_tmp3F3,_tmp3F3 + 35},{_tmp3F4,_tmp3F4,_tmp3F4 + 23},{_tmp3F5,_tmp3F5,_tmp3F5 + 24},{_tmp3F6,_tmp3F6,_tmp3F6 + 18},{_tmp3F7,_tmp3F7,_tmp3F7 + 20},{_tmp3F8,_tmp3F8,_tmp3F8 + 19},{_tmp3F9,_tmp3F9,_tmp3F9 + 19},{_tmp3FA,_tmp3FA,_tmp3FA + 14},{_tmp3FB,_tmp3FB,_tmp3FB + 19},{_tmp3FC,_tmp3FC,_tmp3FC + 20},{_tmp3FD,_tmp3FD,_tmp3FD + 14},{_tmp3FE,_tmp3FE,_tmp3FE + 11},{_tmp3FF,_tmp3FF,_tmp3FF + 23},{_tmp400,_tmp400,_tmp400 + 18},{_tmp401,_tmp401,_tmp401 + 30},{_tmp402,_tmp402,_tmp402 + 8},{_tmp403,_tmp403,_tmp403 + 12},{_tmp404,_tmp404,_tmp404 + 14},{_tmp405,_tmp405,_tmp405 + 13},{_tmp406,_tmp406,_tmp406 + 23},{_tmp407,_tmp407,_tmp407 + 14},{_tmp408,_tmp408,_tmp408 + 18},{_tmp409,_tmp409,_tmp409 + 8},{_tmp40A,_tmp40A,_tmp40A + 11},{_tmp40B,_tmp40B,_tmp40B + 20},{_tmp40C,_tmp40C,_tmp40C + 9},{_tmp40D,_tmp40D,_tmp40D + 16},{_tmp40E,_tmp40E,_tmp40E + 19},{_tmp40F,_tmp40F,_tmp40F + 10},{_tmp410,_tmp410,_tmp410 + 16},{_tmp411,_tmp411,_tmp411 + 11},{_tmp412,_tmp412,_tmp412 + 14},{_tmp413,_tmp413,_tmp413 + 11},{_tmp414,_tmp414,_tmp414 + 15},{_tmp415,_tmp415,_tmp415 + 22},{_tmp416,_tmp416,_tmp416 + 16},{_tmp417,_tmp417,_tmp417 + 17},{_tmp418,_tmp418,_tmp418 + 12},{_tmp419,_tmp419,_tmp419 + 18},{_tmp41A,_tmp41A,_tmp41A + 17},{_tmp41B,_tmp41B,_tmp41B + 12},{_tmp41C,_tmp41C,_tmp41C + 16},{_tmp41D,_tmp41D,_tmp41D + 11},{_tmp41E,_tmp41E,_tmp41E + 10},{_tmp41F,_tmp41F,_tmp41F + 14},{_tmp420,_tmp420,_tmp420 + 15},{_tmp421,_tmp421,_tmp421 + 20},{_tmp422,_tmp422,_tmp422 + 27},{_tmp423,_tmp423,_tmp423 + 10},{_tmp424,_tmp424,_tmp424 + 18},{_tmp425,_tmp425,_tmp425 + 21},{_tmp426,_tmp426,_tmp426 + 19},{_tmp427,_tmp427,_tmp427 + 16},{_tmp428,_tmp428,_tmp428 + 20},{_tmp429,_tmp429,_tmp429 + 15},{_tmp42A,_tmp42A,_tmp42A + 20},{_tmp42B,_tmp42B,_tmp42B + 15},{_tmp42C,_tmp42C,_tmp42C + 12},{_tmp42D,_tmp42D,_tmp42D + 20},{_tmp42E,_tmp42E,_tmp42E + 19},{_tmp42F,_tmp42F,_tmp42F + 20},{_tmp430,_tmp430,_tmp430 + 21},{_tmp431,_tmp431,_tmp431 + 21},{_tmp432,_tmp432,_tmp432 + 12},{_tmp433,_tmp433,_tmp433 + 17},{_tmp434,_tmp434,_tmp434 + 19},{_tmp435,_tmp435,_tmp435 + 14},{_tmp436,_tmp436,_tmp436 + 17},{_tmp437,_tmp437,_tmp437 + 23},{_tmp438,_tmp438,_tmp438 + 13},{_tmp439,_tmp439,_tmp439 + 14},{_tmp43A,_tmp43A,_tmp43A + 16},{_tmp43B,_tmp43B,_tmp43B + 16},{_tmp43C,_tmp43C,_tmp43C + 8},{_tmp43D,_tmp43D,_tmp43D + 19},{_tmp43E,_tmp43E,_tmp43E + 20},{_tmp43F,_tmp43F,_tmp43F + 14},{_tmp440,_tmp440,_tmp440 + 19},{_tmp441,_tmp441,_tmp441 + 20},{_tmp442,_tmp442,_tmp442 + 11},{_tmp443,_tmp443,_tmp443 + 22},{_tmp444,_tmp444,_tmp444 + 20},{_tmp445,_tmp445,_tmp445 + 23},{_tmp446,_tmp446,_tmp446 + 20},{_tmp447,_tmp447,_tmp447 + 22},{_tmp448,_tmp448,_tmp448 + 23},{_tmp449,_tmp449,_tmp449 + 24},{_tmp44A,_tmp44A,_tmp44A + 24},{_tmp44B,_tmp44B,_tmp44B + 15},{_tmp44C,_tmp44C,_tmp44C + 20},{_tmp44D,_tmp44D,_tmp44D + 22},{_tmp44E,_tmp44E,_tmp44E + 17},{_tmp44F,_tmp44F,_tmp44F + 20},{_tmp450,_tmp450,_tmp450 + 26},{_tmp451,_tmp451,_tmp451 + 16},{_tmp452,_tmp452,_tmp452 + 17},{_tmp453,_tmp453,_tmp453 + 15},{_tmp454,_tmp454,_tmp454 + 19},{_tmp455,_tmp455,_tmp455 + 17},{_tmp456,_tmp456,_tmp456 + 19},{_tmp457,_tmp457,_tmp457 + 25},{_tmp458,_tmp458,_tmp458 + 26},{_tmp459,_tmp459,_tmp459 + 9},{_tmp45A,_tmp45A,_tmp45A + 20},{_tmp45B,_tmp45B,_tmp45B + 17},{_tmp45C,_tmp45C,_tmp45C + 18},{_tmp45D,_tmp45D,_tmp45D + 11},{_tmp45E,_tmp45E,_tmp45E + 12}};
# 1665
static short Cyc_yyr1[530]={0,148,149,149,149,149,149,149,149,149,149,149,150,151,152,152,153,153,153,154,154,154,155,155,156,156,156,156,157,157,158,159,160,161,162,162,162,162,162,162,162,163,163,164,164,165,165,165,165,165,165,165,165,165,165,166,166,166,166,166,166,166,167,167,168,169,169,170,170,170,170,171,171,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,173,174,174,174,175,175,175,176,176,177,177,177,178,178,178,178,178,179,179,180,180,181,181,182,182,183,184,184,185,185,186,187,187,187,187,187,187,188,188,188,188,188,188,189,190,190,191,191,191,191,192,192,193,193,194,194,194,195,195,196,196,196,196,197,197,197,198,198,199,199,200,200,201,201,201,201,201,201,201,201,201,201,202,202,202,202,202,202,202,202,202,202,202,203,203,204,205,205,206,206,206,206,206,206,206,206,207,207,207,208,208,209,209,209,210,210,210,211,211,212,212,212,212,213,213,214,214,215,215,216,216,217,217,218,218,219,219,219,219,220,220,221,221,222,222,222,223,224,224,225,225,226,226,226,226,226,227,227,227,227,228,228,229,229,230,230,231,231,232,232,232,232,232,233,233,234,234,234,235,235,235,235,235,235,235,235,235,235,235,236,236,236,236,236,236,236,237,238,238,239,239,240,240,240,240,240,240,240,240,241,241,241,241,241,241,242,242,242,242,242,242,243,243,243,243,243,243,243,243,243,243,243,243,243,243,244,244,244,244,244,244,244,244,245,246,246,247,247,248,248,249,249,250,250,251,251,252,252,252,253,253,253,253,253,254,254,254,255,255,255,256,256,256,256,257,257,258,258,258,258,258,258,259,260,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,262,262,262,263,263,264,264,265,265,265,266,266,267,267,268,268,268,269,269,269,269,269,269,269,269,269,269,269,270,270,270,270,270,270,270,271,272,272,273,273,274,274,275,275,276,276,277,277,277,278,278,278,278,278,279,279,279,280,280,280,281,281,281,281,282,282,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,284,284,284,285,285,285,285,285,285,285,285,285,285,285,286,286,286,286,287,287,287,287,287,287,287,287,287,287,288,289,289,290,290,290,290,290,291,291,292,292,293,293,294,294,295,295};
# 1721
static short Cyc_yyr2[530]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1777
static short Cyc_yydefact[1077]={0,21,55,56,57,58,60,73,74,75,76,77,78,79,80,81,99,100,101,117,118,51,0,0,85,0,61,0,0,155,92,96,0,0,0,0,0,0,42,520,221,522,521,523,0,0,88,0,207,207,206,1,0,0,19,0,0,20,0,0,45,53,47,71,49,82,83,0,86,0,41,166,0,191,194,87,170,115,59,58,52,0,103,0,519,0,520,515,516,517,518,115,0,382,0,0,0,0,244,0,384,385,30,32,0,0,0,0,0,0,0,156,0,0,0,0,0,0,0,204,205,0,2,0,0,0,0,0,34,0,123,124,126,46,54,48,50,119,524,525,115,115,41,43,41,0,23,0,223,0,179,167,192,0,198,199,202,203,0,201,200,212,194,0,72,59,107,0,105,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,504,505,483,0,0,0,0,0,486,484,485,0,410,412,426,434,436,438,440,442,444,447,452,455,458,462,0,464,487,503,502,520,393,0,0,0,0,0,394,392,37,0,0,115,0,0,0,133,129,131,264,266,0,0,9,10,0,0,115,526,527,222,98,0,0,171,89,242,0,239,0,0,3,0,5,0,0,0,35,0,0,41,23,0,120,121,146,114,0,153,0,0,0,0,0,0,0,0,0,0,0,0,520,295,297,0,305,299,0,303,287,288,289,0,290,291,292,0,44,23,126,22,24,271,0,229,245,0,0,225,223,0,209,0,0,0,214,62,213,195,0,108,104,0,0,0,472,0,428,462,0,429,430,0,0,0,0,0,0,0,0,0,0,465,466,41,0,0,0,468,469,467,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,416,417,418,419,420,421,422,423,424,425,415,0,470,0,493,494,0,0,0,507,0,115,386,0,0,407,520,0,0,0,0,260,403,408,0,405,0,383,400,401,0,398,0,246,0,0,0,0,267,0,237,134,139,135,137,130,132,223,0,273,265,274,529,528,0,91,95,0,93,97,113,68,67,0,65,172,223,241,168,273,243,180,181,0,90,208,13,0,31,0,33,0,39,0,0,125,127,248,247,23,25,110,122,0,0,0,141,142,149,0,115,115,161,0,0,0,0,0,520,0,41,0,334,335,336,0,0,338,0,0,41,0,306,300,126,304,298,296,26,0,178,230,0,0,0,236,224,231,149,0,0,0,225,177,211,210,173,209,0,0,215,63,116,109,433,106,102,0,0,0,520,249,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,506,513,0,512,411,435,0,437,439,441,443,445,446,450,451,448,449,453,454,456,457,459,460,461,414,413,492,489,0,491,0,0,0,396,397,263,0,404,258,261,391,0,259,388,0,395,36,0,389,0,268,0,140,136,138,0,225,0,209,0,275,0,223,0,286,270,0,115,0,0,0,133,0,115,0,223,0,190,169,240,0,7,0,4,6,0,0,27,0,145,128,146,0,0,144,225,154,163,162,0,0,157,0,0,0,313,0,0,0,0,0,0,333,337,0,0,0,0,301,294,41,28,272,223,0,233,0,0,151,226,0,149,229,217,174,196,197,215,193,471,0,0,250,0,255,474,0,0,0,0,0,479,482,0,0,0,0,0,463,509,0,0,490,488,0,0,0,262,406,409,399,402,390,269,238,149,0,276,277,209,0,0,225,209,0,94,225,520,0,64,66,0,182,0,0,225,0,209,0,0,8,0,0,143,0,147,119,152,164,161,161,0,0,0,0,0,0,0,0,0,0,0,0,0,313,339,0,0,302,29,225,0,234,232,0,175,0,151,225,0,216,499,0,498,0,251,256,0,0,0,0,0,431,432,492,491,511,495,0,514,427,508,510,0,387,151,149,279,285,149,0,278,209,119,0,69,183,189,149,0,188,184,209,15,0,16,0,38,0,0,0,160,159,307,313,0,0,0,0,0,0,341,342,344,346,348,350,352,354,357,362,365,368,372,374,380,381,0,41,310,319,0,0,0,0,0,0,0,0,0,0,340,293,219,235,0,227,176,218,223,473,0,0,257,475,476,0,0,481,480,496,0,282,151,151,149,280,0,0,151,149,185,14,17,0,0,148,112,0,0,0,313,0,378,0,0,375,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,376,313,0,321,0,0,0,329,0,0,0,0,0,0,312,0,0,225,501,500,0,0,0,497,281,284,151,111,0,186,151,18,40,165,308,309,0,0,0,0,313,315,345,0,347,349,351,353,355,356,360,361,358,359,363,364,366,367,369,370,371,0,314,320,322,323,0,331,330,0,325,0,0,0,150,228,220,0,0,0,0,283,0,187,311,377,0,373,316,0,41,324,332,326,327,0,253,252,477,0,70,0,343,313,317,328,0,379,318,478,0,0,0};
# 1888
static short Cyc_yydefgoto[148]={1074,51,52,466,648,850,53,307,54,291,55,468,56,470,57,58,144,59,60,539,232,452,453,233,63,246,234,65,166,167,66,164,67,268,269,129,130,131,270,235,434,484,485,486,660,801,68,69,665,666,667,70,487,71,457,72,73,161,162,74,119,535,322,704,621,75,622,529,695,521,525,526,428,315,254,97,98,551,476,552,408,409,410,236,308,309,623,440,294,295,296,297,298,299,782,300,301,866,867,868,869,870,871,872,873,874,875,876,877,878,879,880,881,411,419,420,412,413,414,302,197,390,198,544,199,200,201,202,203,204,205,206,207,208,209,210,211,212,808,213,568,569,214,215,77,851,415,444};
# 1906
static short Cyc_yypact[1077]={2880,- -32768,- -32768,- -32768,- -32768,- 28,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3583,243,- 23,- -32768,1040,- -32768,260,6,- -32768,17,36,62,117,127,168,255,258,- -32768,- -32768,222,- -32768,- -32768,- -32768,217,268,264,287,312,312,- -32768,- -32768,336,2741,- -32768,385,617,- -32768,328,821,3583,3583,3583,- -32768,3583,- -32768,- -32768,466,- -32768,260,3497,91,63,574,990,- -32768,- -32768,334,373,410,- -32768,260,405,6600,- -32768,4165,- 45,- -32768,- -32768,- -32768,- -32768,334,6600,- -32768,408,439,4165,436,438,443,- -32768,- 12,- -32768,- -32768,3733,3733,2741,2741,3733,6600,539,- -32768,227,449,200,450,227,4108,6600,- -32768,- -32768,3018,- -32768,2741,3018,2741,3018,66,- -32768,464,467,- -32768,3368,- -32768,- -32768,- -32768,- -32768,4108,- -32768,- -32768,334,139,1739,- -32768,3497,821,479,3733,3658,4934,- -32768,91,- -32768,500,- -32768,- -32768,- -32768,- -32768,502,- -32768,- -32768,78,990,3733,- -32768,- -32768,483,520,537,260,6892,540,6600,6746,575,592,600,633,635,642,660,676,680,6989,6989,- -32768,- -32768,- -32768,2314,688,7037,7037,7037,- -32768,- -32768,- -32768,266,- -32768,- -32768,- 16,638,683,679,686,722,89,749,511,138,- -32768,1141,7037,391,- 39,- -32768,703,110,- -32768,4165,107,2564,273,2842,743,- -32768,- -32768,746,6600,334,2842,731,3883,4108,4183,4108,380,- -32768,- 38,- 38,- -32768,- -32768,43,306,334,- -32768,- -32768,- -32768,- -32768,55,738,- -32768,- -32768,126,324,- -32768,757,771,- -32768,784,- -32768,786,609,803,- -32768,200,5032,3497,479,799,4108,- -32768,433,809,260,811,807,170,813,4290,816,828,825,827,5130,4290,165,817,805,- -32768,- -32768,823,1884,1884,821,1884,- -32768,- -32768,- -32768,832,- -32768,- -32768,- -32768,257,- -32768,479,835,- -32768,- -32768,830,45,854,- -32768,46,837,831,292,838,782,829,6600,3733,- -32768,851,- -32768,- -32768,45,260,- -32768,6600,850,2314,- -32768,4108,- -32768,- -32768,4388,- -32768,881,6600,6600,6600,6600,6600,6600,6600,4108,760,2314,- -32768,- -32768,2029,868,372,6600,- -32768,- -32768,- -32768,6600,- -32768,7037,6600,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,6600,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6600,- -32768,227,- -32768,- -32768,5228,227,6600,- -32768,873,334,- -32768,852,875,- -32768,95,227,6600,4165,241,- -32768,- -32768,- -32768,877,879,872,- -32768,- -32768,- -32768,880,882,4165,- -32768,348,2564,883,3733,- -32768,891,892,- -32768,4183,4183,4183,- -32768,- -32768,3101,5326,350,- -32768,192,- -32768,- -32768,46,- -32768,- -32768,3733,- -32768,- -32768,895,- -32768,890,893,900,- -32768,1398,- -32768,214,632,- -32768,- -32768,- -32768,4108,- -32768,- -32768,- -32768,2602,- -32768,2741,- -32768,2741,- -32768,920,902,- -32768,- -32768,- -32768,- -32768,479,- -32768,- -32768,- -32768,607,6600,909,908,- -32768,- 19,333,334,334,834,6600,6600,906,916,6600,910,1013,1244,923,- -32768,- -32768,- -32768,572,1001,- -32768,5424,6600,2174,2454,- -32768,- -32768,3368,- -32768,- -32768,- -32768,- -32768,3733,- -32768,- -32768,4108,918,3958,- -32768,- -32768,911,971,46,922,4033,831,- -32768,- -32768,- -32768,- -32768,782,928,67,814,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,930,936,958,939,- -32768,- -32768,741,5032,943,949,950,953,954,399,955,956,673,960,959,6844,- -32768,- -32768,961,967,- -32768,638,73,683,679,686,722,89,89,749,749,749,749,511,511,138,138,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,963,- -32768,133,3733,4836,4108,- -32768,- -32768,944,- -32768,- -32768,- -32768,- -32768,2703,- -32768,- -32768,3194,- -32768,- -32768,970,- -32768,102,- -32768,4108,- -32768,- -32768,- -32768,965,831,968,782,948,192,3733,3808,5522,- -32768,- -32768,- 38,334,6698,969,55,3232,974,334,3733,3658,5620,- -32768,214,- -32768,977,- -32768,2741,- -32768,- -32768,972,988,- -32768,787,- -32768,- -32768,433,976,6600,- -32768,831,- -32768,- -32768,- -32768,982,260,597,409,415,6600,853,419,4290,979,5718,5816,641,- -32768,- -32768,986,991,980,457,1884,- -32768,3497,- -32768,830,993,3733,- -32768,994,46,1034,- -32768,992,971,148,- -32768,- -32768,- -32768,- -32768,814,- -32768,996,181,995,- -32768,4500,- -32768,- -32768,6600,6600,1097,6600,6746,- -32768,- -32768,227,227,996,997,4612,- -32768,- -32768,6600,6600,- -32768,- -32768,45,759,1015,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,971,999,- -32768,- -32768,782,45,1000,831,782,989,- -32768,831,1006,1002,- -32768,- -32768,1005,- -32768,45,1007,831,1009,782,1008,239,- -32768,6600,1004,- -32768,6600,- -32768,4108,- -32768,1012,53,834,4290,1019,1017,3049,1003,1021,4290,6600,5914,659,6012,666,6110,853,- -32768,1026,1029,- -32768,- -32768,831,132,- -32768,- -32768,1023,- -32768,1041,1034,831,4108,- -32768,- -32768,613,- -32768,6600,- -32768,- -32768,5032,1030,1032,1027,1042,- -32768,881,1035,1036,- -32768,- -32768,763,- -32768,- -32768,- -32768,- -32768,4836,- -32768,1034,971,- -32768,- -32768,971,1038,- -32768,782,4108,1049,- -32768,- -32768,- -32768,971,1043,- -32768,- -32768,782,- -32768,1046,674,508,- -32768,1045,1052,4108,- -32768,- -32768,1149,853,1056,7134,1051,2454,7037,1048,- -32768,- 3,- -32768,1088,1047,748,808,97,824,612,344,- -32768,- -32768,- -32768,- -32768,1092,7037,2029,- -32768,- -32768,516,4290,541,6208,4290,551,6306,6404,691,1063,- -32768,- -32768,- -32768,- -32768,6600,1064,- -32768,- -32768,993,- -32768,311,293,- -32768,- -32768,- -32768,4108,1164,- -32768,- -32768,- -32768,4724,- -32768,1034,1034,971,- -32768,1068,1070,1034,971,- -32768,- -32768,- -32768,539,1071,- -32768,- -32768,559,4290,1072,853,2314,- -32768,4108,1067,- -32768,1594,7037,6600,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,6600,- -32768,853,1076,- -32768,4290,4290,563,- -32768,4290,4290,576,4290,580,6502,- -32768,1074,46,831,- -32768,- -32768,2454,1075,1077,- -32768,- -32768,- -32768,1034,- -32768,1090,- -32768,1034,- -32768,- -32768,- -32768,- -32768,- -32768,1085,1080,1087,7037,853,- -32768,638,305,683,679,679,722,89,89,749,749,749,749,511,511,138,138,- -32768,- -32768,- -32768,361,- -32768,- -32768,- -32768,- -32768,4290,- -32768,- -32768,4290,- -32768,4290,4290,603,- -32768,- -32768,- -32768,1091,778,1084,4108,- -32768,1093,- -32768,- -32768,996,181,- -32768,- -32768,6600,1594,- -32768,- -32768,- -32768,- -32768,4290,- -32768,- -32768,- -32768,1094,- -32768,648,- -32768,853,- -32768,- -32768,1095,- -32768,- -32768,- -32768,1200,1217,- -32768};
# 2017
static short Cyc_yypgoto[148]={- -32768,286,- -32768,- -32768,- -32768,290,- -32768,- 220,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,22,- -32768,- 92,33,- -32768,- -32768,0,585,- -32768,88,- 184,1109,13,- -32768,- -32768,- 128,- -32768,550,1191,- 724,- -32768,- -32768,- -32768,964,962,565,417,- -32768,- -32768,573,- 68,- 735,- -32768,- -32768,144,- -32768,- -32768,59,- 215,1160,- 389,259,- -32768,1073,- -32768,- -32768,1184,- 396,- -32768,531,- 131,- 159,- 126,- 67,291,548,579,- 416,- 471,- 109,- 419,- 112,- -32768,- 253,- 150,- 542,- 325,- -32768,858,14,- 18,- 135,- 105,- 358,80,- -32768,- -32768,- 15,- 285,- -32768,- 631,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,225,1044,- -32768,667,855,- -32768,143,503,- -32768,- 168,- 356,- 149,- 348,- 341,- 329,912,- 322,- 347,- 338,- 346,- 335,- 176,619,496,931,230,- -32768,- 351,- -32768,245,553,- 64,18,- 111,162};
# 2039
static short Cyc_yytable[7282]={61,247,323,139,333,247,511,512,253,514,553,571,475,64,354,355,356,313,577,578,573,61,314,336,337,583,584,631,325,579,580,581,582,574,64,391,316,456,585,586,266,329,576,645,594,114,139,479,855,432,441,602,692,61,80,146,731,658,663,664,61,61,61,218,61,564,64,398,903,644,61,150,450,64,64,64,- 245,64,359,- 245,626,219,78,64,517,140,237,238,62,442,241,944,143,133,134,135,918,136,399,644,626,443,689,145,115,83,61,61,522,62,228,645,696,659,103,923,229,267,132,64,64,61,360,61,61,61,61,655,243,304,439,441,61,441,64,945,64,64,64,64,700,62,61,104,61,64,740,459,62,62,62,150,62,524,143,64,682,64,62,896,543,40,105,451,292,145,303,446,523,518,442,39,442,249,261,293,- 158,145,367,368,42,824,443,106,988,989,952,953,461,531,993,45,262,40,62,62,702,357,542,587,588,589,351,218,305,727,443,461,321,62,44,62,62,62,62,369,370,147,218,148,62,219,402,954,955,149,196,- 526,744,936,62,739,62,431,39,221,219,403,107,543,638,443,41,42,43,429,433,432,432,432,99,478,242,108,455,1045,520,357,653,1047,437,900,48,49,50,163,61,638,729,312,100,443,273,553,375,19,20,- 223,39,64,- 223,592,376,377,524,595,506,42,303,244,807,61,61,507,61,601,109,245,493,145,710,494,538,771,64,64,1000,64,39,111,217,44,1005,292,292,543,292,42,627,779,628,224,540,305,293,293,629,293,45,112,100,352,152,44,1026,244,48,49,50,122,640,100,641,245,546,113,547,39,642,833,61,39,513,837,62,41,42,43,498,561,42,563,849,64,505,81,604,847,39,524,423,116,292,1052,406,407,39,42,516,62,62,357,62,293,813,42,905,44,127,724,357,639,239,240,461,358,45,357,553,794,445,253,416,447,48,49,50,256,615,257,258,259,260,312,854,117,530,357,461,244,982,686,757,1068,461,984,504,245,632,357,357,431,431,431,118,448,1072,1053,62,757,630,39,922,401,433,433,433,418,462,41,42,43,927,463,418,637,812,643,121,694,163,482,730,100,699,100,61,100,61,612,61,519,357,352,100,392,393,394,436,64,960,64,654,64,437,357,559,541,961,962,352,543,746,1054,438,- 12,357,747,687,61,688,566,748,572,813,759,123,124,436,61,760,458,64,61,437,761,48,49,50,395,165,676,64,357,396,397,64,169,716,762,488,684,145,357,524,143,222,429,777,357,596,39,145,357,778,979,145,223,783,41,42,43,703,76,62,742,62,225,62,826,909,482,226,796,696,483,818,819,227,48,49,50,804,639,39,82,812,248,101,250,102,357,41,42,43,263,793,62,137,685,461,264,813,461,772,809,1006,62,76,965,306,62,543,1012,1013,1008,76,326,820,821,1018,1019,76,732,1014,1015,1016,1017,1009,1010,138,255,141,1020,1021,76,630,1011,319,803,320,429,357,603,168,668,669,931,101,672,357,220,677,327,643,966,611,39,61,101,373,683,318,100,374,41,42,43,1004,76,76,64,328,138,812,357,100,76,332,100,968,795,797,831,76,357,76,76,76,76,972,836,252,462,61,839,61,357,997,942,272,274,1030,679,845,438,357,64,76,64,357,139,271,338,357,1033,292,964,303,1035,48,49,50,252,458,39,703,293,775,145,339,776,168,41,42,43,44,357,340,899,471,361,1059,543,472,482,62,904,125,126,39,488,906,48,49,50,907,934,41,42,43,958,647,685,649,959,650,719,393,394,455,341,789,342,919,357,437,920,477,1067,343,101,62,101,62,101,925,1071,424,981,890,907,101,357,1022,1023,1024,893,344,331,357,334,334,449,751,929,983,430,930,435,84,395,348,349,345,460,720,397,346,334,334,334,977,365,366,357,353,76,786,788,363,524,537,362,460,400,490,1051,364,495,334,16,17,18,271,737,371,372,477,663,664,554,555,556,557,558,76,560,618,619,620,421,100,990,422,100,567,859,994,426,570,533,534,886,708,709,139,454,39,87,185,186,88,89,90,780,781,42,941,168,464,590,828,829,61,1066,916,917,347,948,949,189,827,591,465,39,488,64,567,950,951,1061,357,41,42,43,292,467,834,469,852,956,957,488,1040,482,488,293,473,857,858,843,480,48,49,50,985,887,889,39,892,489,895,491,766,492,500,509,42,809,625,496,128,61,499,508,501,334,502,45,598,510,1001,908,1002,515,64,48,49,50,265,101,520,528,536,292,527,532,967,443,44,971,62,545,101,359,293,101,334,599,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,565,1041,597,606,252,600,607,608,882,610,221,460,609,567,460,614,567,616,998,617,633,634,76,252,76,635,76,636,100,334,252,651,652,656,62,657,970,670,460,974,976,671,661,662,460,673,674,678,680,690,658,1028,1029,693,697,1031,1032,61,1034,477,701,1063,705,706,153,154,155,156,157,707,64,158,159,160,- 526,711,712,713,252,292,714,715,733,84,352,722,745,717,718,252,293,1007,721,725,726,728,738,741,252,768,743,755,765,767,477,334,758,770,773,1025,784,790,792,1055,800,791,1056,312,1057,1058,798,723,802,1037,810,816,830,838,853,822,1042,832,835,840,841,750,85,842,884,844,754,846,1069,856,860,62,848,885,764,861,897,86,87,898,901,88,89,90,912,902,42,924,910,101,911,91,101,914,915,913,921,92,928,93,94,926,567,932,933,95,935,937,940,943,946,96,752,334,963,978,947,980,986,460,991,996,252,992,999,1003,1027,378,1046,1075,76,430,1038,1043,1044,252,460,1048,1049,460,1050,477,1062,1060,814,815,1076,817,774,995,756,1064,1070,1073,251,477,110,474,825,769,481,151,120,806,324,379,380,381,382,383,384,385,386,387,388,805,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,389,605,21,170,171,1039,799,425,735,575,883,562,334,613,1065,22,23,24,84,25,172,0,0,0,173,26,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,477,0,138,0,0,0,0,0,0,36,37,38,0,183,184,0,477,101,0,334,0,271,0,0,0,0,0,0,0,0,334,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,675,0,0,0,0,252,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,334,194,195,0,0,0,0,0,0,0,0,0,0,0,0,271,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,477,252,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,939,0,138,334,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,310,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,312,334,0,0,0,0,0,455,0,46,47,0,0,437,0,48,49,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,0,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,780,781,21,170,171,275,334,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,334,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,- 313,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,289,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,- 41,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,84,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,0,350,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,84,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,0,0,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,646,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,404,0,35,405,87,0,0,88,89,90,0,245,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,406,407,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,734,0,35,405,87,0,0,88,89,90,0,245,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,406,407,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,417,0,35,216,87,0,0,88,89,90,0,0,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,0,0,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,862,863,30,31,- 41,0,0,0,32,33,34,0,0,35,0,84,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,22,23,24,- 11,85,0,0,0,45,0,46,47,0,0,29,0,48,49,50,216,87,30,31,88,89,90,0,0,42,34,0,0,35,91,0,0,0,0,0,864,0,93,94,0,36,37,0,95,0,0,865,0,0,96,193,0,0,194,195,0,0,0,0,0,0,0,0,0,310,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,312,0,0,0,0,0,0,436,0,46,47,0,84,437,0,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,85,0,0,30,31,0,0,0,0,0,0,34,736,0,35,216,87,0,0,88,89,90,0,0,42,0,36,37,0,91,0,0,0,0,0,92,0,93,94,0,0,0,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,482,0,46,47,0,0,0,0,48,49,50,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,- 41,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,0,142,0,0,265,0,0,0,0,46,47,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,40,41,0,43,44,0,0,142,22,23,24,0,0,0,0,46,47,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,310,0,0,311,0,0,0,22,23,24,40,41,0,43,44,0,312,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,230,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,231,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,310,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,312,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,427,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,691,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,698,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,85,0,0,0,0,0,36,37,0,0,0,0,0,0,0,216,87,0,0,88,89,90,0,0,42,0,0,0,0,91,0,0,0,0,0,92,0,93,94,0,40,0,0,95,44,0,0,0,0,96,0,0,0,0,0,46,47,170,171,275,0,276,277,278,279,280,281,282,283,0,0,0,84,0,172,284,0,0,173,0,285,0,0,0,174,175,176,177,178,0,0,0,0,179,0,286,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,87,185,186,88,89,90,0,0,42,0,0,187,288,142,170,171,0,0,0,188,0,548,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,550,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,811,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,823,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,987,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,406,407,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,317,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,503,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,593,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,624,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,681,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,749,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,763,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,785,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,787,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,888,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,891,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,894,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,969,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,973,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,975,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,1036,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,753,87,185,186,88,89,90,180,181,42,182,0,187,0,0,0,0,0,0,0,188,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,0,0,192,193,0,0,194,195,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,39,87,185,186,88,89,90,180,181,42,182,0,187,0,723,0,0,0,0,0,188,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,170,171,0,0,0,0,330,0,0,189,0,0,0,0,190,84,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,39,87,185,186,88,89,90,180,181,42,182,0,187,0,0,0,0,0,0,0,347,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,170,171,0,0,0,0,188,0,0,189,0,0,0,0,190,84,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,0,0,0,0,0,938,0,0,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195};
# 2771
static short Cyc_yycheck[7282]={0,112,161,67,172,116,291,292,117,294,335,359,265,0,190,191,192,148,365,366,361,21,148,173,173,371,372,443,163,367,368,369,370,362,21,211,148,252,373,374,132,169,364,462,395,45,110,267,772,233,88,407,523,53,21,70,598,76,5,6,60,61,62,108,64,350,53,106,803,458,70,71,17,60,61,62,121,64,94,124,438,126,110,70,304,67,104,105,0,127,108,94,70,60,61,62,831,64,137,488,458,139,521,70,45,128,106,107,62,21,122,530,528,132,108,839,128,132,59,106,107,121,138,123,124,125,126,483,110,144,235,88,132,88,121,138,123,124,125,126,536,53,142,126,144,132,617,252,60,61,62,151,64,312,132,142,507,144,70,790,328,115,126,108,142,132,144,124,122,124,127,108,127,114,108,142,123,144,89,90,117,723,139,121,919,920,89,90,252,315,925,128,126,115,106,107,129,124,326,375,376,377,188,108,145,132,139,271,130,121,119,123,124,125,126,126,127,126,108,128,132,126,115,126,127,134,83,132,624,860,142,129,144,233,108,92,126,130,121,407,455,139,116,117,118,231,233,431,432,433,25,266,109,126,128,990,108,124,478,994,134,129,136,137,138,126,266,482,135,121,25,139,133,598,136,20,21,129,108,266,132,392,144,145,443,396,121,117,266,108,109,291,292,128,294,406,128,116,128,266,553,131,320,659,291,292,937,294,108,51,85,119,943,291,292,483,294,117,126,670,128,96,322,264,291,292,134,294,128,107,85,188,73,119,965,108,136,137,138,53,126,96,128,116,330,128,332,108,134,745,350,108,293,749,266,116,117,118,278,345,117,347,123,350,284,122,125,763,108,528,227,107,350,1004,133,134,108,117,121,291,292,124,294,350,709,117,805,119,60,565,124,455,106,107,458,129,128,124,723,684,238,510,129,241,136,137,138,121,426,123,124,125,126,121,770,128,124,124,482,108,109,513,637,1054,488,132,283,116,446,124,124,431,432,433,122,129,1067,132,350,654,440,108,838,218,431,432,433,222,124,116,117,118,848,129,229,455,709,457,122,527,126,128,597,218,531,220,466,222,468,121,470,309,124,330,229,84,85,86,128,466,136,468,482,470,134,124,343,325,144,145,347,659,627,132,235,122,124,628,513,499,518,129,628,360,829,640,121,122,128,509,641,252,499,513,134,641,136,137,138,128,110,499,509,124,133,134,513,122,129,641,271,509,499,124,693,513,128,523,129,124,397,108,509,124,129,901,513,108,129,116,117,118,539,0,466,622,468,121,470,727,813,128,124,689,980,132,716,716,125,136,137,138,698,637,108,22,829,128,25,129,27,124,116,117,118,121,129,499,122,509,654,124,917,657,661,706,944,509,45,884,121,513,770,950,951,946,53,124,719,720,956,957,59,599,952,953,954,955,947,948,67,118,69,958,959,72,626,949,128,697,128,617,124,408,81,492,493,129,85,496,124,91,499,123,644,129,421,108,648,96,139,508,149,408,143,116,117,118,943,106,107,648,125,110,917,124,421,114,128,424,129,686,690,741,121,124,123,124,125,126,129,748,117,124,684,752,686,124,129,865,140,141,129,121,761,436,124,684,145,686,124,765,137,128,124,129,684,883,686,129,136,137,138,148,455,108,703,684,121,686,128,124,169,116,117,118,119,124,128,796,121,93,129,901,125,128,648,804,121,122,108,482,129,136,137,138,133,856,116,117,118,139,466,673,468,143,470,84,85,86,128,128,121,128,832,124,134,835,265,1054,128,218,684,220,686,222,844,129,228,905,121,133,229,124,960,961,962,121,128,170,124,172,173,243,632,121,907,232,124,234,40,128,183,184,128,252,133,134,128,190,191,192,121,91,92,124,128,264,675,676,141,980,319,140,271,122,273,1003,142,276,211,17,18,19,269,610,87,88,335,5,6,338,339,340,341,342,293,344,431,432,433,108,607,921,108,610,353,777,926,128,357,79,80,783,123,124,930,129,108,109,110,111,112,113,114,22,23,117,864,326,123,378,123,124,884,1053,123,124,128,141,142,131,730,390,123,108,637,884,395,91,92,123,124,116,117,118,884,123,746,123,767,87,88,654,981,128,657,884,115,775,776,759,123,136,137,138,912,784,785,108,787,122,789,122,648,128,108,132,117,1050,437,128,121,943,128,128,121,328,121,128,400,128,938,810,940,123,943,136,137,138,125,408,108,132,135,943,129,129,888,139,119,891,884,123,421,94,943,424,359,127,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,129,984,126,123,436,127,124,132,780,124,864,455,129,507,458,129,510,123,935,124,122,128,466,455,468,129,470,124,780,407,462,108,127,121,943,124,890,128,482,893,894,122,489,490,488,132,30,121,44,128,76,968,969,139,129,972,973,1054,975,553,129,1044,129,124,71,72,73,74,75,108,1054,78,79,80,132,129,124,124,510,1054,124,124,135,40,938,123,135,129,129,521,1054,945,129,129,124,129,123,129,530,108,129,129,122,128,598,483,129,128,123,963,128,122,129,1030,77,121,1033,121,1035,1036,123,122,127,977,126,25,108,135,121,129,984,129,129,124,129,629,93,129,132,129,634,129,1059,128,122,1054,135,123,642,129,121,108,109,121,128,112,113,114,128,115,117,109,129,607,129,122,610,129,129,124,129,128,123,130,131,129,670,129,123,136,28,122,128,132,93,142,633,565,93,123,140,124,25,637,123,121,628,124,123,129,121,57,109,0,648,637,129,129,128,641,654,123,129,657,124,709,129,123,712,713,0,715,666,930,636,129,129,129,116,723,36,264,726,657,269,72,49,703,162,95,96,97,98,99,100,101,102,103,104,698,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,125,409,24,25,26,980,693,229,607,363,780,346,659,424,1050,37,38,39,40,41,42,- 1,- 1,- 1,46,47,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,813,- 1,765,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,829,780,- 1,716,- 1,772,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,727,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,805,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,770,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,839,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,917,856,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,862,- 1,930,865,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,883,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,121,901,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,134,- 1,136,137,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,944,- 1,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1003,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1053,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,45,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,25,26,58,59,60,- 1,- 1,- 1,64,65,66,- 1,- 1,69,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,37,38,39,123,93,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,51,- 1,136,137,138,108,109,58,59,112,113,114,- 1,- 1,117,66,- 1,- 1,69,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,81,82,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,40,134,- 1,136,137,138,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,93,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,81,82,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,37,38,39,- 1,- 1,- 1,- 1,130,131,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,122,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,123,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,123,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,115,- 1,- 1,136,119,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,- 1,130,131,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,43,- 1,- 1,46,- 1,48,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,63,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,122,25,26,- 1,- 1,- 1,128,- 1,32,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp51D[8]="stdcall";static char _tmp51E[6]="cdecl";static char _tmp51F[9]="fastcall";static char _tmp520[9]="noreturn";static char _tmp521[6]="const";static char _tmp522[8]="aligned";static char _tmp523[7]="packed";static char _tmp524[7]="shared";static char _tmp525[7]="unused";static char _tmp526[5]="weak";static char _tmp527[10]="dllimport";static char _tmp528[10]="dllexport";static char _tmp529[23]="no_instrument_function";static char _tmp52A[12]="constructor";static char _tmp52B[11]="destructor";static char _tmp52C[22]="no_check_memory_usage";static char _tmp52D[5]="pure";static char _tmp52E[14]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA23(union Cyc_YYSTYPE*yylval,unsigned int*_tmpA22,unsigned int*_tmpA21,union Cyc_YYSTYPE**_tmpA20){for(*_tmpA22=0;*_tmpA22 < *_tmpA21;(*_tmpA22)++){(*_tmpA20)[*_tmpA22]=(union Cyc_YYSTYPE)*yylval;}}static void _tmpA28(unsigned int*_tmpA27,unsigned int*_tmpA26,struct Cyc_Yyltype**_tmpA25){for(*_tmpA27=0;*_tmpA27 < *_tmpA26;(*_tmpA27)++){(*_tmpA25)[*_tmpA27]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmpA2F(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmpA2E,unsigned int*_tmpA2D,short**_tmpA2B){for(*_tmpA2E=0;*_tmpA2E < *_tmpA2D;(*_tmpA2E)++){(*_tmpA2B)[*_tmpA2E]=(short)(
# 216
*_tmpA2E <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmpA2E)): 0);}}static void _tmpA35(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmpA34,unsigned int*_tmpA33,union Cyc_YYSTYPE**_tmpA31){for(*_tmpA34=0;*_tmpA34 < *_tmpA33;(*_tmpA34)++){(*_tmpA31)[*_tmpA34]=(union Cyc_YYSTYPE)(
# 219
*_tmpA34 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmpA34)):*yylval);}}static void _tmpA3B(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmpA3A,unsigned int*_tmpA39,struct Cyc_Yyltype**_tmpA37){for(*_tmpA3A=0;*_tmpA3A < *_tmpA39;(*_tmpA3A)++){(*_tmpA37)[*_tmpA3A]=(struct Cyc_Yyltype)(
# 222
*_tmpA3A <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmpA3A)):
 Cyc_yynewloc());}}static void _tmpF5C(unsigned int*_tmpF5B,unsigned int*_tmpF5A,char**_tmpF58){for(*_tmpF5B=0;*_tmpF5B < *_tmpF5A;(*_tmpF5B)++){(*_tmpF58)[*_tmpF5B]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp460=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp460;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmpA1C;union Cyc_YYSTYPE yylval=((_tmpA1C.YYINITIALSVAL).val=0,(((_tmpA1C.YYINITIALSVAL).tag=64,_tmpA1C)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmpA1E;unsigned int _tmpA1D;struct _dyneither_ptr yyss=(_tmpA1D=200,((_tmpA1E=_region_calloc(yyregion,sizeof(short),_tmpA1D),_tag_dyneither(_tmpA1E,sizeof(short),_tmpA1D))));
# 158
int yyvsp_offset;
unsigned int _tmpA22;unsigned int _tmpA21;union Cyc_YYSTYPE*_tmpA20;unsigned int _tmpA1F;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmpA1F=(unsigned int)200,((_tmpA20=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA1F)),((((_tmpA21=_tmpA1F,_tmpA23(& yylval,& _tmpA22,& _tmpA21,& _tmpA20))),_tmpA20)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmpA27;unsigned int _tmpA26;struct Cyc_Yyltype*_tmpA25;unsigned int _tmpA24;struct _dyneither_ptr yyls=_tag_dyneither(((_tmpA24=(unsigned int)200,((_tmpA25=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA24)),((((_tmpA26=_tmpA24,_tmpA28(& _tmpA27,& _tmpA26,& _tmpA25))),_tmpA25)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmpA29;Cyc_yyerror(((_tmpA29="parser stack overflow",_tag_dyneither(_tmpA29,sizeof(char),22))),yystate,yychar);}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmpA2E;unsigned int _tmpA2D;struct _dyneither_ptr _tmpA2C;short*_tmpA2B;unsigned int _tmpA2A;struct _dyneither_ptr yyss1=(_tmpA2A=(unsigned int)yystacksize,((_tmpA2B=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmpA2A)),((_tmpA2C=_tag_dyneither(_tmpA2B,sizeof(short),_tmpA2A),((((_tmpA2D=_tmpA2A,_tmpA2F(& yyssp_offset,& yyss,& _tmpA2E,& _tmpA2D,& _tmpA2B))),_tmpA2C)))))));
# 217
unsigned int _tmpA34;unsigned int _tmpA33;struct _dyneither_ptr _tmpA32;union Cyc_YYSTYPE*_tmpA31;unsigned int _tmpA30;struct _dyneither_ptr yyvs1=
(_tmpA30=(unsigned int)yystacksize,((_tmpA31=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA30)),((_tmpA32=_tag_dyneither(_tmpA31,sizeof(union Cyc_YYSTYPE),_tmpA30),((((_tmpA33=_tmpA30,_tmpA35(& yyvs,& yyssp_offset,& yylval,& _tmpA34,& _tmpA33,& _tmpA31))),_tmpA32)))))));
# 221
unsigned int _tmpA3A;unsigned int _tmpA39;struct _dyneither_ptr _tmpA38;struct Cyc_Yyltype*_tmpA37;unsigned int _tmpA36;struct _dyneither_ptr yyls1=(_tmpA36=(unsigned int)yystacksize,((_tmpA37=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA36)),((_tmpA38=_tag_dyneither(_tmpA37,sizeof(struct Cyc_Yyltype),_tmpA36),((((_tmpA39=_tmpA36,_tmpA3B(& yyssp_offset,& yyls,& _tmpA3A,& _tmpA39,& _tmpA37))),_tmpA38)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1077,yystate)];
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
yychar1=yychar > 0  && yychar <= 375?(int)Cyc_yytranslate[_check_known_subscript_notnull(376,yychar)]: 296;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7281) || Cyc_yycheck[_check_known_subscript_notnull(7282,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(7282,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1076){
int _tmp46E=0;_npop_handler(0);return _tmp46E;}
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1077,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(530,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL222: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1220 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1223
break;}case 2: _LL223: {
# 1225
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1227
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1226 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp46F;_push_handler(& _tmp46F);{int _tmp471=0;if(setjmp(_tmp46F.handler))_tmp471=1;if(!_tmp471){
x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp470=(void*)_exn_thrown;void*_tmp473=_tmp470;void*_tmp475;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp474=(struct Cyc_Core_Failure_exn_struct*)_tmp473;if(_tmp474->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
# 1231
 x=0;goto _LL225;_LL228: _tmp475=_tmp473;_LL229:(void)_rethrow(_tmp475);_LL225:;}};}
# 1233
{struct _handler_cons _tmp476;_push_handler(& _tmp476);{int _tmp478=0;if(setjmp(_tmp476.handler))_tmp478=1;if(!_tmp478){
y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp477=(void*)_exn_thrown;void*_tmp47A=_tmp477;void*_tmp47C;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp47B=(struct Cyc_Core_Failure_exn_struct*)_tmp47A;if(_tmp47B->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
# 1237
 y=0;goto _LL22A;_LL22D: _tmp47C=_tmp47A;_LL22E:(void)_rethrow(_tmp47C);_LL22A:;}};}
# 1239
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1241
break;}case 3: _LL224: {
# 1243
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1244 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA45;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA44;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA43;struct Cyc_List_List*_tmpA42;yyval=Cyc_YY16(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->hd=((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->r=(void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=10,((_tmpA44.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA44.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA44)))))),_tmpA43)))),((_tmpA45->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA45)))))),((_tmpA42->tl=0,_tmpA42)))))));}
Cyc_Lex_leave_using();
# 1247
break;}case 4: _LL22F: {
# 1249
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1248 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA4F;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA4E;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA4D;struct Cyc_List_List*_tmpA4C;yyval=Cyc_YY16(((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->hd=((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F->r=(void*)((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=10,((_tmpA4E.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA4E.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA4E)))))),_tmpA4D)))),((_tmpA4F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA4F)))))),((_tmpA4C->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA4C)))))));}
break;}case 5: _LL230: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1251 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA5E;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA5D;struct _dyneither_ptr*_tmpA5C;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY16(((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E->r=(void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5D.tag=9,((_tmpA5D.f1=((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA5C)))),((_tmpA5D.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA5D)))))),_tmpA5B)))),((_tmpA5E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA5E)))))),((_tmpA5A->tl=0,_tmpA5A)))))));}
Cyc_Lex_leave_namespace();
# 1254
break;}case 6: _LL231: {
# 1256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1256 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp48A;_push_handler(& _tmp48A);{int _tmp48C=0;if(setjmp(_tmp48A.handler))_tmp48C=1;if(!_tmp48C){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp48B=(void*)_exn_thrown;void*_tmp48E=_tmp48B;void*_tmp490;_LL234: {struct Cyc_Core_Failure_exn_struct*_tmp48F=(struct Cyc_Core_Failure_exn_struct*)_tmp48E;if(_tmp48F->tag != Cyc_Core_Failure)goto _LL236;}_LL235:
{const char*_tmpA5F;nspace=((_tmpA5F="",_tag_dyneither(_tmpA5F,sizeof(char),1)));}goto _LL233;_LL236: _tmp490=_tmp48E;_LL237:(void)_rethrow(_tmp490);_LL233:;}};}
# 1261
{struct _handler_cons _tmp492;_push_handler(& _tmp492);{int _tmp494=0;if(setjmp(_tmp492.handler))_tmp494=1;if(!_tmp494){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp493=(void*)_exn_thrown;void*_tmp496=_tmp493;void*_tmp498;_LL239: {struct Cyc_Core_Failure_exn_struct*_tmp497=(struct Cyc_Core_Failure_exn_struct*)_tmp496;if(_tmp497->tag != Cyc_Core_Failure)goto _LL23B;}_LL23A:
 x=0;goto _LL238;_LL23B: _tmp498=_tmp496;_LL23C:(void)_rethrow(_tmp498);_LL238:;}};}
# 1264
{struct _handler_cons _tmp499;_push_handler(& _tmp499);{int _tmp49B=0;if(setjmp(_tmp499.handler))_tmp49B=1;if(!_tmp49B){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp49A=(void*)_exn_thrown;void*_tmp49D=_tmp49A;void*_tmp49F;_LL23E: {struct Cyc_Core_Failure_exn_struct*_tmp49E=(struct Cyc_Core_Failure_exn_struct*)_tmp49D;if(_tmp49E->tag != Cyc_Core_Failure)goto _LL240;}_LL23F:
 y=0;goto _LL23D;_LL240: _tmp49F=_tmp49D;_LL241:(void)_rethrow(_tmp49F);_LL23D:;}};}
# 1267
{struct Cyc_Absyn_Decl*_tmpA6E;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA6D;struct _dyneither_ptr*_tmpA6C;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA6B;struct Cyc_List_List*_tmpA6A;yyval=Cyc_YY16(((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->hd=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->r=(void*)((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6D.tag=9,((_tmpA6D.f1=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=nspace,_tmpA6C)))),((_tmpA6D.f2=x,_tmpA6D)))))),_tmpA6B)))),((_tmpA6E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA6E)))))),((_tmpA6A->tl=y,_tmpA6A)))))));}
# 1269
break;}case 7: _LL232: {
# 1271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1270 "parse.y"
int _tmp4A5=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp4A5){
struct Cyc_Absyn_Decl*_tmpA78;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA77;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA76;struct Cyc_List_List*_tmpA75;yyval=Cyc_YY16(((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->hd=((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->r=(void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA77.tag=11,((_tmpA77.f1=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA77)))),_tmpA76)))),((_tmpA78->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA78)))))),((_tmpA75->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA75)))))));}else{
# 1274
struct Cyc_Absyn_Decl*_tmpA82;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA81;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA80;struct Cyc_List_List*_tmpA7F;yyval=Cyc_YY16(((_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->hd=((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->r=(void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=12,((_tmpA81.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA81.f2=0,_tmpA81)))))),_tmpA80)))),((_tmpA82->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA82)))))),((_tmpA7F->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA7F)))))));}
# 1276
break;}case 8: _LL242: {
# 1278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1277 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0])){
const char*_tmpA83;Cyc_Parse_err(((_tmpA83="expecting \"C include\"",_tag_dyneither(_tmpA83,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
{struct Cyc_Absyn_Decl*_tmpA8D;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA8C;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA8B;struct Cyc_List_List*_tmpA8A;yyval=Cyc_YY16(((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A->hd=((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->r=(void*)((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8C.tag=12,((_tmpA8C.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA8C.f2=exs,_tmpA8C)))))),_tmpA8B)))),((_tmpA8D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA8D)))))),((_tmpA8A->tl=Cyc_yyget_YY16(yyyyvsp[5]),_tmpA8A)))))));}
# 1282
break;};}case 9: _LL243: {
# 1284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1283 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA90;struct Cyc_List_List*_tmpA8F;yyval=Cyc_YY16(((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA90->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA90)))))),((_tmpA8F->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA8F)))))));}
break;}case 10: _LL244: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1285 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA93;struct Cyc_List_List*_tmpA92;yyval=Cyc_YY16(((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->hd=((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA93->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA93)))))),((_tmpA92->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA92)))))));}
break;}case 11: _LL245:
# 1288
 yyval=Cyc_YY16(0);
break;case 12: _LL246: {
# 1291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1291 "parse.y"
struct _dyneither_ptr _tmp4B7=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
{const char*_tmpA94;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B7,((_tmpA94="C",_tag_dyneither(_tmpA94,sizeof(char),2))))== 0)
yyval=Cyc_YY31(0);else{
const char*_tmpA95;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B7,((_tmpA95="C include",_tag_dyneither(_tmpA95,sizeof(char),10))))== 0)
yyval=Cyc_YY31(1);else{
# 1298
{const char*_tmpA96;Cyc_Parse_err(((_tmpA96="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA96,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY31(1);}}}
# 1302
break;}case 13: _LL247: {
# 1304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1305 "parse.y"
Cyc_Lex_leave_extern_c();
break;}case 14: _LL248: {
# 1308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1309 "parse.y"
yyval=yyyyvsp[2];
break;}case 15: _LL249: {
# 1312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1310 "parse.y"
yyval=Cyc_YY52(0);
break;}case 16: _LL24A: {
# 1313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1314 "parse.y"
{struct _tuple27*_tmpA99;struct Cyc_List_List*_tmpA98;yyval=Cyc_YY52(((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->hd=((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA99->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA99->f3=0,_tmpA99)))))))),((_tmpA98->tl=0,_tmpA98)))))));}
break;}case 17: _LL24B: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1315 "parse.y"
{struct _tuple27*_tmpA9C;struct Cyc_List_List*_tmpA9B;yyval=Cyc_YY52(((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->hd=((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA9C->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA9C->f3=0,_tmpA9C)))))))),((_tmpA9B->tl=0,_tmpA9B)))))));}
break;}case 18: _LL24C: {
# 1318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1317 "parse.y"
{struct _tuple27*_tmpA9F;struct Cyc_List_List*_tmpA9E;yyval=Cyc_YY52(((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E->hd=((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA9F->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA9F->f3=0,_tmpA9F)))))))),((_tmpA9E->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA9E)))))));}
break;}case 19: _LL24D: {
# 1320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1321 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpAA5;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpAA4;struct Cyc_List_List*_tmpAA3;yyval=Cyc_YY16(((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3->hd=Cyc_Absyn_new_decl((void*)((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA4.tag=1,((_tmpAA4.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpAA4)))),_tmpAA5)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpAA3->tl=0,_tmpAA3)))))));}
break;}case 20: _LL24E: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1322 "parse.y"
yyval=yyyyvsp[0];
break;}case 21: _LL24F: {
# 1325
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1323 "parse.y"
yyval=Cyc_YY16(0);
break;}case 24: _LL250: {
# 1326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1332 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),0,Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL251: {
# 1335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1334 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C4=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C4,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL252: {
# 1338
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1347 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1]),Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL253: {
# 1350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1349 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C5=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C5,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 28: _LL254: {
# 1353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1357 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C6=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C6,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 29: _LL255: {
# 1361
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1360 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C7=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C7,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 30: _LL256: {
# 1364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1365 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 31: _LL257: {
# 1368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1368 "parse.y"
Cyc_Lex_leave_using();
break;}case 32: _LL258: {
# 1371
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1373
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1371 "parse.y"
{struct _dyneither_ptr*_tmpAA6;Cyc_Lex_enter_namespace(((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpAA6)))));}yyval=yyyyvsp[1];
break;}case 33: _LL259: {
# 1374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1374 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 34: _LL25A: {
# 1377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp4C9=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),0,(unsigned int)_tmp4C9,(unsigned int)_tmp4C9));
break;}case 35: _LL25B: {
# 1384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1383 "parse.y"
int _tmp4CA=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp4CA));
break;}case 36: _LL25C: {
# 1387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1387 "parse.y"
{struct Cyc_List_List*_tmpAA7;yyval=Cyc_YY16(((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpAA7->tl=0,_tmpAA7)))))));}
break;}case 37: _LL25D: {
# 1390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1389 "parse.y"
struct Cyc_List_List*_tmp4CC=0;
{struct Cyc_List_List*_tmp4CD=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp4CD != 0;_tmp4CD=_tmp4CD->tl){
struct _dyneither_ptr*_tmp4CE=(struct _dyneither_ptr*)_tmp4CD->hd;
struct _tuple0*_tmpAA8;struct _tuple0*qv=(_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->f1=Cyc_Absyn_Rel_n(0),((_tmpAA8->f2=_tmp4CE,_tmpAA8)))));
struct Cyc_Absyn_Vardecl*_tmp4CF=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpAA9;_tmp4CC=((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->hd=_tmp4CF,((_tmpAA9->tl=_tmp4CC,_tmpAA9))))));}}
# 1396
_tmp4CC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4CC);
{struct Cyc_List_List*_tmpAAA;yyval=Cyc_YY16(((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->hd=Cyc_Absyn_letv_decl(_tmp4CC,(unsigned int)(yyyylsp[0]).first_line),((_tmpAAA->tl=0,_tmpAAA)))))));}
# 1399
break;}case 38: _LL25E: {
# 1401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1402 "parse.y"
struct _dyneither_ptr _tmp4D3=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1404
{const char*_tmpAAB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D3,((_tmpAAB="`H",_tag_dyneither(_tmpAAB,sizeof(char),3))))== 0){
const char*_tmpAAE;void*_tmpAAD;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAAD=0,Cyc_aprintf(((_tmpAAE="bad occurrence of heap region",_tag_dyneither(_tmpAAE,sizeof(char),30))),_tag_dyneither(_tmpAAD,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpAAF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D3,((_tmpAAF="`U",_tag_dyneither(_tmpAAF,sizeof(char),3))))== 0){
const char*_tmpAB2;void*_tmpAB1;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAB1=0,Cyc_aprintf(((_tmpAB2="bad occurrence of unique region",_tag_dyneither(_tmpAB2,sizeof(char),32))),_tag_dyneither(_tmpAB1,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAB5;struct Cyc_Absyn_Tvar*_tmpAB4;struct Cyc_Absyn_Tvar*tv=(_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->name=((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5[0]=_tmp4D3,_tmpAB5)))),((_tmpAB4->identity=- 1,((_tmpAB4->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAB4)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAB8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAB7;void*t=(void*)((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7[0]=((_tmpAB8.tag=2,((_tmpAB8.f1=tv,_tmpAB8)))),_tmpAB7))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpABE;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpABD;struct _dyneither_ptr*_tmpABB;struct _tuple0*_tmpABA;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->f1=Cyc_Absyn_Loc_n,((_tmpABA->f2=((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpABB)))),_tmpABA)))))),(void*)((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABE.tag=15,((_tmpABE.f1=t,_tmpABE)))),_tmpABD)))),0);
{struct Cyc_List_List*_tmpABF;yyval=Cyc_YY16(((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpABF->tl=0,_tmpABF)))))));}
# 1413
break;};}case 39: _LL25F: {
# 1415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1415 "parse.y"
int _tmp4E3=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp4E4=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpAC0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E4,((_tmpAC0="H",_tag_dyneither(_tmpAC0,sizeof(char),2))))== 0){
const char*_tmpAC1;Cyc_Parse_err(((_tmpAC1="bad occurrence of heap region `H",_tag_dyneither(_tmpAC1,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpAC2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E4,((_tmpAC2="U",_tag_dyneither(_tmpAC2,sizeof(char),2))))== 0){
const char*_tmpAC3;Cyc_Parse_err(((_tmpAC3="bad occurrence of unique region `U",_tag_dyneither(_tmpAC3,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpAD0;const char*_tmpACF;void*_tmpACE[1];struct Cyc_String_pa_PrintArg_struct _tmpACD;struct Cyc_Absyn_Tvar*_tmpACC;struct Cyc_Absyn_Tvar*tv=(_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->name=((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=(struct _dyneither_ptr)((_tmpACD.tag=0,((_tmpACD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4E4),((_tmpACE[0]=& _tmpACD,Cyc_aprintf(((_tmpACF="`%s",_tag_dyneither(_tmpACF,sizeof(char),4))),_tag_dyneither(_tmpACE,sizeof(void*),1)))))))),_tmpAD0)))),((_tmpACC->identity=- 1,((_tmpACC->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpACC)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAD3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAD2;void*t=(void*)((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD3.tag=2,((_tmpAD3.f1=tv,_tmpAD3)))),_tmpAD2))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAD9;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAD8;struct _dyneither_ptr*_tmpAD6;struct _tuple0*_tmpAD5;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->f1=Cyc_Absyn_Loc_n,((_tmpAD5->f2=((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6[0]=_tmp4E4,_tmpAD6)))),_tmpAD5)))))),(void*)((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpAD9.tag=15,((_tmpAD9.f1=t,_tmpAD9)))),_tmpAD8)))),0);
# 1426
{struct Cyc_List_List*_tmpADA;yyval=Cyc_YY16(((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4E3,0,(unsigned int)(yyyylsp[0]).first_line),((_tmpADA->tl=0,_tmpADA)))))));}
# 1428
break;};}case 40: _LL260: {
# 1430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1430 "parse.y"
struct _dyneither_ptr _tmp4F5=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4F6=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4F7=Cyc_yyget_Exp_tok(yyyyvsp[6]);
{const char*_tmpADB;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4F6,((_tmpADB="open",_tag_dyneither(_tmpADB,sizeof(char),5))))!= 0){const char*_tmpADC;Cyc_Parse_err(((_tmpADC="expecting `open'",_tag_dyneither(_tmpADC,sizeof(char),17))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAE9;const char*_tmpAE8;void*_tmpAE7[1];struct Cyc_String_pa_PrintArg_struct _tmpAE6;struct Cyc_Absyn_Tvar*_tmpAE5;struct Cyc_Absyn_Tvar*tv=(_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->name=((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9[0]=(struct _dyneither_ptr)((_tmpAE6.tag=0,((_tmpAE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4F5),((_tmpAE7[0]=& _tmpAE6,Cyc_aprintf(((_tmpAE8="`%s",_tag_dyneither(_tmpAE8,sizeof(char),4))),_tag_dyneither(_tmpAE7,sizeof(void*),1)))))))),_tmpAE9)))),((_tmpAE5->identity=- 1,((_tmpAE5->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAE5)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAEC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAEB;void*t=(void*)((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEC.tag=2,((_tmpAEC.f1=tv,_tmpAEC)))),_tmpAEB))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAF2;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAF1;struct _dyneither_ptr*_tmpAEF;struct _tuple0*_tmpAEE;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->f1=Cyc_Absyn_Loc_n,((_tmpAEE->f2=((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF[0]=_tmp4F5,_tmpAEF)))),_tmpAEE)))))),(void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF2.tag=15,((_tmpAF2.f1=t,_tmpAF2)))),_tmpAF1)))),0);
# 1439
{struct Cyc_List_List*_tmpAF3;yyval=Cyc_YY16(((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4F7,(unsigned int)(yyyylsp[0]).first_line),((_tmpAF3->tl=0,_tmpAF3)))))));}
# 1441
break;};}case 41: _LL261:
# 1443
 yyval=Cyc_YY31(0);
break;case 42: _LL262: {
# 1446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1445 "parse.y"
yyval=Cyc_YY31(1);
break;}case 43: _LL263: {
# 1448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1450 "parse.y"
yyval=yyyyvsp[0];
break;}case 44: _LL264: {
# 1453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1452 "parse.y"
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1])));
break;}case 45: _LL265: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpAF4;yyval=Cyc_YY17(((_tmpAF4.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAF4.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAF4.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAF4.is_inline=0,((_tmpAF4.attributes=0,_tmpAF4)))))))))));}
break;}case 46: _LL266: {
# 1462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1461 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp507=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp507.sc != 0){
const char*_tmpAF7;void*_tmpAF6;(_tmpAF6=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAF7="Only one storage class is allowed in a declaration (missing ';' or ','?)",_tag_dyneither(_tmpAF7,sizeof(char),73))),_tag_dyneither(_tmpAF6,sizeof(void*),0)));}
# 1465
{struct Cyc_Parse_Declaration_spec _tmpAF8;yyval=Cyc_YY17(((_tmpAF8.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAF8.tq=_tmp507.tq,((_tmpAF8.type_specs=_tmp507.type_specs,((_tmpAF8.is_inline=_tmp507.is_inline,((_tmpAF8.attributes=_tmp507.attributes,_tmpAF8)))))))))));}
# 1469
break;}case 47: _LL267: {
# 1471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1470 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAF9;yyval=Cyc_YY17(((_tmpAF9.sc=0,((_tmpAF9.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAF9.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAF9.is_inline=0,((_tmpAF9.attributes=0,_tmpAF9)))))))))));}
break;}case 48: _LL268: {
# 1474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1473 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50C=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAFA;yyval=Cyc_YY17(((_tmpAFA.sc=_tmp50C.sc,((_tmpAFA.tq=_tmp50C.tq,((_tmpAFA.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp50C.type_specs,Cyc_yyget_YY21(yyyyvsp[0])),((_tmpAFA.is_inline=_tmp50C.is_inline,((_tmpAFA.attributes=_tmp50C.attributes,_tmpAFA)))))))))));}
# 1479
break;}case 49: _LL269: {
# 1481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1480 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAFB;yyval=Cyc_YY17(((_tmpAFB.sc=0,((_tmpAFB.tq=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAFB.type_specs=Cyc_Parse_empty_spec(0),((_tmpAFB.is_inline=0,((_tmpAFB.attributes=0,_tmpAFB)))))))))));}
break;}case 50: _LL26A: {
# 1483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1482 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50F=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAFC;yyval=Cyc_YY17(((_tmpAFC.sc=_tmp50F.sc,((_tmpAFC.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp50F.tq),((_tmpAFC.type_specs=_tmp50F.type_specs,((_tmpAFC.is_inline=_tmp50F.is_inline,((_tmpAFC.attributes=_tmp50F.attributes,_tmpAFC)))))))))));}
# 1487
break;}case 51: _LL26B: {
# 1489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1488 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAFD;yyval=Cyc_YY17(((_tmpAFD.sc=0,((_tmpAFD.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAFD.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAFD.is_inline=1,((_tmpAFD.attributes=0,_tmpAFD)))))))))));}
break;}case 52: _LL26C: {
# 1492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1491 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp512=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAFE;yyval=Cyc_YY17(((_tmpAFE.sc=_tmp512.sc,((_tmpAFE.tq=_tmp512.tq,((_tmpAFE.type_specs=_tmp512.type_specs,((_tmpAFE.is_inline=1,((_tmpAFE.attributes=_tmp512.attributes,_tmpAFE)))))))))));}
# 1495
break;}case 53: _LL26D: {
# 1497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1496 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAFF;yyval=Cyc_YY17(((_tmpAFF.sc=0,((_tmpAFF.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAFF.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAFF.is_inline=0,((_tmpAFF.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAFF)))))))))));}
break;}case 54: _LL26E: {
# 1500
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1499 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp515=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpB00;yyval=Cyc_YY17(((_tmpB00.sc=_tmp515.sc,((_tmpB00.tq=_tmp515.tq,((_tmpB00.type_specs=_tmp515.type_specs,((_tmpB00.is_inline=_tmp515.is_inline,((_tmpB00.attributes=
# 1502
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp515.attributes),_tmpB00)))))))))));}
break;}case 55: _LL26F: {
# 1505
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1507
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1506 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
break;}case 56: _LL270: {
# 1510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1508 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
break;}case 57: _LL271: {
# 1512
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1514
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1510 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
break;}case 58: _LL272: {
# 1514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1512 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
break;}case 59: _LL273: {
# 1516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1515 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpB01;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpB01="C",_tag_dyneither(_tmpB01,sizeof(char),2))))!= 0){
const char*_tmpB02;Cyc_Parse_err(((_tmpB02="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpB02,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY20(& externC_sc);
# 1520
break;}case 60: _LL274: {
# 1522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1520 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
break;}case 61: _LL275: {
# 1524
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1523 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
break;}case 62: _LL276:
# 1527
 yyval=Cyc_YY45(0);
break;case 63: _LL277: {
# 1530
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1530 "parse.y"
yyval=yyyyvsp[0];
break;}case 64: _LL278: {
# 1533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1535 "parse.y"
yyval=yyyyvsp[3];
break;}case 65: _LL279: {
# 1538
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1539 "parse.y"
{struct Cyc_List_List*_tmpB03;yyval=Cyc_YY45(((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpB03->tl=0,_tmpB03)))))));}
break;}case 66: _LL27A: {
# 1542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1540 "parse.y"
{struct Cyc_List_List*_tmpB04;yyval=Cyc_YY45(((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpB04->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpB04)))))));}
break;}case 67: _LL27B: {
# 1543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1545 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple28 att_map[18]={{{_tmp51D,_tmp51D,_tmp51D + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp51E,_tmp51E,_tmp51E + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51F,_tmp51F,_tmp51F + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp520,_tmp520,_tmp520 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp521,_tmp521,_tmp521 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp522,_tmp522,_tmp522 + 8},(void*)& att_aligned},{{_tmp523,_tmp523,_tmp523 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp524,_tmp524,_tmp524 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp525,_tmp525,_tmp525 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp526,_tmp526,_tmp526 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp527,_tmp527,_tmp527 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp528,_tmp528,_tmp528 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp529,_tmp529,_tmp529 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp52A,_tmp52A,_tmp52A + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp52B,_tmp52B,_tmp52B + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp52C,_tmp52C,_tmp52C + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp52D,_tmp52D,_tmp52D + 5},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp52E,_tmp52E,_tmp52E + 14},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1566
struct _dyneither_ptr _tmp51B=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1568
if((((_get_dyneither_size(_tmp51B,sizeof(char))> 4  && ((const char*)_tmp51B.curr)[0]== '_') && ((const char*)_tmp51B.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp51B,sizeof(char),(int)(_get_dyneither_size(_tmp51B,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp51B,sizeof(char),(int)(_get_dyneither_size(_tmp51B,sizeof(char))- 3)))== '_')
# 1570
_tmp51B=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp51B,2,_get_dyneither_size(_tmp51B,sizeof(char))- 5);{
int i=0;
for(0;i < 18;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp51B,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(18,i)]).f2);
break;}}
# 1577
if(i == 18){
{const char*_tmpB05;Cyc_Parse_err(((_tmpB05="unrecognized attribute",_tag_dyneither(_tmpB05,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1582
break;};}case 68: _LL27C: {
# 1584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1582 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;}case 69: _LL27D: {
# 1585
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1584 "parse.y"
struct _dyneither_ptr _tmp530=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp531=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
{const char*_tmpB07;const char*_tmpB06;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB07="aligned",_tag_dyneither(_tmpB07,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB06="__aligned__",_tag_dyneither(_tmpB06,sizeof(char),12))))== 0){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpB0A;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpB09;a=(void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0A.tag=6,((_tmpB0A.f1=_tmp531,_tmpB0A)))),_tmpB09))));}else{
const char*_tmpB0C;const char*_tmpB0B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB0C="section",_tag_dyneither(_tmpB0C,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB0B="__section__",_tag_dyneither(_tmpB0B,sizeof(char),12))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp531);
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpB0F;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpB0E;a=(void*)((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=((_tmpB0F.tag=8,((_tmpB0F.f1=str,_tmpB0F)))),_tmpB0E))));}else{
const char*_tmpB10;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB10="__mode__",_tag_dyneither(_tmpB10,sizeof(char),9))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp531);
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpB13;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpB12;a=(void*)((_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12[0]=((_tmpB13.tag=24,((_tmpB13.f1=str,_tmpB13)))),_tmpB12))));}else{
const char*_tmpB14;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB14="alias",_tag_dyneither(_tmpB14,sizeof(char),6))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp531);
struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmpB17;struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmpB16;a=(void*)((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16[0]=((_tmpB17.tag=25,((_tmpB17.f1=str,_tmpB17)))),_tmpB16))));}else{
# 1600
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp531);
const char*_tmpB19;const char*_tmpB18;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB19="regparm",_tag_dyneither(_tmpB19,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB18="__regparm__",_tag_dyneither(_tmpB18,sizeof(char),12))))== 0){
if(n < 0  || n > 3){
const char*_tmpB1A;Cyc_Parse_err(((_tmpB1A="regparm requires value between 0 and 3",_tag_dyneither(_tmpB1A,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpB1D;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpB1C;a=(void*)((_tmpB1C=_cycalloc_atomic(sizeof(*_tmpB1C)),((_tmpB1C[0]=((_tmpB1D.tag=0,((_tmpB1D.f1=n,_tmpB1D)))),_tmpB1C))));};}else{
const char*_tmpB1F;const char*_tmpB1E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB1F="initializes",_tag_dyneither(_tmpB1F,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB1E="__initializes__",_tag_dyneither(_tmpB1E,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpB22;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpB21;a=(void*)((_tmpB21=_cycalloc_atomic(sizeof(*_tmpB21)),((_tmpB21[0]=((_tmpB22.tag=20,((_tmpB22.f1=n,_tmpB22)))),_tmpB21))));}else{
const char*_tmpB24;const char*_tmpB23;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB24="noliveunique",_tag_dyneither(_tmpB24,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB23="__noliveunique__",_tag_dyneither(_tmpB23,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpB27;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpB26;a=(void*)((_tmpB26=_cycalloc_atomic(sizeof(*_tmpB26)),((_tmpB26[0]=((_tmpB27.tag=21,((_tmpB27.f1=n,_tmpB27)))),_tmpB26))));}else{
const char*_tmpB29;const char*_tmpB28;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB29="noconsume",_tag_dyneither(_tmpB29,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp530,((_tmpB28="__noconsume__",_tag_dyneither(_tmpB28,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpB2C;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpB2B;a=(void*)((_tmpB2B=_cycalloc_atomic(sizeof(*_tmpB2B)),((_tmpB2B[0]=((_tmpB2C.tag=22,((_tmpB2C.f1=n,_tmpB2C)))),_tmpB2B))));}else{
# 1612
{const char*_tmpB2D;Cyc_Parse_err(((_tmpB2D="unrecognized attribute",_tag_dyneither(_tmpB2D,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}}
# 1616
yyval=Cyc_YY46(a);
# 1618
break;}case 70: _LL27E: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1619 "parse.y"
struct _dyneither_ptr _tmp552=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp553=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp554=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_Int_tok(yyyyvsp[4]));
unsigned int _tmp555=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[6]).first_line,Cyc_yyget_Int_tok(yyyyvsp[6]));
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpB2F;const char*_tmpB2E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp552,((_tmpB2F="format",_tag_dyneither(_tmpB2F,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp552,((_tmpB2E="__format__",_tag_dyneither(_tmpB2E,sizeof(char),11))))== 0){
const char*_tmpB30;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp553,((_tmpB30="printf",_tag_dyneither(_tmpB30,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB33;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB32;a=(void*)((_tmpB32=_cycalloc_atomic(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB33.tag=19,((_tmpB33.f1=Cyc_Absyn_Printf_ft,((_tmpB33.f2=(int)_tmp554,((_tmpB33.f3=(int)_tmp555,_tmpB33)))))))),_tmpB32))));}else{
const char*_tmpB34;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp553,((_tmpB34="scanf",_tag_dyneither(_tmpB34,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB37;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB36;a=(void*)((_tmpB36=_cycalloc_atomic(sizeof(*_tmpB36)),((_tmpB36[0]=((_tmpB37.tag=19,((_tmpB37.f1=Cyc_Absyn_Scanf_ft,((_tmpB37.f2=(int)_tmp554,((_tmpB37.f3=(int)_tmp555,_tmpB37)))))))),_tmpB36))));}else{
# 1630
const char*_tmpB38;Cyc_Parse_err(((_tmpB38="unrecognized format type",_tag_dyneither(_tmpB38,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1632
const char*_tmpB39;Cyc_Parse_err(((_tmpB39="unrecognized attribute",_tag_dyneither(_tmpB39,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);
# 1635
break;}case 71: _LL27F: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1644 "parse.y"
yyval=yyyyvsp[0];
break;}case 72: _LL280: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1646 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB3C;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB3B;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((_tmpB3C.tag=17,((_tmpB3C.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB3C.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpB3C.f3=0,((_tmpB3C.f4=0,_tmpB3C)))))))))),_tmpB3B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 73: _LL281: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL282: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL283: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1652 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL284: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1653 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL285: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL286: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL287: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL288: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL289: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1658 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL28A: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1659 "parse.y"
yyval=yyyyvsp[0];
break;}case 83: _LL28B: {
# 1662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1660 "parse.y"
yyval=yyyyvsp[0];
break;}case 84: _LL28C: {
# 1663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1663 "parse.y"
{struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpB3F;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB3E;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB3E=_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E[0]=((_tmpB3F.tag=27,((_tmpB3F.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB3F)))),_tmpB3E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 85: _LL28D: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1665 "parse.y"
{struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmpB45;const char*_tmpB44;struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmpB43;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB45.tag=28,((_tmpB45.f1=((_tmpB44="__builtin_va_list",_tag_dyneither(_tmpB44,sizeof(char),18))),((_tmpB45.f2=& Cyc_Tcutil_bk,_tmpB45)))))),_tmpB43)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 86: _LL28E: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1667 "parse.y"
yyval=yyyyvsp[0];
break;}case 87: _LL28F: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1669 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 88: _LL290: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1671 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 89: _LL291: {
# 1674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1673 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 90: _LL292: {
# 1676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1675 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB48;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB47;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=((_tmpB48.tag=10,((_tmpB48.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpB48)))),_tmpB47)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1678
break;}case 91: _LL293: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1679 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB4B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB4A;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A[0]=((_tmpB4B.tag=15,((_tmpB4B.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB4B)))),_tmpB4A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL294: {
# 1682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1681 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB4E;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB4D;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB4E.tag=15,((_tmpB4E.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpB4E)))),_tmpB4D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1683
break;}case 93: _LL295: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1684 "parse.y"
void*_tmp56D=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB51;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB50;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50[0]=((_tmpB51.tag=16,((_tmpB51.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB51.f2=_tmp56D,_tmpB51)))))),_tmpB50)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1687
break;}case 94: _LL296: {
# 1689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1688 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB54;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB53;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53[0]=((_tmpB54.tag=16,((_tmpB54.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB54.f2=Cyc_yyget_YY44(yyyyvsp[4]),_tmpB54)))))),_tmpB53)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL297: {
# 1691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1690 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB57;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB56;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56[0]=((_tmpB57.tag=19,((_tmpB57.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB57)))),_tmpB56)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 96: _LL298: {
# 1693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1692 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB5A;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB59;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59[0]=((_tmpB5A.tag=19,((_tmpB5A.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB5A)))),_tmpB59)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 97: _LL299: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1694 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB5D;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB5C;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C[0]=((_tmpB5D.tag=18,((_tmpB5D.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB5D)))),_tmpB5C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 98: _LL29A: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 99: _LL29B: {
# 1703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1704 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpB5E;yyval=Cyc_YY23(((_tmpB5E.print_const=1,((_tmpB5E.q_volatile=0,((_tmpB5E.q_restrict=0,((_tmpB5E.real_const=1,((_tmpB5E.loc=loc,_tmpB5E)))))))))));}
break;}case 100: _LL29C: {
# 1708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1706 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB5F;yyval=Cyc_YY23(((_tmpB5F.print_const=0,((_tmpB5F.q_volatile=1,((_tmpB5F.q_restrict=0,((_tmpB5F.real_const=0,((_tmpB5F.loc=0,_tmpB5F)))))))))));}
break;}case 101: _LL29D: {
# 1709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1707 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB60;yyval=Cyc_YY23(((_tmpB60.print_const=0,((_tmpB60.q_volatile=0,((_tmpB60.q_restrict=1,((_tmpB60.real_const=0,((_tmpB60.loc=0,_tmpB60)))))))))));}
break;}case 102: _LL29E: {
# 1710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB6F;struct Cyc_Absyn_Enumdecl*_tmpB6E;struct Cyc_Core_Opt*_tmpB6D;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB6C;struct Cyc_Absyn_TypeDecl*_tmpB6B;struct Cyc_Absyn_TypeDecl*_tmp57B=(_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->r=(void*)((_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F[0]=((_tmpB6C.tag=1,((_tmpB6C.f1=((_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E->sc=Cyc_Absyn_Public,((_tmpB6E->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB6E->fields=((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpB6D)))),_tmpB6E)))))))),_tmpB6C)))),_tmpB6F)))),((_tmpB6B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB6B)))));
# 1715
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB72;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB71;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71[0]=((_tmpB72.tag=26,((_tmpB72.f1=_tmp57B,((_tmpB72.f2=0,_tmpB72)))))),_tmpB71)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1717
break;}case 103: _LL29F: {
# 1719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1718 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB75;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB74;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74[0]=((_tmpB75.tag=13,((_tmpB75.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB75.f2=0,_tmpB75)))))),_tmpB74)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 104: _LL2A0: {
# 1721
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1723
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1720 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB78;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB77;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((_tmpB77[0]=((_tmpB78.tag=14,((_tmpB78.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB78)))),_tmpB77)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 105: _LL2A1: {
# 1723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB79;yyval=Cyc_YY47(((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB79->tag=0,((_tmpB79->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB79)))))))));}
break;}case 106: _LL2A2: {
# 1729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1728 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB7A;yyval=Cyc_YY47(((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB7A->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB7A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB7A)))))))));}
break;}case 107: _LL2A3: {
# 1731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1732 "parse.y"
{struct Cyc_List_List*_tmpB7B;yyval=Cyc_YY48(((_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB7B->tl=0,_tmpB7B)))))));}
break;}case 108: _LL2A4: {
# 1735
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1733 "parse.y"
{struct Cyc_List_List*_tmpB7C;yyval=Cyc_YY48(((_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB7C->tl=0,_tmpB7C)))))));}
break;}case 109: _LL2A5: {
# 1736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1734 "parse.y"
{struct Cyc_List_List*_tmpB7D;yyval=Cyc_YY48(((_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB7D->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB7D)))))));}
break;}case 110: _LL2A6: {
# 1737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB80;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB7F;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB80.tag=12,((_tmpB80.f1=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB80.f2=Cyc_yyget_YY24(yyyyvsp[2]),_tmpB80)))))),_tmpB7F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 111: _LL2A7: {
# 1743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1746 "parse.y"
struct Cyc_List_List*_tmp58E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp58F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp590=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp58E,
Cyc_Absyn_aggrdecl_impl(_tmp58F,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY24(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1751
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB83;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB82;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82[0]=((_tmpB83.tag=26,((_tmpB83.f1=_tmp590,((_tmpB83.f2=0,_tmpB83)))))),_tmpB82)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1753
break;}case 112: _LL2A8: {
# 1755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1757 "parse.y"
struct Cyc_List_List*_tmp593=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp594=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp595=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp593,
Cyc_Absyn_aggrdecl_impl(_tmp594,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY24(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1762
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB86;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB85;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB86.tag=26,((_tmpB86.f1=_tmp595,((_tmpB86.f2=0,_tmpB86)))))),_tmpB85)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1764
break;}case 113: _LL2A9: {
# 1766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1765 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB90;struct Cyc_Core_Opt*_tmpB8F;struct Cyc_Absyn_AggrInfo _tmpB8E;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB8D;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D[0]=((_tmpB90.tag=11,((_tmpB90.f1=((_tmpB8E.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB8F=_cycalloc_atomic(sizeof(*_tmpB8F)),((_tmpB8F->v=(void*)1,_tmpB8F))))),((_tmpB8E.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpB8E)))),_tmpB90)))),_tmpB8D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1768
break;}case 114: _LL2AA: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1769 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB96;struct Cyc_Absyn_AggrInfo _tmpB95;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB94;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB94=_cycalloc(sizeof(*_tmpB94)),((_tmpB94[0]=((_tmpB96.tag=11,((_tmpB96.f1=((_tmpB95.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB95.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB95)))),_tmpB96)))),_tmpB94)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1772
break;}case 115: _LL2AB:
# 1774
 yyval=Cyc_YY40(0);
break;case 116: _LL2AC: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1778 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
break;}case 117: _LL2AD: {
# 1781
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1783
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1782 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
break;}case 118: _LL2AE: {
# 1785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1783 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
break;}case 119: _LL2AF:
# 1786
 yyval=Cyc_YY24(0);
break;case 120: _LL2B0: {
# 1789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1790 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp59F=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp59F != 0;_tmp59F=_tmp59F->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp59F->hd,decls);}}{
# 1794
struct Cyc_List_List*_tmp5A0=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp5A0 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp5A0,decls);
yyval=Cyc_YY24(decls);
# 1799
break;};}case 121: _LL2B1: {
# 1801
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB97;yyval=Cyc_YY25(((_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97->hd=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB97->tl=0,_tmpB97)))))));}
break;}case 122: _LL2B2: {
# 1807
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1809
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1806 "parse.y"
{struct Cyc_List_List*_tmpB98;yyval=Cyc_YY25(((_tmpB98=_cycalloc(sizeof(*_tmpB98)),((_tmpB98->hd=Cyc_yyget_YY24(yyyyvsp[1]),((_tmpB98->tl=Cyc_yyget_YY25(yyyyvsp[0]),_tmpB98)))))));}
break;}case 123: _LL2B3: {
# 1809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1810 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
break;}case 124: _LL2B4: {
# 1813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB99;yyval=Cyc_YY19(((_tmpB99=_region_malloc(yyr,sizeof(*_tmpB99)),((_tmpB99->tl=0,((_tmpB99->hd=Cyc_yyget_YY18(yyyyvsp[0]),_tmpB99)))))));}
break;}case 125: _LL2B5: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1818 "parse.y"
{struct _tuple12*_tmpB9A;yyval=Cyc_YY19(((_tmpB9A=_region_malloc(yyr,sizeof(*_tmpB9A)),((_tmpB9A->tl=Cyc_yyget_YY19(yyyyvsp[0]),((_tmpB9A->hd=Cyc_yyget_YY18(yyyyvsp[2]),_tmpB9A)))))));}
break;}case 126: _LL2B6: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1823 "parse.y"
{struct _tuple11 _tmpB9B;yyval=Cyc_YY18(((_tmpB9B.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB9B.f2=0,_tmpB9B)))));}
break;}case 127: _LL2B7: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1825 "parse.y"
{struct _tuple11 _tmpB9C;yyval=Cyc_YY18(((_tmpB9C.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB9C.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB9C)))));}
break;}case 128: _LL2B8: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1831 "parse.y"
struct _RegionHandle _tmp5A7=_new_region("temp");struct _RegionHandle*temp=& _tmp5A7;_push_region(temp);{
struct _tuple25 _tmp5A8=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp5AA;struct Cyc_Parse_Type_specifier _tmp5AB;struct Cyc_List_List*_tmp5AC;struct _tuple25 _tmp5A9=_tmp5A8;_tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;_tmp5AC=_tmp5A9.f3;
if(_tmp5AA.loc == 0)_tmp5AA.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp5AD=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp5AD != 0;_tmp5AD=_tmp5AD->tl){
struct _tuple24*_tmp5AE=(struct _tuple24*)_tmp5AD->hd;struct Cyc_Parse_Declarator _tmp5B0;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B2;struct _tuple24*_tmp5AF=_tmp5AE;_tmp5B0=_tmp5AF->f1;_tmp5B1=_tmp5AF->f2;_tmp5B2=_tmp5AF->f3;
{struct _tuple10*_tmpB9D;decls=((_tmpB9D=_region_malloc(temp,sizeof(*_tmpB9D)),((_tmpB9D->tl=decls,((_tmpB9D->hd=_tmp5B0,_tmpB9D))))));}{
struct _tuple16*_tmpBA0;struct Cyc_List_List*_tmpB9F;widths_and_reqs=(
(_tmpB9F=_region_malloc(temp,sizeof(*_tmpB9F)),((_tmpB9F->hd=((_tmpBA0=_region_malloc(temp,sizeof(*_tmpBA0)),((_tmpBA0->f1=_tmp5B1,((_tmpBA0->f2=_tmp5B2,_tmpBA0)))))),((_tmpB9F->tl=widths_and_reqs,_tmpB9F))))));};}}
# 1842
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp5B6=Cyc_Parse_speclist2typ(_tmp5AB,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp5B7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp5AA,_tmp5B6,decls,_tmp5AC),widths_and_reqs);
# 1848
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp5B7));};};}
# 1851
_npop_handler(0);break;
# 1831
;_pop_region(temp);}case 129: _LL2B9: {
# 1853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1859 "parse.y"
{struct _tuple25 _tmpBA1;yyval=Cyc_YY35(((_tmpBA1.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpBA1.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpBA1.f3=0,_tmpBA1)))))));}
break;}case 130: _LL2BA: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1861 "parse.y"
struct _tuple25 _tmp5B9=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpBA2;yyval=Cyc_YY35(((_tmpBA2.f1=_tmp5B9.f1,((_tmpBA2.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5B9.f2),((_tmpBA2.f3=_tmp5B9.f3,_tmpBA2)))))));}
break;}case 131: _LL2BB: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1863 "parse.y"
{struct _tuple25 _tmpBA3;yyval=Cyc_YY35(((_tmpBA3.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpBA3.f2=Cyc_Parse_empty_spec(0),((_tmpBA3.f3=0,_tmpBA3)))))));}
break;}case 132: _LL2BC: {
# 1866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1865 "parse.y"
struct _tuple25 _tmp5BC=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpBA4;yyval=Cyc_YY35(((_tmpBA4.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5BC.f1),((_tmpBA4.f2=_tmp5BC.f2,((_tmpBA4.f3=_tmp5BC.f3,_tmpBA4)))))));}
break;}case 133: _LL2BD: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1868 "parse.y"
{struct _tuple25 _tmpBA5;yyval=Cyc_YY35(((_tmpBA5.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpBA5.f2=Cyc_Parse_empty_spec(0),((_tmpBA5.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpBA5)))))));}
break;}case 134: _LL2BE: {
# 1871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1870 "parse.y"
struct _tuple25 _tmp5BF=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpBA6;yyval=Cyc_YY35(((_tmpBA6.f1=_tmp5BF.f1,((_tmpBA6.f2=_tmp5BF.f2,((_tmpBA6.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5BF.f3),_tmpBA6)))))));}
break;}case 135: _LL2BF: {
# 1873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpBA7;yyval=Cyc_YY35(((_tmpBA7.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpBA7.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpBA7.f3=0,_tmpBA7)))))));}
break;}case 136: _LL2C0: {
# 1879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1878 "parse.y"
struct _tuple25 _tmp5C2=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpBA8;yyval=Cyc_YY35(((_tmpBA8.f1=_tmp5C2.f1,((_tmpBA8.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5C2.f2),((_tmpBA8.f3=_tmp5C2.f3,_tmpBA8)))))));}
break;}case 137: _LL2C1: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1880 "parse.y"
{struct _tuple25 _tmpBA9;yyval=Cyc_YY35(((_tmpBA9.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpBA9.f2=Cyc_Parse_empty_spec(0),((_tmpBA9.f3=0,_tmpBA9)))))));}
break;}case 138: _LL2C2: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1882 "parse.y"
struct _tuple25 _tmp5C5=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpBAA;yyval=Cyc_YY35(((_tmpBAA.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5C5.f1),((_tmpBAA.f2=_tmp5C5.f2,((_tmpBAA.f3=_tmp5C5.f3,_tmpBAA)))))));}
break;}case 139: _LL2C3: {
# 1886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1885 "parse.y"
{struct _tuple25 _tmpBAB;yyval=Cyc_YY35(((_tmpBAB.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpBAB.f2=Cyc_Parse_empty_spec(0),((_tmpBAB.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpBAB)))))));}
break;}case 140: _LL2C4: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1887 "parse.y"
struct _tuple25 _tmp5C8=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpBAC;yyval=Cyc_YY35(((_tmpBAC.f1=_tmp5C8.f1,((_tmpBAC.f2=_tmp5C8.f2,((_tmpBAC.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5C8.f3),_tmpBAC)))))));}
break;}case 141: _LL2C5: {
# 1890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1891 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
break;}case 142: _LL2C6: {
# 1894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpBAD;yyval=Cyc_YY29(((_tmpBAD=_region_malloc(yyr,sizeof(*_tmpBAD)),((_tmpBAD->hd=Cyc_yyget_YY28(yyyyvsp[0]),((_tmpBAD->tl=0,_tmpBAD)))))));}
break;}case 143: _LL2C7: {
# 1900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1899 "parse.y"
{struct Cyc_List_List*_tmpBAE;yyval=Cyc_YY29(((_tmpBAE=_region_malloc(yyr,sizeof(*_tmpBAE)),((_tmpBAE->hd=Cyc_yyget_YY28(yyyyvsp[2]),((_tmpBAE->tl=Cyc_yyget_YY29(yyyyvsp[0]),_tmpBAE)))))));}
break;}case 144: _LL2C8: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1904 "parse.y"
{struct _tuple24*_tmpBAF;yyval=Cyc_YY28(((_tmpBAF=_region_malloc(yyr,sizeof(*_tmpBAF)),((_tmpBAF->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpBAF->f2=0,((_tmpBAF->f3=Cyc_yyget_YY55(yyyyvsp[1]),_tmpBAF)))))))));}
break;}case 145: _LL2C9: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1908 "parse.y"
{struct Cyc_Parse_Declarator _tmpBB5;struct _tuple0*_tmpBB4;struct _tuple24*_tmpBB3;yyval=Cyc_YY28(((_tmpBB3=_region_malloc(yyr,sizeof(*_tmpBB3)),((_tmpBB3->f1=((_tmpBB5.id=((_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4->f1=Cyc_Absyn_Rel_n(0),((_tmpBB4->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpBB4)))))),((_tmpBB5.tms=0,_tmpBB5)))),((_tmpBB3->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpBB3->f3=0,_tmpBB3)))))))));}
# 1910
break;}case 146: _LL2CA:
# 1914
{struct Cyc_Parse_Declarator _tmpBBB;struct _tuple0*_tmpBBA;struct _tuple24*_tmpBB9;yyval=Cyc_YY28(((_tmpBB9=_region_malloc(yyr,sizeof(*_tmpBB9)),((_tmpBB9->f1=((_tmpBBB.id=((_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->f1=Cyc_Absyn_Rel_n(0),((_tmpBBA->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpBBA)))))),((_tmpBBB.tms=0,_tmpBBB)))),((_tmpBB9->f2=0,((_tmpBB9->f3=0,_tmpBB9)))))))));}
# 1916
break;case 147: _LL2CB: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1916 "parse.y"
{struct _tuple24*_tmpBBC;yyval=Cyc_YY28(((_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpBBC->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBBC->f3=0,_tmpBBC)))))))));}
break;}case 148: _LL2CC: {
# 1919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1920 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 149: _LL2CD:
# 1923
 yyval=Cyc_YY55(0);
break;case 150: _LL2CE: {
# 1926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1925 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 151: _LL2CF:
# 1928
 yyval=Cyc_YY55(0);
break;case 152: _LL2D0: {
# 1931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1932 "parse.y"
int _tmp5D6=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp5D7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpBBD;struct Cyc_Absyn_TypeDecl*_tmp5D8=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5D7,((_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpBBD)))),_tmp5D6,(unsigned int)(yyyylsp[0]).first_line);
# 1936
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpBC0;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBBF;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF[0]=((_tmpBC0.tag=26,((_tmpBC0.f1=_tmp5D8,((_tmpBC0.f2=0,_tmpBC0)))))),_tmpBBF)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1938
break;}case 153: _LL2D1: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1939 "parse.y"
int _tmp5DC=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpBCA;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBC9;struct Cyc_Absyn_DatatypeInfo _tmpBC8;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpBC7;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBCA.tag=3,((_tmpBCA.f1=((_tmpBC8.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpBC9.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBC9.is_extensible=_tmp5DC,_tmpBC9))))),((_tmpBC8.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpBC8)))),_tmpBCA)))),_tmpBC7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1942
break;}case 154: _LL2D2: {
# 1944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1943 "parse.y"
int _tmp5E1=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpBD4;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBD3;struct Cyc_Absyn_DatatypeFieldInfo _tmpBD2;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpBD1;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD4.tag=4,((_tmpBD4.f1=((_tmpBD2.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpBD3.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBD3.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpBD3.is_extensible=_tmp5E1,_tmpBD3))))))),((_tmpBD2.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpBD2)))),_tmpBD4)))),_tmpBD1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1948
break;}case 155: _LL2D3: {
# 1950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1951 "parse.y"
yyval=Cyc_YY31(0);
break;}case 156: _LL2D4: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1952 "parse.y"
yyval=Cyc_YY31(1);
break;}case 157: _LL2D5: {
# 1955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1956 "parse.y"
{struct Cyc_List_List*_tmpBD5;yyval=Cyc_YY34(((_tmpBD5=_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBD5->tl=0,_tmpBD5)))))));}
break;}case 158: _LL2D6: {
# 1959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1957 "parse.y"
{struct Cyc_List_List*_tmpBD6;yyval=Cyc_YY34(((_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBD6->tl=0,_tmpBD6)))))));}
break;}case 159: _LL2D7: {
# 1960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1958 "parse.y"
{struct Cyc_List_List*_tmpBD7;yyval=Cyc_YY34(((_tmpBD7=_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBD7->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBD7)))))));}
break;}case 160: _LL2D8: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1959 "parse.y"
{struct Cyc_List_List*_tmpBD8;yyval=Cyc_YY34(((_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBD8->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBD8)))))));}
break;}case 161: _LL2D9:
# 1962
 yyval=Cyc_YY32(Cyc_Absyn_Public);
break;case 162: _LL2DA: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1964 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
break;}case 163: _LL2DB: {
# 1967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1965 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
break;}case 164: _LL2DC: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1969 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpBD9;yyval=Cyc_YY33(((_tmpBD9=_cycalloc(sizeof(*_tmpBD9)),((_tmpBD9->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBD9->typs=0,((_tmpBD9->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBD9->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBD9)))))))))));}
break;}case 165: _LL2DD: {
# 1972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1971 "parse.y"
struct Cyc_List_List*_tmp5EB=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpBDA;yyval=Cyc_YY33(((_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBDA->typs=_tmp5EB,((_tmpBDA->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBDA->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBDA)))))))))));}
break;}case 166: _LL2DE: {
# 1975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1977 "parse.y"
yyval=yyyyvsp[0];
break;}case 167: _LL2DF: {
# 1980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1979 "parse.y"
struct Cyc_Parse_Declarator _tmp5ED=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBDB;yyval=Cyc_YY27(((_tmpBDB.id=_tmp5ED.id,((_tmpBDB.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5ED.tms),_tmpBDB)))));}
break;}case 168: _LL2E0: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 169: _LL2E1: {
# 1989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1988 "parse.y"
struct Cyc_Parse_Declarator _tmp5EF=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBDC;yyval=Cyc_YY27(((_tmpBDC.id=_tmp5EF.id,((_tmpBDC.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5EF.tms),_tmpBDC)))));}
break;}case 170: _LL2E2: {
# 1992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1994 "parse.y"
{struct Cyc_Parse_Declarator _tmpBDD;yyval=Cyc_YY27(((_tmpBDD.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBDD.tms=0,_tmpBDD)))));}
break;}case 171: _LL2E3: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1996 "parse.y"
yyval=yyyyvsp[1];
break;}case 172: _LL2E4: {
# 1999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2000 "parse.y"
struct Cyc_Parse_Declarator _tmp5F2=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBE3;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBE2;struct Cyc_List_List*_tmpBE1;_tmp5F2.tms=((_tmpBE1=_region_malloc(yyr,sizeof(*_tmpBE1)),((_tmpBE1->hd=(void*)((_tmpBE3=_region_malloc(yyr,sizeof(*_tmpBE3)),((_tmpBE3[0]=((_tmpBE2.tag=5,((_tmpBE2.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBE2.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBE2)))))),_tmpBE3)))),((_tmpBE1->tl=_tmp5F2.tms,_tmpBE1))))));}
yyval=yyyyvsp[2];
# 2004
break;}case 173: _LL2E5: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2005 "parse.y"
{struct Cyc_List_List*_tmpBED;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBEC;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBEB;struct Cyc_Parse_Declarator _tmpBEA;yyval=Cyc_YY27(((_tmpBEA.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBEA.tms=((_tmpBED=_region_malloc(yyr,sizeof(*_tmpBED)),((_tmpBED->hd=(void*)((_tmpBEB=_region_malloc(yyr,sizeof(*_tmpBEB)),((_tmpBEB[0]=((_tmpBEC.tag=0,((_tmpBEC.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBEC.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBEC)))))),_tmpBEB)))),((_tmpBED->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBED)))))),_tmpBEA)))));}
break;}case 174: _LL2E6: {
# 2008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2007 "parse.y"
{struct Cyc_List_List*_tmpBF7;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBF6;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBF5;struct Cyc_Parse_Declarator _tmpBF4;yyval=Cyc_YY27(((_tmpBF4.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBF4.tms=(
(_tmpBF7=_region_malloc(yyr,sizeof(*_tmpBF7)),((_tmpBF7->hd=(void*)((_tmpBF5=_region_malloc(yyr,sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF6.tag=1,((_tmpBF6.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBF6.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBF6.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBF6)))))))),_tmpBF5)))),((_tmpBF7->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBF7)))))),_tmpBF4)))));}
break;}case 175: _LL2E7: {
# 2011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2010 "parse.y"
struct _tuple26*_tmp5FE=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp600;int _tmp601;struct Cyc_Absyn_VarargInfo*_tmp602;void*_tmp603;struct Cyc_List_List*_tmp604;struct _tuple26*_tmp5FF=_tmp5FE;_tmp600=_tmp5FF->f1;_tmp601=_tmp5FF->f2;_tmp602=_tmp5FF->f3;_tmp603=_tmp5FF->f4;_tmp604=_tmp5FF->f5;{
struct Cyc_Absyn_Exp*_tmp605=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp606=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_List_List*_tmpC0C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC0B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC0A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC09;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC08;struct Cyc_Parse_Declarator _tmpC07;yyval=Cyc_YY27(((_tmpC07.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC07.tms=((_tmpC0C=_region_malloc(yyr,sizeof(*_tmpC0C)),((_tmpC0C->hd=(void*)((_tmpC08=_region_malloc(yyr,sizeof(*_tmpC08)),((_tmpC08[0]=((_tmpC0B.tag=3,((_tmpC0B.f1=(void*)((_tmpC09=_region_malloc(yyr,sizeof(*_tmpC09)),((_tmpC09[0]=((_tmpC0A.tag=1,((_tmpC0A.f1=_tmp600,((_tmpC0A.f2=_tmp601,((_tmpC0A.f3=_tmp602,((_tmpC0A.f4=_tmp603,((_tmpC0A.f5=_tmp604,((_tmpC0A.f6=_tmp605,((_tmpC0A.f7=_tmp606,_tmpC0A)))))))))))))))),_tmpC09)))),_tmpC0B)))),_tmpC08)))),((_tmpC0C->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC0C)))))),_tmpC07)))));}
# 2015
break;};}case 176: _LL2E8: {
# 2017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2016 "parse.y"
{struct Cyc_List_List*_tmpC21;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC20;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC1F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC1E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC1D;struct Cyc_Parse_Declarator _tmpC1C;yyval=Cyc_YY27(((_tmpC1C.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC1C.tms=(
(_tmpC21=_region_malloc(yyr,sizeof(*_tmpC21)),((_tmpC21->hd=(void*)((_tmpC1D=_region_malloc(yyr,sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC20.tag=3,((_tmpC20.f1=(void*)((_tmpC1E=_region_malloc(yyr,sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC1F.tag=1,((_tmpC1F.f1=0,((_tmpC1F.f2=0,((_tmpC1F.f3=0,((_tmpC1F.f4=
# 2019
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC1F.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC1F.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC1F.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC1F)))))))))))))))),_tmpC1E)))),_tmpC20)))),_tmpC1D)))),((_tmpC21->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC21)))))),_tmpC1C)))));}
# 2022
break;}case 177: _LL2E9: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2023 "parse.y"
{struct Cyc_List_List*_tmpC36;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC35;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC34;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC33;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC32;struct Cyc_Parse_Declarator _tmpC31;yyval=Cyc_YY27(((_tmpC31.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC31.tms=((_tmpC36=_region_malloc(yyr,sizeof(*_tmpC36)),((_tmpC36->hd=(void*)((_tmpC32=_region_malloc(yyr,sizeof(*_tmpC32)),((_tmpC32[0]=((_tmpC35.tag=3,((_tmpC35.f1=(void*)((_tmpC33=_region_malloc(yyr,sizeof(*_tmpC33)),((_tmpC33[0]=((_tmpC34.tag=0,((_tmpC34.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC34.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC34)))))),_tmpC33)))),_tmpC35)))),_tmpC32)))),((_tmpC36->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC36)))))),_tmpC31)))));}
break;}case 178: _LL2EA: {
# 2026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2026 "parse.y"
struct Cyc_List_List*_tmp619=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpC40;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC3F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC3E;struct Cyc_Parse_Declarator _tmpC3D;yyval=Cyc_YY27(((_tmpC3D.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC3D.tms=((_tmpC40=_region_malloc(yyr,sizeof(*_tmpC40)),((_tmpC40->hd=(void*)((_tmpC3E=_region_malloc(yyr,sizeof(*_tmpC3E)),((_tmpC3E[0]=((_tmpC3F.tag=4,((_tmpC3F.f1=_tmp619,((_tmpC3F.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC3F.f3=0,_tmpC3F)))))))),_tmpC3E)))),((_tmpC40->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC40)))))),_tmpC3D)))));}
# 2029
break;}case 179: _LL2EB: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2030 "parse.y"
{struct Cyc_List_List*_tmpC4A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC49;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC48;struct Cyc_Parse_Declarator _tmpC47;yyval=Cyc_YY27(((_tmpC47.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC47.tms=((_tmpC4A=_region_malloc(yyr,sizeof(*_tmpC4A)),((_tmpC4A->hd=(void*)((_tmpC48=_region_malloc(yyr,sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=5,((_tmpC49.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC49.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC49)))))),_tmpC48)))),((_tmpC4A->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC4A)))))),_tmpC47)))));}
# 2033
break;}case 180: _LL2EC: {
# 2035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2044 "parse.y"
{struct Cyc_Parse_Declarator _tmpC4B;yyval=Cyc_YY27(((_tmpC4B.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC4B.tms=0,_tmpC4B)))));}
break;}case 181: _LL2ED: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2046 "parse.y"
{struct Cyc_Parse_Declarator _tmpC4C;yyval=Cyc_YY27(((_tmpC4C.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC4C.tms=0,_tmpC4C)))));}
break;}case 182: _LL2EE: {
# 2049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2048 "parse.y"
yyval=yyyyvsp[1];
break;}case 183: _LL2EF: {
# 2051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2052 "parse.y"
struct Cyc_Parse_Declarator _tmp624=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC52;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC51;struct Cyc_List_List*_tmpC50;_tmp624.tms=((_tmpC50=_region_malloc(yyr,sizeof(*_tmpC50)),((_tmpC50->hd=(void*)((_tmpC52=_region_malloc(yyr,sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC51.tag=5,((_tmpC51.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC51.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC51)))))),_tmpC52)))),((_tmpC50->tl=_tmp624.tms,_tmpC50))))));}
yyval=yyyyvsp[2];
# 2056
break;}case 184: _LL2F0: {
# 2058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2057 "parse.y"
struct Cyc_Parse_Declarator _tmp628=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC5C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC5B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC5A;struct Cyc_Parse_Declarator _tmpC59;yyval=Cyc_YY27(((_tmpC59.id=_tmp628.id,((_tmpC59.tms=(
(_tmpC5C=_region_malloc(yyr,sizeof(*_tmpC5C)),((_tmpC5C->hd=(void*)((_tmpC5A=_region_malloc(yyr,sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC5B.tag=0,((_tmpC5B.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpC5B.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC5B)))))),_tmpC5A)))),((_tmpC5C->tl=_tmp628.tms,_tmpC5C)))))),_tmpC59)))));}
break;}case 185: _LL2F1: {
# 2062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2061 "parse.y"
struct Cyc_Parse_Declarator _tmp62D=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC66;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC65;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC64;struct Cyc_Parse_Declarator _tmpC63;yyval=Cyc_YY27(((_tmpC63.id=_tmp62D.id,((_tmpC63.tms=(
(_tmpC66=_region_malloc(yyr,sizeof(*_tmpC66)),((_tmpC66->hd=(void*)((_tmpC64=_region_malloc(yyr,sizeof(*_tmpC64)),((_tmpC64[0]=((_tmpC65.tag=1,((_tmpC65.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpC65.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC65.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC65)))))))),_tmpC64)))),((_tmpC66->tl=_tmp62D.tms,_tmpC66)))))),_tmpC63)))));}
# 2065
break;}case 186: _LL2F2: {
# 2067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2066 "parse.y"
struct _tuple26*_tmp632=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp634;int _tmp635;struct Cyc_Absyn_VarargInfo*_tmp636;void*_tmp637;struct Cyc_List_List*_tmp638;struct _tuple26*_tmp633=_tmp632;_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;_tmp636=_tmp633->f3;_tmp637=_tmp633->f4;_tmp638=_tmp633->f5;{
struct Cyc_Absyn_Exp*_tmp639=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp63A=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp63B=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC7B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC7A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC79;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC78;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC77;struct Cyc_Parse_Declarator _tmpC76;yyval=Cyc_YY27(((_tmpC76.id=_tmp63B.id,((_tmpC76.tms=((_tmpC7B=_region_malloc(yyr,sizeof(*_tmpC7B)),((_tmpC7B->hd=(void*)((_tmpC77=_region_malloc(yyr,sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC7A.tag=3,((_tmpC7A.f1=(void*)((_tmpC78=_region_malloc(yyr,sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC79.tag=1,((_tmpC79.f1=_tmp634,((_tmpC79.f2=_tmp635,((_tmpC79.f3=_tmp636,((_tmpC79.f4=_tmp637,((_tmpC79.f5=_tmp638,((_tmpC79.f6=_tmp639,((_tmpC79.f7=_tmp63A,_tmpC79)))))))))))))))),_tmpC78)))),_tmpC7A)))),_tmpC77)))),((_tmpC7B->tl=_tmp63B.tms,_tmpC7B)))))),_tmpC76)))));}
# 2072
break;};}case 187: _LL2F3: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2073 "parse.y"
struct Cyc_Parse_Declarator _tmp642=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC90;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC8F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC8E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC8D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC8C;struct Cyc_Parse_Declarator _tmpC8B;yyval=Cyc_YY27(((_tmpC8B.id=_tmp642.id,((_tmpC8B.tms=(
(_tmpC90=_region_malloc(yyr,sizeof(*_tmpC90)),((_tmpC90->hd=(void*)((_tmpC8C=_region_malloc(yyr,sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8F.tag=3,((_tmpC8F.f1=(void*)((_tmpC8D=_region_malloc(yyr,sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8E.tag=1,((_tmpC8E.f1=0,((_tmpC8E.f2=0,((_tmpC8E.f3=0,((_tmpC8E.f4=
# 2077
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC8E.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC8E.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC8E.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC8E)))))))))))))))),_tmpC8D)))),_tmpC8F)))),_tmpC8C)))),((_tmpC90->tl=_tmp642.tms,_tmpC90)))))),_tmpC8B)))));}
# 2080
break;}case 188: _LL2F4: {
# 2082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2081 "parse.y"
struct Cyc_Parse_Declarator _tmp649=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpCA5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpCA4;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpCA3;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpCA2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpCA1;struct Cyc_Parse_Declarator _tmpCA0;yyval=Cyc_YY27(((_tmpCA0.id=_tmp649.id,((_tmpCA0.tms=((_tmpCA5=_region_malloc(yyr,sizeof(*_tmpCA5)),((_tmpCA5->hd=(void*)((_tmpCA1=_region_malloc(yyr,sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA4.tag=3,((_tmpCA4.f1=(void*)((_tmpCA2=_region_malloc(yyr,sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA3.tag=0,((_tmpCA3.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpCA3.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpCA3)))))),_tmpCA2)))),_tmpCA4)))),_tmpCA1)))),((_tmpCA5->tl=_tmp649.tms,_tmpCA5)))))),_tmpCA0)))));}
break;}case 189: _LL2F5: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2085 "parse.y"
struct Cyc_List_List*_tmp650=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp651=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpCAF;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpCAE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpCAD;struct Cyc_Parse_Declarator _tmpCAC;yyval=Cyc_YY27(((_tmpCAC.id=_tmp651.id,((_tmpCAC.tms=((_tmpCAF=_region_malloc(yyr,sizeof(*_tmpCAF)),((_tmpCAF->hd=(void*)((_tmpCAD=_region_malloc(yyr,sizeof(*_tmpCAD)),((_tmpCAD[0]=((_tmpCAE.tag=4,((_tmpCAE.f1=_tmp650,((_tmpCAE.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpCAE.f3=0,_tmpCAE)))))))),_tmpCAD)))),((_tmpCAF->tl=_tmp651.tms,_tmpCAF)))))),_tmpCAC)))));}
# 2089
break;}case 190: _LL2F6: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2090 "parse.y"
struct Cyc_Parse_Declarator _tmp656=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpCB9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpCB8;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpCB7;struct Cyc_Parse_Declarator _tmpCB6;yyval=Cyc_YY27(((_tmpCB6.id=_tmp656.id,((_tmpCB6.tms=((_tmpCB9=_region_malloc(yyr,sizeof(*_tmpCB9)),((_tmpCB9->hd=(void*)((_tmpCB7=_region_malloc(yyr,sizeof(*_tmpCB7)),((_tmpCB7[0]=((_tmpCB8.tag=5,((_tmpCB8.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpCB8.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpCB8)))))),_tmpCB7)))),((_tmpCB9->tl=_tmp656.tms,_tmpCB9)))))),_tmpCB6)))));}
# 2093
break;}case 191: _LL2F7: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2103 "parse.y"
yyval=yyyyvsp[0];
break;}case 192: _LL2F8: {
# 2106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2104 "parse.y"
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 193: _LL2F9: {
# 2107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2108 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpCBF;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpCBE;struct Cyc_List_List*_tmpCBD;ans=((_tmpCBD=_region_malloc(yyr,sizeof(*_tmpCBD)),((_tmpCBD->hd=(void*)((_tmpCBF=_region_malloc(yyr,sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCBE.tag=5,((_tmpCBE.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpCBE.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpCBE)))))),_tmpCBF)))),((_tmpCBD->tl=ans,_tmpCBD))))));}{
# 2112
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp65E=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp660;union Cyc_Absyn_Constraint*_tmp661;union Cyc_Absyn_Constraint*_tmp662;struct _tuple21 _tmp65F=_tmp65E;_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;_tmp662=_tmp65F.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpCC0;ptrloc=((_tmpCC0=_cycalloc_atomic(sizeof(*_tmpCC0)),((_tmpCC0->ptr_loc=_tmp660,((_tmpCC0->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpCC0->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpCC0))))))));}{
# 2117
struct _tuple14 _tmp664=Cyc_Parse_collapse_pointer_quals(_tmp660,_tmp661,_tmp662,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp666;union Cyc_Absyn_Constraint*_tmp667;union Cyc_Absyn_Constraint*_tmp668;void*_tmp669;struct _tuple14 _tmp665=_tmp664;_tmp666=_tmp665.f1;_tmp667=_tmp665.f2;_tmp668=_tmp665.f3;_tmp669=_tmp665.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpCCA;struct Cyc_Absyn_PtrAtts _tmpCC9;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpCC8;struct Cyc_List_List*_tmpCC7;ans=((_tmpCC7=_region_malloc(yyr,sizeof(*_tmpCC7)),((_tmpCC7->hd=(void*)((_tmpCCA=_region_malloc(yyr,sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCC8.tag=2,((_tmpCC8.f1=((_tmpCC9.rgn=_tmp669,((_tmpCC9.nullable=_tmp666,((_tmpCC9.bounds=_tmp667,((_tmpCC9.zero_term=_tmp668,((_tmpCC9.ptrloc=ptrloc,_tmpCC9)))))))))),((_tmpCC8.f2=Cyc_yyget_YY23(yyyyvsp[4]),_tmpCC8)))))),_tmpCCA)))),((_tmpCC7->tl=ans,_tmpCC7))))));}
yyval=Cyc_YY26(ans);
# 2121
break;};};}case 194: _LL2FA:
# 2123
 yyval=Cyc_YY54(0);
break;case 195: _LL2FB: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2124 "parse.y"
{struct Cyc_List_List*_tmpCCB;yyval=Cyc_YY54(((_tmpCCB=_region_malloc(yyr,sizeof(*_tmpCCB)),((_tmpCCB->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpCCB->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpCCB)))))));}
break;}case 196: _LL2FC: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2129 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpCCE;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpCCD;yyval=Cyc_YY53((void*)((_tmpCCD=_region_malloc(yyr,sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCE.tag=0,((_tmpCCE.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpCCE)))),_tmpCCD)))));}
break;}case 197: _LL2FD: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2131 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpCD1;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpCD0;yyval=Cyc_YY53((void*)((_tmpCD0=_region_malloc(yyr,sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=1,((_tmpCD1.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCD1)))),_tmpCD0)))));}
break;}case 198: _LL2FE: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2133 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpCD4;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpCD3;yyval=Cyc_YY53((void*)((_tmpCD3=_region_malloc(yyr,sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=2,_tmpCD4)),_tmpCD3)))));}
break;}case 199: _LL2FF: {
# 2136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2135 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpCD7;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpCD6;yyval=Cyc_YY53((void*)((_tmpCD6=_region_malloc(yyr,sizeof(*_tmpCD6)),((_tmpCD6[0]=((_tmpCD7.tag=3,_tmpCD7)),_tmpCD6)))));}
break;}case 200: _LL300: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2137 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCDA;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCD9;yyval=Cyc_YY53((void*)((_tmpCD9=_region_malloc(yyr,sizeof(*_tmpCD9)),((_tmpCD9[0]=((_tmpCDA.tag=4,_tmpCDA)),_tmpCD9)))));}
break;}case 201: _LL301: {
# 2140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2139 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCDD;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCDC;yyval=Cyc_YY53((void*)((_tmpCDC=_region_malloc(yyr,sizeof(*_tmpCDC)),((_tmpCDC[0]=((_tmpCDD.tag=5,_tmpCDD)),_tmpCDC)))));}
break;}case 202: _LL302: {
# 2142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2141 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpCE0;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpCDF;yyval=Cyc_YY53((void*)((_tmpCDF=_region_malloc(yyr,sizeof(*_tmpCDF)),((_tmpCDF[0]=((_tmpCE0.tag=6,_tmpCE0)),_tmpCDF)))));}
break;}case 203: _LL303: {
# 2144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2143 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpCE3;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpCE2;yyval=Cyc_YY53((void*)((_tmpCE2=_region_malloc(yyr,sizeof(*_tmpCE2)),((_tmpCE2[0]=((_tmpCE3.tag=7,_tmpCE3)),_tmpCE2)))));}
break;}case 204: _LL304: {
# 2146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2149 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCE4;yyval=Cyc_YY1(((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4->f1=loc,((_tmpCE4->f2=Cyc_Absyn_true_conref,((_tmpCE4->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCE4)))))))));}
# 2152
break;}case 205: _LL305: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2153 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCE5;yyval=Cyc_YY1(((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5->f1=loc,((_tmpCE5->f2=Cyc_Absyn_false_conref,((_tmpCE5->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCE5)))))))));}
# 2156
break;}case 206: _LL306: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2157 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCE6;yyval=Cyc_YY1(((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6->f1=loc,((_tmpCE6->f2=Cyc_Absyn_true_conref,((_tmpCE6->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpCE6)))))))));}
# 2160
break;}case 207: _LL307:
# 2162
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 208: _LL308: {
# 2165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2163 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpCE9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCE8;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=((_tmpCE9.tag=1,((_tmpCE9.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpCE9)))),_tmpCE8))))));}
break;}case 209: _LL309:
# 2166
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 210: _LL30A: {
# 2169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2167 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;}case 211: _LL30B: {
# 2170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2168 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
break;}case 212: _LL30C:
# 2171
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 213: _LL30D: {
# 2174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2174 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 214: _LL30E: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2175 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 215: _LL30F:
# 2178
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 216: _LL310: {
# 2181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2180 "parse.y"
yyval=Cyc_YY23(Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),Cyc_yyget_YY23(yyyyvsp[1])));
break;}case 217: _LL311: {
# 2183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2185 "parse.y"
{struct _tuple26*_tmpCEA;yyval=Cyc_YY39(((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCEA->f2=0,((_tmpCEA->f3=0,((_tmpCEA->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpCEA->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpCEA)))))))))))));}
break;}case 218: _LL312: {
# 2188
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2187 "parse.y"
{struct _tuple26*_tmpCEB;yyval=Cyc_YY39(((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCEB->f2=1,((_tmpCEB->f3=0,((_tmpCEB->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCEB->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCEB)))))))))))));}
break;}case 219: _LL313: {
# 2190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2190 "parse.y"
struct _tuple8*_tmp686=Cyc_yyget_YY37(yyyyvsp[2]);struct _dyneither_ptr*_tmp688;struct Cyc_Absyn_Tqual _tmp689;void*_tmp68A;struct _tuple8*_tmp687=_tmp686;_tmp688=_tmp687->f1;_tmp689=_tmp687->f2;_tmp68A=_tmp687->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCEC;struct Cyc_Absyn_VarargInfo*_tmp68B=(_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->name=_tmp688,((_tmpCEC->tq=_tmp689,((_tmpCEC->type=_tmp68A,((_tmpCEC->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpCEC)))))))));
{struct _tuple26*_tmpCED;yyval=Cyc_YY39(((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED->f1=0,((_tmpCED->f2=0,((_tmpCED->f3=_tmp68B,((_tmpCED->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCED->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCED)))))))))))));}
# 2194
break;};}case 220: _LL314: {
# 2196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2196 "parse.y"
struct _tuple8*_tmp68E=Cyc_yyget_YY37(yyyyvsp[4]);struct _dyneither_ptr*_tmp690;struct Cyc_Absyn_Tqual _tmp691;void*_tmp692;struct _tuple8*_tmp68F=_tmp68E;_tmp690=_tmp68F->f1;_tmp691=_tmp68F->f2;_tmp692=_tmp68F->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCEE;struct Cyc_Absyn_VarargInfo*_tmp693=(_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE->name=_tmp690,((_tmpCEE->tq=_tmp691,((_tmpCEE->type=_tmp692,((_tmpCEE->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpCEE)))))))));
{struct _tuple26*_tmpCEF;yyval=Cyc_YY39(((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCEF->f2=0,((_tmpCEF->f3=_tmp693,((_tmpCEF->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpCEF->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpCEF)))))))))))));}
# 2200
break;};}case 221: _LL315: {
# 2202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2204 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCF2;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCF1;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=1,((_tmpCF2.f1=0,_tmpCF2)))),_tmpCF1))))));}
break;}case 222: _LL316: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2205 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
break;}case 223: _LL317:
# 2208
 yyval=Cyc_YY49(0);
break;case 224: _LL318: {
# 2211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2209 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCF4;yyval=Cyc_YY49((void*)((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4[0]=((_tmpCF5.tag=24,((_tmpCF5.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCF5)))),_tmpCF4)))));}
break;}case 225: _LL319:
# 2212
 yyval=Cyc_YY50(0);
break;case 226: _LL31A: {
# 2215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2214 "parse.y"
yyval=yyyyvsp[1];
break;}case 227: _LL31B: {
# 2217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2222 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCF8;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCF7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp69A=(_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7[0]=((_tmpCF8.tag=2,((_tmpCF8.f1=0,((_tmpCF8.f2=& Cyc_Tcutil_trk,_tmpCF8)))))),_tmpCF7)));
struct _dyneither_ptr _tmp69B=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp69C=Cyc_Parse_id2type(_tmp69B,(void*)_tmp69A);
{struct _tuple29*_tmpD02;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD01;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD00;struct Cyc_List_List*_tmpCFF;yyval=Cyc_YY50(((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->hd=((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->f1=(void*)((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00[0]=((_tmpD01.tag=24,((_tmpD01.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD01)))),_tmpD00)))),((_tmpD02->f2=_tmp69C,_tmpD02)))))),((_tmpCFF->tl=0,_tmpCFF)))))));}
# 2227
break;}case 228: _LL31C: {
# 2229
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2231
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2229 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpD05;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD04;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6A3=(_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=2,((_tmpD05.f1=0,((_tmpD05.f2=& Cyc_Tcutil_trk,_tmpD05)))))),_tmpD04)));
struct _dyneither_ptr _tmp6A4=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp6A5=Cyc_Parse_id2type(_tmp6A4,(void*)_tmp6A3);
{struct _tuple29*_tmpD0F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD0E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD0D;struct Cyc_List_List*_tmpD0C;yyval=Cyc_YY50(((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->hd=((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->f1=(void*)((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D[0]=((_tmpD0E.tag=24,((_tmpD0E.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD0E)))),_tmpD0D)))),((_tmpD0F->f2=_tmp6A5,_tmpD0F)))))),((_tmpD0C->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpD0C)))))));}
# 2234
break;}case 229: _LL31D:
# 2236
 yyval=Cyc_YY31(0);
break;case 230: _LL31E: {
# 2239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2240 "parse.y"
{const char*_tmpD10;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpD10="inject",_tag_dyneither(_tmpD10,sizeof(char),7))))!= 0){
const char*_tmpD11;Cyc_Parse_err(((_tmpD11="missing type in function declaration",_tag_dyneither(_tmpD11,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY31(1);
# 2244
break;}case 231: _LL31F: {
# 2246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2247 "parse.y"
yyval=yyyyvsp[0];
break;}case 232: _LL320: {
# 2250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2248 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
break;}case 233: _LL321: {
# 2251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2252 "parse.y"
yyval=Cyc_YY40(0);
break;}case 234: _LL322: {
# 2255
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2253 "parse.y"
yyval=yyyyvsp[1];
break;}case 235: _LL323: {
# 2256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2255 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD17;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD16;struct Cyc_List_List*_tmpD15;yyval=Cyc_YY40(((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->hd=(void*)((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD16.tag=25,((_tmpD16.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD16)))),_tmpD17)))),((_tmpD15->tl=0,_tmpD15)))))));}
break;}case 236: _LL324: {
# 2258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2257 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpD18;yyval=Cyc_YY40(((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD18->tl=0,_tmpD18)))))));}
# 2260
break;}case 237: _LL325: {
# 2262
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2264
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2277 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD1E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD1D;struct Cyc_List_List*_tmpD1C;yyval=Cyc_YY40(((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->hd=(void*)((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E[0]=((_tmpD1D.tag=23,((_tmpD1D.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpD1D)))),_tmpD1E)))),((_tmpD1C->tl=0,_tmpD1C)))))));}
break;}case 238: _LL326: {
# 2280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2279 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD24;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD23;struct Cyc_List_List*_tmpD22;yyval=Cyc_YY40(((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->hd=(void*)((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24[0]=((_tmpD23.tag=23,((_tmpD23.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpD23)))),_tmpD24)))),((_tmpD22->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD22)))))));}
break;}case 239: _LL327: {
# 2282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD25;yyval=Cyc_YY38(((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpD25->tl=0,_tmpD25)))))));}
break;}case 240: _LL328: {
# 2288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2287 "parse.y"
{struct Cyc_List_List*_tmpD26;yyval=Cyc_YY38(((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpD26->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpD26)))))));}
break;}case 241: _LL329: {
# 2290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp6BA=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6BC;struct Cyc_Parse_Type_specifier _tmp6BD;struct Cyc_List_List*_tmp6BE;struct _tuple25 _tmp6BB=_tmp6BA;_tmp6BC=_tmp6BB.f1;_tmp6BD=_tmp6BB.f2;_tmp6BE=_tmp6BB.f3;
if(_tmp6BC.loc == 0)_tmp6BC.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp6BF=Cyc_yyget_YY27(yyyyvsp[1]);struct _tuple0*_tmp6C1;struct Cyc_List_List*_tmp6C2;struct Cyc_Parse_Declarator _tmp6C0=_tmp6BF;_tmp6C1=_tmp6C0.id;_tmp6C2=_tmp6C0.tms;{
void*_tmp6C3=Cyc_Parse_speclist2typ(_tmp6BD,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp6C4=Cyc_Parse_apply_tms(_tmp6BC,_tmp6C3,_tmp6BE,_tmp6C2);struct Cyc_Absyn_Tqual _tmp6C6;void*_tmp6C7;struct Cyc_List_List*_tmp6C8;struct Cyc_List_List*_tmp6C9;struct _tuple13 _tmp6C5=_tmp6C4;_tmp6C6=_tmp6C5.f1;_tmp6C7=_tmp6C5.f2;_tmp6C8=_tmp6C5.f3;_tmp6C9=_tmp6C5.f4;
if(_tmp6C8 != 0){
const char*_tmpD27;Cyc_Parse_err(((_tmpD27="parameter with bad type params",_tag_dyneither(_tmpD27,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6C1)){
const char*_tmpD28;Cyc_Parse_err(((_tmpD28="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpD28,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp6C1).f2;
if(_tmp6C9 != 0){
const char*_tmpD2B;void*_tmpD2A;(_tmpD2A=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD2B="extra attributes on parameter, ignoring",_tag_dyneither(_tmpD2B,sizeof(char),40))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}
{struct _tuple8*_tmpD2C;yyval=Cyc_YY37(((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->f1=idopt,((_tmpD2C->f2=_tmp6C6,((_tmpD2C->f3=_tmp6C7,_tmpD2C)))))))));}
# 2307
break;};};};}case 242: _LL32A: {
# 2309
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2308 "parse.y"
struct _tuple25 _tmp6CF=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6D1;struct Cyc_Parse_Type_specifier _tmp6D2;struct Cyc_List_List*_tmp6D3;struct _tuple25 _tmp6D0=_tmp6CF;_tmp6D1=_tmp6D0.f1;_tmp6D2=_tmp6D0.f2;_tmp6D3=_tmp6D0.f3;
if(_tmp6D1.loc == 0)_tmp6D1.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6D4=Cyc_Parse_speclist2typ(_tmp6D2,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6D3 != 0){
const char*_tmpD2F;void*_tmpD2E;(_tmpD2E=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD2F="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD2F,sizeof(char),38))),_tag_dyneither(_tmpD2E,sizeof(void*),0)));}
{struct _tuple8*_tmpD30;yyval=Cyc_YY37(((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->f1=0,((_tmpD30->f2=_tmp6D1,((_tmpD30->f3=_tmp6D4,_tmpD30)))))))));}
# 2315
break;};}case 243: _LL32B: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2316 "parse.y"
struct _tuple25 _tmp6D8=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6DA;struct Cyc_Parse_Type_specifier _tmp6DB;struct Cyc_List_List*_tmp6DC;struct _tuple25 _tmp6D9=_tmp6D8;_tmp6DA=_tmp6D9.f1;_tmp6DB=_tmp6D9.f2;_tmp6DC=_tmp6D9.f3;
if(_tmp6DA.loc == 0)_tmp6DA.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6DD=Cyc_Parse_speclist2typ(_tmp6DB,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6DE=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6DF=Cyc_Parse_apply_tms(_tmp6DA,_tmp6DD,_tmp6DC,_tmp6DE);struct Cyc_Absyn_Tqual _tmp6E1;void*_tmp6E2;struct Cyc_List_List*_tmp6E3;struct Cyc_List_List*_tmp6E4;struct _tuple13 _tmp6E0=_tmp6DF;_tmp6E1=_tmp6E0.f1;_tmp6E2=_tmp6E0.f2;_tmp6E3=_tmp6E0.f3;_tmp6E4=_tmp6E0.f4;
if(_tmp6E3 != 0){
const char*_tmpD33;void*_tmpD32;(_tmpD32=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD33="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpD33,sizeof(char),49))),_tag_dyneither(_tmpD32,sizeof(void*),0)));}
# 2324
if(_tmp6E4 != 0){
const char*_tmpD36;void*_tmpD35;(_tmpD35=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD36="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD36,sizeof(char),38))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}
{struct _tuple8*_tmpD37;yyval=Cyc_YY37(((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->f1=0,((_tmpD37->f2=_tmp6E1,((_tmpD37->f3=_tmp6E2,_tmpD37)))))))));}
# 2328
break;};}case 244: _LL32C: {
# 2330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2332 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
break;}case 245: _LL32D: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2336 "parse.y"
{struct _dyneither_ptr*_tmpD3A;struct Cyc_List_List*_tmpD39;yyval=Cyc_YY36(((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->hd=((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD3A)))),((_tmpD39->tl=0,_tmpD39)))))));}
break;}case 246: _LL32E: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2338 "parse.y"
{struct _dyneither_ptr*_tmpD3D;struct Cyc_List_List*_tmpD3C;yyval=Cyc_YY36(((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->hd=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD3D)))),((_tmpD3C->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpD3C)))))));}
break;}case 247: _LL32F: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2342 "parse.y"
yyval=yyyyvsp[0];
break;}case 248: _LL330: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2343 "parse.y"
yyval=yyyyvsp[0];
break;}case 249: _LL331: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2348 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD40;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD3F;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD40.tag=35,((_tmpD40.f1=0,((_tmpD40.f2=0,_tmpD40)))))),_tmpD3F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 250: _LL332: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2350 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD43;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD42;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42[0]=((_tmpD43.tag=35,((_tmpD43.f1=0,((_tmpD43.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD43)))))),_tmpD42)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 251: _LL333: {
# 2353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2352 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD46;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD45;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45[0]=((_tmpD46.tag=35,((_tmpD46.f1=0,((_tmpD46.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD46)))))),_tmpD45)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 252: _LL334: {
# 2355
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2354 "parse.y"
struct _dyneither_ptr*_tmpD49;struct _tuple0*_tmpD48;struct Cyc_Absyn_Vardecl*_tmp6F4=Cyc_Absyn_new_vardecl(((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->f1=Cyc_Absyn_Loc_n,((_tmpD48->f2=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD49)))),_tmpD48)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2357
(_tmp6F4->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpD4C;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD4B;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4C.tag=26,((_tmpD4C.f1=_tmp6F4,((_tmpD4C.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD4C.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]),((_tmpD4C.f4=0,_tmpD4C)))))))))),_tmpD4B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2360
break;}case 253: _LL335: {
# 2362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2361 "parse.y"
void*_tmp6F9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpD4F;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD4E;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD4F.tag=27,((_tmpD4F.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD4F.f2=_tmp6F9,((_tmpD4F.f3=0,_tmpD4F)))))))),_tmpD4E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2364
break;}case 254: _LL336: {
# 2366
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD52;struct Cyc_List_List*_tmpD51;yyval=Cyc_YY5(((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51->hd=((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52->f1=0,((_tmpD52->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]),_tmpD52)))))),((_tmpD51->tl=0,_tmpD51)))))));}
break;}case 255: _LL337: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2371 "parse.y"
{struct _tuple30*_tmpD55;struct Cyc_List_List*_tmpD54;yyval=Cyc_YY5(((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54->hd=((_tmpD55=_cycalloc(sizeof(*_tmpD55)),((_tmpD55->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpD55->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD55)))))),((_tmpD54->tl=0,_tmpD54)))))));}
break;}case 256: _LL338: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2373 "parse.y"
{struct _tuple30*_tmpD58;struct Cyc_List_List*_tmpD57;yyval=Cyc_YY5(((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57->hd=((_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58->f1=0,((_tmpD58->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpD58)))))),((_tmpD57->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD57)))))));}
break;}case 257: _LL339: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2375 "parse.y"
{struct _tuple30*_tmpD5B;struct Cyc_List_List*_tmpD5A;yyval=Cyc_YY5(((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->hd=((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpD5B->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]),_tmpD5B)))))),((_tmpD5A->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD5A)))))));}
break;}case 258: _LL33A: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2379 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
break;}case 259: _LL33B: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2380 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD65;struct _dyneither_ptr*_tmpD64;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD63;struct Cyc_List_List*_tmpD62;yyval=Cyc_YY41(((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->hd=(void*)((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD63.tag=1,((_tmpD63.f1=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD64)))),_tmpD63)))),_tmpD65)))),((_tmpD62->tl=0,_tmpD62)))))));}
break;}case 260: _LL33C: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2385 "parse.y"
{struct Cyc_List_List*_tmpD66;yyval=Cyc_YY41(((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpD66->tl=0,_tmpD66)))))));}
break;}case 261: _LL33D: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2386 "parse.y"
{struct Cyc_List_List*_tmpD67;yyval=Cyc_YY41(((_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpD67->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpD67)))))));}
break;}case 262: _LL33E: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2390 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD6A;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD69;yyval=Cyc_YY42((void*)((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69[0]=((_tmpD6A.tag=0,((_tmpD6A.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD6A)))),_tmpD69)))));}
break;}case 263: _LL33F: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2391 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD70;struct _dyneither_ptr*_tmpD6F;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD6E;yyval=Cyc_YY42((void*)((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD70.tag=1,((_tmpD70.f1=((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD6F)))),_tmpD70)))),_tmpD6E)))));}
break;}case 264: _LL340: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2396 "parse.y"
struct _tuple25 _tmp70F=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp711;struct Cyc_Parse_Type_specifier _tmp712;struct Cyc_List_List*_tmp713;struct _tuple25 _tmp710=_tmp70F;_tmp711=_tmp710.f1;_tmp712=_tmp710.f2;_tmp713=_tmp710.f3;{
void*_tmp714=Cyc_Parse_speclist2typ(_tmp712,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp713 != 0){
const char*_tmpD73;void*_tmpD72;(_tmpD72=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD73="ignoring attributes in type",_tag_dyneither(_tmpD73,sizeof(char),28))),_tag_dyneither(_tmpD72,sizeof(void*),0)));}
{struct _tuple8*_tmpD74;yyval=Cyc_YY37(((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->f1=0,((_tmpD74->f2=_tmp711,((_tmpD74->f3=_tmp714,_tmpD74)))))))));}
# 2402
break;};}case 265: _LL341: {
# 2404
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2403 "parse.y"
struct _tuple25 _tmp718=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp71A;struct Cyc_Parse_Type_specifier _tmp71B;struct Cyc_List_List*_tmp71C;struct _tuple25 _tmp719=_tmp718;_tmp71A=_tmp719.f1;_tmp71B=_tmp719.f2;_tmp71C=_tmp719.f3;{
void*_tmp71D=Cyc_Parse_speclist2typ(_tmp71B,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp71E=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp71F=Cyc_Parse_apply_tms(_tmp71A,_tmp71D,_tmp71C,_tmp71E);
if(_tmp71F.f3 != 0){
# 2409
const char*_tmpD77;void*_tmpD76;(_tmpD76=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD77="bad type params, ignoring",_tag_dyneither(_tmpD77,sizeof(char),26))),_tag_dyneither(_tmpD76,sizeof(void*),0)));}
if(_tmp71F.f4 != 0){
const char*_tmpD7A;void*_tmpD79;(_tmpD79=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD7A="bad specifiers, ignoring",_tag_dyneither(_tmpD7A,sizeof(char),25))),_tag_dyneither(_tmpD79,sizeof(void*),0)));}
{struct _tuple8*_tmpD7B;yyval=Cyc_YY37(((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B->f1=0,((_tmpD7B->f2=_tmp71F.f1,((_tmpD7B->f3=_tmp71F.f2,_tmpD7B)))))))));}
# 2414
break;};}case 266: _LL342: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2417 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 267: _LL343: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2418 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD7E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD7D;yyval=Cyc_YY44((void*)((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7E.tag=24,((_tmpD7E.f1=0,_tmpD7E)))),_tmpD7D)))));}
break;}case 268: _LL344: {
# 2421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2419 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD81;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD80;yyval=Cyc_YY44((void*)((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD81.tag=24,((_tmpD81.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpD81)))),_tmpD80)))));}
break;}case 269: _LL345: {
# 2422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2420 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD84;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD83;yyval=Cyc_YY44((void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=25,((_tmpD84.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD84)))),_tmpD83)))));}
break;}case 270: _LL346: {
# 2423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2421 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD8A;struct Cyc_List_List*_tmpD89;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD88;yyval=Cyc_YY44((void*)((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88[0]=((_tmpD8A.tag=24,((_tmpD8A.f1=((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD89->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD89)))))),_tmpD8A)))),_tmpD88)))));}
break;}case 271: _LL347: {
# 2424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD8B;yyval=Cyc_YY40(((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD8B->tl=0,_tmpD8B)))))));}
break;}case 272: _LL348: {
# 2430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2428 "parse.y"
{struct Cyc_List_List*_tmpD8C;yyval=Cyc_YY40(((_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD8C->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD8C)))))));}
break;}case 273: _LL349: {
# 2431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2433 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD8D;yyval=Cyc_YY30(((_tmpD8D.tms=Cyc_yyget_YY26(yyyyvsp[0]),_tmpD8D)));}
break;}case 274: _LL34A: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2435 "parse.y"
yyval=yyyyvsp[0];
break;}case 275: _LL34B: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2437 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD8E;yyval=Cyc_YY30(((_tmpD8E.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpD8E)));}
break;}case 276: _LL34C: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2442 "parse.y"
yyval=yyyyvsp[1];
break;}case 277: _LL34D: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2444 "parse.y"
{struct Cyc_List_List*_tmpD98;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD97;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD96;struct Cyc_Parse_Abstractdeclarator _tmpD95;yyval=Cyc_YY30(((_tmpD95.tms=((_tmpD98=_region_malloc(yyr,sizeof(*_tmpD98)),((_tmpD98->hd=(void*)((_tmpD96=_region_malloc(yyr,sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=0,((_tmpD97.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD97.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD97)))))),_tmpD96)))),((_tmpD98->tl=0,_tmpD98)))))),_tmpD95)));}
break;}case 278: _LL34E: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2446 "parse.y"
{struct Cyc_List_List*_tmpDA2;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpDA1;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpDA0;struct Cyc_Parse_Abstractdeclarator _tmpD9F;yyval=Cyc_YY30(((_tmpD9F.tms=((_tmpDA2=_region_malloc(yyr,sizeof(*_tmpDA2)),((_tmpDA2->hd=(void*)((_tmpDA0=_region_malloc(yyr,sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpDA1.tag=0,((_tmpDA1.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpDA1.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpDA1)))))),_tmpDA0)))),((_tmpDA2->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDA2)))))),_tmpD9F)));}
break;}case 279: _LL34F: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2448 "parse.y"
{struct Cyc_List_List*_tmpDAC;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpDAB;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpDAA;struct Cyc_Parse_Abstractdeclarator _tmpDA9;yyval=Cyc_YY30(((_tmpDA9.tms=((_tmpDAC=_region_malloc(yyr,sizeof(*_tmpDAC)),((_tmpDAC->hd=(void*)((_tmpDAA=_region_malloc(yyr,sizeof(*_tmpDAA)),((_tmpDAA[0]=((_tmpDAB.tag=1,((_tmpDAB.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpDAB.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpDAB.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpDAB)))))))),_tmpDAA)))),((_tmpDAC->tl=0,_tmpDAC)))))),_tmpDA9)));}
break;}case 280: _LL350: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2450 "parse.y"
{struct Cyc_List_List*_tmpDB6;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpDB5;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpDB4;struct Cyc_Parse_Abstractdeclarator _tmpDB3;yyval=Cyc_YY30(((_tmpDB3.tms=((_tmpDB6=_region_malloc(yyr,sizeof(*_tmpDB6)),((_tmpDB6->hd=(void*)((_tmpDB4=_region_malloc(yyr,sizeof(*_tmpDB4)),((_tmpDB4[0]=((_tmpDB5.tag=1,((_tmpDB5.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpDB5.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpDB5.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpDB5)))))))),_tmpDB4)))),((_tmpDB6->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDB6)))))),_tmpDB3)));}
# 2453
break;}case 281: _LL351: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2454 "parse.y"
{struct Cyc_List_List*_tmpDCB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDCA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDC9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDC8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDC7;struct Cyc_Parse_Abstractdeclarator _tmpDC6;yyval=Cyc_YY30(((_tmpDC6.tms=((_tmpDCB=_region_malloc(yyr,sizeof(*_tmpDCB)),((_tmpDCB->hd=(void*)((_tmpDC7=_region_malloc(yyr,sizeof(*_tmpDC7)),((_tmpDC7[0]=((_tmpDCA.tag=3,((_tmpDCA.f1=(void*)((_tmpDC8=_region_malloc(yyr,sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDC9.tag=1,((_tmpDC9.f1=0,((_tmpDC9.f2=0,((_tmpDC9.f3=0,((_tmpDC9.f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpDC9.f5=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpDC9.f6=Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDC9.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDC9)))))))))))))))),_tmpDC8)))),_tmpDCA)))),_tmpDC7)))),((_tmpDCB->tl=0,_tmpDCB)))))),_tmpDC6)));}
# 2456
break;}case 282: _LL352: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2457 "parse.y"
struct _tuple26*_tmp748=Cyc_yyget_YY39(yyyyvsp[1]);struct Cyc_List_List*_tmp74A;int _tmp74B;struct Cyc_Absyn_VarargInfo*_tmp74C;void*_tmp74D;struct Cyc_List_List*_tmp74E;struct _tuple26*_tmp749=_tmp748;_tmp74A=_tmp749->f1;_tmp74B=_tmp749->f2;_tmp74C=_tmp749->f3;_tmp74D=_tmp749->f4;_tmp74E=_tmp749->f5;
{struct Cyc_List_List*_tmpDE0;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDDF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDDE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDDD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDDC;struct Cyc_Parse_Abstractdeclarator _tmpDDB;yyval=Cyc_YY30(((_tmpDDB.tms=((_tmpDE0=_region_malloc(yyr,sizeof(*_tmpDE0)),((_tmpDE0->hd=(void*)((_tmpDDC=_region_malloc(yyr,sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDF.tag=3,((_tmpDDF.f1=(void*)((_tmpDDD=_region_malloc(yyr,sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDDE.tag=1,((_tmpDDE.f1=_tmp74A,((_tmpDDE.f2=_tmp74B,((_tmpDDE.f3=_tmp74C,((_tmpDDE.f4=_tmp74D,((_tmpDDE.f5=_tmp74E,((_tmpDDE.f6=Cyc_yyget_YY55(yyyyvsp[3]),((_tmpDDE.f7=Cyc_yyget_YY55(yyyyvsp[4]),_tmpDDE)))))))))))))))),_tmpDDD)))),_tmpDDF)))),_tmpDDC)))),((_tmpDE0->tl=0,_tmpDE0)))))),_tmpDDB)));}
# 2460
break;}case 283: _LL353: {
# 2462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2461 "parse.y"
{struct Cyc_List_List*_tmpDF5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDF4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDF3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDF2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDF1;struct Cyc_Parse_Abstractdeclarator _tmpDF0;yyval=Cyc_YY30(((_tmpDF0.tms=((_tmpDF5=_region_malloc(yyr,sizeof(*_tmpDF5)),((_tmpDF5->hd=(void*)((_tmpDF1=_region_malloc(yyr,sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF4.tag=3,((_tmpDF4.f1=(void*)((_tmpDF2=_region_malloc(yyr,sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF3.tag=1,((_tmpDF3.f1=0,((_tmpDF3.f2=0,((_tmpDF3.f3=0,((_tmpDF3.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpDF3.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpDF3.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpDF3.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpDF3)))))))))))))))),_tmpDF2)))),_tmpDF4)))),_tmpDF1)))),((_tmpDF5->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDF5)))))),_tmpDF0)));}
# 2464
break;}case 284: _LL354: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2465 "parse.y"
struct _tuple26*_tmp75B=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp75D;int _tmp75E;struct Cyc_Absyn_VarargInfo*_tmp75F;void*_tmp760;struct Cyc_List_List*_tmp761;struct _tuple26*_tmp75C=_tmp75B;_tmp75D=_tmp75C->f1;_tmp75E=_tmp75C->f2;_tmp75F=_tmp75C->f3;_tmp760=_tmp75C->f4;_tmp761=_tmp75C->f5;
{struct Cyc_List_List*_tmpE0A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpE09;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpE08;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpE07;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE06;struct Cyc_Parse_Abstractdeclarator _tmpE05;yyval=Cyc_YY30(((_tmpE05.tms=((_tmpE0A=_region_malloc(yyr,sizeof(*_tmpE0A)),((_tmpE0A->hd=(void*)((_tmpE06=_region_malloc(yyr,sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE09.tag=3,((_tmpE09.f1=(void*)((_tmpE07=_region_malloc(yyr,sizeof(*_tmpE07)),((_tmpE07[0]=((_tmpE08.tag=1,((_tmpE08.f1=_tmp75D,((_tmpE08.f2=_tmp75E,((_tmpE08.f3=_tmp75F,((_tmpE08.f4=_tmp760,((_tmpE08.f5=_tmp761,((_tmpE08.f6=
Cyc_yyget_YY55(yyyyvsp[4]),((_tmpE08.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpE08)))))))))))))))),_tmpE07)))),_tmpE09)))),_tmpE06)))),((_tmpE0A->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpE0A)))))),_tmpE05)));}
# 2469
break;}case 285: _LL355: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2471 "parse.y"
struct Cyc_List_List*_tmp768=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpE14;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpE13;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpE12;struct Cyc_Parse_Abstractdeclarator _tmpE11;yyval=Cyc_YY30(((_tmpE11.tms=((_tmpE14=_region_malloc(yyr,sizeof(*_tmpE14)),((_tmpE14->hd=(void*)((_tmpE12=_region_malloc(yyr,sizeof(*_tmpE12)),((_tmpE12[0]=((_tmpE13.tag=4,((_tmpE13.f1=_tmp768,((_tmpE13.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpE13.f3=0,_tmpE13)))))))),_tmpE12)))),((_tmpE14->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpE14)))))),_tmpE11)));}
# 2475
break;}case 286: _LL356: {
# 2477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2476 "parse.y"
{struct Cyc_List_List*_tmpE1E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpE1D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpE1C;struct Cyc_Parse_Abstractdeclarator _tmpE1B;yyval=Cyc_YY30(((_tmpE1B.tms=((_tmpE1E=_region_malloc(yyr,sizeof(*_tmpE1E)),((_tmpE1E->hd=(void*)((_tmpE1C=_region_malloc(yyr,sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1D.tag=5,((_tmpE1D.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpE1D.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpE1D)))))),_tmpE1C)))),((_tmpE1E->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpE1E)))))),_tmpE1B)));}
# 2478
break;}case 287: _LL357: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2482 "parse.y"
yyval=yyyyvsp[0];
break;}case 288: _LL358: {
# 2485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2483 "parse.y"
yyval=yyyyvsp[0];
break;}case 289: _LL359: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2484 "parse.y"
yyval=yyyyvsp[0];
break;}case 290: _LL35A: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2485 "parse.y"
yyval=yyyyvsp[0];
break;}case 291: _LL35B: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2486 "parse.y"
yyval=yyyyvsp[0];
break;}case 292: _LL35C: {
# 2489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2487 "parse.y"
yyval=yyyyvsp[0];
break;}case 293: _LL35D: {
# 2490
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2492
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2490 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpE21;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpE20;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20[0]=((_tmpE21.tag=16,((_tmpE21.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpE21)))),_tmpE20)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 294: _LL35E: {
# 2493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE27;struct _dyneither_ptr*_tmpE26;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE25;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE27.tag=13,((_tmpE27.f1=((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE26)))),((_tmpE27.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]),_tmpE27)))))),_tmpE25)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 295: _LL35F: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2500 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 296: _LL360: {
# 2503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2501 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 297: _LL361: {
# 2504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2506 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 298: _LL362: {
# 2509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2507 "parse.y"
yyval=yyyyvsp[1];
break;}case 299: _LL363: {
# 2510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2512 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 300: _LL364: {
# 2515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2513 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
break;}case 301: _LL365: {
# 2516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2514 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE2D;struct _dyneither_ptr*_tmpE2C;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE2B;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B[0]=((_tmpE2D.tag=13,((_tmpE2D.f1=((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE2C)))),((_tmpE2D.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpE2D)))))),_tmpE2B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2516
break;}case 302: _LL366: {
# 2518
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2516 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE33;struct _dyneither_ptr*_tmpE32;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE31;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE33.tag=13,((_tmpE33.f1=((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE32)))),((_tmpE33.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3])),_tmpE33)))))),_tmpE31)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2518
break;}case 303: _LL367: {
# 2520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2518 "parse.y"
yyval=yyyyvsp[0];
break;}case 304: _LL368: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2519 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL369: {
# 2522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2520 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE36;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE35;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((_tmpE36.tag=1,((_tmpE36.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE36)))),_tmpE35)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 306: _LL36A: {
# 2524
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2523 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE39;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE38;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=((_tmpE39.tag=1,((_tmpE39.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE39)))),_tmpE38)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));}
break;}case 307: _LL36B: {
# 2526
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2528
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2528 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 308: _LL36C: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2530 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 309: _LL36D: {
# 2533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 310: _LL36E: {
# 2539
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2539 "parse.y"
struct Cyc_Absyn_Exp*_tmp780=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp780,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 311: _LL36F: {
# 2543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2543 "parse.y"
struct Cyc_Absyn_Exp*_tmp781=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp781,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2546
break;}case 312: _LL370: {
# 2548
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2549 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 313: _LL371:
# 2552
 yyval=Cyc_YY8(0);
break;case 314: _LL372: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2567 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE3C;struct Cyc_List_List*_tmpE3B;yyval=Cyc_YY8(((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->hd=((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpE3C->pat_vars=0,((_tmpE3C->where_clause=0,((_tmpE3C->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]),((_tmpE3C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE3C)))))))))))),((_tmpE3B->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpE3B)))))));}
break;}case 315: _LL373: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2571 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE3F;struct Cyc_List_List*_tmpE3E;yyval=Cyc_YY8(((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->hd=((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE3F->pat_vars=0,((_tmpE3F->where_clause=0,((_tmpE3F->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpE3F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE3F)))))))))))),((_tmpE3E->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpE3E)))))));}
break;}case 316: _LL374: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2575 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE42;struct Cyc_List_List*_tmpE41;yyval=Cyc_YY8(((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->hd=((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE42->pat_vars=0,((_tmpE42->where_clause=0,((_tmpE42->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]),((_tmpE42->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE42)))))))))))),((_tmpE41->tl=Cyc_yyget_YY8(yyyyvsp[4]),_tmpE41)))))));}
break;}case 317: _LL375: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2577 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE45;struct Cyc_List_List*_tmpE44;yyval=Cyc_YY8(((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44->hd=((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE45->pat_vars=0,((_tmpE45->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE45->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpE45->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE45)))))))))))),((_tmpE44->tl=
Cyc_yyget_YY8(yyyyvsp[5]),_tmpE44)))))));}
break;}case 318: _LL376: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2581 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE48;struct Cyc_List_List*_tmpE47;yyval=Cyc_YY8(((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47->hd=((_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE48->pat_vars=0,((_tmpE48->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE48->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]),((_tmpE48->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE48)))))))))))),((_tmpE47->tl=Cyc_yyget_YY8(yyyyvsp[6]),_tmpE47)))))));}
break;}case 319: _LL377: {
# 2584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2588 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL378: {
# 2591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2592 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL379: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2596 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL37A: {
# 2600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2599 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL37B: {
# 2603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2602 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL37C: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2605 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL37D: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2608 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL37E: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2611 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL37F: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2614 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 328: _LL380: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2617 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 329: _LL381: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2620 "parse.y"
struct Cyc_List_List*_tmp78C=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp78D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp78C,_tmp78D));
# 2625
break;}case 330: _LL382: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2626 "parse.y"
struct Cyc_List_List*_tmp78E=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp78F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp78E,_tmp78F));
# 2631
break;}case 331: _LL383: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2632 "parse.y"
struct Cyc_List_List*_tmp790=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp791=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp790,_tmp791));
# 2637
break;}case 332: _LL384: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2638 "parse.y"
struct Cyc_List_List*_tmp792=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp793=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp792,_tmp793));
# 2643
break;}case 333: _LL385: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpE49;yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE49)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 334: _LL386: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2649 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL387: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2650 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL388: {
# 2653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2651 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL389: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2652 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 338: _LL38A: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2654 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 339: _LL38B: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2655 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 340: _LL38C: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2657 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 341: _LL38D: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2666 "parse.y"
yyval=yyyyvsp[0];
break;}case 342: _LL38E: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2669 "parse.y"
yyval=yyyyvsp[0];
break;}case 343: _LL38F: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2671 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL390: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2674 "parse.y"
yyval=yyyyvsp[0];
break;}case 345: _LL391: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2676 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL392: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2679 "parse.y"
yyval=yyyyvsp[0];
break;}case 347: _LL393: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2681 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL394: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2684 "parse.y"
yyval=yyyyvsp[0];
break;}case 349: _LL395: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2686 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL396: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2689 "parse.y"
yyval=yyyyvsp[0];
break;}case 351: _LL397: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2691 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL398: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2694 "parse.y"
yyval=yyyyvsp[0];
break;}case 353: _LL399: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2696 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL39A: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2699 "parse.y"
yyval=yyyyvsp[0];
break;}case 355: _LL39B: {
# 2702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2701 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL39C: {
# 2704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2703 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL39D: {
# 2706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2706 "parse.y"
yyval=yyyyvsp[0];
break;}case 358: _LL39E: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2708 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL39F: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2710 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL3A0: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2712 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL3A1: {
# 2715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2714 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL3A2: {
# 2717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2717 "parse.y"
yyval=yyyyvsp[0];
break;}case 363: _LL3A3: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2719 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL3A4: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2721 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 365: _LL3A5: {
# 2724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2724 "parse.y"
yyval=yyyyvsp[0];
break;}case 366: _LL3A6: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2726 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL3A7: {
# 2729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2728 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL3A8: {
# 2731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2731 "parse.y"
yyval=yyyyvsp[0];
break;}case 369: _LL3A9: {
# 2734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2733 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 370: _LL3AA: {
# 2736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2735 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 371: _LL3AB: {
# 2738
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2740
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2737 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 372: _LL3AC: {
# 2740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2740 "parse.y"
yyval=yyyyvsp[0];
break;}case 373: _LL3AD: {
# 2743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2742 "parse.y"
void*_tmp795=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp795,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2745
break;}case 374: _LL3AE: {
# 2747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2748 "parse.y"
yyval=yyyyvsp[0];
break;}case 375: _LL3AF: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2751 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 376: _LL3B0: {
# 2754
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2753 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 377: _LL3B1: {
# 2756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2755 "parse.y"
void*_tmp796=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp796,(unsigned int)(yyyylsp[0]).first_line)));
# 2758
break;}case 378: _LL3B2: {
# 2760
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2759 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 379: _LL3B3: {
# 2762
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2761 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 380: _LL3B4: {
# 2764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2766 "parse.y"
yyval=yyyyvsp[0];
break;}case 381: _LL3B5: {
# 2769
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2775 "parse.y"
yyval=yyyyvsp[0];
break;}case 382: _LL3B6: {
# 2778
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2780 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 383: _LL3B7: {
# 2783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2782 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 384: _LL3B8: {
# 2785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2784 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp797=e->r;void*_tmp798=_tmp797;enum Cyc_Absyn_Sign _tmp79A;char _tmp79B;enum Cyc_Absyn_Sign _tmp79D;short _tmp79E;enum Cyc_Absyn_Sign _tmp7A0;int _tmp7A1;struct _dyneither_ptr _tmp7A3;int _tmp7A4;_LL3BB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp799=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp799->tag != 0)goto _LL3BD;else{if(((_tmp799->f1).Char_c).tag != 2)goto _LL3BD;_tmp79A=((struct _tuple3)((_tmp799->f1).Char_c).val).f1;_tmp79B=((struct _tuple3)((_tmp799->f1).Char_c).val).f2;}}_LL3BC:
# 2788
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE4C;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpE4B;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE4B=_cycalloc_atomic(sizeof(*_tmpE4B)),((_tmpE4B[0]=((_tmpE4C.tag=11,((_tmpE4C.f1=_tmp79B,_tmpE4C)))),_tmpE4B)))),e->loc));}goto _LL3BA;_LL3BD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp79C->tag != 0)goto _LL3BF;else{if(((_tmp79C->f1).Short_c).tag != 4)goto _LL3BF;_tmp79D=((struct _tuple4)((_tmp79C->f1).Short_c).val).f1;_tmp79E=((struct _tuple4)((_tmp79C->f1).Short_c).val).f2;}}_LL3BE:
# 2790
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE4F;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE4E;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE4E=_cycalloc_atomic(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE4F.tag=10,((_tmpE4F.f1=_tmp79D,((_tmpE4F.f2=(int)_tmp79E,_tmpE4F)))))),_tmpE4E)))),e->loc));}goto _LL3BA;_LL3BF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp79F->tag != 0)goto _LL3C1;else{if(((_tmp79F->f1).Int_c).tag != 5)goto _LL3C1;_tmp7A0=((struct _tuple5)((_tmp79F->f1).Int_c).val).f1;_tmp7A1=((struct _tuple5)((_tmp79F->f1).Int_c).val).f2;}}_LL3C0:
# 2792
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE52;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE51;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE51=_cycalloc_atomic(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE52.tag=10,((_tmpE52.f1=_tmp7A0,((_tmpE52.f2=_tmp7A1,_tmpE52)))))),_tmpE51)))),e->loc));}goto _LL3BA;_LL3C1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp7A2->tag != 0)goto _LL3C3;else{if(((_tmp7A2->f1).Float_c).tag != 7)goto _LL3C3;_tmp7A3=((struct _tuple7)((_tmp7A2->f1).Float_c).val).f1;_tmp7A4=((struct _tuple7)((_tmp7A2->f1).Float_c).val).f2;}}_LL3C2:
# 2794
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE55;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpE54;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE55.tag=12,((_tmpE55.f1=_tmp7A3,((_tmpE55.f2=_tmp7A4,_tmpE55)))))),_tmpE54)))),e->loc));}goto _LL3BA;_LL3C3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp7A5->tag != 0)goto _LL3C5;else{if(((_tmp7A5->f1).Null_c).tag != 1)goto _LL3C5;}}_LL3C4:
# 2796
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3BA;_LL3C5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp7A6->tag != 0)goto _LL3C7;else{if(((_tmp7A6->f1).String_c).tag != 8)goto _LL3C7;}}_LL3C6:
# 2798
{const char*_tmpE56;Cyc_Parse_err(((_tmpE56="strings cannot occur within patterns",_tag_dyneither(_tmpE56,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3BA;_LL3C7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp7A7->tag != 0)goto _LL3C9;else{if(((_tmp7A7->f1).Wstring_c).tag != 9)goto _LL3C9;}}_LL3C8:
# 2800
{const char*_tmpE57;Cyc_Parse_err(((_tmpE57="strings cannot occur within patterns",_tag_dyneither(_tmpE57,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3BA;_LL3C9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp798;if(_tmp7A8->tag != 0)goto _LL3CB;else{if(((_tmp7A8->f1).LongLong_c).tag != 6)goto _LL3CB;}}_LL3CA:
# 2802
{const char*_tmpE58;Cyc_Parse_unimp(((_tmpE58="long long's in patterns",_tag_dyneither(_tmpE58,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3BA;_LL3CB:;_LL3CC: {
# 2804
const char*_tmpE59;Cyc_Parse_err(((_tmpE59="bad constant in case",_tag_dyneither(_tmpE59,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3BA:;}
# 2807
break;}case 385: _LL3B9: {
# 2809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2808 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE5C;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE5B;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[0]=((_tmpE5C.tag=15,((_tmpE5C.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE5C)))),_tmpE5B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 386: _LL3CD: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2810 "parse.y"
{const char*_tmpE5D;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE5D="as",_tag_dyneither(_tmpE5D,sizeof(char),3))))!= 0){
const char*_tmpE5E;Cyc_Parse_err(((_tmpE5E="expecting `as'",_tag_dyneither(_tmpE5E,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE68;struct _dyneither_ptr*_tmpE67;struct _tuple0*_tmpE66;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE65;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=((_tmpE68.tag=1,((_tmpE68.f1=Cyc_Absyn_new_vardecl(((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->f1=Cyc_Absyn_Loc_n,((_tmpE66->f2=((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE67)))),_tmpE66)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE68.f2=
Cyc_yyget_YY9(yyyyvsp[2]),_tmpE68)))))),_tmpE65)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2815
break;}case 387: _LL3CE: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2816 "parse.y"
{const char*_tmpE69;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE69="alias",_tag_dyneither(_tmpE69,sizeof(char),6))))!= 0){
const char*_tmpE6A;Cyc_Parse_err(((_tmpE6A="expecting `alias'",_tag_dyneither(_tmpE6A,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp7BF=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE72;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE71;struct _dyneither_ptr*_tmpE70;struct Cyc_Absyn_Tvar*_tmpE6F;struct Cyc_Absyn_Tvar*tv=(_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F->name=((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE70)))),((_tmpE6F->identity=- 1,((_tmpE6F->kind=(void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE71.tag=0,((_tmpE71.f1=& Cyc_Tcutil_rk,_tmpE71)))),_tmpE72)))),_tmpE6F)))))));
struct _dyneither_ptr*_tmpE75;struct _tuple0*_tmpE74;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->f1=Cyc_Absyn_Loc_n,((_tmpE74->f2=((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE75)))),_tmpE74)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE78;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE77;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77[0]=((_tmpE78.tag=2,((_tmpE78.f1=tv,((_tmpE78.f2=vd,_tmpE78)))))),_tmpE77)))),(unsigned int)_tmp7BF));}
# 2824
break;};}case 388: _LL3CF: {
# 2826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2825 "parse.y"
struct _tuple22 _tmp7C8=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7CA;int _tmp7CB;struct _tuple22 _tmp7C9=_tmp7C8;_tmp7CA=_tmp7C9.f1;_tmp7CB=_tmp7C9.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE7B;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE7A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=((_tmpE7B.tag=5,((_tmpE7B.f1=_tmp7CA,((_tmpE7B.f2=_tmp7CB,_tmpE7B)))))),_tmpE7A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2828
break;}case 389: _LL3D0: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2829 "parse.y"
struct _tuple22 _tmp7CE=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7D0;int _tmp7D1;struct _tuple22 _tmp7CF=_tmp7CE;_tmp7D0=_tmp7CF.f1;_tmp7D1=_tmp7CF.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE7E;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE7D;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D[0]=((_tmpE7E.tag=16,((_tmpE7E.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE7E.f2=_tmp7D0,((_tmpE7E.f3=_tmp7D1,_tmpE7E)))))))),_tmpE7D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2832
break;}case 390: _LL3D1: {
# 2834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2833 "parse.y"
struct _tuple22 _tmp7D4=*Cyc_yyget_YY14(yyyyvsp[3]);struct Cyc_List_List*_tmp7D6;int _tmp7D7;struct _tuple22 _tmp7D5=_tmp7D4;_tmp7D6=_tmp7D5.f1;_tmp7D7=_tmp7D5.f2;{
struct Cyc_List_List*_tmp7D8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE84;struct Cyc_Absyn_AggrInfo*_tmpE83;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE82;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=((_tmpE84.tag=7,((_tmpE84.f1=((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE83->targs=0,_tmpE83)))))),((_tmpE84.f2=_tmp7D8,((_tmpE84.f3=_tmp7D6,((_tmpE84.f4=_tmp7D7,_tmpE84)))))))))),_tmpE82)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2838
break;};}case 391: _LL3D2: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2839 "parse.y"
struct _tuple22 _tmp7DC=*Cyc_yyget_YY14(yyyyvsp[2]);struct Cyc_List_List*_tmp7DE;int _tmp7DF;struct _tuple22 _tmp7DD=_tmp7DC;_tmp7DE=_tmp7DD.f1;_tmp7DF=_tmp7DD.f2;{
struct Cyc_List_List*_tmp7E0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE87;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE86;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE86=_cycalloc(sizeof(*_tmpE86)),((_tmpE86[0]=((_tmpE87.tag=7,((_tmpE87.f1=0,((_tmpE87.f2=_tmp7E0,((_tmpE87.f3=_tmp7DE,((_tmpE87.f4=_tmp7DF,_tmpE87)))))))))),_tmpE86)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2843
break;};}case 392: _LL3D3: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2844 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE8A;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE89;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((_tmpE89[0]=((_tmpE8A.tag=6,((_tmpE8A.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE8A)))),_tmpE89)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 393: _LL3D4: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2846 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE94;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE93;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE92;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE91;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91[0]=((_tmpE94.tag=6,((_tmpE94.f1=Cyc_Absyn_new_pat((void*)((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92[0]=((_tmpE93.tag=6,((_tmpE93.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE93)))),_tmpE92)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE94)))),_tmpE91)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 394: _LL3D5: {
# 2849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2848 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE9E;struct _dyneither_ptr*_tmpE9D;struct _tuple0*_tmpE9C;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE9B;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9E.tag=3,((_tmpE9E.f1=Cyc_Absyn_new_vardecl(((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C->f1=Cyc_Absyn_Loc_n,((_tmpE9C->f2=((_tmpE9D=_cycalloc(sizeof(*_tmpE9D)),((_tmpE9D[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE9D)))),_tmpE9C)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE9E.f2=
# 2850
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE9E)))))),_tmpE9B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2852
break;}case 395: _LL3D6: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2853 "parse.y"
{const char*_tmpE9F;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE9F="as",_tag_dyneither(_tmpE9F,sizeof(char),3))))!= 0){
const char*_tmpEA0;Cyc_Parse_err(((_tmpEA0="expecting `as'",_tag_dyneither(_tmpEA0,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpEAA;struct _dyneither_ptr*_tmpEA9;struct _tuple0*_tmpEA8;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpEA7;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7[0]=((_tmpEAA.tag=3,((_tmpEAA.f1=Cyc_Absyn_new_vardecl(((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->f1=Cyc_Absyn_Loc_n,((_tmpEA8->f2=((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpEA9)))),_tmpEA8)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpEAA.f2=
# 2857
Cyc_yyget_YY9(yyyyvsp[3]),_tmpEAA)))))),_tmpEA7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2859
break;}case 396: _LL3D7: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2860 "parse.y"
void*_tmp7F3=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEBB;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEBA;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEB9;struct _dyneither_ptr*_tmpEB8;struct _tuple0*_tmpEB7;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpEB6;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpEB6=_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6[0]=((_tmpEBB.tag=4,((_tmpEBB.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7F3),((_tmpEBB.f2=
Cyc_Absyn_new_vardecl(((_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7->f1=Cyc_Absyn_Loc_n,((_tmpEB7->f2=((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEB8)))),_tmpEB7)))))),(void*)(
(_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9[0]=((_tmpEBA.tag=19,((_tmpEBA.f1=_tmp7F3,_tmpEBA)))),_tmpEB9)))),0),_tmpEBB)))))),_tmpEB6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2865
break;}case 397: _LL3D8: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2866 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7FA=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpED7;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpED6;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpED5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpED4;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpED3;struct _dyneither_ptr*_tmpED2;struct _tuple0*_tmpED1;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpED0;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0[0]=((_tmpED7.tag=4,((_tmpED7.f1=_tmp7FA,((_tmpED7.f2=
Cyc_Absyn_new_vardecl(((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1->f1=Cyc_Absyn_Loc_n,((_tmpED1->f2=((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpED2)))),_tmpED1)))))),(void*)(
(_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3[0]=((_tmpED6.tag=19,((_tmpED6.f1=(void*)((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4[0]=((_tmpED5.tag=2,((_tmpED5.f1=_tmp7FA,_tmpED5)))),_tmpED4)))),_tmpED6)))),_tmpED3)))),0),_tmpED7)))))),_tmpED0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2871
break;}case 398: _LL3D9: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2874 "parse.y"
{struct _tuple22*_tmpED8;yyval=Cyc_YY10(((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpED8->f2=0,_tmpED8)))))));}
break;}case 399: _LL3DA: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2875 "parse.y"
{struct _tuple22*_tmpED9;yyval=Cyc_YY10(((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpED9->f2=1,_tmpED9)))))));}
break;}case 400: _LL3DB: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2876 "parse.y"
{struct _tuple22*_tmpEDA;yyval=Cyc_YY10(((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA->f1=0,((_tmpEDA->f2=1,_tmpEDA)))))));}
break;}case 401: _LL3DC: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2881 "parse.y"
{struct Cyc_List_List*_tmpEDB;yyval=Cyc_YY11(((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB->hd=Cyc_yyget_YY9(yyyyvsp[0]),((_tmpEDB->tl=0,_tmpEDB)))))));}
break;}case 402: _LL3DD: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2883 "parse.y"
{struct Cyc_List_List*_tmpEDC;yyval=Cyc_YY11(((_tmpEDC=_cycalloc(sizeof(*_tmpEDC)),((_tmpEDC->hd=Cyc_yyget_YY9(yyyyvsp[2]),((_tmpEDC->tl=Cyc_yyget_YY11(yyyyvsp[0]),_tmpEDC)))))));}
break;}case 403: _LL3DE: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2888 "parse.y"
{struct _tuple23*_tmpEDD;yyval=Cyc_YY12(((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD->f1=0,((_tmpEDD->f2=Cyc_yyget_YY9(yyyyvsp[0]),_tmpEDD)))))));}
break;}case 404: _LL3DF: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2890 "parse.y"
{struct _tuple23*_tmpEDE;yyval=Cyc_YY12(((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpEDE->f2=Cyc_yyget_YY9(yyyyvsp[1]),_tmpEDE)))))));}
break;}case 405: _LL3E0: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2893 "parse.y"
{struct _tuple22*_tmpEDF;yyval=Cyc_YY14(((_tmpEDF=_cycalloc(sizeof(*_tmpEDF)),((_tmpEDF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEDF->f2=0,_tmpEDF)))))));}
break;}case 406: _LL3E1: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2894 "parse.y"
{struct _tuple22*_tmpEE0;yyval=Cyc_YY14(((_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEE0->f2=1,_tmpEE0)))))));}
break;}case 407: _LL3E2: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2895 "parse.y"
{struct _tuple22*_tmpEE1;yyval=Cyc_YY14(((_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1->f1=0,((_tmpEE1->f2=1,_tmpEE1)))))));}
break;}case 408: _LL3E3: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2900 "parse.y"
{struct Cyc_List_List*_tmpEE2;yyval=Cyc_YY13(((_tmpEE2=_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpEE2->tl=0,_tmpEE2)))))));}
break;}case 409: _LL3E4: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2902 "parse.y"
{struct Cyc_List_List*_tmpEE3;yyval=Cyc_YY13(((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpEE3->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpEE3)))))));}
break;}case 410: _LL3E5: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 411: _LL3E6: {
# 2911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2910 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 412: _LL3E7: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2915 "parse.y"
yyval=yyyyvsp[0];
break;}case 413: _LL3E8: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2917 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 414: _LL3E9: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2919 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 415: _LL3EA: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2923 "parse.y"
yyval=Cyc_YY7(0);
break;}case 416: _LL3EB: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2924 "parse.y"
{struct Cyc_Core_Opt*_tmpEE4;yyval=Cyc_YY7(((_tmpEE4=_cycalloc_atomic(sizeof(*_tmpEE4)),((_tmpEE4->v=(void*)Cyc_Absyn_Times,_tmpEE4)))));}
break;}case 417: _LL3EC: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2925 "parse.y"
{struct Cyc_Core_Opt*_tmpEE5;yyval=Cyc_YY7(((_tmpEE5=_cycalloc_atomic(sizeof(*_tmpEE5)),((_tmpEE5->v=(void*)Cyc_Absyn_Div,_tmpEE5)))));}
break;}case 418: _LL3ED: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2926 "parse.y"
{struct Cyc_Core_Opt*_tmpEE6;yyval=Cyc_YY7(((_tmpEE6=_cycalloc_atomic(sizeof(*_tmpEE6)),((_tmpEE6->v=(void*)Cyc_Absyn_Mod,_tmpEE6)))));}
break;}case 419: _LL3EE: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2927 "parse.y"
{struct Cyc_Core_Opt*_tmpEE7;yyval=Cyc_YY7(((_tmpEE7=_cycalloc_atomic(sizeof(*_tmpEE7)),((_tmpEE7->v=(void*)Cyc_Absyn_Plus,_tmpEE7)))));}
break;}case 420: _LL3EF: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2928 "parse.y"
{struct Cyc_Core_Opt*_tmpEE8;yyval=Cyc_YY7(((_tmpEE8=_cycalloc_atomic(sizeof(*_tmpEE8)),((_tmpEE8->v=(void*)Cyc_Absyn_Minus,_tmpEE8)))));}
break;}case 421: _LL3F0: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2929 "parse.y"
{struct Cyc_Core_Opt*_tmpEE9;yyval=Cyc_YY7(((_tmpEE9=_cycalloc_atomic(sizeof(*_tmpEE9)),((_tmpEE9->v=(void*)Cyc_Absyn_Bitlshift,_tmpEE9)))));}
break;}case 422: _LL3F1: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2930 "parse.y"
{struct Cyc_Core_Opt*_tmpEEA;yyval=Cyc_YY7(((_tmpEEA=_cycalloc_atomic(sizeof(*_tmpEEA)),((_tmpEEA->v=(void*)Cyc_Absyn_Bitlrshift,_tmpEEA)))));}
break;}case 423: _LL3F2: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2931 "parse.y"
{struct Cyc_Core_Opt*_tmpEEB;yyval=Cyc_YY7(((_tmpEEB=_cycalloc_atomic(sizeof(*_tmpEEB)),((_tmpEEB->v=(void*)Cyc_Absyn_Bitand,_tmpEEB)))));}
break;}case 424: _LL3F3: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2932 "parse.y"
{struct Cyc_Core_Opt*_tmpEEC;yyval=Cyc_YY7(((_tmpEEC=_cycalloc_atomic(sizeof(*_tmpEEC)),((_tmpEEC->v=(void*)Cyc_Absyn_Bitxor,_tmpEEC)))));}
break;}case 425: _LL3F4: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2933 "parse.y"
{struct Cyc_Core_Opt*_tmpEED;yyval=Cyc_YY7(((_tmpEED=_cycalloc_atomic(sizeof(*_tmpEED)),((_tmpEED->v=(void*)Cyc_Absyn_Bitor,_tmpEED)))));}
break;}case 426: _LL3F5: {
# 2936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2938 "parse.y"
yyval=yyyyvsp[0];
break;}case 427: _LL3F6: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2940 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3F7: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2943 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3F8: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2946 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3F9: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2948 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3FA: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2950 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 432: _LL3FB: {
# 2953
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2952 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3FC: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2956 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3FD: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2960 "parse.y"
yyval=yyyyvsp[0];
break;}case 435: _LL3FE: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2962 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3FF: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2966 "parse.y"
yyval=yyyyvsp[0];
break;}case 437: _LL400: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2968 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 438: _LL401: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2972 "parse.y"
yyval=yyyyvsp[0];
break;}case 439: _LL402: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2974 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL403: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2978 "parse.y"
yyval=yyyyvsp[0];
break;}case 441: _LL404: {
# 2981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2980 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL405: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2984 "parse.y"
yyval=yyyyvsp[0];
break;}case 443: _LL406: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2986 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL407: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2990 "parse.y"
yyval=yyyyvsp[0];
break;}case 445: _LL408: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2992 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL409: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2994 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL40A: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2998 "parse.y"
yyval=yyyyvsp[0];
break;}case 448: _LL40B: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3000 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL40C: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3002 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL40D: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3004 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL40E: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3006 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL40F: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3010 "parse.y"
yyval=yyyyvsp[0];
break;}case 453: _LL410: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3012 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 454: _LL411: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3014 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL412: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3018 "parse.y"
yyval=yyyyvsp[0];
break;}case 456: _LL413: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3020 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL414: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3022 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL415: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3026 "parse.y"
yyval=yyyyvsp[0];
break;}case 459: _LL416: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3028 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 460: _LL417: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3030 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL418: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3032 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL419: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3036 "parse.y"
yyval=yyyyvsp[0];
break;}case 463: _LL41A: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3038 "parse.y"
void*_tmp819=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp819,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 3041
break;}case 464: _LL41B: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3044 "parse.y"
yyval=yyyyvsp[0];
break;}case 465: _LL41C: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3045 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL41D: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3046 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 467: _LL41E: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3047 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL41F: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3048 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 469: _LL420: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3049 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 470: _LL421: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3050 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 471: _LL422: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3052 "parse.y"
void*_tmp81A=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp81A,(unsigned int)(yyyylsp[0]).first_line));
# 3055
break;}case 472: _LL423: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3055 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 473: _LL424: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3057 "parse.y"
void*_tmp81B=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp81B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3060
break;}case 474: _LL425: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3062 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEF3;struct Cyc_Absyn_MallocInfo _tmpEF2;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEF1;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1[0]=((_tmpEF3.tag=33,((_tmpEF3.f1=((_tmpEF2.is_calloc=0,((_tmpEF2.rgn=0,((_tmpEF2.elt_type=0,((_tmpEF2.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEF2.fat_result=0,((_tmpEF2.inline_call=0,_tmpEF2)))))))))))),_tmpEF3)))),_tmpEF1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3064
break;}case 475: _LL426: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3065 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEF9;struct Cyc_Absyn_MallocInfo _tmpEF8;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEF7;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7[0]=((_tmpEF9.tag=33,((_tmpEF9.f1=((_tmpEF8.is_calloc=0,((_tmpEF8.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEF8.elt_type=0,((_tmpEF8.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEF8.fat_result=0,((_tmpEF8.inline_call=0,_tmpEF8)))))))))))),_tmpEF9)))),_tmpEF7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3067
break;}case 476: _LL427: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3068 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEFF;struct Cyc_Absyn_MallocInfo _tmpEFE;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEFD;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD[0]=((_tmpEFF.tag=33,((_tmpEFF.f1=((_tmpEFE.is_calloc=0,((_tmpEFE.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEFE.elt_type=0,((_tmpEFE.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEFE.fat_result=0,((_tmpEFE.inline_call=1,_tmpEFE)))))))))))),_tmpEFF)))),_tmpEFD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3070
break;}case 477: _LL428: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3071 "parse.y"
void*_tmp825=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF09;void**_tmpF08;struct Cyc_Absyn_MallocInfo _tmpF07;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpF06;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06[0]=((_tmpF09.tag=33,((_tmpF09.f1=((_tmpF07.is_calloc=1,((_tmpF07.rgn=0,((_tmpF07.elt_type=((_tmpF08=_cycalloc(sizeof(*_tmpF08)),((_tmpF08[0]=_tmp825,_tmpF08)))),((_tmpF07.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF07.fat_result=0,((_tmpF07.inline_call=0,_tmpF07)))))))))))),_tmpF09)))),_tmpF06)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3074
break;}case 478: _LL429: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3076 "parse.y"
void*_tmp82A=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF13;void**_tmpF12;struct Cyc_Absyn_MallocInfo _tmpF11;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpF10;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF10=_cycalloc(sizeof(*_tmpF10)),((_tmpF10[0]=((_tmpF13.tag=33,((_tmpF13.f1=((_tmpF11.is_calloc=1,((_tmpF11.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF11.elt_type=((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=_tmp82A,_tmpF12)))),((_tmpF11.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpF11.fat_result=0,((_tmpF11.inline_call=0,_tmpF11)))))))))))),_tmpF13)))),_tmpF10)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3079
break;}case 479: _LL42A: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3080 "parse.y"
{struct Cyc_Absyn_Exp*_tmpF14[1];yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpF14[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF14,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 480: _LL42B: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3082 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF1A;struct _dyneither_ptr*_tmpF19;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpF18;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18[0]=((_tmpF1A.tag=37,((_tmpF1A.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF1A.f2=((_tmpF19=_cycalloc(sizeof(*_tmpF19)),((_tmpF19[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpF19)))),_tmpF1A)))))),_tmpF18)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 481: _LL42C: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3084 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF20;struct _dyneither_ptr*_tmpF1F;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpF1E;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=((_tmpF20.tag=37,((_tmpF20.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpF20.f2=((_tmpF1F=_cycalloc(sizeof(*_tmpF1F)),((_tmpF1F[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpF1F)))),_tmpF20)))))),_tmpF1E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 482: _LL42D: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3086 "parse.y"
void*_tmp836=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp836,(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL42E: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3089 "parse.y"
struct _tuple20 _tmp837=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp839;struct _dyneither_ptr _tmp83A;struct _tuple20 _tmp838=_tmp837;_tmp839=_tmp838.f1;_tmp83A=_tmp838.f2;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp839,_tmp83A,(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL42F: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3094 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
break;}case 485: _LL430: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3095 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
break;}case 486: _LL431: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3096 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
break;}case 487: _LL432: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3101 "parse.y"
yyval=yyyyvsp[0];
break;}case 488: _LL433: {
# 3104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3103 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 489: _LL434: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3105 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 490: _LL435: {
# 3108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3107 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 491: _LL436: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3109 "parse.y"
{struct _dyneither_ptr*_tmpF21;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF21=_cycalloc(sizeof(*_tmpF21)),((_tmpF21[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF21)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 492: _LL437: {
# 3112
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3111 "parse.y"
{struct _dyneither_ptr*_tmpF22;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF22=_cycalloc(sizeof(*_tmpF22)),((_tmpF22[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF22)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 493: _LL438: {
# 3114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3113 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 494: _LL439: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3115 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 495: _LL43A: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3117 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF25;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF24;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((_tmpF24[0]=((_tmpF25.tag=24,((_tmpF25.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF25.f2=0,_tmpF25)))))),_tmpF24)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 496: _LL43B: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3119 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF28;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF27;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF27=_cycalloc(sizeof(*_tmpF27)),((_tmpF27[0]=((_tmpF28.tag=24,((_tmpF28.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF28.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpF28)))))),_tmpF27)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 497: _LL43C: {
# 3122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3121 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF2B;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF2A;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF2A=_cycalloc(sizeof(*_tmpF2A)),((_tmpF2A[0]=((_tmpF2B.tag=24,((_tmpF2B.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF2B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpF2B)))))),_tmpF2A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 498: _LL43D: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF35;struct _dyneither_ptr*_tmpF34;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF33;struct Cyc_List_List*_tmpF32;yyval=Cyc_YY3(((_tmpF32=_cycalloc(sizeof(*_tmpF32)),((_tmpF32->hd=(void*)((_tmpF35=_cycalloc(sizeof(*_tmpF35)),((_tmpF35[0]=((_tmpF33.tag=0,((_tmpF33.f1=((_tmpF34=_cycalloc(sizeof(*_tmpF34)),((_tmpF34[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF34)))),_tmpF33)))),_tmpF35)))),((_tmpF32->tl=0,_tmpF32)))))));}
break;}case 499: _LL43E: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3129 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF3B;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF3A;struct Cyc_List_List*_tmpF39;yyval=Cyc_YY3(((_tmpF39=_cycalloc(sizeof(*_tmpF39)),((_tmpF39->hd=(void*)((_tmpF3B=_cycalloc_atomic(sizeof(*_tmpF3B)),((_tmpF3B[0]=((_tmpF3A.tag=1,((_tmpF3A.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_Int_tok(yyyyvsp[0])),_tmpF3A)))),_tmpF3B)))),((_tmpF39->tl=0,_tmpF39)))))));}
break;}case 500: _LL43F: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3131 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF45;struct _dyneither_ptr*_tmpF44;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF43;struct Cyc_List_List*_tmpF42;yyval=Cyc_YY3(((_tmpF42=_cycalloc(sizeof(*_tmpF42)),((_tmpF42->hd=(void*)((_tmpF45=_cycalloc(sizeof(*_tmpF45)),((_tmpF45[0]=((_tmpF43.tag=0,((_tmpF43.f1=((_tmpF44=_cycalloc(sizeof(*_tmpF44)),((_tmpF44[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF44)))),_tmpF43)))),_tmpF45)))),((_tmpF42->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF42)))))));}
break;}case 501: _LL440: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3134 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF4B;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF4A;struct Cyc_List_List*_tmpF49;yyval=Cyc_YY3(((_tmpF49=_cycalloc(sizeof(*_tmpF49)),((_tmpF49->hd=(void*)((_tmpF4B=_cycalloc_atomic(sizeof(*_tmpF4B)),((_tmpF4B[0]=((_tmpF4A.tag=1,((_tmpF4A.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_Int_tok(yyyyvsp[2])),_tmpF4A)))),_tmpF4B)))),((_tmpF49->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF49)))))));}
break;}case 502: _LL441: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL442: {
# 3143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3142 "parse.y"
yyval=yyyyvsp[0];
break;}case 504: _LL443: {
# 3145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3144 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL444: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3146 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 506: _LL445: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3148 "parse.y"
yyval=yyyyvsp[1];
break;}case 507: _LL446: {
# 3151
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3153
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3153 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 508: _LL447: {
# 3156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3155 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 509: _LL448: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3158 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 510: _LL449: {
# 3161
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3161 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF4E;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF4D;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF4D=_cycalloc(sizeof(*_tmpF4D)),((_tmpF4D[0]=((_tmpF4E.tag=28,((_tmpF4E.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF4E.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpF4E.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3])),((_tmpF4E.f4=0,_tmpF4E)))))))))),_tmpF4D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 511: _LL44A: {
# 3164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3164 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 512: _LL44B: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3168 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
break;}case 513: _LL44C: {
# 3171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpF4F;yyval=Cyc_YY4(((_tmpF4F=_cycalloc(sizeof(*_tmpF4F)),((_tmpF4F->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF4F->tl=0,_tmpF4F)))))));}
break;}case 514: _LL44D: {
# 3177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3176 "parse.y"
{struct Cyc_List_List*_tmpF50;yyval=Cyc_YY4(((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF50->tl=Cyc_yyget_YY4(yyyyvsp[0]),_tmpF50)))))));}
break;}case 515: _LL44E: {
# 3179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_const_exp(Cyc_yyget_Int_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 516: _LL44F: {
# 3185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3183 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 517: _LL450: {
# 3186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3184 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 518: _LL451: {
# 3187
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3186 "parse.y"
struct _dyneither_ptr _tmp855=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp855);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp855,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3194
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp855,i,(unsigned int)(yyyylsp[0]).first_line));
# 3196
break;}case 519: _LL452: {
# 3198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3197 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 520: _LL453: {
# 3200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3201 "parse.y"
{struct _dyneither_ptr*_tmpF53;struct _tuple0*_tmpF52;yyval=Cyc_QualId_tok(((_tmpF52=_cycalloc(sizeof(*_tmpF52)),((_tmpF52->f1=Cyc_Absyn_Rel_n(0),((_tmpF52->f2=((_tmpF53=_cycalloc(sizeof(*_tmpF53)),((_tmpF53[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF53)))),_tmpF52)))))));}
break;}case 521: _LL454: {
# 3204
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3202 "parse.y"
yyval=yyyyvsp[0];
break;}case 522: _LL455: {
# 3205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3205 "parse.y"
{struct _dyneither_ptr*_tmpF56;struct _tuple0*_tmpF55;yyval=Cyc_QualId_tok(((_tmpF55=_cycalloc(sizeof(*_tmpF55)),((_tmpF55->f1=Cyc_Absyn_Rel_n(0),((_tmpF55->f2=((_tmpF56=_cycalloc(sizeof(*_tmpF56)),((_tmpF56[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF56)))),_tmpF55)))))));}
break;}case 523: _LL456: {
# 3208
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3206 "parse.y"
yyval=yyyyvsp[0];
break;}case 524: _LL457: {
# 3209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3211 "parse.y"
yyval=yyyyvsp[0];
break;}case 525: _LL458: {
# 3214
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3212 "parse.y"
yyval=yyyyvsp[0];
break;}case 526: _LL459: {
# 3215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3215 "parse.y"
yyval=yyyyvsp[0];
break;}case 527: _LL45A: {
# 3218
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3220
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3216 "parse.y"
yyval=yyyyvsp[0];
break;}case 528: _LL45B: {
# 3219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3221 "parse.y"
break;}case 529: _LL45C: {
# 3223
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3225
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3221 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL45D:
# 3226
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(530,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(148,yyn - 148)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 7281) && Cyc_yycheck[_check_known_subscript_notnull(7282,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(7282,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(148,yyn - 148)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1077,yystate)];
# 431
if(yyn > - 32768  && yyn < 7281){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 296 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(7282,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(296,x)])+ 15,count ++);}
{unsigned int _tmpF5B;unsigned int _tmpF5A;struct _dyneither_ptr _tmpF59;char*_tmpF58;unsigned int _tmpF57;msg=((_tmpF57=(unsigned int)(sze + 15),((_tmpF58=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpF57 + 1)),((_tmpF59=_tag_dyneither(_tmpF58,sizeof(char),_tmpF57 + 1),((((_tmpF5A=_tmpF57,((_tmpF5C(& _tmpF5B,& _tmpF5A,& _tmpF58),_tmpF58[_tmpF5A]=(char)0)))),_tmpF59))))))));}
{const char*_tmpF5D;Cyc_strcpy(msg,((_tmpF5D="parse error",_tag_dyneither(_tmpF5D,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 296 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(7282,x + yyn)]== x){
# 453
{const char*_tmpF5F;const char*_tmpF5E;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpF5F=", expecting `",_tag_dyneither(_tmpF5F,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpF5E=" or `",_tag_dyneither(_tmpF5E,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(296,x)]);
{const char*_tmpF60;Cyc_strcat(msg,((_tmpF60="'",_tag_dyneither(_tmpF60,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
const char*_tmpF61;Cyc_yyerror(((_tmpF61="parse error",_tag_dyneither(_tmpF61,sizeof(char),12))),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp863=1;_npop_handler(0);return _tmp863;}
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
 if(yyssp_offset == 0){int _tmp864=1;_npop_handler(0);return _tmp864;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1077,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 7281) || Cyc_yycheck[_check_known_subscript_notnull(7282,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(7282,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1076){
int _tmp865=0;_npop_handler(0);return _tmp865;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3224 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp86F=v;union Cyc_Absyn_Cnst _tmp870;char _tmp871;struct _dyneither_ptr _tmp872;union Cyc_Absyn_Nmspace _tmp873;struct _dyneither_ptr*_tmp874;struct Cyc_Absyn_Exp*_tmp875;struct Cyc_Absyn_Stmt*_tmp876;_LL460: if((_tmp86F.Int_tok).tag != 1)goto _LL462;_tmp870=(union Cyc_Absyn_Cnst)(_tmp86F.Int_tok).val;_LL461:
{const char*_tmpF65;void*_tmpF64[1];struct Cyc_String_pa_PrintArg_struct _tmpF63;(_tmpF63.tag=0,((_tmpF63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp870)),((_tmpF64[0]=& _tmpF63,Cyc_fprintf(Cyc_stderr,((_tmpF65="%s",_tag_dyneither(_tmpF65,sizeof(char),3))),_tag_dyneither(_tmpF64,sizeof(void*),1)))))));}goto _LL45F;_LL462: if((_tmp86F.Char_tok).tag != 2)goto _LL464;_tmp871=(char)(_tmp86F.Char_tok).val;_LL463:
{const char*_tmpF69;void*_tmpF68[1];struct Cyc_Int_pa_PrintArg_struct _tmpF67;(_tmpF67.tag=1,((_tmpF67.f1=(unsigned long)((int)_tmp871),((_tmpF68[0]=& _tmpF67,Cyc_fprintf(Cyc_stderr,((_tmpF69="%c",_tag_dyneither(_tmpF69,sizeof(char),3))),_tag_dyneither(_tmpF68,sizeof(void*),1)))))));}goto _LL45F;_LL464: if((_tmp86F.String_tok).tag != 3)goto _LL466;_tmp872=(struct _dyneither_ptr)(_tmp86F.String_tok).val;_LL465:
{const char*_tmpF6D;void*_tmpF6C[1];struct Cyc_String_pa_PrintArg_struct _tmpF6B;(_tmpF6B.tag=0,((_tmpF6B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp872),((_tmpF6C[0]=& _tmpF6B,Cyc_fprintf(Cyc_stderr,((_tmpF6D="\"%s\"",_tag_dyneither(_tmpF6D,sizeof(char),5))),_tag_dyneither(_tmpF6C,sizeof(void*),1)))))));}goto _LL45F;_LL466: if((_tmp86F.QualId_tok).tag != 5)goto _LL468;_tmp873=((struct _tuple0*)(_tmp86F.QualId_tok).val)->f1;_tmp874=((struct _tuple0*)(_tmp86F.QualId_tok).val)->f2;_LL467: {
# 3230
struct Cyc_List_List*_tmp880=0;
{union Cyc_Absyn_Nmspace _tmp881=_tmp873;struct Cyc_List_List*_tmp882;struct Cyc_List_List*_tmp883;struct Cyc_List_List*_tmp884;_LL46F: if((_tmp881.Rel_n).tag != 1)goto _LL471;_tmp882=(struct Cyc_List_List*)(_tmp881.Rel_n).val;_LL470:
 _tmp880=_tmp882;goto _LL46E;_LL471: if((_tmp881.Abs_n).tag != 2)goto _LL473;_tmp883=(struct Cyc_List_List*)(_tmp881.Abs_n).val;_LL472:
 _tmp880=_tmp883;goto _LL46E;_LL473: if((_tmp881.C_n).tag != 3)goto _LL475;_tmp884=(struct Cyc_List_List*)(_tmp881.C_n).val;_LL474:
 _tmp880=_tmp884;goto _LL46E;_LL475: if((_tmp881.Loc_n).tag != 4)goto _LL46E;_LL476:
 goto _LL46E;_LL46E:;}
# 3237
for(0;_tmp880 != 0;_tmp880=_tmp880->tl){
const char*_tmpF71;void*_tmpF70[1];struct Cyc_String_pa_PrintArg_struct _tmpF6F;(_tmpF6F.tag=0,((_tmpF6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp880->hd)),((_tmpF70[0]=& _tmpF6F,Cyc_fprintf(Cyc_stderr,((_tmpF71="%s::",_tag_dyneither(_tmpF71,sizeof(char),5))),_tag_dyneither(_tmpF70,sizeof(void*),1)))))));}
{const char*_tmpF75;void*_tmpF74[1];struct Cyc_String_pa_PrintArg_struct _tmpF73;(_tmpF73.tag=0,((_tmpF73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp874),((_tmpF74[0]=& _tmpF73,Cyc_fprintf(Cyc_stderr,((_tmpF75="%s::",_tag_dyneither(_tmpF75,sizeof(char),5))),_tag_dyneither(_tmpF74,sizeof(void*),1)))))));}
goto _LL45F;}_LL468: if((_tmp86F.Exp_tok).tag != 7)goto _LL46A;_tmp875=(struct Cyc_Absyn_Exp*)(_tmp86F.Exp_tok).val;_LL469:
{const char*_tmpF79;void*_tmpF78[1];struct Cyc_String_pa_PrintArg_struct _tmpF77;(_tmpF77.tag=0,((_tmpF77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp875)),((_tmpF78[0]=& _tmpF77,Cyc_fprintf(Cyc_stderr,((_tmpF79="%s",_tag_dyneither(_tmpF79,sizeof(char),3))),_tag_dyneither(_tmpF78,sizeof(void*),1)))))));}goto _LL45F;_LL46A: if((_tmp86F.Stmt_tok).tag != 8)goto _LL46C;_tmp876=(struct Cyc_Absyn_Stmt*)(_tmp86F.Stmt_tok).val;_LL46B:
{const char*_tmpF7D;void*_tmpF7C[1];struct Cyc_String_pa_PrintArg_struct _tmpF7B;(_tmpF7B.tag=0,((_tmpF7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp876)),((_tmpF7C[0]=& _tmpF7B,Cyc_fprintf(Cyc_stderr,((_tmpF7D="%s",_tag_dyneither(_tmpF7D,sizeof(char),3))),_tag_dyneither(_tmpF7C,sizeof(void*),1)))))));}goto _LL45F;_LL46C:;_LL46D:
{const char*_tmpF80;void*_tmpF7F;(_tmpF7F=0,Cyc_fprintf(Cyc_stderr,((_tmpF80="?",_tag_dyneither(_tmpF80,sizeof(char),2))),_tag_dyneither(_tmpF7F,sizeof(void*),0)));}goto _LL45F;_LL45F:;}
# 3248
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp893=_new_region("yyr");struct _RegionHandle*yyr=& _tmp893;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp894=Cyc_Parse_parse_result;_npop_handler(0);return _tmp894;};
# 3251
;_pop_region(yyr);};}
