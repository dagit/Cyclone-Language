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
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_Set_Set;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 372
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 377
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 432 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 448
typedef void*Cyc_Absyn_funcparams_t;
# 451
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 515
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 541
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 548
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 566
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 923 "absyn.h"
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 932
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 946
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 966
extern void*Cyc_Absyn_empty_effect;
# 968
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1001
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
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 73
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 77
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 88
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 90
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 92
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 104 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 109
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 112
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 114
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 121
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 124
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 127
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 129
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 135
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 145
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 150
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 158
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 163
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 168
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 171
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 179
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 181
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 183
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 185
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 187
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 191
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 194
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 197
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 200
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
# 87 "tcenv.cyc"
int Cyc_Tcenv_warn_override=0;char Cyc_Tcenv_Env_error[10]="Env_error";
# 90
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 92
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 95
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 142
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{const struct _tuple10*fallthru_clause;};
# 148
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 171 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 205 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 208
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)
return bs->b;}
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 217
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
return({struct Cyc_Tcenv_Genv*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->grgn=r;_tmp2->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp2->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->availables=0;_tmp2;});}struct _tuple12{void*f1;int f2;};
# 228
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 230
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp3;});{
# 232
struct Cyc_Tcenv_Genv*_tmp4=Cyc_Tcenv_empty_genv(r);
_tmp4->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Datatypedecl**_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=Cyc_Absyn_exn_tud;_tmp5;}));
{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
_tmp4->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp4->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd)->name).f2,({struct _tuple12*_tmp7=_region_malloc(r,sizeof(*_tmp7));_tmp7->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp9;_tmp9.tag=2;_tmp9.f1=Cyc_Absyn_exn_tud;_tmp9.f2=(struct Cyc_Absyn_Datatypefield*)_tmp6->hd;_tmp9;});_tmp8;});_tmp7->f2=1;_tmp7;}));}}{
# 239
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp4);
return({struct Cyc_Tcenv_Tenv*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->ns=0;_tmpA->ae=ae;_tmpA->le=0;_tmpA->allow_valueof=0;_tmpA->in_extern_c_include=0;_tmpA;});};};}
# 245
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 247
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 251
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0)
return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpB="outer_namespace";_tag_dyneither(_tmpB,sizeof(char),16);}));
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 257
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 264
if(n2 != 0)return 0;
return 1;}
# 270
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 275
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpC;_push_handler(& _tmpC);{int _tmpE=0;if(setjmp(_tmpC.handler))_tmpE=1;if(!_tmpE){
lookup(ge2,v,0);
({struct Cyc_String_pa_PrintArg_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmpF[1]={& _tmp11};Cyc_Tcutil_terr(loc,({const char*_tmp10="%s is ambiguous";_tag_dyneither(_tmp10,sizeof(char),16);}),_tag_dyneither(_tmpF,sizeof(void*),1));});});
# 279
;_pop_handler();}else{void*_tmpD=(void*)_exn_thrown;void*_tmp12=_tmpD;void*_tmp13;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp12)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 281
 goto _LL0;}else{_LL3: _tmp13=_tmp12;_LL4:(int)_rethrow(_tmp13);}_LL0:;}};}}
# 283
return;}
# 286
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 291
{struct _handler_cons _tmp14;_push_handler(& _tmp14);{int _tmp16=0;if(setjmp(_tmp14.handler))_tmp16=1;if(!_tmp16){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp17=result;_npop_handler(0);return _tmp17;};}
# 292
;_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp18=_tmp15;void*_tmp19;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp18)->tag == Cyc_Dict_Absent){_LL6: _LL7:
# 295
 goto _LL5;}else{_LL8: _tmp19=_tmp18;_LL9:(int)_rethrow(_tmp19);}_LL5:;}};}
# 297
{struct Cyc_List_List*_tmp1A=ge->availables;for(0;_tmp1A != 0;_tmp1A=_tmp1A->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp1A->hd);
struct _handler_cons _tmp1B;_push_handler(& _tmp1B);{int _tmp1D=0;if(setjmp(_tmp1B.handler))_tmp1D=1;if(!_tmp1D){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp1A->hd,_tmp1A->tl);{
void*_tmp1E=result;_npop_handler(0);return _tmp1E;};}
# 300
;_pop_handler();}else{void*_tmp1C=(void*)_exn_thrown;void*_tmp1F=_tmp1C;void*_tmp20;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1F)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 303
 goto _LLA;}else{_LLD: _tmp20=_tmp1F;_LLE:(int)_rethrow(_tmp20);}_LLA:;}};}}
# 305
if(cns == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp21=_cycalloc_atomic(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Dict_Absent_exn_struct _tmp22;_tmp22.tag=Cyc_Dict_Absent;_tmp22;});_tmp21;}));
# 308
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 312
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp23=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp23,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 319
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple0*_tmp24=q;union Cyc_Absyn_Nmspace _tmp2C;struct _dyneither_ptr*_tmp2B;_LL10: _tmp2C=_tmp24->f1;_tmp2B=_tmp24->f2;_LL11:;{
union Cyc_Absyn_Nmspace _tmp25=_tmp2C;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp29;struct _dyneither_ptr*_tmp28;struct Cyc_List_List*_tmp27;switch((_tmp25.C_n).tag){case 4: _LL13: _LL14:
 goto _LL16;case 1: if((_tmp25.Rel_n).val == 0){_LL15: _LL16:
# 324
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp2B,is_use);}else{_LL17: _tmp28=(struct _dyneither_ptr*)((_tmp25.Rel_n).val)->hd;_tmp27=((_tmp25.Rel_n).val)->tl;_LL18: {
# 326
struct Cyc_Tcenv_Genv*_tmp26=Cyc_Tcenv_lookup_namespace(te,loc,_tmp28,_tmp27);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp26,_tmp2B,is_use);}}case 3: _LL19: _tmp29=(_tmp25.C_n).val;_LL1A:
 _tmp2A=_tmp29;goto _LL1C;default: _LL1B: _tmp2A=(_tmp25.Abs_n).val;_LL1C:
# 330
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2A),_tmp2B,is_use);}_LL12:;};}
# 335
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 337
struct Cyc_List_List*_tmp2D=te->ns;
struct _RegionHandle _tmp2E=_new_region("temp");struct _RegionHandle*temp=& _tmp2E;_push_region(temp);
{struct Cyc_List_List*_tmp2F=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp30=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2D);
struct Cyc_List_List*_tmp31=_tmp30->availables;
struct Cyc_Set_Set*_tmp32=_tmp30->namespaces;
# 345
{struct Cyc_List_List*_tmp33=_tmp31;for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
struct Cyc_Set_Set*_tmp34=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp33->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp34,n))
_tmp2F=({struct Cyc_List_List*_tmp35=_region_malloc(temp,sizeof(*_tmp35));_tmp35->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2D,({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->hd=n;_tmp36->tl=ns;_tmp36;}));_tmp35->tl=_tmp2F;_tmp35;});}}
# 352
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp32,n))
_tmp2F=({struct Cyc_List_List*_tmp37=_region_malloc(temp,sizeof(*_tmp37));_tmp37->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2D,({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->hd=n;_tmp38->tl=ns;_tmp38;}));_tmp37->tl=_tmp2F;_tmp37;});
# 358
if(_tmp2F != 0){
if(_tmp2F->tl != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n);({void*_tmp39[1]={& _tmp3B};Cyc_Tcutil_terr(loc,({const char*_tmp3A="%s is ambiguous";_tag_dyneither(_tmp3A,sizeof(char),16);}),_tag_dyneither(_tmp39,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp3C=(struct Cyc_List_List*)_tmp2F->hd;_npop_handler(0);return _tmp3C;};}
# 363
if(_tmp2D == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp3D=_cycalloc_atomic(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Dict_Absent_exn_struct _tmp3E;_tmp3E.tag=Cyc_Dict_Absent;_tmp3E;});_tmp3D;}));
# 366
_tmp2D=Cyc_Tcenv_outer_namespace(_tmp2D);}}
# 339
;_pop_region(temp);}
# 374
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp3F=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp3F,v);}
# 378
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp40=q;union Cyc_Absyn_Nmspace _tmp49;struct _dyneither_ptr*_tmp48;_LL1E: _tmp49=_tmp40->f1;_tmp48=_tmp40->f2;_LL1F:;{
union Cyc_Absyn_Nmspace _tmp41=_tmp49;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp46;struct _dyneither_ptr*_tmp45;struct Cyc_List_List*_tmp44;switch((_tmp41.C_n).tag){case 4: _LL21: _LL22:
 goto _LL24;case 1: if((_tmp41.Rel_n).val == 0){_LL23: _LL24:
# 383
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp48,0);}else{_LL29: _tmp45=(struct _dyneither_ptr*)((_tmp41.Rel_n).val)->hd;_tmp44=((_tmp41.Rel_n).val)->tl;_LL2A: {
# 389
struct Cyc_Dict_Dict _tmp43=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp45,_tmp44))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp43,_tmp48);}}case 3: _LL25: _tmp46=(_tmp41.C_n).val;_LL26:
# 384
 _tmp47=_tmp46;goto _LL28;default: _LL27: _tmp47=(_tmp41.Abs_n).val;_LL28: {
# 386
struct Cyc_Dict_Dict _tmp42=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp47))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp42,_tmp48);}}_LL20:;};}
# 394
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4A=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4A,v);}
# 398
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp4B=q;union Cyc_Absyn_Nmspace _tmp54;struct _dyneither_ptr*_tmp53;_LL2C: _tmp54=_tmp4B->f1;_tmp53=_tmp4B->f2;_LL2D:;{
union Cyc_Absyn_Nmspace _tmp4C=_tmp54;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp51;struct _dyneither_ptr*_tmp50;struct Cyc_List_List*_tmp4F;switch((_tmp4C.C_n).tag){case 4: _LL2F: _LL30:
 goto _LL32;case 1: if((_tmp4C.Rel_n).val == 0){_LL31: _LL32:
# 403
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp53,0);}else{_LL33: _tmp50=(struct _dyneither_ptr*)((_tmp4C.Rel_n).val)->hd;_tmp4F=((_tmp4C.Rel_n).val)->tl;_LL34: {
# 405
struct Cyc_Dict_Dict _tmp4D=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp50,_tmp4F))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4D,_tmp53);}}case 3: _LL35: _tmp51=(_tmp4C.C_n).val;_LL36:
 _tmp52=_tmp51;goto _LL38;default: _LL37: _tmp52=(_tmp4C.Abs_n).val;_LL38: {
# 409
struct Cyc_Dict_Dict _tmp4E=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp52))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4E,_tmp53);}}_LL2E:;};}
# 414
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 419
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp55=q;union Cyc_Absyn_Nmspace _tmp72;struct _dyneither_ptr*_tmp71;_LL3A: _tmp72=_tmp55->f1;_tmp71=_tmp55->f2;_LL3B:;{
union Cyc_Absyn_Nmspace _tmp56=_tmp72;struct Cyc_List_List*_tmp70;struct Cyc_List_List*_tmp6F;struct _dyneither_ptr*_tmp6E;struct Cyc_List_List*_tmp6D;switch((_tmp56.C_n).tag){case 1: if((_tmp56.Rel_n).val == 0){_LL3D: _LL3E: {
# 424
struct _handler_cons _tmp57;_push_handler(& _tmp57);{int _tmp59=0;if(setjmp(_tmp57.handler))_tmp59=1;if(!_tmp59){{struct Cyc_Absyn_Datatypedecl***_tmp5B=({struct Cyc_Absyn_Datatypedecl***_tmp5A=_region_malloc(r,sizeof(*_tmp5A));_tmp5A[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp71,0);_tmp5A;});_npop_handler(0);return _tmp5B;};_pop_handler();}else{void*_tmp58=(void*)_exn_thrown;void*_tmp5C=_tmp58;void*_tmp5D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp5C)->tag == Cyc_Dict_Absent){_LL48: _LL49:
 return 0;}else{_LL4A: _tmp5D=_tmp5C;_LL4B:(int)_rethrow(_tmp5D);}_LL47:;}};}}else{_LL41: _tmp6E=(struct _dyneither_ptr*)((_tmp56.Rel_n).val)->hd;_tmp6D=((_tmp56.Rel_n).val)->tl;_LL42: {
# 434
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp60;_push_handler(& _tmp60);{int _tmp62=0;if(setjmp(_tmp60.handler))_tmp62=1;if(!_tmp62){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp6E,_tmp6D);;_pop_handler();}else{void*_tmp61=(void*)_exn_thrown;void*_tmp63=_tmp61;void*_tmp68;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp63)->tag == Cyc_Dict_Absent){_LL4D: _LL4E:
# 438
({void*_tmp64=0;Cyc_Tcutil_terr(loc,({const char*_tmp65="bad qualified name for @extensible datatype";_tag_dyneither(_tmp65,sizeof(char),44);}),_tag_dyneither(_tmp64,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp66=_cycalloc_atomic(sizeof(*_tmp66));_tmp66[0]=({struct Cyc_Dict_Absent_exn_struct _tmp67;_tmp67.tag=Cyc_Dict_Absent;_tmp67;});_tmp66;}));}else{_LL4F: _tmp68=_tmp63;_LL50:(int)_rethrow(_tmp68);}_LL4C:;}};}{
# 441
struct Cyc_Dict_Dict _tmp69=ge->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp6A=_region_malloc(r,sizeof(*_tmp6A));_tmp6A[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp69,_tmp71);_tmp6A;});};}}case 4: _LL3F: _LL40:
# 427
({void*_tmp5E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5F="lookup_xdatatypedecl: impossible";_tag_dyneither(_tmp5F,sizeof(char),33);}),_tag_dyneither(_tmp5E,sizeof(void*),0));});
return 0;case 3: _LL43: _tmp6F=(_tmp56.C_n).val;_LL44:
# 443
 _tmp70=_tmp6F;goto _LL46;default: _LL45: _tmp70=(_tmp56.Abs_n).val;_LL46: {
# 445
struct Cyc_Dict_Dict _tmp6B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp70))->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp6C=_region_malloc(r,sizeof(*_tmp6C));_tmp6C[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp6B,_tmp71);_tmp6C;});}}_LL3C:;};}
# 450
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp73=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp73,v);}
# 454
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp74=q;union Cyc_Absyn_Nmspace _tmp7D;struct _dyneither_ptr*_tmp7C;_LL52: _tmp7D=_tmp74->f1;_tmp7C=_tmp74->f2;_LL53:;{
union Cyc_Absyn_Nmspace _tmp75=_tmp7D;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7A;struct _dyneither_ptr*_tmp79;struct Cyc_List_List*_tmp78;switch((_tmp75.C_n).tag){case 4: _LL55: _LL56:
 goto _LL58;case 1: if((_tmp75.Rel_n).val == 0){_LL57: _LL58:
# 459
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp7C,0);}else{_LL59: _tmp79=(struct _dyneither_ptr*)((_tmp75.Rel_n).val)->hd;_tmp78=((_tmp75.Rel_n).val)->tl;_LL5A: {
# 461
struct Cyc_Dict_Dict _tmp76=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp79,_tmp78))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp76,_tmp7C);}}case 3: _LL5B: _tmp7A=(_tmp75.C_n).val;_LL5C:
 _tmp7B=_tmp7A;goto _LL5E;default: _LL5D: _tmp7B=(_tmp75.Abs_n).val;_LL5E: {
# 465
struct Cyc_Dict_Dict _tmp77=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp7B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp77,_tmp7C);}}_LL54:;};}
# 470
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp7E=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,v);}
# 474
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp7F=q;union Cyc_Absyn_Nmspace _tmp88;struct _dyneither_ptr*_tmp87;_LL60: _tmp88=_tmp7F->f1;_tmp87=_tmp7F->f2;_LL61:;{
union Cyc_Absyn_Nmspace _tmp80=_tmp88;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp85;struct _dyneither_ptr*_tmp84;struct Cyc_List_List*_tmp83;switch((_tmp80.C_n).tag){case 4: _LL63: _LL64:
 goto _LL66;case 1: if((_tmp80.Rel_n).val == 0){_LL65: _LL66:
# 479
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp87,0);}else{_LL67: _tmp84=(struct _dyneither_ptr*)((_tmp80.Rel_n).val)->hd;_tmp83=((_tmp80.Rel_n).val)->tl;_LL68: {
# 481
struct Cyc_Dict_Dict _tmp81=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp84,_tmp83))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,_tmp87);}}case 3: _LL69: _tmp85=(_tmp80.C_n).val;_LL6A:
 _tmp86=_tmp85;goto _LL6C;default: _LL6B: _tmp86=(_tmp80.Abs_n).val;_LL6C: {
# 485
struct Cyc_Dict_Dict _tmp82=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp86))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp82,_tmp87);}}_LL62:;};}
# 492
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp89=te->le;
if(_tmp89 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp89;}
# 498
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp8A=te->le;
if(_tmp8A != 0){
struct Cyc_Tcenv_Fenv*_tmp8B=_tmp8A;struct Cyc_Tcenv_Fenv*_tmp8C=_tmp8B;struct _RegionHandle*_tmp8D;_LL6E: _tmp8D=_tmp8C->fnrgn;_LL6F:;
return _tmp8D;}
# 504
return Cyc_Core_heap_region;}
# 507
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp8E="put_fenv";_tag_dyneither(_tmp8E,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp8F=_region_malloc(l,sizeof(*_tmp8F));_tmp8F->ns=te->ns;_tmp8F->ae=te->ae;_tmp8F->le=fe;_tmp8F->allow_valueof=te->allow_valueof;_tmp8F->in_extern_c_include=te->in_extern_c_include;_tmp8F;});}
# 512
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp90=_region_malloc(l,sizeof(*_tmp90));_tmp90->ns=te->ns;_tmp90->ae=te->ae;_tmp90->le=0;_tmp90->allow_valueof=te->allow_valueof;_tmp90->in_extern_c_include=te->in_extern_c_include;_tmp90;});}
# 516
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 518
struct Cyc_Tcenv_Fenv _tmp91=*f;struct Cyc_Tcenv_Fenv _tmp92=_tmp91;struct Cyc_Tcenv_SharedFenv*_tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_RgnOrder_RgnPO*_tmp9A;const struct Cyc_Tcenv_Bindings*_tmp99;struct Cyc_Tcenv_CtrlEnv*_tmp98;void*_tmp97;void*_tmp96;struct Cyc_Tcenv_FenvFlags _tmp95;struct _RegionHandle*_tmp94;_LL71: _tmp9C=_tmp92.shared;_tmp9B=_tmp92.type_vars;_tmp9A=_tmp92.region_order;_tmp99=_tmp92.locals;_tmp98=_tmp92.ctrl_env;_tmp97=_tmp92.capability;_tmp96=_tmp92.curr_rgn;_tmp95=_tmp92.flags;_tmp94=_tmp92.fnrgn;_LL72:;
# 520
return({struct Cyc_Tcenv_Fenv*_tmp93=_region_malloc(l,sizeof(*_tmp93));_tmp93->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9C;_tmp93->type_vars=(struct Cyc_List_List*)_tmp9B;_tmp93->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp9A;_tmp93->locals=(const struct Cyc_Tcenv_Bindings*)_tmp99;_tmp93->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp98;_tmp93->capability=(void*)_tmp97;_tmp93->curr_rgn=(void*)_tmp96;_tmp93->flags=(struct Cyc_Tcenv_FenvFlags)_tmp95;_tmp93->fnrgn=(struct _RegionHandle*)l;_tmp93;});}
# 524
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 526
struct Cyc_Tcenv_Fenv _tmp9D=*f;struct Cyc_Tcenv_Fenv _tmp9E=_tmp9D;struct Cyc_Tcenv_SharedFenv*_tmpAC;struct Cyc_List_List*_tmpAB;struct Cyc_RgnOrder_RgnPO*_tmpAA;const struct Cyc_Tcenv_Bindings*_tmpA9;struct Cyc_Tcenv_CtrlEnv*_tmpA8;void*_tmpA7;void*_tmpA6;struct Cyc_Tcenv_FenvFlags _tmpA5;_LL74: _tmpAC=_tmp9E.shared;_tmpAB=_tmp9E.type_vars;_tmpAA=_tmp9E.region_order;_tmpA9=_tmp9E.locals;_tmpA8=_tmp9E.ctrl_env;_tmpA7=_tmp9E.capability;_tmpA6=_tmp9E.curr_rgn;_tmpA5=_tmp9E.flags;_LL75:;{
# 529
struct Cyc_Tcenv_CtrlEnv _tmp9F=*_tmpA8;struct Cyc_Tcenv_CtrlEnv _tmpA0=_tmp9F;const struct _tuple10*_tmpA4;_LL77: _tmpA4=_tmpA0.fallthru_clause;_LL78:;{
struct Cyc_Tcenv_CtrlEnv*_tmpA1=({struct Cyc_Tcenv_CtrlEnv*_tmpA3=_region_malloc(l,sizeof(*_tmpA3));_tmpA3->fallthru_clause=_tmpA4;_tmpA3;});
# 532
return({struct Cyc_Tcenv_Fenv*_tmpA2=_region_malloc(l,sizeof(*_tmpA2));_tmpA2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAC;_tmpA2->type_vars=(struct Cyc_List_List*)_tmpAB;_tmpA2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAA;_tmpA2->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA9;_tmpA2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA1;_tmpA2->capability=(void*)_tmpA7;_tmpA2->curr_rgn=(void*)_tmpA6;_tmpA2->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA5;_tmpA2->fnrgn=(struct _RegionHandle*)l;_tmpA2;});};};}
# 538
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpAD=Cyc_Tcenv_get_fenv(te,({const char*_tmpB3="return_typ";_tag_dyneither(_tmpB3,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmpAE=_tmpAD;struct Cyc_Tcenv_SharedFenv*_tmpB2;_LL7A: _tmpB2=_tmpAE->shared;_LL7B:;{
struct Cyc_Tcenv_SharedFenv _tmpAF=*_tmpB2;struct Cyc_Tcenv_SharedFenv _tmpB0=_tmpAF;void*_tmpB1;_LL7D: _tmpB1=_tmpB0.return_typ;_LL7E:;
return _tmpB1;};}
# 544
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpB4=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpB5=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpB4));struct Cyc_Tcenv_Fenv _tmpB6=_tmpB5;struct Cyc_List_List*_tmpB7;_LL80: _tmpB7=_tmpB6.type_vars;_LL81:;
return _tmpB7;};}
# 551
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB8;});}
# 555
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 557
struct Cyc_Tcenv_Fenv*_tmpB9=
# 559
Cyc_Tcenv_get_fenv(te,({const char*_tmpC6="add_type_vars";_tag_dyneither(_tmpC6,sizeof(char),14);}));
# 557
struct Cyc_Tcenv_Fenv*_tmpBA=_tmpB9;struct Cyc_Tcenv_SharedFenv*_tmpC5;struct Cyc_List_List*_tmpC4;struct Cyc_RgnOrder_RgnPO*_tmpC3;const struct Cyc_Tcenv_Bindings*_tmpC2;struct Cyc_Tcenv_CtrlEnv*_tmpC1;void*_tmpC0;void*_tmpBF;struct Cyc_Tcenv_FenvFlags _tmpBE;_LL83: _tmpC5=_tmpBA->shared;_tmpC4=_tmpBA->type_vars;_tmpC3=_tmpBA->region_order;_tmpC2=_tmpBA->locals;_tmpC1=_tmpBA->ctrl_env;_tmpC0=_tmpBA->capability;_tmpBF=_tmpBA->curr_rgn;_tmpBE=_tmpBA->flags;_LL84:;
# 561
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC4);
Cyc_Tcutil_check_unique_tvars(loc,_tmpBB);{
struct Cyc_Tcenv_Fenv*_tmpBC=({struct Cyc_Tcenv_Fenv*_tmpBD=_region_malloc(r,sizeof(*_tmpBD));_tmpBD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC5;_tmpBD->type_vars=(struct Cyc_List_List*)_tmpBB;_tmpBD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC3;_tmpBD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC2;_tmpBD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpC1;_tmpBD->capability=(void*)_tmpC0;_tmpBD->curr_rgn=(void*)_tmpBF;_tmpBD->flags=(struct Cyc_Tcenv_FenvFlags)_tmpBE;_tmpBD->fnrgn=(struct _RegionHandle*)r;_tmpBD;});
# 566
return Cyc_Tcenv_put_fenv(r,te,_tmpBC);};};}
# 569
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC7=te->le;
if(_tmpC7 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 574
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpC7);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 579
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 581
struct _dyneither_ptr*_tmpC8=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpC9=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpCA=_tmpC9;if((_tmpCA.Loc_n).tag == 4){_LL86: _LL87:
 goto _LL85;}else{_LL88: _LL89:
# 586
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Core_Impossible_exn_struct _tmpCC;_tmpCC.tag=Cyc_Core_Impossible;_tmpCC.f1=({const char*_tmpCD="add_local_var: called with Rel_n";_tag_dyneither(_tmpCD,sizeof(char),33);});_tmpCC;});_tmpCB;}));}_LL85:;}
# 588
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpCE;_push_handler(& _tmpCE);{int _tmpD0=0;if(setjmp(_tmpCE.handler))_tmpD0=1;if(!_tmpD0){{void*_tmpD1=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpD2=_tmpD1;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpD2)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpD2)->f1)){case 4: _LL8B: _LL8C:
({void*_tmpD3=0;Cyc_Tcutil_warn(loc,({const char*_tmpD4="declaration hides local";_tag_dyneither(_tmpD4,sizeof(char),24);}),_tag_dyneither(_tmpD3,sizeof(void*),0));});goto _LL8A;case 1: _LL8D: _LL8E:
({void*_tmpD5=0;Cyc_Tcutil_warn(loc,({const char*_tmpD6="declaration hides global";_tag_dyneither(_tmpD6,sizeof(char),25);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});goto _LL8A;case 5: _LL8F: _LL90:
({void*_tmpD7=0;Cyc_Tcutil_warn(loc,({const char*_tmpD8="declaration hides pattern variable";_tag_dyneither(_tmpD8,sizeof(char),35);}),_tag_dyneither(_tmpD7,sizeof(void*),0));});goto _LL8A;case 3: _LL91: _LL92:
({void*_tmpD9=0;Cyc_Tcutil_warn(loc,({const char*_tmpDA="declaration hides parameter";_tag_dyneither(_tmpDA,sizeof(char),28);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});goto _LL8A;default: goto _LL93;}else{_LL93: _LL94:
 goto _LL8A;}_LL8A:;}
# 589
;_pop_handler();}else{void*_tmpCF=(void*)_exn_thrown;void*_tmpDB=_tmpCF;void*_tmpDC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpDB)->tag == Cyc_Dict_Absent){_LL96: _LL97:
# 596
 goto _LL95;}else{_LL98: _tmpDC=_tmpDB;_LL99:(int)_rethrow(_tmpDC);}_LL95:;}};}{
struct Cyc_Tcenv_Fenv*_tmpDD=
# 599
Cyc_Tcenv_get_fenv(te,({const char*_tmpEE="add_local_var";_tag_dyneither(_tmpEE,sizeof(char),14);}));
# 597
struct Cyc_Tcenv_Fenv*_tmpDE=_tmpDD;struct Cyc_Tcenv_SharedFenv*_tmpED;struct Cyc_List_List*_tmpEC;struct Cyc_RgnOrder_RgnPO*_tmpEB;const struct Cyc_Tcenv_Bindings*_tmpEA;struct Cyc_Tcenv_CtrlEnv*_tmpE9;void*_tmpE8;void*_tmpE7;struct Cyc_Tcenv_FenvFlags _tmpE6;_LL9B: _tmpED=_tmpDE->shared;_tmpEC=_tmpDE->type_vars;_tmpEB=_tmpDE->region_order;_tmpEA=_tmpDE->locals;_tmpE9=_tmpDE->ctrl_env;_tmpE8=_tmpDE->capability;_tmpE7=_tmpDE->curr_rgn;_tmpE6=_tmpDE->flags;_LL9C:;{
# 600
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDF=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpE5;_tmpE5.tag=4;_tmpE5.f1=vd;_tmpE5;});_tmpE4;});
struct Cyc_Tcenv_Bindings*_tmpE0=({struct Cyc_Tcenv_Bindings*_tmpE3=_region_malloc(r,sizeof(*_tmpE3));_tmpE3->v=_tmpC8;_tmpE3->b=(void*)_tmpDF;_tmpE3->tl=_tmpEA;_tmpE3;});
struct Cyc_Tcenv_Fenv*_tmpE1=({struct Cyc_Tcenv_Fenv*_tmpE2=_region_malloc(r,sizeof(*_tmpE2));_tmpE2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpED;_tmpE2->type_vars=(struct Cyc_List_List*)_tmpEC;_tmpE2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEB;_tmpE2->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpE0);_tmpE2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE9;_tmpE2->capability=(void*)_tmpE8;_tmpE2->curr_rgn=(void*)_tmpE7;_tmpE2->flags=(struct Cyc_Tcenv_FenvFlags)_tmpE6;_tmpE2->fnrgn=(struct _RegionHandle*)r;_tmpE2;});
# 604
return Cyc_Tcenv_put_fenv(r,te,_tmpE1);};};}
# 607
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 609
struct Cyc_Tcenv_Fenv*_tmpEF=te->le;
if(_tmpEF == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpF0=*_tmpEF;struct Cyc_Tcenv_Fenv _tmpF1=_tmpF0;struct Cyc_Tcenv_SharedFenv*_tmpFD;struct Cyc_List_List*_tmpFC;struct Cyc_RgnOrder_RgnPO*_tmpFB;const struct Cyc_Tcenv_Bindings*_tmpFA;struct Cyc_Tcenv_CtrlEnv*_tmpF9;void*_tmpF8;void*_tmpF7;int _tmpF6;int _tmpF5;int _tmpF4;_LL9E: _tmpFD=_tmpF1.shared;_tmpFC=_tmpF1.type_vars;_tmpFB=_tmpF1.region_order;_tmpFA=_tmpF1.locals;_tmpF9=_tmpF1.ctrl_env;_tmpF8=_tmpF1.capability;_tmpF7=_tmpF1.curr_rgn;_tmpF6=(_tmpF1.flags).in_notreadctxt;_tmpF5=(_tmpF1.flags).in_lhs;_tmpF4=(_tmpF1.flags).abstract_ok;_LL9F:;{
# 614
struct Cyc_Tcenv_Fenv*_tmpF2=({struct Cyc_Tcenv_Fenv*_tmpF3=_region_malloc(r,sizeof(*_tmpF3));_tmpF3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFD;_tmpF3->type_vars=(struct Cyc_List_List*)_tmpFC;_tmpF3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFB;_tmpF3->locals=(const struct Cyc_Tcenv_Bindings*)_tmpFA;_tmpF3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF9;_tmpF3->capability=(void*)_tmpF8;_tmpF3->curr_rgn=(void*)_tmpF7;_tmpF3->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpF3->flags).in_notreadctxt=_tmpF6;(_tmpF3->flags).in_lhs=_tmpF5;(_tmpF3->flags).in_new=(int)status;(_tmpF3->flags).abstract_ok=_tmpF4;_tmpF3->flags;});_tmpF3->fnrgn=(struct _RegionHandle*)r;_tmpF3;});
# 617
return Cyc_Tcenv_put_fenv(r,te,_tmpF2);};};}
# 620
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpFE=te->le;
if(_tmpFE == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmpFF=_tmpFE;struct Cyc_Tcenv_Fenv*_tmp100=_tmpFF;int _tmp101;_LLA1: _tmp101=(_tmp100->flags).in_new;_LLA2:;
return(enum Cyc_Tcenv_NewStatus)_tmp101;};}
# 627
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp102=te->le;
if(_tmp102 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp103=_tmp102;struct Cyc_Tcenv_Fenv*_tmp104=_tmp103;int _tmp105;_LLA4: _tmp105=(_tmp104->flags).abstract_ok;_LLA5:;
return _tmp105;};}
# 634
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 636
struct Cyc_Tcenv_Fenv*_tmp106=te->le;
if(_tmp106 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp107=*_tmp106;struct Cyc_Tcenv_Fenv _tmp108=_tmp107;struct Cyc_Tcenv_SharedFenv*_tmp114;struct Cyc_List_List*_tmp113;struct Cyc_RgnOrder_RgnPO*_tmp112;const struct Cyc_Tcenv_Bindings*_tmp111;struct Cyc_Tcenv_CtrlEnv*_tmp110;void*_tmp10F;void*_tmp10E;int _tmp10D;int _tmp10C;int _tmp10B;_LLA7: _tmp114=_tmp108.shared;_tmp113=_tmp108.type_vars;_tmp112=_tmp108.region_order;_tmp111=_tmp108.locals;_tmp110=_tmp108.ctrl_env;_tmp10F=_tmp108.capability;_tmp10E=_tmp108.curr_rgn;_tmp10D=(_tmp108.flags).in_notreadctxt;_tmp10C=(_tmp108.flags).in_lhs;_tmp10B=(_tmp108.flags).in_new;_LLA8:;{
# 641
struct Cyc_Tcenv_Fenv*_tmp109=({struct Cyc_Tcenv_Fenv*_tmp10A=_region_malloc(r,sizeof(*_tmp10A));_tmp10A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp114;_tmp10A->type_vars=(struct Cyc_List_List*)_tmp113;_tmp10A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp112;_tmp10A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp111;_tmp10A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp110;_tmp10A->capability=(void*)_tmp10F;_tmp10A->curr_rgn=(void*)_tmp10E;_tmp10A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp10A->flags).in_notreadctxt=_tmp10D;(_tmp10A->flags).in_lhs=_tmp10C;(_tmp10A->flags).in_new=_tmp10B;(_tmp10A->flags).abstract_ok=1;_tmp10A->flags;});_tmp10A->fnrgn=(struct _RegionHandle*)r;_tmp10A;});
# 644
return Cyc_Tcenv_put_fenv(r,te,_tmp109);};};}
# 647
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 649
struct Cyc_Tcenv_Fenv*_tmp115=te->le;
if(_tmp115 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp116=*_tmp115;struct Cyc_Tcenv_Fenv _tmp117=_tmp116;struct Cyc_Tcenv_SharedFenv*_tmp123;struct Cyc_List_List*_tmp122;struct Cyc_RgnOrder_RgnPO*_tmp121;const struct Cyc_Tcenv_Bindings*_tmp120;struct Cyc_Tcenv_CtrlEnv*_tmp11F;void*_tmp11E;void*_tmp11D;int _tmp11C;int _tmp11B;int _tmp11A;_LLAA: _tmp123=_tmp117.shared;_tmp122=_tmp117.type_vars;_tmp121=_tmp117.region_order;_tmp120=_tmp117.locals;_tmp11F=_tmp117.ctrl_env;_tmp11E=_tmp117.capability;_tmp11D=_tmp117.curr_rgn;_tmp11C=(_tmp117.flags).in_notreadctxt;_tmp11B=(_tmp117.flags).in_lhs;_tmp11A=(_tmp117.flags).in_new;_LLAB:;{
# 654
struct Cyc_Tcenv_Fenv*_tmp118=({struct Cyc_Tcenv_Fenv*_tmp119=_region_malloc(r,sizeof(*_tmp119));_tmp119->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp123;_tmp119->type_vars=(struct Cyc_List_List*)_tmp122;_tmp119->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp121;_tmp119->locals=(const struct Cyc_Tcenv_Bindings*)_tmp120;_tmp119->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp11F;_tmp119->capability=(void*)_tmp11E;_tmp119->curr_rgn=(void*)_tmp11D;_tmp119->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp119->flags).in_notreadctxt=_tmp11C;(_tmp119->flags).in_lhs=_tmp11B;(_tmp119->flags).in_new=_tmp11A;(_tmp119->flags).abstract_ok=0;_tmp119->flags;});_tmp119->fnrgn=(struct _RegionHandle*)r;_tmp119;});
# 657
return Cyc_Tcenv_put_fenv(r,te,_tmp118);};};}
# 660
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp124=te->le;
if(_tmp124 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp125=*_tmp124;struct Cyc_Tcenv_Fenv _tmp126=_tmp125;struct Cyc_Tcenv_SharedFenv*_tmp132;struct Cyc_List_List*_tmp131;struct Cyc_RgnOrder_RgnPO*_tmp130;const struct Cyc_Tcenv_Bindings*_tmp12F;struct Cyc_Tcenv_CtrlEnv*_tmp12E;void*_tmp12D;void*_tmp12C;int _tmp12B;int _tmp12A;int _tmp129;_LLAD: _tmp132=_tmp126.shared;_tmp131=_tmp126.type_vars;_tmp130=_tmp126.region_order;_tmp12F=_tmp126.locals;_tmp12E=_tmp126.ctrl_env;_tmp12D=_tmp126.capability;_tmp12C=_tmp126.curr_rgn;_tmp12B=(_tmp126.flags).in_lhs;_tmp12A=(_tmp126.flags).in_new;_tmp129=(_tmp126.flags).abstract_ok;_LLAE:;{
# 666
struct Cyc_Tcenv_Fenv*_tmp127=({struct Cyc_Tcenv_Fenv*_tmp128=_region_malloc(r,sizeof(*_tmp128));_tmp128->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp132;_tmp128->type_vars=(struct Cyc_List_List*)_tmp131;_tmp128->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp130;_tmp128->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12F;_tmp128->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12E;_tmp128->capability=(void*)_tmp12D;_tmp128->curr_rgn=(void*)_tmp12C;_tmp128->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp128->flags).in_notreadctxt=1;(_tmp128->flags).in_lhs=_tmp12B;(_tmp128->flags).in_new=_tmp12A;(_tmp128->flags).abstract_ok=_tmp129;_tmp128->flags;});_tmp128->fnrgn=(struct _RegionHandle*)r;_tmp128;});
# 669
return Cyc_Tcenv_put_fenv(r,te,_tmp127);};};}
# 672
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp133=te->le;
if(_tmp133 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp134=*_tmp133;struct Cyc_Tcenv_Fenv _tmp135=_tmp134;struct Cyc_Tcenv_SharedFenv*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_RgnOrder_RgnPO*_tmp13F;const struct Cyc_Tcenv_Bindings*_tmp13E;struct Cyc_Tcenv_CtrlEnv*_tmp13D;void*_tmp13C;void*_tmp13B;int _tmp13A;int _tmp139;int _tmp138;_LLB0: _tmp141=_tmp135.shared;_tmp140=_tmp135.type_vars;_tmp13F=_tmp135.region_order;_tmp13E=_tmp135.locals;_tmp13D=_tmp135.ctrl_env;_tmp13C=_tmp135.capability;_tmp13B=_tmp135.curr_rgn;_tmp13A=(_tmp135.flags).in_lhs;_tmp139=(_tmp135.flags).in_new;_tmp138=(_tmp135.flags).abstract_ok;_LLB1:;{
# 678
struct Cyc_Tcenv_Fenv*_tmp136=({struct Cyc_Tcenv_Fenv*_tmp137=_region_malloc(r,sizeof(*_tmp137));_tmp137->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp141;_tmp137->type_vars=(struct Cyc_List_List*)_tmp140;_tmp137->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13F;_tmp137->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13E;_tmp137->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13D;_tmp137->capability=(void*)_tmp13C;_tmp137->curr_rgn=(void*)_tmp13B;_tmp137->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp137->flags).in_notreadctxt=0;(_tmp137->flags).in_lhs=_tmp13A;(_tmp137->flags).in_new=_tmp139;(_tmp137->flags).abstract_ok=_tmp138;_tmp137->flags;});_tmp137->fnrgn=(struct _RegionHandle*)r;_tmp137;});
# 681
return Cyc_Tcenv_put_fenv(r,te,_tmp136);};};}
# 684
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp142=te->le;
if(_tmp142 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp143=_tmp142;struct Cyc_Tcenv_Fenv*_tmp144=_tmp143;int _tmp145;_LLB3: _tmp145=(_tmp144->flags).in_notreadctxt;_LLB4:;
return _tmp145;};}
# 691
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp146=te->le;
if(_tmp146 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp147=*_tmp146;struct Cyc_Tcenv_Fenv _tmp148=_tmp147;struct Cyc_Tcenv_SharedFenv*_tmp154;struct Cyc_List_List*_tmp153;struct Cyc_RgnOrder_RgnPO*_tmp152;const struct Cyc_Tcenv_Bindings*_tmp151;struct Cyc_Tcenv_CtrlEnv*_tmp150;void*_tmp14F;void*_tmp14E;int _tmp14D;int _tmp14C;int _tmp14B;_LLB6: _tmp154=_tmp148.shared;_tmp153=_tmp148.type_vars;_tmp152=_tmp148.region_order;_tmp151=_tmp148.locals;_tmp150=_tmp148.ctrl_env;_tmp14F=_tmp148.capability;_tmp14E=_tmp148.curr_rgn;_tmp14D=(_tmp148.flags).in_notreadctxt;_tmp14C=(_tmp148.flags).in_new;_tmp14B=(_tmp148.flags).abstract_ok;_LLB7:;{
# 697
struct Cyc_Tcenv_Fenv*_tmp149=({struct Cyc_Tcenv_Fenv*_tmp14A=_region_malloc(r,sizeof(*_tmp14A));_tmp14A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp154;_tmp14A->type_vars=(struct Cyc_List_List*)_tmp153;_tmp14A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp152;_tmp14A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp151;_tmp14A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp150;_tmp14A->capability=(void*)_tmp14F;_tmp14A->curr_rgn=(void*)_tmp14E;_tmp14A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp14A->flags).in_notreadctxt=_tmp14D;(_tmp14A->flags).in_lhs=1;(_tmp14A->flags).in_new=_tmp14C;(_tmp14A->flags).abstract_ok=_tmp14B;_tmp14A->flags;});_tmp14A->fnrgn=(struct _RegionHandle*)r;_tmp14A;});
# 700
return Cyc_Tcenv_put_fenv(r,te,_tmp149);};};}
# 703
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp155=te->le;
if(_tmp155 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp156=*_tmp155;struct Cyc_Tcenv_Fenv _tmp157=_tmp156;struct Cyc_Tcenv_SharedFenv*_tmp163;struct Cyc_List_List*_tmp162;struct Cyc_RgnOrder_RgnPO*_tmp161;const struct Cyc_Tcenv_Bindings*_tmp160;struct Cyc_Tcenv_CtrlEnv*_tmp15F;void*_tmp15E;void*_tmp15D;int _tmp15C;int _tmp15B;int _tmp15A;_LLB9: _tmp163=_tmp157.shared;_tmp162=_tmp157.type_vars;_tmp161=_tmp157.region_order;_tmp160=_tmp157.locals;_tmp15F=_tmp157.ctrl_env;_tmp15E=_tmp157.capability;_tmp15D=_tmp157.curr_rgn;_tmp15C=(_tmp157.flags).in_notreadctxt;_tmp15B=(_tmp157.flags).in_new;_tmp15A=(_tmp157.flags).abstract_ok;_LLBA:;{
# 709
struct Cyc_Tcenv_Fenv*_tmp158=({struct Cyc_Tcenv_Fenv*_tmp159=_region_malloc(r,sizeof(*_tmp159));_tmp159->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp163;_tmp159->type_vars=(struct Cyc_List_List*)_tmp162;_tmp159->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp161;_tmp159->locals=(const struct Cyc_Tcenv_Bindings*)_tmp160;_tmp159->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F;_tmp159->capability=(void*)_tmp15E;_tmp159->curr_rgn=(void*)_tmp15D;_tmp159->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp159->flags).in_notreadctxt=_tmp15C;(_tmp159->flags).in_lhs=0;(_tmp159->flags).in_new=_tmp15B;(_tmp159->flags).abstract_ok=_tmp15A;_tmp159->flags;});_tmp159->fnrgn=(struct _RegionHandle*)r;_tmp159;});
# 712
return Cyc_Tcenv_put_fenv(r,te,_tmp158);};};}
# 715
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp164=te->le;
if(_tmp164 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp165=_tmp164;struct Cyc_Tcenv_Fenv*_tmp166=_tmp165;int _tmp167;_LLBC: _tmp167=(_tmp166->flags).in_lhs;_LLBD:;
return _tmp167;};}
# 723
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 725
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp168;_push_handler(& _tmp168);{int _tmp16A=0;if(setjmp(_tmp168.handler))_tmp16A=1;if(!_tmp16A){
{void*_tmp16B=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp16C=_tmp16B;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp16C)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp16C)->f1)){case 4: _LLBF: _LLC0:
# 729
({void*_tmp16D=0;Cyc_Tcutil_warn(loc,({const char*_tmp16E="pattern variable hides local";_tag_dyneither(_tmp16E,sizeof(char),29);}),_tag_dyneither(_tmp16D,sizeof(void*),0));});goto _LLBE;case 1: _LLC1: _LLC2:
# 731
({void*_tmp16F=0;Cyc_Tcutil_warn(loc,({const char*_tmp170="pattern variable hides global";_tag_dyneither(_tmp170,sizeof(char),30);}),_tag_dyneither(_tmp16F,sizeof(void*),0));});goto _LLBE;case 5: _LLC3: _LLC4:
# 733
({void*_tmp171=0;Cyc_Tcutil_warn(loc,({const char*_tmp172="pattern variable hides pattern variable";_tag_dyneither(_tmp172,sizeof(char),40);}),_tag_dyneither(_tmp171,sizeof(void*),0));});goto _LLBE;case 3: _LLC5: _LLC6:
# 735
({void*_tmp173=0;Cyc_Tcutil_warn(loc,({const char*_tmp174="pattern variable hides parameter";_tag_dyneither(_tmp174,sizeof(char),33);}),_tag_dyneither(_tmp173,sizeof(void*),0));});goto _LLBE;default: goto _LLC7;}else{_LLC7: _LLC8:
 goto _LLBE;}_LLBE:;}
# 727
;_pop_handler();}else{void*_tmp169=(void*)_exn_thrown;void*_tmp175=_tmp169;void*_tmp176;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp175)->tag == Cyc_Dict_Absent){_LLCA: _LLCB:
# 738
 goto _LLC9;}else{_LLCC: _tmp176=_tmp175;_LLCD:(int)_rethrow(_tmp176);}_LLC9:;}};}{
struct _dyneither_ptr*_tmp177=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmp178=
# 742
Cyc_Tcenv_get_fenv(te,({const char*_tmp189="add_pat_var";_tag_dyneither(_tmp189,sizeof(char),12);}));
# 740
struct Cyc_Tcenv_Fenv*_tmp179=_tmp178;struct Cyc_Tcenv_SharedFenv*_tmp188;struct Cyc_List_List*_tmp187;struct Cyc_RgnOrder_RgnPO*_tmp186;const struct Cyc_Tcenv_Bindings*_tmp185;struct Cyc_Tcenv_CtrlEnv*_tmp184;void*_tmp183;void*_tmp182;struct Cyc_Tcenv_FenvFlags _tmp181;_LLCF: _tmp188=_tmp179->shared;_tmp187=_tmp179->type_vars;_tmp186=_tmp179->region_order;_tmp185=_tmp179->locals;_tmp184=_tmp179->ctrl_env;_tmp183=_tmp179->capability;_tmp182=_tmp179->curr_rgn;_tmp181=_tmp179->flags;_LLD0:;{
# 743
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp17A=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp180;_tmp180.tag=5;_tmp180.f1=vd;_tmp180;});_tmp17F;});
struct Cyc_Tcenv_Bindings*_tmp17B=({struct Cyc_Tcenv_Bindings*_tmp17E=_region_malloc(r,sizeof(*_tmp17E));_tmp17E->v=_tmp177;_tmp17E->b=(void*)_tmp17A;_tmp17E->tl=_tmp185;_tmp17E;});
struct Cyc_Tcenv_Fenv*_tmp17C=({struct Cyc_Tcenv_Fenv*_tmp17D=_region_malloc(r,sizeof(*_tmp17D));_tmp17D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp188;_tmp17D->type_vars=(struct Cyc_List_List*)_tmp187;_tmp17D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp186;_tmp17D->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp17B);_tmp17D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp184;_tmp17D->capability=(void*)_tmp183;_tmp17D->curr_rgn=(void*)_tmp182;_tmp17D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp181;_tmp17D->fnrgn=(struct _RegionHandle*)r;_tmp17D;});
# 747
return Cyc_Tcenv_put_fenv(r,te,_tmp17C);};};}
# 750
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp18A=te->le;
struct _tuple0*_tmp18B=q;union Cyc_Absyn_Nmspace _tmp1A5;struct _dyneither_ptr*_tmp1A4;_LLD2: _tmp1A5=_tmp18B->f1;_tmp1A4=_tmp18B->f2;_LLD3:;{
union Cyc_Absyn_Nmspace _tmp18C=_tmp1A5;switch((_tmp18C.Rel_n).tag){case 4: _LLD5: _LLD6:
# 755
 if(_tmp18A == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp18D=_cycalloc_atomic(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Dict_Absent_exn_struct _tmp18E;_tmp18E.tag=Cyc_Dict_Absent;_tmp18E;});_tmp18D;}));
goto _LLD8;case 1: if((_tmp18C.Rel_n).val == 0){_LLD7: if(_tmp18A != 0){_LLD8: {
# 758
struct Cyc_Tcenv_Fenv*_tmp18F=_tmp18A;struct Cyc_Tcenv_Fenv*_tmp190=_tmp18F;const struct Cyc_Tcenv_Bindings*_tmp199;_LLDC: _tmp199=_tmp190->locals;_LLDD:;{
# 760
struct _handler_cons _tmp191;_push_handler(& _tmp191);{int _tmp193=0;if(setjmp(_tmp191.handler))_tmp193=1;if(!_tmp193){{void*_tmp196=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp194=_region_malloc(r,sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp195;_tmp195.tag=0;_tmp195.f1=Cyc_Tcenv_lookup_binding(_tmp199,_tmp1A4);_tmp195;});_tmp194;});_npop_handler(0);return _tmp196;};_pop_handler();}else{void*_tmp192=(void*)_exn_thrown;void*_tmp197=_tmp192;void*_tmp198;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp197)->tag == Cyc_Tcenv_NoBinding){_LLDF: _LLE0:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LLE1: _tmp198=_tmp197;_LLE2:(int)_rethrow(_tmp198);}_LLDE:;}};};}}else{goto _LLD9;}}else{goto _LLD9;}default: _LLD9: _LLDA: {
# 763
struct _handler_cons _tmp19A;_push_handler(& _tmp19A);{int _tmp19C=0;if(setjmp(_tmp19A.handler))_tmp19C=1;if(!_tmp19C){{void*_tmp19D=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp19D;};_pop_handler();}else{void*_tmp19B=(void*)_exn_thrown;void*_tmp19E=_tmp19B;void*_tmp1A3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp19E)->tag == Cyc_Dict_Absent){_LLE4: _LLE5:
# 765
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp19F=_region_malloc(r,sizeof(*_tmp19F));_tmp19F[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1[0]=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmp1A2;_tmp1A2.tag=0;_tmp1A2.f1=q;_tmp1A2;});_tmp1A1;});_tmp1A0;});_tmp19F;});}else{_LLE6: _tmp1A3=_tmp19E;_LLE7:(int)_rethrow(_tmp1A3);}_LLE3:;}};}}_LLD4:;};}
# 771
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 773
struct Cyc_Tcenv_Fenv*_tmp1A6=Cyc_Tcenv_get_fenv(te,({const char*_tmp1AB="process_fallthru";_tag_dyneither(_tmp1AB,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp1A7=_tmp1A6;struct Cyc_Tcenv_CtrlEnv*_tmp1AA;_LLE9: _tmp1AA=_tmp1A7->ctrl_env;_LLEA:;{
const struct _tuple10*_tmp1A8=_tmp1AA->fallthru_clause;
if(_tmp1A8 != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9[0]=(*_tmp1A8).f1;_tmp1A9;});
# 778
return _tmp1A8;};}
# 781
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 785
struct Cyc_Tcenv_Fenv*_tmp1AC=
# 787
Cyc_Tcenv_get_fenv(te,({const char*_tmp1BF="set_fallthru";_tag_dyneither(_tmp1BF,sizeof(char),13);}));
# 785
struct Cyc_Tcenv_Fenv*_tmp1AD=_tmp1AC;struct Cyc_Tcenv_SharedFenv*_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_RgnOrder_RgnPO*_tmp1BC;const struct Cyc_Tcenv_Bindings*_tmp1BB;struct Cyc_Tcenv_CtrlEnv*_tmp1BA;void*_tmp1B9;void*_tmp1B8;struct Cyc_Tcenv_FenvFlags _tmp1B7;_LLEC: _tmp1BE=_tmp1AD->shared;_tmp1BD=_tmp1AD->type_vars;_tmp1BC=_tmp1AD->region_order;_tmp1BB=_tmp1AD->locals;_tmp1BA=_tmp1AD->ctrl_env;_tmp1B9=_tmp1AD->capability;_tmp1B8=_tmp1AD->curr_rgn;_tmp1B7=_tmp1AD->flags;_LLED:;{
# 788
struct Cyc_Tcenv_CtrlEnv*_tmp1AE=_tmp1BA;const struct _tuple10*_tmp1B6;_LLEF: _tmp1B6=_tmp1AE->fallthru_clause;_LLF0:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp1AF->tl=ft_typ;_tmp1AF;});}{
struct Cyc_List_List*_tmp1B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp1B1=({struct Cyc_Tcenv_CtrlEnv*_tmp1B4=_region_malloc(r,sizeof(*_tmp1B4));_tmp1B4->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->f1=clause;_tmp1B5->f2=new_tvs;_tmp1B5->f3=(const struct Cyc_Tcenv_CList*)_tmp1B0;_tmp1B5;});_tmp1B4;});
# 795
struct Cyc_Tcenv_Fenv*_tmp1B2=({struct Cyc_Tcenv_Fenv*_tmp1B3=_region_malloc(r,sizeof(*_tmp1B3));_tmp1B3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1BE;_tmp1B3->type_vars=(struct Cyc_List_List*)_tmp1BD;_tmp1B3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1BC;_tmp1B3->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1BB;_tmp1B3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1B1;_tmp1B3->capability=(void*)_tmp1B9;_tmp1B3->curr_rgn=(void*)_tmp1B8;_tmp1B3->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1B7;_tmp1B3->fnrgn=(struct _RegionHandle*)r;_tmp1B3;});
# 799
return Cyc_Tcenv_put_fenv(r,te,_tmp1B2);};};};}
# 802
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1C2="clear_fallthru";_tag_dyneither(_tmp1C2,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp1C0=fe;struct Cyc_Tcenv_CtrlEnv*_tmp1C1;_LLF2: _tmp1C1=_tmp1C0->ctrl_env;_LLF3:;
_tmp1C1->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 809
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp1C3=_region_malloc(r2,sizeof(*_tmp1C3));_tmp1C3->ns=te->ns;_tmp1C3->ae=te->ae;_tmp1C3->le=0;_tmp1C3->allow_valueof=1;_tmp1C3->in_extern_c_include=te->in_extern_c_include;_tmp1C3;});{
struct Cyc_Tcenv_Fenv _tmp1C4=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp1C5=_tmp1C4;struct Cyc_Tcenv_SharedFenv*_tmp1D0;struct Cyc_List_List*_tmp1CF;struct Cyc_RgnOrder_RgnPO*_tmp1CE;const struct Cyc_Tcenv_Bindings*_tmp1CD;struct Cyc_Tcenv_CtrlEnv*_tmp1CC;void*_tmp1CB;void*_tmp1CA;struct Cyc_Tcenv_FenvFlags _tmp1C9;_LLF5: _tmp1D0=_tmp1C5.shared;_tmp1CF=_tmp1C5.type_vars;_tmp1CE=_tmp1C5.region_order;_tmp1CD=_tmp1C5.locals;_tmp1CC=_tmp1C5.ctrl_env;_tmp1CB=_tmp1C5.capability;_tmp1CA=_tmp1C5.curr_rgn;_tmp1C9=_tmp1C5.flags;_LLF6:;{
struct Cyc_Tcenv_Fenv*_tmp1C6=({struct Cyc_Tcenv_Fenv*_tmp1C8=_region_malloc(r2,sizeof(*_tmp1C8));_tmp1C8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1D0;_tmp1C8->type_vars=(struct Cyc_List_List*)_tmp1CF;_tmp1C8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1CE;_tmp1C8->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1CD;_tmp1C8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1CC;_tmp1C8->capability=(void*)_tmp1CB;_tmp1C8->curr_rgn=(void*)_tmp1CA;_tmp1C8->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1C9;_tmp1C8->fnrgn=(struct _RegionHandle*)r2;_tmp1C8;});
return({struct Cyc_Tcenv_Tenv*_tmp1C7=_region_malloc(r2,sizeof(*_tmp1C7));_tmp1C7->ns=te->ns;_tmp1C7->ae=te->ae;_tmp1C7->le=_tmp1C6;_tmp1C7->allow_valueof=1;_tmp1C7->in_extern_c_include=te->in_extern_c_include;_tmp1C7;});};};}
# 817
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp1D1=_region_malloc(r2,sizeof(*_tmp1D1));_tmp1D1->ns=te->ns;_tmp1D1->ae=te->ae;_tmp1D1->le=0;_tmp1D1->allow_valueof=te->allow_valueof;_tmp1D1->in_extern_c_include=1;_tmp1D1;});{
struct Cyc_Tcenv_Fenv _tmp1D2=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp1D3=_tmp1D2;struct Cyc_Tcenv_SharedFenv*_tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_RgnOrder_RgnPO*_tmp1DC;const struct Cyc_Tcenv_Bindings*_tmp1DB;struct Cyc_Tcenv_CtrlEnv*_tmp1DA;void*_tmp1D9;void*_tmp1D8;struct Cyc_Tcenv_FenvFlags _tmp1D7;_LLF8: _tmp1DE=_tmp1D3.shared;_tmp1DD=_tmp1D3.type_vars;_tmp1DC=_tmp1D3.region_order;_tmp1DB=_tmp1D3.locals;_tmp1DA=_tmp1D3.ctrl_env;_tmp1D9=_tmp1D3.capability;_tmp1D8=_tmp1D3.curr_rgn;_tmp1D7=_tmp1D3.flags;_LLF9:;{
struct Cyc_Tcenv_Fenv*_tmp1D4=({struct Cyc_Tcenv_Fenv*_tmp1D6=_region_malloc(r2,sizeof(*_tmp1D6));_tmp1D6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1DE;_tmp1D6->type_vars=(struct Cyc_List_List*)_tmp1DD;_tmp1D6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1DC;_tmp1D6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1DB;_tmp1D6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1DA;_tmp1D6->capability=(void*)_tmp1D9;_tmp1D6->curr_rgn=(void*)_tmp1D8;_tmp1D6->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1D7;_tmp1D6->fnrgn=(struct _RegionHandle*)r2;_tmp1D6;});
return({struct Cyc_Tcenv_Tenv*_tmp1D5=_region_malloc(r2,sizeof(*_tmp1D5));_tmp1D5->ns=te->ns;_tmp1D5->ae=te->ae;_tmp1D5->le=_tmp1D4;_tmp1D5->allow_valueof=te->allow_valueof;_tmp1D5->in_extern_c_include=1;_tmp1D5;});};};}
# 825
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 828
struct Cyc_Tcenv_Fenv _tmp1DF=*
# 830
Cyc_Tcenv_get_fenv(te,({const char*_tmp1F4="add_region";_tag_dyneither(_tmp1F4,sizeof(char),11);}));
# 828
struct Cyc_Tcenv_Fenv _tmp1E0=_tmp1DF;struct Cyc_Tcenv_SharedFenv*_tmp1F3;struct Cyc_List_List*_tmp1F2;struct Cyc_RgnOrder_RgnPO*_tmp1F1;const struct Cyc_Tcenv_Bindings*_tmp1F0;struct Cyc_Tcenv_CtrlEnv*_tmp1EF;void*_tmp1EE;void*_tmp1ED;struct Cyc_Tcenv_FenvFlags _tmp1EC;_LLFB: _tmp1F3=_tmp1E0.shared;_tmp1F2=_tmp1E0.type_vars;_tmp1F1=_tmp1E0.region_order;_tmp1F0=_tmp1E0.locals;_tmp1EF=_tmp1E0.ctrl_env;_tmp1EE=_tmp1E0.capability;_tmp1ED=_tmp1E0.curr_rgn;_tmp1EC=_tmp1E0.flags;_LLFC:;
# 831
{void*_tmp1E1=Cyc_Tcutil_compress(rgn);void*_tmp1E2=_tmp1E1;struct Cyc_Absyn_Tvar*_tmp1E3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1E2)->tag == 2){_LLFE: _tmp1E3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1E2)->f1;_LLFF:
# 833
 _tmp1F1=Cyc_RgnOrder_add_youngest(_tmp1F3->frgn,_tmp1F1,_tmp1E3,resetable,opened);
# 835
goto _LLFD;}else{_LL100: _LL101:
 goto _LLFD;}_LLFD:;}
# 838
_tmp1EE=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1E5;_tmp1E5.tag=24;_tmp1E5.f1=({struct Cyc_List_List*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1E9;_tmp1E9.tag=23;_tmp1E9.f1=rgn;_tmp1E9;});_tmp1E8;});_tmp1E6->tl=({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=_tmp1EE;_tmp1E7->tl=0;_tmp1E7;});_tmp1E6;});_tmp1E5;});_tmp1E4;});{
# 840
struct Cyc_Tcenv_Fenv*_tmp1EA=({struct Cyc_Tcenv_Fenv*_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F3;_tmp1EB->type_vars=(struct Cyc_List_List*)_tmp1F2;_tmp1EB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1F1;_tmp1EB->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1F0;_tmp1EB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1EF;_tmp1EB->capability=(void*)_tmp1EE;_tmp1EB->curr_rgn=(void*)_tmp1ED;_tmp1EB->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1EC;_tmp1EB->fnrgn=(struct _RegionHandle*)r;_tmp1EB;});
# 843
return Cyc_Tcenv_put_fenv(r,te,_tmp1EA);};}
# 846
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 848
struct Cyc_Tcenv_Fenv _tmp1F5=*
# 850
Cyc_Tcenv_get_fenv(te,({const char*_tmp20B="new_named_block";_tag_dyneither(_tmp20B,sizeof(char),16);}));
# 848
struct Cyc_Tcenv_Fenv _tmp1F6=_tmp1F5;struct Cyc_Tcenv_SharedFenv*_tmp20A;struct Cyc_List_List*_tmp209;struct Cyc_RgnOrder_RgnPO*_tmp208;const struct Cyc_Tcenv_Bindings*_tmp207;struct Cyc_Tcenv_CtrlEnv*_tmp206;void*_tmp205;void*_tmp204;struct Cyc_Tcenv_FenvFlags _tmp203;_LL103: _tmp20A=_tmp1F6.shared;_tmp209=_tmp1F6.type_vars;_tmp208=_tmp1F6.region_order;_tmp207=_tmp1F6.locals;_tmp206=_tmp1F6.ctrl_env;_tmp205=_tmp1F6.capability;_tmp204=_tmp1F6.curr_rgn;_tmp203=_tmp1F6.flags;_LL104:;{
# 852
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp202="new_block";_tag_dyneither(_tmp202,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp201;_tmp201.tag=2;_tmp201.f1=block_rgn;_tmp201;});_tmp200;});
_tmp209=({struct Cyc_List_List*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->hd=block_rgn;_tmp1F7->tl=_tmp209;_tmp1F7;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp209);
_tmp208=Cyc_RgnOrder_add_youngest(_tmp20A->frgn,_tmp208,block_rgn,0,0);
_tmp205=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1F9;_tmp1F9.tag=24;_tmp1F9.f1=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1FD;_tmp1FD.tag=23;_tmp1FD.f1=block_typ;_tmp1FD;});_tmp1FC;});_tmp1FA->tl=({struct Cyc_List_List*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->hd=_tmp205;_tmp1FB->tl=0;_tmp1FB;});_tmp1FA;});_tmp1F9;});_tmp1F8;});
_tmp204=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp1FE=({struct Cyc_Tcenv_Fenv*_tmp1FF=_region_malloc(r,sizeof(*_tmp1FF));_tmp1FF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp20A;_tmp1FF->type_vars=(struct Cyc_List_List*)_tmp209;_tmp1FF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp208;_tmp1FF->locals=(const struct Cyc_Tcenv_Bindings*)_tmp207;_tmp1FF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp206;_tmp1FF->capability=(void*)_tmp205;_tmp1FF->curr_rgn=(void*)_tmp204;_tmp1FF->flags=(struct Cyc_Tcenv_FenvFlags)_tmp203;_tmp1FF->fnrgn=(struct _RegionHandle*)r;_tmp1FF;});
# 862
return Cyc_Tcenv_put_fenv(r,te,_tmp1FE);};};}
# 865
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 867
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 869
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 875
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 879
struct Cyc_Tcenv_Fenv _tmp20D=*
# 881
Cyc_Tcenv_get_fenv(te,({const char*_tmp219="new_outlives_constraints";_tag_dyneither(_tmp219,sizeof(char),25);}));
# 879
struct Cyc_Tcenv_Fenv _tmp20E=_tmp20D;struct Cyc_Tcenv_SharedFenv*_tmp218;struct Cyc_List_List*_tmp217;struct Cyc_RgnOrder_RgnPO*_tmp216;const struct Cyc_Tcenv_Bindings*_tmp215;struct Cyc_Tcenv_CtrlEnv*_tmp214;void*_tmp213;void*_tmp212;struct Cyc_Tcenv_FenvFlags _tmp211;_LL106: _tmp218=_tmp20E.shared;_tmp217=_tmp20E.type_vars;_tmp216=_tmp20E.region_order;_tmp215=_tmp20E.locals;_tmp214=_tmp20E.ctrl_env;_tmp213=_tmp20E.capability;_tmp212=_tmp20E.curr_rgn;_tmp211=_tmp20E.flags;_LL107:;
# 882
for(0;cs != 0;cs=cs->tl){
_tmp216=
Cyc_RgnOrder_add_outlives_constraint(_tmp218->frgn,_tmp216,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 887
struct Cyc_Tcenv_Fenv*_tmp20F=({struct Cyc_Tcenv_Fenv*_tmp210=_region_malloc(r,sizeof(*_tmp210));_tmp210->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp218;_tmp210->type_vars=(struct Cyc_List_List*)_tmp217;_tmp210->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp216;_tmp210->locals=(const struct Cyc_Tcenv_Bindings*)_tmp215;_tmp210->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp214;_tmp210->capability=(void*)_tmp213;_tmp210->curr_rgn=(void*)_tmp212;_tmp210->flags=(struct Cyc_Tcenv_FenvFlags)_tmp211;_tmp210->fnrgn=(struct _RegionHandle*)r;_tmp210;});
# 890
return Cyc_Tcenv_put_fenv(r,te,_tmp20F);};}
# 893
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 901
void*_tmp21A=Cyc_Tcutil_compress(r1);
void*_tmp21B=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp21C=Cyc_Tcutil_typ_kind(_tmp21A);
struct Cyc_Absyn_Kind*_tmp21D=Cyc_Tcutil_typ_kind(_tmp21B);
# 907
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp21C,_tmp21D);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp21D,_tmp21C);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp223;_tmp223.tag=0;_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp21D));({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0;_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp21C));({struct Cyc_String_pa_PrintArg_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21B));({struct Cyc_String_pa_PrintArg_struct _tmp220;_tmp220.tag=0;_tmp220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21A));({void*_tmp21E[4]={& _tmp220,& _tmp221,& _tmp222,& _tmp223};Cyc_Tcutil_terr(loc,({const char*_tmp21F="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp21F,sizeof(char),82);}),_tag_dyneither(_tmp21E,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 914
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp224=Cyc_Tcutil_swap_kind(_tmp21B,Cyc_Tcutil_kind_to_bound(_tmp21C));struct _tuple11 _tmp225=_tmp224;struct Cyc_Absyn_Tvar*_tmp228;void*_tmp227;_LL109: _tmp228=_tmp225.f1;_tmp227=_tmp225.f2;_LL10A:;
*oldtv=({struct _tuple11*_tmp226=_region_malloc(r,sizeof(*_tmp226));_tmp226->f1=_tmp228;_tmp226->f2=_tmp227;_tmp226;});}else{
# 918
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp229=Cyc_Tcutil_swap_kind(_tmp21A,Cyc_Tcutil_kind_to_bound(_tmp21D));struct _tuple11 _tmp22A=_tmp229;struct Cyc_Absyn_Tvar*_tmp22D;void*_tmp22C;_LL10C: _tmp22D=_tmp22A.f1;_tmp22C=_tmp22A.f2;_LL10D:;
*oldtv=({struct _tuple11*_tmp22B=_region_malloc(r,sizeof(*_tmp22B));_tmp22B->f1=_tmp22D;_tmp22B->f2=_tmp22C;_tmp22B;});}}}{
# 924
struct _RegionHandle*_tmp22E=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp22F=0;
if((_tmp21A != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp21A != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp21A != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp233;_tmp233.tag=23;_tmp233.f1=_tmp21A;_tmp233;});_tmp232;});
_tmp22F=({struct Cyc_List_List*_tmp230=_region_malloc(_tmp22E,sizeof(*_tmp230));_tmp230->hd=({struct _tuple13*_tmp231=_region_malloc(_tmp22E,sizeof(*_tmp231));_tmp231->f1=eff1;_tmp231->f2=_tmp21B;_tmp231;});_tmp230->tl=_tmp22F;_tmp230;});}
# 930
if((_tmp21B != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp21B != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp21B != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp237;_tmp237.tag=23;_tmp237.f1=_tmp21B;_tmp237;});_tmp236;});
_tmp22F=({struct Cyc_List_List*_tmp234=_region_malloc(_tmp22E,sizeof(*_tmp234));_tmp234->hd=({struct _tuple13*_tmp235=_region_malloc(_tmp22E,sizeof(*_tmp235));_tmp235->f1=eff2;_tmp235->f2=_tmp21A;_tmp235;});_tmp234->tl=_tmp22F;_tmp234;});}
# 935
return Cyc_Tcenv_new_outlives_constraints(_tmp22E,te,_tmp22F,loc);};}
# 938
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp238=te->le;
if(_tmp238 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp238;
struct Cyc_Tcenv_Fenv*_tmp239=fe;void*_tmp23A;_LL10F: _tmp23A=_tmp239->curr_rgn;_LL110:;
return _tmp23A;};}
# 949
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp243="check_rgn_accessible";_tag_dyneither(_tmp243,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp23B=fe;void*_tmp242;struct Cyc_RgnOrder_RgnPO*_tmp241;_LL112: _tmp242=_tmp23B->capability;_tmp241=_tmp23B->region_order;_LL113:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp242) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp242))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp241,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp23D;_tmp23D.tag=23;_tmp23D.f1=rgn;_tmp23D;});_tmp23C;}),_tmp242))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp240;_tmp240.tag=0;_tmp240.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp23E[1]={& _tmp240};Cyc_Tcutil_terr(loc,({const char*_tmp23F="Expression accesses unavailable region %s";_tag_dyneither(_tmp23F,sizeof(char),42);}),_tag_dyneither(_tmp23E,sizeof(void*),1));});});}
# 961
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp244=
Cyc_Tcenv_get_fenv(te,({const char*_tmp24F="check_rgn_resetable";_tag_dyneither(_tmp24F,sizeof(char),20);}));
# 963
struct Cyc_Tcenv_Fenv*_tmp245=_tmp244;struct Cyc_RgnOrder_RgnPO*_tmp24E;_LL115: _tmp24E=_tmp245->region_order;_LL116:;{
# 965
void*_tmp246=Cyc_Tcutil_compress(rgn);void*_tmp247=_tmp246;struct Cyc_Absyn_Tvar*_tmp24D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp247)->tag == 2){_LL118: _tmp24D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp247)->f1;_LL119:
# 967
 if(!Cyc_RgnOrder_is_region_resetable(_tmp24E,_tmp24D))
({struct Cyc_String_pa_PrintArg_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp248[1]={& _tmp24A};Cyc_Tcutil_terr(loc,({const char*_tmp249="Region %s is not resetable";_tag_dyneither(_tmp249,sizeof(char),27);}),_tag_dyneither(_tmp248,sizeof(void*),1));});});
return;}else{_LL11A: _LL11B:
({void*_tmp24B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24C="check_rgn_resetable";_tag_dyneither(_tmp24C,sizeof(char),20);}),_tag_dyneither(_tmp24B,sizeof(void*),0));});}_LL117:;};};}
# 977
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp250=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 982
if(_tmp250 == 0){
void*_tmp251=rt_a;switch(*((int*)_tmp251)){case 22: _LL11D: _LL11E:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LL11F: _LL120:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LL121: _LL122:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL123: _LL124:
 return 0;}_LL11C:;}{
# 990
struct Cyc_Tcenv_Fenv*fe=_tmp250;
struct Cyc_Tcenv_Fenv*_tmp252=fe;struct Cyc_RgnOrder_RgnPO*_tmp256;_LL126: _tmp256=_tmp252->region_order;_LL127:;{
int _tmp253=Cyc_RgnOrder_effect_outlives(_tmp256,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp255;_tmp255.tag=23;_tmp255.f1=rt_a;_tmp255;});_tmp254;}),rt_b);
# 996
return _tmp253;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1001
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp257=
Cyc_Tcenv_get_fenv(te,({const char*_tmp25E="check_effect_accessible";_tag_dyneither(_tmp25E,sizeof(char),24);}));
# 1002
struct Cyc_Tcenv_Fenv*_tmp258=_tmp257;void*_tmp25D;struct Cyc_RgnOrder_RgnPO*_tmp25C;struct Cyc_Tcenv_SharedFenv*_tmp25B;_LL129: _tmp25D=_tmp258->capability;_tmp25C=_tmp258->region_order;_tmp25B=_tmp258->shared;_LL12A:;
# 1004
if(Cyc_Tcutil_subset_effect(0,eff,_tmp25D))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp25C,eff,_tmp25D))
return;{
struct _RegionHandle*frgn=_tmp25B->frgn;
_tmp25B->delayed_effect_checks=({struct Cyc_List_List*_tmp259=_region_malloc(frgn,sizeof(*_tmp259));_tmp259->hd=({struct _tuple14*_tmp25A=_region_malloc(frgn,sizeof(*_tmp25A));_tmp25A->f1=_tmp25D;_tmp25A->f2=eff;_tmp25A->f3=_tmp25C;_tmp25A->f4=loc;_tmp25A;});_tmp259->tl=_tmp25B->delayed_effect_checks;_tmp259;});};}
# 1013
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25F=
Cyc_Tcenv_get_fenv(te,({const char*_tmp26D="check_delayed_effects";_tag_dyneither(_tmp26D,sizeof(char),22);}));
# 1014
struct Cyc_Tcenv_Fenv*_tmp260=_tmp25F;struct Cyc_Tcenv_SharedFenv*_tmp26C;_LL12C: _tmp26C=_tmp260->shared;_LL12D:;{
# 1016
struct Cyc_List_List*_tmp261=_tmp26C->delayed_effect_checks;
for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
struct _tuple14*_tmp262=(struct _tuple14*)_tmp261->hd;struct _tuple14*_tmp263=_tmp262;void*_tmp26B;void*_tmp26A;struct Cyc_RgnOrder_RgnPO*_tmp269;unsigned int _tmp268;_LL12F: _tmp26B=_tmp263->f1;_tmp26A=_tmp263->f2;_tmp269=_tmp263->f3;_tmp268=_tmp263->f4;_LL130:;
if(Cyc_Tcutil_subset_effect(1,_tmp26A,_tmp26B))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp269,_tmp26A,_tmp26B))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp267;_tmp267.tag=0;_tmp267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp26A));({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp26B));({void*_tmp264[2]={& _tmp266,& _tmp267};Cyc_Tcutil_terr(_tmp268,({const char*_tmp265="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp265,sizeof(char),51);}),_tag_dyneither(_tmp264,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1032
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1034
struct Cyc_Tcenv_Fenv*_tmp26E=te->le;
if(_tmp26E == 0){
# 1037
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp270;_tmp270.tag=23;_tmp270.f1=(*((struct _tuple13*)po->hd)).f2;_tmp270;});_tmp26F;}),Cyc_Absyn_empty_effect))
({void*_tmp271=0;Cyc_Tcutil_terr(loc,({const char*_tmp272="the required region ordering is not satisfied here";_tag_dyneither(_tmp272,sizeof(char),51);}),_tag_dyneither(_tmp271,sizeof(void*),0));});}
return;}{
# 1043
struct Cyc_Tcenv_Fenv*_tmp273=_tmp26E;struct Cyc_Tcenv_Fenv*_tmp274=_tmp273;struct Cyc_RgnOrder_RgnPO*_tmp278;struct Cyc_Tcenv_SharedFenv*_tmp277;_LL132: _tmp278=_tmp274->region_order;_tmp277=_tmp274->shared;_LL133:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp278,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp277->delayed_constraint_checks=({struct Cyc_List_List*_tmp275=_region_malloc(_tmp277->frgn,sizeof(*_tmp275));_tmp275->hd=({struct _tuple15*_tmp276=_region_malloc(_tmp277->frgn,sizeof(*_tmp276));_tmp276->f1=_tmp278;_tmp276->f2=po;_tmp276->f3=loc;_tmp276;});_tmp275->tl=_tmp277->delayed_constraint_checks;_tmp275;});};}
# 1050
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp279=
Cyc_Tcenv_get_fenv(te,({const char*_tmp284="check_delayed_constraints";_tag_dyneither(_tmp284,sizeof(char),26);}));
# 1051
struct Cyc_Tcenv_Fenv*_tmp27A=_tmp279;struct Cyc_Tcenv_SharedFenv*_tmp283;_LL135: _tmp283=_tmp27A->shared;_LL136:;{
# 1053
struct Cyc_List_List*_tmp27B=_tmp283->delayed_constraint_checks;
for(0;_tmp27B != 0;_tmp27B=_tmp27B->tl){
struct _tuple15*_tmp27C=(struct _tuple15*)_tmp27B->hd;struct _tuple15*_tmp27D=_tmp27C;struct Cyc_RgnOrder_RgnPO*_tmp282;struct Cyc_List_List*_tmp281;unsigned int _tmp280;_LL138: _tmp282=_tmp27D->f1;_tmp281=_tmp27D->f2;_tmp280=_tmp27D->f3;_LL139:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp282,_tmp281,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp27E=0;Cyc_Tcutil_terr(_tmp280,({const char*_tmp27F="the required region ordering is not satisfied here";_tag_dyneither(_tmp27F,sizeof(char),51);}),_tag_dyneither(_tmp27E,sizeof(void*),0));});}};}
# 1061
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->name=({struct _dyneither_ptr*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp2B7[1]={& _tmp2B9};Cyc_aprintf(({const char*_tmp2B8="`%s";_tag_dyneither(_tmp2B8,sizeof(char),4);}),_tag_dyneither(_tmp2B7,sizeof(void*),1));});});_tmp2B6;});_tmp2B5->identity=
# 1065
Cyc_Tcutil_new_tvar_id();_tmp2B5->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp2B5;});
struct Cyc_List_List*_tmp285=({struct Cyc_List_List*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->hd=rgn0;_tmp2B4->tl=fd->tvs;_tmp2B4;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp285);{
# 1069
struct Cyc_RgnOrder_RgnPO*_tmp286=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1071
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2B3;_tmp2B3.tag=2;_tmp2B3.f1=rgn0;_tmp2B3;});_tmp2B2;});
struct Cyc_List_List*_tmp287=0;
{struct Cyc_List_List*_tmp288=fd->args;for(0;_tmp288 != 0;_tmp288=_tmp288->tl){
struct Cyc_Absyn_Vardecl*_tmp289=({struct Cyc_Absyn_Vardecl*_tmp291=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp291[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp292;_tmp292.sc=Cyc_Absyn_Public;_tmp292.name=({struct _tuple0*_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293->f1=Cyc_Absyn_Loc_n;_tmp293->f2=(*((struct _tuple8*)_tmp288->hd)).f1;_tmp293;});_tmp292.tq=(*((struct _tuple8*)_tmp288->hd)).f2;_tmp292.type=(*((struct _tuple8*)_tmp288->hd)).f3;_tmp292.initializer=0;_tmp292.rgn=param_rgn;_tmp292.attributes=0;_tmp292.escapes=0;_tmp292;});_tmp291;});
# 1081
_tmp287=({struct Cyc_List_List*_tmp28A=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp28A[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp28B;_tmp28B.hd=_tmp289;_tmp28B.tl=_tmp287;_tmp28B;});_tmp28A;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp28C=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp290;_tmp290.tag=3;_tmp290.f1=_tmp289;_tmp290;});_tmp28F;});
struct _dyneither_ptr*_tmp28D=(*((struct _tuple8*)_tmp288->hd)).f1;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp28E=_region_malloc(r,sizeof(*_tmp28E));_tmp28E->v=_tmp28D;_tmp28E->b=(void*)_tmp28C;_tmp28E->tl=locals;_tmp28E;});};}}
# 1086
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp294=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp295=_tmp294;struct _dyneither_ptr*_tmp2A6;struct Cyc_Absyn_Tqual _tmp2A5;void*_tmp2A4;int _tmp2A3;_LL13B: _tmp2A6=_tmp295.name;_tmp2A5=_tmp295.tq;_tmp2A4=_tmp295.type;_tmp2A3=_tmp295.inject;_LL13C:;
if(_tmp2A6 != 0){
void*_tmp296=Cyc_Absyn_dyneither_typ(_tmp2A4,param_rgn,_tmp2A5,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp297=({struct Cyc_Absyn_Vardecl*_tmp29E=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp29E[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp29F;_tmp29F.sc=Cyc_Absyn_Public;_tmp29F.name=({struct _tuple0*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->f1=Cyc_Absyn_Loc_n;_tmp2A0->f2=_tmp2A6;_tmp2A0;});_tmp29F.tq=
# 1092
Cyc_Absyn_empty_tqual(0);_tmp29F.type=_tmp296;_tmp29F.initializer=0;_tmp29F.rgn=param_rgn;_tmp29F.attributes=0;_tmp29F.escapes=0;_tmp29F;});_tmp29E;});
# 1097
_tmp287=({struct Cyc_List_List*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->hd=_tmp297;_tmp298->tl=_tmp287;_tmp298;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp299=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp29D;_tmp29D.tag=3;_tmp29D.f1=_tmp297;_tmp29D;});_tmp29C;});
struct _dyneither_ptr*_tmp29A=_tmp2A6;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp29B=_region_malloc(r,sizeof(*_tmp29B));_tmp29B->v=_tmp29A;_tmp29B->b=(void*)_tmp299;_tmp29B->tl=locals;_tmp29B;});};}else{
# 1102
({void*_tmp2A1=0;Cyc_Tcutil_terr(loc,({const char*_tmp2A2="missing name for varargs";_tag_dyneither(_tmp2A2,sizeof(char),25);}),_tag_dyneither(_tmp2A1,sizeof(void*),0));});}}
# 1104
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp2A7=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp2A7[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp2A8;_tmp2A8.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp287);_tmp2A8;});_tmp2A7;});
return({struct Cyc_Tcenv_Fenv*_tmp2A9=_region_malloc(r,sizeof(*_tmp2A9));_tmp2A9->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp2B1=_region_malloc(r,sizeof(*_tmp2B1));_tmp2B1->frgn=r;_tmp2B1->return_typ=fd->ret_type;_tmp2B1->delayed_effect_checks=0;_tmp2B1->delayed_constraint_checks=0;_tmp2B1;});_tmp2A9->type_vars=(struct Cyc_List_List*)_tmp285;_tmp2A9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp286;_tmp2A9->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp2A9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2B0=_region_malloc(r,sizeof(*_tmp2B0));_tmp2B0->fallthru_clause=0;_tmp2B0;});_tmp2A9->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2AB;_tmp2AB.tag=24;_tmp2AB.f1=({struct Cyc_List_List*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2AF;_tmp2AF.tag=23;_tmp2AF.f1=param_rgn;_tmp2AF;});_tmp2AE;});_tmp2AC->tl=({struct Cyc_List_List*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->hd=(void*)_check_null(fd->effect);_tmp2AD->tl=0;_tmp2AD;});_tmp2AC;});_tmp2AB;});_tmp2AA;}));_tmp2A9->curr_rgn=(void*)param_rgn;_tmp2A9->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp2A9->flags).in_notreadctxt=0;(_tmp2A9->flags).in_lhs=0;(_tmp2A9->flags).in_new=0;(_tmp2A9->flags).abstract_ok=0;_tmp2A9->flags;});_tmp2A9->fnrgn=(struct _RegionHandle*)r;_tmp2A9;});};}
# 1124
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 1127
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));_tmp2DA->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp2DA->identity=Cyc_Tcutil_new_tvar_id();_tmp2DA->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp2DA;});
struct Cyc_List_List*_tmp2BA=({struct Cyc_List_List*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));_tmp2D9->hd=rgn0;_tmp2D9->tl=0;_tmp2D9;});
struct Cyc_RgnOrder_RgnPO*_tmp2BB=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2D8;_tmp2D8.tag=24;_tmp2D8.f1=0;_tmp2D8;});_tmp2D7;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2D6;_tmp2D6.tag=2;_tmp2D6.f1=rgn0;_tmp2D6;});_tmp2D5;});
struct Cyc_List_List*_tmp2BC=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp2BD=*((struct _tuple8*)args->hd);struct _tuple8 _tmp2BE=_tmp2BD;struct _dyneither_ptr*_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CB;void*_tmp2CA;_LL13E: _tmp2CC=_tmp2BE.f1;_tmp2CB=_tmp2BE.f2;_tmp2CA=_tmp2BE.f3;_LL13F:;
if(_tmp2CC != 0){
struct Cyc_Absyn_Vardecl*_tmp2BF=({struct Cyc_Absyn_Vardecl*_tmp2C7=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2C7[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2C8;_tmp2C8.sc=Cyc_Absyn_Public;_tmp2C8.name=({struct _tuple0*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9->f1=Cyc_Absyn_Loc_n;_tmp2C9->f2=_tmp2CC;_tmp2C9;});_tmp2C8.tq=_tmp2CB;_tmp2C8.type=_tmp2CA;_tmp2C8.initializer=0;_tmp2C8.rgn=param_rgn;_tmp2C8.attributes=0;_tmp2C8.escapes=0;_tmp2C8;});_tmp2C7;});
# 1143
_tmp2BC=({struct Cyc_List_List*_tmp2C0=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp2C0[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp2C1;_tmp2C1.hd=_tmp2BF;_tmp2C1.tl=_tmp2BC;_tmp2C1;});_tmp2C0;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2C2=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2C6;_tmp2C6.tag=3;_tmp2C6.f1=_tmp2BF;_tmp2C6;});_tmp2C5;});
struct _dyneither_ptr*_tmp2C3=_tmp2CC;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2C4=_region_malloc(r,sizeof(*_tmp2C4));_tmp2C4->v=_tmp2C3;_tmp2C4->b=(void*)_tmp2C2;_tmp2C4->tl=locals;_tmp2C4;});};}}
# 1149
return({struct Cyc_Tcenv_Fenv*_tmp2CD=_region_malloc(r,sizeof(*_tmp2CD));_tmp2CD->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp2D4=_region_malloc(r,sizeof(*_tmp2D4));_tmp2D4->frgn=r;_tmp2D4->return_typ=ret_type;_tmp2D4->delayed_effect_checks=0;_tmp2D4->delayed_constraint_checks=0;_tmp2D4;});_tmp2CD->type_vars=(struct Cyc_List_List*)_tmp2BA;_tmp2CD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2BB;_tmp2CD->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp2CD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2D3=_region_malloc(r,sizeof(*_tmp2D3));_tmp2D3->fallthru_clause=0;_tmp2D3;});_tmp2CD->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2CF;_tmp2CF.tag=24;_tmp2CF.f1=({struct Cyc_List_List*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2D2;_tmp2D2.tag=23;_tmp2D2.f1=param_rgn;_tmp2D2;});_tmp2D1;});_tmp2D0->tl=0;_tmp2D0;});_tmp2CF;});_tmp2CE;}));_tmp2CD->curr_rgn=(void*)param_rgn;_tmp2CD->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp2CD->flags).in_notreadctxt=0;(_tmp2CD->flags).in_lhs=0;(_tmp2CD->flags).in_new=0;(_tmp2CD->flags).abstract_ok=0;_tmp2CD->flags;});_tmp2CD->fnrgn=(struct _RegionHandle*)r;_tmp2CD;});}
# 1167
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp2DC=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp31E;struct Cyc_RgnOrder_RgnPO*_tmp31D;struct Cyc_List_List*_tmp31C;struct Cyc_Tcenv_SharedFenv*_tmp31B;struct _RegionHandle*_tmp31A;_LL141: _tmp31E=_tmp2DC->locals;_tmp31D=_tmp2DC->region_order;_tmp31C=_tmp2DC->type_vars;_tmp31B=_tmp2DC->shared;_tmp31A=_tmp2DC->fnrgn;_LL142:;{
# 1170
struct _RegionHandle*_tmp2DD=_tmp31B->frgn;
const struct Cyc_Tcenv_Bindings*_tmp2DE=_tmp31E;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->name=({struct _dyneither_ptr*_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp319;_tmp319.tag=0;_tmp319.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp317[1]={& _tmp319};Cyc_aprintf(({const char*_tmp318="`%s";_tag_dyneither(_tmp318,sizeof(char),4);}),_tag_dyneither(_tmp317,sizeof(void*),1));});});_tmp316;});_tmp315->identity=
# 1174
Cyc_Tcutil_new_tvar_id();_tmp315->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp315;});
# 1176
{struct Cyc_List_List*_tmp2DF=fd->tvs;for(0;_tmp2DF != 0;_tmp2DF=_tmp2DF->tl){
struct Cyc_Absyn_Kind*_tmp2E0=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DF->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp2E1=_tmp2E0;enum Cyc_Absyn_KindQual _tmp2E5;enum Cyc_Absyn_AliasQual _tmp2E4;_LL144: _tmp2E5=_tmp2E1->kind;_tmp2E4=_tmp2E1->aliasqual;_LL145:;
if(_tmp2E5 == Cyc_Absyn_RgnKind){
if(_tmp2E4 == Cyc_Absyn_Aliasable)
_tmp31D=Cyc_RgnOrder_add_youngest(_tmp2DD,_tmp31D,(struct Cyc_Absyn_Tvar*)_tmp2DF->hd,0,0);else{
# 1182
({void*_tmp2E2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2E3="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp2E3,sizeof(char),39);}),_tag_dyneither(_tmp2E2,sizeof(void*),0));});}}}}
# 1184
_tmp31D=Cyc_RgnOrder_add_youngest(_tmp2DD,_tmp31D,rgn0,0,0);{
struct Cyc_List_List*_tmp2E6=({struct Cyc_List_List*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->hd=rgn0;_tmp314->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp31C);_tmp314;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp2E6);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp313;_tmp313.tag=2;_tmp313.f1=rgn0;_tmp313;});_tmp312;});
struct Cyc_List_List*_tmp2E7=0;
{struct Cyc_List_List*_tmp2E8=fd->args;for(0;_tmp2E8 != 0;_tmp2E8=_tmp2E8->tl){
struct Cyc_Absyn_Vardecl*_tmp2E9=({struct Cyc_Absyn_Vardecl*_tmp2F1=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2F1[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2F2;_tmp2F2.sc=Cyc_Absyn_Public;_tmp2F2.name=({struct _tuple0*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->f1=Cyc_Absyn_Loc_n;_tmp2F3->f2=(*((struct _tuple8*)_tmp2E8->hd)).f1;_tmp2F3;});_tmp2F2.tq=(*((struct _tuple8*)_tmp2E8->hd)).f2;_tmp2F2.type=(*((struct _tuple8*)_tmp2E8->hd)).f3;_tmp2F2.initializer=0;_tmp2F2.rgn=param_rgn;_tmp2F2.attributes=0;_tmp2F2.escapes=0;_tmp2F2;});_tmp2F1;});
# 1197
_tmp2E7=({struct Cyc_List_List*_tmp2EA=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp2EA[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp2EB;_tmp2EB.hd=_tmp2E9;_tmp2EB.tl=_tmp2E7;_tmp2EB;});_tmp2EA;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2EC=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2F0;_tmp2F0.tag=3;_tmp2F0.f1=_tmp2E9;_tmp2F0;});_tmp2EF;});
struct _dyneither_ptr*_tmp2ED=(*((struct _tuple8*)_tmp2E8->hd)).f1;
_tmp2DE=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2EE=_region_malloc(_tmp2DD,sizeof(*_tmp2EE));_tmp2EE->v=_tmp2ED;_tmp2EE->b=(void*)_tmp2EC;_tmp2EE->tl=_tmp2DE;_tmp2EE;});};}}
# 1202
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp2F4=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp2F5=_tmp2F4;struct _dyneither_ptr*_tmp306;struct Cyc_Absyn_Tqual _tmp305;void*_tmp304;int _tmp303;_LL147: _tmp306=_tmp2F5.name;_tmp305=_tmp2F5.tq;_tmp304=_tmp2F5.type;_tmp303=_tmp2F5.inject;_LL148:;
if(_tmp306 != 0){
void*_tmp2F6=Cyc_Absyn_dyneither_typ(_tmp304,param_rgn,_tmp305,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp2F7=({struct Cyc_Absyn_Vardecl*_tmp2FE=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2FE[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2FF;_tmp2FF.sc=Cyc_Absyn_Public;_tmp2FF.name=({struct _tuple0*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->f1=Cyc_Absyn_Loc_n;_tmp300->f2=_tmp306;_tmp300;});_tmp2FF.tq=
# 1208
Cyc_Absyn_empty_tqual(0);_tmp2FF.type=_tmp2F6;_tmp2FF.initializer=0;_tmp2FF.rgn=param_rgn;_tmp2FF.attributes=0;_tmp2FF.escapes=0;_tmp2FF;});_tmp2FE;});
# 1213
_tmp2E7=({struct Cyc_List_List*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->hd=_tmp2F7;_tmp2F8->tl=_tmp2E7;_tmp2F8;});{
struct _dyneither_ptr*_tmp2F9=_tmp306;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2FA=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2FD;_tmp2FD.tag=3;_tmp2FD.f1=_tmp2F7;_tmp2FD;});_tmp2FC;});
_tmp2DE=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2FB=_region_malloc(_tmp2DD,sizeof(*_tmp2FB));_tmp2FB->v=_tmp2F9;_tmp2FB->b=(void*)_tmp2FA;_tmp2FB->tl=_tmp2DE;_tmp2FB;});};}else{
# 1218
({void*_tmp301=0;Cyc_Tcutil_terr(loc,({const char*_tmp302="missing name for varargs";_tag_dyneither(_tmp302,sizeof(char),25);}),_tag_dyneither(_tmp301,sizeof(void*),0));});}}
# 1220
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp307=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp307[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp308;_tmp308.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2E7);_tmp308;});_tmp307;});
return({struct Cyc_Tcenv_Fenv*_tmp309=_region_malloc(_tmp2DD,sizeof(*_tmp309));_tmp309->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp311=_region_malloc(_tmp2DD,sizeof(*_tmp311));_tmp311->frgn=_tmp2DD;_tmp311->return_typ=fd->ret_type;_tmp311->delayed_effect_checks=0;_tmp311->delayed_constraint_checks=0;_tmp311;});_tmp309->type_vars=(struct Cyc_List_List*)_tmp2E6;_tmp309->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp31D;_tmp309->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2DE;_tmp309->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp310=_region_malloc(_tmp2DD,sizeof(*_tmp310));_tmp310->fallthru_clause=0;_tmp310;});_tmp309->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp30B;_tmp30B.tag=24;_tmp30B.f1=({struct Cyc_List_List*_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp30F;_tmp30F.tag=23;_tmp30F.f1=param_rgn;_tmp30F;});_tmp30E;});_tmp30C->tl=({struct Cyc_List_List*_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D->hd=(void*)_check_null(fd->effect);_tmp30D->tl=0;_tmp30D;});_tmp30C;});_tmp30B;});_tmp30A;}));_tmp309->curr_rgn=(void*)param_rgn;_tmp309->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp309->flags).in_notreadctxt=0;(_tmp309->flags).in_lhs=0;(_tmp309->flags).in_new=0;(_tmp309->flags).abstract_ok=0;_tmp309->flags;});_tmp309->fnrgn=(struct _RegionHandle*)_tmp31A;_tmp309;});};};};}
