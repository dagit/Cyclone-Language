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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 775 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 942
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 951
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 965
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 985
extern void*Cyc_Absyn_empty_effect;
# 987
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1020
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1107
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
# 89
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
# 176
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 182
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 186
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 188
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 192
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 195
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 206
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 208
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 212
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 214
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 218
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 221
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 224
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 227
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
# 142
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{const struct _tuple10*fallthru_clause;};
# 148
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 173 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 208 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 211
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 215
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 220
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
return({struct Cyc_Tcenv_Genv*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->grgn=r;_tmp2->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp2->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->availables=0;_tmp2;});}struct _tuple12{void*f1;int f2;};
# 231
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 233
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp3;});{
# 235
struct Cyc_Tcenv_Genv*_tmp4=Cyc_Tcenv_empty_genv(r);
_tmp4->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Datatypedecl**_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=Cyc_Absyn_exn_tud;_tmp5;}));
{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
_tmp4->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp4->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd)->name).f2,({struct _tuple12*_tmp7=_region_malloc(r,sizeof(*_tmp7));_tmp7->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp9;_tmp9.tag=2;_tmp9.f1=Cyc_Absyn_exn_tud;_tmp9.f2=(struct Cyc_Absyn_Datatypefield*)_tmp6->hd;_tmp9;});_tmp8;});_tmp7->f2=1;_tmp7;}));}}{
# 242
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp4);
return({struct Cyc_Tcenv_Tenv*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->ns=0;_tmpA->ae=ae;_tmpA->le=0;_tmpA->allow_valueof=0;_tmpA->in_extern_c_include=0;_tmpA;});};};}
# 248
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 250
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 254
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0)
return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpB="outer_namespace";_tag_dyneither(_tmpB,sizeof(char),16);}));
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 260
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 267
if(n2 != 0)return 0;
return 1;}
# 273
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 278
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpC;_push_handler(& _tmpC);{int _tmpE=0;if(setjmp(_tmpC.handler))_tmpE=1;if(!_tmpE){
lookup(ge2,v,0);
({struct Cyc_String_pa_PrintArg_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmpF[1]={& _tmp11};Cyc_Tcutil_terr(loc,({const char*_tmp10="%s is ambiguous";_tag_dyneither(_tmp10,sizeof(char),16);}),_tag_dyneither(_tmpF,sizeof(void*),1));});});
# 282
;_pop_handler();}else{void*_tmpD=(void*)_exn_thrown;void*_tmp12=_tmpD;void*_tmp13;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp12)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 284
 goto _LL0;}else{_LL3: _tmp13=_tmp12;_LL4:(int)_rethrow(_tmp13);}_LL0:;}};}}
# 286
return;}
# 289
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 294
{struct _handler_cons _tmp14;_push_handler(& _tmp14);{int _tmp16=0;if(setjmp(_tmp14.handler))_tmp16=1;if(!_tmp16){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp17=result;_npop_handler(0);return _tmp17;};}
# 295
;_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp18=_tmp15;void*_tmp19;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp18)->tag == Cyc_Dict_Absent){_LL6: _LL7:
# 298
 goto _LL5;}else{_LL8: _tmp19=_tmp18;_LL9:(int)_rethrow(_tmp19);}_LL5:;}};}
# 300
{struct Cyc_List_List*_tmp1A=ge->availables;for(0;_tmp1A != 0;_tmp1A=_tmp1A->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp1A->hd);
struct _handler_cons _tmp1B;_push_handler(& _tmp1B);{int _tmp1D=0;if(setjmp(_tmp1B.handler))_tmp1D=1;if(!_tmp1D){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp1A->hd,_tmp1A->tl);{
void*_tmp1E=result;_npop_handler(0);return _tmp1E;};}
# 303
;_pop_handler();}else{void*_tmp1C=(void*)_exn_thrown;void*_tmp1F=_tmp1C;void*_tmp20;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1F)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 306
 goto _LLA;}else{_LLD: _tmp20=_tmp1F;_LLE:(int)_rethrow(_tmp20);}_LLA:;}};}}
# 308
if(cns == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp21=_cycalloc_atomic(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Dict_Absent_exn_struct _tmp22;_tmp22.tag=Cyc_Dict_Absent;_tmp22;});_tmp21;}));
# 311
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 315
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp23=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp23,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 322
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple0*_tmp24=q;union Cyc_Absyn_Nmspace _tmp25;struct _dyneither_ptr*_tmp26;_LL10: _tmp25=_tmp24->f1;_tmp26=_tmp24->f2;_LL11:;{
union Cyc_Absyn_Nmspace _tmp27=_tmp25;struct Cyc_List_List*_tmp28;struct Cyc_List_List*_tmp29;struct _dyneither_ptr*_tmp2A;struct Cyc_List_List*_tmp2B;switch((_tmp27.C_n).tag){case 4: _LL13: _LL14:
 goto _LL16;case 1: if((_tmp27.Rel_n).val == 0){_LL15: _LL16:
# 327
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp26,is_use);}else{_LL17: _tmp2A=(struct _dyneither_ptr*)((_tmp27.Rel_n).val)->hd;_tmp2B=((_tmp27.Rel_n).val)->tl;_LL18: {
# 329
struct Cyc_Tcenv_Genv*_tmp2C=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2A,_tmp2B);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2C,_tmp26,is_use);}}case 3: _LL19: _tmp29=(_tmp27.C_n).val;_LL1A:
 _tmp28=_tmp29;goto _LL1C;default: _LL1B: _tmp28=(_tmp27.Abs_n).val;_LL1C:
# 333
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp28),_tmp26,is_use);}_LL12:;};}
# 338
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 340
struct Cyc_List_List*_tmp2D=te->ns;
struct _RegionHandle _tmp2E=_new_region("temp");struct _RegionHandle*temp=& _tmp2E;_push_region(temp);
{struct Cyc_List_List*_tmp2F=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp30=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2D);
struct Cyc_List_List*_tmp31=_tmp30->availables;
struct Cyc_Set_Set*_tmp32=_tmp30->namespaces;
# 348
{struct Cyc_List_List*_tmp33=_tmp31;for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
struct Cyc_Set_Set*_tmp34=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp33->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp34,n))
_tmp2F=({struct Cyc_List_List*_tmp35=_region_malloc(temp,sizeof(*_tmp35));_tmp35->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2D,({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->hd=n;_tmp36->tl=ns;_tmp36;}));_tmp35->tl=_tmp2F;_tmp35;});}}
# 355
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp32,n))
_tmp2F=({struct Cyc_List_List*_tmp37=_region_malloc(temp,sizeof(*_tmp37));_tmp37->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2D,({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->hd=n;_tmp38->tl=ns;_tmp38;}));_tmp37->tl=_tmp2F;_tmp37;});
# 361
if(_tmp2F != 0){
if(_tmp2F->tl != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n);({void*_tmp39[1]={& _tmp3B};Cyc_Tcutil_terr(loc,({const char*_tmp3A="%s is ambiguous";_tag_dyneither(_tmp3A,sizeof(char),16);}),_tag_dyneither(_tmp39,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp3C=(struct Cyc_List_List*)_tmp2F->hd;_npop_handler(0);return _tmp3C;};}
# 366
if(_tmp2D == 0)
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp3D=_cycalloc_atomic(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Dict_Absent_exn_struct _tmp3E;_tmp3E.tag=Cyc_Dict_Absent;_tmp3E;});_tmp3D;}));
# 369
_tmp2D=Cyc_Tcenv_outer_namespace(_tmp2D);}}
# 342
;_pop_region(temp);}
# 377
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp3F=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp3F,v);}
# 381
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp40=q;union Cyc_Absyn_Nmspace _tmp41;struct _dyneither_ptr*_tmp42;_LL1E: _tmp41=_tmp40->f1;_tmp42=_tmp40->f2;_LL1F:;{
union Cyc_Absyn_Nmspace _tmp43=_tmp41;struct Cyc_List_List*_tmp44;struct Cyc_List_List*_tmp45;struct _dyneither_ptr*_tmp46;struct Cyc_List_List*_tmp47;switch((_tmp43.C_n).tag){case 4: _LL21: _LL22:
 goto _LL24;case 1: if((_tmp43.Rel_n).val == 0){_LL23: _LL24:
# 386
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp42,0);}else{_LL29: _tmp46=(struct _dyneither_ptr*)((_tmp43.Rel_n).val)->hd;_tmp47=((_tmp43.Rel_n).val)->tl;_LL2A: {
# 392
struct Cyc_Dict_Dict _tmp49=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp46,_tmp47))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp49,_tmp42);}}case 3: _LL25: _tmp45=(_tmp43.C_n).val;_LL26:
# 387
 _tmp44=_tmp45;goto _LL28;default: _LL27: _tmp44=(_tmp43.Abs_n).val;_LL28: {
# 389
struct Cyc_Dict_Dict _tmp48=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp44))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp48,_tmp42);}}_LL20:;};}
# 397
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4A=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4A,v);}
# 401
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp4B=q;union Cyc_Absyn_Nmspace _tmp4C;struct _dyneither_ptr*_tmp4D;_LL2C: _tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;_LL2D:;{
union Cyc_Absyn_Nmspace _tmp4E=_tmp4C;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp50;struct _dyneither_ptr*_tmp51;struct Cyc_List_List*_tmp52;switch((_tmp4E.C_n).tag){case 4: _LL2F: _LL30:
 goto _LL32;case 1: if((_tmp4E.Rel_n).val == 0){_LL31: _LL32:
# 406
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp4D,0);}else{_LL33: _tmp51=(struct _dyneither_ptr*)((_tmp4E.Rel_n).val)->hd;_tmp52=((_tmp4E.Rel_n).val)->tl;_LL34: {
# 408
struct Cyc_Dict_Dict _tmp53=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp51,_tmp52))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp53,_tmp4D);}}case 3: _LL35: _tmp50=(_tmp4E.C_n).val;_LL36:
 _tmp4F=_tmp50;goto _LL38;default: _LL37: _tmp4F=(_tmp4E.Abs_n).val;_LL38: {
# 412
struct Cyc_Dict_Dict _tmp54=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4F))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp54,_tmp4D);}}_LL2E:;};}
# 417
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 422
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp55=q;union Cyc_Absyn_Nmspace _tmp56;struct _dyneither_ptr*_tmp57;_LL3A: _tmp56=_tmp55->f1;_tmp57=_tmp55->f2;_LL3B:;{
union Cyc_Absyn_Nmspace _tmp58=_tmp56;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct _dyneither_ptr*_tmp5B;struct Cyc_List_List*_tmp5C;switch((_tmp58.C_n).tag){case 1: if((_tmp58.Rel_n).val == 0){_LL3D: _LL3E: {
# 427
struct _handler_cons _tmp5D;_push_handler(& _tmp5D);{int _tmp5F=0;if(setjmp(_tmp5D.handler))_tmp5F=1;if(!_tmp5F){{struct Cyc_Absyn_Datatypedecl***_tmp61=({struct Cyc_Absyn_Datatypedecl***_tmp60=_region_malloc(r,sizeof(*_tmp60));_tmp60[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp57,0);_tmp60;});_npop_handler(0);return _tmp61;};_pop_handler();}else{void*_tmp5E=(void*)_exn_thrown;void*_tmp62=_tmp5E;void*_tmp63;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp62)->tag == Cyc_Dict_Absent){_LL48: _LL49:
 return 0;}else{_LL4A: _tmp63=_tmp62;_LL4B:(int)_rethrow(_tmp63);}_LL47:;}};}}else{_LL41: _tmp5B=(struct _dyneither_ptr*)((_tmp58.Rel_n).val)->hd;_tmp5C=((_tmp58.Rel_n).val)->tl;_LL42: {
# 437
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp5B,_tmp5C);;_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp69=_tmp67;void*_tmp6A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp69)->tag == Cyc_Dict_Absent){_LL4D: _LL4E:
# 441
({void*_tmp6B=0;Cyc_Tcutil_terr(loc,({const char*_tmp6C="bad qualified name for @extensible datatype";_tag_dyneither(_tmp6C,sizeof(char),44);}),_tag_dyneither(_tmp6B,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp6D=_cycalloc_atomic(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Dict_Absent_exn_struct _tmp6E;_tmp6E.tag=Cyc_Dict_Absent;_tmp6E;});_tmp6D;}));}else{_LL4F: _tmp6A=_tmp69;_LL50:(int)_rethrow(_tmp6A);}_LL4C:;}};}{
# 444
struct Cyc_Dict_Dict _tmp6F=ge->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp70=_region_malloc(r,sizeof(*_tmp70));_tmp70[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp6F,_tmp57);_tmp70;});};}}case 4: _LL3F: _LL40:
# 430
({void*_tmp64=0;Cyc_Tcutil_terr(loc,({const char*_tmp65="lookup_xdatatypedecl: impossible";_tag_dyneither(_tmp65,sizeof(char),33);}),_tag_dyneither(_tmp64,sizeof(void*),0));});
return 0;case 3: _LL43: _tmp5A=(_tmp58.C_n).val;_LL44:
# 446
 _tmp59=_tmp5A;goto _LL46;default: _LL45: _tmp59=(_tmp58.Abs_n).val;_LL46: {
# 448
struct Cyc_Dict_Dict _tmp71=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp59))->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp72=_region_malloc(r,sizeof(*_tmp72));_tmp72[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp71,_tmp57);_tmp72;});}}_LL3C:;};}
# 453
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp73=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp73,v);}
# 457
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp74=q;union Cyc_Absyn_Nmspace _tmp75;struct _dyneither_ptr*_tmp76;_LL52: _tmp75=_tmp74->f1;_tmp76=_tmp74->f2;_LL53:;{
union Cyc_Absyn_Nmspace _tmp77=_tmp75;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp79;struct _dyneither_ptr*_tmp7A;struct Cyc_List_List*_tmp7B;switch((_tmp77.C_n).tag){case 4: _LL55: _LL56:
 goto _LL58;case 1: if((_tmp77.Rel_n).val == 0){_LL57: _LL58:
# 462
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp76,0);}else{_LL59: _tmp7A=(struct _dyneither_ptr*)((_tmp77.Rel_n).val)->hd;_tmp7B=((_tmp77.Rel_n).val)->tl;_LL5A: {
# 464
struct Cyc_Dict_Dict _tmp7C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp7A,_tmp7B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp76);}}case 3: _LL5B: _tmp79=(_tmp77.C_n).val;_LL5C:
 _tmp78=_tmp79;goto _LL5E;default: _LL5D: _tmp78=(_tmp77.Abs_n).val;_LL5E: {
# 468
struct Cyc_Dict_Dict _tmp7D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp78))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7D,_tmp76);}}_LL54:;};}
# 473
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp7E=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,v);}
# 477
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp7F=q;union Cyc_Absyn_Nmspace _tmp80;struct _dyneither_ptr*_tmp81;_LL60: _tmp80=_tmp7F->f1;_tmp81=_tmp7F->f2;_LL61:;{
union Cyc_Absyn_Nmspace _tmp82=_tmp80;struct Cyc_List_List*_tmp83;struct Cyc_List_List*_tmp84;struct _dyneither_ptr*_tmp85;struct Cyc_List_List*_tmp86;switch((_tmp82.C_n).tag){case 4: _LL63: _LL64:
 goto _LL66;case 1: if((_tmp82.Rel_n).val == 0){_LL65: _LL66:
# 482
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp81,0);}else{_LL67: _tmp85=(struct _dyneither_ptr*)((_tmp82.Rel_n).val)->hd;_tmp86=((_tmp82.Rel_n).val)->tl;_LL68: {
# 484
struct Cyc_Dict_Dict _tmp87=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp85,_tmp86))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp87,_tmp81);}}case 3: _LL69: _tmp84=(_tmp82.C_n).val;_LL6A:
 _tmp83=_tmp84;goto _LL6C;default: _LL6B: _tmp83=(_tmp82.Abs_n).val;_LL6C: {
# 488
struct Cyc_Dict_Dict _tmp88=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp83))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp88,_tmp81);}}_LL62:;};}
# 495
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp89=te->le;
if(_tmp89 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp89;}
# 501
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 505
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp8A=te->le;
if(_tmp8A != 0){
struct Cyc_Tcenv_Fenv*_tmp8B=_tmp8A;struct Cyc_Tcenv_Fenv*_tmp8C=_tmp8B;struct _RegionHandle*_tmp8D;_LL6E: _tmp8D=_tmp8C->fnrgn;_LL6F:;
return _tmp8D;}
# 511
return Cyc_Tcenv_coerce_heap_region();}
# 514
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp8E="put_fenv";_tag_dyneither(_tmp8E,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp8F=_region_malloc(l,sizeof(*_tmp8F));_tmp8F->ns=te->ns;_tmp8F->ae=te->ae;_tmp8F->le=fe;_tmp8F->allow_valueof=te->allow_valueof;_tmp8F->in_extern_c_include=te->in_extern_c_include;_tmp8F;});}
# 519
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp90=_region_malloc(l,sizeof(*_tmp90));_tmp90->ns=te->ns;_tmp90->ae=te->ae;_tmp90->le=0;_tmp90->allow_valueof=te->allow_valueof;_tmp90->in_extern_c_include=te->in_extern_c_include;_tmp90;});}
# 523
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 525
struct Cyc_Tcenv_Fenv _tmp91=*f;struct Cyc_Tcenv_Fenv _tmp92=_tmp91;struct Cyc_Tcenv_SharedFenv*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_RgnOrder_RgnPO*_tmp95;const struct Cyc_Tcenv_Bindings*_tmp96;struct Cyc_Absyn_Stmt*_tmp97;struct Cyc_Tcenv_CtrlEnv*_tmp98;void*_tmp99;void*_tmp9A;struct Cyc_Tcenv_FenvFlags _tmp9B;struct _RegionHandle*_tmp9C;_LL71: _tmp93=_tmp92.shared;_tmp94=_tmp92.type_vars;_tmp95=_tmp92.region_order;_tmp96=_tmp92.locals;_tmp97=_tmp92.encloser;_tmp98=_tmp92.ctrl_env;_tmp99=_tmp92.capability;_tmp9A=_tmp92.curr_rgn;_tmp9B=_tmp92.flags;_tmp9C=_tmp92.fnrgn;_LL72:;
# 527
return({struct Cyc_Tcenv_Fenv*_tmp9D=_region_malloc(l,sizeof(*_tmp9D));_tmp9D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp93;_tmp9D->type_vars=(struct Cyc_List_List*)_tmp94;_tmp9D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp95;_tmp9D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp96;_tmp9D->encloser=(struct Cyc_Absyn_Stmt*)_tmp97;_tmp9D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp98;_tmp9D->capability=(void*)_tmp99;_tmp9D->curr_rgn=(void*)_tmp9A;_tmp9D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp9B;_tmp9D->fnrgn=(struct _RegionHandle*)l;_tmp9D;});}
# 531
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 533
struct Cyc_Tcenv_Fenv _tmp9E=*f;struct Cyc_Tcenv_Fenv _tmp9F=_tmp9E;struct Cyc_Tcenv_SharedFenv*_tmpA0;struct Cyc_List_List*_tmpA1;struct Cyc_RgnOrder_RgnPO*_tmpA2;const struct Cyc_Tcenv_Bindings*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Tcenv_CtrlEnv*_tmpA5;void*_tmpA6;void*_tmpA7;struct Cyc_Tcenv_FenvFlags _tmpA8;_LL74: _tmpA0=_tmp9F.shared;_tmpA1=_tmp9F.type_vars;_tmpA2=_tmp9F.region_order;_tmpA3=_tmp9F.locals;_tmpA4=_tmp9F.encloser;_tmpA5=_tmp9F.ctrl_env;_tmpA6=_tmp9F.capability;_tmpA7=_tmp9F.curr_rgn;_tmpA8=_tmp9F.flags;_LL75:;{
# 536
struct Cyc_Tcenv_CtrlEnv _tmpA9=*_tmpA5;struct Cyc_Tcenv_CtrlEnv _tmpAA=_tmpA9;const struct _tuple10*_tmpAB;_LL77: _tmpAB=_tmpAA.fallthru_clause;_LL78:;{
struct Cyc_Tcenv_CtrlEnv*_tmpAC=({struct Cyc_Tcenv_CtrlEnv*_tmpAE=_region_malloc(l,sizeof(*_tmpAE));_tmpAE->fallthru_clause=_tmpAB;_tmpAE;});
# 539
return({struct Cyc_Tcenv_Fenv*_tmpAD=_region_malloc(l,sizeof(*_tmpAD));_tmpAD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA0;_tmpAD->type_vars=(struct Cyc_List_List*)_tmpA1;_tmpAD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA2;_tmpAD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA3;_tmpAD->encloser=(struct Cyc_Absyn_Stmt*)_tmpA4;_tmpAD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpAC;_tmpAD->capability=(void*)_tmpA6;_tmpAD->curr_rgn=(void*)_tmpA7;_tmpAD->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA8;_tmpAD->fnrgn=(struct _RegionHandle*)l;_tmpAD;});};};}
# 545
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpAF=Cyc_Tcenv_get_fenv(te,({const char*_tmpB5="return_typ";_tag_dyneither(_tmpB5,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmpB0=_tmpAF;struct Cyc_Tcenv_SharedFenv*_tmpB1;_LL7A: _tmpB1=_tmpB0->shared;_LL7B:;{
struct Cyc_Tcenv_SharedFenv _tmpB2=*_tmpB1;struct Cyc_Tcenv_SharedFenv _tmpB3=_tmpB2;void*_tmpB4;_LL7D: _tmpB4=_tmpB3.return_typ;_LL7E:;
return _tmpB4;};}
# 551
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpB6=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpB7=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpB6));struct Cyc_Tcenv_Fenv _tmpB8=_tmpB7;struct Cyc_List_List*_tmpB9;_LL80: _tmpB9=_tmpB8.type_vars;_LL81:;
return _tmpB9;};}
# 558
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->v=Cyc_Tcenv_lookup_type_vars(te);_tmpBA;});}
# 562
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 564
struct Cyc_Tcenv_Fenv*_tmpBB=
# 566
Cyc_Tcenv_get_fenv(te,({const char*_tmpC9="add_type_vars";_tag_dyneither(_tmpC9,sizeof(char),14);}));
# 564
struct Cyc_Tcenv_Fenv*_tmpBC=_tmpBB;struct Cyc_Tcenv_SharedFenv*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_RgnOrder_RgnPO*_tmpBF;const struct Cyc_Tcenv_Bindings*_tmpC0;struct Cyc_Absyn_Stmt*_tmpC1;struct Cyc_Tcenv_CtrlEnv*_tmpC2;void*_tmpC3;void*_tmpC4;struct Cyc_Tcenv_FenvFlags _tmpC5;_LL83: _tmpBD=_tmpBC->shared;_tmpBE=_tmpBC->type_vars;_tmpBF=_tmpBC->region_order;_tmpC0=_tmpBC->locals;_tmpC1=_tmpBC->encloser;_tmpC2=_tmpBC->ctrl_env;_tmpC3=_tmpBC->capability;_tmpC4=_tmpBC->curr_rgn;_tmpC5=_tmpBC->flags;_LL84:;
# 568
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpC6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpBE);
Cyc_Tcutil_check_unique_tvars(loc,_tmpC6);{
struct Cyc_Tcenv_Fenv*_tmpC7=({struct Cyc_Tcenv_Fenv*_tmpC8=_region_malloc(r,sizeof(*_tmpC8));_tmpC8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpBD;_tmpC8->type_vars=(struct Cyc_List_List*)_tmpC6;_tmpC8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpBF;_tmpC8->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC0;_tmpC8->encloser=(struct Cyc_Absyn_Stmt*)_tmpC1;_tmpC8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpC2;_tmpC8->capability=(void*)_tmpC3;_tmpC8->curr_rgn=(void*)_tmpC4;_tmpC8->flags=(struct Cyc_Tcenv_FenvFlags)_tmpC5;_tmpC8->fnrgn=(struct _RegionHandle*)r;_tmpC8;});
# 573
return Cyc_Tcenv_put_fenv(r,te,_tmpC7);};};}
# 576
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpCA=te->le;
if(_tmpCA == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 581
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpCA);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 586
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 588
struct _dyneither_ptr*_tmpCB=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpCC=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpCD=_tmpCC;if((_tmpCD.Loc_n).tag == 4){_LL86: _LL87:
 goto _LL85;}else{_LL88: _LL89:
# 593
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Core_Impossible_exn_struct _tmpCF;_tmpCF.tag=Cyc_Core_Impossible;_tmpCF.f1=({const char*_tmpD0="add_local_var: called with Rel_n";_tag_dyneither(_tmpD0,sizeof(char),33);});_tmpCF;});_tmpCE;}));}_LL85:;}
# 595
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpD1;_push_handler(& _tmpD1);{int _tmpD3=0;if(setjmp(_tmpD1.handler))_tmpD3=1;if(!_tmpD3){
{void*_tmpD4=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpD5=_tmpD4;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpD5)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpD5)->f1)){case 4: _LL8B: _LL8C:
# 599
({void*_tmpD6=0;Cyc_Tcutil_warn(loc,({const char*_tmpD7="declaration hides local";_tag_dyneither(_tmpD7,sizeof(char),24);}),_tag_dyneither(_tmpD6,sizeof(void*),0));});goto _LL8A;case 1: _LL8D: _LL8E:
({void*_tmpD8=0;Cyc_Tcutil_warn(loc,({const char*_tmpD9="declaration hides global";_tag_dyneither(_tmpD9,sizeof(char),25);}),_tag_dyneither(_tmpD8,sizeof(void*),0));});goto _LL8A;case 5: _LL8F: _LL90:
({void*_tmpDA=0;Cyc_Tcutil_warn(loc,({const char*_tmpDB="declaration hides pattern variable";_tag_dyneither(_tmpDB,sizeof(char),35);}),_tag_dyneither(_tmpDA,sizeof(void*),0));});goto _LL8A;case 3: _LL91: _LL92:
({void*_tmpDC=0;Cyc_Tcutil_warn(loc,({const char*_tmpDD="declaration hides parameter";_tag_dyneither(_tmpDD,sizeof(char),28);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});goto _LL8A;default: goto _LL93;}else{_LL93: _LL94:
 goto _LL8A;}_LL8A:;}
# 597
;_pop_handler();}else{void*_tmpD2=(void*)_exn_thrown;void*_tmpDE=_tmpD2;void*_tmpDF;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpDE)->tag == Cyc_Dict_Absent){_LL96: _LL97:
# 605
 goto _LL95;}else{_LL98: _tmpDF=_tmpDE;_LL99:(int)_rethrow(_tmpDF);}_LL95:;}};}{
struct Cyc_Tcenv_Fenv*_tmpE0=
# 608
Cyc_Tcenv_get_fenv(te,({const char*_tmpF2="add_local_var";_tag_dyneither(_tmpF2,sizeof(char),14);}));
# 606
struct Cyc_Tcenv_Fenv*_tmpE1=_tmpE0;struct Cyc_Tcenv_SharedFenv*_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_RgnOrder_RgnPO*_tmpE4;const struct Cyc_Tcenv_Bindings*_tmpE5;struct Cyc_Absyn_Stmt*_tmpE6;struct Cyc_Tcenv_CtrlEnv*_tmpE7;void*_tmpE8;void*_tmpE9;struct Cyc_Tcenv_FenvFlags _tmpEA;_LL9B: _tmpE2=_tmpE1->shared;_tmpE3=_tmpE1->type_vars;_tmpE4=_tmpE1->region_order;_tmpE5=_tmpE1->locals;_tmpE6=_tmpE1->encloser;_tmpE7=_tmpE1->ctrl_env;_tmpE8=_tmpE1->capability;_tmpE9=_tmpE1->curr_rgn;_tmpEA=_tmpE1->flags;_LL9C:;{
# 609
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpEB=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpF1;_tmpF1.tag=4;_tmpF1.f1=vd;_tmpF1;});_tmpF0;});
struct Cyc_Tcenv_Bindings*_tmpEC=({struct Cyc_Tcenv_Bindings*_tmpEF=_region_malloc(r,sizeof(*_tmpEF));_tmpEF->v=_tmpCB;_tmpEF->b=(void*)_tmpEB;_tmpEF->tl=_tmpE5;_tmpEF;});
struct Cyc_Tcenv_Fenv*_tmpED=({struct Cyc_Tcenv_Fenv*_tmpEE=_region_malloc(r,sizeof(*_tmpEE));_tmpEE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpE2;_tmpEE->type_vars=(struct Cyc_List_List*)_tmpE3;_tmpEE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE4;_tmpEE->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpEC);_tmpEE->encloser=(struct Cyc_Absyn_Stmt*)_tmpE6;_tmpEE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE7;_tmpEE->capability=(void*)_tmpE8;_tmpEE->curr_rgn=(void*)_tmpE9;_tmpEE->flags=(struct Cyc_Tcenv_FenvFlags)_tmpEA;_tmpEE->fnrgn=(struct _RegionHandle*)r;_tmpEE;});
# 613
return Cyc_Tcenv_put_fenv(r,te,_tmpED);};};}
# 616
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 618
struct Cyc_Tcenv_Fenv*_tmpF3=te->le;
if(_tmpF3 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpF4=*_tmpF3;struct Cyc_Tcenv_Fenv _tmpF5=_tmpF4;struct Cyc_Tcenv_SharedFenv*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_RgnOrder_RgnPO*_tmpF8;const struct Cyc_Tcenv_Bindings*_tmpF9;struct Cyc_Absyn_Stmt*_tmpFA;struct Cyc_Tcenv_CtrlEnv*_tmpFB;void*_tmpFC;void*_tmpFD;int _tmpFE;int _tmpFF;int _tmp100;_LL9E: _tmpF6=_tmpF5.shared;_tmpF7=_tmpF5.type_vars;_tmpF8=_tmpF5.region_order;_tmpF9=_tmpF5.locals;_tmpFA=_tmpF5.encloser;_tmpFB=_tmpF5.ctrl_env;_tmpFC=_tmpF5.capability;_tmpFD=_tmpF5.curr_rgn;_tmpFE=(_tmpF5.flags).in_notreadctxt;_tmpFF=(_tmpF5.flags).in_lhs;_tmp100=(_tmpF5.flags).abstract_ok;_LL9F:;{
# 623
struct Cyc_Tcenv_Fenv*_tmp101=({struct Cyc_Tcenv_Fenv*_tmp102=_region_malloc(r,sizeof(*_tmp102));_tmp102->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF6;_tmp102->type_vars=(struct Cyc_List_List*)_tmpF7;_tmp102->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF8;_tmp102->locals=(const struct Cyc_Tcenv_Bindings*)_tmpF9;_tmp102->encloser=(struct Cyc_Absyn_Stmt*)_tmpFA;_tmp102->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpFB;_tmp102->capability=(void*)_tmpFC;_tmp102->curr_rgn=(void*)_tmpFD;_tmp102->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp102->flags).in_notreadctxt=_tmpFE;(_tmp102->flags).in_lhs=_tmpFF;(_tmp102->flags).in_new=(int)status;(_tmp102->flags).abstract_ok=_tmp100;_tmp102->flags;});_tmp102->fnrgn=(struct _RegionHandle*)r;_tmp102;});
# 626
return Cyc_Tcenv_put_fenv(r,te,_tmp101);};};}
# 629
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp103=te->le;
if(_tmp103 == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp104=_tmp103;struct Cyc_Tcenv_Fenv*_tmp105=_tmp104;int _tmp106;_LLA1: _tmp106=(_tmp105->flags).in_new;_LLA2:;
return(enum Cyc_Tcenv_NewStatus)_tmp106;};}
# 636
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp107=te->le;
if(_tmp107 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp108=_tmp107;struct Cyc_Tcenv_Fenv*_tmp109=_tmp108;int _tmp10A;_LLA4: _tmp10A=(_tmp109->flags).abstract_ok;_LLA5:;
return _tmp10A;};}
# 643
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 645
struct Cyc_Tcenv_Fenv*_tmp10B=te->le;
if(_tmp10B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp10C=*_tmp10B;struct Cyc_Tcenv_Fenv _tmp10D=_tmp10C;struct Cyc_Tcenv_SharedFenv*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_RgnOrder_RgnPO*_tmp110;const struct Cyc_Tcenv_Bindings*_tmp111;struct Cyc_Absyn_Stmt*_tmp112;struct Cyc_Tcenv_CtrlEnv*_tmp113;void*_tmp114;void*_tmp115;int _tmp116;int _tmp117;int _tmp118;_LLA7: _tmp10E=_tmp10D.shared;_tmp10F=_tmp10D.type_vars;_tmp110=_tmp10D.region_order;_tmp111=_tmp10D.locals;_tmp112=_tmp10D.encloser;_tmp113=_tmp10D.ctrl_env;_tmp114=_tmp10D.capability;_tmp115=_tmp10D.curr_rgn;_tmp116=(_tmp10D.flags).in_notreadctxt;_tmp117=(_tmp10D.flags).in_lhs;_tmp118=(_tmp10D.flags).in_new;_LLA8:;{
# 650
struct Cyc_Tcenv_Fenv*_tmp119=({struct Cyc_Tcenv_Fenv*_tmp11A=_region_malloc(r,sizeof(*_tmp11A));_tmp11A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp10E;_tmp11A->type_vars=(struct Cyc_List_List*)_tmp10F;_tmp11A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp110;_tmp11A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp111;_tmp11A->encloser=(struct Cyc_Absyn_Stmt*)_tmp112;_tmp11A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp113;_tmp11A->capability=(void*)_tmp114;_tmp11A->curr_rgn=(void*)_tmp115;_tmp11A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp11A->flags).in_notreadctxt=_tmp116;(_tmp11A->flags).in_lhs=_tmp117;(_tmp11A->flags).in_new=_tmp118;(_tmp11A->flags).abstract_ok=1;_tmp11A->flags;});_tmp11A->fnrgn=(struct _RegionHandle*)r;_tmp11A;});
# 653
return Cyc_Tcenv_put_fenv(r,te,_tmp119);};};}
# 656
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 658
struct Cyc_Tcenv_Fenv*_tmp11B=te->le;
if(_tmp11B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp11C=*_tmp11B;struct Cyc_Tcenv_Fenv _tmp11D=_tmp11C;struct Cyc_Tcenv_SharedFenv*_tmp11E;struct Cyc_List_List*_tmp11F;struct Cyc_RgnOrder_RgnPO*_tmp120;const struct Cyc_Tcenv_Bindings*_tmp121;struct Cyc_Absyn_Stmt*_tmp122;struct Cyc_Tcenv_CtrlEnv*_tmp123;void*_tmp124;void*_tmp125;int _tmp126;int _tmp127;int _tmp128;_LLAA: _tmp11E=_tmp11D.shared;_tmp11F=_tmp11D.type_vars;_tmp120=_tmp11D.region_order;_tmp121=_tmp11D.locals;_tmp122=_tmp11D.encloser;_tmp123=_tmp11D.ctrl_env;_tmp124=_tmp11D.capability;_tmp125=_tmp11D.curr_rgn;_tmp126=(_tmp11D.flags).in_notreadctxt;_tmp127=(_tmp11D.flags).in_lhs;_tmp128=(_tmp11D.flags).in_new;_LLAB:;{
# 663
struct Cyc_Tcenv_Fenv*_tmp129=({struct Cyc_Tcenv_Fenv*_tmp12A=_region_malloc(r,sizeof(*_tmp12A));_tmp12A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp11E;_tmp12A->type_vars=(struct Cyc_List_List*)_tmp11F;_tmp12A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp120;_tmp12A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp121;_tmp12A->encloser=(struct Cyc_Absyn_Stmt*)_tmp122;_tmp12A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp123;_tmp12A->capability=(void*)_tmp124;_tmp12A->curr_rgn=(void*)_tmp125;_tmp12A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp12A->flags).in_notreadctxt=_tmp126;(_tmp12A->flags).in_lhs=_tmp127;(_tmp12A->flags).in_new=_tmp128;(_tmp12A->flags).abstract_ok=0;_tmp12A->flags;});_tmp12A->fnrgn=(struct _RegionHandle*)r;_tmp12A;});
# 666
return Cyc_Tcenv_put_fenv(r,te,_tmp129);};};}
# 669
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp12B=te->le;
if(_tmp12B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp12C=*_tmp12B;struct Cyc_Tcenv_Fenv _tmp12D=_tmp12C;struct Cyc_Tcenv_SharedFenv*_tmp12E;struct Cyc_List_List*_tmp12F;struct Cyc_RgnOrder_RgnPO*_tmp130;const struct Cyc_Tcenv_Bindings*_tmp131;struct Cyc_Absyn_Stmt*_tmp132;struct Cyc_Tcenv_CtrlEnv*_tmp133;void*_tmp134;void*_tmp135;int _tmp136;int _tmp137;int _tmp138;_LLAD: _tmp12E=_tmp12D.shared;_tmp12F=_tmp12D.type_vars;_tmp130=_tmp12D.region_order;_tmp131=_tmp12D.locals;_tmp132=_tmp12D.encloser;_tmp133=_tmp12D.ctrl_env;_tmp134=_tmp12D.capability;_tmp135=_tmp12D.curr_rgn;_tmp136=(_tmp12D.flags).in_lhs;_tmp137=(_tmp12D.flags).in_new;_tmp138=(_tmp12D.flags).abstract_ok;_LLAE:;{
# 675
struct Cyc_Tcenv_Fenv*_tmp139=({struct Cyc_Tcenv_Fenv*_tmp13A=_region_malloc(r,sizeof(*_tmp13A));_tmp13A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12E;_tmp13A->type_vars=(struct Cyc_List_List*)_tmp12F;_tmp13A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp130;_tmp13A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp131;_tmp13A->encloser=(struct Cyc_Absyn_Stmt*)_tmp132;_tmp13A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp133;_tmp13A->capability=(void*)_tmp134;_tmp13A->curr_rgn=(void*)_tmp135;_tmp13A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp13A->flags).in_notreadctxt=1;(_tmp13A->flags).in_lhs=_tmp136;(_tmp13A->flags).in_new=_tmp137;(_tmp13A->flags).abstract_ok=_tmp138;_tmp13A->flags;});_tmp13A->fnrgn=(struct _RegionHandle*)r;_tmp13A;});
# 678
return Cyc_Tcenv_put_fenv(r,te,_tmp139);};};}
# 681
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp13B=te->le;
if(_tmp13B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp13C=*_tmp13B;struct Cyc_Tcenv_Fenv _tmp13D=_tmp13C;struct Cyc_Tcenv_SharedFenv*_tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_RgnOrder_RgnPO*_tmp140;const struct Cyc_Tcenv_Bindings*_tmp141;struct Cyc_Absyn_Stmt*_tmp142;struct Cyc_Tcenv_CtrlEnv*_tmp143;void*_tmp144;void*_tmp145;int _tmp146;int _tmp147;int _tmp148;_LLB0: _tmp13E=_tmp13D.shared;_tmp13F=_tmp13D.type_vars;_tmp140=_tmp13D.region_order;_tmp141=_tmp13D.locals;_tmp142=_tmp13D.encloser;_tmp143=_tmp13D.ctrl_env;_tmp144=_tmp13D.capability;_tmp145=_tmp13D.curr_rgn;_tmp146=(_tmp13D.flags).in_lhs;_tmp147=(_tmp13D.flags).in_new;_tmp148=(_tmp13D.flags).abstract_ok;_LLB1:;{
# 687
struct Cyc_Tcenv_Fenv*_tmp149=({struct Cyc_Tcenv_Fenv*_tmp14A=_region_malloc(r,sizeof(*_tmp14A));_tmp14A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp13E;_tmp14A->type_vars=(struct Cyc_List_List*)_tmp13F;_tmp14A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp140;_tmp14A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp141;_tmp14A->encloser=(struct Cyc_Absyn_Stmt*)_tmp142;_tmp14A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp143;_tmp14A->capability=(void*)_tmp144;_tmp14A->curr_rgn=(void*)_tmp145;_tmp14A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp14A->flags).in_notreadctxt=0;(_tmp14A->flags).in_lhs=_tmp146;(_tmp14A->flags).in_new=_tmp147;(_tmp14A->flags).abstract_ok=_tmp148;_tmp14A->flags;});_tmp14A->fnrgn=(struct _RegionHandle*)r;_tmp14A;});
# 690
return Cyc_Tcenv_put_fenv(r,te,_tmp149);};};}
# 693
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp14B=te->le;
if(_tmp14B == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp14C=_tmp14B;struct Cyc_Tcenv_Fenv*_tmp14D=_tmp14C;int _tmp14E;_LLB3: _tmp14E=(_tmp14D->flags).in_notreadctxt;_LLB4:;
return _tmp14E;};}
# 700
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp14F=te->le;
if(_tmp14F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp150=*_tmp14F;struct Cyc_Tcenv_Fenv _tmp151=_tmp150;struct Cyc_Tcenv_SharedFenv*_tmp152;struct Cyc_List_List*_tmp153;struct Cyc_RgnOrder_RgnPO*_tmp154;const struct Cyc_Tcenv_Bindings*_tmp155;struct Cyc_Absyn_Stmt*_tmp156;struct Cyc_Tcenv_CtrlEnv*_tmp157;void*_tmp158;void*_tmp159;int _tmp15A;int _tmp15B;int _tmp15C;_LLB6: _tmp152=_tmp151.shared;_tmp153=_tmp151.type_vars;_tmp154=_tmp151.region_order;_tmp155=_tmp151.locals;_tmp156=_tmp151.encloser;_tmp157=_tmp151.ctrl_env;_tmp158=_tmp151.capability;_tmp159=_tmp151.curr_rgn;_tmp15A=(_tmp151.flags).in_notreadctxt;_tmp15B=(_tmp151.flags).in_new;_tmp15C=(_tmp151.flags).abstract_ok;_LLB7:;{
# 706
struct Cyc_Tcenv_Fenv*_tmp15D=({struct Cyc_Tcenv_Fenv*_tmp15E=_region_malloc(r,sizeof(*_tmp15E));_tmp15E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp152;_tmp15E->type_vars=(struct Cyc_List_List*)_tmp153;_tmp15E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp154;_tmp15E->locals=(const struct Cyc_Tcenv_Bindings*)_tmp155;_tmp15E->encloser=(struct Cyc_Absyn_Stmt*)_tmp156;_tmp15E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp157;_tmp15E->capability=(void*)_tmp158;_tmp15E->curr_rgn=(void*)_tmp159;_tmp15E->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp15E->flags).in_notreadctxt=_tmp15A;(_tmp15E->flags).in_lhs=1;(_tmp15E->flags).in_new=_tmp15B;(_tmp15E->flags).abstract_ok=_tmp15C;_tmp15E->flags;});_tmp15E->fnrgn=(struct _RegionHandle*)r;_tmp15E;});
# 709
return Cyc_Tcenv_put_fenv(r,te,_tmp15D);};};}
# 712
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp15F=te->le;
if(_tmp15F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp160=*_tmp15F;struct Cyc_Tcenv_Fenv _tmp161=_tmp160;struct Cyc_Tcenv_SharedFenv*_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_RgnOrder_RgnPO*_tmp164;const struct Cyc_Tcenv_Bindings*_tmp165;struct Cyc_Absyn_Stmt*_tmp166;struct Cyc_Tcenv_CtrlEnv*_tmp167;void*_tmp168;void*_tmp169;int _tmp16A;int _tmp16B;int _tmp16C;_LLB9: _tmp162=_tmp161.shared;_tmp163=_tmp161.type_vars;_tmp164=_tmp161.region_order;_tmp165=_tmp161.locals;_tmp166=_tmp161.encloser;_tmp167=_tmp161.ctrl_env;_tmp168=_tmp161.capability;_tmp169=_tmp161.curr_rgn;_tmp16A=(_tmp161.flags).in_notreadctxt;_tmp16B=(_tmp161.flags).in_new;_tmp16C=(_tmp161.flags).abstract_ok;_LLBA:;{
# 718
struct Cyc_Tcenv_Fenv*_tmp16D=({struct Cyc_Tcenv_Fenv*_tmp16E=_region_malloc(r,sizeof(*_tmp16E));_tmp16E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp162;_tmp16E->type_vars=(struct Cyc_List_List*)_tmp163;_tmp16E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp164;_tmp16E->locals=(const struct Cyc_Tcenv_Bindings*)_tmp165;_tmp16E->encloser=(struct Cyc_Absyn_Stmt*)_tmp166;_tmp16E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp167;_tmp16E->capability=(void*)_tmp168;_tmp16E->curr_rgn=(void*)_tmp169;_tmp16E->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp16E->flags).in_notreadctxt=_tmp16A;(_tmp16E->flags).in_lhs=0;(_tmp16E->flags).in_new=_tmp16B;(_tmp16E->flags).abstract_ok=_tmp16C;_tmp16E->flags;});_tmp16E->fnrgn=(struct _RegionHandle*)r;_tmp16E;});
# 721
return Cyc_Tcenv_put_fenv(r,te,_tmp16D);};};}
# 724
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16F=te->le;
if(_tmp16F == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp170=_tmp16F;struct Cyc_Tcenv_Fenv*_tmp171=_tmp170;int _tmp172;_LLBC: _tmp172=(_tmp171->flags).in_lhs;_LLBD:;
return _tmp172;};}
# 732
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 734
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
{void*_tmp176=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp177=_tmp176;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp177)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp177)->f1)){case 4: _LLBF: _LLC0:
# 738
({void*_tmp178=0;Cyc_Tcutil_warn(loc,({const char*_tmp179="pattern variable hides local";_tag_dyneither(_tmp179,sizeof(char),29);}),_tag_dyneither(_tmp178,sizeof(void*),0));});goto _LLBE;case 1: _LLC1: _LLC2:
({void*_tmp17A=0;Cyc_Tcutil_warn(loc,({const char*_tmp17B="pattern variable hides global";_tag_dyneither(_tmp17B,sizeof(char),30);}),_tag_dyneither(_tmp17A,sizeof(void*),0));});goto _LLBE;case 5: _LLC3: _LLC4:
({void*_tmp17C=0;Cyc_Tcutil_warn(loc,({const char*_tmp17D="pattern variable hides pattern variable";_tag_dyneither(_tmp17D,sizeof(char),40);}),_tag_dyneither(_tmp17C,sizeof(void*),0));});goto _LLBE;case 3: _LLC5: _LLC6:
({void*_tmp17E=0;Cyc_Tcutil_warn(loc,({const char*_tmp17F="pattern variable hides parameter";_tag_dyneither(_tmp17F,sizeof(char),33);}),_tag_dyneither(_tmp17E,sizeof(void*),0));});goto _LLBE;default: goto _LLC7;}else{_LLC7: _LLC8:
 goto _LLBE;}_LLBE:;}
# 736
;_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp180=_tmp174;void*_tmp181;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp180)->tag == Cyc_Dict_Absent){_LLCA: _LLCB:
# 744
 goto _LLC9;}else{_LLCC: _tmp181=_tmp180;_LLCD:(int)_rethrow(_tmp181);}_LLC9:;}};}{
struct _dyneither_ptr*_tmp182=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmp183=
# 748
Cyc_Tcenv_get_fenv(te,({const char*_tmp195="add_pat_var";_tag_dyneither(_tmp195,sizeof(char),12);}));
# 746
struct Cyc_Tcenv_Fenv*_tmp184=_tmp183;struct Cyc_Tcenv_SharedFenv*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_RgnOrder_RgnPO*_tmp187;const struct Cyc_Tcenv_Bindings*_tmp188;struct Cyc_Absyn_Stmt*_tmp189;struct Cyc_Tcenv_CtrlEnv*_tmp18A;void*_tmp18B;void*_tmp18C;struct Cyc_Tcenv_FenvFlags _tmp18D;_LLCF: _tmp185=_tmp184->shared;_tmp186=_tmp184->type_vars;_tmp187=_tmp184->region_order;_tmp188=_tmp184->locals;_tmp189=_tmp184->encloser;_tmp18A=_tmp184->ctrl_env;_tmp18B=_tmp184->capability;_tmp18C=_tmp184->curr_rgn;_tmp18D=_tmp184->flags;_LLD0:;{
# 749
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp18E=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp194;_tmp194.tag=5;_tmp194.f1=vd;_tmp194;});_tmp193;});
struct Cyc_Tcenv_Bindings*_tmp18F=({struct Cyc_Tcenv_Bindings*_tmp192=_region_malloc(r,sizeof(*_tmp192));_tmp192->v=_tmp182;_tmp192->b=(void*)_tmp18E;_tmp192->tl=_tmp188;_tmp192;});
struct Cyc_Tcenv_Fenv*_tmp190=({struct Cyc_Tcenv_Fenv*_tmp191=_region_malloc(r,sizeof(*_tmp191));_tmp191->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp185;_tmp191->type_vars=(struct Cyc_List_List*)_tmp186;_tmp191->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp187;_tmp191->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp18F);_tmp191->encloser=(struct Cyc_Absyn_Stmt*)_tmp189;_tmp191->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp18A;_tmp191->capability=(void*)_tmp18B;_tmp191->curr_rgn=(void*)_tmp18C;_tmp191->flags=(struct Cyc_Tcenv_FenvFlags)_tmp18D;_tmp191->fnrgn=(struct _RegionHandle*)r;_tmp191;});
# 753
return Cyc_Tcenv_put_fenv(r,te,_tmp190);};};}
# 756
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp196=te->le;
struct _tuple0*_tmp197=q;union Cyc_Absyn_Nmspace _tmp198;struct _dyneither_ptr*_tmp199;_LLD2: _tmp198=_tmp197->f1;_tmp199=_tmp197->f2;_LLD3:;{
union Cyc_Absyn_Nmspace _tmp19A=_tmp198;switch((_tmp19A.Rel_n).tag){case 4: _LLD5: _LLD6:
# 761
 if(_tmp196 == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp19B=_cycalloc_atomic(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Dict_Absent_exn_struct _tmp19C;_tmp19C.tag=Cyc_Dict_Absent;_tmp19C;});_tmp19B;}));
goto _LLD8;case 1: if((_tmp19A.Rel_n).val == 0){_LLD7: if(_tmp196 != 0){_LLD8: {
# 764
struct Cyc_Tcenv_Fenv*_tmp19D=_tmp196;struct Cyc_Tcenv_Fenv*_tmp19E=_tmp19D;const struct Cyc_Tcenv_Bindings*_tmp19F;_LLDC: _tmp19F=_tmp19E->locals;_LLDD:;{
# 766
struct _handler_cons _tmp1A0;_push_handler(& _tmp1A0);{int _tmp1A2=0;if(setjmp(_tmp1A0.handler))_tmp1A2=1;if(!_tmp1A2){
{void*_tmp1A5=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp1A3=_region_malloc(r,sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1A4;_tmp1A4.tag=0;_tmp1A4.f1=Cyc_Tcenv_lookup_binding(_tmp19F,_tmp199);_tmp1A4;});_tmp1A3;});_npop_handler(0);return _tmp1A5;};_pop_handler();}else{void*_tmp1A1=(void*)_exn_thrown;void*_tmp1A6=_tmp1A1;void*_tmp1A7;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1A6)->tag == Cyc_Tcenv_NoBinding){_LLDF: _LLE0:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LLE1: _tmp1A7=_tmp1A6;_LLE2:(int)_rethrow(_tmp1A7);}_LLDE:;}};};}}else{goto _LLD9;}}else{goto _LLD9;}default: _LLD9: _LLDA: {
# 770
struct _handler_cons _tmp1A8;_push_handler(& _tmp1A8);{int _tmp1AA=0;if(setjmp(_tmp1A8.handler))_tmp1AA=1;if(!_tmp1AA){{void*_tmp1AB=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1AB;};_pop_handler();}else{void*_tmp1A9=(void*)_exn_thrown;void*_tmp1AC=_tmp1A9;void*_tmp1AD;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1AC)->tag == Cyc_Dict_Absent){_LLE4: _LLE5:
# 774
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp1AE=_region_malloc(r,sizeof(*_tmp1AE));_tmp1AE[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1AF;_tmp1AF.tag=0;_tmp1AF.f1=(void*)& Cyc_Absyn_Unresolved_b_val;_tmp1AF;});_tmp1AE;});}else{_LLE6: _tmp1AD=_tmp1AC;_LLE7:(int)_rethrow(_tmp1AD);}_LLE3:;}};}}_LLD4:;};}
# 779
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l){
struct Cyc_Tcenv_Fenv*_tmp1B0=Cyc_Tcenv_get_fenv(te,({const char*_tmp1B6="process_goto";_tag_dyneither(_tmp1B6,sizeof(char),13);}));struct Cyc_Tcenv_Fenv*_tmp1B1=_tmp1B0;struct Cyc_Tcenv_SharedFenv*_tmp1B2;_LLE9: _tmp1B2=_tmp1B1->shared;_LLEA:;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1B2->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1B3=_tmp1B2->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1B3,l);
struct _RegionHandle*frgn=_tmp1B2->frgn;
if(slopt == 0)
slopt=({struct Cyc_List_List**_tmp1B4=_region_malloc(frgn,sizeof(*_tmp1B4));_tmp1B4[0]=0;_tmp1B4;});{
struct Cyc_List_List*new_needed=({struct Cyc_List_List*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5->hd=s;_tmp1B5->tl=*slopt;_tmp1B5;});
_tmp1B2->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1B3,l,new_needed);};}};}
# 794
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 796
struct Cyc_Tcenv_Fenv*_tmp1B7=Cyc_Tcenv_get_fenv(te,({const char*_tmp1BC="process_fallthru";_tag_dyneither(_tmp1BC,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp1B8=_tmp1B7;struct Cyc_Tcenv_CtrlEnv*_tmp1B9;_LLEC: _tmp1B9=_tmp1B8->ctrl_env;_LLED:;{
const struct _tuple10*_tmp1BA=_tmp1B9->fallthru_clause;
if(_tmp1BA != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB[0]=(*_tmp1BA).f1;_tmp1BB;});
# 801
return _tmp1BA;};}
# 804
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 808
struct Cyc_Tcenv_Fenv*_tmp1BD=
# 810
Cyc_Tcenv_get_fenv(te,({const char*_tmp1D1="set_fallthru";_tag_dyneither(_tmp1D1,sizeof(char),13);}));
# 808
struct Cyc_Tcenv_Fenv*_tmp1BE=_tmp1BD;struct Cyc_Tcenv_SharedFenv*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_RgnOrder_RgnPO*_tmp1C1;const struct Cyc_Tcenv_Bindings*_tmp1C2;struct Cyc_Absyn_Stmt*_tmp1C3;struct Cyc_Tcenv_CtrlEnv*_tmp1C4;void*_tmp1C5;void*_tmp1C6;struct Cyc_Tcenv_FenvFlags _tmp1C7;_LLEF: _tmp1BF=_tmp1BE->shared;_tmp1C0=_tmp1BE->type_vars;_tmp1C1=_tmp1BE->region_order;_tmp1C2=_tmp1BE->locals;_tmp1C3=_tmp1BE->encloser;_tmp1C4=_tmp1BE->ctrl_env;_tmp1C5=_tmp1BE->capability;_tmp1C6=_tmp1BE->curr_rgn;_tmp1C7=_tmp1BE->flags;_LLF0:;{
# 811
struct Cyc_Tcenv_CtrlEnv*_tmp1C8=_tmp1C4;const struct _tuple10*_tmp1C9;_LLF2: _tmp1C9=_tmp1C8->fallthru_clause;_LLF3:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp1CA->tl=ft_typ;_tmp1CA;});}{
const struct Cyc_Tcenv_CList*_tmp1CB=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp1CC=({struct Cyc_Tcenv_CtrlEnv*_tmp1CF=_region_malloc(r,sizeof(*_tmp1CF));_tmp1CF->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->f1=clause;_tmp1D0->f2=new_tvs;_tmp1D0->f3=_tmp1CB;_tmp1D0;});_tmp1CF;});
# 818
struct Cyc_Tcenv_Fenv*_tmp1CD=({struct Cyc_Tcenv_Fenv*_tmp1CE=_region_malloc(r,sizeof(*_tmp1CE));_tmp1CE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1BF;_tmp1CE->type_vars=(struct Cyc_List_List*)_tmp1C0;_tmp1CE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C1;_tmp1CE->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1C2;_tmp1CE->encloser=(struct Cyc_Absyn_Stmt*)_tmp1C3;_tmp1CE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1CC;_tmp1CE->capability=(void*)_tmp1C5;_tmp1CE->curr_rgn=(void*)_tmp1C6;_tmp1CE->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1C7;_tmp1CE->fnrgn=(struct _RegionHandle*)r;_tmp1CE;});
# 822
return Cyc_Tcenv_put_fenv(r,te,_tmp1CD);};};};}
# 825
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1D4="clear_fallthru";_tag_dyneither(_tmp1D4,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp1D2=fe;struct Cyc_Tcenv_CtrlEnv*_tmp1D3;_LLF5: _tmp1D3=_tmp1D2->ctrl_env;_LLF6:;
_tmp1D3->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 832
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 834
struct Cyc_Tcenv_Fenv*_tmp1D5=
# 836
Cyc_Tcenv_get_fenv(te,({const char*_tmp1E2="set_in_loop";_tag_dyneither(_tmp1E2,sizeof(char),12);}));
# 834
struct Cyc_Tcenv_Fenv*_tmp1D6=_tmp1D5;struct Cyc_Tcenv_SharedFenv*_tmp1D7;struct Cyc_List_List*_tmp1D8;struct Cyc_RgnOrder_RgnPO*_tmp1D9;const struct Cyc_Tcenv_Bindings*_tmp1DA;struct Cyc_Absyn_Stmt*_tmp1DB;struct Cyc_Tcenv_CtrlEnv*_tmp1DC;void*_tmp1DD;void*_tmp1DE;struct Cyc_Tcenv_FenvFlags _tmp1DF;_LLF8: _tmp1D7=_tmp1D6->shared;_tmp1D8=_tmp1D6->type_vars;_tmp1D9=_tmp1D6->region_order;_tmp1DA=_tmp1D6->locals;_tmp1DB=_tmp1D6->encloser;_tmp1DC=_tmp1D6->ctrl_env;_tmp1DD=_tmp1D6->capability;_tmp1DE=_tmp1D6->curr_rgn;_tmp1DF=_tmp1D6->flags;_LLF9:;{
# 837
struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp1E1=_region_malloc(r,sizeof(*_tmp1E1));_tmp1E1->fallthru_clause=_tmp1DC->fallthru_clause;_tmp1E1;});
# 839
struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1D7;_tmp1E0->type_vars=(struct Cyc_List_List*)_tmp1D8;_tmp1E0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1D9;_tmp1E0->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1DA;_tmp1E0->encloser=(struct Cyc_Absyn_Stmt*)_tmp1DB;_tmp1E0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp1E0->capability=(void*)_tmp1DD;_tmp1E0->curr_rgn=(void*)_tmp1DE;_tmp1E0->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1DF;_tmp1E0->fnrgn=(struct _RegionHandle*)r;_tmp1E0;});
# 842
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 848
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1E5="set_in_switch";_tag_dyneither(_tmp1E5,sizeof(char),14);})));
struct Cyc_Tcenv_Fenv*_tmp1E3=fe;struct Cyc_Tcenv_CtrlEnv*_tmp1E4;_LLFB: _tmp1E4=_tmp1E3->ctrl_env;_LLFC:;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 854
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Tcenv_Fenv*_tmp1E6=Cyc_Tcenv_get_fenv(te,({const char*_tmp1EC="add_label";_tag_dyneither(_tmp1EC,sizeof(char),10);}));struct Cyc_Tcenv_Fenv*_tmp1E7=_tmp1E6;struct Cyc_Tcenv_SharedFenv*_tmp1E8;_LLFE: _tmp1E8=_tmp1E7->shared;_LLFF:;{
struct Cyc_Dict_Dict needed=_tmp1E8->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp1E8->frgn;
if(sl_opt != 0)
_tmp1E8->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);
# 862
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1E8->seen_labels,v))
({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0;_tmp1EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);({void*_tmp1E9[1]={& _tmp1EB};Cyc_Tcutil_terr(s->loc,({const char*_tmp1EA="Repeated label: %s";_tag_dyneither(_tmp1EA,sizeof(char),19);}),_tag_dyneither(_tmp1E9,sizeof(void*),1));});});
_tmp1E8->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1E8->seen_labels,v,s);
return te;};}
# 868
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1ED=Cyc_Tcenv_get_fenv(te,({const char*_tmp1F0="all_labels_resolved";_tag_dyneither(_tmp1F0,sizeof(char),20);}));struct Cyc_Tcenv_Fenv*_tmp1EE=_tmp1ED;struct Cyc_Tcenv_SharedFenv*_tmp1EF;_LL101: _tmp1EF=_tmp1EE->shared;_LL102:;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp1EF->needed_labels);}
# 873
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1F1=Cyc_Tcenv_get_fenv(te,({const char*_tmp1F4="get_encloser";_tag_dyneither(_tmp1F4,sizeof(char),13);}));struct Cyc_Tcenv_Fenv*_tmp1F2=_tmp1F1;struct Cyc_Absyn_Stmt*_tmp1F3;_LL104: _tmp1F3=_tmp1F2->encloser;_LL105:;
return _tmp1F3;}
# 878
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp1F5=_region_malloc(r2,sizeof(*_tmp1F5));_tmp1F5->ns=te->ns;_tmp1F5->ae=te->ae;_tmp1F5->le=0;_tmp1F5->allow_valueof=1;_tmp1F5->in_extern_c_include=te->in_extern_c_include;_tmp1F5;});{
struct Cyc_Tcenv_Fenv _tmp1F6=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp1F7=_tmp1F6;struct Cyc_Tcenv_SharedFenv*_tmp1F8;struct Cyc_List_List*_tmp1F9;struct Cyc_RgnOrder_RgnPO*_tmp1FA;const struct Cyc_Tcenv_Bindings*_tmp1FB;struct Cyc_Absyn_Stmt*_tmp1FC;struct Cyc_Tcenv_CtrlEnv*_tmp1FD;void*_tmp1FE;void*_tmp1FF;struct Cyc_Tcenv_FenvFlags _tmp200;_LL107: _tmp1F8=_tmp1F7.shared;_tmp1F9=_tmp1F7.type_vars;_tmp1FA=_tmp1F7.region_order;_tmp1FB=_tmp1F7.locals;_tmp1FC=_tmp1F7.encloser;_tmp1FD=_tmp1F7.ctrl_env;_tmp1FE=_tmp1F7.capability;_tmp1FF=_tmp1F7.curr_rgn;_tmp200=_tmp1F7.flags;_LL108:;{
struct Cyc_Tcenv_Fenv*_tmp201=({struct Cyc_Tcenv_Fenv*_tmp203=_region_malloc(r2,sizeof(*_tmp203));_tmp203->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F8;_tmp203->type_vars=(struct Cyc_List_List*)_tmp1F9;_tmp203->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FA;_tmp203->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FB;_tmp203->encloser=(struct Cyc_Absyn_Stmt*)_tmp1FC;_tmp203->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1FD;_tmp203->capability=(void*)_tmp1FE;_tmp203->curr_rgn=(void*)_tmp1FF;_tmp203->flags=(struct Cyc_Tcenv_FenvFlags)_tmp200;_tmp203->fnrgn=(struct _RegionHandle*)r2;_tmp203;});
return({struct Cyc_Tcenv_Tenv*_tmp202=_region_malloc(r2,sizeof(*_tmp202));_tmp202->ns=te->ns;_tmp202->ae=te->ae;_tmp202->le=_tmp201;_tmp202->allow_valueof=1;_tmp202->in_extern_c_include=te->in_extern_c_include;_tmp202;});};};}
# 886
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp204=_region_malloc(r2,sizeof(*_tmp204));_tmp204->ns=te->ns;_tmp204->ae=te->ae;_tmp204->le=0;_tmp204->allow_valueof=te->allow_valueof;_tmp204->in_extern_c_include=1;_tmp204;});{
struct Cyc_Tcenv_Fenv _tmp205=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp206=_tmp205;struct Cyc_Tcenv_SharedFenv*_tmp207;struct Cyc_List_List*_tmp208;struct Cyc_RgnOrder_RgnPO*_tmp209;const struct Cyc_Tcenv_Bindings*_tmp20A;struct Cyc_Absyn_Stmt*_tmp20B;struct Cyc_Tcenv_CtrlEnv*_tmp20C;void*_tmp20D;void*_tmp20E;struct Cyc_Tcenv_FenvFlags _tmp20F;_LL10A: _tmp207=_tmp206.shared;_tmp208=_tmp206.type_vars;_tmp209=_tmp206.region_order;_tmp20A=_tmp206.locals;_tmp20B=_tmp206.encloser;_tmp20C=_tmp206.ctrl_env;_tmp20D=_tmp206.capability;_tmp20E=_tmp206.curr_rgn;_tmp20F=_tmp206.flags;_LL10B:;{
struct Cyc_Tcenv_Fenv*_tmp210=({struct Cyc_Tcenv_Fenv*_tmp212=_region_malloc(r2,sizeof(*_tmp212));_tmp212->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp207;_tmp212->type_vars=(struct Cyc_List_List*)_tmp208;_tmp212->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp209;_tmp212->locals=(const struct Cyc_Tcenv_Bindings*)_tmp20A;_tmp212->encloser=(struct Cyc_Absyn_Stmt*)_tmp20B;_tmp212->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp20C;_tmp212->capability=(void*)_tmp20D;_tmp212->curr_rgn=(void*)_tmp20E;_tmp212->flags=(struct Cyc_Tcenv_FenvFlags)_tmp20F;_tmp212->fnrgn=(struct _RegionHandle*)r2;_tmp212;});
return({struct Cyc_Tcenv_Tenv*_tmp211=_region_malloc(r2,sizeof(*_tmp211));_tmp211->ns=te->ns;_tmp211->ae=te->ae;_tmp211->le=_tmp210;_tmp211->allow_valueof=te->allow_valueof;_tmp211->in_extern_c_include=1;_tmp211;});};};}
# 894
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 896
struct Cyc_Tcenv_Fenv _tmp213=*
# 898
Cyc_Tcenv_get_fenv(te,({const char*_tmp220="set_encloser";_tag_dyneither(_tmp220,sizeof(char),13);}));
# 896
struct Cyc_Tcenv_Fenv _tmp214=_tmp213;struct Cyc_Tcenv_SharedFenv*_tmp215;struct Cyc_List_List*_tmp216;struct Cyc_RgnOrder_RgnPO*_tmp217;const struct Cyc_Tcenv_Bindings*_tmp218;struct Cyc_Absyn_Stmt*_tmp219;struct Cyc_Tcenv_CtrlEnv*_tmp21A;void*_tmp21B;void*_tmp21C;struct Cyc_Tcenv_FenvFlags _tmp21D;_LL10D: _tmp215=_tmp214.shared;_tmp216=_tmp214.type_vars;_tmp217=_tmp214.region_order;_tmp218=_tmp214.locals;_tmp219=_tmp214.encloser;_tmp21A=_tmp214.ctrl_env;_tmp21B=_tmp214.capability;_tmp21C=_tmp214.curr_rgn;_tmp21D=_tmp214.flags;_LL10E:;{
# 899
struct Cyc_Tcenv_Fenv*_tmp21E=({struct Cyc_Tcenv_Fenv*_tmp21F=_region_malloc(r,sizeof(*_tmp21F));_tmp21F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp215;_tmp21F->type_vars=(struct Cyc_List_List*)_tmp216;_tmp21F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp217;_tmp21F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp218;_tmp21F->encloser=(struct Cyc_Absyn_Stmt*)s;_tmp21F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp21A;_tmp21F->capability=(void*)_tmp21B;_tmp21F->curr_rgn=(void*)_tmp21C;_tmp21F->flags=(struct Cyc_Tcenv_FenvFlags)_tmp21D;_tmp21F->fnrgn=(struct _RegionHandle*)r;_tmp21F;});
# 902
return Cyc_Tcenv_put_fenv(r,te,_tmp21E);};}
# 905
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 908
struct Cyc_Tcenv_Fenv _tmp221=*
# 910
Cyc_Tcenv_get_fenv(te,({const char*_tmp237="add_region";_tag_dyneither(_tmp237,sizeof(char),11);}));
# 908
struct Cyc_Tcenv_Fenv _tmp222=_tmp221;struct Cyc_Tcenv_SharedFenv*_tmp223;struct Cyc_List_List*_tmp224;struct Cyc_RgnOrder_RgnPO*_tmp225;const struct Cyc_Tcenv_Bindings*_tmp226;struct Cyc_Absyn_Stmt*_tmp227;struct Cyc_Tcenv_CtrlEnv*_tmp228;void*_tmp229;void*_tmp22A;struct Cyc_Tcenv_FenvFlags _tmp22B;_LL110: _tmp223=_tmp222.shared;_tmp224=_tmp222.type_vars;_tmp225=_tmp222.region_order;_tmp226=_tmp222.locals;_tmp227=_tmp222.encloser;_tmp228=_tmp222.ctrl_env;_tmp229=_tmp222.capability;_tmp22A=_tmp222.curr_rgn;_tmp22B=_tmp222.flags;_LL111:;
# 911
{void*_tmp22C=Cyc_Tcutil_compress(rgn);void*_tmp22D=_tmp22C;struct Cyc_Absyn_Tvar*_tmp22E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22D)->tag == 2){_LL113: _tmp22E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22D)->f1;_LL114:
# 913
 _tmp225=Cyc_RgnOrder_add_youngest(_tmp223->frgn,_tmp225,_tmp22E,resetable,opened);
# 915
goto _LL112;}else{_LL115: _LL116:
 goto _LL112;}_LL112:;}
# 918
_tmp229=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp230;_tmp230.tag=24;_tmp230.f1=({struct Cyc_List_List*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp234;_tmp234.tag=23;_tmp234.f1=rgn;_tmp234;});_tmp233;});_tmp231->tl=({struct Cyc_List_List*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->hd=_tmp229;_tmp232->tl=0;_tmp232;});_tmp231;});_tmp230;});_tmp22F;});{
# 920
struct Cyc_Tcenv_Fenv*_tmp235=({struct Cyc_Tcenv_Fenv*_tmp236=_region_malloc(r,sizeof(*_tmp236));_tmp236->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp223;_tmp236->type_vars=(struct Cyc_List_List*)_tmp224;_tmp236->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp225;_tmp236->locals=(const struct Cyc_Tcenv_Bindings*)_tmp226;_tmp236->encloser=(struct Cyc_Absyn_Stmt*)_tmp227;_tmp236->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp228;_tmp236->capability=(void*)_tmp229;_tmp236->curr_rgn=(void*)_tmp22A;_tmp236->flags=(struct Cyc_Tcenv_FenvFlags)_tmp22B;_tmp236->fnrgn=(struct _RegionHandle*)r;_tmp236;});
# 923
return Cyc_Tcenv_put_fenv(r,te,_tmp235);};}
# 926
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 928
struct Cyc_Tcenv_Fenv _tmp238=*
# 930
Cyc_Tcenv_get_fenv(te,({const char*_tmp24F="new_named_block";_tag_dyneither(_tmp24F,sizeof(char),16);}));
# 928
struct Cyc_Tcenv_Fenv _tmp239=_tmp238;struct Cyc_Tcenv_SharedFenv*_tmp23A;struct Cyc_List_List*_tmp23B;struct Cyc_RgnOrder_RgnPO*_tmp23C;const struct Cyc_Tcenv_Bindings*_tmp23D;struct Cyc_Absyn_Stmt*_tmp23E;struct Cyc_Tcenv_CtrlEnv*_tmp23F;void*_tmp240;void*_tmp241;struct Cyc_Tcenv_FenvFlags _tmp242;_LL118: _tmp23A=_tmp239.shared;_tmp23B=_tmp239.type_vars;_tmp23C=_tmp239.region_order;_tmp23D=_tmp239.locals;_tmp23E=_tmp239.encloser;_tmp23F=_tmp239.ctrl_env;_tmp240=_tmp239.capability;_tmp241=_tmp239.curr_rgn;_tmp242=_tmp239.flags;_LL119:;{
# 932
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp24E="new_block";_tag_dyneither(_tmp24E,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp24D;_tmp24D.tag=2;_tmp24D.f1=block_rgn;_tmp24D;});_tmp24C;});
_tmp23B=({struct Cyc_List_List*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->hd=block_rgn;_tmp243->tl=_tmp23B;_tmp243;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp23B);
_tmp23C=Cyc_RgnOrder_add_youngest(_tmp23A->frgn,_tmp23C,block_rgn,0,0);
_tmp240=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp245;_tmp245.tag=24;_tmp245.f1=({struct Cyc_List_List*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp249;_tmp249.tag=23;_tmp249.f1=block_typ;_tmp249;});_tmp248;});_tmp246->tl=({struct Cyc_List_List*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->hd=_tmp240;_tmp247->tl=0;_tmp247;});_tmp246;});_tmp245;});_tmp244;});
_tmp241=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp24A=({struct Cyc_Tcenv_Fenv*_tmp24B=_region_malloc(r,sizeof(*_tmp24B));_tmp24B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp23A;_tmp24B->type_vars=(struct Cyc_List_List*)_tmp23B;_tmp24B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp23C;_tmp24B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp23D;_tmp24B->encloser=(struct Cyc_Absyn_Stmt*)_tmp23E;_tmp24B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp23F;_tmp24B->capability=(void*)_tmp240;_tmp24B->curr_rgn=(void*)_tmp241;_tmp24B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp242;_tmp24B->fnrgn=(struct _RegionHandle*)r;_tmp24B;});
# 942
return Cyc_Tcenv_put_fenv(r,te,_tmp24A);};};}
# 945
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 947
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 949
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 955
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 959
struct Cyc_Tcenv_Fenv _tmp251=*
# 961
Cyc_Tcenv_get_fenv(te,({const char*_tmp25E="new_outlives_constraints";_tag_dyneither(_tmp25E,sizeof(char),25);}));
# 959
struct Cyc_Tcenv_Fenv _tmp252=_tmp251;struct Cyc_Tcenv_SharedFenv*_tmp253;struct Cyc_List_List*_tmp254;struct Cyc_RgnOrder_RgnPO*_tmp255;const struct Cyc_Tcenv_Bindings*_tmp256;struct Cyc_Absyn_Stmt*_tmp257;struct Cyc_Tcenv_CtrlEnv*_tmp258;void*_tmp259;void*_tmp25A;struct Cyc_Tcenv_FenvFlags _tmp25B;_LL11B: _tmp253=_tmp252.shared;_tmp254=_tmp252.type_vars;_tmp255=_tmp252.region_order;_tmp256=_tmp252.locals;_tmp257=_tmp252.encloser;_tmp258=_tmp252.ctrl_env;_tmp259=_tmp252.capability;_tmp25A=_tmp252.curr_rgn;_tmp25B=_tmp252.flags;_LL11C:;
# 962
for(0;cs != 0;cs=cs->tl){
_tmp255=
Cyc_RgnOrder_add_outlives_constraint(_tmp253->frgn,_tmp255,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 967
struct Cyc_Tcenv_Fenv*_tmp25C=({struct Cyc_Tcenv_Fenv*_tmp25D=_region_malloc(r,sizeof(*_tmp25D));_tmp25D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp253;_tmp25D->type_vars=(struct Cyc_List_List*)_tmp254;_tmp25D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp255;_tmp25D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp256;_tmp25D->encloser=(struct Cyc_Absyn_Stmt*)_tmp257;_tmp25D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp258;_tmp25D->capability=(void*)_tmp259;_tmp25D->curr_rgn=(void*)_tmp25A;_tmp25D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp25B;_tmp25D->fnrgn=(struct _RegionHandle*)r;_tmp25D;});
# 970
return Cyc_Tcenv_put_fenv(r,te,_tmp25C);};}
# 973
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 981
void*_tmp25F=Cyc_Tcutil_compress(r1);
void*_tmp260=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp261=Cyc_Tcutil_typ_kind(_tmp25F);
struct Cyc_Absyn_Kind*_tmp262=Cyc_Tcutil_typ_kind(_tmp260);
# 987
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp261,_tmp262);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp262,_tmp261);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp268;_tmp268.tag=0;_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp262));({struct Cyc_String_pa_PrintArg_struct _tmp267;_tmp267.tag=0;_tmp267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp261));({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp260));({struct Cyc_String_pa_PrintArg_struct _tmp265;_tmp265.tag=0;_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25F));({void*_tmp263[4]={& _tmp265,& _tmp266,& _tmp267,& _tmp268};Cyc_Tcutil_terr(loc,({const char*_tmp264="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp264,sizeof(char),82);}),_tag_dyneither(_tmp263,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 994
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp269=Cyc_Tcutil_swap_kind(_tmp260,Cyc_Tcutil_kind_to_bound(_tmp261));struct _tuple11 _tmp26A=_tmp269;struct Cyc_Absyn_Tvar*_tmp26B;void*_tmp26C;_LL11E: _tmp26B=_tmp26A.f1;_tmp26C=_tmp26A.f2;_LL11F:;
*oldtv=({struct _tuple11*_tmp26D=_region_malloc(r,sizeof(*_tmp26D));_tmp26D->f1=_tmp26B;_tmp26D->f2=_tmp26C;_tmp26D;});}else{
# 998
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp26E=Cyc_Tcutil_swap_kind(_tmp25F,Cyc_Tcutil_kind_to_bound(_tmp262));struct _tuple11 _tmp26F=_tmp26E;struct Cyc_Absyn_Tvar*_tmp270;void*_tmp271;_LL121: _tmp270=_tmp26F.f1;_tmp271=_tmp26F.f2;_LL122:;
*oldtv=({struct _tuple11*_tmp272=_region_malloc(r,sizeof(*_tmp272));_tmp272->f1=_tmp270;_tmp272->f2=_tmp271;_tmp272;});}}}{
# 1004
struct _RegionHandle*_tmp273=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp274=0;
if((_tmp25F != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp25F != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp25F != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp278;_tmp278.tag=23;_tmp278.f1=_tmp25F;_tmp278;});_tmp277;});
_tmp274=({struct Cyc_List_List*_tmp275=_region_malloc(_tmp273,sizeof(*_tmp275));_tmp275->hd=({struct _tuple13*_tmp276=_region_malloc(_tmp273,sizeof(*_tmp276));_tmp276->f1=eff1;_tmp276->f2=_tmp260;_tmp276;});_tmp275->tl=_tmp274;_tmp275;});}
# 1010
if((_tmp260 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp260 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp260 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp27C;_tmp27C.tag=23;_tmp27C.f1=_tmp260;_tmp27C;});_tmp27B;});
_tmp274=({struct Cyc_List_List*_tmp279=_region_malloc(_tmp273,sizeof(*_tmp279));_tmp279->hd=({struct _tuple13*_tmp27A=_region_malloc(_tmp273,sizeof(*_tmp27A));_tmp27A->f1=eff2;_tmp27A->f2=_tmp25F;_tmp27A;});_tmp279->tl=_tmp274;_tmp279;});}
# 1015
return Cyc_Tcenv_new_outlives_constraints(_tmp273,te,_tmp274,loc);};}
# 1018
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp27D=te->le;
if(_tmp27D == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp27D;
struct Cyc_Tcenv_Fenv*_tmp27E=fe;void*_tmp27F;_LL124: _tmp27F=_tmp27E->curr_rgn;_LL125:;
return _tmp27F;};}
# 1029
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp288="check_rgn_accessible";_tag_dyneither(_tmp288,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp280=fe;void*_tmp281;struct Cyc_RgnOrder_RgnPO*_tmp282;_LL127: _tmp281=_tmp280->capability;_tmp282=_tmp280->region_order;_LL128:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp281) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp281))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp282,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp284;_tmp284.tag=23;_tmp284.f1=rgn;_tmp284;});_tmp283;}),_tmp281))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp287;_tmp287.tag=0;_tmp287.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp285[1]={& _tmp287};Cyc_Tcutil_terr(loc,({const char*_tmp286="Expression accesses unavailable region %s";_tag_dyneither(_tmp286,sizeof(char),42);}),_tag_dyneither(_tmp285,sizeof(void*),1));});});}
# 1041
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp289=
Cyc_Tcenv_get_fenv(te,({const char*_tmp294="check_rgn_resetable";_tag_dyneither(_tmp294,sizeof(char),20);}));
# 1043
struct Cyc_Tcenv_Fenv*_tmp28A=_tmp289;struct Cyc_RgnOrder_RgnPO*_tmp28B;_LL12A: _tmp28B=_tmp28A->region_order;_LL12B:;{
# 1045
void*_tmp28C=Cyc_Tcutil_compress(rgn);void*_tmp28D=_tmp28C;struct Cyc_Absyn_Tvar*_tmp28E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp28D)->tag == 2){_LL12D: _tmp28E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp28D)->f1;_LL12E:
# 1047
 if(!Cyc_RgnOrder_is_region_resetable(_tmp28B,_tmp28E))
({struct Cyc_String_pa_PrintArg_struct _tmp291;_tmp291.tag=0;_tmp291.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp28F[1]={& _tmp291};Cyc_Tcutil_terr(loc,({const char*_tmp290="Region %s is not resetable";_tag_dyneither(_tmp290,sizeof(char),27);}),_tag_dyneither(_tmp28F,sizeof(void*),1));});});
return;}else{_LL12F: _LL130:
# 1051
({void*_tmp292=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp293="check_rgn_resetable";_tag_dyneither(_tmp293,sizeof(char),20);}),_tag_dyneither(_tmp292,sizeof(void*),0));});}_LL12C:;};};}
# 1058
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp295=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1063
if(_tmp295 == 0){
void*_tmp296=rt_a;switch(*((int*)_tmp296)){case 22: _LL132: _LL133:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LL134: _LL135:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LL136: _LL137:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL138: _LL139:
 return 0;}_LL131:;}{
# 1071
struct Cyc_Tcenv_Fenv*fe=_tmp295;
struct Cyc_Tcenv_Fenv*_tmp297=fe;struct Cyc_RgnOrder_RgnPO*_tmp298;_LL13B: _tmp298=_tmp297->region_order;_LL13C:;{
int _tmp299=Cyc_RgnOrder_effect_outlives(_tmp298,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp29B;_tmp29B.tag=23;_tmp29B.f1=rt_a;_tmp29B;});_tmp29A;}),rt_b);
# 1077
return _tmp299;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1082
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp29C=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2A3="check_effect_accessible";_tag_dyneither(_tmp2A3,sizeof(char),24);}));
# 1083
struct Cyc_Tcenv_Fenv*_tmp29D=_tmp29C;void*_tmp29E;struct Cyc_RgnOrder_RgnPO*_tmp29F;struct Cyc_Tcenv_SharedFenv*_tmp2A0;_LL13E: _tmp29E=_tmp29D->capability;_tmp29F=_tmp29D->region_order;_tmp2A0=_tmp29D->shared;_LL13F:;
# 1085
if(Cyc_Tcutil_subset_effect(0,eff,_tmp29E))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp29F,eff,_tmp29E))
return;{
struct _RegionHandle*frgn=_tmp2A0->frgn;
_tmp2A0->delayed_effect_checks=({struct Cyc_List_List*_tmp2A1=_region_malloc(frgn,sizeof(*_tmp2A1));_tmp2A1->hd=({struct _tuple14*_tmp2A2=_region_malloc(frgn,sizeof(*_tmp2A2));_tmp2A2->f1=_tmp29E;_tmp2A2->f2=eff;_tmp2A2->f3=_tmp29F;_tmp2A2->f4=loc;_tmp2A2;});_tmp2A1->tl=_tmp2A0->delayed_effect_checks;_tmp2A1;});};}
# 1094
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2A4=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2B2="check_delayed_effects";_tag_dyneither(_tmp2B2,sizeof(char),22);}));
# 1095
struct Cyc_Tcenv_Fenv*_tmp2A5=_tmp2A4;struct Cyc_Tcenv_SharedFenv*_tmp2A6;_LL141: _tmp2A6=_tmp2A5->shared;_LL142:;{
# 1097
struct Cyc_List_List*_tmp2A7=_tmp2A6->delayed_effect_checks;
for(0;_tmp2A7 != 0;_tmp2A7=_tmp2A7->tl){
struct _tuple14*_tmp2A8=(struct _tuple14*)_tmp2A7->hd;struct _tuple14*_tmp2A9=_tmp2A8;void*_tmp2AA;void*_tmp2AB;struct Cyc_RgnOrder_RgnPO*_tmp2AC;unsigned int _tmp2AD;_LL144: _tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2A9->f2;_tmp2AC=_tmp2A9->f3;_tmp2AD=_tmp2A9->f4;_LL145:;
if(Cyc_Tcutil_subset_effect(1,_tmp2AB,_tmp2AA))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2AC,_tmp2AB,_tmp2AA))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2AB));({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0;_tmp2B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2AA));({void*_tmp2AE[2]={& _tmp2B0,& _tmp2B1};Cyc_Tcutil_terr(_tmp2AD,({const char*_tmp2AF="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp2AF,sizeof(char),51);}),_tag_dyneither(_tmp2AE,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1113
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1115
struct Cyc_Tcenv_Fenv*_tmp2B3=te->le;
if(_tmp2B3 == 0){
# 1118
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2B5;_tmp2B5.tag=23;_tmp2B5.f1=(*((struct _tuple13*)po->hd)).f2;_tmp2B5;});_tmp2B4;}),Cyc_Absyn_empty_effect))
({void*_tmp2B6=0;Cyc_Tcutil_terr(loc,({const char*_tmp2B7="the required region ordering is not satisfied here";_tag_dyneither(_tmp2B7,sizeof(char),51);}),_tag_dyneither(_tmp2B6,sizeof(void*),0));});}
return;}{
# 1124
struct Cyc_Tcenv_Fenv*_tmp2B8=_tmp2B3;struct Cyc_Tcenv_Fenv*_tmp2B9=_tmp2B8;struct Cyc_RgnOrder_RgnPO*_tmp2BA;struct Cyc_Tcenv_SharedFenv*_tmp2BB;_LL147: _tmp2BA=_tmp2B9->region_order;_tmp2BB=_tmp2B9->shared;_LL148:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2BA,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp2BB->delayed_constraint_checks=({struct Cyc_List_List*_tmp2BC=_region_malloc(_tmp2BB->frgn,sizeof(*_tmp2BC));_tmp2BC->hd=({struct _tuple15*_tmp2BD=_region_malloc(_tmp2BB->frgn,sizeof(*_tmp2BD));_tmp2BD->f1=_tmp2BA;_tmp2BD->f2=po;_tmp2BD->f3=loc;_tmp2BD;});_tmp2BC->tl=_tmp2BB->delayed_constraint_checks;_tmp2BC;});};}
# 1131
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2BE=
Cyc_Tcenv_get_fenv(te,({const char*_tmp2C9="check_delayed_constraints";_tag_dyneither(_tmp2C9,sizeof(char),26);}));
# 1132
struct Cyc_Tcenv_Fenv*_tmp2BF=_tmp2BE;struct Cyc_Tcenv_SharedFenv*_tmp2C0;_LL14A: _tmp2C0=_tmp2BF->shared;_LL14B:;{
# 1134
struct Cyc_List_List*_tmp2C1=_tmp2C0->delayed_constraint_checks;
for(0;_tmp2C1 != 0;_tmp2C1=_tmp2C1->tl){
struct _tuple15*_tmp2C2=(struct _tuple15*)_tmp2C1->hd;struct _tuple15*_tmp2C3=_tmp2C2;struct Cyc_RgnOrder_RgnPO*_tmp2C4;struct Cyc_List_List*_tmp2C5;unsigned int _tmp2C6;_LL14D: _tmp2C4=_tmp2C3->f1;_tmp2C5=_tmp2C3->f2;_tmp2C6=_tmp2C3->f3;_LL14E:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2C4,_tmp2C5,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp2C7=0;Cyc_Tcutil_terr(_tmp2C6,({const char*_tmp2C8="the required region ordering is not satisfied here";_tag_dyneither(_tmp2C8,sizeof(char),51);}),_tag_dyneither(_tmp2C7,sizeof(void*),0));});}};}
# 1142
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA->name=({struct _dyneither_ptr*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FE;_tmp2FE.tag=0;_tmp2FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp2FC[1]={& _tmp2FE};Cyc_aprintf(({const char*_tmp2FD="`%s";_tag_dyneither(_tmp2FD,sizeof(char),4);}),_tag_dyneither(_tmp2FC,sizeof(void*),1));});});_tmp2FB;});_tmp2FA->identity=
# 1146
Cyc_Tcutil_new_tvar_id();_tmp2FA->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp2FA;});
struct Cyc_List_List*_tmp2CA=({struct Cyc_List_List*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->hd=rgn0;_tmp2F9->tl=fd->tvs;_tmp2F9;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp2CA);{
# 1150
struct Cyc_RgnOrder_RgnPO*_tmp2CB=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1152
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2F8;_tmp2F8.tag=2;_tmp2F8.f1=rgn0;_tmp2F8;});_tmp2F7;});
struct Cyc_List_List*_tmp2CC=0;
{struct Cyc_List_List*_tmp2CD=fd->args;for(0;_tmp2CD != 0;_tmp2CD=_tmp2CD->tl){
struct Cyc_Absyn_Vardecl*_tmp2CE=({struct Cyc_Absyn_Vardecl*_tmp2D6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2D6[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2D7;_tmp2D7.sc=Cyc_Absyn_Public;_tmp2D7.name=({struct _tuple0*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->f1=Cyc_Absyn_Loc_n;_tmp2D8->f2=(*((struct _tuple8*)_tmp2CD->hd)).f1;_tmp2D8;});_tmp2D7.tq=(*((struct _tuple8*)_tmp2CD->hd)).f2;_tmp2D7.type=(*((struct _tuple8*)_tmp2CD->hd)).f3;_tmp2D7.initializer=0;_tmp2D7.rgn=param_rgn;_tmp2D7.attributes=0;_tmp2D7.escapes=0;_tmp2D7;});_tmp2D6;});
# 1162
_tmp2CC=({struct Cyc_List_List*_tmp2CF=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp2CF[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp2D0;_tmp2D0.hd=_tmp2CE;_tmp2D0.tl=_tmp2CC;_tmp2D0;});_tmp2CF;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2D1=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2D5;_tmp2D5.tag=3;_tmp2D5.f1=_tmp2CE;_tmp2D5;});_tmp2D4;});
struct _dyneither_ptr*_tmp2D2=(*((struct _tuple8*)_tmp2CD->hd)).f1;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2D3=_region_malloc(r,sizeof(*_tmp2D3));_tmp2D3->v=_tmp2D2;_tmp2D3->b=(void*)_tmp2D1;_tmp2D3->tl=locals;_tmp2D3;});};}}
# 1167
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp2D9=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp2DA=_tmp2D9;struct _dyneither_ptr*_tmp2DB;struct Cyc_Absyn_Tqual _tmp2DC;void*_tmp2DD;int _tmp2DE;_LL150: _tmp2DB=_tmp2DA.name;_tmp2DC=_tmp2DA.tq;_tmp2DD=_tmp2DA.type;_tmp2DE=_tmp2DA.inject;_LL151:;
if(_tmp2DB != 0){
void*_tmp2DF=Cyc_Absyn_dyneither_typ(_tmp2DD,param_rgn,_tmp2DC,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp2E0=({struct Cyc_Absyn_Vardecl*_tmp2E7=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2E7[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2E8;_tmp2E8.sc=Cyc_Absyn_Public;_tmp2E8.name=({struct _tuple0*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->f1=Cyc_Absyn_Loc_n;_tmp2E9->f2=_tmp2DB;_tmp2E9;});_tmp2E8.tq=
# 1173
Cyc_Absyn_empty_tqual(0);_tmp2E8.type=_tmp2DF;_tmp2E8.initializer=0;_tmp2E8.rgn=param_rgn;_tmp2E8.attributes=0;_tmp2E8.escapes=0;_tmp2E8;});_tmp2E7;});
# 1178
_tmp2CC=({struct Cyc_List_List*_tmp2E1=_cycalloc(sizeof(*_tmp2E1));_tmp2E1->hd=_tmp2E0;_tmp2E1->tl=_tmp2CC;_tmp2E1;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E2=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2E6;_tmp2E6.tag=3;_tmp2E6.f1=_tmp2E0;_tmp2E6;});_tmp2E5;});
struct _dyneither_ptr*_tmp2E3=_tmp2DB;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2E4=_region_malloc(r,sizeof(*_tmp2E4));_tmp2E4->v=_tmp2E3;_tmp2E4->b=(void*)_tmp2E2;_tmp2E4->tl=locals;_tmp2E4;});};}else{
# 1183
({void*_tmp2EA=0;Cyc_Tcutil_terr(loc,({const char*_tmp2EB="missing name for varargs";_tag_dyneither(_tmp2EB,sizeof(char),25);}),_tag_dyneither(_tmp2EA,sizeof(void*),0));});}}
# 1185
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp2EC=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp2EC[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp2ED;_tmp2ED.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2CC);_tmp2ED;});_tmp2EC;});
return({struct Cyc_Tcenv_Fenv*_tmp2EE=_region_malloc(r,sizeof(*_tmp2EE));_tmp2EE->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp2F6=_region_malloc(r,sizeof(*_tmp2F6));_tmp2F6->frgn=r;_tmp2F6->return_typ=fd->ret_type;_tmp2F6->seen_labels=
# 1190
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2F6->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2F6->delayed_effect_checks=0;_tmp2F6->delayed_constraint_checks=0;_tmp2F6;});_tmp2EE->type_vars=(struct Cyc_List_List*)_tmp2CA;_tmp2EE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2CB;_tmp2EE->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp2EE->encloser=(struct Cyc_Absyn_Stmt*)fd->body;_tmp2EE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2F5=_region_malloc(r,sizeof(*_tmp2F5));_tmp2F5->fallthru_clause=0;_tmp2F5;});_tmp2EE->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2F0;_tmp2F0.tag=24;_tmp2F0.f1=({struct Cyc_List_List*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2F4;_tmp2F4.tag=23;_tmp2F4.f1=param_rgn;_tmp2F4;});_tmp2F3;});_tmp2F1->tl=({struct Cyc_List_List*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2->hd=(void*)_check_null(fd->effect);_tmp2F2->tl=0;_tmp2F2;});_tmp2F1;});_tmp2F0;});_tmp2EF;}));_tmp2EE->curr_rgn=(void*)param_rgn;_tmp2EE->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp2EE->flags).in_notreadctxt=0;(_tmp2EE->flags).in_lhs=0;(_tmp2EE->flags).in_new=0;(_tmp2EE->flags).abstract_ok=0;_tmp2EE->flags;});_tmp2EE->fnrgn=(struct _RegionHandle*)r;_tmp2EE;});};}
# 1208
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 1211
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp31F->identity=Cyc_Tcutil_new_tvar_id();_tmp31F->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp31F;});
struct Cyc_List_List*_tmp2FF=({struct Cyc_List_List*_tmp31E=_cycalloc(sizeof(*_tmp31E));_tmp31E->hd=rgn0;_tmp31E->tl=0;_tmp31E;});
struct Cyc_RgnOrder_RgnPO*_tmp300=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp31D;_tmp31D.tag=24;_tmp31D.f1=0;_tmp31D;});_tmp31C;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp31B;_tmp31B.tag=2;_tmp31B.f1=rgn0;_tmp31B;});_tmp31A;});
struct Cyc_List_List*_tmp301=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp302=*((struct _tuple8*)args->hd);struct _tuple8 _tmp303=_tmp302;struct _dyneither_ptr*_tmp304;struct Cyc_Absyn_Tqual _tmp305;void*_tmp306;_LL153: _tmp304=_tmp303.f1;_tmp305=_tmp303.f2;_tmp306=_tmp303.f3;_LL154:;
if(_tmp304 != 0){
struct Cyc_Absyn_Vardecl*_tmp307=({struct Cyc_Absyn_Vardecl*_tmp30F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp30F[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp310;_tmp310.sc=Cyc_Absyn_Public;_tmp310.name=({struct _tuple0*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->f1=Cyc_Absyn_Loc_n;_tmp311->f2=_tmp304;_tmp311;});_tmp310.tq=_tmp305;_tmp310.type=_tmp306;_tmp310.initializer=0;_tmp310.rgn=param_rgn;_tmp310.attributes=0;_tmp310.escapes=0;_tmp310;});_tmp30F;});
# 1227
_tmp301=({struct Cyc_List_List*_tmp308=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp308[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp309;_tmp309.hd=_tmp307;_tmp309.tl=_tmp301;_tmp309;});_tmp308;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp30A=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp30E;_tmp30E.tag=3;_tmp30E.f1=_tmp307;_tmp30E;});_tmp30D;});
struct _dyneither_ptr*_tmp30B=_tmp304;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp30C=_region_malloc(r,sizeof(*_tmp30C));_tmp30C->v=_tmp30B;_tmp30C->b=(void*)_tmp30A;_tmp30C->tl=locals;_tmp30C;});};}}
# 1233
return({struct Cyc_Tcenv_Fenv*_tmp312=_region_malloc(r,sizeof(*_tmp312));_tmp312->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp319=_region_malloc(r,sizeof(*_tmp319));_tmp319->frgn=r;_tmp319->return_typ=ret_type;_tmp319->seen_labels=
# 1237
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp319->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp319->delayed_effect_checks=0;_tmp319->delayed_constraint_checks=0;_tmp319;});_tmp312->type_vars=(struct Cyc_List_List*)_tmp2FF;_tmp312->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp300;_tmp312->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp312->encloser=(struct Cyc_Absyn_Stmt*)
# 1245
Cyc_Absyn_skip_stmt(0);_tmp312->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp318=_region_malloc(r,sizeof(*_tmp318));_tmp318->fallthru_clause=0;_tmp318;});_tmp312->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp314;_tmp314.tag=24;_tmp314.f1=({struct Cyc_List_List*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp317;_tmp317.tag=23;_tmp317.f1=param_rgn;_tmp317;});_tmp316;});_tmp315->tl=0;_tmp315;});_tmp314;});_tmp313;}));_tmp312->curr_rgn=(void*)param_rgn;_tmp312->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp312->flags).in_notreadctxt=0;(_tmp312->flags).in_lhs=0;(_tmp312->flags).in_new=0;(_tmp312->flags).abstract_ok=0;_tmp312->flags;});_tmp312->fnrgn=(struct _RegionHandle*)r;_tmp312;});}
# 1254
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp321=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp322;struct Cyc_RgnOrder_RgnPO*_tmp323;struct Cyc_List_List*_tmp324;struct Cyc_Tcenv_SharedFenv*_tmp325;struct _RegionHandle*_tmp326;_LL156: _tmp322=_tmp321->locals;_tmp323=_tmp321->region_order;_tmp324=_tmp321->type_vars;_tmp325=_tmp321->shared;_tmp326=_tmp321->fnrgn;_LL157:;{
# 1257
struct _RegionHandle*_tmp327=_tmp325->frgn;
const struct Cyc_Tcenv_Bindings*_tmp328=_tmp322;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->name=({struct _dyneither_ptr*_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp363;_tmp363.tag=0;_tmp363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp361[1]={& _tmp363};Cyc_aprintf(({const char*_tmp362="`%s";_tag_dyneither(_tmp362,sizeof(char),4);}),_tag_dyneither(_tmp361,sizeof(void*),1));});});_tmp360;});_tmp35F->identity=
# 1261
Cyc_Tcutil_new_tvar_id();_tmp35F->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp35F;});
# 1263
{struct Cyc_List_List*_tmp329=fd->tvs;for(0;_tmp329 != 0;_tmp329=_tmp329->tl){
struct Cyc_Absyn_Kind*_tmp32A=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp329->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp32B=_tmp32A;enum Cyc_Absyn_KindQual _tmp32C;enum Cyc_Absyn_AliasQual _tmp32D;_LL159: _tmp32C=_tmp32B->kind;_tmp32D=_tmp32B->aliasqual;_LL15A:;
if(_tmp32C == Cyc_Absyn_RgnKind){
if(_tmp32D == Cyc_Absyn_Aliasable)
_tmp323=Cyc_RgnOrder_add_youngest(_tmp327,_tmp323,(struct Cyc_Absyn_Tvar*)_tmp329->hd,0,0);else{
# 1269
({void*_tmp32E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp32F="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp32F,sizeof(char),39);}),_tag_dyneither(_tmp32E,sizeof(void*),0));});}}}}
# 1271
_tmp323=Cyc_RgnOrder_add_youngest(_tmp327,_tmp323,rgn0,0,0);{
struct Cyc_List_List*_tmp330=({struct Cyc_List_List*_tmp35E=_cycalloc(sizeof(*_tmp35E));_tmp35E->hd=rgn0;_tmp35E->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp324);_tmp35E;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp330);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp35D;_tmp35D.tag=2;_tmp35D.f1=rgn0;_tmp35D;});_tmp35C;});
struct Cyc_List_List*_tmp331=0;
{struct Cyc_List_List*_tmp332=fd->args;for(0;_tmp332 != 0;_tmp332=_tmp332->tl){
struct Cyc_Absyn_Vardecl*_tmp333=({struct Cyc_Absyn_Vardecl*_tmp33B=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp33B[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp33C;_tmp33C.sc=Cyc_Absyn_Public;_tmp33C.name=({struct _tuple0*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->f1=Cyc_Absyn_Loc_n;_tmp33D->f2=(*((struct _tuple8*)_tmp332->hd)).f1;_tmp33D;});_tmp33C.tq=(*((struct _tuple8*)_tmp332->hd)).f2;_tmp33C.type=(*((struct _tuple8*)_tmp332->hd)).f3;_tmp33C.initializer=0;_tmp33C.rgn=param_rgn;_tmp33C.attributes=0;_tmp33C.escapes=0;_tmp33C;});_tmp33B;});
# 1284
_tmp331=({struct Cyc_List_List*_tmp334=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp334[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp335;_tmp335.hd=_tmp333;_tmp335.tl=_tmp331;_tmp335;});_tmp334;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp336=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp33A;_tmp33A.tag=3;_tmp33A.f1=_tmp333;_tmp33A;});_tmp339;});
struct _dyneither_ptr*_tmp337=(*((struct _tuple8*)_tmp332->hd)).f1;
_tmp328=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp338=_region_malloc(_tmp327,sizeof(*_tmp338));_tmp338->v=_tmp337;_tmp338->b=(void*)_tmp336;_tmp338->tl=_tmp328;_tmp338;});};}}
# 1289
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp33E=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp33F=_tmp33E;struct _dyneither_ptr*_tmp340;struct Cyc_Absyn_Tqual _tmp341;void*_tmp342;int _tmp343;_LL15C: _tmp340=_tmp33F.name;_tmp341=_tmp33F.tq;_tmp342=_tmp33F.type;_tmp343=_tmp33F.inject;_LL15D:;
if(_tmp340 != 0){
void*_tmp344=Cyc_Absyn_dyneither_typ(_tmp342,param_rgn,_tmp341,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp345=({struct Cyc_Absyn_Vardecl*_tmp34C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp34C[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp34D;_tmp34D.sc=Cyc_Absyn_Public;_tmp34D.name=({struct _tuple0*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->f1=Cyc_Absyn_Loc_n;_tmp34E->f2=_tmp340;_tmp34E;});_tmp34D.tq=
# 1295
Cyc_Absyn_empty_tqual(0);_tmp34D.type=_tmp344;_tmp34D.initializer=0;_tmp34D.rgn=param_rgn;_tmp34D.attributes=0;_tmp34D.escapes=0;_tmp34D;});_tmp34C;});
# 1300
_tmp331=({struct Cyc_List_List*_tmp346=_cycalloc(sizeof(*_tmp346));_tmp346->hd=_tmp345;_tmp346->tl=_tmp331;_tmp346;});{
struct _dyneither_ptr*_tmp347=_tmp340;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp348=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp34B;_tmp34B.tag=3;_tmp34B.f1=_tmp345;_tmp34B;});_tmp34A;});
_tmp328=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp349=_region_malloc(_tmp327,sizeof(*_tmp349));_tmp349->v=_tmp347;_tmp349->b=(void*)_tmp348;_tmp349->tl=_tmp328;_tmp349;});};}else{
# 1305
({void*_tmp34F=0;Cyc_Tcutil_terr(loc,({const char*_tmp350="missing name for varargs";_tag_dyneither(_tmp350,sizeof(char),25);}),_tag_dyneither(_tmp34F,sizeof(void*),0));});}}
# 1307
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp351=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp351[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp352;_tmp352.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp331);_tmp352;});_tmp351;});
return({struct Cyc_Tcenv_Fenv*_tmp353=_region_malloc(_tmp327,sizeof(*_tmp353));_tmp353->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp35B=_region_malloc(_tmp327,sizeof(*_tmp35B));_tmp35B->frgn=_tmp327;_tmp35B->return_typ=fd->ret_type;_tmp35B->seen_labels=
# 1312
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp327,Cyc_strptrcmp);_tmp35B->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp327,Cyc_strptrcmp);_tmp35B->delayed_effect_checks=0;_tmp35B->delayed_constraint_checks=0;_tmp35B;});_tmp353->type_vars=(struct Cyc_List_List*)_tmp330;_tmp353->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp323;_tmp353->locals=(const struct Cyc_Tcenv_Bindings*)_tmp328;_tmp353->encloser=(struct Cyc_Absyn_Stmt*)fd->body;_tmp353->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp35A=_region_malloc(_tmp327,sizeof(*_tmp35A));_tmp35A->fallthru_clause=0;_tmp35A;});_tmp353->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp355;_tmp355.tag=24;_tmp355.f1=({struct Cyc_List_List*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp359;_tmp359.tag=23;_tmp359.f1=param_rgn;_tmp359;});_tmp358;});_tmp356->tl=({struct Cyc_List_List*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->hd=(void*)_check_null(fd->effect);_tmp357->tl=0;_tmp357;});_tmp356;});_tmp355;});_tmp354;}));_tmp353->curr_rgn=(void*)param_rgn;_tmp353->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp353->flags).in_notreadctxt=0;(_tmp353->flags).in_lhs=0;(_tmp353->flags).in_new=0;(_tmp353->flags).abstract_ok=0;_tmp353->flags;});_tmp353->fnrgn=(struct _RegionHandle*)_tmp326;_tmp353;});};};};}
