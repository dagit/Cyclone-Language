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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
# 70 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 76
typedef void*Cyc_Tcpat_decision_opt_t;
# 84
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 100
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 108
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
# 159
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 180
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 185
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 223
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 919 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 927
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 933
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 936
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 939
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 947
void*Cyc_Absyn_compress_kb(void*);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 976
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 998
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1009
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1081
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1169
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 65
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct _tuple10{void*f1;void*f2;};
# 232 "dict.h"
struct _tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 255
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 85
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 102 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 113
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 117
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 120
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 193
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 101
int Cyc_Tcutil_is_function_type(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 198
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 225 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 227
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 235
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 239
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 249
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 281
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 291
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 311
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 322
int Cyc_Tcutil_bits_only(void*t);
# 325
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 335
int Cyc_Tcutil_supports_default(void*);
# 346
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 350
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 33 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 35
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 39
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 42
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 66 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 85
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 122
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1]="";
# 46 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 51
int Cyc_Tc_aggressive_warn=0;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 53
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 59
(*_tmp1).f3=1;
return 1;}}
# 63
return 0;}
# 66
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));({void*_tmp2[1]={& _tmp4};Cyc_Tcutil_terr(loc,({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29);}),_tag_dyneither(_tmp2,sizeof(void*),1));});});
return 0;}
# 71
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 73
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 76
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_Int_pa_PrintArg_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(unsigned long)i;({struct Cyc_String_pa_PrintArg_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);({void*_tmp5[3]={& _tmp7,& _tmp8,& _tmp9};Cyc_Tcutil_terr(loc,({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54);}),_tag_dyneither(_tmp5,sizeof(void*),3));});});});});}
# 80
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmpA=_new_region("temp");struct _RegionHandle*temp=& _tmpA;_push_region(temp);
{struct _tuple13 init_params=({struct _tuple13 _tmp3B;_tmp3B.f1=0;_tmp3B.f2=({const char*_tmp3C="initializes";_tag_dyneither(_tmp3C,sizeof(char),12);});_tmp3B;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp39;_tmp39.f1=0;_tmp39.f2=({const char*_tmp3A="noliveunique";_tag_dyneither(_tmp3A,sizeof(char),13);});_tmp39;});
struct _tuple13 noconsume_params=({struct _tuple13 _tmp37;_tmp37.f1=0;_tmp37.f2=({const char*_tmp38="noconsume";_tag_dyneither(_tmp38,sizeof(char),10);});_tmp37;});
void*_tmpB=Cyc_Tcutil_compress(t);void*_tmpC=_tmpB;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp35;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->tag == 9){_LL1: _tmp36=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).attributes;_tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).args;_LL2: {
# 87
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp35);
for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
void*_tmpD=(void*)_tmp36->hd;void*_tmpE=_tmpD;int _tmp32;int _tmp31;int _tmp30;switch(*((int*)_tmpE)){case 20: _LL6: _tmp30=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL7:
# 91
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp30,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,noconsume_params);{
struct _tuple8*_tmpF=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp30 - 1);struct _tuple8*_tmp10=_tmpF;void*_tmp25;_LLF: _tmp25=_tmp10->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp24="initializes attribute allowed only on";_tag_dyneither(_tmp24,sizeof(char),38);});
{void*_tmp11=Cyc_Tcutil_compress(_tmp25);void*_tmp12=_tmp11;void*_tmp22;union Cyc_Absyn_Constraint*_tmp21;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->tag == 5){_LL12: _tmp22=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).elt_typ;_tmp21=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).nullable;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).bounds;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).zero_term;_LL13:
# 98
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp21))
({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp13[1]={& _tmp15};Cyc_Tcutil_terr(loc,({const char*_tmp14="%s non-null pointers";_tag_dyneither(_tmp14,sizeof(char),21);}),_tag_dyneither(_tmp13,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_bound_one(_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp16[1]={& _tmp18};Cyc_Tcutil_terr(loc,({const char*_tmp17="%s pointers of size 1";_tag_dyneither(_tmp17,sizeof(char),22);}),_tag_dyneither(_tmp16,sizeof(void*),1));});});
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp19[1]={& _tmp1B};Cyc_Tcutil_terr(loc,({const char*_tmp1A="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp1A,sizeof(char),42);}),_tag_dyneither(_tmp19,sizeof(void*),1));});});
goto _LL11;}else{_LL14: _LL15:
# 106
({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1C[1]={& _tmp1E};Cyc_Tcutil_terr(loc,({const char*_tmp1D="%s pointers";_tag_dyneither(_tmp1D,sizeof(char),12);}),_tag_dyneither(_tmp1C,sizeof(void*),1));});});}_LL11:;}
# 108
init_params.f1=({struct Cyc_List_List*_tmp23=_region_malloc(temp,sizeof(*_tmp23));_tmp23->hd=(void*)_tmp30;_tmp23->tl=init_params.f1;_tmp23;});
goto _LL5;};};case 21: _LL8: _tmp31=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL9:
# 111
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp26=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp31 - 1);struct _tuple8*_tmp27=_tmp26;void*_tmp2B;_LL17: _tmp2B=_tmp27->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2B,0))
({void*_tmp28=0;Cyc_Tcutil_terr(loc,({const char*_tmp29="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp29,sizeof(char),55);}),_tag_dyneither(_tmp28,sizeof(void*),0));});
nolive_unique_params.f1=({struct Cyc_List_List*_tmp2A=_region_malloc(temp,sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp31;_tmp2A->tl=nolive_unique_params.f1;_tmp2A;});
goto _LL5;};case 22: _LLA: _tmp32=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpE)->f1;_LLB:
# 120
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp32,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp2C=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp32 - 1);struct _tuple8*_tmp2D=_tmp2C;void*_tmp2F;_LL1A: _tmp2F=_tmp2D->f3;_LL1B:;
# 125
noconsume_params.f1=({struct Cyc_List_List*_tmp2E=_region_malloc(temp,sizeof(*_tmp2E));_tmp2E->hd=(void*)_tmp32;_tmp2E->tl=noconsume_params.f1;_tmp2E;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 130
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp33=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp34="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp34,sizeof(char),34);}),_tag_dyneither(_tmp33,sizeof(void*),0));});}_LL0:;}
# 82
;_pop_region(temp);}struct _tuple14{void*f1;int f2;};
# 136
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 140
struct Cyc_Absyn_Vardecl*_tmp3D=vd;enum Cyc_Absyn_Scope _tmpA6;union Cyc_Absyn_Nmspace _tmpA5;struct _dyneither_ptr*_tmpA4;void*_tmpA3;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_List_List*_tmpA1;_LL1D: _tmpA6=_tmp3D->sc;_tmpA5=(_tmp3D->name)->f1;_tmpA4=(_tmp3D->name)->f2;_tmpA3=_tmp3D->type;_tmpA2=_tmp3D->initializer;_tmpA1=_tmp3D->attributes;_LL1E:;
# 147
{void*_tmp3E=Cyc_Tcutil_compress(_tmpA3);void*_tmp3F=_tmp3E;void*_tmp4B;struct Cyc_Absyn_Tqual _tmp4A;union Cyc_Absyn_Constraint*_tmp49;unsigned int _tmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->tag == 8){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).num_elts == 0){_LL20: _tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).tq;_tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zt_loc;if(_tmpA2 != 0){_LL21:
# 149
{void*_tmp40=_tmpA2->r;void*_tmp41=_tmp40;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct _dyneither_ptr _tmp43;struct _dyneither_ptr _tmp42;switch(*((int*)_tmp41)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).tag){case 8: _LL25: _tmp42=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).String_c).val;_LL26:
# 151
 _tmpA3=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp42,sizeof(char)),0),_tmp49,_tmp48));
goto _LL24;case 9: _LL27: _tmp43=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).val;_LL28:
# 154
 _tmpA3=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(1,0),_tmp49,_tmp48));
goto _LL24;default: goto _LL31;}case 26: _LL29: _tmp44=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2A:
 _tmp45=_tmp44;goto _LL2C;case 27: _LL2B: _tmp45=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL2C:
# 159
 _tmpA3=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,_tmp45,_tmp49,_tmp48));
goto _LL24;case 35: _LL2D: _tmp46=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2E:
 _tmp47=_tmp46;goto _LL30;case 25: _LL2F: _tmp47=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL30:
# 163
 _tmpA3=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47),0),_tmp49,_tmp48));
goto _LL24;default: _LL31: _LL32:
 goto _LL24;}_LL24:;}
# 167
goto _LL1F;}else{goto _LL22;}}else{goto _LL22;}}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 171
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmpA3);
# 173
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmpA3);
# 176
{void*_tmp4C=Cyc_Tcutil_compress(_tmpA3);void*_tmp4D=_tmp4C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D)->tag == 8){_LL34: _LL35:
 vd->escapes=0;goto _LL33;}else{_LL36: _LL37:
 vd->escapes=1;goto _LL33;}_LL33:;}
# 181
if(Cyc_Tcutil_is_function_type(_tmpA3)){
_tmpA1=Cyc_Tcutil_transfer_fn_type_atts(_tmpA3,_tmpA1);
Cyc_Tc_fnTypeAttsOK(te,_tmpA3,loc);}
# 186
if(_tmpA6 == Cyc_Absyn_Extern  || _tmpA6 == Cyc_Absyn_ExternC){
if(_tmpA2 != 0  && !in_cinclude)
({void*_tmp4E=0;Cyc_Tcutil_terr(loc,({const char*_tmp4F="extern declaration should not have initializer";_tag_dyneither(_tmp4F,sizeof(char),47);}),_tag_dyneither(_tmp4E,sizeof(void*),0));});}else{
if(!Cyc_Tcutil_is_function_type(_tmpA3)){
# 193
for(0;_tmpA1 != 0;_tmpA1=_tmpA1->tl){
void*_tmp50=(void*)_tmpA1->hd;void*_tmp51=_tmp50;switch(*((int*)_tmp51)){case 6: _LL39: _LL3A:
 goto _LL3C;case 8: _LL3B: _LL3C:
# 198
 goto _LL3E;case 9: _LL3D: _LL3E:
 goto _LL40;case 10: _LL3F: _LL40:
 goto _LL42;case 11: _LL41: _LL42:
 goto _LL44;case 12: _LL43: _LL44:
 goto _LL46;case 13: _LL45: _LL46:
 goto _LL48;case 14: _LL47: _LL48:
 continue;default: _LL49: _LL4A:
# 206
({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name));({struct Cyc_String_pa_PrintArg_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpA1->hd));({void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_Tcutil_terr(loc,({const char*_tmp53="bad attribute %s for variable %s";_tag_dyneither(_tmp53,sizeof(char),33);}),_tag_dyneither(_tmp52,sizeof(void*),2));});});});
goto _LL38;}_LL38:;}
# 211
if(_tmpA2 == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmpA3))
({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpA3));({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));({void*_tmp56[2]={& _tmp58,& _tmp59};Cyc_Tcutil_terr(loc,({const char*_tmp57="initializer required for variable %s of type %s";_tag_dyneither(_tmp57,sizeof(char),48);}),_tag_dyneither(_tmp56,sizeof(void*),2));});});});}else{
# 216
struct Cyc_Absyn_Exp*_tmp5A=_tmpA2;
# 220
struct _handler_cons _tmp5B;_push_handler(& _tmp5B);{int _tmp5D=0;if(setjmp(_tmp5B.handler))_tmp5D=1;if(!_tmp5D){
{void*_tmp5E=Cyc_Tcexp_tcExpInitializer(te,& _tmpA3,_tmp5A);
if(!Cyc_Tcutil_coerce_assign(te,_tmp5A,_tmpA3)){
struct _dyneither_ptr _tmp5F=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp60=" declared with type ";
struct _dyneither_ptr _tmp61=Cyc_Absynpp_typ2string(_tmpA3);
const char*_tmp62=" but initialized with type ";
struct _dyneither_ptr _tmp63=Cyc_Absynpp_typ2string(_tmp5E);
if((((Cyc_strlen((struct _dyneither_ptr)_tmp5F)+ Cyc_strlen(({const char*_tmp64=_tmp60;_tag_dyneither(_tmp64,sizeof(char),_get_zero_arr_size_char((void*)_tmp64,21));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp61))+ Cyc_strlen(({const char*_tmp65=_tmp62;_tag_dyneither(_tmp65,sizeof(char),_get_zero_arr_size_char((void*)_tmp65,28));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp63)> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp63);({struct Cyc_String_pa_PrintArg_struct _tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _dyneither_ptr)({const char*_tmp6E=_tmp62;_tag_dyneither(_tmp6E,sizeof(char),_get_zero_arr_size_char((void*)_tmp6E,28));});({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)({const char*_tmp6D=_tmp60;_tag_dyneither(_tmp6D,sizeof(char),_get_zero_arr_size_char((void*)_tmp6D,21));});({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({void*_tmp66[5]={& _tmp68,& _tmp69,& _tmp6A,& _tmp6B,& _tmp6C};Cyc_Tcutil_terr(loc,({const char*_tmp67="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp67,sizeof(char),16);}),_tag_dyneither(_tmp66,sizeof(void*),5));});});});});});});else{
# 231
({struct Cyc_String_pa_PrintArg_struct _tmp75;_tmp75.tag=0;_tmp75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp63);({struct Cyc_String_pa_PrintArg_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _dyneither_ptr)({const char*_tmp77=_tmp62;_tag_dyneither(_tmp77,sizeof(char),_get_zero_arr_size_char((void*)_tmp77,28));});({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)({const char*_tmp76=_tmp60;_tag_dyneither(_tmp76,sizeof(char),_get_zero_arr_size_char((void*)_tmp76,21));});({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({void*_tmp6F[5]={& _tmp71,& _tmp72,& _tmp73,& _tmp74,& _tmp75};Cyc_Tcutil_terr(loc,({const char*_tmp70="%s%s%s%s%s";_tag_dyneither(_tmp70,sizeof(char),11);}),_tag_dyneither(_tmp6F,sizeof(void*),5));});});});});});});}
Cyc_Tcutil_explain_failure();
# 234
if(!Cyc_Tcutil_is_const_exp(_tmp5A))
({void*_tmp78=0;Cyc_Tcutil_terr(loc,({const char*_tmp79="initializer is not a constant expression";_tag_dyneither(_tmp79,sizeof(char),41);}),_tag_dyneither(_tmp78,sizeof(void*),0));});}}
# 221
;_pop_handler();}else{void*_tmp5C=(void*)_exn_thrown;void*_tmp7A=_tmp5C;void*_tmp7D;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp7A)->tag == Cyc_Tcenv_Env_error){_LL4C: _LL4D:
# 239
({void*_tmp7B=0;Cyc_Tcutil_terr(loc,({const char*_tmp7C="initializer is not a constant expression";_tag_dyneither(_tmp7C,sizeof(char),41);}),_tag_dyneither(_tmp7B,sizeof(void*),0));});
goto _LL4B;}else{_LL4E: _tmp7D=_tmp7A;_LL4F:(int)_rethrow(_tmp7D);}_LL4B:;}};}}else{
# 245
for(0;_tmpA1 != 0;_tmpA1=_tmpA1->tl){
void*_tmp7E=(void*)_tmpA1->hd;void*_tmp7F=_tmp7E;switch(*((int*)_tmp7F)){case 0: _LL51: _LL52:
# 248
 goto _LL54;case 1: _LL53: _LL54:
 goto _LL56;case 2: _LL55: _LL56:
 goto _LL58;case 3: _LL57: _LL58:
 goto _LL5A;case 4: _LL59: _LL5A:
 goto _LL5C;case 19: _LL5B: _LL5C:
 goto _LL5E;case 20: _LL5D: _LL5E:
 goto _LL60;case 23: _LL5F: _LL60:
 goto _LL62;case 26: _LL61: _LL62:
 goto _LL64;case 5: _LL63: _LL64:
({void*_tmp80=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp81="tcVardecl: fn type atts in function var decl";_tag_dyneither(_tmp81,sizeof(char),45);}),_tag_dyneither(_tmp80,sizeof(void*),0));});case 6: _LL65: _LL66:
# 259
 goto _LL68;case 7: _LL67: _LL68:
# 261
({struct Cyc_String_pa_PrintArg_struct _tmp84;_tmp84.tag=0;_tmp84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmpA1->hd));({void*_tmp82[1]={& _tmp84};Cyc_Tcutil_terr(loc,({const char*_tmp83="bad attribute %s in function declaration";_tag_dyneither(_tmp83,sizeof(char),41);}),_tag_dyneither(_tmp82,sizeof(void*),1));});});
goto _LL50;default: _LL69: _LL6A:
 continue;}_LL50:;}}}
# 269
{struct _handler_cons _tmp85;_push_handler(& _tmp85);{int _tmp87=0;if(setjmp(_tmp85.handler))_tmp87=1;if(!_tmp87){
{struct _tuple14*_tmp88=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmpA4);
void*_tmp89=(*_tmp88).f1;void*_tmp8A=_tmp89;void*_tmp99;switch(*((int*)_tmp8A)){case 0: _LL6C: _tmp99=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp8A)->f1;_LL6D: {
# 273
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8B=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp92;_tmp92.tag=1;_tmp92.f1=vd;_tmp92;});_tmp91;});
void**_tmp8C=Cyc_Tcdecl_merge_binding(_tmp99,(void*)_tmp8B,loc,Cyc_Tc_tc_msg);
if(_tmp8C == 0){_npop_handler(0);return;}{
void*_tmp8D=*_tmp8C;
# 278
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp8D != _tmp99  || (*_tmp88).f2)
# 281
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmpA4,({struct _tuple14*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp90;_tmp90.tag=0;_tmp90.f1=_tmp8D;_tmp90;});_tmp8F;});_tmp8E->f2=1;_tmp8E;}));}
# 283
_npop_handler(0);return;};}case 1: _LL6E: _LL6F:
# 285
 if(Cyc_Tc_aggressive_warn)
({void*_tmp93=0;Cyc_Tcutil_warn(loc,({const char*_tmp94="variable declaration shadows previous struct declaration";_tag_dyneither(_tmp94,sizeof(char),57);}),_tag_dyneither(_tmp93,sizeof(void*),0));});
goto _LL6B;case 2: _LL70: _LL71:
# 289
 if(Cyc_Tc_aggressive_warn)
({void*_tmp95=0;Cyc_Tcutil_warn(loc,({const char*_tmp96="variable declaration shadows previous datatype constructor";_tag_dyneither(_tmp96,sizeof(char),59);}),_tag_dyneither(_tmp95,sizeof(void*),0));});
goto _LL6B;case 4: _LL72: _LL73:
 goto _LL75;default: _LL74: _LL75:
# 294
 if(Cyc_Tc_aggressive_warn)
({void*_tmp97=0;Cyc_Tcutil_warn(loc,({const char*_tmp98="variable declaration shadows previous enum tag";_tag_dyneither(_tmp98,sizeof(char),47);}),_tag_dyneither(_tmp97,sizeof(void*),0));});
goto _LL6B;}_LL6B:;}
# 270
;_pop_handler();}else{void*_tmp86=(void*)_exn_thrown;void*_tmp9A=_tmp86;void*_tmp9B;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp9A)->tag == Cyc_Dict_Absent){_LL77: _LL78:
# 298
 goto _LL76;}else{_LL79: _tmp9B=_tmp9A;_LL7A:(int)_rethrow(_tmp9B);}_LL76:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmpA4,({struct _tuple14*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp9E;_tmp9E.tag=0;_tmp9E.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpA0;_tmpA0.tag=1;_tmpA0.f1=vd;_tmpA0;});_tmp9F;});_tmp9E;});_tmp9D;});_tmp9C->f2=0;_tmp9C;}));}
# 304
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmpA7=n;union Cyc_Absyn_Nmspace _tmpAB;struct _dyneither_ptr*_tmpAA;_LL7C: _tmpAB=_tmpA7->f1;_tmpAA=_tmpA7->f2;_LL7D:;{
union Cyc_Absyn_Nmspace _tmpA8=_tmpAB;if((_tmpA8.Abs_n).tag == 2){if((_tmpA8.Abs_n).val == 0){_LL7F: _LL80:
# 308
 return Cyc_strcmp((struct _dyneither_ptr)*_tmpAA,({const char*_tmpA9="main";_tag_dyneither(_tmpA9,sizeof(char),5);}))== 0;}else{goto _LL81;}}else{_LL81: _LL82:
 return 0;}_LL7E:;};}
# 313
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 315
struct _dyneither_ptr*v=(*fd->name).f2;
# 319
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmpAC=0;Cyc_Tcutil_terr(loc,({const char*_tmpAD="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmpAD,sizeof(char),54);}),_tag_dyneither(_tmpAC,sizeof(void*),0));});
# 323
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 325
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 327
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 330
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpAE=(void*)atts->hd;void*_tmpAF=_tmpAE;switch(*((int*)_tmpAF)){case 7: _LL84: _LL85:
 goto _LL87;case 6: _LL86: _LL87:
# 334
({struct Cyc_String_pa_PrintArg_struct _tmpB2;_tmpB2.tag=0;_tmpB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmpB0[1]={& _tmpB2};Cyc_Tcutil_terr(loc,({const char*_tmpB1="bad attribute %s for function";_tag_dyneither(_tmpB1,sizeof(char),30);}),_tag_dyneither(_tmpB0,sizeof(void*),1));});});
goto _LL83;default: _LL88: _LL89:
 goto _LL83;}_LL83:;}}
# 340
{struct _handler_cons _tmpB3;_push_handler(& _tmpB3);{int _tmpB5=0;if(setjmp(_tmpB3.handler))_tmpB5=1;if(!_tmpB5){
{struct _tuple14*_tmpB6=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_tmpB7=(*_tmpB6).f1;void*_tmpB8=_tmpB7;void*_tmpC7;switch(*((int*)_tmpB8)){case 0: _LL8B: _tmpC7=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB8)->f1;_LL8C: {
# 344
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB9=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpC0;_tmpC0.tag=2;_tmpC0.f1=fd;_tmpC0;});_tmpBF;});
void**_tmpBA=Cyc_Tcdecl_merge_binding(_tmpC7,(void*)_tmpB9,loc,Cyc_Tc_tc_msg);
if(_tmpBA == 0)goto _LL8A;{
void*_tmpBB=*_tmpBA;
# 349
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpBB == _tmpC7  && (*_tmpB6).f2))
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple14*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpBE;_tmpBE.tag=0;_tmpBE.f1=_tmpBB;_tmpBE;});_tmpBD;});_tmpBC->f2=1;_tmpBC;}));}
# 353
goto _LL8A;};}case 1: _LL8D: _LL8E:
# 355
({void*_tmpC1=0;Cyc_Tcutil_warn(loc,({const char*_tmpC2="function declaration shadows previous type declaration";_tag_dyneither(_tmpC2,sizeof(char),55);}),_tag_dyneither(_tmpC1,sizeof(void*),0));});
goto _LL8A;case 2: _LL8F: _LL90:
# 358
({void*_tmpC3=0;Cyc_Tcutil_warn(loc,({const char*_tmpC4="function declaration shadows previous datatype constructor";_tag_dyneither(_tmpC4,sizeof(char),59);}),_tag_dyneither(_tmpC3,sizeof(void*),0));});
goto _LL8A;case 4: _LL91: _LL92:
 goto _LL94;default: _LL93: _LL94:
# 362
({void*_tmpC5=0;Cyc_Tcutil_warn(loc,({const char*_tmpC6="function declaration shadows previous enum tag";_tag_dyneither(_tmpC6,sizeof(char),47);}),_tag_dyneither(_tmpC5,sizeof(void*),0));});
goto _LL8A;}_LL8A:;}
# 341
;_pop_handler();}else{void*_tmpB4=(void*)_exn_thrown;void*_tmpC8=_tmpB4;void*_tmpD1;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC8)->tag == Cyc_Dict_Absent){_LL96: _LL97:
# 366
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC9=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpD0;_tmpD0.tag=2;_tmpD0.f1=fd;_tmpD0;});_tmpCF;});
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple14*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpCC;_tmpCC.tag=0;_tmpCC.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpCE;_tmpCE.tag=2;_tmpCE.f1=fd;_tmpCE;});_tmpCD;});_tmpCC;});_tmpCB;});_tmpCA->f2=0;_tmpCA;}));}
# 371
goto _LL95;}else{_LL98: _tmpD1=_tmpC8;_LL99:(int)_rethrow(_tmpD1);}_LL95:;}};}
# 375
if(te->in_extern_c_include)return;
# 380
{struct _RegionHandle _tmpD2=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpD2;_push_region(fnrgn);
{struct Cyc_Tcenv_Fenv*_tmpD3=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmpD4=({struct Cyc_Tcenv_Tenv*_tmpDA=_region_malloc(fnrgn,sizeof(*_tmpDA));_tmpDA->ns=te->ns;_tmpDA->ae=te->ae;_tmpDA->le=_tmpD3;_tmpDA->allow_valueof=0;_tmpDA->in_extern_c_include=0;_tmpDA;});
# 384
struct _handler_cons _tmpD5;_push_handler(& _tmpD5);{int _tmpD7=0;if(setjmp(_tmpD5.handler))_tmpD7=1;if(!_tmpD7){
Cyc_Tcstmt_tcStmt(_tmpD4,fd->body,0);
# 388
Cyc_Tcenv_check_delayed_effects(_tmpD4);
Cyc_Tcenv_check_delayed_constraints(_tmpD4);
# 385
;_pop_handler();}else{void*_tmpD6=(void*)_exn_thrown;void*_tmpD8=_tmpD6;void*_tmpD9;if(((struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*)_tmpD8)->tag == Cyc_Tcutil_AbortTypeCheckingFunction){_LL9B: _LL9C:
# 391
 goto _LL9A;}else{_LL9D: _tmpD9=_tmpD8;_LL9E:
 Cyc_Core_rethrow(_tmpD9);}_LL9A:;}};}
# 381
;_pop_region(fnrgn);}
# 396
if(Cyc_Tc_is_main(fd->name)){
# 398
{void*_tmpDB=Cyc_Tcutil_compress(fd->ret_type);void*_tmpDC=_tmpDB;enum Cyc_Absyn_Size_of _tmpE2;switch(*((int*)_tmpDC)){case 0: _LLA2: _LLA3:
({void*_tmpDD=0;Cyc_Tcutil_warn(loc,({const char*_tmpDE="main declared with return type void";_tag_dyneither(_tmpDE,sizeof(char),36);}),_tag_dyneither(_tmpDD,sizeof(void*),0));});goto _LLA1;case 6: _LLA4: _tmpE2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDC)->f2;_LLA5:
 goto _LLA1;default: _LLA6: _LLA7:
# 402
({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type));({void*_tmpDF[1]={& _tmpE1};Cyc_Tcutil_terr(loc,({const char*_tmpE0="main declared with return type %s instead of int or void";_tag_dyneither(_tmpE0,sizeof(char),57);}),_tag_dyneither(_tmpDF,sizeof(void*),1));});});
goto _LLA1;}_LLA1:;}
# 406
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpE3=0;Cyc_Tcutil_terr(loc,({const char*_tmpE4="main declared with varargs";_tag_dyneither(_tmpE4,sizeof(char),27);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});{
struct Cyc_List_List*_tmpE5=fd->args;
if(_tmpE5 != 0){
struct _tuple8*_tmpE6=(struct _tuple8*)_tmpE5->hd;struct _tuple8*_tmpE7=_tmpE6;void*_tmpFF;_LLA9: _tmpFF=_tmpE7->f3;_LLAA:;
{void*_tmpE8=Cyc_Tcutil_compress(_tmpFF);void*_tmpE9=_tmpE8;enum Cyc_Absyn_Size_of _tmpED;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE9)->tag == 6){_LLAC: _tmpED=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE9)->f2;_LLAD:
 goto _LLAB;}else{_LLAE: _LLAF:
# 414
({struct Cyc_String_pa_PrintArg_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpFF));({void*_tmpEA[1]={& _tmpEC};Cyc_Tcutil_terr(loc,({const char*_tmpEB="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpEB,sizeof(char),60);}),_tag_dyneither(_tmpEA,sizeof(void*),1));});});
goto _LLAB;}_LLAB:;}
# 418
_tmpE5=_tmpE5->tl;
if(_tmpE5 != 0){
struct _tuple8*_tmpEE=(struct _tuple8*)_tmpE5->hd;struct _tuple8*_tmpEF=_tmpEE;void*_tmpFE;_LLB1: _tmpFE=_tmpEF->f3;_LLB2:;
_tmpE5=_tmpE5->tl;
if(_tmpE5 != 0)
({void*_tmpF0=0;Cyc_Tcutil_terr(loc,({const char*_tmpF1="main declared with too many arguments";_tag_dyneither(_tmpF1,sizeof(char),38);}),_tag_dyneither(_tmpF0,sizeof(void*),0));});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->v=fd->tvs;_tmpFD;});
if(((!Cyc_Tcutil_unify(_tmpFE,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->v=& Cyc_Tcutil_rk;_tmpF2;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->v=& Cyc_Tcutil_rk;_tmpF3;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpFE,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->v=& Cyc_Tcutil_rk;_tmpF4;}),tvs)),
# 430
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->v=& Cyc_Tcutil_rk;_tmpF5;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpFE,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->v=& Cyc_Tcutil_rk;_tmpF6;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->v=& Cyc_Tcutil_rk;_tmpF7;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpFE,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->v=& Cyc_Tcutil_rk;_tmpF8;}),tvs)),
# 437
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->v=& Cyc_Tcutil_rk;_tmpF9;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
({struct Cyc_String_pa_PrintArg_struct _tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpFE));({void*_tmpFA[1]={& _tmpFC};Cyc_Tcutil_terr(loc,({const char*_tmpFB="second argument of main has type %s instead of char??";_tag_dyneither(_tmpFB,sizeof(char),54);}),_tag_dyneither(_tmpFA,sizeof(void*),1));});});};}}};}};}
# 447
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 454
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
({struct Cyc_String_pa_PrintArg_struct _tmp102;_tmp102.tag=0;_tmp102.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp100[1]={& _tmp102};Cyc_Tcutil_terr(loc,({const char*_tmp101="redeclaration of typedef %s";_tag_dyneither(_tmp101,sizeof(char),28);}),_tag_dyneither(_tmp100,sizeof(void*),1));});});
return;}
# 459
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 468
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp103=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmp104=_tmp103;struct Cyc_Core_Opt**_tmp10C;struct Cyc_Absyn_Kind*_tmp10B;struct Cyc_Core_Opt**_tmp10A;switch(*((int*)_tmp104)){case 1: _LLB4: _tmp10A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp104)->f1;_LLB5:
# 471
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0;_tmp107.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);({void*_tmp105[1]={& _tmp107};Cyc_Tcutil_warn(loc,({const char*_tmp106="type variable %s is not used in typedef definition";_tag_dyneither(_tmp106,sizeof(char),51);}),_tag_dyneither(_tmp105,sizeof(void*),1));});});
# 474
*_tmp10A=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmp108;});goto _LLB3;case 2: _LLB6: _tmp10C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp104)->f1;_tmp10B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp104)->f2;_LLB7:
# 481
*_tmp10C=({struct Cyc_Core_Opt*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->v=Cyc_Tcutil_kind_to_bound(_tmp10B);_tmp109;});
goto _LLB3;default: _LLB8: _LLB9:
 continue;}_LLB3:;}}
# 487
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 490
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 497
struct _RegionHandle _tmp10D=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp10D;_push_region(uprev_rgn);
# 499
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmp10E=(struct _tuple10*)rpo->hd;struct _tuple10*_tmp10F=_tmp10E;void*_tmp111;void*_tmp110;_LLBB: _tmp111=_tmp10F->f1;_tmp110=_tmp10F->f2;_LLBC:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmp111);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmp110);}{
# 505
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 508
struct Cyc_List_List*_tmp112=fields;for(0;_tmp112 != 0;_tmp112=_tmp112->tl){
struct Cyc_Absyn_Aggrfield*_tmp113=(struct Cyc_Absyn_Aggrfield*)_tmp112->hd;struct Cyc_Absyn_Aggrfield*_tmp114=_tmp113;struct _dyneither_ptr*_tmp131;struct Cyc_Absyn_Tqual _tmp130;void*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;_LLBE: _tmp131=_tmp114->name;_tmp130=_tmp114->tq;_tmp12F=_tmp114->type;_tmp12E=_tmp114->width;_tmp12D=_tmp114->attributes;_tmp12C=_tmp114->requires_clause;_LLBF:;
# 511
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp131))
({struct Cyc_String_pa_PrintArg_struct _tmp117;_tmp117.tag=0;_tmp117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp131);({void*_tmp115[1]={& _tmp117};Cyc_Tcutil_terr(loc,({const char*_tmp116="duplicate member %s";_tag_dyneither(_tmp116,sizeof(char),20);}),_tag_dyneither(_tmp115,sizeof(void*),1));});});
# 515
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp131,({const char*_tmp118="";_tag_dyneither(_tmp118,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp119=_region_malloc(uprev_rgn,sizeof(*_tmp119));_tmp119->hd=_tmp131;_tmp119->tl=prev_fields;_tmp119;});{
# 518
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 522
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp112->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp12F);
# 527
(((struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->tq).print_const,_tmp12F);
# 530
Cyc_Tcutil_check_bitfield(loc,te,_tmp12F,_tmp12E,_tmp131);
# 532
if((unsigned int)_tmp12C){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmp11A=0;Cyc_Tcutil_terr(loc,({const char*_tmp11B="@requires clauses are only allowed on union members";_tag_dyneither(_tmp11B,sizeof(char),52);}),_tag_dyneither(_tmp11A,sizeof(void*),0));});
{struct _RegionHandle _tmp11C=_new_region("temp");struct _RegionHandle*temp=& _tmp11C;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp11D=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp11D,0,_tmp12C);}
# 536
;_pop_region(temp);}
# 539
if(!Cyc_Tcutil_is_integral(_tmp12C))
({struct Cyc_String_pa_PrintArg_struct _tmp120;_tmp120.tag=0;_tmp120.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 542
Cyc_Absynpp_typ2string((void*)_check_null(_tmp12C->topt)));({void*_tmp11E[1]={& _tmp120};Cyc_Tcutil_terr(_tmp12C->loc,({const char*_tmp11F="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp11F,sizeof(char),54);}),_tag_dyneither(_tmp11E,sizeof(void*),1));});});else{
# 544
Cyc_Tcutil_check_type(_tmp12C->loc,te,tvs,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp122;_tmp122.tag=18;_tmp122.f1=_tmp12C;_tmp122;});_tmp121;}));{
# 546
struct Cyc_List_List*_tmp123=Cyc_Relations_exp2relns(uprev_rgn,_tmp12C);
# 553
if(!Cyc_Relations_consistent_relations(_tmp123))
({void*_tmp124=0;Cyc_Tcutil_terr(_tmp12C->loc,({const char*_tmp125="@requires clause may be unsatisfiable";_tag_dyneither(_tmp125,sizeof(char),38);}),_tag_dyneither(_tmp124,sizeof(void*),0));});
# 559
{struct Cyc_List_List*_tmp126=prev_relations;for(0;_tmp126 != 0;_tmp126=_tmp126->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp123,(struct Cyc_List_List*)_tmp126->hd)))
({void*_tmp127=0;Cyc_Tcutil_terr(_tmp12C->loc,({const char*_tmp128="@requires clause may overlap with previous clauses";_tag_dyneither(_tmp128,sizeof(char),51);}),_tag_dyneither(_tmp127,sizeof(void*),0));});}}
# 564
prev_relations=({struct Cyc_List_List*_tmp129=_region_malloc(uprev_rgn,sizeof(*_tmp129));_tmp129->hd=_tmp123;_tmp129->tl=prev_relations;_tmp129;});};}}else{
# 567
if(prev_relations != 0)
({void*_tmp12A=0;Cyc_Tcutil_terr(loc,({const char*_tmp12B="if one field has a @requires clause, they all must";_tag_dyneither(_tmp12B,sizeof(char),51);}),_tag_dyneither(_tmp12A,sizeof(void*),0));});}};}};
# 499
;_pop_region(uprev_rgn);}
# 573
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 575
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp132=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp133=_tmp132;enum Cyc_Absyn_AliasQual _tmp148;struct Cyc_Core_Opt**_tmp147;struct Cyc_Absyn_Kind*_tmp146;struct Cyc_Core_Opt**_tmp145;struct Cyc_Core_Opt**_tmp144;enum Cyc_Absyn_AliasQual _tmp143;struct Cyc_Core_Opt**_tmp142;switch(*((int*)_tmp133)){case 1: _LLC1: _tmp142=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp133)->f1;_LLC2:
# 578
*_tmp142=({struct Cyc_Core_Opt*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp134;});continue;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133)->f2)->kind){case Cyc_Absyn_MemKind: _LLC3: _tmp144=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133)->f1;_tmp143=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133)->f2)->aliasqual;_LLC4:
# 580
 if(constrain_top_kind  && _tmp143 == Cyc_Absyn_Top)
*_tmp144=({struct Cyc_Core_Opt*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp136=_cycalloc_atomic(sizeof(*_tmp136));_tmp136->kind=Cyc_Absyn_BoxKind;_tmp136->aliasqual=Cyc_Absyn_Aliasable;_tmp136;}));_tmp135;});else{
# 583
*_tmp144=({struct Cyc_Core_Opt*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp138=_cycalloc_atomic(sizeof(*_tmp138));_tmp138->kind=Cyc_Absyn_BoxKind;_tmp138->aliasqual=_tmp143;_tmp138;}));_tmp137;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133)->f2)->aliasqual == Cyc_Absyn_Top){_LLC5: _tmp145=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133)->f1;if(constrain_top_kind){_LLC6:
# 586
*_tmp145=({struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp13A=_cycalloc_atomic(sizeof(*_tmp13A));_tmp13A->kind=Cyc_Absyn_BoxKind;_tmp13A->aliasqual=Cyc_Absyn_Aliasable;_tmp13A;}));_tmp139;});
continue;}else{goto _LLC7;}}else{goto _LLC7;}default: _LLC7: _tmp147=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133)->f1;_tmp146=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133)->f2;_LLC8:
# 589
*_tmp147=({struct Cyc_Core_Opt*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->v=Cyc_Tcutil_kind_to_bound(_tmp146);_tmp13B;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp133)->f1)->kind == Cyc_Absyn_MemKind){_LLC9: _tmp148=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp133)->f1)->aliasqual;_LLCA:
# 591
({struct Cyc_String_pa_PrintArg_struct _tmp140;_tmp140.tag=0;_tmp140.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp141=_cycalloc_atomic(sizeof(*_tmp141));_tmp141->kind=Cyc_Absyn_MemKind;_tmp141->aliasqual=_tmp148;_tmp141;})));({struct Cyc_String_pa_PrintArg_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n);({void*_tmp13C[3]={& _tmp13E,& _tmp13F,& _tmp140};Cyc_Tcutil_terr(loc,({const char*_tmp13D="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp13D,sizeof(char),57);}),_tag_dyneither(_tmp13C,sizeof(void*),3));});});});});
continue;}else{_LLCB: _LLCC:
 continue;}}_LLC0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 599
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp149=(*ad->name).f2;
# 606
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp14A=(void*)atts->hd;void*_tmp14B=_tmp14A;switch(*((int*)_tmp14B)){case 7: _LLCE: _LLCF:
 goto _LLD1;case 6: _LLD0: _LLD1:
 continue;default: _LLD2: _LLD3:
# 611
({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp149);({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp14C[2]={& _tmp14E,& _tmp14F};Cyc_Tcutil_terr(loc,({const char*_tmp14D="bad attribute %s in %s definition";_tag_dyneither(_tmp14D,sizeof(char),34);}),_tag_dyneither(_tmp14C,sizeof(void*),2));});});});
goto _LLCD;}_LLCD:;}}{
# 616
struct Cyc_List_List*_tmp150=ad->tvs;
# 619
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 621
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 625
{struct _tuple15 _tmp151=({struct _tuple15 _tmp16F;_tmp16F.f1=ad->impl;_tmp16F.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp149);_tmp16F;});struct _tuple15 _tmp152=_tmp151;struct Cyc_List_List*_tmp16E;struct Cyc_List_List*_tmp16D;struct Cyc_List_List*_tmp16C;int _tmp16B;struct Cyc_Absyn_Aggrdecl**_tmp16A;struct Cyc_List_List*_tmp169;struct Cyc_List_List*_tmp168;struct Cyc_List_List*_tmp167;int _tmp166;struct Cyc_Absyn_Aggrdecl**_tmp165;if(_tmp152.f1 == 0){if(_tmp152.f2 == 0){_LLD5: _LLD6:
# 628
 Cyc_Tc_rule_out_memkind(loc,_tmp149,_tmp150,0);
# 630
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp149,({struct Cyc_Absyn_Aggrdecl**_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=ad;_tmp153;}));
goto _LLD4;}else{_LLDB: _tmp165=*_tmp152.f2;_LLDC: {
# 692
struct Cyc_Absyn_Aggrdecl*_tmp164=Cyc_Tcdecl_merge_aggrdecl(*_tmp165,ad,loc,Cyc_Tc_tc_msg);
if(_tmp164 == 0)
return;else{
# 696
Cyc_Tc_rule_out_memkind(loc,_tmp149,_tmp150,0);
# 699
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp149,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 702
*_tmp165=_tmp164;
ad=_tmp164;
goto _LLD4;}}}}else{if(_tmp152.f2 == 0){_LLD7: _tmp169=(_tmp152.f1)->exist_vars;_tmp168=(_tmp152.f1)->rgn_po;_tmp167=(_tmp152.f1)->fields;_tmp166=(_tmp152.f1)->tagged;_LLD8: {
# 635
struct Cyc_Absyn_Aggrdecl**_tmp154=({struct Cyc_Absyn_Aggrdecl**_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_Aggrdecl*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->kind=ad->kind;_tmp15D->sc=Cyc_Absyn_Extern;_tmp15D->name=ad->name;_tmp15D->tvs=_tmp150;_tmp15D->impl=0;_tmp15D->attributes=ad->attributes;_tmp15D->expected_mem_kind=0;_tmp15D;});_tmp15C;});
# 637
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp149,_tmp154);
# 642
Cyc_Tcutil_check_unique_tvars(loc,_tmp169);
# 644
Cyc_Tcutil_add_tvar_identities(_tmp169);
# 647
if(_tmp166  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp155=0;Cyc_Tcutil_terr(loc,({const char*_tmp156="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp156,sizeof(char),56);}),_tag_dyneither(_tmp155,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp150,_tmp169),_tmp168,_tmp167);
# 651
Cyc_Tc_rule_out_memkind(loc,_tmp149,_tmp150,0);
# 654
Cyc_Tc_rule_out_memkind(loc,_tmp149,_tmp169,1);
# 656
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp166){
# 659
struct Cyc_List_List*f=_tmp167;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp149);({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);({void*_tmp157[3]={& _tmp159,& _tmp15A,& _tmp15B};Cyc_Tcutil_warn(loc,({const char*_tmp158="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp158,sizeof(char),98);}),_tag_dyneither(_tmp157,sizeof(void*),3));});});});});}}
# 665
*_tmp154=ad;
goto _LLD4;}}else{_LLD9: _tmp16E=(_tmp152.f1)->exist_vars;_tmp16D=(_tmp152.f1)->rgn_po;_tmp16C=(_tmp152.f1)->fields;_tmp16B=(_tmp152.f1)->tagged;_tmp16A=*_tmp152.f2;_LLDA:
# 669
 if(ad->kind != (*_tmp16A)->kind)
({void*_tmp15E=0;Cyc_Tcutil_terr(loc,({const char*_tmp15F="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp15F,sizeof(char),52);}),_tag_dyneither(_tmp15E,sizeof(void*),0));});{
# 672
struct Cyc_Absyn_Aggrdecl*_tmp160=*_tmp16A;
# 674
*_tmp16A=({struct Cyc_Absyn_Aggrdecl*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->kind=ad->kind;_tmp161->sc=Cyc_Absyn_Extern;_tmp161->name=ad->name;_tmp161->tvs=_tmp150;_tmp161->impl=0;_tmp161->attributes=ad->attributes;_tmp161->expected_mem_kind=0;_tmp161;});
# 680
Cyc_Tcutil_check_unique_tvars(loc,_tmp16E);
# 682
Cyc_Tcutil_add_tvar_identities(_tmp16E);
# 684
if(_tmp16B  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp162=0;Cyc_Tcutil_terr(loc,({const char*_tmp163="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp163,sizeof(char),56);}),_tag_dyneither(_tmp162,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp150,_tmp16E),_tmp16D,_tmp16C);
# 688
*_tmp16A=_tmp160;
_tmp165=_tmp16A;goto _LLDC;};}}_LLD4:;}
# 708
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp149,({struct _tuple14*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->f1=(void*)({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171[0]=({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp172;_tmp172.tag=1;_tmp172.f1=ad;_tmp172;});_tmp171;});_tmp170->f2=1;_tmp170;}));};}
# 712
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp173=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp174=_tmp173;enum Cyc_Absyn_AliasQual _tmp191;enum Cyc_Absyn_KindQual _tmp190;struct Cyc_Core_Opt**_tmp18F;enum Cyc_Absyn_KindQual _tmp18E;struct Cyc_Core_Opt**_tmp18D;struct Cyc_Core_Opt**_tmp18C;struct Cyc_Core_Opt**_tmp18B;struct Cyc_Core_Opt**_tmp18A;struct Cyc_Core_Opt**_tmp189;struct Cyc_Core_Opt**_tmp188;switch(*((int*)_tmp174)){case 1: _LLDE: _tmp188=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp174)->f1;_LLDF:
 _tmp189=_tmp188;goto _LLE1;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f2)->aliasqual){case Cyc_Absyn_Top: _LLE0: _tmp189=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f1;_LLE1:
# 717
 _tmp18A=_tmp189;goto _LLE3;case Cyc_Absyn_Aliasable: _LLE2: _tmp18A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f1;_LLE3:
# 719
*_tmp18A=({struct Cyc_Core_Opt*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp175;});goto _LLDD;default: goto _LLEC;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f2)->aliasqual){case Cyc_Absyn_Top: _LLE4: _tmp18B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f1;_LLE5:
 _tmp18C=_tmp18B;goto _LLE7;case Cyc_Absyn_Aliasable: _LLE6: _tmp18C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f1;_LLE7:
# 722
*_tmp18C=({struct Cyc_Core_Opt*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp176;});goto _LLDD;default: goto _LLEC;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f2)->aliasqual){case Cyc_Absyn_Top: _LLE8: _tmp18D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f1;_LLE9:
# 724
*_tmp18D=({struct Cyc_Core_Opt*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp177;});goto _LLDD;case Cyc_Absyn_Unique: goto _LLEC;default: goto _LLF2;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f2)->aliasqual == Cyc_Absyn_Unique){_LLEC: _tmp18F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f1;_tmp18E=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp174)->f2)->kind;_LLED:
# 728
 _tmp190=_tmp18E;goto _LLEF;}else{goto _LLF2;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp174)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp174)->f1)->aliasqual){case Cyc_Absyn_Top: _LLEA: _LLEB:
# 726
({struct Cyc_String_pa_PrintArg_struct _tmp17B;_tmp17B.tag=0;_tmp17B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp178[2]={& _tmp17A,& _tmp17B};Cyc_Tcutil_terr(loc,({const char*_tmp179="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp179,sizeof(char),57);}),_tag_dyneither(_tmp178,sizeof(void*),2));});});});
goto _LLDD;case Cyc_Absyn_Unique: goto _LLEE;default: goto _LLF2;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp174)->f1)->aliasqual == Cyc_Absyn_Unique){_LLEE: _tmp190=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp174)->f1)->kind;_LLEF:
# 730
({struct Cyc_String_pa_PrintArg_struct _tmp180;_tmp180.tag=0;_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 732
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp181=_cycalloc_atomic(sizeof(*_tmp181));_tmp181->kind=_tmp190;_tmp181->aliasqual=Cyc_Absyn_Unique;_tmp181;})));({struct Cyc_String_pa_PrintArg_struct _tmp17F;_tmp17F.tag=0;_tmp17F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp17C[3]={& _tmp17E,& _tmp17F,& _tmp180};Cyc_Tcutil_terr(loc,({const char*_tmp17D="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp17D,sizeof(char),57);}),_tag_dyneither(_tmp17C,sizeof(void*),3));});});});});goto _LLDD;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp174)->f1)->kind == Cyc_Absyn_MemKind){_LLF0: _tmp191=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp174)->f1)->aliasqual;_LLF1:
# 734
({struct Cyc_String_pa_PrintArg_struct _tmp186;_tmp186.tag=0;_tmp186.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 736
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp187=_cycalloc_atomic(sizeof(*_tmp187));_tmp187->kind=Cyc_Absyn_MemKind;_tmp187->aliasqual=_tmp191;_tmp187;})));({struct Cyc_String_pa_PrintArg_struct _tmp185;_tmp185.tag=0;_tmp185.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp184;_tmp184.tag=0;_tmp184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp182[3]={& _tmp184,& _tmp185,& _tmp186};Cyc_Tcutil_terr(loc,({const char*_tmp183="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp183,sizeof(char),57);}),_tag_dyneither(_tmp182,sizeof(void*),3));});});});});goto _LLDD;}else{_LLF2: _LLF3:
 goto _LLDD;}}}}_LLDD:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 742
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 751
{struct Cyc_List_List*_tmp192=fields;for(0;_tmp192 != 0;_tmp192=_tmp192->tl){
struct Cyc_Absyn_Datatypefield*_tmp193=(struct Cyc_Absyn_Datatypefield*)_tmp192->hd;
struct Cyc_List_List*typs=_tmp193->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp193->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);{
# 756
struct _RegionHandle _tmp194=_new_region("temp");struct _RegionHandle*temp=& _tmp194;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp197;_tmp197.tag=0;_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 760
Cyc_Absynpp_qvar2string(_tmp193->name));({void*_tmp195[1]={& _tmp197};Cyc_Tcutil_terr(_tmp193->loc,({const char*_tmp196="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp196,sizeof(char),51);}),_tag_dyneither(_tmp195,sizeof(void*),1));});});
# 762
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp193->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 756
;_pop_region(temp);};}}}{
# 768
struct Cyc_List_List*fields2;
if(is_extensible){
# 771
int _tmp198=1;
struct Cyc_List_List*_tmp199=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp198,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp198)
fields2=_tmp199;else{
# 776
fields2=0;}}else{
# 778
struct _RegionHandle _tmp19A=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp19A;_push_region(uprev_rgn);
# 780
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp19B=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp19B->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp19F;_tmp19F.tag=0;_tmp19F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19B->name).f2);({void*_tmp19C[2]={& _tmp19E,& _tmp19F};Cyc_Tcutil_terr(_tmp19B->loc,({const char*_tmp19D="duplicate field name %s in %s";_tag_dyneither(_tmp19D,sizeof(char),30);}),_tag_dyneither(_tmp19C,sizeof(void*),2));});});});else{
# 786
prev_fields=({struct Cyc_List_List*_tmp1A0=_region_malloc(uprev_rgn,sizeof(*_tmp1A0));_tmp1A0->hd=(*_tmp19B->name).f2;_tmp1A0->tl=prev_fields;_tmp1A0;});}
# 788
if(_tmp19B->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19B->name).f2);({void*_tmp1A1[1]={& _tmp1A3};Cyc_Tcutil_warn(loc,({const char*_tmp1A2="ignoring scope of field %s";_tag_dyneither(_tmp1A2,sizeof(char),27);}),_tag_dyneither(_tmp1A1,sizeof(void*),1));});});
_tmp19B->sc=Cyc_Absyn_Public;}}}
# 793
fields2=fields;}
# 780
;_pop_region(uprev_rgn);}
# 799
{struct Cyc_List_List*_tmp1A4=fields;for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){
struct Cyc_Absyn_Datatypefield*_tmp1A5=(struct Cyc_Absyn_Datatypefield*)_tmp1A4->hd;
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1A5->name).f2,({struct _tuple14*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp1A8;_tmp1A8.tag=2;_tmp1A8.f1=tudres;_tmp1A8.f2=_tmp1A5;_tmp1A8;});_tmp1A7;});_tmp1A6->f2=1;_tmp1A6;}));}}
# 805
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 808
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp1C1="@extensible datatype";_tag_dyneither(_tmp1C1,sizeof(char),21);}):({const char*_tmp1C2="datatype";_tag_dyneither(_tmp1C2,sizeof(char),9);});
# 815
struct Cyc_List_List*tvs=tud->tvs;
# 818
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 820
Cyc_Tcutil_add_tvar_identities(tvs);{
# 825
struct _RegionHandle _tmp1A9=_new_region("temp");struct _RegionHandle*temp=& _tmp1A9;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 835 "tc.cyc"
{struct _handler_cons _tmp1AA;_push_handler(& _tmp1AA);{int _tmp1AC=0;if(setjmp(_tmp1AA.handler))_tmp1AC=1;if(!_tmp1AC){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp1AD=_cycalloc_atomic(sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_Dict_Absent_exn_struct _tmp1AE;_tmp1AE.tag=Cyc_Dict_Absent;_tmp1AE;});_tmp1AD;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 841
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 836
;_pop_handler();}else{void*_tmp1AB=(void*)_exn_thrown;void*_tmp1AF=_tmp1AB;void*_tmp1B2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1AF)->tag == Cyc_Dict_Absent){_LLF5: _LLF6: {
# 845
struct Cyc_Absyn_Datatypedecl***_tmp1B0=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
tud_opt=(unsigned int)_tmp1B0?({struct Cyc_Absyn_Datatypedecl***_tmp1B1=_region_malloc(temp,sizeof(*_tmp1B1));_tmp1B1[0]=*_tmp1B0;_tmp1B1;}): 0;
goto _LLF4;}}else{_LLF7: _tmp1B2=_tmp1AF;_LLF8:(int)_rethrow(_tmp1B2);}_LLF4:;}};}{
# 852
struct _tuple17 _tmp1B3=({struct _tuple17 _tmp1C0;_tmp1C0.f1=tud->fields;_tmp1C0.f2=tud_opt;_tmp1C0;});struct _tuple17 _tmp1B4=_tmp1B3;struct Cyc_List_List**_tmp1BF;struct Cyc_Absyn_Datatypedecl**_tmp1BE;struct Cyc_List_List**_tmp1BD;struct Cyc_Absyn_Datatypedecl**_tmp1BC;if(_tmp1B4.f1 == 0){if(_tmp1B4.f2 == 0){_LLFA: _LLFB:
# 855
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,({struct Cyc_Absyn_Datatypedecl**_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=tud;_tmp1B5;}));
goto _LLF9;}else{_LL100: _tmp1BC=*_tmp1B4.f2;_LL101: {
# 888
struct Cyc_Absyn_Datatypedecl*_tmp1BB=Cyc_Tcdecl_merge_datatypedecl(*_tmp1BC,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp1BB == 0){
_npop_handler(0);return;}else{
# 893
*_tmp1BC=_tmp1BB;
goto _LLF9;}}}}else{if(_tmp1B4.f2 == 0){_LLFC: _tmp1BD=(struct Cyc_List_List**)&(_tmp1B4.f1)->v;_LLFD: {
# 860
struct Cyc_Absyn_Datatypedecl**_tmp1B6=({struct Cyc_Absyn_Datatypedecl**_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7[0]=({struct Cyc_Absyn_Datatypedecl*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->sc=Cyc_Absyn_Extern;_tmp1B8->name=tud->name;_tmp1B8->tvs=tvs;_tmp1B8->fields=0;_tmp1B8->is_extensible=tud->is_extensible;_tmp1B8;});_tmp1B7;});
# 862
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1B6);
# 865
*_tmp1BD=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1BD,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp1B6=tud;
goto _LLF9;}}else{_LLFE: _tmp1BF=(struct Cyc_List_List**)&(_tmp1B4.f1)->v;_tmp1BE=*_tmp1B4.f2;_LLFF: {
# 870
struct Cyc_Absyn_Datatypedecl*_tmp1B9=*_tmp1BE;
# 873
if((!tud->is_extensible  && (unsigned int)_tmp1B9) && _tmp1B9->is_extensible)
tud->is_extensible=1;
# 876
*_tmp1BE=({struct Cyc_Absyn_Datatypedecl*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->sc=Cyc_Absyn_Extern;_tmp1BA->name=tud->name;_tmp1BA->tvs=tvs;_tmp1BA->fields=0;_tmp1BA->is_extensible=tud->is_extensible;_tmp1BA;});
# 880
*_tmp1BF=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1BF,tvs,tud);
# 884
*_tmp1BE=_tmp1B9;
_tmp1BC=_tmp1BE;goto _LL101;}}}_LLF9:;};}
# 826 "tc.cyc"
;_pop_region(temp);};}
# 900 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 907
if(ed->fields != 0){struct _RegionHandle _tmp1C3=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1C3;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1C4=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 913
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1C4->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=0;_tmp1C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1C4->name).f2);({void*_tmp1C5[1]={& _tmp1C7};Cyc_Tcutil_terr(_tmp1C4->loc,({const char*_tmp1C6="duplicate enum constructor %s";_tag_dyneither(_tmp1C6,sizeof(char),30);}),_tag_dyneither(_tmp1C5,sizeof(void*),1));});});else{
# 916
prev_fields=({struct Cyc_List_List*_tmp1C8=_region_malloc(uprev_rgn,sizeof(*_tmp1C8));_tmp1C8->hd=(*_tmp1C4->name).f2;_tmp1C8->tl=prev_fields;_tmp1C8;});}
# 918
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp1C4->name).f2)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1C4->name).f2);({void*_tmp1C9[1]={& _tmp1CB};Cyc_Tcutil_terr(_tmp1C4->loc,({const char*_tmp1CA="enum field name %s shadows global name";_tag_dyneither(_tmp1CA,sizeof(char),39);}),_tag_dyneither(_tmp1C9,sizeof(void*),1));});});
# 921
if(_tmp1C4->tag == 0){
_tmp1C4->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1C4->loc);
++ tag_count;}else{
# 926
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1C4->tag))){
struct _tuple11 _tmp1CC=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1C4->tag));struct _tuple11 _tmp1CD=_tmp1CC;unsigned int _tmp1CF;int _tmp1CE;_LL103: _tmp1CF=_tmp1CD.f1;_tmp1CE=_tmp1CD.f2;_LL104:;
if(_tmp1CE)tag_count=_tmp1CF + 1;}}}}
# 908
;_pop_region(uprev_rgn);}
# 934
{struct _handler_cons _tmp1D0;_push_handler(& _tmp1D0);{int _tmp1D2=0;if(setjmp(_tmp1D0.handler))_tmp1D2=1;if(!_tmp1D2){
{struct Cyc_Absyn_Enumdecl**_tmp1D3=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp1D4=Cyc_Tcdecl_merge_enumdecl(*_tmp1D3,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1D4 == 0){_npop_handler(0);return;}
*_tmp1D3=_tmp1D4;}
# 935
;_pop_handler();}else{void*_tmp1D1=(void*)_exn_thrown;void*_tmp1D5=_tmp1D1;void*_tmp1D8;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1D5)->tag == Cyc_Dict_Absent){_LL106: _LL107: {
# 940
struct Cyc_Absyn_Enumdecl**_tmp1D6=({struct Cyc_Absyn_Enumdecl**_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=ed;_tmp1D7;});
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp1D6);
goto _LL105;}}else{_LL108: _tmp1D8=_tmp1D5;_LL109:(int)_rethrow(_tmp1D8);}_LL105:;}};}
# 944
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1D9=(struct Cyc_Absyn_Enumfield*)fs->hd;
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1D9->name).f2,({struct _tuple14*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->f1=(void*)({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp1DC;_tmp1DC.tag=3;_tmp1DC.f1=ed;_tmp1DC.f2=_tmp1D9;_tmp1DC;});_tmp1DB;});_tmp1DA->f2=1;_tmp1DA;}));}}
# 953
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1DD=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1DD->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1DD->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0;_tmp1E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1DD->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1E0;_tmp1E0.tag=0;_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp1DE[2]={& _tmp1E0,& _tmp1E1};Cyc_Tcutil_terr(loc,({const char*_tmp1DF="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1DF,sizeof(char),46);}),_tag_dyneither(_tmp1DE,sizeof(void*),2));});});});}}}
# 963
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1E2=sc;switch(_tmp1E2){case Cyc_Absyn_Static: _LL10B: _LL10C:
# 966
 if(!in_include)
({void*_tmp1E3=0;Cyc_Tcutil_warn(loc,({const char*_tmp1E4="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1E4,sizeof(char),44);}),_tag_dyneither(_tmp1E3,sizeof(void*),0));});
return 0;case Cyc_Absyn_Abstract: _LL10D: _LL10E:
# 970
({void*_tmp1E5=0;Cyc_Tcutil_warn(loc,({const char*_tmp1E6="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1E6,sizeof(char),46);}),_tag_dyneither(_tmp1E5,sizeof(void*),0));});
return 0;case Cyc_Absyn_Public: _LL10F: _LL110:
 goto _LL112;case Cyc_Absyn_Register: _LL111: _LL112:
 goto _LL114;case Cyc_Absyn_Extern: _LL113: _LL114:
 return 1;default: _LL115: _LL116:
# 976
({void*_tmp1E7=0;Cyc_Tcutil_warn(loc,({const char*_tmp1E8="nested extern \"C\" declaration";_tag_dyneither(_tmp1E8,sizeof(char),30);}),_tag_dyneither(_tmp1E7,sizeof(void*),0));});
return 1;}_LL10A:;}
# 986
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 992
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 995
struct Cyc_List_List*_tmp1E9=ds0;for(0;_tmp1E9 != 0;(last=_tmp1E9,_tmp1E9=_tmp1E9->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1E9->hd;
unsigned int loc=d->loc;
# 999
void*_tmp1EA=d->r;void*_tmp1EB=_tmp1EA;struct Cyc_List_List*_tmp208;struct Cyc_List_List*_tmp207;struct Cyc_List_List*_tmp206;union Cyc_Absyn_Nmspace _tmp205;struct _dyneither_ptr*_tmp204;struct Cyc_List_List*_tmp203;struct _dyneither_ptr*_tmp202;struct Cyc_List_List*_tmp201;struct Cyc_Absyn_Enumdecl*_tmp200;struct Cyc_Absyn_Datatypedecl*_tmp1FF;struct Cyc_Absyn_Aggrdecl*_tmp1FE;struct Cyc_Absyn_Typedefdecl*_tmp1FD;struct Cyc_Absyn_Fndecl*_tmp1FC;struct Cyc_Absyn_Vardecl*_tmp1FB;switch(*((int*)_tmp1EB)){case 2: _LL118: _LL119:
 goto _LL11B;case 3: _LL11A: _LL11B:
# 1002
({void*_tmp1EC=0;Cyc_Tcutil_terr(loc,({const char*_tmp1ED="top level let-declarations are not implemented";_tag_dyneither(_tmp1ED,sizeof(char),47);}),_tag_dyneither(_tmp1EC,sizeof(void*),0));});
goto _LL117;case 4: _LL11C: _LL11D:
# 1005
({void*_tmp1EE=0;Cyc_Tcutil_terr(loc,({const char*_tmp1EF="top level region declarations are not implemented";_tag_dyneither(_tmp1EF,sizeof(char),50);}),_tag_dyneither(_tmp1EE,sizeof(void*),0));});
goto _LL117;case 0: _LL11E: _tmp1FB=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_LL11F:
# 1008
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1FB->sc,te->in_extern_c_include))
_tmp1FB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp1FB,check_var_init,te->in_extern_c_include,exports);
goto _LL117;case 1: _LL120: _tmp1FC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_LL121:
# 1013
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1FC->sc,te->in_extern_c_include))
_tmp1FC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp1FC,exports);
goto _LL117;case 8: _LL122: _tmp1FD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_LL123:
# 1018
 _tmp1FD->extern_c=te->in_extern_c_include;
# 1022
Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp1FD);
goto _LL117;case 5: _LL124: _tmp1FE=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_LL125:
# 1025
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1FE->sc,te->in_extern_c_include))
_tmp1FE->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp1FE);
goto _LL117;case 6: _LL126: _tmp1FF=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_LL127:
# 1030
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1FF->sc,te->in_extern_c_include))
_tmp1FF->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp1FF);
goto _LL117;case 7: _LL128: _tmp200=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_LL129:
# 1035
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp200->sc,te->in_extern_c_include))
_tmp200->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp200);
goto _LL117;case 13: _LL12A: _LL12B:
# 1040
({void*_tmp1F0=0;Cyc_Tcutil_warn(d->loc,({const char*_tmp1F1="spurious __cyclone_port_on__";_tag_dyneither(_tmp1F1,sizeof(char),29);}),_tag_dyneither(_tmp1F0,sizeof(void*),0));});
goto _LL117;case 14: _LL12C: _LL12D:
# 1043
 goto _LL117;case 9: _LL12E: _tmp202=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_tmp201=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1EB)->f2;_LL12F: {
# 1046
struct Cyc_List_List*_tmp1F2=te->ns;
struct Cyc_List_List*_tmp1F3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->hd=_tmp202;_tmp1F4->tl=0;_tmp1F4;}));
# 1050
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(te->ae,_tmp1F3))
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp1F3,Cyc_Tcenv_empty_genv(grgn));
# 1054
te->ns=_tmp1F3;
Cyc_Tc_tc_decls(te,_tmp201,in_externC,check_var_init,grgn,exports);
te->ns=_tmp1F2;
goto _LL117;}case 10: _LL130: _tmp205=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1)->f1;_tmp204=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1)->f2;_tmp203=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1EB)->f2;_LL131:
# 1061
 Cyc_Tc_tc_decls(te,_tmp203,in_externC,check_var_init,grgn,exports);
goto _LL117;case 11: _LL132: _tmp206=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_LL133:
# 1065
 Cyc_Tc_tc_decls(te,_tmp206,1,check_var_init,grgn,exports);
goto _LL117;default: _LL134: _tmp208=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1EB)->f1;_tmp207=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1EB)->f2;_LL135: {
# 1069
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp207,(unsigned int)exports?*exports: 0);
{struct _RegionHandle _tmp1F5=_new_region("temp");struct _RegionHandle*temp=& _tmp1F5;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp1F6=Cyc_Tcenv_enter_extern_c_include(temp,te);
Cyc_Tc_tc_decls(_tmp1F6,_tmp208,1,check_var_init,grgn,& newexs);
# 1075
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
struct _tuple12*_tmp1F7=(struct _tuple12*)_tmp207->hd;
if(!(*_tmp1F7).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1F7).f2));({void*_tmp1F8[1]={& _tmp1FA};Cyc_Tcutil_warn((*_tmp1F7).f1,({const char*_tmp1F9="%s is exported but not defined";_tag_dyneither(_tmp1F9,sizeof(char),31);}),_tag_dyneither(_tmp1F8,sizeof(void*),1));});});}}
# 1072
;_pop_region(temp);}
# 1082
goto _LL117;}}_LL117:;}}
# 1087
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1089
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1106 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1108
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1110
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple18{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1114
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp209=d->r;void*_tmp20A=_tmp209;struct Cyc_List_List**_tmp21A;struct Cyc_List_List**_tmp219;struct Cyc_List_List**_tmp218;struct Cyc_List_List**_tmp217;struct Cyc_Absyn_Vardecl*_tmp216;switch(*((int*)_tmp20A)){case 0: _LL137: _tmp216=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL138:
# 1119
 if((env->in_cinclude  || _tmp216->sc != Cyc_Absyn_Extern  && _tmp216->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp216->name,Cyc_Absyn_uniquergn_qvar))
# 1122
return 1;{
# 1124
struct _tuple0*_tmp20B=_tmp216->name;struct _tuple0*_tmp20C=_tmp20B;union Cyc_Absyn_Nmspace _tmp215;struct _dyneither_ptr*_tmp214;_LL144: _tmp215=_tmp20C->f1;_tmp214=_tmp20C->f2;_LL145:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp20D=_tmp215;struct Cyc_List_List*_tmp210;struct Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp20E;switch((_tmp20D.C_n).tag){case 4: _LL147: _LL148:
 ns=0;goto _LL146;case 1: _LL149: _tmp20E=(_tmp20D.Rel_n).val;_LL14A:
 ns=_tmp20E;goto _LL146;case 3: _LL14B: _tmp20F=(_tmp20D.C_n).val;_LL14C:
 ns=_tmp20F;goto _LL146;default: _LL14D: _tmp210=(_tmp20D.Abs_n).val;_LL14E:
 ns=_tmp210;goto _LL146;}_LL146:;}{
# 1132
struct _tuple18*_tmp211=((struct _tuple18*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp212=(*_tmp211).f1;
int _tmp213=(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp212->ordinaries,_tmp214)).f2;
if(!_tmp213)
(*_tmp211).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp211).f2,_tmp214);
# 1140
return _tmp213;};};};case 11: _LL139: _tmp217=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL13A:
 _tmp218=_tmp217;goto _LL13C;case 10: _LL13B: _tmp218=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp20A)->f2;_LL13C:
 _tmp219=_tmp218;goto _LL13E;case 9: _LL13D: _tmp219=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp20A)->f2;_LL13E:
# 1144
*_tmp219=Cyc_Tc_treeshake_f(env,*_tmp219);
return 1;case 12: _LL13F: _tmp21A=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp20A)->f1;_LL140: {
# 1147
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp21A=Cyc_Tc_treeshake_f(env,*_tmp21A);
env->in_cinclude=in_cinclude;
return 1;}default: _LL141: _LL142:
 return 1;}_LL136:;}
# 1156
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1160
static struct _tuple18*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1162
return({struct _tuple18*_tmp21B=_region_malloc(rgn,sizeof(*_tmp21B));_tmp21B->f1=ge;_tmp21B->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp);_tmp21B;});}struct _tuple19{struct Cyc_List_List*f1;struct _tuple18*f2;};
# 1165
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp21C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp21C;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp21D=({struct Cyc_Tc_TreeshakeEnv _tmp225;_tmp225.rgn=rgn;_tmp225.in_cinclude=0;_tmp225.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple18*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae);_tmp225;});
struct Cyc_List_List*_tmp21E=Cyc_Tc_treeshake_f(& _tmp21D,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp21D.nsdict)){
struct Cyc_List_List*_tmp21F=_tmp21E;_npop_handler(0);return _tmp21F;}{
# 1173
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp21D.nsdict);
struct _tuple19 _tmp220=*((struct _tuple19*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp21D.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple19*))Cyc_Iter_next)(iter,& _tmp220)){
struct _tuple19 _tmp221=_tmp220;struct Cyc_Tcenv_Genv*_tmp223;struct Cyc_Set_Set*_tmp222;_LL150: _tmp223=(_tmp221.f2)->f1;_tmp222=(_tmp221.f2)->f2;_LL151:;
_tmp223->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple14*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp223->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple14*y))Cyc_Tc_treeshake_remove_f,_tmp222,_tmp223->ordinaries);}{
# 1179
struct Cyc_List_List*_tmp224=_tmp21E;_npop_handler(0);return _tmp224;};};}
# 1167
;_pop_region(rgn);}
