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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 450 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 778 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 945
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 968
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 988
extern void*Cyc_Absyn_empty_effect;
# 990
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1023
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1110
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
return({struct Cyc_Tcenv_Genv*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->grgn=r;_tmp5->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp5->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->availables=0;_tmp5;});}struct _tuple12{void*f1;int f2;};
# 253
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 255
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp6;});{
# 257
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Datatypedecl**_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=Cyc_Absyn_exn_tud;_tmp8;}));
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,({struct _tuple12*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmpC;_tmpC.tag=2;_tmpC.f1=Cyc_Absyn_exn_tud;_tmpC.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd;_tmpC;});_tmpB;});_tmpA->f2=1;_tmpA;}));}}{
# 264
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
return({struct Cyc_Tcenv_Tenv*_tmpD=_region_malloc(r,sizeof(*_tmpD));_tmpD->ns=0;_tmpD->ae=ae;_tmpD->le=0;_tmpD->allow_valueof=0;_tmpD->in_extern_c_include=0;_tmpD;});};};}
# 270
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 272
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 276
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0)
return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpE="outer_namespace";_tag_dyneither(_tmpE,sizeof(char),16);}));
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
lookup(ge2,v,0);
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp12[1]={& _tmp14};Cyc_Tcutil_terr(loc,({const char*_tmp13="%s is ambiguous";_tag_dyneither(_tmp13,sizeof(char),16);}),_tag_dyneither(_tmp12,sizeof(void*),1));});});
# 304
;_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp15=_tmp10;void*_tmp16;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 306
 goto _LL0;}else{_LL3: _tmp16=_tmp15;_LL4:(int)_rethrow(_tmp16);}_LL0:;}};}}
# 308
return;}
# 311
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 316
{struct _handler_cons _tmp17;_push_handler(& _tmp17);{int _tmp19=0;if(setjmp(_tmp17.handler))_tmp19=1;if(!_tmp19){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1A=result;_npop_handler(0);return _tmp1A;};}
# 317
;_pop_handler();}else{void*_tmp18=(void*)_exn_thrown;void*_tmp1B=_tmp18;void*_tmp1C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B)->tag == Cyc_Dict_Absent){_LL6: _LL7:
# 320
 goto _LL5;}else{_LL8: _tmp1C=_tmp1B;_LL9:(int)_rethrow(_tmp1C);}_LL5:;}};}
# 322
{struct Cyc_List_List*_tmp1D=ge->availables;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp1D->hd);
struct _handler_cons _tmp1E;_push_handler(& _tmp1E);{int _tmp20=0;if(setjmp(_tmp1E.handler))_tmp20=1;if(!_tmp20){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp1D->hd,_tmp1D->tl);{
void*_tmp21=result;_npop_handler(0);return _tmp21;};}
# 325
;_pop_handler();}else{void*_tmp1F=(void*)_exn_thrown;void*_tmp22=_tmp1F;void*_tmp23;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp22)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 328
 goto _LLA;}else{_LLD: _tmp23=_tmp22;_LLE:(int)_rethrow(_tmp23);}_LLA:;}};}}
# 330
if(cns == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp24=_cycalloc_atomic(sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Dict_Absent_exn_struct _tmp25;_tmp25.tag=Cyc_Dict_Absent;_tmp25;});_tmp24;}));
# 333
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 337
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp26=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp26,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 344
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple0*_tmp27=q;union Cyc_Absyn_Nmspace _tmp28;struct _dyneither_ptr*_tmp29;_LL10: _tmp28=_tmp27->f1;_tmp29=_tmp27->f2;_LL11:;{
union Cyc_Absyn_Nmspace _tmp2A=_tmp28;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2C;struct _dyneither_ptr*_tmp2D;struct Cyc_List_List*_tmp2E;switch((_tmp2A.C_n).tag){case 4: _LL13: _LL14:
 goto _LL16;case 1: if((_tmp2A.Rel_n).val == 0){_LL15: _LL16:
# 349
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp29,is_use);}else{_LL17: _tmp2D=(struct _dyneither_ptr*)((_tmp2A.Rel_n).val)->hd;_tmp2E=((_tmp2A.Rel_n).val)->tl;_LL18: {
# 351
struct Cyc_Tcenv_Genv*_tmp2F=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2D,_tmp2E);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2F,_tmp29,is_use);}}case 3: _LL19: _tmp2C=(_tmp2A.C_n).val;_LL1A:
 _tmp2B=_tmp2C;goto _LL1C;default: _LL1B: _tmp2B=(_tmp2A.Abs_n).val;_LL1C:
# 355
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2B),_tmp29,is_use);}_LL12:;};}
# 360
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 362
struct Cyc_List_List*_tmp30=te->ns;
struct _RegionHandle _tmp31=_new_region("temp");struct _RegionHandle*temp=& _tmp31;_push_region(temp);
{struct Cyc_List_List*_tmp32=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp33=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp30);
struct Cyc_List_List*_tmp34=_tmp33->availables;
struct Cyc_Set_Set*_tmp35=_tmp33->namespaces;
# 370
{struct Cyc_List_List*_tmp36=_tmp34;for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
struct Cyc_Set_Set*_tmp37=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp36->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp37,n))
_tmp32=({struct Cyc_List_List*_tmp38=_region_malloc(temp,sizeof(*_tmp38));_tmp38->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30,({struct Cyc_List_List*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->hd=n;_tmp39->tl=ns;_tmp39;}));_tmp38->tl=_tmp32;_tmp38;});}}
# 377
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp35,n))
_tmp32=({struct Cyc_List_List*_tmp3A=_region_malloc(temp,sizeof(*_tmp3A));_tmp3A->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30,({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->hd=n;_tmp3B->tl=ns;_tmp3B;}));_tmp3A->tl=_tmp32;_tmp3A;});
# 383
if(_tmp32 != 0){
if(_tmp32->tl != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n);({void*_tmp3C[1]={& _tmp3E};Cyc_Tcutil_terr(loc,({const char*_tmp3D="%s is ambiguous";_tag_dyneither(_tmp3D,sizeof(char),16);}),_tag_dyneither(_tmp3C,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp3F=(struct Cyc_List_List*)_tmp32->hd;_npop_handler(0);return _tmp3F;};}
# 388
if(_tmp30 == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp40=_cycalloc_atomic(sizeof(*_tmp40));_tmp40[0]=({struct Cyc_Dict_Absent_exn_struct _tmp41;_tmp41.tag=Cyc_Dict_Absent;_tmp41;});_tmp40;}));
# 391
_tmp30=Cyc_Tcenv_outer_namespace(_tmp30);}}
# 364
;_pop_region(temp);}
# 399
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp42=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp42,v);}
# 403
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp43=q;union Cyc_Absyn_Nmspace _tmp44;struct _dyneither_ptr*_tmp45;_LL1E: _tmp44=_tmp43->f1;_tmp45=_tmp43->f2;_LL1F:;{
union Cyc_Absyn_Nmspace _tmp46=_tmp44;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp48;struct _dyneither_ptr*_tmp49;struct Cyc_List_List*_tmp4A;switch((_tmp46.C_n).tag){case 4: _LL21: _LL22:
 goto _LL24;case 1: if((_tmp46.Rel_n).val == 0){_LL23: _LL24:
# 408
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp45,0);}else{_LL29: _tmp49=(struct _dyneither_ptr*)((_tmp46.Rel_n).val)->hd;_tmp4A=((_tmp46.Rel_n).val)->tl;_LL2A: {
# 414
struct Cyc_Dict_Dict _tmp4C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp49,_tmp4A))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp45);}}case 3: _LL25: _tmp48=(_tmp46.C_n).val;_LL26:
# 409
 _tmp47=_tmp48;goto _LL28;default: _LL27: _tmp47=(_tmp46.Abs_n).val;_LL28: {
# 411
struct Cyc_Dict_Dict _tmp4B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp47))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,_tmp45);}}_LL20:;};}
# 419
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4D=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4D,v);}
# 423
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp4E=q;union Cyc_Absyn_Nmspace _tmp4F;struct _dyneither_ptr*_tmp50;_LL2C: _tmp4F=_tmp4E->f1;_tmp50=_tmp4E->f2;_LL2D:;{
union Cyc_Absyn_Nmspace _tmp51=_tmp4F;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;struct _dyneither_ptr*_tmp54;struct Cyc_List_List*_tmp55;switch((_tmp51.C_n).tag){case 4: _LL2F: _LL30:
 goto _LL32;case 1: if((_tmp51.Rel_n).val == 0){_LL31: _LL32:
# 428
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp50,0);}else{_LL33: _tmp54=(struct _dyneither_ptr*)((_tmp51.Rel_n).val)->hd;_tmp55=((_tmp51.Rel_n).val)->tl;_LL34: {
# 430
struct Cyc_Dict_Dict _tmp56=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp54,_tmp55))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp56,_tmp50);}}case 3: _LL35: _tmp53=(_tmp51.C_n).val;_LL36:
 _tmp52=_tmp53;goto _LL38;default: _LL37: _tmp52=(_tmp51.Abs_n).val;_LL38: {
# 434
struct Cyc_Dict_Dict _tmp57=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp52))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp57,_tmp50);}}_LL2E:;};}
# 439
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 444
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp58=q;union Cyc_Absyn_Nmspace _tmp59;struct _dyneither_ptr*_tmp5A;_LL3A: _tmp59=_tmp58->f1;_tmp5A=_tmp58->f2;_LL3B:;{
union Cyc_Absyn_Nmspace _tmp5B=_tmp59;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct _dyneither_ptr*_tmp5E;struct Cyc_List_List*_tmp5F;switch((_tmp5B.C_n).tag){case 1: if((_tmp5B.Rel_n).val == 0){_LL3D: _LL3E: {
# 449
struct _handler_cons _tmp60;_push_handler(& _tmp60);{int _tmp62=0;if(setjmp(_tmp60.handler))_tmp62=1;if(!_tmp62){{struct Cyc_Absyn_Datatypedecl***_tmp64=({struct Cyc_Absyn_Datatypedecl***_tmp63=_region_malloc(r,sizeof(*_tmp63));_tmp63[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp5A,0);_tmp63;});_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp61=(void*)_exn_thrown;void*_tmp65=_tmp61;void*_tmp66;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp65)->tag == Cyc_Dict_Absent){_LL48: _LL49:
 return 0;}else{_LL4A: _tmp66=_tmp65;_LL4B:(int)_rethrow(_tmp66);}_LL47:;}};}}else{_LL41: _tmp5E=(struct _dyneither_ptr*)((_tmp5B.Rel_n).val)->hd;_tmp5F=((_tmp5B.Rel_n).val)->tl;_LL42: {
# 459
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp5E,_tmp5F);;_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp6C=_tmp6A;void*_tmp6D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6C)->tag == Cyc_Dict_Absent){_LL4D: _LL4E:
# 463
({void*_tmp6E=0;Cyc_Tcutil_terr(loc,({const char*_tmp6F="bad qualified name for @extensible datatype";_tag_dyneither(_tmp6F,sizeof(char),44);}),_tag_dyneither(_tmp6E,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp70=_cycalloc_atomic(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Dict_Absent_exn_struct _tmp71;_tmp71.tag=Cyc_Dict_Absent;_tmp71;});_tmp70;}));}else{_LL4F: _tmp6D=_tmp6C;_LL50:(int)_rethrow(_tmp6D);}_LL4C:;}};}{
# 466
struct Cyc_Dict_Dict _tmp72=ge->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp73=_region_malloc(r,sizeof(*_tmp73));_tmp73[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp72,_tmp5A);_tmp73;});};}}case 4: _LL3F: _LL40:
# 452
({void*_tmp67=0;Cyc_Tcutil_terr(loc,({const char*_tmp68="lookup_xdatatypedecl: impossible";_tag_dyneither(_tmp68,sizeof(char),33);}),_tag_dyneither(_tmp67,sizeof(void*),0));});
return 0;case 3: _LL43: _tmp5D=(_tmp5B.C_n).val;_LL44:
# 468
 _tmp5C=_tmp5D;goto _LL46;default: _LL45: _tmp5C=(_tmp5B.Abs_n).val;_LL46: {
# 470
struct Cyc_Dict_Dict _tmp74=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5C))->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp75=_region_malloc(r,sizeof(*_tmp75));_tmp75[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp74,_tmp5A);_tmp75;});}}_LL3C:;};}
# 475
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp76=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp76,v);}
# 479
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp77=q;union Cyc_Absyn_Nmspace _tmp78;struct _dyneither_ptr*_tmp79;_LL52: _tmp78=_tmp77->f1;_tmp79=_tmp77->f2;_LL53:;{
union Cyc_Absyn_Nmspace _tmp7A=_tmp78;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7C;struct _dyneither_ptr*_tmp7D;struct Cyc_List_List*_tmp7E;switch((_tmp7A.C_n).tag){case 4: _LL55: _LL56:
 goto _LL58;case 1: if((_tmp7A.Rel_n).val == 0){_LL57: _LL58:
# 484
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp79,0);}else{_LL59: _tmp7D=(struct _dyneither_ptr*)((_tmp7A.Rel_n).val)->hd;_tmp7E=((_tmp7A.Rel_n).val)->tl;_LL5A: {
# 486
struct Cyc_Dict_Dict _tmp7F=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp7D,_tmp7E))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F,_tmp79);}}case 3: _LL5B: _tmp7C=(_tmp7A.C_n).val;_LL5C:
 _tmp7B=_tmp7C;goto _LL5E;default: _LL5D: _tmp7B=(_tmp7A.Abs_n).val;_LL5E: {
# 490
struct Cyc_Dict_Dict _tmp80=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp7B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp80,_tmp79);}}_LL54:;};}
# 495
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp81=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,v);}
# 499
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp82=q;union Cyc_Absyn_Nmspace _tmp83;struct _dyneither_ptr*_tmp84;_LL60: _tmp83=_tmp82->f1;_tmp84=_tmp82->f2;_LL61:;{
union Cyc_Absyn_Nmspace _tmp85=_tmp83;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct _dyneither_ptr*_tmp88;struct Cyc_List_List*_tmp89;switch((_tmp85.C_n).tag){case 4: _LL63: _LL64:
 goto _LL66;case 1: if((_tmp85.Rel_n).val == 0){_LL65: _LL66:
# 504
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp84,0);}else{_LL67: _tmp88=(struct _dyneither_ptr*)((_tmp85.Rel_n).val)->hd;_tmp89=((_tmp85.Rel_n).val)->tl;_LL68: {
# 506
struct Cyc_Dict_Dict _tmp8A=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp88,_tmp89))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8A,_tmp84);}}case 3: _LL69: _tmp87=(_tmp85.C_n).val;_LL6A:
 _tmp86=_tmp87;goto _LL6C;default: _LL6B: _tmp86=(_tmp85.Abs_n).val;_LL6C: {
# 510
struct Cyc_Dict_Dict _tmp8B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp86))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,_tmp84);}}_LL62:;};}
# 517
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp8C=te->le;
if(_tmp8C == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp8C;}
# 523
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 527
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp8D=te->le;
if(_tmp8D != 0){
struct Cyc_Tcenv_Fenv*_tmp8E=_tmp8D;struct Cyc_Tcenv_Fenv*_tmp8F=_tmp8E;struct _RegionHandle*_tmp90;_LL6E: _tmp90=_tmp8F->fnrgn;_LL6F:;
return _tmp90;}
# 533
return Cyc_Tcenv_coerce_heap_region();}
# 536
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp91="put_fenv";_tag_dyneither(_tmp91,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp92=_region_malloc(l,sizeof(*_tmp92));_tmp92->ns=te->ns;_tmp92->ae=te->ae;_tmp92->le=fe;_tmp92->allow_valueof=te->allow_valueof;_tmp92->in_extern_c_include=te->in_extern_c_include;_tmp92;});}
# 541
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp93=_region_malloc(l,sizeof(*_tmp93));_tmp93->ns=te->ns;_tmp93->ae=te->ae;_tmp93->le=0;_tmp93->allow_valueof=te->allow_valueof;_tmp93->in_extern_c_include=te->in_extern_c_include;_tmp93;});}
# 545
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 547
struct Cyc_Tcenv_Fenv _tmp94=*f;struct Cyc_Tcenv_Fenv _tmp95=_tmp94;struct Cyc_Tcenv_SharedFenv*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_RgnOrder_RgnPO*_tmp98;const struct Cyc_Tcenv_Bindings*_tmp99;struct Cyc_Absyn_Stmt*_tmp9A;struct Cyc_Tcenv_CtrlEnv*_tmp9B;void*_tmp9C;void*_tmp9D;struct Cyc_Tcenv_FenvFlags _tmp9E;struct _RegionHandle*_tmp9F;_LL71: _tmp96=_tmp95.shared;_tmp97=_tmp95.type_vars;_tmp98=_tmp95.region_order;_tmp99=_tmp95.locals;_tmp9A=_tmp95.encloser;_tmp9B=_tmp95.ctrl_env;_tmp9C=_tmp95.capability;_tmp9D=_tmp95.curr_rgn;_tmp9E=_tmp95.flags;_tmp9F=_tmp95.fnrgn;_LL72:;
# 549
return({struct Cyc_Tcenv_Fenv*_tmpA0=_region_malloc(l,sizeof(*_tmpA0));_tmpA0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp96;_tmpA0->type_vars=(struct Cyc_List_List*)_tmp97;_tmpA0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp98;_tmpA0->locals=(const struct Cyc_Tcenv_Bindings*)_tmp99;_tmpA0->encloser=(struct Cyc_Absyn_Stmt*)_tmp9A;_tmpA0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp9B;_tmpA0->capability=(void*)_tmp9C;_tmpA0->curr_rgn=(void*)_tmp9D;_tmpA0->flags=(struct Cyc_Tcenv_FenvFlags)_tmp9E;_tmpA0->fnrgn=(struct _RegionHandle*)l;_tmpA0;});}
# 553
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 555
struct Cyc_Tcenv_Fenv _tmpA1=*f;struct Cyc_Tcenv_Fenv _tmpA2=_tmpA1;struct Cyc_Tcenv_SharedFenv*_tmpA3;struct Cyc_List_List*_tmpA4;struct Cyc_RgnOrder_RgnPO*_tmpA5;const struct Cyc_Tcenv_Bindings*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA7;struct Cyc_Tcenv_CtrlEnv*_tmpA8;void*_tmpA9;void*_tmpAA;struct Cyc_Tcenv_FenvFlags _tmpAB;_LL74: _tmpA3=_tmpA2.shared;_tmpA4=_tmpA2.type_vars;_tmpA5=_tmpA2.region_order;_tmpA6=_tmpA2.locals;_tmpA7=_tmpA2.encloser;_tmpA8=_tmpA2.ctrl_env;_tmpA9=_tmpA2.capability;_tmpAA=_tmpA2.curr_rgn;_tmpAB=_tmpA2.flags;_LL75:;{
# 558
struct Cyc_Tcenv_CtrlEnv _tmpAC=*_tmpA8;struct Cyc_Tcenv_CtrlEnv _tmpAD=_tmpAC;struct _RegionHandle*_tmpAE;void*_tmpAF;void*_tmpB0;const struct _tuple10*_tmpB1;void*_tmpB2;int _tmpB3;_LL77: _tmpAE=_tmpAD.ctrl_rgn;_tmpAF=_tmpAD.continue_stmt;_tmpB0=_tmpAD.break_stmt;_tmpB1=_tmpAD.fallthru_clause;_tmpB2=_tmpAD.next_stmt;_tmpB3=_tmpAD.try_depth;_LL78:;{
struct Cyc_Tcenv_CtrlEnv*_tmpB4=({struct Cyc_Tcenv_CtrlEnv*_tmpB6=_region_malloc(l,sizeof(*_tmpB6));_tmpB6->ctrl_rgn=_tmpAE;_tmpB6->continue_stmt=_tmpAF;_tmpB6->break_stmt=_tmpB0;_tmpB6->fallthru_clause=_tmpB1;_tmpB6->next_stmt=_tmpB2;_tmpB6->try_depth=_tmpB3;_tmpB6;});
# 561
return({struct Cyc_Tcenv_Fenv*_tmpB5=_region_malloc(l,sizeof(*_tmpB5));_tmpB5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA3;_tmpB5->type_vars=(struct Cyc_List_List*)_tmpA4;_tmpB5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA5;_tmpB5->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA6;_tmpB5->encloser=(struct Cyc_Absyn_Stmt*)_tmpA7;_tmpB5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB4;_tmpB5->capability=(void*)_tmpA9;_tmpB5->curr_rgn=(void*)_tmpAA;_tmpB5->flags=(struct Cyc_Tcenv_FenvFlags)_tmpAB;_tmpB5->fnrgn=(struct _RegionHandle*)l;_tmpB5;});};};}
# 567
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpB7=Cyc_Tcenv_get_fenv(te,({const char*_tmpBD="return_typ";_tag_dyneither(_tmpBD,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmpB8=_tmpB7;struct Cyc_Tcenv_SharedFenv*_tmpB9;_LL7A: _tmpB9=_tmpB8->shared;_LL7B:;{
struct Cyc_Tcenv_SharedFenv _tmpBA=*_tmpB9;struct Cyc_Tcenv_SharedFenv _tmpBB=_tmpBA;void*_tmpBC;_LL7D: _tmpBC=_tmpBB.return_typ;_LL7E:;
return _tmpBC;};}
# 573
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpBE=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpBF=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpBE));struct Cyc_Tcenv_Fenv _tmpC0=_tmpBF;struct Cyc_List_List*_tmpC1;_LL80: _tmpC1=_tmpC0.type_vars;_LL81:;
return _tmpC1;};}
# 580
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC2;});}
# 584
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 586
struct Cyc_Tcenv_Fenv*_tmpC3=
# 588
Cyc_Tcenv_get_fenv(te,({const char*_tmpD1="add_type_vars";_tag_dyneither(_tmpD1,sizeof(char),14);}));
# 586
struct Cyc_Tcenv_Fenv*_tmpC4=_tmpC3;struct Cyc_Tcenv_SharedFenv*_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_RgnOrder_RgnPO*_tmpC7;const struct Cyc_Tcenv_Bindings*_tmpC8;struct Cyc_Absyn_Stmt*_tmpC9;struct Cyc_Tcenv_CtrlEnv*_tmpCA;void*_tmpCB;void*_tmpCC;struct Cyc_Tcenv_FenvFlags _tmpCD;_LL83: _tmpC5=_tmpC4->shared;_tmpC6=_tmpC4->type_vars;_tmpC7=_tmpC4->region_order;_tmpC8=_tmpC4->locals;_tmpC9=_tmpC4->encloser;_tmpCA=_tmpC4->ctrl_env;_tmpCB=_tmpC4->capability;_tmpCC=_tmpC4->curr_rgn;_tmpCD=_tmpC4->flags;_LL84:;
# 590
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpCE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC6);
Cyc_Tcutil_check_unique_tvars(loc,_tmpCE);{
struct Cyc_Tcenv_Fenv*_tmpCF=({struct Cyc_Tcenv_Fenv*_tmpD0=_region_malloc(r,sizeof(*_tmpD0));_tmpD0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC5;_tmpD0->type_vars=(struct Cyc_List_List*)_tmpCE;_tmpD0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC7;_tmpD0->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC8;_tmpD0->encloser=(struct Cyc_Absyn_Stmt*)_tmpC9;_tmpD0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpCA;_tmpD0->capability=(void*)_tmpCB;_tmpD0->curr_rgn=(void*)_tmpCC;_tmpD0->flags=(struct Cyc_Tcenv_FenvFlags)_tmpCD;_tmpD0->fnrgn=(struct _RegionHandle*)r;_tmpD0;});
# 595
return Cyc_Tcenv_put_fenv(r,te,_tmpCF);};};}
# 598
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD2=te->le;
if(_tmpD2 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 603
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpD2);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 608
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 610
struct _dyneither_ptr*_tmpD3=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpD4=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpD5=_tmpD4;if((_tmpD5.Loc_n).tag == 4){_LL86: _LL87:
 goto _LL85;}else{_LL88: _LL89:
# 615
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Core_Impossible_exn_struct _tmpD7;_tmpD7.tag=Cyc_Core_Impossible;_tmpD7.f1=({const char*_tmpD8="add_local_var: called with Rel_n";_tag_dyneither(_tmpD8,sizeof(char),33);});_tmpD7;});_tmpD6;}));}_LL85:;}
# 617
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpD9;_push_handler(& _tmpD9);{int _tmpDB=0;if(setjmp(_tmpD9.handler))_tmpDB=1;if(!_tmpDB){
{void*_tmpDC=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpDD=_tmpDC;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD)->f1)){case 4: _LL8B: _LL8C:
# 621
({void*_tmpDE=0;Cyc_Tcutil_warn(loc,({const char*_tmpDF="declaration hides local";_tag_dyneither(_tmpDF,sizeof(char),24);}),_tag_dyneither(_tmpDE,sizeof(void*),0));});goto _LL8A;case 1: _LL8D: _LL8E:
({void*_tmpE0=0;Cyc_Tcutil_warn(loc,({const char*_tmpE1="declaration hides global";_tag_dyneither(_tmpE1,sizeof(char),25);}),_tag_dyneither(_tmpE0,sizeof(void*),0));});goto _LL8A;case 5: _LL8F: _LL90:
({void*_tmpE2=0;Cyc_Tcutil_warn(loc,({const char*_tmpE3="declaration hides pattern variable";_tag_dyneither(_tmpE3,sizeof(char),35);}),_tag_dyneither(_tmpE2,sizeof(void*),0));});goto _LL8A;case 3: _LL91: _LL92:
({void*_tmpE4=0;Cyc_Tcutil_warn(loc,({const char*_tmpE5="declaration hides parameter";_tag_dyneither(_tmpE5,sizeof(char),28);}),_tag_dyneither(_tmpE4,sizeof(void*),0));});goto _LL8A;default: goto _LL93;}else{_LL93: _LL94:
 goto _LL8A;}_LL8A:;}
# 619
;_pop_handler();}else{void*_tmpDA=(void*)_exn_thrown;void*_tmpE6=_tmpDA;void*_tmpE7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpE6)->tag == Cyc_Dict_Absent){_LL96: _LL97:
# 627
 goto _LL95;}else{_LL98: _tmpE7=_tmpE6;_LL99:(int)_rethrow(_tmpE7);}_LL95:;}};}{
struct Cyc_Tcenv_Fenv*_tmpE8=
# 630
Cyc_Tcenv_get_fenv(te,({const char*_tmpFA="add_local_var";_tag_dyneither(_tmpFA,sizeof(char),14);}));
# 628
struct Cyc_Tcenv_Fenv*_tmpE9=_tmpE8;struct Cyc_Tcenv_SharedFenv*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_RgnOrder_RgnPO*_tmpEC;const struct Cyc_Tcenv_Bindings*_tmpED;struct Cyc_Absyn_Stmt*_tmpEE;struct Cyc_Tcenv_CtrlEnv*_tmpEF;void*_tmpF0;void*_tmpF1;struct Cyc_Tcenv_FenvFlags _tmpF2;_LL9B: _tmpEA=_tmpE9->shared;_tmpEB=_tmpE9->type_vars;_tmpEC=_tmpE9->region_order;_tmpED=_tmpE9->locals;_tmpEE=_tmpE9->encloser;_tmpEF=_tmpE9->ctrl_env;_tmpF0=_tmpE9->capability;_tmpF1=_tmpE9->curr_rgn;_tmpF2=_tmpE9->flags;_LL9C:;{
# 631
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF3=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpF9;_tmpF9.tag=4;_tmpF9.f1=vd;_tmpF9;});_tmpF8;});
struct Cyc_Tcenv_Bindings*_tmpF4=({struct Cyc_Tcenv_Bindings*_tmpF7=_region_malloc(r,sizeof(*_tmpF7));_tmpF7->v=_tmpD3;_tmpF7->b=(void*)_tmpF3;_tmpF7->tl=_tmpED;_tmpF7;});
struct Cyc_Tcenv_Fenv*_tmpF5=({struct Cyc_Tcenv_Fenv*_tmpF6=_region_malloc(r,sizeof(*_tmpF6));_tmpF6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpEA;_tmpF6->type_vars=(struct Cyc_List_List*)_tmpEB;_tmpF6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEC;_tmpF6->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpF4);_tmpF6->encloser=(struct Cyc_Absyn_Stmt*)_tmpEE;_tmpF6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpEF;_tmpF6->capability=(void*)_tmpF0;_tmpF6->curr_rgn=(void*)_tmpF1;_tmpF6->flags=(struct Cyc_Tcenv_FenvFlags)_tmpF2;_tmpF6->fnrgn=(struct _RegionHandle*)r;_tmpF6;});
# 635
return Cyc_Tcenv_put_fenv(r,te,_tmpF5);};};}
# 638
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 640
struct Cyc_Tcenv_Fenv*_tmpFB=te->le;
if(_tmpFB == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpFC=*_tmpFB;struct Cyc_Tcenv_Fenv _tmpFD=_tmpFC;struct Cyc_Tcenv_SharedFenv*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_RgnOrder_RgnPO*_tmp100;const struct Cyc_Tcenv_Bindings*_tmp101;struct Cyc_Absyn_Stmt*_tmp102;struct Cyc_Tcenv_CtrlEnv*_tmp103;void*_tmp104;void*_tmp105;int _tmp106;int _tmp107;int _tmp108;_LL9E: _tmpFE=_tmpFD.shared;_tmpFF=_tmpFD.type_vars;_tmp100=_tmpFD.region_order;_tmp101=_tmpFD.locals;_tmp102=_tmpFD.encloser;_tmp103=_tmpFD.ctrl_env;_tmp104=_tmpFD.capability;_tmp105=_tmpFD.curr_rgn;_tmp106=(_tmpFD.flags).in_notreadctxt;_tmp107=(_tmpFD.flags).in_lhs;_tmp108=(_tmpFD.flags).abstract_ok;_LL9F:;{
# 645
struct Cyc_Tcenv_Fenv*_tmp109=({struct Cyc_Tcenv_Fenv*_tmp10A=_region_malloc(r,sizeof(*_tmp10A));_tmp10A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFE;_tmp10A->type_vars=(struct Cyc_List_List*)_tmpFF;_tmp10A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp100;_tmp10A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp101;_tmp10A->encloser=(struct Cyc_Absyn_Stmt*)_tmp102;_tmp10A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp103;_tmp10A->capability=(void*)_tmp104;_tmp10A->curr_rgn=(void*)_tmp105;_tmp10A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp10A->flags).in_notreadctxt=_tmp106;(_tmp10A->flags).in_lhs=_tmp107;(_tmp10A->flags).in_new=(int)status;(_tmp10A->flags).abstract_ok=_tmp108;_tmp10A->flags;});_tmp10A->fnrgn=(struct _RegionHandle*)r;_tmp10A;});
# 648
return Cyc_Tcenv_put_fenv(r,te,_tmp109);};};}
# 651
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10B=te->le;
if(_tmp10B == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp10C=_tmp10B;struct Cyc_Tcenv_Fenv*_tmp10D=_tmp10C;int _tmp10E;_LLA1: _tmp10E=(_tmp10D->flags).in_new;_LLA2:;
return(enum Cyc_Tcenv_NewStatus)_tmp10E;};}
# 658
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp110=_tmp10F;struct Cyc_Tcenv_Fenv*_tmp111=_tmp110;int _tmp112;_LLA4: _tmp112=(_tmp111->flags).abstract_ok;_LLA5:;
return _tmp112;};}
# 665
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 667
struct Cyc_Tcenv_Fenv*_tmp113=te->le;
if(_tmp113 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp114=*_tmp113;struct Cyc_Tcenv_Fenv _tmp115=_tmp114;struct Cyc_Tcenv_SharedFenv*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_RgnOrder_RgnPO*_tmp118;const struct Cyc_Tcenv_Bindings*_tmp119;struct Cyc_Absyn_Stmt*_tmp11A;struct Cyc_Tcenv_CtrlEnv*_tmp11B;void*_tmp11C;void*_tmp11D;int _tmp11E;int _tmp11F;int _tmp120;_LLA7: _tmp116=_tmp115.shared;_tmp117=_tmp115.type_vars;_tmp118=_tmp115.region_order;_tmp119=_tmp115.locals;_tmp11A=_tmp115.encloser;_tmp11B=_tmp115.ctrl_env;_tmp11C=_tmp115.capability;_tmp11D=_tmp115.curr_rgn;_tmp11E=(_tmp115.flags).in_notreadctxt;_tmp11F=(_tmp115.flags).in_lhs;_tmp120=(_tmp115.flags).in_new;_LLA8:;{
# 672
struct Cyc_Tcenv_Fenv*_tmp121=({struct Cyc_Tcenv_Fenv*_tmp122=_region_malloc(r,sizeof(*_tmp122));_tmp122->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp116;_tmp122->type_vars=(struct Cyc_List_List*)_tmp117;_tmp122->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp118;_tmp122->locals=(const struct Cyc_Tcenv_Bindings*)_tmp119;_tmp122->encloser=(struct Cyc_Absyn_Stmt*)_tmp11A;_tmp122->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp11B;_tmp122->capability=(void*)_tmp11C;_tmp122->curr_rgn=(void*)_tmp11D;_tmp122->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp122->flags).in_notreadctxt=_tmp11E;(_tmp122->flags).in_lhs=_tmp11F;(_tmp122->flags).in_new=_tmp120;(_tmp122->flags).abstract_ok=1;_tmp122->flags;});_tmp122->fnrgn=(struct _RegionHandle*)r;_tmp122;});
# 675
return Cyc_Tcenv_put_fenv(r,te,_tmp121);};};}
# 678
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 680
struct Cyc_Tcenv_Fenv*_tmp123=te->le;
if(_tmp123 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp124=*_tmp123;struct Cyc_Tcenv_Fenv _tmp125=_tmp124;struct Cyc_Tcenv_SharedFenv*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_RgnOrder_RgnPO*_tmp128;const struct Cyc_Tcenv_Bindings*_tmp129;struct Cyc_Absyn_Stmt*_tmp12A;struct Cyc_Tcenv_CtrlEnv*_tmp12B;void*_tmp12C;void*_tmp12D;int _tmp12E;int _tmp12F;int _tmp130;_LLAA: _tmp126=_tmp125.shared;_tmp127=_tmp125.type_vars;_tmp128=_tmp125.region_order;_tmp129=_tmp125.locals;_tmp12A=_tmp125.encloser;_tmp12B=_tmp125.ctrl_env;_tmp12C=_tmp125.capability;_tmp12D=_tmp125.curr_rgn;_tmp12E=(_tmp125.flags).in_notreadctxt;_tmp12F=(_tmp125.flags).in_lhs;_tmp130=(_tmp125.flags).in_new;_LLAB:;{
# 685
struct Cyc_Tcenv_Fenv*_tmp131=({struct Cyc_Tcenv_Fenv*_tmp132=_region_malloc(r,sizeof(*_tmp132));_tmp132->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp126;_tmp132->type_vars=(struct Cyc_List_List*)_tmp127;_tmp132->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp128;_tmp132->locals=(const struct Cyc_Tcenv_Bindings*)_tmp129;_tmp132->encloser=(struct Cyc_Absyn_Stmt*)_tmp12A;_tmp132->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12B;_tmp132->capability=(void*)_tmp12C;_tmp132->curr_rgn=(void*)_tmp12D;_tmp132->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp132->flags).in_notreadctxt=_tmp12E;(_tmp132->flags).in_lhs=_tmp12F;(_tmp132->flags).in_new=_tmp130;(_tmp132->flags).abstract_ok=0;_tmp132->flags;});_tmp132->fnrgn=(struct _RegionHandle*)r;_tmp132;});
# 688
return Cyc_Tcenv_put_fenv(r,te,_tmp131);};};}
# 691
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp133=te->le;
if(_tmp133 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp134=*_tmp133;struct Cyc_Tcenv_Fenv _tmp135=_tmp134;struct Cyc_Tcenv_SharedFenv*_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_RgnOrder_RgnPO*_tmp138;const struct Cyc_Tcenv_Bindings*_tmp139;struct Cyc_Absyn_Stmt*_tmp13A;struct Cyc_Tcenv_CtrlEnv*_tmp13B;void*_tmp13C;void*_tmp13D;int _tmp13E;int _tmp13F;int _tmp140;_LLAD: _tmp136=_tmp135.shared;_tmp137=_tmp135.type_vars;_tmp138=_tmp135.region_order;_tmp139=_tmp135.locals;_tmp13A=_tmp135.encloser;_tmp13B=_tmp135.ctrl_env;_tmp13C=_tmp135.capability;_tmp13D=_tmp135.curr_rgn;_tmp13E=(_tmp135.flags).in_lhs;_tmp13F=(_tmp135.flags).in_new;_tmp140=(_tmp135.flags).abstract_ok;_LLAE:;{
# 697
struct Cyc_Tcenv_Fenv*_tmp141=({struct Cyc_Tcenv_Fenv*_tmp142=_region_malloc(r,sizeof(*_tmp142));_tmp142->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp136;_tmp142->type_vars=(struct Cyc_List_List*)_tmp137;_tmp142->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp138;_tmp142->locals=(const struct Cyc_Tcenv_Bindings*)_tmp139;_tmp142->encloser=(struct Cyc_Absyn_Stmt*)_tmp13A;_tmp142->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13B;_tmp142->capability=(void*)_tmp13C;_tmp142->curr_rgn=(void*)_tmp13D;_tmp142->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp142->flags).in_notreadctxt=1;(_tmp142->flags).in_lhs=_tmp13E;(_tmp142->flags).in_new=_tmp13F;(_tmp142->flags).abstract_ok=_tmp140;_tmp142->flags;});_tmp142->fnrgn=(struct _RegionHandle*)r;_tmp142;});
# 700
return Cyc_Tcenv_put_fenv(r,te,_tmp141);};};}
# 703
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp143=te->le;
if(_tmp143 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp144=*_tmp143;struct Cyc_Tcenv_Fenv _tmp145=_tmp144;struct Cyc_Tcenv_SharedFenv*_tmp146;struct Cyc_List_List*_tmp147;struct Cyc_RgnOrder_RgnPO*_tmp148;const struct Cyc_Tcenv_Bindings*_tmp149;struct Cyc_Absyn_Stmt*_tmp14A;struct Cyc_Tcenv_CtrlEnv*_tmp14B;void*_tmp14C;void*_tmp14D;int _tmp14E;int _tmp14F;int _tmp150;_LLB0: _tmp146=_tmp145.shared;_tmp147=_tmp145.type_vars;_tmp148=_tmp145.region_order;_tmp149=_tmp145.locals;_tmp14A=_tmp145.encloser;_tmp14B=_tmp145.ctrl_env;_tmp14C=_tmp145.capability;_tmp14D=_tmp145.curr_rgn;_tmp14E=(_tmp145.flags).in_lhs;_tmp14F=(_tmp145.flags).in_new;_tmp150=(_tmp145.flags).abstract_ok;_LLB1:;{
# 709
struct Cyc_Tcenv_Fenv*_tmp151=({struct Cyc_Tcenv_Fenv*_tmp152=_region_malloc(r,sizeof(*_tmp152));_tmp152->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp146;_tmp152->type_vars=(struct Cyc_List_List*)_tmp147;_tmp152->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp148;_tmp152->locals=(const struct Cyc_Tcenv_Bindings*)_tmp149;_tmp152->encloser=(struct Cyc_Absyn_Stmt*)_tmp14A;_tmp152->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14B;_tmp152->capability=(void*)_tmp14C;_tmp152->curr_rgn=(void*)_tmp14D;_tmp152->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp152->flags).in_notreadctxt=0;(_tmp152->flags).in_lhs=_tmp14E;(_tmp152->flags).in_new=_tmp14F;(_tmp152->flags).abstract_ok=_tmp150;_tmp152->flags;});_tmp152->fnrgn=(struct _RegionHandle*)r;_tmp152;});
# 712
return Cyc_Tcenv_put_fenv(r,te,_tmp151);};};}
# 715
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp153=te->le;
if(_tmp153 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp154=_tmp153;struct Cyc_Tcenv_Fenv*_tmp155=_tmp154;int _tmp156;_LLB3: _tmp156=(_tmp155->flags).in_notreadctxt;_LLB4:;
return _tmp156;};}
# 722
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp157=te->le;
if(_tmp157 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp158=*_tmp157;struct Cyc_Tcenv_Fenv _tmp159=_tmp158;struct Cyc_Tcenv_SharedFenv*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_RgnOrder_RgnPO*_tmp15C;const struct Cyc_Tcenv_Bindings*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Tcenv_CtrlEnv*_tmp15F;void*_tmp160;void*_tmp161;int _tmp162;int _tmp163;int _tmp164;_LLB6: _tmp15A=_tmp159.shared;_tmp15B=_tmp159.type_vars;_tmp15C=_tmp159.region_order;_tmp15D=_tmp159.locals;_tmp15E=_tmp159.encloser;_tmp15F=_tmp159.ctrl_env;_tmp160=_tmp159.capability;_tmp161=_tmp159.curr_rgn;_tmp162=(_tmp159.flags).in_notreadctxt;_tmp163=(_tmp159.flags).in_new;_tmp164=(_tmp159.flags).abstract_ok;_LLB7:;{
# 728
struct Cyc_Tcenv_Fenv*_tmp165=({struct Cyc_Tcenv_Fenv*_tmp166=_region_malloc(r,sizeof(*_tmp166));_tmp166->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15A;_tmp166->type_vars=(struct Cyc_List_List*)_tmp15B;_tmp166->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C;_tmp166->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15D;_tmp166->encloser=(struct Cyc_Absyn_Stmt*)_tmp15E;_tmp166->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F;_tmp166->capability=(void*)_tmp160;_tmp166->curr_rgn=(void*)_tmp161;_tmp166->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp166->flags).in_notreadctxt=_tmp162;(_tmp166->flags).in_lhs=1;(_tmp166->flags).in_new=_tmp163;(_tmp166->flags).abstract_ok=_tmp164;_tmp166->flags;});_tmp166->fnrgn=(struct _RegionHandle*)r;_tmp166;});
# 731
return Cyc_Tcenv_put_fenv(r,te,_tmp165);};};}
# 734
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp167=te->le;
if(_tmp167 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp168=*_tmp167;struct Cyc_Tcenv_Fenv _tmp169=_tmp168;struct Cyc_Tcenv_SharedFenv*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_RgnOrder_RgnPO*_tmp16C;const struct Cyc_Tcenv_Bindings*_tmp16D;struct Cyc_Absyn_Stmt*_tmp16E;struct Cyc_Tcenv_CtrlEnv*_tmp16F;void*_tmp170;void*_tmp171;int _tmp172;int _tmp173;int _tmp174;_LLB9: _tmp16A=_tmp169.shared;_tmp16B=_tmp169.type_vars;_tmp16C=_tmp169.region_order;_tmp16D=_tmp169.locals;_tmp16E=_tmp169.encloser;_tmp16F=_tmp169.ctrl_env;_tmp170=_tmp169.capability;_tmp171=_tmp169.curr_rgn;_tmp172=(_tmp169.flags).in_notreadctxt;_tmp173=(_tmp169.flags).in_new;_tmp174=(_tmp169.flags).abstract_ok;_LLBA:;{
# 740
struct Cyc_Tcenv_Fenv*_tmp175=({struct Cyc_Tcenv_Fenv*_tmp176=_region_malloc(r,sizeof(*_tmp176));_tmp176->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp16A;_tmp176->type_vars=(struct Cyc_List_List*)_tmp16B;_tmp176->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp16C;_tmp176->locals=(const struct Cyc_Tcenv_Bindings*)_tmp16D;_tmp176->encloser=(struct Cyc_Absyn_Stmt*)_tmp16E;_tmp176->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16F;_tmp176->capability=(void*)_tmp170;_tmp176->curr_rgn=(void*)_tmp171;_tmp176->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp176->flags).in_notreadctxt=_tmp172;(_tmp176->flags).in_lhs=0;(_tmp176->flags).in_new=_tmp173;(_tmp176->flags).abstract_ok=_tmp174;_tmp176->flags;});_tmp176->fnrgn=(struct _RegionHandle*)r;_tmp176;});
# 743
return Cyc_Tcenv_put_fenv(r,te,_tmp175);};};}
# 746
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp177=te->le;
if(_tmp177 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp178=_tmp177;struct Cyc_Tcenv_Fenv*_tmp179=_tmp178;int _tmp17A;_LLBC: _tmp17A=(_tmp179->flags).in_lhs;_LLBD:;
return _tmp17A;};}
# 754
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 756
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp17B;_push_handler(& _tmp17B);{int _tmp17D=0;if(setjmp(_tmp17B.handler))_tmp17D=1;if(!_tmp17D){
{void*_tmp17E=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp17F=_tmp17E;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17F)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17F)->f1)){case 4: _LLBF: _LLC0:
# 760
({void*_tmp180=0;Cyc_Tcutil_warn(loc,({const char*_tmp181="pattern variable hides local";_tag_dyneither(_tmp181,sizeof(char),29);}),_tag_dyneither(_tmp180,sizeof(void*),0));});goto _LLBE;case 1: _LLC1: _LLC2:
({void*_tmp182=0;Cyc_Tcutil_warn(loc,({const char*_tmp183="pattern variable hides global";_tag_dyneither(_tmp183,sizeof(char),30);}),_tag_dyneither(_tmp182,sizeof(void*),0));});goto _LLBE;case 5: _LLC3: _LLC4:
({void*_tmp184=0;Cyc_Tcutil_warn(loc,({const char*_tmp185="pattern variable hides pattern variable";_tag_dyneither(_tmp185,sizeof(char),40);}),_tag_dyneither(_tmp184,sizeof(void*),0));});goto _LLBE;case 3: _LLC5: _LLC6:
({void*_tmp186=0;Cyc_Tcutil_warn(loc,({const char*_tmp187="pattern variable hides parameter";_tag_dyneither(_tmp187,sizeof(char),33);}),_tag_dyneither(_tmp186,sizeof(void*),0));});goto _LLBE;default: goto _LLC7;}else{_LLC7: _LLC8:
 goto _LLBE;}_LLBE:;}
# 758
;_pop_handler();}else{void*_tmp17C=(void*)_exn_thrown;void*_tmp188=_tmp17C;void*_tmp189;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp188)->tag == Cyc_Dict_Absent){_LLCA: _LLCB:
# 766
 goto _LLC9;}else{_LLCC: _tmp189=_tmp188;_LLCD:(int)_rethrow(_tmp189);}_LLC9:;}};}{
struct _dyneither_ptr*_tmp18A=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmp18B=
# 770
Cyc_Tcenv_get_fenv(te,({const char*_tmp19D="add_pat_var";_tag_dyneither(_tmp19D,sizeof(char),12);}));
# 768
struct Cyc_Tcenv_Fenv*_tmp18C=_tmp18B;struct Cyc_Tcenv_SharedFenv*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_RgnOrder_RgnPO*_tmp18F;const struct Cyc_Tcenv_Bindings*_tmp190;struct Cyc_Absyn_Stmt*_tmp191;struct Cyc_Tcenv_CtrlEnv*_tmp192;void*_tmp193;void*_tmp194;struct Cyc_Tcenv_FenvFlags _tmp195;_LLCF: _tmp18D=_tmp18C->shared;_tmp18E=_tmp18C->type_vars;_tmp18F=_tmp18C->region_order;_tmp190=_tmp18C->locals;_tmp191=_tmp18C->encloser;_tmp192=_tmp18C->ctrl_env;_tmp193=_tmp18C->capability;_tmp194=_tmp18C->curr_rgn;_tmp195=_tmp18C->flags;_LLD0:;{
# 771
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp196=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp19C;_tmp19C.tag=5;_tmp19C.f1=vd;_tmp19C;});_tmp19B;});
struct Cyc_Tcenv_Bindings*_tmp197=({struct Cyc_Tcenv_Bindings*_tmp19A=_region_malloc(r,sizeof(*_tmp19A));_tmp19A->v=_tmp18A;_tmp19A->b=(void*)_tmp196;_tmp19A->tl=_tmp190;_tmp19A;});
struct Cyc_Tcenv_Fenv*_tmp198=({struct Cyc_Tcenv_Fenv*_tmp199=_region_malloc(r,sizeof(*_tmp199));_tmp199->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp18D;_tmp199->type_vars=(struct Cyc_List_List*)_tmp18E;_tmp199->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp18F;_tmp199->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp197);_tmp199->encloser=(struct Cyc_Absyn_Stmt*)_tmp191;_tmp199->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp192;_tmp199->capability=(void*)_tmp193;_tmp199->curr_rgn=(void*)_tmp194;_tmp199->flags=(struct Cyc_Tcenv_FenvFlags)_tmp195;_tmp199->fnrgn=(struct _RegionHandle*)r;_tmp199;});
# 775
return Cyc_Tcenv_put_fenv(r,te,_tmp198);};};}
# 778
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp19E=te->le;
struct _tuple0*_tmp19F=q;union Cyc_Absyn_Nmspace _tmp1A0;struct _dyneither_ptr*_tmp1A1;_LLD2: _tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;_LLD3:;{
union Cyc_Absyn_Nmspace _tmp1A2=_tmp1A0;switch((_tmp1A2.Rel_n).tag){case 4: _LLD5: _LLD6:
# 783
 if(_tmp19E == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp1A3=_cycalloc_atomic(sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Dict_Absent_exn_struct _tmp1A4;_tmp1A4.tag=Cyc_Dict_Absent;_tmp1A4;});_tmp1A3;}));
goto _LLD8;case 1: if((_tmp1A2.Rel_n).val == 0){_LLD7: if(_tmp19E != 0){_LLD8: {
# 786
struct Cyc_Tcenv_Fenv*_tmp1A5=_tmp19E;struct Cyc_Tcenv_Fenv*_tmp1A6=_tmp1A5;const struct Cyc_Tcenv_Bindings*_tmp1A7;_LLDC: _tmp1A7=_tmp1A6->locals;_LLDD:;{
# 788
struct _handler_cons _tmp1A8;_push_handler(& _tmp1A8);{int _tmp1AA=0;if(setjmp(_tmp1A8.handler))_tmp1AA=1;if(!_tmp1AA){
{void*_tmp1AD=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp1AB=_region_malloc(r,sizeof(*_tmp1AB));_tmp1AB[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=Cyc_Tcenv_lookup_binding(_tmp1A7,_tmp1A1);_tmp1AC;});_tmp1AB;});_npop_handler(0);return _tmp1AD;};_pop_handler();}else{void*_tmp1A9=(void*)_exn_thrown;void*_tmp1AE=_tmp1A9;void*_tmp1AF;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1AE)->tag == Cyc_Tcenv_NoBinding){_LLDF: _LLE0:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LLE1: _tmp1AF=_tmp1AE;_LLE2:(int)_rethrow(_tmp1AF);}_LLDE:;}};};}}else{goto _LLD9;}}else{goto _LLD9;}default: _LLD9: _LLDA: {
# 792
struct _handler_cons _tmp1B0;_push_handler(& _tmp1B0);{int _tmp1B2=0;if(setjmp(_tmp1B0.handler))_tmp1B2=1;if(!_tmp1B2){{void*_tmp1B3=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1B3;};_pop_handler();}else{void*_tmp1B1=(void*)_exn_thrown;void*_tmp1B4=_tmp1B1;void*_tmp1B5;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B4)->tag == Cyc_Dict_Absent){_LLE4: _LLE5:
# 796
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp1B6=_region_malloc(r,sizeof(*_tmp1B6));_tmp1B6[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1B7;_tmp1B7.tag=0;_tmp1B7.f1=(void*)& Cyc_Absyn_Unresolved_b_val;_tmp1B7;});_tmp1B6;});}else{_LLE6: _tmp1B5=_tmp1B4;_LLE7:(int)_rethrow(_tmp1B5);}_LLE3:;}};}}_LLD4:;};}
# 801
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv*_tmp1B8=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1C2="process_continue";_tag_dyneither(_tmp1C2,sizeof(char),17);}));
# 802
struct Cyc_Tcenv_Fenv*_tmp1B9=_tmp1B8;struct Cyc_Tcenv_CtrlEnv*_tmp1BA;_LLE9: _tmp1BA=_tmp1B9->ctrl_env;_LLEA:;{
# 804
void*_tmp1BB=_tmp1BA->continue_stmt;void*_tmp1BC=_tmp1BB;struct Cyc_Absyn_Stmt*_tmp1BD;switch(*((int*)_tmp1BC)){case 3: _LLEC: _tmp1BD=((struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1BC)->f1;_LLED:
# 806
 _tmp1BD->non_local_preds=({struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=s;_tmp1BE->tl=_tmp1BD->non_local_preds;_tmp1BE;});
*sopt=_tmp1BD;
return;case 0: _LLEE: _LLEF:
({void*_tmp1BF=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp1C0="continue not in a loop";_tag_dyneither(_tmp1C0,sizeof(char),23);}),_tag_dyneither(_tmp1BF,sizeof(void*),0));});return;case 1: _LLF0: _LLF1:
 goto _LLF3;default: _LLF2: _LLF3:
((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp1C1="bad continue destination";_tag_dyneither(_tmp1C1,sizeof(char),25);}));}_LLEB:;};}
# 814
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv*_tmp1C3=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1CF="process_break";_tag_dyneither(_tmp1CF,sizeof(char),14);}));
# 815
struct Cyc_Tcenv_Fenv*_tmp1C4=_tmp1C3;struct Cyc_Tcenv_CtrlEnv*_tmp1C5;struct Cyc_Tcenv_SharedFenv*_tmp1C6;_LLF5: _tmp1C5=_tmp1C4->ctrl_env;_tmp1C6=_tmp1C4->shared;_LLF6:;{
# 817
void*_tmp1C7=_tmp1C5->break_stmt;void*_tmp1C8=_tmp1C7;struct Cyc_Absyn_Stmt*_tmp1C9;switch(*((int*)_tmp1C8)){case 3: _LLF8: _tmp1C9=((struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1C8)->f1;_LLF9:
# 819
 _tmp1C9->non_local_preds=({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=s;_tmp1CA->tl=_tmp1C9->non_local_preds;_tmp1CA;});
*sopt=_tmp1C9;
return;case 0: _LLFA: _LLFB:
# 823
({void*_tmp1CB=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp1CC="break not in a loop or switch";_tag_dyneither(_tmp1CC,sizeof(char),30);}),_tag_dyneither(_tmp1CB,sizeof(void*),0));});
return;case 2: _LLFC: _LLFD:
# 843 "tcenv.cyc"
 return;default: _LLFE: _LLFF:
# 848
({void*_tmp1CD=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp1CE="break causes outer switch clause to implicitly fallthru";_tag_dyneither(_tmp1CE,sizeof(char),56);}),_tag_dyneither(_tmp1CD,sizeof(void*),0));});
return;}_LLF7:;};}
# 852
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv*_tmp1D0=Cyc_Tcenv_get_fenv(te,({const char*_tmp1D7="process_goto";_tag_dyneither(_tmp1D7,sizeof(char),13);}));struct Cyc_Tcenv_Fenv*_tmp1D1=_tmp1D0;struct Cyc_Tcenv_SharedFenv*_tmp1D2;_LL101: _tmp1D2=_tmp1D1->shared;_LL102:;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1D2->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1D3=_tmp1D2->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1D3,l);
struct _RegionHandle*frgn=_tmp1D2->frgn;
if(slopt == 0)
slopt=({struct Cyc_List_List**_tmp1D4=_region_malloc(frgn,sizeof(*_tmp1D4));_tmp1D4[0]=0;_tmp1D4;});{
struct Cyc_List_List*new_needed=({struct Cyc_List_List*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->hd=s;_tmp1D5->tl=*slopt;_tmp1D5;});
_tmp1D2->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1D3,l,new_needed);};}else{
# 864
struct Cyc_Absyn_Stmt*s=*sopt2;
s->non_local_preds=({struct Cyc_List_List*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=s;_tmp1D6->tl=s->non_local_preds;_tmp1D6;});
*sopt=s;}};}
# 870
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 872
struct Cyc_Tcenv_Fenv*_tmp1D8=Cyc_Tcenv_get_fenv(te,({const char*_tmp1DE="process_fallthru";_tag_dyneither(_tmp1DE,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp1D9=_tmp1D8;struct Cyc_Tcenv_CtrlEnv*_tmp1DA;_LL104: _tmp1DA=_tmp1D9->ctrl_env;_LL105:;{
const struct _tuple10*_tmp1DB=_tmp1DA->fallthru_clause;
if(_tmp1DB != (const struct _tuple10*)0){
(((*_tmp1DB).f1)->body)->non_local_preds=({struct Cyc_List_List*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->hd=s;_tmp1DC->tl=(((*_tmp1DB).f1)->body)->non_local_preds;_tmp1DC;});
# 877
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=(*_tmp1DB).f1;_tmp1DD;});}
# 879
return _tmp1DB;};}
# 882
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 886
struct Cyc_Tcenv_Fenv*_tmp1DF=
# 888
Cyc_Tcenv_get_fenv(te,({const char*_tmp1F8="set_fallthru";_tag_dyneither(_tmp1F8,sizeof(char),13);}));
# 886
struct Cyc_Tcenv_Fenv*_tmp1E0=_tmp1DF;struct Cyc_Tcenv_SharedFenv*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_RgnOrder_RgnPO*_tmp1E3;const struct Cyc_Tcenv_Bindings*_tmp1E4;struct Cyc_Absyn_Stmt*_tmp1E5;struct Cyc_Tcenv_CtrlEnv*_tmp1E6;void*_tmp1E7;void*_tmp1E8;struct Cyc_Tcenv_FenvFlags _tmp1E9;_LL107: _tmp1E1=_tmp1E0->shared;_tmp1E2=_tmp1E0->type_vars;_tmp1E3=_tmp1E0->region_order;_tmp1E4=_tmp1E0->locals;_tmp1E5=_tmp1E0->encloser;_tmp1E6=_tmp1E0->ctrl_env;_tmp1E7=_tmp1E0->capability;_tmp1E8=_tmp1E0->curr_rgn;_tmp1E9=_tmp1E0->flags;_LL108:;{
# 889
struct Cyc_Tcenv_CtrlEnv*_tmp1EA=_tmp1E6;struct _RegionHandle*_tmp1EB;void*_tmp1EC;void*_tmp1ED;const struct _tuple10*_tmp1EE;void*_tmp1EF;int _tmp1F0;_LL10A: _tmp1EB=_tmp1EA->ctrl_rgn;_tmp1EC=_tmp1EA->continue_stmt;_tmp1ED=_tmp1EA->break_stmt;_tmp1EE=_tmp1EA->fallthru_clause;_tmp1EF=_tmp1EA->next_stmt;_tmp1F0=_tmp1EA->try_depth;_LL10B:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp1F1=_region_malloc(_tmp1EB,sizeof(*_tmp1F1));_tmp1F1->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp1F1->tl=ft_typ;_tmp1F1;});}{
const struct Cyc_Tcenv_CList*_tmp1F2=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp1F3=({struct Cyc_Tcenv_CtrlEnv*_tmp1F6=_region_malloc(r,sizeof(*_tmp1F6));_tmp1F6->ctrl_rgn=_tmp1EB;_tmp1F6->continue_stmt=_tmp1EC;_tmp1F6->break_stmt=_tmp1ED;_tmp1F6->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp1F7=_region_malloc(_tmp1EB,sizeof(*_tmp1F7));_tmp1F7->f1=clause;_tmp1F7->f2=new_tvs;_tmp1F7->f3=_tmp1F2;_tmp1F7;});_tmp1F6->next_stmt=_tmp1EF;_tmp1F6->try_depth=_tmp1F0;_tmp1F6;});
# 897
struct Cyc_Tcenv_Fenv*_tmp1F4=({struct Cyc_Tcenv_Fenv*_tmp1F5=_region_malloc(r,sizeof(*_tmp1F5));_tmp1F5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1E1;_tmp1F5->type_vars=(struct Cyc_List_List*)_tmp1E2;_tmp1F5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E3;_tmp1F5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1E4;_tmp1F5->encloser=(struct Cyc_Absyn_Stmt*)_tmp1E5;_tmp1F5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1F3;_tmp1F5->capability=(void*)_tmp1E7;_tmp1F5->curr_rgn=(void*)_tmp1E8;_tmp1F5->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1E9;_tmp1F5->fnrgn=(struct _RegionHandle*)r;_tmp1F5;});
# 901
return Cyc_Tcenv_put_fenv(r,te,_tmp1F4);};};};}
# 904
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1FB="clear_fallthru";_tag_dyneither(_tmp1FB,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp1F9=fe;struct Cyc_Tcenv_CtrlEnv*_tmp1FA;_LL10D: _tmp1FA=_tmp1F9->ctrl_env;_LL10E:;
_tmp1FA->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 911
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 913
struct Cyc_Tcenv_Fenv*_tmp1FC=
# 915
Cyc_Tcenv_get_fenv(te,({const char*_tmp20D="set_in_loop";_tag_dyneither(_tmp20D,sizeof(char),12);}));
# 913
struct Cyc_Tcenv_Fenv*_tmp1FD=_tmp1FC;struct Cyc_Tcenv_SharedFenv*_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_RgnOrder_RgnPO*_tmp200;const struct Cyc_Tcenv_Bindings*_tmp201;struct Cyc_Absyn_Stmt*_tmp202;struct Cyc_Tcenv_CtrlEnv*_tmp203;void*_tmp204;void*_tmp205;struct Cyc_Tcenv_FenvFlags _tmp206;_LL110: _tmp1FE=_tmp1FD->shared;_tmp1FF=_tmp1FD->type_vars;_tmp200=_tmp1FD->region_order;_tmp201=_tmp1FD->locals;_tmp202=_tmp1FD->encloser;_tmp203=_tmp1FD->ctrl_env;_tmp204=_tmp1FD->capability;_tmp205=_tmp1FD->curr_rgn;_tmp206=_tmp1FD->flags;_LL111:;{
# 916
struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp208=_region_malloc(r,sizeof(*_tmp208));_tmp208->ctrl_rgn=r;_tmp208->continue_stmt=(void*)({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp20C;_tmp20C.tag=3;_tmp20C.f1=continue_dest;_tmp20C;});_tmp20B;});_tmp208->break_stmt=_tmp203->next_stmt;_tmp208->next_stmt=(void*)({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209[0]=({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp20A;_tmp20A.tag=3;_tmp20A.f1=continue_dest;_tmp20A;});_tmp209;});_tmp208->fallthru_clause=_tmp203->fallthru_clause;_tmp208->try_depth=_tmp203->try_depth;_tmp208;});
# 923
struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp207=_region_malloc(r,sizeof(*_tmp207));_tmp207->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1FE;_tmp207->type_vars=(struct Cyc_List_List*)_tmp1FF;_tmp207->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp200;_tmp207->locals=(const struct Cyc_Tcenv_Bindings*)_tmp201;_tmp207->encloser=(struct Cyc_Absyn_Stmt*)_tmp202;_tmp207->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp207->capability=(void*)_tmp204;_tmp207->curr_rgn=(void*)_tmp205;_tmp207->flags=(struct Cyc_Tcenv_FenvFlags)_tmp206;_tmp207->fnrgn=(struct _RegionHandle*)r;_tmp207;});
# 926
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 929
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp210="enter_try";_tag_dyneither(_tmp210,sizeof(char),10);})));
struct Cyc_Tcenv_Fenv*_tmp20E=fe;struct Cyc_Tcenv_CtrlEnv*_tmp20F;_LL113: _tmp20F=_tmp20E->ctrl_env;_LL114:;
++ _tmp20F->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 935
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp211=Cyc_Tcenv_get_fenv(te,({const char*_tmp214="get_try_depth";_tag_dyneither(_tmp214,sizeof(char),14);}));struct Cyc_Tcenv_Fenv*_tmp212=_tmp211;struct Cyc_Tcenv_CtrlEnv*_tmp213;_LL116: _tmp213=_tmp212->ctrl_env;_LL117:;
return _tmp213->try_depth;}
# 942
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp217="set_in_switch";_tag_dyneither(_tmp217,sizeof(char),14);})));
struct Cyc_Tcenv_Fenv*_tmp215=fe;struct Cyc_Tcenv_CtrlEnv*_tmp216;_LL119: _tmp216=_tmp215->ctrl_env;_LL11A:;
_tmp216->break_stmt=_tmp216->next_stmt;
_tmp216->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 950
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 952
struct Cyc_Tcenv_Fenv _tmp218=*
# 954
Cyc_Tcenv_get_fenv(te,({const char*_tmp225="set_next";_tag_dyneither(_tmp225,sizeof(char),9);}));
# 952
struct Cyc_Tcenv_Fenv _tmp219=_tmp218;struct Cyc_Tcenv_SharedFenv*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_RgnOrder_RgnPO*_tmp21C;const struct Cyc_Tcenv_Bindings*_tmp21D;struct Cyc_Absyn_Stmt*_tmp21E;struct Cyc_Tcenv_CtrlEnv*_tmp21F;void*_tmp220;void*_tmp221;struct Cyc_Tcenv_FenvFlags _tmp222;_LL11C: _tmp21A=_tmp219.shared;_tmp21B=_tmp219.type_vars;_tmp21C=_tmp219.region_order;_tmp21D=_tmp219.locals;_tmp21E=_tmp219.encloser;_tmp21F=_tmp219.ctrl_env;_tmp220=_tmp219.capability;_tmp221=_tmp219.curr_rgn;_tmp222=_tmp219.flags;_LL11D:;{
# 955
struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp224=_region_malloc(r,sizeof(*_tmp224));_tmp224->ctrl_rgn=r;_tmp224->continue_stmt=_tmp21F->continue_stmt;_tmp224->break_stmt=_tmp21F->break_stmt;_tmp224->next_stmt=j;_tmp224->fallthru_clause=_tmp21F->fallthru_clause;_tmp224->try_depth=_tmp21F->try_depth;_tmp224;});
# 963
struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp223=_region_malloc(r,sizeof(*_tmp223));_tmp223->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp21A;_tmp223->type_vars=(struct Cyc_List_List*)_tmp21B;_tmp223->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp21C;_tmp223->locals=(const struct Cyc_Tcenv_Bindings*)_tmp21D;_tmp223->encloser=(struct Cyc_Absyn_Stmt*)_tmp21E;_tmp223->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp223->capability=(void*)_tmp220;_tmp223->curr_rgn=(void*)_tmp221;_tmp223->flags=(struct Cyc_Tcenv_FenvFlags)_tmp222;_tmp223->fnrgn=(struct _RegionHandle*)r;_tmp223;});
# 966
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 969
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Tcenv_Fenv*_tmp226=Cyc_Tcenv_get_fenv(te,({const char*_tmp231="add_label";_tag_dyneither(_tmp231,sizeof(char),10);}));struct Cyc_Tcenv_Fenv*_tmp227=_tmp226;struct Cyc_Tcenv_SharedFenv*_tmp228;_LL11F: _tmp228=_tmp227->shared;_LL120:;{
struct Cyc_Dict_Dict needed=_tmp228->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp228->frgn;
if(sl_opt != 0){
_tmp228->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 977
struct Cyc_List_List*_tmp229=*sl_opt;
s->non_local_preds=_tmp229;
for(0;_tmp229 != 0;_tmp229=_tmp229->tl){
void*_tmp22A=((struct Cyc_Absyn_Stmt*)_tmp229->hd)->r;void*_tmp22B=_tmp22A;struct Cyc_Absyn_Stmt**_tmp22C;if(((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp22B)->tag == 8){_LL122: _tmp22C=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp22B)->f2;_LL123:
*_tmp22C=s;goto _LL121;}else{_LL124: _LL125:
((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp22D="Tcenv: add_label backpatching of non-goto";_tag_dyneither(_tmp22D,sizeof(char),42);}));goto _LL121;}_LL121:;}};}
# 985
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp228->seen_labels,v))
({struct Cyc_String_pa_PrintArg_struct _tmp230;_tmp230.tag=0;_tmp230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp22E[1]={& _tmp230};Cyc_Tcutil_terr(s->loc,({const char*_tmp22F="Repeated label: %s";_tag_dyneither(_tmp22F,sizeof(char),19);}),_tag_dyneither(_tmp22E,sizeof(void*),1));});});
_tmp228->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp228->seen_labels,v,s);
return te;};}
# 991
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp232=Cyc_Tcenv_get_fenv(te,({const char*_tmp235="all_labels_resolved";_tag_dyneither(_tmp235,sizeof(char),20);}));struct Cyc_Tcenv_Fenv*_tmp233=_tmp232;struct Cyc_Tcenv_SharedFenv*_tmp234;_LL127: _tmp234=_tmp233->shared;_LL128:;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp234->needed_labels);}
# 996
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp236=Cyc_Tcenv_get_fenv(te,({const char*_tmp239="get_encloser";_tag_dyneither(_tmp239,sizeof(char),13);}));struct Cyc_Tcenv_Fenv*_tmp237=_tmp236;struct Cyc_Absyn_Stmt*_tmp238;_LL12A: _tmp238=_tmp237->encloser;_LL12B:;
return _tmp238;}
# 1001
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp23A=_region_malloc(r2,sizeof(*_tmp23A));_tmp23A->ns=te->ns;_tmp23A->ae=te->ae;_tmp23A->le=0;_tmp23A->allow_valueof=1;_tmp23A->in_extern_c_include=te->in_extern_c_include;_tmp23A;});{
struct Cyc_Tcenv_Fenv _tmp23B=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp23C=_tmp23B;struct Cyc_Tcenv_SharedFenv*_tmp23D;struct Cyc_List_List*_tmp23E;struct Cyc_RgnOrder_RgnPO*_tmp23F;const struct Cyc_Tcenv_Bindings*_tmp240;struct Cyc_Absyn_Stmt*_tmp241;struct Cyc_Tcenv_CtrlEnv*_tmp242;void*_tmp243;void*_tmp244;struct Cyc_Tcenv_FenvFlags _tmp245;_LL12D: _tmp23D=_tmp23C.shared;_tmp23E=_tmp23C.type_vars;_tmp23F=_tmp23C.region_order;_tmp240=_tmp23C.locals;_tmp241=_tmp23C.encloser;_tmp242=_tmp23C.ctrl_env;_tmp243=_tmp23C.capability;_tmp244=_tmp23C.curr_rgn;_tmp245=_tmp23C.flags;_LL12E:;{
struct Cyc_Tcenv_Fenv*_tmp246=({struct Cyc_Tcenv_Fenv*_tmp248=_region_malloc(r2,sizeof(*_tmp248));_tmp248->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp23D;_tmp248->type_vars=(struct Cyc_List_List*)_tmp23E;_tmp248->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp23F;_tmp248->locals=(const struct Cyc_Tcenv_Bindings*)_tmp240;_tmp248->encloser=(struct Cyc_Absyn_Stmt*)_tmp241;_tmp248->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp242;_tmp248->capability=(void*)_tmp243;_tmp248->curr_rgn=(void*)_tmp244;_tmp248->flags=(struct Cyc_Tcenv_FenvFlags)_tmp245;_tmp248->fnrgn=(struct _RegionHandle*)r2;_tmp248;});
return({struct Cyc_Tcenv_Tenv*_tmp247=_region_malloc(r2,sizeof(*_tmp247));_tmp247->ns=te->ns;_tmp247->ae=te->ae;_tmp247->le=_tmp246;_tmp247->allow_valueof=1;_tmp247->in_extern_c_include=te->in_extern_c_include;_tmp247;});};};}
# 1009
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp249=_region_malloc(r2,sizeof(*_tmp249));_tmp249->ns=te->ns;_tmp249->ae=te->ae;_tmp249->le=0;_tmp249->allow_valueof=te->allow_valueof;_tmp249->in_extern_c_include=1;_tmp249;});{
struct Cyc_Tcenv_Fenv _tmp24A=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp24B=_tmp24A;struct Cyc_Tcenv_SharedFenv*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_RgnOrder_RgnPO*_tmp24E;const struct Cyc_Tcenv_Bindings*_tmp24F;struct Cyc_Absyn_Stmt*_tmp250;struct Cyc_Tcenv_CtrlEnv*_tmp251;void*_tmp252;void*_tmp253;struct Cyc_Tcenv_FenvFlags _tmp254;_LL130: _tmp24C=_tmp24B.shared;_tmp24D=_tmp24B.type_vars;_tmp24E=_tmp24B.region_order;_tmp24F=_tmp24B.locals;_tmp250=_tmp24B.encloser;_tmp251=_tmp24B.ctrl_env;_tmp252=_tmp24B.capability;_tmp253=_tmp24B.curr_rgn;_tmp254=_tmp24B.flags;_LL131:;{
struct Cyc_Tcenv_Fenv*_tmp255=({struct Cyc_Tcenv_Fenv*_tmp257=_region_malloc(r2,sizeof(*_tmp257));_tmp257->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp24C;_tmp257->type_vars=(struct Cyc_List_List*)_tmp24D;_tmp257->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24E;_tmp257->locals=(const struct Cyc_Tcenv_Bindings*)_tmp24F;_tmp257->encloser=(struct Cyc_Absyn_Stmt*)_tmp250;_tmp257->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp251;_tmp257->capability=(void*)_tmp252;_tmp257->curr_rgn=(void*)_tmp253;_tmp257->flags=(struct Cyc_Tcenv_FenvFlags)_tmp254;_tmp257->fnrgn=(struct _RegionHandle*)r2;_tmp257;});
return({struct Cyc_Tcenv_Tenv*_tmp256=_region_malloc(r2,sizeof(*_tmp256));_tmp256->ns=te->ns;_tmp256->ae=te->ae;_tmp256->le=_tmp255;_tmp256->allow_valueof=te->allow_valueof;_tmp256->in_extern_c_include=1;_tmp256;});};};}
# 1017
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1019
struct Cyc_Tcenv_Fenv _tmp258=*
# 1021
Cyc_Tcenv_get_fenv(te,({const char*_tmp265="set_encloser";_tag_dyneither(_tmp265,sizeof(char),13);}));
# 1019
struct Cyc_Tcenv_Fenv _tmp259=_tmp258;struct Cyc_Tcenv_SharedFenv*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_RgnOrder_RgnPO*_tmp25C;const struct Cyc_Tcenv_Bindings*_tmp25D;struct Cyc_Absyn_Stmt*_tmp25E;struct Cyc_Tcenv_CtrlEnv*_tmp25F;void*_tmp260;void*_tmp261;struct Cyc_Tcenv_FenvFlags _tmp262;_LL133: _tmp25A=_tmp259.shared;_tmp25B=_tmp259.type_vars;_tmp25C=_tmp259.region_order;_tmp25D=_tmp259.locals;_tmp25E=_tmp259.encloser;_tmp25F=_tmp259.ctrl_env;_tmp260=_tmp259.capability;_tmp261=_tmp259.curr_rgn;_tmp262=_tmp259.flags;_LL134:;{
# 1022
struct Cyc_Tcenv_Fenv*_tmp263=({struct Cyc_Tcenv_Fenv*_tmp264=_region_malloc(r,sizeof(*_tmp264));_tmp264->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp25A;_tmp264->type_vars=(struct Cyc_List_List*)_tmp25B;_tmp264->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp25C;_tmp264->locals=(const struct Cyc_Tcenv_Bindings*)_tmp25D;_tmp264->encloser=(struct Cyc_Absyn_Stmt*)s;_tmp264->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp25F;_tmp264->capability=(void*)_tmp260;_tmp264->curr_rgn=(void*)_tmp261;_tmp264->flags=(struct Cyc_Tcenv_FenvFlags)_tmp262;_tmp264->fnrgn=(struct _RegionHandle*)r;_tmp264;});
# 1025
return Cyc_Tcenv_put_fenv(r,te,_tmp263);};}
# 1028
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1031
struct Cyc_Tcenv_Fenv _tmp266=*
# 1033
Cyc_Tcenv_get_fenv(te,({const char*_tmp27C="add_region";_tag_dyneither(_tmp27C,sizeof(char),11);}));
# 1031
struct Cyc_Tcenv_Fenv _tmp267=_tmp266;struct Cyc_Tcenv_SharedFenv*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_RgnOrder_RgnPO*_tmp26A;const struct Cyc_Tcenv_Bindings*_tmp26B;struct Cyc_Absyn_Stmt*_tmp26C;struct Cyc_Tcenv_CtrlEnv*_tmp26D;void*_tmp26E;void*_tmp26F;struct Cyc_Tcenv_FenvFlags _tmp270;_LL136: _tmp268=_tmp267.shared;_tmp269=_tmp267.type_vars;_tmp26A=_tmp267.region_order;_tmp26B=_tmp267.locals;_tmp26C=_tmp267.encloser;_tmp26D=_tmp267.ctrl_env;_tmp26E=_tmp267.capability;_tmp26F=_tmp267.curr_rgn;_tmp270=_tmp267.flags;_LL137:;
# 1034
{void*_tmp271=Cyc_Tcutil_compress(rgn);void*_tmp272=_tmp271;struct Cyc_Absyn_Tvar*_tmp273;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp272)->tag == 2){_LL139: _tmp273=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp272)->f1;_LL13A:
# 1036
 _tmp26A=Cyc_RgnOrder_add_youngest(_tmp268->frgn,_tmp26A,_tmp273,resetable,opened);
# 1038
goto _LL138;}else{_LL13B: _LL13C:
 goto _LL138;}_LL138:;}
# 1041
_tmp26E=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp275;_tmp275.tag=24;_tmp275.f1=({struct Cyc_List_List*_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp279;_tmp279.tag=23;_tmp279.f1=rgn;_tmp279;});_tmp278;});_tmp276->tl=({struct Cyc_List_List*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->hd=_tmp26E;_tmp277->tl=0;_tmp277;});_tmp276;});_tmp275;});_tmp274;});{
# 1043
struct Cyc_Tcenv_Fenv*_tmp27A=({struct Cyc_Tcenv_Fenv*_tmp27B=_region_malloc(r,sizeof(*_tmp27B));_tmp27B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp268;_tmp27B->type_vars=(struct Cyc_List_List*)_tmp269;_tmp27B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp26A;_tmp27B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp26B;_tmp27B->encloser=(struct Cyc_Absyn_Stmt*)_tmp26C;_tmp27B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp26D;_tmp27B->capability=(void*)_tmp26E;_tmp27B->curr_rgn=(void*)_tmp26F;_tmp27B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp270;_tmp27B->fnrgn=(struct _RegionHandle*)r;_tmp27B;});
# 1046
return Cyc_Tcenv_put_fenv(r,te,_tmp27A);};}
# 1049
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1051
struct Cyc_Tcenv_Fenv _tmp27D=*
# 1053
Cyc_Tcenv_get_fenv(te,({const char*_tmp294="new_named_block";_tag_dyneither(_tmp294,sizeof(char),16);}));
# 1051
struct Cyc_Tcenv_Fenv _tmp27E=_tmp27D;struct Cyc_Tcenv_SharedFenv*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_RgnOrder_RgnPO*_tmp281;const struct Cyc_Tcenv_Bindings*_tmp282;struct Cyc_Absyn_Stmt*_tmp283;struct Cyc_Tcenv_CtrlEnv*_tmp284;void*_tmp285;void*_tmp286;struct Cyc_Tcenv_FenvFlags _tmp287;_LL13E: _tmp27F=_tmp27E.shared;_tmp280=_tmp27E.type_vars;_tmp281=_tmp27E.region_order;_tmp282=_tmp27E.locals;_tmp283=_tmp27E.encloser;_tmp284=_tmp27E.ctrl_env;_tmp285=_tmp27E.capability;_tmp286=_tmp27E.curr_rgn;_tmp287=_tmp27E.flags;_LL13F:;{
# 1055
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp293="new_block";_tag_dyneither(_tmp293,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp292;_tmp292.tag=2;_tmp292.f1=block_rgn;_tmp292;});_tmp291;});
_tmp280=({struct Cyc_List_List*_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->hd=block_rgn;_tmp288->tl=_tmp280;_tmp288;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp280);
_tmp281=Cyc_RgnOrder_add_youngest(_tmp27F->frgn,_tmp281,block_rgn,0,0);
_tmp285=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp28A;_tmp28A.tag=24;_tmp28A.f1=({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp28E;_tmp28E.tag=23;_tmp28E.f1=block_typ;_tmp28E;});_tmp28D;});_tmp28B->tl=({struct Cyc_List_List*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=_tmp285;_tmp28C->tl=0;_tmp28C;});_tmp28B;});_tmp28A;});_tmp289;});
_tmp286=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp28F=({struct Cyc_Tcenv_Fenv*_tmp290=_region_malloc(r,sizeof(*_tmp290));_tmp290->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp27F;_tmp290->type_vars=(struct Cyc_List_List*)_tmp280;_tmp290->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp281;_tmp290->locals=(const struct Cyc_Tcenv_Bindings*)_tmp282;_tmp290->encloser=(struct Cyc_Absyn_Stmt*)_tmp283;_tmp290->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp284;_tmp290->capability=(void*)_tmp285;_tmp290->curr_rgn=(void*)_tmp286;_tmp290->flags=(struct Cyc_Tcenv_FenvFlags)_tmp287;_tmp290->fnrgn=(struct _RegionHandle*)r;_tmp290;});
# 1065
return Cyc_Tcenv_put_fenv(r,te,_tmp28F);};};}
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
struct Cyc_Tcenv_Fenv _tmp296=*
# 1084
Cyc_Tcenv_get_fenv(te,({const char*_tmp2A3="new_outlives_constraints";_tag_dyneither(_tmp2A3,sizeof(char),25);}));
# 1082
struct Cyc_Tcenv_Fenv _tmp297=_tmp296;struct Cyc_Tcenv_SharedFenv*_tmp298;struct Cyc_List_List*_tmp299;struct Cyc_RgnOrder_RgnPO*_tmp29A;const struct Cyc_Tcenv_Bindings*_tmp29B;struct Cyc_Absyn_Stmt*_tmp29C;struct Cyc_Tcenv_CtrlEnv*_tmp29D;void*_tmp29E;void*_tmp29F;struct Cyc_Tcenv_FenvFlags _tmp2A0;_LL141: _tmp298=_tmp297.shared;_tmp299=_tmp297.type_vars;_tmp29A=_tmp297.region_order;_tmp29B=_tmp297.locals;_tmp29C=_tmp297.encloser;_tmp29D=_tmp297.ctrl_env;_tmp29E=_tmp297.capability;_tmp29F=_tmp297.curr_rgn;_tmp2A0=_tmp297.flags;_LL142:;
# 1085
for(0;cs != 0;cs=cs->tl){
_tmp29A=
Cyc_RgnOrder_add_outlives_constraint(_tmp298->frgn,_tmp29A,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1090
struct Cyc_Tcenv_Fenv*_tmp2A1=({struct Cyc_Tcenv_Fenv*_tmp2A2=_region_malloc(r,sizeof(*_tmp2A2));_tmp2A2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp298;_tmp2A2->type_vars=(struct Cyc_List_List*)_tmp299;_tmp2A2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp29A;_tmp2A2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp29B;_tmp2A2->encloser=(struct Cyc_Absyn_Stmt*)_tmp29C;_tmp2A2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp29D;_tmp2A2->capability=(void*)_tmp29E;_tmp2A2->curr_rgn=(void*)_tmp29F;_tmp2A2->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2A0;_tmp2A2->fnrgn=(struct _RegionHandle*)r;_tmp2A2;});
# 1093
return Cyc_Tcenv_put_fenv(r,te,_tmp2A1);};}
# 1096
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1104
void*_tmp2A4=Cyc_Tcutil_compress(r1);
void*_tmp2A5=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp2A6=Cyc_Tcutil_typ_kind(_tmp2A4);
struct Cyc_Absyn_Kind*_tmp2A7=Cyc_Tcutil_typ_kind(_tmp2A5);
# 1110
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp2A6,_tmp2A7);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp2A7,_tmp2A6);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0;_tmp2AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp2A7));({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0;_tmp2AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp2A6));({struct Cyc_String_pa_PrintArg_struct _tmp2AB;_tmp2AB.tag=0;_tmp2AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2A5));({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2A4));({void*_tmp2A8[4]={& _tmp2AA,& _tmp2AB,& _tmp2AC,& _tmp2AD};Cyc_Tcutil_terr(loc,({const char*_tmp2A9="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp2A9,sizeof(char),82);}),_tag_dyneither(_tmp2A8,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1117
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp2AE=Cyc_Tcutil_swap_kind(_tmp2A5,Cyc_Tcutil_kind_to_bound(_tmp2A6));struct _tuple11 _tmp2AF=_tmp2AE;struct Cyc_Absyn_Tvar*_tmp2B0;void*_tmp2B1;_LL144: _tmp2B0=_tmp2AF.f1;_tmp2B1=_tmp2AF.f2;_LL145:;
*oldtv=({struct _tuple11*_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));_tmp2B2->f1=_tmp2B0;_tmp2B2->f2=_tmp2B1;_tmp2B2;});}else{
# 1121
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp2B3=Cyc_Tcutil_swap_kind(_tmp2A4,Cyc_Tcutil_kind_to_bound(_tmp2A7));struct _tuple11 _tmp2B4=_tmp2B3;struct Cyc_Absyn_Tvar*_tmp2B5;void*_tmp2B6;_LL147: _tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;_LL148:;
*oldtv=({struct _tuple11*_tmp2B7=_region_malloc(r,sizeof(*_tmp2B7));_tmp2B7->f1=_tmp2B5;_tmp2B7->f2=_tmp2B6;_tmp2B7;});}}}{
# 1127
struct _RegionHandle*_tmp2B8=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2B9=0;
if((_tmp2A4 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2A4 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2A4 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2BD;_tmp2BD.tag=23;_tmp2BD.f1=_tmp2A4;_tmp2BD;});_tmp2BC;});
_tmp2B9=({struct Cyc_List_List*_tmp2BA=_region_malloc(_tmp2B8,sizeof(*_tmp2BA));_tmp2BA->hd=({struct _tuple13*_tmp2BB=_region_malloc(_tmp2B8,sizeof(*_tmp2BB));_tmp2BB->f1=eff1;_tmp2BB->f2=_tmp2A5;_tmp2BB;});_tmp2BA->tl=_tmp2B9;_tmp2BA;});}
# 1133
if((_tmp2A5 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2A5 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2A5 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2C1;_tmp2C1.tag=23;_tmp2C1.f1=_tmp2A5;_tmp2C1;});_tmp2C0;});
_tmp2B9=({struct Cyc_List_List*_tmp2BE=_region_malloc(_tmp2B8,sizeof(*_tmp2BE));_tmp2BE->hd=({struct _tuple13*_tmp2BF=_region_malloc(_tmp2B8,sizeof(*_tmp2BF));_tmp2BF->f1=eff2;_tmp2BF->f2=_tmp2A4;_tmp2BF;});_tmp2BE->tl=_tmp2B9;_tmp2BE;});}
# 1138
return Cyc_Tcenv_new_outlives_constraints(_tmp2B8,te,_tmp2B9,loc);};}
# 1141
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2C2=te->le;
if(_tmp2C2 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2C2;
struct Cyc_Tcenv_Fenv*_tmp2C3=fe;void*_tmp2C4;_LL14A: _tmp2C4=_tmp2C3->curr_rgn;_LL14B:;
return _tmp2C4;};}
# 1152
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp2CD="check_rgn_accessible";_tag_dyneither(_tmp2CD,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp2C5=fe;void*_tmp2C6;struct Cyc_RgnOrder_RgnPO*_tmp2C7;_LL14D: _tmp2C6=_tmp2C5->capability;_tmp2C7=_tmp2C5->region_order;_LL14E:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2C6) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2C6))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2C7,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2C9;_tmp2C9.tag=23;_tmp2C9.f1=rgn;_tmp2C9;});_tmp2C8;}),_tmp2C6))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp2CC;_tmp2CC.tag=0;_tmp2CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp2CA[1]={& _tmp2CC};Cyc_Tcutil_terr(loc,({const char*_tmp2CB="Expression accesses unavailable region %s";_tag_dyneither(_tmp2CB,sizeof(char),42);}),_tag_dyneither(_tmp2CA,sizeof(void*),1));});});}
# 1164
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp2CE=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2D9="check_rgn_resetable";_tag_dyneither(_tmp2D9,sizeof(char),20);}));
# 1166
struct Cyc_Tcenv_Fenv*_tmp2CF=_tmp2CE;struct Cyc_RgnOrder_RgnPO*_tmp2D0;_LL150: _tmp2D0=_tmp2CF->region_order;_LL151:;{
# 1168
void*_tmp2D1=Cyc_Tcutil_compress(rgn);void*_tmp2D2=_tmp2D1;struct Cyc_Absyn_Tvar*_tmp2D3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D2)->tag == 2){_LL153: _tmp2D3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D2)->f1;_LL154:
# 1170
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2D0,_tmp2D3))
({struct Cyc_String_pa_PrintArg_struct _tmp2D6;_tmp2D6.tag=0;_tmp2D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp2D4[1]={& _tmp2D6};Cyc_Tcutil_terr(loc,({const char*_tmp2D5="Region %s is not resetable";_tag_dyneither(_tmp2D5,sizeof(char),27);}),_tag_dyneither(_tmp2D4,sizeof(void*),1));});});
return;}else{_LL155: _LL156:
# 1174
({void*_tmp2D7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2D8="check_rgn_resetable";_tag_dyneither(_tmp2D8,sizeof(char),20);}),_tag_dyneither(_tmp2D7,sizeof(void*),0));});}_LL152:;};};}
# 1181
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2DA=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1186
if(_tmp2DA == 0){
void*_tmp2DB=rt_a;switch(*((int*)_tmp2DB)){case 22: _LL158: _LL159:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LL15A: _LL15B:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LL15C: _LL15D:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL15E: _LL15F:
 return 0;}_LL157:;}{
# 1194
struct Cyc_Tcenv_Fenv*fe=_tmp2DA;
struct Cyc_Tcenv_Fenv*_tmp2DC=fe;struct Cyc_RgnOrder_RgnPO*_tmp2DD;_LL161: _tmp2DD=_tmp2DC->region_order;_LL162:;{
int _tmp2DE=Cyc_RgnOrder_effect_outlives(_tmp2DD,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));_tmp2DF[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2E0;_tmp2E0.tag=23;_tmp2E0.f1=rt_a;_tmp2E0;});_tmp2DF;}),rt_b);
# 1200
return _tmp2DE;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1205
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp2E1=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2E8="check_effect_accessible";_tag_dyneither(_tmp2E8,sizeof(char),24);}));
# 1206
struct Cyc_Tcenv_Fenv*_tmp2E2=_tmp2E1;void*_tmp2E3;struct Cyc_RgnOrder_RgnPO*_tmp2E4;struct Cyc_Tcenv_SharedFenv*_tmp2E5;_LL164: _tmp2E3=_tmp2E2->capability;_tmp2E4=_tmp2E2->region_order;_tmp2E5=_tmp2E2->shared;_LL165:;
# 1208
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2E3))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2E4,eff,_tmp2E3))
return;{
struct _RegionHandle*frgn=_tmp2E5->frgn;
_tmp2E5->delayed_effect_checks=({struct Cyc_List_List*_tmp2E6=_region_malloc(frgn,sizeof(*_tmp2E6));_tmp2E6->hd=({struct _tuple14*_tmp2E7=_region_malloc(frgn,sizeof(*_tmp2E7));_tmp2E7->f1=_tmp2E3;_tmp2E7->f2=eff;_tmp2E7->f3=_tmp2E4;_tmp2E7->f4=loc;_tmp2E7;});_tmp2E6->tl=_tmp2E5->delayed_effect_checks;_tmp2E6;});};}
# 1217
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2E9=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2F7="check_delayed_effects";_tag_dyneither(_tmp2F7,sizeof(char),22);}));
# 1218
struct Cyc_Tcenv_Fenv*_tmp2EA=_tmp2E9;struct Cyc_Tcenv_SharedFenv*_tmp2EB;_LL167: _tmp2EB=_tmp2EA->shared;_LL168:;{
# 1220
struct Cyc_List_List*_tmp2EC=_tmp2EB->delayed_effect_checks;
for(0;_tmp2EC != 0;_tmp2EC=_tmp2EC->tl){
struct _tuple14*_tmp2ED=(struct _tuple14*)_tmp2EC->hd;struct _tuple14*_tmp2EE=_tmp2ED;void*_tmp2EF;void*_tmp2F0;struct Cyc_RgnOrder_RgnPO*_tmp2F1;unsigned int _tmp2F2;_LL16A: _tmp2EF=_tmp2EE->f1;_tmp2F0=_tmp2EE->f2;_tmp2F1=_tmp2EE->f3;_tmp2F2=_tmp2EE->f4;_LL16B:;
if(Cyc_Tcutil_subset_effect(1,_tmp2F0,_tmp2EF))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2F1,_tmp2F0,_tmp2EF))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp2F6;_tmp2F6.tag=0;_tmp2F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2F0));({struct Cyc_String_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=0;_tmp2F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2EF));({void*_tmp2F3[2]={& _tmp2F5,& _tmp2F6};Cyc_Tcutil_terr(_tmp2F2,({const char*_tmp2F4="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp2F4,sizeof(char),51);}),_tag_dyneither(_tmp2F3,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1236
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1238
struct Cyc_Tcenv_Fenv*_tmp2F8=te->le;
if(_tmp2F8 == 0){
# 1241
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2FA;_tmp2FA.tag=23;_tmp2FA.f1=(*((struct _tuple13*)po->hd)).f2;_tmp2FA;});_tmp2F9;}),Cyc_Absyn_empty_effect))
({void*_tmp2FB=0;Cyc_Tcutil_terr(loc,({const char*_tmp2FC="the required region ordering is not satisfied here";_tag_dyneither(_tmp2FC,sizeof(char),51);}),_tag_dyneither(_tmp2FB,sizeof(void*),0));});}
return;}{
# 1247
struct Cyc_Tcenv_Fenv*_tmp2FD=_tmp2F8;struct Cyc_Tcenv_Fenv*_tmp2FE=_tmp2FD;struct Cyc_RgnOrder_RgnPO*_tmp2FF;struct Cyc_Tcenv_SharedFenv*_tmp300;_LL16D: _tmp2FF=_tmp2FE->region_order;_tmp300=_tmp2FE->shared;_LL16E:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2FF,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp300->delayed_constraint_checks=({struct Cyc_List_List*_tmp301=_region_malloc(_tmp300->frgn,sizeof(*_tmp301));_tmp301->hd=({struct _tuple15*_tmp302=_region_malloc(_tmp300->frgn,sizeof(*_tmp302));_tmp302->f1=_tmp2FF;_tmp302->f2=po;_tmp302->f3=loc;_tmp302;});_tmp301->tl=_tmp300->delayed_constraint_checks;_tmp301;});};}
# 1254
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp303=
Cyc_Tcenv_get_fenv(te,({const char*_tmp30E="check_delayed_constraints";_tag_dyneither(_tmp30E,sizeof(char),26);}));
# 1255
struct Cyc_Tcenv_Fenv*_tmp304=_tmp303;struct Cyc_Tcenv_SharedFenv*_tmp305;_LL170: _tmp305=_tmp304->shared;_LL171:;{
# 1257
struct Cyc_List_List*_tmp306=_tmp305->delayed_constraint_checks;
for(0;_tmp306 != 0;_tmp306=_tmp306->tl){
struct _tuple15*_tmp307=(struct _tuple15*)_tmp306->hd;struct _tuple15*_tmp308=_tmp307;struct Cyc_RgnOrder_RgnPO*_tmp309;struct Cyc_List_List*_tmp30A;unsigned int _tmp30B;_LL173: _tmp309=_tmp308->f1;_tmp30A=_tmp308->f2;_tmp30B=_tmp308->f3;_LL174:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp309,_tmp30A,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp30C=0;Cyc_Tcutil_terr(_tmp30B,({const char*_tmp30D="the required region ordering is not satisfied here";_tag_dyneither(_tmp30D,sizeof(char),51);}),_tag_dyneither(_tmp30C,sizeof(void*),0));});}};}
# 1265
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F->name=({struct _dyneither_ptr*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp343;_tmp343.tag=0;_tmp343.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp341[1]={& _tmp343};Cyc_aprintf(({const char*_tmp342="`%s";_tag_dyneither(_tmp342,sizeof(char),4);}),_tag_dyneither(_tmp341,sizeof(void*),1));});});_tmp340;});_tmp33F->identity=
# 1269
Cyc_Tcutil_new_tvar_id();_tmp33F->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp33F;});
struct Cyc_List_List*_tmp30F=({struct Cyc_List_List*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->hd=rgn0;_tmp33E->tl=fd->tvs;_tmp33E;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp30F);{
# 1273
struct Cyc_RgnOrder_RgnPO*_tmp310=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1275
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp33D;_tmp33D.tag=2;_tmp33D.f1=rgn0;_tmp33D;});_tmp33C;});
struct Cyc_List_List*_tmp311=0;
{struct Cyc_List_List*_tmp312=fd->args;for(0;_tmp312 != 0;_tmp312=_tmp312->tl){
struct Cyc_Absyn_Vardecl*_tmp313=({struct Cyc_Absyn_Vardecl*_tmp31B=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp31B[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp31C;_tmp31C.sc=Cyc_Absyn_Public;_tmp31C.name=({struct _tuple0*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->f1=Cyc_Absyn_Loc_n;_tmp31D->f2=(*((struct _tuple8*)_tmp312->hd)).f1;_tmp31D;});_tmp31C.tq=(*((struct _tuple8*)_tmp312->hd)).f2;_tmp31C.type=(*((struct _tuple8*)_tmp312->hd)).f3;_tmp31C.initializer=0;_tmp31C.rgn=param_rgn;_tmp31C.attributes=0;_tmp31C.escapes=0;_tmp31C;});_tmp31B;});
# 1285
_tmp311=({struct Cyc_List_List*_tmp314=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp314[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp315;_tmp315.hd=_tmp313;_tmp315.tl=_tmp311;_tmp315;});_tmp314;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp316=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp31A;_tmp31A.tag=3;_tmp31A.f1=_tmp313;_tmp31A;});_tmp319;});
struct _dyneither_ptr*_tmp317=(*((struct _tuple8*)_tmp312->hd)).f1;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp318=_region_malloc(r,sizeof(*_tmp318));_tmp318->v=_tmp317;_tmp318->b=(void*)_tmp316;_tmp318->tl=locals;_tmp318;});};}}
# 1290
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp31E=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp31F=_tmp31E;struct _dyneither_ptr*_tmp320;struct Cyc_Absyn_Tqual _tmp321;void*_tmp322;int _tmp323;_LL176: _tmp320=_tmp31F.name;_tmp321=_tmp31F.tq;_tmp322=_tmp31F.type;_tmp323=_tmp31F.inject;_LL177:;
if(_tmp320 != 0){
void*_tmp324=Cyc_Absyn_dyneither_typ(_tmp322,param_rgn,_tmp321,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp325=({struct Cyc_Absyn_Vardecl*_tmp32C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp32C[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp32D;_tmp32D.sc=Cyc_Absyn_Public;_tmp32D.name=({struct _tuple0*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->f1=Cyc_Absyn_Loc_n;_tmp32E->f2=_tmp320;_tmp32E;});_tmp32D.tq=
# 1296
Cyc_Absyn_empty_tqual(0);_tmp32D.type=_tmp324;_tmp32D.initializer=0;_tmp32D.rgn=param_rgn;_tmp32D.attributes=0;_tmp32D.escapes=0;_tmp32D;});_tmp32C;});
# 1301
_tmp311=({struct Cyc_List_List*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->hd=_tmp325;_tmp326->tl=_tmp311;_tmp326;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp327=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp32B;_tmp32B.tag=3;_tmp32B.f1=_tmp325;_tmp32B;});_tmp32A;});
struct _dyneither_ptr*_tmp328=_tmp320;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp329=_region_malloc(r,sizeof(*_tmp329));_tmp329->v=_tmp328;_tmp329->b=(void*)_tmp327;_tmp329->tl=locals;_tmp329;});};}else{
# 1306
({void*_tmp32F=0;Cyc_Tcutil_terr(loc,({const char*_tmp330="missing name for varargs";_tag_dyneither(_tmp330,sizeof(char),25);}),_tag_dyneither(_tmp32F,sizeof(void*),0));});}}
# 1308
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp331=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp331[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp332;_tmp332.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp311);_tmp332;});_tmp331;});
return({struct Cyc_Tcenv_Fenv*_tmp333=_region_malloc(r,sizeof(*_tmp333));_tmp333->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp33B=_region_malloc(r,sizeof(*_tmp33B));_tmp33B->frgn=r;_tmp33B->return_typ=fd->ret_type;_tmp33B->seen_labels=
# 1313
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp33B->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp33B->delayed_effect_checks=0;_tmp33B->delayed_constraint_checks=0;_tmp33B;});_tmp333->type_vars=(struct Cyc_List_List*)_tmp30F;_tmp333->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp310;_tmp333->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp333->encloser=(struct Cyc_Absyn_Stmt*)fd->body;_tmp333->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp33A=_region_malloc(r,sizeof(*_tmp33A));_tmp33A->ctrl_rgn=r;_tmp33A->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp33A->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp33A->fallthru_clause=0;_tmp33A->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val;_tmp33A->try_depth=0;_tmp33A;});_tmp333->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp335;_tmp335.tag=24;_tmp335.f1=({struct Cyc_List_List*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp339;_tmp339.tag=23;_tmp339.f1=param_rgn;_tmp339;});_tmp338;});_tmp336->tl=({struct Cyc_List_List*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->hd=(void*)_check_null(fd->effect);_tmp337->tl=0;_tmp337;});_tmp336;});_tmp335;});_tmp334;}));_tmp333->curr_rgn=(void*)param_rgn;_tmp333->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp333->flags).in_notreadctxt=0;(_tmp333->flags).in_lhs=0;(_tmp333->flags).in_new=0;(_tmp333->flags).abstract_ok=0;_tmp333->flags;});_tmp333->fnrgn=(struct _RegionHandle*)r;_tmp333;});};}
# 1331
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 1334
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp364->identity=Cyc_Tcutil_new_tvar_id();_tmp364->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp364;});
struct Cyc_List_List*_tmp344=({struct Cyc_List_List*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363->hd=rgn0;_tmp363->tl=0;_tmp363;});
struct Cyc_RgnOrder_RgnPO*_tmp345=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp362;_tmp362.tag=24;_tmp362.f1=0;_tmp362;});_tmp361;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp360;_tmp360.tag=2;_tmp360.f1=rgn0;_tmp360;});_tmp35F;});
struct Cyc_List_List*_tmp346=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp347=*((struct _tuple8*)args->hd);struct _tuple8 _tmp348=_tmp347;struct _dyneither_ptr*_tmp349;struct Cyc_Absyn_Tqual _tmp34A;void*_tmp34B;_LL179: _tmp349=_tmp348.f1;_tmp34A=_tmp348.f2;_tmp34B=_tmp348.f3;_LL17A:;
if(_tmp349 != 0){
struct Cyc_Absyn_Vardecl*_tmp34C=({struct Cyc_Absyn_Vardecl*_tmp354=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp354[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp355;_tmp355.sc=Cyc_Absyn_Public;_tmp355.name=({struct _tuple0*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->f1=Cyc_Absyn_Loc_n;_tmp356->f2=_tmp349;_tmp356;});_tmp355.tq=_tmp34A;_tmp355.type=_tmp34B;_tmp355.initializer=0;_tmp355.rgn=param_rgn;_tmp355.attributes=0;_tmp355.escapes=0;_tmp355;});_tmp354;});
# 1350
_tmp346=({struct Cyc_List_List*_tmp34D=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp34D[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp34E;_tmp34E.hd=_tmp34C;_tmp34E.tl=_tmp346;_tmp34E;});_tmp34D;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp34F=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp353;_tmp353.tag=3;_tmp353.f1=_tmp34C;_tmp353;});_tmp352;});
struct _dyneither_ptr*_tmp350=_tmp349;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp351=_region_malloc(r,sizeof(*_tmp351));_tmp351->v=_tmp350;_tmp351->b=(void*)_tmp34F;_tmp351->tl=locals;_tmp351;});};}}
# 1356
return({struct Cyc_Tcenv_Fenv*_tmp357=_region_malloc(r,sizeof(*_tmp357));_tmp357->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp35E=_region_malloc(r,sizeof(*_tmp35E));_tmp35E->frgn=r;_tmp35E->return_typ=ret_type;_tmp35E->seen_labels=
# 1360
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp35E->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp35E->delayed_effect_checks=0;_tmp35E->delayed_constraint_checks=0;_tmp35E;});_tmp357->type_vars=(struct Cyc_List_List*)_tmp344;_tmp357->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp345;_tmp357->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp357->encloser=(struct Cyc_Absyn_Stmt*)
# 1368
Cyc_Absyn_skip_stmt(0);_tmp357->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp35D=_region_malloc(r,sizeof(*_tmp35D));_tmp35D->ctrl_rgn=r;_tmp35D->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp35D->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp35D->fallthru_clause=0;_tmp35D->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val;_tmp35D->try_depth=0;_tmp35D;});_tmp357->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp359;_tmp359.tag=24;_tmp359.f1=({struct Cyc_List_List*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp35C;_tmp35C.tag=23;_tmp35C.f1=param_rgn;_tmp35C;});_tmp35B;});_tmp35A->tl=0;_tmp35A;});_tmp359;});_tmp358;}));_tmp357->curr_rgn=(void*)param_rgn;_tmp357->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp357->flags).in_notreadctxt=0;(_tmp357->flags).in_lhs=0;(_tmp357->flags).in_new=0;(_tmp357->flags).abstract_ok=0;_tmp357->flags;});_tmp357->fnrgn=(struct _RegionHandle*)r;_tmp357;});}
# 1377
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp366=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp367;struct Cyc_RgnOrder_RgnPO*_tmp368;struct Cyc_List_List*_tmp369;struct Cyc_Tcenv_SharedFenv*_tmp36A;struct _RegionHandle*_tmp36B;_LL17C: _tmp367=_tmp366->locals;_tmp368=_tmp366->region_order;_tmp369=_tmp366->type_vars;_tmp36A=_tmp366->shared;_tmp36B=_tmp366->fnrgn;_LL17D:;{
# 1380
struct _RegionHandle*_tmp36C=_tmp36A->frgn;
const struct Cyc_Tcenv_Bindings*_tmp36D=_tmp367;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->name=({struct _dyneither_ptr*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=0;_tmp3A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp3A6[1]={& _tmp3A8};Cyc_aprintf(({const char*_tmp3A7="`%s";_tag_dyneither(_tmp3A7,sizeof(char),4);}),_tag_dyneither(_tmp3A6,sizeof(void*),1));});});_tmp3A5;});_tmp3A4->identity=
# 1384
Cyc_Tcutil_new_tvar_id();_tmp3A4->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp3A4;});
# 1386
{struct Cyc_List_List*_tmp36E=fd->tvs;for(0;_tmp36E != 0;_tmp36E=_tmp36E->tl){
struct Cyc_Absyn_Kind*_tmp36F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp36E->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp370=_tmp36F;enum Cyc_Absyn_KindQual _tmp371;enum Cyc_Absyn_AliasQual _tmp372;_LL17F: _tmp371=_tmp370->kind;_tmp372=_tmp370->aliasqual;_LL180:;
if(_tmp371 == Cyc_Absyn_RgnKind){
if(_tmp372 == Cyc_Absyn_Aliasable)
_tmp368=Cyc_RgnOrder_add_youngest(_tmp36C,_tmp368,(struct Cyc_Absyn_Tvar*)_tmp36E->hd,0,0);else{
# 1392
({void*_tmp373=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp374="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp374,sizeof(char),39);}),_tag_dyneither(_tmp373,sizeof(void*),0));});}}}}
# 1394
_tmp368=Cyc_RgnOrder_add_youngest(_tmp36C,_tmp368,rgn0,0,0);{
struct Cyc_List_List*_tmp375=({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->hd=rgn0;_tmp3A3->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp369);_tmp3A3;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp375);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3A2;_tmp3A2.tag=2;_tmp3A2.f1=rgn0;_tmp3A2;});_tmp3A1;});
struct Cyc_List_List*_tmp376=0;
{struct Cyc_List_List*_tmp377=fd->args;for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
struct Cyc_Absyn_Vardecl*_tmp378=({struct Cyc_Absyn_Vardecl*_tmp380=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp380[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp381;_tmp381.sc=Cyc_Absyn_Public;_tmp381.name=({struct _tuple0*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->f1=Cyc_Absyn_Loc_n;_tmp382->f2=(*((struct _tuple8*)_tmp377->hd)).f1;_tmp382;});_tmp381.tq=(*((struct _tuple8*)_tmp377->hd)).f2;_tmp381.type=(*((struct _tuple8*)_tmp377->hd)).f3;_tmp381.initializer=0;_tmp381.rgn=param_rgn;_tmp381.attributes=0;_tmp381.escapes=0;_tmp381;});_tmp380;});
# 1407
_tmp376=({struct Cyc_List_List*_tmp379=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp379[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp37A;_tmp37A.hd=_tmp378;_tmp37A.tl=_tmp376;_tmp37A;});_tmp379;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37B=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp37F;_tmp37F.tag=3;_tmp37F.f1=_tmp378;_tmp37F;});_tmp37E;});
struct _dyneither_ptr*_tmp37C=(*((struct _tuple8*)_tmp377->hd)).f1;
_tmp36D=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp37D=_region_malloc(_tmp36C,sizeof(*_tmp37D));_tmp37D->v=_tmp37C;_tmp37D->b=(void*)_tmp37B;_tmp37D->tl=_tmp36D;_tmp37D;});};}}
# 1412
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp383=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp384=_tmp383;struct _dyneither_ptr*_tmp385;struct Cyc_Absyn_Tqual _tmp386;void*_tmp387;int _tmp388;_LL182: _tmp385=_tmp384.name;_tmp386=_tmp384.tq;_tmp387=_tmp384.type;_tmp388=_tmp384.inject;_LL183:;
if(_tmp385 != 0){
void*_tmp389=Cyc_Absyn_dyneither_typ(_tmp387,param_rgn,_tmp386,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp38A=({struct Cyc_Absyn_Vardecl*_tmp391=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp391[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp392;_tmp392.sc=Cyc_Absyn_Public;_tmp392.name=({struct _tuple0*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->f1=Cyc_Absyn_Loc_n;_tmp393->f2=_tmp385;_tmp393;});_tmp392.tq=
# 1418
Cyc_Absyn_empty_tqual(0);_tmp392.type=_tmp389;_tmp392.initializer=0;_tmp392.rgn=param_rgn;_tmp392.attributes=0;_tmp392.escapes=0;_tmp392;});_tmp391;});
# 1423
_tmp376=({struct Cyc_List_List*_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B->hd=_tmp38A;_tmp38B->tl=_tmp376;_tmp38B;});{
struct _dyneither_ptr*_tmp38C=_tmp385;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp38D=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp390;_tmp390.tag=3;_tmp390.f1=_tmp38A;_tmp390;});_tmp38F;});
_tmp36D=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp38E=_region_malloc(_tmp36C,sizeof(*_tmp38E));_tmp38E->v=_tmp38C;_tmp38E->b=(void*)_tmp38D;_tmp38E->tl=_tmp36D;_tmp38E;});};}else{
# 1428
({void*_tmp394=0;Cyc_Tcutil_terr(loc,({const char*_tmp395="missing name for varargs";_tag_dyneither(_tmp395,sizeof(char),25);}),_tag_dyneither(_tmp394,sizeof(void*),0));});}}
# 1430
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp396=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp396[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp397;_tmp397.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp376);_tmp397;});_tmp396;});
return({struct Cyc_Tcenv_Fenv*_tmp398=_region_malloc(_tmp36C,sizeof(*_tmp398));_tmp398->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp3A0=_region_malloc(_tmp36C,sizeof(*_tmp3A0));_tmp3A0->frgn=_tmp36C;_tmp3A0->return_typ=fd->ret_type;_tmp3A0->seen_labels=
# 1435
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp36C,Cyc_strptrcmp);_tmp3A0->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp36C,Cyc_strptrcmp);_tmp3A0->delayed_effect_checks=0;_tmp3A0->delayed_constraint_checks=0;_tmp3A0;});_tmp398->type_vars=(struct Cyc_List_List*)_tmp375;_tmp398->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp368;_tmp398->locals=(const struct Cyc_Tcenv_Bindings*)_tmp36D;_tmp398->encloser=(struct Cyc_Absyn_Stmt*)fd->body;_tmp398->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp39F=_region_malloc(_tmp36C,sizeof(*_tmp39F));_tmp39F->ctrl_rgn=_tmp36C;_tmp39F->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp39F->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp39F->fallthru_clause=0;_tmp39F->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val;_tmp39F->try_depth=0;_tmp39F;});_tmp398->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp39A;_tmp39A.tag=24;_tmp39A.f1=({struct Cyc_List_List*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp39E;_tmp39E.tag=23;_tmp39E.f1=param_rgn;_tmp39E;});_tmp39D;});_tmp39B->tl=({struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->hd=(void*)_check_null(fd->effect);_tmp39C->tl=0;_tmp39C;});_tmp39B;});_tmp39A;});_tmp399;}));_tmp398->curr_rgn=(void*)param_rgn;_tmp398->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp398->flags).in_notreadctxt=0;(_tmp398->flags).in_lhs=0;(_tmp398->flags).in_new=0;(_tmp398->flags).abstract_ok=0;_tmp398->flags;});_tmp398->fnrgn=(struct _RegionHandle*)_tmp36B;_tmp398;});};};};}
