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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 119
void*Cyc_Core_identity(void*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 54 "string.h"
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
# 43 "fn.h"
typedef struct Cyc_Fn_Function*Cyc_Fn_fn_t;
# 48
struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(void*,void*),void*x);
# 51
struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*f)(void*));
# 54
void*Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
# 41
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct _tuple0{struct _dyneither_ptr f1;struct Cyc_List_List*f2;};
# 56
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 74
struct Cyc_PP_Doc*Cyc_PP_oline_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 88
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 97
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);
# 100
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs);
# 103
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);
# 105
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 126
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
# 43 "pp.cyc"
typedef int Cyc_PP_dummy_t;struct Cyc_PP_Empty_PP_Alist_struct{int tag;int f1;};struct Cyc_PP_Single_PP_Alist_struct{int tag;void*f1;};struct Cyc_PP_Append_PP_Alist_struct{int tag;void*f1;void*f2;};
# 50
typedef void*Cyc_PP_alist_t;
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_stringptr={0,0};
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_link={0,0};struct _tuple1{void*f1;void*f2;};
# 54
void*Cyc_PP_append(void*a1,void*a2){
struct _tuple1 _tmpA2;struct _tuple1 _tmp2=(_tmpA2.f1=a1,((_tmpA2.f2=a2,_tmpA2)));struct _tuple1 _tmp3=_tmp2;if(((struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f1)->tag == 0){_LL1: _LL2:
 return a2;}else{if(((struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f2)->tag == 0){_LL3: _LL4:
 return a1;}else{_LL5: _LL6: {
struct Cyc_PP_Append_PP_Alist_struct _tmpA5;struct Cyc_PP_Append_PP_Alist_struct*_tmpA4;return(void*)((_tmpA4=_cycalloc(sizeof(*_tmpA4)),((_tmpA4[0]=((_tmpA5.tag=2,((_tmpA5.f1=a1,((_tmpA5.f2=a2,_tmpA5)))))),_tmpA4))));}}}_LL0:;}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){
void*_tmp7=y;void*_tmp8;void*_tmp9;void*_tmpA;switch(*((int*)_tmp7)){case 0: _LL8: _LL9:
 return l;case 1: _LLA: _tmpA=(void*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmp7)->f1;_LLB: {
struct Cyc_List_List*_tmpA6;return(_tmpA6=_cycalloc(sizeof(*_tmpA6)),((_tmpA6->hd=_tmpA,((_tmpA6->tl=l,_tmpA6)))));}default: _LLC: _tmp8=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp7)->f1;_tmp9=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp7)->f2;_LLD:
 return Cyc_PP_list_of_alist_f(_tmp8,Cyc_PP_list_of_alist_f(_tmp9,l));}_LL7:;}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){
return Cyc_PP_list_of_alist_f(x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){
struct Cyc_Xarray_Xarray*_tmpC=((struct Cyc_Xarray_Xarray*(*)(int,void*))Cyc_Xarray_create)(16,al);
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmpC,al);{
int last=0;
while(last >= 0){
void*_tmpD=((void*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmpC,last);void*_tmpE=_tmpD;void*_tmpF;void*_tmp10;struct _dyneither_ptr*_tmp11;switch(*((int*)_tmpE)){case 0: _LLF: _LL10:
 -- last;goto _LLE;case 1: _LL11: _tmp11=(struct _dyneither_ptr*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmpE)->f1;_LL12:
 -- last;{const char*_tmpAA;void*_tmpA9[1];struct Cyc_String_pa_PrintArg_struct _tmpA8;(_tmpA8.tag=0,((_tmpA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp11),((_tmpA9[0]=& _tmpA8,Cyc_fprintf(f,((_tmpAA="%s",_tag_dyneither(_tmpAA,sizeof(char),3))),_tag_dyneither(_tmpA9,sizeof(void*),1)))))));}goto _LLE;default: _LL13: _tmpF=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmpE)->f1;_tmp10=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmpE)->f2;_LL14:
# 107
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmpC,last,_tmp10);
if(last == ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmpC)- 1)
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmpC,_tmpF);else{
# 111
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmpC,last + 1,_tmpF);}
++ last;
goto _LLE;}_LLE:;}
# 116
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpC);};}
# 120
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){
struct Cyc_PP_Ppstate*_tmpAB;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpAB=_cycalloc_atomic(sizeof(*_tmpAB)),((_tmpAB->ci=0,((_tmpAB->cc=0,((_tmpAB->cl=1,((_tmpAB->pw=w,((_tmpAB->epw=w,_tmpAB)))))))))))));
Cyc_PP_dump_out(f,o->ppout);}
# 126
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Ppstate*_tmpAC;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpAC=_cycalloc_atomic(sizeof(*_tmpAC)),((_tmpAC->ci=0,((_tmpAC->cc=0,((_tmpAC->cl=1,((_tmpAC->pw=w,((_tmpAC->epw=w,_tmpAC)))))))))))));
const char*_tmpAD;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout),((_tmpAD="",_tag_dyneither(_tmpAD,sizeof(char),1))));}
# 133
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Ppstate*_tmpAE;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpAE=_cycalloc_atomic(sizeof(*_tmpAE)),((_tmpAE->ci=0,((_tmpAE->cc=0,((_tmpAE->cl=1,((_tmpAE->pw=w,((_tmpAE->epw=w,_tmpAE)))))))))))));
const char*_tmpB1;struct _tuple0*_tmpB0;return(_tmpB0=_cycalloc(sizeof(*_tmpB0)),((_tmpB0->f1=(struct _dyneither_ptr)
Cyc_str_sepstr(((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout),((_tmpB1="",_tag_dyneither(_tmpB1,sizeof(char),1)))),((_tmpB0->f2=
((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->links),_tmpB0)))));}
# 140
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 142
int Cyc_PP_tex_output=0;
# 144
struct _dyneither_ptr Cyc_PP_nlblanks(int i){
if(Cyc_PP_bhashtbl == 0){
struct Cyc_Core_Opt*_tmpB2;Cyc_PP_bhashtbl=((_tmpB2=_cycalloc(sizeof(*_tmpB2)),((_tmpB2->v=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(int(*)(int))Cyc_Core_identity),_tmpB2))));}
if(i < 0){struct Cyc_Core_Failure_exn_struct _tmpB8;const char*_tmpB7;struct Cyc_Core_Failure_exn_struct*_tmpB6;(int)_throw((void*)((_tmpB6=_cycalloc(sizeof(*_tmpB6)),((_tmpB6[0]=((_tmpB8.tag=Cyc_Core_Failure,((_tmpB8.f1=((_tmpB7="nlblanks",_tag_dyneither(_tmpB7,sizeof(char),9))),_tmpB8)))),_tmpB6)))));}{
struct _handler_cons _tmp1F;_push_handler(& _tmp1F);{int _tmp21=0;if(setjmp(_tmp1F.handler))_tmp21=1;if(!_tmp21){
{struct _dyneither_ptr _tmp22=*((struct _dyneither_ptr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i);_npop_handler(0);return _tmp22;};_pop_handler();}else{void*_tmp20=(void*)_exn_thrown;void*_tmp23=_tmp20;void*_tmp24;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp23)->tag == Cyc_Core_Not_found){_LL16: _LL17:
# 152
 if(!Cyc_PP_tex_output){
int _tmp25=i / 8;
int _tmp26=i % 8;
int _tmp27=(2 + _tmp25)+ _tmp26;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)_tmp27);
{char _tmpBB;char _tmpBA;struct _dyneither_ptr _tmpB9;(_tmpB9=_dyneither_ptr_plus(nlb,sizeof(char),0),((_tmpBA=*((char*)_check_dyneither_subscript(_tmpB9,sizeof(char),0)),((_tmpBB='\n',((_get_dyneither_size(_tmpB9,sizeof(char))== 1  && (_tmpBA == '\000'  && _tmpBB != '\000')?_throw_arraybounds(): 1,*((char*)_tmpB9.curr)=_tmpBB)))))));}
{int j=0;for(0;j < _tmp25;++ j){
char _tmpBE;char _tmpBD;struct _dyneither_ptr _tmpBC;(_tmpBC=_dyneither_ptr_plus(nlb,sizeof(char),j + 1),((_tmpBD=*((char*)_check_dyneither_subscript(_tmpBC,sizeof(char),0)),((_tmpBE='\t',((_get_dyneither_size(_tmpBC,sizeof(char))== 1  && (_tmpBD == '\000'  && _tmpBE != '\000')?_throw_arraybounds(): 1,*((char*)_tmpBC.curr)=_tmpBE)))))));}}
{int j=0;for(0;j < _tmp26;++ j){
char _tmpC1;char _tmpC0;struct _dyneither_ptr _tmpBF;(_tmpBF=_dyneither_ptr_plus(nlb,sizeof(char),(j + 1)+ _tmp25),((_tmpC0=*((char*)_check_dyneither_subscript(_tmpBF,sizeof(char),0)),((_tmpC1=' ',((_get_dyneither_size(_tmpBF,sizeof(char))== 1  && (_tmpC0 == '\000'  && _tmpC1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpBF.curr)=_tmpC1)))))));}}
{struct _dyneither_ptr*_tmpC2;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpC2=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpC2[0]=(struct _dyneither_ptr)nlb,_tmpC2)))));}
return(struct _dyneither_ptr)nlb;}else{
# 168
int _tmp32=3 + i;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)(_tmp32 + 1));
{char _tmpC5;char _tmpC4;struct _dyneither_ptr _tmpC3;(_tmpC3=_dyneither_ptr_plus(nlb,sizeof(char),0),((_tmpC4=*((char*)_check_dyneither_subscript(_tmpC3,sizeof(char),0)),((_tmpC5='\\',((_get_dyneither_size(_tmpC3,sizeof(char))== 1  && (_tmpC4 == '\000'  && _tmpC5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpC3.curr)=_tmpC5)))))));}
{char _tmpC8;char _tmpC7;struct _dyneither_ptr _tmpC6;(_tmpC6=_dyneither_ptr_plus(nlb,sizeof(char),1),((_tmpC7=*((char*)_check_dyneither_subscript(_tmpC6,sizeof(char),0)),((_tmpC8='\\',((_get_dyneither_size(_tmpC6,sizeof(char))== 1  && (_tmpC7 == '\000'  && _tmpC8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpC6.curr)=_tmpC8)))))));}
{char _tmpCB;char _tmpCA;struct _dyneither_ptr _tmpC9;(_tmpC9=_dyneither_ptr_plus(nlb,sizeof(char),2),((_tmpCA=*((char*)_check_dyneither_subscript(_tmpC9,sizeof(char),0)),((_tmpCB='\n',((_get_dyneither_size(_tmpC9,sizeof(char))== 1  && (_tmpCA == '\000'  && _tmpCB != '\000')?_throw_arraybounds(): 1,*((char*)_tmpC9.curr)=_tmpCB)))))));}
{int j=3;for(0;j < _tmp32;++ j){
char _tmpCE;char _tmpCD;struct _dyneither_ptr _tmpCC;(_tmpCC=_dyneither_ptr_plus(nlb,sizeof(char),j),((_tmpCD=*((char*)_check_dyneither_subscript(_tmpCC,sizeof(char),0)),((_tmpCE='~',((_get_dyneither_size(_tmpCC,sizeof(char))== 1  && (_tmpCD == '\000'  && _tmpCE != '\000')?_throw_arraybounds(): 1,*((char*)_tmpCC.curr)=_tmpCE)))))));}}
{struct _dyneither_ptr*_tmpCF;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpCF=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpCF[0]=(struct _dyneither_ptr)nlb,_tmpCF)))));}
return(struct _dyneither_ptr)nlb;}}else{_LL18: _tmp24=_tmp23;_LL19:(int)_rethrow(_tmp24);}_LL15:;}};};}
# 182
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 184
int Cyc_PP_infinity=9999999;struct _tuple2{int f1;struct _dyneither_ptr f2;};
# 186
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){
struct _tuple2*_tmp40=clo;int _tmp41;struct _dyneither_ptr _tmp42;_LL1B: _tmp41=_tmp40->f1;_tmp42=_tmp40->f2;_LL1C: {
struct Cyc_PP_Single_PP_Alist_struct*_tmpD9;struct _dyneither_ptr*_tmpD8;struct Cyc_PP_Single_PP_Alist_struct _tmpD7;struct Cyc_PP_Out*_tmpD6;return(_tmpD6=_cycalloc(sizeof(*_tmpD6)),((_tmpD6->newcc=st->cc + _tmp41,((_tmpD6->newcl=st->cl,((_tmpD6->ppout=(void*)(
# 190
(_tmpD9=_cycalloc(sizeof(*_tmpD9)),((_tmpD9[0]=((_tmpD7.tag=1,((_tmpD7.f1=((_tmpD8=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpD8[0]=(struct _dyneither_ptr)_tmp42,_tmpD8)))),_tmpD7)))),_tmpD9)))),((_tmpD6->links=(void*)& Cyc_PP_Empty_link,_tmpD6)))))))));}_LL1A:;}
# 193
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _dyneither_ptr s){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)s);
struct _tuple2*_tmpDC;struct Cyc_PP_Doc*_tmpDB;return
(_tmpDB=_cycalloc(sizeof(*_tmpDB)),((_tmpDB->mwo=slen,((_tmpDB->mw=Cyc_PP_infinity,((_tmpDB->f=
# 198
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,(
# 200
(_tmpDC=_cycalloc(sizeof(*_tmpDC)),((_tmpDC->f1=slen,((_tmpDC->f2=s,_tmpDC))))))),_tmpDB)))))));}
# 202
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl == 0){
t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{
struct Cyc_Core_Opt*_tmpDD;Cyc_PP_str_hashtbl=((_tmpDD=_cycalloc(sizeof(*_tmpDD)),((_tmpDD->v=t,_tmpDD))));};}else{
# 208
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{
# 210
struct _handler_cons _tmp4A;_push_handler(& _tmp4A);{int _tmp4C=0;if(setjmp(_tmp4A.handler))_tmp4C=1;if(!_tmp4C){
{struct _dyneither_ptr*_tmpDE;struct Cyc_PP_Doc*_tmp4E=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((_tmpDE=_cycalloc(sizeof(*_tmpDE)),((_tmpDE[0]=s,_tmpDE)))));_npop_handler(0);return _tmp4E;};_pop_handler();}else{void*_tmp4B=(void*)_exn_thrown;void*_tmp4F=_tmp4B;void*_tmp50;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp4F)->tag == Cyc_Core_Not_found){_LL20: _LL21: {
# 214
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);
{struct _dyneither_ptr*_tmpDF;((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((_tmpDF=_cycalloc(sizeof(*_tmpDF)),((_tmpDF[0]=s,_tmpDF)))),d);}
return d;}}else{_LL22: _tmp50=_tmp4F;_LL23:(int)_rethrow(_tmp50);}_LL1F:;}};};}
# 219
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*s){return Cyc_PP_text(*s);}
# 224
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _dyneither_ptr s,int slen){
struct _tuple2*_tmpE2;struct Cyc_PP_Doc*_tmpE1;return
(_tmpE1=_cycalloc(sizeof(*_tmpE1)),((_tmpE1->mwo=slen,((_tmpE1->mw=Cyc_PP_infinity,((_tmpE1->f=
# 228
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,(
# 230
(_tmpE2=_cycalloc(sizeof(*_tmpE2)),((_tmpE2->f1=slen,((_tmpE2->f2=s,_tmpE2))))))),_tmpE1)))))));}
# 232
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int slen){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2 == 0){
t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{
struct Cyc_Core_Opt*_tmpE3;Cyc_PP_str_hashtbl2=((_tmpE3=_cycalloc(sizeof(*_tmpE3)),((_tmpE3->v=t,_tmpE3))));};}else{
# 239
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{
# 241
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){
{struct _dyneither_ptr*_tmpE4;struct Cyc_PP_Doc*_tmp59=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((_tmpE4=_cycalloc(sizeof(*_tmpE4)),((_tmpE4[0]=s,_tmpE4)))));_npop_handler(0);return _tmp59;};_pop_handler();}else{void*_tmp56=(void*)_exn_thrown;void*_tmp5A=_tmp56;void*_tmp5B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp5A)->tag == Cyc_Core_Not_found){_LL25: _LL26: {
# 245
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);
{struct _dyneither_ptr*_tmpE5;((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((_tmpE5=_cycalloc(sizeof(*_tmpE5)),((_tmpE5[0]=s,_tmpE5)))),d);}
return d;}}else{_LL27: _tmp5B=_tmp5A;_LL28:(int)_rethrow(_tmp5B);}_LL24:;}};};}
# 251
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 257
struct Cyc_PP_Doc*Cyc_PP_nil_doc(){
if(Cyc_PP_nil_doc_opt == 0){
const char*_tmpE8;struct Cyc_Core_Opt*_tmpE7;Cyc_PP_nil_doc_opt=((_tmpE7=_cycalloc(sizeof(*_tmpE7)),((_tmpE7->v=Cyc_PP_text(((_tmpE8="",_tag_dyneither(_tmpE8,sizeof(char),1)))),_tmpE7))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}
# 264
struct Cyc_PP_Doc*Cyc_PP_blank_doc(){
if(Cyc_PP_blank_doc_opt == 0){
const char*_tmpEB;struct Cyc_Core_Opt*_tmpEA;Cyc_PP_blank_doc_opt=((_tmpEA=_cycalloc(sizeof(*_tmpEA)),((_tmpEA->v=Cyc_PP_text(((_tmpEB="",_tag_dyneither(_tmpEB,sizeof(char),1)))),_tmpEA))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple3{int f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple4{int f1;int f2;int f3;struct _dyneither_ptr f4;};
# 270
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){
struct _tuple3*_tmp61=clo;int _tmp62;struct _dyneither_ptr _tmp63;struct _dyneither_ptr _tmp64;_LL2A: _tmp62=_tmp61->f1;_tmp63=_tmp61->f2;_tmp64=_tmp61->f3;_LL2B: {
struct Cyc_PP_Single_PP_Alist_struct*_tmpFE;struct _tuple4*_tmpFD;struct Cyc_PP_Single_PP_Alist_struct _tmpFC;struct Cyc_PP_Single_PP_Alist_struct*_tmpFB;struct _dyneither_ptr*_tmpFA;struct Cyc_PP_Single_PP_Alist_struct _tmpF9;struct Cyc_PP_Out*_tmpF8;return(_tmpF8=_cycalloc(sizeof(*_tmpF8)),((_tmpF8->newcc=st->cc + _tmp62,((_tmpF8->newcl=st->cl,((_tmpF8->ppout=(void*)(
# 274
(_tmpFB=_cycalloc(sizeof(*_tmpFB)),((_tmpFB[0]=((_tmpF9.tag=1,((_tmpF9.f1=((_tmpFA=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpFA[0]=(struct _dyneither_ptr)_tmp63,_tmpFA)))),_tmpF9)))),_tmpFB)))),((_tmpF8->links=(void*)(
(_tmpFE=_cycalloc(sizeof(*_tmpFE)),((_tmpFE[0]=((_tmpFC.tag=1,((_tmpFC.f1=((_tmpFD=_cycalloc(sizeof(*_tmpFD)),((_tmpFD->f1=st->cl,((_tmpFD->f2=st->cc,((_tmpFD->f3=_tmp62,((_tmpFD->f4=_tmp64,_tmpFD)))))))))),_tmpFC)))),_tmpFE)))),_tmpF8)))))))));}_LL29:;}
# 277
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)shrt);
struct _tuple3*_tmp101;struct Cyc_PP_Doc*_tmp100;return(_tmp100=_cycalloc(sizeof(*_tmp100)),((_tmp100->mwo=slen,((_tmp100->mw=Cyc_PP_infinity,((_tmp100->f=
# 281
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,((_tmp101=_cycalloc(sizeof(*_tmp101)),((_tmp101->f1=slen,((_tmp101->f2=shrt,((_tmp101->f3=full,_tmp101))))))))),_tmp100)))))));}
# 285
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Single_PP_Alist_struct*_tmp10B;struct _dyneither_ptr*_tmp10A;struct Cyc_PP_Single_PP_Alist_struct _tmp109;struct Cyc_PP_Out*_tmp108;return(_tmp108=_cycalloc(sizeof(*_tmp108)),((_tmp108->newcc=st->ci,((_tmp108->newcl=st->cl + 1,((_tmp108->ppout=(void*)(
# 288
(_tmp10B=_cycalloc(sizeof(*_tmp10B)),((_tmp10B[0]=((_tmp109.tag=1,((_tmp109.f1=((_tmp10A=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp10A[0]=(struct _dyneither_ptr)Cyc_PP_nlblanks(st->ci),_tmp10A)))),_tmp109)))),_tmp10B)))),((_tmp108->links=(void*)& Cyc_PP_Empty_link,_tmp108)))))))));}
# 291
struct Cyc_PP_Doc*Cyc_PP_line_doc(){
if(Cyc_PP_line_doc_opt == 0){
struct Cyc_PP_Doc*_tmp10E;struct Cyc_Core_Opt*_tmp10D;Cyc_PP_line_doc_opt=((_tmp10D=_cycalloc(sizeof(*_tmp10D)),((_tmp10D->v=((_tmp10E=_cycalloc(sizeof(*_tmp10E)),((_tmp10E->mwo=0,((_tmp10E->mw=0,((_tmp10E->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(Cyc_PP_line_f),_tmp10E)))))))),_tmp10D))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple5{int f1;struct Cyc_PP_Doc*f2;};
# 297
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){
struct _tuple5*_tmp74=clo;int _tmp75;struct Cyc_PP_Doc*_tmp76;_LL2F: _tmp75=_tmp74->f1;_tmp76=_tmp74->f2;_LL30: {
struct Cyc_PP_Ppstate*_tmp10F;struct Cyc_PP_Ppstate*st2=
(_tmp10F=_cycalloc_atomic(sizeof(*_tmp10F)),((_tmp10F->ci=st->ci + _tmp75,((_tmp10F->cc=st->cc,((_tmp10F->cl=st->cl,((_tmp10F->pw=st->pw,((_tmp10F->epw=st->epw,_tmp10F)))))))))));
# 305
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp76->f,st2);}_LL2E:;}
# 307
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
struct _tuple5*_tmp112;struct Cyc_PP_Doc*_tmp111;return(_tmp111=_cycalloc(sizeof(*_tmp111)),((_tmp111->mwo=d->mwo,((_tmp111->mw=d->mw,((_tmp111->f=
# 310
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,(
# 312
(_tmp112=_cycalloc(sizeof(*_tmp112)),((_tmp112->f1=k,((_tmp112->f2=d,_tmp112))))))),_tmp111)))))));}
# 315
int Cyc_PP_min(int x,int y){
if(x <= y)return x;else{
return y;}}
# 320
int Cyc_PP_max(int x,int y){
if(x >= y)return x;else{
return y;}}struct _tuple6{struct Cyc_PP_Doc*f1;struct Cyc_PP_Doc*f2;};
# 325
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp7A=clo;struct Cyc_PP_Doc*_tmp7B;struct Cyc_PP_Doc*_tmp7C;_LL34: _tmp7B=_tmp7A->f1;_tmp7C=_tmp7A->f2;_LL35: {
int epw2=Cyc_PP_max(st->pw - _tmp7C->mw,st->epw - _tmp7B->mwo);
struct Cyc_PP_Ppstate*_tmp113;struct Cyc_PP_Ppstate*st2=(_tmp113=_cycalloc_atomic(sizeof(*_tmp113)),((_tmp113->ci=st->ci,((_tmp113->cc=st->cc,((_tmp113->cl=st->cl,((_tmp113->pw=st->pw,((_tmp113->epw=epw2,_tmp113)))))))))));
struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp7B->f,st2);
struct Cyc_PP_Ppstate*_tmp114;struct Cyc_PP_Ppstate*st3=(_tmp114=_cycalloc_atomic(sizeof(*_tmp114)),((_tmp114->ci=st->ci,((_tmp114->cc=o1->newcc,((_tmp114->cl=o1->newcl,((_tmp114->pw=st->pw,((_tmp114->epw=epw2,_tmp114)))))))))));
struct Cyc_PP_Out*o2=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp7C->f,st3);
struct Cyc_PP_Out*_tmp115;struct Cyc_PP_Out*o3=(_tmp115=_cycalloc(sizeof(*_tmp115)),((_tmp115->newcc=o2->newcc,((_tmp115->newcl=o2->newcl,((_tmp115->ppout=
# 334
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->ppout,o2->ppout),((_tmp115->links=
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->links,o2->links),_tmp115)))))))));
return o3;}_LL33:;}
# 338
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
struct _tuple6*_tmp118;struct Cyc_PP_Doc*_tmp117;return(_tmp117=_cycalloc(sizeof(*_tmp117)),((_tmp117->mwo=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo),((_tmp117->mw=
Cyc_PP_min(d1->mw,d1->mwo + d2->mw),((_tmp117->f=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,((_tmp118=_cycalloc(sizeof(*_tmp118)),((_tmp118->f1=d1,((_tmp118->f2=d2,_tmp118))))))),_tmp117)))))));}
# 343
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr l){
struct Cyc_PP_Doc*_tmp82=Cyc_PP_nil_doc();
{int i=(int)(_get_dyneither_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;-- i){
_tmp82=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_dyneither_subscript(l,sizeof(struct Cyc_PP_Doc*),i)),_tmp82);}}
# 348
return _tmp82;}
# 353
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
{struct Cyc_List_List*_tmp83=ds0;for(0;_tmp83 != 0;_tmp83=_tmp83->tl){
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_tmp83->hd;
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
{struct Cyc_PP_Ppstate*_tmp119;st=((_tmp119=_cycalloc_atomic(sizeof(*_tmp119)),((_tmp119->ci=st->ci,((_tmp119->cc=o->newcc,((_tmp119->cl=o->newcl,((_tmp119->pw=st->pw,((_tmp119->epw=st->epw - d->mwo,_tmp119))))))))))));}{
struct Cyc_List_List*_tmp11A;os=((_tmp11A=_cycalloc(sizeof(*_tmp11A)),((_tmp11A->hd=o,((_tmp11A->tl=os,_tmp11A))))));};}}{
# 361
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
s=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);}{
# 369
struct Cyc_PP_Out*_tmp11B;return(_tmp11B=_cycalloc(sizeof(*_tmp11B)),((_tmp11B->newcc=newcc,((_tmp11B->newcl=newcl,((_tmp11B->ppout=s,((_tmp11B->links=links,_tmp11B)))))))));};};}
# 371
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){
# 375
struct Cyc_List_List*orig=doclist;
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;
doclist=doclist->tl;{
int mw=d->mw;
int mwo=d->mw;
# 381
{struct Cyc_List_List*_tmp87=doclist;for(0;_tmp87 != 0;_tmp87=_tmp87->tl){
int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp87->hd)->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp87->hd)->mw);
mw=mw2;
mwo=mwo2;}}{
# 387
struct Cyc_PP_Doc*_tmp11C;return(_tmp11C=_cycalloc(sizeof(*_tmp11C)),((_tmp11C->mwo=mw,((_tmp11C->mw=mwo,((_tmp11C->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(Cyc_PP_long_cats_f,orig),_tmp11C)))))));};};}
# 390
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){
if(doclist == 0)return Cyc_PP_nil_doc();else{
if(doclist->tl == 0)return(struct Cyc_PP_Doc*)doclist->hd;else{
# 394
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(doclist)> 30)return Cyc_PP_long_cats(doclist);else{
return Cyc_PP_concat((struct Cyc_PP_Doc*)doclist->hd,Cyc_PP_cats(doclist->tl));}}}}
# 398
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _dyneither_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
struct _dyneither_ptr docs=*docs_ptr;
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
{int i=0;for(0;i < sz;++ i){
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
{struct Cyc_PP_Ppstate*_tmp11D;st=((_tmp11D=_cycalloc_atomic(sizeof(*_tmp11D)),((_tmp11D->ci=st->ci,((_tmp11D->cc=o->newcc,((_tmp11D->cl=o->newcl,((_tmp11D->pw=st->pw,((_tmp11D->epw=st->epw - d->mwo,_tmp11D))))))))))));}{
struct Cyc_List_List*_tmp11E;os=((_tmp11E=_cycalloc(sizeof(*_tmp11E)),((_tmp11E->hd=o,((_tmp11E->tl=os,_tmp11E))))));};}}{
# 408
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
s=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);}{
# 416
struct Cyc_PP_Out*_tmp11F;return(_tmp11F=_cycalloc(sizeof(*_tmp11F)),((_tmp11F->newcc=newcc,((_tmp11F->newcl=newcl,((_tmp11F->ppout=s,((_tmp11F->links=links,_tmp11F)))))))));};};}
# 419
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs){
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz == 0){
struct Cyc_Core_Failure_exn_struct _tmp125;const char*_tmp124;struct Cyc_Core_Failure_exn_struct*_tmp123;(int)_throw((void*)((_tmp123=_cycalloc(sizeof(*_tmp123)),((_tmp123[0]=((_tmp125.tag=Cyc_Core_Failure,((_tmp125.f1=((_tmp124="cats_arr -- size zero array",_tag_dyneither(_tmp124,sizeof(char),28))),_tmp125)))),_tmp123)))));}{
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),0));
int mw=d->mw;
int mwo=d->mw;
{int i=1;for(0;i < sz;++ i){
int mw2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);
int mwo2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mw);
mw=mw2;
mwo=mwo2;}}{
# 432
struct _dyneither_ptr*_tmp128;struct Cyc_PP_Doc*_tmp127;return(_tmp127=_cycalloc(sizeof(*_tmp127)),((_tmp127->mwo=mw,((_tmp127->mw=mwo,((_tmp127->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _dyneither_ptr*,struct Cyc_PP_Ppstate*),struct _dyneither_ptr*x))Cyc_Fn_make_fn)(Cyc_PP_cats_arr_f,((_tmp128=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp128[0]=(struct _dyneither_ptr)docs,_tmp128))))),_tmp127)))))));};};}
# 435
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp91=clo;struct Cyc_PP_Doc*_tmp92;struct Cyc_PP_Doc*_tmp93;_LL39: _tmp92=_tmp91->f1;_tmp93=_tmp91->f2;_LL3A: {
int dfits=st->cc + _tmp92->mwo <= st->epw  || st->cc + _tmp92->mw <= st->pw;
# 439
if(dfits)return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp92->f,st);else{
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp93->f,st);}}_LL38:;}
# 442
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){
struct _tuple6*_tmp12B;struct Cyc_PP_Doc*_tmp12A;return(_tmp12A=_cycalloc(sizeof(*_tmp12A)),((_tmp12A->mwo=Cyc_PP_min(d->mwo,d2->mwo),((_tmp12A->mw=
Cyc_PP_min(d->mw,d2->mw),((_tmp12A->f=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,((_tmp12B=_cycalloc(sizeof(*_tmp12B)),((_tmp12B->f1=d,((_tmp12B->f2=d2,_tmp12B))))))),_tmp12A)))))));}
# 449
struct Cyc_PP_Doc*Cyc_PP_oline_doc(){
return Cyc_PP_doc_union(Cyc_PP_nil_doc(),Cyc_PP_line_doc());}
# 453
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*_tmp12C;struct Cyc_PP_Ppstate*st2=(_tmp12C=_cycalloc_atomic(sizeof(*_tmp12C)),((_tmp12C->ci=st->cc,((_tmp12C->cc=st->cc,((_tmp12C->cl=st->cl,((_tmp12C->pw=st->pw,((_tmp12C->epw=st->epw,_tmp12C)))))))))));
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st2);}
# 457
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){
struct Cyc_PP_Doc*_tmp12D;struct Cyc_PP_Doc*d2=
(_tmp12D=_cycalloc(sizeof(*_tmp12D)),((_tmp12D->mwo=d->mwo,((_tmp12D->mw=d->mw,((_tmp12D->f=
# 461
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d),_tmp12D)))))));
return d2;}
# 467
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
struct Cyc_PP_Doc*_tmp12E[4];return(_tmp12E[3]=Cyc_PP_ppseq_f(pp,sep,l->tl),((_tmp12E[2]=Cyc_PP_oline_doc(),((_tmp12E[1]=Cyc_PP_text(sep),((_tmp12E[0]=pp(l->hd),Cyc_PP_cat(_tag_dyneither(_tmp12E,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 473
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}
# 477
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
# 481
struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmp99=l;
while(_tmp99->tl != 0){
struct Cyc_List_List*_tmp9A=_tmp99->tl;
{struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp130;_tmp99->tl=((_tmp130=_cycalloc(sizeof(*_tmp130)),((_tmp130->hd=sep2,((_tmp130->tl=((_tmp131=_cycalloc(sizeof(*_tmp131)),((_tmp131->hd=oline,((_tmp131->tl=_tmp9A,_tmp131)))))),_tmp130))))));}
_tmp99=_tmp9A;}
# 489
return Cyc_PP_cats(l);}}}
# 497
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}
# 502
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
struct Cyc_PP_Doc*_tmp132[4];return(_tmp132[3]=Cyc_PP_ppseql_f(pp,sep,l->tl),((_tmp132[2]=Cyc_PP_line_doc(),((_tmp132[1]=Cyc_PP_text(sep),((_tmp132[0]=pp(l->hd),Cyc_PP_cat(_tag_dyneither(_tmp132,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 507
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,l));}
# 511
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
struct Cyc_PP_Doc*_tmp133[4];return(_tmp133[3]=Cyc_PP_seql_f(sep,l->tl),((_tmp133[2]=Cyc_PP_line_doc(),((_tmp133[1]=Cyc_PP_text(sep),((_tmp133[0]=(struct Cyc_PP_Doc*)l->hd,Cyc_PP_cat(_tag_dyneither(_tmp133,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 516
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}
# 521
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp134[3];return(_tmp134[2]=
# 524
Cyc_PP_text(stop),((_tmp134[1]=
# 523
Cyc_PP_seq(sep,ss),((_tmp134[0]=
# 522
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp134,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 528
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
if(ss == 0)return Cyc_PP_nil_doc();else{
struct Cyc_PP_Doc*_tmp135[3];return(_tmp135[2]=
# 532
Cyc_PP_text(stop),((_tmp135[1]=
# 531
Cyc_PP_seq(sep,ss),((_tmp135[0]=
# 530
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp135,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 535
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp136[3];return(_tmp136[2]=
# 538
Cyc_PP_text(stop),((_tmp136[1]=
# 537
Cyc_PP_seql(sep,ss),((_tmp136[0]=
# 536
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp136,sizeof(struct Cyc_PP_Doc*),3)))))));}
