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
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 249
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 336
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 349
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 429 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 432
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 530
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 927 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 933
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 936
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 939
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 941
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 954
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 957
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 959
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 961
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 971
void*Cyc_Absyn_exn_typ();
# 982
extern void*Cyc_Absyn_bounds_one;
# 987
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 990
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 993
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 998
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1009
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1076
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1082
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1158
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1160
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1163
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1165
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
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
# 88
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 108 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
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
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 135
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 152
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 157
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 165
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 173
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 177
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 181
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 186
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
# 235 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 245
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 247
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 249
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 253
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 259
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 261
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 263
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 273
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 278
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 281
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 286
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 291
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 295
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 322
int Cyc_Tcutil_bits_only(void*t);
# 325
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 354
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 357
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 370
int Cyc_Tcutil_is_array(void*t);
# 374
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 377
int Cyc_Tcutil_zeroable_type(void*t);struct _tuple15{unsigned int f1;int f2;};
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
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 30 "jump_analysis.h"
typedef struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_table_t;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 45 "jump_analysis.h"
typedef struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_anal_res_t;
# 47
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
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
# 51 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 55
({void*_tmp0=0;Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp0,sizeof(void*),0));});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 59
return*topt;}}
# 68
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 72
if(topt == 0){
# 74
e->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2;_tmp2.tag=25;_tmp2.f1=des;_tmp2;});_tmp1;});
return;}{
# 77
void*t=*topt;
void*_tmp3=Cyc_Tcutil_compress(t);void*_tmp4=_tmp3;void*_tmp13;struct Cyc_Absyn_Tqual _tmp12;union Cyc_Absyn_AggrInfoU _tmp11;switch(*((int*)_tmp4)){case 11: _LL1: _tmp11=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1).aggr_info;_LL2:
# 80
{union Cyc_Absyn_AggrInfoU _tmp5=_tmp11;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp5.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7="struct type not properly set";_tag_dyneither(_tmp7,sizeof(char),29);}),_tag_dyneither(_tmp6,sizeof(void*),0));});}else{_LLC: _tmpA=*(_tmp5.KnownAggr).val;_LLD:
 e->r=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp9;_tmp9.tag=28;_tmp9.f1=_tmpA->name;_tmp9.f2=0;_tmp9.f3=des;_tmp9.f4=_tmpA;_tmp9;});_tmp8;});}_LL9:;}
# 84
goto _LL0;case 8: _LL3: _tmp13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).elt_type;_tmp12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).tq;_LL4:
 e->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpC;_tmpC.tag=25;_tmpC.f1=des;_tmpC;});_tmpB;});goto _LL0;case 12: _LL5: _LL6:
 e->r=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE;_tmpE.tag=29;_tmpE.f1=t;_tmpE.f2=des;_tmpE;});_tmpD;});goto _LL0;default: _LL7: _LL8:
 e->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp10;_tmp10.tag=25;_tmp10.f1=des;_tmp10;});_tmpF;});goto _LL0;}_LL0:;};}
# 94
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 98
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 104
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp14=e->r;void*_tmp15=_tmp14;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->tag == 3){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->f2 == 0){_LLF: _LL10:
({void*_tmp16=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp17="assignment in test";_tag_dyneither(_tmp17,sizeof(char),19);}),_tag_dyneither(_tmp16,sizeof(void*),0));});goto _LLE;}else{goto _LL11;}}else{_LL11: _LL12:
 goto _LLE;}_LLE:;}
# 112
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp18[2]={& _tmp1A,& _tmp1B};Cyc_Tcutil_terr(e->loc,({const char*_tmp19="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp19,sizeof(char),59);}),_tag_dyneither(_tmp18,sizeof(void*),2));});});});
# 121
{void*_tmp1C=e->r;void*_tmp1D=_tmp1C;enum Cyc_Absyn_Primop _tmp26;struct Cyc_List_List*_tmp25;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->tag == 2){_LL14: _tmp26=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f1;_tmp25=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f2;_LL15:
# 123
 if(_tmp26 == Cyc_Absyn_Eq  || _tmp26 == Cyc_Absyn_Neq){
struct _tuple0 _tmp1E=({struct _tuple0 _tmp24;_tmp24.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25))->hd)->topt);_tmp24.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp25))->tl))->hd)->topt);_tmp24;});struct _tuple0 _tmp1F=_tmp1E;void*_tmp23;void*_tmp22;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->tag == 15){_LL19: _tmp23=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->f1;_tmp22=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->f1;_LL1A:
# 126
 return({struct Cyc_Tcexp_TestEnv _tmp20;_tmp20.eq=({struct _tuple0*_tmp21=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp21));_tmp21->f1=_tmp23;_tmp21->f2=_tmp22;_tmp21;});_tmp20.isTrue=_tmp26 == Cyc_Absyn_Eq;_tmp20;});}else{goto _LL1B;}}else{_LL1B: _LL1C:
# 128
 goto _LL18;}_LL18:;}
# 131
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 134
return({struct Cyc_Tcexp_TestEnv _tmp27;_tmp27.eq=0;_tmp27.isTrue=0;_tmp27;});}
# 155 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 160
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp29=_tmp28;struct _dyneither_ptr _tmp5F;struct _dyneither_ptr _tmp5E;enum Cyc_Absyn_Sign _tmp5D;int _tmp5C;int _tmp5B;enum Cyc_Absyn_Sign _tmp5A;enum Cyc_Absyn_Sign _tmp59;switch((_tmp29.Wstring_c).tag){case 2: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1E: _LL1F:
 t=Cyc_Absyn_schar_typ;goto _LL1D;case Cyc_Absyn_Unsigned: _LL20: _LL21:
 t=Cyc_Absyn_uchar_typ;goto _LL1D;default: _LL22: _LL23:
 t=Cyc_Absyn_char_typ;goto _LL1D;}case 3: _LL24: _LL25:
 t=Cyc_Absyn_wchar_typ();goto _LL1D;case 4: _LL26: _tmp59=((_tmp29.Short_c).val).f1;_LL27:
# 170
 t=_tmp59 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL1D;case 6: _LL28: _tmp5A=((_tmp29.LongLong_c).val).f1;_LL29:
# 172
 t=_tmp5A == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL1D;case 7: _LL2A: _tmp5B=((_tmp29.Float_c).val).f2;_LL2B:
 t=Cyc_Absyn_float_typ(_tmp5B);goto _LL1D;case 5: _LL2C: _tmp5D=((_tmp29.Int_c).val).f1;_tmp5C=((_tmp29.Int_c).val).f2;_LL2D:
# 175
 if(topt == 0)
t=_tmp5D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 182
void*_tmp2A=Cyc_Tcutil_compress(*topt);void*_tmp2B=_tmp2A;void*_tmp41;void*_tmp40;struct Cyc_Absyn_Tqual _tmp3F;void*_tmp3E;union Cyc_Absyn_Constraint*_tmp3D;union Cyc_Absyn_Constraint*_tmp3C;union Cyc_Absyn_Constraint*_tmp3B;enum Cyc_Absyn_Sign _tmp3A;enum Cyc_Absyn_Sign _tmp39;enum Cyc_Absyn_Sign _tmp38;enum Cyc_Absyn_Sign _tmp37;switch(*((int*)_tmp2B)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f2){case Cyc_Absyn_Char_sz: _LL35: _tmp37=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL36:
# 184
{enum Cyc_Absyn_Sign _tmp2C=_tmp37;switch(_tmp2C){case Cyc_Absyn_Unsigned: _LL44: _LL45:
 t=Cyc_Absyn_uchar_typ;goto _LL43;case Cyc_Absyn_Signed: _LL46: _LL47:
 t=Cyc_Absyn_schar_typ;goto _LL43;default: _LL48: _LL49:
 t=Cyc_Absyn_char_typ;goto _LL43;}_LL43:;}
# 189
*c=Cyc_Absyn_Char_c(_tmp37,(char)_tmp5C);
goto _LL34;case Cyc_Absyn_Short_sz: _LL37: _tmp38=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL38:
# 192
 t=_tmp38 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
*c=Cyc_Absyn_Short_c(_tmp38,(short)_tmp5C);
goto _LL34;case Cyc_Absyn_Int_sz: _LL39: _tmp39=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3A:
# 196
 t=_tmp39 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;case Cyc_Absyn_Long_sz: _LL3B: _tmp3A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3C:
# 199
 t=_tmp3A == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;default: goto _LL41;}case 5: _LL3D: _tmp40=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_typ;_tmp3F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_tq;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).rgn;_tmp3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).nullable;_tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).bounds;_tmp3B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;if(_tmp5C == 0){_LL3E: {
# 203
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp3D))return*topt;{
struct Cyc_List_List*_tmp2D=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp2F;_tmp2F.tag=5;_tmp2F.f1=({struct Cyc_Absyn_PtrInfo _tmp30;_tmp30.elt_typ=_tmp40;_tmp30.elt_tq=_tmp3F;_tmp30.ptr_atts=({(_tmp30.ptr_atts).rgn=_tmp3E;(_tmp30.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp30.ptr_atts).bounds=_tmp3C;(_tmp30.ptr_atts).zero_term=_tmp3B;(_tmp30.ptr_atts).ptrloc=0;_tmp30.ptr_atts;});_tmp30;});_tmp2F;});_tmp2E;});
# 209
goto _LL34;};}}else{goto _LL41;}case 19: _LL3F: _tmp41=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B)->f1;_LL40: {
# 211
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp32=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp36;_tmp36.tag=18;_tmp36.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0);_tmp36;});_tmp35;});
# 218
t=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp34;_tmp34.tag=19;_tmp34.f1=(void*)_tmp32;_tmp34;});_tmp33;});
goto _LL34;}default: _LL41: _LL42:
# 221
 t=_tmp5D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;}_LL34:;}
# 224
goto _LL1D;case 8: _LL2E: _tmp5E=(_tmp29.String_c).val;_LL2F:
# 226
 string_numelts=(int)_get_dyneither_size(_tmp5E,sizeof(char));
_tmp5F=_tmp5E;goto _LL31;case 9: _LL30: _tmp5F=(_tmp29.Wstring_c).val;_LL31:
# 229
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp5F);
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 233
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
elen->topt=Cyc_Absyn_uint_typ;{
# 238
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp42=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp47;_tmp47.tag=1;_tmp47.f1=elen;_tmp47;});_tmp46;});
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp42,Cyc_Absyn_true_conref);
# 241
if(topt != 0){
void*_tmp43=Cyc_Tcutil_compress(*topt);void*_tmp44=_tmp43;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 8: _LL4B: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL4C:
# 246
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp45,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);case 5: _LL4D: _LL4E:
# 248
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 254
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp42,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 262
goto _LL4A;default: _LL4F: _LL50:
 goto _LL4A;}_LL4A:;}
# 266
return t;};};default: _LL32: _LL33:
# 268
 if(topt != 0){
void*_tmp48=Cyc_Tcutil_compress(*topt);void*_tmp49=_tmp48;void*_tmp52;struct Cyc_Absyn_Tqual _tmp51;void*_tmp50;union Cyc_Absyn_Constraint*_tmp4F;union Cyc_Absyn_Constraint*_tmp4E;union Cyc_Absyn_Constraint*_tmp4D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->tag == 5){_LL52: _tmp52=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_typ;_tmp51=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_tq;_tmp50=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).rgn;_tmp4F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).nullable;_tmp4E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).bounds;_tmp4D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).zero_term;_LL53:
# 272
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp4F))return*topt;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4B;_tmp4B.tag=5;_tmp4B.f1=({struct Cyc_Absyn_PtrInfo _tmp4C;_tmp4C.elt_typ=_tmp52;_tmp4C.elt_tq=_tmp51;_tmp4C.ptr_atts=({(_tmp4C.ptr_atts).rgn=_tmp50;(_tmp4C.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4C.ptr_atts).bounds=_tmp4E;(_tmp4C.ptr_atts).zero_term=_tmp4D;(_tmp4C.ptr_atts).ptrloc=0;_tmp4C.ptr_atts;});_tmp4C;});_tmp4B;});_tmp4A;});}else{_LL54: _LL55:
# 275
 goto _LL51;}_LL51:;}{
# 278
struct Cyc_List_List*_tmp53=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp55;_tmp55.tag=5;_tmp55.f1=({struct Cyc_Absyn_PtrInfo _tmp56;_tmp56.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->v=_tmp53;_tmp58;}));_tmp56.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp56.ptr_atts=({(_tmp56.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->v=_tmp53;_tmp57;}));(_tmp56.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp56.ptr_atts).bounds=
# 283
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).ptrloc=0;_tmp56.ptr_atts;});_tmp56;});_tmp55;});_tmp54;});
# 285
goto _LL1D;};}_LL1D:;}
# 287
return t;}
# 291
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 296
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp60=*((void**)_check_null(b));void*_tmp61=_tmp60;struct Cyc_Absyn_Vardecl*_tmp6F;struct Cyc_Absyn_Vardecl*_tmp6E;struct Cyc_Absyn_Vardecl*_tmp6D;struct Cyc_Absyn_Fndecl*_tmp6C;struct Cyc_Absyn_Vardecl*_tmp6B;struct _tuple1*_tmp6A;switch(*((int*)_tmp61)){case 0: _LL57: _tmp6A=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp61)->f1;_LL58:
# 299
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Core_Impossible_exn_struct _tmp63;_tmp63.tag=Cyc_Core_Impossible;_tmp63.f1=({const char*_tmp64="unresolved binding in tcVar";_tag_dyneither(_tmp64,sizeof(char),28);});_tmp63;});_tmp62;}));case 1: _LL59: _tmp6B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5A:
# 303
 Cyc_Tcenv_lookup_ordinary(Cyc_Core_heap_region,te,loc,_tmp6B->name,1);
return _tmp6B->type;case 2: _LL5B: _tmp6C=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5C:
# 308
 Cyc_Tcenv_lookup_ordinary(Cyc_Core_heap_region,te,loc,_tmp6C->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp6C);case 5: _LL5D: _tmp6D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5E:
 _tmp6E=_tmp6D;goto _LL60;case 4: _LL5F: _tmp6E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp61)->f1;_LL60:
 _tmp6F=_tmp6E;goto _LL62;default: _LL61: _tmp6F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp61)->f1;_LL62:
# 314
 Cyc_Tcenv_lookup_ordinary(Cyc_Core_heap_region,te,loc,_tmp6F->name,1);
if(te->allow_valueof){
void*_tmp65=Cyc_Tcutil_compress(_tmp6F->type);void*_tmp66=_tmp65;void*_tmp69;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66)->tag == 19){_LL64: _tmp69=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66)->f1;_LL65:
# 318
 e->r=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp68;_tmp68.tag=38;_tmp68.f1=_tmp69;_tmp68;});_tmp67;});
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}
# 323
return _tmp6F->type;}_LL56:;}
# 327
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 336
struct Cyc_List_List*desc_types;
{void*_tmp70=fmt->r;void*_tmp71=_tmp70;struct _dyneither_ptr _tmp77;struct _dyneither_ptr _tmp76;switch(*((int*)_tmp71)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).tag == 8){_LL69: _tmp76=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).val;_LL6A:
 _tmp77=_tmp76;goto _LL6C;}else{goto _LL6D;}case 13: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).tag == 8){_LL6B: _tmp77=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).val;_LL6C:
# 340
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmp77,fmt->loc);goto _LL68;}else{goto _LL6D;}}else{goto _LL6D;}default: _LL6D: _LL6E:
# 344
 if(opt_args != 0){
struct Cyc_List_List*_tmp72=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp72->hd);{
struct _RegionHandle _tmp73=_new_region("temp");struct _RegionHandle*temp=& _tmp73;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp72->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp72->hd))
({void*_tmp74=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp72->hd)->loc,({const char*_tmp75="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp75,sizeof(char),49);}),_tag_dyneither(_tmp74,sizeof(void*),0));});
# 349
;_pop_region(temp);};}}
# 355
return;}_LL68:;}
# 357
if(opt_args != 0){
struct Cyc_List_List*_tmp78=(struct Cyc_List_List*)opt_args->v;
# 360
for(0;desc_types != 0  && _tmp78 != 0;
(((desc_types=desc_types->tl,_tmp78=_tmp78->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp78->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp79[2]={& _tmp7B,& _tmp7C};Cyc_Tcutil_terr(e->loc,({const char*_tmp7A="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp7A,sizeof(char),48);}),_tag_dyneither(_tmp79,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 371
if(alias_coercion)
*alias_arg_exps=({struct Cyc_List_List*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=(void*)arg_cnt;_tmp7D->tl=*alias_arg_exps;_tmp7D;});{
struct _RegionHandle _tmp7E=_new_region("temp");struct _RegionHandle*temp=& _tmp7E;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e))
({void*_tmp7F=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc,({const char*_tmp80="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp80,sizeof(char),49);}),_tag_dyneither(_tmp7F,sizeof(void*),0));});
# 374
;_pop_region(temp);};}
# 379
if(desc_types != 0)
({void*_tmp81=0;Cyc_Tcutil_terr(fmt->loc,({const char*_tmp82="too few arguments";_tag_dyneither(_tmp82,sizeof(char),18);}),_tag_dyneither(_tmp81,sizeof(void*),0));});
if(_tmp78 != 0){
({void*_tmp83=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc,({const char*_tmp84="too many arguments";_tag_dyneither(_tmp84,sizeof(char),19);}),_tag_dyneither(_tmp83,sizeof(void*),0));});
(int)_throw((void*)({struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp85=_cycalloc_atomic(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp86;_tmp86.tag=Cyc_Tcutil_AbortTypeCheckingFunction;_tmp86;});_tmp85;}));}}}
# 387
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 389
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp87=p;switch(_tmp87){case Cyc_Absyn_Plus: _LL70: _LL71:
 goto _LL73;case Cyc_Absyn_Minus: _LL72: _LL73:
# 393
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp88[1]={& _tmp8A};Cyc_Tcutil_terr(loc,({const char*_tmp89="expecting numeric type but found %s";_tag_dyneither(_tmp89,sizeof(char),36);}),_tag_dyneither(_tmp88,sizeof(void*),1));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL74: _LL75:
# 397
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp8B[1]={& _tmp8D};Cyc_Tcutil_terr(loc,({const char*_tmp8C="expecting integral or * type but found %s";_tag_dyneither(_tmp8C,sizeof(char),42);}),_tag_dyneither(_tmp8B,sizeof(void*),1));});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL76: _LL77:
# 402
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp8E[1]={& _tmp90};Cyc_Tcutil_terr(loc,({const char*_tmp8F="expecting integral type but found %s";_tag_dyneither(_tmp8F,sizeof(char),37);}),_tag_dyneither(_tmp8E,sizeof(void*),1));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL78: _LL79:
# 406
{void*_tmp91=t;union Cyc_Absyn_Constraint*_tmp9A;switch(*((int*)_tmp91)){case 8: _LL7D: _LL7E:
 goto _LL7C;case 5: _LL7F: _tmp9A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).bounds;_LL80:
# 409
{void*_tmp92=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9A);void*_tmp93=_tmp92;struct Cyc_Absyn_Exp*_tmp96;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp93)->tag == 0){_LL84: _LL85:
 goto _LL83;}else{_LL86: _tmp96=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp93)->f1;_LL87:
# 412
 if(!Cyc_Evexp_c_can_eval(_tmp96) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp94=0;Cyc_Tcutil_terr(loc,({const char*_tmp95="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp95,sizeof(char),55);}),_tag_dyneither(_tmp94,sizeof(void*),0));});
goto _LL83;}_LL83:;}
# 416
goto _LL7C;default: _LL81: _LL82:
# 418
({struct Cyc_String_pa_PrintArg_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp97[1]={& _tmp99};Cyc_Tcutil_terr(loc,({const char*_tmp98="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp98,sizeof(char),47);}),_tag_dyneither(_tmp97,sizeof(void*),1));});});}_LL7C:;}
# 420
return Cyc_Absyn_uint_typ;default: _LL7A: _LL7B:
({void*_tmp9B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9C="Non-unary primop";_tag_dyneither(_tmp9C,sizeof(char),17);}),_tag_dyneither(_tmp9B,sizeof(void*),0));});}_LL6F:;}
# 426
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 429
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));({void*_tmp9D[1]={& _tmp9F};Cyc_Tcutil_terr(e1->loc,({const char*_tmp9E="type %s cannot be used here";_tag_dyneither(_tmp9E,sizeof(char),28);}),_tag_dyneither(_tmp9D,sizeof(void*),1));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 433
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));({void*_tmpA0[1]={& _tmpA2};Cyc_Tcutil_terr(e2->loc,({const char*_tmpA1="type %s cannot be used here";_tag_dyneither(_tmpA1,sizeof(char),28);}),_tag_dyneither(_tmpA0,sizeof(void*),1));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 437
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 442
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmpA3=t1;void*_tmpB9;struct Cyc_Absyn_Tqual _tmpB8;void*_tmpB7;union Cyc_Absyn_Constraint*_tmpB6;union Cyc_Absyn_Constraint*_tmpB5;union Cyc_Absyn_Constraint*_tmpB4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->tag == 5){_LL89: _tmpB9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).elt_typ;_tmpB8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).elt_tq;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).ptr_atts).rgn;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).ptr_atts).nullable;_tmpB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).ptr_atts).bounds;_tmpB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3)->f1).ptr_atts).zero_term;_LL8A:
# 447
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpB9),& Cyc_Tcutil_tmk))
({void*_tmpA4=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpA5="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpA5,sizeof(char),50);}),_tag_dyneither(_tmpA4,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpA6=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpA7="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpA7,sizeof(char),54);}),_tag_dyneither(_tmpA6,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));({void*_tmpA8[1]={& _tmpAA};Cyc_Tcutil_terr(e2->loc,({const char*_tmpA9="expecting int but found %s";_tag_dyneither(_tmpA9,sizeof(char),27);}),_tag_dyneither(_tmpA8,sizeof(void*),1));});});{
void*_tmpAB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB5);void*_tmpAC=_tmpAB;struct Cyc_Absyn_Exp*_tmpB3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAC)->tag == 0){_LL8E: _LL8F:
 return t1;}else{_LL90: _tmpB3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAC)->f1;_LL91:
# 458
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB4))
({void*_tmpAD=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpAE="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpAE,sizeof(char),70);}),_tag_dyneither(_tmpAD,sizeof(void*),0));});{
# 466
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAF=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpB1;_tmpB1.tag=5;_tmpB1.f1=({struct Cyc_Absyn_PtrInfo _tmpB2;_tmpB2.elt_typ=_tmpB9;_tmpB2.elt_tq=_tmpB8;_tmpB2.ptr_atts=({(_tmpB2.ptr_atts).rgn=_tmpB7;(_tmpB2.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpB2.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmpB2.ptr_atts).zero_term=_tmpB4;(_tmpB2.ptr_atts).ptrloc=0;_tmpB2.ptr_atts;});_tmpB2;});_tmpB1;});_tmpB0;});
# 470
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpAF,Cyc_Absyn_Other_coercion);
return(void*)_tmpAF;};}_LL8D:;};}else{_LL8B: _LL8C:
# 473
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL88:;}
# 478
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpBD;_tmpBD.tag=0;_tmpBD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 488
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpBA[2]={& _tmpBC,& _tmpBD};Cyc_Tcutil_terr(e1->loc,({const char*_tmpBB="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpBB,sizeof(char),59);}),_tag_dyneither(_tmpBA,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 491
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
({struct Cyc_String_pa_PrintArg_struct _tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 496
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpBE[2]={& _tmpC0,& _tmpC1};Cyc_Tcutil_terr(e1->loc,({const char*_tmpBF="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpBF,sizeof(char),59);}),_tag_dyneither(_tmpBE,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 500
({void*_tmpC2=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpC3="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC3,sizeof(char),60);}),_tag_dyneither(_tmpC2,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 504
return Cyc_Absyn_sint_typ;}else{
# 506
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpC4=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpC5="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpC5,sizeof(char),50);}),_tag_dyneither(_tmpC4,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC6=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpC7="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpC7,sizeof(char),54);}),_tag_dyneither(_tmpC6,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpCB;_tmpCB.tag=0;_tmpCB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpC8[2]={& _tmpCA,& _tmpCB};Cyc_Tcutil_terr(e2->loc,({const char*_tmpC9="expecting either %s or int but found %s";_tag_dyneither(_tmpC9,sizeof(char),40);}),_tag_dyneither(_tmpC8,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 515
return t1;}}}
# 519
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpCC=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpCD="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpCD,sizeof(char),60);}),_tag_dyneither(_tmpCC,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 529
({void*_tmpCE=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpCF="thin pointer subtraction!";_tag_dyneither(_tmpCF,sizeof(char),26);}),_tag_dyneither(_tmpCE,sizeof(void*),0));});
return Cyc_Absyn_sint_typ;}
# 532
({void*_tmpD0=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpD1="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpD1,sizeof(char),56);}),_tag_dyneither(_tmpD0,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 535
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpD2=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpD3="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpD3,sizeof(char),51);}),_tag_dyneither(_tmpD2,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 540
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 543
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 551
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 556
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 569
pointer_cmp: {
struct _tuple0 _tmpD4=({struct _tuple0 _tmpD8;_tmpD8.f1=Cyc_Tcutil_compress(t1);_tmpD8.f2=Cyc_Tcutil_compress(t2);_tmpD8;});struct _tuple0 _tmpD5=_tmpD4;void*_tmpD7;void*_tmpD6;switch(*((int*)_tmpD5.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD5.f2)->tag == 5){_LL93: _tmpD7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD5.f1)->f1).elt_typ;_tmpD6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD5.f2)->f1).elt_typ;_LL94:
# 573
 if(Cyc_Tcutil_unify(_tmpD7,_tmpD6))return Cyc_Absyn_sint_typ;goto _LL92;}else{goto _LL97;}case 15: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD5.f2)->tag == 15){_LL95: _LL96:
# 575
 return Cyc_Absyn_sint_typ;}else{goto _LL97;}default: _LL97: _LL98:
 goto _LL92;}_LL92:;}
# 578
({struct Cyc_String_pa_PrintArg_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpDB;_tmpDB.tag=0;_tmpDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpD9[2]={& _tmpDB,& _tmpDC};Cyc_Tcutil_terr(loc,({const char*_tmpDA="comparison not allowed between %s and %s";_tag_dyneither(_tmpDA,sizeof(char),41);}),_tag_dyneither(_tmpD9,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 585
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 587
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 593
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpDD=({struct _tuple0 _tmpE1;_tmpE1.f1=t1;_tmpE1.f2=t2;_tmpE1;});struct _tuple0 _tmpDE=_tmpDD;void*_tmpE0;void*_tmpDF;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDE.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDE.f2)->tag == 15){_LL9A: _tmpE0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDE.f1)->f1;_tmpDF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDE.f2)->f1;_LL9B:
# 598
 return Cyc_Absyn_sint_typ;}else{goto _LL9C;}}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 601
({struct Cyc_String_pa_PrintArg_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpE4;_tmpE4.tag=0;_tmpE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpE2[2]={& _tmpE4,& _tmpE5};Cyc_Tcutil_terr(loc,({const char*_tmpE3="comparison not allowed between %s and %s";_tag_dyneither(_tmpE3,sizeof(char),41);}),_tag_dyneither(_tmpE2,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 609
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 611
enum Cyc_Absyn_Primop _tmpE6=p;switch(_tmpE6){case Cyc_Absyn_Plus: _LL9F: _LLA0:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLA1: _LLA2:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLA3: _LLA4:
# 615
 goto _LLA6;case Cyc_Absyn_Div: _LLA5: _LLA6:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLA7: _LLA8:
# 618
 goto _LLAA;case Cyc_Absyn_Bitand: _LLA9: _LLAA:
 goto _LLAC;case Cyc_Absyn_Bitor: _LLAB: _LLAC:
 goto _LLAE;case Cyc_Absyn_Bitxor: _LLAD: _LLAE:
 goto _LLB0;case Cyc_Absyn_Bitlshift: _LLAF: _LLB0:
 goto _LLB2;case Cyc_Absyn_Bitlrshift: _LLB1: _LLB2:
 goto _LLB4;case Cyc_Absyn_Bitarshift: _LLB3: _LLB4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLB5: _LLB6:
# 628
 goto _LLB8;case Cyc_Absyn_Neq: _LLB7: _LLB8:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLB9: _LLBA:
# 631
 goto _LLBC;case Cyc_Absyn_Lt: _LLBB: _LLBC:
 goto _LLBE;case Cyc_Absyn_Gte: _LLBD: _LLBE:
 goto _LLC0;case Cyc_Absyn_Lte: _LLBF: _LLC0:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLC1: _LLC2:
# 636
({void*_tmpE7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpE8="bad binary primop";_tag_dyneither(_tmpE8,sizeof(char),18);}),_tag_dyneither(_tmpE7,sizeof(void*),0));});}_LL9E:;}
# 640
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 648
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpE9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpEA=Cyc_Tcexp_tcExp(te,topt,_tmpE9);
if(!Cyc_Tcutil_is_numeric(_tmpE9))
({struct Cyc_String_pa_PrintArg_struct _tmpED;_tmpED.tag=0;_tmpED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpEA));({void*_tmpEB[1]={& _tmpED};Cyc_Tcutil_terr(_tmpE9->loc,({const char*_tmpEC="expecting numeric type but found %s";_tag_dyneither(_tmpEC,sizeof(char),36);}),_tag_dyneither(_tmpEB,sizeof(void*),1));});});
return _tmpEA;}
# 655
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpEE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpEF=_tmpEE;switch(_tmpEF){case 0: _LLC4: _LLC5:
 return({void*_tmpF0=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmpF1="primitive operator has 0 arguments";_tag_dyneither(_tmpF1,sizeof(char),35);}),_tag_dyneither(_tmpF0,sizeof(void*),0));});case 1: _LLC6: _LLC7:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LLC3;case 2: _LLC8: _LLC9:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);goto _LLC3;default: _LLCA: _LLCB:
 return({void*_tmpF2=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmpF3="primitive operator has > 2 arguments";_tag_dyneither(_tmpF3,sizeof(char),37);}),_tag_dyneither(_tmpF2,sizeof(void*),0));});}_LLC3:;}
# 663
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 666
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpF4=Cyc_Tcutil_compress(t);
void*_tmpF5=_tmpF4;struct Cyc_List_List*_tmp111;void*_tmp110;struct Cyc_Absyn_Tqual _tmp10F;struct Cyc_Absyn_Datatypefield*_tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_Aggrdecl*_tmp10C;switch(*((int*)_tmpF5)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF5)->f1).aggr_info).KnownAggr).tag == 2){_LLCD: _tmp10C=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF5)->f1).aggr_info).KnownAggr).val;_LLCE:
# 670
 if(_tmp10C->impl == 0){
({void*_tmpF6=0;Cyc_Tcutil_terr(loc,({const char*_tmpF7="attempt to write an abstract aggregate";_tag_dyneither(_tmpF7,sizeof(char),39);}),_tag_dyneither(_tmpF6,sizeof(void*),0));});
return 0;}else{
# 674
_tmp10D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp10C->impl))->fields;goto _LLD0;}}else{goto _LLD7;}case 12: _LLCF: _tmp10D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF5)->f2;_LLD0:
# 676
 for(0;_tmp10D != 0;_tmp10D=_tmp10D->tl){
struct Cyc_Absyn_Aggrfield*_tmpF8=(struct Cyc_Absyn_Aggrfield*)_tmp10D->hd;
if((_tmpF8->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF8->name);({void*_tmpF9[1]={& _tmpFB};Cyc_Tcutil_terr(loc,({const char*_tmpFA="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpFA,sizeof(char),56);}),_tag_dyneither(_tmpF9,sizeof(void*),1));});});
return 0;}
# 682
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF8->type))return 0;}
# 684
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF5)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp10E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF5)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 686
{struct Cyc_List_List*_tmpFC=_tmp10E->typs;for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){
struct _tuple17*_tmpFD=(struct _tuple17*)_tmpFC->hd;struct _tuple17*_tmpFE=_tmpFD;struct Cyc_Absyn_Tqual _tmp103;void*_tmp102;_LLDA: _tmp103=_tmpFE->f1;_tmp102=_tmpFE->f2;_LLDB:;
if(_tmp103.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp10E->name));({void*_tmpFF[1]={& _tmp101};Cyc_Tcutil_terr(loc,({const char*_tmp100="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmp100,sizeof(char),64);}),_tag_dyneither(_tmpFF,sizeof(void*),1));});});
return 0;}
# 692
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp102))return 0;}}
# 694
return 1;}else{goto _LLD7;}case 8: _LLD3: _tmp110=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF5)->f1).elt_type;_tmp10F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF5)->f1).tq;_LLD4:
# 696
 if(_tmp10F.real_const){
({void*_tmp104=0;Cyc_Tcutil_terr(loc,({const char*_tmp105="attempt to over-write a const array";_tag_dyneither(_tmp105,sizeof(char),36);}),_tag_dyneither(_tmp104,sizeof(void*),0));});
return 0;}
# 700
return Cyc_Tcexp_check_writable_aggr(loc,_tmp110);case 10: _LLD5: _tmp111=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF5)->f1;_LLD6:
# 702
 for(0;_tmp111 != 0;_tmp111=_tmp111->tl){
struct _tuple17*_tmp106=(struct _tuple17*)_tmp111->hd;struct _tuple17*_tmp107=_tmp106;struct Cyc_Absyn_Tqual _tmp10B;void*_tmp10A;_LLDD: _tmp10B=_tmp107->f1;_tmp10A=_tmp107->f2;_LLDE:;
if(_tmp10B.real_const){
({void*_tmp108=0;Cyc_Tcutil_terr(loc,({const char*_tmp109="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmp109,sizeof(char),56);}),_tag_dyneither(_tmp108,sizeof(void*),0));});
return 0;}
# 708
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp10A))return 0;}
# 710
return 1;default: _LLD7: _LLD8:
 return 1;}_LLCC:;}
# 718
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 721
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp112=e->r;void*_tmp113=_tmp112;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct _dyneither_ptr*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct _dyneither_ptr*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Vardecl*_tmp139;struct Cyc_Absyn_Vardecl*_tmp138;struct Cyc_Absyn_Vardecl*_tmp137;struct Cyc_Absyn_Vardecl*_tmp136;switch(*((int*)_tmp113)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113)->f1)){case 3: _LLE0: _tmp136=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113)->f1)->f1;_LLE1:
 _tmp137=_tmp136;goto _LLE3;case 4: _LLE2: _tmp137=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113)->f1)->f1;_LLE3:
 _tmp138=_tmp137;goto _LLE5;case 5: _LLE4: _tmp138=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113)->f1)->f1;_LLE5:
 _tmp139=_tmp138;goto _LLE7;case 1: _LLE6: _tmp139=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113)->f1)->f1;_LLE7:
 if(!(_tmp139->tq).real_const)return;goto _LLDF;default: goto _LLF4;}case 22: _LLE8: _tmp13B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp113)->f1;_tmp13A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp113)->f2;_LLE9:
# 728
{void*_tmp114=Cyc_Tcutil_compress((void*)_check_null(_tmp13B->topt));void*_tmp115=_tmp114;struct Cyc_List_List*_tmp126;struct Cyc_Absyn_Tqual _tmp125;struct Cyc_Absyn_Tqual _tmp124;switch(*((int*)_tmp115)){case 5: _LLF7: _tmp124=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp115)->f1).elt_tq;_LLF8:
 _tmp125=_tmp124;goto _LLFA;case 8: _LLF9: _tmp125=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp115)->f1).tq;_LLFA:
 if(!_tmp125.real_const)return;goto _LLF6;case 10: _LLFB: _tmp126=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp115)->f1;_LLFC: {
# 732
struct _tuple15 _tmp116=Cyc_Evexp_eval_const_uint_exp(_tmp13A);struct _tuple15 _tmp117=_tmp116;unsigned int _tmp123;int _tmp122;_LL100: _tmp123=_tmp117.f1;_tmp122=_tmp117.f2;_LL101:;
if(!_tmp122){
({void*_tmp118=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp119="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp119,sizeof(char),47);}),_tag_dyneither(_tmp118,sizeof(void*),0));});
return;}
# 737
{struct _handler_cons _tmp11A;_push_handler(& _tmp11A);{int _tmp11C=0;if(setjmp(_tmp11A.handler))_tmp11C=1;if(!_tmp11C){
{struct _tuple17*_tmp11D=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp126,(int)_tmp123);struct _tuple17*_tmp11E=_tmp11D;struct Cyc_Absyn_Tqual _tmp11F;_LL103: _tmp11F=_tmp11E->f1;_LL104:;
if(!_tmp11F.real_const){_npop_handler(0);return;}}
# 738
;_pop_handler();}else{void*_tmp11B=(void*)_exn_thrown;void*_tmp120=_tmp11B;void*_tmp121;if(((struct Cyc_List_Nth_exn_struct*)_tmp120)->tag == Cyc_List_Nth){_LL106: _LL107:
# 740
 return;}else{_LL108: _tmp121=_tmp120;_LL109:(int)_rethrow(_tmp121);}_LL105:;}};}
goto _LLF6;}default: _LLFD: _LLFE:
 goto _LLF6;}_LLF6:;}
# 744
goto _LLDF;case 20: _LLEA: _tmp13D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp113)->f1;_tmp13C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp113)->f2;_LLEB:
# 746
{void*_tmp127=Cyc_Tcutil_compress((void*)_check_null(_tmp13D->topt));void*_tmp128=_tmp127;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Aggrdecl**_tmp129;switch(*((int*)_tmp128)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp128)->f1).aggr_info).KnownAggr).tag == 2){_LL10B: _tmp129=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp128)->f1).aggr_info).KnownAggr).val;_LL10C: {
# 748
struct Cyc_Absyn_Aggrfield*sf=
_tmp129 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp129,_tmp13C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}}else{goto _LL10F;}case 12: _LL10D: _tmp12A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp128)->f2;_LL10E: {
# 753
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12A,_tmp13C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}default: _LL10F: _LL110:
 goto _LL10A;}_LL10A:;}
# 758
goto _LLDF;case 21: _LLEC: _tmp13F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp113)->f1;_tmp13E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp113)->f2;_LLED:
# 760
{void*_tmp12B=Cyc_Tcutil_compress((void*)_check_null(_tmp13F->topt));void*_tmp12C=_tmp12B;void*_tmp131;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C)->tag == 5){_LL112: _tmp131=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C)->f1).elt_typ;_LL113:
# 762
{void*_tmp12D=Cyc_Tcutil_compress(_tmp131);void*_tmp12E=_tmp12D;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Aggrdecl**_tmp12F;switch(*((int*)_tmp12E)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12E)->f1).aggr_info).KnownAggr).tag == 2){_LL117: _tmp12F=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12E)->f1).aggr_info).KnownAggr).val;_LL118: {
# 764
struct Cyc_Absyn_Aggrfield*sf=
_tmp12F == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp12F,_tmp13E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}}else{goto _LL11B;}case 12: _LL119: _tmp130=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12E)->f2;_LL11A: {
# 769
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp130,_tmp13E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}default: _LL11B: _LL11C:
 goto _LL116;}_LL116:;}
# 774
goto _LL111;}else{_LL114: _LL115:
 goto _LL111;}_LL111:;}
# 777
goto _LLDF;case 19: _LLEE: _tmp140=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp113)->f1;_LLEF:
# 779
{void*_tmp132=Cyc_Tcutil_compress((void*)_check_null(_tmp140->topt));void*_tmp133=_tmp132;struct Cyc_Absyn_Tqual _tmp135;struct Cyc_Absyn_Tqual _tmp134;switch(*((int*)_tmp133)){case 5: _LL11E: _tmp134=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_tq;_LL11F:
 _tmp135=_tmp134;goto _LL121;case 8: _LL120: _tmp135=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).tq;_LL121:
 if(!_tmp135.real_const)return;goto _LL11D;default: _LL122: _LL123:
 goto _LL11D;}_LL11D:;}
# 784
goto _LLDF;case 11: _LLF0: _tmp141=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp113)->f1;_LLF1:
 _tmp142=_tmp141;goto _LLF3;case 12: _LLF2: _tmp142=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp113)->f1;_LLF3:
 Cyc_Tcexp_check_writable(te,_tmp142);return;default: _LLF4: _LLF5:
 goto _LLDF;}_LLDF:;}
# 789
({struct Cyc_String_pa_PrintArg_struct _tmp145;_tmp145.tag=0;_tmp145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp143[1]={& _tmp145};Cyc_Tcutil_terr(e->loc,({const char*_tmp144="attempt to write a const location: %s";_tag_dyneither(_tmp144,sizeof(char),38);}),_tag_dyneither(_tmp143,sizeof(void*),1));});});}
# 792
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 795
struct _RegionHandle _tmp146=_new_region("temp");struct _RegionHandle*temp=& _tmp146;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 798
if(!Cyc_Absyn_is_lvalue(e)){
void*_tmp149=({void*_tmp147=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp148="increment/decrement of non-lvalue";_tag_dyneither(_tmp148,sizeof(char),34);}),_tag_dyneither(_tmp147,sizeof(void*),0));});_npop_handler(0);return _tmp149;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 803
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp14A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp14B="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp14B,sizeof(char),50);}),_tag_dyneither(_tmp14A,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp14C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp14D="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp14D,sizeof(char),54);}),_tag_dyneither(_tmp14C,sizeof(void*),0));});}else{
# 812
({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp14E[1]={& _tmp150};Cyc_Tcutil_terr(e->loc,({const char*_tmp14F="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp14F,sizeof(char),44);}),_tag_dyneither(_tmp14E,sizeof(void*),1));});});}}{
# 814
void*_tmp151=t;_npop_handler(0);return _tmp151;};};
# 795
;_pop_region(temp);}
# 818
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 820
struct _RegionHandle _tmp152=_new_region("r");struct _RegionHandle*r=& _tmp152;_push_region(r);
Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,({const char*_tmp153="conditional expression";_tag_dyneither(_tmp153,sizeof(char),23);}));
# 823
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 829
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp154=({struct Cyc_List_List _tmp15C;_tmp15C.hd=e3;_tmp15C.tl=0;_tmp15C;});
struct Cyc_List_List _tmp155=({struct Cyc_List_List _tmp15B;_tmp15B.hd=e2;_tmp15B.tl=& _tmp154;_tmp15B;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp155)){
({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0;_tmp158.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));({void*_tmp156[2]={& _tmp158,& _tmp159};Cyc_Tcutil_terr(loc,({const char*_tmp157="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp157,sizeof(char),48);}),_tag_dyneither(_tmp156,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}{
# 837
void*_tmp15A=t;_npop_handler(0);return _tmp15A;};};};
# 820
;_pop_region(r);}
# 841
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 843
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp15D="logical-and expression";_tag_dyneither(_tmp15D,sizeof(char),23);}));
Cyc_Tcexp_tcTest(te,e2,({const char*_tmp15E="logical-and expression";_tag_dyneither(_tmp15E,sizeof(char),23);}));
return Cyc_Absyn_sint_typ;}
# 849
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 851
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp15F="logical-or expression";_tag_dyneither(_tmp15F,sizeof(char),22);}));
Cyc_Tcexp_tcTest(te,e2,({const char*_tmp160="logical-or expression";_tag_dyneither(_tmp160,sizeof(char),22);}));
return Cyc_Absyn_sint_typ;}
# 857
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 864
{struct _RegionHandle _tmp161=_new_region("r");struct _RegionHandle*r=& _tmp161;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);;_pop_region(r);}{
# 867
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 871
{void*_tmp162=Cyc_Tcutil_compress(t1);void*_tmp163=_tmp162;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp163)->tag == 8){_LL125: _LL126:
({void*_tmp164=0;Cyc_Tcutil_terr(loc,({const char*_tmp165="cannot assign to an array";_tag_dyneither(_tmp165,sizeof(char),26);}),_tag_dyneither(_tmp164,sizeof(void*),0));});goto _LL124;}else{_LL127: _LL128:
 goto _LL124;}_LL124:;}
# 876
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp166=0;Cyc_Tcutil_terr(loc,({const char*_tmp167="type is abstract (can't determine size).";_tag_dyneither(_tmp167,sizeof(char),41);}),_tag_dyneither(_tmp166,sizeof(void*),0));});
# 880
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp168=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp169="assignment to non-lvalue";_tag_dyneither(_tmp169,sizeof(char),25);}),_tag_dyneither(_tmp168,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
{struct _RegionHandle _tmp16A=_new_region("temp");struct _RegionHandle*temp=& _tmp16A;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2))
({void*_tmp16B=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmp16C="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp16C,sizeof(char),49);}),_tag_dyneither(_tmp16B,sizeof(void*),0));});
# 885
;_pop_region(temp);}
# 888
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp16D=({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0;_tmp171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0;_tmp170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp16E[2]={& _tmp170,& _tmp171};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp16F="type mismatch: %s != %s";_tag_dyneither(_tmp16F,sizeof(char),24);}),_tag_dyneither(_tmp16E,sizeof(void*),2));});});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp16D;}}else{
# 896
enum Cyc_Absyn_Primop _tmp172=(enum Cyc_Absyn_Primop)po->v;
void*_tmp173=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp172,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp173,t1) || Cyc_Tcutil_coerceable(_tmp173) && Cyc_Tcutil_coerceable(t1))){
void*_tmp174=({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 902
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 901
Cyc_Absynpp_typ2string(t1));({void*_tmp175[2]={& _tmp177,& _tmp178};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp176="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp176,sizeof(char),82);}),_tag_dyneither(_tmp175,sizeof(void*),2));});});});
# 903
Cyc_Tcutil_unify(_tmp173,t1);
Cyc_Tcutil_explain_failure();
return _tmp174;}
# 907
return _tmp173;}
# 909
return t1;};};}
# 913
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _RegionHandle _tmp179=_new_region("r");struct _RegionHandle*r=& _tmp179;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);
# 915
;_pop_region(r);}
# 918
return(void*)_check_null(e2->topt);}
# 922
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 926
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 929
{void*_tmp17A=Cyc_Tcutil_compress(t1);void*_tmp17B=_tmp17A;switch(*((int*)_tmp17B)){case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp17B)->f1 == 0){_LL12A: _LL12B:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL129;}else{goto _LL130;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp17B)->f2){case Cyc_Absyn_Char_sz: _LL12C: _LL12D:
 goto _LL12F;case Cyc_Absyn_Short_sz: _LL12E: _LL12F:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL129;default: goto _LL130;}default: _LL130: _LL131:
 goto _LL129;}_LL129:;}
# 936
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp17C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp17D=_tmp17C;struct _tuple1*_tmp181;struct Cyc_List_List*_tmp180;unsigned int _tmp17F;enum Cyc_Absyn_Scope _tmp17E;_LL133: _tmp181=_tmp17D.name;_tmp180=_tmp17D.typs;_tmp17F=_tmp17D.loc;_tmp17E=_tmp17D.sc;_LL134:;
# 939
if(_tmp180 == 0  || _tmp180->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp180->hd)).f2);
# 942
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 947
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp182=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp183=_tmp182;struct _tuple1*_tmp187;struct Cyc_List_List*_tmp186;unsigned int _tmp185;enum Cyc_Absyn_Scope _tmp184;_LL136: _tmp187=_tmp183.name;_tmp186=_tmp183.typs;_tmp185=_tmp183.loc;_tmp184=_tmp183.sc;_LL137:;
# 950
if(_tmp186 == 0  || _tmp186->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp186->hd)).f2);
# 954
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 959
({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp18A;_tmp18A.tag=0;_tmp18A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));({void*_tmp188[2]={& _tmp18A,& _tmp18B};Cyc_Tcutil_terr(e->loc,({const char*_tmp189="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp189,sizeof(char),54);}),_tag_dyneither(_tmp188,sizeof(void*),2));});});});
return 0;}
# 965
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 971
struct Cyc_List_List*_tmp18C=args;
int _tmp18D=0;
struct _RegionHandle _tmp18E=_new_region("ter");struct _RegionHandle*ter=& _tmp18E;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp18F=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp190=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp18F);
Cyc_Tcexp_tcExp(_tmp190,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 981
void*_tmp191=t;void*_tmp1F9;struct Cyc_Absyn_Tqual _tmp1F8;void*_tmp1F7;union Cyc_Absyn_Constraint*_tmp1F6;union Cyc_Absyn_Constraint*_tmp1F5;union Cyc_Absyn_Constraint*_tmp1F4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191)->tag == 5){_LL139: _tmp1F9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191)->f1).elt_typ;_tmp1F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191)->f1).elt_tq;_tmp1F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191)->f1).ptr_atts).rgn;_tmp1F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191)->f1).ptr_atts).nullable;_tmp1F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191)->f1).ptr_atts).bounds;_tmp1F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191)->f1).ptr_atts).zero_term;_LL13A:
# 986
 Cyc_Tcenv_check_rgn_accessible(_tmp190,loc,_tmp1F7);
# 988
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1F5);{
void*_tmp192=Cyc_Tcutil_compress(_tmp1F9);void*_tmp193=_tmp192;struct Cyc_List_List*_tmp1EF;void*_tmp1EE;struct Cyc_Absyn_Tqual _tmp1ED;void*_tmp1EC;struct Cyc_List_List*_tmp1EB;int _tmp1EA;struct Cyc_Absyn_VarargInfo*_tmp1E9;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_List_List*_tmp1E3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->tag == 9){_LL13E: _tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).tvars;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).effect;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).ret_tqual;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).ret_typ;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).args;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).c_varargs;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).cyc_varargs;_tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).rgn_po;_tmp1E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).attributes;_tmp1E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).requires_clause;_tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).requires_relns;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).ensures_clause;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp193)->f1).ensures_relns;_LL13F:
# 993
 if(_tmp1EF != 0  || _tmp1E8 != 0)
({void*_tmp194=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp195="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp195,sizeof(char),79);}),_tag_dyneither(_tmp194,sizeof(void*),0));});
# 997
if(topt != 0)Cyc_Tcutil_unify(_tmp1EC,*topt);
# 999
while(_tmp18C != 0  && _tmp1EB != 0){
# 1001
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18C->hd;
void*t2=(*((struct _tuple9*)_tmp1EB->hd)).f3;
Cyc_Tcexp_tcExp(_tmp190,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp190,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp1A5="actual argument has type ";_tag_dyneither(_tmp1A5,sizeof(char),26);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp1A4=" but formal has type ";_tag_dyneither(_tmp1A4,sizeof(char),22);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70)
({void*_tmp196=0;Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp19B;_tmp19B.tag=0;_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp19A;_tmp19A.tag=0;_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0;_tmp199.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp197[4]={& _tmp199,& _tmp19A,& _tmp19B,& _tmp19C};Cyc_aprintf(({const char*_tmp198="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp198,sizeof(char),15);}),_tag_dyneither(_tmp197,sizeof(void*),4));});});});});}),_tag_dyneither(_tmp196,sizeof(void*),0));});else{
# 1013
({void*_tmp19D=0;Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp1A2;_tmp1A2.tag=0;_tmp1A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp19E[4]={& _tmp1A0,& _tmp1A1,& _tmp1A2,& _tmp1A3};Cyc_aprintf(({const char*_tmp19F="%s%s%s%s.";_tag_dyneither(_tmp19F,sizeof(char),10);}),_tag_dyneither(_tmp19E,sizeof(void*),4));});});});});}),_tag_dyneither(_tmp19D,sizeof(void*),0));});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1018
if(alias_coercion)
*alias_arg_exps=({struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->hd=(void*)_tmp18D;_tmp1A6->tl=*alias_arg_exps;_tmp1A6;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1))
({void*_tmp1A7=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1A8="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1A8,sizeof(char),49);}),_tag_dyneither(_tmp1A7,sizeof(void*),0));});
_tmp18C=_tmp18C->tl;
_tmp1EB=_tmp1EB->tl;
++ _tmp18D;}{
# 1029
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1E7;for(0;a != 0;a=a->tl){
void*_tmp1A9=(void*)a->hd;void*_tmp1AA=_tmp1A9;enum Cyc_Absyn_Format_Type _tmp1B8;int _tmp1B7;int _tmp1B6;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1AA)->tag == 19){_LL143: _tmp1B8=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1AA)->f1;_tmp1B7=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1AA)->f2;_tmp1B6=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1AA)->f3;_LL144:
# 1033
{struct _handler_cons _tmp1AB;_push_handler(& _tmp1AB);{int _tmp1AD=0;if(setjmp(_tmp1AB.handler))_tmp1AD=1;if(!_tmp1AD){
# 1035
{struct Cyc_Absyn_Exp*_tmp1AE=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1B7 - 1);
# 1037
struct Cyc_Core_Opt*fmt_args;
if(_tmp1B6 == 0)
fmt_args=0;else{
# 1041
fmt_args=({struct Cyc_Core_Opt*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1B6 - 1);_tmp1AF;});}
args_already_checked=1;{
struct _RegionHandle _tmp1B0=_new_region("temp");struct _RegionHandle*temp=& _tmp1B0;_push_region(temp);
{enum Cyc_Absyn_Format_Type _tmp1B1=_tmp1B8;if(_tmp1B1 == Cyc_Absyn_Printf_ft){_LL148: _LL149:
# 1046
 Cyc_Tcexp_check_format_args(_tmp190,_tmp1AE,fmt_args,_tmp1B6 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1049
goto _LL147;}else{_LL14A: _LL14B:
# 1051
 Cyc_Tcexp_check_format_args(_tmp190,_tmp1AE,fmt_args,_tmp1B6 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1054
goto _LL147;}_LL147:;}
# 1044
;_pop_region(temp);};}
# 1035
;_pop_handler();}else{void*_tmp1AC=(void*)_exn_thrown;void*_tmp1B2=_tmp1AC;void*_tmp1B5;if(((struct Cyc_List_Nth_exn_struct*)_tmp1B2)->tag == Cyc_List_Nth){_LL14D: _LL14E:
# 1058
({void*_tmp1B3=0;Cyc_Tcutil_terr(loc,({const char*_tmp1B4="bad format arguments";_tag_dyneither(_tmp1B4,sizeof(char),21);}),_tag_dyneither(_tmp1B3,sizeof(void*),0));});goto _LL14C;}else{_LL14F: _tmp1B5=_tmp1B2;_LL150:(int)_rethrow(_tmp1B5);}_LL14C:;}};}
# 1060
goto _LL142;}else{_LL145: _LL146:
 goto _LL142;}_LL142:;}}
# 1064
if(_tmp1EB != 0)({void*_tmp1B9=0;Cyc_Tcutil_terr(loc,({const char*_tmp1BA="too few arguments for function";_tag_dyneither(_tmp1BA,sizeof(char),31);}),_tag_dyneither(_tmp1B9,sizeof(void*),0));});else{
# 1066
if((_tmp18C != 0  || _tmp1EA) || _tmp1E9 != 0){
if(_tmp1EA)
for(0;_tmp18C != 0;_tmp18C=_tmp18C->tl){
Cyc_Tcexp_tcExp(_tmp190,0,(struct Cyc_Absyn_Exp*)_tmp18C->hd);}else{
if(_tmp1E9 == 0)
({void*_tmp1BB=0;Cyc_Tcutil_terr(loc,({const char*_tmp1BC="too many arguments for function";_tag_dyneither(_tmp1BC,sizeof(char),32);}),_tag_dyneither(_tmp1BB,sizeof(void*),0));});else{
# 1073
struct Cyc_Absyn_VarargInfo _tmp1BD=*_tmp1E9;struct Cyc_Absyn_VarargInfo _tmp1BE=_tmp1BD;void*_tmp1DD;int _tmp1DC;_LL152: _tmp1DD=_tmp1BE.type;_tmp1DC=_tmp1BE.inject;_LL153:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1BF=({struct Cyc_Absyn_VarargCallInfo*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->num_varargs=0;_tmp1DB->injectors=0;_tmp1DB->vai=_tmp1E9;_tmp1DB;});
# 1077
*vararg_call_info=_tmp1BF;
# 1079
if(!_tmp1DC)
# 1081
for(0;_tmp18C != 0;(_tmp18C=_tmp18C->tl,_tmp18D ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18C->hd;
++ _tmp1BF->num_varargs;
Cyc_Tcexp_tcExp(_tmp190,& _tmp1DD,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp190,e1,_tmp1DD,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp1C2;_tmp1C2.tag=0;_tmp1C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1DD));({void*_tmp1C0[2]={& _tmp1C2,& _tmp1C3};Cyc_Tcutil_terr(loc,({const char*_tmp1C1="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1C1,sizeof(char),49);}),_tag_dyneither(_tmp1C0,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 1091
if(alias_coercion)
*alias_arg_exps=({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=(void*)_tmp18D;_tmp1C4->tl=*alias_arg_exps;_tmp1C4;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp1DD) && !
Cyc_Tcutil_is_noalias_path(ter,e1))
({void*_tmp1C5=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1C6="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1C6,sizeof(char),49);}),_tag_dyneither(_tmp1C5,sizeof(void*),0));});}else{
# 1100
void*_tmp1C7=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1DD));void*_tmp1C8=_tmp1C7;struct Cyc_Absyn_Datatypedecl*_tmp1DA;struct Cyc_List_List*_tmp1D9;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C8)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C8)->f1).datatype_info).KnownDatatype).tag == 2){_LL155: _tmp1DA=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C8)->f1).datatype_info).KnownDatatype).val;_tmp1D9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C8)->f1).targs;_LL156: {
# 1104
struct Cyc_Absyn_Datatypedecl*_tmp1C9=*Cyc_Tcenv_lookup_datatypedecl(_tmp190,loc,_tmp1DA->name);
struct Cyc_List_List*fields=0;
if(_tmp1C9->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1CC;_tmp1CC.tag=0;_tmp1CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1DD));({void*_tmp1CA[1]={& _tmp1CC};Cyc_Tcutil_terr(loc,({const char*_tmp1CB="can't inject into abstract datatype %s";_tag_dyneither(_tmp1CB,sizeof(char),39);}),_tag_dyneither(_tmp1CA,sizeof(void*),1));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C9->fields))->v;}
# 1116
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp1DD),Cyc_Tcenv_curr_rgn(_tmp190)))
({struct Cyc_String_pa_PrintArg_struct _tmp1D0;_tmp1D0.tag=0;_tmp1D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp190)));({struct Cyc_String_pa_PrintArg_struct _tmp1CF;_tmp1CF.tag=0;_tmp1CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1DD));({void*_tmp1CD[2]={& _tmp1CF,& _tmp1D0};Cyc_Tcutil_terr(loc,({const char*_tmp1CE="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1CE,sizeof(char),49);}),_tag_dyneither(_tmp1CD,sizeof(void*),2));});});});{
# 1120
struct _RegionHandle _tmp1D1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1D1;_push_region(rgn);{
struct Cyc_List_List*_tmp1D2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1C9->tvs,_tmp1D9);
for(0;_tmp18C != 0;_tmp18C=_tmp18C->tl){
++ _tmp1BF->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18C->hd;
# 1126
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp190,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1))
({void*_tmp1D3=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1D4="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1D4,sizeof(char),49);}),_tag_dyneither(_tmp1D3,sizeof(void*),0));});}{
# 1132
struct Cyc_Absyn_Datatypefield*_tmp1D5=Cyc_Tcexp_tcInjection(_tmp190,e1,Cyc_Tcutil_pointer_elt_type(_tmp1DD),rgn,_tmp1D2,fields);
# 1134
if(_tmp1D5 != 0)
_tmp1BF->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BF->injectors,({struct Cyc_List_List*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=_tmp1D5;_tmp1D6->tl=0;_tmp1D6;}));};};}}
# 1140
_npop_handler(0);goto _LL154;
# 1120
;_pop_region(rgn);};}}else{goto _LL157;}}else{_LL157: _LL158:
# 1141
({void*_tmp1D7=0;Cyc_Tcutil_terr(loc,({const char*_tmp1D8="bad inject vararg type";_tag_dyneither(_tmp1D8,sizeof(char),23);}),_tag_dyneither(_tmp1D7,sizeof(void*),0));});goto _LL154;}_LL154:;}};}}}}
# 1146
if(*alias_arg_exps == 0)
# 1155 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp190,loc,(void*)_check_null(_tmp1EE));{
# 1157
void*_tmp1DE=_tmp1EC;_npop_handler(0);return _tmp1DE;};};}else{_LL140: _LL141: {
void*_tmp1E2=({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0;_tmp1E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1DF[1]={& _tmp1E1};Cyc_Tcexp_expr_err(_tmp190,loc,topt,({const char*_tmp1E0="expected pointer to function but found %s";_tag_dyneither(_tmp1E0,sizeof(char),42);}),_tag_dyneither(_tmp1DF,sizeof(void*),1));});});_npop_handler(0);return _tmp1E2;}}_LL13D:;};}else{_LL13B: _LL13C: {
# 1160
void*_tmp1F3=({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1F0[1]={& _tmp1F2};Cyc_Tcexp_expr_err(_tmp190,loc,topt,({const char*_tmp1F1="expected pointer to function but found %s";_tag_dyneither(_tmp1F1,sizeof(char),42);}),_tag_dyneither(_tmp1F0,sizeof(void*),1));});});_npop_handler(0);return _tmp1F3;}}_LL138:;};}
# 974 "tcexp.cyc"
;_pop_region(ter);}
# 1166 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1FA=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1FA,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1FA,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0;_tmp1FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp1FD;_tmp1FD.tag=0;_tmp1FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1171
Cyc_Absynpp_typ2string(_tmp1FA));({void*_tmp1FB[2]={& _tmp1FD,& _tmp1FE};Cyc_Tcutil_terr(loc,({const char*_tmp1FC="expected %s but found %s";_tag_dyneither(_tmp1FC,sizeof(char),25);}),_tag_dyneither(_tmp1FB,sizeof(void*),2));});});});
# 1173
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1178
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1180
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1182
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1FF=t1;void*_tmp221;struct Cyc_Absyn_Tqual _tmp220;void*_tmp21F;union Cyc_Absyn_Constraint*_tmp21E;union Cyc_Absyn_Constraint*_tmp21D;union Cyc_Absyn_Constraint*_tmp21C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF)->tag == 5){_LL15A: _tmp221=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF)->f1).elt_typ;_tmp220=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF)->f1).elt_tq;_tmp21F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF)->f1).ptr_atts).rgn;_tmp21E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF)->f1).ptr_atts).nullable;_tmp21D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF)->f1).ptr_atts).bounds;_tmp21C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FF)->f1).ptr_atts).zero_term;_LL15B:
# 1186
{void*_tmp200=Cyc_Tcutil_compress(_tmp221);void*_tmp201=_tmp200;struct Cyc_List_List*_tmp21B;void*_tmp21A;struct Cyc_Absyn_Tqual _tmp219;void*_tmp218;struct Cyc_List_List*_tmp217;int _tmp216;struct Cyc_Absyn_VarargInfo*_tmp215;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp213;struct Cyc_Absyn_Exp*_tmp212;struct Cyc_List_List*_tmp211;struct Cyc_Absyn_Exp*_tmp210;struct Cyc_List_List*_tmp20F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->tag == 9){_LL15F: _tmp21B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).tvars;_tmp21A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).effect;_tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).ret_tqual;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).ret_typ;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).args;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).c_varargs;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).cyc_varargs;_tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).rgn_po;_tmp213=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).attributes;_tmp212=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).requires_clause;_tmp211=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).requires_relns;_tmp210=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).ensures_clause;_tmp20F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp201)->f1).ensures_relns;_LL160: {
# 1188
struct _RegionHandle _tmp202=_new_region("temp");struct _RegionHandle*temp=& _tmp202;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1191
for(0;ts != 0  && _tmp21B != 0;(ts=ts->tl,_tmp21B=_tmp21B->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp21B->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp203=_region_malloc(temp,sizeof(*_tmp203));_tmp203->hd=({struct _tuple13*_tmp204=_region_malloc(temp,sizeof(*_tmp204));_tmp204->f1=(struct Cyc_Absyn_Tvar*)_tmp21B->hd;_tmp204->f2=(void*)ts->hd;_tmp204;});_tmp203->tl=instantiation;_tmp203;});}
# 1198
if(ts != 0){
void*_tmp207=({void*_tmp205=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp206="too many type variables in instantiation";_tag_dyneither(_tmp206,sizeof(char),41);}),_tag_dyneither(_tmp205,sizeof(void*),0));});_npop_handler(0);return _tmp207;}
# 1204
if(_tmp21B == 0){
_tmp214=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp214);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp214);
_tmp214=0;}{
# 1209
void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp20D;_tmp20D.tag=9;_tmp20D.f1=({struct Cyc_Absyn_FnInfo _tmp20E;_tmp20E.tvars=_tmp21B;_tmp20E.effect=_tmp21A;_tmp20E.ret_tqual=_tmp219;_tmp20E.ret_typ=_tmp218;_tmp20E.args=_tmp217;_tmp20E.c_varargs=_tmp216;_tmp20E.cyc_varargs=_tmp215;_tmp20E.rgn_po=_tmp214;_tmp20E.attributes=_tmp213;_tmp20E.requires_clause=_tmp212;_tmp20E.requires_relns=_tmp211;_tmp20E.ensures_clause=_tmp210;_tmp20E.ensures_relns=_tmp20F;_tmp20E;});_tmp20D;});_tmp20C;}));
# 1214
void*_tmp20B=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp209;_tmp209.tag=5;_tmp209.f1=({struct Cyc_Absyn_PtrInfo _tmp20A;_tmp20A.elt_typ=new_fn_typ;_tmp20A.elt_tq=_tmp220;_tmp20A.ptr_atts=({(_tmp20A.ptr_atts).rgn=_tmp21F;(_tmp20A.ptr_atts).nullable=_tmp21E;(_tmp20A.ptr_atts).bounds=_tmp21D;(_tmp20A.ptr_atts).zero_term=_tmp21C;(_tmp20A.ptr_atts).ptrloc=0;_tmp20A.ptr_atts;});_tmp20A;});_tmp209;});_tmp208;});_npop_handler(0);return _tmp20B;};}
# 1189
;_pop_region(temp);}}else{_LL161: _LL162:
# 1216
 goto _LL15E;}_LL15E:;}
# 1218
goto _LL159;}else{_LL15C: _LL15D:
 goto _LL159;}_LL159:;}
# 1221
return({struct Cyc_String_pa_PrintArg_struct _tmp224;_tmp224.tag=0;_tmp224.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({void*_tmp222[1]={& _tmp224};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp223="expecting polymorphic type but found %s";_tag_dyneither(_tmp223,sizeof(char),40);}),_tag_dyneither(_tmp222,sizeof(void*),1));});});};}
# 1226
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1228
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk,1,0,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1232
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1237
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1244
Cyc_Tcutil_unify(t2,t);{
void*_tmp225=({struct Cyc_String_pa_PrintArg_struct _tmp229;_tmp229.tag=0;_tmp229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp228;_tmp228.tag=0;_tmp228.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));({void*_tmp226[2]={& _tmp228,& _tmp229};Cyc_Tcexp_expr_err(te,loc,& t,({const char*_tmp227="cannot cast %s to %s";_tag_dyneither(_tmp227,sizeof(char),21);}),_tag_dyneither(_tmp226,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();
return _tmp225;};}}}
# 1254
{struct _tuple0 _tmp22A=({struct _tuple0 _tmp235;_tmp235.f1=e->r;_tmp235.f2=Cyc_Tcutil_compress(t);_tmp235;});struct _tuple0 _tmp22B=_tmp22A;int _tmp234;union Cyc_Absyn_Constraint*_tmp233;union Cyc_Absyn_Constraint*_tmp232;union Cyc_Absyn_Constraint*_tmp231;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp22B.f1)->tag == 33){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B.f2)->tag == 5){_LL164: _tmp234=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp22B.f1)->f1).fat_result;_tmp233=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B.f2)->f1).ptr_atts).nullable;_tmp232=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B.f2)->f1).ptr_atts).bounds;_tmp231=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B.f2)->f1).ptr_atts).zero_term;_LL165:
# 1258
 if((_tmp234  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp231)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp233)){
void*_tmp22C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp232);void*_tmp22D=_tmp22C;struct Cyc_Absyn_Exp*_tmp230;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22D)->tag == 1){_LL169: _tmp230=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22D)->f1;_LL16A:
# 1261
 if((Cyc_Evexp_eval_const_uint_exp(_tmp230)).f1 == 1)
({void*_tmp22E=0;Cyc_Tcutil_warn(loc,({const char*_tmp22F="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp22F,sizeof(char),60);}),_tag_dyneither(_tmp22E,sizeof(void*),0));});
goto _LL168;}else{_LL16B: _LL16C:
 goto _LL168;}_LL168:;}
# 1267
goto _LL163;}else{goto _LL166;}}else{_LL166: _LL167:
 goto _LL163;}_LL163:;}
# 1270
return t;};}
# 1274
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1276
void**_tmp236=0;
struct Cyc_Absyn_Tqual _tmp237=Cyc_Absyn_empty_tqual(0);
int _tmp238=0;
if(topt != 0){
void*_tmp239=Cyc_Tcutil_compress(*topt);void*_tmp23A=_tmp239;void**_tmp23D;struct Cyc_Absyn_Tqual _tmp23C;union Cyc_Absyn_Constraint*_tmp23B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A)->tag == 5){_LL16E: _tmp23D=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A)->f1).elt_typ;_tmp23C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A)->f1).elt_tq;_tmp23B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23A)->f1).ptr_atts).nullable;_LL16F:
# 1282
 _tmp236=_tmp23D;
_tmp237=_tmp23C;
_tmp238=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp23B);
goto _LL16D;}else{_LL170: _LL171:
 goto _LL16D;}_LL16D:;}{
# 1296
struct _RegionHandle _tmp23E=_new_region("r");struct _RegionHandle*r=& _tmp23E;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp236,e);
# 1300
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt)))
({void*_tmp23F=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp240="Cannot take the address of an alias-free path";_tag_dyneither(_tmp240,sizeof(char),46);}),_tag_dyneither(_tmp23F,sizeof(void*),0));});
# 1307
{void*_tmp241=e->r;void*_tmp242=_tmp241;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Exp*_tmp246;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp242)->tag == 22){_LL173: _tmp247=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp242)->f1;_tmp246=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp242)->f2;_LL174:
# 1309
{void*_tmp243=Cyc_Tcutil_compress((void*)_check_null(_tmp247->topt));void*_tmp244=_tmp243;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp244)->tag == 10){_LL178: _LL179:
 goto _LL177;}else{_LL17A: _LL17B:
# 1314
 e0->r=(Cyc_Absyn_add_exp(_tmp247,_tmp246,0))->r;{
void*_tmp245=Cyc_Tcexp_tcPlus(te,_tmp247,_tmp246);_npop_handler(0);return _tmp245;};}_LL177:;}
# 1317
goto _LL172;}else{_LL175: _LL176:
 goto _LL172;}_LL172:;}
# 1322
{void*_tmp248=e->r;void*_tmp249=_tmp248;switch(*((int*)_tmp249)){case 20: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp249)->f3 == 1){_LL17D: _LL17E:
 goto _LL180;}else{goto _LL181;}case 21: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp249)->f3 == 1){_LL17F: _LL180:
# 1325
({void*_tmp24A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp24B="cannot take the address of a @tagged union member";_tag_dyneither(_tmp24B,sizeof(char),50);}),_tag_dyneither(_tmp24A,sizeof(void*),0));});
goto _LL17C;}else{goto _LL181;}default: _LL181: _LL182:
 goto _LL17C;}_LL17C:;}{
# 1331
struct _tuple14 _tmp24C=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp24D=_tmp24C;int _tmp252;void*_tmp251;_LL184: _tmp252=_tmp24D.f1;_tmp251=_tmp24D.f2;_LL185:;
# 1333
if(Cyc_Tcutil_is_noalias_region(_tmp251,0))
({void*_tmp24E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp24F="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp24F,sizeof(char),60);}),_tag_dyneither(_tmp24E,sizeof(void*),0));});{
# 1336
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp252){
tq.print_const=1;
tq.real_const=1;}{
# 1342
void*t=_tmp238?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp251,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp251,tq,Cyc_Absyn_false_conref);
void*_tmp250=t;_npop_handler(0);return _tmp250;};};};
# 1296
;_pop_region(r);};}
# 1349
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1353
return Cyc_Absyn_uint_typ;
# 1355
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmp253[1]={& _tmp255};Cyc_Tcutil_terr(loc,({const char*_tmp254="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp254,sizeof(char),55);}),_tag_dyneither(_tmp253,sizeof(void*),1));});});
if(topt != 0){
void*_tmp256=Cyc_Tcutil_compress(*topt);void*_tmp257=_tmp256;void*_tmp25D;void*_tmp25C;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp257)->tag == 19){_LL187: _tmp25D=_tmp257;_tmp25C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp257)->f1;_LL188: {
# 1364
struct Cyc_Absyn_Exp*_tmp258=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp259=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp25B;_tmp25B.tag=18;_tmp25B.f1=_tmp258;_tmp25B;});_tmp25A;});
if(Cyc_Tcutil_unify(_tmp25C,(void*)_tmp259))return _tmp25D;
goto _LL186;}}else{_LL189: _LL18A:
 goto _LL186;}_LL186:;}
# 1370
return Cyc_Absyn_uint_typ;}
# 1373
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1380
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1382
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp25E=fs;
void*_tmp25F=t_orig;
for(0;_tmp25E != 0;_tmp25E=_tmp25E->tl){
void*_tmp260=(void*)_tmp25E->hd;
void*_tmp261=_tmp260;unsigned int _tmp29B;struct _dyneither_ptr*_tmp29A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp261)->tag == 0){_LL18C: _tmp29A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp261)->f1;_LL18D: {
# 1390
int bad_type=1;
{void*_tmp262=Cyc_Tcutil_compress(_tmp25F);void*_tmp263=_tmp262;struct Cyc_List_List*_tmp26B;struct Cyc_Absyn_Aggrdecl**_tmp26A;switch(*((int*)_tmp263)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp263)->f1).aggr_info).KnownAggr).tag == 2){_LL191: _tmp26A=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp263)->f1).aggr_info).KnownAggr).val;_LL192:
# 1393
 if((*_tmp26A)->impl == 0)goto _LL190;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp29A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp26A)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp29A);({void*_tmp264[1]={& _tmp266};Cyc_Tcutil_terr(loc,({const char*_tmp265="no field of struct/union has name %s";_tag_dyneither(_tmp265,sizeof(char),37);}),_tag_dyneither(_tmp264,sizeof(void*),1));});});else{
# 1398
_tmp25F=t2;}
bad_type=0;
goto _LL190;};}else{goto _LL195;}case 12: _LL193: _tmp26B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp263)->f2;_LL194: {
# 1402
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp29A,_tmp26B);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp269;_tmp269.tag=0;_tmp269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp29A);({void*_tmp267[1]={& _tmp269};Cyc_Tcutil_terr(loc,({const char*_tmp268="no field of struct/union has name %s";_tag_dyneither(_tmp268,sizeof(char),37);}),_tag_dyneither(_tmp267,sizeof(void*),1));});});else{
# 1406
_tmp25F=t2;}
bad_type=0;
goto _LL190;}default: _LL195: _LL196:
 goto _LL190;}_LL190:;}
# 1411
if(bad_type){
if(_tmp25E == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp26E;_tmp26E.tag=0;_tmp26E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25F));({void*_tmp26C[1]={& _tmp26E};Cyc_Tcutil_terr(loc,({const char*_tmp26D="%s is not a known struct/union type";_tag_dyneither(_tmp26D,sizeof(char),36);}),_tag_dyneither(_tmp26C,sizeof(void*),1));});});else{
# 1415
struct _dyneither_ptr _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp282;_tmp282.tag=0;_tmp282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));({void*_tmp280[1]={& _tmp282};Cyc_aprintf(({const char*_tmp281="(%s)";_tag_dyneither(_tmp281,sizeof(char),5);}),_tag_dyneither(_tmp280,sizeof(void*),1));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp25E;x=x->tl){
void*_tmp270=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp271=_tmp270;unsigned int _tmp27B;struct _dyneither_ptr*_tmp27A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp271)->tag == 0){_LL198: _tmp27A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp271)->f1;_LL199:
# 1420
 _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp275;_tmp275.tag=0;_tmp275.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp27A);({struct Cyc_String_pa_PrintArg_struct _tmp274;_tmp274.tag=0;_tmp274.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26F);({void*_tmp272[2]={& _tmp274,& _tmp275};Cyc_aprintf(({const char*_tmp273="%s.%s";_tag_dyneither(_tmp273,sizeof(char),6);}),_tag_dyneither(_tmp272,sizeof(void*),2));});});});goto _LL197;}else{_LL19A: _tmp27B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp271)->f1;_LL19B:
# 1422
 _tmp26F=({struct Cyc_Int_pa_PrintArg_struct _tmp279;_tmp279.tag=1;_tmp279.f1=(unsigned long)((int)_tmp27B);({struct Cyc_String_pa_PrintArg_struct _tmp278;_tmp278.tag=0;_tmp278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26F);({void*_tmp276[2]={& _tmp278,& _tmp279};Cyc_aprintf(({const char*_tmp277="%s.%d";_tag_dyneither(_tmp277,sizeof(char),6);}),_tag_dyneither(_tmp276,sizeof(void*),2));});});});goto _LL197;}_LL197:;}
# 1424
({struct Cyc_String_pa_PrintArg_struct _tmp27F;_tmp27F.tag=0;_tmp27F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25F));({struct Cyc_String_pa_PrintArg_struct _tmp27E;_tmp27E.tag=0;_tmp27E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26F);({void*_tmp27C[2]={& _tmp27E,& _tmp27F};Cyc_Tcutil_terr(loc,({const char*_tmp27D="%s == %s is not a struct/union type";_tag_dyneither(_tmp27D,sizeof(char),36);}),_tag_dyneither(_tmp27C,sizeof(void*),2));});});});}}
# 1427
goto _LL18B;}}else{_LL18E: _tmp29B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp261)->f1;_LL18F: {
# 1429
int bad_type=1;
{void*_tmp283=Cyc_Tcutil_compress(_tmp25F);void*_tmp284=_tmp283;struct Cyc_Absyn_Datatypefield*_tmp296;struct Cyc_List_List*_tmp295;struct Cyc_List_List*_tmp294;struct Cyc_Absyn_Aggrdecl**_tmp293;switch(*((int*)_tmp284)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp284)->f1).aggr_info).KnownAggr).tag == 2){_LL19D: _tmp293=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp284)->f1).aggr_info).KnownAggr).val;_LL19E:
# 1432
 if((*_tmp293)->impl == 0)
goto _LL19C;
_tmp294=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp293)->impl))->fields;goto _LL1A0;}else{goto _LL1A5;}case 12: _LL19F: _tmp294=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp284)->f2;_LL1A0:
# 1436
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp294)<= _tmp29B)
({struct Cyc_Int_pa_PrintArg_struct _tmp288;_tmp288.tag=1;_tmp288.f1=(unsigned long)((int)_tmp29B);({struct Cyc_Int_pa_PrintArg_struct _tmp287;_tmp287.tag=1;_tmp287.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp294);({void*_tmp285[2]={& _tmp287,& _tmp288};Cyc_Tcutil_terr(loc,({const char*_tmp286="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp286,sizeof(char),46);}),_tag_dyneither(_tmp285,sizeof(void*),2));});});});else{
# 1440
_tmp25F=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp294,(int)_tmp29B))->type;}
bad_type=0;
goto _LL19C;case 10: _LL1A1: _tmp295=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp284)->f1;_LL1A2:
# 1444
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp295)<= _tmp29B)
({struct Cyc_Int_pa_PrintArg_struct _tmp28C;_tmp28C.tag=1;_tmp28C.f1=(unsigned long)((int)_tmp29B);({struct Cyc_Int_pa_PrintArg_struct _tmp28B;_tmp28B.tag=1;_tmp28B.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp295);({void*_tmp289[2]={& _tmp28B,& _tmp28C};Cyc_Tcutil_terr(loc,({const char*_tmp28A="tuple has too few components: %d <= %d";_tag_dyneither(_tmp28A,sizeof(char),39);}),_tag_dyneither(_tmp289,sizeof(void*),2));});});});else{
# 1448
_tmp25F=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp295,(int)_tmp29B)).f2;}
bad_type=0;
goto _LL19C;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp284)->f1).field_info).KnownDatatypefield).tag == 2){_LL1A3: _tmp296=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp284)->f1).field_info).KnownDatatypefield).val).f2;_LL1A4:
# 1452
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp296->typs)< _tmp29B)
({struct Cyc_Int_pa_PrintArg_struct _tmp290;_tmp290.tag=1;_tmp290.f1=(unsigned long)((int)_tmp29B);({struct Cyc_Int_pa_PrintArg_struct _tmp28F;_tmp28F.tag=1;_tmp28F.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp296->typs);({void*_tmp28D[2]={& _tmp28F,& _tmp290};Cyc_Tcutil_terr(loc,({const char*_tmp28E="datatype field has too few components: %d < %d";_tag_dyneither(_tmp28E,sizeof(char),47);}),_tag_dyneither(_tmp28D,sizeof(void*),2));});});});else{
# 1456
if(_tmp29B != 0)
_tmp25F=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp296->typs,(int)(_tmp29B - 1))).f2;else{
if(_tmp25E->tl != 0)
({void*_tmp291=0;Cyc_Tcutil_terr(loc,({const char*_tmp292="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp292,sizeof(char),68);}),_tag_dyneither(_tmp291,sizeof(void*),0));});}}
# 1461
bad_type=0;
goto _LL19C;}else{goto _LL1A5;}default: _LL1A5: _LL1A6:
 goto _LL19C;}_LL19C:;}
# 1465
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp299;_tmp299.tag=0;_tmp299.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25F));({void*_tmp297[1]={& _tmp299};Cyc_Tcutil_terr(loc,({const char*_tmp298="%s is not a known type";_tag_dyneither(_tmp298,sizeof(char),23);}),_tag_dyneither(_tmp297,sizeof(void*),1));});});
goto _LL18B;}}_LL18B:;}
# 1470
return Cyc_Absyn_uint_typ;};}
# 1474
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp29C=_new_region("r");struct _RegionHandle*r=& _tmp29C;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp29D=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp29D,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp29E=t;void*_tmp2B6;void*_tmp2B5;union Cyc_Absyn_Constraint*_tmp2B4;union Cyc_Absyn_Constraint*_tmp2B3;switch(*((int*)_tmp29E)){case 1: _LL1A8: _LL1A9: {
# 1481
struct Cyc_List_List*_tmp29F=Cyc_Tcenv_lookup_type_vars(_tmp29D);
void*_tmp2A0=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->v=_tmp29F;_tmp2AB;}));
void*_tmp2A1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->v=_tmp29F;_tmp2AA;}));
union Cyc_Absyn_Constraint*_tmp2A2=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp2A3=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp2A4=({struct Cyc_Absyn_PtrAtts _tmp2A9;_tmp2A9.rgn=_tmp2A1;_tmp2A9.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp2A9.bounds=_tmp2A2;_tmp2A9.zero_term=_tmp2A3;_tmp2A9.ptrloc=0;_tmp2A9;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A5=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp2A7;_tmp2A7.tag=5;_tmp2A7.f1=({struct Cyc_Absyn_PtrInfo _tmp2A8;_tmp2A8.elt_typ=_tmp2A0;_tmp2A8.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp2A8.ptr_atts=_tmp2A4;_tmp2A8;});_tmp2A7;});_tmp2A6;});
Cyc_Tcutil_unify(t,(void*)_tmp2A5);
_tmp2B6=_tmp2A0;_tmp2B5=_tmp2A1;_tmp2B4=_tmp2A2;_tmp2B3=_tmp2A3;goto _LL1AB;}case 5: _LL1AA: _tmp2B6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).elt_typ;_tmp2B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).ptr_atts).rgn;_tmp2B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).ptr_atts).bounds;_tmp2B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E)->f1).ptr_atts).zero_term;_LL1AB:
# 1491
 Cyc_Tcenv_check_rgn_accessible(_tmp29D,loc,_tmp2B5);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2B4);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2B6),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp29D))
({void*_tmp2AC=0;Cyc_Tcutil_terr(loc,({const char*_tmp2AD="can't dereference abstract pointer type";_tag_dyneither(_tmp2AD,sizeof(char),40);}),_tag_dyneither(_tmp2AC,sizeof(void*),0));});{
void*_tmp2AE=_tmp2B6;_npop_handler(0);return _tmp2AE;};default: _LL1AC: _LL1AD: {
# 1497
void*_tmp2B2=({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp2AF[1]={& _tmp2B1};Cyc_Tcexp_expr_err(_tmp29D,loc,topt,({const char*_tmp2B0="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp2B0,sizeof(char),39);}),_tag_dyneither(_tmp2AF,sizeof(void*),1));});});_npop_handler(0);return _tmp2B2;}}_LL1A7:;};}
# 1476
;_pop_region(r);}
# 1503
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1507
struct _RegionHandle _tmp2B7=_new_region("r");struct _RegionHandle*r=& _tmp2B7;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1511
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp2B8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2B9=_tmp2B8;enum Cyc_Absyn_AggrKind _tmp2DC;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Aggrdecl*_tmp2DA;struct Cyc_List_List*_tmp2D9;switch(*((int*)_tmp2B9)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B9)->f1).aggr_info).KnownAggr).tag == 2){_LL1AF: _tmp2DA=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B9)->f1).aggr_info).KnownAggr).val;_tmp2D9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B9)->f1).targs;_LL1B0: {
# 1514
struct Cyc_Absyn_Aggrfield*_tmp2BA=Cyc_Absyn_lookup_decl_field(_tmp2DA,f);
if(_tmp2BA == 0){
void*_tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp2BE;_tmp2BE.tag=0;_tmp2BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2DA->name));({void*_tmp2BB[2]={& _tmp2BD,& _tmp2BE};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2BC="%s has no %s member";_tag_dyneither(_tmp2BC,sizeof(char),20);}),_tag_dyneither(_tmp2BB,sizeof(void*),2));});});});_npop_handler(0);return _tmp2BF;}
# 1519
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DA->impl))->tagged)*is_tagged=1;{
void*t2=_tmp2BA->type;
if(_tmp2D9 != 0){
struct _RegionHandle _tmp2C0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2C0;_push_region(rgn);
{struct Cyc_List_List*_tmp2C1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp2DA->tvs,_tmp2D9);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp2C1,_tmp2BA->type);}
# 1523
;_pop_region(rgn);}
# 1529
if((((_tmp2DA->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DA->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2BA->requires_clause == 0){
void*_tmp2C5=({struct Cyc_String_pa_PrintArg_struct _tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2C2[1]={& _tmp2C4};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2C3="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2C3,sizeof(char),56);}),_tag_dyneither(_tmp2C2,sizeof(void*),1));});});_npop_handler(0);return _tmp2C5;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DA->impl))->exist_vars != 0){
# 1534
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
void*_tmp2C9=({struct Cyc_String_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=0;_tmp2C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2C6[1]={& _tmp2C8};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2C7="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp2C7,sizeof(char),81);}),_tag_dyneither(_tmp2C6,sizeof(void*),1));});});_npop_handler(0);return _tmp2C9;}}{
# 1537
void*_tmp2CA=t2;_npop_handler(0);return _tmp2CA;};};}}else{goto _LL1B3;}case 12: _LL1B1: _tmp2DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B9)->f1;_tmp2DB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B9)->f2;_LL1B2: {
# 1539
struct Cyc_Absyn_Aggrfield*_tmp2CB=Cyc_Absyn_lookup_field(_tmp2DB,f);
if(_tmp2CB == 0){
void*_tmp2CF=({struct Cyc_String_pa_PrintArg_struct _tmp2CE;_tmp2CE.tag=0;_tmp2CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CC[1]={& _tmp2CE};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2CD="type has no %s member";_tag_dyneither(_tmp2CD,sizeof(char),22);}),_tag_dyneither(_tmp2CC,sizeof(void*),1));});});_npop_handler(0);return _tmp2CF;}
# 1544
if(((_tmp2DC == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2CB->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2CB->requires_clause == 0){
# 1546
void*_tmp2D3=({struct Cyc_String_pa_PrintArg_struct _tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D0[1]={& _tmp2D2};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2D1="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2D1,sizeof(char),56);}),_tag_dyneither(_tmp2D0,sizeof(void*),1));});});_npop_handler(0);return _tmp2D3;}{
void*_tmp2D4=_tmp2CB->type;_npop_handler(0);return _tmp2D4;};}default: _LL1B3: _LL1B4: {
# 1549
void*_tmp2D8=({struct Cyc_String_pa_PrintArg_struct _tmp2D7;_tmp2D7.tag=0;_tmp2D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({void*_tmp2D5[1]={& _tmp2D7};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2D6="expecting struct or union, found %s";_tag_dyneither(_tmp2D6,sizeof(char),36);}),_tag_dyneither(_tmp2D5,sizeof(void*),1));});});_npop_handler(0);return _tmp2D8;}}_LL1AE:;};
# 1507
;_pop_region(r);}
# 1555
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1558
struct _RegionHandle _tmp2DD=_new_region("r");struct _RegionHandle*r=& _tmp2DD;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1563
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp2DE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2DF=_tmp2DE;void*_tmp30B;void*_tmp30A;union Cyc_Absyn_Constraint*_tmp309;union Cyc_Absyn_Constraint*_tmp308;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DF)->tag == 5){_LL1B6: _tmp30B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DF)->f1).elt_typ;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DF)->f1).ptr_atts).rgn;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DF)->f1).ptr_atts).bounds;_tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DF)->f1).ptr_atts).zero_term;_LL1B7:
# 1566
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp309);
{void*_tmp2E0=Cyc_Tcutil_compress(_tmp30B);void*_tmp2E1=_tmp2E0;enum Cyc_Absyn_AggrKind _tmp307;struct Cyc_List_List*_tmp306;struct Cyc_Absyn_Aggrdecl*_tmp305;struct Cyc_List_List*_tmp304;switch(*((int*)_tmp2E1)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2E1)->f1).aggr_info).KnownAggr).tag == 2){_LL1BB: _tmp305=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2E1)->f1).aggr_info).KnownAggr).val;_tmp304=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2E1)->f1).targs;_LL1BC: {
# 1569
struct Cyc_Absyn_Aggrfield*_tmp2E2=Cyc_Absyn_lookup_decl_field(_tmp305,f);
if(_tmp2E2 == 0){
void*_tmp2E7=({struct Cyc_String_pa_PrintArg_struct _tmp2E6;_tmp2E6.tag=0;_tmp2E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2E5;_tmp2E5.tag=0;_tmp2E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp305->name));({void*_tmp2E3[2]={& _tmp2E5,& _tmp2E6};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2E4="type %s has no %s member";_tag_dyneither(_tmp2E4,sizeof(char),25);}),_tag_dyneither(_tmp2E3,sizeof(void*),2));});});});_npop_handler(0);return _tmp2E7;}
# 1574
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp305->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2E2->type;
if(_tmp304 != 0){
struct _RegionHandle _tmp2E8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2E8;_push_region(rgn);
{struct Cyc_List_List*_tmp2E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp305->tvs,_tmp304);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2E9,_tmp2E2->type);}
# 1578
;_pop_region(rgn);}{
# 1582
struct Cyc_Absyn_Kind*_tmp2EA=Cyc_Tcutil_typ_kind(t3);
# 1585
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2EA) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2EB=Cyc_Tcutil_compress(t3);void*_tmp2EC=_tmp2EB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2EC)->tag == 8){_LL1C2: _LL1C3:
 goto _LL1C1;}else{_LL1C4: _LL1C5: {
# 1589
void*_tmp2F0=({struct Cyc_String_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2ED[1]={& _tmp2EF};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2EE="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2EE,sizeof(char),48);}),_tag_dyneither(_tmp2ED,sizeof(void*),1));});});_npop_handler(0);return _tmp2F0;}}_LL1C1:;}
# 1594
if((((_tmp305->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp305->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2E2->requires_clause == 0){
# 1597
void*_tmp2F4=({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0;_tmp2F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2F1[1]={& _tmp2F3};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2F2="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2F2,sizeof(char),56);}),_tag_dyneither(_tmp2F1,sizeof(void*),1));});});_npop_handler(0);return _tmp2F4;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp305->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
void*_tmp2F8=({struct Cyc_String_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=0;_tmp2F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2F5[1]={& _tmp2F7};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2F6="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2F6,sizeof(char),72);}),_tag_dyneither(_tmp2F5,sizeof(void*),1));});});_npop_handler(0);return _tmp2F8;}}{
# 1604
void*_tmp2F9=t3;_npop_handler(0);return _tmp2F9;};};};}}else{goto _LL1BF;}case 12: _LL1BD: _tmp307=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2E1)->f1;_tmp306=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2E1)->f2;_LL1BE: {
# 1606
struct Cyc_Absyn_Aggrfield*_tmp2FA=Cyc_Absyn_lookup_field(_tmp306,f);
if(_tmp2FA == 0){
void*_tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2FB[1]={& _tmp2FD};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2FC="type has no %s field";_tag_dyneither(_tmp2FC,sizeof(char),21);}),_tag_dyneither(_tmp2FB,sizeof(void*),1));});});_npop_handler(0);return _tmp2FE;}
# 1611
if((_tmp307 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2FA->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
void*_tmp302=({struct Cyc_String_pa_PrintArg_struct _tmp301;_tmp301.tag=0;_tmp301.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2FF[1]={& _tmp301};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp300="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp300,sizeof(char),56);}),_tag_dyneither(_tmp2FF,sizeof(void*),1));});});_npop_handler(0);return _tmp302;}{
void*_tmp303=_tmp2FA->type;_npop_handler(0);return _tmp303;};}default: _LL1BF: _LL1C0:
 goto _LL1BA;}_LL1BA:;}
# 1616
goto _LL1B5;}else{_LL1B8: _LL1B9:
 goto _LL1B5;}_LL1B5:;}{
# 1619
void*_tmp30F=({struct Cyc_String_pa_PrintArg_struct _tmp30E;_tmp30E.tag=0;_tmp30E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({void*_tmp30C[1]={& _tmp30E};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp30D="expecting struct or union pointer, found %s";_tag_dyneither(_tmp30D,sizeof(char),44);}),_tag_dyneither(_tmp30C,sizeof(void*),1));});});_npop_handler(0);return _tmp30F;};
# 1558
;_pop_region(r);}
# 1624
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1626
struct _tuple15 _tmp310=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp311=_tmp310;unsigned int _tmp31F;int _tmp31E;_LL1C7: _tmp31F=_tmp311.f1;_tmp31E=_tmp311.f2;_LL1C8:;
if(!_tmp31E)
return({void*_tmp312=0;Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp313="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp313,sizeof(char),47);}),_tag_dyneither(_tmp312,sizeof(void*),0));});{
# 1630
struct _handler_cons _tmp314;_push_handler(& _tmp314);{int _tmp316=0;if(setjmp(_tmp314.handler))_tmp316=1;if(!_tmp316){
{void*_tmp317=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp31F)).f2;_npop_handler(0);return _tmp317;};_pop_handler();}else{void*_tmp315=(void*)_exn_thrown;void*_tmp318=_tmp315;void*_tmp31D;if(((struct Cyc_List_Nth_exn_struct*)_tmp318)->tag == Cyc_List_Nth){_LL1CA: _LL1CB:
# 1633
 return({struct Cyc_Int_pa_PrintArg_struct _tmp31C;_tmp31C.tag=1;_tmp31C.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);({struct Cyc_Int_pa_PrintArg_struct _tmp31B;_tmp31B.tag=1;_tmp31B.f1=(unsigned long)((int)_tmp31F);({void*_tmp319[2]={& _tmp31B,& _tmp31C};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp31A="index is %d but tuple has only %d fields";_tag_dyneither(_tmp31A,sizeof(char),41);}),_tag_dyneither(_tmp319,sizeof(void*),2));});});});}else{_LL1CC: _tmp31D=_tmp318;_LL1CD:(int)_rethrow(_tmp31D);}_LL1C9:;}};};}
# 1638
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1640
struct _RegionHandle _tmp320=_new_region("r");struct _RegionHandle*r=& _tmp320;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp321=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp321),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp321),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp321,e2)){
void*_tmp325=({struct Cyc_String_pa_PrintArg_struct _tmp324;_tmp324.tag=0;_tmp324.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({void*_tmp322[1]={& _tmp324};Cyc_Tcexp_expr_err(_tmp321,e2->loc,topt,({const char*_tmp323="expecting int subscript, found %s";_tag_dyneither(_tmp323,sizeof(char),34);}),_tag_dyneither(_tmp322,sizeof(void*),1));});});_npop_handler(0);return _tmp325;}{
# 1651
void*_tmp326=t1;struct Cyc_List_List*_tmp347;void*_tmp346;struct Cyc_Absyn_Tqual _tmp345;void*_tmp344;union Cyc_Absyn_Constraint*_tmp343;union Cyc_Absyn_Constraint*_tmp342;switch(*((int*)_tmp326)){case 5: _LL1CF: _tmp346=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).elt_typ;_tmp345=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).elt_tq;_tmp344=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts).rgn;_tmp343=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts).bounds;_tmp342=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts).zero_term;_LL1D0:
# 1655
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp342)){
int emit_warning=1;
{void*_tmp327=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp343);void*_tmp328=_tmp327;struct Cyc_Absyn_Exp*_tmp331;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp328)->tag == 1){_LL1D6: _tmp331=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp328)->f1;_LL1D7:
# 1659
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp329=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp32A=_tmp329;unsigned int _tmp330;int _tmp32F;_LL1DB: _tmp330=_tmp32A.f1;_tmp32F=_tmp32A.f2;_LL1DC:;
if(_tmp32F){
struct _tuple15 _tmp32B=Cyc_Evexp_eval_const_uint_exp(_tmp331);struct _tuple15 _tmp32C=_tmp32B;unsigned int _tmp32E;int _tmp32D;_LL1DE: _tmp32E=_tmp32C.f1;_tmp32D=_tmp32C.f2;_LL1DF:;
if(_tmp32D  && _tmp32E > _tmp330)emit_warning=0;}}
# 1666
goto _LL1D5;}else{_LL1D8: _LL1D9:
 emit_warning=0;goto _LL1D5;}_LL1D5:;}
# 1669
if(emit_warning)
({void*_tmp332=0;Cyc_Tcutil_warn(e2->loc,({const char*_tmp333="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp333,sizeof(char),63);}),_tag_dyneither(_tmp332,sizeof(void*),0));});}else{
# 1673
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp334=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp335=_tmp334;unsigned int _tmp337;int _tmp336;_LL1E1: _tmp337=_tmp335.f1;_tmp336=_tmp335.f2;_LL1E2:;
if(_tmp336)
Cyc_Tcutil_check_bound(loc,_tmp337,_tmp343);}else{
# 1679
if(Cyc_Tcutil_is_bound_one(_tmp343) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp342))
({void*_tmp338=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmp339="subscript applied to pointer to one object";_tag_dyneither(_tmp339,sizeof(char),43);}),_tag_dyneither(_tmp338,sizeof(void*),0));});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp343);}}
# 1684
Cyc_Tcenv_check_rgn_accessible(_tmp321,loc,_tmp344);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp346),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp321))
({void*_tmp33A=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp33B="can't subscript an abstract pointer";_tag_dyneither(_tmp33B,sizeof(char),36);}),_tag_dyneither(_tmp33A,sizeof(void*),0));});{
void*_tmp33C=_tmp346;_npop_handler(0);return _tmp33C;};case 10: _LL1D1: _tmp347=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp326)->f1;_LL1D2: {
void*_tmp33D=Cyc_Tcexp_ithTupleType(_tmp321,loc,_tmp347,e2);_npop_handler(0);return _tmp33D;}default: _LL1D3: _LL1D4: {
void*_tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp340;_tmp340.tag=0;_tmp340.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp33E[1]={& _tmp340};Cyc_Tcexp_expr_err(_tmp321,loc,topt,({const char*_tmp33F="subscript applied to %s";_tag_dyneither(_tmp33F,sizeof(char),24);}),_tag_dyneither(_tmp33E,sizeof(void*),1));});});_npop_handler(0);return _tmp341;}}_LL1CE:;};};}
# 1641
;_pop_region(r);}
# 1695
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp348=Cyc_Tcutil_compress(*topt);void*_tmp349=_tmp348;struct Cyc_List_List*_tmp34E;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp349)->tag == 10){_LL1E4: _tmp34E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp349)->f1;_LL1E5:
# 1701
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp34E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1704
goto _LL1E3;
# 1706
for(0;es != 0;(es=es->tl,_tmp34E=_tmp34E->tl)){
struct _RegionHandle _tmp34A=_new_region("r");struct _RegionHandle*r=& _tmp34A;_push_region(r);
{int bogus=0;
void*_tmp34B=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp34E))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp34B,(struct Cyc_Absyn_Exp*)es->hd);
# 1712
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp34E->hd)).f2,& bogus);
fields=({struct Cyc_List_List*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C->hd=({struct _tuple17*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->f1=(*((struct _tuple17*)_tmp34E->hd)).f1;_tmp34D->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp34D;});_tmp34C->tl=fields;_tmp34C;});}
# 1708
;_pop_region(r);}
# 1716
done=1;
goto _LL1E3;}else{_LL1E6: _LL1E7:
 goto _LL1E3;}_LL1E3:;}
# 1720
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp34F=_new_region("r");struct _RegionHandle*r=& _tmp34F;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->hd=({struct _tuple17*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->f1=Cyc_Absyn_empty_tqual(0);_tmp351->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp351;});_tmp350->tl=fields;_tmp350;});
# 1723
;_pop_region(r);}
# 1727
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp353;_tmp353.tag=10;_tmp353.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp353;});_tmp352;});}
# 1731
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1734
return({void*_tmp354=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp355="tcCompoundLit";_tag_dyneither(_tmp355,sizeof(char),14);}),_tag_dyneither(_tmp354,sizeof(void*),0));});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1744 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1747
void*res_t2;
struct _RegionHandle _tmp356=_new_region("r");struct _RegionHandle*r=& _tmp356;_push_region(r);{
int _tmp357=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp358=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp360;_tmp360.tag=0;_tmp360.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp357);_tmp360;});_tmp35F;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp358,loc);
# 1756
if(zero_term){
struct Cyc_Absyn_Exp*_tmp359=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp357 - 1);
if(!Cyc_Tcutil_is_zero(_tmp359))
({void*_tmp35A=0;Cyc_Tcutil_terr(_tmp359->loc,({const char*_tmp35B="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp35B,sizeof(char),45);}),_tag_dyneither(_tmp35A,sizeof(void*),0));});}
# 1761
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1766
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1769
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1771
({struct Cyc_String_pa_PrintArg_struct _tmp35E;_tmp35E.tag=0;_tmp35E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res));({void*_tmp35C[1]={& _tmp35E};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp35D="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp35D,sizeof(char),53);}),_tag_dyneither(_tmp35C,sizeof(void*),1));});});}
# 1775
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1780
void*_tmp361=(void*)ds->hd;void*_tmp362=_tmp361;struct Cyc_Absyn_Exp*_tmp36F;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp362)->tag == 1){_LL1E9: _LL1EA:
# 1782
({void*_tmp363=0;Cyc_Tcutil_terr(loc,({const char*_tmp364="only array index designators are supported";_tag_dyneither(_tmp364,sizeof(char),43);}),_tag_dyneither(_tmp363,sizeof(void*),0));});
goto _LL1E8;}else{_LL1EB: _tmp36F=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp362)->f1;_LL1EC:
# 1785
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp36F);{
struct _tuple15 _tmp365=Cyc_Evexp_eval_const_uint_exp(_tmp36F);struct _tuple15 _tmp366=_tmp365;unsigned int _tmp36E;int _tmp36D;_LL1EE: _tmp36E=_tmp366.f1;_tmp36D=_tmp366.f2;_LL1EF:;
if(!_tmp36D)
({void*_tmp367=0;Cyc_Tcutil_terr(_tmp36F->loc,({const char*_tmp368="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp368,sizeof(char),47);}),_tag_dyneither(_tmp367,sizeof(void*),0));});else{
if(_tmp36E != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp36C;_tmp36C.tag=1;_tmp36C.f1=(unsigned long)((int)_tmp36E);({struct Cyc_Int_pa_PrintArg_struct _tmp36B;_tmp36B.tag=1;_tmp36B.f1=(unsigned long)offset;({void*_tmp369[2]={& _tmp36B,& _tmp36C};Cyc_Tcutil_terr(_tmp36F->loc,({const char*_tmp36A="expecting index designator %d but found %d";_tag_dyneither(_tmp36A,sizeof(char),43);}),_tag_dyneither(_tmp369,sizeof(void*),2));});});});}
# 1792
goto _LL1E8;};}_LL1E8:;}}}{
# 1796
void*_tmp370=res_t2;_npop_handler(0);return _tmp370;};
# 1748
;_pop_region(r);}
# 1800
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1803
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp371=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp372=_tmp371;void*_tmp377;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp372)->tag == 19){_LL1F1: _tmp377=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp372)->f1;_LL1F2:
# 1808
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp373=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp377,0),0,Cyc_Absyn_No_coercion,0);
_tmp373->topt=bound->topt;
# 1813
bound=_tmp373;}
# 1815
goto _LL1F0;}else{_LL1F3: _LL1F4:
# 1817
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp376;_tmp376.tag=0;_tmp376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));({void*_tmp374[1]={& _tmp376};Cyc_Tcutil_terr(bound->loc,({const char*_tmp375="expecting unsigned int, found %s";_tag_dyneither(_tmp375,sizeof(char),33);}),_tag_dyneither(_tmp374,sizeof(void*),1));});});}_LL1F0:;}
# 1822
if(!(vd->tq).real_const)
({void*_tmp378=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp379="comprehension index variable is not declared const!";_tag_dyneither(_tmp379,sizeof(char),52);}),_tag_dyneither(_tmp378,sizeof(void*),0));});{
# 1825
struct _RegionHandle _tmp37A=_new_region("r");struct _RegionHandle*r=& _tmp37A;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1831
struct Cyc_Tcenv_Tenv*_tmp37B=te2;
void**_tmp37C=0;
struct Cyc_Absyn_Tqual*_tmp37D=0;
union Cyc_Absyn_Constraint**_tmp37E=0;
# 1836
if(topt != 0){
void*_tmp37F=Cyc_Tcutil_compress(*topt);void*_tmp380=_tmp37F;void*_tmp38B;struct Cyc_Absyn_Tqual _tmp38A;struct Cyc_Absyn_Exp*_tmp389;union Cyc_Absyn_Constraint*_tmp388;struct Cyc_Absyn_PtrInfo _tmp387;switch(*((int*)_tmp380)){case 5: _LL1F6: _tmp387=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp380)->f1;_LL1F7:
# 1839
 _tmp37C=({void**_tmp381=_region_malloc(r,sizeof(*_tmp381));_tmp381[0]=_tmp387.elt_typ;_tmp381;});
_tmp37D=({struct Cyc_Absyn_Tqual*_tmp382=_region_malloc(r,sizeof(*_tmp382));_tmp382[0]=_tmp387.elt_tq;_tmp382;});
_tmp37E=({union Cyc_Absyn_Constraint**_tmp383=_region_malloc(r,sizeof(*_tmp383));_tmp383[0]=(_tmp387.ptr_atts).zero_term;_tmp383;});
goto _LL1F5;case 8: _LL1F8: _tmp38B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).elt_type;_tmp38A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).tq;_tmp389=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).num_elts;_tmp388=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).zero_term;_LL1F9:
# 1844
 _tmp37C=({void**_tmp384=_region_malloc(r,sizeof(*_tmp384));_tmp384[0]=_tmp38B;_tmp384;});
_tmp37D=({struct Cyc_Absyn_Tqual*_tmp385=_region_malloc(r,sizeof(*_tmp385));_tmp385[0]=_tmp38A;_tmp385;});
_tmp37E=({union Cyc_Absyn_Constraint**_tmp386=_region_malloc(r,sizeof(*_tmp386));_tmp386[0]=_tmp388;_tmp386;});
goto _LL1F5;default: _LL1FA: _LL1FB:
# 1849
 goto _LL1F5;}_LL1F5:;}{
# 1852
void*t=Cyc_Tcexp_tcExp(_tmp37B,_tmp37C,body);
# 1854
struct _RegionHandle _tmp38C=_new_region("temp");struct _RegionHandle*temp=& _tmp38C;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body))
({void*_tmp38D=0;Cyc_Tcutil_terr(body->loc,({const char*_tmp38E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp38E,sizeof(char),49);}),_tag_dyneither(_tmp38D,sizeof(void*),0));});
# 1858
if(_tmp37B->le == 0){
# 1860
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp38F=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp390="bound is not constant";_tag_dyneither(_tmp390,sizeof(char),22);}),_tag_dyneither(_tmp38F,sizeof(void*),0));});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp391=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp392="body is not constant";_tag_dyneither(_tmp392,sizeof(char),21);}),_tag_dyneither(_tmp391,sizeof(void*),0));});}
# 1865
if(_tmp37E != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp37E)){
# 1868
struct Cyc_Absyn_Exp*_tmp393=Cyc_Absyn_uint_exp(1,0);_tmp393->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp393,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1872
struct _RegionHandle _tmp394=_new_region("r");struct _RegionHandle*r=& _tmp394;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body))
({void*_tmp395=0;Cyc_Tcutil_terr(body->loc,({const char*_tmp396="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp396,sizeof(char),49);}),_tag_dyneither(_tmp395,sizeof(void*),0));});{
# 1878
void*_tmp397=Cyc_Absyn_array_typ(t,_tmp37D == 0?Cyc_Absyn_empty_tqual(0):*_tmp37D,bound,
_tmp37E == 0?Cyc_Absyn_false_conref:*_tmp37E,0);
void*_tmp398=_tmp397;_npop_handler(2);return _tmp398;};
# 1872
;_pop_region(r);};
# 1854
;_pop_region(temp);};};}
# 1826
;_pop_region(r);};}
# 1885
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1888
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp399=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp39A=_tmp399;void*_tmp39F;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp39A)->tag == 19){_LL1FD: _tmp39F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp39A)->f1;_LL1FE:
# 1893
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp39B=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp39F,0),0,Cyc_Absyn_No_coercion,0);
_tmp39B->topt=bound->topt;
# 1898
bound=_tmp39B;}
# 1900
goto _LL1FC;}else{_LL1FF: _LL200:
# 1902
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp39E;_tmp39E.tag=0;_tmp39E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));({void*_tmp39C[1]={& _tmp39E};Cyc_Tcutil_terr(bound->loc,({const char*_tmp39D="expecting unsigned int, found %s";_tag_dyneither(_tmp39D,sizeof(char),33);}),_tag_dyneither(_tmp39C,sizeof(void*),1));});});}_LL1FC:;}{
# 1907
struct _RegionHandle _tmp3A0=_new_region("r");struct _RegionHandle*r=& _tmp3A0;_push_region(r);
{void**_tmp3A1=0;
struct Cyc_Absyn_Tqual*_tmp3A2=0;
union Cyc_Absyn_Constraint**_tmp3A3=0;
# 1912
if(topt != 0){
void*_tmp3A4=Cyc_Tcutil_compress(*topt);void*_tmp3A5=_tmp3A4;void*_tmp3B0;struct Cyc_Absyn_Tqual _tmp3AF;struct Cyc_Absyn_Exp*_tmp3AE;union Cyc_Absyn_Constraint*_tmp3AD;struct Cyc_Absyn_PtrInfo _tmp3AC;switch(*((int*)_tmp3A5)){case 5: _LL202: _tmp3AC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A5)->f1;_LL203:
# 1915
 _tmp3A1=({void**_tmp3A6=_region_malloc(r,sizeof(*_tmp3A6));_tmp3A6[0]=_tmp3AC.elt_typ;_tmp3A6;});
_tmp3A2=({struct Cyc_Absyn_Tqual*_tmp3A7=_region_malloc(r,sizeof(*_tmp3A7));_tmp3A7[0]=_tmp3AC.elt_tq;_tmp3A7;});
_tmp3A3=({union Cyc_Absyn_Constraint**_tmp3A8=_region_malloc(r,sizeof(*_tmp3A8));_tmp3A8[0]=(_tmp3AC.ptr_atts).zero_term;_tmp3A8;});
goto _LL201;case 8: _LL204: _tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A5)->f1).elt_type;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A5)->f1).tq;_tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A5)->f1).num_elts;_tmp3AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A5)->f1).zero_term;_LL205:
# 1920
 _tmp3A1=({void**_tmp3A9=_region_malloc(r,sizeof(*_tmp3A9));_tmp3A9[0]=_tmp3B0;_tmp3A9;});
_tmp3A2=({struct Cyc_Absyn_Tqual*_tmp3AA=_region_malloc(r,sizeof(*_tmp3AA));_tmp3AA[0]=_tmp3AF;_tmp3AA;});
_tmp3A3=({union Cyc_Absyn_Constraint**_tmp3AB=_region_malloc(r,sizeof(*_tmp3AB));_tmp3AB[0]=_tmp3AD;_tmp3AB;});
goto _LL201;default: _LL206: _LL207:
# 1925
 goto _LL201;}_LL201:;}
# 1928
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,1,t);
if(_tmp3A1 != 0)Cyc_Tcutil_unify(*_tmp3A1,t);
# 1931
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp3B1=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp3B2="bound is not constant";_tag_dyneither(_tmp3B2,sizeof(char),22);}),_tag_dyneither(_tmp3B1,sizeof(void*),0));});}
# 1935
if(_tmp3A3 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3A3)){
# 1938
struct Cyc_Absyn_Exp*_tmp3B3=Cyc_Absyn_uint_exp(1,0);_tmp3B3->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp3B3,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1942
({void*_tmp3B4=0;Cyc_Tcutil_terr(loc,({const char*_tmp3B5="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp3B5,sizeof(char),74);}),_tag_dyneither(_tmp3B4,sizeof(void*),0));});}{
# 1945
void*_tmp3B6=Cyc_Absyn_array_typ(t,_tmp3A2 == 0?Cyc_Absyn_empty_tqual(0):*_tmp3A2,bound,
_tmp3A3 == 0?Cyc_Absyn_false_conref:*_tmp3A3,0);
void*_tmp3B7=_tmp3B6;_npop_handler(0);return _tmp3B7;};}
# 1908
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1961 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1965
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=ad;_tmp3B8;});}else{
# 1971
{struct _handler_cons _tmp3B9;_push_handler(& _tmp3B9);{int _tmp3BB=0;if(setjmp(_tmp3B9.handler))_tmp3BB=1;if(!_tmp3BB){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1971
;_pop_handler();}else{void*_tmp3BA=(void*)_exn_thrown;void*_tmp3BC=_tmp3BA;void*_tmp3C0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3BC)->tag == Cyc_Dict_Absent){_LL209: _LL20A:
# 1974
({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0;_tmp3BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));({void*_tmp3BD[1]={& _tmp3BF};Cyc_Tcutil_terr(loc,({const char*_tmp3BE="unbound struct/union name %s";_tag_dyneither(_tmp3BE,sizeof(char),29);}),_tag_dyneither(_tmp3BD,sizeof(void*),1));});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL20B: _tmp3C0=_tmp3BC;_LL20C:(int)_rethrow(_tmp3C0);}_LL208:;}};}
# 1977
*ad_opt=ad;
*tn=ad->name;}
# 1980
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=0;_tmp3C3.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp3C4="struct";_tag_dyneither(_tmp3C4,sizeof(char),7);}):({const char*_tmp3C5="union";_tag_dyneither(_tmp3C5,sizeof(char),6);}));({void*_tmp3C1[1]={& _tmp3C3};Cyc_Tcutil_terr(loc,({const char*_tmp3C2="can't construct abstract %s";_tag_dyneither(_tmp3C2,sizeof(char),28);}),_tag_dyneither(_tmp3C1,sizeof(void*),1));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1984
struct _RegionHandle _tmp3C6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3C6;_push_region(rgn);
# 1989
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp3C7=Cyc_Tcenv_new_status(te);
if(_tmp3C7 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 1998
te2=Cyc_Tcenv_set_new_status(rgn,_tmp3C7,te);}{
# 2000
struct _tuple12 _tmp3C8=({struct _tuple12 _tmp3F0;_tmp3F0.f1=Cyc_Tcenv_lookup_type_vars(te2);_tmp3F0.f2=rgn;_tmp3F0;});
struct Cyc_List_List*_tmp3C9=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp3C8,ad->tvs);
struct Cyc_List_List*_tmp3CA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp3C8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp3CB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3C9);
struct Cyc_List_List*_tmp3CC=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3CA);
struct Cyc_List_List*_tmp3CD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp3C9,_tmp3CA);
void*res_typ;
# 2011
if(topt != 0){
void*_tmp3CE=Cyc_Tcutil_compress(*topt);void*_tmp3CF=_tmp3CE;struct Cyc_Absyn_Aggrdecl**_tmp3D5;struct Cyc_List_List*_tmp3D4;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CF)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CF)->f1).aggr_info).KnownAggr).tag == 2){_LL20E: _tmp3D5=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CF)->f1).aggr_info).KnownAggr).val;_tmp3D4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CF)->f1).targs;_LL20F:
# 2014
 if(*_tmp3D5 == *adptr){
{struct Cyc_List_List*_tmp3D0=_tmp3CB;for(0;_tmp3D0 != 0  && _tmp3D4 != 0;
(_tmp3D0=_tmp3D0->tl,_tmp3D4=_tmp3D4->tl)){
Cyc_Tcutil_unify((void*)_tmp3D0->hd,(void*)_tmp3D4->hd);}}
# 2019
res_typ=*topt;
goto _LL20D;}
# 2022
goto _LL211;}else{goto _LL210;}}else{_LL210: _LL211:
# 2024
 res_typ=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3D2;_tmp3D2.tag=11;_tmp3D2.f1=({struct Cyc_Absyn_AggrInfo _tmp3D3;_tmp3D3.aggr_info=Cyc_Absyn_KnownAggr(adptr);_tmp3D3.targs=_tmp3CB;_tmp3D3;});_tmp3D2;});_tmp3D1;});}_LL20D:;}else{
# 2027
res_typ=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3D7;_tmp3D7.tag=11;_tmp3D7.f1=({struct Cyc_Absyn_AggrInfo _tmp3D8;_tmp3D8.aggr_info=Cyc_Absyn_KnownAggr(adptr);_tmp3D8.targs=_tmp3CB;_tmp3D8;});_tmp3D7;});_tmp3D6;});}{
# 2030
struct Cyc_List_List*_tmp3D9=*ts;
struct Cyc_List_List*_tmp3DA=_tmp3CC;
while(_tmp3D9 != 0  && _tmp3DA != 0){
# 2034
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,0,(void*)_tmp3D9->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp3D9->hd);
Cyc_Tcutil_unify((void*)_tmp3D9->hd,(void*)_tmp3DA->hd);
_tmp3D9=_tmp3D9->tl;
_tmp3DA=_tmp3DA->tl;}
# 2040
if(_tmp3D9 != 0)
({void*_tmp3DB=0;Cyc_Tcutil_terr(loc,({const char*_tmp3DC="too many explicit witness types";_tag_dyneither(_tmp3DC,sizeof(char),32);}),_tag_dyneither(_tmp3DB,sizeof(void*),0));});
# 2043
*ts=_tmp3CC;{
# 2046
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3DD=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3DE=_tmp3DD;struct Cyc_Absyn_Aggrfield*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3E9;_LL213: _tmp3EA=_tmp3DE->f1;_tmp3E9=_tmp3DE->f2;_LL214:;{
void*_tmp3DF=Cyc_Tcutil_rsubstitute(rgn,_tmp3CD,_tmp3EA->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp3DF,_tmp3E9);
# 2057
if(!Cyc_Tcutil_coerce_arg(te2,_tmp3E9,_tmp3DF,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0;_tmp3E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2061
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3E9->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp3E5;_tmp3E5.tag=0;_tmp3E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2060
Cyc_Absynpp_typ2string(_tmp3DF));({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0;_tmp3E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0;_tmp3E3.f1=(struct _dyneither_ptr)(
# 2059
ad->kind == Cyc_Absyn_StructA?({const char*_tmp3E7="struct";_tag_dyneither(_tmp3E7,sizeof(char),7);}):({const char*_tmp3E8="union";_tag_dyneither(_tmp3E8,sizeof(char),6);}));({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0;_tmp3E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3EA->name);({void*_tmp3E0[5]={& _tmp3E2,& _tmp3E3,& _tmp3E4,& _tmp3E5,& _tmp3E6};Cyc_Tcutil_terr(_tmp3E9->loc,({const char*_tmp3E1="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp3E1,sizeof(char),40);}),_tag_dyneither(_tmp3E0,sizeof(void*),5));});});});});});});
# 2062
Cyc_Tcutil_explain_failure();}};}{
# 2066
struct Cyc_List_List*_tmp3EB=0;
{struct Cyc_List_List*_tmp3EC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp3EC != 0;_tmp3EC=_tmp3EC->tl){
_tmp3EB=({struct Cyc_List_List*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->hd=({struct _tuple0*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp3CD,(*((struct _tuple0*)_tmp3EC->hd)).f1);_tmp3EE->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp3CD,(*((struct _tuple0*)_tmp3EC->hd)).f2);_tmp3EE;});_tmp3ED->tl=_tmp3EB;_tmp3ED;});}}
# 2071
_tmp3EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3EB);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp3EB);{
void*_tmp3EF=res_typ;_npop_handler(0);return _tmp3EF;};};};};};}
# 1989
;_pop_region(rgn);};}
# 2078
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2080
struct _RegionHandle _tmp3F1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3F1;_push_region(rgn);{
void*_tmp3F2=Cyc_Tcutil_compress(ts);void*_tmp3F3=_tmp3F2;enum Cyc_Absyn_AggrKind _tmp402;struct Cyc_List_List*_tmp401;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F3)->tag == 12){_LL216: _tmp402=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F3)->f1;_tmp401=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F3)->f2;_LL217:
# 2083
 if(_tmp402 == Cyc_Absyn_UnionA)
({void*_tmp3F4=0;Cyc_Tcutil_terr(loc,({const char*_tmp3F5="expecting struct but found union";_tag_dyneither(_tmp3F5,sizeof(char),33);}),_tag_dyneither(_tmp3F4,sizeof(void*),0));});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp401);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3F6=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3F7=_tmp3F6;struct Cyc_Absyn_Aggrfield*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;_LL21B: _tmp3FE=_tmp3F7->f1;_tmp3FD=_tmp3F7->f2;_LL21C:;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp3FE->type,_tmp3FD);
# 2092
if(!Cyc_Tcutil_coerce_arg(te,_tmp3FD,_tmp3FE->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3FC;_tmp3FC.tag=0;_tmp3FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2095
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3FD->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp3FB;_tmp3FB.tag=0;_tmp3FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2094
Cyc_Absynpp_typ2string(_tmp3FE->type));({struct Cyc_String_pa_PrintArg_struct _tmp3FA;_tmp3FA.tag=0;_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3FE->name);({void*_tmp3F8[3]={& _tmp3FA,& _tmp3FB,& _tmp3FC};Cyc_Tcutil_terr(_tmp3FD->loc,({const char*_tmp3F9="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3F9,sizeof(char),41);}),_tag_dyneither(_tmp3F8,sizeof(void*),3));});});});});
# 2096
Cyc_Tcutil_explain_failure();}}
# 2099
goto _LL215;};}else{_LL218: _LL219:
({void*_tmp3FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp400="tcAnonStruct: wrong type";_tag_dyneither(_tmp400,sizeof(char),25);}),_tag_dyneither(_tmp3FF,sizeof(void*),0));});}_LL215:;}{
# 2103
void*_tmp403=ts;_npop_handler(0);return _tmp403;};
# 2080
;_pop_region(rgn);}
# 2107
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2110
struct _RegionHandle _tmp404=_new_region("rgn");struct _RegionHandle*rgn=& _tmp404;_push_region(rgn);
{struct _tuple12 _tmp405=({struct _tuple12 _tmp41C;_tmp41C.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp41C.f2=rgn;_tmp41C;});
struct Cyc_List_List*_tmp406=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp405,tud->tvs);
struct Cyc_List_List*_tmp407=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp406);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp41A;_tmp41A.tag=4;_tmp41A.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp41B;_tmp41B.field_info=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp41B.targs=_tmp407;_tmp41B;});_tmp41A;});_tmp419;});
# 2118
if(topt != 0){
void*_tmp408=Cyc_Tcutil_compress(*topt);void*_tmp409=_tmp408;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp409)->tag == 4){_LL21E: _LL21F:
 Cyc_Tcutil_unify(*topt,res);goto _LL21D;}else{_LL220: _LL221:
 goto _LL21D;}_LL21D:;}{
# 2124
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp406 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp406,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp40E;_tmp40E.tag=0;_tmp40E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2135
e->topt == 0?(struct _dyneither_ptr)({const char*_tmp40F="?";_tag_dyneither(_tmp40F,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));({struct Cyc_String_pa_PrintArg_struct _tmp40D;_tmp40D.tag=0;_tmp40D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2134
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp40C;_tmp40C.tag=0;_tmp40C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp40A[3]={& _tmp40C,& _tmp40D,& _tmp40E};Cyc_Tcutil_terr(e->loc,({const char*_tmp40B="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp40B,sizeof(char),82);}),_tag_dyneither(_tmp40A,sizeof(void*),3));});});});});
# 2136
Cyc_Tcutil_explain_failure();}}
# 2139
if(es != 0){
void*_tmp413=({struct Cyc_String_pa_PrintArg_struct _tmp412;_tmp412.tag=0;_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2142
Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp410[1]={& _tmp412};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,({const char*_tmp411="too many arguments for datatype constructor %s";_tag_dyneither(_tmp411,sizeof(char),47);}),_tag_dyneither(_tmp410,sizeof(void*),1));});});_npop_handler(0);return _tmp413;}
if(ts != 0){
void*_tmp417=({struct Cyc_String_pa_PrintArg_struct _tmp416;_tmp416.tag=0;_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp414[1]={& _tmp416};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp415="too few arguments for datatype constructor %s";_tag_dyneither(_tmp415,sizeof(char),46);}),_tag_dyneither(_tmp414,sizeof(void*),1));});});_npop_handler(0);return _tmp417;}{
void*_tmp418=res;_npop_handler(0);return _tmp418;};};}
# 2111
;_pop_region(rgn);}
# 2150
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2152
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2154
if(topt != 0){
void*_tmp41D=Cyc_Tcutil_compress(*topt);void*_tmp41E=_tmp41D;void*_tmp41F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->tag == 5){_LL223: _tmp41F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).elt_typ;_LL224:
# 2157
 Cyc_Tcutil_unify(_tmp41F,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL222;}else{_LL225: _LL226:
 goto _LL222;}_LL222:;}
# 2163
({struct Cyc_String_pa_PrintArg_struct _tmp423;_tmp423.tag=0;_tmp423.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0;_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp424="calloc";_tag_dyneither(_tmp424,sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp425="malloc";_tag_dyneither(_tmp425,sizeof(char),7);})));({void*_tmp420[2]={& _tmp422,& _tmp423};Cyc_Tcutil_terr(loc,({const char*_tmp421="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp421,sizeof(char),60);}),_tag_dyneither(_tmp420,sizeof(void*),2));});});});}
# 2167
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2169
enum Cyc_Absyn_AliasQual _tmp426=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp427=_tmp426;if(_tmp427 == Cyc_Absyn_Unique){_LL228: _LL229:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL22A: _LL22B:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL227:;}
# 2175
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2180
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2183
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp42F;_tmp42F.tag=15;_tmp42F.f1=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp42F;});_tmp42E;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp428=Cyc_Tcutil_compress(handle_type);void*_tmp429=_tmp428;void*_tmp42D;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp429)->tag == 15){_LL22D: _tmp42D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp429)->f1;_LL22E:
# 2188
 rgn=_tmp42D;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22C;}else{_LL22F: _LL230:
# 2192
({struct Cyc_String_pa_PrintArg_struct _tmp42C;_tmp42C.tag=0;_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));({void*_tmp42A[1]={& _tmp42C};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,({const char*_tmp42B="expecting region_t type but found %s";_tag_dyneither(_tmp42B,sizeof(char),37);}),_tag_dyneither(_tmp42A,sizeof(void*),1));});});
goto _LL22C;}_LL22C:;}else{
# 2199
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2206
{struct _RegionHandle _tmp430=_new_region("r");struct _RegionHandle*r=& _tmp430;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2217 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp431=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp432="calloc with empty type";_tag_dyneither(_tmp432,sizeof(char),23);}),_tag_dyneither(_tmp431,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2229
void*er=(*e)->r;
retry_sizeof: {
void*_tmp433=er;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_Absyn_Exp*_tmp448;void*_tmp447;switch(*((int*)_tmp433)){case 16: _LL232: _tmp447=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp433)->f1;_LL233:
# 2233
 elt_type=_tmp447;
*t=({void**_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434[0]=elt_type;_tmp434;});
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL231;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp433)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp433)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp433)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp433)->f2)->tl)->tl == 0){_LL234: _tmp449=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp433)->f2)->hd;_tmp448=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp433)->f2)->tl)->hd;_LL235:
# 2240
{struct _tuple0 _tmp435=({struct _tuple0 _tmp43B;_tmp43B.f1=_tmp449->r;_tmp43B.f2=_tmp448->r;_tmp43B;});struct _tuple0 _tmp436=_tmp435;void*_tmp43A;void*_tmp439;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp436.f1)->tag == 16){_LL239: _tmp439=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp436.f1)->f1;_LL23A:
# 2242
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp439);
elt_type=_tmp439;
*t=({void**_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437[0]=elt_type;_tmp437;});
num_elts=_tmp448;
one_elt=0;
goto _LL238;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp436.f2)->tag == 16){_LL23B: _tmp43A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp436.f2)->f1;_LL23C:
# 2249
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp43A);
elt_type=_tmp43A;
*t=({void**_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=elt_type;_tmp438;});
num_elts=_tmp449;
one_elt=0;
goto _LL238;}else{_LL23D: _LL23E:
 goto No_sizeof;}}_LL238:;}
# 2257
goto _LL231;}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}default: _LL236: _LL237:
# 2259
 No_sizeof: {
# 2262
struct Cyc_Absyn_Exp*_tmp43C=*e;
{void*_tmp43D=_tmp43C->r;void*_tmp43E=_tmp43D;struct Cyc_Absyn_Exp*_tmp43F;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp43E)->tag == 13){_LL240: _tmp43F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp43E)->f2;_LL241:
 _tmp43C=_tmp43F;goto _LL23F;}else{_LL242: _LL243:
 goto _LL23F;}_LL23F:;}
# 2267
{void*_tmp440=Cyc_Tcutil_compress((void*)_check_null(_tmp43C->topt));void*_tmp441=_tmp440;void*_tmp445;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp441)->tag == 19){_LL245: _tmp445=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp441)->f1;_LL246:
# 2269
{void*_tmp442=Cyc_Tcutil_compress(_tmp445);void*_tmp443=_tmp442;struct Cyc_Absyn_Exp*_tmp444;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp443)->tag == 18){_LL24A: _tmp444=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp443)->f1;_LL24B:
# 2271
 er=_tmp444->r;goto retry_sizeof;}else{_LL24C: _LL24D:
 goto _LL249;}_LL249:;}
# 2274
goto _LL244;}else{_LL247: _LL248:
 goto _LL244;}_LL244:;}
# 2277
elt_type=Cyc_Absyn_char_typ;
*t=({void**_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446[0]=elt_type;_tmp446;});
num_elts=*e;
one_elt=0;}
# 2282
goto _LL231;}_LL231:;}}
# 2286
*is_fat=!one_elt;
# 2289
{void*_tmp44A=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp44D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44A)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44A)->f1).aggr_info).KnownAggr).tag == 2){_LL24F: _tmp44D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44A)->f1).aggr_info).KnownAggr).val;_LL250:
# 2291
 if(_tmp44D->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44D->impl))->exist_vars != 0)
({void*_tmp44B=0;Cyc_Tcutil_terr(loc,({const char*_tmp44C="malloc with existential types not yet implemented";_tag_dyneither(_tmp44C,sizeof(char),50);}),_tag_dyneither(_tmp44B,sizeof(void*),0));});
goto _LL24E;}else{goto _LL251;}}else{_LL251: _LL252:
 goto _LL24E;}_LL24E:;}{
# 2298
void*(*_tmp44E)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp44F=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp450=Cyc_Tcutil_compress(*topt);void*_tmp451=_tmp450;union Cyc_Absyn_Constraint*_tmp45E;union Cyc_Absyn_Constraint*_tmp45D;union Cyc_Absyn_Constraint*_tmp45C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp451)->tag == 5){_LL254: _tmp45E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp451)->f1).ptr_atts).nullable;_tmp45D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp451)->f1).ptr_atts).bounds;_tmp45C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp451)->f1).ptr_atts).zero_term;_LL255:
# 2303
 _tmp44F=_tmp45C;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp45E))
_tmp44E=Cyc_Absyn_star_typ;
# 2308
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp45C) && !(*is_calloc)){
({void*_tmp452=0;Cyc_Tcutil_warn(loc,({const char*_tmp453="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp453,sizeof(char),55);}),_tag_dyneither(_tmp452,sizeof(void*),0));});
*is_calloc=1;}
# 2314
{void*_tmp454=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp45D);void*_tmp455=_tmp454;struct Cyc_Absyn_Exp*_tmp45B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp455)->tag == 0){_LL259: _LL25A:
 goto _LL258;}else{_LL25B: _tmp45B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp455)->f1;if(!one_elt){_LL25C: {
# 2317
int _tmp456=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp456  && Cyc_Evexp_same_const_exp(_tmp45B,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp45D),_tmp44F);}
# 2323
{void*_tmp457=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp458=_tmp457;void*_tmp45A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp458)->tag == 19){_LL260: _tmp45A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp458)->f1;_LL261: {
# 2325
struct Cyc_Absyn_Exp*_tmp459=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp45A,0),0,Cyc_Absyn_No_coercion,0);
# 2327
if(Cyc_Evexp_same_const_exp(_tmp459,_tmp45B)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp45D),_tmp44F);}
# 2332
goto _LL25F;}}else{_LL262: _LL263:
 goto _LL25F;}_LL25F:;}
# 2335
goto _LL258;}}else{_LL25D: _LL25E:
 goto _LL258;}}_LL258:;}
# 2338
goto _LL253;}else{_LL256: _LL257:
 goto _LL253;}_LL253:;}
# 2341
if(!one_elt)_tmp44E=Cyc_Absyn_dyneither_typ;
return _tmp44E(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp44F);};};}
# 2346
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2348
struct _RegionHandle _tmp45F=_new_region("r");struct _RegionHandle*r=& _tmp45F;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp460=Cyc_Tcenv_enter_lhs(r,te);
Cyc_Tcexp_tcExpNoPromote(_tmp460,0,e1);{
void*_tmp461=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp460,& _tmp461,e2);};}{
# 2354
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2357
{void*_tmp462=Cyc_Tcutil_compress(t1);void*_tmp463=_tmp462;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp463)->tag == 8){_LL265: _LL266:
({void*_tmp464=0;Cyc_Tcutil_terr(loc,({const char*_tmp465="cannot assign to an array";_tag_dyneither(_tmp465,sizeof(char),26);}),_tag_dyneither(_tmp464,sizeof(void*),0));});goto _LL264;}else{_LL267: _LL268:
 goto _LL264;}_LL264:;}
# 2362
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp466=0;Cyc_Tcutil_terr(loc,({const char*_tmp467="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp467,sizeof(char),58);}),_tag_dyneither(_tmp466,sizeof(void*),0));});
# 2366
if(!Cyc_Absyn_is_lvalue(e1)){
void*_tmp46A=({void*_tmp468=0;Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp469="swap non-lvalue";_tag_dyneither(_tmp469,sizeof(char),16);}),_tag_dyneither(_tmp468,sizeof(void*),0));});_npop_handler(0);return _tmp46A;}
if(!Cyc_Absyn_is_lvalue(e2)){
void*_tmp46D=({void*_tmp46B=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp46C="swap non-lvalue";_tag_dyneither(_tmp46C,sizeof(char),16);}),_tag_dyneither(_tmp46B,sizeof(void*),0));});_npop_handler(0);return _tmp46D;}{
# 2371
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp470=({void*_tmp46E=0;Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp46F="swap value in zeroterm array";_tag_dyneither(_tmp46F,sizeof(char),29);}),_tag_dyneither(_tmp46E,sizeof(void*),0));});_npop_handler(0);return _tmp470;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp473=({void*_tmp471=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp472="swap value in zeroterm array";_tag_dyneither(_tmp472,sizeof(char),29);}),_tag_dyneither(_tmp471,sizeof(void*),0));});_npop_handler(0);return _tmp473;}
# 2378
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp474=({struct Cyc_String_pa_PrintArg_struct _tmp479;_tmp479.tag=0;_tmp479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp478;_tmp478.tag=0;_tmp478.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp476[2]={& _tmp478,& _tmp479};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp477="type mismatch: %s != %s";_tag_dyneither(_tmp477,sizeof(char),24);}),_tag_dyneither(_tmp476,sizeof(void*),2));});});});
void*_tmp475=_tmp474;_npop_handler(0);return _tmp475;}{
# 2385
void*_tmp47A=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp47A;};};};
# 2348
;_pop_region(r);}
# 2389
int Cyc_Tcexp_in_stmt_exp=0;
# 2391
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
{struct _RegionHandle _tmp47B=_new_region("r");struct _RegionHandle*r=& _tmp47B;_push_region(r);
{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,te),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2393
;_pop_region(r);}
# 2398
while(1){
void*_tmp47C=s->r;void*_tmp47D=_tmp47C;struct Cyc_Absyn_Decl*_tmp488;struct Cyc_Absyn_Stmt*_tmp487;struct Cyc_Absyn_Stmt*_tmp486;struct Cyc_Absyn_Stmt*_tmp485;struct Cyc_Absyn_Exp*_tmp484;switch(*((int*)_tmp47D)){case 1: _LL26A: _tmp484=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp47D)->f1;_LL26B: {
# 2402
void*_tmp47E=(void*)_check_null(_tmp484->topt);
if(!Cyc_Tcutil_unify(_tmp47E,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
({struct Cyc_String_pa_PrintArg_struct _tmp481;_tmp481.tag=0;_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp47E));({void*_tmp47F[1]={& _tmp481};Cyc_Tcutil_terr(loc,({const char*_tmp480="statement expression returns type %s";_tag_dyneither(_tmp480,sizeof(char),37);}),_tag_dyneither(_tmp47F,sizeof(void*),1));});});
Cyc_Tcutil_explain_failure();}
# 2408
return _tmp47E;}case 2: _LL26C: _tmp486=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47D)->f1;_tmp485=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47D)->f2;_LL26D:
 s=_tmp485;continue;case 12: _LL26E: _tmp488=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47D)->f1;_tmp487=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47D)->f2;_LL26F:
 s=_tmp487;continue;default: _LL270: _LL271:
# 2412
 return({void*_tmp482=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp483="statement expression must end with expression";_tag_dyneither(_tmp483,sizeof(char),46);}),_tag_dyneither(_tmp482,sizeof(void*),0));});}_LL269:;}}
# 2417
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
{struct _RegionHandle _tmp489=_new_region("r");struct _RegionHandle*r=& _tmp489;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));;_pop_region(r);}
# 2422
{void*_tmp48A=t;struct Cyc_Absyn_Aggrdecl*_tmp48E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48A)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48A)->f1).aggr_info).KnownAggr).tag == 2){_LL273: _tmp48E=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48A)->f1).aggr_info).KnownAggr).val;_LL274:
# 2424
 if((_tmp48E->kind == Cyc_Absyn_UnionA  && _tmp48E->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48E->impl))->tagged)goto _LL272;
goto _LL276;}else{goto _LL275;}}else{_LL275: _LL276:
# 2427
({struct Cyc_String_pa_PrintArg_struct _tmp48D;_tmp48D.tag=0;_tmp48D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp48B[1]={& _tmp48D};Cyc_Tcutil_terr(loc,({const char*_tmp48C="expecting @tagged union but found %s";_tag_dyneither(_tmp48C,sizeof(char),37);}),_tag_dyneither(_tmp48B,sizeof(void*),1));});});
goto _LL272;}_LL272:;}
# 2430
return Cyc_Absyn_uint_typ;}
# 2434
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2438
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp48F=_new_region("r");struct _RegionHandle*r=& _tmp48F;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp490=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2443
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp498;_tmp498.tag=15;_tmp498.f1=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp490));_tmp498;});_tmp497;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp490,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp491=Cyc_Tcutil_compress(handle_type);void*_tmp492=_tmp491;void*_tmp496;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp492)->tag == 15){_LL278: _tmp496=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp492)->f1;_LL279:
# 2448
 rgn=_tmp496;
Cyc_Tcenv_check_rgn_accessible(_tmp490,loc,rgn);
goto _LL277;}else{_LL27A: _LL27B:
# 2452
({struct Cyc_String_pa_PrintArg_struct _tmp495;_tmp495.tag=0;_tmp495.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));({void*_tmp493[1]={& _tmp495};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,({const char*_tmp494="expecting region_t type but found %s";_tag_dyneither(_tmp494,sizeof(char),37);}),_tag_dyneither(_tmp493,sizeof(void*),1));});});
goto _LL277;}_LL277:;}else{
# 2459
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2467
void*_tmp499=e1->r;void*_tmp49A=_tmp499;struct Cyc_List_List*_tmp4BF;struct Cyc_Core_Opt*_tmp4BE;struct Cyc_List_List*_tmp4BD;switch(*((int*)_tmp49A)){case 26: _LL27D: _LL27E:
 goto _LL280;case 27: _LL27F: _LL280: {
# 2472
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp490,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp49B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp49C="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp49C,sizeof(char),45);}),_tag_dyneither(_tmp49B,sizeof(void*),0));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp490,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp490,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2483
void*_tmp49D=res_typ;_npop_handler(0);return _tmp49D;};}case 35: _LL281: _tmp4BE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp49A)->f1;_tmp4BD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp49A)->f2;_LL282:
# 2485
 e1->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp49F;_tmp49F.tag=25;_tmp49F.f1=_tmp4BD;_tmp49F;});_tmp49E;});
_tmp4BF=_tmp4BD;goto _LL284;case 25: _LL283: _tmp4BF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp49A)->f1;_LL284: {
# 2488
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4A0=Cyc_Tcutil_compress(*topt);void*_tmp4A1=_tmp4A0;void**_tmp4A4;struct Cyc_Absyn_Tqual _tmp4A3;union Cyc_Absyn_Constraint*_tmp4A2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->tag == 5){_LL28C: _tmp4A4=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->f1).elt_typ;_tmp4A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->f1).elt_tq;_tmp4A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->f1).ptr_atts).zero_term;_LL28D:
# 2494
 elt_typ_opt=_tmp4A4;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A2);
goto _LL28B;}else{_LL28E: _LL28F:
 goto _LL28B;}_LL28B:;}{
# 2500
void*res_typ=Cyc_Tcexp_tcArray(_tmp490,e1->loc,elt_typ_opt,0,zero_term,_tmp4BF);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp4A5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A6="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp4A6,sizeof(char),50);}),_tag_dyneither(_tmp4A5,sizeof(void*),0));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2509
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp490,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp490,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2515
void*_tmp4A7=res_typ;_npop_handler(0);return _tmp4A7;};};}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49A)->f1).Wstring_c).tag){case 8: _LL285: _LL286: {
# 2520
void*_tmp4A8=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2522
void*_tmp4A9=Cyc_Tcexp_tcExp(_tmp490,& _tmp4A8,e1);
void*_tmp4AC=Cyc_Absyn_atb_typ(_tmp4A9,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp4AB;_tmp4AB.tag=1;_tmp4AB.f1=Cyc_Absyn_uint_exp(1,0);_tmp4AB;});_tmp4AA;}),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp4AC;}case 9: _LL287: _LL288: {
# 2527
void*_tmp4AD=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2529
void*_tmp4AE=Cyc_Tcexp_tcExp(_tmp490,& _tmp4AD,e1);
void*_tmp4B1=Cyc_Absyn_atb_typ(_tmp4AE,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp4B0;_tmp4B0.tag=1;_tmp4B0.f1=Cyc_Absyn_uint_exp(1,0);_tmp4B0;});_tmp4AF;}),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp4B1;}default: goto _LL289;}default: _LL289: _LL28A:
# 2536
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp4B2=Cyc_Tcutil_compress(*topt);void*_tmp4B3=_tmp4B2;void**_tmp4B5;struct Cyc_Absyn_Tqual _tmp4B4;switch(*((int*)_tmp4B3)){case 5: _LL291: _tmp4B5=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B3)->f1).elt_typ;_tmp4B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B3)->f1).elt_tq;_LL292:
# 2542
 topt2=_tmp4B5;goto _LL290;case 3: _LL293: _LL294:
# 2546
 bogus=*topt;
topt2=& bogus;
goto _LL290;default: _LL295: _LL296:
 goto _LL290;}_LL290:;}{
# 2552
void*telt=Cyc_Tcexp_tcExp(_tmp490,topt2,e1);
# 2554
struct _RegionHandle _tmp4B6=_new_region("temp");struct _RegionHandle*temp=& _tmp4B6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1))
({void*_tmp4B7=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp4B8="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp4B8,sizeof(char),49);}),_tag_dyneither(_tmp4B7,sizeof(void*),0));});{
# 2559
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4BB;_tmp4BB.tag=5;_tmp4BB.f1=({struct Cyc_Absyn_PtrInfo _tmp4BC;_tmp4BC.elt_typ=telt;_tmp4BC.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4BC.ptr_atts=({(_tmp4BC.ptr_atts).rgn=rgn;(_tmp4BC.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4BC.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp4BC.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp4BC.ptr_atts).ptrloc=0;_tmp4BC.ptr_atts;});_tmp4BC;});_tmp4BB;});_tmp4BA;});
# 2564
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp490,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp490,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2571
void*_tmp4B9=res_typ;_npop_handler(1);return _tmp4B9;};};
# 2554
;_pop_region(temp);};}}_LL27C:;};}
# 2440
;_pop_region(r);}
# 2578
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
e->topt=(t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0));
return t;}
# 2588
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2592
{struct _RegionHandle _tmp4C0=_new_region("temp");struct _RegionHandle*temp=& _tmp4C0;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e))
# 2598
({void*_tmp4C1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4C2="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp4C2,sizeof(char),49);}),_tag_dyneither(_tmp4C1,sizeof(void*),0));});
# 2593
;_pop_region(temp);}{
# 2601
void*_tmp4C3=e->r;void*_tmp4C4=_tmp4C3;switch(*((int*)_tmp4C4)){case 25: _LL298: _LL299:
 goto _LL29B;case 26: _LL29A: _LL29B:
 goto _LL29D;case 27: _LL29C: _LL29D:
 goto _LL29F;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C4)->f1).String_c).tag){case 9: _LL29E: _LL29F:
 goto _LL2A1;case 8: _LL2A0: _LL2A1:
 return t;default: goto _LL2A2;}default: _LL2A2: _LL2A3:
# 2608
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2611
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2614
return t;}_LL297:;};}
# 2625 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp4C5=e->r;void*_tmp4C6=_tmp4C5;struct Cyc_Absyn_Exp*_tmp4EB;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C6)->tag == 11){_LL2A5: _tmp4EB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C6)->f1;_LL2A6:
# 2629
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp4EB);
_tmp4EB->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp4EB->topt),0);
e->topt=_tmp4EB->topt;
goto _LL2A4;}else{_LL2A7: _LL2A8:
# 2635
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2638
{void*_tmp4C7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp4C8=_tmp4C7;void*_tmp4EA;struct Cyc_Absyn_Tqual _tmp4E9;void*_tmp4E8;union Cyc_Absyn_Constraint*_tmp4E7;union Cyc_Absyn_Constraint*_tmp4E6;union Cyc_Absyn_Constraint*_tmp4E5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->tag == 5){_LL2AA: _tmp4EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).elt_typ;_tmp4E9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).elt_tq;_tmp4E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).ptr_atts).rgn;_tmp4E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).ptr_atts).nullable;_tmp4E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).ptr_atts).bounds;_tmp4E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8)->f1).ptr_atts).zero_term;_LL2AB:
# 2640
{void*_tmp4C9=Cyc_Tcutil_compress(_tmp4EA);void*_tmp4CA=_tmp4C9;struct Cyc_List_List*_tmp4E4;void*_tmp4E3;struct Cyc_Absyn_Tqual _tmp4E2;void*_tmp4E1;struct Cyc_List_List*_tmp4E0;int _tmp4DF;struct Cyc_Absyn_VarargInfo*_tmp4DE;struct Cyc_List_List*_tmp4DD;struct Cyc_List_List*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_List_List*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_List_List*_tmp4D8;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->tag == 9){_LL2AF: _tmp4E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).tvars;_tmp4E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).effect;_tmp4E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).ret_tqual;_tmp4E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).ret_typ;_tmp4E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).args;_tmp4DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).c_varargs;_tmp4DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).cyc_varargs;_tmp4DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).rgn_po;_tmp4DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).attributes;_tmp4DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).requires_clause;_tmp4DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).requires_relns;_tmp4D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).ensures_clause;_tmp4D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4CA)->f1).ensures_relns;_LL2B0:
# 2642
 if(_tmp4E4 != 0){
struct _RegionHandle _tmp4CB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4CB;_push_region(rgn);
{struct _tuple12 _tmp4CC=({struct _tuple12 _tmp4D7;_tmp4D7.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp4D7.f2=rgn;_tmp4D7;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4CC,_tmp4E4);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2651
_tmp4DD=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp4DD);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp4DD);{
void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp4D5;_tmp4D5.tag=9;_tmp4D5.f1=({struct Cyc_Absyn_FnInfo _tmp4D6;_tmp4D6.tvars=0;_tmp4D6.effect=_tmp4E3;_tmp4D6.ret_tqual=_tmp4E2;_tmp4D6.ret_typ=_tmp4E1;_tmp4D6.args=_tmp4E0;_tmp4D6.c_varargs=_tmp4DF;_tmp4D6.cyc_varargs=_tmp4DE;_tmp4D6.rgn_po=0;_tmp4D6.attributes=_tmp4DC;_tmp4D6.requires_clause=_tmp4DB;_tmp4D6.requires_relns=_tmp4DA;_tmp4D6.ensures_clause=_tmp4D9;_tmp4D6.ensures_relns=_tmp4D8;_tmp4D6;});_tmp4D5;});_tmp4D4;}));
# 2658
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CD=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4D2;_tmp4D2.tag=5;_tmp4D2.f1=({struct Cyc_Absyn_PtrInfo _tmp4D3;_tmp4D3.elt_typ=ftyp;_tmp4D3.elt_tq=_tmp4E9;_tmp4D3.ptr_atts=({(_tmp4D3.ptr_atts).rgn=_tmp4E8;(_tmp4D3.ptr_atts).nullable=_tmp4E7;(_tmp4D3.ptr_atts).bounds=_tmp4E6;(_tmp4D3.ptr_atts).zero_term=_tmp4E5;(_tmp4D3.ptr_atts).ptrloc=0;_tmp4D3.ptr_atts;});_tmp4D3;});_tmp4D2;});_tmp4D1;});
# 2660
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF[0]=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp4D0;_tmp4D0.tag=12;_tmp4D0.f1=_tmp4CE;_tmp4D0.f2=ts;_tmp4D0;});_tmp4CF;});
e->topt=(void*)_tmp4CD;};}
# 2644
;_pop_region(rgn);}
# 2665
goto _LL2AE;}else{_LL2B1: _LL2B2:
 goto _LL2AE;}_LL2AE:;}
# 2668
goto _LL2A9;}else{_LL2AC: _LL2AD:
 goto _LL2A9;}_LL2A9:;}
# 2671
goto _LL2A4;}_LL2A4:;}
# 2673
return(void*)_check_null(e->topt);}
# 2681
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2683
struct _RegionHandle _tmp4EC=_new_region("r");struct _RegionHandle*r=& _tmp4EC;_push_region(r);
{struct Cyc_List_List*_tmp4ED=0;
# 2686
{void*_tmp4EE=fn_exp->r;void*_tmp4EF=_tmp4EE;struct Cyc_List_List*_tmp503;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4EF)->tag == 9){_LL2B4: _tmp503=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4EF)->f2;_LL2B5:
# 2688
{void*_tmp4F0=e->r;void*_tmp4F1=_tmp4F0;struct Cyc_List_List*_tmp500;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4F1)->tag == 9){_LL2B9: _tmp500=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4F1)->f2;_LL2BA: {
# 2690
struct Cyc_List_List*_tmp4F2=alias_arg_exps;
int _tmp4F3=0;
while(_tmp4F2 != 0){
while(_tmp4F3 != (int)_tmp4F2->hd){
if(_tmp503 == 0)
({void*_tmp4F4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=1;_tmp4F8.f1=(unsigned long)((int)_tmp4F2->hd);({struct Cyc_Int_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=1;_tmp4F7.f1=(unsigned long)_tmp4F3;({void*_tmp4F5[2]={& _tmp4F7,& _tmp4F8};Cyc_aprintf(({const char*_tmp4F6="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp4F6,sizeof(char),36);}),_tag_dyneither(_tmp4F5,sizeof(void*),2));});});}),_tag_dyneither(_tmp4F4,sizeof(void*),0));});
# 2697
++ _tmp4F3;
_tmp503=_tmp503->tl;
_tmp500=((struct Cyc_List_List*)_check_null(_tmp500))->tl;}{
# 2702
struct _tuple11 _tmp4F9=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp503))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500))->hd)->topt)));struct _tuple11 _tmp4FA=_tmp4F9;struct Cyc_Absyn_Decl*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FC;_LL2BE: _tmp4FD=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;_LL2BF:;
_tmp503->hd=(void*)_tmp4FC;
_tmp4ED=({struct Cyc_List_List*_tmp4FB=_region_malloc(r,sizeof(*_tmp4FB));_tmp4FB->hd=_tmp4FD;_tmp4FB->tl=_tmp4ED;_tmp4FB;});
_tmp4F2=_tmp4F2->tl;};}
# 2707
goto _LL2B8;}}else{_LL2BB: _LL2BC:
({void*_tmp4FE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4FF="not a function call!";_tag_dyneither(_tmp4FF,sizeof(char),21);}),_tag_dyneither(_tmp4FE,sizeof(void*),0));});}_LL2B8:;}
# 2710
goto _LL2B3;}else{_LL2B6: _LL2B7:
({void*_tmp501=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp502="not a function call!";_tag_dyneither(_tmp502,sizeof(char),21);}),_tag_dyneither(_tmp501,sizeof(void*),0));});}_LL2B3:;}
# 2715
while(_tmp4ED != 0){
struct Cyc_Absyn_Decl*_tmp504=(struct Cyc_Absyn_Decl*)_tmp4ED->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp504,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp4ED=_tmp4ED->tl;}
# 2722
e->topt=0;
e->r=fn_exp->r;}
# 2684
;_pop_region(r);}
# 2729
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2733
{void*_tmp505=e->r;void*_tmp506=_tmp505;void*_tmp572;struct Cyc_Absyn_Exp*_tmp571;struct _dyneither_ptr*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp56E;int*_tmp56D;struct Cyc_Absyn_Exp**_tmp56C;void***_tmp56B;struct Cyc_Absyn_Exp**_tmp56A;int*_tmp569;void*_tmp568;struct Cyc_Absyn_Enumfield*_tmp567;struct Cyc_Absyn_Enumdecl*_tmp566;struct Cyc_Absyn_Enumfield*_tmp565;struct Cyc_List_List*_tmp564;struct Cyc_Absyn_Datatypedecl*_tmp563;struct Cyc_Absyn_Datatypefield*_tmp562;void*_tmp561;struct Cyc_List_List*_tmp560;struct _tuple1**_tmp55F;struct Cyc_List_List**_tmp55E;struct Cyc_List_List*_tmp55D;struct Cyc_Absyn_Aggrdecl**_tmp55C;struct Cyc_Absyn_Exp*_tmp55B;void*_tmp55A;int*_tmp559;struct Cyc_Absyn_Vardecl*_tmp558;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp556;int*_tmp555;struct Cyc_Absyn_Stmt*_tmp554;struct Cyc_List_List*_tmp553;struct _tuple9*_tmp552;struct Cyc_List_List*_tmp551;struct Cyc_List_List*_tmp550;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54D;struct _dyneither_ptr*_tmp54C;int*_tmp54B;int*_tmp54A;struct Cyc_Absyn_Exp*_tmp549;struct _dyneither_ptr*_tmp548;int*_tmp547;int*_tmp546;struct Cyc_Absyn_Exp*_tmp545;void*_tmp544;struct Cyc_List_List*_tmp543;void*_tmp542;struct Cyc_Absyn_Exp*_tmp541;struct Cyc_Absyn_Exp**_tmp540;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp*_tmp53E;void*_tmp53D;struct Cyc_Absyn_Exp*_tmp53C;enum Cyc_Absyn_Coercion*_tmp53B;struct Cyc_Absyn_Exp*_tmp53A;struct Cyc_List_List*_tmp539;struct Cyc_Absyn_Exp*_tmp538;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Core_Opt*_tmp52D;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;enum Cyc_Absyn_Incrementor _tmp52A;enum Cyc_Absyn_Primop _tmp529;struct Cyc_List_List*_tmp528;void**_tmp527;union Cyc_Absyn_Cnst*_tmp526;struct Cyc_Core_Opt*_tmp525;struct Cyc_List_List*_tmp524;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_VarargCallInfo**_tmp521;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_List_List*_tmp51F;struct Cyc_Absyn_VarargCallInfo**_tmp51E;int*_tmp51D;struct Cyc_Absyn_Exp*_tmp51C;switch(*((int*)_tmp506)){case 11: _LL2C1: _tmp51C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2C2:
# 2738
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp51C);
return;case 9: _LL2C3: _tmp520=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp51F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp51E=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_tmp51D=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506)->f4;if(!(*_tmp51D)){_LL2C4:
# 2742
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507[0]=({struct Cyc_Core_Impossible_exn_struct _tmp508;_tmp508.tag=Cyc_Core_Impossible;_tmp508.f1=({const char*_tmp509="unresolved function in tcExpNoInst";_tag_dyneither(_tmp509,sizeof(char),35);});_tmp508;});_tmp507;}));}else{_LL2D9: _tmp523=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp522=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp521=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_LL2DA: {
# 2773
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp50A;_push_handler(& _tmp50A);{int _tmp50C=0;if(setjmp(_tmp50A.handler))_tmp50C=1;if(!_tmp50C){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp50B=(void*)_exn_thrown;void*_tmp50D=_tmp50B;void*_tmp50F;struct _dyneither_ptr _tmp50E;if(((struct Cyc_Core_Failure_exn_struct*)_tmp50D)->tag == Cyc_Core_Failure){_LL314: _tmp50E=((struct Cyc_Core_Failure_exn_struct*)_tmp50D)->f1;_LL315:
# 2780
 ok=0;
fn_exp=e;
goto _LL313;}else{_LL316: _tmp50F=_tmp50D;_LL317:(int)_rethrow(_tmp50F);}_LL313:;}};}
# 2784
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp523,_tmp522,_tmp521,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2791
goto _LL2C0;}}case 35: _LL2C5: _tmp525=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp524=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2C6:
# 2746
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp524);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0: _LL2C7: _tmp526=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2C8:
# 2751
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp526,e);goto _LL2C0;case 1: _LL2C9: _tmp527=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2CA:
# 2753
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp527);goto _LL2C0;case 2: _LL2CB: _tmp529=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp528=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2CC:
# 2755
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp529,_tmp528);goto _LL2C0;case 4: _LL2CD: _tmp52B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp52A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2CE:
# 2757
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp52B,_tmp52A);goto _LL2C0;case 3: _LL2CF: _tmp52E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp52D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp52C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_LL2D0:
# 2759
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp52E,_tmp52D,_tmp52C);goto _LL2C0;case 5: _LL2D1: _tmp531=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp530=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp52F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_LL2D2:
# 2761
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp531,_tmp530,_tmp52F);goto _LL2C0;case 6: _LL2D3: _tmp533=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp532=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2D4:
# 2763
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp533,_tmp532);goto _LL2C0;case 7: _LL2D5: _tmp535=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp534=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2D6:
# 2765
 t=Cyc_Tcexp_tcOr(te,loc,_tmp535,_tmp534);goto _LL2C0;case 8: _LL2D7: _tmp537=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp536=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2D8:
# 2767
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp537,_tmp536);goto _LL2C0;case 10: _LL2DB: _tmp538=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2DC:
# 2793
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp538);goto _LL2C0;case 12: _LL2DD: _tmp53A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp539=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2DE:
# 2795
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp53A,_tmp539);goto _LL2C0;case 13: _LL2DF: _tmp53D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp53C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp53B=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp506)->f4;_LL2E0:
# 2797
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp53D,_tmp53C,_tmp53B);goto _LL2C0;case 14: _LL2E1: _tmp53E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2E2:
# 2799
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp53E);goto _LL2C0;case 15: _LL2E3: _tmp540=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp53F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2E4:
# 2801
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp540,e,_tmp53F);goto _LL2C0;case 17: _LL2E5: _tmp541=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2E6: {
# 2803
void*_tmp510=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp541);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp510);goto _LL2C0;}case 16: _LL2E7: _tmp542=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2E8:
# 2806
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp542);goto _LL2C0;case 18: _LL2E9: _tmp544=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp543=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2EA:
# 2808
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp544,_tmp543);goto _LL2C0;case 19: _LL2EB: _tmp545=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2EC:
# 2810
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp545);goto _LL2C0;case 20: _LL2ED: _tmp549=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp548=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp547=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_tmp546=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp506)->f4;_LL2EE:
# 2812
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp549,_tmp548,_tmp547,_tmp546);goto _LL2C0;case 21: _LL2EF: _tmp54D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp54C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp54B=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_tmp54A=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp506)->f4;_LL2F0:
# 2814
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp54D,_tmp54C,_tmp54B,_tmp54A);goto _LL2C0;case 22: _LL2F1: _tmp54F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp54E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2F2:
# 2816
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp54F,_tmp54E);goto _LL2C0;case 23: _LL2F3: _tmp550=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2F4:
# 2818
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp550);goto _LL2C0;case 24: _LL2F5: _tmp552=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp551=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL2F6:
# 2820
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp552,_tmp551);goto _LL2C0;case 25: _LL2F7: _tmp553=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2F8: {
# 2824
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp511=Cyc_Tcutil_compress(*topt);void*_tmp512=_tmp511;void**_tmp515;struct Cyc_Absyn_Tqual*_tmp514;union Cyc_Absyn_Constraint*_tmp513;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp512)->tag == 8){_LL319: _tmp515=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp512)->f1).elt_type;_tmp514=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp512)->f1).tq;_tmp513=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp512)->f1).zero_term;_LL31A:
# 2830
 elt_topt=_tmp515;
elt_tqopt=_tmp514;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp513);
goto _LL318;}else{_LL31B: _LL31C:
 goto _LL318;}_LL318:;}
# 2837
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp553);goto _LL2C0;}case 36: _LL2F9: _tmp554=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL2FA:
# 2839
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp554);goto _LL2C0;case 26: _LL2FB: _tmp558=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp557=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp556=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_tmp555=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp506)->f4;_LL2FC:
# 2841
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp558,_tmp557,_tmp556,_tmp555);goto _LL2C0;case 27: _LL2FD: _tmp55B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp55A=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp559=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_LL2FE:
# 2843
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp55B,_tmp55A,_tmp559);goto _LL2C0;case 28: _LL2FF: _tmp55F=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp55E=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp55D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_tmp55C=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp506)->f4;_LL300:
# 2845
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp55F,_tmp55E,_tmp55D,_tmp55C);goto _LL2C0;case 29: _LL301: _tmp561=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp560=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL302:
# 2847
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp561,_tmp560);goto _LL2C0;case 30: _LL303: _tmp564=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp563=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_tmp562=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp506)->f3;_LL304:
# 2849
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp564,_tmp563,_tmp562);goto _LL2C0;case 31: _LL305: _tmp566=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp565=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL306:
# 2851
 t=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp517;_tmp517.tag=13;_tmp517.f1=_tmp566->name;_tmp517.f2=_tmp566;_tmp517;});_tmp516;});goto _LL2C0;case 32: _LL307: _tmp568=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp567=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL308:
# 2853
 t=_tmp568;goto _LL2C0;case 33: _LL309: _tmp56D=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp506)->f1).is_calloc;_tmp56C=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp506)->f1).rgn;_tmp56B=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp506)->f1).elt_type;_tmp56A=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp506)->f1).num_elts;_tmp569=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp506)->f1).fat_result;_LL30A:
# 2855
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp56C,_tmp56B,_tmp56A,_tmp56D,_tmp569);goto _LL2C0;case 34: _LL30B: _tmp56F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp56E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL30C:
# 2857
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp56F,_tmp56E);goto _LL2C0;case 37: _LL30D: _tmp571=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_tmp570=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp506)->f2;_LL30E:
# 2859
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp571,_tmp570);goto _LL2C0;case 38: _LL30F: _tmp572=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp506)->f1;_LL310:
# 2861
 if(!te->allow_valueof)
({void*_tmp518=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp519="valueof(-) can only occur within types";_tag_dyneither(_tmp519,sizeof(char),39);}),_tag_dyneither(_tmp518,sizeof(void*),0));});
# 2869
t=Cyc_Absyn_sint_typ;
goto _LL2C0;default: _LL311: _LL312:
# 2872
({void*_tmp51A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp51B="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp51B,sizeof(char),50);}),_tag_dyneither(_tmp51A,sizeof(void*),0));});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}_LL2C0:;}
# 2875
e->topt=t;}
