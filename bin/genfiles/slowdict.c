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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};
# 35 "splay.h"
typedef int Cyc_Splay_dummy_t;struct Cyc_Splay_Leaf_Splay_tree_struct{int tag;int f1;};struct Cyc_Splay_Node_Splay_tree_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*left;void*right;};
# 49
int Cyc_Splay_splay(int(*f)(void*,void*),void*,void*);struct Cyc_SlowDict_Dict;
# 40 "slowdict.h"
typedef struct Cyc_SlowDict_Dict*Cyc_SlowDict_dict_t;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};
# 49
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*cmp)(void*,void*));
# 56
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d);
# 60
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*k);
# 64
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*k,void*v);
# 69
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*k,void*v);
# 73
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*l);
# 77
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 81
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*k);
# 84
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*k);
# 89
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*k);
# 94
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*k);
# 98
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum);
# 102
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d,void*accum);
# 106
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d);
# 110
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);
# 114
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d);
# 116
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);
# 120
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d);
# 125
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d);struct _tuple0{void*f1;void*f2;};
# 129
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d);
# 134
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d);char Cyc_SlowDict_Absent[7]="Absent";char Cyc_SlowDict_Present[8]="Present";
# 32 "slowdict.cyc"
struct Cyc_SlowDict_Absent_exn_struct Cyc_SlowDict_Absent_val={Cyc_SlowDict_Absent};
struct Cyc_SlowDict_Present_exn_struct Cyc_SlowDict_Present_val={Cyc_SlowDict_Present};struct Cyc_SlowDict_Dict{int(*reln)(void*,void*);void*tree;};
# 41
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*comp)(void*,void*)){
struct Cyc_Splay_Leaf_Splay_tree_struct _tmp8F;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp8E;void*t=(void*)((_tmp8E=_cycalloc_atomic(sizeof(*_tmp8E)),((_tmp8E[0]=((_tmp8F.tag=0,((_tmp8F.f1=0,_tmp8F)))),_tmp8E))));
struct Cyc_SlowDict_Dict*_tmp90;return(_tmp90=_cycalloc(sizeof(*_tmp90)),((_tmp90->reln=comp,((_tmp90->tree=t,_tmp90)))));}
# 49
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d){
void*_tmp5=d->tree;void*_tmp6=_tmp5;_LL1: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp7=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp6;if(_tmp7->tag != 0)goto _LL3;}_LL2:
 return 1;_LL3: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp8=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp6;if(_tmp8->tag != 1)goto _LL0;}_LL4:
 return 0;_LL0:;}
# 56
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*key){
return Cyc_Splay_splay(d->reln,key,d->tree);}
# 60
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,void*data){
struct Cyc_Splay_Leaf_Splay_tree_struct _tmp93;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp92;void*leaf=(void*)((_tmp92=_cycalloc_atomic(sizeof(*_tmp92)),((_tmp92[0]=((_tmp93.tag=0,((_tmp93.f1=0,_tmp93)))),_tmp92))));
void*newleft=leaf;void*newright=leaf;
if(Cyc_Splay_splay(d->reln,key,d->tree)){
# 65
void*_tmp9=d->tree;void*_tmpA=_tmp9;struct Cyc_Splay_noderef*_tmpC;_LL6: {struct Cyc_Splay_Node_Splay_tree_struct*_tmpB=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmpA;if(_tmpB->tag != 1)goto _LL8;else{_tmpC=_tmpB->f1;}}_LL7:
# 67
 newleft=(_tmpC->v)->left;
newright=(_tmpC->v)->right;
goto _LL5;_LL8:;_LL9:
# 71
 goto _LL5;_LL5:;}else{
# 75
void*_tmpD=d->tree;void*_tmpE=_tmpD;struct Cyc_Splay_noderef*_tmp10;_LLB: {struct Cyc_Splay_Node_Splay_tree_struct*_tmpF=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmpE;if(_tmpF->tag != 1)goto _LLD;else{_tmp10=_tmpF->f1;}}_LLC: {
# 77
struct Cyc_Splay_node*_tmp12=_tmp10->v;
if((d->reln)(key,_tmp12->key)< 0){
newleft=_tmp12->left;{
struct Cyc_Splay_Node_Splay_tree_struct _tmp9D;struct Cyc_Splay_node*_tmp9C;struct Cyc_Splay_noderef*_tmp9B;struct Cyc_Splay_Node_Splay_tree_struct*_tmp9A;newright=(void*)((_tmp9A=_cycalloc(sizeof(*_tmp9A)),((_tmp9A[0]=((_tmp9D.tag=1,((_tmp9D.f1=((_tmp9B=_cycalloc(sizeof(*_tmp9B)),((_tmp9B->v=((_tmp9C=_cycalloc(sizeof(*_tmp9C)),((_tmp9C->key=_tmp12->key,((_tmp9C->data=_tmp12->data,((_tmp9C->left=leaf,((_tmp9C->right=_tmp12->right,_tmp9C)))))))))),_tmp9B)))),_tmp9D)))),_tmp9A))));};}else{
# 84
{struct Cyc_Splay_Node_Splay_tree_struct _tmpA7;struct Cyc_Splay_node*_tmpA6;struct Cyc_Splay_noderef*_tmpA5;struct Cyc_Splay_Node_Splay_tree_struct*_tmpA4;newleft=(void*)((_tmpA4=_cycalloc(sizeof(*_tmpA4)),((_tmpA4[0]=((_tmpA7.tag=1,((_tmpA7.f1=((_tmpA5=_cycalloc(sizeof(*_tmpA5)),((_tmpA5->v=((_tmpA6=_cycalloc(sizeof(*_tmpA6)),((_tmpA6->key=_tmp12->key,((_tmpA6->data=_tmp12->data,((_tmpA6->left=_tmp12->left,((_tmpA6->right=leaf,_tmpA6)))))))))),_tmpA5)))),_tmpA7)))),_tmpA4))));}
# 86
newright=_tmp12->right;}
# 88
goto _LLA;}_LLD: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp11=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmpE;if(_tmp11->tag != 0)goto _LLA;}_LLE:
# 90
 goto _LLA;_LLA:;}{
# 93
struct Cyc_Splay_Node_Splay_tree_struct*_tmpB6;struct Cyc_Splay_noderef*_tmpB5;struct Cyc_Splay_node*_tmpB4;struct Cyc_Splay_Node_Splay_tree_struct _tmpB3;struct Cyc_SlowDict_Dict*_tmpB2;return(_tmpB2=_cycalloc(sizeof(*_tmpB2)),((_tmpB2->reln=d->reln,((_tmpB2->tree=(void*)((_tmpB6=_cycalloc(sizeof(*_tmpB6)),((_tmpB6[0]=((_tmpB3.tag=1,((_tmpB3.f1=((_tmpB5=_cycalloc(sizeof(*_tmpB5)),((_tmpB5->v=(
(_tmpB4=_cycalloc(sizeof(*_tmpB4)),((_tmpB4->key=key,((_tmpB4->data=data,((_tmpB4->left=newleft,((_tmpB4->right=newright,_tmpB4)))))))))),_tmpB5)))),_tmpB3)))),_tmpB6)))),_tmpB2)))));};}
# 98
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*key,void*data){
# 100
if(Cyc_Splay_splay(d->reln,key,d->tree))
(int)_throw((void*)& Cyc_SlowDict_Present_val);
return Cyc_SlowDict_insert(d,key,data);}
# 105
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*kds){
# 107
for(0;kds != 0;kds=kds->tl){
d=Cyc_SlowDict_insert(d,(*((struct _tuple0*)kds->hd)).f1,(*((struct _tuple0*)kds->hd)).f2);}
return d;}
# 112
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){
struct Cyc_Splay_Leaf_Splay_tree_struct _tmpB9;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmpB8;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp22=(_tmpB8=_cycalloc_atomic(sizeof(*_tmpB8)),((_tmpB8[0]=((_tmpB9.tag=0,((_tmpB9.f1=0,_tmpB9)))),_tmpB8)));
struct Cyc_Splay_Node_Splay_tree_struct*_tmpC8;struct Cyc_Splay_noderef*_tmpC7;struct Cyc_Splay_node*_tmpC6;struct Cyc_Splay_Node_Splay_tree_struct _tmpC5;struct Cyc_SlowDict_Dict*_tmpC4;return(_tmpC4=_cycalloc(sizeof(*_tmpC4)),((_tmpC4->reln=comp,((_tmpC4->tree=(void*)((_tmpC8=_cycalloc(sizeof(*_tmpC8)),((_tmpC8[0]=((_tmpC5.tag=1,((_tmpC5.f1=((_tmpC7=_cycalloc(sizeof(*_tmpC7)),((_tmpC7->v=((_tmpC6=_cycalloc(sizeof(*_tmpC6)),((_tmpC6->key=key,((_tmpC6->data=data,((_tmpC6->left=(void*)_tmp22,((_tmpC6->right=(void*)_tmp22,_tmpC6)))))))))),_tmpC7)))),_tmpC5)))),_tmpC8)))),_tmpC4)))));}
# 117
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp2A=d->tree;void*_tmp2B=_tmp2A;struct Cyc_Splay_noderef*_tmp2D;_LL10: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp2C=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp2B;if(_tmp2C->tag != 1)goto _LL12;else{_tmp2D=_tmp2C->f1;}}_LL11:
 return(_tmp2D->v)->data;_LL12: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp2E=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp2B;if(_tmp2E->tag != 0)goto _LLF;}_LL13: {
struct Cyc_Core_Impossible_exn_struct _tmpCE;const char*_tmpCD;struct Cyc_Core_Impossible_exn_struct*_tmpCC;(int)_throw((void*)((_tmpCC=_cycalloc(sizeof(*_tmpCC)),((_tmpCC[0]=((_tmpCE.tag=Cyc_Core_Impossible,((_tmpCE.f1=((_tmpCD="Dict::lookup",_tag_dyneither(_tmpCD,sizeof(char),13))),_tmpCE)))),_tmpCC)))));}_LLF:;}
# 123
(int)_throw((void*)& Cyc_SlowDict_Absent_val);}
# 126
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp32=d->tree;void*_tmp33=_tmp32;struct Cyc_Splay_noderef*_tmp35;_LL15: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp34=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp33;if(_tmp34->tag != 1)goto _LL17;else{_tmp35=_tmp34->f1;}}_LL16: {
struct Cyc_Core_Opt*_tmpCF;return(_tmpCF=_cycalloc(sizeof(*_tmpCF)),((_tmpCF->v=(_tmp35->v)->data,_tmpCF)));}_LL17: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp36=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp33;if(_tmp36->tag != 0)goto _LL14;}_LL18: {
struct Cyc_Core_Impossible_exn_struct _tmpD5;const char*_tmpD4;struct Cyc_Core_Impossible_exn_struct*_tmpD3;(int)_throw((void*)((_tmpD3=_cycalloc(sizeof(*_tmpD3)),((_tmpD3[0]=((_tmpD5.tag=Cyc_Core_Impossible,((_tmpD5.f1=((_tmpD4="Dict::lookup",_tag_dyneither(_tmpD4,sizeof(char),13))),_tmpD5)))),_tmpD3)))));}_LL14:;}
# 132
return 0;}
# 135
static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}
# 137
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp3B=d->tree;void*_tmp3C=_tmp3B;struct Cyc_Splay_noderef*_tmp3F;_LL1A: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp3D=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp3C;if(_tmp3D->tag != 0)goto _LL1C;}_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpDB;const char*_tmpDA;struct Cyc_Core_Impossible_exn_struct*_tmpD9;(int)_throw((void*)((_tmpD9=_cycalloc(sizeof(*_tmpD9)),((_tmpD9[0]=((_tmpDB.tag=Cyc_Core_Impossible,((_tmpDB.f1=((_tmpDA="Dict::lookup",_tag_dyneither(_tmpDA,sizeof(char),13))),_tmpDB)))),_tmpD9)))));}_LL1C: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp3E=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp3C;if(_tmp3E->tag != 1)goto _LL19;else{_tmp3F=_tmp3E->f1;}}_LL1D: {
# 142
struct Cyc_Splay_node*n=_tmp3F->v;
void*_tmp43=n->left;void*_tmp44=_tmp43;struct Cyc_Splay_noderef*_tmp47;_LL1F: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp45=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp44;if(_tmp45->tag != 0)goto _LL21;}_LL20: {
struct Cyc_SlowDict_Dict*_tmpDC;return(_tmpDC=_cycalloc(sizeof(*_tmpDC)),((_tmpDC->reln=d->reln,((_tmpDC->tree=n->right,_tmpDC)))));}_LL21: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp46=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp44;if(_tmp46->tag != 1)goto _LL1E;else{_tmp47=_tmp46->f1;}}_LL22: {
# 146
void*_tmp49=n->right;void*_tmp4A=_tmp49;struct Cyc_Splay_noderef*_tmp4D;_LL24: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp4B=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp4A;if(_tmp4B->tag != 0)goto _LL26;}_LL25: {
struct Cyc_SlowDict_Dict*_tmpDD;return(_tmpDD=_cycalloc(sizeof(*_tmpDD)),((_tmpDD->reln=d->reln,((_tmpDD->tree=n->left,_tmpDD)))));}_LL26: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp4C=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp4A;if(_tmp4C->tag != 1)goto _LL23;else{_tmp4D=_tmp4C->f1;}}_LL27:
# 149
 Cyc_Splay_splay(Cyc_SlowDict_get_largest,key,n->left);{
struct Cyc_Splay_node*newtop=_tmp47->v;
struct Cyc_Splay_Node_Splay_tree_struct*_tmpEC;struct Cyc_Splay_noderef*_tmpEB;struct Cyc_Splay_node*_tmpEA;struct Cyc_Splay_Node_Splay_tree_struct _tmpE9;struct Cyc_SlowDict_Dict*_tmpE8;return(_tmpE8=_cycalloc(sizeof(*_tmpE8)),((_tmpE8->reln=d->reln,((_tmpE8->tree=(void*)(
(_tmpEC=_cycalloc(sizeof(*_tmpEC)),((_tmpEC[0]=((_tmpE9.tag=1,((_tmpE9.f1=((_tmpEB=_cycalloc(sizeof(*_tmpEB)),((_tmpEB->v=((_tmpEA=_cycalloc(sizeof(*_tmpEA)),((_tmpEA->key=newtop->key,((_tmpEA->data=newtop->data,((_tmpEA->left=newtop->left,((_tmpEA->right=n->right,_tmpEA)))))))))),_tmpEB)))),_tmpE9)))),_tmpEC)))),_tmpE8)))));};_LL23:;}_LL1E:;}_LL19:;}else{
# 158
return d;}}
# 161
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*key){
struct Cyc_SlowDict_Dict*_tmp54=Cyc_SlowDict_delete(d,key);
if(d == _tmp54)
(int)_throw((void*)& Cyc_SlowDict_Absent_val);
return _tmp54;}
# 168
static void*Cyc_SlowDict_fold_tree(void*(*f)(void*,void*,void*),void*t,void*accum){
void*_tmp55=t;struct Cyc_Splay_noderef*_tmp58;_LL29: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp56=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp55;if(_tmp56->tag != 0)goto _LL2B;}_LL2A:
# 171
 return accum;_LL2B: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp57=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp55;if(_tmp57->tag != 1)goto _LL28;else{_tmp58=_tmp57->f1;}}_LL2C: {
# 173
struct Cyc_Splay_node*n=_tmp58->v;
return f(n->key,n->data,Cyc_SlowDict_fold_tree(f,n->left,
Cyc_SlowDict_fold_tree(f,n->right,accum)));}_LL28:;}
# 178
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum){
return Cyc_SlowDict_fold_tree(f,d->tree,accum);}
# 182
static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,void*t,void*accum){
void*_tmp59=t;struct Cyc_Splay_noderef*_tmp5C;_LL2E: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp5A=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp59;if(_tmp5A->tag != 0)goto _LL30;}_LL2F:
# 185
 return accum;_LL30: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp5B=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp59;if(_tmp5B->tag != 1)goto _LL2D;else{_tmp5C=_tmp5B->f1;}}_LL31: {
# 187
struct Cyc_Splay_node*n=_tmp5C->v;
return f(env,n->key,n->data,
Cyc_SlowDict_fold_tree_c(f,env,n->left,Cyc_SlowDict_fold_tree_c(f,env,n->right,accum)));}_LL2D:;}
# 192
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){
return Cyc_SlowDict_fold_tree_c(f,env,dict->tree,accum);}
# 196
static void Cyc_SlowDict_app_tree(void*(*f)(void*,void*),void*t){
void*_tmp5D=t;struct Cyc_Splay_noderef*_tmp60;_LL33: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp5E=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp5D;if(_tmp5E->tag != 0)goto _LL35;}_LL34:
 goto _LL32;_LL35: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp5F=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp5D;if(_tmp5F->tag != 1)goto _LL32;else{_tmp60=_tmp5F->f1;}}_LL36: {
# 200
struct Cyc_Splay_node*_tmp61=_tmp60->v;
Cyc_SlowDict_app_tree(f,_tmp61->left);
f(_tmp61->key,_tmp61->data);
Cyc_SlowDict_app_tree(f,_tmp61->right);
goto _LL32;}_LL32:;}
# 207
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree(f,d->tree);}
# 211
static void Cyc_SlowDict_iter_tree(void(*f)(void*,void*),void*t){
void*_tmp62=t;struct Cyc_Splay_noderef*_tmp65;_LL38: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp63=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp62;if(_tmp63->tag != 0)goto _LL3A;}_LL39:
 goto _LL37;_LL3A: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp64=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp62;if(_tmp64->tag != 1)goto _LL37;else{_tmp65=_tmp64->f1;}}_LL3B: {
# 215
struct Cyc_Splay_node*n=_tmp65->v;
Cyc_SlowDict_iter_tree(f,n->left);
f(n->key,n->data);
Cyc_SlowDict_iter_tree(f,n->right);
goto _LL37;}_LL37:;}
# 222
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree(f,d->tree);}
# 226
static void Cyc_SlowDict_app_tree_c(void*(*f)(void*,void*,void*),void*env,void*t){
void*_tmp66=t;struct Cyc_Splay_noderef*_tmp69;_LL3D: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp67=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp66;if(_tmp67->tag != 0)goto _LL3F;}_LL3E:
 goto _LL3C;_LL3F: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp68=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp66;if(_tmp68->tag != 1)goto _LL3C;else{_tmp69=_tmp68->f1;}}_LL40: {
# 230
struct Cyc_Splay_node*n=_tmp69->v;
Cyc_SlowDict_app_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_app_tree_c(f,env,n->right);
goto _LL3C;}_LL3C:;}
# 237
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree_c(f,env,d->tree);}
# 241
static void Cyc_SlowDict_iter_tree_c(void(*f)(void*,void*,void*),void*env,void*t){
void*_tmp6A=t;struct Cyc_Splay_noderef*_tmp6D;_LL42: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp6B=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp6A;if(_tmp6B->tag != 0)goto _LL44;}_LL43:
 goto _LL41;_LL44: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp6C=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp6A;if(_tmp6C->tag != 1)goto _LL41;else{_tmp6D=_tmp6C->f1;}}_LL45: {
# 245
struct Cyc_Splay_node*n=_tmp6D->v;
Cyc_SlowDict_iter_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_iter_tree_c(f,env,n->right);
goto _LL41;}_LL41:;}
# 252
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree_c(f,env,d->tree);}
# 256
static void*Cyc_SlowDict_map_tree(void*(*f)(void*),void*t){
void*_tmp6E=t;struct Cyc_Splay_noderef*_tmp71;_LL47: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp6F=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp6E;if(_tmp6F->tag != 0)goto _LL49;}_LL48: {
struct Cyc_Splay_Leaf_Splay_tree_struct _tmpEF;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmpEE;return(void*)((_tmpEE=_cycalloc_atomic(sizeof(*_tmpEE)),((_tmpEE[0]=((_tmpEF.tag=0,((_tmpEF.f1=0,_tmpEF)))),_tmpEE))));}_LL49: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp70=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp6E;if(_tmp70->tag != 1)goto _LL46;else{_tmp71=_tmp70->f1;}}_LL4A: {
# 260
struct Cyc_Splay_node*_tmp74=_tmp71->v;
struct Cyc_Splay_Node_Splay_tree_struct _tmpF9;struct Cyc_Splay_node*_tmpF8;struct Cyc_Splay_noderef*_tmpF7;struct Cyc_Splay_Node_Splay_tree_struct*_tmpF6;return(void*)((_tmpF6=_cycalloc(sizeof(*_tmpF6)),((_tmpF6[0]=((_tmpF9.tag=1,((_tmpF9.f1=((_tmpF7=_cycalloc(sizeof(*_tmpF7)),((_tmpF7->v=((_tmpF8=_cycalloc(sizeof(*_tmpF8)),((_tmpF8->key=_tmp74->key,((_tmpF8->data=
f(_tmp74->data),((_tmpF8->left=
Cyc_SlowDict_map_tree(f,_tmp74->left),((_tmpF8->right=
Cyc_SlowDict_map_tree(f,_tmp74->right),_tmpF8)))))))))),_tmpF7)))),_tmpF9)))),_tmpF6))));}_LL46:;}
# 267
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d){
struct Cyc_SlowDict_Dict*_tmpFA;return(_tmpFA=_cycalloc(sizeof(*_tmpFA)),((_tmpFA->reln=d->reln,((_tmpFA->tree=Cyc_SlowDict_map_tree(f,d->tree),_tmpFA)))));}
# 271
static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,void*),void*env,void*t){
void*_tmp7A=t;struct Cyc_Splay_noderef*_tmp7D;_LL4C: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp7B=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp7A;if(_tmp7B->tag != 0)goto _LL4E;}_LL4D: {
struct Cyc_Splay_Leaf_Splay_tree_struct _tmpFD;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmpFC;return(void*)((_tmpFC=_cycalloc_atomic(sizeof(*_tmpFC)),((_tmpFC[0]=((_tmpFD.tag=0,((_tmpFD.f1=0,_tmpFD)))),_tmpFC))));}_LL4E: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp7C=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp7A;if(_tmp7C->tag != 1)goto _LL4B;else{_tmp7D=_tmp7C->f1;}}_LL4F: {
# 275
struct Cyc_Splay_node*n=_tmp7D->v;
struct Cyc_Splay_Node_Splay_tree_struct _tmp107;struct Cyc_Splay_node*_tmp106;struct Cyc_Splay_noderef*_tmp105;struct Cyc_Splay_Node_Splay_tree_struct*_tmp104;return(void*)((_tmp104=_cycalloc(sizeof(*_tmp104)),((_tmp104[0]=((_tmp107.tag=1,((_tmp107.f1=((_tmp105=_cycalloc(sizeof(*_tmp105)),((_tmp105->v=((_tmp106=_cycalloc(sizeof(*_tmp106)),((_tmp106->key=n->key,((_tmp106->data=f(env,n->data),((_tmp106->left=
Cyc_SlowDict_map_tree_c(f,env,n->left),((_tmp106->right=
Cyc_SlowDict_map_tree_c(f,env,n->right),_tmp106)))))))))),_tmp105)))),_tmp107)))),_tmp104))));}_LL4B:;}
# 281
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
struct Cyc_SlowDict_Dict*_tmp108;return(_tmp108=_cycalloc(sizeof(*_tmp108)),((_tmp108->reln=d->reln,((_tmp108->tree=Cyc_SlowDict_map_tree_c(f,env,d->tree),_tmp108)))));}
# 285
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){
void*_tmp85=d->tree;void*_tmp86=_tmp85;struct Cyc_Splay_noderef*_tmp89;_LL51: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp87=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp86;if(_tmp87->tag != 0)goto _LL53;}_LL52:
(int)_throw((void*)& Cyc_SlowDict_Absent_val);_LL53: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp88=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp86;if(_tmp88->tag != 1)goto _LL50;else{_tmp89=_tmp88->f1;}}_LL54: {
struct _tuple0*_tmp109;return(_tmp109=_cycalloc(sizeof(*_tmp109)),((_tmp109->f1=(_tmp89->v)->key,((_tmp109->f2=(_tmp89->v)->data,_tmp109)))));}_LL50:;}
# 292
struct Cyc_List_List*Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*accum){
struct _tuple0*_tmp10C;struct Cyc_List_List*_tmp10B;return(_tmp10B=_cycalloc(sizeof(*_tmp10B)),((_tmp10B->hd=((_tmp10C=_cycalloc(sizeof(*_tmp10C)),((_tmp10C->f1=k,((_tmp10C->f2=v,_tmp10C)))))),((_tmp10B->tl=accum,_tmp10B)))));}
# 296
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*accum))Cyc_SlowDict_fold)(Cyc_SlowDict_to_list_f,d,0);}
