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
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&
      _curr != (unsigned char *)0)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
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
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
# 69 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 75
typedef void*Cyc_Tcpat_decision_opt_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 107
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
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 211
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 213
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 222
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 368
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 373
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 511
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 537
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 544
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 562
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 932
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 935
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 946
void*Cyc_Absyn_compress_kb(void*);
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 975
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 997
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1008
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1080
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1168
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 69
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
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 38 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 49
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 65
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 69
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 80
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 97 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 107
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 111
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 114
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 185
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 64
void*Cyc_Tcutil_compress(void*t);
# 67
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 92
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 94
int Cyc_Tcutil_is_function_type(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 138
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 152
void Cyc_Tcutil_explain_failure();
# 154
int Cyc_Tcutil_unify(void*,void*);
# 181
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 191
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 218 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 220
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 228
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 232
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 242
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 274
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 284
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 304
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 315
int Cyc_Tcutil_bits_only(void*t);
# 318
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 328
int Cyc_Tcutil_supports_default(void*);
# 339
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 343
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 34
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 41
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 36 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 61 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 64
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 66
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 74
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 83
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1]="";
# 38 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 43
int Cyc_Tc_aggressive_warn=0;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 45
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 51
(*_tmp1).f3=1;
return 1;}}
# 55
return 0;}
# 58
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));({void*_tmp2[1]={& _tmp4};Cyc_Tcutil_terr(loc,({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29);}),_tag_dyneither(_tmp2,sizeof(void*),1));});});
return 0;}
# 63
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 65
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 68
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_Int_pa_PrintArg_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(unsigned long)i;({struct Cyc_String_pa_PrintArg_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);({void*_tmp5[3]={& _tmp7,& _tmp8,& _tmp9};Cyc_Tcutil_terr(loc,({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54);}),_tag_dyneither(_tmp5,sizeof(void*),3));});});});});}
# 72
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmpA=_new_region("temp");struct _RegionHandle*temp=& _tmpA;_push_region(temp);
{struct _tuple13 init_params=({struct _tuple13 _tmp3B;_tmp3B.f1=0;_tmp3B.f2=({const char*_tmp3C="initializes";_tag_dyneither(_tmp3C,sizeof(char),12);});_tmp3B;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp39;_tmp39.f1=0;_tmp39.f2=({const char*_tmp3A="noliveunique";_tag_dyneither(_tmp3A,sizeof(char),13);});_tmp39;});
struct _tuple13 noconsume_params=({struct _tuple13 _tmp37;_tmp37.f1=0;_tmp37.f2=({const char*_tmp38="noconsume";_tag_dyneither(_tmp38,sizeof(char),10);});_tmp37;});
void*_tmpB=Cyc_Tcutil_compress(t);void*_tmpC=_tmpB;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp35;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->tag == 9){_LL1: _tmp36=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).attributes;_tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).args;_LL2: {
# 79
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp35);
for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
void*_tmpD=(void*)_tmp36->hd;void*_tmpE=_tmpD;int _tmp32;int _tmp31;int _tmp30;switch(*((int*)_tmpE)){case 20: _LL6: _tmp30=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL7:
# 83
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp30,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,noconsume_params);{
struct _tuple8*_tmpF=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp30 - 1);struct _tuple8*_tmp10=_tmpF;void*_tmp25;_LLF: _tmp25=_tmp10->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp24="initializes attribute allowed only on";_tag_dyneither(_tmp24,sizeof(char),38);});
{void*_tmp11=Cyc_Tcutil_compress(_tmp25);void*_tmp12=_tmp11;void*_tmp22;union Cyc_Absyn_Constraint*_tmp21;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->tag == 5){_LL12: _tmp22=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).elt_typ;_tmp21=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).nullable;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).bounds;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).zero_term;_LL13:
# 90
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp21))
({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp13[1]={& _tmp15};Cyc_Tcutil_terr(loc,({const char*_tmp14="%s non-null pointers";_tag_dyneither(_tmp14,sizeof(char),21);}),_tag_dyneither(_tmp13,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_bound_one(_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp16[1]={& _tmp18};Cyc_Tcutil_terr(loc,({const char*_tmp17="%s pointers of size 1";_tag_dyneither(_tmp17,sizeof(char),22);}),_tag_dyneither(_tmp16,sizeof(void*),1));});});
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp19[1]={& _tmp1B};Cyc_Tcutil_terr(loc,({const char*_tmp1A="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp1A,sizeof(char),42);}),_tag_dyneither(_tmp19,sizeof(void*),1));});});
goto _LL11;}else{_LL14: _LL15:
# 98
({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1C[1]={& _tmp1E};Cyc_Tcutil_terr(loc,({const char*_tmp1D="%s pointers";_tag_dyneither(_tmp1D,sizeof(char),12);}),_tag_dyneither(_tmp1C,sizeof(void*),1));});});}_LL11:;}
# 100
init_params.f1=({struct Cyc_List_List*_tmp23=_region_malloc(temp,sizeof(*_tmp23));_tmp23->hd=(void*)_tmp30;_tmp23->tl=init_params.f1;_tmp23;});
goto _LL5;};};case 21: _LL8: _tmp31=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL9:
# 103
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp26=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp31 - 1);struct _tuple8*_tmp27=_tmp26;void*_tmp2B;_LL17: _tmp2B=_tmp27->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2B,0))
({void*_tmp28=0;Cyc_Tcutil_terr(loc,({const char*_tmp29="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp29,sizeof(char),55);}),_tag_dyneither(_tmp28,sizeof(void*),0));});
nolive_unique_params.f1=({struct Cyc_List_List*_tmp2A=_region_malloc(temp,sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp31;_tmp2A->tl=nolive_unique_params.f1;_tmp2A;});
goto _LL5;};case 22: _LLA: _tmp32=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpE)->f1;_LLB:
# 112
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp32,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp2C=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp32 - 1);struct _tuple8*_tmp2D=_tmp2C;void*_tmp2F;_LL1A: _tmp2F=_tmp2D->f3;_LL1B:;
# 117
noconsume_params.f1=({struct Cyc_List_List*_tmp2E=_region_malloc(temp,sizeof(*_tmp2E));_tmp2E->hd=(void*)_tmp32;_tmp2E->tl=noconsume_params.f1;_tmp2E;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 122
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp33=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp34="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp34,sizeof(char),34);}),_tag_dyneither(_tmp33,sizeof(void*),0));});}_LL0:;}
# 74
;_pop_region(temp);}struct _tuple14{void*f1;int f2;};
# 128
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 130
struct Cyc_Absyn_Vardecl*_tmp3D=vd;enum Cyc_Absyn_Scope _tmp9F;struct _tuple0*_tmp9E;void*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_List_List*_tmp9B;_LL1D: _tmp9F=_tmp3D->sc;_tmp9E=_tmp3D->name;_tmp9D=_tmp3D->type;_tmp9C=_tmp3D->initializer;_tmp9B=_tmp3D->attributes;_LL1E:;
# 137
{void*_tmp3E=Cyc_Tcutil_compress(_tmp9D);void*_tmp3F=_tmp3E;void*_tmp4B;struct Cyc_Absyn_Tqual _tmp4A;union Cyc_Absyn_Constraint*_tmp49;unsigned int _tmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->tag == 8){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).num_elts == 0){_LL20: _tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).tq;_tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zt_loc;if(_tmp9C != 0){_LL21:
# 139
{void*_tmp40=_tmp9C->r;void*_tmp41=_tmp40;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct _dyneither_ptr _tmp43;struct _dyneither_ptr _tmp42;switch(*((int*)_tmp41)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).tag){case 8: _LL25: _tmp42=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).String_c).val;_LL26:
# 141
 _tmp9D=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp42,sizeof(char)),0),_tmp49,_tmp48));
goto _LL24;case 9: _LL27: _tmp43=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).val;_LL28:
# 144
 _tmp9D=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(1,0),_tmp49,_tmp48));
goto _LL24;default: goto _LL31;}case 26: _LL29: _tmp44=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2A:
 _tmp45=_tmp44;goto _LL2C;case 27: _LL2B: _tmp45=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL2C:
# 149
 _tmp9D=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,_tmp45,_tmp49,_tmp48));
goto _LL24;case 35: _LL2D: _tmp46=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2E:
 _tmp47=_tmp46;goto _LL30;case 25: _LL2F: _tmp47=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL30:
# 153
 _tmp9D=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47),0),_tmp49,_tmp48));
goto _LL24;default: _LL31: _LL32:
 goto _LL24;}_LL24:;}
# 157
goto _LL1F;}else{goto _LL22;}}else{goto _LL22;}}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 161
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp9D);
# 163
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp9D);
# 166
{void*_tmp4C=Cyc_Tcutil_compress(_tmp9D);void*_tmp4D=_tmp4C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D)->tag == 8){_LL34: _LL35:
 vd->escapes=0;goto _LL33;}else{_LL36: _LL37:
 vd->escapes=1;goto _LL33;}_LL33:;}
# 171
if(Cyc_Tcutil_is_function_type(_tmp9D)){
_tmp9B=Cyc_Tcutil_transfer_fn_type_atts(_tmp9D,_tmp9B);
Cyc_Tc_fnTypeAttsOK(te,_tmp9D,loc);}
# 176
if(_tmp9F == Cyc_Absyn_Extern  || _tmp9F == Cyc_Absyn_ExternC){
if(_tmp9C != 0  && !in_cinclude)
({void*_tmp4E=0;Cyc_Tcutil_terr(loc,({const char*_tmp4F="extern declaration should not have initializer";_tag_dyneither(_tmp4F,sizeof(char),47);}),_tag_dyneither(_tmp4E,sizeof(void*),0));});}else{
if(!Cyc_Tcutil_is_function_type(_tmp9D)){
# 183
for(0;_tmp9B != 0;_tmp9B=_tmp9B->tl){
void*_tmp50=(void*)_tmp9B->hd;void*_tmp51=_tmp50;switch(*((int*)_tmp51)){case 6: _LL39: _LL3A:
 goto _LL3C;case 8: _LL3B: _LL3C:
# 188
 goto _LL3E;case 9: _LL3D: _LL3E:
 goto _LL40;case 10: _LL3F: _LL40:
 goto _LL42;case 11: _LL41: _LL42:
 goto _LL44;case 12: _LL43: _LL44:
 goto _LL46;case 13: _LL45: _LL46:
 goto _LL48;case 14: _LL47: _LL48:
 continue;default: _LL49: _LL4A:
# 196
({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name));({struct Cyc_String_pa_PrintArg_struct _tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp9B->hd));({void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_Tcutil_terr(loc,({const char*_tmp53="bad attribute %s for variable %s";_tag_dyneither(_tmp53,sizeof(char),33);}),_tag_dyneither(_tmp52,sizeof(void*),2));});});});
goto _LL38;}_LL38:;}
# 201
if(_tmp9C == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp9D))
({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9D));({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));({void*_tmp56[2]={& _tmp58,& _tmp59};Cyc_Tcutil_terr(loc,({const char*_tmp57="initializer required for variable %s of type %s";_tag_dyneither(_tmp57,sizeof(char),48);}),_tag_dyneither(_tmp56,sizeof(void*),2));});});});}else{
# 206
struct Cyc_Absyn_Exp*_tmp5A=_tmp9C;
# 210
struct _handler_cons _tmp5B;_push_handler(& _tmp5B);{int _tmp5D=0;if(setjmp(_tmp5B.handler))_tmp5D=1;if(!_tmp5D){
{void*_tmp5E=Cyc_Tcexp_tcExpInitializer(te,& _tmp9D,_tmp5A);
if(!Cyc_Tcutil_coerce_assign(te,_tmp5A,_tmp9D)){
struct _dyneither_ptr _tmp5F=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp60=" declared with type ";
struct _dyneither_ptr _tmp61=Cyc_Absynpp_typ2string(_tmp9D);
const char*_tmp62=" but initialized with type ";
struct _dyneither_ptr _tmp63=Cyc_Absynpp_typ2string(_tmp5E);
if((((Cyc_strlen((struct _dyneither_ptr)_tmp5F)+ Cyc_strlen(({const char*_tmp64=_tmp60;_tag_dyneither(_tmp64,sizeof(char),_get_zero_arr_size_char((void*)_tmp64,21));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp61))+ Cyc_strlen(({const char*_tmp65=_tmp62;_tag_dyneither(_tmp65,sizeof(char),_get_zero_arr_size_char((void*)_tmp65,28));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp63)> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp63);({struct Cyc_String_pa_PrintArg_struct _tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _dyneither_ptr)({const char*_tmp6E=_tmp62;_tag_dyneither(_tmp6E,sizeof(char),_get_zero_arr_size_char((void*)_tmp6E,28));});({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)({const char*_tmp6D=_tmp60;_tag_dyneither(_tmp6D,sizeof(char),_get_zero_arr_size_char((void*)_tmp6D,21));});({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({void*_tmp66[5]={& _tmp68,& _tmp69,& _tmp6A,& _tmp6B,& _tmp6C};Cyc_Tcutil_terr(loc,({const char*_tmp67="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp67,sizeof(char),16);}),_tag_dyneither(_tmp66,sizeof(void*),5));});});});});});});else{
# 221
({struct Cyc_String_pa_PrintArg_struct _tmp75;_tmp75.tag=0;_tmp75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp63);({struct Cyc_String_pa_PrintArg_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _dyneither_ptr)({const char*_tmp77=_tmp62;_tag_dyneither(_tmp77,sizeof(char),_get_zero_arr_size_char((void*)_tmp77,28));});({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)({const char*_tmp76=_tmp60;_tag_dyneither(_tmp76,sizeof(char),_get_zero_arr_size_char((void*)_tmp76,21));});({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({void*_tmp6F[5]={& _tmp71,& _tmp72,& _tmp73,& _tmp74,& _tmp75};Cyc_Tcutil_terr(loc,({const char*_tmp70="%s%s%s%s%s";_tag_dyneither(_tmp70,sizeof(char),11);}),_tag_dyneither(_tmp6F,sizeof(void*),5));});});});});});});}
Cyc_Tcutil_explain_failure();
# 224
if(!Cyc_Tcutil_is_const_exp(_tmp5A))
({void*_tmp78=0;Cyc_Tcutil_terr(loc,({const char*_tmp79="initializer is not a constant expression";_tag_dyneither(_tmp79,sizeof(char),41);}),_tag_dyneither(_tmp78,sizeof(void*),0));});}}
# 211
;_pop_handler();}else{void*_tmp5C=(void*)_exn_thrown;void*_tmp7A=_tmp5C;void*_tmp7D;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp7A)->tag == Cyc_Tcenv_Env_error){_LL4C: _LL4D:
# 229
({void*_tmp7B=0;Cyc_Tcutil_terr(loc,({const char*_tmp7C="initializer is not a constant expression";_tag_dyneither(_tmp7C,sizeof(char),41);}),_tag_dyneither(_tmp7B,sizeof(void*),0));});
goto _LL4B;}else{_LL4E: _tmp7D=_tmp7A;_LL4F:(int)_rethrow(_tmp7D);}_LL4B:;}};}}else{
# 235
for(0;_tmp9B != 0;_tmp9B=_tmp9B->tl){
void*_tmp7E=(void*)_tmp9B->hd;void*_tmp7F=_tmp7E;switch(*((int*)_tmp7F)){case 0: _LL51: _LL52:
# 238
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
# 249
 goto _LL68;case 7: _LL67: _LL68:
# 251
({struct Cyc_String_pa_PrintArg_struct _tmp84;_tmp84.tag=0;_tmp84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp9B->hd));({void*_tmp82[1]={& _tmp84};Cyc_Tcutil_terr(loc,({const char*_tmp83="bad attribute %s in function declaration";_tag_dyneither(_tmp83,sizeof(char),41);}),_tag_dyneither(_tmp82,sizeof(void*),1));});});
goto _LL50;default: _LL69: _LL6A:
 continue;}_LL50:;}}}
# 259
{struct _handler_cons _tmp85;_push_handler(& _tmp85);{int _tmp87=0;if(setjmp(_tmp85.handler))_tmp87=1;if(!_tmp87){
{struct _tuple14*_tmp88=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,_tmp9E);
void*_tmp89=(*_tmp88).f1;void*_tmp8A=_tmp89;void*_tmp93;switch(*((int*)_tmp8A)){case 0: _LL6C: _tmp93=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp8A)->f1;_LL6D: {
# 263
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8B=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp92;_tmp92.tag=1;_tmp92.f1=vd;_tmp92;});_tmp91;});
void**_tmp8C=Cyc_Tcdecl_merge_binding(_tmp93,(void*)_tmp8B,loc,Cyc_Tc_tc_msg);
if(_tmp8C == 0){_npop_handler(0);return;}{
void*_tmp8D=*_tmp8C;
# 268
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp8D != _tmp93  || (*_tmp88).f2)
# 271
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp9E,({struct _tuple14*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp90;_tmp90.tag=0;_tmp90.f1=_tmp8D;_tmp90;});_tmp8F;});_tmp8E->f2=1;_tmp8E;}));}
# 273
_npop_handler(0);return;};}case 1: _LL6E: _LL6F:
 goto _LL71;case 2: _LL70: _LL71:
 goto _LL73;case 4: _LL72: _LL73:
 goto _LL75;default: _LL74: _LL75:
 goto _LL6B;}_LL6B:;}
# 260
;_pop_handler();}else{void*_tmp86=(void*)_exn_thrown;void*_tmp94=_tmp86;void*_tmp95;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp94)->tag == Cyc_Dict_Absent){_LL77: _LL78:
# 279
 goto _LL76;}else{_LL79: _tmp95=_tmp94;_LL7A:(int)_rethrow(_tmp95);}_LL76:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp9E,({struct _tuple14*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp98;_tmp98.tag=0;_tmp98.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp9A;_tmp9A.tag=1;_tmp9A.f1=vd;_tmp9A;});_tmp99;});_tmp98;});_tmp97;});_tmp96->f2=0;_tmp96;}));}
# 285
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmpA0=n;union Cyc_Absyn_Nmspace _tmpA4;struct _dyneither_ptr*_tmpA3;_LL7C: _tmpA4=_tmpA0->f1;_tmpA3=_tmpA0->f2;_LL7D:;{
union Cyc_Absyn_Nmspace _tmpA1=_tmpA4;if((_tmpA1.Abs_n).tag == 2){if((_tmpA1.Abs_n).val == 0){_LL7F: _LL80:
# 289
 return Cyc_strcmp((struct _dyneither_ptr)*_tmpA3,({const char*_tmpA2="main";_tag_dyneither(_tmpA2,sizeof(char),5);}))== 0;}else{goto _LL81;}}else{_LL81: _LL82:
 return 0;}_LL7E:;};}
# 294
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 296
struct _tuple0*q=fd->name;
# 300
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmpA5=0;Cyc_Tcutil_terr(loc,({const char*_tmpA6="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmpA6,sizeof(char),54);}),_tag_dyneither(_tmpA5,sizeof(void*),0));});
# 304
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 306
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 308
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 311
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpA7=(void*)atts->hd;void*_tmpA8=_tmpA7;switch(*((int*)_tmpA8)){case 7: _LL84: _LL85:
 goto _LL87;case 6: _LL86: _LL87:
# 315
({struct Cyc_String_pa_PrintArg_struct _tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmpA9[1]={& _tmpAB};Cyc_Tcutil_terr(loc,({const char*_tmpAA="bad attribute %s for function";_tag_dyneither(_tmpAA,sizeof(char),30);}),_tag_dyneither(_tmpA9,sizeof(void*),1));});});
goto _LL83;default: _LL88: _LL89:
 goto _LL83;}_LL83:;}}
# 321
{struct _handler_cons _tmpAC;_push_handler(& _tmpAC);{int _tmpAE=0;if(setjmp(_tmpAC.handler))_tmpAE=1;if(!_tmpAE){
{struct _tuple14*_tmpAF=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
void*_tmpB0=(*_tmpAF).f1;void*_tmpB1=_tmpB0;void*_tmpC0;switch(*((int*)_tmpB1)){case 0: _LL8B: _tmpC0=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB1)->f1;_LL8C: {
# 325
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB2=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpB9;_tmpB9.tag=2;_tmpB9.f1=fd;_tmpB9;});_tmpB8;});
void**_tmpB3=Cyc_Tcdecl_merge_binding(_tmpC0,(void*)_tmpB2,loc,Cyc_Tc_tc_msg);
if(_tmpB3 == 0)goto _LL8A;{
void*_tmpB4=*_tmpB3;
# 330
if(exports == 0  || Cyc_Tc_export_member(q,*exports)){
if(!(_tmpB4 == _tmpC0  && (*_tmpAF).f2))
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,q,({struct _tuple14*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpB7;_tmpB7.tag=0;_tmpB7.f1=_tmpB4;_tmpB7;});_tmpB6;});_tmpB5->f2=1;_tmpB5;}));}
# 334
goto _LL8A;};}case 1: _LL8D: _LL8E:
# 336
({void*_tmpBA=0;Cyc_Tcutil_warn(loc,({const char*_tmpBB="function declaration shadows previous type declaration";_tag_dyneither(_tmpBB,sizeof(char),55);}),_tag_dyneither(_tmpBA,sizeof(void*),0));});
goto _LL8A;case 2: _LL8F: _LL90:
# 339
({void*_tmpBC=0;Cyc_Tcutil_warn(loc,({const char*_tmpBD="function declaration shadows previous datatype constructor";_tag_dyneither(_tmpBD,sizeof(char),59);}),_tag_dyneither(_tmpBC,sizeof(void*),0));});
goto _LL8A;case 4: _LL91: _LL92:
 goto _LL94;default: _LL93: _LL94:
# 343
({void*_tmpBE=0;Cyc_Tcutil_warn(loc,({const char*_tmpBF="function declaration shadows previous enum tag";_tag_dyneither(_tmpBF,sizeof(char),47);}),_tag_dyneither(_tmpBE,sizeof(void*),0));});
goto _LL8A;}_LL8A:;}
# 322
;_pop_handler();}else{void*_tmpAD=(void*)_exn_thrown;void*_tmpC1=_tmpAD;void*_tmpCA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC1)->tag == Cyc_Dict_Absent){_LL96: _LL97:
# 347
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC2=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpC9;_tmpC9.tag=2;_tmpC9.f1=fd;_tmpC9;});_tmpC8;});
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,q,({struct _tuple14*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpC5;_tmpC5.tag=0;_tmpC5.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpC7;_tmpC7.tag=2;_tmpC7.f1=fd;_tmpC7;});_tmpC6;});_tmpC5;});_tmpC4;});_tmpC3->f2=0;_tmpC3;}));}
# 352
goto _LL95;}else{_LL98: _tmpCA=_tmpC1;_LL99:(int)_rethrow(_tmpCA);}_LL95:;}};}
# 356
if(te->in_extern_c_include)return;
# 361
{struct _RegionHandle _tmpCB=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpCB;_push_region(fnrgn);
{struct Cyc_Tcenv_Fenv*_tmpCC=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmpCD=({struct Cyc_Tcenv_Tenv*_tmpD3=_region_malloc(fnrgn,sizeof(*_tmpD3));_tmpD3->ns=te->ns;_tmpD3->ae=te->ae;_tmpD3->le=_tmpCC;_tmpD3->allow_valueof=0;_tmpD3->in_extern_c_include=0;_tmpD3;});
# 365
struct _handler_cons _tmpCE;_push_handler(& _tmpCE);{int _tmpD0=0;if(setjmp(_tmpCE.handler))_tmpD0=1;if(!_tmpD0){
Cyc_Tcstmt_tcStmt(_tmpCD,fd->body,0);
# 369
Cyc_Tcenv_check_delayed_effects(_tmpCD);
Cyc_Tcenv_check_delayed_constraints(_tmpCD);
# 366
;_pop_handler();}else{void*_tmpCF=(void*)_exn_thrown;void*_tmpD1=_tmpCF;void*_tmpD2;if(((struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*)_tmpD1)->tag == Cyc_Tcutil_AbortTypeCheckingFunction){_LL9B: _LL9C:
# 372
 goto _LL9A;}else{_LL9D: _tmpD2=_tmpD1;_LL9E:
 Cyc_Core_rethrow(_tmpD2);}_LL9A:;}};}
# 362
;_pop_region(fnrgn);}
# 377
if(Cyc_Tc_is_main(q)){
# 379
{void*_tmpD4=Cyc_Tcutil_compress(fd->ret_type);void*_tmpD5=_tmpD4;enum Cyc_Absyn_Size_of _tmpDB;switch(*((int*)_tmpD5)){case 0: _LLA2: _LLA3:
({void*_tmpD6=0;Cyc_Tcutil_warn(loc,({const char*_tmpD7="main declared with return type void";_tag_dyneither(_tmpD7,sizeof(char),36);}),_tag_dyneither(_tmpD6,sizeof(void*),0));});goto _LLA1;case 6: _LLA4: _tmpDB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD5)->f2;_LLA5:
 goto _LLA1;default: _LLA6: _LLA7:
# 383
({struct Cyc_String_pa_PrintArg_struct _tmpDA;_tmpDA.tag=0;_tmpDA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type));({void*_tmpD8[1]={& _tmpDA};Cyc_Tcutil_terr(loc,({const char*_tmpD9="main declared with return type %s instead of int or void";_tag_dyneither(_tmpD9,sizeof(char),57);}),_tag_dyneither(_tmpD8,sizeof(void*),1));});});
goto _LLA1;}_LLA1:;}
# 387
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpDC=0;Cyc_Tcutil_terr(loc,({const char*_tmpDD="main declared with varargs";_tag_dyneither(_tmpDD,sizeof(char),27);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});{
struct Cyc_List_List*_tmpDE=fd->args;
if(_tmpDE != 0){
struct _tuple8*_tmpDF=(struct _tuple8*)_tmpDE->hd;struct _tuple8*_tmpE0=_tmpDF;void*_tmpF8;_LLA9: _tmpF8=_tmpE0->f3;_LLAA:;
{void*_tmpE1=Cyc_Tcutil_compress(_tmpF8);void*_tmpE2=_tmpE1;enum Cyc_Absyn_Size_of _tmpE6;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->tag == 6){_LLAC: _tmpE6=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->f2;_LLAD:
 goto _LLAB;}else{_LLAE: _LLAF:
# 395
({struct Cyc_String_pa_PrintArg_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpF8));({void*_tmpE3[1]={& _tmpE5};Cyc_Tcutil_terr(loc,({const char*_tmpE4="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpE4,sizeof(char),60);}),_tag_dyneither(_tmpE3,sizeof(void*),1));});});
goto _LLAB;}_LLAB:;}
# 399
_tmpDE=_tmpDE->tl;
if(_tmpDE != 0){
struct _tuple8*_tmpE7=(struct _tuple8*)_tmpDE->hd;struct _tuple8*_tmpE8=_tmpE7;void*_tmpF7;_LLB1: _tmpF7=_tmpE8->f3;_LLB2:;
_tmpDE=_tmpDE->tl;
if(_tmpDE != 0)
({void*_tmpE9=0;Cyc_Tcutil_terr(loc,({const char*_tmpEA="main declared with too many arguments";_tag_dyneither(_tmpEA,sizeof(char),38);}),_tag_dyneither(_tmpE9,sizeof(void*),0));});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->v=fd->tvs;_tmpF6;});
if(((!Cyc_Tcutil_unify(_tmpF7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->v=& Cyc_Tcutil_rk;_tmpEB;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->v=& Cyc_Tcutil_rk;_tmpEC;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpF7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->v=& Cyc_Tcutil_rk;_tmpED;}),tvs)),
# 411
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->v=& Cyc_Tcutil_rk;_tmpEE;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpF7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->v=& Cyc_Tcutil_rk;_tmpEF;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->v=& Cyc_Tcutil_rk;_tmpF0;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpF7,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->v=& Cyc_Tcutil_rk;_tmpF1;}),tvs)),
# 418
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->v=& Cyc_Tcutil_rk;_tmpF2;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
({struct Cyc_String_pa_PrintArg_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpF7));({void*_tmpF3[1]={& _tmpF5};Cyc_Tcutil_terr(loc,({const char*_tmpF4="second argument of main has type %s instead of char??";_tag_dyneither(_tmpF4,sizeof(char),54);}),_tag_dyneither(_tmpF3,sizeof(void*),1));});});};}}};}};}
# 428
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 435
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmpF9[1]={& _tmpFB};Cyc_Tcutil_terr(loc,({const char*_tmpFA="redeclaration of typedef %s";_tag_dyneither(_tmpFA,sizeof(char),28);}),_tag_dyneither(_tmpF9,sizeof(void*),1));});});
return;}
# 440
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 449
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpFC=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpFD=_tmpFC;struct Cyc_Core_Opt**_tmp105;struct Cyc_Absyn_Kind*_tmp104;struct Cyc_Core_Opt**_tmp103;switch(*((int*)_tmpFD)){case 1: _LLB4: _tmp103=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_LLB5:
# 452
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp100;_tmp100.tag=0;_tmp100.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);({void*_tmpFE[1]={& _tmp100};Cyc_Tcutil_warn(loc,({const char*_tmpFF="type variable %s is not used in typedef definition";_tag_dyneither(_tmpFF,sizeof(char),51);}),_tag_dyneither(_tmpFE,sizeof(void*),1));});});
# 455
*_tmp103=({struct Cyc_Core_Opt*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmp101;});goto _LLB3;case 2: _LLB6: _tmp105=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_tmp104=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f2;_LLB7:
# 462
*_tmp105=({struct Cyc_Core_Opt*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->v=Cyc_Tcutil_kind_to_bound(_tmp104);_tmp102;});
goto _LLB3;default: _LLB8: _LLB9:
 continue;}_LLB3:;}}
# 468
(te->ae)->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);}
# 471
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 478
struct _RegionHandle _tmp106=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp106;_push_region(uprev_rgn);
# 480
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmp107=(struct _tuple10*)rpo->hd;struct _tuple10*_tmp108=_tmp107;void*_tmp10A;void*_tmp109;_LLBB: _tmp10A=_tmp108->f1;_tmp109=_tmp108->f2;_LLBC:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmp10A);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmp109);}{
# 486
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 489
struct Cyc_List_List*_tmp10B=fields;for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){
struct Cyc_Absyn_Aggrfield*_tmp10C=(struct Cyc_Absyn_Aggrfield*)_tmp10B->hd;struct Cyc_Absyn_Aggrfield*_tmp10D=_tmp10C;struct _dyneither_ptr*_tmp12A;struct Cyc_Absyn_Tqual _tmp129;void*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_List_List*_tmp126;struct Cyc_Absyn_Exp*_tmp125;_LLBE: _tmp12A=_tmp10D->name;_tmp129=_tmp10D->tq;_tmp128=_tmp10D->type;_tmp127=_tmp10D->width;_tmp126=_tmp10D->attributes;_tmp125=_tmp10D->requires_clause;_LLBF:;
# 492
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp12A))
({struct Cyc_String_pa_PrintArg_struct _tmp110;_tmp110.tag=0;_tmp110.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp12A);({void*_tmp10E[1]={& _tmp110};Cyc_Tcutil_terr(loc,({const char*_tmp10F="duplicate member %s";_tag_dyneither(_tmp10F,sizeof(char),20);}),_tag_dyneither(_tmp10E,sizeof(void*),1));});});
# 496
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp12A,({const char*_tmp111="";_tag_dyneither(_tmp111,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp112=_region_malloc(uprev_rgn,sizeof(*_tmp112));_tmp112->hd=_tmp12A;_tmp112->tl=prev_fields;_tmp112;});{
# 499
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 503
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp10B->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp128);
# 508
(((struct Cyc_Absyn_Aggrfield*)_tmp10B->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp10B->hd)->tq).print_const,_tmp128);
# 511
Cyc_Tcutil_check_bitfield(loc,te,_tmp128,_tmp127,_tmp12A);
# 513
if((unsigned int)_tmp125){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmp113=0;Cyc_Tcutil_terr(loc,({const char*_tmp114="@requires clauses are only allowed on union members";_tag_dyneither(_tmp114,sizeof(char),52);}),_tag_dyneither(_tmp113,sizeof(void*),0));});
{struct _RegionHandle _tmp115=_new_region("temp");struct _RegionHandle*temp=& _tmp115;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp116=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp116,0,_tmp125);}
# 517
;_pop_region(temp);}
# 520
if(!Cyc_Tcutil_is_integral(_tmp125))
({struct Cyc_String_pa_PrintArg_struct _tmp119;_tmp119.tag=0;_tmp119.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 523
Cyc_Absynpp_typ2string((void*)_check_null(_tmp125->topt)));({void*_tmp117[1]={& _tmp119};Cyc_Tcutil_terr(_tmp125->loc,({const char*_tmp118="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp118,sizeof(char),54);}),_tag_dyneither(_tmp117,sizeof(void*),1));});});else{
# 525
Cyc_Tcutil_check_type(_tmp125->loc,te,tvs,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp11B;_tmp11B.tag=18;_tmp11B.f1=_tmp125;_tmp11B;});_tmp11A;}));{
# 527
struct Cyc_List_List*_tmp11C=Cyc_Relations_exp2relns(uprev_rgn,_tmp125);
# 534
if(!Cyc_Relations_consistent_relations(_tmp11C))
({void*_tmp11D=0;Cyc_Tcutil_terr(_tmp125->loc,({const char*_tmp11E="@requires clause may be unsatisfiable";_tag_dyneither(_tmp11E,sizeof(char),38);}),_tag_dyneither(_tmp11D,sizeof(void*),0));});
# 540
{struct Cyc_List_List*_tmp11F=prev_relations;for(0;_tmp11F != 0;_tmp11F=_tmp11F->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp11C,(struct Cyc_List_List*)_tmp11F->hd)))
({void*_tmp120=0;Cyc_Tcutil_terr(_tmp125->loc,({const char*_tmp121="@requires clause may overlap with previous clauses";_tag_dyneither(_tmp121,sizeof(char),51);}),_tag_dyneither(_tmp120,sizeof(void*),0));});}}
# 545
prev_relations=({struct Cyc_List_List*_tmp122=_region_malloc(uprev_rgn,sizeof(*_tmp122));_tmp122->hd=_tmp11C;_tmp122->tl=prev_relations;_tmp122;});};}}else{
# 548
if(prev_relations != 0)
({void*_tmp123=0;Cyc_Tcutil_terr(loc,({const char*_tmp124="if one field has a @requires clause, they all must";_tag_dyneither(_tmp124,sizeof(char),51);}),_tag_dyneither(_tmp123,sizeof(void*),0));});}};}};
# 480
;_pop_region(uprev_rgn);}
# 554
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 556
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp12B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp12C=_tmp12B;enum Cyc_Absyn_AliasQual _tmp141;struct Cyc_Core_Opt**_tmp140;struct Cyc_Absyn_Kind*_tmp13F;struct Cyc_Core_Opt**_tmp13E;struct Cyc_Core_Opt**_tmp13D;enum Cyc_Absyn_AliasQual _tmp13C;struct Cyc_Core_Opt**_tmp13B;switch(*((int*)_tmp12C)){case 1: _LLC1: _tmp13B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp12C)->f1;_LLC2:
# 559
*_tmp13B=({struct Cyc_Core_Opt*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp12D;});continue;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C)->f2)->kind){case Cyc_Absyn_MemKind: _LLC3: _tmp13D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C)->f1;_tmp13C=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C)->f2)->aliasqual;_LLC4:
# 561
 if(constrain_top_kind  && _tmp13C == Cyc_Absyn_Top)
*_tmp13D=({struct Cyc_Core_Opt*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp12F=_cycalloc_atomic(sizeof(*_tmp12F));_tmp12F->kind=Cyc_Absyn_BoxKind;_tmp12F->aliasqual=Cyc_Absyn_Aliasable;_tmp12F;}));_tmp12E;});else{
# 564
*_tmp13D=({struct Cyc_Core_Opt*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp131=_cycalloc_atomic(sizeof(*_tmp131));_tmp131->kind=Cyc_Absyn_BoxKind;_tmp131->aliasqual=_tmp13C;_tmp131;}));_tmp130;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C)->f2)->aliasqual == Cyc_Absyn_Top){_LLC5: _tmp13E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C)->f1;if(constrain_top_kind){_LLC6:
# 567
*_tmp13E=({struct Cyc_Core_Opt*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp133=_cycalloc_atomic(sizeof(*_tmp133));_tmp133->kind=Cyc_Absyn_BoxKind;_tmp133->aliasqual=Cyc_Absyn_Aliasable;_tmp133;}));_tmp132;});
continue;}else{goto _LLC7;}}else{goto _LLC7;}default: _LLC7: _tmp140=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C)->f1;_tmp13F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C)->f2;_LLC8:
# 570
*_tmp140=({struct Cyc_Core_Opt*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->v=Cyc_Tcutil_kind_to_bound(_tmp13F);_tmp134;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp12C)->f1)->kind == Cyc_Absyn_MemKind){_LLC9: _tmp141=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp12C)->f1)->aliasqual;_LLCA:
# 572
({struct Cyc_String_pa_PrintArg_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp13A=_cycalloc_atomic(sizeof(*_tmp13A));_tmp13A->kind=Cyc_Absyn_MemKind;_tmp13A->aliasqual=_tmp141;_tmp13A;})));({struct Cyc_String_pa_PrintArg_struct _tmp138;_tmp138.tag=0;_tmp138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*n).f2);({void*_tmp135[3]={& _tmp137,& _tmp138,& _tmp139};Cyc_Tcutil_terr(loc,({const char*_tmp136="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp136,sizeof(char),57);}),_tag_dyneither(_tmp135,sizeof(void*),3));});});});});
continue;}else{_LLCB: _LLCC:
 continue;}}_LLC0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 580
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp142=ad->name;
# 587
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp143=(void*)atts->hd;void*_tmp144=_tmp143;switch(*((int*)_tmp144)){case 7: _LLCE: _LLCF:
 goto _LLD1;case 6: _LLD0: _LLD1:
 continue;default: _LLD2: _LLD3:
# 592
({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp142).f2);({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0;_tmp147.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp145[2]={& _tmp147,& _tmp148};Cyc_Tcutil_terr(loc,({const char*_tmp146="bad attribute %s in %s definition";_tag_dyneither(_tmp146,sizeof(char),34);}),_tag_dyneither(_tmp145,sizeof(void*),2));});});});
goto _LLCD;}_LLCD:;}}{
# 597
struct Cyc_List_List*_tmp149=ad->tvs;
# 600
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 602
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 606
{struct _tuple15 _tmp14A=({struct _tuple15 _tmp168;_tmp168.f1=ad->impl;_tmp168.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,_tmp142);_tmp168;});struct _tuple15 _tmp14B=_tmp14A;struct Cyc_List_List*_tmp167;struct Cyc_List_List*_tmp166;struct Cyc_List_List*_tmp165;int _tmp164;struct Cyc_Absyn_Aggrdecl**_tmp163;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp160;int _tmp15F;struct Cyc_Absyn_Aggrdecl**_tmp15E;if(_tmp14B.f1 == 0){if(_tmp14B.f2 == 0){_LLD5: _LLD6:
# 609
 Cyc_Tc_rule_out_memkind(loc,_tmp142,_tmp149,0);
# 611
(te->ae)->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp142,({struct Cyc_Absyn_Aggrdecl**_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=ad;_tmp14C;}));
goto _LLD4;}else{_LLDB: _tmp15E=*_tmp14B.f2;_LLDC: {
# 673
struct Cyc_Absyn_Aggrdecl*_tmp15D=Cyc_Tcdecl_merge_aggrdecl(*_tmp15E,ad,loc,Cyc_Tc_tc_msg);
if(_tmp15D == 0)
return;else{
# 677
Cyc_Tc_rule_out_memkind(loc,_tmp142,_tmp149,0);
# 680
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp142,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 683
*_tmp15E=_tmp15D;
ad=_tmp15D;
goto _LLD4;}}}}else{if(_tmp14B.f2 == 0){_LLD7: _tmp162=(_tmp14B.f1)->exist_vars;_tmp161=(_tmp14B.f1)->rgn_po;_tmp160=(_tmp14B.f1)->fields;_tmp15F=(_tmp14B.f1)->tagged;_LLD8: {
# 616
struct Cyc_Absyn_Aggrdecl**_tmp14D=({struct Cyc_Absyn_Aggrdecl**_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Absyn_Aggrdecl*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->kind=ad->kind;_tmp156->sc=Cyc_Absyn_Extern;_tmp156->name=ad->name;_tmp156->tvs=_tmp149;_tmp156->impl=0;_tmp156->attributes=ad->attributes;_tmp156->expected_mem_kind=0;_tmp156;});_tmp155;});
# 618
(te->ae)->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp142,_tmp14D);
# 623
Cyc_Tcutil_check_unique_tvars(loc,_tmp162);
# 625
Cyc_Tcutil_add_tvar_identities(_tmp162);
# 628
if(_tmp15F  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp14E=0;Cyc_Tcutil_terr(loc,({const char*_tmp14F="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp14F,sizeof(char),56);}),_tag_dyneither(_tmp14E,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp149,_tmp162),_tmp161,_tmp160);
# 632
Cyc_Tc_rule_out_memkind(loc,_tmp142,_tmp149,0);
# 635
Cyc_Tc_rule_out_memkind(loc,_tmp142,_tmp162,1);
# 637
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp15F){
# 640
struct Cyc_List_List*f=_tmp160;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp142).f2);({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0;_tmp152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);({void*_tmp150[3]={& _tmp152,& _tmp153,& _tmp154};Cyc_Tcutil_warn(loc,({const char*_tmp151="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp151,sizeof(char),98);}),_tag_dyneither(_tmp150,sizeof(void*),3));});});});});}}
# 646
*_tmp14D=ad;
goto _LLD4;}}else{_LLD9: _tmp167=(_tmp14B.f1)->exist_vars;_tmp166=(_tmp14B.f1)->rgn_po;_tmp165=(_tmp14B.f1)->fields;_tmp164=(_tmp14B.f1)->tagged;_tmp163=*_tmp14B.f2;_LLDA:
# 650
 if(ad->kind != (*_tmp163)->kind)
({void*_tmp157=0;Cyc_Tcutil_terr(loc,({const char*_tmp158="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp158,sizeof(char),52);}),_tag_dyneither(_tmp157,sizeof(void*),0));});{
# 653
struct Cyc_Absyn_Aggrdecl*_tmp159=*_tmp163;
# 655
*_tmp163=({struct Cyc_Absyn_Aggrdecl*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A->kind=ad->kind;_tmp15A->sc=Cyc_Absyn_Extern;_tmp15A->name=ad->name;_tmp15A->tvs=_tmp149;_tmp15A->impl=0;_tmp15A->attributes=ad->attributes;_tmp15A->expected_mem_kind=0;_tmp15A;});
# 661
Cyc_Tcutil_check_unique_tvars(loc,_tmp167);
# 663
Cyc_Tcutil_add_tvar_identities(_tmp167);
# 665
if(_tmp164  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp15B=0;Cyc_Tcutil_terr(loc,({const char*_tmp15C="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp15C,sizeof(char),56);}),_tag_dyneither(_tmp15B,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp149,_tmp167),_tmp166,_tmp165);
# 669
*_tmp163=_tmp159;
_tmp15E=_tmp163;goto _LLDC;};}}_LLD4:;}
# 689
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp142,({struct _tuple14*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->f1=(void*)({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp16B;_tmp16B.tag=1;_tmp16B.f1=ad;_tmp16B;});_tmp16A;});_tmp169->f2=1;_tmp169;}));};}
# 693
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp16C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp16D=_tmp16C;enum Cyc_Absyn_AliasQual _tmp18A;enum Cyc_Absyn_KindQual _tmp189;struct Cyc_Core_Opt**_tmp188;enum Cyc_Absyn_KindQual _tmp187;struct Cyc_Core_Opt**_tmp186;struct Cyc_Core_Opt**_tmp185;struct Cyc_Core_Opt**_tmp184;struct Cyc_Core_Opt**_tmp183;struct Cyc_Core_Opt**_tmp182;struct Cyc_Core_Opt**_tmp181;switch(*((int*)_tmp16D)){case 1: _LLDE: _tmp181=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16D)->f1;_LLDF:
 _tmp182=_tmp181;goto _LLE1;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f2)->aliasqual){case Cyc_Absyn_Top: _LLE0: _tmp182=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f1;_LLE1:
# 698
 _tmp183=_tmp182;goto _LLE3;case Cyc_Absyn_Aliasable: _LLE2: _tmp183=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f1;_LLE3:
# 700
*_tmp183=({struct Cyc_Core_Opt*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp16E;});goto _LLDD;default: goto _LLEC;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f2)->aliasqual){case Cyc_Absyn_Top: _LLE4: _tmp184=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f1;_LLE5:
 _tmp185=_tmp184;goto _LLE7;case Cyc_Absyn_Aliasable: _LLE6: _tmp185=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f1;_LLE7:
# 703
*_tmp185=({struct Cyc_Core_Opt*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp16F;});goto _LLDD;default: goto _LLEC;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f2)->aliasqual){case Cyc_Absyn_Top: _LLE8: _tmp186=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f1;_LLE9:
# 705
*_tmp186=({struct Cyc_Core_Opt*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp170;});goto _LLDD;case Cyc_Absyn_Unique: goto _LLEC;default: goto _LLF2;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f2)->aliasqual == Cyc_Absyn_Unique){_LLEC: _tmp188=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f1;_tmp187=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16D)->f2)->kind;_LLED:
# 709
 _tmp189=_tmp187;goto _LLEF;}else{goto _LLF2;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D)->f1)->aliasqual){case Cyc_Absyn_Top: _LLEA: _LLEB:
# 707
({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0;_tmp173.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp171[2]={& _tmp173,& _tmp174};Cyc_Tcutil_terr(loc,({const char*_tmp172="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp172,sizeof(char),57);}),_tag_dyneither(_tmp171,sizeof(void*),2));});});});
goto _LLDD;case Cyc_Absyn_Unique: goto _LLEE;default: goto _LLF2;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D)->f1)->aliasqual == Cyc_Absyn_Unique){_LLEE: _tmp189=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D)->f1)->kind;_LLEF:
# 711
({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 713
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp17A=_cycalloc_atomic(sizeof(*_tmp17A));_tmp17A->kind=_tmp189;_tmp17A->aliasqual=Cyc_Absyn_Unique;_tmp17A;})));({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp175[3]={& _tmp177,& _tmp178,& _tmp179};Cyc_Tcutil_terr(loc,({const char*_tmp176="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp176,sizeof(char),57);}),_tag_dyneither(_tmp175,sizeof(void*),3));});});});});goto _LLDD;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D)->f1)->kind == Cyc_Absyn_MemKind){_LLF0: _tmp18A=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16D)->f1)->aliasqual;_LLF1:
# 715
({struct Cyc_String_pa_PrintArg_struct _tmp17F;_tmp17F.tag=0;_tmp17F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 717
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp180=_cycalloc_atomic(sizeof(*_tmp180));_tmp180->kind=Cyc_Absyn_MemKind;_tmp180->aliasqual=_tmp18A;_tmp180;})));({struct Cyc_String_pa_PrintArg_struct _tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp17D;_tmp17D.tag=0;_tmp17D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp17B[3]={& _tmp17D,& _tmp17E,& _tmp17F};Cyc_Tcutil_terr(loc,({const char*_tmp17C="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp17C,sizeof(char),57);}),_tag_dyneither(_tmp17B,sizeof(void*),3));});});});});goto _LLDD;}else{_LLF2: _LLF3:
 goto _LLDD;}}}}_LLDD:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 723
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 731
{struct Cyc_List_List*_tmp18B=fields;for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){
struct Cyc_Absyn_Datatypefield*_tmp18C=(struct Cyc_Absyn_Datatypefield*)_tmp18B->hd;
struct Cyc_List_List*typs=_tmp18C->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp18C->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);{
# 736
struct _RegionHandle _tmp18D=_new_region("temp");struct _RegionHandle*temp=& _tmp18D;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp190;_tmp190.tag=0;_tmp190.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 740
Cyc_Absynpp_qvar2string(_tmp18C->name));({void*_tmp18E[1]={& _tmp190};Cyc_Tcutil_terr(_tmp18C->loc,({const char*_tmp18F="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp18F,sizeof(char),51);}),_tag_dyneither(_tmp18E,sizeof(void*),1));});});
# 742
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp18C->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 736
;_pop_region(temp);};}}}{
# 748
struct Cyc_List_List*fields2;
if(is_extensible){
# 751
int _tmp191=1;
struct Cyc_List_List*_tmp192=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp191,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp191)
fields2=_tmp192;else{
# 756
fields2=0;}}else{
# 758
struct _RegionHandle _tmp193=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp193;_push_region(uprev_rgn);
# 760
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp194=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp194->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);({struct Cyc_String_pa_PrintArg_struct _tmp197;_tmp197.tag=0;_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp194->name).f2);({void*_tmp195[2]={& _tmp197,& _tmp198};Cyc_Tcutil_terr(_tmp194->loc,({const char*_tmp196="duplicate field name %s in %s";_tag_dyneither(_tmp196,sizeof(char),30);}),_tag_dyneither(_tmp195,sizeof(void*),2));});});});else{
# 766
prev_fields=({struct Cyc_List_List*_tmp199=_region_malloc(uprev_rgn,sizeof(*_tmp199));_tmp199->hd=(*_tmp194->name).f2;_tmp199->tl=prev_fields;_tmp199;});}
# 768
if(_tmp194->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp194->name).f2);({void*_tmp19A[1]={& _tmp19C};Cyc_Tcutil_warn(loc,({const char*_tmp19B="ignoring scope of field %s";_tag_dyneither(_tmp19B,sizeof(char),27);}),_tag_dyneither(_tmp19A,sizeof(void*),1));});});
_tmp194->sc=Cyc_Absyn_Public;}}}
# 773
fields2=fields;}
# 760
;_pop_region(uprev_rgn);}
# 779
{struct Cyc_List_List*_tmp19D=fields;for(0;_tmp19D != 0;_tmp19D=_tmp19D->tl){
struct Cyc_Absyn_Datatypefield*_tmp19E=(struct Cyc_Absyn_Datatypefield*)_tmp19D->hd;
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp19E->name,({struct _tuple14*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp1A1;_tmp1A1.tag=2;_tmp1A1.f1=tudres;_tmp1A1.f2=_tmp19E;_tmp1A1;});_tmp1A0;});_tmp19F->f2=1;_tmp19F;}));}}
# 785
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 788
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp1BA="@extensible datatype";_tag_dyneither(_tmp1BA,sizeof(char),21);}):({const char*_tmp1BB="datatype";_tag_dyneither(_tmp1BB,sizeof(char),9);});
# 795
struct Cyc_List_List*tvs=tud->tvs;
# 798
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 800
Cyc_Tcutil_add_tvar_identities(tvs);{
# 805
struct _RegionHandle _tmp1A2=_new_region("temp");struct _RegionHandle*temp=& _tmp1A2;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 816 "tc.cyc"
{struct _handler_cons _tmp1A3;_push_handler(& _tmp1A3);{int _tmp1A5=0;if(setjmp(_tmp1A3.handler))_tmp1A5=1;if(!_tmp1A5){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp1A6=_cycalloc_atomic(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Dict_Absent_exn_struct _tmp1A7;_tmp1A7.tag=Cyc_Dict_Absent;_tmp1A7;});_tmp1A6;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 822
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 817
;_pop_handler();}else{void*_tmp1A4=(void*)_exn_thrown;void*_tmp1A8=_tmp1A4;void*_tmp1AB;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1A8)->tag == Cyc_Dict_Absent){_LLF5: _LLF6: {
# 826
struct Cyc_Absyn_Datatypedecl***_tmp1A9=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);
tud_opt=(unsigned int)_tmp1A9?({struct Cyc_Absyn_Datatypedecl***_tmp1AA=_region_malloc(temp,sizeof(*_tmp1AA));_tmp1AA[0]=*_tmp1A9;_tmp1AA;}): 0;
goto _LLF4;}}else{_LLF7: _tmp1AB=_tmp1A8;_LLF8:(int)_rethrow(_tmp1AB);}_LLF4:;}};}{
# 833
struct _tuple17 _tmp1AC=({struct _tuple17 _tmp1B9;_tmp1B9.f1=tud->fields;_tmp1B9.f2=tud_opt;_tmp1B9;});struct _tuple17 _tmp1AD=_tmp1AC;struct Cyc_List_List**_tmp1B8;struct Cyc_Absyn_Datatypedecl**_tmp1B7;struct Cyc_List_List**_tmp1B6;struct Cyc_Absyn_Datatypedecl**_tmp1B5;if(_tmp1AD.f1 == 0){if(_tmp1AD.f2 == 0){_LLFA: _LLFB:
# 836
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
(te->ae)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,({struct Cyc_Absyn_Datatypedecl**_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE[0]=tud;_tmp1AE;}));
goto _LLF9;}else{_LL100: _tmp1B5=*_tmp1AD.f2;_LL101: {
# 869
struct Cyc_Absyn_Datatypedecl*_tmp1B4=Cyc_Tcdecl_merge_datatypedecl(*_tmp1B5,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(_tmp1B4 == 0){
_npop_handler(0);return;}else{
# 874
*_tmp1B5=_tmp1B4;
goto _LLF9;}}}}else{if(_tmp1AD.f2 == 0){_LLFC: _tmp1B6=(struct Cyc_List_List**)&(_tmp1AD.f1)->v;_LLFD: {
# 841
struct Cyc_Absyn_Datatypedecl**_tmp1AF=({struct Cyc_Absyn_Datatypedecl**_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0[0]=({struct Cyc_Absyn_Datatypedecl*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->sc=Cyc_Absyn_Extern;_tmp1B1->name=tud->name;_tmp1B1->tvs=tvs;_tmp1B1->fields=0;_tmp1B1->is_extensible=tud->is_extensible;_tmp1B1;});_tmp1B0;});
# 843
(te->ae)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,_tmp1AF);
# 846
*_tmp1B6=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp1B6,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*_tmp1AF=tud;
goto _LLF9;}}else{_LLFE: _tmp1B8=(struct Cyc_List_List**)&(_tmp1AD.f1)->v;_tmp1B7=*_tmp1AD.f2;_LLFF: {
# 851
struct Cyc_Absyn_Datatypedecl*_tmp1B2=*_tmp1B7;
# 854
if((!tud->is_extensible  && (unsigned int)_tmp1B2) && _tmp1B2->is_extensible)
tud->is_extensible=1;
# 857
*_tmp1B7=({struct Cyc_Absyn_Datatypedecl*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->sc=Cyc_Absyn_Extern;_tmp1B3->name=tud->name;_tmp1B3->tvs=tvs;_tmp1B3->fields=0;_tmp1B3->is_extensible=tud->is_extensible;_tmp1B3;});
# 861
*_tmp1B8=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp1B8,tvs,tud);
# 865
*_tmp1B7=_tmp1B2;
_tmp1B5=_tmp1B7;goto _LL101;}}}_LLF9:;};}
# 806 "tc.cyc"
;_pop_region(temp);};}
# 881 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 888
if(ed->fields != 0){struct _RegionHandle _tmp1BC=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1BC;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1BD=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 894
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1BD->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp1C0;_tmp1C0.tag=0;_tmp1C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1BD->name).f2);({void*_tmp1BE[1]={& _tmp1C0};Cyc_Tcutil_terr(_tmp1BD->loc,({const char*_tmp1BF="duplicate enum constructor %s";_tag_dyneither(_tmp1BF,sizeof(char),30);}),_tag_dyneither(_tmp1BE,sizeof(void*),1));});});else{
# 897
prev_fields=({struct Cyc_List_List*_tmp1C1=_region_malloc(uprev_rgn,sizeof(*_tmp1C1));_tmp1C1->hd=(*_tmp1BD->name).f2;_tmp1C1->tl=prev_fields;_tmp1C1;});}
# 899
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,_tmp1BD->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=0;_tmp1C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1BD->name).f2);({void*_tmp1C2[1]={& _tmp1C4};Cyc_Tcutil_terr(_tmp1BD->loc,({const char*_tmp1C3="enum field name %s shadows global name";_tag_dyneither(_tmp1C3,sizeof(char),39);}),_tag_dyneither(_tmp1C2,sizeof(void*),1));});});
# 902
if(_tmp1BD->tag == 0){
_tmp1BD->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1BD->loc);
++ tag_count;}else{
# 907
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1BD->tag))){
struct _tuple11 _tmp1C5=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1BD->tag));struct _tuple11 _tmp1C6=_tmp1C5;unsigned int _tmp1C8;int _tmp1C7;_LL103: _tmp1C8=_tmp1C6.f1;_tmp1C7=_tmp1C6.f2;_LL104:;
if(_tmp1C7)tag_count=_tmp1C8 + 1;}}}}
# 889
;_pop_region(uprev_rgn);}
# 915
{struct _handler_cons _tmp1C9;_push_handler(& _tmp1C9);{int _tmp1CB=0;if(setjmp(_tmp1C9.handler))_tmp1CB=1;if(!_tmp1CB){
{struct Cyc_Absyn_Enumdecl**_tmp1CC=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);
struct Cyc_Absyn_Enumdecl*_tmp1CD=Cyc_Tcdecl_merge_enumdecl(*_tmp1CC,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1CD == 0){_npop_handler(0);return;}
*_tmp1CC=_tmp1CD;}
# 916
;_pop_handler();}else{void*_tmp1CA=(void*)_exn_thrown;void*_tmp1CE=_tmp1CA;void*_tmp1D1;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1CE)->tag == Cyc_Dict_Absent){_LL106: _LL107: {
# 921
struct Cyc_Absyn_Enumdecl**_tmp1CF=({struct Cyc_Absyn_Enumdecl**_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0[0]=ed;_tmp1D0;});
(te->ae)->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,_tmp1CF);
goto _LL105;}}else{_LL108: _tmp1D1=_tmp1CE;_LL109:(int)_rethrow(_tmp1D1);}_LL105:;}};}
# 925
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1D2=(struct Cyc_Absyn_Enumfield*)fs->hd;
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp1D2->name,({struct _tuple14*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=(void*)({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4[0]=({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp1D5;_tmp1D5.tag=3;_tmp1D5.f1=ed;_tmp1D5.f2=_tmp1D2;_tmp1D5;});_tmp1D4;});_tmp1D3->f2=1;_tmp1D3;}));}}
# 934
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1D6=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1D6->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1D6->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D6->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1D9;_tmp1D9.tag=0;_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp1D7[2]={& _tmp1D9,& _tmp1DA};Cyc_Tcutil_terr(loc,({const char*_tmp1D8="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1D8,sizeof(char),46);}),_tag_dyneither(_tmp1D7,sizeof(void*),2));});});});}}}
# 944
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1DB=sc;switch(_tmp1DB){case Cyc_Absyn_Static: _LL10B: _LL10C:
# 947
 if(!in_include)
({void*_tmp1DC=0;Cyc_Tcutil_warn(loc,({const char*_tmp1DD="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1DD,sizeof(char),44);}),_tag_dyneither(_tmp1DC,sizeof(void*),0));});
return 0;case Cyc_Absyn_Abstract: _LL10D: _LL10E:
# 951
({void*_tmp1DE=0;Cyc_Tcutil_warn(loc,({const char*_tmp1DF="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1DF,sizeof(char),46);}),_tag_dyneither(_tmp1DE,sizeof(void*),0));});
return 0;case Cyc_Absyn_Public: _LL10F: _LL110:
 goto _LL112;case Cyc_Absyn_Register: _LL111: _LL112:
 goto _LL114;case Cyc_Absyn_Extern: _LL113: _LL114:
 return 1;default: _LL115: _LL116:
# 957
({void*_tmp1E0=0;Cyc_Tcutil_warn(loc,({const char*_tmp1E1="nested extern \"C\" declaration";_tag_dyneither(_tmp1E1,sizeof(char),30);}),_tag_dyneither(_tmp1E0,sizeof(void*),0));});
return 1;}_LL10A:;}
# 967
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 973
struct Cyc_List_List*last=0;
# 975
struct Cyc_List_List*_tmp1E2=ds0;for(0;_tmp1E2 != 0;(last=_tmp1E2,_tmp1E2=_tmp1E2->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1E2->hd;
unsigned int loc=d->loc;
# 979
void*_tmp1E3=d->r;void*_tmp1E4=_tmp1E3;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp1FE;union Cyc_Absyn_Nmspace _tmp1FD;struct _dyneither_ptr*_tmp1FC;struct Cyc_List_List*_tmp1FB;struct _dyneither_ptr*_tmp1FA;struct Cyc_List_List*_tmp1F9;struct Cyc_Absyn_Enumdecl*_tmp1F8;struct Cyc_Absyn_Datatypedecl*_tmp1F7;struct Cyc_Absyn_Aggrdecl*_tmp1F6;struct Cyc_Absyn_Typedefdecl*_tmp1F5;struct Cyc_Absyn_Fndecl*_tmp1F4;struct Cyc_Absyn_Vardecl*_tmp1F3;switch(*((int*)_tmp1E4)){case 2: _LL118: _LL119:
 goto _LL11B;case 3: _LL11A: _LL11B:
# 982
({void*_tmp1E5=0;Cyc_Tcutil_terr(loc,({const char*_tmp1E6="top level let-declarations are not implemented";_tag_dyneither(_tmp1E6,sizeof(char),47);}),_tag_dyneither(_tmp1E5,sizeof(void*),0));});
goto _LL117;case 4: _LL11C: _LL11D:
# 985
({void*_tmp1E7=0;Cyc_Tcutil_terr(loc,({const char*_tmp1E8="top level region declarations are not implemented";_tag_dyneither(_tmp1E8,sizeof(char),50);}),_tag_dyneither(_tmp1E7,sizeof(void*),0));});
goto _LL117;case 0: _LL11E: _tmp1F3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_LL11F:
# 988
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F3->sc,te->in_extern_c_include))
_tmp1F3->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,_tmp1F3,check_var_init,te->in_extern_c_include,exports);
goto _LL117;case 1: _LL120: _tmp1F4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_LL121:
# 993
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F4->sc,te->in_extern_c_include))
_tmp1F4->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,_tmp1F4,exports);
goto _LL117;case 8: _LL122: _tmp1F5=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_LL123:
# 998
 _tmp1F5->extern_c=te->in_extern_c_include;
# 1002
Cyc_Tc_tcTypedefdecl(te,loc,_tmp1F5);
goto _LL117;case 5: _LL124: _tmp1F6=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_LL125:
# 1005
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F6->sc,te->in_extern_c_include))
_tmp1F6->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp1F6);
goto _LL117;case 6: _LL126: _tmp1F7=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_LL127:
# 1010
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F7->sc,te->in_extern_c_include))
_tmp1F7->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,_tmp1F7);
goto _LL117;case 7: _LL128: _tmp1F8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_LL129:
# 1015
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F8->sc,te->in_extern_c_include))
_tmp1F8->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp1F8);
goto _LL117;case 13: _LL12A: _LL12B:
# 1020
({void*_tmp1E9=0;Cyc_Tcutil_warn(d->loc,({const char*_tmp1EA="spurious __cyclone_port_on__";_tag_dyneither(_tmp1EA,sizeof(char),29);}),_tag_dyneither(_tmp1E9,sizeof(void*),0));});
goto _LL117;case 14: _LL12C: _LL12D:
# 1023
 goto _LL117;case 9: _LL12E: _tmp1FA=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_tmp1F9=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1E4)->f2;_LL12F: {
# 1026
struct Cyc_List_List*_tmp1EB=te->ns;
te->ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1EB,({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=_tmp1FA;_tmp1EC->tl=0;_tmp1EC;}));
Cyc_Tc_tc_decls(te,_tmp1F9,in_externC,check_var_init,grgn,exports);
te->ns=_tmp1EB;
goto _LL117;}case 10: _LL130: _tmp1FD=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1)->f1;_tmp1FC=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1)->f2;_tmp1FB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1E4)->f2;_LL131:
# 1034
 Cyc_Tc_tc_decls(te,_tmp1FB,in_externC,check_var_init,grgn,exports);
goto _LL117;case 11: _LL132: _tmp1FE=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_LL133:
# 1038
 Cyc_Tc_tc_decls(te,_tmp1FE,1,check_var_init,grgn,exports);
goto _LL117;default: _LL134: _tmp200=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1E4)->f1;_tmp1FF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1E4)->f2;_LL135: {
# 1044
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1FF,(unsigned int)exports?*exports: 0);
{struct _RegionHandle _tmp1ED=_new_region("temp");struct _RegionHandle*temp=& _tmp1ED;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp1EE=Cyc_Tcenv_enter_extern_c_include(temp,te);
Cyc_Tc_tc_decls(_tmp1EE,_tmp200,1,check_var_init,grgn,& newexs);
# 1050
for(0;_tmp1FF != 0;_tmp1FF=_tmp1FF->tl){
struct _tuple12*_tmp1EF=(struct _tuple12*)_tmp1FF->hd;
if(!(*_tmp1EF).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1EF).f2));({void*_tmp1F0[1]={& _tmp1F2};Cyc_Tcutil_warn((*_tmp1EF).f1,({const char*_tmp1F1="%s is exported but not defined";_tag_dyneither(_tmp1F1,sizeof(char),31);}),_tag_dyneither(_tmp1F0,sizeof(void*),1));});});}}
# 1047
;_pop_region(temp);}
# 1057
goto _LL117;}}_LL117:;}}
# 1062
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1064
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1081 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1083
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1085
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1089
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp201=d->r;void*_tmp202=_tmp201;struct Cyc_List_List**_tmp207;struct Cyc_List_List**_tmp206;struct Cyc_List_List**_tmp205;struct Cyc_List_List**_tmp204;struct Cyc_Absyn_Vardecl*_tmp203;switch(*((int*)_tmp202)){case 0: _LL137: _tmp203=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp202)->f1;_LL138:
# 1094
 if((env->in_cinclude  || _tmp203->sc != Cyc_Absyn_Extern  && _tmp203->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp203->name,Cyc_Absyn_uniquergn_qvar))
# 1097
return 1;
# 1099
return(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,_tmp203->name)).f2;case 11: _LL139: _tmp204=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp202)->f1;_LL13A:
 _tmp205=_tmp204;goto _LL13C;case 10: _LL13B: _tmp205=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp202)->f2;_LL13C:
 _tmp206=_tmp205;goto _LL13E;case 9: _LL13D: _tmp206=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp202)->f2;_LL13E:
# 1103
*_tmp206=Cyc_Tc_treeshake_f(env,*_tmp206);
return 1;case 12: _LL13F: _tmp207=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp202)->f1;_LL140: {
# 1106
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp207=Cyc_Tc_treeshake_f(env,*_tmp207);
env->in_cinclude=in_cinclude;
return 1;}default: _LL141: _LL142:
 return 1;}_LL136:;}
# 1115
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp208=({struct Cyc_Tc_TreeshakeEnv _tmp209;_tmp209.in_cinclude=0;_tmp209.ordinaries=(te->ae)->ordinaries;_tmp209;});
return Cyc_Tc_treeshake_f(& _tmp208,ds);}
