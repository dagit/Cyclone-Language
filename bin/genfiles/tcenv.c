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
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_SlowDict_Dict;
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
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_RgnOrder_RgnPO;
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
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 71
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 75
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 86
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 88
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 90
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 102 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 107
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 111
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 118
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 121
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 124
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 126
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 129
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 132
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 134
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 138
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 155
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 160
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 165
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 168
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 176
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 178
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 180
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 182
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 184
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 188
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 191
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 194
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 197
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
# 117
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{const struct _tuple10*fallthru_clause;};
# 123
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 146 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 180 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 183
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)
return bs->b;}
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 192
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
return({struct Cyc_Tcenv_Genv*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->grgn=r;_tmp2->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp2->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp2;});}struct _tuple12{void*f1;int f2;};
# 202
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 204
Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp3;});{
# 206
struct Cyc_Tcenv_Genv*_tmp4=Cyc_Tcenv_empty_genv(r);
_tmp4->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Datatypedecl**_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=Cyc_Absyn_exn_tud;_tmp5;}));
{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
_tmp4->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp4->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd)->name).f2,({struct _tuple12*_tmp7=_region_malloc(r,sizeof(*_tmp7));_tmp7->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp9;_tmp9.tag=2;_tmp9.f1=Cyc_Absyn_exn_tud;_tmp9.f2=(struct Cyc_Absyn_Datatypefield*)_tmp6->hd;_tmp9;});_tmp8;});_tmp7->f2=1;_tmp7;}));}}{
# 213
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp4);
return({struct Cyc_Tcenv_Tenv*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->ns=0;_tmpA->ae=ae;_tmpA->le=0;_tmpA->allow_valueof=0;_tmpA->in_extern_c_include=0;_tmpA;});};};}
# 219
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0)
return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpB="outer_namespace";_tag_dyneither(_tmpB,sizeof(char),16);}));
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 225
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 232
if(n2 != 0)return 0;
return 1;}
# 236
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmpC=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmpC,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 243
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple0*_tmpD=q;union Cyc_Absyn_Nmspace _tmp15;struct _dyneither_ptr*_tmp14;_LL1: _tmp15=_tmpD->f1;_tmp14=_tmpD->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpE=_tmp15;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp12;switch((_tmpE.Abs_n).tag){case 3: _LL4: _tmp12=(_tmpE.C_n).val;_LL5:
 _tmp13=_tmp12;goto _LL7;case 2: _LL6: _tmp13=(_tmpE.Abs_n).val;_LL7:
# 248
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp13),_tmp14,is_use);default: _LL8: _LL9:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Impossible_exn_struct _tmp10;_tmp10.tag=Cyc_Core_Impossible;_tmp10.f1=({const char*_tmp11="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp11,sizeof(char),37);});_tmp10;});_tmpF;}));}_LL3:;};}
# 253
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp16=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp16,v);}
# 257
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp17=q;union Cyc_Absyn_Nmspace _tmp23;struct _dyneither_ptr*_tmp22;_LLB: _tmp23=_tmp17->f1;_tmp22=_tmp17->f2;_LLC:;{
union Cyc_Absyn_Nmspace _tmp18=_tmp23;struct Cyc_List_List*_tmp21;struct Cyc_List_List*_tmp20;switch((_tmp18.Abs_n).tag){case 3: _LLE: _tmp20=(_tmp18.C_n).val;_LLF:
 _tmp21=_tmp20;goto _LL11;case 2: _LL10: _tmp21=(_tmp18.Abs_n).val;_LL11: {
# 262
struct Cyc_Dict_Dict _tmp19=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp21))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp19,_tmp22);}default: _LL12: _LL13:
# 265
({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q));({void*_tmp1A[1]={& _tmp1C};Cyc_Tcutil_terr(loc,({const char*_tmp1B="??? %s";_tag_dyneither(_tmp1B,sizeof(char),7);}),_tag_dyneither(_tmp1A,sizeof(void*),1));});});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Core_Impossible_exn_struct _tmp1E;_tmp1E.tag=Cyc_Core_Impossible;_tmp1E.f1=({const char*_tmp1F="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp1F,sizeof(char),37);});_tmp1E;});_tmp1D;}));}_LLD:;};}
# 270
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp24=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp24,v);}
# 274
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp25=q;union Cyc_Absyn_Nmspace _tmp2E;struct _dyneither_ptr*_tmp2D;_LL15: _tmp2E=_tmp25->f1;_tmp2D=_tmp25->f2;_LL16:;{
union Cyc_Absyn_Nmspace _tmp26=_tmp2E;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2B;switch((_tmp26.Abs_n).tag){case 3: _LL18: _tmp2B=(_tmp26.C_n).val;_LL19:
 _tmp2C=_tmp2B;goto _LL1B;case 2: _LL1A: _tmp2C=(_tmp26.Abs_n).val;_LL1B: {
# 279
struct Cyc_Dict_Dict _tmp27=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2C))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27,_tmp2D);}default: _LL1C: _LL1D:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Core_Impossible_exn_struct _tmp29;_tmp29.tag=Cyc_Core_Impossible;_tmp29.f1=({const char*_tmp2A="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp2A,sizeof(char),37);});_tmp29;});_tmp28;}));}_LL17:;};}
# 285
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 289
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp2F=q;union Cyc_Absyn_Nmspace _tmp39;struct _dyneither_ptr*_tmp38;_LL1F: _tmp39=_tmp2F->f1;_tmp38=_tmp2F->f2;_LL20:;{
union Cyc_Absyn_Nmspace _tmp30=_tmp39;struct Cyc_List_List*_tmp37;struct Cyc_List_List*_tmp36;switch((_tmp30.Abs_n).tag){case 3: _LL22: _tmp36=(_tmp30.C_n).val;_LL23:
 _tmp37=_tmp36;goto _LL25;case 2: _LL24: _tmp37=(_tmp30.Abs_n).val;_LL25: {
# 294
struct Cyc_Dict_Dict _tmp31=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp37))->datatypedecls;
return({struct Cyc_Absyn_Datatypedecl***_tmp32=_region_malloc(r,sizeof(*_tmp32));_tmp32[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp31,_tmp38);_tmp32;});}default: _LL26: _LL27:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Core_Impossible_exn_struct _tmp34;_tmp34.tag=Cyc_Core_Impossible;_tmp34.f1=({const char*_tmp35="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp35,sizeof(char),37);});_tmp34;});_tmp33;}));}_LL21:;};}
# 300
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp3A=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp3A,v);}
# 304
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp3B=q;union Cyc_Absyn_Nmspace _tmp44;struct _dyneither_ptr*_tmp43;_LL29: _tmp44=_tmp3B->f1;_tmp43=_tmp3B->f2;_LL2A:;{
union Cyc_Absyn_Nmspace _tmp3C=_tmp44;struct Cyc_List_List*_tmp42;struct Cyc_List_List*_tmp41;switch((_tmp3C.Abs_n).tag){case 3: _LL2C: _tmp41=(_tmp3C.C_n).val;_LL2D:
 _tmp42=_tmp41;goto _LL2F;case 2: _LL2E: _tmp42=(_tmp3C.Abs_n).val;_LL2F: {
# 309
struct Cyc_Dict_Dict _tmp3D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp42))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp3D,_tmp43);}default: _LL30: _LL31:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=({struct Cyc_Core_Impossible_exn_struct _tmp3F;_tmp3F.tag=Cyc_Core_Impossible;_tmp3F.f1=({const char*_tmp40="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp40,sizeof(char),37);});_tmp3F;});_tmp3E;}));}_LL2B:;};}
# 315
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp45=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp45,v);}
# 319
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
struct _tuple0*_tmp46=q;union Cyc_Absyn_Nmspace _tmp4F;struct _dyneither_ptr*_tmp4E;_LL33: _tmp4F=_tmp46->f1;_tmp4E=_tmp46->f2;_LL34:;{
union Cyc_Absyn_Nmspace _tmp47=_tmp4F;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4C;switch((_tmp47.Abs_n).tag){case 3: _LL36: _tmp4C=(_tmp47.C_n).val;_LL37:
 _tmp4D=_tmp4C;goto _LL39;case 2: _LL38: _tmp4D=(_tmp47.Abs_n).val;_LL39: {
# 324
struct Cyc_Dict_Dict _tmp48=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4D))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp48,_tmp4E);}default: _LL3A: _LL3B:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Core_Impossible_exn_struct _tmp4A;_tmp4A.tag=Cyc_Core_Impossible;_tmp4A.f1=({const char*_tmp4B="compiler-bug: relative name in tcenv";_tag_dyneither(_tmp4B,sizeof(char),37);});_tmp4A;});_tmp49;}));}_LL35:;};}
# 332
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp50=te->le;
if(_tmp50 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp50;}
# 338
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp51=te->le;
if(_tmp51 != 0){
struct Cyc_Tcenv_Fenv*_tmp52=_tmp51;struct Cyc_Tcenv_Fenv*_tmp53=_tmp52;struct _RegionHandle*_tmp54;_LL3D: _tmp54=_tmp53->fnrgn;_LL3E:;
return _tmp54;}
# 344
return Cyc_Core_heap_region;}
# 347
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp55="put_fenv";_tag_dyneither(_tmp55,sizeof(char),9);}));
return({struct Cyc_Tcenv_Tenv*_tmp56=_region_malloc(l,sizeof(*_tmp56));_tmp56->ns=te->ns;_tmp56->ae=te->ae;_tmp56->le=fe;_tmp56->allow_valueof=te->allow_valueof;_tmp56->in_extern_c_include=te->in_extern_c_include;_tmp56;});}
# 352
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Tcenv_Tenv*_tmp57=_region_malloc(l,sizeof(*_tmp57));_tmp57->ns=te->ns;_tmp57->ae=te->ae;_tmp57->le=0;_tmp57->allow_valueof=te->allow_valueof;_tmp57->in_extern_c_include=te->in_extern_c_include;_tmp57;});}
# 356
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 358
struct Cyc_Tcenv_Fenv _tmp58=*f;struct Cyc_Tcenv_Fenv _tmp59=_tmp58;struct Cyc_Tcenv_SharedFenv*_tmp63;struct Cyc_List_List*_tmp62;struct Cyc_RgnOrder_RgnPO*_tmp61;const struct Cyc_Tcenv_Bindings*_tmp60;struct Cyc_Tcenv_CtrlEnv*_tmp5F;void*_tmp5E;void*_tmp5D;struct Cyc_Tcenv_FenvFlags _tmp5C;struct _RegionHandle*_tmp5B;_LL40: _tmp63=_tmp59.shared;_tmp62=_tmp59.type_vars;_tmp61=_tmp59.region_order;_tmp60=_tmp59.locals;_tmp5F=_tmp59.ctrl_env;_tmp5E=_tmp59.capability;_tmp5D=_tmp59.curr_rgn;_tmp5C=_tmp59.flags;_tmp5B=_tmp59.fnrgn;_LL41:;
# 360
return({struct Cyc_Tcenv_Fenv*_tmp5A=_region_malloc(l,sizeof(*_tmp5A));_tmp5A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp63;_tmp5A->type_vars=(struct Cyc_List_List*)_tmp62;_tmp5A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp61;_tmp5A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp60;_tmp5A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp5F;_tmp5A->capability=(void*)_tmp5E;_tmp5A->curr_rgn=(void*)_tmp5D;_tmp5A->flags=(struct Cyc_Tcenv_FenvFlags)_tmp5C;_tmp5A->fnrgn=(struct _RegionHandle*)l;_tmp5A;});}
# 364
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 366
struct Cyc_Tcenv_Fenv _tmp64=*f;struct Cyc_Tcenv_Fenv _tmp65=_tmp64;struct Cyc_Tcenv_SharedFenv*_tmp73;struct Cyc_List_List*_tmp72;struct Cyc_RgnOrder_RgnPO*_tmp71;const struct Cyc_Tcenv_Bindings*_tmp70;struct Cyc_Tcenv_CtrlEnv*_tmp6F;void*_tmp6E;void*_tmp6D;struct Cyc_Tcenv_FenvFlags _tmp6C;_LL43: _tmp73=_tmp65.shared;_tmp72=_tmp65.type_vars;_tmp71=_tmp65.region_order;_tmp70=_tmp65.locals;_tmp6F=_tmp65.ctrl_env;_tmp6E=_tmp65.capability;_tmp6D=_tmp65.curr_rgn;_tmp6C=_tmp65.flags;_LL44:;{
# 369
struct Cyc_Tcenv_CtrlEnv _tmp66=*_tmp6F;struct Cyc_Tcenv_CtrlEnv _tmp67=_tmp66;const struct _tuple10*_tmp6B;_LL46: _tmp6B=_tmp67.fallthru_clause;_LL47:;{
struct Cyc_Tcenv_CtrlEnv*_tmp68=({struct Cyc_Tcenv_CtrlEnv*_tmp6A=_region_malloc(l,sizeof(*_tmp6A));_tmp6A->fallthru_clause=_tmp6B;_tmp6A;});
# 372
return({struct Cyc_Tcenv_Fenv*_tmp69=_region_malloc(l,sizeof(*_tmp69));_tmp69->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp73;_tmp69->type_vars=(struct Cyc_List_List*)_tmp72;_tmp69->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp71;_tmp69->locals=(const struct Cyc_Tcenv_Bindings*)_tmp70;_tmp69->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp68;_tmp69->capability=(void*)_tmp6E;_tmp69->curr_rgn=(void*)_tmp6D;_tmp69->flags=(struct Cyc_Tcenv_FenvFlags)_tmp6C;_tmp69->fnrgn=(struct _RegionHandle*)l;_tmp69;});};};}
# 378
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp74=Cyc_Tcenv_get_fenv(te,({const char*_tmp7A="return_typ";_tag_dyneither(_tmp7A,sizeof(char),11);}));struct Cyc_Tcenv_Fenv*_tmp75=_tmp74;struct Cyc_Tcenv_SharedFenv*_tmp79;_LL49: _tmp79=_tmp75->shared;_LL4A:;{
struct Cyc_Tcenv_SharedFenv _tmp76=*_tmp79;struct Cyc_Tcenv_SharedFenv _tmp77=_tmp76;void*_tmp78;_LL4C: _tmp78=_tmp77.return_typ;_LL4D:;
return _tmp78;};}
# 384
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp7B=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmp7C=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmp7B));struct Cyc_Tcenv_Fenv _tmp7D=_tmp7C;struct Cyc_List_List*_tmp7E;_LL4F: _tmp7E=_tmp7D.type_vars;_LL50:;
return _tmp7E;};}
# 391
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->v=Cyc_Tcenv_lookup_type_vars(te);_tmp7F;});}
# 395
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 397
struct Cyc_Tcenv_Fenv*_tmp80=
# 399
Cyc_Tcenv_get_fenv(te,({const char*_tmp8D="add_type_vars";_tag_dyneither(_tmp8D,sizeof(char),14);}));
# 397
struct Cyc_Tcenv_Fenv*_tmp81=_tmp80;struct Cyc_Tcenv_SharedFenv*_tmp8C;struct Cyc_List_List*_tmp8B;struct Cyc_RgnOrder_RgnPO*_tmp8A;const struct Cyc_Tcenv_Bindings*_tmp89;struct Cyc_Tcenv_CtrlEnv*_tmp88;void*_tmp87;void*_tmp86;struct Cyc_Tcenv_FenvFlags _tmp85;_LL52: _tmp8C=_tmp81->shared;_tmp8B=_tmp81->type_vars;_tmp8A=_tmp81->region_order;_tmp89=_tmp81->locals;_tmp88=_tmp81->ctrl_env;_tmp87=_tmp81->capability;_tmp86=_tmp81->curr_rgn;_tmp85=_tmp81->flags;_LL53:;
# 401
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp82=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp8B);
Cyc_Tcutil_check_unique_tvars(loc,_tmp82);{
struct Cyc_Tcenv_Fenv*_tmp83=({struct Cyc_Tcenv_Fenv*_tmp84=_region_malloc(r,sizeof(*_tmp84));_tmp84->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp8C;_tmp84->type_vars=(struct Cyc_List_List*)_tmp82;_tmp84->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp8A;_tmp84->locals=(const struct Cyc_Tcenv_Bindings*)_tmp89;_tmp84->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp88;_tmp84->capability=(void*)_tmp87;_tmp84->curr_rgn=(void*)_tmp86;_tmp84->flags=(struct Cyc_Tcenv_FenvFlags)_tmp85;_tmp84->fnrgn=(struct _RegionHandle*)r;_tmp84;});
# 406
return Cyc_Tcenv_put_fenv(r,te,_tmp83);};};}
# 409
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp8E=te->le;
if(_tmp8E == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 414
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmp8E);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 419
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 421
struct _dyneither_ptr*_tmp8F=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmp90=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmp91=_tmp90;if((_tmp91.Loc_n).tag == 4){_LL55: _LL56:
 goto _LL54;}else{_LL57: _LL58:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Core_Impossible_exn_struct _tmp93;_tmp93.tag=Cyc_Core_Impossible;_tmp93.f1=({const char*_tmp94="add_local_var: called with non-local";_tag_dyneither(_tmp94,sizeof(char),37);});_tmp93;});_tmp92;}));}_LL54:;}
# 426
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp95;_push_handler(& _tmp95);{int _tmp97=0;if(setjmp(_tmp95.handler))_tmp97=1;if(!_tmp97){{void*_tmp98=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp99=_tmp98;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp99)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp99)->f1)){case 4: _LL5A: _LL5B:
({void*_tmp9A=0;Cyc_Tcutil_warn(loc,({const char*_tmp9B="declaration hides local";_tag_dyneither(_tmp9B,sizeof(char),24);}),_tag_dyneither(_tmp9A,sizeof(void*),0));});goto _LL59;case 1: _LL5C: _LL5D:
({void*_tmp9C=0;Cyc_Tcutil_warn(loc,({const char*_tmp9D="declaration hides global";_tag_dyneither(_tmp9D,sizeof(char),25);}),_tag_dyneither(_tmp9C,sizeof(void*),0));});goto _LL59;case 5: _LL5E: _LL5F:
({void*_tmp9E=0;Cyc_Tcutil_warn(loc,({const char*_tmp9F="declaration hides pattern variable";_tag_dyneither(_tmp9F,sizeof(char),35);}),_tag_dyneither(_tmp9E,sizeof(void*),0));});goto _LL59;case 3: _LL60: _LL61:
({void*_tmpA0=0;Cyc_Tcutil_warn(loc,({const char*_tmpA1="declaration hides parameter";_tag_dyneither(_tmpA1,sizeof(char),28);}),_tag_dyneither(_tmpA0,sizeof(void*),0));});goto _LL59;default: goto _LL62;}else{_LL62: _LL63:
 goto _LL59;}_LL59:;}
# 427
;_pop_handler();}else{void*_tmp96=(void*)_exn_thrown;void*_tmpA2=_tmp96;void*_tmpA3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA2)->tag == Cyc_Dict_Absent){_LL65: _LL66:
# 434
 goto _LL64;}else{_LL67: _tmpA3=_tmpA2;_LL68:(int)_rethrow(_tmpA3);}_LL64:;}};}{
struct Cyc_Tcenv_Fenv*_tmpA4=
# 437
Cyc_Tcenv_get_fenv(te,({const char*_tmpB5="add_local_var";_tag_dyneither(_tmpB5,sizeof(char),14);}));
# 435
struct Cyc_Tcenv_Fenv*_tmpA5=_tmpA4;struct Cyc_Tcenv_SharedFenv*_tmpB4;struct Cyc_List_List*_tmpB3;struct Cyc_RgnOrder_RgnPO*_tmpB2;const struct Cyc_Tcenv_Bindings*_tmpB1;struct Cyc_Tcenv_CtrlEnv*_tmpB0;void*_tmpAF;void*_tmpAE;struct Cyc_Tcenv_FenvFlags _tmpAD;_LL6A: _tmpB4=_tmpA5->shared;_tmpB3=_tmpA5->type_vars;_tmpB2=_tmpA5->region_order;_tmpB1=_tmpA5->locals;_tmpB0=_tmpA5->ctrl_env;_tmpAF=_tmpA5->capability;_tmpAE=_tmpA5->curr_rgn;_tmpAD=_tmpA5->flags;_LL6B:;{
# 438
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAC;_tmpAC.tag=4;_tmpAC.f1=vd;_tmpAC;});_tmpAB;});
struct Cyc_Tcenv_Bindings*_tmpA7=({struct Cyc_Tcenv_Bindings*_tmpAA=_region_malloc(r,sizeof(*_tmpAA));_tmpAA->v=_tmp8F;_tmpAA->b=(void*)_tmpA6;_tmpAA->tl=_tmpB1;_tmpAA;});
struct Cyc_Tcenv_Fenv*_tmpA8=({struct Cyc_Tcenv_Fenv*_tmpA9=_region_malloc(r,sizeof(*_tmpA9));_tmpA9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB4;_tmpA9->type_vars=(struct Cyc_List_List*)_tmpB3;_tmpA9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB2;_tmpA9->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpA7);_tmpA9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB0;_tmpA9->capability=(void*)_tmpAF;_tmpA9->curr_rgn=(void*)_tmpAE;_tmpA9->flags=(struct Cyc_Tcenv_FenvFlags)_tmpAD;_tmpA9->fnrgn=(struct _RegionHandle*)r;_tmpA9;});
# 442
return Cyc_Tcenv_put_fenv(r,te,_tmpA8);};};}
# 445
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 447
struct Cyc_Tcenv_Fenv*_tmpB6=te->le;
if(_tmpB6 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpB7=*_tmpB6;struct Cyc_Tcenv_Fenv _tmpB8=_tmpB7;struct Cyc_Tcenv_SharedFenv*_tmpC4;struct Cyc_List_List*_tmpC3;struct Cyc_RgnOrder_RgnPO*_tmpC2;const struct Cyc_Tcenv_Bindings*_tmpC1;struct Cyc_Tcenv_CtrlEnv*_tmpC0;void*_tmpBF;void*_tmpBE;int _tmpBD;int _tmpBC;int _tmpBB;_LL6D: _tmpC4=_tmpB8.shared;_tmpC3=_tmpB8.type_vars;_tmpC2=_tmpB8.region_order;_tmpC1=_tmpB8.locals;_tmpC0=_tmpB8.ctrl_env;_tmpBF=_tmpB8.capability;_tmpBE=_tmpB8.curr_rgn;_tmpBD=(_tmpB8.flags).in_notreadctxt;_tmpBC=(_tmpB8.flags).in_lhs;_tmpBB=(_tmpB8.flags).abstract_ok;_LL6E:;{
# 452
struct Cyc_Tcenv_Fenv*_tmpB9=({struct Cyc_Tcenv_Fenv*_tmpBA=_region_malloc(r,sizeof(*_tmpBA));_tmpBA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC4;_tmpBA->type_vars=(struct Cyc_List_List*)_tmpC3;_tmpBA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC2;_tmpBA->locals=(const struct Cyc_Tcenv_Bindings*)_tmpC1;_tmpBA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpC0;_tmpBA->capability=(void*)_tmpBF;_tmpBA->curr_rgn=(void*)_tmpBE;_tmpBA->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpBA->flags).in_notreadctxt=_tmpBD;(_tmpBA->flags).in_lhs=_tmpBC;(_tmpBA->flags).in_new=(int)status;(_tmpBA->flags).abstract_ok=_tmpBB;_tmpBA->flags;});_tmpBA->fnrgn=(struct _RegionHandle*)r;_tmpBA;});
# 455
return Cyc_Tcenv_put_fenv(r,te,_tmpB9);};};}
# 458
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC5=te->le;
if(_tmpC5 == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmpC6=_tmpC5;struct Cyc_Tcenv_Fenv*_tmpC7=_tmpC6;int _tmpC8;_LL70: _tmpC8=(_tmpC7->flags).in_new;_LL71:;
return(enum Cyc_Tcenv_NewStatus)_tmpC8;};}
# 465
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC9=te->le;
if(_tmpC9 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmpCA=_tmpC9;struct Cyc_Tcenv_Fenv*_tmpCB=_tmpCA;int _tmpCC;_LL73: _tmpCC=(_tmpCB->flags).abstract_ok;_LL74:;
return _tmpCC;};}
# 472
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 474
struct Cyc_Tcenv_Fenv*_tmpCD=te->le;
if(_tmpCD == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpCE=*_tmpCD;struct Cyc_Tcenv_Fenv _tmpCF=_tmpCE;struct Cyc_Tcenv_SharedFenv*_tmpDB;struct Cyc_List_List*_tmpDA;struct Cyc_RgnOrder_RgnPO*_tmpD9;const struct Cyc_Tcenv_Bindings*_tmpD8;struct Cyc_Tcenv_CtrlEnv*_tmpD7;void*_tmpD6;void*_tmpD5;int _tmpD4;int _tmpD3;int _tmpD2;_LL76: _tmpDB=_tmpCF.shared;_tmpDA=_tmpCF.type_vars;_tmpD9=_tmpCF.region_order;_tmpD8=_tmpCF.locals;_tmpD7=_tmpCF.ctrl_env;_tmpD6=_tmpCF.capability;_tmpD5=_tmpCF.curr_rgn;_tmpD4=(_tmpCF.flags).in_notreadctxt;_tmpD3=(_tmpCF.flags).in_lhs;_tmpD2=(_tmpCF.flags).in_new;_LL77:;{
# 479
struct Cyc_Tcenv_Fenv*_tmpD0=({struct Cyc_Tcenv_Fenv*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));_tmpD1->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpDB;_tmpD1->type_vars=(struct Cyc_List_List*)_tmpDA;_tmpD1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpD9;_tmpD1->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD8;_tmpD1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD7;_tmpD1->capability=(void*)_tmpD6;_tmpD1->curr_rgn=(void*)_tmpD5;_tmpD1->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpD1->flags).in_notreadctxt=_tmpD4;(_tmpD1->flags).in_lhs=_tmpD3;(_tmpD1->flags).in_new=_tmpD2;(_tmpD1->flags).abstract_ok=1;_tmpD1->flags;});_tmpD1->fnrgn=(struct _RegionHandle*)r;_tmpD1;});
# 482
return Cyc_Tcenv_put_fenv(r,te,_tmpD0);};};}
# 485
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 487
struct Cyc_Tcenv_Fenv*_tmpDC=te->le;
if(_tmpDC == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpDD=*_tmpDC;struct Cyc_Tcenv_Fenv _tmpDE=_tmpDD;struct Cyc_Tcenv_SharedFenv*_tmpEA;struct Cyc_List_List*_tmpE9;struct Cyc_RgnOrder_RgnPO*_tmpE8;const struct Cyc_Tcenv_Bindings*_tmpE7;struct Cyc_Tcenv_CtrlEnv*_tmpE6;void*_tmpE5;void*_tmpE4;int _tmpE3;int _tmpE2;int _tmpE1;_LL79: _tmpEA=_tmpDE.shared;_tmpE9=_tmpDE.type_vars;_tmpE8=_tmpDE.region_order;_tmpE7=_tmpDE.locals;_tmpE6=_tmpDE.ctrl_env;_tmpE5=_tmpDE.capability;_tmpE4=_tmpDE.curr_rgn;_tmpE3=(_tmpDE.flags).in_notreadctxt;_tmpE2=(_tmpDE.flags).in_lhs;_tmpE1=(_tmpDE.flags).in_new;_LL7A:;{
# 492
struct Cyc_Tcenv_Fenv*_tmpDF=({struct Cyc_Tcenv_Fenv*_tmpE0=_region_malloc(r,sizeof(*_tmpE0));_tmpE0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpEA;_tmpE0->type_vars=(struct Cyc_List_List*)_tmpE9;_tmpE0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE8;_tmpE0->locals=(const struct Cyc_Tcenv_Bindings*)_tmpE7;_tmpE0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE6;_tmpE0->capability=(void*)_tmpE5;_tmpE0->curr_rgn=(void*)_tmpE4;_tmpE0->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpE0->flags).in_notreadctxt=_tmpE3;(_tmpE0->flags).in_lhs=_tmpE2;(_tmpE0->flags).in_new=_tmpE1;(_tmpE0->flags).abstract_ok=0;_tmpE0->flags;});_tmpE0->fnrgn=(struct _RegionHandle*)r;_tmpE0;});
# 495
return Cyc_Tcenv_put_fenv(r,te,_tmpDF);};};}
# 498
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpEB=te->le;
if(_tmpEB == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpEC=*_tmpEB;struct Cyc_Tcenv_Fenv _tmpED=_tmpEC;struct Cyc_Tcenv_SharedFenv*_tmpF9;struct Cyc_List_List*_tmpF8;struct Cyc_RgnOrder_RgnPO*_tmpF7;const struct Cyc_Tcenv_Bindings*_tmpF6;struct Cyc_Tcenv_CtrlEnv*_tmpF5;void*_tmpF4;void*_tmpF3;int _tmpF2;int _tmpF1;int _tmpF0;_LL7C: _tmpF9=_tmpED.shared;_tmpF8=_tmpED.type_vars;_tmpF7=_tmpED.region_order;_tmpF6=_tmpED.locals;_tmpF5=_tmpED.ctrl_env;_tmpF4=_tmpED.capability;_tmpF3=_tmpED.curr_rgn;_tmpF2=(_tmpED.flags).in_lhs;_tmpF1=(_tmpED.flags).in_new;_tmpF0=(_tmpED.flags).abstract_ok;_LL7D:;{
# 504
struct Cyc_Tcenv_Fenv*_tmpEE=({struct Cyc_Tcenv_Fenv*_tmpEF=_region_malloc(r,sizeof(*_tmpEF));_tmpEF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF9;_tmpEF->type_vars=(struct Cyc_List_List*)_tmpF8;_tmpEF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF7;_tmpEF->locals=(const struct Cyc_Tcenv_Bindings*)_tmpF6;_tmpEF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF5;_tmpEF->capability=(void*)_tmpF4;_tmpEF->curr_rgn=(void*)_tmpF3;_tmpEF->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpEF->flags).in_notreadctxt=1;(_tmpEF->flags).in_lhs=_tmpF2;(_tmpEF->flags).in_new=_tmpF1;(_tmpEF->flags).abstract_ok=_tmpF0;_tmpEF->flags;});_tmpEF->fnrgn=(struct _RegionHandle*)r;_tmpEF;});
# 507
return Cyc_Tcenv_put_fenv(r,te,_tmpEE);};};}
# 510
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpFA=te->le;
if(_tmpFA == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmpFB=*_tmpFA;struct Cyc_Tcenv_Fenv _tmpFC=_tmpFB;struct Cyc_Tcenv_SharedFenv*_tmp108;struct Cyc_List_List*_tmp107;struct Cyc_RgnOrder_RgnPO*_tmp106;const struct Cyc_Tcenv_Bindings*_tmp105;struct Cyc_Tcenv_CtrlEnv*_tmp104;void*_tmp103;void*_tmp102;int _tmp101;int _tmp100;int _tmpFF;_LL7F: _tmp108=_tmpFC.shared;_tmp107=_tmpFC.type_vars;_tmp106=_tmpFC.region_order;_tmp105=_tmpFC.locals;_tmp104=_tmpFC.ctrl_env;_tmp103=_tmpFC.capability;_tmp102=_tmpFC.curr_rgn;_tmp101=(_tmpFC.flags).in_lhs;_tmp100=(_tmpFC.flags).in_new;_tmpFF=(_tmpFC.flags).abstract_ok;_LL80:;{
# 516
struct Cyc_Tcenv_Fenv*_tmpFD=({struct Cyc_Tcenv_Fenv*_tmpFE=_region_malloc(r,sizeof(*_tmpFE));_tmpFE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp108;_tmpFE->type_vars=(struct Cyc_List_List*)_tmp107;_tmpFE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp106;_tmpFE->locals=(const struct Cyc_Tcenv_Bindings*)_tmp105;_tmpFE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp104;_tmpFE->capability=(void*)_tmp103;_tmpFE->curr_rgn=(void*)_tmp102;_tmpFE->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmpFE->flags).in_notreadctxt=0;(_tmpFE->flags).in_lhs=_tmp101;(_tmpFE->flags).in_new=_tmp100;(_tmpFE->flags).abstract_ok=_tmpFF;_tmpFE->flags;});_tmpFE->fnrgn=(struct _RegionHandle*)r;_tmpFE;});
# 519
return Cyc_Tcenv_put_fenv(r,te,_tmpFD);};};}
# 522
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp109=te->le;
if(_tmp109 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp10A=_tmp109;struct Cyc_Tcenv_Fenv*_tmp10B=_tmp10A;int _tmp10C;_LL82: _tmp10C=(_tmp10B->flags).in_notreadctxt;_LL83:;
return _tmp10C;};}
# 529
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10D=te->le;
if(_tmp10D == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp10E=*_tmp10D;struct Cyc_Tcenv_Fenv _tmp10F=_tmp10E;struct Cyc_Tcenv_SharedFenv*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_RgnOrder_RgnPO*_tmp119;const struct Cyc_Tcenv_Bindings*_tmp118;struct Cyc_Tcenv_CtrlEnv*_tmp117;void*_tmp116;void*_tmp115;int _tmp114;int _tmp113;int _tmp112;_LL85: _tmp11B=_tmp10F.shared;_tmp11A=_tmp10F.type_vars;_tmp119=_tmp10F.region_order;_tmp118=_tmp10F.locals;_tmp117=_tmp10F.ctrl_env;_tmp116=_tmp10F.capability;_tmp115=_tmp10F.curr_rgn;_tmp114=(_tmp10F.flags).in_notreadctxt;_tmp113=(_tmp10F.flags).in_new;_tmp112=(_tmp10F.flags).abstract_ok;_LL86:;{
# 535
struct Cyc_Tcenv_Fenv*_tmp110=({struct Cyc_Tcenv_Fenv*_tmp111=_region_malloc(r,sizeof(*_tmp111));_tmp111->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp11B;_tmp111->type_vars=(struct Cyc_List_List*)_tmp11A;_tmp111->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp119;_tmp111->locals=(const struct Cyc_Tcenv_Bindings*)_tmp118;_tmp111->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp117;_tmp111->capability=(void*)_tmp116;_tmp111->curr_rgn=(void*)_tmp115;_tmp111->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp111->flags).in_notreadctxt=_tmp114;(_tmp111->flags).in_lhs=1;(_tmp111->flags).in_new=_tmp113;(_tmp111->flags).abstract_ok=_tmp112;_tmp111->flags;});_tmp111->fnrgn=(struct _RegionHandle*)r;_tmp111;});
# 538
return Cyc_Tcenv_put_fenv(r,te,_tmp110);};};}
# 541
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp11C=te->le;
if(_tmp11C == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp11D=*_tmp11C;struct Cyc_Tcenv_Fenv _tmp11E=_tmp11D;struct Cyc_Tcenv_SharedFenv*_tmp12A;struct Cyc_List_List*_tmp129;struct Cyc_RgnOrder_RgnPO*_tmp128;const struct Cyc_Tcenv_Bindings*_tmp127;struct Cyc_Tcenv_CtrlEnv*_tmp126;void*_tmp125;void*_tmp124;int _tmp123;int _tmp122;int _tmp121;_LL88: _tmp12A=_tmp11E.shared;_tmp129=_tmp11E.type_vars;_tmp128=_tmp11E.region_order;_tmp127=_tmp11E.locals;_tmp126=_tmp11E.ctrl_env;_tmp125=_tmp11E.capability;_tmp124=_tmp11E.curr_rgn;_tmp123=(_tmp11E.flags).in_notreadctxt;_tmp122=(_tmp11E.flags).in_new;_tmp121=(_tmp11E.flags).abstract_ok;_LL89:;{
# 547
struct Cyc_Tcenv_Fenv*_tmp11F=({struct Cyc_Tcenv_Fenv*_tmp120=_region_malloc(r,sizeof(*_tmp120));_tmp120->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12A;_tmp120->type_vars=(struct Cyc_List_List*)_tmp129;_tmp120->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp128;_tmp120->locals=(const struct Cyc_Tcenv_Bindings*)_tmp127;_tmp120->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp126;_tmp120->capability=(void*)_tmp125;_tmp120->curr_rgn=(void*)_tmp124;_tmp120->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp120->flags).in_notreadctxt=_tmp123;(_tmp120->flags).in_lhs=0;(_tmp120->flags).in_new=_tmp122;(_tmp120->flags).abstract_ok=_tmp121;_tmp120->flags;});_tmp120->fnrgn=(struct _RegionHandle*)r;_tmp120;});
# 550
return Cyc_Tcenv_put_fenv(r,te,_tmp11F);};};}
# 553
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp12B=te->le;
if(_tmp12B == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp12C=_tmp12B;struct Cyc_Tcenv_Fenv*_tmp12D=_tmp12C;int _tmp12E;_LL8B: _tmp12E=(_tmp12D->flags).in_lhs;_LL8C:;
return _tmp12E;};}
# 561
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 563
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp12F;_push_handler(& _tmp12F);{int _tmp131=0;if(setjmp(_tmp12F.handler))_tmp131=1;if(!_tmp131){
{void*_tmp132=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp133=_tmp132;if(((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp133)->tag == 0)switch(*((int*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp133)->f1)){case 4: _LL8E: _LL8F:
# 567
({void*_tmp134=0;Cyc_Tcutil_warn(loc,({const char*_tmp135="pattern variable hides local";_tag_dyneither(_tmp135,sizeof(char),29);}),_tag_dyneither(_tmp134,sizeof(void*),0));});goto _LL8D;case 1: _LL90: _LL91:
# 569
({void*_tmp136=0;Cyc_Tcutil_warn(loc,({const char*_tmp137="pattern variable hides global";_tag_dyneither(_tmp137,sizeof(char),30);}),_tag_dyneither(_tmp136,sizeof(void*),0));});goto _LL8D;case 5: _LL92: _LL93:
# 571
({void*_tmp138=0;Cyc_Tcutil_warn(loc,({const char*_tmp139="pattern variable hides pattern variable";_tag_dyneither(_tmp139,sizeof(char),40);}),_tag_dyneither(_tmp138,sizeof(void*),0));});goto _LL8D;case 3: _LL94: _LL95:
# 573
({void*_tmp13A=0;Cyc_Tcutil_warn(loc,({const char*_tmp13B="pattern variable hides parameter";_tag_dyneither(_tmp13B,sizeof(char),33);}),_tag_dyneither(_tmp13A,sizeof(void*),0));});goto _LL8D;default: goto _LL96;}else{_LL96: _LL97:
 goto _LL8D;}_LL8D:;}
# 565
;_pop_handler();}else{void*_tmp130=(void*)_exn_thrown;void*_tmp13C=_tmp130;void*_tmp13D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp13C)->tag == Cyc_Dict_Absent){_LL99: _LL9A:
# 576
 goto _LL98;}else{_LL9B: _tmp13D=_tmp13C;_LL9C:(int)_rethrow(_tmp13D);}_LL98:;}};}{
struct _dyneither_ptr*_tmp13E=(*vd->name).f2;
struct Cyc_Tcenv_Fenv*_tmp13F=
# 580
Cyc_Tcenv_get_fenv(te,({const char*_tmp150="add_pat_var";_tag_dyneither(_tmp150,sizeof(char),12);}));
# 578
struct Cyc_Tcenv_Fenv*_tmp140=_tmp13F;struct Cyc_Tcenv_SharedFenv*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_RgnOrder_RgnPO*_tmp14D;const struct Cyc_Tcenv_Bindings*_tmp14C;struct Cyc_Tcenv_CtrlEnv*_tmp14B;void*_tmp14A;void*_tmp149;struct Cyc_Tcenv_FenvFlags _tmp148;_LL9E: _tmp14F=_tmp140->shared;_tmp14E=_tmp140->type_vars;_tmp14D=_tmp140->region_order;_tmp14C=_tmp140->locals;_tmp14B=_tmp140->ctrl_env;_tmp14A=_tmp140->capability;_tmp149=_tmp140->curr_rgn;_tmp148=_tmp140->flags;_LL9F:;{
# 581
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp141=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp147;_tmp147.tag=5;_tmp147.f1=vd;_tmp147;});_tmp146;});
struct Cyc_Tcenv_Bindings*_tmp142=({struct Cyc_Tcenv_Bindings*_tmp145=_region_malloc(r,sizeof(*_tmp145));_tmp145->v=_tmp13E;_tmp145->b=(void*)_tmp141;_tmp145->tl=_tmp14C;_tmp145;});
struct Cyc_Tcenv_Fenv*_tmp143=({struct Cyc_Tcenv_Fenv*_tmp144=_region_malloc(r,sizeof(*_tmp144));_tmp144->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp14F;_tmp144->type_vars=(struct Cyc_List_List*)_tmp14E;_tmp144->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14D;_tmp144->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp142);_tmp144->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14B;_tmp144->capability=(void*)_tmp14A;_tmp144->curr_rgn=(void*)_tmp149;_tmp144->flags=(struct Cyc_Tcenv_FenvFlags)_tmp148;_tmp144->fnrgn=(struct _RegionHandle*)r;_tmp144;});
# 585
return Cyc_Tcenv_put_fenv(r,te,_tmp143);};};}
# 588
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp151=te->le;
struct _tuple0*_tmp152=q;union Cyc_Absyn_Nmspace _tmp16C;struct _dyneither_ptr*_tmp16B;_LLA1: _tmp16C=_tmp152->f1;_tmp16B=_tmp152->f2;_LLA2:;{
union Cyc_Absyn_Nmspace _tmp153=_tmp16C;if((_tmp153.Loc_n).tag == 4){_LLA4: _LLA5:
# 593
 if(_tmp151 == 0)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp154=_cycalloc_atomic(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Dict_Absent_exn_struct _tmp155;_tmp155.tag=Cyc_Dict_Absent;_tmp155;});_tmp154;}));{
struct Cyc_Tcenv_Fenv*_tmp156=_tmp151;struct Cyc_Tcenv_Fenv*_tmp157=_tmp156;const struct Cyc_Tcenv_Bindings*_tmp160;_LLA9: _tmp160=_tmp157->locals;_LLAA:;{
# 596
struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!_tmp15A){{void*_tmp15D=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp15B=_region_malloc(r,sizeof(*_tmp15B));_tmp15B[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp15C;_tmp15C.tag=0;_tmp15C.f1=Cyc_Tcenv_lookup_binding(_tmp160,_tmp16B);_tmp15C;});_tmp15B;});_npop_handler(0);return _tmp15D;};_pop_handler();}else{void*_tmp159=(void*)_exn_thrown;void*_tmp15E=_tmp159;void*_tmp15F;if(((struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp15E)->tag == Cyc_Tcenv_NoBinding){_LLAC: _LLAD:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);}else{_LLAE: _tmp15F=_tmp15E;_LLAF:(int)_rethrow(_tmp15F);}_LLAB:;}};};};}else{_LLA6: _LLA7: {
# 599
struct _handler_cons _tmp161;_push_handler(& _tmp161);{int _tmp163=0;if(setjmp(_tmp161.handler))_tmp163=1;if(!_tmp163){{void*_tmp164=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp164;};_pop_handler();}else{void*_tmp162=(void*)_exn_thrown;void*_tmp165=_tmp162;void*_tmp16A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp165)->tag == Cyc_Dict_Absent){_LLB1: _LLB2:
# 601
 return(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp166=_region_malloc(r,sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct _tmp169;_tmp169.tag=0;_tmp169.f1=q;_tmp169;});_tmp168;});_tmp167;});_tmp166;});}else{_LLB3: _tmp16A=_tmp165;_LLB4:(int)_rethrow(_tmp16A);}_LLB0:;}};}}_LLA3:;};}
# 607
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 609
struct Cyc_Tcenv_Fenv*_tmp16D=Cyc_Tcenv_get_fenv(te,({const char*_tmp172="process_fallthru";_tag_dyneither(_tmp172,sizeof(char),17);}));struct Cyc_Tcenv_Fenv*_tmp16E=_tmp16D;struct Cyc_Tcenv_CtrlEnv*_tmp171;_LLB6: _tmp171=_tmp16E->ctrl_env;_LLB7:;{
const struct _tuple10*_tmp16F=_tmp171->fallthru_clause;
if(_tmp16F != (const struct _tuple10*)0)
*clauseopt=({struct Cyc_Absyn_Switch_clause**_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=(*_tmp16F).f1;_tmp170;});
# 614
return _tmp16F;};}
# 617
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 621
struct Cyc_Tcenv_Fenv*_tmp173=
# 623
Cyc_Tcenv_get_fenv(te,({const char*_tmp186="set_fallthru";_tag_dyneither(_tmp186,sizeof(char),13);}));
# 621
struct Cyc_Tcenv_Fenv*_tmp174=_tmp173;struct Cyc_Tcenv_SharedFenv*_tmp185;struct Cyc_List_List*_tmp184;struct Cyc_RgnOrder_RgnPO*_tmp183;const struct Cyc_Tcenv_Bindings*_tmp182;struct Cyc_Tcenv_CtrlEnv*_tmp181;void*_tmp180;void*_tmp17F;struct Cyc_Tcenv_FenvFlags _tmp17E;_LLB9: _tmp185=_tmp174->shared;_tmp184=_tmp174->type_vars;_tmp183=_tmp174->region_order;_tmp182=_tmp174->locals;_tmp181=_tmp174->ctrl_env;_tmp180=_tmp174->capability;_tmp17F=_tmp174->curr_rgn;_tmp17E=_tmp174->flags;_LLBA:;{
# 624
struct Cyc_Tcenv_CtrlEnv*_tmp175=_tmp181;const struct _tuple10*_tmp17D;_LLBC: _tmp17D=_tmp175->fallthru_clause;_LLBD:;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp176->tl=ft_typ;_tmp176;});}{
struct Cyc_List_List*_tmp177=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct Cyc_Tcenv_CtrlEnv*_tmp178=({struct Cyc_Tcenv_CtrlEnv*_tmp17B=_region_malloc(r,sizeof(*_tmp17B));_tmp17B->fallthru_clause=(const struct _tuple10*)({struct _tuple10*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=clause;_tmp17C->f2=new_tvs;_tmp17C->f3=(const struct Cyc_Tcenv_CList*)_tmp177;_tmp17C;});_tmp17B;});
# 631
struct Cyc_Tcenv_Fenv*_tmp179=({struct Cyc_Tcenv_Fenv*_tmp17A=_region_malloc(r,sizeof(*_tmp17A));_tmp17A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp185;_tmp17A->type_vars=(struct Cyc_List_List*)_tmp184;_tmp17A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp183;_tmp17A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp182;_tmp17A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp178;_tmp17A->capability=(void*)_tmp180;_tmp17A->curr_rgn=(void*)_tmp17F;_tmp17A->flags=(struct Cyc_Tcenv_FenvFlags)_tmp17E;_tmp17A->fnrgn=(struct _RegionHandle*)r;_tmp17A;});
# 635
return Cyc_Tcenv_put_fenv(r,te,_tmp179);};};};}
# 638
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp189="clear_fallthru";_tag_dyneither(_tmp189,sizeof(char),15);})));
struct Cyc_Tcenv_Fenv*_tmp187=fe;struct Cyc_Tcenv_CtrlEnv*_tmp188;_LLBF: _tmp188=_tmp187->ctrl_env;_LLC0:;
_tmp188->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 645
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp18A=_region_malloc(r2,sizeof(*_tmp18A));_tmp18A->ns=te->ns;_tmp18A->ae=te->ae;_tmp18A->le=0;_tmp18A->allow_valueof=1;_tmp18A->in_extern_c_include=te->in_extern_c_include;_tmp18A;});{
struct Cyc_Tcenv_Fenv _tmp18B=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp18C=_tmp18B;struct Cyc_Tcenv_SharedFenv*_tmp197;struct Cyc_List_List*_tmp196;struct Cyc_RgnOrder_RgnPO*_tmp195;const struct Cyc_Tcenv_Bindings*_tmp194;struct Cyc_Tcenv_CtrlEnv*_tmp193;void*_tmp192;void*_tmp191;struct Cyc_Tcenv_FenvFlags _tmp190;_LLC2: _tmp197=_tmp18C.shared;_tmp196=_tmp18C.type_vars;_tmp195=_tmp18C.region_order;_tmp194=_tmp18C.locals;_tmp193=_tmp18C.ctrl_env;_tmp192=_tmp18C.capability;_tmp191=_tmp18C.curr_rgn;_tmp190=_tmp18C.flags;_LLC3:;{
struct Cyc_Tcenv_Fenv*_tmp18D=({struct Cyc_Tcenv_Fenv*_tmp18F=_region_malloc(r2,sizeof(*_tmp18F));_tmp18F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp197;_tmp18F->type_vars=(struct Cyc_List_List*)_tmp196;_tmp18F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp195;_tmp18F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp194;_tmp18F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp193;_tmp18F->capability=(void*)_tmp192;_tmp18F->curr_rgn=(void*)_tmp191;_tmp18F->flags=(struct Cyc_Tcenv_FenvFlags)_tmp190;_tmp18F->fnrgn=(struct _RegionHandle*)r2;_tmp18F;});
return({struct Cyc_Tcenv_Tenv*_tmp18E=_region_malloc(r2,sizeof(*_tmp18E));_tmp18E->ns=te->ns;_tmp18E->ae=te->ae;_tmp18E->le=_tmp18D;_tmp18E->allow_valueof=1;_tmp18E->in_extern_c_include=te->in_extern_c_include;_tmp18E;});};};}
# 653
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)
return({struct Cyc_Tcenv_Tenv*_tmp198=_region_malloc(r2,sizeof(*_tmp198));_tmp198->ns=te->ns;_tmp198->ae=te->ae;_tmp198->le=0;_tmp198->allow_valueof=te->allow_valueof;_tmp198->in_extern_c_include=1;_tmp198;});{
struct Cyc_Tcenv_Fenv _tmp199=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_Fenv _tmp19A=_tmp199;struct Cyc_Tcenv_SharedFenv*_tmp1A5;struct Cyc_List_List*_tmp1A4;struct Cyc_RgnOrder_RgnPO*_tmp1A3;const struct Cyc_Tcenv_Bindings*_tmp1A2;struct Cyc_Tcenv_CtrlEnv*_tmp1A1;void*_tmp1A0;void*_tmp19F;struct Cyc_Tcenv_FenvFlags _tmp19E;_LLC5: _tmp1A5=_tmp19A.shared;_tmp1A4=_tmp19A.type_vars;_tmp1A3=_tmp19A.region_order;_tmp1A2=_tmp19A.locals;_tmp1A1=_tmp19A.ctrl_env;_tmp1A0=_tmp19A.capability;_tmp19F=_tmp19A.curr_rgn;_tmp19E=_tmp19A.flags;_LLC6:;{
struct Cyc_Tcenv_Fenv*_tmp19B=({struct Cyc_Tcenv_Fenv*_tmp19D=_region_malloc(r2,sizeof(*_tmp19D));_tmp19D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1A5;_tmp19D->type_vars=(struct Cyc_List_List*)_tmp1A4;_tmp19D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1A3;_tmp19D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1A2;_tmp19D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1A1;_tmp19D->capability=(void*)_tmp1A0;_tmp19D->curr_rgn=(void*)_tmp19F;_tmp19D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp19E;_tmp19D->fnrgn=(struct _RegionHandle*)r2;_tmp19D;});
return({struct Cyc_Tcenv_Tenv*_tmp19C=_region_malloc(r2,sizeof(*_tmp19C));_tmp19C->ns=te->ns;_tmp19C->ae=te->ae;_tmp19C->le=_tmp19B;_tmp19C->allow_valueof=te->allow_valueof;_tmp19C->in_extern_c_include=1;_tmp19C;});};};}
# 661
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 664
struct Cyc_Tcenv_Fenv _tmp1A6=*
# 666
Cyc_Tcenv_get_fenv(te,({const char*_tmp1BB="add_region";_tag_dyneither(_tmp1BB,sizeof(char),11);}));
# 664
struct Cyc_Tcenv_Fenv _tmp1A7=_tmp1A6;struct Cyc_Tcenv_SharedFenv*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_RgnOrder_RgnPO*_tmp1B8;const struct Cyc_Tcenv_Bindings*_tmp1B7;struct Cyc_Tcenv_CtrlEnv*_tmp1B6;void*_tmp1B5;void*_tmp1B4;struct Cyc_Tcenv_FenvFlags _tmp1B3;_LLC8: _tmp1BA=_tmp1A7.shared;_tmp1B9=_tmp1A7.type_vars;_tmp1B8=_tmp1A7.region_order;_tmp1B7=_tmp1A7.locals;_tmp1B6=_tmp1A7.ctrl_env;_tmp1B5=_tmp1A7.capability;_tmp1B4=_tmp1A7.curr_rgn;_tmp1B3=_tmp1A7.flags;_LLC9:;
# 667
{void*_tmp1A8=Cyc_Tcutil_compress(rgn);void*_tmp1A9=_tmp1A8;struct Cyc_Absyn_Tvar*_tmp1AA;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A9)->tag == 2){_LLCB: _tmp1AA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A9)->f1;_LLCC:
# 669
 _tmp1B8=Cyc_RgnOrder_add_youngest(_tmp1BA->frgn,_tmp1B8,_tmp1AA,resetable,opened);
# 671
goto _LLCA;}else{_LLCD: _LLCE:
 goto _LLCA;}_LLCA:;}
# 674
_tmp1B5=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1AC;_tmp1AC.tag=24;_tmp1AC.f1=({struct Cyc_List_List*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1B0;_tmp1B0.tag=23;_tmp1B0.f1=rgn;_tmp1B0;});_tmp1AF;});_tmp1AD->tl=({struct Cyc_List_List*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->hd=_tmp1B5;_tmp1AE->tl=0;_tmp1AE;});_tmp1AD;});_tmp1AC;});_tmp1AB;});{
# 676
struct Cyc_Tcenv_Fenv*_tmp1B1=({struct Cyc_Tcenv_Fenv*_tmp1B2=_region_malloc(r,sizeof(*_tmp1B2));_tmp1B2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1BA;_tmp1B2->type_vars=(struct Cyc_List_List*)_tmp1B9;_tmp1B2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1B8;_tmp1B2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1B7;_tmp1B2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1B6;_tmp1B2->capability=(void*)_tmp1B5;_tmp1B2->curr_rgn=(void*)_tmp1B4;_tmp1B2->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1B3;_tmp1B2->fnrgn=(struct _RegionHandle*)r;_tmp1B2;});
# 679
return Cyc_Tcenv_put_fenv(r,te,_tmp1B1);};}
# 682
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 684
struct Cyc_Tcenv_Fenv _tmp1BC=*
# 686
Cyc_Tcenv_get_fenv(te,({const char*_tmp1D2="new_named_block";_tag_dyneither(_tmp1D2,sizeof(char),16);}));
# 684
struct Cyc_Tcenv_Fenv _tmp1BD=_tmp1BC;struct Cyc_Tcenv_SharedFenv*_tmp1D1;struct Cyc_List_List*_tmp1D0;struct Cyc_RgnOrder_RgnPO*_tmp1CF;const struct Cyc_Tcenv_Bindings*_tmp1CE;struct Cyc_Tcenv_CtrlEnv*_tmp1CD;void*_tmp1CC;void*_tmp1CB;struct Cyc_Tcenv_FenvFlags _tmp1CA;_LLD0: _tmp1D1=_tmp1BD.shared;_tmp1D0=_tmp1BD.type_vars;_tmp1CF=_tmp1BD.region_order;_tmp1CE=_tmp1BD.locals;_tmp1CD=_tmp1BD.ctrl_env;_tmp1CC=_tmp1BD.capability;_tmp1CB=_tmp1BD.curr_rgn;_tmp1CA=_tmp1BD.flags;_LLD1:;{
# 688
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp1C9="new_block";_tag_dyneither(_tmp1C9,sizeof(char),10);})));
void*block_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1C8;_tmp1C8.tag=2;_tmp1C8.f1=block_rgn;_tmp1C8;});_tmp1C7;});
_tmp1D0=({struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=block_rgn;_tmp1BE->tl=_tmp1D0;_tmp1BE;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp1D0);
_tmp1CF=Cyc_RgnOrder_add_youngest(_tmp1D1->frgn,_tmp1CF,block_rgn,0,0);
_tmp1CC=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1C0;_tmp1C0.tag=24;_tmp1C0.f1=({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1C4;_tmp1C4.tag=23;_tmp1C4.f1=block_typ;_tmp1C4;});_tmp1C3;});_tmp1C1->tl=({struct Cyc_List_List*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->hd=_tmp1CC;_tmp1C2->tl=0;_tmp1C2;});_tmp1C1;});_tmp1C0;});_tmp1BF;});
_tmp1CB=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp1C5=({struct Cyc_Tcenv_Fenv*_tmp1C6=_region_malloc(r,sizeof(*_tmp1C6));_tmp1C6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1D1;_tmp1C6->type_vars=(struct Cyc_List_List*)_tmp1D0;_tmp1C6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1CF;_tmp1C6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1CE;_tmp1C6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1CD;_tmp1C6->capability=(void*)_tmp1CC;_tmp1C6->curr_rgn=(void*)_tmp1CB;_tmp1C6->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1CA;_tmp1C6->fnrgn=(struct _RegionHandle*)r;_tmp1C6;});
# 698
return Cyc_Tcenv_put_fenv(r,te,_tmp1C5);};};}
# 701
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 703
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 705
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 711
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 715
struct Cyc_Tcenv_Fenv _tmp1D4=*
# 717
Cyc_Tcenv_get_fenv(te,({const char*_tmp1E0="new_outlives_constraints";_tag_dyneither(_tmp1E0,sizeof(char),25);}));
# 715
struct Cyc_Tcenv_Fenv _tmp1D5=_tmp1D4;struct Cyc_Tcenv_SharedFenv*_tmp1DF;struct Cyc_List_List*_tmp1DE;struct Cyc_RgnOrder_RgnPO*_tmp1DD;const struct Cyc_Tcenv_Bindings*_tmp1DC;struct Cyc_Tcenv_CtrlEnv*_tmp1DB;void*_tmp1DA;void*_tmp1D9;struct Cyc_Tcenv_FenvFlags _tmp1D8;_LLD3: _tmp1DF=_tmp1D5.shared;_tmp1DE=_tmp1D5.type_vars;_tmp1DD=_tmp1D5.region_order;_tmp1DC=_tmp1D5.locals;_tmp1DB=_tmp1D5.ctrl_env;_tmp1DA=_tmp1D5.capability;_tmp1D9=_tmp1D5.curr_rgn;_tmp1D8=_tmp1D5.flags;_LLD4:;
# 718
for(0;cs != 0;cs=cs->tl){
_tmp1DD=
Cyc_RgnOrder_add_outlives_constraint(_tmp1DF->frgn,_tmp1DD,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 723
struct Cyc_Tcenv_Fenv*_tmp1D6=({struct Cyc_Tcenv_Fenv*_tmp1D7=_region_malloc(r,sizeof(*_tmp1D7));_tmp1D7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1DF;_tmp1D7->type_vars=(struct Cyc_List_List*)_tmp1DE;_tmp1D7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1DD;_tmp1D7->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1DC;_tmp1D7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1DB;_tmp1D7->capability=(void*)_tmp1DA;_tmp1D7->curr_rgn=(void*)_tmp1D9;_tmp1D7->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1D8;_tmp1D7->fnrgn=(struct _RegionHandle*)r;_tmp1D7;});
# 726
return Cyc_Tcenv_put_fenv(r,te,_tmp1D6);};}
# 729
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 737
void*_tmp1E1=Cyc_Tcutil_compress(r1);
void*_tmp1E2=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp1E3=Cyc_Tcutil_typ_kind(_tmp1E1);
struct Cyc_Absyn_Kind*_tmp1E4=Cyc_Tcutil_typ_kind(_tmp1E2);
# 743
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp1E3,_tmp1E4);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp1E4,_tmp1E3);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp1E4));({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0;_tmp1E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp1E3));({struct Cyc_String_pa_PrintArg_struct _tmp1E8;_tmp1E8.tag=0;_tmp1E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1E2));({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0;_tmp1E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1E1));({void*_tmp1E5[4]={& _tmp1E7,& _tmp1E8,& _tmp1E9,& _tmp1EA};Cyc_Tcutil_terr(loc,({const char*_tmp1E6="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp1E6,sizeof(char),82);}),_tag_dyneither(_tmp1E5,sizeof(void*),4));});});});});});
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 750
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp1EB=Cyc_Tcutil_swap_kind(_tmp1E2,Cyc_Tcutil_kind_to_bound(_tmp1E3));struct _tuple11 _tmp1EC=_tmp1EB;struct Cyc_Absyn_Tvar*_tmp1EF;void*_tmp1EE;_LLD6: _tmp1EF=_tmp1EC.f1;_tmp1EE=_tmp1EC.f2;_LLD7:;
*oldtv=({struct _tuple11*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->f1=_tmp1EF;_tmp1ED->f2=_tmp1EE;_tmp1ED;});}else{
# 754
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp1F0=Cyc_Tcutil_swap_kind(_tmp1E1,Cyc_Tcutil_kind_to_bound(_tmp1E4));struct _tuple11 _tmp1F1=_tmp1F0;struct Cyc_Absyn_Tvar*_tmp1F4;void*_tmp1F3;_LLD9: _tmp1F4=_tmp1F1.f1;_tmp1F3=_tmp1F1.f2;_LLDA:;
*oldtv=({struct _tuple11*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->f1=_tmp1F4;_tmp1F2->f2=_tmp1F3;_tmp1F2;});}}}{
# 760
struct _RegionHandle*_tmp1F5=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp1F6=0;
if((_tmp1E1 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp1E1 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp1E1 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1FA;_tmp1FA.tag=23;_tmp1FA.f1=_tmp1E1;_tmp1FA;});_tmp1F9;});
_tmp1F6=({struct Cyc_List_List*_tmp1F7=_region_malloc(_tmp1F5,sizeof(*_tmp1F7));_tmp1F7->hd=({struct _tuple13*_tmp1F8=_region_malloc(_tmp1F5,sizeof(*_tmp1F8));_tmp1F8->f1=eff1;_tmp1F8->f2=_tmp1E2;_tmp1F8;});_tmp1F7->tl=_tmp1F6;_tmp1F7;});}
# 766
if((_tmp1E2 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp1E2 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp1E2 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1FE;_tmp1FE.tag=23;_tmp1FE.f1=_tmp1E2;_tmp1FE;});_tmp1FD;});
_tmp1F6=({struct Cyc_List_List*_tmp1FB=_region_malloc(_tmp1F5,sizeof(*_tmp1FB));_tmp1FB->hd=({struct _tuple13*_tmp1FC=_region_malloc(_tmp1F5,sizeof(*_tmp1FC));_tmp1FC->f1=eff2;_tmp1FC->f2=_tmp1E1;_tmp1FC;});_tmp1FB->tl=_tmp1F6;_tmp1FB;});}
# 771
return Cyc_Tcenv_new_outlives_constraints(_tmp1F5,te,_tmp1F6,loc);};}
# 774
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1FF=te->le;
if(_tmp1FF == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp1FF;
struct Cyc_Tcenv_Fenv*_tmp200=fe;void*_tmp201;_LLDC: _tmp201=_tmp200->curr_rgn;_LLDD:;
return _tmp201;};}
# 785
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp20A="check_rgn_accessible";_tag_dyneither(_tmp20A,sizeof(char),21);}));
struct Cyc_Tcenv_Fenv*_tmp202=fe;void*_tmp209;struct Cyc_RgnOrder_RgnPO*_tmp208;_LLDF: _tmp209=_tmp202->capability;_tmp208=_tmp202->region_order;_LLE0:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp209) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp209))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp208,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp204;_tmp204.tag=23;_tmp204.f1=rgn;_tmp204;});_tmp203;}),_tmp209))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0;_tmp207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp205[1]={& _tmp207};Cyc_Tcutil_terr(loc,({const char*_tmp206="Expression accesses unavailable region %s";_tag_dyneither(_tmp206,sizeof(char),42);}),_tag_dyneither(_tmp205,sizeof(void*),1));});});}
# 797
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv*_tmp20B=
Cyc_Tcenv_get_fenv(te,({const char*_tmp216="check_rgn_resetable";_tag_dyneither(_tmp216,sizeof(char),20);}));
# 799
struct Cyc_Tcenv_Fenv*_tmp20C=_tmp20B;struct Cyc_RgnOrder_RgnPO*_tmp215;_LLE2: _tmp215=_tmp20C->region_order;_LLE3:;{
# 801
void*_tmp20D=Cyc_Tcutil_compress(rgn);void*_tmp20E=_tmp20D;struct Cyc_Absyn_Tvar*_tmp214;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp20E)->tag == 2){_LLE5: _tmp214=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp20E)->f1;_LLE6:
# 803
 if(!Cyc_RgnOrder_is_region_resetable(_tmp215,_tmp214))
({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0;_tmp211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));({void*_tmp20F[1]={& _tmp211};Cyc_Tcutil_terr(loc,({const char*_tmp210="Region %s is not resetable";_tag_dyneither(_tmp210,sizeof(char),27);}),_tag_dyneither(_tmp20F,sizeof(void*),1));});});
return;}else{_LLE7: _LLE8:
({void*_tmp212=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp213="check_rgn_resetable";_tag_dyneither(_tmp213,sizeof(char),20);}),_tag_dyneither(_tmp212,sizeof(void*),0));});}_LLE4:;};};}
# 813
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp217=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 818
if(_tmp217 == 0){
void*_tmp218=rt_a;switch(*((int*)_tmp218)){case 22: _LLEA: _LLEB:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21: _LLEC: _LLED:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20: _LLEE: _LLEF:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LLF0: _LLF1:
 return 0;}_LLE9:;}{
# 826
struct Cyc_Tcenv_Fenv*fe=_tmp217;
struct Cyc_Tcenv_Fenv*_tmp219=fe;struct Cyc_RgnOrder_RgnPO*_tmp21D;_LLF3: _tmp21D=_tmp219->region_order;_LLF4:;{
int _tmp21A=Cyc_RgnOrder_effect_outlives(_tmp21D,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp21C;_tmp21C.tag=23;_tmp21C.f1=rt_a;_tmp21C;});_tmp21B;}),rt_b);
# 832
return _tmp21A;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 837
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp21E=
Cyc_Tcenv_get_fenv(te,({const char*_tmp225="check_effect_accessible";_tag_dyneither(_tmp225,sizeof(char),24);}));
# 838
struct Cyc_Tcenv_Fenv*_tmp21F=_tmp21E;void*_tmp224;struct Cyc_RgnOrder_RgnPO*_tmp223;struct Cyc_Tcenv_SharedFenv*_tmp222;_LLF6: _tmp224=_tmp21F->capability;_tmp223=_tmp21F->region_order;_tmp222=_tmp21F->shared;_LLF7:;
# 840
if(Cyc_Tcutil_subset_effect(0,eff,_tmp224))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp223,eff,_tmp224))
return;{
struct _RegionHandle*frgn=_tmp222->frgn;
_tmp222->delayed_effect_checks=({struct Cyc_List_List*_tmp220=_region_malloc(frgn,sizeof(*_tmp220));_tmp220->hd=({struct _tuple14*_tmp221=_region_malloc(frgn,sizeof(*_tmp221));_tmp221->f1=_tmp224;_tmp221->f2=eff;_tmp221->f3=_tmp223;_tmp221->f4=loc;_tmp221;});_tmp220->tl=_tmp222->delayed_effect_checks;_tmp220;});};}
# 849
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp226=
Cyc_Tcenv_get_fenv(te,({const char*_tmp234="check_delayed_effects";_tag_dyneither(_tmp234,sizeof(char),22);}));
# 850
struct Cyc_Tcenv_Fenv*_tmp227=_tmp226;struct Cyc_Tcenv_SharedFenv*_tmp233;_LLF9: _tmp233=_tmp227->shared;_LLFA:;{
# 852
struct Cyc_List_List*_tmp228=_tmp233->delayed_effect_checks;
for(0;_tmp228 != 0;_tmp228=_tmp228->tl){
struct _tuple14*_tmp229=(struct _tuple14*)_tmp228->hd;struct _tuple14*_tmp22A=_tmp229;void*_tmp232;void*_tmp231;struct Cyc_RgnOrder_RgnPO*_tmp230;unsigned int _tmp22F;_LLFC: _tmp232=_tmp22A->f1;_tmp231=_tmp22A->f2;_tmp230=_tmp22A->f3;_tmp22F=_tmp22A->f4;_LLFD:;
if(Cyc_Tcutil_subset_effect(1,_tmp231,_tmp232))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp230,_tmp231,_tmp232))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp22E;_tmp22E.tag=0;_tmp22E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp231));({struct Cyc_String_pa_PrintArg_struct _tmp22D;_tmp22D.tag=0;_tmp22D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp232));({void*_tmp22B[2]={& _tmp22D,& _tmp22E};Cyc_Tcutil_terr(_tmp22F,({const char*_tmp22C="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp22C,sizeof(char),51);}),_tag_dyneither(_tmp22B,sizeof(void*),2));});});});}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 868
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 870
struct Cyc_Tcenv_Fenv*_tmp235=te->le;
if(_tmp235 == 0){
# 873
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp237;_tmp237.tag=23;_tmp237.f1=(*((struct _tuple13*)po->hd)).f2;_tmp237;});_tmp236;}),Cyc_Absyn_empty_effect))
({void*_tmp238=0;Cyc_Tcutil_terr(loc,({const char*_tmp239="the required region ordering is not satisfied here";_tag_dyneither(_tmp239,sizeof(char),51);}),_tag_dyneither(_tmp238,sizeof(void*),0));});}
return;}{
# 879
struct Cyc_Tcenv_Fenv*_tmp23A=_tmp235;struct Cyc_Tcenv_Fenv*_tmp23B=_tmp23A;struct Cyc_RgnOrder_RgnPO*_tmp23F;struct Cyc_Tcenv_SharedFenv*_tmp23E;_LLFF: _tmp23F=_tmp23B->region_order;_tmp23E=_tmp23B->shared;_LL100:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp23F,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
_tmp23E->delayed_constraint_checks=({struct Cyc_List_List*_tmp23C=_region_malloc(_tmp23E->frgn,sizeof(*_tmp23C));_tmp23C->hd=({struct _tuple15*_tmp23D=_region_malloc(_tmp23E->frgn,sizeof(*_tmp23D));_tmp23D->f1=_tmp23F;_tmp23D->f2=po;_tmp23D->f3=loc;_tmp23D;});_tmp23C->tl=_tmp23E->delayed_constraint_checks;_tmp23C;});};}
# 886
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp240=
Cyc_Tcenv_get_fenv(te,({const char*_tmp24B="check_delayed_constraints";_tag_dyneither(_tmp24B,sizeof(char),26);}));
# 887
struct Cyc_Tcenv_Fenv*_tmp241=_tmp240;struct Cyc_Tcenv_SharedFenv*_tmp24A;_LL102: _tmp24A=_tmp241->shared;_LL103:;{
# 889
struct Cyc_List_List*_tmp242=_tmp24A->delayed_constraint_checks;
for(0;_tmp242 != 0;_tmp242=_tmp242->tl){
struct _tuple15*_tmp243=(struct _tuple15*)_tmp242->hd;struct _tuple15*_tmp244=_tmp243;struct Cyc_RgnOrder_RgnPO*_tmp249;struct Cyc_List_List*_tmp248;unsigned int _tmp247;_LL105: _tmp249=_tmp244->f1;_tmp248=_tmp244->f2;_tmp247=_tmp244->f3;_LL106:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp249,_tmp248,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmp245=0;Cyc_Tcutil_terr(_tmp247,({const char*_tmp246="the required region ordering is not satisfied here";_tag_dyneither(_tmp246,sizeof(char),51);}),_tag_dyneither(_tmp245,sizeof(void*),0));});}};}
# 897
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->name=({struct _dyneither_ptr*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp26B;_tmp26B.tag=0;_tmp26B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp269[1]={& _tmp26B};Cyc_aprintf(({const char*_tmp26A="`%s";_tag_dyneither(_tmp26A,sizeof(char),4);}),_tag_dyneither(_tmp269,sizeof(void*),1));});});_tmp268;});_tmp267->identity=
# 901
Cyc_Tcutil_new_tvar_id();_tmp267->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp267;});
struct Cyc_List_List*_tmp24C=({struct Cyc_List_List*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->hd=rgn0;_tmp266->tl=fd->tvs;_tmp266;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp24C);{
# 905
struct Cyc_RgnOrder_RgnPO*_tmp24D=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 907
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp265;_tmp265.tag=2;_tmp265.f1=rgn0;_tmp265;});_tmp264;});
{struct Cyc_List_List*_tmp24E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp24E != 0;_tmp24E=_tmp24E->tl){
struct Cyc_Absyn_Vardecl*_tmp24F=(struct Cyc_Absyn_Vardecl*)_tmp24E->hd;
_tmp24F->rgn=param_rgn;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp250=_region_malloc(r,sizeof(*_tmp250));_tmp250->v=(*_tmp24F->name).f2;_tmp250->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp252;_tmp252.tag=3;_tmp252.f1=_tmp24F;_tmp252;});_tmp251;});_tmp250->tl=locals;_tmp250;});}}
# 913
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp253=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp254=_tmp253;struct _dyneither_ptr*_tmp25A;struct Cyc_Absyn_Tqual _tmp259;void*_tmp258;int _tmp257;_LL108: _tmp25A=_tmp254.name;_tmp259=_tmp254.tq;_tmp258=_tmp254.type;_tmp257=_tmp254.inject;_LL109:;{
# 916
struct Cyc_List_List*_tmp255=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp255 != 0;_tmp255=_tmp255->tl){
struct Cyc_Absyn_Vardecl*_tmp256=(struct Cyc_Absyn_Vardecl*)_tmp255->hd;
if(Cyc_strptrcmp((*_tmp256->name).f2,(struct _dyneither_ptr*)_check_null(_tmp25A))== 0){
_tmp256->type=Cyc_Absyn_dyneither_typ(_tmp258,param_rgn,_tmp259,Cyc_Absyn_false_conref);
break;}}};}
# 924
return({struct Cyc_Tcenv_Fenv*_tmp25B=_region_malloc(r,sizeof(*_tmp25B));_tmp25B->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp263=_region_malloc(r,sizeof(*_tmp263));_tmp263->frgn=r;_tmp263->return_typ=fd->ret_type;_tmp263->delayed_effect_checks=0;_tmp263->delayed_constraint_checks=0;_tmp263;});_tmp25B->type_vars=(struct Cyc_List_List*)_tmp24C;_tmp25B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24D;_tmp25B->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp25B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp262=_region_malloc(r,sizeof(*_tmp262));_tmp262->fallthru_clause=0;_tmp262;});_tmp25B->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp25D;_tmp25D.tag=24;_tmp25D.f1=({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp261;_tmp261.tag=23;_tmp261.f1=param_rgn;_tmp261;});_tmp260;});_tmp25E->tl=({struct Cyc_List_List*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->hd=(void*)_check_null(fd->effect);_tmp25F->tl=0;_tmp25F;});_tmp25E;});_tmp25D;});_tmp25C;}));_tmp25B->curr_rgn=(void*)param_rgn;_tmp25B->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp25B->flags).in_notreadctxt=0;(_tmp25B->flags).in_lhs=0;(_tmp25B->flags).in_new=0;(_tmp25B->flags).abstract_ok=0;_tmp25B->flags;});_tmp25B->fnrgn=(struct _RegionHandle*)r;_tmp25B;});};}
# 943
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 946
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6);_tmp28C->identity=Cyc_Tcutil_new_tvar_id();_tmp28C->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp28C;});
struct Cyc_List_List*_tmp26C=({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B->hd=rgn0;_tmp28B->tl=0;_tmp28B;});
struct Cyc_RgnOrder_RgnPO*_tmp26D=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp28A;_tmp28A.tag=24;_tmp28A.f1=0;_tmp28A;});_tmp289;}),rgn0,0);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp288;_tmp288.tag=2;_tmp288.f1=rgn0;_tmp288;});_tmp287;});
struct Cyc_List_List*_tmp26E=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp26F=*((struct _tuple8*)args->hd);struct _tuple8 _tmp270=_tmp26F;struct _dyneither_ptr*_tmp27E;struct Cyc_Absyn_Tqual _tmp27D;void*_tmp27C;_LL10B: _tmp27E=_tmp270.f1;_tmp27D=_tmp270.f2;_tmp27C=_tmp270.f3;_LL10C:;
if(_tmp27E != 0){
struct Cyc_Absyn_Vardecl*_tmp271=({struct Cyc_Absyn_Vardecl*_tmp279=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp279[0]=(struct Cyc_Absyn_Vardecl)({struct Cyc_Absyn_Vardecl _tmp27A;_tmp27A.sc=Cyc_Absyn_Public;_tmp27A.name=({struct _tuple0*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->f1=Cyc_Absyn_Loc_n;_tmp27B->f2=_tmp27E;_tmp27B;});_tmp27A.tq=_tmp27D;_tmp27A.type=_tmp27C;_tmp27A.initializer=0;_tmp27A.rgn=param_rgn;_tmp27A.attributes=0;_tmp27A.escapes=0;_tmp27A;});_tmp279;});
# 962
_tmp26E=({struct Cyc_List_List*_tmp272=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp272[0]=(struct Cyc_List_List)({struct Cyc_List_List _tmp273;_tmp273.hd=_tmp271;_tmp273.tl=_tmp26E;_tmp273;});_tmp272;});{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp274=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp278;_tmp278.tag=3;_tmp278.f1=_tmp271;_tmp278;});_tmp277;});
struct _dyneither_ptr*_tmp275=_tmp27E;
locals=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp276=_region_malloc(r,sizeof(*_tmp276));_tmp276->v=_tmp275;_tmp276->b=(void*)_tmp274;_tmp276->tl=locals;_tmp276;});};}}
# 968
return({struct Cyc_Tcenv_Fenv*_tmp27F=_region_malloc(r,sizeof(*_tmp27F));_tmp27F->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp286=_region_malloc(r,sizeof(*_tmp286));_tmp286->frgn=r;_tmp286->return_typ=ret_type;_tmp286->delayed_effect_checks=0;_tmp286->delayed_constraint_checks=0;_tmp286;});_tmp27F->type_vars=(struct Cyc_List_List*)_tmp26C;_tmp27F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp26D;_tmp27F->locals=(const struct Cyc_Tcenv_Bindings*)locals;_tmp27F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp285=_region_malloc(r,sizeof(*_tmp285));_tmp285->fallthru_clause=0;_tmp285;});_tmp27F->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp281;_tmp281.tag=24;_tmp281.f1=({struct Cyc_List_List*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp284;_tmp284.tag=23;_tmp284.f1=param_rgn;_tmp284;});_tmp283;});_tmp282->tl=0;_tmp282;});_tmp281;});_tmp280;}));_tmp27F->curr_rgn=(void*)param_rgn;_tmp27F->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp27F->flags).in_notreadctxt=0;(_tmp27F->flags).in_lhs=0;(_tmp27F->flags).in_new=0;(_tmp27F->flags).abstract_ok=0;_tmp27F->flags;});_tmp27F->fnrgn=(struct _RegionHandle*)r;_tmp27F;});}
# 986
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmp28E=old_fenv;const struct Cyc_Tcenv_Bindings*_tmp2BB;struct Cyc_RgnOrder_RgnPO*_tmp2BA;struct Cyc_List_List*_tmp2B9;struct Cyc_Tcenv_SharedFenv*_tmp2B8;struct _RegionHandle*_tmp2B7;_LL10E: _tmp2BB=_tmp28E->locals;_tmp2BA=_tmp28E->region_order;_tmp2B9=_tmp28E->type_vars;_tmp2B8=_tmp28E->shared;_tmp2B7=_tmp28E->fnrgn;_LL10F:;{
# 989
struct _RegionHandle*_tmp28F=_tmp2B8->frgn;
const struct Cyc_Tcenv_Bindings*_tmp290=_tmp2BB;
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->name=({struct _dyneither_ptr*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmp2B4[1]={& _tmp2B6};Cyc_aprintf(({const char*_tmp2B5="`%s";_tag_dyneither(_tmp2B5,sizeof(char),4);}),_tag_dyneither(_tmp2B4,sizeof(void*),1));});});_tmp2B3;});_tmp2B2->identity=
# 993
Cyc_Tcutil_new_tvar_id();_tmp2B2->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmp2B2;});
# 995
{struct Cyc_List_List*_tmp291=fd->tvs;for(0;_tmp291 != 0;_tmp291=_tmp291->tl){
struct Cyc_Absyn_Kind*_tmp292=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp291->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp293=_tmp292;enum Cyc_Absyn_KindQual _tmp297;enum Cyc_Absyn_AliasQual _tmp296;_LL111: _tmp297=_tmp293->kind;_tmp296=_tmp293->aliasqual;_LL112:;
if(_tmp297 == Cyc_Absyn_RgnKind){
if(_tmp296 == Cyc_Absyn_Aliasable)
_tmp2BA=Cyc_RgnOrder_add_youngest(_tmp28F,_tmp2BA,(struct Cyc_Absyn_Tvar*)_tmp291->hd,0,0);else{
# 1001
({void*_tmp294=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp295="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmp295,sizeof(char),39);}),_tag_dyneither(_tmp294,sizeof(void*),0));});}}}}
# 1003
_tmp2BA=Cyc_RgnOrder_add_youngest(_tmp28F,_tmp2BA,rgn0,0,0);{
struct Cyc_List_List*_tmp298=({struct Cyc_List_List*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->hd=rgn0;_tmp2B1->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp2B9);_tmp2B1;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp298);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2B0;_tmp2B0.tag=2;_tmp2B0.f1=rgn0;_tmp2B0;});_tmp2AF;});
{struct Cyc_List_List*_tmp299=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp299 != 0;_tmp299=_tmp299->tl){
struct Cyc_Absyn_Vardecl*_tmp29A=(struct Cyc_Absyn_Vardecl*)_tmp299->hd;
_tmp29A->rgn=param_rgn;
_tmp290=(const struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp29B=_region_malloc(_tmp28F,sizeof(*_tmp29B));_tmp29B->v=(*_tmp29A->name).f2;_tmp29B->b=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C[0]=({struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp29D;_tmp29D.tag=3;_tmp29D.f1=_tmp29A;_tmp29D;});_tmp29C;});_tmp29B->tl=_tmp290;_tmp29B;});}}
# 1012
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp29E=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp29F=_tmp29E;struct _dyneither_ptr*_tmp2A5;struct Cyc_Absyn_Tqual _tmp2A4;void*_tmp2A3;int _tmp2A2;_LL114: _tmp2A5=_tmp29F.name;_tmp2A4=_tmp29F.tq;_tmp2A3=_tmp29F.type;_tmp2A2=_tmp29F.inject;_LL115:;{
# 1015
struct Cyc_List_List*_tmp2A0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp2A0 != 0;_tmp2A0=_tmp2A0->tl){
struct Cyc_Absyn_Vardecl*_tmp2A1=(struct Cyc_Absyn_Vardecl*)_tmp2A0->hd;
if(Cyc_strptrcmp((*_tmp2A1->name).f2,(struct _dyneither_ptr*)_check_null(_tmp2A5))== 0){
_tmp2A1->type=Cyc_Absyn_dyneither_typ(_tmp2A3,param_rgn,_tmp2A4,Cyc_Absyn_false_conref);
break;}}};}
# 1023
return({struct Cyc_Tcenv_Fenv*_tmp2A6=_region_malloc(_tmp28F,sizeof(*_tmp2A6));_tmp2A6->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*_tmp2AE=_region_malloc(_tmp28F,sizeof(*_tmp2AE));_tmp2AE->frgn=_tmp28F;_tmp2AE->return_typ=fd->ret_type;_tmp2AE->delayed_effect_checks=0;_tmp2AE->delayed_constraint_checks=0;_tmp2AE;});_tmp2A6->type_vars=(struct Cyc_List_List*)_tmp298;_tmp2A6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2BA;_tmp2A6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp290;_tmp2A6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp2AD=_region_malloc(_tmp28F,sizeof(*_tmp2AD));_tmp2AD->fallthru_clause=0;_tmp2AD;});_tmp2A6->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2A8;_tmp2A8.tag=24;_tmp2A8.f1=({struct Cyc_List_List*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2AC;_tmp2AC.tag=23;_tmp2AC.f1=param_rgn;_tmp2AC;});_tmp2AB;});_tmp2A9->tl=({struct Cyc_List_List*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->hd=(void*)_check_null(fd->effect);_tmp2AA->tl=0;_tmp2AA;});_tmp2A9;});_tmp2A8;});_tmp2A7;}));_tmp2A6->curr_rgn=(void*)param_rgn;_tmp2A6->flags=(struct Cyc_Tcenv_FenvFlags)({(_tmp2A6->flags).in_notreadctxt=0;(_tmp2A6->flags).in_lhs=0;(_tmp2A6->flags).in_new=0;(_tmp2A6->flags).abstract_ok=0;_tmp2A6->flags;});_tmp2A6->fnrgn=(struct _RegionHandle*)_tmp2B7;_tmp2A6;});};};};}
