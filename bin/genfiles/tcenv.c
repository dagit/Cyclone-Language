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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 777 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 944
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 953
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 967
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 987
extern void*Cyc_Absyn_empty_effect;
# 989
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1022
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1109
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
# 176
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 182
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 187
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 191
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 195
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 201
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 209
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 211
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 213
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 215
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 217
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 221
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 224
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 227
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 230
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
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;const struct _tuple10*fallthru_clause;void*next_stmt;};
# 166
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 194 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 229 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 232
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 236
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 241
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
return({struct Cyc_Tcenv_Genv*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->grgn=r;_tmp5->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp5->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp5->availables=0;_tmp5;});}struct _tuple12{void*f1;int f2;};
# 252
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 254
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp6;});{
# 256
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Datatypedecl**_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=Cyc_Absyn_exn_tud;_tmp8;}));
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,({struct _tuple12*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmpC;_tmpC.tag=2;_tmpC.f1=Cyc_Absyn_exn_tud;_tmpC.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd;_tmpC;});_tmpB;});_tmpA->f2=1;_tmpA;}));}}{
# 263
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
return({struct Cyc_Tcenv_Tenv*_tmpD=_region_malloc(r,sizeof(*_tmpD));_tmpD->ns=0;_tmpD->ae=ae;_tmpD->le=0;_tmpD->allow_valueof=0;_tmpD->in_extern_c_include=0;_tmpD;});};};}
# 269
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 271
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 275
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0)
return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpE="outer_namespace";_tag_dyneither(_tmpE,sizeof(char),16);}));
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 281
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 288
if(n2 != 0)return 0;
return 1;}
# 294
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 299
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpF;_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){
lookup(ge2,v,0);
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp12[1]={& _tmp14};Cyc_Tcutil_terr(loc,({const char*_tmp13="%s is ambiguous";_tag_dyneither(_tmp13,sizeof(char),16);}),_tag_dyneither(_tmp12,sizeof(void*),1));});});
# 303
;_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp15=_tmp10;void*_tmp16;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 305
 goto _LL0;}else{_LL3: _tmp16=_tmp15;_LL4:(int)_rethrow(_tmp16);}_LL0:;}};}}
# 307
return;}
# 310
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 315
{struct _handler_cons _tmp17;_push_handler(& _tmp17);{int _tmp19=0;if(setjmp(_tmp17.handler))_tmp19=1;if(!_tmp19){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1A=result;_npop_handler(0);return _tmp1A;};}
# 316
;_pop_handler();}else{void*_tmp18=(void*)_exn_thrown;void*_tmp1B=_tmp18;void*_tmp1C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B)->tag == Cyc_Dict_Absent){_LL6: _LL7:
# 319
 goto _LL5;}else{_LL8: _tmp1C=_tmp1B;_LL9:(int)_rethrow(_tmp1C);}_LL5:;}};}
# 321
{struct Cyc_List_List*_tmp1D=ge->availables;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp1D->hd);
struct _handler_cons _tmp1E;_push_handler(& _tmp1E);{int _tmp20=0;if(setjmp(_tmp1E.handler))_tmp20=1;if(!_tmp20){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp1D->hd,_tmp1D->tl);{
void*_tmp21=result;_npop_handler(0);return _tmp21;};}
# 324
;_pop_handler();}else{void*_tmp1F=(void*)_exn_thrown;void*_tmp22=_tmp1F;void*_tmp23;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp22)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 327
 goto _LLA;}else{_LLD: _tmp23=_tmp22;_LLE:(int)_rethrow(_tmp23);}_LLA:;}};}}
# 329
if(cns == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp24=_cycalloc_atomic(sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Dict_Absent_exn_struct _tmp25;_tmp25.tag=Cyc_Dict_Absent;_tmp25;});_tmp24;}));
# 332
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 336
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp26=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp26,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 343
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple0*_tmp27=q;union Cyc_Absyn_Nmspace _tmp28;struct _dyneither_ptr*_tmp29;_LL10: _tmp28=_tmp27->f1;_tmp29=_tmp27->f2;_LL11:;{
union Cyc_Absyn_Nmspace _tmp2A=_tmp28;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2C;struct _dyneither_ptr*_tmp2D;struct Cyc_List_List*_tmp2E;switch((_tmp2A.C_n).tag){case 4: _LL13: _LL14:
 goto _LL16;case 1: if((_tmp2A.Rel_n).val == 0){_LL15: _LL16:
# 348
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp29,is_use);}else{_LL17: _tmp2D=(struct _dyneither_ptr*)((_tmp2A.Rel_n).val)->hd;_tmp2E=((_tmp2A.Rel_n).val)->tl;_LL18: {
# 350
struct Cyc_Tcenv_Genv*_tmp2F=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2D,_tmp2E);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2F,_tmp29,is_use);}}case 3: _LL19: _tmp2C=(_tmp2A.C_n).val;_LL1A:
 _tmp2B=_tmp2C;goto _LL1C;default: _LL1B: _tmp2B=(_tmp2A.Abs_n).val;_LL1C:
# 354
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2B),_tmp29,is_use);}_LL12:;};}
# 359
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 361
struct Cyc_List_List*_tmp30=te->ns;
struct _RegionHandle _tmp31=_new_region("temp");struct _RegionHandle*temp=& _tmp31;_push_region(temp);
{struct Cyc_List_List*_tmp32=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp33=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp30);
struct Cyc_List_List*_tmp34=_tmp33->availables;
struct Cyc_Set_Set*_tmp35=_tmp33->namespaces;
# 369
{struct Cyc_List_List*_tmp36=_tmp34;for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
struct Cyc_Set_Set*_tmp37=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp36->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp37,n))
_tmp32=({struct Cyc_List_List*_tmp38=_region_malloc(temp,sizeof(*_tmp38));_tmp38->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30,({struct Cyc_List_List*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->hd=n;_tmp39->tl=ns;_tmp39;}));_tmp38->tl=_tmp32;_tmp38;});}}
# 376
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp35,n))
_tmp32=({struct Cyc_List_List*_tmp3A=_region_malloc(temp,sizeof(*_tmp3A));_tmp3A->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30,({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->hd=n;_tmp3B->tl=ns;_tmp3B;}));_tmp3A->tl=_tmp32;_tmp3A;});
# 382
if(_tmp32 != 0){
if(_tmp32->tl != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n);({void*_tmp3C[1]={& _tmp3E};Cyc_Tcutil_terr(loc,({const char*_tmp3D="%s is ambiguous";_tag_dyneither(_tmp3D,sizeof(char),16);}),_tag_dyneither(_tmp3C,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp3F=(struct Cyc_List_List*)_tmp32->hd;_npop_handler(0);return _tmp3F;};}
# 387
if(_tmp30 == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp40=_cycalloc_atomic(sizeof(*_tmp40));_tmp40[0]=({struct Cyc_Dict_Absent_exn_struct _tmp41;_tmp41.tag=Cyc_Dict_Absent;_tmp41;});_tmp40;}));
# 390
_tmp30=Cyc_Tcenv_outer_namespace(_tmp30);}}
# 363
;_pop_region(temp);}
# 398
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp42=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp42,v);}
# 402
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp43=q;union Cyc_Absyn_Nmspace _tmp44;struct _dyneither_ptr*_tmp45;_LL1E: _tmp44=_tmp43->f1;_tmp45=_tmp43->f2;_LL1F:;{
union Cyc_Absyn_Nmspace _tmp46=_tmp44;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp48;struct _dyneither_ptr*_tmp49;struct Cyc_List_List*_tmp4A;switch((_tmp46.C_n).tag){case 4: _LL21: _LL22:
 goto _LL24;case 1: if((_tmp46.Rel_n).val == 0){_LL23: _LL24:
# 407
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp45,0);}else{_LL29: _tmp49=(struct _dyneither_ptr*)((_tmp46.Rel_n).val)->hd;_tmp4A=((_tmp46.Rel_n).val)->tl;_LL2A: {
# 413
struct Cyc_Dict_Dict _tmp4C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp49,_tmp4A))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp45);}}case 3: _LL25: _tmp48=(_tmp46.C_n).val;_LL26:
# 408
 _tmp47=_tmp48;goto _LL28;default: _LL27: _tmp47=(_tmp46.Abs_n).val;_LL28: {
# 410
struct Cyc_Dict_Dict _tmp4B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp47))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,_tmp45);}}_LL20:;};}
# 418
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4D=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4D,v);}
# 422
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp4E=q;union Cyc_Absyn_Nmspace _tmp4F;struct _dyneither_ptr*_tmp50;_LL2C: _tmp4F=_tmp4E->f1;_tmp50=_tmp4E->f2;_LL2D:;{
union Cyc_Absyn_Nmspace _tmp51=_tmp4F;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;struct _dyneither_ptr*_tmp54;struct Cyc_List_List*_tmp55;switch((_tmp51.C_n).tag){case 4: _LL2F: _LL30:
 goto _LL32;case 1: if((_tmp51.Rel_n).val == 0){_LL31: _LL32:
# 427
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp50,0);}else{_LL33: _tmp54=(struct _dyneither_ptr*)((_tmp51.Rel_n).val)->hd;_tmp55=((_tmp51.Rel_n).val)->tl;_LL34: {
# 429
struct Cyc_Dict_Dict _tmp56=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp54,_tmp55))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp56,_tmp50);}}case 3: _LL35: _tmp53=(_tmp51.C_n).val;_LL36:
 _tmp52=_tmp53;goto _LL38;default: _LL37: _tmp52=(_tmp51.Abs_n).val;_LL38: {
# 433
struct Cyc_Dict_Dict _tmp57=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp52))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp57,_tmp50);}}_LL2E:;};}
# 438
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 443
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp58=q;union Cyc_Absyn_Nmspace _tmp59;struct _dyneither_ptr*_tmp5A;_LL3A: _tmp59=_tmp58->f1;_tmp5A=_tmp58->f2;_LL3B:;{
union Cyc_Absyn_Nmspace _tmp5B=_tmp59;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct _dyneither_ptr*_tmp5E;struct Cyc_List_List*_tmp5F;switch((_tmp5B.C_n).tag){case 1: if((_tmp5B.Rel_n).val == 0){_LL3D: _LL3E: {
# 448
struct _handler_cons _tmp60;_push_handler(& _tmp60);{int _tmp62=0;if(setjmp(_tmp60.handler))_tmp62=1;if(!_tmp62){{struct Cyc_Absyn_Datatypedecl***_tmp64=({struct Cyc_Absyn_Datatypedecl***_tmp63=_region_malloc(r,sizeof(*_tmp63));_tmp63[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp5A,0);_tmp63;});_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp61=(void*)_exn_thrown;void*_tmp65=_tmp61;void*_tmp66;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp65)->tag == Cyc_Dict_Absent){_LL48: _LL49:
 return 0;}else{_LL4A: _tmp66=_tmp65;_LL4B:(int)_rethrow(_tmp66);}_LL47:;}};}}else{_LL41: _tmp5E=(struct _dyneither_ptr*)((_tmp5B.Rel_n).val)->hd;_tmp5F=((_tmp5B.Rel_n).val)->tl;_LL42: {
# 458
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp5E,_tmp5F);;_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp6C=_tmp6A;void*_tmp6D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6C)->tag == Cyc_Dict_Absent){_LL4D: _LL4E:
# 462
({void*_tmp6E=0;Cyc_Tcutil_terr(loc,({const char*_tmp6F="bad qualified name for @extensible datatype";_tag_dyneither(_tmp6F,sizeof(char),44);}),_tag_dyneither(_tmp6E,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp70=_cycalloc_atomic(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Dict_Absent_exn_struct _tmp71;_tmp71.tag=Cyc_Dict_Absent;_tmp71;});_tmp70;}));}else{_LL4F: _tmp6D=_tmp6C;_LL50:(int)_rethrow(_tmp6D);}_LL4C:;}};}{
# 465
struct Cyc_Dict_Dict _tmp72=ge->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp73=_region_malloc(r,sizeof(*_tmp73));_tmp73[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp72,_tmp5A);_tmp73;});};}}case 4: _LL3F: _LL40:
# 451
({void*_tmp67=0;Cyc_Tcutil_terr(loc,({const char*_tmp68="lookup_xdatatypedecl: impossible";_tag_dyneither(_tmp68,sizeof(char),33);}),_tag_dyneither(_tmp67,sizeof(void*),0));});
return 0;case 3: _LL43: _tmp5D=(_tmp5B.C_n).val;_LL44:
# 467
 _tmp5C=_tmp5D;goto _LL46;default: _LL45: _tmp5C=(_tmp5B.Abs_n).val;_LL46: {
# 469
struct Cyc_Dict_Dict _tmp74=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5C))->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp75=_region_malloc(r,sizeof(*_tmp75));_tmp75[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp74,_tmp5A);_tmp75;});}}_LL3C:;};}
# 474
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp76=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp76,v);}
# 478
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp77=q;union Cyc_Absyn_Nmspace _tmp78;struct _dyneither_ptr*_tmp79;_LL52: _tmp78=_tmp77->f1;_tmp79=_tmp77->f2;_LL53:;{
union Cyc_Absyn_Nmspace _tmp7A=_tmp78;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7C;struct _dyneither_ptr*_tmp7D;struct Cyc_List_List*_tmp7E;switch((_tmp7A.C_n).tag){case 4: _LL55: _LL56:
 goto _LL58;case 1: if((_tmp7A.Rel_n).val == 0){_LL57: _LL58:
# 483
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp79,0);}else{_LL59: _tmp7D=(struct _dyneither_ptr*)((_tmp7A.Rel_n).val)->hd;_tmp7E=((_tmp7A.Rel_n).val)->tl;_LL5A: {
# 485
struct Cyc_Dict_Dict _tmp7F=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp7D,_tmp7E))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F,_tmp79);}}case 3: _LL5B: _tmp7C=(_tmp7A.C_n).val;_LL5C:
 _tmp7B=_tmp7C;goto _LL5E;default: _LL5D: _tmp7B=(_tmp7A.Abs_n).val;_LL5E: {
# 489
struct Cyc_Dict_Dict _tmp80=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp7B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp80,_tmp79);}}_LL54:;};}
# 494
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp81=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,v);}
# 498
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp82=q;union Cyc_Absyn_Nmspace _tmp83;struct _dyneither_ptr*_tmp84;_LL60: _tmp83=_tmp82->f1;_tmp84=_tmp82->f2;_LL61:;{
union Cyc_Absyn_Nmspace _tmp85=_tmp83;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct _dyneither_ptr*_tmp88;struct Cyc_List_List*_tmp89;switch((_tmp85.C_n).tag){case 4: _LL63: _LL64:
 goto _LL66;case 1: if((_tmp85.Rel_n).val == 0){_LL65: _LL66:
# 503
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp84,0);}else{_LL67: _tmp88=(struct _dyneither_ptr*)((_tmp85.Rel_n).val)->hd;_tmp89=((_tmp85.Rel_n).val)->tl;_LL68: {
# 505
struct Cyc_Dict_Dict _tmp8A=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp88,_tmp89))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8A,_tmp84);}}case 3: _LL69: _tmp87=(_tmp85.C_n).val;_LL6A:
 _tmp86=_tmp87;goto _LL6C;default: _LL6B: _tmp86=(_tmp85.Abs_n).val;_LL6C: {
# 509
struct Cyc_Dict_Dict _tmp8B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp86))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,_tmp84);}}_LL62:;};}
# 516
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp8C=te->le;
if(_tmp8C == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp8C;}
# 522
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 526
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp8D=te->le;
if(_tmp8D != 0){
struct Cyc_Tcenv_Fenv*_tmp8E=_tmp8D;struct Cyc_Tcenv_Fenv*_tmp8F=_tmp8E;struct _RegionHandle*_tmp90;_LL6E: _tmp90=_tmp8F->fnrgn;_LL6F:;
return _tmp90;}
# 532
return Cyc_Tcenv_coerce_heap_region();}
# 535
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp91="put_fenv";_tag_dyneither(_tmp91,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp92=_region_malloc(l,sizeof(*_tmp92));_tmp92->ns=te->ns;_tmp92->ae=te->ae;_tmp92->le=fe;_tmp92->allow_valueof=te->allow_valueof;_tmp92->in_extern_c_include=te->in_extern_c_include;_tmp92;});}
# 540
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp93=_region_malloc(l,sizeof(*_tmp93));_tmp93->ns=te->ns;_tmp93->ae=te->ae;_tmp93->le=0;_tmp93->allow_valueof=te->allow_valueof;_tmp93->in_extern_c_include=te->in_extern_c_include;_tmp93;});}
# 544
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 546
struct Cyc_Tcenv_Fenv _tmp94=*f;struct Cyc_Tcenv_Fenv _tmp95=_tmp94;struct Cyc_Tcenv_SharedFenv*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_RgnOrder_RgnPO*_tmp98;const struct Cyc_Tcenv_Bindings*_tmp99;struct Cyc_Absyn_Stmt*_tmp9A;struct Cyc_Tcenv_CtrlEnv*_tmp9B;void*_tmp9C;void*_tmp9D;struct Cyc_Tcenv_FenvFlags _tmp9E;struct _RegionHandle*_tmp9F;_LL71: _tmp96=_tmp95.shared;_tmp97=_tmp95.type_vars;_tmp98=_tmp95.region_order;_tmp99=_tmp95.locals;_tmp9A=_tmp95.encloser;_tmp9B=_tmp95.ctrl_env;_tmp9C=_tmp95.capability;_tmp9D=_tmp95.curr_rgn;_tmp9E=_tmp95.flags;_tmp9F=_tmp95.fnrgn;_LL72:;
# 548
return({struct Cyc_Tcenv_Fenv*_tmpA0=_region_malloc(l,sizeof(*_tmpA0));_tmpA0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp96;_tmpA0->type_vars=(struct Cyc_List_List*)_tmp97;_tmpA0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp98;_tmpA0->locals=(const struct Cyc_Tcenv_Bindings*)_tmp99;_tmpA0->encloser=(struct Cyc_Absyn_Stmt*)_tmp9A;_tmpA0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp9B;_tmpA0->capability=(void*)_tmp9C;_tmpA0->curr_rgn=(void*)_tmp9D;_tmpA0->flags=(struct Cyc_Tcenv_FenvFlags)_tmp9E;_tmpA0->fnrgn=(struct _RegionHandle*)l;_tmpA0;});}
# 552
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 554
struct Cyc_Tcenv_Fenv _tmpA1=*f;struct Cyc_Tcenv_Fenv _tmpA2=_tmpA1;struct Cyc_Tcenv_SharedFenv*_tmpA3;struct Cyc_List_List*_tmpA4;struct Cyc_RgnOrder_RgnPO*_tmpA5;const struct Cyc_Tcenv_Bindings*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA7;struct Cyc_Tcenv_CtrlEnv*_tmpA8;void*_tmpA9;void*_tmpAA;struct Cyc_Tcenv_FenvFlags _tmpAB;_LL74: _tmpA3=_tmpA2.shared;_tmpA4=_tmpA2.type_vars;_tmpA5=_tmpA2.region_order;_tmpA6=_tmpA2.locals;_tmpA7=_tmpA2.encloser;_tmpA8=_tmpA2.ctrl_env;_tmpA9=_tmpA2.capability;_tmpAA=_tmpA2.curr_rgn;_tmpAB=_tmpA2.flags;_LL75:;{
# 557
struct Cyc_Tcenv_CtrlEnv _tmpAC=*_tmpA8;struct Cyc_Tcenv_CtrlEnv _tmpAD=_tmpAC;struct _RegionHandle*_tmpAE;void*_tmpAF;void*_tmpB0;const struct _tuple10*_tmpB1;void*_tmpB2;_LL77: _tmpAE=_tmpAD.ctrl_rgn;_tmpAF=_tmpAD.continue_stmt;_tmpB0=_tmpAD.break_stmt;_tmpB1=_tmpAD.fallthru_clause;_tmpB2=_tmpAD.next_stmt;_LL78:;{
struct Cyc_Tcenv_CtrlEnv*_tmpB3=({struct Cyc_Tcenv_CtrlEnv*_tmpB5=_region_malloc(l,sizeof(*_tmpB5));_tmpB5->ctrl_rgn=_tmpAE;_tmpB5->continue_stmt=_tmpAF;_tmpB5->break_stmt=_tmpB0;_tmpB5->fallthru_clause=_tmpB1;_tmpB5->next_stmt=_tmpB2;_tmpB5;});
# 560
return({struct Cyc_Tcenv_Fenv*_tmpB4=_region_malloc(l,sizeof(*_tmpB4));_tmpB4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA3;_tmpB4->type_vars=(struct Cyc_List_List*)_tmpA4;_tmpB4->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA5;_tmpB4->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA6;_tmpB4->encloser=(struct Cyc_Absyn_Stmt*)_tmpA7;_tmpB4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB3;_tmpB4->capability=(void*)_tmpA9;_tmpB4->curr_rgn=(void*)_tmpAA;_tmpB4->flags=(struct Cyc_Tcenv_FenvFlags)_tmpAB;_tmpB4->fnrgn=(struct _RegionHandle*)l;_tmpB4;});};};}
# 566
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpB6=Cyc_Tcenv_get_fenv(te,({const char*_tmpBC="return_typ";_tag_dyneither(_tmpBC,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmpB7=_tmpB6;struct Cyc_Tcenv_SharedFenv*_tmpB8;_LL7A: _tmpB8=_tmpB7->shared;_LL7B:;{
struct Cyc_Tcenv_SharedFenv _tmpB9=*_tmpB8;struct Cyc_Tcenv_SharedFenv _tmpBA=_tmpB9;void*_tmpBB;_LL7D: _tmpBB=_tmpBA.return_typ;_LL7E:;
return _tmpBB;};}
# 572
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpBD=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpBE=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpBD));struct Cyc_Tcenv_Fenv _tmpBF=_tmpBE;struct Cyc_List_List*_tmpC0;_LL80: _tmpC0=_tmpBF.type_vars;_LL81:;
return _tmpC0;};}
# 579
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC1;});}
# 583
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 585
struct Cyc_Tcenv_Fenv*_tmpC2=
# 587
Cyc_Tcenv_get_fenv(te,({const char*_tmpD0="add_type_vars";_tag_dyneither(_tmpD0,sizeof(char),14);}));
# 585
struct Cyc_Tcenv_Fenv*_tmpC3=_tmpC2;struct Cyc_Tcenv_SharedFenv*_tmpC4;struct Cyc_List_List*_tmpC5;struct Cyc_RgnOrder_RgnPO*_tmpC6;const struct Cyc_Tcenv_Bindings*_tmpC7;struct Cyc_Absyn_Stmt*_tmpC8;struct Cyc_Tcenv_CtrlEnv*_tmpC9;void*_tmpCA;void*_tmpCB;struct Cyc_Tcenv_FenvFlags _tmpCC;_LL83: _tmpC4=_tmpC3->shared;_tmpC5=_tmpC3->type_vars;_tmpC6=_tmpC3->region_order;_tmpC7=_tmpC3->locals;_tmpC8=_tmpC3->encloser;_tmpC9=_tmpC3->ctrl_env;_tmpCA=_tmpC3->capability;_tmpCB=_tmpC3->curr_rgn;_tmpCC=_tmpC3->flags;_LL84:;
# 589
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpCD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC5);
Cyc_Tcutil_check_unique_tvars(loc,_tmpCD);{
struct Cyc_Tcenv_Fenv*_tmpCE=({struct Cyc_Tcenv_Fenv*_tmpCF=_region_malloc(r,sizeof(*_tmpCF));_tmpCF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC4;_tmpCF->type_vars=(struct Cyc_List_List*)_tmpCD;_tmpCF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC6;_tmpCF->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC7;_tmpCF->encloser=(struct Cyc_Absyn_Stmt*)_tmpC8;_tmpCF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpC9;_tmpCF->capability=(void*)_tmpCA;_tmpCF->curr_rgn=(void*)_tmpCB;_tmpCF->flags=(struct Cyc_Tcenv_FenvFlags)_tmpCC;_tmpCF->fnrgn=(struct _RegionHandle*)r;_tmpCF;});
# 594
return Cyc_Tcenv_put_fenv(r,te,_tmpCE);};};}
# 597
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD1=te->le;
if(_tmpD1 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 602
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpD1);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 607
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 609
struct _dyneither_ptr*_tmpD2=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpD3=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpD4=_tmpD3;if((_tmpD4.Loc_n).tag == 4){_LL86: _LL87:
 goto _LL85;}else{_LL88: _LL89:
# 614
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Core_Impossible_exn_struct _tmpD6;_tmpD6.tag=Cyc_Core_Impossible;_tmpD6.f1=({const char*_tmpD7="add_local_var: called with Rel_n";_tag_dyneither(_tmpD7,sizeof(char),33);});_tmpD6;});_tmpD5;}));}_LL85:;}
# 616
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int _tmpDA=0;if(setjmp(_tmpD8.handler))_tmpDA=1;if(!_tmpDA){
{void*_tmpDB=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpDC=_tmpDB;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDC)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDC)->f1)){case 4: _LL8B: _LL8C:
# 620
({void*_tmpDD=0;Cyc_Tcutil_warn(loc,({const char*_tmpDE="declaration hides local";_tag_dyneither(_tmpDE,sizeof(char),24);}),_tag_dyneither(_tmpDD,sizeof(void*),0));});goto _LL8A;case 1: _LL8D: _LL8E:
({void*_tmpDF=0;Cyc_Tcutil_warn(loc,({const char*_tmpE0="declaration hides global";_tag_dyneither(_tmpE0,sizeof(char),25);}),_tag_dyneither(_tmpDF,sizeof(void*),0));});goto _LL8A;case 5: _LL8F: _LL90:
({void*_tmpE1=0;Cyc_Tcutil_warn(loc,({const char*_tmpE2="declaration hides pattern variable";_tag_dyneither(_tmpE2,sizeof(char),35);}),_tag_dyneither(_tmpE1,sizeof(void*),0));});goto _LL8A;case 3: _LL91: _LL92:
({void*_tmpE3=0;Cyc_Tcutil_warn(loc,({const char*_tmpE4="declaration hides parameter";_tag_dyneither(_tmpE4,sizeof(char),28);}),_tag_dyneither(_tmpE3,sizeof(void*),0));});goto _LL8A;default: goto _LL93;}else{_LL93: _LL94:
 goto _LL8A;}_LL8A:;}
# 618
;_pop_handler();}else{void*_tmpD9=(void*)_exn_thrown;void*_tmpE5=_tmpD9;void*_tmpE6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpE5)->tag == Cyc_Dict_Absent){_LL96: _LL97:
# 626
 goto _LL95;}else{_LL98: _tmpE6=_tmpE5;_LL99:(int)_rethrow(_tmpE6);}_LL95:;}};}{
struct Cyc_Tcenv_Fenv*_tmpE7=
# 629
Cyc_Tcenv_get_fenv(te,({const char*_tmpF9="add_local_var";_tag_dyneither(_tmpF9,sizeof(char),14);}));
# 627
struct Cyc_Tcenv_Fenv*_tmpE8=_tmpE7;struct Cyc_Tcenv_SharedFenv*_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_RgnOrder_RgnPO*_tmpEB;const struct Cyc_Tcenv_Bindings*_tmpEC;struct Cyc_Absyn_Stmt*_tmpED;struct Cyc_Tcenv_CtrlEnv*_tmpEE;void*_tmpEF;void*_tmpF0;struct Cyc_Tcenv_FenvFlags _tmpF1;_LL9B: _tmpE9=_tmpE8->shared;_tmpEA=_tmpE8->type_vars;_tmpEB=_tmpE8->region_order;_tmpEC=_tmpE8->locals;_tmpED=_tmpE8->encloser;_tmpEE=_tmpE8->ctrl_env;_tmpEF=_tmpE8->capability;_tmpF0=_tmpE8->curr_rgn;_tmpF1=_tmpE8->flags;_LL9C:;{
# 630
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF2=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpF8;_tmpF8.tag=4;_tmpF8.f1=vd;_tmpF8;});_tmpF7;});
struct Cyc_Tcenv_Bindings*_tmpF3=({struct Cyc_Tcenv_Bindings*_tmpF6=_region_malloc(r,sizeof(*_tmpF6));_tmpF6->v=_tmpD2;_tmpF6->b=(void*)_tmpF2;_tmpF6->tl=_tmpEC;_tmpF6;});
struct Cyc_Tcenv_Fenv*_tmpF4=({struct Cyc_Tcenv_Fenv*_tmpF5=_region_malloc(r,sizeof(*_tmpF5));_tmpF5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpE9;_tmpF5->type_vars=(struct Cyc_List_List*)_tmpEA;_tmpF5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEB;_tmpF5->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpF3);_tmpF5->encloser=(struct Cyc_Absyn_Stmt*)_tmpED;_tmpF5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpEE;_tmpF5->capability=(void*)_tmpEF;_tmpF5->curr_rgn=(void*)_tmpF0;_tmpF5->flags=(struct Cyc_Tcenv_FenvFlags)_tmpF1;_tmpF5->fnrgn=(struct _RegionHandle*)r;_tmpF5;});
# 634
return Cyc_Tcenv_put_fenv(r,te,_tmpF4);};};}
# 637
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 639
struct Cyc_Tcenv_Fenv*_tmpFA=te->le;
if(_tmpFA == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpFB=*_tmpFA;struct Cyc_Tcenv_Fenv _tmpFC=_tmpFB;struct Cyc_Tcenv_SharedFenv*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_RgnOrder_RgnPO*_tmpFF;const struct Cyc_Tcenv_Bindings*_tmp100;struct Cyc_Absyn_Stmt*_tmp101;struct Cyc_Tcenv_CtrlEnv*_tmp102;void*_tmp103;void*_tmp104;int _tmp105;int _tmp106;int _tmp107;_LL9E: _tmpFD=_tmpFC.shared;_tmpFE=_tmpFC.type_vars;_tmpFF=_tmpFC.region_order;_tmp100=_tmpFC.locals;_tmp101=_tmpFC.encloser;_tmp102=_tmpFC.ctrl_env;_tmp103=_tmpFC.capability;_tmp104=_tmpFC.curr_rgn;_tmp105=(_tmpFC.flags).in_notreadctxt;_tmp106=(_tmpFC.flags).in_lhs;_tmp107=(_tmpFC.flags).abstract_ok;_LL9F:;{
# 644
struct Cyc_Tcenv_Fenv*_tmp108=({struct Cyc_Tcenv_Fenv*_tmp109=_region_malloc(r,sizeof(*_tmp109));_tmp109->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFD;_tmp109->type_vars=(struct Cyc_List_List*)_tmpFE;_tmp109->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFF;_tmp109->locals=(const struct Cyc_Tcenv_Bindings*)_tmp100;_tmp109->encloser=(struct Cyc_Absyn_Stmt*)_tmp101;_tmp109->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp102;_tmp109->capability=(void*)_tmp103;_tmp109->curr_rgn=(void*)_tmp104;_tmp109->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp109->flags).in_notreadctxt=_tmp105;(_tmp109->flags).in_lhs=_tmp106;(_tmp109->flags).in_new=(int)status;(_tmp109->flags).abstract_ok=_tmp107;_tmp109->flags;});_tmp109->fnrgn=(struct _RegionHandle*)r;_tmp109;});
# 647
return Cyc_Tcenv_put_fenv(r,te,_tmp108);};};}
# 650
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10A=te->le;
if(_tmp10A == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp10B=_tmp10A;struct Cyc_Tcenv_Fenv*_tmp10C=_tmp10B;int _tmp10D;_LLA1: _tmp10D=(_tmp10C->flags).in_new;_LLA2:;
return(enum Cyc_Tcenv_NewStatus)_tmp10D;};}
# 657
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10E=te->le;
if(_tmp10E == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp10F=_tmp10E;struct Cyc_Tcenv_Fenv*_tmp110=_tmp10F;int _tmp111;_LLA4: _tmp111=(_tmp110->flags).abstract_ok;_LLA5:;
return _tmp111;};}
# 664
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 666
struct Cyc_Tcenv_Fenv*_tmp112=te->le;
if(_tmp112 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp113=*_tmp112;struct Cyc_Tcenv_Fenv _tmp114=_tmp113;struct Cyc_Tcenv_SharedFenv*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_RgnOrder_RgnPO*_tmp117;const struct Cyc_Tcenv_Bindings*_tmp118;struct Cyc_Absyn_Stmt*_tmp119;struct Cyc_Tcenv_CtrlEnv*_tmp11A;void*_tmp11B;void*_tmp11C;int _tmp11D;int _tmp11E;int _tmp11F;_LLA7: _tmp115=_tmp114.shared;_tmp116=_tmp114.type_vars;_tmp117=_tmp114.region_order;_tmp118=_tmp114.locals;_tmp119=_tmp114.encloser;_tmp11A=_tmp114.ctrl_env;_tmp11B=_tmp114.capability;_tmp11C=_tmp114.curr_rgn;_tmp11D=(_tmp114.flags).in_notreadctxt;_tmp11E=(_tmp114.flags).in_lhs;_tmp11F=(_tmp114.flags).in_new;_LLA8:;{
# 671
struct Cyc_Tcenv_Fenv*_tmp120=({struct Cyc_Tcenv_Fenv*_tmp121=_region_malloc(r,sizeof(*_tmp121));_tmp121->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp115;_tmp121->type_vars=(struct Cyc_List_List*)_tmp116;_tmp121->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp117;_tmp121->locals=(const struct Cyc_Tcenv_Bindings*)_tmp118;_tmp121->encloser=(struct Cyc_Absyn_Stmt*)_tmp119;_tmp121->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp11A;_tmp121->capability=(void*)_tmp11B;_tmp121->curr_rgn=(void*)_tmp11C;_tmp121->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp121->flags).in_notreadctxt=_tmp11D;(_tmp121->flags).in_lhs=_tmp11E;(_tmp121->flags).in_new=_tmp11F;(_tmp121->flags).abstract_ok=1;_tmp121->flags;});_tmp121->fnrgn=(struct _RegionHandle*)r;_tmp121;});
# 674
return Cyc_Tcenv_put_fenv(r,te,_tmp120);};};}
# 677
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 679
struct Cyc_Tcenv_Fenv*_tmp122=te->le;
if(_tmp122 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp123=*_tmp122;struct Cyc_Tcenv_Fenv _tmp124=_tmp123;struct Cyc_Tcenv_SharedFenv*_tmp125;struct Cyc_List_List*_tmp126;struct Cyc_RgnOrder_RgnPO*_tmp127;const struct Cyc_Tcenv_Bindings*_tmp128;struct Cyc_Absyn_Stmt*_tmp129;struct Cyc_Tcenv_CtrlEnv*_tmp12A;void*_tmp12B;void*_tmp12C;int _tmp12D;int _tmp12E;int _tmp12F;_LLAA: _tmp125=_tmp124.shared;_tmp126=_tmp124.type_vars;_tmp127=_tmp124.region_order;_tmp128=_tmp124.locals;_tmp129=_tmp124.encloser;_tmp12A=_tmp124.ctrl_env;_tmp12B=_tmp124.capability;_tmp12C=_tmp124.curr_rgn;_tmp12D=(_tmp124.flags).in_notreadctxt;_tmp12E=(_tmp124.flags).in_lhs;_tmp12F=(_tmp124.flags).in_new;_LLAB:;{
# 684
struct Cyc_Tcenv_Fenv*_tmp130=({struct Cyc_Tcenv_Fenv*_tmp131=_region_malloc(r,sizeof(*_tmp131));_tmp131->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp125;_tmp131->type_vars=(struct Cyc_List_List*)_tmp126;_tmp131->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp127;_tmp131->locals=(const struct Cyc_Tcenv_Bindings*)_tmp128;_tmp131->encloser=(struct Cyc_Absyn_Stmt*)_tmp129;_tmp131->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12A;_tmp131->capability=(void*)_tmp12B;_tmp131->curr_rgn=(void*)_tmp12C;_tmp131->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp131->flags).in_notreadctxt=_tmp12D;(_tmp131->flags).in_lhs=_tmp12E;(_tmp131->flags).in_new=_tmp12F;(_tmp131->flags).abstract_ok=0;_tmp131->flags;});_tmp131->fnrgn=(struct _RegionHandle*)r;_tmp131;});
# 687
return Cyc_Tcenv_put_fenv(r,te,_tmp130);};};}
# 690
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp132=te->le;
if(_tmp132 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp133=*_tmp132;struct Cyc_Tcenv_Fenv _tmp134=_tmp133;struct Cyc_Tcenv_SharedFenv*_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_RgnOrder_RgnPO*_tmp137;const struct Cyc_Tcenv_Bindings*_tmp138;struct Cyc_Absyn_Stmt*_tmp139;struct Cyc_Tcenv_CtrlEnv*_tmp13A;void*_tmp13B;void*_tmp13C;int _tmp13D;int _tmp13E;int _tmp13F;_LLAD: _tmp135=_tmp134.shared;_tmp136=_tmp134.type_vars;_tmp137=_tmp134.region_order;_tmp138=_tmp134.locals;_tmp139=_tmp134.encloser;_tmp13A=_tmp134.ctrl_env;_tmp13B=_tmp134.capability;_tmp13C=_tmp134.curr_rgn;_tmp13D=(_tmp134.flags).in_lhs;_tmp13E=(_tmp134.flags).in_new;_tmp13F=(_tmp134.flags).abstract_ok;_LLAE:;{
# 696
struct Cyc_Tcenv_Fenv*_tmp140=({struct Cyc_Tcenv_Fenv*_tmp141=_region_malloc(r,sizeof(*_tmp141));_tmp141->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp135;_tmp141->type_vars=(struct Cyc_List_List*)_tmp136;_tmp141->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp137;_tmp141->locals=(const struct Cyc_Tcenv_Bindings*)_tmp138;_tmp141->encloser=(struct Cyc_Absyn_Stmt*)_tmp139;_tmp141->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13A;_tmp141->capability=(void*)_tmp13B;_tmp141->curr_rgn=(void*)_tmp13C;_tmp141->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp141->flags).in_notreadctxt=1;(_tmp141->flags).in_lhs=_tmp13D;(_tmp141->flags).in_new=_tmp13E;(_tmp141->flags).abstract_ok=_tmp13F;_tmp141->flags;});_tmp141->fnrgn=(struct _RegionHandle*)r;_tmp141;});
# 699
return Cyc_Tcenv_put_fenv(r,te,_tmp140);};};}
# 702
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp142=te->le;
if(_tmp142 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp143=*_tmp142;struct Cyc_Tcenv_Fenv _tmp144=_tmp143;struct Cyc_Tcenv_SharedFenv*_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_RgnOrder_RgnPO*_tmp147;const struct Cyc_Tcenv_Bindings*_tmp148;struct Cyc_Absyn_Stmt*_tmp149;struct Cyc_Tcenv_CtrlEnv*_tmp14A;void*_tmp14B;void*_tmp14C;int _tmp14D;int _tmp14E;int _tmp14F;_LLB0: _tmp145=_tmp144.shared;_tmp146=_tmp144.type_vars;_tmp147=_tmp144.region_order;_tmp148=_tmp144.locals;_tmp149=_tmp144.encloser;_tmp14A=_tmp144.ctrl_env;_tmp14B=_tmp144.capability;_tmp14C=_tmp144.curr_rgn;_tmp14D=(_tmp144.flags).in_lhs;_tmp14E=(_tmp144.flags).in_new;_tmp14F=(_tmp144.flags).abstract_ok;_LLB1:;{
# 708
struct Cyc_Tcenv_Fenv*_tmp150=({struct Cyc_Tcenv_Fenv*_tmp151=_region_malloc(r,sizeof(*_tmp151));_tmp151->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp145;_tmp151->type_vars=(struct Cyc_List_List*)_tmp146;_tmp151->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp147;_tmp151->locals=(const struct Cyc_Tcenv_Bindings*)_tmp148;_tmp151->encloser=(struct Cyc_Absyn_Stmt*)_tmp149;_tmp151->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14A;_tmp151->capability=(void*)_tmp14B;_tmp151->curr_rgn=(void*)_tmp14C;_tmp151->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp151->flags).in_notreadctxt=0;(_tmp151->flags).in_lhs=_tmp14D;(_tmp151->flags).in_new=_tmp14E;(_tmp151->flags).abstract_ok=_tmp14F;_tmp151->flags;});_tmp151->fnrgn=(struct _RegionHandle*)r;_tmp151;});
# 711
return Cyc_Tcenv_put_fenv(r,te,_tmp150);};};}
# 714
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp152=te->le;
if(_tmp152 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp153=_tmp152;struct Cyc_Tcenv_Fenv*_tmp154=_tmp153;int _tmp155;_LLB3: _tmp155=(_tmp154->flags).in_notreadctxt;_LLB4:;
return _tmp155;};}
# 721
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp156=te->le;
if(_tmp156 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp157=*_tmp156;struct Cyc_Tcenv_Fenv _tmp158=_tmp157;struct Cyc_Tcenv_SharedFenv*_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_RgnOrder_RgnPO*_tmp15B;const struct Cyc_Tcenv_Bindings*_tmp15C;struct Cyc_Absyn_Stmt*_tmp15D;struct Cyc_Tcenv_CtrlEnv*_tmp15E;void*_tmp15F;void*_tmp160;int _tmp161;int _tmp162;int _tmp163;_LLB6: _tmp159=_tmp158.shared;_tmp15A=_tmp158.type_vars;_tmp15B=_tmp158.region_order;_tmp15C=_tmp158.locals;_tmp15D=_tmp158.encloser;_tmp15E=_tmp158.ctrl_env;_tmp15F=_tmp158.capability;_tmp160=_tmp158.curr_rgn;_tmp161=(_tmp158.flags).in_notreadctxt;_tmp162=(_tmp158.flags).in_new;_tmp163=(_tmp158.flags).abstract_ok;_LLB7:;{
# 727
struct Cyc_Tcenv_Fenv*_tmp164=({struct Cyc_Tcenv_Fenv*_tmp165=_region_malloc(r,sizeof(*_tmp165));_tmp165->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp159;_tmp165->type_vars=(struct Cyc_List_List*)_tmp15A;_tmp165->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15B;_tmp165->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15C;_tmp165->encloser=(struct Cyc_Absyn_Stmt*)_tmp15D;_tmp165->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15E;_tmp165->capability=(void*)_tmp15F;_tmp165->curr_rgn=(void*)_tmp160;_tmp165->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp165->flags).in_notreadctxt=_tmp161;(_tmp165->flags).in_lhs=1;(_tmp165->flags).in_new=_tmp162;(_tmp165->flags).abstract_ok=_tmp163;_tmp165->flags;});_tmp165->fnrgn=(struct _RegionHandle*)r;_tmp165;});
# 730
return Cyc_Tcenv_put_fenv(r,te,_tmp164);};};}
# 733
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp166=te->le;
if(_tmp166 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp167=*_tmp166;struct Cyc_Tcenv_Fenv _tmp168=_tmp167;struct Cyc_Tcenv_SharedFenv*_tmp169;struct Cyc_List_List*_tmp16A;struct Cyc_RgnOrder_RgnPO*_tmp16B;const struct Cyc_Tcenv_Bindings*_tmp16C;struct Cyc_Absyn_Stmt*_tmp16D;struct Cyc_Tcenv_CtrlEnv*_tmp16E;void*_tmp16F;void*_tmp170;int _tmp171;int _tmp172;int _tmp173;_LLB9: _tmp169=_tmp168.shared;_tmp16A=_tmp168.type_vars;_tmp16B=_tmp168.region_order;_tmp16C=_tmp168.locals;_tmp16D=_tmp168.encloser;_tmp16E=_tmp168.ctrl_env;_tmp16F=_tmp168.capability;_tmp170=_tmp168.curr_rgn;_tmp171=(_tmp168.flags).in_notreadctxt;_tmp172=(_tmp168.flags).in_new;_tmp173=(_tmp168.flags).abstract_ok;_LLBA:;{
# 739
struct Cyc_Tcenv_Fenv*_tmp174=({struct Cyc_Tcenv_Fenv*_tmp175=_region_malloc(r,sizeof(*_tmp175));_tmp175->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp169;_tmp175->type_vars=(struct Cyc_List_List*)_tmp16A;_tmp175->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp16B;_tmp175->locals=(const struct Cyc_Tcenv_Bindings*)_tmp16C;_tmp175->encloser=(struct Cyc_Absyn_Stmt*)_tmp16D;_tmp175->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16E;_tmp175->capability=(void*)_tmp16F;_tmp175->curr_rgn=(void*)_tmp170;_tmp175->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp175->flags).in_notreadctxt=_tmp171;(_tmp175->flags).in_lhs=0;(_tmp175->flags).in_new=_tmp172;(_tmp175->flags).abstract_ok=_tmp173;_tmp175->flags;});_tmp175->fnrgn=(struct _RegionHandle*)r;_tmp175;});
# 742
return Cyc_Tcenv_put_fenv(r,te,_tmp174);};};}
# 745
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp176=te->le;
if(_tmp176 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp177=_tmp176;struct Cyc_Tcenv_Fenv*_tmp178=_tmp177;int _tmp179;_LLBC: _tmp179=(_tmp178->flags).in_lhs;_LLBD:;
return _tmp179;};}
# 753
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 755
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp17A;_push_handler(& _tmp17A);{int _tmp17C=0;if(setjmp(_tmp17A.handler))_tmp17C=1;if(!_tmp17C){
{void*_tmp17D=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp17E=_tmp17D;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17E)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17E)->f1)){case 4: _LLBF: _LLC0:
# 759
({void*_tmp17F=0;Cyc_Tcutil_warn(loc,({const char*_tmp180="pattern variable hides local";_tag_dyneither(_tmp180,sizeof(char),29);}),_tag_dyneither(_tmp17F,sizeof(void*),0));});goto _LLBE;case 1: _LLC1: _LLC2:
({void*_tmp181=0;Cyc_Tcutil_warn(loc,({const char*_tmp182="pattern variable hides global";_tag_dyneither(_tmp182,sizeof(char),30);}),_tag_dyneither(_tmp181,sizeof(void*),0));});goto _LLBE;case 5: _LLC3: _LLC4:
({void*_tmp183=0;Cyc_Tcutil_warn(loc,({const char*_tmp184="pattern variable hides pattern variable";_tag_dyneither(_tmp184,sizeof(char),40);}),_tag_dyneither(_tmp183,sizeof(void*),0));});goto _LLBE;case 3: _LLC5: _LLC6:
({void*_tmp185=0;Cyc_Tcutil_warn(loc,({const char*_tmp186="pattern variable hides parameter";_tag_dyneither(_tmp186,sizeof(char),33);}),_tag_dyneither(_tmp185,sizeof(void*),0));});goto _LLBE;default: goto _LLC7;}else{_LLC7: _LLC8:
 goto _LLBE;}_LLBE:;}
# 757
;_pop_handler();}else{void*_tmp17B=(void*)_exn_thrown;void*_tmp187=_tmp17B;void*_tmp188;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp187)->tag == Cyc_Dict_Absent){_LLCA: _LLCB:
# 765
 goto _LLC9;}else{_LLCC: _tmp188=_tmp187;_LLCD:(int)_rethrow(_tmp188);}_LLC9:;}};}{
struct _dyneither_ptr*_tmp189=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmp18A=
# 769
Cyc_Tcenv_get_fenv(te,({const char*_tmp19C="add_pat_var";_tag_dyneither(_tmp19C,sizeof(char),12);}));
# 767
struct Cyc_Tcenv_Fenv*_tmp18B=_tmp18A;struct Cyc_Tcenv_SharedFenv*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_RgnOrder_RgnPO*_tmp18E;const struct Cyc_Tcenv_Bindings*_tmp18F;struct Cyc_Absyn_Stmt*_tmp190;struct Cyc_Tcenv_CtrlEnv*_tmp191;void*_tmp192;void*_tmp193;struct Cyc_Tcenv_FenvFlags _tmp194;_LLCF: _tmp18C=_tmp18B->shared;_tmp18D=_tmp18B->type_vars;_tmp18E=_tmp18B->region_order;_tmp18F=_tmp18B->locals;_tmp190=_tmp18B->encloser;_tmp191=_tmp18B->ctrl_env;_tmp192=_tmp18B->capability;_tmp193=_tmp18B->curr_rgn;_tmp194=_tmp18B->flags;_LLD0:;{
# 770
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp195=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp19B;_tmp19B.tag=5;_tmp19B.f1=vd;_tmp19B;});_tmp19A;});
struct Cyc_Tcenv_Bindings*_tmp196=({struct Cyc_Tcenv_Bindings*_tmp199=_region_malloc(r,sizeof(*_tmp199));_tmp199->v=_tmp189;_tmp199->b=(void*)_tmp195;_tmp199->tl=_tmp18F;_tmp199;});
struct Cyc_Tcenv_Fenv*_tmp197=({struct Cyc_Tcenv_Fenv*_tmp198=_region_malloc(r,sizeof(*_tmp198));_tmp198->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp18C;_tmp198->type_vars=(struct Cyc_List_List*)_tmp18D;_tmp198->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp18E;_tmp198->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp196);_tmp198->encloser=(struct Cyc_Absyn_Stmt*)_tmp190;_tmp198->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp191;_tmp198->capability=(void*)_tmp192;_tmp198->curr_rgn=(void*)_tmp193;_tmp198->flags=(struct Cyc_Tcenv_FenvFlags)_tmp194;_tmp198->fnrgn=(struct _RegionHandle*)r;_tmp198;});
# 774
return Cyc_Tcenv_put_fenv(r,te,_tmp197);};};}
# 777
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp19D=te->le;
struct _tuple0*_tmp19E=q;union Cyc_Absyn_Nmspace _tmp19F;struct _dyneither_ptr*_tmp1A0;_LLD2: _tmp19F=_tmp19E->f1;_tmp1A0=_tmp19E->f2;_LLD3:;{
union Cyc_Absyn_Nmspace _tmp1A1=_tmp19F;switch((_tmp1A1.Rel_n).tag){case 4: _LLD5: _LLD6:
# 782
 if(_tmp19D == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp1A2=_cycalloc_atomic(sizeof(*_tmp1A2));_tmp1A2[0]=({struct Cyc_Dict_Absent_exn_struct _tmp1A3;_tmp1A3.tag=Cyc_Dict_Absent;_tmp1A3;});_tmp1A2;}));
goto _LLD8;case 1: if((_tmp1A1.Rel_n).val == 0){_LLD7: if(_tmp19D != 0){_LLD8: {
# 785
struct Cyc_Tcenv_Fenv*_tmp1A4=_tmp19D;struct Cyc_Tcenv_Fenv*_tmp1A5=_tmp1A4;const struct Cyc_Tcenv_Bindings*_tmp1A6;_LLDC: _tmp1A6=_tmp1A5->locals;_LLDD:;{
# 787
struct _handler_cons _tmp1A7;_push_handler(& _tmp1A7);{int _tmp1A9=0;if(setjmp(_tmp1A7.handler))_tmp1A9=1;if(!_tmp1A9){
{void*_tmp1AC=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp1AA=_region_malloc(r,sizeof(*_tmp1AA));_tmp1AA[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1AB;_tmp1AB.tag=0;_tmp1AB.f1=Cyc_Tcenv_lookup_binding(_tmp1A6,_tmp1A0);_tmp1AB;});_tmp1AA;});_npop_handler(0);return _tmp1AC;};_pop_handler();}else{void*_tmp1A8=(void*)_exn_thrown;void*_tmp1AD=_tmp1A8;void*_tmp1AE;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1AD)->tag == Cyc_Tcenv_NoBinding){_LLDF: _LLE0:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LLE1: _tmp1AE=_tmp1AD;_LLE2:(int)_rethrow(_tmp1AE);}_LLDE:;}};};}}else{goto _LLD9;}}else{goto _LLD9;}default: _LLD9: _LLDA: {
# 791
struct _handler_cons _tmp1AF;_push_handler(& _tmp1AF);{int _tmp1B1=0;if(setjmp(_tmp1AF.handler))_tmp1B1=1;if(!_tmp1B1){{void*_tmp1B2=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1B2;};_pop_handler();}else{void*_tmp1B0=(void*)_exn_thrown;void*_tmp1B3=_tmp1B0;void*_tmp1B4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B3)->tag == Cyc_Dict_Absent){_LLE4: _LLE5:
# 795
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp1B5=_region_malloc(r,sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1B6;_tmp1B6.tag=0;_tmp1B6.f1=(void*)& Cyc_Absyn_Unresolved_b_val;_tmp1B6;});_tmp1B5;});}else{_LLE6: _tmp1B4=_tmp1B3;_LLE7:(int)_rethrow(_tmp1B4);}_LLE3:;}};}}_LLD4:;};}
# 800
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv*_tmp1B7=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1C1="process_continue";_tag_dyneither(_tmp1C1,sizeof(char),17);}));
# 801
struct Cyc_Tcenv_Fenv*_tmp1B8=_tmp1B7;struct Cyc_Tcenv_CtrlEnv*_tmp1B9;_LLE9: _tmp1B9=_tmp1B8->ctrl_env;_LLEA:;{
# 803
void*_tmp1BA=_tmp1B9->continue_stmt;void*_tmp1BB=_tmp1BA;struct Cyc_Absyn_Stmt*_tmp1BC;switch(*((int*)_tmp1BB)){case 3: _LLEC: _tmp1BC=((struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1BB)->f1;_LLED:
# 805
 _tmp1BC->non_local_preds=({struct Cyc_List_List*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->hd=s;_tmp1BD->tl=_tmp1BC->non_local_preds;_tmp1BD;});
*sopt=_tmp1BC;
return;case 0: _LLEE: _LLEF:
({void*_tmp1BE=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp1BF="continue not in a loop";_tag_dyneither(_tmp1BF,sizeof(char),23);}),_tag_dyneither(_tmp1BE,sizeof(void*),0));});return;case 1: _LLF0: _LLF1:
 goto _LLF3;default: _LLF2: _LLF3:
((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp1C0="bad continue destination";_tag_dyneither(_tmp1C0,sizeof(char),25);}));}_LLEB:;};}
# 813
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv*_tmp1C2=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1CE="process_break";_tag_dyneither(_tmp1CE,sizeof(char),14);}));
# 814
struct Cyc_Tcenv_Fenv*_tmp1C3=_tmp1C2;struct Cyc_Tcenv_CtrlEnv*_tmp1C4;struct Cyc_Tcenv_SharedFenv*_tmp1C5;_LLF5: _tmp1C4=_tmp1C3->ctrl_env;_tmp1C5=_tmp1C3->shared;_LLF6:;{
# 816
void*_tmp1C6=_tmp1C4->break_stmt;void*_tmp1C7=_tmp1C6;struct Cyc_Absyn_Stmt*_tmp1C8;switch(*((int*)_tmp1C7)){case 3: _LLF8: _tmp1C8=((struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1C7)->f1;_LLF9:
# 818
 _tmp1C8->non_local_preds=({struct Cyc_List_List*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->hd=s;_tmp1C9->tl=_tmp1C8->non_local_preds;_tmp1C9;});
*sopt=_tmp1C8;
return;case 0: _LLFA: _LLFB:
# 822
({void*_tmp1CA=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp1CB="break not in a loop or switch";_tag_dyneither(_tmp1CB,sizeof(char),30);}),_tag_dyneither(_tmp1CA,sizeof(void*),0));});
return;case 2: _LLFC: _LLFD:
# 842 "tcenv.cyc"
 return;default: _LLFE: _LLFF:
# 847
({void*_tmp1CC=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp1CD="break causes outer switch clause to implicitly fallthru";_tag_dyneither(_tmp1CD,sizeof(char),56);}),_tag_dyneither(_tmp1CC,sizeof(void*),0));});
return;}_LLF7:;};}
# 851
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_Fenv*_tmp1CF=Cyc_Tcenv_get_fenv(te,({const char*_tmp1D6="process_goto";_tag_dyneither(_tmp1D6,sizeof(char),13);}));struct Cyc_Tcenv_Fenv*_tmp1D0=_tmp1CF;struct Cyc_Tcenv_SharedFenv*_tmp1D1;_LL101: _tmp1D1=_tmp1D0->shared;_LL102:;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1D1->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1D2=_tmp1D1->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1D2,l);
struct _RegionHandle*frgn=_tmp1D1->frgn;
if(slopt == 0)
slopt=({struct Cyc_List_List**_tmp1D3=_region_malloc(frgn,sizeof(*_tmp1D3));_tmp1D3[0]=0;_tmp1D3;});{
struct Cyc_List_List*new_needed=({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=s;_tmp1D4->tl=*slopt;_tmp1D4;});
_tmp1D1->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1D2,l,new_needed);};}else{
# 863
struct Cyc_Absyn_Stmt*s=*sopt2;
s->non_local_preds=({struct Cyc_List_List*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->hd=s;_tmp1D5->tl=s->non_local_preds;_tmp1D5;});
*sopt=s;}};}
# 870
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 872
struct Cyc_Tcenv_Fenv*_tmp1D7=Cyc_Tcenv_get_fenv(te,({const char*_tmp1DD="process_fallthru";_tag_dyneither(_tmp1DD,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp1D8=_tmp1D7;struct Cyc_Tcenv_CtrlEnv*_tmp1D9;_LL104: _tmp1D9=_tmp1D8->ctrl_env;_LL105:;{
const struct _tuple10*_tmp1DA=_tmp1D9->fallthru_clause;
if(_tmp1DA != (const struct _tuple10*)0){
(((*_tmp1DA).f1)->body)->non_local_preds=({struct Cyc_List_List*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->hd=s;_tmp1DB->tl=(((*_tmp1DA).f1)->body)->non_local_preds;_tmp1DB;});
# 877
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=(*_tmp1DA).f1;_tmp1DC;});}
# 879
return _tmp1DA;};}
# 882
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 886
struct Cyc_Tcenv_Fenv*_tmp1DE=
# 888
Cyc_Tcenv_get_fenv(te,({const char*_tmp1F6="set_fallthru";_tag_dyneither(_tmp1F6,sizeof(char),13);}));
# 886
struct Cyc_Tcenv_Fenv*_tmp1DF=_tmp1DE;struct Cyc_Tcenv_SharedFenv*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_RgnOrder_RgnPO*_tmp1E2;const struct Cyc_Tcenv_Bindings*_tmp1E3;struct Cyc_Absyn_Stmt*_tmp1E4;struct Cyc_Tcenv_CtrlEnv*_tmp1E5;void*_tmp1E6;void*_tmp1E7;struct Cyc_Tcenv_FenvFlags _tmp1E8;_LL107: _tmp1E0=_tmp1DF->shared;_tmp1E1=_tmp1DF->type_vars;_tmp1E2=_tmp1DF->region_order;_tmp1E3=_tmp1DF->locals;_tmp1E4=_tmp1DF->encloser;_tmp1E5=_tmp1DF->ctrl_env;_tmp1E6=_tmp1DF->capability;_tmp1E7=_tmp1DF->curr_rgn;_tmp1E8=_tmp1DF->flags;_LL108:;{
# 889
struct Cyc_Tcenv_CtrlEnv*_tmp1E9=_tmp1E5;struct _RegionHandle*_tmp1EA;void*_tmp1EB;void*_tmp1EC;const struct _tuple10*_tmp1ED;void*_tmp1EE;_LL10A: _tmp1EA=_tmp1E9->ctrl_rgn;_tmp1EB=_tmp1E9->continue_stmt;_tmp1EC=_tmp1E9->break_stmt;_tmp1ED=_tmp1E9->fallthru_clause;_tmp1EE=_tmp1E9->next_stmt;_LL10B:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp1EF=_region_malloc(_tmp1EA,sizeof(*_tmp1EF));_tmp1EF->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp1EF->tl=ft_typ;_tmp1EF;});}{
const struct Cyc_Tcenv_CList*_tmp1F0=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp1F1=({struct Cyc_Tcenv_CtrlEnv*_tmp1F4=_region_malloc(r,sizeof(*_tmp1F4));_tmp1F4->ctrl_rgn=_tmp1EA;_tmp1F4->continue_stmt=_tmp1EB;_tmp1F4->break_stmt=_tmp1EC;_tmp1F4->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp1F5=_region_malloc(_tmp1EA,sizeof(*_tmp1F5));_tmp1F5->f1=clause;_tmp1F5->f2=new_tvs;_tmp1F5->f3=_tmp1F0;_tmp1F5;});_tmp1F4->next_stmt=_tmp1EE;_tmp1F4;});
# 897
struct Cyc_Tcenv_Fenv*_tmp1F2=({struct Cyc_Tcenv_Fenv*_tmp1F3=_region_malloc(r,sizeof(*_tmp1F3));_tmp1F3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1E0;_tmp1F3->type_vars=(struct Cyc_List_List*)_tmp1E1;_tmp1F3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E2;_tmp1F3->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1E3;_tmp1F3->encloser=(struct Cyc_Absyn_Stmt*)_tmp1E4;_tmp1F3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1F1;_tmp1F3->capability=(void*)_tmp1E6;_tmp1F3->curr_rgn=(void*)_tmp1E7;_tmp1F3->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1E8;_tmp1F3->fnrgn=(struct _RegionHandle*)r;_tmp1F3;});
# 901
return Cyc_Tcenv_put_fenv(r,te,_tmp1F2);};};};}
# 904
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1F9="clear_fallthru";_tag_dyneither(_tmp1F9,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp1F7=fe;struct Cyc_Tcenv_CtrlEnv*_tmp1F8;_LL10D: _tmp1F8=_tmp1F7->ctrl_env;_LL10E:;
_tmp1F8->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 911
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 913
struct Cyc_Tcenv_Fenv*_tmp1FA=
# 915
Cyc_Tcenv_get_fenv(te,({const char*_tmp20B="set_in_loop";_tag_dyneither(_tmp20B,sizeof(char),12);}));
# 913
struct Cyc_Tcenv_Fenv*_tmp1FB=_tmp1FA;struct Cyc_Tcenv_SharedFenv*_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_RgnOrder_RgnPO*_tmp1FE;const struct Cyc_Tcenv_Bindings*_tmp1FF;struct Cyc_Absyn_Stmt*_tmp200;struct Cyc_Tcenv_CtrlEnv*_tmp201;void*_tmp202;void*_tmp203;struct Cyc_Tcenv_FenvFlags _tmp204;_LL110: _tmp1FC=_tmp1FB->shared;_tmp1FD=_tmp1FB->type_vars;_tmp1FE=_tmp1FB->region_order;_tmp1FF=_tmp1FB->locals;_tmp200=_tmp1FB->encloser;_tmp201=_tmp1FB->ctrl_env;_tmp202=_tmp1FB->capability;_tmp203=_tmp1FB->curr_rgn;_tmp204=_tmp1FB->flags;_LL111:;{
# 916
struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp206=_region_malloc(r,sizeof(*_tmp206));_tmp206->ctrl_rgn=r;_tmp206->continue_stmt=(void*)({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209[0]=({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp20A;_tmp20A.tag=3;_tmp20A.f1=continue_dest;_tmp20A;});_tmp209;});_tmp206->break_stmt=_tmp201->next_stmt;_tmp206->next_stmt=(void*)({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp207=_region_malloc(r,sizeof(*_tmp207));_tmp207[0]=({struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp208;_tmp208.tag=3;_tmp208.f1=continue_dest;_tmp208;});_tmp207;});_tmp206->fallthru_clause=_tmp201->fallthru_clause;_tmp206;});
# 922
struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp205=_region_malloc(r,sizeof(*_tmp205));_tmp205->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1FC;_tmp205->type_vars=(struct Cyc_List_List*)_tmp1FD;_tmp205->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FE;_tmp205->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FF;_tmp205->encloser=(struct Cyc_Absyn_Stmt*)_tmp200;_tmp205->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp205->capability=(void*)_tmp202;_tmp205->curr_rgn=(void*)_tmp203;_tmp205->flags=(struct Cyc_Tcenv_FenvFlags)_tmp204;_tmp205->fnrgn=(struct _RegionHandle*)r;_tmp205;});
# 925
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 930
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp20E="set_in_switch";_tag_dyneither(_tmp20E,sizeof(char),14);})));
struct Cyc_Tcenv_Fenv*_tmp20C=fe;struct Cyc_Tcenv_CtrlEnv*_tmp20D;_LL113: _tmp20D=_tmp20C->ctrl_env;_LL114:;
_tmp20D->break_stmt=_tmp20D->next_stmt;
_tmp20D->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 938
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 940
struct Cyc_Tcenv_Fenv _tmp20F=*
# 942
Cyc_Tcenv_get_fenv(te,({const char*_tmp21C="set_next";_tag_dyneither(_tmp21C,sizeof(char),9);}));
# 940
struct Cyc_Tcenv_Fenv _tmp210=_tmp20F;struct Cyc_Tcenv_SharedFenv*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_RgnOrder_RgnPO*_tmp213;const struct Cyc_Tcenv_Bindings*_tmp214;struct Cyc_Absyn_Stmt*_tmp215;struct Cyc_Tcenv_CtrlEnv*_tmp216;void*_tmp217;void*_tmp218;struct Cyc_Tcenv_FenvFlags _tmp219;_LL116: _tmp211=_tmp210.shared;_tmp212=_tmp210.type_vars;_tmp213=_tmp210.region_order;_tmp214=_tmp210.locals;_tmp215=_tmp210.encloser;_tmp216=_tmp210.ctrl_env;_tmp217=_tmp210.capability;_tmp218=_tmp210.curr_rgn;_tmp219=_tmp210.flags;_LL117:;{
# 943
struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp21B=_region_malloc(r,sizeof(*_tmp21B));_tmp21B->ctrl_rgn=r;_tmp21B->continue_stmt=_tmp216->continue_stmt;_tmp21B->break_stmt=_tmp216->break_stmt;_tmp21B->next_stmt=j;_tmp21B->fallthru_clause=_tmp216->fallthru_clause;_tmp21B;});
# 950
struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp21A=_region_malloc(r,sizeof(*_tmp21A));_tmp21A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp211;_tmp21A->type_vars=(struct Cyc_List_List*)_tmp212;_tmp21A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp213;_tmp21A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp214;_tmp21A->encloser=(struct Cyc_Absyn_Stmt*)_tmp215;_tmp21A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp21A->capability=(void*)_tmp217;_tmp21A->curr_rgn=(void*)_tmp218;_tmp21A->flags=(struct Cyc_Tcenv_FenvFlags)_tmp219;_tmp21A->fnrgn=(struct _RegionHandle*)r;_tmp21A;});
# 953
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 956
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Tcenv_Fenv*_tmp21D=Cyc_Tcenv_get_fenv(te,({const char*_tmp228="add_label";_tag_dyneither(_tmp228,sizeof(char),10);}));struct Cyc_Tcenv_Fenv*_tmp21E=_tmp21D;struct Cyc_Tcenv_SharedFenv*_tmp21F;_LL119: _tmp21F=_tmp21E->shared;_LL11A:;{
struct Cyc_Dict_Dict needed=_tmp21F->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp21F->frgn;
if(sl_opt != 0){
_tmp21F->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 964
struct Cyc_List_List*_tmp220=*sl_opt;
s->non_local_preds=_tmp220;
for(0;_tmp220 != 0;_tmp220=_tmp220->tl){
void*_tmp221=((struct Cyc_Absyn_Stmt*)_tmp220->hd)->r;void*_tmp222=_tmp221;struct Cyc_Absyn_Stmt**_tmp223;if(((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp222)->tag == 8){_LL11C: _tmp223=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp222)->f2;_LL11D:
*_tmp223=s;goto _LL11B;}else{_LL11E: _LL11F:
((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp224="Tcenv: add_label backpatching of non-goto";_tag_dyneither(_tmp224,sizeof(char),42);}));goto _LL11B;}_LL11B:;}};}
# 972
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp21F->seen_labels,v))
({struct Cyc_String_pa_PrintArg_struct _tmp227;_tmp227.tag=0;_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp225[1]={& _tmp227};Cyc_Tcutil_terr(s->loc,({const char*_tmp226="Repeated label: %s";_tag_dyneither(_tmp226,sizeof(char),19);}),_tag_dyneither(_tmp225,sizeof(void*),1));});});
_tmp21F->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp21F->seen_labels,v,s);
return te;};}
# 978
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp229=Cyc_Tcenv_get_fenv(te,({const char*_tmp22C="all_labels_resolved";_tag_dyneither(_tmp22C,sizeof(char),20);}));struct Cyc_Tcenv_Fenv*_tmp22A=_tmp229;struct Cyc_Tcenv_SharedFenv*_tmp22B;_LL121: _tmp22B=_tmp22A->shared;_LL122:;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp22B->needed_labels);}
# 983
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp22D=Cyc_Tcenv_get_fenv(te,({const char*_tmp230="get_encloser";_tag_dyneither(_tmp230,sizeof(char),13);}));struct Cyc_Tcenv_Fenv*_tmp22E=_tmp22D;struct Cyc_Absyn_Stmt*_tmp22F;_LL124: _tmp22F=_tmp22E->encloser;_LL125:;
return _tmp22F;}
# 988
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp231=_region_malloc(r2,sizeof(*_tmp231));_tmp231->ns=te->ns;_tmp231->ae=te->ae;_tmp231->le=0;_tmp231->allow_valueof=1;_tmp231->in_extern_c_include=te->in_extern_c_include;_tmp231;});{
struct Cyc_Tcenv_Fenv _tmp232=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp233=_tmp232;struct Cyc_Tcenv_SharedFenv*_tmp234;struct Cyc_List_List*_tmp235;struct Cyc_RgnOrder_RgnPO*_tmp236;const struct Cyc_Tcenv_Bindings*_tmp237;struct Cyc_Absyn_Stmt*_tmp238;struct Cyc_Tcenv_CtrlEnv*_tmp239;void*_tmp23A;void*_tmp23B;struct Cyc_Tcenv_FenvFlags _tmp23C;_LL127: _tmp234=_tmp233.shared;_tmp235=_tmp233.type_vars;_tmp236=_tmp233.region_order;_tmp237=_tmp233.locals;_tmp238=_tmp233.encloser;_tmp239=_tmp233.ctrl_env;_tmp23A=_tmp233.capability;_tmp23B=_tmp233.curr_rgn;_tmp23C=_tmp233.flags;_LL128:;{
struct Cyc_Tcenv_Fenv*_tmp23D=({struct Cyc_Tcenv_Fenv*_tmp23F=_region_malloc(r2,sizeof(*_tmp23F));_tmp23F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp234;_tmp23F->type_vars=(struct Cyc_List_List*)_tmp235;_tmp23F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp236;_tmp23F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp237;_tmp23F->encloser=(struct Cyc_Absyn_Stmt*)_tmp238;_tmp23F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp239;_tmp23F->capability=(void*)_tmp23A;_tmp23F->curr_rgn=(void*)_tmp23B;_tmp23F->flags=(struct Cyc_Tcenv_FenvFlags)_tmp23C;_tmp23F->fnrgn=(struct _RegionHandle*)r2;_tmp23F;});
return({struct Cyc_Tcenv_Tenv*_tmp23E=_region_malloc(r2,sizeof(*_tmp23E));_tmp23E->ns=te->ns;_tmp23E->ae=te->ae;_tmp23E->le=_tmp23D;_tmp23E->allow_valueof=1;_tmp23E->in_extern_c_include=te->in_extern_c_include;_tmp23E;});};};}
# 996
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp240=_region_malloc(r2,sizeof(*_tmp240));_tmp240->ns=te->ns;_tmp240->ae=te->ae;_tmp240->le=0;_tmp240->allow_valueof=te->allow_valueof;_tmp240->in_extern_c_include=1;_tmp240;});{
struct Cyc_Tcenv_Fenv _tmp241=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp242=_tmp241;struct Cyc_Tcenv_SharedFenv*_tmp243;struct Cyc_List_List*_tmp244;struct Cyc_RgnOrder_RgnPO*_tmp245;const struct Cyc_Tcenv_Bindings*_tmp246;struct Cyc_Absyn_Stmt*_tmp247;struct Cyc_Tcenv_CtrlEnv*_tmp248;void*_tmp249;void*_tmp24A;struct Cyc_Tcenv_FenvFlags _tmp24B;_LL12A: _tmp243=_tmp242.shared;_tmp244=_tmp242.type_vars;_tmp245=_tmp242.region_order;_tmp246=_tmp242.locals;_tmp247=_tmp242.encloser;_tmp248=_tmp242.ctrl_env;_tmp249=_tmp242.capability;_tmp24A=_tmp242.curr_rgn;_tmp24B=_tmp242.flags;_LL12B:;{
struct Cyc_Tcenv_Fenv*_tmp24C=({struct Cyc_Tcenv_Fenv*_tmp24E=_region_malloc(r2,sizeof(*_tmp24E));_tmp24E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp243;_tmp24E->type_vars=(struct Cyc_List_List*)_tmp244;_tmp24E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp245;_tmp24E->locals=(const struct Cyc_Tcenv_Bindings*)_tmp246;_tmp24E->encloser=(struct Cyc_Absyn_Stmt*)_tmp247;_tmp24E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp248;_tmp24E->capability=(void*)_tmp249;_tmp24E->curr_rgn=(void*)_tmp24A;_tmp24E->flags=(struct Cyc_Tcenv_FenvFlags)_tmp24B;_tmp24E->fnrgn=(struct _RegionHandle*)r2;_tmp24E;});
return({struct Cyc_Tcenv_Tenv*_tmp24D=_region_malloc(r2,sizeof(*_tmp24D));_tmp24D->ns=te->ns;_tmp24D->ae=te->ae;_tmp24D->le=_tmp24C;_tmp24D->allow_valueof=te->allow_valueof;_tmp24D->in_extern_c_include=1;_tmp24D;});};};}
# 1004
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1006
struct Cyc_Tcenv_Fenv _tmp24F=*
# 1008
Cyc_Tcenv_get_fenv(te,({const char*_tmp25C="set_encloser";_tag_dyneither(_tmp25C,sizeof(char),13);}));
# 1006
struct Cyc_Tcenv_Fenv _tmp250=_tmp24F;struct Cyc_Tcenv_SharedFenv*_tmp251;struct Cyc_List_List*_tmp252;struct Cyc_RgnOrder_RgnPO*_tmp253;const struct Cyc_Tcenv_Bindings*_tmp254;struct Cyc_Absyn_Stmt*_tmp255;struct Cyc_Tcenv_CtrlEnv*_tmp256;void*_tmp257;void*_tmp258;struct Cyc_Tcenv_FenvFlags _tmp259;_LL12D: _tmp251=_tmp250.shared;_tmp252=_tmp250.type_vars;_tmp253=_tmp250.region_order;_tmp254=_tmp250.locals;_tmp255=_tmp250.encloser;_tmp256=_tmp250.ctrl_env;_tmp257=_tmp250.capability;_tmp258=_tmp250.curr_rgn;_tmp259=_tmp250.flags;_LL12E:;{
# 1009
struct Cyc_Tcenv_Fenv*_tmp25A=({struct Cyc_Tcenv_Fenv*_tmp25B=_region_malloc(r,sizeof(*_tmp25B));_tmp25B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp251;_tmp25B->type_vars=(struct Cyc_List_List*)_tmp252;_tmp25B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp253;_tmp25B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp254;_tmp25B->encloser=(struct Cyc_Absyn_Stmt*)s;_tmp25B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp256;_tmp25B->capability=(void*)_tmp257;_tmp25B->curr_rgn=(void*)_tmp258;_tmp25B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp259;_tmp25B->fnrgn=(struct _RegionHandle*)r;_tmp25B;});
# 1012
return Cyc_Tcenv_put_fenv(r,te,_tmp25A);};}
# 1015
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1018
struct Cyc_Tcenv_Fenv _tmp25D=*
# 1020
Cyc_Tcenv_get_fenv(te,({const char*_tmp273="add_region";_tag_dyneither(_tmp273,sizeof(char),11);}));
# 1018
struct Cyc_Tcenv_Fenv _tmp25E=_tmp25D;struct Cyc_Tcenv_SharedFenv*_tmp25F;struct Cyc_List_List*_tmp260;struct Cyc_RgnOrder_RgnPO*_tmp261;const struct Cyc_Tcenv_Bindings*_tmp262;struct Cyc_Absyn_Stmt*_tmp263;struct Cyc_Tcenv_CtrlEnv*_tmp264;void*_tmp265;void*_tmp266;struct Cyc_Tcenv_FenvFlags _tmp267;_LL130: _tmp25F=_tmp25E.shared;_tmp260=_tmp25E.type_vars;_tmp261=_tmp25E.region_order;_tmp262=_tmp25E.locals;_tmp263=_tmp25E.encloser;_tmp264=_tmp25E.ctrl_env;_tmp265=_tmp25E.capability;_tmp266=_tmp25E.curr_rgn;_tmp267=_tmp25E.flags;_LL131:;
# 1021
{void*_tmp268=Cyc_Tcutil_compress(rgn);void*_tmp269=_tmp268;struct Cyc_Absyn_Tvar*_tmp26A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp269)->tag == 2){_LL133: _tmp26A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp269)->f1;_LL134:
# 1023
 _tmp261=Cyc_RgnOrder_add_youngest(_tmp25F->frgn,_tmp261,_tmp26A,resetable,opened);
# 1025
goto _LL132;}else{_LL135: _LL136:
 goto _LL132;}_LL132:;}
# 1028
_tmp265=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp26C;_tmp26C.tag=24;_tmp26C.f1=({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp270;_tmp270.tag=23;_tmp270.f1=rgn;_tmp270;});_tmp26F;});_tmp26D->tl=({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->hd=_tmp265;_tmp26E->tl=0;_tmp26E;});_tmp26D;});_tmp26C;});_tmp26B;});{
# 1030
struct Cyc_Tcenv_Fenv*_tmp271=({struct Cyc_Tcenv_Fenv*_tmp272=_region_malloc(r,sizeof(*_tmp272));_tmp272->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp25F;_tmp272->type_vars=(struct Cyc_List_List*)_tmp260;_tmp272->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp261;_tmp272->locals=(const struct Cyc_Tcenv_Bindings*)_tmp262;_tmp272->encloser=(struct Cyc_Absyn_Stmt*)_tmp263;_tmp272->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp264;_tmp272->capability=(void*)_tmp265;_tmp272->curr_rgn=(void*)_tmp266;_tmp272->flags=(struct Cyc_Tcenv_FenvFlags)_tmp267;_tmp272->fnrgn=(struct _RegionHandle*)r;_tmp272;});
# 1033
return Cyc_Tcenv_put_fenv(r,te,_tmp271);};}
# 1036
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1038
struct Cyc_Tcenv_Fenv _tmp274=*
# 1040
Cyc_Tcenv_get_fenv(te,({const char*_tmp28B="new_named_block";_tag_dyneither(_tmp28B,sizeof(char),16);}));
# 1038
struct Cyc_Tcenv_Fenv _tmp275=_tmp274;struct Cyc_Tcenv_SharedFenv*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_RgnOrder_RgnPO*_tmp278;const struct Cyc_Tcenv_Bindings*_tmp279;struct Cyc_Absyn_Stmt*_tmp27A;struct Cyc_Tcenv_CtrlEnv*_tmp27B;void*_tmp27C;void*_tmp27D;struct Cyc_Tcenv_FenvFlags _tmp27E;_LL138: _tmp276=_tmp275.shared;_tmp277=_tmp275.type_vars;_tmp278=_tmp275.region_order;_tmp279=_tmp275.locals;_tmp27A=_tmp275.encloser;_tmp27B=_tmp275.ctrl_env;_tmp27C=_tmp275.capability;_tmp27D=_tmp275.curr_rgn;_tmp27E=_tmp275.flags;_LL139:;{
# 1042
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp28A="new_block";_tag_dyneither(_tmp28A,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp289;_tmp289.tag=2;_tmp289.f1=block_rgn;_tmp289;});_tmp288;});
_tmp277=({struct Cyc_List_List*_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->hd=block_rgn;_tmp27F->tl=_tmp277;_tmp27F;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp277);
_tmp278=Cyc_RgnOrder_add_youngest(_tmp276->frgn,_tmp278,block_rgn,0,0);
_tmp27C=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp281;_tmp281.tag=24;_tmp281.f1=({struct Cyc_List_List*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp285;_tmp285.tag=23;_tmp285.f1=block_typ;_tmp285;});_tmp284;});_tmp282->tl=({struct Cyc_List_List*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->hd=_tmp27C;_tmp283->tl=0;_tmp283;});_tmp282;});_tmp281;});_tmp280;});
_tmp27D=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp286=({struct Cyc_Tcenv_Fenv*_tmp287=_region_malloc(r,sizeof(*_tmp287));_tmp287->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp276;_tmp287->type_vars=(struct Cyc_List_List*)_tmp277;_tmp287->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp278;_tmp287->locals=(const struct Cyc_Tcenv_Bindings*)_tmp279;_tmp287->encloser=(struct Cyc_Absyn_Stmt*)_tmp27A;_tmp287->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp27B;_tmp287->capability=(void*)_tmp27C;_tmp287->curr_rgn=(void*)_tmp27D;_tmp287->flags=(struct Cyc_Tcenv_FenvFlags)_tmp27E;_tmp287->fnrgn=(struct _RegionHandle*)r;_tmp287;});
# 1052
return Cyc_Tcenv_put_fenv(r,te,_tmp286);};};}
# 1055
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 1057
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 1059
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 1065
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1069
struct Cyc_Tcenv_Fenv _tmp28D=*
# 1071
Cyc_Tcenv_get_fenv(te,({const char*_tmp29A="new_outlives_constraints";_tag_dyneither(_tmp29A,sizeof(char),25);}));
# 1069
struct Cyc_Tcenv_Fenv _tmp28E=_tmp28D;struct Cyc_Tcenv_SharedFenv*_tmp28F;struct Cyc_List_List*_tmp290;struct Cyc_RgnOrder_RgnPO*_tmp291;const struct Cyc_Tcenv_Bindings*_tmp292;struct Cyc_Absyn_Stmt*_tmp293;struct Cyc_Tcenv_CtrlEnv*_tmp294;void*_tmp295;void*_tmp296;struct Cyc_Tcenv_FenvFlags _tmp297;_LL13B: _tmp28F=_tmp28E.shared;_tmp290=_tmp28E.type_vars;_tmp291=_tmp28E.region_order;_tmp292=_tmp28E.locals;_tmp293=_tmp28E.encloser;_tmp294=_tmp28E.ctrl_env;_tmp295=_tmp28E.capability;_tmp296=_tmp28E.curr_rgn;_tmp297=_tmp28E.flags;_LL13C:;
# 1072
for(0;cs != 0;cs=cs->tl){
_tmp291=
Cyc_RgnOrder_add_outlives_constraint(_tmp28F->frgn,_tmp291,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1077
struct Cyc_Tcenv_Fenv*_tmp298=({struct Cyc_Tcenv_Fenv*_tmp299=_region_malloc(r,sizeof(*_tmp299));_tmp299->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp28F;_tmp299->type_vars=(struct Cyc_List_List*)_tmp290;_tmp299->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp291;_tmp299->locals=(const struct Cyc_Tcenv_Bindings*)_tmp292;_tmp299->encloser=(struct Cyc_Absyn_Stmt*)_tmp293;_tmp299->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp294;_tmp299->capability=(void*)_tmp295;_tmp299->curr_rgn=(void*)_tmp296;_tmp299->flags=(struct Cyc_Tcenv_FenvFlags)_tmp297;_tmp299->fnrgn=(struct _RegionHandle*)r;_tmp299;});
# 1080
return Cyc_Tcenv_put_fenv(r,te,_tmp298);};}
# 1083
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1091
void*_tmp29B=Cyc_Tcutil_compress(r1);
void*_tmp29C=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp29D=Cyc_Tcutil_typ_kind(_tmp29B);
struct Cyc_Absyn_Kind*_tmp29E=Cyc_Tcutil_typ_kind(_tmp29C);
# 1097
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp29D,_tmp29E);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp29E,_tmp29D);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp29E));({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp29D));({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp29C));({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp29B));({void*_tmp29F[4]={& _tmp2A1,& _tmp2A2,& _tmp2A3,& _tmp2A4};Cyc_Tcutil_terr(loc,({const char*_tmp2A0="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp2A0,sizeof(char),82);}),_tag_dyneither(_tmp29F,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1104
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp2A5=Cyc_Tcutil_swap_kind(_tmp29C,Cyc_Tcutil_kind_to_bound(_tmp29D));struct _tuple11 _tmp2A6=_tmp2A5;struct Cyc_Absyn_Tvar*_tmp2A7;void*_tmp2A8;_LL13E: _tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;_LL13F:;
*oldtv=({struct _tuple11*_tmp2A9=_region_malloc(r,sizeof(*_tmp2A9));_tmp2A9->f1=_tmp2A7;_tmp2A9->f2=_tmp2A8;_tmp2A9;});}else{
# 1108
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp2AA=Cyc_Tcutil_swap_kind(_tmp29B,Cyc_Tcutil_kind_to_bound(_tmp29E));struct _tuple11 _tmp2AB=_tmp2AA;struct Cyc_Absyn_Tvar*_tmp2AC;void*_tmp2AD;_LL141: _tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;_LL142:;
*oldtv=({struct _tuple11*_tmp2AE=_region_malloc(r,sizeof(*_tmp2AE));_tmp2AE->f1=_tmp2AC;_tmp2AE->f2=_tmp2AD;_tmp2AE;});}}}{
# 1114
struct _RegionHandle*_tmp2AF=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2B0=0;
if((_tmp29B != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp29B != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp29B != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2B4;_tmp2B4.tag=23;_tmp2B4.f1=_tmp29B;_tmp2B4;});_tmp2B3;});
_tmp2B0=({struct Cyc_List_List*_tmp2B1=_region_malloc(_tmp2AF,sizeof(*_tmp2B1));_tmp2B1->hd=({struct _tuple13*_tmp2B2=_region_malloc(_tmp2AF,sizeof(*_tmp2B2));_tmp2B2->f1=eff1;_tmp2B2->f2=_tmp29C;_tmp2B2;});_tmp2B1->tl=_tmp2B0;_tmp2B1;});}
# 1120
if((_tmp29C != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp29C != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp29C != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2B8;_tmp2B8.tag=23;_tmp2B8.f1=_tmp29C;_tmp2B8;});_tmp2B7;});
_tmp2B0=({struct Cyc_List_List*_tmp2B5=_region_malloc(_tmp2AF,sizeof(*_tmp2B5));_tmp2B5->hd=({struct _tuple13*_tmp2B6=_region_malloc(_tmp2AF,sizeof(*_tmp2B6));_tmp2B6->f1=eff2;_tmp2B6->f2=_tmp29B;_tmp2B6;});_tmp2B5->tl=_tmp2B0;_tmp2B5;});}
# 1125
return Cyc_Tcenv_new_outlives_constraints(_tmp2AF,te,_tmp2B0,loc);};}
# 1128
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B9=te->le;
if(_tmp2B9 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2B9;
struct Cyc_Tcenv_Fenv*_tmp2BA=fe;void*_tmp2BB;_LL144: _tmp2BB=_tmp2BA->curr_rgn;_LL145:;
return _tmp2BB;};}
# 1139
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp2C4="check_rgn_accessible";_tag_dyneither(_tmp2C4,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp2BC=fe;void*_tmp2BD;struct Cyc_RgnOrder_RgnPO*_tmp2BE;_LL147: _tmp2BD=_tmp2BC->capability;_tmp2BE=_tmp2BC->region_order;_LL148:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2BD) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2BD))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2BE,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2C0;_tmp2C0.tag=23;_tmp2C0.f1=rgn;_tmp2C0;});_tmp2BF;}),_tmp2BD))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp2C1[1]={& _tmp2C3};Cyc_Tcutil_terr(loc,({const char*_tmp2C2="Expression accesses unavailable region %s";_tag_dyneither(_tmp2C2,sizeof(char),42);}),_tag_dyneither(_tmp2C1,sizeof(void*),1));});});}
# 1151
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp2C5=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2D0="check_rgn_resetable";_tag_dyneither(_tmp2D0,sizeof(char),20);}));
# 1153
struct Cyc_Tcenv_Fenv*_tmp2C6=_tmp2C5;struct Cyc_RgnOrder_RgnPO*_tmp2C7;_LL14A: _tmp2C7=_tmp2C6->region_order;_LL14B:;{
# 1155
void*_tmp2C8=Cyc_Tcutil_compress(rgn);void*_tmp2C9=_tmp2C8;struct Cyc_Absyn_Tvar*_tmp2CA;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C9)->tag == 2){_LL14D: _tmp2CA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C9)->f1;_LL14E:
# 1157
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2C7,_tmp2CA))
({struct Cyc_String_pa_PrintArg_struct _tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp2CB[1]={& _tmp2CD};Cyc_Tcutil_terr(loc,({const char*_tmp2CC="Region %s is not resetable";_tag_dyneither(_tmp2CC,sizeof(char),27);}),_tag_dyneither(_tmp2CB,sizeof(void*),1));});});
return;}else{_LL14F: _LL150:
# 1161
({void*_tmp2CE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2CF="check_rgn_resetable";_tag_dyneither(_tmp2CF,sizeof(char),20);}),_tag_dyneither(_tmp2CE,sizeof(void*),0));});}_LL14C:;};};}
# 1168
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2D1=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1173
if(_tmp2D1 == 0){
void*_tmp2D2=rt_a;switch(*((int*)_tmp2D2)){case 22: _LL152: _LL153:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LL154: _LL155:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LL156: _LL157:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL158: _LL159:
 return 0;}_LL151:;}{
# 1181
struct Cyc_Tcenv_Fenv*fe=_tmp2D1;
struct Cyc_Tcenv_Fenv*_tmp2D3=fe;struct Cyc_RgnOrder_RgnPO*_tmp2D4;_LL15B: _tmp2D4=_tmp2D3->region_order;_LL15C:;{
int _tmp2D5=Cyc_RgnOrder_effect_outlives(_tmp2D4,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2D7;_tmp2D7.tag=23;_tmp2D7.f1=rt_a;_tmp2D7;});_tmp2D6;}),rt_b);
# 1187
return _tmp2D5;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1192
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp2D8=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2DF="check_effect_accessible";_tag_dyneither(_tmp2DF,sizeof(char),24);}));
# 1193
struct Cyc_Tcenv_Fenv*_tmp2D9=_tmp2D8;void*_tmp2DA;struct Cyc_RgnOrder_RgnPO*_tmp2DB;struct Cyc_Tcenv_SharedFenv*_tmp2DC;_LL15E: _tmp2DA=_tmp2D9->capability;_tmp2DB=_tmp2D9->region_order;_tmp2DC=_tmp2D9->shared;_LL15F:;
# 1195
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2DA))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2DB,eff,_tmp2DA))
return;{
struct _RegionHandle*frgn=_tmp2DC->frgn;
_tmp2DC->delayed_effect_checks=({struct Cyc_List_List*_tmp2DD=_region_malloc(frgn,sizeof(*_tmp2DD));_tmp2DD->hd=({struct _tuple14*_tmp2DE=_region_malloc(frgn,sizeof(*_tmp2DE));_tmp2DE->f1=_tmp2DA;_tmp2DE->f2=eff;_tmp2DE->f3=_tmp2DB;_tmp2DE->f4=loc;_tmp2DE;});_tmp2DD->tl=_tmp2DC->delayed_effect_checks;_tmp2DD;});};}
# 1204
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2E0=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2EE="check_delayed_effects";_tag_dyneither(_tmp2EE,sizeof(char),22);}));
# 1205
struct Cyc_Tcenv_Fenv*_tmp2E1=_tmp2E0;struct Cyc_Tcenv_SharedFenv*_tmp2E2;_LL161: _tmp2E2=_tmp2E1->shared;_LL162:;{
# 1207
struct Cyc_List_List*_tmp2E3=_tmp2E2->delayed_effect_checks;
for(0;_tmp2E3 != 0;_tmp2E3=_tmp2E3->tl){
struct _tuple14*_tmp2E4=(struct _tuple14*)_tmp2E3->hd;struct _tuple14*_tmp2E5=_tmp2E4;void*_tmp2E6;void*_tmp2E7;struct Cyc_RgnOrder_RgnPO*_tmp2E8;unsigned int _tmp2E9;_LL164: _tmp2E6=_tmp2E5->f1;_tmp2E7=_tmp2E5->f2;_tmp2E8=_tmp2E5->f3;_tmp2E9=_tmp2E5->f4;_LL165:;
if(Cyc_Tcutil_subset_effect(1,_tmp2E7,_tmp2E6))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2E8,_tmp2E7,_tmp2E6))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp2ED;_tmp2ED.tag=0;_tmp2ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2E7));({struct Cyc_String_pa_PrintArg_struct _tmp2EC;_tmp2EC.tag=0;_tmp2EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2E6));({void*_tmp2EA[2]={& _tmp2EC,& _tmp2ED};Cyc_Tcutil_terr(_tmp2E9,({const char*_tmp2EB="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp2EB,sizeof(char),51);}),_tag_dyneither(_tmp2EA,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1223
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1225
struct Cyc_Tcenv_Fenv*_tmp2EF=te->le;
if(_tmp2EF == 0){
# 1228
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2F1;_tmp2F1.tag=23;_tmp2F1.f1=(*((struct _tuple13*)po->hd)).f2;_tmp2F1;});_tmp2F0;}),Cyc_Absyn_empty_effect))
({void*_tmp2F2=0;Cyc_Tcutil_terr(loc,({const char*_tmp2F3="the required region ordering is not satisfied here";_tag_dyneither(_tmp2F3,sizeof(char),51);}),_tag_dyneither(_tmp2F2,sizeof(void*),0));});}
return;}{
# 1234
struct Cyc_Tcenv_Fenv*_tmp2F4=_tmp2EF;struct Cyc_Tcenv_Fenv*_tmp2F5=_tmp2F4;struct Cyc_RgnOrder_RgnPO*_tmp2F6;struct Cyc_Tcenv_SharedFenv*_tmp2F7;_LL167: _tmp2F6=_tmp2F5->region_order;_tmp2F7=_tmp2F5->shared;_LL168:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2F6,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp2F7->delayed_constraint_checks=({struct Cyc_List_List*_tmp2F8=_region_malloc(_tmp2F7->frgn,sizeof(*_tmp2F8));_tmp2F8->hd=({struct _tuple15*_tmp2F9=_region_malloc(_tmp2F7->frgn,sizeof(*_tmp2F9));_tmp2F9->f1=_tmp2F6;_tmp2F9->f2=po;_tmp2F9->f3=loc;_tmp2F9;});_tmp2F8->tl=_tmp2F7->delayed_constraint_checks;_tmp2F8;});};}
# 1241
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2FA=
Cyc_Tcenv_get_fenv(te,({const char*_tmp305="check_delayed_constraints";_tag_dyneither(_tmp305,sizeof(char),26);}));
# 1242
struct Cyc_Tcenv_Fenv*_tmp2FB=_tmp2FA;struct Cyc_Tcenv_SharedFenv*_tmp2FC;_LL16A: _tmp2FC=_tmp2FB->shared;_LL16B:;{
# 1244
struct Cyc_List_List*_tmp2FD=_tmp2FC->delayed_constraint_checks;
for(0;_tmp2FD != 0;_tmp2FD=_tmp2FD->tl){
struct _tuple15*_tmp2FE=(struct _tuple15*)_tmp2FD->hd;struct _tuple15*_tmp2FF=_tmp2FE;struct Cyc_RgnOrder_RgnPO*_tmp300;struct Cyc_List_List*_tmp301;unsigned int _tmp302;_LL16D: _tmp300=_tmp2FF->f1;_tmp301=_tmp2FF->f2;_tmp302=_tmp2FF->f3;_LL16E:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp300,_tmp301,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp303=0;Cyc_Tcutil_terr(_tmp302,({const char*_tmp304="the required region ordering is not satisfied here";_tag_dyneither(_tmp304,sizeof(char),51);}),_tag_dyneither(_tmp303,sizeof(void*),0));});}};}
# 1252
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->name=({struct _dyneither_ptr*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33A;_tmp33A.tag=0;_tmp33A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp338[1]={& _tmp33A};Cyc_aprintf(({const char*_tmp339="`%s";_tag_dyneither(_tmp339,sizeof(char),4);}),_tag_dyneither(_tmp338,sizeof(void*),1));});});_tmp337;});_tmp336->identity=
# 1256
Cyc_Tcutil_new_tvar_id();_tmp336->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp336;});
struct Cyc_List_List*_tmp306=({struct Cyc_List_List*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335->hd=rgn0;_tmp335->tl=fd->tvs;_tmp335;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp306);{
# 1260
struct Cyc_RgnOrder_RgnPO*_tmp307=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1262
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp334;_tmp334.tag=2;_tmp334.f1=rgn0;_tmp334;});_tmp333;});
struct Cyc_List_List*_tmp308=0;
{struct Cyc_List_List*_tmp309=fd->args;for(0;_tmp309 != 0;_tmp309=_tmp309->tl){
struct Cyc_Absyn_Vardecl*_tmp30A=({struct Cyc_Absyn_Vardecl*_tmp312=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp312[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp313;_tmp313.sc=Cyc_Absyn_Public;_tmp313.name=({struct _tuple0*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->f1=Cyc_Absyn_Loc_n;_tmp314->f2=(*((struct _tuple8*)_tmp309->hd)).f1;_tmp314;});_tmp313.tq=(*((struct _tuple8*)_tmp309->hd)).f2;_tmp313.type=(*((struct _tuple8*)_tmp309->hd)).f3;_tmp313.initializer=0;_tmp313.rgn=param_rgn;_tmp313.attributes=0;_tmp313.escapes=0;_tmp313;});_tmp312;});
# 1272
_tmp308=({struct Cyc_List_List*_tmp30B=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp30B[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp30C;_tmp30C.hd=_tmp30A;_tmp30C.tl=_tmp308;_tmp30C;});_tmp30B;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp30D=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp311;_tmp311.tag=3;_tmp311.f1=_tmp30A;_tmp311;});_tmp310;});
struct _dyneither_ptr*_tmp30E=(*((struct _tuple8*)_tmp309->hd)).f1;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp30F=_region_malloc(r,sizeof(*_tmp30F));_tmp30F->v=_tmp30E;_tmp30F->b=(void*)_tmp30D;_tmp30F->tl=locals;_tmp30F;});};}}
# 1277
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp315=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp316=_tmp315;struct _dyneither_ptr*_tmp317;struct Cyc_Absyn_Tqual _tmp318;void*_tmp319;int _tmp31A;_LL170: _tmp317=_tmp316.name;_tmp318=_tmp316.tq;_tmp319=_tmp316.type;_tmp31A=_tmp316.inject;_LL171:;
if(_tmp317 != 0){
void*_tmp31B=Cyc_Absyn_dyneither_typ(_tmp319,param_rgn,_tmp318,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp31C=({struct Cyc_Absyn_Vardecl*_tmp323=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp323[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp324;_tmp324.sc=Cyc_Absyn_Public;_tmp324.name=({struct _tuple0*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325->f1=Cyc_Absyn_Loc_n;_tmp325->f2=_tmp317;_tmp325;});_tmp324.tq=
# 1283
Cyc_Absyn_empty_tqual(0);_tmp324.type=_tmp31B;_tmp324.initializer=0;_tmp324.rgn=param_rgn;_tmp324.attributes=0;_tmp324.escapes=0;_tmp324;});_tmp323;});
# 1288
_tmp308=({struct Cyc_List_List*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->hd=_tmp31C;_tmp31D->tl=_tmp308;_tmp31D;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp31E=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp322;_tmp322.tag=3;_tmp322.f1=_tmp31C;_tmp322;});_tmp321;});
struct _dyneither_ptr*_tmp31F=_tmp317;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp320=_region_malloc(r,sizeof(*_tmp320));_tmp320->v=_tmp31F;_tmp320->b=(void*)_tmp31E;_tmp320->tl=locals;_tmp320;});};}else{
# 1293
({void*_tmp326=0;Cyc_Tcutil_terr(loc,({const char*_tmp327="missing name for varargs";_tag_dyneither(_tmp327,sizeof(char),25);}),_tag_dyneither(_tmp326,sizeof(void*),0));});}}
# 1295
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp328=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp328[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp329;_tmp329.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp308);_tmp329;});_tmp328;});
return({struct Cyc_Tcenv_Fenv*_tmp32A=_region_malloc(r,sizeof(*_tmp32A));_tmp32A->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp332=_region_malloc(r,sizeof(*_tmp332));_tmp332->frgn=r;_tmp332->return_typ=fd->ret_type;_tmp332->seen_labels=
# 1300
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp332->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp332->delayed_effect_checks=0;_tmp332->delayed_constraint_checks=0;_tmp332;});_tmp32A->type_vars=(struct Cyc_List_List*)_tmp306;_tmp32A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp307;_tmp32A->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp32A->encloser=(struct Cyc_Absyn_Stmt*)fd->body;_tmp32A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp331=_region_malloc(r,sizeof(*_tmp331));_tmp331->ctrl_rgn=r;_tmp331->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp331->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp331->fallthru_clause=0;_tmp331->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val;_tmp331;});_tmp32A->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp32C;_tmp32C.tag=24;_tmp32C.f1=({struct Cyc_List_List*_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp330;_tmp330.tag=23;_tmp330.f1=param_rgn;_tmp330;});_tmp32F;});_tmp32D->tl=({struct Cyc_List_List*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->hd=(void*)_check_null(fd->effect);_tmp32E->tl=0;_tmp32E;});_tmp32D;});_tmp32C;});_tmp32B;}));_tmp32A->curr_rgn=(void*)param_rgn;_tmp32A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp32A->flags).in_notreadctxt=0;(_tmp32A->flags).in_lhs=0;(_tmp32A->flags).in_new=0;(_tmp32A->flags).abstract_ok=0;_tmp32A->flags;});_tmp32A->fnrgn=(struct _RegionHandle*)r;_tmp32A;});};}
# 1318
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 1321
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp35B->identity=Cyc_Tcutil_new_tvar_id();_tmp35B->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp35B;});
struct Cyc_List_List*_tmp33B=({struct Cyc_List_List*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A->hd=rgn0;_tmp35A->tl=0;_tmp35A;});
struct Cyc_RgnOrder_RgnPO*_tmp33C=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp359;_tmp359.tag=24;_tmp359.f1=0;_tmp359;});_tmp358;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp357;_tmp357.tag=2;_tmp357.f1=rgn0;_tmp357;});_tmp356;});
struct Cyc_List_List*_tmp33D=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp33E=*((struct _tuple8*)args->hd);struct _tuple8 _tmp33F=_tmp33E;struct _dyneither_ptr*_tmp340;struct Cyc_Absyn_Tqual _tmp341;void*_tmp342;_LL173: _tmp340=_tmp33F.f1;_tmp341=_tmp33F.f2;_tmp342=_tmp33F.f3;_LL174:;
if(_tmp340 != 0){
struct Cyc_Absyn_Vardecl*_tmp343=({struct Cyc_Absyn_Vardecl*_tmp34B=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp34B[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp34C;_tmp34C.sc=Cyc_Absyn_Public;_tmp34C.name=({struct _tuple0*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->f1=Cyc_Absyn_Loc_n;_tmp34D->f2=_tmp340;_tmp34D;});_tmp34C.tq=_tmp341;_tmp34C.type=_tmp342;_tmp34C.initializer=0;_tmp34C.rgn=param_rgn;_tmp34C.attributes=0;_tmp34C.escapes=0;_tmp34C;});_tmp34B;});
# 1337
_tmp33D=({struct Cyc_List_List*_tmp344=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp344[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp345;_tmp345.hd=_tmp343;_tmp345.tl=_tmp33D;_tmp345;});_tmp344;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp346=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp34A;_tmp34A.tag=3;_tmp34A.f1=_tmp343;_tmp34A;});_tmp349;});
struct _dyneither_ptr*_tmp347=_tmp340;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp348=_region_malloc(r,sizeof(*_tmp348));_tmp348->v=_tmp347;_tmp348->b=(void*)_tmp346;_tmp348->tl=locals;_tmp348;});};}}
# 1343
return({struct Cyc_Tcenv_Fenv*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));_tmp34E->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp355=_region_malloc(r,sizeof(*_tmp355));_tmp355->frgn=r;_tmp355->return_typ=ret_type;_tmp355->seen_labels=
# 1347
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp355->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp355->delayed_effect_checks=0;_tmp355->delayed_constraint_checks=0;_tmp355;});_tmp34E->type_vars=(struct Cyc_List_List*)_tmp33B;_tmp34E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp33C;_tmp34E->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp34E->encloser=(struct Cyc_Absyn_Stmt*)
# 1355
Cyc_Absyn_skip_stmt(0);_tmp34E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp354=_region_malloc(r,sizeof(*_tmp354));_tmp354->ctrl_rgn=r;_tmp354->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp354->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp354->fallthru_clause=0;_tmp354->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val;_tmp354;});_tmp34E->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp350;_tmp350.tag=24;_tmp350.f1=({struct Cyc_List_List*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp353;_tmp353.tag=23;_tmp353.f1=param_rgn;_tmp353;});_tmp352;});_tmp351->tl=0;_tmp351;});_tmp350;});_tmp34F;}));_tmp34E->curr_rgn=(void*)param_rgn;_tmp34E->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp34E->flags).in_notreadctxt=0;(_tmp34E->flags).in_lhs=0;(_tmp34E->flags).in_new=0;(_tmp34E->flags).abstract_ok=0;_tmp34E->flags;});_tmp34E->fnrgn=(struct _RegionHandle*)r;_tmp34E;});}
# 1364
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp35D=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp35E;struct Cyc_RgnOrder_RgnPO*_tmp35F;struct Cyc_List_List*_tmp360;struct Cyc_Tcenv_SharedFenv*_tmp361;struct _RegionHandle*_tmp362;_LL176: _tmp35E=_tmp35D->locals;_tmp35F=_tmp35D->region_order;_tmp360=_tmp35D->type_vars;_tmp361=_tmp35D->shared;_tmp362=_tmp35D->fnrgn;_LL177:;{
# 1367
struct _RegionHandle*_tmp363=_tmp361->frgn;
const struct Cyc_Tcenv_Bindings*_tmp364=_tmp35E;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->name=({struct _dyneither_ptr*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39F;_tmp39F.tag=0;_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp39D[1]={& _tmp39F};Cyc_aprintf(({const char*_tmp39E="`%s";_tag_dyneither(_tmp39E,sizeof(char),4);}),_tag_dyneither(_tmp39D,sizeof(void*),1));});});_tmp39C;});_tmp39B->identity=
# 1371
Cyc_Tcutil_new_tvar_id();_tmp39B->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp39B;});
# 1373
{struct Cyc_List_List*_tmp365=fd->tvs;for(0;_tmp365 != 0;_tmp365=_tmp365->tl){
struct Cyc_Absyn_Kind*_tmp366=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp365->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp367=_tmp366;enum Cyc_Absyn_KindQual _tmp368;enum Cyc_Absyn_AliasQual _tmp369;_LL179: _tmp368=_tmp367->kind;_tmp369=_tmp367->aliasqual;_LL17A:;
if(_tmp368 == Cyc_Absyn_RgnKind){
if(_tmp369 == Cyc_Absyn_Aliasable)
_tmp35F=Cyc_RgnOrder_add_youngest(_tmp363,_tmp35F,(struct Cyc_Absyn_Tvar*)_tmp365->hd,0,0);else{
# 1379
({void*_tmp36A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp36B="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp36B,sizeof(char),39);}),_tag_dyneither(_tmp36A,sizeof(void*),0));});}}}}
# 1381
_tmp35F=Cyc_RgnOrder_add_youngest(_tmp363,_tmp35F,rgn0,0,0);{
struct Cyc_List_List*_tmp36C=({struct Cyc_List_List*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->hd=rgn0;_tmp39A->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp360);_tmp39A;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp36C);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp399;_tmp399.tag=2;_tmp399.f1=rgn0;_tmp399;});_tmp398;});
struct Cyc_List_List*_tmp36D=0;
{struct Cyc_List_List*_tmp36E=fd->args;for(0;_tmp36E != 0;_tmp36E=_tmp36E->tl){
struct Cyc_Absyn_Vardecl*_tmp36F=({struct Cyc_Absyn_Vardecl*_tmp377=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp377[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp378;_tmp378.sc=Cyc_Absyn_Public;_tmp378.name=({struct _tuple0*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->f1=Cyc_Absyn_Loc_n;_tmp379->f2=(*((struct _tuple8*)_tmp36E->hd)).f1;_tmp379;});_tmp378.tq=(*((struct _tuple8*)_tmp36E->hd)).f2;_tmp378.type=(*((struct _tuple8*)_tmp36E->hd)).f3;_tmp378.initializer=0;_tmp378.rgn=param_rgn;_tmp378.attributes=0;_tmp378.escapes=0;_tmp378;});_tmp377;});
# 1394
_tmp36D=({struct Cyc_List_List*_tmp370=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp370[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp371;_tmp371.hd=_tmp36F;_tmp371.tl=_tmp36D;_tmp371;});_tmp370;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp372=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp376;_tmp376.tag=3;_tmp376.f1=_tmp36F;_tmp376;});_tmp375;});
struct _dyneither_ptr*_tmp373=(*((struct _tuple8*)_tmp36E->hd)).f1;
_tmp364=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp374=_region_malloc(_tmp363,sizeof(*_tmp374));_tmp374->v=_tmp373;_tmp374->b=(void*)_tmp372;_tmp374->tl=_tmp364;_tmp374;});};}}
# 1399
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp37A=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp37B=_tmp37A;struct _dyneither_ptr*_tmp37C;struct Cyc_Absyn_Tqual _tmp37D;void*_tmp37E;int _tmp37F;_LL17C: _tmp37C=_tmp37B.name;_tmp37D=_tmp37B.tq;_tmp37E=_tmp37B.type;_tmp37F=_tmp37B.inject;_LL17D:;
if(_tmp37C != 0){
void*_tmp380=Cyc_Absyn_dyneither_typ(_tmp37E,param_rgn,_tmp37D,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp381=({struct Cyc_Absyn_Vardecl*_tmp388=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp388[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp389;_tmp389.sc=Cyc_Absyn_Public;_tmp389.name=({struct _tuple0*_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A->f1=Cyc_Absyn_Loc_n;_tmp38A->f2=_tmp37C;_tmp38A;});_tmp389.tq=
# 1405
Cyc_Absyn_empty_tqual(0);_tmp389.type=_tmp380;_tmp389.initializer=0;_tmp389.rgn=param_rgn;_tmp389.attributes=0;_tmp389.escapes=0;_tmp389;});_tmp388;});
# 1410
_tmp36D=({struct Cyc_List_List*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->hd=_tmp381;_tmp382->tl=_tmp36D;_tmp382;});{
struct _dyneither_ptr*_tmp383=_tmp37C;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp384=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp387;_tmp387.tag=3;_tmp387.f1=_tmp381;_tmp387;});_tmp386;});
_tmp364=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp385=_region_malloc(_tmp363,sizeof(*_tmp385));_tmp385->v=_tmp383;_tmp385->b=(void*)_tmp384;_tmp385->tl=_tmp364;_tmp385;});};}else{
# 1415
({void*_tmp38B=0;Cyc_Tcutil_terr(loc,({const char*_tmp38C="missing name for varargs";_tag_dyneither(_tmp38C,sizeof(char),25);}),_tag_dyneither(_tmp38B,sizeof(void*),0));});}}
# 1417
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp38D=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp38D[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp38E;_tmp38E.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp36D);_tmp38E;});_tmp38D;});
return({struct Cyc_Tcenv_Fenv*_tmp38F=_region_malloc(_tmp363,sizeof(*_tmp38F));_tmp38F->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp397=_region_malloc(_tmp363,sizeof(*_tmp397));_tmp397->frgn=_tmp363;_tmp397->return_typ=fd->ret_type;_tmp397->seen_labels=
# 1422
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp363,Cyc_strptrcmp);_tmp397->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp363,Cyc_strptrcmp);_tmp397->delayed_effect_checks=0;_tmp397->delayed_constraint_checks=0;_tmp397;});_tmp38F->type_vars=(struct Cyc_List_List*)_tmp36C;_tmp38F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp35F;_tmp38F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp364;_tmp38F->encloser=(struct Cyc_Absyn_Stmt*)fd->body;_tmp38F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp396=_region_malloc(_tmp363,sizeof(*_tmp396));_tmp396->ctrl_rgn=_tmp363;_tmp396->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp396->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val;_tmp396->fallthru_clause=0;_tmp396->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val;_tmp396;});_tmp38F->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp391;_tmp391.tag=24;_tmp391.f1=({struct Cyc_List_List*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp395;_tmp395.tag=23;_tmp395.f1=param_rgn;_tmp395;});_tmp394;});_tmp392->tl=({struct Cyc_List_List*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->hd=(void*)_check_null(fd->effect);_tmp393->tl=0;_tmp393;});_tmp392;});_tmp391;});_tmp390;}));_tmp38F->curr_rgn=(void*)param_rgn;_tmp38F->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp38F->flags).in_notreadctxt=0;(_tmp38F->flags).in_lhs=0;(_tmp38F->flags).in_new=0;(_tmp38F->flags).abstract_ok=0;_tmp38F->flags;});_tmp38F->fnrgn=(struct _RegionHandle*)_tmp362;_tmp38F;});};};};}
