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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Relations_Reln;
# 77 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 86
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 98
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 102
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
# 110
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
# 161
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 182
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 187
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 194
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 214
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 216
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 225
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 251
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 352
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 444 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 447
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 527
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 543
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 545
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 553
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 560
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 578
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 944 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 950
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 953
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 956
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 958
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 969
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 971
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 974
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 976
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 978
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 988
void*Cyc_Absyn_exn_typ();
# 999
extern void*Cyc_Absyn_bounds_one;
# 1004
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1007
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1010
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1015
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1026
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1079
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1081
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1093
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1099
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1114
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1175
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1177
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1180
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1182
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1194
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 105
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 129 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 136
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
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
# 152
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 156
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 178
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 184
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 197
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 203
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 211
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 215
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 219
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 226
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 80
int Cyc_Tcutil_coerceable(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 102
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
# 106
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 118
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 137
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 153
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 155
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 193
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 202
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 238 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 248
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 250
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 298
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 357
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 360
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 43 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 54
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 65
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 67
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 72
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 121
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 128
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp6D1;(_tmp6D1=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6D1,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp6D2;return(_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2->f1=0,((_tmp6D2->f2=e,_tmp6D2)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;void*_tmp3=_tmp2;struct _tuple1*_tmp5;void**_tmp6;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3;if(_tmp4->tag != 1)goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(void**)((void**)& _tmp4->f2);}}_LL2: {
# 79
struct _handler_cons _tmp7;_push_handler(& _tmp7);{int _tmp9=0;if(setjmp(_tmp7.handler))_tmp9=1;if(!_tmp9){
{struct _RegionHandle _tmpA=_new_region("r");struct _RegionHandle*r=& _tmpA;_push_region(r);
{void*_tmpB=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp5,1);void*_tmpC=_tmpB;void*_tmp10;struct Cyc_Absyn_Datatypedecl*_tmp12;struct Cyc_Absyn_Datatypefield*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp15;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpD->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpD->f1);if(_tmpE->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp6D6;void*_tmp6D5[1];struct Cyc_String_pa_PrintArg_struct _tmp6D4;(_tmp6D4.tag=0,((_tmp6D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6D5[0]=& _tmp6D4,Cyc_Tcutil_terr(e1->loc,((_tmp6D6="undeclared identifier %s",_tag_dyneither(_tmp6D6,sizeof(char),25))),_tag_dyneither(_tmp6D5,sizeof(void*),1)))))));}{
int _tmp1B=0;_npop_handler(1);return _tmp1B;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpF->tag != 0)goto _LLA;else{_tmp10=(void*)_tmpF->f1;}}_LL9:
# 86
*_tmp6=_tmp10;{
int _tmp1C=1;_npop_handler(1);return _tmp1C;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp11=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp11->tag != 2)goto _LLC;else{_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;}}_LLB:
# 89
 if(_tmp13->typs == 0){
# 91
{const char*_tmp6DA;void*_tmp6D9[1];struct Cyc_String_pa_PrintArg_struct _tmp6D8;(_tmp6D8.tag=0,((_tmp6D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp6D9[0]=& _tmp6D8,Cyc_Tcutil_terr(e->loc,((_tmp6DA="%s is a constant, not a function",_tag_dyneither(_tmp6DA,sizeof(char),33))),_tag_dyneither(_tmp6D9,sizeof(void*),1)))))));}{
int _tmp20=0;_npop_handler(1);return _tmp20;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6DD;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6DC;e->r=(void*)((_tmp6DC=_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DD.tag=30,((_tmp6DD.f1=es,((_tmp6DD.f2=_tmp12,((_tmp6DD.f3=_tmp13,_tmp6DD)))))))),_tmp6DC))));}{
int _tmp23=1;_npop_handler(1);return _tmp23;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp14->tag != 1)goto _LLE;else{_tmp15=_tmp14->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp24=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6E0;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6DF;e->r=(void*)((_tmp6DF=_cycalloc(sizeof(*_tmp6DF)),((_tmp6DF[0]=((_tmp6E0.tag=28,((_tmp6E0.f1=_tmp15->name,((_tmp6E0.f2=0,((_tmp6E0.f3=_tmp24,((_tmp6E0.f4=_tmp15,_tmp6E0)))))))))),_tmp6DF))));}{
int _tmp27=1;_npop_handler(1);return _tmp27;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp16=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp16->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp17=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp17->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp6E4;void*_tmp6E3[1];struct Cyc_String_pa_PrintArg_struct _tmp6E2;(_tmp6E2.tag=0,((_tmp6E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6E3[0]=& _tmp6E2,Cyc_Tcutil_terr(e->loc,((_tmp6E4="%s is an enum constructor, not a function",_tag_dyneither(_tmp6E4,sizeof(char),42))),_tag_dyneither(_tmp6E3,sizeof(void*),1)))))));}{
int _tmp2B=0;_npop_handler(1);return _tmp2B;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp8=(void*)_exn_thrown;void*_tmp2D=_tmp8;void*_tmp2F;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp6E8;void*_tmp6E7[1];struct Cyc_String_pa_PrintArg_struct _tmp6E6;(_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6E7[0]=& _tmp6E6,Cyc_Tcutil_terr(e1->loc,((_tmp6E8="undeclared identifier %s",_tag_dyneither(_tmp6E8,sizeof(char),25))),_tag_dyneither(_tmp6E7,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2F=_tmp2D;_LL16:(void)_rethrow(_tmp2F);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6EB;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6EA;e->r=(void*)((_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA[0]=((_tmp6EB.tag=25,((_tmp6EB.f1=des,_tmp6EB)))),_tmp6EA))));}
return;}{
# 125
void*t=*topt;
void*_tmp35=Cyc_Tcutil_compress(t);void*_tmp36=_tmp35;union Cyc_Absyn_AggrInfoU _tmp38;void*_tmp3A;struct Cyc_Absyn_Tqual _tmp3B;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36;if(_tmp37->tag != 11)goto _LL1A;else{_tmp38=(_tmp37->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3D=_tmp38;struct Cyc_Absyn_Aggrdecl*_tmp3E;_LL21: if((_tmp3D.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp6EE;void*_tmp6ED;(_tmp6ED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6EE="struct type not properly set",_tag_dyneither(_tmp6EE,sizeof(char),29))),_tag_dyneither(_tmp6ED,sizeof(void*),0)));}_LL23: if((_tmp3D.KnownAggr).tag != 2)goto _LL20;_tmp3E=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3D.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6F1;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6F0;e->r=(void*)((_tmp6F0=_cycalloc(sizeof(*_tmp6F0)),((_tmp6F0[0]=((_tmp6F1.tag=28,((_tmp6F1.f1=_tmp3E->name,((_tmp6F1.f2=0,((_tmp6F1.f3=des,((_tmp6F1.f4=_tmp3E,_tmp6F1)))))))))),_tmp6F0))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36;if(_tmp39->tag != 8)goto _LL1C;else{_tmp3A=(_tmp39->f1).elt_type;_tmp3B=(_tmp39->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6F4;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6F3;e->r=(void*)((_tmp6F3=_cycalloc(sizeof(*_tmp6F3)),((_tmp6F3[0]=((_tmp6F4.tag=25,((_tmp6F4.f1=des,_tmp6F4)))),_tmp6F3))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36;if(_tmp3C->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6F7;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6F6;e->r=(void*)((_tmp6F6=_cycalloc(sizeof(*_tmp6F6)),((_tmp6F6[0]=((_tmp6F7.tag=29,((_tmp6F7.f1=t,((_tmp6F7.f2=des,_tmp6F7)))))),_tmp6F6))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6FA;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6F9;e->r=(void*)((_tmp6F9=_cycalloc(sizeof(*_tmp6F9)),((_tmp6F9[0]=((_tmp6FA.tag=25,((_tmp6FA.f1=des,_tmp6FA)))),_tmp6F9))));}goto _LL17;_LL17:;};}
# 142
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 146
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 152
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcutil_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6FF;void*_tmp6FE[2];struct Cyc_String_pa_PrintArg_struct _tmp6FD;struct Cyc_String_pa_PrintArg_struct _tmp6FC;(_tmp6FC.tag=0,((_tmp6FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6FD.tag=0,((_tmp6FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6FE[0]=& _tmp6FD,((_tmp6FE[1]=& _tmp6FC,Cyc_Tcutil_terr(e->loc,((_tmp6FF="test of %s has type %s instead of integral or pointer type",_tag_dyneither(_tmp6FF,sizeof(char),59))),_tag_dyneither(_tmp6FE,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4D=e->r;void*_tmp4E=_tmp4D;enum Cyc_Absyn_Primop _tmp50;struct Cyc_List_List*_tmp51;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E;if(_tmp4F->tag != 2)goto _LL28;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL27:
# 163
 if(_tmp50 == Cyc_Absyn_Eq  || _tmp50 == Cyc_Absyn_Neq){
struct _tuple0 _tmp700;struct _tuple0 _tmp52=(_tmp700.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->topt),((_tmp700.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp51))->tl))->hd)->topt),_tmp700)));struct _tuple0 _tmp53=_tmp52;void*_tmp55;void*_tmp57;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp54=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f1;if(_tmp54->tag != 15)goto _LL2D;else{_tmp55=(void*)_tmp54->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp56=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f2;if(_tmp56->tag != 15)goto _LL2D;else{_tmp57=(void*)_tmp56->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp703;struct Cyc_Tcexp_TestEnv _tmp702;return(_tmp702.eq=((_tmp703=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp703)),((_tmp703->f1=_tmp55,((_tmp703->f2=_tmp57,_tmp703)))))),((_tmp702.isTrue=_tmp50 == Cyc_Absyn_Eq,_tmp702)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp704;return(_tmp704.eq=0,((_tmp704.isTrue=0,_tmp704)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp5C=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp5D=_tmp5C;enum Cyc_Absyn_Sign _tmp5E;enum Cyc_Absyn_Sign _tmp5F;int _tmp60;enum Cyc_Absyn_Sign _tmp61;int _tmp62;struct _dyneither_ptr _tmp63;struct _dyneither_ptr _tmp64;_LL30: if((_tmp5D.Char_c).tag != 2)goto _LL32;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_Signed)goto _LL32;_LL31:
 t=Cyc_Absyn_schar_typ;goto _LL2F;_LL32: if((_tmp5D.Char_c).tag != 2)goto _LL34;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL34;_LL33:
 t=Cyc_Absyn_uchar_typ;goto _LL2F;_LL34: if((_tmp5D.Char_c).tag != 2)goto _LL36;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_None)goto _LL36;_LL35:
 t=Cyc_Absyn_char_typ;goto _LL2F;_LL36: if((_tmp5D.Wchar_c).tag != 3)goto _LL38;_LL37:
 t=Cyc_Absyn_wchar_typ();goto _LL2F;_LL38: if((_tmp5D.Short_c).tag != 4)goto _LL3A;_tmp5E=((struct _tuple5)(_tmp5D.Short_c).val).f1;_LL39:
# 210
 t=_tmp5E == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL2F;_LL3A: if((_tmp5D.LongLong_c).tag != 6)goto _LL3C;_tmp5F=((struct _tuple7)(_tmp5D.LongLong_c).val).f1;_LL3B:
# 212
 t=_tmp5F == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL2F;_LL3C: if((_tmp5D.Float_c).tag != 7)goto _LL3E;_tmp60=((struct _tuple8)(_tmp5D.Float_c).val).f2;_LL3D:
 t=Cyc_Absyn_float_typ(_tmp60);goto _LL2F;_LL3E: if((_tmp5D.Int_c).tag != 5)goto _LL40;_tmp61=((struct _tuple6)(_tmp5D.Int_c).val).f1;_tmp62=((struct _tuple6)(_tmp5D.Int_c).val).f2;_LL3F:
# 215
 if(topt == 0)
t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 222
void*_tmp65=Cyc_Tcutil_compress(*topt);void*_tmp66=_tmp65;enum Cyc_Absyn_Sign _tmp68;enum Cyc_Absyn_Sign _tmp6A;enum Cyc_Absyn_Sign _tmp6C;enum Cyc_Absyn_Sign _tmp6E;void*_tmp70;struct Cyc_Absyn_Tqual _tmp71;void*_tmp72;union Cyc_Absyn_Constraint*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;void*_tmp77;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp67->tag != 6)goto _LL49;else{_tmp68=_tmp67->f1;if(_tmp67->f2 != Cyc_Absyn_Char_sz)goto _LL49;}}_LL48:
# 224
 switch(_tmp68){case Cyc_Absyn_Unsigned: _LL55:
 t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL56:
 t=Cyc_Absyn_schar_typ;break;case Cyc_Absyn_None: _LL57:
 t=Cyc_Absyn_char_typ;break;}
# 229
*c=Cyc_Absyn_Char_c(_tmp68,(char)_tmp62);
goto _LL46;_LL49: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp69->tag != 6)goto _LL4B;else{_tmp6A=_tmp69->f1;if(_tmp69->f2 != Cyc_Absyn_Short_sz)goto _LL4B;}}_LL4A:
# 232
 t=_tmp6A == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
*c=Cyc_Absyn_Short_c(_tmp6A,(short)_tmp62);
goto _LL46;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6B->tag != 6)goto _LL4D;else{_tmp6C=_tmp6B->f1;if(_tmp6B->f2 != Cyc_Absyn_Int_sz)goto _LL4D;}}_LL4C:
# 236
 t=_tmp6C == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6D->tag != 6)goto _LL4F;else{_tmp6E=_tmp6D->f1;if(_tmp6D->f2 != Cyc_Absyn_Long_sz)goto _LL4F;}}_LL4E:
# 239
 t=_tmp6E == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66;if(_tmp6F->tag != 5)goto _LL51;else{_tmp70=(_tmp6F->f1).elt_typ;_tmp71=(_tmp6F->f1).elt_tq;_tmp72=((_tmp6F->f1).ptr_atts).rgn;_tmp73=((_tmp6F->f1).ptr_atts).nullable;_tmp74=((_tmp6F->f1).ptr_atts).bounds;_tmp75=((_tmp6F->f1).ptr_atts).zero_term;}}if(!(_tmp62 == 0))goto _LL51;_LL50: {
# 243
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp73))return*topt;{
struct Cyc_List_List*_tmp78=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp70A;struct Cyc_Absyn_PtrInfo _tmp709;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp708;t=(void*)((_tmp708=_cycalloc(sizeof(*_tmp708)),((_tmp708[0]=((_tmp70A.tag=5,((_tmp70A.f1=((_tmp709.elt_typ=_tmp70,((_tmp709.elt_tq=_tmp71,((_tmp709.ptr_atts=(
((_tmp709.ptr_atts).rgn=_tmp72,(((_tmp709.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp709.ptr_atts).bounds=_tmp74,(((_tmp709.ptr_atts).zero_term=_tmp75,(((_tmp709.ptr_atts).ptrloc=0,_tmp709.ptr_atts)))))))))),_tmp709)))))),_tmp70A)))),_tmp708))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66;if(_tmp76->tag != 19)goto _LL53;else{_tmp77=(void*)_tmp76->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp70D;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp70C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7D=(_tmp70C=_cycalloc(sizeof(*_tmp70C)),((_tmp70C[0]=((_tmp70D.tag=18,((_tmp70D.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp62,0),_tmp70D)))),_tmp70C)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp710;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp70F;t=(void*)((_tmp70F=_cycalloc(sizeof(*_tmp70F)),((_tmp70F[0]=((_tmp710.tag=19,((_tmp710.f1=(void*)_tmp7D,_tmp710)))),_tmp70F))));}
goto _LL46;}_LL53:;_LL54:
# 261
 t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL46:;}
# 264
goto _LL2F;_LL40: if((_tmp5D.String_c).tag != 8)goto _LL42;_tmp63=(struct _dyneither_ptr)(_tmp5D.String_c).val;_LL41:
# 266
 string_numelts=(int)_get_dyneither_size(_tmp63,sizeof(char));
_tmp64=_tmp63;goto _LL43;_LL42: if((_tmp5D.Wstring_c).tag != 9)goto _LL44;_tmp64=(struct _dyneither_ptr)(_tmp5D.Wstring_c).val;_LL43:
# 269
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp64);
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 273
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
elen->topt=Cyc_Absyn_uint_typ;{
# 278
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp713;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp712;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp82=(_tmp712=_cycalloc(sizeof(*_tmp712)),((_tmp712[0]=((_tmp713.tag=1,((_tmp713.f1=elen,_tmp713)))),_tmp712)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp82,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_tmp83=Cyc_Tcutil_compress(*topt);void*_tmp84=_tmp83;struct Cyc_Absyn_Tqual _tmp86;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp84;if(_tmp85->tag != 8)goto _LL5C;else{_tmp86=(_tmp85->f1).tq;}}_LL5B:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp86,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84;if(_tmp87->tag != 5)goto _LL5E;}_LL5D:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp82,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 302
goto _LL59;_LL5E:;_LL5F:
 goto _LL59;_LL59:;}
# 306
return t;};};_LL44: if((_tmp5D.Null_c).tag != 1)goto _LL2F;_LL45:
# 308
 if(topt != 0){
void*_tmp8A=Cyc_Tcutil_compress(*topt);void*_tmp8B=_tmp8A;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8F;union Cyc_Absyn_Constraint*_tmp90;union Cyc_Absyn_Constraint*_tmp91;union Cyc_Absyn_Constraint*_tmp92;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B;if(_tmp8C->tag != 5)goto _LL63;else{_tmp8D=(_tmp8C->f1).elt_typ;_tmp8E=(_tmp8C->f1).elt_tq;_tmp8F=((_tmp8C->f1).ptr_atts).rgn;_tmp90=((_tmp8C->f1).ptr_atts).nullable;_tmp91=((_tmp8C->f1).ptr_atts).bounds;_tmp92=((_tmp8C->f1).ptr_atts).zero_term;}}_LL62:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp90))return*topt;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp719;struct Cyc_Absyn_PtrInfo _tmp718;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp717;return(void*)((_tmp717=_cycalloc(sizeof(*_tmp717)),((_tmp717[0]=((_tmp719.tag=5,((_tmp719.f1=((_tmp718.elt_typ=_tmp8D,((_tmp718.elt_tq=_tmp8E,((_tmp718.ptr_atts=(((_tmp718.ptr_atts).rgn=_tmp8F,(((_tmp718.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp718.ptr_atts).bounds=_tmp91,(((_tmp718.ptr_atts).zero_term=_tmp92,(((_tmp718.ptr_atts).ptrloc=0,_tmp718.ptr_atts)))))))))),_tmp718)))))),_tmp719)))),_tmp717))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp96=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp728;struct Cyc_Core_Opt*_tmp727;struct Cyc_Core_Opt*_tmp726;struct Cyc_Absyn_PtrInfo _tmp725;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp724;t=(void*)((_tmp724=_cycalloc(sizeof(*_tmp724)),((_tmp724[0]=((_tmp728.tag=5,((_tmp728.f1=((_tmp725.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp726=_cycalloc(sizeof(*_tmp726)),((_tmp726->v=_tmp96,_tmp726))))),((_tmp725.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp725.ptr_atts=(
((_tmp725.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp727=_cycalloc(sizeof(*_tmp727)),((_tmp727->v=_tmp96,_tmp727))))),(((_tmp725.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp725.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp725.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp725.ptr_atts).ptrloc=0,_tmp725.ptr_atts)))))))))),_tmp725)))))),_tmp728)))),_tmp724))));}
# 325
goto _LL2F;};_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_tmp9C=*((void**)_check_null(b));void*_tmp9D=_tmp9C;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Fndecl*_tmpA2;struct Cyc_Absyn_Vardecl*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA6;struct Cyc_Absyn_Vardecl*_tmpA8;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp9E=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp9D;if(_tmp9E->tag != 0)goto _LL68;}_LL67: {
# 342
struct _handler_cons _tmpA9;_push_handler(& _tmpA9);{int _tmpAB=0;if(setjmp(_tmpA9.handler))_tmpAB=1;if(!_tmpAB){
{struct _RegionHandle _tmpAC=_new_region("r");struct _RegionHandle*r=& _tmpAC;_push_region(r);
{void*_tmpAD=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmpAE=_tmpAD;void*_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB4;struct Cyc_Absyn_Enumfield*_tmpB5;void*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Datatypedecl*_tmpBA;struct Cyc_Absyn_Datatypefield*_tmpBB;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpAF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpAF->tag != 0)goto _LL75;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpB0=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpAF->f1);if(_tmpB0->tag != 0)goto _LL75;}}_LL74: {
# 346
const char*_tmp72C;void*_tmp72B[1];struct Cyc_String_pa_PrintArg_struct _tmp72A;void*_tmpC0=(_tmp72A.tag=0,((_tmp72A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp72B[0]=& _tmp72A,Cyc_Tcexp_expr_err(te,loc,0,((_tmp72C="undeclared identifier %s",_tag_dyneither(_tmp72C,sizeof(char),25))),_tag_dyneither(_tmp72B,sizeof(void*),1)))))));_npop_handler(1);return _tmpC0;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB1->tag != 0)goto _LL77;else{_tmpB2=(void*)_tmpB1->f1;}}_LL76:
# 348
*b=_tmpB2;{
void*_tmpC1=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpC1;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpB3=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB3->tag != 3)goto _LL79;else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp72F;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp72E;e->r=(void*)((_tmp72E=_cycalloc(sizeof(*_tmp72E)),((_tmp72E[0]=((_tmp72F.tag=31,((_tmp72F.f1=_tmpB5->name,((_tmp72F.f2=_tmpB4,((_tmp72F.f3=_tmpB5,_tmp72F)))))))),_tmp72E))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp732;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp731;void*_tmpC6=(void*)((_tmp731=_cycalloc(sizeof(*_tmp731)),((_tmp731[0]=((_tmp732.tag=13,((_tmp732.f1=_tmpB4->name,((_tmp732.f2=_tmpB4,_tmp732)))))),_tmp731))));_npop_handler(1);return _tmpC6;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpB6=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB6->tag != 4)goto _LL7B;else{_tmpB7=(void*)_tmpB6->f1;_tmpB8=_tmpB6->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp735;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp734;e->r=(void*)((_tmp734=_cycalloc(sizeof(*_tmp734)),((_tmp734[0]=((_tmp735.tag=32,((_tmp735.f1=_tmpB8->name,((_tmp735.f2=_tmpB7,((_tmp735.f3=_tmpB8,_tmp735)))))))),_tmp734))));}{
void*_tmpC9=_tmpB7;_npop_handler(1);return _tmpC9;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB9=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB9->tag != 2)goto _LL7D;else{_tmpBA=_tmpB9->f1;_tmpBB=_tmpB9->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp738;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp737;e->r=(void*)((_tmp737=_cycalloc(sizeof(*_tmp737)),((_tmp737[0]=((_tmp738.tag=30,((_tmp738.f1=0,((_tmp738.f2=_tmpBA,((_tmp738.f3=_tmpBB,_tmp738)))))))),_tmp737))));}{
void*_tmpCC=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpBA,_tmpBB);_npop_handler(1);return _tmpCC;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpBC=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpBC->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp73C;void*_tmp73B[1];struct Cyc_String_pa_PrintArg_struct _tmp73A;void*_tmpD0=(_tmp73A.tag=0,((_tmp73A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp73B[0]=& _tmp73A,Cyc_Tcexp_expr_err(te,loc,0,((_tmp73C="bad occurrence of type name %s",_tag_dyneither(_tmp73C,sizeof(char),31))),_tag_dyneither(_tmp73B,sizeof(void*),1)))))));_npop_handler(1);return _tmpD0;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpAA=(void*)_exn_thrown;void*_tmpD2=_tmpAA;void*_tmpD4;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpD3=(struct Cyc_Dict_Absent_exn_struct*)_tmpD2;if(_tmpD3->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp740;void*_tmp73F[1];struct Cyc_String_pa_PrintArg_struct _tmp73E;return(_tmp73E.tag=0,((_tmp73E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp73F[0]=& _tmp73E,Cyc_Tcexp_expr_err(te,loc,0,((_tmp740="undeclared identifier %s",_tag_dyneither(_tmp740,sizeof(char),25))),_tag_dyneither(_tmp73F,sizeof(void*),1)))))));}_LL82: _tmpD4=_tmpD2;_LL83:(void)_rethrow(_tmpD4);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp9D;if(_tmp9F->tag != 1)goto _LL6A;else{_tmpA0=_tmp9F->f1;}}_LL69:
# 368
*q=*_tmpA0->name;
return _tmpA0->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA1=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA1->tag != 2)goto _LL6C;else{_tmpA2=_tmpA1->f1;}}_LL6B:
*q=*_tmpA2->name;return Cyc_Tcutil_fndecl2typ(_tmpA2);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpA3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA3->tag != 5)goto _LL6E;else{_tmpA4=_tmpA3->f1;}}_LL6D:
 _tmpA6=_tmpA4;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA5->tag != 4)goto _LL70;else{_tmpA6=_tmpA5->f1;}}_LL6F:
 _tmpA8=_tmpA6;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpA7=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA7->tag != 3)goto _LL65;else{_tmpA8=_tmpA7->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp741;(*q).f1=(((_tmp741.Loc_n).val=0,(((_tmp741.Loc_n).tag=4,_tmp741))));}
if(te->allow_valueof){
void*_tmpD9=Cyc_Tcutil_compress(_tmpA8->type);void*_tmpDA=_tmpD9;void*_tmpDC;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpDA;if(_tmpDB->tag != 19)goto _LL87;else{_tmpDC=(void*)_tmpDB->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp744;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp743;e->r=(void*)((_tmp743=_cycalloc(sizeof(*_tmp743)),((_tmp743[0]=((_tmp744.tag=38,((_tmp744.f1=_tmpDC,_tmp744)))),_tmp743))));}
goto _LL84;_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 383
return _tmpA8->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_tmpDF=fmt->r;void*_tmpE0=_tmpDF;struct _dyneither_ptr _tmpE2;struct _dyneither_ptr _tmpE5;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE1->tag != 0)goto _LL8C;else{if(((_tmpE1->f1).String_c).tag != 8)goto _LL8C;_tmpE2=(struct _dyneither_ptr)((_tmpE1->f1).String_c).val;}}_LL8B:
 _tmpE5=_tmpE2;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE3->tag != 13)goto _LL8E;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmpE3->f2)->r;if(_tmpE4->tag != 0)goto _LL8E;else{if(((_tmpE4->f1).String_c).tag != 8)goto _LL8E;_tmpE5=(struct _dyneither_ptr)((_tmpE4->f1).String_c).val;}}}_LL8D:
# 400
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpE5,fmt->loc);goto _LL89;_LL8E:;_LL8F:
# 404
 if(opt_args != 0){
struct Cyc_List_List*_tmpE6=(struct Cyc_List_List*)opt_args->v;
for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpE6->hd);{
struct _RegionHandle _tmpE7=_new_region("temp");struct _RegionHandle*temp=& _tmpE7;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpE6->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpE6->hd)){
const char*_tmp747;void*_tmp746;(_tmp746=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE6->hd)->loc,((_tmp747="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp747,sizeof(char),49))),_tag_dyneither(_tmp746,sizeof(void*),0)));}
# 409
;_pop_region(temp);};}}
# 415
return;_LL89:;}
# 417
if(opt_args != 0){
struct Cyc_List_List*_tmpEA=(struct Cyc_List_List*)opt_args->v;
# 420
for(0;desc_types != 0  && _tmpEA != 0;
(((desc_types=desc_types->tl,_tmpEA=_tmpEA->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpEA->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
{const char*_tmp74C;void*_tmp74B[2];struct Cyc_String_pa_PrintArg_struct _tmp74A;struct Cyc_String_pa_PrintArg_struct _tmp749;(_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp74A.tag=0,((_tmp74A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp74B[0]=& _tmp74A,((_tmp74B[1]=& _tmp749,Cyc_Tcutil_terr(e->loc,((_tmp74C="descriptor has type %s but argument has type %s",_tag_dyneither(_tmp74C,sizeof(char),48))),_tag_dyneither(_tmp74B,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp74D;*alias_arg_exps=((_tmp74D=_cycalloc(sizeof(*_tmp74D)),((_tmp74D->hd=(void*)arg_cnt,((_tmp74D->tl=*alias_arg_exps,_tmp74D))))));}{
struct _RegionHandle _tmpF0=_new_region("temp");struct _RegionHandle*temp=& _tmpF0;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp750;void*_tmp74F;(_tmp74F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp750="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp750,sizeof(char),49))),_tag_dyneither(_tmp74F,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp753;void*_tmp752;(_tmp752=0,Cyc_Tcutil_terr(fmt->loc,((_tmp753="too few arguments",_tag_dyneither(_tmp753,sizeof(char),18))),_tag_dyneither(_tmp752,sizeof(void*),0)));}
if(_tmpEA != 0){
{const char*_tmp756;void*_tmp755;(_tmp755=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp756="too many arguments",_tag_dyneither(_tmp756,sizeof(char),19))),_tag_dyneither(_tmp755,sizeof(void*),0)));}{
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp759;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp758;(int)_throw((void*)((_tmp758=_cycalloc_atomic(sizeof(*_tmp758)),((_tmp758[0]=((_tmp759.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp759)),_tmp758)))));};}}}
# 447
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 449
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 453
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp75D;void*_tmp75C[1];struct Cyc_String_pa_PrintArg_struct _tmp75B;(_tmp75B.tag=0,((_tmp75B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp75C[0]=& _tmp75B,Cyc_Tcutil_terr(loc,((_tmp75D="expecting numeric type but found %s",_tag_dyneither(_tmp75D,sizeof(char),36))),_tag_dyneither(_tmp75C,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 457
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp761;void*_tmp760[1];struct Cyc_String_pa_PrintArg_struct _tmp75F;(_tmp75F.tag=0,((_tmp75F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp760[0]=& _tmp75F,Cyc_Tcutil_terr(loc,((_tmp761="expecting integral or * type but found %s",_tag_dyneither(_tmp761,sizeof(char),42))),_tag_dyneither(_tmp760,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 462
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp765;void*_tmp764[1];struct Cyc_String_pa_PrintArg_struct _tmp763;(_tmp763.tag=0,((_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp764[0]=& _tmp763,Cyc_Tcutil_terr(loc,((_tmp765="expecting integral type but found %s",_tag_dyneither(_tmp765,sizeof(char),37))),_tag_dyneither(_tmp764,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 466
{void*_tmp102=t;union Cyc_Absyn_Constraint*_tmp105;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp103=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102;if(_tmp103->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp104=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102;if(_tmp104->tag != 5)goto _LL9B;else{_tmp105=((_tmp104->f1).ptr_atts).bounds;}}_LL9A:
# 469
{void*_tmp106=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp105);void*_tmp107=_tmp106;struct Cyc_Absyn_Exp*_tmp10A;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp108=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp107;if(_tmp108->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp109=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp107;if(_tmp109->tag != 1)goto _LL9D;else{_tmp10A=_tmp109->f1;}}_LLA1:
# 472
 if(!Cyc_Evexp_c_can_eval(_tmp10A) && !((unsigned int)Cyc_Tcenv_allow_valueof)){
const char*_tmp768;void*_tmp767;(_tmp767=0,Cyc_Tcutil_terr(loc,((_tmp768="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp768,sizeof(char),55))),_tag_dyneither(_tmp767,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 476
goto _LL96;_LL9B:;_LL9C: {
# 478
const char*_tmp76C;void*_tmp76B[1];struct Cyc_String_pa_PrintArg_struct _tmp76A;(_tmp76A.tag=0,((_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp76B[0]=& _tmp76A,Cyc_Tcutil_terr(loc,((_tmp76C="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp76C,sizeof(char),47))),_tag_dyneither(_tmp76B,sizeof(void*),1)))))));}_LL96:;}
# 480
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp76F;void*_tmp76E;(_tmp76E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp76F="Non-unary primop",_tag_dyneither(_tmp76F,sizeof(char),17))),_tag_dyneither(_tmp76E,sizeof(void*),0)));}}}
# 486
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 489
if(!checker(e1)){
{const char*_tmp773;void*_tmp772[1];struct Cyc_String_pa_PrintArg_struct _tmp771;(_tmp771.tag=0,((_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp772[0]=& _tmp771,Cyc_Tcutil_terr(e1->loc,((_tmp773="type %s cannot be used here",_tag_dyneither(_tmp773,sizeof(char),28))),_tag_dyneither(_tmp772,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 493
if(!checker(e2)){
{const char*_tmp777;void*_tmp776[1];struct Cyc_String_pa_PrintArg_struct _tmp775;(_tmp775.tag=0,((_tmp775.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp776[0]=& _tmp775,Cyc_Tcutil_terr(e2->loc,((_tmp777="type %s cannot be used here",_tag_dyneither(_tmp777,sizeof(char),28))),_tag_dyneither(_tmp776,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 497
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 502
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp118=t1;void*_tmp11A;struct Cyc_Absyn_Tqual _tmp11B;void*_tmp11C;union Cyc_Absyn_Constraint*_tmp11D;union Cyc_Absyn_Constraint*_tmp11E;union Cyc_Absyn_Constraint*_tmp11F;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp119=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp118;if(_tmp119->tag != 5)goto _LLA6;else{_tmp11A=(_tmp119->f1).elt_typ;_tmp11B=(_tmp119->f1).elt_tq;_tmp11C=((_tmp119->f1).ptr_atts).rgn;_tmp11D=((_tmp119->f1).ptr_atts).nullable;_tmp11E=((_tmp119->f1).ptr_atts).bounds;_tmp11F=((_tmp119->f1).ptr_atts).zero_term;}}_LLA5:
# 507
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp11A),& Cyc_Tcutil_tmk)){
const char*_tmp77A;void*_tmp779;(_tmp779=0,Cyc_Tcutil_terr(e1->loc,((_tmp77A="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp77A,sizeof(char),50))),_tag_dyneither(_tmp779,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp77D;void*_tmp77C;(_tmp77C=0,Cyc_Tcutil_terr(e1->loc,((_tmp77D="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp77D,sizeof(char),54))),_tag_dyneither(_tmp77C,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp781;void*_tmp780[1];struct Cyc_String_pa_PrintArg_struct _tmp77F;(_tmp77F.tag=0,((_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp780[0]=& _tmp77F,Cyc_Tcutil_terr(e2->loc,((_tmp781="expecting int but found %s",_tag_dyneither(_tmp781,sizeof(char),27))),_tag_dyneither(_tmp780,sizeof(void*),1)))))));}{
void*_tmp127=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp11E);void*_tmp128=_tmp127;struct Cyc_Absyn_Exp*_tmp12B;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp129=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp128;if(_tmp129->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp128;if(_tmp12A->tag != 1)goto _LLA8;else{_tmp12B=_tmp12A->f1;}}_LLAC:
# 518
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11F)){
const char*_tmp784;void*_tmp783;(_tmp783=0,Cyc_Tcutil_warn(e1->loc,((_tmp784="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp784,sizeof(char),70))),_tag_dyneither(_tmp783,sizeof(void*),0)));}{
# 526
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp78A;struct Cyc_Absyn_PtrInfo _tmp789;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp788;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12E=(_tmp788=_cycalloc(sizeof(*_tmp788)),((_tmp788[0]=((_tmp78A.tag=5,((_tmp78A.f1=((_tmp789.elt_typ=_tmp11A,((_tmp789.elt_tq=_tmp11B,((_tmp789.ptr_atts=(
((_tmp789.ptr_atts).rgn=_tmp11C,(((_tmp789.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp789.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp789.ptr_atts).zero_term=_tmp11F,(((_tmp789.ptr_atts).ptrloc=0,_tmp789.ptr_atts)))))))))),_tmp789)))))),_tmp78A)))),_tmp788)));
# 530
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp12E,Cyc_Absyn_Other_coercion);
return(void*)_tmp12E;};_LLA8:;};_LLA6:;_LLA7:
# 533
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3:;}
# 538
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
{const char*_tmp78F;void*_tmp78E[2];struct Cyc_String_pa_PrintArg_struct _tmp78D;struct Cyc_String_pa_PrintArg_struct _tmp78C;(_tmp78C.tag=0,((_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_typ2string(t2)),((_tmp78D.tag=0,((_tmp78D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp78E[0]=& _tmp78D,((_tmp78E[1]=& _tmp78C,Cyc_Tcutil_terr(e1->loc,((_tmp78F="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp78F,sizeof(char),59))),_tag_dyneither(_tmp78E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 551
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp794;void*_tmp793[2];struct Cyc_String_pa_PrintArg_struct _tmp792;struct Cyc_String_pa_PrintArg_struct _tmp791;(_tmp791.tag=0,((_tmp791.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp792.tag=0,((_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp793[0]=& _tmp792,((_tmp793[1]=& _tmp791,Cyc_Tcutil_terr(e1->loc,((_tmp794="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp794,sizeof(char),59))),_tag_dyneither(_tmp793,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 560
{const char*_tmp797;void*_tmp796;(_tmp796=0,Cyc_Tcutil_warn(e1->loc,((_tmp797="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp797,sizeof(char),60))),_tag_dyneither(_tmp796,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 564
return Cyc_Absyn_sint_typ;}else{
# 566
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp79A;void*_tmp799;(_tmp799=0,Cyc_Tcutil_terr(e1->loc,((_tmp79A="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp79A,sizeof(char),50))),_tag_dyneither(_tmp799,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp79D;void*_tmp79C;(_tmp79C=0,Cyc_Tcutil_terr(e1->loc,((_tmp79D="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp79D,sizeof(char),54))),_tag_dyneither(_tmp79C,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp7A2;void*_tmp7A1[2];struct Cyc_String_pa_PrintArg_struct _tmp7A0;struct Cyc_String_pa_PrintArg_struct _tmp79F;(_tmp79F.tag=0,((_tmp79F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7A0.tag=0,((_tmp7A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7A1[0]=& _tmp7A0,((_tmp7A1[1]=& _tmp79F,Cyc_Tcutil_terr(e2->loc,((_tmp7A2="expecting either %s or int but found %s",_tag_dyneither(_tmp7A2,sizeof(char),40))),_tag_dyneither(_tmp7A1,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 575
return t1;}}}
# 579
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp7A5;void*_tmp7A4;(_tmp7A4=0,Cyc_Tcutil_warn(e1->loc,((_tmp7A5="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp7A5,sizeof(char),60))),_tag_dyneither(_tmp7A4,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 589
{const char*_tmp7A8;void*_tmp7A7;(_tmp7A7=0,Cyc_Tcutil_warn(e1->loc,((_tmp7A8="thin pointer subtraction!",_tag_dyneither(_tmp7A8,sizeof(char),26))),_tag_dyneither(_tmp7A7,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 592
{const char*_tmp7AB;void*_tmp7AA;(_tmp7AA=0,Cyc_Tcutil_warn(e1->loc,((_tmp7AB="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp7AB,sizeof(char),56))),_tag_dyneither(_tmp7AA,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 595
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp7AE;void*_tmp7AD;(_tmp7AD=0,Cyc_Tcutil_warn(e1->loc,((_tmp7AE="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp7AE,sizeof(char),51))),_tag_dyneither(_tmp7AD,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 600
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 603
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 611
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 616
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 629
pointer_cmp: {
struct _tuple0 _tmp7AF;struct _tuple0 _tmp14C=(_tmp7AF.f1=Cyc_Tcutil_compress(t1),((_tmp7AF.f2=Cyc_Tcutil_compress(t2),_tmp7AF)));struct _tuple0 _tmp14D=_tmp14C;void*_tmp14F;void*_tmp151;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp14E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp14E->tag != 5)goto _LLB0;else{_tmp14F=(_tmp14E->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp150->tag != 5)goto _LLB0;else{_tmp151=(_tmp150->f1).elt_typ;}};_LLAF:
# 633
 if(Cyc_Tcutil_unify(_tmp14F,_tmp151))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp152->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp153=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp153->tag != 15)goto _LLB2;};_LLB1:
# 635
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 638
{const char*_tmp7B4;void*_tmp7B3[2];struct Cyc_String_pa_PrintArg_struct _tmp7B2;struct Cyc_String_pa_PrintArg_struct _tmp7B1;(_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7B2.tag=0,((_tmp7B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B3[0]=& _tmp7B2,((_tmp7B3[1]=& _tmp7B1,Cyc_Tcutil_terr(loc,((_tmp7B4="comparison not allowed between %s and %s",_tag_dyneither(_tmp7B4,sizeof(char),41))),_tag_dyneither(_tmp7B3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 645
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 647
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 653
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmp7B5;struct _tuple0 _tmp159=(_tmp7B5.f1=t1,((_tmp7B5.f2=t2,_tmp7B5)));struct _tuple0 _tmp15A=_tmp159;void*_tmp15C;void*_tmp15E;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f1;if(_tmp15B->tag != 15)goto _LLB7;else{_tmp15C=(void*)_tmp15B->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f2;if(_tmp15D->tag != 15)goto _LLB7;else{_tmp15E=(void*)_tmp15D->f1;}};_LLB6:
# 658
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 661
{const char*_tmp7BA;void*_tmp7B9[2];struct Cyc_String_pa_PrintArg_struct _tmp7B8;struct Cyc_String_pa_PrintArg_struct _tmp7B7;(_tmp7B7.tag=0,((_tmp7B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7B8.tag=0,((_tmp7B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B9[0]=& _tmp7B8,((_tmp7B9[1]=& _tmp7B7,Cyc_Tcutil_terr(loc,((_tmp7BA="comparison not allowed between %s and %s",_tag_dyneither(_tmp7BA,sizeof(char),41))),_tag_dyneither(_tmp7B9,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 669
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 671
switch(p){case Cyc_Absyn_Plus: _LLB9:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 675
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 678
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 688
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 691
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 696
const char*_tmp7BD;void*_tmp7BC;(_tmp7BC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7BD="bad binary primop",_tag_dyneither(_tmp7BD,sizeof(char),18))),_tag_dyneither(_tmp7BC,sizeof(void*),0)));}}}
# 700
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 708
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp166=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmp167=Cyc_Tcexp_tcExp(te,topt,_tmp166);
if(!Cyc_Tcutil_is_numeric(_tmp166)){
const char*_tmp7C1;void*_tmp7C0[1];struct Cyc_String_pa_PrintArg_struct _tmp7BF;(_tmp7BF.tag=0,((_tmp7BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp167)),((_tmp7C0[0]=& _tmp7BF,Cyc_Tcutil_terr(_tmp166->loc,((_tmp7C1="expecting numeric type but found %s",_tag_dyneither(_tmp7C1,sizeof(char),36))),_tag_dyneither(_tmp7C0,sizeof(void*),1)))))));}
return _tmp167;}
# 715
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmp16B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_tmp16B){case 0: _LLCC: {
const char*_tmp7C4;void*_tmp7C3;return(_tmp7C3=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C4="primitive operator has 0 arguments",_tag_dyneither(_tmp7C4,sizeof(char),35))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp7C7;void*_tmp7C6;return(_tmp7C6=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C7="primitive operator has > 2 arguments",_tag_dyneither(_tmp7C7,sizeof(char),37))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}}}
# 723
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 726
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp170=Cyc_Tcutil_compress(t);
void*_tmp171=_tmp170;struct Cyc_Absyn_Aggrdecl*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Datatypefield*_tmp177;void*_tmp179;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_List_List*_tmp17C;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp171;if(_tmp172->tag != 11)goto _LLD4;else{if((((_tmp172->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp173=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp172->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 730
 if(_tmp173->impl == 0){
{const char*_tmp7CA;void*_tmp7C9;(_tmp7C9=0,Cyc_Tcutil_terr(loc,((_tmp7CA="attempt to write an abstract aggregate",_tag_dyneither(_tmp7CA,sizeof(char),39))),_tag_dyneither(_tmp7C9,sizeof(void*),0)));}
return 0;}else{
# 734
_tmp175=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp173->impl))->fields;goto _LLD5;}_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp171;if(_tmp174->tag != 12)goto _LLD6;else{_tmp175=_tmp174->f2;}}_LLD5:
# 736
 for(0;_tmp175 != 0;_tmp175=_tmp175->tl){
struct Cyc_Absyn_Aggrfield*_tmp17F=(struct Cyc_Absyn_Aggrfield*)_tmp175->hd;
if((_tmp17F->tq).real_const){
{const char*_tmp7CE;void*_tmp7CD[1];struct Cyc_String_pa_PrintArg_struct _tmp7CC;(_tmp7CC.tag=0,((_tmp7CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp17F->name),((_tmp7CD[0]=& _tmp7CC,Cyc_Tcutil_terr(loc,((_tmp7CE="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp7CE,sizeof(char),56))),_tag_dyneither(_tmp7CD,sizeof(void*),1)))))));}
return 0;}
# 742
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp17F->type))return 0;}
# 744
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp171;if(_tmp176->tag != 4)goto _LLD8;else{if((((_tmp176->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp177=((struct _tuple2)(((_tmp176->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 746
{struct Cyc_List_List*_tmp183=_tmp177->typs;for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
struct _tuple18*_tmp184=(struct _tuple18*)_tmp183->hd;struct Cyc_Absyn_Tqual _tmp186;void*_tmp187;struct _tuple18*_tmp185=_tmp184;_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;
if(_tmp186.real_const){
{const char*_tmp7D2;void*_tmp7D1[1];struct Cyc_String_pa_PrintArg_struct _tmp7D0;(_tmp7D0.tag=0,((_tmp7D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp177->name)),((_tmp7D1[0]=& _tmp7D0,Cyc_Tcutil_terr(loc,((_tmp7D2="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp7D2,sizeof(char),64))),_tag_dyneither(_tmp7D1,sizeof(void*),1)))))));}
return 0;}
# 752
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp187))return 0;}}
# 754
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp171;if(_tmp178->tag != 8)goto _LLDA;else{_tmp179=(_tmp178->f1).elt_type;_tmp17A=(_tmp178->f1).tq;}}_LLD9:
# 756
 if(_tmp17A.real_const){
{const char*_tmp7D5;void*_tmp7D4;(_tmp7D4=0,Cyc_Tcutil_terr(loc,((_tmp7D5="attempt to over-write a const array",_tag_dyneither(_tmp7D5,sizeof(char),36))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}
return 0;}
# 760
return Cyc_Tcexp_check_writable_aggr(loc,_tmp179);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp171;if(_tmp17B->tag != 10)goto _LLDC;else{_tmp17C=_tmp17B->f1;}}_LLDB:
# 762
 for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
struct _tuple18*_tmp18D=(struct _tuple18*)_tmp17C->hd;struct Cyc_Absyn_Tqual _tmp18F;void*_tmp190;struct _tuple18*_tmp18E=_tmp18D;_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;
if(_tmp18F.real_const){
{const char*_tmp7D8;void*_tmp7D7;(_tmp7D7=0,Cyc_Tcutil_terr(loc,((_tmp7D8="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp7D8,sizeof(char),56))),_tag_dyneither(_tmp7D7,sizeof(void*),0)));}
return 0;}
# 768
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp190))return 0;}
# 770
return 1;_LLDC:;_LLDD:
 return 1;_LLD1:;}
# 778
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 781
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp193=e->r;void*_tmp194=_tmp193;struct Cyc_Absyn_Vardecl*_tmp197;struct Cyc_Absyn_Vardecl*_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct _dyneither_ptr*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct _dyneither_ptr*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp195->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp196=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp195->f2);if(_tmp196->tag != 3)goto _LLE1;else{_tmp197=_tmp196->f1;}}}_LLE0:
 _tmp19A=_tmp197;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp198->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp199=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp198->f2);if(_tmp199->tag != 4)goto _LLE3;else{_tmp19A=_tmp199->f1;}}}_LLE2:
 _tmp19D=_tmp19A;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp19B->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp19B->f2);if(_tmp19C->tag != 5)goto _LLE5;else{_tmp19D=_tmp19C->f1;}}}_LLE4:
 _tmp1A0=_tmp19D;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp19E->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp19F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp19E->f2);if(_tmp19F->tag != 1)goto _LLE7;else{_tmp1A0=_tmp19F->f1;}}}_LLE6:
 if(!(_tmp1A0->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A1->tag != 22)goto _LLE9;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;}}_LLE8:
# 788
{void*_tmp1B0=Cyc_Tcutil_compress((void*)_check_null(_tmp1A2->topt));void*_tmp1B1=_tmp1B0;struct Cyc_Absyn_Tqual _tmp1B3;struct Cyc_Absyn_Tqual _tmp1B5;struct Cyc_List_List*_tmp1B7;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B2->tag != 5)goto _LLF8;else{_tmp1B3=(_tmp1B2->f1).elt_tq;}}_LLF7:
 _tmp1B5=_tmp1B3;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B4->tag != 8)goto _LLFA;else{_tmp1B5=(_tmp1B4->f1).tq;}}_LLF9:
 if(!_tmp1B5.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1B6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B6->tag != 10)goto _LLFC;else{_tmp1B7=_tmp1B6->f1;}}_LLFB: {
# 792
struct _tuple15 _tmp1B8=Cyc_Evexp_eval_const_uint_exp(_tmp1A3);unsigned int _tmp1BA;int _tmp1BB;struct _tuple15 _tmp1B9=_tmp1B8;_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;
if(!_tmp1BB){
{const char*_tmp7DB;void*_tmp7DA;(_tmp7DA=0,Cyc_Tcutil_terr(e->loc,((_tmp7DB="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp7DB,sizeof(char),47))),_tag_dyneither(_tmp7DA,sizeof(void*),0)));}
return;}
# 797
{struct _handler_cons _tmp1BE;_push_handler(& _tmp1BE);{int _tmp1C0=0;if(setjmp(_tmp1BE.handler))_tmp1C0=1;if(!_tmp1C0){
{struct _tuple18*_tmp1C1=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp1B7,(int)_tmp1BA);struct Cyc_Absyn_Tqual _tmp1C3;struct _tuple18*_tmp1C2=_tmp1C1;_tmp1C3=_tmp1C2->f1;
if(!_tmp1C3.real_const){_npop_handler(0);return;}}
# 798
;_pop_handler();}else{void*_tmp1BF=(void*)_exn_thrown;void*_tmp1C5=_tmp1BF;void*_tmp1C7;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1C6=(struct Cyc_List_Nth_exn_struct*)_tmp1C5;if(_tmp1C6->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 800
 return;_LL101: _tmp1C7=_tmp1C5;_LL102:(void)_rethrow(_tmp1C7);_LLFE:;}};}
goto _LLF5;}_LLFC:;_LLFD:
 goto _LLF5;_LLF5:;}
# 804
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A4->tag != 20)goto _LLEB;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LLEA:
# 806
{void*_tmp1C8=Cyc_Tcutil_compress((void*)_check_null(_tmp1A5->topt));void*_tmp1C9=_tmp1C8;struct Cyc_Absyn_Aggrdecl**_tmp1CB;struct Cyc_List_List*_tmp1CD;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C9;if(_tmp1CA->tag != 11)goto _LL106;else{if((((_tmp1CA->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp1CB=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1CA->f1).aggr_info).KnownAggr).val;}}_LL105: {
# 808
struct Cyc_Absyn_Aggrfield*sf=
_tmp1CB == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1CB,_tmp1A6);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1CC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C9;if(_tmp1CC->tag != 12)goto _LL108;else{_tmp1CD=_tmp1CC->f2;}}_LL107: {
# 813
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1CD,_tmp1A6);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL108:;_LL109:
 goto _LL103;_LL103:;}
# 818
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A7->tag != 21)goto _LLED;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LLEC:
# 820
{void*_tmp1CE=Cyc_Tcutil_compress((void*)_check_null(_tmp1A8->topt));void*_tmp1CF=_tmp1CE;void*_tmp1D1;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CF;if(_tmp1D0->tag != 5)goto _LL10D;else{_tmp1D1=(_tmp1D0->f1).elt_typ;}}_LL10C:
# 822
{void*_tmp1D2=Cyc_Tcutil_compress(_tmp1D1);void*_tmp1D3=_tmp1D2;struct Cyc_Absyn_Aggrdecl**_tmp1D5;struct Cyc_List_List*_tmp1D7;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D3;if(_tmp1D4->tag != 11)goto _LL112;else{if((((_tmp1D4->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1D5=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1D4->f1).aggr_info).KnownAggr).val;}}_LL111: {
# 824
struct Cyc_Absyn_Aggrfield*sf=
_tmp1D5 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1D5,_tmp1A9);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1D3;if(_tmp1D6->tag != 12)goto _LL114;else{_tmp1D7=_tmp1D6->f2;}}_LL113: {
# 829
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1D7,_tmp1A9);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL114:;_LL115:
 goto _LL10F;_LL10F:;}
# 834
goto _LL10A;_LL10D:;_LL10E:
 goto _LL10A;_LL10A:;}
# 837
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AA->tag != 19)goto _LLEF;else{_tmp1AB=_tmp1AA->f1;}}_LLEE:
# 839
{void*_tmp1D8=Cyc_Tcutil_compress((void*)_check_null(_tmp1AB->topt));void*_tmp1D9=_tmp1D8;struct Cyc_Absyn_Tqual _tmp1DB;struct Cyc_Absyn_Tqual _tmp1DD;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DA->tag != 5)goto _LL119;else{_tmp1DB=(_tmp1DA->f1).elt_tq;}}_LL118:
 _tmp1DD=_tmp1DB;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DC->tag != 8)goto _LL11B;else{_tmp1DD=(_tmp1DC->f1).tq;}}_LL11A:
 if(!_tmp1DD.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 844
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AC->tag != 11)goto _LLF1;else{_tmp1AD=_tmp1AC->f1;}}_LLF0:
 _tmp1AF=_tmp1AD;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AE->tag != 12)goto _LLF3;else{_tmp1AF=_tmp1AE->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp1AF);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 849
const char*_tmp7DF;void*_tmp7DE[1];struct Cyc_String_pa_PrintArg_struct _tmp7DD;(_tmp7DD.tag=0,((_tmp7DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7DE[0]=& _tmp7DD,Cyc_Tcutil_terr(e->loc,((_tmp7DF="attempt to write a const location: %s",_tag_dyneither(_tmp7DF,sizeof(char),38))),_tag_dyneither(_tmp7DE,sizeof(void*),1)))))));};}
# 852
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 855
struct _RegionHandle _tmp1E1=_new_region("temp");struct _RegionHandle*temp=& _tmp1E1;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 858
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp7E2;void*_tmp7E1;void*_tmp1E4=(_tmp7E1=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E2="increment/decrement of non-lvalue",_tag_dyneither(_tmp7E2,sizeof(char),34))),_tag_dyneither(_tmp7E1,sizeof(void*),0)));_npop_handler(0);return _tmp1E4;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 863
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp7E5;void*_tmp7E4;(_tmp7E4=0,Cyc_Tcutil_terr(e->loc,((_tmp7E5="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7E5,sizeof(char),50))),_tag_dyneither(_tmp7E4,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp7E8;void*_tmp7E7;(_tmp7E7=0,Cyc_Tcutil_terr(e->loc,((_tmp7E8="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7E8,sizeof(char),54))),_tag_dyneither(_tmp7E7,sizeof(void*),0)));}}else{
# 872
const char*_tmp7EC;void*_tmp7EB[1];struct Cyc_String_pa_PrintArg_struct _tmp7EA;(_tmp7EA.tag=0,((_tmp7EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7EB[0]=& _tmp7EA,Cyc_Tcutil_terr(e->loc,((_tmp7EC="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7EC,sizeof(char),44))),_tag_dyneither(_tmp7EB,sizeof(void*),1)))))));}}{
# 874
void*_tmp1EC=t;_npop_handler(0);return _tmp1EC;};};
# 855
;_pop_region(temp);}
# 878
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 880
struct _RegionHandle _tmp1ED=_new_region("r");struct _RegionHandle*r=& _tmp1ED;_push_region(r);{
const char*_tmp7ED;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp7ED="conditional expression",_tag_dyneither(_tmp7ED,sizeof(char),23))));}
# 883
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 889
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp7EE;struct Cyc_List_List _tmp1EF=(_tmp7EE.hd=e3,((_tmp7EE.tl=0,_tmp7EE)));
struct Cyc_List_List _tmp7EF;struct Cyc_List_List _tmp1F0=(_tmp7EF.hd=e2,((_tmp7EF.tl=& _tmp1EF,_tmp7EF)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1F0)){
{const char*_tmp7F4;void*_tmp7F3[2];struct Cyc_String_pa_PrintArg_struct _tmp7F2;struct Cyc_String_pa_PrintArg_struct _tmp7F1;(_tmp7F1.tag=0,((_tmp7F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7F2.tag=0,((_tmp7F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7F3[0]=& _tmp7F2,((_tmp7F3[1]=& _tmp7F1,Cyc_Tcutil_terr(loc,((_tmp7F4="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7F4,sizeof(char),48))),_tag_dyneither(_tmp7F3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 897
void*_tmp1F5=t;_npop_handler(0);return _tmp1F5;};};};
# 880
;_pop_region(r);}
# 901
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 903
{const char*_tmp7F5;Cyc_Tcexp_tcTest(te,e1,((_tmp7F5="logical-and expression",_tag_dyneither(_tmp7F5,sizeof(char),23))));}
{const char*_tmp7F6;Cyc_Tcexp_tcTest(te,e2,((_tmp7F6="logical-and expression",_tag_dyneither(_tmp7F6,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 909
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 911
{const char*_tmp7F7;Cyc_Tcexp_tcTest(te,e1,((_tmp7F7="logical-or expression",_tag_dyneither(_tmp7F7,sizeof(char),22))));}
{const char*_tmp7F8;Cyc_Tcexp_tcTest(te,e2,((_tmp7F8="logical-or expression",_tag_dyneither(_tmp7F8,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 917
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 924
struct _RegionHandle _tmp1FC=_new_region("r");struct _RegionHandle*r=& _tmp1FC;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 927
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 931
{void*_tmp1FD=Cyc_Tcutil_compress(t1);void*_tmp1FE=_tmp1FD;_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1FE;if(_tmp1FF->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp7FB;void*_tmp7FA;(_tmp7FA=0,Cyc_Tcutil_terr(loc,((_tmp7FB="cannot assign to an array",_tag_dyneither(_tmp7FB,sizeof(char),26))),_tag_dyneither(_tmp7FA,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 936
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp7FE;void*_tmp7FD;(_tmp7FD=0,Cyc_Tcutil_terr(loc,((_tmp7FE="type is abstract (can't determine size).",_tag_dyneither(_tmp7FE,sizeof(char),41))),_tag_dyneither(_tmp7FD,sizeof(void*),0)));}
# 940
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp801;void*_tmp800;void*_tmp206=(_tmp800=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp801="assignment to non-lvalue",_tag_dyneither(_tmp801,sizeof(char),25))),_tag_dyneither(_tmp800,sizeof(void*),0)));_npop_handler(0);return _tmp206;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp207=_new_region("temp");struct _RegionHandle*temp=& _tmp207;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp804;void*_tmp803;(_tmp803=0,Cyc_Tcutil_terr(e2->loc,((_tmp804="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp804,sizeof(char),49))),_tag_dyneither(_tmp803,sizeof(void*),0)));}
# 948
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp809;void*_tmp808[2];struct Cyc_String_pa_PrintArg_struct _tmp807;struct Cyc_String_pa_PrintArg_struct _tmp806;void*_tmp20A=(_tmp806.tag=0,((_tmp806.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp807.tag=0,((_tmp807.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp808[0]=& _tmp807,((_tmp808[1]=& _tmp806,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp809="type mismatch: %s != %s",_tag_dyneither(_tmp809,sizeof(char),24))),_tag_dyneither(_tmp808,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp20B=_tmp20A;_npop_handler(1);return _tmp20B;};}
# 944
;_pop_region(temp);}else{
# 956
enum Cyc_Absyn_Primop _tmp210=(enum Cyc_Absyn_Primop)po->v;
void*_tmp211=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp210,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp211,t1) || Cyc_Tcutil_coerceable(_tmp211) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp80E;void*_tmp80D[2];struct Cyc_String_pa_PrintArg_struct _tmp80C;struct Cyc_String_pa_PrintArg_struct _tmp80B;void*_tmp212=
(_tmp80B.tag=0,((_tmp80B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 962
Cyc_Absynpp_typ2string(t2)),((_tmp80C.tag=0,((_tmp80C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 961
Cyc_Absynpp_typ2string(t1)),((_tmp80D[0]=& _tmp80C,((_tmp80D[1]=& _tmp80B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp80E="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp80E,sizeof(char),82))),_tag_dyneither(_tmp80D,sizeof(void*),2)))))))))))));
# 963
Cyc_Tcutil_unify(_tmp211,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp213=_tmp212;_npop_handler(0);return _tmp213;};}{
# 967
void*_tmp218=_tmp211;_npop_handler(0);return _tmp218;};}{
# 969
void*_tmp219=t1;_npop_handler(0);return _tmp219;};};};
# 924
;_pop_region(r);}
# 973
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _RegionHandle _tmp21A=_new_region("r");struct _RegionHandle*r=& _tmp21A;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);{
# 978
void*_tmp21B=(void*)_check_null(e2->topt);_npop_handler(0);return _tmp21B;};
# 974
;_pop_region(r);}
# 982
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 986
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 989
{void*_tmp21C=Cyc_Tcutil_compress(t1);void*_tmp21D=_tmp21C;_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp21D;if(_tmp21E->tag != 7)goto _LL125;else{if(_tmp21E->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21D;if(_tmp21F->tag != 6)goto _LL127;else{if(_tmp21F->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21D;if(_tmp220->tag != 6)goto _LL129;else{if(_tmp220->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 996
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp221=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp223;struct Cyc_List_List*_tmp224;unsigned int _tmp225;enum Cyc_Absyn_Scope _tmp226;struct Cyc_Absyn_Datatypefield _tmp222=_tmp221;_tmp223=_tmp222.name;_tmp224=_tmp222.typs;_tmp225=_tmp222.loc;_tmp226=_tmp222.sc;
# 999
if(_tmp224 == 0  || _tmp224->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp224->hd)).f2);
# 1002
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1007
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp227=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp229;struct Cyc_List_List*_tmp22A;unsigned int _tmp22B;enum Cyc_Absyn_Scope _tmp22C;struct Cyc_Absyn_Datatypefield _tmp228=_tmp227;_tmp229=_tmp228.name;_tmp22A=_tmp228.typs;_tmp22B=_tmp228.loc;_tmp22C=_tmp228.sc;
# 1010
if(_tmp22A == 0  || _tmp22A->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp22A->hd)).f2);
# 1014
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1019
{const char*_tmp813;void*_tmp812[2];struct Cyc_String_pa_PrintArg_struct _tmp811;struct Cyc_String_pa_PrintArg_struct _tmp810;(_tmp810.tag=0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp811.tag=0,((_tmp811.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp812[0]=& _tmp811,((_tmp812[1]=& _tmp810,Cyc_Tcutil_terr(e->loc,((_tmp813="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp813,sizeof(char),54))),_tag_dyneither(_tmp812,sizeof(void*),2)))))))))))));}
return 0;}
# 1025
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1031
struct Cyc_List_List*_tmp231=args;
int _tmp232=0;
struct _RegionHandle _tmp233=_new_region("ter");struct _RegionHandle*ter=& _tmp233;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp234=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp235=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp234);
Cyc_Tcexp_tcExp(_tmp235,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1041
void*_tmp236=t;void*_tmp238;struct Cyc_Absyn_Tqual _tmp239;void*_tmp23A;union Cyc_Absyn_Constraint*_tmp23B;union Cyc_Absyn_Constraint*_tmp23C;union Cyc_Absyn_Constraint*_tmp23D;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236;if(_tmp237->tag != 5)goto _LL12E;else{_tmp238=(_tmp237->f1).elt_typ;_tmp239=(_tmp237->f1).elt_tq;_tmp23A=((_tmp237->f1).ptr_atts).rgn;_tmp23B=((_tmp237->f1).ptr_atts).nullable;_tmp23C=((_tmp237->f1).ptr_atts).bounds;_tmp23D=((_tmp237->f1).ptr_atts).zero_term;}}_LL12D:
# 1046
 Cyc_Tcenv_check_rgn_accessible(_tmp235,loc,_tmp23A);
# 1048
Cyc_Tcutil_check_nonzero_bound(loc,_tmp23C);{
void*_tmp23E=Cyc_Tcutil_compress(_tmp238);void*_tmp23F=_tmp23E;struct Cyc_List_List*_tmp241;void*_tmp242;struct Cyc_Absyn_Tqual _tmp243;void*_tmp244;struct Cyc_List_List*_tmp245;int _tmp246;struct Cyc_Absyn_VarargInfo*_tmp247;struct Cyc_List_List*_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_List_List*_tmp24B;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_List_List*_tmp24D;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23F;if(_tmp240->tag != 9)goto _LL133;else{_tmp241=(_tmp240->f1).tvars;_tmp242=(_tmp240->f1).effect;_tmp243=(_tmp240->f1).ret_tqual;_tmp244=(_tmp240->f1).ret_typ;_tmp245=(_tmp240->f1).args;_tmp246=(_tmp240->f1).c_varargs;_tmp247=(_tmp240->f1).cyc_varargs;_tmp248=(_tmp240->f1).rgn_po;_tmp249=(_tmp240->f1).attributes;_tmp24A=(_tmp240->f1).requires_clause;_tmp24B=(_tmp240->f1).requires_relns;_tmp24C=(_tmp240->f1).ensures_clause;_tmp24D=(_tmp240->f1).ensures_relns;}}_LL132:
# 1053
 if(_tmp241 != 0  || _tmp248 != 0){
const char*_tmp816;void*_tmp815;(_tmp815=0,Cyc_Tcutil_terr(e->loc,((_tmp816="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp816,sizeof(char),79))),_tag_dyneither(_tmp815,sizeof(void*),0)));}
# 1057
if(topt != 0)Cyc_Tcutil_unify(_tmp244,*topt);
# 1059
while(_tmp231 != 0  && _tmp245 != 0){
# 1061
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
void*t2=(*((struct _tuple9*)_tmp245->hd)).f3;
Cyc_Tcexp_tcExp(_tmp235,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp235,e1,t2,& alias_coercion)){
const char*_tmp817;struct _dyneither_ptr s0=(_tmp817="actual argument has type ",_tag_dyneither(_tmp817,sizeof(char),26));
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
const char*_tmp818;struct _dyneither_ptr s2=(_tmp818=" but formal has type ",_tag_dyneither(_tmp818,sizeof(char),22));
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70){
struct Cyc_String_pa_PrintArg_struct _tmp826;struct Cyc_String_pa_PrintArg_struct _tmp825;struct Cyc_String_pa_PrintArg_struct _tmp824;struct Cyc_String_pa_PrintArg_struct _tmp823;void*_tmp822[4];const char*_tmp821;void*_tmp820;(_tmp820=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp826.tag=0,((_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp825.tag=0,((_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp824.tag=0,((_tmp824.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp823.tag=0,((_tmp823.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp822[0]=& _tmp823,((_tmp822[1]=& _tmp824,((_tmp822[2]=& _tmp825,((_tmp822[3]=& _tmp826,Cyc_aprintf(((_tmp821="%s\n\t%s\n%s\n\t%s.",_tag_dyneither(_tmp821,sizeof(char),15))),_tag_dyneither(_tmp822,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp820,sizeof(void*),0)));}else{
# 1073
struct Cyc_String_pa_PrintArg_struct _tmp834;struct Cyc_String_pa_PrintArg_struct _tmp833;struct Cyc_String_pa_PrintArg_struct _tmp832;struct Cyc_String_pa_PrintArg_struct _tmp831;void*_tmp830[4];const char*_tmp82F;void*_tmp82E;(_tmp82E=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp834.tag=0,((_tmp834.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp833.tag=0,((_tmp833.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp832.tag=0,((_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp831.tag=0,((_tmp831.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp830[0]=& _tmp831,((_tmp830[1]=& _tmp832,((_tmp830[2]=& _tmp833,((_tmp830[3]=& _tmp834,Cyc_aprintf(((_tmp82F="%s%s%s%s.",_tag_dyneither(_tmp82F,sizeof(char),10))),_tag_dyneither(_tmp830,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp82E,sizeof(void*),0)));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1078
if(alias_coercion){
struct Cyc_List_List*_tmp835;*alias_arg_exps=((_tmp835=_cycalloc(sizeof(*_tmp835)),((_tmp835->hd=(void*)_tmp232,((_tmp835->tl=*alias_arg_exps,_tmp835))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp838;void*_tmp837;(_tmp837=0,Cyc_Tcutil_terr(e1->loc,((_tmp838="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp838,sizeof(char),49))),_tag_dyneither(_tmp837,sizeof(void*),0)));}
_tmp231=_tmp231->tl;
_tmp245=_tmp245->tl;
++ _tmp232;}{
# 1089
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp249;for(0;a != 0;a=a->tl){
void*_tmp263=(void*)a->hd;void*_tmp264=_tmp263;enum Cyc_Absyn_Format_Type _tmp266;int _tmp267;int _tmp268;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp265=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp264;if(_tmp265->tag != 19)goto _LL138;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;_tmp268=_tmp265->f3;}}_LL137:
# 1093
{struct _handler_cons _tmp269;_push_handler(& _tmp269);{int _tmp26B=0;if(setjmp(_tmp269.handler))_tmp26B=1;if(!_tmp26B){
# 1095
{struct Cyc_Absyn_Exp*_tmp26C=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp267 - 1);
# 1097
struct Cyc_Core_Opt*fmt_args;
if(_tmp268 == 0)
fmt_args=0;else{
# 1101
struct Cyc_Core_Opt*_tmp839;fmt_args=((_tmp839=_cycalloc(sizeof(*_tmp839)),((_tmp839->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp268 - 1),_tmp839))));}
args_already_checked=1;{
struct _RegionHandle _tmp26E=_new_region("temp");struct _RegionHandle*temp=& _tmp26E;_push_region(temp);
switch(_tmp266){case Cyc_Absyn_Printf_ft: _LL13A:
# 1106
 Cyc_Tcexp_check_format_args(_tmp235,_tmp26C,fmt_args,_tmp268 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1109
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1111
 Cyc_Tcexp_check_format_args(_tmp235,_tmp26C,fmt_args,_tmp268 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1114
break;}
# 1104
;_pop_region(temp);};}
# 1095
;_pop_handler();}else{void*_tmp26A=(void*)_exn_thrown;void*_tmp270=_tmp26A;void*_tmp272;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp271=(struct Cyc_List_Nth_exn_struct*)_tmp270;if(_tmp271->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1118
{const char*_tmp83C;void*_tmp83B;(_tmp83B=0,Cyc_Tcutil_terr(loc,((_tmp83C="bad format arguments",_tag_dyneither(_tmp83C,sizeof(char),21))),_tag_dyneither(_tmp83B,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp272=_tmp270;_LL141:(void)_rethrow(_tmp272);_LL13D:;}};}
# 1120
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1124
if(_tmp245 != 0){const char*_tmp83F;void*_tmp83E;(_tmp83E=0,Cyc_Tcutil_terr(loc,((_tmp83F="too few arguments for function",_tag_dyneither(_tmp83F,sizeof(char),31))),_tag_dyneither(_tmp83E,sizeof(void*),0)));}else{
# 1126
if((_tmp231 != 0  || _tmp246) || _tmp247 != 0){
if(_tmp246)
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
Cyc_Tcexp_tcExp(_tmp235,0,(struct Cyc_Absyn_Exp*)_tmp231->hd);}else{
if(_tmp247 == 0){
const char*_tmp842;void*_tmp841;(_tmp841=0,Cyc_Tcutil_terr(loc,((_tmp842="too many arguments for function",_tag_dyneither(_tmp842,sizeof(char),32))),_tag_dyneither(_tmp841,sizeof(void*),0)));}else{
# 1133
struct Cyc_Absyn_VarargInfo _tmp279=*_tmp247;void*_tmp27B;int _tmp27C;struct Cyc_Absyn_VarargInfo _tmp27A=_tmp279;_tmp27B=_tmp27A.type;_tmp27C=_tmp27A.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp843;struct Cyc_Absyn_VarargCallInfo*_tmp27D=(_tmp843=_cycalloc(sizeof(*_tmp843)),((_tmp843->num_varargs=0,((_tmp843->injectors=0,((_tmp843->vai=_tmp247,_tmp843)))))));
# 1137
*vararg_call_info=_tmp27D;
# 1139
if(!_tmp27C)
# 1141
for(0;_tmp231 != 0;(_tmp231=_tmp231->tl,_tmp232 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
++ _tmp27D->num_varargs;
Cyc_Tcexp_tcExp(_tmp235,& _tmp27B,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp235,e1,_tmp27B,& alias_coercion)){
{const char*_tmp848;void*_tmp847[2];struct Cyc_String_pa_PrintArg_struct _tmp846;struct Cyc_String_pa_PrintArg_struct _tmp845;(_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp846.tag=0,((_tmp846.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp27B)),((_tmp847[0]=& _tmp846,((_tmp847[1]=& _tmp845,Cyc_Tcutil_terr(loc,((_tmp848="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp848,sizeof(char),49))),_tag_dyneither(_tmp847,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1151
if(alias_coercion){
struct Cyc_List_List*_tmp849;*alias_arg_exps=((_tmp849=_cycalloc(sizeof(*_tmp849)),((_tmp849->hd=(void*)_tmp232,((_tmp849->tl=*alias_arg_exps,_tmp849))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp27B) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp84C;void*_tmp84B;(_tmp84B=0,Cyc_Tcutil_terr(e1->loc,((_tmp84C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp84C,sizeof(char),49))),_tag_dyneither(_tmp84B,sizeof(void*),0)));}}else{
# 1160
void*_tmp285=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp27B));void*_tmp286=_tmp285;struct Cyc_Absyn_Datatypedecl*_tmp288;struct Cyc_List_List*_tmp289;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp287=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp286;if(_tmp287->tag != 3)goto _LL145;else{if((((_tmp287->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp288=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp287->f1).datatype_info).KnownDatatype).val);_tmp289=(_tmp287->f1).targs;}}_LL144: {
# 1164
struct Cyc_Absyn_Datatypedecl*_tmp28A=*Cyc_Tcenv_lookup_datatypedecl(_tmp235,loc,_tmp288->name);
struct Cyc_List_List*fields=0;
if(_tmp28A->fields == 0){
const char*_tmp850;void*_tmp84F[1];struct Cyc_String_pa_PrintArg_struct _tmp84E;(_tmp84E.tag=0,((_tmp84E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp27B)),((_tmp84F[0]=& _tmp84E,Cyc_Tcutil_terr(loc,((_tmp850="can't inject into abstract datatype %s",_tag_dyneither(_tmp850,sizeof(char),39))),_tag_dyneither(_tmp84F,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp28A->fields))->v;}
# 1176
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp27B),Cyc_Tcenv_curr_rgn(_tmp235))){
const char*_tmp855;void*_tmp854[2];struct Cyc_String_pa_PrintArg_struct _tmp853;struct Cyc_String_pa_PrintArg_struct _tmp852;(_tmp852.tag=0,((_tmp852.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp235))),((_tmp853.tag=0,((_tmp853.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp27B)),((_tmp854[0]=& _tmp853,((_tmp854[1]=& _tmp852,Cyc_Tcutil_terr(loc,((_tmp855="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp855,sizeof(char),49))),_tag_dyneither(_tmp854,sizeof(void*),2)))))))))))));}{
# 1180
struct _RegionHandle _tmp292=_new_region("rgn");struct _RegionHandle*rgn=& _tmp292;_push_region(rgn);{
struct Cyc_List_List*_tmp293=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp28A->tvs,_tmp289);
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
++ _tmp27D->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
# 1186
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp235,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp858;void*_tmp857;(_tmp857=0,Cyc_Tcutil_terr(e1->loc,((_tmp858="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp858,sizeof(char),49))),_tag_dyneither(_tmp857,sizeof(void*),0)));}}{
# 1192
struct Cyc_Absyn_Datatypefield*_tmp296=Cyc_Tcexp_tcInjection(_tmp235,e1,Cyc_Tcutil_pointer_elt_type(_tmp27B),rgn,_tmp293,fields);
# 1194
if(_tmp296 != 0){
struct Cyc_List_List*_tmp859;_tmp27D->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp27D->injectors,(
(_tmp859=_cycalloc(sizeof(*_tmp859)),((_tmp859->hd=_tmp296,((_tmp859->tl=0,_tmp859)))))));}};};}}
# 1200
_npop_handler(0);goto _LL142;
# 1180
;_pop_region(rgn);};}_LL145:;_LL146:
# 1201
{const char*_tmp85C;void*_tmp85B;(_tmp85B=0,Cyc_Tcutil_terr(loc,((_tmp85C="bad inject vararg type",_tag_dyneither(_tmp85C,sizeof(char),23))),_tag_dyneither(_tmp85B,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1206
if(*alias_arg_exps == 0)
# 1215 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp235,loc,(void*)_check_null(_tmp242));{
# 1217
void*_tmp29B=_tmp244;_npop_handler(0);return _tmp29B;};};_LL133:;_LL134: {
const char*_tmp860;void*_tmp85F[1];struct Cyc_String_pa_PrintArg_struct _tmp85E;void*_tmp29F=(_tmp85E.tag=0,((_tmp85E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp85F[0]=& _tmp85E,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp860="expected pointer to function but found %s",_tag_dyneither(_tmp860,sizeof(char),42))),_tag_dyneither(_tmp85F,sizeof(void*),1)))))));_npop_handler(0);return _tmp29F;}_LL130:;};_LL12E:;_LL12F: {
# 1220
const char*_tmp864;void*_tmp863[1];struct Cyc_String_pa_PrintArg_struct _tmp862;void*_tmp2A3=(_tmp862.tag=0,((_tmp862.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp863[0]=& _tmp862,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp864="expected pointer to function but found %s",_tag_dyneither(_tmp864,sizeof(char),42))),_tag_dyneither(_tmp863,sizeof(void*),1)))))));_npop_handler(0);return _tmp2A3;}_LL12B:;};}
# 1034 "tcexp.cyc"
;_pop_region(ter);}
# 1226 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp2A4=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp2A4,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp2A4,& bogus)){
const char*_tmp869;void*_tmp868[2];struct Cyc_String_pa_PrintArg_struct _tmp867;struct Cyc_String_pa_PrintArg_struct _tmp866;(_tmp866.tag=0,((_tmp866.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp867.tag=0,((_tmp867.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1231
Cyc_Absynpp_typ2string(_tmp2A4)),((_tmp868[0]=& _tmp867,((_tmp868[1]=& _tmp866,Cyc_Tcutil_terr(loc,((_tmp869="expected %s but found %s",_tag_dyneither(_tmp869,sizeof(char),25))),_tag_dyneither(_tmp868,sizeof(void*),2)))))))))))));}
# 1233
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1238
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1240
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1242
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp2A9=t1;void*_tmp2AB;struct Cyc_Absyn_Tqual _tmp2AC;void*_tmp2AD;union Cyc_Absyn_Constraint*_tmp2AE;union Cyc_Absyn_Constraint*_tmp2AF;union Cyc_Absyn_Constraint*_tmp2B0;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2AA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A9;if(_tmp2AA->tag != 5)goto _LL14A;else{_tmp2AB=(_tmp2AA->f1).elt_typ;_tmp2AC=(_tmp2AA->f1).elt_tq;_tmp2AD=((_tmp2AA->f1).ptr_atts).rgn;_tmp2AE=((_tmp2AA->f1).ptr_atts).nullable;_tmp2AF=((_tmp2AA->f1).ptr_atts).bounds;_tmp2B0=((_tmp2AA->f1).ptr_atts).zero_term;}}_LL149:
# 1246
{void*_tmp2B1=Cyc_Tcutil_compress(_tmp2AB);void*_tmp2B2=_tmp2B1;struct Cyc_List_List*_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Tqual _tmp2B6;void*_tmp2B7;struct Cyc_List_List*_tmp2B8;int _tmp2B9;struct Cyc_Absyn_VarargInfo*_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_List_List*_tmp2C0;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2B3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B2;if(_tmp2B3->tag != 9)goto _LL14F;else{_tmp2B4=(_tmp2B3->f1).tvars;_tmp2B5=(_tmp2B3->f1).effect;_tmp2B6=(_tmp2B3->f1).ret_tqual;_tmp2B7=(_tmp2B3->f1).ret_typ;_tmp2B8=(_tmp2B3->f1).args;_tmp2B9=(_tmp2B3->f1).c_varargs;_tmp2BA=(_tmp2B3->f1).cyc_varargs;_tmp2BB=(_tmp2B3->f1).rgn_po;_tmp2BC=(_tmp2B3->f1).attributes;_tmp2BD=(_tmp2B3->f1).requires_clause;_tmp2BE=(_tmp2B3->f1).requires_relns;_tmp2BF=(_tmp2B3->f1).ensures_clause;_tmp2C0=(_tmp2B3->f1).ensures_relns;}}_LL14E: {
# 1248
struct _RegionHandle _tmp2C1=_new_region("temp");struct _RegionHandle*temp=& _tmp2C1;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1251
for(0;ts != 0  && _tmp2B4 != 0;(ts=ts->tl,_tmp2B4=_tmp2B4->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2B4->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp86C;struct Cyc_List_List*_tmp86B;instantiation=(
(_tmp86B=_region_malloc(temp,sizeof(*_tmp86B)),((_tmp86B->hd=((_tmp86C=_region_malloc(temp,sizeof(*_tmp86C)),((_tmp86C->f1=(struct Cyc_Absyn_Tvar*)_tmp2B4->hd,((_tmp86C->f2=(void*)ts->hd,_tmp86C)))))),((_tmp86B->tl=instantiation,_tmp86B))))));};}
# 1258
if(ts != 0){
const char*_tmp86F;void*_tmp86E;void*_tmp2C6=
(_tmp86E=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86F="too many type variables in instantiation",_tag_dyneither(_tmp86F,sizeof(char),41))),_tag_dyneither(_tmp86E,sizeof(void*),0)));_npop_handler(0);return _tmp2C6;}
# 1264
if(_tmp2B4 == 0){
_tmp2BB=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp2BB);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp2BB);
_tmp2BB=0;}{
# 1269
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp875;struct Cyc_Absyn_FnInfo _tmp874;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp873;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp873=_cycalloc(sizeof(*_tmp873)),((_tmp873[0]=((_tmp875.tag=9,((_tmp875.f1=((_tmp874.tvars=_tmp2B4,((_tmp874.effect=_tmp2B5,((_tmp874.ret_tqual=_tmp2B6,((_tmp874.ret_typ=_tmp2B7,((_tmp874.args=_tmp2B8,((_tmp874.c_varargs=_tmp2B9,((_tmp874.cyc_varargs=_tmp2BA,((_tmp874.rgn_po=_tmp2BB,((_tmp874.attributes=_tmp2BC,((_tmp874.requires_clause=_tmp2BD,((_tmp874.requires_relns=_tmp2BE,((_tmp874.ensures_clause=_tmp2BF,((_tmp874.ensures_relns=_tmp2C0,_tmp874)))))))))))))))))))))))))),_tmp875)))),_tmp873)))));
# 1274
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp87B;struct Cyc_Absyn_PtrInfo _tmp87A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp879;void*_tmp2CA=(void*)((_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879[0]=((_tmp87B.tag=5,((_tmp87B.f1=((_tmp87A.elt_typ=new_fn_typ,((_tmp87A.elt_tq=_tmp2AC,((_tmp87A.ptr_atts=(((_tmp87A.ptr_atts).rgn=_tmp2AD,(((_tmp87A.ptr_atts).nullable=_tmp2AE,(((_tmp87A.ptr_atts).bounds=_tmp2AF,(((_tmp87A.ptr_atts).zero_term=_tmp2B0,(((_tmp87A.ptr_atts).ptrloc=0,_tmp87A.ptr_atts)))))))))),_tmp87A)))))),_tmp87B)))),_tmp879))));_npop_handler(0);return _tmp2CA;};}
# 1249
;_pop_region(temp);}_LL14F:;_LL150:
# 1276
 goto _LL14C;_LL14C:;}
# 1278
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1281
const char*_tmp87F;void*_tmp87E[1];struct Cyc_String_pa_PrintArg_struct _tmp87D;return(_tmp87D.tag=0,((_tmp87D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp87E[0]=& _tmp87D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87F="expecting polymorphic type but found %s",_tag_dyneither(_tmp87F,sizeof(char),40))),_tag_dyneither(_tmp87E,sizeof(void*),1)))))));};};}
# 1286
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1288
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk,1,0,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1292
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1297
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1304
Cyc_Tcutil_unify(t2,t);{
const char*_tmp884;void*_tmp883[2];struct Cyc_String_pa_PrintArg_struct _tmp882;struct Cyc_String_pa_PrintArg_struct _tmp881;void*_tmp2D1=(_tmp881.tag=0,((_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp882.tag=0,((_tmp882.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp883[0]=& _tmp882,((_tmp883[1]=& _tmp881,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp884="cannot cast %s to %s",_tag_dyneither(_tmp884,sizeof(char),21))),_tag_dyneither(_tmp883,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp2D1;};}}}
# 1314
{struct _tuple0 _tmp885;struct _tuple0 _tmp2D6=(_tmp885.f1=e->r,((_tmp885.f2=Cyc_Tcutil_compress(t),_tmp885)));struct _tuple0 _tmp2D7=_tmp2D6;int _tmp2D9;union Cyc_Absyn_Constraint*_tmp2DB;union Cyc_Absyn_Constraint*_tmp2DC;union Cyc_Absyn_Constraint*_tmp2DD;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2D8=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D7.f1;if(_tmp2D8->tag != 33)goto _LL154;else{_tmp2D9=(_tmp2D8->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7.f2;if(_tmp2DA->tag != 5)goto _LL154;else{_tmp2DB=((_tmp2DA->f1).ptr_atts).nullable;_tmp2DC=((_tmp2DA->f1).ptr_atts).bounds;_tmp2DD=((_tmp2DA->f1).ptr_atts).zero_term;}};_LL153:
# 1318
 if((_tmp2D9  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DD)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DB)){
void*_tmp2DE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2DC);void*_tmp2DF=_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E1;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2E0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2DF;if(_tmp2E0->tag != 1)goto _LL159;else{_tmp2E1=_tmp2E0->f1;}}_LL158:
# 1321
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2E1)).f1 == 1){
const char*_tmp888;void*_tmp887;(_tmp887=0,Cyc_Tcutil_warn(loc,((_tmp888="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp888,sizeof(char),60))),_tag_dyneither(_tmp887,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1327
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1330
return t;};}
# 1334
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1336
void**_tmp2E5=0;
struct Cyc_Absyn_Tqual _tmp2E6=Cyc_Absyn_empty_tqual(0);
int _tmp2E7=0;
if(topt != 0){
void*_tmp2E8=Cyc_Tcutil_compress(*topt);void*_tmp2E9=_tmp2E8;void**_tmp2EB;struct Cyc_Absyn_Tqual _tmp2EC;union Cyc_Absyn_Constraint*_tmp2ED;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9;if(_tmp2EA->tag != 5)goto _LL15E;else{_tmp2EB=(void**)&(_tmp2EA->f1).elt_typ;_tmp2EC=(_tmp2EA->f1).elt_tq;_tmp2ED=((_tmp2EA->f1).ptr_atts).nullable;}}_LL15D:
# 1342
 _tmp2E5=_tmp2EB;
_tmp2E6=_tmp2EC;
_tmp2E7=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2ED);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1356
struct _RegionHandle _tmp2EE=_new_region("r");struct _RegionHandle*r=& _tmp2EE;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp2E5,e);
# 1360
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp88B;void*_tmp88A;(_tmp88A=0,Cyc_Tcutil_terr(e->loc,((_tmp88B="Cannot take the address of an alias-free path",_tag_dyneither(_tmp88B,sizeof(char),46))),_tag_dyneither(_tmp88A,sizeof(void*),0)));}
# 1367
{void*_tmp2F1=e->r;void*_tmp2F2=_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2F3=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2F3->tag != 22)goto _LL163;else{_tmp2F4=_tmp2F3->f1;_tmp2F5=_tmp2F3->f2;}}_LL162:
# 1369
{void*_tmp2F6=Cyc_Tcutil_compress((void*)_check_null(_tmp2F4->topt));void*_tmp2F7=_tmp2F6;_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F7;if(_tmp2F8->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1374
 e0->r=(Cyc_Absyn_add_exp(_tmp2F4,_tmp2F5,0))->r;{
void*_tmp2F9=Cyc_Tcexp_tcPlus(te,_tmp2F4,_tmp2F5);_npop_handler(0);return _tmp2F9;};_LL165:;}
# 1377
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1382
{void*_tmp2FA=e->r;void*_tmp2FB=_tmp2FA;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2FC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FB;if(_tmp2FC->tag != 20)goto _LL16D;else{if(_tmp2FC->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2FD=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FB;if(_tmp2FD->tag != 21)goto _LL16F;else{if(_tmp2FD->f3 != 1)goto _LL16F;}}_LL16E:
# 1385
{const char*_tmp88E;void*_tmp88D;(_tmp88D=0,Cyc_Tcutil_terr(e->loc,((_tmp88E="cannot take the address of a @tagged union member",_tag_dyneither(_tmp88E,sizeof(char),50))),_tag_dyneither(_tmp88D,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1391
struct _tuple14 _tmp300=Cyc_Tcutil_addressof_props(te,e);int _tmp302;void*_tmp303;struct _tuple14 _tmp301=_tmp300;_tmp302=_tmp301.f1;_tmp303=_tmp301.f2;
# 1393
if(Cyc_Tcutil_is_noalias_region(_tmp303,0)){
const char*_tmp891;void*_tmp890;(_tmp890=0,Cyc_Tcutil_terr(e->loc,((_tmp891="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp891,sizeof(char),60))),_tag_dyneither(_tmp890,sizeof(void*),0)));}{
# 1396
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp302){
tq.print_const=1;
tq.real_const=1;}{
# 1402
void*t=_tmp2E7?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp303,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp303,tq,Cyc_Absyn_false_conref);
void*_tmp306=t;_npop_handler(0);return _tmp306;};};};
# 1356
;_pop_region(r);};}
# 1409
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1413
return Cyc_Absyn_uint_typ;
# 1415
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp895;void*_tmp894[1];struct Cyc_String_pa_PrintArg_struct _tmp893;(_tmp893.tag=0,((_tmp893.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp894[0]=& _tmp893,Cyc_Tcutil_terr(loc,((_tmp895="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp895,sizeof(char),55))),_tag_dyneither(_tmp894,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp30A=Cyc_Tcutil_compress(*topt);void*_tmp30B=_tmp30A;void*_tmp30C;void*_tmp30E;_LL172: _tmp30C=_tmp30B;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp30D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp30C;if(_tmp30D->tag != 19)goto _LL174;else{_tmp30E=(void*)_tmp30D->f1;}};_LL173: {
# 1424
struct Cyc_Absyn_Exp*_tmp30F=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp898;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp897;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp310=(_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897[0]=((_tmp898.tag=18,((_tmp898.f1=_tmp30F,_tmp898)))),_tmp897)));
if(Cyc_Tcutil_unify(_tmp30E,(void*)_tmp310))return _tmp30C;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1430
return Cyc_Absyn_uint_typ;}
# 1433
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1440
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1442
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp313=fs;
void*_tmp314=t_orig;
for(0;_tmp313 != 0;_tmp313=_tmp313->tl){
void*_tmp315=(void*)_tmp313->hd;
void*_tmp316=_tmp315;struct _dyneither_ptr*_tmp318;unsigned int _tmp31A;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp317=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp316;if(_tmp317->tag != 0)goto _LL179;else{_tmp318=_tmp317->f1;}}_LL178: {
# 1450
int bad_type=1;
{void*_tmp31B=Cyc_Tcutil_compress(_tmp314);void*_tmp31C=_tmp31B;struct Cyc_Absyn_Aggrdecl**_tmp31E;struct Cyc_List_List*_tmp320;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp31D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31C;if(_tmp31D->tag != 11)goto _LL17E;else{if((((_tmp31D->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp31E=(struct Cyc_Absyn_Aggrdecl**)(((_tmp31D->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1453
 if((*_tmp31E)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp318,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp31E)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp89C;void*_tmp89B[1];struct Cyc_String_pa_PrintArg_struct _tmp89A;(_tmp89A.tag=0,((_tmp89A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp318),((_tmp89B[0]=& _tmp89A,Cyc_Tcutil_terr(loc,((_tmp89C="no field of struct/union has name %s",_tag_dyneither(_tmp89C,sizeof(char),37))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))));}else{
# 1458
_tmp314=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31C;if(_tmp31F->tag != 12)goto _LL180;else{_tmp320=_tmp31F->f2;}}_LL17F: {
# 1462
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp318,_tmp320);
if(!((unsigned int)t2)){
const char*_tmp8A0;void*_tmp89F[1];struct Cyc_String_pa_PrintArg_struct _tmp89E;(_tmp89E.tag=0,((_tmp89E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp318),((_tmp89F[0]=& _tmp89E,Cyc_Tcutil_terr(loc,((_tmp8A0="no field of struct/union has name %s",_tag_dyneither(_tmp8A0,sizeof(char),37))),_tag_dyneither(_tmp89F,sizeof(void*),1)))))));}else{
# 1466
_tmp314=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1471
if(bad_type){
if(_tmp313 == fs){
const char*_tmp8A4;void*_tmp8A3[1];struct Cyc_String_pa_PrintArg_struct _tmp8A2;(_tmp8A2.tag=0,((_tmp8A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp8A3[0]=& _tmp8A2,Cyc_Tcutil_terr(loc,((_tmp8A4="%s is not a known struct/union type",_tag_dyneither(_tmp8A4,sizeof(char),36))),_tag_dyneither(_tmp8A3,sizeof(void*),1)))))));}else{
# 1475
const char*_tmp8A8;void*_tmp8A7[1];struct Cyc_String_pa_PrintArg_struct _tmp8A6;struct _dyneither_ptr _tmp32A=(_tmp8A6.tag=0,((_tmp8A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp8A7[0]=& _tmp8A6,Cyc_aprintf(((_tmp8A8="(%s)",_tag_dyneither(_tmp8A8,sizeof(char),5))),_tag_dyneither(_tmp8A7,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp313;x=x->tl){
void*_tmp32B=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp32C=_tmp32B;struct _dyneither_ptr*_tmp32E;unsigned int _tmp330;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp32D=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp32C;if(_tmp32D->tag != 0)goto _LL185;else{_tmp32E=_tmp32D->f1;}}_LL184:
# 1480
{const char*_tmp8AD;void*_tmp8AC[2];struct Cyc_String_pa_PrintArg_struct _tmp8AB;struct Cyc_String_pa_PrintArg_struct _tmp8AA;_tmp32A=((_tmp8AA.tag=0,((_tmp8AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp32E),((_tmp8AB.tag=0,((_tmp8AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp8AC[0]=& _tmp8AB,((_tmp8AC[1]=& _tmp8AA,Cyc_aprintf(((_tmp8AD="%s.%s",_tag_dyneither(_tmp8AD,sizeof(char),6))),_tag_dyneither(_tmp8AC,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp32F=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp32C;if(_tmp32F->tag != 1)goto _LL182;else{_tmp330=_tmp32F->f1;}}_LL186:
# 1482
{const char*_tmp8B2;void*_tmp8B1[2];struct Cyc_String_pa_PrintArg_struct _tmp8B0;struct Cyc_Int_pa_PrintArg_struct _tmp8AF;_tmp32A=((_tmp8AF.tag=1,((_tmp8AF.f1=(unsigned long)((int)_tmp330),((_tmp8B0.tag=0,((_tmp8B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp8B1[0]=& _tmp8B0,((_tmp8B1[1]=& _tmp8AF,Cyc_aprintf(((_tmp8B2="%s.%d",_tag_dyneither(_tmp8B2,sizeof(char),6))),_tag_dyneither(_tmp8B1,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1484
const char*_tmp8B7;void*_tmp8B6[2];struct Cyc_String_pa_PrintArg_struct _tmp8B5;struct Cyc_String_pa_PrintArg_struct _tmp8B4;(_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp8B6[0]=& _tmp8B5,((_tmp8B6[1]=& _tmp8B4,Cyc_Tcutil_terr(loc,((_tmp8B7="%s == %s is not a struct/union type",_tag_dyneither(_tmp8B7,sizeof(char),36))),_tag_dyneither(_tmp8B6,sizeof(void*),2)))))))))))));};}}
# 1487
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp319=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp316;if(_tmp319->tag != 1)goto _LL176;else{_tmp31A=_tmp319->f1;}}_LL17A: {
# 1489
int bad_type=1;
{void*_tmp340=Cyc_Tcutil_compress(_tmp314);void*_tmp341=_tmp340;struct Cyc_Absyn_Aggrdecl**_tmp343;struct Cyc_List_List*_tmp345;struct Cyc_List_List*_tmp347;struct Cyc_Absyn_Datatypefield*_tmp349;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp341;if(_tmp342->tag != 11)goto _LL18A;else{if((((_tmp342->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp343=(struct Cyc_Absyn_Aggrdecl**)(((_tmp342->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1492
 if((*_tmp343)->impl == 0)
goto _LL187;
_tmp345=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp343)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp341;if(_tmp344->tag != 12)goto _LL18C;else{_tmp345=_tmp344->f2;}}_LL18B:
# 1496
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp345)<= _tmp31A){
const char*_tmp8BC;void*_tmp8BB[2];struct Cyc_Int_pa_PrintArg_struct _tmp8BA;struct Cyc_Int_pa_PrintArg_struct _tmp8B9;(_tmp8B9.tag=1,((_tmp8B9.f1=(unsigned long)((int)_tmp31A),((_tmp8BA.tag=1,((_tmp8BA.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp345),((_tmp8BB[0]=& _tmp8BA,((_tmp8BB[1]=& _tmp8B9,Cyc_Tcutil_terr(loc,((_tmp8BC="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp8BC,sizeof(char),46))),_tag_dyneither(_tmp8BB,sizeof(void*),2)))))))))))));}else{
# 1500
_tmp314=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp345,(int)_tmp31A))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp341;if(_tmp346->tag != 10)goto _LL18E;else{_tmp347=_tmp346->f1;}}_LL18D:
# 1504
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp347)<= _tmp31A){
const char*_tmp8C1;void*_tmp8C0[2];struct Cyc_Int_pa_PrintArg_struct _tmp8BF;struct Cyc_Int_pa_PrintArg_struct _tmp8BE;(_tmp8BE.tag=1,((_tmp8BE.f1=(unsigned long)((int)_tmp31A),((_tmp8BF.tag=1,((_tmp8BF.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp347),((_tmp8C0[0]=& _tmp8BF,((_tmp8C0[1]=& _tmp8BE,Cyc_Tcutil_terr(loc,((_tmp8C1="tuple has too few components: %d <= %d",_tag_dyneither(_tmp8C1,sizeof(char),39))),_tag_dyneither(_tmp8C0,sizeof(void*),2)))))))))))));}else{
# 1508
_tmp314=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp347,(int)_tmp31A)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp341;if(_tmp348->tag != 4)goto _LL190;else{if((((_tmp348->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp349=((struct _tuple2)(((_tmp348->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1512
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp349->typs)< _tmp31A){
const char*_tmp8C6;void*_tmp8C5[2];struct Cyc_Int_pa_PrintArg_struct _tmp8C4;struct Cyc_Int_pa_PrintArg_struct _tmp8C3;(_tmp8C3.tag=1,((_tmp8C3.f1=(unsigned long)((int)_tmp31A),((_tmp8C4.tag=1,((_tmp8C4.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp349->typs),((_tmp8C5[0]=& _tmp8C4,((_tmp8C5[1]=& _tmp8C3,Cyc_Tcutil_terr(loc,((_tmp8C6="datatype field has too few components: %d < %d",_tag_dyneither(_tmp8C6,sizeof(char),47))),_tag_dyneither(_tmp8C5,sizeof(void*),2)))))))))))));}else{
# 1516
if(_tmp31A != 0)
_tmp314=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp349->typs,(int)(_tmp31A - 1))).f2;else{
if(_tmp313->tl != 0){
const char*_tmp8C9;void*_tmp8C8;(_tmp8C8=0,Cyc_Tcutil_terr(loc,((_tmp8C9="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp8C9,sizeof(char),68))),_tag_dyneither(_tmp8C8,sizeof(void*),0)));}}}
# 1521
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1525
if(bad_type){
const char*_tmp8CD;void*_tmp8CC[1];struct Cyc_String_pa_PrintArg_struct _tmp8CB;(_tmp8CB.tag=0,((_tmp8CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp8CC[0]=& _tmp8CB,Cyc_Tcutil_terr(loc,((_tmp8CD="%s is not a known type",_tag_dyneither(_tmp8CD,sizeof(char),23))),_tag_dyneither(_tmp8CC,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1530
return Cyc_Absyn_uint_typ;};}
# 1534
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp35B=_new_region("r");struct _RegionHandle*r=& _tmp35B;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp35C=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp35C,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp35D=t;void*_tmp360;void*_tmp361;union Cyc_Absyn_Constraint*_tmp362;union Cyc_Absyn_Constraint*_tmp363;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35D;if(_tmp35E->tag != 1)goto _LL195;}_LL194: {
# 1541
struct Cyc_List_List*_tmp364=Cyc_Tcenv_lookup_type_vars(_tmp35C);
struct Cyc_Core_Opt*_tmp8CE;void*_tmp365=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE->v=_tmp364,_tmp8CE)))));
struct Cyc_Core_Opt*_tmp8CF;void*_tmp366=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF->v=_tmp364,_tmp8CF)))));
union Cyc_Absyn_Constraint*_tmp367=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp368=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp8D0;struct Cyc_Absyn_PtrAtts _tmp369=(_tmp8D0.rgn=_tmp366,((_tmp8D0.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp8D0.bounds=_tmp367,((_tmp8D0.zero_term=_tmp368,((_tmp8D0.ptrloc=0,_tmp8D0)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8D6;struct Cyc_Absyn_PtrInfo _tmp8D5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8D4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36A=(_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4[0]=((_tmp8D6.tag=5,((_tmp8D6.f1=((_tmp8D5.elt_typ=_tmp365,((_tmp8D5.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp8D5.ptr_atts=_tmp369,_tmp8D5)))))),_tmp8D6)))),_tmp8D4)));
Cyc_Tcutil_unify(t,(void*)_tmp36A);
_tmp360=_tmp365;_tmp361=_tmp366;_tmp362=_tmp367;_tmp363=_tmp368;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D;if(_tmp35F->tag != 5)goto _LL197;else{_tmp360=(_tmp35F->f1).elt_typ;_tmp361=((_tmp35F->f1).ptr_atts).rgn;_tmp362=((_tmp35F->f1).ptr_atts).bounds;_tmp363=((_tmp35F->f1).ptr_atts).zero_term;}}_LL196:
# 1551
 Cyc_Tcenv_check_rgn_accessible(_tmp35C,loc,_tmp361);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp362);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp360),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp35C)){
const char*_tmp8D9;void*_tmp8D8;(_tmp8D8=0,Cyc_Tcutil_terr(loc,((_tmp8D9="can't dereference abstract pointer type",_tag_dyneither(_tmp8D9,sizeof(char),40))),_tag_dyneither(_tmp8D8,sizeof(void*),0)));}{
void*_tmp373=_tmp360;_npop_handler(0);return _tmp373;};_LL197:;_LL198: {
# 1557
const char*_tmp8DD;void*_tmp8DC[1];struct Cyc_String_pa_PrintArg_struct _tmp8DB;void*_tmp377=(_tmp8DB.tag=0,((_tmp8DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8DC[0]=& _tmp8DB,Cyc_Tcexp_expr_err(_tmp35C,loc,topt,((_tmp8DD="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp8DD,sizeof(char),39))),_tag_dyneither(_tmp8DC,sizeof(void*),1)))))));_npop_handler(0);return _tmp377;}_LL192:;};}
# 1536
;_pop_region(r);}
# 1563
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1567
struct _RegionHandle _tmp378=_new_region("r");struct _RegionHandle*r=& _tmp378;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1571
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp379=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp37A=_tmp379;struct Cyc_Absyn_Aggrdecl*_tmp37C;struct Cyc_List_List*_tmp37D;enum Cyc_Absyn_AggrKind _tmp37F;struct Cyc_List_List*_tmp380;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37A;if(_tmp37B->tag != 11)goto _LL19C;else{if((((_tmp37B->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp37C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp37B->f1).aggr_info).KnownAggr).val);_tmp37D=(_tmp37B->f1).targs;}}_LL19B: {
# 1574
struct Cyc_Absyn_Aggrfield*_tmp381=Cyc_Absyn_lookup_decl_field(_tmp37C,f);
if(_tmp381 == 0){
const char*_tmp8E2;void*_tmp8E1[2];struct Cyc_String_pa_PrintArg_struct _tmp8E0;struct Cyc_String_pa_PrintArg_struct _tmp8DF;void*_tmp386=(_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E0.tag=0,((_tmp8E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp37C->name)),((_tmp8E1[0]=& _tmp8E0,((_tmp8E1[1]=& _tmp8DF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E2="%s has no %s member",_tag_dyneither(_tmp8E2,sizeof(char),20))),_tag_dyneither(_tmp8E1,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp386;}
# 1579
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37C->impl))->tagged)*is_tagged=1;{
void*t2=_tmp381->type;
if(_tmp37D != 0){
struct _RegionHandle _tmp387=_new_region("rgn");struct _RegionHandle*rgn=& _tmp387;_push_region(rgn);
{struct Cyc_List_List*_tmp388=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp37C->tvs,_tmp37D);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp388,_tmp381->type);}
# 1583
;_pop_region(rgn);}
# 1589
if((((_tmp37C->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37C->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp381->requires_clause == 0){
const char*_tmp8E6;void*_tmp8E5[1];struct Cyc_String_pa_PrintArg_struct _tmp8E4;void*_tmp38C=(_tmp8E4.tag=0,((_tmp8E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E5[0]=& _tmp8E4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8E6,sizeof(char),56))),_tag_dyneither(_tmp8E5,sizeof(void*),1)))))));_npop_handler(0);return _tmp38C;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37C->impl))->exist_vars != 0){
# 1594
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8EA;void*_tmp8E9[1];struct Cyc_String_pa_PrintArg_struct _tmp8E8;void*_tmp390=(_tmp8E8.tag=0,((_tmp8E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E9[0]=& _tmp8E8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8EA="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp8EA,sizeof(char),81))),_tag_dyneither(_tmp8E9,sizeof(void*),1)))))));_npop_handler(0);return _tmp390;}}{
# 1597
void*_tmp391=t2;_npop_handler(0);return _tmp391;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp37E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp37A;if(_tmp37E->tag != 12)goto _LL19E;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37E->f2;}}_LL19D: {
# 1599
struct Cyc_Absyn_Aggrfield*_tmp392=Cyc_Absyn_lookup_field(_tmp380,f);
if(_tmp392 == 0){
const char*_tmp8EE;void*_tmp8ED[1];struct Cyc_String_pa_PrintArg_struct _tmp8EC;void*_tmp396=(_tmp8EC.tag=0,((_tmp8EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8ED[0]=& _tmp8EC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8EE="type has no %s member",_tag_dyneither(_tmp8EE,sizeof(char),22))),_tag_dyneither(_tmp8ED,sizeof(void*),1)))))));_npop_handler(0);return _tmp396;}
# 1604
if(((_tmp37F == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp392->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp392->requires_clause == 0){
# 1606
const char*_tmp8F2;void*_tmp8F1[1];struct Cyc_String_pa_PrintArg_struct _tmp8F0;void*_tmp39A=(_tmp8F0.tag=0,((_tmp8F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8F1[0]=& _tmp8F0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F2="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8F2,sizeof(char),56))),_tag_dyneither(_tmp8F1,sizeof(void*),1)))))));_npop_handler(0);return _tmp39A;}{
void*_tmp39B=_tmp392->type;_npop_handler(0);return _tmp39B;};}_LL19E:;_LL19F: {
# 1609
const char*_tmp8F6;void*_tmp8F5[1];struct Cyc_String_pa_PrintArg_struct _tmp8F4;void*_tmp39F=(_tmp8F4.tag=0,((_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8F5[0]=& _tmp8F4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F6="expecting struct or union, found %s",_tag_dyneither(_tmp8F6,sizeof(char),36))),_tag_dyneither(_tmp8F5,sizeof(void*),1)))))));_npop_handler(0);return _tmp39F;}_LL199:;};
# 1567
;_pop_region(r);}
# 1615
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1618
struct _RegionHandle _tmp3A0=_new_region("r");struct _RegionHandle*r=& _tmp3A0;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1623
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp3A1=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3A2=_tmp3A1;void*_tmp3A4;void*_tmp3A5;union Cyc_Absyn_Constraint*_tmp3A6;union Cyc_Absyn_Constraint*_tmp3A7;_LL1A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A2;if(_tmp3A3->tag != 5)goto _LL1A3;else{_tmp3A4=(_tmp3A3->f1).elt_typ;_tmp3A5=((_tmp3A3->f1).ptr_atts).rgn;_tmp3A6=((_tmp3A3->f1).ptr_atts).bounds;_tmp3A7=((_tmp3A3->f1).ptr_atts).zero_term;}}_LL1A2:
# 1626
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp3A6);
{void*_tmp3A8=Cyc_Tcutil_compress(_tmp3A4);void*_tmp3A9=_tmp3A8;struct Cyc_Absyn_Aggrdecl*_tmp3AB;struct Cyc_List_List*_tmp3AC;enum Cyc_Absyn_AggrKind _tmp3AE;struct Cyc_List_List*_tmp3AF;_LL1A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3AA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AA->tag != 11)goto _LL1A8;else{if((((_tmp3AA->f1).aggr_info).KnownAggr).tag != 2)goto _LL1A8;_tmp3AB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp3AA->f1).aggr_info).KnownAggr).val);_tmp3AC=(_tmp3AA->f1).targs;}}_LL1A7: {
# 1629
struct Cyc_Absyn_Aggrfield*_tmp3B0=Cyc_Absyn_lookup_decl_field(_tmp3AB,f);
if(_tmp3B0 == 0){
const char*_tmp8FB;void*_tmp8FA[2];struct Cyc_String_pa_PrintArg_struct _tmp8F9;struct Cyc_String_pa_PrintArg_struct _tmp8F8;void*_tmp3B5=(_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8F9.tag=0,((_tmp8F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3AB->name)),((_tmp8FA[0]=& _tmp8F9,((_tmp8FA[1]=& _tmp8F8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FB="type %s has no %s member",_tag_dyneither(_tmp8FB,sizeof(char),25))),_tag_dyneither(_tmp8FA,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp3B5;}
# 1634
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->tagged)*is_tagged=1;{
void*t3=_tmp3B0->type;
if(_tmp3AC != 0){
struct _RegionHandle _tmp3B6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3B6;_push_region(rgn);
{struct Cyc_List_List*_tmp3B7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp3AB->tvs,_tmp3AC);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp3B7,_tmp3B0->type);}
# 1638
;_pop_region(rgn);}{
# 1642
struct Cyc_Absyn_Kind*_tmp3B8=Cyc_Tcutil_typ_kind(t3);
# 1645
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp3B8) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp3B9=Cyc_Tcutil_compress(t3);void*_tmp3BA=_tmp3B9;_LL1AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3BB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BA;if(_tmp3BB->tag != 8)goto _LL1AF;}_LL1AE:
 goto _LL1AC;_LL1AF:;_LL1B0: {
# 1649
const char*_tmp8FF;void*_tmp8FE[1];struct Cyc_String_pa_PrintArg_struct _tmp8FD;void*_tmp3BF=(_tmp8FD.tag=0,((_tmp8FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8FE[0]=& _tmp8FD,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FF="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8FF,sizeof(char),48))),_tag_dyneither(_tmp8FE,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BF;}_LL1AC:;}
# 1654
if((((_tmp3AB->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp3B0->requires_clause == 0){
# 1657
const char*_tmp903;void*_tmp902[1];struct Cyc_String_pa_PrintArg_struct _tmp901;void*_tmp3C3=(_tmp901.tag=0,((_tmp901.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp902[0]=& _tmp901,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp903="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp903,sizeof(char),56))),_tag_dyneither(_tmp902,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C3;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp907;void*_tmp906[1];struct Cyc_String_pa_PrintArg_struct _tmp905;void*_tmp3C7=(_tmp905.tag=0,((_tmp905.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp906[0]=& _tmp905,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp907="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp907,sizeof(char),72))),_tag_dyneither(_tmp906,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C7;}}{
# 1664
void*_tmp3C8=t3;_npop_handler(0);return _tmp3C8;};};};}_LL1A8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AD->tag != 12)goto _LL1AA;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;}}_LL1A9: {
# 1666
struct Cyc_Absyn_Aggrfield*_tmp3C9=Cyc_Absyn_lookup_field(_tmp3AF,f);
if(_tmp3C9 == 0){
const char*_tmp90B;void*_tmp90A[1];struct Cyc_String_pa_PrintArg_struct _tmp909;void*_tmp3CD=(_tmp909.tag=0,((_tmp909.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp90A[0]=& _tmp909,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp90B="type has no %s field",_tag_dyneither(_tmp90B,sizeof(char),21))),_tag_dyneither(_tmp90A,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CD;}
# 1671
if((_tmp3AE == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp3C9->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp90F;void*_tmp90E[1];struct Cyc_String_pa_PrintArg_struct _tmp90D;void*_tmp3D1=(_tmp90D.tag=0,((_tmp90D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp90E[0]=& _tmp90D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp90F="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp90F,sizeof(char),56))),_tag_dyneither(_tmp90E,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D1;}{
void*_tmp3D2=_tmp3C9->type;_npop_handler(0);return _tmp3D2;};}_LL1AA:;_LL1AB:
 goto _LL1A5;_LL1A5:;}
# 1676
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1679
const char*_tmp913;void*_tmp912[1];struct Cyc_String_pa_PrintArg_struct _tmp911;void*_tmp3D6=(_tmp911.tag=0,((_tmp911.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp912[0]=& _tmp911,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp913="expecting struct or union pointer, found %s",_tag_dyneither(_tmp913,sizeof(char),44))),_tag_dyneither(_tmp912,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D6;};
# 1618
;_pop_region(r);}
# 1684
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1686
struct _tuple15 _tmp3D7=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp3D9;int _tmp3DA;struct _tuple15 _tmp3D8=_tmp3D7;_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;
if(!_tmp3DA){
const char*_tmp916;void*_tmp915;return(_tmp915=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp916="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp916,sizeof(char),47))),_tag_dyneither(_tmp915,sizeof(void*),0)));}{
# 1690
struct _handler_cons _tmp3DD;_push_handler(& _tmp3DD);{int _tmp3DF=0;if(setjmp(_tmp3DD.handler))_tmp3DF=1;if(!_tmp3DF){
{void*_tmp3E0=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3D9)).f2;_npop_handler(0);return _tmp3E0;};_pop_handler();}else{void*_tmp3DE=(void*)_exn_thrown;void*_tmp3E2=_tmp3DE;void*_tmp3E4;_LL1B2: {struct Cyc_List_Nth_exn_struct*_tmp3E3=(struct Cyc_List_Nth_exn_struct*)_tmp3E2;if(_tmp3E3->tag != Cyc_List_Nth)goto _LL1B4;}_LL1B3: {
# 1693
const char*_tmp91B;void*_tmp91A[2];struct Cyc_Int_pa_PrintArg_struct _tmp919;struct Cyc_Int_pa_PrintArg_struct _tmp918;return(_tmp918.tag=1,((_tmp918.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp919.tag=1,((_tmp919.f1=(unsigned long)((int)_tmp3D9),((_tmp91A[0]=& _tmp919,((_tmp91A[1]=& _tmp918,Cyc_Tcexp_expr_err(te,loc,0,((_tmp91B="index is %d but tuple has only %d fields",_tag_dyneither(_tmp91B,sizeof(char),41))),_tag_dyneither(_tmp91A,sizeof(void*),2)))))))))))));}_LL1B4: _tmp3E4=_tmp3E2;_LL1B5:(void)_rethrow(_tmp3E4);_LL1B1:;}};};}
# 1698
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1700
struct _RegionHandle _tmp3E9=_new_region("r");struct _RegionHandle*r=& _tmp3E9;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3EA=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3EA),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3EA),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3EA,e2)){
const char*_tmp91F;void*_tmp91E[1];struct Cyc_String_pa_PrintArg_struct _tmp91D;void*_tmp3EE=(_tmp91D.tag=0,((_tmp91D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp91E[0]=& _tmp91D,Cyc_Tcexp_expr_err(_tmp3EA,e2->loc,topt,((_tmp91F="expecting int subscript, found %s",_tag_dyneither(_tmp91F,sizeof(char),34))),_tag_dyneither(_tmp91E,sizeof(void*),1)))))));_npop_handler(0);return _tmp3EE;}{
# 1711
void*_tmp3EF=t1;void*_tmp3F1;struct Cyc_Absyn_Tqual _tmp3F2;void*_tmp3F3;union Cyc_Absyn_Constraint*_tmp3F4;union Cyc_Absyn_Constraint*_tmp3F5;struct Cyc_List_List*_tmp3F7;_LL1B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F0->tag != 5)goto _LL1B9;else{_tmp3F1=(_tmp3F0->f1).elt_typ;_tmp3F2=(_tmp3F0->f1).elt_tq;_tmp3F3=((_tmp3F0->f1).ptr_atts).rgn;_tmp3F4=((_tmp3F0->f1).ptr_atts).bounds;_tmp3F5=((_tmp3F0->f1).ptr_atts).zero_term;}}_LL1B8:
# 1715
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F5)){
int emit_warning=1;
{void*_tmp3F8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3F4);void*_tmp3F9=_tmp3F8;struct Cyc_Absyn_Exp*_tmp3FB;_LL1BE: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3FA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F9;if(_tmp3FA->tag != 1)goto _LL1C0;else{_tmp3FB=_tmp3FA->f1;}}_LL1BF:
# 1719
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp3FD=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3FF;int _tmp400;struct _tuple15 _tmp3FE=_tmp3FD;_tmp3FF=_tmp3FE.f1;_tmp400=_tmp3FE.f2;
if(_tmp400){
struct _tuple15 _tmp401=Cyc_Evexp_eval_const_uint_exp(_tmp3FB);unsigned int _tmp403;int _tmp404;struct _tuple15 _tmp402=_tmp401;_tmp403=_tmp402.f1;_tmp404=_tmp402.f2;
if(_tmp404  && _tmp403 > _tmp3FF)emit_warning=0;}}
# 1726
goto _LL1BD;_LL1C0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3FC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3F9;if(_tmp3FC->tag != 0)goto _LL1BD;}_LL1C1:
 emit_warning=0;goto _LL1BD;_LL1BD:;}
# 1729
if(emit_warning){
const char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_warn(e2->loc,((_tmp922="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp922,sizeof(char),63))),_tag_dyneither(_tmp921,sizeof(void*),0)));}}else{
# 1733
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp407=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp409;int _tmp40A;struct _tuple15 _tmp408=_tmp407;_tmp409=_tmp408.f1;_tmp40A=_tmp408.f2;
if(_tmp40A)
Cyc_Tcutil_check_bound(loc,_tmp409,_tmp3F4);}else{
# 1739
if(Cyc_Tcutil_is_bound_one(_tmp3F4) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F5)){
const char*_tmp925;void*_tmp924;(_tmp924=0,Cyc_Tcutil_warn(e1->loc,((_tmp925="subscript applied to pointer to one object",_tag_dyneither(_tmp925,sizeof(char),43))),_tag_dyneither(_tmp924,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3F4);}}
# 1744
Cyc_Tcenv_check_rgn_accessible(_tmp3EA,loc,_tmp3F3);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3F1),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3EA)){
const char*_tmp928;void*_tmp927;(_tmp927=0,Cyc_Tcutil_terr(e1->loc,((_tmp928="can't subscript an abstract pointer",_tag_dyneither(_tmp928,sizeof(char),36))),_tag_dyneither(_tmp927,sizeof(void*),0)));}{
void*_tmp40F=_tmp3F1;_npop_handler(0);return _tmp40F;};_LL1B9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F6->tag != 10)goto _LL1BB;else{_tmp3F7=_tmp3F6->f1;}}_LL1BA: {
void*_tmp410=Cyc_Tcexp_ithTupleType(_tmp3EA,loc,_tmp3F7,e2);_npop_handler(0);return _tmp410;}_LL1BB:;_LL1BC: {
const char*_tmp92C;void*_tmp92B[1];struct Cyc_String_pa_PrintArg_struct _tmp92A;void*_tmp414=(_tmp92A.tag=0,((_tmp92A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp92B[0]=& _tmp92A,Cyc_Tcexp_expr_err(_tmp3EA,loc,topt,((_tmp92C="subscript applied to %s",_tag_dyneither(_tmp92C,sizeof(char),24))),_tag_dyneither(_tmp92B,sizeof(void*),1)))))));_npop_handler(0);return _tmp414;}_LL1B6:;};};}
# 1701
;_pop_region(r);}
# 1755
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp415=Cyc_Tcutil_compress(*topt);void*_tmp416=_tmp415;struct Cyc_List_List*_tmp418;_LL1C3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp416;if(_tmp417->tag != 10)goto _LL1C5;else{_tmp418=_tmp417->f1;}}_LL1C4:
# 1761
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp418)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1764
goto _LL1C2;
# 1766
for(0;es != 0;(es=es->tl,_tmp418=_tmp418->tl)){
struct _RegionHandle _tmp419=_new_region("r");struct _RegionHandle*r=& _tmp419;_push_region(r);
{int bogus=0;
void*_tmp41A=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp418))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp41A,(struct Cyc_Absyn_Exp*)es->hd);
# 1772
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)_tmp418->hd)).f2,& bogus);{
struct _tuple18*_tmp92F;struct Cyc_List_List*_tmp92E;fields=((_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E->hd=((_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F->f1=(*((struct _tuple18*)_tmp418->hd)).f1,((_tmp92F->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp92F)))))),((_tmp92E->tl=fields,_tmp92E))))));};}
# 1768
;_pop_region(r);}
# 1776
done=1;
goto _LL1C2;_LL1C5:;_LL1C6:
 goto _LL1C2;_LL1C2:;}
# 1780
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp41D=_new_region("r");struct _RegionHandle*r=& _tmp41D;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple18*_tmp932;struct Cyc_List_List*_tmp931;fields=((_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->hd=((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932->f1=Cyc_Absyn_empty_tqual(0),((_tmp932->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp932)))))),((_tmp931->tl=fields,_tmp931))))));};
# 1783
;_pop_region(r);}{
# 1787
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp935;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp934;return(void*)((_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934[0]=((_tmp935.tag=10,((_tmp935.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp935)))),_tmp934))));};}
# 1791
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1794
const char*_tmp938;void*_tmp937;return(_tmp937=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp938="tcCompoundLit",_tag_dyneither(_tmp938,sizeof(char),14))),_tag_dyneither(_tmp937,sizeof(void*),0)));}
# 1804 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1807
void*res_t2;
struct _RegionHandle _tmp424=_new_region("r");struct _RegionHandle*r=& _tmp424;_push_region(r);{
int _tmp425=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp93B;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp93A;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp426=(_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A[0]=((_tmp93B.tag=0,((_tmp93B.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp425),_tmp93B)))),_tmp93A)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp426,loc);
# 1816
if(zero_term){
struct Cyc_Absyn_Exp*_tmp427=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp425 - 1);
if(!Cyc_Tcutil_is_zero(_tmp427)){
const char*_tmp93E;void*_tmp93D;(_tmp93D=0,Cyc_Tcutil_terr(_tmp427->loc,((_tmp93E="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp93E,sizeof(char),45))),_tag_dyneither(_tmp93D,sizeof(void*),0)));}}
# 1821
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1826
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1829
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1831
const char*_tmp942;void*_tmp941[1];struct Cyc_String_pa_PrintArg_struct _tmp940;(_tmp940.tag=0,((_tmp940.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp941[0]=& _tmp940,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp942="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp942,sizeof(char),53))),_tag_dyneither(_tmp941,sizeof(void*),1)))))));}}
# 1835
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1840
void*_tmp42F=(void*)ds->hd;void*_tmp430=_tmp42F;struct Cyc_Absyn_Exp*_tmp433;_LL1C8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp431=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp430;if(_tmp431->tag != 1)goto _LL1CA;}_LL1C9:
# 1842
{const char*_tmp945;void*_tmp944;(_tmp944=0,Cyc_Tcutil_terr(loc,((_tmp945="only array index designators are supported",_tag_dyneither(_tmp945,sizeof(char),43))),_tag_dyneither(_tmp944,sizeof(void*),0)));}
goto _LL1C7;_LL1CA: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp432=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp430;if(_tmp432->tag != 0)goto _LL1C7;else{_tmp433=_tmp432->f1;}}_LL1CB:
# 1845
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp433);{
struct _tuple15 _tmp436=Cyc_Evexp_eval_const_uint_exp(_tmp433);unsigned int _tmp438;int _tmp439;struct _tuple15 _tmp437=_tmp436;_tmp438=_tmp437.f1;_tmp439=_tmp437.f2;
if(!_tmp439){
const char*_tmp948;void*_tmp947;(_tmp947=0,Cyc_Tcutil_terr(_tmp433->loc,((_tmp948="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp948,sizeof(char),47))),_tag_dyneither(_tmp947,sizeof(void*),0)));}else{
if(_tmp438 != offset){
const char*_tmp94D;void*_tmp94C[2];struct Cyc_Int_pa_PrintArg_struct _tmp94B;struct Cyc_Int_pa_PrintArg_struct _tmp94A;(_tmp94A.tag=1,((_tmp94A.f1=(unsigned long)((int)_tmp438),((_tmp94B.tag=1,((_tmp94B.f1=(unsigned long)offset,((_tmp94C[0]=& _tmp94B,((_tmp94C[1]=& _tmp94A,Cyc_Tcutil_terr(_tmp433->loc,((_tmp94D="expecting index designator %d but found %d",_tag_dyneither(_tmp94D,sizeof(char),43))),_tag_dyneither(_tmp94C,sizeof(void*),2)))))))))))));}}
# 1852
goto _LL1C7;};_LL1C7:;}}}{
# 1856
void*_tmp440=res_t2;_npop_handler(0);return _tmp440;};
# 1808
;_pop_region(r);}
# 1860
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1863
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp441=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp442=_tmp441;void*_tmp444;_LL1CD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp443=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp442;if(_tmp443->tag != 19)goto _LL1CF;else{_tmp444=(void*)_tmp443->f1;}}_LL1CE:
# 1868
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp445=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp444,0),0,Cyc_Absyn_No_coercion,0);
_tmp445->topt=bound->topt;
# 1873
bound=_tmp445;}
# 1875
goto _LL1CC;_LL1CF:;_LL1D0:
# 1877
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp951;void*_tmp950[1];struct Cyc_String_pa_PrintArg_struct _tmp94F;(_tmp94F.tag=0,((_tmp94F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp950[0]=& _tmp94F,Cyc_Tcutil_terr(bound->loc,((_tmp951="expecting unsigned int, found %s",_tag_dyneither(_tmp951,sizeof(char),33))),_tag_dyneither(_tmp950,sizeof(void*),1)))))));}_LL1CC:;}
# 1882
if(!(vd->tq).real_const){
const char*_tmp954;void*_tmp953;(_tmp953=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp954="comprehension index variable is not declared const!",_tag_dyneither(_tmp954,sizeof(char),52))),_tag_dyneither(_tmp953,sizeof(void*),0)));}{
# 1885
struct _RegionHandle _tmp44B=_new_region("r");struct _RegionHandle*r=& _tmp44B;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1891
struct Cyc_Tcenv_Tenv*_tmp44C=te2;
void**_tmp44D=0;
struct Cyc_Absyn_Tqual*_tmp44E=0;
union Cyc_Absyn_Constraint**_tmp44F=0;
# 1896
if(topt != 0){
void*_tmp450=Cyc_Tcutil_compress(*topt);void*_tmp451=_tmp450;struct Cyc_Absyn_PtrInfo _tmp453;void*_tmp455;struct Cyc_Absyn_Tqual _tmp456;struct Cyc_Absyn_Exp*_tmp457;union Cyc_Absyn_Constraint*_tmp458;_LL1D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp451;if(_tmp452->tag != 5)goto _LL1D4;else{_tmp453=_tmp452->f1;}}_LL1D3:
# 1899
{void**_tmp955;_tmp44D=((_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955[0]=_tmp453.elt_typ,_tmp955))));}
{struct Cyc_Absyn_Tqual*_tmp956;_tmp44E=((_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=_tmp453.elt_tq,_tmp956))));}
{union Cyc_Absyn_Constraint**_tmp957;_tmp44F=((_tmp957=_region_malloc(r,sizeof(*_tmp957)),((_tmp957[0]=(_tmp453.ptr_atts).zero_term,_tmp957))));}
goto _LL1D1;_LL1D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp451;if(_tmp454->tag != 8)goto _LL1D6;else{_tmp455=(_tmp454->f1).elt_type;_tmp456=(_tmp454->f1).tq;_tmp457=(_tmp454->f1).num_elts;_tmp458=(_tmp454->f1).zero_term;}}_LL1D5:
# 1904
{void**_tmp958;_tmp44D=((_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958[0]=_tmp455,_tmp958))));}
{struct Cyc_Absyn_Tqual*_tmp959;_tmp44E=((_tmp959=_region_malloc(r,sizeof(*_tmp959)),((_tmp959[0]=_tmp456,_tmp959))));}
{union Cyc_Absyn_Constraint**_tmp95A;_tmp44F=((_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A[0]=_tmp458,_tmp95A))));}
goto _LL1D1;_LL1D6:;_LL1D7:
# 1909
 goto _LL1D1;_LL1D1:;}{
# 1912
void*t=Cyc_Tcexp_tcExp(_tmp44C,_tmp44D,body);
# 1914
struct _RegionHandle _tmp45F=_new_region("temp");struct _RegionHandle*temp=& _tmp45F;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body)){
const char*_tmp95D;void*_tmp95C;(_tmp95C=0,Cyc_Tcutil_terr(body->loc,((_tmp95D="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp95D,sizeof(char),49))),_tag_dyneither(_tmp95C,sizeof(void*),0)));}
# 1918
if(_tmp44C->le == 0){
# 1920
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp960;void*_tmp95F;(_tmp95F=0,Cyc_Tcutil_terr(bound->loc,((_tmp960="bound is not constant",_tag_dyneither(_tmp960,sizeof(char),22))),_tag_dyneither(_tmp95F,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp963;void*_tmp962;(_tmp962=0,Cyc_Tcutil_terr(bound->loc,((_tmp963="body is not constant",_tag_dyneither(_tmp963,sizeof(char),21))),_tag_dyneither(_tmp962,sizeof(void*),0)));}}
# 1925
if(_tmp44F != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp44F)){
# 1928
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_uint_exp(1,0);_tmp466->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp466,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1932
struct _RegionHandle _tmp467=_new_region("r");struct _RegionHandle*r=& _tmp467;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body)){
const char*_tmp966;void*_tmp965;(_tmp965=0,Cyc_Tcutil_terr(body->loc,((_tmp966="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp966,sizeof(char),49))),_tag_dyneither(_tmp965,sizeof(void*),0)));}{
# 1938
void*_tmp46A=Cyc_Absyn_array_typ(t,_tmp44E == 0?Cyc_Absyn_empty_tqual(0):*_tmp44E,bound,
_tmp44F == 0?Cyc_Absyn_false_conref:*_tmp44F,0);
void*_tmp46B=_tmp46A;_npop_handler(2);return _tmp46B;};
# 1932
;_pop_region(r);};
# 1914
;_pop_region(temp);};};}
# 1886
;_pop_region(r);};}
# 1945
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1948
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp46C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp46D=_tmp46C;void*_tmp46F;_LL1D9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46D;if(_tmp46E->tag != 19)goto _LL1DB;else{_tmp46F=(void*)_tmp46E->f1;}}_LL1DA:
# 1953
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp46F,0),0,Cyc_Absyn_No_coercion,0);
_tmp470->topt=bound->topt;
# 1958
bound=_tmp470;}
# 1960
goto _LL1D8;_LL1DB:;_LL1DC:
# 1962
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp96A;void*_tmp969[1];struct Cyc_String_pa_PrintArg_struct _tmp968;(_tmp968.tag=0,((_tmp968.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp969[0]=& _tmp968,Cyc_Tcutil_terr(bound->loc,((_tmp96A="expecting unsigned int, found %s",_tag_dyneither(_tmp96A,sizeof(char),33))),_tag_dyneither(_tmp969,sizeof(void*),1)))))));}_LL1D8:;}{
# 1967
struct _RegionHandle _tmp474=_new_region("r");struct _RegionHandle*r=& _tmp474;_push_region(r);
{void**_tmp475=0;
struct Cyc_Absyn_Tqual*_tmp476=0;
union Cyc_Absyn_Constraint**_tmp477=0;
# 1972
if(topt != 0){
void*_tmp478=Cyc_Tcutil_compress(*topt);void*_tmp479=_tmp478;struct Cyc_Absyn_PtrInfo _tmp47B;void*_tmp47D;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Exp*_tmp47F;union Cyc_Absyn_Constraint*_tmp480;_LL1DE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479;if(_tmp47A->tag != 5)goto _LL1E0;else{_tmp47B=_tmp47A->f1;}}_LL1DF:
# 1975
{void**_tmp96B;_tmp475=((_tmp96B=_region_malloc(r,sizeof(*_tmp96B)),((_tmp96B[0]=_tmp47B.elt_typ,_tmp96B))));}
{struct Cyc_Absyn_Tqual*_tmp96C;_tmp476=((_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C[0]=_tmp47B.elt_tq,_tmp96C))));}
{union Cyc_Absyn_Constraint**_tmp96D;_tmp477=((_tmp96D=_region_malloc(r,sizeof(*_tmp96D)),((_tmp96D[0]=(_tmp47B.ptr_atts).zero_term,_tmp96D))));}
goto _LL1DD;_LL1E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp479;if(_tmp47C->tag != 8)goto _LL1E2;else{_tmp47D=(_tmp47C->f1).elt_type;_tmp47E=(_tmp47C->f1).tq;_tmp47F=(_tmp47C->f1).num_elts;_tmp480=(_tmp47C->f1).zero_term;}}_LL1E1:
# 1980
{void**_tmp96E;_tmp475=((_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=_tmp47D,_tmp96E))));}
{struct Cyc_Absyn_Tqual*_tmp96F;_tmp476=((_tmp96F=_region_malloc(r,sizeof(*_tmp96F)),((_tmp96F[0]=_tmp47E,_tmp96F))));}
{union Cyc_Absyn_Constraint**_tmp970;_tmp477=((_tmp970=_region_malloc(r,sizeof(*_tmp970)),((_tmp970[0]=_tmp480,_tmp970))));}
goto _LL1DD;_LL1E2:;_LL1E3:
# 1985
 goto _LL1DD;_LL1DD:;}
# 1988
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,1,t);
if(_tmp475 != 0)Cyc_Tcutil_unify(*_tmp475,t);
# 1991
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp973;void*_tmp972;(_tmp972=0,Cyc_Tcutil_terr(bound->loc,((_tmp973="bound is not constant",_tag_dyneither(_tmp973,sizeof(char),22))),_tag_dyneither(_tmp972,sizeof(void*),0)));}}
# 1995
if(_tmp477 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp477)){
# 1998
struct Cyc_Absyn_Exp*_tmp489=Cyc_Absyn_uint_exp(1,0);_tmp489->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp489,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 2002
const char*_tmp976;void*_tmp975;(_tmp975=0,Cyc_Tcutil_terr(loc,((_tmp976="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp976,sizeof(char),74))),_tag_dyneither(_tmp975,sizeof(void*),0)));};}{
# 2005
void*_tmp48C=Cyc_Absyn_array_typ(t,_tmp476 == 0?Cyc_Absyn_empty_tqual(0):*_tmp476,bound,
_tmp477 == 0?Cyc_Absyn_false_conref:*_tmp477,0);
void*_tmp48D=_tmp48C;_npop_handler(0);return _tmp48D;};}
# 1968
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2021 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2025
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp977;adptr=((_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977[0]=ad,_tmp977))));};}else{
# 2031
{struct _handler_cons _tmp48F;_push_handler(& _tmp48F);{int _tmp491=0;if(setjmp(_tmp48F.handler))_tmp491=1;if(!_tmp491){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2031
;_pop_handler();}else{void*_tmp490=(void*)_exn_thrown;void*_tmp493=_tmp490;void*_tmp495;_LL1E5: {struct Cyc_Dict_Absent_exn_struct*_tmp494=(struct Cyc_Dict_Absent_exn_struct*)_tmp493;if(_tmp494->tag != Cyc_Dict_Absent)goto _LL1E7;}_LL1E6:
# 2034
{const char*_tmp97B;void*_tmp97A[1];struct Cyc_String_pa_PrintArg_struct _tmp979;(_tmp979.tag=0,((_tmp979.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp97A[0]=& _tmp979,Cyc_Tcutil_terr(loc,((_tmp97B="unbound struct/union name %s",_tag_dyneither(_tmp97B,sizeof(char),29))),_tag_dyneither(_tmp97A,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1E7: _tmp495=_tmp493;_LL1E8:(void)_rethrow(_tmp495);_LL1E4:;}};}
# 2037
*ad_opt=ad;
*tn=ad->name;}
# 2040
if(ad->impl == 0){
{const char*_tmp983;void*_tmp982[1];const char*_tmp981;const char*_tmp980;struct Cyc_String_pa_PrintArg_struct _tmp97F;(_tmp97F.tag=0,((_tmp97F.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp980="struct",_tag_dyneither(_tmp980,sizeof(char),7)):((_tmp981="union",_tag_dyneither(_tmp981,sizeof(char),6)))),((_tmp982[0]=& _tmp97F,Cyc_Tcutil_terr(loc,((_tmp983="can't construct abstract %s",_tag_dyneither(_tmp983,sizeof(char),28))),_tag_dyneither(_tmp982,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 2044
struct _RegionHandle _tmp49E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp49E;_push_region(rgn);
# 2049
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp49F=Cyc_Tcenv_new_status(te);
if(_tmp49F == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 2058
te2=Cyc_Tcenv_set_new_status(rgn,_tmp49F,te);}{
# 2060
struct _tuple12 _tmp984;struct _tuple12 _tmp4A0=(_tmp984.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp984.f2=rgn,_tmp984)));
struct Cyc_List_List*_tmp4A1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4A0,ad->tvs);
struct Cyc_List_List*_tmp4A2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4A0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp4A3=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4A1);
struct Cyc_List_List*_tmp4A4=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4A2);
struct Cyc_List_List*_tmp4A5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp4A1,_tmp4A2);
void*res_typ;
# 2071
if(topt != 0){
void*_tmp4A6=Cyc_Tcutil_compress(*topt);void*_tmp4A7=_tmp4A6;struct Cyc_Absyn_Aggrdecl**_tmp4A9;struct Cyc_List_List*_tmp4AA;_LL1EA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4A8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4A7;if(_tmp4A8->tag != 11)goto _LL1EC;else{if((((_tmp4A8->f1).aggr_info).KnownAggr).tag != 2)goto _LL1EC;_tmp4A9=(struct Cyc_Absyn_Aggrdecl**)(((_tmp4A8->f1).aggr_info).KnownAggr).val;_tmp4AA=(_tmp4A8->f1).targs;}}_LL1EB:
# 2074
 if(*_tmp4A9 == *adptr){
{struct Cyc_List_List*_tmp4AB=_tmp4A3;for(0;_tmp4AB != 0  && _tmp4AA != 0;
(_tmp4AB=_tmp4AB->tl,_tmp4AA=_tmp4AA->tl)){
Cyc_Tcutil_unify((void*)_tmp4AB->hd,(void*)_tmp4AA->hd);}}
# 2079
res_typ=*topt;
goto _LL1E9;}
# 2082
goto _LL1ED;_LL1EC:;_LL1ED: {
# 2084
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp98A;struct Cyc_Absyn_AggrInfo _tmp989;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp988;res_typ=(void*)((_tmp988=_cycalloc(sizeof(*_tmp988)),((_tmp988[0]=((_tmp98A.tag=11,((_tmp98A.f1=((_tmp989.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp989.targs=_tmp4A3,_tmp989)))),_tmp98A)))),_tmp988))));}_LL1E9:;}else{
# 2087
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp990;struct Cyc_Absyn_AggrInfo _tmp98F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp98E;res_typ=(void*)((_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp990.tag=11,((_tmp990.f1=((_tmp98F.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp98F.targs=_tmp4A3,_tmp98F)))),_tmp990)))),_tmp98E))));}{
# 2090
struct Cyc_List_List*_tmp4B2=*ts;
struct Cyc_List_List*_tmp4B3=_tmp4A4;
while(_tmp4B2 != 0  && _tmp4B3 != 0){
# 2094
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,0,(void*)_tmp4B2->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp4B2->hd);
Cyc_Tcutil_unify((void*)_tmp4B2->hd,(void*)_tmp4B3->hd);
_tmp4B2=_tmp4B2->tl;
_tmp4B3=_tmp4B3->tl;}
# 2100
if(_tmp4B2 != 0){
const char*_tmp993;void*_tmp992;(_tmp992=0,Cyc_Tcutil_terr(loc,((_tmp993="too many explicit witness types",_tag_dyneither(_tmp993,sizeof(char),32))),_tag_dyneither(_tmp992,sizeof(void*),0)));}
# 2103
*ts=_tmp4A4;{
# 2106
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp4B6=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct _tuple19*_tmp4B7=_tmp4B6;_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;{
void*_tmp4BA=Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,_tmp4B8->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp4BA,_tmp4B9);
# 2117
if(!Cyc_Tcutil_coerce_arg(te2,_tmp4B9,_tmp4BA,& bogus)){
{const char*_tmp99F;void*_tmp99E[5];struct Cyc_String_pa_PrintArg_struct _tmp99D;const char*_tmp99C;const char*_tmp99B;struct Cyc_String_pa_PrintArg_struct _tmp99A;struct Cyc_String_pa_PrintArg_struct _tmp999;struct Cyc_String_pa_PrintArg_struct _tmp998;struct Cyc_String_pa_PrintArg_struct _tmp997;(_tmp997.tag=0,((_tmp997.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2121
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4B9->topt))),((_tmp998.tag=0,((_tmp998.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2120
Cyc_Absynpp_typ2string(_tmp4BA)),((_tmp999.tag=0,((_tmp999.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp99A.tag=0,((_tmp99A.f1=(struct _dyneither_ptr)(
# 2119
ad->kind == Cyc_Absyn_StructA?(_tmp99B="struct",_tag_dyneither(_tmp99B,sizeof(char),7)):((_tmp99C="union",_tag_dyneither(_tmp99C,sizeof(char),6)))),((_tmp99D.tag=0,((_tmp99D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B8->name),((_tmp99E[0]=& _tmp99D,((_tmp99E[1]=& _tmp99A,((_tmp99E[2]=& _tmp999,((_tmp99E[3]=& _tmp998,((_tmp99E[4]=& _tmp997,Cyc_Tcutil_terr(_tmp4B9->loc,((_tmp99F="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp99F,sizeof(char),40))),_tag_dyneither(_tmp99E,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2122
Cyc_Tcutil_explain_failure();}};}{
# 2126
struct Cyc_List_List*_tmp4C4=0;
{struct Cyc_List_List*_tmp4C5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp4C5 != 0;_tmp4C5=_tmp4C5->tl){
struct _tuple0*_tmp9A2;struct Cyc_List_List*_tmp9A1;_tmp4C4=((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1->hd=((_tmp9A2=_cycalloc(sizeof(*_tmp9A2)),((_tmp9A2->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,(*((struct _tuple0*)_tmp4C5->hd)).f1),((_tmp9A2->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,(*((struct _tuple0*)_tmp4C5->hd)).f2),_tmp9A2)))))),((_tmp9A1->tl=_tmp4C4,_tmp9A1))))));}}
# 2131
_tmp4C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4C4);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp4C4);{
void*_tmp4C8=res_typ;_npop_handler(0);return _tmp4C8;};};};};};}
# 2049
;_pop_region(rgn);};}
# 2138
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2140
struct _RegionHandle _tmp4CA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4CA;_push_region(rgn);{
void*_tmp4CB=Cyc_Tcutil_compress(ts);void*_tmp4CC=_tmp4CB;enum Cyc_Absyn_AggrKind _tmp4CE;struct Cyc_List_List*_tmp4CF;_LL1EF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4CD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4CC;if(_tmp4CD->tag != 12)goto _LL1F1;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;}}_LL1F0:
# 2143
 if(_tmp4CE == Cyc_Absyn_UnionA){
const char*_tmp9A5;void*_tmp9A4;(_tmp9A4=0,Cyc_Tcutil_terr(loc,((_tmp9A5="expecting struct but found union",_tag_dyneither(_tmp9A5,sizeof(char),33))),_tag_dyneither(_tmp9A4,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp4CF);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp4D2=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct _tuple19*_tmp4D3=_tmp4D2;_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp4D4->type,_tmp4D5);
# 2152
if(!Cyc_Tcutil_coerce_arg(te,_tmp4D5,_tmp4D4->type,& bogus)){
{const char*_tmp9AB;void*_tmp9AA[3];struct Cyc_String_pa_PrintArg_struct _tmp9A9;struct Cyc_String_pa_PrintArg_struct _tmp9A8;struct Cyc_String_pa_PrintArg_struct _tmp9A7;(_tmp9A7.tag=0,((_tmp9A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2155
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4D5->topt))),((_tmp9A8.tag=0,((_tmp9A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2154
Cyc_Absynpp_typ2string(_tmp4D4->type)),((_tmp9A9.tag=0,((_tmp9A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D4->name),((_tmp9AA[0]=& _tmp9A9,((_tmp9AA[1]=& _tmp9A8,((_tmp9AA[2]=& _tmp9A7,Cyc_Tcutil_terr(_tmp4D5->loc,((_tmp9AB="field %s of struct expects type %s != %s",_tag_dyneither(_tmp9AB,sizeof(char),41))),_tag_dyneither(_tmp9AA,sizeof(void*),3)))))))))))))))))));}
# 2156
Cyc_Tcutil_explain_failure();}}
# 2159
goto _LL1EE;};_LL1F1:;_LL1F2: {
const char*_tmp9AE;void*_tmp9AD;(_tmp9AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AE="tcAnonStruct: wrong type",_tag_dyneither(_tmp9AE,sizeof(char),25))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));}_LL1EE:;}{
# 2163
void*_tmp4DD=ts;_npop_handler(0);return _tmp4DD;};
# 2140
;_pop_region(rgn);}
# 2167
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2170
struct _RegionHandle _tmp4DE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4DE;_push_region(rgn);
{struct _tuple12 _tmp9AF;struct _tuple12 _tmp4DF=(_tmp9AF.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9AF.f2=rgn,_tmp9AF)));
struct Cyc_List_List*_tmp4E0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4DF,tud->tvs);
struct Cyc_List_List*_tmp4E1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4E0);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp9B5;struct Cyc_Absyn_DatatypeFieldInfo _tmp9B4;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9B3;void*res=(void*)(
(_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B5.tag=4,((_tmp9B5.f1=((_tmp9B4.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp9B4.targs=_tmp4E1,_tmp9B4)))),_tmp9B5)))),_tmp9B3))));
# 2178
if(topt != 0){
void*_tmp4E2=Cyc_Tcutil_compress(*topt);void*_tmp4E3=_tmp4E2;_LL1F4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4E3;if(_tmp4E4->tag != 4)goto _LL1F6;}_LL1F5:
 Cyc_Tcutil_unify(*topt,res);goto _LL1F3;_LL1F6:;_LL1F7:
 goto _LL1F3;_LL1F3:;}{
# 2184
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple18*)ts->hd)).f2;
if(_tmp4E0 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp4E0,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp9BD;void*_tmp9BC[3];struct Cyc_String_pa_PrintArg_struct _tmp9BB;struct Cyc_String_pa_PrintArg_struct _tmp9BA;const char*_tmp9B9;struct Cyc_String_pa_PrintArg_struct _tmp9B8;(_tmp9B8.tag=0,((_tmp9B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2195
e->topt == 0?(struct _dyneither_ptr)((_tmp9B9="?",_tag_dyneither(_tmp9B9,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp9BA.tag=0,((_tmp9BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2194
Cyc_Absynpp_typ2string(t)),((_tmp9BB.tag=0,((_tmp9BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9BC[0]=& _tmp9BB,((_tmp9BC[1]=& _tmp9BA,((_tmp9BC[2]=& _tmp9B8,Cyc_Tcutil_terr(e->loc,((_tmp9BD="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp9BD,sizeof(char),82))),_tag_dyneither(_tmp9BC,sizeof(void*),3)))))))))))))))))));}
# 2196
Cyc_Tcutil_explain_failure();}}
# 2199
if(es != 0){
const char*_tmp9C1;void*_tmp9C0[1];struct Cyc_String_pa_PrintArg_struct _tmp9BF;void*_tmp4EE=(_tmp9BF.tag=0,((_tmp9BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2202
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9C0[0]=& _tmp9BF,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp9C1="too many arguments for datatype constructor %s",_tag_dyneither(_tmp9C1,sizeof(char),47))),_tag_dyneither(_tmp9C0,sizeof(void*),1)))))));_npop_handler(0);return _tmp4EE;}
if(ts != 0){
const char*_tmp9C5;void*_tmp9C4[1];struct Cyc_String_pa_PrintArg_struct _tmp9C3;void*_tmp4F2=(_tmp9C3.tag=0,((_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9C4[0]=& _tmp9C3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9C5="too few arguments for datatype constructor %s",_tag_dyneither(_tmp9C5,sizeof(char),46))),_tag_dyneither(_tmp9C4,sizeof(void*),1)))))));_npop_handler(0);return _tmp4F2;}{
void*_tmp4F3=res;_npop_handler(0);return _tmp4F3;};};}
# 2171
;_pop_region(rgn);}
# 2210
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp4F8=Cyc_Tcutil_compress(t);void*_tmp4F9=_tmp4F8;union Cyc_Absyn_Constraint*_tmp500;void*_tmp504;struct Cyc_List_List*_tmp507;union Cyc_Absyn_AggrInfoU _tmp509;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*_tmp50E;_LL1F9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FA->tag != 0)goto _LL1FB;}_LL1FA:
 return 1;_LL1FB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4F9;if(_tmp4FB->tag != 1)goto _LL1FD;}_LL1FC:
 goto _LL1FE;_LL1FD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FC->tag != 2)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FD->tag != 3)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FE->tag != 4)goto _LL203;}_LL202:
 return 0;_LL203: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FF->tag != 5)goto _LL205;else{_tmp500=((_tmp4FF->f1).ptr_atts).nullable;}}_LL204:
# 2218
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp500);_LL205: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F9;if(_tmp501->tag != 6)goto _LL207;}_LL206:
 goto _LL208;_LL207: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp502=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4F9;if(_tmp502->tag != 7)goto _LL209;}_LL208:
 return 1;_LL209: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F9;if(_tmp503->tag != 8)goto _LL20B;else{_tmp504=(_tmp503->f1).elt_type;}}_LL20A:
 return Cyc_Tcexp_zeroable_type(_tmp504);_LL20B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4F9;if(_tmp505->tag != 9)goto _LL20D;}_LL20C:
 return 0;_LL20D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F9;if(_tmp506->tag != 10)goto _LL20F;else{_tmp507=_tmp506->f1;}}_LL20E:
# 2224
 for(0;(unsigned int)_tmp507;_tmp507=_tmp507->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple18*)_tmp507->hd)).f2))return 0;}
return 1;_LL20F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp508=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F9;if(_tmp508->tag != 11)goto _LL211;else{_tmp509=(_tmp508->f1).aggr_info;_tmp50A=(_tmp508->f1).targs;}}_LL210: {
# 2228
struct Cyc_Absyn_Aggrdecl*_tmp51C=Cyc_Absyn_get_known_aggrdecl(_tmp509);
if(_tmp51C->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp51D=_new_region("r");struct _RegionHandle*r=& _tmp51D;_push_region(r);
{struct Cyc_List_List*_tmp51E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp51C->tvs,_tmp50A);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51C->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp51E,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp51F=0;_npop_handler(0);return _tmp51F;}}}{
int _tmp520=1;_npop_handler(0);return _tmp520;};}
# 2232
;_pop_region(r);};}_LL211: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4F9;if(_tmp50B->tag != 13)goto _LL213;}_LL212:
# 2237
 return 1;_LL213: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp50C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F9;if(_tmp50C->tag != 19)goto _LL215;}_LL214:
 return 1;_LL215: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F9;if(_tmp50D->tag != 12)goto _LL217;else{_tmp50E=_tmp50D->f2;}}_LL216:
# 2240
 for(0;_tmp50E != 0;_tmp50E=_tmp50E->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp50E->hd)->type))return 0;}
return 1;_LL217: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4F9;if(_tmp50F->tag != 14)goto _LL219;}_LL218:
 return 1;_LL219: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4F9;if(_tmp510->tag != 17)goto _LL21B;}_LL21A:
 return 0;_LL21B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4F9;if(_tmp511->tag != 16)goto _LL21D;}_LL21C:
 return 0;_LL21D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F9;if(_tmp512->tag != 15)goto _LL21F;}_LL21E:
 return 0;_LL21F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4F9;if(_tmp513->tag != 26)goto _LL221;}_LL220:
# 2248
 goto _LL222;_LL221: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp4F9;if(_tmp514->tag != 27)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F9;if(_tmp515->tag != 18)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4F9;if(_tmp516->tag != 20)goto _LL227;}_LL226:
 goto _LL228;_LL227: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4F9;if(_tmp517->tag != 21)goto _LL229;}_LL228:
 goto _LL22A;_LL229: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp518=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4F9;if(_tmp518->tag != 22)goto _LL22B;}_LL22A:
 goto _LL22C;_LL22B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4F9;if(_tmp519->tag != 23)goto _LL22D;}_LL22C:
 goto _LL22E;_LL22D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4F9;if(_tmp51A->tag != 24)goto _LL22F;}_LL22E:
 goto _LL230;_LL22F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4F9;if(_tmp51B->tag != 25)goto _LL1F8;}_LL230: {
const char*_tmp9C9;void*_tmp9C8[1];struct Cyc_String_pa_PrintArg_struct _tmp9C7;(_tmp9C7.tag=0,((_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9C8[0]=& _tmp9C7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C9="bad type `%s' in zeroable type",_tag_dyneither(_tmp9C9,sizeof(char),31))),_tag_dyneither(_tmp9C8,sizeof(void*),1)))))));}_LL1F8:;}
# 2260
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2262
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2264
if(topt != 0){
void*_tmp524=Cyc_Tcutil_compress(*topt);void*_tmp525=_tmp524;void*_tmp527;_LL232: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp526=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp525;if(_tmp526->tag != 5)goto _LL234;else{_tmp527=(_tmp526->f1).elt_typ;}}_LL233:
# 2267
 Cyc_Tcutil_unify(_tmp527,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL231;_LL234:;_LL235:
 goto _LL231;_LL231:;}{
# 2273
const char*_tmp9D2;void*_tmp9D1[2];const char*_tmp9D0;const char*_tmp9CF;struct Cyc_String_pa_PrintArg_struct _tmp9CE;struct Cyc_String_pa_PrintArg_struct _tmp9CD;(_tmp9CD.tag=0,((_tmp9CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp9CE.tag=0,((_tmp9CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp9CF="calloc",_tag_dyneither(_tmp9CF,sizeof(char),7))):(struct _dyneither_ptr)((_tmp9D0="malloc",_tag_dyneither(_tmp9D0,sizeof(char),7))))),((_tmp9D1[0]=& _tmp9CE,((_tmp9D1[1]=& _tmp9CD,Cyc_Tcutil_terr(loc,((_tmp9D2="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp9D2,sizeof(char),60))),_tag_dyneither(_tmp9D1,sizeof(void*),2)))))))))))));};}
# 2277
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2279
enum Cyc_Absyn_AliasQual _tmp52E=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;switch(_tmp52E){case Cyc_Absyn_Unique: _LL236:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL237:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2285
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2290
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2293
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9D5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9D4;void*expected_type=(void*)(
(_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=15,((_tmp9D5.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp9D5)))),_tmp9D4))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp52F=Cyc_Tcutil_compress(handle_type);void*_tmp530=_tmp52F;void*_tmp532;_LL23A: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp531=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp530;if(_tmp531->tag != 15)goto _LL23C;else{_tmp532=(void*)_tmp531->f1;}}_LL23B:
# 2298
 rgn=_tmp532;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL239;_LL23C:;_LL23D:
# 2302
{const char*_tmp9D9;void*_tmp9D8[1];struct Cyc_String_pa_PrintArg_struct _tmp9D7;(_tmp9D7.tag=0,((_tmp9D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9D8[0]=& _tmp9D7,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp9D9="expecting region_t type but found %s",_tag_dyneither(_tmp9D9,sizeof(char),37))),_tag_dyneither(_tmp9D8,sizeof(void*),1)))))));}
goto _LL239;_LL239:;}else{
# 2309
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2316
{struct _RegionHandle _tmp538=_new_region("r");struct _RegionHandle*r=& _tmp538;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2327 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp9DC;void*_tmp9DB;(_tmp9DB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9DC="calloc with empty type",_tag_dyneither(_tmp9DC,sizeof(char),23))),_tag_dyneither(_tmp9DB,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2339
void*er=(*e)->r;
retry_sizeof: {
void*_tmp53B=er;void*_tmp53D;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp*_tmp540;_LL23F: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp53C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp53B;if(_tmp53C->tag != 16)goto _LL241;else{_tmp53D=(void*)_tmp53C->f1;}}_LL240:
# 2343
 elt_type=_tmp53D;
{void**_tmp9DD;*t=((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=elt_type,_tmp9DD))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL23E;_LL241: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp53E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp53B;if(_tmp53E->tag != 2)goto _LL243;else{if(_tmp53E->f1 != Cyc_Absyn_Times)goto _LL243;if(_tmp53E->f2 == 0)goto _LL243;_tmp53F=(struct Cyc_Absyn_Exp*)(_tmp53E->f2)->hd;if((_tmp53E->f2)->tl == 0)goto _LL243;_tmp540=(struct Cyc_Absyn_Exp*)((_tmp53E->f2)->tl)->hd;if(((_tmp53E->f2)->tl)->tl != 0)goto _LL243;}}_LL242:
# 2350
{struct _tuple0 _tmp9DE;struct _tuple0 _tmp542=(_tmp9DE.f1=_tmp53F->r,((_tmp9DE.f2=_tmp540->r,_tmp9DE)));struct _tuple0 _tmp543=_tmp542;void*_tmp545;void*_tmp547;_LL246: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp544=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp543.f1;if(_tmp544->tag != 16)goto _LL248;else{_tmp545=(void*)_tmp544->f1;}}_LL247:
# 2352
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp545);
elt_type=_tmp545;
{void**_tmp9DF;*t=((_tmp9DF=_cycalloc(sizeof(*_tmp9DF)),((_tmp9DF[0]=elt_type,_tmp9DF))));}
num_elts=_tmp540;
one_elt=0;
goto _LL245;_LL248: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp546=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp543.f2;if(_tmp546->tag != 16)goto _LL24A;else{_tmp547=(void*)_tmp546->f1;}}_LL249:
# 2359
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp547);
elt_type=_tmp547;
{void**_tmp9E0;*t=((_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0[0]=elt_type,_tmp9E0))));}
num_elts=_tmp53F;
one_elt=0;
goto _LL245;_LL24A:;_LL24B:
 goto No_sizeof;_LL245:;}
# 2367
goto _LL23E;_LL243:;_LL244:
# 2369
 No_sizeof: {
# 2372
struct Cyc_Absyn_Exp*_tmp54B=*e;
{void*_tmp54C=_tmp54B->r;void*_tmp54D=_tmp54C;struct Cyc_Absyn_Exp*_tmp54F;_LL24D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp54E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp54E->tag != 13)goto _LL24F;else{_tmp54F=_tmp54E->f2;}}_LL24E:
 _tmp54B=_tmp54F;goto _LL24C;_LL24F:;_LL250:
 goto _LL24C;_LL24C:;}
# 2377
{void*_tmp550=Cyc_Tcutil_compress((void*)_check_null(_tmp54B->topt));void*_tmp551=_tmp550;void*_tmp553;_LL252: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp551;if(_tmp552->tag != 19)goto _LL254;else{_tmp553=(void*)_tmp552->f1;}}_LL253:
# 2379
{void*_tmp554=Cyc_Tcutil_compress(_tmp553);void*_tmp555=_tmp554;struct Cyc_Absyn_Exp*_tmp557;_LL257: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp555;if(_tmp556->tag != 18)goto _LL259;else{_tmp557=_tmp556->f1;}}_LL258:
# 2381
 er=_tmp557->r;goto retry_sizeof;_LL259:;_LL25A:
 goto _LL256;_LL256:;}
# 2384
goto _LL251;_LL254:;_LL255:
 goto _LL251;_LL251:;}
# 2387
elt_type=Cyc_Absyn_char_typ;
{void**_tmp9E1;*t=((_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1[0]=elt_type,_tmp9E1))));}
num_elts=*e;
one_elt=0;}
# 2392
goto _LL23E;_LL23E:;}}
# 2396
*is_fat=!one_elt;
# 2399
{void*_tmp559=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp55B;_LL25C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp559;if(_tmp55A->tag != 11)goto _LL25E;else{if((((_tmp55A->f1).aggr_info).KnownAggr).tag != 2)goto _LL25E;_tmp55B=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp55A->f1).aggr_info).KnownAggr).val);}}_LL25D:
# 2401
 if(_tmp55B->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55B->impl))->exist_vars != 0){
const char*_tmp9E4;void*_tmp9E3;(_tmp9E3=0,Cyc_Tcutil_terr(loc,((_tmp9E4="malloc with existential types not yet implemented",_tag_dyneither(_tmp9E4,sizeof(char),50))),_tag_dyneither(_tmp9E3,sizeof(void*),0)));}
goto _LL25B;_LL25E:;_LL25F:
 goto _LL25B;_LL25B:;}{
# 2408
void*(*_tmp55E)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp55F=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp560=Cyc_Tcutil_compress(*topt);void*_tmp561=_tmp560;union Cyc_Absyn_Constraint*_tmp563;union Cyc_Absyn_Constraint*_tmp564;union Cyc_Absyn_Constraint*_tmp565;_LL261: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp561;if(_tmp562->tag != 5)goto _LL263;else{_tmp563=((_tmp562->f1).ptr_atts).nullable;_tmp564=((_tmp562->f1).ptr_atts).bounds;_tmp565=((_tmp562->f1).ptr_atts).zero_term;}}_LL262:
# 2413
 _tmp55F=_tmp565;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp563))
_tmp55E=Cyc_Absyn_star_typ;
# 2418
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp565) && !(*is_calloc)){
{const char*_tmp9E7;void*_tmp9E6;(_tmp9E6=0,Cyc_Tcutil_warn(loc,((_tmp9E7="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp9E7,sizeof(char),55))),_tag_dyneither(_tmp9E6,sizeof(void*),0)));}
*is_calloc=1;}
# 2424
{void*_tmp568=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp564);void*_tmp569=_tmp568;struct Cyc_Absyn_Exp*_tmp56C;_LL266: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp56A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp569;if(_tmp56A->tag != 0)goto _LL268;}_LL267:
 goto _LL265;_LL268:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp56B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp569;if(_tmp56B->tag != 1)goto _LL26A;else{_tmp56C=_tmp56B->f1;}}if(!(!one_elt))goto _LL26A;_LL269: {
# 2427
int _tmp56D=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp56D  && Cyc_Evexp_same_const_exp(_tmp56C,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp564),_tmp55F);}
# 2433
{void*_tmp56E=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp56F=_tmp56E;void*_tmp571;_LL26D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp56F;if(_tmp570->tag != 19)goto _LL26F;else{_tmp571=(void*)_tmp570->f1;}}_LL26E: {
# 2435
struct Cyc_Absyn_Exp*_tmp572=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp571,0),0,Cyc_Absyn_No_coercion,0);
# 2437
if(Cyc_Evexp_same_const_exp(_tmp572,_tmp56C)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp564),_tmp55F);}
# 2442
goto _LL26C;}_LL26F:;_LL270:
 goto _LL26C;_LL26C:;}
# 2445
goto _LL265;}_LL26A:;_LL26B:
 goto _LL265;_LL265:;}
# 2448
goto _LL260;_LL263:;_LL264:
 goto _LL260;_LL260:;}
# 2451
if(!one_elt)_tmp55E=Cyc_Absyn_dyneither_typ;
return _tmp55E(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp55F);};};}
# 2456
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2462
struct _RegionHandle _tmp573=_new_region("r");struct _RegionHandle*r=& _tmp573;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp574=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExpNoPromote(_tmp574,0,e1);{
void*_tmp575=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp574,& _tmp575,e2);};}{
# 2468
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2471
{void*_tmp576=Cyc_Tcutil_compress(t1);void*_tmp577=_tmp576;_LL272: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp578=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp577;if(_tmp578->tag != 8)goto _LL274;}_LL273:
{const char*_tmp9EA;void*_tmp9E9;(_tmp9E9=0,Cyc_Tcutil_terr(loc,((_tmp9EA="cannot assign to an array",_tag_dyneither(_tmp9EA,sizeof(char),26))),_tag_dyneither(_tmp9E9,sizeof(void*),0)));}goto _LL271;_LL274:;_LL275:
 goto _LL271;_LL271:;}
# 2476
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1)){
const char*_tmp9ED;void*_tmp9EC;(_tmp9EC=0,Cyc_Tcutil_terr(loc,((_tmp9ED="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp9ED,sizeof(char),58))),_tag_dyneither(_tmp9EC,sizeof(void*),0)));}
# 2480
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp9F0;void*_tmp9EF;void*_tmp57F=(_tmp9EF=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9F0="swap non-lvalue",_tag_dyneither(_tmp9F0,sizeof(char),16))),_tag_dyneither(_tmp9EF,sizeof(void*),0)));_npop_handler(0);return _tmp57F;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp9F3;void*_tmp9F2;void*_tmp582=(_tmp9F2=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9F3="swap non-lvalue",_tag_dyneither(_tmp9F3,sizeof(char),16))),_tag_dyneither(_tmp9F2,sizeof(void*),0)));_npop_handler(0);return _tmp582;}{
# 2485
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9F6;void*_tmp9F5;void*_tmp585=(_tmp9F5=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9F6="swap value in zeroterm array",_tag_dyneither(_tmp9F6,sizeof(char),29))),_tag_dyneither(_tmp9F5,sizeof(void*),0)));_npop_handler(0);return _tmp585;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9F9;void*_tmp9F8;void*_tmp588=(_tmp9F8=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9F9="swap value in zeroterm array",_tag_dyneither(_tmp9F9,sizeof(char),29))),_tag_dyneither(_tmp9F8,sizeof(void*),0)));_npop_handler(0);return _tmp588;}
# 2492
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp9FE;void*_tmp9FD[2];struct Cyc_String_pa_PrintArg_struct _tmp9FC;struct Cyc_String_pa_PrintArg_struct _tmp9FB;void*_tmp589=(_tmp9FB.tag=0,((_tmp9FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp9FC.tag=0,((_tmp9FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9FD[0]=& _tmp9FC,((_tmp9FD[1]=& _tmp9FB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9FE="type mismatch: %s != %s",_tag_dyneither(_tmp9FE,sizeof(char),24))),_tag_dyneither(_tmp9FD,sizeof(void*),2)))))))))))));
void*_tmp58A=_tmp589;_npop_handler(0);return _tmp58A;}{
# 2499
void*_tmp58F=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp58F;};};};
# 2462
;_pop_region(r);}
# 2503
int Cyc_Tcexp_in_stmt_exp=0;
# 2505
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp590=_new_region("r");struct _RegionHandle*r=& _tmp590;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2512
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2514
while(1){
void*_tmp591=s->r;void*_tmp592=_tmp591;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Stmt*_tmp596;struct Cyc_Absyn_Stmt*_tmp597;struct Cyc_Absyn_Decl*_tmp599;struct Cyc_Absyn_Stmt*_tmp59A;_LL277: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp593=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp592;if(_tmp593->tag != 1)goto _LL279;else{_tmp594=_tmp593->f1;}}_LL278: {
# 2518
void*_tmp59B=(void*)_check_null(_tmp594->topt);
if(!Cyc_Tcutil_unify(_tmp59B,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmpA02;void*_tmpA01[1];struct Cyc_String_pa_PrintArg_struct _tmpA00;(_tmpA00.tag=0,((_tmpA00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp59B)),((_tmpA01[0]=& _tmpA00,Cyc_Tcutil_terr(loc,((_tmpA02="statement expression returns type %s",_tag_dyneither(_tmpA02,sizeof(char),37))),_tag_dyneither(_tmpA01,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2524
void*_tmp59F=_tmp59B;_npop_handler(0);return _tmp59F;};}_LL279: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp595=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp592;if(_tmp595->tag != 2)goto _LL27B;else{_tmp596=_tmp595->f1;_tmp597=_tmp595->f2;}}_LL27A:
 s=_tmp597;continue;_LL27B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp598=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp592;if(_tmp598->tag != 12)goto _LL27D;else{_tmp599=_tmp598->f1;_tmp59A=_tmp598->f2;}}_LL27C:
 s=_tmp59A;continue;_LL27D:;_LL27E: {
# 2528
const char*_tmpA05;void*_tmpA04;void*_tmp5A2=(_tmpA04=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmpA05="statement expression must end with expression",_tag_dyneither(_tmpA05,sizeof(char),46))),_tag_dyneither(_tmpA04,sizeof(void*),0)));_npop_handler(0);return _tmp5A2;}_LL276:;}
# 2506
;_pop_region(r);}
# 2533
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp5A3=_new_region("r");struct _RegionHandle*r=& _tmp5A3;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2538
{void*_tmp5A4=t;struct Cyc_Absyn_Aggrdecl*_tmp5A6;_LL280: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5A4;if(_tmp5A5->tag != 11)goto _LL282;else{if((((_tmp5A5->f1).aggr_info).KnownAggr).tag != 2)goto _LL282;_tmp5A6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp5A5->f1).aggr_info).KnownAggr).val);}}_LL281:
# 2540
 if((_tmp5A6->kind == Cyc_Absyn_UnionA  && _tmp5A6->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5A6->impl))->tagged)goto _LL27F;
goto _LL283;_LL282:;_LL283:
# 2543
{const char*_tmpA09;void*_tmpA08[1];struct Cyc_String_pa_PrintArg_struct _tmpA07;(_tmpA07.tag=0,((_tmpA07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA08[0]=& _tmpA07,Cyc_Tcutil_terr(loc,((_tmpA09="expecting @tagged union but found %s",_tag_dyneither(_tmpA09,sizeof(char),37))),_tag_dyneither(_tmpA08,sizeof(void*),1)))))));}
goto _LL27F;_LL27F:;}{
# 2546
void*_tmp5AA=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp5AA;};
# 2535
;_pop_region(r);}
# 2550
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2554
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp5AB=_new_region("r");struct _RegionHandle*r=& _tmp5AB;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp5AC=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2559
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA0C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA0B;void*expected_type=(void*)(
(_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0C.tag=15,((_tmpA0C.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp5AC)),_tmpA0C)))),_tmpA0B))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp5AC,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp5AD=Cyc_Tcutil_compress(handle_type);void*_tmp5AE=_tmp5AD;void*_tmp5B0;_LL285: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5AE;if(_tmp5AF->tag != 15)goto _LL287;else{_tmp5B0=(void*)_tmp5AF->f1;}}_LL286:
# 2564
 rgn=_tmp5B0;
Cyc_Tcenv_check_rgn_accessible(_tmp5AC,loc,rgn);
goto _LL284;_LL287:;_LL288:
# 2568
{const char*_tmpA10;void*_tmpA0F[1];struct Cyc_String_pa_PrintArg_struct _tmpA0E;(_tmpA0E.tag=0,((_tmpA0E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmpA0F[0]=& _tmpA0E,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmpA10="expecting region_t type but found %s",_tag_dyneither(_tmpA10,sizeof(char),37))),_tag_dyneither(_tmpA0F,sizeof(void*),1)))))));}
goto _LL284;_LL284:;}else{
# 2575
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2583
void*_tmp5B6=e1->r;void*_tmp5B7=_tmp5B6;struct Cyc_Core_Opt*_tmp5BB;struct Cyc_List_List*_tmp5BC;struct Cyc_List_List*_tmp5BE;_LL28A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5B8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5B7;if(_tmp5B8->tag != 26)goto _LL28C;}_LL28B:
 goto _LL28D;_LL28C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5B9=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5B7;if(_tmp5B9->tag != 27)goto _LL28E;}_LL28D: {
# 2588
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp5AC,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmpA13;void*_tmpA12;(_tmpA12=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA13="tcNew: comprehension returned non-array type",_tag_dyneither(_tmpA13,sizeof(char),45))),_tag_dyneither(_tmpA12,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5AC,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5AC,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2599
void*_tmp5C3=res_typ;_npop_handler(0);return _tmp5C3;};}_LL28E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5BA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5B7;if(_tmp5BA->tag != 35)goto _LL290;else{_tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BA->f2;}}_LL28F:
# 2601
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpA16;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpA15;e1->r=(void*)((_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15[0]=((_tmpA16.tag=25,((_tmpA16.f1=_tmp5BC,_tmpA16)))),_tmpA15))));}
_tmp5BE=_tmp5BC;goto _LL291;_LL290: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5BD=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5B7;if(_tmp5BD->tag != 25)goto _LL292;else{_tmp5BE=_tmp5BD->f1;}}_LL291: {
# 2604
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp5C6=Cyc_Tcutil_compress(*topt);void*_tmp5C7=_tmp5C6;void**_tmp5C9;struct Cyc_Absyn_Tqual _tmp5CA;union Cyc_Absyn_Constraint*_tmp5CB;_LL299: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7;if(_tmp5C8->tag != 5)goto _LL29B;else{_tmp5C9=(void**)&(_tmp5C8->f1).elt_typ;_tmp5CA=(_tmp5C8->f1).elt_tq;_tmp5CB=((_tmp5C8->f1).ptr_atts).zero_term;}}_LL29A:
# 2610
 elt_typ_opt=_tmp5C9;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5CB);
goto _LL298;_LL29B:;_LL29C:
 goto _LL298;_LL298:;}{
# 2616
void*res_typ=Cyc_Tcexp_tcArray(_tmp5AC,e1->loc,elt_typ_opt,0,zero_term,_tmp5BE);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmpA19;void*_tmpA18;(_tmpA18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA19="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmpA19,sizeof(char),50))),_tag_dyneither(_tmpA18,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2625
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5AC,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5AC,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2631
void*_tmp5CE=res_typ;_npop_handler(0);return _tmp5CE;};};}_LL292: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5BF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B7;if(_tmp5BF->tag != 0)goto _LL294;else{if(((_tmp5BF->f1).String_c).tag != 8)goto _LL294;}}_LL293: {
# 2636
void*_tmp5CF=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2638
void*_tmp5D0=Cyc_Tcexp_tcExp(_tmp5AC,& _tmp5CF,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA1C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA1B;void*_tmp5D3=Cyc_Absyn_atb_typ(_tmp5D0,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B[0]=((_tmpA1C.tag=1,((_tmpA1C.f1=Cyc_Absyn_uint_exp(1,0),_tmpA1C)))),_tmpA1B)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5D3;}_LL294: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5C0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B7;if(_tmp5C0->tag != 0)goto _LL296;else{if(((_tmp5C0->f1).Wstring_c).tag != 9)goto _LL296;}}_LL295: {
# 2643
void*_tmp5D4=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2645
void*_tmp5D5=Cyc_Tcexp_tcExp(_tmp5AC,& _tmp5D4,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA1F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA1E;void*_tmp5D8=Cyc_Absyn_atb_typ(_tmp5D5,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=((_tmpA1F.tag=1,((_tmpA1F.f1=Cyc_Absyn_uint_exp(1,0),_tmpA1F)))),_tmpA1E)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5D8;}_LL296:;_LL297:
# 2652
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp5D9=Cyc_Tcutil_compress(*topt);void*_tmp5DA=_tmp5D9;void**_tmp5DC;struct Cyc_Absyn_Tqual _tmp5DD;_LL29E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DA;if(_tmp5DB->tag != 5)goto _LL2A0;else{_tmp5DC=(void**)&(_tmp5DB->f1).elt_typ;_tmp5DD=(_tmp5DB->f1).elt_tq;}}_LL29F:
# 2658
 topt2=_tmp5DC;goto _LL29D;_LL2A0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5DA;if(_tmp5DE->tag != 3)goto _LL2A2;}_LL2A1:
# 2662
 bogus=*topt;
topt2=& bogus;
goto _LL29D;_LL2A2:;_LL2A3:
 goto _LL29D;_LL29D:;}{
# 2668
void*telt=Cyc_Tcexp_tcExp(_tmp5AC,topt2,e1);
# 2670
struct _RegionHandle _tmp5DF=_new_region("temp");struct _RegionHandle*temp=& _tmp5DF;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1)){
const char*_tmpA22;void*_tmpA21;(_tmpA21=0,Cyc_Tcutil_terr(e1->loc,((_tmpA22="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA22,sizeof(char),49))),_tag_dyneither(_tmpA21,sizeof(void*),0)));}{
# 2675
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA28;struct Cyc_Absyn_PtrInfo _tmpA27;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA26;void*res_typ=(void*)(
(_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26[0]=((_tmpA28.tag=5,((_tmpA28.f1=((_tmpA27.elt_typ=telt,((_tmpA27.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmpA27.ptr_atts=(
((_tmpA27.ptr_atts).rgn=rgn,(((_tmpA27.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpA27.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmpA27.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmpA27.ptr_atts).ptrloc=0,_tmpA27.ptr_atts)))))))))),_tmpA27)))))),_tmpA28)))),_tmpA26))));
# 2680
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5AC,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5AC,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2687
void*_tmp5E2=res_typ;_npop_handler(1);return _tmp5E2;};};
# 2670
;_pop_region(temp);};}_LL289:;};}
# 2556
;_pop_region(r);}
# 2694
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
e->topt=(t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0));
return t;}
# 2704
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2708
struct _RegionHandle _tmp5E6=_new_region("temp");struct _RegionHandle*temp=& _tmp5E6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2714
const char*_tmpA2B;void*_tmpA2A;(_tmpA2A=0,Cyc_Tcutil_terr(e->loc,((_tmpA2B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA2B,sizeof(char),49))),_tag_dyneither(_tmpA2A,sizeof(void*),0)));}{
# 2717
void*_tmp5E9=e->r;void*_tmp5EA=_tmp5E9;_LL2A5: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EB->tag != 25)goto _LL2A7;}_LL2A6:
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5EC=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EC->tag != 26)goto _LL2A9;}_LL2A8:
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5ED=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5ED->tag != 27)goto _LL2AB;}_LL2AA:
 goto _LL2AC;_LL2AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5EE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EE->tag != 0)goto _LL2AD;else{if(((_tmp5EE->f1).Wstring_c).tag != 9)goto _LL2AD;}}_LL2AC:
 goto _LL2AE;_LL2AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5EF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EF->tag != 0)goto _LL2AF;else{if(((_tmp5EF->f1).String_c).tag != 8)goto _LL2AF;}}_LL2AE: {
void*_tmp5F0=t;_npop_handler(0);return _tmp5F0;}_LL2AF:;_LL2B0:
# 2724
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2727
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}{
# 2730
void*_tmp5F1=t;_npop_handler(0);return _tmp5F1;};_LL2A4:;};
# 2708
;_pop_region(temp);}
# 2741 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp5F2=e->r;void*_tmp5F3=_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F5;_LL2B2: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5F4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp5F4->tag != 11)goto _LL2B4;else{_tmp5F5=_tmp5F4->f1;}}_LL2B3:
# 2745
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5F5);
_tmp5F5->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5F5->topt),0);
e->topt=_tmp5F5->topt;
goto _LL2B1;_LL2B4:;_LL2B5:
# 2751
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2754
{void*_tmp5F6=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5F7=_tmp5F6;void*_tmp5F9;struct Cyc_Absyn_Tqual _tmp5FA;void*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;union Cyc_Absyn_Constraint*_tmp5FD;union Cyc_Absyn_Constraint*_tmp5FE;_LL2B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 5)goto _LL2B9;else{_tmp5F9=(_tmp5F8->f1).elt_typ;_tmp5FA=(_tmp5F8->f1).elt_tq;_tmp5FB=((_tmp5F8->f1).ptr_atts).rgn;_tmp5FC=((_tmp5F8->f1).ptr_atts).nullable;_tmp5FD=((_tmp5F8->f1).ptr_atts).bounds;_tmp5FE=((_tmp5F8->f1).ptr_atts).zero_term;}}_LL2B8:
# 2756
{void*_tmp5FF=Cyc_Tcutil_compress(_tmp5F9);void*_tmp600=_tmp5FF;struct Cyc_List_List*_tmp602;void*_tmp603;struct Cyc_Absyn_Tqual _tmp604;void*_tmp605;struct Cyc_List_List*_tmp606;int _tmp607;struct Cyc_Absyn_VarargInfo*_tmp608;struct Cyc_List_List*_tmp609;struct Cyc_List_List*_tmp60A;struct Cyc_Absyn_Exp*_tmp60B;struct Cyc_List_List*_tmp60C;struct Cyc_Absyn_Exp*_tmp60D;struct Cyc_List_List*_tmp60E;_LL2BC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp600;if(_tmp601->tag != 9)goto _LL2BE;else{_tmp602=(_tmp601->f1).tvars;_tmp603=(_tmp601->f1).effect;_tmp604=(_tmp601->f1).ret_tqual;_tmp605=(_tmp601->f1).ret_typ;_tmp606=(_tmp601->f1).args;_tmp607=(_tmp601->f1).c_varargs;_tmp608=(_tmp601->f1).cyc_varargs;_tmp609=(_tmp601->f1).rgn_po;_tmp60A=(_tmp601->f1).attributes;_tmp60B=(_tmp601->f1).requires_clause;_tmp60C=(_tmp601->f1).requires_relns;_tmp60D=(_tmp601->f1).ensures_clause;_tmp60E=(_tmp601->f1).ensures_relns;}}_LL2BD:
# 2758
 if(_tmp602 != 0){
struct _RegionHandle _tmp60F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp60F;_push_region(rgn);
{struct _tuple12 _tmpA2C;struct _tuple12 _tmp610=(_tmpA2C.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmpA2C.f2=rgn,_tmpA2C)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp610,_tmp602);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2767
_tmp609=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp609);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp609);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA32;struct Cyc_Absyn_FnInfo _tmpA31;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA30;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30[0]=((_tmpA32.tag=9,((_tmpA32.f1=((_tmpA31.tvars=0,((_tmpA31.effect=_tmp603,((_tmpA31.ret_tqual=_tmp604,((_tmpA31.ret_typ=_tmp605,((_tmpA31.args=_tmp606,((_tmpA31.c_varargs=_tmp607,((_tmpA31.cyc_varargs=_tmp608,((_tmpA31.rgn_po=0,((_tmpA31.attributes=_tmp60A,((_tmpA31.requires_clause=_tmp60B,((_tmpA31.requires_relns=_tmp60C,((_tmpA31.ensures_clause=_tmp60D,((_tmpA31.ensures_relns=_tmp60E,_tmpA31)))))))))))))))))))))))))),_tmpA32)))),_tmpA30)))));
# 2774
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA38;struct Cyc_Absyn_PtrInfo _tmpA37;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA36;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp611=(_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36[0]=((_tmpA38.tag=5,((_tmpA38.f1=((_tmpA37.elt_typ=ftyp,((_tmpA37.elt_tq=_tmp5FA,((_tmpA37.ptr_atts=(((_tmpA37.ptr_atts).rgn=_tmp5FB,(((_tmpA37.ptr_atts).nullable=_tmp5FC,(((_tmpA37.ptr_atts).bounds=_tmp5FD,(((_tmpA37.ptr_atts).zero_term=_tmp5FE,(((_tmpA37.ptr_atts).ptrloc=0,_tmpA37.ptr_atts)))))))))),_tmpA37)))))),_tmpA38)))),_tmpA36)));
# 2776
struct Cyc_Absyn_Exp*_tmp612=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpA3B;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA3A;e->r=(void*)((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=12,((_tmpA3B.f1=_tmp612,((_tmpA3B.f2=ts,_tmpA3B)))))),_tmpA3A))));}
e->topt=(void*)_tmp611;};}
# 2760
;_pop_region(rgn);}
# 2781
goto _LL2BB;_LL2BE:;_LL2BF:
 goto _LL2BB;_LL2BB:;}
# 2784
goto _LL2B6;_LL2B9:;_LL2BA:
 goto _LL2B6;_LL2B6:;}
# 2787
goto _LL2B1;_LL2B1:;}
# 2789
return(void*)_check_null(e->topt);}
# 2797
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2799
struct _RegionHandle _tmp61C=_new_region("r");struct _RegionHandle*r=& _tmp61C;_push_region(r);
{struct Cyc_List_List*_tmp61D=0;
# 2802
{void*_tmp61E=fn_exp->r;void*_tmp61F=_tmp61E;struct Cyc_List_List*_tmp621;_LL2C1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp620=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp61F;if(_tmp620->tag != 9)goto _LL2C3;else{_tmp621=_tmp620->f2;}}_LL2C2:
# 2804
{void*_tmp622=e->r;void*_tmp623=_tmp622;struct Cyc_List_List*_tmp625;_LL2C6: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp624=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp623;if(_tmp624->tag != 9)goto _LL2C8;else{_tmp625=_tmp624->f2;}}_LL2C7: {
# 2806
struct Cyc_List_List*_tmp626=alias_arg_exps;
int _tmp627=0;
while(_tmp626 != 0){
while(_tmp627 != (int)_tmp626->hd){
if(_tmp621 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmpA45;struct Cyc_Int_pa_PrintArg_struct _tmpA44;void*_tmpA43[2];const char*_tmpA42;void*_tmpA41;(_tmpA41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA45.tag=1,((_tmpA45.f1=(unsigned long)((int)_tmp626->hd),((_tmpA44.tag=1,((_tmpA44.f1=(unsigned long)_tmp627,((_tmpA43[0]=& _tmpA44,((_tmpA43[1]=& _tmpA45,Cyc_aprintf(((_tmpA42="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA42,sizeof(char),36))),_tag_dyneither(_tmpA43,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA41,sizeof(void*),0)));}
# 2813
++ _tmp627;
_tmp621=_tmp621->tl;
_tmp625=((struct Cyc_List_List*)_check_null(_tmp625))->tl;}{
# 2818
struct _tuple11 _tmp62D=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp621))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp625))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp62F;struct Cyc_Absyn_Exp*_tmp630;struct _tuple11 _tmp62E=_tmp62D;_tmp62F=_tmp62E.f1;_tmp630=_tmp62E.f2;
_tmp621->hd=(void*)_tmp630;
{struct Cyc_List_List*_tmpA46;_tmp61D=((_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46->hd=_tmp62F,((_tmpA46->tl=_tmp61D,_tmpA46))))));}
_tmp626=_tmp626->tl;};}
# 2823
goto _LL2C5;}_LL2C8:;_LL2C9: {
const char*_tmpA49;void*_tmpA48;(_tmpA48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA49="not a function call!",_tag_dyneither(_tmpA49,sizeof(char),21))),_tag_dyneither(_tmpA48,sizeof(void*),0)));}_LL2C5:;}
# 2826
goto _LL2C0;_LL2C3:;_LL2C4: {
const char*_tmpA4C;void*_tmpA4B;(_tmpA4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA4C="not a function call!",_tag_dyneither(_tmpA4C,sizeof(char),21))),_tag_dyneither(_tmpA4B,sizeof(void*),0)));}_LL2C0:;}
# 2831
while(_tmp61D != 0){
struct Cyc_Absyn_Decl*_tmp636=(struct Cyc_Absyn_Decl*)_tmp61D->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp636,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp61D=_tmp61D->tl;}
# 2838
e->topt=0;
e->r=fn_exp->r;}
# 2800
;_pop_region(r);}
# 2845
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2849
{void*_tmp637=e->r;void*_tmp638=_tmp637;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_List_List*_tmp63D;struct Cyc_Absyn_VarargCallInfo**_tmp63E;int*_tmp63F;struct Cyc_Core_Opt*_tmp641;struct Cyc_List_List*_tmp642;union Cyc_Absyn_Cnst*_tmp644;struct _tuple1*_tmp646;void**_tmp647;enum Cyc_Absyn_Primop _tmp649;struct Cyc_List_List*_tmp64A;struct Cyc_Absyn_Exp*_tmp64C;enum Cyc_Absyn_Incrementor _tmp64D;struct Cyc_Absyn_Exp*_tmp64F;struct Cyc_Core_Opt*_tmp650;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Exp*_tmp654;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp65B;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_Absyn_Exp*_tmp65E;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_List_List*_tmp661;struct Cyc_Absyn_VarargCallInfo**_tmp662;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_List_List*_tmp667;void*_tmp669;struct Cyc_Absyn_Exp*_tmp66A;enum Cyc_Absyn_Coercion*_tmp66B;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Exp**_tmp66F;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp672;void*_tmp674;void*_tmp676;struct Cyc_List_List*_tmp677;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_Absyn_Exp*_tmp67B;struct _dyneither_ptr*_tmp67C;int*_tmp67D;int*_tmp67E;struct Cyc_Absyn_Exp*_tmp680;struct _dyneither_ptr*_tmp681;int*_tmp682;int*_tmp683;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_List_List*_tmp688;struct _tuple9*_tmp68A;struct Cyc_List_List*_tmp68B;struct Cyc_List_List*_tmp68D;struct Cyc_Absyn_Stmt*_tmp68F;struct Cyc_Absyn_Vardecl*_tmp691;struct Cyc_Absyn_Exp*_tmp692;struct Cyc_Absyn_Exp*_tmp693;int*_tmp694;struct Cyc_Absyn_Exp*_tmp696;void*_tmp697;int*_tmp698;struct _tuple1**_tmp69A;struct Cyc_List_List**_tmp69B;struct Cyc_List_List*_tmp69C;struct Cyc_Absyn_Aggrdecl**_tmp69D;void*_tmp69F;struct Cyc_List_List*_tmp6A0;struct Cyc_List_List*_tmp6A2;struct Cyc_Absyn_Datatypedecl*_tmp6A3;struct Cyc_Absyn_Datatypefield*_tmp6A4;struct _tuple1**_tmp6A6;struct Cyc_Absyn_Enumdecl*_tmp6A7;struct Cyc_Absyn_Enumfield*_tmp6A8;struct _tuple1**_tmp6AA;void*_tmp6AB;struct Cyc_Absyn_Enumfield*_tmp6AC;int*_tmp6AE;struct Cyc_Absyn_Exp**_tmp6AF;void***_tmp6B0;struct Cyc_Absyn_Exp**_tmp6B1;int*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B7;struct _dyneither_ptr*_tmp6B8;void*_tmp6BA;_LL2CB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp639=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp639->tag != 11)goto _LL2CD;else{_tmp63A=_tmp639->f1;}}_LL2CC:
# 2854
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp63A);
return;_LL2CD:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp63B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp63B->tag != 9)goto _LL2CF;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63B->f2;_tmp63E=(struct Cyc_Absyn_VarargCallInfo**)& _tmp63B->f3;_tmp63F=(int*)& _tmp63B->f4;}}if(!(!(*_tmp63F)))goto _LL2CF;_LL2CE:
# 2860
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp63C,_tmp63D)){
*_tmp63F=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2864
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2867
return;_LL2CF: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp640=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp640->tag != 35)goto _LL2D1;else{_tmp641=_tmp640->f1;_tmp642=_tmp640->f2;}}_LL2D0:
# 2871
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp642);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2D1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp643=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp643->tag != 0)goto _LL2D3;else{_tmp644=(union Cyc_Absyn_Cnst*)& _tmp643->f1;}}_LL2D2:
# 2876
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp644,e);goto _LL2CA;_LL2D3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp645=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp645->tag != 1)goto _LL2D5;else{_tmp646=_tmp645->f1;_tmp647=(void**)((void**)& _tmp645->f2);}}_LL2D4:
# 2878
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp646,_tmp647);goto _LL2CA;_LL2D5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp648=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp648->tag != 2)goto _LL2D7;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;}}_LL2D6:
# 2880
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp649,_tmp64A);goto _LL2CA;_LL2D7: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp64B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp64B->tag != 4)goto _LL2D9;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;}}_LL2D8:
# 2882
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp64C,_tmp64D);goto _LL2CA;_LL2D9: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp64E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp64E->tag != 3)goto _LL2DB;else{_tmp64F=_tmp64E->f1;_tmp650=_tmp64E->f2;_tmp651=_tmp64E->f3;}}_LL2DA:
# 2884
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp64F,_tmp650,_tmp651);goto _LL2CA;_LL2DB: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp652=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp652->tag != 5)goto _LL2DD;else{_tmp653=_tmp652->f1;_tmp654=_tmp652->f2;_tmp655=_tmp652->f3;}}_LL2DC:
# 2886
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp653,_tmp654,_tmp655);goto _LL2CA;_LL2DD: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp656=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp656->tag != 6)goto _LL2DF;else{_tmp657=_tmp656->f1;_tmp658=_tmp656->f2;}}_LL2DE:
# 2888
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp657,_tmp658);goto _LL2CA;_LL2DF: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp659=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp659->tag != 7)goto _LL2E1;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;}}_LL2E0:
# 2890
 t=Cyc_Tcexp_tcOr(te,loc,_tmp65A,_tmp65B);goto _LL2CA;_LL2E1: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp65C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp65C->tag != 8)goto _LL2E3;else{_tmp65D=_tmp65C->f1;_tmp65E=_tmp65C->f2;}}_LL2E2:
# 2892
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp65D,_tmp65E);goto _LL2CA;_LL2E3: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp65F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp65F->tag != 9)goto _LL2E5;else{_tmp660=_tmp65F->f1;_tmp661=_tmp65F->f2;_tmp662=(struct Cyc_Absyn_VarargCallInfo**)& _tmp65F->f3;}}_LL2E4: {
# 2898
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp6BC;_push_handler(& _tmp6BC);{int _tmp6BE=0;if(setjmp(_tmp6BC.handler))_tmp6BE=1;if(!_tmp6BE){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp6BD=(void*)_exn_thrown;void*_tmp6C0=_tmp6BD;struct _dyneither_ptr _tmp6C2;void*_tmp6C3;_LL31E: {struct Cyc_Core_Failure_exn_struct*_tmp6C1=(struct Cyc_Core_Failure_exn_struct*)_tmp6C0;if(_tmp6C1->tag != Cyc_Core_Failure)goto _LL320;else{_tmp6C2=_tmp6C1->f1;}}_LL31F:
# 2905
 ok=0;
fn_exp=e;
goto _LL31D;_LL320: _tmp6C3=_tmp6C0;_LL321:(void)_rethrow(_tmp6C3);_LL31D:;}};}
# 2909
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp660,_tmp661,_tmp662,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2916
goto _LL2CA;}_LL2E5: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp663=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp663->tag != 10)goto _LL2E7;else{_tmp664=_tmp663->f1;}}_LL2E6:
# 2918
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp664);goto _LL2CA;_LL2E7: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp665=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp665->tag != 12)goto _LL2E9;else{_tmp666=_tmp665->f1;_tmp667=_tmp665->f2;}}_LL2E8:
# 2920
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp666,_tmp667);goto _LL2CA;_LL2E9: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp668=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp668->tag != 13)goto _LL2EB;else{_tmp669=(void*)_tmp668->f1;_tmp66A=_tmp668->f2;_tmp66B=(enum Cyc_Absyn_Coercion*)& _tmp668->f4;}}_LL2EA:
# 2922
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp669,_tmp66A,_tmp66B);goto _LL2CA;_LL2EB: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp66C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp66C->tag != 14)goto _LL2ED;else{_tmp66D=_tmp66C->f1;}}_LL2EC:
# 2924
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp66D);goto _LL2CA;_LL2ED: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp66E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp66E->tag != 15)goto _LL2EF;else{_tmp66F=(struct Cyc_Absyn_Exp**)& _tmp66E->f1;_tmp670=_tmp66E->f2;}}_LL2EE:
# 2926
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp66F,e,_tmp670);goto _LL2CA;_LL2EF: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp671->tag != 17)goto _LL2F1;else{_tmp672=_tmp671->f1;}}_LL2F0: {
# 2928
void*_tmp6C4=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp672);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp6C4);goto _LL2CA;}_LL2F1: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp673->tag != 16)goto _LL2F3;else{_tmp674=(void*)_tmp673->f1;}}_LL2F2:
# 2931
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp674);goto _LL2CA;_LL2F3: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp675=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp675->tag != 18)goto _LL2F5;else{_tmp676=(void*)_tmp675->f1;_tmp677=_tmp675->f2;}}_LL2F4:
# 2933
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp676,_tmp677);goto _LL2CA;_LL2F5: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp678=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp678->tag != 19)goto _LL2F7;else{_tmp679=_tmp678->f1;}}_LL2F6:
# 2935
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp679);goto _LL2CA;_LL2F7: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp67A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp67A->tag != 20)goto _LL2F9;else{_tmp67B=_tmp67A->f1;_tmp67C=_tmp67A->f2;_tmp67D=(int*)& _tmp67A->f3;_tmp67E=(int*)& _tmp67A->f4;}}_LL2F8:
# 2937
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp67B,_tmp67C,_tmp67D,_tmp67E);goto _LL2CA;_LL2F9: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp67F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp67F->tag != 21)goto _LL2FB;else{_tmp680=_tmp67F->f1;_tmp681=_tmp67F->f2;_tmp682=(int*)& _tmp67F->f3;_tmp683=(int*)& _tmp67F->f4;}}_LL2FA:
# 2939
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp680,_tmp681,_tmp682,_tmp683);goto _LL2CA;_LL2FB: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp684=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp684->tag != 22)goto _LL2FD;else{_tmp685=_tmp684->f1;_tmp686=_tmp684->f2;}}_LL2FC:
# 2941
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp685,_tmp686);goto _LL2CA;_LL2FD: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp687=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp687->tag != 23)goto _LL2FF;else{_tmp688=_tmp687->f1;}}_LL2FE:
# 2943
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp688);goto _LL2CA;_LL2FF: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp689=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp689->tag != 24)goto _LL301;else{_tmp68A=_tmp689->f1;_tmp68B=_tmp689->f2;}}_LL300:
# 2945
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp68A,_tmp68B);goto _LL2CA;_LL301: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp68C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp68C->tag != 25)goto _LL303;else{_tmp68D=_tmp68C->f1;}}_LL302: {
# 2949
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp6C5=Cyc_Tcutil_compress(*topt);void*_tmp6C6=_tmp6C5;void**_tmp6C8;struct Cyc_Absyn_Tqual*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;_LL323: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6C7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C6;if(_tmp6C7->tag != 8)goto _LL325;else{_tmp6C8=(void**)&(_tmp6C7->f1).elt_type;_tmp6C9=(struct Cyc_Absyn_Tqual*)&(_tmp6C7->f1).tq;_tmp6CA=(_tmp6C7->f1).zero_term;}}_LL324:
# 2955
 elt_topt=_tmp6C8;
elt_tqopt=_tmp6C9;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CA);
goto _LL322;_LL325:;_LL326:
 goto _LL322;_LL322:;}
# 2962
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp68D);goto _LL2CA;}_LL303: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp68E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp68E->tag != 36)goto _LL305;else{_tmp68F=_tmp68E->f1;}}_LL304:
# 2964
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp68F);goto _LL2CA;_LL305: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp690=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp690->tag != 26)goto _LL307;else{_tmp691=_tmp690->f1;_tmp692=_tmp690->f2;_tmp693=_tmp690->f3;_tmp694=(int*)& _tmp690->f4;}}_LL306:
# 2966
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp691,_tmp692,_tmp693,_tmp694);goto _LL2CA;_LL307: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp695=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp695->tag != 27)goto _LL309;else{_tmp696=_tmp695->f1;_tmp697=(void*)_tmp695->f2;_tmp698=(int*)& _tmp695->f3;}}_LL308:
# 2968
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp696,_tmp697,_tmp698);goto _LL2CA;_LL309: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp699=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp699->tag != 28)goto _LL30B;else{_tmp69A=(struct _tuple1**)& _tmp699->f1;_tmp69B=(struct Cyc_List_List**)& _tmp699->f2;_tmp69C=_tmp699->f3;_tmp69D=(struct Cyc_Absyn_Aggrdecl**)& _tmp699->f4;}}_LL30A:
# 2970
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp69A,_tmp69B,_tmp69C,_tmp69D);goto _LL2CA;_LL30B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp69E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp69E->tag != 29)goto _LL30D;else{_tmp69F=(void*)_tmp69E->f1;_tmp6A0=_tmp69E->f2;}}_LL30C:
# 2972
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp69F,_tmp6A0);goto _LL2CA;_LL30D: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6A1=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6A1->tag != 30)goto _LL30F;else{_tmp6A2=_tmp6A1->f1;_tmp6A3=_tmp6A1->f2;_tmp6A4=_tmp6A1->f3;}}_LL30E:
# 2974
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp6A2,_tmp6A3,_tmp6A4);goto _LL2CA;_LL30F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6A5=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6A5->tag != 31)goto _LL311;else{_tmp6A6=(struct _tuple1**)& _tmp6A5->f1;_tmp6A7=_tmp6A5->f2;_tmp6A8=_tmp6A5->f3;}}_LL310:
# 2976
*_tmp6A6=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6A8))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA4F;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA4E;t=(void*)((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E[0]=((_tmpA4F.tag=13,((_tmpA4F.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp6A7))->name,((_tmpA4F.f2=_tmp6A7,_tmpA4F)))))),_tmpA4E))));}goto _LL2CA;_LL311: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6A9=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6A9->tag != 32)goto _LL313;else{_tmp6AA=(struct _tuple1**)& _tmp6A9->f1;_tmp6AB=(void*)_tmp6A9->f2;_tmp6AC=_tmp6A9->f3;}}_LL312:
# 2979
*_tmp6AA=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6AC))->name;
t=_tmp6AB;goto _LL2CA;_LL313: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp6AD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6AD->tag != 33)goto _LL315;else{_tmp6AE=(int*)&(_tmp6AD->f1).is_calloc;_tmp6AF=(struct Cyc_Absyn_Exp**)&(_tmp6AD->f1).rgn;_tmp6B0=(void***)&(_tmp6AD->f1).elt_type;_tmp6B1=(struct Cyc_Absyn_Exp**)&(_tmp6AD->f1).num_elts;_tmp6B2=(int*)&(_tmp6AD->f1).fat_result;}}_LL314:
# 2982
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp6AF,_tmp6B0,_tmp6B1,_tmp6AE,_tmp6B2);goto _LL2CA;_LL315: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6B3=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6B3->tag != 34)goto _LL317;else{_tmp6B4=_tmp6B3->f1;_tmp6B5=_tmp6B3->f2;}}_LL316:
# 2984
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp6B4,_tmp6B5);goto _LL2CA;_LL317: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp6B6=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6B6->tag != 37)goto _LL319;else{_tmp6B7=_tmp6B6->f1;_tmp6B8=_tmp6B6->f2;}}_LL318:
# 2986
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp6B7,_tmp6B8);goto _LL2CA;_LL319: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6B9=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6B9->tag != 38)goto _LL31B;else{_tmp6BA=(void*)_tmp6B9->f1;}}_LL31A:
# 2988
 if(!te->allow_valueof){
const char*_tmpA52;void*_tmpA51;(_tmpA51=0,Cyc_Tcutil_terr(e->loc,((_tmpA52="valueof(-) can only occur within types",_tag_dyneither(_tmpA52,sizeof(char),39))),_tag_dyneither(_tmpA51,sizeof(void*),0)));}
# 2996
t=Cyc_Absyn_sint_typ;
goto _LL2CA;_LL31B: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6BB=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp6BB->tag != 39)goto _LL2CA;}_LL31C:
# 2999
{const char*_tmpA55;void*_tmpA54;(_tmpA54=0,Cyc_Tcutil_terr(e->loc,((_tmpA55="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA55,sizeof(char),50))),_tag_dyneither(_tmpA54,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2CA:;}
# 3002
e->topt=t;}
