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

# 26 "cycboot.h"
 typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 947 "absyn.h"
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 42
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RType(void*);union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple10{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 65
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
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
# 99
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 101
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 103
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 105
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 107
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 109
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 111
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 113
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r);
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 122
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 137 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 161
int Cyc_Tcutil_unify(void*,void*);
# 328 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned int val;};struct _union_Node_NParamNumelts{int tag;unsigned int val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 61 "pratt.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
typedef union Cyc_Pratt_Node Cyc_Pratt_node_t;
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NType(void*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned int);
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned int);
union Cyc_Pratt_Node Cyc_Pratt_NReturn();struct Cyc_Pratt_Graph;
# 72
typedef struct Cyc_Pratt_Graph Cyc_Pratt_graph_t;
# 76
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph();
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*);
# 81
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,int a);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 64 "string.h"
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 31 "relations.cyc"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int i){
union Cyc_Relations_RelnOp _tmpF6;return((_tmpF6.RConst).val=i,(((_tmpF6.RConst).tag=1,_tmpF6)));}
# 34
union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmpF7;return((_tmpF7.RVar).val=v,(((_tmpF7.RVar).tag=2,_tmpF7)));}
# 37
union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmpF8;return((_tmpF8.RNumelts).val=v,(((_tmpF8.RNumelts).tag=3,_tmpF8)));}
# 40
union Cyc_Relations_RelnOp Cyc_Relations_RType(void*t){
union Cyc_Relations_RelnOp _tmpF9;return((_tmpF9.RType).val=Cyc_Tcutil_compress(t),(((_tmpF9.RType).tag=4,_tmpF9)));}
# 43
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int i){
union Cyc_Relations_RelnOp _tmpFA;return((_tmpFA.RParam).val=i,(((_tmpFA.RParam).tag=5,_tmpFA)));}
# 46
union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int i){
union Cyc_Relations_RelnOp _tmpFB;return((_tmpFB.RParamNumelts).val=i,(((_tmpFB.RParamNumelts).tag=6,_tmpFB)));}
# 49
union Cyc_Relations_RelnOp Cyc_Relations_RReturn(){
union Cyc_Relations_RelnOp _tmpFC;return((_tmpFC.RReturn).val=0,(((_tmpFC.RReturn).tag=7,_tmpFC)));}struct _tuple12{union Cyc_Relations_RelnOp f1;union Cyc_Relations_RelnOp f2;};
# 53
static int Cyc_Relations_same_relop(union Cyc_Relations_RelnOp r1,union Cyc_Relations_RelnOp r2){
struct _tuple12 _tmpFD;struct _tuple12 _tmp7=(_tmpFD.f1=r1,((_tmpFD.f2=r2,_tmpFD)));struct _tuple12 _tmp8=_tmp7;unsigned int _tmp9;unsigned int _tmpA;struct Cyc_Absyn_Vardecl*_tmpB;struct Cyc_Absyn_Vardecl*_tmpC;struct Cyc_Absyn_Vardecl*_tmpD;struct Cyc_Absyn_Vardecl*_tmpE;void*_tmpF;void*_tmp10;unsigned int _tmp11;unsigned int _tmp12;unsigned int _tmp13;unsigned int _tmp14;_LL1: if(((_tmp8.f1).RConst).tag != 1)goto _LL3;_tmp9=(unsigned int)((_tmp8.f1).RConst).val;if(((_tmp8.f2).RConst).tag != 1)goto _LL3;_tmpA=(unsigned int)((_tmp8.f2).RConst).val;_LL2:
 return _tmp9 == _tmpA;_LL3: if(((_tmp8.f1).RVar).tag != 2)goto _LL5;_tmpB=(struct Cyc_Absyn_Vardecl*)((_tmp8.f1).RVar).val;if(((_tmp8.f2).RVar).tag != 2)goto _LL5;_tmpC=(struct Cyc_Absyn_Vardecl*)((_tmp8.f2).RVar).val;_LL4:
 return _tmpB == _tmpC;_LL5: if(((_tmp8.f1).RNumelts).tag != 3)goto _LL7;_tmpD=(struct Cyc_Absyn_Vardecl*)((_tmp8.f1).RNumelts).val;if(((_tmp8.f2).RNumelts).tag != 3)goto _LL7;_tmpE=(struct Cyc_Absyn_Vardecl*)((_tmp8.f2).RNumelts).val;_LL6:
 return _tmpD == _tmpE;_LL7: if(((_tmp8.f1).RType).tag != 4)goto _LL9;_tmpF=(void*)((_tmp8.f1).RType).val;if(((_tmp8.f2).RType).tag != 4)goto _LL9;_tmp10=(void*)((_tmp8.f2).RType).val;_LL8:
 return Cyc_Tcutil_unify(_tmpF,_tmp10);_LL9: if(((_tmp8.f1).RParam).tag != 5)goto _LLB;_tmp11=(unsigned int)((_tmp8.f1).RParam).val;if(((_tmp8.f2).RParam).tag != 5)goto _LLB;_tmp12=(unsigned int)((_tmp8.f2).RParam).val;_LLA:
 return _tmp11 == _tmp12;_LLB: if(((_tmp8.f1).RParamNumelts).tag != 6)goto _LLD;_tmp13=(unsigned int)((_tmp8.f1).RParamNumelts).val;if(((_tmp8.f2).RParamNumelts).tag != 6)goto _LLD;_tmp14=(unsigned int)((_tmp8.f2).RParamNumelts).val;_LLC:
 return _tmp13 == _tmp14;_LLD: if(((_tmp8.f1).RReturn).tag != 7)goto _LLF;if(((_tmp8.f2).RReturn).tag != 7)goto _LLF;_LLE:
 return 1;_LLF:;_LL10:
 return 0;_LL0:;}
# 66
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation relation,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns){
# 70
{struct Cyc_List_List*_tmp16=relns;for(0;_tmp16 != 0;_tmp16=_tmp16->tl){
struct Cyc_Relations_Reln*_tmp17=(struct Cyc_Relations_Reln*)_tmp16->hd;
if((Cyc_Relations_same_relop(_tmp17->rop1,rop1) && _tmp17->relation == relation) && 
Cyc_Relations_same_relop(_tmp17->rop2,rop2))return relns;}}{
# 75
struct Cyc_Relations_Reln*_tmp100;struct Cyc_List_List*_tmpFF;return(_tmpFF=_region_malloc(rgn,sizeof(*_tmpFF)),((_tmpFF->hd=((_tmp100=_region_malloc(rgn,sizeof(*_tmp100)),((_tmp100->rop1=rop1,((_tmp100->relation=relation,((_tmp100->rop2=rop2,_tmp100)))))))),((_tmpFF->tl=relns,_tmpFF)))));};}
# 78
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 82
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_Relations_Reln*_tmp1A=(struct Cyc_Relations_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp1B=r2s;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){
struct Cyc_Relations_Reln*_tmp1C=(struct Cyc_Relations_Reln*)_tmp1B->hd;
if(_tmp1A == _tmp1C  || (Cyc_Relations_same_relop(_tmp1A->rop1,_tmp1C->rop1) && _tmp1A->relation == _tmp1C->relation) && 
# 89
Cyc_Relations_same_relop(_tmp1A->rop2,_tmp1C->rop2)){
found=1;
break;}}}
# 94
if(!found)
return 0;}
# 97
return 1;}
# 100
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp1D=r1s;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
struct Cyc_Relations_Reln*_tmp1E=(struct Cyc_Relations_Reln*)_tmp1D->hd;
int found=0;
{struct Cyc_List_List*_tmp1F=r2s;for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){
struct Cyc_Relations_Reln*_tmp20=(struct Cyc_Relations_Reln*)_tmp1F->hd;
if(_tmp1E == _tmp20  || (Cyc_Relations_same_relop(_tmp1E->rop1,_tmp20->rop1) && _tmp1E->relation == _tmp20->relation) && 
# 111
Cyc_Relations_same_relop(_tmp1E->rop2,_tmp20->rop2)){
{struct Cyc_List_List*_tmp101;res=((_tmp101=_region_malloc(r,sizeof(*_tmp101)),((_tmp101->hd=_tmp1E,((_tmp101->tl=res,_tmp101))))));}
found=1;
break;}}}
# 117
if(!found)diff=1;}}
# 119
if(!diff)res=r1s;
return res;};}
# 123
static int Cyc_Relations_rop_contains_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmp22=r;struct Cyc_Absyn_Vardecl*_tmp23;struct Cyc_Absyn_Vardecl*_tmp24;_LL12: if((_tmp22.RVar).tag != 2)goto _LL14;_tmp23=(struct Cyc_Absyn_Vardecl*)(_tmp22.RVar).val;_LL13:
 return v == _tmp23;_LL14: if((_tmp22.RNumelts).tag != 3)goto _LL16;_tmp24=(struct Cyc_Absyn_Vardecl*)(_tmp22.RNumelts).val;_LL15:
 return v == _tmp24;_LL16: if((_tmp22.RType).tag != 4)goto _LL18;_LL17:
 goto _LL19;_LL18: if((_tmp22.RParam).tag != 5)goto _LL1A;_LL19:
 goto _LL1B;_LL1A: if((_tmp22.RParamNumelts).tag != 6)goto _LL1C;_LL1B:
 goto _LL1D;_LL1C: if((_tmp22.RReturn).tag != 7)goto _LL1E;_LL1D:
 goto _LL1F;_LL1E: if((_tmp22.RConst).tag != 1)goto _LL11;_LL1F:
 return 0;_LL11:;}
# 135
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found  && p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp25=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp25->rop1,v) || Cyc_Relations_rop_contains_var(_tmp25->rop2,v)){
found=1;
break;}}
# 145
if(!found)return rs;{
# 147
struct Cyc_List_List*_tmp26=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp27=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp27->rop1,v) || Cyc_Relations_rop_contains_var(_tmp27->rop2,v))continue;{
struct Cyc_List_List*_tmp102;_tmp26=((_tmp102=_region_malloc(rgn,sizeof(*_tmp102)),((_tmp102->hd=_tmp27,((_tmp102->tl=_tmp26,_tmp102))))));};}
# 153
return _tmp26;};}
# 159
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p){
# 162
RETRY:
 if(e->topt != 0){
void*_tmp29=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A=_tmp29;void*_tmp2C;struct Cyc_Absyn_Exp*_tmp2E;_LL21: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2A;if(_tmp2B->tag != 19)goto _LL23;else{_tmp2C=(void*)_tmp2B->f1;}}_LL22:
# 166
*p=Cyc_Relations_RType(_tmp2C);return 1;
goto _LL20;_LL23: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2A;if(_tmp2D->tag != 18)goto _LL25;else{_tmp2E=_tmp2D->f1;}}_LL24:
# 169
 e=_tmp2E;
goto _LL20;_LL25:;_LL26:
 goto _LL20;_LL20:;}
# 174
{void*_tmp2F=e->r;void*_tmp30=_tmp2F;struct Cyc_Absyn_Exp*_tmp32;void*_tmp34;int _tmp36;void*_tmp39;void*_tmp3B;_LL28: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp31=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30;if(_tmp31->tag != 13)goto _LL2A;else{_tmp32=_tmp31->f2;}}_LL29:
 return Cyc_Relations_exp2relnop(_tmp32,p);_LL2A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30;if(_tmp33->tag != 1)goto _LL2C;else{_tmp34=(void*)_tmp33->f2;}}_LL2B: {
# 177
struct Cyc_Absyn_Vardecl*_tmp3C=Cyc_Tcutil_nonesc_vardecl(_tmp34);
if(_tmp3C != 0){
*p=Cyc_Relations_RVar(_tmp3C);
return 1;}
# 182
goto _LL27;}_LL2C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30;if(_tmp35->tag != 0)goto _LL2E;else{if(((_tmp35->f1).Int_c).tag != 5)goto _LL2E;_tmp36=((struct _tuple5)((_tmp35->f1).Int_c).val).f2;}}_LL2D:
# 184
*p=Cyc_Relations_RConst((unsigned int)_tmp36);
return 1;_LL2E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp37=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30;if(_tmp37->tag != 2)goto _LL30;else{if(_tmp37->f1 != Cyc_Absyn_Numelts)goto _LL30;if(_tmp37->f2 == 0)goto _LL30;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp38=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(_tmp37->f2)->hd)->r;if(_tmp38->tag != 1)goto _LL30;else{_tmp39=(void*)_tmp38->f2;}};}}_LL2F: {
# 187
struct Cyc_Absyn_Vardecl*_tmp3D=Cyc_Tcutil_nonesc_vardecl(_tmp39);
if(_tmp3D != 0){
*p=Cyc_Relations_RNumelts(_tmp3D);
return 1;}
# 192
goto _LL27;}_LL30: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp3A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp30;if(_tmp3A->tag != 38)goto _LL32;else{_tmp3B=(void*)_tmp3A->f1;}}_LL31:
# 194
{void*_tmp3E=Cyc_Tcutil_compress(_tmp3B);void*_tmp3F=_tmp3E;struct Cyc_Absyn_Exp*_tmp41;_LL35: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F;if(_tmp40->tag != 18)goto _LL37;else{_tmp41=_tmp40->f1;}}_LL36:
# 196
 e=_tmp41;
goto RETRY;_LL37:;_LL38:
# 199
*p=Cyc_Relations_RType(_tmp3B);
return 1;_LL34:;}
# 202
goto _LL27;_LL32:;_LL33:
 goto _LL27;_LL27:;}
# 206
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmp42=Cyc_Evexp_eval_const_uint_exp(e);unsigned int _tmp44;int _tmp45;struct _tuple11 _tmp43=_tmp42;_tmp44=_tmp43.f1;_tmp45=_tmp43.f2;
if(_tmp45){
*p=Cyc_Relations_RConst(_tmp44);
return 1;}}
# 213
return 0;}
# 216
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp46=e->r;void*_tmp47=_tmp46;void*_tmp49;_LL3A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp48=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp47;if(_tmp48->tag != 1)goto _LL3C;else{_tmp49=(void*)_tmp48->f2;}}_LL3B: {
# 219
struct Cyc_Absyn_Vardecl*_tmp4A=Cyc_Tcutil_nonesc_vardecl(_tmp49);
if(_tmp4A != 0)
return Cyc_Relations_reln_kill_var(rgn,r,_tmp4A);else{
return r;}}_LL3C:;_LL3D:
 return r;_LL39:;}
# 227
struct Cyc_Relations_Reln*Cyc_Relations_copy_reln(struct _RegionHandle*r2,struct Cyc_Relations_Reln*r){
struct Cyc_Relations_Reln*_tmp103;return(_tmp103=_region_malloc(r2,sizeof(*_tmp103)),((_tmp103[0]=*r,_tmp103)));}
# 231
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*(*f)(struct _RegionHandle*,struct Cyc_Relations_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_Relations_copy_reln,r2,relns);}
# 235
int Cyc_Relations_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_Relations_Reln*_tmp4C=(struct Cyc_Relations_Reln*)r1->hd;
struct Cyc_Relations_Reln*_tmp4D=(struct Cyc_Relations_Reln*)r2->hd;
if((!Cyc_Relations_same_relop(_tmp4C->rop1,_tmp4D->rop1) || _tmp4C->relation != _tmp4D->relation) || !
# 241
Cyc_Relations_same_relop(_tmp4C->rop2,_tmp4D->rop2))return 0;}
# 243
if(r1 != 0  || r2 != 0)return 0;else{
return 1;}}
# 247
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 251
if(v->escapes)return r;
# 254
r=Cyc_Relations_reln_kill_var(rgn,r,v);
# 259
{void*_tmp4E=e->r;void*_tmp4F=_tmp4E;struct Cyc_Absyn_Exp*_tmp51;_LL3F: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp50=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4F;if(_tmp50->tag != 33)goto _LL41;else{_tmp51=(_tmp50->f1).num_elts;if((_tmp50->f1).fat_result != 1)goto _LL41;}}_LL40:
# 262
 malloc_loop: {
void*_tmp52=_tmp51->r;void*_tmp53=_tmp52;struct Cyc_Absyn_Exp*_tmp55;void*_tmp57;_LL44: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp54=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53;if(_tmp54->tag != 13)goto _LL46;else{_tmp55=_tmp54->f2;}}_LL45:
 _tmp51=_tmp55;goto malloc_loop;_LL46: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp56=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53;if(_tmp56->tag != 1)goto _LL48;else{_tmp57=(void*)_tmp56->f2;}}_LL47: {
# 266
struct Cyc_Absyn_Vardecl*_tmp58=Cyc_Tcutil_nonesc_vardecl(_tmp57);
if(_tmp58 != 0)
return Cyc_Relations_add_relation(rgn,Cyc_Relations_RVar(_tmp58),Cyc_Relations_Req,Cyc_Relations_RNumelts(v),r);else{
return r;}}_LL48:;_LL49:
 return r;_LL43:;}_LL41:;_LL42:
# 273
 goto _LL3E;_LL3E:;}
# 276
{void*_tmp59=Cyc_Tcutil_compress(v->type);void*_tmp5A=_tmp59;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A;if(_tmp5B->tag != 6)goto _LL4D;}_LL4C:
 goto _LL4A;_LL4D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5A;if(_tmp5C->tag != 19)goto _LL4F;}_LL4E:
 goto _LL4A;_LL4F:;_LL50:
 return r;_LL4A:;}{
# 282
union Cyc_Relations_RelnOp eop=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(e,& eop))
return Cyc_Relations_add_relation(rgn,Cyc_Relations_RVar(v),Cyc_Relations_Req,eop,r);
# 286
return r;};}
# 289
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp5D=e1->r;void*_tmp5E=_tmp5D;void*_tmp60;_LL52: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5E;if(_tmp5F->tag != 1)goto _LL54;else{_tmp60=(void*)_tmp5F->f2;}}_LL53: {
# 292
struct Cyc_Absyn_Vardecl*_tmp61=Cyc_Tcutil_nonesc_vardecl(_tmp60);
if(_tmp61 != 0)
return Cyc_Relations_reln_assign_var(rgn,r,_tmp61,e2);else{
return r;}}_LL54:;_LL55:
 return r;_LL51:;}
# 301
static union Cyc_Pratt_Node Cyc_Relations_rop2node(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp62=r;struct Cyc_Absyn_Vardecl*_tmp63;struct Cyc_Absyn_Vardecl*_tmp64;void*_tmp65;unsigned int _tmp66;unsigned int _tmp67;_LL57: if((_tmp62.RVar).tag != 2)goto _LL59;_tmp63=(struct Cyc_Absyn_Vardecl*)(_tmp62.RVar).val;_LL58:
 return Cyc_Pratt_NVar(_tmp63);_LL59: if((_tmp62.RNumelts).tag != 3)goto _LL5B;_tmp64=(struct Cyc_Absyn_Vardecl*)(_tmp62.RNumelts).val;_LL5A:
 return Cyc_Pratt_NNumelts(_tmp64);_LL5B: if((_tmp62.RType).tag != 4)goto _LL5D;_tmp65=(void*)(_tmp62.RType).val;_LL5C:
 return Cyc_Pratt_NType(_tmp65);_LL5D: if((_tmp62.RParam).tag != 5)goto _LL5F;_tmp66=(unsigned int)(_tmp62.RParam).val;_LL5E:
 return Cyc_Pratt_NParam(_tmp66);_LL5F: if((_tmp62.RParamNumelts).tag != 6)goto _LL61;_tmp67=(unsigned int)(_tmp62.RParamNumelts).val;_LL60:
 return Cyc_Pratt_NParamNumelts(_tmp67);_LL61: if((_tmp62.RReturn).tag != 7)goto _LL63;_LL62:
 return Cyc_Pratt_NReturn();_LL63:;_LL64: {
const char*_tmp106;void*_tmp105;(_tmp105=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106="rop2node",_tag_dyneither(_tmp106,sizeof(char),9))),_tag_dyneither(_tmp105,sizeof(void*),0)));}_LL56:;}
# 313
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r){
switch(r){case Cyc_Relations_Req: _LL65: {
const char*_tmp107;return(_tmp107="==",_tag_dyneither(_tmp107,sizeof(char),3));}case Cyc_Relations_Rneq: _LL66: {
const char*_tmp108;return(_tmp108="!=",_tag_dyneither(_tmp108,sizeof(char),3));}case Cyc_Relations_Rlt: _LL67: {
const char*_tmp109;return(_tmp109="<",_tag_dyneither(_tmp109,sizeof(char),2));}case Cyc_Relations_Rlte: _LL68: {
const char*_tmp10A;return(_tmp10A="<=",_tag_dyneither(_tmp10A,sizeof(char),3));}}}
# 322
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp6E=r;unsigned int _tmp6F;struct Cyc_Absyn_Vardecl*_tmp70;void*_tmp71;struct Cyc_Absyn_Vardecl*_tmp72;unsigned int _tmp73;unsigned int _tmp74;_LL6B: if((_tmp6E.RConst).tag != 1)goto _LL6D;_tmp6F=(unsigned int)(_tmp6E.RConst).val;_LL6C: {
const char*_tmp10E;void*_tmp10D[1];struct Cyc_Int_pa_PrintArg_struct _tmp10C;return(struct _dyneither_ptr)((_tmp10C.tag=1,((_tmp10C.f1=(unsigned long)((int)_tmp6F),((_tmp10D[0]=& _tmp10C,Cyc_aprintf(((_tmp10E="%d",_tag_dyneither(_tmp10E,sizeof(char),3))),_tag_dyneither(_tmp10D,sizeof(void*),1))))))));}_LL6D: if((_tmp6E.RVar).tag != 2)goto _LL6F;_tmp70=(struct Cyc_Absyn_Vardecl*)(_tmp6E.RVar).val;_LL6E: {
const char*_tmp112;void*_tmp111[1];struct Cyc_String_pa_PrintArg_struct _tmp110;return(struct _dyneither_ptr)((_tmp110.tag=0,((_tmp110.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp70->name)),((_tmp111[0]=& _tmp110,Cyc_aprintf(((_tmp112="%s",_tag_dyneither(_tmp112,sizeof(char),3))),_tag_dyneither(_tmp111,sizeof(void*),1))))))));}_LL6F: if((_tmp6E.RType).tag != 4)goto _LL71;_tmp71=(void*)(_tmp6E.RType).val;_LL70:
 return Cyc_Absynpp_typ2string(_tmp71);_LL71: if((_tmp6E.RNumelts).tag != 3)goto _LL73;_tmp72=(struct Cyc_Absyn_Vardecl*)(_tmp6E.RNumelts).val;_LL72: {
const char*_tmp116;void*_tmp115[1];struct Cyc_String_pa_PrintArg_struct _tmp114;return(struct _dyneither_ptr)((_tmp114.tag=0,((_tmp114.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp72->name)),((_tmp115[0]=& _tmp114,Cyc_aprintf(((_tmp116="numelts(%s)",_tag_dyneither(_tmp116,sizeof(char),12))),_tag_dyneither(_tmp115,sizeof(void*),1))))))));}_LL73: if((_tmp6E.RParam).tag != 5)goto _LL75;_tmp73=(unsigned int)(_tmp6E.RParam).val;_LL74: {
const char*_tmp11A;void*_tmp119[1];struct Cyc_Int_pa_PrintArg_struct _tmp118;return(struct _dyneither_ptr)((_tmp118.tag=1,((_tmp118.f1=(unsigned long)((int)_tmp73),((_tmp119[0]=& _tmp118,Cyc_aprintf(((_tmp11A="param(%d)",_tag_dyneither(_tmp11A,sizeof(char),10))),_tag_dyneither(_tmp119,sizeof(void*),1))))))));}_LL75: if((_tmp6E.RParamNumelts).tag != 6)goto _LL77;_tmp74=(unsigned int)(_tmp6E.RParamNumelts).val;_LL76: {
const char*_tmp11E;void*_tmp11D[1];struct Cyc_Int_pa_PrintArg_struct _tmp11C;return(struct _dyneither_ptr)((_tmp11C.tag=1,((_tmp11C.f1=(unsigned long)((int)_tmp74),((_tmp11D[0]=& _tmp11C,Cyc_aprintf(((_tmp11E="numelts(param(%d))",_tag_dyneither(_tmp11E,sizeof(char),19))),_tag_dyneither(_tmp11D,sizeof(void*),1))))))));}_LL77: if((_tmp6E.RReturn).tag != 7)goto _LL6A;_LL78: {
const char*_tmp121;void*_tmp120;return(struct _dyneither_ptr)((_tmp120=0,Cyc_aprintf(((_tmp121="return_value",_tag_dyneither(_tmp121,sizeof(char),13))),_tag_dyneither(_tmp120,sizeof(void*),0))));}_LL6A:;}
# 334
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r){
const char*_tmp127;void*_tmp126[3];struct Cyc_String_pa_PrintArg_struct _tmp125;struct Cyc_String_pa_PrintArg_struct _tmp124;struct Cyc_String_pa_PrintArg_struct _tmp123;return(struct _dyneither_ptr)((_tmp123.tag=0,((_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_rop2string(r->rop2)),((_tmp124.tag=0,((_tmp124.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 335
Cyc_Relations_relation2string(r->relation)),((_tmp125.tag=0,((_tmp125.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_rop2string(r->rop1)),((_tmp126[0]=& _tmp125,((_tmp126[1]=& _tmp124,((_tmp126[2]=& _tmp123,Cyc_aprintf(((_tmp127="%s %s %s",_tag_dyneither(_tmp127,sizeof(char),9))),_tag_dyneither(_tmp126,sizeof(void*),3))))))))))))))))))));}
# 339
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*rs){
if(rs == 0){const char*_tmp128;return(_tmp128="true",_tag_dyneither(_tmp128,sizeof(char),5));}{
struct Cyc_List_List*ss=0;
for(0;rs != 0;rs=rs->tl){
{struct _dyneither_ptr*_tmp12B;struct Cyc_List_List*_tmp12A;ss=((_tmp12A=_cycalloc(sizeof(*_tmp12A)),((_tmp12A->hd=((_tmp12B=_cycalloc(sizeof(*_tmp12B)),((_tmp12B[0]=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)rs->hd),_tmp12B)))),((_tmp12A->tl=ss,_tmp12A))))));}
if(rs->tl != 0){struct Cyc_List_List*_tmp12C;ss=((_tmp12C=_cycalloc(sizeof(*_tmp12C)),((_tmp12C->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr))," && ",sizeof(char),5),((_tmp12C->tl=ss,_tmp12C))))));}}
# 346
return(struct _dyneither_ptr)Cyc_strconcat_l(ss);};}
# 349
void Cyc_Relations_print_relns(struct Cyc___cycFILE*f,struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
{const char*_tmp130;void*_tmp12F[1];struct Cyc_String_pa_PrintArg_struct _tmp12E;(_tmp12E.tag=0,((_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)r->hd)),((_tmp12F[0]=& _tmp12E,Cyc_fprintf(f,((_tmp130="%s",_tag_dyneither(_tmp130,sizeof(char),3))),_tag_dyneither(_tmp12F,sizeof(void*),1)))))));}
if(r->tl != 0){const char*_tmp133;void*_tmp132;(_tmp132=0,Cyc_fprintf(f,((_tmp133=",",_tag_dyneither(_tmp133,sizeof(char),2))),_tag_dyneither(_tmp132,sizeof(void*),0)));}}}
# 356
static union Cyc_Relations_RelnOp Cyc_Relations_reduce_relnop(union Cyc_Relations_RelnOp rop){
{union Cyc_Relations_RelnOp _tmp95=rop;void*_tmp96;struct Cyc_Absyn_Vardecl*_tmp97;_LL7A: if((_tmp95.RType).tag != 4)goto _LL7C;_tmp96=(void*)(_tmp95.RType).val;_LL7B:
# 359
{void*_tmp98=Cyc_Tcutil_compress(_tmp96);void*_tmp99=_tmp98;struct Cyc_Absyn_Exp*_tmp9B;_LL81: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp99;if(_tmp9A->tag != 18)goto _LL83;else{_tmp9B=_tmp9A->f1;}}_LL82: {
# 361
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp9B,& rop2))return rop2;
goto _LL80;}_LL83:;_LL84:
 goto _LL80;_LL80:;}
# 366
goto _LL79;_LL7C: if((_tmp95.RNumelts).tag != 3)goto _LL7E;_tmp97=(struct Cyc_Absyn_Vardecl*)(_tmp95.RNumelts).val;_LL7D:
# 368
{void*_tmp9C=Cyc_Tcutil_compress(_tmp97->type);void*_tmp9D=_tmp9C;union Cyc_Absyn_Constraint*_tmp9F;_LL86: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9D;if(_tmp9E->tag != 5)goto _LL88;else{_tmp9F=((_tmp9E->f1).ptr_atts).bounds;}}_LL87:
# 370
{void*_tmpA0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9F);void*_tmpA1=_tmpA0;struct Cyc_Absyn_Exp*_tmpA3;_LL8B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA1;if(_tmpA2->tag != 1)goto _LL8D;else{_tmpA3=_tmpA2->f1;}}_LL8C: {
# 372
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmpA3,& rop2))return rop2;
goto _LL8A;}_LL8D:;_LL8E:
 goto _LL8A;_LL8A:;}
# 377
goto _LL85;_LL88:;_LL89:
 goto _LL85;_LL85:;}
# 380
goto _LL79;_LL7E:;_LL7F:
 goto _LL79;_LL79:;}
# 383
return rop;}
# 388
static int Cyc_Relations_consist_relations(struct Cyc_Pratt_Graph*G,struct Cyc_List_List*rlns){
for(0;(unsigned int)G  && rlns != 0;rlns=rlns->tl){
# 395
struct Cyc_Relations_Reln*_tmpA4=(struct Cyc_Relations_Reln*)rlns->hd;
_tmpA4->rop1=Cyc_Relations_reduce_relnop(_tmpA4->rop1);
_tmpA4->rop2=Cyc_Relations_reduce_relnop(_tmpA4->rop2);{
struct Cyc_Relations_Reln*_tmpA5=_tmpA4;unsigned int _tmpA6;enum Cyc_Relations_Relation _tmpA7;unsigned int _tmpA8;union Cyc_Relations_RelnOp _tmpA9;unsigned int _tmpAA;unsigned int _tmpAB;union Cyc_Relations_RelnOp _tmpAC;union Cyc_Relations_RelnOp _tmpAD;union Cyc_Relations_RelnOp _tmpAE;union Cyc_Relations_RelnOp _tmpAF;enum Cyc_Relations_Relation _tmpB0;unsigned int _tmpB1;unsigned int _tmpB2;enum Cyc_Relations_Relation _tmpB3;union Cyc_Relations_RelnOp _tmpB4;union Cyc_Relations_RelnOp _tmpB5;enum Cyc_Relations_Relation _tmpB6;union Cyc_Relations_RelnOp _tmpB7;_LL90: if(((_tmpA5->rop1).RConst).tag != 1)goto _LL92;_tmpA6=(unsigned int)((_tmpA5->rop1).RConst).val;_tmpA7=_tmpA5->relation;if(((_tmpA5->rop2).RConst).tag != 1)goto _LL92;_tmpA8=(unsigned int)((_tmpA5->rop2).RConst).val;_LL91:
# 400
 switch(_tmpA7){case Cyc_Relations_Req: _LL9E:
 if(_tmpA6 != _tmpA8)return 0;break;case Cyc_Relations_Rneq: _LL9F:
 if(_tmpA6 == _tmpA8)return 0;break;case Cyc_Relations_Rlt: _LLA0:
 if(_tmpA6 >= _tmpA8)return 0;break;case Cyc_Relations_Rlte: _LLA1:
 if(_tmpA6 > _tmpA8)return 0;break;}
# 406
goto _LL8F;_LL92: _tmpA9=_tmpA5->rop1;if(_tmpA5->relation != Cyc_Relations_Rneq)goto _LL94;if(((_tmpA5->rop2).RConst).tag != 1)goto _LL94;_tmpAA=(unsigned int)((_tmpA5->rop2).RConst).val;_LL93:
# 411
 _tmpAB=_tmpAA;_tmpAC=_tmpA9;goto _LL95;_LL94: if(((_tmpA5->rop1).RConst).tag != 1)goto _LL96;_tmpAB=(unsigned int)((_tmpA5->rop1).RConst).val;if(_tmpA5->relation != Cyc_Relations_Rneq)goto _LL96;_tmpAC=_tmpA5->rop2;_LL95: {
# 413
union Cyc_Pratt_Node _tmpB8=Cyc_Relations_rop2node(_tmpAC);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=Cyc_Pratt_add_edge(G2,_tmpB8,Cyc_Pratt_zero_node,(int)(_tmpAB - 1));
G=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmpB8,(int)(-(_tmpAB + 1)));
# 423
if(G != 0  && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl) || 
Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL8F;}_LL96: _tmpAD=_tmpA5->rop1;if(_tmpA5->relation != Cyc_Relations_Rneq)goto _LL98;_tmpAE=_tmpA5->rop2;_LL97: {
# 430
union Cyc_Pratt_Node _tmpB9=Cyc_Relations_rop2node(_tmpAD);
union Cyc_Pratt_Node _tmpBA=Cyc_Relations_rop2node(_tmpAE);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=Cyc_Pratt_add_edge(G2,_tmpB9,_tmpBA,- 1);
G=Cyc_Pratt_add_edge(G,_tmpBA,_tmpB9,- 1);
if(G != 0  && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl) || 
Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL8F;}_LL98: _tmpAF=_tmpA5->rop1;_tmpB0=_tmpA5->relation;if(((_tmpA5->rop2).RConst).tag != 1)goto _LL9A;_tmpB1=(unsigned int)((_tmpA5->rop2).RConst).val;_LL99: {
# 444
union Cyc_Pratt_Node _tmpBB=Cyc_Relations_rop2node(_tmpAF);
if(_tmpB0 == Cyc_Relations_Rlt)_tmpB1=_tmpB1 - 1;
G=Cyc_Pratt_add_edge(G,_tmpBB,Cyc_Pratt_zero_node,(int)_tmpB1);
# 448
if((unsigned int)G  && _tmpB0 == Cyc_Relations_Req)
G=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmpBB,(int)(- _tmpB1));
goto _LL8F;}_LL9A: if(((_tmpA5->rop1).RConst).tag != 1)goto _LL9C;_tmpB2=(unsigned int)((_tmpA5->rop1).RConst).val;_tmpB3=_tmpA5->relation;_tmpB4=_tmpA5->rop2;_LL9B: {
# 457
union Cyc_Pratt_Node _tmpBC=Cyc_Relations_rop2node(_tmpB4);
if(_tmpB3 == Cyc_Relations_Rlt)_tmpB2=_tmpB2 + 1;
G=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmpBC,(int)(- _tmpB2));
# 462
if((unsigned int)G  && _tmpB3 == Cyc_Relations_Req)
# 467
G=Cyc_Pratt_add_edge(G,_tmpBC,Cyc_Pratt_zero_node,(int)_tmpB2);
# 474
goto _LL8F;}_LL9C: _tmpB5=_tmpA5->rop1;_tmpB6=_tmpA5->relation;_tmpB7=_tmpA5->rop2;_LL9D: {
# 477
union Cyc_Pratt_Node _tmpBD=Cyc_Relations_rop2node(_tmpB5);
union Cyc_Pratt_Node _tmpBE=Cyc_Relations_rop2node(_tmpB7);
int i=_tmpB6 == Cyc_Relations_Rlt?- 1: 0;
# 482
G=Cyc_Pratt_add_edge(G,_tmpBD,_tmpBE,i);
# 484
if((unsigned int)G  && _tmpB6 == Cyc_Relations_Req)
G=Cyc_Pratt_add_edge(G,_tmpBE,_tmpBD,i);
goto _LL8F;}_LL8F:;};}
# 489
if(G != 0)return 1;else{return 0;}}
# 497
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns){
# 503
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
return Cyc_Relations_consist_relations(G,rlns);}
# 511
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 513
switch(p){case Cyc_Absyn_Eq: _LLA3: {
struct _tuple10 _tmp134;return(_tmp134.f1=e1,((_tmp134.f2=Cyc_Relations_Req,((_tmp134.f3=e2,_tmp134)))));}case Cyc_Absyn_Neq: _LLA4: {
struct _tuple10 _tmp135;return(_tmp135.f1=e1,((_tmp135.f2=Cyc_Relations_Rneq,((_tmp135.f3=e2,_tmp135)))));}case Cyc_Absyn_Lt: _LLA5: {
struct _tuple10 _tmp136;return(_tmp136.f1=e1,((_tmp136.f2=Cyc_Relations_Rlt,((_tmp136.f3=e2,_tmp136)))));}case Cyc_Absyn_Lte: _LLA6: {
struct _tuple10 _tmp137;return(_tmp137.f1=e1,((_tmp137.f2=Cyc_Relations_Rlte,((_tmp137.f3=e2,_tmp137)))));}case Cyc_Absyn_Gt: _LLA7: {
struct _tuple10 _tmp138;return(_tmp138.f1=e2,((_tmp138.f2=Cyc_Relations_Rlt,((_tmp138.f3=e1,_tmp138)))));}case Cyc_Absyn_Gte: _LLA8: {
struct _tuple10 _tmp139;return(_tmp139.f1=e2,((_tmp139.f2=Cyc_Relations_Rlte,((_tmp139.f3=e1,_tmp139)))));}default: _LLA9: {
struct Cyc_Core_Impossible_exn_struct _tmp13F;const char*_tmp13E;struct Cyc_Core_Impossible_exn_struct*_tmp13D;(int)_throw((void*)((_tmp13D=_cycalloc(sizeof(*_tmp13D)),((_tmp13D[0]=((_tmp13F.tag=Cyc_Core_Impossible,((_tmp13F.f1=((_tmp13E="primop2relation",_tag_dyneither(_tmp13E,sizeof(char),16))),_tmp13F)))),_tmp13D)))));}}}
# 525
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r){
switch(r){case Cyc_Relations_Req: _LLAB:
 return Cyc_Relations_Rneq;case Cyc_Relations_Rneq: _LLAC:
 return Cyc_Relations_Req;case Cyc_Relations_Rlt: _LLAD:
 return Cyc_Relations_Rlte;case Cyc_Relations_Rlte: _LLAE:
 return Cyc_Relations_Rlt;}}
# 534
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*r,struct Cyc_Relations_Reln*rln){
struct Cyc_Relations_Reln*_tmp140;return(_tmp140=_region_malloc(r,sizeof(*_tmp140)),((_tmp140->rop1=rln->rop2,((_tmp140->relation=Cyc_Relations_flip_relation(rln->relation),((_tmp140->rop2=rln->rop1,_tmp140)))))));}
# 545
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 547
{void*_tmpC9=e->r;void*_tmpCA=_tmpC9;void*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;enum Cyc_Absyn_Primop _tmpD3;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;enum Cyc_Absyn_Primop _tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;_LLB1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCA;if(_tmpCB->tag != 13)goto _LLB3;else{_tmpCC=(void*)_tmpCB->f1;_tmpCD=_tmpCB->f2;}}_LLB2:
# 550
{void*_tmpDA=Cyc_Tcutil_compress(_tmpCC);void*_tmpDB=_tmpDA;_LLBC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDB;if(_tmpDC->tag != 6)goto _LLBE;else{if(_tmpDC->f2 != Cyc_Absyn_Int_sz)goto _LLBE;}}_LLBD:
 return Cyc_Relations_exp2relns(r,_tmpCD);_LLBE:;_LLBF:
 goto _LLBB;_LLBB:;}
# 554
goto _LLB0;_LLB3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpCE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCA;if(_tmpCE->tag != 6)goto _LLB5;else{_tmpCF=_tmpCE->f1;_tmpD0=_tmpCE->f2;}}_LLB4:
 return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,Cyc_Relations_exp2relns(r,_tmpCF),Cyc_Relations_exp2relns(r,_tmpD0));_LLB5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD1=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA;if(_tmpD1->tag != 2)goto _LLB7;else{if(_tmpD1->f1 != Cyc_Absyn_Not)goto _LLB7;if(_tmpD1->f2 == 0)goto _LLB7;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD2=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(_tmpD1->f2)->hd)->r;if(_tmpD2->tag != 2)goto _LLB7;else{_tmpD3=_tmpD2->f1;if(_tmpD2->f2 == 0)goto _LLB7;_tmpD4=(struct Cyc_Absyn_Exp*)(_tmpD2->f2)->hd;if((_tmpD2->f2)->tl == 0)goto _LLB7;_tmpD5=(struct Cyc_Absyn_Exp*)((_tmpD2->f2)->tl)->hd;if(((_tmpD2->f2)->tl)->tl != 0)goto _LLB7;}}if((_tmpD1->f2)->tl != 0)goto _LLB7;}}_LLB6:
# 557
 switch(_tmpD3){case Cyc_Absyn_Eq: _LLC0:
 goto _LLC1;case Cyc_Absyn_Neq: _LLC1: goto _LLC2;case Cyc_Absyn_Lt: _LLC2: goto _LLC3;case Cyc_Absyn_Lte: _LLC3: goto _LLC4;case Cyc_Absyn_Gt: _LLC4: goto _LLC5;case Cyc_Absyn_Gte: _LLC5: {
struct _tuple10 _tmpDD=Cyc_Relations_primop2relation(_tmpD4,_tmpD3,_tmpD5);struct Cyc_Absyn_Exp*_tmpDF;enum Cyc_Relations_Relation _tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct _tuple10 _tmpDE=_tmpDD;_tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;_tmpE1=_tmpDE.f3;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmpDF,& rop1) && Cyc_Relations_exp2relnop(_tmpE1,& rop2)){
struct Cyc_Relations_Reln*_tmp143;struct Cyc_List_List*_tmp142;return(_tmp142=_region_malloc(r,sizeof(*_tmp142)),((_tmp142->hd=((_tmp143=_region_malloc(r,sizeof(*_tmp143)),((_tmp143->rop1=rop2,((_tmp143->relation=Cyc_Relations_flip_relation(_tmpE0),((_tmp143->rop2=rop1,_tmp143)))))))),((_tmp142->tl=0,_tmp142)))));}
break;};}default: _LLC6:
 break;}
# 567
goto _LLB0;_LLB7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA;if(_tmpD6->tag != 2)goto _LLB9;else{_tmpD7=_tmpD6->f1;if(_tmpD6->f2 == 0)goto _LLB9;_tmpD8=(struct Cyc_Absyn_Exp*)(_tmpD6->f2)->hd;if((_tmpD6->f2)->tl == 0)goto _LLB9;_tmpD9=(struct Cyc_Absyn_Exp*)((_tmpD6->f2)->tl)->hd;if(((_tmpD6->f2)->tl)->tl != 0)goto _LLB9;}}_LLB8:
# 569
 switch(_tmpD7){case Cyc_Absyn_Eq: _LLC8:
 goto _LLC9;case Cyc_Absyn_Neq: _LLC9: goto _LLCA;case Cyc_Absyn_Lt: _LLCA: goto _LLCB;case Cyc_Absyn_Lte: _LLCB: goto _LLCC;case Cyc_Absyn_Gt: _LLCC: goto _LLCD;case Cyc_Absyn_Gte: _LLCD: {
struct _tuple10 _tmpE4=Cyc_Relations_primop2relation(_tmpD8,_tmpD7,_tmpD9);struct Cyc_Absyn_Exp*_tmpE6;enum Cyc_Relations_Relation _tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct _tuple10 _tmpE5=_tmpE4;_tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;_tmpE8=_tmpE5.f3;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmpE6,& rop1) && Cyc_Relations_exp2relnop(_tmpE8,& rop2)){
struct Cyc_Relations_Reln*_tmp146;struct Cyc_List_List*_tmp145;return(_tmp145=_region_malloc(r,sizeof(*_tmp145)),((_tmp145->hd=((_tmp146=_region_malloc(r,sizeof(*_tmp146)),((_tmp146->rop1=rop1,((_tmp146->relation=_tmpE7,((_tmp146->rop2=rop2,_tmp146)))))))),((_tmp145->tl=0,_tmp145)))));}
break;};}default: _LLCE:
 break;}
# 579
goto _LLB0;_LLB9:;_LLBA:
 goto _LLB0;_LLB0:;}
# 583
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmpEB=Cyc_Evexp_eval_const_uint_exp(e);unsigned int _tmpED;int _tmpEE;struct _tuple11 _tmpEC=_tmpEB;_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;
if(_tmpEE){
if((int)_tmpED)return 0;else{
# 588
struct Cyc_Relations_Reln*_tmp149;struct Cyc_List_List*_tmp148;return(_tmp148=_region_malloc(r,sizeof(*_tmp148)),((_tmp148->hd=((_tmp149=_region_malloc(r,sizeof(*_tmp149)),((_tmp149->rop1=Cyc_Relations_RConst(0),((_tmp149->relation=Cyc_Relations_Rlt,((_tmp149->rop2=Cyc_Relations_RConst(0),_tmp149)))))))),((_tmp148->tl=0,_tmp148)))));}}{
# 590
const char*_tmp14D;void*_tmp14C[1];struct Cyc_String_pa_PrintArg_struct _tmp14B;(_tmp14B.tag=0,((_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp14C[0]=& _tmp14B,Cyc_Tcutil_terr(e->loc,((_tmp14D="unable to convert `%s' to static relation",_tag_dyneither(_tmp14D,sizeof(char),42))),_tag_dyneither(_tmp14C,sizeof(void*),1)))))));};}{
# 594
struct Cyc_Relations_Reln*_tmp150;struct Cyc_List_List*_tmp14F;return(_tmp14F=_region_malloc(r,sizeof(*_tmp14F)),((_tmp14F->hd=((_tmp150=_region_malloc(r,sizeof(*_tmp150)),((_tmp150->rop1=Cyc_Relations_RConst(0),((_tmp150->relation=Cyc_Relations_Rlt,((_tmp150->rop2=Cyc_Relations_RConst(0),_tmp150)))))))),((_tmp14F->tl=0,_tmp14F)))));};}
