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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 775 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 941
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 951
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 970
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1191
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{enum Cyc_Absyn_Scope f1;int f2;};
# 61 "tcdecl.h"
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int allow_externC_extern_merge);
# 66
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 73
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 82
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 146 "tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 164
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 254 "tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 321
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 76
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13]="Incompatible";
# 43 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 49
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _dyneither_ptr fmt2;
if(Cyc_strlen((struct _dyneither_ptr)*msg1)== 0)
fmt2=(struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1="%s";_tag_dyneither(_tmp1,sizeof(char),3);}),(struct _dyneither_ptr)fmt);else{
# 55
fmt2=(struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp2="%s ";_tag_dyneither(_tmp2,sizeof(char),4);}),(struct _dyneither_ptr)fmt);}{
struct _dyneither_ptr ap2=({unsigned int _tmp3=_get_dyneither_size(ap,sizeof(void*))+ 1;void**_tmp4=(void**)_cycalloc(_check_times(sizeof(void*),_tmp3));struct _dyneither_ptr _tmp8=_tag_dyneither(_tmp4,sizeof(void*),_tmp3);{unsigned int _tmp5=_tmp3;unsigned int i;for(i=0;i < _tmp5;i ++){_tmp4[i]=(void*)(
i == 0?(void*)({struct Cyc_String_pa_PrintArg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*msg1);_tmp7;});_tmp6;}):*((void**)_check_dyneither_subscript(ap,sizeof(void*),(int)(i - 1))));}}_tmp8;});
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)
Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));};};}
# 64
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 66
struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?({const char*_tmp10="public ";_tag_dyneither(_tmp10,sizeof(char),8);}): Cyc_Absynpp_scope2string(s0);
struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?({const char*_tmpF="public ";_tag_dyneither(_tmpF,sizeof(char),8);}): Cyc_Absynpp_scope2string(s1);
({struct Cyc_String_pa_PrintArg_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0_str);({struct Cyc_String_pa_PrintArg_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1_str);({struct Cyc_String_pa_PrintArg_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp9[4]={& _tmpB,& _tmpC,& _tmpD,& _tmpE};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA="%s %s is %sbut expected scope is %s";_tag_dyneither(_tmpA,sizeof(char),36);}),_tag_dyneither(_tmp9,sizeof(void*),4));});});});});});}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 72
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 74
{struct _tuple11 _tmp11=({struct _tuple11 _tmp15;_tmp15.f1=s0;_tmp15.f2=s1;_tmp15;});struct _tuple11 _tmp12=_tmp11;switch(_tmp12.f1){case Cyc_Absyn_ExternC: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LL1: _LL2:
# 76
 goto _LL0;case Cyc_Absyn_Public: _LL3: _LL4:
# 78
 goto _LL6;case Cyc_Absyn_Extern: _LL5: _LL6:
 goto _LL8;default: _LLB: _LLC:
# 83
 goto _LLE;}case Cyc_Absyn_Public: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LL7: _LL8:
# 80
 goto _LLA;case Cyc_Absyn_Extern: goto _LLF;case Cyc_Absyn_Public: _LL15: _LL16:
# 90
 goto _LL18;default: goto _LL1B;}case Cyc_Absyn_Extern: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LL9: _LLA:
# 81
 if(externCmerge)goto _LL0;
goto _LLC;case Cyc_Absyn_Extern: goto _LLF;default: _LL11: _LL12:
# 87
 goto _LL0;}default: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LLD: _LLE:
# 84
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp13;_tmp13.f1=s1;_tmp13.f2=0;_tmp13;});case Cyc_Absyn_Extern: _LLF: _LL10:
# 86
 s1=s0;goto _LL0;default: switch(_tmp12.f1){case Cyc_Absyn_Static: if(_tmp12.f2 == Cyc_Absyn_Static){_LL13: _LL14:
# 89
 goto _LL16;}else{goto _LL1B;}case Cyc_Absyn_Abstract: if(_tmp12.f2 == Cyc_Absyn_Abstract){_LL17: _LL18:
# 91
 goto _LL0;}else{goto _LL1B;}default: if(_tmp12.f2 == Cyc_Absyn_Register){_LL19: _LL1A:
 goto _LL0;}else{_LL1B: _LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp14;_tmp14.f1=s1;_tmp14.f2=0;_tmp14;});}}}}_LL0:;}
# 95
return({struct _tuple10 _tmp16;_tmp16.f1=s1;_tmp16.f2=1;_tmp16;});}
# 98
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 100
return Cyc_Tcutil_unify(t0,t1);}
# 104
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmp17=e->r;void*_tmp18=_tmp17;int _tmp19;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18)->f1).Int_c).tag == 5){_LL1E: _tmp19=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18)->f1).Int_c).val).f2;_LL1F:
 return(unsigned int)_tmp19;}else{goto _LL20;}}else{_LL20: _LL21:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp1B;_tmp1B.tag=Cyc_Core_Invalid_argument;_tmp1B.f1=({const char*_tmp1C="Tcdecl::get_uint_const_value";_tag_dyneither(_tmp1C,sizeof(char),29);});_tmp1B;});_tmp1A;}));}_LL1D:;}
# 111
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 114
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){
({struct Cyc_String_pa_PrintArg_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp1D[2]={& _tmp1F,& _tmp20};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp1E="%s %s has a different number of type parameters";_tag_dyneither(_tmp1E,sizeof(char),48);}),_tag_dyneither(_tmp1D,sizeof(void*),2));});});});
return 0;}{
# 118
struct Cyc_List_List*_tmp21=tvs0;
struct Cyc_List_List*_tmp22=tvs1;
for(0;_tmp21 != 0;(_tmp21=_tmp21->tl,_tmp22=_tmp22->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp21->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp22))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp23=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp21->hd)->kind);
struct Cyc_Absyn_Kind*_tmp24=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp22->hd)->kind);
if(_tmp23 == _tmp24)continue;
({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp24));({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp21->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp29;_tmp29.tag=0;_tmp29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp23));({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp25[5]={& _tmp27,& _tmp28,& _tmp29,& _tmp2A,& _tmp2B};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp26="%s %s has a different kind (%s) for type parameter %s (%s)";_tag_dyneither(_tmp26,sizeof(char),59);}),_tag_dyneither(_tmp25,sizeof(void*),5));});});});});});});
return 0;};}
# 129
return 1;};}
# 132
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 134
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp2C[2]={& _tmp2E,& _tmp2F};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp2D="%s %s has different attributes";_tag_dyneither(_tmp2D,sizeof(char),31);}),_tag_dyneither(_tmp2C,sizeof(void*),2));});});});
return 0;}
# 138
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 141
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 143
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
inst=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=({struct _tuple12*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd;_tmp31->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp33;_tmp33.tag=2;_tmp33.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp33;});_tmp32;});_tmp31;});_tmp30->tl=inst;_tmp30;});}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 149
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 151
struct _dyneither_ptr _tmp34=Cyc_Absynpp_qvar2string(d0->name);
int _tmp35=1;
# 157
if(!(d0->kind == d1->kind))return 0;
# 160
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_tmp36="type";_tag_dyneither(_tmp36,sizeof(char),5);}),_tmp34,loc,msg))return 0;{
# 163
struct _tuple10 _tmp37=
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,({const char*_tmp90="type";_tag_dyneither(_tmp90,sizeof(char),5);}),_tmp34,loc,msg,1);
# 163
struct _tuple10 _tmp38=_tmp37;enum Cyc_Absyn_Scope _tmp39;int _tmp3A;_LL23: _tmp39=_tmp38.f1;_tmp3A=_tmp38.f2;_LL24:;
# 165
if(!_tmp3A)_tmp35=0;
# 168
if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,({const char*_tmp3B="type";_tag_dyneither(_tmp3B,sizeof(char),5);}),_tmp34,loc,msg))_tmp35=0;{
# 170
struct Cyc_Absyn_Aggrdecl*d2;
# 172
{struct _tuple13 _tmp3C=({struct _tuple13 _tmp8E;_tmp8E.f1=d0->impl;_tmp8E.f2=d1->impl;_tmp8E;});struct _tuple13 _tmp3D=_tmp3C;struct Cyc_List_List*_tmp3E;struct Cyc_List_List*_tmp3F;struct Cyc_List_List*_tmp40;int _tmp41;struct Cyc_List_List*_tmp42;struct Cyc_List_List*_tmp43;struct Cyc_List_List*_tmp44;int _tmp45;if(_tmp3D.f2 == 0){_LL26: _LL27:
 d2=d0;goto _LL25;}else{if(_tmp3D.f1 == 0){_LL28: _LL29:
 d2=d1;goto _LL25;}else{_LL2A: _tmp3E=(_tmp3D.f1)->exist_vars;_tmp3F=(_tmp3D.f1)->rgn_po;_tmp40=(_tmp3D.f1)->fields;_tmp41=(_tmp3D.f1)->tagged;_tmp42=(_tmp3D.f2)->exist_vars;_tmp43=(_tmp3D.f2)->rgn_po;_tmp44=(_tmp3D.f2)->fields;_tmp45=(_tmp3D.f2)->tagged;_LL2B:
# 179
 if(!Cyc_Tcdecl_check_tvs(_tmp3E,_tmp42,({const char*_tmp46="type";_tag_dyneither(_tmp46,sizeof(char),5);}),_tmp34,loc,msg))
return 0;{
# 183
struct Cyc_List_List*_tmp47=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp3E),
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp42));
# 188
for(0;_tmp3F != 0  && _tmp43 != 0;
(_tmp3F=_tmp3F->tl,_tmp43=_tmp43->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3F->hd)).f1,(*((struct _tuple14*)_tmp43->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3F->hd)).f2,(*((struct _tuple14*)_tmp43->hd)).f2);}
# 195
for(0;_tmp40 != 0  && _tmp44 != 0;(_tmp40=_tmp40->tl,_tmp44=_tmp44->tl)){
struct Cyc_Absyn_Aggrfield*_tmp48=(struct Cyc_Absyn_Aggrfield*)_tmp40->hd;struct Cyc_Absyn_Aggrfield*_tmp49=_tmp48;struct _dyneither_ptr*_tmp4A;struct Cyc_Absyn_Tqual _tmp4B;void*_tmp4C;struct Cyc_Absyn_Exp*_tmp4D;struct Cyc_List_List*_tmp4E;struct Cyc_Absyn_Exp*_tmp4F;_LL2D: _tmp4A=_tmp49->name;_tmp4B=_tmp49->tq;_tmp4C=_tmp49->type;_tmp4D=_tmp49->width;_tmp4E=_tmp49->attributes;_tmp4F=_tmp49->requires_clause;_LL2E:;{
struct Cyc_Absyn_Aggrfield*_tmp50=(struct Cyc_Absyn_Aggrfield*)_tmp44->hd;struct Cyc_Absyn_Aggrfield*_tmp51=_tmp50;struct _dyneither_ptr*_tmp52;struct Cyc_Absyn_Tqual _tmp53;void*_tmp54;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_List_List*_tmp56;struct Cyc_Absyn_Exp*_tmp57;_LL30: _tmp52=_tmp51->name;_tmp53=_tmp51->tq;_tmp54=_tmp51->type;_tmp55=_tmp51->width;_tmp56=_tmp51->attributes;_tmp57=_tmp51->requires_clause;_LL31:;
# 199
if(Cyc_strptrcmp(_tmp4A,_tmp52)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0;_tmp5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp52);({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A);({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0;_tmp5A.f1=(struct _dyneither_ptr)({const char*_tmp5E="type";_tag_dyneither(_tmp5E,sizeof(char),5);});({void*_tmp58[4]={& _tmp5A,& _tmp5B,& _tmp5C,& _tmp5D};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp59="%s %s : field name mismatch %s != %s";_tag_dyneither(_tmp59,sizeof(char),37);}),_tag_dyneither(_tmp58,sizeof(void*),4));});});});});});
# 202
return 0;}
# 204
if(!Cyc_Tcutil_same_atts(_tmp4E,_tmp56)){
({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0;_tmp63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A);({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(struct _dyneither_ptr)({const char*_tmp64="type";_tag_dyneither(_tmp64,sizeof(char),5);});({void*_tmp5F[3]={& _tmp61,& _tmp62,& _tmp63};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp60="%s %s : attribute mismatch on field %s";_tag_dyneither(_tmp60,sizeof(char),39);}),_tag_dyneither(_tmp5F,sizeof(void*),3));});});});});
_tmp35=0;}
# 208
if(!Cyc_Tcutil_equal_tqual(_tmp4B,_tmp53)){
({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A);({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _dyneither_ptr)({const char*_tmp6A="type";_tag_dyneither(_tmp6A,sizeof(char),5);});({void*_tmp65[3]={& _tmp67,& _tmp68,& _tmp69};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp66="%s %s : qualifier mismatch on field %s";_tag_dyneither(_tmp66,sizeof(char),39);}),_tag_dyneither(_tmp65,sizeof(void*),3));});});});});
_tmp35=0;}
# 212
if(((_tmp4D != 0  && _tmp55 != 0) && 
Cyc_Tcdecl_get_uint_const_value(_tmp4D)!= 
Cyc_Tcdecl_get_uint_const_value(_tmp55) || 
_tmp4D == 0  && _tmp55 != 0) || 
_tmp4D != 0  && _tmp55 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A);({struct Cyc_String_pa_PrintArg_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp6D;_tmp6D.tag=0;_tmp6D.f1=(struct _dyneither_ptr)({const char*_tmp70="type";_tag_dyneither(_tmp70,sizeof(char),5);});({void*_tmp6B[3]={& _tmp6D,& _tmp6E,& _tmp6F};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp6C="%s %s : bitfield mismatch on field %s";_tag_dyneither(_tmp6C,sizeof(char),38);}),_tag_dyneither(_tmp6B,sizeof(void*),3));});});});});
_tmp35=0;}
# 220
if(_tmp4F != 0  && _tmp57 != 0){
void*subst_w1=Cyc_Tcutil_substitute(_tmp47,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp79;_tmp79.tag=18;_tmp79.f1=_tmp57;_tmp79;});_tmp78;}));
if(!Cyc_Tcdecl_check_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp72;_tmp72.tag=18;_tmp72.f1=_tmp4F;_tmp72;});_tmp71;}),subst_w1)){
({struct Cyc_String_pa_PrintArg_struct _tmp77;_tmp77.tag=0;_tmp77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp57));({struct Cyc_String_pa_PrintArg_struct _tmp76;_tmp76.tag=0;_tmp76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp4F));({struct Cyc_String_pa_PrintArg_struct _tmp75;_tmp75.tag=0;_tmp75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp73[3]={& _tmp75,& _tmp76,& _tmp77};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp74="type %s : mismatch on @requires clauses: %s != %s";_tag_dyneither(_tmp74,sizeof(char),50);}),_tag_dyneither(_tmp73,sizeof(void*),3));});});});});
_tmp35=0;}}else{
# 227
if(_tmp4F == 0  && _tmp57 != 0  || 
_tmp4F != 0  && _tmp57 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp7A[1]={& _tmp7C};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp7B="type %s : mismatch on @requires clauses";_tag_dyneither(_tmp7B,sizeof(char),40);}),_tag_dyneither(_tmp7A,sizeof(void*),1));});});
_tmp35=0;}}{
# 233
void*subst_t1=Cyc_Tcutil_substitute(_tmp47,_tmp54);
if(!Cyc_Tcdecl_check_type(_tmp4C,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1));({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0;_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4C));({struct Cyc_String_pa_PrintArg_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A);({struct Cyc_String_pa_PrintArg_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp7D[4]={& _tmp7F,& _tmp80,& _tmp81,& _tmp82};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp7E="type %s : type mismatch on field %s: %s != %s";_tag_dyneither(_tmp7E,sizeof(char),46);}),_tag_dyneither(_tmp7D,sizeof(void*),4));});});});});});
Cyc_Tcutil_explain_failure();
_tmp35=0;}};};}
# 242
if(_tmp40 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp40->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp83[2]={& _tmp85,& _tmp86};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp84="type %s is missing field %s";_tag_dyneither(_tmp84,sizeof(char),28);}),_tag_dyneither(_tmp83,sizeof(void*),2));});});});
_tmp35=0;}
# 246
if(_tmp44 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp44->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp87[2]={& _tmp89,& _tmp8A};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp88="type %s has extra field %s";_tag_dyneither(_tmp88,sizeof(char),27);}),_tag_dyneither(_tmp87,sizeof(void*),2));});});});
_tmp35=0;}
# 251
if(_tmp41 != _tmp45){
({struct Cyc_String_pa_PrintArg_struct _tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp8B[1]={& _tmp8D};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp8C="%s : previous declaration disagrees with @tagged qualifier";_tag_dyneither(_tmp8C,sizeof(char),59);}),_tag_dyneither(_tmp8B,sizeof(void*),1));});});
_tmp35=0;}
# 256
d2=d0;
goto _LL25;};}}_LL25:;}
# 260
if(!_tmp35)return 0;
# 262
if(_tmp39 == d2->sc)
return d2;else{
# 265
d2=({struct Cyc_Absyn_Aggrdecl*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=*d2;_tmp8F;});
d2->sc=_tmp39;
return d2;}};};}
# 271
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
return is_x?({const char*_tmp91="@extensible datatype";_tag_dyneither(_tmp91,sizeof(char),21);}):({const char*_tmp92="datatype";_tag_dyneither(_tmp92,sizeof(char),9);});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 277
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 281
unsigned int loc=f1->loc;
# 284
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp98;_tmp98.tag=0;_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp97;_tmp97.tag=0;_tmp97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp96;_tmp96.tag=0;_tmp96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp95;_tmp95.tag=0;_tmp95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp93[4]={& _tmp95,& _tmp96,& _tmp97,& _tmp98};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp94="%s %s: field name mismatch %s != %s";_tag_dyneither(_tmp94,sizeof(char),36);}),_tag_dyneither(_tmp93,sizeof(void*),4));});});});});});
# 287
return 0;}{
# 290
struct _dyneither_ptr _tmp99=*(*f0->name).f2;
# 293
struct _tuple10 _tmp9A=
Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpB3[2]={& _tmpB5,& _tmpB6};Cyc_aprintf(({const char*_tmpB4="in %s %s, field";_tag_dyneither(_tmpB4,sizeof(char),16);}),_tag_dyneither(_tmpB3,sizeof(void*),2));});});}),_tmp99,loc,msg,0);
# 293
struct _tuple10 _tmp9B=_tmp9A;enum Cyc_Absyn_Scope _tmp9C;int _tmp9D;_LL33: _tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;_LL34:;{
# 297
struct Cyc_List_List*_tmp9E=f0->typs;
struct Cyc_List_List*_tmp9F=f1->typs;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F)){
({struct Cyc_String_pa_PrintArg_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({struct Cyc_String_pa_PrintArg_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpA0[3]={& _tmpA2,& _tmpA3,& _tmpA4};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA1="%s %s, field %s: parameter number mismatch";_tag_dyneither(_tmpA1,sizeof(char),43);}),_tag_dyneither(_tmpA0,sizeof(void*),3));});});});});
_tmp9D=0;}
# 304
for(0;_tmp9E != 0  && _tmp9F != 0;(_tmp9E=_tmp9E->tl,_tmp9F=_tmp9F->tl)){
# 306
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp9E->hd)).f1,(*((struct _tuple15*)_tmp9F->hd)).f1)){
({struct Cyc_String_pa_PrintArg_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({struct Cyc_String_pa_PrintArg_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpA7;_tmpA7.tag=0;_tmpA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpA5[3]={& _tmpA7,& _tmpA8,& _tmpA9};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA6="%s %s, field %s: parameter qualifier";_tag_dyneither(_tmpA6,sizeof(char),37);}),_tag_dyneither(_tmpA5,sizeof(void*),3));});});});});
_tmp9D=0;}{
# 310
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmp9F->hd)).f2);
# 312
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp9E->hd)).f2,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmpB0;_tmpB0.tag=0;_tmpB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1));({struct Cyc_String_pa_PrintArg_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp9E->hd)).f2));({struct Cyc_String_pa_PrintArg_struct _tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({struct Cyc_String_pa_PrintArg_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpAC;_tmpAC.tag=0;_tmpAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpAA[5]={& _tmpAC,& _tmpAD,& _tmpAE,& _tmpAF,& _tmpB0};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpAB="%s %s, field %s: parameter type mismatch %s != %s";_tag_dyneither(_tmpAB,sizeof(char),50);}),_tag_dyneither(_tmpAA,sizeof(void*),5));});});});});});});
Cyc_Tcutil_explain_failure();
_tmp9D=0;}};}
# 320
if(!_tmp9D)return 0;
if(f0->sc != _tmp9C){
struct Cyc_Absyn_Datatypefield*_tmpB1=({struct Cyc_Absyn_Datatypefield*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=*f0;_tmpB2;});
_tmpB1->sc=_tmp9C;
return _tmpB1;}else{
return f0;}};};}
# 328
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 331
struct _tuple15*_tmpB7=x;struct Cyc_Absyn_Tqual _tmpB8;void*_tmpB9;_LL36: _tmpB8=_tmpB7->f1;_tmpB9=_tmpB7->f2;_LL37:;
return({struct _tuple15*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=_tmpB8;_tmpBA->f2=Cyc_Tcutil_substitute(inst,_tmpB9);_tmpBA;});}
# 337
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 339
struct Cyc_Absyn_Datatypefield*_tmpBB=({struct Cyc_Absyn_Datatypefield*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=*f1;_tmpBC;});
_tmpBB->typs=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);
# 342
return _tmpBB;}
# 349
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 358
struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=0;_tmpC5;});
struct Cyc_List_List**_tmpBD=f2sp;
int cmp=-1;
# 362
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*_tmpBE=({struct Cyc_List_List*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd;_tmpBF->tl=0;_tmpBF;});
*_tmpBD=_tmpBE;
_tmpBD=& _tmpBE->tl;
f0s=f0s->tl;}
# 369
if(f0s == 0  || cmp > 0){
# 371
*incl=0;{
struct Cyc_List_List*_tmpC0=({struct Cyc_List_List*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->hd=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_tmpC1->tl=0;_tmpC1;});
*_tmpBD=_tmpC0;
_tmpBD=& _tmpC0->tl;};}else{
# 377
struct Cyc_Absyn_Datatypefield*_tmpC2=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpC2 != 0){
if(_tmpC2 != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmpC3=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->hd=_tmpC2;_tmpC4->tl=0;_tmpC4;});
*_tmpBD=_tmpC3;
_tmpBD=& _tmpC3->tl;};}else{
# 384
*res=0;}
# 386
f0s=f0s->tl;}}
# 391
if(f1s != 0){
*incl=0;
*_tmpBD=f1s;}else{
# 395
*_tmpBD=f0s;}
# 398
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 402
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0)return({struct _tuple16 _tmpC6;_tmpC6.f1=0;_tmpC6.f2=0;_tmpC6;});
if(f->tl == 0)return({struct _tuple16 _tmpC7;_tmpC7.f1=f;_tmpC7.f2=0;_tmpC7;});{
struct _tuple16 _tmpC8=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);struct _tuple16 _tmpC9=_tmpC8;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpCB;_LL39: _tmpCA=_tmpC9.f1;_tmpCB=_tmpC9.f2;_LL3A:;
return({struct _tuple16 _tmpCC;_tmpCC.f1=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=f->hd;_tmpCE->tl=_tmpCA;_tmpCE;});_tmpCC.f2=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd;_tmpCD->tl=_tmpCB;_tmpCD;});_tmpCC;});};}
# 411
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 414
struct _tuple16 _tmpCF=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);struct _tuple16 _tmpD0=_tmpCF;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD2;_LL3C: _tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;_LL3D:;
if(_tmpD1 != 0  && _tmpD1->tl != 0)
_tmpD1=Cyc_Tcdecl_sort_xdatatype_fields(_tmpD1,res,v,loc,msg);
if(_tmpD2 != 0  && _tmpD2->tl != 0)
_tmpD2=Cyc_Tcdecl_sort_xdatatype_fields(_tmpD2,res,v,loc,msg);
return Cyc_Tcdecl_merge_xdatatype_fields(_tmpD1,_tmpD2,0,0,0,res,({int*_tmpD3=_cycalloc_atomic(sizeof(*_tmpD3));_tmpD3[0]=1;_tmpD3;}),({const char*_tmpD4="@extensible datatype";_tag_dyneither(_tmpD4,sizeof(char),21);}),*v,loc,msg);}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 423
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 425
struct _dyneither_ptr _tmpD5=Cyc_Absynpp_qvar2string(d0->name);
struct _dyneither_ptr t=({const char*_tmpF5="datatype";_tag_dyneither(_tmpF5,sizeof(char),9);});
int _tmpD6=1;
# 432
if(d0->is_extensible != d1->is_extensible){
({struct Cyc_String_pa_PrintArg_struct _tmpDB;_tmpDB.tag=0;_tmpDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcdecl_is_x2string(d1->is_extensible));({struct Cyc_String_pa_PrintArg_struct _tmpDA;_tmpDA.tag=0;_tmpDA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible));({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5);({void*_tmpD7[3]={& _tmpD9,& _tmpDA,& _tmpDB};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpD8="expected %s to be a %s instead of a %s";_tag_dyneither(_tmpD8,sizeof(char),39);}),_tag_dyneither(_tmpD7,sizeof(void*),3));});});});});
_tmpD6=0;}else{
# 437
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 441
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpD5,loc,msg))return 0;{
# 444
struct _tuple10 _tmpDC=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpD5,loc,msg,0);struct _tuple10 _tmpDD=_tmpDC;enum Cyc_Absyn_Scope _tmpDE;int _tmpDF;_LL3F: _tmpDE=_tmpDD.f1;_tmpDF=_tmpDD.f2;_LL40:;
if(!_tmpDF)_tmpD6=0;{
# 447
struct Cyc_Absyn_Datatypedecl*d2;
# 450
{struct _tuple17 _tmpE0=({struct _tuple17 _tmpF3;_tmpF3.f1=d0->fields;_tmpF3.f2=d1->fields;_tmpF3;});struct _tuple17 _tmpE1=_tmpE0;struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE3;if(_tmpE1.f2 == 0){_LL42: _LL43:
 d2=d0;goto _LL41;}else{if(_tmpE1.f1 == 0){_LL44: _LL45:
 d2=d1;goto _LL41;}else{_LL46: _tmpE2=(struct Cyc_List_List*)(_tmpE1.f1)->v;_tmpE3=(struct Cyc_List_List*)(_tmpE1.f2)->v;_LL47: {
# 464 "tcdecl.cyc"
struct Cyc_List_List*_tmpE4=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 467
if(d0->is_extensible){
# 471
int _tmpE5=1;
struct Cyc_List_List*_tmpE6=Cyc_Tcdecl_merge_xdatatype_fields(_tmpE2,_tmpE3,_tmpE4,d0->tvs,d1->tvs,& _tmpD6,& _tmpE5,t,_tmpD5,loc,msg);
# 475
if(_tmpE5)
d2=d0;else{
# 478
d2=({struct Cyc_Absyn_Datatypedecl*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=*d0;_tmpE7;});
d2->sc=_tmpDE;
d2->fields=({struct Cyc_Core_Opt*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->v=_tmpE6;_tmpE8;});}}else{
# 484
for(0;_tmpE2 != 0  && _tmpE3 != 0;(_tmpE2=_tmpE2->tl,_tmpE3=_tmpE3->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpE2->hd,(struct Cyc_Absyn_Datatypefield*)_tmpE3->hd,_tmpE4,t,_tmpD5,msg);}
# 487
if(_tmpE2 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpED;_tmpED.tag=0;_tmpED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpE2->hd)->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5);({struct Cyc_String_pa_PrintArg_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpE9[3]={& _tmpEB,& _tmpEC,& _tmpED};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpEA="%s %s is missing field %s";_tag_dyneither(_tmpEA,sizeof(char),26);}),_tag_dyneither(_tmpE9,sizeof(void*),3));});});});});
_tmpD6=0;}
# 491
if(_tmpE3 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpE3->hd)->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5);({struct Cyc_String_pa_PrintArg_struct _tmpF0;_tmpF0.tag=0;_tmpF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpEE[3]={& _tmpF0,& _tmpF1,& _tmpF2};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpEF="%s %s has extra field %s";_tag_dyneither(_tmpEF,sizeof(char),25);}),_tag_dyneither(_tmpEE,sizeof(void*),3));});});});});
_tmpD6=0;}
# 495
d2=d0;}
# 497
goto _LL41;}}}_LL41:;}
# 500
if(!_tmpD6)return 0;
# 502
if(_tmpDE == d2->sc)
return d2;else{
# 505
d2=({struct Cyc_Absyn_Datatypedecl*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=*d2;_tmpF4;});
d2->sc=_tmpDE;
return d2;}};};}
# 511
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 513
struct _dyneither_ptr _tmpF6=Cyc_Absynpp_qvar2string(d0->name);
int _tmpF7=1;
# 517
struct _tuple10 _tmpF8=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,({const char*_tmp115="enum";_tag_dyneither(_tmp115,sizeof(char),5);}),_tmpF6,loc,msg,1);struct _tuple10 _tmpF9=_tmpF8;enum Cyc_Absyn_Scope _tmpFA;int _tmpFB;_LL49: _tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;_LL4A:;
if(!_tmpFB)_tmpF7=0;{
# 520
struct Cyc_Absyn_Enumdecl*d2;
# 523
{struct _tuple17 _tmpFC=({struct _tuple17 _tmp113;_tmp113.f1=d0->fields;_tmp113.f2=d1->fields;_tmp113;});struct _tuple17 _tmpFD=_tmpFC;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFF;if(_tmpFD.f2 == 0){_LL4C: _LL4D:
 d2=d0;goto _LL4B;}else{if(_tmpFD.f1 == 0){_LL4E: _LL4F:
 d2=d1;goto _LL4B;}else{_LL50: _tmpFE=(struct Cyc_List_List*)(_tmpFD.f1)->v;_tmpFF=(struct Cyc_List_List*)(_tmpFD.f2)->v;_LL51:
# 528
 for(0;_tmpFE != 0  && _tmpFF != 0;(_tmpFE=_tmpFE->tl,_tmpFF=_tmpFF->tl)){
struct Cyc_Absyn_Enumfield*_tmp100=(struct Cyc_Absyn_Enumfield*)_tmpFE->hd;struct Cyc_Absyn_Enumfield*_tmp101=_tmp100;struct _dyneither_ptr*_tmp102;struct Cyc_Absyn_Exp*_tmp103;unsigned int _tmp104;_LL53: _tmp102=(_tmp101->name)->f2;_tmp103=_tmp101->tag;_tmp104=_tmp101->loc;_LL54:;{
struct Cyc_Absyn_Enumfield*_tmp105=(struct Cyc_Absyn_Enumfield*)_tmpFF->hd;struct Cyc_Absyn_Enumfield*_tmp106=_tmp105;struct _dyneither_ptr*_tmp107;struct Cyc_Absyn_Exp*_tmp108;unsigned int _tmp109;_LL56: _tmp107=(_tmp106->name)->f2;_tmp108=_tmp106->tag;_tmp109=_tmp106->loc;_LL57:;
# 533
if(Cyc_strptrcmp(_tmp102,_tmp107)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0;_tmp10E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp107);({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp102);({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF6);({void*_tmp10A[3]={& _tmp10C,& _tmp10D,& _tmp10E};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp10B="enum %s: field name mismatch %s != %s";_tag_dyneither(_tmp10B,sizeof(char),38);}),_tag_dyneither(_tmp10A,sizeof(void*),3));});});});});
_tmpF7=0;}
# 538
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp103))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp108))){
({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0;_tmp112.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp107);({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0;_tmp111.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF6);({void*_tmp10F[2]={& _tmp111,& _tmp112};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp110="enum %s, field %s, value mismatch";_tag_dyneither(_tmp110,sizeof(char),34);}),_tag_dyneither(_tmp10F,sizeof(void*),2));});});});
_tmpF7=0;}};}
# 543
d2=d0;
goto _LL4B;}}_LL4B:;}
# 547
if(!_tmpF7)return 0;
# 549
if(d2->sc == _tmpFA)
return d2;else{
# 552
d2=({struct Cyc_Absyn_Enumdecl*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=*d2;_tmp114;});
d2->sc=_tmpFA;
return d2;}};}
# 558
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 562
int _tmp116=1;
# 565
struct _tuple10 _tmp117=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);struct _tuple10 _tmp118=_tmp117;enum Cyc_Absyn_Scope _tmp119;int _tmp11A;_LL59: _tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;_LL5A:;
if(!_tmp11A)_tmp116=0;
# 569
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _dyneither_ptr _tmp11B=Cyc_Absynpp_typ2string(t0);
struct _dyneither_ptr _tmp11C=Cyc_Absynpp_typ2string(t1);
if((((Cyc_strlen((struct _dyneither_ptr)_tmp11B)+ Cyc_strlen((struct _dyneither_ptr)_tmp11C))+ Cyc_strlen((struct _dyneither_ptr)t))+ Cyc_strlen((struct _dyneither_ptr)v))+ 
Cyc_strlen(({const char*_tmp11D="  was declared  instead of ";_tag_dyneither(_tmp11D,sizeof(char),28);}))> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11B);({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0;_tmp122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11C);({struct Cyc_String_pa_PrintArg_struct _tmp121;_tmp121.tag=0;_tmp121.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp120;_tmp120.tag=0;_tmp120.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp11E[4]={& _tmp120,& _tmp121,& _tmp122,& _tmp123};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp11F="%s %s was declared \n\t%s\n instead of \n\t%s";_tag_dyneither(_tmp11F,sizeof(char),41);}),_tag_dyneither(_tmp11E,sizeof(void*),4));});});});});});else{
# 577
({struct Cyc_String_pa_PrintArg_struct _tmp129;_tmp129.tag=0;_tmp129.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11B);({struct Cyc_String_pa_PrintArg_struct _tmp128;_tmp128.tag=0;_tmp128.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11C);({struct Cyc_String_pa_PrintArg_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp124[4]={& _tmp126,& _tmp127,& _tmp128,& _tmp129};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp125="%s %s was declared %s instead of %s";_tag_dyneither(_tmp125,sizeof(char),36);}),_tag_dyneither(_tmp124,sizeof(void*),4));});});});});});}
# 579
Cyc_Tcutil_explain_failure();
_tmp116=0;}
# 584
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp12A[2]={& _tmp12C,& _tmp12D};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp12B="%s %s has different type qualifiers";_tag_dyneither(_tmp12B,sizeof(char),36);}),_tag_dyneither(_tmp12A,sizeof(void*),2));});});});
_tmp116=0;}
# 590
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp131;_tmp131.tag=0;_tmp131.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp130;_tmp130.tag=0;_tmp130.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp12E[2]={& _tmp130,& _tmp131};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp12F="%s %s has different attributes";_tag_dyneither(_tmp12F,sizeof(char),31);}),_tag_dyneither(_tmp12E,sizeof(void*),2));});});});
({void*_tmp132=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp133="\tprevious attributes: ";_tag_dyneither(_tmp133,sizeof(char),23);}),_tag_dyneither(_tmp132,sizeof(void*),0));});
for(0;atts0 != 0;atts0=atts0->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd));({void*_tmp134[1]={& _tmp136};Cyc_fprintf(Cyc_stderr,({const char*_tmp135="%s ";_tag_dyneither(_tmp135,sizeof(char),4);}),_tag_dyneither(_tmp134,sizeof(void*),1));});});}
({void*_tmp137=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp138="\n\tcurrent attributes: ";_tag_dyneither(_tmp138,sizeof(char),23);}),_tag_dyneither(_tmp137,sizeof(void*),0));});
for(0;atts1 != 0;atts1=atts1->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp13B;_tmp13B.tag=0;_tmp13B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd));({void*_tmp139[1]={& _tmp13B};Cyc_fprintf(Cyc_stderr,({const char*_tmp13A="%s ";_tag_dyneither(_tmp13A,sizeof(char),4);}),_tag_dyneither(_tmp139,sizeof(void*),1));});});}
({void*_tmp13C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13D="\n";_tag_dyneither(_tmp13D,sizeof(char),2);}),_tag_dyneither(_tmp13C,sizeof(void*),0));});
_tmp116=0;}
# 602
return({struct _tuple10 _tmp13E;_tmp13E.f1=_tmp119;_tmp13E.f2=_tmp116;_tmp13E;});}
# 605
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 607
struct _dyneither_ptr _tmp13F=Cyc_Absynpp_qvar2string(d0->name);
# 610
struct _tuple10 _tmp140=
Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,({const char*_tmp146="variable";_tag_dyneither(_tmp146,sizeof(char),9);}),_tmp13F,loc,msg);
# 610
struct _tuple10 _tmp141=_tmp140;enum Cyc_Absyn_Scope _tmp142;int _tmp143;_LL5C: _tmp142=_tmp141.f1;_tmp143=_tmp141.f2;_LL5D:;
# 614
if(!_tmp143)return 0;
# 616
if(d0->sc == _tmp142)
return d0;else{
# 619
struct Cyc_Absyn_Vardecl*_tmp144=({struct Cyc_Absyn_Vardecl*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[0]=*d0;_tmp145;});
_tmp144->sc=_tmp142;
return _tmp144;}}
# 625
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 627
struct _dyneither_ptr _tmp147=Cyc_Absynpp_qvar2string(d0->name);
# 630
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_tmp148="typedef";_tag_dyneither(_tmp148,sizeof(char),8);}),_tmp147,loc,msg))return 0;{
# 633
struct Cyc_List_List*_tmp149=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 636
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp149,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn)));({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1));({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp147);({void*_tmp14A[3]={& _tmp14C,& _tmp14D,& _tmp14E};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp14B="typedef %s does not refer to the same type: %s != %s";_tag_dyneither(_tmp14B,sizeof(char),53);}),_tag_dyneither(_tmp14A,sizeof(void*),3));});});});});
return 0;}}
# 644
return d0;};}
# 647
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp14F=({struct _tuple14 _tmp169;_tmp169.f1=b0;_tmp169.f2=b1;_tmp169;});struct _tuple14 _tmp150=_tmp14F;struct Cyc_Absyn_Fndecl*_tmp151;struct Cyc_Absyn_Vardecl*_tmp152;struct Cyc_Absyn_Fndecl*_tmp153;struct Cyc_Absyn_Vardecl*_tmp154;struct Cyc_Absyn_Fndecl*_tmp155;struct Cyc_Absyn_Vardecl*_tmp156;struct Cyc_Absyn_Vardecl*_tmp157;switch(*((int*)_tmp150.f1)){case 0: if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp150.f2)->tag == 0){_LL5F: _LL60:
 return(void*)& Cyc_Absyn_Unresolved_b_val;}else{goto _LL69;}case 1: switch(*((int*)_tmp150.f2)){case 1: _LL61: _tmp156=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f1)->f1;_tmp157=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL62: {
# 651
struct Cyc_Absyn_Vardecl*_tmp158=Cyc_Tcdecl_merge_vardecl(_tmp156,_tmp157,loc,msg);
if(_tmp158 == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
if(_tmp158 == _tmp156)return b0;
if(_tmp158 == _tmp157)return b1;
return(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp15A;_tmp15A.tag=1;_tmp15A.f1=_tmp158;_tmp15A;});_tmp159;});}case 2: _LL63: _tmp154=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f1)->f1;_tmp155=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL64: {
# 657
struct _tuple10 _tmp15B=Cyc_Tcdecl_check_var_or_fn_decl(_tmp154->sc,_tmp154->type,_tmp154->tq,_tmp154->attributes,_tmp155->sc,(void*)_check_null(_tmp155->cached_typ),
Cyc_Absyn_empty_tqual(0),_tmp155->attributes,({const char*_tmp15E="function";_tag_dyneither(_tmp15E,sizeof(char),9);}),
# 660
Cyc_Absynpp_qvar2string(_tmp154->name),loc,msg);
# 657
struct _tuple10 _tmp15C=_tmp15B;int _tmp15D;_LL6C: _tmp15D=_tmp15C.f2;_LL6D:;
# 661
return _tmp15D?b1:(void*)& Cyc_Absyn_Unresolved_b_val;}default: goto _LL69;}case 2: switch(*((int*)_tmp150.f2)){case 2: _LL65: _tmp153=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL66:
# 663
({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0;_tmp161.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp153->name));({void*_tmp15F[1]={& _tmp161};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp160="redefinition of function %s";_tag_dyneither(_tmp160,sizeof(char),28);}),_tag_dyneither(_tmp15F,sizeof(void*),1));});});
return(void*)& Cyc_Absyn_Unresolved_b_val;case 1: _LL67: _tmp151=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp150.f1)->f1;_tmp152=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL68: {
# 666
struct _tuple10 _tmp162=Cyc_Tcdecl_check_var_or_fn_decl(_tmp151->sc,(void*)_check_null(_tmp151->cached_typ),Cyc_Absyn_empty_tqual(0),_tmp151->attributes,_tmp152->sc,_tmp152->type,_tmp152->tq,_tmp152->attributes,({const char*_tmp165="variable";_tag_dyneither(_tmp165,sizeof(char),9);}),
# 669
Cyc_Absynpp_qvar2string(_tmp151->name),loc,msg);
# 666
struct _tuple10 _tmp163=_tmp162;int _tmp164;_LL6F: _tmp164=_tmp163.f2;_LL70:;
# 670
return _tmp164?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}default: goto _LL69;}default: _LL69: _LL6A:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp167;_tmp167.tag=Cyc_Core_Invalid_argument;_tmp167.f1=({const char*_tmp168="Tcdecl::merge_binding";_tag_dyneither(_tmp168,sizeof(char),22);});_tmp167;});_tmp166;}));}_LL5E:;}
# 675
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 678
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp16A=d0;struct Cyc_Absyn_Datatypedecl*_tmp16B;struct Cyc_Absyn_Datatypefield*_tmp16C;_LL72: _tmp16B=_tmp16A->base;_tmp16C=_tmp16A->field;_LL73:;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp16D=d1;struct Cyc_Absyn_Datatypedecl*_tmp16E;struct Cyc_Absyn_Datatypefield*_tmp16F;_LL75: _tmp16E=_tmp16D->base;_tmp16F=_tmp16D->field;_LL76:;{
struct _dyneither_ptr _tmp170=Cyc_Absynpp_qvar2string(_tmp16C->name);
# 683
if(Cyc_Absyn_qvar_cmp(_tmp16B->name,_tmp16E->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp170);({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp16E->name));({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0;_tmp173.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp16B->name));({void*_tmp171[3]={& _tmp173,& _tmp174,& _tmp175};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp172="@extensible datatype %s and %s have both a field named %s";_tag_dyneither(_tmp172,sizeof(char),58);}),_tag_dyneither(_tmp171,sizeof(void*),3));});});});});
return 0;}
# 690
if(!Cyc_Tcdecl_check_tvs(_tmp16B->tvs,_tmp16E->tvs,({const char*_tmp176="@extensible datatype";_tag_dyneither(_tmp176,sizeof(char),21);}),Cyc_Absynpp_qvar2string(_tmp16B->name),loc,msg))
return 0;{
# 693
struct Cyc_List_List*_tmp177=Cyc_Tcdecl_build_tvs_map(_tmp16B->tvs,_tmp16E->tvs);
# 696
struct Cyc_Absyn_Datatypefield*_tmp178=Cyc_Tcdecl_merge_datatypefield(_tmp16C,_tmp16F,_tmp177,({const char*_tmp17A="@extensible datatype field";_tag_dyneither(_tmp17A,sizeof(char),27);}),_tmp170,msg);
if(_tmp178 == 0)return 0;
if(_tmp178 == _tmp16C)return d0;
return({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->base=_tmp16B;_tmp179->field=_tmp178;_tmp179;});};};};}
