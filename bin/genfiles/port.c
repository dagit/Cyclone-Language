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
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 42
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;struct Cyc_Relations_Reln;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 251
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 525
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 551
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 558
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 576
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 934 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 944
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 948
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 951
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 969
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 974
extern void*Cyc_Absyn_sint_typ;
# 991
void*Cyc_Absyn_string_typ(void*rgn);
# 1013
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 70 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39 "tcexp.h"
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;struct Cyc_Position_Segment{int start;int end;};struct Cyc_Port_Edit{unsigned int loc;struct _dyneither_ptr old_string;struct _dyneither_ptr new_string;};
# 92 "port.cyc"
typedef struct Cyc_Port_Edit*Cyc_Port_edit_t;
# 97
typedef struct Cyc_List_List*Cyc_Port_edits_t;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 99
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){
return(int)e1 - (int)e2;}
# 107
typedef void*Cyc_Port_ctype_t;struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};
# 109
typedef struct Cyc_Port_Cvar*Cyc_Port_cvar_t;struct Cyc_Port_Cfield{void*qual;struct _dyneither_ptr*f;void*type;};
# 116
typedef struct Cyc_Port_Cfield*Cyc_Port_cfield_t;
# 121
typedef struct Cyc_List_List*Cyc_Port_cfields_t;struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Cvar;struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
# 151
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9};
# 164
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
static struct _dyneither_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_tmpA=t;struct _dyneither_ptr*_tmp16;void*_tmp18;void*_tmp19;void*_tmp1A;void*_tmp1B;void*_tmp1C;void*_tmp1E;void*_tmp1F;void*_tmp20;struct Cyc_Absyn_Aggrdecl*_tmp22;struct Cyc_List_List*_tmp23;struct Cyc_List_List*_tmp25;void*_tmp26;struct Cyc_List_List*_tmp28;void*_tmp2A;struct Cyc_List_List*_tmp2B;struct Cyc_Port_Cvar*_tmp2D;_LL1: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmpB=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL3;}_LL2: {
const char*_tmp4CD;return(_tmp4CD="const",_tag_dyneither(_tmp4CD,sizeof(char),6));}_LL3: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmpC=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmpA;if(_tmpC->tag != 1)goto _LL5;}_LL4: {
const char*_tmp4CE;return(_tmp4CE="notconst",_tag_dyneither(_tmp4CE,sizeof(char),9));}_LL5: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmpD=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmpA;if(_tmpD->tag != 2)goto _LL7;}_LL6: {
const char*_tmp4CF;return(_tmp4CF="thin",_tag_dyneither(_tmp4CF,sizeof(char),5));}_LL7: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmpE=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_tmpA;if(_tmpE->tag != 3)goto _LL9;}_LL8: {
const char*_tmp4D0;return(_tmp4D0="fat",_tag_dyneither(_tmp4D0,sizeof(char),4));}_LL9: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmpF=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmpA;if(_tmpF->tag != 4)goto _LLB;}_LLA: {
const char*_tmp4D1;return(_tmp4D1="void",_tag_dyneither(_tmp4D1,sizeof(char),5));}_LLB: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp10=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmpA;if(_tmp10->tag != 5)goto _LLD;}_LLC: {
const char*_tmp4D2;return(_tmp4D2="zero",_tag_dyneither(_tmp4D2,sizeof(char),5));}_LLD: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp11=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmpA;if(_tmp11->tag != 6)goto _LLF;}_LLE: {
const char*_tmp4D3;return(_tmp4D3="arith",_tag_dyneither(_tmp4D3,sizeof(char),6));}_LLF: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp12=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmpA;if(_tmp12->tag != 7)goto _LL11;}_LL10: {
const char*_tmp4D4;return(_tmp4D4="heap",_tag_dyneither(_tmp4D4,sizeof(char),5));}_LL11: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp13=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmpA;if(_tmp13->tag != 8)goto _LL13;}_LL12: {
const char*_tmp4D5;return(_tmp4D5="ZT",_tag_dyneither(_tmp4D5,sizeof(char),3));}_LL13: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp14=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmpA;if(_tmp14->tag != 9)goto _LL15;}_LL14: {
const char*_tmp4D6;return(_tmp4D6="NZT",_tag_dyneither(_tmp4D6,sizeof(char),4));}_LL15: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp15=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpA;if(_tmp15->tag != 10)goto _LL17;else{_tmp16=_tmp15->f1;}}_LL16: {
const char*_tmp4DA;void*_tmp4D9[1];struct Cyc_String_pa_PrintArg_struct _tmp4D8;return(struct _dyneither_ptr)((_tmp4D8.tag=0,((_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp16),((_tmp4D9[0]=& _tmp4D8,Cyc_aprintf(((_tmp4DA="%s",_tag_dyneither(_tmp4DA,sizeof(char),3))),_tag_dyneither(_tmp4D9,sizeof(void*),1))))))));}_LL17: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp17=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA;if(_tmp17->tag != 11)goto _LL19;else{_tmp18=(void*)_tmp17->f1;_tmp19=(void*)_tmp17->f2;_tmp1A=(void*)_tmp17->f3;_tmp1B=(void*)_tmp17->f4;_tmp1C=(void*)_tmp17->f5;}}_LL18: {
# 180
const char*_tmp4E2;void*_tmp4E1[5];struct Cyc_String_pa_PrintArg_struct _tmp4E0;struct Cyc_String_pa_PrintArg_struct _tmp4DF;struct Cyc_String_pa_PrintArg_struct _tmp4DE;struct Cyc_String_pa_PrintArg_struct _tmp4DD;struct Cyc_String_pa_PrintArg_struct _tmp4DC;return(struct _dyneither_ptr)((_tmp4DC.tag=0,((_tmp4DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 182
Cyc_Port_ctype2string(deep,_tmp1C)),((_tmp4DD.tag=0,((_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1B)),((_tmp4DE.tag=0,((_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 181
Cyc_Port_ctype2string(deep,_tmp1A)),((_tmp4DF.tag=0,((_tmp4DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp19)),((_tmp4E0.tag=0,((_tmp4E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 180
Cyc_Port_ctype2string(deep,_tmp18)),((_tmp4E1[0]=& _tmp4E0,((_tmp4E1[1]=& _tmp4DF,((_tmp4E1[2]=& _tmp4DE,((_tmp4E1[3]=& _tmp4DD,((_tmp4E1[4]=& _tmp4DC,Cyc_aprintf(((_tmp4E2="ptr(%s,%s,%s,%s,%s)",_tag_dyneither(_tmp4E2,sizeof(char),20))),_tag_dyneither(_tmp4E1,sizeof(void*),5))))))))))))))))))))))))))))))));}_LL19: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp1D=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA;if(_tmp1D->tag != 12)goto _LL1B;else{_tmp1E=(void*)_tmp1D->f1;_tmp1F=(void*)_tmp1D->f2;_tmp20=(void*)_tmp1D->f3;}}_LL1A: {
# 184
const char*_tmp4E8;void*_tmp4E7[3];struct Cyc_String_pa_PrintArg_struct _tmp4E6;struct Cyc_String_pa_PrintArg_struct _tmp4E5;struct Cyc_String_pa_PrintArg_struct _tmp4E4;return(struct _dyneither_ptr)((_tmp4E4.tag=0,((_tmp4E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_ctype2string(deep,_tmp20)),((_tmp4E5.tag=0,((_tmp4E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1F)),((_tmp4E6.tag=0,((_tmp4E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 184
Cyc_Port_ctype2string(deep,_tmp1E)),((_tmp4E7[0]=& _tmp4E6,((_tmp4E7[1]=& _tmp4E5,((_tmp4E7[2]=& _tmp4E4,Cyc_aprintf(((_tmp4E8="array(%s,%s,%s)",_tag_dyneither(_tmp4E8,sizeof(char),16))),_tag_dyneither(_tmp4E7,sizeof(void*),3))))))))))))))))))));}_LL1B: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp21=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA;if(_tmp21->tag != 13)goto _LL1D;else{_tmp22=(_tmp21->f1)->f1;_tmp23=(_tmp21->f1)->f2;}}_LL1C: {
# 187
const char*_tmp4EA;const char*_tmp4E9;struct _dyneither_ptr s=_tmp22->kind == Cyc_Absyn_StructA?(_tmp4EA="struct",_tag_dyneither(_tmp4EA,sizeof(char),7)):((_tmp4E9="union",_tag_dyneither(_tmp4E9,sizeof(char),6)));
if(!deep){
const char*_tmp4EF;void*_tmp4EE[2];struct Cyc_String_pa_PrintArg_struct _tmp4ED;struct Cyc_String_pa_PrintArg_struct _tmp4EC;return(struct _dyneither_ptr)((_tmp4EC.tag=0,((_tmp4EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp22->name)),((_tmp4ED.tag=0,((_tmp4ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp4EE[0]=& _tmp4ED,((_tmp4EE[1]=& _tmp4EC,Cyc_aprintf(((_tmp4EF="%s %s",_tag_dyneither(_tmp4EF,sizeof(char),6))),_tag_dyneither(_tmp4EE,sizeof(void*),2))))))))))))));}else{
# 191
const char*_tmp4F5;void*_tmp4F4[3];struct Cyc_String_pa_PrintArg_struct _tmp4F3;struct Cyc_String_pa_PrintArg_struct _tmp4F2;struct Cyc_String_pa_PrintArg_struct _tmp4F1;return(struct _dyneither_ptr)((_tmp4F1.tag=0,((_tmp4F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_cfields2string(0,_tmp23)),((_tmp4F2.tag=0,((_tmp4F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 191
Cyc_Absynpp_qvar2string(_tmp22->name)),((_tmp4F3.tag=0,((_tmp4F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp4F4[0]=& _tmp4F3,((_tmp4F4[1]=& _tmp4F2,((_tmp4F4[2]=& _tmp4F1,Cyc_aprintf(((_tmp4F5="%s %s {%s}",_tag_dyneither(_tmp4F5,sizeof(char),11))),_tag_dyneither(_tmp4F4,sizeof(void*),3))))))))))))))))))));}}_LL1D: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp24=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA;if(_tmp24->tag != 14)goto _LL1F;else{_tmp25=_tmp24->f1;if(_tmp24->f2 == 0)goto _LL1F;_tmp26=*_tmp24->f2;}}_LL1E:
# 193
 return Cyc_Port_ctype2string(deep,_tmp26);_LL1F: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp27=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA;if(_tmp27->tag != 14)goto _LL21;else{_tmp28=_tmp27->f1;}}_LL20: {
# 195
const char*_tmp4F9;void*_tmp4F8[1];struct Cyc_String_pa_PrintArg_struct _tmp4F7;return(struct _dyneither_ptr)((_tmp4F7.tag=0,((_tmp4F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp28)),((_tmp4F8[0]=& _tmp4F7,Cyc_aprintf(((_tmp4F9="aggr {%s}",_tag_dyneither(_tmp4F9,sizeof(char),10))),_tag_dyneither(_tmp4F8,sizeof(void*),1))))))));}_LL21: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp29=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA;if(_tmp29->tag != 15)goto _LL23;else{_tmp2A=(void*)_tmp29->f1;_tmp2B=_tmp29->f2;}}_LL22: {
# 197
const char*_tmp4FE;void*_tmp4FD[2];struct Cyc_String_pa_PrintArg_struct _tmp4FC;struct Cyc_String_pa_PrintArg_struct _tmp4FB;return(struct _dyneither_ptr)((_tmp4FB.tag=0,((_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp2A)),((_tmp4FC.tag=0,((_tmp4FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp2B)),((_tmp4FD[0]=& _tmp4FC,((_tmp4FD[1]=& _tmp4FB,Cyc_aprintf(((_tmp4FE="fn(%s)->%s",_tag_dyneither(_tmp4FE,sizeof(char),11))),_tag_dyneither(_tmp4FD,sizeof(void*),2))))))))))))));}_LL23: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp2C=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpA;if(_tmp2C->tag != 16)goto _LL0;else{_tmp2D=_tmp2C->f1;}}_LL24:
# 199
 if((unsigned int)_tmp2D->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp2D->eq)));else{
if(!deep  || _tmp2D->uppers == 0  && _tmp2D->lowers == 0){
const char*_tmp502;void*_tmp501[1];struct Cyc_Int_pa_PrintArg_struct _tmp500;return(struct _dyneither_ptr)((_tmp500.tag=1,((_tmp500.f1=(unsigned long)_tmp2D->id,((_tmp501[0]=& _tmp500,Cyc_aprintf(((_tmp502="var(%d)",_tag_dyneither(_tmp502,sizeof(char),8))),_tag_dyneither(_tmp501,sizeof(void*),1))))))));}else{
const char*_tmp508;void*_tmp507[3];struct Cyc_String_pa_PrintArg_struct _tmp506;struct Cyc_Int_pa_PrintArg_struct _tmp505;struct Cyc_String_pa_PrintArg_struct _tmp504;return(struct _dyneither_ptr)((_tmp504.tag=0,((_tmp504.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 206
Cyc_Port_ctypes2string(0,_tmp2D->uppers)),((_tmp505.tag=1,((_tmp505.f1=(unsigned long)_tmp2D->id,((_tmp506.tag=0,((_tmp506.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 204
Cyc_Port_ctypes2string(0,_tmp2D->lowers)),((_tmp507[0]=& _tmp506,((_tmp507[1]=& _tmp505,((_tmp507[2]=& _tmp504,Cyc_aprintf(((_tmp508="var([%s]<=%d<=[%s])",_tag_dyneither(_tmp508,sizeof(char),20))),_tag_dyneither(_tmp507,sizeof(void*),3))))))))))))))))))));}}_LL0:;}
# 209
static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){struct _dyneither_ptr*_tmp509;return(_tmp509=_cycalloc(sizeof(*_tmp509)),((_tmp509[0]=Cyc_Port_ctype2string(deep,t),_tmp509)));}
struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,struct Cyc_List_List*xs){
struct _dyneither_ptr*_tmp50A;struct _dyneither_ptr*_tmp62=(_tmp50A=_cycalloc(sizeof(*_tmp50A)),((_tmp50A[0]=s,_tmp50A)));
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp63=xs;
struct Cyc_List_List*_tmp64=xs->tl;
for(0;_tmp64 != 0;(_tmp63=_tmp64,_tmp64=_tmp64->tl)){
struct Cyc_List_List*_tmp50B;_tmp63->tl=((_tmp50B=_cycalloc(sizeof(*_tmp50B)),((_tmp50B->hd=_tmp62,((_tmp50B->tl=_tmp64,_tmp50B))))));}
# 218
return xs;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 220
static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
const char*_tmp511;void*_tmp510[3];struct Cyc_String_pa_PrintArg_struct _tmp50F;struct Cyc_String_pa_PrintArg_struct _tmp50E;struct Cyc_String_pa_PrintArg_struct _tmp50D;struct _dyneither_ptr s=(struct _dyneither_ptr)(
(_tmp50D.tag=0,((_tmp50D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->type)),((_tmp50E.tag=0,((_tmp50E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f),((_tmp50F.tag=0,((_tmp50F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->qual)),((_tmp510[0]=& _tmp50F,((_tmp510[1]=& _tmp50E,((_tmp510[2]=& _tmp50D,Cyc_aprintf(((_tmp511="%s %s: %s",_tag_dyneither(_tmp511,sizeof(char),10))),_tag_dyneither(_tmp510,sizeof(void*),3))))))))))))))))))));
struct _dyneither_ptr*_tmp512;return(_tmp512=_cycalloc(sizeof(*_tmp512)),((_tmp512[0]=s,_tmp512)));}
# 225
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
const char*_tmp513;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((_tmp513=",",_tag_dyneither(_tmp513,sizeof(char),2))),((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts)));}struct Cyc_Port_Cfield;
# 228
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
const char*_tmp514;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((_tmp514=";",_tag_dyneither(_tmp514,sizeof(char),2))),((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs)));}
# 234
static void*Cyc_Port_notconst_ct(){return(void*)& Cyc_Port_Notconst_ct_val;}
static void*Cyc_Port_const_ct(){return(void*)& Cyc_Port_Const_ct_val;}
static void*Cyc_Port_thin_ct(){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct(){return(void*)& Cyc_Port_Fat_ct_val;}
static void*Cyc_Port_void_ct(){return(void*)& Cyc_Port_Void_ct_val;}
static void*Cyc_Port_zero_ct(){return(void*)& Cyc_Port_Zero_ct_val;}
static void*Cyc_Port_arith_ct(){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct(){return(void*)& Cyc_Port_Heap_ct_val;}
static void*Cyc_Port_zterm_ct(){return(void*)& Cyc_Port_Zterm_ct_val;}
static void*Cyc_Port_nozterm_ct(){return(void*)& Cyc_Port_Nozterm_ct_val;}
static void*Cyc_Port_rgnvar_ct(struct _dyneither_ptr*n){struct Cyc_Port_RgnVar_ct_Port_Ctype_struct _tmp517;struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp516;return(void*)((_tmp516=_cycalloc(sizeof(*_tmp516)),((_tmp516[0]=((_tmp517.tag=10,((_tmp517.f1=n,_tmp517)))),_tmp516))));}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct _tmp51A;struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp519;return(void*)((_tmp519=_cycalloc(sizeof(*_tmp519)),((_tmp519[0]=((_tmp51A.tag=14,((_tmp51A.f1=fs,((_tmp51A.f2=0,_tmp51A)))))),_tmp519))));}
# 248
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct _tmp51D;struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp51C;return(void*)((_tmp51C=_cycalloc(sizeof(*_tmp51C)),((_tmp51C[0]=((_tmp51D.tag=13,((_tmp51D.f1=p,_tmp51D)))),_tmp51C))));}
# 251
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 253
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp520;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp51F;return(void*)((_tmp51F=_cycalloc(sizeof(*_tmp51F)),((_tmp51F[0]=((_tmp520.tag=11,((_tmp520.f1=elt,((_tmp520.f2=qual,((_tmp520.f3=ptr_kind,((_tmp520.f4=r,((_tmp520.f5=zt,_tmp520)))))))))))),_tmp51F))));}
# 255
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp523;struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp522;return(void*)((_tmp522=_cycalloc(sizeof(*_tmp522)),((_tmp522[0]=((_tmp523.tag=12,((_tmp523.f1=elt,((_tmp523.f2=qual,((_tmp523.f3=zt,_tmp523)))))))),_tmp522))));}
# 258
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp526;struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp525;return(void*)((_tmp525=_cycalloc(sizeof(*_tmp525)),((_tmp525[0]=((_tmp526.tag=15,((_tmp526.f1=return_type,((_tmp526.f2=args,_tmp526)))))),_tmp525))));}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 261
static void*Cyc_Port_var(){
static int counter=0;
struct Cyc_Port_Var_ct_Port_Ctype_struct _tmp52C;struct Cyc_Port_Cvar*_tmp52B;struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp52A;return(void*)((_tmp52A=_cycalloc(sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52C.tag=16,((_tmp52C.f1=((_tmp52B=_cycalloc(sizeof(*_tmp52B)),((_tmp52B->id=counter ++,((_tmp52B->eq=0,((_tmp52B->uppers=0,((_tmp52B->lowers=0,_tmp52B)))))))))),_tmp52C)))),_tmp52A))));}
# 265
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 268
static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;
const char*_tmp530;void*_tmp52F[1];struct Cyc_Int_pa_PrintArg_struct _tmp52E;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp52E.tag=1,((_tmp52E.f1=(unsigned long)counter ++,((_tmp52F[0]=& _tmp52E,Cyc_aprintf(((_tmp530="`R%d",_tag_dyneither(_tmp530,sizeof(char),5))),_tag_dyneither(_tmp52F,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp531;return(_tmp531=_cycalloc(sizeof(*_tmp531)),((_tmp531[0]=s,_tmp531)));}
# 276
static int Cyc_Port_unifies(void*t1,void*t2);struct Cyc_Port_Cvar;
# 278
static void*Cyc_Port_compress_ct(void*t){
void*_tmp82=t;void***_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp86;void**_tmp88;_LL26: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp83=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp82;if(_tmp83->tag != 16)goto _LL28;else{_tmp84=(void***)&(_tmp83->f1)->eq;_tmp85=(_tmp83->f1)->uppers;_tmp86=(_tmp83->f1)->lowers;}}_LL27: {
# 281
void**_tmp89=*_tmp84;
if((unsigned int)_tmp89){
# 285
void*r=Cyc_Port_compress_ct(*_tmp89);
if(*_tmp89 != r){void**_tmp532;*_tmp84=((_tmp532=_cycalloc(sizeof(*_tmp532)),((_tmp532[0]=r,_tmp532))));}
return r;}
# 292
for(0;_tmp86 != 0;_tmp86=_tmp86->tl){
void*_tmp8B=(void*)_tmp86->hd;void*_tmp8C=_tmp8B;_LL2D: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp8D=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp8C;if(_tmp8D->tag != 0)goto _LL2F;}_LL2E:
 goto _LL30;_LL2F: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp8E=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp8C;if(_tmp8E->tag != 9)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp8F=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp8C;if(_tmp8F->tag != 4)goto _LL33;}_LL32:
 goto _LL34;_LL33: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp90=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp8C;if(_tmp90->tag != 13)goto _LL35;}_LL34:
 goto _LL36;_LL35: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp91=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp8C;if(_tmp91->tag != 15)goto _LL37;}_LL36:
# 299
{void**_tmp533;*_tmp84=((_tmp533=_cycalloc(sizeof(*_tmp533)),((_tmp533[0]=(void*)_tmp86->hd,_tmp533))));}
return(void*)_tmp86->hd;_LL37:;_LL38:
# 302
 goto _LL2C;_LL2C:;}
# 305
for(0;_tmp85 != 0;_tmp85=_tmp85->tl){
void*_tmp93=(void*)_tmp85->hd;void*_tmp94=_tmp93;_LL3A: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp95=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp94;if(_tmp95->tag != 1)goto _LL3C;}_LL3B:
 goto _LL3D;_LL3C: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp96=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp94;if(_tmp96->tag != 8)goto _LL3E;}_LL3D:
 goto _LL3F;_LL3E: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp97=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp94;if(_tmp97->tag != 5)goto _LL40;}_LL3F:
 goto _LL41;_LL40: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp98=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp94;if(_tmp98->tag != 13)goto _LL42;}_LL41:
 goto _LL43;_LL42: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp99=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp94;if(_tmp99->tag != 15)goto _LL44;}_LL43:
# 312
{void**_tmp534;*_tmp84=((_tmp534=_cycalloc(sizeof(*_tmp534)),((_tmp534[0]=(void*)_tmp85->hd,_tmp534))));}
return(void*)_tmp85->hd;_LL44:;_LL45:
# 315
 goto _LL39;_LL39:;}
# 318
return t;}_LL28: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp87=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp82;if(_tmp87->tag != 14)goto _LL2A;else{_tmp88=_tmp87->f2;}}_LL29:
# 321
 if((unsigned int)_tmp88)return Cyc_Port_compress_ct(*_tmp88);else{
return t;}_LL2A:;_LL2B:
# 325
 return t;_LL25:;}
# 331
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp9B=Cyc_Port_compress_ct(t);void*_tmp9C=_tmp9B;struct _dyneither_ptr*_tmpAB;void*_tmpAD;void*_tmpAE;void*_tmpAF;void*_tmpB0;void*_tmpB1;void*_tmpB3;void*_tmpB4;void*_tmpB5;void*_tmpB7;struct Cyc_List_List*_tmpB8;_LL47: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp9D=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp9C;if(_tmp9D->tag != 0)goto _LL49;}_LL48:
 goto _LL4A;_LL49: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp9E=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp9C;if(_tmp9E->tag != 1)goto _LL4B;}_LL4A:
 goto _LL4C;_LL4B: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp9F=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmp9C;if(_tmp9F->tag != 2)goto _LL4D;}_LL4C:
 goto _LL4E;_LL4D: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmpA0=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA0->tag != 3)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmpA1=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA1->tag != 4)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmpA2=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA2->tag != 5)goto _LL53;}_LL52:
 goto _LL54;_LL53: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmpA3=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA3->tag != 6)goto _LL55;}_LL54:
 goto _LL56;_LL55: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmpA4=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA4->tag != 8)goto _LL57;}_LL56:
 goto _LL58;_LL57: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmpA5=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA5->tag != 9)goto _LL59;}_LL58:
 goto _LL5A;_LL59: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmpA6=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA6->tag != 14)goto _LL5B;}_LL5A:
 goto _LL5C;_LL5B: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmpA7=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA7->tag != 13)goto _LL5D;}_LL5C:
 goto _LL5E;_LL5D: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmpA8=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA8->tag != 16)goto _LL5F;}_LL5E:
 goto _LL60;_LL5F: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmpA9=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp9C;if(_tmpA9->tag != 7)goto _LL61;}_LL60:
 return t;_LL61: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmpAA=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9C;if(_tmpAA->tag != 10)goto _LL63;else{_tmpAB=_tmpAA->f1;}}_LL62:
# 347
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*instenv,_tmpAB))
*instenv=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*instenv,_tmpAB,Cyc_Port_var());
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*instenv,_tmpAB);_LL63: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpAC=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9C;if(_tmpAC->tag != 11)goto _LL65;else{_tmpAD=(void*)_tmpAC->f1;_tmpAE=(void*)_tmpAC->f2;_tmpAF=(void*)_tmpAC->f3;_tmpB0=(void*)_tmpAC->f4;_tmpB1=(void*)_tmpAC->f5;}}_LL64: {
# 351
struct _tuple10 _tmp535;struct _tuple10 _tmpB9=(_tmp535.f1=Cyc_Port_inst(instenv,_tmpAD),((_tmp535.f2=Cyc_Port_inst(instenv,_tmpB0),_tmp535)));void*_tmpBB;void*_tmpBC;struct _tuple10 _tmpBA=_tmpB9;_tmpBB=_tmpBA.f1;_tmpBC=_tmpBA.f2;
if(_tmpBB == _tmpAD  && _tmpBC == _tmpB0)return t;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp538;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp537;return(void*)((_tmp537=_cycalloc(sizeof(*_tmp537)),((_tmp537[0]=((_tmp538.tag=11,((_tmp538.f1=_tmpBB,((_tmp538.f2=_tmpAE,((_tmp538.f3=_tmpAF,((_tmp538.f4=_tmpBC,((_tmp538.f5=_tmpB1,_tmp538)))))))))))),_tmp537))));};}_LL65: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmpB2=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9C;if(_tmpB2->tag != 12)goto _LL67;else{_tmpB3=(void*)_tmpB2->f1;_tmpB4=(void*)_tmpB2->f2;_tmpB5=(void*)_tmpB2->f3;}}_LL66: {
# 355
void*_tmpC0=Cyc_Port_inst(instenv,_tmpB3);
if(_tmpC0 == _tmpB3)return t;{
struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp53B;struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp53A;return(void*)((_tmp53A=_cycalloc(sizeof(*_tmp53A)),((_tmp53A[0]=((_tmp53B.tag=12,((_tmp53B.f1=_tmpC0,((_tmp53B.f2=_tmpB4,((_tmp53B.f3=_tmpB5,_tmp53B)))))))),_tmp53A))));};}_LL67: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmpB6=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9C;if(_tmpB6->tag != 15)goto _LL46;else{_tmpB7=(void*)_tmpB6->f1;_tmpB8=_tmpB6->f2;}}_LL68: {
# 359
struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp53E;struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp53D;return(void*)((_tmp53D=_cycalloc(sizeof(*_tmp53D)),((_tmp53D[0]=((_tmp53E.tag=15,((_tmp53E.f1=Cyc_Port_inst(instenv,_tmpB7),((_tmp53E.f2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmpB8),_tmp53E)))))),_tmp53D))));}_LL46:;}
# 363
void*Cyc_Port_instantiate(void*t){
struct Cyc_Dict_Dict*_tmp53F;return Cyc_Port_inst(((_tmp53F=_cycalloc(sizeof(*_tmp53F)),((_tmp53F[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),_tmp53F)))),t);}
# 370
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*_tmpC6=ts;for(0;(unsigned int)_tmpC6;_tmpC6=_tmpC6->tl){
void*_tmpC7=Cyc_Port_compress_ct((void*)_tmpC6->hd);
if(t == _tmpC7)found=1;}}
# 376
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(0;ts != 0;ts=ts->tl){
if(t == Cyc_Port_compress_ct((void*)ts->hd))continue;{
struct Cyc_List_List*_tmp540;res=((_tmp540=_cycalloc(sizeof(*_tmp540)),((_tmp540->hd=(void*)ts->hd,((_tmp540->tl=res,_tmp540))))));};}
# 382
return res;};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 387
void Cyc_Port_generalize(int is_rgn,void*t){
t=Cyc_Port_compress_ct(t);{
void*_tmpC9=t;struct Cyc_Port_Cvar*_tmpCB;void*_tmpDA;void*_tmpDB;void*_tmpDC;void*_tmpDD;void*_tmpDE;void*_tmpE0;void*_tmpE1;void*_tmpE2;void*_tmpE4;struct Cyc_List_List*_tmpE5;_LL6A: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmpCA=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpC9;if(_tmpCA->tag != 16)goto _LL6C;else{_tmpCB=_tmpCA->f1;}}_LL6B:
# 392
 _tmpCB->uppers=Cyc_Port_filter_self(t,_tmpCB->uppers);
_tmpCB->lowers=Cyc_Port_filter_self(t,_tmpCB->lowers);
if(is_rgn){
# 397
if(_tmpCB->uppers == 0  && _tmpCB->lowers == 0){
Cyc_Port_unifies(t,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));
return;}
# 402
if((unsigned int)_tmpCB->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmpCB->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 406
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmpCB->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 410
return;_LL6C: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmpCC=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmpC9;if(_tmpCC->tag != 0)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmpCD=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmpC9;if(_tmpCD->tag != 1)goto _LL70;}_LL6F:
 goto _LL71;_LL70: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmpCE=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmpC9;if(_tmpCE->tag != 2)goto _LL72;}_LL71:
 goto _LL73;_LL72: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmpCF=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_tmpC9;if(_tmpCF->tag != 3)goto _LL74;}_LL73:
 goto _LL75;_LL74: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmpD0=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD0->tag != 4)goto _LL76;}_LL75:
 goto _LL77;_LL76: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmpD1=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD1->tag != 5)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmpD2=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD2->tag != 6)goto _LL7A;}_LL79:
 goto _LL7B;_LL7A: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmpD3=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD3->tag != 14)goto _LL7C;}_LL7B:
 goto _LL7D;_LL7C: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmpD4=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD4->tag != 13)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmpD5=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD5->tag != 10)goto _LL80;}_LL7F:
 goto _LL81;_LL80: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmpD6=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD6->tag != 9)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmpD7=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD7->tag != 8)goto _LL84;}_LL83:
 goto _LL85;_LL84: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmpD8=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD8->tag != 7)goto _LL86;}_LL85:
 return;_LL86: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpD9=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC9;if(_tmpD9->tag != 11)goto _LL88;else{_tmpDA=(void*)_tmpD9->f1;_tmpDB=(void*)_tmpD9->f2;_tmpDC=(void*)_tmpD9->f3;_tmpDD=(void*)_tmpD9->f4;_tmpDE=(void*)_tmpD9->f5;}}_LL87:
# 427
 Cyc_Port_generalize(0,_tmpDA);Cyc_Port_generalize(1,_tmpDD);goto _LL69;_LL88: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmpDF=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpC9;if(_tmpDF->tag != 12)goto _LL8A;else{_tmpE0=(void*)_tmpDF->f1;_tmpE1=(void*)_tmpDF->f2;_tmpE2=(void*)_tmpDF->f3;}}_LL89:
# 429
 Cyc_Port_generalize(0,_tmpE0);Cyc_Port_generalize(0,_tmpE1);goto _LL69;_LL8A: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmpE3=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpC9;if(_tmpE3->tag != 15)goto _LL69;else{_tmpE4=(void*)_tmpE3->f1;_tmpE5=_tmpE3->f2;}}_LL8B:
# 431
 Cyc_Port_generalize(0,_tmpE4);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmpE5);goto _LL69;_LL69:;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 437
static int Cyc_Port_occurs(void*v,void*t){
t=Cyc_Port_compress_ct(t);
if(t == v)return 1;{
void*_tmpE6=t;void*_tmpE8;void*_tmpE9;void*_tmpEA;void*_tmpEB;void*_tmpEC;void*_tmpEE;void*_tmpEF;void*_tmpF0;void*_tmpF2;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF7;_LL8D: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpE7=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE6;if(_tmpE7->tag != 11)goto _LL8F;else{_tmpE8=(void*)_tmpE7->f1;_tmpE9=(void*)_tmpE7->f2;_tmpEA=(void*)_tmpE7->f3;_tmpEB=(void*)_tmpE7->f4;_tmpEC=(void*)_tmpE7->f5;}}_LL8E:
# 442
 return(((Cyc_Port_occurs(v,_tmpE8) || Cyc_Port_occurs(v,_tmpE9)) || Cyc_Port_occurs(v,_tmpEA)) || Cyc_Port_occurs(v,_tmpEB)) || 
Cyc_Port_occurs(v,_tmpEC);_LL8F: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmpED=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpE6;if(_tmpED->tag != 12)goto _LL91;else{_tmpEE=(void*)_tmpED->f1;_tmpEF=(void*)_tmpED->f2;_tmpF0=(void*)_tmpED->f3;}}_LL90:
# 445
 return(Cyc_Port_occurs(v,_tmpEE) || Cyc_Port_occurs(v,_tmpEF)) || Cyc_Port_occurs(v,_tmpF0);_LL91: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmpF1=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpE6;if(_tmpF1->tag != 15)goto _LL93;else{_tmpF2=(void*)_tmpF1->f1;_tmpF3=_tmpF1->f2;}}_LL92:
# 447
 if(Cyc_Port_occurs(v,_tmpF2))return 1;
for(0;(unsigned int)_tmpF3;_tmpF3=_tmpF3->tl){
if(Cyc_Port_occurs(v,(void*)_tmpF3->hd))return 1;}
return 0;_LL93: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmpF4=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpE6;if(_tmpF4->tag != 13)goto _LL95;else{_tmpF5=(_tmpF4->f1)->f2;}}_LL94:
 return 0;_LL95: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmpF6=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpE6;if(_tmpF6->tag != 14)goto _LL97;else{_tmpF7=_tmpF6->f1;}}_LL96:
# 453
 for(0;(unsigned int)_tmpF7;_tmpF7=_tmpF7->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpF7->hd)->qual) || Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpF7->hd)->type))return 1;}
return 0;_LL97:;_LL98:
 return 0;_LL8C:;};}char Cyc_Port_Unify_ct[9]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 465
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 467
static int Cyc_Port_leq(void*t1,void*t2);
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 471
static void Cyc_Port_unify_ct(void*t1,void*t2){
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);
if(t1 == t2)return;{
struct _tuple10 _tmp541;struct _tuple10 _tmpF9=(_tmp541.f1=t1,((_tmp541.f2=t2,_tmp541)));struct _tuple10 _tmpFA=_tmpF9;struct Cyc_Port_Cvar*_tmpFC;struct Cyc_Port_Cvar*_tmpFE;void*_tmp100;void*_tmp101;void*_tmp102;void*_tmp103;void*_tmp104;void*_tmp106;void*_tmp107;void*_tmp108;void*_tmp109;void*_tmp10A;struct _dyneither_ptr _tmp10C;struct _dyneither_ptr _tmp10E;void*_tmp110;void*_tmp111;void*_tmp112;void*_tmp114;void*_tmp115;void*_tmp116;void*_tmp118;struct Cyc_List_List*_tmp119;void*_tmp11B;struct Cyc_List_List*_tmp11C;struct _tuple11*_tmp11E;struct _tuple11*_tmp120;struct Cyc_List_List*_tmp122;void***_tmp123;struct Cyc_List_List*_tmp125;void***_tmp126;struct Cyc_List_List*_tmp128;void***_tmp129;struct Cyc_Absyn_Aggrdecl*_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Aggrdecl*_tmp12E;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp131;void***_tmp132;_LL9A: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmpFB=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmpFB->tag != 16)goto _LL9C;else{_tmpFC=_tmpFB->f1;}}_LL9B:
# 477
 if(!Cyc_Port_occurs(t1,t2)){
# 480
{struct Cyc_List_List*_tmp133=Cyc_Port_filter_self(t1,_tmpFC->uppers);for(0;(unsigned int)_tmp133;_tmp133=_tmp133->tl){
if(!Cyc_Port_leq(t2,(void*)_tmp133->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmp134=Cyc_Port_filter_self(t1,_tmpFC->lowers);for(0;(unsigned int)_tmp134;_tmp134=_tmp134->tl){
if(!Cyc_Port_leq((void*)_tmp134->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{void**_tmp542;_tmpFC->eq=((_tmp542=_cycalloc(sizeof(*_tmp542)),((_tmp542[0]=t2,_tmp542))));}
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}_LL9C: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmpFD=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmpFD->tag != 16)goto _LL9E;else{_tmpFE=_tmpFD->f1;}}_LL9D:
 Cyc_Port_unify_ct(t2,t1);return;_LL9E:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFF=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmpFF->tag != 11)goto _LLA0;else{_tmp100=(void*)_tmpFF->f1;_tmp101=(void*)_tmpFF->f2;_tmp102=(void*)_tmpFF->f3;_tmp103=(void*)_tmpFF->f4;_tmp104=(void*)_tmpFF->f5;}}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp105=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp105->tag != 11)goto _LLA0;else{_tmp106=(void*)_tmp105->f1;_tmp107=(void*)_tmp105->f2;_tmp108=(void*)_tmp105->f3;_tmp109=(void*)_tmp105->f4;_tmp10A=(void*)_tmp105->f5;}};_LL9F:
# 489
 Cyc_Port_unify_ct(_tmp100,_tmp106);Cyc_Port_unify_ct(_tmp101,_tmp107);Cyc_Port_unify_ct(_tmp102,_tmp108);Cyc_Port_unify_ct(_tmp103,_tmp109);
Cyc_Port_unify_ct(_tmp104,_tmp10A);
return;_LLA0:{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp10B=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmp10B->tag != 10)goto _LLA2;else{_tmp10C=*_tmp10B->f1;}}{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp10D=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp10D->tag != 10)goto _LLA2;else{_tmp10E=*_tmp10D->f1;}};_LLA1:
# 493
 if(Cyc_strcmp((struct _dyneither_ptr)_tmp10C,(struct _dyneither_ptr)_tmp10E)!= 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;_LLA2:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp10F=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmp10F->tag != 12)goto _LLA4;else{_tmp110=(void*)_tmp10F->f1;_tmp111=(void*)_tmp10F->f2;_tmp112=(void*)_tmp10F->f3;}}{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp113=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp113->tag != 12)goto _LLA4;else{_tmp114=(void*)_tmp113->f1;_tmp115=(void*)_tmp113->f2;_tmp116=(void*)_tmp113->f3;}};_LLA3:
# 496
 Cyc_Port_unify_ct(_tmp110,_tmp114);Cyc_Port_unify_ct(_tmp111,_tmp115);Cyc_Port_unify_ct(_tmp112,_tmp116);return;_LLA4:{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp117=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmp117->tag != 15)goto _LLA6;else{_tmp118=(void*)_tmp117->f1;_tmp119=_tmp117->f2;}}{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp11A=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp11A->tag != 15)goto _LLA6;else{_tmp11B=(void*)_tmp11A->f1;_tmp11C=_tmp11A->f2;}};_LLA5:
# 498
 Cyc_Port_unify_ct(_tmp118,_tmp11B);Cyc_Port_unify_cts(_tmp119,_tmp11C);return;_LLA6:{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp11D=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmp11D->tag != 13)goto _LLA8;else{_tmp11E=_tmp11D->f1;}}{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp11F=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp11F->tag != 13)goto _LLA8;else{_tmp120=_tmp11F->f1;}};_LLA7:
# 500
 if(_tmp11E == _tmp120)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}_LLA8:{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp121=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmp121->tag != 14)goto _LLAA;else{_tmp122=_tmp121->f1;_tmp123=(void***)& _tmp121->f2;}}{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp124=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp124->tag != 14)goto _LLAA;else{_tmp125=_tmp124->f1;_tmp126=(void***)& _tmp124->f2;}};_LLA9: {
# 502
void*_tmp136=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmp122,_tmp125,1));
{void**_tmp543;*_tmp123=(*_tmp126=((_tmp543=_cycalloc(sizeof(*_tmp543)),((_tmp543[0]=_tmp136,_tmp543)))));}
return;}_LLAA:{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp127=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmp127->tag != 14)goto _LLAC;else{_tmp128=_tmp127->f1;_tmp129=(void***)& _tmp127->f2;}}{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp12A=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp12A->tag != 13)goto _LLAC;else{_tmp12B=(_tmp12A->f1)->f1;_tmp12C=(_tmp12A->f1)->f2;}};_LLAB:
# 506
 Cyc_Port_merge_fields(_tmp12C,_tmp128,0);
{void**_tmp544;*_tmp129=((_tmp544=_cycalloc(sizeof(*_tmp544)),((_tmp544[0]=t2,_tmp544))));}
return;_LLAC:{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp12D=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpFA.f1;if(_tmp12D->tag != 13)goto _LLAE;else{_tmp12E=(_tmp12D->f1)->f1;_tmp12F=(_tmp12D->f1)->f2;}}{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp130=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpFA.f2;if(_tmp130->tag != 14)goto _LLAE;else{_tmp131=_tmp130->f1;_tmp132=(void***)& _tmp130->f2;}};_LLAD:
# 510
 Cyc_Port_merge_fields(_tmp12F,_tmp131,0);
{void**_tmp545;*_tmp132=((_tmp545=_cycalloc(sizeof(*_tmp545)),((_tmp545[0]=t1,_tmp545))));}
return;_LLAE:;_LLAF:
(int)_throw((void*)& Cyc_Port_Unify_ct_val);_LL99:;};}
# 517
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 521
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 527
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){
# 529
struct Cyc_List_List*common=0;
{struct Cyc_List_List*_tmp13B=fs2;for(0;(unsigned int)_tmp13B;_tmp13B=_tmp13B->tl){
struct Cyc_Port_Cfield*_tmp13C=(struct Cyc_Port_Cfield*)_tmp13B->hd;
int found=0;
{struct Cyc_List_List*_tmp13D=fs1;for(0;(unsigned int)_tmp13D;_tmp13D=_tmp13D->tl){
struct Cyc_Port_Cfield*_tmp13E=(struct Cyc_Port_Cfield*)_tmp13D->hd;
if(Cyc_strptrcmp(_tmp13E->f,_tmp13C->f)== 0){
{struct Cyc_List_List*_tmp546;common=((_tmp546=_cycalloc(sizeof(*_tmp546)),((_tmp546->hd=_tmp13E,((_tmp546->tl=common,_tmp546))))));}
Cyc_Port_unify_ct(_tmp13E->qual,_tmp13C->qual);
Cyc_Port_unify_ct(_tmp13E->type,_tmp13C->type);
found=1;
break;}}}
# 543
if(!found){
if(allow_f1_subset_f2){
struct Cyc_List_List*_tmp547;common=((_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547->hd=_tmp13C,((_tmp547->tl=common,_tmp547))))));}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 549
{struct Cyc_List_List*_tmp141=fs1;for(0;(unsigned int)_tmp141;_tmp141=_tmp141->tl){
struct Cyc_Port_Cfield*_tmp142=(struct Cyc_Port_Cfield*)_tmp141->hd;
int found=0;
{struct Cyc_List_List*_tmp143=fs2;for(0;(unsigned int)_tmp143;_tmp143=_tmp143->tl){
struct Cyc_Port_Cfield*_tmp144=(struct Cyc_Port_Cfield*)_tmp143->hd;
if(Cyc_strptrcmp(_tmp142->f,_tmp144->f))found=1;}}
# 556
if(!found){
struct Cyc_List_List*_tmp548;common=((_tmp548=_cycalloc(sizeof(*_tmp548)),((_tmp548->hd=_tmp142,((_tmp548->tl=common,_tmp548))))));}}}
# 559
return common;}
# 562
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _tmp146;_push_handler(& _tmp146);{int _tmp148=0;if(setjmp(_tmp146.handler))_tmp148=1;if(!_tmp148){
# 569
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmp147=(void*)_exn_thrown;void*_tmp14A=_tmp147;void*_tmp14C;_LLB1: {struct Cyc_Port_Unify_ct_exn_struct*_tmp14B=(struct Cyc_Port_Unify_ct_exn_struct*)_tmp14A;if(_tmp14B->tag != Cyc_Port_Unify_ct)goto _LLB3;}_LLB2:
# 576
 return 0;_LLB3: _tmp14C=_tmp14A;_LLB4:(void)_rethrow(_tmp14C);_LLB0:;}};}
# 578
return 1;}struct _tuple12{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 584
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){
# 586
t=Cyc_Port_compress_ct(t);
{void*_tmp14D=t;_LLB6: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp14E=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp14D;if(_tmp14E->tag != 1)goto _LLB8;}_LLB7:
# 590
 goto _LLB9;_LLB8: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp14F=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp14D;if(_tmp14F->tag != 8)goto _LLBA;}_LLB9:
 goto _LLBB;_LLBA: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp150=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp14D;if(_tmp150->tag != 5)goto _LLBC;}_LLBB:
 goto _LLBD;_LLBC: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp151=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmp14D;if(_tmp151->tag != 2)goto _LLBE;}_LLBD:
 goto _LLBF;_LLBE: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp152=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_tmp14D;if(_tmp152->tag != 3)goto _LLC0;}_LLBF:
 goto _LLC1;_LLC0: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp153=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp14D;if(_tmp153->tag != 12)goto _LLC2;}_LLC1:
 goto _LLC3;_LLC2: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp154=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp14D;if(_tmp154->tag != 13)goto _LLC4;}_LLC3:
 goto _LLC5;_LLC4: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp155=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp14D;if(_tmp155->tag != 15)goto _LLC6;}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp156=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp14D;if(_tmp156->tag != 7)goto _LLC8;}_LLC7:
# 601
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;_LLC8: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp157=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp14D;if(_tmp157->tag != 4)goto _LLCA;}_LLC9:
# 606
 goto _LLCB;_LLCA: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp158=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp14D;if(_tmp158->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp159=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp14D;if(_tmp159->tag != 9)goto _LLCE;}_LLCD:
# 609
 return*uppers;_LLCE:;_LLCF:
 goto _LLB5;_LLB5:;}
# 613
{struct Cyc_List_List*_tmp15A=*uppers;for(0;(unsigned int)_tmp15A;_tmp15A=_tmp15A->tl){
void*_tmp15B=Cyc_Port_compress_ct((void*)_tmp15A->hd);
# 616
if(t == _tmp15B)return*uppers;{
struct _tuple10 _tmp549;struct _tuple10 _tmp15C=(_tmp549.f1=t,((_tmp549.f2=_tmp15B,_tmp549)));struct _tuple10 _tmp15D=_tmp15C;void*_tmp165;void*_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16B;void*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;_LLD1:{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp15E=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp15D.f1;if(_tmp15E->tag != 6)goto _LLD3;}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp15F=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp15D.f2;if(_tmp15F->tag != 11)goto _LLD3;};_LLD2:
# 621
 goto _LLD4;_LLD3:{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp160=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp15D.f1;if(_tmp160->tag != 6)goto _LLD5;}{struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp161=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp15D.f2;if(_tmp161->tag != 5)goto _LLD5;};_LLD4:
 goto _LLD6;_LLD5:{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp162=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp15D.f1;if(_tmp162->tag != 6)goto _LLD7;}{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp163=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp15D.f2;if(_tmp163->tag != 12)goto _LLD7;};_LLD6:
# 624
 return*uppers;_LLD7:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp164=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp15D.f1;if(_tmp164->tag != 11)goto _LLD9;else{_tmp165=(void*)_tmp164->f1;_tmp166=(void*)_tmp164->f2;_tmp167=(void*)_tmp164->f3;_tmp168=(void*)_tmp164->f4;_tmp169=(void*)_tmp164->f5;}}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp16A=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp15D.f2;if(_tmp16A->tag != 11)goto _LLD9;else{_tmp16B=(void*)_tmp16A->f1;_tmp16C=(void*)_tmp16A->f2;_tmp16D=(void*)_tmp16A->f3;_tmp16E=(void*)_tmp16A->f4;_tmp16F=(void*)_tmp16A->f5;}};_LLD8: {
# 629
struct _tuple12 _tmp54A;struct _tuple12 _tmp170=(_tmp54A.f1=Cyc_Port_var(),((_tmp54A.f2=Cyc_Port_var(),((_tmp54A.f3=Cyc_Port_var(),((_tmp54A.f4=Cyc_Port_var(),((_tmp54A.f5=Cyc_Port_var(),_tmp54A)))))))));void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;void*_tmp176;struct _tuple12 _tmp171=_tmp170;_tmp172=_tmp171.f1;_tmp173=_tmp171.f2;_tmp174=_tmp171.f3;_tmp175=_tmp171.f4;_tmp176=_tmp171.f5;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp54D;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp54C;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp177=(_tmp54C=_cycalloc(sizeof(*_tmp54C)),((_tmp54C[0]=((_tmp54D.tag=11,((_tmp54D.f1=_tmp172,((_tmp54D.f2=_tmp173,((_tmp54D.f3=_tmp174,((_tmp54D.f4=_tmp175,((_tmp54D.f5=_tmp176,_tmp54D)))))))))))),_tmp54C)));
Cyc_Port_leq(_tmp172,_tmp165);Cyc_Port_leq(_tmp172,_tmp16B);
Cyc_Port_leq(_tmp173,_tmp166);Cyc_Port_leq(_tmp173,_tmp16C);
Cyc_Port_leq(_tmp174,_tmp167);Cyc_Port_leq(_tmp174,_tmp16C);
Cyc_Port_leq(_tmp175,_tmp168);Cyc_Port_leq(_tmp175,_tmp16E);
Cyc_Port_leq(_tmp176,_tmp169);Cyc_Port_leq(_tmp176,_tmp16F);
_tmp15A->hd=(void*)((void*)_tmp177);
return*uppers;};}_LLD9:;_LLDA:
 goto _LLD0;_LLD0:;};}}{
# 641
struct Cyc_List_List*_tmp54E;return(_tmp54E=_cycalloc(sizeof(*_tmp54E)),((_tmp54E->hd=t,((_tmp54E->tl=*uppers,_tmp54E)))));};}
# 646
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){
# 648
t=Cyc_Port_compress_ct(t);
{void*_tmp17D=t;_LLDC: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp17E=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp17D;if(_tmp17E->tag != 0)goto _LLDE;}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp17F=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp17D;if(_tmp17F->tag != 8)goto _LLE0;}_LLDF:
 goto _LLE1;_LLE0: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp180=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmp17D;if(_tmp180->tag != 2)goto _LLE2;}_LLE1:
 goto _LLE3;_LLE2: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp181=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_tmp17D;if(_tmp181->tag != 3)goto _LLE4;}_LLE3:
 goto _LLE5;_LLE4: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp182=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp17D;if(_tmp182->tag != 4)goto _LLE6;}_LLE5:
 goto _LLE7;_LLE6: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp183=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp17D;if(_tmp183->tag != 13)goto _LLE8;}_LLE7:
 goto _LLE9;_LLE8: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp184=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp17D;if(_tmp184->tag != 15)goto _LLEA;}_LLE9:
 goto _LLEB;_LLEA: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp185=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp17D;if(_tmp185->tag != 10)goto _LLEC;}_LLEB:
# 658
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;_LLEC: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp186=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp17D;if(_tmp186->tag != 7)goto _LLEE;}_LLED:
 goto _LLEF;_LLEE: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp187=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp17D;if(_tmp187->tag != 1)goto _LLF0;}_LLEF:
 goto _LLF1;_LLF0: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp188=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp17D;if(_tmp188->tag != 9)goto _LLF2;}_LLF1:
# 664
 return*lowers;_LLF2:;_LLF3:
# 666
 goto _LLDB;_LLDB:;}
# 668
{struct Cyc_List_List*_tmp189=*lowers;for(0;(unsigned int)_tmp189;_tmp189=_tmp189->tl){
void*_tmp18A=Cyc_Port_compress_ct((void*)_tmp189->hd);
if(t == _tmp18A)return*lowers;{
struct _tuple10 _tmp54F;struct _tuple10 _tmp18B=(_tmp54F.f1=t,((_tmp54F.f2=_tmp18A,_tmp54F)));struct _tuple10 _tmp18C=_tmp18B;void*_tmp197;void*_tmp198;void*_tmp199;void*_tmp19A;void*_tmp19B;void*_tmp19D;void*_tmp19E;void*_tmp19F;void*_tmp1A0;void*_tmp1A1;_LLF5: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp18D=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp18C.f2;if(_tmp18D->tag != 4)goto _LLF7;}_LLF6:
 goto _LLF8;_LLF7:{struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp18E=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp18C.f1;if(_tmp18E->tag != 5)goto _LLF9;}{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp18F=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp18C.f2;if(_tmp18F->tag != 6)goto _LLF9;};_LLF8:
 goto _LLFA;_LLF9:{struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp190=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp18C.f1;if(_tmp190->tag != 5)goto _LLFB;}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp191=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp18C.f2;if(_tmp191->tag != 11)goto _LLFB;};_LLFA:
 goto _LLFC;_LLFB:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp192=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp18C.f1;if(_tmp192->tag != 11)goto _LLFD;}{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp193=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp18C.f2;if(_tmp193->tag != 6)goto _LLFD;};_LLFC:
 goto _LLFE;_LLFD:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp194=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp18C.f1;if(_tmp194->tag != 12)goto _LLFF;}{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp195=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp18C.f2;if(_tmp195->tag != 6)goto _LLFF;};_LLFE:
# 677
 return*lowers;_LLFF:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp196=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp18C.f1;if(_tmp196->tag != 11)goto _LL101;else{_tmp197=(void*)_tmp196->f1;_tmp198=(void*)_tmp196->f2;_tmp199=(void*)_tmp196->f3;_tmp19A=(void*)_tmp196->f4;_tmp19B=(void*)_tmp196->f5;}}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp19C=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp18C.f2;if(_tmp19C->tag != 11)goto _LL101;else{_tmp19D=(void*)_tmp19C->f1;_tmp19E=(void*)_tmp19C->f2;_tmp19F=(void*)_tmp19C->f3;_tmp1A0=(void*)_tmp19C->f4;_tmp1A1=(void*)_tmp19C->f5;}};_LL100: {
# 682
struct _tuple12 _tmp550;struct _tuple12 _tmp1A2=(_tmp550.f1=Cyc_Port_var(),((_tmp550.f2=Cyc_Port_var(),((_tmp550.f3=Cyc_Port_var(),((_tmp550.f4=Cyc_Port_var(),((_tmp550.f5=Cyc_Port_var(),_tmp550)))))))));void*_tmp1A4;void*_tmp1A5;void*_tmp1A6;void*_tmp1A7;void*_tmp1A8;struct _tuple12 _tmp1A3=_tmp1A2;_tmp1A4=_tmp1A3.f1;_tmp1A5=_tmp1A3.f2;_tmp1A6=_tmp1A3.f3;_tmp1A7=_tmp1A3.f4;_tmp1A8=_tmp1A3.f5;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp553;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp552;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1A9=(_tmp552=_cycalloc(sizeof(*_tmp552)),((_tmp552[0]=((_tmp553.tag=11,((_tmp553.f1=_tmp1A4,((_tmp553.f2=_tmp1A5,((_tmp553.f3=_tmp1A6,((_tmp553.f4=_tmp1A7,((_tmp553.f5=_tmp1A8,_tmp553)))))))))))),_tmp552)));
Cyc_Port_leq(_tmp197,_tmp1A4);Cyc_Port_leq(_tmp19D,_tmp1A4);
Cyc_Port_leq(_tmp198,_tmp1A5);Cyc_Port_leq(_tmp19E,_tmp1A5);
Cyc_Port_leq(_tmp199,_tmp1A6);Cyc_Port_leq(_tmp19E,_tmp1A6);
Cyc_Port_leq(_tmp19A,_tmp1A7);Cyc_Port_leq(_tmp1A0,_tmp1A7);
Cyc_Port_leq(_tmp19B,_tmp1A8);Cyc_Port_leq(_tmp1A1,_tmp1A8);
_tmp189->hd=(void*)((void*)_tmp1A9);
return*lowers;};}_LL101:;_LL102:
 goto _LLF4;_LLF4:;};}}{
# 694
struct Cyc_List_List*_tmp554;return(_tmp554=_cycalloc(sizeof(*_tmp554)),((_tmp554->hd=t,((_tmp554->tl=*lowers,_tmp554)))));};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 701
static int Cyc_Port_leq(void*t1,void*t2){
# 707
if(t1 == t2)return 1;
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{
struct _tuple10 _tmp555;struct _tuple10 _tmp1AF=(_tmp555.f1=t1,((_tmp555.f2=t2,_tmp555)));struct _tuple10 _tmp1B0=_tmp1AF;struct _dyneither_ptr _tmp1B3;struct _dyneither_ptr _tmp1B5;struct _dyneither_ptr _tmp1B7;void*_tmp1D5;void*_tmp1D6;void*_tmp1D7;void*_tmp1D8;void*_tmp1D9;void*_tmp1DB;void*_tmp1DC;void*_tmp1DD;void*_tmp1DE;void*_tmp1DF;void*_tmp1E1;void*_tmp1E2;void*_tmp1E3;void*_tmp1E5;void*_tmp1E6;void*_tmp1E7;void*_tmp1E9;void*_tmp1EA;void*_tmp1EB;void*_tmp1ED;void*_tmp1EE;void*_tmp1EF;void*_tmp1F0;struct Cyc_Port_Cvar*_tmp1F2;struct Cyc_Port_Cvar*_tmp1F4;struct Cyc_Port_Cvar*_tmp1F6;struct Cyc_Port_Cvar*_tmp1F8;_LL104: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp1B1=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1B1->tag != 7)goto _LL106;}_LL105:
 return 1;_LL106:{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp1B2=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1B2->tag != 10)goto _LL108;else{_tmp1B3=*_tmp1B2->f1;}}{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp1B4=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1B4->tag != 10)goto _LL108;else{_tmp1B5=*_tmp1B4->f1;}};_LL107:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1B3,(struct _dyneither_ptr)_tmp1B5)== 0;_LL108:{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp1B6=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1B6->tag != 10)goto _LL10A;else{_tmp1B7=*_tmp1B6->f1;}}{struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp1B8=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1B8->tag != 7)goto _LL10A;};_LL109:
 return 0;_LL10A:{struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp1B9=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1B9->tag != 1)goto _LL10C;}{struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp1BA=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1BA->tag != 0)goto _LL10C;};_LL10B:
 return 1;_LL10C:{struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp1BB=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1BB->tag != 0)goto _LL10E;}{struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp1BC=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1BC->tag != 1)goto _LL10E;};_LL10D:
 return 0;_LL10E:{struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp1BD=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1BD->tag != 9)goto _LL110;}{struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp1BE=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1BE->tag != 8)goto _LL110;};_LL10F:
 return 0;_LL110:{struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp1BF=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1BF->tag != 8)goto _LL112;}{struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp1C0=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1C0->tag != 9)goto _LL112;};_LL111:
 return 1;_LL112:{struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp1C1=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1C1->tag != 16)goto _LL114;}{struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp1C2=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1C2->tag != 0)goto _LL114;};_LL113:
 return 1;_LL114:{struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp1C3=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1C3->tag != 16)goto _LL116;}{struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp1C4=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1C4->tag != 4)goto _LL116;};_LL115:
 return 1;_LL116: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp1C5=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1C5->tag != 4)goto _LL118;}_LL117:
 return 0;_LL118:{struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp1C6=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1C6->tag != 5)goto _LL11A;}{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp1C7=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1C7->tag != 6)goto _LL11A;};_LL119:
 return 1;_LL11A:{struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp1C8=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1C8->tag != 5)goto _LL11C;}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1C9=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1C9->tag != 11)goto _LL11C;};_LL11B:
 return 1;_LL11C:{struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp1CA=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1CA->tag != 5)goto _LL11E;}{struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp1CB=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1CB->tag != 4)goto _LL11E;};_LL11D:
 return 1;_LL11E:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1CC=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1CC->tag != 11)goto _LL120;}{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp1CD=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1CD->tag != 6)goto _LL120;};_LL11F:
 return 1;_LL120:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1CE=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1CE->tag != 11)goto _LL122;}{struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp1CF=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1CF->tag != 4)goto _LL122;};_LL121:
 return 1;_LL122:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp1D0=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1D0->tag != 12)goto _LL124;}{struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp1D1=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1D1->tag != 6)goto _LL124;};_LL123:
 return 1;_LL124:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp1D2=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1D2->tag != 12)goto _LL126;}{struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp1D3=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1D3->tag != 4)goto _LL126;};_LL125:
 return 1;_LL126:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1D4=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1D4->tag != 11)goto _LL128;else{_tmp1D5=(void*)_tmp1D4->f1;_tmp1D6=(void*)_tmp1D4->f2;_tmp1D7=(void*)_tmp1D4->f3;_tmp1D8=(void*)_tmp1D4->f4;_tmp1D9=(void*)_tmp1D4->f5;}}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1DA=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1DA->tag != 11)goto _LL128;else{_tmp1DB=(void*)_tmp1DA->f1;_tmp1DC=(void*)_tmp1DA->f2;_tmp1DD=(void*)_tmp1DA->f3;_tmp1DE=(void*)_tmp1DA->f4;_tmp1DF=(void*)_tmp1DA->f5;}};_LL127:
# 729
 return(((Cyc_Port_leq(_tmp1D5,_tmp1DB) && Cyc_Port_leq(_tmp1D6,_tmp1DC)) && Cyc_Port_unifies(_tmp1D7,_tmp1DD)) && Cyc_Port_leq(_tmp1D8,_tmp1DE)) && 
Cyc_Port_leq(_tmp1D9,_tmp1DF);_LL128:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp1E0=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1E0->tag != 12)goto _LL12A;else{_tmp1E1=(void*)_tmp1E0->f1;_tmp1E2=(void*)_tmp1E0->f2;_tmp1E3=(void*)_tmp1E0->f3;}}{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp1E4=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1E4->tag != 12)goto _LL12A;else{_tmp1E5=(void*)_tmp1E4->f1;_tmp1E6=(void*)_tmp1E4->f2;_tmp1E7=(void*)_tmp1E4->f3;}};_LL129:
# 732
 return(Cyc_Port_leq(_tmp1E1,_tmp1E5) && Cyc_Port_leq(_tmp1E2,_tmp1E6)) && Cyc_Port_leq(_tmp1E3,_tmp1E7);_LL12A:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp1E8=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1E8->tag != 12)goto _LL12C;else{_tmp1E9=(void*)_tmp1E8->f1;_tmp1EA=(void*)_tmp1E8->f2;_tmp1EB=(void*)_tmp1E8->f3;}}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1EC=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1EC->tag != 11)goto _LL12C;else{_tmp1ED=(void*)_tmp1EC->f1;_tmp1EE=(void*)_tmp1EC->f2;_tmp1EF=(void*)_tmp1EC->f3;_tmp1F0=(void*)_tmp1EC->f5;}};_LL12B:
# 734
 return((Cyc_Port_leq(_tmp1E9,_tmp1ED) && Cyc_Port_leq(_tmp1EA,_tmp1EE)) && Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,_tmp1EF)) && 
Cyc_Port_leq(_tmp1EB,_tmp1F0);_LL12C:{struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp1F1=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1F1->tag != 16)goto _LL12E;else{_tmp1F2=_tmp1F1->f1;}}{struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp1F3=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1F3->tag != 16)goto _LL12E;else{_tmp1F4=_tmp1F3->f1;}};_LL12D:
# 737
 _tmp1F2->uppers=Cyc_Port_filter_self(t1,_tmp1F2->uppers);
_tmp1F4->lowers=Cyc_Port_filter_self(t2,_tmp1F4->lowers);
_tmp1F2->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp1F2->uppers);
_tmp1F4->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp1F4->lowers);
return 1;_LL12E: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp1F5=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp1B0.f1;if(_tmp1F5->tag != 16)goto _LL130;else{_tmp1F6=_tmp1F5->f1;}}_LL12F:
# 743
 _tmp1F6->uppers=Cyc_Port_filter_self(t1,_tmp1F6->uppers);
_tmp1F6->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp1F6->uppers);
return 1;_LL130: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp1F7=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp1B0.f2;if(_tmp1F7->tag != 16)goto _LL132;else{_tmp1F8=_tmp1F7->f1;}}_LL131:
# 747
 _tmp1F8->lowers=Cyc_Port_filter_self(t2,_tmp1F8->lowers);
_tmp1F8->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp1F8->lowers);
return 1;_LL132:;_LL133:
 return Cyc_Port_unifies(t1,t2);_LL103:;};}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*edits;int porting;};
# 756
typedef struct Cyc_Port_GlobalCenv*Cyc_Port_gcenv_t;
# 779
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos  = 0,Cyc_Port_FnArg_pos  = 1,Cyc_Port_FnBody_pos  = 2,Cyc_Port_Toplevel_pos  = 3};
typedef enum Cyc_Port_CPos Cyc_Port_cpos_t;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 782
typedef struct Cyc_Port_Cenv*Cyc_Port_cenv_t;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;
# 792
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){
struct Cyc_Port_GlobalCenv*_tmp556;struct Cyc_Port_GlobalCenv*g=(_tmp556=_cycalloc(sizeof(*_tmp556)),((_tmp556->typedef_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp556->struct_dict=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp556->union_dict=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp556->qualifier_edits=0,((_tmp556->pointer_edits=0,((_tmp556->zeroterm_edits=0,((_tmp556->edits=0,((_tmp556->porting=0,((_tmp556->return_type=
# 801
Cyc_Port_void_ct(),_tmp556)))))))))))))))))));
struct Cyc_Port_Cenv*_tmp557;return(_tmp557=_cycalloc(sizeof(*_tmp557)),((_tmp557->gcenv=g,((_tmp557->cpos=Cyc_Port_Toplevel_pos,((_tmp557->var_dict=
# 804
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),_tmp557)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 810
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnArg_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 813
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnRes_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 816
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_Toplevel_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 819
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
return(env->gcenv)->return_type;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 822
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp1FC;_push_handler(& _tmp1FC);{int _tmp1FE=0;if(setjmp(_tmp1FC.handler))_tmp1FE=1;if(!_tmp1FE){
{struct _tuple10 _tmp1FF=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);void*_tmp201;struct _tuple10 _tmp200=_tmp1FF;_tmp201=_tmp200.f1;{
void*_tmp202=_tmp201;_npop_handler(0);return _tmp202;};}
# 824
;_pop_handler();}else{void*_tmp1FD=(void*)_exn_thrown;void*_tmp204=_tmp1FD;void*_tmp206;_LL135: {struct Cyc_Dict_Absent_exn_struct*_tmp205=(struct Cyc_Dict_Absent_exn_struct*)_tmp204;if(_tmp205->tag != Cyc_Dict_Absent)goto _LL137;}_LL136:
# 831
 return Cyc_Absyn_sint_typ;_LL137: _tmp206=_tmp204;_LL138:(void)_rethrow(_tmp206);_LL134:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 836
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp207;_push_handler(& _tmp207);{int _tmp209=0;if(setjmp(_tmp207.handler))_tmp209=1;if(!_tmp209){
{struct _tuple10 _tmp20A=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);void*_tmp20C;struct _tuple10 _tmp20B=_tmp20A;_tmp20C=_tmp20B.f2;{
void*_tmp20D=_tmp20C;_npop_handler(0);return _tmp20D;};}
# 838
;_pop_handler();}else{void*_tmp208=(void*)_exn_thrown;void*_tmp20F=_tmp208;void*_tmp211;_LL13A: {struct Cyc_Dict_Absent_exn_struct*_tmp210=(struct Cyc_Dict_Absent_exn_struct*)_tmp20F;if(_tmp210->tag != Cyc_Dict_Absent)goto _LL13C;}_LL13B:
# 845
 return Cyc_Port_var();_LL13C: _tmp211=_tmp20F;_LL13D:(void)_rethrow(_tmp211);_LL139:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 851
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 853
struct _tuple11**_tmp212=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);
if((unsigned int)_tmp212)
return*_tmp212;else{
# 857
struct Cyc_Absyn_Aggrdecl*_tmp558;struct Cyc_Absyn_Aggrdecl*_tmp213=(_tmp558=_cycalloc(sizeof(*_tmp558)),((_tmp558->kind=Cyc_Absyn_StructA,((_tmp558->sc=Cyc_Absyn_Public,((_tmp558->name=n,((_tmp558->tvs=0,((_tmp558->impl=0,((_tmp558->attributes=0,((_tmp558->expected_mem_kind=0,_tmp558)))))))))))))));
# 860
struct _tuple11*_tmp559;struct _tuple11*p=(_tmp559=_cycalloc(sizeof(*_tmp559)),((_tmp559->f1=_tmp213,((_tmp559->f2=0,_tmp559)))));
(env->gcenv)->struct_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);
return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 868
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 870
struct _tuple11**_tmp216=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);
if((unsigned int)_tmp216)
return*_tmp216;else{
# 874
struct Cyc_Absyn_Aggrdecl*_tmp55A;struct Cyc_Absyn_Aggrdecl*_tmp217=(_tmp55A=_cycalloc(sizeof(*_tmp55A)),((_tmp55A->kind=Cyc_Absyn_UnionA,((_tmp55A->sc=Cyc_Absyn_Public,((_tmp55A->name=n,((_tmp55A->tvs=0,((_tmp55A->impl=0,((_tmp55A->attributes=0,((_tmp55A->expected_mem_kind=0,_tmp55A)))))))))))))));
# 877
struct _tuple11*_tmp55B;struct _tuple11*p=(_tmp55B=_cycalloc(sizeof(*_tmp55B)),((_tmp55B->f1=_tmp217,((_tmp55B->f2=0,_tmp55B)))));
(env->gcenv)->union_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);
return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple13{void*f1;struct _tuple10*f2;};
# 884
static struct _tuple10 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp21A;_push_handler(& _tmp21A);{int _tmp21C=0;if(setjmp(_tmp21A.handler))_tmp21C=1;if(!_tmp21C){
{struct _tuple13 _tmp21D=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple10*_tmp21F;struct _tuple13 _tmp21E=_tmp21D;_tmp21F=_tmp21E.f2;{
struct _tuple10 _tmp220=*_tmp21F;_npop_handler(0);return _tmp220;};}
# 886
;_pop_handler();}else{void*_tmp21B=(void*)_exn_thrown;void*_tmp222=_tmp21B;void*_tmp224;_LL13F: {struct Cyc_Dict_Absent_exn_struct*_tmp223=(struct Cyc_Dict_Absent_exn_struct*)_tmp222;if(_tmp223->tag != Cyc_Dict_Absent)goto _LL141;}_LL140: {
# 893
struct _tuple10 _tmp55C;return(_tmp55C.f1=Cyc_Port_var(),((_tmp55C.f2=Cyc_Port_var(),_tmp55C)));}_LL141: _tmp224=_tmp222;_LL142:(void)_rethrow(_tmp224);_LL13E:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 896
static struct _tuple13*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 900
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 904
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp226;_push_handler(& _tmp226);{int _tmp228=0;if(setjmp(_tmp226.handler))_tmp228=1;if(!_tmp228){
{struct _tuple13 _tmp229=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);void*_tmp22B;struct _tuple13 _tmp22A=_tmp229;_tmp22B=_tmp22A.f1;
LOOP: {void*_tmp22C=_tmp22B;struct _tuple0*_tmp22E;_LL144: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp22C;if(_tmp22D->tag != 17)goto _LL146;else{_tmp22E=_tmp22D->f1;}}_LL145:
 _tmp22B=Cyc_Port_lookup_typedef(env,_tmp22E);goto LOOP;_LL146: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp22C;if(_tmp22F->tag != 9)goto _LL148;}_LL147: {
int _tmp230=1;_npop_handler(0);return _tmp230;}_LL148:;_LL149: {
int _tmp231=0;_npop_handler(0);return _tmp231;}_LL143:;}}
# 906
;_pop_handler();}else{void*_tmp227=(void*)_exn_thrown;void*_tmp233=_tmp227;void*_tmp235;_LL14B: {struct Cyc_Dict_Absent_exn_struct*_tmp234=(struct Cyc_Dict_Absent_exn_struct*)_tmp233;if(_tmp234->tag != Cyc_Dict_Absent)goto _LL14D;}_LL14C:
# 917
 return 0;_LL14D: _tmp235=_tmp233;_LL14E:(void)_rethrow(_tmp235);_LL14A:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 921
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp236;_push_handler(& _tmp236);{int _tmp238=0;if(setjmp(_tmp236.handler))_tmp238=1;if(!_tmp238){
{struct _tuple13 _tmp239=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);void*_tmp23B;struct _tuple13 _tmp23A=_tmp239;_tmp23B=_tmp23A.f1;
LOOP: {void*_tmp23C=_tmp23B;struct _tuple0*_tmp23E;_LL150: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp23D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23C;if(_tmp23D->tag != 17)goto _LL152;else{_tmp23E=_tmp23D->f1;}}_LL151:
 _tmp23B=Cyc_Port_lookup_typedef(env,_tmp23E);goto LOOP;_LL152: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C;if(_tmp23F->tag != 8)goto _LL154;}_LL153: {
int _tmp240=1;_npop_handler(0);return _tmp240;}_LL154:;_LL155: {
int _tmp241=0;_npop_handler(0);return _tmp241;}_LL14F:;}}
# 923
;_pop_handler();}else{void*_tmp237=(void*)_exn_thrown;void*_tmp243=_tmp237;void*_tmp245;_LL157: {struct Cyc_Dict_Absent_exn_struct*_tmp244=(struct Cyc_Dict_Absent_exn_struct*)_tmp243;if(_tmp244->tag != Cyc_Dict_Absent)goto _LL159;}_LL158:
# 934
 return 0;_LL159: _tmp245=_tmp243;_LL15A:(void)_rethrow(_tmp245);_LL156:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 940
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){
struct Cyc_Port_Cenv*_tmp55D;return(_tmp55D=_cycalloc(sizeof(*_tmp55D)),((_tmp55D->gcenv=env->gcenv,((_tmp55D->var_dict=env->var_dict,((_tmp55D->cpos=cpos,_tmp55D)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 945
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
(env->gcenv)->return_type=t;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 950
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype){
# 952
struct _tuple13*_tmp563;struct _tuple10*_tmp562;struct Cyc_Port_Cenv*_tmp561;return(_tmp561=_cycalloc(sizeof(*_tmp561)),((_tmp561->gcenv=env->gcenv,((_tmp561->var_dict=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(env->var_dict,x,((_tmp563=_cycalloc(sizeof(*_tmp563)),((_tmp563->f1=t,((_tmp563->f2=((_tmp562=_cycalloc(sizeof(*_tmp562)),((_tmp562->f1=qual,((_tmp562->f2=ctype,_tmp562)))))),_tmp563))))))),((_tmp561->cpos=env->cpos,_tmp561)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 958
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
struct _tuple10*_tmp564;(env->gcenv)->typedef_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->typedef_dict,n,(
(_tmp564=_cycalloc(sizeof(*_tmp564)),((_tmp564->f1=t,((_tmp564->f2=ct,_tmp564)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple14{void*f1;void*f2;unsigned int f3;};
# 965
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned int loc){
# 967
struct _tuple14*_tmp567;struct Cyc_List_List*_tmp566;(env->gcenv)->qualifier_edits=((_tmp566=_cycalloc(sizeof(*_tmp566)),((_tmp566->hd=((_tmp567=_cycalloc(sizeof(*_tmp567)),((_tmp567->f1=new_qual,((_tmp567->f2=orig_qual,((_tmp567->f3=loc,_tmp567)))))))),((_tmp566->tl=(env->gcenv)->qualifier_edits,_tmp566))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 973
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned int loc){
# 975
struct _tuple14*_tmp56A;struct Cyc_List_List*_tmp569;(env->gcenv)->pointer_edits=((_tmp569=_cycalloc(sizeof(*_tmp569)),((_tmp569->hd=((_tmp56A=_cycalloc(sizeof(*_tmp56A)),((_tmp56A->f1=new_ptr,((_tmp56A->f2=orig_ptr,((_tmp56A->f3=loc,_tmp56A)))))))),((_tmp569->tl=(env->gcenv)->pointer_edits,_tmp569))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 979
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned int loc){
# 981
struct _tuple14*_tmp56D;struct Cyc_List_List*_tmp56C;(env->gcenv)->zeroterm_edits=((_tmp56C=_cycalloc(sizeof(*_tmp56C)),((_tmp56C->hd=((_tmp56D=_cycalloc(sizeof(*_tmp56D)),((_tmp56D->f1=new_zt,((_tmp56D->f2=orig_zt,((_tmp56D->f3=loc,_tmp56D)))))))),((_tmp56C->tl=(env->gcenv)->zeroterm_edits,_tmp56C))))));}
# 987
static void*Cyc_Port_main_type(){
struct _tuple8*_tmp56E;struct _tuple8*arg1=
(_tmp56E=_cycalloc(sizeof(*_tmp56E)),((_tmp56E->f1=0,((_tmp56E->f2=Cyc_Absyn_empty_tqual(0),((_tmp56E->f3=Cyc_Absyn_sint_typ,_tmp56E)))))));
struct _tuple8*_tmp56F;struct _tuple8*arg2=
(_tmp56F=_cycalloc(sizeof(*_tmp56F)),((_tmp56F->f1=0,((_tmp56F->f2=Cyc_Absyn_empty_tqual(0),((_tmp56F->f3=
Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0)),Cyc_Absyn_wildtyp(0),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()),_tmp56F)))))));
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp579;struct _tuple8*_tmp578[2];struct Cyc_Absyn_FnInfo _tmp577;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp576;return(void*)((_tmp576=_cycalloc(sizeof(*_tmp576)),((_tmp576[0]=((_tmp579.tag=9,((_tmp579.f1=((_tmp577.tvars=0,((_tmp577.effect=0,((_tmp577.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp577.ret_typ=Cyc_Absyn_sint_typ,((_tmp577.args=(
# 997
(_tmp578[1]=arg2,((_tmp578[0]=arg1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp578,sizeof(struct _tuple8*),2)))))),((_tmp577.c_varargs=0,((_tmp577.cyc_varargs=0,((_tmp577.rgn_po=0,((_tmp577.attributes=0,((_tmp577.requires_clause=0,((_tmp577.requires_relns=0,((_tmp577.ensures_clause=0,((_tmp577.ensures_relns=0,_tmp577)))))))))))))))))))))))))),_tmp579)))),_tmp576))));}struct Cyc_Port_Cenv;
# 1006
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1009
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_Cenv*_tmp257=Cyc_Port_empty_cenv();
# 1012
{struct _tuple0*_tmp57A;_tmp257=Cyc_Port_add_var(_tmp257,((_tmp57A=_cycalloc(sizeof(*_tmp57A)),((_tmp57A->f1=Cyc_Absyn_Loc_n,((_tmp57A->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"main",sizeof(char),5),_tmp57A)))))),Cyc_Port_main_type(),Cyc_Port_const_ct(),
Cyc_Port_type_to_ctype(_tmp257,Cyc_Port_main_type()));}
return _tmp257;}
# 1020
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp25A=Cyc_Port_compress_ct(t);void*_tmp25B=_tmp25A;struct _dyneither_ptr*_tmp26A;void*_tmp26C;void*_tmp26D;void*_tmp26E;void*_tmp26F;void*_tmp270;void*_tmp272;void*_tmp273;void*_tmp274;void*_tmp276;struct Cyc_List_List*_tmp277;_LL15C: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp25C=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp25B;if(_tmp25C->tag != 0)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp25D=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp25B;if(_tmp25D->tag != 1)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp25E=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmp25B;if(_tmp25E->tag != 2)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp25F=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_tmp25B;if(_tmp25F->tag != 3)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp260=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp25B;if(_tmp260->tag != 4)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp261=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp25B;if(_tmp261->tag != 5)goto _LL168;}_LL167:
 goto _LL169;_LL168: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp262=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp25B;if(_tmp262->tag != 6)goto _LL16A;}_LL169:
 goto _LL16B;_LL16A: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp263=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp25B;if(_tmp263->tag != 14)goto _LL16C;}_LL16B:
 goto _LL16D;_LL16C: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp264=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp25B;if(_tmp264->tag != 13)goto _LL16E;}_LL16D:
 goto _LL16F;_LL16E: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp265=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp25B;if(_tmp265->tag != 16)goto _LL170;}_LL16F:
 goto _LL171;_LL170: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp266=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp25B;if(_tmp266->tag != 8)goto _LL172;}_LL171:
 goto _LL173;_LL172: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp267=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp25B;if(_tmp267->tag != 9)goto _LL174;}_LL173:
 goto _LL175;_LL174: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp268=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp25B;if(_tmp268->tag != 7)goto _LL176;}_LL175:
 return;_LL176: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp269=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp25B;if(_tmp269->tag != 10)goto _LL178;else{_tmp26A=_tmp269->f1;}}_LL177:
# 1036
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,_tmp26A)){
int*_tmp57B;*cts=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,int*v))Cyc_Dict_insert)(*cts,_tmp26A,((_tmp57B=_cycalloc_atomic(sizeof(*_tmp57B)),((_tmp57B[0]=0,_tmp57B)))));}{
int*_tmp279=((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*cts,_tmp26A);
*_tmp279=*_tmp279 + 1;
return;};_LL178: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp26B=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp25B;if(_tmp26B->tag != 11)goto _LL17A;else{_tmp26C=(void*)_tmp26B->f1;_tmp26D=(void*)_tmp26B->f2;_tmp26E=(void*)_tmp26B->f3;_tmp26F=(void*)_tmp26B->f4;_tmp270=(void*)_tmp26B->f5;}}_LL179:
# 1042
 Cyc_Port_region_counts(cts,_tmp26C);
Cyc_Port_region_counts(cts,_tmp26F);
return;_LL17A: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp271=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp25B;if(_tmp271->tag != 12)goto _LL17C;else{_tmp272=(void*)_tmp271->f1;_tmp273=(void*)_tmp271->f2;_tmp274=(void*)_tmp271->f3;}}_LL17B:
# 1046
 Cyc_Port_region_counts(cts,_tmp272);
return;_LL17C: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp275=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp25B;if(_tmp275->tag != 15)goto _LL15B;else{_tmp276=(void*)_tmp275->f1;_tmp277=_tmp275->f2;}}_LL17D:
# 1049
 Cyc_Port_region_counts(cts,_tmp276);
for(0;(unsigned int)_tmp277;_tmp277=_tmp277->tl){Cyc_Port_region_counts(cts,(void*)_tmp277->hd);}
return;_LL15B:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1060
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1062
c=Cyc_Port_compress_ct(c);{
struct _tuple10 _tmp57C;struct _tuple10 _tmp27A=(_tmp57C.f1=t,((_tmp57C.f2=c,_tmp57C)));struct _tuple10 _tmp27B=_tmp27A;void*_tmp27D;void*_tmp27E;struct Cyc_Absyn_PtrLoc*_tmp27F;void*_tmp281;void*_tmp282;void*_tmp284;void*_tmp286;void*_tmp288;struct Cyc_List_List*_tmp289;void*_tmp28B;struct Cyc_List_List*_tmp28C;_LL17F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp27C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27B.f1;if(_tmp27C->tag != 5)goto _LL181;else{_tmp27D=(_tmp27C->f1).elt_typ;_tmp27E=((_tmp27C->f1).ptr_atts).rgn;_tmp27F=((_tmp27C->f1).ptr_atts).ptrloc;}}{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp280=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp27B.f2;if(_tmp280->tag != 11)goto _LL181;else{_tmp281=(void*)_tmp280->f1;_tmp282=(void*)_tmp280->f4;}};_LL180:
# 1065
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp27D,_tmp281);{
unsigned int _tmp28D=(unsigned int)_tmp27F?_tmp27F->rgn_loc:(unsigned int)0;
_tmp282=Cyc_Port_compress_ct(_tmp282);
{struct _tuple10 _tmp57D;struct _tuple10 _tmp28E=(_tmp57D.f1=_tmp27E,((_tmp57D.f2=_tmp282,_tmp57D)));struct _tuple10 _tmp28F=_tmp28E;struct _dyneither_ptr*_tmp294;_LL188:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp290=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28F.f1;if(_tmp290->tag != 1)goto _LL18A;}{struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp291=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp28F.f2;if(_tmp291->tag != 7)goto _LL18A;};if(!(!fn_res))goto _LL18A;_LL189:
# 1070
{struct Cyc_Port_Edit*_tmp586;const char*_tmp585;const char*_tmp584;struct Cyc_List_List*_tmp583;(env->gcenv)->edits=(
(_tmp583=_cycalloc(sizeof(*_tmp583)),((_tmp583->hd=((_tmp586=_cycalloc(sizeof(*_tmp586)),((_tmp586->loc=_tmp28D,((_tmp586->old_string=((_tmp585="`H ",_tag_dyneither(_tmp585,sizeof(char),4))),((_tmp586->new_string=((_tmp584="",_tag_dyneither(_tmp584,sizeof(char),1))),_tmp586)))))))),((_tmp583->tl=(env->gcenv)->edits,_tmp583))))));}
goto _LL187;_LL18A:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp292=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28F.f1;if(_tmp292->tag != 1)goto _LL18C;}{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp293=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp28F.f2;if(_tmp293->tag != 10)goto _LL18C;else{_tmp294=_tmp293->f1;}};_LL18B: {
# 1074
int _tmp299=*((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(counts,_tmp294);
if(_tmp299 > 1){
struct Cyc_Port_Edit*_tmp596;const char*_tmp595;void*_tmp594[1];struct Cyc_String_pa_PrintArg_struct _tmp593;const char*_tmp592;struct Cyc_List_List*_tmp591;(env->gcenv)->edits=(
(_tmp591=_cycalloc(sizeof(*_tmp591)),((_tmp591->hd=((_tmp596=_cycalloc(sizeof(*_tmp596)),((_tmp596->loc=_tmp28D,((_tmp596->old_string=(struct _dyneither_ptr)((_tmp593.tag=0,((_tmp593.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp294),((_tmp594[0]=& _tmp593,Cyc_aprintf(((_tmp595="%s ",_tag_dyneither(_tmp595,sizeof(char),4))),_tag_dyneither(_tmp594,sizeof(void*),1)))))))),((_tmp596->new_string=((_tmp592="",_tag_dyneither(_tmp592,sizeof(char),1))),_tmp596)))))))),((_tmp591->tl=(env->gcenv)->edits,_tmp591))))));}
goto _LL187;}_LL18C:;_LL18D:
 goto _LL187;_LL187:;}
# 1081
goto _LL17E;};_LL181:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp283=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp27B.f1;if(_tmp283->tag != 8)goto _LL183;else{_tmp284=(_tmp283->f1).elt_type;}}{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp285=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp27B.f2;if(_tmp285->tag != 12)goto _LL183;else{_tmp286=(void*)_tmp285->f1;}};_LL182:
# 1083
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp284,_tmp286);
goto _LL17E;_LL183:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp287=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp27B.f1;if(_tmp287->tag != 9)goto _LL185;else{_tmp288=(_tmp287->f1).ret_typ;_tmp289=(_tmp287->f1).args;}}{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp28A=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp27B.f2;if(_tmp28A->tag != 15)goto _LL185;else{_tmp28B=(void*)_tmp28A->f1;_tmp28C=_tmp28A->f2;}};_LL184:
# 1086
 Cyc_Port_register_rgns(env,counts,1,_tmp288,_tmp28B);
for(0;_tmp289 != 0  && _tmp28C != 0;(_tmp289=_tmp289->tl,_tmp28C=_tmp28C->tl)){
struct _tuple8 _tmp2A1=*((struct _tuple8*)_tmp289->hd);void*_tmp2A3;struct _tuple8 _tmp2A2=_tmp2A1;_tmp2A3=_tmp2A2.f3;{
void*_tmp2A4=(void*)_tmp28C->hd;
Cyc_Port_register_rgns(env,counts,0,_tmp2A3,_tmp2A4);};}
# 1092
goto _LL17E;_LL185:;_LL186:
 goto _LL17E;_LL17E:;};}struct Cyc_Port_Cenv;
# 1099
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);struct Cyc_Port_Cenv;
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1104
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp2A6=t;struct _tuple0*_tmp2A8;enum Cyc_Absyn_Size_of _tmp2AA;_LL18F: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2A7=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A6;if(_tmp2A7->tag != 17)goto _LL191;else{_tmp2A8=_tmp2A7->f1;}}_LL190:
# 1107
(*_tmp2A8).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_is_char(env,Cyc_Port_lookup_typedef(env,_tmp2A8));_LL191: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2A6;if(_tmp2A9->tag != 6)goto _LL193;else{_tmp2AA=_tmp2A9->f2;}}_LL192:
 return 1;_LL193:;_LL194:
 return 0;_LL18E:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1115
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp2AB=t;struct _tuple0*_tmp2AD;void*_tmp2B0;struct Cyc_Absyn_Tqual _tmp2B1;void*_tmp2B2;union Cyc_Absyn_Constraint*_tmp2B3;union Cyc_Absyn_Constraint*_tmp2B4;union Cyc_Absyn_Constraint*_tmp2B5;struct Cyc_Absyn_PtrLoc*_tmp2B6;void*_tmp2BA;struct Cyc_Absyn_Tqual _tmp2BB;union Cyc_Absyn_Constraint*_tmp2BC;unsigned int _tmp2BD;void*_tmp2BF;struct Cyc_List_List*_tmp2C0;union Cyc_Absyn_AggrInfoU _tmp2C2;struct Cyc_List_List*_tmp2C5;_LL196: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2AC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2AB;if(_tmp2AC->tag != 17)goto _LL198;else{_tmp2AD=_tmp2AC->f1;}}_LL197:
# 1118
(*_tmp2AD).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,_tmp2AD);_LL198: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2AE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2AB;if(_tmp2AE->tag != 0)goto _LL19A;}_LL199:
 return Cyc_Port_void_ct();_LL19A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AB;if(_tmp2AF->tag != 5)goto _LL19C;else{_tmp2B0=(_tmp2AF->f1).elt_typ;_tmp2B1=(_tmp2AF->f1).elt_tq;_tmp2B2=((_tmp2AF->f1).ptr_atts).rgn;_tmp2B3=((_tmp2AF->f1).ptr_atts).nullable;_tmp2B4=((_tmp2AF->f1).ptr_atts).bounds;_tmp2B5=((_tmp2AF->f1).ptr_atts).zero_term;_tmp2B6=((_tmp2AF->f1).ptr_atts).ptrloc;}}_LL19B: {
# 1122
unsigned int _tmp2C6=(unsigned int)_tmp2B6?_tmp2B6->ptr_loc:(unsigned int)0;
unsigned int _tmp2C7=(unsigned int)_tmp2B6?_tmp2B6->rgn_loc:(unsigned int)0;
unsigned int _tmp2C8=(unsigned int)_tmp2B6?_tmp2B6->zt_loc:(unsigned int)0;
# 1128
void*_tmp2C9=Cyc_Port_type_to_ctype(env,_tmp2B0);
void*new_rgn;
# 1131
{void*_tmp2CA=_tmp2B2;struct _dyneither_ptr*_tmp2CD;_LL1AD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2CB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2CA;if(_tmp2CB->tag != 20)goto _LL1AF;}_LL1AE:
# 1133
 new_rgn=Cyc_Port_heap_ct();goto _LL1AC;_LL1AF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2CC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2CA;if(_tmp2CC->tag != 2)goto _LL1B1;else{_tmp2CD=(_tmp2CC->f1)->name;}}_LL1B0:
# 1135
 new_rgn=Cyc_Port_rgnvar_ct(_tmp2CD);goto _LL1AC;_LL1B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2CE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CA;if(_tmp2CE->tag != 1)goto _LL1B3;}_LL1B2:
# 1139
 if(Cyc_Port_in_fn_arg(env))
new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());else{
# 1142
if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))
new_rgn=Cyc_Port_heap_ct();else{
new_rgn=Cyc_Port_var();}}
goto _LL1AC;_LL1B3:;_LL1B4:
# 1147
 new_rgn=Cyc_Port_heap_ct();goto _LL1AC;_LL1AC:;}{
# 1149
int _tmp2CF=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2B4)== (void*)& Cyc_Absyn_DynEither_b_val;
int orig_zt;
{union Cyc_Absyn_Constraint _tmp2D0=*_tmp2B5;union Cyc_Absyn_Constraint _tmp2D1=_tmp2D0;_LL1B6: if((_tmp2D1.No_constr).tag != 3)goto _LL1B8;_LL1B7:
 orig_zt=Cyc_Port_is_char(env,_tmp2B0);goto _LL1B5;_LL1B8:;_LL1B9:
 orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2B5);goto _LL1B5;_LL1B5:;}
# 1155
if((env->gcenv)->porting){
void*_tmp2D2=Cyc_Port_var();
# 1160
Cyc_Port_register_const_cvar(env,_tmp2D2,_tmp2B1.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp2B1.loc);
# 1163
if(_tmp2B1.print_const)Cyc_Port_unify_ct(_tmp2D2,Cyc_Port_const_ct());{
# 1169
void*_tmp2D3=Cyc_Port_var();
# 1172
Cyc_Port_register_ptr_cvars(env,_tmp2D3,_tmp2CF?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),_tmp2C6);
# 1174
if(_tmp2CF)Cyc_Port_unify_ct(_tmp2D3,Cyc_Port_fat_ct());{
void*_tmp2D4=Cyc_Port_var();
# 1178
Cyc_Port_register_zeroterm_cvars(env,_tmp2D4,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp2C8);
# 1180
{union Cyc_Absyn_Constraint _tmp2D5=*_tmp2B5;union Cyc_Absyn_Constraint _tmp2D6=_tmp2D5;_LL1BB: if((_tmp2D6.No_constr).tag != 3)goto _LL1BD;_LL1BC:
# 1182
 goto _LL1BA;_LL1BD:;_LL1BE:
# 1184
 Cyc_Port_unify_ct(_tmp2D4,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2B5)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1BA;_LL1BA:;}
# 1190
return Cyc_Port_ptr_ct(_tmp2C9,_tmp2D2,_tmp2D3,new_rgn,_tmp2D4);};};}else{
# 1194
return Cyc_Port_ptr_ct(_tmp2C9,_tmp2B1.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
_tmp2CF?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),new_rgn,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}};}_LL19C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2AB;if(_tmp2B7->tag != 6)goto _LL19E;}_LL19D:
# 1198
 goto _LL19F;_LL19E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2B8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2AB;if(_tmp2B8->tag != 7)goto _LL1A0;}_LL19F:
 return Cyc_Port_arith_ct();_LL1A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2B9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB;if(_tmp2B9->tag != 8)goto _LL1A2;else{_tmp2BA=(_tmp2B9->f1).elt_type;_tmp2BB=(_tmp2B9->f1).tq;_tmp2BC=(_tmp2B9->f1).zero_term;_tmp2BD=(_tmp2B9->f1).zt_loc;}}_LL1A1: {
# 1202
void*_tmp2D7=Cyc_Port_type_to_ctype(env,_tmp2BA);
int orig_zt;
{union Cyc_Absyn_Constraint _tmp2D8=*_tmp2BC;union Cyc_Absyn_Constraint _tmp2D9=_tmp2D8;_LL1C0: if((_tmp2D9.No_constr).tag != 3)goto _LL1C2;_LL1C1:
 orig_zt=0;goto _LL1BF;_LL1C2:;_LL1C3:
 orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2BC);goto _LL1BF;_LL1BF:;}
# 1208
if((env->gcenv)->porting){
void*_tmp2DA=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,_tmp2DA,_tmp2BB.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp2BB.loc);{
# 1217
void*_tmp2DB=Cyc_Port_var();
Cyc_Port_register_zeroterm_cvars(env,_tmp2DB,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp2BD);
# 1220
{union Cyc_Absyn_Constraint _tmp2DC=*_tmp2BC;union Cyc_Absyn_Constraint _tmp2DD=_tmp2DC;_LL1C5: if((_tmp2DD.No_constr).tag != 3)goto _LL1C7;_LL1C6:
# 1222
 goto _LL1C4;_LL1C7:;_LL1C8:
# 1224
 Cyc_Port_unify_ct(_tmp2DB,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2BC)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1C4;_LL1C4:;}
# 1227
return Cyc_Port_array_ct(_tmp2D7,_tmp2DA,_tmp2DB);};}else{
# 1229
return Cyc_Port_array_ct(_tmp2D7,_tmp2BB.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}_LL1A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2BE=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2AB;if(_tmp2BE->tag != 9)goto _LL1A4;else{_tmp2BF=(_tmp2BE->f1).ret_typ;_tmp2C0=(_tmp2BE->f1).args;}}_LL1A3: {
# 1233
void*_tmp2DE=Cyc_Port_type_to_ctype(Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos),_tmp2BF);
struct Cyc_Port_Cenv*_tmp2DF=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;
for(0;_tmp2C0 != 0;_tmp2C0=_tmp2C0->tl){
struct _tuple8 _tmp2E0=*((struct _tuple8*)_tmp2C0->hd);struct Cyc_Absyn_Tqual _tmp2E2;void*_tmp2E3;struct _tuple8 _tmp2E1=_tmp2E0;_tmp2E2=_tmp2E1.f2;_tmp2E3=_tmp2E1.f3;{
struct Cyc_List_List*_tmp597;cargs=((_tmp597=_cycalloc(sizeof(*_tmp597)),((_tmp597->hd=Cyc_Port_type_to_ctype(_tmp2DF,_tmp2E3),((_tmp597->tl=cargs,_tmp597))))));};}
# 1240
return Cyc_Port_fn_ct(_tmp2DE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));}_LL1A4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2C1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2AB;if(_tmp2C1->tag != 11)goto _LL1A6;else{_tmp2C2=(_tmp2C1->f1).aggr_info;}}_LL1A5: {
# 1242
union Cyc_Absyn_AggrInfoU _tmp2E5=_tmp2C2;struct _tuple0*_tmp2E6;struct _tuple0*_tmp2E7;struct Cyc_Absyn_Aggrdecl**_tmp2E8;_LL1CA: if((_tmp2E5.UnknownAggr).tag != 1)goto _LL1CC;if(((struct _tuple2)(_tmp2E5.UnknownAggr).val).f1 != Cyc_Absyn_StructA)goto _LL1CC;_tmp2E6=((struct _tuple2)(_tmp2E5.UnknownAggr).val).f2;_LL1CB:
# 1244
(*_tmp2E6).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp2E9=Cyc_Port_lookup_struct_decl(env,_tmp2E6);
return Cyc_Port_known_aggr_ct(_tmp2E9);};_LL1CC: if((_tmp2E5.UnknownAggr).tag != 1)goto _LL1CE;if(((struct _tuple2)(_tmp2E5.UnknownAggr).val).f1 != Cyc_Absyn_UnionA)goto _LL1CE;_tmp2E7=((struct _tuple2)(_tmp2E5.UnknownAggr).val).f2;_LL1CD:
# 1248
(*_tmp2E7).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp2EA=Cyc_Port_lookup_union_decl(env,_tmp2E7);
return Cyc_Port_known_aggr_ct(_tmp2EA);};_LL1CE: if((_tmp2E5.KnownAggr).tag != 2)goto _LL1C9;_tmp2E8=(struct Cyc_Absyn_Aggrdecl**)(_tmp2E5.KnownAggr).val;_LL1CF: {
# 1252
struct Cyc_Absyn_Aggrdecl*_tmp2EB=*_tmp2E8;
enum Cyc_Absyn_AggrKind _tmp2EC=_tmp2EB->kind;switch(_tmp2EC){case Cyc_Absyn_StructA: _LL1D0: {
# 1255
struct _tuple11*_tmp2ED=Cyc_Port_lookup_struct_decl(env,_tmp2EB->name);
return Cyc_Port_known_aggr_ct(_tmp2ED);}case Cyc_Absyn_UnionA: _LL1D1: {
# 1258
struct _tuple11*_tmp2EE=Cyc_Port_lookup_union_decl(env,_tmp2EB->name);
return Cyc_Port_known_aggr_ct(_tmp2EE);}}}_LL1C9:;}_LL1A6: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2C3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2AB;if(_tmp2C3->tag != 13)goto _LL1A8;}_LL1A7:
# 1262
 return Cyc_Port_arith_ct();_LL1A8: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2C4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2AB;if(_tmp2C4->tag != 14)goto _LL1AA;else{_tmp2C5=_tmp2C4->f1;}}_LL1A9:
# 1265
 for(0;(unsigned int)_tmp2C5;_tmp2C5=_tmp2C5->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp2C5->hd)->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp2C5->hd)->name,Cyc_Absyn_sint_typ,Cyc_Port_const_ct(),Cyc_Port_arith_ct());}
# 1269
return Cyc_Port_arith_ct();_LL1AA:;_LL1AB:
 return Cyc_Port_arith_ct();_LL195:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1275
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*_tmp2EF=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);
struct Cyc_List_List*_tmp2F0=args->tl;
enum Cyc_Absyn_Primop _tmp2F1=p;enum Cyc_Absyn_Primop _tmp2F2;_LL1D4: if(_tmp2F1 != Cyc_Absyn_Plus)goto _LL1D6;_LL1D5: {
# 1283
void*_tmp2F3=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp2F0))->hd);
if(Cyc_Port_leq(_tmp2EF,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){
Cyc_Port_leq(_tmp2F3,Cyc_Port_arith_ct());
return _tmp2EF;}else{
if(Cyc_Port_leq(_tmp2F3,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){
Cyc_Port_leq(_tmp2EF,Cyc_Port_arith_ct());
return _tmp2F3;}else{
# 1291
Cyc_Port_leq(_tmp2EF,Cyc_Port_arith_ct());
Cyc_Port_leq(_tmp2F3,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}}}_LL1D6: if(_tmp2F1 != Cyc_Absyn_Minus)goto _LL1D8;_LL1D7:
# 1300
 if(_tmp2F0 == 0){
# 1302
Cyc_Port_leq(_tmp2EF,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}else{
# 1305
void*_tmp2F4=Cyc_Port_compress_ct((void*)_tmp2F0->hd);
if(Cyc_Port_leq(_tmp2EF,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()))){
if(Cyc_Port_leq(_tmp2F4,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var())))
return Cyc_Port_arith_ct();else{
# 1310
Cyc_Port_leq(_tmp2F4,Cyc_Port_arith_ct());
return _tmp2EF;}}else{
# 1314
Cyc_Port_leq(_tmp2EF,Cyc_Port_arith_ct());
Cyc_Port_leq(_tmp2EF,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}}_LL1D8: if(_tmp2F1 != Cyc_Absyn_Times)goto _LL1DA;_LL1D9:
# 1319
 goto _LL1DB;_LL1DA: if(_tmp2F1 != Cyc_Absyn_Div)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if(_tmp2F1 != Cyc_Absyn_Mod)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp2F1 != Cyc_Absyn_Eq)goto _LL1E0;_LL1DF: goto _LL1E1;_LL1E0: if(_tmp2F1 != Cyc_Absyn_Neq)goto _LL1E2;_LL1E1: goto _LL1E3;_LL1E2: if(_tmp2F1 != Cyc_Absyn_Lt)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if(_tmp2F1 != Cyc_Absyn_Gt)goto _LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if(_tmp2F1 != Cyc_Absyn_Lte)goto _LL1E8;_LL1E7:
 goto _LL1E9;_LL1E8: if(_tmp2F1 != Cyc_Absyn_Gte)goto _LL1EA;_LL1E9: goto _LL1EB;_LL1EA: if(_tmp2F1 != Cyc_Absyn_Bitand)goto _LL1EC;_LL1EB: goto _LL1ED;_LL1EC: if(_tmp2F1 != Cyc_Absyn_Bitor)goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE: if(_tmp2F1 != Cyc_Absyn_Bitxor)goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0: if(_tmp2F1 != Cyc_Absyn_Bitlshift)goto _LL1F2;_LL1F1: goto _LL1F3;_LL1F2: if(_tmp2F1 != Cyc_Absyn_Bitlrshift)goto _LL1F4;_LL1F3:
 goto _LL1F5;_LL1F4: if(_tmp2F1 != Cyc_Absyn_Bitarshift)goto _LL1F6;_LL1F5:
 Cyc_Port_leq(_tmp2EF,Cyc_Port_arith_ct());
Cyc_Port_leq((void*)((struct Cyc_List_List*)_check_null(_tmp2F0))->hd,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL1F6: if(_tmp2F1 != Cyc_Absyn_Not)goto _LL1F8;_LL1F7:
 goto _LL1F9;_LL1F8: if(_tmp2F1 != Cyc_Absyn_Bitnot)goto _LL1FA;_LL1F9:
# 1327
 Cyc_Port_leq(_tmp2EF,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL1FA: _tmp2F2=_tmp2F1;_LL1FB: {
const char*_tmp59A;void*_tmp599;(_tmp599=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp59A=".size primop",_tag_dyneither(_tmp59A,sizeof(char),13))),_tag_dyneither(_tmp599,sizeof(void*),0)));}_LL1D3:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1334
static struct _tuple10 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp2F7=e->r;void*_tmp2F8=_tmp2F7;struct _tuple0*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Exp*_tmp301;struct _dyneither_ptr*_tmp302;struct Cyc_Absyn_Exp*_tmp304;struct _dyneither_ptr*_tmp305;_LL1FD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2F9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2F8;if(_tmp2F9->tag != 1)goto _LL1FF;else{_tmp2FA=_tmp2F9->f1;}}_LL1FE:
# 1337
(*_tmp2FA).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_var(env,_tmp2FA);_LL1FF: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2FB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F8;if(_tmp2FB->tag != 22)goto _LL201;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FB->f2;}}_LL200: {
# 1340
void*_tmp306=Cyc_Port_var();
void*_tmp307=Cyc_Port_var();
void*_tmp308=Cyc_Port_gen_exp(env,_tmp2FC);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2FD),Cyc_Port_arith_ct());{
void*_tmp309=Cyc_Port_ptr_ct(_tmp306,_tmp307,Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var());
Cyc_Port_leq(_tmp308,_tmp309);{
struct _tuple10 _tmp59B;return(_tmp59B.f1=_tmp307,((_tmp59B.f2=_tmp306,_tmp59B)));};};}_LL201: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2FE=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2F8;if(_tmp2FE->tag != 19)goto _LL203;else{_tmp2FF=_tmp2FE->f1;}}_LL202: {
# 1348
void*_tmp30B=Cyc_Port_var();
void*_tmp30C=Cyc_Port_var();
void*_tmp30D=Cyc_Port_ptr_ct(_tmp30B,_tmp30C,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2FF),_tmp30D);{
struct _tuple10 _tmp59C;return(_tmp59C.f1=_tmp30C,((_tmp59C.f2=_tmp30B,_tmp59C)));};}_LL203: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp300=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8;if(_tmp300->tag != 20)goto _LL205;else{_tmp301=_tmp300->f1;_tmp302=_tmp300->f2;}}_LL204: {
# 1354
void*_tmp30F=Cyc_Port_var();
void*_tmp310=Cyc_Port_var();
void*_tmp311=Cyc_Port_gen_exp(env,_tmp301);
{struct Cyc_Port_Cfield*_tmp59F;struct Cyc_Port_Cfield*_tmp59E[1];Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp301),Cyc_Port_unknown_aggr_ct(((_tmp59E[0]=((_tmp59F=_cycalloc(sizeof(*_tmp59F)),((_tmp59F->qual=_tmp310,((_tmp59F->f=_tmp302,((_tmp59F->type=_tmp30F,_tmp59F)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59E,sizeof(struct Cyc_Port_Cfield*),1))))));}{
struct _tuple10 _tmp5A0;return(_tmp5A0.f1=_tmp310,((_tmp5A0.f2=_tmp30F,_tmp5A0)));};}_LL205: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp303=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8;if(_tmp303->tag != 21)goto _LL207;else{_tmp304=_tmp303->f1;_tmp305=_tmp303->f2;}}_LL206: {
# 1360
void*_tmp315=Cyc_Port_var();
void*_tmp316=Cyc_Port_var();
void*_tmp317=Cyc_Port_gen_exp(env,_tmp304);
{struct Cyc_Port_Cfield*_tmp5A3;struct Cyc_Port_Cfield*_tmp5A2[1];Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp304),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp5A2[0]=((_tmp5A3=_cycalloc(sizeof(*_tmp5A3)),((_tmp5A3->qual=_tmp316,((_tmp5A3->f=_tmp305,((_tmp5A3->type=_tmp315,_tmp5A3)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5A2,sizeof(struct Cyc_Port_Cfield*),1))))),
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}{
struct _tuple10 _tmp5A4;return(_tmp5A4.f1=_tmp316,((_tmp5A4.f2=_tmp315,_tmp5A4)));};}_LL207:;_LL208: {
struct Cyc_String_pa_PrintArg_struct _tmp5AC;void*_tmp5AB[1];const char*_tmp5AA;void*_tmp5A9;(_tmp5A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp5AC.tag=0,((_tmp5AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp5AB[0]=& _tmp5AC,Cyc_aprintf(((_tmp5AA="gen_lhs: %s",_tag_dyneither(_tmp5AA,sizeof(char),12))),_tag_dyneither(_tmp5AB,sizeof(void*),1)))))))),_tag_dyneither(_tmp5A9,sizeof(void*),0)));}_LL1FC:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1371
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp31F=e->r;void*_tmp320=_tmp31F;struct _tuple0*_tmp331;enum Cyc_Absyn_Primop _tmp333;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Core_Opt*_tmp337;struct Cyc_Absyn_Exp*_tmp338;struct Cyc_Absyn_Exp*_tmp33A;enum Cyc_Absyn_Incrementor _tmp33B;struct Cyc_Absyn_Exp*_tmp33D;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp347;struct Cyc_Absyn_Exp*_tmp348;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_List_List*_tmp34B;struct Cyc_Absyn_Exp*_tmp34E;void*_tmp351;struct Cyc_Absyn_Exp*_tmp352;struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp35B;struct _dyneither_ptr*_tmp35C;struct Cyc_Absyn_Exp*_tmp35E;struct _dyneither_ptr*_tmp35F;void**_tmp361;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*_tmp365;struct Cyc_Absyn_Stmt*_tmp371;_LL20A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp321=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp321->tag != 0)goto _LL20C;else{if(((_tmp321->f1).Char_c).tag != 2)goto _LL20C;}}_LL20B:
 goto _LL20D;_LL20C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp322=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp322->tag != 0)goto _LL20E;else{if(((_tmp322->f1).Wchar_c).tag != 3)goto _LL20E;}}_LL20D:
 goto _LL20F;_LL20E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp323=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp323->tag != 0)goto _LL210;else{if(((_tmp323->f1).Short_c).tag != 4)goto _LL210;}}_LL20F:
 goto _LL211;_LL210: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp324=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp324->tag != 0)goto _LL212;else{if(((_tmp324->f1).LongLong_c).tag != 6)goto _LL212;}}_LL211:
 goto _LL213;_LL212: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp325=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp325->tag != 16)goto _LL214;}_LL213:
 goto _LL215;_LL214: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp326=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp326->tag != 17)goto _LL216;}_LL215:
 goto _LL217;_LL216: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp327=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp327->tag != 18)goto _LL218;}_LL217:
 goto _LL219;_LL218: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp328=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp328->tag != 31)goto _LL21A;}_LL219:
 goto _LL21B;_LL21A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp329=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp329->tag != 32)goto _LL21C;}_LL21B:
 goto _LL21D;_LL21C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp32A->tag != 0)goto _LL21E;else{if(((_tmp32A->f1).Float_c).tag != 7)goto _LL21E;}}_LL21D:
 return Cyc_Port_arith_ct();_LL21E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp32B->tag != 0)goto _LL220;else{if(((_tmp32B->f1).Int_c).tag != 5)goto _LL220;if(((struct _tuple5)((_tmp32B->f1).Int_c).val).f2 != 0)goto _LL220;}}_LL21F:
 return Cyc_Port_zero_ct();_LL220: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp32C->tag != 0)goto _LL222;else{if(((_tmp32C->f1).Int_c).tag != 5)goto _LL222;}}_LL221:
 return Cyc_Port_arith_ct();_LL222: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp32D->tag != 0)goto _LL224;else{if(((_tmp32D->f1).String_c).tag != 8)goto _LL224;}}_LL223:
# 1386
 return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());_LL224: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp32E->tag != 0)goto _LL226;else{if(((_tmp32E->f1).Wstring_c).tag != 9)goto _LL226;}}_LL225:
# 1388
 return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());_LL226: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp32F->tag != 0)goto _LL228;else{if(((_tmp32F->f1).Null_c).tag != 1)goto _LL228;}}_LL227:
 return Cyc_Port_zero_ct();_LL228: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp330=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp330->tag != 1)goto _LL22A;else{_tmp331=_tmp330->f1;}}_LL229:
# 1391
(*_tmp331).f1=Cyc_Absyn_Loc_n;{
struct _tuple10 _tmp375=Cyc_Port_lookup_var(env,_tmp331);void*_tmp377;struct _tuple10 _tmp376=_tmp375;_tmp377=_tmp376.f2;
if(Cyc_Port_isfn(env,_tmp331)){
_tmp377=Cyc_Port_instantiate(_tmp377);
return Cyc_Port_ptr_ct(_tmp377,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_heap_ct(),Cyc_Port_nozterm_ct());}else{
# 1397
if(Cyc_Port_isarray(env,_tmp331)){
void*_tmp378=Cyc_Port_var();
void*_tmp379=Cyc_Port_var();
void*_tmp37A=Cyc_Port_var();
void*_tmp37B=Cyc_Port_array_ct(_tmp378,_tmp379,_tmp37A);
Cyc_Port_unifies(_tmp377,_tmp37B);{
void*_tmp37C=Cyc_Port_ptr_ct(_tmp378,_tmp379,Cyc_Port_fat_ct(),Cyc_Port_var(),_tmp37A);
return _tmp37C;};}else{
return _tmp377;}}};_LL22A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp332=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp332->tag != 2)goto _LL22C;else{_tmp333=_tmp332->f1;_tmp334=_tmp332->f2;}}_LL22B:
 return Cyc_Port_gen_primop(env,_tmp333,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp334));_LL22C: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp335=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp335->tag != 3)goto _LL22E;else{_tmp336=_tmp335->f1;_tmp337=_tmp335->f2;_tmp338=_tmp335->f3;}}_LL22D: {
# 1408
struct _tuple10 _tmp37D=Cyc_Port_gen_lhs(env,_tmp336);void*_tmp37F;void*_tmp380;struct _tuple10 _tmp37E=_tmp37D;_tmp37F=_tmp37E.f1;_tmp380=_tmp37E.f2;
Cyc_Port_unifies(_tmp37F,Cyc_Port_notconst_ct());{
void*_tmp381=Cyc_Port_gen_exp(env,_tmp338);
if((unsigned int)_tmp337){
struct Cyc_List_List _tmp5AD;struct Cyc_List_List x2=(_tmp5AD.hd=_tmp381,((_tmp5AD.tl=0,_tmp5AD)));
struct Cyc_List_List _tmp5AE;struct Cyc_List_List x1=(_tmp5AE.hd=_tmp380,((_tmp5AE.tl=& x2,_tmp5AE)));
void*_tmp382=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)_tmp337->v,& x1);
Cyc_Port_leq(_tmp382,_tmp380);
return _tmp380;}else{
# 1418
Cyc_Port_leq(_tmp381,_tmp380);
return _tmp380;}};}_LL22E: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp339=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp339->tag != 4)goto _LL230;else{_tmp33A=_tmp339->f1;_tmp33B=_tmp339->f2;}}_LL22F: {
# 1422
struct _tuple10 _tmp385=Cyc_Port_gen_lhs(env,_tmp33A);void*_tmp387;void*_tmp388;struct _tuple10 _tmp386=_tmp385;_tmp387=_tmp386.f1;_tmp388=_tmp386.f2;
Cyc_Port_unifies(_tmp387,Cyc_Port_notconst_ct());
# 1425
Cyc_Port_leq(_tmp388,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));
Cyc_Port_leq(_tmp388,Cyc_Port_arith_ct());
return _tmp388;}_LL230: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp33C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp33C->tag != 5)goto _LL232;else{_tmp33D=_tmp33C->f1;_tmp33E=_tmp33C->f2;_tmp33F=_tmp33C->f3;}}_LL231: {
# 1429
void*_tmp389=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp33D),Cyc_Port_arith_ct());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp33E),_tmp389);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp33F),_tmp389);
return _tmp389;}_LL232: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp340=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp340->tag != 6)goto _LL234;else{_tmp341=_tmp340->f1;_tmp342=_tmp340->f2;}}_LL233:
 _tmp344=_tmp341;_tmp345=_tmp342;goto _LL235;_LL234: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp343=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp343->tag != 7)goto _LL236;else{_tmp344=_tmp343->f1;_tmp345=_tmp343->f2;}}_LL235:
# 1436
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp344),Cyc_Port_arith_ct());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp345),Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL236: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp346=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp346->tag != 8)goto _LL238;else{_tmp347=_tmp346->f1;_tmp348=_tmp346->f2;}}_LL237:
# 1440
 Cyc_Port_gen_exp(env,_tmp347);
return Cyc_Port_gen_exp(env,_tmp348);_LL238: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp349=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp349->tag != 9)goto _LL23A;else{_tmp34A=_tmp349->f1;_tmp34B=_tmp349->f2;}}_LL239: {
# 1443
void*_tmp38A=Cyc_Port_var();
void*_tmp38B=Cyc_Port_gen_exp(env,_tmp34A);
struct Cyc_List_List*_tmp38C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp34B);
struct Cyc_List_List*_tmp38D=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp38C);
Cyc_Port_unifies(_tmp38B,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(_tmp38A,_tmp38D),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_nozterm_ct()));
for(0;_tmp38C != 0;(_tmp38C=_tmp38C->tl,_tmp38D=_tmp38D->tl)){
Cyc_Port_leq((void*)_tmp38C->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp38D))->hd);}
# 1451
return _tmp38A;}_LL23A: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp34C=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp34C->tag != 10)goto _LL23C;}_LL23B: {
const char*_tmp5B1;void*_tmp5B0;(_tmp5B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5B1="throw",_tag_dyneither(_tmp5B1,sizeof(char),6))),_tag_dyneither(_tmp5B0,sizeof(void*),0)));}_LL23C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp34D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp34D->tag != 11)goto _LL23E;else{_tmp34E=_tmp34D->f1;}}_LL23D:
 return Cyc_Port_gen_exp(env,_tmp34E);_LL23E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp34F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp34F->tag != 12)goto _LL240;}_LL23F: {
const char*_tmp5B4;void*_tmp5B3;(_tmp5B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5B4="instantiate",_tag_dyneither(_tmp5B4,sizeof(char),12))),_tag_dyneither(_tmp5B3,sizeof(void*),0)));}_LL240: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp350=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp350->tag != 13)goto _LL242;else{_tmp351=(void*)_tmp350->f1;_tmp352=_tmp350->f2;}}_LL241: {
# 1456
void*_tmp392=Cyc_Port_type_to_ctype(env,_tmp351);
void*_tmp393=Cyc_Port_gen_exp(env,_tmp352);
Cyc_Port_leq(_tmp392,_tmp393);
return _tmp393;}_LL242: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp353=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp353->tag != 14)goto _LL244;else{_tmp354=_tmp353->f1;}}_LL243: {
# 1461
struct _tuple10 _tmp394=Cyc_Port_gen_lhs(env,_tmp354);void*_tmp396;void*_tmp397;struct _tuple10 _tmp395=_tmp394;_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;
return Cyc_Port_ptr_ct(_tmp397,_tmp396,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());}_LL244: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp355=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp355->tag != 22)goto _LL246;else{_tmp356=_tmp355->f1;_tmp357=_tmp355->f2;}}_LL245: {
# 1464
void*_tmp398=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp357),Cyc_Port_arith_ct());{
void*_tmp399=Cyc_Port_gen_exp(env,_tmp356);
Cyc_Port_leq(_tmp399,Cyc_Port_ptr_ct(_tmp398,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));
return _tmp398;};}_LL246: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp358=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp358->tag != 19)goto _LL248;else{_tmp359=_tmp358->f1;}}_LL247: {
# 1470
void*_tmp39A=Cyc_Port_var();
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp359),Cyc_Port_ptr_ct(_tmp39A,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));
return _tmp39A;}_LL248: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp35A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp35A->tag != 20)goto _LL24A;else{_tmp35B=_tmp35A->f1;_tmp35C=_tmp35A->f2;}}_LL249: {
# 1474
void*_tmp39B=Cyc_Port_var();
void*_tmp39C=Cyc_Port_gen_exp(env,_tmp35B);
{struct Cyc_Port_Cfield*_tmp5B7;struct Cyc_Port_Cfield*_tmp5B6[1];Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp35B),Cyc_Port_unknown_aggr_ct(((_tmp5B6[0]=((_tmp5B7=_cycalloc(sizeof(*_tmp5B7)),((_tmp5B7->qual=Cyc_Port_var(),((_tmp5B7->f=_tmp35C,((_tmp5B7->type=_tmp39B,_tmp5B7)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B6,sizeof(struct Cyc_Port_Cfield*),1))))));}
return _tmp39B;}_LL24A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp35D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp35D->tag != 21)goto _LL24C;else{_tmp35E=_tmp35D->f1;_tmp35F=_tmp35D->f2;}}_LL24B: {
# 1479
void*_tmp39F=Cyc_Port_var();
void*_tmp3A0=Cyc_Port_gen_exp(env,_tmp35E);
{struct Cyc_Port_Cfield*_tmp5BA;struct Cyc_Port_Cfield*_tmp5B9[1];Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp35E),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp5B9[0]=((_tmp5BA=_cycalloc(sizeof(*_tmp5BA)),((_tmp5BA->qual=Cyc_Port_var(),((_tmp5BA->f=_tmp35F,((_tmp5BA->type=_tmp39F,_tmp5BA)))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B9,sizeof(struct Cyc_Port_Cfield*),1))))),
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}
return _tmp39F;}_LL24C: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp360=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp360->tag != 33)goto _LL24E;else{_tmp361=(_tmp360->f1).elt_type;_tmp362=(_tmp360->f1).num_elts;}}_LL24D:
# 1485
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp362),Cyc_Port_arith_ct());{
void*_tmp3A3=(unsigned int)_tmp361?Cyc_Port_type_to_ctype(env,*_tmp361): Cyc_Port_var();
return Cyc_Port_ptr_ct(_tmp3A3,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_var());};_LL24E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp363->tag != 34)goto _LL250;else{_tmp364=_tmp363->f1;_tmp365=_tmp363->f2;}}_LL24F: {
const char*_tmp5BD;void*_tmp5BC;(_tmp5BC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5BD="Swap_e",_tag_dyneither(_tmp5BD,sizeof(char),7))),_tag_dyneither(_tmp5BC,sizeof(void*),0)));}_LL250: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp366=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp366->tag != 15)goto _LL252;}_LL251: {
const char*_tmp5C0;void*_tmp5BF;(_tmp5BF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5C0="New_e",_tag_dyneither(_tmp5C0,sizeof(char),6))),_tag_dyneither(_tmp5BF,sizeof(void*),0)));}_LL252: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp367=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp367->tag != 23)goto _LL254;}_LL253: {
const char*_tmp5C3;void*_tmp5C2;(_tmp5C2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5C3="Tuple_e",_tag_dyneither(_tmp5C3,sizeof(char),8))),_tag_dyneither(_tmp5C2,sizeof(void*),0)));}_LL254: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp368=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp368->tag != 24)goto _LL256;}_LL255: {
const char*_tmp5C6;void*_tmp5C5;(_tmp5C5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5C6="CompoundLit_e",_tag_dyneither(_tmp5C6,sizeof(char),14))),_tag_dyneither(_tmp5C5,sizeof(void*),0)));}_LL256: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp369=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp369->tag != 25)goto _LL258;}_LL257: {
const char*_tmp5C9;void*_tmp5C8;(_tmp5C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5C9="Array_e",_tag_dyneither(_tmp5C9,sizeof(char),8))),_tag_dyneither(_tmp5C8,sizeof(void*),0)));}_LL258: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp36A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp36A->tag != 26)goto _LL25A;}_LL259: {
const char*_tmp5CC;void*_tmp5CB;(_tmp5CB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5CC="Comprehension_e",_tag_dyneither(_tmp5CC,sizeof(char),16))),_tag_dyneither(_tmp5CB,sizeof(void*),0)));}_LL25A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp36B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp36B->tag != 27)goto _LL25C;}_LL25B: {
const char*_tmp5CF;void*_tmp5CE;(_tmp5CE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5CF="ComprehensionNoinit_e",_tag_dyneither(_tmp5CF,sizeof(char),22))),_tag_dyneither(_tmp5CE,sizeof(void*),0)));}_LL25C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp36C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp36C->tag != 28)goto _LL25E;}_LL25D: {
const char*_tmp5D2;void*_tmp5D1;(_tmp5D1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5D2="Aggregate_e",_tag_dyneither(_tmp5D2,sizeof(char),12))),_tag_dyneither(_tmp5D1,sizeof(void*),0)));}_LL25E: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp36D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp36D->tag != 29)goto _LL260;}_LL25F: {
const char*_tmp5D5;void*_tmp5D4;(_tmp5D4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5D5="AnonStruct_e",_tag_dyneither(_tmp5D5,sizeof(char),13))),_tag_dyneither(_tmp5D4,sizeof(void*),0)));}_LL260: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp36E=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp36E->tag != 30)goto _LL262;}_LL261: {
const char*_tmp5D8;void*_tmp5D7;(_tmp5D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5D8="Datatype_e",_tag_dyneither(_tmp5D8,sizeof(char),11))),_tag_dyneither(_tmp5D7,sizeof(void*),0)));}_LL262: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp36F=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp36F->tag != 35)goto _LL264;}_LL263: {
const char*_tmp5DB;void*_tmp5DA;(_tmp5DA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5DB="UnresolvedMem_e",_tag_dyneither(_tmp5DB,sizeof(char),16))),_tag_dyneither(_tmp5DA,sizeof(void*),0)));}_LL264: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp370=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp370->tag != 36)goto _LL266;else{_tmp371=_tmp370->f1;}}_LL265: {
const char*_tmp5DE;void*_tmp5DD;(_tmp5DD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5DE="StmtExp_e",_tag_dyneither(_tmp5DE,sizeof(char),10))),_tag_dyneither(_tmp5DD,sizeof(void*),0)));}_LL266: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp372=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp372->tag != 38)goto _LL268;}_LL267: {
const char*_tmp5E1;void*_tmp5E0;(_tmp5E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5E1="Valueof_e",_tag_dyneither(_tmp5E1,sizeof(char),10))),_tag_dyneither(_tmp5E0,sizeof(void*),0)));}_LL268: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp373->tag != 39)goto _LL26A;}_LL269: {
const char*_tmp5E4;void*_tmp5E3;(_tmp5E3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5E4="Asm_e",_tag_dyneither(_tmp5E4,sizeof(char),6))),_tag_dyneither(_tmp5E3,sizeof(void*),0)));}_LL26A: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp374=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp320;if(_tmp374->tag != 37)goto _LL209;}_LL26B: {
const char*_tmp5E7;void*_tmp5E6;(_tmp5E6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5E7="Tagcheck_e",_tag_dyneither(_tmp5E7,sizeof(char),11))),_tag_dyneither(_tmp5E6,sizeof(void*),0)));}_LL209:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1507
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp3C2=s->r;void*_tmp3C3=_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Stmt*_tmp3CE;struct Cyc_Absyn_Stmt*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_List_List*_tmp3DD;struct Cyc_Absyn_Decl*_tmp3E0;struct Cyc_Absyn_Stmt*_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E3;struct Cyc_Absyn_Stmt*_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E6;_LL26D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3C4=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3C4->tag != 0)goto _LL26F;}_LL26E:
 return;_LL26F: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3C5=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3C5->tag != 1)goto _LL271;else{_tmp3C6=_tmp3C5->f1;}}_LL270:
 Cyc_Port_gen_exp(env,_tmp3C6);return;_LL271: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3C7=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3C7->tag != 2)goto _LL273;else{_tmp3C8=_tmp3C7->f1;_tmp3C9=_tmp3C7->f2;}}_LL272:
 Cyc_Port_gen_stmt(env,_tmp3C8);Cyc_Port_gen_stmt(env,_tmp3C9);return;_LL273: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3CA=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3CA->tag != 3)goto _LL275;else{_tmp3CB=_tmp3CA->f1;}}_LL274: {
# 1513
void*_tmp3E9=Cyc_Port_lookup_return_type(env);
void*_tmp3EA=(unsigned int)_tmp3CB?Cyc_Port_gen_exp(env,_tmp3CB): Cyc_Port_void_ct();
Cyc_Port_leq(_tmp3EA,_tmp3E9);
return;}_LL275: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3CC=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3CC->tag != 4)goto _LL277;else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;_tmp3CF=_tmp3CC->f3;}}_LL276:
# 1518
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3CD),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp3CE);
Cyc_Port_gen_stmt(env,_tmp3CF);
return;_LL277: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3D0=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3D0->tag != 5)goto _LL279;else{_tmp3D1=(_tmp3D0->f1).f1;_tmp3D2=_tmp3D0->f2;}}_LL278:
# 1523
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3D1),Cyc_Port_arith_ct());
Cyc_Port_gen_stmt(env,_tmp3D2);
return;_LL279: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3D3=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3D3->tag != 6)goto _LL27B;}_LL27A:
 goto _LL27C;_LL27B: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3D4=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3D4->tag != 7)goto _LL27D;}_LL27C:
 goto _LL27E;_LL27D: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3D5=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3D5->tag != 8)goto _LL27F;}_LL27E:
 return;_LL27F: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3D6=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3D6->tag != 9)goto _LL281;else{_tmp3D7=_tmp3D6->f1;_tmp3D8=(_tmp3D6->f2).f1;_tmp3D9=(_tmp3D6->f3).f1;_tmp3DA=_tmp3D6->f4;}}_LL280:
# 1530
 Cyc_Port_gen_exp(env,_tmp3D7);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3D8),Cyc_Port_arith_ct());
Cyc_Port_gen_exp(env,_tmp3D9);
Cyc_Port_gen_stmt(env,_tmp3DA);
return;_LL281: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3DB=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3DB->tag != 10)goto _LL283;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DB->f2;}}_LL282:
# 1536
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3DC),Cyc_Port_arith_ct());
for(0;(unsigned int)_tmp3DD;_tmp3DD=_tmp3DD->tl){
# 1539
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp3DD->hd)->body);}
# 1541
return;_LL283: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3DE=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3DE->tag != 11)goto _LL285;}_LL284: {
const char*_tmp5EA;void*_tmp5E9;(_tmp5E9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5EA="fallthru",_tag_dyneither(_tmp5EA,sizeof(char),9))),_tag_dyneither(_tmp5E9,sizeof(void*),0)));}_LL285: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3DF=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3DF->tag != 12)goto _LL287;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;}}_LL286:
 env=Cyc_Port_gen_localdecl(env,_tmp3E0);Cyc_Port_gen_stmt(env,_tmp3E1);return;_LL287: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3E2=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3E2->tag != 13)goto _LL289;else{_tmp3E3=_tmp3E2->f2;}}_LL288:
 Cyc_Port_gen_stmt(env,_tmp3E3);return;_LL289: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3E4=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3E4->tag != 14)goto _LL28B;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=(_tmp3E4->f2).f1;}}_LL28A:
# 1546
 Cyc_Port_gen_stmt(env,_tmp3E5);
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3E6),Cyc_Port_arith_ct());
return;_LL28B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3E7=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3E7->tag != 15)goto _LL28D;}_LL28C: {
const char*_tmp5ED;void*_tmp5EC;(_tmp5EC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5ED="try/catch",_tag_dyneither(_tmp5ED,sizeof(char),10))),_tag_dyneither(_tmp5EC,sizeof(void*),0)));}_LL28D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3E8=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3C3;if(_tmp3E8->tag != 16)goto _LL26C;}_LL28E: {
const char*_tmp5F0;void*_tmp5EF;(_tmp5EF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5F0="reset region",_tag_dyneither(_tmp5F0,sizeof(char),13))),_tag_dyneither(_tmp5EF,sizeof(void*),0)));}_LL26C:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1555
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp3F1=e->r;void*_tmp3F2=_tmp3F1;struct Cyc_List_List*_tmp3F4;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*_tmp3F8;_LL290: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp3F3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp3F2;if(_tmp3F3->tag != 35)goto _LL292;else{_tmp3F4=_tmp3F3->f2;}}_LL291:
 _tmp3F6=_tmp3F4;goto _LL293;_LL292: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3F5=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3F2;if(_tmp3F5->tag != 25)goto _LL294;else{_tmp3F6=_tmp3F5->f1;}}_LL293:
 _tmp3F8=_tmp3F6;goto _LL295;_LL294: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp3F7=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp3F2;if(_tmp3F7->tag != 24)goto _LL296;else{_tmp3F8=_tmp3F7->f2;}}_LL295:
# 1560
 LOOP: {
void*_tmp3FB=t;struct _tuple0*_tmp3FD;void*_tmp3FF;union Cyc_Absyn_Constraint*_tmp400;unsigned int _tmp401;struct _tuple0*_tmp403;struct Cyc_Absyn_Aggrdecl*_tmp405;_LL29D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3FB;if(_tmp3FC->tag != 17)goto _LL29F;else{_tmp3FD=_tmp3FC->f1;}}_LL29E:
# 1563
(*_tmp3FD).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,_tmp3FD);goto LOOP;_LL29F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3FB;if(_tmp3FE->tag != 8)goto _LL2A1;else{_tmp3FF=(_tmp3FE->f1).elt_type;_tmp400=(_tmp3FE->f1).zero_term;_tmp401=(_tmp3FE->f1).zt_loc;}}_LL2A0: {
# 1566
void*_tmp406=Cyc_Port_var();
void*_tmp407=Cyc_Port_var();
void*_tmp408=Cyc_Port_var();
void*_tmp409=Cyc_Port_type_to_ctype(env,_tmp3FF);
for(0;_tmp3F8 != 0;_tmp3F8=_tmp3F8->tl){
struct _tuple15 _tmp40A=*((struct _tuple15*)_tmp3F8->hd);struct Cyc_List_List*_tmp40C;struct Cyc_Absyn_Exp*_tmp40D;struct _tuple15 _tmp40B=_tmp40A;_tmp40C=_tmp40B.f1;_tmp40D=_tmp40B.f2;
if((unsigned int)_tmp40C){const char*_tmp5F3;void*_tmp5F2;(_tmp5F2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5F3="designators in initializers",_tag_dyneither(_tmp5F3,sizeof(char),28))),_tag_dyneither(_tmp5F2,sizeof(void*),0)));}{
void*_tmp410=Cyc_Port_gen_initializer(env,_tmp3FF,_tmp40D);
Cyc_Port_leq(_tmp410,_tmp406);};}
# 1576
return Cyc_Port_array_ct(_tmp406,_tmp407,_tmp408);}_LL2A1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3FB;if(_tmp402->tag != 11)goto _LL2A3;else{if((((_tmp402->f1).aggr_info).UnknownAggr).tag != 1)goto _LL2A3;if(((struct _tuple2)(((_tmp402->f1).aggr_info).UnknownAggr).val).f1 != Cyc_Absyn_StructA)goto _LL2A3;_tmp403=((struct _tuple2)(((_tmp402->f1).aggr_info).UnknownAggr).val).f2;}}_LL2A2:
# 1578
(*_tmp403).f1=Cyc_Absyn_Loc_n;{
struct _tuple11 _tmp411=*Cyc_Port_lookup_struct_decl(env,_tmp403);struct Cyc_Absyn_Aggrdecl*_tmp413;struct _tuple11 _tmp412=_tmp411;_tmp413=_tmp412.f1;
if(_tmp413 == 0){const char*_tmp5F6;void*_tmp5F5;(_tmp5F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5F6="struct is not yet defined",_tag_dyneither(_tmp5F6,sizeof(char),26))),_tag_dyneither(_tmp5F5,sizeof(void*),0)));}
_tmp405=_tmp413;goto _LL2A4;};_LL2A3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3FB;if(_tmp404->tag != 11)goto _LL2A5;else{if((((_tmp404->f1).aggr_info).KnownAggr).tag != 2)goto _LL2A5;_tmp405=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp404->f1).aggr_info).KnownAggr).val);}}_LL2A4: {
# 1583
struct Cyc_List_List*_tmp416=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp405->impl))->fields;
for(0;_tmp3F8 != 0;_tmp3F8=_tmp3F8->tl){
struct _tuple15 _tmp417=*((struct _tuple15*)_tmp3F8->hd);struct Cyc_List_List*_tmp419;struct Cyc_Absyn_Exp*_tmp41A;struct _tuple15 _tmp418=_tmp417;_tmp419=_tmp418.f1;_tmp41A=_tmp418.f2;
if((unsigned int)_tmp419){const char*_tmp5F9;void*_tmp5F8;(_tmp5F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5F9="designators in initializers",_tag_dyneither(_tmp5F9,sizeof(char),28))),_tag_dyneither(_tmp5F8,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrfield*_tmp41D=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp416))->hd;
_tmp416=_tmp416->tl;{
void*_tmp41E=Cyc_Port_gen_initializer(env,_tmp41D->type,_tmp41A);;};};}
# 1591
return Cyc_Port_type_to_ctype(env,t);}_LL2A5:;_LL2A6: {
const char*_tmp5FC;void*_tmp5FB;(_tmp5FB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5FC="bad type for aggregate initializer",_tag_dyneither(_tmp5FC,sizeof(char),35))),_tag_dyneither(_tmp5FB,sizeof(void*),0)));}_LL29C:;}_LL296: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3F9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3F2;if(_tmp3F9->tag != 0)goto _LL298;else{if(((_tmp3F9->f1).String_c).tag != 8)goto _LL298;}}_LL297:
# 1594
 goto _LL299;_LL298: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3FA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3F2;if(_tmp3FA->tag != 0)goto _LL29A;else{if(((_tmp3FA->f1).Wstring_c).tag != 9)goto _LL29A;}}_LL299:
# 1596
 LOOP2: {
void*_tmp421=t;struct _tuple0*_tmp423;_LL2A8: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp421;if(_tmp422->tag != 17)goto _LL2AA;else{_tmp423=_tmp422->f1;}}_LL2A9:
# 1599
(*_tmp423).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,_tmp423);goto LOOP2;_LL2AA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp421;if(_tmp424->tag != 8)goto _LL2AC;}_LL2AB:
 return Cyc_Port_array_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_zterm_ct());_LL2AC:;_LL2AD:
 return Cyc_Port_gen_exp(env,e);_LL2A7:;}_LL29A:;_LL29B:
# 1604
 return Cyc_Port_gen_exp(env,e);_LL28F:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1609
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp425=d->r;void*_tmp426=_tmp425;struct Cyc_Absyn_Vardecl*_tmp428;_LL2AF: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp427=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp426;if(_tmp427->tag != 0)goto _LL2B1;else{_tmp428=_tmp427->f1;}}_LL2B0: {
# 1612
void*_tmp429=Cyc_Port_var();
void*q;
if((env->gcenv)->porting){
q=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,q,
(_tmp428->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp428->tq).loc);}else{
# 1625
q=(_tmp428->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1627
(*_tmp428->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,_tmp428->name,_tmp428->type,q,_tmp429);
Cyc_Port_unifies(_tmp429,Cyc_Port_type_to_ctype(env,_tmp428->type));
if((unsigned int)_tmp428->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp428->initializer);
void*t2=Cyc_Port_gen_initializer(env,_tmp428->type,e);
Cyc_Port_leq(t2,_tmp429);}
# 1635
return env;}_LL2B1:;_LL2B2: {
const char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5FF="non-local decl that isn't a variable",_tag_dyneither(_tmp5FF,sizeof(char),37))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));}_LL2AE:;}
# 1640
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp42C=*arg;struct _dyneither_ptr*_tmp42E;struct Cyc_Absyn_Tqual _tmp42F;void*_tmp430;struct _tuple8 _tmp42D=_tmp42C;_tmp42E=_tmp42D.f1;_tmp42F=_tmp42D.f2;_tmp430=_tmp42D.f3;{
struct _tuple8*_tmp600;return(_tmp600=_cycalloc(sizeof(*_tmp600)),((_tmp600->f1=0,((_tmp600->f2=_tmp42F,((_tmp600->f3=_tmp430,_tmp600)))))));};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1645
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple16{struct _dyneither_ptr*f1;void*f2;void*f3;void*f4;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1647
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp432=d->r;void*_tmp433=_tmp432;struct Cyc_Absyn_Vardecl*_tmp437;struct Cyc_Absyn_Fndecl*_tmp439;struct Cyc_Absyn_Typedefdecl*_tmp43B;struct Cyc_Absyn_Aggrdecl*_tmp43D;struct Cyc_Absyn_Enumdecl*_tmp43F;_LL2B4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp434=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp433;if(_tmp434->tag != 13)goto _LL2B6;}_LL2B5:
# 1650
(env->gcenv)->porting=1;
return env;_LL2B6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp435=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp433;if(_tmp435->tag != 14)goto _LL2B8;}_LL2B7:
# 1653
(env->gcenv)->porting=0;
return env;_LL2B8: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp436=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp433;if(_tmp436->tag != 0)goto _LL2BA;else{_tmp437=_tmp436->f1;}}_LL2B9:
# 1656
(*_tmp437->name).f1=Cyc_Absyn_Loc_n;
# 1660
if(Cyc_Port_declared_var(env,_tmp437->name)){
struct _tuple10 _tmp440=Cyc_Port_lookup_var(env,_tmp437->name);void*_tmp442;void*_tmp443;struct _tuple10 _tmp441=_tmp440;_tmp442=_tmp441.f1;_tmp443=_tmp441.f2;{
void*q2;
if((env->gcenv)->porting  && !Cyc_Port_isfn(env,_tmp437->name)){
q2=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,q2,
(_tmp437->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp437->tq).loc);}else{
# 1674
q2=(_tmp437->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1676
Cyc_Port_unifies(_tmp442,q2);
Cyc_Port_unifies(_tmp443,Cyc_Port_type_to_ctype(env,_tmp437->type));
if((unsigned int)_tmp437->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp437->initializer);
Cyc_Port_leq(Cyc_Port_gen_initializer(env,_tmp437->type,e),_tmp443);}
# 1682
return env;};}else{
# 1684
return Cyc_Port_gen_localdecl(env,d);}_LL2BA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp438=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp433;if(_tmp438->tag != 1)goto _LL2BC;else{_tmp439=_tmp438->f1;}}_LL2BB:
# 1690
(*_tmp439->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple13*predeclared=0;
# 1693
if(Cyc_Port_declared_var(env,_tmp439->name))
predeclared=Cyc_Port_lookup_full_var(env,_tmp439->name);{
# 1696
void*_tmp444=_tmp439->ret_type;
struct Cyc_List_List*_tmp445=_tmp439->args;
struct Cyc_List_List*_tmp446=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp445);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp606;struct Cyc_Absyn_FnInfo _tmp605;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp604;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp447=
(_tmp604=_cycalloc(sizeof(*_tmp604)),((_tmp604[0]=((_tmp606.tag=9,((_tmp606.f1=((_tmp605.tvars=0,((_tmp605.effect=0,((_tmp605.ret_tqual=Cyc_Absyn_empty_tqual(0),((_tmp605.ret_typ=_tmp444,((_tmp605.args=_tmp446,((_tmp605.c_varargs=0,((_tmp605.cyc_varargs=0,((_tmp605.rgn_po=0,((_tmp605.attributes=0,((_tmp605.requires_clause=0,((_tmp605.requires_relns=0,((_tmp605.ensures_clause=0,((_tmp605.ensures_relns=0,_tmp605)))))))))))))))))))))))))),_tmp606)))),_tmp604)));
# 1703
struct Cyc_Port_Cenv*_tmp448=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp449=Cyc_Port_type_to_ctype(_tmp448,_tmp444);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp44A=_tmp445;for(0;(unsigned int)_tmp44A;_tmp44A=_tmp44A->tl){
struct _tuple8 _tmp44B=*((struct _tuple8*)_tmp44A->hd);struct _dyneither_ptr*_tmp44D;struct Cyc_Absyn_Tqual _tmp44E;void*_tmp44F;struct _tuple8 _tmp44C=_tmp44B;_tmp44D=_tmp44C.f1;_tmp44E=_tmp44C.f2;_tmp44F=_tmp44C.f3;{
# 1711
void*_tmp450=Cyc_Port_type_to_ctype(_tmp448,_tmp44F);
void*tqv;
if((env->gcenv)->porting){
tqv=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,tqv,_tmp44E.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp44E.loc);}else{
# 1723
tqv=_tmp44E.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1725
{struct _tuple16*_tmp609;struct Cyc_List_List*_tmp608;c_args=((_tmp608=_cycalloc(sizeof(*_tmp608)),((_tmp608->hd=((_tmp609=_cycalloc(sizeof(*_tmp609)),((_tmp609->f1=_tmp44D,((_tmp609->f2=_tmp44F,((_tmp609->f3=tqv,((_tmp609->f4=_tmp450,_tmp609)))))))))),((_tmp608->tl=c_args,_tmp608))))));}{
struct Cyc_List_List*_tmp60A;c_arg_types=((_tmp60A=_cycalloc(sizeof(*_tmp60A)),((_tmp60A->hd=_tmp450,((_tmp60A->tl=c_arg_types,_tmp60A))))));};};}}
# 1728
c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);
c_arg_types=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_arg_types);{
void*_tmp454=Cyc_Port_fn_ct(_tmp449,c_arg_types);
# 1734
(*_tmp439->name).f1=Cyc_Absyn_Loc_n;
_tmp448=Cyc_Port_add_var(_tmp448,_tmp439->name,(void*)_tmp447,Cyc_Port_const_ct(),_tmp454);
Cyc_Port_add_return_type(_tmp448,_tmp449);
{struct Cyc_List_List*_tmp455=c_args;for(0;(unsigned int)_tmp455;_tmp455=_tmp455->tl){
struct _tuple16 _tmp456=*((struct _tuple16*)_tmp455->hd);struct _dyneither_ptr*_tmp458;void*_tmp459;void*_tmp45A;void*_tmp45B;struct _tuple16 _tmp457=_tmp456;_tmp458=_tmp457.f1;_tmp459=_tmp457.f2;_tmp45A=_tmp457.f3;_tmp45B=_tmp457.f4;{
struct _tuple0*_tmp60B;_tmp448=Cyc_Port_add_var(_tmp448,((_tmp60B=_cycalloc(sizeof(*_tmp60B)),((_tmp60B->f1=Cyc_Absyn_Loc_n,((_tmp60B->f2=_tmp458,_tmp60B)))))),_tmp459,_tmp45A,_tmp45B);};}}
# 1741
Cyc_Port_gen_stmt(_tmp448,_tmp439->body);
# 1746
Cyc_Port_generalize(0,_tmp454);{
# 1753
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,_tmp454);
# 1757
Cyc_Port_register_rgns(env,counts,1,(void*)_tmp447,_tmp454);
env=Cyc_Port_add_var(_tmp448,_tmp439->name,(void*)_tmp447,Cyc_Port_const_ct(),_tmp454);
if((unsigned int)predeclared){
# 1766
struct _tuple13 _tmp45D=*predeclared;void*_tmp45F;void*_tmp460;void*_tmp461;struct _tuple13 _tmp45E=_tmp45D;_tmp45F=_tmp45E.f1;_tmp460=(_tmp45E.f2)->f1;_tmp461=(_tmp45E.f2)->f2;
Cyc_Port_unifies(_tmp460,Cyc_Port_const_ct());
Cyc_Port_unifies(_tmp461,Cyc_Port_instantiate(_tmp454));
# 1770
Cyc_Port_register_rgns(env,counts,1,_tmp45F,_tmp454);}
# 1772
return env;};};};};_LL2BC: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp43A=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp433;if(_tmp43A->tag != 8)goto _LL2BE;else{_tmp43B=_tmp43A->f1;}}_LL2BD: {
# 1778
void*_tmp465=(void*)_check_null(_tmp43B->defn);
void*_tmp466=Cyc_Port_type_to_ctype(env,_tmp465);
(*_tmp43B->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,_tmp43B->name,_tmp465,_tmp466);
return env;}_LL2BE: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp43C=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp433;if(_tmp43C->tag != 5)goto _LL2C0;else{_tmp43D=_tmp43C->f1;}}_LL2BF: {
# 1788
struct _tuple0*_tmp467=_tmp43D->name;
(*_tmp43D->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp468=_tmp43D->kind;switch(_tmp468){case Cyc_Absyn_StructA: _LL2C4:
# 1794
 previous=Cyc_Port_lookup_struct_decl(env,_tmp467);
break;case Cyc_Absyn_UnionA: _LL2C5:
# 1797
 previous=Cyc_Port_lookup_union_decl(env,_tmp467);
break;}}
# 1800
if((unsigned int)_tmp43D->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp469=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43D->impl))->fields;for(0;(unsigned int)_tmp469;_tmp469=_tmp469->tl){
struct Cyc_Absyn_Aggrfield*_tmp46A=(struct Cyc_Absyn_Aggrfield*)_tmp469->hd;
void*qv;
if((env->gcenv)->porting){
qv=Cyc_Port_var();
Cyc_Port_register_const_cvar(env,qv,
(_tmp46A->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp46A->tq).loc);}else{
# 1816
qv=(_tmp46A->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
# 1818
void*_tmp46B=Cyc_Port_type_to_ctype(env,_tmp46A->type);
if(cf != 0){
struct Cyc_Port_Cfield _tmp46C=*((struct Cyc_Port_Cfield*)cf->hd);void*_tmp46E;void*_tmp46F;struct Cyc_Port_Cfield _tmp46D=_tmp46C;_tmp46E=_tmp46D.qual;_tmp46F=_tmp46D.type;
cf=cf->tl;
Cyc_Port_unifies(qv,_tmp46E);
Cyc_Port_unifies(_tmp46B,_tmp46F);}{
# 1825
struct Cyc_Port_Cfield*_tmp60E;struct Cyc_List_List*_tmp60D;curr=((_tmp60D=_cycalloc(sizeof(*_tmp60D)),((_tmp60D->hd=((_tmp60E=_cycalloc(sizeof(*_tmp60E)),((_tmp60E->qual=qv,((_tmp60E->f=_tmp46A->name,((_tmp60E->type=_tmp46B,_tmp60E)))))))),((_tmp60D->tl=curr,_tmp60D))))));};};}}
# 1827
curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1832
return env;};}_LL2C0: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp43E=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp433;if(_tmp43E->tag != 7)goto _LL2C2;else{_tmp43F=_tmp43E->f1;}}_LL2C1:
# 1837
(*_tmp43F->name).f1=Cyc_Absyn_Loc_n;
# 1839
if((unsigned int)_tmp43F->fields){
struct Cyc_List_List*_tmp472=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp43F->fields))->v;for(0;(unsigned int)_tmp472;_tmp472=_tmp472->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp472->hd)->name).f1=Cyc_Absyn_Loc_n;{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp611;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp610;env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp472->hd)->name,(void*)((_tmp610=_cycalloc(sizeof(*_tmp610)),((_tmp610[0]=((_tmp611.tag=13,((_tmp611.f1=_tmp43F->name,((_tmp611.f2=_tmp43F,_tmp611)))))),_tmp610)))),
Cyc_Port_const_ct(),Cyc_Port_arith_ct());};}}
# 1845
return env;_LL2C2:;_LL2C3:
# 1847
 if((env->gcenv)->porting){
const char*_tmp614;void*_tmp613;(_tmp613=0,Cyc_fprintf(Cyc_stderr,((_tmp614="Warning: Cyclone declaration found in code to be ported -- skipping.",_tag_dyneither(_tmp614,sizeof(char),69))),_tag_dyneither(_tmp613,sizeof(void*),0)));}
return env;_LL2B3:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1854
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(0;(unsigned int)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}
return env;}struct Cyc_Port_Cenv;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1861
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1863
struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(Cyc_Port_initial_cenv(),ds);
# 1865
struct Cyc_List_List*_tmp477=(env->gcenv)->pointer_edits;
struct Cyc_List_List*_tmp478=(env->gcenv)->qualifier_edits;
struct Cyc_List_List*_tmp479=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp47A=(env->gcenv)->edits;
# 1870
{struct Cyc_List_List*_tmp47B=_tmp477;for(0;(unsigned int)_tmp47B;_tmp47B=_tmp47B->tl){
struct _tuple14 _tmp47C=*((struct _tuple14*)_tmp47B->hd);void*_tmp47E;void*_tmp47F;unsigned int _tmp480;struct _tuple14 _tmp47D=_tmp47C;_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;_tmp480=_tmp47D.f3;
Cyc_Port_unifies(_tmp47E,_tmp47F);}}
# 1874
{struct Cyc_List_List*_tmp481=_tmp478;for(0;(unsigned int)_tmp481;_tmp481=_tmp481->tl){
struct _tuple14 _tmp482=*((struct _tuple14*)_tmp481->hd);void*_tmp484;void*_tmp485;unsigned int _tmp486;struct _tuple14 _tmp483=_tmp482;_tmp484=_tmp483.f1;_tmp485=_tmp483.f2;_tmp486=_tmp483.f3;
Cyc_Port_unifies(_tmp484,_tmp485);}}
# 1878
{struct Cyc_List_List*_tmp487=_tmp479;for(0;(unsigned int)_tmp487;_tmp487=_tmp487->tl){
struct _tuple14 _tmp488=*((struct _tuple14*)_tmp487->hd);void*_tmp48A;void*_tmp48B;unsigned int _tmp48C;struct _tuple14 _tmp489=_tmp488;_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;_tmp48C=_tmp489.f3;
Cyc_Port_unifies(_tmp48A,_tmp48B);}}
# 1884
for(0;(unsigned int)_tmp477;_tmp477=_tmp477->tl){
struct _tuple14 _tmp48D=*((struct _tuple14*)_tmp477->hd);void*_tmp48F;void*_tmp490;unsigned int _tmp491;struct _tuple14 _tmp48E=_tmp48D;_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;_tmp491=_tmp48E.f3;
if(!Cyc_Port_unifies(_tmp48F,_tmp490) && (int)_tmp491){
void*_tmp492=Cyc_Port_compress_ct(_tmp490);void*_tmp493=_tmp492;_LL2C8: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp494=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmp493;if(_tmp494->tag != 2)goto _LL2CA;}_LL2C9:
# 1889
{struct Cyc_Port_Edit*_tmp61D;const char*_tmp61C;const char*_tmp61B;struct Cyc_List_List*_tmp61A;_tmp47A=((_tmp61A=_cycalloc(sizeof(*_tmp61A)),((_tmp61A->hd=((_tmp61D=_cycalloc(sizeof(*_tmp61D)),((_tmp61D->loc=_tmp491,((_tmp61D->old_string=((_tmp61C="?",_tag_dyneither(_tmp61C,sizeof(char),2))),((_tmp61D->new_string=((_tmp61B="*",_tag_dyneither(_tmp61B,sizeof(char),2))),_tmp61D)))))))),((_tmp61A->tl=_tmp47A,_tmp61A))))));}
goto _LL2C7;_LL2CA: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp495=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_tmp493;if(_tmp495->tag != 3)goto _LL2CC;}_LL2CB:
# 1892
{struct Cyc_Port_Edit*_tmp626;const char*_tmp625;const char*_tmp624;struct Cyc_List_List*_tmp623;_tmp47A=((_tmp623=_cycalloc(sizeof(*_tmp623)),((_tmp623->hd=((_tmp626=_cycalloc(sizeof(*_tmp626)),((_tmp626->loc=_tmp491,((_tmp626->old_string=((_tmp625="*",_tag_dyneither(_tmp625,sizeof(char),2))),((_tmp626->new_string=((_tmp624="?",_tag_dyneither(_tmp624,sizeof(char),2))),_tmp626)))))))),((_tmp623->tl=_tmp47A,_tmp623))))));}
goto _LL2C7;_LL2CC:;_LL2CD:
 goto _LL2C7;_LL2C7:;}}
# 1899
for(0;(unsigned int)_tmp478;_tmp478=_tmp478->tl){
struct _tuple14 _tmp49E=*((struct _tuple14*)_tmp478->hd);void*_tmp4A0;void*_tmp4A1;unsigned int _tmp4A2;struct _tuple14 _tmp49F=_tmp49E;_tmp4A0=_tmp49F.f1;_tmp4A1=_tmp49F.f2;_tmp4A2=_tmp49F.f3;
if(!Cyc_Port_unifies(_tmp4A0,_tmp4A1) && (int)_tmp4A2){
void*_tmp4A3=Cyc_Port_compress_ct(_tmp4A1);void*_tmp4A4=_tmp4A3;_LL2CF: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp4A5=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp4A4;if(_tmp4A5->tag != 1)goto _LL2D1;}_LL2D0:
# 1904
{struct Cyc_Port_Edit*_tmp62F;const char*_tmp62E;const char*_tmp62D;struct Cyc_List_List*_tmp62C;_tmp47A=((_tmp62C=_cycalloc(sizeof(*_tmp62C)),((_tmp62C->hd=((_tmp62F=_cycalloc(sizeof(*_tmp62F)),((_tmp62F->loc=_tmp4A2,((_tmp62F->old_string=((_tmp62E="const ",_tag_dyneither(_tmp62E,sizeof(char),7))),((_tmp62F->new_string=((_tmp62D="",_tag_dyneither(_tmp62D,sizeof(char),1))),_tmp62F)))))))),((_tmp62C->tl=_tmp47A,_tmp62C))))));}goto _LL2CE;_LL2D1: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp4A6=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp4A4;if(_tmp4A6->tag != 0)goto _LL2D3;}_LL2D2:
# 1906
{struct Cyc_Port_Edit*_tmp638;const char*_tmp637;const char*_tmp636;struct Cyc_List_List*_tmp635;_tmp47A=((_tmp635=_cycalloc(sizeof(*_tmp635)),((_tmp635->hd=((_tmp638=_cycalloc(sizeof(*_tmp638)),((_tmp638->loc=_tmp4A2,((_tmp638->old_string=((_tmp637="",_tag_dyneither(_tmp637,sizeof(char),1))),((_tmp638->new_string=((_tmp636="const ",_tag_dyneither(_tmp636,sizeof(char),7))),_tmp638)))))))),((_tmp635->tl=_tmp47A,_tmp635))))));}goto _LL2CE;_LL2D3:;_LL2D4:
 goto _LL2CE;_LL2CE:;}}
# 1912
for(0;(unsigned int)_tmp479;_tmp479=_tmp479->tl){
struct _tuple14 _tmp4AF=*((struct _tuple14*)_tmp479->hd);void*_tmp4B1;void*_tmp4B2;unsigned int _tmp4B3;struct _tuple14 _tmp4B0=_tmp4AF;_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;_tmp4B3=_tmp4B0.f3;
if(!Cyc_Port_unifies(_tmp4B1,_tmp4B2) && (int)_tmp4B3){
void*_tmp4B4=Cyc_Port_compress_ct(_tmp4B2);void*_tmp4B5=_tmp4B4;_LL2D6: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp4B6=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp4B5;if(_tmp4B6->tag != 8)goto _LL2D8;}_LL2D7:
# 1917
{struct Cyc_Port_Edit*_tmp641;const char*_tmp640;const char*_tmp63F;struct Cyc_List_List*_tmp63E;_tmp47A=((_tmp63E=_cycalloc(sizeof(*_tmp63E)),((_tmp63E->hd=((_tmp641=_cycalloc(sizeof(*_tmp641)),((_tmp641->loc=_tmp4B3,((_tmp641->old_string=((_tmp640="@nozeroterm ",_tag_dyneither(_tmp640,sizeof(char),13))),((_tmp641->new_string=((_tmp63F="",_tag_dyneither(_tmp63F,sizeof(char),1))),_tmp641)))))))),((_tmp63E->tl=_tmp47A,_tmp63E))))));}goto _LL2D5;_LL2D8: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp4B7=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp4B5;if(_tmp4B7->tag != 9)goto _LL2DA;}_LL2D9:
# 1919
{struct Cyc_Port_Edit*_tmp64A;const char*_tmp649;const char*_tmp648;struct Cyc_List_List*_tmp647;_tmp47A=((_tmp647=_cycalloc(sizeof(*_tmp647)),((_tmp647->hd=((_tmp64A=_cycalloc(sizeof(*_tmp64A)),((_tmp64A->loc=_tmp4B3,((_tmp64A->old_string=((_tmp649="@zeroterm ",_tag_dyneither(_tmp649,sizeof(char),11))),((_tmp64A->new_string=((_tmp648="",_tag_dyneither(_tmp648,sizeof(char),1))),_tmp64A)))))))),((_tmp647->tl=_tmp47A,_tmp647))))));}goto _LL2D5;_LL2DA:;_LL2DB:
 goto _LL2D5;_LL2D5:;}}
# 1926
_tmp47A=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp47A);
# 1928
while((unsigned int)_tmp47A  && ((struct Cyc_Port_Edit*)_tmp47A->hd)->loc == 0){
# 1932
_tmp47A=_tmp47A->tl;}
# 1934
return _tmp47A;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1937
static unsigned int Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
return e->loc;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1943
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp4C0=Cyc_Port_gen_edits(ds);
struct Cyc_List_List*_tmp4C1=((struct Cyc_List_List*(*)(unsigned int(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp4C0);
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp4C2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp4C1));
for(0;(unsigned int)_tmp4C0;(_tmp4C0=_tmp4C0->tl,_tmp4C2=_tmp4C2->tl)){
struct Cyc_Port_Edit _tmp4C3=*((struct Cyc_Port_Edit*)_tmp4C0->hd);unsigned int _tmp4C5;struct _dyneither_ptr _tmp4C6;struct _dyneither_ptr _tmp4C7;struct Cyc_Port_Edit _tmp4C4=_tmp4C3;_tmp4C5=_tmp4C4.loc;_tmp4C6=_tmp4C4.old_string;_tmp4C7=_tmp4C4.new_string;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp4C2))->hd);
const char*_tmp650;void*_tmp64F[3];struct Cyc_String_pa_PrintArg_struct _tmp64E;struct Cyc_String_pa_PrintArg_struct _tmp64D;struct Cyc_String_pa_PrintArg_struct _tmp64C;(_tmp64C.tag=0,((_tmp64C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4C7),((_tmp64D.tag=0,((_tmp64D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4C6),((_tmp64E.tag=0,((_tmp64E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sloc),((_tmp64F[0]=& _tmp64E,((_tmp64F[1]=& _tmp64D,((_tmp64F[2]=& _tmp64C,Cyc_printf(((_tmp650="%s: insert `%s' for `%s'\n",_tag_dyneither(_tmp650,sizeof(char),26))),_tag_dyneither(_tmp64F,sizeof(void*),3)))))))))))))))))));};}};}
