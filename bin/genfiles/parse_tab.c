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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 338
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 355
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 369
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 450 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 505
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 512
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
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 750 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 922
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 949
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 953
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 957
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 967
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 972
void*Cyc_Absyn_compress_kb(void*);
# 977
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 979
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 982
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 984
extern void*Cyc_Absyn_sint_typ;
# 986
void*Cyc_Absyn_float_typ(int);
# 1034
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1083
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1085
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
# 1101
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1109
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1114
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1127
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1130
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1133
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1137
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1142
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1144
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1151
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1160
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1165
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1171
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1196
int Cyc_Absyn_fntype_att(void*a);
# 1216
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
{struct _tuple10*_tmp848;declarators=((_tmp848=_region_malloc(r,sizeof(*_tmp848)),((_tmp848->tl=declarators,((_tmp848->hd=_tmp2,_tmp848))))));}{
struct Cyc_List_List*_tmp849;exprs=((_tmp849=_region_malloc(r,sizeof(*_tmp849)),((_tmp849->hd=_tmp3,((_tmp849->tl=exprs,_tmp849))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp84C;struct Cyc_Position_Exit_exn_struct*_tmp84B;(int)_throw((void*)((_tmp84B=_cycalloc_atomic(sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp84C.tag=Cyc_Position_Exit,_tmp84C)),_tmp84B)))));};}
# 191
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp851;void*_tmp850[2];struct Cyc_String_pa_PrintArg_struct _tmp84F;struct Cyc_String_pa_PrintArg_struct _tmp84E;(_tmp84E.tag=0,((_tmp84E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp84F.tag=0,((_tmp84F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp850[0]=& _tmp84F,((_tmp850[1]=& _tmp84E,Cyc_fprintf(Cyc_stderr,((_tmp851="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp851,sizeof(char),46))),_tag_dyneither(_tmp850,sizeof(void*),2)))))))))))));}
return;}
# 197
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 199
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp854;void*_tmp853;(_tmp853=0,Cyc_Tcutil_warn(loc,((_tmp854="qualifier on type is ignored",_tag_dyneither(_tmp854,sizeof(char),29))),_tag_dyneither(_tmp853,sizeof(void*),0)));};}
# 204
return _tmpE;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 207
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 213
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp11=(void*)pqs->hd;void*_tmp12=_tmp11;void*_tmp13;struct Cyc_Absyn_Exp*_tmp14;switch(*((int*)_tmp12)){case 4: _LL1: _LL2:
# 217
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;case 5: _LL3: _LL4:
# 219
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;case 7: _LL5: _LL6:
# 221
 nullable=Cyc_Absyn_true_conref;goto _LL0;case 6: _LL7: _LL8:
# 223
 nullable=Cyc_Absyn_false_conref;goto _LL0;case 3: _LL9: _LLA:
# 225
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;case 2: _LLB: _LLC:
# 227
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;case 0: _LLD: _tmp14=((struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp12)->f1;_LLE:
# 229
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp857;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp856;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp856=_cycalloc(sizeof(*_tmp856)),((_tmp856[0]=((_tmp857.tag=1,((_tmp857.f1=_tmp14,_tmp857)))),_tmp856)))));}goto _LL0;default: _LLF: _tmp13=(void*)((struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12)->f1;_LL10:
# 231
 rgn=_tmp13;goto _LL0;}_LL0:;}{
# 234
struct _tuple14 _tmp858;return(_tmp858.f1=nullable,((_tmp858.f2=bound,((_tmp858.f3=zeroterm,((_tmp858.f4=rgn,_tmp858)))))));};}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
struct _dyneither_ptr*_tmp865;const char*_tmp864;void*_tmp863[1];struct Cyc_Int_pa_PrintArg_struct _tmp862;struct _tuple0*_tmp861;return(_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861->f1=Cyc_Absyn_Rel_n(0),((_tmp861->f2=(
(_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865[0]=(struct _dyneither_ptr)((_tmp862.tag=1,((_tmp862.f1=(unsigned long)enum_counter ++,((_tmp863[0]=& _tmp862,Cyc_aprintf(((_tmp864="__anonymous_enum_%d__",_tag_dyneither(_tmp864,sizeof(char),22))),_tag_dyneither(_tmp863,sizeof(void*),1)))))))),_tmp865)))),_tmp861)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple0*_tmp1E;struct Cyc_Absyn_Tqual _tmp1F;void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_List_List*_tmp22;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp24;struct _tuple17*_tmp1D=field_info;_tmp1E=(_tmp1D->f1)->f1;_tmp1F=(_tmp1D->f1)->f2;_tmp20=(_tmp1D->f1)->f3;_tmp21=(_tmp1D->f1)->f4;_tmp22=(_tmp1D->f1)->f5;_tmp23=(_tmp1D->f2)->f1;_tmp24=(_tmp1D->f2)->f2;
if(_tmp21 != 0){
const char*_tmp866;Cyc_Parse_err(((_tmp866="bad type params in struct field",_tag_dyneither(_tmp866,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp1E)){
const char*_tmp867;Cyc_Parse_err(((_tmp867="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp867,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp868;return(_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868->name=(*_tmp1E).f2,((_tmp868->tq=_tmp1F,((_tmp868->type=_tmp20,((_tmp868->width=_tmp23,((_tmp868->attributes=_tmp22,((_tmp868->requires_clause=_tmp24,_tmp868)))))))))))));};}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp869;return(_tmp869.Signed_spec=0,((_tmp869.Unsigned_spec=0,((_tmp869.Short_spec=0,((_tmp869.Long_spec=0,((_tmp869.Long_Long_spec=0,((_tmp869.Valid_type_spec=0,((_tmp869.Type_spec=Cyc_Absyn_sint_typ,((_tmp869.loc=loc,_tmp869)))))))))))))));}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp29=Cyc_Parse_empty_spec(loc);
_tmp29.Type_spec=t;
_tmp29.Valid_type_spec=1;
return _tmp29;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2A=Cyc_Parse_empty_spec(loc);
_tmp2A.Signed_spec=1;
return _tmp2A;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2B=Cyc_Parse_empty_spec(loc);
_tmp2B.Unsigned_spec=1;
return _tmp2B;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2C=Cyc_Parse_empty_spec(loc);
_tmp2C.Short_spec=1;
return _tmp2C;}
# 294
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2D=Cyc_Parse_empty_spec(loc);
_tmp2D.Long_spec=1;
return _tmp2D;}
# 301
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 303
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 316 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp2E=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp2F=(struct _tuple8*)x->hd;struct _tuple8*_tmp30=_tmp2F;struct _dyneither_ptr _tmp31;void**_tmp32;struct _dyneither_ptr*_tmp33;void*_tmp34;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)((struct _tuple8*)_tmp30)->f3)->tag == 19){_LL12: _tmp33=_tmp30->f1;_tmp34=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp30->f3)->f1;if(_tmp33 != 0){_LL13:
# 321
{void*_tmp35=_tmp34;void**_tmp36;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35)->tag == 1){_LL19: _tmp36=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35)->f2;_LL1A: {
# 325
struct Cyc_String_pa_PrintArg_struct _tmp871;void*_tmp870[1];const char*_tmp86F;struct _dyneither_ptr*_tmp86E;struct _dyneither_ptr*nm=(_tmp86E=_cycalloc(sizeof(*_tmp86E)),((_tmp86E[0]=(struct _dyneither_ptr)((_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp33),((_tmp870[0]=& _tmp871,Cyc_aprintf(((_tmp86F="`%s",_tag_dyneither(_tmp86F,sizeof(char),4))),_tag_dyneither(_tmp870,sizeof(void*),1)))))))),_tmp86E)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp880;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp87F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp87E;struct Cyc_Absyn_Tvar*_tmp87D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp87C;*_tmp36=(void*)((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=((_tmp880.tag=2,((_tmp880.f1=((_tmp87D=_cycalloc(sizeof(*_tmp87D)),((_tmp87D->name=nm,((_tmp87D->identity=- 1,((_tmp87D->kind=(void*)((_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=((_tmp87E.tag=0,((_tmp87E.f1=& Cyc_Tcutil_ik,_tmp87E)))),_tmp87F)))),_tmp87D)))))))),_tmp880)))),_tmp87C))));}
goto _LL18;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 330
{struct _tuple18*_tmp883;struct Cyc_List_List*_tmp882;_tmp2E=((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882->hd=((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883->f1=_tmp33,((_tmp883->f2=_tmp34,_tmp883)))))),((_tmp882->tl=_tmp2E,_tmp882))))));}goto _LL11;}else{if(((struct _tuple8*)_tmp30)->f1 != 0)goto _LL16;else{goto _LL16;}}}else{if(((struct _tuple8*)_tmp30)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp30)->f3)->tag == 15){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp30)->f3)->f1)->tag == 1){_LL14: _tmp31=*_tmp30->f1;_tmp32=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp30->f3)->f1)->f2;_LL15: {
# 334
struct Cyc_String_pa_PrintArg_struct _tmp88B;void*_tmp88A[1];const char*_tmp889;struct _dyneither_ptr*_tmp888;struct _dyneither_ptr*nm=(_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=(struct _dyneither_ptr)((_tmp88B.tag=0,((_tmp88B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31),((_tmp88A[0]=& _tmp88B,Cyc_aprintf(((_tmp889="`%s",_tag_dyneither(_tmp889,sizeof(char),4))),_tag_dyneither(_tmp88A,sizeof(void*),1)))))))),_tmp888)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp89A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp899;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp898;struct Cyc_Absyn_Tvar*_tmp897;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp896;*_tmp32=(void*)((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896[0]=((_tmp89A.tag=2,((_tmp89A.f1=((_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897->name=nm,((_tmp897->identity=- 1,((_tmp897->kind=(void*)((_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899[0]=((_tmp898.tag=0,((_tmp898.f1=& Cyc_Tcutil_rk,_tmp898)))),_tmp899)))),_tmp897)))))))),_tmp89A)))),_tmp896))));}
goto _LL11;}}else{goto _LL16;}}else{goto _LL16;}}else{_LL16: _LL17:
 goto _LL11;}}_LL11:;}
# 340
return _tmp2E;}
# 344
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp4B=0;
for(0;x != 0;x=x->tl){
void*_tmp4C=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp4D=_tmp4C;void*_tmp4E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4D)->tag == 19){_LL1E: _tmp4E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4D)->f1;_LL1F:
# 349
{struct _tuple18*_tmp89D;struct Cyc_List_List*_tmp89C;_tmp4B=((_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C->hd=((_tmp89D=_cycalloc(sizeof(*_tmp89D)),((_tmp89D->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp89D->f2=_tmp4E,_tmp89D)))))),((_tmp89C->tl=_tmp4B,_tmp89C))))));}goto _LL1D;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 353
return _tmp4B;}
# 357
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp51=e->r;void*_tmp52=_tmp51;struct _dyneither_ptr*_tmp53;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->f1)->f1).Rel_n).val == 0){_LL23: _tmp53=(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->f1)->f2;_LL24:
# 360
{struct Cyc_List_List*_tmp54=tags;for(0;_tmp54 != 0;_tmp54=_tmp54->tl){
struct _tuple18*_tmp55=(struct _tuple18*)_tmp54->hd;struct _dyneither_ptr*_tmp57;void*_tmp58;struct _tuple18*_tmp56=_tmp55;_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;
if(Cyc_strptrcmp(_tmp57,_tmp53)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8A0;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp89F;return Cyc_Absyn_new_exp((void*)((_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F[0]=((_tmp8A0.tag=38,((_tmp8A0.f1=Cyc_Tcutil_copy_type(_tmp58),_tmp8A0)))),_tmp89F)))),e->loc);}}}
# 365
goto _LL22;}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 368
return e;}
# 373
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp5B=t;void*_tmp5C;struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;union Cyc_Absyn_Constraint*_tmp5F;union Cyc_Absyn_Constraint*_tmp60;union Cyc_Absyn_Constraint*_tmp61;struct Cyc_Absyn_PtrLoc*_tmp62;void*_tmp63;struct Cyc_Absyn_Tqual _tmp64;struct Cyc_Absyn_Exp*_tmp65;union Cyc_Absyn_Constraint*_tmp66;unsigned int _tmp67;switch(*((int*)_tmp5B)){case 8: _LL28: _tmp63=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).elt_type;_tmp64=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).tq;_tmp65=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).num_elts;_tmp66=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).zero_term;_tmp67=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).zt_loc;_LL29: {
# 376
struct Cyc_Absyn_Exp*nelts2=_tmp65;
if(_tmp65 != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp65);{
# 380
void*_tmp68=Cyc_Parse_substitute_tags(tags,_tmp63);
if(_tmp65 != nelts2  || _tmp63 != _tmp68){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8A6;struct Cyc_Absyn_ArrayInfo _tmp8A5;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8A4;return(void*)((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A6.tag=8,((_tmp8A6.f1=((_tmp8A5.elt_type=_tmp68,((_tmp8A5.tq=_tmp64,((_tmp8A5.num_elts=nelts2,((_tmp8A5.zero_term=_tmp66,((_tmp8A5.zt_loc=_tmp67,_tmp8A5)))))))))),_tmp8A6)))),_tmp8A4))));}
goto _LL27;};}case 5: _LL2A: _tmp5C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).elt_typ;_tmp5D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).elt_tq;_tmp5E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).rgn;_tmp5F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).nullable;_tmp60=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).bounds;_tmp61=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).zero_term;_tmp62=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).ptrloc;_LL2B: {
# 385
void*_tmp6C=Cyc_Parse_substitute_tags(tags,_tmp5C);
union Cyc_Absyn_Constraint*_tmp6D=_tmp60;
{union Cyc_Absyn_Constraint _tmp6E=*_tmp60;union Cyc_Absyn_Constraint _tmp6F=_tmp6E;struct Cyc_Absyn_Exp*_tmp70;if((_tmp6F.Eq_constr).tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp6F.Eq_constr).val)->tag == 1){_LL2F: _tmp70=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp6F.Eq_constr).val)->f1;_LL30:
# 389
 _tmp70=Cyc_Parse_substitute_tags_exp(tags,_tmp70);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8AC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8AB;union Cyc_Absyn_Constraint*_tmp8AA;_tmp6D=((_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),(((_tmp8AA->Eq_constr).val=(void*)((_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AB.tag=1,((_tmp8AB.f1=_tmp70,_tmp8AB)))),_tmp8AC)))),(((_tmp8AA->Eq_constr).tag=1,_tmp8AA))))));}
goto _LL2E;}else{goto _LL31;}}else{_LL31: _LL32:
 goto _LL2E;}_LL2E:;}
# 394
if(_tmp6C != _tmp5C  || _tmp6D != _tmp60){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8B2;struct Cyc_Absyn_PtrInfo _tmp8B1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8B0;return(void*)((_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B2.tag=5,((_tmp8B2.f1=((_tmp8B1.elt_typ=_tmp6C,((_tmp8B1.elt_tq=_tmp5D,((_tmp8B1.ptr_atts=(((_tmp8B1.ptr_atts).rgn=_tmp5E,(((_tmp8B1.ptr_atts).nullable=_tmp5F,(((_tmp8B1.ptr_atts).bounds=_tmp6D,(((_tmp8B1.ptr_atts).zero_term=_tmp61,(((_tmp8B1.ptr_atts).ptrloc=_tmp62,_tmp8B1.ptr_atts)))))))))),_tmp8B1)))))),_tmp8B2)))),_tmp8B0))));}
goto _LL27;}default: _LL2C: _LL2D:
# 399
 goto _LL27;}_LL27:;}
# 401
return t;}
# 406
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 413
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 415
struct _tuple19*_tmp8B3;return(_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3->f1=(*t).f2,((_tmp8B3->f2=(*t).f3,_tmp8B3)))));}
# 418
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp78=d->r;void*_tmp79=_tmp78;struct Cyc_Absyn_Vardecl*_tmp7A;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79)->tag == 0){_LL34: _tmp7A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79)->f1;_LL35:
 return _tmp7A;}else{_LL36: _LL37: {
const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8B6="bad declaration in `forarray' statement",_tag_dyneither(_tmp8B6,sizeof(char),40))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}}_LL33:;}
# 425
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp7D=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp7D)->tag == 4){_LL39: _LL3A:
 return 1;}else{_LL3B: _LL3C:
 return 0;}_LL38:;}
# 434
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8B7;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8B7="`H",_tag_dyneither(_tmp8B7,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8B8;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8B8="`U",_tag_dyneither(_tmp8B8,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8B9;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8B9="`RC",_tag_dyneither(_tmp8B9,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 442
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8C3;struct _dyneither_ptr*_tmp8C2;struct Cyc_Absyn_Tvar*_tmp8C1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8C0;return(void*)((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C3.tag=2,((_tmp8C3.f1=((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1->name=((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=s,_tmp8C2)))),((_tmp8C1->identity=- 1,((_tmp8C1->kind=k,_tmp8C1)))))))),_tmp8C3)))),_tmp8C0))));}}}}
# 445
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp85=Cyc_Absyn_compress_kb(t->kind);void*_tmp86=_tmp85;void*_tmp87;struct Cyc_Absyn_Kind*_tmp88;switch(*((int*)_tmp86)){case 1: _LL3E: _LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8C6;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8C5;k=(void*)((_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5[0]=((_tmp8C6.tag=1,((_tmp8C6.f1=0,_tmp8C6)))),_tmp8C5))));}goto _LL3D;case 2: _LL40: _tmp88=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86)->f2;_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8C9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8C8;k=(void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=2,((_tmp8C9.f1=0,((_tmp8C9.f2=_tmp88,_tmp8C9)))))),_tmp8C8))));}goto _LL3D;default: _LL42: _tmp87=_tmp86;_LL43:
 k=_tmp87;goto _LL3D;}_LL3D:;}{
# 452
struct Cyc_Absyn_Tvar*_tmp8CA;return(_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA->name=t->name,((_tmp8CA->identity=- 1,((_tmp8CA->kind=k,_tmp8CA)))))));};}
# 459
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp8E=t;struct Cyc_Absyn_Tvar*_tmp8F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8E)->tag == 2){_LL45: _tmp8F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8E)->f1;_LL46:
 return _tmp8F;}else{_LL47: _LL48: {
const char*_tmp8CD;void*_tmp8CC;(_tmp8CC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8CD="expecting a list of type variables, not types",_tag_dyneither(_tmp8CD,sizeof(char),46))),_tag_dyneither(_tmp8CC,sizeof(void*),0)));}}_LL44:;}
# 465
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8CF;return(void*)((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF[0]=((_tmp8D0.tag=2,((_tmp8D0.f1=pr,_tmp8D0)))),_tmp8CF))));}
# 470
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp94=Cyc_Tcutil_compress(t);void*_tmp95=_tmp94;void**_tmp96;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp95)->tag == 2){_LL4A: _tmp96=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp95)->f1)->kind;_LL4B: {
# 473
void*_tmp97=Cyc_Absyn_compress_kb(*_tmp96);void*_tmp98=_tmp97;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp98)->tag == 1){_LL4F: _LL50:
# 475
 if(!leq)*_tmp96=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8D3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8D2;*_tmp96=(void*)((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=2,((_tmp8D3.f1=0,((_tmp8D3.f2=k,_tmp8D3)))))),_tmp8D2))));}
return;}else{_LL51: _LL52:
 return;}_LL4E:;}}else{_LL4C: _LL4D:
# 480
 return;}_LL49:;}
# 485
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 491
if(tds == 0)return tms;
# 496
if(tms == 0)return 0;{
# 498
void*_tmp9B=(void*)tms->hd;void*_tmp9C=_tmp9B;void*_tmp9D;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp9C)->tag == 3){_LL54: _tmp9D=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp9C)->f1;_LL55:
# 501
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 504
void*_tmp9E=_tmp9D;struct Cyc_List_List*_tmp9F;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp9E)->tag == 1){_LL59: _LL5A:
# 506
{const char*_tmp8D6;void*_tmp8D5;(_tmp8D5=0,Cyc_Tcutil_warn(loc,((_tmp8D6="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8D6,sizeof(char),93))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}
# 508
return tms;}else{_LL5B: _tmp9F=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp9E)->f1;_LL5C:
# 510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8D9;void*_tmp8D8;(_tmp8D8=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8D9="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8D9,sizeof(char),73))),_tag_dyneither(_tmp8D8,sizeof(void*),0)));}{
# 514
struct Cyc_List_List*rev_new_params=0;
for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
struct Cyc_List_List*_tmpA4=tds;
for(0;_tmpA4 != 0;_tmpA4=_tmpA4->tl){
struct Cyc_Absyn_Decl*_tmpA5=(struct Cyc_Absyn_Decl*)_tmpA4->hd;
void*_tmpA6=_tmpA5->r;void*_tmpA7=_tmpA6;struct Cyc_Absyn_Vardecl*_tmpA8;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA7)->tag == 0){_LL5E: _tmpA8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA7)->f1;_LL5F:
# 521
 if(Cyc_zstrptrcmp((*_tmpA8->name).f2,(struct _dyneither_ptr*)_tmp9F->hd)!= 0)
continue;
if(_tmpA8->initializer != 0){
const char*_tmp8DC;void*_tmp8DB;(_tmp8DB=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpA5->loc,((_tmp8DC="initializer found in parameter declaration",_tag_dyneither(_tmp8DC,sizeof(char),43))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpA8->name)){
const char*_tmp8DF;void*_tmp8DE;(_tmp8DE=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpA5->loc,((_tmp8DF="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8DF,sizeof(char),47))),_tag_dyneither(_tmp8DE,sizeof(void*),0)));}
{struct _tuple8*_tmp8E2;struct Cyc_List_List*_tmp8E1;rev_new_params=(
(_tmp8E1=_cycalloc(sizeof(*_tmp8E1)),((_tmp8E1->hd=((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2->f1=(*_tmpA8->name).f2,((_tmp8E2->f2=_tmpA8->tq,((_tmp8E2->f3=_tmpA8->type,_tmp8E2)))))))),((_tmp8E1->tl=rev_new_params,_tmp8E1))))));}
# 530
goto L;}else{_LL60: _LL61: {
const char*_tmp8E5;void*_tmp8E4;(_tmp8E4=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpA5->loc,((_tmp8E5="nonvariable declaration in parameter type",_tag_dyneither(_tmp8E5,sizeof(char),42))),_tag_dyneither(_tmp8E4,sizeof(void*),0)));}}_LL5D:;}
# 534
L: if(_tmpA4 == 0){
const char*_tmp8E9;void*_tmp8E8[1];struct Cyc_String_pa_PrintArg_struct _tmp8E7;(_tmp8E7.tag=0,((_tmp8E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp9F->hd)),((_tmp8E8[0]=& _tmp8E7,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8E9="%s is not given a type",_tag_dyneither(_tmp8E9,sizeof(char),23))),_tag_dyneither(_tmp8E8,sizeof(void*),1)))))));}}{
# 537
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8F8;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8F7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8F6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8F5;struct Cyc_List_List*_tmp8F4;return
(_tmp8F4=_region_malloc(yy,sizeof(*_tmp8F4)),((_tmp8F4->hd=(void*)((_tmp8F8=_region_malloc(yy,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F5.tag=3,((_tmp8F5.f1=(void*)((_tmp8F7=_region_malloc(yy,sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8F6.tag=1,((_tmp8F6.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8F6.f2=0,((_tmp8F6.f3=0,((_tmp8F6.f4=0,((_tmp8F6.f5=0,((_tmp8F6.f6=0,((_tmp8F6.f7=0,_tmp8F6)))))))))))))))),_tmp8F7)))),_tmp8F5)))),_tmp8F8)))),((_tmp8F4->tl=0,_tmp8F4)))));};};}_LL58:;}
# 544
goto _LL57;}else{_LL56: _LL57: {
struct Cyc_List_List*_tmp8F9;return(_tmp8F9=_region_malloc(yy,sizeof(*_tmp8F9)),((_tmp8F9->hd=(void*)tms->hd,((_tmp8F9->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8F9)))));}}_LL53:;};}
# 552
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 556
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8FA;d=((_tmp8FA.id=d.id,((_tmp8FA.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8FA))));}{
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
enum Cyc_Parse_Storage_class _tmpBB=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmpBC=_tmpBB;switch(_tmpBC){case Cyc_Parse_Extern_sc: _LL63: _LL64:
 sc=Cyc_Absyn_Extern;goto _LL62;case Cyc_Parse_Static_sc: _LL65: _LL66:
 sc=Cyc_Absyn_Static;goto _LL62;default: _LL67: _LL68:
{const char*_tmp8FB;Cyc_Parse_err(((_tmp8FB="bad storage class on function",_tag_dyneither(_tmp8FB,sizeof(char),30))),loc);}goto _LL62;}_LL62:;}}{
# 578
void*_tmpBE=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpBF=Cyc_Parse_apply_tms(tq,_tmpBE,atts,d.tms);struct Cyc_Absyn_Tqual _tmpC1;void*_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;struct _tuple13 _tmpC0=_tmpBF;_tmpC1=_tmpC0.f1;_tmpC2=_tmpC0.f2;_tmpC3=_tmpC0.f3;_tmpC4=_tmpC0.f4;
# 583
if(_tmpC3 != 0){
# 586
const char*_tmp8FE;void*_tmp8FD;(_tmp8FD=0,Cyc_Tcutil_warn(loc,((_tmp8FE="bad type params, ignoring",_tag_dyneither(_tmp8FE,sizeof(char),26))),_tag_dyneither(_tmp8FD,sizeof(void*),0)));}{
# 588
void*_tmpC7=_tmpC2;struct Cyc_List_List*_tmpC8;void*_tmpC9;struct Cyc_Absyn_Tqual _tmpCA;void*_tmpCB;struct Cyc_List_List*_tmpCC;int _tmpCD;struct Cyc_Absyn_VarargInfo*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_List_List*_tmpD4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->tag == 9){_LL6A: _tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).tvars;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).effect;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ret_tqual;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ret_typ;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).args;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).c_varargs;_tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).cyc_varargs;_tmpCF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).rgn_po;_tmpD0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).attributes;_tmpD1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).requires_clause;_tmpD2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).requires_relns;_tmpD3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ensures_clause;_tmpD4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ensures_relns;_LL6B: {
# 592
struct Cyc_List_List*_tmpD5=0;
{struct Cyc_List_List*_tmpD6=_tmpCC;for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
struct _tuple8*_tmpD7=(struct _tuple8*)_tmpD6->hd;struct _dyneither_ptr*_tmpD9;struct Cyc_Absyn_Tqual _tmpDA;void*_tmpDB;struct _tuple8*_tmpD8=_tmpD7;_tmpD9=_tmpD8->f1;_tmpDA=_tmpD8->f2;_tmpDB=_tmpD8->f3;
if(_tmpD9 == 0){
{const char*_tmp8FF;Cyc_Parse_err(((_tmp8FF="missing argument variable in function prototype",_tag_dyneither(_tmp8FF,sizeof(char),48))),loc);}{
struct _tuple8*_tmp902;struct Cyc_List_List*_tmp901;_tmpD5=((_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901->hd=((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp902->f2=_tmpDA,((_tmp902->f3=_tmpDB,_tmp902)))))))),((_tmp901->tl=_tmpD5,_tmp901))))));};}else{
# 599
struct _tuple8*_tmp905;struct Cyc_List_List*_tmp904;_tmpD5=((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904->hd=((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905->f1=_tmpD9,((_tmp905->f2=_tmpDA,((_tmp905->f3=_tmpDB,_tmp905)))))))),((_tmp904->tl=_tmpD5,_tmp904))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp906;return(_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906->sc=sc,((_tmp906->name=d.id,((_tmp906->tvs=_tmpC8,((_tmp906->is_inline=is_inline,((_tmp906->effect=_tmpC9,((_tmp906->ret_tqual=_tmpCA,((_tmp906->ret_type=_tmpCB,((_tmp906->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD5),((_tmp906->c_varargs=_tmpCD,((_tmp906->cyc_varargs=_tmpCE,((_tmp906->rgn_po=_tmpCF,((_tmp906->body=body,((_tmp906->cached_typ=0,((_tmp906->param_vardecls=0,((_tmp906->fn_vardecl=0,((_tmp906->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpD0,_tmpC4),((_tmp906->requires_clause=_tmpD1,((_tmp906->requires_relns=0,((_tmp906->ensures_clause=_tmpD3,((_tmp906->ensures_relns=0,_tmp906)))))))))))))))))))))))))))))))))))))))));};}}else{_LL6C: _LL6D: {
# 618
const char*_tmp909;void*_tmp908;(_tmp908=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp909="declarator is not a function prototype",_tag_dyneither(_tmp909,sizeof(char),39))),_tag_dyneither(_tmp908,sizeof(void*),0)));}}_LL69:;};};};}static char _tmpE5[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 622
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpE5,_tmpE5,_tmpE5 + 76};static char _tmpE6[87]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 624
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpE6,_tmpE6,_tmpE6 + 87};static char _tmpE7[74]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 626
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpE7,_tmpE7,_tmpE7 + 74};static char _tmpE8[84]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 628
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpE8,_tmpE8,_tmpE8 + 84};
# 635
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 638
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp90A;(_tmp90A=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp90A,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp90B;(_tmp90B=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp90B,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp90C;(_tmp90C=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp90C,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp90D;(_tmp90D=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp90D,sizeof(void*),0)));}
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
const char*_tmp910;void*_tmp90F;(_tmp90F=0,Cyc_Tcutil_warn(loc,((_tmp910="missing type within specifier",_tag_dyneither(_tmp910,sizeof(char),30))),_tag_dyneither(_tmp90F,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 693
if(seen_sign){
void*_tmpEF=t;enum Cyc_Absyn_Sign _tmpF0;enum Cyc_Absyn_Size_of _tmpF1;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpEF)->tag == 6){_LL6F: _tmpF0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpEF)->f1;_tmpF1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpEF)->f2;_LL70:
# 696
 if(_tmpF0 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmpF1);
goto _LL6E;}else{_LL71: _LL72:
{const char*_tmp911;Cyc_Parse_err(((_tmp911="sign specification on non-integral type",_tag_dyneither(_tmp911,sizeof(char),40))),loc);}goto _LL6E;}_LL6E:;}
# 701
if(seen_size){
void*_tmpF3=t;enum Cyc_Absyn_Sign _tmpF4;enum Cyc_Absyn_Size_of _tmpF5;switch(*((int*)_tmpF3)){case 6: _LL74: _tmpF4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpF3)->f1;_tmpF5=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpF3)->f2;_LL75:
# 704
 if(_tmpF5 != sz)
t=Cyc_Absyn_int_typ(_tmpF4,sz);
goto _LL73;case 7: _LL76: _LL77:
# 708
 t=Cyc_Absyn_float_typ(2);goto _LL73;default: _LL78: _LL79:
{const char*_tmp912;Cyc_Parse_err(((_tmp912="size qualifier on non-integral type",_tag_dyneither(_tmp912,sizeof(char),36))),loc);}goto _LL73;}_LL73:;}}
# 712
return t;}
# 715
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 719
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpF7=d.id;
struct _tuple13 _tmpF8=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct Cyc_Absyn_Tqual _tmpFA;void*_tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFD;struct _tuple13 _tmpF9=_tmpF8;_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;_tmpFC=_tmpF9.f3;_tmpFD=_tmpF9.f4;
# 725
if(ds->tl == 0){
struct _tuple15*_tmp915;struct Cyc_List_List*_tmp914;return(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914->hd=((_tmp915=_region_malloc(r,sizeof(*_tmp915)),((_tmp915->f1=_tmpF7,((_tmp915->f2=_tmpFA,((_tmp915->f3=_tmpFB,((_tmp915->f4=_tmpFC,((_tmp915->f5=_tmpFD,_tmp915)))))))))))),((_tmp914->tl=0,_tmp914)))));}else{
# 728
struct _tuple15*_tmp918;struct Cyc_List_List*_tmp917;return(_tmp917=_region_malloc(r,sizeof(*_tmp917)),((_tmp917->hd=((_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=_tmpF7,((_tmp918->f2=_tmpFA,((_tmp918->f3=_tmpFB,((_tmp918->f4=_tmpFC,((_tmp918->f5=_tmpFD,_tmp918)))))))))))),((_tmp917->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp917)))));}};}
# 732
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 735
if(tms == 0){struct _tuple13 _tmp919;return(_tmp919.f1=tq,((_tmp919.f2=t,((_tmp919.f3=0,((_tmp919.f4=atts,_tmp919)))))));}{
void*_tmp103=(void*)tms->hd;void*_tmp104=_tmp103;unsigned int _tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_PtrAtts _tmp107;struct Cyc_Absyn_Tqual _tmp108;struct Cyc_List_List*_tmp109;unsigned int _tmp10A;void*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;union Cyc_Absyn_Constraint*_tmp10D;unsigned int _tmp10E;union Cyc_Absyn_Constraint*_tmp10F;unsigned int _tmp110;switch(*((int*)_tmp104)){case 0: _LL7B: _tmp10F=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp110=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL7C:
# 738
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp10F,_tmp110),atts,tms->tl);case 1: _LL7D: _tmp10C=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp10D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_tmp10E=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp104)->f3;_LL7E:
# 741
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp10C,_tmp10D,_tmp10E),atts,tms->tl);case 3: _LL7F: _tmp10B=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_LL80: {
# 744
void*_tmp111=_tmp10B;unsigned int _tmp112;struct Cyc_List_List*_tmp113;int _tmp114;struct Cyc_Absyn_VarargInfo*_tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->tag == 1){_LL88: _tmp113=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f1;_tmp114=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f2;_tmp115=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f3;_tmp116=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f4;_tmp117=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f5;_tmp118=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f6;_tmp119=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f7;_LL89: {
# 746
struct Cyc_List_List*typvars=0;
# 748
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp91A;fn_atts=((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->hd=(void*)as->hd,((_tmp91A->tl=fn_atts,_tmp91A))))));}else{
# 753
struct Cyc_List_List*_tmp91B;new_atts=((_tmp91B=_cycalloc(sizeof(*_tmp91B)),((_tmp91B->hd=(void*)as->hd,((_tmp91B->tl=new_atts,_tmp91B))))));}}}
# 756
if(tms->tl != 0){
void*_tmp11C=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp11D=_tmp11C;struct Cyc_List_List*_tmp11E;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11D)->tag == 4){_LL8D: _tmp11E=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11D)->f1;_LL8E:
# 759
 typvars=_tmp11E;
tms=tms->tl;
goto _LL8C;}else{_LL8F: _LL90:
# 763
 goto _LL8C;}_LL8C:;}
# 767
if(((((!_tmp114  && _tmp115 == 0) && _tmp113 != 0) && _tmp113->tl == 0) && (*((struct _tuple8*)_tmp113->hd)).f1 == 0) && (*((struct _tuple8*)_tmp113->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 772
_tmp113=0;{
# 775
struct Cyc_List_List*_tmp11F=Cyc_Parse_get_arg_tags(_tmp113);
# 777
if(_tmp11F != 0)
t=Cyc_Parse_substitute_tags(_tmp11F,t);
t=Cyc_Parse_array2ptr(t,0);
# 782
{struct Cyc_List_List*_tmp120=_tmp113;for(0;_tmp120 != 0;_tmp120=_tmp120->tl){
struct _tuple8*_tmp121=(struct _tuple8*)_tmp120->hd;struct _dyneither_ptr*_tmp123;struct Cyc_Absyn_Tqual _tmp124;void**_tmp125;struct _tuple8*_tmp122=_tmp121;_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;_tmp125=(void**)& _tmp122->f3;
if(_tmp11F != 0)
*_tmp125=Cyc_Parse_substitute_tags(_tmp11F,*_tmp125);
*_tmp125=Cyc_Parse_array2ptr(*_tmp125,1);}}
# 794
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp116,tq,t,_tmp113,_tmp114,_tmp115,_tmp117,fn_atts,_tmp118,_tmp119),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}}else{_LL8A: _tmp112=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp111)->f2;_LL8B: {
# 801
const char*_tmp91E;void*_tmp91D;(_tmp91D=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp112,((_tmp91E="function declaration without parameter types",_tag_dyneither(_tmp91E,sizeof(char),45))),_tag_dyneither(_tmp91D,sizeof(void*),0)));}}_LL87:;}case 4: _LL81: _tmp109=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp10A=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL82:
# 808
 if(tms->tl == 0){
struct _tuple13 _tmp91F;return(_tmp91F.f1=tq,((_tmp91F.f2=t,((_tmp91F.f3=_tmp109,((_tmp91F.f4=atts,_tmp91F)))))));}{
# 813
const char*_tmp922;void*_tmp921;(_tmp921=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp10A,((_tmp922="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp922,sizeof(char),68))),_tag_dyneither(_tmp921,sizeof(void*),0)));};case 2: _LL83: _tmp107=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp108=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL84: {
# 816
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp928;struct Cyc_Absyn_PtrInfo _tmp927;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp926;return Cyc_Parse_apply_tms(_tmp108,(void*)((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926[0]=((_tmp928.tag=5,((_tmp928.f1=((_tmp927.elt_typ=t,((_tmp927.elt_tq=tq,((_tmp927.ptr_atts=_tmp107,_tmp927)))))),_tmp928)))),_tmp926)))),atts,tms->tl);}default: _LL85: _tmp105=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp106=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL86:
# 821
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp106),tms->tl);}_LL7A:;};}
# 827
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 836
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp12F;struct Cyc_Absyn_Tqual _tmp130;void*_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_List_List*_tmp133;struct _tuple15*_tmp12E=t;_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;_tmp131=_tmp12E->f3;_tmp132=_tmp12E->f4;_tmp133=_tmp12E->f5;
# 839
Cyc_Lex_register_typedef(_tmp12F);{
# 841
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp134=_tmp131;struct Cyc_Core_Opt*_tmp135;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp134)->tag == 1){_LL92: _tmp135=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp134)->f1;_LL93:
# 845
 type=0;
if(_tmp135 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp135;}
goto _LL91;}else{_LL94: _LL95:
 kind=0;type=_tmp131;goto _LL91;}_LL91:;}{
# 851
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp92E;struct Cyc_Absyn_Typedefdecl*_tmp92D;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp92C;return Cyc_Absyn_new_decl((void*)((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92E.tag=8,((_tmp92E.f1=((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D->name=_tmp12F,((_tmp92D->tvs=_tmp132,((_tmp92D->kind=kind,((_tmp92D->defn=type,((_tmp92D->atts=_tmp133,((_tmp92D->tq=_tmp130,((_tmp92D->extern_c=0,_tmp92D)))))))))))))))),_tmp92E)))),_tmp92C)))),loc);};};}
# 858
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp931;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp930;return Cyc_Absyn_new_stmt((void*)((_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930[0]=((_tmp931.tag=12,((_tmp931.f1=d,((_tmp931.f2=s,_tmp931)))))),_tmp930)))),d->loc);}
# 864
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 873
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 877
struct _RegionHandle _tmp13B=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp13B;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp13D;struct Cyc_Parse_Type_specifier _tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_Parse_Declaration_spec _tmp13C=ds;_tmp13D=_tmp13C.tq;_tmp13E=_tmp13C.type_specs;_tmp13F=_tmp13C.attributes;
if(_tmp13D.loc == 0)_tmp13D.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp934;void*_tmp933;(_tmp933=0,Cyc_Tcutil_warn(loc,((_tmp934="inline qualifier on non-function definition",_tag_dyneither(_tmp934,sizeof(char),44))),_tag_dyneither(_tmp933,sizeof(void*),0)));}{
# 883
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp142=*ds.sc;enum Cyc_Parse_Storage_class _tmp143=_tmp142;switch(_tmp143){case Cyc_Parse_Typedef_sc: _LL97: _LL98:
 istypedef=1;goto _LL96;case Cyc_Parse_Extern_sc: _LL99: _LL9A:
 s=Cyc_Absyn_Extern;goto _LL96;case Cyc_Parse_ExternC_sc: _LL9B: _LL9C:
 s=Cyc_Absyn_ExternC;goto _LL96;case Cyc_Parse_Static_sc: _LL9D: _LL9E:
 s=Cyc_Absyn_Static;goto _LL96;case Cyc_Parse_Auto_sc: _LL9F: _LLA0:
 s=Cyc_Absyn_Public;goto _LL96;case Cyc_Parse_Register_sc: _LLA1: _LLA2:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}goto _LL96;default: _LLA3: _LLA4:
 s=Cyc_Absyn_Abstract;goto _LL96;}_LL96:;}{
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
void*_tmp144=Cyc_Parse_collapse_type_specifiers(_tmp13E,loc);
if(declarators == 0){
# 915
void*_tmp145=_tmp144;struct Cyc_List_List*_tmp146;struct _tuple0*_tmp147;struct _tuple0*_tmp148;int _tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_Absyn_Datatypedecl**_tmp14B;enum Cyc_Absyn_AggrKind _tmp14C;struct _tuple0*_tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Datatypedecl*_tmp14F;struct Cyc_Absyn_Enumdecl*_tmp150;struct Cyc_Absyn_Aggrdecl*_tmp151;switch(*((int*)_tmp145)){case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)){case 0: _LLA6: _tmp151=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)->f1;_LLA7:
# 917
 _tmp151->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp151->attributes,_tmp13F);
_tmp151->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp93A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp939;struct Cyc_List_List*_tmp938;struct Cyc_List_List*_tmp155=(_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938->hd=Cyc_Absyn_new_decl((void*)((_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A[0]=((_tmp939.tag=5,((_tmp939.f1=_tmp151,_tmp939)))),_tmp93A)))),loc),((_tmp938->tl=0,_tmp938)))));_npop_handler(0);return _tmp155;};case 1: _LLA8: _tmp150=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)->f1;_LLA9:
# 921
 if(_tmp13F != 0){const char*_tmp93B;Cyc_Parse_err(((_tmp93B="attributes on enum not supported",_tag_dyneither(_tmp93B,sizeof(char),33))),loc);}
_tmp150->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp941;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp940;struct Cyc_List_List*_tmp93F;struct Cyc_List_List*_tmp15A=(_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F->hd=Cyc_Absyn_new_decl((void*)((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941[0]=((_tmp940.tag=7,((_tmp940.f1=_tmp150,_tmp940)))),_tmp941)))),loc),((_tmp93F->tl=0,_tmp93F)))));_npop_handler(0);return _tmp15A;};default: _LLAA: _tmp14F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)->f1;_LLAB:
# 925
 if(_tmp13F != 0){const char*_tmp942;Cyc_Parse_err(((_tmp942="attributes on datatypes not supported",_tag_dyneither(_tmp942,sizeof(char),38))),loc);}
_tmp14F->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp948;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp947;struct Cyc_List_List*_tmp946;struct Cyc_List_List*_tmp15F=(_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946->hd=Cyc_Absyn_new_decl((void*)((_tmp948=_cycalloc(sizeof(*_tmp948)),((_tmp948[0]=((_tmp947.tag=6,((_tmp947.f1=_tmp14F,_tmp947)))),_tmp948)))),loc),((_tmp946->tl=0,_tmp946)))));_npop_handler(0);return _tmp15F;};}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).aggr_info).UnknownAggr).tag == 1){_LLAC: _tmp14C=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).aggr_info).UnknownAggr).val).f1;_tmp14D=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).aggr_info).UnknownAggr).val).f2;_tmp14E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).targs;_LLAD: {
# 929
struct Cyc_List_List*_tmp160=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp14E);
struct Cyc_Absyn_Aggrdecl*_tmp949;struct Cyc_Absyn_Aggrdecl*_tmp161=(_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949->kind=_tmp14C,((_tmp949->sc=s,((_tmp949->name=_tmp14D,((_tmp949->tvs=_tmp160,((_tmp949->impl=0,((_tmp949->attributes=0,((_tmp949->expected_mem_kind=0,_tmp949)))))))))))))));
if(_tmp13F != 0){const char*_tmp94A;Cyc_Parse_err(((_tmp94A="bad attributes on type declaration",_tag_dyneither(_tmp94A,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp950;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp94F;struct Cyc_List_List*_tmp94E;struct Cyc_List_List*_tmp166=(_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E->hd=Cyc_Absyn_new_decl((void*)((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp94F.tag=5,((_tmp94F.f1=_tmp161,_tmp94F)))),_tmp950)))),loc),((_tmp94E->tl=0,_tmp94E)))));_npop_handler(0);return _tmp166;};}}else{goto _LLB6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).KnownDatatype).tag == 2){_LLAE: _tmp14B=(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).KnownDatatype).val;_LLAF:
# 934
 if(_tmp13F != 0){const char*_tmp951;Cyc_Parse_err(((_tmp951="bad attributes on datatype",_tag_dyneither(_tmp951,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp957;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp956;struct Cyc_List_List*_tmp955;struct Cyc_List_List*_tmp16C=(_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955->hd=Cyc_Absyn_new_decl((void*)((_tmp957=_cycalloc(sizeof(*_tmp957)),((_tmp957[0]=((_tmp956.tag=6,((_tmp956.f1=*_tmp14B,_tmp956)))),_tmp957)))),loc),((_tmp955->tl=0,_tmp955)))));_npop_handler(0);return _tmp16C;};}else{_LLB0: _tmp148=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).UnknownDatatype).val).name;_tmp149=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp14A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).targs;_LLB1: {
# 937
struct Cyc_List_List*_tmp16D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp14A);
struct Cyc_Absyn_Decl*_tmp16E=Cyc_Absyn_datatype_decl(s,_tmp148,_tmp16D,0,_tmp149,loc);
if(_tmp13F != 0){const char*_tmp958;Cyc_Parse_err(((_tmp958="bad attributes on datatype",_tag_dyneither(_tmp958,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp959;struct Cyc_List_List*_tmp171=(_tmp959=_cycalloc(sizeof(*_tmp959)),((_tmp959->hd=_tmp16E,((_tmp959->tl=0,_tmp959)))));_npop_handler(0);return _tmp171;};}}case 13: _LLB2: _tmp147=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp145)->f1;_LLB3: {
# 942
struct Cyc_Absyn_Enumdecl*_tmp95A;struct Cyc_Absyn_Enumdecl*_tmp172=(_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A->sc=s,((_tmp95A->name=_tmp147,((_tmp95A->fields=0,_tmp95A)))))));
if(_tmp13F != 0){const char*_tmp95B;Cyc_Parse_err(((_tmp95B="bad attributes on enum",_tag_dyneither(_tmp95B,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp965;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp964;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp963;struct Cyc_List_List*_tmp962;struct Cyc_List_List*_tmp178=(_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962->hd=((_tmp965=_cycalloc(sizeof(*_tmp965)),((_tmp965->r=(void*)((_tmp963=_cycalloc(sizeof(*_tmp963)),((_tmp963[0]=((_tmp964.tag=7,((_tmp964.f1=_tmp172,_tmp964)))),_tmp963)))),((_tmp965->loc=loc,_tmp965)))))),((_tmp962->tl=0,_tmp962)))));_npop_handler(0);return _tmp178;};}case 14: _LLB4: _tmp146=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp145)->f1;_LLB5: {
# 948
struct Cyc_Core_Opt*_tmp968;struct Cyc_Absyn_Enumdecl*_tmp967;struct Cyc_Absyn_Enumdecl*_tmp17A=(_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967->sc=s,((_tmp967->name=Cyc_Parse_gensym_enum(),((_tmp967->fields=((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968->v=_tmp146,_tmp968)))),_tmp967)))))));
if(_tmp13F != 0){const char*_tmp969;Cyc_Parse_err(((_tmp969="bad attributes on enum",_tag_dyneither(_tmp969,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp973;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp972;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp971;struct Cyc_List_List*_tmp970;struct Cyc_List_List*_tmp180=(_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970->hd=((_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973->r=(void*)((_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=((_tmp972.tag=7,((_tmp972.f1=_tmp17A,_tmp972)))),_tmp971)))),((_tmp973->loc=loc,_tmp973)))))),((_tmp970->tl=0,_tmp970)))));_npop_handler(0);return _tmp180;};}default: _LLB6: _LLB7:
{const char*_tmp974;Cyc_Parse_err(((_tmp974="missing declarator",_tag_dyneither(_tmp974,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp184=0;_npop_handler(0);return _tmp184;};}_LLA5:;}else{
# 955
struct Cyc_List_List*_tmp185=Cyc_Parse_apply_tmss(mkrgn,_tmp13D,_tmp144,declarators,_tmp13F);
if(istypedef){
# 960
if(!exps_empty){
const char*_tmp975;Cyc_Parse_err(((_tmp975="initializer in typedef declaration",_tag_dyneither(_tmp975,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp185);
struct Cyc_List_List*_tmp187=decls;_npop_handler(0);return _tmp187;};}else{
# 966
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp188=_tmp185;for(0;_tmp188 != 0;(_tmp188=_tmp188->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp189=(struct _tuple15*)_tmp188->hd;struct _tuple0*_tmp18B;struct Cyc_Absyn_Tqual _tmp18C;void*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18F;struct _tuple15*_tmp18A=_tmp189;_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;_tmp18D=_tmp18A->f3;_tmp18E=_tmp18A->f4;_tmp18F=_tmp18A->f5;
if(_tmp18E != 0){
const char*_tmp978;void*_tmp977;(_tmp977=0,Cyc_Tcutil_warn(loc,((_tmp978="bad type params, ignoring",_tag_dyneither(_tmp978,sizeof(char),26))),_tag_dyneither(_tmp977,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp97B;void*_tmp97A;(_tmp97A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp97B="unexpected NULL in parse!",_tag_dyneither(_tmp97B,sizeof(char),26))),_tag_dyneither(_tmp97A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp194=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp195=Cyc_Absyn_new_vardecl(_tmp18B,_tmp18D,_tmp194);
_tmp195->tq=_tmp18C;
_tmp195->sc=s;
_tmp195->attributes=_tmp18F;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp981;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp980;struct Cyc_Absyn_Decl*_tmp97F;struct Cyc_Absyn_Decl*_tmp196=(_tmp97F=_cycalloc(sizeof(*_tmp97F)),((_tmp97F->r=(void*)((_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981[0]=((_tmp980.tag=0,((_tmp980.f1=_tmp195,_tmp980)))),_tmp981)))),((_tmp97F->loc=loc,_tmp97F)))));
struct Cyc_List_List*_tmp982;decls=((_tmp982=_cycalloc(sizeof(*_tmp982)),((_tmp982->hd=_tmp196,((_tmp982->tl=decls,_tmp982))))));};};}}{
# 981
struct Cyc_List_List*_tmp19B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp19B;};}}};};};};}
# 878
;_pop_region(mkrgn);}
# 988
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp19C=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp19D=_tmp19C;switch(_tmp19D){case 65: _LLB9: _LLBA:
 return& Cyc_Tcutil_ak;case 77: _LLBB: _LLBC:
 return& Cyc_Tcutil_mk;case 66: _LLBD: _LLBE:
 return& Cyc_Tcutil_bk;case 82: _LLBF: _LLC0:
 return& Cyc_Tcutil_rk;case 69: _LLC1: _LLC2:
 return& Cyc_Tcutil_ek;case 73: _LLC3: _LLC4:
 return& Cyc_Tcutil_ik;case 85: _LLC5: _LLC6:
# 998
{char _tmp19E=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp19F=_tmp19E;switch(_tmp19F){case 82: _LLCC: _LLCD:
 return& Cyc_Tcutil_urk;case 65: _LLCE: _LLCF:
 return& Cyc_Tcutil_uak;case 77: _LLD0: _LLD1:
 return& Cyc_Tcutil_umk;case 66: _LLD2: _LLD3:
 return& Cyc_Tcutil_ubk;default: _LLD4: _LLD5:
 goto _LLCB;}_LLCB:;}
# 1005
goto _LLB8;case 84: _LLC7: _LLC8:
# 1007
{char _tmp1A0=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp1A1=_tmp1A0;switch(_tmp1A1){case 82: _LLD7: _LLD8:
 return& Cyc_Tcutil_trk;case 65: _LLD9: _LLDA:
 return& Cyc_Tcutil_tak;case 77: _LLDB: _LLDC:
 return& Cyc_Tcutil_tmk;case 66: _LLDD: _LLDE:
 return& Cyc_Tcutil_tbk;default: _LLDF: _LLE0:
 goto _LLD6;}_LLD6:;}
# 1014
goto _LLB8;default: _LLC9: _LLCA:
 goto _LLB8;}_LLB8:;}
# 1017
{const char*_tmp987;void*_tmp986[2];struct Cyc_String_pa_PrintArg_struct _tmp985;struct Cyc_Int_pa_PrintArg_struct _tmp984;Cyc_Parse_err((struct _dyneither_ptr)((_tmp984.tag=1,((_tmp984.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp985.tag=0,((_tmp985.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp986[0]=& _tmp985,((_tmp986[1]=& _tmp984,Cyc_aprintf(((_tmp987="bad kind: %s; strlen=%d",_tag_dyneither(_tmp987,sizeof(char),24))),_tag_dyneither(_tmp986,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1022
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1A6=e->r;void*_tmp1A7=_tmp1A6;int _tmp1A8;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A7)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A7)->f1).Int_c).tag == 5){_LLE2: _tmp1A8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A7)->f1).Int_c).val).f2;_LLE3:
 return _tmp1A8;}else{goto _LLE4;}}else{_LLE4: _LLE5:
# 1026
{const char*_tmp988;Cyc_Parse_err(((_tmp988="expecting integer constant",_tag_dyneither(_tmp988,sizeof(char),27))),loc);}
return 0;}_LLE1:;}
# 1032
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1AA=e->r;void*_tmp1AB=_tmp1AA;struct _dyneither_ptr _tmp1AC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AB)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AB)->f1).String_c).tag == 8){_LLE7: _tmp1AC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AB)->f1).String_c).val;_LLE8:
 return _tmp1AC;}else{goto _LLE9;}}else{_LLE9: _LLEA:
# 1036
{const char*_tmp989;Cyc_Parse_err(((_tmp989="expecting string constant",_tag_dyneither(_tmp989,sizeof(char),26))),loc);}
return _tag_dyneither(0,0,0);}_LLE6:;}
# 1042
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1AE=x;long long _tmp1AF;char _tmp1B0;int _tmp1B1;switch((_tmp1AE.LongLong_c).tag){case 5: _LLEC: _tmp1B1=((_tmp1AE.Int_c).val).f2;_LLED:
 return(unsigned int)_tmp1B1;case 2: _LLEE: _tmp1B0=((_tmp1AE.Char_c).val).f2;_LLEF:
 return(unsigned int)_tmp1B0;case 6: _LLF0: _tmp1AF=((_tmp1AE.LongLong_c).val).f2;_LLF1: {
# 1047
unsigned long long y=(unsigned long long)_tmp1AF;
if(y > -1){
const char*_tmp98A;Cyc_Parse_err(((_tmp98A="integer constant too large",_tag_dyneither(_tmp98A,sizeof(char),27))),loc);}
return(unsigned int)_tmp1AF;}default: _LLF2: _LLF3:
# 1052
{const char*_tmp98E;void*_tmp98D[1];struct Cyc_String_pa_PrintArg_struct _tmp98C;Cyc_Parse_err((struct _dyneither_ptr)((_tmp98C.tag=0,((_tmp98C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x)),((_tmp98D[0]=& _tmp98C,Cyc_aprintf(((_tmp98E="expected integer constant but found %s",_tag_dyneither(_tmp98E,sizeof(char),39))),_tag_dyneither(_tmp98D,sizeof(void*),1)))))))),loc);}
return 0;}_LLEB:;}
# 1058
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1B6=p->r;void*_tmp1B7=_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B8;struct _tuple0*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct _dyneither_ptr _tmp1BB;int _tmp1BC;char _tmp1BD;enum Cyc_Absyn_Sign _tmp1BE;int _tmp1BF;struct Cyc_Absyn_Pat*_tmp1C0;struct Cyc_Absyn_Vardecl*_tmp1C1;struct _tuple0*_tmp1C2;switch(*((int*)_tmp1B7)){case 15: _LLF5: _tmp1C2=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LLF6:
 return Cyc_Absyn_unknownid_exp(_tmp1C2,p->loc);case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2)->r)->tag == 0){_LLF7: _tmp1C1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LLF8:
# 1062
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1C1->name,p->loc),p->loc);}else{goto _LL107;}case 6: _LLF9: _tmp1C0=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LLFA:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1C0),p->loc);case 9: _LLFB: _LLFC:
 return Cyc_Absyn_null_exp(p->loc);case 10: _LLFD: _tmp1BE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_tmp1BF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2;_LLFE:
 return Cyc_Absyn_int_exp(_tmp1BE,_tmp1BF,p->loc);case 11: _LLFF: _tmp1BD=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LL100:
 return Cyc_Absyn_char_exp(_tmp1BD,p->loc);case 12: _LL101: _tmp1BB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_tmp1BC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2;_LL102:
 return Cyc_Absyn_float_exp(_tmp1BB,_tmp1BC,p->loc);case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B7)->f3 == 0){_LL103: _tmp1B9=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_tmp1BA=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2;_LL104: {
# 1069
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1B9,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1BA);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL107;}case 17: _LL105: _tmp1B8=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LL106:
 return _tmp1B8;default: _LL107: _LL108:
# 1074
{const char*_tmp98F;Cyc_Parse_err(((_tmp98F="cannot mix patterns and expressions in case",_tag_dyneither(_tmp98F,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);}_LLF4:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1C5[7]="cnst_t";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 7}};
# 1133
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1C6=yy1;union Cyc_Absyn_Cnst _tmp1C7;if((_tmp1C6.Int_tok).tag == 1){_LL10A: _tmp1C7=(_tmp1C6.Int_tok).val;_LL10B:
# 1137
 yyzzz=_tmp1C7;
goto _LL109;}else{_LL10C: _LL10D:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL109:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _tmp990;return((_tmp990.Int_tok).val=yy1,(((_tmp990.Int_tok).tag=1,_tmp990)));}static char _tmp1CA[5]="char";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1CA,_tmp1CA,_tmp1CA + 5}};
# 1134
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1CB=yy1;char _tmp1CC;if((_tmp1CB.Char_tok).tag == 2){_LL10F: _tmp1CC=(_tmp1CB.Char_tok).val;_LL110:
# 1138
 yyzzz=_tmp1CC;
goto _LL10E;}else{_LL111: _LL112:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL10E:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp991;return((_tmp991.Char_tok).val=yy1,(((_tmp991.Char_tok).tag=2,_tmp991)));}static char _tmp1CF[13]="string_t<`H>";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1CF,_tmp1CF,_tmp1CF + 13}};
# 1135
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1D0=yy1;struct _dyneither_ptr _tmp1D1;if((_tmp1D0.String_tok).tag == 3){_LL114: _tmp1D1=(_tmp1D0.String_tok).val;_LL115:
# 1139
 yyzzz=_tmp1D1;
goto _LL113;}else{_LL116: _LL117:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL113:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp992;return((_tmp992.String_tok).val=yy1,(((_tmp992.String_tok).tag=3,_tmp992)));}static char _tmp1D4[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,_tmp1D4 + 56}};
# 1138
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1D5=yy1;struct _tuple21*_tmp1D6;if((_tmp1D5.YY1).tag == 9){_LL119: _tmp1D6=(_tmp1D5.YY1).val;_LL11A:
# 1142
 yyzzz=_tmp1D6;
goto _LL118;}else{_LL11B: _LL11C:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL118:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp993;return((_tmp993.YY1).val=yy1,(((_tmp993.YY1).tag=9,_tmp993)));}static char _tmp1D9[19]="conref_t<bounds_t>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1D9,_tmp1D9,_tmp1D9 + 19}};
# 1139
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1DA=yy1;union Cyc_Absyn_Constraint*_tmp1DB;if((_tmp1DA.YY2).tag == 10){_LL11E: _tmp1DB=(_tmp1DA.YY2).val;_LL11F:
# 1143
 yyzzz=_tmp1DB;
goto _LL11D;}else{_LL120: _LL121:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL11D:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp994;return((_tmp994.YY2).val=yy1,(((_tmp994.YY2).tag=10,_tmp994)));}static char _tmp1DE[28]="list_t<offsetof_field_t,`H>";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 28}};
# 1140
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1DF=yy1;struct Cyc_List_List*_tmp1E0;if((_tmp1DF.YY3).tag == 11){_LL123: _tmp1E0=(_tmp1DF.YY3).val;_LL124:
# 1144
 yyzzz=_tmp1E0;
goto _LL122;}else{_LL125: _LL126:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL122:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp995;return((_tmp995.YY3).val=yy1,(((_tmp995.YY3).tag=11,_tmp995)));}static char _tmp1E3[6]="exp_t";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 6}};
# 1141
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1E4=yy1;struct Cyc_Absyn_Exp*_tmp1E5;if((_tmp1E4.Exp_tok).tag == 7){_LL128: _tmp1E5=(_tmp1E4.Exp_tok).val;_LL129:
# 1145
 yyzzz=_tmp1E5;
goto _LL127;}else{_LL12A: _LL12B:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL127:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp996;return((_tmp996.Exp_tok).val=yy1,(((_tmp996.Exp_tok).tag=7,_tmp996)));}static char _tmp1E8[17]="list_t<exp_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,_tmp1E8 + 17}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E9=yy1;struct Cyc_List_List*_tmp1EA;if((_tmp1E9.YY4).tag == 12){_LL12D: _tmp1EA=(_tmp1E9.YY4).val;_LL12E:
# 1153
 yyzzz=_tmp1EA;
goto _LL12C;}else{_LL12F: _LL130:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL12C:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp997;return((_tmp997.YY4).val=yy1,(((_tmp997.YY4).tag=12,_tmp997)));}static char _tmp1ED[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 47}};
# 1150
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EE=yy1;struct Cyc_List_List*_tmp1EF;if((_tmp1EE.YY5).tag == 13){_LL132: _tmp1EF=(_tmp1EE.YY5).val;_LL133:
# 1154
 yyzzz=_tmp1EF;
goto _LL131;}else{_LL134: _LL135:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL131:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp998;return((_tmp998.YY5).val=yy1,(((_tmp998.YY5).tag=13,_tmp998)));}static char _tmp1F2[9]="primop_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 9}};
# 1151
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;enum Cyc_Absyn_Primop _tmp1F4;if((_tmp1F3.YY6).tag == 14){_LL137: _tmp1F4=(_tmp1F3.YY6).val;_LL138:
# 1155
 yyzzz=_tmp1F4;
goto _LL136;}else{_LL139: _LL13A:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL136:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp999;return((_tmp999.YY6).val=yy1,(((_tmp999.YY6).tag=14,_tmp999)));}static char _tmp1F7[19]="opt_t<primop_t,`H>";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 19}};
# 1152
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1F8=yy1;struct Cyc_Core_Opt*_tmp1F9;if((_tmp1F8.YY7).tag == 15){_LL13C: _tmp1F9=(_tmp1F8.YY7).val;_LL13D:
# 1156
 yyzzz=_tmp1F9;
goto _LL13B;}else{_LL13E: _LL13F:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL13B:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp99A;return((_tmp99A.YY7).val=yy1,(((_tmp99A.YY7).tag=15,_tmp99A)));}static char _tmp1FC[7]="qvar_t";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 7}};
# 1153
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1FD=yy1;struct _tuple0*_tmp1FE;if((_tmp1FD.QualId_tok).tag == 5){_LL141: _tmp1FE=(_tmp1FD.QualId_tok).val;_LL142:
# 1157
 yyzzz=_tmp1FE;
goto _LL140;}else{_LL143: _LL144:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL140:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.QualId_tok).val=yy1,(((_tmp99B.QualId_tok).tag=5,_tmp99B)));}static char _tmp201[7]="stmt_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 7}};
# 1156
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_Absyn_Stmt*_tmp203;if((_tmp202.Stmt_tok).tag == 8){_LL146: _tmp203=(_tmp202.Stmt_tok).val;_LL147:
# 1160
 yyzzz=_tmp203;
goto _LL145;}else{_LL148: _LL149:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL145:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.Stmt_tok).val=yy1,(((_tmp99C.Stmt_tok).tag=8,_tmp99C)));}static char _tmp206[27]="list_t<switch_clause_t,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 27}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp207=yy1;struct Cyc_List_List*_tmp208;if((_tmp207.YY8).tag == 16){_LL14B: _tmp208=(_tmp207.YY8).val;_LL14C:
# 1164
 yyzzz=_tmp208;
goto _LL14A;}else{_LL14D: _LL14E:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL14A:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.YY8).val=yy1,(((_tmp99D.YY8).tag=16,_tmp99D)));}static char _tmp20B[6]="pat_t";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 6}};
# 1161
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct Cyc_Absyn_Pat*_tmp20D;if((_tmp20C.YY9).tag == 17){_LL150: _tmp20D=(_tmp20C.YY9).val;_LL151:
# 1165
 yyzzz=_tmp20D;
goto _LL14F;}else{_LL152: _LL153:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL14F:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY9).val=yy1,(((_tmp99E.YY9).tag=17,_tmp99E)));}static char _tmp210[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 28}};
# 1166
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct _tuple22*_tmp212;if((_tmp211.YY10).tag == 18){_LL155: _tmp212=(_tmp211.YY10).val;_LL156:
# 1170
 yyzzz=_tmp212;
goto _LL154;}else{_LL157: _LL158:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL154:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.YY10).val=yy1,(((_tmp99F.YY10).tag=18,_tmp99F)));}static char _tmp215[17]="list_t<pat_t,`H>";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp215,_tmp215,_tmp215 + 17}};
# 1167
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp216=yy1;struct Cyc_List_List*_tmp217;if((_tmp216.YY11).tag == 19){_LL15A: _tmp217=(_tmp216.YY11).val;_LL15B:
# 1171
 yyzzz=_tmp217;
goto _LL159;}else{_LL15C: _LL15D:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL159:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY11).val=yy1,(((_tmp9A0.YY11).tag=19,_tmp9A0)));}static char _tmp21A[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 36}};
# 1168
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;struct _tuple23*_tmp21C;if((_tmp21B.YY12).tag == 20){_LL15F: _tmp21C=(_tmp21B.YY12).val;_LL160:
# 1172
 yyzzz=_tmp21C;
goto _LL15E;}else{_LL161: _LL162:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL15E:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY12).val=yy1,(((_tmp9A1.YY12).tag=20,_tmp9A1)));}static char _tmp21F[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 47}};
# 1169
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_List_List*_tmp221;if((_tmp220.YY13).tag == 21){_LL164: _tmp221=(_tmp220.YY13).val;_LL165:
# 1173
 yyzzz=_tmp221;
goto _LL163;}else{_LL166: _LL167:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL163:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY13).val=yy1,(((_tmp9A2.YY13).tag=21,_tmp9A2)));}static char _tmp224[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 58}};
# 1170
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp225=yy1;struct _tuple22*_tmp226;if((_tmp225.YY14).tag == 22){_LL169: _tmp226=(_tmp225.YY14).val;_LL16A:
# 1174
 yyzzz=_tmp226;
goto _LL168;}else{_LL16B: _LL16C:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL168:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.YY14).val=yy1,(((_tmp9A3.YY14).tag=22,_tmp9A3)));}static char _tmp229[9]="fndecl_t";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 9}};
# 1171
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_Absyn_Fndecl*_tmp22B;if((_tmp22A.YY15).tag == 23){_LL16E: _tmp22B=(_tmp22A.YY15).val;_LL16F:
# 1175
 yyzzz=_tmp22B;
goto _LL16D;}else{_LL170: _LL171:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL16D:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9A4;return((_tmp9A4.YY15).val=yy1,(((_tmp9A4.YY15).tag=23,_tmp9A4)));}static char _tmp22E[18]="list_t<decl_t,`H>";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 18}};
# 1172
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;struct Cyc_List_List*_tmp230;if((_tmp22F.YY16).tag == 24){_LL173: _tmp230=(_tmp22F.YY16).val;_LL174:
# 1176
 yyzzz=_tmp230;
goto _LL172;}else{_LL175: _LL176:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL172:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A5;return((_tmp9A5.YY16).val=yy1,(((_tmp9A5.YY16).tag=24,_tmp9A5)));}static char _tmp233[12]="decl_spec_t";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 12}};
# 1174
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Parse_Declaration_spec _tmp235;if((_tmp234.YY17).tag == 25){_LL178: _tmp235=(_tmp234.YY17).val;_LL179:
# 1178
 yyzzz=_tmp235;
goto _LL177;}else{_LL17A: _LL17B:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL177:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9A6;return((_tmp9A6.YY17).val=yy1,(((_tmp9A6.YY17).tag=25,_tmp9A6)));}static char _tmp238[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 31}};
# 1175
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple11 _tmp23A;if((_tmp239.YY18).tag == 26){_LL17D: _tmp23A=(_tmp239.YY18).val;_LL17E:
# 1179
 yyzzz=_tmp23A;
goto _LL17C;}else{_LL17F: _LL180:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL17C:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9A7;return((_tmp9A7.YY18).val=yy1,(((_tmp9A7.YY18).tag=26,_tmp9A7)));}static char _tmp23D[23]="declarator_list_t<`yy>";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 23}};
# 1176
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct _tuple12*_tmp23F;if((_tmp23E.YY19).tag == 27){_LL182: _tmp23F=(_tmp23E.YY19).val;_LL183:
# 1180
 yyzzz=_tmp23F;
goto _LL181;}else{_LL184: _LL185:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL181:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9A8;return((_tmp9A8.YY19).val=yy1,(((_tmp9A8.YY19).tag=27,_tmp9A8)));}static char _tmp242[19]="storage_class_t@`H";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 19}};
# 1177
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp243=yy1;enum Cyc_Parse_Storage_class*_tmp244;if((_tmp243.YY20).tag == 28){_LL187: _tmp244=(_tmp243.YY20).val;_LL188:
# 1181
 yyzzz=_tmp244;
goto _LL186;}else{_LL189: _LL18A:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL186:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9A9;return((_tmp9A9.YY20).val=yy1,(((_tmp9A9.YY20).tag=28,_tmp9A9)));}static char _tmp247[17]="type_specifier_t";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 17}};
# 1178
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_Parse_Type_specifier _tmp249;if((_tmp248.YY21).tag == 29){_LL18C: _tmp249=(_tmp248.YY21).val;_LL18D:
# 1182
 yyzzz=_tmp249;
goto _LL18B;}else{_LL18E: _LL18F:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL18B:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9AA;return((_tmp9AA.YY21).val=yy1,(((_tmp9AA.YY21).tag=29,_tmp9AA)));}static char _tmp24C[12]="aggr_kind_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 12}};
# 1180
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;enum Cyc_Absyn_AggrKind _tmp24E;if((_tmp24D.YY22).tag == 30){_LL191: _tmp24E=(_tmp24D.YY22).val;_LL192:
# 1184
 yyzzz=_tmp24E;
goto _LL190;}else{_LL193: _LL194:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL190:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9AB;return((_tmp9AB.YY22).val=yy1,(((_tmp9AB.YY22).tag=30,_tmp9AB)));}static char _tmp251[8]="tqual_t";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 8}};
# 1181
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_Absyn_Tqual _tmp253;if((_tmp252.YY23).tag == 31){_LL196: _tmp253=(_tmp252.YY23).val;_LL197:
# 1185
 yyzzz=_tmp253;
goto _LL195;}else{_LL198: _LL199:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL195:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9AC;return((_tmp9AC.YY23).val=yy1,(((_tmp9AC.YY23).tag=31,_tmp9AC)));}static char _tmp256[23]="list_t<aggrfield_t,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 23}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct Cyc_List_List*_tmp258;if((_tmp257.YY24).tag == 32){_LL19B: _tmp258=(_tmp257.YY24).val;_LL19C:
# 1186
 yyzzz=_tmp258;
goto _LL19A;}else{_LL19D: _LL19E:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL19A:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AD;return((_tmp9AD.YY24).val=yy1,(((_tmp9AD.YY24).tag=32,_tmp9AD)));}static char _tmp25B[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 34}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_List_List*_tmp25D;if((_tmp25C.YY25).tag == 33){_LL1A0: _tmp25D=(_tmp25C.YY25).val;_LL1A1:
# 1187
 yyzzz=_tmp25D;
goto _LL19F;}else{_LL1A2: _LL1A3:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL19F:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AE;return((_tmp9AE.YY25).val=yy1,(((_tmp9AE.YY25).tag=33,_tmp9AE)));}static char _tmp260[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp260,_tmp260,_tmp260 + 33}};
# 1184
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp261=yy1;struct Cyc_List_List*_tmp262;if((_tmp261.YY26).tag == 34){_LL1A5: _tmp262=(_tmp261.YY26).val;_LL1A6:
# 1188
 yyzzz=_tmp262;
goto _LL1A4;}else{_LL1A7: _LL1A8:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL1A4:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AF;return((_tmp9AF.YY26).val=yy1,(((_tmp9AF.YY26).tag=34,_tmp9AF)));}static char _tmp265[18]="declarator_t<`yy>";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 18}};
# 1185
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp266=yy1;struct Cyc_Parse_Declarator _tmp267;if((_tmp266.YY27).tag == 35){_LL1AA: _tmp267=(_tmp266.YY27).val;_LL1AB:
# 1189
 yyzzz=_tmp267;
goto _LL1A9;}else{_LL1AC: _LL1AD:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL1A9:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9B0;return((_tmp9B0.YY27).val=yy1,(((_tmp9B0.YY27).tag=35,_tmp9B0)));}static char _tmp26A[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 45}};
# 1186
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp26B=yy1;struct _tuple24*_tmp26C;if((_tmp26B.YY28).tag == 36){_LL1AF: _tmp26C=(_tmp26B.YY28).val;_LL1B0:
# 1190
 yyzzz=_tmp26C;
goto _LL1AE;}else{_LL1B1: _LL1B2:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL1AE:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9B1;return((_tmp9B1.YY28).val=yy1,(((_tmp9B1.YY28).tag=36,_tmp9B1)));}static char _tmp26F[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 57}};
# 1187
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp270=yy1;struct Cyc_List_List*_tmp271;if((_tmp270.YY29).tag == 37){_LL1B4: _tmp271=(_tmp270.YY29).val;_LL1B5:
# 1191
 yyzzz=_tmp271;
goto _LL1B3;}else{_LL1B6: _LL1B7:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL1B3:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B2;return((_tmp9B2.YY29).val=yy1,(((_tmp9B2.YY29).tag=37,_tmp9B2)));}static char _tmp274[26]="abstractdeclarator_t<`yy>";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 26}};
# 1188
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp275=yy1;struct Cyc_Parse_Abstractdeclarator _tmp276;if((_tmp275.YY30).tag == 38){_LL1B9: _tmp276=(_tmp275.YY30).val;_LL1BA:
# 1192
 yyzzz=_tmp276;
goto _LL1B8;}else{_LL1BB: _LL1BC:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL1B8:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9B3;return((_tmp9B3.YY30).val=yy1,(((_tmp9B3.YY30).tag=38,_tmp9B3)));}static char _tmp279[5]="bool";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 5}};
# 1189
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp27A=yy1;int _tmp27B;if((_tmp27A.YY31).tag == 39){_LL1BE: _tmp27B=(_tmp27A.YY31).val;_LL1BF:
# 1193
 yyzzz=_tmp27B;
goto _LL1BD;}else{_LL1C0: _LL1C1:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL1BD:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9B4;return((_tmp9B4.YY31).val=yy1,(((_tmp9B4.YY31).tag=39,_tmp9B4)));}static char _tmp27E[8]="scope_t";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 8}};
# 1190
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp27F=yy1;enum Cyc_Absyn_Scope _tmp280;if((_tmp27F.YY32).tag == 40){_LL1C3: _tmp280=(_tmp27F.YY32).val;_LL1C4:
# 1194
 yyzzz=_tmp280;
goto _LL1C2;}else{_LL1C5: _LL1C6:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL1C2:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9B5;return((_tmp9B5.YY32).val=yy1,(((_tmp9B5.YY32).tag=40,_tmp9B5)));}static char _tmp283[16]="datatypefield_t";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp283,_tmp283,_tmp283 + 16}};
# 1191
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_Absyn_Datatypefield*_tmp285;if((_tmp284.YY33).tag == 41){_LL1C8: _tmp285=(_tmp284.YY33).val;_LL1C9:
# 1195
 yyzzz=_tmp285;
goto _LL1C7;}else{_LL1CA: _LL1CB:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL1C7:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9B6;return((_tmp9B6.YY33).val=yy1,(((_tmp9B6.YY33).tag=41,_tmp9B6)));}static char _tmp288[27]="list_t<datatypefield_t,`H>";
# 1190 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 27}};
# 1192
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp289=yy1;struct Cyc_List_List*_tmp28A;if((_tmp289.YY34).tag == 42){_LL1CD: _tmp28A=(_tmp289.YY34).val;_LL1CE:
# 1196
 yyzzz=_tmp28A;
goto _LL1CC;}else{_LL1CF: _LL1D0:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL1CC:;}
# 1200
return yyzzz;}
# 1202
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B7;return((_tmp9B7.YY34).val=yy1,(((_tmp9B7.YY34).tag=42,_tmp9B7)));}static char _tmp28D[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1191 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 41}};
# 1193
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp28E=yy1;struct _tuple25 _tmp28F;if((_tmp28E.YY35).tag == 43){_LL1D2: _tmp28F=(_tmp28E.YY35).val;_LL1D3:
# 1197
 yyzzz=_tmp28F;
goto _LL1D1;}else{_LL1D4: _LL1D5:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL1D1:;}
# 1201
return yyzzz;}
# 1203
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9B8;return((_tmp9B8.YY35).val=yy1,(((_tmp9B8.YY35).tag=43,_tmp9B8)));}static char _tmp292[17]="list_t<var_t,`H>";
# 1192 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 17}};
# 1194
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp293=yy1;struct Cyc_List_List*_tmp294;if((_tmp293.YY36).tag == 44){_LL1D7: _tmp294=(_tmp293.YY36).val;_LL1D8:
# 1198
 yyzzz=_tmp294;
goto _LL1D6;}else{_LL1D9: _LL1DA:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL1D6:;}
# 1202
return yyzzz;}
# 1204
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B9;return((_tmp9B9.YY36).val=yy1,(((_tmp9B9.YY36).tag=44,_tmp9B9)));}static char _tmp297[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1193 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp297,_tmp297,_tmp297 + 31}};
# 1195
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp298=yy1;struct _tuple8*_tmp299;if((_tmp298.YY37).tag == 45){_LL1DC: _tmp299=(_tmp298.YY37).val;_LL1DD:
# 1199
 yyzzz=_tmp299;
goto _LL1DB;}else{_LL1DE: _LL1DF:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL1DB:;}
# 1203
return yyzzz;}
# 1205
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.YY37).val=yy1,(((_tmp9BA.YY37).tag=45,_tmp9BA)));}static char _tmp29C[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1194 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 42}};
# 1196
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29D=yy1;struct Cyc_List_List*_tmp29E;if((_tmp29D.YY38).tag == 46){_LL1E1: _tmp29E=(_tmp29D.YY38).val;_LL1E2:
# 1200
 yyzzz=_tmp29E;
goto _LL1E0;}else{_LL1E3: _LL1E4:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL1E0:;}
# 1204
return yyzzz;}
# 1206
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.YY38).val=yy1,(((_tmp9BB.YY38).tag=46,_tmp9BB)));}static char _tmp2A1[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1195 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 115}};
# 1197
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2A2=yy1;struct _tuple26*_tmp2A3;if((_tmp2A2.YY39).tag == 47){_LL1E6: _tmp2A3=(_tmp2A2.YY39).val;_LL1E7:
# 1201
 yyzzz=_tmp2A3;
goto _LL1E5;}else{_LL1E8: _LL1E9:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL1E5:;}
# 1205
return yyzzz;}
# 1207
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY39).val=yy1,(((_tmp9BC.YY39).tag=47,_tmp9BC)));}static char _tmp2A6[18]="list_t<type_t,`H>";
# 1196 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 18}};
# 1198
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A7=yy1;struct Cyc_List_List*_tmp2A8;if((_tmp2A7.YY40).tag == 48){_LL1EB: _tmp2A8=(_tmp2A7.YY40).val;_LL1EC:
# 1202
 yyzzz=_tmp2A8;
goto _LL1EA;}else{_LL1ED: _LL1EE:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL1EA:;}
# 1206
return yyzzz;}
# 1208
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY40).val=yy1,(((_tmp9BD.YY40).tag=48,_tmp9BD)));}static char _tmp2AB[24]="list_t<designator_t,`H>";
# 1198 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 24}};
# 1200
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_List_List*_tmp2AD;if((_tmp2AC.YY41).tag == 49){_LL1F0: _tmp2AD=(_tmp2AC.YY41).val;_LL1F1:
# 1204
 yyzzz=_tmp2AD;
goto _LL1EF;}else{_LL1F2: _LL1F3:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL1EF:;}
# 1208
return yyzzz;}
# 1210
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY41).val=yy1,(((_tmp9BE.YY41).tag=49,_tmp9BE)));}static char _tmp2B0[13]="designator_t";
# 1199 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 13}};
# 1201
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2B1=yy1;void*_tmp2B2;if((_tmp2B1.YY42).tag == 50){_LL1F5: _tmp2B2=(_tmp2B1.YY42).val;_LL1F6:
# 1205
 yyzzz=_tmp2B2;
goto _LL1F4;}else{_LL1F7: _LL1F8:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL1F4:;}
# 1209
return yyzzz;}
# 1211
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.YY42).val=yy1,(((_tmp9BF.YY42).tag=50,_tmp9BF)));}static char _tmp2B5[7]="kind_t";
# 1200 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2B5,_tmp2B5,_tmp2B5 + 7}};
# 1202
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2B6=yy1;struct Cyc_Absyn_Kind*_tmp2B7;if((_tmp2B6.YY43).tag == 51){_LL1FA: _tmp2B7=(_tmp2B6.YY43).val;_LL1FB:
# 1206
 yyzzz=_tmp2B7;
goto _LL1F9;}else{_LL1FC: _LL1FD:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL1F9:;}
# 1210
return yyzzz;}
# 1212
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY43).val=yy1,(((_tmp9C0.YY43).tag=51,_tmp9C0)));}static char _tmp2BA[7]="type_t";
# 1201 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 7}};
# 1203
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2BB=yy1;void*_tmp2BC;if((_tmp2BB.YY44).tag == 52){_LL1FF: _tmp2BC=(_tmp2BB.YY44).val;_LL200:
# 1207
 yyzzz=_tmp2BC;
goto _LL1FE;}else{_LL201: _LL202:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL1FE:;}
# 1211
return yyzzz;}
# 1213
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY44).val=yy1,(((_tmp9C1.YY44).tag=52,_tmp9C1)));}static char _tmp2BF[23]="list_t<attribute_t,`H>";
# 1202 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 23}};
# 1204
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C0=yy1;struct Cyc_List_List*_tmp2C1;if((_tmp2C0.YY45).tag == 53){_LL204: _tmp2C1=(_tmp2C0.YY45).val;_LL205:
# 1208
 yyzzz=_tmp2C1;
goto _LL203;}else{_LL206: _LL207:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL203:;}
# 1212
return yyzzz;}
# 1214
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY45).val=yy1,(((_tmp9C2.YY45).tag=53,_tmp9C2)));}static char _tmp2C4[12]="attribute_t";
# 1203 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 12}};
# 1205
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C5=yy1;void*_tmp2C6;if((_tmp2C5.YY46).tag == 54){_LL209: _tmp2C6=(_tmp2C5.YY46).val;_LL20A:
# 1209
 yyzzz=_tmp2C6;
goto _LL208;}else{_LL20B: _LL20C:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL208:;}
# 1213
return yyzzz;}
# 1215
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY46).val=yy1,(((_tmp9C3.YY46).tag=54,_tmp9C3)));}static char _tmp2C9[12]="enumfield_t";
# 1204 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 12}};
# 1206
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2CA=yy1;struct Cyc_Absyn_Enumfield*_tmp2CB;if((_tmp2CA.YY47).tag == 55){_LL20E: _tmp2CB=(_tmp2CA.YY47).val;_LL20F:
# 1210
 yyzzz=_tmp2CB;
goto _LL20D;}else{_LL210: _LL211:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL20D:;}
# 1214
return yyzzz;}
# 1216
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.YY47).val=yy1,(((_tmp9C4.YY47).tag=55,_tmp9C4)));}static char _tmp2CE[23]="list_t<enumfield_t,`H>";
# 1205 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 23}};
# 1207
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_List_List*_tmp2D0;if((_tmp2CF.YY48).tag == 56){_LL213: _tmp2D0=(_tmp2CF.YY48).val;_LL214:
# 1211
 yyzzz=_tmp2D0;
goto _LL212;}else{_LL215: _LL216:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL212:;}
# 1215
return yyzzz;}
# 1217
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY48).val=yy1,(((_tmp9C5.YY48).tag=56,_tmp9C5)));}static char _tmp2D3[11]="type_opt_t";
# 1206 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 11}};
# 1208
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D4=yy1;void*_tmp2D5;if((_tmp2D4.YY49).tag == 57){_LL218: _tmp2D5=(_tmp2D4.YY49).val;_LL219:
# 1212
 yyzzz=_tmp2D5;
goto _LL217;}else{_LL21A: _LL21B:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL217:;}
# 1216
return yyzzz;}
# 1218
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY49).val=yy1,(((_tmp9C6.YY49).tag=57,_tmp9C6)));}static char _tmp2D8[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1207 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2D8,_tmp2D8,_tmp2D8 + 31}};
# 1209
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D9=yy1;struct Cyc_List_List*_tmp2DA;if((_tmp2D9.YY50).tag == 58){_LL21D: _tmp2DA=(_tmp2D9.YY50).val;_LL21E:
# 1213
 yyzzz=_tmp2DA;
goto _LL21C;}else{_LL21F: _LL220:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL21C:;}
# 1217
return yyzzz;}
# 1219
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY50).val=yy1,(((_tmp9C7.YY50).tag=58,_tmp9C7)));}static char _tmp2DD[15]="conref_t<bool>";
# 1208 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 15}};
# 1210
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2DE=yy1;union Cyc_Absyn_Constraint*_tmp2DF;if((_tmp2DE.YY51).tag == 59){_LL222: _tmp2DF=(_tmp2DE.YY51).val;_LL223:
# 1214
 yyzzz=_tmp2DF;
goto _LL221;}else{_LL224: _LL225:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL221:;}
# 1218
return yyzzz;}
# 1220
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY51).val=yy1,(((_tmp9C8.YY51).tag=59,_tmp9C8)));}static char _tmp2E2[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1209 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 45}};
# 1211
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E3=yy1;struct Cyc_List_List*_tmp2E4;if((_tmp2E3.YY52).tag == 60){_LL227: _tmp2E4=(_tmp2E3.YY52).val;_LL228:
# 1215
 yyzzz=_tmp2E4;
goto _LL226;}else{_LL229: _LL22A:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL226:;}
# 1219
return yyzzz;}
# 1221
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY52).val=yy1,(((_tmp9C9.YY52).tag=60,_tmp9C9)));}static char _tmp2E7[20]="pointer_qual_t<`yy>";
# 1210 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 20}};
# 1212
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E8=yy1;void*_tmp2E9;if((_tmp2E8.YY53).tag == 61){_LL22C: _tmp2E9=(_tmp2E8.YY53).val;_LL22D:
# 1216
 yyzzz=_tmp2E9;
goto _LL22B;}else{_LL22E: _LL22F:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL22B:;}
# 1220
return yyzzz;}
# 1222
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY53).val=yy1,(((_tmp9CA.YY53).tag=61,_tmp9CA)));}static char _tmp2EC[21]="pointer_quals_t<`yy>";
# 1211 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 21}};
# 1213
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2ED=yy1;struct Cyc_List_List*_tmp2EE;if((_tmp2ED.YY54).tag == 62){_LL231: _tmp2EE=(_tmp2ED.YY54).val;_LL232:
# 1217
 yyzzz=_tmp2EE;
goto _LL230;}else{_LL233: _LL234:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL230:;}
# 1221
return yyzzz;}
# 1223
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY54).val=yy1,(((_tmp9CB.YY54).tag=62,_tmp9CB)));}static char _tmp2F1[21]="$(bool,string_t<`H>)";
# 1212 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 21}};
# 1214
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp2F2=yy1;struct _tuple20 _tmp2F3;if((_tmp2F2.Asm_tok).tag == 6){_LL236: _tmp2F3=(_tmp2F2.Asm_tok).val;_LL237:
# 1218
 yyzzz=_tmp2F3;
goto _LL235;}else{_LL238: _LL239:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL235:;}
# 1222
return yyzzz;}
# 1224
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.Asm_tok).val=yy1,(((_tmp9CC.Asm_tok).tag=6,_tmp9CC)));}static char _tmp2F6[10]="exp_opt_t";
# 1213 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,_tmp2F6 + 10}};
# 1215
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp2F7=yy1;struct Cyc_Absyn_Exp*_tmp2F8;if((_tmp2F7.YY55).tag == 63){_LL23B: _tmp2F8=(_tmp2F7.YY55).val;_LL23C:
# 1219
 yyzzz=_tmp2F8;
goto _LL23A;}else{_LL23D: _LL23E:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL23A:;}
# 1223
return yyzzz;}
# 1225
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY55).val=yy1,(((_tmp9CD.YY55).tag=63,_tmp9CD)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1237
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1241
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9CE;return(_tmp9CE.timestamp=0,((_tmp9CE.first_line=0,((_tmp9CE.first_column=0,((_tmp9CE.last_line=0,((_tmp9CE.last_column=0,_tmp9CE)))))))));}
# 1244
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1255 "parse.y"
static short Cyc_yytranslate[376]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,147,2,2,131,145,142,2,128,129,136,139,124,143,133,144,2,2,2,2,2,2,2,2,2,2,132,121,126,125,127,138,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,134,2,135,141,130,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,122,140,123,146,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};static char _tmp2FB[2]="$";static char _tmp2FC[6]="error";static char _tmp2FD[12]="$undefined.";static char _tmp2FE[5]="AUTO";static char _tmp2FF[9]="REGISTER";static char _tmp300[7]="STATIC";static char _tmp301[7]="EXTERN";static char _tmp302[8]="TYPEDEF";static char _tmp303[5]="VOID";static char _tmp304[5]="CHAR";static char _tmp305[6]="SHORT";static char _tmp306[4]="INT";static char _tmp307[5]="LONG";static char _tmp308[6]="FLOAT";static char _tmp309[7]="DOUBLE";static char _tmp30A[7]="SIGNED";static char _tmp30B[9]="UNSIGNED";static char _tmp30C[6]="CONST";static char _tmp30D[9]="VOLATILE";static char _tmp30E[9]="RESTRICT";static char _tmp30F[7]="STRUCT";static char _tmp310[6]="UNION";static char _tmp311[5]="CASE";static char _tmp312[8]="DEFAULT";static char _tmp313[7]="INLINE";static char _tmp314[7]="SIZEOF";static char _tmp315[9]="OFFSETOF";static char _tmp316[3]="IF";static char _tmp317[5]="ELSE";static char _tmp318[7]="SWITCH";static char _tmp319[6]="WHILE";static char _tmp31A[3]="DO";static char _tmp31B[4]="FOR";static char _tmp31C[5]="GOTO";static char _tmp31D[9]="CONTINUE";static char _tmp31E[6]="BREAK";static char _tmp31F[7]="RETURN";static char _tmp320[5]="ENUM";static char _tmp321[7]="TYPEOF";static char _tmp322[16]="BUILTIN_VA_LIST";static char _tmp323[8]="NULL_kw";static char _tmp324[4]="LET";static char _tmp325[6]="THROW";static char _tmp326[4]="TRY";static char _tmp327[6]="CATCH";static char _tmp328[7]="EXPORT";static char _tmp329[4]="NEW";static char _tmp32A[9]="ABSTRACT";static char _tmp32B[9]="FALLTHRU";static char _tmp32C[6]="USING";static char _tmp32D[10]="NAMESPACE";static char _tmp32E[9]="DATATYPE";static char _tmp32F[7]="MALLOC";static char _tmp330[8]="RMALLOC";static char _tmp331[15]="RMALLOC_INLINE";static char _tmp332[7]="CALLOC";static char _tmp333[8]="RCALLOC";static char _tmp334[5]="SWAP";static char _tmp335[9]="REGION_T";static char _tmp336[6]="TAG_T";static char _tmp337[7]="REGION";static char _tmp338[5]="RNEW";static char _tmp339[8]="REGIONS";static char _tmp33A[13]="RESET_REGION";static char _tmp33B[7]="PORTON";static char _tmp33C[8]="PORTOFF";static char _tmp33D[12]="DYNREGION_T";static char _tmp33E[8]="NUMELTS";static char _tmp33F[8]="VALUEOF";static char _tmp340[10]="VALUEOF_T";static char _tmp341[9]="TAGCHECK";static char _tmp342[13]="NUMELTS_QUAL";static char _tmp343[10]="THIN_QUAL";static char _tmp344[9]="FAT_QUAL";static char _tmp345[13]="NOTNULL_QUAL";static char _tmp346[14]="NULLABLE_QUAL";static char _tmp347[14]="REQUIRES_QUAL";static char _tmp348[13]="ENSURES_QUAL";static char _tmp349[12]="REGION_QUAL";static char _tmp34A[16]="NOZEROTERM_QUAL";static char _tmp34B[14]="ZEROTERM_QUAL";static char _tmp34C[12]="TAGGED_QUAL";static char _tmp34D[16]="EXTENSIBLE_QUAL";static char _tmp34E[15]="RESETABLE_QUAL";static char _tmp34F[7]="PTR_OP";static char _tmp350[7]="INC_OP";static char _tmp351[7]="DEC_OP";static char _tmp352[8]="LEFT_OP";static char _tmp353[9]="RIGHT_OP";static char _tmp354[6]="LE_OP";static char _tmp355[6]="GE_OP";static char _tmp356[6]="EQ_OP";static char _tmp357[6]="NE_OP";static char _tmp358[7]="AND_OP";static char _tmp359[6]="OR_OP";static char _tmp35A[11]="MUL_ASSIGN";static char _tmp35B[11]="DIV_ASSIGN";static char _tmp35C[11]="MOD_ASSIGN";static char _tmp35D[11]="ADD_ASSIGN";static char _tmp35E[11]="SUB_ASSIGN";static char _tmp35F[12]="LEFT_ASSIGN";static char _tmp360[13]="RIGHT_ASSIGN";static char _tmp361[11]="AND_ASSIGN";static char _tmp362[11]="XOR_ASSIGN";static char _tmp363[10]="OR_ASSIGN";static char _tmp364[9]="ELLIPSIS";static char _tmp365[11]="LEFT_RIGHT";static char _tmp366[12]="COLON_COLON";static char _tmp367[11]="IDENTIFIER";static char _tmp368[17]="INTEGER_CONSTANT";static char _tmp369[7]="STRING";static char _tmp36A[8]="WSTRING";static char _tmp36B[19]="CHARACTER_CONSTANT";static char _tmp36C[20]="WCHARACTER_CONSTANT";static char _tmp36D[18]="FLOATING_CONSTANT";static char _tmp36E[9]="TYPE_VAR";static char _tmp36F[13]="TYPEDEF_NAME";static char _tmp370[16]="QUAL_IDENTIFIER";static char _tmp371[18]="QUAL_TYPEDEF_NAME";static char _tmp372[10]="ATTRIBUTE";static char _tmp373[4]="ASM";static char _tmp374[4]="';'";static char _tmp375[4]="'{'";static char _tmp376[4]="'}'";static char _tmp377[4]="','";static char _tmp378[4]="'='";static char _tmp379[4]="'<'";static char _tmp37A[4]="'>'";static char _tmp37B[4]="'('";static char _tmp37C[4]="')'";static char _tmp37D[4]="'_'";static char _tmp37E[4]="'$'";static char _tmp37F[4]="':'";static char _tmp380[4]="'.'";static char _tmp381[4]="'['";static char _tmp382[4]="']'";static char _tmp383[4]="'*'";static char _tmp384[4]="'@'";static char _tmp385[4]="'?'";static char _tmp386[4]="'+'";static char _tmp387[4]="'|'";static char _tmp388[4]="'^'";static char _tmp389[4]="'&'";static char _tmp38A[4]="'-'";static char _tmp38B[4]="'/'";static char _tmp38C[4]="'%'";static char _tmp38D[4]="'~'";static char _tmp38E[4]="'!'";static char _tmp38F[5]="prog";static char _tmp390[17]="translation_unit";static char _tmp391[16]="extern_c_action";static char _tmp392[13]="end_extern_c";static char _tmp393[12]="export_list";static char _tmp394[19]="export_list_values";static char _tmp395[21]="external_declaration";static char _tmp396[15]="optional_comma";static char _tmp397[20]="function_definition";static char _tmp398[21]="function_definition2";static char _tmp399[13]="using_action";static char _tmp39A[15]="unusing_action";static char _tmp39B[17]="namespace_action";static char _tmp39C[19]="unnamespace_action";static char _tmp39D[12]="declaration";static char _tmp39E[19]="resetable_qual_opt";static char _tmp39F[17]="declaration_list";static char _tmp3A0[23]="declaration_specifiers";static char _tmp3A1[24]="storage_class_specifier";static char _tmp3A2[15]="attributes_opt";static char _tmp3A3[11]="attributes";static char _tmp3A4[15]="attribute_list";static char _tmp3A5[10]="attribute";static char _tmp3A6[15]="type_specifier";static char _tmp3A7[25]="type_specifier_notypedef";static char _tmp3A8[5]="kind";static char _tmp3A9[15]="type_qualifier";static char _tmp3AA[15]="enum_specifier";static char _tmp3AB[11]="enum_field";static char _tmp3AC[22]="enum_declaration_list";static char _tmp3AD[26]="struct_or_union_specifier";static char _tmp3AE[16]="type_params_opt";static char _tmp3AF[16]="struct_or_union";static char _tmp3B0[24]="struct_declaration_list";static char _tmp3B1[25]="struct_declaration_list0";static char _tmp3B2[21]="init_declarator_list";static char _tmp3B3[22]="init_declarator_list0";static char _tmp3B4[16]="init_declarator";static char _tmp3B5[19]="struct_declaration";static char _tmp3B6[25]="specifier_qualifier_list";static char _tmp3B7[35]="notypedef_specifier_qualifier_list";static char _tmp3B8[23]="struct_declarator_list";static char _tmp3B9[24]="struct_declarator_list0";static char _tmp3BA[18]="struct_declarator";static char _tmp3BB[20]="requires_clause_opt";static char _tmp3BC[19]="ensures_clause_opt";static char _tmp3BD[19]="datatype_specifier";static char _tmp3BE[14]="qual_datatype";static char _tmp3BF[19]="datatypefield_list";static char _tmp3C0[20]="datatypefield_scope";static char _tmp3C1[14]="datatypefield";static char _tmp3C2[11]="declarator";static char _tmp3C3[23]="declarator_withtypedef";static char _tmp3C4[18]="direct_declarator";static char _tmp3C5[30]="direct_declarator_withtypedef";static char _tmp3C6[8]="pointer";static char _tmp3C7[12]="one_pointer";static char _tmp3C8[14]="pointer_quals";static char _tmp3C9[13]="pointer_qual";static char _tmp3CA[23]="pointer_null_and_bound";static char _tmp3CB[14]="pointer_bound";static char _tmp3CC[18]="zeroterm_qual_opt";static char _tmp3CD[8]="rgn_opt";static char _tmp3CE[11]="tqual_list";static char _tmp3CF[20]="parameter_type_list";static char _tmp3D0[9]="type_var";static char _tmp3D1[16]="optional_effect";static char _tmp3D2[19]="optional_rgn_order";static char _tmp3D3[10]="rgn_order";static char _tmp3D4[16]="optional_inject";static char _tmp3D5[11]="effect_set";static char _tmp3D6[14]="atomic_effect";static char _tmp3D7[11]="region_set";static char _tmp3D8[15]="parameter_list";static char _tmp3D9[22]="parameter_declaration";static char _tmp3DA[16]="identifier_list";static char _tmp3DB[17]="identifier_list0";static char _tmp3DC[12]="initializer";static char _tmp3DD[18]="array_initializer";static char _tmp3DE[17]="initializer_list";static char _tmp3DF[12]="designation";static char _tmp3E0[16]="designator_list";static char _tmp3E1[11]="designator";static char _tmp3E2[10]="type_name";static char _tmp3E3[14]="any_type_name";static char _tmp3E4[15]="type_name_list";static char _tmp3E5[20]="abstract_declarator";static char _tmp3E6[27]="direct_abstract_declarator";static char _tmp3E7[10]="statement";static char _tmp3E8[18]="labeled_statement";static char _tmp3E9[21]="expression_statement";static char _tmp3EA[19]="compound_statement";static char _tmp3EB[16]="block_item_list";static char _tmp3EC[20]="selection_statement";static char _tmp3ED[15]="switch_clauses";static char _tmp3EE[20]="iteration_statement";static char _tmp3EF[15]="jump_statement";static char _tmp3F0[12]="exp_pattern";static char _tmp3F1[20]="conditional_pattern";static char _tmp3F2[19]="logical_or_pattern";static char _tmp3F3[20]="logical_and_pattern";static char _tmp3F4[21]="inclusive_or_pattern";static char _tmp3F5[21]="exclusive_or_pattern";static char _tmp3F6[12]="and_pattern";static char _tmp3F7[17]="equality_pattern";static char _tmp3F8[19]="relational_pattern";static char _tmp3F9[14]="shift_pattern";static char _tmp3FA[17]="additive_pattern";static char _tmp3FB[23]="multiplicative_pattern";static char _tmp3FC[13]="cast_pattern";static char _tmp3FD[14]="unary_pattern";static char _tmp3FE[16]="postfix_pattern";static char _tmp3FF[16]="primary_pattern";static char _tmp400[8]="pattern";static char _tmp401[19]="tuple_pattern_list";static char _tmp402[20]="tuple_pattern_list0";static char _tmp403[14]="field_pattern";static char _tmp404[19]="field_pattern_list";static char _tmp405[20]="field_pattern_list0";static char _tmp406[11]="expression";static char _tmp407[22]="assignment_expression";static char _tmp408[20]="assignment_operator";static char _tmp409[23]="conditional_expression";static char _tmp40A[20]="constant_expression";static char _tmp40B[22]="logical_or_expression";static char _tmp40C[23]="logical_and_expression";static char _tmp40D[24]="inclusive_or_expression";static char _tmp40E[24]="exclusive_or_expression";static char _tmp40F[15]="and_expression";static char _tmp410[20]="equality_expression";static char _tmp411[22]="relational_expression";static char _tmp412[17]="shift_expression";static char _tmp413[20]="additive_expression";static char _tmp414[26]="multiplicative_expression";static char _tmp415[16]="cast_expression";static char _tmp416[17]="unary_expression";static char _tmp417[15]="unary_operator";static char _tmp418[19]="postfix_expression";static char _tmp419[17]="field_expression";static char _tmp41A[19]="primary_expression";static char _tmp41B[25]="argument_expression_list";static char _tmp41C[26]="argument_expression_list0";static char _tmp41D[9]="constant";static char _tmp41E[20]="qual_opt_identifier";static char _tmp41F[17]="qual_opt_typedef";static char _tmp420[18]="struct_union_name";static char _tmp421[11]="field_name";static char _tmp422[12]="right_angle";
# 1609 "parse.y"
static struct _dyneither_ptr Cyc_yytname[296]={{_tmp2FB,_tmp2FB,_tmp2FB + 2},{_tmp2FC,_tmp2FC,_tmp2FC + 6},{_tmp2FD,_tmp2FD,_tmp2FD + 12},{_tmp2FE,_tmp2FE,_tmp2FE + 5},{_tmp2FF,_tmp2FF,_tmp2FF + 9},{_tmp300,_tmp300,_tmp300 + 7},{_tmp301,_tmp301,_tmp301 + 7},{_tmp302,_tmp302,_tmp302 + 8},{_tmp303,_tmp303,_tmp303 + 5},{_tmp304,_tmp304,_tmp304 + 5},{_tmp305,_tmp305,_tmp305 + 6},{_tmp306,_tmp306,_tmp306 + 4},{_tmp307,_tmp307,_tmp307 + 5},{_tmp308,_tmp308,_tmp308 + 6},{_tmp309,_tmp309,_tmp309 + 7},{_tmp30A,_tmp30A,_tmp30A + 7},{_tmp30B,_tmp30B,_tmp30B + 9},{_tmp30C,_tmp30C,_tmp30C + 6},{_tmp30D,_tmp30D,_tmp30D + 9},{_tmp30E,_tmp30E,_tmp30E + 9},{_tmp30F,_tmp30F,_tmp30F + 7},{_tmp310,_tmp310,_tmp310 + 6},{_tmp311,_tmp311,_tmp311 + 5},{_tmp312,_tmp312,_tmp312 + 8},{_tmp313,_tmp313,_tmp313 + 7},{_tmp314,_tmp314,_tmp314 + 7},{_tmp315,_tmp315,_tmp315 + 9},{_tmp316,_tmp316,_tmp316 + 3},{_tmp317,_tmp317,_tmp317 + 5},{_tmp318,_tmp318,_tmp318 + 7},{_tmp319,_tmp319,_tmp319 + 6},{_tmp31A,_tmp31A,_tmp31A + 3},{_tmp31B,_tmp31B,_tmp31B + 4},{_tmp31C,_tmp31C,_tmp31C + 5},{_tmp31D,_tmp31D,_tmp31D + 9},{_tmp31E,_tmp31E,_tmp31E + 6},{_tmp31F,_tmp31F,_tmp31F + 7},{_tmp320,_tmp320,_tmp320 + 5},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 16},{_tmp323,_tmp323,_tmp323 + 8},{_tmp324,_tmp324,_tmp324 + 4},{_tmp325,_tmp325,_tmp325 + 6},{_tmp326,_tmp326,_tmp326 + 4},{_tmp327,_tmp327,_tmp327 + 6},{_tmp328,_tmp328,_tmp328 + 7},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 9},{_tmp32B,_tmp32B,_tmp32B + 9},{_tmp32C,_tmp32C,_tmp32C + 6},{_tmp32D,_tmp32D,_tmp32D + 10},{_tmp32E,_tmp32E,_tmp32E + 9},{_tmp32F,_tmp32F,_tmp32F + 7},{_tmp330,_tmp330,_tmp330 + 8},{_tmp331,_tmp331,_tmp331 + 15},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 8},{_tmp334,_tmp334,_tmp334 + 5},{_tmp335,_tmp335,_tmp335 + 9},{_tmp336,_tmp336,_tmp336 + 6},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 5},{_tmp339,_tmp339,_tmp339 + 8},{_tmp33A,_tmp33A,_tmp33A + 13},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 8},{_tmp33D,_tmp33D,_tmp33D + 12},{_tmp33E,_tmp33E,_tmp33E + 8},{_tmp33F,_tmp33F,_tmp33F + 8},{_tmp340,_tmp340,_tmp340 + 10},{_tmp341,_tmp341,_tmp341 + 9},{_tmp342,_tmp342,_tmp342 + 13},{_tmp343,_tmp343,_tmp343 + 10},{_tmp344,_tmp344,_tmp344 + 9},{_tmp345,_tmp345,_tmp345 + 13},{_tmp346,_tmp346,_tmp346 + 14},{_tmp347,_tmp347,_tmp347 + 14},{_tmp348,_tmp348,_tmp348 + 13},{_tmp349,_tmp349,_tmp349 + 12},{_tmp34A,_tmp34A,_tmp34A + 16},{_tmp34B,_tmp34B,_tmp34B + 14},{_tmp34C,_tmp34C,_tmp34C + 12},{_tmp34D,_tmp34D,_tmp34D + 16},{_tmp34E,_tmp34E,_tmp34E + 15},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 7},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 8},{_tmp353,_tmp353,_tmp353 + 9},{_tmp354,_tmp354,_tmp354 + 6},{_tmp355,_tmp355,_tmp355 + 6},{_tmp356,_tmp356,_tmp356 + 6},{_tmp357,_tmp357,_tmp357 + 6},{_tmp358,_tmp358,_tmp358 + 7},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 11},{_tmp35B,_tmp35B,_tmp35B + 11},{_tmp35C,_tmp35C,_tmp35C + 11},{_tmp35D,_tmp35D,_tmp35D + 11},{_tmp35E,_tmp35E,_tmp35E + 11},{_tmp35F,_tmp35F,_tmp35F + 12},{_tmp360,_tmp360,_tmp360 + 13},{_tmp361,_tmp361,_tmp361 + 11},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 10},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,_tmp365,_tmp365 + 11},{_tmp366,_tmp366,_tmp366 + 12},{_tmp367,_tmp367,_tmp367 + 11},{_tmp368,_tmp368,_tmp368 + 17},{_tmp369,_tmp369,_tmp369 + 7},{_tmp36A,_tmp36A,_tmp36A + 8},{_tmp36B,_tmp36B,_tmp36B + 19},{_tmp36C,_tmp36C,_tmp36C + 20},{_tmp36D,_tmp36D,_tmp36D + 18},{_tmp36E,_tmp36E,_tmp36E + 9},{_tmp36F,_tmp36F,_tmp36F + 13},{_tmp370,_tmp370,_tmp370 + 16},{_tmp371,_tmp371,_tmp371 + 18},{_tmp372,_tmp372,_tmp372 + 10},{_tmp373,_tmp373,_tmp373 + 4},{_tmp374,_tmp374,_tmp374 + 4},{_tmp375,_tmp375,_tmp375 + 4},{_tmp376,_tmp376,_tmp376 + 4},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 5},{_tmp390,_tmp390,_tmp390 + 17},{_tmp391,_tmp391,_tmp391 + 16},{_tmp392,_tmp392,_tmp392 + 13},{_tmp393,_tmp393,_tmp393 + 12},{_tmp394,_tmp394,_tmp394 + 19},{_tmp395,_tmp395,_tmp395 + 21},{_tmp396,_tmp396,_tmp396 + 15},{_tmp397,_tmp397,_tmp397 + 20},{_tmp398,_tmp398,_tmp398 + 21},{_tmp399,_tmp399,_tmp399 + 13},{_tmp39A,_tmp39A,_tmp39A + 15},{_tmp39B,_tmp39B,_tmp39B + 17},{_tmp39C,_tmp39C,_tmp39C + 19},{_tmp39D,_tmp39D,_tmp39D + 12},{_tmp39E,_tmp39E,_tmp39E + 19},{_tmp39F,_tmp39F,_tmp39F + 17},{_tmp3A0,_tmp3A0,_tmp3A0 + 23},{_tmp3A1,_tmp3A1,_tmp3A1 + 24},{_tmp3A2,_tmp3A2,_tmp3A2 + 15},{_tmp3A3,_tmp3A3,_tmp3A3 + 11},{_tmp3A4,_tmp3A4,_tmp3A4 + 15},{_tmp3A5,_tmp3A5,_tmp3A5 + 10},{_tmp3A6,_tmp3A6,_tmp3A6 + 15},{_tmp3A7,_tmp3A7,_tmp3A7 + 25},{_tmp3A8,_tmp3A8,_tmp3A8 + 5},{_tmp3A9,_tmp3A9,_tmp3A9 + 15},{_tmp3AA,_tmp3AA,_tmp3AA + 15},{_tmp3AB,_tmp3AB,_tmp3AB + 11},{_tmp3AC,_tmp3AC,_tmp3AC + 22},{_tmp3AD,_tmp3AD,_tmp3AD + 26},{_tmp3AE,_tmp3AE,_tmp3AE + 16},{_tmp3AF,_tmp3AF,_tmp3AF + 16},{_tmp3B0,_tmp3B0,_tmp3B0 + 24},{_tmp3B1,_tmp3B1,_tmp3B1 + 25},{_tmp3B2,_tmp3B2,_tmp3B2 + 21},{_tmp3B3,_tmp3B3,_tmp3B3 + 22},{_tmp3B4,_tmp3B4,_tmp3B4 + 16},{_tmp3B5,_tmp3B5,_tmp3B5 + 19},{_tmp3B6,_tmp3B6,_tmp3B6 + 25},{_tmp3B7,_tmp3B7,_tmp3B7 + 35},{_tmp3B8,_tmp3B8,_tmp3B8 + 23},{_tmp3B9,_tmp3B9,_tmp3B9 + 24},{_tmp3BA,_tmp3BA,_tmp3BA + 18},{_tmp3BB,_tmp3BB,_tmp3BB + 20},{_tmp3BC,_tmp3BC,_tmp3BC + 19},{_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 14},{_tmp3BF,_tmp3BF,_tmp3BF + 19},{_tmp3C0,_tmp3C0,_tmp3C0 + 20},{_tmp3C1,_tmp3C1,_tmp3C1 + 14},{_tmp3C2,_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 23},{_tmp3C4,_tmp3C4,_tmp3C4 + 18},{_tmp3C5,_tmp3C5,_tmp3C5 + 30},{_tmp3C6,_tmp3C6,_tmp3C6 + 8},{_tmp3C7,_tmp3C7,_tmp3C7 + 12},{_tmp3C8,_tmp3C8,_tmp3C8 + 14},{_tmp3C9,_tmp3C9,_tmp3C9 + 13},{_tmp3CA,_tmp3CA,_tmp3CA + 23},{_tmp3CB,_tmp3CB,_tmp3CB + 14},{_tmp3CC,_tmp3CC,_tmp3CC + 18},{_tmp3CD,_tmp3CD,_tmp3CD + 8},{_tmp3CE,_tmp3CE,_tmp3CE + 11},{_tmp3CF,_tmp3CF,_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 9},{_tmp3D1,_tmp3D1,_tmp3D1 + 16},{_tmp3D2,_tmp3D2,_tmp3D2 + 19},{_tmp3D3,_tmp3D3,_tmp3D3 + 10},{_tmp3D4,_tmp3D4,_tmp3D4 + 16},{_tmp3D5,_tmp3D5,_tmp3D5 + 11},{_tmp3D6,_tmp3D6,_tmp3D6 + 14},{_tmp3D7,_tmp3D7,_tmp3D7 + 11},{_tmp3D8,_tmp3D8,_tmp3D8 + 15},{_tmp3D9,_tmp3D9,_tmp3D9 + 22},{_tmp3DA,_tmp3DA,_tmp3DA + 16},{_tmp3DB,_tmp3DB,_tmp3DB + 17},{_tmp3DC,_tmp3DC,_tmp3DC + 12},{_tmp3DD,_tmp3DD,_tmp3DD + 18},{_tmp3DE,_tmp3DE,_tmp3DE + 17},{_tmp3DF,_tmp3DF,_tmp3DF + 12},{_tmp3E0,_tmp3E0,_tmp3E0 + 16},{_tmp3E1,_tmp3E1,_tmp3E1 + 11},{_tmp3E2,_tmp3E2,_tmp3E2 + 10},{_tmp3E3,_tmp3E3,_tmp3E3 + 14},{_tmp3E4,_tmp3E4,_tmp3E4 + 15},{_tmp3E5,_tmp3E5,_tmp3E5 + 20},{_tmp3E6,_tmp3E6,_tmp3E6 + 27},{_tmp3E7,_tmp3E7,_tmp3E7 + 10},{_tmp3E8,_tmp3E8,_tmp3E8 + 18},{_tmp3E9,_tmp3E9,_tmp3E9 + 21},{_tmp3EA,_tmp3EA,_tmp3EA + 19},{_tmp3EB,_tmp3EB,_tmp3EB + 16},{_tmp3EC,_tmp3EC,_tmp3EC + 20},{_tmp3ED,_tmp3ED,_tmp3ED + 15},{_tmp3EE,_tmp3EE,_tmp3EE + 20},{_tmp3EF,_tmp3EF,_tmp3EF + 15},{_tmp3F0,_tmp3F0,_tmp3F0 + 12},{_tmp3F1,_tmp3F1,_tmp3F1 + 20},{_tmp3F2,_tmp3F2,_tmp3F2 + 19},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 21},{_tmp3F5,_tmp3F5,_tmp3F5 + 21},{_tmp3F6,_tmp3F6,_tmp3F6 + 12},{_tmp3F7,_tmp3F7,_tmp3F7 + 17},{_tmp3F8,_tmp3F8,_tmp3F8 + 19},{_tmp3F9,_tmp3F9,_tmp3F9 + 14},{_tmp3FA,_tmp3FA,_tmp3FA + 17},{_tmp3FB,_tmp3FB,_tmp3FB + 23},{_tmp3FC,_tmp3FC,_tmp3FC + 13},{_tmp3FD,_tmp3FD,_tmp3FD + 14},{_tmp3FE,_tmp3FE,_tmp3FE + 16},{_tmp3FF,_tmp3FF,_tmp3FF + 16},{_tmp400,_tmp400,_tmp400 + 8},{_tmp401,_tmp401,_tmp401 + 19},{_tmp402,_tmp402,_tmp402 + 20},{_tmp403,_tmp403,_tmp403 + 14},{_tmp404,_tmp404,_tmp404 + 19},{_tmp405,_tmp405,_tmp405 + 20},{_tmp406,_tmp406,_tmp406 + 11},{_tmp407,_tmp407,_tmp407 + 22},{_tmp408,_tmp408,_tmp408 + 20},{_tmp409,_tmp409,_tmp409 + 23},{_tmp40A,_tmp40A,_tmp40A + 20},{_tmp40B,_tmp40B,_tmp40B + 22},{_tmp40C,_tmp40C,_tmp40C + 23},{_tmp40D,_tmp40D,_tmp40D + 24},{_tmp40E,_tmp40E,_tmp40E + 24},{_tmp40F,_tmp40F,_tmp40F + 15},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 22},{_tmp412,_tmp412,_tmp412 + 17},{_tmp413,_tmp413,_tmp413 + 20},{_tmp414,_tmp414,_tmp414 + 26},{_tmp415,_tmp415,_tmp415 + 16},{_tmp416,_tmp416,_tmp416 + 17},{_tmp417,_tmp417,_tmp417 + 15},{_tmp418,_tmp418,_tmp418 + 19},{_tmp419,_tmp419,_tmp419 + 17},{_tmp41A,_tmp41A,_tmp41A + 19},{_tmp41B,_tmp41B,_tmp41B + 25},{_tmp41C,_tmp41C,_tmp41C + 26},{_tmp41D,_tmp41D,_tmp41D + 9},{_tmp41E,_tmp41E,_tmp41E + 20},{_tmp41F,_tmp41F,_tmp41F + 17},{_tmp420,_tmp420,_tmp420 + 18},{_tmp421,_tmp421,_tmp421 + 11},{_tmp422,_tmp422,_tmp422 + 12}};
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
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4D8[8]="stdcall";static char _tmp4D9[6]="cdecl";static char _tmp4DA[9]="fastcall";static char _tmp4DB[9]="noreturn";static char _tmp4DC[6]="const";static char _tmp4DD[8]="aligned";static char _tmp4DE[7]="packed";static char _tmp4DF[7]="shared";static char _tmp4E0[7]="unused";static char _tmp4E1[5]="weak";static char _tmp4E2[10]="dllimport";static char _tmp4E3[10]="dllexport";static char _tmp4E4[23]="no_instrument_function";static char _tmp4E5[12]="constructor";static char _tmp4E6[11]="destructor";static char _tmp4E7[22]="no_check_memory_usage";static char _tmp4E8[5]="pure";static char _tmp4E9[14]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9D6(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9D5,unsigned int*_tmp9D4,union Cyc_YYSTYPE**_tmp9D3){for(*_tmp9D5=0;*_tmp9D5 < *_tmp9D4;(*_tmp9D5)++){(*_tmp9D3)[*_tmp9D5]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9DB(unsigned int*_tmp9DA,unsigned int*_tmp9D9,struct Cyc_Yyltype**_tmp9D8){for(*_tmp9DA=0;*_tmp9DA < *_tmp9D9;(*_tmp9DA)++){(*_tmp9D8)[*_tmp9DA]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9E2(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9E1,unsigned int*_tmp9E0,short**_tmp9DE){for(*_tmp9E1=0;*_tmp9E1 < *_tmp9E0;(*_tmp9E1)++){(*_tmp9DE)[*_tmp9E1]=(short)(
# 216
*_tmp9E1 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9E1)): 0);}}static void _tmp9E8(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9E7,unsigned int*_tmp9E6,union Cyc_YYSTYPE**_tmp9E4){for(*_tmp9E7=0;*_tmp9E7 < *_tmp9E6;(*_tmp9E7)++){(*_tmp9E4)[*_tmp9E7]=(union Cyc_YYSTYPE)(
# 219
*_tmp9E7 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9E7)):*yylval);}}static void _tmp9EE(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9ED,unsigned int*_tmp9EC,struct Cyc_Yyltype**_tmp9EA){for(*_tmp9ED=0;*_tmp9ED < *_tmp9EC;(*_tmp9ED)++){(*_tmp9EA)[*_tmp9ED]=(struct Cyc_Yyltype)(
# 222
*_tmp9ED <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9ED)):
 Cyc_yynewloc());}}static void _tmpF0F(unsigned int*_tmpF0E,unsigned int*_tmpF0D,char**_tmpF0B){for(*_tmpF0E=0;*_tmpF0E < *_tmpF0D;(*_tmpF0E)++){(*_tmpF0B)[*_tmpF0E]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp424=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp424;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9CF;union Cyc_YYSTYPE yylval=((_tmp9CF.YYINITIALSVAL).val=0,(((_tmp9CF.YYINITIALSVAL).tag=64,_tmp9CF)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9D1;unsigned int _tmp9D0;struct _dyneither_ptr yyss=(_tmp9D0=200,((_tmp9D1=_region_calloc(yyregion,sizeof(short),_tmp9D0),_tag_dyneither(_tmp9D1,sizeof(short),_tmp9D0))));
# 158
int yyvsp_offset;
unsigned int _tmp9D5;unsigned int _tmp9D4;union Cyc_YYSTYPE*_tmp9D3;unsigned int _tmp9D2;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9D2=(unsigned int)200,((_tmp9D3=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9D2)),((((_tmp9D4=_tmp9D2,_tmp9D6(& yylval,& _tmp9D5,& _tmp9D4,& _tmp9D3))),_tmp9D3)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9DA;unsigned int _tmp9D9;struct Cyc_Yyltype*_tmp9D8;unsigned int _tmp9D7;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9D7=(unsigned int)200,((_tmp9D8=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9D7)),((((_tmp9D9=_tmp9D7,_tmp9DB(& _tmp9DA,& _tmp9D9,& _tmp9D8))),_tmp9D8)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9DC;Cyc_yyerror(((_tmp9DC="parser stack overflow",_tag_dyneither(_tmp9DC,sizeof(char),22))),yystate,yychar);}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9E1;unsigned int _tmp9E0;struct _dyneither_ptr _tmp9DF;short*_tmp9DE;unsigned int _tmp9DD;struct _dyneither_ptr yyss1=(_tmp9DD=(unsigned int)yystacksize,((_tmp9DE=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9DD)),((_tmp9DF=_tag_dyneither(_tmp9DE,sizeof(short),_tmp9DD),((((_tmp9E0=_tmp9DD,_tmp9E2(& yyssp_offset,& yyss,& _tmp9E1,& _tmp9E0,& _tmp9DE))),_tmp9DF)))))));
# 217
unsigned int _tmp9E7;unsigned int _tmp9E6;struct _dyneither_ptr _tmp9E5;union Cyc_YYSTYPE*_tmp9E4;unsigned int _tmp9E3;struct _dyneither_ptr yyvs1=
(_tmp9E3=(unsigned int)yystacksize,((_tmp9E4=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9E3)),((_tmp9E5=_tag_dyneither(_tmp9E4,sizeof(union Cyc_YYSTYPE),_tmp9E3),((((_tmp9E6=_tmp9E3,_tmp9E8(& yyvs,& yyssp_offset,& yylval,& _tmp9E7,& _tmp9E6,& _tmp9E4))),_tmp9E5)))))));
# 221
unsigned int _tmp9ED;unsigned int _tmp9EC;struct _dyneither_ptr _tmp9EB;struct Cyc_Yyltype*_tmp9EA;unsigned int _tmp9E9;struct _dyneither_ptr yyls1=(_tmp9E9=(unsigned int)yystacksize,((_tmp9EA=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9E9)),((_tmp9EB=_tag_dyneither(_tmp9EA,sizeof(struct Cyc_Yyltype),_tmp9E9),((((_tmp9EC=_tmp9E9,_tmp9EE(& yyssp_offset,& yyls,& _tmp9ED,& _tmp9EC,& _tmp9EA))),_tmp9EB)))))));
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
int _tmp432=0;_npop_handler(0);return _tmp432;}
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
{int _tmp433=yyn;switch(_tmp433){case 1: _LL240: _LL241: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1220 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1223
goto _LL23F;}case 2: _LL242: _LL243: {
# 1225
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1227
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1226 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp434;_push_handler(& _tmp434);{int _tmp436=0;if(setjmp(_tmp434.handler))_tmp436=1;if(!_tmp436){
x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp435=(void*)_exn_thrown;void*_tmp437=_tmp435;void*_tmp438;if(((struct Cyc_Core_Failure_exn_struct*)_tmp437)->tag == Cyc_Core_Failure){_LL661: _LL662:
# 1231
 x=0;goto _LL660;}else{_LL663: _tmp438=_tmp437;_LL664:(int)_rethrow(_tmp438);}_LL660:;}};}
# 1233
{struct _handler_cons _tmp439;_push_handler(& _tmp439);{int _tmp43B=0;if(setjmp(_tmp439.handler))_tmp43B=1;if(!_tmp43B){
y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp43A=(void*)_exn_thrown;void*_tmp43C=_tmp43A;void*_tmp43D;if(((struct Cyc_Core_Failure_exn_struct*)_tmp43C)->tag == Cyc_Core_Failure){_LL666: _LL667:
# 1237
 y=0;goto _LL665;}else{_LL668: _tmp43D=_tmp43C;_LL669:(int)_rethrow(_tmp43D);}_LL665:;}};}
# 1239
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1241
goto _LL23F;}case 3: _LL244: _LL245: {
# 1243
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1244 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9F8;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9F7;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9F6;struct Cyc_List_List*_tmp9F5;yyval=Cyc_YY16(((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5->hd=((_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8->r=(void*)((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6[0]=((_tmp9F7.tag=10,((_tmp9F7.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9F7.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmp9F7)))))),_tmp9F6)))),((_tmp9F8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F8)))))),((_tmp9F5->tl=0,_tmp9F5)))))));}
Cyc_Lex_leave_using();
# 1247
goto _LL23F;}case 4: _LL246: _LL247: {
# 1249
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1248 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA02;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA01;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA00;struct Cyc_List_List*_tmp9FF;yyval=Cyc_YY16(((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF->hd=((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->r=(void*)((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=10,((_tmpA01.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA01.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA01)))))),_tmpA00)))),((_tmpA02->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA02)))))),((_tmp9FF->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmp9FF)))))));}
goto _LL23F;}case 5: _LL248: _LL249: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1251 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA11;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA10;struct _dyneither_ptr*_tmpA0F;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA0E;struct Cyc_List_List*_tmpA0D;yyval=Cyc_YY16(((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->hd=((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->r=(void*)((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E[0]=((_tmpA10.tag=9,((_tmpA10.f1=((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA0F)))),((_tmpA10.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA10)))))),_tmpA0E)))),((_tmpA11->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA11)))))),((_tmpA0D->tl=0,_tmpA0D)))))));}
Cyc_Lex_leave_namespace();
# 1254
goto _LL23F;}case 6: _LL24A: _LL24B: {
# 1256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1256 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp44B;_push_handler(& _tmp44B);{int _tmp44D=0;if(setjmp(_tmp44B.handler))_tmp44D=1;if(!_tmp44D){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp44C=(void*)_exn_thrown;void*_tmp44E=_tmp44C;void*_tmp44F;if(((struct Cyc_Core_Failure_exn_struct*)_tmp44E)->tag == Cyc_Core_Failure){_LL66B: _LL66C:
{const char*_tmpA12;nspace=((_tmpA12="",_tag_dyneither(_tmpA12,sizeof(char),1)));}goto _LL66A;}else{_LL66D: _tmp44F=_tmp44E;_LL66E:(int)_rethrow(_tmp44F);}_LL66A:;}};}
# 1261
{struct _handler_cons _tmp451;_push_handler(& _tmp451);{int _tmp453=0;if(setjmp(_tmp451.handler))_tmp453=1;if(!_tmp453){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp452=(void*)_exn_thrown;void*_tmp454=_tmp452;void*_tmp455;if(((struct Cyc_Core_Failure_exn_struct*)_tmp454)->tag == Cyc_Core_Failure){_LL670: _LL671:
 x=0;goto _LL66F;}else{_LL672: _tmp455=_tmp454;_LL673:(int)_rethrow(_tmp455);}_LL66F:;}};}
# 1264
{struct _handler_cons _tmp456;_push_handler(& _tmp456);{int _tmp458=0;if(setjmp(_tmp456.handler))_tmp458=1;if(!_tmp458){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp457=(void*)_exn_thrown;void*_tmp459=_tmp457;void*_tmp45A;if(((struct Cyc_Core_Failure_exn_struct*)_tmp459)->tag == Cyc_Core_Failure){_LL675: _LL676:
 y=0;goto _LL674;}else{_LL677: _tmp45A=_tmp459;_LL678:(int)_rethrow(_tmp45A);}_LL674:;}};}
# 1267
{struct Cyc_Absyn_Decl*_tmpA21;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA20;struct _dyneither_ptr*_tmpA1F;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA1E;struct Cyc_List_List*_tmpA1D;yyval=Cyc_YY16(((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->hd=((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->r=(void*)((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=((_tmpA20.tag=9,((_tmpA20.f1=((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F[0]=nspace,_tmpA1F)))),((_tmpA20.f2=x,_tmpA20)))))),_tmpA1E)))),((_tmpA21->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA21)))))),((_tmpA1D->tl=y,_tmpA1D)))))));}
# 1269
goto _LL23F;}case 7: _LL24C: _LL24D: {
# 1271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1270 "parse.y"
int _tmp460=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp460){
struct Cyc_Absyn_Decl*_tmpA2B;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA2A;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA29;struct Cyc_List_List*_tmpA28;yyval=Cyc_YY16(((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->hd=((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B->r=(void*)((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29[0]=((_tmpA2A.tag=11,((_tmpA2A.f1=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA2A)))),_tmpA29)))),((_tmpA2B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA2B)))))),((_tmpA28->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA28)))))));}else{
# 1274
struct Cyc_Absyn_Decl*_tmpA35;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA34;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA33;struct Cyc_List_List*_tmpA32;yyval=Cyc_YY16(((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->hd=((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35->r=(void*)((_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33[0]=((_tmpA34.tag=12,((_tmpA34.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA34.f2=0,_tmpA34)))))),_tmpA33)))),((_tmpA35->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA35)))))),((_tmpA32->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA32)))))));}
# 1276
goto _LL23F;}case 8: _LL24E: _LL24F: {
# 1278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1277 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0])){
const char*_tmpA36;Cyc_Parse_err(((_tmpA36="expecting \"C include\"",_tag_dyneither(_tmpA36,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
{struct Cyc_Absyn_Decl*_tmpA40;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA3F;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA3E;struct Cyc_List_List*_tmpA3D;yyval=Cyc_YY16(((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->hd=((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40->r=(void*)((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((_tmpA3F.tag=12,((_tmpA3F.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA3F.f2=exs,_tmpA3F)))))),_tmpA3E)))),((_tmpA40->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA40)))))),((_tmpA3D->tl=Cyc_yyget_YY16(yyyyvsp[5]),_tmpA3D)))))));}
# 1282
goto _LL23F;};}case 9: _LL250: _LL251: {
# 1284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1283 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA43;struct Cyc_List_List*_tmpA42;yyval=Cyc_YY16(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->hd=((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA43->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA43)))))),((_tmpA42->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA42)))))));}
goto _LL23F;}case 10: _LL252: _LL253: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1285 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA46;struct Cyc_List_List*_tmpA45;yyval=Cyc_YY16(((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->hd=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA46->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA46)))))),((_tmpA45->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA45)))))));}
goto _LL23F;}case 11: _LL254: _LL255:
# 1288
 yyval=Cyc_YY16(0);
goto _LL23F;case 12: _LL256: _LL257: {
# 1291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1291 "parse.y"
struct _dyneither_ptr _tmp472=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
{const char*_tmpA47;if(Cyc_strcmp((struct _dyneither_ptr)_tmp472,((_tmpA47="C",_tag_dyneither(_tmpA47,sizeof(char),2))))== 0)
yyval=Cyc_YY31(0);else{
const char*_tmpA48;if(Cyc_strcmp((struct _dyneither_ptr)_tmp472,((_tmpA48="C include",_tag_dyneither(_tmpA48,sizeof(char),10))))== 0)
yyval=Cyc_YY31(1);else{
# 1298
{const char*_tmpA49;Cyc_Parse_err(((_tmpA49="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA49,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY31(1);}}}
# 1302
goto _LL23F;}case 13: _LL258: _LL259: {
# 1304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1305 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL23F;}case 14: _LL25A: _LL25B: {
# 1308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1309 "parse.y"
yyval=yyyyvsp[2];
goto _LL23F;}case 15: _LL25C: _LL25D: {
# 1312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1310 "parse.y"
yyval=Cyc_YY52(0);
goto _LL23F;}case 16: _LL25E: _LL25F: {
# 1313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1314 "parse.y"
{struct _tuple27*_tmpA4C;struct Cyc_List_List*_tmpA4B;yyval=Cyc_YY52(((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B->hd=((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA4C->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA4C->f3=0,_tmpA4C)))))))),((_tmpA4B->tl=0,_tmpA4B)))))));}
goto _LL23F;}case 17: _LL260: _LL261: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1315 "parse.y"
{struct _tuple27*_tmpA4F;struct Cyc_List_List*_tmpA4E;yyval=Cyc_YY52(((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA4F->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA4F->f3=0,_tmpA4F)))))))),((_tmpA4E->tl=0,_tmpA4E)))))));}
goto _LL23F;}case 18: _LL262: _LL263: {
# 1318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1317 "parse.y"
{struct _tuple27*_tmpA52;struct Cyc_List_List*_tmpA51;yyval=Cyc_YY52(((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->hd=((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA52->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA52->f3=0,_tmpA52)))))))),((_tmpA51->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA51)))))));}
goto _LL23F;}case 19: _LL264: _LL265: {
# 1320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1321 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA58;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA57;struct Cyc_List_List*_tmpA56;yyval=Cyc_YY16(((_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56->hd=Cyc_Absyn_new_decl((void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA57.tag=1,((_tmpA57.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpA57)))),_tmpA58)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA56->tl=0,_tmpA56)))))));}
goto _LL23F;}case 20: _LL266: _LL267: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1322 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 21: _LL268: _LL269: {
# 1325
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1323 "parse.y"
yyval=Cyc_YY16(0);
goto _LL23F;}case 24: _LL26A: _LL26B: {
# 1326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1332 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),0,Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 25: _LL26C: _LL26D: {
# 1335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1334 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp47F=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp47F,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 26: _LL26E: _LL26F: {
# 1338
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1347 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1]),Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 27: _LL270: _LL271: {
# 1350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1349 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp480=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp480,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 28: _LL272: _LL273: {
# 1353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1357 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp481=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp481,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 29: _LL274: _LL275: {
# 1361
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1360 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp482=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp482,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 30: _LL276: _LL277: {
# 1364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1365 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL23F;}case 31: _LL278: _LL279: {
# 1368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1368 "parse.y"
Cyc_Lex_leave_using();
goto _LL23F;}case 32: _LL27A: _LL27B: {
# 1371
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1373
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1371 "parse.y"
{struct _dyneither_ptr*_tmpA59;Cyc_Lex_enter_namespace(((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA59)))));}yyval=yyyyvsp[1];
goto _LL23F;}case 33: _LL27C: _LL27D: {
# 1374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1374 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL23F;}case 34: _LL27E: _LL27F: {
# 1377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp484=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),0,(unsigned int)_tmp484,(unsigned int)_tmp484));
goto _LL23F;}case 35: _LL280: _LL281: {
# 1384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1383 "parse.y"
int _tmp485=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp485));
goto _LL23F;}case 36: _LL282: _LL283: {
# 1387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1387 "parse.y"
{struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY16(((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA5A->tl=0,_tmpA5A)))))));}
goto _LL23F;}case 37: _LL284: _LL285: {
# 1390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1389 "parse.y"
struct Cyc_List_List*_tmp487=0;
{struct Cyc_List_List*_tmp488=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp488 != 0;_tmp488=_tmp488->tl){
struct _dyneither_ptr*_tmp489=(struct _dyneither_ptr*)_tmp488->hd;
struct _tuple0*_tmpA5B;struct _tuple0*qv=(_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->f1=Cyc_Absyn_Rel_n(0),((_tmpA5B->f2=_tmp489,_tmpA5B)))));
struct Cyc_Absyn_Vardecl*_tmp48A=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA5C;_tmp487=((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->hd=_tmp48A,((_tmpA5C->tl=_tmp487,_tmpA5C))))));}}
# 1396
_tmp487=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp487);
{struct Cyc_List_List*_tmpA5D;yyval=Cyc_YY16(((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->hd=Cyc_Absyn_letv_decl(_tmp487,(unsigned int)(yyyylsp[0]).first_line),((_tmpA5D->tl=0,_tmpA5D)))))));}
# 1399
goto _LL23F;}case 38: _LL286: _LL287: {
# 1401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1402 "parse.y"
struct _dyneither_ptr _tmp48E=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1404
{const char*_tmpA5E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp48E,((_tmpA5E="`H",_tag_dyneither(_tmpA5E,sizeof(char),3))))== 0){
const char*_tmpA61;void*_tmpA60;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA60=0,Cyc_aprintf(((_tmpA61="bad occurrence of heap region",_tag_dyneither(_tmpA61,sizeof(char),30))),_tag_dyneither(_tmpA60,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA62;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp48E,((_tmpA62="`U",_tag_dyneither(_tmpA62,sizeof(char),3))))== 0){
const char*_tmpA65;void*_tmpA64;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA64=0,Cyc_aprintf(((_tmpA65="bad occurrence of unique region",_tag_dyneither(_tmpA65,sizeof(char),32))),_tag_dyneither(_tmpA64,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA68;struct Cyc_Absyn_Tvar*_tmpA67;struct Cyc_Absyn_Tvar*tv=(_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67->name=((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68[0]=_tmp48E,_tmpA68)))),((_tmpA67->identity=- 1,((_tmpA67->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA67)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA6B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA6A;void*t=(void*)((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6B.tag=2,((_tmpA6B.f1=tv,_tmpA6B)))),_tmpA6A))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA71;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA70;struct _dyneither_ptr*_tmpA6E;struct _tuple0*_tmpA6D;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->f1=Cyc_Absyn_Loc_n,((_tmpA6D->f2=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA6E)))),_tmpA6D)))))),(void*)((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=15,((_tmpA71.f1=t,_tmpA71)))),_tmpA70)))),0);
{struct Cyc_List_List*_tmpA72;yyval=Cyc_YY16(((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA72->tl=0,_tmpA72)))))));}
# 1413
goto _LL23F;};}case 39: _LL288: _LL289: {
# 1415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1415 "parse.y"
int _tmp49E=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp49F=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpA73;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49F,((_tmpA73="H",_tag_dyneither(_tmpA73,sizeof(char),2))))== 0){
const char*_tmpA74;Cyc_Parse_err(((_tmpA74="bad occurrence of heap region `H",_tag_dyneither(_tmpA74,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA75;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49F,((_tmpA75="U",_tag_dyneither(_tmpA75,sizeof(char),2))))== 0){
const char*_tmpA76;Cyc_Parse_err(((_tmpA76="bad occurrence of unique region `U",_tag_dyneither(_tmpA76,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpA83;const char*_tmpA82;void*_tmpA81[1];struct Cyc_String_pa_PrintArg_struct _tmpA80;struct Cyc_Absyn_Tvar*_tmpA7F;struct Cyc_Absyn_Tvar*tv=(_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->name=((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=(struct _dyneither_ptr)((_tmpA80.tag=0,((_tmpA80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp49F),((_tmpA81[0]=& _tmpA80,Cyc_aprintf(((_tmpA82="`%s",_tag_dyneither(_tmpA82,sizeof(char),4))),_tag_dyneither(_tmpA81,sizeof(void*),1)))))))),_tmpA83)))),((_tmpA7F->identity=- 1,((_tmpA7F->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA7F)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA86;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA85;void*t=(void*)((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85[0]=((_tmpA86.tag=2,((_tmpA86.f1=tv,_tmpA86)))),_tmpA85))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA8C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA8B;struct _dyneither_ptr*_tmpA89;struct _tuple0*_tmpA88;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->f1=Cyc_Absyn_Loc_n,((_tmpA88->f2=((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=_tmp49F,_tmpA89)))),_tmpA88)))))),(void*)((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8C.tag=15,((_tmpA8C.f1=t,_tmpA8C)))),_tmpA8B)))),0);
# 1426
{struct Cyc_List_List*_tmpA8D;yyval=Cyc_YY16(((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->hd=Cyc_Absyn_region_decl(tv,vd,_tmp49E,0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA8D->tl=0,_tmpA8D)))))));}
# 1428
goto _LL23F;};}case 40: _LL28A: _LL28B: {
# 1430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1430 "parse.y"
struct _dyneither_ptr _tmp4B0=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4B1=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_yyget_Exp_tok(yyyyvsp[6]);
{const char*_tmpA8E;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B1,((_tmpA8E="open",_tag_dyneither(_tmpA8E,sizeof(char),5))))!= 0){const char*_tmpA8F;Cyc_Parse_err(((_tmpA8F="expecting `open'",_tag_dyneither(_tmpA8F,sizeof(char),17))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA9C;const char*_tmpA9B;void*_tmpA9A[1];struct Cyc_String_pa_PrintArg_struct _tmpA99;struct Cyc_Absyn_Tvar*_tmpA98;struct Cyc_Absyn_Tvar*tv=(_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->name=((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=(struct _dyneither_ptr)((_tmpA99.tag=0,((_tmpA99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4B0),((_tmpA9A[0]=& _tmpA99,Cyc_aprintf(((_tmpA9B="`%s",_tag_dyneither(_tmpA9B,sizeof(char),4))),_tag_dyneither(_tmpA9A,sizeof(void*),1)))))))),_tmpA9C)))),((_tmpA98->identity=- 1,((_tmpA98->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA98)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA9F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA9E;void*t=(void*)((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E[0]=((_tmpA9F.tag=2,((_tmpA9F.f1=tv,_tmpA9F)))),_tmpA9E))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAA5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAA4;struct _dyneither_ptr*_tmpAA2;struct _tuple0*_tmpAA1;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->f1=Cyc_Absyn_Loc_n,((_tmpAA1->f2=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=_tmp4B0,_tmpAA2)))),_tmpAA1)))))),(void*)((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=((_tmpAA5.tag=15,((_tmpAA5.f1=t,_tmpAA5)))),_tmpAA4)))),0);
# 1439
{struct Cyc_List_List*_tmpAA6;yyval=Cyc_YY16(((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4B2,(unsigned int)(yyyylsp[0]).first_line),((_tmpAA6->tl=0,_tmpAA6)))))));}
# 1441
goto _LL23F;};}case 41: _LL28C: _LL28D:
# 1443
 yyval=Cyc_YY31(0);
goto _LL23F;case 42: _LL28E: _LL28F: {
# 1446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1445 "parse.y"
yyval=Cyc_YY31(1);
goto _LL23F;}case 43: _LL290: _LL291: {
# 1448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1450 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 44: _LL292: _LL293: {
# 1453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1452 "parse.y"
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1])));
goto _LL23F;}case 45: _LL294: _LL295: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpAA7;yyval=Cyc_YY17(((_tmpAA7.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAA7.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA7.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAA7.is_inline=0,((_tmpAA7.attributes=0,_tmpAA7)))))))))));}
goto _LL23F;}case 46: _LL296: _LL297: {
# 1462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1461 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C2=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp4C2.sc != 0){
const char*_tmpAAA;void*_tmpAA9;(_tmpAA9=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAAA="Only one storage class is allowed in a declaration (missing ';' or ','?)",_tag_dyneither(_tmpAAA,sizeof(char),73))),_tag_dyneither(_tmpAA9,sizeof(void*),0)));}
# 1465
{struct Cyc_Parse_Declaration_spec _tmpAAB;yyval=Cyc_YY17(((_tmpAAB.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAAB.tq=_tmp4C2.tq,((_tmpAAB.type_specs=_tmp4C2.type_specs,((_tmpAAB.is_inline=_tmp4C2.is_inline,((_tmpAAB.attributes=_tmp4C2.attributes,_tmpAAB)))))))))));}
# 1469
goto _LL23F;}case 47: _LL298: _LL299: {
# 1471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1470 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAAC;yyval=Cyc_YY17(((_tmpAAC.sc=0,((_tmpAAC.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAAC.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAAC.is_inline=0,((_tmpAAC.attributes=0,_tmpAAC)))))))))));}
goto _LL23F;}case 48: _LL29A: _LL29B: {
# 1474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1473 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C7=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAAD;yyval=Cyc_YY17(((_tmpAAD.sc=_tmp4C7.sc,((_tmpAAD.tq=_tmp4C7.tq,((_tmpAAD.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4C7.type_specs,Cyc_yyget_YY21(yyyyvsp[0])),((_tmpAAD.is_inline=_tmp4C7.is_inline,((_tmpAAD.attributes=_tmp4C7.attributes,_tmpAAD)))))))))));}
# 1479
goto _LL23F;}case 49: _LL29C: _LL29D: {
# 1481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1480 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAAE;yyval=Cyc_YY17(((_tmpAAE.sc=0,((_tmpAAE.tq=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAAE.type_specs=Cyc_Parse_empty_spec(0),((_tmpAAE.is_inline=0,((_tmpAAE.attributes=0,_tmpAAE)))))))))));}
goto _LL23F;}case 50: _LL29E: _LL29F: {
# 1483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1482 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CA=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAAF;yyval=Cyc_YY17(((_tmpAAF.sc=_tmp4CA.sc,((_tmpAAF.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp4CA.tq),((_tmpAAF.type_specs=_tmp4CA.type_specs,((_tmpAAF.is_inline=_tmp4CA.is_inline,((_tmpAAF.attributes=_tmp4CA.attributes,_tmpAAF)))))))))));}
# 1487
goto _LL23F;}case 51: _LL2A0: _LL2A1: {
# 1489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1488 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAB0;yyval=Cyc_YY17(((_tmpAB0.sc=0,((_tmpAB0.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAB0.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAB0.is_inline=1,((_tmpAB0.attributes=0,_tmpAB0)))))))))));}
goto _LL23F;}case 52: _LL2A2: _LL2A3: {
# 1492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1491 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CD=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAB1;yyval=Cyc_YY17(((_tmpAB1.sc=_tmp4CD.sc,((_tmpAB1.tq=_tmp4CD.tq,((_tmpAB1.type_specs=_tmp4CD.type_specs,((_tmpAB1.is_inline=1,((_tmpAB1.attributes=_tmp4CD.attributes,_tmpAB1)))))))))));}
# 1495
goto _LL23F;}case 53: _LL2A4: _LL2A5: {
# 1497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1496 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAB2;yyval=Cyc_YY17(((_tmpAB2.sc=0,((_tmpAB2.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAB2.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAB2.is_inline=0,((_tmpAB2.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAB2)))))))))));}
goto _LL23F;}case 54: _LL2A6: _LL2A7: {
# 1500
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1499 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D0=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAB3;yyval=Cyc_YY17(((_tmpAB3.sc=_tmp4D0.sc,((_tmpAB3.tq=_tmp4D0.tq,((_tmpAB3.type_specs=_tmp4D0.type_specs,((_tmpAB3.is_inline=_tmp4D0.is_inline,((_tmpAB3.attributes=
# 1502
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp4D0.attributes),_tmpAB3)))))))))));}
goto _LL23F;}case 55: _LL2A8: _LL2A9: {
# 1505
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1507
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1506 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL23F;}case 56: _LL2AA: _LL2AB: {
# 1510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1508 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL23F;}case 57: _LL2AC: _LL2AD: {
# 1512
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1514
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1510 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL23F;}case 58: _LL2AE: _LL2AF: {
# 1514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1512 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL23F;}case 59: _LL2B0: _LL2B1: {
# 1516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1515 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpAB4;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAB4="C",_tag_dyneither(_tmpAB4,sizeof(char),2))))!= 0){
const char*_tmpAB5;Cyc_Parse_err(((_tmpAB5="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAB5,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY20(& externC_sc);
# 1520
goto _LL23F;}case 60: _LL2B2: _LL2B3: {
# 1522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1520 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL23F;}case 61: _LL2B4: _LL2B5: {
# 1524
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1523 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL23F;}case 62: _LL2B6: _LL2B7:
# 1527
 yyval=Cyc_YY45(0);
goto _LL23F;case 63: _LL2B8: _LL2B9: {
# 1530
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1530 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 64: _LL2BA: _LL2BB: {
# 1533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1535 "parse.y"
yyval=yyyyvsp[3];
goto _LL23F;}case 65: _LL2BC: _LL2BD: {
# 1538
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1539 "parse.y"
{struct Cyc_List_List*_tmpAB6;yyval=Cyc_YY45(((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAB6->tl=0,_tmpAB6)))))));}
goto _LL23F;}case 66: _LL2BE: _LL2BF: {
# 1542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1540 "parse.y"
{struct Cyc_List_List*_tmpAB7;yyval=Cyc_YY45(((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAB7->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAB7)))))));}
goto _LL23F;}case 67: _LL2C0: _LL2C1: {
# 1543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1545 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple28 att_map[18]={{{_tmp4D8,_tmp4D8,_tmp4D8 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4D9,_tmp4D9,_tmp4D9 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4DA,_tmp4DA,_tmp4DA + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4DB,_tmp4DB,_tmp4DB + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4DC,_tmp4DC,_tmp4DC + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4DD,_tmp4DD,_tmp4DD + 8},(void*)& att_aligned},{{_tmp4DE,_tmp4DE,_tmp4DE + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4DF,_tmp4DF,_tmp4DF + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E0,_tmp4E0,_tmp4E0 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4E1,_tmp4E1,_tmp4E1 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4E7,_tmp4E7,_tmp4E7 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 5},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 14},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1566
struct _dyneither_ptr _tmp4D6=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1568
if((((_get_dyneither_size(_tmp4D6,sizeof(char))> 4  && ((const char*)_tmp4D6.curr)[0]== '_') && ((const char*)_tmp4D6.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),(int)(_get_dyneither_size(_tmp4D6,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),(int)(_get_dyneither_size(_tmp4D6,sizeof(char))- 3)))== '_')
# 1570
_tmp4D6=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4D6,2,_get_dyneither_size(_tmp4D6,sizeof(char))- 5);{
int i=0;
for(0;i < 18;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4D6,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(18,i)]).f2);
break;}}
# 1577
if(i == 18){
{const char*_tmpAB8;Cyc_Parse_err(((_tmpAB8="unrecognized attribute",_tag_dyneither(_tmpAB8,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1582
goto _LL23F;};}case 68: _LL2C2: _LL2C3: {
# 1584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1582 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL23F;}case 69: _LL2C4: _LL2C5: {
# 1585
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1584 "parse.y"
struct _dyneither_ptr _tmp4EB=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp4EC=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
{const char*_tmpABA;const char*_tmpAB9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpABA="aligned",_tag_dyneither(_tmpABA,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpAB9="__aligned__",_tag_dyneither(_tmpAB9,sizeof(char),12))))== 0){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpABD;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpABC;a=(void*)((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=((_tmpABD.tag=6,((_tmpABD.f1=_tmp4EC,_tmpABD)))),_tmpABC))));}else{
const char*_tmpABF;const char*_tmpABE;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpABF="section",_tag_dyneither(_tmpABF,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpABE="__section__",_tag_dyneither(_tmpABE,sizeof(char),12))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAC2;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAC1;a=(void*)((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1[0]=((_tmpAC2.tag=8,((_tmpAC2.f1=str,_tmpAC2)))),_tmpAC1))));}else{
const char*_tmpAC3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpAC3="__mode__",_tag_dyneither(_tmpAC3,sizeof(char),9))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAC6;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAC5;a=(void*)((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5[0]=((_tmpAC6.tag=24,((_tmpAC6.f1=str,_tmpAC6)))),_tmpAC5))));}else{
const char*_tmpAC7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpAC7="alias",_tag_dyneither(_tmpAC7,sizeof(char),6))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmpACA;struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmpAC9;a=(void*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=25,((_tmpACA.f1=str,_tmpACA)))),_tmpAC9))));}else{
# 1600
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
const char*_tmpACC;const char*_tmpACB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpACC="regparm",_tag_dyneither(_tmpACC,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpACB="__regparm__",_tag_dyneither(_tmpACB,sizeof(char),12))))== 0){
if(n < 0  || n > 3){
const char*_tmpACD;Cyc_Parse_err(((_tmpACD="regparm requires value between 0 and 3",_tag_dyneither(_tmpACD,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpAD0;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpACF;a=(void*)((_tmpACF=_cycalloc_atomic(sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=0,((_tmpAD0.f1=n,_tmpAD0)))),_tmpACF))));};}else{
const char*_tmpAD2;const char*_tmpAD1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpAD2="initializes",_tag_dyneither(_tmpAD2,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpAD1="__initializes__",_tag_dyneither(_tmpAD1,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpAD5;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpAD4;a=(void*)((_tmpAD4=_cycalloc_atomic(sizeof(*_tmpAD4)),((_tmpAD4[0]=((_tmpAD5.tag=20,((_tmpAD5.f1=n,_tmpAD5)))),_tmpAD4))));}else{
const char*_tmpAD7;const char*_tmpAD6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpAD7="noliveunique",_tag_dyneither(_tmpAD7,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpAD6="__noliveunique__",_tag_dyneither(_tmpAD6,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpADA;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpAD9;a=(void*)((_tmpAD9=_cycalloc_atomic(sizeof(*_tmpAD9)),((_tmpAD9[0]=((_tmpADA.tag=21,((_tmpADA.f1=n,_tmpADA)))),_tmpAD9))));}else{
const char*_tmpADC;const char*_tmpADB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpADC="noconsume",_tag_dyneither(_tmpADC,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,((_tmpADB="__noconsume__",_tag_dyneither(_tmpADB,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpADF;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpADE;a=(void*)((_tmpADE=_cycalloc_atomic(sizeof(*_tmpADE)),((_tmpADE[0]=((_tmpADF.tag=22,((_tmpADF.f1=n,_tmpADF)))),_tmpADE))));}else{
# 1612
{const char*_tmpAE0;Cyc_Parse_err(((_tmpAE0="unrecognized attribute",_tag_dyneither(_tmpAE0,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}}
# 1616
yyval=Cyc_YY46(a);
# 1618
goto _LL23F;}case 70: _LL2C6: _LL2C7: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1619 "parse.y"
struct _dyneither_ptr _tmp50D=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp50E=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp50F=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_Int_tok(yyyyvsp[4]));
unsigned int _tmp510=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[6]).first_line,Cyc_yyget_Int_tok(yyyyvsp[6]));
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAE2;const char*_tmpAE1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50D,((_tmpAE2="format",_tag_dyneither(_tmpAE2,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50D,((_tmpAE1="__format__",_tag_dyneither(_tmpAE1,sizeof(char),11))))== 0){
const char*_tmpAE3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,((_tmpAE3="printf",_tag_dyneither(_tmpAE3,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAE6;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAE5;a=(void*)((_tmpAE5=_cycalloc_atomic(sizeof(*_tmpAE5)),((_tmpAE5[0]=((_tmpAE6.tag=19,((_tmpAE6.f1=Cyc_Absyn_Printf_ft,((_tmpAE6.f2=(int)_tmp50F,((_tmpAE6.f3=(int)_tmp510,_tmpAE6)))))))),_tmpAE5))));}else{
const char*_tmpAE7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,((_tmpAE7="scanf",_tag_dyneither(_tmpAE7,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAEA;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAE9;a=(void*)((_tmpAE9=_cycalloc_atomic(sizeof(*_tmpAE9)),((_tmpAE9[0]=((_tmpAEA.tag=19,((_tmpAEA.f1=Cyc_Absyn_Scanf_ft,((_tmpAEA.f2=(int)_tmp50F,((_tmpAEA.f3=(int)_tmp510,_tmpAEA)))))))),_tmpAE9))));}else{
# 1630
const char*_tmpAEB;Cyc_Parse_err(((_tmpAEB="unrecognized format type",_tag_dyneither(_tmpAEB,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1632
const char*_tmpAEC;Cyc_Parse_err(((_tmpAEC="unrecognized attribute",_tag_dyneither(_tmpAEC,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);
# 1635
goto _LL23F;}case 71: _LL2C8: _LL2C9: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1644 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 72: _LL2CA: _LL2CB: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1646 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAEF;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAEE;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAEF.tag=17,((_tmpAEF.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAEF.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpAEF.f3=0,((_tmpAEF.f4=0,_tmpAEF)))))))))),_tmpAEE)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 73: _LL2CC: _LL2CD: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 74: _LL2CE: _LL2CF: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 75: _LL2D0: _LL2D1: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1652 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 76: _LL2D2: _LL2D3: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1653 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 77: _LL2D4: _LL2D5: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 78: _LL2D6: _LL2D7: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 79: _LL2D8: _LL2D9: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 80: _LL2DA: _LL2DB: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 81: _LL2DC: _LL2DD: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1658 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 82: _LL2DE: _LL2DF: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1659 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 83: _LL2E0: _LL2E1: {
# 1662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1660 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 84: _LL2E2: _LL2E3: {
# 1663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1663 "parse.y"
{struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpAF2;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAF1;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF2.tag=27,((_tmpAF2.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpAF2)))),_tmpAF1)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 85: _LL2E4: _LL2E5: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1665 "parse.y"
{struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmpAF8;const char*_tmpAF7;struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmpAF6;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF8.tag=28,((_tmpAF8.f1=((_tmpAF7="__builtin_va_list",_tag_dyneither(_tmpAF7,sizeof(char),18))),((_tmpAF8.f2=& Cyc_Tcutil_bk,_tmpAF8)))))),_tmpAF6)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 86: _LL2E6: _LL2E7: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1667 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 87: _LL2E8: _LL2E9: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1669 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 88: _LL2EA: _LL2EB: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1671 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 89: _LL2EC: _LL2ED: {
# 1674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1673 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 90: _LL2EE: _LL2EF: {
# 1676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1675 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAFB;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAFA;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=10,((_tmpAFB.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpAFB)))),_tmpAFA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1678
goto _LL23F;}case 91: _LL2F0: _LL2F1: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1679 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAFE;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAFD;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD[0]=((_tmpAFE.tag=15,((_tmpAFE.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpAFE)))),_tmpAFD)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 92: _LL2F2: _LL2F3: {
# 1682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1681 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB01;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB00;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=15,((_tmpB01.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpB01)))),_tmpB00)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1683
goto _LL23F;}case 93: _LL2F4: _LL2F5: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1684 "parse.y"
void*_tmp528=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB04;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB03;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB04.tag=16,((_tmpB04.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB04.f2=_tmp528,_tmpB04)))))),_tmpB03)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1687
goto _LL23F;}case 94: _LL2F6: _LL2F7: {
# 1689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1688 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB07;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB06;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06[0]=((_tmpB07.tag=16,((_tmpB07.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB07.f2=Cyc_yyget_YY44(yyyyvsp[4]),_tmpB07)))))),_tmpB06)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 95: _LL2F8: _LL2F9: {
# 1691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1690 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB0A;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB09;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0A.tag=19,((_tmpB0A.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB0A)))),_tmpB09)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 96: _LL2FA: _LL2FB: {
# 1693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1692 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB0D;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB0C;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C[0]=((_tmpB0D.tag=19,((_tmpB0D.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB0D)))),_tmpB0C)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 97: _LL2FC: _LL2FD: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1694 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB10;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB0F;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB10.tag=18,((_tmpB10.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB10)))),_tmpB0F)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 98: _LL2FE: _LL2FF: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 99: _LL300: _LL301: {
# 1703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1704 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpB11;yyval=Cyc_YY23(((_tmpB11.print_const=1,((_tmpB11.q_volatile=0,((_tmpB11.q_restrict=0,((_tmpB11.real_const=1,((_tmpB11.loc=loc,_tmpB11)))))))))));}
goto _LL23F;}case 100: _LL302: _LL303: {
# 1708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1706 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB12;yyval=Cyc_YY23(((_tmpB12.print_const=0,((_tmpB12.q_volatile=1,((_tmpB12.q_restrict=0,((_tmpB12.real_const=0,((_tmpB12.loc=0,_tmpB12)))))))))));}
goto _LL23F;}case 101: _LL304: _LL305: {
# 1709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1707 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB13;yyval=Cyc_YY23(((_tmpB13.print_const=0,((_tmpB13.q_volatile=0,((_tmpB13.q_restrict=1,((_tmpB13.real_const=0,((_tmpB13.loc=0,_tmpB13)))))))))));}
goto _LL23F;}case 102: _LL306: _LL307: {
# 1710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB22;struct Cyc_Absyn_Enumdecl*_tmpB21;struct Cyc_Core_Opt*_tmpB20;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB1F;struct Cyc_Absyn_TypeDecl*_tmpB1E;struct Cyc_Absyn_TypeDecl*_tmp536=(_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E->r=(void*)((_tmpB22=_cycalloc(sizeof(*_tmpB22)),((_tmpB22[0]=((_tmpB1F.tag=1,((_tmpB1F.f1=((_tmpB21=_cycalloc(sizeof(*_tmpB21)),((_tmpB21->sc=Cyc_Absyn_Public,((_tmpB21->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB21->fields=((_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpB20)))),_tmpB21)))))))),_tmpB1F)))),_tmpB22)))),((_tmpB1E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB1E)))));
# 1715
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB25;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB24;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB25.tag=26,((_tmpB25.f1=_tmp536,((_tmpB25.f2=0,_tmpB25)))))),_tmpB24)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1717
goto _LL23F;}case 103: _LL308: _LL309: {
# 1719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1718 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB28;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB27;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=((_tmpB28.tag=13,((_tmpB28.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB28.f2=0,_tmpB28)))))),_tmpB27)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 104: _LL30A: _LL30B: {
# 1721
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1723
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1720 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB2B;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB2A;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A[0]=((_tmpB2B.tag=14,((_tmpB2B.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB2B)))),_tmpB2A)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 105: _LL30C: _LL30D: {
# 1723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB2C;yyval=Cyc_YY47(((_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB2C->tag=0,((_tmpB2C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB2C)))))))));}
goto _LL23F;}case 106: _LL30E: _LL30F: {
# 1729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1728 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB2D;yyval=Cyc_YY47(((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB2D->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB2D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB2D)))))))));}
goto _LL23F;}case 107: _LL310: _LL311: {
# 1731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1732 "parse.y"
{struct Cyc_List_List*_tmpB2E;yyval=Cyc_YY48(((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB2E->tl=0,_tmpB2E)))))));}
goto _LL23F;}case 108: _LL312: _LL313: {
# 1735
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1733 "parse.y"
{struct Cyc_List_List*_tmpB2F;yyval=Cyc_YY48(((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB2F->tl=0,_tmpB2F)))))));}
goto _LL23F;}case 109: _LL314: _LL315: {
# 1736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1734 "parse.y"
{struct Cyc_List_List*_tmpB30;yyval=Cyc_YY48(((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB30->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB30)))))));}
goto _LL23F;}case 110: _LL316: _LL317: {
# 1737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB33;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB32;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB33.tag=12,((_tmpB33.f1=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB33.f2=Cyc_yyget_YY24(yyyyvsp[2]),_tmpB33)))))),_tmpB32)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 111: _LL318: _LL319: {
# 1743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1746 "parse.y"
struct Cyc_List_List*_tmp549=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp54A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp54B=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp549,
Cyc_Absyn_aggrdecl_impl(_tmp54A,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY24(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1751
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB36;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB35;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB36.tag=26,((_tmpB36.f1=_tmp54B,((_tmpB36.f2=0,_tmpB36)))))),_tmpB35)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1753
goto _LL23F;}case 112: _LL31A: _LL31B: {
# 1755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1757 "parse.y"
struct Cyc_List_List*_tmp54E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp54F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp550=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp54E,
Cyc_Absyn_aggrdecl_impl(_tmp54F,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY24(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1762
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB39;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB38;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB39.tag=26,((_tmpB39.f1=_tmp550,((_tmpB39.f2=0,_tmpB39)))))),_tmpB38)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1764
goto _LL23F;}case 113: _LL31C: _LL31D: {
# 1766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1765 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB43;struct Cyc_Core_Opt*_tmpB42;struct Cyc_Absyn_AggrInfo _tmpB41;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB40;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40[0]=((_tmpB43.tag=11,((_tmpB43.f1=((_tmpB41.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB42=_cycalloc_atomic(sizeof(*_tmpB42)),((_tmpB42->v=(void*)1,_tmpB42))))),((_tmpB41.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpB41)))),_tmpB43)))),_tmpB40)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1768
goto _LL23F;}case 114: _LL31E: _LL31F: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1769 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB49;struct Cyc_Absyn_AggrInfo _tmpB48;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB47;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=((_tmpB49.tag=11,((_tmpB49.f1=((_tmpB48.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB48.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB48)))),_tmpB49)))),_tmpB47)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1772
goto _LL23F;}case 115: _LL320: _LL321:
# 1774
 yyval=Cyc_YY40(0);
goto _LL23F;case 116: _LL322: _LL323: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1778 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL23F;}case 117: _LL324: _LL325: {
# 1781
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1783
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1782 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL23F;}case 118: _LL326: _LL327: {
# 1785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1783 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL23F;}case 119: _LL328: _LL329:
# 1786
 yyval=Cyc_YY24(0);
goto _LL23F;case 120: _LL32A: _LL32B: {
# 1789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1790 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp55A=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp55A != 0;_tmp55A=_tmp55A->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp55A->hd,decls);}}{
# 1794
struct Cyc_List_List*_tmp55B=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp55B != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp55B,decls);
yyval=Cyc_YY24(decls);
# 1799
goto _LL23F;};}case 121: _LL32C: _LL32D: {
# 1801
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB4A;yyval=Cyc_YY25(((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A->hd=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB4A->tl=0,_tmpB4A)))))));}
goto _LL23F;}case 122: _LL32E: _LL32F: {
# 1807
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1809
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1806 "parse.y"
{struct Cyc_List_List*_tmpB4B;yyval=Cyc_YY25(((_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B->hd=Cyc_yyget_YY24(yyyyvsp[1]),((_tmpB4B->tl=Cyc_yyget_YY25(yyyyvsp[0]),_tmpB4B)))))));}
goto _LL23F;}case 123: _LL330: _LL331: {
# 1809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1810 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
goto _LL23F;}case 124: _LL332: _LL333: {
# 1813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB4C;yyval=Cyc_YY19(((_tmpB4C=_region_malloc(yyr,sizeof(*_tmpB4C)),((_tmpB4C->tl=0,((_tmpB4C->hd=Cyc_yyget_YY18(yyyyvsp[0]),_tmpB4C)))))));}
goto _LL23F;}case 125: _LL334: _LL335: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1818 "parse.y"
{struct _tuple12*_tmpB4D;yyval=Cyc_YY19(((_tmpB4D=_region_malloc(yyr,sizeof(*_tmpB4D)),((_tmpB4D->tl=Cyc_yyget_YY19(yyyyvsp[0]),((_tmpB4D->hd=Cyc_yyget_YY18(yyyyvsp[2]),_tmpB4D)))))));}
goto _LL23F;}case 126: _LL336: _LL337: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1823 "parse.y"
{struct _tuple11 _tmpB4E;yyval=Cyc_YY18(((_tmpB4E.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB4E.f2=0,_tmpB4E)))));}
goto _LL23F;}case 127: _LL338: _LL339: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1825 "parse.y"
{struct _tuple11 _tmpB4F;yyval=Cyc_YY18(((_tmpB4F.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB4F.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB4F)))));}
goto _LL23F;}case 128: _LL33A: _LL33B: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1831 "parse.y"
struct _RegionHandle _tmp562=_new_region("temp");struct _RegionHandle*temp=& _tmp562;_push_region(temp);{
struct _tuple25 _tmp563=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp565;struct Cyc_Parse_Type_specifier _tmp566;struct Cyc_List_List*_tmp567;struct _tuple25 _tmp564=_tmp563;_tmp565=_tmp564.f1;_tmp566=_tmp564.f2;_tmp567=_tmp564.f3;
if(_tmp565.loc == 0)_tmp565.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp568=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp568 != 0;_tmp568=_tmp568->tl){
struct _tuple24*_tmp569=(struct _tuple24*)_tmp568->hd;struct Cyc_Parse_Declarator _tmp56B;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Exp*_tmp56D;struct _tuple24*_tmp56A=_tmp569;_tmp56B=_tmp56A->f1;_tmp56C=_tmp56A->f2;_tmp56D=_tmp56A->f3;
{struct _tuple10*_tmpB50;decls=((_tmpB50=_region_malloc(temp,sizeof(*_tmpB50)),((_tmpB50->tl=decls,((_tmpB50->hd=_tmp56B,_tmpB50))))));}{
struct _tuple16*_tmpB53;struct Cyc_List_List*_tmpB52;widths_and_reqs=(
(_tmpB52=_region_malloc(temp,sizeof(*_tmpB52)),((_tmpB52->hd=((_tmpB53=_region_malloc(temp,sizeof(*_tmpB53)),((_tmpB53->f1=_tmp56C,((_tmpB53->f2=_tmp56D,_tmpB53)))))),((_tmpB52->tl=widths_and_reqs,_tmpB52))))));};}}
# 1842
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp571=Cyc_Parse_speclist2typ(_tmp566,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp572=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp565,_tmp571,decls,_tmp567),widths_and_reqs);
# 1848
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp572));};};}
# 1851
_npop_handler(0);goto _LL23F;
# 1831
;_pop_region(temp);}case 129: _LL33C: _LL33D: {
# 1853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1859 "parse.y"
{struct _tuple25 _tmpB54;yyval=Cyc_YY35(((_tmpB54.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB54.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB54.f3=0,_tmpB54)))))));}
goto _LL23F;}case 130: _LL33E: _LL33F: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1861 "parse.y"
struct _tuple25 _tmp574=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB55;yyval=Cyc_YY35(((_tmpB55.f1=_tmp574.f1,((_tmpB55.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp574.f2),((_tmpB55.f3=_tmp574.f3,_tmpB55)))))));}
goto _LL23F;}case 131: _LL340: _LL341: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1863 "parse.y"
{struct _tuple25 _tmpB56;yyval=Cyc_YY35(((_tmpB56.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB56.f2=Cyc_Parse_empty_spec(0),((_tmpB56.f3=0,_tmpB56)))))));}
goto _LL23F;}case 132: _LL342: _LL343: {
# 1866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1865 "parse.y"
struct _tuple25 _tmp577=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB57;yyval=Cyc_YY35(((_tmpB57.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp577.f1),((_tmpB57.f2=_tmp577.f2,((_tmpB57.f3=_tmp577.f3,_tmpB57)))))));}
goto _LL23F;}case 133: _LL344: _LL345: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1868 "parse.y"
{struct _tuple25 _tmpB58;yyval=Cyc_YY35(((_tmpB58.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB58.f2=Cyc_Parse_empty_spec(0),((_tmpB58.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB58)))))));}
goto _LL23F;}case 134: _LL346: _LL347: {
# 1871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1870 "parse.y"
struct _tuple25 _tmp57A=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB59;yyval=Cyc_YY35(((_tmpB59.f1=_tmp57A.f1,((_tmpB59.f2=_tmp57A.f2,((_tmpB59.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp57A.f3),_tmpB59)))))));}
goto _LL23F;}case 135: _LL348: _LL349: {
# 1873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB5A;yyval=Cyc_YY35(((_tmpB5A.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB5A.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB5A.f3=0,_tmpB5A)))))));}
goto _LL23F;}case 136: _LL34A: _LL34B: {
# 1879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1878 "parse.y"
struct _tuple25 _tmp57D=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB5B;yyval=Cyc_YY35(((_tmpB5B.f1=_tmp57D.f1,((_tmpB5B.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp57D.f2),((_tmpB5B.f3=_tmp57D.f3,_tmpB5B)))))));}
goto _LL23F;}case 137: _LL34C: _LL34D: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1880 "parse.y"
{struct _tuple25 _tmpB5C;yyval=Cyc_YY35(((_tmpB5C.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB5C.f2=Cyc_Parse_empty_spec(0),((_tmpB5C.f3=0,_tmpB5C)))))));}
goto _LL23F;}case 138: _LL34E: _LL34F: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1882 "parse.y"
struct _tuple25 _tmp580=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB5D;yyval=Cyc_YY35(((_tmpB5D.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp580.f1),((_tmpB5D.f2=_tmp580.f2,((_tmpB5D.f3=_tmp580.f3,_tmpB5D)))))));}
goto _LL23F;}case 139: _LL350: _LL351: {
# 1886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1885 "parse.y"
{struct _tuple25 _tmpB5E;yyval=Cyc_YY35(((_tmpB5E.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB5E.f2=Cyc_Parse_empty_spec(0),((_tmpB5E.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB5E)))))));}
goto _LL23F;}case 140: _LL352: _LL353: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1887 "parse.y"
struct _tuple25 _tmp583=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB5F;yyval=Cyc_YY35(((_tmpB5F.f1=_tmp583.f1,((_tmpB5F.f2=_tmp583.f2,((_tmpB5F.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp583.f3),_tmpB5F)))))));}
goto _LL23F;}case 141: _LL354: _LL355: {
# 1890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1891 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
goto _LL23F;}case 142: _LL356: _LL357: {
# 1894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB60;yyval=Cyc_YY29(((_tmpB60=_region_malloc(yyr,sizeof(*_tmpB60)),((_tmpB60->hd=Cyc_yyget_YY28(yyyyvsp[0]),((_tmpB60->tl=0,_tmpB60)))))));}
goto _LL23F;}case 143: _LL358: _LL359: {
# 1900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1899 "parse.y"
{struct Cyc_List_List*_tmpB61;yyval=Cyc_YY29(((_tmpB61=_region_malloc(yyr,sizeof(*_tmpB61)),((_tmpB61->hd=Cyc_yyget_YY28(yyyyvsp[2]),((_tmpB61->tl=Cyc_yyget_YY29(yyyyvsp[0]),_tmpB61)))))));}
goto _LL23F;}case 144: _LL35A: _LL35B: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1904 "parse.y"
{struct _tuple24*_tmpB62;yyval=Cyc_YY28(((_tmpB62=_region_malloc(yyr,sizeof(*_tmpB62)),((_tmpB62->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB62->f2=0,((_tmpB62->f3=Cyc_yyget_YY55(yyyyvsp[1]),_tmpB62)))))))));}
goto _LL23F;}case 145: _LL35C: _LL35D: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1908 "parse.y"
{struct Cyc_Parse_Declarator _tmpB68;struct _tuple0*_tmpB67;struct _tuple24*_tmpB66;yyval=Cyc_YY28(((_tmpB66=_region_malloc(yyr,sizeof(*_tmpB66)),((_tmpB66->f1=((_tmpB68.id=((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->f1=Cyc_Absyn_Rel_n(0),((_tmpB67->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB67)))))),((_tmpB68.tms=0,_tmpB68)))),((_tmpB66->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpB66->f3=0,_tmpB66)))))))));}
# 1910
goto _LL23F;}case 146: _LL35E: _LL35F:
# 1914
{struct Cyc_Parse_Declarator _tmpB6E;struct _tuple0*_tmpB6D;struct _tuple24*_tmpB6C;yyval=Cyc_YY28(((_tmpB6C=_region_malloc(yyr,sizeof(*_tmpB6C)),((_tmpB6C->f1=((_tmpB6E.id=((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->f1=Cyc_Absyn_Rel_n(0),((_tmpB6D->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB6D)))))),((_tmpB6E.tms=0,_tmpB6E)))),((_tmpB6C->f2=0,((_tmpB6C->f3=0,_tmpB6C)))))))));}
# 1916
goto _LL23F;case 147: _LL360: _LL361: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1916 "parse.y"
{struct _tuple24*_tmpB6F;yyval=Cyc_YY28(((_tmpB6F=_region_malloc(yyr,sizeof(*_tmpB6F)),((_tmpB6F->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB6F->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB6F->f3=0,_tmpB6F)))))))));}
goto _LL23F;}case 148: _LL362: _LL363: {
# 1919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1920 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL23F;}case 149: _LL364: _LL365:
# 1923
 yyval=Cyc_YY55(0);
goto _LL23F;case 150: _LL366: _LL367: {
# 1926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1925 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL23F;}case 151: _LL368: _LL369:
# 1928
 yyval=Cyc_YY55(0);
goto _LL23F;case 152: _LL36A: _LL36B: {
# 1931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1932 "parse.y"
int _tmp591=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp592=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB70;struct Cyc_Absyn_TypeDecl*_tmp593=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp592,((_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpB70)))),_tmp591,(unsigned int)(yyyylsp[0]).first_line);
# 1936
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB73;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB72;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=26,((_tmpB73.f1=_tmp593,((_tmpB73.f2=0,_tmpB73)))))),_tmpB72)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1938
goto _LL23F;}case 153: _LL36C: _LL36D: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1939 "parse.y"
int _tmp597=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB7D;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB7C;struct Cyc_Absyn_DatatypeInfo _tmpB7B;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB7A;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A[0]=((_tmpB7D.tag=3,((_tmpB7D.f1=((_tmpB7B.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB7C.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB7C.is_extensible=_tmp597,_tmpB7C))))),((_tmpB7B.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB7B)))),_tmpB7D)))),_tmpB7A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1942
goto _LL23F;}case 154: _LL36E: _LL36F: {
# 1944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1943 "parse.y"
int _tmp59C=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB87;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB86;struct Cyc_Absyn_DatatypeFieldInfo _tmpB85;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB84;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84[0]=((_tmpB87.tag=4,((_tmpB87.f1=((_tmpB85.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB86.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB86.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB86.is_extensible=_tmp59C,_tmpB86))))))),((_tmpB85.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpB85)))),_tmpB87)))),_tmpB84)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1948
goto _LL23F;}case 155: _LL370: _LL371: {
# 1950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1951 "parse.y"
yyval=Cyc_YY31(0);
goto _LL23F;}case 156: _LL372: _LL373: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1952 "parse.y"
yyval=Cyc_YY31(1);
goto _LL23F;}case 157: _LL374: _LL375: {
# 1955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1956 "parse.y"
{struct Cyc_List_List*_tmpB88;yyval=Cyc_YY34(((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB88->tl=0,_tmpB88)))))));}
goto _LL23F;}case 158: _LL376: _LL377: {
# 1959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1957 "parse.y"
{struct Cyc_List_List*_tmpB89;yyval=Cyc_YY34(((_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB89->tl=0,_tmpB89)))))));}
goto _LL23F;}case 159: _LL378: _LL379: {
# 1960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1958 "parse.y"
{struct Cyc_List_List*_tmpB8A;yyval=Cyc_YY34(((_tmpB8A=_cycalloc(sizeof(*_tmpB8A)),((_tmpB8A->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB8A->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB8A)))))));}
goto _LL23F;}case 160: _LL37A: _LL37B: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1959 "parse.y"
{struct Cyc_List_List*_tmpB8B;yyval=Cyc_YY34(((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB8B->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB8B)))))));}
goto _LL23F;}case 161: _LL37C: _LL37D:
# 1962
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL23F;case 162: _LL37E: _LL37F: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1964 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL23F;}case 163: _LL380: _LL381: {
# 1967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1965 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL23F;}case 164: _LL382: _LL383: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1969 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpB8C;yyval=Cyc_YY33(((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB8C->typs=0,((_tmpB8C->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB8C->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB8C)))))))))));}
goto _LL23F;}case 165: _LL384: _LL385: {
# 1972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1971 "parse.y"
struct Cyc_List_List*_tmp5A6=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB8D;yyval=Cyc_YY33(((_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB8D->typs=_tmp5A6,((_tmpB8D->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB8D->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB8D)))))))))));}
goto _LL23F;}case 166: _LL386: _LL387: {
# 1975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1977 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 167: _LL388: _LL389: {
# 1980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1979 "parse.y"
struct Cyc_Parse_Declarator _tmp5A8=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB8E;yyval=Cyc_YY27(((_tmpB8E.id=_tmp5A8.id,((_tmpB8E.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5A8.tms),_tmpB8E)))));}
goto _LL23F;}case 168: _LL38A: _LL38B: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
goto _LL23F;}case 169: _LL38C: _LL38D: {
# 1989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1988 "parse.y"
struct Cyc_Parse_Declarator _tmp5AA=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB8F;yyval=Cyc_YY27(((_tmpB8F.id=_tmp5AA.id,((_tmpB8F.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5AA.tms),_tmpB8F)))));}
goto _LL23F;}case 170: _LL38E: _LL38F: {
# 1992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1994 "parse.y"
{struct Cyc_Parse_Declarator _tmpB90;yyval=Cyc_YY27(((_tmpB90.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB90.tms=0,_tmpB90)))));}
goto _LL23F;}case 171: _LL390: _LL391: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1996 "parse.y"
yyval=yyyyvsp[1];
goto _LL23F;}case 172: _LL392: _LL393: {
# 1999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2000 "parse.y"
struct Cyc_Parse_Declarator _tmp5AD=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB96;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB95;struct Cyc_List_List*_tmpB94;_tmp5AD.tms=((_tmpB94=_region_malloc(yyr,sizeof(*_tmpB94)),((_tmpB94->hd=(void*)((_tmpB96=_region_malloc(yyr,sizeof(*_tmpB96)),((_tmpB96[0]=((_tmpB95.tag=5,((_tmpB95.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB95.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpB95)))))),_tmpB96)))),((_tmpB94->tl=_tmp5AD.tms,_tmpB94))))));}
yyval=yyyyvsp[2];
# 2004
goto _LL23F;}case 173: _LL394: _LL395: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2005 "parse.y"
{struct Cyc_List_List*_tmpBA0;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB9F;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB9E;struct Cyc_Parse_Declarator _tmpB9D;yyval=Cyc_YY27(((_tmpB9D.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpB9D.tms=((_tmpBA0=_region_malloc(yyr,sizeof(*_tmpBA0)),((_tmpBA0->hd=(void*)((_tmpB9E=_region_malloc(yyr,sizeof(*_tmpB9E)),((_tmpB9E[0]=((_tmpB9F.tag=0,((_tmpB9F.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpB9F.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB9F)))))),_tmpB9E)))),((_tmpBA0->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBA0)))))),_tmpB9D)))));}
goto _LL23F;}case 174: _LL396: _LL397: {
# 2008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2007 "parse.y"
{struct Cyc_List_List*_tmpBAA;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBA9;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBA8;struct Cyc_Parse_Declarator _tmpBA7;yyval=Cyc_YY27(((_tmpBA7.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBA7.tms=(
(_tmpBAA=_region_malloc(yyr,sizeof(*_tmpBAA)),((_tmpBAA->hd=(void*)((_tmpBA8=_region_malloc(yyr,sizeof(*_tmpBA8)),((_tmpBA8[0]=((_tmpBA9.tag=1,((_tmpBA9.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBA9.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBA9.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBA9)))))))),_tmpBA8)))),((_tmpBAA->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBAA)))))),_tmpBA7)))));}
goto _LL23F;}case 175: _LL398: _LL399: {
# 2011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2010 "parse.y"
struct _tuple26*_tmp5B9=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp5BB;int _tmp5BC;struct Cyc_Absyn_VarargInfo*_tmp5BD;void*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct _tuple26*_tmp5BA=_tmp5B9;_tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BA->f2;_tmp5BD=_tmp5BA->f3;_tmp5BE=_tmp5BA->f4;_tmp5BF=_tmp5BA->f5;{
struct Cyc_Absyn_Exp*_tmp5C0=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5C1=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_List_List*_tmpBBF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBBE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBBD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBBC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBBB;struct Cyc_Parse_Declarator _tmpBBA;yyval=Cyc_YY27(((_tmpBBA.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBBA.tms=((_tmpBBF=_region_malloc(yyr,sizeof(*_tmpBBF)),((_tmpBBF->hd=(void*)((_tmpBBB=_region_malloc(yyr,sizeof(*_tmpBBB)),((_tmpBBB[0]=((_tmpBBE.tag=3,((_tmpBBE.f1=(void*)((_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC[0]=((_tmpBBD.tag=1,((_tmpBBD.f1=_tmp5BB,((_tmpBBD.f2=_tmp5BC,((_tmpBBD.f3=_tmp5BD,((_tmpBBD.f4=_tmp5BE,((_tmpBBD.f5=_tmp5BF,((_tmpBBD.f6=_tmp5C0,((_tmpBBD.f7=_tmp5C1,_tmpBBD)))))))))))))))),_tmpBBC)))),_tmpBBE)))),_tmpBBB)))),((_tmpBBF->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBBF)))))),_tmpBBA)))));}
# 2015
goto _LL23F;};}case 176: _LL39A: _LL39B: {
# 2017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2016 "parse.y"
{struct Cyc_List_List*_tmpBD4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBD3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBD2;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBD1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBD0;struct Cyc_Parse_Declarator _tmpBCF;yyval=Cyc_YY27(((_tmpBCF.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBCF.tms=(
(_tmpBD4=_region_malloc(yyr,sizeof(*_tmpBD4)),((_tmpBD4->hd=(void*)((_tmpBD0=_region_malloc(yyr,sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBD3.tag=3,((_tmpBD3.f1=(void*)((_tmpBD1=_region_malloc(yyr,sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD2.tag=1,((_tmpBD2.f1=0,((_tmpBD2.f2=0,((_tmpBD2.f3=0,((_tmpBD2.f4=
# 2019
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpBD2.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpBD2.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpBD2.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpBD2)))))))))))))))),_tmpBD1)))),_tmpBD3)))),_tmpBD0)))),((_tmpBD4->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBD4)))))),_tmpBCF)))));}
# 2022
goto _LL23F;}case 177: _LL39C: _LL39D: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2023 "parse.y"
{struct Cyc_List_List*_tmpBE9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE8;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBE7;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBE6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE5;struct Cyc_Parse_Declarator _tmpBE4;yyval=Cyc_YY27(((_tmpBE4.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBE4.tms=((_tmpBE9=_region_malloc(yyr,sizeof(*_tmpBE9)),((_tmpBE9->hd=(void*)((_tmpBE5=_region_malloc(yyr,sizeof(*_tmpBE5)),((_tmpBE5[0]=((_tmpBE8.tag=3,((_tmpBE8.f1=(void*)((_tmpBE6=_region_malloc(yyr,sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE7.tag=0,((_tmpBE7.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpBE7.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBE7)))))),_tmpBE6)))),_tmpBE8)))),_tmpBE5)))),((_tmpBE9->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBE9)))))),_tmpBE4)))));}
goto _LL23F;}case 178: _LL39E: _LL39F: {
# 2026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2026 "parse.y"
struct Cyc_List_List*_tmp5D4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBF3;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBF2;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBF1;struct Cyc_Parse_Declarator _tmpBF0;yyval=Cyc_YY27(((_tmpBF0.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBF0.tms=((_tmpBF3=_region_malloc(yyr,sizeof(*_tmpBF3)),((_tmpBF3->hd=(void*)((_tmpBF1=_region_malloc(yyr,sizeof(*_tmpBF1)),((_tmpBF1[0]=((_tmpBF2.tag=4,((_tmpBF2.f1=_tmp5D4,((_tmpBF2.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBF2.f3=0,_tmpBF2)))))))),_tmpBF1)))),((_tmpBF3->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBF3)))))),_tmpBF0)))));}
# 2029
goto _LL23F;}case 179: _LL3A0: _LL3A1: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2030 "parse.y"
{struct Cyc_List_List*_tmpBFD;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBFC;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBFB;struct Cyc_Parse_Declarator _tmpBFA;yyval=Cyc_YY27(((_tmpBFA.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBFA.tms=((_tmpBFD=_region_malloc(yyr,sizeof(*_tmpBFD)),((_tmpBFD->hd=(void*)((_tmpBFB=_region_malloc(yyr,sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFC.tag=5,((_tmpBFC.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBFC.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBFC)))))),_tmpBFB)))),((_tmpBFD->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBFD)))))),_tmpBFA)))));}
# 2033
goto _LL23F;}case 180: _LL3A2: _LL3A3: {
# 2035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2044 "parse.y"
{struct Cyc_Parse_Declarator _tmpBFE;yyval=Cyc_YY27(((_tmpBFE.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBFE.tms=0,_tmpBFE)))));}
goto _LL23F;}case 181: _LL3A4: _LL3A5: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2046 "parse.y"
{struct Cyc_Parse_Declarator _tmpBFF;yyval=Cyc_YY27(((_tmpBFF.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBFF.tms=0,_tmpBFF)))));}
goto _LL23F;}case 182: _LL3A6: _LL3A7: {
# 2049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2048 "parse.y"
yyval=yyyyvsp[1];
goto _LL23F;}case 183: _LL3A8: _LL3A9: {
# 2051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2052 "parse.y"
struct Cyc_Parse_Declarator _tmp5DF=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC05;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC04;struct Cyc_List_List*_tmpC03;_tmp5DF.tms=((_tmpC03=_region_malloc(yyr,sizeof(*_tmpC03)),((_tmpC03->hd=(void*)((_tmpC05=_region_malloc(yyr,sizeof(*_tmpC05)),((_tmpC05[0]=((_tmpC04.tag=5,((_tmpC04.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC04.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC04)))))),_tmpC05)))),((_tmpC03->tl=_tmp5DF.tms,_tmpC03))))));}
yyval=yyyyvsp[2];
# 2056
goto _LL23F;}case 184: _LL3AA: _LL3AB: {
# 2058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2057 "parse.y"
struct Cyc_Parse_Declarator _tmp5E3=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC0F;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC0E;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC0D;struct Cyc_Parse_Declarator _tmpC0C;yyval=Cyc_YY27(((_tmpC0C.id=_tmp5E3.id,((_tmpC0C.tms=(
(_tmpC0F=_region_malloc(yyr,sizeof(*_tmpC0F)),((_tmpC0F->hd=(void*)((_tmpC0D=_region_malloc(yyr,sizeof(*_tmpC0D)),((_tmpC0D[0]=((_tmpC0E.tag=0,((_tmpC0E.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpC0E.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC0E)))))),_tmpC0D)))),((_tmpC0F->tl=_tmp5E3.tms,_tmpC0F)))))),_tmpC0C)))));}
goto _LL23F;}case 185: _LL3AC: _LL3AD: {
# 2062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2061 "parse.y"
struct Cyc_Parse_Declarator _tmp5E8=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC19;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC18;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC17;struct Cyc_Parse_Declarator _tmpC16;yyval=Cyc_YY27(((_tmpC16.id=_tmp5E8.id,((_tmpC16.tms=(
(_tmpC19=_region_malloc(yyr,sizeof(*_tmpC19)),((_tmpC19->hd=(void*)((_tmpC17=_region_malloc(yyr,sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC18.tag=1,((_tmpC18.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpC18.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC18.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC18)))))))),_tmpC17)))),((_tmpC19->tl=_tmp5E8.tms,_tmpC19)))))),_tmpC16)))));}
# 2065
goto _LL23F;}case 186: _LL3AE: _LL3AF: {
# 2067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2066 "parse.y"
struct _tuple26*_tmp5ED=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp5EF;int _tmp5F0;struct Cyc_Absyn_VarargInfo*_tmp5F1;void*_tmp5F2;struct Cyc_List_List*_tmp5F3;struct _tuple26*_tmp5EE=_tmp5ED;_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;_tmp5F1=_tmp5EE->f3;_tmp5F2=_tmp5EE->f4;_tmp5F3=_tmp5EE->f5;{
struct Cyc_Absyn_Exp*_tmp5F4=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5F5=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp5F6=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC2E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC2D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC2C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC2B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC2A;struct Cyc_Parse_Declarator _tmpC29;yyval=Cyc_YY27(((_tmpC29.id=_tmp5F6.id,((_tmpC29.tms=((_tmpC2E=_region_malloc(yyr,sizeof(*_tmpC2E)),((_tmpC2E->hd=(void*)((_tmpC2A=_region_malloc(yyr,sizeof(*_tmpC2A)),((_tmpC2A[0]=((_tmpC2D.tag=3,((_tmpC2D.f1=(void*)((_tmpC2B=_region_malloc(yyr,sizeof(*_tmpC2B)),((_tmpC2B[0]=((_tmpC2C.tag=1,((_tmpC2C.f1=_tmp5EF,((_tmpC2C.f2=_tmp5F0,((_tmpC2C.f3=_tmp5F1,((_tmpC2C.f4=_tmp5F2,((_tmpC2C.f5=_tmp5F3,((_tmpC2C.f6=_tmp5F4,((_tmpC2C.f7=_tmp5F5,_tmpC2C)))))))))))))))),_tmpC2B)))),_tmpC2D)))),_tmpC2A)))),((_tmpC2E->tl=_tmp5F6.tms,_tmpC2E)))))),_tmpC29)))));}
# 2072
goto _LL23F;};}case 187: _LL3B0: _LL3B1: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2073 "parse.y"
struct Cyc_Parse_Declarator _tmp5FD=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC43;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC42;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC41;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC40;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC3F;struct Cyc_Parse_Declarator _tmpC3E;yyval=Cyc_YY27(((_tmpC3E.id=_tmp5FD.id,((_tmpC3E.tms=(
(_tmpC43=_region_malloc(yyr,sizeof(*_tmpC43)),((_tmpC43->hd=(void*)((_tmpC3F=_region_malloc(yyr,sizeof(*_tmpC3F)),((_tmpC3F[0]=((_tmpC42.tag=3,((_tmpC42.f1=(void*)((_tmpC40=_region_malloc(yyr,sizeof(*_tmpC40)),((_tmpC40[0]=((_tmpC41.tag=1,((_tmpC41.f1=0,((_tmpC41.f2=0,((_tmpC41.f3=0,((_tmpC41.f4=
# 2077
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC41.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC41.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC41.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC41)))))))))))))))),_tmpC40)))),_tmpC42)))),_tmpC3F)))),((_tmpC43->tl=_tmp5FD.tms,_tmpC43)))))),_tmpC3E)))));}
# 2080
goto _LL23F;}case 188: _LL3B2: _LL3B3: {
# 2082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2081 "parse.y"
struct Cyc_Parse_Declarator _tmp604=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC58;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC57;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC56;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC55;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC54;struct Cyc_Parse_Declarator _tmpC53;yyval=Cyc_YY27(((_tmpC53.id=_tmp604.id,((_tmpC53.tms=((_tmpC58=_region_malloc(yyr,sizeof(*_tmpC58)),((_tmpC58->hd=(void*)((_tmpC54=_region_malloc(yyr,sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC57.tag=3,((_tmpC57.f1=(void*)((_tmpC55=_region_malloc(yyr,sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=0,((_tmpC56.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC56.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC56)))))),_tmpC55)))),_tmpC57)))),_tmpC54)))),((_tmpC58->tl=_tmp604.tms,_tmpC58)))))),_tmpC53)))));}
goto _LL23F;}case 189: _LL3B4: _LL3B5: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2085 "parse.y"
struct Cyc_List_List*_tmp60B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp60C=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC62;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC61;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC60;struct Cyc_Parse_Declarator _tmpC5F;yyval=Cyc_YY27(((_tmpC5F.id=_tmp60C.id,((_tmpC5F.tms=((_tmpC62=_region_malloc(yyr,sizeof(*_tmpC62)),((_tmpC62->hd=(void*)((_tmpC60=_region_malloc(yyr,sizeof(*_tmpC60)),((_tmpC60[0]=((_tmpC61.tag=4,((_tmpC61.f1=_tmp60B,((_tmpC61.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC61.f3=0,_tmpC61)))))))),_tmpC60)))),((_tmpC62->tl=_tmp60C.tms,_tmpC62)))))),_tmpC5F)))));}
# 2089
goto _LL23F;}case 190: _LL3B6: _LL3B7: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2090 "parse.y"
struct Cyc_Parse_Declarator _tmp611=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC6C;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC6B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC6A;struct Cyc_Parse_Declarator _tmpC69;yyval=Cyc_YY27(((_tmpC69.id=_tmp611.id,((_tmpC69.tms=((_tmpC6C=_region_malloc(yyr,sizeof(*_tmpC6C)),((_tmpC6C->hd=(void*)((_tmpC6A=_region_malloc(yyr,sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6B.tag=5,((_tmpC6B.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC6B.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC6B)))))),_tmpC6A)))),((_tmpC6C->tl=_tmp611.tms,_tmpC6C)))))),_tmpC69)))));}
# 2093
goto _LL23F;}case 191: _LL3B8: _LL3B9: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2103 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 192: _LL3BA: _LL3BB: {
# 2106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2104 "parse.y"
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
goto _LL23F;}case 193: _LL3BC: _LL3BD: {
# 2107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2108 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC72;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC71;struct Cyc_List_List*_tmpC70;ans=((_tmpC70=_region_malloc(yyr,sizeof(*_tmpC70)),((_tmpC70->hd=(void*)((_tmpC72=_region_malloc(yyr,sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC71.tag=5,((_tmpC71.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC71.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpC71)))))),_tmpC72)))),((_tmpC70->tl=ans,_tmpC70))))));}{
# 2112
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp619=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp61B;union Cyc_Absyn_Constraint*_tmp61C;union Cyc_Absyn_Constraint*_tmp61D;struct _tuple21 _tmp61A=_tmp619;_tmp61B=_tmp61A.f1;_tmp61C=_tmp61A.f2;_tmp61D=_tmp61A.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC73;ptrloc=((_tmpC73=_cycalloc_atomic(sizeof(*_tmpC73)),((_tmpC73->ptr_loc=_tmp61B,((_tmpC73->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC73->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC73))))))));}{
# 2117
struct _tuple14 _tmp61F=Cyc_Parse_collapse_pointer_quals(_tmp61B,_tmp61C,_tmp61D,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp621;union Cyc_Absyn_Constraint*_tmp622;union Cyc_Absyn_Constraint*_tmp623;void*_tmp624;struct _tuple14 _tmp620=_tmp61F;_tmp621=_tmp620.f1;_tmp622=_tmp620.f2;_tmp623=_tmp620.f3;_tmp624=_tmp620.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC7D;struct Cyc_Absyn_PtrAtts _tmpC7C;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC7B;struct Cyc_List_List*_tmpC7A;ans=((_tmpC7A=_region_malloc(yyr,sizeof(*_tmpC7A)),((_tmpC7A->hd=(void*)((_tmpC7D=_region_malloc(yyr,sizeof(*_tmpC7D)),((_tmpC7D[0]=((_tmpC7B.tag=2,((_tmpC7B.f1=((_tmpC7C.rgn=_tmp624,((_tmpC7C.nullable=_tmp621,((_tmpC7C.bounds=_tmp622,((_tmpC7C.zero_term=_tmp623,((_tmpC7C.ptrloc=ptrloc,_tmpC7C)))))))))),((_tmpC7B.f2=Cyc_yyget_YY23(yyyyvsp[4]),_tmpC7B)))))),_tmpC7D)))),((_tmpC7A->tl=ans,_tmpC7A))))));}
yyval=Cyc_YY26(ans);
# 2121
goto _LL23F;};};}case 194: _LL3BE: _LL3BF:
# 2123
 yyval=Cyc_YY54(0);
goto _LL23F;case 195: _LL3C0: _LL3C1: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2124 "parse.y"
{struct Cyc_List_List*_tmpC7E;yyval=Cyc_YY54(((_tmpC7E=_region_malloc(yyr,sizeof(*_tmpC7E)),((_tmpC7E->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpC7E->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpC7E)))))));}
goto _LL23F;}case 196: _LL3C2: _LL3C3: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2129 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC81;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC80;yyval=Cyc_YY53((void*)((_tmpC80=_region_malloc(yyr,sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC81.tag=0,((_tmpC81.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpC81)))),_tmpC80)))));}
goto _LL23F;}case 197: _LL3C4: _LL3C5: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2131 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC84;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC83;yyval=Cyc_YY53((void*)((_tmpC83=_region_malloc(yyr,sizeof(*_tmpC83)),((_tmpC83[0]=((_tmpC84.tag=1,((_tmpC84.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpC84)))),_tmpC83)))));}
goto _LL23F;}case 198: _LL3C6: _LL3C7: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2133 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC87;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC86;yyval=Cyc_YY53((void*)((_tmpC86=_region_malloc(yyr,sizeof(*_tmpC86)),((_tmpC86[0]=((_tmpC87.tag=2,_tmpC87)),_tmpC86)))));}
goto _LL23F;}case 199: _LL3C8: _LL3C9: {
# 2136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2135 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC8A;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC89;yyval=Cyc_YY53((void*)((_tmpC89=_region_malloc(yyr,sizeof(*_tmpC89)),((_tmpC89[0]=((_tmpC8A.tag=3,_tmpC8A)),_tmpC89)))));}
goto _LL23F;}case 200: _LL3CA: _LL3CB: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2137 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC8D;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC8C;yyval=Cyc_YY53((void*)((_tmpC8C=_region_malloc(yyr,sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8D.tag=4,_tmpC8D)),_tmpC8C)))));}
goto _LL23F;}case 201: _LL3CC: _LL3CD: {
# 2140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2139 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC90;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC8F;yyval=Cyc_YY53((void*)((_tmpC8F=_region_malloc(yyr,sizeof(*_tmpC8F)),((_tmpC8F[0]=((_tmpC90.tag=5,_tmpC90)),_tmpC8F)))));}
goto _LL23F;}case 202: _LL3CE: _LL3CF: {
# 2142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2141 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC93;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC92;yyval=Cyc_YY53((void*)((_tmpC92=_region_malloc(yyr,sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC93.tag=6,_tmpC93)),_tmpC92)))));}
goto _LL23F;}case 203: _LL3D0: _LL3D1: {
# 2144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2143 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC96;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC95;yyval=Cyc_YY53((void*)((_tmpC95=_region_malloc(yyr,sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC96.tag=7,_tmpC96)),_tmpC95)))));}
goto _LL23F;}case 204: _LL3D2: _LL3D3: {
# 2146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2149 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC97;yyval=Cyc_YY1(((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97->f1=loc,((_tmpC97->f2=Cyc_Absyn_true_conref,((_tmpC97->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC97)))))))));}
# 2152
goto _LL23F;}case 205: _LL3D4: _LL3D5: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2153 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC98;yyval=Cyc_YY1(((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98->f1=loc,((_tmpC98->f2=Cyc_Absyn_false_conref,((_tmpC98->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC98)))))))));}
# 2156
goto _LL23F;}case 206: _LL3D6: _LL3D7: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2157 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC99;yyval=Cyc_YY1(((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->f1=loc,((_tmpC99->f2=Cyc_Absyn_true_conref,((_tmpC99->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC99)))))))));}
# 2160
goto _LL23F;}case 207: _LL3D8: _LL3D9:
# 2162
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
goto _LL23F;case 208: _LL3DA: _LL3DB: {
# 2165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2163 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC9C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC9B;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=1,((_tmpC9C.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpC9C)))),_tmpC9B))))));}
goto _LL23F;}case 209: _LL3DC: _LL3DD:
# 2166
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
goto _LL23F;case 210: _LL3DE: _LL3DF: {
# 2169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2167 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
goto _LL23F;}case 211: _LL3E0: _LL3E1: {
# 2170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2168 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
goto _LL23F;}case 212: _LL3E2: _LL3E3:
# 2171
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL23F;case 213: _LL3E4: _LL3E5: {
# 2174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2174 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL23F;}case 214: _LL3E6: _LL3E7: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2175 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL23F;}case 215: _LL3E8: _LL3E9:
# 2178
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
goto _LL23F;case 216: _LL3EA: _LL3EB: {
# 2181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2180 "parse.y"
yyval=Cyc_YY23(Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),Cyc_yyget_YY23(yyyyvsp[1])));
goto _LL23F;}case 217: _LL3EC: _LL3ED: {
# 2183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2185 "parse.y"
{struct _tuple26*_tmpC9D;yyval=Cyc_YY39(((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC9D->f2=0,((_tmpC9D->f3=0,((_tmpC9D->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpC9D->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpC9D)))))))))))));}
goto _LL23F;}case 218: _LL3EE: _LL3EF: {
# 2188
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2187 "parse.y"
{struct _tuple26*_tmpC9E;yyval=Cyc_YY39(((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC9E->f2=1,((_tmpC9E->f3=0,((_tmpC9E->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpC9E->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC9E)))))))))))));}
goto _LL23F;}case 219: _LL3F0: _LL3F1: {
# 2190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2190 "parse.y"
struct _tuple8*_tmp641=Cyc_yyget_YY37(yyyyvsp[2]);struct _dyneither_ptr*_tmp643;struct Cyc_Absyn_Tqual _tmp644;void*_tmp645;struct _tuple8*_tmp642=_tmp641;_tmp643=_tmp642->f1;_tmp644=_tmp642->f2;_tmp645=_tmp642->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC9F;struct Cyc_Absyn_VarargInfo*_tmp646=(_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F->name=_tmp643,((_tmpC9F->tq=_tmp644,((_tmpC9F->type=_tmp645,((_tmpC9F->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpC9F)))))))));
{struct _tuple26*_tmpCA0;yyval=Cyc_YY39(((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0->f1=0,((_tmpCA0->f2=0,((_tmpCA0->f3=_tmp646,((_tmpCA0->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCA0->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCA0)))))))))))));}
# 2194
goto _LL23F;};}case 220: _LL3F2: _LL3F3: {
# 2196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2196 "parse.y"
struct _tuple8*_tmp649=Cyc_yyget_YY37(yyyyvsp[4]);struct _dyneither_ptr*_tmp64B;struct Cyc_Absyn_Tqual _tmp64C;void*_tmp64D;struct _tuple8*_tmp64A=_tmp649;_tmp64B=_tmp64A->f1;_tmp64C=_tmp64A->f2;_tmp64D=_tmp64A->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCA1;struct Cyc_Absyn_VarargInfo*_tmp64E=(_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->name=_tmp64B,((_tmpCA1->tq=_tmp64C,((_tmpCA1->type=_tmp64D,((_tmpCA1->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpCA1)))))))));
{struct _tuple26*_tmpCA2;yyval=Cyc_YY39(((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCA2->f2=0,((_tmpCA2->f3=_tmp64E,((_tmpCA2->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpCA2->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpCA2)))))))))))));}
# 2200
goto _LL23F;};}case 221: _LL3F4: _LL3F5: {
# 2202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2204 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCA5;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCA4;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA5.tag=1,((_tmpCA5.f1=0,_tmpCA5)))),_tmpCA4))))));}
goto _LL23F;}case 222: _LL3F6: _LL3F7: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2205 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
goto _LL23F;}case 223: _LL3F8: _LL3F9:
# 2208
 yyval=Cyc_YY49(0);
goto _LL23F;case 224: _LL3FA: _LL3FB: {
# 2211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2209 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCA8;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCA7;yyval=Cyc_YY49((void*)((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7[0]=((_tmpCA8.tag=24,((_tmpCA8.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCA8)))),_tmpCA7)))));}
goto _LL23F;}case 225: _LL3FC: _LL3FD:
# 2212
 yyval=Cyc_YY50(0);
goto _LL23F;case 226: _LL3FE: _LL3FF: {
# 2215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2214 "parse.y"
yyval=yyyyvsp[1];
goto _LL23F;}case 227: _LL400: _LL401: {
# 2217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2222 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCAB;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCAA;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp655=(_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA[0]=((_tmpCAB.tag=2,((_tmpCAB.f1=0,((_tmpCAB.f2=& Cyc_Tcutil_trk,_tmpCAB)))))),_tmpCAA)));
struct _dyneither_ptr _tmp656=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp657=Cyc_Parse_id2type(_tmp656,(void*)_tmp655);
{struct _tuple29*_tmpCB5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCB4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCB3;struct Cyc_List_List*_tmpCB2;yyval=Cyc_YY50(((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->hd=((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->f1=(void*)((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3[0]=((_tmpCB4.tag=24,((_tmpCB4.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCB4)))),_tmpCB3)))),((_tmpCB5->f2=_tmp657,_tmpCB5)))))),((_tmpCB2->tl=0,_tmpCB2)))))));}
# 2227
goto _LL23F;}case 228: _LL402: _LL403: {
# 2229
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2231
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2229 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCB8;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp65E=(_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7[0]=((_tmpCB8.tag=2,((_tmpCB8.f1=0,((_tmpCB8.f2=& Cyc_Tcutil_trk,_tmpCB8)))))),_tmpCB7)));
struct _dyneither_ptr _tmp65F=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp660=Cyc_Parse_id2type(_tmp65F,(void*)_tmp65E);
{struct _tuple29*_tmpCC2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCC1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCC0;struct Cyc_List_List*_tmpCBF;yyval=Cyc_YY50(((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->hd=((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=(void*)((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0[0]=((_tmpCC1.tag=24,((_tmpCC1.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCC1)))),_tmpCC0)))),((_tmpCC2->f2=_tmp660,_tmpCC2)))))),((_tmpCBF->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCBF)))))));}
# 2234
goto _LL23F;}case 229: _LL404: _LL405:
# 2236
 yyval=Cyc_YY31(0);
goto _LL23F;case 230: _LL406: _LL407: {
# 2239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2240 "parse.y"
{const char*_tmpCC3;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCC3="inject",_tag_dyneither(_tmpCC3,sizeof(char),7))))!= 0){
const char*_tmpCC4;Cyc_Parse_err(((_tmpCC4="missing type in function declaration",_tag_dyneither(_tmpCC4,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY31(1);
# 2244
goto _LL23F;}case 231: _LL408: _LL409: {
# 2246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2247 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 232: _LL40A: _LL40B: {
# 2250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2248 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
goto _LL23F;}case 233: _LL40C: _LL40D: {
# 2251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2252 "parse.y"
yyval=Cyc_YY40(0);
goto _LL23F;}case 234: _LL40E: _LL40F: {
# 2255
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2253 "parse.y"
yyval=yyyyvsp[1];
goto _LL23F;}case 235: _LL410: _LL411: {
# 2256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2255 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCCA;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCC9;struct Cyc_List_List*_tmpCC8;yyval=Cyc_YY40(((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->hd=(void*)((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCC9.tag=25,((_tmpCC9.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCC9)))),_tmpCCA)))),((_tmpCC8->tl=0,_tmpCC8)))))));}
goto _LL23F;}case 236: _LL412: _LL413: {
# 2258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2257 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCCB;yyval=Cyc_YY40(((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCCB->tl=0,_tmpCCB)))))));}
# 2260
goto _LL23F;}case 237: _LL414: _LL415: {
# 2262
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2264
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2277 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCD1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCD0;struct Cyc_List_List*_tmpCCF;yyval=Cyc_YY40(((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->hd=(void*)((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1[0]=((_tmpCD0.tag=23,((_tmpCD0.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCD0)))),_tmpCD1)))),((_tmpCCF->tl=0,_tmpCCF)))))));}
goto _LL23F;}case 238: _LL416: _LL417: {
# 2280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2279 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCD7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCD6;struct Cyc_List_List*_tmpCD5;yyval=Cyc_YY40(((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=(void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7[0]=((_tmpCD6.tag=23,((_tmpCD6.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCD6)))),_tmpCD7)))),((_tmpCD5->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpCD5)))))));}
goto _LL23F;}case 239: _LL418: _LL419: {
# 2282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCD8;yyval=Cyc_YY38(((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpCD8->tl=0,_tmpCD8)))))));}
goto _LL23F;}case 240: _LL41A: _LL41B: {
# 2288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2287 "parse.y"
{struct Cyc_List_List*_tmpCD9;yyval=Cyc_YY38(((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpCD9->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpCD9)))))));}
goto _LL23F;}case 241: _LL41C: _LL41D: {
# 2290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp675=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp677;struct Cyc_Parse_Type_specifier _tmp678;struct Cyc_List_List*_tmp679;struct _tuple25 _tmp676=_tmp675;_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;
if(_tmp677.loc == 0)_tmp677.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp67A=Cyc_yyget_YY27(yyyyvsp[1]);struct _tuple0*_tmp67C;struct Cyc_List_List*_tmp67D;struct Cyc_Parse_Declarator _tmp67B=_tmp67A;_tmp67C=_tmp67B.id;_tmp67D=_tmp67B.tms;{
void*_tmp67E=Cyc_Parse_speclist2typ(_tmp678,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp67F=Cyc_Parse_apply_tms(_tmp677,_tmp67E,_tmp679,_tmp67D);struct Cyc_Absyn_Tqual _tmp681;void*_tmp682;struct Cyc_List_List*_tmp683;struct Cyc_List_List*_tmp684;struct _tuple13 _tmp680=_tmp67F;_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;_tmp684=_tmp680.f4;
if(_tmp683 != 0){
const char*_tmpCDA;Cyc_Parse_err(((_tmpCDA="parameter with bad type params",_tag_dyneither(_tmpCDA,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp67C)){
const char*_tmpCDB;Cyc_Parse_err(((_tmpCDB="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCDB,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp67C).f2;
if(_tmp684 != 0){
const char*_tmpCDE;void*_tmpCDD;(_tmpCDD=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCDE="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCDE,sizeof(char),40))),_tag_dyneither(_tmpCDD,sizeof(void*),0)));}
{struct _tuple8*_tmpCDF;yyval=Cyc_YY37(((_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->f1=idopt,((_tmpCDF->f2=_tmp681,((_tmpCDF->f3=_tmp682,_tmpCDF)))))))));}
# 2307
goto _LL23F;};};};}case 242: _LL41E: _LL41F: {
# 2309
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2308 "parse.y"
struct _tuple25 _tmp68A=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp68C;struct Cyc_Parse_Type_specifier _tmp68D;struct Cyc_List_List*_tmp68E;struct _tuple25 _tmp68B=_tmp68A;_tmp68C=_tmp68B.f1;_tmp68D=_tmp68B.f2;_tmp68E=_tmp68B.f3;
if(_tmp68C.loc == 0)_tmp68C.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp68F=Cyc_Parse_speclist2typ(_tmp68D,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp68E != 0){
const char*_tmpCE2;void*_tmpCE1;(_tmpCE1=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE2="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCE2,sizeof(char),38))),_tag_dyneither(_tmpCE1,sizeof(void*),0)));}
{struct _tuple8*_tmpCE3;yyval=Cyc_YY37(((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->f1=0,((_tmpCE3->f2=_tmp68C,((_tmpCE3->f3=_tmp68F,_tmpCE3)))))))));}
# 2315
goto _LL23F;};}case 243: _LL420: _LL421: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2316 "parse.y"
struct _tuple25 _tmp693=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp695;struct Cyc_Parse_Type_specifier _tmp696;struct Cyc_List_List*_tmp697;struct _tuple25 _tmp694=_tmp693;_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;_tmp697=_tmp694.f3;
if(_tmp695.loc == 0)_tmp695.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp698=Cyc_Parse_speclist2typ(_tmp696,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp699=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp69A=Cyc_Parse_apply_tms(_tmp695,_tmp698,_tmp697,_tmp699);struct Cyc_Absyn_Tqual _tmp69C;void*_tmp69D;struct Cyc_List_List*_tmp69E;struct Cyc_List_List*_tmp69F;struct _tuple13 _tmp69B=_tmp69A;_tmp69C=_tmp69B.f1;_tmp69D=_tmp69B.f2;_tmp69E=_tmp69B.f3;_tmp69F=_tmp69B.f4;
if(_tmp69E != 0){
const char*_tmpCE6;void*_tmpCE5;(_tmpCE5=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE6="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCE6,sizeof(char),49))),_tag_dyneither(_tmpCE5,sizeof(void*),0)));}
# 2324
if(_tmp69F != 0){
const char*_tmpCE9;void*_tmpCE8;(_tmpCE8=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE9="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCE9,sizeof(char),38))),_tag_dyneither(_tmpCE8,sizeof(void*),0)));}
{struct _tuple8*_tmpCEA;yyval=Cyc_YY37(((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->f1=0,((_tmpCEA->f2=_tmp69C,((_tmpCEA->f3=_tmp69D,_tmpCEA)))))))));}
# 2328
goto _LL23F;};}case 244: _LL422: _LL423: {
# 2330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2332 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
goto _LL23F;}case 245: _LL424: _LL425: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2336 "parse.y"
{struct _dyneither_ptr*_tmpCED;struct Cyc_List_List*_tmpCEC;yyval=Cyc_YY36(((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->hd=((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCED)))),((_tmpCEC->tl=0,_tmpCEC)))))));}
goto _LL23F;}case 246: _LL426: _LL427: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2338 "parse.y"
{struct _dyneither_ptr*_tmpCF0;struct Cyc_List_List*_tmpCEF;yyval=Cyc_YY36(((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF->hd=((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCF0)))),((_tmpCEF->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpCEF)))))));}
goto _LL23F;}case 247: _LL428: _LL429: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2342 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 248: _LL42A: _LL42B: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2343 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 249: _LL42C: _LL42D: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2348 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCF3;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCF2;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2[0]=((_tmpCF3.tag=35,((_tmpCF3.f1=0,((_tmpCF3.f2=0,_tmpCF3)))))),_tmpCF2)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 250: _LL42E: _LL42F: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2350 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCF6;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCF5;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF6.tag=35,((_tmpCF6.f1=0,((_tmpCF6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpCF6)))))),_tmpCF5)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 251: _LL430: _LL431: {
# 2353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2352 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCF9;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCF8;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF9.tag=35,((_tmpCF9.f1=0,((_tmpCF9.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpCF9)))))),_tmpCF8)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 252: _LL432: _LL433: {
# 2355
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2354 "parse.y"
struct _dyneither_ptr*_tmpCFC;struct _tuple0*_tmpCFB;struct Cyc_Absyn_Vardecl*_tmp6AF=Cyc_Absyn_new_vardecl(((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB->f1=Cyc_Absyn_Loc_n,((_tmpCFB->f2=((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCFC)))),_tmpCFB)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2357
(_tmp6AF->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCFF;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCFE;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=26,((_tmpCFF.f1=_tmp6AF,((_tmpCFF.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpCFF.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]),((_tmpCFF.f4=0,_tmpCFF)))))))))),_tmpCFE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2360
goto _LL23F;}case 253: _LL434: _LL435: {
# 2362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2361 "parse.y"
void*_tmp6B4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpD02;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD01;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=27,((_tmpD02.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD02.f2=_tmp6B4,((_tmpD02.f3=0,_tmpD02)))))))),_tmpD01)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2364
goto _LL23F;}case 254: _LL436: _LL437: {
# 2366
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD05;struct Cyc_List_List*_tmpD04;yyval=Cyc_YY5(((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->hd=((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->f1=0,((_tmpD05->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]),_tmpD05)))))),((_tmpD04->tl=0,_tmpD04)))))));}
goto _LL23F;}case 255: _LL438: _LL439: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2371 "parse.y"
{struct _tuple30*_tmpD08;struct Cyc_List_List*_tmpD07;yyval=Cyc_YY5(((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07->hd=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpD08->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD08)))))),((_tmpD07->tl=0,_tmpD07)))))));}
goto _LL23F;}case 256: _LL43A: _LL43B: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2373 "parse.y"
{struct _tuple30*_tmpD0B;struct Cyc_List_List*_tmpD0A;yyval=Cyc_YY5(((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->hd=((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->f1=0,((_tmpD0B->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpD0B)))))),((_tmpD0A->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD0A)))))));}
goto _LL23F;}case 257: _LL43C: _LL43D: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2375 "parse.y"
{struct _tuple30*_tmpD0E;struct Cyc_List_List*_tmpD0D;yyval=Cyc_YY5(((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->hd=((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpD0E->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]),_tmpD0E)))))),((_tmpD0D->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD0D)))))));}
goto _LL23F;}case 258: _LL43E: _LL43F: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2379 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
goto _LL23F;}case 259: _LL440: _LL441: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2380 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD18;struct _dyneither_ptr*_tmpD17;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD16;struct Cyc_List_List*_tmpD15;yyval=Cyc_YY41(((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->hd=(void*)((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD16.tag=1,((_tmpD16.f1=((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD17)))),_tmpD16)))),_tmpD18)))),((_tmpD15->tl=0,_tmpD15)))))));}
goto _LL23F;}case 260: _LL442: _LL443: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2385 "parse.y"
{struct Cyc_List_List*_tmpD19;yyval=Cyc_YY41(((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpD19->tl=0,_tmpD19)))))));}
goto _LL23F;}case 261: _LL444: _LL445: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2386 "parse.y"
{struct Cyc_List_List*_tmpD1A;yyval=Cyc_YY41(((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpD1A->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpD1A)))))));}
goto _LL23F;}case 262: _LL446: _LL447: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2390 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD1D;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD1C;yyval=Cyc_YY42((void*)((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[0]=((_tmpD1D.tag=0,((_tmpD1D.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD1D)))),_tmpD1C)))));}
goto _LL23F;}case 263: _LL448: _LL449: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2391 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD23;struct _dyneither_ptr*_tmpD22;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD21;yyval=Cyc_YY42((void*)((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD23.tag=1,((_tmpD23.f1=((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD22)))),_tmpD23)))),_tmpD21)))));}
goto _LL23F;}case 264: _LL44A: _LL44B: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2396 "parse.y"
struct _tuple25 _tmp6CA=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6CC;struct Cyc_Parse_Type_specifier _tmp6CD;struct Cyc_List_List*_tmp6CE;struct _tuple25 _tmp6CB=_tmp6CA;_tmp6CC=_tmp6CB.f1;_tmp6CD=_tmp6CB.f2;_tmp6CE=_tmp6CB.f3;{
void*_tmp6CF=Cyc_Parse_speclist2typ(_tmp6CD,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6CE != 0){
const char*_tmpD26;void*_tmpD25;(_tmpD25=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD26="ignoring attributes in type",_tag_dyneither(_tmpD26,sizeof(char),28))),_tag_dyneither(_tmpD25,sizeof(void*),0)));}
{struct _tuple8*_tmpD27;yyval=Cyc_YY37(((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->f1=0,((_tmpD27->f2=_tmp6CC,((_tmpD27->f3=_tmp6CF,_tmpD27)))))))));}
# 2402
goto _LL23F;};}case 265: _LL44C: _LL44D: {
# 2404
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2403 "parse.y"
struct _tuple25 _tmp6D3=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6D5;struct Cyc_Parse_Type_specifier _tmp6D6;struct Cyc_List_List*_tmp6D7;struct _tuple25 _tmp6D4=_tmp6D3;_tmp6D5=_tmp6D4.f1;_tmp6D6=_tmp6D4.f2;_tmp6D7=_tmp6D4.f3;{
void*_tmp6D8=Cyc_Parse_speclist2typ(_tmp6D6,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6D9=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6DA=Cyc_Parse_apply_tms(_tmp6D5,_tmp6D8,_tmp6D7,_tmp6D9);
if(_tmp6DA.f3 != 0){
# 2409
const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD2A="bad type params, ignoring",_tag_dyneither(_tmpD2A,sizeof(char),26))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}
if(_tmp6DA.f4 != 0){
const char*_tmpD2D;void*_tmpD2C;(_tmpD2C=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD2D="bad specifiers, ignoring",_tag_dyneither(_tmpD2D,sizeof(char),25))),_tag_dyneither(_tmpD2C,sizeof(void*),0)));}
{struct _tuple8*_tmpD2E;yyval=Cyc_YY37(((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->f1=0,((_tmpD2E->f2=_tmp6DA.f1,((_tmpD2E->f3=_tmp6DA.f2,_tmpD2E)))))))));}
# 2414
goto _LL23F;};}case 266: _LL44E: _LL44F: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2417 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 267: _LL450: _LL451: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2418 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD31;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD30;yyval=Cyc_YY44((void*)((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((_tmpD31.tag=24,((_tmpD31.f1=0,_tmpD31)))),_tmpD30)))));}
goto _LL23F;}case 268: _LL452: _LL453: {
# 2421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2419 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD34;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD33;yyval=Cyc_YY44((void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=24,((_tmpD34.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpD34)))),_tmpD33)))));}
goto _LL23F;}case 269: _LL454: _LL455: {
# 2422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2420 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD37;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD36;yyval=Cyc_YY44((void*)((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD37.tag=25,((_tmpD37.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD37)))),_tmpD36)))));}
goto _LL23F;}case 270: _LL456: _LL457: {
# 2423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2421 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD3D;struct Cyc_List_List*_tmpD3C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD3B;yyval=Cyc_YY44((void*)((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B[0]=((_tmpD3D.tag=24,((_tmpD3D.f1=((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD3C->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD3C)))))),_tmpD3D)))),_tmpD3B)))));}
goto _LL23F;}case 271: _LL458: _LL459: {
# 2424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD3E;yyval=Cyc_YY40(((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD3E->tl=0,_tmpD3E)))))));}
goto _LL23F;}case 272: _LL45A: _LL45B: {
# 2430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2428 "parse.y"
{struct Cyc_List_List*_tmpD3F;yyval=Cyc_YY40(((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD3F->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD3F)))))));}
goto _LL23F;}case 273: _LL45C: _LL45D: {
# 2431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2433 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD40;yyval=Cyc_YY30(((_tmpD40.tms=Cyc_yyget_YY26(yyyyvsp[0]),_tmpD40)));}
goto _LL23F;}case 274: _LL45E: _LL45F: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2435 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 275: _LL460: _LL461: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2437 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD41;yyval=Cyc_YY30(((_tmpD41.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpD41)));}
goto _LL23F;}case 276: _LL462: _LL463: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2442 "parse.y"
yyval=yyyyvsp[1];
goto _LL23F;}case 277: _LL464: _LL465: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2444 "parse.y"
{struct Cyc_List_List*_tmpD4B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD4A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD49;struct Cyc_Parse_Abstractdeclarator _tmpD48;yyval=Cyc_YY30(((_tmpD48.tms=((_tmpD4B=_region_malloc(yyr,sizeof(*_tmpD4B)),((_tmpD4B->hd=(void*)((_tmpD49=_region_malloc(yyr,sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD4A.tag=0,((_tmpD4A.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD4A.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD4A)))))),_tmpD49)))),((_tmpD4B->tl=0,_tmpD4B)))))),_tmpD48)));}
goto _LL23F;}case 278: _LL466: _LL467: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2446 "parse.y"
{struct Cyc_List_List*_tmpD55;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD54;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD53;struct Cyc_Parse_Abstractdeclarator _tmpD52;yyval=Cyc_YY30(((_tmpD52.tms=((_tmpD55=_region_malloc(yyr,sizeof(*_tmpD55)),((_tmpD55->hd=(void*)((_tmpD53=_region_malloc(yyr,sizeof(*_tmpD53)),((_tmpD53[0]=((_tmpD54.tag=0,((_tmpD54.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD54.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD54)))))),_tmpD53)))),((_tmpD55->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD55)))))),_tmpD52)));}
goto _LL23F;}case 279: _LL468: _LL469: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2448 "parse.y"
{struct Cyc_List_List*_tmpD5F;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD5E;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD5D;struct Cyc_Parse_Abstractdeclarator _tmpD5C;yyval=Cyc_YY30(((_tmpD5C.tms=((_tmpD5F=_region_malloc(yyr,sizeof(*_tmpD5F)),((_tmpD5F->hd=(void*)((_tmpD5D=_region_malloc(yyr,sizeof(*_tmpD5D)),((_tmpD5D[0]=((_tmpD5E.tag=1,((_tmpD5E.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpD5E.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD5E.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD5E)))))))),_tmpD5D)))),((_tmpD5F->tl=0,_tmpD5F)))))),_tmpD5C)));}
goto _LL23F;}case 280: _LL46A: _LL46B: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2450 "parse.y"
{struct Cyc_List_List*_tmpD69;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD68;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD67;struct Cyc_Parse_Abstractdeclarator _tmpD66;yyval=Cyc_YY30(((_tmpD66.tms=((_tmpD69=_region_malloc(yyr,sizeof(*_tmpD69)),((_tmpD69->hd=(void*)((_tmpD67=_region_malloc(yyr,sizeof(*_tmpD67)),((_tmpD67[0]=((_tmpD68.tag=1,((_tmpD68.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpD68.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpD68.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD68)))))))),_tmpD67)))),((_tmpD69->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD69)))))),_tmpD66)));}
# 2453
goto _LL23F;}case 281: _LL46C: _LL46D: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2454 "parse.y"
{struct Cyc_List_List*_tmpD7E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD7D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD7C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD7B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD7A;struct Cyc_Parse_Abstractdeclarator _tmpD79;yyval=Cyc_YY30(((_tmpD79.tms=((_tmpD7E=_region_malloc(yyr,sizeof(*_tmpD7E)),((_tmpD7E->hd=(void*)((_tmpD7A=_region_malloc(yyr,sizeof(*_tmpD7A)),((_tmpD7A[0]=((_tmpD7D.tag=3,((_tmpD7D.f1=(void*)((_tmpD7B=_region_malloc(yyr,sizeof(*_tmpD7B)),((_tmpD7B[0]=((_tmpD7C.tag=1,((_tmpD7C.f1=0,((_tmpD7C.f2=0,((_tmpD7C.f3=0,((_tmpD7C.f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpD7C.f5=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpD7C.f6=Cyc_yyget_YY55(yyyyvsp[4]),((_tmpD7C.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpD7C)))))))))))))))),_tmpD7B)))),_tmpD7D)))),_tmpD7A)))),((_tmpD7E->tl=0,_tmpD7E)))))),_tmpD79)));}
# 2456
goto _LL23F;}case 282: _LL46E: _LL46F: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2457 "parse.y"
struct _tuple26*_tmp703=Cyc_yyget_YY39(yyyyvsp[1]);struct Cyc_List_List*_tmp705;int _tmp706;struct Cyc_Absyn_VarargInfo*_tmp707;void*_tmp708;struct Cyc_List_List*_tmp709;struct _tuple26*_tmp704=_tmp703;_tmp705=_tmp704->f1;_tmp706=_tmp704->f2;_tmp707=_tmp704->f3;_tmp708=_tmp704->f4;_tmp709=_tmp704->f5;
{struct Cyc_List_List*_tmpD93;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD92;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD91;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD90;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD8F;struct Cyc_Parse_Abstractdeclarator _tmpD8E;yyval=Cyc_YY30(((_tmpD8E.tms=((_tmpD93=_region_malloc(yyr,sizeof(*_tmpD93)),((_tmpD93->hd=(void*)((_tmpD8F=_region_malloc(yyr,sizeof(*_tmpD8F)),((_tmpD8F[0]=((_tmpD92.tag=3,((_tmpD92.f1=(void*)((_tmpD90=_region_malloc(yyr,sizeof(*_tmpD90)),((_tmpD90[0]=((_tmpD91.tag=1,((_tmpD91.f1=_tmp705,((_tmpD91.f2=_tmp706,((_tmpD91.f3=_tmp707,((_tmpD91.f4=_tmp708,((_tmpD91.f5=_tmp709,((_tmpD91.f6=Cyc_yyget_YY55(yyyyvsp[3]),((_tmpD91.f7=Cyc_yyget_YY55(yyyyvsp[4]),_tmpD91)))))))))))))))),_tmpD90)))),_tmpD92)))),_tmpD8F)))),((_tmpD93->tl=0,_tmpD93)))))),_tmpD8E)));}
# 2460
goto _LL23F;}case 283: _LL470: _LL471: {
# 2462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2461 "parse.y"
{struct Cyc_List_List*_tmpDA8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDA7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDA6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDA5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDA4;struct Cyc_Parse_Abstractdeclarator _tmpDA3;yyval=Cyc_YY30(((_tmpDA3.tms=((_tmpDA8=_region_malloc(yyr,sizeof(*_tmpDA8)),((_tmpDA8->hd=(void*)((_tmpDA4=_region_malloc(yyr,sizeof(*_tmpDA4)),((_tmpDA4[0]=((_tmpDA7.tag=3,((_tmpDA7.f1=(void*)((_tmpDA5=_region_malloc(yyr,sizeof(*_tmpDA5)),((_tmpDA5[0]=((_tmpDA6.tag=1,((_tmpDA6.f1=0,((_tmpDA6.f2=0,((_tmpDA6.f3=0,((_tmpDA6.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpDA6.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpDA6.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpDA6.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpDA6)))))))))))))))),_tmpDA5)))),_tmpDA7)))),_tmpDA4)))),((_tmpDA8->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDA8)))))),_tmpDA3)));}
# 2464
goto _LL23F;}case 284: _LL472: _LL473: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2465 "parse.y"
struct _tuple26*_tmp716=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp718;int _tmp719;struct Cyc_Absyn_VarargInfo*_tmp71A;void*_tmp71B;struct Cyc_List_List*_tmp71C;struct _tuple26*_tmp717=_tmp716;_tmp718=_tmp717->f1;_tmp719=_tmp717->f2;_tmp71A=_tmp717->f3;_tmp71B=_tmp717->f4;_tmp71C=_tmp717->f5;
{struct Cyc_List_List*_tmpDBD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDBC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDBB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDBA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDB9;struct Cyc_Parse_Abstractdeclarator _tmpDB8;yyval=Cyc_YY30(((_tmpDB8.tms=((_tmpDBD=_region_malloc(yyr,sizeof(*_tmpDBD)),((_tmpDBD->hd=(void*)((_tmpDB9=_region_malloc(yyr,sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBC.tag=3,((_tmpDBC.f1=(void*)((_tmpDBA=_region_malloc(yyr,sizeof(*_tmpDBA)),((_tmpDBA[0]=((_tmpDBB.tag=1,((_tmpDBB.f1=_tmp718,((_tmpDBB.f2=_tmp719,((_tmpDBB.f3=_tmp71A,((_tmpDBB.f4=_tmp71B,((_tmpDBB.f5=_tmp71C,((_tmpDBB.f6=
Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDBB.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDBB)))))))))))))))),_tmpDBA)))),_tmpDBC)))),_tmpDB9)))),((_tmpDBD->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDBD)))))),_tmpDB8)));}
# 2469
goto _LL23F;}case 285: _LL474: _LL475: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2471 "parse.y"
struct Cyc_List_List*_tmp723=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDC7;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDC6;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDC5;struct Cyc_Parse_Abstractdeclarator _tmpDC4;yyval=Cyc_YY30(((_tmpDC4.tms=((_tmpDC7=_region_malloc(yyr,sizeof(*_tmpDC7)),((_tmpDC7->hd=(void*)((_tmpDC5=_region_malloc(yyr,sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=4,((_tmpDC6.f1=_tmp723,((_tmpDC6.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDC6.f3=0,_tmpDC6)))))))),_tmpDC5)))),((_tmpDC7->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDC7)))))),_tmpDC4)));}
# 2475
goto _LL23F;}case 286: _LL476: _LL477: {
# 2477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2476 "parse.y"
{struct Cyc_List_List*_tmpDD1;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDD0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDCF;struct Cyc_Parse_Abstractdeclarator _tmpDCE;yyval=Cyc_YY30(((_tmpDCE.tms=((_tmpDD1=_region_malloc(yyr,sizeof(*_tmpDD1)),((_tmpDD1->hd=(void*)((_tmpDCF=_region_malloc(yyr,sizeof(*_tmpDCF)),((_tmpDCF[0]=((_tmpDD0.tag=5,((_tmpDD0.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDD0.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpDD0)))))),_tmpDCF)))),((_tmpDD1->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDD1)))))),_tmpDCE)));}
# 2478
goto _LL23F;}case 287: _LL478: _LL479: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2482 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 288: _LL47A: _LL47B: {
# 2485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2483 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 289: _LL47C: _LL47D: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2484 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 290: _LL47E: _LL47F: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2485 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 291: _LL480: _LL481: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2486 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 292: _LL482: _LL483: {
# 2489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2487 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 293: _LL484: _LL485: {
# 2490
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2492
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2490 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDD4;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDD3;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=((_tmpDD4.tag=16,((_tmpDD4.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpDD4)))),_tmpDD3)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 294: _LL486: _LL487: {
# 2493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDDA;struct _dyneither_ptr*_tmpDD9;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDD8;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=((_tmpDDA.tag=13,((_tmpDDA.f1=((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDD9)))),((_tmpDDA.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]),_tmpDDA)))))),_tmpDD8)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 295: _LL488: _LL489: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2500 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 296: _LL48A: _LL48B: {
# 2503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2501 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 297: _LL48C: _LL48D: {
# 2504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2506 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 298: _LL48E: _LL48F: {
# 2509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2507 "parse.y"
yyval=yyyyvsp[1];
goto _LL23F;}case 299: _LL490: _LL491: {
# 2510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2512 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 300: _LL492: _LL493: {
# 2515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2513 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
goto _LL23F;}case 301: _LL494: _LL495: {
# 2516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2514 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDE0;struct _dyneither_ptr*_tmpDDF;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDDE;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDE0.tag=13,((_tmpDE0.f1=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDDF)))),((_tmpDE0.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpDE0)))))),_tmpDDE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2516
goto _LL23F;}case 302: _LL496: _LL497: {
# 2518
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2516 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDE6;struct _dyneither_ptr*_tmpDE5;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDE4;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4[0]=((_tmpDE6.tag=13,((_tmpDE6.f1=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDE5)))),((_tmpDE6.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3])),_tmpDE6)))))),_tmpDE4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2518
goto _LL23F;}case 303: _LL498: _LL499: {
# 2520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2518 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 304: _LL49A: _LL49B: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2519 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 305: _LL49C: _LL49D: {
# 2522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2520 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDE9;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDE8;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDE9.tag=1,((_tmpDE9.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpDE9)))),_tmpDE8)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
goto _LL23F;}case 306: _LL49E: _LL49F: {
# 2524
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2523 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDEC;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDEB;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB[0]=((_tmpDEC.tag=1,((_tmpDEC.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpDEC)))),_tmpDEB)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));}
goto _LL23F;}case 307: _LL4A0: _LL4A1: {
# 2526
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2528
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2528 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 308: _LL4A2: _LL4A3: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2530 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 309: _LL4A4: _LL4A5: {
# 2533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 310: _LL4A6: _LL4A7: {
# 2539
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2539 "parse.y"
struct Cyc_Absyn_Exp*_tmp73B=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp73B,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 311: _LL4A8: _LL4A9: {
# 2543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2543 "parse.y"
struct Cyc_Absyn_Exp*_tmp73C=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp73C,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2546
goto _LL23F;}case 312: _LL4AA: _LL4AB: {
# 2548
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2549 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 313: _LL4AC: _LL4AD:
# 2552
 yyval=Cyc_YY8(0);
goto _LL23F;case 314: _LL4AE: _LL4AF: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2567 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDEF;struct Cyc_List_List*_tmpDEE;yyval=Cyc_YY8(((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->hd=((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDEF->pat_vars=0,((_tmpDEF->where_clause=0,((_tmpDEF->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]),((_tmpDEF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDEF)))))))))))),((_tmpDEE->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpDEE)))))));}
goto _LL23F;}case 315: _LL4B0: _LL4B1: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2571 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDF2;struct Cyc_List_List*_tmpDF1;yyval=Cyc_YY8(((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->hd=((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpDF2->pat_vars=0,((_tmpDF2->where_clause=0,((_tmpDF2->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDF2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDF2)))))))))))),((_tmpDF1->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpDF1)))))));}
goto _LL23F;}case 316: _LL4B2: _LL4B3: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2575 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDF5;struct Cyc_List_List*_tmpDF4;yyval=Cyc_YY8(((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->hd=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpDF5->pat_vars=0,((_tmpDF5->where_clause=0,((_tmpDF5->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]),((_tmpDF5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDF5)))))))))))),((_tmpDF4->tl=Cyc_yyget_YY8(yyyyvsp[4]),_tmpDF4)))))));}
goto _LL23F;}case 317: _LL4B4: _LL4B5: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2577 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDF8;struct Cyc_List_List*_tmpDF7;yyval=Cyc_YY8(((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7->hd=((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpDF8->pat_vars=0,((_tmpDF8->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpDF8->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDF8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDF8)))))))))))),((_tmpDF7->tl=
Cyc_yyget_YY8(yyyyvsp[5]),_tmpDF7)))))));}
goto _LL23F;}case 318: _LL4B6: _LL4B7: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2581 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDFB;struct Cyc_List_List*_tmpDFA;yyval=Cyc_YY8(((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->hd=((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpDFB->pat_vars=0,((_tmpDFB->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpDFB->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]),((_tmpDFB->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDFB)))))))))))),((_tmpDFA->tl=Cyc_yyget_YY8(yyyyvsp[6]),_tmpDFA)))))));}
goto _LL23F;}case 319: _LL4B8: _LL4B9: {
# 2584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2588 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 320: _LL4BA: _LL4BB: {
# 2591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2592 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 321: _LL4BC: _LL4BD: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2596 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 322: _LL4BE: _LL4BF: {
# 2600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2599 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 323: _LL4C0: _LL4C1: {
# 2603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2602 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 324: _LL4C2: _LL4C3: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2605 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 325: _LL4C4: _LL4C5: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2608 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 326: _LL4C6: _LL4C7: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2611 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 327: _LL4C8: _LL4C9: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2614 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 328: _LL4CA: _LL4CB: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2617 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 329: _LL4CC: _LL4CD: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2620 "parse.y"
struct Cyc_List_List*_tmp747=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp748=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp747,_tmp748));
# 2625
goto _LL23F;}case 330: _LL4CE: _LL4CF: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2626 "parse.y"
struct Cyc_List_List*_tmp749=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp74A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp749,_tmp74A));
# 2631
goto _LL23F;}case 331: _LL4D0: _LL4D1: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2632 "parse.y"
struct Cyc_List_List*_tmp74B=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp74C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp74B,_tmp74C));
# 2637
goto _LL23F;}case 332: _LL4D2: _LL4D3: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2638 "parse.y"
struct Cyc_List_List*_tmp74D=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp74E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp74D,_tmp74E));
# 2643
goto _LL23F;}case 333: _LL4D4: _LL4D5: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpDFC;yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDFC)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 334: _LL4D6: _LL4D7: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2649 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 335: _LL4D8: _LL4D9: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2650 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 336: _LL4DA: _LL4DB: {
# 2653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2651 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 337: _LL4DC: _LL4DD: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2652 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 338: _LL4DE: _LL4DF: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2654 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 339: _LL4E0: _LL4E1: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2655 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 340: _LL4E2: _LL4E3: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2657 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 341: _LL4E4: _LL4E5: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2666 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 342: _LL4E6: _LL4E7: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2669 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 343: _LL4E8: _LL4E9: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2671 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 344: _LL4EA: _LL4EB: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2674 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 345: _LL4EC: _LL4ED: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2676 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 346: _LL4EE: _LL4EF: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2679 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 347: _LL4F0: _LL4F1: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2681 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 348: _LL4F2: _LL4F3: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2684 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 349: _LL4F4: _LL4F5: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2686 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 350: _LL4F6: _LL4F7: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2689 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 351: _LL4F8: _LL4F9: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2691 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 352: _LL4FA: _LL4FB: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2694 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 353: _LL4FC: _LL4FD: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2696 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 354: _LL4FE: _LL4FF: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2699 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 355: _LL500: _LL501: {
# 2702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2701 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 356: _LL502: _LL503: {
# 2704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2703 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 357: _LL504: _LL505: {
# 2706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2706 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 358: _LL506: _LL507: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2708 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 359: _LL508: _LL509: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2710 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 360: _LL50A: _LL50B: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2712 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 361: _LL50C: _LL50D: {
# 2715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2714 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 362: _LL50E: _LL50F: {
# 2717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2717 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 363: _LL510: _LL511: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2719 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 364: _LL512: _LL513: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2721 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 365: _LL514: _LL515: {
# 2724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2724 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 366: _LL516: _LL517: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2726 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 367: _LL518: _LL519: {
# 2729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2728 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 368: _LL51A: _LL51B: {
# 2731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2731 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 369: _LL51C: _LL51D: {
# 2734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2733 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 370: _LL51E: _LL51F: {
# 2736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2735 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 371: _LL520: _LL521: {
# 2738
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2740
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2737 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 372: _LL522: _LL523: {
# 2740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2740 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 373: _LL524: _LL525: {
# 2743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2742 "parse.y"
void*_tmp750=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp750,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2745
goto _LL23F;}case 374: _LL526: _LL527: {
# 2747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2748 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 375: _LL528: _LL529: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2751 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL23F;}case 376: _LL52A: _LL52B: {
# 2754
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2753 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 377: _LL52C: _LL52D: {
# 2756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2755 "parse.y"
void*_tmp751=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp751,(unsigned int)(yyyylsp[0]).first_line)));
# 2758
goto _LL23F;}case 378: _LL52E: _LL52F: {
# 2760
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2759 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 379: _LL530: _LL531: {
# 2762
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2761 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL23F;}case 380: _LL532: _LL533: {
# 2764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2766 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 381: _LL534: _LL535: {
# 2769
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2775 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 382: _LL536: _LL537: {
# 2778
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2780 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 383: _LL538: _LL539: {
# 2783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2782 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL23F;}case 384: _LL53A: _LL53B: {
# 2785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2784 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp752=e->r;void*_tmp753=_tmp752;struct _dyneither_ptr _tmp754;int _tmp755;enum Cyc_Absyn_Sign _tmp756;int _tmp757;enum Cyc_Absyn_Sign _tmp758;short _tmp759;enum Cyc_Absyn_Sign _tmp75A;char _tmp75B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).LongLong_c).tag){case 2: _LL67A: _tmp75A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Char_c).val).f1;_tmp75B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Char_c).val).f2;_LL67B:
# 2788
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDFF;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDFE;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpDFE=_cycalloc_atomic(sizeof(*_tmpDFE)),((_tmpDFE[0]=((_tmpDFF.tag=11,((_tmpDFF.f1=_tmp75B,_tmpDFF)))),_tmpDFE)))),e->loc));}goto _LL679;case 4: _LL67C: _tmp758=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Short_c).val).f1;_tmp759=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Short_c).val).f2;_LL67D:
# 2790
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE02;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE01;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE01=_cycalloc_atomic(sizeof(*_tmpE01)),((_tmpE01[0]=((_tmpE02.tag=10,((_tmpE02.f1=_tmp758,((_tmpE02.f2=(int)_tmp759,_tmpE02)))))),_tmpE01)))),e->loc));}goto _LL679;case 5: _LL67E: _tmp756=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Int_c).val).f1;_tmp757=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Int_c).val).f2;_LL67F:
# 2792
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE05;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE04;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE04=_cycalloc_atomic(sizeof(*_tmpE04)),((_tmpE04[0]=((_tmpE05.tag=10,((_tmpE05.f1=_tmp756,((_tmpE05.f2=_tmp757,_tmpE05)))))),_tmpE04)))),e->loc));}goto _LL679;case 7: _LL680: _tmp754=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Float_c).val).f1;_tmp755=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Float_c).val).f2;_LL681:
# 2794
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE08;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpE07;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=((_tmpE08.tag=12,((_tmpE08.f1=_tmp754,((_tmpE08.f2=_tmp755,_tmpE08)))))),_tmpE07)))),e->loc));}goto _LL679;case 1: _LL682: _LL683:
# 2796
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL679;case 8: _LL684: _LL685:
# 2798
{const char*_tmpE09;Cyc_Parse_err(((_tmpE09="strings cannot occur within patterns",_tag_dyneither(_tmpE09,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL679;case 9: _LL686: _LL687:
# 2800
{const char*_tmpE0A;Cyc_Parse_err(((_tmpE0A="strings cannot occur within patterns",_tag_dyneither(_tmpE0A,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL679;case 6: _LL688: _LL689:
# 2802
{const char*_tmpE0B;Cyc_Parse_unimp(((_tmpE0B="long long's in patterns",_tag_dyneither(_tmpE0B,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL679;default: goto _LL68A;}else{_LL68A: _LL68B: {
# 2804
const char*_tmpE0C;Cyc_Parse_err(((_tmpE0C="bad constant in case",_tag_dyneither(_tmpE0C,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}}_LL679:;}
# 2807
goto _LL23F;}case 385: _LL53C: _LL53D: {
# 2809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2808 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE0F;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE0E;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0F.tag=15,((_tmpE0F.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE0F)))),_tmpE0E)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 386: _LL53E: _LL53F: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2810 "parse.y"
{const char*_tmpE10;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE10="as",_tag_dyneither(_tmpE10,sizeof(char),3))))!= 0){
const char*_tmpE11;Cyc_Parse_err(((_tmpE11="expecting `as'",_tag_dyneither(_tmpE11,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE1B;struct _dyneither_ptr*_tmpE1A;struct _tuple0*_tmpE19;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE18;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE1B.tag=1,((_tmpE1B.f1=Cyc_Absyn_new_vardecl(((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->f1=Cyc_Absyn_Loc_n,((_tmpE19->f2=((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE1A)))),_tmpE19)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE1B.f2=
Cyc_yyget_YY9(yyyyvsp[2]),_tmpE1B)))))),_tmpE18)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2815
goto _LL23F;}case 387: _LL540: _LL541: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2816 "parse.y"
{const char*_tmpE1C;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE1C="alias",_tag_dyneither(_tmpE1C,sizeof(char),6))))!= 0){
const char*_tmpE1D;Cyc_Parse_err(((_tmpE1D="expecting `alias'",_tag_dyneither(_tmpE1D,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp772=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE25;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE24;struct _dyneither_ptr*_tmpE23;struct Cyc_Absyn_Tvar*_tmpE22;struct Cyc_Absyn_Tvar*tv=(_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->name=((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE23)))),((_tmpE22->identity=- 1,((_tmpE22->kind=(void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE24.tag=0,((_tmpE24.f1=& Cyc_Tcutil_rk,_tmpE24)))),_tmpE25)))),_tmpE22)))))));
struct _dyneither_ptr*_tmpE28;struct _tuple0*_tmpE27;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27->f1=Cyc_Absyn_Loc_n,((_tmpE27->f2=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE28)))),_tmpE27)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE2B;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE2A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A[0]=((_tmpE2B.tag=2,((_tmpE2B.f1=tv,((_tmpE2B.f2=vd,_tmpE2B)))))),_tmpE2A)))),(unsigned int)_tmp772));}
# 2824
goto _LL23F;};}case 388: _LL542: _LL543: {
# 2826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2825 "parse.y"
struct _tuple22 _tmp77B=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp77D;int _tmp77E;struct _tuple22 _tmp77C=_tmp77B;_tmp77D=_tmp77C.f1;_tmp77E=_tmp77C.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE2E;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE2D;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2E.tag=5,((_tmpE2E.f1=_tmp77D,((_tmpE2E.f2=_tmp77E,_tmpE2E)))))),_tmpE2D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2828
goto _LL23F;}case 389: _LL544: _LL545: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2829 "parse.y"
struct _tuple22 _tmp781=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp783;int _tmp784;struct _tuple22 _tmp782=_tmp781;_tmp783=_tmp782.f1;_tmp784=_tmp782.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE31;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE30;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE31.tag=16,((_tmpE31.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE31.f2=_tmp783,((_tmpE31.f3=_tmp784,_tmpE31)))))))),_tmpE30)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2832
goto _LL23F;}case 390: _LL546: _LL547: {
# 2834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2833 "parse.y"
struct _tuple22 _tmp787=*Cyc_yyget_YY14(yyyyvsp[3]);struct Cyc_List_List*_tmp789;int _tmp78A;struct _tuple22 _tmp788=_tmp787;_tmp789=_tmp788.f1;_tmp78A=_tmp788.f2;{
struct Cyc_List_List*_tmp78B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE37;struct Cyc_Absyn_AggrInfo*_tmpE36;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE35;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((_tmpE37.tag=7,((_tmpE37.f1=((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE36->targs=0,_tmpE36)))))),((_tmpE37.f2=_tmp78B,((_tmpE37.f3=_tmp789,((_tmpE37.f4=_tmp78A,_tmpE37)))))))))),_tmpE35)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2838
goto _LL23F;};}case 391: _LL548: _LL549: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2839 "parse.y"
struct _tuple22 _tmp78F=*Cyc_yyget_YY14(yyyyvsp[2]);struct Cyc_List_List*_tmp791;int _tmp792;struct _tuple22 _tmp790=_tmp78F;_tmp791=_tmp790.f1;_tmp792=_tmp790.f2;{
struct Cyc_List_List*_tmp793=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE3A;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE39;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39[0]=((_tmpE3A.tag=7,((_tmpE3A.f1=0,((_tmpE3A.f2=_tmp793,((_tmpE3A.f3=_tmp791,((_tmpE3A.f4=_tmp792,_tmpE3A)))))))))),_tmpE39)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2843
goto _LL23F;};}case 392: _LL54A: _LL54B: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2844 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE3D;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE3C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3D.tag=6,((_tmpE3D.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE3D)))),_tmpE3C)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 393: _LL54C: _LL54D: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2846 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE47;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE46;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE45;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE44;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE47.tag=6,((_tmpE47.f1=Cyc_Absyn_new_pat((void*)((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45[0]=((_tmpE46.tag=6,((_tmpE46.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE46)))),_tmpE45)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE47)))),_tmpE44)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 394: _LL54E: _LL54F: {
# 2849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2848 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE51;struct _dyneither_ptr*_tmpE50;struct _tuple0*_tmpE4F;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE4E;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE51.tag=3,((_tmpE51.f1=Cyc_Absyn_new_vardecl(((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F->f1=Cyc_Absyn_Loc_n,((_tmpE4F->f2=((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE50)))),_tmpE4F)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE51.f2=
# 2850
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE51)))))),_tmpE4E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2852
goto _LL23F;}case 395: _LL550: _LL551: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2853 "parse.y"
{const char*_tmpE52;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE52="as",_tag_dyneither(_tmpE52,sizeof(char),3))))!= 0){
const char*_tmpE53;Cyc_Parse_err(((_tmpE53="expecting `as'",_tag_dyneither(_tmpE53,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE5D;struct _dyneither_ptr*_tmpE5C;struct _tuple0*_tmpE5B;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE5A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5D.tag=3,((_tmpE5D.f1=Cyc_Absyn_new_vardecl(((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B->f1=Cyc_Absyn_Loc_n,((_tmpE5B->f2=((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE5C)))),_tmpE5B)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE5D.f2=
# 2857
Cyc_yyget_YY9(yyyyvsp[3]),_tmpE5D)))))),_tmpE5A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2859
goto _LL23F;}case 396: _LL552: _LL553: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2860 "parse.y"
void*_tmp7A6=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE6E;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE6D;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE6C;struct _dyneither_ptr*_tmpE6B;struct _tuple0*_tmpE6A;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE69;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=((_tmpE6E.tag=4,((_tmpE6E.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7A6),((_tmpE6E.f2=
Cyc_Absyn_new_vardecl(((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A->f1=Cyc_Absyn_Loc_n,((_tmpE6A->f2=((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE6B)))),_tmpE6A)))))),(void*)(
(_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C[0]=((_tmpE6D.tag=19,((_tmpE6D.f1=_tmp7A6,_tmpE6D)))),_tmpE6C)))),0),_tmpE6E)))))),_tmpE69)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2865
goto _LL23F;}case 397: _LL554: _LL555: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2866 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7AD=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE8A;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE89;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE88;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE87;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE86;struct _dyneither_ptr*_tmpE85;struct _tuple0*_tmpE84;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE83;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=((_tmpE8A.tag=4,((_tmpE8A.f1=_tmp7AD,((_tmpE8A.f2=
Cyc_Absyn_new_vardecl(((_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84->f1=Cyc_Absyn_Loc_n,((_tmpE84->f2=((_tmpE85=_cycalloc(sizeof(*_tmpE85)),((_tmpE85[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE85)))),_tmpE84)))))),(void*)(
(_tmpE86=_cycalloc(sizeof(*_tmpE86)),((_tmpE86[0]=((_tmpE89.tag=19,((_tmpE89.f1=(void*)((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87[0]=((_tmpE88.tag=2,((_tmpE88.f1=_tmp7AD,_tmpE88)))),_tmpE87)))),_tmpE89)))),_tmpE86)))),0),_tmpE8A)))))),_tmpE83)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2871
goto _LL23F;}case 398: _LL556: _LL557: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2874 "parse.y"
{struct _tuple22*_tmpE8B;yyval=Cyc_YY10(((_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpE8B->f2=0,_tmpE8B)))))));}
goto _LL23F;}case 399: _LL558: _LL559: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2875 "parse.y"
{struct _tuple22*_tmpE8C;yyval=Cyc_YY10(((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpE8C->f2=1,_tmpE8C)))))));}
goto _LL23F;}case 400: _LL55A: _LL55B: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2876 "parse.y"
{struct _tuple22*_tmpE8D;yyval=Cyc_YY10(((_tmpE8D=_cycalloc(sizeof(*_tmpE8D)),((_tmpE8D->f1=0,((_tmpE8D->f2=1,_tmpE8D)))))));}
goto _LL23F;}case 401: _LL55C: _LL55D: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2881 "parse.y"
{struct Cyc_List_List*_tmpE8E;yyval=Cyc_YY11(((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E->hd=Cyc_yyget_YY9(yyyyvsp[0]),((_tmpE8E->tl=0,_tmpE8E)))))));}
goto _LL23F;}case 402: _LL55E: _LL55F: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2883 "parse.y"
{struct Cyc_List_List*_tmpE8F;yyval=Cyc_YY11(((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F->hd=Cyc_yyget_YY9(yyyyvsp[2]),((_tmpE8F->tl=Cyc_yyget_YY11(yyyyvsp[0]),_tmpE8F)))))));}
goto _LL23F;}case 403: _LL560: _LL561: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2888 "parse.y"
{struct _tuple23*_tmpE90;yyval=Cyc_YY12(((_tmpE90=_cycalloc(sizeof(*_tmpE90)),((_tmpE90->f1=0,((_tmpE90->f2=Cyc_yyget_YY9(yyyyvsp[0]),_tmpE90)))))));}
goto _LL23F;}case 404: _LL562: _LL563: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2890 "parse.y"
{struct _tuple23*_tmpE91;yyval=Cyc_YY12(((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpE91->f2=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE91)))))));}
goto _LL23F;}case 405: _LL564: _LL565: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2893 "parse.y"
{struct _tuple22*_tmpE92;yyval=Cyc_YY14(((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE92->f2=0,_tmpE92)))))));}
goto _LL23F;}case 406: _LL566: _LL567: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2894 "parse.y"
{struct _tuple22*_tmpE93;yyval=Cyc_YY14(((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE93->f2=1,_tmpE93)))))));}
goto _LL23F;}case 407: _LL568: _LL569: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2895 "parse.y"
{struct _tuple22*_tmpE94;yyval=Cyc_YY14(((_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94->f1=0,((_tmpE94->f2=1,_tmpE94)))))));}
goto _LL23F;}case 408: _LL56A: _LL56B: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2900 "parse.y"
{struct Cyc_List_List*_tmpE95;yyval=Cyc_YY13(((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE95->tl=0,_tmpE95)))))));}
goto _LL23F;}case 409: _LL56C: _LL56D: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2902 "parse.y"
{struct Cyc_List_List*_tmpE96;yyval=Cyc_YY13(((_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE96->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpE96)))))));}
goto _LL23F;}case 410: _LL56E: _LL56F: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
goto _LL23F;}case 411: _LL570: _LL571: {
# 2911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2910 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 412: _LL572: _LL573: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2915 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 413: _LL574: _LL575: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2917 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 414: _LL576: _LL577: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2919 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 415: _LL578: _LL579: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2923 "parse.y"
yyval=Cyc_YY7(0);
goto _LL23F;}case 416: _LL57A: _LL57B: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2924 "parse.y"
{struct Cyc_Core_Opt*_tmpE97;yyval=Cyc_YY7(((_tmpE97=_cycalloc_atomic(sizeof(*_tmpE97)),((_tmpE97->v=(void*)Cyc_Absyn_Times,_tmpE97)))));}
goto _LL23F;}case 417: _LL57C: _LL57D: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2925 "parse.y"
{struct Cyc_Core_Opt*_tmpE98;yyval=Cyc_YY7(((_tmpE98=_cycalloc_atomic(sizeof(*_tmpE98)),((_tmpE98->v=(void*)Cyc_Absyn_Div,_tmpE98)))));}
goto _LL23F;}case 418: _LL57E: _LL57F: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2926 "parse.y"
{struct Cyc_Core_Opt*_tmpE99;yyval=Cyc_YY7(((_tmpE99=_cycalloc_atomic(sizeof(*_tmpE99)),((_tmpE99->v=(void*)Cyc_Absyn_Mod,_tmpE99)))));}
goto _LL23F;}case 419: _LL580: _LL581: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2927 "parse.y"
{struct Cyc_Core_Opt*_tmpE9A;yyval=Cyc_YY7(((_tmpE9A=_cycalloc_atomic(sizeof(*_tmpE9A)),((_tmpE9A->v=(void*)Cyc_Absyn_Plus,_tmpE9A)))));}
goto _LL23F;}case 420: _LL582: _LL583: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2928 "parse.y"
{struct Cyc_Core_Opt*_tmpE9B;yyval=Cyc_YY7(((_tmpE9B=_cycalloc_atomic(sizeof(*_tmpE9B)),((_tmpE9B->v=(void*)Cyc_Absyn_Minus,_tmpE9B)))));}
goto _LL23F;}case 421: _LL584: _LL585: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2929 "parse.y"
{struct Cyc_Core_Opt*_tmpE9C;yyval=Cyc_YY7(((_tmpE9C=_cycalloc_atomic(sizeof(*_tmpE9C)),((_tmpE9C->v=(void*)Cyc_Absyn_Bitlshift,_tmpE9C)))));}
goto _LL23F;}case 422: _LL586: _LL587: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2930 "parse.y"
{struct Cyc_Core_Opt*_tmpE9D;yyval=Cyc_YY7(((_tmpE9D=_cycalloc_atomic(sizeof(*_tmpE9D)),((_tmpE9D->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE9D)))));}
goto _LL23F;}case 423: _LL588: _LL589: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2931 "parse.y"
{struct Cyc_Core_Opt*_tmpE9E;yyval=Cyc_YY7(((_tmpE9E=_cycalloc_atomic(sizeof(*_tmpE9E)),((_tmpE9E->v=(void*)Cyc_Absyn_Bitand,_tmpE9E)))));}
goto _LL23F;}case 424: _LL58A: _LL58B: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2932 "parse.y"
{struct Cyc_Core_Opt*_tmpE9F;yyval=Cyc_YY7(((_tmpE9F=_cycalloc_atomic(sizeof(*_tmpE9F)),((_tmpE9F->v=(void*)Cyc_Absyn_Bitxor,_tmpE9F)))));}
goto _LL23F;}case 425: _LL58C: _LL58D: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2933 "parse.y"
{struct Cyc_Core_Opt*_tmpEA0;yyval=Cyc_YY7(((_tmpEA0=_cycalloc_atomic(sizeof(*_tmpEA0)),((_tmpEA0->v=(void*)Cyc_Absyn_Bitor,_tmpEA0)))));}
goto _LL23F;}case 426: _LL58E: _LL58F: {
# 2936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2938 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 427: _LL590: _LL591: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2940 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 428: _LL592: _LL593: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2943 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 429: _LL594: _LL595: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2946 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 430: _LL596: _LL597: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2948 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 431: _LL598: _LL599: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2950 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 432: _LL59A: _LL59B: {
# 2953
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2952 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 433: _LL59C: _LL59D: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2956 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 434: _LL59E: _LL59F: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2960 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 435: _LL5A0: _LL5A1: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2962 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 436: _LL5A2: _LL5A3: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2966 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 437: _LL5A4: _LL5A5: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2968 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 438: _LL5A6: _LL5A7: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2972 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 439: _LL5A8: _LL5A9: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2974 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 440: _LL5AA: _LL5AB: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2978 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 441: _LL5AC: _LL5AD: {
# 2981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2980 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 442: _LL5AE: _LL5AF: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2984 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 443: _LL5B0: _LL5B1: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2986 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 444: _LL5B2: _LL5B3: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2990 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 445: _LL5B4: _LL5B5: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2992 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 446: _LL5B6: _LL5B7: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2994 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 447: _LL5B8: _LL5B9: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2998 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 448: _LL5BA: _LL5BB: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3000 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 449: _LL5BC: _LL5BD: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3002 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 450: _LL5BE: _LL5BF: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3004 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 451: _LL5C0: _LL5C1: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3006 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 452: _LL5C2: _LL5C3: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3010 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 453: _LL5C4: _LL5C5: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3012 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 454: _LL5C6: _LL5C7: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3014 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 455: _LL5C8: _LL5C9: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3018 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 456: _LL5CA: _LL5CB: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3020 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 457: _LL5CC: _LL5CD: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3022 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 458: _LL5CE: _LL5CF: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3026 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 459: _LL5D0: _LL5D1: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3028 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 460: _LL5D2: _LL5D3: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3030 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 461: _LL5D4: _LL5D5: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3032 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 462: _LL5D6: _LL5D7: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3036 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 463: _LL5D8: _LL5D9: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3038 "parse.y"
void*_tmp7CC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp7CC,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 3041
goto _LL23F;}case 464: _LL5DA: _LL5DB: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3044 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 465: _LL5DC: _LL5DD: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3045 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 466: _LL5DE: _LL5DF: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3046 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 467: _LL5E0: _LL5E1: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3047 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 468: _LL5E2: _LL5E3: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3048 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 469: _LL5E4: _LL5E5: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3049 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 470: _LL5E6: _LL5E7: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3050 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 471: _LL5E8: _LL5E9: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3052 "parse.y"
void*_tmp7CD=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp7CD,(unsigned int)(yyyylsp[0]).first_line));
# 3055
goto _LL23F;}case 472: _LL5EA: _LL5EB: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3055 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 473: _LL5EC: _LL5ED: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3057 "parse.y"
void*_tmp7CE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp7CE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3060
goto _LL23F;}case 474: _LL5EE: _LL5EF: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3062 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEA6;struct Cyc_Absyn_MallocInfo _tmpEA5;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEA4;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4[0]=((_tmpEA6.tag=33,((_tmpEA6.f1=((_tmpEA5.is_calloc=0,((_tmpEA5.rgn=0,((_tmpEA5.elt_type=0,((_tmpEA5.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEA5.fat_result=0,((_tmpEA5.inline_call=0,_tmpEA5)))))))))))),_tmpEA6)))),_tmpEA4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3064
goto _LL23F;}case 475: _LL5F0: _LL5F1: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3065 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEAC;struct Cyc_Absyn_MallocInfo _tmpEAB;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEAA;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=((_tmpEAC.tag=33,((_tmpEAC.f1=((_tmpEAB.is_calloc=0,((_tmpEAB.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEAB.elt_type=0,((_tmpEAB.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEAB.fat_result=0,((_tmpEAB.inline_call=0,_tmpEAB)))))))))))),_tmpEAC)))),_tmpEAA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3067
goto _LL23F;}case 476: _LL5F2: _LL5F3: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3068 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEB2;struct Cyc_Absyn_MallocInfo _tmpEB1;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEB0;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=((_tmpEB2.tag=33,((_tmpEB2.f1=((_tmpEB1.is_calloc=0,((_tmpEB1.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEB1.elt_type=0,((_tmpEB1.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEB1.fat_result=0,((_tmpEB1.inline_call=1,_tmpEB1)))))))))))),_tmpEB2)))),_tmpEB0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3070
goto _LL23F;}case 477: _LL5F4: _LL5F5: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3071 "parse.y"
void*_tmp7D8=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEBC;void**_tmpEBB;struct Cyc_Absyn_MallocInfo _tmpEBA;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEB9;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9[0]=((_tmpEBC.tag=33,((_tmpEBC.f1=((_tmpEBA.is_calloc=1,((_tmpEBA.rgn=0,((_tmpEBA.elt_type=((_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((_tmpEBB[0]=_tmp7D8,_tmpEBB)))),((_tmpEBA.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEBA.fat_result=0,((_tmpEBA.inline_call=0,_tmpEBA)))))))))))),_tmpEBC)))),_tmpEB9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3074
goto _LL23F;}case 478: _LL5F6: _LL5F7: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3076 "parse.y"
void*_tmp7DD=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEC6;void**_tmpEC5;struct Cyc_Absyn_MallocInfo _tmpEC4;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEC3;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3[0]=((_tmpEC6.tag=33,((_tmpEC6.f1=((_tmpEC4.is_calloc=1,((_tmpEC4.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEC4.elt_type=((_tmpEC5=_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=_tmp7DD,_tmpEC5)))),((_tmpEC4.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEC4.fat_result=0,((_tmpEC4.inline_call=0,_tmpEC4)))))))))))),_tmpEC6)))),_tmpEC3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3079
goto _LL23F;}case 479: _LL5F8: _LL5F9: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3080 "parse.y"
{struct Cyc_Absyn_Exp*_tmpEC7[1];yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpEC7[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEC7,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 480: _LL5FA: _LL5FB: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3082 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpECD;struct _dyneither_ptr*_tmpECC;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpECB;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB[0]=((_tmpECD.tag=37,((_tmpECD.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpECD.f2=((_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpECC)))),_tmpECD)))))),_tmpECB)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 481: _LL5FC: _LL5FD: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3084 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpED3;struct _dyneither_ptr*_tmpED2;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpED1;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=((_tmpED3.tag=37,((_tmpED3.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpED3.f2=((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpED2)))),_tmpED3)))))),_tmpED1)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 482: _LL5FE: _LL5FF: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3086 "parse.y"
void*_tmp7E9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp7E9,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 483: _LL600: _LL601: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3089 "parse.y"
struct _tuple20 _tmp7EA=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp7EC;struct _dyneither_ptr _tmp7ED;struct _tuple20 _tmp7EB=_tmp7EA;_tmp7EC=_tmp7EB.f1;_tmp7ED=_tmp7EB.f2;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp7EC,_tmp7ED,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 484: _LL602: _LL603: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3094 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL23F;}case 485: _LL604: _LL605: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3095 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL23F;}case 486: _LL606: _LL607: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3096 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL23F;}case 487: _LL608: _LL609: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3101 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 488: _LL60A: _LL60B: {
# 3104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3103 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 489: _LL60C: _LL60D: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3105 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 490: _LL60E: _LL60F: {
# 3108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3107 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 491: _LL610: _LL611: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3109 "parse.y"
{struct _dyneither_ptr*_tmpED4;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpED4)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 492: _LL612: _LL613: {
# 3112
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3111 "parse.y"
{struct _dyneither_ptr*_tmpED5;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpED5)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 493: _LL614: _LL615: {
# 3114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3113 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 494: _LL616: _LL617: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3115 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 495: _LL618: _LL619: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3117 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpED8;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpED7;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpED7=_cycalloc(sizeof(*_tmpED7)),((_tmpED7[0]=((_tmpED8.tag=24,((_tmpED8.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpED8.f2=0,_tmpED8)))))),_tmpED7)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 496: _LL61A: _LL61B: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3119 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEDB;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEDA;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA[0]=((_tmpEDB.tag=24,((_tmpEDB.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEDB.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpEDB)))))),_tmpEDA)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 497: _LL61C: _LL61D: {
# 3122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3121 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEDE;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEDD;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD[0]=((_tmpEDE.tag=24,((_tmpEDE.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEDE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpEDE)))))),_tmpEDD)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 498: _LL61E: _LL61F: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEE8;struct _dyneither_ptr*_tmpEE7;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEE6;struct Cyc_List_List*_tmpEE5;yyval=Cyc_YY3(((_tmpEE5=_cycalloc(sizeof(*_tmpEE5)),((_tmpEE5->hd=(void*)((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=((_tmpEE6.tag=0,((_tmpEE6.f1=((_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEE7)))),_tmpEE6)))),_tmpEE8)))),((_tmpEE5->tl=0,_tmpEE5)))))));}
goto _LL23F;}case 499: _LL620: _LL621: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3129 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEEE;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEED;struct Cyc_List_List*_tmpEEC;yyval=Cyc_YY3(((_tmpEEC=_cycalloc(sizeof(*_tmpEEC)),((_tmpEEC->hd=(void*)((_tmpEEE=_cycalloc_atomic(sizeof(*_tmpEEE)),((_tmpEEE[0]=((_tmpEED.tag=1,((_tmpEED.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_Int_tok(yyyyvsp[0])),_tmpEED)))),_tmpEEE)))),((_tmpEEC->tl=0,_tmpEEC)))))));}
goto _LL23F;}case 500: _LL622: _LL623: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3131 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEF8;struct _dyneither_ptr*_tmpEF7;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEF6;struct Cyc_List_List*_tmpEF5;yyval=Cyc_YY3(((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5->hd=(void*)((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=((_tmpEF6.tag=0,((_tmpEF6.f1=((_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEF7)))),_tmpEF6)))),_tmpEF8)))),((_tmpEF5->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpEF5)))))));}
goto _LL23F;}case 501: _LL624: _LL625: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3134 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEFE;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEFD;struct Cyc_List_List*_tmpEFC;yyval=Cyc_YY3(((_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC->hd=(void*)((_tmpEFE=_cycalloc_atomic(sizeof(*_tmpEFE)),((_tmpEFE[0]=((_tmpEFD.tag=1,((_tmpEFD.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_Int_tok(yyyyvsp[2])),_tmpEFD)))),_tmpEFE)))),((_tmpEFC->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpEFC)))))));}
goto _LL23F;}case 502: _LL626: _LL627: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 503: _LL628: _LL629: {
# 3143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3142 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 504: _LL62A: _LL62B: {
# 3145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3144 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 505: _LL62C: _LL62D: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3146 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 506: _LL62E: _LL62F: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3148 "parse.y"
yyval=yyyyvsp[1];
goto _LL23F;}case 507: _LL630: _LL631: {
# 3151
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3153
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3153 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 508: _LL632: _LL633: {
# 3156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3155 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 509: _LL634: _LL635: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3158 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 510: _LL636: _LL637: {
# 3161
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3161 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF01;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF00;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF00=_cycalloc(sizeof(*_tmpF00)),((_tmpF00[0]=((_tmpF01.tag=28,((_tmpF01.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF01.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpF01.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3])),((_tmpF01.f4=0,_tmpF01)))))))))),_tmpF00)))),(unsigned int)(yyyylsp[0]).first_line));}
goto _LL23F;}case 511: _LL638: _LL639: {
# 3164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3164 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 512: _LL63A: _LL63B: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3168 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
goto _LL23F;}case 513: _LL63C: _LL63D: {
# 3171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpF02;yyval=Cyc_YY4(((_tmpF02=_cycalloc(sizeof(*_tmpF02)),((_tmpF02->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF02->tl=0,_tmpF02)))))));}
goto _LL23F;}case 514: _LL63E: _LL63F: {
# 3177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3176 "parse.y"
{struct Cyc_List_List*_tmpF03;yyval=Cyc_YY4(((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF03->tl=Cyc_yyget_YY4(yyyyvsp[0]),_tmpF03)))))));}
goto _LL23F;}case 515: _LL640: _LL641: {
# 3179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_const_exp(Cyc_yyget_Int_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 516: _LL642: _LL643: {
# 3185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3183 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 517: _LL644: _LL645: {
# 3186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3184 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 518: _LL646: _LL647: {
# 3187
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3186 "parse.y"
struct _dyneither_ptr _tmp808=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp808);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp808,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3194
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp808,i,(unsigned int)(yyyylsp[0]).first_line));
# 3196
goto _LL23F;}case 519: _LL648: _LL649: {
# 3198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3197 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
goto _LL23F;}case 520: _LL64A: _LL64B: {
# 3200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3201 "parse.y"
{struct _dyneither_ptr*_tmpF06;struct _tuple0*_tmpF05;yyval=Cyc_QualId_tok(((_tmpF05=_cycalloc(sizeof(*_tmpF05)),((_tmpF05->f1=Cyc_Absyn_Rel_n(0),((_tmpF05->f2=((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF06)))),_tmpF05)))))));}
goto _LL23F;}case 521: _LL64C: _LL64D: {
# 3204
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3202 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 522: _LL64E: _LL64F: {
# 3205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3205 "parse.y"
{struct _dyneither_ptr*_tmpF09;struct _tuple0*_tmpF08;yyval=Cyc_QualId_tok(((_tmpF08=_cycalloc(sizeof(*_tmpF08)),((_tmpF08->f1=Cyc_Absyn_Rel_n(0),((_tmpF08->f2=((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((_tmpF09[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF09)))),_tmpF08)))))));}
goto _LL23F;}case 523: _LL650: _LL651: {
# 3208
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3206 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 524: _LL652: _LL653: {
# 3209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3211 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 525: _LL654: _LL655: {
# 3214
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3212 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 526: _LL656: _LL657: {
# 3215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3215 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 527: _LL658: _LL659: {
# 3218
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3220
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3216 "parse.y"
yyval=yyyyvsp[0];
goto _LL23F;}case 528: _LL65A: _LL65B: {
# 3219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3221 "parse.y"
goto _LL23F;}case 529: _LL65C: _LL65D: {
# 3223
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3225
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3221 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL23F;}default: _LL65E: _LL65F:
# 3226
 goto _LL23F;}_LL23F:;}
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
{unsigned int _tmpF0E;unsigned int _tmpF0D;struct _dyneither_ptr _tmpF0C;char*_tmpF0B;unsigned int _tmpF0A;msg=((_tmpF0A=(unsigned int)(sze + 15),((_tmpF0B=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpF0A + 1)),((_tmpF0C=_tag_dyneither(_tmpF0B,sizeof(char),_tmpF0A + 1),((((_tmpF0D=_tmpF0A,((_tmpF0F(& _tmpF0E,& _tmpF0D,& _tmpF0B),_tmpF0B[_tmpF0D]=(char)0)))),_tmpF0C))))))));}
{const char*_tmpF10;Cyc_strcpy(msg,((_tmpF10="parse error",_tag_dyneither(_tmpF10,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 296 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(7282,x + yyn)]== x){
# 453
{const char*_tmpF12;const char*_tmpF11;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpF12=", expecting `",_tag_dyneither(_tmpF12,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpF11=" or `",_tag_dyneither(_tmpF11,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(296,x)]);
{const char*_tmpF13;Cyc_strcat(msg,((_tmpF13="'",_tag_dyneither(_tmpF13,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
const char*_tmpF14;Cyc_yyerror(((_tmpF14="parse error",_tag_dyneither(_tmpF14,sizeof(char),12))),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp816=1;_npop_handler(0);return _tmp816;}
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
 if(yyssp_offset == 0){int _tmp817=1;_npop_handler(0);return _tmp817;}
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
int _tmp818=0;_npop_handler(0);return _tmp818;}
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
union Cyc_YYSTYPE _tmp822=v;struct Cyc_Absyn_Stmt*_tmp823;struct Cyc_Absyn_Exp*_tmp824;union Cyc_Absyn_Nmspace _tmp825;struct _dyneither_ptr*_tmp826;struct _dyneither_ptr _tmp827;char _tmp828;union Cyc_Absyn_Cnst _tmp829;switch((_tmp822.Stmt_tok).tag){case 1: _LL68D: _tmp829=(_tmp822.Int_tok).val;_LL68E:
{const char*_tmpF18;void*_tmpF17[1];struct Cyc_String_pa_PrintArg_struct _tmpF16;(_tmpF16.tag=0,((_tmpF16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp829)),((_tmpF17[0]=& _tmpF16,Cyc_fprintf(Cyc_stderr,((_tmpF18="%s",_tag_dyneither(_tmpF18,sizeof(char),3))),_tag_dyneither(_tmpF17,sizeof(void*),1)))))));}goto _LL68C;case 2: _LL68F: _tmp828=(_tmp822.Char_tok).val;_LL690:
{const char*_tmpF1C;void*_tmpF1B[1];struct Cyc_Int_pa_PrintArg_struct _tmpF1A;(_tmpF1A.tag=1,((_tmpF1A.f1=(unsigned long)((int)_tmp828),((_tmpF1B[0]=& _tmpF1A,Cyc_fprintf(Cyc_stderr,((_tmpF1C="%c",_tag_dyneither(_tmpF1C,sizeof(char),3))),_tag_dyneither(_tmpF1B,sizeof(void*),1)))))));}goto _LL68C;case 3: _LL691: _tmp827=(_tmp822.String_tok).val;_LL692:
{const char*_tmpF20;void*_tmpF1F[1];struct Cyc_String_pa_PrintArg_struct _tmpF1E;(_tmpF1E.tag=0,((_tmpF1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp827),((_tmpF1F[0]=& _tmpF1E,Cyc_fprintf(Cyc_stderr,((_tmpF20="\"%s\"",_tag_dyneither(_tmpF20,sizeof(char),5))),_tag_dyneither(_tmpF1F,sizeof(void*),1)))))));}goto _LL68C;case 5: _LL693: _tmp825=((_tmp822.QualId_tok).val)->f1;_tmp826=((_tmp822.QualId_tok).val)->f2;_LL694: {
# 3230
struct Cyc_List_List*_tmp833=0;
{union Cyc_Absyn_Nmspace _tmp834=_tmp825;struct Cyc_List_List*_tmp835;struct Cyc_List_List*_tmp836;struct Cyc_List_List*_tmp837;switch((_tmp834.C_n).tag){case 1: _LL69C: _tmp837=(_tmp834.Rel_n).val;_LL69D:
 _tmp833=_tmp837;goto _LL69B;case 2: _LL69E: _tmp836=(_tmp834.Abs_n).val;_LL69F:
 _tmp833=_tmp836;goto _LL69B;case 3: _LL6A0: _tmp835=(_tmp834.C_n).val;_LL6A1:
 _tmp833=_tmp835;goto _LL69B;default: _LL6A2: _LL6A3:
 goto _LL69B;}_LL69B:;}
# 3237
for(0;_tmp833 != 0;_tmp833=_tmp833->tl){
const char*_tmpF24;void*_tmpF23[1];struct Cyc_String_pa_PrintArg_struct _tmpF22;(_tmpF22.tag=0,((_tmpF22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp833->hd)),((_tmpF23[0]=& _tmpF22,Cyc_fprintf(Cyc_stderr,((_tmpF24="%s::",_tag_dyneither(_tmpF24,sizeof(char),5))),_tag_dyneither(_tmpF23,sizeof(void*),1)))))));}
{const char*_tmpF28;void*_tmpF27[1];struct Cyc_String_pa_PrintArg_struct _tmpF26;(_tmpF26.tag=0,((_tmpF26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp826),((_tmpF27[0]=& _tmpF26,Cyc_fprintf(Cyc_stderr,((_tmpF28="%s::",_tag_dyneither(_tmpF28,sizeof(char),5))),_tag_dyneither(_tmpF27,sizeof(void*),1)))))));}
goto _LL68C;}case 7: _LL695: _tmp824=(_tmp822.Exp_tok).val;_LL696:
{const char*_tmpF2C;void*_tmpF2B[1];struct Cyc_String_pa_PrintArg_struct _tmpF2A;(_tmpF2A.tag=0,((_tmpF2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp824)),((_tmpF2B[0]=& _tmpF2A,Cyc_fprintf(Cyc_stderr,((_tmpF2C="%s",_tag_dyneither(_tmpF2C,sizeof(char),3))),_tag_dyneither(_tmpF2B,sizeof(void*),1)))))));}goto _LL68C;case 8: _LL697: _tmp823=(_tmp822.Stmt_tok).val;_LL698:
{const char*_tmpF30;void*_tmpF2F[1];struct Cyc_String_pa_PrintArg_struct _tmpF2E;(_tmpF2E.tag=0,((_tmpF2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp823)),((_tmpF2F[0]=& _tmpF2E,Cyc_fprintf(Cyc_stderr,((_tmpF30="%s",_tag_dyneither(_tmpF30,sizeof(char),3))),_tag_dyneither(_tmpF2F,sizeof(void*),1)))))));}goto _LL68C;default: _LL699: _LL69A:
{const char*_tmpF33;void*_tmpF32;(_tmpF32=0,Cyc_fprintf(Cyc_stderr,((_tmpF33="?",_tag_dyneither(_tmpF33,sizeof(char),2))),_tag_dyneither(_tmpF32,sizeof(void*),0)));}goto _LL68C;}_LL68C:;}
# 3248
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp846=_new_region("yyr");struct _RegionHandle*yyr=& _tmp846;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp847=Cyc_Parse_parse_result;_npop_handler(0);return _tmp847;};
# 3251
;_pop_region(yyr);};}
