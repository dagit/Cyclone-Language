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
# 73 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 79
typedef void*Cyc_Tcpat_decision_opt_t;
# 87
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 99
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 103
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 106
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 111
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
# 162
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 183
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 188
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 195
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 215
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 217
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 226
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 287
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 334
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 351
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 386
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 391
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 446 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 462
typedef void*Cyc_Absyn_funcparams_t;
# 465
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 501
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 508
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
# 529
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 555
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 562
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 580
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 743 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 914
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 941
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 945
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 949
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 959
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 964
void*Cyc_Absyn_compress_kb(void*);
# 969
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 971
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 974
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 976
extern void*Cyc_Absyn_sint_typ;
# 978
void*Cyc_Absyn_float_typ(int);
# 1026
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1077
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
# 1093
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1106
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1119
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1122
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1125
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1129
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1134
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1136
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1143
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1152
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1157
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1163
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1188
int Cyc_Absyn_fntype_att(void*a);
# 1207
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
# 43 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 54
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 73
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 77
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 88
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 124 "tcenv.h"
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
# 306 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 370
int Cyc_Tcutil_is_array(void*t);
# 374
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
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2:;
declarators=({struct _tuple10*_tmp4=_region_malloc(r,sizeof(*_tmp4));_tmp4->tl=declarators;_tmp4->hd=_tmp2;_tmp4;});
exprs=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=_tmp3;_tmp5->tl=exprs;_tmp5;});}
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
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);
(int)_throw((void*)({struct Cyc_Position_Exit_exn_struct*_tmp6=_cycalloc_atomic(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Position_Exit_exn_struct _tmp7;_tmp7.tag=Cyc_Position_Exit;_tmp7;});_tmp6;}));}
# 191
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
({struct Cyc_String_pa_PrintArg_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg));({void*_tmp8[2]={& _tmpA,& _tmpB};Cyc_fprintf(Cyc_stderr,({const char*_tmp9="%s: Warning: Cyclone does not yet support %s\n";_tag_dyneither(_tmp9,sizeof(char),46);}),_tag_dyneither(_tmp8,sizeof(void*),2));});});});
return;}
# 197
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 199
struct _tuple8*_tmpC=tqt;struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;_LL4: _tmpD=_tmpC->f2;_tmpE=_tmpC->f3;_LL5:;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;
({void*_tmpF=0;Cyc_Tcutil_warn(loc,({const char*_tmp10="qualifier on type is ignored";_tag_dyneither(_tmp10,sizeof(char),29);}),_tag_dyneither(_tmpF,sizeof(void*),0));});}
# 204
return _tmpE;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 207
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 213
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp11=(void*)pqs->hd;void*_tmp12=_tmp11;void*_tmp13;struct Cyc_Absyn_Exp*_tmp14;switch(*((int*)_tmp12)){case 4: _LL7: _LL8:
# 217
 zeroterm=Cyc_Absyn_true_conref;goto _LL6;case 5: _LL9: _LLA:
# 219
 zeroterm=Cyc_Absyn_false_conref;goto _LL6;case 7: _LLB: _LLC:
# 221
 nullable=Cyc_Absyn_true_conref;goto _LL6;case 6: _LLD: _LLE:
# 223
 nullable=Cyc_Absyn_false_conref;goto _LL6;case 3: _LLF: _LL10:
# 225
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL6;case 2: _LL11: _LL12:
# 227
 bound=Cyc_Absyn_bounds_one_conref;goto _LL6;case 0: _LL13: _tmp14=((struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp12)->f1;_LL14:
# 229
 bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp16;_tmp16.tag=1;_tmp16.f1=_tmp14;_tmp16;});_tmp15;}));goto _LL6;default: _LL15: _tmp13=(void*)((struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12)->f1;_LL16:
# 231
 rgn=_tmp13;goto _LL6;}_LL6:;}
# 234
return({struct _tuple14 _tmp17;_tmp17.f1=nullable;_tmp17.f2=bound;_tmp17.f3=zeroterm;_tmp17.f4=rgn;_tmp17;});}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
return({struct _tuple0*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->f1=Cyc_Absyn_Rel_n(0);_tmp18->f2=({struct _dyneither_ptr*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C;_tmp1C.tag=1;_tmp1C.f1=(unsigned long)enum_counter ++;({void*_tmp1A[1]={& _tmp1C};Cyc_aprintf(({const char*_tmp1B="__anonymous_enum_%d__";_tag_dyneither(_tmp1B,sizeof(char),22);}),_tag_dyneither(_tmp1A,sizeof(void*),1));});});_tmp19;});_tmp18;});}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple17*_tmp1D=field_info;struct _tuple0*_tmp1E;struct Cyc_Absyn_Tqual _tmp1F;void*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_List_List*_tmp22;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp24;_LL18: _tmp1E=(_tmp1D->f1)->f1;_tmp1F=(_tmp1D->f1)->f2;_tmp20=(_tmp1D->f1)->f3;_tmp21=(_tmp1D->f1)->f4;_tmp22=(_tmp1D->f1)->f5;_tmp23=(_tmp1D->f2)->f1;_tmp24=(_tmp1D->f2)->f2;_LL19:;
if(_tmp21 != 0)
Cyc_Parse_err(({const char*_tmp25="bad type params in struct field";_tag_dyneither(_tmp25,sizeof(char),32);}),loc);
if(Cyc_Absyn_is_qvar_qualified(_tmp1E))
Cyc_Parse_err(({const char*_tmp26="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp26,sizeof(char),59);}),loc);
return({struct Cyc_Absyn_Aggrfield*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->name=(*_tmp1E).f2;_tmp27->tq=_tmp1F;_tmp27->type=_tmp20;_tmp27->width=_tmp23;_tmp27->attributes=_tmp22;_tmp27->requires_clause=_tmp24;_tmp27;});}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp28;_tmp28.Signed_spec=0;_tmp28.Unsigned_spec=0;_tmp28.Short_spec=0;_tmp28.Long_spec=0;_tmp28.Long_Long_spec=0;_tmp28.Valid_type_spec=0;_tmp28.Type_spec=Cyc_Absyn_sint_typ;_tmp28.loc=loc;_tmp28;});}
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
struct _tuple8*_tmp2F=(struct _tuple8*)x->hd;struct _tuple8*_tmp30=_tmp2F;struct _dyneither_ptr _tmp31;void**_tmp32;struct _dyneither_ptr*_tmp33;void*_tmp34;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)((struct _tuple8*)_tmp30)->f3)->tag == 19){_LL1B: _tmp33=_tmp30->f1;_tmp34=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp30->f3)->f1;if(_tmp33 != 0){_LL1C:
# 321
{void*_tmp35=_tmp34;void**_tmp36;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35)->tag == 1){_LL22: _tmp36=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35)->f2;_LL23: {
# 325
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp33);({void*_tmp3D[1]={& _tmp3F};Cyc_aprintf(({const char*_tmp3E="`%s";_tag_dyneither(_tmp3E,sizeof(char),4);}),_tag_dyneither(_tmp3D,sizeof(void*),1));});});_tmp3C;});
*_tmp36=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp38;_tmp38.tag=2;_tmp38.f1=({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=nm;_tmp39->identity=- 1;_tmp39->kind=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=& Cyc_Tcutil_ik;_tmp3B;});_tmp3A;});_tmp39;});_tmp38;});_tmp37;});
goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 330
_tmp2E=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->hd=({struct _tuple18*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=_tmp33;_tmp41->f2=_tmp34;_tmp41;});_tmp40->tl=_tmp2E;_tmp40;});goto _LL1A;}else{if(((struct _tuple8*)_tmp30)->f1 != 0)goto _LL1F;else{goto _LL1F;}}}else{if(((struct _tuple8*)_tmp30)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp30)->f3)->tag == 15){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp30)->f3)->f1)->tag == 1){_LL1D: _tmp31=*_tmp30->f1;_tmp32=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp30->f3)->f1)->f2;_LL1E: {
# 334
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31);({void*_tmp48[1]={& _tmp4A};Cyc_aprintf(({const char*_tmp49="`%s";_tag_dyneither(_tmp49,sizeof(char),4);}),_tag_dyneither(_tmp48,sizeof(void*),1));});});_tmp47;});
*_tmp32=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp43;_tmp43.tag=2;_tmp43.f1=({struct Cyc_Absyn_Tvar*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->name=nm;_tmp44->identity=- 1;_tmp44->kind=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp46;_tmp46.tag=0;_tmp46.f1=& Cyc_Tcutil_rk;_tmp46;});_tmp45;});_tmp44;});_tmp43;});_tmp42;});
goto _LL1A;}}else{goto _LL1F;}}else{goto _LL1F;}}else{_LL1F: _LL20:
 goto _LL1A;}}_LL1A:;}
# 340
return _tmp2E;}
# 344
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp4B=0;
for(0;x != 0;x=x->tl){
void*_tmp4C=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp4D=_tmp4C;void*_tmp4E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4D)->tag == 19){_LL27: _tmp4E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4D)->f1;_LL28:
# 349
 _tmp4B=({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->hd=({struct _tuple18*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name;_tmp50->f2=_tmp4E;_tmp50;});_tmp4F->tl=_tmp4B;_tmp4F;});goto _LL26;}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}
# 353
return _tmp4B;}
# 357
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp51=e->r;void*_tmp52=_tmp51;struct _dyneither_ptr*_tmp53;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->tag == 1){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->f1)->tag == 0){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->f1)->f1)->f1).Rel_n).val == 0){_LL2C: _tmp53=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp52)->f1)->f1)->f2;_LL2D:
# 360
{struct Cyc_List_List*_tmp54=tags;for(0;_tmp54 != 0;_tmp54=_tmp54->tl){
struct _tuple18*_tmp55=(struct _tuple18*)_tmp54->hd;struct _tuple18*_tmp56=_tmp55;struct _dyneither_ptr*_tmp57;void*_tmp58;_LL31: _tmp57=_tmp56->f1;_tmp58=_tmp56->f2;_LL32:;
if(Cyc_strptrcmp(_tmp57,_tmp53)== 0)
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp5A;_tmp5A.tag=38;_tmp5A.f1=Cyc_Tcutil_copy_type(_tmp58);_tmp5A;});_tmp59;}),e->loc);}}
# 365
goto _LL2B;}else{goto _LL2E;}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 368
return e;}
# 373
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp5B=t;void*_tmp5C;struct Cyc_Absyn_Tqual _tmp5D;void*_tmp5E;union Cyc_Absyn_Constraint*_tmp5F;union Cyc_Absyn_Constraint*_tmp60;union Cyc_Absyn_Constraint*_tmp61;struct Cyc_Absyn_PtrLoc*_tmp62;void*_tmp63;struct Cyc_Absyn_Tqual _tmp64;struct Cyc_Absyn_Exp*_tmp65;union Cyc_Absyn_Constraint*_tmp66;unsigned int _tmp67;switch(*((int*)_tmp5B)){case 8: _LL34: _tmp63=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).elt_type;_tmp64=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).tq;_tmp65=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).num_elts;_tmp66=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).zero_term;_tmp67=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->f1).zt_loc;_LL35: {
# 376
struct Cyc_Absyn_Exp*nelts2=_tmp65;
if(_tmp65 != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp65);{
# 380
void*_tmp68=Cyc_Parse_substitute_tags(tags,_tmp63);
if(_tmp65 != nelts2  || _tmp63 != _tmp68)
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp6A;_tmp6A.tag=8;_tmp6A.f1=({struct Cyc_Absyn_ArrayInfo _tmp6B;_tmp6B.elt_type=_tmp68;_tmp6B.tq=_tmp64;_tmp6B.num_elts=nelts2;_tmp6B.zero_term=_tmp66;_tmp6B.zt_loc=_tmp67;_tmp6B;});_tmp6A;});_tmp69;});
goto _LL33;};}case 5: _LL36: _tmp5C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).elt_typ;_tmp5D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).elt_tq;_tmp5E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).rgn;_tmp5F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).nullable;_tmp60=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).bounds;_tmp61=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).zero_term;_tmp62=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B)->f1).ptr_atts).ptrloc;_LL37: {
# 385
void*_tmp6C=Cyc_Parse_substitute_tags(tags,_tmp5C);
union Cyc_Absyn_Constraint*_tmp6D=_tmp60;
{union Cyc_Absyn_Constraint _tmp6E=*_tmp60;union Cyc_Absyn_Constraint _tmp6F=_tmp6E;struct Cyc_Absyn_Exp*_tmp70;if((_tmp6F.Eq_constr).tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp6F.Eq_constr).val)->tag == 1){_LL3B: _tmp70=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp6F.Eq_constr).val)->f1;_LL3C:
# 389
 _tmp70=Cyc_Parse_substitute_tags_exp(tags,_tmp70);
_tmp6D=({union Cyc_Absyn_Constraint*_tmp71=_cycalloc(sizeof(*_tmp71));(_tmp71->Eq_constr).val=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp73;_tmp73.tag=1;_tmp73.f1=_tmp70;_tmp73;});_tmp72;});(_tmp71->Eq_constr).tag=1;_tmp71;});
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}
# 394
if(_tmp6C != _tmp5C  || _tmp6D != _tmp60)
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp75;_tmp75.tag=5;_tmp75.f1=({struct Cyc_Absyn_PtrInfo _tmp76;_tmp76.elt_typ=_tmp6C;_tmp76.elt_tq=_tmp5D;_tmp76.ptr_atts=({(_tmp76.ptr_atts).rgn=_tmp5E;(_tmp76.ptr_atts).nullable=_tmp5F;(_tmp76.ptr_atts).bounds=_tmp6D;(_tmp76.ptr_atts).zero_term=_tmp61;(_tmp76.ptr_atts).ptrloc=_tmp62;_tmp76.ptr_atts;});_tmp76;});_tmp75;});_tmp74;});
goto _LL33;}default: _LL38: _LL39:
# 399
 goto _LL33;}_LL33:;}
# 401
return t;}
# 406
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 413
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 415
return({struct _tuple19*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=(*t).f2;_tmp77->f2=(*t).f3;_tmp77;});}
# 418
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp78=d->r;void*_tmp79=_tmp78;struct Cyc_Absyn_Vardecl*_tmp7A;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79)->tag == 0){_LL40: _tmp7A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79)->f1;_LL41:
 return _tmp7A;}else{_LL42: _LL43:
({void*_tmp7B=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,({const char*_tmp7C="bad declaration in `forarray' statement";_tag_dyneither(_tmp7C,sizeof(char),40);}),_tag_dyneither(_tmp7B,sizeof(void*),0));});}_LL3F:;}
# 425
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp7D=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp7D)->tag == 4){_LL45: _LL46:
 return 1;}else{_LL47: _LL48:
 return 0;}_LL44:;}
# 434
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp7E="`H";_tag_dyneither(_tmp7E,sizeof(char),3);}))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp7F="`U";_tag_dyneither(_tmp7F,sizeof(char),3);}))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp80="`RC";_tag_dyneither(_tmp80,sizeof(char),4);}))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 442
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp82;_tmp82.tag=2;_tmp82.f1=({struct Cyc_Absyn_Tvar*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->name=({struct _dyneither_ptr*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=s;_tmp84;});_tmp83->identity=- 1;_tmp83->kind=k;_tmp83;});_tmp82;});_tmp81;});}}}}
# 445
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp85=Cyc_Absyn_compress_kb(t->kind);void*_tmp86=_tmp85;void*_tmp87;struct Cyc_Absyn_Kind*_tmp88;switch(*((int*)_tmp86)){case 1: _LL4A: _LL4B:
 k=(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8A;_tmp8A.tag=1;_tmp8A.f1=0;_tmp8A;});_tmp89;});goto _LL49;case 2: _LL4C: _tmp88=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86)->f2;_LL4D:
 k=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8C;_tmp8C.tag=2;_tmp8C.f1=0;_tmp8C.f2=_tmp88;_tmp8C;});_tmp8B;});goto _LL49;default: _LL4E: _tmp87=_tmp86;_LL4F:
 k=_tmp87;goto _LL49;}_LL49:;}
# 452
return({struct Cyc_Absyn_Tvar*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->name=t->name;_tmp8D->identity=- 1;_tmp8D->kind=k;_tmp8D;});}
# 459
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp8E=t;struct Cyc_Absyn_Tvar*_tmp8F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8E)->tag == 2){_LL51: _tmp8F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8E)->f1;_LL52:
 return _tmp8F;}else{_LL53: _LL54:
({void*_tmp90=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmp91="expecting a list of type variables, not types";_tag_dyneither(_tmp91,sizeof(char),46);}),_tag_dyneither(_tmp90,sizeof(void*),0));});}_LL50:;}
# 465
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp93;_tmp93.tag=2;_tmp93.f1=pr;_tmp93;});_tmp92;});}
# 470
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp94=Cyc_Tcutil_compress(t);void*_tmp95=_tmp94;void**_tmp96;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp95)->tag == 2){_LL56: _tmp96=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp95)->f1)->kind;_LL57: {
# 473
void*_tmp97=Cyc_Absyn_compress_kb(*_tmp96);void*_tmp98=_tmp97;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp98)->tag == 1){_LL5B: _LL5C:
# 475
 if(!leq)*_tmp96=Cyc_Tcutil_kind_to_bound(k);else{
*_tmp96=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9A;_tmp9A.tag=2;_tmp9A.f1=0;_tmp9A.f2=k;_tmp9A;});_tmp99;});}
return;}else{_LL5D: _LL5E:
 return;}_LL5A:;}}else{_LL58: _LL59:
# 480
 return;}_LL55:;}
# 485
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 491
if(tds == 0)return tms;
# 496
if(tms == 0)return 0;{
# 498
void*_tmp9B=(void*)tms->hd;void*_tmp9C=_tmp9B;void*_tmp9D;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp9C)->tag == 3){_LL60: _tmp9D=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp9C)->f1;_LL61:
# 501
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 504
void*_tmp9E=_tmp9D;struct Cyc_List_List*_tmp9F;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp9E)->tag == 1){_LL65: _LL66:
# 506
({void*_tmpA0=0;Cyc_Tcutil_warn(loc,({const char*_tmpA1="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmpA1,sizeof(char),93);}),_tag_dyneither(_tmpA0,sizeof(void*),0));});
# 508
return tms;}else{_LL67: _tmp9F=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp9E)->f1;_LL68:
# 510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds))
({void*_tmpA2=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmpA3="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmpA3,sizeof(char),73);}),_tag_dyneither(_tmpA2,sizeof(void*),0));});{
# 514
struct Cyc_List_List*rev_new_params=0;
for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
struct Cyc_List_List*_tmpA4=tds;
for(0;_tmpA4 != 0;_tmpA4=_tmpA4->tl){
struct Cyc_Absyn_Decl*_tmpA5=(struct Cyc_Absyn_Decl*)_tmpA4->hd;
void*_tmpA6=_tmpA5->r;void*_tmpA7=_tmpA6;struct Cyc_Absyn_Vardecl*_tmpA8;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA7)->tag == 0){_LL6A: _tmpA8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA7)->f1;_LL6B:
# 521
 if(Cyc_zstrptrcmp((*_tmpA8->name).f2,(struct _dyneither_ptr*)_tmp9F->hd)!= 0)
continue;
if(_tmpA8->initializer != 0)
({void*_tmpA9=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpA5->loc,({const char*_tmpAA="initializer found in parameter declaration";_tag_dyneither(_tmpAA,sizeof(char),43);}),_tag_dyneither(_tmpA9,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmpA8->name))
({void*_tmpAB=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpA5->loc,({const char*_tmpAC="namespaces forbidden in parameter declarations";_tag_dyneither(_tmpAC,sizeof(char),47);}),_tag_dyneither(_tmpAB,sizeof(void*),0));});
rev_new_params=({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->hd=({struct _tuple8*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->f1=(*_tmpA8->name).f2;_tmpAE->f2=_tmpA8->tq;_tmpAE->f3=_tmpA8->type;_tmpAE;});_tmpAD->tl=rev_new_params;_tmpAD;});
# 530
goto L;}else{_LL6C: _LL6D:
({void*_tmpAF=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpA5->loc,({const char*_tmpB0="nonvariable declaration in parameter type";_tag_dyneither(_tmpB0,sizeof(char),42);}),_tag_dyneither(_tmpAF,sizeof(void*),0));});}_LL69:;}
# 534
L: if(_tmpA4 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpB3;_tmpB3.tag=0;_tmpB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp9F->hd));({void*_tmpB1[1]={& _tmpB3};((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmpB2="%s is not given a type";_tag_dyneither(_tmpB2,sizeof(char),23);}),_tag_dyneither(_tmpB1,sizeof(void*),1));});});}
# 537
return({struct Cyc_List_List*_tmpB4=_region_malloc(yy,sizeof(*_tmpB4));_tmpB4->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB5=_region_malloc(yy,sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB6;_tmpB6.tag=3;_tmpB6.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB7=_region_malloc(yy,sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB8;_tmpB8.tag=1;_tmpB8.f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmpB8.f2=0;_tmpB8.f3=0;_tmpB8.f4=0;_tmpB8.f5=0;_tmpB8.f6=0;_tmpB8.f7=0;_tmpB8;});_tmpB7;});_tmpB6;});_tmpB5;});_tmpB4->tl=0;_tmpB4;});};}_LL64:;}
# 544
goto _LL63;}else{_LL62: _LL63:
 return({struct Cyc_List_List*_tmpB9=_region_malloc(yy,sizeof(*_tmpB9));_tmpB9->hd=(void*)tms->hd;_tmpB9->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmpB9;});}_LL5F:;};}
# 552
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 556
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmpBA;_tmpBA.id=d.id;_tmpBA.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmpBA;});{
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
enum Cyc_Parse_Storage_class _tmpBB=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmpBC=_tmpBB;switch(_tmpBC){case Cyc_Parse_Extern_sc: _LL6F: _LL70:
 sc=Cyc_Absyn_Extern;goto _LL6E;case Cyc_Parse_Static_sc: _LL71: _LL72:
 sc=Cyc_Absyn_Static;goto _LL6E;default: _LL73: _LL74:
 Cyc_Parse_err(({const char*_tmpBD="bad storage class on function";_tag_dyneither(_tmpBD,sizeof(char),30);}),loc);goto _LL6E;}_LL6E:;}}{
# 578
void*_tmpBE=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpBF=Cyc_Parse_apply_tms(tq,_tmpBE,atts,d.tms);struct _tuple13 _tmpC0=_tmpBF;struct Cyc_Absyn_Tqual _tmpC1;void*_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;_LL76: _tmpC1=_tmpC0.f1;_tmpC2=_tmpC0.f2;_tmpC3=_tmpC0.f3;_tmpC4=_tmpC0.f4;_LL77:;
# 583
if(_tmpC3 != 0)
# 586
({void*_tmpC5=0;Cyc_Tcutil_warn(loc,({const char*_tmpC6="bad type params, ignoring";_tag_dyneither(_tmpC6,sizeof(char),26);}),_tag_dyneither(_tmpC5,sizeof(void*),0));});{
# 588
void*_tmpC7=_tmpC2;struct Cyc_List_List*_tmpC8;void*_tmpC9;struct Cyc_Absyn_Tqual _tmpCA;void*_tmpCB;struct Cyc_List_List*_tmpCC;int _tmpCD;struct Cyc_Absyn_VarargInfo*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_List_List*_tmpD4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->tag == 9){_LL79: _tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).tvars;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).effect;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ret_tqual;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ret_typ;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).args;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).c_varargs;_tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).cyc_varargs;_tmpCF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).rgn_po;_tmpD0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).attributes;_tmpD1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).requires_clause;_tmpD2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).requires_relns;_tmpD3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ensures_clause;_tmpD4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7)->f1).ensures_relns;_LL7A: {
# 592
struct Cyc_List_List*_tmpD5=0;
{struct Cyc_List_List*_tmpD6=_tmpCC;for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
struct _tuple8*_tmpD7=(struct _tuple8*)_tmpD6->hd;struct _tuple8*_tmpD8=_tmpD7;struct _dyneither_ptr*_tmpD9;struct Cyc_Absyn_Tqual _tmpDA;void*_tmpDB;_LL7E: _tmpD9=_tmpD8->f1;_tmpDA=_tmpD8->f2;_tmpDB=_tmpD8->f3;_LL7F:;
if(_tmpD9 == 0){
Cyc_Parse_err(({const char*_tmpDC="missing argument variable in function prototype";_tag_dyneither(_tmpDC,sizeof(char),48);}),loc);
_tmpD5=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=({struct _tuple8*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2);_tmpDE->f2=_tmpDA;_tmpDE->f3=_tmpDB;_tmpDE;});_tmpDD->tl=_tmpD5;_tmpDD;});}else{
# 599
_tmpD5=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->hd=({struct _tuple8*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->f1=_tmpD9;_tmpE1->f2=_tmpDA;_tmpE1->f3=_tmpDB;_tmpE1;});_tmpE0->tl=_tmpD5;_tmpE0;});}}}
# 603
return({struct Cyc_Absyn_Fndecl*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->sc=sc;_tmpE2->name=d.id;_tmpE2->tvs=_tmpC8;_tmpE2->is_inline=is_inline;_tmpE2->effect=_tmpC9;_tmpE2->ret_tqual=_tmpCA;_tmpE2->ret_type=_tmpCB;_tmpE2->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD5);_tmpE2->c_varargs=_tmpCD;_tmpE2->cyc_varargs=_tmpCE;_tmpE2->rgn_po=_tmpCF;_tmpE2->body=body;_tmpE2->cached_typ=0;_tmpE2->param_vardecls=0;_tmpE2->fn_vardecl=0;_tmpE2->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpD0,_tmpC4);_tmpE2->requires_clause=_tmpD1;_tmpE2->requires_relns=0;_tmpE2->ensures_clause=_tmpD3;_tmpE2->ensures_relns=0;_tmpE2;});}}else{_LL7B: _LL7C:
# 618
({void*_tmpE3=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmpE4="declarator is not a function prototype";_tag_dyneither(_tmpE4,sizeof(char),39);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});}_LL78:;};};};}static char _tmpE5[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
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
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpE9=0;Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpE9,sizeof(void*),0));});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpEA=0;Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpEA,sizeof(void*),0));});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpEB=0;Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpEB,sizeof(void*),0));});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpEC=0;Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmpEC,sizeof(void*),0));});
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
if(!seen_sign  && !seen_size)
({void*_tmpED=0;Cyc_Tcutil_warn(loc,({const char*_tmpEE="missing type within specifier";_tag_dyneither(_tmpEE,sizeof(char),30);}),_tag_dyneither(_tmpED,sizeof(void*),0));});
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 693
if(seen_sign){
void*_tmpEF=t;enum Cyc_Absyn_Sign _tmpF0;enum Cyc_Absyn_Size_of _tmpF1;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpEF)->tag == 6){_LL81: _tmpF0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpEF)->f1;_tmpF1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpEF)->f2;_LL82:
# 696
 if(_tmpF0 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmpF1);
goto _LL80;}else{_LL83: _LL84:
 Cyc_Parse_err(({const char*_tmpF2="sign specification on non-integral type";_tag_dyneither(_tmpF2,sizeof(char),40);}),loc);goto _LL80;}_LL80:;}
# 701
if(seen_size){
void*_tmpF3=t;enum Cyc_Absyn_Sign _tmpF4;enum Cyc_Absyn_Size_of _tmpF5;switch(*((int*)_tmpF3)){case 6: _LL86: _tmpF4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpF3)->f1;_tmpF5=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpF3)->f2;_LL87:
# 704
 if(_tmpF5 != sz)
t=Cyc_Absyn_int_typ(_tmpF4,sz);
goto _LL85;case 7: _LL88: _LL89:
# 708
 t=Cyc_Absyn_float_typ(2);goto _LL85;default: _LL8A: _LL8B:
 Cyc_Parse_err(({const char*_tmpF6="size qualifier on non-integral type";_tag_dyneither(_tmpF6,sizeof(char),36);}),loc);goto _LL85;}_LL85:;}}
# 712
return t;}
# 715
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 719
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpF7=d.id;
struct _tuple13 _tmpF8=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpF9=_tmpF8;struct Cyc_Absyn_Tqual _tmpFA;void*_tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFD;_LL8D: _tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;_tmpFC=_tmpF9.f3;_tmpFD=_tmpF9.f4;_LL8E:;
# 725
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpFE=_region_malloc(r,sizeof(*_tmpFE));_tmpFE->hd=({struct _tuple15*_tmpFF=_region_malloc(r,sizeof(*_tmpFF));_tmpFF->f1=_tmpF7;_tmpFF->f2=_tmpFA;_tmpFF->f3=_tmpFB;_tmpFF->f4=_tmpFC;_tmpFF->f5=_tmpFD;_tmpFF;});_tmpFE->tl=0;_tmpFE;});else{
# 728
return({struct Cyc_List_List*_tmp100=_region_malloc(r,sizeof(*_tmp100));_tmp100->hd=({struct _tuple15*_tmp101=_region_malloc(r,sizeof(*_tmp101));_tmp101->f1=_tmpF7;_tmp101->f2=_tmpFA;_tmp101->f3=_tmpFB;_tmp101->f4=_tmpFC;_tmp101->f5=_tmpFD;_tmp101;});_tmp100->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmp100;});}};}
# 732
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 735
if(tms == 0)return({struct _tuple13 _tmp102;_tmp102.f1=tq;_tmp102.f2=t;_tmp102.f3=0;_tmp102.f4=atts;_tmp102;});{
void*_tmp103=(void*)tms->hd;void*_tmp104=_tmp103;unsigned int _tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_PtrAtts _tmp107;struct Cyc_Absyn_Tqual _tmp108;struct Cyc_List_List*_tmp109;unsigned int _tmp10A;void*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;union Cyc_Absyn_Constraint*_tmp10D;unsigned int _tmp10E;union Cyc_Absyn_Constraint*_tmp10F;unsigned int _tmp110;switch(*((int*)_tmp104)){case 0: _LL90: _tmp10F=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp110=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL91:
# 738
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp10F,_tmp110),atts,tms->tl);case 1: _LL92: _tmp10C=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp10D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_tmp10E=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp104)->f3;_LL93:
# 741
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp10C,_tmp10D,_tmp10E),atts,tms->tl);case 3: _LL94: _tmp10B=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_LL95: {
# 744
void*_tmp111=_tmp10B;unsigned int _tmp112;struct Cyc_List_List*_tmp113;int _tmp114;struct Cyc_Absyn_VarargInfo*_tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->tag == 1){_LL9D: _tmp113=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f1;_tmp114=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f2;_tmp115=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f3;_tmp116=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f4;_tmp117=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f5;_tmp118=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f6;_tmp119=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111)->f7;_LL9E: {
# 746
struct Cyc_List_List*typvars=0;
# 748
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->hd=(void*)as->hd;_tmp11A->tl=fn_atts;_tmp11A;});else{
# 753
new_atts=({struct Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->hd=(void*)as->hd;_tmp11B->tl=new_atts;_tmp11B;});}}}
# 756
if(tms->tl != 0){
void*_tmp11C=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp11D=_tmp11C;struct Cyc_List_List*_tmp11E;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11D)->tag == 4){_LLA2: _tmp11E=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11D)->f1;_LLA3:
# 759
 typvars=_tmp11E;
tms=tms->tl;
goto _LLA1;}else{_LLA4: _LLA5:
# 763
 goto _LLA1;}_LLA1:;}
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
struct _tuple8*_tmp121=(struct _tuple8*)_tmp120->hd;struct _tuple8*_tmp122=_tmp121;struct _dyneither_ptr*_tmp123;struct Cyc_Absyn_Tqual _tmp124;void**_tmp125;_LLA7: _tmp123=_tmp122->f1;_tmp124=_tmp122->f2;_tmp125=(void**)& _tmp122->f3;_LLA8:;
if(_tmp11F != 0)
*_tmp125=Cyc_Parse_substitute_tags(_tmp11F,*_tmp125);
*_tmp125=Cyc_Parse_array2ptr(*_tmp125,1);}}
# 794
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp116,tq,t,_tmp113,_tmp114,_tmp115,_tmp117,fn_atts,_tmp118,_tmp119),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}}else{_LL9F: _tmp112=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp111)->f2;_LLA0:
# 801
({void*_tmp126=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp112,({const char*_tmp127="function declaration without parameter types";_tag_dyneither(_tmp127,sizeof(char),45);}),_tag_dyneither(_tmp126,sizeof(void*),0));});}_LL9C:;}case 4: _LL96: _tmp109=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp10A=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL97:
# 808
 if(tms->tl == 0)
return({struct _tuple13 _tmp128;_tmp128.f1=tq;_tmp128.f2=t;_tmp128.f3=_tmp109;_tmp128.f4=atts;_tmp128;});
# 813
({void*_tmp129=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp10A,({const char*_tmp12A="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmp12A,sizeof(char),68);}),_tag_dyneither(_tmp129,sizeof(void*),0));});case 2: _LL98: _tmp107=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp108=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL99:
# 816
 return Cyc_Parse_apply_tms(_tmp108,(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12C;_tmp12C.tag=5;_tmp12C.f1=({struct Cyc_Absyn_PtrInfo _tmp12D;_tmp12D.elt_typ=t;_tmp12D.elt_tq=tq;_tmp12D.ptr_atts=_tmp107;_tmp12D;});_tmp12C;});_tmp12B;}),atts,tms->tl);default: _LL9A: _tmp105=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp104)->f1;_tmp106=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp104)->f2;_LL9B:
# 821
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp106),tms->tl);}_LL8F:;};}
# 827
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 836
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmp12E=t;struct _tuple0*_tmp12F;struct Cyc_Absyn_Tqual _tmp130;void*_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_List_List*_tmp133;_LLAA: _tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;_tmp131=_tmp12E->f3;_tmp132=_tmp12E->f4;_tmp133=_tmp12E->f5;_LLAB:;
# 839
Cyc_Lex_register_typedef(_tmp12F);{
# 841
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp134=_tmp131;struct Cyc_Core_Opt*_tmp135;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp134)->tag == 1){_LLAD: _tmp135=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp134)->f1;_LLAE:
# 845
 type=0;
if(_tmp135 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp135;}
goto _LLAC;}else{_LLAF: _LLB0:
 kind=0;type=_tmp131;goto _LLAC;}_LLAC:;}
# 851
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp137;_tmp137.tag=8;_tmp137.f1=({struct Cyc_Absyn_Typedefdecl*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->name=_tmp12F;_tmp138->tvs=_tmp132;_tmp138->kind=kind;_tmp138->defn=type;_tmp138->atts=_tmp133;_tmp138->tq=_tmp130;_tmp138->extern_c=0;_tmp138;});_tmp137;});_tmp136;}),loc);};}
# 858
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139[0]=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp13A;_tmp13A.tag=12;_tmp13A.f1=d;_tmp13A.f2=s;_tmp13A;});_tmp139;}),d->loc);}
# 864
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 873
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 877
struct _RegionHandle _tmp13B=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp13B;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp13C=ds;struct Cyc_Absyn_Tqual _tmp13D;struct Cyc_Parse_Type_specifier _tmp13E;struct Cyc_List_List*_tmp13F;_LLB2: _tmp13D=_tmp13C.tq;_tmp13E=_tmp13C.type_specs;_tmp13F=_tmp13C.attributes;_LLB3:;
if(_tmp13D.loc == 0)_tmp13D.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp140=0;Cyc_Tcutil_warn(loc,({const char*_tmp141="inline qualifier on non-function definition";_tag_dyneither(_tmp141,sizeof(char),44);}),_tag_dyneither(_tmp140,sizeof(void*),0));});{
# 883
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp142=*ds.sc;enum Cyc_Parse_Storage_class _tmp143=_tmp142;switch(_tmp143){case Cyc_Parse_Typedef_sc: _LLB5: _LLB6:
 istypedef=1;goto _LLB4;case Cyc_Parse_Extern_sc: _LLB7: _LLB8:
 s=Cyc_Absyn_Extern;goto _LLB4;case Cyc_Parse_ExternC_sc: _LLB9: _LLBA:
 s=Cyc_Absyn_ExternC;goto _LLB4;case Cyc_Parse_Static_sc: _LLBB: _LLBC:
 s=Cyc_Absyn_Static;goto _LLB4;case Cyc_Parse_Auto_sc: _LLBD: _LLBE:
 s=Cyc_Absyn_Public;goto _LLB4;case Cyc_Parse_Register_sc: _LLBF: _LLC0:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}goto _LLB4;default: _LLC1: _LLC2:
 s=Cyc_Absyn_Abstract;goto _LLB4;}_LLB4:;}{
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
void*_tmp145=_tmp144;struct Cyc_List_List*_tmp146;struct _tuple0*_tmp147;struct _tuple0*_tmp148;int _tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_Absyn_Datatypedecl**_tmp14B;enum Cyc_Absyn_AggrKind _tmp14C;struct _tuple0*_tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Datatypedecl*_tmp14F;struct Cyc_Absyn_Enumdecl*_tmp150;struct Cyc_Absyn_Aggrdecl*_tmp151;switch(*((int*)_tmp145)){case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)){case 0: _LLC4: _tmp151=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)->f1;_LLC5:
# 917
 _tmp151->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp151->attributes,_tmp13F);
_tmp151->sc=s;{
struct Cyc_List_List*_tmp155=({struct Cyc_List_List*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp154;_tmp154.tag=5;_tmp154.f1=_tmp151;_tmp154;});_tmp153;}),loc);_tmp152->tl=0;_tmp152;});_npop_handler(0);return _tmp155;};case 1: _LLC6: _tmp150=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)->f1;_LLC7:
# 921
 if(_tmp13F != 0)Cyc_Parse_err(({const char*_tmp156="attributes on enum not supported";_tag_dyneither(_tmp156,sizeof(char),33);}),loc);
_tmp150->sc=s;{
struct Cyc_List_List*_tmp15A=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp159;_tmp159.tag=7;_tmp159.f1=_tmp150;_tmp159;});_tmp158;}),loc);_tmp157->tl=0;_tmp157;});_npop_handler(0);return _tmp15A;};default: _LLC8: _tmp14F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp145)->f1)->r)->f1;_LLC9:
# 925
 if(_tmp13F != 0)Cyc_Parse_err(({const char*_tmp15B="attributes on datatypes not supported";_tag_dyneither(_tmp15B,sizeof(char),38);}),loc);
_tmp14F->sc=s;{
struct Cyc_List_List*_tmp15F=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp15E;_tmp15E.tag=6;_tmp15E.f1=_tmp14F;_tmp15E;});_tmp15D;}),loc);_tmp15C->tl=0;_tmp15C;});_npop_handler(0);return _tmp15F;};}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).aggr_info).UnknownAggr).tag == 1){_LLCA: _tmp14C=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).aggr_info).UnknownAggr).val).f1;_tmp14D=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).aggr_info).UnknownAggr).val).f2;_tmp14E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp145)->f1).targs;_LLCB: {
# 929
struct Cyc_List_List*_tmp160=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp14E);
struct Cyc_Absyn_Aggrdecl*_tmp161=({struct Cyc_Absyn_Aggrdecl*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->kind=_tmp14C;_tmp167->sc=s;_tmp167->name=_tmp14D;_tmp167->tvs=_tmp160;_tmp167->impl=0;_tmp167->attributes=0;_tmp167->expected_mem_kind=0;_tmp167;});
if(_tmp13F != 0)Cyc_Parse_err(({const char*_tmp162="bad attributes on type declaration";_tag_dyneither(_tmp162,sizeof(char),35);}),loc);{
struct Cyc_List_List*_tmp166=({struct Cyc_List_List*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp165;_tmp165.tag=5;_tmp165.f1=_tmp161;_tmp165;});_tmp164;}),loc);_tmp163->tl=0;_tmp163;});_npop_handler(0);return _tmp166;};}}else{goto _LLD4;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).KnownDatatype).tag == 2){_LLCC: _tmp14B=(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).KnownDatatype).val;_LLCD:
# 934
 if(_tmp13F != 0)Cyc_Parse_err(({const char*_tmp168="bad attributes on datatype";_tag_dyneither(_tmp168,sizeof(char),27);}),loc);{
struct Cyc_List_List*_tmp16C=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp16B;_tmp16B.tag=6;_tmp16B.f1=*_tmp14B;_tmp16B;});_tmp16A;}),loc);_tmp169->tl=0;_tmp169;});_npop_handler(0);return _tmp16C;};}else{_LLCE: _tmp148=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).UnknownDatatype).val).name;_tmp149=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp14A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp145)->f1).targs;_LLCF: {
# 937
struct Cyc_List_List*_tmp16D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp14A);
struct Cyc_Absyn_Decl*_tmp16E=Cyc_Absyn_datatype_decl(s,_tmp148,_tmp16D,0,_tmp149,loc);
if(_tmp13F != 0)Cyc_Parse_err(({const char*_tmp16F="bad attributes on datatype";_tag_dyneither(_tmp16F,sizeof(char),27);}),loc);{
struct Cyc_List_List*_tmp171=({struct Cyc_List_List*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->hd=_tmp16E;_tmp170->tl=0;_tmp170;});_npop_handler(0);return _tmp171;};}}case 13: _LLD0: _tmp147=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp145)->f1;_LLD1: {
# 942
struct Cyc_Absyn_Enumdecl*_tmp172=({struct Cyc_Absyn_Enumdecl*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->sc=s;_tmp179->name=_tmp147;_tmp179->fields=0;_tmp179;});
if(_tmp13F != 0)Cyc_Parse_err(({const char*_tmp173="bad attributes on enum";_tag_dyneither(_tmp173,sizeof(char),23);}),loc);{
struct Cyc_List_List*_tmp178=({struct Cyc_List_List*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->hd=({struct Cyc_Absyn_Decl*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp177;_tmp177.tag=7;_tmp177.f1=_tmp172;_tmp177;});_tmp176;});_tmp175->loc=loc;_tmp175;});_tmp174->tl=0;_tmp174;});_npop_handler(0);return _tmp178;};}case 14: _LLD2: _tmp146=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp145)->f1;_LLD3: {
# 948
struct Cyc_Absyn_Enumdecl*_tmp17A=({struct Cyc_Absyn_Enumdecl*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->sc=s;_tmp181->name=Cyc_Parse_gensym_enum();_tmp181->fields=({struct Cyc_Core_Opt*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->v=_tmp146;_tmp182;});_tmp181;});
if(_tmp13F != 0)Cyc_Parse_err(({const char*_tmp17B="bad attributes on enum";_tag_dyneither(_tmp17B,sizeof(char),23);}),loc);{
struct Cyc_List_List*_tmp180=({struct Cyc_List_List*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->hd=({struct Cyc_Absyn_Decl*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->r=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp17F;_tmp17F.tag=7;_tmp17F.f1=_tmp17A;_tmp17F;});_tmp17E;});_tmp17D->loc=loc;_tmp17D;});_tmp17C->tl=0;_tmp17C;});_npop_handler(0);return _tmp180;};}default: _LLD4: _LLD5:
 Cyc_Parse_err(({const char*_tmp183="missing declarator";_tag_dyneither(_tmp183,sizeof(char),19);}),loc);{struct Cyc_List_List*_tmp184=0;_npop_handler(0);return _tmp184;};}_LLC3:;}else{
# 955
struct Cyc_List_List*_tmp185=Cyc_Parse_apply_tmss(mkrgn,_tmp13D,_tmp144,declarators,_tmp13F);
if(istypedef){
# 960
if(!exps_empty)
Cyc_Parse_err(({const char*_tmp186="initializer in typedef declaration";_tag_dyneither(_tmp186,sizeof(char),35);}),loc);{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp185);
struct Cyc_List_List*_tmp187=decls;_npop_handler(0);return _tmp187;};}else{
# 966
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp188=_tmp185;for(0;_tmp188 != 0;(_tmp188=_tmp188->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp189=(struct _tuple15*)_tmp188->hd;struct _tuple15*_tmp18A=_tmp189;struct _tuple0*_tmp18B;struct Cyc_Absyn_Tqual _tmp18C;void*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18F;_LLD7: _tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;_tmp18D=_tmp18A->f3;_tmp18E=_tmp18A->f4;_tmp18F=_tmp18A->f5;_LLD8:;
if(_tmp18E != 0)
({void*_tmp190=0;Cyc_Tcutil_warn(loc,({const char*_tmp191="bad type params, ignoring";_tag_dyneither(_tmp191,sizeof(char),26);}),_tag_dyneither(_tmp190,sizeof(void*),0));});
if(exprs == 0)
({void*_tmp192=0;((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,({const char*_tmp193="unexpected NULL in parse!";_tag_dyneither(_tmp193,sizeof(char),26);}),_tag_dyneither(_tmp192,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp194=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp195=Cyc_Absyn_new_vardecl(_tmp18B,_tmp18D,_tmp194);
_tmp195->tq=_tmp18C;
_tmp195->sc=s;
_tmp195->attributes=_tmp18F;{
struct Cyc_Absyn_Decl*_tmp196=({struct Cyc_Absyn_Decl*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp19A;_tmp19A.tag=0;_tmp19A.f1=_tmp195;_tmp19A;});_tmp199;});_tmp198->loc=loc;_tmp198;});
decls=({struct Cyc_List_List*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->hd=_tmp196;_tmp197->tl=decls;_tmp197;});};};}}{
# 981
struct Cyc_List_List*_tmp19B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp19B;};}}};};};};}
# 878
;_pop_region(mkrgn);}
# 987
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp19C=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp19D=_tmp19C;switch(_tmp19D){case 65: _LLDA: _LLDB:
 return& Cyc_Tcutil_ak;case 77: _LLDC: _LLDD:
 return& Cyc_Tcutil_mk;case 66: _LLDE: _LLDF:
 return& Cyc_Tcutil_bk;case 82: _LLE0: _LLE1:
 return& Cyc_Tcutil_rk;case 69: _LLE2: _LLE3:
 return& Cyc_Tcutil_ek;case 73: _LLE4: _LLE5:
 return& Cyc_Tcutil_ik;case 85: _LLE6: _LLE7:
# 997
{char _tmp19E=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp19F=_tmp19E;switch(_tmp19F){case 82: _LLED: _LLEE:
 return& Cyc_Tcutil_urk;case 65: _LLEF: _LLF0:
 return& Cyc_Tcutil_uak;case 77: _LLF1: _LLF2:
 return& Cyc_Tcutil_umk;case 66: _LLF3: _LLF4:
 return& Cyc_Tcutil_ubk;default: _LLF5: _LLF6:
 goto _LLEC;}_LLEC:;}
# 1004
goto _LLD9;case 84: _LLE8: _LLE9:
# 1006
{char _tmp1A0=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp1A1=_tmp1A0;switch(_tmp1A1){case 82: _LLF8: _LLF9:
 return& Cyc_Tcutil_trk;case 65: _LLFA: _LLFB:
 return& Cyc_Tcutil_tak;case 77: _LLFC: _LLFD:
 return& Cyc_Tcutil_tmk;case 66: _LLFE: _LLFF:
 return& Cyc_Tcutil_tbk;default: _LL100: _LL101:
 goto _LLF7;}_LLF7:;}
# 1013
goto _LLD9;default: _LLEA: _LLEB:
 goto _LLD9;}_LLD9:;}
# 1016
Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A5;_tmp1A5.tag=1;_tmp1A5.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));({struct Cyc_String_pa_PrintArg_struct _tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1A2[2]={& _tmp1A4,& _tmp1A5};Cyc_aprintf(({const char*_tmp1A3="bad kind: %s; strlen=%d";_tag_dyneither(_tmp1A3,sizeof(char),24);}),_tag_dyneither(_tmp1A2,sizeof(void*),2));});});}),loc);
return& Cyc_Tcutil_bk;}
# 1021
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1A6=e->r;void*_tmp1A7=_tmp1A6;int _tmp1A8;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A7)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A7)->f1).Int_c).tag == 5){_LL103: _tmp1A8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A7)->f1).Int_c).val).f2;_LL104:
 return _tmp1A8;}else{goto _LL105;}}else{_LL105: _LL106:
# 1025
 Cyc_Parse_err(({const char*_tmp1A9="expecting integer constant";_tag_dyneither(_tmp1A9,sizeof(char),27);}),loc);
return 0;}_LL102:;}
# 1031
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1AA=e->r;void*_tmp1AB=_tmp1AA;struct _dyneither_ptr _tmp1AC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AB)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AB)->f1).String_c).tag == 8){_LL108: _tmp1AC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AB)->f1).String_c).val;_LL109:
 return _tmp1AC;}else{goto _LL10A;}}else{_LL10A: _LL10B:
# 1035
 Cyc_Parse_err(({const char*_tmp1AD="expecting string constant";_tag_dyneither(_tmp1AD,sizeof(char),26);}),loc);
return _tag_dyneither(0,0,0);}_LL107:;}
# 1041
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1AE=x;long long _tmp1AF;char _tmp1B0;int _tmp1B1;switch((_tmp1AE.LongLong_c).tag){case 5: _LL10D: _tmp1B1=((_tmp1AE.Int_c).val).f2;_LL10E:
 return(unsigned int)_tmp1B1;case 2: _LL10F: _tmp1B0=((_tmp1AE.Char_c).val).f2;_LL110:
 return(unsigned int)_tmp1B0;case 6: _LL111: _tmp1AF=((_tmp1AE.LongLong_c).val).f2;_LL112: {
# 1046
unsigned long long y=(unsigned long long)_tmp1AF;
if(y > -1)
Cyc_Parse_err(({const char*_tmp1B2="integer constant too large";_tag_dyneither(_tmp1B2,sizeof(char),27);}),loc);
return(unsigned int)_tmp1AF;}default: _LL113: _LL114:
# 1051
 Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B5;_tmp1B5.tag=0;_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));({void*_tmp1B3[1]={& _tmp1B5};Cyc_aprintf(({const char*_tmp1B4="expected integer constant but found %s";_tag_dyneither(_tmp1B4,sizeof(char),39);}),_tag_dyneither(_tmp1B3,sizeof(void*),1));});}),loc);
return 0;}_LL10C:;}
# 1057
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1B6=p->r;void*_tmp1B7=_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B8;struct _tuple0*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct _dyneither_ptr _tmp1BB;int _tmp1BC;char _tmp1BD;enum Cyc_Absyn_Sign _tmp1BE;int _tmp1BF;struct Cyc_Absyn_Pat*_tmp1C0;struct Cyc_Absyn_Vardecl*_tmp1C1;struct _tuple0*_tmp1C2;switch(*((int*)_tmp1B7)){case 15: _LL116: _tmp1C2=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LL117:
 return Cyc_Absyn_unknownid_exp(_tmp1C2,p->loc);case 3: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2)->r)->tag == 0){_LL118: _tmp1C1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LL119:
# 1061
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1C1->name,p->loc),p->loc);}else{goto _LL128;}case 6: _LL11A: _tmp1C0=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LL11B:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1C0),p->loc);case 9: _LL11C: _LL11D:
 return Cyc_Absyn_null_exp(p->loc);case 10: _LL11E: _tmp1BE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_tmp1BF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2;_LL11F:
 return Cyc_Absyn_int_exp(_tmp1BE,_tmp1BF,p->loc);case 11: _LL120: _tmp1BD=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LL121:
 return Cyc_Absyn_char_exp(_tmp1BD,p->loc);case 12: _LL122: _tmp1BB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_tmp1BC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2;_LL123:
 return Cyc_Absyn_float_exp(_tmp1BB,_tmp1BC,p->loc);case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B7)->f3 == 0){_LL124: _tmp1B9=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_tmp1BA=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B7)->f2;_LL125: {
# 1068
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1B9,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1BA);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL128;}case 17: _LL126: _tmp1B8=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1B7)->f1;_LL127:
 return _tmp1B8;default: _LL128: _LL129:
# 1073
 Cyc_Parse_err(({const char*_tmp1C3="cannot mix patterns and expressions in case";_tag_dyneither(_tmp1C3,sizeof(char),44);}),p->loc);
return Cyc_Absyn_null_exp(p->loc);}_LL115:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1C5[7]="cnst_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 7}};
# 1132
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1C6=yy1;union Cyc_Absyn_Cnst _tmp1C7;if((_tmp1C6.Int_tok).tag == 1){_LL12B: _tmp1C7=(_tmp1C6.Int_tok).val;_LL12C:
# 1136
 yyzzz=_tmp1C7;
goto _LL12A;}else{_LL12D: _LL12E:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL12A:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp1C8;(_tmp1C8.Int_tok).val=yy1;(_tmp1C8.Int_tok).tag=1;_tmp1C8;});}static char _tmp1CA[5]="char";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1CA,_tmp1CA,_tmp1CA + 5}};
# 1133
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1CB=yy1;char _tmp1CC;if((_tmp1CB.Char_tok).tag == 2){_LL130: _tmp1CC=(_tmp1CB.Char_tok).val;_LL131:
# 1137
 yyzzz=_tmp1CC;
goto _LL12F;}else{_LL132: _LL133:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL12F:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp1CD;(_tmp1CD.Char_tok).val=yy1;(_tmp1CD.Char_tok).tag=2;_tmp1CD;});}static char _tmp1CF[13]="string_t<`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1CF,_tmp1CF,_tmp1CF + 13}};
# 1134
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1D0=yy1;struct _dyneither_ptr _tmp1D1;if((_tmp1D0.String_tok).tag == 3){_LL135: _tmp1D1=(_tmp1D0.String_tok).val;_LL136:
# 1138
 yyzzz=_tmp1D1;
goto _LL134;}else{_LL137: _LL138:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL134:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp1D2;(_tmp1D2.String_tok).val=yy1;(_tmp1D2.String_tok).tag=3;_tmp1D2;});}static char _tmp1D4[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,_tmp1D4 + 56}};
# 1137
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1D5=yy1;struct _tuple21*_tmp1D6;if((_tmp1D5.YY1).tag == 9){_LL13A: _tmp1D6=(_tmp1D5.YY1).val;_LL13B:
# 1141
 yyzzz=_tmp1D6;
goto _LL139;}else{_LL13C: _LL13D:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL139:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp1D7;(_tmp1D7.YY1).val=yy1;(_tmp1D7.YY1).tag=9;_tmp1D7;});}static char _tmp1D9[19]="conref_t<bounds_t>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1D9,_tmp1D9,_tmp1D9 + 19}};
# 1138
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1DA=yy1;union Cyc_Absyn_Constraint*_tmp1DB;if((_tmp1DA.YY2).tag == 10){_LL13F: _tmp1DB=(_tmp1DA.YY2).val;_LL140:
# 1142
 yyzzz=_tmp1DB;
goto _LL13E;}else{_LL141: _LL142:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL13E:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp1DC;(_tmp1DC.YY2).val=yy1;(_tmp1DC.YY2).tag=10;_tmp1DC;});}static char _tmp1DE[28]="list_t<offsetof_field_t,`H>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 28}};
# 1139
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1DF=yy1;struct Cyc_List_List*_tmp1E0;if((_tmp1DF.YY3).tag == 11){_LL144: _tmp1E0=(_tmp1DF.YY3).val;_LL145:
# 1143
 yyzzz=_tmp1E0;
goto _LL143;}else{_LL146: _LL147:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL143:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1E1;(_tmp1E1.YY3).val=yy1;(_tmp1E1.YY3).tag=11;_tmp1E1;});}static char _tmp1E3[6]="exp_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 6}};
# 1140
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1E4=yy1;struct Cyc_Absyn_Exp*_tmp1E5;if((_tmp1E4.Exp_tok).tag == 7){_LL149: _tmp1E5=(_tmp1E4.Exp_tok).val;_LL14A:
# 1144
 yyzzz=_tmp1E5;
goto _LL148;}else{_LL14B: _LL14C:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL148:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp1E6;(_tmp1E6.Exp_tok).val=yy1;(_tmp1E6.Exp_tok).tag=7;_tmp1E6;});}static char _tmp1E8[17]="list_t<exp_t,`H>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,_tmp1E8 + 17}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E9=yy1;struct Cyc_List_List*_tmp1EA;if((_tmp1E9.YY4).tag == 12){_LL14E: _tmp1EA=(_tmp1E9.YY4).val;_LL14F:
# 1152
 yyzzz=_tmp1EA;
goto _LL14D;}else{_LL150: _LL151:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL14D:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1EB;(_tmp1EB.YY4).val=yy1;(_tmp1EB.YY4).tag=12;_tmp1EB;});}static char _tmp1ED[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 47}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EE=yy1;struct Cyc_List_List*_tmp1EF;if((_tmp1EE.YY5).tag == 13){_LL153: _tmp1EF=(_tmp1EE.YY5).val;_LL154:
# 1153
 yyzzz=_tmp1EF;
goto _LL152;}else{_LL155: _LL156:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL152:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1F0;(_tmp1F0.YY5).val=yy1;(_tmp1F0.YY5).tag=13;_tmp1F0;});}static char _tmp1F2[9]="primop_t";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 9}};
# 1150
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;enum Cyc_Absyn_Primop _tmp1F4;if((_tmp1F3.YY6).tag == 14){_LL158: _tmp1F4=(_tmp1F3.YY6).val;_LL159:
# 1154
 yyzzz=_tmp1F4;
goto _LL157;}else{_LL15A: _LL15B:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL157:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp1F5;(_tmp1F5.YY6).val=yy1;(_tmp1F5.YY6).tag=14;_tmp1F5;});}static char _tmp1F7[19]="opt_t<primop_t,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 19}};
# 1151
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1F8=yy1;struct Cyc_Core_Opt*_tmp1F9;if((_tmp1F8.YY7).tag == 15){_LL15D: _tmp1F9=(_tmp1F8.YY7).val;_LL15E:
# 1155
 yyzzz=_tmp1F9;
goto _LL15C;}else{_LL15F: _LL160:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL15C:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp1FA;(_tmp1FA.YY7).val=yy1;(_tmp1FA.YY7).tag=15;_tmp1FA;});}static char _tmp1FC[7]="qvar_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 7}};
# 1152
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1FD=yy1;struct _tuple0*_tmp1FE;if((_tmp1FD.QualId_tok).tag == 5){_LL162: _tmp1FE=(_tmp1FD.QualId_tok).val;_LL163:
# 1156
 yyzzz=_tmp1FE;
goto _LL161;}else{_LL164: _LL165:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL161:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp1FF;(_tmp1FF.QualId_tok).val=yy1;(_tmp1FF.QualId_tok).tag=5;_tmp1FF;});}static char _tmp201[7]="stmt_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 7}};
# 1155
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_Absyn_Stmt*_tmp203;if((_tmp202.Stmt_tok).tag == 8){_LL167: _tmp203=(_tmp202.Stmt_tok).val;_LL168:
# 1159
 yyzzz=_tmp203;
goto _LL166;}else{_LL169: _LL16A:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL166:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp204;(_tmp204.Stmt_tok).val=yy1;(_tmp204.Stmt_tok).tag=8;_tmp204;});}static char _tmp206[27]="list_t<switch_clause_t,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 27}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp207=yy1;struct Cyc_List_List*_tmp208;if((_tmp207.YY8).tag == 16){_LL16C: _tmp208=(_tmp207.YY8).val;_LL16D:
# 1163
 yyzzz=_tmp208;
goto _LL16B;}else{_LL16E: _LL16F:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL16B:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp209;(_tmp209.YY8).val=yy1;(_tmp209.YY8).tag=16;_tmp209;});}static char _tmp20B[6]="pat_t";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 6}};
# 1160
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct Cyc_Absyn_Pat*_tmp20D;if((_tmp20C.YY9).tag == 17){_LL171: _tmp20D=(_tmp20C.YY9).val;_LL172:
# 1164
 yyzzz=_tmp20D;
goto _LL170;}else{_LL173: _LL174:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL170:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp20E;(_tmp20E.YY9).val=yy1;(_tmp20E.YY9).tag=17;_tmp20E;});}static char _tmp210[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 28}};
# 1165
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct _tuple22*_tmp212;if((_tmp211.YY10).tag == 18){_LL176: _tmp212=(_tmp211.YY10).val;_LL177:
# 1169
 yyzzz=_tmp212;
goto _LL175;}else{_LL178: _LL179:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL175:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp213;(_tmp213.YY10).val=yy1;(_tmp213.YY10).tag=18;_tmp213;});}static char _tmp215[17]="list_t<pat_t,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp215,_tmp215,_tmp215 + 17}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp216=yy1;struct Cyc_List_List*_tmp217;if((_tmp216.YY11).tag == 19){_LL17B: _tmp217=(_tmp216.YY11).val;_LL17C:
# 1170
 yyzzz=_tmp217;
goto _LL17A;}else{_LL17D: _LL17E:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL17A:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp218;(_tmp218.YY11).val=yy1;(_tmp218.YY11).tag=19;_tmp218;});}static char _tmp21A[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 36}};
# 1167
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;struct _tuple23*_tmp21C;if((_tmp21B.YY12).tag == 20){_LL180: _tmp21C=(_tmp21B.YY12).val;_LL181:
# 1171
 yyzzz=_tmp21C;
goto _LL17F;}else{_LL182: _LL183:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL17F:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp21D;(_tmp21D.YY12).val=yy1;(_tmp21D.YY12).tag=20;_tmp21D;});}static char _tmp21F[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 47}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_List_List*_tmp221;if((_tmp220.YY13).tag == 21){_LL185: _tmp221=(_tmp220.YY13).val;_LL186:
# 1172
 yyzzz=_tmp221;
goto _LL184;}else{_LL187: _LL188:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL184:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp222;(_tmp222.YY13).val=yy1;(_tmp222.YY13).tag=21;_tmp222;});}static char _tmp224[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 58}};
# 1169
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp225=yy1;struct _tuple22*_tmp226;if((_tmp225.YY14).tag == 22){_LL18A: _tmp226=(_tmp225.YY14).val;_LL18B:
# 1173
 yyzzz=_tmp226;
goto _LL189;}else{_LL18C: _LL18D:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL189:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp227;(_tmp227.YY14).val=yy1;(_tmp227.YY14).tag=22;_tmp227;});}static char _tmp229[9]="fndecl_t";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 9}};
# 1170
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_Absyn_Fndecl*_tmp22B;if((_tmp22A.YY15).tag == 23){_LL18F: _tmp22B=(_tmp22A.YY15).val;_LL190:
# 1174
 yyzzz=_tmp22B;
goto _LL18E;}else{_LL191: _LL192:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL18E:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp22C;(_tmp22C.YY15).val=yy1;(_tmp22C.YY15).tag=23;_tmp22C;});}static char _tmp22E[18]="list_t<decl_t,`H>";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 18}};
# 1171
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;struct Cyc_List_List*_tmp230;if((_tmp22F.YY16).tag == 24){_LL194: _tmp230=(_tmp22F.YY16).val;_LL195:
# 1175
 yyzzz=_tmp230;
goto _LL193;}else{_LL196: _LL197:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL193:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp231;(_tmp231.YY16).val=yy1;(_tmp231.YY16).tag=24;_tmp231;});}static char _tmp233[12]="decl_spec_t";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 12}};
# 1173
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Parse_Declaration_spec _tmp235;if((_tmp234.YY17).tag == 25){_LL199: _tmp235=(_tmp234.YY17).val;_LL19A:
# 1177
 yyzzz=_tmp235;
goto _LL198;}else{_LL19B: _LL19C:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL198:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp236;(_tmp236.YY17).val=yy1;(_tmp236.YY17).tag=25;_tmp236;});}static char _tmp238[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 31}};
# 1174
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple11 _tmp23A;if((_tmp239.YY18).tag == 26){_LL19E: _tmp23A=(_tmp239.YY18).val;_LL19F:
# 1178
 yyzzz=_tmp23A;
goto _LL19D;}else{_LL1A0: _LL1A1:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL19D:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp23B;(_tmp23B.YY18).val=yy1;(_tmp23B.YY18).tag=26;_tmp23B;});}static char _tmp23D[23]="declarator_list_t<`yy>";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 23}};
# 1175
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct _tuple12*_tmp23F;if((_tmp23E.YY19).tag == 27){_LL1A3: _tmp23F=(_tmp23E.YY19).val;_LL1A4:
# 1179
 yyzzz=_tmp23F;
goto _LL1A2;}else{_LL1A5: _LL1A6:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL1A2:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp240;(_tmp240.YY19).val=yy1;(_tmp240.YY19).tag=27;_tmp240;});}static char _tmp242[19]="storage_class_t@`H";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 19}};
# 1176
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp243=yy1;enum Cyc_Parse_Storage_class*_tmp244;if((_tmp243.YY20).tag == 28){_LL1A8: _tmp244=(_tmp243.YY20).val;_LL1A9:
# 1180
 yyzzz=_tmp244;
goto _LL1A7;}else{_LL1AA: _LL1AB:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL1A7:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp245;(_tmp245.YY20).val=yy1;(_tmp245.YY20).tag=28;_tmp245;});}static char _tmp247[17]="type_specifier_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 17}};
# 1177
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_Parse_Type_specifier _tmp249;if((_tmp248.YY21).tag == 29){_LL1AD: _tmp249=(_tmp248.YY21).val;_LL1AE:
# 1181
 yyzzz=_tmp249;
goto _LL1AC;}else{_LL1AF: _LL1B0:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL1AC:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp24A;(_tmp24A.YY21).val=yy1;(_tmp24A.YY21).tag=29;_tmp24A;});}static char _tmp24C[12]="aggr_kind_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 12}};
# 1179
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;enum Cyc_Absyn_AggrKind _tmp24E;if((_tmp24D.YY22).tag == 30){_LL1B2: _tmp24E=(_tmp24D.YY22).val;_LL1B3:
# 1183
 yyzzz=_tmp24E;
goto _LL1B1;}else{_LL1B4: _LL1B5:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL1B1:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp24F;(_tmp24F.YY22).val=yy1;(_tmp24F.YY22).tag=30;_tmp24F;});}static char _tmp251[8]="tqual_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 8}};
# 1180
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_Absyn_Tqual _tmp253;if((_tmp252.YY23).tag == 31){_LL1B7: _tmp253=(_tmp252.YY23).val;_LL1B8:
# 1184
 yyzzz=_tmp253;
goto _LL1B6;}else{_LL1B9: _LL1BA:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL1B6:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp254;(_tmp254.YY23).val=yy1;(_tmp254.YY23).tag=31;_tmp254;});}static char _tmp256[23]="list_t<aggrfield_t,`H>";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 23}};
# 1181
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct Cyc_List_List*_tmp258;if((_tmp257.YY24).tag == 32){_LL1BC: _tmp258=(_tmp257.YY24).val;_LL1BD:
# 1185
 yyzzz=_tmp258;
goto _LL1BB;}else{_LL1BE: _LL1BF:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL1BB:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp259;(_tmp259.YY24).val=yy1;(_tmp259.YY24).tag=32;_tmp259;});}static char _tmp25B[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 34}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_List_List*_tmp25D;if((_tmp25C.YY25).tag == 33){_LL1C1: _tmp25D=(_tmp25C.YY25).val;_LL1C2:
# 1186
 yyzzz=_tmp25D;
goto _LL1C0;}else{_LL1C3: _LL1C4:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL1C0:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp25E;(_tmp25E.YY25).val=yy1;(_tmp25E.YY25).tag=33;_tmp25E;});}static char _tmp260[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp260,_tmp260,_tmp260 + 33}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp261=yy1;struct Cyc_List_List*_tmp262;if((_tmp261.YY26).tag == 34){_LL1C6: _tmp262=(_tmp261.YY26).val;_LL1C7:
# 1187
 yyzzz=_tmp262;
goto _LL1C5;}else{_LL1C8: _LL1C9:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL1C5:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp263;(_tmp263.YY26).val=yy1;(_tmp263.YY26).tag=34;_tmp263;});}static char _tmp265[18]="declarator_t<`yy>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 18}};
# 1184
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp266=yy1;struct Cyc_Parse_Declarator _tmp267;if((_tmp266.YY27).tag == 35){_LL1CB: _tmp267=(_tmp266.YY27).val;_LL1CC:
# 1188
 yyzzz=_tmp267;
goto _LL1CA;}else{_LL1CD: _LL1CE:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL1CA:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp268;(_tmp268.YY27).val=yy1;(_tmp268.YY27).tag=35;_tmp268;});}static char _tmp26A[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 45}};
# 1185
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp26B=yy1;struct _tuple24*_tmp26C;if((_tmp26B.YY28).tag == 36){_LL1D0: _tmp26C=(_tmp26B.YY28).val;_LL1D1:
# 1189
 yyzzz=_tmp26C;
goto _LL1CF;}else{_LL1D2: _LL1D3:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL1CF:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp26D;(_tmp26D.YY28).val=yy1;(_tmp26D.YY28).tag=36;_tmp26D;});}static char _tmp26F[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 57}};
# 1186
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp270=yy1;struct Cyc_List_List*_tmp271;if((_tmp270.YY29).tag == 37){_LL1D5: _tmp271=(_tmp270.YY29).val;_LL1D6:
# 1190
 yyzzz=_tmp271;
goto _LL1D4;}else{_LL1D7: _LL1D8:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL1D4:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp272;(_tmp272.YY29).val=yy1;(_tmp272.YY29).tag=37;_tmp272;});}static char _tmp274[26]="abstractdeclarator_t<`yy>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 26}};
# 1187
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp275=yy1;struct Cyc_Parse_Abstractdeclarator _tmp276;if((_tmp275.YY30).tag == 38){_LL1DA: _tmp276=(_tmp275.YY30).val;_LL1DB:
# 1191
 yyzzz=_tmp276;
goto _LL1D9;}else{_LL1DC: _LL1DD:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL1D9:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp277;(_tmp277.YY30).val=yy1;(_tmp277.YY30).tag=38;_tmp277;});}static char _tmp279[5]="bool";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 5}};
# 1188
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp27A=yy1;int _tmp27B;if((_tmp27A.YY31).tag == 39){_LL1DF: _tmp27B=(_tmp27A.YY31).val;_LL1E0:
# 1192
 yyzzz=_tmp27B;
goto _LL1DE;}else{_LL1E1: _LL1E2:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL1DE:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp27C;(_tmp27C.YY31).val=yy1;(_tmp27C.YY31).tag=39;_tmp27C;});}static char _tmp27E[8]="scope_t";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 8}};
# 1189
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp27F=yy1;enum Cyc_Absyn_Scope _tmp280;if((_tmp27F.YY32).tag == 40){_LL1E4: _tmp280=(_tmp27F.YY32).val;_LL1E5:
# 1193
 yyzzz=_tmp280;
goto _LL1E3;}else{_LL1E6: _LL1E7:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL1E3:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp281;(_tmp281.YY32).val=yy1;(_tmp281.YY32).tag=40;_tmp281;});}static char _tmp283[16]="datatypefield_t";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp283,_tmp283,_tmp283 + 16}};
# 1190
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_Absyn_Datatypefield*_tmp285;if((_tmp284.YY33).tag == 41){_LL1E9: _tmp285=(_tmp284.YY33).val;_LL1EA:
# 1194
 yyzzz=_tmp285;
goto _LL1E8;}else{_LL1EB: _LL1EC:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL1E8:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp286;(_tmp286.YY33).val=yy1;(_tmp286.YY33).tag=41;_tmp286;});}static char _tmp288[27]="list_t<datatypefield_t,`H>";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 27}};
# 1191
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp289=yy1;struct Cyc_List_List*_tmp28A;if((_tmp289.YY34).tag == 42){_LL1EE: _tmp28A=(_tmp289.YY34).val;_LL1EF:
# 1195
 yyzzz=_tmp28A;
goto _LL1ED;}else{_LL1F0: _LL1F1:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL1ED:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp28B;(_tmp28B.YY34).val=yy1;(_tmp28B.YY34).tag=42;_tmp28B;});}static char _tmp28D[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1190 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 41}};
# 1192
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp28E=yy1;struct _tuple25 _tmp28F;if((_tmp28E.YY35).tag == 43){_LL1F3: _tmp28F=(_tmp28E.YY35).val;_LL1F4:
# 1196
 yyzzz=_tmp28F;
goto _LL1F2;}else{_LL1F5: _LL1F6:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL1F2:;}
# 1200
return yyzzz;}
# 1202
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp290;(_tmp290.YY35).val=yy1;(_tmp290.YY35).tag=43;_tmp290;});}static char _tmp292[17]="list_t<var_t,`H>";
# 1191 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 17}};
# 1193
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp293=yy1;struct Cyc_List_List*_tmp294;if((_tmp293.YY36).tag == 44){_LL1F8: _tmp294=(_tmp293.YY36).val;_LL1F9:
# 1197
 yyzzz=_tmp294;
goto _LL1F7;}else{_LL1FA: _LL1FB:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL1F7:;}
# 1201
return yyzzz;}
# 1203
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp295;(_tmp295.YY36).val=yy1;(_tmp295.YY36).tag=44;_tmp295;});}static char _tmp297[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1192 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp297,_tmp297,_tmp297 + 31}};
# 1194
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp298=yy1;struct _tuple8*_tmp299;if((_tmp298.YY37).tag == 45){_LL1FD: _tmp299=(_tmp298.YY37).val;_LL1FE:
# 1198
 yyzzz=_tmp299;
goto _LL1FC;}else{_LL1FF: _LL200:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL1FC:;}
# 1202
return yyzzz;}
# 1204
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp29A;(_tmp29A.YY37).val=yy1;(_tmp29A.YY37).tag=45;_tmp29A;});}static char _tmp29C[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1193 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 42}};
# 1195
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29D=yy1;struct Cyc_List_List*_tmp29E;if((_tmp29D.YY38).tag == 46){_LL202: _tmp29E=(_tmp29D.YY38).val;_LL203:
# 1199
 yyzzz=_tmp29E;
goto _LL201;}else{_LL204: _LL205:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL201:;}
# 1203
return yyzzz;}
# 1205
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp29F;(_tmp29F.YY38).val=yy1;(_tmp29F.YY38).tag=46;_tmp29F;});}static char _tmp2A1[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1194 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 115}};
# 1196
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2A2=yy1;struct _tuple26*_tmp2A3;if((_tmp2A2.YY39).tag == 47){_LL207: _tmp2A3=(_tmp2A2.YY39).val;_LL208:
# 1200
 yyzzz=_tmp2A3;
goto _LL206;}else{_LL209: _LL20A:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL206:;}
# 1204
return yyzzz;}
# 1206
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp2A4;(_tmp2A4.YY39).val=yy1;(_tmp2A4.YY39).tag=47;_tmp2A4;});}static char _tmp2A6[18]="list_t<type_t,`H>";
# 1195 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 18}};
# 1197
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A7=yy1;struct Cyc_List_List*_tmp2A8;if((_tmp2A7.YY40).tag == 48){_LL20C: _tmp2A8=(_tmp2A7.YY40).val;_LL20D:
# 1201
 yyzzz=_tmp2A8;
goto _LL20B;}else{_LL20E: _LL20F:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL20B:;}
# 1205
return yyzzz;}
# 1207
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2A9;(_tmp2A9.YY40).val=yy1;(_tmp2A9.YY40).tag=48;_tmp2A9;});}static char _tmp2AB[24]="list_t<designator_t,`H>";
# 1197 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 24}};
# 1199
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_List_List*_tmp2AD;if((_tmp2AC.YY41).tag == 49){_LL211: _tmp2AD=(_tmp2AC.YY41).val;_LL212:
# 1203
 yyzzz=_tmp2AD;
goto _LL210;}else{_LL213: _LL214:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL210:;}
# 1207
return yyzzz;}
# 1209
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2AE;(_tmp2AE.YY41).val=yy1;(_tmp2AE.YY41).tag=49;_tmp2AE;});}static char _tmp2B0[13]="designator_t";
# 1198 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 13}};
# 1200
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2B1=yy1;void*_tmp2B2;if((_tmp2B1.YY42).tag == 50){_LL216: _tmp2B2=(_tmp2B1.YY42).val;_LL217:
# 1204
 yyzzz=_tmp2B2;
goto _LL215;}else{_LL218: _LL219:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL215:;}
# 1208
return yyzzz;}
# 1210
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp2B3;(_tmp2B3.YY42).val=yy1;(_tmp2B3.YY42).tag=50;_tmp2B3;});}static char _tmp2B5[7]="kind_t";
# 1199 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2B5,_tmp2B5,_tmp2B5 + 7}};
# 1201
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2B6=yy1;struct Cyc_Absyn_Kind*_tmp2B7;if((_tmp2B6.YY43).tag == 51){_LL21B: _tmp2B7=(_tmp2B6.YY43).val;_LL21C:
# 1205
 yyzzz=_tmp2B7;
goto _LL21A;}else{_LL21D: _LL21E:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL21A:;}
# 1209
return yyzzz;}
# 1211
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp2B8;(_tmp2B8.YY43).val=yy1;(_tmp2B8.YY43).tag=51;_tmp2B8;});}static char _tmp2BA[7]="type_t";
# 1200 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 7}};
# 1202
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2BB=yy1;void*_tmp2BC;if((_tmp2BB.YY44).tag == 52){_LL220: _tmp2BC=(_tmp2BB.YY44).val;_LL221:
# 1206
 yyzzz=_tmp2BC;
goto _LL21F;}else{_LL222: _LL223:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL21F:;}
# 1210
return yyzzz;}
# 1212
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp2BD;(_tmp2BD.YY44).val=yy1;(_tmp2BD.YY44).tag=52;_tmp2BD;});}static char _tmp2BF[23]="list_t<attribute_t,`H>";
# 1201 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 23}};
# 1203
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C0=yy1;struct Cyc_List_List*_tmp2C1;if((_tmp2C0.YY45).tag == 53){_LL225: _tmp2C1=(_tmp2C0.YY45).val;_LL226:
# 1207
 yyzzz=_tmp2C1;
goto _LL224;}else{_LL227: _LL228:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL224:;}
# 1211
return yyzzz;}
# 1213
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2C2;(_tmp2C2.YY45).val=yy1;(_tmp2C2.YY45).tag=53;_tmp2C2;});}static char _tmp2C4[12]="attribute_t";
# 1202 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 12}};
# 1204
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C5=yy1;void*_tmp2C6;if((_tmp2C5.YY46).tag == 54){_LL22A: _tmp2C6=(_tmp2C5.YY46).val;_LL22B:
# 1208
 yyzzz=_tmp2C6;
goto _LL229;}else{_LL22C: _LL22D:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL229:;}
# 1212
return yyzzz;}
# 1214
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp2C7;(_tmp2C7.YY46).val=yy1;(_tmp2C7.YY46).tag=54;_tmp2C7;});}static char _tmp2C9[12]="enumfield_t";
# 1203 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 12}};
# 1205
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2CA=yy1;struct Cyc_Absyn_Enumfield*_tmp2CB;if((_tmp2CA.YY47).tag == 55){_LL22F: _tmp2CB=(_tmp2CA.YY47).val;_LL230:
# 1209
 yyzzz=_tmp2CB;
goto _LL22E;}else{_LL231: _LL232:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL22E:;}
# 1213
return yyzzz;}
# 1215
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp2CC;(_tmp2CC.YY47).val=yy1;(_tmp2CC.YY47).tag=55;_tmp2CC;});}static char _tmp2CE[23]="list_t<enumfield_t,`H>";
# 1204 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 23}};
# 1206
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_List_List*_tmp2D0;if((_tmp2CF.YY48).tag == 56){_LL234: _tmp2D0=(_tmp2CF.YY48).val;_LL235:
# 1210
 yyzzz=_tmp2D0;
goto _LL233;}else{_LL236: _LL237:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL233:;}
# 1214
return yyzzz;}
# 1216
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2D1;(_tmp2D1.YY48).val=yy1;(_tmp2D1.YY48).tag=56;_tmp2D1;});}static char _tmp2D3[11]="type_opt_t";
# 1205 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 11}};
# 1207
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D4=yy1;void*_tmp2D5;if((_tmp2D4.YY49).tag == 57){_LL239: _tmp2D5=(_tmp2D4.YY49).val;_LL23A:
# 1211
 yyzzz=_tmp2D5;
goto _LL238;}else{_LL23B: _LL23C:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL238:;}
# 1215
return yyzzz;}
# 1217
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp2D6;(_tmp2D6.YY49).val=yy1;(_tmp2D6.YY49).tag=57;_tmp2D6;});}static char _tmp2D8[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1206 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2D8,_tmp2D8,_tmp2D8 + 31}};
# 1208
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D9=yy1;struct Cyc_List_List*_tmp2DA;if((_tmp2D9.YY50).tag == 58){_LL23E: _tmp2DA=(_tmp2D9.YY50).val;_LL23F:
# 1212
 yyzzz=_tmp2DA;
goto _LL23D;}else{_LL240: _LL241:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL23D:;}
# 1216
return yyzzz;}
# 1218
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2DB;(_tmp2DB.YY50).val=yy1;(_tmp2DB.YY50).tag=58;_tmp2DB;});}static char _tmp2DD[15]="conref_t<bool>";
# 1207 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 15}};
# 1209
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2DE=yy1;union Cyc_Absyn_Constraint*_tmp2DF;if((_tmp2DE.YY51).tag == 59){_LL243: _tmp2DF=(_tmp2DE.YY51).val;_LL244:
# 1213
 yyzzz=_tmp2DF;
goto _LL242;}else{_LL245: _LL246:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL242:;}
# 1217
return yyzzz;}
# 1219
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp2E0;(_tmp2E0.YY51).val=yy1;(_tmp2E0.YY51).tag=59;_tmp2E0;});}static char _tmp2E2[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1208 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 45}};
# 1210
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E3=yy1;struct Cyc_List_List*_tmp2E4;if((_tmp2E3.YY52).tag == 60){_LL248: _tmp2E4=(_tmp2E3.YY52).val;_LL249:
# 1214
 yyzzz=_tmp2E4;
goto _LL247;}else{_LL24A: _LL24B:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL247:;}
# 1218
return yyzzz;}
# 1220
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2E5;(_tmp2E5.YY52).val=yy1;(_tmp2E5.YY52).tag=60;_tmp2E5;});}static char _tmp2E7[20]="pointer_qual_t<`yy>";
# 1209 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 20}};
# 1211
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E8=yy1;void*_tmp2E9;if((_tmp2E8.YY53).tag == 61){_LL24D: _tmp2E9=(_tmp2E8.YY53).val;_LL24E:
# 1215
 yyzzz=_tmp2E9;
goto _LL24C;}else{_LL24F: _LL250:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL24C:;}
# 1219
return yyzzz;}
# 1221
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp2EA;(_tmp2EA.YY53).val=yy1;(_tmp2EA.YY53).tag=61;_tmp2EA;});}static char _tmp2EC[21]="pointer_quals_t<`yy>";
# 1210 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 21}};
# 1212
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2ED=yy1;struct Cyc_List_List*_tmp2EE;if((_tmp2ED.YY54).tag == 62){_LL252: _tmp2EE=(_tmp2ED.YY54).val;_LL253:
# 1216
 yyzzz=_tmp2EE;
goto _LL251;}else{_LL254: _LL255:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL251:;}
# 1220
return yyzzz;}
# 1222
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2EF;(_tmp2EF.YY54).val=yy1;(_tmp2EF.YY54).tag=62;_tmp2EF;});}static char _tmp2F1[21]="$(bool,string_t<`H>)";
# 1211 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 21}};
# 1213
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp2F2=yy1;struct _tuple20 _tmp2F3;if((_tmp2F2.Asm_tok).tag == 6){_LL257: _tmp2F3=(_tmp2F2.Asm_tok).val;_LL258:
# 1217
 yyzzz=_tmp2F3;
goto _LL256;}else{_LL259: _LL25A:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL256:;}
# 1221
return yyzzz;}
# 1223
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp2F4;(_tmp2F4.Asm_tok).val=yy1;(_tmp2F4.Asm_tok).tag=6;_tmp2F4;});}static char _tmp2F6[10]="exp_opt_t";
# 1212 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,_tmp2F6 + 10}};
# 1214
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp2F7=yy1;struct Cyc_Absyn_Exp*_tmp2F8;if((_tmp2F7.YY55).tag == 63){_LL25C: _tmp2F8=(_tmp2F7.YY55).val;_LL25D:
# 1218
 yyzzz=_tmp2F8;
goto _LL25B;}else{_LL25E: _LL25F:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL25B:;}
# 1222
return yyzzz;}
# 1224
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp2F9;(_tmp2F9.YY55).val=yy1;(_tmp2F9.YY55).tag=63;_tmp2F9;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1236
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1240
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp2FA;_tmp2FA.timestamp=0;_tmp2FA.first_line=0;_tmp2FA.first_column=0;_tmp2FA.last_line=0;_tmp2FA.last_column=0;_tmp2FA;});}
# 1243
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1254 "parse.y"
static short Cyc_yytranslate[376]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,147,2,2,131,145,142,2,128,129,136,139,124,143,133,144,2,2,2,2,2,2,2,2,2,2,132,121,126,125,127,138,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,134,2,135,141,130,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,122,140,123,146,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};static char _tmp2FB[2]="$";static char _tmp2FC[6]="error";static char _tmp2FD[12]="$undefined.";static char _tmp2FE[5]="AUTO";static char _tmp2FF[9]="REGISTER";static char _tmp300[7]="STATIC";static char _tmp301[7]="EXTERN";static char _tmp302[8]="TYPEDEF";static char _tmp303[5]="VOID";static char _tmp304[5]="CHAR";static char _tmp305[6]="SHORT";static char _tmp306[4]="INT";static char _tmp307[5]="LONG";static char _tmp308[6]="FLOAT";static char _tmp309[7]="DOUBLE";static char _tmp30A[7]="SIGNED";static char _tmp30B[9]="UNSIGNED";static char _tmp30C[6]="CONST";static char _tmp30D[9]="VOLATILE";static char _tmp30E[9]="RESTRICT";static char _tmp30F[7]="STRUCT";static char _tmp310[6]="UNION";static char _tmp311[5]="CASE";static char _tmp312[8]="DEFAULT";static char _tmp313[7]="INLINE";static char _tmp314[7]="SIZEOF";static char _tmp315[9]="OFFSETOF";static char _tmp316[3]="IF";static char _tmp317[5]="ELSE";static char _tmp318[7]="SWITCH";static char _tmp319[6]="WHILE";static char _tmp31A[3]="DO";static char _tmp31B[4]="FOR";static char _tmp31C[5]="GOTO";static char _tmp31D[9]="CONTINUE";static char _tmp31E[6]="BREAK";static char _tmp31F[7]="RETURN";static char _tmp320[5]="ENUM";static char _tmp321[7]="TYPEOF";static char _tmp322[16]="BUILTIN_VA_LIST";static char _tmp323[8]="NULL_kw";static char _tmp324[4]="LET";static char _tmp325[6]="THROW";static char _tmp326[4]="TRY";static char _tmp327[6]="CATCH";static char _tmp328[7]="EXPORT";static char _tmp329[4]="NEW";static char _tmp32A[9]="ABSTRACT";static char _tmp32B[9]="FALLTHRU";static char _tmp32C[6]="USING";static char _tmp32D[10]="NAMESPACE";static char _tmp32E[9]="DATATYPE";static char _tmp32F[7]="MALLOC";static char _tmp330[8]="RMALLOC";static char _tmp331[15]="RMALLOC_INLINE";static char _tmp332[7]="CALLOC";static char _tmp333[8]="RCALLOC";static char _tmp334[5]="SWAP";static char _tmp335[9]="REGION_T";static char _tmp336[6]="TAG_T";static char _tmp337[7]="REGION";static char _tmp338[5]="RNEW";static char _tmp339[8]="REGIONS";static char _tmp33A[13]="RESET_REGION";static char _tmp33B[7]="PORTON";static char _tmp33C[8]="PORTOFF";static char _tmp33D[12]="DYNREGION_T";static char _tmp33E[8]="NUMELTS";static char _tmp33F[8]="VALUEOF";static char _tmp340[10]="VALUEOF_T";static char _tmp341[9]="TAGCHECK";static char _tmp342[13]="NUMELTS_QUAL";static char _tmp343[10]="THIN_QUAL";static char _tmp344[9]="FAT_QUAL";static char _tmp345[13]="NOTNULL_QUAL";static char _tmp346[14]="NULLABLE_QUAL";static char _tmp347[14]="REQUIRES_QUAL";static char _tmp348[13]="ENSURES_QUAL";static char _tmp349[12]="REGION_QUAL";static char _tmp34A[16]="NOZEROTERM_QUAL";static char _tmp34B[14]="ZEROTERM_QUAL";static char _tmp34C[12]="TAGGED_QUAL";static char _tmp34D[16]="EXTENSIBLE_QUAL";static char _tmp34E[15]="RESETABLE_QUAL";static char _tmp34F[7]="PTR_OP";static char _tmp350[7]="INC_OP";static char _tmp351[7]="DEC_OP";static char _tmp352[8]="LEFT_OP";static char _tmp353[9]="RIGHT_OP";static char _tmp354[6]="LE_OP";static char _tmp355[6]="GE_OP";static char _tmp356[6]="EQ_OP";static char _tmp357[6]="NE_OP";static char _tmp358[7]="AND_OP";static char _tmp359[6]="OR_OP";static char _tmp35A[11]="MUL_ASSIGN";static char _tmp35B[11]="DIV_ASSIGN";static char _tmp35C[11]="MOD_ASSIGN";static char _tmp35D[11]="ADD_ASSIGN";static char _tmp35E[11]="SUB_ASSIGN";static char _tmp35F[12]="LEFT_ASSIGN";static char _tmp360[13]="RIGHT_ASSIGN";static char _tmp361[11]="AND_ASSIGN";static char _tmp362[11]="XOR_ASSIGN";static char _tmp363[10]="OR_ASSIGN";static char _tmp364[9]="ELLIPSIS";static char _tmp365[11]="LEFT_RIGHT";static char _tmp366[12]="COLON_COLON";static char _tmp367[11]="IDENTIFIER";static char _tmp368[17]="INTEGER_CONSTANT";static char _tmp369[7]="STRING";static char _tmp36A[8]="WSTRING";static char _tmp36B[19]="CHARACTER_CONSTANT";static char _tmp36C[20]="WCHARACTER_CONSTANT";static char _tmp36D[18]="FLOATING_CONSTANT";static char _tmp36E[9]="TYPE_VAR";static char _tmp36F[13]="TYPEDEF_NAME";static char _tmp370[16]="QUAL_IDENTIFIER";static char _tmp371[18]="QUAL_TYPEDEF_NAME";static char _tmp372[10]="ATTRIBUTE";static char _tmp373[4]="ASM";static char _tmp374[4]="';'";static char _tmp375[4]="'{'";static char _tmp376[4]="'}'";static char _tmp377[4]="','";static char _tmp378[4]="'='";static char _tmp379[4]="'<'";static char _tmp37A[4]="'>'";static char _tmp37B[4]="'('";static char _tmp37C[4]="')'";static char _tmp37D[4]="'_'";static char _tmp37E[4]="'$'";static char _tmp37F[4]="':'";static char _tmp380[4]="'.'";static char _tmp381[4]="'['";static char _tmp382[4]="']'";static char _tmp383[4]="'*'";static char _tmp384[4]="'@'";static char _tmp385[4]="'?'";static char _tmp386[4]="'+'";static char _tmp387[4]="'|'";static char _tmp388[4]="'^'";static char _tmp389[4]="'&'";static char _tmp38A[4]="'-'";static char _tmp38B[4]="'/'";static char _tmp38C[4]="'%'";static char _tmp38D[4]="'~'";static char _tmp38E[4]="'!'";static char _tmp38F[5]="prog";static char _tmp390[17]="translation_unit";static char _tmp391[16]="extern_c_action";static char _tmp392[13]="end_extern_c";static char _tmp393[12]="export_list";static char _tmp394[19]="export_list_values";static char _tmp395[21]="external_declaration";static char _tmp396[15]="optional_comma";static char _tmp397[20]="function_definition";static char _tmp398[21]="function_definition2";static char _tmp399[13]="using_action";static char _tmp39A[15]="unusing_action";static char _tmp39B[17]="namespace_action";static char _tmp39C[19]="unnamespace_action";static char _tmp39D[12]="declaration";static char _tmp39E[19]="resetable_qual_opt";static char _tmp39F[17]="declaration_list";static char _tmp3A0[23]="declaration_specifiers";static char _tmp3A1[24]="storage_class_specifier";static char _tmp3A2[15]="attributes_opt";static char _tmp3A3[11]="attributes";static char _tmp3A4[15]="attribute_list";static char _tmp3A5[10]="attribute";static char _tmp3A6[15]="type_specifier";static char _tmp3A7[25]="type_specifier_notypedef";static char _tmp3A8[5]="kind";static char _tmp3A9[15]="type_qualifier";static char _tmp3AA[15]="enum_specifier";static char _tmp3AB[11]="enum_field";static char _tmp3AC[22]="enum_declaration_list";static char _tmp3AD[26]="struct_or_union_specifier";static char _tmp3AE[16]="type_params_opt";static char _tmp3AF[16]="struct_or_union";static char _tmp3B0[24]="struct_declaration_list";static char _tmp3B1[25]="struct_declaration_list0";static char _tmp3B2[21]="init_declarator_list";static char _tmp3B3[22]="init_declarator_list0";static char _tmp3B4[16]="init_declarator";static char _tmp3B5[19]="struct_declaration";static char _tmp3B6[25]="specifier_qualifier_list";static char _tmp3B7[35]="notypedef_specifier_qualifier_list";static char _tmp3B8[23]="struct_declarator_list";static char _tmp3B9[24]="struct_declarator_list0";static char _tmp3BA[18]="struct_declarator";static char _tmp3BB[20]="requires_clause_opt";static char _tmp3BC[19]="ensures_clause_opt";static char _tmp3BD[19]="datatype_specifier";static char _tmp3BE[14]="qual_datatype";static char _tmp3BF[19]="datatypefield_list";static char _tmp3C0[20]="datatypefield_scope";static char _tmp3C1[14]="datatypefield";static char _tmp3C2[11]="declarator";static char _tmp3C3[23]="declarator_withtypedef";static char _tmp3C4[18]="direct_declarator";static char _tmp3C5[30]="direct_declarator_withtypedef";static char _tmp3C6[8]="pointer";static char _tmp3C7[12]="one_pointer";static char _tmp3C8[14]="pointer_quals";static char _tmp3C9[13]="pointer_qual";static char _tmp3CA[23]="pointer_null_and_bound";static char _tmp3CB[14]="pointer_bound";static char _tmp3CC[18]="zeroterm_qual_opt";static char _tmp3CD[8]="rgn_opt";static char _tmp3CE[11]="tqual_list";static char _tmp3CF[20]="parameter_type_list";static char _tmp3D0[9]="type_var";static char _tmp3D1[16]="optional_effect";static char _tmp3D2[19]="optional_rgn_order";static char _tmp3D3[10]="rgn_order";static char _tmp3D4[16]="optional_inject";static char _tmp3D5[11]="effect_set";static char _tmp3D6[14]="atomic_effect";static char _tmp3D7[11]="region_set";static char _tmp3D8[15]="parameter_list";static char _tmp3D9[22]="parameter_declaration";static char _tmp3DA[16]="identifier_list";static char _tmp3DB[17]="identifier_list0";static char _tmp3DC[12]="initializer";static char _tmp3DD[18]="array_initializer";static char _tmp3DE[17]="initializer_list";static char _tmp3DF[12]="designation";static char _tmp3E0[16]="designator_list";static char _tmp3E1[11]="designator";static char _tmp3E2[10]="type_name";static char _tmp3E3[14]="any_type_name";static char _tmp3E4[15]="type_name_list";static char _tmp3E5[20]="abstract_declarator";static char _tmp3E6[27]="direct_abstract_declarator";static char _tmp3E7[10]="statement";static char _tmp3E8[18]="labeled_statement";static char _tmp3E9[21]="expression_statement";static char _tmp3EA[19]="compound_statement";static char _tmp3EB[16]="block_item_list";static char _tmp3EC[20]="selection_statement";static char _tmp3ED[15]="switch_clauses";static char _tmp3EE[20]="iteration_statement";static char _tmp3EF[15]="jump_statement";static char _tmp3F0[12]="exp_pattern";static char _tmp3F1[20]="conditional_pattern";static char _tmp3F2[19]="logical_or_pattern";static char _tmp3F3[20]="logical_and_pattern";static char _tmp3F4[21]="inclusive_or_pattern";static char _tmp3F5[21]="exclusive_or_pattern";static char _tmp3F6[12]="and_pattern";static char _tmp3F7[17]="equality_pattern";static char _tmp3F8[19]="relational_pattern";static char _tmp3F9[14]="shift_pattern";static char _tmp3FA[17]="additive_pattern";static char _tmp3FB[23]="multiplicative_pattern";static char _tmp3FC[13]="cast_pattern";static char _tmp3FD[14]="unary_pattern";static char _tmp3FE[16]="postfix_pattern";static char _tmp3FF[16]="primary_pattern";static char _tmp400[8]="pattern";static char _tmp401[19]="tuple_pattern_list";static char _tmp402[20]="tuple_pattern_list0";static char _tmp403[14]="field_pattern";static char _tmp404[19]="field_pattern_list";static char _tmp405[20]="field_pattern_list0";static char _tmp406[11]="expression";static char _tmp407[22]="assignment_expression";static char _tmp408[20]="assignment_operator";static char _tmp409[23]="conditional_expression";static char _tmp40A[20]="constant_expression";static char _tmp40B[22]="logical_or_expression";static char _tmp40C[23]="logical_and_expression";static char _tmp40D[24]="inclusive_or_expression";static char _tmp40E[24]="exclusive_or_expression";static char _tmp40F[15]="and_expression";static char _tmp410[20]="equality_expression";static char _tmp411[22]="relational_expression";static char _tmp412[17]="shift_expression";static char _tmp413[20]="additive_expression";static char _tmp414[26]="multiplicative_expression";static char _tmp415[16]="cast_expression";static char _tmp416[17]="unary_expression";static char _tmp417[15]="unary_operator";static char _tmp418[19]="postfix_expression";static char _tmp419[17]="field_expression";static char _tmp41A[19]="primary_expression";static char _tmp41B[25]="argument_expression_list";static char _tmp41C[26]="argument_expression_list0";static char _tmp41D[9]="constant";static char _tmp41E[20]="qual_opt_identifier";static char _tmp41F[17]="qual_opt_typedef";static char _tmp420[18]="struct_union_name";static char _tmp421[11]="field_name";static char _tmp422[12]="right_angle";
# 1608 "parse.y"
static struct _dyneither_ptr Cyc_yytname[296]={{_tmp2FB,_tmp2FB,_tmp2FB + 2},{_tmp2FC,_tmp2FC,_tmp2FC + 6},{_tmp2FD,_tmp2FD,_tmp2FD + 12},{_tmp2FE,_tmp2FE,_tmp2FE + 5},{_tmp2FF,_tmp2FF,_tmp2FF + 9},{_tmp300,_tmp300,_tmp300 + 7},{_tmp301,_tmp301,_tmp301 + 7},{_tmp302,_tmp302,_tmp302 + 8},{_tmp303,_tmp303,_tmp303 + 5},{_tmp304,_tmp304,_tmp304 + 5},{_tmp305,_tmp305,_tmp305 + 6},{_tmp306,_tmp306,_tmp306 + 4},{_tmp307,_tmp307,_tmp307 + 5},{_tmp308,_tmp308,_tmp308 + 6},{_tmp309,_tmp309,_tmp309 + 7},{_tmp30A,_tmp30A,_tmp30A + 7},{_tmp30B,_tmp30B,_tmp30B + 9},{_tmp30C,_tmp30C,_tmp30C + 6},{_tmp30D,_tmp30D,_tmp30D + 9},{_tmp30E,_tmp30E,_tmp30E + 9},{_tmp30F,_tmp30F,_tmp30F + 7},{_tmp310,_tmp310,_tmp310 + 6},{_tmp311,_tmp311,_tmp311 + 5},{_tmp312,_tmp312,_tmp312 + 8},{_tmp313,_tmp313,_tmp313 + 7},{_tmp314,_tmp314,_tmp314 + 7},{_tmp315,_tmp315,_tmp315 + 9},{_tmp316,_tmp316,_tmp316 + 3},{_tmp317,_tmp317,_tmp317 + 5},{_tmp318,_tmp318,_tmp318 + 7},{_tmp319,_tmp319,_tmp319 + 6},{_tmp31A,_tmp31A,_tmp31A + 3},{_tmp31B,_tmp31B,_tmp31B + 4},{_tmp31C,_tmp31C,_tmp31C + 5},{_tmp31D,_tmp31D,_tmp31D + 9},{_tmp31E,_tmp31E,_tmp31E + 6},{_tmp31F,_tmp31F,_tmp31F + 7},{_tmp320,_tmp320,_tmp320 + 5},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 16},{_tmp323,_tmp323,_tmp323 + 8},{_tmp324,_tmp324,_tmp324 + 4},{_tmp325,_tmp325,_tmp325 + 6},{_tmp326,_tmp326,_tmp326 + 4},{_tmp327,_tmp327,_tmp327 + 6},{_tmp328,_tmp328,_tmp328 + 7},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 9},{_tmp32B,_tmp32B,_tmp32B + 9},{_tmp32C,_tmp32C,_tmp32C + 6},{_tmp32D,_tmp32D,_tmp32D + 10},{_tmp32E,_tmp32E,_tmp32E + 9},{_tmp32F,_tmp32F,_tmp32F + 7},{_tmp330,_tmp330,_tmp330 + 8},{_tmp331,_tmp331,_tmp331 + 15},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 8},{_tmp334,_tmp334,_tmp334 + 5},{_tmp335,_tmp335,_tmp335 + 9},{_tmp336,_tmp336,_tmp336 + 6},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 5},{_tmp339,_tmp339,_tmp339 + 8},{_tmp33A,_tmp33A,_tmp33A + 13},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 8},{_tmp33D,_tmp33D,_tmp33D + 12},{_tmp33E,_tmp33E,_tmp33E + 8},{_tmp33F,_tmp33F,_tmp33F + 8},{_tmp340,_tmp340,_tmp340 + 10},{_tmp341,_tmp341,_tmp341 + 9},{_tmp342,_tmp342,_tmp342 + 13},{_tmp343,_tmp343,_tmp343 + 10},{_tmp344,_tmp344,_tmp344 + 9},{_tmp345,_tmp345,_tmp345 + 13},{_tmp346,_tmp346,_tmp346 + 14},{_tmp347,_tmp347,_tmp347 + 14},{_tmp348,_tmp348,_tmp348 + 13},{_tmp349,_tmp349,_tmp349 + 12},{_tmp34A,_tmp34A,_tmp34A + 16},{_tmp34B,_tmp34B,_tmp34B + 14},{_tmp34C,_tmp34C,_tmp34C + 12},{_tmp34D,_tmp34D,_tmp34D + 16},{_tmp34E,_tmp34E,_tmp34E + 15},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 7},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 8},{_tmp353,_tmp353,_tmp353 + 9},{_tmp354,_tmp354,_tmp354 + 6},{_tmp355,_tmp355,_tmp355 + 6},{_tmp356,_tmp356,_tmp356 + 6},{_tmp357,_tmp357,_tmp357 + 6},{_tmp358,_tmp358,_tmp358 + 7},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 11},{_tmp35B,_tmp35B,_tmp35B + 11},{_tmp35C,_tmp35C,_tmp35C + 11},{_tmp35D,_tmp35D,_tmp35D + 11},{_tmp35E,_tmp35E,_tmp35E + 11},{_tmp35F,_tmp35F,_tmp35F + 12},{_tmp360,_tmp360,_tmp360 + 13},{_tmp361,_tmp361,_tmp361 + 11},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 10},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,_tmp365,_tmp365 + 11},{_tmp366,_tmp366,_tmp366 + 12},{_tmp367,_tmp367,_tmp367 + 11},{_tmp368,_tmp368,_tmp368 + 17},{_tmp369,_tmp369,_tmp369 + 7},{_tmp36A,_tmp36A,_tmp36A + 8},{_tmp36B,_tmp36B,_tmp36B + 19},{_tmp36C,_tmp36C,_tmp36C + 20},{_tmp36D,_tmp36D,_tmp36D + 18},{_tmp36E,_tmp36E,_tmp36E + 9},{_tmp36F,_tmp36F,_tmp36F + 13},{_tmp370,_tmp370,_tmp370 + 16},{_tmp371,_tmp371,_tmp371 + 18},{_tmp372,_tmp372,_tmp372 + 10},{_tmp373,_tmp373,_tmp373 + 4},{_tmp374,_tmp374,_tmp374 + 4},{_tmp375,_tmp375,_tmp375 + 4},{_tmp376,_tmp376,_tmp376 + 4},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 5},{_tmp390,_tmp390,_tmp390 + 17},{_tmp391,_tmp391,_tmp391 + 16},{_tmp392,_tmp392,_tmp392 + 13},{_tmp393,_tmp393,_tmp393 + 12},{_tmp394,_tmp394,_tmp394 + 19},{_tmp395,_tmp395,_tmp395 + 21},{_tmp396,_tmp396,_tmp396 + 15},{_tmp397,_tmp397,_tmp397 + 20},{_tmp398,_tmp398,_tmp398 + 21},{_tmp399,_tmp399,_tmp399 + 13},{_tmp39A,_tmp39A,_tmp39A + 15},{_tmp39B,_tmp39B,_tmp39B + 17},{_tmp39C,_tmp39C,_tmp39C + 19},{_tmp39D,_tmp39D,_tmp39D + 12},{_tmp39E,_tmp39E,_tmp39E + 19},{_tmp39F,_tmp39F,_tmp39F + 17},{_tmp3A0,_tmp3A0,_tmp3A0 + 23},{_tmp3A1,_tmp3A1,_tmp3A1 + 24},{_tmp3A2,_tmp3A2,_tmp3A2 + 15},{_tmp3A3,_tmp3A3,_tmp3A3 + 11},{_tmp3A4,_tmp3A4,_tmp3A4 + 15},{_tmp3A5,_tmp3A5,_tmp3A5 + 10},{_tmp3A6,_tmp3A6,_tmp3A6 + 15},{_tmp3A7,_tmp3A7,_tmp3A7 + 25},{_tmp3A8,_tmp3A8,_tmp3A8 + 5},{_tmp3A9,_tmp3A9,_tmp3A9 + 15},{_tmp3AA,_tmp3AA,_tmp3AA + 15},{_tmp3AB,_tmp3AB,_tmp3AB + 11},{_tmp3AC,_tmp3AC,_tmp3AC + 22},{_tmp3AD,_tmp3AD,_tmp3AD + 26},{_tmp3AE,_tmp3AE,_tmp3AE + 16},{_tmp3AF,_tmp3AF,_tmp3AF + 16},{_tmp3B0,_tmp3B0,_tmp3B0 + 24},{_tmp3B1,_tmp3B1,_tmp3B1 + 25},{_tmp3B2,_tmp3B2,_tmp3B2 + 21},{_tmp3B3,_tmp3B3,_tmp3B3 + 22},{_tmp3B4,_tmp3B4,_tmp3B4 + 16},{_tmp3B5,_tmp3B5,_tmp3B5 + 19},{_tmp3B6,_tmp3B6,_tmp3B6 + 25},{_tmp3B7,_tmp3B7,_tmp3B7 + 35},{_tmp3B8,_tmp3B8,_tmp3B8 + 23},{_tmp3B9,_tmp3B9,_tmp3B9 + 24},{_tmp3BA,_tmp3BA,_tmp3BA + 18},{_tmp3BB,_tmp3BB,_tmp3BB + 20},{_tmp3BC,_tmp3BC,_tmp3BC + 19},{_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 14},{_tmp3BF,_tmp3BF,_tmp3BF + 19},{_tmp3C0,_tmp3C0,_tmp3C0 + 20},{_tmp3C1,_tmp3C1,_tmp3C1 + 14},{_tmp3C2,_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 23},{_tmp3C4,_tmp3C4,_tmp3C4 + 18},{_tmp3C5,_tmp3C5,_tmp3C5 + 30},{_tmp3C6,_tmp3C6,_tmp3C6 + 8},{_tmp3C7,_tmp3C7,_tmp3C7 + 12},{_tmp3C8,_tmp3C8,_tmp3C8 + 14},{_tmp3C9,_tmp3C9,_tmp3C9 + 13},{_tmp3CA,_tmp3CA,_tmp3CA + 23},{_tmp3CB,_tmp3CB,_tmp3CB + 14},{_tmp3CC,_tmp3CC,_tmp3CC + 18},{_tmp3CD,_tmp3CD,_tmp3CD + 8},{_tmp3CE,_tmp3CE,_tmp3CE + 11},{_tmp3CF,_tmp3CF,_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 9},{_tmp3D1,_tmp3D1,_tmp3D1 + 16},{_tmp3D2,_tmp3D2,_tmp3D2 + 19},{_tmp3D3,_tmp3D3,_tmp3D3 + 10},{_tmp3D4,_tmp3D4,_tmp3D4 + 16},{_tmp3D5,_tmp3D5,_tmp3D5 + 11},{_tmp3D6,_tmp3D6,_tmp3D6 + 14},{_tmp3D7,_tmp3D7,_tmp3D7 + 11},{_tmp3D8,_tmp3D8,_tmp3D8 + 15},{_tmp3D9,_tmp3D9,_tmp3D9 + 22},{_tmp3DA,_tmp3DA,_tmp3DA + 16},{_tmp3DB,_tmp3DB,_tmp3DB + 17},{_tmp3DC,_tmp3DC,_tmp3DC + 12},{_tmp3DD,_tmp3DD,_tmp3DD + 18},{_tmp3DE,_tmp3DE,_tmp3DE + 17},{_tmp3DF,_tmp3DF,_tmp3DF + 12},{_tmp3E0,_tmp3E0,_tmp3E0 + 16},{_tmp3E1,_tmp3E1,_tmp3E1 + 11},{_tmp3E2,_tmp3E2,_tmp3E2 + 10},{_tmp3E3,_tmp3E3,_tmp3E3 + 14},{_tmp3E4,_tmp3E4,_tmp3E4 + 15},{_tmp3E5,_tmp3E5,_tmp3E5 + 20},{_tmp3E6,_tmp3E6,_tmp3E6 + 27},{_tmp3E7,_tmp3E7,_tmp3E7 + 10},{_tmp3E8,_tmp3E8,_tmp3E8 + 18},{_tmp3E9,_tmp3E9,_tmp3E9 + 21},{_tmp3EA,_tmp3EA,_tmp3EA + 19},{_tmp3EB,_tmp3EB,_tmp3EB + 16},{_tmp3EC,_tmp3EC,_tmp3EC + 20},{_tmp3ED,_tmp3ED,_tmp3ED + 15},{_tmp3EE,_tmp3EE,_tmp3EE + 20},{_tmp3EF,_tmp3EF,_tmp3EF + 15},{_tmp3F0,_tmp3F0,_tmp3F0 + 12},{_tmp3F1,_tmp3F1,_tmp3F1 + 20},{_tmp3F2,_tmp3F2,_tmp3F2 + 19},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 21},{_tmp3F5,_tmp3F5,_tmp3F5 + 21},{_tmp3F6,_tmp3F6,_tmp3F6 + 12},{_tmp3F7,_tmp3F7,_tmp3F7 + 17},{_tmp3F8,_tmp3F8,_tmp3F8 + 19},{_tmp3F9,_tmp3F9,_tmp3F9 + 14},{_tmp3FA,_tmp3FA,_tmp3FA + 17},{_tmp3FB,_tmp3FB,_tmp3FB + 23},{_tmp3FC,_tmp3FC,_tmp3FC + 13},{_tmp3FD,_tmp3FD,_tmp3FD + 14},{_tmp3FE,_tmp3FE,_tmp3FE + 16},{_tmp3FF,_tmp3FF,_tmp3FF + 16},{_tmp400,_tmp400,_tmp400 + 8},{_tmp401,_tmp401,_tmp401 + 19},{_tmp402,_tmp402,_tmp402 + 20},{_tmp403,_tmp403,_tmp403 + 14},{_tmp404,_tmp404,_tmp404 + 19},{_tmp405,_tmp405,_tmp405 + 20},{_tmp406,_tmp406,_tmp406 + 11},{_tmp407,_tmp407,_tmp407 + 22},{_tmp408,_tmp408,_tmp408 + 20},{_tmp409,_tmp409,_tmp409 + 23},{_tmp40A,_tmp40A,_tmp40A + 20},{_tmp40B,_tmp40B,_tmp40B + 22},{_tmp40C,_tmp40C,_tmp40C + 23},{_tmp40D,_tmp40D,_tmp40D + 24},{_tmp40E,_tmp40E,_tmp40E + 24},{_tmp40F,_tmp40F,_tmp40F + 15},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 22},{_tmp412,_tmp412,_tmp412 + 17},{_tmp413,_tmp413,_tmp413 + 20},{_tmp414,_tmp414,_tmp414 + 26},{_tmp415,_tmp415,_tmp415 + 16},{_tmp416,_tmp416,_tmp416 + 17},{_tmp417,_tmp417,_tmp417 + 15},{_tmp418,_tmp418,_tmp418 + 19},{_tmp419,_tmp419,_tmp419 + 17},{_tmp41A,_tmp41A,_tmp41A + 19},{_tmp41B,_tmp41B,_tmp41B + 25},{_tmp41C,_tmp41C,_tmp41C + 26},{_tmp41D,_tmp41D,_tmp41D + 9},{_tmp41E,_tmp41E,_tmp41E + 20},{_tmp41F,_tmp41F,_tmp41F + 17},{_tmp420,_tmp420,_tmp420 + 18},{_tmp421,_tmp421,_tmp421 + 11},{_tmp422,_tmp422,_tmp422 + 12}};
# 1664
static short Cyc_yyr1[530]={0,148,149,149,149,149,149,149,149,149,149,149,150,151,152,152,153,153,153,154,154,154,155,155,156,156,156,156,157,157,158,159,160,161,162,162,162,162,162,162,162,163,163,164,164,165,165,165,165,165,165,165,165,165,165,166,166,166,166,166,166,166,167,167,168,169,169,170,170,170,170,171,171,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,173,174,174,174,175,175,175,176,176,177,177,177,178,178,178,178,178,179,179,180,180,181,181,182,182,183,184,184,185,185,186,187,187,187,187,187,187,188,188,188,188,188,188,189,190,190,191,191,191,191,192,192,193,193,194,194,194,195,195,196,196,196,196,197,197,197,198,198,199,199,200,200,201,201,201,201,201,201,201,201,201,201,202,202,202,202,202,202,202,202,202,202,202,203,203,204,205,205,206,206,206,206,206,206,206,206,207,207,207,208,208,209,209,209,210,210,210,211,211,212,212,212,212,213,213,214,214,215,215,216,216,217,217,218,218,219,219,219,219,220,220,221,221,222,222,222,223,224,224,225,225,226,226,226,226,226,227,227,227,227,228,228,229,229,230,230,231,231,232,232,232,232,232,233,233,234,234,234,235,235,235,235,235,235,235,235,235,235,235,236,236,236,236,236,236,236,237,238,238,239,239,240,240,240,240,240,240,240,240,241,241,241,241,241,241,242,242,242,242,242,242,243,243,243,243,243,243,243,243,243,243,243,243,243,243,244,244,244,244,244,244,244,244,245,246,246,247,247,248,248,249,249,250,250,251,251,252,252,252,253,253,253,253,253,254,254,254,255,255,255,256,256,256,256,257,257,258,258,258,258,258,258,259,260,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,262,262,262,263,263,264,264,265,265,265,266,266,267,267,268,268,268,269,269,269,269,269,269,269,269,269,269,269,270,270,270,270,270,270,270,271,272,272,273,273,274,274,275,275,276,276,277,277,277,278,278,278,278,278,279,279,279,280,280,280,281,281,281,281,282,282,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,284,284,284,285,285,285,285,285,285,285,285,285,285,285,286,286,286,286,287,287,287,287,287,287,287,287,287,287,288,289,289,290,290,290,290,290,291,291,292,292,293,293,294,294,295,295};
# 1720
static short Cyc_yyr2[530]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1776
static short Cyc_yydefact[1077]={0,21,55,56,57,58,60,73,74,75,76,77,78,79,80,81,99,100,101,117,118,51,0,0,85,0,61,0,0,155,92,96,0,0,0,0,0,0,42,520,221,522,521,523,0,0,88,0,207,207,206,1,0,0,19,0,0,20,0,0,45,53,47,71,49,82,83,0,86,0,41,166,0,191,194,87,170,115,59,58,52,0,103,0,519,0,520,515,516,517,518,115,0,382,0,0,0,0,244,0,384,385,30,32,0,0,0,0,0,0,0,156,0,0,0,0,0,0,0,204,205,0,2,0,0,0,0,0,34,0,123,124,126,46,54,48,50,119,524,525,115,115,41,43,41,0,23,0,223,0,179,167,192,0,198,199,202,203,0,201,200,212,194,0,72,59,107,0,105,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,504,505,483,0,0,0,0,0,486,484,485,0,410,412,426,434,436,438,440,442,444,447,452,455,458,462,0,464,487,503,502,520,393,0,0,0,0,0,394,392,37,0,0,115,0,0,0,133,129,131,264,266,0,0,9,10,0,0,115,526,527,222,98,0,0,171,89,242,0,239,0,0,3,0,5,0,0,0,35,0,0,41,23,0,120,121,146,114,0,153,0,0,0,0,0,0,0,0,0,0,0,0,520,295,297,0,305,299,0,303,287,288,289,0,290,291,292,0,44,23,126,22,24,271,0,229,245,0,0,225,223,0,209,0,0,0,214,62,213,195,0,108,104,0,0,0,472,0,428,462,0,429,430,0,0,0,0,0,0,0,0,0,0,465,466,41,0,0,0,468,469,467,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,416,417,418,419,420,421,422,423,424,425,415,0,470,0,493,494,0,0,0,507,0,115,386,0,0,407,520,0,0,0,0,260,403,408,0,405,0,383,400,401,0,398,0,246,0,0,0,0,267,0,237,134,139,135,137,130,132,223,0,273,265,274,529,528,0,91,95,0,93,97,113,68,67,0,65,172,223,241,168,273,243,180,181,0,90,208,13,0,31,0,33,0,39,0,0,125,127,248,247,23,25,110,122,0,0,0,141,142,149,0,115,115,161,0,0,0,0,0,520,0,41,0,334,335,336,0,0,338,0,0,41,0,306,300,126,304,298,296,26,0,178,230,0,0,0,236,224,231,149,0,0,0,225,177,211,210,173,209,0,0,215,63,116,109,433,106,102,0,0,0,520,249,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,506,513,0,512,411,435,0,437,439,441,443,445,446,450,451,448,449,453,454,456,457,459,460,461,414,413,492,489,0,491,0,0,0,396,397,263,0,404,258,261,391,0,259,388,0,395,36,0,389,0,268,0,140,136,138,0,225,0,209,0,275,0,223,0,286,270,0,115,0,0,0,133,0,115,0,223,0,190,169,240,0,7,0,4,6,0,0,27,0,145,128,146,0,0,144,225,154,163,162,0,0,157,0,0,0,313,0,0,0,0,0,0,333,337,0,0,0,0,301,294,41,28,272,223,0,233,0,0,151,226,0,149,229,217,174,196,197,215,193,471,0,0,250,0,255,474,0,0,0,0,0,479,482,0,0,0,0,0,463,509,0,0,490,488,0,0,0,262,406,409,399,402,390,269,238,149,0,276,277,209,0,0,225,209,0,94,225,520,0,64,66,0,182,0,0,225,0,209,0,0,8,0,0,143,0,147,119,152,164,161,161,0,0,0,0,0,0,0,0,0,0,0,0,0,313,339,0,0,302,29,225,0,234,232,0,175,0,151,225,0,216,499,0,498,0,251,256,0,0,0,0,0,431,432,492,491,511,495,0,514,427,508,510,0,387,151,149,279,285,149,0,278,209,119,0,69,183,189,149,0,188,184,209,15,0,16,0,38,0,0,0,160,159,307,313,0,0,0,0,0,0,341,342,344,346,348,350,352,354,357,362,365,368,372,374,380,381,0,41,310,319,0,0,0,0,0,0,0,0,0,0,340,293,219,235,0,227,176,218,223,473,0,0,257,475,476,0,0,481,480,496,0,282,151,151,149,280,0,0,151,149,185,14,17,0,0,148,112,0,0,0,313,0,378,0,0,375,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,376,313,0,321,0,0,0,329,0,0,0,0,0,0,312,0,0,225,501,500,0,0,0,497,281,284,151,111,0,186,151,18,40,165,308,309,0,0,0,0,313,315,345,0,347,349,351,353,355,356,360,361,358,359,363,364,366,367,369,370,371,0,314,320,322,323,0,331,330,0,325,0,0,0,150,228,220,0,0,0,0,283,0,187,311,377,0,373,316,0,41,324,332,326,327,0,253,252,477,0,70,0,343,313,317,328,0,379,318,478,0,0,0};
# 1887
static short Cyc_yydefgoto[148]={1074,51,52,466,648,850,53,307,54,291,55,468,56,470,57,58,144,59,60,539,232,452,453,233,63,246,234,65,166,167,66,164,67,268,269,129,130,131,270,235,434,484,485,486,660,801,68,69,665,666,667,70,487,71,457,72,73,161,162,74,119,535,322,704,621,75,622,529,695,521,525,526,428,315,254,97,98,551,476,552,408,409,410,236,308,309,623,440,294,295,296,297,298,299,782,300,301,866,867,868,869,870,871,872,873,874,875,876,877,878,879,880,881,411,419,420,412,413,414,302,197,390,198,544,199,200,201,202,203,204,205,206,207,208,209,210,211,212,808,213,568,569,214,215,77,851,415,444};
# 1905
static short Cyc_yypact[1077]={2880,- -32768,- -32768,- -32768,- -32768,- 28,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3583,243,- 23,- -32768,1040,- -32768,260,6,- -32768,17,36,62,117,127,168,255,258,- -32768,- -32768,222,- -32768,- -32768,- -32768,217,268,264,287,312,312,- -32768,- -32768,336,2741,- -32768,385,617,- -32768,328,821,3583,3583,3583,- -32768,3583,- -32768,- -32768,466,- -32768,260,3497,91,63,574,990,- -32768,- -32768,334,373,410,- -32768,260,405,6600,- -32768,4165,- 45,- -32768,- -32768,- -32768,- -32768,334,6600,- -32768,408,439,4165,436,438,443,- -32768,- 12,- -32768,- -32768,3733,3733,2741,2741,3733,6600,539,- -32768,227,449,200,450,227,4108,6600,- -32768,- -32768,3018,- -32768,2741,3018,2741,3018,66,- -32768,464,467,- -32768,3368,- -32768,- -32768,- -32768,- -32768,4108,- -32768,- -32768,334,139,1739,- -32768,3497,821,479,3733,3658,4934,- -32768,91,- -32768,500,- -32768,- -32768,- -32768,- -32768,502,- -32768,- -32768,78,990,3733,- -32768,- -32768,483,520,537,260,6892,540,6600,6746,575,592,600,633,635,642,660,676,680,6989,6989,- -32768,- -32768,- -32768,2314,688,7037,7037,7037,- -32768,- -32768,- -32768,266,- -32768,- -32768,- 16,638,683,679,686,722,89,749,511,138,- -32768,1141,7037,391,- 39,- -32768,703,110,- -32768,4165,107,2564,273,2842,743,- -32768,- -32768,746,6600,334,2842,731,3883,4108,4183,4108,380,- -32768,- 38,- 38,- -32768,- -32768,43,306,334,- -32768,- -32768,- -32768,- -32768,55,738,- -32768,- -32768,126,324,- -32768,757,771,- -32768,784,- -32768,786,609,803,- -32768,200,5032,3497,479,799,4108,- -32768,433,809,260,811,807,170,813,4290,816,828,825,827,5130,4290,165,817,805,- -32768,- -32768,823,1884,1884,821,1884,- -32768,- -32768,- -32768,832,- -32768,- -32768,- -32768,257,- -32768,479,835,- -32768,- -32768,830,45,854,- -32768,46,837,831,292,838,782,829,6600,3733,- -32768,851,- -32768,- -32768,45,260,- -32768,6600,850,2314,- -32768,4108,- -32768,- -32768,4388,- -32768,881,6600,6600,6600,6600,6600,6600,6600,4108,760,2314,- -32768,- -32768,2029,868,372,6600,- -32768,- -32768,- -32768,6600,- -32768,7037,6600,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,6600,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6600,- -32768,227,- -32768,- -32768,5228,227,6600,- -32768,873,334,- -32768,852,875,- -32768,95,227,6600,4165,241,- -32768,- -32768,- -32768,877,879,872,- -32768,- -32768,- -32768,880,882,4165,- -32768,348,2564,883,3733,- -32768,891,892,- -32768,4183,4183,4183,- -32768,- -32768,3101,5326,350,- -32768,192,- -32768,- -32768,46,- -32768,- -32768,3733,- -32768,- -32768,895,- -32768,890,893,900,- -32768,1398,- -32768,214,632,- -32768,- -32768,- -32768,4108,- -32768,- -32768,- -32768,2602,- -32768,2741,- -32768,2741,- -32768,920,902,- -32768,- -32768,- -32768,- -32768,479,- -32768,- -32768,- -32768,607,6600,909,908,- -32768,- 19,333,334,334,834,6600,6600,906,916,6600,910,1013,1244,923,- -32768,- -32768,- -32768,572,1001,- -32768,5424,6600,2174,2454,- -32768,- -32768,3368,- -32768,- -32768,- -32768,- -32768,3733,- -32768,- -32768,4108,918,3958,- -32768,- -32768,911,971,46,922,4033,831,- -32768,- -32768,- -32768,- -32768,782,928,67,814,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,930,936,958,939,- -32768,- -32768,741,5032,943,949,950,953,954,399,955,956,673,960,959,6844,- -32768,- -32768,961,967,- -32768,638,73,683,679,686,722,89,89,749,749,749,749,511,511,138,138,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,963,- -32768,133,3733,4836,4108,- -32768,- -32768,944,- -32768,- -32768,- -32768,- -32768,2703,- -32768,- -32768,3194,- -32768,- -32768,970,- -32768,102,- -32768,4108,- -32768,- -32768,- -32768,965,831,968,782,948,192,3733,3808,5522,- -32768,- -32768,- 38,334,6698,969,55,3232,974,334,3733,3658,5620,- -32768,214,- -32768,977,- -32768,2741,- -32768,- -32768,972,988,- -32768,787,- -32768,- -32768,433,976,6600,- -32768,831,- -32768,- -32768,- -32768,982,260,597,409,415,6600,853,419,4290,979,5718,5816,641,- -32768,- -32768,986,991,980,457,1884,- -32768,3497,- -32768,830,993,3733,- -32768,994,46,1034,- -32768,992,971,148,- -32768,- -32768,- -32768,- -32768,814,- -32768,996,181,995,- -32768,4500,- -32768,- -32768,6600,6600,1097,6600,6746,- -32768,- -32768,227,227,996,997,4612,- -32768,- -32768,6600,6600,- -32768,- -32768,45,759,1015,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,971,999,- -32768,- -32768,782,45,1000,831,782,989,- -32768,831,1006,1002,- -32768,- -32768,1005,- -32768,45,1007,831,1009,782,1008,239,- -32768,6600,1004,- -32768,6600,- -32768,4108,- -32768,1012,53,834,4290,1019,1017,3049,1003,1021,4290,6600,5914,659,6012,666,6110,853,- -32768,1026,1029,- -32768,- -32768,831,132,- -32768,- -32768,1023,- -32768,1041,1034,831,4108,- -32768,- -32768,613,- -32768,6600,- -32768,- -32768,5032,1030,1032,1027,1042,- -32768,881,1035,1036,- -32768,- -32768,763,- -32768,- -32768,- -32768,- -32768,4836,- -32768,1034,971,- -32768,- -32768,971,1038,- -32768,782,4108,1049,- -32768,- -32768,- -32768,971,1043,- -32768,- -32768,782,- -32768,1046,674,508,- -32768,1045,1052,4108,- -32768,- -32768,1149,853,1056,7134,1051,2454,7037,1048,- -32768,- 3,- -32768,1088,1047,748,808,97,824,612,344,- -32768,- -32768,- -32768,- -32768,1092,7037,2029,- -32768,- -32768,516,4290,541,6208,4290,551,6306,6404,691,1063,- -32768,- -32768,- -32768,- -32768,6600,1064,- -32768,- -32768,993,- -32768,311,293,- -32768,- -32768,- -32768,4108,1164,- -32768,- -32768,- -32768,4724,- -32768,1034,1034,971,- -32768,1068,1070,1034,971,- -32768,- -32768,- -32768,539,1071,- -32768,- -32768,559,4290,1072,853,2314,- -32768,4108,1067,- -32768,1594,7037,6600,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,7037,6600,- -32768,853,1076,- -32768,4290,4290,563,- -32768,4290,4290,576,4290,580,6502,- -32768,1074,46,831,- -32768,- -32768,2454,1075,1077,- -32768,- -32768,- -32768,1034,- -32768,1090,- -32768,1034,- -32768,- -32768,- -32768,- -32768,- -32768,1085,1080,1087,7037,853,- -32768,638,305,683,679,679,722,89,89,749,749,749,749,511,511,138,138,- -32768,- -32768,- -32768,361,- -32768,- -32768,- -32768,- -32768,4290,- -32768,- -32768,4290,- -32768,4290,4290,603,- -32768,- -32768,- -32768,1091,778,1084,4108,- -32768,1093,- -32768,- -32768,996,181,- -32768,- -32768,6600,1594,- -32768,- -32768,- -32768,- -32768,4290,- -32768,- -32768,- -32768,1094,- -32768,648,- -32768,853,- -32768,- -32768,1095,- -32768,- -32768,- -32768,1200,1217,- -32768};
# 2016
static short Cyc_yypgoto[148]={- -32768,286,- -32768,- -32768,- -32768,290,- -32768,- 220,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,22,- -32768,- 92,33,- -32768,- -32768,0,585,- -32768,88,- 184,1109,13,- -32768,- -32768,- 128,- -32768,550,1191,- 724,- -32768,- -32768,- -32768,964,962,565,417,- -32768,- -32768,573,- 68,- 735,- -32768,- -32768,144,- -32768,- -32768,59,- 215,1160,- 389,259,- -32768,1073,- -32768,- -32768,1184,- 396,- -32768,531,- 131,- 159,- 126,- 67,291,548,579,- 416,- 471,- 109,- 419,- 112,- -32768,- 253,- 150,- 542,- 325,- -32768,858,14,- 18,- 135,- 105,- 358,80,- -32768,- -32768,- 15,- 285,- -32768,- 631,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,225,1044,- -32768,667,855,- -32768,143,503,- -32768,- 168,- 356,- 149,- 348,- 341,- 329,912,- 322,- 347,- 338,- 346,- 335,- 176,619,496,931,230,- -32768,- 351,- -32768,245,553,- 64,18,- 111,162};
# 2038
static short Cyc_yytable[7282]={61,247,323,139,333,247,511,512,253,514,553,571,475,64,354,355,356,313,577,578,573,61,314,336,337,583,584,631,325,579,580,581,582,574,64,391,316,456,585,586,266,329,576,645,594,114,139,479,855,432,441,602,692,61,80,146,731,658,663,664,61,61,61,218,61,564,64,398,903,644,61,150,450,64,64,64,- 245,64,359,- 245,626,219,78,64,517,140,237,238,62,442,241,944,143,133,134,135,918,136,399,644,626,443,689,145,115,83,61,61,522,62,228,645,696,659,103,923,229,267,132,64,64,61,360,61,61,61,61,655,243,304,439,441,61,441,64,945,64,64,64,64,700,62,61,104,61,64,740,459,62,62,62,150,62,524,143,64,682,64,62,896,543,40,105,451,292,145,303,446,523,518,442,39,442,249,261,293,- 158,145,367,368,42,824,443,106,988,989,952,953,461,531,993,45,262,40,62,62,702,357,542,587,588,589,351,218,305,727,443,461,321,62,44,62,62,62,62,369,370,147,218,148,62,219,402,954,955,149,196,- 526,744,936,62,739,62,431,39,221,219,403,107,543,638,443,41,42,43,429,433,432,432,432,99,478,242,108,455,1045,520,357,653,1047,437,900,48,49,50,163,61,638,729,312,100,443,273,553,375,19,20,- 223,39,64,- 223,592,376,377,524,595,506,42,303,244,807,61,61,507,61,601,109,245,493,145,710,494,538,771,64,64,1000,64,39,111,217,44,1005,292,292,543,292,42,627,779,628,224,540,305,293,293,629,293,45,112,100,352,152,44,1026,244,48,49,50,122,640,100,641,245,546,113,547,39,642,833,61,39,513,837,62,41,42,43,498,561,42,563,849,64,505,81,604,847,39,524,423,116,292,1052,406,407,39,42,516,62,62,357,62,293,813,42,905,44,127,724,357,639,239,240,461,358,45,357,553,794,445,253,416,447,48,49,50,256,615,257,258,259,260,312,854,117,530,357,461,244,982,686,757,1068,461,984,504,245,632,357,357,431,431,431,118,448,1072,1053,62,757,630,39,922,401,433,433,433,418,462,41,42,43,927,463,418,637,812,643,121,694,163,482,730,100,699,100,61,100,61,612,61,519,357,352,100,392,393,394,436,64,960,64,654,64,437,357,559,541,961,962,352,543,746,1054,438,- 12,357,747,687,61,688,566,748,572,813,759,123,124,436,61,760,458,64,61,437,761,48,49,50,395,165,676,64,357,396,397,64,169,716,762,488,684,145,357,524,143,222,429,777,357,596,39,145,357,778,979,145,223,783,41,42,43,703,76,62,742,62,225,62,826,909,482,226,796,696,483,818,819,227,48,49,50,804,639,39,82,812,248,101,250,102,357,41,42,43,263,793,62,137,685,461,264,813,461,772,809,1006,62,76,965,306,62,543,1012,1013,1008,76,326,820,821,1018,1019,76,732,1014,1015,1016,1017,1009,1010,138,255,141,1020,1021,76,630,1011,319,803,320,429,357,603,168,668,669,931,101,672,357,220,677,327,643,966,611,39,61,101,373,683,318,100,374,41,42,43,1004,76,76,64,328,138,812,357,100,76,332,100,968,795,797,831,76,357,76,76,76,76,972,836,252,462,61,839,61,357,997,942,272,274,1030,679,845,438,357,64,76,64,357,139,271,338,357,1033,292,964,303,1035,48,49,50,252,458,39,703,293,775,145,339,776,168,41,42,43,44,357,340,899,471,361,1059,543,472,482,62,904,125,126,39,488,906,48,49,50,907,934,41,42,43,958,647,685,649,959,650,719,393,394,455,341,789,342,919,357,437,920,477,1067,343,101,62,101,62,101,925,1071,424,981,890,907,101,357,1022,1023,1024,893,344,331,357,334,334,449,751,929,983,430,930,435,84,395,348,349,345,460,720,397,346,334,334,334,977,365,366,357,353,76,786,788,363,524,537,362,460,400,490,1051,364,495,334,16,17,18,271,737,371,372,477,663,664,554,555,556,557,558,76,560,618,619,620,421,100,990,422,100,567,859,994,426,570,533,534,886,708,709,139,454,39,87,185,186,88,89,90,780,781,42,941,168,464,590,828,829,61,1066,916,917,347,948,949,189,827,591,465,39,488,64,567,950,951,1061,357,41,42,43,292,467,834,469,852,956,957,488,1040,482,488,293,473,857,858,843,480,48,49,50,985,887,889,39,892,489,895,491,766,492,500,509,42,809,625,496,128,61,499,508,501,334,502,45,598,510,1001,908,1002,515,64,48,49,50,265,101,520,528,536,292,527,532,967,443,44,971,62,545,101,359,293,101,334,599,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,565,1041,597,606,252,600,607,608,882,610,221,460,609,567,460,614,567,616,998,617,633,634,76,252,76,635,76,636,100,334,252,651,652,656,62,657,970,670,460,974,976,671,661,662,460,673,674,678,680,690,658,1028,1029,693,697,1031,1032,61,1034,477,701,1063,705,706,153,154,155,156,157,707,64,158,159,160,- 526,711,712,713,252,292,714,715,733,84,352,722,745,717,718,252,293,1007,721,725,726,728,738,741,252,768,743,755,765,767,477,334,758,770,773,1025,784,790,792,1055,800,791,1056,312,1057,1058,798,723,802,1037,810,816,830,838,853,822,1042,832,835,840,841,750,85,842,884,844,754,846,1069,856,860,62,848,885,764,861,897,86,87,898,901,88,89,90,912,902,42,924,910,101,911,91,101,914,915,913,921,92,928,93,94,926,567,932,933,95,935,937,940,943,946,96,752,334,963,978,947,980,986,460,991,996,252,992,999,1003,1027,378,1046,1075,76,430,1038,1043,1044,252,460,1048,1049,460,1050,477,1062,1060,814,815,1076,817,774,995,756,1064,1070,1073,251,477,110,474,825,769,481,151,120,806,324,379,380,381,382,383,384,385,386,387,388,805,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,389,605,21,170,171,1039,799,425,735,575,883,562,334,613,1065,22,23,24,84,25,172,0,0,0,173,26,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,477,0,138,0,0,0,0,0,0,36,37,38,0,183,184,0,477,101,0,334,0,271,0,0,0,0,0,0,0,0,334,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,675,0,0,0,0,252,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,334,194,195,0,0,0,0,0,0,0,0,0,0,0,0,271,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,477,252,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,939,0,138,334,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,310,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,312,334,0,0,0,0,0,455,0,46,47,0,0,437,0,48,49,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,0,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,334,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,780,781,21,170,171,275,334,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,334,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,- 313,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,289,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,- 41,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,275,0,276,277,278,279,280,281,282,283,22,23,24,84,25,172,284,0,0,173,26,285,0,0,29,174,175,176,177,178,0,30,31,0,179,0,286,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,38,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,87,185,186,88,89,90,40,41,42,43,44,187,288,142,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,84,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,0,350,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,84,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,0,34,180,181,35,182,0,0,0,0,0,0,0,0,0,0,36,37,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,40,41,42,43,44,187,0,0,0,0,0,0,0,188,0,46,290,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,646,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,404,0,35,405,87,0,0,88,89,90,0,245,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,406,407,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,734,0,35,405,87,0,0,88,89,90,0,245,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,406,407,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,84,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,85,0,0,30,31,- 41,0,0,0,32,33,34,417,0,35,216,87,0,0,88,89,90,0,0,42,0,36,37,38,91,0,0,0,0,0,92,0,93,94,0,0,0,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,862,863,30,31,- 41,0,0,0,32,33,34,0,0,35,0,84,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,22,23,24,- 11,85,0,0,0,45,0,46,47,0,0,29,0,48,49,50,216,87,30,31,88,89,90,0,0,42,34,0,0,35,91,0,0,0,0,0,864,0,93,94,0,36,37,0,95,0,0,865,0,0,96,193,0,0,194,195,0,0,0,0,0,0,0,0,0,310,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,312,0,0,0,0,0,0,436,0,46,47,0,84,437,0,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,85,0,0,30,31,0,0,0,0,0,0,34,736,0,35,216,87,0,0,88,89,90,0,0,42,0,36,37,0,91,0,0,0,0,0,92,0,93,94,0,0,0,0,95,0,0,0,0,0,96,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,482,0,46,47,0,0,0,0,48,49,50,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,- 41,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,0,142,0,0,265,0,0,0,0,46,47,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,38,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,40,41,0,43,44,0,0,142,22,23,24,0,0,0,0,46,47,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,310,0,0,311,0,0,0,22,23,24,40,41,0,43,44,0,312,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,230,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,231,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,310,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,312,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,427,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,691,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,698,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,40,41,0,43,44,0,0,0,0,0,0,29,0,0,0,46,47,0,30,31,0,0,0,0,0,0,34,0,0,35,0,0,0,0,0,85,0,0,0,0,0,36,37,0,0,0,0,0,0,0,216,87,0,0,88,89,90,0,0,42,0,0,0,0,91,0,0,0,0,0,92,0,93,94,0,40,0,0,95,44,0,0,0,0,96,0,0,0,0,0,46,47,170,171,275,0,276,277,278,279,280,281,282,283,0,0,0,84,0,172,284,0,0,173,0,285,0,0,0,174,175,176,177,178,0,0,0,0,179,0,286,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,87,185,186,88,89,90,0,0,42,0,0,187,288,142,170,171,0,0,0,188,0,548,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,550,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,811,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,823,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,987,0,0,0,0,188,0,0,189,0,406,407,0,190,170,171,191,0,0,192,193,0,0,194,195,0,0,0,0,84,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,87,185,186,88,89,90,0,245,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,406,407,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,317,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,503,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,593,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,624,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,681,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,749,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,763,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,785,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,787,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,888,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,891,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,894,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,969,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,973,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,975,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,1036,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,172,0,192,193,173,0,194,195,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,753,87,185,186,88,89,90,180,181,42,182,0,187,0,0,0,0,0,0,0,188,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,335,170,171,0,0,0,188,0,0,189,0,0,0,0,190,0,84,191,0,0,192,193,0,0,194,195,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,39,87,185,186,88,89,90,180,181,42,182,0,187,0,723,0,0,0,0,0,188,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,170,171,0,0,0,0,330,0,0,189,0,0,0,0,190,84,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,170,171,0,0,0,0,0,0,0,0,0,0,183,184,0,84,0,0,0,0,0,0,0,0,0,0,0,174,175,176,177,178,0,0,0,39,87,185,186,88,89,90,180,181,42,182,0,187,0,0,0,0,0,0,0,347,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,170,171,0,0,0,0,188,0,0,189,0,0,0,0,190,84,0,191,0,0,192,193,0,0,194,195,0,174,175,176,177,178,0,0,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,185,186,88,89,90,0,0,42,0,0,187,0,0,0,0,0,0,0,938,0,0,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195};
# 2770
static short Cyc_yycheck[7282]={0,112,161,67,172,116,291,292,117,294,335,359,265,0,190,191,192,148,365,366,361,21,148,173,173,371,372,443,163,367,368,369,370,362,21,211,148,252,373,374,132,169,364,462,395,45,110,267,772,233,88,407,523,53,21,70,598,76,5,6,60,61,62,108,64,350,53,106,803,458,70,71,17,60,61,62,121,64,94,124,438,126,110,70,304,67,104,105,0,127,108,94,70,60,61,62,831,64,137,488,458,139,521,70,45,128,106,107,62,21,122,530,528,132,108,839,128,132,59,106,107,121,138,123,124,125,126,483,110,144,235,88,132,88,121,138,123,124,125,126,536,53,142,126,144,132,617,252,60,61,62,151,64,312,132,142,507,144,70,790,328,115,126,108,142,132,144,124,122,124,127,108,127,114,108,142,123,144,89,90,117,723,139,121,919,920,89,90,252,315,925,128,126,115,106,107,129,124,326,375,376,377,188,108,145,132,139,271,130,121,119,123,124,125,126,126,127,126,108,128,132,126,115,126,127,134,83,132,624,860,142,129,144,233,108,92,126,130,121,407,455,139,116,117,118,231,233,431,432,433,25,266,109,126,128,990,108,124,478,994,134,129,136,137,138,126,266,482,135,121,25,139,133,598,136,20,21,129,108,266,132,392,144,145,443,396,121,117,266,108,109,291,292,128,294,406,128,116,128,266,553,131,320,659,291,292,937,294,108,51,85,119,943,291,292,483,294,117,126,670,128,96,322,264,291,292,134,294,128,107,85,188,73,119,965,108,136,137,138,53,126,96,128,116,330,128,332,108,134,745,350,108,293,749,266,116,117,118,278,345,117,347,123,350,284,122,125,763,108,528,227,107,350,1004,133,134,108,117,121,291,292,124,294,350,709,117,805,119,60,565,124,455,106,107,458,129,128,124,723,684,238,510,129,241,136,137,138,121,426,123,124,125,126,121,770,128,124,124,482,108,109,513,637,1054,488,132,283,116,446,124,124,431,432,433,122,129,1067,132,350,654,440,108,838,218,431,432,433,222,124,116,117,118,848,129,229,455,709,457,122,527,126,128,597,218,531,220,466,222,468,121,470,309,124,330,229,84,85,86,128,466,136,468,482,470,134,124,343,325,144,145,347,659,627,132,235,122,124,628,513,499,518,129,628,360,829,640,121,122,128,509,641,252,499,513,134,641,136,137,138,128,110,499,509,124,133,134,513,122,129,641,271,509,499,124,693,513,128,523,129,124,397,108,509,124,129,901,513,108,129,116,117,118,539,0,466,622,468,121,470,727,813,128,124,689,980,132,716,716,125,136,137,138,698,637,108,22,829,128,25,129,27,124,116,117,118,121,129,499,122,509,654,124,917,657,661,706,944,509,45,884,121,513,770,950,951,946,53,124,719,720,956,957,59,599,952,953,954,955,947,948,67,118,69,958,959,72,626,949,128,697,128,617,124,408,81,492,493,129,85,496,124,91,499,123,644,129,421,108,648,96,139,508,149,408,143,116,117,118,943,106,107,648,125,110,917,124,421,114,128,424,129,686,690,741,121,124,123,124,125,126,129,748,117,124,684,752,686,124,129,865,140,141,129,121,761,436,124,684,145,686,124,765,137,128,124,129,684,883,686,129,136,137,138,148,455,108,703,684,121,686,128,124,169,116,117,118,119,124,128,796,121,93,129,901,125,128,648,804,121,122,108,482,129,136,137,138,133,856,116,117,118,139,466,673,468,143,470,84,85,86,128,128,121,128,832,124,134,835,265,1054,128,218,684,220,686,222,844,129,228,905,121,133,229,124,960,961,962,121,128,170,124,172,173,243,632,121,907,232,124,234,40,128,183,184,128,252,133,134,128,190,191,192,121,91,92,124,128,264,675,676,141,980,319,140,271,122,273,1003,142,276,211,17,18,19,269,610,87,88,335,5,6,338,339,340,341,342,293,344,431,432,433,108,607,921,108,610,353,777,926,128,357,79,80,783,123,124,930,129,108,109,110,111,112,113,114,22,23,117,864,326,123,378,123,124,884,1053,123,124,128,141,142,131,730,390,123,108,637,884,395,91,92,123,124,116,117,118,884,123,746,123,767,87,88,654,981,128,657,884,115,775,776,759,123,136,137,138,912,784,785,108,787,122,789,122,648,128,108,132,117,1050,437,128,121,943,128,128,121,328,121,128,400,128,938,810,940,123,943,136,137,138,125,408,108,132,135,943,129,129,888,139,119,891,884,123,421,94,943,424,359,127,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,129,984,126,123,436,127,124,132,780,124,864,455,129,507,458,129,510,123,935,124,122,128,466,455,468,129,470,124,780,407,462,108,127,121,943,124,890,128,482,893,894,122,489,490,488,132,30,121,44,128,76,968,969,139,129,972,973,1054,975,553,129,1044,129,124,71,72,73,74,75,108,1054,78,79,80,132,129,124,124,510,1054,124,124,135,40,938,123,135,129,129,521,1054,945,129,129,124,129,123,129,530,108,129,129,122,128,598,483,129,128,123,963,128,122,129,1030,77,121,1033,121,1035,1036,123,122,127,977,126,25,108,135,121,129,984,129,129,124,129,629,93,129,132,129,634,129,1059,128,122,1054,135,123,642,129,121,108,109,121,128,112,113,114,128,115,117,109,129,607,129,122,610,129,129,124,129,128,123,130,131,129,670,129,123,136,28,122,128,132,93,142,633,565,93,123,140,124,25,637,123,121,628,124,123,129,121,57,109,0,648,637,129,129,128,641,654,123,129,657,124,709,129,123,712,713,0,715,666,930,636,129,129,129,116,723,36,264,726,657,269,72,49,703,162,95,96,97,98,99,100,101,102,103,104,698,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,125,409,24,25,26,980,693,229,607,363,780,346,659,424,1050,37,38,39,40,41,42,- 1,- 1,- 1,46,47,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,813,- 1,765,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,829,780,- 1,716,- 1,772,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,727,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,805,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,770,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,839,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,917,856,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,862,- 1,930,865,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,883,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,121,901,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,134,- 1,136,137,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,944,- 1,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1003,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1053,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,63,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,45,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,40,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,93,- 1,- 1,58,59,60,- 1,- 1,- 1,64,65,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,81,82,83,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,25,26,58,59,60,- 1,- 1,- 1,64,65,66,- 1,- 1,69,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,37,38,39,123,93,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,51,- 1,136,137,138,108,109,58,59,112,113,114,- 1,- 1,117,66,- 1,- 1,69,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,81,82,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,40,134,- 1,136,137,138,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,93,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,105,- 1,69,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,81,82,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,37,38,39,- 1,- 1,- 1,- 1,130,131,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,122,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,123,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,123,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,115,116,- 1,118,119,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,130,131,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,- 1,117,- 1,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,115,- 1,- 1,136,119,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,- 1,130,131,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,43,- 1,- 1,46,- 1,48,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,63,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,122,25,26,- 1,- 1,- 1,128,- 1,32,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,42,- 1,142,143,46,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,25,26,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,40,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,40,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,108,109,110,111,112,113,114,67,68,117,70,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,85,86,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4D8[8]="stdcall";static char _tmp4D9[6]="cdecl";static char _tmp4DA[9]="fastcall";static char _tmp4DB[9]="noreturn";static char _tmp4DC[6]="const";static char _tmp4DD[8]="aligned";static char _tmp4DE[7]="packed";static char _tmp4DF[7]="shared";static char _tmp4E0[7]="unused";static char _tmp4E1[5]="weak";static char _tmp4E2[10]="dllimport";static char _tmp4E3[10]="dllexport";static char _tmp4E4[23]="no_instrument_function";static char _tmp4E5[12]="constructor";static char _tmp4E6[11]="destructor";static char _tmp4E7[22]="no_check_memory_usage";static char _tmp4E8[5]="pure";static char _tmp4E9[14]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp424=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp424;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp821;(_tmp821.YYINITIALSVAL).val=0;(_tmp821.YYINITIALSVAL).tag=64;_tmp821;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp81F=200;short*_tmp820=_region_calloc(yyregion,sizeof(short),_tmp81F);_tag_dyneither(_tmp820,sizeof(short),_tmp81F);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp81C=(unsigned int)200;union Cyc_YYSTYPE*_tmp81D=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp81C));{unsigned int _tmp81E=_tmp81C;unsigned int i;for(i=0;i < _tmp81E;i ++){_tmp81D[i]=(union Cyc_YYSTYPE)yylval;}}_tmp81D;}),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp819=(unsigned int)200;struct Cyc_Yyltype*_tmp81A=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp819));{unsigned int _tmp81B=_tmp819;unsigned int i;for(i=0;i < _tmp81B;i ++){_tmp81A[i]=(struct Cyc_Yyltype)Cyc_yynewloc();}}_tmp81A;}),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
Cyc_yyerror(({const char*_tmp425="parser stack overflow";_tag_dyneither(_tmp425,sizeof(char),22);}),yystate,yychar);
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp42E=(unsigned int)yystacksize;short*_tmp42F=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp42E));struct _dyneither_ptr _tmp431=_tag_dyneither(_tmp42F,sizeof(short),_tmp42E);{unsigned int _tmp430=_tmp42E;unsigned int i;for(i=0;i < _tmp430;i ++){_tmp42F[i]=(short)(
i <= yyssp_offset?*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)): 0);}}_tmp431;});
struct _dyneither_ptr yyvs1=({unsigned int _tmp42A=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp42B=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp42A));struct _dyneither_ptr _tmp42D=_tag_dyneither(_tmp42B,sizeof(union Cyc_YYSTYPE),_tmp42A);{unsigned int _tmp42C=_tmp42A;unsigned int i;for(i=0;i < _tmp42C;i ++){_tmp42B[i]=(union Cyc_YYSTYPE)(
# 219
i <= yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)): yylval);}}_tmp42D;});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp426=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp427=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp426));struct _dyneither_ptr _tmp429=_tag_dyneither(_tmp427,sizeof(struct Cyc_Yyltype),_tmp426);{unsigned int _tmp428=_tmp426;unsigned int i;for(i=0;i < _tmp428;i ++){_tmp427[i]=(struct Cyc_Yyltype)(
i <= yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):
 Cyc_yynewloc());}}_tmp429;});
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
{int _tmp433=yyn;switch(_tmp433){case 1: _LL261: _LL262: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1219 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1222
goto _LL260;}case 2: _LL263: _LL264: {
# 1224
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1225 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp434;_push_handler(& _tmp434);{int _tmp436=0;if(setjmp(_tmp434.handler))_tmp436=1;if(!_tmp436){
x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp435=(void*)_exn_thrown;void*_tmp437=_tmp435;void*_tmp438;if(((struct Cyc_Core_Failure_exn_struct*)_tmp437)->tag == Cyc_Core_Failure){_LL682: _LL683:
# 1230
 x=0;goto _LL681;}else{_LL684: _tmp438=_tmp437;_LL685:(int)_rethrow(_tmp438);}_LL681:;}};}
# 1232
{struct _handler_cons _tmp439;_push_handler(& _tmp439);{int _tmp43B=0;if(setjmp(_tmp439.handler))_tmp43B=1;if(!_tmp43B){
y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp43A=(void*)_exn_thrown;void*_tmp43C=_tmp43A;void*_tmp43D;if(((struct Cyc_Core_Failure_exn_struct*)_tmp43C)->tag == Cyc_Core_Failure){_LL687: _LL688:
# 1236
 y=0;goto _LL686;}else{_LL689: _tmp43D=_tmp43C;_LL68A:(int)_rethrow(_tmp43D);}_LL686:;}};}
# 1238
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1240
goto _LL260;}case 3: _LL265: _LL266: {
# 1242
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1244
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1243 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->hd=({struct Cyc_Absyn_Decl*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->r=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440[0]=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp441;_tmp441.tag=10;_tmp441.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp441.f2=Cyc_yyget_YY16(yyyyvsp[2]);_tmp441;});_tmp440;});_tmp43F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp43F;});_tmp43E->tl=0;_tmp43E;}));
Cyc_Lex_leave_using();
# 1246
goto _LL260;}case 4: _LL267: _LL268: {
# 1248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1247 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->hd=({struct Cyc_Absyn_Decl*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->r=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp445;_tmp445.tag=10;_tmp445.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp445.f2=Cyc_yyget_YY16(yyyyvsp[2]);_tmp445;});_tmp444;});_tmp443->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp443;});_tmp442->tl=Cyc_yyget_YY16(yyyyvsp[4]);_tmp442;}));
goto _LL260;}case 5: _LL269: _LL26A: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446->hd=({struct Cyc_Absyn_Decl*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->r=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448[0]=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp449;_tmp449.tag=9;_tmp449.f1=({struct _dyneither_ptr*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp44A;});_tmp449.f2=Cyc_yyget_YY16(yyyyvsp[2]);_tmp449;});_tmp448;});_tmp447->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp447;});_tmp446->tl=0;_tmp446;}));
Cyc_Lex_leave_namespace();
# 1253
goto _LL260;}case 6: _LL26B: _LL26C: {
# 1255
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1255 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp44B;_push_handler(& _tmp44B);{int _tmp44D=0;if(setjmp(_tmp44B.handler))_tmp44D=1;if(!_tmp44D){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp44C=(void*)_exn_thrown;void*_tmp44E=_tmp44C;void*_tmp44F;if(((struct Cyc_Core_Failure_exn_struct*)_tmp44E)->tag == Cyc_Core_Failure){_LL68C: _LL68D:
 nspace=({const char*_tmp450="";_tag_dyneither(_tmp450,sizeof(char),1);});goto _LL68B;}else{_LL68E: _tmp44F=_tmp44E;_LL68F:(int)_rethrow(_tmp44F);}_LL68B:;}};}
# 1260
{struct _handler_cons _tmp451;_push_handler(& _tmp451);{int _tmp453=0;if(setjmp(_tmp451.handler))_tmp453=1;if(!_tmp453){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp452=(void*)_exn_thrown;void*_tmp454=_tmp452;void*_tmp455;if(((struct Cyc_Core_Failure_exn_struct*)_tmp454)->tag == Cyc_Core_Failure){_LL691: _LL692:
 x=0;goto _LL690;}else{_LL693: _tmp455=_tmp454;_LL694:(int)_rethrow(_tmp455);}_LL690:;}};}
# 1263
{struct _handler_cons _tmp456;_push_handler(& _tmp456);{int _tmp458=0;if(setjmp(_tmp456.handler))_tmp458=1;if(!_tmp458){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp457=(void*)_exn_thrown;void*_tmp459=_tmp457;void*_tmp45A;if(((struct Cyc_Core_Failure_exn_struct*)_tmp459)->tag == Cyc_Core_Failure){_LL696: _LL697:
 y=0;goto _LL695;}else{_LL698: _tmp45A=_tmp459;_LL699:(int)_rethrow(_tmp45A);}_LL695:;}};}
# 1266
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->hd=({struct Cyc_Absyn_Decl*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->r=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp45E;_tmp45E.tag=9;_tmp45E.f1=({struct _dyneither_ptr*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F[0]=nspace;_tmp45F;});_tmp45E.f2=x;_tmp45E;});_tmp45D;});_tmp45C->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp45C;});_tmp45B->tl=y;_tmp45B;}));
# 1268
goto _LL260;}case 7: _LL26D: _LL26E: {
# 1270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1269 "parse.y"
int _tmp460=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp460)
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->hd=({struct Cyc_Absyn_Decl*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->r=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463[0]=({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp464;_tmp464.tag=11;_tmp464.f1=Cyc_yyget_YY16(yyyyvsp[2]);_tmp464;});_tmp463;});_tmp462->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp462;});_tmp461->tl=Cyc_yyget_YY16(yyyyvsp[4]);_tmp461;}));else{
# 1273
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->hd=({struct Cyc_Absyn_Decl*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp468;_tmp468.tag=12;_tmp468.f1=Cyc_yyget_YY16(yyyyvsp[2]);_tmp468.f2=0;_tmp468;});_tmp467;});_tmp466->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp466;});_tmp465->tl=Cyc_yyget_YY16(yyyyvsp[4]);_tmp465;}));}
# 1275
goto _LL260;}case 8: _LL26F: _LL270: {
# 1277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1276 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
Cyc_Parse_err(({const char*_tmp469="expecting \"C include\"";_tag_dyneither(_tmp469,sizeof(char),22);}),(unsigned int)(yyyylsp[0]).first_line);{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->hd=({struct Cyc_Absyn_Decl*_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C[0]=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp46D;_tmp46D.tag=12;_tmp46D.f1=Cyc_yyget_YY16(yyyyvsp[2]);_tmp46D.f2=exs;_tmp46D;});_tmp46C;});_tmp46B->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp46B;});_tmp46A->tl=Cyc_yyget_YY16(yyyyvsp[5]);_tmp46A;}));
# 1281
goto _LL260;};}case 9: _LL271: _LL272: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1282 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->hd=({struct Cyc_Absyn_Decl*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->r=(void*)& Cyc_Absyn_Porton_d_val;_tmp46F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp46F;});_tmp46E->tl=Cyc_yyget_YY16(yyyyvsp[2]);_tmp46E;}));
goto _LL260;}case 10: _LL273: _LL274: {
# 1285
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1287
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1284 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->hd=({struct Cyc_Absyn_Decl*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->r=(void*)& Cyc_Absyn_Portoff_d_val;_tmp471->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp471;});_tmp470->tl=Cyc_yyget_YY16(yyyyvsp[2]);_tmp470;}));
goto _LL260;}case 11: _LL275: _LL276:
# 1287
 yyval=Cyc_YY16(0);
goto _LL260;case 12: _LL277: _LL278: {
# 1290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1290 "parse.y"
struct _dyneither_ptr _tmp472=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(Cyc_strcmp((struct _dyneither_ptr)_tmp472,({const char*_tmp473="C";_tag_dyneither(_tmp473,sizeof(char),2);}))== 0)
yyval=Cyc_YY31(0);else{
if(Cyc_strcmp((struct _dyneither_ptr)_tmp472,({const char*_tmp474="C include";_tag_dyneither(_tmp474,sizeof(char),10);}))== 0)
yyval=Cyc_YY31(1);else{
# 1297
Cyc_Parse_err(({const char*_tmp475="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp475,sizeof(char),29);}),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY31(1);}}
# 1301
goto _LL260;}case 13: _LL279: _LL27A: {
# 1303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1304 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL260;}case 14: _LL27B: _LL27C: {
# 1307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1308 "parse.y"
yyval=yyyyvsp[2];
goto _LL260;}case 15: _LL27D: _LL27E: {
# 1311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1309 "parse.y"
yyval=Cyc_YY52(0);
goto _LL260;}case 16: _LL27F: _LL280: {
# 1312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1313 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->hd=({struct _tuple27*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->f1=(unsigned int)(yyyylsp[0]).first_line;_tmp477->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp477->f3=0;_tmp477;});_tmp476->tl=0;_tmp476;}));
goto _LL260;}case 17: _LL281: _LL282: {
# 1316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1314 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->hd=({struct _tuple27*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->f1=(unsigned int)(yyyylsp[0]).first_line;_tmp479->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp479->f3=0;_tmp479;});_tmp478->tl=0;_tmp478;}));
goto _LL260;}case 18: _LL283: _LL284: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1316 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->hd=({struct _tuple27*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->f1=(unsigned int)(yyyylsp[0]).first_line;_tmp47B->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47B->f3=0;_tmp47B;});_tmp47A->tl=Cyc_yyget_YY52(yyyyvsp[2]);_tmp47A;}));
goto _LL260;}case 19: _LL285: _LL286: {
# 1319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1320 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D[0]=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp47E;_tmp47E.tag=1;_tmp47E.f1=Cyc_yyget_YY15(yyyyvsp[0]);_tmp47E;});_tmp47D;}),(unsigned int)(yyyylsp[0]).first_line);_tmp47C->tl=0;_tmp47C;}));
goto _LL260;}case 20: _LL287: _LL288: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1321 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 21: _LL289: _LL28A: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1322 "parse.y"
yyval=Cyc_YY16(0);
goto _LL260;}case 24: _LL28B: _LL28C: {
# 1325
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1331 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),0,Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 25: _LL28D: _LL28E: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1333 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp47F=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp47F,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 26: _LL28F: _LL290: {
# 1337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1346 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1]),Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 27: _LL291: _LL292: {
# 1349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1348 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp480=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp480,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 28: _LL293: _LL294: {
# 1352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1356 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp481=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp481,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 29: _LL295: _LL296: {
# 1360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1359 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp482=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp482,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 30: _LL297: _LL298: {
# 1363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1364 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL260;}case 31: _LL299: _LL29A: {
# 1367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1367 "parse.y"
Cyc_Lex_leave_using();
goto _LL260;}case 32: _LL29B: _LL29C: {
# 1370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1370 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp483;}));yyval=yyyyvsp[1];
goto _LL260;}case 33: _LL29D: _LL29E: {
# 1373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1373 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL260;}case 34: _LL29F: _LL2A0: {
# 1376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp484=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),0,(unsigned int)_tmp484,(unsigned int)_tmp484));
goto _LL260;}case 35: _LL2A1: _LL2A2: {
# 1383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1382 "parse.y"
int _tmp485=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp485));
goto _LL260;}case 36: _LL2A3: _LL2A4: {
# 1386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1386 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line);_tmp486->tl=0;_tmp486;}));
goto _LL260;}case 37: _LL2A5: _LL2A6: {
# 1389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1388 "parse.y"
struct Cyc_List_List*_tmp487=0;
{struct Cyc_List_List*_tmp488=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp488 != 0;_tmp488=_tmp488->tl){
struct _dyneither_ptr*_tmp489=(struct _dyneither_ptr*)_tmp488->hd;
struct _tuple0*qv=({struct _tuple0*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->f1=Cyc_Absyn_Rel_n(0);_tmp48C->f2=_tmp489;_tmp48C;});
struct Cyc_Absyn_Vardecl*_tmp48A=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
_tmp487=({struct Cyc_List_List*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->hd=_tmp48A;_tmp48B->tl=_tmp487;_tmp48B;});}}
# 1395
_tmp487=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp487);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->hd=Cyc_Absyn_letv_decl(_tmp487,(unsigned int)(yyyylsp[0]).first_line);_tmp48D->tl=0;_tmp48D;}));
# 1398
goto _LL260;}case 38: _LL2A7: _LL2A8: {
# 1400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1401 "parse.y"
struct _dyneither_ptr _tmp48E=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1403
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp48E,({const char*_tmp48F="`H";_tag_dyneither(_tmp48F,sizeof(char),3);}))== 0)
Cyc_Parse_err((struct _dyneither_ptr)({void*_tmp490=0;Cyc_aprintf(({const char*_tmp491="bad occurrence of heap region";_tag_dyneither(_tmp491,sizeof(char),30);}),_tag_dyneither(_tmp490,sizeof(void*),0));}),(unsigned int)(yyyylsp[3]).first_line);
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp48E,({const char*_tmp492="`U";_tag_dyneither(_tmp492,sizeof(char),3);}))== 0)
Cyc_Parse_err((struct _dyneither_ptr)({void*_tmp493=0;Cyc_aprintf(({const char*_tmp494="bad occurrence of unique region";_tag_dyneither(_tmp494,sizeof(char),32);}),_tag_dyneither(_tmp493,sizeof(void*),0));}),(unsigned int)(yyyylsp[3]).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C->name=({struct _dyneither_ptr*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D[0]=_tmp48E;_tmp49D;});_tmp49C->identity=- 1;_tmp49C->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp49C;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp49B;_tmp49B.tag=2;_tmp49B.f1=tv;_tmp49B;});_tmp49A;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->f1=Cyc_Absyn_Loc_n;_tmp496->f2=({struct _dyneither_ptr*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497[0]=Cyc_yyget_String_tok(yyyyvsp[5]);_tmp497;});_tmp496;}),(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp499;_tmp499.tag=15;_tmp499.f1=t;_tmp499;});_tmp498;}),0);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line);_tmp495->tl=0;_tmp495;}));
# 1412
goto _LL260;};}case 39: _LL2A9: _LL2AA: {
# 1414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1414 "parse.y"
int _tmp49E=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp49F=Cyc_yyget_String_tok(yyyyvsp[2]);
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49F,({const char*_tmp4A0="H";_tag_dyneither(_tmp4A0,sizeof(char),2);}))== 0)
Cyc_Parse_err(({const char*_tmp4A1="bad occurrence of heap region `H";_tag_dyneither(_tmp4A1,sizeof(char),33);}),(unsigned int)(yyyylsp[2]).first_line);
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49F,({const char*_tmp4A2="U";_tag_dyneither(_tmp4A2,sizeof(char),2);}))== 0)
Cyc_Parse_err(({const char*_tmp4A3="bad occurrence of unique region `U";_tag_dyneither(_tmp4A3,sizeof(char),35);}),(unsigned int)(yyyylsp[2]).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->name=({struct _dyneither_ptr*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4AF;_tmp4AF.tag=0;_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp49F);({void*_tmp4AD[1]={& _tmp4AF};Cyc_aprintf(({const char*_tmp4AE="`%s";_tag_dyneither(_tmp4AE,sizeof(char),4);}),_tag_dyneither(_tmp4AD,sizeof(void*),1));});});_tmp4AC;});_tmp4AB->identity=- 1;_tmp4AB->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4AB;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4AA;_tmp4AA.tag=2;_tmp4AA.f1=tv;_tmp4AA;});_tmp4A9;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5->f1=Cyc_Absyn_Loc_n;_tmp4A5->f2=({struct _dyneither_ptr*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6[0]=_tmp49F;_tmp4A6;});_tmp4A5;}),(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4A8;_tmp4A8.tag=15;_tmp4A8.f1=t;_tmp4A8;});_tmp4A7;}),0);
# 1425
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->hd=Cyc_Absyn_region_decl(tv,vd,_tmp49E,0,(unsigned int)(yyyylsp[0]).first_line);_tmp4A4->tl=0;_tmp4A4;}));
# 1427
goto _LL260;};}case 40: _LL2AB: _LL2AC: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1429 "parse.y"
struct _dyneither_ptr _tmp4B0=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4B1=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_yyget_Exp_tok(yyyyvsp[6]);
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B1,({const char*_tmp4B3="open";_tag_dyneither(_tmp4B3,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp4B4="expecting `open'";_tag_dyneither(_tmp4B4,sizeof(char),17);}),(unsigned int)(yyyylsp[3]).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->name=({struct _dyneither_ptr*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4C0;_tmp4C0.tag=0;_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4B0);({void*_tmp4BE[1]={& _tmp4C0};Cyc_aprintf(({const char*_tmp4BF="`%s";_tag_dyneither(_tmp4BF,sizeof(char),4);}),_tag_dyneither(_tmp4BE,sizeof(void*),1));});});_tmp4BD;});_tmp4BC->identity=- 1;_tmp4BC->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4BC;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4BB;_tmp4BB.tag=2;_tmp4BB.f1=tv;_tmp4BB;});_tmp4BA;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->f1=Cyc_Absyn_Loc_n;_tmp4B6->f2=({struct _dyneither_ptr*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=_tmp4B0;_tmp4B7;});_tmp4B6;}),(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4B9;_tmp4B9.tag=15;_tmp4B9.f1=t;_tmp4B9;});_tmp4B8;}),0);
# 1438
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4B2,(unsigned int)(yyyylsp[0]).first_line);_tmp4B5->tl=0;_tmp4B5;}));
# 1440
goto _LL260;};}case 41: _LL2AD: _LL2AE:
# 1442
 yyval=Cyc_YY31(0);
goto _LL260;case 42: _LL2AF: _LL2B0: {
# 1445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1444 "parse.y"
yyval=Cyc_YY31(1);
goto _LL260;}case 43: _LL2B1: _LL2B2: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 44: _LL2B3: _LL2B4: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1451 "parse.y"
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1])));
goto _LL260;}case 45: _LL2B5: _LL2B6: {
# 1454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C1;_tmp4C1.sc=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4C1.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4C1.type_specs=
Cyc_Parse_empty_spec(0);_tmp4C1.is_inline=0;_tmp4C1.attributes=0;_tmp4C1;}));
goto _LL260;}case 46: _LL2B7: _LL2B8: {
# 1461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1460 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C2=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp4C2.sc != 0)
({void*_tmp4C3=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp4C4="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp4C4,sizeof(char),73);}),_tag_dyneither(_tmp4C3,sizeof(void*),0));});
# 1464
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C5;_tmp4C5.sc=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4C5.tq=_tmp4C2.tq;_tmp4C5.type_specs=_tmp4C2.type_specs;_tmp4C5.is_inline=_tmp4C2.is_inline;_tmp4C5.attributes=_tmp4C2.attributes;_tmp4C5;}));
# 1468
goto _LL260;}case 47: _LL2B9: _LL2BA: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1469 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C6;_tmp4C6.sc=0;_tmp4C6.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4C6.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]);_tmp4C6.is_inline=0;_tmp4C6.attributes=0;_tmp4C6;}));
goto _LL260;}case 48: _LL2BB: _LL2BC: {
# 1473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1472 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C7=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C8;_tmp4C8.sc=_tmp4C7.sc;_tmp4C8.tq=_tmp4C7.tq;_tmp4C8.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4C7.type_specs,Cyc_yyget_YY21(yyyyvsp[0]));_tmp4C8.is_inline=_tmp4C7.is_inline;_tmp4C8.attributes=_tmp4C7.attributes;_tmp4C8;}));
# 1478
goto _LL260;}case 49: _LL2BD: _LL2BE: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1479 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C9;_tmp4C9.sc=0;_tmp4C9.tq=Cyc_yyget_YY23(yyyyvsp[0]);_tmp4C9.type_specs=Cyc_Parse_empty_spec(0);_tmp4C9.is_inline=0;_tmp4C9.attributes=0;_tmp4C9;}));
goto _LL260;}case 50: _LL2BF: _LL2C0: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1481 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CA=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CB;_tmp4CB.sc=_tmp4CA.sc;_tmp4CB.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp4CA.tq);_tmp4CB.type_specs=_tmp4CA.type_specs;_tmp4CB.is_inline=_tmp4CA.is_inline;_tmp4CB.attributes=_tmp4CA.attributes;_tmp4CB;}));
# 1486
goto _LL260;}case 51: _LL2C1: _LL2C2: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1487 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CC;_tmp4CC.sc=0;_tmp4CC.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4CC.type_specs=
Cyc_Parse_empty_spec(0);_tmp4CC.is_inline=1;_tmp4CC.attributes=0;_tmp4CC;}));
goto _LL260;}case 52: _LL2C3: _LL2C4: {
# 1491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1490 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CD=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CE;_tmp4CE.sc=_tmp4CD.sc;_tmp4CE.tq=_tmp4CD.tq;_tmp4CE.type_specs=_tmp4CD.type_specs;_tmp4CE.is_inline=1;_tmp4CE.attributes=_tmp4CD.attributes;_tmp4CE;}));
# 1494
goto _LL260;}case 53: _LL2C5: _LL2C6: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1495 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CF;_tmp4CF.sc=0;_tmp4CF.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4CF.type_specs=
Cyc_Parse_empty_spec(0);_tmp4CF.is_inline=0;_tmp4CF.attributes=Cyc_yyget_YY45(yyyyvsp[0]);_tmp4CF;}));
goto _LL260;}case 54: _LL2C7: _LL2C8: {
# 1499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1498 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D0=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D1;_tmp4D1.sc=_tmp4D0.sc;_tmp4D1.tq=_tmp4D0.tq;_tmp4D1.type_specs=_tmp4D0.type_specs;_tmp4D1.is_inline=_tmp4D0.is_inline;_tmp4D1.attributes=
# 1501
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp4D0.attributes);_tmp4D1;}));
goto _LL260;}case 55: _LL2C9: _LL2CA: {
# 1504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1505 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL260;}case 56: _LL2CB: _LL2CC: {
# 1509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1507 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL260;}case 57: _LL2CD: _LL2CE: {
# 1511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1509 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL260;}case 58: _LL2CF: _LL2D0: {
# 1513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1511 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL260;}case 59: _LL2D1: _LL2D2: {
# 1515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1514 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),({const char*_tmp4D2="C";_tag_dyneither(_tmp4D2,sizeof(char),2);}))!= 0)
Cyc_Parse_err(({const char*_tmp4D3="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp4D3,sizeof(char),37);}),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY20(& externC_sc);
# 1519
goto _LL260;}case 60: _LL2D3: _LL2D4: {
# 1521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1519 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL260;}case 61: _LL2D5: _LL2D6: {
# 1523
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1522 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL260;}case 62: _LL2D7: _LL2D8:
# 1526
 yyval=Cyc_YY45(0);
goto _LL260;case 63: _LL2D9: _LL2DA: {
# 1529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1529 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 64: _LL2DB: _LL2DC: {
# 1532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1534 "parse.y"
yyval=yyyyvsp[3];
goto _LL260;}case 65: _LL2DD: _LL2DE: {
# 1537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1538 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4->hd=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4D4->tl=0;_tmp4D4;}));
goto _LL260;}case 66: _LL2DF: _LL2E0: {
# 1541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1539 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5->hd=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4D5->tl=Cyc_yyget_YY45(yyyyvsp[2]);_tmp4D5;}));
goto _LL260;}case 67: _LL2E1: _LL2E2: {
# 1542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1544 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple28 att_map[18]={{{_tmp4D8,_tmp4D8,_tmp4D8 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4D9,_tmp4D9,_tmp4D9 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4DA,_tmp4DA,_tmp4DA + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4DB,_tmp4DB,_tmp4DB + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4DC,_tmp4DC,_tmp4DC + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4DD,_tmp4DD,_tmp4DD + 8},(void*)& att_aligned},{{_tmp4DE,_tmp4DE,_tmp4DE + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4DF,_tmp4DF,_tmp4DF + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E0,_tmp4E0,_tmp4E0 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4E1,_tmp4E1,_tmp4E1 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4E7,_tmp4E7,_tmp4E7 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 5},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 14},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1565
struct _dyneither_ptr _tmp4D6=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1567
if((((_get_dyneither_size(_tmp4D6,sizeof(char))> 4  && ((const char*)_tmp4D6.curr)[0]== '_') && ((const char*)_tmp4D6.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),(int)(_get_dyneither_size(_tmp4D6,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),(int)(_get_dyneither_size(_tmp4D6,sizeof(char))- 3)))== '_')
# 1569
_tmp4D6=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4D6,2,_get_dyneither_size(_tmp4D6,sizeof(char))- 5);{
int i=0;
for(0;i < 18;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4D6,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(18,i)]).f2);
break;}}
# 1576
if(i == 18){
Cyc_Parse_err(({const char*_tmp4D7="unrecognized attribute";_tag_dyneither(_tmp4D7,sizeof(char),23);}),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1581
goto _LL260;};}case 68: _LL2E3: _LL2E4: {
# 1583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1581 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL260;}case 69: _LL2E5: _LL2E6: {
# 1584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1583 "parse.y"
struct _dyneither_ptr _tmp4EB=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp4EC=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4ED="aligned";_tag_dyneither(_tmp4ED,sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4EE="__aligned__";_tag_dyneither(_tmp4EE,sizeof(char),12);}))== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp4F0;_tmp4F0.tag=6;_tmp4F0.f1=_tmp4EC;_tmp4F0;});_tmp4EF;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4F1="section";_tag_dyneither(_tmp4F1,sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4F2="__section__";_tag_dyneither(_tmp4F2,sizeof(char),12);}))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmp4F4;_tmp4F4.tag=8;_tmp4F4.f1=str;_tmp4F4;});_tmp4F3;});}else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4F5="__mode__";_tag_dyneither(_tmp4F5,sizeof(char),9);}))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmp4F7;_tmp4F7.tag=24;_tmp4F7.f1=str;_tmp4F7;});_tmp4F6;});}else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4F8="alias";_tag_dyneither(_tmp4F8,sizeof(char),6);}))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9[0]=({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmp4FA;_tmp4FA.tag=25;_tmp4FA.f1=str;_tmp4FA;});_tmp4F9;});}else{
# 1599
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp4EC);
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4FB="regparm";_tag_dyneither(_tmp4FB,sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp4FC="__regparm__";_tag_dyneither(_tmp4FC,sizeof(char),12);}))== 0){
if(n < 0  || n > 3)
Cyc_Parse_err(({const char*_tmp4FD="regparm requires value between 0 and 3";_tag_dyneither(_tmp4FD,sizeof(char),39);}),(unsigned int)(yyyylsp[2]).first_line);
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp4FE=_cycalloc_atomic(sizeof(*_tmp4FE));_tmp4FE[0]=({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmp4FF;_tmp4FF.tag=0;_tmp4FF.f1=n;_tmp4FF;});_tmp4FE;});}else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp500="initializes";_tag_dyneither(_tmp500,sizeof(char),12);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp501="__initializes__";_tag_dyneither(_tmp501,sizeof(char),16);}))== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp502=_cycalloc_atomic(sizeof(*_tmp502));_tmp502[0]=({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmp503;_tmp503.tag=20;_tmp503.f1=n;_tmp503;});_tmp502;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp504="noliveunique";_tag_dyneither(_tmp504,sizeof(char),13);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp505="__noliveunique__";_tag_dyneither(_tmp505,sizeof(char),17);}))== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp506=_cycalloc_atomic(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmp507;_tmp507.tag=21;_tmp507.f1=n;_tmp507;});_tmp506;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp508="noconsume";_tag_dyneither(_tmp508,sizeof(char),10);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EB,({const char*_tmp509="__noconsume__";_tag_dyneither(_tmp509,sizeof(char),14);}))== 0)
a=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp50A=_cycalloc_atomic(sizeof(*_tmp50A));_tmp50A[0]=({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmp50B;_tmp50B.tag=22;_tmp50B.f1=n;_tmp50B;});_tmp50A;});else{
# 1611
Cyc_Parse_err(({const char*_tmp50C="unrecognized attribute";_tag_dyneither(_tmp50C,sizeof(char),23);}),(unsigned int)(yyyylsp[0]).first_line);
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1615
yyval=Cyc_YY46(a);
# 1617
goto _LL260;}case 70: _LL2E7: _LL2E8: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1618 "parse.y"
struct _dyneither_ptr _tmp50D=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp50E=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp50F=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_Int_tok(yyyyvsp[4]));
unsigned int _tmp510=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[6]).first_line,Cyc_yyget_Int_tok(yyyyvsp[6]));
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50D,({const char*_tmp511="format";_tag_dyneither(_tmp511,sizeof(char),7);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50D,({const char*_tmp512="__format__";_tag_dyneither(_tmp512,sizeof(char),11);}))== 0){
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,({const char*_tmp513="printf";_tag_dyneither(_tmp513,sizeof(char),7);}))== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp514=_cycalloc_atomic(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp515;_tmp515.tag=19;_tmp515.f1=Cyc_Absyn_Printf_ft;_tmp515.f2=(int)_tmp50F;_tmp515.f3=(int)_tmp510;_tmp515;});_tmp514;});else{
if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,({const char*_tmp516="scanf";_tag_dyneither(_tmp516,sizeof(char),6);}))== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp517=_cycalloc_atomic(sizeof(*_tmp517));_tmp517[0]=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp518;_tmp518.tag=19;_tmp518.f1=Cyc_Absyn_Scanf_ft;_tmp518.f2=(int)_tmp50F;_tmp518.f3=(int)_tmp510;_tmp518;});_tmp517;});else{
# 1629
Cyc_Parse_err(({const char*_tmp519="unrecognized format type";_tag_dyneither(_tmp519,sizeof(char),25);}),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1631
Cyc_Parse_err(({const char*_tmp51A="unrecognized attribute";_tag_dyneither(_tmp51A,sizeof(char),23);}),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46(a);
# 1634
goto _LL260;}case 71: _LL2E9: _LL2EA: {
# 1636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1643 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 72: _LL2EB: _LL2EC: {
# 1646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1645 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp51C;_tmp51C.tag=17;_tmp51C.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp51C.f2=Cyc_yyget_YY40(yyyyvsp[1]);_tmp51C.f3=0;_tmp51C.f4=0;_tmp51C;});_tmp51B;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 73: _LL2ED: _LL2EE: {
# 1648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1649 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 74: _LL2EF: _LL2F0: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 75: _LL2F1: _LL2F2: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 76: _LL2F3: _LL2F4: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1652 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 77: _LL2F5: _LL2F6: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1653 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 78: _LL2F7: _LL2F8: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 79: _LL2F9: _LL2FA: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 80: _LL2FB: _LL2FC: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 81: _LL2FD: _LL2FE: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 82: _LL2FF: _LL300: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1658 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 83: _LL301: _LL302: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1659 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 84: _LL303: _LL304: {
# 1662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1662 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp51E;_tmp51E.tag=27;_tmp51E.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp51E;});_tmp51D;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 85: _LL305: _LL306: {
# 1665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1664 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F[0]=({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmp520;_tmp520.tag=28;_tmp520.f1=({const char*_tmp521="__builtin_va_list";_tag_dyneither(_tmp521,sizeof(char),18);});_tmp520.f2=& Cyc_Tcutil_bk;_tmp520;});_tmp51F;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 86: _LL307: _LL308: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1666 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 87: _LL309: _LL30A: {
# 1669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1668 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 88: _LL30B: _LL30C: {
# 1671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1670 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 89: _LL30D: _LL30E: {
# 1673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1672 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 90: _LL30F: _LL310: {
# 1675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1674 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp523;_tmp523.tag=10;_tmp523.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));_tmp523;});_tmp522;}),(unsigned int)(yyyylsp[0]).first_line));
# 1677
goto _LL260;}case 91: _LL311: _LL312: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1678 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp525;_tmp525.tag=15;_tmp525.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp525;});_tmp524;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 92: _LL313: _LL314: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1680 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp527;_tmp527.tag=15;_tmp527.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);_tmp527;});_tmp526;}),(unsigned int)(yyyylsp[0]).first_line));
# 1682
goto _LL260;}case 93: _LL315: _LL316: {
# 1684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1683 "parse.y"
void*_tmp528=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp52A;_tmp52A.tag=16;_tmp52A.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp52A.f2=_tmp528;_tmp52A;});_tmp529;}),(unsigned int)(yyyylsp[0]).first_line));
# 1686
goto _LL260;}case 94: _LL317: _LL318: {
# 1688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1687 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp52C;_tmp52C.tag=16;_tmp52C.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp52C.f2=Cyc_yyget_YY44(yyyyvsp[4]);_tmp52C;});_tmp52B;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 95: _LL319: _LL31A: {
# 1690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1689 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp52E;_tmp52E.tag=19;_tmp52E.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp52E;});_tmp52D;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 96: _LL31B: _LL31C: {
# 1692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1691 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp530;_tmp530.tag=19;_tmp530.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0);_tmp530;});_tmp52F;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 97: _LL31D: _LL31E: {
# 1694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1693 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp532;_tmp532.tag=18;_tmp532.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp532;});_tmp531;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 98: _LL31F: _LL320: {
# 1696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 99: _LL321: _LL322: {
# 1702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1703 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp533;_tmp533.print_const=1;_tmp533.q_volatile=0;_tmp533.q_restrict=0;_tmp533.real_const=1;_tmp533.loc=loc;_tmp533;}));
goto _LL260;}case 100: _LL323: _LL324: {
# 1707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1705 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp534;_tmp534.print_const=0;_tmp534.q_volatile=1;_tmp534.q_restrict=0;_tmp534.real_const=0;_tmp534.loc=0;_tmp534;}));
goto _LL260;}case 101: _LL325: _LL326: {
# 1708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1706 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp535;_tmp535.print_const=0;_tmp535.q_volatile=0;_tmp535.q_restrict=1;_tmp535.real_const=0;_tmp535.loc=0;_tmp535;}));
goto _LL260;}case 102: _LL327: _LL328: {
# 1709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_TypeDecl*_tmp536=({struct Cyc_Absyn_TypeDecl*_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539->r=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmp53B;_tmp53B.tag=1;_tmp53B.f1=({struct Cyc_Absyn_Enumdecl*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->sc=Cyc_Absyn_Public;_tmp53C->name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp53C->fields=({struct Cyc_Core_Opt*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->v=Cyc_yyget_YY48(yyyyvsp[3]);_tmp53D;});_tmp53C;});_tmp53B;});_tmp53A;});_tmp539->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp539;});
# 1714
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp538;_tmp538.tag=26;_tmp538.f1=_tmp536;_tmp538.f2=0;_tmp538;});_tmp537;}),(unsigned int)(yyyylsp[0]).first_line));
# 1716
goto _LL260;}case 103: _LL329: _LL32A: {
# 1718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1717 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp53E=_cycalloc(sizeof(*_tmp53E));_tmp53E[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp53F;_tmp53F.tag=13;_tmp53F.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp53F.f2=0;_tmp53F;});_tmp53E;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 104: _LL32B: _LL32C: {
# 1720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1719 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp541;_tmp541.tag=14;_tmp541.f1=Cyc_yyget_YY48(yyyyvsp[2]);_tmp541;});_tmp540;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 105: _LL32D: _LL32E: {
# 1722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->name=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp542->tag=0;_tmp542->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp542;}));
goto _LL260;}case 106: _LL32F: _LL330: {
# 1728
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1730
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1727 "parse.y"
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->name=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp543->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp543->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp543;}));
goto _LL260;}case 107: _LL331: _LL332: {
# 1730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1731 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544->hd=Cyc_yyget_YY47(yyyyvsp[0]);_tmp544->tl=0;_tmp544;}));
goto _LL260;}case 108: _LL333: _LL334: {
# 1734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1732 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->hd=Cyc_yyget_YY47(yyyyvsp[0]);_tmp545->tl=0;_tmp545;}));
goto _LL260;}case 109: _LL335: _LL336: {
# 1735
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1733 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->hd=Cyc_yyget_YY47(yyyyvsp[0]);_tmp546->tl=Cyc_yyget_YY48(yyyyvsp[2]);_tmp546;}));
goto _LL260;}case 110: _LL337: _LL338: {
# 1736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp548;_tmp548.tag=12;_tmp548.f1=Cyc_yyget_YY22(yyyyvsp[0]);_tmp548.f2=Cyc_yyget_YY24(yyyyvsp[2]);_tmp548;});_tmp547;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 111: _LL339: _LL33A: {
# 1742
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1745 "parse.y"
struct Cyc_List_List*_tmp549=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp54A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp54B=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp549,
Cyc_Absyn_aggrdecl_impl(_tmp54A,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY24(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1750
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp54D;_tmp54D.tag=26;_tmp54D.f1=_tmp54B;_tmp54D.f2=0;_tmp54D;});_tmp54C;}),(unsigned int)(yyyylsp[0]).first_line));
# 1752
goto _LL260;}case 112: _LL33B: _LL33C: {
# 1754
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1756 "parse.y"
struct Cyc_List_List*_tmp54E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp54F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp550=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp54E,
Cyc_Absyn_aggrdecl_impl(_tmp54F,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY24(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1761
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp552;_tmp552.tag=26;_tmp552.f1=_tmp550;_tmp552.f2=0;_tmp552;});_tmp551;}),(unsigned int)(yyyylsp[0]).first_line));
# 1763
goto _LL260;}case 113: _LL33D: _LL33E: {
# 1765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1764 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp554;_tmp554.tag=11;_tmp554.f1=({struct Cyc_Absyn_AggrInfo _tmp555;_tmp555.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),({struct Cyc_Core_Opt*_tmp556=_cycalloc_atomic(sizeof(*_tmp556));_tmp556->v=(void*)1;_tmp556;}));_tmp555.targs=Cyc_yyget_YY40(yyyyvsp[3]);_tmp555;});_tmp554;});_tmp553;}),(unsigned int)(yyyylsp[0]).first_line));
# 1767
goto _LL260;}case 114: _LL33F: _LL340: {
# 1769
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1768 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp558;_tmp558.tag=11;_tmp558.f1=({struct Cyc_Absyn_AggrInfo _tmp559;_tmp559.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0);_tmp559.targs=Cyc_yyget_YY40(yyyyvsp[2]);_tmp559;});_tmp558;});_tmp557;}),(unsigned int)(yyyylsp[0]).first_line));
# 1771
goto _LL260;}case 115: _LL341: _LL342:
# 1773
 yyval=Cyc_YY40(0);
goto _LL260;case 116: _LL343: _LL344: {
# 1776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1777 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL260;}case 117: _LL345: _LL346: {
# 1780
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1782
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1781 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL260;}case 118: _LL347: _LL348: {
# 1784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1782 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL260;}case 119: _LL349: _LL34A:
# 1785
 yyval=Cyc_YY24(0);
goto _LL260;case 120: _LL34B: _LL34C: {
# 1788
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1790
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1789 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp55A=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp55A != 0;_tmp55A=_tmp55A->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp55A->hd,decls);}}{
# 1793
struct Cyc_List_List*_tmp55B=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp55B != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp55B,decls);
yyval=Cyc_YY24(decls);
# 1798
goto _LL260;};}case 121: _LL34D: _LL34E: {
# 1800
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->hd=Cyc_yyget_YY24(yyyyvsp[0]);_tmp55C->tl=0;_tmp55C;}));
goto _LL260;}case 122: _LL34F: _LL350: {
# 1806
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1805 "parse.y"
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->hd=Cyc_yyget_YY24(yyyyvsp[1]);_tmp55D->tl=Cyc_yyget_YY25(yyyyvsp[0]);_tmp55D;}));
goto _LL260;}case 123: _LL351: _LL352: {
# 1808
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1809 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
goto _LL260;}case 124: _LL353: _LL354: {
# 1812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY19(({struct _tuple12*_tmp55E=_region_malloc(yyr,sizeof(*_tmp55E));_tmp55E->tl=0;_tmp55E->hd=Cyc_yyget_YY18(yyyyvsp[0]);_tmp55E;}));
goto _LL260;}case 125: _LL355: _LL356: {
# 1818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1817 "parse.y"
yyval=Cyc_YY19(({struct _tuple12*_tmp55F=_region_malloc(yyr,sizeof(*_tmp55F));_tmp55F->tl=Cyc_yyget_YY19(yyyyvsp[0]);_tmp55F->hd=Cyc_yyget_YY18(yyyyvsp[2]);_tmp55F;}));
goto _LL260;}case 126: _LL357: _LL358: {
# 1820
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1822 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp560;_tmp560.f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp560.f2=0;_tmp560;}));
goto _LL260;}case 127: _LL359: _LL35A: {
# 1825
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1824 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp561;_tmp561.f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp561.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp561;}));
goto _LL260;}case 128: _LL35B: _LL35C: {
# 1827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1830 "parse.y"
struct _RegionHandle _tmp562=_new_region("temp");struct _RegionHandle*temp=& _tmp562;_push_region(temp);
{struct _tuple25 _tmp563=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp564=_tmp563;struct Cyc_Absyn_Tqual _tmp565;struct Cyc_Parse_Type_specifier _tmp566;struct Cyc_List_List*_tmp567;_LL69B: _tmp565=_tmp564.f1;_tmp566=_tmp564.f2;_tmp567=_tmp564.f3;_LL69C:;
if(_tmp565.loc == 0)_tmp565.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp568=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp568 != 0;_tmp568=_tmp568->tl){
struct _tuple24*_tmp569=(struct _tuple24*)_tmp568->hd;struct _tuple24*_tmp56A=_tmp569;struct Cyc_Parse_Declarator _tmp56B;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Exp*_tmp56D;_LL69E: _tmp56B=_tmp56A->f1;_tmp56C=_tmp56A->f2;_tmp56D=_tmp56A->f3;_LL69F:;
decls=({struct _tuple10*_tmp56E=_region_malloc(temp,sizeof(*_tmp56E));_tmp56E->tl=decls;_tmp56E->hd=_tmp56B;_tmp56E;});
widths_and_reqs=({struct Cyc_List_List*_tmp56F=_region_malloc(temp,sizeof(*_tmp56F));_tmp56F->hd=({struct _tuple16*_tmp570=_region_malloc(temp,sizeof(*_tmp570));_tmp570->f1=_tmp56C;_tmp570->f2=_tmp56D;_tmp570;});_tmp56F->tl=widths_and_reqs;_tmp56F;});}}
# 1841
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp571=Cyc_Parse_speclist2typ(_tmp566,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp572=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp565,_tmp571,decls,_tmp567),widths_and_reqs);
# 1847
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp572));
# 1849
_npop_handler(0);goto _LL260;};};}
# 1831
;_pop_region(temp);}case 129: _LL35D: _LL35E: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1857 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp573;_tmp573.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp573.f2=Cyc_yyget_YY21(yyyyvsp[0]);_tmp573.f3=0;_tmp573;}));
goto _LL260;}case 130: _LL35F: _LL360: {
# 1860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1859 "parse.y"
struct _tuple25 _tmp574=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp575;_tmp575.f1=_tmp574.f1;_tmp575.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp574.f2);_tmp575.f3=_tmp574.f3;_tmp575;}));
goto _LL260;}case 131: _LL361: _LL362: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1861 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp576;_tmp576.f1=Cyc_yyget_YY23(yyyyvsp[0]);_tmp576.f2=Cyc_Parse_empty_spec(0);_tmp576.f3=0;_tmp576;}));
goto _LL260;}case 132: _LL363: _LL364: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1863 "parse.y"
struct _tuple25 _tmp577=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp578;_tmp578.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp577.f1);_tmp578.f2=_tmp577.f2;_tmp578.f3=_tmp577.f3;_tmp578;}));
goto _LL260;}case 133: _LL365: _LL366: {
# 1867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1866 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp579;_tmp579.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp579.f2=Cyc_Parse_empty_spec(0);_tmp579.f3=Cyc_yyget_YY45(yyyyvsp[0]);_tmp579;}));
goto _LL260;}case 134: _LL367: _LL368: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1868 "parse.y"
struct _tuple25 _tmp57A=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp57B;_tmp57B.f1=_tmp57A.f1;_tmp57B.f2=_tmp57A.f2;_tmp57B.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp57A.f3);_tmp57B;}));
goto _LL260;}case 135: _LL369: _LL36A: {
# 1871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY35(({struct _tuple25 _tmp57C;_tmp57C.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp57C.f2=Cyc_yyget_YY21(yyyyvsp[0]);_tmp57C.f3=0;_tmp57C;}));
goto _LL260;}case 136: _LL36B: _LL36C: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1876 "parse.y"
struct _tuple25 _tmp57D=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp57E;_tmp57E.f1=_tmp57D.f1;_tmp57E.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp57D.f2);_tmp57E.f3=_tmp57D.f3;_tmp57E;}));
goto _LL260;}case 137: _LL36D: _LL36E: {
# 1879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1878 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp57F;_tmp57F.f1=Cyc_yyget_YY23(yyyyvsp[0]);_tmp57F.f2=Cyc_Parse_empty_spec(0);_tmp57F.f3=0;_tmp57F;}));
goto _LL260;}case 138: _LL36F: _LL370: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1880 "parse.y"
struct _tuple25 _tmp580=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp581;_tmp581.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp580.f1);_tmp581.f2=_tmp580.f2;_tmp581.f3=_tmp580.f3;_tmp581;}));
goto _LL260;}case 139: _LL371: _LL372: {
# 1884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1883 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp582;_tmp582.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp582.f2=Cyc_Parse_empty_spec(0);_tmp582.f3=Cyc_yyget_YY45(yyyyvsp[0]);_tmp582;}));
goto _LL260;}case 140: _LL373: _LL374: {
# 1886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1885 "parse.y"
struct _tuple25 _tmp583=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp584;_tmp584.f1=_tmp583.f1;_tmp584.f2=_tmp583.f2;_tmp584.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp583.f3);_tmp584;}));
goto _LL260;}case 141: _LL375: _LL376: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1889 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
goto _LL260;}case 142: _LL377: _LL378: {
# 1892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp585=_region_malloc(yyr,sizeof(*_tmp585));_tmp585->hd=Cyc_yyget_YY28(yyyyvsp[0]);_tmp585->tl=0;_tmp585;}));
goto _LL260;}case 143: _LL379: _LL37A: {
# 1898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1897 "parse.y"
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp586=_region_malloc(yyr,sizeof(*_tmp586));_tmp586->hd=Cyc_yyget_YY28(yyyyvsp[2]);_tmp586->tl=Cyc_yyget_YY29(yyyyvsp[0]);_tmp586;}));
goto _LL260;}case 144: _LL37B: _LL37C: {
# 1900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1902 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp587=_region_malloc(yyr,sizeof(*_tmp587));_tmp587->f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp587->f2=0;_tmp587->f3=Cyc_yyget_YY55(yyyyvsp[1]);_tmp587;}));
goto _LL260;}case 145: _LL37D: _LL37E: {
# 1905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1906 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp588=_region_malloc(yyr,sizeof(*_tmp588));_tmp588->f1=({struct Cyc_Parse_Declarator _tmp589;_tmp589.id=({struct _tuple0*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->f1=Cyc_Absyn_Rel_n(0);_tmp58A->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);_tmp58A;});_tmp589.tms=0;_tmp589;});_tmp588->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp588->f3=0;_tmp588;}));
# 1908
goto _LL260;}case 146: _LL37F: _LL380:
# 1912
 yyval=Cyc_YY28(({struct _tuple24*_tmp58C=_region_malloc(yyr,sizeof(*_tmp58C));_tmp58C->f1=({struct Cyc_Parse_Declarator _tmp58D;_tmp58D.id=({struct _tuple0*_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->f1=Cyc_Absyn_Rel_n(0);_tmp58E->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);_tmp58E;});_tmp58D.tms=0;_tmp58D;});_tmp58C->f2=0;_tmp58C->f3=0;_tmp58C;}));
# 1914
goto _LL260;case 147: _LL381: _LL382: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1914 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp590=_region_malloc(yyr,sizeof(*_tmp590));_tmp590->f1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp590->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp590->f3=0;_tmp590;}));
goto _LL260;}case 148: _LL383: _LL384: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1918 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL260;}case 149: _LL385: _LL386:
# 1921
 yyval=Cyc_YY55(0);
goto _LL260;case 150: _LL387: _LL388: {
# 1924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1923 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL260;}case 151: _LL389: _LL38A:
# 1926
 yyval=Cyc_YY55(0);
goto _LL260;case 152: _LL38B: _LL38C: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1930 "parse.y"
int _tmp591=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp592=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Absyn_TypeDecl*_tmp593=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp592,({struct Cyc_Core_Opt*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->v=Cyc_yyget_YY34(yyyyvsp[4]);_tmp596;}),_tmp591,(unsigned int)(yyyylsp[0]).first_line);
# 1934
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594[0]=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp595;_tmp595.tag=26;_tmp595.f1=_tmp593;_tmp595.f2=0;_tmp595;});_tmp594;}),(unsigned int)(yyyylsp[0]).first_line));
# 1936
goto _LL260;}case 153: _LL38D: _LL38E: {
# 1938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1937 "parse.y"
int _tmp597=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp599;_tmp599.tag=3;_tmp599.f1=({struct Cyc_Absyn_DatatypeInfo _tmp59A;_tmp59A.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp59B;_tmp59B.name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp59B.is_extensible=_tmp597;_tmp59B;}));_tmp59A.targs=Cyc_yyget_YY40(yyyyvsp[2]);_tmp59A;});_tmp599;});_tmp598;}),(unsigned int)(yyyylsp[0]).first_line));
# 1940
goto _LL260;}case 154: _LL38F: _LL390: {
# 1942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1941 "parse.y"
int _tmp59C=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp59E;_tmp59E.tag=4;_tmp59E.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp59F;_tmp59F.field_info=
Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp5A0;_tmp5A0.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5A0.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp5A0.is_extensible=_tmp59C;_tmp5A0;}));_tmp59F.targs=Cyc_yyget_YY40(yyyyvsp[4]);_tmp59F;});_tmp59E;});_tmp59D;}),(unsigned int)(yyyylsp[0]).first_line));
# 1946
goto _LL260;}case 155: _LL391: _LL392: {
# 1948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1949 "parse.y"
yyval=Cyc_YY31(0);
goto _LL260;}case 156: _LL393: _LL394: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1950 "parse.y"
yyval=Cyc_YY31(1);
goto _LL260;}case 157: _LL395: _LL396: {
# 1953
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1954 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5A1->tl=0;_tmp5A1;}));
goto _LL260;}case 158: _LL397: _LL398: {
# 1957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1955 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5A2->tl=0;_tmp5A2;}));
goto _LL260;}case 159: _LL399: _LL39A: {
# 1958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1956 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5A3->tl=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5A3;}));
goto _LL260;}case 160: _LL39B: _LL39C: {
# 1959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1957 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->hd=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5A4->tl=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5A4;}));
goto _LL260;}case 161: _LL39D: _LL39E:
# 1960
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL260;case 162: _LL39F: _LL3A0: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1962 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL260;}case 163: _LL3A1: _LL3A2: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1963 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL260;}case 164: _LL3A3: _LL3A4: {
# 1966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1967 "parse.y"
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5A5->typs=0;_tmp5A5->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5A5->sc=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5A5;}));
goto _LL260;}case 165: _LL3A5: _LL3A6: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1969 "parse.y"
struct Cyc_List_List*_tmp5A6=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->name=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5A7->typs=_tmp5A6;_tmp5A7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5A7->sc=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5A7;}));
goto _LL260;}case 166: _LL3A7: _LL3A8: {
# 1973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1975 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 167: _LL3A9: _LL3AA: {
# 1978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1977 "parse.y"
struct Cyc_Parse_Declarator _tmp5A8=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5A9;_tmp5A9.id=_tmp5A8.id;_tmp5A9.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5A8.tms);_tmp5A9;}));
goto _LL260;}case 168: _LL3AB: _LL3AC: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
goto _LL260;}case 169: _LL3AD: _LL3AE: {
# 1987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1986 "parse.y"
struct Cyc_Parse_Declarator _tmp5AA=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5AB;_tmp5AB.id=_tmp5AA.id;_tmp5AB.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5AA.tms);_tmp5AB;}));
goto _LL260;}case 170: _LL3AF: _LL3B0: {
# 1990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1992 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5AC;_tmp5AC.id=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5AC.tms=0;_tmp5AC;}));
goto _LL260;}case 171: _LL3B1: _LL3B2: {
# 1995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1994 "parse.y"
yyval=yyyyvsp[1];
goto _LL260;}case 172: _LL3B3: _LL3B4: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1998 "parse.y"
struct Cyc_Parse_Declarator _tmp5AD=Cyc_yyget_YY27(yyyyvsp[2]);
_tmp5AD.tms=({struct Cyc_List_List*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));_tmp5AE->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));_tmp5AF[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5B0;_tmp5B0.tag=5;_tmp5B0.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp5B0.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5B0;});_tmp5AF;});_tmp5AE->tl=_tmp5AD.tms;_tmp5AE;});
yyval=yyyyvsp[2];
# 2002
goto _LL260;}case 173: _LL3B5: _LL3B6: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2003 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5B1;_tmp5B1.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5B1.tms=({struct Cyc_List_List*_tmp5B2=_region_malloc(yyr,sizeof(*_tmp5B2));_tmp5B2->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5B3=_region_malloc(yyr,sizeof(*_tmp5B3));_tmp5B3[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5B4;_tmp5B4.tag=0;_tmp5B4.f1=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5B4.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5B4;});_tmp5B3;});_tmp5B2->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5B2;});_tmp5B1;}));
goto _LL260;}case 174: _LL3B7: _LL3B8: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2005 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5B5;_tmp5B5.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5B5.tms=({struct Cyc_List_List*_tmp5B6=_region_malloc(yyr,sizeof(*_tmp5B6));_tmp5B6->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5B7=_region_malloc(yyr,sizeof(*_tmp5B7));_tmp5B7[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5B8;_tmp5B8.tag=1;_tmp5B8.f1=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5B8.f2=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5B8.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5B8;});_tmp5B7;});_tmp5B6->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5B6;});_tmp5B5;}));
goto _LL260;}case 175: _LL3B9: _LL3BA: {
# 2009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2008 "parse.y"
struct _tuple26*_tmp5B9=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5BA=_tmp5B9;struct Cyc_List_List*_tmp5BB;int _tmp5BC;struct Cyc_Absyn_VarargInfo*_tmp5BD;void*_tmp5BE;struct Cyc_List_List*_tmp5BF;_LL6A1: _tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BA->f2;_tmp5BD=_tmp5BA->f3;_tmp5BE=_tmp5BA->f4;_tmp5BF=_tmp5BA->f5;_LL6A2:;{
struct Cyc_Absyn_Exp*_tmp5C0=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5C1=Cyc_yyget_YY55(yyyyvsp[5]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5C2;_tmp5C2.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5C2.tms=({struct Cyc_List_List*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));_tmp5C3->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5C5;_tmp5C5.tag=3;_tmp5C5.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));_tmp5C6[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5C7;_tmp5C7.tag=1;_tmp5C7.f1=_tmp5BB;_tmp5C7.f2=_tmp5BC;_tmp5C7.f3=_tmp5BD;_tmp5C7.f4=_tmp5BE;_tmp5C7.f5=_tmp5BF;_tmp5C7.f6=_tmp5C0;_tmp5C7.f7=_tmp5C1;_tmp5C7;});_tmp5C6;});_tmp5C5;});_tmp5C4;});_tmp5C3->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5C3;});_tmp5C2;}));
# 2013
goto _LL260;};}case 176: _LL3BB: _LL3BC: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2014 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5C8;_tmp5C8.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5C8.tms=({struct Cyc_List_List*_tmp5C9=_region_malloc(yyr,sizeof(*_tmp5C9));_tmp5C9->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));_tmp5CA[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5CB;_tmp5CB.tag=3;_tmp5CB.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5CC=_region_malloc(yyr,sizeof(*_tmp5CC));_tmp5CC[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5CD;_tmp5CD.tag=1;_tmp5CD.f1=0;_tmp5CD.f2=0;_tmp5CD.f3=0;_tmp5CD.f4=
# 2017
Cyc_yyget_YY49(yyyyvsp[2]);_tmp5CD.f5=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5CD.f6=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5CD.f7=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5CD;});_tmp5CC;});_tmp5CB;});_tmp5CA;});_tmp5C9->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5C9;});_tmp5C8;}));
# 2020
goto _LL260;}case 177: _LL3BD: _LL3BE: {
# 2022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2021 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5CE;_tmp5CE.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5CE.tms=({struct Cyc_List_List*_tmp5CF=_region_malloc(yyr,sizeof(*_tmp5CF));_tmp5CF->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5D0=_region_malloc(yyr,sizeof(*_tmp5D0));_tmp5D0[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5D1;_tmp5D1.tag=3;_tmp5D1.f1=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp5D2=_region_malloc(yyr,sizeof(*_tmp5D2));_tmp5D2[0]=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp5D3;_tmp5D3.tag=0;_tmp5D3.f1=Cyc_yyget_YY36(yyyyvsp[2]);_tmp5D3.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5D3;});_tmp5D2;});_tmp5D1;});_tmp5D0;});_tmp5CF->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5CF;});_tmp5CE;}));
goto _LL260;}case 178: _LL3BF: _LL3C0: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2024 "parse.y"
struct Cyc_List_List*_tmp5D4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5D5;_tmp5D5.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5D5.tms=({struct Cyc_List_List*_tmp5D6=_region_malloc(yyr,sizeof(*_tmp5D6));_tmp5D6->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5D7=_region_malloc(yyr,sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp5D8;_tmp5D8.tag=4;_tmp5D8.f1=_tmp5D4;_tmp5D8.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5D8.f3=0;_tmp5D8;});_tmp5D7;});_tmp5D6->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5D6;});_tmp5D5;}));
# 2027
goto _LL260;}case 179: _LL3C1: _LL3C2: {
# 2029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2028 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5D9;_tmp5D9.id=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5D9.tms=({struct Cyc_List_List*_tmp5DA=_region_malloc(yyr,sizeof(*_tmp5DA));_tmp5DA->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5DB=_region_malloc(yyr,sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5DC;_tmp5DC.tag=5;_tmp5DC.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp5DC.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5DC;});_tmp5DB;});_tmp5DA->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5DA;});_tmp5D9;}));
# 2031
goto _LL260;}case 180: _LL3C3: _LL3C4: {
# 2033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2042 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5DD;_tmp5DD.id=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5DD.tms=0;_tmp5DD;}));
goto _LL260;}case 181: _LL3C5: _LL3C6: {
# 2045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2044 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5DE;_tmp5DE.id=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5DE.tms=0;_tmp5DE;}));
goto _LL260;}case 182: _LL3C7: _LL3C8: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2046 "parse.y"
yyval=yyyyvsp[1];
goto _LL260;}case 183: _LL3C9: _LL3CA: {
# 2049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2050 "parse.y"
struct Cyc_Parse_Declarator _tmp5DF=Cyc_yyget_YY27(yyyyvsp[2]);
_tmp5DF.tms=({struct Cyc_List_List*_tmp5E0=_region_malloc(yyr,sizeof(*_tmp5E0));_tmp5E0->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5E1=_region_malloc(yyr,sizeof(*_tmp5E1));_tmp5E1[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5E2;_tmp5E2.tag=5;_tmp5E2.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp5E2.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5E2;});_tmp5E1;});_tmp5E0->tl=_tmp5DF.tms;_tmp5E0;});
yyval=yyyyvsp[2];
# 2054
goto _LL260;}case 184: _LL3CB: _LL3CC: {
# 2056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2055 "parse.y"
struct Cyc_Parse_Declarator _tmp5E3=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E4;_tmp5E4.id=_tmp5E3.id;_tmp5E4.tms=({struct Cyc_List_List*_tmp5E5=_region_malloc(yyr,sizeof(*_tmp5E5));_tmp5E5->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5E6=_region_malloc(yyr,sizeof(*_tmp5E6));_tmp5E6[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5E7;_tmp5E7.tag=0;_tmp5E7.f1=
Cyc_yyget_YY51(yyyyvsp[3]);_tmp5E7.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5E7;});_tmp5E6;});_tmp5E5->tl=_tmp5E3.tms;_tmp5E5;});_tmp5E4;}));
goto _LL260;}case 185: _LL3CD: _LL3CE: {
# 2060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2059 "parse.y"
struct Cyc_Parse_Declarator _tmp5E8=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E9;_tmp5E9.id=_tmp5E8.id;_tmp5E9.tms=({struct Cyc_List_List*_tmp5EA=_region_malloc(yyr,sizeof(*_tmp5EA));_tmp5EA->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5EB=_region_malloc(yyr,sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5EC;_tmp5EC.tag=1;_tmp5EC.f1=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5EC.f2=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5EC.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5EC;});_tmp5EB;});_tmp5EA->tl=_tmp5E8.tms;_tmp5EA;});_tmp5E9;}));
# 2063
goto _LL260;}case 186: _LL3CF: _LL3D0: {
# 2065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2064 "parse.y"
struct _tuple26*_tmp5ED=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5EE=_tmp5ED;struct Cyc_List_List*_tmp5EF;int _tmp5F0;struct Cyc_Absyn_VarargInfo*_tmp5F1;void*_tmp5F2;struct Cyc_List_List*_tmp5F3;_LL6A4: _tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;_tmp5F1=_tmp5EE->f3;_tmp5F2=_tmp5EE->f4;_tmp5F3=_tmp5EE->f5;_LL6A5:;{
struct Cyc_Absyn_Exp*_tmp5F4=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5F5=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp5F6=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5F7;_tmp5F7.id=_tmp5F6.id;_tmp5F7.tms=({struct Cyc_List_List*_tmp5F8=_region_malloc(yyr,sizeof(*_tmp5F8));_tmp5F8->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5F9=_region_malloc(yyr,sizeof(*_tmp5F9));_tmp5F9[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5FA;_tmp5FA.tag=3;_tmp5FA.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5FB=_region_malloc(yyr,sizeof(*_tmp5FB));_tmp5FB[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5FC;_tmp5FC.tag=1;_tmp5FC.f1=_tmp5EF;_tmp5FC.f2=_tmp5F0;_tmp5FC.f3=_tmp5F1;_tmp5FC.f4=_tmp5F2;_tmp5FC.f5=_tmp5F3;_tmp5FC.f6=_tmp5F4;_tmp5FC.f7=_tmp5F5;_tmp5FC;});_tmp5FB;});_tmp5FA;});_tmp5F9;});_tmp5F8->tl=_tmp5F6.tms;_tmp5F8;});_tmp5F7;}));
# 2070
goto _LL260;};}case 187: _LL3D1: _LL3D2: {
# 2072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2071 "parse.y"
struct Cyc_Parse_Declarator _tmp5FD=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5FE;_tmp5FE.id=_tmp5FD.id;_tmp5FE.tms=({struct Cyc_List_List*_tmp5FF=_region_malloc(yyr,sizeof(*_tmp5FF));_tmp5FF->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp600=_region_malloc(yyr,sizeof(*_tmp600));_tmp600[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp601;_tmp601.tag=3;_tmp601.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp602=_region_malloc(yyr,sizeof(*_tmp602));_tmp602[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp603;_tmp603.tag=1;_tmp603.f1=0;_tmp603.f2=0;_tmp603.f3=0;_tmp603.f4=
# 2075
Cyc_yyget_YY49(yyyyvsp[2]);_tmp603.f5=Cyc_yyget_YY50(yyyyvsp[3]);_tmp603.f6=Cyc_yyget_YY55(yyyyvsp[5]);_tmp603.f7=Cyc_yyget_YY55(yyyyvsp[6]);_tmp603;});_tmp602;});_tmp601;});_tmp600;});_tmp5FF->tl=_tmp5FD.tms;_tmp5FF;});_tmp5FE;}));
# 2078
goto _LL260;}case 188: _LL3D3: _LL3D4: {
# 2080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2079 "parse.y"
struct Cyc_Parse_Declarator _tmp604=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp605;_tmp605.id=_tmp604.id;_tmp605.tms=({struct Cyc_List_List*_tmp606=_region_malloc(yyr,sizeof(*_tmp606));_tmp606->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp607=_region_malloc(yyr,sizeof(*_tmp607));_tmp607[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp608;_tmp608.tag=3;_tmp608.f1=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp609=_region_malloc(yyr,sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp60A;_tmp60A.tag=0;_tmp60A.f1=Cyc_yyget_YY36(yyyyvsp[2]);_tmp60A.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp60A;});_tmp609;});_tmp608;});_tmp607;});_tmp606->tl=_tmp604.tms;_tmp606;});_tmp605;}));
goto _LL260;}case 189: _LL3D5: _LL3D6: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2083 "parse.y"
struct Cyc_List_List*_tmp60B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp60C=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp60D;_tmp60D.id=_tmp60C.id;_tmp60D.tms=({struct Cyc_List_List*_tmp60E=_region_malloc(yyr,sizeof(*_tmp60E));_tmp60E->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp60F=_region_malloc(yyr,sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp610;_tmp610.tag=4;_tmp610.f1=_tmp60B;_tmp610.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp610.f3=0;_tmp610;});_tmp60F;});_tmp60E->tl=_tmp60C.tms;_tmp60E;});_tmp60D;}));
# 2087
goto _LL260;}case 190: _LL3D7: _LL3D8: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2088 "parse.y"
struct Cyc_Parse_Declarator _tmp611=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp612;_tmp612.id=_tmp611.id;_tmp612.tms=({struct Cyc_List_List*_tmp613=_region_malloc(yyr,sizeof(*_tmp613));_tmp613->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp614=_region_malloc(yyr,sizeof(*_tmp614));_tmp614[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp615;_tmp615.tag=5;_tmp615.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp615.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp615;});_tmp614;});_tmp613->tl=_tmp611.tms;_tmp613;});_tmp612;}));
# 2091
goto _LL260;}case 191: _LL3D9: _LL3DA: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2101 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 192: _LL3DB: _LL3DC: {
# 2104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2102 "parse.y"
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
goto _LL260;}case 193: _LL3DD: _LL3DE: {
# 2105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2106 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
ans=({struct Cyc_List_List*_tmp616=_region_malloc(yyr,sizeof(*_tmp616));_tmp616->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp617=_region_malloc(yyr,sizeof(*_tmp617));_tmp617[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp618;_tmp618.tag=5;_tmp618.f1=(unsigned int)(yyyylsp[3]).first_line;_tmp618.f2=Cyc_yyget_YY45(yyyyvsp[3]);_tmp618;});_tmp617;});_tmp616->tl=ans;_tmp616;});{
# 2110
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp619=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp61A=_tmp619;unsigned int _tmp61B;union Cyc_Absyn_Constraint*_tmp61C;union Cyc_Absyn_Constraint*_tmp61D;_LL6A7: _tmp61B=_tmp61A.f1;_tmp61C=_tmp61A.f2;_tmp61D=_tmp61A.f3;_LL6A8:;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp61E=_cycalloc_atomic(sizeof(*_tmp61E));_tmp61E->ptr_loc=_tmp61B;_tmp61E->rgn_loc=(unsigned int)(yyyylsp[2]).first_line;_tmp61E->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp61E;});{
# 2115
struct _tuple14 _tmp61F=Cyc_Parse_collapse_pointer_quals(_tmp61B,_tmp61C,_tmp61D,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));struct _tuple14 _tmp620=_tmp61F;union Cyc_Absyn_Constraint*_tmp621;union Cyc_Absyn_Constraint*_tmp622;union Cyc_Absyn_Constraint*_tmp623;void*_tmp624;_LL6AA: _tmp621=_tmp620.f1;_tmp622=_tmp620.f2;_tmp623=_tmp620.f3;_tmp624=_tmp620.f4;_LL6AB:;
ans=({struct Cyc_List_List*_tmp625=_region_malloc(yyr,sizeof(*_tmp625));_tmp625->hd=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp626=_region_malloc(yyr,sizeof(*_tmp626));_tmp626[0]=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp627;_tmp627.tag=2;_tmp627.f1=({struct Cyc_Absyn_PtrAtts _tmp628;_tmp628.rgn=_tmp624;_tmp628.nullable=_tmp621;_tmp628.bounds=_tmp622;_tmp628.zero_term=_tmp623;_tmp628.ptrloc=ptrloc;_tmp628;});_tmp627.f2=Cyc_yyget_YY23(yyyyvsp[4]);_tmp627;});_tmp626;});_tmp625->tl=ans;_tmp625;});
yyval=Cyc_YY26(ans);
# 2119
goto _LL260;};};}case 194: _LL3DF: _LL3E0:
# 2121
 yyval=Cyc_YY54(0);
goto _LL260;case 195: _LL3E1: _LL3E2: {
# 2124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2122 "parse.y"
yyval=Cyc_YY54(({struct Cyc_List_List*_tmp629=_region_malloc(yyr,sizeof(*_tmp629));_tmp629->hd=Cyc_yyget_YY53(yyyyvsp[0]);_tmp629->tl=Cyc_yyget_YY54(yyyyvsp[1]);_tmp629;}));
goto _LL260;}case 196: _LL3E3: _LL3E4: {
# 2125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2127 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp62A=_region_malloc(yyr,sizeof(*_tmp62A));_tmp62A[0]=({struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmp62B;_tmp62B.tag=0;_tmp62B.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp62B;});_tmp62A;}));
goto _LL260;}case 197: _LL3E5: _LL3E6: {
# 2130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2129 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp62C=_region_malloc(yyr,sizeof(*_tmp62C));_tmp62C[0]=({struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmp62D;_tmp62D.tag=1;_tmp62D.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp62D;});_tmp62C;}));
goto _LL260;}case 198: _LL3E7: _LL3E8: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2131 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp62E=_region_malloc(yyr,sizeof(*_tmp62E));_tmp62E[0]=({struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmp62F;_tmp62F.tag=2;_tmp62F;});_tmp62E;}));
goto _LL260;}case 199: _LL3E9: _LL3EA: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2133 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp630=_region_malloc(yyr,sizeof(*_tmp630));_tmp630[0]=({struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmp631;_tmp631.tag=3;_tmp631;});_tmp630;}));
goto _LL260;}case 200: _LL3EB: _LL3EC: {
# 2136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2135 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp632=_region_malloc(yyr,sizeof(*_tmp632));_tmp632[0]=({struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmp633;_tmp633.tag=4;_tmp633;});_tmp632;}));
goto _LL260;}case 201: _LL3ED: _LL3EE: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2137 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp634=_region_malloc(yyr,sizeof(*_tmp634));_tmp634[0]=({struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmp635;_tmp635.tag=5;_tmp635;});_tmp634;}));
goto _LL260;}case 202: _LL3EF: _LL3F0: {
# 2140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2139 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp636=_region_malloc(yyr,sizeof(*_tmp636));_tmp636[0]=({struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmp637;_tmp637.tag=6;_tmp637;});_tmp636;}));
goto _LL260;}case 203: _LL3F1: _LL3F2: {
# 2142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2141 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp638=_region_malloc(yyr,sizeof(*_tmp638));_tmp638[0]=({struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmp639;_tmp639.tag=7;_tmp639;});_tmp638;}));
goto _LL260;}case 204: _LL3F3: _LL3F4: {
# 2144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2147 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->f1=loc;_tmp63A->f2=Cyc_Absyn_true_conref;_tmp63A->f3=Cyc_yyget_YY2(yyyyvsp[1]);_tmp63A;}));
# 2150
goto _LL260;}case 205: _LL3F5: _LL3F6: {
# 2152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2151 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->f1=loc;_tmp63B->f2=Cyc_Absyn_false_conref;_tmp63B->f3=Cyc_yyget_YY2(yyyyvsp[1]);_tmp63B;}));
# 2154
goto _LL260;}case 206: _LL3F7: _LL3F8: {
# 2156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2155 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->f1=loc;_tmp63C->f2=Cyc_Absyn_true_conref;_tmp63C->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp63C;}));
# 2158
goto _LL260;}case 207: _LL3F9: _LL3FA:
# 2160
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
goto _LL260;case 208: _LL3FB: _LL3FC: {
# 2163
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2165
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2161 "parse.y"
yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp63E;_tmp63E.tag=1;_tmp63E.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp63E;});_tmp63D;})));
goto _LL260;}case 209: _LL3FD: _LL3FE:
# 2164
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
goto _LL260;case 210: _LL3FF: _LL400: {
# 2167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2165 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
goto _LL260;}case 211: _LL401: _LL402: {
# 2168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2166 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
goto _LL260;}case 212: _LL403: _LL404:
# 2169
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL260;case 213: _LL405: _LL406: {
# 2172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2172 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL260;}case 214: _LL407: _LL408: {
# 2175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2173 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL260;}case 215: _LL409: _LL40A:
# 2176
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
goto _LL260;case 216: _LL40B: _LL40C: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2178 "parse.y"
yyval=Cyc_YY23(Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),Cyc_yyget_YY23(yyyyvsp[1])));
goto _LL260;}case 217: _LL40D: _LL40E: {
# 2181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2183 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp63F->f2=0;_tmp63F->f3=0;_tmp63F->f4=Cyc_yyget_YY49(yyyyvsp[1]);_tmp63F->f5=Cyc_yyget_YY50(yyyyvsp[2]);_tmp63F;}));
goto _LL260;}case 218: _LL40F: _LL410: {
# 2186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2185 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp640->f2=1;_tmp640->f3=0;_tmp640->f4=Cyc_yyget_YY49(yyyyvsp[3]);_tmp640->f5=Cyc_yyget_YY50(yyyyvsp[4]);_tmp640;}));
goto _LL260;}case 219: _LL411: _LL412: {
# 2188
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2188 "parse.y"
struct _tuple8*_tmp641=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp642=_tmp641;struct _dyneither_ptr*_tmp643;struct Cyc_Absyn_Tqual _tmp644;void*_tmp645;_LL6AD: _tmp643=_tmp642->f1;_tmp644=_tmp642->f2;_tmp645=_tmp642->f3;_LL6AE:;{
struct Cyc_Absyn_VarargInfo*_tmp646=({struct Cyc_Absyn_VarargInfo*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->name=_tmp643;_tmp648->tq=_tmp644;_tmp648->type=_tmp645;_tmp648->inject=Cyc_yyget_YY31(yyyyvsp[1]);_tmp648;});
yyval=Cyc_YY39(({struct _tuple26*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->f1=0;_tmp647->f2=0;_tmp647->f3=_tmp646;_tmp647->f4=Cyc_yyget_YY49(yyyyvsp[3]);_tmp647->f5=Cyc_yyget_YY50(yyyyvsp[4]);_tmp647;}));
# 2192
goto _LL260;};}case 220: _LL413: _LL414: {
# 2194
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2194 "parse.y"
struct _tuple8*_tmp649=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp64A=_tmp649;struct _dyneither_ptr*_tmp64B;struct Cyc_Absyn_Tqual _tmp64C;void*_tmp64D;_LL6B0: _tmp64B=_tmp64A->f1;_tmp64C=_tmp64A->f2;_tmp64D=_tmp64A->f3;_LL6B1:;{
struct Cyc_Absyn_VarargInfo*_tmp64E=({struct Cyc_Absyn_VarargInfo*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->name=_tmp64B;_tmp650->tq=_tmp64C;_tmp650->type=_tmp64D;_tmp650->inject=Cyc_yyget_YY31(yyyyvsp[3]);_tmp650;});
yyval=Cyc_YY39(({struct _tuple26*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp64F->f2=0;_tmp64F->f3=_tmp64E;_tmp64F->f4=Cyc_yyget_YY49(yyyyvsp[5]);_tmp64F->f5=Cyc_yyget_YY50(yyyyvsp[6]);_tmp64F;}));
# 2198
goto _LL260;};}case 221: _LL415: _LL416: {
# 2200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2202 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp652;_tmp652.tag=1;_tmp652.f1=0;_tmp652;});_tmp651;})));
goto _LL260;}case 222: _LL417: _LL418: {
# 2205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2203 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
goto _LL260;}case 223: _LL419: _LL41A:
# 2206
 yyval=Cyc_YY49(0);
goto _LL260;case 224: _LL41B: _LL41C: {
# 2209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2207 "parse.y"
yyval=Cyc_YY49((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp654;_tmp654.tag=24;_tmp654.f1=Cyc_yyget_YY40(yyyyvsp[1]);_tmp654;});_tmp653;}));
goto _LL260;}case 225: _LL41D: _LL41E:
# 2210
 yyval=Cyc_YY50(0);
goto _LL260;case 226: _LL41F: _LL420: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2212 "parse.y"
yyval=yyyyvsp[1];
goto _LL260;}case 227: _LL421: _LL422: {
# 2215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2220 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp655=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp65D;_tmp65D.tag=2;_tmp65D.f1=0;_tmp65D.f2=& Cyc_Tcutil_trk;_tmp65D;});_tmp65C;});
struct _dyneither_ptr _tmp656=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp657=Cyc_Parse_id2type(_tmp656,(void*)_tmp655);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->hd=({struct _tuple29*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->f1=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp65B;_tmp65B.tag=24;_tmp65B.f1=Cyc_yyget_YY40(yyyyvsp[0]);_tmp65B;});_tmp65A;});_tmp659->f2=_tmp657;_tmp659;});_tmp658->tl=0;_tmp658;}));
# 2225
goto _LL260;}case 228: _LL423: _LL424: {
# 2227
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2229
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2227 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp65E=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp666;_tmp666.tag=2;_tmp666.f1=0;_tmp666.f2=& Cyc_Tcutil_trk;_tmp666;});_tmp665;});
struct _dyneither_ptr _tmp65F=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp660=Cyc_Parse_id2type(_tmp65F,(void*)_tmp65E);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->hd=({struct _tuple29*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->f1=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp664;_tmp664.tag=24;_tmp664.f1=Cyc_yyget_YY40(yyyyvsp[0]);_tmp664;});_tmp663;});_tmp662->f2=_tmp660;_tmp662;});_tmp661->tl=Cyc_yyget_YY50(yyyyvsp[4]);_tmp661;}));
# 2232
goto _LL260;}case 229: _LL425: _LL426:
# 2234
 yyval=Cyc_YY31(0);
goto _LL260;case 230: _LL427: _LL428: {
# 2237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2238 "parse.y"
if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),({const char*_tmp667="inject";_tag_dyneither(_tmp667,sizeof(char),7);}))!= 0)
Cyc_Parse_err(({const char*_tmp668="missing type in function declaration";_tag_dyneither(_tmp668,sizeof(char),37);}),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY31(1);
# 2242
goto _LL260;}case 231: _LL429: _LL42A: {
# 2244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2245 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 232: _LL42B: _LL42C: {
# 2248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2246 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
goto _LL260;}case 233: _LL42D: _LL42E: {
# 2249
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2250 "parse.y"
yyval=Cyc_YY40(0);
goto _LL260;}case 234: _LL42F: _LL430: {
# 2253
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2255
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2251 "parse.y"
yyval=yyyyvsp[1];
goto _LL260;}case 235: _LL431: _LL432: {
# 2254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2253 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp66B;_tmp66B.tag=25;_tmp66B.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp66B;});_tmp66A;});_tmp669->tl=0;_tmp669;}));
goto _LL260;}case 236: _LL433: _LL434: {
# 2256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2255 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->hd=Cyc_yyget_YY44(yyyyvsp[0]);_tmp66C->tl=0;_tmp66C;}));
# 2258
goto _LL260;}case 237: _LL435: _LL436: {
# 2260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2275 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp66F;_tmp66F.tag=23;_tmp66F.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line);_tmp66F;});_tmp66E;});_tmp66D->tl=0;_tmp66D;}));
goto _LL260;}case 238: _LL437: _LL438: {
# 2278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2277 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp672;_tmp672.tag=23;_tmp672.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line);_tmp672;});_tmp671;});_tmp670->tl=Cyc_yyget_YY40(yyyyvsp[2]);_tmp670;}));
goto _LL260;}case 239: _LL439: _LL43A: {
# 2280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->hd=Cyc_yyget_YY37(yyyyvsp[0]);_tmp673->tl=0;_tmp673;}));
goto _LL260;}case 240: _LL43B: _LL43C: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2285 "parse.y"
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->hd=Cyc_yyget_YY37(yyyyvsp[2]);_tmp674->tl=Cyc_yyget_YY38(yyyyvsp[0]);_tmp674;}));
goto _LL260;}case 241: _LL43D: _LL43E: {
# 2288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp675=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp676=_tmp675;struct Cyc_Absyn_Tqual _tmp677;struct Cyc_Parse_Type_specifier _tmp678;struct Cyc_List_List*_tmp679;_LL6B3: _tmp677=_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;_LL6B4:;
if(_tmp677.loc == 0)_tmp677.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp67A=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp67B=_tmp67A;struct _tuple0*_tmp67C;struct Cyc_List_List*_tmp67D;_LL6B6: _tmp67C=_tmp67B.id;_tmp67D=_tmp67B.tms;_LL6B7:;{
void*_tmp67E=Cyc_Parse_speclist2typ(_tmp678,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp67F=Cyc_Parse_apply_tms(_tmp677,_tmp67E,_tmp679,_tmp67D);struct _tuple13 _tmp680=_tmp67F;struct Cyc_Absyn_Tqual _tmp681;void*_tmp682;struct Cyc_List_List*_tmp683;struct Cyc_List_List*_tmp684;_LL6B9: _tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;_tmp684=_tmp680.f4;_LL6BA:;
if(_tmp683 != 0)
Cyc_Parse_err(({const char*_tmp685="parameter with bad type params";_tag_dyneither(_tmp685,sizeof(char),31);}),(unsigned int)(yyyylsp[1]).first_line);
if(Cyc_Absyn_is_qvar_qualified(_tmp67C))
Cyc_Parse_err(({const char*_tmp686="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp686,sizeof(char),47);}),(unsigned int)(yyyylsp[0]).first_line);{
struct _dyneither_ptr*idopt=(*_tmp67C).f2;
if(_tmp684 != 0)
({void*_tmp687=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp688="extra attributes on parameter, ignoring";_tag_dyneither(_tmp688,sizeof(char),40);}),_tag_dyneither(_tmp687,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->f1=idopt;_tmp689->f2=_tmp681;_tmp689->f3=_tmp682;_tmp689;}));
# 2305
goto _LL260;};};};}case 242: _LL43F: _LL440: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2306 "parse.y"
struct _tuple25 _tmp68A=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp68B=_tmp68A;struct Cyc_Absyn_Tqual _tmp68C;struct Cyc_Parse_Type_specifier _tmp68D;struct Cyc_List_List*_tmp68E;_LL6BC: _tmp68C=_tmp68B.f1;_tmp68D=_tmp68B.f2;_tmp68E=_tmp68B.f3;_LL6BD:;
if(_tmp68C.loc == 0)_tmp68C.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp68F=Cyc_Parse_speclist2typ(_tmp68D,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp68E != 0)
({void*_tmp690=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp691="bad attributes on parameter, ignoring";_tag_dyneither(_tmp691,sizeof(char),38);}),_tag_dyneither(_tmp690,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->f1=0;_tmp692->f2=_tmp68C;_tmp692->f3=_tmp68F;_tmp692;}));
# 2313
goto _LL260;};}case 243: _LL441: _LL442: {
# 2315
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2314 "parse.y"
struct _tuple25 _tmp693=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp694=_tmp693;struct Cyc_Absyn_Tqual _tmp695;struct Cyc_Parse_Type_specifier _tmp696;struct Cyc_List_List*_tmp697;_LL6BF: _tmp695=_tmp694.f1;_tmp696=_tmp694.f2;_tmp697=_tmp694.f3;_LL6C0:;
if(_tmp695.loc == 0)_tmp695.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp698=Cyc_Parse_speclist2typ(_tmp696,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp699=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp69A=Cyc_Parse_apply_tms(_tmp695,_tmp698,_tmp697,_tmp699);struct _tuple13 _tmp69B=_tmp69A;struct Cyc_Absyn_Tqual _tmp69C;void*_tmp69D;struct Cyc_List_List*_tmp69E;struct Cyc_List_List*_tmp69F;_LL6C2: _tmp69C=_tmp69B.f1;_tmp69D=_tmp69B.f2;_tmp69E=_tmp69B.f3;_tmp69F=_tmp69B.f4;_LL6C3:;
if(_tmp69E != 0)
({void*_tmp6A0=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp6A1="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp6A1,sizeof(char),49);}),_tag_dyneither(_tmp6A0,sizeof(void*),0));});
# 2322
if(_tmp69F != 0)
({void*_tmp6A2=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp6A3="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6A3,sizeof(char),38);}),_tag_dyneither(_tmp6A2,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->f1=0;_tmp6A4->f2=_tmp69C;_tmp6A4->f3=_tmp69D;_tmp6A4;}));
# 2326
goto _LL260;};}case 244: _LL443: _LL444: {
# 2328
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2330 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
goto _LL260;}case 245: _LL445: _LL446: {
# 2333
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2335
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2334 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=({struct _dyneither_ptr*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6A6;});_tmp6A5->tl=0;_tmp6A5;}));
goto _LL260;}case 246: _LL447: _LL448: {
# 2337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2336 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->hd=({struct _dyneither_ptr*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6A8;});_tmp6A7->tl=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6A7;}));
goto _LL260;}case 247: _LL449: _LL44A: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2340 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 248: _LL44B: _LL44C: {
# 2343
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2341 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 249: _LL44D: _LL44E: {
# 2344
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2346 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6AA;_tmp6AA.tag=35;_tmp6AA.f1=0;_tmp6AA.f2=0;_tmp6AA;});_tmp6A9;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 250: _LL44F: _LL450: {
# 2349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2348 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6AC;_tmp6AC.tag=35;_tmp6AC.f1=0;_tmp6AC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6AC;});_tmp6AB;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 251: _LL451: _LL452: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2350 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6AE;_tmp6AE.tag=35;_tmp6AE.f1=0;_tmp6AE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6AE;});_tmp6AD;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 252: _LL453: _LL454: {
# 2353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2352 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp6AF=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->f1=Cyc_Absyn_Loc_n;_tmp6B2->f2=({struct _dyneither_ptr*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6B3;});_tmp6B2;}),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2355
(_tmp6AF->tq).real_const=1;
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp6B1;_tmp6B1.tag=26;_tmp6B1.f1=_tmp6AF;_tmp6B1.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6B1.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp6B1.f4=0;_tmp6B1;});_tmp6B0;}),(unsigned int)(yyyylsp[0]).first_line));
# 2358
goto _LL260;}case 253: _LL455: _LL456: {
# 2360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2359 "parse.y"
void*_tmp6B4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp6B6;_tmp6B6.tag=27;_tmp6B6.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6B6.f2=_tmp6B4;_tmp6B6.f3=0;_tmp6B6;});_tmp6B5;}),(unsigned int)(yyyylsp[0]).first_line));
# 2362
goto _LL260;}case 254: _LL457: _LL458: {
# 2364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->hd=({struct _tuple30*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->f1=0;_tmp6B8->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp6B8;});_tmp6B7->tl=0;_tmp6B7;}));
goto _LL260;}case 255: _LL459: _LL45A: {
# 2370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2369 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->hd=({struct _tuple30*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->f1=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6BA->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6BA;});_tmp6B9->tl=0;_tmp6B9;}));
goto _LL260;}case 256: _LL45B: _LL45C: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2371 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->hd=({struct _tuple30*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->f1=0;_tmp6BC->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp6BC;});_tmp6BB->tl=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6BB;}));
goto _LL260;}case 257: _LL45D: _LL45E: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2373 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD->hd=({struct _tuple30*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->f1=Cyc_yyget_YY41(yyyyvsp[2]);_tmp6BE->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp6BE;});_tmp6BD->tl=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6BD;}));
goto _LL260;}case 258: _LL45F: _LL460: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2377 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
goto _LL260;}case 259: _LL461: _LL462: {
# 2380
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2378 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6C1;_tmp6C1.tag=1;_tmp6C1.f1=({struct _dyneither_ptr*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6C2;});_tmp6C1;});_tmp6C0;});_tmp6BF->tl=0;_tmp6BF;}));
goto _LL260;}case 260: _LL463: _LL464: {
# 2381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2383 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->hd=Cyc_yyget_YY42(yyyyvsp[0]);_tmp6C3->tl=0;_tmp6C3;}));
goto _LL260;}case 261: _LL465: _LL466: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2384 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4->hd=Cyc_yyget_YY42(yyyyvsp[1]);_tmp6C4->tl=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6C4;}));
goto _LL260;}case 262: _LL467: _LL468: {
# 2387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2388 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp6C6;_tmp6C6.tag=0;_tmp6C6.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6C6;});_tmp6C5;}));
goto _LL260;}case 263: _LL469: _LL46A: {
# 2391
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2389 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6C8;_tmp6C8.tag=1;_tmp6C8.f1=({struct _dyneither_ptr*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp6C9;});_tmp6C8;});_tmp6C7;}));
goto _LL260;}case 264: _LL46B: _LL46C: {
# 2392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2394 "parse.y"
struct _tuple25 _tmp6CA=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6CB=_tmp6CA;struct Cyc_Absyn_Tqual _tmp6CC;struct Cyc_Parse_Type_specifier _tmp6CD;struct Cyc_List_List*_tmp6CE;_LL6C5: _tmp6CC=_tmp6CB.f1;_tmp6CD=_tmp6CB.f2;_tmp6CE=_tmp6CB.f3;_LL6C6:;{
void*_tmp6CF=Cyc_Parse_speclist2typ(_tmp6CD,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6CE != 0)
({void*_tmp6D0=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,({const char*_tmp6D1="ignoring attributes in type";_tag_dyneither(_tmp6D1,sizeof(char),28);}),_tag_dyneither(_tmp6D0,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2->f1=0;_tmp6D2->f2=_tmp6CC;_tmp6D2->f3=_tmp6CF;_tmp6D2;}));
# 2400
goto _LL260;};}case 265: _LL46D: _LL46E: {
# 2402
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2404
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2401 "parse.y"
struct _tuple25 _tmp6D3=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6D4=_tmp6D3;struct Cyc_Absyn_Tqual _tmp6D5;struct Cyc_Parse_Type_specifier _tmp6D6;struct Cyc_List_List*_tmp6D7;_LL6C8: _tmp6D5=_tmp6D4.f1;_tmp6D6=_tmp6D4.f2;_tmp6D7=_tmp6D4.f3;_LL6C9:;{
void*_tmp6D8=Cyc_Parse_speclist2typ(_tmp6D6,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6D9=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6DA=Cyc_Parse_apply_tms(_tmp6D5,_tmp6D8,_tmp6D7,_tmp6D9);
if(_tmp6DA.f3 != 0)
# 2407
({void*_tmp6DB=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,({const char*_tmp6DC="bad type params, ignoring";_tag_dyneither(_tmp6DC,sizeof(char),26);}),_tag_dyneither(_tmp6DB,sizeof(void*),0));});
if(_tmp6DA.f4 != 0)
({void*_tmp6DD=0;Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,({const char*_tmp6DE="bad specifiers, ignoring";_tag_dyneither(_tmp6DE,sizeof(char),25);}),_tag_dyneither(_tmp6DD,sizeof(void*),0));});
yyval=Cyc_YY37(({struct _tuple8*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->f1=0;_tmp6DF->f2=_tmp6DA.f1;_tmp6DF->f3=_tmp6DA.f2;_tmp6DF;}));
# 2412
goto _LL260;};}case 266: _LL46F: _LL470: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2415 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 267: _LL471: _LL472: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2416 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6E1;_tmp6E1.tag=24;_tmp6E1.f1=0;_tmp6E1;});_tmp6E0;}));
goto _LL260;}case 268: _LL473: _LL474: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2417 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6E3;_tmp6E3.tag=24;_tmp6E3.f1=Cyc_yyget_YY40(yyyyvsp[1]);_tmp6E3;});_tmp6E2;}));
goto _LL260;}case 269: _LL475: _LL476: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2418 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6E5;_tmp6E5.tag=25;_tmp6E5.f1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6E5;});_tmp6E4;}));
goto _LL260;}case 270: _LL477: _LL478: {
# 2421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2419 "parse.y"
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6E7;_tmp6E7.tag=24;_tmp6E7.f1=({struct Cyc_List_List*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->hd=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6E8->tl=Cyc_yyget_YY40(yyyyvsp[2]);_tmp6E8;});_tmp6E7;});_tmp6E6;}));
goto _LL260;}case 271: _LL479: _LL47A: {
# 2422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->hd=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6E9->tl=0;_tmp6E9;}));
goto _LL260;}case 272: _LL47B: _LL47C: {
# 2428
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2430
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2426 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));_tmp6EA->hd=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6EA->tl=Cyc_yyget_YY40(yyyyvsp[0]);_tmp6EA;}));
goto _LL260;}case 273: _LL47D: _LL47E: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2431 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6EB;_tmp6EB.tms=Cyc_yyget_YY26(yyyyvsp[0]);_tmp6EB;}));
goto _LL260;}case 274: _LL47F: _LL480: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2433 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 275: _LL481: _LL482: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2435 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6EC;_tmp6EC.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms);_tmp6EC;}));
goto _LL260;}case 276: _LL483: _LL484: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2440 "parse.y"
yyval=yyyyvsp[1];
goto _LL260;}case 277: _LL485: _LL486: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2442 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6ED;_tmp6ED.tms=({struct Cyc_List_List*_tmp6EE=_region_malloc(yyr,sizeof(*_tmp6EE));_tmp6EE->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp6EF=_region_malloc(yyr,sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp6F0;_tmp6F0.tag=0;_tmp6F0.f1=Cyc_yyget_YY51(yyyyvsp[2]);_tmp6F0.f2=(unsigned int)(yyyylsp[2]).first_line;_tmp6F0;});_tmp6EF;});_tmp6EE->tl=0;_tmp6EE;});_tmp6ED;}));
goto _LL260;}case 278: _LL487: _LL488: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2444 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6F1;_tmp6F1.tms=({struct Cyc_List_List*_tmp6F2=_region_malloc(yyr,sizeof(*_tmp6F2));_tmp6F2->hd=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp6F3=_region_malloc(yyr,sizeof(*_tmp6F3));_tmp6F3[0]=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp6F4;_tmp6F4.tag=0;_tmp6F4.f1=Cyc_yyget_YY51(yyyyvsp[3]);_tmp6F4.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp6F4;});_tmp6F3;});_tmp6F2->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp6F2;});_tmp6F1;}));
goto _LL260;}case 279: _LL489: _LL48A: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2446 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6F5;_tmp6F5.tms=({struct Cyc_List_List*_tmp6F6=_region_malloc(yyr,sizeof(*_tmp6F6));_tmp6F6->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp6F7=_region_malloc(yyr,sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp6F8;_tmp6F8.tag=1;_tmp6F8.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6F8.f2=Cyc_yyget_YY51(yyyyvsp[3]);_tmp6F8.f3=(unsigned int)(yyyylsp[3]).first_line;_tmp6F8;});_tmp6F7;});_tmp6F6->tl=0;_tmp6F6;});_tmp6F5;}));
goto _LL260;}case 280: _LL48B: _LL48C: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2448 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6F9;_tmp6F9.tms=({struct Cyc_List_List*_tmp6FA=_region_malloc(yyr,sizeof(*_tmp6FA));_tmp6FA->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp6FB=_region_malloc(yyr,sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp6FC;_tmp6FC.tag=1;_tmp6FC.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp6FC.f2=Cyc_yyget_YY51(yyyyvsp[4]);_tmp6FC.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp6FC;});_tmp6FB;});_tmp6FA->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp6FA;});_tmp6F9;}));
# 2451
goto _LL260;}case 281: _LL48D: _LL48E: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2452 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6FD;_tmp6FD.tms=({struct Cyc_List_List*_tmp6FE=_region_malloc(yyr,sizeof(*_tmp6FE));_tmp6FE->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp6FF=_region_malloc(yyr,sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp700;_tmp700.tag=3;_tmp700.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp701=_region_malloc(yyr,sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp702;_tmp702.tag=1;_tmp702.f1=0;_tmp702.f2=0;_tmp702.f3=0;_tmp702.f4=Cyc_yyget_YY49(yyyyvsp[1]);_tmp702.f5=Cyc_yyget_YY50(yyyyvsp[2]);_tmp702.f6=Cyc_yyget_YY55(yyyyvsp[4]);_tmp702.f7=Cyc_yyget_YY55(yyyyvsp[5]);_tmp702;});_tmp701;});_tmp700;});_tmp6FF;});_tmp6FE->tl=0;_tmp6FE;});_tmp6FD;}));
# 2454
goto _LL260;}case 282: _LL48F: _LL490: {
# 2456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2455 "parse.y"
struct _tuple26*_tmp703=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp704=_tmp703;struct Cyc_List_List*_tmp705;int _tmp706;struct Cyc_Absyn_VarargInfo*_tmp707;void*_tmp708;struct Cyc_List_List*_tmp709;_LL6CB: _tmp705=_tmp704->f1;_tmp706=_tmp704->f2;_tmp707=_tmp704->f3;_tmp708=_tmp704->f4;_tmp709=_tmp704->f5;_LL6CC:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70A;_tmp70A.tms=({struct Cyc_List_List*_tmp70B=_region_malloc(yyr,sizeof(*_tmp70B));_tmp70B->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp70C=_region_malloc(yyr,sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp70D;_tmp70D.tag=3;_tmp70D.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp70E=_region_malloc(yyr,sizeof(*_tmp70E));_tmp70E[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp70F;_tmp70F.tag=1;_tmp70F.f1=_tmp705;_tmp70F.f2=_tmp706;_tmp70F.f3=_tmp707;_tmp70F.f4=_tmp708;_tmp70F.f5=_tmp709;_tmp70F.f6=Cyc_yyget_YY55(yyyyvsp[3]);_tmp70F.f7=Cyc_yyget_YY55(yyyyvsp[4]);_tmp70F;});_tmp70E;});_tmp70D;});_tmp70C;});_tmp70B->tl=0;_tmp70B;});_tmp70A;}));
# 2458
goto _LL260;}case 283: _LL491: _LL492: {
# 2460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2459 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp710;_tmp710.tms=({struct Cyc_List_List*_tmp711=_region_malloc(yyr,sizeof(*_tmp711));_tmp711->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp712=_region_malloc(yyr,sizeof(*_tmp712));_tmp712[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp713;_tmp713.tag=3;_tmp713.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp714=_region_malloc(yyr,sizeof(*_tmp714));_tmp714[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp715;_tmp715.tag=1;_tmp715.f1=0;_tmp715.f2=0;_tmp715.f3=0;_tmp715.f4=Cyc_yyget_YY49(yyyyvsp[2]);_tmp715.f5=Cyc_yyget_YY50(yyyyvsp[3]);_tmp715.f6=Cyc_yyget_YY55(yyyyvsp[5]);_tmp715.f7=Cyc_yyget_YY55(yyyyvsp[6]);_tmp715;});_tmp714;});_tmp713;});_tmp712;});_tmp711->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp711;});_tmp710;}));
# 2462
goto _LL260;}case 284: _LL493: _LL494: {
# 2464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2463 "parse.y"
struct _tuple26*_tmp716=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp717=_tmp716;struct Cyc_List_List*_tmp718;int _tmp719;struct Cyc_Absyn_VarargInfo*_tmp71A;void*_tmp71B;struct Cyc_List_List*_tmp71C;_LL6CE: _tmp718=_tmp717->f1;_tmp719=_tmp717->f2;_tmp71A=_tmp717->f3;_tmp71B=_tmp717->f4;_tmp71C=_tmp717->f5;_LL6CF:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp71D;_tmp71D.tms=({struct Cyc_List_List*_tmp71E=_region_malloc(yyr,sizeof(*_tmp71E));_tmp71E->hd=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp71F=_region_malloc(yyr,sizeof(*_tmp71F));_tmp71F[0]=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp720;_tmp720.tag=3;_tmp720.f1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp721=_region_malloc(yyr,sizeof(*_tmp721));_tmp721[0]=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp722;_tmp722.tag=1;_tmp722.f1=_tmp718;_tmp722.f2=_tmp719;_tmp722.f3=_tmp71A;_tmp722.f4=_tmp71B;_tmp722.f5=_tmp71C;_tmp722.f6=
Cyc_yyget_YY55(yyyyvsp[4]);_tmp722.f7=Cyc_yyget_YY55(yyyyvsp[5]);_tmp722;});_tmp721;});_tmp720;});_tmp71F;});_tmp71E->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp71E;});_tmp71D;}));
# 2467
goto _LL260;}case 285: _LL495: _LL496: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2469 "parse.y"
struct Cyc_List_List*_tmp723=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp724;_tmp724.tms=({struct Cyc_List_List*_tmp725=_region_malloc(yyr,sizeof(*_tmp725));_tmp725->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp726=_region_malloc(yyr,sizeof(*_tmp726));_tmp726[0]=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp727;_tmp727.tag=4;_tmp727.f1=_tmp723;_tmp727.f2=(unsigned int)(yyyylsp[1]).first_line;_tmp727.f3=0;_tmp727;});_tmp726;});_tmp725->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp725;});_tmp724;}));
# 2473
goto _LL260;}case 286: _LL497: _LL498: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2474 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp728;_tmp728.tms=({struct Cyc_List_List*_tmp729=_region_malloc(yyr,sizeof(*_tmp729));_tmp729->hd=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp72A=_region_malloc(yyr,sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp72B;_tmp72B.tag=5;_tmp72B.f1=(unsigned int)(yyyylsp[1]).first_line;_tmp72B.f2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp72B;});_tmp72A;});_tmp729->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp729;});_tmp728;}));
# 2476
goto _LL260;}case 287: _LL499: _LL49A: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2480 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 288: _LL49B: _LL49C: {
# 2483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2481 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 289: _LL49D: _LL49E: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2482 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 290: _LL49F: _LL4A0: {
# 2485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2483 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 291: _LL4A1: _LL4A2: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2484 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 292: _LL4A3: _LL4A4: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2485 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 293: _LL4A5: _LL4A6: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2488 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmp72D;_tmp72D.tag=16;_tmp72D.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp72D;});_tmp72C;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 294: _LL4A7: _LL4A8: {
# 2491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp72E=_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp72F;_tmp72F.tag=13;_tmp72F.f1=({struct _dyneither_ptr*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp730;});_tmp72F.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp72F;});_tmp72E;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 295: _LL4A9: _LL4AA: {
# 2497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2498 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 296: _LL4AB: _LL4AC: {
# 2501
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2499 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 297: _LL4AD: _LL4AE: {
# 2502
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2504
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2504 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 298: _LL4AF: _LL4B0: {
# 2507
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2505 "parse.y"
yyval=yyyyvsp[1];
goto _LL260;}case 299: _LL4B1: _LL4B2: {
# 2508
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2510 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 300: _LL4B3: _LL4B4: {
# 2513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2511 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
goto _LL260;}case 301: _LL4B5: _LL4B6: {
# 2514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2512 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731[0]=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp732;_tmp732.tag=13;_tmp732.f1=({struct _dyneither_ptr*_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp733;});_tmp732.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0));_tmp732;});_tmp731;}),(unsigned int)(yyyylsp[0]).first_line));
# 2514
goto _LL260;}case 302: _LL4B7: _LL4B8: {
# 2516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2514 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp734=_cycalloc(sizeof(*_tmp734));_tmp734[0]=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp735;_tmp735.tag=13;_tmp735.f1=({struct _dyneither_ptr*_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp736;});_tmp735.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]));_tmp735;});_tmp734;}),(unsigned int)(yyyylsp[0]).first_line));
# 2516
goto _LL260;}case 303: _LL4B9: _LL4BA: {
# 2518
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2516 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 304: _LL4BB: _LL4BC: {
# 2519
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2517 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 305: _LL4BD: _LL4BE: {
# 2520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2518 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737[0]=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp738;_tmp738.tag=1;_tmp738.f1=Cyc_yyget_YY15(yyyyvsp[0]);_tmp738;});_tmp737;}),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));
goto _LL260;}case 306: _LL4BF: _LL4C0: {
# 2522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2521 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739[0]=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp73A;_tmp73A.tag=1;_tmp73A.f1=Cyc_yyget_YY15(yyyyvsp[0]);_tmp73A;});_tmp739;}),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
goto _LL260;}case 307: _LL4C1: _LL4C2: {
# 2524
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2526 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 308: _LL4C3: _LL4C4: {
# 2529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2528 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 309: _LL4C5: _LL4C6: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 310: _LL4C7: _LL4C8: {
# 2537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2537 "parse.y"
struct Cyc_Absyn_Exp*_tmp73B=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp73B,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 311: _LL4C9: _LL4CA: {
# 2541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2541 "parse.y"
struct Cyc_Absyn_Exp*_tmp73C=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp73C,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2544
goto _LL260;}case 312: _LL4CB: _LL4CC: {
# 2546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2547 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 313: _LL4CD: _LL4CE:
# 2550
 yyval=Cyc_YY8(0);
goto _LL260;case 314: _LL4CF: _LL4D0: {
# 2553
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2555
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2565 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D->hd=({struct Cyc_Absyn_Switch_clause*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp73E->pat_vars=0;_tmp73E->where_clause=0;_tmp73E->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp73E->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp73E;});_tmp73D->tl=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp73D;}));
goto _LL260;}case 315: _LL4D1: _LL4D2: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2569 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F->hd=({struct Cyc_Absyn_Switch_clause*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp740->pat_vars=0;_tmp740->where_clause=0;_tmp740->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp740->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp740;});_tmp73F->tl=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp73F;}));
goto _LL260;}case 316: _LL4D3: _LL4D4: {
# 2574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2573 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->hd=({struct Cyc_Absyn_Switch_clause*_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp742->pat_vars=0;_tmp742->where_clause=0;_tmp742->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp742->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp742;});_tmp741->tl=Cyc_yyget_YY8(yyyyvsp[4]);_tmp741;}));
goto _LL260;}case 317: _LL4D5: _LL4D6: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2575 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743->hd=({struct Cyc_Absyn_Switch_clause*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp744->pat_vars=0;_tmp744->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp744->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp744->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp744;});_tmp743->tl=
Cyc_yyget_YY8(yyyyvsp[5]);_tmp743;}));
goto _LL260;}case 318: _LL4D7: _LL4D8: {
# 2580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2579 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->hd=({struct Cyc_Absyn_Switch_clause*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->pattern=Cyc_yyget_YY9(yyyyvsp[1]);_tmp746->pat_vars=0;_tmp746->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp746->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp746->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp746;});_tmp745->tl=Cyc_yyget_YY8(yyyyvsp[6]);_tmp745;}));
goto _LL260;}case 319: _LL4D9: _LL4DA: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2586 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 320: _LL4DB: _LL4DC: {
# 2589
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2591
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2590 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 321: _LL4DD: _LL4DE: {
# 2593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2594 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 322: _LL4DF: _LL4E0: {
# 2598
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2597 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 323: _LL4E1: _LL4E2: {
# 2601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2600 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 324: _LL4E3: _LL4E4: {
# 2604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2603 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 325: _LL4E5: _LL4E6: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2606 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 326: _LL4E7: _LL4E8: {
# 2610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2609 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 327: _LL4E9: _LL4EA: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2612 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 328: _LL4EB: _LL4EC: {
# 2616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2615 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 329: _LL4ED: _LL4EE: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2618 "parse.y"
struct Cyc_List_List*_tmp747=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp748=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp747,_tmp748));
# 2623
goto _LL260;}case 330: _LL4EF: _LL4F0: {
# 2625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2624 "parse.y"
struct Cyc_List_List*_tmp749=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp74A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp749,_tmp74A));
# 2629
goto _LL260;}case 331: _LL4F1: _LL4F2: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2630 "parse.y"
struct Cyc_List_List*_tmp74B=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp74C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp74B,_tmp74C));
# 2635
goto _LL260;}case 332: _LL4F3: _LL4F4: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2636 "parse.y"
struct Cyc_List_List*_tmp74D=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp74E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp74D,_tmp74E));
# 2641
goto _LL260;}case 333: _LL4F5: _LL4F6: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(({struct _dyneither_ptr*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp74F;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 334: _LL4F7: _LL4F8: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2647 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 335: _LL4F9: _LL4FA: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2648 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 336: _LL4FB: _LL4FC: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2649 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 337: _LL4FD: _LL4FE: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2650 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 338: _LL4FF: _LL500: {
# 2653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2652 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 339: _LL501: _LL502: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2653 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 340: _LL503: _LL504: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2655 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 341: _LL505: _LL506: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2664 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 342: _LL507: _LL508: {
# 2667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2667 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 343: _LL509: _LL50A: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2669 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 344: _LL50B: _LL50C: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2672 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 345: _LL50D: _LL50E: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2674 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 346: _LL50F: _LL510: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2677 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 347: _LL511: _LL512: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2679 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 348: _LL513: _LL514: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2682 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 349: _LL515: _LL516: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2684 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 350: _LL517: _LL518: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2687 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 351: _LL519: _LL51A: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2689 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 352: _LL51B: _LL51C: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2692 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 353: _LL51D: _LL51E: {
# 2695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2694 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 354: _LL51F: _LL520: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2697 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 355: _LL521: _LL522: {
# 2700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2699 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 356: _LL523: _LL524: {
# 2702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2701 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 357: _LL525: _LL526: {
# 2704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2704 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 358: _LL527: _LL528: {
# 2707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2706 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 359: _LL529: _LL52A: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2708 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 360: _LL52B: _LL52C: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2710 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 361: _LL52D: _LL52E: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2712 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 362: _LL52F: _LL530: {
# 2715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2715 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 363: _LL531: _LL532: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2717 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 364: _LL533: _LL534: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2719 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 365: _LL535: _LL536: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2722 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 366: _LL537: _LL538: {
# 2725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2724 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 367: _LL539: _LL53A: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2726 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 368: _LL53B: _LL53C: {
# 2729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2729 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 369: _LL53D: _LL53E: {
# 2732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2731 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 370: _LL53F: _LL540: {
# 2734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2733 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 371: _LL541: _LL542: {
# 2736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2735 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 372: _LL543: _LL544: {
# 2738
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2740
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2738 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 373: _LL545: _LL546: {
# 2741
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2740 "parse.y"
void*_tmp750=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp750,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2743
goto _LL260;}case 374: _LL547: _LL548: {
# 2745
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2746 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 375: _LL549: _LL54A: {
# 2749
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2749 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL260;}case 376: _LL54B: _LL54C: {
# 2752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2751 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 377: _LL54D: _LL54E: {
# 2754
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2753 "parse.y"
void*_tmp751=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp751,(unsigned int)(yyyylsp[0]).first_line)));
# 2756
goto _LL260;}case 378: _LL54F: _LL550: {
# 2758
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2757 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 379: _LL551: _LL552: {
# 2760
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2759 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
goto _LL260;}case 380: _LL553: _LL554: {
# 2762
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2764 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 381: _LL555: _LL556: {
# 2767
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2769
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2773 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 382: _LL557: _LL558: {
# 2776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2778 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 383: _LL559: _LL55A: {
# 2781
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2783
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2780 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL260;}case 384: _LL55B: _LL55C: {
# 2783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2782 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp752=e->r;void*_tmp753=_tmp752;struct _dyneither_ptr _tmp754;int _tmp755;enum Cyc_Absyn_Sign _tmp756;int _tmp757;enum Cyc_Absyn_Sign _tmp758;short _tmp759;enum Cyc_Absyn_Sign _tmp75A;char _tmp75B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).LongLong_c).tag){case 2: _LL6D1: _tmp75A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Char_c).val).f1;_tmp75B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Char_c).val).f2;_LL6D2:
# 2786
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp75C=_cycalloc_atomic(sizeof(*_tmp75C));_tmp75C[0]=({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmp75D;_tmp75D.tag=11;_tmp75D.f1=_tmp75B;_tmp75D;});_tmp75C;}),e->loc));goto _LL6D0;case 4: _LL6D3: _tmp758=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Short_c).val).f1;_tmp759=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Short_c).val).f2;_LL6D4:
# 2788
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp75E=_cycalloc_atomic(sizeof(*_tmp75E));_tmp75E[0]=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp75F;_tmp75F.tag=10;_tmp75F.f1=_tmp758;_tmp75F.f2=(int)_tmp759;_tmp75F;});_tmp75E;}),e->loc));goto _LL6D0;case 5: _LL6D5: _tmp756=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Int_c).val).f1;_tmp757=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Int_c).val).f2;_LL6D6:
# 2790
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp760=_cycalloc_atomic(sizeof(*_tmp760));_tmp760[0]=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp761;_tmp761.tag=10;_tmp761.f1=_tmp756;_tmp761.f2=_tmp757;_tmp761;});_tmp760;}),e->loc));goto _LL6D0;case 7: _LL6D7: _tmp754=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Float_c).val).f1;_tmp755=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp753)->f1).Float_c).val).f2;_LL6D8:
# 2792
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762[0]=({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmp763;_tmp763.tag=12;_tmp763.f1=_tmp754;_tmp763.f2=_tmp755;_tmp763;});_tmp762;}),e->loc));goto _LL6D0;case 1: _LL6D9: _LL6DA:
# 2794
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL6D0;case 8: _LL6DB: _LL6DC:
# 2796
 Cyc_Parse_err(({const char*_tmp764="strings cannot occur within patterns";_tag_dyneither(_tmp764,sizeof(char),37);}),(unsigned int)(yyyylsp[0]).first_line);goto _LL6D0;case 9: _LL6DD: _LL6DE:
# 2798
 Cyc_Parse_err(({const char*_tmp765="strings cannot occur within patterns";_tag_dyneither(_tmp765,sizeof(char),37);}),(unsigned int)(yyyylsp[0]).first_line);goto _LL6D0;case 6: _LL6DF: _LL6E0:
# 2800
 Cyc_Parse_unimp(({const char*_tmp766="long long's in patterns";_tag_dyneither(_tmp766,sizeof(char),24);}),(unsigned int)(yyyylsp[0]).first_line);goto _LL6D0;default: goto _LL6E1;}else{_LL6E1: _LL6E2:
# 2802
 Cyc_Parse_err(({const char*_tmp767="bad constant in case";_tag_dyneither(_tmp767,sizeof(char),21);}),(unsigned int)(yyyylsp[0]).first_line);}_LL6D0:;}
# 2805
goto _LL260;}case 385: _LL55D: _LL55E: {
# 2807
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2809
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2806 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768[0]=({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmp769;_tmp769.tag=15;_tmp769.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp769;});_tmp768;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 386: _LL55F: _LL560: {
# 2809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2808 "parse.y"
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),({const char*_tmp76A="as";_tag_dyneither(_tmp76A,sizeof(char),3);}))!= 0)
Cyc_Parse_err(({const char*_tmp76B="expecting `as'";_tag_dyneither(_tmp76B,sizeof(char),15);}),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp76D;_tmp76D.tag=1;_tmp76D.f1=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E->f1=Cyc_Absyn_Loc_n;_tmp76E->f2=({struct _dyneither_ptr*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp76F;});_tmp76E;}),(void*)& Cyc_Absyn_VoidType_val,0);_tmp76D.f2=
Cyc_yyget_YY9(yyyyvsp[2]);_tmp76D;});_tmp76C;}),(unsigned int)(yyyylsp[0]).first_line));
# 2813
goto _LL260;}case 387: _LL561: _LL562: {
# 2815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2814 "parse.y"
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),({const char*_tmp770="alias";_tag_dyneither(_tmp770,sizeof(char),6);}))!= 0)
Cyc_Parse_err(({const char*_tmp771="expecting `alias'";_tag_dyneither(_tmp771,sizeof(char),18);}),(unsigned int)(yyyylsp[1]).first_line);{
int _tmp772=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777->name=({struct _dyneither_ptr*_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp77A;});_tmp777->identity=- 1;_tmp777->kind=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp779;_tmp779.tag=0;_tmp779.f1=& Cyc_Tcutil_rk;_tmp779;});_tmp778;});_tmp777;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775->f1=Cyc_Absyn_Loc_n;_tmp775->f2=({struct _dyneither_ptr*_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776[0]=Cyc_yyget_String_tok(yyyyvsp[5]);_tmp776;});_tmp775;}),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773[0]=({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp774;_tmp774.tag=2;_tmp774.f1=tv;_tmp774.f2=vd;_tmp774;});_tmp773;}),(unsigned int)_tmp772));
# 2822
goto _LL260;};}case 388: _LL563: _LL564: {
# 2824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2823 "parse.y"
struct _tuple22 _tmp77B=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp77C=_tmp77B;struct Cyc_List_List*_tmp77D;int _tmp77E;_LL6E4: _tmp77D=_tmp77C.f1;_tmp77E=_tmp77C.f2;_LL6E5:;
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmp780;_tmp780.tag=5;_tmp780.f1=_tmp77D;_tmp780.f2=_tmp77E;_tmp780;});_tmp77F;}),(unsigned int)(yyyylsp[0]).first_line));
# 2826
goto _LL260;}case 389: _LL565: _LL566: {
# 2828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2827 "parse.y"
struct _tuple22 _tmp781=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp782=_tmp781;struct Cyc_List_List*_tmp783;int _tmp784;_LL6E7: _tmp783=_tmp782.f1;_tmp784=_tmp782.f2;_LL6E8:;
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785[0]=({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmp786;_tmp786.tag=16;_tmp786.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp786.f2=_tmp783;_tmp786.f3=_tmp784;_tmp786;});_tmp785;}),(unsigned int)(yyyylsp[0]).first_line));
# 2830
goto _LL260;}case 390: _LL567: _LL568: {
# 2832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2831 "parse.y"
struct _tuple22 _tmp787=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp788=_tmp787;struct Cyc_List_List*_tmp789;int _tmp78A;_LL6EA: _tmp789=_tmp788.f1;_tmp78A=_tmp788.f2;_LL6EB:;{
struct Cyc_List_List*_tmp78B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp78D;_tmp78D.tag=7;_tmp78D.f1=({struct Cyc_Absyn_AggrInfo*_tmp78E=_cycalloc(sizeof(*_tmp78E));_tmp78E->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);_tmp78E->targs=0;_tmp78E;});_tmp78D.f2=_tmp78B;_tmp78D.f3=_tmp789;_tmp78D.f4=_tmp78A;_tmp78D;});_tmp78C;}),(unsigned int)(yyyylsp[0]).first_line));
# 2836
goto _LL260;};}case 391: _LL569: _LL56A: {
# 2838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2837 "parse.y"
struct _tuple22 _tmp78F=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp790=_tmp78F;struct Cyc_List_List*_tmp791;int _tmp792;_LL6ED: _tmp791=_tmp790.f1;_tmp792=_tmp790.f2;_LL6EE:;{
struct Cyc_List_List*_tmp793=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp794=_cycalloc(sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp795;_tmp795.tag=7;_tmp795.f1=0;_tmp795.f2=_tmp793;_tmp795.f3=_tmp791;_tmp795.f4=_tmp792;_tmp795;});_tmp794;}),(unsigned int)(yyyylsp[0]).first_line));
# 2841
goto _LL260;};}case 392: _LL56B: _LL56C: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2842 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp797;_tmp797.tag=6;_tmp797.f1=Cyc_yyget_YY9(yyyyvsp[1]);_tmp797;});_tmp796;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 393: _LL56D: _LL56E: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2844 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[0]=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp799;_tmp799.tag=6;_tmp799.f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A[0]=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp79B;_tmp79B.tag=6;_tmp79B.f1=Cyc_yyget_YY9(yyyyvsp[1]);_tmp79B;});_tmp79A;}),(unsigned int)(yyyylsp[0]).first_line);_tmp799;});_tmp798;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 394: _LL56F: _LL570: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2846 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C[0]=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp79D;_tmp79D.tag=3;_tmp79D.f1=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E->f1=Cyc_Absyn_Loc_n;_tmp79E->f2=({struct _dyneither_ptr*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp79F;});_tmp79E;}),(void*)& Cyc_Absyn_VoidType_val,0);_tmp79D.f2=
# 2848
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp79D;});_tmp79C;}),(unsigned int)(yyyylsp[0]).first_line));
# 2850
goto _LL260;}case 395: _LL571: _LL572: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2851 "parse.y"
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),({const char*_tmp7A0="as";_tag_dyneither(_tmp7A0,sizeof(char),3);}))!= 0)
Cyc_Parse_err(({const char*_tmp7A1="expecting `as'";_tag_dyneither(_tmp7A1,sizeof(char),15);}),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp7A3;_tmp7A3.tag=3;_tmp7A3.f1=Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));_tmp7A4->f1=Cyc_Absyn_Loc_n;_tmp7A4->f2=({struct _dyneither_ptr*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5[0]=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp7A5;});_tmp7A4;}),(void*)& Cyc_Absyn_VoidType_val,0);_tmp7A3.f2=
# 2855
Cyc_yyget_YY9(yyyyvsp[3]);_tmp7A3;});_tmp7A2;}),(unsigned int)(yyyylsp[0]).first_line));
# 2857
goto _LL260;}case 396: _LL573: _LL574: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2858 "parse.y"
void*_tmp7A6=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7[0]=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7A8;_tmp7A8.tag=4;_tmp7A8.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7A6);_tmp7A8.f2=
Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9->f1=Cyc_Absyn_Loc_n;_tmp7A9->f2=({struct _dyneither_ptr*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7AA;});_tmp7A9;}),(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7AC;_tmp7AC.tag=19;_tmp7AC.f1=_tmp7A6;_tmp7AC;});_tmp7AB;}),0);_tmp7A8;});_tmp7A7;}),(unsigned int)(yyyylsp[0]).first_line));
# 2863
goto _LL260;}case 397: _LL575: _LL576: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2864 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7AD=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7AF;_tmp7AF.tag=4;_tmp7AF.f1=_tmp7AD;_tmp7AF.f2=
Cyc_Absyn_new_vardecl(({struct _tuple0*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0->f1=Cyc_Absyn_Loc_n;_tmp7B0->f2=({struct _dyneither_ptr*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7B1;});_tmp7B0;}),(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));_tmp7B2[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7B3;_tmp7B3.tag=19;_tmp7B3.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7B5;_tmp7B5.tag=2;_tmp7B5.f1=_tmp7AD;_tmp7B5;});_tmp7B4;});_tmp7B3;});_tmp7B2;}),0);_tmp7AF;});_tmp7AE;}),(unsigned int)(yyyylsp[0]).first_line));
# 2869
goto _LL260;}case 398: _LL577: _LL578: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2872 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7B6->f2=0;_tmp7B6;}));
goto _LL260;}case 399: _LL579: _LL57A: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2873 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7B7->f2=1;_tmp7B7;}));
goto _LL260;}case 400: _LL57B: _LL57C: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2874 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->f1=0;_tmp7B8->f2=1;_tmp7B8;}));
goto _LL260;}case 401: _LL57D: _LL57E: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2879 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9->hd=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7B9->tl=0;_tmp7B9;}));
goto _LL260;}case 402: _LL57F: _LL580: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2881 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));_tmp7BA->hd=Cyc_yyget_YY9(yyyyvsp[2]);_tmp7BA->tl=Cyc_yyget_YY11(yyyyvsp[0]);_tmp7BA;}));
goto _LL260;}case 403: _LL581: _LL582: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2886 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB->f1=0;_tmp7BB->f2=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7BB;}));
goto _LL260;}case 404: _LL583: _LL584: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2888 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC->f1=Cyc_yyget_YY41(yyyyvsp[0]);_tmp7BC->f2=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7BC;}));
goto _LL260;}case 405: _LL585: _LL586: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2891 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7BD->f2=0;_tmp7BD;}));
goto _LL260;}case 406: _LL587: _LL588: {
# 2894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2892 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));_tmp7BE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7BE->f2=1;_tmp7BE;}));
goto _LL260;}case 407: _LL589: _LL58A: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2893 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF->f1=0;_tmp7BF->f2=1;_tmp7BF;}));
goto _LL260;}case 408: _LL58B: _LL58C: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2898 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));_tmp7C0->hd=Cyc_yyget_YY12(yyyyvsp[0]);_tmp7C0->tl=0;_tmp7C0;}));
goto _LL260;}case 409: _LL58D: _LL58E: {
# 2901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2900 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1->hd=Cyc_yyget_YY12(yyyyvsp[2]);_tmp7C1->tl=Cyc_yyget_YY13(yyyyvsp[0]);_tmp7C1;}));
goto _LL260;}case 410: _LL58F: _LL590: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
goto _LL260;}case 411: _LL591: _LL592: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2908 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 412: _LL593: _LL594: {
# 2911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2913 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 413: _LL595: _LL596: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2915 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 414: _LL597: _LL598: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2917 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 415: _LL599: _LL59A: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2921 "parse.y"
yyval=Cyc_YY7(0);
goto _LL260;}case 416: _LL59B: _LL59C: {
# 2924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2922 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C2=_cycalloc_atomic(sizeof(*_tmp7C2));_tmp7C2->v=(void*)Cyc_Absyn_Times;_tmp7C2;}));
goto _LL260;}case 417: _LL59D: _LL59E: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2923 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C3=_cycalloc_atomic(sizeof(*_tmp7C3));_tmp7C3->v=(void*)Cyc_Absyn_Div;_tmp7C3;}));
goto _LL260;}case 418: _LL59F: _LL5A0: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2924 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C4=_cycalloc_atomic(sizeof(*_tmp7C4));_tmp7C4->v=(void*)Cyc_Absyn_Mod;_tmp7C4;}));
goto _LL260;}case 419: _LL5A1: _LL5A2: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2925 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C5=_cycalloc_atomic(sizeof(*_tmp7C5));_tmp7C5->v=(void*)Cyc_Absyn_Plus;_tmp7C5;}));
goto _LL260;}case 420: _LL5A3: _LL5A4: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2926 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C6=_cycalloc_atomic(sizeof(*_tmp7C6));_tmp7C6->v=(void*)Cyc_Absyn_Minus;_tmp7C6;}));
goto _LL260;}case 421: _LL5A5: _LL5A6: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2927 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C7=_cycalloc_atomic(sizeof(*_tmp7C7));_tmp7C7->v=(void*)Cyc_Absyn_Bitlshift;_tmp7C7;}));
goto _LL260;}case 422: _LL5A7: _LL5A8: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2928 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C8=_cycalloc_atomic(sizeof(*_tmp7C8));_tmp7C8->v=(void*)Cyc_Absyn_Bitlrshift;_tmp7C8;}));
goto _LL260;}case 423: _LL5A9: _LL5AA: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2929 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7C9=_cycalloc_atomic(sizeof(*_tmp7C9));_tmp7C9->v=(void*)Cyc_Absyn_Bitand;_tmp7C9;}));
goto _LL260;}case 424: _LL5AB: _LL5AC: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2930 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7CA=_cycalloc_atomic(sizeof(*_tmp7CA));_tmp7CA->v=(void*)Cyc_Absyn_Bitxor;_tmp7CA;}));
goto _LL260;}case 425: _LL5AD: _LL5AE: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2931 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7CB=_cycalloc_atomic(sizeof(*_tmp7CB));_tmp7CB->v=(void*)Cyc_Absyn_Bitor;_tmp7CB;}));
goto _LL260;}case 426: _LL5AF: _LL5B0: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2936 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 427: _LL5B1: _LL5B2: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2938 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 428: _LL5B3: _LL5B4: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2941 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 429: _LL5B5: _LL5B6: {
# 2944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2944 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 430: _LL5B7: _LL5B8: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2946 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 431: _LL5B9: _LL5BA: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2948 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 432: _LL5BB: _LL5BC: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2950 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 433: _LL5BD: _LL5BE: {
# 2953
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2954 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 434: _LL5BF: _LL5C0: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2958 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 435: _LL5C1: _LL5C2: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2960 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 436: _LL5C3: _LL5C4: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2964 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 437: _LL5C5: _LL5C6: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2966 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 438: _LL5C7: _LL5C8: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2970 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 439: _LL5C9: _LL5CA: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2972 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 440: _LL5CB: _LL5CC: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2976 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 441: _LL5CD: _LL5CE: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2978 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 442: _LL5CF: _LL5D0: {
# 2981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2982 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 443: _LL5D1: _LL5D2: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2984 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 444: _LL5D3: _LL5D4: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2988 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 445: _LL5D5: _LL5D6: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2990 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 446: _LL5D7: _LL5D8: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2992 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 447: _LL5D9: _LL5DA: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2996 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 448: _LL5DB: _LL5DC: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2998 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 449: _LL5DD: _LL5DE: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3000 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 450: _LL5DF: _LL5E0: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3002 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 451: _LL5E1: _LL5E2: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3004 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 452: _LL5E3: _LL5E4: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3008 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 453: _LL5E5: _LL5E6: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3010 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 454: _LL5E7: _LL5E8: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3012 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 455: _LL5E9: _LL5EA: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3016 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 456: _LL5EB: _LL5EC: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3018 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 457: _LL5ED: _LL5EE: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3020 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 458: _LL5EF: _LL5F0: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3024 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 459: _LL5F1: _LL5F2: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3026 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 460: _LL5F3: _LL5F4: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3028 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 461: _LL5F5: _LL5F6: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3030 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 462: _LL5F7: _LL5F8: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3034 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 463: _LL5F9: _LL5FA: {
# 3037
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3036 "parse.y"
void*_tmp7CC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp7CC,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 3039
goto _LL260;}case 464: _LL5FB: _LL5FC: {
# 3041
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3042 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 465: _LL5FD: _LL5FE: {
# 3045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3043 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 466: _LL5FF: _LL600: {
# 3046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3044 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 467: _LL601: _LL602: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3045 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 468: _LL603: _LL604: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3046 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 469: _LL605: _LL606: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3047 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 470: _LL607: _LL608: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3048 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 471: _LL609: _LL60A: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3050 "parse.y"
void*_tmp7CD=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp7CD,(unsigned int)(yyyylsp[0]).first_line));
# 3053
goto _LL260;}case 472: _LL60B: _LL60C: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3053 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 473: _LL60D: _LL60E: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3055 "parse.y"
void*_tmp7CE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp7CE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3058
goto _LL260;}case 474: _LL60F: _LL610: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3060 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7D0;_tmp7D0.tag=33;_tmp7D0.f1=({struct Cyc_Absyn_MallocInfo _tmp7D1;_tmp7D1.is_calloc=0;_tmp7D1.rgn=0;_tmp7D1.elt_type=0;_tmp7D1.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7D1.fat_result=0;_tmp7D1.inline_call=0;_tmp7D1;});_tmp7D0;});_tmp7CF;}),(unsigned int)(yyyylsp[0]).first_line));
# 3062
goto _LL260;}case 475: _LL611: _LL612: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3063 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7D3;_tmp7D3.tag=33;_tmp7D3.f1=({struct Cyc_Absyn_MallocInfo _tmp7D4;_tmp7D4.is_calloc=0;_tmp7D4.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7D4.elt_type=0;_tmp7D4.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7D4.fat_result=0;_tmp7D4.inline_call=0;_tmp7D4;});_tmp7D3;});_tmp7D2;}),(unsigned int)(yyyylsp[0]).first_line));
# 3065
goto _LL260;}case 476: _LL613: _LL614: {
# 3067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3066 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7D6;_tmp7D6.tag=33;_tmp7D6.f1=({struct Cyc_Absyn_MallocInfo _tmp7D7;_tmp7D7.is_calloc=0;_tmp7D7.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7D7.elt_type=0;_tmp7D7.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7D7.fat_result=0;_tmp7D7.inline_call=1;_tmp7D7;});_tmp7D6;});_tmp7D5;}),(unsigned int)(yyyylsp[0]).first_line));
# 3068
goto _LL260;}case 477: _LL615: _LL616: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3069 "parse.y"
void*_tmp7D8=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));_tmp7D9[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7DA;_tmp7DA.tag=33;_tmp7DA.f1=({struct Cyc_Absyn_MallocInfo _tmp7DB;_tmp7DB.is_calloc=1;_tmp7DB.rgn=0;_tmp7DB.elt_type=({void**_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC[0]=_tmp7D8;_tmp7DC;});_tmp7DB.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7DB.fat_result=0;_tmp7DB.inline_call=0;_tmp7DB;});_tmp7DA;});_tmp7D9;}),(unsigned int)(yyyylsp[0]).first_line));
# 3072
goto _LL260;}case 478: _LL617: _LL618: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3074 "parse.y"
void*_tmp7DD=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7DE=_cycalloc(sizeof(*_tmp7DE));_tmp7DE[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7DF;_tmp7DF.tag=33;_tmp7DF.f1=({struct Cyc_Absyn_MallocInfo _tmp7E0;_tmp7E0.is_calloc=1;_tmp7E0.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7E0.elt_type=({void**_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1[0]=_tmp7DD;_tmp7E1;});_tmp7E0.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7E0.fat_result=0;_tmp7E0.inline_call=0;_tmp7E0;});_tmp7DF;});_tmp7DE;}),(unsigned int)(yyyylsp[0]).first_line));
# 3077
goto _LL260;}case 479: _LL619: _LL61A: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3078 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,({struct Cyc_Absyn_Exp*_tmp7E2[1];_tmp7E2[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7E2,sizeof(struct Cyc_Absyn_Exp*),1));}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 480: _LL61B: _LL61C: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3080 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp7E4;_tmp7E4.tag=37;_tmp7E4.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7E4.f2=({struct _dyneither_ptr*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5[0]=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp7E5;});_tmp7E4;});_tmp7E3;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 481: _LL61D: _LL61E: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3082 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp7E7;_tmp7E7.tag=37;_tmp7E7.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);_tmp7E7.f2=({struct _dyneither_ptr*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8[0]=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp7E8;});_tmp7E7;});_tmp7E6;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 482: _LL61F: _LL620: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3084 "parse.y"
void*_tmp7E9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp7E9,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 483: _LL621: _LL622: {
# 3088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3087 "parse.y"
struct _tuple20 _tmp7EA=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp7EB=_tmp7EA;int _tmp7EC;struct _dyneither_ptr _tmp7ED;_LL6F0: _tmp7EC=_tmp7EB.f1;_tmp7ED=_tmp7EB.f2;_LL6F1:;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp7EC,_tmp7ED,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 484: _LL623: _LL624: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3092 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL260;}case 485: _LL625: _LL626: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3093 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL260;}case 486: _LL627: _LL628: {
# 3096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3094 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL260;}case 487: _LL629: _LL62A: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3099 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 488: _LL62B: _LL62C: {
# 3102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3101 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 489: _LL62D: _LL62E: {
# 3104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3103 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 490: _LL62F: _LL630: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3105 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 491: _LL631: _LL632: {
# 3108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3107 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),({struct _dyneither_ptr*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));_tmp7EE[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp7EE;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 492: _LL633: _LL634: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3109 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),({struct _dyneither_ptr*_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp7EF;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 493: _LL635: _LL636: {
# 3112
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3111 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 494: _LL637: _LL638: {
# 3114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3113 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 495: _LL639: _LL63A: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3115 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp7F1;_tmp7F1.tag=24;_tmp7F1.f1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp7F1.f2=0;_tmp7F1;});_tmp7F0;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 496: _LL63B: _LL63C: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3117 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp7F3;_tmp7F3.tag=24;_tmp7F3.f1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp7F3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp7F3;});_tmp7F2;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 497: _LL63D: _LL63E: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3119 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp7F4=_cycalloc(sizeof(*_tmp7F4));_tmp7F4[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp7F5;_tmp7F5.tag=24;_tmp7F5.f1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp7F5.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp7F5;});_tmp7F4;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 498: _LL63F: _LL640: {
# 3122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3124 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6->hd=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));_tmp7F7[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp7F8;_tmp7F8.tag=0;_tmp7F8.f1=({struct _dyneither_ptr*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7F9;});_tmp7F8;});_tmp7F7;});_tmp7F6->tl=0;_tmp7F6;}));
goto _LL260;}case 499: _LL641: _LL642: {
# 3127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3127 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA->hd=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp7FB=_cycalloc_atomic(sizeof(*_tmp7FB));_tmp7FB[0]=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp7FC;_tmp7FC.tag=1;_tmp7FC.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_Int_tok(yyyyvsp[0]));_tmp7FC;});_tmp7FB;});_tmp7FA->tl=0;_tmp7FA;}));
goto _LL260;}case 500: _LL643: _LL644: {
# 3130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3129 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD->hd=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp7FF;_tmp7FF.tag=0;_tmp7FF.f1=({struct _dyneither_ptr*_tmp800=_cycalloc(sizeof(*_tmp800));_tmp800[0]=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp800;});_tmp7FF;});_tmp7FE;});_tmp7FD->tl=Cyc_yyget_YY3(yyyyvsp[0]);_tmp7FD;}));
goto _LL260;}case 501: _LL645: _LL646: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3132 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801->hd=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp802=_cycalloc_atomic(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp803;_tmp803.tag=1;_tmp803.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_Int_tok(yyyyvsp[2]));_tmp803;});_tmp802;});_tmp801->tl=Cyc_yyget_YY3(yyyyvsp[0]);_tmp801;}));
goto _LL260;}case 502: _LL647: _LL648: {
# 3135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 503: _LL649: _LL64A: {
# 3141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3140 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 504: _LL64B: _LL64C: {
# 3143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3142 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 505: _LL64D: _LL64E: {
# 3145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3144 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 506: _LL64F: _LL650: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3146 "parse.y"
yyval=yyyyvsp[1];
goto _LL260;}case 507: _LL651: _LL652: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3151 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 508: _LL653: _LL654: {
# 3154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3153 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 509: _LL655: _LL656: {
# 3156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3156 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 510: _LL657: _LL658: {
# 3159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3159 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp805;_tmp805.tag=28;_tmp805.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp805.f2=Cyc_yyget_YY40(yyyyvsp[2]);_tmp805.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp805.f4=0;_tmp805;});_tmp804;}),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 511: _LL659: _LL65A: {
# 3162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3162 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 512: _LL65B: _LL65C: {
# 3165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3166 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
goto _LL260;}case 513: _LL65D: _LL65E: {
# 3169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp806->tl=0;_tmp806;}));
goto _LL260;}case 514: _LL65F: _LL660: {
# 3175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3174 "parse.y"
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp807->tl=Cyc_yyget_YY4(yyyyvsp[0]);_tmp807;}));
goto _LL260;}case 515: _LL661: _LL662: {
# 3177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_const_exp(Cyc_yyget_Int_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 516: _LL663: _LL664: {
# 3183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3181 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 517: _LL665: _LL666: {
# 3184
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3186
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3182 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 518: _LL667: _LL668: {
# 3185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3184 "parse.y"
struct _dyneither_ptr _tmp808=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp808);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp808,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3192
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp808,i,(unsigned int)(yyyylsp[0]).first_line));
# 3194
goto _LL260;}case 519: _LL669: _LL66A: {
# 3196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3195 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
goto _LL260;}case 520: _LL66B: _LL66C: {
# 3198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3199 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809->f1=Cyc_Absyn_Rel_n(0);_tmp809->f2=({struct _dyneither_ptr*_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp80A;});_tmp809;}));
goto _LL260;}case 521: _LL66D: _LL66E: {
# 3202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3200 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 522: _LL66F: _LL670: {
# 3203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3203 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B->f1=Cyc_Absyn_Rel_n(0);_tmp80B->f2=({struct _dyneither_ptr*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp80C;});_tmp80B;}));
goto _LL260;}case 523: _LL671: _LL672: {
# 3206
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3204 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 524: _LL673: _LL674: {
# 3207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3209 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 525: _LL675: _LL676: {
# 3212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3210 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 526: _LL677: _LL678: {
# 3213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3213 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 527: _LL679: _LL67A: {
# 3216
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3218
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3214 "parse.y"
yyval=yyyyvsp[0];
goto _LL260;}case 528: _LL67B: _LL67C: {
# 3217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3219 "parse.y"
goto _LL260;}case 529: _LL67D: _LL67E: {
# 3221
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3223
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3219 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL260;}default: _LL67F: _LL680:
# 3224
 goto _LL260;}_LL260:;}
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
msg=({unsigned int _tmp80D=(unsigned int)(sze + 15);char*_tmp80E=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmp80D + 1));struct _dyneither_ptr _tmp810=_tag_dyneither(_tmp80E,sizeof(char),_tmp80D + 1);{unsigned int _tmp80F=_tmp80D;unsigned int i;for(i=0;i < _tmp80F;i ++){_tmp80E[i]=(char)'\000';}_tmp80E[_tmp80F]=(char)0;}_tmp810;});
Cyc_strcpy(msg,({const char*_tmp811="parse error";_tag_dyneither(_tmp811,sizeof(char),12);}));
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 296 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(7282,x + yyn)]== x){
# 453
Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp812=", expecting `";_tag_dyneither(_tmp812,sizeof(char),14);}):(struct _dyneither_ptr)({const char*_tmp813=" or `";_tag_dyneither(_tmp813,sizeof(char),6);})));
# 456
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(296,x)]);
Cyc_strcat(msg,({const char*_tmp814="'";_tag_dyneither(_tmp814,sizeof(char),2);}));
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
Cyc_yyerror(({const char*_tmp815="parse error";_tag_dyneither(_tmp815,sizeof(char),12);}),yystate,yychar);}}
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
# 3222 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp822=v;struct Cyc_Absyn_Stmt*_tmp823;struct Cyc_Absyn_Exp*_tmp824;union Cyc_Absyn_Nmspace _tmp825;struct _dyneither_ptr*_tmp826;struct _dyneither_ptr _tmp827;char _tmp828;union Cyc_Absyn_Cnst _tmp829;switch((_tmp822.Stmt_tok).tag){case 1: _LL6F3: _tmp829=(_tmp822.Int_tok).val;_LL6F4:
({struct Cyc_String_pa_PrintArg_struct _tmp82C;_tmp82C.tag=0;_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp829));({void*_tmp82A[1]={& _tmp82C};Cyc_fprintf(Cyc_stderr,({const char*_tmp82B="%s";_tag_dyneither(_tmp82B,sizeof(char),3);}),_tag_dyneither(_tmp82A,sizeof(void*),1));});});goto _LL6F2;case 2: _LL6F5: _tmp828=(_tmp822.Char_tok).val;_LL6F6:
({struct Cyc_Int_pa_PrintArg_struct _tmp82F;_tmp82F.tag=1;_tmp82F.f1=(unsigned long)((int)_tmp828);({void*_tmp82D[1]={& _tmp82F};Cyc_fprintf(Cyc_stderr,({const char*_tmp82E="%c";_tag_dyneither(_tmp82E,sizeof(char),3);}),_tag_dyneither(_tmp82D,sizeof(void*),1));});});goto _LL6F2;case 3: _LL6F7: _tmp827=(_tmp822.String_tok).val;_LL6F8:
({struct Cyc_String_pa_PrintArg_struct _tmp832;_tmp832.tag=0;_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp827);({void*_tmp830[1]={& _tmp832};Cyc_fprintf(Cyc_stderr,({const char*_tmp831="\"%s\"";_tag_dyneither(_tmp831,sizeof(char),5);}),_tag_dyneither(_tmp830,sizeof(void*),1));});});goto _LL6F2;case 5: _LL6F9: _tmp825=((_tmp822.QualId_tok).val)->f1;_tmp826=((_tmp822.QualId_tok).val)->f2;_LL6FA: {
# 3228
struct Cyc_List_List*_tmp833=0;
{union Cyc_Absyn_Nmspace _tmp834=_tmp825;struct Cyc_List_List*_tmp835;struct Cyc_List_List*_tmp836;struct Cyc_List_List*_tmp837;switch((_tmp834.C_n).tag){case 1: _LL702: _tmp837=(_tmp834.Rel_n).val;_LL703:
 _tmp833=_tmp837;goto _LL701;case 2: _LL704: _tmp836=(_tmp834.Abs_n).val;_LL705:
 _tmp833=_tmp836;goto _LL701;case 3: _LL706: _tmp835=(_tmp834.C_n).val;_LL707:
 _tmp833=_tmp835;goto _LL701;default: _LL708: _LL709:
 goto _LL701;}_LL701:;}
# 3235
for(0;_tmp833 != 0;_tmp833=_tmp833->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp83A;_tmp83A.tag=0;_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp833->hd));({void*_tmp838[1]={& _tmp83A};Cyc_fprintf(Cyc_stderr,({const char*_tmp839="%s::";_tag_dyneither(_tmp839,sizeof(char),5);}),_tag_dyneither(_tmp838,sizeof(void*),1));});});}
({struct Cyc_String_pa_PrintArg_struct _tmp83D;_tmp83D.tag=0;_tmp83D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp826);({void*_tmp83B[1]={& _tmp83D};Cyc_fprintf(Cyc_stderr,({const char*_tmp83C="%s::";_tag_dyneither(_tmp83C,sizeof(char),5);}),_tag_dyneither(_tmp83B,sizeof(void*),1));});});
goto _LL6F2;}case 7: _LL6FB: _tmp824=(_tmp822.Exp_tok).val;_LL6FC:
({struct Cyc_String_pa_PrintArg_struct _tmp840;_tmp840.tag=0;_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp824));({void*_tmp83E[1]={& _tmp840};Cyc_fprintf(Cyc_stderr,({const char*_tmp83F="%s";_tag_dyneither(_tmp83F,sizeof(char),3);}),_tag_dyneither(_tmp83E,sizeof(void*),1));});});goto _LL6F2;case 8: _LL6FD: _tmp823=(_tmp822.Stmt_tok).val;_LL6FE:
({struct Cyc_String_pa_PrintArg_struct _tmp843;_tmp843.tag=0;_tmp843.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp823));({void*_tmp841[1]={& _tmp843};Cyc_fprintf(Cyc_stderr,({const char*_tmp842="%s";_tag_dyneither(_tmp842,sizeof(char),3);}),_tag_dyneither(_tmp841,sizeof(void*),1));});});goto _LL6F2;default: _LL6FF: _LL700:
({void*_tmp844=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp845="?";_tag_dyneither(_tmp845,sizeof(char),2);}),_tag_dyneither(_tmp844,sizeof(void*),0));});goto _LL6F2;}_LL6F2:;}
# 3246
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp846=_new_region("yyr");struct _RegionHandle*yyr=& _tmp846;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp847=Cyc_Parse_parse_result;_npop_handler(0);return _tmp847;};
# 3249
;_pop_region(yyr);};}
