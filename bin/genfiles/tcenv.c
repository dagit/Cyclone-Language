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
struct Cyc_Tcenv_Genv*_tmp3A9;return(_tmp3A9=_region_malloc(r,sizeof(*_tmp3A9)),((_tmp3A9->grgn=r,((_tmp3A9->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp3A9->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->availables=0,_tmp3A9)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 253
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 255
{struct Cyc_Core_Opt*_tmp3AA;Cyc_Tcutil_empty_var_set=((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3AA))));}{
# 257
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp3AB;_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB[0]=Cyc_Absyn_exn_tud,_tmp3AB)))));}
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3B1;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3B0;struct _tuple12*_tmp3AF;_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,(
(_tmp3AF=_region_malloc(r,sizeof(*_tmp3AF)),((_tmp3AF->f1=(void*)(
(_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1[0]=((_tmp3B0.tag=2,((_tmp3B0.f1=Cyc_Absyn_exn_tud,((_tmp3B0.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd,_tmp3B0)))))),_tmp3B1)))),((_tmp3AF->f2=1,_tmp3AF)))))));}}{
# 264
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
struct Cyc_Tcenv_Tenv*_tmp3B2;return(_tmp3B2=_region_malloc(r,sizeof(*_tmp3B2)),((_tmp3B2->ns=0,((_tmp3B2->ae=ae,((_tmp3B2->le=0,((_tmp3B2->allow_valueof=0,((_tmp3B2->in_extern_c_include=0,_tmp3B2)))))))))));};};}
# 270
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 272
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 276
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp3B3;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3B3="outer_namespace",_tag_dyneither(_tmp3B3,sizeof(char),16))));}
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
const char*_tmp3B7;void*_tmp3B6[1];struct Cyc_String_pa_PrintArg_struct _tmp3B5;(_tmp3B5.tag=0,((_tmp3B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3B6[0]=& _tmp3B5,Cyc_Tcutil_terr(loc,((_tmp3B7="%s is ambiguous",_tag_dyneither(_tmp3B7,sizeof(char),16))),_tag_dyneither(_tmp3B6,sizeof(void*),1)))))));};
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
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3BA;struct Cyc_Dict_Absent_exn_struct*_tmp3B9;(int)_throw((void*)((_tmp3B9=_cycalloc_atomic(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3BA.tag=Cyc_Dict_Absent,_tmp3BA)),_tmp3B9)))));}
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
struct _tuple0*_tmp27=q;union Cyc_Absyn_Nmspace _tmp28;struct _dyneither_ptr*_tmp29;_LL10: _tmp28=_tmp27->f1;_tmp29=_tmp27->f2;_LL11: {
union Cyc_Absyn_Nmspace _tmp2A=_tmp28;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2C;struct _dyneither_ptr*_tmp2D;struct Cyc_List_List*_tmp2E;switch((_tmp2A.C_n).tag){case 4: _LL15: _LL16:
 goto _LL18;case 1: if((_tmp2A.Rel_n).val == 0){_LL17: _LL18:
# 349
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp29,is_use);}else{_LL19: _tmp2D=(struct _dyneither_ptr*)((_tmp2A.Rel_n).val)->hd;_tmp2E=((_tmp2A.Rel_n).val)->tl;_LL1A: {
# 351
struct Cyc_Tcenv_Genv*_tmp2F=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2D,_tmp2E);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2F,_tmp29,is_use);}}case 3: _LL1B: _tmp2C=(_tmp2A.C_n).val;_LL1C:
 _tmp2B=_tmp2C;goto _LL1E;default: _LL1D: _tmp2B=(_tmp2A.Abs_n).val;_LL1E:
# 355
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2B),_tmp29,is_use);}_LL14:;}_LLF:;}
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
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp37,n)){
struct Cyc_List_List*_tmp3BD;struct Cyc_List_List*_tmp3BC;_tmp32=((_tmp3BC=_region_malloc(temp,sizeof(*_tmp3BC)),((_tmp3BC->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30,((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD->hd=n,((_tmp3BD->tl=ns,_tmp3BD))))))),((_tmp3BC->tl=_tmp32,_tmp3BC))))));}}}
# 377
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp35,n)){
struct Cyc_List_List*_tmp3C0;struct Cyc_List_List*_tmp3BF;_tmp32=((_tmp3BF=_region_malloc(temp,sizeof(*_tmp3BF)),((_tmp3BF->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30,((_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0->hd=n,((_tmp3C0->tl=ns,_tmp3C0))))))),((_tmp3BF->tl=_tmp32,_tmp3BF))))));}
# 383
if(_tmp32 != 0){
if(_tmp32->tl != 0){
const char*_tmp3C4;void*_tmp3C3[1];struct Cyc_String_pa_PrintArg_struct _tmp3C2;(_tmp3C2.tag=0,((_tmp3C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp3C3[0]=& _tmp3C2,Cyc_Tcutil_terr(loc,((_tmp3C4="%s is ambiguous",_tag_dyneither(_tmp3C4,sizeof(char),16))),_tag_dyneither(_tmp3C3,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp3F=(struct Cyc_List_List*)_tmp32->hd;_npop_handler(0);return _tmp3F;};}
# 388
if(_tmp30 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3C7;struct Cyc_Dict_Absent_exn_struct*_tmp3C6;(int)_throw((void*)((_tmp3C6=_cycalloc_atomic(sizeof(*_tmp3C6)),((_tmp3C6[0]=((_tmp3C7.tag=Cyc_Dict_Absent,_tmp3C7)),_tmp3C6)))));}
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
struct _tuple0*_tmp43=q;union Cyc_Absyn_Nmspace _tmp44;struct _dyneither_ptr*_tmp45;_LL20: _tmp44=_tmp43->f1;_tmp45=_tmp43->f2;_LL21: {
union Cyc_Absyn_Nmspace _tmp46=_tmp44;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp48;struct _dyneither_ptr*_tmp49;struct Cyc_List_List*_tmp4A;switch((_tmp46.C_n).tag){case 4: _LL25: _LL26:
 goto _LL28;case 1: if((_tmp46.Rel_n).val == 0){_LL27: _LL28:
# 408
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp45,0);}else{_LL2D: _tmp49=(struct _dyneither_ptr*)((_tmp46.Rel_n).val)->hd;_tmp4A=((_tmp46.Rel_n).val)->tl;_LL2E: {
# 414
struct Cyc_Dict_Dict _tmp4C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp49,_tmp4A))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp45);}}case 3: _LL29: _tmp48=(_tmp46.C_n).val;_LL2A:
# 409
 _tmp47=_tmp48;goto _LL2C;default: _LL2B: _tmp47=(_tmp46.Abs_n).val;_LL2C: {
# 411
struct Cyc_Dict_Dict _tmp4B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp47))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,_tmp45);}}_LL24:;}_LL1F:;}
# 419
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4D=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4D,v);}
# 423
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp4E=q;union Cyc_Absyn_Nmspace _tmp4F;struct _dyneither_ptr*_tmp50;_LL30: _tmp4F=_tmp4E->f1;_tmp50=_tmp4E->f2;_LL31: {
union Cyc_Absyn_Nmspace _tmp51=_tmp4F;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;struct _dyneither_ptr*_tmp54;struct Cyc_List_List*_tmp55;switch((_tmp51.C_n).tag){case 4: _LL35: _LL36:
 goto _LL38;case 1: if((_tmp51.Rel_n).val == 0){_LL37: _LL38:
# 428
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp50,0);}else{_LL39: _tmp54=(struct _dyneither_ptr*)((_tmp51.Rel_n).val)->hd;_tmp55=((_tmp51.Rel_n).val)->tl;_LL3A: {
# 430
struct Cyc_Dict_Dict _tmp56=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp54,_tmp55))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp56,_tmp50);}}case 3: _LL3B: _tmp53=(_tmp51.C_n).val;_LL3C:
 _tmp52=_tmp53;goto _LL3E;default: _LL3D: _tmp52=(_tmp51.Abs_n).val;_LL3E: {
# 434
struct Cyc_Dict_Dict _tmp57=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp52))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp57,_tmp50);}}_LL34:;}_LL2F:;}
# 439
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 444
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp58=q;union Cyc_Absyn_Nmspace _tmp59;struct _dyneither_ptr*_tmp5A;_LL40: _tmp59=_tmp58->f1;_tmp5A=_tmp58->f2;_LL41: {
union Cyc_Absyn_Nmspace _tmp5B=_tmp59;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct _dyneither_ptr*_tmp5E;struct Cyc_List_List*_tmp5F;switch((_tmp5B.C_n).tag){case 1: if((_tmp5B.Rel_n).val == 0){_LL45: _LL46: {
# 449
struct _handler_cons _tmp60;_push_handler(& _tmp60);{int _tmp62=0;if(setjmp(_tmp60.handler))_tmp62=1;if(!_tmp62){{struct Cyc_Absyn_Datatypedecl***_tmp3C8;struct Cyc_Absyn_Datatypedecl***_tmp64=(_tmp3C8=_region_malloc(r,sizeof(*_tmp3C8)),((_tmp3C8[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp5A,0),_tmp3C8)));_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp61=(void*)_exn_thrown;void*_tmp65=_tmp61;void*_tmp66;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp65)->tag == Cyc_Dict_Absent){_LL50: _LL51:
 return 0;}else{_LL52: _tmp66=_tmp65;_LL53:(int)_rethrow(_tmp66);}_LL4F:;}};}}else{_LL49: _tmp5E=(struct _dyneither_ptr*)((_tmp5B.Rel_n).val)->hd;_tmp5F=((_tmp5B.Rel_n).val)->tl;_LL4A: {
# 459
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp5E,_tmp5F);;_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp6C=_tmp6A;void*_tmp6D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6C)->tag == Cyc_Dict_Absent){_LL55: _LL56:
# 463
{const char*_tmp3CB;void*_tmp3CA;(_tmp3CA=0,Cyc_Tcutil_terr(loc,((_tmp3CB="bad qualified name for @extensible datatype",_tag_dyneither(_tmp3CB,sizeof(char),44))),_tag_dyneither(_tmp3CA,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp3CE;struct Cyc_Dict_Absent_exn_struct*_tmp3CD;(int)_throw((void*)((_tmp3CD=_cycalloc_atomic(sizeof(*_tmp3CD)),((_tmp3CD[0]=((_tmp3CE.tag=Cyc_Dict_Absent,_tmp3CE)),_tmp3CD)))));};}else{_LL57: _tmp6D=_tmp6C;_LL58:(int)_rethrow(_tmp6D);}_LL54:;}};}{
# 466
struct Cyc_Dict_Dict _tmp72=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3CF;return(_tmp3CF=_region_malloc(r,sizeof(*_tmp3CF)),((_tmp3CF[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp72,_tmp5A),_tmp3CF)));};}}case 4: _LL47: _LL48:
# 452
{const char*_tmp3D2;void*_tmp3D1;(_tmp3D1=0,Cyc_Tcutil_terr(loc,((_tmp3D2="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp3D2,sizeof(char),33))),_tag_dyneither(_tmp3D1,sizeof(void*),0)));}
return 0;case 3: _LL4B: _tmp5D=(_tmp5B.C_n).val;_LL4C:
# 468
 _tmp5C=_tmp5D;goto _LL4E;default: _LL4D: _tmp5C=(_tmp5B.Abs_n).val;_LL4E: {
# 470
struct Cyc_Dict_Dict _tmp74=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5C))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3D3;return(_tmp3D3=_region_malloc(r,sizeof(*_tmp3D3)),((_tmp3D3[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp74,_tmp5A),_tmp3D3)));}}_LL44:;}_LL3F:;}
# 475
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp76=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp76,v);}
# 479
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp77=q;union Cyc_Absyn_Nmspace _tmp78;struct _dyneither_ptr*_tmp79;_LL5A: _tmp78=_tmp77->f1;_tmp79=_tmp77->f2;_LL5B: {
union Cyc_Absyn_Nmspace _tmp7A=_tmp78;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7C;struct _dyneither_ptr*_tmp7D;struct Cyc_List_List*_tmp7E;switch((_tmp7A.C_n).tag){case 4: _LL5F: _LL60:
 goto _LL62;case 1: if((_tmp7A.Rel_n).val == 0){_LL61: _LL62:
# 484
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp79,0);}else{_LL63: _tmp7D=(struct _dyneither_ptr*)((_tmp7A.Rel_n).val)->hd;_tmp7E=((_tmp7A.Rel_n).val)->tl;_LL64: {
# 486
struct Cyc_Dict_Dict _tmp7F=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp7D,_tmp7E))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F,_tmp79);}}case 3: _LL65: _tmp7C=(_tmp7A.C_n).val;_LL66:
 _tmp7B=_tmp7C;goto _LL68;default: _LL67: _tmp7B=(_tmp7A.Abs_n).val;_LL68: {
# 490
struct Cyc_Dict_Dict _tmp80=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp7B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp80,_tmp79);}}_LL5E:;}_LL59:;}
# 495
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp81=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,v);}
# 499
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp82=q;union Cyc_Absyn_Nmspace _tmp83;struct _dyneither_ptr*_tmp84;_LL6A: _tmp83=_tmp82->f1;_tmp84=_tmp82->f2;_LL6B: {
union Cyc_Absyn_Nmspace _tmp85=_tmp83;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct _dyneither_ptr*_tmp88;struct Cyc_List_List*_tmp89;switch((_tmp85.C_n).tag){case 4: _LL6F: _LL70:
 goto _LL72;case 1: if((_tmp85.Rel_n).val == 0){_LL71: _LL72:
# 504
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp84,0);}else{_LL73: _tmp88=(struct _dyneither_ptr*)((_tmp85.Rel_n).val)->hd;_tmp89=((_tmp85.Rel_n).val)->tl;_LL74: {
# 506
struct Cyc_Dict_Dict _tmp8A=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp88,_tmp89))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8A,_tmp84);}}case 3: _LL75: _tmp87=(_tmp85.C_n).val;_LL76:
 _tmp86=_tmp87;goto _LL78;default: _LL77: _tmp86=(_tmp85.Abs_n).val;_LL78: {
# 510
struct Cyc_Dict_Dict _tmp8B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp86))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,_tmp84);}}_LL6E:;}_LL69:;}
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
struct Cyc_Tcenv_Fenv*_tmp8E=_tmp8D;struct Cyc_Tcenv_Fenv*_tmp8F=_tmp8E;struct _RegionHandle*_tmp90;_LL7A: _tmp90=_tmp8F->fnrgn;_LL7B:
 return _tmp90;_LL79:;}
# 533
return Cyc_Tcenv_coerce_heap_region();}
# 536
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp3D4;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3D4="put_fenv",_tag_dyneither(_tmp3D4,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp3D5;return(_tmp3D5=_region_malloc(l,sizeof(*_tmp3D5)),((_tmp3D5->ns=te->ns,((_tmp3D5->ae=te->ae,((_tmp3D5->le=fe,((_tmp3D5->allow_valueof=te->allow_valueof,((_tmp3D5->in_extern_c_include=te->in_extern_c_include,_tmp3D5)))))))))));};}
# 541
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp3D6;return(_tmp3D6=_region_malloc(l,sizeof(*_tmp3D6)),((_tmp3D6->ns=te->ns,((_tmp3D6->ae=te->ae,((_tmp3D6->le=0,((_tmp3D6->allow_valueof=te->allow_valueof,((_tmp3D6->in_extern_c_include=te->in_extern_c_include,_tmp3D6)))))))))));}
# 545
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 547
struct Cyc_Tcenv_Fenv _tmp94=*f;struct Cyc_Tcenv_Fenv _tmp95=_tmp94;struct Cyc_Tcenv_SharedFenv*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_RgnOrder_RgnPO*_tmp98;const struct Cyc_Tcenv_Bindings*_tmp99;struct Cyc_Absyn_Stmt*_tmp9A;struct Cyc_Tcenv_CtrlEnv*_tmp9B;void*_tmp9C;void*_tmp9D;struct Cyc_Tcenv_FenvFlags _tmp9E;struct _RegionHandle*_tmp9F;_LL7F: _tmp96=_tmp95.shared;_tmp97=_tmp95.type_vars;_tmp98=_tmp95.region_order;_tmp99=_tmp95.locals;_tmp9A=_tmp95.encloser;_tmp9B=_tmp95.ctrl_env;_tmp9C=_tmp95.capability;_tmp9D=_tmp95.curr_rgn;_tmp9E=_tmp95.flags;_tmp9F=_tmp95.fnrgn;_LL80: {
# 549
struct Cyc_Tcenv_Fenv*_tmp3D7;return(_tmp3D7=_region_malloc(l,sizeof(*_tmp3D7)),((_tmp3D7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp96,((_tmp3D7->type_vars=(struct Cyc_List_List*)_tmp97,((_tmp3D7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp98,((_tmp3D7->locals=(const struct Cyc_Tcenv_Bindings*)_tmp99,((_tmp3D7->encloser=(struct Cyc_Absyn_Stmt*)_tmp9A,((_tmp3D7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp9B,((_tmp3D7->capability=(void*)_tmp9C,((_tmp3D7->curr_rgn=(void*)_tmp9D,((_tmp3D7->flags=(struct Cyc_Tcenv_FenvFlags)_tmp9E,((_tmp3D7->fnrgn=(struct _RegionHandle*)l,_tmp3D7)))))))))))))))))))));}_LL7E:;}
# 553
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 555
struct Cyc_Tcenv_Fenv _tmpA1=*f;struct Cyc_Tcenv_Fenv _tmpA2=_tmpA1;struct Cyc_Tcenv_SharedFenv*_tmpA3;struct Cyc_List_List*_tmpA4;struct Cyc_RgnOrder_RgnPO*_tmpA5;const struct Cyc_Tcenv_Bindings*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA7;struct Cyc_Tcenv_CtrlEnv*_tmpA8;void*_tmpA9;void*_tmpAA;struct Cyc_Tcenv_FenvFlags _tmpAB;_LL84: _tmpA3=_tmpA2.shared;_tmpA4=_tmpA2.type_vars;_tmpA5=_tmpA2.region_order;_tmpA6=_tmpA2.locals;_tmpA7=_tmpA2.encloser;_tmpA8=_tmpA2.ctrl_env;_tmpA9=_tmpA2.capability;_tmpAA=_tmpA2.curr_rgn;_tmpAB=_tmpA2.flags;_LL85: {
# 558
struct Cyc_Tcenv_CtrlEnv _tmpAC=*_tmpA8;struct Cyc_Tcenv_CtrlEnv _tmpAD=_tmpAC;struct _RegionHandle*_tmpAE;void*_tmpAF;void*_tmpB0;const struct _tuple10*_tmpB1;void*_tmpB2;int _tmpB3;_LL89: _tmpAE=_tmpAD.ctrl_rgn;_tmpAF=_tmpAD.continue_stmt;_tmpB0=_tmpAD.break_stmt;_tmpB1=_tmpAD.fallthru_clause;_tmpB2=_tmpAD.next_stmt;_tmpB3=_tmpAD.try_depth;_LL8A: {
struct Cyc_Tcenv_CtrlEnv*_tmp3D8;struct Cyc_Tcenv_CtrlEnv*_tmpB4=
(_tmp3D8=_region_malloc(l,sizeof(*_tmp3D8)),((_tmp3D8->ctrl_rgn=_tmpAE,((_tmp3D8->continue_stmt=_tmpAF,((_tmp3D8->break_stmt=_tmpB0,((_tmp3D8->fallthru_clause=_tmpB1,((_tmp3D8->next_stmt=_tmpB2,((_tmp3D8->try_depth=_tmpB3,_tmp3D8)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp3D9;return(_tmp3D9=_region_malloc(l,sizeof(*_tmp3D9)),((_tmp3D9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA3,((_tmp3D9->type_vars=(struct Cyc_List_List*)_tmpA4,((_tmp3D9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA5,((_tmp3D9->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA6,((_tmp3D9->encloser=(struct Cyc_Absyn_Stmt*)_tmpA7,((_tmp3D9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB4,((_tmp3D9->capability=(void*)_tmpA9,((_tmp3D9->curr_rgn=(void*)_tmpAA,((_tmp3D9->flags=(struct Cyc_Tcenv_FenvFlags)_tmpAB,((_tmp3D9->fnrgn=(struct _RegionHandle*)l,_tmp3D9)))))))))))))))))))));}_LL88:;}_LL83:;}
# 567
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3DA;struct Cyc_Tcenv_Fenv*_tmpB7=Cyc_Tcenv_get_fenv(te,((_tmp3DA="return_typ",_tag_dyneither(_tmp3DA,sizeof(char),11))));struct Cyc_Tcenv_Fenv*_tmpB8=_tmpB7;struct Cyc_Tcenv_SharedFenv*_tmpB9;_LL8E: _tmpB9=_tmpB8->shared;_LL8F: {
struct Cyc_Tcenv_SharedFenv _tmpBA=*_tmpB9;struct Cyc_Tcenv_SharedFenv _tmpBB=_tmpBA;void*_tmpBC;_LL93: _tmpBC=_tmpBB.return_typ;_LL94:
 return _tmpBC;_LL92:;}_LL8D:;}
# 573
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpBE=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpBF=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpBE));struct Cyc_Tcenv_Fenv _tmpC0=_tmpBF;struct Cyc_List_List*_tmpC1;_LL98: _tmpC1=_tmpC0.type_vars;_LL99:
 return _tmpC1;_LL97:;};}
# 580
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp3DB;return(_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3DB)));}
# 584
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 586
const char*_tmp3DC;struct Cyc_Tcenv_Fenv*_tmpC3=
# 588
Cyc_Tcenv_get_fenv(te,((_tmp3DC="add_type_vars",_tag_dyneither(_tmp3DC,sizeof(char),14))));
# 586
struct Cyc_Tcenv_Fenv*_tmpC4=_tmpC3;struct Cyc_Tcenv_SharedFenv*_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_RgnOrder_RgnPO*_tmpC7;const struct Cyc_Tcenv_Bindings*_tmpC8;struct Cyc_Absyn_Stmt*_tmpC9;struct Cyc_Tcenv_CtrlEnv*_tmpCA;void*_tmpCB;void*_tmpCC;struct Cyc_Tcenv_FenvFlags _tmpCD;_LL9D: _tmpC5=_tmpC4->shared;_tmpC6=_tmpC4->type_vars;_tmpC7=_tmpC4->region_order;_tmpC8=_tmpC4->locals;_tmpC9=_tmpC4->encloser;_tmpCA=_tmpC4->ctrl_env;_tmpCB=_tmpC4->capability;_tmpCC=_tmpC4->curr_rgn;_tmpCD=_tmpC4->flags;_LL9E:
# 590
 Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpCE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC6);
Cyc_Tcutil_check_unique_tvars(loc,_tmpCE);{
struct Cyc_Tcenv_Fenv*_tmp3DD;struct Cyc_Tcenv_Fenv*_tmpCF=(_tmp3DD=_region_malloc(r,sizeof(*_tmp3DD)),((_tmp3DD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC5,((_tmp3DD->type_vars=(struct Cyc_List_List*)_tmpCE,((_tmp3DD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC7,((_tmp3DD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC8,((_tmp3DD->encloser=(struct Cyc_Absyn_Stmt*)_tmpC9,((_tmp3DD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpCA,((_tmp3DD->capability=(void*)_tmpCB,((_tmp3DD->curr_rgn=(void*)_tmpCC,((_tmp3DD->flags=(struct Cyc_Tcenv_FenvFlags)_tmpCD,((_tmp3DD->fnrgn=(struct _RegionHandle*)r,_tmp3DD)))))))))))))))))))));
# 595
return Cyc_Tcenv_put_fenv(r,te,_tmpCF);};};_LL9C:;}
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
{union Cyc_Absyn_Nmspace _tmpD4=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpD5=_tmpD4;if((_tmpD5.Loc_n).tag == 4){_LLA2: _LLA3:
 goto _LLA1;}else{_LLA4: _LLA5: {
# 615
struct Cyc_Core_Impossible_exn_struct _tmp3E3;const char*_tmp3E2;struct Cyc_Core_Impossible_exn_struct*_tmp3E1;(int)_throw((void*)((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E3.tag=Cyc_Core_Impossible,((_tmp3E3.f1=((_tmp3E2="add_local_var: called with Rel_n",_tag_dyneither(_tmp3E2,sizeof(char),33))),_tmp3E3)))),_tmp3E1)))));}}_LLA1:;}
# 617
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpD9;_push_handler(& _tmpD9);{int _tmpDB=0;if(setjmp(_tmpD9.handler))_tmpDB=1;if(!_tmpDB){
{void*_tmpDC=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpDD=_tmpDC;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD)->f1)){case 4: _LLA7: _LLA8:
# 621
{const char*_tmp3E6;void*_tmp3E5;(_tmp3E5=0,Cyc_Tcutil_warn(loc,((_tmp3E6="declaration hides local",_tag_dyneither(_tmp3E6,sizeof(char),24))),_tag_dyneither(_tmp3E5,sizeof(void*),0)));}goto _LLA6;case 1: _LLA9: _LLAA:
{const char*_tmp3E9;void*_tmp3E8;(_tmp3E8=0,Cyc_Tcutil_warn(loc,((_tmp3E9="declaration hides global",_tag_dyneither(_tmp3E9,sizeof(char),25))),_tag_dyneither(_tmp3E8,sizeof(void*),0)));}goto _LLA6;case 5: _LLAB: _LLAC:
{const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,Cyc_Tcutil_warn(loc,((_tmp3EC="declaration hides pattern variable",_tag_dyneither(_tmp3EC,sizeof(char),35))),_tag_dyneither(_tmp3EB,sizeof(void*),0)));}goto _LLA6;case 3: _LLAD: _LLAE:
{const char*_tmp3EF;void*_tmp3EE;(_tmp3EE=0,Cyc_Tcutil_warn(loc,((_tmp3EF="declaration hides parameter",_tag_dyneither(_tmp3EF,sizeof(char),28))),_tag_dyneither(_tmp3EE,sizeof(void*),0)));}goto _LLA6;default: goto _LLAF;}else{_LLAF: _LLB0:
 goto _LLA6;}_LLA6:;}
# 619
;_pop_handler();}else{void*_tmpDA=(void*)_exn_thrown;void*_tmpE6=_tmpDA;void*_tmpE7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpE6)->tag == Cyc_Dict_Absent){_LLB2: _LLB3:
# 627
 goto _LLB1;}else{_LLB4: _tmpE7=_tmpE6;_LLB5:(int)_rethrow(_tmpE7);}_LLB1:;}};}{
const char*_tmp3F0;struct Cyc_Tcenv_Fenv*_tmpE8=
# 630
Cyc_Tcenv_get_fenv(te,((_tmp3F0="add_local_var",_tag_dyneither(_tmp3F0,sizeof(char),14))));
# 628
struct Cyc_Tcenv_Fenv*_tmpE9=_tmpE8;struct Cyc_Tcenv_SharedFenv*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_RgnOrder_RgnPO*_tmpEC;const struct Cyc_Tcenv_Bindings*_tmpED;struct Cyc_Absyn_Stmt*_tmpEE;struct Cyc_Tcenv_CtrlEnv*_tmpEF;void*_tmpF0;void*_tmpF1;struct Cyc_Tcenv_FenvFlags _tmpF2;_LLB7: _tmpEA=_tmpE9->shared;_tmpEB=_tmpE9->type_vars;_tmpEC=_tmpE9->region_order;_tmpED=_tmpE9->locals;_tmpEE=_tmpE9->encloser;_tmpEF=_tmpE9->ctrl_env;_tmpF0=_tmpE9->capability;_tmpF1=_tmpE9->curr_rgn;_tmpF2=_tmpE9->flags;_LLB8: {
# 631
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp3F3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF3=(_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2[0]=((_tmp3F3.tag=4,((_tmp3F3.f1=vd,_tmp3F3)))),_tmp3F2)));
struct Cyc_Tcenv_Bindings*_tmp3F4;struct Cyc_Tcenv_Bindings*_tmpF4=(_tmp3F4=_region_malloc(r,sizeof(*_tmp3F4)),((_tmp3F4->v=_tmpD3,((_tmp3F4->b=(void*)_tmpF3,((_tmp3F4->tl=_tmpED,_tmp3F4)))))));
struct Cyc_Tcenv_Fenv*_tmp3F5;struct Cyc_Tcenv_Fenv*_tmpF5=(_tmp3F5=_region_malloc(r,sizeof(*_tmp3F5)),((_tmp3F5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpEA,((_tmp3F5->type_vars=(struct Cyc_List_List*)_tmpEB,((_tmp3F5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEC,((_tmp3F5->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpF4),((_tmp3F5->encloser=(struct Cyc_Absyn_Stmt*)_tmpEE,((_tmp3F5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpEF,((_tmp3F5->capability=(void*)_tmpF0,((_tmp3F5->curr_rgn=(void*)_tmpF1,((_tmp3F5->flags=(struct Cyc_Tcenv_FenvFlags)_tmpF2,((_tmp3F5->fnrgn=(struct _RegionHandle*)r,_tmp3F5)))))))))))))))))))));
# 635
return Cyc_Tcenv_put_fenv(r,te,_tmpF5);}_LLB6:;};}
# 638
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 640
struct Cyc_Tcenv_Fenv*_tmpFB=te->le;
if(_tmpFB == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpFC=*_tmpFB;struct Cyc_Tcenv_Fenv _tmpFD=_tmpFC;struct Cyc_Tcenv_SharedFenv*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_RgnOrder_RgnPO*_tmp100;const struct Cyc_Tcenv_Bindings*_tmp101;struct Cyc_Absyn_Stmt*_tmp102;struct Cyc_Tcenv_CtrlEnv*_tmp103;void*_tmp104;void*_tmp105;int _tmp106;int _tmp107;int _tmp108;_LLBC: _tmpFE=_tmpFD.shared;_tmpFF=_tmpFD.type_vars;_tmp100=_tmpFD.region_order;_tmp101=_tmpFD.locals;_tmp102=_tmpFD.encloser;_tmp103=_tmpFD.ctrl_env;_tmp104=_tmpFD.capability;_tmp105=_tmpFD.curr_rgn;_tmp106=(_tmpFD.flags).in_notreadctxt;_tmp107=(_tmpFD.flags).in_lhs;_tmp108=(_tmpFD.flags).abstract_ok;_LLBD: {
# 645
struct Cyc_Tcenv_Fenv*_tmp3F6;struct Cyc_Tcenv_Fenv*_tmp109=(_tmp3F6=_region_malloc(r,sizeof(*_tmp3F6)),((_tmp3F6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFE,((_tmp3F6->type_vars=(struct Cyc_List_List*)_tmpFF,((_tmp3F6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp100,((_tmp3F6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp101,((_tmp3F6->encloser=(struct Cyc_Absyn_Stmt*)_tmp102,((_tmp3F6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp103,((_tmp3F6->capability=(void*)_tmp104,((_tmp3F6->curr_rgn=(void*)_tmp105,((_tmp3F6->flags=(struct Cyc_Tcenv_FenvFlags)(
# 647
((_tmp3F6->flags).in_notreadctxt=_tmp106,(((_tmp3F6->flags).in_lhs=_tmp107,(((_tmp3F6->flags).in_new=(int)status,(((_tmp3F6->flags).abstract_ok=_tmp108,_tmp3F6->flags)))))))),((_tmp3F6->fnrgn=(struct _RegionHandle*)r,_tmp3F6)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp109);}_LLBB:;};}
# 651
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10B=te->le;
if(_tmp10B == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp10C=_tmp10B;struct Cyc_Tcenv_Fenv*_tmp10D=_tmp10C;int _tmp10E;_LLC1: _tmp10E=(_tmp10D->flags).in_new;_LLC2:
 return(enum Cyc_Tcenv_NewStatus)_tmp10E;_LLC0:;};}
# 658
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp110=_tmp10F;struct Cyc_Tcenv_Fenv*_tmp111=_tmp110;int _tmp112;_LLC6: _tmp112=(_tmp111->flags).abstract_ok;_LLC7:
 return _tmp112;_LLC5:;};}
# 665
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 667
struct Cyc_Tcenv_Fenv*_tmp113=te->le;
if(_tmp113 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp114=*_tmp113;struct Cyc_Tcenv_Fenv _tmp115=_tmp114;struct Cyc_Tcenv_SharedFenv*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_RgnOrder_RgnPO*_tmp118;const struct Cyc_Tcenv_Bindings*_tmp119;struct Cyc_Absyn_Stmt*_tmp11A;struct Cyc_Tcenv_CtrlEnv*_tmp11B;void*_tmp11C;void*_tmp11D;int _tmp11E;int _tmp11F;int _tmp120;_LLCB: _tmp116=_tmp115.shared;_tmp117=_tmp115.type_vars;_tmp118=_tmp115.region_order;_tmp119=_tmp115.locals;_tmp11A=_tmp115.encloser;_tmp11B=_tmp115.ctrl_env;_tmp11C=_tmp115.capability;_tmp11D=_tmp115.curr_rgn;_tmp11E=(_tmp115.flags).in_notreadctxt;_tmp11F=(_tmp115.flags).in_lhs;_tmp120=(_tmp115.flags).in_new;_LLCC: {
# 672
struct Cyc_Tcenv_Fenv*_tmp3F7;struct Cyc_Tcenv_Fenv*_tmp121=(_tmp3F7=_region_malloc(r,sizeof(*_tmp3F7)),((_tmp3F7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp116,((_tmp3F7->type_vars=(struct Cyc_List_List*)_tmp117,((_tmp3F7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp118,((_tmp3F7->locals=(const struct Cyc_Tcenv_Bindings*)_tmp119,((_tmp3F7->encloser=(struct Cyc_Absyn_Stmt*)_tmp11A,((_tmp3F7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp11B,((_tmp3F7->capability=(void*)_tmp11C,((_tmp3F7->curr_rgn=(void*)_tmp11D,((_tmp3F7->flags=(struct Cyc_Tcenv_FenvFlags)(
# 674
((_tmp3F7->flags).in_notreadctxt=_tmp11E,(((_tmp3F7->flags).in_lhs=_tmp11F,(((_tmp3F7->flags).in_new=_tmp120,(((_tmp3F7->flags).abstract_ok=1,_tmp3F7->flags)))))))),((_tmp3F7->fnrgn=(struct _RegionHandle*)r,_tmp3F7)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp121);}_LLCA:;};}
# 678
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 680
struct Cyc_Tcenv_Fenv*_tmp123=te->le;
if(_tmp123 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp124=*_tmp123;struct Cyc_Tcenv_Fenv _tmp125=_tmp124;struct Cyc_Tcenv_SharedFenv*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_RgnOrder_RgnPO*_tmp128;const struct Cyc_Tcenv_Bindings*_tmp129;struct Cyc_Absyn_Stmt*_tmp12A;struct Cyc_Tcenv_CtrlEnv*_tmp12B;void*_tmp12C;void*_tmp12D;int _tmp12E;int _tmp12F;int _tmp130;_LLD0: _tmp126=_tmp125.shared;_tmp127=_tmp125.type_vars;_tmp128=_tmp125.region_order;_tmp129=_tmp125.locals;_tmp12A=_tmp125.encloser;_tmp12B=_tmp125.ctrl_env;_tmp12C=_tmp125.capability;_tmp12D=_tmp125.curr_rgn;_tmp12E=(_tmp125.flags).in_notreadctxt;_tmp12F=(_tmp125.flags).in_lhs;_tmp130=(_tmp125.flags).in_new;_LLD1: {
# 685
struct Cyc_Tcenv_Fenv*_tmp3F8;struct Cyc_Tcenv_Fenv*_tmp131=(_tmp3F8=_region_malloc(r,sizeof(*_tmp3F8)),((_tmp3F8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp126,((_tmp3F8->type_vars=(struct Cyc_List_List*)_tmp127,((_tmp3F8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp128,((_tmp3F8->locals=(const struct Cyc_Tcenv_Bindings*)_tmp129,((_tmp3F8->encloser=(struct Cyc_Absyn_Stmt*)_tmp12A,((_tmp3F8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12B,((_tmp3F8->capability=(void*)_tmp12C,((_tmp3F8->curr_rgn=(void*)_tmp12D,((_tmp3F8->flags=(struct Cyc_Tcenv_FenvFlags)(
# 687
((_tmp3F8->flags).in_notreadctxt=_tmp12E,(((_tmp3F8->flags).in_lhs=_tmp12F,(((_tmp3F8->flags).in_new=_tmp130,(((_tmp3F8->flags).abstract_ok=0,_tmp3F8->flags)))))))),((_tmp3F8->fnrgn=(struct _RegionHandle*)r,_tmp3F8)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp131);}_LLCF:;};}
# 691
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp133=te->le;
if(_tmp133 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp134=*_tmp133;struct Cyc_Tcenv_Fenv _tmp135=_tmp134;struct Cyc_Tcenv_SharedFenv*_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_RgnOrder_RgnPO*_tmp138;const struct Cyc_Tcenv_Bindings*_tmp139;struct Cyc_Absyn_Stmt*_tmp13A;struct Cyc_Tcenv_CtrlEnv*_tmp13B;void*_tmp13C;void*_tmp13D;int _tmp13E;int _tmp13F;int _tmp140;_LLD5: _tmp136=_tmp135.shared;_tmp137=_tmp135.type_vars;_tmp138=_tmp135.region_order;_tmp139=_tmp135.locals;_tmp13A=_tmp135.encloser;_tmp13B=_tmp135.ctrl_env;_tmp13C=_tmp135.capability;_tmp13D=_tmp135.curr_rgn;_tmp13E=(_tmp135.flags).in_lhs;_tmp13F=(_tmp135.flags).in_new;_tmp140=(_tmp135.flags).abstract_ok;_LLD6: {
# 697
struct Cyc_Tcenv_Fenv*_tmp3F9;struct Cyc_Tcenv_Fenv*_tmp141=(_tmp3F9=_region_malloc(r,sizeof(*_tmp3F9)),((_tmp3F9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp136,((_tmp3F9->type_vars=(struct Cyc_List_List*)_tmp137,((_tmp3F9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp138,((_tmp3F9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp139,((_tmp3F9->encloser=(struct Cyc_Absyn_Stmt*)_tmp13A,((_tmp3F9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13B,((_tmp3F9->capability=(void*)_tmp13C,((_tmp3F9->curr_rgn=(void*)_tmp13D,((_tmp3F9->flags=(struct Cyc_Tcenv_FenvFlags)(
# 699
((_tmp3F9->flags).in_notreadctxt=1,(((_tmp3F9->flags).in_lhs=_tmp13E,(((_tmp3F9->flags).in_new=_tmp13F,(((_tmp3F9->flags).abstract_ok=_tmp140,_tmp3F9->flags)))))))),((_tmp3F9->fnrgn=(struct _RegionHandle*)r,_tmp3F9)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp141);}_LLD4:;};}
# 703
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp143=te->le;
if(_tmp143 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp144=*_tmp143;struct Cyc_Tcenv_Fenv _tmp145=_tmp144;struct Cyc_Tcenv_SharedFenv*_tmp146;struct Cyc_List_List*_tmp147;struct Cyc_RgnOrder_RgnPO*_tmp148;const struct Cyc_Tcenv_Bindings*_tmp149;struct Cyc_Absyn_Stmt*_tmp14A;struct Cyc_Tcenv_CtrlEnv*_tmp14B;void*_tmp14C;void*_tmp14D;int _tmp14E;int _tmp14F;int _tmp150;_LLDA: _tmp146=_tmp145.shared;_tmp147=_tmp145.type_vars;_tmp148=_tmp145.region_order;_tmp149=_tmp145.locals;_tmp14A=_tmp145.encloser;_tmp14B=_tmp145.ctrl_env;_tmp14C=_tmp145.capability;_tmp14D=_tmp145.curr_rgn;_tmp14E=(_tmp145.flags).in_lhs;_tmp14F=(_tmp145.flags).in_new;_tmp150=(_tmp145.flags).abstract_ok;_LLDB: {
# 709
struct Cyc_Tcenv_Fenv*_tmp3FA;struct Cyc_Tcenv_Fenv*_tmp151=(_tmp3FA=_region_malloc(r,sizeof(*_tmp3FA)),((_tmp3FA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp146,((_tmp3FA->type_vars=(struct Cyc_List_List*)_tmp147,((_tmp3FA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp148,((_tmp3FA->locals=(const struct Cyc_Tcenv_Bindings*)_tmp149,((_tmp3FA->encloser=(struct Cyc_Absyn_Stmt*)_tmp14A,((_tmp3FA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14B,((_tmp3FA->capability=(void*)_tmp14C,((_tmp3FA->curr_rgn=(void*)_tmp14D,((_tmp3FA->flags=(struct Cyc_Tcenv_FenvFlags)(
# 711
((_tmp3FA->flags).in_notreadctxt=0,(((_tmp3FA->flags).in_lhs=_tmp14E,(((_tmp3FA->flags).in_new=_tmp14F,(((_tmp3FA->flags).abstract_ok=_tmp150,_tmp3FA->flags)))))))),((_tmp3FA->fnrgn=(struct _RegionHandle*)r,_tmp3FA)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp151);}_LLD9:;};}
# 715
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp153=te->le;
if(_tmp153 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp154=_tmp153;struct Cyc_Tcenv_Fenv*_tmp155=_tmp154;int _tmp156;_LLDF: _tmp156=(_tmp155->flags).in_notreadctxt;_LLE0:
 return _tmp156;_LLDE:;};}
# 722
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp157=te->le;
if(_tmp157 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp158=*_tmp157;struct Cyc_Tcenv_Fenv _tmp159=_tmp158;struct Cyc_Tcenv_SharedFenv*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_RgnOrder_RgnPO*_tmp15C;const struct Cyc_Tcenv_Bindings*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Tcenv_CtrlEnv*_tmp15F;void*_tmp160;void*_tmp161;int _tmp162;int _tmp163;int _tmp164;_LLE4: _tmp15A=_tmp159.shared;_tmp15B=_tmp159.type_vars;_tmp15C=_tmp159.region_order;_tmp15D=_tmp159.locals;_tmp15E=_tmp159.encloser;_tmp15F=_tmp159.ctrl_env;_tmp160=_tmp159.capability;_tmp161=_tmp159.curr_rgn;_tmp162=(_tmp159.flags).in_notreadctxt;_tmp163=(_tmp159.flags).in_new;_tmp164=(_tmp159.flags).abstract_ok;_LLE5: {
# 728
struct Cyc_Tcenv_Fenv*_tmp3FB;struct Cyc_Tcenv_Fenv*_tmp165=(_tmp3FB=_region_malloc(r,sizeof(*_tmp3FB)),((_tmp3FB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15A,((_tmp3FB->type_vars=(struct Cyc_List_List*)_tmp15B,((_tmp3FB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C,((_tmp3FB->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15D,((_tmp3FB->encloser=(struct Cyc_Absyn_Stmt*)_tmp15E,((_tmp3FB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F,((_tmp3FB->capability=(void*)_tmp160,((_tmp3FB->curr_rgn=(void*)_tmp161,((_tmp3FB->flags=(struct Cyc_Tcenv_FenvFlags)(
# 730
((_tmp3FB->flags).in_notreadctxt=_tmp162,(((_tmp3FB->flags).in_lhs=1,(((_tmp3FB->flags).in_new=_tmp163,(((_tmp3FB->flags).abstract_ok=_tmp164,_tmp3FB->flags)))))))),((_tmp3FB->fnrgn=(struct _RegionHandle*)r,_tmp3FB)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp165);}_LLE3:;};}
# 734
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp167=te->le;
if(_tmp167 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp168=*_tmp167;struct Cyc_Tcenv_Fenv _tmp169=_tmp168;struct Cyc_Tcenv_SharedFenv*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_RgnOrder_RgnPO*_tmp16C;const struct Cyc_Tcenv_Bindings*_tmp16D;struct Cyc_Absyn_Stmt*_tmp16E;struct Cyc_Tcenv_CtrlEnv*_tmp16F;void*_tmp170;void*_tmp171;int _tmp172;int _tmp173;int _tmp174;_LLE9: _tmp16A=_tmp169.shared;_tmp16B=_tmp169.type_vars;_tmp16C=_tmp169.region_order;_tmp16D=_tmp169.locals;_tmp16E=_tmp169.encloser;_tmp16F=_tmp169.ctrl_env;_tmp170=_tmp169.capability;_tmp171=_tmp169.curr_rgn;_tmp172=(_tmp169.flags).in_notreadctxt;_tmp173=(_tmp169.flags).in_new;_tmp174=(_tmp169.flags).abstract_ok;_LLEA: {
# 740
struct Cyc_Tcenv_Fenv*_tmp3FC;struct Cyc_Tcenv_Fenv*_tmp175=(_tmp3FC=_region_malloc(r,sizeof(*_tmp3FC)),((_tmp3FC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp16A,((_tmp3FC->type_vars=(struct Cyc_List_List*)_tmp16B,((_tmp3FC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp16C,((_tmp3FC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp16D,((_tmp3FC->encloser=(struct Cyc_Absyn_Stmt*)_tmp16E,((_tmp3FC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16F,((_tmp3FC->capability=(void*)_tmp170,((_tmp3FC->curr_rgn=(void*)_tmp171,((_tmp3FC->flags=(struct Cyc_Tcenv_FenvFlags)(
# 742
((_tmp3FC->flags).in_notreadctxt=_tmp172,(((_tmp3FC->flags).in_lhs=0,(((_tmp3FC->flags).in_new=_tmp173,(((_tmp3FC->flags).abstract_ok=_tmp174,_tmp3FC->flags)))))))),((_tmp3FC->fnrgn=(struct _RegionHandle*)r,_tmp3FC)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp175);}_LLE8:;};}
# 746
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp177=te->le;
if(_tmp177 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp178=_tmp177;struct Cyc_Tcenv_Fenv*_tmp179=_tmp178;int _tmp17A;_LLEE: _tmp17A=(_tmp179->flags).in_lhs;_LLEF:
 return _tmp17A;_LLED:;};}
# 754
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 756
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp17B;_push_handler(& _tmp17B);{int _tmp17D=0;if(setjmp(_tmp17B.handler))_tmp17D=1;if(!_tmp17D){
{void*_tmp17E=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp17F=_tmp17E;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17F)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17F)->f1)){case 4: _LLF3: _LLF4:
# 760
{const char*_tmp3FF;void*_tmp3FE;(_tmp3FE=0,Cyc_Tcutil_warn(loc,((_tmp3FF="pattern variable hides local",_tag_dyneither(_tmp3FF,sizeof(char),29))),_tag_dyneither(_tmp3FE,sizeof(void*),0)));}goto _LLF2;case 1: _LLF5: _LLF6:
{const char*_tmp402;void*_tmp401;(_tmp401=0,Cyc_Tcutil_warn(loc,((_tmp402="pattern variable hides global",_tag_dyneither(_tmp402,sizeof(char),30))),_tag_dyneither(_tmp401,sizeof(void*),0)));}goto _LLF2;case 5: _LLF7: _LLF8:
{const char*_tmp405;void*_tmp404;(_tmp404=0,Cyc_Tcutil_warn(loc,((_tmp405="pattern variable hides pattern variable",_tag_dyneither(_tmp405,sizeof(char),40))),_tag_dyneither(_tmp404,sizeof(void*),0)));}goto _LLF2;case 3: _LLF9: _LLFA:
{const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_Tcutil_warn(loc,((_tmp408="pattern variable hides parameter",_tag_dyneither(_tmp408,sizeof(char),33))),_tag_dyneither(_tmp407,sizeof(void*),0)));}goto _LLF2;default: goto _LLFB;}else{_LLFB: _LLFC:
 goto _LLF2;}_LLF2:;}
# 758
;_pop_handler();}else{void*_tmp17C=(void*)_exn_thrown;void*_tmp188=_tmp17C;void*_tmp189;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp188)->tag == Cyc_Dict_Absent){_LLFE: _LLFF:
# 766
 goto _LLFD;}else{_LL100: _tmp189=_tmp188;_LL101:(int)_rethrow(_tmp189);}_LLFD:;}};}{
struct _dyneither_ptr*_tmp18A=(*vd->name).f2;
const char*_tmp409;struct Cyc_Tcenv_Fenv*_tmp18B=
# 770
Cyc_Tcenv_get_fenv(te,((_tmp409="add_pat_var",_tag_dyneither(_tmp409,sizeof(char),12))));
# 768
struct Cyc_Tcenv_Fenv*_tmp18C=_tmp18B;struct Cyc_Tcenv_SharedFenv*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_RgnOrder_RgnPO*_tmp18F;const struct Cyc_Tcenv_Bindings*_tmp190;struct Cyc_Absyn_Stmt*_tmp191;struct Cyc_Tcenv_CtrlEnv*_tmp192;void*_tmp193;void*_tmp194;struct Cyc_Tcenv_FenvFlags _tmp195;_LL103: _tmp18D=_tmp18C->shared;_tmp18E=_tmp18C->type_vars;_tmp18F=_tmp18C->region_order;_tmp190=_tmp18C->locals;_tmp191=_tmp18C->encloser;_tmp192=_tmp18C->ctrl_env;_tmp193=_tmp18C->capability;_tmp194=_tmp18C->curr_rgn;_tmp195=_tmp18C->flags;_LL104: {
# 771
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp40C;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp40B;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp196=(_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B[0]=((_tmp40C.tag=5,((_tmp40C.f1=vd,_tmp40C)))),_tmp40B)));
struct Cyc_Tcenv_Bindings*_tmp40D;struct Cyc_Tcenv_Bindings*_tmp197=(_tmp40D=_region_malloc(r,sizeof(*_tmp40D)),((_tmp40D->v=_tmp18A,((_tmp40D->b=(void*)_tmp196,((_tmp40D->tl=_tmp190,_tmp40D)))))));
struct Cyc_Tcenv_Fenv*_tmp40E;struct Cyc_Tcenv_Fenv*_tmp198=(_tmp40E=_region_malloc(r,sizeof(*_tmp40E)),((_tmp40E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp18D,((_tmp40E->type_vars=(struct Cyc_List_List*)_tmp18E,((_tmp40E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp18F,((_tmp40E->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp197),((_tmp40E->encloser=(struct Cyc_Absyn_Stmt*)_tmp191,((_tmp40E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp192,((_tmp40E->capability=(void*)_tmp193,((_tmp40E->curr_rgn=(void*)_tmp194,((_tmp40E->flags=(struct Cyc_Tcenv_FenvFlags)_tmp195,((_tmp40E->fnrgn=(struct _RegionHandle*)r,_tmp40E)))))))))))))))))))));
# 775
return Cyc_Tcenv_put_fenv(r,te,_tmp198);}_LL102:;};}
# 778
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp19E=te->le;
struct _tuple0*_tmp19F=q;union Cyc_Absyn_Nmspace _tmp1A0;struct _dyneither_ptr*_tmp1A1;_LL108: _tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;_LL109: {
union Cyc_Absyn_Nmspace _tmp1A2=_tmp1A0;switch((_tmp1A2.Rel_n).tag){case 4: _LL10D: _LL10E:
# 783
 if(_tmp19E == 0){struct Cyc_Dict_Absent_exn_struct _tmp411;struct Cyc_Dict_Absent_exn_struct*_tmp410;(int)_throw((void*)((_tmp410=_cycalloc_atomic(sizeof(*_tmp410)),((_tmp410[0]=((_tmp411.tag=Cyc_Dict_Absent,_tmp411)),_tmp410)))));}
goto _LL110;case 1: if((_tmp1A2.Rel_n).val == 0){_LL10F: if(_tmp19E != 0){_LL110: {
# 786
struct Cyc_Tcenv_Fenv*_tmp1A5=_tmp19E;struct Cyc_Tcenv_Fenv*_tmp1A6=_tmp1A5;const struct Cyc_Tcenv_Bindings*_tmp1A7;_LL114: _tmp1A7=_tmp1A6->locals;_LL115: {
# 788
struct _handler_cons _tmp1A8;_push_handler(& _tmp1A8);{int _tmp1AA=0;if(setjmp(_tmp1A8.handler))_tmp1AA=1;if(!_tmp1AA){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp414;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp413;void*_tmp1AD=(void*)((_tmp413=_region_malloc(r,sizeof(*_tmp413)),((_tmp413[0]=((_tmp414.tag=0,((_tmp414.f1=Cyc_Tcenv_lookup_binding(_tmp1A7,_tmp1A1),_tmp414)))),_tmp413))));_npop_handler(0);return _tmp1AD;};_pop_handler();}else{void*_tmp1A9=(void*)_exn_thrown;void*_tmp1AE=_tmp1A9;void*_tmp1AF;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1AE)->tag == Cyc_Tcenv_NoBinding){_LL119: _LL11A:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LL11B: _tmp1AF=_tmp1AE;_LL11C:(int)_rethrow(_tmp1AF);}_LL118:;}};}_LL113:;}}else{goto _LL111;}}else{goto _LL111;}default: _LL111: _LL112: {
# 792
struct _handler_cons _tmp1B0;_push_handler(& _tmp1B0);{int _tmp1B2=0;if(setjmp(_tmp1B0.handler))_tmp1B2=1;if(!_tmp1B2){{void*_tmp1B3=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1B3;};_pop_handler();}else{void*_tmp1B1=(void*)_exn_thrown;void*_tmp1B4=_tmp1B1;void*_tmp1B5;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B4)->tag == Cyc_Dict_Absent){_LL11E: _LL11F: {
# 796
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp417;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp416;return(void*)((_tmp416=_region_malloc(r,sizeof(*_tmp416)),((_tmp416[0]=((_tmp417.tag=0,((_tmp417.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp417)))),_tmp416))));}}else{_LL120: _tmp1B5=_tmp1B4;_LL121:(int)_rethrow(_tmp1B5);}_LL11D:;}};}}_LL10C:;}_LL107:;}
# 801
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp418;struct Cyc_Tcenv_Fenv*_tmp1B8=
Cyc_Tcenv_get_fenv(te,((_tmp418="process_continue",_tag_dyneither(_tmp418,sizeof(char),17))));
# 802
struct Cyc_Tcenv_Fenv*_tmp1B9=_tmp1B8;struct Cyc_Tcenv_CtrlEnv*_tmp1BA;_LL123: _tmp1BA=_tmp1B9->ctrl_env;_LL124: {
# 804
void*_tmp1BB=_tmp1BA->continue_stmt;void*_tmp1BC=_tmp1BB;struct Cyc_Absyn_Stmt*_tmp1BD;switch(*((int*)_tmp1BC)){case 3: _LL128: _tmp1BD=((struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1BC)->f1;_LL129:
# 806
{struct Cyc_List_List*_tmp419;_tmp1BD->non_local_preds=((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419->hd=s,((_tmp419->tl=_tmp1BD->non_local_preds,_tmp419))))));}
*sopt=_tmp1BD;
return;case 0: _LL12A: _LL12B:
{const char*_tmp41C;void*_tmp41B;(_tmp41B=0,Cyc_Tcutil_terr(s->loc,((_tmp41C="continue not in a loop",_tag_dyneither(_tmp41C,sizeof(char),23))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}return;case 1: _LL12C: _LL12D:
 goto _LL12F;default: _LL12E: _LL12F: {
const char*_tmp41D;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp41D="bad continue destination",_tag_dyneither(_tmp41D,sizeof(char),25))));}}_LL127:;}_LL122:;}
# 814
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp41E;struct Cyc_Tcenv_Fenv*_tmp1C3=
Cyc_Tcenv_get_fenv(te,((_tmp41E="process_break",_tag_dyneither(_tmp41E,sizeof(char),14))));
# 815
struct Cyc_Tcenv_Fenv*_tmp1C4=_tmp1C3;struct Cyc_Tcenv_CtrlEnv*_tmp1C5;struct Cyc_Tcenv_SharedFenv*_tmp1C6;_LL131: _tmp1C5=_tmp1C4->ctrl_env;_tmp1C6=_tmp1C4->shared;_LL132: {
# 817
void*_tmp1C7=_tmp1C5->break_stmt;void*_tmp1C8=_tmp1C7;struct Cyc_Absyn_Stmt*_tmp1C9;switch(*((int*)_tmp1C8)){case 3: _LL136: _tmp1C9=((struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1C8)->f1;_LL137:
# 819
{struct Cyc_List_List*_tmp41F;_tmp1C9->non_local_preds=((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F->hd=s,((_tmp41F->tl=_tmp1C9->non_local_preds,_tmp41F))))));}
*sopt=_tmp1C9;
return;case 0: _LL138: _LL139:
# 823
{const char*_tmp422;void*_tmp421;(_tmp421=0,Cyc_Tcutil_terr(s->loc,((_tmp422="break not in a loop or switch",_tag_dyneither(_tmp422,sizeof(char),30))),_tag_dyneither(_tmp421,sizeof(void*),0)));}
return;case 2: _LL13A: _LL13B:
# 843 "tcenv.cyc"
 return;default: _LL13C: _LL13D:
# 848
{const char*_tmp425;void*_tmp424;(_tmp424=0,Cyc_Tcutil_terr(s->loc,((_tmp425="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp425,sizeof(char),56))),_tag_dyneither(_tmp424,sizeof(void*),0)));}
return;}_LL135:;}_LL130:;}
# 852
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp426;struct Cyc_Tcenv_Fenv*_tmp1D0=Cyc_Tcenv_get_fenv(te,((_tmp426="process_goto",_tag_dyneither(_tmp426,sizeof(char),13))));struct Cyc_Tcenv_Fenv*_tmp1D1=_tmp1D0;struct Cyc_Tcenv_SharedFenv*_tmp1D2;_LL13F: _tmp1D2=_tmp1D1->shared;_LL140: {
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1D2->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1D3=_tmp1D2->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1D3,l);
struct _RegionHandle*frgn=_tmp1D2->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp427;slopt=((_tmp427=_region_malloc(frgn,sizeof(*_tmp427)),((_tmp427[0]=0,_tmp427))));}{
struct Cyc_List_List*_tmp428;struct Cyc_List_List*new_needed=(_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428->hd=s,((_tmp428->tl=*slopt,_tmp428)))));
_tmp1D2->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1D3,l,new_needed);};}else{
# 864
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp429;s->non_local_preds=((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->hd=s,((_tmp429->tl=s->non_local_preds,_tmp429))))));}
*sopt=s;}}_LL13E:;}
# 870
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 872
const char*_tmp42A;struct Cyc_Tcenv_Fenv*_tmp1D8=Cyc_Tcenv_get_fenv(te,((_tmp42A="process_fallthru",_tag_dyneither(_tmp42A,sizeof(char),17))));struct Cyc_Tcenv_Fenv*_tmp1D9=_tmp1D8;struct Cyc_Tcenv_CtrlEnv*_tmp1DA;_LL144: _tmp1DA=_tmp1D9->ctrl_env;_LL145: {
const struct _tuple10*_tmp1DB=_tmp1DA->fallthru_clause;
if(_tmp1DB != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp42B;(((*_tmp1DB).f1)->body)->non_local_preds=(
(_tmp42B=_cycalloc(sizeof(*_tmp42B)),((_tmp42B->hd=s,((_tmp42B->tl=(((*_tmp1DB).f1)->body)->non_local_preds,_tmp42B))))));}{
struct Cyc_Absyn_Switch_clause**_tmp42C;*clauseopt=((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C[0]=(*_tmp1DB).f1,_tmp42C))));};}
# 879
return _tmp1DB;}_LL143:;}
# 882
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 886
const char*_tmp42D;struct Cyc_Tcenv_Fenv*_tmp1DF=
# 888
Cyc_Tcenv_get_fenv(te,((_tmp42D="set_fallthru",_tag_dyneither(_tmp42D,sizeof(char),13))));
# 886
struct Cyc_Tcenv_Fenv*_tmp1E0=_tmp1DF;struct Cyc_Tcenv_SharedFenv*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_RgnOrder_RgnPO*_tmp1E3;const struct Cyc_Tcenv_Bindings*_tmp1E4;struct Cyc_Absyn_Stmt*_tmp1E5;struct Cyc_Tcenv_CtrlEnv*_tmp1E6;void*_tmp1E7;void*_tmp1E8;struct Cyc_Tcenv_FenvFlags _tmp1E9;_LL149: _tmp1E1=_tmp1E0->shared;_tmp1E2=_tmp1E0->type_vars;_tmp1E3=_tmp1E0->region_order;_tmp1E4=_tmp1E0->locals;_tmp1E5=_tmp1E0->encloser;_tmp1E6=_tmp1E0->ctrl_env;_tmp1E7=_tmp1E0->capability;_tmp1E8=_tmp1E0->curr_rgn;_tmp1E9=_tmp1E0->flags;_LL14A: {
# 889
struct Cyc_Tcenv_CtrlEnv*_tmp1EA=_tmp1E6;struct _RegionHandle*_tmp1EB;void*_tmp1EC;void*_tmp1ED;const struct _tuple10*_tmp1EE;void*_tmp1EF;int _tmp1F0;_LL14E: _tmp1EB=_tmp1EA->ctrl_rgn;_tmp1EC=_tmp1EA->continue_stmt;_tmp1ED=_tmp1EA->break_stmt;_tmp1EE=_tmp1EA->fallthru_clause;_tmp1EF=_tmp1EA->next_stmt;_tmp1F0=_tmp1EA->try_depth;_LL14F: {
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp42E;ft_typ=((_tmp42E=_region_malloc(_tmp1EB,sizeof(*_tmp42E)),((_tmp42E->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp42E->tl=ft_typ,_tmp42E))))));}{
const struct Cyc_Tcenv_CList*_tmp1F2=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp431;struct Cyc_Tcenv_CtrlEnv*_tmp430;struct Cyc_Tcenv_CtrlEnv*_tmp1F3=
(_tmp430=_region_malloc(r,sizeof(*_tmp430)),((_tmp430->ctrl_rgn=_tmp1EB,((_tmp430->continue_stmt=_tmp1EC,((_tmp430->break_stmt=_tmp1ED,((_tmp430->fallthru_clause=(const struct _tuple10*)((_tmp431=_region_malloc(_tmp1EB,sizeof(*_tmp431)),((_tmp431->f1=clause,((_tmp431->f2=new_tvs,((_tmp431->f3=_tmp1F2,_tmp431)))))))),((_tmp430->next_stmt=_tmp1EF,((_tmp430->try_depth=_tmp1F0,_tmp430)))))))))))));
# 897
struct Cyc_Tcenv_Fenv*_tmp432;struct Cyc_Tcenv_Fenv*_tmp1F4=(_tmp432=_region_malloc(r,sizeof(*_tmp432)),((_tmp432->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1E1,((_tmp432->type_vars=(struct Cyc_List_List*)_tmp1E2,((_tmp432->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E3,((_tmp432->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1E4,((_tmp432->encloser=(struct Cyc_Absyn_Stmt*)_tmp1E5,((_tmp432->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1F3,((_tmp432->capability=(void*)_tmp1E7,((_tmp432->curr_rgn=(void*)_tmp1E8,((_tmp432->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1E9,((_tmp432->fnrgn=(struct _RegionHandle*)r,_tmp432)))))))))))))))))))));
# 901
return Cyc_Tcenv_put_fenv(r,te,_tmp1F4);};}_LL14D:;}_LL148:;}
# 904
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp433;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp433="clear_fallthru",_tag_dyneither(_tmp433,sizeof(char),15)))));
struct Cyc_Tcenv_Fenv*_tmp1F9=fe;struct Cyc_Tcenv_CtrlEnv*_tmp1FA;_LL153: _tmp1FA=_tmp1F9->ctrl_env;_LL154:
 _tmp1FA->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);_LL152:;}
# 911
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 913
const char*_tmp434;struct Cyc_Tcenv_Fenv*_tmp1FC=
# 915
Cyc_Tcenv_get_fenv(te,((_tmp434="set_in_loop",_tag_dyneither(_tmp434,sizeof(char),12))));
# 913
struct Cyc_Tcenv_Fenv*_tmp1FD=_tmp1FC;struct Cyc_Tcenv_SharedFenv*_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_RgnOrder_RgnPO*_tmp200;const struct Cyc_Tcenv_Bindings*_tmp201;struct Cyc_Absyn_Stmt*_tmp202;struct Cyc_Tcenv_CtrlEnv*_tmp203;void*_tmp204;void*_tmp205;struct Cyc_Tcenv_FenvFlags _tmp206;_LL158: _tmp1FE=_tmp1FD->shared;_tmp1FF=_tmp1FD->type_vars;_tmp200=_tmp1FD->region_order;_tmp201=_tmp1FD->locals;_tmp202=_tmp1FD->encloser;_tmp203=_tmp1FD->ctrl_env;_tmp204=_tmp1FD->capability;_tmp205=_tmp1FD->curr_rgn;_tmp206=_tmp1FD->flags;_LL159: {
# 916
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp43F;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp43E;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp43D;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp43C;struct Cyc_Tcenv_CtrlEnv*_tmp43B;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp43B=_region_malloc(r,sizeof(*_tmp43B)),((_tmp43B->ctrl_rgn=r,((_tmp43B->continue_stmt=(void*)(
(_tmp43D=_region_malloc(r,sizeof(*_tmp43D)),((_tmp43D[0]=((_tmp43C.tag=3,((_tmp43C.f1=continue_dest,_tmp43C)))),_tmp43D)))),((_tmp43B->break_stmt=_tmp203->next_stmt,((_tmp43B->next_stmt=(void*)(
# 920
(_tmp43F=_region_malloc(r,sizeof(*_tmp43F)),((_tmp43F[0]=((_tmp43E.tag=3,((_tmp43E.f1=continue_dest,_tmp43E)))),_tmp43F)))),((_tmp43B->fallthru_clause=_tmp203->fallthru_clause,((_tmp43B->try_depth=_tmp203->try_depth,_tmp43B)))))))))))));
# 923
struct Cyc_Tcenv_Fenv*_tmp440;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp440=_region_malloc(r,sizeof(*_tmp440)),((_tmp440->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1FE,((_tmp440->type_vars=(struct Cyc_List_List*)_tmp1FF,((_tmp440->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp200,((_tmp440->locals=(const struct Cyc_Tcenv_Bindings*)_tmp201,((_tmp440->encloser=(struct Cyc_Absyn_Stmt*)_tmp202,((_tmp440->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp440->capability=(void*)_tmp204,((_tmp440->curr_rgn=(void*)_tmp205,((_tmp440->flags=(struct Cyc_Tcenv_FenvFlags)_tmp206,((_tmp440->fnrgn=(struct _RegionHandle*)r,_tmp440)))))))))))))))))))));
# 926
return Cyc_Tcenv_put_fenv(r,te,new_fenv);}_LL157:;}
# 929
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp441;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp441="enter_try",_tag_dyneither(_tmp441,sizeof(char),10)))));
struct Cyc_Tcenv_Fenv*_tmp20E=fe;struct Cyc_Tcenv_CtrlEnv*_tmp20F;_LL15D: _tmp20F=_tmp20E->ctrl_env;_LL15E:
 ++ _tmp20F->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);_LL15C:;}
# 935
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp442;struct Cyc_Tcenv_Fenv*_tmp211=Cyc_Tcenv_get_fenv(te,((_tmp442="get_try_depth",_tag_dyneither(_tmp442,sizeof(char),14))));struct Cyc_Tcenv_Fenv*_tmp212=_tmp211;struct Cyc_Tcenv_CtrlEnv*_tmp213;_LL162: _tmp213=_tmp212->ctrl_env;_LL163:
 return _tmp213->try_depth;_LL161:;}
# 942
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp443;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp443="set_in_switch",_tag_dyneither(_tmp443,sizeof(char),14)))));
struct Cyc_Tcenv_Fenv*_tmp215=fe;struct Cyc_Tcenv_CtrlEnv*_tmp216;_LL167: _tmp216=_tmp215->ctrl_env;_LL168:
 _tmp216->break_stmt=_tmp216->next_stmt;
_tmp216->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);_LL166:;}
# 950
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 952
const char*_tmp444;struct Cyc_Tcenv_Fenv _tmp218=*
# 954
Cyc_Tcenv_get_fenv(te,((_tmp444="set_next",_tag_dyneither(_tmp444,sizeof(char),9))));
# 952
struct Cyc_Tcenv_Fenv _tmp219=_tmp218;struct Cyc_Tcenv_SharedFenv*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_RgnOrder_RgnPO*_tmp21C;const struct Cyc_Tcenv_Bindings*_tmp21D;struct Cyc_Absyn_Stmt*_tmp21E;struct Cyc_Tcenv_CtrlEnv*_tmp21F;void*_tmp220;void*_tmp221;struct Cyc_Tcenv_FenvFlags _tmp222;_LL16C: _tmp21A=_tmp219.shared;_tmp21B=_tmp219.type_vars;_tmp21C=_tmp219.region_order;_tmp21D=_tmp219.locals;_tmp21E=_tmp219.encloser;_tmp21F=_tmp219.ctrl_env;_tmp220=_tmp219.capability;_tmp221=_tmp219.curr_rgn;_tmp222=_tmp219.flags;_LL16D: {
# 955
struct Cyc_Tcenv_CtrlEnv*_tmp445;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp445=_region_malloc(r,sizeof(*_tmp445)),((_tmp445->ctrl_rgn=r,((_tmp445->continue_stmt=_tmp21F->continue_stmt,((_tmp445->break_stmt=_tmp21F->break_stmt,((_tmp445->next_stmt=j,((_tmp445->fallthru_clause=_tmp21F->fallthru_clause,((_tmp445->try_depth=_tmp21F->try_depth,_tmp445)))))))))))));
# 963
struct Cyc_Tcenv_Fenv*_tmp446;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp446=_region_malloc(r,sizeof(*_tmp446)),((_tmp446->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp21A,((_tmp446->type_vars=(struct Cyc_List_List*)_tmp21B,((_tmp446->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp21C,((_tmp446->locals=(const struct Cyc_Tcenv_Bindings*)_tmp21D,((_tmp446->encloser=(struct Cyc_Absyn_Stmt*)_tmp21E,((_tmp446->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp446->capability=(void*)_tmp220,((_tmp446->curr_rgn=(void*)_tmp221,((_tmp446->flags=(struct Cyc_Tcenv_FenvFlags)_tmp222,((_tmp446->fnrgn=(struct _RegionHandle*)r,_tmp446)))))))))))))))))))));
# 966
return Cyc_Tcenv_put_fenv(r,te,new_fenv);}_LL16B:;}
# 969
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp447;struct Cyc_Tcenv_Fenv*_tmp226=Cyc_Tcenv_get_fenv(te,((_tmp447="add_label",_tag_dyneither(_tmp447,sizeof(char),10))));struct Cyc_Tcenv_Fenv*_tmp227=_tmp226;struct Cyc_Tcenv_SharedFenv*_tmp228;_LL171: _tmp228=_tmp227->shared;_LL172: {
struct Cyc_Dict_Dict needed=_tmp228->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp228->frgn;
if(sl_opt != 0){
_tmp228->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 977
struct Cyc_List_List*_tmp229=*sl_opt;
s->non_local_preds=_tmp229;
for(0;_tmp229 != 0;_tmp229=_tmp229->tl){
void*_tmp22A=((struct Cyc_Absyn_Stmt*)_tmp229->hd)->r;void*_tmp22B=_tmp22A;struct Cyc_Absyn_Stmt**_tmp22C;if(((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp22B)->tag == 8){_LL176: _tmp22C=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp22B)->f2;_LL177:
*_tmp22C=s;goto _LL175;}else{_LL178: _LL179:
{const char*_tmp448;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp448="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp448,sizeof(char),42))));}goto _LL175;}_LL175:;}};}
# 985
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp228->seen_labels,v)){
const char*_tmp44C;void*_tmp44B[1];struct Cyc_String_pa_PrintArg_struct _tmp44A;(_tmp44A.tag=0,((_tmp44A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp44B[0]=& _tmp44A,Cyc_Tcutil_terr(s->loc,((_tmp44C="Repeated label: %s",_tag_dyneither(_tmp44C,sizeof(char),19))),_tag_dyneither(_tmp44B,sizeof(void*),1)))))));}
_tmp228->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp228->seen_labels,v,s);
return te;}_LL170:;}
# 991
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp44D;struct Cyc_Tcenv_Fenv*_tmp232=Cyc_Tcenv_get_fenv(te,((_tmp44D="all_labels_resolved",_tag_dyneither(_tmp44D,sizeof(char),20))));struct Cyc_Tcenv_Fenv*_tmp233=_tmp232;struct Cyc_Tcenv_SharedFenv*_tmp234;_LL17B: _tmp234=_tmp233->shared;_LL17C:
 return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp234->needed_labels);_LL17A:;}
# 996
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp44E;struct Cyc_Tcenv_Fenv*_tmp236=Cyc_Tcenv_get_fenv(te,((_tmp44E="get_encloser",_tag_dyneither(_tmp44E,sizeof(char),13))));struct Cyc_Tcenv_Fenv*_tmp237=_tmp236;struct Cyc_Absyn_Stmt*_tmp238;_LL180: _tmp238=_tmp237->encloser;_LL181:
 return _tmp238;_LL17F:;}
# 1001
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp44F;return(_tmp44F=_region_malloc(r2,sizeof(*_tmp44F)),((_tmp44F->ns=te->ns,((_tmp44F->ae=te->ae,((_tmp44F->le=0,((_tmp44F->allow_valueof=1,((_tmp44F->in_extern_c_include=te->in_extern_c_include,_tmp44F)))))))))));}{
struct Cyc_Tcenv_Fenv _tmp23B=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp23C=_tmp23B;struct Cyc_Tcenv_SharedFenv*_tmp23D;struct Cyc_List_List*_tmp23E;struct Cyc_RgnOrder_RgnPO*_tmp23F;const struct Cyc_Tcenv_Bindings*_tmp240;struct Cyc_Absyn_Stmt*_tmp241;struct Cyc_Tcenv_CtrlEnv*_tmp242;void*_tmp243;void*_tmp244;struct Cyc_Tcenv_FenvFlags _tmp245;_LL185: _tmp23D=_tmp23C.shared;_tmp23E=_tmp23C.type_vars;_tmp23F=_tmp23C.region_order;_tmp240=_tmp23C.locals;_tmp241=_tmp23C.encloser;_tmp242=_tmp23C.ctrl_env;_tmp243=_tmp23C.capability;_tmp244=_tmp23C.curr_rgn;_tmp245=_tmp23C.flags;_LL186: {
struct Cyc_Tcenv_Fenv*_tmp450;struct Cyc_Tcenv_Fenv*_tmp246=(_tmp450=_region_malloc(r2,sizeof(*_tmp450)),((_tmp450->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp23D,((_tmp450->type_vars=(struct Cyc_List_List*)_tmp23E,((_tmp450->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp23F,((_tmp450->locals=(const struct Cyc_Tcenv_Bindings*)_tmp240,((_tmp450->encloser=(struct Cyc_Absyn_Stmt*)_tmp241,((_tmp450->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp242,((_tmp450->capability=(void*)_tmp243,((_tmp450->curr_rgn=(void*)_tmp244,((_tmp450->flags=(struct Cyc_Tcenv_FenvFlags)_tmp245,((_tmp450->fnrgn=(struct _RegionHandle*)r2,_tmp450)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp451;return(_tmp451=_region_malloc(r2,sizeof(*_tmp451)),((_tmp451->ns=te->ns,((_tmp451->ae=te->ae,((_tmp451->le=_tmp246,((_tmp451->allow_valueof=1,((_tmp451->in_extern_c_include=te->in_extern_c_include,_tmp451)))))))))));}_LL184:;};}
# 1009
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp452;return(_tmp452=_region_malloc(r2,sizeof(*_tmp452)),((_tmp452->ns=te->ns,((_tmp452->ae=te->ae,((_tmp452->le=0,((_tmp452->allow_valueof=te->allow_valueof,((_tmp452->in_extern_c_include=1,_tmp452)))))))))));}{
struct Cyc_Tcenv_Fenv _tmp24A=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp24B=_tmp24A;struct Cyc_Tcenv_SharedFenv*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_RgnOrder_RgnPO*_tmp24E;const struct Cyc_Tcenv_Bindings*_tmp24F;struct Cyc_Absyn_Stmt*_tmp250;struct Cyc_Tcenv_CtrlEnv*_tmp251;void*_tmp252;void*_tmp253;struct Cyc_Tcenv_FenvFlags _tmp254;_LL18A: _tmp24C=_tmp24B.shared;_tmp24D=_tmp24B.type_vars;_tmp24E=_tmp24B.region_order;_tmp24F=_tmp24B.locals;_tmp250=_tmp24B.encloser;_tmp251=_tmp24B.ctrl_env;_tmp252=_tmp24B.capability;_tmp253=_tmp24B.curr_rgn;_tmp254=_tmp24B.flags;_LL18B: {
struct Cyc_Tcenv_Fenv*_tmp453;struct Cyc_Tcenv_Fenv*_tmp255=(_tmp453=_region_malloc(r2,sizeof(*_tmp453)),((_tmp453->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp24C,((_tmp453->type_vars=(struct Cyc_List_List*)_tmp24D,((_tmp453->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24E,((_tmp453->locals=(const struct Cyc_Tcenv_Bindings*)_tmp24F,((_tmp453->encloser=(struct Cyc_Absyn_Stmt*)_tmp250,((_tmp453->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp251,((_tmp453->capability=(void*)_tmp252,((_tmp453->curr_rgn=(void*)_tmp253,((_tmp453->flags=(struct Cyc_Tcenv_FenvFlags)_tmp254,((_tmp453->fnrgn=(struct _RegionHandle*)r2,_tmp453)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp454;return(_tmp454=_region_malloc(r2,sizeof(*_tmp454)),((_tmp454->ns=te->ns,((_tmp454->ae=te->ae,((_tmp454->le=_tmp255,((_tmp454->allow_valueof=te->allow_valueof,((_tmp454->in_extern_c_include=1,_tmp454)))))))))));}_LL189:;};}
# 1017
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1019
const char*_tmp455;struct Cyc_Tcenv_Fenv _tmp258=*
# 1021
Cyc_Tcenv_get_fenv(te,((_tmp455="set_encloser",_tag_dyneither(_tmp455,sizeof(char),13))));
# 1019
struct Cyc_Tcenv_Fenv _tmp259=_tmp258;struct Cyc_Tcenv_SharedFenv*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_RgnOrder_RgnPO*_tmp25C;const struct Cyc_Tcenv_Bindings*_tmp25D;struct Cyc_Absyn_Stmt*_tmp25E;struct Cyc_Tcenv_CtrlEnv*_tmp25F;void*_tmp260;void*_tmp261;struct Cyc_Tcenv_FenvFlags _tmp262;_LL18F: _tmp25A=_tmp259.shared;_tmp25B=_tmp259.type_vars;_tmp25C=_tmp259.region_order;_tmp25D=_tmp259.locals;_tmp25E=_tmp259.encloser;_tmp25F=_tmp259.ctrl_env;_tmp260=_tmp259.capability;_tmp261=_tmp259.curr_rgn;_tmp262=_tmp259.flags;_LL190: {
# 1022
struct Cyc_Tcenv_Fenv*_tmp456;struct Cyc_Tcenv_Fenv*_tmp263=
(_tmp456=_region_malloc(r,sizeof(*_tmp456)),((_tmp456->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp25A,((_tmp456->type_vars=(struct Cyc_List_List*)_tmp25B,((_tmp456->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp25C,((_tmp456->locals=(const struct Cyc_Tcenv_Bindings*)_tmp25D,((_tmp456->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp456->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp25F,((_tmp456->capability=(void*)_tmp260,((_tmp456->curr_rgn=(void*)_tmp261,((_tmp456->flags=(struct Cyc_Tcenv_FenvFlags)_tmp262,((_tmp456->fnrgn=(struct _RegionHandle*)r,_tmp456)))))))))))))))))))));
# 1025
return Cyc_Tcenv_put_fenv(r,te,_tmp263);}_LL18E:;}
# 1028
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1031
const char*_tmp457;struct Cyc_Tcenv_Fenv _tmp266=*
# 1033
Cyc_Tcenv_get_fenv(te,((_tmp457="add_region",_tag_dyneither(_tmp457,sizeof(char),11))));
# 1031
struct Cyc_Tcenv_Fenv _tmp267=_tmp266;struct Cyc_Tcenv_SharedFenv*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_RgnOrder_RgnPO*_tmp26A;const struct Cyc_Tcenv_Bindings*_tmp26B;struct Cyc_Absyn_Stmt*_tmp26C;struct Cyc_Tcenv_CtrlEnv*_tmp26D;void*_tmp26E;void*_tmp26F;struct Cyc_Tcenv_FenvFlags _tmp270;_LL194: _tmp268=_tmp267.shared;_tmp269=_tmp267.type_vars;_tmp26A=_tmp267.region_order;_tmp26B=_tmp267.locals;_tmp26C=_tmp267.encloser;_tmp26D=_tmp267.ctrl_env;_tmp26E=_tmp267.capability;_tmp26F=_tmp267.curr_rgn;_tmp270=_tmp267.flags;_LL195:
# 1034
{void*_tmp271=Cyc_Tcutil_compress(rgn);void*_tmp272=_tmp271;struct Cyc_Absyn_Tvar*_tmp273;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp272)->tag == 2){_LL199: _tmp273=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp272)->f1;_LL19A:
# 1036
 _tmp26A=Cyc_RgnOrder_add_youngest(_tmp268->frgn,_tmp26A,_tmp273,resetable,opened);
# 1038
goto _LL198;}else{_LL19B: _LL19C:
 goto _LL198;}_LL198:;}
# 1041
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp46A;struct Cyc_List_List*_tmp469;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp468;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp467;struct Cyc_List_List*_tmp466;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp465;_tmp26E=(void*)((_tmp465=_cycalloc(sizeof(*_tmp465)),((_tmp465[0]=((_tmp46A.tag=24,((_tmp46A.f1=((_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466->hd=(void*)((_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468[0]=((_tmp467.tag=23,((_tmp467.f1=rgn,_tmp467)))),_tmp468)))),((_tmp466->tl=(
(_tmp469=_cycalloc(sizeof(*_tmp469)),((_tmp469->hd=_tmp26E,((_tmp469->tl=0,_tmp469)))))),_tmp466)))))),_tmp46A)))),_tmp465))));}{
struct Cyc_Tcenv_Fenv*_tmp46B;struct Cyc_Tcenv_Fenv*_tmp27A=
(_tmp46B=_region_malloc(r,sizeof(*_tmp46B)),((_tmp46B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp268,((_tmp46B->type_vars=(struct Cyc_List_List*)_tmp269,((_tmp46B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp26A,((_tmp46B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp26B,((_tmp46B->encloser=(struct Cyc_Absyn_Stmt*)_tmp26C,((_tmp46B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp26D,((_tmp46B->capability=(void*)_tmp26E,((_tmp46B->curr_rgn=(void*)_tmp26F,((_tmp46B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp270,((_tmp46B->fnrgn=(struct _RegionHandle*)r,_tmp46B)))))))))))))))))))));
# 1046
return Cyc_Tcenv_put_fenv(r,te,_tmp27A);};_LL193:;}
# 1049
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1051
const char*_tmp46C;struct Cyc_Tcenv_Fenv _tmp27D=*
# 1053
Cyc_Tcenv_get_fenv(te,((_tmp46C="new_named_block",_tag_dyneither(_tmp46C,sizeof(char),16))));
# 1051
struct Cyc_Tcenv_Fenv _tmp27E=_tmp27D;struct Cyc_Tcenv_SharedFenv*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_RgnOrder_RgnPO*_tmp281;const struct Cyc_Tcenv_Bindings*_tmp282;struct Cyc_Absyn_Stmt*_tmp283;struct Cyc_Tcenv_CtrlEnv*_tmp284;void*_tmp285;void*_tmp286;struct Cyc_Tcenv_FenvFlags _tmp287;_LL19E: _tmp27F=_tmp27E.shared;_tmp280=_tmp27E.type_vars;_tmp281=_tmp27E.region_order;_tmp282=_tmp27E.locals;_tmp283=_tmp27E.encloser;_tmp284=_tmp27E.ctrl_env;_tmp285=_tmp27E.capability;_tmp286=_tmp27E.curr_rgn;_tmp287=_tmp27E.flags;_LL19F: {
# 1055
const char*_tmp46D;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp46D="new_block",_tag_dyneither(_tmp46D,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp470;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp46F;void*block_typ=(void*)((_tmp46F=_cycalloc(sizeof(*_tmp46F)),((_tmp46F[0]=((_tmp470.tag=2,((_tmp470.f1=block_rgn,_tmp470)))),_tmp46F))));
{struct Cyc_List_List*_tmp471;_tmp280=((_tmp471=_cycalloc(sizeof(*_tmp471)),((_tmp471->hd=block_rgn,((_tmp471->tl=_tmp280,_tmp471))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp280);
_tmp281=Cyc_RgnOrder_add_youngest(_tmp27F->frgn,_tmp281,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp484;struct Cyc_List_List*_tmp483;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp482;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp481;struct Cyc_List_List*_tmp480;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp47F;_tmp285=(void*)((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((_tmp484.tag=24,((_tmp484.f1=((_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480->hd=(void*)((_tmp482=_cycalloc(sizeof(*_tmp482)),((_tmp482[0]=((_tmp481.tag=23,((_tmp481.f1=block_typ,_tmp481)))),_tmp482)))),((_tmp480->tl=((_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483->hd=_tmp285,((_tmp483->tl=0,_tmp483)))))),_tmp480)))))),_tmp484)))),_tmp47F))));}
_tmp286=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp485;struct Cyc_Tcenv_Fenv*_tmp28F=
(_tmp485=_region_malloc(r,sizeof(*_tmp485)),((_tmp485->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp27F,((_tmp485->type_vars=(struct Cyc_List_List*)_tmp280,((_tmp485->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp281,((_tmp485->locals=(const struct Cyc_Tcenv_Bindings*)_tmp282,((_tmp485->encloser=(struct Cyc_Absyn_Stmt*)_tmp283,((_tmp485->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp284,((_tmp485->capability=(void*)_tmp285,((_tmp485->curr_rgn=(void*)_tmp286,((_tmp485->flags=(struct Cyc_Tcenv_FenvFlags)_tmp287,((_tmp485->fnrgn=(struct _RegionHandle*)r,_tmp485)))))))))))))))))))));
# 1065
return Cyc_Tcenv_put_fenv(r,te,_tmp28F);};}_LL19D:;}
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
const char*_tmp486;struct Cyc_Tcenv_Fenv _tmp296=*
# 1084
Cyc_Tcenv_get_fenv(te,((_tmp486="new_outlives_constraints",_tag_dyneither(_tmp486,sizeof(char),25))));
# 1082
struct Cyc_Tcenv_Fenv _tmp297=_tmp296;struct Cyc_Tcenv_SharedFenv*_tmp298;struct Cyc_List_List*_tmp299;struct Cyc_RgnOrder_RgnPO*_tmp29A;const struct Cyc_Tcenv_Bindings*_tmp29B;struct Cyc_Absyn_Stmt*_tmp29C;struct Cyc_Tcenv_CtrlEnv*_tmp29D;void*_tmp29E;void*_tmp29F;struct Cyc_Tcenv_FenvFlags _tmp2A0;_LL1A3: _tmp298=_tmp297.shared;_tmp299=_tmp297.type_vars;_tmp29A=_tmp297.region_order;_tmp29B=_tmp297.locals;_tmp29C=_tmp297.encloser;_tmp29D=_tmp297.ctrl_env;_tmp29E=_tmp297.capability;_tmp29F=_tmp297.curr_rgn;_tmp2A0=_tmp297.flags;_LL1A4:
# 1085
 for(0;cs != 0;cs=cs->tl){
_tmp29A=
Cyc_RgnOrder_add_outlives_constraint(_tmp298->frgn,_tmp29A,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1090
struct Cyc_Tcenv_Fenv*_tmp487;struct Cyc_Tcenv_Fenv*_tmp2A1=
(_tmp487=_region_malloc(r,sizeof(*_tmp487)),((_tmp487->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp298,((_tmp487->type_vars=(struct Cyc_List_List*)_tmp299,((_tmp487->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp29A,((_tmp487->locals=(const struct Cyc_Tcenv_Bindings*)_tmp29B,((_tmp487->encloser=(struct Cyc_Absyn_Stmt*)_tmp29C,((_tmp487->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp29D,((_tmp487->capability=(void*)_tmp29E,((_tmp487->curr_rgn=(void*)_tmp29F,((_tmp487->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2A0,((_tmp487->fnrgn=(struct _RegionHandle*)r,_tmp487)))))))))))))))))))));
# 1093
return Cyc_Tcenv_put_fenv(r,te,_tmp2A1);};_LL1A2:;}
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
{const char*_tmp48E;void*_tmp48D[4];struct Cyc_String_pa_PrintArg_struct _tmp48C;struct Cyc_String_pa_PrintArg_struct _tmp48B;struct Cyc_String_pa_PrintArg_struct _tmp48A;struct Cyc_String_pa_PrintArg_struct _tmp489;(_tmp489.tag=0,((_tmp489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp2A7)),((_tmp48A.tag=0,((_tmp48A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp2A6)),((_tmp48B.tag=0,((_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2A5)),((_tmp48C.tag=0,((_tmp48C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2A4)),((_tmp48D[0]=& _tmp48C,((_tmp48D[1]=& _tmp48B,((_tmp48D[2]=& _tmp48A,((_tmp48D[3]=& _tmp489,Cyc_Tcutil_terr(loc,((_tmp48E="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp48E,sizeof(char),82))),_tag_dyneither(_tmp48D,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1117
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp2AE=Cyc_Tcutil_swap_kind(_tmp2A5,Cyc_Tcutil_kind_to_bound(_tmp2A6));struct _tuple11 _tmp2AF=_tmp2AE;struct Cyc_Absyn_Tvar*_tmp2B0;void*_tmp2B1;_LL1A8: _tmp2B0=_tmp2AF.f1;_tmp2B1=_tmp2AF.f2;_LL1A9: {
struct _tuple11*_tmp48F;*oldtv=((_tmp48F=_region_malloc(r,sizeof(*_tmp48F)),((_tmp48F->f1=_tmp2B0,((_tmp48F->f2=_tmp2B1,_tmp48F))))));}_LL1A7:;}else{
# 1121
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp2B3=Cyc_Tcutil_swap_kind(_tmp2A4,Cyc_Tcutil_kind_to_bound(_tmp2A7));struct _tuple11 _tmp2B4=_tmp2B3;struct Cyc_Absyn_Tvar*_tmp2B5;void*_tmp2B6;_LL1AD: _tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;_LL1AE: {
struct _tuple11*_tmp490;*oldtv=((_tmp490=_region_malloc(r,sizeof(*_tmp490)),((_tmp490->f1=_tmp2B5,((_tmp490->f2=_tmp2B6,_tmp490))))));}_LL1AC:;}}}{
# 1127
struct _RegionHandle*_tmp2B8=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2B9=0;
if((_tmp2A4 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2A4 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2A4 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp493;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp492;void*eff1=(void*)((_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492[0]=((_tmp493.tag=23,((_tmp493.f1=_tmp2A4,_tmp493)))),_tmp492))));
struct _tuple13*_tmp496;struct Cyc_List_List*_tmp495;_tmp2B9=((_tmp495=_region_malloc(_tmp2B8,sizeof(*_tmp495)),((_tmp495->hd=((_tmp496=_region_malloc(_tmp2B8,sizeof(*_tmp496)),((_tmp496->f1=eff1,((_tmp496->f2=_tmp2A5,_tmp496)))))),((_tmp495->tl=_tmp2B9,_tmp495))))));}
# 1133
if((_tmp2A5 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2A5 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2A5 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp499;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp498;void*eff2=(void*)((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498[0]=((_tmp499.tag=23,((_tmp499.f1=_tmp2A5,_tmp499)))),_tmp498))));
struct _tuple13*_tmp49C;struct Cyc_List_List*_tmp49B;_tmp2B9=((_tmp49B=_region_malloc(_tmp2B8,sizeof(*_tmp49B)),((_tmp49B->hd=((_tmp49C=_region_malloc(_tmp2B8,sizeof(*_tmp49C)),((_tmp49C->f1=eff2,((_tmp49C->f2=_tmp2A4,_tmp49C)))))),((_tmp49B->tl=_tmp2B9,_tmp49B))))));}
# 1138
return Cyc_Tcenv_new_outlives_constraints(_tmp2B8,te,_tmp2B9,loc);};}
# 1141
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2C2=te->le;
if(_tmp2C2 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2C2;
struct Cyc_Tcenv_Fenv*_tmp2C3=fe;void*_tmp2C4;_LL1B2: _tmp2C4=_tmp2C3->curr_rgn;_LL1B3:
 return _tmp2C4;_LL1B1:;};}
# 1152
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp49D;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp49D="check_rgn_accessible",_tag_dyneither(_tmp49D,sizeof(char),21))));
struct Cyc_Tcenv_Fenv*_tmp2C5=fe;void*_tmp2C6;struct Cyc_RgnOrder_RgnPO*_tmp2C7;_LL1B7: _tmp2C6=_tmp2C5->capability;_tmp2C7=_tmp2C5->region_order;_LL1B8:
 if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2C6) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2C6))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4A0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp49F;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2C7,(void*)((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F[0]=((_tmp4A0.tag=23,((_tmp4A0.f1=rgn,_tmp4A0)))),_tmp49F)))),_tmp2C6))
return;}{
const char*_tmp4A4;void*_tmp4A3[1];struct Cyc_String_pa_PrintArg_struct _tmp4A2;(_tmp4A2.tag=0,((_tmp4A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4A3[0]=& _tmp4A2,Cyc_Tcutil_terr(loc,((_tmp4A4="Expression accesses unavailable region %s",_tag_dyneither(_tmp4A4,sizeof(char),42))),_tag_dyneither(_tmp4A3,sizeof(void*),1)))))));};_LL1B6:;}
# 1164
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp4A5;struct Cyc_Tcenv_Fenv*_tmp2CE=
Cyc_Tcenv_get_fenv(te,((_tmp4A5="check_rgn_resetable",_tag_dyneither(_tmp4A5,sizeof(char),20))));
# 1166
struct Cyc_Tcenv_Fenv*_tmp2CF=_tmp2CE;struct Cyc_RgnOrder_RgnPO*_tmp2D0;_LL1BC: _tmp2D0=_tmp2CF->region_order;_LL1BD: {
# 1168
void*_tmp2D1=Cyc_Tcutil_compress(rgn);void*_tmp2D2=_tmp2D1;struct Cyc_Absyn_Tvar*_tmp2D3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D2)->tag == 2){_LL1C1: _tmp2D3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D2)->f1;_LL1C2:
# 1170
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2D0,_tmp2D3)){
const char*_tmp4A9;void*_tmp4A8[1];struct Cyc_String_pa_PrintArg_struct _tmp4A7;(_tmp4A7.tag=0,((_tmp4A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4A8[0]=& _tmp4A7,Cyc_Tcutil_terr(loc,((_tmp4A9="Region %s is not resetable",_tag_dyneither(_tmp4A9,sizeof(char),27))),_tag_dyneither(_tmp4A8,sizeof(void*),1)))))));}
return;}else{_LL1C3: _LL1C4: {
# 1174
const char*_tmp4AC;void*_tmp4AB;(_tmp4AB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AC="check_rgn_resetable",_tag_dyneither(_tmp4AC,sizeof(char),20))),_tag_dyneither(_tmp4AB,sizeof(void*),0)));}}_LL1C0:;}_LL1BB:;};}
# 1181
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2DA=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1186
if(_tmp2DA == 0){
void*_tmp2DB=rt_a;switch(*((int*)_tmp2DB)){case 22: _LL1C6: _LL1C7:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LL1C8: _LL1C9:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LL1CA: _LL1CB:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL1CC: _LL1CD:
 return 0;}_LL1C5:;}{
# 1194
struct Cyc_Tcenv_Fenv*fe=_tmp2DA;
struct Cyc_Tcenv_Fenv*_tmp2DC=fe;struct Cyc_RgnOrder_RgnPO*_tmp2DD;_LL1CF: _tmp2DD=_tmp2DC->region_order;_LL1D0: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4AF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4AE;int _tmp2DE=Cyc_RgnOrder_effect_outlives(_tmp2DD,(void*)((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE[0]=((_tmp4AF.tag=23,((_tmp4AF.f1=rt_a,_tmp4AF)))),_tmp4AE)))),rt_b);
# 1200
return _tmp2DE;}_LL1CE:;};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1205
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp4B0;struct Cyc_Tcenv_Fenv*_tmp2E1=
Cyc_Tcenv_get_fenv(te,((_tmp4B0="check_effect_accessible",_tag_dyneither(_tmp4B0,sizeof(char),24))));
# 1206
struct Cyc_Tcenv_Fenv*_tmp2E2=_tmp2E1;void*_tmp2E3;struct Cyc_RgnOrder_RgnPO*_tmp2E4;struct Cyc_Tcenv_SharedFenv*_tmp2E5;_LL1D4: _tmp2E3=_tmp2E2->capability;_tmp2E4=_tmp2E2->region_order;_tmp2E5=_tmp2E2->shared;_LL1D5:
# 1208
 if(Cyc_Tcutil_subset_effect(0,eff,_tmp2E3))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2E4,eff,_tmp2E3))
return;{
struct _RegionHandle*frgn=_tmp2E5->frgn;
struct _tuple14*_tmp4B3;struct Cyc_List_List*_tmp4B2;_tmp2E5->delayed_effect_checks=(
(_tmp4B2=_region_malloc(frgn,sizeof(*_tmp4B2)),((_tmp4B2->hd=((_tmp4B3=_region_malloc(frgn,sizeof(*_tmp4B3)),((_tmp4B3->f1=_tmp2E3,((_tmp4B3->f2=eff,((_tmp4B3->f3=_tmp2E4,((_tmp4B3->f4=loc,_tmp4B3)))))))))),((_tmp4B2->tl=_tmp2E5->delayed_effect_checks,_tmp4B2))))));};_LL1D3:;}
# 1217
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4B4;struct Cyc_Tcenv_Fenv*_tmp2E9=
Cyc_Tcenv_get_fenv(te,((_tmp4B4="check_delayed_effects",_tag_dyneither(_tmp4B4,sizeof(char),22))));
# 1218
struct Cyc_Tcenv_Fenv*_tmp2EA=_tmp2E9;struct Cyc_Tcenv_SharedFenv*_tmp2EB;_LL1D9: _tmp2EB=_tmp2EA->shared;_LL1DA: {
# 1220
struct Cyc_List_List*_tmp2EC=_tmp2EB->delayed_effect_checks;
for(0;_tmp2EC != 0;_tmp2EC=_tmp2EC->tl){
struct _tuple14*_tmp2ED=(struct _tuple14*)_tmp2EC->hd;struct _tuple14*_tmp2EE=_tmp2ED;void*_tmp2EF;void*_tmp2F0;struct Cyc_RgnOrder_RgnPO*_tmp2F1;unsigned int _tmp2F2;_LL1DE: _tmp2EF=_tmp2EE->f1;_tmp2F0=_tmp2EE->f2;_tmp2F1=_tmp2EE->f3;_tmp2F2=_tmp2EE->f4;_LL1DF:
 if(Cyc_Tcutil_subset_effect(1,_tmp2F0,_tmp2EF))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2F1,_tmp2F0,_tmp2EF))
continue;{
const char*_tmp4B9;void*_tmp4B8[2];struct Cyc_String_pa_PrintArg_struct _tmp4B7;struct Cyc_String_pa_PrintArg_struct _tmp4B6;(_tmp4B6.tag=0,((_tmp4B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2F0)),((_tmp4B7.tag=0,((_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2EF)),((_tmp4B8[0]=& _tmp4B7,((_tmp4B8[1]=& _tmp4B6,Cyc_Tcutil_terr(_tmp2F2,((_tmp4B9="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp4B9,sizeof(char),51))),_tag_dyneither(_tmp4B8,sizeof(void*),2)))))))))))));};_LL1DD:;}}_LL1D8:;}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1236
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1238
struct Cyc_Tcenv_Fenv*_tmp2F8=te->le;
if(_tmp2F8 == 0){
# 1241
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4BC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4BB;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB[0]=((_tmp4BC.tag=23,((_tmp4BC.f1=(*((struct _tuple13*)po->hd)).f2,_tmp4BC)))),_tmp4BB)))),Cyc_Absyn_empty_effect)){
const char*_tmp4BF;void*_tmp4BE;(_tmp4BE=0,Cyc_Tcutil_terr(loc,((_tmp4BF="the required region ordering is not satisfied here",_tag_dyneither(_tmp4BF,sizeof(char),51))),_tag_dyneither(_tmp4BE,sizeof(void*),0)));}}
return;}{
# 1247
struct Cyc_Tcenv_Fenv*_tmp2FD=_tmp2F8;struct Cyc_Tcenv_Fenv*_tmp2FE=_tmp2FD;struct Cyc_RgnOrder_RgnPO*_tmp2FF;struct Cyc_Tcenv_SharedFenv*_tmp300;_LL1E3: _tmp2FF=_tmp2FE->region_order;_tmp300=_tmp2FE->shared;_LL1E4:
 if(!Cyc_RgnOrder_satisfies_constraints(_tmp2FF,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp4C2;struct Cyc_List_List*_tmp4C1;_tmp300->delayed_constraint_checks=(
(_tmp4C1=_region_malloc(_tmp300->frgn,sizeof(*_tmp4C1)),((_tmp4C1->hd=((_tmp4C2=_region_malloc(_tmp300->frgn,sizeof(*_tmp4C2)),((_tmp4C2->f1=_tmp2FF,((_tmp4C2->f2=po,((_tmp4C2->f3=loc,_tmp4C2)))))))),((_tmp4C1->tl=_tmp300->delayed_constraint_checks,_tmp4C1))))));}_LL1E2:;};}
# 1254
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4C3;struct Cyc_Tcenv_Fenv*_tmp303=
Cyc_Tcenv_get_fenv(te,((_tmp4C3="check_delayed_constraints",_tag_dyneither(_tmp4C3,sizeof(char),26))));
# 1255
struct Cyc_Tcenv_Fenv*_tmp304=_tmp303;struct Cyc_Tcenv_SharedFenv*_tmp305;_LL1E8: _tmp305=_tmp304->shared;_LL1E9: {
# 1257
struct Cyc_List_List*_tmp306=_tmp305->delayed_constraint_checks;
for(0;_tmp306 != 0;_tmp306=_tmp306->tl){
struct _tuple15*_tmp307=(struct _tuple15*)_tmp306->hd;struct _tuple15*_tmp308=_tmp307;struct Cyc_RgnOrder_RgnPO*_tmp309;struct Cyc_List_List*_tmp30A;unsigned int _tmp30B;_LL1ED: _tmp309=_tmp308->f1;_tmp30A=_tmp308->f2;_tmp30B=_tmp308->f3;_LL1EE:
 if(!Cyc_RgnOrder_satisfies_constraints(_tmp309,_tmp30A,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp4C6;void*_tmp4C5;(_tmp4C5=0,Cyc_Tcutil_terr(_tmp30B,((_tmp4C6="the required region ordering is not satisfied here",_tag_dyneither(_tmp4C6,sizeof(char),51))),_tag_dyneither(_tmp4C5,sizeof(void*),0)));}_LL1EC:;}}_LL1E7:;}
# 1265
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp4D3;const char*_tmp4D2;void*_tmp4D1[1];struct Cyc_String_pa_PrintArg_struct _tmp4D0;struct Cyc_Absyn_Tvar*_tmp4CF;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4CF=_cycalloc(sizeof(*_tmp4CF)),((_tmp4CF->name=((_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3[0]=(struct _dyneither_ptr)((_tmp4D0.tag=0,((_tmp4D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4D1[0]=& _tmp4D0,Cyc_aprintf(((_tmp4D2="`%s",_tag_dyneither(_tmp4D2,sizeof(char),4))),_tag_dyneither(_tmp4D1,sizeof(void*),1)))))))),_tmp4D3)))),((_tmp4CF->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4CF->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4CF)))))));
struct Cyc_List_List*_tmp4D4;struct Cyc_List_List*_tmp30F=(_tmp4D4=_cycalloc(sizeof(*_tmp4D4)),((_tmp4D4->hd=rgn0,((_tmp4D4->tl=fd->tvs,_tmp4D4)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp30F);{
# 1273
struct Cyc_RgnOrder_RgnPO*_tmp310=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1275
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4D7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D6;void*param_rgn=(void*)((_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D7.tag=2,((_tmp4D7.f1=rgn0,_tmp4D7)))),_tmp4D6))));
struct Cyc_List_List*_tmp311=0;
{struct Cyc_List_List*_tmp312=fd->args;for(0;_tmp312 != 0;_tmp312=_tmp312->tl){
struct Cyc_Absyn_Vardecl _tmp4DD;struct _tuple0*_tmp4DC;struct Cyc_Absyn_Vardecl*_tmp4DB;struct Cyc_Absyn_Vardecl*_tmp313=(_tmp4DB=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4DB[0]=(struct Cyc_Absyn_Vardecl)((_tmp4DD.sc=Cyc_Absyn_Public,((_tmp4DD.name=(
(_tmp4DC=_cycalloc(sizeof(*_tmp4DC)),((_tmp4DC->f1=Cyc_Absyn_Loc_n,((_tmp4DC->f2=(*((struct _tuple8*)_tmp312->hd)).f1,_tmp4DC)))))),((_tmp4DD.tq=(*((struct _tuple8*)_tmp312->hd)).f2,((_tmp4DD.type=(*((struct _tuple8*)_tmp312->hd)).f3,((_tmp4DD.initializer=0,((_tmp4DD.rgn=param_rgn,((_tmp4DD.attributes=0,((_tmp4DD.escapes=0,_tmp4DD)))))))))))))))),_tmp4DB)));
# 1285
{struct Cyc_List_List _tmp4E0;struct Cyc_List_List*_tmp4DF;_tmp311=((_tmp4DF=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4DF[0]=(struct Cyc_List_List)((_tmp4E0.hd=_tmp313,((_tmp4E0.tl=_tmp311,_tmp4E0)))),_tmp4DF))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4E3;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4E2;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp316=(_tmp4E2=_cycalloc(sizeof(*_tmp4E2)),((_tmp4E2[0]=((_tmp4E3.tag=3,((_tmp4E3.f1=_tmp313,_tmp4E3)))),_tmp4E2)));
struct _dyneither_ptr*_tmp317=(*((struct _tuple8*)_tmp312->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4E4;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4->v=_tmp317,((_tmp4E4->b=(void*)_tmp316,((_tmp4E4->tl=locals,_tmp4E4))))))));};}}
# 1290
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp31E=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp31F=_tmp31E;struct _dyneither_ptr*_tmp320;struct Cyc_Absyn_Tqual _tmp321;void*_tmp322;int _tmp323;_LL1F2: _tmp320=_tmp31F.name;_tmp321=_tmp31F.tq;_tmp322=_tmp31F.type;_tmp323=_tmp31F.inject;_LL1F3:
 if(_tmp320 != 0){
void*_tmp324=Cyc_Absyn_dyneither_typ(_tmp322,param_rgn,_tmp321,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4EA;struct _tuple0*_tmp4E9;struct Cyc_Absyn_Vardecl*_tmp4E8;struct Cyc_Absyn_Vardecl*_tmp325=(_tmp4E8=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4E8[0]=(struct Cyc_Absyn_Vardecl)((_tmp4EA.sc=Cyc_Absyn_Public,((_tmp4EA.name=(
(_tmp4E9=_cycalloc(sizeof(*_tmp4E9)),((_tmp4E9->f1=Cyc_Absyn_Loc_n,((_tmp4E9->f2=_tmp320,_tmp4E9)))))),((_tmp4EA.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4EA.type=_tmp324,((_tmp4EA.initializer=0,((_tmp4EA.rgn=param_rgn,((_tmp4EA.attributes=0,((_tmp4EA.escapes=0,_tmp4EA)))))))))))))))),_tmp4E8)));
# 1301
{struct Cyc_List_List*_tmp4EB;_tmp311=((_tmp4EB=_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB->hd=_tmp325,((_tmp4EB->tl=_tmp311,_tmp4EB))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4EE;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4ED;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp327=(_tmp4ED=_cycalloc(sizeof(*_tmp4ED)),((_tmp4ED[0]=((_tmp4EE.tag=3,((_tmp4EE.f1=_tmp325,_tmp4EE)))),_tmp4ED)));
struct _dyneither_ptr*_tmp328=_tmp320;
struct Cyc_Tcenv_Bindings*_tmp4EF;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF)),((_tmp4EF->v=_tmp328,((_tmp4EF->b=(void*)_tmp327,((_tmp4EF->tl=locals,_tmp4EF))))))));};}else{
# 1306
const char*_tmp4F2;void*_tmp4F1;(_tmp4F1=0,Cyc_Tcutil_terr(loc,((_tmp4F2="missing name for varargs",_tag_dyneither(_tmp4F2,sizeof(char),25))),_tag_dyneither(_tmp4F1,sizeof(void*),0)));}_LL1F1:;}
# 1308
{struct Cyc_Core_Opt _tmp4F5;struct Cyc_Core_Opt*_tmp4F4;fd->param_vardecls=((_tmp4F4=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4F4[0]=(struct Cyc_Core_Opt)((_tmp4F5.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp311),_tmp4F5)),_tmp4F4))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp513;struct Cyc_List_List*_tmp512;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp511;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp510;struct Cyc_List_List*_tmp50F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp50E;struct Cyc_Tcenv_CtrlEnv*_tmp50D;struct Cyc_Tcenv_SharedFenv*_tmp50C;struct Cyc_Tcenv_Fenv*_tmp50B;return(_tmp50B=_region_malloc(r,sizeof(*_tmp50B)),((_tmp50B->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp50C=_region_malloc(r,sizeof(*_tmp50C)),((_tmp50C->frgn=r,((_tmp50C->return_typ=fd->ret_type,((_tmp50C->seen_labels=
# 1313
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp50C->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp50C->delayed_effect_checks=0,((_tmp50C->delayed_constraint_checks=0,_tmp50C)))))))))))))),((_tmp50B->type_vars=(struct Cyc_List_List*)_tmp30F,((_tmp50B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp310,((_tmp50B->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp50B->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp50B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1322
(_tmp50D=_region_malloc(r,sizeof(*_tmp50D)),((_tmp50D->ctrl_rgn=r,((_tmp50D->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp50D->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp50D->fallthru_clause=0,((_tmp50D->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp50D->try_depth=0,_tmp50D)))))))))))))),((_tmp50B->capability=(void*)((void*)(
(_tmp513=_cycalloc(sizeof(*_tmp513)),((_tmp513[0]=((_tmp50E.tag=24,((_tmp50E.f1=((_tmp512=_cycalloc(sizeof(*_tmp512)),((_tmp512->hd=(void*)((_tmp510=_cycalloc(sizeof(*_tmp510)),((_tmp510[0]=((_tmp511.tag=23,((_tmp511.f1=param_rgn,_tmp511)))),_tmp510)))),((_tmp512->tl=(
(_tmp50F=_cycalloc(sizeof(*_tmp50F)),((_tmp50F->hd=(void*)_check_null(fd->effect),((_tmp50F->tl=0,_tmp50F)))))),_tmp512)))))),_tmp50E)))),_tmp513))))),((_tmp50B->curr_rgn=(void*)param_rgn,((_tmp50B->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1326
((_tmp50B->flags).in_notreadctxt=0,(((_tmp50B->flags).in_lhs=0,(((_tmp50B->flags).in_new=0,(((_tmp50B->flags).abstract_ok=0,_tmp50B->flags)))))))),((_tmp50B->fnrgn=(struct _RegionHandle*)r,_tmp50B)))))))))))))))))))));};};}
# 1331
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 1334
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*_tmp514;struct Cyc_Absyn_Tvar*rgn0=(_tmp514=_cycalloc(sizeof(*_tmp514)),((_tmp514->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6),((_tmp514->identity=Cyc_Tcutil_new_tvar_id(),((_tmp514->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp514)))))));
struct Cyc_List_List*_tmp515;struct Cyc_List_List*_tmp344=(_tmp515=_cycalloc(sizeof(*_tmp515)),((_tmp515->hd=rgn0,((_tmp515->tl=0,_tmp515)))));
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp518;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp517;struct Cyc_RgnOrder_RgnPO*_tmp345=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)((_tmp517=_cycalloc(sizeof(*_tmp517)),((_tmp517[0]=((_tmp518.tag=24,((_tmp518.f1=0,_tmp518)))),_tmp517)))),rgn0,0);
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp51B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp51A;void*param_rgn=(void*)((_tmp51A=_cycalloc(sizeof(*_tmp51A)),((_tmp51A[0]=((_tmp51B.tag=2,((_tmp51B.f1=rgn0,_tmp51B)))),_tmp51A))));
struct Cyc_List_List*_tmp346=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp347=*((struct _tuple8*)args->hd);struct _tuple8 _tmp348=_tmp347;struct _dyneither_ptr*_tmp349;struct Cyc_Absyn_Tqual _tmp34A;void*_tmp34B;_LL1F7: _tmp349=_tmp348.f1;_tmp34A=_tmp348.f2;_tmp34B=_tmp348.f3;_LL1F8:
 if(_tmp349 != 0){
struct Cyc_Absyn_Vardecl _tmp521;struct _tuple0*_tmp520;struct Cyc_Absyn_Vardecl*_tmp51F;struct Cyc_Absyn_Vardecl*_tmp34C=(_tmp51F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp51F[0]=(struct Cyc_Absyn_Vardecl)((_tmp521.sc=Cyc_Absyn_Public,((_tmp521.name=(
(_tmp520=_cycalloc(sizeof(*_tmp520)),((_tmp520->f1=Cyc_Absyn_Loc_n,((_tmp520->f2=_tmp349,_tmp520)))))),((_tmp521.tq=_tmp34A,((_tmp521.type=_tmp34B,((_tmp521.initializer=0,((_tmp521.rgn=param_rgn,((_tmp521.attributes=0,((_tmp521.escapes=0,_tmp521)))))))))))))))),_tmp51F)));
# 1350
{struct Cyc_List_List _tmp524;struct Cyc_List_List*_tmp523;_tmp346=((_tmp523=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp523[0]=(struct Cyc_List_List)((_tmp524.hd=_tmp34C,((_tmp524.tl=_tmp346,_tmp524)))),_tmp523))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp527;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp526;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp34F=(_tmp526=_cycalloc(sizeof(*_tmp526)),((_tmp526[0]=((_tmp527.tag=3,((_tmp527.f1=_tmp34C,_tmp527)))),_tmp526)));
struct _dyneither_ptr*_tmp350=_tmp349;
struct Cyc_Tcenv_Bindings*_tmp528;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp528=_region_malloc(r,sizeof(*_tmp528)),((_tmp528->v=_tmp350,((_tmp528->b=(void*)_tmp34F,((_tmp528->tl=locals,_tmp528))))))));};}_LL1F6:;}{
# 1356
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp541;struct Cyc_List_List*_tmp540;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp53F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp53E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp53D;struct Cyc_Tcenv_CtrlEnv*_tmp53C;struct Cyc_Tcenv_SharedFenv*_tmp53B;struct Cyc_Tcenv_Fenv*_tmp53A;return(_tmp53A=_region_malloc(r,sizeof(*_tmp53A)),((_tmp53A->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp53B=_region_malloc(r,sizeof(*_tmp53B)),((_tmp53B->frgn=r,((_tmp53B->return_typ=ret_type,((_tmp53B->seen_labels=
# 1360
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp53B->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp53B->delayed_effect_checks=0,((_tmp53B->delayed_constraint_checks=0,_tmp53B)))))))))))))),((_tmp53A->type_vars=(struct Cyc_List_List*)_tmp344,((_tmp53A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp345,((_tmp53A->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp53A->encloser=(struct Cyc_Absyn_Stmt*)
# 1368
Cyc_Absyn_skip_stmt(0),((_tmp53A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
(_tmp53C=_region_malloc(r,sizeof(*_tmp53C)),((_tmp53C->ctrl_rgn=r,((_tmp53C->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp53C->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp53C->fallthru_clause=0,((_tmp53C->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp53C->try_depth=0,_tmp53C)))))))))))))),((_tmp53A->capability=(void*)((void*)(
(_tmp541=_cycalloc(sizeof(*_tmp541)),((_tmp541[0]=((_tmp53D.tag=24,((_tmp53D.f1=((_tmp540=_cycalloc(sizeof(*_tmp540)),((_tmp540->hd=(void*)((_tmp53E=_cycalloc(sizeof(*_tmp53E)),((_tmp53E[0]=((_tmp53F.tag=23,((_tmp53F.f1=param_rgn,_tmp53F)))),_tmp53E)))),((_tmp540->tl=0,_tmp540)))))),_tmp53D)))),_tmp541))))),((_tmp53A->curr_rgn=(void*)param_rgn,((_tmp53A->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1372
((_tmp53A->flags).in_notreadctxt=0,(((_tmp53A->flags).in_lhs=0,(((_tmp53A->flags).in_new=0,(((_tmp53A->flags).abstract_ok=0,_tmp53A->flags)))))))),((_tmp53A->fnrgn=(struct _RegionHandle*)r,_tmp53A)))))))))))))))))))));};}
# 1377
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp366=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp367;struct Cyc_RgnOrder_RgnPO*_tmp368;struct Cyc_List_List*_tmp369;struct Cyc_Tcenv_SharedFenv*_tmp36A;struct _RegionHandle*_tmp36B;_LL1FC: _tmp367=_tmp366->locals;_tmp368=_tmp366->region_order;_tmp369=_tmp366->type_vars;_tmp36A=_tmp366->shared;_tmp36B=_tmp366->fnrgn;_LL1FD: {
# 1380
struct _RegionHandle*_tmp36C=_tmp36A->frgn;
const struct Cyc_Tcenv_Bindings*_tmp36D=_tmp367;
struct _dyneither_ptr*_tmp54E;const char*_tmp54D;void*_tmp54C[1];struct Cyc_String_pa_PrintArg_struct _tmp54B;struct Cyc_Absyn_Tvar*_tmp54A;struct Cyc_Absyn_Tvar*rgn0=
(_tmp54A=_cycalloc(sizeof(*_tmp54A)),((_tmp54A->name=((_tmp54E=_cycalloc(sizeof(*_tmp54E)),((_tmp54E[0]=(struct _dyneither_ptr)((_tmp54B.tag=0,((_tmp54B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp54C[0]=& _tmp54B,Cyc_aprintf(((_tmp54D="`%s",_tag_dyneither(_tmp54D,sizeof(char),4))),_tag_dyneither(_tmp54C,sizeof(void*),1)))))))),_tmp54E)))),((_tmp54A->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp54A->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp54A)))))));
# 1386
{struct Cyc_List_List*_tmp36E=fd->tvs;for(0;_tmp36E != 0;_tmp36E=_tmp36E->tl){
struct Cyc_Absyn_Kind*_tmp36F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp36E->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp370=_tmp36F;enum Cyc_Absyn_KindQual _tmp371;enum Cyc_Absyn_AliasQual _tmp372;_LL201: _tmp371=_tmp370->kind;_tmp372=_tmp370->aliasqual;_LL202:
 if(_tmp371 == Cyc_Absyn_RgnKind){
if(_tmp372 == Cyc_Absyn_Aliasable)
_tmp368=Cyc_RgnOrder_add_youngest(_tmp36C,_tmp368,(struct Cyc_Absyn_Tvar*)_tmp36E->hd,0,0);else{
# 1392
const char*_tmp551;void*_tmp550;(_tmp550=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp551="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp551,sizeof(char),39))),_tag_dyneither(_tmp550,sizeof(void*),0)));}}_LL200:;}}
# 1394
_tmp368=Cyc_RgnOrder_add_youngest(_tmp36C,_tmp368,rgn0,0,0);{
struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp375=(_tmp552=_cycalloc(sizeof(*_tmp552)),((_tmp552->hd=rgn0,((_tmp552->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp369),_tmp552)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp375);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp555;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp554;void*param_rgn=(void*)((_tmp554=_cycalloc(sizeof(*_tmp554)),((_tmp554[0]=((_tmp555.tag=2,((_tmp555.f1=rgn0,_tmp555)))),_tmp554))));
struct Cyc_List_List*_tmp376=0;
{struct Cyc_List_List*_tmp377=fd->args;for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
struct Cyc_Absyn_Vardecl _tmp55B;struct _tuple0*_tmp55A;struct Cyc_Absyn_Vardecl*_tmp559;struct Cyc_Absyn_Vardecl*_tmp378=(_tmp559=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp559[0]=(struct Cyc_Absyn_Vardecl)((_tmp55B.sc=Cyc_Absyn_Public,((_tmp55B.name=(
(_tmp55A=_cycalloc(sizeof(*_tmp55A)),((_tmp55A->f1=Cyc_Absyn_Loc_n,((_tmp55A->f2=(*((struct _tuple8*)_tmp377->hd)).f1,_tmp55A)))))),((_tmp55B.tq=(*((struct _tuple8*)_tmp377->hd)).f2,((_tmp55B.type=(*((struct _tuple8*)_tmp377->hd)).f3,((_tmp55B.initializer=0,((_tmp55B.rgn=param_rgn,((_tmp55B.attributes=0,((_tmp55B.escapes=0,_tmp55B)))))))))))))))),_tmp559)));
# 1407
{struct Cyc_List_List _tmp55E;struct Cyc_List_List*_tmp55D;_tmp376=((_tmp55D=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp55D[0]=(struct Cyc_List_List)((_tmp55E.hd=_tmp378,((_tmp55E.tl=_tmp376,_tmp55E)))),_tmp55D))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp561;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp560;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37B=(_tmp560=_cycalloc(sizeof(*_tmp560)),((_tmp560[0]=((_tmp561.tag=3,((_tmp561.f1=_tmp378,_tmp561)))),_tmp560)));
struct _dyneither_ptr*_tmp37C=(*((struct _tuple8*)_tmp377->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp562;_tmp36D=(const struct Cyc_Tcenv_Bindings*)((_tmp562=_region_malloc(_tmp36C,sizeof(*_tmp562)),((_tmp562->v=_tmp37C,((_tmp562->b=(void*)_tmp37B,((_tmp562->tl=_tmp36D,_tmp562))))))));};}}
# 1412
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp383=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp384=_tmp383;struct _dyneither_ptr*_tmp385;struct Cyc_Absyn_Tqual _tmp386;void*_tmp387;int _tmp388;_LL206: _tmp385=_tmp384.name;_tmp386=_tmp384.tq;_tmp387=_tmp384.type;_tmp388=_tmp384.inject;_LL207:
 if(_tmp385 != 0){
void*_tmp389=Cyc_Absyn_dyneither_typ(_tmp387,param_rgn,_tmp386,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp568;struct _tuple0*_tmp567;struct Cyc_Absyn_Vardecl*_tmp566;struct Cyc_Absyn_Vardecl*_tmp38A=(_tmp566=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp566[0]=(struct Cyc_Absyn_Vardecl)((_tmp568.sc=Cyc_Absyn_Public,((_tmp568.name=(
(_tmp567=_cycalloc(sizeof(*_tmp567)),((_tmp567->f1=Cyc_Absyn_Loc_n,((_tmp567->f2=_tmp385,_tmp567)))))),((_tmp568.tq=
Cyc_Absyn_empty_tqual(0),((_tmp568.type=_tmp389,((_tmp568.initializer=0,((_tmp568.rgn=param_rgn,((_tmp568.attributes=0,((_tmp568.escapes=0,_tmp568)))))))))))))))),_tmp566)));
# 1423
{struct Cyc_List_List*_tmp569;_tmp376=((_tmp569=_cycalloc(sizeof(*_tmp569)),((_tmp569->hd=_tmp38A,((_tmp569->tl=_tmp376,_tmp569))))));}{
struct _dyneither_ptr*_tmp38C=_tmp385;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp56C;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp56B;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp38D=(_tmp56B=_cycalloc(sizeof(*_tmp56B)),((_tmp56B[0]=((_tmp56C.tag=3,((_tmp56C.f1=_tmp38A,_tmp56C)))),_tmp56B)));
struct Cyc_Tcenv_Bindings*_tmp56D;_tmp36D=(const struct Cyc_Tcenv_Bindings*)((_tmp56D=_region_malloc(_tmp36C,sizeof(*_tmp56D)),((_tmp56D->v=_tmp38C,((_tmp56D->b=(void*)_tmp38D,((_tmp56D->tl=_tmp36D,_tmp56D))))))));};}else{
# 1428
const char*_tmp570;void*_tmp56F;(_tmp56F=0,Cyc_Tcutil_terr(loc,((_tmp570="missing name for varargs",_tag_dyneither(_tmp570,sizeof(char),25))),_tag_dyneither(_tmp56F,sizeof(void*),0)));}_LL205:;}
# 1430
{struct Cyc_Core_Opt _tmp573;struct Cyc_Core_Opt*_tmp572;fd->param_vardecls=((_tmp572=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp572[0]=(struct Cyc_Core_Opt)((_tmp573.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp376),_tmp573)),_tmp572))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp591;struct Cyc_List_List*_tmp590;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp58F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp58E;struct Cyc_List_List*_tmp58D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp58C;struct Cyc_Tcenv_CtrlEnv*_tmp58B;struct Cyc_Tcenv_SharedFenv*_tmp58A;struct Cyc_Tcenv_Fenv*_tmp589;return(_tmp589=_region_malloc(_tmp36C,sizeof(*_tmp589)),((_tmp589->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp58A=_region_malloc(_tmp36C,sizeof(*_tmp58A)),((_tmp58A->frgn=_tmp36C,((_tmp58A->return_typ=fd->ret_type,((_tmp58A->seen_labels=
# 1435
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp36C,Cyc_strptrcmp),((_tmp58A->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp36C,Cyc_strptrcmp),((_tmp58A->delayed_effect_checks=0,((_tmp58A->delayed_constraint_checks=0,_tmp58A)))))))))))))),((_tmp589->type_vars=(struct Cyc_List_List*)_tmp375,((_tmp589->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp368,((_tmp589->locals=(const struct Cyc_Tcenv_Bindings*)_tmp36D,((_tmp589->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp589->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1444
(_tmp58B=_region_malloc(_tmp36C,sizeof(*_tmp58B)),((_tmp58B->ctrl_rgn=_tmp36C,((_tmp58B->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp58B->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp58B->fallthru_clause=0,((_tmp58B->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp58B->try_depth=0,_tmp58B)))))))))))))),((_tmp589->capability=(void*)((void*)(
(_tmp591=_cycalloc(sizeof(*_tmp591)),((_tmp591[0]=((_tmp58C.tag=24,((_tmp58C.f1=((_tmp590=_cycalloc(sizeof(*_tmp590)),((_tmp590->hd=(void*)((_tmp58E=_cycalloc(sizeof(*_tmp58E)),((_tmp58E[0]=((_tmp58F.tag=23,((_tmp58F.f1=param_rgn,_tmp58F)))),_tmp58E)))),((_tmp590->tl=(
(_tmp58D=_cycalloc(sizeof(*_tmp58D)),((_tmp58D->hd=(void*)_check_null(fd->effect),((_tmp58D->tl=0,_tmp58D)))))),_tmp590)))))),_tmp58C)))),_tmp591))))),((_tmp589->curr_rgn=(void*)param_rgn,((_tmp589->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1448
((_tmp589->flags).in_notreadctxt=0,(((_tmp589->flags).in_lhs=0,(((_tmp589->flags).in_new=0,(((_tmp589->flags).abstract_ok=0,_tmp589->flags)))))))),((_tmp589->fnrgn=(struct _RegionHandle*)_tmp36B,_tmp589)))))))))))))))))))));};};};}_LL1FB:;}
