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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 386
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 391
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 462 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 465
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 529
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 555
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 562
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 580
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 936 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 944
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 950
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 953
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 956
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 964
void*Cyc_Absyn_compress_kb(void*);
# 969
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 993
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 1015
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1026
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1098
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1186
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
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 73
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 77
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 88
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 105 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 112
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 117
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 121
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 124
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 197
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
void*_tmpB=Cyc_Tcutil_compress(t);void*_tmpC=_tmpB;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->tag == 9){_LL1: _tmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).attributes;_tmpE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).args;_LL2: {
# 87
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE);
for(0;_tmpD != 0;_tmpD=_tmpD->tl){
void*_tmpF=(void*)_tmpD->hd;void*_tmp10=_tmpF;int _tmp11;int _tmp12;int _tmp13;switch(*((int*)_tmp10)){case 20: _LL6: _tmp13=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10)->f1;_LL7:
# 91
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp13,nargs,(void*)_tmpD->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,init_params,noconsume_params);{
struct _tuple8*_tmp14=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpE,_tmp13 - 1);struct _tuple8*_tmp15=_tmp14;void*_tmp16;_LLF: _tmp16=_tmp15->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp2A="initializes attribute allowed only on";_tag_dyneither(_tmp2A,sizeof(char),38);});
{void*_tmp17=Cyc_Tcutil_compress(_tmp16);void*_tmp18=_tmp17;void*_tmp19;union Cyc_Absyn_Constraint*_tmp1A;union Cyc_Absyn_Constraint*_tmp1B;union Cyc_Absyn_Constraint*_tmp1C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->tag == 5){_LL12: _tmp19=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).elt_typ;_tmp1A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).nullable;_tmp1B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).bounds;_tmp1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).zero_term;_LL13:
# 98
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1A))
({struct Cyc_String_pa_PrintArg_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1D[1]={& _tmp1F};Cyc_Tcutil_terr(loc,({const char*_tmp1E="%s non-null pointers";_tag_dyneither(_tmp1E,sizeof(char),21);}),_tag_dyneither(_tmp1D,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_bound_one(_tmp1B))
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp20[1]={& _tmp22};Cyc_Tcutil_terr(loc,({const char*_tmp21="%s pointers of size 1";_tag_dyneither(_tmp21,sizeof(char),22);}),_tag_dyneither(_tmp20,sizeof(void*),1));});});
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1C))
({struct Cyc_String_pa_PrintArg_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp23[1]={& _tmp25};Cyc_Tcutil_terr(loc,({const char*_tmp24="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp24,sizeof(char),42);}),_tag_dyneither(_tmp23,sizeof(void*),1));});});
goto _LL11;}else{_LL14: _LL15:
# 106
({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp26[1]={& _tmp28};Cyc_Tcutil_terr(loc,({const char*_tmp27="%s pointers";_tag_dyneither(_tmp27,sizeof(char),12);}),_tag_dyneither(_tmp26,sizeof(void*),1));});});}_LL11:;}
# 108
init_params.f1=({struct Cyc_List_List*_tmp29=_region_malloc(temp,sizeof(*_tmp29));_tmp29->hd=(void*)_tmp13;_tmp29->tl=init_params.f1;_tmp29;});
goto _LL5;};};case 21: _LL8: _tmp12=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp10)->f1;_LL9:
# 111
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp12,nargs,(void*)_tmpD->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp12,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp12,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp2B=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpE,_tmp12 - 1);struct _tuple8*_tmp2C=_tmp2B;void*_tmp2D;_LL17: _tmp2D=_tmp2C->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2D,0))
({void*_tmp2E=0;Cyc_Tcutil_terr(loc,({const char*_tmp2F="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp2F,sizeof(char),55);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});
nolive_unique_params.f1=({struct Cyc_List_List*_tmp30=_region_malloc(temp,sizeof(*_tmp30));_tmp30->hd=(void*)_tmp12;_tmp30->tl=nolive_unique_params.f1;_tmp30;});
goto _LL5;};case 22: _LLA: _tmp11=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp10)->f1;_LLB:
# 120
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp11,nargs,(void*)_tmpD->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp11,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp11,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp31=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpE,_tmp11 - 1);struct _tuple8*_tmp32=_tmp31;void*_tmp33;_LL1A: _tmp33=_tmp32->f3;_LL1B:;
# 125
noconsume_params.f1=({struct Cyc_List_List*_tmp34=_region_malloc(temp,sizeof(*_tmp34));_tmp34->hd=(void*)_tmp11;_tmp34->tl=noconsume_params.f1;_tmp34;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 130
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp35=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp36="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp36,sizeof(char),34);}),_tag_dyneither(_tmp35,sizeof(void*),0));});}_LL0:;}
# 82
;_pop_region(temp);}struct _tuple14{void*f1;int f2;};
# 136
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 140
struct Cyc_Absyn_Vardecl*_tmp3D=vd;enum Cyc_Absyn_Scope _tmp3E;union Cyc_Absyn_Nmspace _tmp3F;struct _dyneither_ptr*_tmp40;void*_tmp41;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_List_List*_tmp43;_LL1D: _tmp3E=_tmp3D->sc;_tmp3F=(_tmp3D->name)->f1;_tmp40=(_tmp3D->name)->f2;_tmp41=_tmp3D->type;_tmp42=_tmp3D->initializer;_tmp43=_tmp3D->attributes;_LL1E:;
# 146
{union Cyc_Absyn_Nmspace _tmp44=_tmp3F;if((_tmp44.Rel_n).tag == 1){_LL20: _LL21:
# 149
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL1F;}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 155
{void*_tmp45=Cyc_Tcutil_compress(_tmp41);void*_tmp46=_tmp45;void*_tmp47;struct Cyc_Absyn_Tqual _tmp48;union Cyc_Absyn_Constraint*_tmp49;unsigned int _tmp4A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46)->tag == 8){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46)->f1).num_elts == 0){_LL25: _tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46)->f1).elt_type;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46)->f1).tq;_tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46)->f1).zero_term;_tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46)->f1).zt_loc;if(_tmp42 != 0){_LL26:
# 157
{void*_tmp4B=_tmp42->r;void*_tmp4C=_tmp4B;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;struct Cyc_Absyn_Exp*_tmp4F;struct Cyc_Absyn_Exp*_tmp50;struct _dyneither_ptr _tmp51;struct _dyneither_ptr _tmp52;switch(*((int*)_tmp4C)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C)->f1).Wstring_c).tag){case 8: _LL2A: _tmp52=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C)->f1).String_c).val;_LL2B:
# 159
 _tmp41=(vd->type=Cyc_Absyn_array_typ(_tmp47,_tmp48,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp52,sizeof(char)),0),_tmp49,_tmp4A));
goto _LL29;case 9: _LL2C: _tmp51=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C)->f1).Wstring_c).val;_LL2D:
# 162
 _tmp41=(vd->type=Cyc_Absyn_array_typ(_tmp47,_tmp48,Cyc_Absyn_uint_exp(1,0),_tmp49,_tmp4A));
goto _LL29;default: goto _LL36;}case 26: _LL2E: _tmp50=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LL2F:
 _tmp4F=_tmp50;goto _LL31;case 27: _LL30: _tmp4F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_LL31:
# 167
 _tmp41=(vd->type=Cyc_Absyn_array_typ(_tmp47,_tmp48,_tmp4F,_tmp49,_tmp4A));
goto _LL29;case 35: _LL32: _tmp4E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LL33:
 _tmp4D=_tmp4E;goto _LL35;case 25: _LL34: _tmp4D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_LL35:
# 171
 _tmp41=(vd->type=Cyc_Absyn_array_typ(_tmp47,_tmp48,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4D),0),_tmp49,_tmp4A));
goto _LL29;default: _LL36: _LL37:
 goto _LL29;}_LL29:;}
# 175
goto _LL24;}else{goto _LL27;}}else{goto _LL27;}}else{_LL27: _LL28:
 goto _LL24;}_LL24:;}
# 179
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp41);
# 181
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp41);
# 184
{void*_tmp53=Cyc_Tcutil_compress(_tmp41);void*_tmp54=_tmp53;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54)->tag == 8){_LL39: _LL3A:
 vd->escapes=0;goto _LL38;}else{_LL3B: _LL3C:
 vd->escapes=1;goto _LL38;}_LL38:;}
# 189
if(Cyc_Tcutil_is_function_type(_tmp41)){
_tmp43=Cyc_Tcutil_transfer_fn_type_atts(_tmp41,_tmp43);
Cyc_Tc_fnTypeAttsOK(te,_tmp41,loc);}
# 194
if(_tmp3E == Cyc_Absyn_Extern  || _tmp3E == Cyc_Absyn_ExternC){
if(_tmp42 != 0  && !in_cinclude)
({void*_tmp55=0;Cyc_Tcutil_terr(loc,({const char*_tmp56="extern declaration should not have initializer";_tag_dyneither(_tmp56,sizeof(char),47);}),_tag_dyneither(_tmp55,sizeof(void*),0));});}else{
if(!Cyc_Tcutil_is_function_type(_tmp41)){
# 201
for(0;_tmp43 != 0;_tmp43=_tmp43->tl){
void*_tmp57=(void*)_tmp43->hd;void*_tmp58=_tmp57;switch(*((int*)_tmp58)){case 6: _LL3E: _LL3F:
 goto _LL41;case 8: _LL40: _LL41:
# 206
 goto _LL43;case 9: _LL42: _LL43:
 goto _LL45;case 10: _LL44: _LL45:
 goto _LL47;case 11: _LL46: _LL47:
 goto _LL49;case 12: _LL48: _LL49:
 goto _LL4B;case 13: _LL4A: _LL4B:
 goto _LL4D;case 14: _LL4C: _LL4D:
 continue;default: _LL4E: _LL4F:
# 214
({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name));({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp43->hd));({void*_tmp59[2]={& _tmp5B,& _tmp5C};Cyc_Tcutil_terr(loc,({const char*_tmp5A="bad attribute %s for variable %s";_tag_dyneither(_tmp5A,sizeof(char),33);}),_tag_dyneither(_tmp59,sizeof(void*),2));});});});
goto _LL3D;}_LL3D:;}
# 219
if(_tmp42 == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp41))
({struct Cyc_String_pa_PrintArg_struct _tmp60;_tmp60.tag=0;_tmp60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp41));({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));({void*_tmp5D[2]={& _tmp5F,& _tmp60};Cyc_Tcutil_terr(loc,({const char*_tmp5E="initializer required for variable %s of type %s";_tag_dyneither(_tmp5E,sizeof(char),48);}),_tag_dyneither(_tmp5D,sizeof(void*),2));});});});}else{
# 224
struct Cyc_Absyn_Exp*_tmp61=_tmp42;
# 228
struct _handler_cons _tmp62;_push_handler(& _tmp62);{int _tmp64=0;if(setjmp(_tmp62.handler))_tmp64=1;if(!_tmp64){
{void*_tmp65=Cyc_Tcexp_tcExpInitializer(te,& _tmp41,_tmp61);
if(!Cyc_Tcutil_coerce_assign(te,_tmp61,_tmp41)){
struct _dyneither_ptr _tmp66=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp67=" declared with type ";
struct _dyneither_ptr _tmp68=Cyc_Absynpp_typ2string(_tmp41);
const char*_tmp69=" but initialized with type ";
struct _dyneither_ptr _tmp6A=Cyc_Absynpp_typ2string(_tmp65);
if((((Cyc_strlen((struct _dyneither_ptr)_tmp66)+ Cyc_strlen(({const char*_tmp6B=_tmp67;_tag_dyneither(_tmp6B,sizeof(char),_get_zero_arr_size_char((void*)_tmp6B,21));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp68))+ Cyc_strlen(({const char*_tmp6C=_tmp69;_tag_dyneither(_tmp6C,sizeof(char),_get_zero_arr_size_char((void*)_tmp6C,28));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp6A)> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6A);({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)({const char*_tmp75=_tmp69;_tag_dyneither(_tmp75,sizeof(char),_get_zero_arr_size_char((void*)_tmp75,28));});({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp68);({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct _dyneither_ptr)({const char*_tmp74=_tmp67;_tag_dyneither(_tmp74,sizeof(char),_get_zero_arr_size_char((void*)_tmp74,21));});({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp66);({void*_tmp6D[5]={& _tmp6F,& _tmp70,& _tmp71,& _tmp72,& _tmp73};Cyc_Tcutil_terr(loc,({const char*_tmp6E="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp6E,sizeof(char),16);}),_tag_dyneither(_tmp6D,sizeof(void*),5));});});});});});});else{
# 239
({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6A);({struct Cyc_String_pa_PrintArg_struct _tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _dyneither_ptr)({const char*_tmp7E=_tmp69;_tag_dyneither(_tmp7E,sizeof(char),_get_zero_arr_size_char((void*)_tmp7E,28));});({struct Cyc_String_pa_PrintArg_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp68);({struct Cyc_String_pa_PrintArg_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct _dyneither_ptr)({const char*_tmp7D=_tmp67;_tag_dyneither(_tmp7D,sizeof(char),_get_zero_arr_size_char((void*)_tmp7D,21));});({struct Cyc_String_pa_PrintArg_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp66);({void*_tmp76[5]={& _tmp78,& _tmp79,& _tmp7A,& _tmp7B,& _tmp7C};Cyc_Tcutil_terr(loc,({const char*_tmp77="%s%s%s%s%s";_tag_dyneither(_tmp77,sizeof(char),11);}),_tag_dyneither(_tmp76,sizeof(void*),5));});});});});});});}
Cyc_Tcutil_explain_failure();
# 242
if(!Cyc_Tcutil_is_const_exp(_tmp61))
({void*_tmp7F=0;Cyc_Tcutil_terr(loc,({const char*_tmp80="initializer is not a constant expression";_tag_dyneither(_tmp80,sizeof(char),41);}),_tag_dyneither(_tmp7F,sizeof(void*),0));});}}
# 229
;_pop_handler();}else{void*_tmp63=(void*)_exn_thrown;void*_tmp81=_tmp63;void*_tmp82;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp81)->tag == Cyc_Tcenv_Env_error){_LL51: _LL52:
# 247
({void*_tmp83=0;Cyc_Tcutil_terr(loc,({const char*_tmp84="initializer is not a constant expression";_tag_dyneither(_tmp84,sizeof(char),41);}),_tag_dyneither(_tmp83,sizeof(void*),0));});
goto _LL50;}else{_LL53: _tmp82=_tmp81;_LL54:(int)_rethrow(_tmp82);}_LL50:;}};}}else{
# 253
for(0;_tmp43 != 0;_tmp43=_tmp43->tl){
void*_tmp85=(void*)_tmp43->hd;void*_tmp86=_tmp85;switch(*((int*)_tmp86)){case 0: _LL56: _LL57:
# 256
 goto _LL59;case 1: _LL58: _LL59:
 goto _LL5B;case 2: _LL5A: _LL5B:
 goto _LL5D;case 3: _LL5C: _LL5D:
 goto _LL5F;case 4: _LL5E: _LL5F:
 goto _LL61;case 19: _LL60: _LL61:
 goto _LL63;case 20: _LL62: _LL63:
 goto _LL65;case 23: _LL64: _LL65:
 goto _LL67;case 26: _LL66: _LL67:
 goto _LL69;case 5: _LL68: _LL69:
({void*_tmp87=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp88="tcVardecl: fn type atts in function var decl";_tag_dyneither(_tmp88,sizeof(char),45);}),_tag_dyneither(_tmp87,sizeof(void*),0));});case 6: _LL6A: _LL6B:
# 267
 goto _LL6D;case 7: _LL6C: _LL6D:
# 269
({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp43->hd));({void*_tmp89[1]={& _tmp8B};Cyc_Tcutil_terr(loc,({const char*_tmp8A="bad attribute %s in function declaration";_tag_dyneither(_tmp8A,sizeof(char),41);}),_tag_dyneither(_tmp89,sizeof(void*),1));});});
goto _LL55;default: _LL6E: _LL6F:
 continue;}_LL55:;}}}
# 277
{struct _handler_cons _tmp8C;_push_handler(& _tmp8C);{int _tmp8E=0;if(setjmp(_tmp8C.handler))_tmp8E=1;if(!_tmp8E){
{struct _tuple14*_tmp8F=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp40);
void*_tmp90=(*_tmp8F).f1;void*_tmp91=_tmp90;void*_tmp92;switch(*((int*)_tmp91)){case 0: _LL71: _tmp92=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp91)->f1;_LL72: {
# 281
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp93=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp9A;_tmp9A.tag=1;_tmp9A.f1=vd;_tmp9A;});_tmp99;});
void**_tmp94=Cyc_Tcdecl_merge_binding(_tmp92,(void*)_tmp93,loc,Cyc_Tc_tc_msg);
if(_tmp94 == 0){_npop_handler(0);return;}{
void*_tmp95=*_tmp94;
# 286
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp95 != _tmp92  || (*_tmp8F).f2)
# 289
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp40,({struct _tuple14*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp98;_tmp98.tag=0;_tmp98.f1=_tmp95;_tmp98;});_tmp97;});_tmp96->f2=1;_tmp96;}));}
# 291
_npop_handler(0);return;};}case 1: _LL73: _LL74:
# 293
 if(Cyc_Tc_aggressive_warn)
({void*_tmp9B=0;Cyc_Tcutil_warn(loc,({const char*_tmp9C="variable declaration shadows previous struct declaration";_tag_dyneither(_tmp9C,sizeof(char),57);}),_tag_dyneither(_tmp9B,sizeof(void*),0));});
goto _LL70;case 2: _LL75: _LL76:
# 297
 if(Cyc_Tc_aggressive_warn)
({void*_tmp9D=0;Cyc_Tcutil_warn(loc,({const char*_tmp9E="variable declaration shadows previous datatype constructor";_tag_dyneither(_tmp9E,sizeof(char),59);}),_tag_dyneither(_tmp9D,sizeof(void*),0));});
goto _LL70;case 4: _LL77: _LL78:
 goto _LL7A;default: _LL79: _LL7A:
# 302
 if(Cyc_Tc_aggressive_warn)
({void*_tmp9F=0;Cyc_Tcutil_warn(loc,({const char*_tmpA0="variable declaration shadows previous enum tag";_tag_dyneither(_tmpA0,sizeof(char),47);}),_tag_dyneither(_tmp9F,sizeof(void*),0));});
goto _LL70;}_LL70:;}
# 278
;_pop_handler();}else{void*_tmp8D=(void*)_exn_thrown;void*_tmpA1=_tmp8D;void*_tmpA2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA1)->tag == Cyc_Dict_Absent){_LL7C: _LL7D:
# 306
 goto _LL7B;}else{_LL7E: _tmpA2=_tmpA1;_LL7F:(int)_rethrow(_tmpA2);}_LL7B:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp40,({struct _tuple14*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpA7;_tmpA7.tag=1;_tmpA7.f1=vd;_tmpA7;});_tmpA6;});_tmpA5;});_tmpA4;});_tmpA3->f2=0;_tmpA3;}));}
# 312
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmpA8=n;union Cyc_Absyn_Nmspace _tmpA9;struct _dyneither_ptr*_tmpAA;_LL81: _tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;_LL82:;{
union Cyc_Absyn_Nmspace _tmpAB=_tmpA9;if((_tmpAB.Abs_n).tag == 2){if((_tmpAB.Abs_n).val == 0){_LL84: _LL85:
# 316
 return Cyc_strcmp((struct _dyneither_ptr)*_tmpAA,({const char*_tmpAC="main";_tag_dyneither(_tmpAC,sizeof(char),5);}))== 0;}else{goto _LL86;}}else{_LL86: _LL87:
 return 0;}_LL83:;};}
# 321
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 323
struct _dyneither_ptr*v=(*fd->name).f2;
# 327
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmpAD=0;Cyc_Tcutil_terr(loc,({const char*_tmpAE="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmpAE,sizeof(char),54);}),_tag_dyneither(_tmpAD,sizeof(void*),0));});
# 330
{union Cyc_Absyn_Nmspace _tmpAF=(*fd->name).f1;union Cyc_Absyn_Nmspace _tmpB0=_tmpAF;if((_tmpB0.Rel_n).tag == 1){if((_tmpB0.Rel_n).val == 0){_LL89: _LL8A:
# 333
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || te->in_extern_c_include);
goto _LL88;}else{goto _LL8B;}}else{_LL8B: _LL8C:
# 336
 goto _LL88;}_LL88:;}
# 339
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 341
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 343
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 346
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpB1=(void*)atts->hd;void*_tmpB2=_tmpB1;switch(*((int*)_tmpB2)){case 7: _LL8E: _LL8F:
 goto _LL91;case 6: _LL90: _LL91:
# 350
({struct Cyc_String_pa_PrintArg_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmpB3[1]={& _tmpB5};Cyc_Tcutil_terr(loc,({const char*_tmpB4="bad attribute %s for function";_tag_dyneither(_tmpB4,sizeof(char),30);}),_tag_dyneither(_tmpB3,sizeof(void*),1));});});
goto _LL8D;default: _LL92: _LL93:
 goto _LL8D;}_LL8D:;}}
# 356
{struct _handler_cons _tmpB6;_push_handler(& _tmpB6);{int _tmpB8=0;if(setjmp(_tmpB6.handler))_tmpB8=1;if(!_tmpB8){
{struct _tuple14*_tmpB9=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_tmpBA=(*_tmpB9).f1;void*_tmpBB=_tmpBA;void*_tmpBC;switch(*((int*)_tmpBB)){case 0: _LL95: _tmpBC=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpBB)->f1;_LL96: {
# 360
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBD=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpC4;_tmpC4.tag=2;_tmpC4.f1=fd;_tmpC4;});_tmpC3;});
void**_tmpBE=Cyc_Tcdecl_merge_binding(_tmpBC,(void*)_tmpBD,loc,Cyc_Tc_tc_msg);
if(_tmpBE == 0)goto _LL94;{
void*_tmpBF=*_tmpBE;
# 365
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpBF == _tmpBC  && (*_tmpB9).f2))
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple14*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=_tmpBF;_tmpC2;});_tmpC1;});_tmpC0->f2=1;_tmpC0;}));}
# 369
goto _LL94;};}case 1: _LL97: _LL98:
# 371
({void*_tmpC5=0;Cyc_Tcutil_warn(loc,({const char*_tmpC6="function declaration shadows previous type declaration";_tag_dyneither(_tmpC6,sizeof(char),55);}),_tag_dyneither(_tmpC5,sizeof(void*),0));});
goto _LL94;case 2: _LL99: _LL9A:
# 374
({void*_tmpC7=0;Cyc_Tcutil_warn(loc,({const char*_tmpC8="function declaration shadows previous datatype constructor";_tag_dyneither(_tmpC8,sizeof(char),59);}),_tag_dyneither(_tmpC7,sizeof(void*),0));});
goto _LL94;case 4: _LL9B: _LL9C:
 goto _LL9E;default: _LL9D: _LL9E:
# 378
({void*_tmpC9=0;Cyc_Tcutil_warn(loc,({const char*_tmpCA="function declaration shadows previous enum tag";_tag_dyneither(_tmpCA,sizeof(char),47);}),_tag_dyneither(_tmpC9,sizeof(void*),0));});
goto _LL94;}_LL94:;}
# 357
;_pop_handler();}else{void*_tmpB7=(void*)_exn_thrown;void*_tmpCB=_tmpB7;void*_tmpCC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpCB)->tag == Cyc_Dict_Absent){_LLA0: _LLA1:
# 382
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpCD=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpD4;_tmpD4.tag=2;_tmpD4.f1=fd;_tmpD4;});_tmpD3;});
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple14*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpD0;_tmpD0.tag=0;_tmpD0.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpD2;_tmpD2.tag=2;_tmpD2.f1=fd;_tmpD2;});_tmpD1;});_tmpD0;});_tmpCF;});_tmpCE->f2=0;_tmpCE;}));}
# 387
goto _LL9F;}else{_LLA2: _tmpCC=_tmpCB;_LLA3:(int)_rethrow(_tmpCC);}_LL9F:;}};}
# 391
if(te->in_extern_c_include)return;
# 396
{struct _RegionHandle _tmpD5=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpD5;_push_region(fnrgn);
{struct Cyc_Tcenv_Fenv*_tmpD6=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmpD7=({struct Cyc_Tcenv_Tenv*_tmpDD=_region_malloc(fnrgn,sizeof(*_tmpDD));_tmpDD->ns=te->ns;_tmpDD->ae=te->ae;_tmpDD->le=_tmpD6;_tmpDD->allow_valueof=0;_tmpDD->in_extern_c_include=0;_tmpDD;});
# 400
struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int _tmpDA=0;if(setjmp(_tmpD8.handler))_tmpDA=1;if(!_tmpDA){
Cyc_Tcstmt_tcStmt(_tmpD7,fd->body,0);
# 404
Cyc_Tcenv_check_delayed_effects(_tmpD7);
Cyc_Tcenv_check_delayed_constraints(_tmpD7);
# 401
;_pop_handler();}else{void*_tmpD9=(void*)_exn_thrown;void*_tmpDB=_tmpD9;void*_tmpDC;if(((struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*)_tmpDB)->tag == Cyc_Tcutil_AbortTypeCheckingFunction){_LLA5: _LLA6:
# 407
 goto _LLA4;}else{_LLA7: _tmpDC=_tmpDB;_LLA8:
 Cyc_Core_rethrow(_tmpDC);}_LLA4:;}};}
# 397
;_pop_region(fnrgn);}
# 412
if(Cyc_Tc_is_main(fd->name)){
# 414
{void*_tmpDE=Cyc_Tcutil_compress(fd->ret_type);void*_tmpDF=_tmpDE;enum Cyc_Absyn_Size_of _tmpE0;switch(*((int*)_tmpDF)){case 0: _LLAC: _LLAD:
({void*_tmpE1=0;Cyc_Tcutil_warn(loc,({const char*_tmpE2="main declared with return type void";_tag_dyneither(_tmpE2,sizeof(char),36);}),_tag_dyneither(_tmpE1,sizeof(void*),0));});goto _LLAB;case 6: _LLAE: _tmpE0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDF)->f2;_LLAF:
 goto _LLAB;default: _LLB0: _LLB1:
# 418
({struct Cyc_String_pa_PrintArg_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type));({void*_tmpE3[1]={& _tmpE5};Cyc_Tcutil_terr(loc,({const char*_tmpE4="main declared with return type %s instead of int or void";_tag_dyneither(_tmpE4,sizeof(char),57);}),_tag_dyneither(_tmpE3,sizeof(void*),1));});});
goto _LLAB;}_LLAB:;}
# 422
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpE6=0;Cyc_Tcutil_terr(loc,({const char*_tmpE7="main declared with varargs";_tag_dyneither(_tmpE7,sizeof(char),27);}),_tag_dyneither(_tmpE6,sizeof(void*),0));});{
struct Cyc_List_List*_tmpE8=fd->args;
if(_tmpE8 != 0){
struct _tuple8*_tmpE9=(struct _tuple8*)_tmpE8->hd;struct _tuple8*_tmpEA=_tmpE9;void*_tmpEB;_LLB3: _tmpEB=_tmpEA->f3;_LLB4:;
{void*_tmpEC=Cyc_Tcutil_compress(_tmpEB);void*_tmpED=_tmpEC;enum Cyc_Absyn_Size_of _tmpEE;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpED)->tag == 6){_LLB6: _tmpEE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpED)->f2;_LLB7:
 goto _LLB5;}else{_LLB8: _LLB9:
# 430
({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpEB));({void*_tmpEF[1]={& _tmpF1};Cyc_Tcutil_terr(loc,({const char*_tmpF0="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpF0,sizeof(char),60);}),_tag_dyneither(_tmpEF,sizeof(void*),1));});});
goto _LLB5;}_LLB5:;}
# 434
_tmpE8=_tmpE8->tl;
if(_tmpE8 != 0){
struct _tuple8*_tmpF2=(struct _tuple8*)_tmpE8->hd;struct _tuple8*_tmpF3=_tmpF2;void*_tmpF4;_LLBB: _tmpF4=_tmpF3->f3;_LLBC:;
_tmpE8=_tmpE8->tl;
if(_tmpE8 != 0)
({void*_tmpF5=0;Cyc_Tcutil_terr(loc,({const char*_tmpF6="main declared with too many arguments";_tag_dyneither(_tmpF6,sizeof(char),38);}),_tag_dyneither(_tmpF5,sizeof(void*),0));});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->v=fd->tvs;_tmp102;});
if(((!Cyc_Tcutil_unify(_tmpF4,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->v=& Cyc_Tcutil_rk;_tmpF7;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->v=& Cyc_Tcutil_rk;_tmpF8;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpF4,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->v=& Cyc_Tcutil_rk;_tmpF9;}),tvs)),
# 446
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->v=& Cyc_Tcutil_rk;_tmpFA;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpF4,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->v=& Cyc_Tcutil_rk;_tmpFB;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->v=& Cyc_Tcutil_rk;_tmpFC;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpF4,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->v=& Cyc_Tcutil_rk;_tmpFD;}),tvs)),
# 453
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->v=& Cyc_Tcutil_rk;_tmpFE;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpF4));({void*_tmpFF[1]={& _tmp101};Cyc_Tcutil_terr(loc,({const char*_tmp100="second argument of main has type %s instead of char??";_tag_dyneither(_tmp100,sizeof(char),54);}),_tag_dyneither(_tmpFF,sizeof(void*),1));});});};}}};}};}
# 463
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 469
{union Cyc_Absyn_Nmspace _tmp103=(*td->name).f1;union Cyc_Absyn_Nmspace _tmp104=_tmp103;if((_tmp104.Rel_n).tag == 1){_LLBE: _LLBF:
# 472
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,td->extern_c);
goto _LLBD;}else{_LLC0: _LLC1:
 goto _LLBD;}_LLBD:;}
# 477
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0;_tmp107.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp105[1]={& _tmp107};Cyc_Tcutil_terr(loc,({const char*_tmp106="redeclaration of typedef %s";_tag_dyneither(_tmp106,sizeof(char),28);}),_tag_dyneither(_tmp105,sizeof(void*),1));});});
return;}
# 482
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 491
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp108=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmp109=_tmp108;struct Cyc_Core_Opt**_tmp10A;struct Cyc_Absyn_Kind*_tmp10B;struct Cyc_Core_Opt**_tmp10C;switch(*((int*)_tmp109)){case 1: _LLC3: _tmp10C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp109)->f1;_LLC4:
# 494
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp10F;_tmp10F.tag=0;_tmp10F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);({void*_tmp10D[1]={& _tmp10F};Cyc_Tcutil_warn(loc,({const char*_tmp10E="type variable %s is not used in typedef definition";_tag_dyneither(_tmp10E,sizeof(char),51);}),_tag_dyneither(_tmp10D,sizeof(void*),1));});});
# 497
*_tmp10C=({struct Cyc_Core_Opt*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmp110;});goto _LLC2;case 2: _LLC5: _tmp10A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp109)->f1;_tmp10B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp109)->f2;_LLC6:
# 504
*_tmp10A=({struct Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->v=Cyc_Tcutil_kind_to_bound(_tmp10B);_tmp111;});
goto _LLC2;default: _LLC7: _LLC8:
 continue;}_LLC2:;}}
# 510
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 513
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 520
struct _RegionHandle _tmp112=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp112;_push_region(uprev_rgn);
# 522
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmp113=(struct _tuple10*)rpo->hd;struct _tuple10*_tmp114=_tmp113;void*_tmp115;void*_tmp116;_LLCA: _tmp115=_tmp114->f1;_tmp116=_tmp114->f2;_LLCB:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmp115);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmp116);}{
# 528
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 531
struct Cyc_List_List*_tmp117=fields;for(0;_tmp117 != 0;_tmp117=_tmp117->tl){
struct Cyc_Absyn_Aggrfield*_tmp118=(struct Cyc_Absyn_Aggrfield*)_tmp117->hd;struct Cyc_Absyn_Aggrfield*_tmp119=_tmp118;struct _dyneither_ptr*_tmp11A;struct Cyc_Absyn_Tqual _tmp11B;void*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_List_List*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;_LLCD: _tmp11A=_tmp119->name;_tmp11B=_tmp119->tq;_tmp11C=_tmp119->type;_tmp11D=_tmp119->width;_tmp11E=_tmp119->attributes;_tmp11F=_tmp119->requires_clause;_LLCE:;
# 534
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp11A))
({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0;_tmp122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp11A);({void*_tmp120[1]={& _tmp122};Cyc_Tcutil_terr(loc,({const char*_tmp121="duplicate member %s";_tag_dyneither(_tmp121,sizeof(char),20);}),_tag_dyneither(_tmp120,sizeof(void*),1));});});
# 538
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp11A,({const char*_tmp123="";_tag_dyneither(_tmp123,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp124=_region_malloc(uprev_rgn,sizeof(*_tmp124));_tmp124->hd=_tmp11A;_tmp124->tl=prev_fields;_tmp124;});{
# 541
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 545
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp117->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp11C);
# 550
(((struct Cyc_Absyn_Aggrfield*)_tmp117->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp117->hd)->tq).print_const,_tmp11C);
# 553
Cyc_Tcutil_check_bitfield(loc,te,_tmp11C,_tmp11D,_tmp11A);
# 555
if((unsigned int)_tmp11F){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmp125=0;Cyc_Tcutil_terr(loc,({const char*_tmp126="@requires clauses are only allowed on union members";_tag_dyneither(_tmp126,sizeof(char),52);}),_tag_dyneither(_tmp125,sizeof(void*),0));});
{struct _RegionHandle _tmp127=_new_region("temp");struct _RegionHandle*temp=& _tmp127;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp128=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp128,0,_tmp11F);}
# 559
;_pop_region(temp);}
# 562
if(!Cyc_Tcutil_is_integral(_tmp11F))
({struct Cyc_String_pa_PrintArg_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 565
Cyc_Absynpp_typ2string((void*)_check_null(_tmp11F->topt)));({void*_tmp129[1]={& _tmp12B};Cyc_Tcutil_terr(_tmp11F->loc,({const char*_tmp12A="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp12A,sizeof(char),54);}),_tag_dyneither(_tmp129,sizeof(void*),1));});});else{
# 567
Cyc_Tcutil_check_type(_tmp11F->loc,te,tvs,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp12D;_tmp12D.tag=18;_tmp12D.f1=_tmp11F;_tmp12D;});_tmp12C;}));{
# 569
struct Cyc_List_List*_tmp12E=Cyc_Relations_exp2relns(uprev_rgn,_tmp11F);
# 576
if(!Cyc_Relations_consistent_relations(_tmp12E))
({void*_tmp12F=0;Cyc_Tcutil_terr(_tmp11F->loc,({const char*_tmp130="@requires clause may be unsatisfiable";_tag_dyneither(_tmp130,sizeof(char),38);}),_tag_dyneither(_tmp12F,sizeof(void*),0));});
# 582
{struct Cyc_List_List*_tmp131=prev_relations;for(0;_tmp131 != 0;_tmp131=_tmp131->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp12E,(struct Cyc_List_List*)_tmp131->hd)))
({void*_tmp132=0;Cyc_Tcutil_terr(_tmp11F->loc,({const char*_tmp133="@requires clause may overlap with previous clauses";_tag_dyneither(_tmp133,sizeof(char),51);}),_tag_dyneither(_tmp132,sizeof(void*),0));});}}
# 587
prev_relations=({struct Cyc_List_List*_tmp134=_region_malloc(uprev_rgn,sizeof(*_tmp134));_tmp134->hd=_tmp12E;_tmp134->tl=prev_relations;_tmp134;});};}}else{
# 590
if(prev_relations != 0)
({void*_tmp135=0;Cyc_Tcutil_terr(loc,({const char*_tmp136="if one field has a @requires clause, they all must";_tag_dyneither(_tmp136,sizeof(char),51);}),_tag_dyneither(_tmp135,sizeof(void*),0));});}};}};
# 522
;_pop_region(uprev_rgn);}
# 596
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 598
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp137=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp138=_tmp137;enum Cyc_Absyn_AliasQual _tmp139;struct Cyc_Core_Opt**_tmp13A;struct Cyc_Absyn_Kind*_tmp13B;struct Cyc_Core_Opt**_tmp13C;struct Cyc_Core_Opt**_tmp13D;enum Cyc_Absyn_AliasQual _tmp13E;struct Cyc_Core_Opt**_tmp13F;switch(*((int*)_tmp138)){case 1: _LLD0: _tmp13F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp138)->f1;_LLD1:
# 601
*_tmp13F=({struct Cyc_Core_Opt*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp140;});continue;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f2)->kind){case Cyc_Absyn_MemKind: _LLD2: _tmp13D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f1;_tmp13E=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f2)->aliasqual;_LLD3:
# 603
 if(constrain_top_kind  && _tmp13E == Cyc_Absyn_Top)
*_tmp13D=({struct Cyc_Core_Opt*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp142=_cycalloc_atomic(sizeof(*_tmp142));_tmp142->kind=Cyc_Absyn_BoxKind;_tmp142->aliasqual=Cyc_Absyn_Aliasable;_tmp142;}));_tmp141;});else{
# 606
*_tmp13D=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp144=_cycalloc_atomic(sizeof(*_tmp144));_tmp144->kind=Cyc_Absyn_BoxKind;_tmp144->aliasqual=_tmp13E;_tmp144;}));_tmp143;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f2)->aliasqual == Cyc_Absyn_Top){_LLD4: _tmp13C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f1;if(constrain_top_kind){_LLD5:
# 609
*_tmp13C=({struct Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp146=_cycalloc_atomic(sizeof(*_tmp146));_tmp146->kind=Cyc_Absyn_BoxKind;_tmp146->aliasqual=Cyc_Absyn_Aliasable;_tmp146;}));_tmp145;});
continue;}else{goto _LLD6;}}else{goto _LLD6;}default: _LLD6: _tmp13A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f1;_tmp13B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f2;_LLD7:
# 612
*_tmp13A=({struct Cyc_Core_Opt*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->v=Cyc_Tcutil_kind_to_bound(_tmp13B);_tmp147;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp138)->f1)->kind == Cyc_Absyn_MemKind){_LLD8: _tmp139=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp138)->f1)->aliasqual;_LLD9:
# 614
({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp14D=_cycalloc_atomic(sizeof(*_tmp14D));_tmp14D->kind=Cyc_Absyn_MemKind;_tmp14D->aliasqual=_tmp139;_tmp14D;})));({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n);({void*_tmp148[3]={& _tmp14A,& _tmp14B,& _tmp14C};Cyc_Tcutil_terr(loc,({const char*_tmp149="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp149,sizeof(char),57);}),_tag_dyneither(_tmp148,sizeof(void*),3));});});});});
continue;}else{_LLDA: _LLDB:
 continue;}}_LLCF:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 622
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp14E=(*ad->name).f2;
# 629
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp14F=(void*)atts->hd;void*_tmp150=_tmp14F;switch(*((int*)_tmp150)){case 7: _LLDD: _LLDE:
 goto _LLE0;case 6: _LLDF: _LLE0:
 continue;default: _LLE1: _LLE2:
# 634
({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14E);({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp151[2]={& _tmp153,& _tmp154};Cyc_Tcutil_terr(loc,({const char*_tmp152="bad attribute %s in %s definition";_tag_dyneither(_tmp152,sizeof(char),34);}),_tag_dyneither(_tmp151,sizeof(void*),2));});});});
goto _LLDC;}_LLDC:;}}{
# 639
struct Cyc_List_List*_tmp155=ad->tvs;
# 641
{union Cyc_Absyn_Nmspace _tmp156=(*ad->name).f1;union Cyc_Absyn_Nmspace _tmp157=_tmp156;if((_tmp157.Rel_n).tag == 1){if((_tmp157.Rel_n).val == 0){_LLE4: _LLE5:
# 644
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLE3;}else{goto _LLE6;}}else{_LLE6: _LLE7:
# 647
 goto _LLE3;}_LLE3:;}
# 651
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 653
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 657
{struct _tuple15 _tmp158=({struct _tuple15 _tmp176;_tmp176.f1=ad->impl;_tmp176.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp14E);_tmp176;});struct _tuple15 _tmp159=_tmp158;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15C;int _tmp15D;struct Cyc_Absyn_Aggrdecl**_tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;int _tmp162;struct Cyc_Absyn_Aggrdecl**_tmp163;if(_tmp159.f1 == 0){if(_tmp159.f2 == 0){_LLE9: _LLEA:
# 660
 Cyc_Tc_rule_out_memkind(loc,_tmp14E,_tmp155,0);
# 662
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp14E,({struct Cyc_Absyn_Aggrdecl**_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164[0]=ad;_tmp164;}));
goto _LLE8;}else{_LLEF: _tmp163=*_tmp159.f2;_LLF0: {
# 724
struct Cyc_Absyn_Aggrdecl*_tmp175=Cyc_Tcdecl_merge_aggrdecl(*_tmp163,ad,loc,Cyc_Tc_tc_msg);
if(_tmp175 == 0)
return;else{
# 728
Cyc_Tc_rule_out_memkind(loc,_tmp14E,_tmp155,0);
# 731
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp14E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 734
*_tmp163=_tmp175;
ad=_tmp175;
goto _LLE8;}}}}else{if(_tmp159.f2 == 0){_LLEB: _tmp15F=(_tmp159.f1)->exist_vars;_tmp160=(_tmp159.f1)->rgn_po;_tmp161=(_tmp159.f1)->fields;_tmp162=(_tmp159.f1)->tagged;_LLEC: {
# 667
struct Cyc_Absyn_Aggrdecl**_tmp165=({struct Cyc_Absyn_Aggrdecl**_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_Absyn_Aggrdecl*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->kind=ad->kind;_tmp16E->sc=Cyc_Absyn_Extern;_tmp16E->name=ad->name;_tmp16E->tvs=_tmp155;_tmp16E->impl=0;_tmp16E->attributes=ad->attributes;_tmp16E->expected_mem_kind=0;_tmp16E;});_tmp16D;});
# 669
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp14E,_tmp165);
# 674
Cyc_Tcutil_check_unique_tvars(loc,_tmp15F);
# 676
Cyc_Tcutil_add_tvar_identities(_tmp15F);
# 679
if(_tmp162  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp166=0;Cyc_Tcutil_terr(loc,({const char*_tmp167="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp167,sizeof(char),56);}),_tag_dyneither(_tmp166,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp155,_tmp15F),_tmp160,_tmp161);
# 683
Cyc_Tc_rule_out_memkind(loc,_tmp14E,_tmp155,0);
# 686
Cyc_Tc_rule_out_memkind(loc,_tmp14E,_tmp15F,1);
# 688
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp162){
# 691
struct Cyc_List_List*f=_tmp161;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0;_tmp16C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14E);({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0;_tmp16A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);({void*_tmp168[3]={& _tmp16A,& _tmp16B,& _tmp16C};Cyc_Tcutil_warn(loc,({const char*_tmp169="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp169,sizeof(char),98);}),_tag_dyneither(_tmp168,sizeof(void*),3));});});});});}}
# 697
*_tmp165=ad;
goto _LLE8;}}else{_LLED: _tmp15A=(_tmp159.f1)->exist_vars;_tmp15B=(_tmp159.f1)->rgn_po;_tmp15C=(_tmp159.f1)->fields;_tmp15D=(_tmp159.f1)->tagged;_tmp15E=*_tmp159.f2;_LLEE:
# 701
 if(ad->kind != (*_tmp15E)->kind)
({void*_tmp16F=0;Cyc_Tcutil_terr(loc,({const char*_tmp170="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp170,sizeof(char),52);}),_tag_dyneither(_tmp16F,sizeof(void*),0));});{
# 704
struct Cyc_Absyn_Aggrdecl*_tmp171=*_tmp15E;
# 706
*_tmp15E=({struct Cyc_Absyn_Aggrdecl*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->kind=ad->kind;_tmp172->sc=Cyc_Absyn_Extern;_tmp172->name=ad->name;_tmp172->tvs=_tmp155;_tmp172->impl=0;_tmp172->attributes=ad->attributes;_tmp172->expected_mem_kind=0;_tmp172;});
# 712
Cyc_Tcutil_check_unique_tvars(loc,_tmp15A);
# 714
Cyc_Tcutil_add_tvar_identities(_tmp15A);
# 716
if(_tmp15D  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp173=0;Cyc_Tcutil_terr(loc,({const char*_tmp174="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp174,sizeof(char),56);}),_tag_dyneither(_tmp173,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp155,_tmp15A),_tmp15B,_tmp15C);
# 720
*_tmp15E=_tmp171;
_tmp163=_tmp15E;goto _LLF0;};}}_LLE8:;}
# 740
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp14E,({struct _tuple14*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=(void*)({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp179;_tmp179.tag=1;_tmp179.f1=ad;_tmp179;});_tmp178;});_tmp177->f2=1;_tmp177;}));};}
# 744
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp17A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp17B=_tmp17A;enum Cyc_Absyn_AliasQual _tmp17C;enum Cyc_Absyn_KindQual _tmp17D;struct Cyc_Core_Opt**_tmp17E;enum Cyc_Absyn_KindQual _tmp17F;struct Cyc_Core_Opt**_tmp180;struct Cyc_Core_Opt**_tmp181;struct Cyc_Core_Opt**_tmp182;struct Cyc_Core_Opt**_tmp183;struct Cyc_Core_Opt**_tmp184;struct Cyc_Core_Opt**_tmp185;switch(*((int*)_tmp17B)){case 1: _LLF2: _tmp185=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_LLF3:
 _tmp184=_tmp185;goto _LLF5;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF4: _tmp184=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_LLF5:
# 749
 _tmp183=_tmp184;goto _LLF7;case Cyc_Absyn_Aliasable: _LLF6: _tmp183=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_LLF7:
# 751
*_tmp183=({struct Cyc_Core_Opt*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp186;});goto _LLF1;default: goto _LL100;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF8: _tmp182=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_LLF9:
 _tmp181=_tmp182;goto _LLFB;case Cyc_Absyn_Aliasable: _LLFA: _tmp181=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_LLFB:
# 754
*_tmp181=({struct Cyc_Core_Opt*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp187;});goto _LLF1;default: goto _LL100;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f2)->aliasqual){case Cyc_Absyn_Top: _LLFC: _tmp180=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_LLFD:
# 756
*_tmp180=({struct Cyc_Core_Opt*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp188;});goto _LLF1;case Cyc_Absyn_Unique: goto _LL100;default: goto _LL106;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f2)->aliasqual == Cyc_Absyn_Unique){_LL100: _tmp17E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_tmp17F=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f2)->kind;_LL101:
# 760
 _tmp17D=_tmp17F;goto _LL103;}else{goto _LL106;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B)->f1)->aliasqual){case Cyc_Absyn_Top: _LLFE: _LLFF:
# 758
({struct Cyc_String_pa_PrintArg_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp189[2]={& _tmp18B,& _tmp18C};Cyc_Tcutil_terr(loc,({const char*_tmp18A="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp18A,sizeof(char),57);}),_tag_dyneither(_tmp189,sizeof(void*),2));});});});
goto _LLF1;case Cyc_Absyn_Unique: goto _LL102;default: goto _LL106;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B)->f1)->aliasqual == Cyc_Absyn_Unique){_LL102: _tmp17D=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B)->f1)->kind;_LL103:
# 762
({struct Cyc_String_pa_PrintArg_struct _tmp191;_tmp191.tag=0;_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 764
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp192=_cycalloc_atomic(sizeof(*_tmp192));_tmp192->kind=_tmp17D;_tmp192->aliasqual=Cyc_Absyn_Unique;_tmp192;})));({struct Cyc_String_pa_PrintArg_struct _tmp190;_tmp190.tag=0;_tmp190.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp18D[3]={& _tmp18F,& _tmp190,& _tmp191};Cyc_Tcutil_terr(loc,({const char*_tmp18E="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp18E,sizeof(char),57);}),_tag_dyneither(_tmp18D,sizeof(void*),3));});});});});goto _LLF1;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B)->f1)->kind == Cyc_Absyn_MemKind){_LL104: _tmp17C=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B)->f1)->aliasqual;_LL105:
# 766
({struct Cyc_String_pa_PrintArg_struct _tmp197;_tmp197.tag=0;_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 768
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp198=_cycalloc_atomic(sizeof(*_tmp198));_tmp198->kind=Cyc_Absyn_MemKind;_tmp198->aliasqual=_tmp17C;_tmp198;})));({struct Cyc_String_pa_PrintArg_struct _tmp196;_tmp196.tag=0;_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp195;_tmp195.tag=0;_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp193[3]={& _tmp195,& _tmp196,& _tmp197};Cyc_Tcutil_terr(loc,({const char*_tmp194="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp194,sizeof(char),57);}),_tag_dyneither(_tmp193,sizeof(void*),3));});});});});goto _LLF1;}else{_LL106: _LL107:
 goto _LLF1;}}}}_LLF1:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 774
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 783
{struct Cyc_List_List*_tmp199=fields;for(0;_tmp199 != 0;_tmp199=_tmp199->tl){
struct Cyc_Absyn_Datatypefield*_tmp19A=(struct Cyc_Absyn_Datatypefield*)_tmp199->hd;
# 786
{struct Cyc_List_List*typs=_tmp19A->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp19A->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);{
# 789
struct _RegionHandle _tmp19B=_new_region("temp");struct _RegionHandle*temp=& _tmp19B;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 793
Cyc_Absynpp_qvar2string(_tmp19A->name));({void*_tmp19C[1]={& _tmp19E};Cyc_Tcutil_terr(_tmp19A->loc,({const char*_tmp19D="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp19D,sizeof(char),51);}),_tag_dyneither(_tmp19C,sizeof(void*),1));});});
# 795
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp19A->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 789
;_pop_region(temp);};}}{
# 800
union Cyc_Absyn_Nmspace _tmp19F=(*_tmp19A->name).f1;union Cyc_Absyn_Nmspace _tmp1A0=_tmp19F;switch((_tmp1A0.Abs_n).tag){case 1: if((_tmp1A0.Rel_n).val == 0){_LL109: _LL10A:
# 802
 if(is_extensible)
(*_tmp19A->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 805
(*_tmp19A->name).f1=(*name).f1;}
goto _LL108;}else{_LL10B: _LL10C:
# 808
({struct Cyc_String_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp19A->name));({void*_tmp1A1[1]={& _tmp1A3};Cyc_Tcutil_terr(_tmp19A->loc,({const char*_tmp1A2="qualified datatypefield declarations are not allowed (%s)";_tag_dyneither(_tmp1A2,sizeof(char),58);}),_tag_dyneither(_tmp1A1,sizeof(void*),1));});});
goto _LL108;}case 3: _LL10D: _LL10E:
 goto _LL108;case 2: _LL10F: _LL110:
 goto _LL108;default: _LL111: _LL112:
({void*_tmp1A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A5="tcDatatypeFields: Loc_n";_tag_dyneither(_tmp1A5,sizeof(char),24);}),_tag_dyneither(_tmp1A4,sizeof(void*),0));});}_LL108:;};}}{
# 817
struct Cyc_List_List*fields2;
if(is_extensible){
# 820
int _tmp1A6=1;
struct Cyc_List_List*_tmp1A7=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1A6,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1A6)
fields2=_tmp1A7;else{
# 825
fields2=0;}}else{
# 827
struct _RegionHandle _tmp1A8=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1A8;_push_region(uprev_rgn);
# 829
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp1A9=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1A9->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=0;_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);({struct Cyc_String_pa_PrintArg_struct _tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1A9->name).f2);({void*_tmp1AA[2]={& _tmp1AC,& _tmp1AD};Cyc_Tcutil_terr(_tmp1A9->loc,({const char*_tmp1AB="duplicate field name %s in %s";_tag_dyneither(_tmp1AB,sizeof(char),30);}),_tag_dyneither(_tmp1AA,sizeof(void*),2));});});});else{
# 835
prev_fields=({struct Cyc_List_List*_tmp1AE=_region_malloc(uprev_rgn,sizeof(*_tmp1AE));_tmp1AE->hd=(*_tmp1A9->name).f2;_tmp1AE->tl=prev_fields;_tmp1AE;});}
# 837
if(_tmp1A9->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp1B1;_tmp1B1.tag=0;_tmp1B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1A9->name).f2);({void*_tmp1AF[1]={& _tmp1B1};Cyc_Tcutil_warn(loc,({const char*_tmp1B0="ignoring scope of field %s";_tag_dyneither(_tmp1B0,sizeof(char),27);}),_tag_dyneither(_tmp1AF,sizeof(void*),1));});});
_tmp1A9->sc=Cyc_Absyn_Public;}}}
# 842
fields2=fields;}
# 829
;_pop_region(uprev_rgn);}
# 848
{struct Cyc_List_List*_tmp1B2=fields;for(0;_tmp1B2 != 0;_tmp1B2=_tmp1B2->tl){
struct Cyc_Absyn_Datatypefield*_tmp1B3=(struct Cyc_Absyn_Datatypefield*)_tmp1B2->hd;
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1B3->name).f2,({struct _tuple14*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp1B6;_tmp1B6.tag=2;_tmp1B6.f1=tudres;_tmp1B6.f2=_tmp1B3;_tmp1B6;});_tmp1B5;});_tmp1B4->f2=1;_tmp1B4;}));}}
# 854
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 857
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp1D1="@extensible datatype";_tag_dyneither(_tmp1D1,sizeof(char),21);}):({const char*_tmp1D2="datatype";_tag_dyneither(_tmp1D2,sizeof(char),9);});
# 864
struct Cyc_List_List*tvs=tud->tvs;
# 867
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 869
Cyc_Tcutil_add_tvar_identities(tvs);{
# 874
struct _RegionHandle _tmp1B7=_new_region("temp");struct _RegionHandle*temp=& _tmp1B7;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 884 "tc.cyc"
{struct _handler_cons _tmp1B8;_push_handler(& _tmp1B8);{int _tmp1BA=0;if(setjmp(_tmp1B8.handler))_tmp1BA=1;if(!_tmp1BA){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp1BB=_cycalloc_atomic(sizeof(*_tmp1BB));_tmp1BB[0]=({struct Cyc_Dict_Absent_exn_struct _tmp1BC;_tmp1BC.tag=Cyc_Dict_Absent;_tmp1BC;});_tmp1BB;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 890
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 885
;_pop_handler();}else{void*_tmp1B9=(void*)_exn_thrown;void*_tmp1BD=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1BD)->tag == Cyc_Dict_Absent){_LL114: _LL115:
# 894
{union Cyc_Absyn_Nmspace _tmp1BF=(*tud->name).f1;union Cyc_Absyn_Nmspace _tmp1C0=_tmp1BF;if((_tmp1C0.Rel_n).tag == 1){if((_tmp1C0.Rel_n).val == 0){_LL119: _LL11A:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL118;}else{goto _LL11B;}}else{_LL11B: _LL11C:
 goto _LL118;}_LL118:;}{
# 898
struct Cyc_Absyn_Datatypedecl***_tmp1C1=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
tud_opt=(unsigned int)_tmp1C1?({struct Cyc_Absyn_Datatypedecl***_tmp1C2=_region_malloc(temp,sizeof(*_tmp1C2));_tmp1C2[0]=*_tmp1C1;_tmp1C2;}): 0;
goto _LL113;};}else{_LL116: _tmp1BE=_tmp1BD;_LL117:(int)_rethrow(_tmp1BE);}_LL113:;}};}{
# 905
struct _tuple17 _tmp1C3=({struct _tuple17 _tmp1D0;_tmp1D0.f1=tud->fields;_tmp1D0.f2=tud_opt;_tmp1D0;});struct _tuple17 _tmp1C4=_tmp1C3;struct Cyc_List_List**_tmp1C5;struct Cyc_Absyn_Datatypedecl**_tmp1C6;struct Cyc_List_List**_tmp1C7;struct Cyc_Absyn_Datatypedecl**_tmp1C8;if(_tmp1C4.f1 == 0){if(_tmp1C4.f2 == 0){_LL11E: _LL11F:
# 908
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,({struct Cyc_Absyn_Datatypedecl**_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9[0]=tud;_tmp1C9;}));
goto _LL11D;}else{_LL124: _tmp1C8=*_tmp1C4.f2;_LL125: {
# 941
struct Cyc_Absyn_Datatypedecl*_tmp1CF=Cyc_Tcdecl_merge_datatypedecl(*_tmp1C8,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp1CF == 0){
_npop_handler(0);return;}else{
# 946
*_tmp1C8=_tmp1CF;
goto _LL11D;}}}}else{if(_tmp1C4.f2 == 0){_LL120: _tmp1C7=(struct Cyc_List_List**)&(_tmp1C4.f1)->v;_LL121: {
# 913
struct Cyc_Absyn_Datatypedecl**_tmp1CA=({struct Cyc_Absyn_Datatypedecl**_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_Absyn_Datatypedecl*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->sc=Cyc_Absyn_Extern;_tmp1CC->name=tud->name;_tmp1CC->tvs=tvs;_tmp1CC->fields=0;_tmp1CC->is_extensible=tud->is_extensible;_tmp1CC;});_tmp1CB;});
# 915
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1CA);
# 918
*_tmp1C7=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1C7,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp1CA=tud;
goto _LL11D;}}else{_LL122: _tmp1C5=(struct Cyc_List_List**)&(_tmp1C4.f1)->v;_tmp1C6=*_tmp1C4.f2;_LL123: {
# 923
struct Cyc_Absyn_Datatypedecl*_tmp1CD=*_tmp1C6;
# 926
if((!tud->is_extensible  && (unsigned int)_tmp1CD) && _tmp1CD->is_extensible)
tud->is_extensible=1;
# 929
*_tmp1C6=({struct Cyc_Absyn_Datatypedecl*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->sc=Cyc_Absyn_Extern;_tmp1CE->name=tud->name;_tmp1CE->tvs=tvs;_tmp1CE->fields=0;_tmp1CE->is_extensible=tud->is_extensible;_tmp1CE;});
# 933
*_tmp1C5=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1C5,tvs,tud);
# 937
*_tmp1C6=_tmp1CD;
_tmp1C8=_tmp1C6;goto _LL125;}}}_LL11D:;};}
# 875 "tc.cyc"
;_pop_region(temp);};}
# 953 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 960
{union Cyc_Absyn_Nmspace _tmp1D3=(*ed->name).f1;union Cyc_Absyn_Nmspace _tmp1D4=_tmp1D3;if((_tmp1D4.Rel_n).tag == 1){if((_tmp1D4.Rel_n).val == 0){_LL127: _LL128:
# 963
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LL126;}else{goto _LL129;}}else{_LL129: _LL12A:
 goto _LL126;}_LL126:;}
# 969
if(ed->fields != 0){struct _RegionHandle _tmp1D5=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1D5;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1D6=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 975
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1D6->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp1D9;_tmp1D9.tag=0;_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D6->name).f2);({void*_tmp1D7[1]={& _tmp1D9};Cyc_Tcutil_terr(_tmp1D6->loc,({const char*_tmp1D8="duplicate enum constructor %s";_tag_dyneither(_tmp1D8,sizeof(char),30);}),_tag_dyneither(_tmp1D7,sizeof(void*),1));});});else{
# 978
prev_fields=({struct Cyc_List_List*_tmp1DA=_region_malloc(uprev_rgn,sizeof(*_tmp1DA));_tmp1DA->hd=(*_tmp1D6->name).f2;_tmp1DA->tl=prev_fields;_tmp1DA;});}
# 980
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp1D6->name).f2)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1DD;_tmp1DD.tag=0;_tmp1DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D6->name).f2);({void*_tmp1DB[1]={& _tmp1DD};Cyc_Tcutil_terr(_tmp1D6->loc,({const char*_tmp1DC="enum field name %s shadows global name";_tag_dyneither(_tmp1DC,sizeof(char),39);}),_tag_dyneither(_tmp1DB,sizeof(void*),1));});});
# 983
if(_tmp1D6->tag == 0){
_tmp1D6->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1D6->loc);
++ tag_count;}else{
# 988
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1D6->tag))){
struct _tuple11 _tmp1DE=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1D6->tag));struct _tuple11 _tmp1DF=_tmp1DE;unsigned int _tmp1E0;int _tmp1E1;_LL12C: _tmp1E0=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;_LL12D:;
if(_tmp1E1)tag_count=_tmp1E0 + 1;}}
# 992
(*_tmp1D6->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 970
;_pop_region(uprev_rgn);}
# 997
{struct _handler_cons _tmp1E2;_push_handler(& _tmp1E2);{int _tmp1E4=0;if(setjmp(_tmp1E2.handler))_tmp1E4=1;if(!_tmp1E4){
{struct Cyc_Absyn_Enumdecl**_tmp1E5=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp1E6=Cyc_Tcdecl_merge_enumdecl(*_tmp1E5,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1E6 == 0){_npop_handler(0);return;}
*_tmp1E5=_tmp1E6;}
# 998
;_pop_handler();}else{void*_tmp1E3=(void*)_exn_thrown;void*_tmp1E7=_tmp1E3;void*_tmp1E8;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1E7)->tag == Cyc_Dict_Absent){_LL12F: _LL130: {
# 1003
struct Cyc_Absyn_Enumdecl**_tmp1E9=({struct Cyc_Absyn_Enumdecl**_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=ed;_tmp1EA;});
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp1E9);
goto _LL12E;}}else{_LL131: _tmp1E8=_tmp1E7;_LL132:(int)_rethrow(_tmp1E8);}_LL12E:;}};}
# 1007
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1EB=(struct Cyc_Absyn_Enumfield*)fs->hd;
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1EB->name).f2,({struct _tuple14*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->f1=(void*)({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp1EE;_tmp1EE.tag=3;_tmp1EE.f1=ed;_tmp1EE.f2=_tmp1EB;_tmp1EE;});_tmp1ED;});_tmp1EC->f2=1;_tmp1EC;}));}}
# 1016
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1EF=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1EF->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1EF->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0;_tmp1F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1EF->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp1F0[2]={& _tmp1F2,& _tmp1F3};Cyc_Tcutil_terr(loc,({const char*_tmp1F1="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1F1,sizeof(char),46);}),_tag_dyneither(_tmp1F0,sizeof(void*),2));});});});}}}
# 1026
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1F4=sc;switch(_tmp1F4){case Cyc_Absyn_Static: _LL134: _LL135:
# 1029
 if(!in_include)
({void*_tmp1F5=0;Cyc_Tcutil_warn(loc,({const char*_tmp1F6="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1F6,sizeof(char),44);}),_tag_dyneither(_tmp1F5,sizeof(void*),0));});
return 0;case Cyc_Absyn_Abstract: _LL136: _LL137:
# 1033
({void*_tmp1F7=0;Cyc_Tcutil_warn(loc,({const char*_tmp1F8="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1F8,sizeof(char),46);}),_tag_dyneither(_tmp1F7,sizeof(void*),0));});
return 0;case Cyc_Absyn_Public: _LL138: _LL139:
 goto _LL13B;case Cyc_Absyn_Register: _LL13A: _LL13B:
 goto _LL13D;case Cyc_Absyn_Extern: _LL13C: _LL13D:
 return 1;default: _LL13E: _LL13F:
# 1039
({void*_tmp1F9=0;Cyc_Tcutil_warn(loc,({const char*_tmp1FA="nested extern \"C\" declaration";_tag_dyneither(_tmp1FA,sizeof(char),30);}),_tag_dyneither(_tmp1F9,sizeof(void*),0));});
return 1;}_LL133:;}
# 1044
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
struct _tuple12 _tmp1FB=*exp;struct _tuple12 _tmp1FC=_tmp1FB;unsigned int _tmp1FD;struct _tuple0*_tmp1FE;_LL141: _tmp1FD=_tmp1FC.f1;_tmp1FE=_tmp1FC.f2;_LL142:;{
struct _tuple0*_tmp1FF=_tmp1FE;union Cyc_Absyn_Nmspace _tmp200;struct _dyneither_ptr*_tmp201;_LL144: _tmp200=_tmp1FF->f1;_tmp201=_tmp1FF->f2;_LL145:;{
union Cyc_Absyn_Nmspace _tmp202=_tmp200;if((_tmp202.Rel_n).tag == 1){if((_tmp202.Rel_n).val == 0){_LL147: _LL148:
# 1050
(*_tmp1FE).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL146;}else{goto _LL149;}}else{_LL149: _LL14A:
 goto _LL146;}_LL146:;};};}
# 1061
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 1067
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 1070
struct Cyc_List_List*_tmp203=ds0;for(0;_tmp203 != 0;(last=_tmp203,_tmp203=_tmp203->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp203->hd;
unsigned int loc=d->loc;
# 1074
void*_tmp204=d->r;void*_tmp205=_tmp204;struct Cyc_List_List*_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_List_List*_tmp208;union Cyc_Absyn_Nmspace _tmp209;struct _dyneither_ptr*_tmp20A;struct Cyc_List_List*_tmp20B;struct _dyneither_ptr*_tmp20C;struct Cyc_List_List*_tmp20D;struct Cyc_Absyn_Enumdecl*_tmp20E;struct Cyc_Absyn_Datatypedecl*_tmp20F;struct Cyc_Absyn_Aggrdecl*_tmp210;struct Cyc_Absyn_Typedefdecl*_tmp211;struct Cyc_Absyn_Fndecl*_tmp212;struct Cyc_Absyn_Vardecl*_tmp213;switch(*((int*)_tmp205)){case 2: _LL14C: _LL14D:
 goto _LL14F;case 3: _LL14E: _LL14F:
# 1077
({void*_tmp214=0;Cyc_Tcutil_terr(loc,({const char*_tmp215="top level let-declarations are not implemented";_tag_dyneither(_tmp215,sizeof(char),47);}),_tag_dyneither(_tmp214,sizeof(void*),0));});
goto _LL14B;case 4: _LL150: _LL151:
# 1080
({void*_tmp216=0;Cyc_Tcutil_terr(loc,({const char*_tmp217="top level region declarations are not implemented";_tag_dyneither(_tmp217,sizeof(char),50);}),_tag_dyneither(_tmp216,sizeof(void*),0));});
goto _LL14B;case 0: _LL152: _tmp213=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_LL153:
# 1083
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp213->sc,te->in_extern_c_include))
_tmp213->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp213,check_var_init,te->in_extern_c_include,exports);
goto _LL14B;case 1: _LL154: _tmp212=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_LL155:
# 1088
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp212->sc,te->in_extern_c_include))
_tmp212->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp212,exports);
goto _LL14B;case 8: _LL156: _tmp211=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_LL157:
# 1093
 _tmp211->extern_c=te->in_extern_c_include;
# 1097
Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp211);
goto _LL14B;case 5: _LL158: _tmp210=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_LL159:
# 1100
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp210->sc,te->in_extern_c_include))
_tmp210->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp210);
goto _LL14B;case 6: _LL15A: _tmp20F=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_LL15B:
# 1105
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp20F->sc,te->in_extern_c_include))
_tmp20F->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp20F);
goto _LL14B;case 7: _LL15C: _tmp20E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_LL15D:
# 1110
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp20E->sc,te->in_extern_c_include))
_tmp20E->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp20E);
goto _LL14B;case 13: _LL15E: _LL15F:
# 1115
({void*_tmp218=0;Cyc_Tcutil_warn(d->loc,({const char*_tmp219="spurious __cyclone_port_on__";_tag_dyneither(_tmp219,sizeof(char),29);}),_tag_dyneither(_tmp218,sizeof(void*),0));});
goto _LL14B;case 14: _LL160: _LL161:
# 1118
 goto _LL14B;case 9: _LL162: _tmp20C=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_tmp20D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp205)->f2;_LL163: {
# 1121
struct Cyc_List_List*_tmp21A=te->ns;
struct Cyc_List_List*_tmp21B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp21A,({struct Cyc_List_List*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->hd=_tmp20C;_tmp21C->tl=0;_tmp21C;}));
# 1125
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp20C)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp20C);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp21B,Cyc_Tcenv_empty_genv(grgn));}
# 1130
te->ns=_tmp21B;
Cyc_Tc_tc_decls(te,_tmp20D,in_externC,check_var_init,grgn,exports);
te->ns=_tmp21A;
goto _LL14B;}case 10: _LL164: _tmp209=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp205)->f1)->f1;_tmp20A=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp205)->f1)->f2;_tmp20B=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp205)->f2;_LL165: {
# 1137
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp21D=_tmp209;struct _dyneither_ptr*_tmp21E;struct Cyc_List_List*_tmp21F;struct _dyneither_ptr*_tmp220;struct Cyc_List_List*_tmp221;struct _dyneither_ptr*_tmp222;struct Cyc_List_List*_tmp223;switch((_tmp21D.C_n).tag){case 4: _LL16B: _LL16C:
 goto _LL16E;case 1: if((_tmp21D.Rel_n).val == 0){_LL16D: _LL16E:
 goto _LL170;}else{_LL173: _tmp222=(struct _dyneither_ptr*)((_tmp21D.Rel_n).val)->hd;_tmp223=((_tmp21D.Rel_n).val)->tl;_LL174:
# 1145
 _tmp220=_tmp222;_tmp221=_tmp223;goto _LL176;}case 3: if((_tmp21D.C_n).val == 0){_LL16F: _LL170:
# 1142
 goto _LL172;}else{_LL175: _tmp220=(struct _dyneither_ptr*)((_tmp21D.C_n).val)->hd;_tmp221=((_tmp21D.C_n).val)->tl;_LL176:
# 1146
 _tmp21E=_tmp220;_tmp21F=_tmp221;goto _LL178;}default: if((_tmp21D.Abs_n).val == 0){_LL171: _LL172:
# 1144
 first=_tmp20A;rest=0;goto _LL16A;}else{_LL177: _tmp21E=(struct _dyneither_ptr*)((_tmp21D.Abs_n).val)->hd;_tmp21F=((_tmp21D.Abs_n).val)->tl;_LL178:
# 1148
 first=_tmp21E;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp21F,({struct Cyc_List_List*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->hd=_tmp20A;_tmp224->tl=0;_tmp224;}));goto _LL16A;}}_LL16A:;}{
# 1150
struct Cyc_List_List*_tmp225=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
ge->availables=({struct Cyc_List_List*_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226->hd=_tmp225;_tmp226->tl=ge->availables;_tmp226;});
Cyc_Tc_tc_decls(te,_tmp20B,in_externC,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL14B;};}case 11: _LL166: _tmp208=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_LL167:
# 1157
 Cyc_Tc_tc_decls(te,_tmp208,1,check_var_init,grgn,exports);
goto _LL14B;default: _LL168: _tmp206=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp205)->f1;_tmp207=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp205)->f2;_LL169:
# 1161
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp207);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp207,(unsigned int)exports?*exports: 0);
{struct _RegionHandle _tmp227=_new_region("temp");struct _RegionHandle*temp=& _tmp227;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp228=Cyc_Tcenv_enter_extern_c_include(temp,te);
Cyc_Tc_tc_decls(_tmp228,_tmp206,1,check_var_init,grgn,& newexs);
# 1168
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
struct _tuple12*_tmp229=(struct _tuple12*)_tmp207->hd;
if(!(*_tmp229).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp22C;_tmp22C.tag=0;_tmp22C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp229).f2));({void*_tmp22A[1]={& _tmp22C};Cyc_Tcutil_warn((*_tmp229).f1,({const char*_tmp22B="%s is exported but not defined";_tag_dyneither(_tmp22B,sizeof(char),31);}),_tag_dyneither(_tmp22A,sizeof(void*),1));});});}}
# 1165
;_pop_region(temp);}
# 1175
goto _LL14B;};}_LL14B:;}}
# 1180
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1182
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1199 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1201
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1203
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple18{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1207
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp22D=d->r;void*_tmp22E=_tmp22D;struct Cyc_List_List**_tmp22F;struct Cyc_List_List**_tmp230;struct Cyc_List_List**_tmp231;struct Cyc_List_List**_tmp232;struct Cyc_Absyn_Vardecl*_tmp233;switch(*((int*)_tmp22E)){case 0: _LL17A: _tmp233=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp22E)->f1;_LL17B:
# 1212
 if((env->in_cinclude  || _tmp233->sc != Cyc_Absyn_Extern  && _tmp233->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp233->name,Cyc_Absyn_uniquergn_qvar))
# 1215
return 1;{
# 1217
struct _tuple0*_tmp234=_tmp233->name;struct _tuple0*_tmp235=_tmp234;union Cyc_Absyn_Nmspace _tmp236;struct _dyneither_ptr*_tmp237;_LL187: _tmp236=_tmp235->f1;_tmp237=_tmp235->f2;_LL188:;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp238=_tmp236;struct Cyc_List_List*_tmp239;struct Cyc_List_List*_tmp23A;struct Cyc_List_List*_tmp23B;switch((_tmp238.C_n).tag){case 4: _LL18A: _LL18B:
 ns=0;goto _LL189;case 1: _LL18C: _tmp23B=(_tmp238.Rel_n).val;_LL18D:
 ns=_tmp23B;goto _LL189;case 3: _LL18E: _tmp23A=(_tmp238.C_n).val;_LL18F:
 ns=_tmp23A;goto _LL189;default: _LL190: _tmp239=(_tmp238.Abs_n).val;_LL191:
 ns=_tmp239;goto _LL189;}_LL189:;}{
# 1225
struct _tuple18*_tmp23C=((struct _tuple18*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp23D=(*_tmp23C).f1;
int _tmp23E=(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp23D->ordinaries,_tmp237)).f2;
if(!_tmp23E)
(*_tmp23C).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp23C).f2,_tmp237);
# 1233
return _tmp23E;};};};case 11: _LL17C: _tmp232=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp22E)->f1;_LL17D:
 _tmp231=_tmp232;goto _LL17F;case 10: _LL17E: _tmp231=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp22E)->f2;_LL17F:
 _tmp230=_tmp231;goto _LL181;case 9: _LL180: _tmp230=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp22E)->f2;_LL181:
# 1237
*_tmp230=Cyc_Tc_treeshake_f(env,*_tmp230);
return 1;case 12: _LL182: _tmp22F=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp22E)->f1;_LL183: {
# 1240
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp22F=Cyc_Tc_treeshake_f(env,*_tmp22F);
env->in_cinclude=in_cinclude;
return 1;}default: _LL184: _LL185:
 return 1;}_LL179:;}
# 1249
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1253
static struct _tuple18*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1255
return({struct _tuple18*_tmp23F=_region_malloc(rgn,sizeof(*_tmp23F));_tmp23F->f1=ge;_tmp23F->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp);_tmp23F;});}struct _tuple19{struct Cyc_List_List*f1;struct _tuple18*f2;};
# 1258
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp240=_new_region("rgn");struct _RegionHandle*rgn=& _tmp240;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp241=({struct Cyc_Tc_TreeshakeEnv _tmp249;_tmp249.rgn=rgn;_tmp249.in_cinclude=0;_tmp249.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple18*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae);_tmp249;});
struct Cyc_List_List*_tmp242=Cyc_Tc_treeshake_f(& _tmp241,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp241.nsdict)){
struct Cyc_List_List*_tmp243=_tmp242;_npop_handler(0);return _tmp243;}{
# 1266
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp241.nsdict);
struct _tuple19 _tmp244=*((struct _tuple19*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp241.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple19*))Cyc_Iter_next)(iter,& _tmp244)){
struct _tuple19 _tmp245=_tmp244;struct Cyc_Tcenv_Genv*_tmp246;struct Cyc_Set_Set*_tmp247;_LL193: _tmp246=(_tmp245.f2)->f1;_tmp247=(_tmp245.f2)->f2;_LL194:;
_tmp246->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple14*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp246->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple14*y))Cyc_Tc_treeshake_remove_f,_tmp247,_tmp246->ordinaries);}{
# 1272
struct Cyc_List_List*_tmp248=_tmp242;_npop_handler(0);return _tmp248;};};}
# 1260
;_pop_region(rgn);}
