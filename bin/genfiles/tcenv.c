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
# 42 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 53
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 69
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 73
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 84
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 86
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 88
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 100 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 104
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 108
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 118
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 121
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 123
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 126
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 129
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 135
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 139
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 144
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 152
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 157
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 162
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 165
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 173
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 175
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 177
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 179
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 181
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 183
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 186
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 189
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
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
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10]="Env_error";
# 64 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 66
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 69
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 110
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 135 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;const struct _tuple10*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 168 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 170
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)
return bs->b;}
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}struct _tuple12{void*f1;int f2;};
# 178
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 180
struct Cyc_Tcenv_Genv*_tmp2=({struct Cyc_Tcenv_Genv*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->grgn=r;_tmp9->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp9;});
_tmp2->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp2->datatypedecls,Cyc_Absyn_exn_name,({struct Cyc_Absyn_Datatypedecl**_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=Cyc_Absyn_exn_tud;_tmp3;}));
{struct Cyc_List_List*_tmp4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp4 != 0;_tmp4=_tmp4->tl){
_tmp2->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp2->ordinaries,((struct Cyc_Absyn_Datatypefield*)_tmp4->hd)->name,({struct _tuple12*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp7;_tmp7.tag=2;_tmp7.f1=Cyc_Absyn_exn_tud;_tmp7.f2=(struct Cyc_Absyn_Datatypefield*)_tmp4->hd;_tmp7;});_tmp6;});_tmp5->f2=1;_tmp5;}));}}
# 191
return({struct Cyc_Tcenv_Tenv*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->ns=0;_tmp8->ae=_tmp2;_tmp8->le=0;_tmp8->allow_valueof=0;_tmp8->in_extern_c_include=0;_tmp8;});}
# 193
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple12*_tmpA=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
if(is_use)
(*_tmpA).f2=1;
return(*_tmpA).f1;}
# 199
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->aggrdecls,q);}
# 202
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);}
# 205
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmpB=_region_malloc(r,sizeof(*_tmpB));_tmpB[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);_tmpB;});}
# 208
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);}
# 211
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->typedefs,q);}
# 217
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmpC=te->le;
if(_tmpC == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmpC;}
# 223
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD=te->le;
if(_tmpD != 0){
struct Cyc_Tcenv_Fenv*_tmpE=_tmpD;struct Cyc_Tcenv_Fenv*_tmpF=_tmpE;struct _RegionHandle*_tmp10;_LL1: _tmp10=_tmpF->fnrgn;_LL2:;
return _tmp10;}
# 229
return Cyc_Core_heap_region;}
# 232
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp11="put_fenv";_tag_dyneither(_tmp11,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp12=_region_malloc(l,sizeof(*_tmp12));_tmp12->ns=te->ns;_tmp12->ae=te->ae;_tmp12->le=fe;_tmp12->allow_valueof=te->allow_valueof;_tmp12->in_extern_c_include=te->in_extern_c_include;_tmp12;});}
# 237
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp13=_region_malloc(l,sizeof(*_tmp13));_tmp13->ns=te->ns;_tmp13->ae=te->ae;_tmp13->le=0;_tmp13->allow_valueof=te->allow_valueof;_tmp13->in_extern_c_include=te->in_extern_c_include;_tmp13;});}
# 241
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 243
struct Cyc_Tcenv_Fenv _tmp14=*f;struct Cyc_Tcenv_Fenv _tmp15=_tmp14;struct Cyc_Tcenv_SharedFenv*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_RgnOrder_RgnPO*_tmp1D;const struct Cyc_Tcenv_Bindings*_tmp1C;const struct _tuple10*_tmp1B;void*_tmp1A;void*_tmp19;struct Cyc_Tcenv_FenvFlags _tmp18;struct _RegionHandle*_tmp17;_LL4: _tmp1F=_tmp15.shared;_tmp1E=_tmp15.type_vars;_tmp1D=_tmp15.region_order;_tmp1C=_tmp15.locals;_tmp1B=_tmp15.ctrl_env;_tmp1A=_tmp15.capability;_tmp19=_tmp15.curr_rgn;_tmp18=_tmp15.flags;_tmp17=_tmp15.fnrgn;_LL5:;
# 245
return({struct Cyc_Tcenv_Fenv*_tmp16=_region_malloc(l,sizeof(*_tmp16));_tmp16->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F;_tmp16->type_vars=(struct Cyc_List_List*)_tmp1E;_tmp16->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1D;_tmp16->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1C;_tmp16->ctrl_env=(const struct _tuple10*)_tmp1B;_tmp16->capability=(void*)_tmp1A;_tmp16->curr_rgn=(void*)_tmp19;_tmp16->flags=(struct Cyc_Tcenv_FenvFlags)_tmp18;_tmp16->fnrgn=(struct _RegionHandle*)l;_tmp16;});}
# 250
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp20=Cyc_Tcenv_get_fenv(te,({const char*_tmp26="return_typ";_tag_dyneither(_tmp26,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmp21=_tmp20;struct Cyc_Tcenv_SharedFenv*_tmp25;_LL7: _tmp25=_tmp21->shared;_LL8:;{
struct Cyc_Tcenv_SharedFenv _tmp22=*_tmp25;struct Cyc_Tcenv_SharedFenv _tmp23=_tmp22;void*_tmp24;_LLA: _tmp24=_tmp23.return_typ;_LLB:;
return _tmp24;};}
# 256
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp27=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmp28=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmp27));struct Cyc_Tcenv_Fenv _tmp29=_tmp28;struct Cyc_List_List*_tmp2A;_LLD: _tmp2A=_tmp29.type_vars;_LLE:;
return _tmp2A;};}
# 263
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->v=Cyc_Tcenv_lookup_type_vars(te);_tmp2B;});}
# 267
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 269
struct Cyc_Tcenv_Fenv*_tmp2C=
# 271
Cyc_Tcenv_get_fenv(te,({const char*_tmp39="add_type_vars";_tag_dyneither(_tmp39,sizeof(char),14);}));
# 269
struct Cyc_Tcenv_Fenv*_tmp2D=_tmp2C;struct Cyc_Tcenv_SharedFenv*_tmp38;struct Cyc_List_List*_tmp37;struct Cyc_RgnOrder_RgnPO*_tmp36;const struct Cyc_Tcenv_Bindings*_tmp35;const struct _tuple10*_tmp34;void*_tmp33;void*_tmp32;struct Cyc_Tcenv_FenvFlags _tmp31;_LL10: _tmp38=_tmp2D->shared;_tmp37=_tmp2D->type_vars;_tmp36=_tmp2D->region_order;_tmp35=_tmp2D->locals;_tmp34=_tmp2D->ctrl_env;_tmp33=_tmp2D->capability;_tmp32=_tmp2D->curr_rgn;_tmp31=_tmp2D->flags;_LL11:;
# 273
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp37);
Cyc_Tcutil_check_unique_tvars(loc,_tmp2E);{
struct Cyc_Tcenv_Fenv*_tmp2F=({struct Cyc_Tcenv_Fenv*_tmp30=_region_malloc(r,sizeof(*_tmp30));_tmp30->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp38;_tmp30->type_vars=(struct Cyc_List_List*)_tmp2E;_tmp30->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp36;_tmp30->locals=(const struct Cyc_Tcenv_Bindings*)_tmp35;_tmp30->ctrl_env=(const struct _tuple10*)_tmp34;_tmp30->capability=(void*)_tmp33;_tmp30->curr_rgn=(void*)_tmp32;_tmp30->flags=(struct Cyc_Tcenv_FenvFlags)_tmp31;_tmp30->fnrgn=(struct _RegionHandle*)r;_tmp30;});
# 278
return Cyc_Tcenv_put_fenv(r,te,_tmp2F);};};}
# 281
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3A=te->le;
if(_tmp3A == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 286
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv(r,_tmp3A);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 291
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
struct _dyneither_ptr*_tmp3B=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmp3C=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmp3D=_tmp3C;if((_tmp3D.Loc_n).tag == 4){_LL13: _LL14:
 goto _LL12;}else{_LL15: _LL16:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3F;_tmp3F.tag=Cyc_Core_Impossible;_tmp3F.f1=({const char*_tmp40="add_local_var: called with non-local";_tag_dyneither(_tmp40,sizeof(char),37);});_tmp3F;});_tmp3E;}));}_LL12:;}{
# 297
struct Cyc_Tcenv_Fenv*_tmp41=
# 299
Cyc_Tcenv_get_fenv(te,({const char*_tmp51="add_local_var";_tag_dyneither(_tmp51,sizeof(char),14);}));
# 297
struct Cyc_Tcenv_Fenv*_tmp42=_tmp41;struct Cyc_Tcenv_SharedFenv*_tmp50;struct Cyc_List_List*_tmp4F;struct Cyc_RgnOrder_RgnPO*_tmp4E;const struct Cyc_Tcenv_Bindings*_tmp4D;const struct _tuple10*_tmp4C;void*_tmp4B;void*_tmp4A;struct Cyc_Tcenv_FenvFlags _tmp49;_LL18: _tmp50=_tmp42->shared;_tmp4F=_tmp42->type_vars;_tmp4E=_tmp42->region_order;_tmp4D=_tmp42->locals;_tmp4C=_tmp42->ctrl_env;_tmp4B=_tmp42->capability;_tmp4A=_tmp42->curr_rgn;_tmp49=_tmp42->flags;_LL19:;{
# 300
struct Cyc_Tcenv_Bindings*_tmp43=({struct Cyc_Tcenv_Bindings*_tmp46=_region_malloc(r,sizeof(*_tmp46));_tmp46->v=_tmp3B;_tmp46->b=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp48;_tmp48.tag=4;_tmp48.f1=vd;_tmp48;});_tmp47;});_tmp46->tl=_tmp4D;_tmp46;});
struct Cyc_Tcenv_Fenv*_tmp44=({struct Cyc_Tcenv_Fenv*_tmp45=_region_malloc(r,sizeof(*_tmp45));_tmp45->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp50;_tmp45->type_vars=(struct Cyc_List_List*)_tmp4F;_tmp45->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp4E;_tmp45->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp43);_tmp45->ctrl_env=(const struct _tuple10*)_tmp4C;_tmp45->capability=(void*)_tmp4B;_tmp45->curr_rgn=(void*)_tmp4A;_tmp45->flags=(struct Cyc_Tcenv_FenvFlags)_tmp49;_tmp45->fnrgn=(struct _RegionHandle*)r;_tmp45;});
# 303
return Cyc_Tcenv_put_fenv(r,te,_tmp44);};};}
# 306
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 308
struct Cyc_Tcenv_Fenv*_tmp52=te->le;
if(_tmp52 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp53=*_tmp52;struct Cyc_Tcenv_Fenv _tmp54=_tmp53;struct Cyc_Tcenv_SharedFenv*_tmp60;struct Cyc_List_List*_tmp5F;struct Cyc_RgnOrder_RgnPO*_tmp5E;const struct Cyc_Tcenv_Bindings*_tmp5D;const struct _tuple10*_tmp5C;void*_tmp5B;void*_tmp5A;int _tmp59;int _tmp58;int _tmp57;_LL1B: _tmp60=_tmp54.shared;_tmp5F=_tmp54.type_vars;_tmp5E=_tmp54.region_order;_tmp5D=_tmp54.locals;_tmp5C=_tmp54.ctrl_env;_tmp5B=_tmp54.capability;_tmp5A=_tmp54.curr_rgn;_tmp59=(_tmp54.flags).in_notreadctxt;_tmp58=(_tmp54.flags).in_lhs;_tmp57=(_tmp54.flags).abstract_ok;_LL1C:;{
# 313
struct Cyc_Tcenv_Fenv*_tmp55=({struct Cyc_Tcenv_Fenv*_tmp56=_region_malloc(r,sizeof(*_tmp56));_tmp56->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp60;_tmp56->type_vars=(struct Cyc_List_List*)_tmp5F;_tmp56->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp5E;_tmp56->locals=(const struct Cyc_Tcenv_Bindings*)_tmp5D;_tmp56->ctrl_env=(const struct _tuple10*)_tmp5C;_tmp56->capability=(void*)_tmp5B;_tmp56->curr_rgn=(void*)_tmp5A;_tmp56->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp56->flags).in_notreadctxt=_tmp59;(_tmp56->flags).in_lhs=_tmp58;(_tmp56->flags).in_new=(int)status;(_tmp56->flags).abstract_ok=_tmp57;_tmp56->flags;});_tmp56->fnrgn=(struct _RegionHandle*)r;_tmp56;});
# 316
return Cyc_Tcenv_put_fenv(r,te,_tmp55);};};}
# 319
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp61=te->le;
if(_tmp61 == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp62=_tmp61;struct Cyc_Tcenv_Fenv*_tmp63=_tmp62;int _tmp64;_LL1E: _tmp64=(_tmp63->flags).in_new;_LL1F:;
return(enum Cyc_Tcenv_NewStatus)_tmp64;};}
# 326
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp65=te->le;
if(_tmp65 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp66=_tmp65;struct Cyc_Tcenv_Fenv*_tmp67=_tmp66;int _tmp68;_LL21: _tmp68=(_tmp67->flags).abstract_ok;_LL22:;
return _tmp68;};}
# 333
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 335
struct Cyc_Tcenv_Fenv*_tmp69=te->le;
if(_tmp69 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp6A=*_tmp69;struct Cyc_Tcenv_Fenv _tmp6B=_tmp6A;struct Cyc_Tcenv_SharedFenv*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_RgnOrder_RgnPO*_tmp75;const struct Cyc_Tcenv_Bindings*_tmp74;const struct _tuple10*_tmp73;void*_tmp72;void*_tmp71;int _tmp70;int _tmp6F;int _tmp6E;_LL24: _tmp77=_tmp6B.shared;_tmp76=_tmp6B.type_vars;_tmp75=_tmp6B.region_order;_tmp74=_tmp6B.locals;_tmp73=_tmp6B.ctrl_env;_tmp72=_tmp6B.capability;_tmp71=_tmp6B.curr_rgn;_tmp70=(_tmp6B.flags).in_notreadctxt;_tmp6F=(_tmp6B.flags).in_lhs;_tmp6E=(_tmp6B.flags).in_new;_LL25:;{
# 340
struct Cyc_Tcenv_Fenv*_tmp6C=({struct Cyc_Tcenv_Fenv*_tmp6D=_region_malloc(r,sizeof(*_tmp6D));_tmp6D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp77;_tmp6D->type_vars=(struct Cyc_List_List*)_tmp76;_tmp6D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp75;_tmp6D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp74;_tmp6D->ctrl_env=(const struct _tuple10*)_tmp73;_tmp6D->capability=(void*)_tmp72;_tmp6D->curr_rgn=(void*)_tmp71;_tmp6D->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp6D->flags).in_notreadctxt=_tmp70;(_tmp6D->flags).in_lhs=_tmp6F;(_tmp6D->flags).in_new=_tmp6E;(_tmp6D->flags).abstract_ok=1;_tmp6D->flags;});_tmp6D->fnrgn=(struct _RegionHandle*)r;_tmp6D;});
# 343
return Cyc_Tcenv_put_fenv(r,te,_tmp6C);};};}
# 346
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 348
struct Cyc_Tcenv_Fenv*_tmp78=te->le;
if(_tmp78 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp79=*_tmp78;struct Cyc_Tcenv_Fenv _tmp7A=_tmp79;struct Cyc_Tcenv_SharedFenv*_tmp86;struct Cyc_List_List*_tmp85;struct Cyc_RgnOrder_RgnPO*_tmp84;const struct Cyc_Tcenv_Bindings*_tmp83;const struct _tuple10*_tmp82;void*_tmp81;void*_tmp80;int _tmp7F;int _tmp7E;int _tmp7D;_LL27: _tmp86=_tmp7A.shared;_tmp85=_tmp7A.type_vars;_tmp84=_tmp7A.region_order;_tmp83=_tmp7A.locals;_tmp82=_tmp7A.ctrl_env;_tmp81=_tmp7A.capability;_tmp80=_tmp7A.curr_rgn;_tmp7F=(_tmp7A.flags).in_notreadctxt;_tmp7E=(_tmp7A.flags).in_lhs;_tmp7D=(_tmp7A.flags).in_new;_LL28:;{
# 353
struct Cyc_Tcenv_Fenv*_tmp7B=({struct Cyc_Tcenv_Fenv*_tmp7C=_region_malloc(r,sizeof(*_tmp7C));_tmp7C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp86;_tmp7C->type_vars=(struct Cyc_List_List*)_tmp85;_tmp7C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp84;_tmp7C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp83;_tmp7C->ctrl_env=(const struct _tuple10*)_tmp82;_tmp7C->capability=(void*)_tmp81;_tmp7C->curr_rgn=(void*)_tmp80;_tmp7C->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp7C->flags).in_notreadctxt=_tmp7F;(_tmp7C->flags).in_lhs=_tmp7E;(_tmp7C->flags).in_new=_tmp7D;(_tmp7C->flags).abstract_ok=0;_tmp7C->flags;});_tmp7C->fnrgn=(struct _RegionHandle*)r;_tmp7C;});
# 356
return Cyc_Tcenv_put_fenv(r,te,_tmp7B);};};}
# 359
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp87=te->le;
if(_tmp87 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp88=*_tmp87;struct Cyc_Tcenv_Fenv _tmp89=_tmp88;struct Cyc_Tcenv_SharedFenv*_tmp95;struct Cyc_List_List*_tmp94;struct Cyc_RgnOrder_RgnPO*_tmp93;const struct Cyc_Tcenv_Bindings*_tmp92;const struct _tuple10*_tmp91;void*_tmp90;void*_tmp8F;int _tmp8E;int _tmp8D;int _tmp8C;_LL2A: _tmp95=_tmp89.shared;_tmp94=_tmp89.type_vars;_tmp93=_tmp89.region_order;_tmp92=_tmp89.locals;_tmp91=_tmp89.ctrl_env;_tmp90=_tmp89.capability;_tmp8F=_tmp89.curr_rgn;_tmp8E=(_tmp89.flags).in_lhs;_tmp8D=(_tmp89.flags).in_new;_tmp8C=(_tmp89.flags).abstract_ok;_LL2B:;{
# 365
struct Cyc_Tcenv_Fenv*_tmp8A=({struct Cyc_Tcenv_Fenv*_tmp8B=_region_malloc(r,sizeof(*_tmp8B));_tmp8B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp95;_tmp8B->type_vars=(struct Cyc_List_List*)_tmp94;_tmp8B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp93;_tmp8B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp92;_tmp8B->ctrl_env=(const struct _tuple10*)_tmp91;_tmp8B->capability=(void*)_tmp90;_tmp8B->curr_rgn=(void*)_tmp8F;_tmp8B->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp8B->flags).in_notreadctxt=1;(_tmp8B->flags).in_lhs=_tmp8E;(_tmp8B->flags).in_new=_tmp8D;(_tmp8B->flags).abstract_ok=_tmp8C;_tmp8B->flags;});_tmp8B->fnrgn=(struct _RegionHandle*)r;_tmp8B;});
# 368
return Cyc_Tcenv_put_fenv(r,te,_tmp8A);};};}
# 371
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp96=te->le;
if(_tmp96 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp97=*_tmp96;struct Cyc_Tcenv_Fenv _tmp98=_tmp97;struct Cyc_Tcenv_SharedFenv*_tmpA4;struct Cyc_List_List*_tmpA3;struct Cyc_RgnOrder_RgnPO*_tmpA2;const struct Cyc_Tcenv_Bindings*_tmpA1;const struct _tuple10*_tmpA0;void*_tmp9F;void*_tmp9E;int _tmp9D;int _tmp9C;int _tmp9B;_LL2D: _tmpA4=_tmp98.shared;_tmpA3=_tmp98.type_vars;_tmpA2=_tmp98.region_order;_tmpA1=_tmp98.locals;_tmpA0=_tmp98.ctrl_env;_tmp9F=_tmp98.capability;_tmp9E=_tmp98.curr_rgn;_tmp9D=(_tmp98.flags).in_lhs;_tmp9C=(_tmp98.flags).in_new;_tmp9B=(_tmp98.flags).abstract_ok;_LL2E:;{
# 377
struct Cyc_Tcenv_Fenv*_tmp99=({struct Cyc_Tcenv_Fenv*_tmp9A=_region_malloc(r,sizeof(*_tmp9A));_tmp9A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA4;_tmp9A->type_vars=(struct Cyc_List_List*)_tmpA3;_tmp9A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA2;_tmp9A->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA1;_tmp9A->ctrl_env=(const struct _tuple10*)_tmpA0;_tmp9A->capability=(void*)_tmp9F;_tmp9A->curr_rgn=(void*)_tmp9E;_tmp9A->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp9A->flags).in_notreadctxt=0;(_tmp9A->flags).in_lhs=_tmp9D;(_tmp9A->flags).in_new=_tmp9C;(_tmp9A->flags).abstract_ok=_tmp9B;_tmp9A->flags;});_tmp9A->fnrgn=(struct _RegionHandle*)r;_tmp9A;});
# 380
return Cyc_Tcenv_put_fenv(r,te,_tmp99);};};}
# 383
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpA5=te->le;
if(_tmpA5 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpA6=_tmpA5;struct Cyc_Tcenv_Fenv*_tmpA7=_tmpA6;int _tmpA8;_LL30: _tmpA8=(_tmpA7->flags).in_notreadctxt;_LL31:;
return _tmpA8;};}
# 390
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpA9=te->le;
if(_tmpA9 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpAA=*_tmpA9;struct Cyc_Tcenv_Fenv _tmpAB=_tmpAA;struct Cyc_Tcenv_SharedFenv*_tmpB7;struct Cyc_List_List*_tmpB6;struct Cyc_RgnOrder_RgnPO*_tmpB5;const struct Cyc_Tcenv_Bindings*_tmpB4;const struct _tuple10*_tmpB3;void*_tmpB2;void*_tmpB1;int _tmpB0;int _tmpAF;int _tmpAE;_LL33: _tmpB7=_tmpAB.shared;_tmpB6=_tmpAB.type_vars;_tmpB5=_tmpAB.region_order;_tmpB4=_tmpAB.locals;_tmpB3=_tmpAB.ctrl_env;_tmpB2=_tmpAB.capability;_tmpB1=_tmpAB.curr_rgn;_tmpB0=(_tmpAB.flags).in_notreadctxt;_tmpAF=(_tmpAB.flags).in_new;_tmpAE=(_tmpAB.flags).abstract_ok;_LL34:;{
# 396
struct Cyc_Tcenv_Fenv*_tmpAC=({struct Cyc_Tcenv_Fenv*_tmpAD=_region_malloc(r,sizeof(*_tmpAD));_tmpAD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB7;_tmpAD->type_vars=(struct Cyc_List_List*)_tmpB6;_tmpAD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB5;_tmpAD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB4;_tmpAD->ctrl_env=(const struct _tuple10*)_tmpB3;_tmpAD->capability=(void*)_tmpB2;_tmpAD->curr_rgn=(void*)_tmpB1;_tmpAD->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpAD->flags).in_notreadctxt=_tmpB0;(_tmpAD->flags).in_lhs=1;(_tmpAD->flags).in_new=_tmpAF;(_tmpAD->flags).abstract_ok=_tmpAE;_tmpAD->flags;});_tmpAD->fnrgn=(struct _RegionHandle*)r;_tmpAD;});
# 399
return Cyc_Tcenv_put_fenv(r,te,_tmpAC);};};}
# 402
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpB8=te->le;
if(_tmpB8 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpB9=*_tmpB8;struct Cyc_Tcenv_Fenv _tmpBA=_tmpB9;struct Cyc_Tcenv_SharedFenv*_tmpC6;struct Cyc_List_List*_tmpC5;struct Cyc_RgnOrder_RgnPO*_tmpC4;const struct Cyc_Tcenv_Bindings*_tmpC3;const struct _tuple10*_tmpC2;void*_tmpC1;void*_tmpC0;int _tmpBF;int _tmpBE;int _tmpBD;_LL36: _tmpC6=_tmpBA.shared;_tmpC5=_tmpBA.type_vars;_tmpC4=_tmpBA.region_order;_tmpC3=_tmpBA.locals;_tmpC2=_tmpBA.ctrl_env;_tmpC1=_tmpBA.capability;_tmpC0=_tmpBA.curr_rgn;_tmpBF=(_tmpBA.flags).in_notreadctxt;_tmpBE=(_tmpBA.flags).in_new;_tmpBD=(_tmpBA.flags).abstract_ok;_LL37:;{
# 408
struct Cyc_Tcenv_Fenv*_tmpBB=({struct Cyc_Tcenv_Fenv*_tmpBC=_region_malloc(r,sizeof(*_tmpBC));_tmpBC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC6;_tmpBC->type_vars=(struct Cyc_List_List*)_tmpC5;_tmpBC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC4;_tmpBC->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC3;_tmpBC->ctrl_env=(const struct _tuple10*)_tmpC2;_tmpBC->capability=(void*)_tmpC1;_tmpBC->curr_rgn=(void*)_tmpC0;_tmpBC->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpBC->flags).in_notreadctxt=_tmpBF;(_tmpBC->flags).in_lhs=0;(_tmpBC->flags).in_new=_tmpBE;(_tmpBC->flags).abstract_ok=_tmpBD;_tmpBC->flags;});_tmpBC->fnrgn=(struct _RegionHandle*)r;_tmpBC;});
# 411
return Cyc_Tcenv_put_fenv(r,te,_tmpBB);};};}
# 414
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC7=te->le;
if(_tmpC7 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpC8=_tmpC7;struct Cyc_Tcenv_Fenv*_tmpC9=_tmpC8;int _tmpCA;_LL39: _tmpCA=(_tmpC9->flags).in_lhs;_LL3A:;
return _tmpCA;};}
# 422
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 424
struct _dyneither_ptr*_tmpCB=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmpCC=
# 427
Cyc_Tcenv_get_fenv(te,({const char*_tmpDF="add_pat_var";_tag_dyneither(_tmpDF,sizeof(char),12);}));
# 425
struct Cyc_Tcenv_Fenv*_tmpCD=_tmpCC;struct Cyc_Tcenv_SharedFenv*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_RgnOrder_RgnPO*_tmpDC;const struct Cyc_Tcenv_Bindings*_tmpDB;const struct _tuple10*_tmpDA;void*_tmpD9;void*_tmpD8;struct Cyc_Tcenv_FenvFlags _tmpD7;_LL3C: _tmpDE=_tmpCD->shared;_tmpDD=_tmpCD->type_vars;_tmpDC=_tmpCD->region_order;_tmpDB=_tmpCD->locals;_tmpDA=_tmpCD->ctrl_env;_tmpD9=_tmpCD->capability;_tmpD8=_tmpCD->curr_rgn;_tmpD7=_tmpCD->flags;_LL3D:;{
# 428
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpCE=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmpD6;_tmpD6.tag=5;_tmpD6.f1=vd;_tmpD6;});_tmpD5;});
struct Cyc_Tcenv_Bindings*_tmpCF=({struct Cyc_Tcenv_Bindings*_tmpD2=_region_malloc(r,sizeof(*_tmpD2));_tmpD2->v=_tmpCB;_tmpD2->b=(void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmpD4;_tmpD4.tag=5;_tmpD4.f1=vd;_tmpD4;});_tmpD3;});_tmpD2->tl=_tmpDB;_tmpD2;});
struct Cyc_Tcenv_Fenv*_tmpD0=({struct Cyc_Tcenv_Fenv*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));_tmpD1->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpDE;_tmpD1->type_vars=(struct Cyc_List_List*)_tmpDD;_tmpD1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpDC;_tmpD1->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpCF);_tmpD1->ctrl_env=(const struct _tuple10*)_tmpDA;_tmpD1->capability=(void*)_tmpD9;_tmpD1->curr_rgn=(void*)_tmpD8;_tmpD1->flags=(struct Cyc_Tcenv_FenvFlags)_tmpD7;_tmpD1->fnrgn=(struct _RegionHandle*)r;_tmpD1;});
# 432
return Cyc_Tcenv_put_fenv(r,te,_tmpD0);};}
# 435
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmpE0=te->le;
struct _tuple0*_tmpE1=q;union Cyc_Absyn_Nmspace _tmpFB;struct _dyneither_ptr*_tmpFA;_LL3F: _tmpFB=_tmpE1->f1;_tmpFA=_tmpE1->f2;_LL40:;{
union Cyc_Absyn_Nmspace _tmpE2=_tmpFB;if((_tmpE2.Loc_n).tag == 4){_LL42: _LL43:
# 440
 if(_tmpE0 == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmpE3=_cycalloc_atomic(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Dict_Absent_exn_struct _tmpE4;_tmpE4.tag=Cyc_Dict_Absent;_tmpE4;});_tmpE3;}));{
struct Cyc_Tcenv_Fenv*_tmpE5=_tmpE0;struct Cyc_Tcenv_Fenv*_tmpE6=_tmpE5;const struct Cyc_Tcenv_Bindings*_tmpEF;_LL47: _tmpEF=_tmpE6->locals;_LL48:;{
# 443
struct _handler_cons _tmpE7;_push_handler(& _tmpE7);{int _tmpE9=0;if(setjmp(_tmpE7.handler))_tmpE9=1;if(!_tmpE9){{void*_tmpEC=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEA=_region_malloc(r,sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=Cyc_Tcenv_lookup_binding(_tmpEF,_tmpFA);_tmpEB;});_tmpEA;});_npop_handler(0);return _tmpEC;};_pop_handler();}else{void*_tmpE8=(void*)_exn_thrown;void*_tmpED=_tmpE8;void*_tmpEE;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmpED)->tag == Cyc_Tcenv_NoBinding){_LL4A: _LL4B:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LL4C: _tmpEE=_tmpED;_LL4D:(int)_rethrow(_tmpEE);}_LL49:;}};};};}else{_LL44: _LL45: {
# 446
struct _handler_cons _tmpF0;_push_handler(& _tmpF0);{int _tmpF2=0;if(setjmp(_tmpF0.handler))_tmpF2=1;if(!_tmpF2){{void*_tmpF3=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmpF3;};_pop_handler();}else{void*_tmpF1=(void*)_exn_thrown;void*_tmpF4=_tmpF1;void*_tmpF9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpF4)->tag == Cyc_Dict_Absent){_LL4F: _LL50:
# 448
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF5=_region_malloc(r,sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=q;_tmpF8;});_tmpF7;});_tmpF6;});_tmpF5;});}else{_LL51: _tmpF9=_tmpF4;_LL52:(int)_rethrow(_tmpF9);}_LL4E:;}};}}_LL41:;};}
# 454
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 456
struct Cyc_Tcenv_Fenv*_tmpFC=Cyc_Tcenv_get_fenv(te,({const char*_tmp101="process_fallthru";_tag_dyneither(_tmp101,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmpFD=_tmpFC;const struct _tuple10*_tmp100;_LL54: _tmp100=_tmpFD->ctrl_env;_LL55:;{
const struct _tuple10*_tmpFE=_tmp100;
if(_tmpFE != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=(*_tmpFE).f1;_tmpFF;});
return _tmpFE;};}struct _tuple13{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 463
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 467
struct Cyc_Tcenv_Fenv*_tmp102=
# 469
Cyc_Tcenv_get_fenv(te,({const char*_tmp113="set_fallthru";_tag_dyneither(_tmp113,sizeof(char),13);}));
# 467
struct Cyc_Tcenv_Fenv*_tmp103=_tmp102;struct Cyc_Tcenv_SharedFenv*_tmp112;struct Cyc_List_List*_tmp111;struct Cyc_RgnOrder_RgnPO*_tmp110;const struct Cyc_Tcenv_Bindings*_tmp10F;const struct _tuple10*_tmp10E;void*_tmp10D;void*_tmp10C;struct Cyc_Tcenv_FenvFlags _tmp10B;_LL57: _tmp112=_tmp103->shared;_tmp111=_tmp103->type_vars;_tmp110=_tmp103->region_order;_tmp10F=_tmp103->locals;_tmp10E=_tmp103->ctrl_env;_tmp10D=_tmp103->capability;_tmp10C=_tmp103->curr_rgn;_tmp10B=_tmp103->flags;_LL58:;{
# 470
const struct _tuple10*_tmp104=_tmp10E;
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp105->tl=ft_typ;_tmp105;});}{
struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple13*_tmp107=({struct _tuple13*_tmp10A=_region_malloc(r,sizeof(*_tmp10A));_tmp10A->f1=clause;_tmp10A->f2=new_tvs;_tmp10A->f3=_tmp106;_tmp10A;});
struct Cyc_Tcenv_Fenv*_tmp108=({struct Cyc_Tcenv_Fenv*_tmp109=_region_malloc(r,sizeof(*_tmp109));_tmp109->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp112;_tmp109->type_vars=(struct Cyc_List_List*)_tmp111;_tmp109->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp110;_tmp109->locals=(const struct Cyc_Tcenv_Bindings*)_tmp10F;_tmp109->ctrl_env=(const struct _tuple10*)((const struct _tuple10*)_tmp107);_tmp109->capability=(void*)_tmp10D;_tmp109->curr_rgn=(void*)_tmp10C;_tmp109->flags=(struct Cyc_Tcenv_FenvFlags)_tmp10B;_tmp109->fnrgn=(struct _RegionHandle*)r;_tmp109;});
# 480
return Cyc_Tcenv_put_fenv(r,te,_tmp108);};};}
# 483
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp116="clear_fallthru";_tag_dyneither(_tmp116,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp114=fe;const struct _tuple10*_tmp115;_LL5A: _tmp115=_tmp114->ctrl_env;_LL5B:;
_tmp115=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 490
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp117=_region_malloc(r2,sizeof(*_tmp117));_tmp117->ns=te->ns;_tmp117->ae=te->ae;_tmp117->le=0;_tmp117->allow_valueof=1;_tmp117->in_extern_c_include=te->in_extern_c_include;_tmp117;});{
struct Cyc_Tcenv_Fenv _tmp118=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp119=_tmp118;struct Cyc_Tcenv_SharedFenv*_tmp124;struct Cyc_List_List*_tmp123;struct Cyc_RgnOrder_RgnPO*_tmp122;const struct Cyc_Tcenv_Bindings*_tmp121;const struct _tuple10*_tmp120;void*_tmp11F;void*_tmp11E;struct Cyc_Tcenv_FenvFlags _tmp11D;_LL5D: _tmp124=_tmp119.shared;_tmp123=_tmp119.type_vars;_tmp122=_tmp119.region_order;_tmp121=_tmp119.locals;_tmp120=_tmp119.ctrl_env;_tmp11F=_tmp119.capability;_tmp11E=_tmp119.curr_rgn;_tmp11D=_tmp119.flags;_LL5E:;{
struct Cyc_Tcenv_Fenv*_tmp11A=({struct Cyc_Tcenv_Fenv*_tmp11C=_region_malloc(r2,sizeof(*_tmp11C));_tmp11C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp124;_tmp11C->type_vars=(struct Cyc_List_List*)_tmp123;_tmp11C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp122;_tmp11C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp121;_tmp11C->ctrl_env=(const struct _tuple10*)_tmp120;_tmp11C->capability=(void*)_tmp11F;_tmp11C->curr_rgn=(void*)_tmp11E;_tmp11C->flags=(struct Cyc_Tcenv_FenvFlags)_tmp11D;_tmp11C->fnrgn=(struct _RegionHandle*)r2;_tmp11C;});
return({struct Cyc_Tcenv_Tenv*_tmp11B=_region_malloc(r2,sizeof(*_tmp11B));_tmp11B->ns=te->ns;_tmp11B->ae=te->ae;_tmp11B->le=_tmp11A;_tmp11B->allow_valueof=1;_tmp11B->in_extern_c_include=te->in_extern_c_include;_tmp11B;});};};}
# 498
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp125=_region_malloc(r2,sizeof(*_tmp125));_tmp125->ns=te->ns;_tmp125->ae=te->ae;_tmp125->le=0;_tmp125->allow_valueof=te->allow_valueof;_tmp125->in_extern_c_include=1;_tmp125;});{
struct Cyc_Tcenv_Fenv _tmp126=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp127=_tmp126;struct Cyc_Tcenv_SharedFenv*_tmp132;struct Cyc_List_List*_tmp131;struct Cyc_RgnOrder_RgnPO*_tmp130;const struct Cyc_Tcenv_Bindings*_tmp12F;const struct _tuple10*_tmp12E;void*_tmp12D;void*_tmp12C;struct Cyc_Tcenv_FenvFlags _tmp12B;_LL60: _tmp132=_tmp127.shared;_tmp131=_tmp127.type_vars;_tmp130=_tmp127.region_order;_tmp12F=_tmp127.locals;_tmp12E=_tmp127.ctrl_env;_tmp12D=_tmp127.capability;_tmp12C=_tmp127.curr_rgn;_tmp12B=_tmp127.flags;_LL61:;{
struct Cyc_Tcenv_Fenv*_tmp128=({struct Cyc_Tcenv_Fenv*_tmp12A=_region_malloc(r2,sizeof(*_tmp12A));_tmp12A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp132;_tmp12A->type_vars=(struct Cyc_List_List*)_tmp131;_tmp12A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp130;_tmp12A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12F;_tmp12A->ctrl_env=(const struct _tuple10*)_tmp12E;_tmp12A->capability=(void*)_tmp12D;_tmp12A->curr_rgn=(void*)_tmp12C;_tmp12A->flags=(struct Cyc_Tcenv_FenvFlags)_tmp12B;_tmp12A->fnrgn=(struct _RegionHandle*)r2;_tmp12A;});
return({struct Cyc_Tcenv_Tenv*_tmp129=_region_malloc(r2,sizeof(*_tmp129));_tmp129->ns=te->ns;_tmp129->ae=te->ae;_tmp129->le=_tmp128;_tmp129->allow_valueof=te->allow_valueof;_tmp129->in_extern_c_include=1;_tmp129;});};};}
# 506
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 509
struct Cyc_Tcenv_Fenv _tmp133=*
# 511
Cyc_Tcenv_get_fenv(te,({const char*_tmp148="add_region";_tag_dyneither(_tmp148,sizeof(char),11);}));
# 509
struct Cyc_Tcenv_Fenv _tmp134=_tmp133;struct Cyc_Tcenv_SharedFenv*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_RgnOrder_RgnPO*_tmp145;const struct Cyc_Tcenv_Bindings*_tmp144;const struct _tuple10*_tmp143;void*_tmp142;void*_tmp141;struct Cyc_Tcenv_FenvFlags _tmp140;_LL63: _tmp147=_tmp134.shared;_tmp146=_tmp134.type_vars;_tmp145=_tmp134.region_order;_tmp144=_tmp134.locals;_tmp143=_tmp134.ctrl_env;_tmp142=_tmp134.capability;_tmp141=_tmp134.curr_rgn;_tmp140=_tmp134.flags;_LL64:;
# 512
{void*_tmp135=Cyc_Tcutil_compress(rgn);void*_tmp136=_tmp135;struct Cyc_Absyn_Tvar*_tmp137;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp136)->tag == 2){_LL66: _tmp137=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp136)->f1;_LL67:
# 514
 _tmp145=Cyc_RgnOrder_add_youngest(_tmp147->frgn,_tmp145,_tmp137,resetable,opened);
# 516
goto _LL65;}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}
# 519
_tmp142=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp139;_tmp139.tag=24;_tmp139.f1=({struct Cyc_List_List*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp13D;_tmp13D.tag=23;_tmp13D.f1=rgn;_tmp13D;});_tmp13C;});_tmp13A->tl=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->hd=_tmp142;_tmp13B->tl=0;_tmp13B;});_tmp13A;});_tmp139;});_tmp138;});{
# 521
struct Cyc_Tcenv_Fenv*_tmp13E=({struct Cyc_Tcenv_Fenv*_tmp13F=_region_malloc(r,sizeof(*_tmp13F));_tmp13F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp147;_tmp13F->type_vars=(struct Cyc_List_List*)_tmp146;_tmp13F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp145;_tmp13F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp144;_tmp13F->ctrl_env=(const struct _tuple10*)_tmp143;_tmp13F->capability=(void*)_tmp142;_tmp13F->curr_rgn=(void*)_tmp141;_tmp13F->flags=(struct Cyc_Tcenv_FenvFlags)_tmp140;_tmp13F->fnrgn=(struct _RegionHandle*)r;_tmp13F;});
# 524
return Cyc_Tcenv_put_fenv(r,te,_tmp13E);};}
# 527
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 529
struct Cyc_Tcenv_Fenv _tmp149=*
# 531
Cyc_Tcenv_get_fenv(te,({const char*_tmp15F="new_named_block";_tag_dyneither(_tmp15F,sizeof(char),16);}));
# 529
struct Cyc_Tcenv_Fenv _tmp14A=_tmp149;struct Cyc_Tcenv_SharedFenv*_tmp15E;struct Cyc_List_List*_tmp15D;struct Cyc_RgnOrder_RgnPO*_tmp15C;const struct Cyc_Tcenv_Bindings*_tmp15B;const struct _tuple10*_tmp15A;void*_tmp159;void*_tmp158;struct Cyc_Tcenv_FenvFlags _tmp157;_LL6B: _tmp15E=_tmp14A.shared;_tmp15D=_tmp14A.type_vars;_tmp15C=_tmp14A.region_order;_tmp15B=_tmp14A.locals;_tmp15A=_tmp14A.ctrl_env;_tmp159=_tmp14A.capability;_tmp158=_tmp14A.curr_rgn;_tmp157=_tmp14A.flags;_LL6C:;{
# 533
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp156="new_block";_tag_dyneither(_tmp156,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp155;_tmp155.tag=2;_tmp155.f1=block_rgn;_tmp155;});_tmp154;});
_tmp15D=({struct Cyc_List_List*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->hd=block_rgn;_tmp14B->tl=_tmp15D;_tmp14B;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp15D);
_tmp15C=Cyc_RgnOrder_add_youngest(_tmp15E->frgn,_tmp15C,block_rgn,0,0);
_tmp159=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp14D;_tmp14D.tag=24;_tmp14D.f1=({struct Cyc_List_List*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp151;_tmp151.tag=23;_tmp151.f1=block_typ;_tmp151;});_tmp150;});_tmp14E->tl=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->hd=_tmp159;_tmp14F->tl=0;_tmp14F;});_tmp14E;});_tmp14D;});_tmp14C;});
_tmp158=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp152=({struct Cyc_Tcenv_Fenv*_tmp153=_region_malloc(r,sizeof(*_tmp153));_tmp153->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15E;_tmp153->type_vars=(struct Cyc_List_List*)_tmp15D;_tmp153->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C;_tmp153->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15B;_tmp153->ctrl_env=(const struct _tuple10*)_tmp15A;_tmp153->capability=(void*)_tmp159;_tmp153->curr_rgn=(void*)_tmp158;_tmp153->flags=(struct Cyc_Tcenv_FenvFlags)_tmp157;_tmp153->fnrgn=(struct _RegionHandle*)r;_tmp153;});
# 543
return Cyc_Tcenv_put_fenv(r,te,_tmp152);};};}
# 546
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 548
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 550
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple14{void*f1;void*f2;};
# 556
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 560
struct Cyc_Tcenv_Fenv _tmp161=*
# 562
Cyc_Tcenv_get_fenv(te,({const char*_tmp16D="new_outlives_constraints";_tag_dyneither(_tmp16D,sizeof(char),25);}));
# 560
struct Cyc_Tcenv_Fenv _tmp162=_tmp161;struct Cyc_Tcenv_SharedFenv*_tmp16C;struct Cyc_List_List*_tmp16B;struct Cyc_RgnOrder_RgnPO*_tmp16A;const struct Cyc_Tcenv_Bindings*_tmp169;const struct _tuple10*_tmp168;void*_tmp167;void*_tmp166;struct Cyc_Tcenv_FenvFlags _tmp165;_LL6E: _tmp16C=_tmp162.shared;_tmp16B=_tmp162.type_vars;_tmp16A=_tmp162.region_order;_tmp169=_tmp162.locals;_tmp168=_tmp162.ctrl_env;_tmp167=_tmp162.capability;_tmp166=_tmp162.curr_rgn;_tmp165=_tmp162.flags;_LL6F:;
# 563
for(0;cs != 0;cs=cs->tl){
_tmp16A=
Cyc_RgnOrder_add_outlives_constraint(_tmp16C->frgn,_tmp16A,(*((struct _tuple14*)cs->hd)).f1,(*((struct _tuple14*)cs->hd)).f2,loc);}{
# 568
struct Cyc_Tcenv_Fenv*_tmp163=({struct Cyc_Tcenv_Fenv*_tmp164=_region_malloc(r,sizeof(*_tmp164));_tmp164->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp16C;_tmp164->type_vars=(struct Cyc_List_List*)_tmp16B;_tmp164->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp16A;_tmp164->locals=(const struct Cyc_Tcenv_Bindings*)_tmp169;_tmp164->ctrl_env=(const struct _tuple10*)_tmp168;_tmp164->capability=(void*)_tmp167;_tmp164->curr_rgn=(void*)_tmp166;_tmp164->flags=(struct Cyc_Tcenv_FenvFlags)_tmp165;_tmp164->fnrgn=(struct _RegionHandle*)r;_tmp164;});
# 571
return Cyc_Tcenv_put_fenv(r,te,_tmp163);};}
# 574
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 582
void*_tmp16E=Cyc_Tcutil_compress(r1);
void*_tmp16F=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp170=Cyc_Tcutil_typ_kind(_tmp16E);
struct Cyc_Absyn_Kind*_tmp171=Cyc_Tcutil_typ_kind(_tmp16F);
# 588
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp170,_tmp171);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp171,_tmp170);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp171));({struct Cyc_String_pa_PrintArg_struct _tmp176;_tmp176.tag=0;_tmp176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp170));({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp16F));({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp16E));({void*_tmp172[4]={& _tmp174,& _tmp175,& _tmp176,& _tmp177};Cyc_Tcutil_terr(loc,({const char*_tmp173="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp173,sizeof(char),82);}),_tag_dyneither(_tmp172,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 595
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp178=Cyc_Tcutil_swap_kind(_tmp16F,Cyc_Tcutil_kind_to_bound(_tmp170));struct _tuple11 _tmp179=_tmp178;struct Cyc_Absyn_Tvar*_tmp17C;void*_tmp17B;_LL71: _tmp17C=_tmp179.f1;_tmp17B=_tmp179.f2;_LL72:;
*oldtv=({struct _tuple11*_tmp17A=_region_malloc(r,sizeof(*_tmp17A));_tmp17A->f1=_tmp17C;_tmp17A->f2=_tmp17B;_tmp17A;});}else{
# 599
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp17D=Cyc_Tcutil_swap_kind(_tmp16E,Cyc_Tcutil_kind_to_bound(_tmp171));struct _tuple11 _tmp17E=_tmp17D;struct Cyc_Absyn_Tvar*_tmp181;void*_tmp180;_LL74: _tmp181=_tmp17E.f1;_tmp180=_tmp17E.f2;_LL75:;
*oldtv=({struct _tuple11*_tmp17F=_region_malloc(r,sizeof(*_tmp17F));_tmp17F->f1=_tmp181;_tmp17F->f2=_tmp180;_tmp17F;});}}}{
# 605
struct _RegionHandle*_tmp182=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp183=0;
if((_tmp16E != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp16E != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp16E != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp187;_tmp187.tag=23;_tmp187.f1=_tmp16E;_tmp187;});_tmp186;});
_tmp183=({struct Cyc_List_List*_tmp184=_region_malloc(_tmp182,sizeof(*_tmp184));_tmp184->hd=({struct _tuple14*_tmp185=_region_malloc(_tmp182,sizeof(*_tmp185));_tmp185->f1=eff1;_tmp185->f2=_tmp16F;_tmp185;});_tmp184->tl=_tmp183;_tmp184;});}
# 611
if((_tmp16F != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp16F != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp16F != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp18B;_tmp18B.tag=23;_tmp18B.f1=_tmp16F;_tmp18B;});_tmp18A;});
_tmp183=({struct Cyc_List_List*_tmp188=_region_malloc(_tmp182,sizeof(*_tmp188));_tmp188->hd=({struct _tuple14*_tmp189=_region_malloc(_tmp182,sizeof(*_tmp189));_tmp189->f1=eff2;_tmp189->f2=_tmp16E;_tmp189;});_tmp188->tl=_tmp183;_tmp188;});}
# 616
return Cyc_Tcenv_new_outlives_constraints(_tmp182,te,_tmp183,loc);};}
# 619
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18C=te->le;
if(_tmp18C == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp18C;
struct Cyc_Tcenv_Fenv*_tmp18D=fe;void*_tmp18E;_LL77: _tmp18E=_tmp18D->curr_rgn;_LL78:;
return _tmp18E;};}
# 630
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp197="check_rgn_accessible";_tag_dyneither(_tmp197,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp18F=fe;void*_tmp196;struct Cyc_RgnOrder_RgnPO*_tmp195;_LL7A: _tmp196=_tmp18F->capability;_tmp195=_tmp18F->region_order;_LL7B:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp196) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp196))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp195,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp191;_tmp191.tag=23;_tmp191.f1=rgn;_tmp191;});_tmp190;}),_tmp196))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp194;_tmp194.tag=0;_tmp194.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp192[1]={& _tmp194};Cyc_Tcutil_terr(loc,({const char*_tmp193="Expression accesses unavailable region %s";_tag_dyneither(_tmp193,sizeof(char),42);}),_tag_dyneither(_tmp192,sizeof(void*),1));});});}
# 642
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp198=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1A3="check_rgn_resetable";_tag_dyneither(_tmp1A3,sizeof(char),20);}));
# 644
struct Cyc_Tcenv_Fenv*_tmp199=_tmp198;struct Cyc_RgnOrder_RgnPO*_tmp1A2;_LL7D: _tmp1A2=_tmp199->region_order;_LL7E:;{
# 646
void*_tmp19A=Cyc_Tcutil_compress(rgn);void*_tmp19B=_tmp19A;struct Cyc_Absyn_Tvar*_tmp1A1;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19B)->tag == 2){_LL80: _tmp1A1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19B)->f1;_LL81:
# 648
 if(!Cyc_RgnOrder_is_region_resetable(_tmp1A2,_tmp1A1))
({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp19C[1]={& _tmp19E};Cyc_Tcutil_terr(loc,({const char*_tmp19D="Region %s is not resetable";_tag_dyneither(_tmp19D,sizeof(char),27);}),_tag_dyneither(_tmp19C,sizeof(void*),1));});});
return;}else{_LL82: _LL83:
({void*_tmp19F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A0="check_rgn_resetable";_tag_dyneither(_tmp1A0,sizeof(char),20);}),_tag_dyneither(_tmp19F,sizeof(void*),0));});}_LL7F:;};};}
# 658
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp1A4=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 663
if(_tmp1A4 == 0){
void*_tmp1A5=rt_a;switch(*((int*)_tmp1A5)){case 22: _LL85: _LL86:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LL87: _LL88:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LL89: _LL8A:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL8B: _LL8C:
 return 0;}_LL84:;}{
# 671
struct Cyc_Tcenv_Fenv*fe=_tmp1A4;
struct Cyc_Tcenv_Fenv*_tmp1A6=fe;struct Cyc_RgnOrder_RgnPO*_tmp1AA;_LL8E: _tmp1AA=_tmp1A6->region_order;_LL8F:;{
int _tmp1A7=Cyc_RgnOrder_effect_outlives(_tmp1AA,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1A9;_tmp1A9.tag=23;_tmp1A9.f1=rt_a;_tmp1A9;});_tmp1A8;}),rt_b);
# 677
return _tmp1A7;};};}struct _tuple15{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 682
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp1AB=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1B2="check_effect_accessible";_tag_dyneither(_tmp1B2,sizeof(char),24);}));
# 683
struct Cyc_Tcenv_Fenv*_tmp1AC=_tmp1AB;void*_tmp1B1;struct Cyc_RgnOrder_RgnPO*_tmp1B0;struct Cyc_Tcenv_SharedFenv*_tmp1AF;_LL91: _tmp1B1=_tmp1AC->capability;_tmp1B0=_tmp1AC->region_order;_tmp1AF=_tmp1AC->shared;_LL92:;
# 685
if(Cyc_Tcutil_subset_effect(0,eff,_tmp1B1))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp1B0,eff,_tmp1B1))
return;{
struct _RegionHandle*frgn=_tmp1AF->frgn;
_tmp1AF->delayed_effect_checks=({struct Cyc_List_List*_tmp1AD=_region_malloc(frgn,sizeof(*_tmp1AD));_tmp1AD->hd=({struct _tuple15*_tmp1AE=_region_malloc(frgn,sizeof(*_tmp1AE));_tmp1AE->f1=_tmp1B1;_tmp1AE->f2=eff;_tmp1AE->f3=_tmp1B0;_tmp1AE->f4=loc;_tmp1AE;});_tmp1AD->tl=_tmp1AF->delayed_effect_checks;_tmp1AD;});};}
# 694
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1B3=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1C1="check_delayed_effects";_tag_dyneither(_tmp1C1,sizeof(char),22);}));
# 695
struct Cyc_Tcenv_Fenv*_tmp1B4=_tmp1B3;struct Cyc_Tcenv_SharedFenv*_tmp1C0;_LL94: _tmp1C0=_tmp1B4->shared;_LL95:;{
# 697
struct Cyc_List_List*_tmp1B5=_tmp1C0->delayed_effect_checks;
for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){
struct _tuple15*_tmp1B6=(struct _tuple15*)_tmp1B5->hd;struct _tuple15*_tmp1B7=_tmp1B6;void*_tmp1BF;void*_tmp1BE;struct Cyc_RgnOrder_RgnPO*_tmp1BD;unsigned int _tmp1BC;_LL97: _tmp1BF=_tmp1B7->f1;_tmp1BE=_tmp1B7->f2;_tmp1BD=_tmp1B7->f3;_tmp1BC=_tmp1B7->f4;_LL98:;
if(Cyc_Tcutil_subset_effect(1,_tmp1BE,_tmp1BF))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp1BD,_tmp1BE,_tmp1BF))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1BE));({struct Cyc_String_pa_PrintArg_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1BF));({void*_tmp1B8[2]={& _tmp1BA,& _tmp1BB};Cyc_Tcutil_terr(_tmp1BC,({const char*_tmp1B9="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp1B9,sizeof(char),51);}),_tag_dyneither(_tmp1B8,sizeof(void*),2));});});});}};}struct _tuple16{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 713
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 715
struct Cyc_Tcenv_Fenv*_tmp1C2=te->le;
if(_tmp1C2 == 0){
# 718
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple14*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1C4;_tmp1C4.tag=23;_tmp1C4.f1=(*((struct _tuple14*)po->hd)).f2;_tmp1C4;});_tmp1C3;}),Cyc_Absyn_empty_effect))
({void*_tmp1C5=0;Cyc_Tcutil_terr(loc,({const char*_tmp1C6="the required region ordering is not satisfied here";_tag_dyneither(_tmp1C6,sizeof(char),51);}),_tag_dyneither(_tmp1C5,sizeof(void*),0));});}
return;}{
# 724
struct Cyc_Tcenv_Fenv*_tmp1C7=_tmp1C2;struct Cyc_Tcenv_Fenv*_tmp1C8=_tmp1C7;struct Cyc_RgnOrder_RgnPO*_tmp1CC;struct Cyc_Tcenv_SharedFenv*_tmp1CB;_LL9A: _tmp1CC=_tmp1C8->region_order;_tmp1CB=_tmp1C8->shared;_LL9B:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp1CC,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp1CB->delayed_constraint_checks=({struct Cyc_List_List*_tmp1C9=_region_malloc(_tmp1CB->frgn,sizeof(*_tmp1C9));_tmp1C9->hd=({struct _tuple16*_tmp1CA=_region_malloc(_tmp1CB->frgn,sizeof(*_tmp1CA));_tmp1CA->f1=_tmp1CC;_tmp1CA->f2=po;_tmp1CA->f3=loc;_tmp1CA;});_tmp1C9->tl=_tmp1CB->delayed_constraint_checks;_tmp1C9;});};}
# 731
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1CD=
Cyc_Tcenv_get_fenv(te,({const char*_tmp1D8="check_delayed_constraints";_tag_dyneither(_tmp1D8,sizeof(char),26);}));
# 732
struct Cyc_Tcenv_Fenv*_tmp1CE=_tmp1CD;struct Cyc_Tcenv_SharedFenv*_tmp1D7;_LL9D: _tmp1D7=_tmp1CE->shared;_LL9E:;{
# 734
struct Cyc_List_List*_tmp1CF=_tmp1D7->delayed_constraint_checks;
for(0;_tmp1CF != 0;_tmp1CF=_tmp1CF->tl){
struct _tuple16*_tmp1D0=(struct _tuple16*)_tmp1CF->hd;struct _tuple16*_tmp1D1=_tmp1D0;struct Cyc_RgnOrder_RgnPO*_tmp1D6;struct Cyc_List_List*_tmp1D5;unsigned int _tmp1D4;_LLA0: _tmp1D6=_tmp1D1->f1;_tmp1D5=_tmp1D1->f2;_tmp1D4=_tmp1D1->f3;_LLA1:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp1D6,_tmp1D5,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp1D2=0;Cyc_Tcutil_terr(_tmp1D4,({const char*_tmp1D3="the required region ordering is not satisfied here";_tag_dyneither(_tmp1D3,sizeof(char),51);}),_tag_dyneither(_tmp1D2,sizeof(void*),0));});}};}
# 742
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->name=({struct _dyneither_ptr*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0;_tmp1F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp1F5[1]={& _tmp1F7};Cyc_aprintf(({const char*_tmp1F6="`%s";_tag_dyneither(_tmp1F6,sizeof(char),4);}),_tag_dyneither(_tmp1F5,sizeof(void*),1));});});_tmp1F4;});_tmp1F3->identity=
# 746
Cyc_Tcutil_new_tvar_id();_tmp1F3->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp1F3;});
struct Cyc_List_List*_tmp1D9=({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->hd=rgn0;_tmp1F2->tl=fd->tvs;_tmp1F2;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp1D9);{
# 750
struct Cyc_RgnOrder_RgnPO*_tmp1DA=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 752
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1F1;_tmp1F1.tag=2;_tmp1F1.f1=rgn0;_tmp1F1;});_tmp1F0;});
{struct Cyc_List_List*_tmp1DB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp1DB != 0;_tmp1DB=_tmp1DB->tl){
struct Cyc_Absyn_Vardecl*_tmp1DC=(struct Cyc_Absyn_Vardecl*)_tmp1DB->hd;
_tmp1DC->rgn=param_rgn;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->v=(*_tmp1DC->name).f2;_tmp1DD->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp1DF;_tmp1DF.tag=3;_tmp1DF.f1=_tmp1DC;_tmp1DF;});_tmp1DE;});_tmp1DD->tl=locals;_tmp1DD;});}}
# 758
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp1E0=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp1E1=_tmp1E0;struct _dyneither_ptr*_tmp1E7;struct Cyc_Absyn_Tqual _tmp1E6;void*_tmp1E5;int _tmp1E4;_LLA3: _tmp1E7=_tmp1E1.name;_tmp1E6=_tmp1E1.tq;_tmp1E5=_tmp1E1.type;_tmp1E4=_tmp1E1.inject;_LLA4:;{
# 761
struct Cyc_List_List*_tmp1E2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
struct Cyc_Absyn_Vardecl*_tmp1E3=(struct Cyc_Absyn_Vardecl*)_tmp1E2->hd;
if(Cyc_strptrcmp((*_tmp1E3->name).f2,(struct _dyneither_ptr*)_check_null(_tmp1E7))== 0){
_tmp1E3->type=Cyc_Absyn_dyneither_typ(_tmp1E5,param_rgn,_tmp1E6,Cyc_Absyn_false_conref);
break;}}};}
# 769
return({struct Cyc_Tcenv_Fenv*_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));_tmp1E8->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp1EF=_region_malloc(r,sizeof(*_tmp1EF));_tmp1EF->frgn=r;_tmp1EF->return_typ=fd->ret_type;_tmp1EF->delayed_effect_checks=0;_tmp1EF->delayed_constraint_checks=0;_tmp1EF;});_tmp1E8->type_vars=(struct Cyc_List_List*)_tmp1D9;_tmp1E8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1DA;_tmp1E8->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp1E8->ctrl_env=(const struct _tuple10*)0;_tmp1E8->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1EA;_tmp1EA.tag=24;_tmp1EA.f1=({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1EE;_tmp1EE.tag=23;_tmp1EE.f1=param_rgn;_tmp1EE;});_tmp1ED;});_tmp1EB->tl=({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=(void*)_check_null(fd->effect);_tmp1EC->tl=0;_tmp1EC;});_tmp1EB;});_tmp1EA;});_tmp1E9;}));_tmp1E8->curr_rgn=(void*)param_rgn;_tmp1E8->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp1E8->flags).in_notreadctxt=0;(_tmp1E8->flags).in_lhs=0;(_tmp1E8->flags).in_new=0;(_tmp1E8->flags).abstract_ok=0;_tmp1E8->flags;});_tmp1E8->fnrgn=(struct _RegionHandle*)r;_tmp1E8;});};}
# 788
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp1F8=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp224;struct Cyc_RgnOrder_RgnPO*_tmp223;struct Cyc_List_List*_tmp222;struct Cyc_Tcenv_SharedFenv*_tmp221;struct _RegionHandle*_tmp220;_LLA6: _tmp224=_tmp1F8->locals;_tmp223=_tmp1F8->region_order;_tmp222=_tmp1F8->type_vars;_tmp221=_tmp1F8->shared;_tmp220=_tmp1F8->fnrgn;_LLA7:;{
# 791
struct _RegionHandle*_tmp1F9=_tmp221->frgn;
const struct Cyc_Tcenv_Bindings*_tmp1FA=_tmp224;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->name=({struct _dyneither_ptr*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp21F;_tmp21F.tag=0;_tmp21F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp21D[1]={& _tmp21F};Cyc_aprintf(({const char*_tmp21E="`%s";_tag_dyneither(_tmp21E,sizeof(char),4);}),_tag_dyneither(_tmp21D,sizeof(void*),1));});});_tmp21C;});_tmp21B->identity=
# 795
Cyc_Tcutil_new_tvar_id();_tmp21B->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp21B;});
# 797
{struct Cyc_List_List*_tmp1FB=fd->tvs;for(0;_tmp1FB != 0;_tmp1FB=_tmp1FB->tl){
struct Cyc_Absyn_Kind*_tmp1FC=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1FB->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1FD=_tmp1FC;enum Cyc_Absyn_KindQual _tmp201;enum Cyc_Absyn_AliasQual _tmp200;_LLA9: _tmp201=_tmp1FD->kind;_tmp200=_tmp1FD->aliasqual;_LLAA:;
if(_tmp201 == Cyc_Absyn_RgnKind){
if(_tmp200 == Cyc_Absyn_Aliasable)
_tmp223=Cyc_RgnOrder_add_youngest(_tmp1F9,_tmp223,(struct Cyc_Absyn_Tvar*)_tmp1FB->hd,0,0);else{
# 803
({void*_tmp1FE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1FF="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp1FF,sizeof(char),39);}),_tag_dyneither(_tmp1FE,sizeof(void*),0));});}}}}
# 805
_tmp223=Cyc_RgnOrder_add_youngest(_tmp1F9,_tmp223,rgn0,0,0);{
struct Cyc_List_List*_tmp202=({struct Cyc_List_List*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->hd=rgn0;_tmp21A->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp222);_tmp21A;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp202);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp219;_tmp219.tag=2;_tmp219.f1=rgn0;_tmp219;});_tmp218;});
{struct Cyc_List_List*_tmp203=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp203 != 0;_tmp203=_tmp203->tl){
struct Cyc_Absyn_Vardecl*_tmp204=(struct Cyc_Absyn_Vardecl*)_tmp203->hd;
_tmp204->rgn=param_rgn;
_tmp1FA=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp205=_region_malloc(_tmp1F9,sizeof(*_tmp205));_tmp205->v=(*_tmp204->name).f2;_tmp205->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp207;_tmp207.tag=3;_tmp207.f1=_tmp204;_tmp207;});_tmp206;});_tmp205->tl=_tmp1FA;_tmp205;});}}
# 814
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp208=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp209=_tmp208;struct _dyneither_ptr*_tmp20F;struct Cyc_Absyn_Tqual _tmp20E;void*_tmp20D;int _tmp20C;_LLAC: _tmp20F=_tmp209.name;_tmp20E=_tmp209.tq;_tmp20D=_tmp209.type;_tmp20C=_tmp209.inject;_LLAD:;{
# 817
struct Cyc_List_List*_tmp20A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp20A != 0;_tmp20A=_tmp20A->tl){
struct Cyc_Absyn_Vardecl*_tmp20B=(struct Cyc_Absyn_Vardecl*)_tmp20A->hd;
if(Cyc_strptrcmp((*_tmp20B->name).f2,(struct _dyneither_ptr*)_check_null(_tmp20F))== 0){
_tmp20B->type=Cyc_Absyn_dyneither_typ(_tmp20D,param_rgn,_tmp20E,Cyc_Absyn_false_conref);
break;}}};}
# 825
return({struct Cyc_Tcenv_Fenv*_tmp210=_region_malloc(_tmp1F9,sizeof(*_tmp210));_tmp210->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp217=_region_malloc(_tmp1F9,sizeof(*_tmp217));_tmp217->frgn=_tmp1F9;_tmp217->return_typ=fd->ret_type;_tmp217->delayed_effect_checks=0;_tmp217->delayed_constraint_checks=0;_tmp217;});_tmp210->type_vars=(struct Cyc_List_List*)_tmp202;_tmp210->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp223;_tmp210->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FA;_tmp210->ctrl_env=(const struct _tuple10*)0;_tmp210->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp212;_tmp212.tag=24;_tmp212.f1=({struct Cyc_List_List*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp216;_tmp216.tag=23;_tmp216.f1=param_rgn;_tmp216;});_tmp215;});_tmp213->tl=({struct Cyc_List_List*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214->hd=(void*)_check_null(fd->effect);_tmp214->tl=0;_tmp214;});_tmp213;});_tmp212;});_tmp211;}));_tmp210->curr_rgn=(void*)param_rgn;_tmp210->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp210->flags).in_notreadctxt=0;(_tmp210->flags).in_lhs=0;(_tmp210->flags).in_new=0;(_tmp210->flags).abstract_ok=0;_tmp210->flags;});_tmp210->fnrgn=(struct _RegionHandle*)_tmp220;_tmp210;});};};};}
# 844
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 847
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp244->identity=Cyc_Tcutil_new_tvar_id();_tmp244->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp244;});
struct Cyc_List_List*_tmp225=({struct Cyc_List_List*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->hd=rgn0;_tmp243->tl=0;_tmp243;});
struct Cyc_RgnOrder_RgnPO*_tmp226=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp242;_tmp242.tag=24;_tmp242.f1=0;_tmp242;});_tmp241;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp240;_tmp240.tag=2;_tmp240.f1=rgn0;_tmp240;});_tmp23F;});
struct Cyc_List_List*_tmp227=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp228=*((struct _tuple8*)args->hd);struct _tuple8 _tmp229=_tmp228;struct _dyneither_ptr*_tmp237;struct Cyc_Absyn_Tqual _tmp236;void*_tmp235;_LLAF: _tmp237=_tmp229.f1;_tmp236=_tmp229.f2;_tmp235=_tmp229.f3;_LLB0:;
if(_tmp237 != 0){
struct Cyc_Absyn_Vardecl*_tmp22A=({struct Cyc_Absyn_Vardecl*_tmp232=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp232[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp233;_tmp233.sc=Cyc_Absyn_Public;_tmp233.name=({struct _tuple0*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->f1=Cyc_Absyn_Loc_n;_tmp234->f2=_tmp237;_tmp234;});_tmp233.tq=_tmp236;_tmp233.type=_tmp235;_tmp233.initializer=0;_tmp233.rgn=param_rgn;_tmp233.attributes=0;_tmp233.escapes=0;_tmp233;});_tmp232;});
# 863
_tmp227=({struct Cyc_List_List*_tmp22B=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp22B[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp22C;_tmp22C.hd=_tmp22A;_tmp22C.tl=_tmp227;_tmp22C;});_tmp22B;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp22D=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp231;_tmp231.tag=3;_tmp231.f1=_tmp22A;_tmp231;});_tmp230;});
struct _dyneither_ptr*_tmp22E=_tmp237;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp22F=_region_malloc(r,sizeof(*_tmp22F));_tmp22F->v=_tmp22E;_tmp22F->b=(void*)_tmp22D;_tmp22F->tl=locals;_tmp22F;});};}}
# 869
return({struct Cyc_Tcenv_Fenv*_tmp238=_region_malloc(r,sizeof(*_tmp238));_tmp238->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp23E=_region_malloc(r,sizeof(*_tmp23E));_tmp23E->frgn=r;_tmp23E->return_typ=ret_type;_tmp23E->delayed_effect_checks=0;_tmp23E->delayed_constraint_checks=0;_tmp23E;});_tmp238->type_vars=(struct Cyc_List_List*)_tmp225;_tmp238->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp226;_tmp238->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp238->ctrl_env=(const struct _tuple10*)0;_tmp238->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp23A;_tmp23A.tag=24;_tmp23A.f1=({struct Cyc_List_List*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp23D;_tmp23D.tag=23;_tmp23D.f1=param_rgn;_tmp23D;});_tmp23C;});_tmp23B->tl=0;_tmp23B;});_tmp23A;});_tmp239;}));_tmp238->curr_rgn=(void*)param_rgn;_tmp238->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp238->flags).in_notreadctxt=0;(_tmp238->flags).in_lhs=0;(_tmp238->flags).in_new=0;(_tmp238->flags).abstract_ok=0;_tmp238->flags;});_tmp238->fnrgn=(struct _RegionHandle*)r;_tmp238;});}
