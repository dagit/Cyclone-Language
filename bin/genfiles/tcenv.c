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
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_SlowDict_Dict;
# 40 "slowdict.h"
typedef struct Cyc_SlowDict_Dict*Cyc_SlowDict_dict_t;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Relations_Reln;
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 429 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
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
# 920 "absyn.h"
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 943
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 963
extern void*Cyc_Absyn_empty_effect;
# 965
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 998
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
# 87
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 89
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 101 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 106
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 110
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 117
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 120
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 123
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 125
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 128
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 131
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 133
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 141
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 146
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 154
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 159
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 164
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 167
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 175
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 177
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 179
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 181
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 183
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 187
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 190
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 193
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 196
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
# 239 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 306
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 308
int Cyc_Tcutil_new_tvar_id();
# 310
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 65 "tcenv.cyc"
int Cyc_Tcenv_warn_override=0;char Cyc_Tcenv_Env_error[10]="Env_error";
# 68
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 70
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 73
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 115
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{const struct _tuple10*fallthru_clause;};
# 121
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 144 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 178 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 181
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)
return bs->b;}
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 190
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
return({struct Cyc_Tcenv_Genv*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->grgn=r;_tmp2->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2;});}struct _tuple12{void*f1;int f2;};
# 199
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 201
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp3;});{
# 203
struct Cyc_Tcenv_Genv*_tmp4=Cyc_Tcenv_empty_genv(r);
_tmp4->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Datatypedecl**_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=Cyc_Absyn_exn_tud;_tmp5;}));
{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
_tmp4->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp4->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd)->name).f2,({struct _tuple12*_tmp7=_region_malloc(r,sizeof(*_tmp7));_tmp7->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp9;_tmp9.tag=2;_tmp9.f1=Cyc_Absyn_exn_tud;_tmp9.f2=(struct Cyc_Absyn_Datatypefield*)_tmp6->hd;_tmp9;});_tmp8;});_tmp7->f2=1;_tmp7;}));}}{
# 210
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp4);
return({struct Cyc_Tcenv_Tenv*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->ns=0;_tmpA->ae=ae;_tmpA->le=0;_tmpA->allow_valueof=0;_tmpA->in_extern_c_include=0;_tmpA;});};};}
# 216
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple0*_tmpB=q;union Cyc_Absyn_Nmspace _tmp14;struct _dyneither_ptr*_tmp13;_LL1: _tmp14=_tmpB->f1;_tmp13=_tmpB->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpC=_tmp14;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp11;switch((_tmpC.Abs_n).tag){case 3: _LL4: _tmp11=(_tmpC.C_n).val;_LL5:
 _tmp12=_tmp11;goto _LL7;case 2: _LL6: _tmp12=(_tmpC.Abs_n).val;_LL7: {
# 221
struct _tuple12*_tmpD=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)((((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp12))->ordinaries,_tmp13);
if(is_use)
(*_tmpD).f2=1;
return(*_tmpD).f1;}default: _LL8: _LL9:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=({struct Cyc_Core_Impossible_exn_struct _tmpF;_tmpF.tag=Cyc_Core_Impossible;_tmpF.f1=({const char*_tmp10="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp10,sizeof(char),37);});_tmpF;});_tmpE;}));}_LL3:;};}
# 228
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp15=q;union Cyc_Absyn_Nmspace _tmp1D;struct _dyneither_ptr*_tmp1C;_LLB: _tmp1D=_tmp15->f1;_tmp1C=_tmp15->f2;_LLC:;{
union Cyc_Absyn_Nmspace _tmp16=_tmp1D;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;switch((_tmp16.Abs_n).tag){case 3: _LLE: _tmp1A=(_tmp16.C_n).val;_LLF:
 _tmp1B=_tmp1A;goto _LL11;case 2: _LL10: _tmp1B=(_tmp16.Abs_n).val;_LL11:
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)((((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp1B))->aggrdecls,_tmp1C);default: _LL12: _LL13:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Core_Impossible_exn_struct _tmp18;_tmp18.tag=Cyc_Core_Impossible;_tmp18.f1=({const char*_tmp19="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp19,sizeof(char),37);});_tmp18;});_tmp17;}));}_LLD:;};}
# 236
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp1E=q;union Cyc_Absyn_Nmspace _tmp26;struct _dyneither_ptr*_tmp25;_LL15: _tmp26=_tmp1E->f1;_tmp25=_tmp1E->f2;_LL16:;{
union Cyc_Absyn_Nmspace _tmp1F=_tmp26;struct Cyc_List_List*_tmp24;struct Cyc_List_List*_tmp23;switch((_tmp1F.Abs_n).tag){case 3: _LL18: _tmp23=(_tmp1F.C_n).val;_LL19:
 _tmp24=_tmp23;goto _LL1B;case 2: _LL1A: _tmp24=(_tmp1F.Abs_n).val;_LL1B:
# 241
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)((((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp24))->datatypedecls,_tmp25);default: _LL1C: _LL1D:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Core_Impossible_exn_struct _tmp21;_tmp21.tag=Cyc_Core_Impossible;_tmp21.f1=({const char*_tmp22="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp22,sizeof(char),37);});_tmp21;});_tmp20;}));}_LL17:;};}
# 245
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp27=q;union Cyc_Absyn_Nmspace _tmp30;struct _dyneither_ptr*_tmp2F;_LL1F: _tmp30=_tmp27->f1;_tmp2F=_tmp27->f2;_LL20:;{
union Cyc_Absyn_Nmspace _tmp28=_tmp30;struct Cyc_List_List*_tmp2E;struct Cyc_List_List*_tmp2D;switch((_tmp28.Abs_n).tag){case 3: _LL22: _tmp2D=(_tmp28.C_n).val;_LL23:
 _tmp2E=_tmp2D;goto _LL25;case 2: _LL24: _tmp2E=(_tmp28.Abs_n).val;_LL25:
# 250
 return({struct Cyc_Absyn_Datatypedecl***_tmp29=_region_malloc(r,sizeof(*_tmp29));_tmp29[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)((((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2E))->datatypedecls,_tmp2F);_tmp29;});default: _LL26: _LL27:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2B;_tmp2B.tag=Cyc_Core_Impossible;_tmp2B.f1=({const char*_tmp2C="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp2C,sizeof(char),37);});_tmp2B;});_tmp2A;}));}_LL21:;};}
# 254
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp31=q;union Cyc_Absyn_Nmspace _tmp39;struct _dyneither_ptr*_tmp38;_LL29: _tmp39=_tmp31->f1;_tmp38=_tmp31->f2;_LL2A:;{
union Cyc_Absyn_Nmspace _tmp32=_tmp39;struct Cyc_List_List*_tmp37;struct Cyc_List_List*_tmp36;switch((_tmp32.Abs_n).tag){case 3: _LL2C: _tmp36=(_tmp32.C_n).val;_LL2D:
 _tmp37=_tmp36;goto _LL2F;case 2: _LL2E: _tmp37=(_tmp32.Abs_n).val;_LL2F:
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)((((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp37))->enumdecls,_tmp38);default: _LL30: _LL31:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Core_Impossible_exn_struct _tmp34;_tmp34.tag=Cyc_Core_Impossible;_tmp34.f1=({const char*_tmp35="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp35,sizeof(char),37);});_tmp34;});_tmp33;}));}_LL2B:;};}
# 262
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp3A=q;union Cyc_Absyn_Nmspace _tmp42;struct _dyneither_ptr*_tmp41;_LL33: _tmp42=_tmp3A->f1;_tmp41=_tmp3A->f2;_LL34:;{
union Cyc_Absyn_Nmspace _tmp3B=_tmp42;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp3F;switch((_tmp3B.Abs_n).tag){case 3: _LL36: _tmp3F=(_tmp3B.C_n).val;_LL37:
 _tmp40=_tmp3F;goto _LL39;case 2: _LL38: _tmp40=(_tmp3B.Abs_n).val;_LL39:
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)((((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp40))->typedefs,_tmp41);default: _LL3A: _LL3B:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3D;_tmp3D.tag=Cyc_Core_Impossible;_tmp3D.f1=({const char*_tmp3E="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp3E,sizeof(char),37);});_tmp3D;});_tmp3C;}));}_LL35:;};}
# 273
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp43=te->le;
if(_tmp43 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp43;}
# 279
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp44=te->le;
if(_tmp44 != 0){
struct Cyc_Tcenv_Fenv*_tmp45=_tmp44;struct Cyc_Tcenv_Fenv*_tmp46=_tmp45;struct _RegionHandle*_tmp47;_LL3D: _tmp47=_tmp46->fnrgn;_LL3E:;
return _tmp47;}
# 285
return Cyc_Core_heap_region;}
# 288
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp48="put_fenv";_tag_dyneither(_tmp48,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp49=_region_malloc(l,sizeof(*_tmp49));_tmp49->ns=te->ns;_tmp49->ae=te->ae;_tmp49->le=fe;_tmp49->allow_valueof=te->allow_valueof;_tmp49->in_extern_c_include=te->in_extern_c_include;_tmp49;});}
# 293
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp4A=_region_malloc(l,sizeof(*_tmp4A));_tmp4A->ns=te->ns;_tmp4A->ae=te->ae;_tmp4A->le=0;_tmp4A->allow_valueof=te->allow_valueof;_tmp4A->in_extern_c_include=te->in_extern_c_include;_tmp4A;});}
# 297
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 299
struct Cyc_Tcenv_Fenv _tmp4B=*f;struct Cyc_Tcenv_Fenv _tmp4C=_tmp4B;struct Cyc_Tcenv_SharedFenv*_tmp56;struct Cyc_List_List*_tmp55;struct Cyc_RgnOrder_RgnPO*_tmp54;const struct Cyc_Tcenv_Bindings*_tmp53;struct Cyc_Tcenv_CtrlEnv*_tmp52;void*_tmp51;void*_tmp50;struct Cyc_Tcenv_FenvFlags _tmp4F;struct _RegionHandle*_tmp4E;_LL40: _tmp56=_tmp4C.shared;_tmp55=_tmp4C.type_vars;_tmp54=_tmp4C.region_order;_tmp53=_tmp4C.locals;_tmp52=_tmp4C.ctrl_env;_tmp51=_tmp4C.capability;_tmp50=_tmp4C.curr_rgn;_tmp4F=_tmp4C.flags;_tmp4E=_tmp4C.fnrgn;_LL41:;
# 301
return({struct Cyc_Tcenv_Fenv*_tmp4D=_region_malloc(l,sizeof(*_tmp4D));_tmp4D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp56;_tmp4D->type_vars=(struct Cyc_List_List*)_tmp55;_tmp4D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp54;_tmp4D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp53;_tmp4D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp52;_tmp4D->capability=(void*)_tmp51;_tmp4D->curr_rgn=(void*)_tmp50;_tmp4D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp4F;_tmp4D->fnrgn=(struct _RegionHandle*)l;_tmp4D;});}
# 305
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 307
struct Cyc_Tcenv_Fenv _tmp57=*f;struct Cyc_Tcenv_Fenv _tmp58=_tmp57;struct Cyc_Tcenv_SharedFenv*_tmp66;struct Cyc_List_List*_tmp65;struct Cyc_RgnOrder_RgnPO*_tmp64;const struct Cyc_Tcenv_Bindings*_tmp63;struct Cyc_Tcenv_CtrlEnv*_tmp62;void*_tmp61;void*_tmp60;struct Cyc_Tcenv_FenvFlags _tmp5F;_LL43: _tmp66=_tmp58.shared;_tmp65=_tmp58.type_vars;_tmp64=_tmp58.region_order;_tmp63=_tmp58.locals;_tmp62=_tmp58.ctrl_env;_tmp61=_tmp58.capability;_tmp60=_tmp58.curr_rgn;_tmp5F=_tmp58.flags;_LL44:;{
# 310
struct Cyc_Tcenv_CtrlEnv _tmp59=*_tmp62;struct Cyc_Tcenv_CtrlEnv _tmp5A=_tmp59;const struct _tuple10*_tmp5E;_LL46: _tmp5E=_tmp5A.fallthru_clause;_LL47:;{
struct Cyc_Tcenv_CtrlEnv*_tmp5B=({struct Cyc_Tcenv_CtrlEnv*_tmp5D=_region_malloc(l,sizeof(*_tmp5D));_tmp5D->fallthru_clause=_tmp5E;_tmp5D;});
# 313
return({struct Cyc_Tcenv_Fenv*_tmp5C=_region_malloc(l,sizeof(*_tmp5C));_tmp5C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp66;_tmp5C->type_vars=(struct Cyc_List_List*)_tmp65;_tmp5C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp64;_tmp5C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp63;_tmp5C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp5B;_tmp5C->capability=(void*)_tmp61;_tmp5C->curr_rgn=(void*)_tmp60;_tmp5C->flags=(struct Cyc_Tcenv_FenvFlags)_tmp5F;_tmp5C->fnrgn=(struct _RegionHandle*)l;_tmp5C;});};};}
# 319
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp67=Cyc_Tcenv_get_fenv(te,({const char*_tmp6D="return_typ";_tag_dyneither(_tmp6D,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmp68=_tmp67;struct Cyc_Tcenv_SharedFenv*_tmp6C;_LL49: _tmp6C=_tmp68->shared;_LL4A:;{
struct Cyc_Tcenv_SharedFenv _tmp69=*_tmp6C;struct Cyc_Tcenv_SharedFenv _tmp6A=_tmp69;void*_tmp6B;_LL4C: _tmp6B=_tmp6A.return_typ;_LL4D:;
return _tmp6B;};}
# 325
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp6E=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmp6F=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmp6E));struct Cyc_Tcenv_Fenv _tmp70=_tmp6F;struct Cyc_List_List*_tmp71;_LL4F: _tmp71=_tmp70.type_vars;_LL50:;
return _tmp71;};}
# 332
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->v=Cyc_Tcenv_lookup_type_vars(te);_tmp72;});}
# 336
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 338
struct Cyc_Tcenv_Fenv*_tmp73=
# 340
Cyc_Tcenv_get_fenv(te,({const char*_tmp80="add_type_vars";_tag_dyneither(_tmp80,sizeof(char),14);}));
# 338
struct Cyc_Tcenv_Fenv*_tmp74=_tmp73;struct Cyc_Tcenv_SharedFenv*_tmp7F;struct Cyc_List_List*_tmp7E;struct Cyc_RgnOrder_RgnPO*_tmp7D;const struct Cyc_Tcenv_Bindings*_tmp7C;struct Cyc_Tcenv_CtrlEnv*_tmp7B;void*_tmp7A;void*_tmp79;struct Cyc_Tcenv_FenvFlags _tmp78;_LL52: _tmp7F=_tmp74->shared;_tmp7E=_tmp74->type_vars;_tmp7D=_tmp74->region_order;_tmp7C=_tmp74->locals;_tmp7B=_tmp74->ctrl_env;_tmp7A=_tmp74->capability;_tmp79=_tmp74->curr_rgn;_tmp78=_tmp74->flags;_LL53:;
# 342
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp75=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp7E);
Cyc_Tcutil_check_unique_tvars(loc,_tmp75);{
struct Cyc_Tcenv_Fenv*_tmp76=({struct Cyc_Tcenv_Fenv*_tmp77=_region_malloc(r,sizeof(*_tmp77));_tmp77->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp7F;_tmp77->type_vars=(struct Cyc_List_List*)_tmp75;_tmp77->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp7D;_tmp77->locals=(const struct Cyc_Tcenv_Bindings*)_tmp7C;_tmp77->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp7B;_tmp77->capability=(void*)_tmp7A;_tmp77->curr_rgn=(void*)_tmp79;_tmp77->flags=(struct Cyc_Tcenv_FenvFlags)_tmp78;_tmp77->fnrgn=(struct _RegionHandle*)r;_tmp77;});
# 347
return Cyc_Tcenv_put_fenv(r,te,_tmp76);};};}
# 350
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp81=te->le;
if(_tmp81 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 355
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmp81);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 360
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 362
struct _dyneither_ptr*_tmp82=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmp83=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmp84=_tmp83;if((_tmp84.Loc_n).tag == 4){_LL55: _LL56:
 goto _LL54;}else{_LL57: _LL58:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Core_Impossible_exn_struct _tmp86;_tmp86.tag=Cyc_Core_Impossible;_tmp86.f1=({const char*_tmp87="add_local_var: called with non-local";_tag_dyneither(_tmp87,sizeof(char),37);});_tmp86;});_tmp85;}));}_LL54:;}
# 367
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp88;_push_handler(& _tmp88);{int _tmp8A=0;if(setjmp(_tmp88.handler))_tmp8A=1;if(!_tmp8A){{void*_tmp8B=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp8C=_tmp8B;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp8C)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp8C)->f1)){case 4: _LL5A: _LL5B:
({void*_tmp8D=0;Cyc_Tcutil_warn(loc,({const char*_tmp8E="declaration hides local";_tag_dyneither(_tmp8E,sizeof(char),24);}),_tag_dyneither(_tmp8D,sizeof(void*),0));});goto _LL59;case 1: _LL5C: _LL5D:
({void*_tmp8F=0;Cyc_Tcutil_warn(loc,({const char*_tmp90="declaration hides global";_tag_dyneither(_tmp90,sizeof(char),25);}),_tag_dyneither(_tmp8F,sizeof(void*),0));});goto _LL59;case 5: _LL5E: _LL5F:
({void*_tmp91=0;Cyc_Tcutil_warn(loc,({const char*_tmp92="declaration hides pattern variable";_tag_dyneither(_tmp92,sizeof(char),35);}),_tag_dyneither(_tmp91,sizeof(void*),0));});goto _LL59;case 3: _LL60: _LL61:
({void*_tmp93=0;Cyc_Tcutil_warn(loc,({const char*_tmp94="declaration hides parameter";_tag_dyneither(_tmp94,sizeof(char),28);}),_tag_dyneither(_tmp93,sizeof(void*),0));});goto _LL59;default: goto _LL62;}else{_LL62: _LL63:
 goto _LL59;}_LL59:;}
# 368
;_pop_handler();}else{void*_tmp89=(void*)_exn_thrown;void*_tmp95=_tmp89;void*_tmp96;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp95)->tag == Cyc_Dict_Absent){_LL65: _LL66:
# 375
 goto _LL64;}else{_LL67: _tmp96=_tmp95;_LL68:(int)_rethrow(_tmp96);}_LL64:;}};}{
struct Cyc_Tcenv_Fenv*_tmp97=
# 378
Cyc_Tcenv_get_fenv(te,({const char*_tmpA8="add_local_var";_tag_dyneither(_tmpA8,sizeof(char),14);}));
# 376
struct Cyc_Tcenv_Fenv*_tmp98=_tmp97;struct Cyc_Tcenv_SharedFenv*_tmpA7;struct Cyc_List_List*_tmpA6;struct Cyc_RgnOrder_RgnPO*_tmpA5;const struct Cyc_Tcenv_Bindings*_tmpA4;struct Cyc_Tcenv_CtrlEnv*_tmpA3;void*_tmpA2;void*_tmpA1;struct Cyc_Tcenv_FenvFlags _tmpA0;_LL6A: _tmpA7=_tmp98->shared;_tmpA6=_tmp98->type_vars;_tmpA5=_tmp98->region_order;_tmpA4=_tmp98->locals;_tmpA3=_tmp98->ctrl_env;_tmpA2=_tmp98->capability;_tmpA1=_tmp98->curr_rgn;_tmpA0=_tmp98->flags;_LL6B:;{
# 379
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp99=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp9F;_tmp9F.tag=4;_tmp9F.f1=vd;_tmp9F;});_tmp9E;});
struct Cyc_Tcenv_Bindings*_tmp9A=({struct Cyc_Tcenv_Bindings*_tmp9D=_region_malloc(r,sizeof(*_tmp9D));_tmp9D->v=_tmp82;_tmp9D->b=(void*)_tmp99;_tmp9D->tl=_tmpA4;_tmp9D;});
struct Cyc_Tcenv_Fenv*_tmp9B=({struct Cyc_Tcenv_Fenv*_tmp9C=_region_malloc(r,sizeof(*_tmp9C));_tmp9C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA7;_tmp9C->type_vars=(struct Cyc_List_List*)_tmpA6;_tmp9C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA5;_tmp9C->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp9A);_tmp9C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA3;_tmp9C->capability=(void*)_tmpA2;_tmp9C->curr_rgn=(void*)_tmpA1;_tmp9C->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA0;_tmp9C->fnrgn=(struct _RegionHandle*)r;_tmp9C;});
# 383
return Cyc_Tcenv_put_fenv(r,te,_tmp9B);};};}
# 386
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 388
struct Cyc_Tcenv_Fenv*_tmpA9=te->le;
if(_tmpA9 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpAA=*_tmpA9;struct Cyc_Tcenv_Fenv _tmpAB=_tmpAA;struct Cyc_Tcenv_SharedFenv*_tmpB7;struct Cyc_List_List*_tmpB6;struct Cyc_RgnOrder_RgnPO*_tmpB5;const struct Cyc_Tcenv_Bindings*_tmpB4;struct Cyc_Tcenv_CtrlEnv*_tmpB3;void*_tmpB2;void*_tmpB1;int _tmpB0;int _tmpAF;int _tmpAE;_LL6D: _tmpB7=_tmpAB.shared;_tmpB6=_tmpAB.type_vars;_tmpB5=_tmpAB.region_order;_tmpB4=_tmpAB.locals;_tmpB3=_tmpAB.ctrl_env;_tmpB2=_tmpAB.capability;_tmpB1=_tmpAB.curr_rgn;_tmpB0=(_tmpAB.flags).in_notreadctxt;_tmpAF=(_tmpAB.flags).in_lhs;_tmpAE=(_tmpAB.flags).abstract_ok;_LL6E:;{
# 393
struct Cyc_Tcenv_Fenv*_tmpAC=({struct Cyc_Tcenv_Fenv*_tmpAD=_region_malloc(r,sizeof(*_tmpAD));_tmpAD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB7;_tmpAD->type_vars=(struct Cyc_List_List*)_tmpB6;_tmpAD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB5;_tmpAD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB4;_tmpAD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB3;_tmpAD->capability=(void*)_tmpB2;_tmpAD->curr_rgn=(void*)_tmpB1;_tmpAD->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpAD->flags).in_notreadctxt=_tmpB0;(_tmpAD->flags).in_lhs=_tmpAF;(_tmpAD->flags).in_new=(int)status;(_tmpAD->flags).abstract_ok=_tmpAE;_tmpAD->flags;});_tmpAD->fnrgn=(struct _RegionHandle*)r;_tmpAD;});
# 396
return Cyc_Tcenv_put_fenv(r,te,_tmpAC);};};}
# 399
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpB8=te->le;
if(_tmpB8 == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmpB9=_tmpB8;struct Cyc_Tcenv_Fenv*_tmpBA=_tmpB9;int _tmpBB;_LL70: _tmpBB=(_tmpBA->flags).in_new;_LL71:;
return(enum Cyc_Tcenv_NewStatus)_tmpBB;};}
# 406
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpBC=te->le;
if(_tmpBC == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpBD=_tmpBC;struct Cyc_Tcenv_Fenv*_tmpBE=_tmpBD;int _tmpBF;_LL73: _tmpBF=(_tmpBE->flags).abstract_ok;_LL74:;
return _tmpBF;};}
# 413
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 415
struct Cyc_Tcenv_Fenv*_tmpC0=te->le;
if(_tmpC0 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpC1=*_tmpC0;struct Cyc_Tcenv_Fenv _tmpC2=_tmpC1;struct Cyc_Tcenv_SharedFenv*_tmpCE;struct Cyc_List_List*_tmpCD;struct Cyc_RgnOrder_RgnPO*_tmpCC;const struct Cyc_Tcenv_Bindings*_tmpCB;struct Cyc_Tcenv_CtrlEnv*_tmpCA;void*_tmpC9;void*_tmpC8;int _tmpC7;int _tmpC6;int _tmpC5;_LL76: _tmpCE=_tmpC2.shared;_tmpCD=_tmpC2.type_vars;_tmpCC=_tmpC2.region_order;_tmpCB=_tmpC2.locals;_tmpCA=_tmpC2.ctrl_env;_tmpC9=_tmpC2.capability;_tmpC8=_tmpC2.curr_rgn;_tmpC7=(_tmpC2.flags).in_notreadctxt;_tmpC6=(_tmpC2.flags).in_lhs;_tmpC5=(_tmpC2.flags).in_new;_LL77:;{
# 420
struct Cyc_Tcenv_Fenv*_tmpC3=({struct Cyc_Tcenv_Fenv*_tmpC4=_region_malloc(r,sizeof(*_tmpC4));_tmpC4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpCE;_tmpC4->type_vars=(struct Cyc_List_List*)_tmpCD;_tmpC4->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpCC;_tmpC4->locals=(const struct Cyc_Tcenv_Bindings*)_tmpCB;_tmpC4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpCA;_tmpC4->capability=(void*)_tmpC9;_tmpC4->curr_rgn=(void*)_tmpC8;_tmpC4->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpC4->flags).in_notreadctxt=_tmpC7;(_tmpC4->flags).in_lhs=_tmpC6;(_tmpC4->flags).in_new=_tmpC5;(_tmpC4->flags).abstract_ok=1;_tmpC4->flags;});_tmpC4->fnrgn=(struct _RegionHandle*)r;_tmpC4;});
# 423
return Cyc_Tcenv_put_fenv(r,te,_tmpC3);};};}
# 426
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 428
struct Cyc_Tcenv_Fenv*_tmpCF=te->le;
if(_tmpCF == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpD0=*_tmpCF;struct Cyc_Tcenv_Fenv _tmpD1=_tmpD0;struct Cyc_Tcenv_SharedFenv*_tmpDD;struct Cyc_List_List*_tmpDC;struct Cyc_RgnOrder_RgnPO*_tmpDB;const struct Cyc_Tcenv_Bindings*_tmpDA;struct Cyc_Tcenv_CtrlEnv*_tmpD9;void*_tmpD8;void*_tmpD7;int _tmpD6;int _tmpD5;int _tmpD4;_LL79: _tmpDD=_tmpD1.shared;_tmpDC=_tmpD1.type_vars;_tmpDB=_tmpD1.region_order;_tmpDA=_tmpD1.locals;_tmpD9=_tmpD1.ctrl_env;_tmpD8=_tmpD1.capability;_tmpD7=_tmpD1.curr_rgn;_tmpD6=(_tmpD1.flags).in_notreadctxt;_tmpD5=(_tmpD1.flags).in_lhs;_tmpD4=(_tmpD1.flags).in_new;_LL7A:;{
# 433
struct Cyc_Tcenv_Fenv*_tmpD2=({struct Cyc_Tcenv_Fenv*_tmpD3=_region_malloc(r,sizeof(*_tmpD3));_tmpD3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpDD;_tmpD3->type_vars=(struct Cyc_List_List*)_tmpDC;_tmpD3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpDB;_tmpD3->locals=(const struct Cyc_Tcenv_Bindings*)_tmpDA;_tmpD3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD9;_tmpD3->capability=(void*)_tmpD8;_tmpD3->curr_rgn=(void*)_tmpD7;_tmpD3->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpD3->flags).in_notreadctxt=_tmpD6;(_tmpD3->flags).in_lhs=_tmpD5;(_tmpD3->flags).in_new=_tmpD4;(_tmpD3->flags).abstract_ok=0;_tmpD3->flags;});_tmpD3->fnrgn=(struct _RegionHandle*)r;_tmpD3;});
# 436
return Cyc_Tcenv_put_fenv(r,te,_tmpD2);};};}
# 439
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpDE=te->le;
if(_tmpDE == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpDF=*_tmpDE;struct Cyc_Tcenv_Fenv _tmpE0=_tmpDF;struct Cyc_Tcenv_SharedFenv*_tmpEC;struct Cyc_List_List*_tmpEB;struct Cyc_RgnOrder_RgnPO*_tmpEA;const struct Cyc_Tcenv_Bindings*_tmpE9;struct Cyc_Tcenv_CtrlEnv*_tmpE8;void*_tmpE7;void*_tmpE6;int _tmpE5;int _tmpE4;int _tmpE3;_LL7C: _tmpEC=_tmpE0.shared;_tmpEB=_tmpE0.type_vars;_tmpEA=_tmpE0.region_order;_tmpE9=_tmpE0.locals;_tmpE8=_tmpE0.ctrl_env;_tmpE7=_tmpE0.capability;_tmpE6=_tmpE0.curr_rgn;_tmpE5=(_tmpE0.flags).in_lhs;_tmpE4=(_tmpE0.flags).in_new;_tmpE3=(_tmpE0.flags).abstract_ok;_LL7D:;{
# 445
struct Cyc_Tcenv_Fenv*_tmpE1=({struct Cyc_Tcenv_Fenv*_tmpE2=_region_malloc(r,sizeof(*_tmpE2));_tmpE2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpEC;_tmpE2->type_vars=(struct Cyc_List_List*)_tmpEB;_tmpE2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEA;_tmpE2->locals=(const struct Cyc_Tcenv_Bindings*)_tmpE9;_tmpE2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE8;_tmpE2->capability=(void*)_tmpE7;_tmpE2->curr_rgn=(void*)_tmpE6;_tmpE2->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpE2->flags).in_notreadctxt=1;(_tmpE2->flags).in_lhs=_tmpE5;(_tmpE2->flags).in_new=_tmpE4;(_tmpE2->flags).abstract_ok=_tmpE3;_tmpE2->flags;});_tmpE2->fnrgn=(struct _RegionHandle*)r;_tmpE2;});
# 448
return Cyc_Tcenv_put_fenv(r,te,_tmpE1);};};}
# 451
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpED=te->le;
if(_tmpED == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpEE=*_tmpED;struct Cyc_Tcenv_Fenv _tmpEF=_tmpEE;struct Cyc_Tcenv_SharedFenv*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_RgnOrder_RgnPO*_tmpF9;const struct Cyc_Tcenv_Bindings*_tmpF8;struct Cyc_Tcenv_CtrlEnv*_tmpF7;void*_tmpF6;void*_tmpF5;int _tmpF4;int _tmpF3;int _tmpF2;_LL7F: _tmpFB=_tmpEF.shared;_tmpFA=_tmpEF.type_vars;_tmpF9=_tmpEF.region_order;_tmpF8=_tmpEF.locals;_tmpF7=_tmpEF.ctrl_env;_tmpF6=_tmpEF.capability;_tmpF5=_tmpEF.curr_rgn;_tmpF4=(_tmpEF.flags).in_lhs;_tmpF3=(_tmpEF.flags).in_new;_tmpF2=(_tmpEF.flags).abstract_ok;_LL80:;{
# 457
struct Cyc_Tcenv_Fenv*_tmpF0=({struct Cyc_Tcenv_Fenv*_tmpF1=_region_malloc(r,sizeof(*_tmpF1));_tmpF1->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFB;_tmpF1->type_vars=(struct Cyc_List_List*)_tmpFA;_tmpF1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF9;_tmpF1->locals=(const struct Cyc_Tcenv_Bindings*)_tmpF8;_tmpF1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF7;_tmpF1->capability=(void*)_tmpF6;_tmpF1->curr_rgn=(void*)_tmpF5;_tmpF1->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpF1->flags).in_notreadctxt=0;(_tmpF1->flags).in_lhs=_tmpF4;(_tmpF1->flags).in_new=_tmpF3;(_tmpF1->flags).abstract_ok=_tmpF2;_tmpF1->flags;});_tmpF1->fnrgn=(struct _RegionHandle*)r;_tmpF1;});
# 460
return Cyc_Tcenv_put_fenv(r,te,_tmpF0);};};}
# 463
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpFC=te->le;
if(_tmpFC == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpFD=_tmpFC;struct Cyc_Tcenv_Fenv*_tmpFE=_tmpFD;int _tmpFF;_LL82: _tmpFF=(_tmpFE->flags).in_notreadctxt;_LL83:;
return _tmpFF;};}
# 470
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp100=te->le;
if(_tmp100 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp101=*_tmp100;struct Cyc_Tcenv_Fenv _tmp102=_tmp101;struct Cyc_Tcenv_SharedFenv*_tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_RgnOrder_RgnPO*_tmp10C;const struct Cyc_Tcenv_Bindings*_tmp10B;struct Cyc_Tcenv_CtrlEnv*_tmp10A;void*_tmp109;void*_tmp108;int _tmp107;int _tmp106;int _tmp105;_LL85: _tmp10E=_tmp102.shared;_tmp10D=_tmp102.type_vars;_tmp10C=_tmp102.region_order;_tmp10B=_tmp102.locals;_tmp10A=_tmp102.ctrl_env;_tmp109=_tmp102.capability;_tmp108=_tmp102.curr_rgn;_tmp107=(_tmp102.flags).in_notreadctxt;_tmp106=(_tmp102.flags).in_new;_tmp105=(_tmp102.flags).abstract_ok;_LL86:;{
# 476
struct Cyc_Tcenv_Fenv*_tmp103=({struct Cyc_Tcenv_Fenv*_tmp104=_region_malloc(r,sizeof(*_tmp104));_tmp104->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp10E;_tmp104->type_vars=(struct Cyc_List_List*)_tmp10D;_tmp104->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp10C;_tmp104->locals=(const struct Cyc_Tcenv_Bindings*)_tmp10B;_tmp104->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp10A;_tmp104->capability=(void*)_tmp109;_tmp104->curr_rgn=(void*)_tmp108;_tmp104->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp104->flags).in_notreadctxt=_tmp107;(_tmp104->flags).in_lhs=1;(_tmp104->flags).in_new=_tmp106;(_tmp104->flags).abstract_ok=_tmp105;_tmp104->flags;});_tmp104->fnrgn=(struct _RegionHandle*)r;_tmp104;});
# 479
return Cyc_Tcenv_put_fenv(r,te,_tmp103);};};}
# 482
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp110=*_tmp10F;struct Cyc_Tcenv_Fenv _tmp111=_tmp110;struct Cyc_Tcenv_SharedFenv*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_RgnOrder_RgnPO*_tmp11B;const struct Cyc_Tcenv_Bindings*_tmp11A;struct Cyc_Tcenv_CtrlEnv*_tmp119;void*_tmp118;void*_tmp117;int _tmp116;int _tmp115;int _tmp114;_LL88: _tmp11D=_tmp111.shared;_tmp11C=_tmp111.type_vars;_tmp11B=_tmp111.region_order;_tmp11A=_tmp111.locals;_tmp119=_tmp111.ctrl_env;_tmp118=_tmp111.capability;_tmp117=_tmp111.curr_rgn;_tmp116=(_tmp111.flags).in_notreadctxt;_tmp115=(_tmp111.flags).in_new;_tmp114=(_tmp111.flags).abstract_ok;_LL89:;{
# 488
struct Cyc_Tcenv_Fenv*_tmp112=({struct Cyc_Tcenv_Fenv*_tmp113=_region_malloc(r,sizeof(*_tmp113));_tmp113->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp11D;_tmp113->type_vars=(struct Cyc_List_List*)_tmp11C;_tmp113->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp11B;_tmp113->locals=(const struct Cyc_Tcenv_Bindings*)_tmp11A;_tmp113->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp119;_tmp113->capability=(void*)_tmp118;_tmp113->curr_rgn=(void*)_tmp117;_tmp113->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp113->flags).in_notreadctxt=_tmp116;(_tmp113->flags).in_lhs=0;(_tmp113->flags).in_new=_tmp115;(_tmp113->flags).abstract_ok=_tmp114;_tmp113->flags;});_tmp113->fnrgn=(struct _RegionHandle*)r;_tmp113;});
# 491
return Cyc_Tcenv_put_fenv(r,te,_tmp112);};};}
# 494
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp11E=te->le;
if(_tmp11E == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp11F=_tmp11E;struct Cyc_Tcenv_Fenv*_tmp120=_tmp11F;int _tmp121;_LL8B: _tmp121=(_tmp120->flags).in_lhs;_LL8C:;
return _tmp121;};}
# 502
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 504
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp122;_push_handler(& _tmp122);{int _tmp124=0;if(setjmp(_tmp122.handler))_tmp124=1;if(!_tmp124){
{void*_tmp125=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp126=_tmp125;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp126)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp126)->f1)){case 4: _LL8E: _LL8F:
# 508
({void*_tmp127=0;Cyc_Tcutil_warn(loc,({const char*_tmp128="pattern variable hides local";_tag_dyneither(_tmp128,sizeof(char),29);}),_tag_dyneither(_tmp127,sizeof(void*),0));});goto _LL8D;case 1: _LL90: _LL91:
# 510
({void*_tmp129=0;Cyc_Tcutil_warn(loc,({const char*_tmp12A="pattern variable hides global";_tag_dyneither(_tmp12A,sizeof(char),30);}),_tag_dyneither(_tmp129,sizeof(void*),0));});goto _LL8D;case 5: _LL92: _LL93:
# 512
({void*_tmp12B=0;Cyc_Tcutil_warn(loc,({const char*_tmp12C="pattern variable hides pattern variable";_tag_dyneither(_tmp12C,sizeof(char),40);}),_tag_dyneither(_tmp12B,sizeof(void*),0));});goto _LL8D;case 3: _LL94: _LL95:
# 514
({void*_tmp12D=0;Cyc_Tcutil_warn(loc,({const char*_tmp12E="pattern variable hides parameter";_tag_dyneither(_tmp12E,sizeof(char),33);}),_tag_dyneither(_tmp12D,sizeof(void*),0));});goto _LL8D;default: goto _LL96;}else{_LL96: _LL97:
 goto _LL8D;}_LL8D:;}
# 506
;_pop_handler();}else{void*_tmp123=(void*)_exn_thrown;void*_tmp12F=_tmp123;void*_tmp130;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp12F)->tag == Cyc_Dict_Absent){_LL99: _LL9A:
# 517
 goto _LL98;}else{_LL9B: _tmp130=_tmp12F;_LL9C:(int)_rethrow(_tmp130);}_LL98:;}};}{
struct _dyneither_ptr*_tmp131=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmp132=
# 521
Cyc_Tcenv_get_fenv(te,({const char*_tmp143="add_pat_var";_tag_dyneither(_tmp143,sizeof(char),12);}));
# 519
struct Cyc_Tcenv_Fenv*_tmp133=_tmp132;struct Cyc_Tcenv_SharedFenv*_tmp142;struct Cyc_List_List*_tmp141;struct Cyc_RgnOrder_RgnPO*_tmp140;const struct Cyc_Tcenv_Bindings*_tmp13F;struct Cyc_Tcenv_CtrlEnv*_tmp13E;void*_tmp13D;void*_tmp13C;struct Cyc_Tcenv_FenvFlags _tmp13B;_LL9E: _tmp142=_tmp133->shared;_tmp141=_tmp133->type_vars;_tmp140=_tmp133->region_order;_tmp13F=_tmp133->locals;_tmp13E=_tmp133->ctrl_env;_tmp13D=_tmp133->capability;_tmp13C=_tmp133->curr_rgn;_tmp13B=_tmp133->flags;_LL9F:;{
# 522
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp134=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp13A;_tmp13A.tag=5;_tmp13A.f1=vd;_tmp13A;});_tmp139;});
struct Cyc_Tcenv_Bindings*_tmp135=({struct Cyc_Tcenv_Bindings*_tmp138=_region_malloc(r,sizeof(*_tmp138));_tmp138->v=_tmp131;_tmp138->b=(void*)_tmp134;_tmp138->tl=_tmp13F;_tmp138;});
struct Cyc_Tcenv_Fenv*_tmp136=({struct Cyc_Tcenv_Fenv*_tmp137=_region_malloc(r,sizeof(*_tmp137));_tmp137->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp142;_tmp137->type_vars=(struct Cyc_List_List*)_tmp141;_tmp137->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp140;_tmp137->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp135);_tmp137->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13E;_tmp137->capability=(void*)_tmp13D;_tmp137->curr_rgn=(void*)_tmp13C;_tmp137->flags=(struct Cyc_Tcenv_FenvFlags)_tmp13B;_tmp137->fnrgn=(struct _RegionHandle*)r;_tmp137;});
# 526
return Cyc_Tcenv_put_fenv(r,te,_tmp136);};};}
# 529
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp144=te->le;
struct _tuple0*_tmp145=q;union Cyc_Absyn_Nmspace _tmp15F;struct _dyneither_ptr*_tmp15E;_LLA1: _tmp15F=_tmp145->f1;_tmp15E=_tmp145->f2;_LLA2:;{
union Cyc_Absyn_Nmspace _tmp146=_tmp15F;if((_tmp146.Loc_n).tag == 4){_LLA4: _LLA5:
# 534
 if(_tmp144 == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp147=_cycalloc_atomic(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Dict_Absent_exn_struct _tmp148;_tmp148.tag=Cyc_Dict_Absent;_tmp148;});_tmp147;}));{
struct Cyc_Tcenv_Fenv*_tmp149=_tmp144;struct Cyc_Tcenv_Fenv*_tmp14A=_tmp149;const struct Cyc_Tcenv_Bindings*_tmp153;_LLA9: _tmp153=_tmp14A->locals;_LLAA:;{
# 537
struct _handler_cons _tmp14B;_push_handler(& _tmp14B);{int _tmp14D=0;if(setjmp(_tmp14B.handler))_tmp14D=1;if(!_tmp14D){{void*_tmp150=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp14E=_region_malloc(r,sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=Cyc_Tcenv_lookup_binding(_tmp153,_tmp15E);_tmp14F;});_tmp14E;});_npop_handler(0);return _tmp150;};_pop_handler();}else{void*_tmp14C=(void*)_exn_thrown;void*_tmp151=_tmp14C;void*_tmp152;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp151)->tag == Cyc_Tcenv_NoBinding){_LLAC: _LLAD:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LLAE: _tmp152=_tmp151;_LLAF:(int)_rethrow(_tmp152);}_LLAB:;}};};};}else{_LLA6: _LLA7: {
# 540
struct _handler_cons _tmp154;_push_handler(& _tmp154);{int _tmp156=0;if(setjmp(_tmp154.handler))_tmp156=1;if(!_tmp156){{void*_tmp157=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp157;};_pop_handler();}else{void*_tmp155=(void*)_exn_thrown;void*_tmp158=_tmp155;void*_tmp15D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp158)->tag == Cyc_Dict_Absent){_LLB1: _LLB2:
# 542
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp159=_region_malloc(r,sizeof(*_tmp159));_tmp159[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp15A;_tmp15A.tag=0;_tmp15A.f1=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmp15C;_tmp15C.tag=0;_tmp15C.f1=q;_tmp15C;});_tmp15B;});_tmp15A;});_tmp159;});}else{_LLB3: _tmp15D=_tmp158;_LLB4:(int)_rethrow(_tmp15D);}_LLB0:;}};}}_LLA3:;};}
# 548
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 550
struct Cyc_Tcenv_Fenv*_tmp160=Cyc_Tcenv_get_fenv(te,({const char*_tmp165="process_fallthru";_tag_dyneither(_tmp165,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp161=_tmp160;struct Cyc_Tcenv_CtrlEnv*_tmp164;_LLB6: _tmp164=_tmp161->ctrl_env;_LLB7:;{
const struct _tuple10*_tmp162=_tmp164->fallthru_clause;
if(_tmp162 != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163[0]=(*_tmp162).f1;_tmp163;});
# 555
return _tmp162;};}
# 558
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 562
struct Cyc_Tcenv_Fenv*_tmp166=
# 564
Cyc_Tcenv_get_fenv(te,({const char*_tmp179="set_fallthru";_tag_dyneither(_tmp179,sizeof(char),13);}));
# 562
struct Cyc_Tcenv_Fenv*_tmp167=_tmp166;struct Cyc_Tcenv_SharedFenv*_tmp178;struct Cyc_List_List*_tmp177;struct Cyc_RgnOrder_RgnPO*_tmp176;const struct Cyc_Tcenv_Bindings*_tmp175;struct Cyc_Tcenv_CtrlEnv*_tmp174;void*_tmp173;void*_tmp172;struct Cyc_Tcenv_FenvFlags _tmp171;_LLB9: _tmp178=_tmp167->shared;_tmp177=_tmp167->type_vars;_tmp176=_tmp167->region_order;_tmp175=_tmp167->locals;_tmp174=_tmp167->ctrl_env;_tmp173=_tmp167->capability;_tmp172=_tmp167->curr_rgn;_tmp171=_tmp167->flags;_LLBA:;{
# 565
struct Cyc_Tcenv_CtrlEnv*_tmp168=_tmp174;const struct _tuple10*_tmp170;_LLBC: _tmp170=_tmp168->fallthru_clause;_LLBD:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp169->tl=ft_typ;_tmp169;});}{
struct Cyc_List_List*_tmp16A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp16B=({struct Cyc_Tcenv_CtrlEnv*_tmp16E=_region_malloc(r,sizeof(*_tmp16E));_tmp16E->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->f1=clause;_tmp16F->f2=new_tvs;_tmp16F->f3=(const struct Cyc_Tcenv_CList*)_tmp16A;_tmp16F;});_tmp16E;});
# 572
struct Cyc_Tcenv_Fenv*_tmp16C=({struct Cyc_Tcenv_Fenv*_tmp16D=_region_malloc(r,sizeof(*_tmp16D));_tmp16D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp178;_tmp16D->type_vars=(struct Cyc_List_List*)_tmp177;_tmp16D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp176;_tmp16D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp175;_tmp16D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16B;_tmp16D->capability=(void*)_tmp173;_tmp16D->curr_rgn=(void*)_tmp172;_tmp16D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp171;_tmp16D->fnrgn=(struct _RegionHandle*)r;_tmp16D;});
# 576
return Cyc_Tcenv_put_fenv(r,te,_tmp16C);};};};}
# 579
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp17C="clear_fallthru";_tag_dyneither(_tmp17C,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp17A=fe;struct Cyc_Tcenv_CtrlEnv*_tmp17B;_LLBF: _tmp17B=_tmp17A->ctrl_env;_LLC0:;
_tmp17B->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 586
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp17D=_region_malloc(r2,sizeof(*_tmp17D));_tmp17D->ns=te->ns;_tmp17D->ae=te->ae;_tmp17D->le=0;_tmp17D->allow_valueof=1;_tmp17D->in_extern_c_include=te->in_extern_c_include;_tmp17D;});{
struct Cyc_Tcenv_Fenv _tmp17E=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp17F=_tmp17E;struct Cyc_Tcenv_SharedFenv*_tmp18A;struct Cyc_List_List*_tmp189;struct Cyc_RgnOrder_RgnPO*_tmp188;const struct Cyc_Tcenv_Bindings*_tmp187;struct Cyc_Tcenv_CtrlEnv*_tmp186;void*_tmp185;void*_tmp184;struct Cyc_Tcenv_FenvFlags _tmp183;_LLC2: _tmp18A=_tmp17F.shared;_tmp189=_tmp17F.type_vars;_tmp188=_tmp17F.region_order;_tmp187=_tmp17F.locals;_tmp186=_tmp17F.ctrl_env;_tmp185=_tmp17F.capability;_tmp184=_tmp17F.curr_rgn;_tmp183=_tmp17F.flags;_LLC3:;{
struct Cyc_Tcenv_Fenv*_tmp180=({struct Cyc_Tcenv_Fenv*_tmp182=_region_malloc(r2,sizeof(*_tmp182));_tmp182->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp18A;_tmp182->type_vars=(struct Cyc_List_List*)_tmp189;_tmp182->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp188;_tmp182->locals=(const struct Cyc_Tcenv_Bindings*)_tmp187;_tmp182->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp186;_tmp182->capability=(void*)_tmp185;_tmp182->curr_rgn=(void*)_tmp184;_tmp182->flags=(struct Cyc_Tcenv_FenvFlags)_tmp183;_tmp182->fnrgn=(struct _RegionHandle*)r2;_tmp182;});
return({struct Cyc_Tcenv_Tenv*_tmp181=_region_malloc(r2,sizeof(*_tmp181));_tmp181->ns=te->ns;_tmp181->ae=te->ae;_tmp181->le=_tmp180;_tmp181->allow_valueof=1;_tmp181->in_extern_c_include=te->in_extern_c_include;_tmp181;});};};}
# 594
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp18B=_region_malloc(r2,sizeof(*_tmp18B));_tmp18B->ns=te->ns;_tmp18B->ae=te->ae;_tmp18B->le=0;_tmp18B->allow_valueof=te->allow_valueof;_tmp18B->in_extern_c_include=1;_tmp18B;});{
struct Cyc_Tcenv_Fenv _tmp18C=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp18D=_tmp18C;struct Cyc_Tcenv_SharedFenv*_tmp198;struct Cyc_List_List*_tmp197;struct Cyc_RgnOrder_RgnPO*_tmp196;const struct Cyc_Tcenv_Bindings*_tmp195;struct Cyc_Tcenv_CtrlEnv*_tmp194;void*_tmp193;void*_tmp192;struct Cyc_Tcenv_FenvFlags _tmp191;_LLC5: _tmp198=_tmp18D.shared;_tmp197=_tmp18D.type_vars;_tmp196=_tmp18D.region_order;_tmp195=_tmp18D.locals;_tmp194=_tmp18D.ctrl_env;_tmp193=_tmp18D.capability;_tmp192=_tmp18D.curr_rgn;_tmp191=_tmp18D.flags;_LLC6:;{
struct Cyc_Tcenv_Fenv*_tmp18E=({struct Cyc_Tcenv_Fenv*_tmp190=_region_malloc(r2,sizeof(*_tmp190));_tmp190->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp198;_tmp190->type_vars=(struct Cyc_List_List*)_tmp197;_tmp190->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp196;_tmp190->locals=(const struct Cyc_Tcenv_Bindings*)_tmp195;_tmp190->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp194;_tmp190->capability=(void*)_tmp193;_tmp190->curr_rgn=(void*)_tmp192;_tmp190->flags=(struct Cyc_Tcenv_FenvFlags)_tmp191;_tmp190->fnrgn=(struct _RegionHandle*)r2;_tmp190;});
return({struct Cyc_Tcenv_Tenv*_tmp18F=_region_malloc(r2,sizeof(*_tmp18F));_tmp18F->ns=te->ns;_tmp18F->ae=te->ae;_tmp18F->le=_tmp18E;_tmp18F->allow_valueof=te->allow_valueof;_tmp18F->in_extern_c_include=1;_tmp18F;});};};}
# 602
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 605
struct Cyc_Tcenv_Fenv _tmp199=*
# 607
Cyc_Tcenv_get_fenv(te,({const char*_tmp1AE="add_region";_tag_dyneither(_tmp1AE,sizeof(char),11);}));
# 605
struct Cyc_Tcenv_Fenv _tmp19A=_tmp199;struct Cyc_Tcenv_SharedFenv*_tmp1AD;struct Cyc_List_List*_tmp1AC;struct Cyc_RgnOrder_RgnPO*_tmp1AB;const struct Cyc_Tcenv_Bindings*_tmp1AA;struct Cyc_Tcenv_CtrlEnv*_tmp1A9;void*_tmp1A8;void*_tmp1A7;struct Cyc_Tcenv_FenvFlags _tmp1A6;_LLC8: _tmp1AD=_tmp19A.shared;_tmp1AC=_tmp19A.type_vars;_tmp1AB=_tmp19A.region_order;_tmp1AA=_tmp19A.locals;_tmp1A9=_tmp19A.ctrl_env;_tmp1A8=_tmp19A.capability;_tmp1A7=_tmp19A.curr_rgn;_tmp1A6=_tmp19A.flags;_LLC9:;
# 608
{void*_tmp19B=Cyc_Tcutil_compress(rgn);void*_tmp19C=_tmp19B;struct Cyc_Absyn_Tvar*_tmp19D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19C)->tag == 2){_LLCB: _tmp19D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19C)->f1;_LLCC:
# 610
 _tmp1AB=Cyc_RgnOrder_add_youngest(_tmp1AD->frgn,_tmp1AB,_tmp19D,resetable,opened);
# 612
goto _LLCA;}else{_LLCD: _LLCE:
 goto _LLCA;}_LLCA:;}
# 615
_tmp1A8=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp19F;_tmp19F.tag=24;_tmp19F.f1=({struct Cyc_List_List*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1A3;_tmp1A3.tag=23;_tmp1A3.f1=rgn;_tmp1A3;});_tmp1A2;});_tmp1A0->tl=({struct Cyc_List_List*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->hd=_tmp1A8;_tmp1A1->tl=0;_tmp1A1;});_tmp1A0;});_tmp19F;});_tmp19E;});{
# 617
struct Cyc_Tcenv_Fenv*_tmp1A4=({struct Cyc_Tcenv_Fenv*_tmp1A5=_region_malloc(r,sizeof(*_tmp1A5));_tmp1A5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1AD;_tmp1A5->type_vars=(struct Cyc_List_List*)_tmp1AC;_tmp1A5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1AB;_tmp1A5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1AA;_tmp1A5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1A9;_tmp1A5->capability=(void*)_tmp1A8;_tmp1A5->curr_rgn=(void*)_tmp1A7;_tmp1A5->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1A6;_tmp1A5->fnrgn=(struct _RegionHandle*)r;_tmp1A5;});
# 620
return Cyc_Tcenv_put_fenv(r,te,_tmp1A4);};}
# 623
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 625
struct Cyc_Tcenv_Fenv _tmp1AF=*
# 627
Cyc_Tcenv_get_fenv(te,({const char*_tmp1C5="new_named_block";_tag_dyneither(_tmp1C5,sizeof(char),16);}));
# 625
struct Cyc_Tcenv_Fenv _tmp1B0=_tmp1AF;struct Cyc_Tcenv_SharedFenv*_tmp1C4;struct Cyc_List_List*_tmp1C3;struct Cyc_RgnOrder_RgnPO*_tmp1C2;const struct Cyc_Tcenv_Bindings*_tmp1C1;struct Cyc_Tcenv_CtrlEnv*_tmp1C0;void*_tmp1BF;void*_tmp1BE;struct Cyc_Tcenv_FenvFlags _tmp1BD;_LLD0: _tmp1C4=_tmp1B0.shared;_tmp1C3=_tmp1B0.type_vars;_tmp1C2=_tmp1B0.region_order;_tmp1C1=_tmp1B0.locals;_tmp1C0=_tmp1B0.ctrl_env;_tmp1BF=_tmp1B0.capability;_tmp1BE=_tmp1B0.curr_rgn;_tmp1BD=_tmp1B0.flags;_LLD1:;{
# 629
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1BC="new_block";_tag_dyneither(_tmp1BC,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1BB;_tmp1BB.tag=2;_tmp1BB.f1=block_rgn;_tmp1BB;});_tmp1BA;});
_tmp1C3=({struct Cyc_List_List*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->hd=block_rgn;_tmp1B1->tl=_tmp1C3;_tmp1B1;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp1C3);
_tmp1C2=Cyc_RgnOrder_add_youngest(_tmp1C4->frgn,_tmp1C2,block_rgn,0,0);
_tmp1BF=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1B3;_tmp1B3.tag=24;_tmp1B3.f1=({struct Cyc_List_List*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1B7;_tmp1B7.tag=23;_tmp1B7.f1=block_typ;_tmp1B7;});_tmp1B6;});_tmp1B4->tl=({struct Cyc_List_List*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->hd=_tmp1BF;_tmp1B5->tl=0;_tmp1B5;});_tmp1B4;});_tmp1B3;});_tmp1B2;});
_tmp1BE=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp1B8=({struct Cyc_Tcenv_Fenv*_tmp1B9=_region_malloc(r,sizeof(*_tmp1B9));_tmp1B9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1C4;_tmp1B9->type_vars=(struct Cyc_List_List*)_tmp1C3;_tmp1B9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C2;_tmp1B9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1C1;_tmp1B9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1C0;_tmp1B9->capability=(void*)_tmp1BF;_tmp1B9->curr_rgn=(void*)_tmp1BE;_tmp1B9->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1BD;_tmp1B9->fnrgn=(struct _RegionHandle*)r;_tmp1B9;});
# 639
return Cyc_Tcenv_put_fenv(r,te,_tmp1B8);};};}
# 642
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 644
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 646
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 652
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 656
struct Cyc_Tcenv_Fenv _tmp1C7=*
# 658
Cyc_Tcenv_get_fenv(te,({const char*_tmp1D3="new_outlives_constraints";_tag_dyneither(_tmp1D3,sizeof(char),25);}));
# 656
struct Cyc_Tcenv_Fenv _tmp1C8=_tmp1C7;struct Cyc_Tcenv_SharedFenv*_tmp1D2;struct Cyc_List_List*_tmp1D1;struct Cyc_RgnOrder_RgnPO*_tmp1D0;const struct Cyc_Tcenv_Bindings*_tmp1CF;struct Cyc_Tcenv_CtrlEnv*_tmp1CE;void*_tmp1CD;void*_tmp1CC;struct Cyc_Tcenv_FenvFlags _tmp1CB;_LLD3: _tmp1D2=_tmp1C8.shared;_tmp1D1=_tmp1C8.type_vars;_tmp1D0=_tmp1C8.region_order;_tmp1CF=_tmp1C8.locals;_tmp1CE=_tmp1C8.ctrl_env;_tmp1CD=_tmp1C8.capability;_tmp1CC=_tmp1C8.curr_rgn;_tmp1CB=_tmp1C8.flags;_LLD4:;
# 659
for(0;cs != 0;cs=cs->tl){
_tmp1D0=
Cyc_RgnOrder_add_outlives_constraint(_tmp1D2->frgn,_tmp1D0,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 664
struct Cyc_Tcenv_Fenv*_tmp1C9=({struct Cyc_Tcenv_Fenv*_tmp1CA=_region_malloc(r,sizeof(*_tmp1CA));_tmp1CA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1D2;_tmp1CA->type_vars=(struct Cyc_List_List*)_tmp1D1;_tmp1CA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1D0;_tmp1CA->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1CF;_tmp1CA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1CE;_tmp1CA->capability=(void*)_tmp1CD;_tmp1CA->curr_rgn=(void*)_tmp1CC;_tmp1CA->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1CB;_tmp1CA->fnrgn=(struct _RegionHandle*)r;_tmp1CA;});
# 667
return Cyc_Tcenv_put_fenv(r,te,_tmp1C9);};}
# 670
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 678
void*_tmp1D4=Cyc_Tcutil_compress(r1);
void*_tmp1D5=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp1D6=Cyc_Tcutil_typ_kind(_tmp1D4);
struct Cyc_Absyn_Kind*_tmp1D7=Cyc_Tcutil_typ_kind(_tmp1D5);
# 684
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp1D6,_tmp1D7);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp1D7,_tmp1D6);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp1DD;_tmp1DD.tag=0;_tmp1DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp1D7));({struct Cyc_String_pa_PrintArg_struct _tmp1DC;_tmp1DC.tag=0;_tmp1DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp1D6));({struct Cyc_String_pa_PrintArg_struct _tmp1DB;_tmp1DB.tag=0;_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D5));({struct Cyc_String_pa_PrintArg_struct _tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D4));({void*_tmp1D8[4]={& _tmp1DA,& _tmp1DB,& _tmp1DC,& _tmp1DD};Cyc_Tcutil_terr(loc,({const char*_tmp1D9="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp1D9,sizeof(char),82);}),_tag_dyneither(_tmp1D8,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 691
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp1DE=Cyc_Tcutil_swap_kind(_tmp1D5,Cyc_Tcutil_kind_to_bound(_tmp1D6));struct _tuple11 _tmp1DF=_tmp1DE;struct Cyc_Absyn_Tvar*_tmp1E2;void*_tmp1E1;_LLD6: _tmp1E2=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;_LLD7:;
*oldtv=({struct _tuple11*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->f1=_tmp1E2;_tmp1E0->f2=_tmp1E1;_tmp1E0;});}else{
# 695
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp1E3=Cyc_Tcutil_swap_kind(_tmp1D4,Cyc_Tcutil_kind_to_bound(_tmp1D7));struct _tuple11 _tmp1E4=_tmp1E3;struct Cyc_Absyn_Tvar*_tmp1E7;void*_tmp1E6;_LLD9: _tmp1E7=_tmp1E4.f1;_tmp1E6=_tmp1E4.f2;_LLDA:;
*oldtv=({struct _tuple11*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->f1=_tmp1E7;_tmp1E5->f2=_tmp1E6;_tmp1E5;});}}}{
# 701
struct _RegionHandle*_tmp1E8=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp1E9=0;
if((_tmp1D4 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp1D4 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp1D4 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1ED;_tmp1ED.tag=23;_tmp1ED.f1=_tmp1D4;_tmp1ED;});_tmp1EC;});
_tmp1E9=({struct Cyc_List_List*_tmp1EA=_region_malloc(_tmp1E8,sizeof(*_tmp1EA));_tmp1EA->hd=({struct _tuple13*_tmp1EB=_region_malloc(_tmp1E8,sizeof(*_tmp1EB));_tmp1EB->f1=eff1;_tmp1EB->f2=_tmp1D5;_tmp1EB;});_tmp1EA->tl=_tmp1E9;_tmp1EA;});}
# 707
if((_tmp1D5 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp1D5 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp1D5 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1F1;_tmp1F1.tag=23;_tmp1F1.f1=_tmp1D5;_tmp1F1;});_tmp1F0;});
_tmp1E9=({struct Cyc_List_List*_tmp1EE=_region_malloc(_tmp1E8,sizeof(*_tmp1EE));_tmp1EE->hd=({struct _tuple13*_tmp1EF=_region_malloc(_tmp1E8,sizeof(*_tmp1EF));_tmp1EF->f1=eff2;_tmp1EF->f2=_tmp1D4;_tmp1EF;});_tmp1EE->tl=_tmp1E9;_tmp1EE;});}
# 712
return Cyc_Tcenv_new_outlives_constraints(_tmp1E8,te,_tmp1E9,loc);};}
# 715
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1F2=te->le;
if(_tmp1F2 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp1F2;
struct Cyc_Tcenv_Fenv*_tmp1F3=fe;void*_tmp1F4;_LLDC: _tmp1F4=_tmp1F3->curr_rgn;_LLDD:;
return _tmp1F4;};}
# 726
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp1FD="check_rgn_accessible";_tag_dyneither(_tmp1FD,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp1F5=fe;void*_tmp1FC;struct Cyc_RgnOrder_RgnPO*_tmp1FB;_LLDF: _tmp1FC=_tmp1F5->capability;_tmp1FB=_tmp1F5->region_order;_LLE0:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp1FC) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp1FC))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp1FB,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1F7;_tmp1F7.tag=23;_tmp1F7.f1=rgn;_tmp1F7;});_tmp1F6;}),_tmp1FC))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp1F8[1]={& _tmp1FA};Cyc_Tcutil_terr(loc,({const char*_tmp1F9="Expression accesses unavailable region %s";_tag_dyneither(_tmp1F9,sizeof(char),42);}),_tag_dyneither(_tmp1F8,sizeof(void*),1));});});}
# 738
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp1FE=
Cyc_Tcenv_get_fenv(te,({const char*_tmp209="check_rgn_resetable";_tag_dyneither(_tmp209,sizeof(char),20);}));
# 740
struct Cyc_Tcenv_Fenv*_tmp1FF=_tmp1FE;struct Cyc_RgnOrder_RgnPO*_tmp208;_LLE2: _tmp208=_tmp1FF->region_order;_LLE3:;{
# 742
void*_tmp200=Cyc_Tcutil_compress(rgn);void*_tmp201=_tmp200;struct Cyc_Absyn_Tvar*_tmp207;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp201)->tag == 2){_LLE5: _tmp207=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp201)->f1;_LLE6:
# 744
 if(!Cyc_RgnOrder_is_region_resetable(_tmp208,_tmp207))
({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0;_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp202[1]={& _tmp204};Cyc_Tcutil_terr(loc,({const char*_tmp203="Region %s is not resetable";_tag_dyneither(_tmp203,sizeof(char),27);}),_tag_dyneither(_tmp202,sizeof(void*),1));});});
return;}else{_LLE7: _LLE8:
({void*_tmp205=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp206="check_rgn_resetable";_tag_dyneither(_tmp206,sizeof(char),20);}),_tag_dyneither(_tmp205,sizeof(void*),0));});}_LLE4:;};};}
# 754
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp20A=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 759
if(_tmp20A == 0){
void*_tmp20B=rt_a;switch(*((int*)_tmp20B)){case 22: _LLEA: _LLEB:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LLEC: _LLED:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LLEE: _LLEF:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LLF0: _LLF1:
 return 0;}_LLE9:;}{
# 767
struct Cyc_Tcenv_Fenv*fe=_tmp20A;
struct Cyc_Tcenv_Fenv*_tmp20C=fe;struct Cyc_RgnOrder_RgnPO*_tmp210;_LLF3: _tmp210=_tmp20C->region_order;_LLF4:;{
int _tmp20D=Cyc_RgnOrder_effect_outlives(_tmp210,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp20F;_tmp20F.tag=23;_tmp20F.f1=rt_a;_tmp20F;});_tmp20E;}),rt_b);
# 773
return _tmp20D;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 778
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp211=
Cyc_Tcenv_get_fenv(te,({const char*_tmp218="check_effect_accessible";_tag_dyneither(_tmp218,sizeof(char),24);}));
# 779
struct Cyc_Tcenv_Fenv*_tmp212=_tmp211;void*_tmp217;struct Cyc_RgnOrder_RgnPO*_tmp216;struct Cyc_Tcenv_SharedFenv*_tmp215;_LLF6: _tmp217=_tmp212->capability;_tmp216=_tmp212->region_order;_tmp215=_tmp212->shared;_LLF7:;
# 781
if(Cyc_Tcutil_subset_effect(0,eff,_tmp217))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp216,eff,_tmp217))
return;{
struct _RegionHandle*frgn=_tmp215->frgn;
_tmp215->delayed_effect_checks=({struct Cyc_List_List*_tmp213=_region_malloc(frgn,sizeof(*_tmp213));_tmp213->hd=({struct _tuple14*_tmp214=_region_malloc(frgn,sizeof(*_tmp214));_tmp214->f1=_tmp217;_tmp214->f2=eff;_tmp214->f3=_tmp216;_tmp214->f4=loc;_tmp214;});_tmp213->tl=_tmp215->delayed_effect_checks;_tmp213;});};}
# 790
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp219=
Cyc_Tcenv_get_fenv(te,({const char*_tmp227="check_delayed_effects";_tag_dyneither(_tmp227,sizeof(char),22);}));
# 791
struct Cyc_Tcenv_Fenv*_tmp21A=_tmp219;struct Cyc_Tcenv_SharedFenv*_tmp226;_LLF9: _tmp226=_tmp21A->shared;_LLFA:;{
# 793
struct Cyc_List_List*_tmp21B=_tmp226->delayed_effect_checks;
for(0;_tmp21B != 0;_tmp21B=_tmp21B->tl){
struct _tuple14*_tmp21C=(struct _tuple14*)_tmp21B->hd;struct _tuple14*_tmp21D=_tmp21C;void*_tmp225;void*_tmp224;struct Cyc_RgnOrder_RgnPO*_tmp223;unsigned int _tmp222;_LLFC: _tmp225=_tmp21D->f1;_tmp224=_tmp21D->f2;_tmp223=_tmp21D->f3;_tmp222=_tmp21D->f4;_LLFD:;
if(Cyc_Tcutil_subset_effect(1,_tmp224,_tmp225))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp223,_tmp224,_tmp225))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp224));({struct Cyc_String_pa_PrintArg_struct _tmp220;_tmp220.tag=0;_tmp220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp225));({void*_tmp21E[2]={& _tmp220,& _tmp221};Cyc_Tcutil_terr(_tmp222,({const char*_tmp21F="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp21F,sizeof(char),51);}),_tag_dyneither(_tmp21E,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 809
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 811
struct Cyc_Tcenv_Fenv*_tmp228=te->le;
if(_tmp228 == 0){
# 814
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp22A;_tmp22A.tag=23;_tmp22A.f1=(*((struct _tuple13*)po->hd)).f2;_tmp22A;});_tmp229;}),Cyc_Absyn_empty_effect))
({void*_tmp22B=0;Cyc_Tcutil_terr(loc,({const char*_tmp22C="the required region ordering is not satisfied here";_tag_dyneither(_tmp22C,sizeof(char),51);}),_tag_dyneither(_tmp22B,sizeof(void*),0));});}
return;}{
# 820
struct Cyc_Tcenv_Fenv*_tmp22D=_tmp228;struct Cyc_Tcenv_Fenv*_tmp22E=_tmp22D;struct Cyc_RgnOrder_RgnPO*_tmp232;struct Cyc_Tcenv_SharedFenv*_tmp231;_LLFF: _tmp232=_tmp22E->region_order;_tmp231=_tmp22E->shared;_LL100:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp232,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp231->delayed_constraint_checks=({struct Cyc_List_List*_tmp22F=_region_malloc(_tmp231->frgn,sizeof(*_tmp22F));_tmp22F->hd=({struct _tuple15*_tmp230=_region_malloc(_tmp231->frgn,sizeof(*_tmp230));_tmp230->f1=_tmp232;_tmp230->f2=po;_tmp230->f3=loc;_tmp230;});_tmp22F->tl=_tmp231->delayed_constraint_checks;_tmp22F;});};}
# 827
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp233=
Cyc_Tcenv_get_fenv(te,({const char*_tmp23E="check_delayed_constraints";_tag_dyneither(_tmp23E,sizeof(char),26);}));
# 828
struct Cyc_Tcenv_Fenv*_tmp234=_tmp233;struct Cyc_Tcenv_SharedFenv*_tmp23D;_LL102: _tmp23D=_tmp234->shared;_LL103:;{
# 830
struct Cyc_List_List*_tmp235=_tmp23D->delayed_constraint_checks;
for(0;_tmp235 != 0;_tmp235=_tmp235->tl){
struct _tuple15*_tmp236=(struct _tuple15*)_tmp235->hd;struct _tuple15*_tmp237=_tmp236;struct Cyc_RgnOrder_RgnPO*_tmp23C;struct Cyc_List_List*_tmp23B;unsigned int _tmp23A;_LL105: _tmp23C=_tmp237->f1;_tmp23B=_tmp237->f2;_tmp23A=_tmp237->f3;_LL106:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp23C,_tmp23B,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp238=0;Cyc_Tcutil_terr(_tmp23A,({const char*_tmp239="the required region ordering is not satisfied here";_tag_dyneither(_tmp239,sizeof(char),51);}),_tag_dyneither(_tmp238,sizeof(void*),0));});}};}
# 838
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->name=({struct _dyneither_ptr*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp25C[1]={& _tmp25E};Cyc_aprintf(({const char*_tmp25D="`%s";_tag_dyneither(_tmp25D,sizeof(char),4);}),_tag_dyneither(_tmp25C,sizeof(void*),1));});});_tmp25B;});_tmp25A->identity=
# 842
Cyc_Tcutil_new_tvar_id();_tmp25A->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp25A;});
struct Cyc_List_List*_tmp23F=({struct Cyc_List_List*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->hd=rgn0;_tmp259->tl=fd->tvs;_tmp259;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp23F);{
# 846
struct Cyc_RgnOrder_RgnPO*_tmp240=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 848
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp258;_tmp258.tag=2;_tmp258.f1=rgn0;_tmp258;});_tmp257;});
{struct Cyc_List_List*_tmp241=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp241 != 0;_tmp241=_tmp241->tl){
struct Cyc_Absyn_Vardecl*_tmp242=(struct Cyc_Absyn_Vardecl*)_tmp241->hd;
_tmp242->rgn=param_rgn;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp243=_region_malloc(r,sizeof(*_tmp243));_tmp243->v=(*_tmp242->name).f2;_tmp243->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp245;_tmp245.tag=3;_tmp245.f1=_tmp242;_tmp245;});_tmp244;});_tmp243->tl=locals;_tmp243;});}}
# 854
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp246=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp247=_tmp246;struct _dyneither_ptr*_tmp24D;struct Cyc_Absyn_Tqual _tmp24C;void*_tmp24B;int _tmp24A;_LL108: _tmp24D=_tmp247.name;_tmp24C=_tmp247.tq;_tmp24B=_tmp247.type;_tmp24A=_tmp247.inject;_LL109:;{
# 857
struct Cyc_List_List*_tmp248=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
struct Cyc_Absyn_Vardecl*_tmp249=(struct Cyc_Absyn_Vardecl*)_tmp248->hd;
if(Cyc_strptrcmp((*_tmp249->name).f2,(struct _dyneither_ptr*)_check_null(_tmp24D))== 0){
_tmp249->type=Cyc_Absyn_dyneither_typ(_tmp24B,param_rgn,_tmp24C,Cyc_Absyn_false_conref);
break;}}};}
# 865
return({struct Cyc_Tcenv_Fenv*_tmp24E=_region_malloc(r,sizeof(*_tmp24E));_tmp24E->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp256=_region_malloc(r,sizeof(*_tmp256));_tmp256->frgn=r;_tmp256->return_typ=fd->ret_type;_tmp256->delayed_effect_checks=0;_tmp256->delayed_constraint_checks=0;_tmp256;});_tmp24E->type_vars=(struct Cyc_List_List*)_tmp23F;_tmp24E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp240;_tmp24E->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp24E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp255=_region_malloc(r,sizeof(*_tmp255));_tmp255->fallthru_clause=0;_tmp255;});_tmp24E->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp250;_tmp250.tag=24;_tmp250.f1=({struct Cyc_List_List*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp254;_tmp254.tag=23;_tmp254.f1=param_rgn;_tmp254;});_tmp253;});_tmp251->tl=({struct Cyc_List_List*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252->hd=(void*)_check_null(fd->effect);_tmp252->tl=0;_tmp252;});_tmp251;});_tmp250;});_tmp24F;}));_tmp24E->curr_rgn=(void*)param_rgn;_tmp24E->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp24E->flags).in_notreadctxt=0;(_tmp24E->flags).in_lhs=0;(_tmp24E->flags).in_new=0;(_tmp24E->flags).abstract_ok=0;_tmp24E->flags;});_tmp24E->fnrgn=(struct _RegionHandle*)r;_tmp24E;});};}
# 884
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 887
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp27F->identity=Cyc_Tcutil_new_tvar_id();_tmp27F->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp27F;});
struct Cyc_List_List*_tmp25F=({struct Cyc_List_List*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->hd=rgn0;_tmp27E->tl=0;_tmp27E;});
struct Cyc_RgnOrder_RgnPO*_tmp260=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp27D;_tmp27D.tag=24;_tmp27D.f1=0;_tmp27D;});_tmp27C;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp27B;_tmp27B.tag=2;_tmp27B.f1=rgn0;_tmp27B;});_tmp27A;});
struct Cyc_List_List*_tmp261=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp262=*((struct _tuple8*)args->hd);struct _tuple8 _tmp263=_tmp262;struct _dyneither_ptr*_tmp271;struct Cyc_Absyn_Tqual _tmp270;void*_tmp26F;_LL10B: _tmp271=_tmp263.f1;_tmp270=_tmp263.f2;_tmp26F=_tmp263.f3;_LL10C:;
if(_tmp271 != 0){
struct Cyc_Absyn_Vardecl*_tmp264=({struct Cyc_Absyn_Vardecl*_tmp26C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp26C[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp26D;_tmp26D.sc=Cyc_Absyn_Public;_tmp26D.name=({struct _tuple0*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->f1=Cyc_Absyn_Loc_n;_tmp26E->f2=_tmp271;_tmp26E;});_tmp26D.tq=_tmp270;_tmp26D.type=_tmp26F;_tmp26D.initializer=0;_tmp26D.rgn=param_rgn;_tmp26D.attributes=0;_tmp26D.escapes=0;_tmp26D;});_tmp26C;});
# 903
_tmp261=({struct Cyc_List_List*_tmp265=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp265[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp266;_tmp266.hd=_tmp264;_tmp266.tl=_tmp261;_tmp266;});_tmp265;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp267=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp26B;_tmp26B.tag=3;_tmp26B.f1=_tmp264;_tmp26B;});_tmp26A;});
struct _dyneither_ptr*_tmp268=_tmp271;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp269=_region_malloc(r,sizeof(*_tmp269));_tmp269->v=_tmp268;_tmp269->b=(void*)_tmp267;_tmp269->tl=locals;_tmp269;});};}}
# 909
return({struct Cyc_Tcenv_Fenv*_tmp272=_region_malloc(r,sizeof(*_tmp272));_tmp272->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp279=_region_malloc(r,sizeof(*_tmp279));_tmp279->frgn=r;_tmp279->return_typ=ret_type;_tmp279->delayed_effect_checks=0;_tmp279->delayed_constraint_checks=0;_tmp279;});_tmp272->type_vars=(struct Cyc_List_List*)_tmp25F;_tmp272->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp260;_tmp272->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp272->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp278=_region_malloc(r,sizeof(*_tmp278));_tmp278->fallthru_clause=0;_tmp278;});_tmp272->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp274;_tmp274.tag=24;_tmp274.f1=({struct Cyc_List_List*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp277;_tmp277.tag=23;_tmp277.f1=param_rgn;_tmp277;});_tmp276;});_tmp275->tl=0;_tmp275;});_tmp274;});_tmp273;}));_tmp272->curr_rgn=(void*)param_rgn;_tmp272->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp272->flags).in_notreadctxt=0;(_tmp272->flags).in_lhs=0;(_tmp272->flags).in_new=0;(_tmp272->flags).abstract_ok=0;_tmp272->flags;});_tmp272->fnrgn=(struct _RegionHandle*)r;_tmp272;});}
# 927
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp281=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp2AE;struct Cyc_RgnOrder_RgnPO*_tmp2AD;struct Cyc_List_List*_tmp2AC;struct Cyc_Tcenv_SharedFenv*_tmp2AB;struct _RegionHandle*_tmp2AA;_LL10E: _tmp2AE=_tmp281->locals;_tmp2AD=_tmp281->region_order;_tmp2AC=_tmp281->type_vars;_tmp2AB=_tmp281->shared;_tmp2AA=_tmp281->fnrgn;_LL10F:;{
# 930
struct _RegionHandle*_tmp282=_tmp2AB->frgn;
const struct Cyc_Tcenv_Bindings*_tmp283=_tmp2AE;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5->name=({struct _dyneither_ptr*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp2A7[1]={& _tmp2A9};Cyc_aprintf(({const char*_tmp2A8="`%s";_tag_dyneither(_tmp2A8,sizeof(char),4);}),_tag_dyneither(_tmp2A7,sizeof(void*),1));});});_tmp2A6;});_tmp2A5->identity=
# 934
Cyc_Tcutil_new_tvar_id();_tmp2A5->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp2A5;});
# 936
{struct Cyc_List_List*_tmp284=fd->tvs;for(0;_tmp284 != 0;_tmp284=_tmp284->tl){
struct Cyc_Absyn_Kind*_tmp285=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp284->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp286=_tmp285;enum Cyc_Absyn_KindQual _tmp28A;enum Cyc_Absyn_AliasQual _tmp289;_LL111: _tmp28A=_tmp286->kind;_tmp289=_tmp286->aliasqual;_LL112:;
if(_tmp28A == Cyc_Absyn_RgnKind){
if(_tmp289 == Cyc_Absyn_Aliasable)
_tmp2AD=Cyc_RgnOrder_add_youngest(_tmp282,_tmp2AD,(struct Cyc_Absyn_Tvar*)_tmp284->hd,0,0);else{
# 942
({void*_tmp287=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp288="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp288,sizeof(char),39);}),_tag_dyneither(_tmp287,sizeof(void*),0));});}}}}
# 944
_tmp2AD=Cyc_RgnOrder_add_youngest(_tmp282,_tmp2AD,rgn0,0,0);{
struct Cyc_List_List*_tmp28B=({struct Cyc_List_List*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4->hd=rgn0;_tmp2A4->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp2AC);_tmp2A4;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp28B);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2A3;_tmp2A3.tag=2;_tmp2A3.f1=rgn0;_tmp2A3;});_tmp2A2;});
{struct Cyc_List_List*_tmp28C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp28C != 0;_tmp28C=_tmp28C->tl){
struct Cyc_Absyn_Vardecl*_tmp28D=(struct Cyc_Absyn_Vardecl*)_tmp28C->hd;
_tmp28D->rgn=param_rgn;
_tmp283=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp28E=_region_malloc(_tmp282,sizeof(*_tmp28E));_tmp28E->v=(*_tmp28D->name).f2;_tmp28E->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp290;_tmp290.tag=3;_tmp290.f1=_tmp28D;_tmp290;});_tmp28F;});_tmp28E->tl=_tmp283;_tmp28E;});}}
# 953
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp291=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp292=_tmp291;struct _dyneither_ptr*_tmp298;struct Cyc_Absyn_Tqual _tmp297;void*_tmp296;int _tmp295;_LL114: _tmp298=_tmp292.name;_tmp297=_tmp292.tq;_tmp296=_tmp292.type;_tmp295=_tmp292.inject;_LL115:;{
# 956
struct Cyc_List_List*_tmp293=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp293 != 0;_tmp293=_tmp293->tl){
struct Cyc_Absyn_Vardecl*_tmp294=(struct Cyc_Absyn_Vardecl*)_tmp293->hd;
if(Cyc_strptrcmp((*_tmp294->name).f2,(struct _dyneither_ptr*)_check_null(_tmp298))== 0){
_tmp294->type=Cyc_Absyn_dyneither_typ(_tmp296,param_rgn,_tmp297,Cyc_Absyn_false_conref);
break;}}};}
# 964
return({struct Cyc_Tcenv_Fenv*_tmp299=_region_malloc(_tmp282,sizeof(*_tmp299));_tmp299->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp2A1=_region_malloc(_tmp282,sizeof(*_tmp2A1));_tmp2A1->frgn=_tmp282;_tmp2A1->return_typ=fd->ret_type;_tmp2A1->delayed_effect_checks=0;_tmp2A1->delayed_constraint_checks=0;_tmp2A1;});_tmp299->type_vars=(struct Cyc_List_List*)_tmp28B;_tmp299->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2AD;_tmp299->locals=(const struct Cyc_Tcenv_Bindings*)_tmp283;_tmp299->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2A0=_region_malloc(_tmp282,sizeof(*_tmp2A0));_tmp2A0->fallthru_clause=0;_tmp2A0;});_tmp299->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp29B;_tmp29B.tag=24;_tmp29B.f1=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp29F;_tmp29F.tag=23;_tmp29F.f1=param_rgn;_tmp29F;});_tmp29E;});_tmp29C->tl=({struct Cyc_List_List*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->hd=(void*)_check_null(fd->effect);_tmp29D->tl=0;_tmp29D;});_tmp29C;});_tmp29B;});_tmp29A;}));_tmp299->curr_rgn=(void*)param_rgn;_tmp299->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp299->flags).in_notreadctxt=0;(_tmp299->flags).in_lhs=0;(_tmp299->flags).in_new=0;(_tmp299->flags).abstract_ok=0;_tmp299->flags;});_tmp299->fnrgn=(struct _RegionHandle*)_tmp2AA;_tmp299;});};};};}
