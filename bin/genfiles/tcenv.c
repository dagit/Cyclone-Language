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
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 50 "string.h"
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 428 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 444
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
# 942
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 962
extern void*Cyc_Absyn_empty_effect;
# 964
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 997
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 82
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 84
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 96 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 100
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 104
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 111
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 114
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 117
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 119
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 122
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 125
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 127
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 140
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 148
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 153
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 158
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 161
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 169
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 171
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 173
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 175
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 177
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 179
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 182
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 185
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 57
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 61
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 64
void*Cyc_Tcutil_compress(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 138
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 141
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 175
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 179
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 232 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 299
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 301
int Cyc_Tcutil_new_tvar_id();
# 303
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 51 "tcenv.cyc"
typedef struct Cyc_Set_Set*Cyc_Tcenv_set_t;
typedef unsigned int Cyc_Tcenv_seg_t;char Cyc_Tcenv_Env_error[10]="Env_error";
# 55
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 57
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 60
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 101
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct _tuple10*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};struct _tuple12{void*f1;int f2;};
# 146 "tcenv.cyc"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 148
struct Cyc_Tcenv_Genv*_tmp1=({struct Cyc_Tcenv_Genv*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->grgn=r;_tmp8->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp8->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp8->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp8->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp8->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp8;});
_tmp1->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp1->datatypedecls,Cyc_Absyn_exn_name,({struct Cyc_Absyn_Datatypedecl**_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2[0]=Cyc_Absyn_exn_tud;_tmp2;}));
{struct Cyc_List_List*_tmp3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){
_tmp1->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp1->ordinaries,((struct Cyc_Absyn_Datatypefield*)_tmp3->hd)->name,({struct _tuple12*_tmp4=_region_malloc(r,sizeof(*_tmp4));_tmp4->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp6;_tmp6.tag=2;_tmp6.f1=Cyc_Absyn_exn_tud;_tmp6.f2=(struct Cyc_Absyn_Datatypefield*)_tmp3->hd;_tmp6;});_tmp5;});_tmp4->f2=1;_tmp4;}));}}
# 159
return({struct Cyc_Tcenv_Tenv*_tmp7=_region_malloc(r,sizeof(*_tmp7));_tmp7->ns=0;_tmp7->ae=_tmp1;_tmp7->le=0;_tmp7->allow_valueof=0;_tmp7->in_extern_c_include=0;_tmp7;});}
# 161
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple12*_tmp9=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
if(is_use)
(*_tmp9).f2=1;
return(*_tmp9).f1;}
# 167
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->aggrdecls,q);}
# 170
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);}
# 173
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);_tmpA;});}
# 176
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);}
# 179
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->typedefs,q);}
# 185
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmpB=te->le;
if(_tmpB == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmpB;}
# 191
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC=te->le;
if(_tmpC != 0){
struct Cyc_Tcenv_Fenv*_tmpD=_tmpC;struct Cyc_Tcenv_Fenv*_tmpE=_tmpD;struct _RegionHandle*_tmpF;_LL1: _tmpF=_tmpE->fnrgn;_LL2:;
return _tmpF;}
# 197
return Cyc_Core_heap_region;}
# 200
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp10="put_fenv";_tag_dyneither(_tmp10,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp11=_region_malloc(l,sizeof(*_tmp11));_tmp11->ns=te->ns;_tmp11->ae=te->ae;_tmp11->le=fe;_tmp11->allow_valueof=te->allow_valueof;_tmp11->in_extern_c_include=te->in_extern_c_include;_tmp11;});}
# 205
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp12=_region_malloc(l,sizeof(*_tmp12));_tmp12->ns=te->ns;_tmp12->ae=te->ae;_tmp12->le=0;_tmp12->allow_valueof=te->allow_valueof;_tmp12->in_extern_c_include=te->in_extern_c_include;_tmp12;});}
# 209
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 211
struct Cyc_Tcenv_Fenv _tmp13=*f;struct Cyc_Tcenv_Fenv _tmp14=_tmp13;struct Cyc_Tcenv_SharedFenv*_tmp1D;struct Cyc_List_List*_tmp1C;struct Cyc_RgnOrder_RgnPO*_tmp1B;const struct _tuple10*_tmp1A;void*_tmp19;void*_tmp18;struct Cyc_Tcenv_FenvFlags _tmp17;struct _RegionHandle*_tmp16;_LL4: _tmp1D=_tmp14.shared;_tmp1C=_tmp14.type_vars;_tmp1B=_tmp14.region_order;_tmp1A=_tmp14.ctrl_env;_tmp19=_tmp14.capability;_tmp18=_tmp14.curr_rgn;_tmp17=_tmp14.flags;_tmp16=_tmp14.fnrgn;_LL5:;
# 213
return({struct Cyc_Tcenv_Fenv*_tmp15=_region_malloc(l,sizeof(*_tmp15));_tmp15->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1D;_tmp15->type_vars=(struct Cyc_List_List*)_tmp1C;_tmp15->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1B;_tmp15->ctrl_env=(const struct _tuple10*)_tmp1A;_tmp15->capability=(void*)_tmp19;_tmp15->curr_rgn=(void*)_tmp18;_tmp15->flags=(struct Cyc_Tcenv_FenvFlags)_tmp17;_tmp15->fnrgn=(struct _RegionHandle*)l;_tmp15;});}
# 218
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1E=Cyc_Tcenv_get_fenv(te,({const char*_tmp24="return_typ";_tag_dyneither(_tmp24,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmp1F=_tmp1E;struct Cyc_Tcenv_SharedFenv*_tmp23;_LL7: _tmp23=_tmp1F->shared;_LL8:;{
struct Cyc_Tcenv_SharedFenv _tmp20=*_tmp23;struct Cyc_Tcenv_SharedFenv _tmp21=_tmp20;void*_tmp22;_LLA: _tmp22=_tmp21.return_typ;_LLB:;
return _tmp22;};}
# 224
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmp26=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmp25));struct Cyc_Tcenv_Fenv _tmp27=_tmp26;struct Cyc_List_List*_tmp28;_LLD: _tmp28=_tmp27.type_vars;_LLE:;
return _tmp28;};}
# 231
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->v=Cyc_Tcenv_lookup_type_vars(te);_tmp29;});}
# 235
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 237
struct Cyc_Tcenv_Fenv*_tmp2A=
# 239
Cyc_Tcenv_get_fenv(te,({const char*_tmp36="add_type_vars";_tag_dyneither(_tmp36,sizeof(char),14);}));
# 237
struct Cyc_Tcenv_Fenv*_tmp2B=_tmp2A;struct Cyc_Tcenv_SharedFenv*_tmp35;struct Cyc_List_List*_tmp34;struct Cyc_RgnOrder_RgnPO*_tmp33;const struct _tuple10*_tmp32;void*_tmp31;void*_tmp30;struct Cyc_Tcenv_FenvFlags _tmp2F;_LL10: _tmp35=_tmp2B->shared;_tmp34=_tmp2B->type_vars;_tmp33=_tmp2B->region_order;_tmp32=_tmp2B->ctrl_env;_tmp31=_tmp2B->capability;_tmp30=_tmp2B->curr_rgn;_tmp2F=_tmp2B->flags;_LL11:;
# 241
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp2C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp34);
Cyc_Tcutil_check_unique_tvars(loc,_tmp2C);{
struct Cyc_Tcenv_Fenv*_tmp2D=({struct Cyc_Tcenv_Fenv*_tmp2E=_region_malloc(r,sizeof(*_tmp2E));_tmp2E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp35;_tmp2E->type_vars=(struct Cyc_List_List*)_tmp2C;_tmp2E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp33;_tmp2E->ctrl_env=(const struct _tuple10*)_tmp32;_tmp2E->capability=(void*)_tmp31;_tmp2E->curr_rgn=(void*)_tmp30;_tmp2E->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2F;_tmp2E->fnrgn=(struct _RegionHandle*)r;_tmp2E;});
# 246
return Cyc_Tcenv_put_fenv(r,te,_tmp2D);};};}
# 249
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp37=te->le;
if(_tmp37 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 254
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv(r,_tmp37);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 259
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 261
struct Cyc_Tcenv_Fenv*_tmp38=te->le;
if(_tmp38 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp39=*_tmp38;struct Cyc_Tcenv_Fenv _tmp3A=_tmp39;struct Cyc_Tcenv_SharedFenv*_tmp45;struct Cyc_List_List*_tmp44;struct Cyc_RgnOrder_RgnPO*_tmp43;const struct _tuple10*_tmp42;void*_tmp41;void*_tmp40;int _tmp3F;int _tmp3E;int _tmp3D;_LL13: _tmp45=_tmp3A.shared;_tmp44=_tmp3A.type_vars;_tmp43=_tmp3A.region_order;_tmp42=_tmp3A.ctrl_env;_tmp41=_tmp3A.capability;_tmp40=_tmp3A.curr_rgn;_tmp3F=(_tmp3A.flags).in_notreadctxt;_tmp3E=(_tmp3A.flags).in_lhs;_tmp3D=(_tmp3A.flags).abstract_ok;_LL14:;{
# 266
struct Cyc_Tcenv_Fenv*_tmp3B=({struct Cyc_Tcenv_Fenv*_tmp3C=_region_malloc(r,sizeof(*_tmp3C));_tmp3C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp45;_tmp3C->type_vars=(struct Cyc_List_List*)_tmp44;_tmp3C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp43;_tmp3C->ctrl_env=(const struct _tuple10*)_tmp42;_tmp3C->capability=(void*)_tmp41;_tmp3C->curr_rgn=(void*)_tmp40;_tmp3C->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp3C->flags).in_notreadctxt=_tmp3F;(_tmp3C->flags).in_lhs=_tmp3E;(_tmp3C->flags).in_new=(int)status;(_tmp3C->flags).abstract_ok=_tmp3D;_tmp3C->flags;});_tmp3C->fnrgn=(struct _RegionHandle*)r;_tmp3C;});
# 269
return Cyc_Tcenv_put_fenv(r,te,_tmp3B);};};}
# 272
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp46=te->le;
if(_tmp46 == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp47=_tmp46;struct Cyc_Tcenv_Fenv*_tmp48=_tmp47;int _tmp49;_LL16: _tmp49=(_tmp48->flags).in_new;_LL17:;
return(enum Cyc_Tcenv_NewStatus)_tmp49;};}
# 279
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp4A=te->le;
if(_tmp4A == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp4B=_tmp4A;struct Cyc_Tcenv_Fenv*_tmp4C=_tmp4B;int _tmp4D;_LL19: _tmp4D=(_tmp4C->flags).abstract_ok;_LL1A:;
return _tmp4D;};}
# 286
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 288
struct Cyc_Tcenv_Fenv*_tmp4E=te->le;
if(_tmp4E == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp4F=*_tmp4E;struct Cyc_Tcenv_Fenv _tmp50=_tmp4F;struct Cyc_Tcenv_SharedFenv*_tmp5B;struct Cyc_List_List*_tmp5A;struct Cyc_RgnOrder_RgnPO*_tmp59;const struct _tuple10*_tmp58;void*_tmp57;void*_tmp56;int _tmp55;int _tmp54;int _tmp53;_LL1C: _tmp5B=_tmp50.shared;_tmp5A=_tmp50.type_vars;_tmp59=_tmp50.region_order;_tmp58=_tmp50.ctrl_env;_tmp57=_tmp50.capability;_tmp56=_tmp50.curr_rgn;_tmp55=(_tmp50.flags).in_notreadctxt;_tmp54=(_tmp50.flags).in_lhs;_tmp53=(_tmp50.flags).in_new;_LL1D:;{
# 293
struct Cyc_Tcenv_Fenv*_tmp51=({struct Cyc_Tcenv_Fenv*_tmp52=_region_malloc(r,sizeof(*_tmp52));_tmp52->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp5B;_tmp52->type_vars=(struct Cyc_List_List*)_tmp5A;_tmp52->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp59;_tmp52->ctrl_env=(const struct _tuple10*)_tmp58;_tmp52->capability=(void*)_tmp57;_tmp52->curr_rgn=(void*)_tmp56;_tmp52->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp52->flags).in_notreadctxt=_tmp55;(_tmp52->flags).in_lhs=_tmp54;(_tmp52->flags).in_new=_tmp53;(_tmp52->flags).abstract_ok=1;_tmp52->flags;});_tmp52->fnrgn=(struct _RegionHandle*)r;_tmp52;});
# 296
return Cyc_Tcenv_put_fenv(r,te,_tmp51);};};}
# 299
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 301
struct Cyc_Tcenv_Fenv*_tmp5C=te->le;
if(_tmp5C == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp5D=*_tmp5C;struct Cyc_Tcenv_Fenv _tmp5E=_tmp5D;struct Cyc_Tcenv_SharedFenv*_tmp69;struct Cyc_List_List*_tmp68;struct Cyc_RgnOrder_RgnPO*_tmp67;const struct _tuple10*_tmp66;void*_tmp65;void*_tmp64;int _tmp63;int _tmp62;int _tmp61;_LL1F: _tmp69=_tmp5E.shared;_tmp68=_tmp5E.type_vars;_tmp67=_tmp5E.region_order;_tmp66=_tmp5E.ctrl_env;_tmp65=_tmp5E.capability;_tmp64=_tmp5E.curr_rgn;_tmp63=(_tmp5E.flags).in_notreadctxt;_tmp62=(_tmp5E.flags).in_lhs;_tmp61=(_tmp5E.flags).in_new;_LL20:;{
# 306
struct Cyc_Tcenv_Fenv*_tmp5F=({struct Cyc_Tcenv_Fenv*_tmp60=_region_malloc(r,sizeof(*_tmp60));_tmp60->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp69;_tmp60->type_vars=(struct Cyc_List_List*)_tmp68;_tmp60->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp67;_tmp60->ctrl_env=(const struct _tuple10*)_tmp66;_tmp60->capability=(void*)_tmp65;_tmp60->curr_rgn=(void*)_tmp64;_tmp60->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp60->flags).in_notreadctxt=_tmp63;(_tmp60->flags).in_lhs=_tmp62;(_tmp60->flags).in_new=_tmp61;(_tmp60->flags).abstract_ok=0;_tmp60->flags;});_tmp60->fnrgn=(struct _RegionHandle*)r;_tmp60;});
# 309
return Cyc_Tcenv_put_fenv(r,te,_tmp5F);};};}
# 312
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp6A=te->le;
if(_tmp6A == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp6B=*_tmp6A;struct Cyc_Tcenv_Fenv _tmp6C=_tmp6B;struct Cyc_Tcenv_SharedFenv*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_RgnOrder_RgnPO*_tmp75;const struct _tuple10*_tmp74;void*_tmp73;void*_tmp72;int _tmp71;int _tmp70;int _tmp6F;_LL22: _tmp77=_tmp6C.shared;_tmp76=_tmp6C.type_vars;_tmp75=_tmp6C.region_order;_tmp74=_tmp6C.ctrl_env;_tmp73=_tmp6C.capability;_tmp72=_tmp6C.curr_rgn;_tmp71=(_tmp6C.flags).in_lhs;_tmp70=(_tmp6C.flags).in_new;_tmp6F=(_tmp6C.flags).abstract_ok;_LL23:;{
# 318
struct Cyc_Tcenv_Fenv*_tmp6D=({struct Cyc_Tcenv_Fenv*_tmp6E=_region_malloc(r,sizeof(*_tmp6E));_tmp6E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp77;_tmp6E->type_vars=(struct Cyc_List_List*)_tmp76;_tmp6E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp75;_tmp6E->ctrl_env=(const struct _tuple10*)_tmp74;_tmp6E->capability=(void*)_tmp73;_tmp6E->curr_rgn=(void*)_tmp72;_tmp6E->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp6E->flags).in_notreadctxt=1;(_tmp6E->flags).in_lhs=_tmp71;(_tmp6E->flags).in_new=_tmp70;(_tmp6E->flags).abstract_ok=_tmp6F;_tmp6E->flags;});_tmp6E->fnrgn=(struct _RegionHandle*)r;_tmp6E;});
# 321
return Cyc_Tcenv_put_fenv(r,te,_tmp6D);};};}
# 324
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp78=te->le;
if(_tmp78 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp79=*_tmp78;struct Cyc_Tcenv_Fenv _tmp7A=_tmp79;struct Cyc_Tcenv_SharedFenv*_tmp85;struct Cyc_List_List*_tmp84;struct Cyc_RgnOrder_RgnPO*_tmp83;const struct _tuple10*_tmp82;void*_tmp81;void*_tmp80;int _tmp7F;int _tmp7E;int _tmp7D;_LL25: _tmp85=_tmp7A.shared;_tmp84=_tmp7A.type_vars;_tmp83=_tmp7A.region_order;_tmp82=_tmp7A.ctrl_env;_tmp81=_tmp7A.capability;_tmp80=_tmp7A.curr_rgn;_tmp7F=(_tmp7A.flags).in_lhs;_tmp7E=(_tmp7A.flags).in_new;_tmp7D=(_tmp7A.flags).abstract_ok;_LL26:;{
# 330
struct Cyc_Tcenv_Fenv*_tmp7B=({struct Cyc_Tcenv_Fenv*_tmp7C=_region_malloc(r,sizeof(*_tmp7C));_tmp7C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp85;_tmp7C->type_vars=(struct Cyc_List_List*)_tmp84;_tmp7C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp83;_tmp7C->ctrl_env=(const struct _tuple10*)_tmp82;_tmp7C->capability=(void*)_tmp81;_tmp7C->curr_rgn=(void*)_tmp80;_tmp7C->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp7C->flags).in_notreadctxt=0;(_tmp7C->flags).in_lhs=_tmp7F;(_tmp7C->flags).in_new=_tmp7E;(_tmp7C->flags).abstract_ok=_tmp7D;_tmp7C->flags;});_tmp7C->fnrgn=(struct _RegionHandle*)r;_tmp7C;});
# 333
return Cyc_Tcenv_put_fenv(r,te,_tmp7B);};};}
# 336
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp86=te->le;
if(_tmp86 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp87=_tmp86;struct Cyc_Tcenv_Fenv*_tmp88=_tmp87;int _tmp89;_LL28: _tmp89=(_tmp88->flags).in_notreadctxt;_LL29:;
return _tmp89;};}
# 343
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp8A=te->le;
if(_tmp8A == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp8B=*_tmp8A;struct Cyc_Tcenv_Fenv _tmp8C=_tmp8B;struct Cyc_Tcenv_SharedFenv*_tmp97;struct Cyc_List_List*_tmp96;struct Cyc_RgnOrder_RgnPO*_tmp95;const struct _tuple10*_tmp94;void*_tmp93;void*_tmp92;int _tmp91;int _tmp90;int _tmp8F;_LL2B: _tmp97=_tmp8C.shared;_tmp96=_tmp8C.type_vars;_tmp95=_tmp8C.region_order;_tmp94=_tmp8C.ctrl_env;_tmp93=_tmp8C.capability;_tmp92=_tmp8C.curr_rgn;_tmp91=(_tmp8C.flags).in_notreadctxt;_tmp90=(_tmp8C.flags).in_new;_tmp8F=(_tmp8C.flags).abstract_ok;_LL2C:;{
# 349
struct Cyc_Tcenv_Fenv*_tmp8D=({struct Cyc_Tcenv_Fenv*_tmp8E=_region_malloc(r,sizeof(*_tmp8E));_tmp8E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp97;_tmp8E->type_vars=(struct Cyc_List_List*)_tmp96;_tmp8E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp95;_tmp8E->ctrl_env=(const struct _tuple10*)_tmp94;_tmp8E->capability=(void*)_tmp93;_tmp8E->curr_rgn=(void*)_tmp92;_tmp8E->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp8E->flags).in_notreadctxt=_tmp91;(_tmp8E->flags).in_lhs=1;(_tmp8E->flags).in_new=_tmp90;(_tmp8E->flags).abstract_ok=_tmp8F;_tmp8E->flags;});_tmp8E->fnrgn=(struct _RegionHandle*)r;_tmp8E;});
# 352
return Cyc_Tcenv_put_fenv(r,te,_tmp8D);};};}
# 355
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp98=te->le;
if(_tmp98 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp99=*_tmp98;struct Cyc_Tcenv_Fenv _tmp9A=_tmp99;struct Cyc_Tcenv_SharedFenv*_tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_RgnOrder_RgnPO*_tmpA3;const struct _tuple10*_tmpA2;void*_tmpA1;void*_tmpA0;int _tmp9F;int _tmp9E;int _tmp9D;_LL2E: _tmpA5=_tmp9A.shared;_tmpA4=_tmp9A.type_vars;_tmpA3=_tmp9A.region_order;_tmpA2=_tmp9A.ctrl_env;_tmpA1=_tmp9A.capability;_tmpA0=_tmp9A.curr_rgn;_tmp9F=(_tmp9A.flags).in_notreadctxt;_tmp9E=(_tmp9A.flags).in_new;_tmp9D=(_tmp9A.flags).abstract_ok;_LL2F:;{
# 361
struct Cyc_Tcenv_Fenv*_tmp9B=({struct Cyc_Tcenv_Fenv*_tmp9C=_region_malloc(r,sizeof(*_tmp9C));_tmp9C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA5;_tmp9C->type_vars=(struct Cyc_List_List*)_tmpA4;_tmp9C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA3;_tmp9C->ctrl_env=(const struct _tuple10*)_tmpA2;_tmp9C->capability=(void*)_tmpA1;_tmp9C->curr_rgn=(void*)_tmpA0;_tmp9C->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp9C->flags).in_notreadctxt=_tmp9F;(_tmp9C->flags).in_lhs=0;(_tmp9C->flags).in_new=_tmp9E;(_tmp9C->flags).abstract_ok=_tmp9D;_tmp9C->flags;});_tmp9C->fnrgn=(struct _RegionHandle*)r;_tmp9C;});
# 364
return Cyc_Tcenv_put_fenv(r,te,_tmp9B);};};}
# 367
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpA6=te->le;
if(_tmpA6 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpA7=_tmpA6;struct Cyc_Tcenv_Fenv*_tmpA8=_tmpA7;int _tmpA9;_LL31: _tmpA9=(_tmpA8->flags).in_lhs;_LL32:;
return _tmpA9;};}
# 376
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 378
struct Cyc_Tcenv_Fenv*_tmpAA=Cyc_Tcenv_get_fenv(te,({const char*_tmpAF="process_fallthru";_tag_dyneither(_tmpAF,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmpAB=_tmpAA;const struct _tuple10*_tmpAE;_LL34: _tmpAE=_tmpAB->ctrl_env;_LL35:;{
const struct _tuple10*_tmpAC=_tmpAE;
if(_tmpAC != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=(*_tmpAC).f1;_tmpAD;});
return _tmpAC;};}struct _tuple13{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 385
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 389
struct Cyc_Tcenv_Fenv*_tmpB0=
# 391
Cyc_Tcenv_get_fenv(te,({const char*_tmpC0="set_fallthru";_tag_dyneither(_tmpC0,sizeof(char),13);}));
# 389
struct Cyc_Tcenv_Fenv*_tmpB1=_tmpB0;struct Cyc_Tcenv_SharedFenv*_tmpBF;struct Cyc_List_List*_tmpBE;struct Cyc_RgnOrder_RgnPO*_tmpBD;const struct _tuple10*_tmpBC;void*_tmpBB;void*_tmpBA;struct Cyc_Tcenv_FenvFlags _tmpB9;_LL37: _tmpBF=_tmpB1->shared;_tmpBE=_tmpB1->type_vars;_tmpBD=_tmpB1->region_order;_tmpBC=_tmpB1->ctrl_env;_tmpBB=_tmpB1->capability;_tmpBA=_tmpB1->curr_rgn;_tmpB9=_tmpB1->flags;_LL38:;{
# 392
const struct _tuple10*_tmpB2=_tmpBC;
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmpB3->tl=ft_typ;_tmpB3;});}{
struct Cyc_List_List*_tmpB4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple13*_tmpB5=({struct _tuple13*_tmpB8=_region_malloc(r,sizeof(*_tmpB8));_tmpB8->f1=clause;_tmpB8->f2=new_tvs;_tmpB8->f3=_tmpB4;_tmpB8;});
struct Cyc_Tcenv_Fenv*_tmpB6=({struct Cyc_Tcenv_Fenv*_tmpB7=_region_malloc(r,sizeof(*_tmpB7));_tmpB7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpBF;_tmpB7->type_vars=(struct Cyc_List_List*)_tmpBE;_tmpB7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpBD;_tmpB7->ctrl_env=(const struct _tuple10*)((const struct _tuple10*)_tmpB5);_tmpB7->capability=(void*)_tmpBB;_tmpB7->curr_rgn=(void*)_tmpBA;_tmpB7->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB9;_tmpB7->fnrgn=(struct _RegionHandle*)r;_tmpB7;});
# 401
return Cyc_Tcenv_put_fenv(r,te,_tmpB6);};};}
# 404
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv(r,Cyc_Tcenv_get_fenv(te,({const char*_tmpC3="clear_fallthru";_tag_dyneither(_tmpC3,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmpC1=fe;const struct _tuple10*_tmpC2;_LL3A: _tmpC2=_tmpC1->ctrl_env;_LL3B:;
_tmpC2=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 411
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmpC4=_region_malloc(r2,sizeof(*_tmpC4));_tmpC4->ns=te->ns;_tmpC4->ae=te->ae;_tmpC4->le=0;_tmpC4->allow_valueof=1;_tmpC4->in_extern_c_include=te->in_extern_c_include;_tmpC4;});{
struct Cyc_Tcenv_Fenv _tmpC5=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmpC6=_tmpC5;struct Cyc_Tcenv_SharedFenv*_tmpD0;struct Cyc_List_List*_tmpCF;struct Cyc_RgnOrder_RgnPO*_tmpCE;const struct _tuple10*_tmpCD;void*_tmpCC;void*_tmpCB;struct Cyc_Tcenv_FenvFlags _tmpCA;_LL3D: _tmpD0=_tmpC6.shared;_tmpCF=_tmpC6.type_vars;_tmpCE=_tmpC6.region_order;_tmpCD=_tmpC6.ctrl_env;_tmpCC=_tmpC6.capability;_tmpCB=_tmpC6.curr_rgn;_tmpCA=_tmpC6.flags;_LL3E:;{
struct Cyc_Tcenv_Fenv*_tmpC7=({struct Cyc_Tcenv_Fenv*_tmpC9=_region_malloc(r2,sizeof(*_tmpC9));_tmpC9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpD0;_tmpC9->type_vars=(struct Cyc_List_List*)_tmpCF;_tmpC9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpCE;_tmpC9->ctrl_env=(const struct _tuple10*)_tmpCD;_tmpC9->capability=(void*)_tmpCC;_tmpC9->curr_rgn=(void*)_tmpCB;_tmpC9->flags=(struct Cyc_Tcenv_FenvFlags)_tmpCA;_tmpC9->fnrgn=(struct _RegionHandle*)r2;_tmpC9;});
return({struct Cyc_Tcenv_Tenv*_tmpC8=_region_malloc(r2,sizeof(*_tmpC8));_tmpC8->ns=te->ns;_tmpC8->ae=te->ae;_tmpC8->le=_tmpC7;_tmpC8->allow_valueof=1;_tmpC8->in_extern_c_include=te->in_extern_c_include;_tmpC8;});};};}
# 419
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmpD1=_region_malloc(r2,sizeof(*_tmpD1));_tmpD1->ns=te->ns;_tmpD1->ae=te->ae;_tmpD1->le=0;_tmpD1->allow_valueof=te->allow_valueof;_tmpD1->in_extern_c_include=1;_tmpD1;});{
struct Cyc_Tcenv_Fenv _tmpD2=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmpD3=_tmpD2;struct Cyc_Tcenv_SharedFenv*_tmpDD;struct Cyc_List_List*_tmpDC;struct Cyc_RgnOrder_RgnPO*_tmpDB;const struct _tuple10*_tmpDA;void*_tmpD9;void*_tmpD8;struct Cyc_Tcenv_FenvFlags _tmpD7;_LL40: _tmpDD=_tmpD3.shared;_tmpDC=_tmpD3.type_vars;_tmpDB=_tmpD3.region_order;_tmpDA=_tmpD3.ctrl_env;_tmpD9=_tmpD3.capability;_tmpD8=_tmpD3.curr_rgn;_tmpD7=_tmpD3.flags;_LL41:;{
struct Cyc_Tcenv_Fenv*_tmpD4=({struct Cyc_Tcenv_Fenv*_tmpD6=_region_malloc(r2,sizeof(*_tmpD6));_tmpD6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpDD;_tmpD6->type_vars=(struct Cyc_List_List*)_tmpDC;_tmpD6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpDB;_tmpD6->ctrl_env=(const struct _tuple10*)_tmpDA;_tmpD6->capability=(void*)_tmpD9;_tmpD6->curr_rgn=(void*)_tmpD8;_tmpD6->flags=(struct Cyc_Tcenv_FenvFlags)_tmpD7;_tmpD6->fnrgn=(struct _RegionHandle*)r2;_tmpD6;});
return({struct Cyc_Tcenv_Tenv*_tmpD5=_region_malloc(r2,sizeof(*_tmpD5));_tmpD5->ns=te->ns;_tmpD5->ae=te->ae;_tmpD5->le=_tmpD4;_tmpD5->allow_valueof=te->allow_valueof;_tmpD5->in_extern_c_include=1;_tmpD5;});};};}
# 427
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 430
struct Cyc_Tcenv_Fenv _tmpDE=*
# 432
Cyc_Tcenv_get_fenv(te,({const char*_tmpF2="add_region";_tag_dyneither(_tmpF2,sizeof(char),11);}));
# 430
struct Cyc_Tcenv_Fenv _tmpDF=_tmpDE;struct Cyc_Tcenv_SharedFenv*_tmpF1;struct Cyc_List_List*_tmpF0;struct Cyc_RgnOrder_RgnPO*_tmpEF;const struct _tuple10*_tmpEE;void*_tmpED;void*_tmpEC;struct Cyc_Tcenv_FenvFlags _tmpEB;_LL43: _tmpF1=_tmpDF.shared;_tmpF0=_tmpDF.type_vars;_tmpEF=_tmpDF.region_order;_tmpEE=_tmpDF.ctrl_env;_tmpED=_tmpDF.capability;_tmpEC=_tmpDF.curr_rgn;_tmpEB=_tmpDF.flags;_LL44:;
# 433
{void*_tmpE0=Cyc_Tcutil_compress(rgn);void*_tmpE1=_tmpE0;struct Cyc_Absyn_Tvar*_tmpE2;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpE1)->tag == 2){_LL46: _tmpE2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpE1)->f1;_LL47:
# 435
 _tmpEF=Cyc_RgnOrder_add_youngest(_tmpF1->frgn,_tmpEF,_tmpE2,resetable,opened);
# 437
goto _LL45;}else{_LL48: _LL49:
 goto _LL45;}_LL45:;}
# 440
_tmpED=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE4;_tmpE4.tag=24;_tmpE4.f1=({struct Cyc_List_List*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE8;_tmpE8.tag=23;_tmpE8.f1=rgn;_tmpE8;});_tmpE7;});_tmpE5->tl=({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=_tmpED;_tmpE6->tl=0;_tmpE6;});_tmpE5;});_tmpE4;});_tmpE3;});{
# 442
struct Cyc_Tcenv_Fenv*_tmpE9=({struct Cyc_Tcenv_Fenv*_tmpEA=_region_malloc(r,sizeof(*_tmpEA));_tmpEA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF1;_tmpEA->type_vars=(struct Cyc_List_List*)_tmpF0;_tmpEA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEF;_tmpEA->ctrl_env=(const struct _tuple10*)_tmpEE;_tmpEA->capability=(void*)_tmpED;_tmpEA->curr_rgn=(void*)_tmpEC;_tmpEA->flags=(struct Cyc_Tcenv_FenvFlags)_tmpEB;_tmpEA->fnrgn=(struct _RegionHandle*)r;_tmpEA;});
# 445
return Cyc_Tcenv_put_fenv(r,te,_tmpE9);};}
# 448
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 450
struct Cyc_Tcenv_Fenv _tmpF3=*
# 452
Cyc_Tcenv_get_fenv(te,({const char*_tmp108="new_named_block";_tag_dyneither(_tmp108,sizeof(char),16);}));
# 450
struct Cyc_Tcenv_Fenv _tmpF4=_tmpF3;struct Cyc_Tcenv_SharedFenv*_tmp107;struct Cyc_List_List*_tmp106;struct Cyc_RgnOrder_RgnPO*_tmp105;const struct _tuple10*_tmp104;void*_tmp103;void*_tmp102;struct Cyc_Tcenv_FenvFlags _tmp101;_LL4B: _tmp107=_tmpF4.shared;_tmp106=_tmpF4.type_vars;_tmp105=_tmpF4.region_order;_tmp104=_tmpF4.ctrl_env;_tmp103=_tmpF4.capability;_tmp102=_tmpF4.curr_rgn;_tmp101=_tmpF4.flags;_LL4C:;{
# 454
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp100="new_block";_tag_dyneither(_tmp100,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFF;_tmpFF.tag=2;_tmpFF.f1=block_rgn;_tmpFF;});_tmpFE;});
_tmp106=({struct Cyc_List_List*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=block_rgn;_tmpF5->tl=_tmp106;_tmpF5;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp106);
_tmp105=Cyc_RgnOrder_add_youngest(_tmp107->frgn,_tmp105,block_rgn,0,0);
_tmp103=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF7;_tmpF7.tag=24;_tmpF7.f1=({struct Cyc_List_List*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFB;_tmpFB.tag=23;_tmpFB.f1=block_typ;_tmpFB;});_tmpFA;});_tmpF8->tl=({struct Cyc_List_List*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->hd=_tmp103;_tmpF9->tl=0;_tmpF9;});_tmpF8;});_tmpF7;});_tmpF6;});
_tmp102=block_typ;{
struct Cyc_Tcenv_Fenv*_tmpFC=({struct Cyc_Tcenv_Fenv*_tmpFD=_region_malloc(r,sizeof(*_tmpFD));_tmpFD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp107;_tmpFD->type_vars=(struct Cyc_List_List*)_tmp106;_tmpFD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp105;_tmpFD->ctrl_env=(const struct _tuple10*)_tmp104;_tmpFD->capability=(void*)_tmp103;_tmpFD->curr_rgn=(void*)_tmp102;_tmpFD->flags=(struct Cyc_Tcenv_FenvFlags)_tmp101;_tmpFD->fnrgn=(struct _RegionHandle*)r;_tmpFD;});
# 464
return Cyc_Tcenv_put_fenv(r,te,_tmpFC);};};}
# 467
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 469
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 471
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple14{void*f1;void*f2;};
# 477
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 481
struct Cyc_Tcenv_Fenv _tmp10A=*
# 483
Cyc_Tcenv_get_fenv(te,({const char*_tmp115="new_outlives_constraints";_tag_dyneither(_tmp115,sizeof(char),25);}));
# 481
struct Cyc_Tcenv_Fenv _tmp10B=_tmp10A;struct Cyc_Tcenv_SharedFenv*_tmp114;struct Cyc_List_List*_tmp113;struct Cyc_RgnOrder_RgnPO*_tmp112;const struct _tuple10*_tmp111;void*_tmp110;void*_tmp10F;struct Cyc_Tcenv_FenvFlags _tmp10E;_LL4E: _tmp114=_tmp10B.shared;_tmp113=_tmp10B.type_vars;_tmp112=_tmp10B.region_order;_tmp111=_tmp10B.ctrl_env;_tmp110=_tmp10B.capability;_tmp10F=_tmp10B.curr_rgn;_tmp10E=_tmp10B.flags;_LL4F:;
# 484
for(0;cs != 0;cs=cs->tl){
_tmp112=
Cyc_RgnOrder_add_outlives_constraint(_tmp114->frgn,_tmp112,(*((struct _tuple14*)cs->hd)).f1,(*((struct _tuple14*)cs->hd)).f2,loc);}{
# 489
struct Cyc_Tcenv_Fenv*_tmp10C=({struct Cyc_Tcenv_Fenv*_tmp10D=_region_malloc(r,sizeof(*_tmp10D));_tmp10D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp114;_tmp10D->type_vars=(struct Cyc_List_List*)_tmp113;_tmp10D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp112;_tmp10D->ctrl_env=(const struct _tuple10*)_tmp111;_tmp10D->capability=(void*)_tmp110;_tmp10D->curr_rgn=(void*)_tmp10F;_tmp10D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp10E;_tmp10D->fnrgn=(struct _RegionHandle*)r;_tmp10D;});
# 492
return Cyc_Tcenv_put_fenv(r,te,_tmp10C);};}
# 495
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 503
void*_tmp116=Cyc_Tcutil_compress(r1);
void*_tmp117=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp118=Cyc_Tcutil_typ_kind(_tmp116);
struct Cyc_Absyn_Kind*_tmp119=Cyc_Tcutil_typ_kind(_tmp117);
# 509
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp118,_tmp119);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp119,_tmp118);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp11F;_tmp11F.tag=0;_tmp11F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp119));({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp118));({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp117));({struct Cyc_String_pa_PrintArg_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp116));({void*_tmp11A[4]={& _tmp11C,& _tmp11D,& _tmp11E,& _tmp11F};Cyc_Tcutil_terr(loc,({const char*_tmp11B="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp11B,sizeof(char),82);}),_tag_dyneither(_tmp11A,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 516
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp120=Cyc_Tcutil_swap_kind(_tmp117,Cyc_Tcutil_kind_to_bound(_tmp118));struct _tuple11 _tmp121=_tmp120;struct Cyc_Absyn_Tvar*_tmp124;void*_tmp123;_LL51: _tmp124=_tmp121.f1;_tmp123=_tmp121.f2;_LL52:;
*oldtv=({struct _tuple11*_tmp122=_region_malloc(r,sizeof(*_tmp122));_tmp122->f1=_tmp124;_tmp122->f2=_tmp123;_tmp122;});}else{
# 520
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp125=Cyc_Tcutil_swap_kind(_tmp116,Cyc_Tcutil_kind_to_bound(_tmp119));struct _tuple11 _tmp126=_tmp125;struct Cyc_Absyn_Tvar*_tmp129;void*_tmp128;_LL54: _tmp129=_tmp126.f1;_tmp128=_tmp126.f2;_LL55:;
*oldtv=({struct _tuple11*_tmp127=_region_malloc(r,sizeof(*_tmp127));_tmp127->f1=_tmp129;_tmp127->f2=_tmp128;_tmp127;});}}}{
# 526
struct _RegionHandle*_tmp12A=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp12B=0;
if((_tmp116 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp116 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp116 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp12F;_tmp12F.tag=23;_tmp12F.f1=_tmp116;_tmp12F;});_tmp12E;});
_tmp12B=({struct Cyc_List_List*_tmp12C=_region_malloc(_tmp12A,sizeof(*_tmp12C));_tmp12C->hd=({struct _tuple14*_tmp12D=_region_malloc(_tmp12A,sizeof(*_tmp12D));_tmp12D->f1=eff1;_tmp12D->f2=_tmp117;_tmp12D;});_tmp12C->tl=_tmp12B;_tmp12C;});}
# 532
if((_tmp117 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp117 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp117 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp133;_tmp133.tag=23;_tmp133.f1=_tmp117;_tmp133;});_tmp132;});
_tmp12B=({struct Cyc_List_List*_tmp130=_region_malloc(_tmp12A,sizeof(*_tmp130));_tmp130->hd=({struct _tuple14*_tmp131=_region_malloc(_tmp12A,sizeof(*_tmp131));_tmp131->f1=eff2;_tmp131->f2=_tmp116;_tmp131;});_tmp130->tl=_tmp12B;_tmp130;});}
# 537
return Cyc_Tcenv_new_outlives_constraints(_tmp12A,te,_tmp12B,loc);};}
# 540
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp134=te->le;
if(_tmp134 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp134;
struct Cyc_Tcenv_Fenv*_tmp135=fe;void*_tmp136;_LL57: _tmp136=_tmp135->curr_rgn;_LL58:;
return _tmp136;};}
# 551
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp13F="check_rgn_accessible";_tag_dyneither(_tmp13F,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp137=fe;void*_tmp13E;struct Cyc_RgnOrder_RgnPO*_tmp13D;_LL5A: _tmp13E=_tmp137->capability;_tmp13D=_tmp137->region_order;_LL5B:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp13E) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp13E))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp13D,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp139;_tmp139.tag=23;_tmp139.f1=rgn;_tmp139;});_tmp138;}),_tmp13E))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp13A[1]={& _tmp13C};Cyc_Tcutil_terr(loc,({const char*_tmp13B="Expression accesses unavailable region %s";_tag_dyneither(_tmp13B,sizeof(char),42);}),_tag_dyneither(_tmp13A,sizeof(void*),1));});});}
# 563
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp140=
Cyc_Tcenv_get_fenv(te,({const char*_tmp14B="check_rgn_resetable";_tag_dyneither(_tmp14B,sizeof(char),20);}));
# 565
struct Cyc_Tcenv_Fenv*_tmp141=_tmp140;struct Cyc_RgnOrder_RgnPO*_tmp14A;_LL5D: _tmp14A=_tmp141->region_order;_LL5E:;{
# 567
void*_tmp142=Cyc_Tcutil_compress(rgn);void*_tmp143=_tmp142;struct Cyc_Absyn_Tvar*_tmp149;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->tag == 2){_LL60: _tmp149=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LL61:
# 569
 if(!Cyc_RgnOrder_is_region_resetable(_tmp14A,_tmp149))
({struct Cyc_String_pa_PrintArg_struct _tmp146;_tmp146.tag=0;_tmp146.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp144[1]={& _tmp146};Cyc_Tcutil_terr(loc,({const char*_tmp145="Region %s is not resetable";_tag_dyneither(_tmp145,sizeof(char),27);}),_tag_dyneither(_tmp144,sizeof(void*),1));});});
return;}else{_LL62: _LL63:
({void*_tmp147=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp148="check_rgn_resetable";_tag_dyneither(_tmp148,sizeof(char),20);}),_tag_dyneither(_tmp147,sizeof(void*),0));});}_LL5F:;};};}
# 579
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp14C=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 584
if(_tmp14C == 0){
void*_tmp14D=rt_a;switch(*((int*)_tmp14D)){case 22: _LL65: _LL66:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LL67: _LL68:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LL69: _LL6A:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL6B: _LL6C:
 return 0;}_LL64:;}{
# 592
struct Cyc_Tcenv_Fenv*fe=_tmp14C;
struct Cyc_Tcenv_Fenv*_tmp14E=fe;struct Cyc_RgnOrder_RgnPO*_tmp152;_LL6E: _tmp152=_tmp14E->region_order;_LL6F:;{
int _tmp14F=Cyc_RgnOrder_effect_outlives(_tmp152,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp151;_tmp151.tag=23;_tmp151.f1=rt_a;_tmp151;});_tmp150;}),rt_b);
# 598
return _tmp14F;};};}struct _tuple15{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 603
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp153=
Cyc_Tcenv_get_fenv(te,({const char*_tmp15A="check_effect_accessible";_tag_dyneither(_tmp15A,sizeof(char),24);}));
# 604
struct Cyc_Tcenv_Fenv*_tmp154=_tmp153;void*_tmp159;struct Cyc_RgnOrder_RgnPO*_tmp158;struct Cyc_Tcenv_SharedFenv*_tmp157;_LL71: _tmp159=_tmp154->capability;_tmp158=_tmp154->region_order;_tmp157=_tmp154->shared;_LL72:;
# 606
if(Cyc_Tcutil_subset_effect(0,eff,_tmp159))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp158,eff,_tmp159))
return;{
struct _RegionHandle*frgn=_tmp157->frgn;
_tmp157->delayed_effect_checks=({struct Cyc_List_List*_tmp155=_region_malloc(frgn,sizeof(*_tmp155));_tmp155->hd=({struct _tuple15*_tmp156=_region_malloc(frgn,sizeof(*_tmp156));_tmp156->f1=_tmp159;_tmp156->f2=eff;_tmp156->f3=_tmp158;_tmp156->f4=loc;_tmp156;});_tmp155->tl=_tmp157->delayed_effect_checks;_tmp155;});};}
# 615
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp15B=
Cyc_Tcenv_get_fenv(te,({const char*_tmp169="check_delayed_effects";_tag_dyneither(_tmp169,sizeof(char),22);}));
# 616
struct Cyc_Tcenv_Fenv*_tmp15C=_tmp15B;struct Cyc_Tcenv_SharedFenv*_tmp168;_LL74: _tmp168=_tmp15C->shared;_LL75:;{
# 618
struct Cyc_List_List*_tmp15D=_tmp168->delayed_effect_checks;
for(0;_tmp15D != 0;_tmp15D=_tmp15D->tl){
struct _tuple15*_tmp15E=(struct _tuple15*)_tmp15D->hd;struct _tuple15*_tmp15F=_tmp15E;void*_tmp167;void*_tmp166;struct Cyc_RgnOrder_RgnPO*_tmp165;unsigned int _tmp164;_LL77: _tmp167=_tmp15F->f1;_tmp166=_tmp15F->f2;_tmp165=_tmp15F->f3;_tmp164=_tmp15F->f4;_LL78:;
if(Cyc_Tcutil_subset_effect(1,_tmp166,_tmp167))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp165,_tmp166,_tmp167))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0;_tmp163.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp166));({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0;_tmp162.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp167));({void*_tmp160[2]={& _tmp162,& _tmp163};Cyc_Tcutil_terr(_tmp164,({const char*_tmp161="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp161,sizeof(char),51);}),_tag_dyneither(_tmp160,sizeof(void*),2));});});});}};}struct _tuple16{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 634
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 636
struct Cyc_Tcenv_Fenv*_tmp16A=te->le;
if(_tmp16A == 0){
# 639
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple14*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp16C;_tmp16C.tag=23;_tmp16C.f1=(*((struct _tuple14*)po->hd)).f2;_tmp16C;});_tmp16B;}),Cyc_Absyn_empty_effect))
({void*_tmp16D=0;Cyc_Tcutil_terr(loc,({const char*_tmp16E="the required region ordering is not satisfied here";_tag_dyneither(_tmp16E,sizeof(char),51);}),_tag_dyneither(_tmp16D,sizeof(void*),0));});}
return;}{
# 645
struct Cyc_Tcenv_Fenv*_tmp16F=_tmp16A;struct Cyc_Tcenv_Fenv*_tmp170=_tmp16F;struct Cyc_RgnOrder_RgnPO*_tmp174;struct Cyc_Tcenv_SharedFenv*_tmp173;_LL7A: _tmp174=_tmp170->region_order;_tmp173=_tmp170->shared;_LL7B:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp174,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp173->delayed_constraint_checks=({struct Cyc_List_List*_tmp171=_region_malloc(_tmp173->frgn,sizeof(*_tmp171));_tmp171->hd=({struct _tuple16*_tmp172=_region_malloc(_tmp173->frgn,sizeof(*_tmp172));_tmp172->f1=_tmp174;_tmp172->f2=po;_tmp172->f3=loc;_tmp172;});_tmp171->tl=_tmp173->delayed_constraint_checks;_tmp171;});};}
# 652
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp175=
Cyc_Tcenv_get_fenv(te,({const char*_tmp180="check_delayed_constraints";_tag_dyneither(_tmp180,sizeof(char),26);}));
# 653
struct Cyc_Tcenv_Fenv*_tmp176=_tmp175;struct Cyc_Tcenv_SharedFenv*_tmp17F;_LL7D: _tmp17F=_tmp176->shared;_LL7E:;{
# 655
struct Cyc_List_List*_tmp177=_tmp17F->delayed_constraint_checks;
for(0;_tmp177 != 0;_tmp177=_tmp177->tl){
struct _tuple16*_tmp178=(struct _tuple16*)_tmp177->hd;struct _tuple16*_tmp179=_tmp178;struct Cyc_RgnOrder_RgnPO*_tmp17E;struct Cyc_List_List*_tmp17D;unsigned int _tmp17C;_LL80: _tmp17E=_tmp179->f1;_tmp17D=_tmp179->f2;_tmp17C=_tmp179->f3;_LL81:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp17E,_tmp17D,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp17A=0;Cyc_Tcutil_terr(_tmp17C,({const char*_tmp17B="the required region ordering is not satisfied here";_tag_dyneither(_tmp17B,sizeof(char),51);}),_tag_dyneither(_tmp17A,sizeof(void*),0));});}};}
# 663
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->name=({struct _dyneither_ptr*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19B;_tmp19B.tag=0;_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp199[1]={& _tmp19B};Cyc_aprintf(({const char*_tmp19A="`%s";_tag_dyneither(_tmp19A,sizeof(char),4);}),_tag_dyneither(_tmp199,sizeof(void*),1));});});_tmp198;});_tmp197->identity=
# 666
Cyc_Tcutil_new_tvar_id();_tmp197->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp197;});
struct Cyc_List_List*_tmp181=({struct Cyc_List_List*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=rgn0;_tmp196->tl=fd->tvs;_tmp196;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp181);{
# 670
struct Cyc_RgnOrder_RgnPO*_tmp182=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 672
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp195;_tmp195.tag=2;_tmp195.f1=rgn0;_tmp195;});_tmp194;});
{struct Cyc_List_List*_tmp183=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
((struct Cyc_Absyn_Vardecl*)_tmp183->hd)->rgn=param_rgn;}}
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp184=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp185=_tmp184;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Tqual _tmp18A;void*_tmp189;int _tmp188;_LL83: _tmp18B=_tmp185.name;_tmp18A=_tmp185.tq;_tmp189=_tmp185.type;_tmp188=_tmp185.inject;_LL84:;{
# 678
struct Cyc_List_List*_tmp186=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
struct Cyc_Absyn_Vardecl*_tmp187=(struct Cyc_Absyn_Vardecl*)_tmp186->hd;
if(Cyc_strptrcmp((*_tmp187->name).f2,(struct _dyneither_ptr*)_check_null(_tmp18B))== 0){
_tmp187->type=Cyc_Absyn_dyneither_typ(_tmp189,param_rgn,_tmp18A,Cyc_Absyn_false_conref);
break;}}};}
# 686
return({struct Cyc_Tcenv_Fenv*_tmp18C=_region_malloc(r,sizeof(*_tmp18C));_tmp18C->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp193=_region_malloc(r,sizeof(*_tmp193));_tmp193->frgn=r;_tmp193->return_typ=fd->ret_type;_tmp193->delayed_effect_checks=0;_tmp193->delayed_constraint_checks=0;_tmp193;});_tmp18C->type_vars=(struct Cyc_List_List*)_tmp181;_tmp18C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp182;_tmp18C->ctrl_env=(const struct _tuple10*)0;_tmp18C->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp18E;_tmp18E.tag=24;_tmp18E.f1=({struct Cyc_List_List*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp192;_tmp192.tag=23;_tmp192.f1=param_rgn;_tmp192;});_tmp191;});_tmp18F->tl=({struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=(void*)_check_null(fd->effect);_tmp190->tl=0;_tmp190;});_tmp18F;});_tmp18E;});_tmp18D;}));_tmp18C->curr_rgn=(void*)param_rgn;_tmp18C->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp18C->flags).in_notreadctxt=0;(_tmp18C->flags).in_lhs=0;(_tmp18C->flags).in_new=0;(_tmp18C->flags).abstract_ok=0;_tmp18C->flags;});_tmp18C->fnrgn=(struct _RegionHandle*)r;_tmp18C;});};}
# 704
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp19C=old_fenv;struct Cyc_RgnOrder_RgnPO*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_Tcenv_SharedFenv*_tmp1C0;struct _RegionHandle*_tmp1BF;_LL86: _tmp1C2=_tmp19C->region_order;_tmp1C1=_tmp19C->type_vars;_tmp1C0=_tmp19C->shared;_tmp1BF=_tmp19C->fnrgn;_LL87:;{
# 707
struct _RegionHandle*_tmp19D=_tmp1C0->frgn;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->name=({struct _dyneither_ptr*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1BE;_tmp1BE.tag=0;_tmp1BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp1BC[1]={& _tmp1BE};Cyc_aprintf(({const char*_tmp1BD="`%s";_tag_dyneither(_tmp1BD,sizeof(char),4);}),_tag_dyneither(_tmp1BC,sizeof(void*),1));});});_tmp1BB;});_tmp1BA->identity=
# 710
Cyc_Tcutil_new_tvar_id();_tmp1BA->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp1BA;});
# 712
{struct Cyc_List_List*_tmp19E=fd->tvs;for(0;_tmp19E != 0;_tmp19E=_tmp19E->tl){
struct Cyc_Absyn_Kind*_tmp19F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp19E->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1A0=_tmp19F;enum Cyc_Absyn_KindQual _tmp1A4;enum Cyc_Absyn_AliasQual _tmp1A3;_LL89: _tmp1A4=_tmp1A0->kind;_tmp1A3=_tmp1A0->aliasqual;_LL8A:;
if(_tmp1A4 == Cyc_Absyn_RgnKind){
if(_tmp1A3 == Cyc_Absyn_Aliasable)
_tmp1C2=Cyc_RgnOrder_add_youngest(_tmp19D,_tmp1C2,(struct Cyc_Absyn_Tvar*)_tmp19E->hd,0,0);else{
# 718
({void*_tmp1A1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A2="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp1A2,sizeof(char),39);}),_tag_dyneither(_tmp1A1,sizeof(void*),0));});}}}}
# 720
_tmp1C2=Cyc_RgnOrder_add_youngest(_tmp19D,_tmp1C2,rgn0,0,0);{
struct Cyc_List_List*_tmp1A5=({struct Cyc_List_List*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->hd=rgn0;_tmp1B9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp1C1);_tmp1B9;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp1A5);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1B8;_tmp1B8.tag=2;_tmp1B8.f1=rgn0;_tmp1B8;});_tmp1B7;});
{struct Cyc_List_List*_tmp1A6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){
((struct Cyc_Absyn_Vardecl*)_tmp1A6->hd)->rgn=param_rgn;}}
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp1A7=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp1A8=_tmp1A7;struct _dyneither_ptr*_tmp1AE;struct Cyc_Absyn_Tqual _tmp1AD;void*_tmp1AC;int _tmp1AB;_LL8C: _tmp1AE=_tmp1A8.name;_tmp1AD=_tmp1A8.tq;_tmp1AC=_tmp1A8.type;_tmp1AB=_tmp1A8.inject;_LL8D:;{
# 729
struct Cyc_List_List*_tmp1A9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){
struct Cyc_Absyn_Vardecl*_tmp1AA=(struct Cyc_Absyn_Vardecl*)_tmp1A9->hd;
if(Cyc_strptrcmp((*_tmp1AA->name).f2,(struct _dyneither_ptr*)_check_null(_tmp1AE))== 0){
_tmp1AA->type=Cyc_Absyn_dyneither_typ(_tmp1AC,param_rgn,_tmp1AD,Cyc_Absyn_false_conref);
break;}}};}
# 737
return({struct Cyc_Tcenv_Fenv*_tmp1AF=_region_malloc(_tmp19D,sizeof(*_tmp1AF));_tmp1AF->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp1B6=_region_malloc(_tmp19D,sizeof(*_tmp1B6));_tmp1B6->frgn=_tmp19D;_tmp1B6->return_typ=fd->ret_type;_tmp1B6->delayed_effect_checks=0;_tmp1B6->delayed_constraint_checks=0;_tmp1B6;});_tmp1AF->type_vars=(struct Cyc_List_List*)_tmp1A5;_tmp1AF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C2;_tmp1AF->ctrl_env=(const struct _tuple10*)0;_tmp1AF->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1B1;_tmp1B1.tag=24;_tmp1B1.f1=({struct Cyc_List_List*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1B5;_tmp1B5.tag=23;_tmp1B5.f1=param_rgn;_tmp1B5;});_tmp1B4;});_tmp1B2->tl=({struct Cyc_List_List*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->hd=(void*)_check_null(fd->effect);_tmp1B3->tl=0;_tmp1B3;});_tmp1B2;});_tmp1B1;});_tmp1B0;}));_tmp1AF->curr_rgn=(void*)param_rgn;_tmp1AF->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp1AF->flags).in_notreadctxt=0;(_tmp1AF->flags).in_lhs=0;(_tmp1AF->flags).in_new=0;(_tmp1AF->flags).abstract_ok=0;_tmp1AF->flags;});_tmp1AF->fnrgn=(struct _RegionHandle*)_tmp1BF;_tmp1AF;});};};};}
# 755
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 757
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp1D1->identity=Cyc_Tcutil_new_tvar_id();_tmp1D1->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp1D1;});
struct Cyc_List_List*_tmp1C3=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=rgn0;_tmp1D0->tl=0;_tmp1D0;});
struct Cyc_RgnOrder_RgnPO*_tmp1C4=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1CF;_tmp1CF.tag=24;_tmp1CF.f1=0;_tmp1CF;});_tmp1CE;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1CD;_tmp1CD.tag=2;_tmp1CD.f1=rgn0;_tmp1CD;});_tmp1CC;});
return({struct Cyc_Tcenv_Fenv*_tmp1C5=_region_malloc(r,sizeof(*_tmp1C5));_tmp1C5->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp1CB=_region_malloc(r,sizeof(*_tmp1CB));_tmp1CB->frgn=r;_tmp1CB->return_typ=ret_type;_tmp1CB->delayed_effect_checks=0;_tmp1CB->delayed_constraint_checks=0;_tmp1CB;});_tmp1C5->type_vars=(struct Cyc_List_List*)_tmp1C3;_tmp1C5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C4;_tmp1C5->ctrl_env=(const struct _tuple10*)0;_tmp1C5->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1C7;_tmp1C7.tag=24;_tmp1C7.f1=({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1CA;_tmp1CA.tag=23;_tmp1CA.f1=param_rgn;_tmp1CA;});_tmp1C9;});_tmp1C8->tl=0;_tmp1C8;});_tmp1C7;});_tmp1C6;}));_tmp1C5->curr_rgn=(void*)param_rgn;_tmp1C5->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp1C5->flags).in_notreadctxt=0;(_tmp1C5->flags).in_lhs=0;(_tmp1C5->flags).in_new=0;(_tmp1C5->flags).abstract_ok=0;_tmp1C5->flags;});_tmp1C5->fnrgn=(struct _RegionHandle*)r;_tmp1C5;});}
