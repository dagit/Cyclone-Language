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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 287
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 386
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 391
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 446 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 462
typedef void*Cyc_Absyn_funcparams_t;
# 465
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 529
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 555
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 562
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 580
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 937 "absyn.h"
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
struct _tuple0*_tmp24=q;union Cyc_Absyn_Nmspace _tmp25;struct _dyneither_ptr*_tmp26;_LL10: _tmp25=_tmp24->f1;_tmp26=_tmp24->f2;_LL11:;{
union Cyc_Absyn_Nmspace _tmp27=_tmp25;struct Cyc_List_List*_tmp28;struct Cyc_List_List*_tmp29;struct _dyneither_ptr*_tmp2A;struct Cyc_List_List*_tmp2B;switch((_tmp27.C_n).tag){case 4: _LL13: _LL14:
 goto _LL16;case 1: if((_tmp27.Rel_n).val == 0){_LL15: _LL16:
# 324
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp26,is_use);}else{_LL17: _tmp2A=(struct _dyneither_ptr*)((_tmp27.Rel_n).val)->hd;_tmp2B=((_tmp27.Rel_n).val)->tl;_LL18: {
# 326
struct Cyc_Tcenv_Genv*_tmp2C=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2A,_tmp2B);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2C,_tmp26,is_use);}}case 3: _LL19: _tmp29=(_tmp27.C_n).val;_LL1A:
 _tmp28=_tmp29;goto _LL1C;default: _LL1B: _tmp28=(_tmp27.Abs_n).val;_LL1C:
# 330
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp28),_tmp26,is_use);}_LL12:;};}
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
struct _tuple0*_tmp40=q;union Cyc_Absyn_Nmspace _tmp41;struct _dyneither_ptr*_tmp42;_LL1E: _tmp41=_tmp40->f1;_tmp42=_tmp40->f2;_LL1F:;{
union Cyc_Absyn_Nmspace _tmp43=_tmp41;struct Cyc_List_List*_tmp44;struct Cyc_List_List*_tmp45;struct _dyneither_ptr*_tmp46;struct Cyc_List_List*_tmp47;switch((_tmp43.C_n).tag){case 4: _LL21: _LL22:
 goto _LL24;case 1: if((_tmp43.Rel_n).val == 0){_LL23: _LL24:
# 383
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp42,0);}else{_LL29: _tmp46=(struct _dyneither_ptr*)((_tmp43.Rel_n).val)->hd;_tmp47=((_tmp43.Rel_n).val)->tl;_LL2A: {
# 389
struct Cyc_Dict_Dict _tmp49=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp46,_tmp47))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp49,_tmp42);}}case 3: _LL25: _tmp45=(_tmp43.C_n).val;_LL26:
# 384
 _tmp44=_tmp45;goto _LL28;default: _LL27: _tmp44=(_tmp43.Abs_n).val;_LL28: {
# 386
struct Cyc_Dict_Dict _tmp48=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp44))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp48,_tmp42);}}_LL20:;};}
# 394
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4A=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4A,v);}
# 398
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp4B=q;union Cyc_Absyn_Nmspace _tmp4C;struct _dyneither_ptr*_tmp4D;_LL2C: _tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;_LL2D:;{
union Cyc_Absyn_Nmspace _tmp4E=_tmp4C;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp50;struct _dyneither_ptr*_tmp51;struct Cyc_List_List*_tmp52;switch((_tmp4E.C_n).tag){case 4: _LL2F: _LL30:
 goto _LL32;case 1: if((_tmp4E.Rel_n).val == 0){_LL31: _LL32:
# 403
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp4D,0);}else{_LL33: _tmp51=(struct _dyneither_ptr*)((_tmp4E.Rel_n).val)->hd;_tmp52=((_tmp4E.Rel_n).val)->tl;_LL34: {
# 405
struct Cyc_Dict_Dict _tmp53=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp51,_tmp52))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp53,_tmp4D);}}case 3: _LL35: _tmp50=(_tmp4E.C_n).val;_LL36:
 _tmp4F=_tmp50;goto _LL38;default: _LL37: _tmp4F=(_tmp4E.Abs_n).val;_LL38: {
# 409
struct Cyc_Dict_Dict _tmp54=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4F))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp54,_tmp4D);}}_LL2E:;};}
# 414
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 419
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp55=q;union Cyc_Absyn_Nmspace _tmp56;struct _dyneither_ptr*_tmp57;_LL3A: _tmp56=_tmp55->f1;_tmp57=_tmp55->f2;_LL3B:;{
union Cyc_Absyn_Nmspace _tmp58=_tmp56;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct _dyneither_ptr*_tmp5B;struct Cyc_List_List*_tmp5C;switch((_tmp58.C_n).tag){case 1: if((_tmp58.Rel_n).val == 0){_LL3D: _LL3E: {
# 424
struct _handler_cons _tmp5D;_push_handler(& _tmp5D);{int _tmp5F=0;if(setjmp(_tmp5D.handler))_tmp5F=1;if(!_tmp5F){{struct Cyc_Absyn_Datatypedecl***_tmp61=({struct Cyc_Absyn_Datatypedecl***_tmp60=_region_malloc(r,sizeof(*_tmp60));_tmp60[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp57,0);_tmp60;});_npop_handler(0);return _tmp61;};_pop_handler();}else{void*_tmp5E=(void*)_exn_thrown;void*_tmp62=_tmp5E;void*_tmp63;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp62)->tag == Cyc_Dict_Absent){_LL48: _LL49:
 return 0;}else{_LL4A: _tmp63=_tmp62;_LL4B:(int)_rethrow(_tmp63);}_LL47:;}};}}else{_LL41: _tmp5B=(struct _dyneither_ptr*)((_tmp58.Rel_n).val)->hd;_tmp5C=((_tmp58.Rel_n).val)->tl;_LL42: {
# 434
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp5B,_tmp5C);;_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp69=_tmp67;void*_tmp6A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp69)->tag == Cyc_Dict_Absent){_LL4D: _LL4E:
# 438
({void*_tmp6B=0;Cyc_Tcutil_terr(loc,({const char*_tmp6C="bad qualified name for @extensible datatype";_tag_dyneither(_tmp6C,sizeof(char),44);}),_tag_dyneither(_tmp6B,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp6D=_cycalloc_atomic(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Dict_Absent_exn_struct _tmp6E;_tmp6E.tag=Cyc_Dict_Absent;_tmp6E;});_tmp6D;}));}else{_LL4F: _tmp6A=_tmp69;_LL50:(int)_rethrow(_tmp6A);}_LL4C:;}};}{
# 441
struct Cyc_Dict_Dict _tmp6F=ge->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp70=_region_malloc(r,sizeof(*_tmp70));_tmp70[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp6F,_tmp57);_tmp70;});};}}case 4: _LL3F: _LL40:
# 427
({void*_tmp64=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp65="lookup_xdatatypedecl: impossible";_tag_dyneither(_tmp65,sizeof(char),33);}),_tag_dyneither(_tmp64,sizeof(void*),0));});
return 0;case 3: _LL43: _tmp5A=(_tmp58.C_n).val;_LL44:
# 443
 _tmp59=_tmp5A;goto _LL46;default: _LL45: _tmp59=(_tmp58.Abs_n).val;_LL46: {
# 445
struct Cyc_Dict_Dict _tmp71=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp59))->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp72=_region_malloc(r,sizeof(*_tmp72));_tmp72[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp71,_tmp57);_tmp72;});}}_LL3C:;};}
# 450
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp73=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp73,v);}
# 454
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp74=q;union Cyc_Absyn_Nmspace _tmp75;struct _dyneither_ptr*_tmp76;_LL52: _tmp75=_tmp74->f1;_tmp76=_tmp74->f2;_LL53:;{
union Cyc_Absyn_Nmspace _tmp77=_tmp75;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp79;struct _dyneither_ptr*_tmp7A;struct Cyc_List_List*_tmp7B;switch((_tmp77.C_n).tag){case 4: _LL55: _LL56:
 goto _LL58;case 1: if((_tmp77.Rel_n).val == 0){_LL57: _LL58:
# 459
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp76,0);}else{_LL59: _tmp7A=(struct _dyneither_ptr*)((_tmp77.Rel_n).val)->hd;_tmp7B=((_tmp77.Rel_n).val)->tl;_LL5A: {
# 461
struct Cyc_Dict_Dict _tmp7C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp7A,_tmp7B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp76);}}case 3: _LL5B: _tmp79=(_tmp77.C_n).val;_LL5C:
 _tmp78=_tmp79;goto _LL5E;default: _LL5D: _tmp78=(_tmp77.Abs_n).val;_LL5E: {
# 465
struct Cyc_Dict_Dict _tmp7D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp78))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7D,_tmp76);}}_LL54:;};}
# 470
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp7E=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,v);}
# 474
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp7F=q;union Cyc_Absyn_Nmspace _tmp80;struct _dyneither_ptr*_tmp81;_LL60: _tmp80=_tmp7F->f1;_tmp81=_tmp7F->f2;_LL61:;{
union Cyc_Absyn_Nmspace _tmp82=_tmp80;struct Cyc_List_List*_tmp83;struct Cyc_List_List*_tmp84;struct _dyneither_ptr*_tmp85;struct Cyc_List_List*_tmp86;switch((_tmp82.C_n).tag){case 4: _LL63: _LL64:
 goto _LL66;case 1: if((_tmp82.Rel_n).val == 0){_LL65: _LL66:
# 479
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp81,0);}else{_LL67: _tmp85=(struct _dyneither_ptr*)((_tmp82.Rel_n).val)->hd;_tmp86=((_tmp82.Rel_n).val)->tl;_LL68: {
# 481
struct Cyc_Dict_Dict _tmp87=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp85,_tmp86))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp87,_tmp81);}}case 3: _LL69: _tmp84=(_tmp82.C_n).val;_LL6A:
 _tmp83=_tmp84;goto _LL6C;default: _LL6B: _tmp83=(_tmp82.Abs_n).val;_LL6C: {
# 485
struct Cyc_Dict_Dict _tmp88=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp83))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp88,_tmp81);}}_LL62:;};}
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
struct Cyc_Tcenv_Fenv _tmp91=*f;struct Cyc_Tcenv_Fenv _tmp92=_tmp91;struct Cyc_Tcenv_SharedFenv*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_RgnOrder_RgnPO*_tmp95;const struct Cyc_Tcenv_Bindings*_tmp96;struct Cyc_Tcenv_CtrlEnv*_tmp97;void*_tmp98;void*_tmp99;struct Cyc_Tcenv_FenvFlags _tmp9A;struct _RegionHandle*_tmp9B;_LL71: _tmp93=_tmp92.shared;_tmp94=_tmp92.type_vars;_tmp95=_tmp92.region_order;_tmp96=_tmp92.locals;_tmp97=_tmp92.ctrl_env;_tmp98=_tmp92.capability;_tmp99=_tmp92.curr_rgn;_tmp9A=_tmp92.flags;_tmp9B=_tmp92.fnrgn;_LL72:;
# 520
return({struct Cyc_Tcenv_Fenv*_tmp9C=_region_malloc(l,sizeof(*_tmp9C));_tmp9C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp93;_tmp9C->type_vars=(struct Cyc_List_List*)_tmp94;_tmp9C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp95;_tmp9C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp96;_tmp9C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp97;_tmp9C->capability=(void*)_tmp98;_tmp9C->curr_rgn=(void*)_tmp99;_tmp9C->flags=(struct Cyc_Tcenv_FenvFlags)_tmp9A;_tmp9C->fnrgn=(struct _RegionHandle*)l;_tmp9C;});}
# 524
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 526
struct Cyc_Tcenv_Fenv _tmp9D=*f;struct Cyc_Tcenv_Fenv _tmp9E=_tmp9D;struct Cyc_Tcenv_SharedFenv*_tmp9F;struct Cyc_List_List*_tmpA0;struct Cyc_RgnOrder_RgnPO*_tmpA1;const struct Cyc_Tcenv_Bindings*_tmpA2;struct Cyc_Tcenv_CtrlEnv*_tmpA3;void*_tmpA4;void*_tmpA5;struct Cyc_Tcenv_FenvFlags _tmpA6;_LL74: _tmp9F=_tmp9E.shared;_tmpA0=_tmp9E.type_vars;_tmpA1=_tmp9E.region_order;_tmpA2=_tmp9E.locals;_tmpA3=_tmp9E.ctrl_env;_tmpA4=_tmp9E.capability;_tmpA5=_tmp9E.curr_rgn;_tmpA6=_tmp9E.flags;_LL75:;{
# 529
struct Cyc_Tcenv_CtrlEnv _tmpA7=*_tmpA3;struct Cyc_Tcenv_CtrlEnv _tmpA8=_tmpA7;const struct _tuple10*_tmpA9;_LL77: _tmpA9=_tmpA8.fallthru_clause;_LL78:;{
struct Cyc_Tcenv_CtrlEnv*_tmpAA=({struct Cyc_Tcenv_CtrlEnv*_tmpAC=_region_malloc(l,sizeof(*_tmpAC));_tmpAC->fallthru_clause=_tmpA9;_tmpAC;});
# 532
return({struct Cyc_Tcenv_Fenv*_tmpAB=_region_malloc(l,sizeof(*_tmpAB));_tmpAB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9F;_tmpAB->type_vars=(struct Cyc_List_List*)_tmpA0;_tmpAB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA1;_tmpAB->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA2;_tmpAB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpAA;_tmpAB->capability=(void*)_tmpA4;_tmpAB->curr_rgn=(void*)_tmpA5;_tmpAB->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA6;_tmpAB->fnrgn=(struct _RegionHandle*)l;_tmpAB;});};};}
# 538
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpAD=Cyc_Tcenv_get_fenv(te,({const char*_tmpB3="return_typ";_tag_dyneither(_tmpB3,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmpAE=_tmpAD;struct Cyc_Tcenv_SharedFenv*_tmpAF;_LL7A: _tmpAF=_tmpAE->shared;_LL7B:;{
struct Cyc_Tcenv_SharedFenv _tmpB0=*_tmpAF;struct Cyc_Tcenv_SharedFenv _tmpB1=_tmpB0;void*_tmpB2;_LL7D: _tmpB2=_tmpB1.return_typ;_LL7E:;
return _tmpB2;};}
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
struct Cyc_Tcenv_Fenv*_tmpBA=_tmpB9;struct Cyc_Tcenv_SharedFenv*_tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_RgnOrder_RgnPO*_tmpBD;const struct Cyc_Tcenv_Bindings*_tmpBE;struct Cyc_Tcenv_CtrlEnv*_tmpBF;void*_tmpC0;void*_tmpC1;struct Cyc_Tcenv_FenvFlags _tmpC2;_LL83: _tmpBB=_tmpBA->shared;_tmpBC=_tmpBA->type_vars;_tmpBD=_tmpBA->region_order;_tmpBE=_tmpBA->locals;_tmpBF=_tmpBA->ctrl_env;_tmpC0=_tmpBA->capability;_tmpC1=_tmpBA->curr_rgn;_tmpC2=_tmpBA->flags;_LL84:;
# 561
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpBC);
Cyc_Tcutil_check_unique_tvars(loc,_tmpC3);{
struct Cyc_Tcenv_Fenv*_tmpC4=({struct Cyc_Tcenv_Fenv*_tmpC5=_region_malloc(r,sizeof(*_tmpC5));_tmpC5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpBB;_tmpC5->type_vars=(struct Cyc_List_List*)_tmpC3;_tmpC5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpBD;_tmpC5->locals=(const struct Cyc_Tcenv_Bindings*)_tmpBE;_tmpC5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBF;_tmpC5->capability=(void*)_tmpC0;_tmpC5->curr_rgn=(void*)_tmpC1;_tmpC5->flags=(struct Cyc_Tcenv_FenvFlags)_tmpC2;_tmpC5->fnrgn=(struct _RegionHandle*)r;_tmpC5;});
# 566
return Cyc_Tcenv_put_fenv(r,te,_tmpC4);};};}
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
struct Cyc_Tcenv_Fenv*_tmpDE=_tmpDD;struct Cyc_Tcenv_SharedFenv*_tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_RgnOrder_RgnPO*_tmpE1;const struct Cyc_Tcenv_Bindings*_tmpE2;struct Cyc_Tcenv_CtrlEnv*_tmpE3;void*_tmpE4;void*_tmpE5;struct Cyc_Tcenv_FenvFlags _tmpE6;_LL9B: _tmpDF=_tmpDE->shared;_tmpE0=_tmpDE->type_vars;_tmpE1=_tmpDE->region_order;_tmpE2=_tmpDE->locals;_tmpE3=_tmpDE->ctrl_env;_tmpE4=_tmpDE->capability;_tmpE5=_tmpDE->curr_rgn;_tmpE6=_tmpDE->flags;_LL9C:;{
# 600
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE7=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpED;_tmpED.tag=4;_tmpED.f1=vd;_tmpED;});_tmpEC;});
struct Cyc_Tcenv_Bindings*_tmpE8=({struct Cyc_Tcenv_Bindings*_tmpEB=_region_malloc(r,sizeof(*_tmpEB));_tmpEB->v=_tmpC8;_tmpEB->b=(void*)_tmpE7;_tmpEB->tl=_tmpE2;_tmpEB;});
struct Cyc_Tcenv_Fenv*_tmpE9=({struct Cyc_Tcenv_Fenv*_tmpEA=_region_malloc(r,sizeof(*_tmpEA));_tmpEA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpDF;_tmpEA->type_vars=(struct Cyc_List_List*)_tmpE0;_tmpEA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE1;_tmpEA->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpE8);_tmpEA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE3;_tmpEA->capability=(void*)_tmpE4;_tmpEA->curr_rgn=(void*)_tmpE5;_tmpEA->flags=(struct Cyc_Tcenv_FenvFlags)_tmpE6;_tmpEA->fnrgn=(struct _RegionHandle*)r;_tmpEA;});
# 604
return Cyc_Tcenv_put_fenv(r,te,_tmpE9);};};}
# 607
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 609
struct Cyc_Tcenv_Fenv*_tmpEF=te->le;
if(_tmpEF == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpF0=*_tmpEF;struct Cyc_Tcenv_Fenv _tmpF1=_tmpF0;struct Cyc_Tcenv_SharedFenv*_tmpF2;struct Cyc_List_List*_tmpF3;struct Cyc_RgnOrder_RgnPO*_tmpF4;const struct Cyc_Tcenv_Bindings*_tmpF5;struct Cyc_Tcenv_CtrlEnv*_tmpF6;void*_tmpF7;void*_tmpF8;int _tmpF9;int _tmpFA;int _tmpFB;_LL9E: _tmpF2=_tmpF1.shared;_tmpF3=_tmpF1.type_vars;_tmpF4=_tmpF1.region_order;_tmpF5=_tmpF1.locals;_tmpF6=_tmpF1.ctrl_env;_tmpF7=_tmpF1.capability;_tmpF8=_tmpF1.curr_rgn;_tmpF9=(_tmpF1.flags).in_notreadctxt;_tmpFA=(_tmpF1.flags).in_lhs;_tmpFB=(_tmpF1.flags).abstract_ok;_LL9F:;{
# 614
struct Cyc_Tcenv_Fenv*_tmpFC=({struct Cyc_Tcenv_Fenv*_tmpFD=_region_malloc(r,sizeof(*_tmpFD));_tmpFD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF2;_tmpFD->type_vars=(struct Cyc_List_List*)_tmpF3;_tmpFD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF4;_tmpFD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpF5;_tmpFD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF6;_tmpFD->capability=(void*)_tmpF7;_tmpFD->curr_rgn=(void*)_tmpF8;_tmpFD->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpFD->flags).in_notreadctxt=_tmpF9;(_tmpFD->flags).in_lhs=_tmpFA;(_tmpFD->flags).in_new=(int)status;(_tmpFD->flags).abstract_ok=_tmpFB;_tmpFD->flags;});_tmpFD->fnrgn=(struct _RegionHandle*)r;_tmpFD;});
# 617
return Cyc_Tcenv_put_fenv(r,te,_tmpFC);};};}
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
struct Cyc_Tcenv_Fenv _tmp107=*_tmp106;struct Cyc_Tcenv_Fenv _tmp108=_tmp107;struct Cyc_Tcenv_SharedFenv*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_RgnOrder_RgnPO*_tmp10B;const struct Cyc_Tcenv_Bindings*_tmp10C;struct Cyc_Tcenv_CtrlEnv*_tmp10D;void*_tmp10E;void*_tmp10F;int _tmp110;int _tmp111;int _tmp112;_LLA7: _tmp109=_tmp108.shared;_tmp10A=_tmp108.type_vars;_tmp10B=_tmp108.region_order;_tmp10C=_tmp108.locals;_tmp10D=_tmp108.ctrl_env;_tmp10E=_tmp108.capability;_tmp10F=_tmp108.curr_rgn;_tmp110=(_tmp108.flags).in_notreadctxt;_tmp111=(_tmp108.flags).in_lhs;_tmp112=(_tmp108.flags).in_new;_LLA8:;{
# 641
struct Cyc_Tcenv_Fenv*_tmp113=({struct Cyc_Tcenv_Fenv*_tmp114=_region_malloc(r,sizeof(*_tmp114));_tmp114->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp109;_tmp114->type_vars=(struct Cyc_List_List*)_tmp10A;_tmp114->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp10B;_tmp114->locals=(const struct Cyc_Tcenv_Bindings*)_tmp10C;_tmp114->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp10D;_tmp114->capability=(void*)_tmp10E;_tmp114->curr_rgn=(void*)_tmp10F;_tmp114->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp114->flags).in_notreadctxt=_tmp110;(_tmp114->flags).in_lhs=_tmp111;(_tmp114->flags).in_new=_tmp112;(_tmp114->flags).abstract_ok=1;_tmp114->flags;});_tmp114->fnrgn=(struct _RegionHandle*)r;_tmp114;});
# 644
return Cyc_Tcenv_put_fenv(r,te,_tmp113);};};}
# 647
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 649
struct Cyc_Tcenv_Fenv*_tmp115=te->le;
if(_tmp115 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp116=*_tmp115;struct Cyc_Tcenv_Fenv _tmp117=_tmp116;struct Cyc_Tcenv_SharedFenv*_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_RgnOrder_RgnPO*_tmp11A;const struct Cyc_Tcenv_Bindings*_tmp11B;struct Cyc_Tcenv_CtrlEnv*_tmp11C;void*_tmp11D;void*_tmp11E;int _tmp11F;int _tmp120;int _tmp121;_LLAA: _tmp118=_tmp117.shared;_tmp119=_tmp117.type_vars;_tmp11A=_tmp117.region_order;_tmp11B=_tmp117.locals;_tmp11C=_tmp117.ctrl_env;_tmp11D=_tmp117.capability;_tmp11E=_tmp117.curr_rgn;_tmp11F=(_tmp117.flags).in_notreadctxt;_tmp120=(_tmp117.flags).in_lhs;_tmp121=(_tmp117.flags).in_new;_LLAB:;{
# 654
struct Cyc_Tcenv_Fenv*_tmp122=({struct Cyc_Tcenv_Fenv*_tmp123=_region_malloc(r,sizeof(*_tmp123));_tmp123->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp118;_tmp123->type_vars=(struct Cyc_List_List*)_tmp119;_tmp123->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp11A;_tmp123->locals=(const struct Cyc_Tcenv_Bindings*)_tmp11B;_tmp123->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp11C;_tmp123->capability=(void*)_tmp11D;_tmp123->curr_rgn=(void*)_tmp11E;_tmp123->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp123->flags).in_notreadctxt=_tmp11F;(_tmp123->flags).in_lhs=_tmp120;(_tmp123->flags).in_new=_tmp121;(_tmp123->flags).abstract_ok=0;_tmp123->flags;});_tmp123->fnrgn=(struct _RegionHandle*)r;_tmp123;});
# 657
return Cyc_Tcenv_put_fenv(r,te,_tmp122);};};}
# 660
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp124=te->le;
if(_tmp124 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp125=*_tmp124;struct Cyc_Tcenv_Fenv _tmp126=_tmp125;struct Cyc_Tcenv_SharedFenv*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_RgnOrder_RgnPO*_tmp129;const struct Cyc_Tcenv_Bindings*_tmp12A;struct Cyc_Tcenv_CtrlEnv*_tmp12B;void*_tmp12C;void*_tmp12D;int _tmp12E;int _tmp12F;int _tmp130;_LLAD: _tmp127=_tmp126.shared;_tmp128=_tmp126.type_vars;_tmp129=_tmp126.region_order;_tmp12A=_tmp126.locals;_tmp12B=_tmp126.ctrl_env;_tmp12C=_tmp126.capability;_tmp12D=_tmp126.curr_rgn;_tmp12E=(_tmp126.flags).in_lhs;_tmp12F=(_tmp126.flags).in_new;_tmp130=(_tmp126.flags).abstract_ok;_LLAE:;{
# 666
struct Cyc_Tcenv_Fenv*_tmp131=({struct Cyc_Tcenv_Fenv*_tmp132=_region_malloc(r,sizeof(*_tmp132));_tmp132->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp127;_tmp132->type_vars=(struct Cyc_List_List*)_tmp128;_tmp132->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp129;_tmp132->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12A;_tmp132->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12B;_tmp132->capability=(void*)_tmp12C;_tmp132->curr_rgn=(void*)_tmp12D;_tmp132->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp132->flags).in_notreadctxt=1;(_tmp132->flags).in_lhs=_tmp12E;(_tmp132->flags).in_new=_tmp12F;(_tmp132->flags).abstract_ok=_tmp130;_tmp132->flags;});_tmp132->fnrgn=(struct _RegionHandle*)r;_tmp132;});
# 669
return Cyc_Tcenv_put_fenv(r,te,_tmp131);};};}
# 672
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp133=te->le;
if(_tmp133 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp134=*_tmp133;struct Cyc_Tcenv_Fenv _tmp135=_tmp134;struct Cyc_Tcenv_SharedFenv*_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_RgnOrder_RgnPO*_tmp138;const struct Cyc_Tcenv_Bindings*_tmp139;struct Cyc_Tcenv_CtrlEnv*_tmp13A;void*_tmp13B;void*_tmp13C;int _tmp13D;int _tmp13E;int _tmp13F;_LLB0: _tmp136=_tmp135.shared;_tmp137=_tmp135.type_vars;_tmp138=_tmp135.region_order;_tmp139=_tmp135.locals;_tmp13A=_tmp135.ctrl_env;_tmp13B=_tmp135.capability;_tmp13C=_tmp135.curr_rgn;_tmp13D=(_tmp135.flags).in_lhs;_tmp13E=(_tmp135.flags).in_new;_tmp13F=(_tmp135.flags).abstract_ok;_LLB1:;{
# 678
struct Cyc_Tcenv_Fenv*_tmp140=({struct Cyc_Tcenv_Fenv*_tmp141=_region_malloc(r,sizeof(*_tmp141));_tmp141->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp136;_tmp141->type_vars=(struct Cyc_List_List*)_tmp137;_tmp141->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp138;_tmp141->locals=(const struct Cyc_Tcenv_Bindings*)_tmp139;_tmp141->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13A;_tmp141->capability=(void*)_tmp13B;_tmp141->curr_rgn=(void*)_tmp13C;_tmp141->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp141->flags).in_notreadctxt=0;(_tmp141->flags).in_lhs=_tmp13D;(_tmp141->flags).in_new=_tmp13E;(_tmp141->flags).abstract_ok=_tmp13F;_tmp141->flags;});_tmp141->fnrgn=(struct _RegionHandle*)r;_tmp141;});
# 681
return Cyc_Tcenv_put_fenv(r,te,_tmp140);};};}
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
struct Cyc_Tcenv_Fenv _tmp147=*_tmp146;struct Cyc_Tcenv_Fenv _tmp148=_tmp147;struct Cyc_Tcenv_SharedFenv*_tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_RgnOrder_RgnPO*_tmp14B;const struct Cyc_Tcenv_Bindings*_tmp14C;struct Cyc_Tcenv_CtrlEnv*_tmp14D;void*_tmp14E;void*_tmp14F;int _tmp150;int _tmp151;int _tmp152;_LLB6: _tmp149=_tmp148.shared;_tmp14A=_tmp148.type_vars;_tmp14B=_tmp148.region_order;_tmp14C=_tmp148.locals;_tmp14D=_tmp148.ctrl_env;_tmp14E=_tmp148.capability;_tmp14F=_tmp148.curr_rgn;_tmp150=(_tmp148.flags).in_notreadctxt;_tmp151=(_tmp148.flags).in_new;_tmp152=(_tmp148.flags).abstract_ok;_LLB7:;{
# 697
struct Cyc_Tcenv_Fenv*_tmp153=({struct Cyc_Tcenv_Fenv*_tmp154=_region_malloc(r,sizeof(*_tmp154));_tmp154->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp149;_tmp154->type_vars=(struct Cyc_List_List*)_tmp14A;_tmp154->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14B;_tmp154->locals=(const struct Cyc_Tcenv_Bindings*)_tmp14C;_tmp154->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14D;_tmp154->capability=(void*)_tmp14E;_tmp154->curr_rgn=(void*)_tmp14F;_tmp154->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp154->flags).in_notreadctxt=_tmp150;(_tmp154->flags).in_lhs=1;(_tmp154->flags).in_new=_tmp151;(_tmp154->flags).abstract_ok=_tmp152;_tmp154->flags;});_tmp154->fnrgn=(struct _RegionHandle*)r;_tmp154;});
# 700
return Cyc_Tcenv_put_fenv(r,te,_tmp153);};};}
# 703
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp155=te->le;
if(_tmp155 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp156=*_tmp155;struct Cyc_Tcenv_Fenv _tmp157=_tmp156;struct Cyc_Tcenv_SharedFenv*_tmp158;struct Cyc_List_List*_tmp159;struct Cyc_RgnOrder_RgnPO*_tmp15A;const struct Cyc_Tcenv_Bindings*_tmp15B;struct Cyc_Tcenv_CtrlEnv*_tmp15C;void*_tmp15D;void*_tmp15E;int _tmp15F;int _tmp160;int _tmp161;_LLB9: _tmp158=_tmp157.shared;_tmp159=_tmp157.type_vars;_tmp15A=_tmp157.region_order;_tmp15B=_tmp157.locals;_tmp15C=_tmp157.ctrl_env;_tmp15D=_tmp157.capability;_tmp15E=_tmp157.curr_rgn;_tmp15F=(_tmp157.flags).in_notreadctxt;_tmp160=(_tmp157.flags).in_new;_tmp161=(_tmp157.flags).abstract_ok;_LLBA:;{
# 709
struct Cyc_Tcenv_Fenv*_tmp162=({struct Cyc_Tcenv_Fenv*_tmp163=_region_malloc(r,sizeof(*_tmp163));_tmp163->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp158;_tmp163->type_vars=(struct Cyc_List_List*)_tmp159;_tmp163->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15A;_tmp163->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15B;_tmp163->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15C;_tmp163->capability=(void*)_tmp15D;_tmp163->curr_rgn=(void*)_tmp15E;_tmp163->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp163->flags).in_notreadctxt=_tmp15F;(_tmp163->flags).in_lhs=0;(_tmp163->flags).in_new=_tmp160;(_tmp163->flags).abstract_ok=_tmp161;_tmp163->flags;});_tmp163->fnrgn=(struct _RegionHandle*)r;_tmp163;});
# 712
return Cyc_Tcenv_put_fenv(r,te,_tmp162);};};}
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
struct Cyc_Tcenv_Fenv*_tmp179=_tmp178;struct Cyc_Tcenv_SharedFenv*_tmp17A;struct Cyc_List_List*_tmp17B;struct Cyc_RgnOrder_RgnPO*_tmp17C;const struct Cyc_Tcenv_Bindings*_tmp17D;struct Cyc_Tcenv_CtrlEnv*_tmp17E;void*_tmp17F;void*_tmp180;struct Cyc_Tcenv_FenvFlags _tmp181;_LLCF: _tmp17A=_tmp179->shared;_tmp17B=_tmp179->type_vars;_tmp17C=_tmp179->region_order;_tmp17D=_tmp179->locals;_tmp17E=_tmp179->ctrl_env;_tmp17F=_tmp179->capability;_tmp180=_tmp179->curr_rgn;_tmp181=_tmp179->flags;_LLD0:;{
# 743
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp182=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp188;_tmp188.tag=5;_tmp188.f1=vd;_tmp188;});_tmp187;});
struct Cyc_Tcenv_Bindings*_tmp183=({struct Cyc_Tcenv_Bindings*_tmp186=_region_malloc(r,sizeof(*_tmp186));_tmp186->v=_tmp177;_tmp186->b=(void*)_tmp182;_tmp186->tl=_tmp17D;_tmp186;});
struct Cyc_Tcenv_Fenv*_tmp184=({struct Cyc_Tcenv_Fenv*_tmp185=_region_malloc(r,sizeof(*_tmp185));_tmp185->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp17A;_tmp185->type_vars=(struct Cyc_List_List*)_tmp17B;_tmp185->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp17C;_tmp185->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp183);_tmp185->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp17E;_tmp185->capability=(void*)_tmp17F;_tmp185->curr_rgn=(void*)_tmp180;_tmp185->flags=(struct Cyc_Tcenv_FenvFlags)_tmp181;_tmp185->fnrgn=(struct _RegionHandle*)r;_tmp185;});
# 747
return Cyc_Tcenv_put_fenv(r,te,_tmp184);};};}
# 750
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp18A=te->le;
struct _tuple0*_tmp18B=q;union Cyc_Absyn_Nmspace _tmp18C;struct _dyneither_ptr*_tmp18D;_LLD2: _tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;_LLD3:;{
union Cyc_Absyn_Nmspace _tmp18E=_tmp18C;switch((_tmp18E.Rel_n).tag){case 4: _LLD5: _LLD6:
# 755
 if(_tmp18A == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp18F=_cycalloc_atomic(sizeof(*_tmp18F));_tmp18F[0]=({struct Cyc_Dict_Absent_exn_struct _tmp190;_tmp190.tag=Cyc_Dict_Absent;_tmp190;});_tmp18F;}));
goto _LLD8;case 1: if((_tmp18E.Rel_n).val == 0){_LLD7: if(_tmp18A != 0){_LLD8: {
# 758
struct Cyc_Tcenv_Fenv*_tmp191=_tmp18A;struct Cyc_Tcenv_Fenv*_tmp192=_tmp191;const struct Cyc_Tcenv_Bindings*_tmp193;_LLDC: _tmp193=_tmp192->locals;_LLDD:;{
# 760
struct _handler_cons _tmp194;_push_handler(& _tmp194);{int _tmp196=0;if(setjmp(_tmp194.handler))_tmp196=1;if(!_tmp196){{void*_tmp199=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp197=_region_malloc(r,sizeof(*_tmp197));_tmp197[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp198;_tmp198.tag=0;_tmp198.f1=Cyc_Tcenv_lookup_binding(_tmp193,_tmp18D);_tmp198;});_tmp197;});_npop_handler(0);return _tmp199;};_pop_handler();}else{void*_tmp195=(void*)_exn_thrown;void*_tmp19A=_tmp195;void*_tmp19B;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp19A)->tag == Cyc_Tcenv_NoBinding){_LLDF: _LLE0:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LLE1: _tmp19B=_tmp19A;_LLE2:(int)_rethrow(_tmp19B);}_LLDE:;}};};}}else{goto _LLD9;}}else{goto _LLD9;}default: _LLD9: _LLDA: {
# 763
struct _handler_cons _tmp19C;_push_handler(& _tmp19C);{int _tmp19E=0;if(setjmp(_tmp19C.handler))_tmp19E=1;if(!_tmp19E){{void*_tmp19F=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp19F;};_pop_handler();}else{void*_tmp19D=(void*)_exn_thrown;void*_tmp1A0=_tmp19D;void*_tmp1A1;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1A0)->tag == Cyc_Dict_Absent){_LLE4: _LLE5:
# 765
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp1A2=_region_malloc(r,sizeof(*_tmp1A2));_tmp1A2[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4[0]=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=q;_tmp1A5;});_tmp1A4;});_tmp1A3;});_tmp1A2;});}else{_LLE6: _tmp1A1=_tmp1A0;_LLE7:(int)_rethrow(_tmp1A1);}_LLE3:;}};}}_LLD4:;};}
# 771
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 773
struct Cyc_Tcenv_Fenv*_tmp1A6=Cyc_Tcenv_get_fenv(te,({const char*_tmp1AB="process_fallthru";_tag_dyneither(_tmp1AB,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp1A7=_tmp1A6;struct Cyc_Tcenv_CtrlEnv*_tmp1A8;_LLE9: _tmp1A8=_tmp1A7->ctrl_env;_LLEA:;{
const struct _tuple10*_tmp1A9=_tmp1A8->fallthru_clause;
if(_tmp1A9 != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=(*_tmp1A9).f1;_tmp1AA;});
# 778
return _tmp1A9;};}
# 781
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 785
struct Cyc_Tcenv_Fenv*_tmp1AC=
# 787
Cyc_Tcenv_get_fenv(te,({const char*_tmp1BF="set_fallthru";_tag_dyneither(_tmp1BF,sizeof(char),13);}));
# 785
struct Cyc_Tcenv_Fenv*_tmp1AD=_tmp1AC;struct Cyc_Tcenv_SharedFenv*_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_RgnOrder_RgnPO*_tmp1B0;const struct Cyc_Tcenv_Bindings*_tmp1B1;struct Cyc_Tcenv_CtrlEnv*_tmp1B2;void*_tmp1B3;void*_tmp1B4;struct Cyc_Tcenv_FenvFlags _tmp1B5;_LLEC: _tmp1AE=_tmp1AD->shared;_tmp1AF=_tmp1AD->type_vars;_tmp1B0=_tmp1AD->region_order;_tmp1B1=_tmp1AD->locals;_tmp1B2=_tmp1AD->ctrl_env;_tmp1B3=_tmp1AD->capability;_tmp1B4=_tmp1AD->curr_rgn;_tmp1B5=_tmp1AD->flags;_LLED:;{
# 788
struct Cyc_Tcenv_CtrlEnv*_tmp1B6=_tmp1B2;const struct _tuple10*_tmp1B7;_LLEF: _tmp1B7=_tmp1B6->fallthru_clause;_LLF0:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp1B8->tl=ft_typ;_tmp1B8;});}{
struct Cyc_List_List*_tmp1B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp1BA=({struct Cyc_Tcenv_CtrlEnv*_tmp1BD=_region_malloc(r,sizeof(*_tmp1BD));_tmp1BD->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->f1=clause;_tmp1BE->f2=new_tvs;_tmp1BE->f3=(const struct Cyc_Tcenv_CList*)_tmp1B9;_tmp1BE;});_tmp1BD;});
# 795
struct Cyc_Tcenv_Fenv*_tmp1BB=({struct Cyc_Tcenv_Fenv*_tmp1BC=_region_malloc(r,sizeof(*_tmp1BC));_tmp1BC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1AE;_tmp1BC->type_vars=(struct Cyc_List_List*)_tmp1AF;_tmp1BC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1B0;_tmp1BC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1B1;_tmp1BC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1BA;_tmp1BC->capability=(void*)_tmp1B3;_tmp1BC->curr_rgn=(void*)_tmp1B4;_tmp1BC->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1B5;_tmp1BC->fnrgn=(struct _RegionHandle*)r;_tmp1BC;});
# 799
return Cyc_Tcenv_put_fenv(r,te,_tmp1BB);};};};}
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
struct Cyc_Tcenv_Fenv _tmp1C4=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp1C5=_tmp1C4;struct Cyc_Tcenv_SharedFenv*_tmp1C6;struct Cyc_List_List*_tmp1C7;struct Cyc_RgnOrder_RgnPO*_tmp1C8;const struct Cyc_Tcenv_Bindings*_tmp1C9;struct Cyc_Tcenv_CtrlEnv*_tmp1CA;void*_tmp1CB;void*_tmp1CC;struct Cyc_Tcenv_FenvFlags _tmp1CD;_LLF5: _tmp1C6=_tmp1C5.shared;_tmp1C7=_tmp1C5.type_vars;_tmp1C8=_tmp1C5.region_order;_tmp1C9=_tmp1C5.locals;_tmp1CA=_tmp1C5.ctrl_env;_tmp1CB=_tmp1C5.capability;_tmp1CC=_tmp1C5.curr_rgn;_tmp1CD=_tmp1C5.flags;_LLF6:;{
struct Cyc_Tcenv_Fenv*_tmp1CE=({struct Cyc_Tcenv_Fenv*_tmp1D0=_region_malloc(r2,sizeof(*_tmp1D0));_tmp1D0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1C6;_tmp1D0->type_vars=(struct Cyc_List_List*)_tmp1C7;_tmp1D0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C8;_tmp1D0->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1C9;_tmp1D0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1CA;_tmp1D0->capability=(void*)_tmp1CB;_tmp1D0->curr_rgn=(void*)_tmp1CC;_tmp1D0->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1CD;_tmp1D0->fnrgn=(struct _RegionHandle*)r2;_tmp1D0;});
return({struct Cyc_Tcenv_Tenv*_tmp1CF=_region_malloc(r2,sizeof(*_tmp1CF));_tmp1CF->ns=te->ns;_tmp1CF->ae=te->ae;_tmp1CF->le=_tmp1CE;_tmp1CF->allow_valueof=1;_tmp1CF->in_extern_c_include=te->in_extern_c_include;_tmp1CF;});};};}
# 817
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp1D1=_region_malloc(r2,sizeof(*_tmp1D1));_tmp1D1->ns=te->ns;_tmp1D1->ae=te->ae;_tmp1D1->le=0;_tmp1D1->allow_valueof=te->allow_valueof;_tmp1D1->in_extern_c_include=1;_tmp1D1;});{
struct Cyc_Tcenv_Fenv _tmp1D2=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp1D3=_tmp1D2;struct Cyc_Tcenv_SharedFenv*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_RgnOrder_RgnPO*_tmp1D6;const struct Cyc_Tcenv_Bindings*_tmp1D7;struct Cyc_Tcenv_CtrlEnv*_tmp1D8;void*_tmp1D9;void*_tmp1DA;struct Cyc_Tcenv_FenvFlags _tmp1DB;_LLF8: _tmp1D4=_tmp1D3.shared;_tmp1D5=_tmp1D3.type_vars;_tmp1D6=_tmp1D3.region_order;_tmp1D7=_tmp1D3.locals;_tmp1D8=_tmp1D3.ctrl_env;_tmp1D9=_tmp1D3.capability;_tmp1DA=_tmp1D3.curr_rgn;_tmp1DB=_tmp1D3.flags;_LLF9:;{
struct Cyc_Tcenv_Fenv*_tmp1DC=({struct Cyc_Tcenv_Fenv*_tmp1DE=_region_malloc(r2,sizeof(*_tmp1DE));_tmp1DE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1D4;_tmp1DE->type_vars=(struct Cyc_List_List*)_tmp1D5;_tmp1DE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1D6;_tmp1DE->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1D7;_tmp1DE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1D8;_tmp1DE->capability=(void*)_tmp1D9;_tmp1DE->curr_rgn=(void*)_tmp1DA;_tmp1DE->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1DB;_tmp1DE->fnrgn=(struct _RegionHandle*)r2;_tmp1DE;});
return({struct Cyc_Tcenv_Tenv*_tmp1DD=_region_malloc(r2,sizeof(*_tmp1DD));_tmp1DD->ns=te->ns;_tmp1DD->ae=te->ae;_tmp1DD->le=_tmp1DC;_tmp1DD->allow_valueof=te->allow_valueof;_tmp1DD->in_extern_c_include=1;_tmp1DD;});};};}
# 825
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 828
struct Cyc_Tcenv_Fenv _tmp1DF=*
# 830
Cyc_Tcenv_get_fenv(te,({const char*_tmp1F4="add_region";_tag_dyneither(_tmp1F4,sizeof(char),11);}));
# 828
struct Cyc_Tcenv_Fenv _tmp1E0=_tmp1DF;struct Cyc_Tcenv_SharedFenv*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_RgnOrder_RgnPO*_tmp1E3;const struct Cyc_Tcenv_Bindings*_tmp1E4;struct Cyc_Tcenv_CtrlEnv*_tmp1E5;void*_tmp1E6;void*_tmp1E7;struct Cyc_Tcenv_FenvFlags _tmp1E8;_LLFB: _tmp1E1=_tmp1E0.shared;_tmp1E2=_tmp1E0.type_vars;_tmp1E3=_tmp1E0.region_order;_tmp1E4=_tmp1E0.locals;_tmp1E5=_tmp1E0.ctrl_env;_tmp1E6=_tmp1E0.capability;_tmp1E7=_tmp1E0.curr_rgn;_tmp1E8=_tmp1E0.flags;_LLFC:;
# 831
{void*_tmp1E9=Cyc_Tcutil_compress(rgn);void*_tmp1EA=_tmp1E9;struct Cyc_Absyn_Tvar*_tmp1EB;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EA)->tag == 2){_LLFE: _tmp1EB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EA)->f1;_LLFF:
# 833
 _tmp1E3=Cyc_RgnOrder_add_youngest(_tmp1E1->frgn,_tmp1E3,_tmp1EB,resetable,opened);
# 835
goto _LLFD;}else{_LL100: _LL101:
 goto _LLFD;}_LLFD:;}
# 838
_tmp1E6=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1ED;_tmp1ED.tag=24;_tmp1ED.f1=({struct Cyc_List_List*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1F1;_tmp1F1.tag=23;_tmp1F1.f1=rgn;_tmp1F1;});_tmp1F0;});_tmp1EE->tl=({struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->hd=_tmp1E6;_tmp1EF->tl=0;_tmp1EF;});_tmp1EE;});_tmp1ED;});_tmp1EC;});{
# 840
struct Cyc_Tcenv_Fenv*_tmp1F2=({struct Cyc_Tcenv_Fenv*_tmp1F3=_region_malloc(r,sizeof(*_tmp1F3));_tmp1F3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1E1;_tmp1F3->type_vars=(struct Cyc_List_List*)_tmp1E2;_tmp1F3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E3;_tmp1F3->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1E4;_tmp1F3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1E5;_tmp1F3->capability=(void*)_tmp1E6;_tmp1F3->curr_rgn=(void*)_tmp1E7;_tmp1F3->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1E8;_tmp1F3->fnrgn=(struct _RegionHandle*)r;_tmp1F3;});
# 843
return Cyc_Tcenv_put_fenv(r,te,_tmp1F2);};}
# 846
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 848
struct Cyc_Tcenv_Fenv _tmp1F5=*
# 850
Cyc_Tcenv_get_fenv(te,({const char*_tmp20B="new_named_block";_tag_dyneither(_tmp20B,sizeof(char),16);}));
# 848
struct Cyc_Tcenv_Fenv _tmp1F6=_tmp1F5;struct Cyc_Tcenv_SharedFenv*_tmp1F7;struct Cyc_List_List*_tmp1F8;struct Cyc_RgnOrder_RgnPO*_tmp1F9;const struct Cyc_Tcenv_Bindings*_tmp1FA;struct Cyc_Tcenv_CtrlEnv*_tmp1FB;void*_tmp1FC;void*_tmp1FD;struct Cyc_Tcenv_FenvFlags _tmp1FE;_LL103: _tmp1F7=_tmp1F6.shared;_tmp1F8=_tmp1F6.type_vars;_tmp1F9=_tmp1F6.region_order;_tmp1FA=_tmp1F6.locals;_tmp1FB=_tmp1F6.ctrl_env;_tmp1FC=_tmp1F6.capability;_tmp1FD=_tmp1F6.curr_rgn;_tmp1FE=_tmp1F6.flags;_LL104:;{
# 852
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp20A="new_block";_tag_dyneither(_tmp20A,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp209;_tmp209.tag=2;_tmp209.f1=block_rgn;_tmp209;});_tmp208;});
_tmp1F8=({struct Cyc_List_List*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->hd=block_rgn;_tmp1FF->tl=_tmp1F8;_tmp1FF;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp1F8);
_tmp1F9=Cyc_RgnOrder_add_youngest(_tmp1F7->frgn,_tmp1F9,block_rgn,0,0);
_tmp1FC=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp201;_tmp201.tag=24;_tmp201.f1=({struct Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp205;_tmp205.tag=23;_tmp205.f1=block_typ;_tmp205;});_tmp204;});_tmp202->tl=({struct Cyc_List_List*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->hd=_tmp1FC;_tmp203->tl=0;_tmp203;});_tmp202;});_tmp201;});_tmp200;});
_tmp1FD=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp206=({struct Cyc_Tcenv_Fenv*_tmp207=_region_malloc(r,sizeof(*_tmp207));_tmp207->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F7;_tmp207->type_vars=(struct Cyc_List_List*)_tmp1F8;_tmp207->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1F9;_tmp207->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FA;_tmp207->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1FB;_tmp207->capability=(void*)_tmp1FC;_tmp207->curr_rgn=(void*)_tmp1FD;_tmp207->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1FE;_tmp207->fnrgn=(struct _RegionHandle*)r;_tmp207;});
# 862
return Cyc_Tcenv_put_fenv(r,te,_tmp206);};};}
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
struct Cyc_Tcenv_Fenv _tmp20E=_tmp20D;struct Cyc_Tcenv_SharedFenv*_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_RgnOrder_RgnPO*_tmp211;const struct Cyc_Tcenv_Bindings*_tmp212;struct Cyc_Tcenv_CtrlEnv*_tmp213;void*_tmp214;void*_tmp215;struct Cyc_Tcenv_FenvFlags _tmp216;_LL106: _tmp20F=_tmp20E.shared;_tmp210=_tmp20E.type_vars;_tmp211=_tmp20E.region_order;_tmp212=_tmp20E.locals;_tmp213=_tmp20E.ctrl_env;_tmp214=_tmp20E.capability;_tmp215=_tmp20E.curr_rgn;_tmp216=_tmp20E.flags;_LL107:;
# 882
for(0;cs != 0;cs=cs->tl){
_tmp211=
Cyc_RgnOrder_add_outlives_constraint(_tmp20F->frgn,_tmp211,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 887
struct Cyc_Tcenv_Fenv*_tmp217=({struct Cyc_Tcenv_Fenv*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp20F;_tmp218->type_vars=(struct Cyc_List_List*)_tmp210;_tmp218->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp211;_tmp218->locals=(const struct Cyc_Tcenv_Bindings*)_tmp212;_tmp218->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp213;_tmp218->capability=(void*)_tmp214;_tmp218->curr_rgn=(void*)_tmp215;_tmp218->flags=(struct Cyc_Tcenv_FenvFlags)_tmp216;_tmp218->fnrgn=(struct _RegionHandle*)r;_tmp218;});
# 890
return Cyc_Tcenv_put_fenv(r,te,_tmp217);};}
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
struct _tuple11 _tmp224=Cyc_Tcutil_swap_kind(_tmp21B,Cyc_Tcutil_kind_to_bound(_tmp21C));struct _tuple11 _tmp225=_tmp224;struct Cyc_Absyn_Tvar*_tmp226;void*_tmp227;_LL109: _tmp226=_tmp225.f1;_tmp227=_tmp225.f2;_LL10A:;
*oldtv=({struct _tuple11*_tmp228=_region_malloc(r,sizeof(*_tmp228));_tmp228->f1=_tmp226;_tmp228->f2=_tmp227;_tmp228;});}else{
# 918
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp229=Cyc_Tcutil_swap_kind(_tmp21A,Cyc_Tcutil_kind_to_bound(_tmp21D));struct _tuple11 _tmp22A=_tmp229;struct Cyc_Absyn_Tvar*_tmp22B;void*_tmp22C;_LL10C: _tmp22B=_tmp22A.f1;_tmp22C=_tmp22A.f2;_LL10D:;
*oldtv=({struct _tuple11*_tmp22D=_region_malloc(r,sizeof(*_tmp22D));_tmp22D->f1=_tmp22B;_tmp22D->f2=_tmp22C;_tmp22D;});}}}{
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
struct Cyc_Tcenv_Fenv*_tmp23B=fe;void*_tmp23C;struct Cyc_RgnOrder_RgnPO*_tmp23D;_LL112: _tmp23C=_tmp23B->capability;_tmp23D=_tmp23B->region_order;_LL113:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp23C) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp23C))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp23D,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp23F;_tmp23F.tag=23;_tmp23F.f1=rgn;_tmp23F;});_tmp23E;}),_tmp23C))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp242;_tmp242.tag=0;_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp240[1]={& _tmp242};Cyc_Tcutil_terr(loc,({const char*_tmp241="Expression accesses unavailable region %s";_tag_dyneither(_tmp241,sizeof(char),42);}),_tag_dyneither(_tmp240,sizeof(void*),1));});});}
# 961
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp244=
Cyc_Tcenv_get_fenv(te,({const char*_tmp24F="check_rgn_resetable";_tag_dyneither(_tmp24F,sizeof(char),20);}));
# 963
struct Cyc_Tcenv_Fenv*_tmp245=_tmp244;struct Cyc_RgnOrder_RgnPO*_tmp246;_LL115: _tmp246=_tmp245->region_order;_LL116:;{
# 965
void*_tmp247=Cyc_Tcutil_compress(rgn);void*_tmp248=_tmp247;struct Cyc_Absyn_Tvar*_tmp249;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp248)->tag == 2){_LL118: _tmp249=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp248)->f1;_LL119:
# 967
 if(!Cyc_RgnOrder_is_region_resetable(_tmp246,_tmp249))
({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0;_tmp24C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp24A[1]={& _tmp24C};Cyc_Tcutil_terr(loc,({const char*_tmp24B="Region %s is not resetable";_tag_dyneither(_tmp24B,sizeof(char),27);}),_tag_dyneither(_tmp24A,sizeof(void*),1));});});
return;}else{_LL11A: _LL11B:
({void*_tmp24D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24E="check_rgn_resetable";_tag_dyneither(_tmp24E,sizeof(char),20);}),_tag_dyneither(_tmp24D,sizeof(void*),0));});}_LL117:;};};}
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
struct Cyc_Tcenv_Fenv*_tmp252=fe;struct Cyc_RgnOrder_RgnPO*_tmp253;_LL126: _tmp253=_tmp252->region_order;_LL127:;{
int _tmp254=Cyc_RgnOrder_effect_outlives(_tmp253,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp256;_tmp256.tag=23;_tmp256.f1=rt_a;_tmp256;});_tmp255;}),rt_b);
# 996
return _tmp254;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1001
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp257=
Cyc_Tcenv_get_fenv(te,({const char*_tmp25E="check_effect_accessible";_tag_dyneither(_tmp25E,sizeof(char),24);}));
# 1002
struct Cyc_Tcenv_Fenv*_tmp258=_tmp257;void*_tmp259;struct Cyc_RgnOrder_RgnPO*_tmp25A;struct Cyc_Tcenv_SharedFenv*_tmp25B;_LL129: _tmp259=_tmp258->capability;_tmp25A=_tmp258->region_order;_tmp25B=_tmp258->shared;_LL12A:;
# 1004
if(Cyc_Tcutil_subset_effect(0,eff,_tmp259))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp25A,eff,_tmp259))
return;{
struct _RegionHandle*frgn=_tmp25B->frgn;
_tmp25B->delayed_effect_checks=({struct Cyc_List_List*_tmp25C=_region_malloc(frgn,sizeof(*_tmp25C));_tmp25C->hd=({struct _tuple14*_tmp25D=_region_malloc(frgn,sizeof(*_tmp25D));_tmp25D->f1=_tmp259;_tmp25D->f2=eff;_tmp25D->f3=_tmp25A;_tmp25D->f4=loc;_tmp25D;});_tmp25C->tl=_tmp25B->delayed_effect_checks;_tmp25C;});};}
# 1013
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25F=
Cyc_Tcenv_get_fenv(te,({const char*_tmp26D="check_delayed_effects";_tag_dyneither(_tmp26D,sizeof(char),22);}));
# 1014
struct Cyc_Tcenv_Fenv*_tmp260=_tmp25F;struct Cyc_Tcenv_SharedFenv*_tmp261;_LL12C: _tmp261=_tmp260->shared;_LL12D:;{
# 1016
struct Cyc_List_List*_tmp262=_tmp261->delayed_effect_checks;
for(0;_tmp262 != 0;_tmp262=_tmp262->tl){
struct _tuple14*_tmp263=(struct _tuple14*)_tmp262->hd;struct _tuple14*_tmp264=_tmp263;void*_tmp265;void*_tmp266;struct Cyc_RgnOrder_RgnPO*_tmp267;unsigned int _tmp268;_LL12F: _tmp265=_tmp264->f1;_tmp266=_tmp264->f2;_tmp267=_tmp264->f3;_tmp268=_tmp264->f4;_LL130:;
if(Cyc_Tcutil_subset_effect(1,_tmp266,_tmp265))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp267,_tmp266,_tmp265))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp266));({struct Cyc_String_pa_PrintArg_struct _tmp26B;_tmp26B.tag=0;_tmp26B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp265));({void*_tmp269[2]={& _tmp26B,& _tmp26C};Cyc_Tcutil_terr(_tmp268,({const char*_tmp26A="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp26A,sizeof(char),51);}),_tag_dyneither(_tmp269,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
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
struct Cyc_Tcenv_Fenv*_tmp273=_tmp26E;struct Cyc_Tcenv_Fenv*_tmp274=_tmp273;struct Cyc_RgnOrder_RgnPO*_tmp275;struct Cyc_Tcenv_SharedFenv*_tmp276;_LL132: _tmp275=_tmp274->region_order;_tmp276=_tmp274->shared;_LL133:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp275,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp276->delayed_constraint_checks=({struct Cyc_List_List*_tmp277=_region_malloc(_tmp276->frgn,sizeof(*_tmp277));_tmp277->hd=({struct _tuple15*_tmp278=_region_malloc(_tmp276->frgn,sizeof(*_tmp278));_tmp278->f1=_tmp275;_tmp278->f2=po;_tmp278->f3=loc;_tmp278;});_tmp277->tl=_tmp276->delayed_constraint_checks;_tmp277;});};}
# 1050
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp279=
Cyc_Tcenv_get_fenv(te,({const char*_tmp284="check_delayed_constraints";_tag_dyneither(_tmp284,sizeof(char),26);}));
# 1051
struct Cyc_Tcenv_Fenv*_tmp27A=_tmp279;struct Cyc_Tcenv_SharedFenv*_tmp27B;_LL135: _tmp27B=_tmp27A->shared;_LL136:;{
# 1053
struct Cyc_List_List*_tmp27C=_tmp27B->delayed_constraint_checks;
for(0;_tmp27C != 0;_tmp27C=_tmp27C->tl){
struct _tuple15*_tmp27D=(struct _tuple15*)_tmp27C->hd;struct _tuple15*_tmp27E=_tmp27D;struct Cyc_RgnOrder_RgnPO*_tmp27F;struct Cyc_List_List*_tmp280;unsigned int _tmp281;_LL138: _tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;_tmp281=_tmp27E->f3;_LL139:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp27F,_tmp280,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp282=0;Cyc_Tcutil_terr(_tmp281,({const char*_tmp283="the required region ordering is not satisfied here";_tag_dyneither(_tmp283,sizeof(char),51);}),_tag_dyneither(_tmp282,sizeof(void*),0));});}};}
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
struct Cyc_Absyn_VarargInfo _tmp294=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp295=_tmp294;struct _dyneither_ptr*_tmp296;struct Cyc_Absyn_Tqual _tmp297;void*_tmp298;int _tmp299;_LL13B: _tmp296=_tmp295.name;_tmp297=_tmp295.tq;_tmp298=_tmp295.type;_tmp299=_tmp295.inject;_LL13C:;
if(_tmp296 != 0){
void*_tmp29A=Cyc_Absyn_dyneither_typ(_tmp298,param_rgn,_tmp297,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp29B=({struct Cyc_Absyn_Vardecl*_tmp2A2=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2A2[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2A3;_tmp2A3.sc=Cyc_Absyn_Public;_tmp2A3.name=({struct _tuple0*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4->f1=Cyc_Absyn_Loc_n;_tmp2A4->f2=_tmp296;_tmp2A4;});_tmp2A3.tq=
# 1092
Cyc_Absyn_empty_tqual(0);_tmp2A3.type=_tmp29A;_tmp2A3.initializer=0;_tmp2A3.rgn=param_rgn;_tmp2A3.attributes=0;_tmp2A3.escapes=0;_tmp2A3;});_tmp2A2;});
# 1097
_tmp287=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->hd=_tmp29B;_tmp29C->tl=_tmp287;_tmp29C;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp29D=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2A1;_tmp2A1.tag=3;_tmp2A1.f1=_tmp29B;_tmp2A1;});_tmp2A0;});
struct _dyneither_ptr*_tmp29E=_tmp296;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp29F=_region_malloc(r,sizeof(*_tmp29F));_tmp29F->v=_tmp29E;_tmp29F->b=(void*)_tmp29D;_tmp29F->tl=locals;_tmp29F;});};}else{
# 1102
({void*_tmp2A5=0;Cyc_Tcutil_terr(loc,({const char*_tmp2A6="missing name for varargs";_tag_dyneither(_tmp2A6,sizeof(char),25);}),_tag_dyneither(_tmp2A5,sizeof(void*),0));});}}
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
struct _tuple8 _tmp2BD=*((struct _tuple8*)args->hd);struct _tuple8 _tmp2BE=_tmp2BD;struct _dyneither_ptr*_tmp2BF;struct Cyc_Absyn_Tqual _tmp2C0;void*_tmp2C1;_LL13E: _tmp2BF=_tmp2BE.f1;_tmp2C0=_tmp2BE.f2;_tmp2C1=_tmp2BE.f3;_LL13F:;
if(_tmp2BF != 0){
struct Cyc_Absyn_Vardecl*_tmp2C2=({struct Cyc_Absyn_Vardecl*_tmp2CA=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2CA[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2CB;_tmp2CB.sc=Cyc_Absyn_Public;_tmp2CB.name=({struct _tuple0*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->f1=Cyc_Absyn_Loc_n;_tmp2CC->f2=_tmp2BF;_tmp2CC;});_tmp2CB.tq=_tmp2C0;_tmp2CB.type=_tmp2C1;_tmp2CB.initializer=0;_tmp2CB.rgn=param_rgn;_tmp2CB.attributes=0;_tmp2CB.escapes=0;_tmp2CB;});_tmp2CA;});
# 1143
_tmp2BC=({struct Cyc_List_List*_tmp2C3=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp2C3[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp2C4;_tmp2C4.hd=_tmp2C2;_tmp2C4.tl=_tmp2BC;_tmp2C4;});_tmp2C3;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2C5=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2C9;_tmp2C9.tag=3;_tmp2C9.f1=_tmp2C2;_tmp2C9;});_tmp2C8;});
struct _dyneither_ptr*_tmp2C6=_tmp2BF;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2C7=_region_malloc(r,sizeof(*_tmp2C7));_tmp2C7->v=_tmp2C6;_tmp2C7->b=(void*)_tmp2C5;_tmp2C7->tl=locals;_tmp2C7;});};}}
# 1149
return({struct Cyc_Tcenv_Fenv*_tmp2CD=_region_malloc(r,sizeof(*_tmp2CD));_tmp2CD->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp2D4=_region_malloc(r,sizeof(*_tmp2D4));_tmp2D4->frgn=r;_tmp2D4->return_typ=ret_type;_tmp2D4->delayed_effect_checks=0;_tmp2D4->delayed_constraint_checks=0;_tmp2D4;});_tmp2CD->type_vars=(struct Cyc_List_List*)_tmp2BA;_tmp2CD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2BB;_tmp2CD->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp2CD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2D3=_region_malloc(r,sizeof(*_tmp2D3));_tmp2D3->fallthru_clause=0;_tmp2D3;});_tmp2CD->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2CF;_tmp2CF.tag=24;_tmp2CF.f1=({struct Cyc_List_List*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2D2;_tmp2D2.tag=23;_tmp2D2.f1=param_rgn;_tmp2D2;});_tmp2D1;});_tmp2D0->tl=0;_tmp2D0;});_tmp2CF;});_tmp2CE;}));_tmp2CD->curr_rgn=(void*)param_rgn;_tmp2CD->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp2CD->flags).in_notreadctxt=0;(_tmp2CD->flags).in_lhs=0;(_tmp2CD->flags).in_new=0;(_tmp2CD->flags).abstract_ok=0;_tmp2CD->flags;});_tmp2CD->fnrgn=(struct _RegionHandle*)r;_tmp2CD;});}
# 1167
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp2DC=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp2DD;struct Cyc_RgnOrder_RgnPO*_tmp2DE;struct Cyc_List_List*_tmp2DF;struct Cyc_Tcenv_SharedFenv*_tmp2E0;struct _RegionHandle*_tmp2E1;_LL141: _tmp2DD=_tmp2DC->locals;_tmp2DE=_tmp2DC->region_order;_tmp2DF=_tmp2DC->type_vars;_tmp2E0=_tmp2DC->shared;_tmp2E1=_tmp2DC->fnrgn;_LL142:;{
# 1170
struct _RegionHandle*_tmp2E2=_tmp2E0->frgn;
const struct Cyc_Tcenv_Bindings*_tmp2E3=_tmp2DD;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A->name=({struct _dyneither_ptr*_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31E;_tmp31E.tag=0;_tmp31E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp31C[1]={& _tmp31E};Cyc_aprintf(({const char*_tmp31D="`%s";_tag_dyneither(_tmp31D,sizeof(char),4);}),_tag_dyneither(_tmp31C,sizeof(void*),1));});});_tmp31B;});_tmp31A->identity=
# 1174
Cyc_Tcutil_new_tvar_id();_tmp31A->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp31A;});
# 1176
{struct Cyc_List_List*_tmp2E4=fd->tvs;for(0;_tmp2E4 != 0;_tmp2E4=_tmp2E4->tl){
struct Cyc_Absyn_Kind*_tmp2E5=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2E4->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp2E6=_tmp2E5;enum Cyc_Absyn_KindQual _tmp2E7;enum Cyc_Absyn_AliasQual _tmp2E8;_LL144: _tmp2E7=_tmp2E6->kind;_tmp2E8=_tmp2E6->aliasqual;_LL145:;
if(_tmp2E7 == Cyc_Absyn_RgnKind){
if(_tmp2E8 == Cyc_Absyn_Aliasable)
_tmp2DE=Cyc_RgnOrder_add_youngest(_tmp2E2,_tmp2DE,(struct Cyc_Absyn_Tvar*)_tmp2E4->hd,0,0);else{
# 1182
({void*_tmp2E9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2EA="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp2EA,sizeof(char),39);}),_tag_dyneither(_tmp2E9,sizeof(void*),0));});}}}}
# 1184
_tmp2DE=Cyc_RgnOrder_add_youngest(_tmp2E2,_tmp2DE,rgn0,0,0);{
struct Cyc_List_List*_tmp2EB=({struct Cyc_List_List*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->hd=rgn0;_tmp319->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp2DF);_tmp319;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp2EB);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp318;_tmp318.tag=2;_tmp318.f1=rgn0;_tmp318;});_tmp317;});
struct Cyc_List_List*_tmp2EC=0;
{struct Cyc_List_List*_tmp2ED=fd->args;for(0;_tmp2ED != 0;_tmp2ED=_tmp2ED->tl){
struct Cyc_Absyn_Vardecl*_tmp2EE=({struct Cyc_Absyn_Vardecl*_tmp2F6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp2F6[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp2F7;_tmp2F7.sc=Cyc_Absyn_Public;_tmp2F7.name=({struct _tuple0*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->f1=Cyc_Absyn_Loc_n;_tmp2F8->f2=(*((struct _tuple8*)_tmp2ED->hd)).f1;_tmp2F8;});_tmp2F7.tq=(*((struct _tuple8*)_tmp2ED->hd)).f2;_tmp2F7.type=(*((struct _tuple8*)_tmp2ED->hd)).f3;_tmp2F7.initializer=0;_tmp2F7.rgn=param_rgn;_tmp2F7.attributes=0;_tmp2F7.escapes=0;_tmp2F7;});_tmp2F6;});
# 1197
_tmp2EC=({struct Cyc_List_List*_tmp2EF=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp2EF[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp2F0;_tmp2F0.hd=_tmp2EE;_tmp2F0.tl=_tmp2EC;_tmp2F0;});_tmp2EF;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2F1=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp2F5;_tmp2F5.tag=3;_tmp2F5.f1=_tmp2EE;_tmp2F5;});_tmp2F4;});
struct _dyneither_ptr*_tmp2F2=(*((struct _tuple8*)_tmp2ED->hd)).f1;
_tmp2E3=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2F3=_region_malloc(_tmp2E2,sizeof(*_tmp2F3));_tmp2F3->v=_tmp2F2;_tmp2F3->b=(void*)_tmp2F1;_tmp2F3->tl=_tmp2E3;_tmp2F3;});};}}
# 1202
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp2F9=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp2FA=_tmp2F9;struct _dyneither_ptr*_tmp2FB;struct Cyc_Absyn_Tqual _tmp2FC;void*_tmp2FD;int _tmp2FE;_LL147: _tmp2FB=_tmp2FA.name;_tmp2FC=_tmp2FA.tq;_tmp2FD=_tmp2FA.type;_tmp2FE=_tmp2FA.inject;_LL148:;
if(_tmp2FB != 0){
void*_tmp2FF=Cyc_Absyn_dyneither_typ(_tmp2FD,param_rgn,_tmp2FC,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl*_tmp300=({struct Cyc_Absyn_Vardecl*_tmp307=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp307[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp308;_tmp308.sc=Cyc_Absyn_Public;_tmp308.name=({struct _tuple0*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->f1=Cyc_Absyn_Loc_n;_tmp309->f2=_tmp2FB;_tmp309;});_tmp308.tq=
# 1208
Cyc_Absyn_empty_tqual(0);_tmp308.type=_tmp2FF;_tmp308.initializer=0;_tmp308.rgn=param_rgn;_tmp308.attributes=0;_tmp308.escapes=0;_tmp308;});_tmp307;});
# 1213
_tmp2EC=({struct Cyc_List_List*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->hd=_tmp300;_tmp301->tl=_tmp2EC;_tmp301;});{
struct _dyneither_ptr*_tmp302=_tmp2FB;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp303=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp306;_tmp306.tag=3;_tmp306.f1=_tmp300;_tmp306;});_tmp305;});
_tmp2E3=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp304=_region_malloc(_tmp2E2,sizeof(*_tmp304));_tmp304->v=_tmp302;_tmp304->b=(void*)_tmp303;_tmp304->tl=_tmp2E3;_tmp304;});};}else{
# 1218
({void*_tmp30A=0;Cyc_Tcutil_terr(loc,({const char*_tmp30B="missing name for varargs";_tag_dyneither(_tmp30B,sizeof(char),25);}),_tag_dyneither(_tmp30A,sizeof(void*),0));});}}
# 1220
fd->param_vardecls=({struct Cyc_Core_Opt*_tmp30C=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp30C[0]=(struct Cyc_Core_Opt)({struct Cyc_Core_Opt _tmp30D;_tmp30D.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2EC);_tmp30D;});_tmp30C;});
return({struct Cyc_Tcenv_Fenv*_tmp30E=_region_malloc(_tmp2E2,sizeof(*_tmp30E));_tmp30E->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp316=_region_malloc(_tmp2E2,sizeof(*_tmp316));_tmp316->frgn=_tmp2E2;_tmp316->return_typ=fd->ret_type;_tmp316->delayed_effect_checks=0;_tmp316->delayed_constraint_checks=0;_tmp316;});_tmp30E->type_vars=(struct Cyc_List_List*)_tmp2EB;_tmp30E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2DE;_tmp30E->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2E3;_tmp30E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp315=_region_malloc(_tmp2E2,sizeof(*_tmp315));_tmp315->fallthru_clause=0;_tmp315;});_tmp30E->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp310;_tmp310.tag=24;_tmp310.f1=({struct Cyc_List_List*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp314;_tmp314.tag=23;_tmp314.f1=param_rgn;_tmp314;});_tmp313;});_tmp311->tl=({struct Cyc_List_List*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->hd=(void*)_check_null(fd->effect);_tmp312->tl=0;_tmp312;});_tmp311;});_tmp310;});_tmp30F;}));_tmp30E->curr_rgn=(void*)param_rgn;_tmp30E->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp30E->flags).in_notreadctxt=0;(_tmp30E->flags).in_lhs=0;(_tmp30E->flags).in_new=0;(_tmp30E->flags).abstract_ok=0;_tmp30E->flags;});_tmp30E->fnrgn=(struct _RegionHandle*)_tmp2E1;_tmp30E;});};};};}
