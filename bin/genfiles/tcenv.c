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
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_SlowDict_Dict;
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
# 919 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
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
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 85
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 87
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 89
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 101 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 105
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 109
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 116
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 119
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 122
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 124
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 127
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 130
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 136
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 140
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 145
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 153
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 158
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 163
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 166
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 174
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 176
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 178
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 180
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 182
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 186
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 189
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 192
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 195
extern int Cyc_Tcenv_warn_override;
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
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
# 63 "tcenv.cyc"
int Cyc_Tcenv_warn_override=0;char Cyc_Tcenv_Env_error[10]="Env_error";
# 66
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 68
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 71
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 112
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{const struct _tuple10*fallthru_clause;};
# 118
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 141 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 175 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 178
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)
return bs->b;}
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}struct _tuple12{void*f1;int f2;};
# 186
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 188
struct Cyc_Tcenv_Genv*_tmp2=({struct Cyc_Tcenv_Genv*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->grgn=r;_tmp9->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9;});
_tmp2->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp2->datatypedecls,Cyc_Absyn_exn_name,({struct Cyc_Absyn_Datatypedecl**_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=Cyc_Absyn_exn_tud;_tmp3;}));
{struct Cyc_List_List*_tmp4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp4 != 0;_tmp4=_tmp4->tl){
_tmp2->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp2->ordinaries,((struct Cyc_Absyn_Datatypefield*)_tmp4->hd)->name,({struct _tuple12*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp7;_tmp7.tag=2;_tmp7.f1=Cyc_Absyn_exn_tud;_tmp7.f2=(struct Cyc_Absyn_Datatypefield*)_tmp4->hd;_tmp7;});_tmp6;});_tmp5->f2=1;_tmp5;}));}}
# 199
return({struct Cyc_Tcenv_Tenv*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->ns=0;_tmp8->ae=_tmp2;_tmp8->le=0;_tmp8->allow_valueof=0;_tmp8->in_extern_c_include=0;_tmp8;});}
# 201
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple12*_tmpA=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
if(is_use)
(*_tmpA).f2=1;
return(*_tmpA).f1;}
# 207
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->aggrdecls,q);}
# 210
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);}
# 213
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmpB=_region_malloc(r,sizeof(*_tmpB));_tmpB[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);_tmpB;});}
# 216
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);}
# 219
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->typedefs,q);}
# 225
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmpC=te->le;
if(_tmpC == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmpC;}
# 231
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD=te->le;
if(_tmpD != 0){
struct Cyc_Tcenv_Fenv*_tmpE=_tmpD;struct Cyc_Tcenv_Fenv*_tmpF=_tmpE;struct _RegionHandle*_tmp10;_LL1: _tmp10=_tmpF->fnrgn;_LL2:;
return _tmp10;}
# 237
return Cyc_Core_heap_region;}
# 240
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp11="put_fenv";_tag_dyneither(_tmp11,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp12=_region_malloc(l,sizeof(*_tmp12));_tmp12->ns=te->ns;_tmp12->ae=te->ae;_tmp12->le=fe;_tmp12->allow_valueof=te->allow_valueof;_tmp12->in_extern_c_include=te->in_extern_c_include;_tmp12;});}
# 245
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp13=_region_malloc(l,sizeof(*_tmp13));_tmp13->ns=te->ns;_tmp13->ae=te->ae;_tmp13->le=0;_tmp13->allow_valueof=te->allow_valueof;_tmp13->in_extern_c_include=te->in_extern_c_include;_tmp13;});}
# 249
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 251
struct Cyc_Tcenv_Fenv _tmp14=*f;struct Cyc_Tcenv_Fenv _tmp15=_tmp14;struct Cyc_Tcenv_SharedFenv*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_RgnOrder_RgnPO*_tmp1D;const struct Cyc_Tcenv_Bindings*_tmp1C;struct Cyc_Tcenv_CtrlEnv*_tmp1B;void*_tmp1A;void*_tmp19;struct Cyc_Tcenv_FenvFlags _tmp18;struct _RegionHandle*_tmp17;_LL4: _tmp1F=_tmp15.shared;_tmp1E=_tmp15.type_vars;_tmp1D=_tmp15.region_order;_tmp1C=_tmp15.locals;_tmp1B=_tmp15.ctrl_env;_tmp1A=_tmp15.capability;_tmp19=_tmp15.curr_rgn;_tmp18=_tmp15.flags;_tmp17=_tmp15.fnrgn;_LL5:;
# 253
return({struct Cyc_Tcenv_Fenv*_tmp16=_region_malloc(l,sizeof(*_tmp16));_tmp16->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F;_tmp16->type_vars=(struct Cyc_List_List*)_tmp1E;_tmp16->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1D;_tmp16->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1C;_tmp16->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1B;_tmp16->capability=(void*)_tmp1A;_tmp16->curr_rgn=(void*)_tmp19;_tmp16->flags=(struct Cyc_Tcenv_FenvFlags)_tmp18;_tmp16->fnrgn=(struct _RegionHandle*)l;_tmp16;});}
# 257
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 259
struct Cyc_Tcenv_Fenv _tmp20=*f;struct Cyc_Tcenv_Fenv _tmp21=_tmp20;struct Cyc_Tcenv_SharedFenv*_tmp2F;struct Cyc_List_List*_tmp2E;struct Cyc_RgnOrder_RgnPO*_tmp2D;const struct Cyc_Tcenv_Bindings*_tmp2C;struct Cyc_Tcenv_CtrlEnv*_tmp2B;void*_tmp2A;void*_tmp29;struct Cyc_Tcenv_FenvFlags _tmp28;_LL7: _tmp2F=_tmp21.shared;_tmp2E=_tmp21.type_vars;_tmp2D=_tmp21.region_order;_tmp2C=_tmp21.locals;_tmp2B=_tmp21.ctrl_env;_tmp2A=_tmp21.capability;_tmp29=_tmp21.curr_rgn;_tmp28=_tmp21.flags;_LL8:;{
# 262
struct Cyc_Tcenv_CtrlEnv _tmp22=*_tmp2B;struct Cyc_Tcenv_CtrlEnv _tmp23=_tmp22;const struct _tuple10*_tmp27;_LLA: _tmp27=_tmp23.fallthru_clause;_LLB:;{
struct Cyc_Tcenv_CtrlEnv*_tmp24=({struct Cyc_Tcenv_CtrlEnv*_tmp26=_region_malloc(l,sizeof(*_tmp26));_tmp26->fallthru_clause=_tmp27;_tmp26;});
# 265
return({struct Cyc_Tcenv_Fenv*_tmp25=_region_malloc(l,sizeof(*_tmp25));_tmp25->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp2F;_tmp25->type_vars=(struct Cyc_List_List*)_tmp2E;_tmp25->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2D;_tmp25->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2C;_tmp25->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp24;_tmp25->capability=(void*)_tmp2A;_tmp25->curr_rgn=(void*)_tmp29;_tmp25->flags=(struct Cyc_Tcenv_FenvFlags)_tmp28;_tmp25->fnrgn=(struct _RegionHandle*)l;_tmp25;});};};}
# 271
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp30=Cyc_Tcenv_get_fenv(te,({const char*_tmp36="return_typ";_tag_dyneither(_tmp36,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmp31=_tmp30;struct Cyc_Tcenv_SharedFenv*_tmp35;_LLD: _tmp35=_tmp31->shared;_LLE:;{
struct Cyc_Tcenv_SharedFenv _tmp32=*_tmp35;struct Cyc_Tcenv_SharedFenv _tmp33=_tmp32;void*_tmp34;_LL10: _tmp34=_tmp33.return_typ;_LL11:;
return _tmp34;};}
# 277
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp37=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmp38=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmp37));struct Cyc_Tcenv_Fenv _tmp39=_tmp38;struct Cyc_List_List*_tmp3A;_LL13: _tmp3A=_tmp39.type_vars;_LL14:;
return _tmp3A;};}
# 284
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->v=Cyc_Tcenv_lookup_type_vars(te);_tmp3B;});}
# 288
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 290
struct Cyc_Tcenv_Fenv*_tmp3C=
# 292
Cyc_Tcenv_get_fenv(te,({const char*_tmp49="add_type_vars";_tag_dyneither(_tmp49,sizeof(char),14);}));
# 290
struct Cyc_Tcenv_Fenv*_tmp3D=_tmp3C;struct Cyc_Tcenv_SharedFenv*_tmp48;struct Cyc_List_List*_tmp47;struct Cyc_RgnOrder_RgnPO*_tmp46;const struct Cyc_Tcenv_Bindings*_tmp45;struct Cyc_Tcenv_CtrlEnv*_tmp44;void*_tmp43;void*_tmp42;struct Cyc_Tcenv_FenvFlags _tmp41;_LL16: _tmp48=_tmp3D->shared;_tmp47=_tmp3D->type_vars;_tmp46=_tmp3D->region_order;_tmp45=_tmp3D->locals;_tmp44=_tmp3D->ctrl_env;_tmp43=_tmp3D->capability;_tmp42=_tmp3D->curr_rgn;_tmp41=_tmp3D->flags;_LL17:;
# 294
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp3E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp47);
Cyc_Tcutil_check_unique_tvars(loc,_tmp3E);{
struct Cyc_Tcenv_Fenv*_tmp3F=({struct Cyc_Tcenv_Fenv*_tmp40=_region_malloc(r,sizeof(*_tmp40));_tmp40->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp48;_tmp40->type_vars=(struct Cyc_List_List*)_tmp3E;_tmp40->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp46;_tmp40->locals=(const struct Cyc_Tcenv_Bindings*)_tmp45;_tmp40->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp44;_tmp40->capability=(void*)_tmp43;_tmp40->curr_rgn=(void*)_tmp42;_tmp40->flags=(struct Cyc_Tcenv_FenvFlags)_tmp41;_tmp40->fnrgn=(struct _RegionHandle*)r;_tmp40;});
# 299
return Cyc_Tcenv_put_fenv(r,te,_tmp3F);};};}
# 302
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp4A=te->le;
if(_tmp4A == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 307
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmp4A);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 312
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 314
struct _dyneither_ptr*_tmp4B=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmp4C=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmp4D=_tmp4C;if((_tmp4D.Loc_n).tag == 4){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _LL1C:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4F;_tmp4F.tag=Cyc_Core_Impossible;_tmp4F.f1=({const char*_tmp50="add_local_var: called with non-local";_tag_dyneither(_tmp50,sizeof(char),37);});_tmp4F;});_tmp4E;}));}_LL18:;}
# 319
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp51;_push_handler(& _tmp51);{int _tmp53=0;if(setjmp(_tmp51.handler))_tmp53=1;if(!_tmp53){{void*_tmp54=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp55=_tmp54;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp55)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp55)->f1)){case 4: _LL1E: _LL1F:
({void*_tmp56=0;Cyc_Tcutil_warn(loc,({const char*_tmp57="declaration hides local";_tag_dyneither(_tmp57,sizeof(char),24);}),_tag_dyneither(_tmp56,sizeof(void*),0));});goto _LL1D;case 1: _LL20: _LL21:
({void*_tmp58=0;Cyc_Tcutil_warn(loc,({const char*_tmp59="declaration hides global";_tag_dyneither(_tmp59,sizeof(char),25);}),_tag_dyneither(_tmp58,sizeof(void*),0));});goto _LL1D;case 5: _LL22: _LL23:
({void*_tmp5A=0;Cyc_Tcutil_warn(loc,({const char*_tmp5B="declaration hides pattern variable";_tag_dyneither(_tmp5B,sizeof(char),35);}),_tag_dyneither(_tmp5A,sizeof(void*),0));});goto _LL1D;case 3: _LL24: _LL25:
({void*_tmp5C=0;Cyc_Tcutil_warn(loc,({const char*_tmp5D="declaration hides parameter";_tag_dyneither(_tmp5D,sizeof(char),28);}),_tag_dyneither(_tmp5C,sizeof(void*),0));});goto _LL1D;default: goto _LL26;}else{_LL26: _LL27:
 goto _LL1D;}_LL1D:;}
# 320
;_pop_handler();}else{void*_tmp52=(void*)_exn_thrown;void*_tmp5E=_tmp52;void*_tmp5F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp5E)->tag == Cyc_Dict_Absent){_LL29: _LL2A:
# 327
 goto _LL28;}else{_LL2B: _tmp5F=_tmp5E;_LL2C:(int)_rethrow(_tmp5F);}_LL28:;}};}{
struct Cyc_Tcenv_Fenv*_tmp60=
# 330
Cyc_Tcenv_get_fenv(te,({const char*_tmp71="add_local_var";_tag_dyneither(_tmp71,sizeof(char),14);}));
# 328
struct Cyc_Tcenv_Fenv*_tmp61=_tmp60;struct Cyc_Tcenv_SharedFenv*_tmp70;struct Cyc_List_List*_tmp6F;struct Cyc_RgnOrder_RgnPO*_tmp6E;const struct Cyc_Tcenv_Bindings*_tmp6D;struct Cyc_Tcenv_CtrlEnv*_tmp6C;void*_tmp6B;void*_tmp6A;struct Cyc_Tcenv_FenvFlags _tmp69;_LL2E: _tmp70=_tmp61->shared;_tmp6F=_tmp61->type_vars;_tmp6E=_tmp61->region_order;_tmp6D=_tmp61->locals;_tmp6C=_tmp61->ctrl_env;_tmp6B=_tmp61->capability;_tmp6A=_tmp61->curr_rgn;_tmp69=_tmp61->flags;_LL2F:;{
# 331
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp62=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp68;_tmp68.tag=4;_tmp68.f1=vd;_tmp68;});_tmp67;});
struct Cyc_Tcenv_Bindings*_tmp63=({struct Cyc_Tcenv_Bindings*_tmp66=_region_malloc(r,sizeof(*_tmp66));_tmp66->v=_tmp4B;_tmp66->b=(void*)_tmp62;_tmp66->tl=_tmp6D;_tmp66;});
struct Cyc_Tcenv_Fenv*_tmp64=({struct Cyc_Tcenv_Fenv*_tmp65=_region_malloc(r,sizeof(*_tmp65));_tmp65->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp70;_tmp65->type_vars=(struct Cyc_List_List*)_tmp6F;_tmp65->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp6E;_tmp65->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp63);_tmp65->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp6C;_tmp65->capability=(void*)_tmp6B;_tmp65->curr_rgn=(void*)_tmp6A;_tmp65->flags=(struct Cyc_Tcenv_FenvFlags)_tmp69;_tmp65->fnrgn=(struct _RegionHandle*)r;_tmp65;});
# 335
return Cyc_Tcenv_put_fenv(r,te,_tmp64);};};}
# 338
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 340
struct Cyc_Tcenv_Fenv*_tmp72=te->le;
if(_tmp72 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp73=*_tmp72;struct Cyc_Tcenv_Fenv _tmp74=_tmp73;struct Cyc_Tcenv_SharedFenv*_tmp80;struct Cyc_List_List*_tmp7F;struct Cyc_RgnOrder_RgnPO*_tmp7E;const struct Cyc_Tcenv_Bindings*_tmp7D;struct Cyc_Tcenv_CtrlEnv*_tmp7C;void*_tmp7B;void*_tmp7A;int _tmp79;int _tmp78;int _tmp77;_LL31: _tmp80=_tmp74.shared;_tmp7F=_tmp74.type_vars;_tmp7E=_tmp74.region_order;_tmp7D=_tmp74.locals;_tmp7C=_tmp74.ctrl_env;_tmp7B=_tmp74.capability;_tmp7A=_tmp74.curr_rgn;_tmp79=(_tmp74.flags).in_notreadctxt;_tmp78=(_tmp74.flags).in_lhs;_tmp77=(_tmp74.flags).abstract_ok;_LL32:;{
# 345
struct Cyc_Tcenv_Fenv*_tmp75=({struct Cyc_Tcenv_Fenv*_tmp76=_region_malloc(r,sizeof(*_tmp76));_tmp76->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp80;_tmp76->type_vars=(struct Cyc_List_List*)_tmp7F;_tmp76->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp7E;_tmp76->locals=(const struct Cyc_Tcenv_Bindings*)_tmp7D;_tmp76->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp7C;_tmp76->capability=(void*)_tmp7B;_tmp76->curr_rgn=(void*)_tmp7A;_tmp76->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp76->flags).in_notreadctxt=_tmp79;(_tmp76->flags).in_lhs=_tmp78;(_tmp76->flags).in_new=(int)status;(_tmp76->flags).abstract_ok=_tmp77;_tmp76->flags;});_tmp76->fnrgn=(struct _RegionHandle*)r;_tmp76;});
# 348
return Cyc_Tcenv_put_fenv(r,te,_tmp75);};};}
# 351
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp81=te->le;
if(_tmp81 == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp82=_tmp81;struct Cyc_Tcenv_Fenv*_tmp83=_tmp82;int _tmp84;_LL34: _tmp84=(_tmp83->flags).in_new;_LL35:;
return(enum Cyc_Tcenv_NewStatus)_tmp84;};}
# 358
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp85=te->le;
if(_tmp85 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp86=_tmp85;struct Cyc_Tcenv_Fenv*_tmp87=_tmp86;int _tmp88;_LL37: _tmp88=(_tmp87->flags).abstract_ok;_LL38:;
return _tmp88;};}
# 365
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 367
struct Cyc_Tcenv_Fenv*_tmp89=te->le;
if(_tmp89 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp8A=*_tmp89;struct Cyc_Tcenv_Fenv _tmp8B=_tmp8A;struct Cyc_Tcenv_SharedFenv*_tmp97;struct Cyc_List_List*_tmp96;struct Cyc_RgnOrder_RgnPO*_tmp95;const struct Cyc_Tcenv_Bindings*_tmp94;struct Cyc_Tcenv_CtrlEnv*_tmp93;void*_tmp92;void*_tmp91;int _tmp90;int _tmp8F;int _tmp8E;_LL3A: _tmp97=_tmp8B.shared;_tmp96=_tmp8B.type_vars;_tmp95=_tmp8B.region_order;_tmp94=_tmp8B.locals;_tmp93=_tmp8B.ctrl_env;_tmp92=_tmp8B.capability;_tmp91=_tmp8B.curr_rgn;_tmp90=(_tmp8B.flags).in_notreadctxt;_tmp8F=(_tmp8B.flags).in_lhs;_tmp8E=(_tmp8B.flags).in_new;_LL3B:;{
# 372
struct Cyc_Tcenv_Fenv*_tmp8C=({struct Cyc_Tcenv_Fenv*_tmp8D=_region_malloc(r,sizeof(*_tmp8D));_tmp8D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp97;_tmp8D->type_vars=(struct Cyc_List_List*)_tmp96;_tmp8D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp95;_tmp8D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp94;_tmp8D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp93;_tmp8D->capability=(void*)_tmp92;_tmp8D->curr_rgn=(void*)_tmp91;_tmp8D->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp8D->flags).in_notreadctxt=_tmp90;(_tmp8D->flags).in_lhs=_tmp8F;(_tmp8D->flags).in_new=_tmp8E;(_tmp8D->flags).abstract_ok=1;_tmp8D->flags;});_tmp8D->fnrgn=(struct _RegionHandle*)r;_tmp8D;});
# 375
return Cyc_Tcenv_put_fenv(r,te,_tmp8C);};};}
# 378
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 380
struct Cyc_Tcenv_Fenv*_tmp98=te->le;
if(_tmp98 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp99=*_tmp98;struct Cyc_Tcenv_Fenv _tmp9A=_tmp99;struct Cyc_Tcenv_SharedFenv*_tmpA6;struct Cyc_List_List*_tmpA5;struct Cyc_RgnOrder_RgnPO*_tmpA4;const struct Cyc_Tcenv_Bindings*_tmpA3;struct Cyc_Tcenv_CtrlEnv*_tmpA2;void*_tmpA1;void*_tmpA0;int _tmp9F;int _tmp9E;int _tmp9D;_LL3D: _tmpA6=_tmp9A.shared;_tmpA5=_tmp9A.type_vars;_tmpA4=_tmp9A.region_order;_tmpA3=_tmp9A.locals;_tmpA2=_tmp9A.ctrl_env;_tmpA1=_tmp9A.capability;_tmpA0=_tmp9A.curr_rgn;_tmp9F=(_tmp9A.flags).in_notreadctxt;_tmp9E=(_tmp9A.flags).in_lhs;_tmp9D=(_tmp9A.flags).in_new;_LL3E:;{
# 385
struct Cyc_Tcenv_Fenv*_tmp9B=({struct Cyc_Tcenv_Fenv*_tmp9C=_region_malloc(r,sizeof(*_tmp9C));_tmp9C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA6;_tmp9C->type_vars=(struct Cyc_List_List*)_tmpA5;_tmp9C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA4;_tmp9C->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA3;_tmp9C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA2;_tmp9C->capability=(void*)_tmpA1;_tmp9C->curr_rgn=(void*)_tmpA0;_tmp9C->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp9C->flags).in_notreadctxt=_tmp9F;(_tmp9C->flags).in_lhs=_tmp9E;(_tmp9C->flags).in_new=_tmp9D;(_tmp9C->flags).abstract_ok=0;_tmp9C->flags;});_tmp9C->fnrgn=(struct _RegionHandle*)r;_tmp9C;});
# 388
return Cyc_Tcenv_put_fenv(r,te,_tmp9B);};};}
# 391
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpA7=te->le;
if(_tmpA7 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpA8=*_tmpA7;struct Cyc_Tcenv_Fenv _tmpA9=_tmpA8;struct Cyc_Tcenv_SharedFenv*_tmpB5;struct Cyc_List_List*_tmpB4;struct Cyc_RgnOrder_RgnPO*_tmpB3;const struct Cyc_Tcenv_Bindings*_tmpB2;struct Cyc_Tcenv_CtrlEnv*_tmpB1;void*_tmpB0;void*_tmpAF;int _tmpAE;int _tmpAD;int _tmpAC;_LL40: _tmpB5=_tmpA9.shared;_tmpB4=_tmpA9.type_vars;_tmpB3=_tmpA9.region_order;_tmpB2=_tmpA9.locals;_tmpB1=_tmpA9.ctrl_env;_tmpB0=_tmpA9.capability;_tmpAF=_tmpA9.curr_rgn;_tmpAE=(_tmpA9.flags).in_lhs;_tmpAD=(_tmpA9.flags).in_new;_tmpAC=(_tmpA9.flags).abstract_ok;_LL41:;{
# 397
struct Cyc_Tcenv_Fenv*_tmpAA=({struct Cyc_Tcenv_Fenv*_tmpAB=_region_malloc(r,sizeof(*_tmpAB));_tmpAB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB5;_tmpAB->type_vars=(struct Cyc_List_List*)_tmpB4;_tmpAB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB3;_tmpAB->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB2;_tmpAB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB1;_tmpAB->capability=(void*)_tmpB0;_tmpAB->curr_rgn=(void*)_tmpAF;_tmpAB->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpAB->flags).in_notreadctxt=1;(_tmpAB->flags).in_lhs=_tmpAE;(_tmpAB->flags).in_new=_tmpAD;(_tmpAB->flags).abstract_ok=_tmpAC;_tmpAB->flags;});_tmpAB->fnrgn=(struct _RegionHandle*)r;_tmpAB;});
# 400
return Cyc_Tcenv_put_fenv(r,te,_tmpAA);};};}
# 403
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpB6=te->le;
if(_tmpB6 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpB7=*_tmpB6;struct Cyc_Tcenv_Fenv _tmpB8=_tmpB7;struct Cyc_Tcenv_SharedFenv*_tmpC4;struct Cyc_List_List*_tmpC3;struct Cyc_RgnOrder_RgnPO*_tmpC2;const struct Cyc_Tcenv_Bindings*_tmpC1;struct Cyc_Tcenv_CtrlEnv*_tmpC0;void*_tmpBF;void*_tmpBE;int _tmpBD;int _tmpBC;int _tmpBB;_LL43: _tmpC4=_tmpB8.shared;_tmpC3=_tmpB8.type_vars;_tmpC2=_tmpB8.region_order;_tmpC1=_tmpB8.locals;_tmpC0=_tmpB8.ctrl_env;_tmpBF=_tmpB8.capability;_tmpBE=_tmpB8.curr_rgn;_tmpBD=(_tmpB8.flags).in_lhs;_tmpBC=(_tmpB8.flags).in_new;_tmpBB=(_tmpB8.flags).abstract_ok;_LL44:;{
# 409
struct Cyc_Tcenv_Fenv*_tmpB9=({struct Cyc_Tcenv_Fenv*_tmpBA=_region_malloc(r,sizeof(*_tmpBA));_tmpBA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC4;_tmpBA->type_vars=(struct Cyc_List_List*)_tmpC3;_tmpBA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC2;_tmpBA->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC1;_tmpBA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpC0;_tmpBA->capability=(void*)_tmpBF;_tmpBA->curr_rgn=(void*)_tmpBE;_tmpBA->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpBA->flags).in_notreadctxt=0;(_tmpBA->flags).in_lhs=_tmpBD;(_tmpBA->flags).in_new=_tmpBC;(_tmpBA->flags).abstract_ok=_tmpBB;_tmpBA->flags;});_tmpBA->fnrgn=(struct _RegionHandle*)r;_tmpBA;});
# 412
return Cyc_Tcenv_put_fenv(r,te,_tmpB9);};};}
# 415
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC5=te->le;
if(_tmpC5 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpC6=_tmpC5;struct Cyc_Tcenv_Fenv*_tmpC7=_tmpC6;int _tmpC8;_LL46: _tmpC8=(_tmpC7->flags).in_notreadctxt;_LL47:;
return _tmpC8;};}
# 422
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC9=te->le;
if(_tmpC9 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpCA=*_tmpC9;struct Cyc_Tcenv_Fenv _tmpCB=_tmpCA;struct Cyc_Tcenv_SharedFenv*_tmpD7;struct Cyc_List_List*_tmpD6;struct Cyc_RgnOrder_RgnPO*_tmpD5;const struct Cyc_Tcenv_Bindings*_tmpD4;struct Cyc_Tcenv_CtrlEnv*_tmpD3;void*_tmpD2;void*_tmpD1;int _tmpD0;int _tmpCF;int _tmpCE;_LL49: _tmpD7=_tmpCB.shared;_tmpD6=_tmpCB.type_vars;_tmpD5=_tmpCB.region_order;_tmpD4=_tmpCB.locals;_tmpD3=_tmpCB.ctrl_env;_tmpD2=_tmpCB.capability;_tmpD1=_tmpCB.curr_rgn;_tmpD0=(_tmpCB.flags).in_notreadctxt;_tmpCF=(_tmpCB.flags).in_new;_tmpCE=(_tmpCB.flags).abstract_ok;_LL4A:;{
# 428
struct Cyc_Tcenv_Fenv*_tmpCC=({struct Cyc_Tcenv_Fenv*_tmpCD=_region_malloc(r,sizeof(*_tmpCD));_tmpCD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpD7;_tmpCD->type_vars=(struct Cyc_List_List*)_tmpD6;_tmpCD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpD5;_tmpCD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD4;_tmpCD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD3;_tmpCD->capability=(void*)_tmpD2;_tmpCD->curr_rgn=(void*)_tmpD1;_tmpCD->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpCD->flags).in_notreadctxt=_tmpD0;(_tmpCD->flags).in_lhs=1;(_tmpCD->flags).in_new=_tmpCF;(_tmpCD->flags).abstract_ok=_tmpCE;_tmpCD->flags;});_tmpCD->fnrgn=(struct _RegionHandle*)r;_tmpCD;});
# 431
return Cyc_Tcenv_put_fenv(r,te,_tmpCC);};};}
# 434
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD8=te->le;
if(_tmpD8 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpD9=*_tmpD8;struct Cyc_Tcenv_Fenv _tmpDA=_tmpD9;struct Cyc_Tcenv_SharedFenv*_tmpE6;struct Cyc_List_List*_tmpE5;struct Cyc_RgnOrder_RgnPO*_tmpE4;const struct Cyc_Tcenv_Bindings*_tmpE3;struct Cyc_Tcenv_CtrlEnv*_tmpE2;void*_tmpE1;void*_tmpE0;int _tmpDF;int _tmpDE;int _tmpDD;_LL4C: _tmpE6=_tmpDA.shared;_tmpE5=_tmpDA.type_vars;_tmpE4=_tmpDA.region_order;_tmpE3=_tmpDA.locals;_tmpE2=_tmpDA.ctrl_env;_tmpE1=_tmpDA.capability;_tmpE0=_tmpDA.curr_rgn;_tmpDF=(_tmpDA.flags).in_notreadctxt;_tmpDE=(_tmpDA.flags).in_new;_tmpDD=(_tmpDA.flags).abstract_ok;_LL4D:;{
# 440
struct Cyc_Tcenv_Fenv*_tmpDB=({struct Cyc_Tcenv_Fenv*_tmpDC=_region_malloc(r,sizeof(*_tmpDC));_tmpDC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpE6;_tmpDC->type_vars=(struct Cyc_List_List*)_tmpE5;_tmpDC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE4;_tmpDC->locals=(const struct Cyc_Tcenv_Bindings*)_tmpE3;_tmpDC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE2;_tmpDC->capability=(void*)_tmpE1;_tmpDC->curr_rgn=(void*)_tmpE0;_tmpDC->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpDC->flags).in_notreadctxt=_tmpDF;(_tmpDC->flags).in_lhs=0;(_tmpDC->flags).in_new=_tmpDE;(_tmpDC->flags).abstract_ok=_tmpDD;_tmpDC->flags;});_tmpDC->fnrgn=(struct _RegionHandle*)r;_tmpDC;});
# 443
return Cyc_Tcenv_put_fenv(r,te,_tmpDB);};};}
# 446
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpE7=te->le;
if(_tmpE7 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpE8=_tmpE7;struct Cyc_Tcenv_Fenv*_tmpE9=_tmpE8;int _tmpEA;_LL4F: _tmpEA=(_tmpE9->flags).in_lhs;_LL50:;
return _tmpEA;};}
# 454
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 456
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpEB;_push_handler(& _tmpEB);{int _tmpED=0;if(setjmp(_tmpEB.handler))_tmpED=1;if(!_tmpED){
{void*_tmpEE=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpEF=_tmpEE;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpEF)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpEF)->f1)){case 4: _LL52: _LL53:
# 460
({void*_tmpF0=0;Cyc_Tcutil_warn(loc,({const char*_tmpF1="pattern variable hides local";_tag_dyneither(_tmpF1,sizeof(char),29);}),_tag_dyneither(_tmpF0,sizeof(void*),0));});goto _LL51;case 1: _LL54: _LL55:
# 462
({void*_tmpF2=0;Cyc_Tcutil_warn(loc,({const char*_tmpF3="pattern variable hides global";_tag_dyneither(_tmpF3,sizeof(char),30);}),_tag_dyneither(_tmpF2,sizeof(void*),0));});goto _LL51;case 5: _LL56: _LL57:
# 464
({void*_tmpF4=0;Cyc_Tcutil_warn(loc,({const char*_tmpF5="pattern variable hides pattern variable";_tag_dyneither(_tmpF5,sizeof(char),40);}),_tag_dyneither(_tmpF4,sizeof(void*),0));});goto _LL51;case 3: _LL58: _LL59:
# 466
({void*_tmpF6=0;Cyc_Tcutil_warn(loc,({const char*_tmpF7="pattern variable hides parameter";_tag_dyneither(_tmpF7,sizeof(char),33);}),_tag_dyneither(_tmpF6,sizeof(void*),0));});goto _LL51;default: goto _LL5A;}else{_LL5A: _LL5B:
 goto _LL51;}_LL51:;}
# 458
;_pop_handler();}else{void*_tmpEC=(void*)_exn_thrown;void*_tmpF8=_tmpEC;void*_tmpF9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpF8)->tag == Cyc_Dict_Absent){_LL5D: _LL5E:
# 469
 goto _LL5C;}else{_LL5F: _tmpF9=_tmpF8;_LL60:(int)_rethrow(_tmpF9);}_LL5C:;}};}{
struct _dyneither_ptr*_tmpFA=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmpFB=
# 473
Cyc_Tcenv_get_fenv(te,({const char*_tmp10C="add_pat_var";_tag_dyneither(_tmp10C,sizeof(char),12);}));
# 471
struct Cyc_Tcenv_Fenv*_tmpFC=_tmpFB;struct Cyc_Tcenv_SharedFenv*_tmp10B;struct Cyc_List_List*_tmp10A;struct Cyc_RgnOrder_RgnPO*_tmp109;const struct Cyc_Tcenv_Bindings*_tmp108;struct Cyc_Tcenv_CtrlEnv*_tmp107;void*_tmp106;void*_tmp105;struct Cyc_Tcenv_FenvFlags _tmp104;_LL62: _tmp10B=_tmpFC->shared;_tmp10A=_tmpFC->type_vars;_tmp109=_tmpFC->region_order;_tmp108=_tmpFC->locals;_tmp107=_tmpFC->ctrl_env;_tmp106=_tmpFC->capability;_tmp105=_tmpFC->curr_rgn;_tmp104=_tmpFC->flags;_LL63:;{
# 474
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpFD=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp103;_tmp103.tag=5;_tmp103.f1=vd;_tmp103;});_tmp102;});
struct Cyc_Tcenv_Bindings*_tmpFE=({struct Cyc_Tcenv_Bindings*_tmp101=_region_malloc(r,sizeof(*_tmp101));_tmp101->v=_tmpFA;_tmp101->b=(void*)_tmpFD;_tmp101->tl=_tmp108;_tmp101;});
struct Cyc_Tcenv_Fenv*_tmpFF=({struct Cyc_Tcenv_Fenv*_tmp100=_region_malloc(r,sizeof(*_tmp100));_tmp100->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp10B;_tmp100->type_vars=(struct Cyc_List_List*)_tmp10A;_tmp100->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp109;_tmp100->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpFE);_tmp100->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp107;_tmp100->capability=(void*)_tmp106;_tmp100->curr_rgn=(void*)_tmp105;_tmp100->flags=(struct Cyc_Tcenv_FenvFlags)_tmp104;_tmp100->fnrgn=(struct _RegionHandle*)r;_tmp100;});
# 478
return Cyc_Tcenv_put_fenv(r,te,_tmpFF);};};}
# 481
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp10D=te->le;
struct _tuple0*_tmp10E=q;union Cyc_Absyn_Nmspace _tmp128;struct _dyneither_ptr*_tmp127;_LL65: _tmp128=_tmp10E->f1;_tmp127=_tmp10E->f2;_LL66:;{
union Cyc_Absyn_Nmspace _tmp10F=_tmp128;if((_tmp10F.Loc_n).tag == 4){_LL68: _LL69:
# 486
 if(_tmp10D == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp110=_cycalloc_atomic(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Dict_Absent_exn_struct _tmp111;_tmp111.tag=Cyc_Dict_Absent;_tmp111;});_tmp110;}));{
struct Cyc_Tcenv_Fenv*_tmp112=_tmp10D;struct Cyc_Tcenv_Fenv*_tmp113=_tmp112;const struct Cyc_Tcenv_Bindings*_tmp11C;_LL6D: _tmp11C=_tmp113->locals;_LL6E:;{
# 489
struct _handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=0;if(setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){{void*_tmp119=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp117=_region_malloc(r,sizeof(*_tmp117));_tmp117[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp118;_tmp118.tag=0;_tmp118.f1=Cyc_Tcenv_lookup_binding(_tmp11C,_tmp127);_tmp118;});_tmp117;});_npop_handler(0);return _tmp119;};_pop_handler();}else{void*_tmp115=(void*)_exn_thrown;void*_tmp11A=_tmp115;void*_tmp11B;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp11A)->tag == Cyc_Tcenv_NoBinding){_LL70: _LL71:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LL72: _tmp11B=_tmp11A;_LL73:(int)_rethrow(_tmp11B);}_LL6F:;}};};};}else{_LL6A: _LL6B: {
# 492
struct _handler_cons _tmp11D;_push_handler(& _tmp11D);{int _tmp11F=0;if(setjmp(_tmp11D.handler))_tmp11F=1;if(!_tmp11F){{void*_tmp120=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp120;};_pop_handler();}else{void*_tmp11E=(void*)_exn_thrown;void*_tmp121=_tmp11E;void*_tmp126;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp121)->tag == Cyc_Dict_Absent){_LL75: _LL76:
# 494
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp122=_region_malloc(r,sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmp125;_tmp125.tag=0;_tmp125.f1=q;_tmp125;});_tmp124;});_tmp123;});_tmp122;});}else{_LL77: _tmp126=_tmp121;_LL78:(int)_rethrow(_tmp126);}_LL74:;}};}}_LL67:;};}
# 500
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 502
struct Cyc_Tcenv_Fenv*_tmp129=Cyc_Tcenv_get_fenv(te,({const char*_tmp12E="process_fallthru";_tag_dyneither(_tmp12E,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp12A=_tmp129;struct Cyc_Tcenv_CtrlEnv*_tmp12D;_LL7A: _tmp12D=_tmp12A->ctrl_env;_LL7B:;{
const struct _tuple10*_tmp12B=_tmp12D->fallthru_clause;
if(_tmp12B != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C[0]=(*_tmp12B).f1;_tmp12C;});
# 507
return _tmp12B;};}
# 510
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 514
struct Cyc_Tcenv_Fenv*_tmp12F=
# 516
Cyc_Tcenv_get_fenv(te,({const char*_tmp142="set_fallthru";_tag_dyneither(_tmp142,sizeof(char),13);}));
# 514
struct Cyc_Tcenv_Fenv*_tmp130=_tmp12F;struct Cyc_Tcenv_SharedFenv*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_RgnOrder_RgnPO*_tmp13F;const struct Cyc_Tcenv_Bindings*_tmp13E;struct Cyc_Tcenv_CtrlEnv*_tmp13D;void*_tmp13C;void*_tmp13B;struct Cyc_Tcenv_FenvFlags _tmp13A;_LL7D: _tmp141=_tmp130->shared;_tmp140=_tmp130->type_vars;_tmp13F=_tmp130->region_order;_tmp13E=_tmp130->locals;_tmp13D=_tmp130->ctrl_env;_tmp13C=_tmp130->capability;_tmp13B=_tmp130->curr_rgn;_tmp13A=_tmp130->flags;_LL7E:;{
# 517
struct Cyc_Tcenv_CtrlEnv*_tmp131=_tmp13D;const struct _tuple10*_tmp139;_LL80: _tmp139=_tmp131->fallthru_clause;_LL81:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp132->tl=ft_typ;_tmp132;});}{
struct Cyc_List_List*_tmp133=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp134=({struct Cyc_Tcenv_CtrlEnv*_tmp137=_region_malloc(r,sizeof(*_tmp137));_tmp137->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->f1=clause;_tmp138->f2=new_tvs;_tmp138->f3=(const struct Cyc_Tcenv_CList*)_tmp133;_tmp138;});_tmp137;});
# 524
struct Cyc_Tcenv_Fenv*_tmp135=({struct Cyc_Tcenv_Fenv*_tmp136=_region_malloc(r,sizeof(*_tmp136));_tmp136->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp141;_tmp136->type_vars=(struct Cyc_List_List*)_tmp140;_tmp136->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13F;_tmp136->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13E;_tmp136->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp134;_tmp136->capability=(void*)_tmp13C;_tmp136->curr_rgn=(void*)_tmp13B;_tmp136->flags=(struct Cyc_Tcenv_FenvFlags)_tmp13A;_tmp136->fnrgn=(struct _RegionHandle*)r;_tmp136;});
# 528
return Cyc_Tcenv_put_fenv(r,te,_tmp135);};};};}
# 531
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp145="clear_fallthru";_tag_dyneither(_tmp145,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp143=fe;struct Cyc_Tcenv_CtrlEnv*_tmp144;_LL83: _tmp144=_tmp143->ctrl_env;_LL84:;
_tmp144->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 538
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp146=_region_malloc(r2,sizeof(*_tmp146));_tmp146->ns=te->ns;_tmp146->ae=te->ae;_tmp146->le=0;_tmp146->allow_valueof=1;_tmp146->in_extern_c_include=te->in_extern_c_include;_tmp146;});{
struct Cyc_Tcenv_Fenv _tmp147=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp148=_tmp147;struct Cyc_Tcenv_SharedFenv*_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_RgnOrder_RgnPO*_tmp151;const struct Cyc_Tcenv_Bindings*_tmp150;struct Cyc_Tcenv_CtrlEnv*_tmp14F;void*_tmp14E;void*_tmp14D;struct Cyc_Tcenv_FenvFlags _tmp14C;_LL86: _tmp153=_tmp148.shared;_tmp152=_tmp148.type_vars;_tmp151=_tmp148.region_order;_tmp150=_tmp148.locals;_tmp14F=_tmp148.ctrl_env;_tmp14E=_tmp148.capability;_tmp14D=_tmp148.curr_rgn;_tmp14C=_tmp148.flags;_LL87:;{
struct Cyc_Tcenv_Fenv*_tmp149=({struct Cyc_Tcenv_Fenv*_tmp14B=_region_malloc(r2,sizeof(*_tmp14B));_tmp14B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp153;_tmp14B->type_vars=(struct Cyc_List_List*)_tmp152;_tmp14B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp151;_tmp14B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp150;_tmp14B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14F;_tmp14B->capability=(void*)_tmp14E;_tmp14B->curr_rgn=(void*)_tmp14D;_tmp14B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp14C;_tmp14B->fnrgn=(struct _RegionHandle*)r2;_tmp14B;});
return({struct Cyc_Tcenv_Tenv*_tmp14A=_region_malloc(r2,sizeof(*_tmp14A));_tmp14A->ns=te->ns;_tmp14A->ae=te->ae;_tmp14A->le=_tmp149;_tmp14A->allow_valueof=1;_tmp14A->in_extern_c_include=te->in_extern_c_include;_tmp14A;});};};}
# 546
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp154=_region_malloc(r2,sizeof(*_tmp154));_tmp154->ns=te->ns;_tmp154->ae=te->ae;_tmp154->le=0;_tmp154->allow_valueof=te->allow_valueof;_tmp154->in_extern_c_include=1;_tmp154;});{
struct Cyc_Tcenv_Fenv _tmp155=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp156=_tmp155;struct Cyc_Tcenv_SharedFenv*_tmp161;struct Cyc_List_List*_tmp160;struct Cyc_RgnOrder_RgnPO*_tmp15F;const struct Cyc_Tcenv_Bindings*_tmp15E;struct Cyc_Tcenv_CtrlEnv*_tmp15D;void*_tmp15C;void*_tmp15B;struct Cyc_Tcenv_FenvFlags _tmp15A;_LL89: _tmp161=_tmp156.shared;_tmp160=_tmp156.type_vars;_tmp15F=_tmp156.region_order;_tmp15E=_tmp156.locals;_tmp15D=_tmp156.ctrl_env;_tmp15C=_tmp156.capability;_tmp15B=_tmp156.curr_rgn;_tmp15A=_tmp156.flags;_LL8A:;{
struct Cyc_Tcenv_Fenv*_tmp157=({struct Cyc_Tcenv_Fenv*_tmp159=_region_malloc(r2,sizeof(*_tmp159));_tmp159->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp161;_tmp159->type_vars=(struct Cyc_List_List*)_tmp160;_tmp159->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15F;_tmp159->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15E;_tmp159->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15D;_tmp159->capability=(void*)_tmp15C;_tmp159->curr_rgn=(void*)_tmp15B;_tmp159->flags=(struct Cyc_Tcenv_FenvFlags)_tmp15A;_tmp159->fnrgn=(struct _RegionHandle*)r2;_tmp159;});
return({struct Cyc_Tcenv_Tenv*_tmp158=_region_malloc(r2,sizeof(*_tmp158));_tmp158->ns=te->ns;_tmp158->ae=te->ae;_tmp158->le=_tmp157;_tmp158->allow_valueof=te->allow_valueof;_tmp158->in_extern_c_include=1;_tmp158;});};};}
# 554
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 557
struct Cyc_Tcenv_Fenv _tmp162=*
# 559
Cyc_Tcenv_get_fenv(te,({const char*_tmp177="add_region";_tag_dyneither(_tmp177,sizeof(char),11);}));
# 557
struct Cyc_Tcenv_Fenv _tmp163=_tmp162;struct Cyc_Tcenv_SharedFenv*_tmp176;struct Cyc_List_List*_tmp175;struct Cyc_RgnOrder_RgnPO*_tmp174;const struct Cyc_Tcenv_Bindings*_tmp173;struct Cyc_Tcenv_CtrlEnv*_tmp172;void*_tmp171;void*_tmp170;struct Cyc_Tcenv_FenvFlags _tmp16F;_LL8C: _tmp176=_tmp163.shared;_tmp175=_tmp163.type_vars;_tmp174=_tmp163.region_order;_tmp173=_tmp163.locals;_tmp172=_tmp163.ctrl_env;_tmp171=_tmp163.capability;_tmp170=_tmp163.curr_rgn;_tmp16F=_tmp163.flags;_LL8D:;
# 560
{void*_tmp164=Cyc_Tcutil_compress(rgn);void*_tmp165=_tmp164;struct Cyc_Absyn_Tvar*_tmp166;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp165)->tag == 2){_LL8F: _tmp166=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp165)->f1;_LL90:
# 562
 _tmp174=Cyc_RgnOrder_add_youngest(_tmp176->frgn,_tmp174,_tmp166,resetable,opened);
# 564
goto _LL8E;}else{_LL91: _LL92:
 goto _LL8E;}_LL8E:;}
# 567
_tmp171=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp168;_tmp168.tag=24;_tmp168.f1=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp16C;_tmp16C.tag=23;_tmp16C.f1=rgn;_tmp16C;});_tmp16B;});_tmp169->tl=({struct Cyc_List_List*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->hd=_tmp171;_tmp16A->tl=0;_tmp16A;});_tmp169;});_tmp168;});_tmp167;});{
# 569
struct Cyc_Tcenv_Fenv*_tmp16D=({struct Cyc_Tcenv_Fenv*_tmp16E=_region_malloc(r,sizeof(*_tmp16E));_tmp16E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp176;_tmp16E->type_vars=(struct Cyc_List_List*)_tmp175;_tmp16E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp174;_tmp16E->locals=(const struct Cyc_Tcenv_Bindings*)_tmp173;_tmp16E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp172;_tmp16E->capability=(void*)_tmp171;_tmp16E->curr_rgn=(void*)_tmp170;_tmp16E->flags=(struct Cyc_Tcenv_FenvFlags)_tmp16F;_tmp16E->fnrgn=(struct _RegionHandle*)r;_tmp16E;});
# 572
return Cyc_Tcenv_put_fenv(r,te,_tmp16D);};}
# 575
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 577
struct Cyc_Tcenv_Fenv _tmp178=*
# 579
Cyc_Tcenv_get_fenv(te,({const char*_tmp18E="new_named_block";_tag_dyneither(_tmp18E,sizeof(char),16);}));
# 577
struct Cyc_Tcenv_Fenv _tmp179=_tmp178;struct Cyc_Tcenv_SharedFenv*_tmp18D;struct Cyc_List_List*_tmp18C;struct Cyc_RgnOrder_RgnPO*_tmp18B;const struct Cyc_Tcenv_Bindings*_tmp18A;struct Cyc_Tcenv_CtrlEnv*_tmp189;void*_tmp188;void*_tmp187;struct Cyc_Tcenv_FenvFlags _tmp186;_LL94: _tmp18D=_tmp179.shared;_tmp18C=_tmp179.type_vars;_tmp18B=_tmp179.region_order;_tmp18A=_tmp179.locals;_tmp189=_tmp179.ctrl_env;_tmp188=_tmp179.capability;_tmp187=_tmp179.curr_rgn;_tmp186=_tmp179.flags;_LL95:;{
# 581
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp185="new_block";_tag_dyneither(_tmp185,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp184;_tmp184.tag=2;_tmp184.f1=block_rgn;_tmp184;});_tmp183;});
_tmp18C=({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->hd=block_rgn;_tmp17A->tl=_tmp18C;_tmp17A;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp18C);
_tmp18B=Cyc_RgnOrder_add_youngest(_tmp18D->frgn,_tmp18B,block_rgn,0,0);
_tmp188=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp17C;_tmp17C.tag=24;_tmp17C.f1=({struct Cyc_List_List*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp180;_tmp180.tag=23;_tmp180.f1=block_typ;_tmp180;});_tmp17F;});_tmp17D->tl=({struct Cyc_List_List*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->hd=_tmp188;_tmp17E->tl=0;_tmp17E;});_tmp17D;});_tmp17C;});_tmp17B;});
_tmp187=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp181=({struct Cyc_Tcenv_Fenv*_tmp182=_region_malloc(r,sizeof(*_tmp182));_tmp182->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp18D;_tmp182->type_vars=(struct Cyc_List_List*)_tmp18C;_tmp182->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp18B;_tmp182->locals=(const struct Cyc_Tcenv_Bindings*)_tmp18A;_tmp182->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp189;_tmp182->capability=(void*)_tmp188;_tmp182->curr_rgn=(void*)_tmp187;_tmp182->flags=(struct Cyc_Tcenv_FenvFlags)_tmp186;_tmp182->fnrgn=(struct _RegionHandle*)r;_tmp182;});
# 591
return Cyc_Tcenv_put_fenv(r,te,_tmp181);};};}
# 594
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 596
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 598
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 604
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 608
struct Cyc_Tcenv_Fenv _tmp190=*
# 610
Cyc_Tcenv_get_fenv(te,({const char*_tmp19C="new_outlives_constraints";_tag_dyneither(_tmp19C,sizeof(char),25);}));
# 608
struct Cyc_Tcenv_Fenv _tmp191=_tmp190;struct Cyc_Tcenv_SharedFenv*_tmp19B;struct Cyc_List_List*_tmp19A;struct Cyc_RgnOrder_RgnPO*_tmp199;const struct Cyc_Tcenv_Bindings*_tmp198;struct Cyc_Tcenv_CtrlEnv*_tmp197;void*_tmp196;void*_tmp195;struct Cyc_Tcenv_FenvFlags _tmp194;_LL97: _tmp19B=_tmp191.shared;_tmp19A=_tmp191.type_vars;_tmp199=_tmp191.region_order;_tmp198=_tmp191.locals;_tmp197=_tmp191.ctrl_env;_tmp196=_tmp191.capability;_tmp195=_tmp191.curr_rgn;_tmp194=_tmp191.flags;_LL98:;
# 611
for(0;cs != 0;cs=cs->tl){
_tmp199=
Cyc_RgnOrder_add_outlives_constraint(_tmp19B->frgn,_tmp199,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 616
struct Cyc_Tcenv_Fenv*_tmp192=({struct Cyc_Tcenv_Fenv*_tmp193=_region_malloc(r,sizeof(*_tmp193));_tmp193->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp19B;_tmp193->type_vars=(struct Cyc_List_List*)_tmp19A;_tmp193->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp199;_tmp193->locals=(const struct Cyc_Tcenv_Bindings*)_tmp198;_tmp193->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp197;_tmp193->capability=(void*)_tmp196;_tmp193->curr_rgn=(void*)_tmp195;_tmp193->flags=(struct Cyc_Tcenv_FenvFlags)_tmp194;_tmp193->fnrgn=(struct _RegionHandle*)r;_tmp193;});
# 619
return Cyc_Tcenv_put_fenv(r,te,_tmp192);};}
# 622
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 630
void*_tmp19D=Cyc_Tcutil_compress(r1);
void*_tmp19E=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp19F=Cyc_Tcutil_typ_kind(_tmp19D);
struct Cyc_Absyn_Kind*_tmp1A0=Cyc_Tcutil_typ_kind(_tmp19E);
# 636
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp19F,_tmp1A0);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp1A0,_tmp19F);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp1A6;_tmp1A6.tag=0;_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp1A0));({struct Cyc_String_pa_PrintArg_struct _tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp19F));({struct Cyc_String_pa_PrintArg_struct _tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp19E));({struct Cyc_String_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp19D));({void*_tmp1A1[4]={& _tmp1A3,& _tmp1A4,& _tmp1A5,& _tmp1A6};Cyc_Tcutil_terr(loc,({const char*_tmp1A2="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp1A2,sizeof(char),82);}),_tag_dyneither(_tmp1A1,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 643
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp1A7=Cyc_Tcutil_swap_kind(_tmp19E,Cyc_Tcutil_kind_to_bound(_tmp19F));struct _tuple11 _tmp1A8=_tmp1A7;struct Cyc_Absyn_Tvar*_tmp1AB;void*_tmp1AA;_LL9A: _tmp1AB=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;_LL9B:;
*oldtv=({struct _tuple11*_tmp1A9=_region_malloc(r,sizeof(*_tmp1A9));_tmp1A9->f1=_tmp1AB;_tmp1A9->f2=_tmp1AA;_tmp1A9;});}else{
# 647
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp1AC=Cyc_Tcutil_swap_kind(_tmp19D,Cyc_Tcutil_kind_to_bound(_tmp1A0));struct _tuple11 _tmp1AD=_tmp1AC;struct Cyc_Absyn_Tvar*_tmp1B0;void*_tmp1AF;_LL9D: _tmp1B0=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;_LL9E:;
*oldtv=({struct _tuple11*_tmp1AE=_region_malloc(r,sizeof(*_tmp1AE));_tmp1AE->f1=_tmp1B0;_tmp1AE->f2=_tmp1AF;_tmp1AE;});}}}{
# 653
struct _RegionHandle*_tmp1B1=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp1B2=0;
if((_tmp19D != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp19D != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp19D != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1B6;_tmp1B6.tag=23;_tmp1B6.f1=_tmp19D;_tmp1B6;});_tmp1B5;});
_tmp1B2=({struct Cyc_List_List*_tmp1B3=_region_malloc(_tmp1B1,sizeof(*_tmp1B3));_tmp1B3->hd=({struct _tuple13*_tmp1B4=_region_malloc(_tmp1B1,sizeof(*_tmp1B4));_tmp1B4->f1=eff1;_tmp1B4->f2=_tmp19E;_tmp1B4;});_tmp1B3->tl=_tmp1B2;_tmp1B3;});}
# 659
if((_tmp19E != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp19E != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp19E != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1BA;_tmp1BA.tag=23;_tmp1BA.f1=_tmp19E;_tmp1BA;});_tmp1B9;});
_tmp1B2=({struct Cyc_List_List*_tmp1B7=_region_malloc(_tmp1B1,sizeof(*_tmp1B7));_tmp1B7->hd=({struct _tuple13*_tmp1B8=_region_malloc(_tmp1B1,sizeof(*_tmp1B8));_tmp1B8->f1=eff2;_tmp1B8->f2=_tmp19D;_tmp1B8;});_tmp1B7->tl=_tmp1B2;_tmp1B7;});}
# 664
return Cyc_Tcenv_new_outlives_constraints(_tmp1B1,te,_tmp1B2,loc);};}
# 667
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1BB=te->le;
if(_tmp1BB == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp1BB;
struct Cyc_Tcenv_Fenv*_tmp1BC=fe;void*_tmp1BD;_LLA0: _tmp1BD=_tmp1BC->curr_rgn;_LLA1:;
return _tmp1BD;};}
# 678
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp1C6="check_rgn_accessible";_tag_dyneither(_tmp1C6,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp1BE=fe;void*_tmp1C5;struct Cyc_RgnOrder_RgnPO*_tmp1C4;_LLA3: _tmp1C5=_tmp1BE->capability;_tmp1C4=_tmp1BE->region_order;_LLA4:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp1C5) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp1C5))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp1C4,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1C0;_tmp1C0.tag=23;_tmp1C0.f1=rgn;_tmp1C0;});_tmp1BF;}),_tmp1C5))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp1C1[1]={& _tmp1C3};Cyc_Tcutil_terr(loc,({const char*_tmp1C2="Expression accesses unavailable region %s";_tag_dyneither(_tmp1C2,sizeof(char),42);}),_tag_dyneither(_tmp1C1,sizeof(void*),1));});});}
# 690
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp1C7=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1D2="check_rgn_resetable";_tag_dyneither(_tmp1D2,sizeof(char),20);}));
# 692
struct Cyc_Tcenv_Fenv*_tmp1C8=_tmp1C7;struct Cyc_RgnOrder_RgnPO*_tmp1D1;_LLA6: _tmp1D1=_tmp1C8->region_order;_LLA7:;{
# 694
void*_tmp1C9=Cyc_Tcutil_compress(rgn);void*_tmp1CA=_tmp1C9;struct Cyc_Absyn_Tvar*_tmp1D0;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CA)->tag == 2){_LLA9: _tmp1D0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CA)->f1;_LLAA:
# 696
 if(!Cyc_RgnOrder_is_region_resetable(_tmp1D1,_tmp1D0))
({struct Cyc_String_pa_PrintArg_struct _tmp1CD;_tmp1CD.tag=0;_tmp1CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp1CB[1]={& _tmp1CD};Cyc_Tcutil_terr(loc,({const char*_tmp1CC="Region %s is not resetable";_tag_dyneither(_tmp1CC,sizeof(char),27);}),_tag_dyneither(_tmp1CB,sizeof(void*),1));});});
return;}else{_LLAB: _LLAC:
({void*_tmp1CE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CF="check_rgn_resetable";_tag_dyneither(_tmp1CF,sizeof(char),20);}),_tag_dyneither(_tmp1CE,sizeof(void*),0));});}_LLA8:;};};}
# 706
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp1D3=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 711
if(_tmp1D3 == 0){
void*_tmp1D4=rt_a;switch(*((int*)_tmp1D4)){case 22: _LLAE: _LLAF:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LLB0: _LLB1:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LLB2: _LLB3:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LLB4: _LLB5:
 return 0;}_LLAD:;}{
# 719
struct Cyc_Tcenv_Fenv*fe=_tmp1D3;
struct Cyc_Tcenv_Fenv*_tmp1D5=fe;struct Cyc_RgnOrder_RgnPO*_tmp1D9;_LLB7: _tmp1D9=_tmp1D5->region_order;_LLB8:;{
int _tmp1D6=Cyc_RgnOrder_effect_outlives(_tmp1D9,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1D8;_tmp1D8.tag=23;_tmp1D8.f1=rt_a;_tmp1D8;});_tmp1D7;}),rt_b);
# 725
return _tmp1D6;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 730
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp1DA=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1E1="check_effect_accessible";_tag_dyneither(_tmp1E1,sizeof(char),24);}));
# 731
struct Cyc_Tcenv_Fenv*_tmp1DB=_tmp1DA;void*_tmp1E0;struct Cyc_RgnOrder_RgnPO*_tmp1DF;struct Cyc_Tcenv_SharedFenv*_tmp1DE;_LLBA: _tmp1E0=_tmp1DB->capability;_tmp1DF=_tmp1DB->region_order;_tmp1DE=_tmp1DB->shared;_LLBB:;
# 733
if(Cyc_Tcutil_subset_effect(0,eff,_tmp1E0))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp1DF,eff,_tmp1E0))
return;{
struct _RegionHandle*frgn=_tmp1DE->frgn;
_tmp1DE->delayed_effect_checks=({struct Cyc_List_List*_tmp1DC=_region_malloc(frgn,sizeof(*_tmp1DC));_tmp1DC->hd=({struct _tuple14*_tmp1DD=_region_malloc(frgn,sizeof(*_tmp1DD));_tmp1DD->f1=_tmp1E0;_tmp1DD->f2=eff;_tmp1DD->f3=_tmp1DF;_tmp1DD->f4=loc;_tmp1DD;});_tmp1DC->tl=_tmp1DE->delayed_effect_checks;_tmp1DC;});};}
# 742
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1E2=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1F0="check_delayed_effects";_tag_dyneither(_tmp1F0,sizeof(char),22);}));
# 743
struct Cyc_Tcenv_Fenv*_tmp1E3=_tmp1E2;struct Cyc_Tcenv_SharedFenv*_tmp1EF;_LLBD: _tmp1EF=_tmp1E3->shared;_LLBE:;{
# 745
struct Cyc_List_List*_tmp1E4=_tmp1EF->delayed_effect_checks;
for(0;_tmp1E4 != 0;_tmp1E4=_tmp1E4->tl){
struct _tuple14*_tmp1E5=(struct _tuple14*)_tmp1E4->hd;struct _tuple14*_tmp1E6=_tmp1E5;void*_tmp1EE;void*_tmp1ED;struct Cyc_RgnOrder_RgnPO*_tmp1EC;unsigned int _tmp1EB;_LLC0: _tmp1EE=_tmp1E6->f1;_tmp1ED=_tmp1E6->f2;_tmp1EC=_tmp1E6->f3;_tmp1EB=_tmp1E6->f4;_LLC1:;
if(Cyc_Tcutil_subset_effect(1,_tmp1ED,_tmp1EE))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp1EC,_tmp1ED,_tmp1EE))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1ED));({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1EE));({void*_tmp1E7[2]={& _tmp1E9,& _tmp1EA};Cyc_Tcutil_terr(_tmp1EB,({const char*_tmp1E8="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp1E8,sizeof(char),51);}),_tag_dyneither(_tmp1E7,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 761
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 763
struct Cyc_Tcenv_Fenv*_tmp1F1=te->le;
if(_tmp1F1 == 0){
# 766
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1F3;_tmp1F3.tag=23;_tmp1F3.f1=(*((struct _tuple13*)po->hd)).f2;_tmp1F3;});_tmp1F2;}),Cyc_Absyn_empty_effect))
({void*_tmp1F4=0;Cyc_Tcutil_terr(loc,({const char*_tmp1F5="the required region ordering is not satisfied here";_tag_dyneither(_tmp1F5,sizeof(char),51);}),_tag_dyneither(_tmp1F4,sizeof(void*),0));});}
return;}{
# 772
struct Cyc_Tcenv_Fenv*_tmp1F6=_tmp1F1;struct Cyc_Tcenv_Fenv*_tmp1F7=_tmp1F6;struct Cyc_RgnOrder_RgnPO*_tmp1FB;struct Cyc_Tcenv_SharedFenv*_tmp1FA;_LLC3: _tmp1FB=_tmp1F7->region_order;_tmp1FA=_tmp1F7->shared;_LLC4:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp1FB,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp1FA->delayed_constraint_checks=({struct Cyc_List_List*_tmp1F8=_region_malloc(_tmp1FA->frgn,sizeof(*_tmp1F8));_tmp1F8->hd=({struct _tuple15*_tmp1F9=_region_malloc(_tmp1FA->frgn,sizeof(*_tmp1F9));_tmp1F9->f1=_tmp1FB;_tmp1F9->f2=po;_tmp1F9->f3=loc;_tmp1F9;});_tmp1F8->tl=_tmp1FA->delayed_constraint_checks;_tmp1F8;});};}
# 779
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1FC=
Cyc_Tcenv_get_fenv(te,({const char*_tmp207="check_delayed_constraints";_tag_dyneither(_tmp207,sizeof(char),26);}));
# 780
struct Cyc_Tcenv_Fenv*_tmp1FD=_tmp1FC;struct Cyc_Tcenv_SharedFenv*_tmp206;_LLC6: _tmp206=_tmp1FD->shared;_LLC7:;{
# 782
struct Cyc_List_List*_tmp1FE=_tmp206->delayed_constraint_checks;
for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){
struct _tuple15*_tmp1FF=(struct _tuple15*)_tmp1FE->hd;struct _tuple15*_tmp200=_tmp1FF;struct Cyc_RgnOrder_RgnPO*_tmp205;struct Cyc_List_List*_tmp204;unsigned int _tmp203;_LLC9: _tmp205=_tmp200->f1;_tmp204=_tmp200->f2;_tmp203=_tmp200->f3;_LLCA:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp205,_tmp204,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp201=0;Cyc_Tcutil_terr(_tmp203,({const char*_tmp202="the required region ordering is not satisfied here";_tag_dyneither(_tmp202,sizeof(char),51);}),_tag_dyneither(_tmp201,sizeof(void*),0));});}};}
# 790
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->name=({struct _dyneither_ptr*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp227;_tmp227.tag=0;_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp225[1]={& _tmp227};Cyc_aprintf(({const char*_tmp226="`%s";_tag_dyneither(_tmp226,sizeof(char),4);}),_tag_dyneither(_tmp225,sizeof(void*),1));});});_tmp224;});_tmp223->identity=
# 794
Cyc_Tcutil_new_tvar_id();_tmp223->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp223;});
struct Cyc_List_List*_tmp208=({struct Cyc_List_List*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->hd=rgn0;_tmp222->tl=fd->tvs;_tmp222;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp208);{
# 798
struct Cyc_RgnOrder_RgnPO*_tmp209=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 800
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp221;_tmp221.tag=2;_tmp221.f1=rgn0;_tmp221;});_tmp220;});
{struct Cyc_List_List*_tmp20A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp20A != 0;_tmp20A=_tmp20A->tl){
struct Cyc_Absyn_Vardecl*_tmp20B=(struct Cyc_Absyn_Vardecl*)_tmp20A->hd;
_tmp20B->rgn=param_rgn;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp20C=_region_malloc(r,sizeof(*_tmp20C));_tmp20C->v=(*_tmp20B->name).f2;_tmp20C->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp20E;_tmp20E.tag=3;_tmp20E.f1=_tmp20B;_tmp20E;});_tmp20D;});_tmp20C->tl=locals;_tmp20C;});}}
# 806
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp20F=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp210=_tmp20F;struct _dyneither_ptr*_tmp216;struct Cyc_Absyn_Tqual _tmp215;void*_tmp214;int _tmp213;_LLCC: _tmp216=_tmp210.name;_tmp215=_tmp210.tq;_tmp214=_tmp210.type;_tmp213=_tmp210.inject;_LLCD:;{
# 809
struct Cyc_List_List*_tmp211=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
struct Cyc_Absyn_Vardecl*_tmp212=(struct Cyc_Absyn_Vardecl*)_tmp211->hd;
if(Cyc_strptrcmp((*_tmp212->name).f2,(struct _dyneither_ptr*)_check_null(_tmp216))== 0){
_tmp212->type=Cyc_Absyn_dyneither_typ(_tmp214,param_rgn,_tmp215,Cyc_Absyn_false_conref);
break;}}};}
# 817
return({struct Cyc_Tcenv_Fenv*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp21F=_region_malloc(r,sizeof(*_tmp21F));_tmp21F->frgn=r;_tmp21F->return_typ=fd->ret_type;_tmp21F->delayed_effect_checks=0;_tmp21F->delayed_constraint_checks=0;_tmp21F;});_tmp217->type_vars=(struct Cyc_List_List*)_tmp208;_tmp217->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp209;_tmp217->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp217->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp21E=_region_malloc(r,sizeof(*_tmp21E));_tmp21E->fallthru_clause=0;_tmp21E;});_tmp217->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp219;_tmp219.tag=24;_tmp219.f1=({struct Cyc_List_List*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp21D;_tmp21D.tag=23;_tmp21D.f1=param_rgn;_tmp21D;});_tmp21C;});_tmp21A->tl=({struct Cyc_List_List*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->hd=(void*)_check_null(fd->effect);_tmp21B->tl=0;_tmp21B;});_tmp21A;});_tmp219;});_tmp218;}));_tmp217->curr_rgn=(void*)param_rgn;_tmp217->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp217->flags).in_notreadctxt=0;(_tmp217->flags).in_lhs=0;(_tmp217->flags).in_new=0;(_tmp217->flags).abstract_ok=0;_tmp217->flags;});_tmp217->fnrgn=(struct _RegionHandle*)r;_tmp217;});};}
# 836
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 839
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp248->identity=Cyc_Tcutil_new_tvar_id();_tmp248->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp248;});
struct Cyc_List_List*_tmp228=({struct Cyc_List_List*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->hd=rgn0;_tmp247->tl=0;_tmp247;});
struct Cyc_RgnOrder_RgnPO*_tmp229=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp246;_tmp246.tag=24;_tmp246.f1=0;_tmp246;});_tmp245;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp244;_tmp244.tag=2;_tmp244.f1=rgn0;_tmp244;});_tmp243;});
struct Cyc_List_List*_tmp22A=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp22B=*((struct _tuple8*)args->hd);struct _tuple8 _tmp22C=_tmp22B;struct _dyneither_ptr*_tmp23A;struct Cyc_Absyn_Tqual _tmp239;void*_tmp238;_LLCF: _tmp23A=_tmp22C.f1;_tmp239=_tmp22C.f2;_tmp238=_tmp22C.f3;_LLD0:;
if(_tmp23A != 0){
struct Cyc_Absyn_Vardecl*_tmp22D=({struct Cyc_Absyn_Vardecl*_tmp235=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp235[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp236;_tmp236.sc=Cyc_Absyn_Public;_tmp236.name=({struct _tuple0*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237->f1=Cyc_Absyn_Loc_n;_tmp237->f2=_tmp23A;_tmp237;});_tmp236.tq=_tmp239;_tmp236.type=_tmp238;_tmp236.initializer=0;_tmp236.rgn=param_rgn;_tmp236.attributes=0;_tmp236.escapes=0;_tmp236;});_tmp235;});
# 855
_tmp22A=({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp22E[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp22F;_tmp22F.hd=_tmp22D;_tmp22F.tl=_tmp22A;_tmp22F;});_tmp22E;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp230=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp234;_tmp234.tag=3;_tmp234.f1=_tmp22D;_tmp234;});_tmp233;});
struct _dyneither_ptr*_tmp231=_tmp23A;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp232=_region_malloc(r,sizeof(*_tmp232));_tmp232->v=_tmp231;_tmp232->b=(void*)_tmp230;_tmp232->tl=locals;_tmp232;});};}}
# 861
return({struct Cyc_Tcenv_Fenv*_tmp23B=_region_malloc(r,sizeof(*_tmp23B));_tmp23B->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp242=_region_malloc(r,sizeof(*_tmp242));_tmp242->frgn=r;_tmp242->return_typ=ret_type;_tmp242->delayed_effect_checks=0;_tmp242->delayed_constraint_checks=0;_tmp242;});_tmp23B->type_vars=(struct Cyc_List_List*)_tmp228;_tmp23B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp229;_tmp23B->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp23B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp241=_region_malloc(r,sizeof(*_tmp241));_tmp241->fallthru_clause=0;_tmp241;});_tmp23B->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp23D;_tmp23D.tag=24;_tmp23D.f1=({struct Cyc_List_List*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp240;_tmp240.tag=23;_tmp240.f1=param_rgn;_tmp240;});_tmp23F;});_tmp23E->tl=0;_tmp23E;});_tmp23D;});_tmp23C;}));_tmp23B->curr_rgn=(void*)param_rgn;_tmp23B->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp23B->flags).in_notreadctxt=0;(_tmp23B->flags).in_lhs=0;(_tmp23B->flags).in_new=0;(_tmp23B->flags).abstract_ok=0;_tmp23B->flags;});_tmp23B->fnrgn=(struct _RegionHandle*)r;_tmp23B;});}
# 879
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp24A=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp277;struct Cyc_RgnOrder_RgnPO*_tmp276;struct Cyc_List_List*_tmp275;struct Cyc_Tcenv_SharedFenv*_tmp274;struct _RegionHandle*_tmp273;_LLD2: _tmp277=_tmp24A->locals;_tmp276=_tmp24A->region_order;_tmp275=_tmp24A->type_vars;_tmp274=_tmp24A->shared;_tmp273=_tmp24A->fnrgn;_LLD3:;{
# 882
struct _RegionHandle*_tmp24B=_tmp274->frgn;
const struct Cyc_Tcenv_Bindings*_tmp24C=_tmp277;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->name=({struct _dyneither_ptr*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp272;_tmp272.tag=0;_tmp272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp270[1]={& _tmp272};Cyc_aprintf(({const char*_tmp271="`%s";_tag_dyneither(_tmp271,sizeof(char),4);}),_tag_dyneither(_tmp270,sizeof(void*),1));});});_tmp26F;});_tmp26E->identity=
# 886
Cyc_Tcutil_new_tvar_id();_tmp26E->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp26E;});
# 888
{struct Cyc_List_List*_tmp24D=fd->tvs;for(0;_tmp24D != 0;_tmp24D=_tmp24D->tl){
struct Cyc_Absyn_Kind*_tmp24E=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp24D->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp24F=_tmp24E;enum Cyc_Absyn_KindQual _tmp253;enum Cyc_Absyn_AliasQual _tmp252;_LLD5: _tmp253=_tmp24F->kind;_tmp252=_tmp24F->aliasqual;_LLD6:;
if(_tmp253 == Cyc_Absyn_RgnKind){
if(_tmp252 == Cyc_Absyn_Aliasable)
_tmp276=Cyc_RgnOrder_add_youngest(_tmp24B,_tmp276,(struct Cyc_Absyn_Tvar*)_tmp24D->hd,0,0);else{
# 894
({void*_tmp250=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp251="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp251,sizeof(char),39);}),_tag_dyneither(_tmp250,sizeof(void*),0));});}}}}
# 896
_tmp276=Cyc_RgnOrder_add_youngest(_tmp24B,_tmp276,rgn0,0,0);{
struct Cyc_List_List*_tmp254=({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=rgn0;_tmp26D->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp275);_tmp26D;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp254);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp26C;_tmp26C.tag=2;_tmp26C.f1=rgn0;_tmp26C;});_tmp26B;});
{struct Cyc_List_List*_tmp255=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp255 != 0;_tmp255=_tmp255->tl){
struct Cyc_Absyn_Vardecl*_tmp256=(struct Cyc_Absyn_Vardecl*)_tmp255->hd;
_tmp256->rgn=param_rgn;
_tmp24C=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp257=_region_malloc(_tmp24B,sizeof(*_tmp257));_tmp257->v=(*_tmp256->name).f2;_tmp257->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp259;_tmp259.tag=3;_tmp259.f1=_tmp256;_tmp259;});_tmp258;});_tmp257->tl=_tmp24C;_tmp257;});}}
# 905
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp25A=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp25B=_tmp25A;struct _dyneither_ptr*_tmp261;struct Cyc_Absyn_Tqual _tmp260;void*_tmp25F;int _tmp25E;_LLD8: _tmp261=_tmp25B.name;_tmp260=_tmp25B.tq;_tmp25F=_tmp25B.type;_tmp25E=_tmp25B.inject;_LLD9:;{
# 908
struct Cyc_List_List*_tmp25C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){
struct Cyc_Absyn_Vardecl*_tmp25D=(struct Cyc_Absyn_Vardecl*)_tmp25C->hd;
if(Cyc_strptrcmp((*_tmp25D->name).f2,(struct _dyneither_ptr*)_check_null(_tmp261))== 0){
_tmp25D->type=Cyc_Absyn_dyneither_typ(_tmp25F,param_rgn,_tmp260,Cyc_Absyn_false_conref);
break;}}};}
# 916
return({struct Cyc_Tcenv_Fenv*_tmp262=_region_malloc(_tmp24B,sizeof(*_tmp262));_tmp262->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp26A=_region_malloc(_tmp24B,sizeof(*_tmp26A));_tmp26A->frgn=_tmp24B;_tmp26A->return_typ=fd->ret_type;_tmp26A->delayed_effect_checks=0;_tmp26A->delayed_constraint_checks=0;_tmp26A;});_tmp262->type_vars=(struct Cyc_List_List*)_tmp254;_tmp262->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp276;_tmp262->locals=(const struct Cyc_Tcenv_Bindings*)_tmp24C;_tmp262->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp269=_region_malloc(_tmp24B,sizeof(*_tmp269));_tmp269->fallthru_clause=0;_tmp269;});_tmp262->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp264;_tmp264.tag=24;_tmp264.f1=({struct Cyc_List_List*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp268;_tmp268.tag=23;_tmp268.f1=param_rgn;_tmp268;});_tmp267;});_tmp265->tl=({struct Cyc_List_List*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->hd=(void*)_check_null(fd->effect);_tmp266->tl=0;_tmp266;});_tmp265;});_tmp264;});_tmp263;}));_tmp262->curr_rgn=(void*)param_rgn;_tmp262->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp262->flags).in_notreadctxt=0;(_tmp262->flags).in_lhs=0;(_tmp262->flags).in_new=0;(_tmp262->flags).abstract_ok=0;_tmp262->flags;});_tmp262->fnrgn=(struct _RegionHandle*)_tmp273;_tmp262;});};};};}
