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
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
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
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 461 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 464
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 528
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 554
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 561
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 579
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 772 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 938
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 948
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 952
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 955
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 958
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 966
void*Cyc_Absyn_compress_kb(void*);
# 971
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 995
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 1017
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1028
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1100
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1188
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
# 120 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 127
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 132
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 136
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 139
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 201
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 229
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
# 228 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 230
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 242
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 314
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 338
int Cyc_Tcutil_supports_default(void*);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
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
# 34 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 36
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 40
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 43
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
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
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
# 47 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 52
int Cyc_Tc_aggressive_warn=0;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 54
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 60
(*_tmp1).f3=1;
return 1;}}
# 64
return 0;}
# 67
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
{const char*_tmp2B0;void*_tmp2AF[1];struct Cyc_String_pa_PrintArg_struct _tmp2AE;(_tmp2AE.tag=0,((_tmp2AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att)),((_tmp2AF[0]=& _tmp2AE,Cyc_Tcutil_terr(loc,((_tmp2B0="%s has an out-of-range index",_tag_dyneither(_tmp2B0,sizeof(char),29))),_tag_dyneither(_tmp2AF,sizeof(void*),1)))))));}
return 0;}
# 72
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 74
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 77
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i)){
const char*_tmp2B6;void*_tmp2B5[3];struct Cyc_String_pa_PrintArg_struct _tmp2B4;struct Cyc_String_pa_PrintArg_struct _tmp2B3;struct Cyc_Int_pa_PrintArg_struct _tmp2B2;(_tmp2B2.tag=1,((_tmp2B2.f1=(unsigned long)i,((_tmp2B3.tag=0,((_tmp2B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2),((_tmp2B4.tag=0,((_tmp2B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2),((_tmp2B5[0]=& _tmp2B4,((_tmp2B5[1]=& _tmp2B3,((_tmp2B5[2]=& _tmp2B2,Cyc_Tcutil_terr(loc,((_tmp2B6="incompatible %s() and %s() attributes on parameter %d",_tag_dyneither(_tmp2B6,sizeof(char),54))),_tag_dyneither(_tmp2B5,sizeof(void*),3)))))))))))))))))));}}
# 81
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmpA=_new_region("temp");struct _RegionHandle*temp=& _tmpA;_push_region(temp);
{const char*_tmp2B9;struct _tuple13 _tmp2B8;struct _tuple13 init_params=(_tmp2B8.f1=0,((_tmp2B8.f2=((_tmp2B9="initializes",_tag_dyneither(_tmp2B9,sizeof(char),12))),_tmp2B8)));
const char*_tmp2BC;struct _tuple13 _tmp2BB;struct _tuple13 nolive_unique_params=(_tmp2BB.f1=0,((_tmp2BB.f2=((_tmp2BC="noliveunique",_tag_dyneither(_tmp2BC,sizeof(char),13))),_tmp2BB)));
const char*_tmp2BF;struct _tuple13 _tmp2BE;struct _tuple13 noconsume_params=(_tmp2BE.f1=0,((_tmp2BE.f2=((_tmp2BF="noconsume",_tag_dyneither(_tmp2BF,sizeof(char),10))),_tmp2BE)));
void*_tmpB=Cyc_Tcutil_compress(t);void*_tmpC=_tmpB;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpF;_LL1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC;if(_tmpD->tag != 9)goto _LL3;else{_tmpE=(_tmpD->f1).attributes;_tmpF=(_tmpD->f1).args;}}_LL2: {
# 88
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF);
for(0;_tmpE != 0;_tmpE=_tmpE->tl){
void*_tmp10=(void*)_tmpE->hd;void*_tmp11=_tmp10;int _tmp13;int _tmp15;int _tmp17;_LL6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp12=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp11;if(_tmp12->tag != 20)goto _LL8;else{_tmp13=_tmp12->f1;}}_LL7:
# 92
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp13,nargs,(void*)_tmpE->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,init_params,noconsume_params);{
struct _tuple8*_tmp18=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpF,_tmp13 - 1);void*_tmp1A;struct _tuple8*_tmp19=_tmp18;_tmp1A=_tmp19->f3;{
const char*_tmp2C0;struct _dyneither_ptr s=(_tmp2C0="initializes attribute allowed only on",_tag_dyneither(_tmp2C0,sizeof(char),38));
{void*_tmp1B=Cyc_Tcutil_compress(_tmp1A);void*_tmp1C=_tmp1B;void*_tmp1E;union Cyc_Absyn_Constraint*_tmp1F;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp21;_LLF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C;if(_tmp1D->tag != 5)goto _LL11;else{_tmp1E=(_tmp1D->f1).elt_typ;_tmp1F=((_tmp1D->f1).ptr_atts).nullable;_tmp20=((_tmp1D->f1).ptr_atts).bounds;_tmp21=((_tmp1D->f1).ptr_atts).zero_term;}}_LL10:
# 99
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1F)){
const char*_tmp2C4;void*_tmp2C3[1];struct Cyc_String_pa_PrintArg_struct _tmp2C2;(_tmp2C2.tag=0,((_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2C3[0]=& _tmp2C2,Cyc_Tcutil_terr(loc,((_tmp2C4="%s non-null pointers",_tag_dyneither(_tmp2C4,sizeof(char),21))),_tag_dyneither(_tmp2C3,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_bound_one(_tmp20)){
const char*_tmp2C8;void*_tmp2C7[1];struct Cyc_String_pa_PrintArg_struct _tmp2C6;(_tmp2C6.tag=0,((_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2C7[0]=& _tmp2C6,Cyc_Tcutil_terr(loc,((_tmp2C8="%s pointers of size 1",_tag_dyneither(_tmp2C8,sizeof(char),22))),_tag_dyneither(_tmp2C7,sizeof(void*),1)))))));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp21)){
const char*_tmp2CC;void*_tmp2CB[1];struct Cyc_String_pa_PrintArg_struct _tmp2CA;(_tmp2CA.tag=0,((_tmp2CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2CB[0]=& _tmp2CA,Cyc_Tcutil_terr(loc,((_tmp2CC="%s pointers to non-zero-terminated arrays",_tag_dyneither(_tmp2CC,sizeof(char),42))),_tag_dyneither(_tmp2CB,sizeof(void*),1)))))));}
goto _LLE;_LL11:;_LL12: {
# 107
const char*_tmp2D0;void*_tmp2CF[1];struct Cyc_String_pa_PrintArg_struct _tmp2CE;(_tmp2CE.tag=0,((_tmp2CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2CF[0]=& _tmp2CE,Cyc_Tcutil_terr(loc,((_tmp2D0="%s pointers",_tag_dyneither(_tmp2D0,sizeof(char),12))),_tag_dyneither(_tmp2CF,sizeof(void*),1)))))));}_LLE:;}
# 109
{struct Cyc_List_List*_tmp2D1;init_params.f1=((_tmp2D1=_region_malloc(temp,sizeof(*_tmp2D1)),((_tmp2D1->hd=(void*)_tmp13,((_tmp2D1->tl=init_params.f1,_tmp2D1))))));}
goto _LL5;};};_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp14=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp11;if(_tmp14->tag != 21)goto _LLA;else{_tmp15=_tmp14->f1;}}_LL9:
# 112
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp15,nargs,(void*)_tmpE->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp15,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp15,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp30=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpF,_tmp15 - 1);void*_tmp32;struct _tuple8*_tmp31=_tmp30;_tmp32=_tmp31->f3;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp32,0)){
const char*_tmp2D4;void*_tmp2D3;(_tmp2D3=0,Cyc_Tcutil_terr(loc,((_tmp2D4="noliveunique attribute allowed only on unique pointers",_tag_dyneither(_tmp2D4,sizeof(char),55))),_tag_dyneither(_tmp2D3,sizeof(void*),0)));}
{struct Cyc_List_List*_tmp2D5;nolive_unique_params.f1=((_tmp2D5=_region_malloc(temp,sizeof(*_tmp2D5)),((_tmp2D5->hd=(void*)_tmp15,((_tmp2D5->tl=nolive_unique_params.f1,_tmp2D5))))));}
goto _LL5;};_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp16=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp11;if(_tmp16->tag != 22)goto _LLC;else{_tmp17=_tmp16->f1;}}_LLB:
# 121
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp17,nargs,(void*)_tmpE->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp17,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp17,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp36=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpF,_tmp17 - 1);void*_tmp38;struct _tuple8*_tmp37=_tmp36;_tmp38=_tmp37->f3;
# 126
{struct Cyc_List_List*_tmp2D6;noconsume_params.f1=((_tmp2D6=_region_malloc(temp,sizeof(*_tmp2D6)),((_tmp2D6->hd=(void*)_tmp17,((_tmp2D6->tl=noconsume_params.f1,_tmp2D6))))));}
goto _LL5;};_LLC:;_LLD:
 goto _LL5;_LL5:;}
# 131
goto _LL0;}_LL3:;_LL4: {
const char*_tmp2D9;void*_tmp2D8;(_tmp2D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2D9="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp2D9,sizeof(char),34))),_tag_dyneither(_tmp2D8,sizeof(void*),0)));}_LL0:;}
# 83
;_pop_region(temp);}struct _tuple14{void*f1;int f2;};
# 137
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 141
enum Cyc_Absyn_Scope _tmp43;union Cyc_Absyn_Nmspace _tmp44;struct _dyneither_ptr*_tmp45;void*_tmp46;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_List_List*_tmp48;struct Cyc_Absyn_Vardecl*_tmp42=vd;_tmp43=_tmp42->sc;_tmp44=(_tmp42->name)->f1;_tmp45=(_tmp42->name)->f2;_tmp46=_tmp42->type;_tmp47=_tmp42->initializer;_tmp48=_tmp42->attributes;
# 147
{union Cyc_Absyn_Nmspace _tmp49=_tmp44;_LL14: if((_tmp49.Rel_n).tag != 1)goto _LL16;_LL15:
# 150
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL13;_LL16:;_LL17:
 goto _LL13;_LL13:;}
# 156
{void*_tmp4A=Cyc_Tcutil_compress(_tmp46);void*_tmp4B=_tmp4A;void*_tmp4D;struct Cyc_Absyn_Tqual _tmp4E;union Cyc_Absyn_Constraint*_tmp4F;unsigned int _tmp50;_LL19:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B;if(_tmp4C->tag != 8)goto _LL1B;else{_tmp4D=(_tmp4C->f1).elt_type;_tmp4E=(_tmp4C->f1).tq;if((_tmp4C->f1).num_elts != 0)goto _LL1B;_tmp4F=(_tmp4C->f1).zero_term;_tmp50=(_tmp4C->f1).zt_loc;}}if(!(_tmp47 != 0))goto _LL1B;_LL1A:
# 158
{void*_tmp51=_tmp47->r;void*_tmp52=_tmp51;struct _dyneither_ptr _tmp54;struct _dyneither_ptr _tmp56;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5E;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp53=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp53->tag != 0)goto _LL20;else{if(((_tmp53->f1).String_c).tag != 8)goto _LL20;_tmp54=(struct _dyneither_ptr)((_tmp53->f1).String_c).val;}}_LL1F:
# 160
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp54,sizeof(char)),0),_tmp4F,_tmp50));
goto _LL1D;_LL20: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp55=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp55->tag != 0)goto _LL22;else{if(((_tmp55->f1).Wstring_c).tag != 9)goto _LL22;_tmp56=(struct _dyneither_ptr)((_tmp55->f1).Wstring_c).val;}}_LL21:
# 163
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,Cyc_Absyn_uint_exp(1,0),_tmp4F,_tmp50));
goto _LL1D;_LL22: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp57=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp57->tag != 26)goto _LL24;else{_tmp58=_tmp57->f2;}}_LL23:
 _tmp5A=_tmp58;goto _LL25;_LL24: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp59=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp59->tag != 27)goto _LL26;else{_tmp5A=_tmp59->f1;}}_LL25:
# 168
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,_tmp5A,_tmp4F,_tmp50));
goto _LL1D;_LL26: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp5B->tag != 35)goto _LL28;else{_tmp5C=_tmp5B->f2;}}_LL27:
 _tmp5E=_tmp5C;goto _LL29;_LL28: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp5D->tag != 25)goto _LL2A;else{_tmp5E=_tmp5D->f1;}}_LL29:
# 172
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5E),0),_tmp4F,_tmp50));
goto _LL1D;_LL2A:;_LL2B:
 goto _LL1D;_LL1D:;}
# 176
goto _LL18;_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 180
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp46);
# 182
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp46);
# 185
{void*_tmp5F=Cyc_Tcutil_compress(_tmp46);void*_tmp60=_tmp5F;_LL2D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60;if(_tmp61->tag != 8)goto _LL2F;}_LL2E:
 vd->escapes=0;goto _LL2C;_LL2F:;_LL30:
 vd->escapes=1;goto _LL2C;_LL2C:;}
# 190
if(Cyc_Tcutil_is_function_type(_tmp46)){
_tmp48=Cyc_Tcutil_transfer_fn_type_atts(_tmp46,_tmp48);
Cyc_Tc_fnTypeAttsOK(te,_tmp46,loc);}
# 195
if(_tmp43 == Cyc_Absyn_Extern  || _tmp43 == Cyc_Absyn_ExternC){
if(_tmp47 != 0  && !in_cinclude){
const char*_tmp2DC;void*_tmp2DB;(_tmp2DB=0,Cyc_Tcutil_terr(loc,((_tmp2DC="extern declaration should not have initializer",_tag_dyneither(_tmp2DC,sizeof(char),47))),_tag_dyneither(_tmp2DB,sizeof(void*),0)));}}else{
if(!Cyc_Tcutil_is_function_type(_tmp46)){
# 202
for(0;_tmp48 != 0;_tmp48=_tmp48->tl){
void*_tmp64=(void*)_tmp48->hd;void*_tmp65=_tmp64;_LL32: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp66=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp65;if(_tmp66->tag != 6)goto _LL34;}_LL33:
 goto _LL35;_LL34: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp67=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp65;if(_tmp67->tag != 8)goto _LL36;}_LL35:
# 207
 goto _LL37;_LL36: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp68=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp65;if(_tmp68->tag != 9)goto _LL38;}_LL37:
 goto _LL39;_LL38: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp69=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp65;if(_tmp69->tag != 10)goto _LL3A;}_LL39:
 goto _LL3B;_LL3A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp6A=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6A->tag != 11)goto _LL3C;}_LL3B:
 goto _LL3D;_LL3C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp6B=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6B->tag != 12)goto _LL3E;}_LL3D:
 goto _LL3F;_LL3E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp6C=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6C->tag != 13)goto _LL40;}_LL3F:
 goto _LL41;_LL40: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp6D=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6D->tag != 14)goto _LL42;}_LL41:
 continue;_LL42:;_LL43:
# 215
{const char*_tmp2E1;void*_tmp2E0[2];struct Cyc_String_pa_PrintArg_struct _tmp2DF;struct Cyc_String_pa_PrintArg_struct _tmp2DE;(_tmp2DE.tag=0,((_tmp2DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2DF.tag=0,((_tmp2DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp48->hd)),((_tmp2E0[0]=& _tmp2DF,((_tmp2E0[1]=& _tmp2DE,Cyc_Tcutil_terr(loc,((_tmp2E1="bad attribute %s for variable %s",_tag_dyneither(_tmp2E1,sizeof(char),33))),_tag_dyneither(_tmp2E0,sizeof(void*),2)))))))))))));}
goto _LL31;_LL31:;}
# 220
if(_tmp47 == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp46)){
const char*_tmp2E6;void*_tmp2E5[2];struct Cyc_String_pa_PrintArg_struct _tmp2E4;struct Cyc_String_pa_PrintArg_struct _tmp2E3;(_tmp2E3.tag=0,((_tmp2E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp46)),((_tmp2E4.tag=0,((_tmp2E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2E5[0]=& _tmp2E4,((_tmp2E5[1]=& _tmp2E3,Cyc_Tcutil_terr(loc,((_tmp2E6="initializer required for variable %s of type %s",_tag_dyneither(_tmp2E6,sizeof(char),48))),_tag_dyneither(_tmp2E5,sizeof(void*),2)))))))))))));}}else{
# 225
struct Cyc_Absyn_Exp*_tmp76=_tmp47;
# 229
struct _handler_cons _tmp77;_push_handler(& _tmp77);{int _tmp79=0;if(setjmp(_tmp77.handler))_tmp79=1;if(!_tmp79){
{void*_tmp7A=Cyc_Tcexp_tcExpInitializer(te,& _tmp46,_tmp76);
if(!Cyc_Tcutil_coerce_assign(te,_tmp76,_tmp46)){
struct _dyneither_ptr _tmp7B=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp7C=" declared with type ";
struct _dyneither_ptr _tmp7D=Cyc_Absynpp_typ2string(_tmp46);
const char*_tmp7E=" but initialized with type ";
struct _dyneither_ptr _tmp7F=Cyc_Absynpp_typ2string(_tmp7A);
{const char*_tmp2E8;const char*_tmp2E7;if((((Cyc_strlen((struct _dyneither_ptr)_tmp7B)+ Cyc_strlen(((_tmp2E7=_tmp7C,_tag_dyneither(_tmp2E7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E7,21))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp7D))+ Cyc_strlen(((_tmp2E8=_tmp7E,_tag_dyneither(_tmp2E8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E8,28))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp7F)> 70){
const char*_tmp2F4;void*_tmp2F3[5];struct Cyc_String_pa_PrintArg_struct _tmp2F2;const char*_tmp2F1;struct Cyc_String_pa_PrintArg_struct _tmp2F0;struct Cyc_String_pa_PrintArg_struct _tmp2EF;const char*_tmp2EE;struct Cyc_String_pa_PrintArg_struct _tmp2ED;struct Cyc_String_pa_PrintArg_struct _tmp2EC;(_tmp2EC.tag=0,((_tmp2EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7F),((_tmp2ED.tag=0,((_tmp2ED.f1=(struct _dyneither_ptr)((_tmp2EE=_tmp7E,_tag_dyneither(_tmp2EE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EE,28)))),((_tmp2EF.tag=0,((_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7D),((_tmp2F0.tag=0,((_tmp2F0.f1=(struct _dyneither_ptr)((_tmp2F1=_tmp7C,_tag_dyneither(_tmp2F1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F1,21)))),((_tmp2F2.tag=0,((_tmp2F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7B),((_tmp2F3[0]=& _tmp2F2,((_tmp2F3[1]=& _tmp2F0,((_tmp2F3[2]=& _tmp2EF,((_tmp2F3[3]=& _tmp2ED,((_tmp2F3[4]=& _tmp2EC,Cyc_Tcutil_terr(loc,((_tmp2F4="%s%s\n\t%s\n%s\n\t%s",_tag_dyneither(_tmp2F4,sizeof(char),16))),_tag_dyneither(_tmp2F3,sizeof(void*),5)))))))))))))))))))))))))))))));}else{
# 240
const char*_tmp300;void*_tmp2FF[5];struct Cyc_String_pa_PrintArg_struct _tmp2FE;const char*_tmp2FD;struct Cyc_String_pa_PrintArg_struct _tmp2FC;struct Cyc_String_pa_PrintArg_struct _tmp2FB;const char*_tmp2FA;struct Cyc_String_pa_PrintArg_struct _tmp2F9;struct Cyc_String_pa_PrintArg_struct _tmp2F8;(_tmp2F8.tag=0,((_tmp2F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7F),((_tmp2F9.tag=0,((_tmp2F9.f1=(struct _dyneither_ptr)((_tmp2FA=_tmp7E,_tag_dyneither(_tmp2FA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FA,28)))),((_tmp2FB.tag=0,((_tmp2FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7D),((_tmp2FC.tag=0,((_tmp2FC.f1=(struct _dyneither_ptr)((_tmp2FD=_tmp7C,_tag_dyneither(_tmp2FD,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FD,21)))),((_tmp2FE.tag=0,((_tmp2FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7B),((_tmp2FF[0]=& _tmp2FE,((_tmp2FF[1]=& _tmp2FC,((_tmp2FF[2]=& _tmp2FB,((_tmp2FF[3]=& _tmp2F9,((_tmp2FF[4]=& _tmp2F8,Cyc_Tcutil_terr(loc,((_tmp300="%s%s%s%s%s",_tag_dyneither(_tmp300,sizeof(char),11))),_tag_dyneither(_tmp2FF,sizeof(void*),5)))))))))))))))))))))))))))))));}}
Cyc_Tcutil_explain_failure();
# 243
if(!Cyc_Tcutil_is_const_exp(_tmp76)){
const char*_tmp303;void*_tmp302;(_tmp302=0,Cyc_Tcutil_terr(loc,((_tmp303="initializer is not a constant expression",_tag_dyneither(_tmp303,sizeof(char),41))),_tag_dyneither(_tmp302,sizeof(void*),0)));}}}
# 230
;_pop_handler();}else{void*_tmp78=(void*)_exn_thrown;void*_tmp97=_tmp78;void*_tmp99;_LL45: {struct Cyc_Tcenv_Env_error_exn_struct*_tmp98=(struct Cyc_Tcenv_Env_error_exn_struct*)_tmp97;if(_tmp98->tag != Cyc_Tcenv_Env_error)goto _LL47;}_LL46:
# 248
{const char*_tmp306;void*_tmp305;(_tmp305=0,Cyc_Tcutil_terr(loc,((_tmp306="initializer is not a constant expression",_tag_dyneither(_tmp306,sizeof(char),41))),_tag_dyneither(_tmp305,sizeof(void*),0)));}
goto _LL44;_LL47: _tmp99=_tmp97;_LL48:(void)_rethrow(_tmp99);_LL44:;}};}}else{
# 254
for(0;_tmp48 != 0;_tmp48=_tmp48->tl){
void*_tmp9C=(void*)_tmp48->hd;void*_tmp9D=_tmp9C;_LL4A: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp9E=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp9D;if(_tmp9E->tag != 0)goto _LL4C;}_LL4B:
# 257
 goto _LL4D;_LL4C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp9F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp9D;if(_tmp9F->tag != 1)goto _LL4E;}_LL4D:
 goto _LL4F;_LL4E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA0->tag != 2)goto _LL50;}_LL4F:
 goto _LL51;_LL50: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA1->tag != 3)goto _LL52;}_LL51:
 goto _LL53;_LL52: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpA2=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA2->tag != 4)goto _LL54;}_LL53:
 goto _LL55;_LL54: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA3=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA3->tag != 19)goto _LL56;}_LL55:
 goto _LL57;_LL56: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA4=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA4->tag != 20)goto _LL58;}_LL57:
 goto _LL59;_LL58: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmpA5=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA5->tag != 23)goto _LL5A;}_LL59:
 goto _LL5B;_LL5A: {struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct*_tmpA6=(struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA6->tag != 26)goto _LL5C;}_LL5B:
 goto _LL5D;_LL5C: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmpA7=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA7->tag != 5)goto _LL5E;}_LL5D: {
const char*_tmp309;void*_tmp308;(_tmp308=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp309="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp309,sizeof(char),45))),_tag_dyneither(_tmp308,sizeof(void*),0)));}_LL5E: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA8=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA8->tag != 6)goto _LL60;}_LL5F:
# 268
 goto _LL61;_LL60: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpA9=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA9->tag != 7)goto _LL62;}_LL61:
# 270
{const char*_tmp30D;void*_tmp30C[1];struct Cyc_String_pa_PrintArg_struct _tmp30B;(_tmp30B.tag=0,((_tmp30B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp48->hd)),((_tmp30C[0]=& _tmp30B,Cyc_Tcutil_terr(loc,((_tmp30D="bad attribute %s in function declaration",_tag_dyneither(_tmp30D,sizeof(char),41))),_tag_dyneither(_tmp30C,sizeof(void*),1)))))));}
goto _LL49;_LL62:;_LL63:
 continue;_LL49:;}}}
# 278
{struct _handler_cons _tmpAF;_push_handler(& _tmpAF);{int _tmpB1=0;if(setjmp(_tmpAF.handler))_tmpB1=1;if(!_tmpB1){
{struct _tuple14*_tmpB2=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp45);
void*_tmpB3=(*_tmpB2).f1;void*_tmpB4=_tmpB3;void*_tmpB6;_LL65: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB5=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB4;if(_tmpB5->tag != 0)goto _LL67;else{_tmpB6=(void*)_tmpB5->f1;}}_LL66: {
# 282
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp310;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp30F;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpBB=(_tmp30F=_cycalloc(sizeof(*_tmp30F)),((_tmp30F[0]=((_tmp310.tag=1,((_tmp310.f1=vd,_tmp310)))),_tmp30F)));
void*_tmpBC=Cyc_Tcdecl_merge_binding(_tmpB6,(void*)_tmpBB,loc,Cyc_Tc_tc_msg);
if(_tmpBC == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 286
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmpBC != _tmpB6  || (*_tmpB2).f2){
# 289
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp316;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp315;struct _tuple14*_tmp314;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp45,(
(_tmp314=_cycalloc(sizeof(*_tmp314)),((_tmp314->f1=(void*)((_tmp316=_cycalloc(sizeof(*_tmp316)),((_tmp316[0]=((_tmp315.tag=0,((_tmp315.f1=_tmpBC,_tmp315)))),_tmp316)))),((_tmp314->f2=1,_tmp314)))))));}}
_npop_handler(0);return;}_LL67: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB7=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB4;if(_tmpB7->tag != 1)goto _LL69;}_LL68:
# 293
 if(Cyc_Tc_aggressive_warn){
const char*_tmp319;void*_tmp318;(_tmp318=0,Cyc_Tcutil_warn(loc,((_tmp319="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp319,sizeof(char),57))),_tag_dyneither(_tmp318,sizeof(void*),0)));}
goto _LL64;_LL69: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB8=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB4;if(_tmpB8->tag != 2)goto _LL6B;}_LL6A:
# 297
 if(Cyc_Tc_aggressive_warn){
const char*_tmp31C;void*_tmp31B;(_tmp31B=0,Cyc_Tcutil_warn(loc,((_tmp31C="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp31C,sizeof(char),59))),_tag_dyneither(_tmp31B,sizeof(void*),0)));}
goto _LL64;_LL6B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpB9=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpB4;if(_tmpB9->tag != 4)goto _LL6D;}_LL6C:
 goto _LL6E;_LL6D: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpBA=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpB4;if(_tmpBA->tag != 3)goto _LL64;}_LL6E:
# 302
 if(Cyc_Tc_aggressive_warn){
const char*_tmp31F;void*_tmp31E;(_tmp31E=0,Cyc_Tcutil_warn(loc,((_tmp31F="variable declaration shadows previous enum tag",_tag_dyneither(_tmp31F,sizeof(char),47))),_tag_dyneither(_tmp31E,sizeof(void*),0)));}
goto _LL64;_LL64:;}
# 279
;_pop_handler();}else{void*_tmpB0=(void*)_exn_thrown;void*_tmpC9=_tmpB0;void*_tmpCB;_LL70: {struct Cyc_Dict_Absent_exn_struct*_tmpCA=(struct Cyc_Dict_Absent_exn_struct*)_tmpC9;if(_tmpCA->tag != Cyc_Dict_Absent)goto _LL72;}_LL71:
# 306
 goto _LL6F;_LL72: _tmpCB=_tmpC9;_LL73:(void)_rethrow(_tmpCB);_LL6F:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp32E;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp32D;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp32C;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp32B;struct _tuple14*_tmp32A;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp45,(
(_tmp32A=_cycalloc(sizeof(*_tmp32A)),((_tmp32A->f1=(void*)((_tmp32E=_cycalloc(sizeof(*_tmp32E)),((_tmp32E[0]=((_tmp32B.tag=0,((_tmp32B.f1=(void*)((_tmp32D=_cycalloc(sizeof(*_tmp32D)),((_tmp32D[0]=((_tmp32C.tag=1,((_tmp32C.f1=vd,_tmp32C)))),_tmp32D)))),_tmp32B)))),_tmp32E)))),((_tmp32A->f2=0,_tmp32A)))))));}}
# 312
static int Cyc_Tc_is_main(struct _tuple0*n){
union Cyc_Absyn_Nmspace _tmpD2;struct _dyneither_ptr*_tmpD3;struct _tuple0*_tmpD1=n;_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;{
union Cyc_Absyn_Nmspace _tmpD4=_tmpD2;_LL75: if((_tmpD4.Abs_n).tag != 2)goto _LL77;if((struct Cyc_List_List*)(_tmpD4.Abs_n).val != 0)goto _LL77;_LL76: {
# 316
const char*_tmp32F;return Cyc_strcmp((struct _dyneither_ptr)*_tmpD3,((_tmp32F="main",_tag_dyneither(_tmp32F,sizeof(char),5))))== 0;}_LL77:;_LL78:
 return 0;_LL74:;};}
# 321
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 323
struct _dyneither_ptr*v=(*fd->name).f2;
# 327
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include){
const char*_tmp332;void*_tmp331;(_tmp331=0,Cyc_Tcutil_terr(loc,((_tmp332="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp332,sizeof(char),54))),_tag_dyneither(_tmp331,sizeof(void*),0)));}
# 330
{union Cyc_Absyn_Nmspace _tmpD8=(*fd->name).f1;union Cyc_Absyn_Nmspace _tmpD9=_tmpD8;_LL7A: if((_tmpD9.Rel_n).tag != 1)goto _LL7C;if((struct Cyc_List_List*)(_tmpD9.Rel_n).val != 0)goto _LL7C;_LL7B:
# 333
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || te->in_extern_c_include);
goto _LL79;_LL7C:;_LL7D:
# 336
 goto _LL79;_LL79:;}
# 339
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 341
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 343
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 346
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpDA=(void*)atts->hd;void*_tmpDB=_tmpDA;_LL7F: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpDC=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpDB;if(_tmpDC->tag != 7)goto _LL81;}_LL80:
 goto _LL82;_LL81: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpDD=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpDB;if(_tmpDD->tag != 6)goto _LL83;}_LL82:
# 350
{const char*_tmp336;void*_tmp335[1];struct Cyc_String_pa_PrintArg_struct _tmp334;(_tmp334.tag=0,((_tmp334.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp335[0]=& _tmp334,Cyc_Tcutil_terr(loc,((_tmp336="bad attribute %s for function",_tag_dyneither(_tmp336,sizeof(char),30))),_tag_dyneither(_tmp335,sizeof(void*),1)))))));}
goto _LL7E;_LL83:;_LL84:
 goto _LL7E;_LL7E:;}}
# 356
{struct _handler_cons _tmpE1;_push_handler(& _tmpE1);{int _tmpE3=0;if(setjmp(_tmpE1.handler))_tmpE3=1;if(!_tmpE3){
{struct _tuple14*_tmpE4=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_tmpE5=(*_tmpE4).f1;void*_tmpE6=_tmpE5;void*_tmpE8;_LL86: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE7=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE6;if(_tmpE7->tag != 0)goto _LL88;else{_tmpE8=(void*)_tmpE7->f1;}}_LL87: {
# 360
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp339;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp338;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpED=(_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338[0]=((_tmp339.tag=2,((_tmp339.f1=fd,_tmp339)))),_tmp338)));
void*_tmpEE=Cyc_Tcdecl_merge_binding(_tmpE8,(void*)_tmpED,loc,Cyc_Tc_tc_msg);
if(_tmpEE == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL85;
# 364
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpEE == _tmpE8  && (*_tmpE4).f2)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp33F;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp33E;struct _tuple14*_tmp33D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp33D=_cycalloc(sizeof(*_tmp33D)),((_tmp33D->f1=(void*)((_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F[0]=((_tmp33E.tag=0,((_tmp33E.f1=_tmpEE,_tmp33E)))),_tmp33F)))),((_tmp33D->f2=1,_tmp33D)))))));}}
goto _LL85;}_LL88: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpE9=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpE6;if(_tmpE9->tag != 1)goto _LL8A;}_LL89:
# 370
{const char*_tmp342;void*_tmp341;(_tmp341=0,Cyc_Tcutil_warn(loc,((_tmp342="function declaration shadows previous type declaration",_tag_dyneither(_tmp342,sizeof(char),55))),_tag_dyneither(_tmp341,sizeof(void*),0)));}
goto _LL85;_LL8A: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpEA=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpE6;if(_tmpEA->tag != 2)goto _LL8C;}_LL8B:
# 373
{const char*_tmp345;void*_tmp344;(_tmp344=0,Cyc_Tcutil_warn(loc,((_tmp345="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp345,sizeof(char),59))),_tag_dyneither(_tmp344,sizeof(void*),0)));}
goto _LL85;_LL8C: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpEB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpE6;if(_tmpEB->tag != 4)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpEC=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpE6;if(_tmpEC->tag != 3)goto _LL85;}_LL8F:
# 377
{const char*_tmp348;void*_tmp347;(_tmp347=0,Cyc_Tcutil_warn(loc,((_tmp348="function declaration shadows previous enum tag",_tag_dyneither(_tmp348,sizeof(char),47))),_tag_dyneither(_tmp347,sizeof(void*),0)));}
goto _LL85;_LL85:;}
# 357
;_pop_handler();}else{void*_tmpE2=(void*)_exn_thrown;void*_tmpFB=_tmpE2;void*_tmpFD;_LL91: {struct Cyc_Dict_Absent_exn_struct*_tmpFC=(struct Cyc_Dict_Absent_exn_struct*)_tmpFB;if(_tmpFC->tag != Cyc_Dict_Absent)goto _LL93;}_LL92:
# 381
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp34B;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp34A;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpFE=(_tmp34A=_cycalloc(sizeof(*_tmp34A)),((_tmp34A[0]=((_tmp34B.tag=2,((_tmp34B.f1=fd,_tmp34B)))),_tmp34A)));
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp35A;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp359;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp358;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp357;struct _tuple14*_tmp356;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356->f1=(void*)((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=((_tmp357.tag=0,((_tmp357.f1=(void*)((_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359[0]=((_tmp358.tag=2,((_tmp358.f1=fd,_tmp358)))),_tmp359)))),_tmp357)))),_tmp35A)))),((_tmp356->f2=0,_tmp356)))))));}
# 386
goto _LL90;_LL93: _tmpFD=_tmpFB;_LL94:(void)_rethrow(_tmpFD);_LL90:;}};}
# 390
if(te->in_extern_c_include)return;{
# 395
struct _RegionHandle _tmp106=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmp106;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmp107=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmp35B;struct Cyc_Tcenv_Tenv*_tmp108=(_tmp35B=_region_malloc(fnrgn,sizeof(*_tmp35B)),((_tmp35B->ns=te->ns,((_tmp35B->ae=te->ae,((_tmp35B->le=_tmp107,((_tmp35B->allow_valueof=0,((_tmp35B->in_extern_c_include=0,_tmp35B)))))))))));
# 399
struct _handler_cons _tmp109;_push_handler(& _tmp109);{int _tmp10B=0;if(setjmp(_tmp109.handler))_tmp10B=1;if(!_tmp10B){
Cyc_Tcstmt_tcStmt(_tmp108,fd->body,0);
# 403
Cyc_Tcenv_check_delayed_effects(_tmp108);
Cyc_Tcenv_check_delayed_constraints(_tmp108);
# 407
if(!Cyc_Tcenv_all_labels_resolved(_tmp108)){
const char*_tmp35E;void*_tmp35D;(_tmp35D=0,Cyc_Tcutil_terr(loc,((_tmp35E="function has goto statements to undefined labels",_tag_dyneither(_tmp35E,sizeof(char),49))),_tag_dyneither(_tmp35D,sizeof(void*),0)));}
# 400
;_pop_handler();}else{void*_tmp10A=(void*)_exn_thrown;void*_tmp10F=_tmp10A;void*_tmp111;void*_tmp112;_LL96: {struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp110=(struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*)_tmp10F;if(_tmp110->tag != Cyc_Tcutil_AbortTypeCheckingFunction)goto _LL98;}_LL97:
# 410
 goto _LL95;_LL98: _tmp111=_tmp10F;_LL99:
 Cyc_Core_rethrow(_tmp111);_LL9A: _tmp112=_tmp10F;_LL9B:(void)_rethrow(_tmp112);_LL95:;}};}
# 415
if(Cyc_Tc_is_main(fd->name)){
# 417
{void*_tmp114=Cyc_Tcutil_compress(fd->ret_type);void*_tmp115=_tmp114;enum Cyc_Absyn_Size_of _tmp118;_LL9D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp116=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp115;if(_tmp116->tag != 0)goto _LL9F;}_LL9E:
{const char*_tmp361;void*_tmp360;(_tmp360=0,Cyc_Tcutil_warn(loc,((_tmp361="main declared with return type void",_tag_dyneither(_tmp361,sizeof(char),36))),_tag_dyneither(_tmp360,sizeof(void*),0)));}goto _LL9C;_LL9F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp117=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp115;if(_tmp117->tag != 6)goto _LLA1;else{_tmp118=_tmp117->f2;}}_LLA0:
 goto _LL9C;_LLA1:;_LLA2:
# 421
{const char*_tmp365;void*_tmp364[1];struct Cyc_String_pa_PrintArg_struct _tmp363;(_tmp363.tag=0,((_tmp363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp364[0]=& _tmp363,Cyc_Tcutil_terr(loc,((_tmp365="main declared with return type %s instead of int or void",_tag_dyneither(_tmp365,sizeof(char),57))),_tag_dyneither(_tmp364,sizeof(void*),1)))))));}
goto _LL9C;_LL9C:;}
# 425
if(fd->c_varargs  || fd->cyc_varargs != 0){
const char*_tmp368;void*_tmp367;(_tmp367=0,Cyc_Tcutil_terr(loc,((_tmp368="main declared with varargs",_tag_dyneither(_tmp368,sizeof(char),27))),_tag_dyneither(_tmp367,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp120=fd->args;
if(_tmp120 != 0){
struct _tuple8*_tmp121=(struct _tuple8*)_tmp120->hd;void*_tmp123;struct _tuple8*_tmp122=_tmp121;_tmp123=_tmp122->f3;
{void*_tmp124=Cyc_Tcutil_compress(_tmp123);void*_tmp125=_tmp124;enum Cyc_Absyn_Size_of _tmp127;_LLA4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp126=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp125;if(_tmp126->tag != 6)goto _LLA6;else{_tmp127=_tmp126->f2;}}_LLA5:
 goto _LLA3;_LLA6:;_LLA7:
# 433
{const char*_tmp36C;void*_tmp36B[1];struct Cyc_String_pa_PrintArg_struct _tmp36A;(_tmp36A.tag=0,((_tmp36A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp123)),((_tmp36B[0]=& _tmp36A,Cyc_Tcutil_terr(loc,((_tmp36C="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp36C,sizeof(char),60))),_tag_dyneither(_tmp36B,sizeof(void*),1)))))));}
goto _LLA3;_LLA3:;}
# 437
_tmp120=_tmp120->tl;
if(_tmp120 != 0){
struct _tuple8*_tmp12B=(struct _tuple8*)_tmp120->hd;void*_tmp12D;struct _tuple8*_tmp12C=_tmp12B;_tmp12D=_tmp12C->f3;
_tmp120=_tmp120->tl;
if(_tmp120 != 0){
const char*_tmp36F;void*_tmp36E;(_tmp36E=0,Cyc_Tcutil_terr(loc,((_tmp36F="main declared with too many arguments",_tag_dyneither(_tmp36F,sizeof(char),38))),_tag_dyneither(_tmp36E,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp370;struct Cyc_Core_Opt*tvs=(_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370->v=fd->tvs,_tmp370)));
struct Cyc_Core_Opt*_tmp378;struct Cyc_Core_Opt*_tmp377;struct Cyc_Core_Opt*_tmp376;struct Cyc_Core_Opt*_tmp375;struct Cyc_Core_Opt*_tmp374;struct Cyc_Core_Opt*_tmp373;struct Cyc_Core_Opt*_tmp372;struct Cyc_Core_Opt*_tmp371;if(((!Cyc_Tcutil_unify(_tmp12D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377->v=& Cyc_Tcutil_rk,_tmp377)))),tvs)),
Cyc_Absyn_new_evar(((_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378->v=& Cyc_Tcutil_rk,_tmp378)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmp12D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375->v=& Cyc_Tcutil_rk,_tmp375)))),tvs)),
# 449
Cyc_Absyn_new_evar(((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376->v=& Cyc_Tcutil_rk,_tmp376)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp12D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373->v=& Cyc_Tcutil_rk,_tmp373)))),tvs)),
Cyc_Absyn_new_evar(((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374->v=& Cyc_Tcutil_rk,_tmp374)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp12D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371->v=& Cyc_Tcutil_rk,_tmp371)))),tvs)),
# 456
Cyc_Absyn_new_evar(((_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372->v=& Cyc_Tcutil_rk,_tmp372)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
const char*_tmp37C;void*_tmp37B[1];struct Cyc_String_pa_PrintArg_struct _tmp37A;(_tmp37A.tag=0,((_tmp37A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp12D)),((_tmp37B[0]=& _tmp37A,Cyc_Tcutil_terr(loc,((_tmp37C="second argument of main has type %s instead of char??",_tag_dyneither(_tmp37C,sizeof(char),54))),_tag_dyneither(_tmp37B,sizeof(void*),1)))))));}};}}};}
# 395
;_pop_region(fnrgn);};};}
# 466
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 472
{union Cyc_Absyn_Nmspace _tmp13C=(*td->name).f1;union Cyc_Absyn_Nmspace _tmp13D=_tmp13C;_LLA9: if((_tmp13D.Rel_n).tag != 1)goto _LLAB;_LLAA:
# 475
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,td->extern_c);
goto _LLA8;_LLAB:;_LLAC:
 goto _LLA8;_LLA8:;}
# 480
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
{const char*_tmp380;void*_tmp37F[1];struct Cyc_String_pa_PrintArg_struct _tmp37E;(_tmp37E.tag=0,((_tmp37E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp37F[0]=& _tmp37E,Cyc_Tcutil_terr(loc,((_tmp380="redeclaration of typedef %s",_tag_dyneither(_tmp380,sizeof(char),28))),_tag_dyneither(_tmp37F,sizeof(void*),1)))))));}
return;}
# 485
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 494
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp141=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmp142=_tmp141;struct Cyc_Core_Opt**_tmp144;struct Cyc_Core_Opt**_tmp146;struct Cyc_Absyn_Kind*_tmp147;_LLAE: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp143=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp142;if(_tmp143->tag != 1)goto _LLB0;else{_tmp144=(struct Cyc_Core_Opt**)& _tmp143->f1;}}_LLAF:
# 497
 if(td->defn != 0){
const char*_tmp384;void*_tmp383[1];struct Cyc_String_pa_PrintArg_struct _tmp382;(_tmp382.tag=0,((_tmp382.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp383[0]=& _tmp382,Cyc_Tcutil_warn(loc,((_tmp384="type variable %s is not used in typedef definition",_tag_dyneither(_tmp384,sizeof(char),51))),_tag_dyneither(_tmp383,sizeof(void*),1)))))));}
# 500
{struct Cyc_Core_Opt*_tmp385;*_tmp144=((_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp385))));}goto _LLAD;_LLB0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp145=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142;if(_tmp145->tag != 2)goto _LLB2;else{_tmp146=(struct Cyc_Core_Opt**)& _tmp145->f1;_tmp147=_tmp145->f2;}}_LLB1:
# 507
{struct Cyc_Core_Opt*_tmp386;*_tmp146=((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->v=Cyc_Tcutil_kind_to_bound(_tmp147),_tmp386))));}
goto _LLAD;_LLB2:;_LLB3:
 continue;_LLAD:;}}
# 513
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 516
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 523
struct _RegionHandle _tmp14D=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp14D;_push_region(uprev_rgn);
# 525
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmp14E=(struct _tuple10*)rpo->hd;void*_tmp150;void*_tmp151;struct _tuple10*_tmp14F=_tmp14E;_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmp150);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmp151);}{
# 531
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 534
struct Cyc_List_List*_tmp152=fields;for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
struct Cyc_Absyn_Aggrfield*_tmp153=(struct Cyc_Absyn_Aggrfield*)_tmp152->hd;struct _dyneither_ptr*_tmp155;struct Cyc_Absyn_Tqual _tmp156;void*_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_List_List*_tmp159;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Aggrfield*_tmp154=_tmp153;_tmp155=_tmp154->name;_tmp156=_tmp154->tq;_tmp157=_tmp154->type;_tmp158=_tmp154->width;_tmp159=_tmp154->attributes;_tmp15A=_tmp154->requires_clause;
# 537
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp155)){
const char*_tmp38A;void*_tmp389[1];struct Cyc_String_pa_PrintArg_struct _tmp388;(_tmp388.tag=0,((_tmp388.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp155),((_tmp389[0]=& _tmp388,Cyc_Tcutil_terr(loc,((_tmp38A="duplicate member %s",_tag_dyneither(_tmp38A,sizeof(char),20))),_tag_dyneither(_tmp389,sizeof(void*),1)))))));}
# 541
{const char*_tmp38B;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp155,((_tmp38B="",_tag_dyneither(_tmp38B,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp38C;prev_fields=((_tmp38C=_region_malloc(uprev_rgn,sizeof(*_tmp38C)),((_tmp38C->hd=_tmp155,((_tmp38C->tl=prev_fields,_tmp38C))))));}}{
# 544
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 548
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp152->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp157);
# 553
(((struct Cyc_Absyn_Aggrfield*)_tmp152->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp152->hd)->tq).print_const,_tmp157);
# 556
Cyc_Tcutil_check_bitfield(loc,te,_tmp157,_tmp158,_tmp155);
# 558
if((unsigned int)_tmp15A){
if(str_or_union != Cyc_Absyn_UnionA){
const char*_tmp38F;void*_tmp38E;(_tmp38E=0,Cyc_Tcutil_terr(loc,((_tmp38F="@requires clauses are only allowed on union members",_tag_dyneither(_tmp38F,sizeof(char),52))),_tag_dyneither(_tmp38E,sizeof(void*),0)));}
{struct _RegionHandle _tmp162=_new_region("temp");struct _RegionHandle*temp=& _tmp162;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp163=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp163,0,_tmp15A);}
# 562
;_pop_region(temp);}
# 565
if(!Cyc_Tcutil_is_integral(_tmp15A)){
const char*_tmp393;void*_tmp392[1];struct Cyc_String_pa_PrintArg_struct _tmp391;(_tmp391.tag=0,((_tmp391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 568
Cyc_Absynpp_typ2string((void*)_check_null(_tmp15A->topt))),((_tmp392[0]=& _tmp391,Cyc_Tcutil_terr(_tmp15A->loc,((_tmp393="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp393,sizeof(char),54))),_tag_dyneither(_tmp392,sizeof(void*),1)))))));}else{
# 570
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp396;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp395;Cyc_Tcutil_check_type(_tmp15A->loc,te,tvs,& Cyc_Tcutil_ik,0,0,(void*)((_tmp395=_cycalloc(sizeof(*_tmp395)),((_tmp395[0]=((_tmp396.tag=18,((_tmp396.f1=_tmp15A,_tmp396)))),_tmp395)))));}{
# 572
struct Cyc_List_List*_tmp169=Cyc_Relations_exp2relns(uprev_rgn,_tmp15A);
# 579
if(!Cyc_Relations_consistent_relations(_tmp169)){
const char*_tmp399;void*_tmp398;(_tmp398=0,Cyc_Tcutil_terr(_tmp15A->loc,((_tmp399="@requires clause may be unsatisfiable",_tag_dyneither(_tmp399,sizeof(char),38))),_tag_dyneither(_tmp398,sizeof(void*),0)));}
# 585
{struct Cyc_List_List*_tmp16C=prev_relations;for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp169,(struct Cyc_List_List*)_tmp16C->hd))){
const char*_tmp39C;void*_tmp39B;(_tmp39B=0,Cyc_Tcutil_terr(_tmp15A->loc,((_tmp39C="@requires clause may overlap with previous clauses",_tag_dyneither(_tmp39C,sizeof(char),51))),_tag_dyneither(_tmp39B,sizeof(void*),0)));}}}{
# 590
struct Cyc_List_List*_tmp39D;prev_relations=((_tmp39D=_region_malloc(uprev_rgn,sizeof(*_tmp39D)),((_tmp39D->hd=_tmp169,((_tmp39D->tl=prev_relations,_tmp39D))))));};};}}else{
# 593
if(prev_relations != 0){
const char*_tmp3A0;void*_tmp39F;(_tmp39F=0,Cyc_Tcutil_terr(loc,((_tmp3A0="if one field has a @requires clause, they all must",_tag_dyneither(_tmp3A0,sizeof(char),51))),_tag_dyneither(_tmp39F,sizeof(void*),0)));}}};}};
# 525
;_pop_region(uprev_rgn);}
# 600
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 602
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp172=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp173=_tmp172;struct Cyc_Core_Opt**_tmp175;struct Cyc_Core_Opt**_tmp177;enum Cyc_Absyn_AliasQual _tmp178;struct Cyc_Core_Opt**_tmp17A;struct Cyc_Core_Opt**_tmp17C;struct Cyc_Absyn_Kind*_tmp17D;enum Cyc_Absyn_AliasQual _tmp17F;_LLB5: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp174=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp173;if(_tmp174->tag != 1)goto _LLB7;else{_tmp175=(struct Cyc_Core_Opt**)& _tmp174->f1;}}_LLB6:
# 605
{struct Cyc_Core_Opt*_tmp3A1;*_tmp175=((_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((_tmp3A1->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp3A1))));}continue;_LLB7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp176=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173;if(_tmp176->tag != 2)goto _LLB9;else{_tmp177=(struct Cyc_Core_Opt**)& _tmp176->f1;if((_tmp176->f2)->kind != Cyc_Absyn_MemKind)goto _LLB9;_tmp178=(_tmp176->f2)->aliasqual;}}_LLB8:
# 607
 if(constrain_top_kind  && _tmp178 == Cyc_Absyn_Top){
struct Cyc_Absyn_Kind*_tmp3A4;struct Cyc_Core_Opt*_tmp3A3;*_tmp177=((_tmp3A3=_cycalloc(sizeof(*_tmp3A3)),((_tmp3A3->v=Cyc_Tcutil_kind_to_bound(((_tmp3A4=_cycalloc_atomic(sizeof(*_tmp3A4)),((_tmp3A4->kind=Cyc_Absyn_BoxKind,((_tmp3A4->aliasqual=Cyc_Absyn_Aliasable,_tmp3A4))))))),_tmp3A3))));}else{
# 610
struct Cyc_Absyn_Kind*_tmp3A7;struct Cyc_Core_Opt*_tmp3A6;*_tmp177=((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->v=Cyc_Tcutil_kind_to_bound(((_tmp3A7=_cycalloc_atomic(sizeof(*_tmp3A7)),((_tmp3A7->kind=Cyc_Absyn_BoxKind,((_tmp3A7->aliasqual=_tmp178,_tmp3A7))))))),_tmp3A6))));}
continue;_LLB9:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp179=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173;if(_tmp179->tag != 2)goto _LLBB;else{_tmp17A=(struct Cyc_Core_Opt**)& _tmp179->f1;if((_tmp179->f2)->kind != Cyc_Absyn_BoxKind)goto _LLBB;if((_tmp179->f2)->aliasqual != Cyc_Absyn_Top)goto _LLBB;}}if(!constrain_top_kind)goto _LLBB;_LLBA:
# 613
{struct Cyc_Absyn_Kind*_tmp3AA;struct Cyc_Core_Opt*_tmp3A9;*_tmp17A=((_tmp3A9=_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9->v=Cyc_Tcutil_kind_to_bound(((_tmp3AA=_cycalloc_atomic(sizeof(*_tmp3AA)),((_tmp3AA->kind=Cyc_Absyn_BoxKind,((_tmp3AA->aliasqual=Cyc_Absyn_Aliasable,_tmp3AA))))))),_tmp3A9))));}
continue;_LLBB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp17B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173;if(_tmp17B->tag != 2)goto _LLBD;else{_tmp17C=(struct Cyc_Core_Opt**)& _tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LLBC:
# 616
{struct Cyc_Core_Opt*_tmp3AB;*_tmp17C=((_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB->v=Cyc_Tcutil_kind_to_bound(_tmp17D),_tmp3AB))));}continue;_LLBD: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp17E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp173;if(_tmp17E->tag != 0)goto _LLBF;else{if((_tmp17E->f1)->kind != Cyc_Absyn_MemKind)goto _LLBF;_tmp17F=(_tmp17E->f1)->aliasqual;}}_LLBE:
# 618
{const char*_tmp3B3;void*_tmp3B2[3];struct Cyc_String_pa_PrintArg_struct _tmp3B1;struct Cyc_String_pa_PrintArg_struct _tmp3B0;struct Cyc_Absyn_Kind*_tmp3AF;struct Cyc_String_pa_PrintArg_struct _tmp3AE;(_tmp3AE.tag=0,((_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp3AF=_cycalloc_atomic(sizeof(*_tmp3AF)),((_tmp3AF->kind=Cyc_Absyn_MemKind,((_tmp3AF->aliasqual=_tmp17F,_tmp3AF)))))))),((_tmp3B0.tag=0,((_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3B1.tag=0,((_tmp3B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp3B2[0]=& _tmp3B1,((_tmp3B2[1]=& _tmp3B0,((_tmp3B2[2]=& _tmp3AE,Cyc_Tcutil_terr(loc,((_tmp3B3="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3B3,sizeof(char),57))),_tag_dyneither(_tmp3B2,sizeof(void*),3)))))))))))))))))));}
continue;_LLBF:;_LLC0:
 continue;_LLB4:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 626
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp18E=(*ad->name).f2;
# 633
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp18F=(void*)atts->hd;void*_tmp190=_tmp18F;_LLC2: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp191=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp190;if(_tmp191->tag != 7)goto _LLC4;}_LLC3:
 goto _LLC5;_LLC4: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp192=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp190;if(_tmp192->tag != 6)goto _LLC6;}_LLC5:
 continue;_LLC6:;_LLC7:
# 638
{const char*_tmp3B8;void*_tmp3B7[2];struct Cyc_String_pa_PrintArg_struct _tmp3B6;struct Cyc_String_pa_PrintArg_struct _tmp3B5;(_tmp3B5.tag=0,((_tmp3B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp18E),((_tmp3B6.tag=0,((_tmp3B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp3B7[0]=& _tmp3B6,((_tmp3B7[1]=& _tmp3B5,Cyc_Tcutil_terr(loc,((_tmp3B8="bad attribute %s in %s definition",_tag_dyneither(_tmp3B8,sizeof(char),34))),_tag_dyneither(_tmp3B7,sizeof(void*),2)))))))))))));}
goto _LLC1;_LLC1:;}}{
# 643
struct Cyc_List_List*_tmp197=ad->tvs;
# 645
{union Cyc_Absyn_Nmspace _tmp198=(*ad->name).f1;union Cyc_Absyn_Nmspace _tmp199=_tmp198;_LLC9: if((_tmp199.Rel_n).tag != 1)goto _LLCB;if((struct Cyc_List_List*)(_tmp199.Rel_n).val != 0)goto _LLCB;_LLCA:
# 648
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLC8;_LLCB:;_LLCC:
# 651
 goto _LLC8;_LLC8:;}
# 655
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 657
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 661
{struct _tuple15 _tmp3B9;struct _tuple15 _tmp19A=(_tmp3B9.f1=ad->impl,((_tmp3B9.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp18E),_tmp3B9)));struct _tuple15 _tmp19B=_tmp19A;struct Cyc_List_List*_tmp19C;struct Cyc_List_List*_tmp19D;struct Cyc_List_List*_tmp19E;int _tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*_tmp1A1;struct Cyc_List_List*_tmp1A2;int _tmp1A3;struct Cyc_Absyn_Aggrdecl**_tmp1A4;struct Cyc_Absyn_Aggrdecl**_tmp1A5;_LLCE: if(_tmp19B.f1 != 0)goto _LLD0;if(_tmp19B.f2 != 0)goto _LLD0;_LLCF:
# 664
 Cyc_Tc_rule_out_memkind(loc,_tmp18E,_tmp197,0);
# 666
{struct Cyc_Absyn_Aggrdecl**_tmp3BA;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp18E,((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=ad,_tmp3BA)))));}
goto _LLCD;_LLD0: if(_tmp19B.f1 == 0)goto _LLD2;_tmp19C=(_tmp19B.f1)->exist_vars;_tmp19D=(_tmp19B.f1)->rgn_po;_tmp19E=(_tmp19B.f1)->fields;_tmp19F=(_tmp19B.f1)->tagged;if(_tmp19B.f2 != 0)goto _LLD2;_LLD1: {
# 671
struct Cyc_Absyn_Aggrdecl*_tmp3BD;struct Cyc_Absyn_Aggrdecl**_tmp3BC;struct Cyc_Absyn_Aggrdecl**_tmp1A7=(_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC[0]=((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD->kind=ad->kind,((_tmp3BD->sc=Cyc_Absyn_Extern,((_tmp3BD->name=ad->name,((_tmp3BD->tvs=_tmp197,((_tmp3BD->impl=0,((_tmp3BD->attributes=ad->attributes,((_tmp3BD->expected_mem_kind=0,_tmp3BD)))))))))))))))),_tmp3BC)));
# 673
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp18E,_tmp1A7);
# 678
Cyc_Tcutil_check_unique_tvars(loc,_tmp19C);
# 680
Cyc_Tcutil_add_tvar_identities(_tmp19C);
# 683
if(_tmp19F  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp3C0;void*_tmp3BF;(_tmp3BF=0,Cyc_Tcutil_terr(loc,((_tmp3C0="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3C0,sizeof(char),56))),_tag_dyneither(_tmp3BF,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp197,_tmp19C),_tmp19D,_tmp19E);
# 687
Cyc_Tc_rule_out_memkind(loc,_tmp18E,_tmp197,0);
# 690
Cyc_Tc_rule_out_memkind(loc,_tmp18E,_tmp19C,1);
# 692
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp19F){
# 695
struct Cyc_List_List*f=_tmp19E;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0){
const char*_tmp3C6;void*_tmp3C5[3];struct Cyc_String_pa_PrintArg_struct _tmp3C4;struct Cyc_String_pa_PrintArg_struct _tmp3C3;struct Cyc_String_pa_PrintArg_struct _tmp3C2;(_tmp3C2.tag=0,((_tmp3C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp3C3.tag=0,((_tmp3C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp18E),((_tmp3C4.tag=0,((_tmp3C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp3C5[0]=& _tmp3C4,((_tmp3C5[1]=& _tmp3C3,((_tmp3C5[2]=& _tmp3C2,Cyc_Tcutil_warn(loc,((_tmp3C6="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp3C6,sizeof(char),98))),_tag_dyneither(_tmp3C5,sizeof(void*),3)))))))))))))))))));}}}
# 701
*_tmp1A7=ad;
goto _LLCD;}_LLD2: if(_tmp19B.f1 == 0)goto _LLD4;_tmp1A0=(_tmp19B.f1)->exist_vars;_tmp1A1=(_tmp19B.f1)->rgn_po;_tmp1A2=(_tmp19B.f1)->fields;_tmp1A3=(_tmp19B.f1)->tagged;if(_tmp19B.f2 == 0)goto _LLD4;_tmp1A4=*_tmp19B.f2;_LLD3:
# 705
 if(ad->kind != (*_tmp1A4)->kind){
const char*_tmp3C9;void*_tmp3C8;(_tmp3C8=0,Cyc_Tcutil_terr(loc,((_tmp3C9="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp3C9,sizeof(char),52))),_tag_dyneither(_tmp3C8,sizeof(void*),0)));}{
# 708
struct Cyc_Absyn_Aggrdecl*_tmp1B3=*_tmp1A4;
# 710
{struct Cyc_Absyn_Aggrdecl*_tmp3CA;*_tmp1A4=((_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA->kind=ad->kind,((_tmp3CA->sc=Cyc_Absyn_Extern,((_tmp3CA->name=ad->name,((_tmp3CA->tvs=_tmp197,((_tmp3CA->impl=0,((_tmp3CA->attributes=ad->attributes,((_tmp3CA->expected_mem_kind=0,_tmp3CA))))))))))))))));}
# 716
Cyc_Tcutil_check_unique_tvars(loc,_tmp1A0);
# 718
Cyc_Tcutil_add_tvar_identities(_tmp1A0);
# 720
if(_tmp1A3  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp3CD;void*_tmp3CC;(_tmp3CC=0,Cyc_Tcutil_terr(loc,((_tmp3CD="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3CD,sizeof(char),56))),_tag_dyneither(_tmp3CC,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp197,_tmp1A0),_tmp1A1,_tmp1A2);
# 724
*_tmp1A4=_tmp1B3;
_tmp1A5=_tmp1A4;goto _LLD5;};_LLD4: if(_tmp19B.f1 != 0)goto _LLCD;if(_tmp19B.f2 == 0)goto _LLCD;_tmp1A5=*_tmp19B.f2;_LLD5: {
# 728
struct Cyc_Absyn_Aggrdecl*_tmp1B7=Cyc_Tcdecl_merge_aggrdecl(*_tmp1A5,ad,loc,Cyc_Tc_tc_msg);
if(_tmp1B7 == 0)
return;else{
# 732
Cyc_Tc_rule_out_memkind(loc,_tmp18E,_tmp197,0);
# 735
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp18E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 738
*_tmp1A5=_tmp1B7;
ad=_tmp1B7;
goto _LLCD;}}_LLCD:;}{
# 744
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3D3;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp3D2;struct _tuple14*_tmp3D1;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp18E,(
(_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1->f1=(void*)((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3[0]=((_tmp3D2.tag=1,((_tmp3D2.f1=ad,_tmp3D2)))),_tmp3D3)))),((_tmp3D1->f2=1,_tmp3D1)))))));};};}
# 748
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp1BC=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp1BD=_tmp1BC;struct Cyc_Core_Opt**_tmp1BF;struct Cyc_Core_Opt**_tmp1C1;struct Cyc_Core_Opt**_tmp1C3;struct Cyc_Core_Opt**_tmp1C5;struct Cyc_Core_Opt**_tmp1C7;struct Cyc_Core_Opt**_tmp1C9;struct Cyc_Core_Opt**_tmp1CC;enum Cyc_Absyn_KindQual _tmp1CD;enum Cyc_Absyn_KindQual _tmp1CF;enum Cyc_Absyn_AliasQual _tmp1D1;_LLD7: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1BE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1BE->tag != 1)goto _LLD9;else{_tmp1BF=(struct Cyc_Core_Opt**)& _tmp1BE->f1;}}_LLD8:
 _tmp1C1=_tmp1BF;goto _LLDA;_LLD9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1C0->tag != 2)goto _LLDB;else{_tmp1C1=(struct Cyc_Core_Opt**)& _tmp1C0->f1;if((_tmp1C0->f2)->kind != Cyc_Absyn_MemKind)goto _LLDB;if((_tmp1C0->f2)->aliasqual != Cyc_Absyn_Top)goto _LLDB;}}_LLDA:
# 753
 _tmp1C3=_tmp1C1;goto _LLDC;_LLDB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1C2->tag != 2)goto _LLDD;else{_tmp1C3=(struct Cyc_Core_Opt**)& _tmp1C2->f1;if((_tmp1C2->f2)->kind != Cyc_Absyn_MemKind)goto _LLDD;if((_tmp1C2->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLDD;}}_LLDC:
# 755
{struct Cyc_Core_Opt*_tmp3D4;*_tmp1C3=((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp3D4))));}goto _LLD6;_LLDD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1C4->tag != 2)goto _LLDF;else{_tmp1C5=(struct Cyc_Core_Opt**)& _tmp1C4->f1;if((_tmp1C4->f2)->kind != Cyc_Absyn_AnyKind)goto _LLDF;if((_tmp1C4->f2)->aliasqual != Cyc_Absyn_Top)goto _LLDF;}}_LLDE:
 _tmp1C7=_tmp1C5;goto _LLE0;_LLDF: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1C6->tag != 2)goto _LLE1;else{_tmp1C7=(struct Cyc_Core_Opt**)& _tmp1C6->f1;if((_tmp1C6->f2)->kind != Cyc_Absyn_AnyKind)goto _LLE1;if((_tmp1C6->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLE1;}}_LLE0:
# 758
{struct Cyc_Core_Opt*_tmp3D5;*_tmp1C7=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp3D5))));}goto _LLD6;_LLE1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1C8->tag != 2)goto _LLE3;else{_tmp1C9=(struct Cyc_Core_Opt**)& _tmp1C8->f1;if((_tmp1C8->f2)->kind != Cyc_Absyn_RgnKind)goto _LLE3;if((_tmp1C8->f2)->aliasqual != Cyc_Absyn_Top)goto _LLE3;}}_LLE2:
# 760
{struct Cyc_Core_Opt*_tmp3D6;*_tmp1C9=((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp3D6))));}goto _LLD6;_LLE3: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1CA->tag != 0)goto _LLE5;else{if((_tmp1CA->f1)->kind != Cyc_Absyn_RgnKind)goto _LLE5;if((_tmp1CA->f1)->aliasqual != Cyc_Absyn_Top)goto _LLE5;}}_LLE4:
# 762
{const char*_tmp3DB;void*_tmp3DA[2];struct Cyc_String_pa_PrintArg_struct _tmp3D9;struct Cyc_String_pa_PrintArg_struct _tmp3D8;(_tmp3D8.tag=0,((_tmp3D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3D9.tag=0,((_tmp3D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3DA[0]=& _tmp3D9,((_tmp3DA[1]=& _tmp3D8,Cyc_Tcutil_terr(loc,((_tmp3DB="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp3DB,sizeof(char),57))),_tag_dyneither(_tmp3DA,sizeof(void*),2)))))))))))));}
goto _LLD6;_LLE5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1CB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1CB->tag != 2)goto _LLE7;else{_tmp1CC=(struct Cyc_Core_Opt**)& _tmp1CB->f1;_tmp1CD=(_tmp1CB->f2)->kind;if((_tmp1CB->f2)->aliasqual != Cyc_Absyn_Unique)goto _LLE7;}}_LLE6:
 _tmp1CF=_tmp1CD;goto _LLE8;_LLE7: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1CE->tag != 0)goto _LLE9;else{_tmp1CF=(_tmp1CE->f1)->kind;if((_tmp1CE->f1)->aliasqual != Cyc_Absyn_Unique)goto _LLE9;}}_LLE8:
# 766
{const char*_tmp3E3;void*_tmp3E2[3];struct Cyc_String_pa_PrintArg_struct _tmp3E1;struct Cyc_String_pa_PrintArg_struct _tmp3E0;struct Cyc_Absyn_Kind*_tmp3DF;struct Cyc_String_pa_PrintArg_struct _tmp3DE;(_tmp3DE.tag=0,((_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 768
Cyc_Absynpp_kind2string(((_tmp3DF=_cycalloc_atomic(sizeof(*_tmp3DF)),((_tmp3DF->kind=_tmp1CF,((_tmp3DF->aliasqual=Cyc_Absyn_Unique,_tmp3DF)))))))),((_tmp3E0.tag=0,((_tmp3E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3E1.tag=0,((_tmp3E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3E2[0]=& _tmp3E1,((_tmp3E2[1]=& _tmp3E0,((_tmp3E2[2]=& _tmp3DE,Cyc_Tcutil_terr(loc,((_tmp3E3="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3E3,sizeof(char),57))),_tag_dyneither(_tmp3E2,sizeof(void*),3)))))))))))))))))));}goto _LLD6;_LLE9: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1BD;if(_tmp1D0->tag != 0)goto _LLEB;else{if((_tmp1D0->f1)->kind != Cyc_Absyn_MemKind)goto _LLEB;_tmp1D1=(_tmp1D0->f1)->aliasqual;}}_LLEA:
# 770
{const char*_tmp3EB;void*_tmp3EA[3];struct Cyc_String_pa_PrintArg_struct _tmp3E9;struct Cyc_String_pa_PrintArg_struct _tmp3E8;struct Cyc_Absyn_Kind*_tmp3E7;struct Cyc_String_pa_PrintArg_struct _tmp3E6;(_tmp3E6.tag=0,((_tmp3E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 772
Cyc_Absynpp_kind2string(((_tmp3E7=_cycalloc_atomic(sizeof(*_tmp3E7)),((_tmp3E7->kind=Cyc_Absyn_MemKind,((_tmp3E7->aliasqual=_tmp1D1,_tmp3E7)))))))),((_tmp3E8.tag=0,((_tmp3E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3E9.tag=0,((_tmp3E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3EA[0]=& _tmp3E9,((_tmp3EA[1]=& _tmp3E8,((_tmp3EA[2]=& _tmp3E6,Cyc_Tcutil_terr(loc,((_tmp3EB="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3EB,sizeof(char),57))),_tag_dyneither(_tmp3EA,sizeof(void*),3)))))))))))))))))));}goto _LLD6;_LLEB:;_LLEC:
 goto _LLD6;_LLD6:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 779
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 788
{struct Cyc_List_List*_tmp1E5=fields;for(0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){
struct Cyc_Absyn_Datatypefield*_tmp1E6=(struct Cyc_Absyn_Datatypefield*)_tmp1E5->hd;
# 791
{struct Cyc_List_List*typs=_tmp1E6->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp1E6->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);{
# 794
struct _RegionHandle _tmp1E7=_new_region("temp");struct _RegionHandle*temp=& _tmp1E7;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple16*)typs->hd)).f2)){
const char*_tmp3EF;void*_tmp3EE[1];struct Cyc_String_pa_PrintArg_struct _tmp3ED;(_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 798
Cyc_Absynpp_qvar2string(_tmp1E6->name)),((_tmp3EE[0]=& _tmp3ED,Cyc_Tcutil_terr(_tmp1E6->loc,((_tmp3EF="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp3EF,sizeof(char),51))),_tag_dyneither(_tmp3EE,sizeof(void*),1)))))));}
# 800
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp1E6->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 794
;_pop_region(temp);};}}{
# 805
union Cyc_Absyn_Nmspace _tmp1EB=(*_tmp1E6->name).f1;union Cyc_Absyn_Nmspace _tmp1EC=_tmp1EB;_LLEE: if((_tmp1EC.Rel_n).tag != 1)goto _LLF0;if((struct Cyc_List_List*)(_tmp1EC.Rel_n).val != 0)goto _LLF0;_LLEF:
# 807
 if(is_extensible)
(*_tmp1E6->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 810
(*_tmp1E6->name).f1=(*name).f1;}
goto _LLED;_LLF0: if((_tmp1EC.Rel_n).tag != 1)goto _LLF2;_LLF1:
# 813
{const char*_tmp3F3;void*_tmp3F2[1];struct Cyc_String_pa_PrintArg_struct _tmp3F1;(_tmp3F1.tag=0,((_tmp3F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1E6->name)),((_tmp3F2[0]=& _tmp3F1,Cyc_Tcutil_terr(_tmp1E6->loc,((_tmp3F3="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp3F3,sizeof(char),58))),_tag_dyneither(_tmp3F2,sizeof(void*),1)))))));}
goto _LLED;_LLF2: if((_tmp1EC.C_n).tag != 3)goto _LLF4;_LLF3:
 goto _LLED;_LLF4: if((_tmp1EC.Abs_n).tag != 2)goto _LLF6;_LLF5:
 goto _LLED;_LLF6: if((_tmp1EC.Loc_n).tag != 4)goto _LLED;_LLF7: {
const char*_tmp3F6;void*_tmp3F5;(_tmp3F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3F6="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp3F6,sizeof(char),24))),_tag_dyneither(_tmp3F5,sizeof(void*),0)));}_LLED:;};}}{
# 822
struct Cyc_List_List*fields2;
if(is_extensible){
# 825
int _tmp1F2=1;
struct Cyc_List_List*_tmp1F3=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1F2,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1F2)
fields2=_tmp1F3;else{
# 830
fields2=0;}}else{
# 832
struct _RegionHandle _tmp1F4=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1F4;_push_region(uprev_rgn);
# 834
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp1F5=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1F5->name).f2)){
const char*_tmp3FB;void*_tmp3FA[2];struct Cyc_String_pa_PrintArg_struct _tmp3F9;struct Cyc_String_pa_PrintArg_struct _tmp3F8;(_tmp3F8.tag=0,((_tmp3F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3F9.tag=0,((_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1F5->name).f2),((_tmp3FA[0]=& _tmp3F9,((_tmp3FA[1]=& _tmp3F8,Cyc_Tcutil_terr(_tmp1F5->loc,((_tmp3FB="duplicate field name %s in %s",_tag_dyneither(_tmp3FB,sizeof(char),30))),_tag_dyneither(_tmp3FA,sizeof(void*),2)))))))))))));}else{
# 840
struct Cyc_List_List*_tmp3FC;prev_fields=((_tmp3FC=_region_malloc(uprev_rgn,sizeof(*_tmp3FC)),((_tmp3FC->hd=(*_tmp1F5->name).f2,((_tmp3FC->tl=prev_fields,_tmp3FC))))));}
# 842
if(_tmp1F5->sc != Cyc_Absyn_Public){
{const char*_tmp400;void*_tmp3FF[1];struct Cyc_String_pa_PrintArg_struct _tmp3FE;(_tmp3FE.tag=0,((_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1F5->name).f2),((_tmp3FF[0]=& _tmp3FE,Cyc_Tcutil_warn(loc,((_tmp400="ignoring scope of field %s",_tag_dyneither(_tmp400,sizeof(char),27))),_tag_dyneither(_tmp3FF,sizeof(void*),1)))))));}
_tmp1F5->sc=Cyc_Absyn_Public;}}}
# 847
fields2=fields;}
# 834
;_pop_region(uprev_rgn);}
# 853
{struct Cyc_List_List*_tmp1FE=fields;for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){
struct Cyc_Absyn_Datatypefield*_tmp1FF=(struct Cyc_Absyn_Datatypefield*)_tmp1FE->hd;
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp406;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp405;struct _tuple14*_tmp404;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1FF->name).f2,(
(_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404->f1=(void*)((_tmp406=_cycalloc(sizeof(*_tmp406)),((_tmp406[0]=((_tmp405.tag=2,((_tmp405.f1=tudres,((_tmp405.f2=_tmp1FF,_tmp405)))))),_tmp406)))),((_tmp404->f2=1,_tmp404)))))));}}
# 859
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 862
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp408;const char*_tmp407;struct _dyneither_ptr obj=tud->is_extensible?(_tmp408="@extensible datatype",_tag_dyneither(_tmp408,sizeof(char),21)):((_tmp407="datatype",_tag_dyneither(_tmp407,sizeof(char),9)));
# 869
struct Cyc_List_List*tvs=tud->tvs;
# 872
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 874
Cyc_Tcutil_add_tvar_identities(tvs);{
# 879
struct _RegionHandle _tmp203=_new_region("temp");struct _RegionHandle*temp=& _tmp203;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 889 "tc.cyc"
{struct _handler_cons _tmp204;_push_handler(& _tmp204);{int _tmp206=0;if(setjmp(_tmp204.handler))_tmp206=1;if(!_tmp206){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp40B;struct Cyc_Dict_Absent_exn_struct*_tmp40A;(int)_throw((void*)((_tmp40A=_cycalloc_atomic(sizeof(*_tmp40A)),((_tmp40A[0]=((_tmp40B.tag=Cyc_Dict_Absent,_tmp40B)),_tmp40A)))));}
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 895
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 890
;_pop_handler();}else{void*_tmp205=(void*)_exn_thrown;void*_tmp20A=_tmp205;void*_tmp20C;_LLF9: {struct Cyc_Dict_Absent_exn_struct*_tmp20B=(struct Cyc_Dict_Absent_exn_struct*)_tmp20A;if(_tmp20B->tag != Cyc_Dict_Absent)goto _LLFB;}_LLFA:
# 899
{union Cyc_Absyn_Nmspace _tmp20D=(*tud->name).f1;union Cyc_Absyn_Nmspace _tmp20E=_tmp20D;_LLFE: if((_tmp20E.Rel_n).tag != 1)goto _LL100;if((struct Cyc_List_List*)(_tmp20E.Rel_n).val != 0)goto _LL100;_LLFF:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLFD;_LL100:;_LL101:
 goto _LLFD;_LLFD:;}{
# 903
struct Cyc_Absyn_Datatypedecl***_tmp20F=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
{struct Cyc_Absyn_Datatypedecl***_tmp40C;tud_opt=(unsigned int)_tmp20F?(_tmp40C=_region_malloc(temp,sizeof(*_tmp40C)),((_tmp40C[0]=*_tmp20F,_tmp40C))): 0;}
goto _LLF8;};_LLFB: _tmp20C=_tmp20A;_LLFC:(void)_rethrow(_tmp20C);_LLF8:;}};}{
# 910
struct _tuple17 _tmp40D;struct _tuple17 _tmp211=(_tmp40D.f1=tud->fields,((_tmp40D.f2=tud_opt,_tmp40D)));struct _tuple17 _tmp212=_tmp211;struct Cyc_List_List**_tmp213;struct Cyc_List_List**_tmp214;struct Cyc_Absyn_Datatypedecl**_tmp215;struct Cyc_Absyn_Datatypedecl**_tmp216;_LL103: if(_tmp212.f1 != 0)goto _LL105;if(_tmp212.f2 != 0)goto _LL105;_LL104:
# 913
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
{struct Cyc_Absyn_Datatypedecl**_tmp40E;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp40E=_cycalloc(sizeof(*_tmp40E)),((_tmp40E[0]=tud,_tmp40E)))));}
goto _LL102;_LL105: if(_tmp212.f1 == 0)goto _LL107;_tmp213=(struct Cyc_List_List**)&(_tmp212.f1)->v;if(_tmp212.f2 != 0)goto _LL107;_LL106: {
# 918
struct Cyc_Absyn_Datatypedecl*_tmp411;struct Cyc_Absyn_Datatypedecl**_tmp410;struct Cyc_Absyn_Datatypedecl**_tmp218=(_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411->sc=Cyc_Absyn_Extern,((_tmp411->name=tud->name,((_tmp411->tvs=tvs,((_tmp411->fields=0,((_tmp411->is_extensible=tud->is_extensible,_tmp411)))))))))))),_tmp410)));
# 920
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp218);
# 923
*_tmp213=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp213,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp218=tud;
goto _LL102;}_LL107: if(_tmp212.f1 == 0)goto _LL109;_tmp214=(struct Cyc_List_List**)&(_tmp212.f1)->v;if(_tmp212.f2 == 0)goto _LL109;_tmp215=*_tmp212.f2;_LL108: {
# 928
struct Cyc_Absyn_Datatypedecl*_tmp21B=*_tmp215;
# 931
if((!tud->is_extensible  && (unsigned int)_tmp21B) && _tmp21B->is_extensible)
tud->is_extensible=1;
# 934
{struct Cyc_Absyn_Datatypedecl*_tmp412;*_tmp215=((_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412->sc=Cyc_Absyn_Extern,((_tmp412->name=tud->name,((_tmp412->tvs=tvs,((_tmp412->fields=0,((_tmp412->is_extensible=tud->is_extensible,_tmp412))))))))))));}
# 938
*_tmp214=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp214,tvs,tud);
# 942
*_tmp215=_tmp21B;
_tmp216=_tmp215;goto _LL10A;}_LL109: if(_tmp212.f1 != 0)goto _LL102;if(_tmp212.f2 == 0)goto _LL102;_tmp216=*_tmp212.f2;_LL10A: {
# 946
struct Cyc_Absyn_Datatypedecl*_tmp21D=Cyc_Tcdecl_merge_datatypedecl(*_tmp216,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp21D == 0){
_npop_handler(0);return;}else{
# 951
*_tmp216=_tmp21D;
goto _LL102;}}_LL102:;};}
# 880 "tc.cyc"
;_pop_region(temp);};}
# 958 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 965
{union Cyc_Absyn_Nmspace _tmp221=(*ed->name).f1;union Cyc_Absyn_Nmspace _tmp222=_tmp221;_LL10C: if((_tmp222.Rel_n).tag != 1)goto _LL10E;if((struct Cyc_List_List*)(_tmp222.Rel_n).val != 0)goto _LL10E;_LL10D:
# 968
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LL10B;_LL10E:;_LL10F:
 goto _LL10B;_LL10B:;}
# 974
if(ed->fields != 0){struct _RegionHandle _tmp223=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp223;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp224=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 980
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp224->name).f2)){
const char*_tmp416;void*_tmp415[1];struct Cyc_String_pa_PrintArg_struct _tmp414;(_tmp414.tag=0,((_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp224->name).f2),((_tmp415[0]=& _tmp414,Cyc_Tcutil_terr(_tmp224->loc,((_tmp416="duplicate enum constructor %s",_tag_dyneither(_tmp416,sizeof(char),30))),_tag_dyneither(_tmp415,sizeof(void*),1)))))));}else{
# 983
struct Cyc_List_List*_tmp417;prev_fields=((_tmp417=_region_malloc(uprev_rgn,sizeof(*_tmp417)),((_tmp417->hd=(*_tmp224->name).f2,((_tmp417->tl=prev_fields,_tmp417))))));}
# 985
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp224->name).f2)!= 0){
const char*_tmp41B;void*_tmp41A[1];struct Cyc_String_pa_PrintArg_struct _tmp419;(_tmp419.tag=0,((_tmp419.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp224->name).f2),((_tmp41A[0]=& _tmp419,Cyc_Tcutil_terr(_tmp224->loc,((_tmp41B="enum field name %s shadows global name",_tag_dyneither(_tmp41B,sizeof(char),39))),_tag_dyneither(_tmp41A,sizeof(void*),1)))))));}
# 988
if(_tmp224->tag == 0){
_tmp224->tag=Cyc_Absyn_uint_exp(tag_count,_tmp224->loc);
++ tag_count;}else{
# 993
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp224->tag))){
struct _tuple11 _tmp22C=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp224->tag));unsigned int _tmp22E;int _tmp22F;struct _tuple11 _tmp22D=_tmp22C;_tmp22E=_tmp22D.f1;_tmp22F=_tmp22D.f2;
if(_tmp22F)tag_count=_tmp22E + 1;}}
# 997
(*_tmp224->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 975
;_pop_region(uprev_rgn);}
# 1002
{struct _handler_cons _tmp230;_push_handler(& _tmp230);{int _tmp232=0;if(setjmp(_tmp230.handler))_tmp232=1;if(!_tmp232){
{struct Cyc_Absyn_Enumdecl**_tmp233=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp234=Cyc_Tcdecl_merge_enumdecl(*_tmp233,ed,loc,Cyc_Tc_tc_msg);
if(_tmp234 == 0){_npop_handler(0);return;}
*_tmp233=_tmp234;}
# 1003
;_pop_handler();}else{void*_tmp231=(void*)_exn_thrown;void*_tmp236=_tmp231;void*_tmp238;_LL111: {struct Cyc_Dict_Absent_exn_struct*_tmp237=(struct Cyc_Dict_Absent_exn_struct*)_tmp236;if(_tmp237->tag != Cyc_Dict_Absent)goto _LL113;}_LL112: {
# 1008
struct Cyc_Absyn_Enumdecl**_tmp41C;struct Cyc_Absyn_Enumdecl**_tmp239=(_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C[0]=ed,_tmp41C)));
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp239);
goto _LL110;}_LL113: _tmp238=_tmp236;_LL114:(void)_rethrow(_tmp238);_LL110:;}};}
# 1012
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp23B=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp422;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp421;struct _tuple14*_tmp420;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp23B->name).f2,(
(_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420->f1=(void*)((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422[0]=((_tmp421.tag=3,((_tmp421.f1=ed,((_tmp421.f2=_tmp23B,_tmp421)))))),_tmp422)))),((_tmp420->f2=1,_tmp420)))))));}}
# 1021
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp23F=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp23F->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp23F->tag))){
const char*_tmp427;void*_tmp426[2];struct Cyc_String_pa_PrintArg_struct _tmp425;struct Cyc_String_pa_PrintArg_struct _tmp424;(_tmp424.tag=0,((_tmp424.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp23F->name).f2),((_tmp425.tag=0,((_tmp425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp426[0]=& _tmp425,((_tmp426[1]=& _tmp424,Cyc_Tcutil_terr(loc,((_tmp427="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp427,sizeof(char),46))),_tag_dyneither(_tmp426,sizeof(void*),2)))))))))))));}}}}
# 1031
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
switch(sc){case Cyc_Absyn_Static: _LL115:
# 1034
 if(!in_include){
const char*_tmp42A;void*_tmp429;(_tmp429=0,Cyc_Tcutil_warn(loc,((_tmp42A="static declaration nested within extern \"C\"",_tag_dyneither(_tmp42A,sizeof(char),44))),_tag_dyneither(_tmp429,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL116:
# 1038
{const char*_tmp42D;void*_tmp42C;(_tmp42C=0,Cyc_Tcutil_warn(loc,((_tmp42D="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp42D,sizeof(char),46))),_tag_dyneither(_tmp42C,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL117:
 goto _LL118;case Cyc_Absyn_Register: _LL118:
 goto _LL119;case Cyc_Absyn_Extern: _LL119:
 return 1;case Cyc_Absyn_ExternC: _LL11A:
# 1044
{const char*_tmp430;void*_tmp42F;(_tmp42F=0,Cyc_Tcutil_warn(loc,((_tmp430="nested extern \"C\" declaration",_tag_dyneither(_tmp430,sizeof(char),30))),_tag_dyneither(_tmp42F,sizeof(void*),0)));}
return 1;}}
# 1049
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
struct _tuple12 _tmp24A=*exp;unsigned int _tmp24C;struct _tuple0*_tmp24D;struct _tuple12 _tmp24B=_tmp24A;_tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;{
union Cyc_Absyn_Nmspace _tmp24F;struct _dyneither_ptr*_tmp250;struct _tuple0*_tmp24E=_tmp24D;_tmp24F=_tmp24E->f1;_tmp250=_tmp24E->f2;{
union Cyc_Absyn_Nmspace _tmp251=_tmp24F;_LL11D: if((_tmp251.Rel_n).tag != 1)goto _LL11F;if((struct Cyc_List_List*)(_tmp251.Rel_n).val != 0)goto _LL11F;_LL11E:
# 1055
(*_tmp24D).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL11C;_LL11F:;_LL120:
 goto _LL11C;_LL11C:;};};}
# 1066
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 1072
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 1075
struct Cyc_List_List*_tmp252=ds0;for(0;_tmp252 != 0;(last=_tmp252,_tmp252=_tmp252->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp252->hd;
unsigned int loc=d->loc;
# 1079
void*_tmp253=d->r;void*_tmp254=_tmp253;struct Cyc_Absyn_Vardecl*_tmp259;struct Cyc_Absyn_Fndecl*_tmp25B;struct Cyc_Absyn_Typedefdecl*_tmp25D;struct Cyc_Absyn_Aggrdecl*_tmp25F;struct Cyc_Absyn_Datatypedecl*_tmp261;struct Cyc_Absyn_Enumdecl*_tmp263;struct _dyneither_ptr*_tmp267;struct Cyc_List_List*_tmp268;union Cyc_Absyn_Nmspace _tmp26A;struct _dyneither_ptr*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*_tmp270;struct Cyc_List_List*_tmp271;_LL122: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp255=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp255->tag != 2)goto _LL124;}_LL123:
 goto _LL125;_LL124: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp256=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp256->tag != 3)goto _LL126;}_LL125:
# 1082
{const char*_tmp433;void*_tmp432;(_tmp432=0,Cyc_Tcutil_terr(loc,((_tmp433="top level let-declarations are not implemented",_tag_dyneither(_tmp433,sizeof(char),47))),_tag_dyneither(_tmp432,sizeof(void*),0)));}
goto _LL121;_LL126: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp257=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp257->tag != 4)goto _LL128;}_LL127:
# 1085
{const char*_tmp436;void*_tmp435;(_tmp435=0,Cyc_Tcutil_terr(loc,((_tmp436="top level region declarations are not implemented",_tag_dyneither(_tmp436,sizeof(char),50))),_tag_dyneither(_tmp435,sizeof(void*),0)));}
goto _LL121;_LL128: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp258=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp258->tag != 0)goto _LL12A;else{_tmp259=_tmp258->f1;}}_LL129:
# 1088
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp259->sc,te->in_extern_c_include))
_tmp259->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp259,check_var_init,te->in_extern_c_include,exports);
goto _LL121;_LL12A: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp25A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp25A->tag != 1)goto _LL12C;else{_tmp25B=_tmp25A->f1;}}_LL12B:
# 1093
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp25B->sc,te->in_extern_c_include))
_tmp25B->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp25B,exports);
goto _LL121;_LL12C: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp25C=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp25C->tag != 8)goto _LL12E;else{_tmp25D=_tmp25C->f1;}}_LL12D:
# 1098
 _tmp25D->extern_c=te->in_extern_c_include;
# 1102
Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp25D);
goto _LL121;_LL12E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp25E=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp25E->tag != 5)goto _LL130;else{_tmp25F=_tmp25E->f1;}}_LL12F:
# 1105
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp25F->sc,te->in_extern_c_include))
_tmp25F->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp25F);
goto _LL121;_LL130: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp260=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp260->tag != 6)goto _LL132;else{_tmp261=_tmp260->f1;}}_LL131:
# 1110
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp261->sc,te->in_extern_c_include))
_tmp261->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp261);
goto _LL121;_LL132: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp262=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp262->tag != 7)goto _LL134;else{_tmp263=_tmp262->f1;}}_LL133:
# 1115
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp263->sc,te->in_extern_c_include))
_tmp263->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp263);
goto _LL121;_LL134: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp264=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp264->tag != 13)goto _LL136;}_LL135:
# 1120
{const char*_tmp439;void*_tmp438;(_tmp438=0,Cyc_Tcutil_warn(d->loc,((_tmp439="spurious __cyclone_port_on__",_tag_dyneither(_tmp439,sizeof(char),29))),_tag_dyneither(_tmp438,sizeof(void*),0)));}
goto _LL121;_LL136: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp265=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp265->tag != 14)goto _LL138;}_LL137:
# 1123
 goto _LL121;_LL138: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp266=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp266->tag != 9)goto _LL13A;else{_tmp267=_tmp266->f1;_tmp268=_tmp266->f2;}}_LL139: {
# 1126
struct Cyc_List_List*_tmp278=te->ns;
struct Cyc_List_List*_tmp43A;struct Cyc_List_List*_tmp279=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp278,((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A->hd=_tmp267,((_tmp43A->tl=0,_tmp43A)))))));
# 1130
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp267)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp267);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp279,Cyc_Tcenv_empty_genv(grgn));}
# 1135
te->ns=_tmp279;
Cyc_Tc_tc_decls(te,_tmp268,in_externC,check_var_init,grgn,exports);
te->ns=_tmp278;
goto _LL121;}_LL13A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp269=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp269->tag != 10)goto _LL13C;else{_tmp26A=(_tmp269->f1)->f1;_tmp26B=(_tmp269->f1)->f2;_tmp26C=_tmp269->f2;}}_LL13B: {
# 1142
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp27B=_tmp26A;struct _dyneither_ptr*_tmp27C;struct Cyc_List_List*_tmp27D;struct _dyneither_ptr*_tmp27E;struct Cyc_List_List*_tmp27F;struct _dyneither_ptr*_tmp280;struct Cyc_List_List*_tmp281;_LL141: if((_tmp27B.Loc_n).tag != 4)goto _LL143;_LL142:
 goto _LL144;_LL143: if((_tmp27B.Rel_n).tag != 1)goto _LL145;if((struct Cyc_List_List*)(_tmp27B.Rel_n).val != 0)goto _LL145;_LL144:
 goto _LL146;_LL145: if((_tmp27B.C_n).tag != 3)goto _LL147;if((struct Cyc_List_List*)(_tmp27B.C_n).val != 0)goto _LL147;_LL146:
 goto _LL148;_LL147: if((_tmp27B.Abs_n).tag != 2)goto _LL149;if((struct Cyc_List_List*)(_tmp27B.Abs_n).val != 0)goto _LL149;_LL148:
# 1149
 first=_tmp26B;rest=0;goto _LL140;_LL149: if((_tmp27B.Rel_n).tag != 1)goto _LL14B;if((struct Cyc_List_List*)(_tmp27B.Rel_n).val == 0)goto _LL14B;_tmp27C=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp27B.Rel_n).val)->hd;_tmp27D=((struct Cyc_List_List*)(_tmp27B.Rel_n).val)->tl;_LL14A:
 _tmp27E=_tmp27C;_tmp27F=_tmp27D;goto _LL14C;_LL14B: if((_tmp27B.C_n).tag != 3)goto _LL14D;if((struct Cyc_List_List*)(_tmp27B.C_n).val == 0)goto _LL14D;_tmp27E=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp27B.C_n).val)->hd;_tmp27F=((struct Cyc_List_List*)(_tmp27B.C_n).val)->tl;_LL14C:
 _tmp280=_tmp27E;_tmp281=_tmp27F;goto _LL14E;_LL14D: if((_tmp27B.Abs_n).tag != 2)goto _LL140;if((struct Cyc_List_List*)(_tmp27B.Abs_n).val == 0)goto _LL140;_tmp280=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp27B.Abs_n).val)->hd;_tmp281=((struct Cyc_List_List*)(_tmp27B.Abs_n).val)->tl;_LL14E:
# 1153
 first=_tmp280;{struct Cyc_List_List*_tmp43B;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp281,((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B->hd=_tmp26B,((_tmp43B->tl=0,_tmp43B)))))));}goto _LL140;_LL140:;}{
# 1155
struct Cyc_List_List*_tmp283=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
{struct Cyc_List_List*_tmp43C;ge->availables=((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C->hd=_tmp283,((_tmp43C->tl=ge->availables,_tmp43C))))));}
Cyc_Tc_tc_decls(te,_tmp26C,in_externC,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL121;};}_LL13C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp26D=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp26D->tag != 11)goto _LL13E;else{_tmp26E=_tmp26D->f1;}}_LL13D:
# 1162
 Cyc_Tc_tc_decls(te,_tmp26E,1,check_var_init,grgn,exports);
goto _LL121;_LL13E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp26F=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp254;if(_tmp26F->tag != 12)goto _LL121;else{_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;}}_LL13F:
# 1166
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp271);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp271,(unsigned int)exports?*exports: 0);
{struct _RegionHandle _tmp285=_new_region("temp");struct _RegionHandle*temp=& _tmp285;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp286=Cyc_Tcenv_enter_extern_c_include(temp,te);
Cyc_Tc_tc_decls(_tmp286,_tmp270,1,check_var_init,grgn,& newexs);
# 1173
for(0;_tmp271 != 0;_tmp271=_tmp271->tl){
struct _tuple12*_tmp287=(struct _tuple12*)_tmp271->hd;
if(!(*_tmp287).f3){
const char*_tmp440;void*_tmp43F[1];struct Cyc_String_pa_PrintArg_struct _tmp43E;(_tmp43E.tag=0,((_tmp43E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp287).f2)),((_tmp43F[0]=& _tmp43E,Cyc_Tcutil_warn((*_tmp287).f1,((_tmp440="%s is exported but not defined",_tag_dyneither(_tmp440,sizeof(char),31))),_tag_dyneither(_tmp43F,sizeof(void*),1)))))));}}}
# 1170
;_pop_region(temp);}
# 1180
goto _LL121;};_LL121:;}}
# 1185
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1187
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1204 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1206
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1208
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple18{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1212
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp28B=d->r;void*_tmp28C=_tmp28B;struct Cyc_Absyn_Vardecl*_tmp28E;struct Cyc_List_List**_tmp290;struct Cyc_List_List**_tmp292;struct Cyc_List_List**_tmp294;struct Cyc_List_List**_tmp296;_LL150: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp28C;if(_tmp28D->tag != 0)goto _LL152;else{_tmp28E=_tmp28D->f1;}}_LL151:
# 1217
 if((env->in_cinclude  || _tmp28E->sc != Cyc_Absyn_Extern  && _tmp28E->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp28E->name,Cyc_Absyn_uniquergn_qvar))
# 1220
return 1;{
# 1222
struct _tuple0*_tmp297=_tmp28E->name;union Cyc_Absyn_Nmspace _tmp299;struct _dyneither_ptr*_tmp29A;struct _tuple0*_tmp298=_tmp297;_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp29B=_tmp299;struct Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29D;struct Cyc_List_List*_tmp29E;_LL15D: if((_tmp29B.Loc_n).tag != 4)goto _LL15F;_LL15E:
 ns=0;goto _LL15C;_LL15F: if((_tmp29B.Rel_n).tag != 1)goto _LL161;_tmp29C=(struct Cyc_List_List*)(_tmp29B.Rel_n).val;_LL160:
 ns=_tmp29C;goto _LL15C;_LL161: if((_tmp29B.C_n).tag != 3)goto _LL163;_tmp29D=(struct Cyc_List_List*)(_tmp29B.C_n).val;_LL162:
 ns=_tmp29D;goto _LL15C;_LL163: if((_tmp29B.Abs_n).tag != 2)goto _LL15C;_tmp29E=(struct Cyc_List_List*)(_tmp29B.Abs_n).val;_LL164:
 ns=_tmp29E;goto _LL15C;_LL15C:;}{
# 1230
struct _tuple18*_tmp29F=((struct _tuple18*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp2A0=(*_tmp29F).f1;
int _tmp2A1=(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2A0->ordinaries,_tmp29A)).f2;
if(!_tmp2A1)
(*_tmp29F).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp29F).f2,_tmp29A);
# 1238
return _tmp2A1;};};};_LL152: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp28F=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp28C;if(_tmp28F->tag != 11)goto _LL154;else{_tmp290=(struct Cyc_List_List**)& _tmp28F->f1;}}_LL153:
 _tmp292=_tmp290;goto _LL155;_LL154: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp291=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp28C;if(_tmp291->tag != 10)goto _LL156;else{_tmp292=(struct Cyc_List_List**)& _tmp291->f2;}}_LL155:
 _tmp294=_tmp292;goto _LL157;_LL156: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp293=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp28C;if(_tmp293->tag != 9)goto _LL158;else{_tmp294=(struct Cyc_List_List**)& _tmp293->f2;}}_LL157:
# 1242
*_tmp294=Cyc_Tc_treeshake_f(env,*_tmp294);
return 1;_LL158: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp295=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp28C;if(_tmp295->tag != 12)goto _LL15A;else{_tmp296=(struct Cyc_List_List**)& _tmp295->f1;}}_LL159: {
# 1245
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp296=Cyc_Tc_treeshake_f(env,*_tmp296);
env->in_cinclude=in_cinclude;
return 1;}_LL15A:;_LL15B:
 return 1;_LL14F:;}
# 1254
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1258
static struct _tuple18*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1260
struct _tuple18*_tmp441;return(_tmp441=_region_malloc(rgn,sizeof(*_tmp441)),((_tmp441->f1=ge,((_tmp441->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp441)))));}struct _tuple19{struct Cyc_List_List*f1;struct _tuple18*f2;};
# 1263
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp2A3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2A3;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp442;struct Cyc_Tc_TreeshakeEnv _tmp2A4=(_tmp442.rgn=rgn,((_tmp442.in_cinclude=0,((_tmp442.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple18*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp442)))));
struct Cyc_List_List*_tmp2A5=Cyc_Tc_treeshake_f(& _tmp2A4,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp2A4.nsdict)){
struct Cyc_List_List*_tmp2A6=_tmp2A5;_npop_handler(0);return _tmp2A6;}{
# 1271
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp2A4.nsdict);
struct _tuple19 _tmp2A7=*((struct _tuple19*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp2A4.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple19*))Cyc_Iter_next)(iter,& _tmp2A7)){
struct Cyc_Tcenv_Genv*_tmp2A9;struct Cyc_Set_Set*_tmp2AA;struct _tuple19 _tmp2A8=_tmp2A7;_tmp2A9=(_tmp2A8.f2)->f1;_tmp2AA=(_tmp2A8.f2)->f2;
_tmp2A9->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple14*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp2A9->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple14*y))Cyc_Tc_treeshake_remove_f,_tmp2AA,_tmp2A9->ordinaries);}{
# 1277
struct Cyc_List_List*_tmp2AB=_tmp2A5;_npop_handler(0);return _tmp2AB;};};}
# 1265
;_pop_region(rgn);}
