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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
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
# 279 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 54
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_SlowDict_Dict;
# 40 "slowdict.h"
typedef struct Cyc_SlowDict_Dict*Cyc_SlowDict_dict_t;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Relations_Reln;
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 444 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 527
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 553
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 560
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 578
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 770 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 937
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 960
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 980
extern void*Cyc_Absyn_empty_effect;
# 982
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1015
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1102
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
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
# 86
extern struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val;
extern struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val;
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 101
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 103
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 105
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 119 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 124
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 127
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 129
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 136
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 139
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 142
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 144
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 150
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 152
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 156
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 160
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 167
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 178
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 184
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 189
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 193
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 197
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 200
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 203
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 211
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 213
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 215
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 217
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 219
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 223
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 226
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 229
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 232
extern int Cyc_Tcenv_warn_override;
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 53
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 148
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 182
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 186
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 242 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 311
int Cyc_Tcutil_new_tvar_id();
# 313
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 87 "tcenv.cyc"
int Cyc_Tcenv_warn_override=0;char Cyc_Tcenv_Env_error[10]="Env_error";
# 90
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 92
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 95
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 148
struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val={0};
struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val={1};
struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val={2};
# 153
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;const struct _tuple10*fallthru_clause;void*next_stmt;int try_depth;};
# 167
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 195 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 230 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 233
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 237
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 242
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
struct Cyc_Tcenv_Genv*_tmp3D9;return(_tmp3D9=_region_malloc(r,sizeof(*_tmp3D9)),((_tmp3D9->grgn=r,((_tmp3D9->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp3D9->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3D9->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3D9->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3D9->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3D9->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3D9->availables=0,_tmp3D9)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 253
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 255
{struct Cyc_Core_Opt*_tmp3DA;Cyc_Tcutil_empty_var_set=((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3DA))));}{
# 257
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp3DB;_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB[0]=Cyc_Absyn_exn_tud,_tmp3DB)))));}
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3E1;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3E0;struct _tuple12*_tmp3DF;_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,(
(_tmp3DF=_region_malloc(r,sizeof(*_tmp3DF)),((_tmp3DF->f1=(void*)(
(_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E0.tag=2,((_tmp3E0.f1=Cyc_Absyn_exn_tud,((_tmp3E0.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd,_tmp3E0)))))),_tmp3E1)))),((_tmp3DF->f2=1,_tmp3DF)))))));}}{
# 264
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
struct Cyc_Tcenv_Tenv*_tmp3E2;return(_tmp3E2=_region_malloc(r,sizeof(*_tmp3E2)),((_tmp3E2->ns=0,((_tmp3E2->ae=ae,((_tmp3E2->le=0,((_tmp3E2->allow_valueof=0,((_tmp3E2->in_extern_c_include=0,_tmp3E2)))))))))));};};}
# 270
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 272
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 276
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp3E3;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3E3="outer_namespace",_tag_dyneither(_tmp3E3,sizeof(char),16))));}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 282
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 289
if(n2 != 0)return 0;
return 1;}
# 295
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 300
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpF;_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){
lookup(ge2,v,0);{
const char*_tmp3E7;void*_tmp3E6[1];struct Cyc_String_pa_PrintArg_struct _tmp3E5;(_tmp3E5.tag=0,((_tmp3E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3E6[0]=& _tmp3E5,Cyc_Tcutil_terr(loc,((_tmp3E7="%s is ambiguous",_tag_dyneither(_tmp3E7,sizeof(char),16))),_tag_dyneither(_tmp3E6,sizeof(void*),1)))))));};
# 304
;_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp16=_tmp10;void*_tmp18;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp17=(struct Cyc_Dict_Absent_exn_struct*)_tmp16;if(_tmp17->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 306
 goto _LL0;_LL3: _tmp18=_tmp16;_LL4:(void)_rethrow(_tmp18);_LL0:;}};}}
# 308
return;}
# 311
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 316
{struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1C=result;_npop_handler(0);return _tmp1C;};}
# 317
;_pop_handler();}else{void*_tmp1A=(void*)_exn_thrown;void*_tmp1E=_tmp1A;void*_tmp20;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp1F=(struct Cyc_Dict_Absent_exn_struct*)_tmp1E;if(_tmp1F->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 320
 goto _LL5;_LL8: _tmp20=_tmp1E;_LL9:(void)_rethrow(_tmp20);_LL5:;}};}
# 322
{struct Cyc_List_List*_tmp21=ge->availables;for(0;_tmp21 != 0;_tmp21=_tmp21->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp21->hd);
struct _handler_cons _tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;if(!_tmp24){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp21->hd,_tmp21->tl);{
void*_tmp25=result;_npop_handler(0);return _tmp25;};}
# 325
;_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp27=_tmp23;void*_tmp29;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp28=(struct Cyc_Dict_Absent_exn_struct*)_tmp27;if(_tmp28->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 328
 goto _LLA;_LLD: _tmp29=_tmp27;_LLE:(void)_rethrow(_tmp29);_LLA:;}};}}
# 330
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3EA;struct Cyc_Dict_Absent_exn_struct*_tmp3E9;(int)_throw((void*)((_tmp3E9=_cycalloc_atomic(sizeof(*_tmp3E9)),((_tmp3E9[0]=((_tmp3EA.tag=Cyc_Dict_Absent,_tmp3EA)),_tmp3E9)))));}
# 333
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 337
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2C,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 344
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp2E;struct _dyneither_ptr*_tmp2F;struct _tuple0*_tmp2D=q;_tmp2E=_tmp2D->f1;_tmp2F=_tmp2D->f2;{
union Cyc_Absyn_Nmspace _tmp30=_tmp2E;struct _dyneither_ptr*_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_List_List*_tmp33;struct Cyc_List_List*_tmp34;_LL10: if((_tmp30.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp30.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL14;_LL13:
# 349
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp2F,is_use);_LL14: if((_tmp30.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp30.Rel_n).val == 0)goto _LL16;_tmp31=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp30.Rel_n).val)->hd;_tmp32=((struct Cyc_List_List*)(_tmp30.Rel_n).val)->tl;_LL15: {
# 351
struct Cyc_Tcenv_Genv*_tmp35=Cyc_Tcenv_lookup_namespace(te,loc,_tmp31,_tmp32);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp35,_tmp2F,is_use);}_LL16: if((_tmp30.C_n).tag != 3)goto _LL18;_tmp33=(struct Cyc_List_List*)(_tmp30.C_n).val;_LL17:
 _tmp34=_tmp33;goto _LL19;_LL18: if((_tmp30.Abs_n).tag != 2)goto _LLF;_tmp34=(struct Cyc_List_List*)(_tmp30.Abs_n).val;_LL19:
# 355
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp34),_tmp2F,is_use);_LLF:;};}
# 360
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 362
struct Cyc_List_List*_tmp36=te->ns;
struct _RegionHandle _tmp37=_new_region("temp");struct _RegionHandle*temp=& _tmp37;_push_region(temp);
{struct Cyc_List_List*_tmp38=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp39=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp36);
struct Cyc_List_List*_tmp3A=_tmp39->availables;
struct Cyc_Set_Set*_tmp3B=_tmp39->namespaces;
# 370
{struct Cyc_List_List*_tmp3C=_tmp3A;for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){
struct Cyc_Set_Set*_tmp3D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp3C->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3D,n)){
struct Cyc_List_List*_tmp3ED;struct Cyc_List_List*_tmp3EC;_tmp38=((_tmp3EC=_region_malloc(temp,sizeof(*_tmp3EC)),((_tmp3EC->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED->hd=n,((_tmp3ED->tl=ns,_tmp3ED))))))),((_tmp3EC->tl=_tmp38,_tmp3EC))))));}}}
# 377
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3B,n)){
struct Cyc_List_List*_tmp3F0;struct Cyc_List_List*_tmp3EF;_tmp38=((_tmp3EF=_region_malloc(temp,sizeof(*_tmp3EF)),((_tmp3EF->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=n,((_tmp3F0->tl=ns,_tmp3F0))))))),((_tmp3EF->tl=_tmp38,_tmp3EF))))));}
# 383
if(_tmp38 != 0){
if(_tmp38->tl != 0){
const char*_tmp3F4;void*_tmp3F3[1];struct Cyc_String_pa_PrintArg_struct _tmp3F2;(_tmp3F2.tag=0,((_tmp3F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp3F3[0]=& _tmp3F2,Cyc_Tcutil_terr(loc,((_tmp3F4="%s is ambiguous",_tag_dyneither(_tmp3F4,sizeof(char),16))),_tag_dyneither(_tmp3F3,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp45=(struct Cyc_List_List*)_tmp38->hd;_npop_handler(0);return _tmp45;};}
# 388
if(_tmp36 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3F7;struct Cyc_Dict_Absent_exn_struct*_tmp3F6;(int)_throw((void*)((_tmp3F6=_cycalloc_atomic(sizeof(*_tmp3F6)),((_tmp3F6[0]=((_tmp3F7.tag=Cyc_Dict_Absent,_tmp3F7)),_tmp3F6)))));}
# 391
_tmp36=Cyc_Tcenv_outer_namespace(_tmp36);}}
# 364
;_pop_region(temp);}
# 399
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp48=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp48,v);}
# 403
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4A;struct _dyneither_ptr*_tmp4B;struct _tuple0*_tmp49=q;_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;{
union Cyc_Absyn_Nmspace _tmp4C=_tmp4A;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;struct _dyneither_ptr*_tmp4F;struct Cyc_List_List*_tmp50;_LL1B: if((_tmp4C.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4C.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val != 0)goto _LL1F;_LL1E:
# 408
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4B,0);_LL1F: if((_tmp4C.C_n).tag != 3)goto _LL21;_tmp4D=(struct Cyc_List_List*)(_tmp4C.C_n).val;_LL20:
 _tmp4E=_tmp4D;goto _LL22;_LL21: if((_tmp4C.Abs_n).tag != 2)goto _LL23;_tmp4E=(struct Cyc_List_List*)(_tmp4C.Abs_n).val;_LL22: {
# 411
struct Cyc_Dict_Dict _tmp51=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4E))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp51,_tmp4B);}_LL23: if((_tmp4C.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val == 0)goto _LL1A;_tmp4F=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->hd;_tmp50=((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->tl;_LL24: {
# 414
struct Cyc_Dict_Dict _tmp52=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp4F,_tmp50))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp52,_tmp4B);}_LL1A:;};}
# 419
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp53=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp53,v);}
# 423
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp55;struct _dyneither_ptr*_tmp56;struct _tuple0*_tmp54=q;_tmp55=_tmp54->f1;_tmp56=_tmp54->f2;{
union Cyc_Absyn_Nmspace _tmp57=_tmp55;struct _dyneither_ptr*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp5B;_LL26: if((_tmp57.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp57.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp57.Rel_n).val != 0)goto _LL2A;_LL29:
# 428
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp56,0);_LL2A: if((_tmp57.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp57.Rel_n).val == 0)goto _LL2C;_tmp58=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp57.Rel_n).val)->hd;_tmp59=((struct Cyc_List_List*)(_tmp57.Rel_n).val)->tl;_LL2B: {
# 430
struct Cyc_Dict_Dict _tmp5C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp58,_tmp59))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5C,_tmp56);}_LL2C: if((_tmp57.C_n).tag != 3)goto _LL2E;_tmp5A=(struct Cyc_List_List*)(_tmp57.C_n).val;_LL2D:
 _tmp5B=_tmp5A;goto _LL2F;_LL2E: if((_tmp57.Abs_n).tag != 2)goto _LL25;_tmp5B=(struct Cyc_List_List*)(_tmp57.Abs_n).val;_LL2F: {
# 434
struct Cyc_Dict_Dict _tmp5D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5B))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5D,_tmp56);}_LL25:;};}
# 439
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 444
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp60;struct _tuple0*_tmp5E=q;_tmp5F=_tmp5E->f1;_tmp60=_tmp5E->f2;{
union Cyc_Absyn_Nmspace _tmp61=_tmp5F;struct _dyneither_ptr*_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*_tmp65;_LL31: if((_tmp61.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp61.Rel_n).val != 0)goto _LL33;_LL32: {
# 449
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){{struct Cyc_Absyn_Datatypedecl***_tmp3F8;struct Cyc_Absyn_Datatypedecl***_tmp6A=(_tmp3F8=_region_malloc(r,sizeof(*_tmp3F8)),((_tmp3F8[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp60,0),_tmp3F8)));_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;void*_tmp6E;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp6D=(struct Cyc_Dict_Absent_exn_struct*)_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp6E=_tmp6C;_LL3F:(void)_rethrow(_tmp6E);_LL3B:;}};}_LL33: if((_tmp61.Loc_n).tag != 4)goto _LL35;_LL34:
# 452
{const char*_tmp3FB;void*_tmp3FA;(_tmp3FA=0,Cyc_Tcutil_terr(loc,((_tmp3FB="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp3FB,sizeof(char),33))),_tag_dyneither(_tmp3FA,sizeof(void*),0)));}
return 0;_LL35: if((_tmp61.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp61.Rel_n).val == 0)goto _LL37;_tmp62=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp61.Rel_n).val)->hd;_tmp63=((struct Cyc_List_List*)(_tmp61.Rel_n).val)->tl;_LL36: {
# 459
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp62,_tmp63);;_pop_handler();}else{void*_tmp72=(void*)_exn_thrown;void*_tmp75=_tmp72;void*_tmp77;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp76=(struct Cyc_Dict_Absent_exn_struct*)_tmp75;if(_tmp76->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 463
{const char*_tmp3FE;void*_tmp3FD;(_tmp3FD=0,Cyc_Tcutil_terr(loc,((_tmp3FE="bad qualified name for @extensible datatype",_tag_dyneither(_tmp3FE,sizeof(char),44))),_tag_dyneither(_tmp3FD,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp401;struct Cyc_Dict_Absent_exn_struct*_tmp400;(int)_throw((void*)((_tmp400=_cycalloc_atomic(sizeof(*_tmp400)),((_tmp400[0]=((_tmp401.tag=Cyc_Dict_Absent,_tmp401)),_tmp400)))));};_LL43: _tmp77=_tmp75;_LL44:(void)_rethrow(_tmp77);_LL40:;}};}{
# 466
struct Cyc_Dict_Dict _tmp7C=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp402;return(_tmp402=_region_malloc(r,sizeof(*_tmp402)),((_tmp402[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp60),_tmp402)));};}_LL37: if((_tmp61.C_n).tag != 3)goto _LL39;_tmp64=(struct Cyc_List_List*)(_tmp61.C_n).val;_LL38:
 _tmp65=_tmp64;goto _LL3A;_LL39: if((_tmp61.Abs_n).tag != 2)goto _LL30;_tmp65=(struct Cyc_List_List*)(_tmp61.Abs_n).val;_LL3A: {
# 470
struct Cyc_Dict_Dict _tmp7E=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp403;return(_tmp403=_region_malloc(r,sizeof(*_tmp403)),((_tmp403[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,_tmp60),_tmp403)));}_LL30:;};}
# 475
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp80=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp80,v);}
# 479
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp82;struct _dyneither_ptr*_tmp83;struct _tuple0*_tmp81=q;_tmp82=_tmp81->f1;_tmp83=_tmp81->f2;{
union Cyc_Absyn_Nmspace _tmp84=_tmp82;struct _dyneither_ptr*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp88;_LL46: if((_tmp84.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp84.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp84.Rel_n).val != 0)goto _LL4A;_LL49:
# 484
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp83,0);_LL4A: if((_tmp84.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp84.Rel_n).val == 0)goto _LL4C;_tmp85=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp84.Rel_n).val)->hd;_tmp86=((struct Cyc_List_List*)(_tmp84.Rel_n).val)->tl;_LL4B: {
# 486
struct Cyc_Dict_Dict _tmp89=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp85,_tmp86))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp89,_tmp83);}_LL4C: if((_tmp84.C_n).tag != 3)goto _LL4E;_tmp87=(struct Cyc_List_List*)(_tmp84.C_n).val;_LL4D:
 _tmp88=_tmp87;goto _LL4F;_LL4E: if((_tmp84.Abs_n).tag != 2)goto _LL45;_tmp88=(struct Cyc_List_List*)(_tmp84.Abs_n).val;_LL4F: {
# 490
struct Cyc_Dict_Dict _tmp8A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp88))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8A,_tmp83);}_LL45:;};}
# 495
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8B=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,v);}
# 499
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp8D;struct _dyneither_ptr*_tmp8E;struct _tuple0*_tmp8C=q;_tmp8D=_tmp8C->f1;_tmp8E=_tmp8C->f2;{
union Cyc_Absyn_Nmspace _tmp8F=_tmp8D;struct _dyneither_ptr*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;_LL51: if((_tmp8F.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp8F.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val != 0)goto _LL55;_LL54:
# 504
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8E,0);_LL55: if((_tmp8F.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val == 0)goto _LL57;_tmp90=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->hd;_tmp91=((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->tl;_LL56: {
# 506
struct Cyc_Dict_Dict _tmp94=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp90,_tmp91))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp94,_tmp8E);}_LL57: if((_tmp8F.C_n).tag != 3)goto _LL59;_tmp92=(struct Cyc_List_List*)(_tmp8F.C_n).val;_LL58:
 _tmp93=_tmp92;goto _LL5A;_LL59: if((_tmp8F.Abs_n).tag != 2)goto _LL50;_tmp93=(struct Cyc_List_List*)(_tmp8F.Abs_n).val;_LL5A: {
# 510
struct Cyc_Dict_Dict _tmp95=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp93))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp95,_tmp8E);}_LL50:;};}
# 517
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp96=te->le;
if(_tmp96 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp96;}
# 523
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 527
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp97=te->le;
if(_tmp97 != 0){
struct Cyc_Tcenv_Fenv*_tmp98=_tmp97;struct _RegionHandle*_tmp9A;struct Cyc_Tcenv_Fenv*_tmp99=_tmp98;_tmp9A=_tmp99->fnrgn;
return _tmp9A;}
# 533
return Cyc_Tcenv_coerce_heap_region();}
# 536
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp404;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp404="put_fenv",_tag_dyneither(_tmp404,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp405;return(_tmp405=_region_malloc(l,sizeof(*_tmp405)),((_tmp405->ns=te->ns,((_tmp405->ae=te->ae,((_tmp405->le=fe,((_tmp405->allow_valueof=te->allow_valueof,((_tmp405->in_extern_c_include=te->in_extern_c_include,_tmp405)))))))))));};}
# 541
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp406;return(_tmp406=_region_malloc(l,sizeof(*_tmp406)),((_tmp406->ns=te->ns,((_tmp406->ae=te->ae,((_tmp406->le=0,((_tmp406->allow_valueof=te->allow_valueof,((_tmp406->in_extern_c_include=te->in_extern_c_include,_tmp406)))))))))));}
# 545
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 547
struct Cyc_Tcenv_Fenv _tmp9E=*f;struct Cyc_Tcenv_SharedFenv*_tmpA0;struct Cyc_List_List*_tmpA1;struct Cyc_RgnOrder_RgnPO*_tmpA2;const struct Cyc_Tcenv_Bindings*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Tcenv_CtrlEnv*_tmpA5;void*_tmpA6;void*_tmpA7;struct Cyc_Tcenv_FenvFlags _tmpA8;struct _RegionHandle*_tmpA9;struct Cyc_Tcenv_Fenv _tmp9F=_tmp9E;_tmpA0=_tmp9F.shared;_tmpA1=_tmp9F.type_vars;_tmpA2=_tmp9F.region_order;_tmpA3=_tmp9F.locals;_tmpA4=_tmp9F.encloser;_tmpA5=_tmp9F.ctrl_env;_tmpA6=_tmp9F.capability;_tmpA7=_tmp9F.curr_rgn;_tmpA8=_tmp9F.flags;_tmpA9=_tmp9F.fnrgn;{
# 549
struct Cyc_Tcenv_Fenv*_tmp407;return(_tmp407=_region_malloc(l,sizeof(*_tmp407)),((_tmp407->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA0,((_tmp407->type_vars=(struct Cyc_List_List*)_tmpA1,((_tmp407->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA2,((_tmp407->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA3,((_tmp407->encloser=(struct Cyc_Absyn_Stmt*)_tmpA4,((_tmp407->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA5,((_tmp407->capability=(void*)_tmpA6,((_tmp407->curr_rgn=(void*)_tmpA7,((_tmp407->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA8,((_tmp407->fnrgn=(struct _RegionHandle*)l,_tmp407)))))))))))))))))))));};}
# 553
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 555
struct Cyc_Tcenv_Fenv _tmpAB=*f;struct Cyc_Tcenv_SharedFenv*_tmpAD;struct Cyc_List_List*_tmpAE;struct Cyc_RgnOrder_RgnPO*_tmpAF;const struct Cyc_Tcenv_Bindings*_tmpB0;struct Cyc_Absyn_Stmt*_tmpB1;struct Cyc_Tcenv_CtrlEnv*_tmpB2;void*_tmpB3;void*_tmpB4;struct Cyc_Tcenv_FenvFlags _tmpB5;struct Cyc_Tcenv_Fenv _tmpAC=_tmpAB;_tmpAD=_tmpAC.shared;_tmpAE=_tmpAC.type_vars;_tmpAF=_tmpAC.region_order;_tmpB0=_tmpAC.locals;_tmpB1=_tmpAC.encloser;_tmpB2=_tmpAC.ctrl_env;_tmpB3=_tmpAC.capability;_tmpB4=_tmpAC.curr_rgn;_tmpB5=_tmpAC.flags;{
# 558
struct Cyc_Tcenv_CtrlEnv _tmpB6=*_tmpB2;struct _RegionHandle*_tmpB8;void*_tmpB9;void*_tmpBA;const struct _tuple10*_tmpBB;void*_tmpBC;int _tmpBD;struct Cyc_Tcenv_CtrlEnv _tmpB7=_tmpB6;_tmpB8=_tmpB7.ctrl_rgn;_tmpB9=_tmpB7.continue_stmt;_tmpBA=_tmpB7.break_stmt;_tmpBB=_tmpB7.fallthru_clause;_tmpBC=_tmpB7.next_stmt;_tmpBD=_tmpB7.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp408;struct Cyc_Tcenv_CtrlEnv*_tmpBE=
(_tmp408=_region_malloc(l,sizeof(*_tmp408)),((_tmp408->ctrl_rgn=_tmpB8,((_tmp408->continue_stmt=_tmpB9,((_tmp408->break_stmt=_tmpBA,((_tmp408->fallthru_clause=_tmpBB,((_tmp408->next_stmt=_tmpBC,((_tmp408->try_depth=_tmpBD,_tmp408)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp409;return(_tmp409=_region_malloc(l,sizeof(*_tmp409)),((_tmp409->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAD,((_tmp409->type_vars=(struct Cyc_List_List*)_tmpAE,((_tmp409->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAF,((_tmp409->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB0,((_tmp409->encloser=(struct Cyc_Absyn_Stmt*)_tmpB1,((_tmp409->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBE,((_tmp409->capability=(void*)_tmpB3,((_tmp409->curr_rgn=(void*)_tmpB4,((_tmp409->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB5,((_tmp409->fnrgn=(struct _RegionHandle*)l,_tmp409)))))))))))))))))))));};};}
# 567
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp40A;struct Cyc_Tcenv_Fenv*_tmpC1=Cyc_Tcenv_get_fenv(te,((_tmp40A="return_typ",_tag_dyneither(_tmp40A,sizeof(char),11))));struct Cyc_Tcenv_SharedFenv*_tmpC3;struct Cyc_Tcenv_Fenv*_tmpC2=_tmpC1;_tmpC3=_tmpC2->shared;{
struct Cyc_Tcenv_SharedFenv _tmpC4=*_tmpC3;void*_tmpC6;struct Cyc_Tcenv_SharedFenv _tmpC5=_tmpC4;_tmpC6=_tmpC5.return_typ;
return _tmpC6;};}
# 573
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC8=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpC9=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC8));struct Cyc_List_List*_tmpCB;struct Cyc_Tcenv_Fenv _tmpCA=_tmpC9;_tmpCB=_tmpCA.type_vars;
return _tmpCB;};}
# 580
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp40B;return(_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B->v=Cyc_Tcenv_lookup_type_vars(te),_tmp40B)));}
# 584
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 586
const char*_tmp40C;struct Cyc_Tcenv_Fenv*_tmpCD=
# 588
Cyc_Tcenv_get_fenv(te,((_tmp40C="add_type_vars",_tag_dyneither(_tmp40C,sizeof(char),14))));
# 586
struct Cyc_Tcenv_SharedFenv*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_RgnOrder_RgnPO*_tmpD1;const struct Cyc_Tcenv_Bindings*_tmpD2;struct Cyc_Absyn_Stmt*_tmpD3;struct Cyc_Tcenv_CtrlEnv*_tmpD4;void*_tmpD5;void*_tmpD6;struct Cyc_Tcenv_FenvFlags _tmpD7;struct Cyc_Tcenv_Fenv*_tmpCE=_tmpCD;_tmpCF=_tmpCE->shared;_tmpD0=_tmpCE->type_vars;_tmpD1=_tmpCE->region_order;_tmpD2=_tmpCE->locals;_tmpD3=_tmpCE->encloser;_tmpD4=_tmpCE->ctrl_env;_tmpD5=_tmpCE->capability;_tmpD6=_tmpCE->curr_rgn;_tmpD7=_tmpCE->flags;
# 590
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpD0);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD8);{
struct Cyc_Tcenv_Fenv*_tmp40D;struct Cyc_Tcenv_Fenv*_tmpD9=(_tmp40D=_region_malloc(r,sizeof(*_tmp40D)),((_tmp40D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpCF,((_tmp40D->type_vars=(struct Cyc_List_List*)_tmpD8,((_tmp40D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpD1,((_tmp40D->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD2,((_tmp40D->encloser=(struct Cyc_Absyn_Stmt*)_tmpD3,((_tmp40D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD4,((_tmp40D->capability=(void*)_tmpD5,((_tmp40D->curr_rgn=(void*)_tmpD6,((_tmp40D->flags=(struct Cyc_Tcenv_FenvFlags)_tmpD7,((_tmp40D->fnrgn=(struct _RegionHandle*)r,_tmp40D)))))))))))))))))))));
# 595
return Cyc_Tcenv_put_fenv(r,te,_tmpD9);};};}
# 598
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpDC=te->le;
if(_tmpDC == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 603
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpDC);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 608
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 610
struct _dyneither_ptr*_tmpDD=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpDE=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpDF=_tmpDE;_LL5C: if((_tmpDF.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 615
struct Cyc_Core_Impossible_exn_struct _tmp413;const char*_tmp412;struct Cyc_Core_Impossible_exn_struct*_tmp411;(int)_throw((void*)((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=((_tmp413.tag=Cyc_Core_Impossible,((_tmp413.f1=((_tmp412="add_local_var: called with Rel_n",_tag_dyneither(_tmp412,sizeof(char),33))),_tmp413)))),_tmp411)))));}_LL5B:;}
# 617
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpE3;_push_handler(& _tmpE3);{int _tmpE5=0;if(setjmp(_tmpE3.handler))_tmpE5=1;if(!_tmpE5){
{void*_tmpE6=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpE7=_tmpE6;_LL61: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE8=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpE8->tag != 0)goto _LL63;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpE8->f1);if(_tmpE9->tag != 4)goto _LL63;}}_LL62:
# 621
{const char*_tmp416;void*_tmp415;(_tmp415=0,Cyc_Tcutil_warn(loc,((_tmp416="declaration hides local",_tag_dyneither(_tmp416,sizeof(char),24))),_tag_dyneither(_tmp415,sizeof(void*),0)));}goto _LL60;_LL63: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEA=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEA->tag != 0)goto _LL65;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpEB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpEA->f1);if(_tmpEB->tag != 1)goto _LL65;}}_LL64:
{const char*_tmp419;void*_tmp418;(_tmp418=0,Cyc_Tcutil_warn(loc,((_tmp419="declaration hides global",_tag_dyneither(_tmp419,sizeof(char),25))),_tag_dyneither(_tmp418,sizeof(void*),0)));}goto _LL60;_LL65: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEC=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEC->tag != 0)goto _LL67;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpED=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpEC->f1);if(_tmpED->tag != 5)goto _LL67;}}_LL66:
{const char*_tmp41C;void*_tmp41B;(_tmp41B=0,Cyc_Tcutil_warn(loc,((_tmp41C="declaration hides pattern variable",_tag_dyneither(_tmp41C,sizeof(char),35))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}goto _LL60;_LL67: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEE=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEE->tag != 0)goto _LL69;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpEF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpEE->f1);if(_tmpEF->tag != 3)goto _LL69;}}_LL68:
{const char*_tmp41F;void*_tmp41E;(_tmp41E=0,Cyc_Tcutil_warn(loc,((_tmp41F="declaration hides parameter",_tag_dyneither(_tmp41F,sizeof(char),28))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}goto _LL60;_LL69:;_LL6A:
 goto _LL60;_LL60:;}
# 619
;_pop_handler();}else{void*_tmpE4=(void*)_exn_thrown;void*_tmpF9=_tmpE4;void*_tmpFB;_LL6C: {struct Cyc_Dict_Absent_exn_struct*_tmpFA=(struct Cyc_Dict_Absent_exn_struct*)_tmpF9;if(_tmpFA->tag != Cyc_Dict_Absent)goto _LL6E;}_LL6D:
# 627
 goto _LL6B;_LL6E: _tmpFB=_tmpF9;_LL6F:(void)_rethrow(_tmpFB);_LL6B:;}};}{
const char*_tmp420;struct Cyc_Tcenv_Fenv*_tmpFC=
# 630
Cyc_Tcenv_get_fenv(te,((_tmp420="add_local_var",_tag_dyneither(_tmp420,sizeof(char),14))));
# 628
struct Cyc_Tcenv_SharedFenv*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_RgnOrder_RgnPO*_tmp100;const struct Cyc_Tcenv_Bindings*_tmp101;struct Cyc_Absyn_Stmt*_tmp102;struct Cyc_Tcenv_CtrlEnv*_tmp103;void*_tmp104;void*_tmp105;struct Cyc_Tcenv_FenvFlags _tmp106;struct Cyc_Tcenv_Fenv*_tmpFD=_tmpFC;_tmpFE=_tmpFD->shared;_tmpFF=_tmpFD->type_vars;_tmp100=_tmpFD->region_order;_tmp101=_tmpFD->locals;_tmp102=_tmpFD->encloser;_tmp103=_tmpFD->ctrl_env;_tmp104=_tmpFD->capability;_tmp105=_tmpFD->curr_rgn;_tmp106=_tmpFD->flags;{
# 631
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp423;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp422;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp107=(_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422[0]=((_tmp423.tag=4,((_tmp423.f1=vd,_tmp423)))),_tmp422)));
struct Cyc_Tcenv_Bindings*_tmp424;struct Cyc_Tcenv_Bindings*_tmp108=(_tmp424=_region_malloc(r,sizeof(*_tmp424)),((_tmp424->v=_tmpDD,((_tmp424->b=(void*)_tmp107,((_tmp424->tl=_tmp101,_tmp424)))))));
struct Cyc_Tcenv_Fenv*_tmp425;struct Cyc_Tcenv_Fenv*_tmp109=(_tmp425=_region_malloc(r,sizeof(*_tmp425)),((_tmp425->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFE,((_tmp425->type_vars=(struct Cyc_List_List*)_tmpFF,((_tmp425->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp100,((_tmp425->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp108),((_tmp425->encloser=(struct Cyc_Absyn_Stmt*)_tmp102,((_tmp425->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp103,((_tmp425->capability=(void*)_tmp104,((_tmp425->curr_rgn=(void*)_tmp105,((_tmp425->flags=(struct Cyc_Tcenv_FenvFlags)_tmp106,((_tmp425->fnrgn=(struct _RegionHandle*)r,_tmp425)))))))))))))))))))));
# 635
return Cyc_Tcenv_put_fenv(r,te,_tmp109);};};}
# 638
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 640
struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp110=*_tmp10F;struct Cyc_Tcenv_SharedFenv*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_RgnOrder_RgnPO*_tmp114;const struct Cyc_Tcenv_Bindings*_tmp115;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Tcenv_CtrlEnv*_tmp117;void*_tmp118;void*_tmp119;int _tmp11A;int _tmp11B;int _tmp11C;struct Cyc_Tcenv_Fenv _tmp111=_tmp110;_tmp112=_tmp111.shared;_tmp113=_tmp111.type_vars;_tmp114=_tmp111.region_order;_tmp115=_tmp111.locals;_tmp116=_tmp111.encloser;_tmp117=_tmp111.ctrl_env;_tmp118=_tmp111.capability;_tmp119=_tmp111.curr_rgn;_tmp11A=(_tmp111.flags).in_notreadctxt;_tmp11B=(_tmp111.flags).in_lhs;_tmp11C=(_tmp111.flags).abstract_ok;{
# 645
struct Cyc_Tcenv_Fenv*_tmp426;struct Cyc_Tcenv_Fenv*_tmp11D=(_tmp426=_region_malloc(r,sizeof(*_tmp426)),((_tmp426->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp112,((_tmp426->type_vars=(struct Cyc_List_List*)_tmp113,((_tmp426->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp114,((_tmp426->locals=(const struct Cyc_Tcenv_Bindings*)_tmp115,((_tmp426->encloser=(struct Cyc_Absyn_Stmt*)_tmp116,((_tmp426->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp117,((_tmp426->capability=(void*)_tmp118,((_tmp426->curr_rgn=(void*)_tmp119,((_tmp426->flags=(struct Cyc_Tcenv_FenvFlags)(
# 647
((_tmp426->flags).in_notreadctxt=_tmp11A,(((_tmp426->flags).in_lhs=_tmp11B,(((_tmp426->flags).in_new=(int)status,(((_tmp426->flags).abstract_ok=_tmp11C,_tmp426->flags)))))))),((_tmp426->fnrgn=(struct _RegionHandle*)r,_tmp426)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp11D);};};}
# 651
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp11F=te->le;
if(_tmp11F == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp120=_tmp11F;int _tmp122;struct Cyc_Tcenv_Fenv*_tmp121=_tmp120;_tmp122=(_tmp121->flags).in_new;
return(enum Cyc_Tcenv_NewStatus)_tmp122;};}
# 658
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp123=te->le;
if(_tmp123 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp124=_tmp123;int _tmp126;struct Cyc_Tcenv_Fenv*_tmp125=_tmp124;_tmp126=(_tmp125->flags).abstract_ok;
return _tmp126;};}
# 665
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 667
struct Cyc_Tcenv_Fenv*_tmp127=te->le;
if(_tmp127 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp128=*_tmp127;struct Cyc_Tcenv_SharedFenv*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_RgnOrder_RgnPO*_tmp12C;const struct Cyc_Tcenv_Bindings*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Tcenv_CtrlEnv*_tmp12F;void*_tmp130;void*_tmp131;int _tmp132;int _tmp133;int _tmp134;struct Cyc_Tcenv_Fenv _tmp129=_tmp128;_tmp12A=_tmp129.shared;_tmp12B=_tmp129.type_vars;_tmp12C=_tmp129.region_order;_tmp12D=_tmp129.locals;_tmp12E=_tmp129.encloser;_tmp12F=_tmp129.ctrl_env;_tmp130=_tmp129.capability;_tmp131=_tmp129.curr_rgn;_tmp132=(_tmp129.flags).in_notreadctxt;_tmp133=(_tmp129.flags).in_lhs;_tmp134=(_tmp129.flags).in_new;{
# 672
struct Cyc_Tcenv_Fenv*_tmp427;struct Cyc_Tcenv_Fenv*_tmp135=(_tmp427=_region_malloc(r,sizeof(*_tmp427)),((_tmp427->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12A,((_tmp427->type_vars=(struct Cyc_List_List*)_tmp12B,((_tmp427->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp12C,((_tmp427->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12D,((_tmp427->encloser=(struct Cyc_Absyn_Stmt*)_tmp12E,((_tmp427->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12F,((_tmp427->capability=(void*)_tmp130,((_tmp427->curr_rgn=(void*)_tmp131,((_tmp427->flags=(struct Cyc_Tcenv_FenvFlags)(
# 674
((_tmp427->flags).in_notreadctxt=_tmp132,(((_tmp427->flags).in_lhs=_tmp133,(((_tmp427->flags).in_new=_tmp134,(((_tmp427->flags).abstract_ok=1,_tmp427->flags)))))))),((_tmp427->fnrgn=(struct _RegionHandle*)r,_tmp427)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp135);};};}
# 678
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 680
struct Cyc_Tcenv_Fenv*_tmp137=te->le;
if(_tmp137 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp138=*_tmp137;struct Cyc_Tcenv_SharedFenv*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_RgnOrder_RgnPO*_tmp13C;const struct Cyc_Tcenv_Bindings*_tmp13D;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Tcenv_CtrlEnv*_tmp13F;void*_tmp140;void*_tmp141;int _tmp142;int _tmp143;int _tmp144;struct Cyc_Tcenv_Fenv _tmp139=_tmp138;_tmp13A=_tmp139.shared;_tmp13B=_tmp139.type_vars;_tmp13C=_tmp139.region_order;_tmp13D=_tmp139.locals;_tmp13E=_tmp139.encloser;_tmp13F=_tmp139.ctrl_env;_tmp140=_tmp139.capability;_tmp141=_tmp139.curr_rgn;_tmp142=(_tmp139.flags).in_notreadctxt;_tmp143=(_tmp139.flags).in_lhs;_tmp144=(_tmp139.flags).in_new;{
# 685
struct Cyc_Tcenv_Fenv*_tmp428;struct Cyc_Tcenv_Fenv*_tmp145=(_tmp428=_region_malloc(r,sizeof(*_tmp428)),((_tmp428->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp13A,((_tmp428->type_vars=(struct Cyc_List_List*)_tmp13B,((_tmp428->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13C,((_tmp428->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13D,((_tmp428->encloser=(struct Cyc_Absyn_Stmt*)_tmp13E,((_tmp428->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13F,((_tmp428->capability=(void*)_tmp140,((_tmp428->curr_rgn=(void*)_tmp141,((_tmp428->flags=(struct Cyc_Tcenv_FenvFlags)(
# 687
((_tmp428->flags).in_notreadctxt=_tmp142,(((_tmp428->flags).in_lhs=_tmp143,(((_tmp428->flags).in_new=_tmp144,(((_tmp428->flags).abstract_ok=0,_tmp428->flags)))))))),((_tmp428->fnrgn=(struct _RegionHandle*)r,_tmp428)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp145);};};}
# 691
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp147=te->le;
if(_tmp147 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp148=*_tmp147;struct Cyc_Tcenv_SharedFenv*_tmp14A;struct Cyc_List_List*_tmp14B;struct Cyc_RgnOrder_RgnPO*_tmp14C;const struct Cyc_Tcenv_Bindings*_tmp14D;struct Cyc_Absyn_Stmt*_tmp14E;struct Cyc_Tcenv_CtrlEnv*_tmp14F;void*_tmp150;void*_tmp151;int _tmp152;int _tmp153;int _tmp154;struct Cyc_Tcenv_Fenv _tmp149=_tmp148;_tmp14A=_tmp149.shared;_tmp14B=_tmp149.type_vars;_tmp14C=_tmp149.region_order;_tmp14D=_tmp149.locals;_tmp14E=_tmp149.encloser;_tmp14F=_tmp149.ctrl_env;_tmp150=_tmp149.capability;_tmp151=_tmp149.curr_rgn;_tmp152=(_tmp149.flags).in_lhs;_tmp153=(_tmp149.flags).in_new;_tmp154=(_tmp149.flags).abstract_ok;{
# 697
struct Cyc_Tcenv_Fenv*_tmp429;struct Cyc_Tcenv_Fenv*_tmp155=(_tmp429=_region_malloc(r,sizeof(*_tmp429)),((_tmp429->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp14A,((_tmp429->type_vars=(struct Cyc_List_List*)_tmp14B,((_tmp429->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14C,((_tmp429->locals=(const struct Cyc_Tcenv_Bindings*)_tmp14D,((_tmp429->encloser=(struct Cyc_Absyn_Stmt*)_tmp14E,((_tmp429->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14F,((_tmp429->capability=(void*)_tmp150,((_tmp429->curr_rgn=(void*)_tmp151,((_tmp429->flags=(struct Cyc_Tcenv_FenvFlags)(
# 699
((_tmp429->flags).in_notreadctxt=1,(((_tmp429->flags).in_lhs=_tmp152,(((_tmp429->flags).in_new=_tmp153,(((_tmp429->flags).abstract_ok=_tmp154,_tmp429->flags)))))))),((_tmp429->fnrgn=(struct _RegionHandle*)r,_tmp429)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp155);};};}
# 703
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp157=te->le;
if(_tmp157 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp158=*_tmp157;struct Cyc_Tcenv_SharedFenv*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_RgnOrder_RgnPO*_tmp15C;const struct Cyc_Tcenv_Bindings*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Tcenv_CtrlEnv*_tmp15F;void*_tmp160;void*_tmp161;int _tmp162;int _tmp163;int _tmp164;struct Cyc_Tcenv_Fenv _tmp159=_tmp158;_tmp15A=_tmp159.shared;_tmp15B=_tmp159.type_vars;_tmp15C=_tmp159.region_order;_tmp15D=_tmp159.locals;_tmp15E=_tmp159.encloser;_tmp15F=_tmp159.ctrl_env;_tmp160=_tmp159.capability;_tmp161=_tmp159.curr_rgn;_tmp162=(_tmp159.flags).in_lhs;_tmp163=(_tmp159.flags).in_new;_tmp164=(_tmp159.flags).abstract_ok;{
# 709
struct Cyc_Tcenv_Fenv*_tmp42A;struct Cyc_Tcenv_Fenv*_tmp165=(_tmp42A=_region_malloc(r,sizeof(*_tmp42A)),((_tmp42A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15A,((_tmp42A->type_vars=(struct Cyc_List_List*)_tmp15B,((_tmp42A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C,((_tmp42A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15D,((_tmp42A->encloser=(struct Cyc_Absyn_Stmt*)_tmp15E,((_tmp42A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F,((_tmp42A->capability=(void*)_tmp160,((_tmp42A->curr_rgn=(void*)_tmp161,((_tmp42A->flags=(struct Cyc_Tcenv_FenvFlags)(
# 711
((_tmp42A->flags).in_notreadctxt=0,(((_tmp42A->flags).in_lhs=_tmp162,(((_tmp42A->flags).in_new=_tmp163,(((_tmp42A->flags).abstract_ok=_tmp164,_tmp42A->flags)))))))),((_tmp42A->fnrgn=(struct _RegionHandle*)r,_tmp42A)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp165);};};}
# 715
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp167=te->le;
if(_tmp167 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp168=_tmp167;int _tmp16A;struct Cyc_Tcenv_Fenv*_tmp169=_tmp168;_tmp16A=(_tmp169->flags).in_notreadctxt;
return _tmp16A;};}
# 722
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16B=te->le;
if(_tmp16B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp16C=*_tmp16B;struct Cyc_Tcenv_SharedFenv*_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_RgnOrder_RgnPO*_tmp170;const struct Cyc_Tcenv_Bindings*_tmp171;struct Cyc_Absyn_Stmt*_tmp172;struct Cyc_Tcenv_CtrlEnv*_tmp173;void*_tmp174;void*_tmp175;int _tmp176;int _tmp177;int _tmp178;struct Cyc_Tcenv_Fenv _tmp16D=_tmp16C;_tmp16E=_tmp16D.shared;_tmp16F=_tmp16D.type_vars;_tmp170=_tmp16D.region_order;_tmp171=_tmp16D.locals;_tmp172=_tmp16D.encloser;_tmp173=_tmp16D.ctrl_env;_tmp174=_tmp16D.capability;_tmp175=_tmp16D.curr_rgn;_tmp176=(_tmp16D.flags).in_notreadctxt;_tmp177=(_tmp16D.flags).in_new;_tmp178=(_tmp16D.flags).abstract_ok;{
# 728
struct Cyc_Tcenv_Fenv*_tmp42B;struct Cyc_Tcenv_Fenv*_tmp179=(_tmp42B=_region_malloc(r,sizeof(*_tmp42B)),((_tmp42B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp16E,((_tmp42B->type_vars=(struct Cyc_List_List*)_tmp16F,((_tmp42B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp170,((_tmp42B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp171,((_tmp42B->encloser=(struct Cyc_Absyn_Stmt*)_tmp172,((_tmp42B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp173,((_tmp42B->capability=(void*)_tmp174,((_tmp42B->curr_rgn=(void*)_tmp175,((_tmp42B->flags=(struct Cyc_Tcenv_FenvFlags)(
# 730
((_tmp42B->flags).in_notreadctxt=_tmp176,(((_tmp42B->flags).in_lhs=1,(((_tmp42B->flags).in_new=_tmp177,(((_tmp42B->flags).abstract_ok=_tmp178,_tmp42B->flags)))))))),((_tmp42B->fnrgn=(struct _RegionHandle*)r,_tmp42B)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp179);};};}
# 734
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17B=te->le;
if(_tmp17B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp17C=*_tmp17B;struct Cyc_Tcenv_SharedFenv*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_RgnOrder_RgnPO*_tmp180;const struct Cyc_Tcenv_Bindings*_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Tcenv_CtrlEnv*_tmp183;void*_tmp184;void*_tmp185;int _tmp186;int _tmp187;int _tmp188;struct Cyc_Tcenv_Fenv _tmp17D=_tmp17C;_tmp17E=_tmp17D.shared;_tmp17F=_tmp17D.type_vars;_tmp180=_tmp17D.region_order;_tmp181=_tmp17D.locals;_tmp182=_tmp17D.encloser;_tmp183=_tmp17D.ctrl_env;_tmp184=_tmp17D.capability;_tmp185=_tmp17D.curr_rgn;_tmp186=(_tmp17D.flags).in_notreadctxt;_tmp187=(_tmp17D.flags).in_new;_tmp188=(_tmp17D.flags).abstract_ok;{
# 740
struct Cyc_Tcenv_Fenv*_tmp42C;struct Cyc_Tcenv_Fenv*_tmp189=(_tmp42C=_region_malloc(r,sizeof(*_tmp42C)),((_tmp42C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp17E,((_tmp42C->type_vars=(struct Cyc_List_List*)_tmp17F,((_tmp42C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp180,((_tmp42C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp181,((_tmp42C->encloser=(struct Cyc_Absyn_Stmt*)_tmp182,((_tmp42C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp183,((_tmp42C->capability=(void*)_tmp184,((_tmp42C->curr_rgn=(void*)_tmp185,((_tmp42C->flags=(struct Cyc_Tcenv_FenvFlags)(
# 742
((_tmp42C->flags).in_notreadctxt=_tmp186,(((_tmp42C->flags).in_lhs=0,(((_tmp42C->flags).in_new=_tmp187,(((_tmp42C->flags).abstract_ok=_tmp188,_tmp42C->flags)))))))),((_tmp42C->fnrgn=(struct _RegionHandle*)r,_tmp42C)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp189);};};}
# 746
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18B=te->le;
if(_tmp18B == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp18C=_tmp18B;int _tmp18E;struct Cyc_Tcenv_Fenv*_tmp18D=_tmp18C;_tmp18E=(_tmp18D->flags).in_lhs;
return _tmp18E;};}
# 754
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 756
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp18F;_push_handler(& _tmp18F);{int _tmp191=0;if(setjmp(_tmp18F.handler))_tmp191=1;if(!_tmp191){
{void*_tmp192=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp193=_tmp192;_LL71: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp194=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp194->tag != 0)goto _LL73;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp195=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp194->f1);if(_tmp195->tag != 4)goto _LL73;}}_LL72:
# 760
{const char*_tmp42F;void*_tmp42E;(_tmp42E=0,Cyc_Tcutil_warn(loc,((_tmp42F="pattern variable hides local",_tag_dyneither(_tmp42F,sizeof(char),29))),_tag_dyneither(_tmp42E,sizeof(void*),0)));}goto _LL70;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp196=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp196->tag != 0)goto _LL75;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp197=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp196->f1);if(_tmp197->tag != 1)goto _LL75;}}_LL74:
{const char*_tmp432;void*_tmp431;(_tmp431=0,Cyc_Tcutil_warn(loc,((_tmp432="pattern variable hides global",_tag_dyneither(_tmp432,sizeof(char),30))),_tag_dyneither(_tmp431,sizeof(void*),0)));}goto _LL70;_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp198=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp198->tag != 0)goto _LL77;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp199=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp198->f1);if(_tmp199->tag != 5)goto _LL77;}}_LL76:
{const char*_tmp435;void*_tmp434;(_tmp434=0,Cyc_Tcutil_warn(loc,((_tmp435="pattern variable hides pattern variable",_tag_dyneither(_tmp435,sizeof(char),40))),_tag_dyneither(_tmp434,sizeof(void*),0)));}goto _LL70;_LL77: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp19A=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp19A->tag != 0)goto _LL79;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp19B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp19A->f1);if(_tmp19B->tag != 3)goto _LL79;}}_LL78:
{const char*_tmp438;void*_tmp437;(_tmp437=0,Cyc_Tcutil_warn(loc,((_tmp438="pattern variable hides parameter",_tag_dyneither(_tmp438,sizeof(char),33))),_tag_dyneither(_tmp437,sizeof(void*),0)));}goto _LL70;_LL79:;_LL7A:
 goto _LL70;_LL70:;}
# 758
;_pop_handler();}else{void*_tmp190=(void*)_exn_thrown;void*_tmp1A5=_tmp190;void*_tmp1A7;_LL7C: {struct Cyc_Dict_Absent_exn_struct*_tmp1A6=(struct Cyc_Dict_Absent_exn_struct*)_tmp1A5;if(_tmp1A6->tag != Cyc_Dict_Absent)goto _LL7E;}_LL7D:
# 766
 goto _LL7B;_LL7E: _tmp1A7=_tmp1A5;_LL7F:(void)_rethrow(_tmp1A7);_LL7B:;}};}{
struct _dyneither_ptr*_tmp1A8=(*vd->name).f2;
const char*_tmp439;struct Cyc_Tcenv_Fenv*_tmp1A9=
# 770
Cyc_Tcenv_get_fenv(te,((_tmp439="add_pat_var",_tag_dyneither(_tmp439,sizeof(char),12))));
# 768
struct Cyc_Tcenv_SharedFenv*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_RgnOrder_RgnPO*_tmp1AD;const struct Cyc_Tcenv_Bindings*_tmp1AE;struct Cyc_Absyn_Stmt*_tmp1AF;struct Cyc_Tcenv_CtrlEnv*_tmp1B0;void*_tmp1B1;void*_tmp1B2;struct Cyc_Tcenv_FenvFlags _tmp1B3;struct Cyc_Tcenv_Fenv*_tmp1AA=_tmp1A9;_tmp1AB=_tmp1AA->shared;_tmp1AC=_tmp1AA->type_vars;_tmp1AD=_tmp1AA->region_order;_tmp1AE=_tmp1AA->locals;_tmp1AF=_tmp1AA->encloser;_tmp1B0=_tmp1AA->ctrl_env;_tmp1B1=_tmp1AA->capability;_tmp1B2=_tmp1AA->curr_rgn;_tmp1B3=_tmp1AA->flags;{
# 771
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp43C;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp43B;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1B4=(_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43C.tag=5,((_tmp43C.f1=vd,_tmp43C)))),_tmp43B)));
struct Cyc_Tcenv_Bindings*_tmp43D;struct Cyc_Tcenv_Bindings*_tmp1B5=(_tmp43D=_region_malloc(r,sizeof(*_tmp43D)),((_tmp43D->v=_tmp1A8,((_tmp43D->b=(void*)_tmp1B4,((_tmp43D->tl=_tmp1AE,_tmp43D)))))));
struct Cyc_Tcenv_Fenv*_tmp43E;struct Cyc_Tcenv_Fenv*_tmp1B6=(_tmp43E=_region_malloc(r,sizeof(*_tmp43E)),((_tmp43E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1AB,((_tmp43E->type_vars=(struct Cyc_List_List*)_tmp1AC,((_tmp43E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1AD,((_tmp43E->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp1B5),((_tmp43E->encloser=(struct Cyc_Absyn_Stmt*)_tmp1AF,((_tmp43E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1B0,((_tmp43E->capability=(void*)_tmp1B1,((_tmp43E->curr_rgn=(void*)_tmp1B2,((_tmp43E->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1B3,((_tmp43E->fnrgn=(struct _RegionHandle*)r,_tmp43E)))))))))))))))))))));
# 775
return Cyc_Tcenv_put_fenv(r,te,_tmp1B6);};};}
# 778
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp1BC=te->le;
union Cyc_Absyn_Nmspace _tmp1BE;struct _dyneither_ptr*_tmp1BF;struct _tuple0*_tmp1BD=q;_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;{
union Cyc_Absyn_Nmspace _tmp1C0=_tmp1BE;_LL81: if((_tmp1C0.Loc_n).tag != 4)goto _LL83;_LL82:
# 783
 if(_tmp1BC == 0){struct Cyc_Dict_Absent_exn_struct _tmp441;struct Cyc_Dict_Absent_exn_struct*_tmp440;(int)_throw((void*)((_tmp440=_cycalloc_atomic(sizeof(*_tmp440)),((_tmp440[0]=((_tmp441.tag=Cyc_Dict_Absent,_tmp441)),_tmp440)))));}
goto _LL84;_LL83: if((_tmp1C0.Rel_n).tag != 1)goto _LL85;if((struct Cyc_List_List*)(_tmp1C0.Rel_n).val != 0)goto _LL85;if(!(_tmp1BC != 0))goto _LL85;_LL84: {
# 786
struct Cyc_Tcenv_Fenv*_tmp1C3=_tmp1BC;const struct Cyc_Tcenv_Bindings*_tmp1C5;struct Cyc_Tcenv_Fenv*_tmp1C4=_tmp1C3;_tmp1C5=_tmp1C4->locals;{
# 788
struct _handler_cons _tmp1C6;_push_handler(& _tmp1C6);{int _tmp1C8=0;if(setjmp(_tmp1C6.handler))_tmp1C8=1;if(!_tmp1C8){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp444;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp443;void*_tmp1CB=(void*)((_tmp443=_region_malloc(r,sizeof(*_tmp443)),((_tmp443[0]=((_tmp444.tag=0,((_tmp444.f1=Cyc_Tcenv_lookup_binding(_tmp1C5,_tmp1BF),_tmp444)))),_tmp443))));_npop_handler(0);return _tmp1CB;};_pop_handler();}else{void*_tmp1C7=(void*)_exn_thrown;void*_tmp1CD=_tmp1C7;void*_tmp1CF;_LL88: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp1CE=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1CD;if(_tmp1CE->tag != Cyc_Tcenv_NoBinding)goto _LL8A;}_LL89:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL8A: _tmp1CF=_tmp1CD;_LL8B:(void)_rethrow(_tmp1CF);_LL87:;}};};}_LL85:;_LL86: {
# 792
struct _handler_cons _tmp1D0;_push_handler(& _tmp1D0);{int _tmp1D2=0;if(setjmp(_tmp1D0.handler))_tmp1D2=1;if(!_tmp1D2){{void*_tmp1D3=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1D3;};_pop_handler();}else{void*_tmp1D1=(void*)_exn_thrown;void*_tmp1D5=_tmp1D1;void*_tmp1D7;_LL8D: {struct Cyc_Dict_Absent_exn_struct*_tmp1D6=(struct Cyc_Dict_Absent_exn_struct*)_tmp1D5;if(_tmp1D6->tag != Cyc_Dict_Absent)goto _LL8F;}_LL8E: {
# 796
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp447;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp446;return(void*)((_tmp446=_region_malloc(r,sizeof(*_tmp446)),((_tmp446[0]=((_tmp447.tag=0,((_tmp447.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp447)))),_tmp446))));}_LL8F: _tmp1D7=_tmp1D5;_LL90:(void)_rethrow(_tmp1D7);_LL8C:;}};}_LL80:;};}
# 801
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp448;struct Cyc_Tcenv_Fenv*_tmp1DA=
Cyc_Tcenv_get_fenv(te,((_tmp448="process_continue",_tag_dyneither(_tmp448,sizeof(char),17))));
# 802
struct Cyc_Tcenv_CtrlEnv*_tmp1DC;struct Cyc_Tcenv_Fenv*_tmp1DB=_tmp1DA;_tmp1DC=_tmp1DB->ctrl_env;{
# 804
void*_tmp1DD=_tmp1DC->continue_stmt;void*_tmp1DE=_tmp1DD;struct Cyc_Absyn_Stmt*_tmp1E0;_LL92: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1DF=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1DF->tag != 3)goto _LL94;else{_tmp1E0=_tmp1DF->f1;}}_LL93:
# 806
{struct Cyc_List_List*_tmp449;_tmp1E0->non_local_preds=((_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449->hd=s,((_tmp449->tl=_tmp1E0->non_local_preds,_tmp449))))));}
*sopt=_tmp1E0;
return;_LL94: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1E1=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E1->tag != 0)goto _LL96;}_LL95:
{const char*_tmp44C;void*_tmp44B;(_tmp44B=0,Cyc_Tcutil_terr(s->loc,((_tmp44C="continue not in a loop",_tag_dyneither(_tmp44C,sizeof(char),23))),_tag_dyneither(_tmp44B,sizeof(void*),0)));}return;_LL96: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1E2=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E2->tag != 1)goto _LL98;}_LL97:
 goto _LL99;_LL98: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1E3=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E3->tag != 2)goto _LL91;}_LL99: {
const char*_tmp44D;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp44D="bad continue destination",_tag_dyneither(_tmp44D,sizeof(char),25))));}_LL91:;};}
# 814
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp44E;struct Cyc_Tcenv_Fenv*_tmp1E9=
Cyc_Tcenv_get_fenv(te,((_tmp44E="process_break",_tag_dyneither(_tmp44E,sizeof(char),14))));
# 815
struct Cyc_Tcenv_CtrlEnv*_tmp1EB;struct Cyc_Tcenv_SharedFenv*_tmp1EC;struct Cyc_Tcenv_Fenv*_tmp1EA=_tmp1E9;_tmp1EB=_tmp1EA->ctrl_env;_tmp1EC=_tmp1EA->shared;{
# 817
void*_tmp1ED=_tmp1EB->break_stmt;void*_tmp1EE=_tmp1ED;struct Cyc_Absyn_Stmt*_tmp1F0;_LL9B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1EF=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1EF->tag != 3)goto _LL9D;else{_tmp1F0=_tmp1EF->f1;}}_LL9C:
# 819
{struct Cyc_List_List*_tmp44F;_tmp1F0->non_local_preds=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F->hd=s,((_tmp44F->tl=_tmp1F0->non_local_preds,_tmp44F))))));}
*sopt=_tmp1F0;
return;_LL9D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1F1=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F1->tag != 0)goto _LL9F;}_LL9E:
# 823
{const char*_tmp452;void*_tmp451;(_tmp451=0,Cyc_Tcutil_terr(s->loc,((_tmp452="break not in a loop or switch",_tag_dyneither(_tmp452,sizeof(char),30))),_tag_dyneither(_tmp451,sizeof(void*),0)));}
return;_LL9F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1F2=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F2->tag != 2)goto _LLA1;}_LLA0:
# 843 "tcenv.cyc"
 return;_LLA1: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1F3=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F3->tag != 1)goto _LL9A;}_LLA2:
# 848
{const char*_tmp455;void*_tmp454;(_tmp454=0,Cyc_Tcutil_terr(s->loc,((_tmp455="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp455,sizeof(char),56))),_tag_dyneither(_tmp454,sizeof(void*),0)));}
return;_LL9A:;};}
# 852
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp456;struct Cyc_Tcenv_Fenv*_tmp1FA=Cyc_Tcenv_get_fenv(te,((_tmp456="process_goto",_tag_dyneither(_tmp456,sizeof(char),13))));struct Cyc_Tcenv_SharedFenv*_tmp1FC;struct Cyc_Tcenv_Fenv*_tmp1FB=_tmp1FA;_tmp1FC=_tmp1FB->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1FC->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1FD=_tmp1FC->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1FD,l);
struct _RegionHandle*frgn=_tmp1FC->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp457;slopt=((_tmp457=_region_malloc(frgn,sizeof(*_tmp457)),((_tmp457[0]=0,_tmp457))));}{
struct Cyc_List_List*_tmp458;struct Cyc_List_List*new_needed=(_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458->hd=s,((_tmp458->tl=*slopt,_tmp458)))));
_tmp1FC->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1FD,l,new_needed);};}else{
# 864
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp459;s->non_local_preds=((_tmp459=_cycalloc(sizeof(*_tmp459)),((_tmp459->hd=s,((_tmp459->tl=s->non_local_preds,_tmp459))))));}
*sopt=s;}};}
# 870
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 872
const char*_tmp45A;struct Cyc_Tcenv_Fenv*_tmp202=Cyc_Tcenv_get_fenv(te,((_tmp45A="process_fallthru",_tag_dyneither(_tmp45A,sizeof(char),17))));struct Cyc_Tcenv_CtrlEnv*_tmp204;struct Cyc_Tcenv_Fenv*_tmp203=_tmp202;_tmp204=_tmp203->ctrl_env;{
const struct _tuple10*_tmp205=_tmp204->fallthru_clause;
if(_tmp205 != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp45B;(((*_tmp205).f1)->body)->non_local_preds=(
(_tmp45B=_cycalloc(sizeof(*_tmp45B)),((_tmp45B->hd=s,((_tmp45B->tl=(((*_tmp205).f1)->body)->non_local_preds,_tmp45B))))));}{
struct Cyc_Absyn_Switch_clause**_tmp45C;*clauseopt=((_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C[0]=(*_tmp205).f1,_tmp45C))));};}
# 879
return _tmp205;};}
# 882
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 886
const char*_tmp45D;struct Cyc_Tcenv_Fenv*_tmp209=
# 888
Cyc_Tcenv_get_fenv(te,((_tmp45D="set_fallthru",_tag_dyneither(_tmp45D,sizeof(char),13))));
# 886
struct Cyc_Tcenv_SharedFenv*_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_RgnOrder_RgnPO*_tmp20D;const struct Cyc_Tcenv_Bindings*_tmp20E;struct Cyc_Absyn_Stmt*_tmp20F;struct Cyc_Tcenv_CtrlEnv*_tmp210;void*_tmp211;void*_tmp212;struct Cyc_Tcenv_FenvFlags _tmp213;struct Cyc_Tcenv_Fenv*_tmp20A=_tmp209;_tmp20B=_tmp20A->shared;_tmp20C=_tmp20A->type_vars;_tmp20D=_tmp20A->region_order;_tmp20E=_tmp20A->locals;_tmp20F=_tmp20A->encloser;_tmp210=_tmp20A->ctrl_env;_tmp211=_tmp20A->capability;_tmp212=_tmp20A->curr_rgn;_tmp213=_tmp20A->flags;{
# 889
struct _RegionHandle*_tmp215;void*_tmp216;void*_tmp217;const struct _tuple10*_tmp218;void*_tmp219;int _tmp21A;struct Cyc_Tcenv_CtrlEnv*_tmp214=_tmp210;_tmp215=_tmp214->ctrl_rgn;_tmp216=_tmp214->continue_stmt;_tmp217=_tmp214->break_stmt;_tmp218=_tmp214->fallthru_clause;_tmp219=_tmp214->next_stmt;_tmp21A=_tmp214->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp45E;ft_typ=((_tmp45E=_region_malloc(_tmp215,sizeof(*_tmp45E)),((_tmp45E->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp45E->tl=ft_typ,_tmp45E))))));}{
const struct Cyc_Tcenv_CList*_tmp21C=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp461;struct Cyc_Tcenv_CtrlEnv*_tmp460;struct Cyc_Tcenv_CtrlEnv*_tmp21D=
(_tmp460=_region_malloc(r,sizeof(*_tmp460)),((_tmp460->ctrl_rgn=_tmp215,((_tmp460->continue_stmt=_tmp216,((_tmp460->break_stmt=_tmp217,((_tmp460->fallthru_clause=(const struct _tuple10*)((_tmp461=_region_malloc(_tmp215,sizeof(*_tmp461)),((_tmp461->f1=clause,((_tmp461->f2=new_tvs,((_tmp461->f3=_tmp21C,_tmp461)))))))),((_tmp460->next_stmt=_tmp219,((_tmp460->try_depth=_tmp21A,_tmp460)))))))))))));
# 897
struct Cyc_Tcenv_Fenv*_tmp462;struct Cyc_Tcenv_Fenv*_tmp21E=(_tmp462=_region_malloc(r,sizeof(*_tmp462)),((_tmp462->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp20B,((_tmp462->type_vars=(struct Cyc_List_List*)_tmp20C,((_tmp462->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp20D,((_tmp462->locals=(const struct Cyc_Tcenv_Bindings*)_tmp20E,((_tmp462->encloser=(struct Cyc_Absyn_Stmt*)_tmp20F,((_tmp462->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp21D,((_tmp462->capability=(void*)_tmp211,((_tmp462->curr_rgn=(void*)_tmp212,((_tmp462->flags=(struct Cyc_Tcenv_FenvFlags)_tmp213,((_tmp462->fnrgn=(struct _RegionHandle*)r,_tmp462)))))))))))))))))))));
# 901
return Cyc_Tcenv_put_fenv(r,te,_tmp21E);};};};}
# 904
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp463;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp463="clear_fallthru",_tag_dyneither(_tmp463,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp224;struct Cyc_Tcenv_Fenv*_tmp223=fe;_tmp224=_tmp223->ctrl_env;
_tmp224->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 911
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 913
const char*_tmp464;struct Cyc_Tcenv_Fenv*_tmp226=
# 915
Cyc_Tcenv_get_fenv(te,((_tmp464="set_in_loop",_tag_dyneither(_tmp464,sizeof(char),12))));
# 913
struct Cyc_Tcenv_SharedFenv*_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_RgnOrder_RgnPO*_tmp22A;const struct Cyc_Tcenv_Bindings*_tmp22B;struct Cyc_Absyn_Stmt*_tmp22C;struct Cyc_Tcenv_CtrlEnv*_tmp22D;void*_tmp22E;void*_tmp22F;struct Cyc_Tcenv_FenvFlags _tmp230;struct Cyc_Tcenv_Fenv*_tmp227=_tmp226;_tmp228=_tmp227->shared;_tmp229=_tmp227->type_vars;_tmp22A=_tmp227->region_order;_tmp22B=_tmp227->locals;_tmp22C=_tmp227->encloser;_tmp22D=_tmp227->ctrl_env;_tmp22E=_tmp227->capability;_tmp22F=_tmp227->curr_rgn;_tmp230=_tmp227->flags;{
# 916
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp46F;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp46E;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp46D;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp46C;struct Cyc_Tcenv_CtrlEnv*_tmp46B;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp46B=_region_malloc(r,sizeof(*_tmp46B)),((_tmp46B->ctrl_rgn=r,((_tmp46B->continue_stmt=(void*)(
(_tmp46D=_region_malloc(r,sizeof(*_tmp46D)),((_tmp46D[0]=((_tmp46C.tag=3,((_tmp46C.f1=continue_dest,_tmp46C)))),_tmp46D)))),((_tmp46B->break_stmt=_tmp22D->next_stmt,((_tmp46B->next_stmt=(void*)(
# 920
(_tmp46F=_region_malloc(r,sizeof(*_tmp46F)),((_tmp46F[0]=((_tmp46E.tag=3,((_tmp46E.f1=continue_dest,_tmp46E)))),_tmp46F)))),((_tmp46B->fallthru_clause=_tmp22D->fallthru_clause,((_tmp46B->try_depth=_tmp22D->try_depth,_tmp46B)))))))))))));
# 923
struct Cyc_Tcenv_Fenv*_tmp470;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp470=_region_malloc(r,sizeof(*_tmp470)),((_tmp470->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp228,((_tmp470->type_vars=(struct Cyc_List_List*)_tmp229,((_tmp470->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp22A,((_tmp470->locals=(const struct Cyc_Tcenv_Bindings*)_tmp22B,((_tmp470->encloser=(struct Cyc_Absyn_Stmt*)_tmp22C,((_tmp470->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp470->capability=(void*)_tmp22E,((_tmp470->curr_rgn=(void*)_tmp22F,((_tmp470->flags=(struct Cyc_Tcenv_FenvFlags)_tmp230,((_tmp470->fnrgn=(struct _RegionHandle*)r,_tmp470)))))))))))))))))))));
# 926
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 929
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp471;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp471="enter_try",_tag_dyneither(_tmp471,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp239;struct Cyc_Tcenv_Fenv*_tmp238=fe;_tmp239=_tmp238->ctrl_env;
++ _tmp239->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 935
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp472;struct Cyc_Tcenv_Fenv*_tmp23B=Cyc_Tcenv_get_fenv(te,((_tmp472="get_try_depth",_tag_dyneither(_tmp472,sizeof(char),14))));struct Cyc_Tcenv_CtrlEnv*_tmp23D;struct Cyc_Tcenv_Fenv*_tmp23C=_tmp23B;_tmp23D=_tmp23C->ctrl_env;
return _tmp23D->try_depth;}
# 942
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp473;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp473="set_in_switch",_tag_dyneither(_tmp473,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp240;struct Cyc_Tcenv_Fenv*_tmp23F=fe;_tmp240=_tmp23F->ctrl_env;
_tmp240->break_stmt=_tmp240->next_stmt;
_tmp240->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 950
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 952
const char*_tmp474;struct Cyc_Tcenv_Fenv _tmp242=*
# 954
Cyc_Tcenv_get_fenv(te,((_tmp474="set_next",_tag_dyneither(_tmp474,sizeof(char),9))));
# 952
struct Cyc_Tcenv_SharedFenv*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_RgnOrder_RgnPO*_tmp246;const struct Cyc_Tcenv_Bindings*_tmp247;struct Cyc_Absyn_Stmt*_tmp248;struct Cyc_Tcenv_CtrlEnv*_tmp249;void*_tmp24A;void*_tmp24B;struct Cyc_Tcenv_FenvFlags _tmp24C;struct Cyc_Tcenv_Fenv _tmp243=_tmp242;_tmp244=_tmp243.shared;_tmp245=_tmp243.type_vars;_tmp246=_tmp243.region_order;_tmp247=_tmp243.locals;_tmp248=_tmp243.encloser;_tmp249=_tmp243.ctrl_env;_tmp24A=_tmp243.capability;_tmp24B=_tmp243.curr_rgn;_tmp24C=_tmp243.flags;{
# 955
struct Cyc_Tcenv_CtrlEnv*_tmp475;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp475=_region_malloc(r,sizeof(*_tmp475)),((_tmp475->ctrl_rgn=r,((_tmp475->continue_stmt=_tmp249->continue_stmt,((_tmp475->break_stmt=_tmp249->break_stmt,((_tmp475->next_stmt=j,((_tmp475->fallthru_clause=_tmp249->fallthru_clause,((_tmp475->try_depth=_tmp249->try_depth,_tmp475)))))))))))));
# 963
struct Cyc_Tcenv_Fenv*_tmp476;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp476=_region_malloc(r,sizeof(*_tmp476)),((_tmp476->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp244,((_tmp476->type_vars=(struct Cyc_List_List*)_tmp245,((_tmp476->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp246,((_tmp476->locals=(const struct Cyc_Tcenv_Bindings*)_tmp247,((_tmp476->encloser=(struct Cyc_Absyn_Stmt*)_tmp248,((_tmp476->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp476->capability=(void*)_tmp24A,((_tmp476->curr_rgn=(void*)_tmp24B,((_tmp476->flags=(struct Cyc_Tcenv_FenvFlags)_tmp24C,((_tmp476->fnrgn=(struct _RegionHandle*)r,_tmp476)))))))))))))))))))));
# 966
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 969
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp477;struct Cyc_Tcenv_Fenv*_tmp250=Cyc_Tcenv_get_fenv(te,((_tmp477="add_label",_tag_dyneither(_tmp477,sizeof(char),10))));struct Cyc_Tcenv_SharedFenv*_tmp252;struct Cyc_Tcenv_Fenv*_tmp251=_tmp250;_tmp252=_tmp251->shared;{
struct Cyc_Dict_Dict needed=_tmp252->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp252->frgn;
if(sl_opt != 0){
_tmp252->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 977
struct Cyc_List_List*_tmp253=*sl_opt;
s->non_local_preds=_tmp253;
for(0;_tmp253 != 0;_tmp253=_tmp253->tl){
void*_tmp254=((struct Cyc_Absyn_Stmt*)_tmp253->hd)->r;void*_tmp255=_tmp254;struct Cyc_Absyn_Stmt**_tmp257;_LLA4: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp256=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp255;if(_tmp256->tag != 8)goto _LLA6;else{_tmp257=(struct Cyc_Absyn_Stmt**)& _tmp256->f2;}}_LLA5:
*_tmp257=s;goto _LLA3;_LLA6:;_LLA7:
{const char*_tmp478;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp478="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp478,sizeof(char),42))));}goto _LLA3;_LLA3:;}};}
# 985
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp252->seen_labels,v)){
const char*_tmp47C;void*_tmp47B[1];struct Cyc_String_pa_PrintArg_struct _tmp47A;(_tmp47A.tag=0,((_tmp47A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp47B[0]=& _tmp47A,Cyc_Tcutil_terr(s->loc,((_tmp47C="Repeated label: %s",_tag_dyneither(_tmp47C,sizeof(char),19))),_tag_dyneither(_tmp47B,sizeof(void*),1)))))));}
_tmp252->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp252->seen_labels,v,s);
return te;};}
# 991
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp47D;struct Cyc_Tcenv_Fenv*_tmp25D=Cyc_Tcenv_get_fenv(te,((_tmp47D="all_labels_resolved",_tag_dyneither(_tmp47D,sizeof(char),20))));struct Cyc_Tcenv_SharedFenv*_tmp25F;struct Cyc_Tcenv_Fenv*_tmp25E=_tmp25D;_tmp25F=_tmp25E->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp25F->needed_labels);}
# 996
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp47E;struct Cyc_Tcenv_Fenv*_tmp261=Cyc_Tcenv_get_fenv(te,((_tmp47E="get_encloser",_tag_dyneither(_tmp47E,sizeof(char),13))));struct Cyc_Absyn_Stmt*_tmp263;struct Cyc_Tcenv_Fenv*_tmp262=_tmp261;_tmp263=_tmp262->encloser;
return _tmp263;}
# 1001
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp47F;return(_tmp47F=_region_malloc(r2,sizeof(*_tmp47F)),((_tmp47F->ns=te->ns,((_tmp47F->ae=te->ae,((_tmp47F->le=0,((_tmp47F->allow_valueof=1,((_tmp47F->in_extern_c_include=te->in_extern_c_include,_tmp47F)))))))))));}{
struct Cyc_Tcenv_Fenv _tmp266=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_RgnOrder_RgnPO*_tmp26A;const struct Cyc_Tcenv_Bindings*_tmp26B;struct Cyc_Absyn_Stmt*_tmp26C;struct Cyc_Tcenv_CtrlEnv*_tmp26D;void*_tmp26E;void*_tmp26F;struct Cyc_Tcenv_FenvFlags _tmp270;struct Cyc_Tcenv_Fenv _tmp267=_tmp266;_tmp268=_tmp267.shared;_tmp269=_tmp267.type_vars;_tmp26A=_tmp267.region_order;_tmp26B=_tmp267.locals;_tmp26C=_tmp267.encloser;_tmp26D=_tmp267.ctrl_env;_tmp26E=_tmp267.capability;_tmp26F=_tmp267.curr_rgn;_tmp270=_tmp267.flags;{
struct Cyc_Tcenv_Fenv*_tmp480;struct Cyc_Tcenv_Fenv*_tmp271=(_tmp480=_region_malloc(r2,sizeof(*_tmp480)),((_tmp480->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp268,((_tmp480->type_vars=(struct Cyc_List_List*)_tmp269,((_tmp480->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp26A,((_tmp480->locals=(const struct Cyc_Tcenv_Bindings*)_tmp26B,((_tmp480->encloser=(struct Cyc_Absyn_Stmt*)_tmp26C,((_tmp480->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp26D,((_tmp480->capability=(void*)_tmp26E,((_tmp480->curr_rgn=(void*)_tmp26F,((_tmp480->flags=(struct Cyc_Tcenv_FenvFlags)_tmp270,((_tmp480->fnrgn=(struct _RegionHandle*)r2,_tmp480)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp481;return(_tmp481=_region_malloc(r2,sizeof(*_tmp481)),((_tmp481->ns=te->ns,((_tmp481->ae=te->ae,((_tmp481->le=_tmp271,((_tmp481->allow_valueof=1,((_tmp481->in_extern_c_include=te->in_extern_c_include,_tmp481)))))))))));};};}
# 1009
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp482;return(_tmp482=_region_malloc(r2,sizeof(*_tmp482)),((_tmp482->ns=te->ns,((_tmp482->ae=te->ae,((_tmp482->le=0,((_tmp482->allow_valueof=te->allow_valueof,((_tmp482->in_extern_c_include=1,_tmp482)))))))))));}{
struct Cyc_Tcenv_Fenv _tmp275=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_RgnOrder_RgnPO*_tmp279;const struct Cyc_Tcenv_Bindings*_tmp27A;struct Cyc_Absyn_Stmt*_tmp27B;struct Cyc_Tcenv_CtrlEnv*_tmp27C;void*_tmp27D;void*_tmp27E;struct Cyc_Tcenv_FenvFlags _tmp27F;struct Cyc_Tcenv_Fenv _tmp276=_tmp275;_tmp277=_tmp276.shared;_tmp278=_tmp276.type_vars;_tmp279=_tmp276.region_order;_tmp27A=_tmp276.locals;_tmp27B=_tmp276.encloser;_tmp27C=_tmp276.ctrl_env;_tmp27D=_tmp276.capability;_tmp27E=_tmp276.curr_rgn;_tmp27F=_tmp276.flags;{
struct Cyc_Tcenv_Fenv*_tmp483;struct Cyc_Tcenv_Fenv*_tmp280=(_tmp483=_region_malloc(r2,sizeof(*_tmp483)),((_tmp483->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp277,((_tmp483->type_vars=(struct Cyc_List_List*)_tmp278,((_tmp483->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp279,((_tmp483->locals=(const struct Cyc_Tcenv_Bindings*)_tmp27A,((_tmp483->encloser=(struct Cyc_Absyn_Stmt*)_tmp27B,((_tmp483->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp27C,((_tmp483->capability=(void*)_tmp27D,((_tmp483->curr_rgn=(void*)_tmp27E,((_tmp483->flags=(struct Cyc_Tcenv_FenvFlags)_tmp27F,((_tmp483->fnrgn=(struct _RegionHandle*)r2,_tmp483)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp484;return(_tmp484=_region_malloc(r2,sizeof(*_tmp484)),((_tmp484->ns=te->ns,((_tmp484->ae=te->ae,((_tmp484->le=_tmp280,((_tmp484->allow_valueof=te->allow_valueof,((_tmp484->in_extern_c_include=1,_tmp484)))))))))));};};}
# 1017
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1019
const char*_tmp485;struct Cyc_Tcenv_Fenv _tmp283=*
# 1021
Cyc_Tcenv_get_fenv(te,((_tmp485="set_encloser",_tag_dyneither(_tmp485,sizeof(char),13))));
# 1019
struct Cyc_Tcenv_SharedFenv*_tmp285;struct Cyc_List_List*_tmp286;struct Cyc_RgnOrder_RgnPO*_tmp287;const struct Cyc_Tcenv_Bindings*_tmp288;struct Cyc_Absyn_Stmt*_tmp289;struct Cyc_Tcenv_CtrlEnv*_tmp28A;void*_tmp28B;void*_tmp28C;struct Cyc_Tcenv_FenvFlags _tmp28D;struct Cyc_Tcenv_Fenv _tmp284=_tmp283;_tmp285=_tmp284.shared;_tmp286=_tmp284.type_vars;_tmp287=_tmp284.region_order;_tmp288=_tmp284.locals;_tmp289=_tmp284.encloser;_tmp28A=_tmp284.ctrl_env;_tmp28B=_tmp284.capability;_tmp28C=_tmp284.curr_rgn;_tmp28D=_tmp284.flags;{
# 1022
struct Cyc_Tcenv_Fenv*_tmp486;struct Cyc_Tcenv_Fenv*_tmp28E=
(_tmp486=_region_malloc(r,sizeof(*_tmp486)),((_tmp486->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp285,((_tmp486->type_vars=(struct Cyc_List_List*)_tmp286,((_tmp486->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp287,((_tmp486->locals=(const struct Cyc_Tcenv_Bindings*)_tmp288,((_tmp486->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp486->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp28A,((_tmp486->capability=(void*)_tmp28B,((_tmp486->curr_rgn=(void*)_tmp28C,((_tmp486->flags=(struct Cyc_Tcenv_FenvFlags)_tmp28D,((_tmp486->fnrgn=(struct _RegionHandle*)r,_tmp486)))))))))))))))))))));
# 1025
return Cyc_Tcenv_put_fenv(r,te,_tmp28E);};}
# 1028
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1031
const char*_tmp487;struct Cyc_Tcenv_Fenv _tmp291=*
# 1033
Cyc_Tcenv_get_fenv(te,((_tmp487="add_region",_tag_dyneither(_tmp487,sizeof(char),11))));
# 1031
struct Cyc_Tcenv_SharedFenv*_tmp293;struct Cyc_List_List*_tmp294;struct Cyc_RgnOrder_RgnPO*_tmp295;const struct Cyc_Tcenv_Bindings*_tmp296;struct Cyc_Absyn_Stmt*_tmp297;struct Cyc_Tcenv_CtrlEnv*_tmp298;void*_tmp299;void*_tmp29A;struct Cyc_Tcenv_FenvFlags _tmp29B;struct Cyc_Tcenv_Fenv _tmp292=_tmp291;_tmp293=_tmp292.shared;_tmp294=_tmp292.type_vars;_tmp295=_tmp292.region_order;_tmp296=_tmp292.locals;_tmp297=_tmp292.encloser;_tmp298=_tmp292.ctrl_env;_tmp299=_tmp292.capability;_tmp29A=_tmp292.curr_rgn;_tmp29B=_tmp292.flags;
# 1034
{void*_tmp29C=Cyc_Tcutil_compress(rgn);void*_tmp29D=_tmp29C;struct Cyc_Absyn_Tvar*_tmp29F;_LLA9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp29E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29D;if(_tmp29E->tag != 2)goto _LLAB;else{_tmp29F=_tmp29E->f1;}}_LLAA:
# 1036
 _tmp295=Cyc_RgnOrder_add_youngest(_tmp293->frgn,_tmp295,_tmp29F,resetable,opened);
# 1038
goto _LLA8;_LLAB:;_LLAC:
 goto _LLA8;_LLA8:;}
# 1041
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp498;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp497;struct Cyc_List_List*_tmp496;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp495;_tmp299=(void*)((_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495[0]=((_tmp49A.tag=24,((_tmp49A.f1=((_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->hd=(void*)((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498[0]=((_tmp497.tag=23,((_tmp497.f1=rgn,_tmp497)))),_tmp498)))),((_tmp496->tl=(
(_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499->hd=_tmp299,((_tmp499->tl=0,_tmp499)))))),_tmp496)))))),_tmp49A)))),_tmp495))));}{
struct Cyc_Tcenv_Fenv*_tmp49B;struct Cyc_Tcenv_Fenv*_tmp2A6=
(_tmp49B=_region_malloc(r,sizeof(*_tmp49B)),((_tmp49B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp293,((_tmp49B->type_vars=(struct Cyc_List_List*)_tmp294,((_tmp49B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp295,((_tmp49B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp296,((_tmp49B->encloser=(struct Cyc_Absyn_Stmt*)_tmp297,((_tmp49B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp298,((_tmp49B->capability=(void*)_tmp299,((_tmp49B->curr_rgn=(void*)_tmp29A,((_tmp49B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp29B,((_tmp49B->fnrgn=(struct _RegionHandle*)r,_tmp49B)))))))))))))))))))));
# 1046
return Cyc_Tcenv_put_fenv(r,te,_tmp2A6);};}
# 1049
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1051
const char*_tmp49C;struct Cyc_Tcenv_Fenv _tmp2A9=*
# 1053
Cyc_Tcenv_get_fenv(te,((_tmp49C="new_named_block",_tag_dyneither(_tmp49C,sizeof(char),16))));
# 1051
struct Cyc_Tcenv_SharedFenv*_tmp2AB;struct Cyc_List_List*_tmp2AC;struct Cyc_RgnOrder_RgnPO*_tmp2AD;const struct Cyc_Tcenv_Bindings*_tmp2AE;struct Cyc_Absyn_Stmt*_tmp2AF;struct Cyc_Tcenv_CtrlEnv*_tmp2B0;void*_tmp2B1;void*_tmp2B2;struct Cyc_Tcenv_FenvFlags _tmp2B3;struct Cyc_Tcenv_Fenv _tmp2AA=_tmp2A9;_tmp2AB=_tmp2AA.shared;_tmp2AC=_tmp2AA.type_vars;_tmp2AD=_tmp2AA.region_order;_tmp2AE=_tmp2AA.locals;_tmp2AF=_tmp2AA.encloser;_tmp2B0=_tmp2AA.ctrl_env;_tmp2B1=_tmp2AA.capability;_tmp2B2=_tmp2AA.curr_rgn;_tmp2B3=_tmp2AA.flags;{
# 1055
const char*_tmp49D;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp49D="new_block",_tag_dyneither(_tmp49D,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49F;void*block_typ=(void*)((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F[0]=((_tmp4A0.tag=2,((_tmp4A0.f1=block_rgn,_tmp4A0)))),_tmp49F))));
{struct Cyc_List_List*_tmp4A1;_tmp2AC=((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->hd=block_rgn,((_tmp4A1->tl=_tmp2AC,_tmp4A1))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp2AC);
_tmp2AD=Cyc_RgnOrder_add_youngest(_tmp2AB->frgn,_tmp2AD,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4B4;struct Cyc_List_List*_tmp4B3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4B2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4B1;struct Cyc_List_List*_tmp4B0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4AF;_tmp2B1=(void*)((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF[0]=((_tmp4B4.tag=24,((_tmp4B4.f1=((_tmp4B0=_cycalloc(sizeof(*_tmp4B0)),((_tmp4B0->hd=(void*)((_tmp4B2=_cycalloc(sizeof(*_tmp4B2)),((_tmp4B2[0]=((_tmp4B1.tag=23,((_tmp4B1.f1=block_typ,_tmp4B1)))),_tmp4B2)))),((_tmp4B0->tl=((_tmp4B3=_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->hd=_tmp2B1,((_tmp4B3->tl=0,_tmp4B3)))))),_tmp4B0)))))),_tmp4B4)))),_tmp4AF))));}
_tmp2B2=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp4B5;struct Cyc_Tcenv_Fenv*_tmp2BB=
(_tmp4B5=_region_malloc(r,sizeof(*_tmp4B5)),((_tmp4B5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp2AB,((_tmp4B5->type_vars=(struct Cyc_List_List*)_tmp2AC,((_tmp4B5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2AD,((_tmp4B5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2AE,((_tmp4B5->encloser=(struct Cyc_Absyn_Stmt*)_tmp2AF,((_tmp4B5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp2B0,((_tmp4B5->capability=(void*)_tmp2B1,((_tmp4B5->curr_rgn=(void*)_tmp2B2,((_tmp4B5->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2B3,((_tmp4B5->fnrgn=(struct _RegionHandle*)r,_tmp4B5)))))))))))))))))))));
# 1065
return Cyc_Tcenv_put_fenv(r,te,_tmp2BB);};};}
# 1068
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 1070
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 1072
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 1078
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1082
const char*_tmp4B6;struct Cyc_Tcenv_Fenv _tmp2C2=*
# 1084
Cyc_Tcenv_get_fenv(te,((_tmp4B6="new_outlives_constraints",_tag_dyneither(_tmp4B6,sizeof(char),25))));
# 1082
struct Cyc_Tcenv_SharedFenv*_tmp2C4;struct Cyc_List_List*_tmp2C5;struct Cyc_RgnOrder_RgnPO*_tmp2C6;const struct Cyc_Tcenv_Bindings*_tmp2C7;struct Cyc_Absyn_Stmt*_tmp2C8;struct Cyc_Tcenv_CtrlEnv*_tmp2C9;void*_tmp2CA;void*_tmp2CB;struct Cyc_Tcenv_FenvFlags _tmp2CC;struct Cyc_Tcenv_Fenv _tmp2C3=_tmp2C2;_tmp2C4=_tmp2C3.shared;_tmp2C5=_tmp2C3.type_vars;_tmp2C6=_tmp2C3.region_order;_tmp2C7=_tmp2C3.locals;_tmp2C8=_tmp2C3.encloser;_tmp2C9=_tmp2C3.ctrl_env;_tmp2CA=_tmp2C3.capability;_tmp2CB=_tmp2C3.curr_rgn;_tmp2CC=_tmp2C3.flags;
# 1085
for(0;cs != 0;cs=cs->tl){
_tmp2C6=
Cyc_RgnOrder_add_outlives_constraint(_tmp2C4->frgn,_tmp2C6,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1090
struct Cyc_Tcenv_Fenv*_tmp4B7;struct Cyc_Tcenv_Fenv*_tmp2CD=
(_tmp4B7=_region_malloc(r,sizeof(*_tmp4B7)),((_tmp4B7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp2C4,((_tmp4B7->type_vars=(struct Cyc_List_List*)_tmp2C5,((_tmp4B7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2C6,((_tmp4B7->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2C7,((_tmp4B7->encloser=(struct Cyc_Absyn_Stmt*)_tmp2C8,((_tmp4B7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp2C9,((_tmp4B7->capability=(void*)_tmp2CA,((_tmp4B7->curr_rgn=(void*)_tmp2CB,((_tmp4B7->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2CC,((_tmp4B7->fnrgn=(struct _RegionHandle*)r,_tmp4B7)))))))))))))))))))));
# 1093
return Cyc_Tcenv_put_fenv(r,te,_tmp2CD);};}
# 1096
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1104
void*_tmp2D0=Cyc_Tcutil_compress(r1);
void*_tmp2D1=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp2D2=Cyc_Tcutil_typ_kind(_tmp2D0);
struct Cyc_Absyn_Kind*_tmp2D3=Cyc_Tcutil_typ_kind(_tmp2D1);
# 1110
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp2D2,_tmp2D3);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp2D3,_tmp2D2);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp4BE;void*_tmp4BD[4];struct Cyc_String_pa_PrintArg_struct _tmp4BC;struct Cyc_String_pa_PrintArg_struct _tmp4BB;struct Cyc_String_pa_PrintArg_struct _tmp4BA;struct Cyc_String_pa_PrintArg_struct _tmp4B9;(_tmp4B9.tag=0,((_tmp4B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp2D3)),((_tmp4BA.tag=0,((_tmp4BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp2D2)),((_tmp4BB.tag=0,((_tmp4BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2D1)),((_tmp4BC.tag=0,((_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2D0)),((_tmp4BD[0]=& _tmp4BC,((_tmp4BD[1]=& _tmp4BB,((_tmp4BD[2]=& _tmp4BA,((_tmp4BD[3]=& _tmp4B9,Cyc_Tcutil_terr(loc,((_tmp4BE="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp4BE,sizeof(char),82))),_tag_dyneither(_tmp4BD,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1117
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp2DA=Cyc_Tcutil_swap_kind(_tmp2D1,Cyc_Tcutil_kind_to_bound(_tmp2D2));struct Cyc_Absyn_Tvar*_tmp2DC;void*_tmp2DD;struct _tuple11 _tmp2DB=_tmp2DA;_tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;{
struct _tuple11*_tmp4BF;*oldtv=((_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF)),((_tmp4BF->f1=_tmp2DC,((_tmp4BF->f2=_tmp2DD,_tmp4BF))))));};}else{
# 1121
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp2DF=Cyc_Tcutil_swap_kind(_tmp2D0,Cyc_Tcutil_kind_to_bound(_tmp2D3));struct Cyc_Absyn_Tvar*_tmp2E1;void*_tmp2E2;struct _tuple11 _tmp2E0=_tmp2DF;_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;{
struct _tuple11*_tmp4C0;*oldtv=((_tmp4C0=_region_malloc(r,sizeof(*_tmp4C0)),((_tmp4C0->f1=_tmp2E1,((_tmp4C0->f2=_tmp2E2,_tmp4C0))))));};}}}{
# 1127
struct _RegionHandle*_tmp2E4=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2E5=0;
if((_tmp2D0 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2D0 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2D0 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4C3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C2;void*eff1=(void*)((_tmp4C2=_cycalloc(sizeof(*_tmp4C2)),((_tmp4C2[0]=((_tmp4C3.tag=23,((_tmp4C3.f1=_tmp2D0,_tmp4C3)))),_tmp4C2))));
struct _tuple13*_tmp4C6;struct Cyc_List_List*_tmp4C5;_tmp2E5=((_tmp4C5=_region_malloc(_tmp2E4,sizeof(*_tmp4C5)),((_tmp4C5->hd=((_tmp4C6=_region_malloc(_tmp2E4,sizeof(*_tmp4C6)),((_tmp4C6->f1=eff1,((_tmp4C6->f2=_tmp2D1,_tmp4C6)))))),((_tmp4C5->tl=_tmp2E5,_tmp4C5))))));}
# 1133
if((_tmp2D1 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2D1 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2D1 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4C9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C8;void*eff2=(void*)((_tmp4C8=_cycalloc(sizeof(*_tmp4C8)),((_tmp4C8[0]=((_tmp4C9.tag=23,((_tmp4C9.f1=_tmp2D1,_tmp4C9)))),_tmp4C8))));
struct _tuple13*_tmp4CC;struct Cyc_List_List*_tmp4CB;_tmp2E5=((_tmp4CB=_region_malloc(_tmp2E4,sizeof(*_tmp4CB)),((_tmp4CB->hd=((_tmp4CC=_region_malloc(_tmp2E4,sizeof(*_tmp4CC)),((_tmp4CC->f1=eff2,((_tmp4CC->f2=_tmp2D0,_tmp4CC)))))),((_tmp4CB->tl=_tmp2E5,_tmp4CB))))));}
# 1138
return Cyc_Tcenv_new_outlives_constraints(_tmp2E4,te,_tmp2E5,loc);};}
# 1141
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2EE=te->le;
if(_tmp2EE == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2EE;
void*_tmp2F0;struct Cyc_Tcenv_Fenv*_tmp2EF=fe;_tmp2F0=_tmp2EF->curr_rgn;
return _tmp2F0;};}
# 1152
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp4CD;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp4CD="check_rgn_accessible",_tag_dyneither(_tmp4CD,sizeof(char),21))));
void*_tmp2F2;struct Cyc_RgnOrder_RgnPO*_tmp2F3;struct Cyc_Tcenv_Fenv*_tmp2F1=fe;_tmp2F2=_tmp2F1->capability;_tmp2F3=_tmp2F1->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2F2) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2F2))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4D0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4CF;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2F3,(void*)((_tmp4CF=_cycalloc(sizeof(*_tmp4CF)),((_tmp4CF[0]=((_tmp4D0.tag=23,((_tmp4D0.f1=rgn,_tmp4D0)))),_tmp4CF)))),_tmp2F2))
return;}{
const char*_tmp4D4;void*_tmp4D3[1];struct Cyc_String_pa_PrintArg_struct _tmp4D2;(_tmp4D2.tag=0,((_tmp4D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4D3[0]=& _tmp4D2,Cyc_Tcutil_terr(loc,((_tmp4D4="Expression accesses unavailable region %s",_tag_dyneither(_tmp4D4,sizeof(char),42))),_tag_dyneither(_tmp4D3,sizeof(void*),1)))))));};}
# 1164
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp4D5;struct Cyc_Tcenv_Fenv*_tmp2FA=
Cyc_Tcenv_get_fenv(te,((_tmp4D5="check_rgn_resetable",_tag_dyneither(_tmp4D5,sizeof(char),20))));
# 1166
struct Cyc_RgnOrder_RgnPO*_tmp2FC;struct Cyc_Tcenv_Fenv*_tmp2FB=_tmp2FA;_tmp2FC=_tmp2FB->region_order;{
# 1168
void*_tmp2FD=Cyc_Tcutil_compress(rgn);void*_tmp2FE=_tmp2FD;struct Cyc_Absyn_Tvar*_tmp300;_LLAE: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2FF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2FE;if(_tmp2FF->tag != 2)goto _LLB0;else{_tmp300=_tmp2FF->f1;}}_LLAF:
# 1170
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2FC,_tmp300)){
const char*_tmp4D9;void*_tmp4D8[1];struct Cyc_String_pa_PrintArg_struct _tmp4D7;(_tmp4D7.tag=0,((_tmp4D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4D8[0]=& _tmp4D7,Cyc_Tcutil_terr(loc,((_tmp4D9="Region %s is not resetable",_tag_dyneither(_tmp4D9,sizeof(char),27))),_tag_dyneither(_tmp4D8,sizeof(void*),1)))))));}
return;_LLB0:;_LLB1: {
# 1174
const char*_tmp4DC;void*_tmp4DB;(_tmp4DB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DC="check_rgn_resetable",_tag_dyneither(_tmp4DC,sizeof(char),20))),_tag_dyneither(_tmp4DB,sizeof(void*),0)));}_LLAD:;};};}
# 1181
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp307=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1186
if(_tmp307 == 0){
void*_tmp308=rt_a;_LLB3: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp309=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp308;if(_tmp309->tag != 22)goto _LLB5;}_LLB4:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LLB5: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp30A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp308;if(_tmp30A->tag != 21)goto _LLB7;}_LLB6:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLB7: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp30B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp308;if(_tmp30B->tag != 20)goto _LLB9;}_LLB8:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLB9:;_LLBA:
 return 0;_LLB2:;}{
# 1194
struct Cyc_Tcenv_Fenv*fe=_tmp307;
struct Cyc_RgnOrder_RgnPO*_tmp30D;struct Cyc_Tcenv_Fenv*_tmp30C=fe;_tmp30D=_tmp30C->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4DF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4DE;int _tmp30E=Cyc_RgnOrder_effect_outlives(_tmp30D,(void*)((_tmp4DE=_cycalloc(sizeof(*_tmp4DE)),((_tmp4DE[0]=((_tmp4DF.tag=23,((_tmp4DF.f1=rt_a,_tmp4DF)))),_tmp4DE)))),rt_b);
# 1200
return _tmp30E;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1205
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp4E0;struct Cyc_Tcenv_Fenv*_tmp311=
Cyc_Tcenv_get_fenv(te,((_tmp4E0="check_effect_accessible",_tag_dyneither(_tmp4E0,sizeof(char),24))));
# 1206
void*_tmp313;struct Cyc_RgnOrder_RgnPO*_tmp314;struct Cyc_Tcenv_SharedFenv*_tmp315;struct Cyc_Tcenv_Fenv*_tmp312=_tmp311;_tmp313=_tmp312->capability;_tmp314=_tmp312->region_order;_tmp315=_tmp312->shared;
# 1208
if(Cyc_Tcutil_subset_effect(0,eff,_tmp313))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp314,eff,_tmp313))
return;{
struct _RegionHandle*frgn=_tmp315->frgn;
struct _tuple14*_tmp4E3;struct Cyc_List_List*_tmp4E2;_tmp315->delayed_effect_checks=(
(_tmp4E2=_region_malloc(frgn,sizeof(*_tmp4E2)),((_tmp4E2->hd=((_tmp4E3=_region_malloc(frgn,sizeof(*_tmp4E3)),((_tmp4E3->f1=_tmp313,((_tmp4E3->f2=eff,((_tmp4E3->f3=_tmp314,((_tmp4E3->f4=loc,_tmp4E3)))))))))),((_tmp4E2->tl=_tmp315->delayed_effect_checks,_tmp4E2))))));};}
# 1217
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4E4;struct Cyc_Tcenv_Fenv*_tmp319=
Cyc_Tcenv_get_fenv(te,((_tmp4E4="check_delayed_effects",_tag_dyneither(_tmp4E4,sizeof(char),22))));
# 1218
struct Cyc_Tcenv_SharedFenv*_tmp31B;struct Cyc_Tcenv_Fenv*_tmp31A=_tmp319;_tmp31B=_tmp31A->shared;{
# 1220
struct Cyc_List_List*_tmp31C=_tmp31B->delayed_effect_checks;
for(0;_tmp31C != 0;_tmp31C=_tmp31C->tl){
struct _tuple14*_tmp31D=(struct _tuple14*)_tmp31C->hd;void*_tmp31F;void*_tmp320;struct Cyc_RgnOrder_RgnPO*_tmp321;unsigned int _tmp322;struct _tuple14*_tmp31E=_tmp31D;_tmp31F=_tmp31E->f1;_tmp320=_tmp31E->f2;_tmp321=_tmp31E->f3;_tmp322=_tmp31E->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp320,_tmp31F))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp321,_tmp320,_tmp31F))
continue;{
const char*_tmp4E9;void*_tmp4E8[2];struct Cyc_String_pa_PrintArg_struct _tmp4E7;struct Cyc_String_pa_PrintArg_struct _tmp4E6;(_tmp4E6.tag=0,((_tmp4E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp320)),((_tmp4E7.tag=0,((_tmp4E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp31F)),((_tmp4E8[0]=& _tmp4E7,((_tmp4E8[1]=& _tmp4E6,Cyc_Tcutil_terr(_tmp322,((_tmp4E9="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp4E9,sizeof(char),51))),_tag_dyneither(_tmp4E8,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1236
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1238
struct Cyc_Tcenv_Fenv*_tmp328=te->le;
if(_tmp328 == 0){
# 1241
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4EC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4EB;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp4EB=_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB[0]=((_tmp4EC.tag=23,((_tmp4EC.f1=(*((struct _tuple13*)po->hd)).f2,_tmp4EC)))),_tmp4EB)))),Cyc_Absyn_empty_effect)){
const char*_tmp4EF;void*_tmp4EE;(_tmp4EE=0,Cyc_Tcutil_terr(loc,((_tmp4EF="the required region ordering is not satisfied here",_tag_dyneither(_tmp4EF,sizeof(char),51))),_tag_dyneither(_tmp4EE,sizeof(void*),0)));}}
return;}{
# 1247
struct Cyc_Tcenv_Fenv*_tmp32D=_tmp328;struct Cyc_RgnOrder_RgnPO*_tmp32F;struct Cyc_Tcenv_SharedFenv*_tmp330;struct Cyc_Tcenv_Fenv*_tmp32E=_tmp32D;_tmp32F=_tmp32E->region_order;_tmp330=_tmp32E->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp32F,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp4F2;struct Cyc_List_List*_tmp4F1;_tmp330->delayed_constraint_checks=(
(_tmp4F1=_region_malloc(_tmp330->frgn,sizeof(*_tmp4F1)),((_tmp4F1->hd=((_tmp4F2=_region_malloc(_tmp330->frgn,sizeof(*_tmp4F2)),((_tmp4F2->f1=_tmp32F,((_tmp4F2->f2=po,((_tmp4F2->f3=loc,_tmp4F2)))))))),((_tmp4F1->tl=_tmp330->delayed_constraint_checks,_tmp4F1))))));}};}
# 1254
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4F3;struct Cyc_Tcenv_Fenv*_tmp333=
Cyc_Tcenv_get_fenv(te,((_tmp4F3="check_delayed_constraints",_tag_dyneither(_tmp4F3,sizeof(char),26))));
# 1255
struct Cyc_Tcenv_SharedFenv*_tmp335;struct Cyc_Tcenv_Fenv*_tmp334=_tmp333;_tmp335=_tmp334->shared;{
# 1257
struct Cyc_List_List*_tmp336=_tmp335->delayed_constraint_checks;
for(0;_tmp336 != 0;_tmp336=_tmp336->tl){
struct _tuple15*_tmp337=(struct _tuple15*)_tmp336->hd;struct Cyc_RgnOrder_RgnPO*_tmp339;struct Cyc_List_List*_tmp33A;unsigned int _tmp33B;struct _tuple15*_tmp338=_tmp337;_tmp339=_tmp338->f1;_tmp33A=_tmp338->f2;_tmp33B=_tmp338->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp339,_tmp33A,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp4F6;void*_tmp4F5;(_tmp4F5=0,Cyc_Tcutil_terr(_tmp33B,((_tmp4F6="the required region ordering is not satisfied here",_tag_dyneither(_tmp4F6,sizeof(char),51))),_tag_dyneither(_tmp4F5,sizeof(void*),0)));}}};}
# 1265
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp503;const char*_tmp502;void*_tmp501[1];struct Cyc_String_pa_PrintArg_struct _tmp500;struct Cyc_Absyn_Tvar*_tmp4FF;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4FF=_cycalloc(sizeof(*_tmp4FF)),((_tmp4FF->name=((_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503[0]=(struct _dyneither_ptr)((_tmp500.tag=0,((_tmp500.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp501[0]=& _tmp500,Cyc_aprintf(((_tmp502="`%s",_tag_dyneither(_tmp502,sizeof(char),4))),_tag_dyneither(_tmp501,sizeof(void*),1)))))))),_tmp503)))),((_tmp4FF->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4FF->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4FF)))))));
struct Cyc_List_List*_tmp504;struct Cyc_List_List*_tmp33F=(_tmp504=_cycalloc(sizeof(*_tmp504)),((_tmp504->hd=rgn0,((_tmp504->tl=fd->tvs,_tmp504)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp33F);{
# 1273
struct Cyc_RgnOrder_RgnPO*_tmp340=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1275
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp507;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp506;void*param_rgn=(void*)((_tmp506=_cycalloc(sizeof(*_tmp506)),((_tmp506[0]=((_tmp507.tag=2,((_tmp507.f1=rgn0,_tmp507)))),_tmp506))));
struct Cyc_List_List*_tmp341=0;
{struct Cyc_List_List*_tmp342=fd->args;for(0;_tmp342 != 0;_tmp342=_tmp342->tl){
struct Cyc_Absyn_Vardecl _tmp50D;struct _tuple0*_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50B;struct Cyc_Absyn_Vardecl*_tmp343=(_tmp50B=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp50B[0]=(struct Cyc_Absyn_Vardecl)((_tmp50D.sc=Cyc_Absyn_Public,((_tmp50D.name=(
(_tmp50C=_cycalloc(sizeof(*_tmp50C)),((_tmp50C->f1=Cyc_Absyn_Loc_n,((_tmp50C->f2=(*((struct _tuple8*)_tmp342->hd)).f1,_tmp50C)))))),((_tmp50D.tq=(*((struct _tuple8*)_tmp342->hd)).f2,((_tmp50D.type=(*((struct _tuple8*)_tmp342->hd)).f3,((_tmp50D.initializer=0,((_tmp50D.rgn=param_rgn,((_tmp50D.attributes=0,((_tmp50D.escapes=0,_tmp50D)))))))))))))))),_tmp50B)));
# 1285
{struct Cyc_List_List _tmp510;struct Cyc_List_List*_tmp50F;_tmp341=((_tmp50F=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp50F[0]=(struct Cyc_List_List)((_tmp510.hd=_tmp343,((_tmp510.tl=_tmp341,_tmp510)))),_tmp50F))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp513;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp512;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp346=(_tmp512=_cycalloc(sizeof(*_tmp512)),((_tmp512[0]=((_tmp513.tag=3,((_tmp513.f1=_tmp343,_tmp513)))),_tmp512)));
struct _dyneither_ptr*_tmp347=(*((struct _tuple8*)_tmp342->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp514;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp514=_region_malloc(r,sizeof(*_tmp514)),((_tmp514->v=_tmp347,((_tmp514->b=(void*)_tmp346,((_tmp514->tl=locals,_tmp514))))))));};}}
# 1290
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp34E=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp350;struct Cyc_Absyn_Tqual _tmp351;void*_tmp352;int _tmp353;struct Cyc_Absyn_VarargInfo _tmp34F=_tmp34E;_tmp350=_tmp34F.name;_tmp351=_tmp34F.tq;_tmp352=_tmp34F.type;_tmp353=_tmp34F.inject;
if(_tmp350 != 0){
void*_tmp354=Cyc_Absyn_dyneither_typ(_tmp352,param_rgn,_tmp351,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp51A;struct _tuple0*_tmp519;struct Cyc_Absyn_Vardecl*_tmp518;struct Cyc_Absyn_Vardecl*_tmp355=(_tmp518=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp518[0]=(struct Cyc_Absyn_Vardecl)((_tmp51A.sc=Cyc_Absyn_Public,((_tmp51A.name=(
(_tmp519=_cycalloc(sizeof(*_tmp519)),((_tmp519->f1=Cyc_Absyn_Loc_n,((_tmp519->f2=_tmp350,_tmp519)))))),((_tmp51A.tq=
Cyc_Absyn_empty_tqual(0),((_tmp51A.type=_tmp354,((_tmp51A.initializer=0,((_tmp51A.rgn=param_rgn,((_tmp51A.attributes=0,((_tmp51A.escapes=0,_tmp51A)))))))))))))))),_tmp518)));
# 1301
{struct Cyc_List_List*_tmp51B;_tmp341=((_tmp51B=_cycalloc(sizeof(*_tmp51B)),((_tmp51B->hd=_tmp355,((_tmp51B->tl=_tmp341,_tmp51B))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp51E;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp51D;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp357=(_tmp51D=_cycalloc(sizeof(*_tmp51D)),((_tmp51D[0]=((_tmp51E.tag=3,((_tmp51E.f1=_tmp355,_tmp51E)))),_tmp51D)));
struct _dyneither_ptr*_tmp358=_tmp350;
struct Cyc_Tcenv_Bindings*_tmp51F;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp51F=_region_malloc(r,sizeof(*_tmp51F)),((_tmp51F->v=_tmp358,((_tmp51F->b=(void*)_tmp357,((_tmp51F->tl=locals,_tmp51F))))))));};}else{
# 1306
const char*_tmp522;void*_tmp521;(_tmp521=0,Cyc_Tcutil_terr(loc,((_tmp522="missing name for varargs",_tag_dyneither(_tmp522,sizeof(char),25))),_tag_dyneither(_tmp521,sizeof(void*),0)));}}
# 1308
{struct Cyc_Core_Opt _tmp525;struct Cyc_Core_Opt*_tmp524;fd->param_vardecls=((_tmp524=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp524[0]=(struct Cyc_Core_Opt)((_tmp525.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp341),_tmp525)),_tmp524))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp543;struct Cyc_List_List*_tmp542;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp541;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp540;struct Cyc_List_List*_tmp53F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp53E;struct Cyc_Tcenv_CtrlEnv*_tmp53D;struct Cyc_Tcenv_SharedFenv*_tmp53C;struct Cyc_Tcenv_Fenv*_tmp53B;return(_tmp53B=_region_malloc(r,sizeof(*_tmp53B)),((_tmp53B->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp53C=_region_malloc(r,sizeof(*_tmp53C)),((_tmp53C->frgn=r,((_tmp53C->return_typ=fd->ret_type,((_tmp53C->seen_labels=
# 1313
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp53C->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp53C->delayed_effect_checks=0,((_tmp53C->delayed_constraint_checks=0,_tmp53C)))))))))))))),((_tmp53B->type_vars=(struct Cyc_List_List*)_tmp33F,((_tmp53B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp340,((_tmp53B->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp53B->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp53B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1322
(_tmp53D=_region_malloc(r,sizeof(*_tmp53D)),((_tmp53D->ctrl_rgn=r,((_tmp53D->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp53D->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp53D->fallthru_clause=0,((_tmp53D->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp53D->try_depth=0,_tmp53D)))))))))))))),((_tmp53B->capability=(void*)((void*)(
(_tmp543=_cycalloc(sizeof(*_tmp543)),((_tmp543[0]=((_tmp53E.tag=24,((_tmp53E.f1=((_tmp542=_cycalloc(sizeof(*_tmp542)),((_tmp542->hd=(void*)((_tmp540=_cycalloc(sizeof(*_tmp540)),((_tmp540[0]=((_tmp541.tag=23,((_tmp541.f1=param_rgn,_tmp541)))),_tmp540)))),((_tmp542->tl=(
(_tmp53F=_cycalloc(sizeof(*_tmp53F)),((_tmp53F->hd=(void*)_check_null(fd->effect),((_tmp53F->tl=0,_tmp53F)))))),_tmp542)))))),_tmp53E)))),_tmp543))))),((_tmp53B->curr_rgn=(void*)param_rgn,((_tmp53B->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1326
((_tmp53B->flags).in_notreadctxt=0,(((_tmp53B->flags).in_lhs=0,(((_tmp53B->flags).in_new=0,(((_tmp53B->flags).abstract_ok=0,_tmp53B->flags)))))))),((_tmp53B->fnrgn=(struct _RegionHandle*)r,_tmp53B)))))))))))))))))))));};};}
# 1331
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 1334
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*_tmp544;struct Cyc_Absyn_Tvar*rgn0=(_tmp544=_cycalloc(sizeof(*_tmp544)),((_tmp544->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6),((_tmp544->identity=Cyc_Tcutil_new_tvar_id(),((_tmp544->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp544)))))));
struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp374=(_tmp545=_cycalloc(sizeof(*_tmp545)),((_tmp545->hd=rgn0,((_tmp545->tl=0,_tmp545)))));
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp548;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp547;struct Cyc_RgnOrder_RgnPO*_tmp375=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)((_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547[0]=((_tmp548.tag=24,((_tmp548.f1=0,_tmp548)))),_tmp547)))),rgn0,0);
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp54B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp54A;void*param_rgn=(void*)((_tmp54A=_cycalloc(sizeof(*_tmp54A)),((_tmp54A[0]=((_tmp54B.tag=2,((_tmp54B.f1=rgn0,_tmp54B)))),_tmp54A))));
struct Cyc_List_List*_tmp376=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp377=*((struct _tuple8*)args->hd);struct _dyneither_ptr*_tmp379;struct Cyc_Absyn_Tqual _tmp37A;void*_tmp37B;struct _tuple8 _tmp378=_tmp377;_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;_tmp37B=_tmp378.f3;
if(_tmp379 != 0){
struct Cyc_Absyn_Vardecl _tmp551;struct _tuple0*_tmp550;struct Cyc_Absyn_Vardecl*_tmp54F;struct Cyc_Absyn_Vardecl*_tmp37C=(_tmp54F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp54F[0]=(struct Cyc_Absyn_Vardecl)((_tmp551.sc=Cyc_Absyn_Public,((_tmp551.name=(
(_tmp550=_cycalloc(sizeof(*_tmp550)),((_tmp550->f1=Cyc_Absyn_Loc_n,((_tmp550->f2=_tmp379,_tmp550)))))),((_tmp551.tq=_tmp37A,((_tmp551.type=_tmp37B,((_tmp551.initializer=0,((_tmp551.rgn=param_rgn,((_tmp551.attributes=0,((_tmp551.escapes=0,_tmp551)))))))))))))))),_tmp54F)));
# 1350
{struct Cyc_List_List _tmp554;struct Cyc_List_List*_tmp553;_tmp376=((_tmp553=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp553[0]=(struct Cyc_List_List)((_tmp554.hd=_tmp37C,((_tmp554.tl=_tmp376,_tmp554)))),_tmp553))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp557;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp556;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37F=(_tmp556=_cycalloc(sizeof(*_tmp556)),((_tmp556[0]=((_tmp557.tag=3,((_tmp557.f1=_tmp37C,_tmp557)))),_tmp556)));
struct _dyneither_ptr*_tmp380=_tmp379;
struct Cyc_Tcenv_Bindings*_tmp558;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp558=_region_malloc(r,sizeof(*_tmp558)),((_tmp558->v=_tmp380,((_tmp558->b=(void*)_tmp37F,((_tmp558->tl=locals,_tmp558))))))));};}}{
# 1356
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp571;struct Cyc_List_List*_tmp570;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp56F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp56E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp56D;struct Cyc_Tcenv_CtrlEnv*_tmp56C;struct Cyc_Tcenv_SharedFenv*_tmp56B;struct Cyc_Tcenv_Fenv*_tmp56A;return(_tmp56A=_region_malloc(r,sizeof(*_tmp56A)),((_tmp56A->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp56B=_region_malloc(r,sizeof(*_tmp56B)),((_tmp56B->frgn=r,((_tmp56B->return_typ=ret_type,((_tmp56B->seen_labels=
# 1360
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp56B->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp56B->delayed_effect_checks=0,((_tmp56B->delayed_constraint_checks=0,_tmp56B)))))))))))))),((_tmp56A->type_vars=(struct Cyc_List_List*)_tmp374,((_tmp56A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp375,((_tmp56A->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp56A->encloser=(struct Cyc_Absyn_Stmt*)
# 1368
Cyc_Absyn_skip_stmt(0),((_tmp56A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
(_tmp56C=_region_malloc(r,sizeof(*_tmp56C)),((_tmp56C->ctrl_rgn=r,((_tmp56C->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp56C->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp56C->fallthru_clause=0,((_tmp56C->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp56C->try_depth=0,_tmp56C)))))))))))))),((_tmp56A->capability=(void*)((void*)(
(_tmp571=_cycalloc(sizeof(*_tmp571)),((_tmp571[0]=((_tmp56D.tag=24,((_tmp56D.f1=((_tmp570=_cycalloc(sizeof(*_tmp570)),((_tmp570->hd=(void*)((_tmp56E=_cycalloc(sizeof(*_tmp56E)),((_tmp56E[0]=((_tmp56F.tag=23,((_tmp56F.f1=param_rgn,_tmp56F)))),_tmp56E)))),((_tmp570->tl=0,_tmp570)))))),_tmp56D)))),_tmp571))))),((_tmp56A->curr_rgn=(void*)param_rgn,((_tmp56A->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1372
((_tmp56A->flags).in_notreadctxt=0,(((_tmp56A->flags).in_lhs=0,(((_tmp56A->flags).in_new=0,(((_tmp56A->flags).abstract_ok=0,_tmp56A->flags)))))))),((_tmp56A->fnrgn=(struct _RegionHandle*)r,_tmp56A)))))))))))))))))))));};}
# 1377
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp397;struct Cyc_RgnOrder_RgnPO*_tmp398;struct Cyc_List_List*_tmp399;struct Cyc_Tcenv_SharedFenv*_tmp39A;struct _RegionHandle*_tmp39B;struct Cyc_Tcenv_Fenv*_tmp396=old_fenv;_tmp397=_tmp396->locals;_tmp398=_tmp396->region_order;_tmp399=_tmp396->type_vars;_tmp39A=_tmp396->shared;_tmp39B=_tmp396->fnrgn;{
# 1380
struct _RegionHandle*_tmp39C=_tmp39A->frgn;
const struct Cyc_Tcenv_Bindings*_tmp39D=_tmp397;
struct _dyneither_ptr*_tmp57E;const char*_tmp57D;void*_tmp57C[1];struct Cyc_String_pa_PrintArg_struct _tmp57B;struct Cyc_Absyn_Tvar*_tmp57A;struct Cyc_Absyn_Tvar*rgn0=
(_tmp57A=_cycalloc(sizeof(*_tmp57A)),((_tmp57A->name=((_tmp57E=_cycalloc(sizeof(*_tmp57E)),((_tmp57E[0]=(struct _dyneither_ptr)((_tmp57B.tag=0,((_tmp57B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp57C[0]=& _tmp57B,Cyc_aprintf(((_tmp57D="`%s",_tag_dyneither(_tmp57D,sizeof(char),4))),_tag_dyneither(_tmp57C,sizeof(void*),1)))))))),_tmp57E)))),((_tmp57A->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp57A->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp57A)))))));
# 1386
{struct Cyc_List_List*_tmp39E=fd->tvs;for(0;_tmp39E != 0;_tmp39E=_tmp39E->tl){
struct Cyc_Absyn_Kind*_tmp39F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp39E->hd,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmp3A1;enum Cyc_Absyn_AliasQual _tmp3A2;struct Cyc_Absyn_Kind*_tmp3A0=_tmp39F;_tmp3A1=_tmp3A0->kind;_tmp3A2=_tmp3A0->aliasqual;
if(_tmp3A1 == Cyc_Absyn_RgnKind){
if(_tmp3A2 == Cyc_Absyn_Aliasable)
_tmp398=Cyc_RgnOrder_add_youngest(_tmp39C,_tmp398,(struct Cyc_Absyn_Tvar*)_tmp39E->hd,0,0);else{
# 1392
const char*_tmp581;void*_tmp580;(_tmp580=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp581="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp581,sizeof(char),39))),_tag_dyneither(_tmp580,sizeof(void*),0)));}}}}
# 1394
_tmp398=Cyc_RgnOrder_add_youngest(_tmp39C,_tmp398,rgn0,0,0);{
struct Cyc_List_List*_tmp582;struct Cyc_List_List*_tmp3A5=(_tmp582=_cycalloc(sizeof(*_tmp582)),((_tmp582->hd=rgn0,((_tmp582->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp399),_tmp582)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp3A5);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp585;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp584;void*param_rgn=(void*)((_tmp584=_cycalloc(sizeof(*_tmp584)),((_tmp584[0]=((_tmp585.tag=2,((_tmp585.f1=rgn0,_tmp585)))),_tmp584))));
struct Cyc_List_List*_tmp3A6=0;
{struct Cyc_List_List*_tmp3A7=fd->args;for(0;_tmp3A7 != 0;_tmp3A7=_tmp3A7->tl){
struct Cyc_Absyn_Vardecl _tmp58B;struct _tuple0*_tmp58A;struct Cyc_Absyn_Vardecl*_tmp589;struct Cyc_Absyn_Vardecl*_tmp3A8=(_tmp589=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp589[0]=(struct Cyc_Absyn_Vardecl)((_tmp58B.sc=Cyc_Absyn_Public,((_tmp58B.name=(
(_tmp58A=_cycalloc(sizeof(*_tmp58A)),((_tmp58A->f1=Cyc_Absyn_Loc_n,((_tmp58A->f2=(*((struct _tuple8*)_tmp3A7->hd)).f1,_tmp58A)))))),((_tmp58B.tq=(*((struct _tuple8*)_tmp3A7->hd)).f2,((_tmp58B.type=(*((struct _tuple8*)_tmp3A7->hd)).f3,((_tmp58B.initializer=0,((_tmp58B.rgn=param_rgn,((_tmp58B.attributes=0,((_tmp58B.escapes=0,_tmp58B)))))))))))))))),_tmp589)));
# 1407
{struct Cyc_List_List _tmp58E;struct Cyc_List_List*_tmp58D;_tmp3A6=((_tmp58D=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp58D[0]=(struct Cyc_List_List)((_tmp58E.hd=_tmp3A8,((_tmp58E.tl=_tmp3A6,_tmp58E)))),_tmp58D))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp591;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp590;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3AB=(_tmp590=_cycalloc(sizeof(*_tmp590)),((_tmp590[0]=((_tmp591.tag=3,((_tmp591.f1=_tmp3A8,_tmp591)))),_tmp590)));
struct _dyneither_ptr*_tmp3AC=(*((struct _tuple8*)_tmp3A7->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp592;_tmp39D=(const struct Cyc_Tcenv_Bindings*)((_tmp592=_region_malloc(_tmp39C,sizeof(*_tmp592)),((_tmp592->v=_tmp3AC,((_tmp592->b=(void*)_tmp3AB,((_tmp592->tl=_tmp39D,_tmp592))))))));};}}
# 1412
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp3B3=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp3B5;struct Cyc_Absyn_Tqual _tmp3B6;void*_tmp3B7;int _tmp3B8;struct Cyc_Absyn_VarargInfo _tmp3B4=_tmp3B3;_tmp3B5=_tmp3B4.name;_tmp3B6=_tmp3B4.tq;_tmp3B7=_tmp3B4.type;_tmp3B8=_tmp3B4.inject;
if(_tmp3B5 != 0){
void*_tmp3B9=Cyc_Absyn_dyneither_typ(_tmp3B7,param_rgn,_tmp3B6,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp598;struct _tuple0*_tmp597;struct Cyc_Absyn_Vardecl*_tmp596;struct Cyc_Absyn_Vardecl*_tmp3BA=(_tmp596=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp596[0]=(struct Cyc_Absyn_Vardecl)((_tmp598.sc=Cyc_Absyn_Public,((_tmp598.name=(
(_tmp597=_cycalloc(sizeof(*_tmp597)),((_tmp597->f1=Cyc_Absyn_Loc_n,((_tmp597->f2=_tmp3B5,_tmp597)))))),((_tmp598.tq=
Cyc_Absyn_empty_tqual(0),((_tmp598.type=_tmp3B9,((_tmp598.initializer=0,((_tmp598.rgn=param_rgn,((_tmp598.attributes=0,((_tmp598.escapes=0,_tmp598)))))))))))))))),_tmp596)));
# 1423
{struct Cyc_List_List*_tmp599;_tmp3A6=((_tmp599=_cycalloc(sizeof(*_tmp599)),((_tmp599->hd=_tmp3BA,((_tmp599->tl=_tmp3A6,_tmp599))))));}{
struct _dyneither_ptr*_tmp3BC=_tmp3B5;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp59C;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp59B;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3BD=(_tmp59B=_cycalloc(sizeof(*_tmp59B)),((_tmp59B[0]=((_tmp59C.tag=3,((_tmp59C.f1=_tmp3BA,_tmp59C)))),_tmp59B)));
struct Cyc_Tcenv_Bindings*_tmp59D;_tmp39D=(const struct Cyc_Tcenv_Bindings*)((_tmp59D=_region_malloc(_tmp39C,sizeof(*_tmp59D)),((_tmp59D->v=_tmp3BC,((_tmp59D->b=(void*)_tmp3BD,((_tmp59D->tl=_tmp39D,_tmp59D))))))));};}else{
# 1428
const char*_tmp5A0;void*_tmp59F;(_tmp59F=0,Cyc_Tcutil_terr(loc,((_tmp5A0="missing name for varargs",_tag_dyneither(_tmp5A0,sizeof(char),25))),_tag_dyneither(_tmp59F,sizeof(void*),0)));}}
# 1430
{struct Cyc_Core_Opt _tmp5A3;struct Cyc_Core_Opt*_tmp5A2;fd->param_vardecls=((_tmp5A2=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp5A2[0]=(struct Cyc_Core_Opt)((_tmp5A3.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3A6),_tmp5A3)),_tmp5A2))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5C1;struct Cyc_List_List*_tmp5C0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp5BF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5BE;struct Cyc_List_List*_tmp5BD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp5BC;struct Cyc_Tcenv_CtrlEnv*_tmp5BB;struct Cyc_Tcenv_SharedFenv*_tmp5BA;struct Cyc_Tcenv_Fenv*_tmp5B9;return(_tmp5B9=_region_malloc(_tmp39C,sizeof(*_tmp5B9)),((_tmp5B9->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp5BA=_region_malloc(_tmp39C,sizeof(*_tmp5BA)),((_tmp5BA->frgn=_tmp39C,((_tmp5BA->return_typ=fd->ret_type,((_tmp5BA->seen_labels=
# 1435
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp39C,Cyc_strptrcmp),((_tmp5BA->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp39C,Cyc_strptrcmp),((_tmp5BA->delayed_effect_checks=0,((_tmp5BA->delayed_constraint_checks=0,_tmp5BA)))))))))))))),((_tmp5B9->type_vars=(struct Cyc_List_List*)_tmp3A5,((_tmp5B9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp398,((_tmp5B9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp39D,((_tmp5B9->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp5B9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1444
(_tmp5BB=_region_malloc(_tmp39C,sizeof(*_tmp5BB)),((_tmp5BB->ctrl_rgn=_tmp39C,((_tmp5BB->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp5BB->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp5BB->fallthru_clause=0,((_tmp5BB->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp5BB->try_depth=0,_tmp5BB)))))))))))))),((_tmp5B9->capability=(void*)((void*)(
(_tmp5C1=_cycalloc(sizeof(*_tmp5C1)),((_tmp5C1[0]=((_tmp5BC.tag=24,((_tmp5BC.f1=((_tmp5C0=_cycalloc(sizeof(*_tmp5C0)),((_tmp5C0->hd=(void*)((_tmp5BE=_cycalloc(sizeof(*_tmp5BE)),((_tmp5BE[0]=((_tmp5BF.tag=23,((_tmp5BF.f1=param_rgn,_tmp5BF)))),_tmp5BE)))),((_tmp5C0->tl=(
(_tmp5BD=_cycalloc(sizeof(*_tmp5BD)),((_tmp5BD->hd=(void*)_check_null(fd->effect),((_tmp5BD->tl=0,_tmp5BD)))))),_tmp5C0)))))),_tmp5BC)))),_tmp5C1))))),((_tmp5B9->curr_rgn=(void*)param_rgn,((_tmp5B9->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1448
((_tmp5B9->flags).in_notreadctxt=0,(((_tmp5B9->flags).in_lhs=0,(((_tmp5B9->flags).in_new=0,(((_tmp5B9->flags).abstract_ok=0,_tmp5B9->flags)))))))),((_tmp5B9->fnrgn=(struct _RegionHandle*)_tmp39B,_tmp5B9)))))))))))))))))))));};};};};}
