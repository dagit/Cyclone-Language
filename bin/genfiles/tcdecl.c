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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 768 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 934
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 944
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 963
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1184
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
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;
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
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void _tmp190(struct _dyneither_ptr**msg1,struct _dyneither_ptr*ap,unsigned int*_tmp18F,unsigned int*_tmp18E,void***_tmp18C){for(*_tmp18F=0;*_tmp18F < *_tmp18E;(*_tmp18F)++){struct Cyc_String_pa_PrintArg_struct _tmp18A;struct Cyc_String_pa_PrintArg_struct*_tmp189;(*_tmp18C)[*_tmp18F]=(void*)(
# 57
*_tmp18F == 0?(void*)((_tmp189=_cycalloc(sizeof(*_tmp189)),((_tmp189[0]=((_tmp18A.tag=0,((_tmp18A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*msg1)),_tmp18A)))),_tmp189)))):*((void**)_check_dyneither_subscript(*ap,sizeof(void*),(int)(*_tmp18F - 1))));}}
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 49
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _dyneither_ptr fmt2;
if(Cyc_strlen((struct _dyneither_ptr)*msg1)== 0){
const char*_tmp186;fmt2=(struct _dyneither_ptr)Cyc_strconcat(((_tmp186="%s",_tag_dyneither(_tmp186,sizeof(char),3))),(struct _dyneither_ptr)fmt);}else{
# 55
const char*_tmp187;fmt2=(struct _dyneither_ptr)Cyc_strconcat(((_tmp187="%s ",_tag_dyneither(_tmp187,sizeof(char),4))),(struct _dyneither_ptr)fmt);}{
unsigned int _tmp18F;unsigned int _tmp18E;struct _dyneither_ptr _tmp18D;void**_tmp18C;unsigned int _tmp18B;struct _dyneither_ptr ap2=(_tmp18B=_get_dyneither_size(ap,sizeof(void*))+ 1,((_tmp18C=(void**)_cycalloc(_check_times(sizeof(void*),_tmp18B)),((_tmp18D=_tag_dyneither(_tmp18C,sizeof(void*),_tmp18B),((((_tmp18E=_tmp18B,_tmp190(& msg1,& ap,& _tmp18F,& _tmp18E,& _tmp18C))),_tmp18D)))))));
# 58
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)
Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));};};}
# 64
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 66
const char*_tmp191;struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?(_tmp191="public ",_tag_dyneither(_tmp191,sizeof(char),8)): Cyc_Absynpp_scope2string(s0);
const char*_tmp192;struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?(_tmp192="public ",_tag_dyneither(_tmp192,sizeof(char),8)): Cyc_Absynpp_scope2string(s1);
const char*_tmp199;void*_tmp198[4];struct Cyc_String_pa_PrintArg_struct _tmp197;struct Cyc_String_pa_PrintArg_struct _tmp196;struct Cyc_String_pa_PrintArg_struct _tmp195;struct Cyc_String_pa_PrintArg_struct _tmp194;(_tmp194.tag=0,((_tmp194.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0_str),((_tmp195.tag=0,((_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1_str),((_tmp196.tag=0,((_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp197.tag=0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp198[0]=& _tmp197,((_tmp198[1]=& _tmp196,((_tmp198[2]=& _tmp195,((_tmp198[3]=& _tmp194,Cyc_Tcdecl_merr(loc,msg,((_tmp199="%s %s is %sbut expected scope is %s",_tag_dyneither(_tmp199,sizeof(char),36))),_tag_dyneither(_tmp198,sizeof(void*),4)))))))))))))))))))))))));}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 72
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 74
{struct _tuple11 _tmp19A;struct _tuple11 _tmp11=(_tmp19A.f1=s0,((_tmp19A.f2=s1,_tmp19A)));struct _tuple11 _tmp12=_tmp11;_LL1: if(_tmp12.f1 != Cyc_Absyn_ExternC)goto _LL3;if(_tmp12.f2 != Cyc_Absyn_ExternC)goto _LL3;_LL2:
# 76
 goto _LL0;_LL3: if(_tmp12.f1 != Cyc_Absyn_ExternC)goto _LL5;if(_tmp12.f2 != Cyc_Absyn_Public)goto _LL5;_LL4:
# 78
 goto _LL6;_LL5: if(_tmp12.f1 != Cyc_Absyn_ExternC)goto _LL7;if(_tmp12.f2 != Cyc_Absyn_Extern)goto _LL7;_LL6:
 goto _LL8;_LL7: if(_tmp12.f1 != Cyc_Absyn_Public)goto _LL9;if(_tmp12.f2 != Cyc_Absyn_ExternC)goto _LL9;_LL8:
 goto _LLA;_LL9: if(_tmp12.f1 != Cyc_Absyn_Extern)goto _LLB;if(_tmp12.f2 != Cyc_Absyn_ExternC)goto _LLB;_LLA:
 if(externCmerge)goto _LL0;
goto _LLC;_LLB: if(_tmp12.f1 != Cyc_Absyn_ExternC)goto _LLD;_LLC:
 goto _LLE;_LLD: if(_tmp12.f2 != Cyc_Absyn_ExternC)goto _LLF;_LLE:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp19B;return(_tmp19B.f1=s1,((_tmp19B.f2=0,_tmp19B)));};_LLF: if(_tmp12.f2 != Cyc_Absyn_Extern)goto _LL11;_LL10:
# 86
 s1=s0;goto _LL0;_LL11: if(_tmp12.f1 != Cyc_Absyn_Extern)goto _LL13;_LL12:
 goto _LL0;_LL13: if(_tmp12.f1 != Cyc_Absyn_Static)goto _LL15;if(_tmp12.f2 != Cyc_Absyn_Static)goto _LL15;_LL14:
# 89
 goto _LL16;_LL15: if(_tmp12.f1 != Cyc_Absyn_Public)goto _LL17;if(_tmp12.f2 != Cyc_Absyn_Public)goto _LL17;_LL16:
 goto _LL18;_LL17: if(_tmp12.f1 != Cyc_Absyn_Abstract)goto _LL19;if(_tmp12.f2 != Cyc_Absyn_Abstract)goto _LL19;_LL18:
 goto _LL0;_LL19: if(_tmp12.f1 != Cyc_Absyn_Register)goto _LL1B;if(_tmp12.f2 != Cyc_Absyn_Register)goto _LL1B;_LL1A:
 goto _LL0;_LL1B:;_LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp19C;return(_tmp19C.f1=s1,((_tmp19C.f2=0,_tmp19C)));};_LL0:;}{
# 95
struct _tuple10 _tmp19D;return(_tmp19D.f1=s1,((_tmp19D.f2=1,_tmp19D)));};}
# 98
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 100
return Cyc_Tcutil_unify(t0,t1);}
# 104
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmp17=e->r;void*_tmp18=_tmp17;int _tmp1A;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp19=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18;if(_tmp19->tag != 0)goto _LL20;else{if(((_tmp19->f1).Int_c).tag != 5)goto _LL20;_tmp1A=((struct _tuple5)((_tmp19->f1).Int_c).val).f2;}}_LL1F:
 return(unsigned int)_tmp1A;_LL20:;_LL21: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp1A3;const char*_tmp1A2;struct Cyc_Core_Invalid_argument_exn_struct*_tmp1A1;(int)_throw((void*)((_tmp1A1=_cycalloc(sizeof(*_tmp1A1)),((_tmp1A1[0]=((_tmp1A3.tag=Cyc_Core_Invalid_argument,((_tmp1A3.f1=((_tmp1A2="Tcdecl::get_uint_const_value",_tag_dyneither(_tmp1A2,sizeof(char),29))),_tmp1A3)))),_tmp1A1)))));}_LL1D:;}
# 111
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 114
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){
{const char*_tmp1A8;void*_tmp1A7[2];struct Cyc_String_pa_PrintArg_struct _tmp1A6;struct Cyc_String_pa_PrintArg_struct _tmp1A5;(_tmp1A5.tag=0,((_tmp1A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1A6.tag=0,((_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1A7[0]=& _tmp1A6,((_tmp1A7[1]=& _tmp1A5,Cyc_Tcdecl_merr(loc,msg,((_tmp1A8="%s %s has a different number of type parameters",_tag_dyneither(_tmp1A8,sizeof(char),48))),_tag_dyneither(_tmp1A7,sizeof(void*),2)))))))))))));}
return 0;}{
# 118
struct Cyc_List_List*_tmp22=tvs0;
struct Cyc_List_List*_tmp23=tvs1;
for(0;_tmp22 != 0;(_tmp22=_tmp22->tl,_tmp23=_tmp23->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp22->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp23))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp24=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp22->hd)->kind);
struct Cyc_Absyn_Kind*_tmp25=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp23->hd)->kind);
if(_tmp24 == _tmp25)continue;
{const char*_tmp1B0;void*_tmp1AF[5];struct Cyc_String_pa_PrintArg_struct _tmp1AE;struct Cyc_String_pa_PrintArg_struct _tmp1AD;struct Cyc_String_pa_PrintArg_struct _tmp1AC;struct Cyc_String_pa_PrintArg_struct _tmp1AB;struct Cyc_String_pa_PrintArg_struct _tmp1AA;(_tmp1AA.tag=0,((_tmp1AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp25)),((_tmp1AB.tag=0,((_tmp1AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp22->hd)->name),((_tmp1AC.tag=0,((_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp24)),((_tmp1AD.tag=0,((_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1AE.tag=0,((_tmp1AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1AF[0]=& _tmp1AE,((_tmp1AF[1]=& _tmp1AD,((_tmp1AF[2]=& _tmp1AC,((_tmp1AF[3]=& _tmp1AB,((_tmp1AF[4]=& _tmp1AA,Cyc_Tcdecl_merr(loc,msg,((_tmp1B0="%s %s has a different kind (%s) for type parameter %s (%s)",_tag_dyneither(_tmp1B0,sizeof(char),59))),_tag_dyneither(_tmp1AF,sizeof(void*),5)))))))))))))))))))))))))))))));}
return 0;};}
# 129
return 1;};}
# 132
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 134
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp1B5;void*_tmp1B4[2];struct Cyc_String_pa_PrintArg_struct _tmp1B3;struct Cyc_String_pa_PrintArg_struct _tmp1B2;(_tmp1B2.tag=0,((_tmp1B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1B3.tag=0,((_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1B4[0]=& _tmp1B3,((_tmp1B4[1]=& _tmp1B2,Cyc_Tcdecl_merr(loc,msg,((_tmp1B5="%s %s has different attributes",_tag_dyneither(_tmp1B5,sizeof(char),31))),_tag_dyneither(_tmp1B4,sizeof(void*),2)))))))))))));}
return 0;}
# 138
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 141
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 143
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
struct _tuple12*_tmp1BF;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1BE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1BD;struct Cyc_List_List*_tmp1BC;inst=((_tmp1BC=_cycalloc(sizeof(*_tmp1BC)),((_tmp1BC->hd=((_tmp1BF=_cycalloc(sizeof(*_tmp1BF)),((_tmp1BF->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd,((_tmp1BF->f2=(void*)((_tmp1BD=_cycalloc(sizeof(*_tmp1BD)),((_tmp1BD[0]=((_tmp1BE.tag=2,((_tmp1BE.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp1BE)))),_tmp1BD)))),_tmp1BF)))))),((_tmp1BC->tl=inst,_tmp1BC))))));}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 149
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 151
struct _dyneither_ptr _tmp35=Cyc_Absynpp_qvar2string(d0->name);
int _tmp36=1;
# 157
if(!(d0->kind == d1->kind))return 0;
# 160
{const char*_tmp1C0;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp1C0="type",_tag_dyneither(_tmp1C0,sizeof(char),5))),_tmp35,loc,msg))return 0;}{
# 163
const char*_tmp1C1;struct _tuple10 _tmp38=
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp1C1="type",_tag_dyneither(_tmp1C1,sizeof(char),5))),_tmp35,loc,msg,1);
# 163
enum Cyc_Absyn_Scope _tmp3A;int _tmp3B;struct _tuple10 _tmp39=_tmp38;_tmp3A=_tmp39.f1;_tmp3B=_tmp39.f2;
# 165
if(!_tmp3B)_tmp36=0;
# 168
{const char*_tmp1C2;if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,((_tmp1C2="type",_tag_dyneither(_tmp1C2,sizeof(char),5))),_tmp35,loc,msg))_tmp36=0;}{
# 170
struct Cyc_Absyn_Aggrdecl*d2;
# 172
{struct _tuple13 _tmp1C3;struct _tuple13 _tmp3D=(_tmp1C3.f1=d0->impl,((_tmp1C3.f2=d1->impl,_tmp1C3)));struct _tuple13 _tmp3E=_tmp3D;struct Cyc_List_List*_tmp3F;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;int _tmp42;struct Cyc_List_List*_tmp43;struct Cyc_List_List*_tmp44;struct Cyc_List_List*_tmp45;int _tmp46;_LL23: if(_tmp3E.f2 != 0)goto _LL25;_LL24:
 d2=d0;goto _LL22;_LL25: if(_tmp3E.f1 != 0)goto _LL27;_LL26:
 d2=d1;goto _LL22;_LL27: if(_tmp3E.f1 == 0)goto _LL22;_tmp3F=(_tmp3E.f1)->exist_vars;_tmp40=(_tmp3E.f1)->rgn_po;_tmp41=(_tmp3E.f1)->fields;_tmp42=(_tmp3E.f1)->tagged;if(_tmp3E.f2 == 0)goto _LL22;_tmp43=(_tmp3E.f2)->exist_vars;_tmp44=(_tmp3E.f2)->rgn_po;_tmp45=(_tmp3E.f2)->fields;_tmp46=(_tmp3E.f2)->tagged;_LL28:
# 179
{const char*_tmp1C4;if(!Cyc_Tcdecl_check_tvs(_tmp3F,_tmp43,((_tmp1C4="type",_tag_dyneither(_tmp1C4,sizeof(char),5))),_tmp35,loc,msg))
return 0;}{
# 183
struct Cyc_List_List*_tmp48=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp3F),
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp43));
# 188
for(0;_tmp40 != 0  && _tmp44 != 0;
(_tmp40=_tmp40->tl,_tmp44=_tmp44->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp40->hd)).f1,(*((struct _tuple14*)_tmp44->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp40->hd)).f2,(*((struct _tuple14*)_tmp44->hd)).f2);}
# 195
for(0;_tmp41 != 0  && _tmp45 != 0;(_tmp41=_tmp41->tl,_tmp45=_tmp45->tl)){
struct Cyc_Absyn_Aggrfield*_tmp49=(struct Cyc_Absyn_Aggrfield*)_tmp41->hd;struct _dyneither_ptr*_tmp4B;struct Cyc_Absyn_Tqual _tmp4C;void*_tmp4D;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_List_List*_tmp4F;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_Absyn_Aggrfield*_tmp4A=_tmp49;_tmp4B=_tmp4A->name;_tmp4C=_tmp4A->tq;_tmp4D=_tmp4A->type;_tmp4E=_tmp4A->width;_tmp4F=_tmp4A->attributes;_tmp50=_tmp4A->requires_clause;{
struct Cyc_Absyn_Aggrfield*_tmp51=(struct Cyc_Absyn_Aggrfield*)_tmp45->hd;struct _dyneither_ptr*_tmp53;struct Cyc_Absyn_Tqual _tmp54;void*_tmp55;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_List_List*_tmp57;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Aggrfield*_tmp52=_tmp51;_tmp53=_tmp52->name;_tmp54=_tmp52->tq;_tmp55=_tmp52->type;_tmp56=_tmp52->width;_tmp57=_tmp52->attributes;_tmp58=_tmp52->requires_clause;
# 199
if(Cyc_strptrcmp(_tmp4B,_tmp53)!= 0){
{const char*_tmp1CD;void*_tmp1CC[4];const char*_tmp1CB;struct Cyc_String_pa_PrintArg_struct _tmp1CA;struct Cyc_String_pa_PrintArg_struct _tmp1C9;struct Cyc_String_pa_PrintArg_struct _tmp1C8;struct Cyc_String_pa_PrintArg_struct _tmp1C7;(_tmp1C7.tag=0,((_tmp1C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp53),((_tmp1C8.tag=0,((_tmp1C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B),((_tmp1C9.tag=0,((_tmp1C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp1CA.tag=0,((_tmp1CA.f1=(struct _dyneither_ptr)((_tmp1CB="type",_tag_dyneither(_tmp1CB,sizeof(char),5))),((_tmp1CC[0]=& _tmp1CA,((_tmp1CC[1]=& _tmp1C9,((_tmp1CC[2]=& _tmp1C8,((_tmp1CC[3]=& _tmp1C7,Cyc_Tcdecl_merr(loc,msg,((_tmp1CD="%s %s : field name mismatch %s != %s",_tag_dyneither(_tmp1CD,sizeof(char),37))),_tag_dyneither(_tmp1CC,sizeof(void*),4)))))))))))))))))))))))));}
# 202
return 0;}
# 204
if(!Cyc_Tcutil_same_atts(_tmp4F,_tmp57)){
{const char*_tmp1D5;void*_tmp1D4[3];const char*_tmp1D3;struct Cyc_String_pa_PrintArg_struct _tmp1D2;struct Cyc_String_pa_PrintArg_struct _tmp1D1;struct Cyc_String_pa_PrintArg_struct _tmp1D0;(_tmp1D0.tag=0,((_tmp1D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B),((_tmp1D1.tag=0,((_tmp1D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp1D2.tag=0,((_tmp1D2.f1=(struct _dyneither_ptr)((_tmp1D3="type",_tag_dyneither(_tmp1D3,sizeof(char),5))),((_tmp1D4[0]=& _tmp1D2,((_tmp1D4[1]=& _tmp1D1,((_tmp1D4[2]=& _tmp1D0,Cyc_Tcdecl_merr(loc,msg,((_tmp1D5="%s %s : attribute mismatch on field %s",_tag_dyneither(_tmp1D5,sizeof(char),39))),_tag_dyneither(_tmp1D4,sizeof(void*),3)))))))))))))))))));}
_tmp36=0;}
# 208
if(!Cyc_Tcutil_equal_tqual(_tmp4C,_tmp54)){
{const char*_tmp1DD;void*_tmp1DC[3];const char*_tmp1DB;struct Cyc_String_pa_PrintArg_struct _tmp1DA;struct Cyc_String_pa_PrintArg_struct _tmp1D9;struct Cyc_String_pa_PrintArg_struct _tmp1D8;(_tmp1D8.tag=0,((_tmp1D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B),((_tmp1D9.tag=0,((_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((_tmp1DB="type",_tag_dyneither(_tmp1DB,sizeof(char),5))),((_tmp1DC[0]=& _tmp1DA,((_tmp1DC[1]=& _tmp1D9,((_tmp1DC[2]=& _tmp1D8,Cyc_Tcdecl_merr(loc,msg,((_tmp1DD="%s %s : qualifier mismatch on field %s",_tag_dyneither(_tmp1DD,sizeof(char),39))),_tag_dyneither(_tmp1DC,sizeof(void*),3)))))))))))))))))));}
_tmp36=0;}
# 212
if(((_tmp4E != 0  && _tmp56 != 0) && 
Cyc_Tcdecl_get_uint_const_value(_tmp4E)!= 
Cyc_Tcdecl_get_uint_const_value(_tmp56) || 
_tmp4E == 0  && _tmp56 != 0) || 
_tmp4E != 0  && _tmp56 == 0){
{const char*_tmp1E5;void*_tmp1E4[3];const char*_tmp1E3;struct Cyc_String_pa_PrintArg_struct _tmp1E2;struct Cyc_String_pa_PrintArg_struct _tmp1E1;struct Cyc_String_pa_PrintArg_struct _tmp1E0;(_tmp1E0.tag=0,((_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B),((_tmp1E1.tag=0,((_tmp1E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp1E2.tag=0,((_tmp1E2.f1=(struct _dyneither_ptr)((_tmp1E3="type",_tag_dyneither(_tmp1E3,sizeof(char),5))),((_tmp1E4[0]=& _tmp1E2,((_tmp1E4[1]=& _tmp1E1,((_tmp1E4[2]=& _tmp1E0,Cyc_Tcdecl_merr(loc,msg,((_tmp1E5="%s %s : bitfield mismatch on field %s",_tag_dyneither(_tmp1E5,sizeof(char),38))),_tag_dyneither(_tmp1E4,sizeof(void*),3)))))))))))))))))));}
_tmp36=0;}
# 220
if(_tmp50 != 0  && _tmp58 != 0){
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1E8;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E7;void*subst_w1=Cyc_Tcutil_substitute(_tmp48,(void*)((_tmp1E7=_cycalloc(sizeof(*_tmp1E7)),((_tmp1E7[0]=((_tmp1E8.tag=18,((_tmp1E8.f1=_tmp58,_tmp1E8)))),_tmp1E7)))));
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1EB;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1EA;if(!Cyc_Tcdecl_check_type((void*)((_tmp1EA=_cycalloc(sizeof(*_tmp1EA)),((_tmp1EA[0]=((_tmp1EB.tag=18,((_tmp1EB.f1=_tmp50,_tmp1EB)))),_tmp1EA)))),subst_w1)){
{const char*_tmp1F1;void*_tmp1F0[3];struct Cyc_String_pa_PrintArg_struct _tmp1EF;struct Cyc_String_pa_PrintArg_struct _tmp1EE;struct Cyc_String_pa_PrintArg_struct _tmp1ED;(_tmp1ED.tag=0,((_tmp1ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp58)),((_tmp1EE.tag=0,((_tmp1EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp50)),((_tmp1EF.tag=0,((_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp1F0[0]=& _tmp1EF,((_tmp1F0[1]=& _tmp1EE,((_tmp1F0[2]=& _tmp1ED,Cyc_Tcdecl_merr(loc,msg,((_tmp1F1="type %s : mismatch on @requires clauses: %s != %s",_tag_dyneither(_tmp1F1,sizeof(char),50))),_tag_dyneither(_tmp1F0,sizeof(void*),3)))))))))))))))))));}
_tmp36=0;}}else{
# 227
if(_tmp50 == 0  && _tmp58 != 0  || 
_tmp50 != 0  && _tmp58 == 0){
{const char*_tmp1F5;void*_tmp1F4[1];struct Cyc_String_pa_PrintArg_struct _tmp1F3;(_tmp1F3.tag=0,((_tmp1F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp1F4[0]=& _tmp1F3,Cyc_Tcdecl_merr(loc,msg,((_tmp1F5="type %s : mismatch on @requires clauses",_tag_dyneither(_tmp1F5,sizeof(char),40))),_tag_dyneither(_tmp1F4,sizeof(void*),1)))))));}
_tmp36=0;}}{
# 233
void*subst_t1=Cyc_Tcutil_substitute(_tmp48,_tmp55);
if(!Cyc_Tcdecl_check_type(_tmp4D,subst_t1)){
{const char*_tmp1FC;void*_tmp1FB[4];struct Cyc_String_pa_PrintArg_struct _tmp1FA;struct Cyc_String_pa_PrintArg_struct _tmp1F9;struct Cyc_String_pa_PrintArg_struct _tmp1F8;struct Cyc_String_pa_PrintArg_struct _tmp1F7;(_tmp1F7.tag=0,((_tmp1F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp1F8.tag=0,((_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4D)),((_tmp1F9.tag=0,((_tmp1F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B),((_tmp1FA.tag=0,((_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp1FB[0]=& _tmp1FA,((_tmp1FB[1]=& _tmp1F9,((_tmp1FB[2]=& _tmp1F8,((_tmp1FB[3]=& _tmp1F7,Cyc_Tcdecl_merr(loc,msg,((_tmp1FC="type %s : type mismatch on field %s: %s != %s",_tag_dyneither(_tmp1FC,sizeof(char),46))),_tag_dyneither(_tmp1FB,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp36=0;}};};}
# 242
if(_tmp41 != 0){
{const char*_tmp201;void*_tmp200[2];struct Cyc_String_pa_PrintArg_struct _tmp1FF;struct Cyc_String_pa_PrintArg_struct _tmp1FE;(_tmp1FE.tag=0,((_tmp1FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp41->hd)->name),((_tmp1FF.tag=0,((_tmp1FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp200[0]=& _tmp1FF,((_tmp200[1]=& _tmp1FE,Cyc_Tcdecl_merr(loc,msg,((_tmp201="type %s is missing field %s",_tag_dyneither(_tmp201,sizeof(char),28))),_tag_dyneither(_tmp200,sizeof(void*),2)))))))))))));}
_tmp36=0;}
# 246
if(_tmp45 != 0){
{const char*_tmp206;void*_tmp205[2];struct Cyc_String_pa_PrintArg_struct _tmp204;struct Cyc_String_pa_PrintArg_struct _tmp203;(_tmp203.tag=0,((_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp45->hd)->name),((_tmp204.tag=0,((_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp205[0]=& _tmp204,((_tmp205[1]=& _tmp203,Cyc_Tcdecl_merr(loc,msg,((_tmp206="type %s has extra field %s",_tag_dyneither(_tmp206,sizeof(char),27))),_tag_dyneither(_tmp205,sizeof(void*),2)))))))))))));}
_tmp36=0;}
# 251
if(_tmp42 != _tmp46){
{const char*_tmp20A;void*_tmp209[1];struct Cyc_String_pa_PrintArg_struct _tmp208;(_tmp208.tag=0,((_tmp208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35),((_tmp209[0]=& _tmp208,Cyc_Tcdecl_merr(loc,msg,((_tmp20A="%s : previous declaration disagrees with @tagged qualifier",_tag_dyneither(_tmp20A,sizeof(char),59))),_tag_dyneither(_tmp209,sizeof(void*),1)))))));}
_tmp36=0;}
# 256
d2=d0;
goto _LL22;};_LL22:;}
# 260
if(!_tmp36)return 0;
# 262
if(_tmp3A == d2->sc)
return d2;else{
# 265
{struct Cyc_Absyn_Aggrdecl*_tmp20B;d2=((_tmp20B=_cycalloc(sizeof(*_tmp20B)),((_tmp20B[0]=*d2,_tmp20B))));}
d2->sc=_tmp3A;
return d2;}};};}
# 271
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
const char*_tmp20D;const char*_tmp20C;return is_x?(_tmp20D="@extensible datatype",_tag_dyneither(_tmp20D,sizeof(char),21)):((_tmp20C="datatype",_tag_dyneither(_tmp20C,sizeof(char),9)));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 277
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 281
unsigned int loc=f1->loc;
# 284
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
{const char*_tmp214;void*_tmp213[4];struct Cyc_String_pa_PrintArg_struct _tmp212;struct Cyc_String_pa_PrintArg_struct _tmp211;struct Cyc_String_pa_PrintArg_struct _tmp210;struct Cyc_String_pa_PrintArg_struct _tmp20F;(_tmp20F.tag=0,((_tmp20F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2),((_tmp210.tag=0,((_tmp210.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2),((_tmp211.tag=0,((_tmp211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp212.tag=0,((_tmp212.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp213[0]=& _tmp212,((_tmp213[1]=& _tmp211,((_tmp213[2]=& _tmp210,((_tmp213[3]=& _tmp20F,Cyc_Tcdecl_merr(loc,msg,((_tmp214="%s %s: field name mismatch %s != %s",_tag_dyneither(_tmp214,sizeof(char),36))),_tag_dyneither(_tmp213,sizeof(void*),4)))))))))))))))))))))))));}
# 287
return 0;}{
# 290
struct _dyneither_ptr _tmp9A=*(*f0->name).f2;
# 293
const char*_tmp219;void*_tmp218[2];struct Cyc_String_pa_PrintArg_struct _tmp217;struct Cyc_String_pa_PrintArg_struct _tmp216;struct _tuple10 _tmp9B=
Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct _dyneither_ptr)((_tmp216.tag=0,((_tmp216.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp217.tag=0,((_tmp217.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp218[0]=& _tmp217,((_tmp218[1]=& _tmp216,Cyc_aprintf(((_tmp219="in %s %s, field",_tag_dyneither(_tmp219,sizeof(char),16))),_tag_dyneither(_tmp218,sizeof(void*),2)))))))))))))),_tmp9A,loc,msg,0);
# 293
enum Cyc_Absyn_Scope _tmp9D;int _tmp9E;struct _tuple10 _tmp9C=_tmp9B;_tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;{
# 297
struct Cyc_List_List*_tmp9F=f0->typs;
struct Cyc_List_List*_tmpA0=f1->typs;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA0)){
{const char*_tmp21F;void*_tmp21E[3];struct Cyc_String_pa_PrintArg_struct _tmp21D;struct Cyc_String_pa_PrintArg_struct _tmp21C;struct Cyc_String_pa_PrintArg_struct _tmp21B;(_tmp21B.tag=0,((_tmp21B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9A),((_tmp21C.tag=0,((_tmp21C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp21D.tag=0,((_tmp21D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp21E[0]=& _tmp21D,((_tmp21E[1]=& _tmp21C,((_tmp21E[2]=& _tmp21B,Cyc_Tcdecl_merr(loc,msg,((_tmp21F="%s %s, field %s: parameter number mismatch",_tag_dyneither(_tmp21F,sizeof(char),43))),_tag_dyneither(_tmp21E,sizeof(void*),3)))))))))))))))))));}
_tmp9E=0;}
# 304
for(0;_tmp9F != 0  && _tmpA0 != 0;(_tmp9F=_tmp9F->tl,_tmpA0=_tmpA0->tl)){
# 306
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp9F->hd)).f1,(*((struct _tuple15*)_tmpA0->hd)).f1)){
{const char*_tmp225;void*_tmp224[3];struct Cyc_String_pa_PrintArg_struct _tmp223;struct Cyc_String_pa_PrintArg_struct _tmp222;struct Cyc_String_pa_PrintArg_struct _tmp221;(_tmp221.tag=0,((_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9A),((_tmp222.tag=0,((_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp223.tag=0,((_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp224[0]=& _tmp223,((_tmp224[1]=& _tmp222,((_tmp224[2]=& _tmp221,Cyc_Tcdecl_merr(loc,msg,((_tmp225="%s %s, field %s: parameter qualifier",_tag_dyneither(_tmp225,sizeof(char),37))),_tag_dyneither(_tmp224,sizeof(void*),3)))))))))))))))))));}
_tmp9E=0;}{
# 310
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmpA0->hd)).f2);
# 312
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp9F->hd)).f2,subst_t1)){
{const char*_tmp22D;void*_tmp22C[5];struct Cyc_String_pa_PrintArg_struct _tmp22B;struct Cyc_String_pa_PrintArg_struct _tmp22A;struct Cyc_String_pa_PrintArg_struct _tmp229;struct Cyc_String_pa_PrintArg_struct _tmp228;struct Cyc_String_pa_PrintArg_struct _tmp227;(_tmp227.tag=0,((_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp228.tag=0,((_tmp228.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp9F->hd)).f2)),((_tmp229.tag=0,((_tmp229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9A),((_tmp22A.tag=0,((_tmp22A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp22B.tag=0,((_tmp22B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp22C[0]=& _tmp22B,((_tmp22C[1]=& _tmp22A,((_tmp22C[2]=& _tmp229,((_tmp22C[3]=& _tmp228,((_tmp22C[4]=& _tmp227,Cyc_Tcdecl_merr(loc,msg,((_tmp22D="%s %s, field %s: parameter type mismatch %s != %s",_tag_dyneither(_tmp22D,sizeof(char),50))),_tag_dyneither(_tmp22C,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp9E=0;}};}
# 320
if(!_tmp9E)return 0;
if(f0->sc != _tmp9D){
struct Cyc_Absyn_Datatypefield*_tmp22E;struct Cyc_Absyn_Datatypefield*_tmpB2=(_tmp22E=_cycalloc(sizeof(*_tmp22E)),((_tmp22E[0]=*f0,_tmp22E)));
_tmpB2->sc=_tmp9D;
return _tmpB2;}else{
return f0;}};};}
# 328
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 331
struct Cyc_Absyn_Tqual _tmpB9;void*_tmpBA;struct _tuple15*_tmpB8=x;_tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;{
struct _tuple15*_tmp22F;return(_tmp22F=_cycalloc(sizeof(*_tmp22F)),((_tmp22F->f1=_tmpB9,((_tmp22F->f2=Cyc_Tcutil_substitute(inst,_tmpBA),_tmp22F)))));};}
# 337
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 339
struct Cyc_Absyn_Datatypefield*_tmp230;struct Cyc_Absyn_Datatypefield*_tmpBC=(_tmp230=_cycalloc(sizeof(*_tmp230)),((_tmp230[0]=*f1,_tmp230)));
_tmpBC->typs=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);
# 342
return _tmpBC;}
# 349
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 358
struct Cyc_List_List**_tmp231;struct Cyc_List_List**f2sp=(_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231[0]=0,_tmp231)));
struct Cyc_List_List**_tmpBE=f2sp;
int cmp=-1;
# 362
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*_tmp232;struct Cyc_List_List*_tmpBF=(_tmp232=_cycalloc(sizeof(*_tmp232)),((_tmp232->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,((_tmp232->tl=0,_tmp232)))));
*_tmpBE=_tmpBF;
_tmpBE=& _tmpBF->tl;
f0s=f0s->tl;}
# 369
if(f0s == 0  || cmp > 0){
# 371
*incl=0;{
struct Cyc_List_List*_tmp233;struct Cyc_List_List*_tmpC1=(_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233->hd=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd),((_tmp233->tl=0,_tmp233)))));
*_tmpBE=_tmpC1;
_tmpBE=& _tmpC1->tl;};}else{
# 377
struct Cyc_Absyn_Datatypefield*_tmpC3=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpC3 != 0){
if(_tmpC3 != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmp234;struct Cyc_List_List*_tmpC4=(_tmp234=_cycalloc(sizeof(*_tmp234)),((_tmp234->hd=_tmpC3,((_tmp234->tl=0,_tmp234)))));
*_tmpBE=_tmpC4;
_tmpBE=& _tmpC4->tl;};}else{
# 384
*res=0;}
# 386
f0s=f0s->tl;}}
# 391
if(f1s != 0){
*incl=0;
*_tmpBE=f1s;}else{
# 395
*_tmpBE=f0s;}
# 398
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 402
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0){struct _tuple16 _tmp235;return(_tmp235.f1=0,((_tmp235.f2=0,_tmp235)));}
if(f->tl == 0){struct _tuple16 _tmp236;return(_tmp236.f1=f,((_tmp236.f2=0,_tmp236)));}{
struct _tuple16 _tmpC9=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCC;struct _tuple16 _tmpCA=_tmpC9;_tmpCB=_tmpCA.f1;_tmpCC=_tmpCA.f2;{
struct Cyc_List_List*_tmp23B;struct Cyc_List_List*_tmp23A;struct _tuple16 _tmp239;return(_tmp239.f1=((_tmp23A=_cycalloc(sizeof(*_tmp23A)),((_tmp23A->hd=f->hd,((_tmp23A->tl=_tmpCB,_tmp23A)))))),((_tmp239.f2=((_tmp23B=_cycalloc(sizeof(*_tmp23B)),((_tmp23B->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd,((_tmp23B->tl=_tmpCC,_tmp23B)))))),_tmp239)));};};}
# 411
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 414
struct _tuple16 _tmpD0=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD3;struct _tuple16 _tmpD1=_tmpD0;_tmpD2=_tmpD1.f1;_tmpD3=_tmpD1.f2;
if(_tmpD2 != 0  && _tmpD2->tl != 0)
_tmpD2=Cyc_Tcdecl_sort_xdatatype_fields(_tmpD2,res,v,loc,msg);
if(_tmpD3 != 0  && _tmpD3->tl != 0)
_tmpD3=Cyc_Tcdecl_sort_xdatatype_fields(_tmpD3,res,v,loc,msg);{
const char*_tmp23D;int*_tmp23C;return Cyc_Tcdecl_merge_xdatatype_fields(_tmpD2,_tmpD3,0,0,0,res,((_tmp23C=_cycalloc_atomic(sizeof(*_tmp23C)),((_tmp23C[0]=1,_tmp23C)))),(
(_tmp23D="@extensible datatype",_tag_dyneither(_tmp23D,sizeof(char),21))),*v,loc,msg);};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 423
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 425
struct _dyneither_ptr _tmpD6=Cyc_Absynpp_qvar2string(d0->name);
const char*_tmp23E;struct _dyneither_ptr t=(_tmp23E="datatype",_tag_dyneither(_tmp23E,sizeof(char),9));
int _tmpD7=1;
# 432
if(d0->is_extensible != d1->is_extensible){
{const char*_tmp244;void*_tmp243[3];struct Cyc_String_pa_PrintArg_struct _tmp242;struct Cyc_String_pa_PrintArg_struct _tmp241;struct Cyc_String_pa_PrintArg_struct _tmp240;(_tmp240.tag=0,((_tmp240.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcdecl_is_x2string(d1->is_extensible)),((_tmp241.tag=0,((_tmp241.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible)),((_tmp242.tag=0,((_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp243[0]=& _tmp242,((_tmp243[1]=& _tmp241,((_tmp243[2]=& _tmp240,Cyc_Tcdecl_merr(loc,msg,((_tmp244="expected %s to be a %s instead of a %s",_tag_dyneither(_tmp244,sizeof(char),39))),_tag_dyneither(_tmp243,sizeof(void*),3)))))))))))))))))));}
_tmpD7=0;}else{
# 437
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 441
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpD6,loc,msg))return 0;{
# 444
struct _tuple10 _tmpDD=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpD6,loc,msg,0);enum Cyc_Absyn_Scope _tmpDF;int _tmpE0;struct _tuple10 _tmpDE=_tmpDD;_tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;
if(!_tmpE0)_tmpD7=0;{
# 447
struct Cyc_Absyn_Datatypedecl*d2;
# 450
{struct _tuple17 _tmp245;struct _tuple17 _tmpE1=(_tmp245.f1=d0->fields,((_tmp245.f2=d1->fields,_tmp245)));struct _tuple17 _tmpE2=_tmpE1;struct Cyc_List_List*_tmpE3;struct Cyc_List_List*_tmpE4;_LL2A: if(_tmpE2.f2 != 0)goto _LL2C;_LL2B:
 d2=d0;goto _LL29;_LL2C: if(_tmpE2.f1 != 0)goto _LL2E;_LL2D:
 d2=d1;goto _LL29;_LL2E: if(_tmpE2.f1 == 0)goto _LL29;_tmpE3=(struct Cyc_List_List*)(_tmpE2.f1)->v;if(_tmpE2.f2 == 0)goto _LL29;_tmpE4=(struct Cyc_List_List*)(_tmpE2.f2)->v;_LL2F: {
# 464 "tcdecl.cyc"
struct Cyc_List_List*_tmpE5=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 467
if(d0->is_extensible){
# 471
int _tmpE6=1;
struct Cyc_List_List*_tmpE7=Cyc_Tcdecl_merge_xdatatype_fields(_tmpE3,_tmpE4,_tmpE5,d0->tvs,d1->tvs,& _tmpD7,& _tmpE6,t,_tmpD6,loc,msg);
# 475
if(_tmpE6)
d2=d0;else{
# 478
{struct Cyc_Absyn_Datatypedecl*_tmp246;d2=((_tmp246=_cycalloc(sizeof(*_tmp246)),((_tmp246[0]=*d0,_tmp246))));}
d2->sc=_tmpDF;{
struct Cyc_Core_Opt*_tmp247;d2->fields=((_tmp247=_cycalloc(sizeof(*_tmp247)),((_tmp247->v=_tmpE7,_tmp247))));};}}else{
# 484
for(0;_tmpE3 != 0  && _tmpE4 != 0;(_tmpE3=_tmpE3->tl,_tmpE4=_tmpE4->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpE3->hd,(struct Cyc_Absyn_Datatypefield*)_tmpE4->hd,_tmpE5,t,_tmpD6,msg);}
# 487
if(_tmpE3 != 0){
{const char*_tmp24D;void*_tmp24C[3];struct Cyc_String_pa_PrintArg_struct _tmp24B;struct Cyc_String_pa_PrintArg_struct _tmp24A;struct Cyc_String_pa_PrintArg_struct _tmp249;(_tmp249.tag=0,((_tmp249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpE3->hd)->name).f2),((_tmp24A.tag=0,((_tmp24A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp24B.tag=0,((_tmp24B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp24C[0]=& _tmp24B,((_tmp24C[1]=& _tmp24A,((_tmp24C[2]=& _tmp249,Cyc_Tcdecl_merr(loc,msg,((_tmp24D="%s %s is missing field %s",_tag_dyneither(_tmp24D,sizeof(char),26))),_tag_dyneither(_tmp24C,sizeof(void*),3)))))))))))))))))));}
_tmpD7=0;}
# 491
if(_tmpE4 != 0){
{const char*_tmp253;void*_tmp252[3];struct Cyc_String_pa_PrintArg_struct _tmp251;struct Cyc_String_pa_PrintArg_struct _tmp250;struct Cyc_String_pa_PrintArg_struct _tmp24F;(_tmp24F.tag=0,((_tmp24F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpE4->hd)->name).f2),((_tmp250.tag=0,((_tmp250.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp251.tag=0,((_tmp251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp252[0]=& _tmp251,((_tmp252[1]=& _tmp250,((_tmp252[2]=& _tmp24F,Cyc_Tcdecl_merr(loc,msg,((_tmp253="%s %s has extra field %s",_tag_dyneither(_tmp253,sizeof(char),25))),_tag_dyneither(_tmp252,sizeof(void*),3)))))))))))))))))));}
_tmpD7=0;}
# 495
d2=d0;}
# 497
goto _LL29;}_LL29:;}
# 500
if(!_tmpD7)return 0;
# 502
if(_tmpDF == d2->sc)
return d2;else{
# 505
{struct Cyc_Absyn_Datatypedecl*_tmp254;d2=((_tmp254=_cycalloc(sizeof(*_tmp254)),((_tmp254[0]=*d2,_tmp254))));}
d2->sc=_tmpDF;
return d2;}};};}
# 511
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 513
struct _dyneither_ptr _tmpF7=Cyc_Absynpp_qvar2string(d0->name);
int _tmpF8=1;
# 517
const char*_tmp255;struct _tuple10 _tmpF9=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp255="enum",_tag_dyneither(_tmp255,sizeof(char),5))),_tmpF7,loc,msg,1);enum Cyc_Absyn_Scope _tmpFB;int _tmpFC;struct _tuple10 _tmpFA=_tmpF9;_tmpFB=_tmpFA.f1;_tmpFC=_tmpFA.f2;
if(!_tmpFC)_tmpF8=0;{
# 520
struct Cyc_Absyn_Enumdecl*d2;
# 523
{struct _tuple17 _tmp256;struct _tuple17 _tmpFD=(_tmp256.f1=d0->fields,((_tmp256.f2=d1->fields,_tmp256)));struct _tuple17 _tmpFE=_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmp100;_LL31: if(_tmpFE.f2 != 0)goto _LL33;_LL32:
 d2=d0;goto _LL30;_LL33: if(_tmpFE.f1 != 0)goto _LL35;_LL34:
 d2=d1;goto _LL30;_LL35: if(_tmpFE.f1 == 0)goto _LL30;_tmpFF=(struct Cyc_List_List*)(_tmpFE.f1)->v;if(_tmpFE.f2 == 0)goto _LL30;_tmp100=(struct Cyc_List_List*)(_tmpFE.f2)->v;_LL36:
# 528
 for(0;_tmpFF != 0  && _tmp100 != 0;(_tmpFF=_tmpFF->tl,_tmp100=_tmp100->tl)){
struct Cyc_Absyn_Enumfield*_tmp101=(struct Cyc_Absyn_Enumfield*)_tmpFF->hd;struct _dyneither_ptr*_tmp103;struct Cyc_Absyn_Exp*_tmp104;unsigned int _tmp105;struct Cyc_Absyn_Enumfield*_tmp102=_tmp101;_tmp103=(_tmp102->name)->f2;_tmp104=_tmp102->tag;_tmp105=_tmp102->loc;{
struct Cyc_Absyn_Enumfield*_tmp106=(struct Cyc_Absyn_Enumfield*)_tmp100->hd;struct _dyneither_ptr*_tmp108;struct Cyc_Absyn_Exp*_tmp109;unsigned int _tmp10A;struct Cyc_Absyn_Enumfield*_tmp107=_tmp106;_tmp108=(_tmp107->name)->f2;_tmp109=_tmp107->tag;_tmp10A=_tmp107->loc;
# 533
if(Cyc_strptrcmp(_tmp103,_tmp108)!= 0){
{const char*_tmp25C;void*_tmp25B[3];struct Cyc_String_pa_PrintArg_struct _tmp25A;struct Cyc_String_pa_PrintArg_struct _tmp259;struct Cyc_String_pa_PrintArg_struct _tmp258;(_tmp258.tag=0,((_tmp258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp108),((_tmp259.tag=0,((_tmp259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp103),((_tmp25A.tag=0,((_tmp25A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF7),((_tmp25B[0]=& _tmp25A,((_tmp25B[1]=& _tmp259,((_tmp25B[2]=& _tmp258,Cyc_Tcdecl_merr(loc,msg,((_tmp25C="enum %s: field name mismatch %s != %s",_tag_dyneither(_tmp25C,sizeof(char),38))),_tag_dyneither(_tmp25B,sizeof(void*),3)))))))))))))))))));}
_tmpF8=0;}
# 538
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp104))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp109))){
{const char*_tmp261;void*_tmp260[2];struct Cyc_String_pa_PrintArg_struct _tmp25F;struct Cyc_String_pa_PrintArg_struct _tmp25E;(_tmp25E.tag=0,((_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp108),((_tmp25F.tag=0,((_tmp25F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF7),((_tmp260[0]=& _tmp25F,((_tmp260[1]=& _tmp25E,Cyc_Tcdecl_merr(loc,msg,((_tmp261="enum %s, field %s, value mismatch",_tag_dyneither(_tmp261,sizeof(char),34))),_tag_dyneither(_tmp260,sizeof(void*),2)))))))))))));}
_tmpF8=0;}};}
# 543
d2=d0;
goto _LL30;_LL30:;}
# 547
if(!_tmpF8)return 0;
# 549
if(d2->sc == _tmpFB)
return d2;else{
# 552
{struct Cyc_Absyn_Enumdecl*_tmp262;d2=((_tmp262=_cycalloc(sizeof(*_tmp262)),((_tmp262[0]=*d2,_tmp262))));}
d2->sc=_tmpFB;
return d2;}};}
# 558
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 562
int _tmp117=1;
# 565
struct _tuple10 _tmp118=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);enum Cyc_Absyn_Scope _tmp11A;int _tmp11B;struct _tuple10 _tmp119=_tmp118;_tmp11A=_tmp119.f1;_tmp11B=_tmp119.f2;
if(!_tmp11B)_tmp117=0;
# 569
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _dyneither_ptr _tmp11C=Cyc_Absynpp_typ2string(t0);
struct _dyneither_ptr _tmp11D=Cyc_Absynpp_typ2string(t1);
{const char*_tmp263;if((((Cyc_strlen((struct _dyneither_ptr)_tmp11C)+ Cyc_strlen((struct _dyneither_ptr)_tmp11D))+ Cyc_strlen((struct _dyneither_ptr)t))+ Cyc_strlen((struct _dyneither_ptr)v))+ 
Cyc_strlen(((_tmp263="  was declared  instead of ",_tag_dyneither(_tmp263,sizeof(char),28))))> 70){
const char*_tmp26A;void*_tmp269[4];struct Cyc_String_pa_PrintArg_struct _tmp268;struct Cyc_String_pa_PrintArg_struct _tmp267;struct Cyc_String_pa_PrintArg_struct _tmp266;struct Cyc_String_pa_PrintArg_struct _tmp265;(_tmp265.tag=0,((_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11C),((_tmp266.tag=0,((_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11D),((_tmp267.tag=0,((_tmp267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp268.tag=0,((_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp269[0]=& _tmp268,((_tmp269[1]=& _tmp267,((_tmp269[2]=& _tmp266,((_tmp269[3]=& _tmp265,Cyc_Tcdecl_merr(loc,msg,((_tmp26A="%s %s was declared \n\t%s\n instead of \n\t%s",_tag_dyneither(_tmp26A,sizeof(char),41))),_tag_dyneither(_tmp269,sizeof(void*),4)))))))))))))))))))))))));}else{
# 577
const char*_tmp271;void*_tmp270[4];struct Cyc_String_pa_PrintArg_struct _tmp26F;struct Cyc_String_pa_PrintArg_struct _tmp26E;struct Cyc_String_pa_PrintArg_struct _tmp26D;struct Cyc_String_pa_PrintArg_struct _tmp26C;(_tmp26C.tag=0,((_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11C),((_tmp26D.tag=0,((_tmp26D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11D),((_tmp26E.tag=0,((_tmp26E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp26F.tag=0,((_tmp26F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp270[0]=& _tmp26F,((_tmp270[1]=& _tmp26E,((_tmp270[2]=& _tmp26D,((_tmp270[3]=& _tmp26C,Cyc_Tcdecl_merr(loc,msg,((_tmp271="%s %s was declared %s instead of %s",_tag_dyneither(_tmp271,sizeof(char),36))),_tag_dyneither(_tmp270,sizeof(void*),4)))))))))))))))))))))))));}}
# 579
Cyc_Tcutil_explain_failure();
_tmp117=0;}
# 584
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
{const char*_tmp276;void*_tmp275[2];struct Cyc_String_pa_PrintArg_struct _tmp274;struct Cyc_String_pa_PrintArg_struct _tmp273;(_tmp273.tag=0,((_tmp273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp274.tag=0,((_tmp274.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp275[0]=& _tmp274,((_tmp275[1]=& _tmp273,Cyc_Tcdecl_merr(loc,msg,((_tmp276="%s %s has different type qualifiers",_tag_dyneither(_tmp276,sizeof(char),36))),_tag_dyneither(_tmp275,sizeof(void*),2)))))))))))));}
_tmp117=0;}
# 590
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp27B;void*_tmp27A[2];struct Cyc_String_pa_PrintArg_struct _tmp279;struct Cyc_String_pa_PrintArg_struct _tmp278;(_tmp278.tag=0,((_tmp278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp279.tag=0,((_tmp279.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp27A[0]=& _tmp279,((_tmp27A[1]=& _tmp278,Cyc_Tcdecl_merr(loc,msg,((_tmp27B="%s %s has different attributes",_tag_dyneither(_tmp27B,sizeof(char),31))),_tag_dyneither(_tmp27A,sizeof(void*),2)))))))))))));}
{const char*_tmp27E;void*_tmp27D;(_tmp27D=0,Cyc_fprintf(Cyc_stderr,((_tmp27E="\tprevious attributes: ",_tag_dyneither(_tmp27E,sizeof(char),23))),_tag_dyneither(_tmp27D,sizeof(void*),0)));}
for(0;atts0 != 0;atts0=atts0->tl){
const char*_tmp282;void*_tmp281[1];struct Cyc_String_pa_PrintArg_struct _tmp280;(_tmp280.tag=0,((_tmp280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd)),((_tmp281[0]=& _tmp280,Cyc_fprintf(Cyc_stderr,((_tmp282="%s ",_tag_dyneither(_tmp282,sizeof(char),4))),_tag_dyneither(_tmp281,sizeof(void*),1)))))));}
{const char*_tmp285;void*_tmp284;(_tmp284=0,Cyc_fprintf(Cyc_stderr,((_tmp285="\n\tcurrent attributes: ",_tag_dyneither(_tmp285,sizeof(char),23))),_tag_dyneither(_tmp284,sizeof(void*),0)));}
for(0;atts1 != 0;atts1=atts1->tl){
const char*_tmp289;void*_tmp288[1];struct Cyc_String_pa_PrintArg_struct _tmp287;(_tmp287.tag=0,((_tmp287.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd)),((_tmp288[0]=& _tmp287,Cyc_fprintf(Cyc_stderr,((_tmp289="%s ",_tag_dyneither(_tmp289,sizeof(char),4))),_tag_dyneither(_tmp288,sizeof(void*),1)))))));}
{const char*_tmp28C;void*_tmp28B;(_tmp28B=0,Cyc_fprintf(Cyc_stderr,((_tmp28C="\n",_tag_dyneither(_tmp28C,sizeof(char),2))),_tag_dyneither(_tmp28B,sizeof(void*),0)));}
_tmp117=0;}{
# 602
struct _tuple10 _tmp28D;return(_tmp28D.f1=_tmp11A,((_tmp28D.f2=_tmp117,_tmp28D)));};}
# 605
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 607
struct _dyneither_ptr _tmp140=Cyc_Absynpp_qvar2string(d0->name);
# 610
const char*_tmp28E;struct _tuple10 _tmp141=
Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,(
# 613
(_tmp28E="variable",_tag_dyneither(_tmp28E,sizeof(char),9))),_tmp140,loc,msg);
# 610
enum Cyc_Absyn_Scope _tmp143;int _tmp144;struct _tuple10 _tmp142=_tmp141;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;
# 614
if(!_tmp144)return 0;
# 616
if(d0->sc == _tmp143)
return d0;else{
# 619
struct Cyc_Absyn_Vardecl*_tmp28F;struct Cyc_Absyn_Vardecl*_tmp145=(_tmp28F=_cycalloc(sizeof(*_tmp28F)),((_tmp28F[0]=*d0,_tmp28F)));
_tmp145->sc=_tmp143;
return _tmp145;}}
# 625
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 627
struct _dyneither_ptr _tmp148=Cyc_Absynpp_qvar2string(d0->name);
# 630
{const char*_tmp290;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp290="typedef",_tag_dyneither(_tmp290,sizeof(char),8))),_tmp148,loc,msg))return 0;}{
# 633
struct Cyc_List_List*_tmp14A=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 636
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp14A,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
{const char*_tmp296;void*_tmp295[3];struct Cyc_String_pa_PrintArg_struct _tmp294;struct Cyc_String_pa_PrintArg_struct _tmp293;struct Cyc_String_pa_PrintArg_struct _tmp292;(_tmp292.tag=0,((_tmp292.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn))),((_tmp293.tag=0,((_tmp293.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1)),((_tmp294.tag=0,((_tmp294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp148),((_tmp295[0]=& _tmp294,((_tmp295[1]=& _tmp293,((_tmp295[2]=& _tmp292,Cyc_Tcdecl_merr(loc,msg,((_tmp296="typedef %s does not refer to the same type: %s != %s",_tag_dyneither(_tmp296,sizeof(char),53))),_tag_dyneither(_tmp295,sizeof(void*),3)))))))))))))))))));}
return 0;}}
# 644
return d0;};}
# 647
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp297;struct _tuple14 _tmp150=(_tmp297.f1=b0,((_tmp297.f2=b1,_tmp297)));struct _tuple14 _tmp151=_tmp150;struct Cyc_Absyn_Vardecl*_tmp155;struct Cyc_Absyn_Vardecl*_tmp157;struct Cyc_Absyn_Vardecl*_tmp159;struct Cyc_Absyn_Fndecl*_tmp15B;struct Cyc_Absyn_Fndecl*_tmp15E;struct Cyc_Absyn_Fndecl*_tmp160;struct Cyc_Absyn_Vardecl*_tmp162;_LL38:{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp152=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp151.f1;if(_tmp152->tag != 0)goto _LL3A;}{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp153=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp151.f2;if(_tmp153->tag != 0)goto _LL3A;};_LL39:
 return(void*)& Cyc_Absyn_Unresolved_b_val;_LL3A:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp154=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp151.f1;if(_tmp154->tag != 1)goto _LL3C;else{_tmp155=_tmp154->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp156=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp151.f2;if(_tmp156->tag != 1)goto _LL3C;else{_tmp157=_tmp156->f1;}};_LL3B: {
# 651
struct Cyc_Absyn_Vardecl*_tmp163=Cyc_Tcdecl_merge_vardecl(_tmp155,_tmp157,loc,msg);
if(_tmp163 == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
if(_tmp163 == _tmp155)return b0;
if(_tmp163 == _tmp157)return b1;{
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp29A;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp299;return(void*)((_tmp299=_cycalloc(sizeof(*_tmp299)),((_tmp299[0]=((_tmp29A.tag=1,((_tmp29A.f1=_tmp163,_tmp29A)))),_tmp299))));};}_LL3C:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp158=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp151.f1;if(_tmp158->tag != 1)goto _LL3E;else{_tmp159=_tmp158->f1;}}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp15A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp151.f2;if(_tmp15A->tag != 2)goto _LL3E;else{_tmp15B=_tmp15A->f1;}};_LL3D: {
# 657
const char*_tmp29B;struct _tuple10 _tmp166=Cyc_Tcdecl_check_var_or_fn_decl(_tmp159->sc,_tmp159->type,_tmp159->tq,_tmp159->attributes,_tmp15B->sc,(void*)_check_null(_tmp15B->cached_typ),
Cyc_Absyn_empty_tqual(0),_tmp15B->attributes,(
(_tmp29B="function",_tag_dyneither(_tmp29B,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp159->name),loc,msg);
# 657
int _tmp168;struct _tuple10 _tmp167=_tmp166;_tmp168=_tmp167.f2;
# 661
return _tmp168?b1:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL3E:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp15C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp151.f1;if(_tmp15C->tag != 2)goto _LL40;}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp15D=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp151.f2;if(_tmp15D->tag != 2)goto _LL40;else{_tmp15E=_tmp15D->f1;}};_LL3F:
# 663
{const char*_tmp29F;void*_tmp29E[1];struct Cyc_String_pa_PrintArg_struct _tmp29D;(_tmp29D.tag=0,((_tmp29D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name)),((_tmp29E[0]=& _tmp29D,Cyc_Tcdecl_merr(loc,msg,((_tmp29F="redefinition of function %s",_tag_dyneither(_tmp29F,sizeof(char),28))),_tag_dyneither(_tmp29E,sizeof(void*),1)))))));}
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL40:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp15F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp151.f1;if(_tmp15F->tag != 2)goto _LL42;else{_tmp160=_tmp15F->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp161=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp151.f2;if(_tmp161->tag != 1)goto _LL42;else{_tmp162=_tmp161->f1;}};_LL41: {
# 666
const char*_tmp2A0;struct _tuple10 _tmp16D=Cyc_Tcdecl_check_var_or_fn_decl(_tmp160->sc,(void*)_check_null(_tmp160->cached_typ),Cyc_Absyn_empty_tqual(0),_tmp160->attributes,_tmp162->sc,_tmp162->type,_tmp162->tq,_tmp162->attributes,(
# 668
(_tmp2A0="variable",_tag_dyneither(_tmp2A0,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp160->name),loc,msg);
# 666
int _tmp16F;struct _tuple10 _tmp16E=_tmp16D;_tmp16F=_tmp16E.f2;
# 670
return _tmp16F?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL42:;_LL43: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp2A6;const char*_tmp2A5;struct Cyc_Core_Invalid_argument_exn_struct*_tmp2A4;(int)_throw((void*)((_tmp2A4=_cycalloc(sizeof(*_tmp2A4)),((_tmp2A4[0]=((_tmp2A6.tag=Cyc_Core_Invalid_argument,((_tmp2A6.f1=((_tmp2A5="Tcdecl::merge_binding",_tag_dyneither(_tmp2A5,sizeof(char),22))),_tmp2A6)))),_tmp2A4)))));}_LL37:;}
# 675
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 678
struct Cyc_Absyn_Datatypedecl*_tmp176;struct Cyc_Absyn_Datatypefield*_tmp177;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp175=d0;_tmp176=_tmp175->base;_tmp177=_tmp175->field;{
struct Cyc_Absyn_Datatypedecl*_tmp179;struct Cyc_Absyn_Datatypefield*_tmp17A;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp178=d1;_tmp179=_tmp178->base;_tmp17A=_tmp178->field;{
struct _dyneither_ptr _tmp17B=Cyc_Absynpp_qvar2string(_tmp177->name);
# 683
if(Cyc_Absyn_qvar_cmp(_tmp176->name,_tmp179->name)!= 0){
{const char*_tmp2AC;void*_tmp2AB[3];struct Cyc_String_pa_PrintArg_struct _tmp2AA;struct Cyc_String_pa_PrintArg_struct _tmp2A9;struct Cyc_String_pa_PrintArg_struct _tmp2A8;(_tmp2A8.tag=0,((_tmp2A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp17B),((_tmp2A9.tag=0,((_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp179->name)),((_tmp2AA.tag=0,((_tmp2AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp176->name)),((_tmp2AB[0]=& _tmp2AA,((_tmp2AB[1]=& _tmp2A9,((_tmp2AB[2]=& _tmp2A8,Cyc_Tcdecl_merr(loc,msg,((_tmp2AC="@extensible datatype %s and %s have both a field named %s",_tag_dyneither(_tmp2AC,sizeof(char),58))),_tag_dyneither(_tmp2AB,sizeof(void*),3)))))))))))))))))));}
return 0;}
# 690
{const char*_tmp2AD;if(!Cyc_Tcdecl_check_tvs(_tmp176->tvs,_tmp179->tvs,((_tmp2AD="@extensible datatype",_tag_dyneither(_tmp2AD,sizeof(char),21))),Cyc_Absynpp_qvar2string(_tmp176->name),loc,msg))
return 0;}{
# 693
struct Cyc_List_List*_tmp182=Cyc_Tcdecl_build_tvs_map(_tmp176->tvs,_tmp179->tvs);
# 696
const char*_tmp2AE;struct Cyc_Absyn_Datatypefield*_tmp183=Cyc_Tcdecl_merge_datatypefield(_tmp177,_tmp17A,_tmp182,((_tmp2AE="@extensible datatype field",_tag_dyneither(_tmp2AE,sizeof(char),27))),_tmp17B,msg);
if(_tmp183 == 0)return 0;
if(_tmp183 == _tmp177)return d0;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp2AF;return(_tmp2AF=_cycalloc(sizeof(*_tmp2AF)),((_tmp2AF->base=_tmp176,((_tmp2AF->field=_tmp183,_tmp2AF)))));};};};};}
