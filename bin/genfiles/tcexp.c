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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 110
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 256
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 291
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 357
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 370
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 450 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 453
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 549
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 551
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 952 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 958
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 961
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 964
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 966
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 977
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 979
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 982
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 984
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 986
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 996
void*Cyc_Absyn_exn_typ();
# 1007
extern void*Cyc_Absyn_bounds_one;
# 1012
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1015
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1018
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1023
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1034
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1087
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1097
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1101
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1107
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1111
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1122
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1183
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1185
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1188
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1190
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
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 380
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
{void*_tmp5C5;(_tmp5C5=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp5C5,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp5C6;return(_tmp5C6=_cycalloc(sizeof(*_tmp5C6)),((_tmp5C6->f1=0,((_tmp5C6->f2=e,_tmp5C6)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;void*_tmp3=_tmp2;struct _tuple1*_tmp4;void**_tmp5;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3)->tag == 1){_LL1: _tmp4=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp5=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_tmpA=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpB=_tmpA;struct Cyc_Absyn_Aggrdecl*_tmpC;struct Cyc_Absyn_Datatypedecl*_tmpD;struct Cyc_Absyn_Datatypefield*_tmpE;void*_tmpF;switch(*((int*)_tmpB)){case 0: if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB)->f1)->tag == 0){_LL6: _LL7:
# 83
{const char*_tmp5CA;void*_tmp5C9[1];struct Cyc_String_pa_PrintArg_struct _tmp5C8;(_tmp5C8.tag=0,((_tmp5C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp5C9[0]=& _tmp5C8,Cyc_Tcutil_terr(e1->loc,((_tmp5CA="undeclared identifier %s",_tag_dyneither(_tmp5CA,sizeof(char),25))),_tag_dyneither(_tmp5C9,sizeof(void*),1)))))));}{
int _tmp13=0;_npop_handler(1);return _tmp13;};}else{_LL8: _tmpF=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB)->f1;_LL9:
# 86
*_tmp5=_tmpF;{
int _tmp14=1;_npop_handler(1);return _tmp14;};}case 2: _LLA: _tmpD=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB)->f1;_tmpE=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB)->f2;_LLB:
# 89
 if(_tmpE->typs == 0){
# 91
{const char*_tmp5CE;void*_tmp5CD[1];struct Cyc_String_pa_PrintArg_struct _tmp5CC;(_tmp5CC.tag=0,((_tmp5CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpE->name)),((_tmp5CD[0]=& _tmp5CC,Cyc_Tcutil_terr(e->loc,((_tmp5CE="%s is a constant, not a function",_tag_dyneither(_tmp5CE,sizeof(char),33))),_tag_dyneither(_tmp5CD,sizeof(void*),1)))))));}{
int _tmp18=0;_npop_handler(1);return _tmp18;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp5D1;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp5D0;e->r=(void*)((_tmp5D0=_cycalloc(sizeof(*_tmp5D0)),((_tmp5D0[0]=((_tmp5D1.tag=30,((_tmp5D1.f1=es,((_tmp5D1.f2=_tmpD,((_tmp5D1.f3=_tmpE,_tmp5D1)))))))),_tmp5D0))));}{
int _tmp1B=1;_npop_handler(1);return _tmp1B;};case 1: _LLC: _tmpC=((struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB)->f1;_LLD: {
# 98
struct Cyc_List_List*_tmp1C=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp5D4;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5D3;e->r=(void*)((_tmp5D3=_cycalloc(sizeof(*_tmp5D3)),((_tmp5D3[0]=((_tmp5D4.tag=28,((_tmp5D4.f1=_tmpC->name,((_tmp5D4.f2=0,((_tmp5D4.f3=_tmp1C,((_tmp5D4.f4=_tmpC,_tmp5D4)))))))))),_tmp5D3))));}{
int _tmp1F=1;_npop_handler(1);return _tmp1F;};}case 4: _LLE: _LLF:
 goto _LL11;default: _LL10: _LL11:
# 103
{const char*_tmp5D8;void*_tmp5D7[1];struct Cyc_String_pa_PrintArg_struct _tmp5D6;(_tmp5D6.tag=0,((_tmp5D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp5D7[0]=& _tmp5D6,Cyc_Tcutil_terr(e->loc,((_tmp5D8="%s is an enum constructor, not a function",_tag_dyneither(_tmp5D8,sizeof(char),42))),_tag_dyneither(_tmp5D7,sizeof(void*),1)))))));}{
int _tmp23=0;_npop_handler(1);return _tmp23;};}_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp24=_tmp7;void*_tmp25;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp24)->tag == Cyc_Dict_Absent){_LL13: _LL14:
# 108
{const char*_tmp5DC;void*_tmp5DB[1];struct Cyc_String_pa_PrintArg_struct _tmp5DA;(_tmp5DA.tag=0,((_tmp5DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp5DB[0]=& _tmp5DA,Cyc_Tcutil_terr(e1->loc,((_tmp5DC="undeclared identifier %s",_tag_dyneither(_tmp5DC,sizeof(char),25))),_tag_dyneither(_tmp5DB,sizeof(void*),1)))))));}
return 0;}else{_LL15: _tmp25=_tmp24;_LL16:(int)_rethrow(_tmp25);}_LL12:;}};}}else{_LL3: _LL4:
# 112
 return 1;}_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp5DF;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5DE;e->r=(void*)((_tmp5DE=_cycalloc(sizeof(*_tmp5DE)),((_tmp5DE[0]=((_tmp5DF.tag=25,((_tmp5DF.f1=des,_tmp5DF)))),_tmp5DE))));}
return;}{
# 125
void*t=*topt;
void*_tmp2B=Cyc_Tcutil_compress(t);void*_tmp2C=_tmp2B;void*_tmp2D;struct Cyc_Absyn_Tqual _tmp2E;union Cyc_Absyn_AggrInfoU _tmp2F;switch(*((int*)_tmp2C)){case 11: _LL18: _tmp2F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C)->f1).aggr_info;_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp30=_tmp2F;struct Cyc_Absyn_Aggrdecl*_tmp31;if((_tmp30.UnknownAggr).tag == 1){_LL21: _LL22: {
const char*_tmp5E2;void*_tmp5E1;(_tmp5E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5E2="struct type not properly set",_tag_dyneither(_tmp5E2,sizeof(char),29))),_tag_dyneither(_tmp5E1,sizeof(void*),0)));}}else{_LL23: _tmp31=*(_tmp30.KnownAggr).val;_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp5E5;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5E4;e->r=(void*)((_tmp5E4=_cycalloc(sizeof(*_tmp5E4)),((_tmp5E4[0]=((_tmp5E5.tag=28,((_tmp5E5.f1=_tmp31->name,((_tmp5E5.f2=0,((_tmp5E5.f3=des,((_tmp5E5.f4=_tmp31,_tmp5E5)))))))))),_tmp5E4))));}}_LL20:;}
# 132
goto _LL17;case 8: _LL1A: _tmp2D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).elt_type;_tmp2E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).tq;_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp5E8;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5E7;e->r=(void*)((_tmp5E7=_cycalloc(sizeof(*_tmp5E7)),((_tmp5E7[0]=((_tmp5E8.tag=25,((_tmp5E8.f1=des,_tmp5E8)))),_tmp5E7))));}goto _LL17;case 12: _LL1C: _LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp5EB;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp5EA;e->r=(void*)((_tmp5EA=_cycalloc(sizeof(*_tmp5EA)),((_tmp5EA[0]=((_tmp5EB.tag=29,((_tmp5EB.f1=t,((_tmp5EB.f2=des,_tmp5EB)))))),_tmp5EA))));}goto _LL17;default: _LL1E: _LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp5EE;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5ED;e->r=(void*)((_tmp5ED=_cycalloc(sizeof(*_tmp5ED)),((_tmp5ED[0]=((_tmp5EE.tag=25,((_tmp5EE.f1=des,_tmp5EE)))),_tmp5ED))));}goto _LL17;}_LL17:;};}
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
const char*_tmp5F3;void*_tmp5F2[2];struct Cyc_String_pa_PrintArg_struct _tmp5F1;struct Cyc_String_pa_PrintArg_struct _tmp5F0;(_tmp5F0.tag=0,((_tmp5F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp5F1.tag=0,((_tmp5F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp5F2[0]=& _tmp5F1,((_tmp5F2[1]=& _tmp5F0,Cyc_Tcutil_terr(e->loc,((_tmp5F3="test of %s has type %s instead of integral or pointer type",_tag_dyneither(_tmp5F3,sizeof(char),59))),_tag_dyneither(_tmp5F2,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp40=e->r;void*_tmp41=_tmp40;enum Cyc_Absyn_Primop _tmp42;struct Cyc_List_List*_tmp43;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41)->tag == 2){_LL26: _tmp42=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_tmp43=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL27:
# 163
 if(_tmp42 == Cyc_Absyn_Eq  || _tmp42 == Cyc_Absyn_Neq){
struct _tuple0 _tmp5F4;struct _tuple0 _tmp44=(_tmp5F4.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp43))->hd)->topt),((_tmp5F4.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp43))->tl))->hd)->topt),_tmp5F4)));struct _tuple0 _tmp45=_tmp44;void*_tmp46;void*_tmp47;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp45.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp45.f2)->tag == 15){_LL2B: _tmp46=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp45.f1)->f1;_tmp47=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp45.f2)->f1;_LL2C: {
# 166
struct _tuple0*_tmp5F7;struct Cyc_Tcexp_TestEnv _tmp5F6;return(_tmp5F6.eq=((_tmp5F7=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp5F7)),((_tmp5F7->f1=_tmp46,((_tmp5F7->f2=_tmp47,_tmp5F7)))))),((_tmp5F6.isTrue=_tmp42 == Cyc_Absyn_Eq,_tmp5F6)));}}else{goto _LL2D;}}else{_LL2D: _LL2E:
# 168
 goto _LL2A;}_LL2A:;}
# 171
goto _LL25;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp5F8;return(_tmp5F8.eq=0,((_tmp5F8.isTrue=0,_tmp5F8)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp4C=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp4D=_tmp4C;struct _dyneither_ptr _tmp4E;struct _dyneither_ptr _tmp4F;enum Cyc_Absyn_Sign _tmp50;int _tmp51;int _tmp52;enum Cyc_Absyn_Sign _tmp53;enum Cyc_Absyn_Sign _tmp54;switch((_tmp4D.Wstring_c).tag){case 2: switch(((_tmp4D.Char_c).val).f1){case Cyc_Absyn_Signed: _LL30: _LL31:
 t=Cyc_Absyn_schar_typ;goto _LL2F;case Cyc_Absyn_Unsigned: _LL32: _LL33:
 t=Cyc_Absyn_uchar_typ;goto _LL2F;default: _LL34: _LL35:
 t=Cyc_Absyn_char_typ;goto _LL2F;}case 3: _LL36: _LL37:
 t=Cyc_Absyn_wchar_typ();goto _LL2F;case 4: _LL38: _tmp54=((_tmp4D.Short_c).val).f1;_LL39:
# 210
 t=_tmp54 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL2F;case 6: _LL3A: _tmp53=((_tmp4D.LongLong_c).val).f1;_LL3B:
# 212
 t=_tmp53 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL2F;case 7: _LL3C: _tmp52=((_tmp4D.Float_c).val).f2;_LL3D:
 t=Cyc_Absyn_float_typ(_tmp52);goto _LL2F;case 5: _LL3E: _tmp50=((_tmp4D.Int_c).val).f1;_tmp51=((_tmp4D.Int_c).val).f2;_LL3F:
# 215
 if(topt == 0)
t=_tmp50 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 222
void*_tmp55=Cyc_Tcutil_compress(*topt);void*_tmp56=_tmp55;void*_tmp57;void*_tmp58;struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;union Cyc_Absyn_Constraint*_tmp5B;union Cyc_Absyn_Constraint*_tmp5C;union Cyc_Absyn_Constraint*_tmp5D;enum Cyc_Absyn_Sign _tmp5E;enum Cyc_Absyn_Sign _tmp5F;enum Cyc_Absyn_Sign _tmp60;enum Cyc_Absyn_Sign _tmp61;switch(*((int*)_tmp56)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp56)->f2){case Cyc_Absyn_Char_sz: _LL47: _tmp61=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp56)->f1;_LL48:
# 224
{enum Cyc_Absyn_Sign _tmp62=_tmp61;switch(_tmp62){case Cyc_Absyn_Unsigned: _LL56: _LL57:
 t=Cyc_Absyn_uchar_typ;goto _LL55;case Cyc_Absyn_Signed: _LL58: _LL59:
 t=Cyc_Absyn_schar_typ;goto _LL55;default: _LL5A: _LL5B:
 t=Cyc_Absyn_char_typ;goto _LL55;}_LL55:;}
# 229
*c=Cyc_Absyn_Char_c(_tmp61,(char)_tmp51);
goto _LL46;case Cyc_Absyn_Short_sz: _LL49: _tmp60=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp56)->f1;_LL4A:
# 232
 t=_tmp60 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
*c=Cyc_Absyn_Short_c(_tmp60,(short)_tmp51);
goto _LL46;case Cyc_Absyn_Int_sz: _LL4B: _tmp5F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp56)->f1;_LL4C:
# 236
 t=_tmp5F == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;case Cyc_Absyn_Long_sz: _LL4D: _tmp5E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp56)->f1;_LL4E:
# 239
 t=_tmp5E == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;default: goto _LL53;}case 5: _LL4F: _tmp58=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56)->f1).elt_typ;_tmp59=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56)->f1).elt_tq;_tmp5A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56)->f1).ptr_atts).rgn;_tmp5B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56)->f1).ptr_atts).nullable;_tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56)->f1).ptr_atts).bounds;_tmp5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56)->f1).ptr_atts).zero_term;if(_tmp51 == 0){_LL50: {
# 243
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp5B))return*topt;{
struct Cyc_List_List*_tmp63=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5FE;struct Cyc_Absyn_PtrInfo _tmp5FD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FC;t=(void*)((_tmp5FC=_cycalloc(sizeof(*_tmp5FC)),((_tmp5FC[0]=((_tmp5FE.tag=5,((_tmp5FE.f1=((_tmp5FD.elt_typ=_tmp58,((_tmp5FD.elt_tq=_tmp59,((_tmp5FD.ptr_atts=(
((_tmp5FD.ptr_atts).rgn=_tmp5A,(((_tmp5FD.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp5FD.ptr_atts).bounds=_tmp5C,(((_tmp5FD.ptr_atts).zero_term=_tmp5D,(((_tmp5FD.ptr_atts).ptrloc=0,_tmp5FD.ptr_atts)))))))))),_tmp5FD)))))),_tmp5FE)))),_tmp5FC))));}
goto _LL46;};}}else{goto _LL53;}case 19: _LL51: _tmp57=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp56)->f1;_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp601;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp600;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp68=(_tmp600=_cycalloc(sizeof(*_tmp600)),((_tmp600[0]=((_tmp601.tag=18,((_tmp601.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp51,0),_tmp601)))),_tmp600)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp604;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp603;t=(void*)((_tmp603=_cycalloc(sizeof(*_tmp603)),((_tmp603[0]=((_tmp604.tag=19,((_tmp604.f1=(void*)_tmp68,_tmp604)))),_tmp603))));}
goto _LL46;}default: _LL53: _LL54:
# 261
 t=_tmp50 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;}_LL46:;}
# 264
goto _LL2F;case 8: _LL40: _tmp4F=(_tmp4D.String_c).val;_LL41:
# 266
 string_numelts=(int)_get_dyneither_size(_tmp4F,sizeof(char));
_tmp4E=_tmp4F;goto _LL43;case 9: _LL42: _tmp4E=(_tmp4D.Wstring_c).val;_LL43:
# 269
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp4E);
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 273
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
elen->topt=Cyc_Absyn_uint_typ;{
# 278
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp607;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp606;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D=(_tmp606=_cycalloc(sizeof(*_tmp606)),((_tmp606[0]=((_tmp607.tag=1,((_tmp607.f1=elen,_tmp607)))),_tmp606)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp6D,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_tmp6E=Cyc_Tcutil_compress(*topt);void*_tmp6F=_tmp6E;struct Cyc_Absyn_Tqual _tmp70;switch(*((int*)_tmp6F)){case 8: _LL5D: _tmp70=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6F)->f1).tq;_LL5E:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp70,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);case 5: _LL5F: _LL60:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp6D,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 302
goto _LL5C;default: _LL61: _LL62:
 goto _LL5C;}_LL5C:;}
# 306
return t;};};default: _LL44: _LL45:
# 308
 if(topt != 0){
void*_tmp73=Cyc_Tcutil_compress(*topt);void*_tmp74=_tmp73;void*_tmp75;struct Cyc_Absyn_Tqual _tmp76;void*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;union Cyc_Absyn_Constraint*_tmp7A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->tag == 5){_LL64: _tmp75=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->f1).elt_typ;_tmp76=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->f1).elt_tq;_tmp77=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->f1).ptr_atts).rgn;_tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->f1).ptr_atts).nullable;_tmp79=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->f1).ptr_atts).bounds;_tmp7A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->f1).ptr_atts).zero_term;_LL65:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp78))return*topt;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp60D;struct Cyc_Absyn_PtrInfo _tmp60C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60B;return(void*)((_tmp60B=_cycalloc(sizeof(*_tmp60B)),((_tmp60B[0]=((_tmp60D.tag=5,((_tmp60D.f1=((_tmp60C.elt_typ=_tmp75,((_tmp60C.elt_tq=_tmp76,((_tmp60C.ptr_atts=(((_tmp60C.ptr_atts).rgn=_tmp77,(((_tmp60C.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp60C.ptr_atts).bounds=_tmp79,(((_tmp60C.ptr_atts).zero_term=_tmp7A,(((_tmp60C.ptr_atts).ptrloc=0,_tmp60C.ptr_atts)))))))))),_tmp60C)))))),_tmp60D)))),_tmp60B))));};}else{_LL66: _LL67:
# 315
 goto _LL63;}_LL63:;}{
# 318
struct Cyc_List_List*_tmp7E=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp61C;struct Cyc_Core_Opt*_tmp61B;struct Cyc_Core_Opt*_tmp61A;struct Cyc_Absyn_PtrInfo _tmp619;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp618;t=(void*)((_tmp618=_cycalloc(sizeof(*_tmp618)),((_tmp618[0]=((_tmp61C.tag=5,((_tmp61C.f1=((_tmp619.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp61A=_cycalloc(sizeof(*_tmp61A)),((_tmp61A->v=_tmp7E,_tmp61A))))),((_tmp619.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp619.ptr_atts=(
((_tmp619.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp61B=_cycalloc(sizeof(*_tmp61B)),((_tmp61B->v=_tmp7E,_tmp61B))))),(((_tmp619.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp619.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp619.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp619.ptr_atts).ptrloc=0,_tmp619.ptr_atts)))))))))),_tmp619)))))),_tmp61C)))),_tmp618))));}
# 325
goto _LL2F;};}_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_tmp84=*((void**)_check_null(b));void*_tmp85=_tmp84;struct Cyc_Absyn_Vardecl*_tmp86;struct Cyc_Absyn_Vardecl*_tmp87;struct Cyc_Absyn_Vardecl*_tmp88;struct Cyc_Absyn_Fndecl*_tmp89;struct Cyc_Absyn_Vardecl*_tmp8A;switch(*((int*)_tmp85)){case 0: _LL69: _LL6A: {
# 342
struct _handler_cons _tmp8B;_push_handler(& _tmp8B);{int _tmp8D=0;if(setjmp(_tmp8B.handler))_tmp8D=1;if(!_tmp8D){
{struct _RegionHandle _tmp8E=_new_region("r");struct _RegionHandle*r=& _tmp8E;_push_region(r);
{void*_tmp8F=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmp90=_tmp8F;struct Cyc_Absyn_Datatypedecl*_tmp91;struct Cyc_Absyn_Datatypefield*_tmp92;void*_tmp93;struct Cyc_Absyn_Enumfield*_tmp94;struct Cyc_Absyn_Enumdecl*_tmp95;struct Cyc_Absyn_Enumfield*_tmp96;void*_tmp97;switch(*((int*)_tmp90)){case 0: if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp90)->f1)->tag == 0){_LL76: _LL77: {
# 346
const char*_tmp620;void*_tmp61F[1];struct Cyc_String_pa_PrintArg_struct _tmp61E;void*_tmp9B=(_tmp61E.tag=0,((_tmp61E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp61F[0]=& _tmp61E,Cyc_Tcexp_expr_err(te,loc,0,((_tmp620="undeclared identifier %s",_tag_dyneither(_tmp620,sizeof(char),25))),_tag_dyneither(_tmp61F,sizeof(void*),1)))))));_npop_handler(1);return _tmp9B;}}else{_LL78: _tmp97=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp90)->f1;_LL79:
# 348
*b=_tmp97;{
void*_tmp9C=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmp9C;};}case 3: _LL7A: _tmp95=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp90)->f1;_tmp96=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp90)->f2;_LL7B:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp623;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp622;e->r=(void*)((_tmp622=_cycalloc(sizeof(*_tmp622)),((_tmp622[0]=((_tmp623.tag=31,((_tmp623.f1=_tmp96->name,((_tmp623.f2=_tmp95,((_tmp623.f3=_tmp96,_tmp623)))))))),_tmp622))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp626;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp625;void*_tmpA1=(void*)((_tmp625=_cycalloc(sizeof(*_tmp625)),((_tmp625[0]=((_tmp626.tag=13,((_tmp626.f1=_tmp95->name,((_tmp626.f2=_tmp95,_tmp626)))))),_tmp625))));_npop_handler(1);return _tmpA1;};case 4: _LL7C: _tmp93=(void*)((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp90)->f1;_tmp94=((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp90)->f2;_LL7D:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp629;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp628;e->r=(void*)((_tmp628=_cycalloc(sizeof(*_tmp628)),((_tmp628[0]=((_tmp629.tag=32,((_tmp629.f1=_tmp94->name,((_tmp629.f2=_tmp93,((_tmp629.f3=_tmp94,_tmp629)))))))),_tmp628))));}{
void*_tmpA4=_tmp93;_npop_handler(1);return _tmpA4;};case 2: _LL7E: _tmp91=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp90)->f1;_tmp92=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp90)->f2;_LL7F:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp62C;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp62B;e->r=(void*)((_tmp62B=_cycalloc(sizeof(*_tmp62B)),((_tmp62B[0]=((_tmp62C.tag=30,((_tmp62C.f1=0,((_tmp62C.f2=_tmp91,((_tmp62C.f3=_tmp92,_tmp62C)))))))),_tmp62B))));}{
void*_tmpA7=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmp91,_tmp92);_npop_handler(1);return _tmpA7;};default: _LL80: _LL81: {
# 360
const char*_tmp630;void*_tmp62F[1];struct Cyc_String_pa_PrintArg_struct _tmp62E;void*_tmpAB=(_tmp62E.tag=0,((_tmp62E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp62F[0]=& _tmp62E,Cyc_Tcexp_expr_err(te,loc,0,((_tmp630="bad occurrence of type name %s",_tag_dyneither(_tmp630,sizeof(char),31))),_tag_dyneither(_tmp62F,sizeof(void*),1)))))));_npop_handler(1);return _tmpAB;}}_LL75:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmp8C=(void*)_exn_thrown;void*_tmpAC=_tmp8C;void*_tmpAD;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpAC)->tag == Cyc_Dict_Absent){_LL83: _LL84: {
# 365
const char*_tmp634;void*_tmp633[1];struct Cyc_String_pa_PrintArg_struct _tmp632;return(_tmp632.tag=0,((_tmp632.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp633[0]=& _tmp632,Cyc_Tcexp_expr_err(te,loc,0,((_tmp634="undeclared identifier %s",_tag_dyneither(_tmp634,sizeof(char),25))),_tag_dyneither(_tmp633,sizeof(void*),1)))))));}}else{_LL85: _tmpAD=_tmpAC;_LL86:(int)_rethrow(_tmpAD);}_LL82:;}};}case 1: _LL6B: _tmp8A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp85)->f1;_LL6C:
# 368
*q=*_tmp8A->name;
return _tmp8A->type;case 2: _LL6D: _tmp89=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp85)->f1;_LL6E:
*q=*_tmp89->name;return Cyc_Tcutil_fndecl2typ(_tmp89);case 5: _LL6F: _tmp88=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp85)->f1;_LL70:
 _tmp87=_tmp88;goto _LL72;case 4: _LL71: _tmp87=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp85)->f1;_LL72:
 _tmp86=_tmp87;goto _LL74;default: _LL73: _tmp86=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp85)->f1;_LL74:
# 374
{union Cyc_Absyn_Nmspace _tmp635;(*q).f1=(((_tmp635.Loc_n).val=0,(((_tmp635.Loc_n).tag=4,_tmp635))));}
if(te->allow_valueof){
void*_tmpB2=Cyc_Tcutil_compress(_tmp86->type);void*_tmpB3=_tmpB2;void*_tmpB4;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB3)->tag == 19){_LL88: _tmpB4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB3)->f1;_LL89:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp638;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp637;e->r=(void*)((_tmp637=_cycalloc(sizeof(*_tmp637)),((_tmp637[0]=((_tmp638.tag=38,((_tmp638.f1=_tmpB4,_tmp638)))),_tmp637))));}
goto _LL87;}else{_LL8A: _LL8B:
 goto _LL87;}_LL87:;}
# 383
return _tmp86->type;}_LL68:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_tmpB7=fmt->r;void*_tmpB8=_tmpB7;struct _dyneither_ptr _tmpB9;struct _dyneither_ptr _tmpBA;switch(*((int*)_tmpB8)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB8)->f1).String_c).tag == 8){_LL8D: _tmpBA=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB8)->f1).String_c).val;_LL8E:
 _tmpB9=_tmpBA;goto _LL90;}else{goto _LL91;}case 13: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB8)->f2)->r)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB8)->f2)->r)->f1).String_c).tag == 8){_LL8F: _tmpB9=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB8)->f2)->r)->f1).String_c).val;_LL90:
# 400
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpB9,fmt->loc);goto _LL8C;}else{goto _LL91;}}else{goto _LL91;}default: _LL91: _LL92:
# 404
 if(opt_args != 0){
struct Cyc_List_List*_tmpBB=(struct Cyc_List_List*)opt_args->v;
for(0;_tmpBB != 0;_tmpBB=_tmpBB->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpBB->hd);{
struct _RegionHandle _tmpBC=_new_region("temp");struct _RegionHandle*temp=& _tmpBC;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpBB->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpBB->hd)){
const char*_tmp63B;void*_tmp63A;(_tmp63A=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpBB->hd)->loc,((_tmp63B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp63B,sizeof(char),49))),_tag_dyneither(_tmp63A,sizeof(void*),0)));}
# 409
;_pop_region(temp);};}}
# 415
return;}_LL8C:;}
# 417
if(opt_args != 0){
struct Cyc_List_List*_tmpBF=(struct Cyc_List_List*)opt_args->v;
# 420
for(0;desc_types != 0  && _tmpBF != 0;
(((desc_types=desc_types->tl,_tmpBF=_tmpBF->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpBF->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
{const char*_tmp640;void*_tmp63F[2];struct Cyc_String_pa_PrintArg_struct _tmp63E;struct Cyc_String_pa_PrintArg_struct _tmp63D;(_tmp63D.tag=0,((_tmp63D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp63E.tag=0,((_tmp63E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp63F[0]=& _tmp63E,((_tmp63F[1]=& _tmp63D,Cyc_Tcutil_terr(e->loc,((_tmp640="descriptor has type %s but argument has type %s",_tag_dyneither(_tmp640,sizeof(char),48))),_tag_dyneither(_tmp63F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp641;*alias_arg_exps=((_tmp641=_cycalloc(sizeof(*_tmp641)),((_tmp641->hd=(void*)arg_cnt,((_tmp641->tl=*alias_arg_exps,_tmp641))))));}{
struct _RegionHandle _tmpC5=_new_region("temp");struct _RegionHandle*temp=& _tmpC5;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp644;void*_tmp643;(_tmp643=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpBF->hd)->loc,((_tmp644="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp644,sizeof(char),49))),_tag_dyneither(_tmp643,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp647;void*_tmp646;(_tmp646=0,Cyc_Tcutil_terr(fmt->loc,((_tmp647="too few arguments",_tag_dyneither(_tmp647,sizeof(char),18))),_tag_dyneither(_tmp646,sizeof(void*),0)));}
if(_tmpBF != 0){
{const char*_tmp64A;void*_tmp649;(_tmp649=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpBF->hd)->loc,((_tmp64A="too many arguments",_tag_dyneither(_tmp64A,sizeof(char),19))),_tag_dyneither(_tmp649,sizeof(void*),0)));}{
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp64D;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp64C;(int)_throw((void*)((_tmp64C=_cycalloc_atomic(sizeof(*_tmp64C)),((_tmp64C[0]=((_tmp64D.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp64D)),_tmp64C)))));};}}}
# 447
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 449
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmpCE=p;switch(_tmpCE){case Cyc_Absyn_Plus: _LL94: _LL95:
 goto _LL97;case Cyc_Absyn_Minus: _LL96: _LL97:
# 453
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp651;void*_tmp650[1];struct Cyc_String_pa_PrintArg_struct _tmp64F;(_tmp64F.tag=0,((_tmp64F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp650[0]=& _tmp64F,Cyc_Tcutil_terr(loc,((_tmp651="expecting numeric type but found %s",_tag_dyneither(_tmp651,sizeof(char),36))),_tag_dyneither(_tmp650,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL98: _LL99:
# 457
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp655;void*_tmp654[1];struct Cyc_String_pa_PrintArg_struct _tmp653;(_tmp653.tag=0,((_tmp653.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp654[0]=& _tmp653,Cyc_Tcutil_terr(loc,((_tmp655="expecting integral or * type but found %s",_tag_dyneither(_tmp655,sizeof(char),42))),_tag_dyneither(_tmp654,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL9A: _LL9B:
# 462
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp659;void*_tmp658[1];struct Cyc_String_pa_PrintArg_struct _tmp657;(_tmp657.tag=0,((_tmp657.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp658[0]=& _tmp657,Cyc_Tcutil_terr(loc,((_tmp659="expecting integral type but found %s",_tag_dyneither(_tmp659,sizeof(char),37))),_tag_dyneither(_tmp658,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9C: _LL9D:
# 466
{void*_tmpD8=t;union Cyc_Absyn_Constraint*_tmpD9;switch(*((int*)_tmpD8)){case 8: _LLA1: _LLA2:
 goto _LLA0;case 5: _LLA3: _tmpD9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD8)->f1).ptr_atts).bounds;_LLA4:
# 469
{void*_tmpDA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD9);void*_tmpDB=_tmpDA;struct Cyc_Absyn_Exp*_tmpDC;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpDB)->tag == 0){_LLA8: _LLA9:
 goto _LLA7;}else{_LLAA: _tmpDC=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDB)->f1;_LLAB:
# 472
 if(!Cyc_Evexp_c_can_eval(_tmpDC) && !((unsigned int)Cyc_Tcenv_allow_valueof)){
const char*_tmp65C;void*_tmp65B;(_tmp65B=0,Cyc_Tcutil_terr(loc,((_tmp65C="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp65C,sizeof(char),55))),_tag_dyneither(_tmp65B,sizeof(void*),0)));}
goto _LLA7;}_LLA7:;}
# 476
goto _LLA0;default: _LLA5: _LLA6: {
# 478
const char*_tmp660;void*_tmp65F[1];struct Cyc_String_pa_PrintArg_struct _tmp65E;(_tmp65E.tag=0,((_tmp65E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp65F[0]=& _tmp65E,Cyc_Tcutil_terr(loc,((_tmp660="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp660,sizeof(char),47))),_tag_dyneither(_tmp65F,sizeof(void*),1)))))));}}_LLA0:;}
# 480
return Cyc_Absyn_uint_typ;default: _LL9E: _LL9F: {
const char*_tmp663;void*_tmp662;(_tmp662=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp663="Non-unary primop",_tag_dyneither(_tmp663,sizeof(char),17))),_tag_dyneither(_tmp662,sizeof(void*),0)));}}_LL93:;}
# 486
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 489
if(!checker(e1)){
{const char*_tmp667;void*_tmp666[1];struct Cyc_String_pa_PrintArg_struct _tmp665;(_tmp665.tag=0,((_tmp665.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp666[0]=& _tmp665,Cyc_Tcutil_terr(e1->loc,((_tmp667="type %s cannot be used here",_tag_dyneither(_tmp667,sizeof(char),28))),_tag_dyneither(_tmp666,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 493
if(!checker(e2)){
{const char*_tmp66B;void*_tmp66A[1];struct Cyc_String_pa_PrintArg_struct _tmp669;(_tmp669.tag=0,((_tmp669.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp66A[0]=& _tmp669,Cyc_Tcutil_terr(e2->loc,((_tmp66B="type %s cannot be used here",_tag_dyneither(_tmp66B,sizeof(char),28))),_tag_dyneither(_tmp66A,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 497
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 502
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmpEA=t1;void*_tmpEB;struct Cyc_Absyn_Tqual _tmpEC;void*_tmpED;union Cyc_Absyn_Constraint*_tmpEE;union Cyc_Absyn_Constraint*_tmpEF;union Cyc_Absyn_Constraint*_tmpF0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA)->tag == 5){_LLAD: _tmpEB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA)->f1).elt_typ;_tmpEC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA)->f1).elt_tq;_tmpED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA)->f1).ptr_atts).rgn;_tmpEE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA)->f1).ptr_atts).nullable;_tmpEF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA)->f1).ptr_atts).bounds;_tmpF0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA)->f1).ptr_atts).zero_term;_LLAE:
# 507
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpEB),& Cyc_Tcutil_tmk)){
const char*_tmp66E;void*_tmp66D;(_tmp66D=0,Cyc_Tcutil_terr(e1->loc,((_tmp66E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp66E,sizeof(char),50))),_tag_dyneither(_tmp66D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp671;void*_tmp670;(_tmp670=0,Cyc_Tcutil_terr(e1->loc,((_tmp671="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp671,sizeof(char),54))),_tag_dyneither(_tmp670,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp675;void*_tmp674[1];struct Cyc_String_pa_PrintArg_struct _tmp673;(_tmp673.tag=0,((_tmp673.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp674[0]=& _tmp673,Cyc_Tcutil_terr(e2->loc,((_tmp675="expecting int but found %s",_tag_dyneither(_tmp675,sizeof(char),27))),_tag_dyneither(_tmp674,sizeof(void*),1)))))));}{
void*_tmpF8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpEF);void*_tmpF9=_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpF9)->tag == 0){_LLB2: _LLB3:
 return t1;}else{_LLB4: _tmpFA=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpF9)->f1;_LLB5:
# 518
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF0)){
const char*_tmp678;void*_tmp677;(_tmp677=0,Cyc_Tcutil_warn(e1->loc,((_tmp678="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp678,sizeof(char),70))),_tag_dyneither(_tmp677,sizeof(void*),0)));}{
# 526
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp67E;struct Cyc_Absyn_PtrInfo _tmp67D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFD=(_tmp67C=_cycalloc(sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67E.tag=5,((_tmp67E.f1=((_tmp67D.elt_typ=_tmpEB,((_tmp67D.elt_tq=_tmpEC,((_tmp67D.ptr_atts=(
((_tmp67D.ptr_atts).rgn=_tmpED,(((_tmp67D.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp67D.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp67D.ptr_atts).zero_term=_tmpF0,(((_tmp67D.ptr_atts).ptrloc=0,_tmp67D.ptr_atts)))))))))),_tmp67D)))))),_tmp67E)))),_tmp67C)));
# 530
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpFD,Cyc_Absyn_Other_coercion);
return(void*)_tmpFD;};}_LLB1:;};}else{_LLAF: _LLB0:
# 533
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LLAC:;}
# 538
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
{const char*_tmp683;void*_tmp682[2];struct Cyc_String_pa_PrintArg_struct _tmp681;struct Cyc_String_pa_PrintArg_struct _tmp680;(_tmp680.tag=0,((_tmp680.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_typ2string(t2)),((_tmp681.tag=0,((_tmp681.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp682[0]=& _tmp681,((_tmp682[1]=& _tmp680,Cyc_Tcutil_terr(e1->loc,((_tmp683="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp683,sizeof(char),59))),_tag_dyneither(_tmp682,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 551
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp688;void*_tmp687[2];struct Cyc_String_pa_PrintArg_struct _tmp686;struct Cyc_String_pa_PrintArg_struct _tmp685;(_tmp685.tag=0,((_tmp685.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp686.tag=0,((_tmp686.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp687[0]=& _tmp686,((_tmp687[1]=& _tmp685,Cyc_Tcutil_terr(e1->loc,((_tmp688="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp688,sizeof(char),59))),_tag_dyneither(_tmp687,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 560
{const char*_tmp68B;void*_tmp68A;(_tmp68A=0,Cyc_Tcutil_warn(e1->loc,((_tmp68B="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp68B,sizeof(char),60))),_tag_dyneither(_tmp68A,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 564
return Cyc_Absyn_sint_typ;}else{
# 566
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp68E;void*_tmp68D;(_tmp68D=0,Cyc_Tcutil_terr(e1->loc,((_tmp68E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp68E,sizeof(char),50))),_tag_dyneither(_tmp68D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp691;void*_tmp690;(_tmp690=0,Cyc_Tcutil_terr(e1->loc,((_tmp691="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp691,sizeof(char),54))),_tag_dyneither(_tmp690,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp696;void*_tmp695[2];struct Cyc_String_pa_PrintArg_struct _tmp694;struct Cyc_String_pa_PrintArg_struct _tmp693;(_tmp693.tag=0,((_tmp693.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp694.tag=0,((_tmp694.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp695[0]=& _tmp694,((_tmp695[1]=& _tmp693,Cyc_Tcutil_terr(e2->loc,((_tmp696="expecting either %s or int but found %s",_tag_dyneither(_tmp696,sizeof(char),40))),_tag_dyneither(_tmp695,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 575
return t1;}}}
# 579
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp699;void*_tmp698;(_tmp698=0,Cyc_Tcutil_warn(e1->loc,((_tmp699="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp699,sizeof(char),60))),_tag_dyneither(_tmp698,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 589
{const char*_tmp69C;void*_tmp69B;(_tmp69B=0,Cyc_Tcutil_warn(e1->loc,((_tmp69C="thin pointer subtraction!",_tag_dyneither(_tmp69C,sizeof(char),26))),_tag_dyneither(_tmp69B,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 592
{const char*_tmp69F;void*_tmp69E;(_tmp69E=0,Cyc_Tcutil_warn(e1->loc,((_tmp69F="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp69F,sizeof(char),56))),_tag_dyneither(_tmp69E,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 595
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp6A2;void*_tmp6A1;(_tmp6A1=0,Cyc_Tcutil_warn(e1->loc,((_tmp6A2="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp6A2,sizeof(char),51))),_tag_dyneither(_tmp6A1,sizeof(void*),0)));}
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
struct _tuple0 _tmp6A3;struct _tuple0 _tmp11B=(_tmp6A3.f1=Cyc_Tcutil_compress(t1),((_tmp6A3.f2=Cyc_Tcutil_compress(t2),_tmp6A3)));struct _tuple0 _tmp11C=_tmp11B;void*_tmp11D;void*_tmp11E;switch(*((int*)_tmp11C.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C.f2)->tag == 5){_LLB7: _tmp11D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C.f1)->f1).elt_typ;_tmp11E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C.f2)->f1).elt_typ;_LLB8:
# 633
 if(Cyc_Tcutil_unify(_tmp11D,_tmp11E))return Cyc_Absyn_sint_typ;goto _LLB6;}else{goto _LLBB;}case 15: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp11C.f2)->tag == 15){_LLB9: _LLBA:
# 635
 return Cyc_Absyn_sint_typ;}else{goto _LLBB;}default: _LLBB: _LLBC:
 goto _LLB6;}_LLB6:;}
# 638
{const char*_tmp6A8;void*_tmp6A7[2];struct Cyc_String_pa_PrintArg_struct _tmp6A6;struct Cyc_String_pa_PrintArg_struct _tmp6A5;(_tmp6A5.tag=0,((_tmp6A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp6A6.tag=0,((_tmp6A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp6A7[0]=& _tmp6A6,((_tmp6A7[1]=& _tmp6A5,Cyc_Tcutil_terr(loc,((_tmp6A8="comparison not allowed between %s and %s",_tag_dyneither(_tmp6A8,sizeof(char),41))),_tag_dyneither(_tmp6A7,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp6A9;struct _tuple0 _tmp124=(_tmp6A9.f1=t1,((_tmp6A9.f2=t2,_tmp6A9)));struct _tuple0 _tmp125=_tmp124;void*_tmp126;void*_tmp127;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp125.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp125.f2)->tag == 15){_LLBE: _tmp126=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp125.f1)->f1;_tmp127=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp125.f2)->f1;_LLBF:
# 658
 return Cyc_Absyn_sint_typ;}else{goto _LLC0;}}else{_LLC0: _LLC1:
 goto _LLBD;}_LLBD:;}
# 661
{const char*_tmp6AE;void*_tmp6AD[2];struct Cyc_String_pa_PrintArg_struct _tmp6AC;struct Cyc_String_pa_PrintArg_struct _tmp6AB;(_tmp6AB.tag=0,((_tmp6AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp6AC.tag=0,((_tmp6AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp6AD[0]=& _tmp6AC,((_tmp6AD[1]=& _tmp6AB,Cyc_Tcutil_terr(loc,((_tmp6AE="comparison not allowed between %s and %s",_tag_dyneither(_tmp6AE,sizeof(char),41))),_tag_dyneither(_tmp6AD,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 669
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 671
enum Cyc_Absyn_Primop _tmp12D=p;switch(_tmp12D){case Cyc_Absyn_Plus: _LLC3: _LLC4:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLC5: _LLC6:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLC7: _LLC8:
# 675
 goto _LLCA;case Cyc_Absyn_Div: _LLC9: _LLCA:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLCB: _LLCC:
# 678
 goto _LLCE;case Cyc_Absyn_Bitand: _LLCD: _LLCE:
 goto _LLD0;case Cyc_Absyn_Bitor: _LLCF: _LLD0:
 goto _LLD2;case Cyc_Absyn_Bitxor: _LLD1: _LLD2:
 goto _LLD4;case Cyc_Absyn_Bitlshift: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_Bitlrshift: _LLD5: _LLD6:
 goto _LLD8;case Cyc_Absyn_Bitarshift: _LLD7: _LLD8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLD9: _LLDA:
# 688
 goto _LLDC;case Cyc_Absyn_Neq: _LLDB: _LLDC:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLDD: _LLDE:
# 691
 goto _LLE0;case Cyc_Absyn_Lt: _LLDF: _LLE0:
 goto _LLE2;case Cyc_Absyn_Gte: _LLE1: _LLE2:
 goto _LLE4;case Cyc_Absyn_Lte: _LLE3: _LLE4:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLE5: _LLE6: {
# 696
const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6B1="bad binary primop",_tag_dyneither(_tmp6B1,sizeof(char),18))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));}}_LLC2:;}
# 700
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 708
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp130=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmp131=Cyc_Tcexp_tcExp(te,topt,_tmp130);
if(!Cyc_Tcutil_is_numeric(_tmp130)){
const char*_tmp6B5;void*_tmp6B4[1];struct Cyc_String_pa_PrintArg_struct _tmp6B3;(_tmp6B3.tag=0,((_tmp6B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp131)),((_tmp6B4[0]=& _tmp6B3,Cyc_Tcutil_terr(_tmp130->loc,((_tmp6B5="expecting numeric type but found %s",_tag_dyneither(_tmp6B5,sizeof(char),36))),_tag_dyneither(_tmp6B4,sizeof(void*),1)))))));}
return _tmp131;}
# 715
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmp135=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmp136=_tmp135;switch(_tmp136){case 0: _LLE8: _LLE9: {
const char*_tmp6B8;void*_tmp6B7;return(_tmp6B7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6B8="primitive operator has 0 arguments",_tag_dyneither(_tmp6B8,sizeof(char),35))),_tag_dyneither(_tmp6B7,sizeof(void*),0)));}case 1: _LLEA: _LLEB:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LLE7;case 2: _LLEC: _LLED:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);goto _LLE7;default: _LLEE: _LLEF: {
const char*_tmp6BB;void*_tmp6BA;return(_tmp6BA=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6BB="primitive operator has > 2 arguments",_tag_dyneither(_tmp6BB,sizeof(char),37))),_tag_dyneither(_tmp6BA,sizeof(void*),0)));}}_LLE7:;}
# 723
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 726
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp13B=Cyc_Tcutil_compress(t);
void*_tmp13C=_tmp13B;struct Cyc_List_List*_tmp13D;void*_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;struct Cyc_Absyn_Datatypefield*_tmp140;struct Cyc_List_List*_tmp141;struct Cyc_Absyn_Aggrdecl*_tmp142;switch(*((int*)_tmp13C)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13C)->f1).aggr_info).KnownAggr).tag == 2){_LLF1: _tmp142=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13C)->f1).aggr_info).KnownAggr).val;_LLF2:
# 730
 if(_tmp142->impl == 0){
{const char*_tmp6BE;void*_tmp6BD;(_tmp6BD=0,Cyc_Tcutil_terr(loc,((_tmp6BE="attempt to write an abstract aggregate",_tag_dyneither(_tmp6BE,sizeof(char),39))),_tag_dyneither(_tmp6BD,sizeof(void*),0)));}
return 0;}else{
# 734
_tmp141=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp142->impl))->fields;goto _LLF4;}}else{goto _LLFB;}case 12: _LLF3: _tmp141=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13C)->f2;_LLF4:
# 736
 for(0;_tmp141 != 0;_tmp141=_tmp141->tl){
struct Cyc_Absyn_Aggrfield*_tmp145=(struct Cyc_Absyn_Aggrfield*)_tmp141->hd;
if((_tmp145->tq).real_const){
{const char*_tmp6C2;void*_tmp6C1[1];struct Cyc_String_pa_PrintArg_struct _tmp6C0;(_tmp6C0.tag=0,((_tmp6C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp145->name),((_tmp6C1[0]=& _tmp6C0,Cyc_Tcutil_terr(loc,((_tmp6C2="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp6C2,sizeof(char),56))),_tag_dyneither(_tmp6C1,sizeof(void*),1)))))));}
return 0;}
# 742
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp145->type))return 0;}
# 744
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp13C)->f1).field_info).KnownDatatypefield).tag == 2){_LLF5: _tmp140=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp13C)->f1).field_info).KnownDatatypefield).val).f2;_LLF6:
# 746
{struct Cyc_List_List*_tmp149=_tmp140->typs;for(0;_tmp149 != 0;_tmp149=_tmp149->tl){
struct _tuple18*_tmp14A=(struct _tuple18*)_tmp149->hd;struct _tuple18*_tmp14B=_tmp14A;struct Cyc_Absyn_Tqual _tmp14C;void*_tmp14D;_LLFE: _tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;_LLFF:;
if(_tmp14C.real_const){
{const char*_tmp6C6;void*_tmp6C5[1];struct Cyc_String_pa_PrintArg_struct _tmp6C4;(_tmp6C4.tag=0,((_tmp6C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp140->name)),((_tmp6C5[0]=& _tmp6C4,Cyc_Tcutil_terr(loc,((_tmp6C6="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp6C6,sizeof(char),64))),_tag_dyneither(_tmp6C5,sizeof(void*),1)))))));}
return 0;}
# 752
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp14D))return 0;}}
# 754
return 1;}else{goto _LLFB;}case 8: _LLF7: _tmp13E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->f1).elt_type;_tmp13F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->f1).tq;_LLF8:
# 756
 if(_tmp13F.real_const){
{const char*_tmp6C9;void*_tmp6C8;(_tmp6C8=0,Cyc_Tcutil_terr(loc,((_tmp6C9="attempt to over-write a const array",_tag_dyneither(_tmp6C9,sizeof(char),36))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}
return 0;}
# 760
return Cyc_Tcexp_check_writable_aggr(loc,_tmp13E);case 10: _LLF9: _tmp13D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13C)->f1;_LLFA:
# 762
 for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
struct _tuple18*_tmp153=(struct _tuple18*)_tmp13D->hd;struct _tuple18*_tmp154=_tmp153;struct Cyc_Absyn_Tqual _tmp155;void*_tmp156;_LL101: _tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_LL102:;
if(_tmp155.real_const){
{const char*_tmp6CC;void*_tmp6CB;(_tmp6CB=0,Cyc_Tcutil_terr(loc,((_tmp6CC="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp6CC,sizeof(char),56))),_tag_dyneither(_tmp6CB,sizeof(void*),0)));}
return 0;}
# 768
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp156))return 0;}
# 770
return 1;default: _LLFB: _LLFC:
 return 1;}_LLF0:;}
# 778
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 781
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp159=e->r;void*_tmp15A=_tmp159;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15E;struct _dyneither_ptr*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;struct _dyneither_ptr*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Vardecl*_tmp164;struct Cyc_Absyn_Vardecl*_tmp165;struct Cyc_Absyn_Vardecl*_tmp166;struct Cyc_Absyn_Vardecl*_tmp167;switch(*((int*)_tmp15A)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15A)->f2)){case 3: _LL104: _tmp167=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15A)->f2)->f1;_LL105:
 _tmp166=_tmp167;goto _LL107;case 4: _LL106: _tmp166=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15A)->f2)->f1;_LL107:
 _tmp165=_tmp166;goto _LL109;case 5: _LL108: _tmp165=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15A)->f2)->f1;_LL109:
 _tmp164=_tmp165;goto _LL10B;case 1: _LL10A: _tmp164=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15A)->f2)->f1;_LL10B:
 if(!(_tmp164->tq).real_const)return;goto _LL103;default: goto _LL118;}case 22: _LL10C: _tmp162=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp163=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL10D:
# 788
{void*_tmp168=Cyc_Tcutil_compress((void*)_check_null(_tmp162->topt));void*_tmp169=_tmp168;struct Cyc_List_List*_tmp16A;struct Cyc_Absyn_Tqual _tmp16B;struct Cyc_Absyn_Tqual _tmp16C;switch(*((int*)_tmp169)){case 5: _LL11B: _tmp16C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp169)->f1).elt_tq;_LL11C:
 _tmp16B=_tmp16C;goto _LL11E;case 8: _LL11D: _tmp16B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp169)->f1).tq;_LL11E:
 if(!_tmp16B.real_const)return;goto _LL11A;case 10: _LL11F: _tmp16A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp169)->f1;_LL120: {
# 792
struct _tuple15 _tmp16D=Cyc_Evexp_eval_const_uint_exp(_tmp163);struct _tuple15 _tmp16E=_tmp16D;unsigned int _tmp16F;int _tmp170;_LL124: _tmp16F=_tmp16E.f1;_tmp170=_tmp16E.f2;_LL125:;
if(!_tmp170){
{const char*_tmp6CF;void*_tmp6CE;(_tmp6CE=0,Cyc_Tcutil_terr(e->loc,((_tmp6CF="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp6CF,sizeof(char),47))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}
return;}
# 797
{struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
{struct _tuple18*_tmp176=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp16A,(int)_tmp16F);struct _tuple18*_tmp177=_tmp176;struct Cyc_Absyn_Tqual _tmp178;_LL127: _tmp178=_tmp177->f1;_LL128:;
if(!_tmp178.real_const){_npop_handler(0);return;}}
# 798
;_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp179=_tmp174;void*_tmp17A;if(((struct Cyc_List_Nth_exn_struct*)_tmp179)->tag == Cyc_List_Nth){_LL12A: _LL12B:
# 800
 return;}else{_LL12C: _tmp17A=_tmp179;_LL12D:(int)_rethrow(_tmp17A);}_LL129:;}};}
goto _LL11A;}default: _LL121: _LL122:
 goto _LL11A;}_LL11A:;}
# 804
goto _LL103;case 20: _LL10E: _tmp160=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp161=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL10F:
# 806
{void*_tmp17B=Cyc_Tcutil_compress((void*)_check_null(_tmp160->topt));void*_tmp17C=_tmp17B;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Aggrdecl**_tmp17E;switch(*((int*)_tmp17C)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp17C)->f1).aggr_info).KnownAggr).tag == 2){_LL12F: _tmp17E=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp17C)->f1).aggr_info).KnownAggr).val;_LL130: {
# 808
struct Cyc_Absyn_Aggrfield*sf=
_tmp17E == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp17E,_tmp161);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL12E;}}else{goto _LL133;}case 12: _LL131: _tmp17D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17C)->f2;_LL132: {
# 813
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp17D,_tmp161);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL12E;}default: _LL133: _LL134:
 goto _LL12E;}_LL12E:;}
# 818
goto _LL103;case 21: _LL110: _tmp15E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp15F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL111:
# 820
{void*_tmp17F=Cyc_Tcutil_compress((void*)_check_null(_tmp15E->topt));void*_tmp180=_tmp17F;void*_tmp181;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp180)->tag == 5){_LL136: _tmp181=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp180)->f1).elt_typ;_LL137:
# 822
{void*_tmp182=Cyc_Tcutil_compress(_tmp181);void*_tmp183=_tmp182;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Aggrdecl**_tmp185;switch(*((int*)_tmp183)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp183)->f1).aggr_info).KnownAggr).tag == 2){_LL13B: _tmp185=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp183)->f1).aggr_info).KnownAggr).val;_LL13C: {
# 824
struct Cyc_Absyn_Aggrfield*sf=
_tmp185 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp185,_tmp15F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL13A;}}else{goto _LL13F;}case 12: _LL13D: _tmp184=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp183)->f2;_LL13E: {
# 829
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp184,_tmp15F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL13A;}default: _LL13F: _LL140:
 goto _LL13A;}_LL13A:;}
# 834
goto _LL135;}else{_LL138: _LL139:
 goto _LL135;}_LL135:;}
# 837
goto _LL103;case 19: _LL112: _tmp15D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL113:
# 839
{void*_tmp186=Cyc_Tcutil_compress((void*)_check_null(_tmp15D->topt));void*_tmp187=_tmp186;struct Cyc_Absyn_Tqual _tmp188;struct Cyc_Absyn_Tqual _tmp189;switch(*((int*)_tmp187)){case 5: _LL142: _tmp189=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->f1).elt_tq;_LL143:
 _tmp188=_tmp189;goto _LL145;case 8: _LL144: _tmp188=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp187)->f1).tq;_LL145:
 if(!_tmp188.real_const)return;goto _LL141;default: _LL146: _LL147:
 goto _LL141;}_LL141:;}
# 844
goto _LL103;case 11: _LL114: _tmp15C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL115:
 _tmp15B=_tmp15C;goto _LL117;case 12: _LL116: _tmp15B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL117:
 Cyc_Tcexp_check_writable(te,_tmp15B);return;default: _LL118: _LL119:
 goto _LL103;}_LL103:;}{
# 849
const char*_tmp6D3;void*_tmp6D2[1];struct Cyc_String_pa_PrintArg_struct _tmp6D1;(_tmp6D1.tag=0,((_tmp6D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp6D2[0]=& _tmp6D1,Cyc_Tcutil_terr(e->loc,((_tmp6D3="attempt to write a const location: %s",_tag_dyneither(_tmp6D3,sizeof(char),38))),_tag_dyneither(_tmp6D2,sizeof(void*),1)))))));};}
# 852
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 855
struct _RegionHandle _tmp18D=_new_region("temp");struct _RegionHandle*temp=& _tmp18D;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 858
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp6D6;void*_tmp6D5;void*_tmp190=(_tmp6D5=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D6="increment/decrement of non-lvalue",_tag_dyneither(_tmp6D6,sizeof(char),34))),_tag_dyneither(_tmp6D5,sizeof(void*),0)));_npop_handler(0);return _tmp190;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 863
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp6D9;void*_tmp6D8;(_tmp6D8=0,Cyc_Tcutil_terr(e->loc,((_tmp6D9="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp6D9,sizeof(char),50))),_tag_dyneither(_tmp6D8,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp6DC;void*_tmp6DB;(_tmp6DB=0,Cyc_Tcutil_terr(e->loc,((_tmp6DC="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp6DC,sizeof(char),54))),_tag_dyneither(_tmp6DB,sizeof(void*),0)));}}else{
# 872
const char*_tmp6E0;void*_tmp6DF[1];struct Cyc_String_pa_PrintArg_struct _tmp6DE;(_tmp6DE.tag=0,((_tmp6DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6DF[0]=& _tmp6DE,Cyc_Tcutil_terr(e->loc,((_tmp6E0="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp6E0,sizeof(char),44))),_tag_dyneither(_tmp6DF,sizeof(void*),1)))))));}}{
# 874
void*_tmp198=t;_npop_handler(0);return _tmp198;};};
# 855
;_pop_region(temp);}
# 878
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 880
struct _RegionHandle _tmp199=_new_region("r");struct _RegionHandle*r=& _tmp199;_push_region(r);{
const char*_tmp6E1;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp6E1="conditional expression",_tag_dyneither(_tmp6E1,sizeof(char),23))));}
# 883
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 889
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp6E2;struct Cyc_List_List _tmp19B=(_tmp6E2.hd=e3,((_tmp6E2.tl=0,_tmp6E2)));
struct Cyc_List_List _tmp6E3;struct Cyc_List_List _tmp19C=(_tmp6E3.hd=e2,((_tmp6E3.tl=& _tmp19B,_tmp6E3)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp19C)){
{const char*_tmp6E8;void*_tmp6E7[2];struct Cyc_String_pa_PrintArg_struct _tmp6E6;struct Cyc_String_pa_PrintArg_struct _tmp6E5;(_tmp6E5.tag=0,((_tmp6E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp6E7[0]=& _tmp6E6,((_tmp6E7[1]=& _tmp6E5,Cyc_Tcutil_terr(loc,((_tmp6E8="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp6E8,sizeof(char),48))),_tag_dyneither(_tmp6E7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 897
void*_tmp1A1=t;_npop_handler(0);return _tmp1A1;};};};
# 880
;_pop_region(r);}
# 901
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 903
{const char*_tmp6E9;Cyc_Tcexp_tcTest(te,e1,((_tmp6E9="logical-and expression",_tag_dyneither(_tmp6E9,sizeof(char),23))));}
{const char*_tmp6EA;Cyc_Tcexp_tcTest(te,e2,((_tmp6EA="logical-and expression",_tag_dyneither(_tmp6EA,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 909
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 911
{const char*_tmp6EB;Cyc_Tcexp_tcTest(te,e1,((_tmp6EB="logical-or expression",_tag_dyneither(_tmp6EB,sizeof(char),22))));}
{const char*_tmp6EC;Cyc_Tcexp_tcTest(te,e2,((_tmp6EC="logical-or expression",_tag_dyneither(_tmp6EC,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 917
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 924
struct _RegionHandle _tmp1A8=_new_region("r");struct _RegionHandle*r=& _tmp1A8;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 927
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 931
{void*_tmp1A9=Cyc_Tcutil_compress(t1);void*_tmp1AA=_tmp1A9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AA)->tag == 8){_LL149: _LL14A:
{const char*_tmp6EF;void*_tmp6EE;(_tmp6EE=0,Cyc_Tcutil_terr(loc,((_tmp6EF="cannot assign to an array",_tag_dyneither(_tmp6EF,sizeof(char),26))),_tag_dyneither(_tmp6EE,sizeof(void*),0)));}goto _LL148;}else{_LL14B: _LL14C:
 goto _LL148;}_LL148:;}
# 936
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp6F2;void*_tmp6F1;(_tmp6F1=0,Cyc_Tcutil_terr(loc,((_tmp6F2="type is abstract (can't determine size).",_tag_dyneither(_tmp6F2,sizeof(char),41))),_tag_dyneither(_tmp6F1,sizeof(void*),0)));}
# 940
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp6F5;void*_tmp6F4;void*_tmp1B1=(_tmp6F4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6F5="assignment to non-lvalue",_tag_dyneither(_tmp6F5,sizeof(char),25))),_tag_dyneither(_tmp6F4,sizeof(void*),0)));_npop_handler(0);return _tmp1B1;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1B2=_new_region("temp");struct _RegionHandle*temp=& _tmp1B2;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp6F8;void*_tmp6F7;(_tmp6F7=0,Cyc_Tcutil_terr(e2->loc,((_tmp6F8="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6F8,sizeof(char),49))),_tag_dyneither(_tmp6F7,sizeof(void*),0)));}
# 948
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp6FD;void*_tmp6FC[2];struct Cyc_String_pa_PrintArg_struct _tmp6FB;struct Cyc_String_pa_PrintArg_struct _tmp6FA;void*_tmp1B5=(_tmp6FA.tag=0,((_tmp6FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp6FB.tag=0,((_tmp6FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp6FC[0]=& _tmp6FB,((_tmp6FC[1]=& _tmp6FA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6FD="type mismatch: %s != %s",_tag_dyneither(_tmp6FD,sizeof(char),24))),_tag_dyneither(_tmp6FC,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1B6=_tmp1B5;_npop_handler(1);return _tmp1B6;};}
# 944
;_pop_region(temp);}else{
# 956
enum Cyc_Absyn_Primop _tmp1BB=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1BC=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1BB,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1BC,t1) || Cyc_Tcutil_coerceable(_tmp1BC) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp702;void*_tmp701[2];struct Cyc_String_pa_PrintArg_struct _tmp700;struct Cyc_String_pa_PrintArg_struct _tmp6FF;void*_tmp1BD=
(_tmp6FF.tag=0,((_tmp6FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 962
Cyc_Absynpp_typ2string(t2)),((_tmp700.tag=0,((_tmp700.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 961
Cyc_Absynpp_typ2string(t1)),((_tmp701[0]=& _tmp700,((_tmp701[1]=& _tmp6FF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp702="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp702,sizeof(char),82))),_tag_dyneither(_tmp701,sizeof(void*),2)))))))))))));
# 963
Cyc_Tcutil_unify(_tmp1BC,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp1BE=_tmp1BD;_npop_handler(0);return _tmp1BE;};}{
# 967
void*_tmp1C3=_tmp1BC;_npop_handler(0);return _tmp1C3;};}{
# 969
void*_tmp1C4=t1;_npop_handler(0);return _tmp1C4;};};};
# 924
;_pop_region(r);}
# 973
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _RegionHandle _tmp1C5=_new_region("r");struct _RegionHandle*r=& _tmp1C5;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);{
# 978
void*_tmp1C6=(void*)_check_null(e2->topt);_npop_handler(0);return _tmp1C6;};
# 974
;_pop_region(r);}
# 982
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 986
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 989
{void*_tmp1C7=Cyc_Tcutil_compress(t1);void*_tmp1C8=_tmp1C7;switch(*((int*)_tmp1C8)){case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1C8)->f1 == 0){_LL14E: _LL14F:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL14D;}else{goto _LL154;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1C8)->f2){case Cyc_Absyn_Char_sz: _LL150: _LL151:
 goto _LL153;case Cyc_Absyn_Short_sz: _LL152: _LL153:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL14D;default: goto _LL154;}default: _LL154: _LL155:
 goto _LL14D;}_LL14D:;}
# 996
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp1C9=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp1CA=_tmp1C9;struct _tuple1*_tmp1CB;struct Cyc_List_List*_tmp1CC;unsigned int _tmp1CD;enum Cyc_Absyn_Scope _tmp1CE;_LL157: _tmp1CB=_tmp1CA.name;_tmp1CC=_tmp1CA.typs;_tmp1CD=_tmp1CA.loc;_tmp1CE=_tmp1CA.sc;_LL158:;
# 999
if(_tmp1CC == 0  || _tmp1CC->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp1CC->hd)).f2);
# 1002
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1007
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp1CF=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp1D0=_tmp1CF;struct _tuple1*_tmp1D1;struct Cyc_List_List*_tmp1D2;unsigned int _tmp1D3;enum Cyc_Absyn_Scope _tmp1D4;_LL15A: _tmp1D1=_tmp1D0.name;_tmp1D2=_tmp1D0.typs;_tmp1D3=_tmp1D0.loc;_tmp1D4=_tmp1D0.sc;_LL15B:;
# 1010
if(_tmp1D2 == 0  || _tmp1D2->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp1D2->hd)).f2);
# 1014
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1019
{const char*_tmp707;void*_tmp706[2];struct Cyc_String_pa_PrintArg_struct _tmp705;struct Cyc_String_pa_PrintArg_struct _tmp704;(_tmp704.tag=0,((_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp705.tag=0,((_tmp705.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp706[0]=& _tmp705,((_tmp706[1]=& _tmp704,Cyc_Tcutil_terr(e->loc,((_tmp707="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp707,sizeof(char),54))),_tag_dyneither(_tmp706,sizeof(void*),2)))))))))))));}
return 0;}
# 1025
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1031
struct Cyc_List_List*_tmp1D9=args;
int _tmp1DA=0;
struct _RegionHandle _tmp1DB=_new_region("ter");struct _RegionHandle*ter=& _tmp1DB;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp1DC=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp1DD=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp1DC);
Cyc_Tcexp_tcExp(_tmp1DD,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1041
void*_tmp1DE=t;void*_tmp1DF;struct Cyc_Absyn_Tqual _tmp1E0;void*_tmp1E1;union Cyc_Absyn_Constraint*_tmp1E2;union Cyc_Absyn_Constraint*_tmp1E3;union Cyc_Absyn_Constraint*_tmp1E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->tag == 5){_LL15D: _tmp1DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->f1).elt_typ;_tmp1E0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->f1).elt_tq;_tmp1E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->f1).ptr_atts).rgn;_tmp1E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->f1).ptr_atts).nullable;_tmp1E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->f1).ptr_atts).bounds;_tmp1E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DE)->f1).ptr_atts).zero_term;_LL15E:
# 1046
 Cyc_Tcenv_check_rgn_accessible(_tmp1DD,loc,_tmp1E1);
# 1048
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1E3);{
void*_tmp1E5=Cyc_Tcutil_compress(_tmp1DF);void*_tmp1E6=_tmp1E5;struct Cyc_List_List*_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Tqual _tmp1E9;void*_tmp1EA;struct Cyc_List_List*_tmp1EB;int _tmp1EC;struct Cyc_Absyn_VarargInfo*_tmp1ED;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*_tmp1F3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->tag == 9){_LL162: _tmp1E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).tvars;_tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).effect;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).ret_tqual;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).ret_typ;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).args;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).c_varargs;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).cyc_varargs;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).rgn_po;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).attributes;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).requires_clause;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).requires_relns;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).ensures_clause;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E6)->f1).ensures_relns;_LL163:
# 1053
 if(_tmp1E7 != 0  || _tmp1EE != 0){
const char*_tmp70A;void*_tmp709;(_tmp709=0,Cyc_Tcutil_terr(e->loc,((_tmp70A="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp70A,sizeof(char),79))),_tag_dyneither(_tmp709,sizeof(void*),0)));}
# 1057
if(topt != 0)Cyc_Tcutil_unify(_tmp1EA,*topt);
# 1059
while(_tmp1D9 != 0  && _tmp1EB != 0){
# 1061
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1D9->hd;
void*t2=(*((struct _tuple9*)_tmp1EB->hd)).f3;
Cyc_Tcexp_tcExp(_tmp1DD,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp1DD,e1,t2,& alias_coercion)){
const char*_tmp70B;struct _dyneither_ptr s0=(_tmp70B="actual argument has type ",_tag_dyneither(_tmp70B,sizeof(char),26));
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
const char*_tmp70C;struct _dyneither_ptr s2=(_tmp70C=" but formal has type ",_tag_dyneither(_tmp70C,sizeof(char),22));
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70){
struct Cyc_String_pa_PrintArg_struct _tmp71A;struct Cyc_String_pa_PrintArg_struct _tmp719;struct Cyc_String_pa_PrintArg_struct _tmp718;struct Cyc_String_pa_PrintArg_struct _tmp717;void*_tmp716[4];const char*_tmp715;void*_tmp714;(_tmp714=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp71A.tag=0,((_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp719.tag=0,((_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp718.tag=0,((_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp717.tag=0,((_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp716[0]=& _tmp717,((_tmp716[1]=& _tmp718,((_tmp716[2]=& _tmp719,((_tmp716[3]=& _tmp71A,Cyc_aprintf(((_tmp715="%s\n\t%s\n%s\n\t%s.",_tag_dyneither(_tmp715,sizeof(char),15))),_tag_dyneither(_tmp716,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp714,sizeof(void*),0)));}else{
# 1073
struct Cyc_String_pa_PrintArg_struct _tmp728;struct Cyc_String_pa_PrintArg_struct _tmp727;struct Cyc_String_pa_PrintArg_struct _tmp726;struct Cyc_String_pa_PrintArg_struct _tmp725;void*_tmp724[4];const char*_tmp723;void*_tmp722;(_tmp722=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp728.tag=0,((_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp727.tag=0,((_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp726.tag=0,((_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp725.tag=0,((_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp724[0]=& _tmp725,((_tmp724[1]=& _tmp726,((_tmp724[2]=& _tmp727,((_tmp724[3]=& _tmp728,Cyc_aprintf(((_tmp723="%s%s%s%s.",_tag_dyneither(_tmp723,sizeof(char),10))),_tag_dyneither(_tmp724,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp722,sizeof(void*),0)));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1078
if(alias_coercion){
struct Cyc_List_List*_tmp729;*alias_arg_exps=((_tmp729=_cycalloc(sizeof(*_tmp729)),((_tmp729->hd=(void*)_tmp1DA,((_tmp729->tl=*alias_arg_exps,_tmp729))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp72C;void*_tmp72B;(_tmp72B=0,Cyc_Tcutil_terr(e1->loc,((_tmp72C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp72C,sizeof(char),49))),_tag_dyneither(_tmp72B,sizeof(void*),0)));}
_tmp1D9=_tmp1D9->tl;
_tmp1EB=_tmp1EB->tl;
++ _tmp1DA;}{
# 1089
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1EF;for(0;a != 0;a=a->tl){
void*_tmp209=(void*)a->hd;void*_tmp20A=_tmp209;enum Cyc_Absyn_Format_Type _tmp20B;int _tmp20C;int _tmp20D;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp20A)->tag == 19){_LL167: _tmp20B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp20A)->f1;_tmp20C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp20A)->f2;_tmp20D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp20A)->f3;_LL168:
# 1093
{struct _handler_cons _tmp20E;_push_handler(& _tmp20E);{int _tmp210=0;if(setjmp(_tmp20E.handler))_tmp210=1;if(!_tmp210){
# 1095
{struct Cyc_Absyn_Exp*_tmp211=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp20C - 1);
# 1097
struct Cyc_Core_Opt*fmt_args;
if(_tmp20D == 0)
fmt_args=0;else{
# 1101
struct Cyc_Core_Opt*_tmp72D;fmt_args=((_tmp72D=_cycalloc(sizeof(*_tmp72D)),((_tmp72D->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp20D - 1),_tmp72D))));}
args_already_checked=1;{
struct _RegionHandle _tmp213=_new_region("temp");struct _RegionHandle*temp=& _tmp213;_push_region(temp);
{enum Cyc_Absyn_Format_Type _tmp214=_tmp20B;if(_tmp214 == Cyc_Absyn_Printf_ft){_LL16C: _LL16D:
# 1106
 Cyc_Tcexp_check_format_args(_tmp1DD,_tmp211,fmt_args,_tmp20D - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1109
goto _LL16B;}else{_LL16E: _LL16F:
# 1111
 Cyc_Tcexp_check_format_args(_tmp1DD,_tmp211,fmt_args,_tmp20D - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1114
goto _LL16B;}_LL16B:;}
# 1104
;_pop_region(temp);};}
# 1095
;_pop_handler();}else{void*_tmp20F=(void*)_exn_thrown;void*_tmp215=_tmp20F;void*_tmp216;if(((struct Cyc_List_Nth_exn_struct*)_tmp215)->tag == Cyc_List_Nth){_LL171: _LL172:
# 1118
{const char*_tmp730;void*_tmp72F;(_tmp72F=0,Cyc_Tcutil_terr(loc,((_tmp730="bad format arguments",_tag_dyneither(_tmp730,sizeof(char),21))),_tag_dyneither(_tmp72F,sizeof(void*),0)));}goto _LL170;}else{_LL173: _tmp216=_tmp215;_LL174:(int)_rethrow(_tmp216);}_LL170:;}};}
# 1120
goto _LL166;}else{_LL169: _LL16A:
 goto _LL166;}_LL166:;}}
# 1124
if(_tmp1EB != 0){const char*_tmp733;void*_tmp732;(_tmp732=0,Cyc_Tcutil_terr(loc,((_tmp733="too few arguments for function",_tag_dyneither(_tmp733,sizeof(char),31))),_tag_dyneither(_tmp732,sizeof(void*),0)));}else{
# 1126
if((_tmp1D9 != 0  || _tmp1EC) || _tmp1ED != 0){
if(_tmp1EC)
for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){
Cyc_Tcexp_tcExp(_tmp1DD,0,(struct Cyc_Absyn_Exp*)_tmp1D9->hd);}else{
if(_tmp1ED == 0){
const char*_tmp736;void*_tmp735;(_tmp735=0,Cyc_Tcutil_terr(loc,((_tmp736="too many arguments for function",_tag_dyneither(_tmp736,sizeof(char),32))),_tag_dyneither(_tmp735,sizeof(void*),0)));}else{
# 1133
struct Cyc_Absyn_VarargInfo _tmp21D=*_tmp1ED;struct Cyc_Absyn_VarargInfo _tmp21E=_tmp21D;void*_tmp21F;int _tmp220;_LL176: _tmp21F=_tmp21E.type;_tmp220=_tmp21E.inject;_LL177:;{
struct Cyc_Absyn_VarargCallInfo*_tmp737;struct Cyc_Absyn_VarargCallInfo*_tmp221=(_tmp737=_cycalloc(sizeof(*_tmp737)),((_tmp737->num_varargs=0,((_tmp737->injectors=0,((_tmp737->vai=_tmp1ED,_tmp737)))))));
# 1137
*vararg_call_info=_tmp221;
# 1139
if(!_tmp220)
# 1141
for(0;_tmp1D9 != 0;(_tmp1D9=_tmp1D9->tl,_tmp1DA ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1D9->hd;
++ _tmp221->num_varargs;
Cyc_Tcexp_tcExp(_tmp1DD,& _tmp21F,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp1DD,e1,_tmp21F,& alias_coercion)){
{const char*_tmp73C;void*_tmp73B[2];struct Cyc_String_pa_PrintArg_struct _tmp73A;struct Cyc_String_pa_PrintArg_struct _tmp739;(_tmp739.tag=0,((_tmp739.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp73A.tag=0,((_tmp73A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21F)),((_tmp73B[0]=& _tmp73A,((_tmp73B[1]=& _tmp739,Cyc_Tcutil_terr(loc,((_tmp73C="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp73C,sizeof(char),49))),_tag_dyneither(_tmp73B,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1151
if(alias_coercion){
struct Cyc_List_List*_tmp73D;*alias_arg_exps=((_tmp73D=_cycalloc(sizeof(*_tmp73D)),((_tmp73D->hd=(void*)_tmp1DA,((_tmp73D->tl=*alias_arg_exps,_tmp73D))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp21F) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp740;void*_tmp73F;(_tmp73F=0,Cyc_Tcutil_terr(e1->loc,((_tmp740="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp740,sizeof(char),49))),_tag_dyneither(_tmp73F,sizeof(void*),0)));}}else{
# 1160
void*_tmp229=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp21F));void*_tmp22A=_tmp229;struct Cyc_Absyn_Datatypedecl*_tmp22B;struct Cyc_List_List*_tmp22C;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp22A)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp22A)->f1).datatype_info).KnownDatatype).tag == 2){_LL179: _tmp22B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp22A)->f1).datatype_info).KnownDatatype).val;_tmp22C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp22A)->f1).targs;_LL17A: {
# 1164
struct Cyc_Absyn_Datatypedecl*_tmp22D=*Cyc_Tcenv_lookup_datatypedecl(_tmp1DD,loc,_tmp22B->name);
struct Cyc_List_List*fields=0;
if(_tmp22D->fields == 0){
const char*_tmp744;void*_tmp743[1];struct Cyc_String_pa_PrintArg_struct _tmp742;(_tmp742.tag=0,((_tmp742.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp21F)),((_tmp743[0]=& _tmp742,Cyc_Tcutil_terr(loc,((_tmp744="can't inject into abstract datatype %s",_tag_dyneither(_tmp744,sizeof(char),39))),_tag_dyneither(_tmp743,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22D->fields))->v;}
# 1176
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp21F),Cyc_Tcenv_curr_rgn(_tmp1DD))){
const char*_tmp749;void*_tmp748[2];struct Cyc_String_pa_PrintArg_struct _tmp747;struct Cyc_String_pa_PrintArg_struct _tmp746;(_tmp746.tag=0,((_tmp746.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp1DD))),((_tmp747.tag=0,((_tmp747.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21F)),((_tmp748[0]=& _tmp747,((_tmp748[1]=& _tmp746,Cyc_Tcutil_terr(loc,((_tmp749="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp749,sizeof(char),49))),_tag_dyneither(_tmp748,sizeof(void*),2)))))))))))));}{
# 1180
struct _RegionHandle _tmp235=_new_region("rgn");struct _RegionHandle*rgn=& _tmp235;_push_region(rgn);{
struct Cyc_List_List*_tmp236=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp22D->tvs,_tmp22C);
for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){
++ _tmp221->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1D9->hd;
# 1186
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp1DD,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp74C;void*_tmp74B;(_tmp74B=0,Cyc_Tcutil_terr(e1->loc,((_tmp74C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp74C,sizeof(char),49))),_tag_dyneither(_tmp74B,sizeof(void*),0)));}}{
# 1192
struct Cyc_Absyn_Datatypefield*_tmp239=Cyc_Tcexp_tcInjection(_tmp1DD,e1,Cyc_Tcutil_pointer_elt_type(_tmp21F),rgn,_tmp236,fields);
# 1194
if(_tmp239 != 0){
struct Cyc_List_List*_tmp74D;_tmp221->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp221->injectors,(
(_tmp74D=_cycalloc(sizeof(*_tmp74D)),((_tmp74D->hd=_tmp239,((_tmp74D->tl=0,_tmp74D)))))));}};};}}
# 1200
_npop_handler(0);goto _LL178;
# 1180
;_pop_region(rgn);};}}else{goto _LL17B;}}else{_LL17B: _LL17C:
# 1201
{const char*_tmp750;void*_tmp74F;(_tmp74F=0,Cyc_Tcutil_terr(loc,((_tmp750="bad inject vararg type",_tag_dyneither(_tmp750,sizeof(char),23))),_tag_dyneither(_tmp74F,sizeof(void*),0)));}goto _LL178;}_LL178:;}};}}}}
# 1206
if(*alias_arg_exps == 0)
# 1215 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp1DD,loc,(void*)_check_null(_tmp1E8));{
# 1217
void*_tmp23E=_tmp1EA;_npop_handler(0);return _tmp23E;};};}else{_LL164: _LL165: {
const char*_tmp754;void*_tmp753[1];struct Cyc_String_pa_PrintArg_struct _tmp752;void*_tmp242=(_tmp752.tag=0,((_tmp752.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp753[0]=& _tmp752,Cyc_Tcexp_expr_err(_tmp1DD,loc,topt,((_tmp754="expected pointer to function but found %s",_tag_dyneither(_tmp754,sizeof(char),42))),_tag_dyneither(_tmp753,sizeof(void*),1)))))));_npop_handler(0);return _tmp242;}}_LL161:;};}else{_LL15F: _LL160: {
# 1220
const char*_tmp758;void*_tmp757[1];struct Cyc_String_pa_PrintArg_struct _tmp756;void*_tmp246=(_tmp756.tag=0,((_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp757[0]=& _tmp756,Cyc_Tcexp_expr_err(_tmp1DD,loc,topt,((_tmp758="expected pointer to function but found %s",_tag_dyneither(_tmp758,sizeof(char),42))),_tag_dyneither(_tmp757,sizeof(void*),1)))))));_npop_handler(0);return _tmp246;}}_LL15C:;};}
# 1034 "tcexp.cyc"
;_pop_region(ter);}
# 1226 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp247=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp247,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp247,& bogus)){
const char*_tmp75D;void*_tmp75C[2];struct Cyc_String_pa_PrintArg_struct _tmp75B;struct Cyc_String_pa_PrintArg_struct _tmp75A;(_tmp75A.tag=0,((_tmp75A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp75B.tag=0,((_tmp75B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1231
Cyc_Absynpp_typ2string(_tmp247)),((_tmp75C[0]=& _tmp75B,((_tmp75C[1]=& _tmp75A,Cyc_Tcutil_terr(loc,((_tmp75D="expected %s but found %s",_tag_dyneither(_tmp75D,sizeof(char),25))),_tag_dyneither(_tmp75C,sizeof(void*),2)))))))))))));}
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
{void*_tmp24C=t1;void*_tmp24D;struct Cyc_Absyn_Tqual _tmp24E;void*_tmp24F;union Cyc_Absyn_Constraint*_tmp250;union Cyc_Absyn_Constraint*_tmp251;union Cyc_Absyn_Constraint*_tmp252;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->tag == 5){_LL17E: _tmp24D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).elt_typ;_tmp24E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).elt_tq;_tmp24F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).ptr_atts).rgn;_tmp250=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).ptr_atts).nullable;_tmp251=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).ptr_atts).bounds;_tmp252=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C)->f1).ptr_atts).zero_term;_LL17F:
# 1246
{void*_tmp253=Cyc_Tcutil_compress(_tmp24D);void*_tmp254=_tmp253;struct Cyc_List_List*_tmp255;void*_tmp256;struct Cyc_Absyn_Tqual _tmp257;void*_tmp258;struct Cyc_List_List*_tmp259;int _tmp25A;struct Cyc_Absyn_VarargInfo*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_List_List*_tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_List_List*_tmp261;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->tag == 9){_LL183: _tmp255=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).tvars;_tmp256=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).effect;_tmp257=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).ret_tqual;_tmp258=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).ret_typ;_tmp259=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).args;_tmp25A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).c_varargs;_tmp25B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).cyc_varargs;_tmp25C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).rgn_po;_tmp25D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).attributes;_tmp25E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).requires_clause;_tmp25F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).requires_relns;_tmp260=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).ensures_clause;_tmp261=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp254)->f1).ensures_relns;_LL184: {
# 1248
struct _RegionHandle _tmp262=_new_region("temp");struct _RegionHandle*temp=& _tmp262;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1251
for(0;ts != 0  && _tmp255 != 0;(ts=ts->tl,_tmp255=_tmp255->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp255->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp760;struct Cyc_List_List*_tmp75F;instantiation=(
(_tmp75F=_region_malloc(temp,sizeof(*_tmp75F)),((_tmp75F->hd=((_tmp760=_region_malloc(temp,sizeof(*_tmp760)),((_tmp760->f1=(struct Cyc_Absyn_Tvar*)_tmp255->hd,((_tmp760->f2=(void*)ts->hd,_tmp760)))))),((_tmp75F->tl=instantiation,_tmp75F))))));};}
# 1258
if(ts != 0){
const char*_tmp763;void*_tmp762;void*_tmp267=
(_tmp762=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp763="too many type variables in instantiation",_tag_dyneither(_tmp763,sizeof(char),41))),_tag_dyneither(_tmp762,sizeof(void*),0)));_npop_handler(0);return _tmp267;}
# 1264
if(_tmp255 == 0){
_tmp25C=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp25C);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp25C);
_tmp25C=0;}{
# 1269
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp769;struct Cyc_Absyn_FnInfo _tmp768;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp767;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767[0]=((_tmp769.tag=9,((_tmp769.f1=((_tmp768.tvars=_tmp255,((_tmp768.effect=_tmp256,((_tmp768.ret_tqual=_tmp257,((_tmp768.ret_typ=_tmp258,((_tmp768.args=_tmp259,((_tmp768.c_varargs=_tmp25A,((_tmp768.cyc_varargs=_tmp25B,((_tmp768.rgn_po=_tmp25C,((_tmp768.attributes=_tmp25D,((_tmp768.requires_clause=_tmp25E,((_tmp768.requires_relns=_tmp25F,((_tmp768.ensures_clause=_tmp260,((_tmp768.ensures_relns=_tmp261,_tmp768)))))))))))))))))))))))))),_tmp769)))),_tmp767)))));
# 1274
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp76F;struct Cyc_Absyn_PtrInfo _tmp76E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp76D;void*_tmp26B=(void*)((_tmp76D=_cycalloc(sizeof(*_tmp76D)),((_tmp76D[0]=((_tmp76F.tag=5,((_tmp76F.f1=((_tmp76E.elt_typ=new_fn_typ,((_tmp76E.elt_tq=_tmp24E,((_tmp76E.ptr_atts=(((_tmp76E.ptr_atts).rgn=_tmp24F,(((_tmp76E.ptr_atts).nullable=_tmp250,(((_tmp76E.ptr_atts).bounds=_tmp251,(((_tmp76E.ptr_atts).zero_term=_tmp252,(((_tmp76E.ptr_atts).ptrloc=0,_tmp76E.ptr_atts)))))))))),_tmp76E)))))),_tmp76F)))),_tmp76D))));_npop_handler(0);return _tmp26B;};}
# 1249
;_pop_region(temp);}}else{_LL185: _LL186:
# 1276
 goto _LL182;}_LL182:;}
# 1278
goto _LL17D;}else{_LL180: _LL181:
 goto _LL17D;}_LL17D:;}{
# 1281
const char*_tmp773;void*_tmp772[1];struct Cyc_String_pa_PrintArg_struct _tmp771;return(_tmp771.tag=0,((_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp772[0]=& _tmp771,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp773="expecting polymorphic type but found %s",_tag_dyneither(_tmp773,sizeof(char),40))),_tag_dyneither(_tmp772,sizeof(void*),1)))))));};};}
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
const char*_tmp778;void*_tmp777[2];struct Cyc_String_pa_PrintArg_struct _tmp776;struct Cyc_String_pa_PrintArg_struct _tmp775;void*_tmp272=(_tmp775.tag=0,((_tmp775.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp776.tag=0,((_tmp776.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp777[0]=& _tmp776,((_tmp777[1]=& _tmp775,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp778="cannot cast %s to %s",_tag_dyneither(_tmp778,sizeof(char),21))),_tag_dyneither(_tmp777,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp272;};}}}
# 1314
{struct _tuple0 _tmp779;struct _tuple0 _tmp277=(_tmp779.f1=e->r,((_tmp779.f2=Cyc_Tcutil_compress(t),_tmp779)));struct _tuple0 _tmp278=_tmp277;int _tmp279;union Cyc_Absyn_Constraint*_tmp27A;union Cyc_Absyn_Constraint*_tmp27B;union Cyc_Absyn_Constraint*_tmp27C;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp278.f1)->tag == 33){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp278.f2)->tag == 5){_LL188: _tmp279=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp278.f1)->f1).fat_result;_tmp27A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp278.f2)->f1).ptr_atts).nullable;_tmp27B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp278.f2)->f1).ptr_atts).bounds;_tmp27C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp278.f2)->f1).ptr_atts).zero_term;_LL189:
# 1318
 if((_tmp279  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp27C)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp27A)){
void*_tmp27D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp27B);void*_tmp27E=_tmp27D;struct Cyc_Absyn_Exp*_tmp27F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp27E)->tag == 1){_LL18D: _tmp27F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp27E)->f1;_LL18E:
# 1321
 if((Cyc_Evexp_eval_const_uint_exp(_tmp27F)).f1 == 1){
const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_warn(loc,((_tmp77C="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp77C,sizeof(char),60))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}
goto _LL18C;}else{_LL18F: _LL190:
 goto _LL18C;}_LL18C:;}
# 1327
goto _LL187;}else{goto _LL18A;}}else{_LL18A: _LL18B:
 goto _LL187;}_LL187:;}
# 1330
return t;};}
# 1334
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1336
void**_tmp283=0;
struct Cyc_Absyn_Tqual _tmp284=Cyc_Absyn_empty_tqual(0);
int _tmp285=0;
if(topt != 0){
void*_tmp286=Cyc_Tcutil_compress(*topt);void*_tmp287=_tmp286;void**_tmp288;struct Cyc_Absyn_Tqual _tmp289;union Cyc_Absyn_Constraint*_tmp28A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp287)->tag == 5){_LL192: _tmp288=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp287)->f1).elt_typ;_tmp289=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp287)->f1).elt_tq;_tmp28A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp287)->f1).ptr_atts).nullable;_LL193:
# 1342
 _tmp283=_tmp288;
_tmp284=_tmp289;
_tmp285=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp28A);
goto _LL191;}else{_LL194: _LL195:
 goto _LL191;}_LL191:;}{
# 1356
struct _RegionHandle _tmp28B=_new_region("r");struct _RegionHandle*r=& _tmp28B;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp283,e);
# 1360
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp77F;void*_tmp77E;(_tmp77E=0,Cyc_Tcutil_terr(e->loc,((_tmp77F="Cannot take the address of an alias-free path",_tag_dyneither(_tmp77F,sizeof(char),46))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}
# 1367
{void*_tmp28E=e->r;void*_tmp28F=_tmp28E;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp291;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28F)->tag == 22){_LL197: _tmp290=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28F)->f1;_tmp291=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28F)->f2;_LL198:
# 1369
{void*_tmp292=Cyc_Tcutil_compress((void*)_check_null(_tmp290->topt));void*_tmp293=_tmp292;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp293)->tag == 10){_LL19C: _LL19D:
 goto _LL19B;}else{_LL19E: _LL19F:
# 1374
 e0->r=(Cyc_Absyn_add_exp(_tmp290,_tmp291,0))->r;{
void*_tmp294=Cyc_Tcexp_tcPlus(te,_tmp290,_tmp291);_npop_handler(0);return _tmp294;};}_LL19B:;}
# 1377
goto _LL196;}else{_LL199: _LL19A:
 goto _LL196;}_LL196:;}
# 1382
{void*_tmp295=e->r;void*_tmp296=_tmp295;switch(*((int*)_tmp296)){case 20: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp296)->f3 == 1){_LL1A1: _LL1A2:
 goto _LL1A4;}else{goto _LL1A5;}case 21: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp296)->f3 == 1){_LL1A3: _LL1A4:
# 1385
{const char*_tmp782;void*_tmp781;(_tmp781=0,Cyc_Tcutil_terr(e->loc,((_tmp782="cannot take the address of a @tagged union member",_tag_dyneither(_tmp782,sizeof(char),50))),_tag_dyneither(_tmp781,sizeof(void*),0)));}
goto _LL1A0;}else{goto _LL1A5;}default: _LL1A5: _LL1A6:
 goto _LL1A0;}_LL1A0:;}{
# 1391
struct _tuple14 _tmp299=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp29A=_tmp299;int _tmp29B;void*_tmp29C;_LL1A8: _tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;_LL1A9:;
# 1393
if(Cyc_Tcutil_is_noalias_region(_tmp29C,0)){
const char*_tmp785;void*_tmp784;(_tmp784=0,Cyc_Tcutil_terr(e->loc,((_tmp785="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp785,sizeof(char),60))),_tag_dyneither(_tmp784,sizeof(void*),0)));}{
# 1396
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp29B){
tq.print_const=1;
tq.real_const=1;}{
# 1402
void*t=_tmp285?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp29C,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp29C,tq,Cyc_Absyn_false_conref);
void*_tmp29F=t;_npop_handler(0);return _tmp29F;};};};
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
const char*_tmp789;void*_tmp788[1];struct Cyc_String_pa_PrintArg_struct _tmp787;(_tmp787.tag=0,((_tmp787.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp788[0]=& _tmp787,Cyc_Tcutil_terr(loc,((_tmp789="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp789,sizeof(char),55))),_tag_dyneither(_tmp788,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp2A3=Cyc_Tcutil_compress(*topt);void*_tmp2A4=_tmp2A3;void*_tmp2A5;void*_tmp2A6;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2A4)->tag == 19){_LL1AB: _tmp2A5=_tmp2A4;_tmp2A6=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2A4)->f1;_LL1AC: {
# 1424
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp78C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp78B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2A8=(_tmp78B=_cycalloc(sizeof(*_tmp78B)),((_tmp78B[0]=((_tmp78C.tag=18,((_tmp78C.f1=_tmp2A7,_tmp78C)))),_tmp78B)));
if(Cyc_Tcutil_unify(_tmp2A6,(void*)_tmp2A8))return _tmp2A5;
goto _LL1AA;}}else{_LL1AD: _LL1AE:
 goto _LL1AA;}_LL1AA:;}
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
struct Cyc_List_List*_tmp2AB=fs;
void*_tmp2AC=t_orig;
for(0;_tmp2AB != 0;_tmp2AB=_tmp2AB->tl){
void*_tmp2AD=(void*)_tmp2AB->hd;
void*_tmp2AE=_tmp2AD;unsigned int _tmp2AF;struct _dyneither_ptr*_tmp2B0;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2AE)->tag == 0){_LL1B0: _tmp2B0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2AE)->f1;_LL1B1: {
# 1450
int bad_type=1;
{void*_tmp2B1=Cyc_Tcutil_compress(_tmp2AC);void*_tmp2B2=_tmp2B1;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_Aggrdecl**_tmp2B4;switch(*((int*)_tmp2B2)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B2)->f1).aggr_info).KnownAggr).tag == 2){_LL1B5: _tmp2B4=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B2)->f1).aggr_info).KnownAggr).val;_LL1B6:
# 1453
 if((*_tmp2B4)->impl == 0)goto _LL1B4;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2B0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2B4)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp790;void*_tmp78F[1];struct Cyc_String_pa_PrintArg_struct _tmp78E;(_tmp78E.tag=0,((_tmp78E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B0),((_tmp78F[0]=& _tmp78E,Cyc_Tcutil_terr(loc,((_tmp790="no field of struct/union has name %s",_tag_dyneither(_tmp790,sizeof(char),37))),_tag_dyneither(_tmp78F,sizeof(void*),1)))))));}else{
# 1458
_tmp2AC=t2;}
bad_type=0;
goto _LL1B4;};}else{goto _LL1B9;}case 12: _LL1B7: _tmp2B3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B2)->f2;_LL1B8: {
# 1462
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2B0,_tmp2B3);
if(!((unsigned int)t2)){
const char*_tmp794;void*_tmp793[1];struct Cyc_String_pa_PrintArg_struct _tmp792;(_tmp792.tag=0,((_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B0),((_tmp793[0]=& _tmp792,Cyc_Tcutil_terr(loc,((_tmp794="no field of struct/union has name %s",_tag_dyneither(_tmp794,sizeof(char),37))),_tag_dyneither(_tmp793,sizeof(void*),1)))))));}else{
# 1466
_tmp2AC=t2;}
bad_type=0;
goto _LL1B4;}default: _LL1B9: _LL1BA:
 goto _LL1B4;}_LL1B4:;}
# 1471
if(bad_type){
if(_tmp2AB == fs){
const char*_tmp798;void*_tmp797[1];struct Cyc_String_pa_PrintArg_struct _tmp796;(_tmp796.tag=0,((_tmp796.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2AC)),((_tmp797[0]=& _tmp796,Cyc_Tcutil_terr(loc,((_tmp798="%s is not a known struct/union type",_tag_dyneither(_tmp798,sizeof(char),36))),_tag_dyneither(_tmp797,sizeof(void*),1)))))));}else{
# 1475
const char*_tmp79C;void*_tmp79B[1];struct Cyc_String_pa_PrintArg_struct _tmp79A;struct _dyneither_ptr _tmp2BE=(_tmp79A.tag=0,((_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp79B[0]=& _tmp79A,Cyc_aprintf(((_tmp79C="(%s)",_tag_dyneither(_tmp79C,sizeof(char),5))),_tag_dyneither(_tmp79B,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2AB;x=x->tl){
void*_tmp2BF=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp2C0=_tmp2BF;unsigned int _tmp2C1;struct _dyneither_ptr*_tmp2C2;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2C0)->tag == 0){_LL1BC: _tmp2C2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2C0)->f1;_LL1BD:
# 1480
{const char*_tmp7A1;void*_tmp7A0[2];struct Cyc_String_pa_PrintArg_struct _tmp79F;struct Cyc_String_pa_PrintArg_struct _tmp79E;_tmp2BE=((_tmp79E.tag=0,((_tmp79E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2C2),((_tmp79F.tag=0,((_tmp79F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BE),((_tmp7A0[0]=& _tmp79F,((_tmp7A0[1]=& _tmp79E,Cyc_aprintf(((_tmp7A1="%s.%s",_tag_dyneither(_tmp7A1,sizeof(char),6))),_tag_dyneither(_tmp7A0,sizeof(void*),2))))))))))))));}goto _LL1BB;}else{_LL1BE: _tmp2C1=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2C0)->f1;_LL1BF:
# 1482
{const char*_tmp7A6;void*_tmp7A5[2];struct Cyc_String_pa_PrintArg_struct _tmp7A4;struct Cyc_Int_pa_PrintArg_struct _tmp7A3;_tmp2BE=((_tmp7A3.tag=1,((_tmp7A3.f1=(unsigned long)((int)_tmp2C1),((_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BE),((_tmp7A5[0]=& _tmp7A4,((_tmp7A5[1]=& _tmp7A3,Cyc_aprintf(((_tmp7A6="%s.%d",_tag_dyneither(_tmp7A6,sizeof(char),6))),_tag_dyneither(_tmp7A5,sizeof(void*),2))))))))))))));}goto _LL1BB;}_LL1BB:;}{
# 1484
const char*_tmp7AB;void*_tmp7AA[2];struct Cyc_String_pa_PrintArg_struct _tmp7A9;struct Cyc_String_pa_PrintArg_struct _tmp7A8;(_tmp7A8.tag=0,((_tmp7A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2AC)),((_tmp7A9.tag=0,((_tmp7A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BE),((_tmp7AA[0]=& _tmp7A9,((_tmp7AA[1]=& _tmp7A8,Cyc_Tcutil_terr(loc,((_tmp7AB="%s == %s is not a struct/union type",_tag_dyneither(_tmp7AB,sizeof(char),36))),_tag_dyneither(_tmp7AA,sizeof(void*),2)))))))))))));};}}
# 1487
goto _LL1AF;}}else{_LL1B2: _tmp2AF=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2AE)->f1;_LL1B3: {
# 1489
int bad_type=1;
{void*_tmp2D2=Cyc_Tcutil_compress(_tmp2AC);void*_tmp2D3=_tmp2D2;struct Cyc_Absyn_Datatypefield*_tmp2D4;struct Cyc_List_List*_tmp2D5;struct Cyc_List_List*_tmp2D6;struct Cyc_Absyn_Aggrdecl**_tmp2D7;switch(*((int*)_tmp2D3)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D3)->f1).aggr_info).KnownAggr).tag == 2){_LL1C1: _tmp2D7=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D3)->f1).aggr_info).KnownAggr).val;_LL1C2:
# 1492
 if((*_tmp2D7)->impl == 0)
goto _LL1C0;
_tmp2D6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2D7)->impl))->fields;goto _LL1C4;}else{goto _LL1C9;}case 12: _LL1C3: _tmp2D6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3)->f2;_LL1C4:
# 1496
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D6)<= _tmp2AF){
const char*_tmp7B0;void*_tmp7AF[2];struct Cyc_Int_pa_PrintArg_struct _tmp7AE;struct Cyc_Int_pa_PrintArg_struct _tmp7AD;(_tmp7AD.tag=1,((_tmp7AD.f1=(unsigned long)((int)_tmp2AF),((_tmp7AE.tag=1,((_tmp7AE.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D6),((_tmp7AF[0]=& _tmp7AE,((_tmp7AF[1]=& _tmp7AD,Cyc_Tcutil_terr(loc,((_tmp7B0="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp7B0,sizeof(char),46))),_tag_dyneither(_tmp7AF,sizeof(void*),2)))))))))))));}else{
# 1500
_tmp2AC=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2D6,(int)_tmp2AF))->type;}
bad_type=0;
goto _LL1C0;case 10: _LL1C5: _tmp2D5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3)->f1;_LL1C6:
# 1504
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D5)<= _tmp2AF){
const char*_tmp7B5;void*_tmp7B4[2];struct Cyc_Int_pa_PrintArg_struct _tmp7B3;struct Cyc_Int_pa_PrintArg_struct _tmp7B2;(_tmp7B2.tag=1,((_tmp7B2.f1=(unsigned long)((int)_tmp2AF),((_tmp7B3.tag=1,((_tmp7B3.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D5),((_tmp7B4[0]=& _tmp7B3,((_tmp7B4[1]=& _tmp7B2,Cyc_Tcutil_terr(loc,((_tmp7B5="tuple has too few components: %d <= %d",_tag_dyneither(_tmp7B5,sizeof(char),39))),_tag_dyneither(_tmp7B4,sizeof(void*),2)))))))))))));}else{
# 1508
_tmp2AC=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2D5,(int)_tmp2AF)).f2;}
bad_type=0;
goto _LL1C0;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D3)->f1).field_info).KnownDatatypefield).tag == 2){_LL1C7: _tmp2D4=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D3)->f1).field_info).KnownDatatypefield).val).f2;_LL1C8:
# 1512
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D4->typs)< _tmp2AF){
const char*_tmp7BA;void*_tmp7B9[2];struct Cyc_Int_pa_PrintArg_struct _tmp7B8;struct Cyc_Int_pa_PrintArg_struct _tmp7B7;(_tmp7B7.tag=1,((_tmp7B7.f1=(unsigned long)((int)_tmp2AF),((_tmp7B8.tag=1,((_tmp7B8.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D4->typs),((_tmp7B9[0]=& _tmp7B8,((_tmp7B9[1]=& _tmp7B7,Cyc_Tcutil_terr(loc,((_tmp7BA="datatype field has too few components: %d < %d",_tag_dyneither(_tmp7BA,sizeof(char),47))),_tag_dyneither(_tmp7B9,sizeof(void*),2)))))))))))));}else{
# 1516
if(_tmp2AF != 0)
_tmp2AC=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2D4->typs,(int)(_tmp2AF - 1))).f2;else{
if(_tmp2AB->tl != 0){
const char*_tmp7BD;void*_tmp7BC;(_tmp7BC=0,Cyc_Tcutil_terr(loc,((_tmp7BD="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp7BD,sizeof(char),68))),_tag_dyneither(_tmp7BC,sizeof(void*),0)));}}}
# 1521
bad_type=0;
goto _LL1C0;}else{goto _LL1C9;}default: _LL1C9: _LL1CA:
 goto _LL1C0;}_LL1C0:;}
# 1525
if(bad_type){
const char*_tmp7C1;void*_tmp7C0[1];struct Cyc_String_pa_PrintArg_struct _tmp7BF;(_tmp7BF.tag=0,((_tmp7BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2AC)),((_tmp7C0[0]=& _tmp7BF,Cyc_Tcutil_terr(loc,((_tmp7C1="%s is not a known type",_tag_dyneither(_tmp7C1,sizeof(char),23))),_tag_dyneither(_tmp7C0,sizeof(void*),1)))))));}
goto _LL1AF;}}_LL1AF:;}
# 1530
return Cyc_Absyn_uint_typ;};}
# 1534
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp2E9=_new_region("r");struct _RegionHandle*r=& _tmp2E9;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp2EA=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp2EA,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp2EB=t;void*_tmp2EC;void*_tmp2ED;union Cyc_Absyn_Constraint*_tmp2EE;union Cyc_Absyn_Constraint*_tmp2EF;switch(*((int*)_tmp2EB)){case 1: _LL1CC: _LL1CD: {
# 1541
struct Cyc_List_List*_tmp2F0=Cyc_Tcenv_lookup_type_vars(_tmp2EA);
struct Cyc_Core_Opt*_tmp7C2;void*_tmp2F1=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp7C2=_cycalloc(sizeof(*_tmp7C2)),((_tmp7C2->v=_tmp2F0,_tmp7C2)))));
struct Cyc_Core_Opt*_tmp7C3;void*_tmp2F2=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp7C3=_cycalloc(sizeof(*_tmp7C3)),((_tmp7C3->v=_tmp2F0,_tmp7C3)))));
union Cyc_Absyn_Constraint*_tmp2F3=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp2F4=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp7C4;struct Cyc_Absyn_PtrAtts _tmp2F5=(_tmp7C4.rgn=_tmp2F2,((_tmp7C4.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp7C4.bounds=_tmp2F3,((_tmp7C4.zero_term=_tmp2F4,((_tmp7C4.ptrloc=0,_tmp7C4)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7CA;struct Cyc_Absyn_PtrInfo _tmp7C9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7C8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F6=(_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7CA.tag=5,((_tmp7CA.f1=((_tmp7C9.elt_typ=_tmp2F1,((_tmp7C9.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp7C9.ptr_atts=_tmp2F5,_tmp7C9)))))),_tmp7CA)))),_tmp7C8)));
Cyc_Tcutil_unify(t,(void*)_tmp2F6);
_tmp2EC=_tmp2F1;_tmp2ED=_tmp2F2;_tmp2EE=_tmp2F3;_tmp2EF=_tmp2F4;goto _LL1CF;}case 5: _LL1CE: _tmp2EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EB)->f1).elt_typ;_tmp2ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EB)->f1).ptr_atts).rgn;_tmp2EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EB)->f1).ptr_atts).bounds;_tmp2EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EB)->f1).ptr_atts).zero_term;_LL1CF:
# 1551
 Cyc_Tcenv_check_rgn_accessible(_tmp2EA,loc,_tmp2ED);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2EE);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2EC),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2EA)){
const char*_tmp7CD;void*_tmp7CC;(_tmp7CC=0,Cyc_Tcutil_terr(loc,((_tmp7CD="can't dereference abstract pointer type",_tag_dyneither(_tmp7CD,sizeof(char),40))),_tag_dyneither(_tmp7CC,sizeof(void*),0)));}{
void*_tmp2FF=_tmp2EC;_npop_handler(0);return _tmp2FF;};default: _LL1D0: _LL1D1: {
# 1557
const char*_tmp7D1;void*_tmp7D0[1];struct Cyc_String_pa_PrintArg_struct _tmp7CF;void*_tmp303=(_tmp7CF.tag=0,((_tmp7CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7D0[0]=& _tmp7CF,Cyc_Tcexp_expr_err(_tmp2EA,loc,topt,((_tmp7D1="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp7D1,sizeof(char),39))),_tag_dyneither(_tmp7D0,sizeof(void*),1)))))));_npop_handler(0);return _tmp303;}}_LL1CB:;};}
# 1536
;_pop_region(r);}
# 1563
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1567
struct _RegionHandle _tmp304=_new_region("r");struct _RegionHandle*r=& _tmp304;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1571
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp305=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp306=_tmp305;enum Cyc_Absyn_AggrKind _tmp307;struct Cyc_List_List*_tmp308;struct Cyc_Absyn_Aggrdecl*_tmp309;struct Cyc_List_List*_tmp30A;switch(*((int*)_tmp306)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp306)->f1).aggr_info).KnownAggr).tag == 2){_LL1D3: _tmp309=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp306)->f1).aggr_info).KnownAggr).val;_tmp30A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp306)->f1).targs;_LL1D4: {
# 1574
struct Cyc_Absyn_Aggrfield*_tmp30B=Cyc_Absyn_lookup_decl_field(_tmp309,f);
if(_tmp30B == 0){
const char*_tmp7D6;void*_tmp7D5[2];struct Cyc_String_pa_PrintArg_struct _tmp7D4;struct Cyc_String_pa_PrintArg_struct _tmp7D3;void*_tmp310=(_tmp7D3.tag=0,((_tmp7D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7D4.tag=0,((_tmp7D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp309->name)),((_tmp7D5[0]=& _tmp7D4,((_tmp7D5[1]=& _tmp7D3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D6="%s has no %s member",_tag_dyneither(_tmp7D6,sizeof(char),20))),_tag_dyneither(_tmp7D5,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp310;}
# 1579
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp309->impl))->tagged)*is_tagged=1;{
void*t2=_tmp30B->type;
if(_tmp30A != 0){
struct _RegionHandle _tmp311=_new_region("rgn");struct _RegionHandle*rgn=& _tmp311;_push_region(rgn);
{struct Cyc_List_List*_tmp312=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp309->tvs,_tmp30A);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp312,_tmp30B->type);}
# 1583
;_pop_region(rgn);}
# 1589
if((((_tmp309->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp309->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp30B->requires_clause == 0){
const char*_tmp7DA;void*_tmp7D9[1];struct Cyc_String_pa_PrintArg_struct _tmp7D8;void*_tmp316=(_tmp7D8.tag=0,((_tmp7D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7D9[0]=& _tmp7D8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DA="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp7DA,sizeof(char),56))),_tag_dyneither(_tmp7D9,sizeof(void*),1)))))));_npop_handler(0);return _tmp316;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp309->impl))->exist_vars != 0){
# 1594
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp7DE;void*_tmp7DD[1];struct Cyc_String_pa_PrintArg_struct _tmp7DC;void*_tmp31A=(_tmp7DC.tag=0,((_tmp7DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7DD[0]=& _tmp7DC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DE="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp7DE,sizeof(char),81))),_tag_dyneither(_tmp7DD,sizeof(void*),1)))))));_npop_handler(0);return _tmp31A;}}{
# 1597
void*_tmp31B=t2;_npop_handler(0);return _tmp31B;};};}}else{goto _LL1D7;}case 12: _LL1D5: _tmp307=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp306)->f1;_tmp308=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp306)->f2;_LL1D6: {
# 1599
struct Cyc_Absyn_Aggrfield*_tmp31C=Cyc_Absyn_lookup_field(_tmp308,f);
if(_tmp31C == 0){
const char*_tmp7E2;void*_tmp7E1[1];struct Cyc_String_pa_PrintArg_struct _tmp7E0;void*_tmp320=(_tmp7E0.tag=0,((_tmp7E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7E1[0]=& _tmp7E0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E2="type has no %s member",_tag_dyneither(_tmp7E2,sizeof(char),22))),_tag_dyneither(_tmp7E1,sizeof(void*),1)))))));_npop_handler(0);return _tmp320;}
# 1604
if(((_tmp307 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp31C->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp31C->requires_clause == 0){
# 1606
const char*_tmp7E6;void*_tmp7E5[1];struct Cyc_String_pa_PrintArg_struct _tmp7E4;void*_tmp324=(_tmp7E4.tag=0,((_tmp7E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7E5[0]=& _tmp7E4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp7E6,sizeof(char),56))),_tag_dyneither(_tmp7E5,sizeof(void*),1)))))));_npop_handler(0);return _tmp324;}{
void*_tmp325=_tmp31C->type;_npop_handler(0);return _tmp325;};}default: _LL1D7: _LL1D8: {
# 1609
const char*_tmp7EA;void*_tmp7E9[1];struct Cyc_String_pa_PrintArg_struct _tmp7E8;void*_tmp329=(_tmp7E8.tag=0,((_tmp7E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7E9[0]=& _tmp7E8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7EA="expecting struct or union, found %s",_tag_dyneither(_tmp7EA,sizeof(char),36))),_tag_dyneither(_tmp7E9,sizeof(void*),1)))))));_npop_handler(0);return _tmp329;}}_LL1D2:;};
# 1567
;_pop_region(r);}
# 1615
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1618
struct _RegionHandle _tmp32A=_new_region("r");struct _RegionHandle*r=& _tmp32A;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1623
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp32B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp32C=_tmp32B;void*_tmp32D;void*_tmp32E;union Cyc_Absyn_Constraint*_tmp32F;union Cyc_Absyn_Constraint*_tmp330;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->tag == 5){_LL1DA: _tmp32D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->f1).elt_typ;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->f1).ptr_atts).rgn;_tmp32F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->f1).ptr_atts).bounds;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32C)->f1).ptr_atts).zero_term;_LL1DB:
# 1626
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp32F);
{void*_tmp331=Cyc_Tcutil_compress(_tmp32D);void*_tmp332=_tmp331;enum Cyc_Absyn_AggrKind _tmp333;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Aggrdecl*_tmp335;struct Cyc_List_List*_tmp336;switch(*((int*)_tmp332)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp332)->f1).aggr_info).KnownAggr).tag == 2){_LL1DF: _tmp335=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp332)->f1).aggr_info).KnownAggr).val;_tmp336=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp332)->f1).targs;_LL1E0: {
# 1629
struct Cyc_Absyn_Aggrfield*_tmp337=Cyc_Absyn_lookup_decl_field(_tmp335,f);
if(_tmp337 == 0){
const char*_tmp7EF;void*_tmp7EE[2];struct Cyc_String_pa_PrintArg_struct _tmp7ED;struct Cyc_String_pa_PrintArg_struct _tmp7EC;void*_tmp33C=(_tmp7EC.tag=0,((_tmp7EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7ED.tag=0,((_tmp7ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp335->name)),((_tmp7EE[0]=& _tmp7ED,((_tmp7EE[1]=& _tmp7EC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7EF="type %s has no %s member",_tag_dyneither(_tmp7EF,sizeof(char),25))),_tag_dyneither(_tmp7EE,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp33C;}
# 1634
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp335->impl))->tagged)*is_tagged=1;{
void*t3=_tmp337->type;
if(_tmp336 != 0){
struct _RegionHandle _tmp33D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33D;_push_region(rgn);
{struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp335->tvs,_tmp336);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp33E,_tmp337->type);}
# 1638
;_pop_region(rgn);}{
# 1642
struct Cyc_Absyn_Kind*_tmp33F=Cyc_Tcutil_typ_kind(t3);
# 1645
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp33F) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp340=Cyc_Tcutil_compress(t3);void*_tmp341=_tmp340;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp341)->tag == 8){_LL1E6: _LL1E7:
 goto _LL1E5;}else{_LL1E8: _LL1E9: {
# 1649
const char*_tmp7F3;void*_tmp7F2[1];struct Cyc_String_pa_PrintArg_struct _tmp7F1;void*_tmp345=(_tmp7F1.tag=0,((_tmp7F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7F2[0]=& _tmp7F1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7F3="cannot get member %s since its type is abstract",_tag_dyneither(_tmp7F3,sizeof(char),48))),_tag_dyneither(_tmp7F2,sizeof(void*),1)))))));_npop_handler(0);return _tmp345;}}_LL1E5:;}
# 1654
if((((_tmp335->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp335->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp337->requires_clause == 0){
# 1657
const char*_tmp7F7;void*_tmp7F6[1];struct Cyc_String_pa_PrintArg_struct _tmp7F5;void*_tmp349=(_tmp7F5.tag=0,((_tmp7F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7F6[0]=& _tmp7F5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7F7="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp7F7,sizeof(char),56))),_tag_dyneither(_tmp7F6,sizeof(void*),1)))))));_npop_handler(0);return _tmp349;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp335->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp7FB;void*_tmp7FA[1];struct Cyc_String_pa_PrintArg_struct _tmp7F9;void*_tmp34D=(_tmp7F9.tag=0,((_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7FA[0]=& _tmp7F9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FB="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp7FB,sizeof(char),72))),_tag_dyneither(_tmp7FA,sizeof(void*),1)))))));_npop_handler(0);return _tmp34D;}}{
# 1664
void*_tmp34E=t3;_npop_handler(0);return _tmp34E;};};};}}else{goto _LL1E3;}case 12: _LL1E1: _tmp333=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp332)->f1;_tmp334=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp332)->f2;_LL1E2: {
# 1666
struct Cyc_Absyn_Aggrfield*_tmp34F=Cyc_Absyn_lookup_field(_tmp334,f);
if(_tmp34F == 0){
const char*_tmp7FF;void*_tmp7FE[1];struct Cyc_String_pa_PrintArg_struct _tmp7FD;void*_tmp353=(_tmp7FD.tag=0,((_tmp7FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp7FE[0]=& _tmp7FD,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FF="type has no %s field",_tag_dyneither(_tmp7FF,sizeof(char),21))),_tag_dyneither(_tmp7FE,sizeof(void*),1)))))));_npop_handler(0);return _tmp353;}
# 1671
if((_tmp333 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp34F->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp803;void*_tmp802[1];struct Cyc_String_pa_PrintArg_struct _tmp801;void*_tmp357=(_tmp801.tag=0,((_tmp801.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp802[0]=& _tmp801,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp803="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp803,sizeof(char),56))),_tag_dyneither(_tmp802,sizeof(void*),1)))))));_npop_handler(0);return _tmp357;}{
void*_tmp358=_tmp34F->type;_npop_handler(0);return _tmp358;};}default: _LL1E3: _LL1E4:
 goto _LL1DE;}_LL1DE:;}
# 1676
goto _LL1D9;}else{_LL1DC: _LL1DD:
 goto _LL1D9;}_LL1D9:;}{
# 1679
const char*_tmp807;void*_tmp806[1];struct Cyc_String_pa_PrintArg_struct _tmp805;void*_tmp35C=(_tmp805.tag=0,((_tmp805.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp806[0]=& _tmp805,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp807="expecting struct or union pointer, found %s",_tag_dyneither(_tmp807,sizeof(char),44))),_tag_dyneither(_tmp806,sizeof(void*),1)))))));_npop_handler(0);return _tmp35C;};
# 1618
;_pop_region(r);}
# 1684
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1686
struct _tuple15 _tmp35D=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp35E=_tmp35D;unsigned int _tmp35F;int _tmp360;_LL1EB: _tmp35F=_tmp35E.f1;_tmp360=_tmp35E.f2;_LL1EC:;
if(!_tmp360){
const char*_tmp80A;void*_tmp809;return(_tmp809=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp80A="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp80A,sizeof(char),47))),_tag_dyneither(_tmp809,sizeof(void*),0)));}{
# 1690
struct _handler_cons _tmp363;_push_handler(& _tmp363);{int _tmp365=0;if(setjmp(_tmp363.handler))_tmp365=1;if(!_tmp365){
{void*_tmp366=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp35F)).f2;_npop_handler(0);return _tmp366;};_pop_handler();}else{void*_tmp364=(void*)_exn_thrown;void*_tmp367=_tmp364;void*_tmp368;if(((struct Cyc_List_Nth_exn_struct*)_tmp367)->tag == Cyc_List_Nth){_LL1EE: _LL1EF: {
# 1693
const char*_tmp80F;void*_tmp80E[2];struct Cyc_Int_pa_PrintArg_struct _tmp80D;struct Cyc_Int_pa_PrintArg_struct _tmp80C;return(_tmp80C.tag=1,((_tmp80C.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp80D.tag=1,((_tmp80D.f1=(unsigned long)((int)_tmp35F),((_tmp80E[0]=& _tmp80D,((_tmp80E[1]=& _tmp80C,Cyc_Tcexp_expr_err(te,loc,0,((_tmp80F="index is %d but tuple has only %d fields",_tag_dyneither(_tmp80F,sizeof(char),41))),_tag_dyneither(_tmp80E,sizeof(void*),2)))))))))))));}}else{_LL1F0: _tmp368=_tmp367;_LL1F1:(int)_rethrow(_tmp368);}_LL1ED:;}};};}
# 1698
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1700
struct _RegionHandle _tmp36D=_new_region("r");struct _RegionHandle*r=& _tmp36D;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp36E=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp36E),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp36E),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp36E,e2)){
const char*_tmp813;void*_tmp812[1];struct Cyc_String_pa_PrintArg_struct _tmp811;void*_tmp372=(_tmp811.tag=0,((_tmp811.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp812[0]=& _tmp811,Cyc_Tcexp_expr_err(_tmp36E,e2->loc,topt,((_tmp813="expecting int subscript, found %s",_tag_dyneither(_tmp813,sizeof(char),34))),_tag_dyneither(_tmp812,sizeof(void*),1)))))));_npop_handler(0);return _tmp372;}{
# 1711
void*_tmp373=t1;struct Cyc_List_List*_tmp374;void*_tmp375;struct Cyc_Absyn_Tqual _tmp376;void*_tmp377;union Cyc_Absyn_Constraint*_tmp378;union Cyc_Absyn_Constraint*_tmp379;switch(*((int*)_tmp373)){case 5: _LL1F3: _tmp375=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373)->f1).elt_typ;_tmp376=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373)->f1).elt_tq;_tmp377=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373)->f1).ptr_atts).rgn;_tmp378=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373)->f1).ptr_atts).bounds;_tmp379=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373)->f1).ptr_atts).zero_term;_LL1F4:
# 1715
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp379)){
int emit_warning=1;
{void*_tmp37A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp378);void*_tmp37B=_tmp37A;struct Cyc_Absyn_Exp*_tmp37C;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37B)->tag == 1){_LL1FA: _tmp37C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37B)->f1;_LL1FB:
# 1719
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp37D=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp37E=_tmp37D;unsigned int _tmp37F;int _tmp380;_LL1FF: _tmp37F=_tmp37E.f1;_tmp380=_tmp37E.f2;_LL200:;
if(_tmp380){
struct _tuple15 _tmp381=Cyc_Evexp_eval_const_uint_exp(_tmp37C);struct _tuple15 _tmp382=_tmp381;unsigned int _tmp383;int _tmp384;_LL202: _tmp383=_tmp382.f1;_tmp384=_tmp382.f2;_LL203:;
if(_tmp384  && _tmp383 > _tmp37F)emit_warning=0;}}
# 1726
goto _LL1F9;}else{_LL1FC: _LL1FD:
 emit_warning=0;goto _LL1F9;}_LL1F9:;}
# 1729
if(emit_warning){
const char*_tmp816;void*_tmp815;(_tmp815=0,Cyc_Tcutil_warn(e2->loc,((_tmp816="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp816,sizeof(char),63))),_tag_dyneither(_tmp815,sizeof(void*),0)));}}else{
# 1733
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp387=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp388=_tmp387;unsigned int _tmp389;int _tmp38A;_LL205: _tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;_LL206:;
if(_tmp38A)
Cyc_Tcutil_check_bound(loc,_tmp389,_tmp378);}else{
# 1739
if(Cyc_Tcutil_is_bound_one(_tmp378) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp379)){
const char*_tmp819;void*_tmp818;(_tmp818=0,Cyc_Tcutil_warn(e1->loc,((_tmp819="subscript applied to pointer to one object",_tag_dyneither(_tmp819,sizeof(char),43))),_tag_dyneither(_tmp818,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp378);}}
# 1744
Cyc_Tcenv_check_rgn_accessible(_tmp36E,loc,_tmp377);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp375),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp36E)){
const char*_tmp81C;void*_tmp81B;(_tmp81B=0,Cyc_Tcutil_terr(e1->loc,((_tmp81C="can't subscript an abstract pointer",_tag_dyneither(_tmp81C,sizeof(char),36))),_tag_dyneither(_tmp81B,sizeof(void*),0)));}{
void*_tmp38F=_tmp375;_npop_handler(0);return _tmp38F;};case 10: _LL1F5: _tmp374=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp373)->f1;_LL1F6: {
void*_tmp390=Cyc_Tcexp_ithTupleType(_tmp36E,loc,_tmp374,e2);_npop_handler(0);return _tmp390;}default: _LL1F7: _LL1F8: {
const char*_tmp820;void*_tmp81F[1];struct Cyc_String_pa_PrintArg_struct _tmp81E;void*_tmp394=(_tmp81E.tag=0,((_tmp81E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp81F[0]=& _tmp81E,Cyc_Tcexp_expr_err(_tmp36E,loc,topt,((_tmp820="subscript applied to %s",_tag_dyneither(_tmp820,sizeof(char),24))),_tag_dyneither(_tmp81F,sizeof(void*),1)))))));_npop_handler(0);return _tmp394;}}_LL1F2:;};};}
# 1701
;_pop_region(r);}
# 1755
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp395=Cyc_Tcutil_compress(*topt);void*_tmp396=_tmp395;struct Cyc_List_List*_tmp397;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp396)->tag == 10){_LL208: _tmp397=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp396)->f1;_LL209:
# 1761
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp397)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1764
goto _LL207;
# 1766
for(0;es != 0;(es=es->tl,_tmp397=_tmp397->tl)){
struct _RegionHandle _tmp398=_new_region("r");struct _RegionHandle*r=& _tmp398;_push_region(r);
{int bogus=0;
void*_tmp399=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp397))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp399,(struct Cyc_Absyn_Exp*)es->hd);
# 1772
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)_tmp397->hd)).f2,& bogus);{
struct _tuple18*_tmp823;struct Cyc_List_List*_tmp822;fields=((_tmp822=_cycalloc(sizeof(*_tmp822)),((_tmp822->hd=((_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823->f1=(*((struct _tuple18*)_tmp397->hd)).f1,((_tmp823->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp823)))))),((_tmp822->tl=fields,_tmp822))))));};}
# 1768
;_pop_region(r);}
# 1776
done=1;
goto _LL207;}else{_LL20A: _LL20B:
 goto _LL207;}_LL207:;}
# 1780
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp39C=_new_region("r");struct _RegionHandle*r=& _tmp39C;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple18*_tmp826;struct Cyc_List_List*_tmp825;fields=((_tmp825=_cycalloc(sizeof(*_tmp825)),((_tmp825->hd=((_tmp826=_cycalloc(sizeof(*_tmp826)),((_tmp826->f1=Cyc_Absyn_empty_tqual(0),((_tmp826->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp826)))))),((_tmp825->tl=fields,_tmp825))))));};
# 1783
;_pop_region(r);}{
# 1787
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp829;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp828;return(void*)((_tmp828=_cycalloc(sizeof(*_tmp828)),((_tmp828[0]=((_tmp829.tag=10,((_tmp829.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp829)))),_tmp828))));};}
# 1791
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1794
const char*_tmp82C;void*_tmp82B;return(_tmp82B=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp82C="tcCompoundLit",_tag_dyneither(_tmp82C,sizeof(char),14))),_tag_dyneither(_tmp82B,sizeof(void*),0)));}
# 1804 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1807
void*res_t2;
struct _RegionHandle _tmp3A3=_new_region("r");struct _RegionHandle*r=& _tmp3A3;_push_region(r);{
int _tmp3A4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp82F;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp82E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3A5=(_tmp82E=_cycalloc(sizeof(*_tmp82E)),((_tmp82E[0]=((_tmp82F.tag=0,((_tmp82F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3A4),_tmp82F)))),_tmp82E)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3A5,loc);
# 1816
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3A6=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3A4 - 1);
if(!Cyc_Tcutil_is_zero(_tmp3A6)){
const char*_tmp832;void*_tmp831;(_tmp831=0,Cyc_Tcutil_terr(_tmp3A6->loc,((_tmp832="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp832,sizeof(char),45))),_tag_dyneither(_tmp831,sizeof(void*),0)));}}
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
const char*_tmp836;void*_tmp835[1];struct Cyc_String_pa_PrintArg_struct _tmp834;(_tmp834.tag=0,((_tmp834.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp835[0]=& _tmp834,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp836="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp836,sizeof(char),53))),_tag_dyneither(_tmp835,sizeof(void*),1)))))));}}
# 1835
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1840
void*_tmp3AE=(void*)ds->hd;void*_tmp3AF=_tmp3AE;struct Cyc_Absyn_Exp*_tmp3B0;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3AF)->tag == 1){_LL20D: _LL20E:
# 1842
{const char*_tmp839;void*_tmp838;(_tmp838=0,Cyc_Tcutil_terr(loc,((_tmp839="only array index designators are supported",_tag_dyneither(_tmp839,sizeof(char),43))),_tag_dyneither(_tmp838,sizeof(void*),0)));}
goto _LL20C;}else{_LL20F: _tmp3B0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3AF)->f1;_LL210:
# 1845
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3B0);{
struct _tuple15 _tmp3B3=Cyc_Evexp_eval_const_uint_exp(_tmp3B0);struct _tuple15 _tmp3B4=_tmp3B3;unsigned int _tmp3B5;int _tmp3B6;_LL212: _tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;_LL213:;
if(!_tmp3B6){
const char*_tmp83C;void*_tmp83B;(_tmp83B=0,Cyc_Tcutil_terr(_tmp3B0->loc,((_tmp83C="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp83C,sizeof(char),47))),_tag_dyneither(_tmp83B,sizeof(void*),0)));}else{
if(_tmp3B5 != offset){
const char*_tmp841;void*_tmp840[2];struct Cyc_Int_pa_PrintArg_struct _tmp83F;struct Cyc_Int_pa_PrintArg_struct _tmp83E;(_tmp83E.tag=1,((_tmp83E.f1=(unsigned long)((int)_tmp3B5),((_tmp83F.tag=1,((_tmp83F.f1=(unsigned long)offset,((_tmp840[0]=& _tmp83F,((_tmp840[1]=& _tmp83E,Cyc_Tcutil_terr(_tmp3B0->loc,((_tmp841="expecting index designator %d but found %d",_tag_dyneither(_tmp841,sizeof(char),43))),_tag_dyneither(_tmp840,sizeof(void*),2)))))))))))));}}
# 1852
goto _LL20C;};}_LL20C:;}}}{
# 1856
void*_tmp3BD=res_t2;_npop_handler(0);return _tmp3BD;};
# 1808
;_pop_region(r);}
# 1860
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1863
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp3BE=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp3BF=_tmp3BE;void*_tmp3C0;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3BF)->tag == 19){_LL215: _tmp3C0=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3BF)->f1;_LL216:
# 1868
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp3C1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3C0,0),0,Cyc_Absyn_No_coercion,0);
_tmp3C1->topt=bound->topt;
# 1873
bound=_tmp3C1;}
# 1875
goto _LL214;}else{_LL217: _LL218:
# 1877
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp845;void*_tmp844[1];struct Cyc_String_pa_PrintArg_struct _tmp843;(_tmp843.tag=0,((_tmp843.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp844[0]=& _tmp843,Cyc_Tcutil_terr(bound->loc,((_tmp845="expecting unsigned int, found %s",_tag_dyneither(_tmp845,sizeof(char),33))),_tag_dyneither(_tmp844,sizeof(void*),1)))))));}}_LL214:;}
# 1882
if(!(vd->tq).real_const){
const char*_tmp848;void*_tmp847;(_tmp847=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp848="comprehension index variable is not declared const!",_tag_dyneither(_tmp848,sizeof(char),52))),_tag_dyneither(_tmp847,sizeof(void*),0)));}{
# 1885
struct _RegionHandle _tmp3C7=_new_region("r");struct _RegionHandle*r=& _tmp3C7;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1891
struct Cyc_Tcenv_Tenv*_tmp3C8=te2;
void**_tmp3C9=0;
struct Cyc_Absyn_Tqual*_tmp3CA=0;
union Cyc_Absyn_Constraint**_tmp3CB=0;
# 1896
if(topt != 0){
void*_tmp3CC=Cyc_Tcutil_compress(*topt);void*_tmp3CD=_tmp3CC;void*_tmp3CE;struct Cyc_Absyn_Tqual _tmp3CF;struct Cyc_Absyn_Exp*_tmp3D0;union Cyc_Absyn_Constraint*_tmp3D1;struct Cyc_Absyn_PtrInfo _tmp3D2;switch(*((int*)_tmp3CD)){case 5: _LL21A: _tmp3D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CD)->f1;_LL21B:
# 1899
{void**_tmp849;_tmp3C9=((_tmp849=_region_malloc(r,sizeof(*_tmp849)),((_tmp849[0]=_tmp3D2.elt_typ,_tmp849))));}
{struct Cyc_Absyn_Tqual*_tmp84A;_tmp3CA=((_tmp84A=_region_malloc(r,sizeof(*_tmp84A)),((_tmp84A[0]=_tmp3D2.elt_tq,_tmp84A))));}
{union Cyc_Absyn_Constraint**_tmp84B;_tmp3CB=((_tmp84B=_region_malloc(r,sizeof(*_tmp84B)),((_tmp84B[0]=(_tmp3D2.ptr_atts).zero_term,_tmp84B))));}
goto _LL219;case 8: _LL21C: _tmp3CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3CD)->f1).elt_type;_tmp3CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3CD)->f1).tq;_tmp3D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3CD)->f1).num_elts;_tmp3D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3CD)->f1).zero_term;_LL21D:
# 1904
{void**_tmp84C;_tmp3C9=((_tmp84C=_region_malloc(r,sizeof(*_tmp84C)),((_tmp84C[0]=_tmp3CE,_tmp84C))));}
{struct Cyc_Absyn_Tqual*_tmp84D;_tmp3CA=((_tmp84D=_region_malloc(r,sizeof(*_tmp84D)),((_tmp84D[0]=_tmp3CF,_tmp84D))));}
{union Cyc_Absyn_Constraint**_tmp84E;_tmp3CB=((_tmp84E=_region_malloc(r,sizeof(*_tmp84E)),((_tmp84E[0]=_tmp3D1,_tmp84E))));}
goto _LL219;default: _LL21E: _LL21F:
# 1909
 goto _LL219;}_LL219:;}{
# 1912
void*t=Cyc_Tcexp_tcExp(_tmp3C8,_tmp3C9,body);
# 1914
struct _RegionHandle _tmp3D9=_new_region("temp");struct _RegionHandle*temp=& _tmp3D9;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body)){
const char*_tmp851;void*_tmp850;(_tmp850=0,Cyc_Tcutil_terr(body->loc,((_tmp851="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp851,sizeof(char),49))),_tag_dyneither(_tmp850,sizeof(void*),0)));}
# 1918
if(_tmp3C8->le == 0){
# 1920
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp854;void*_tmp853;(_tmp853=0,Cyc_Tcutil_terr(bound->loc,((_tmp854="bound is not constant",_tag_dyneither(_tmp854,sizeof(char),22))),_tag_dyneither(_tmp853,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp857;void*_tmp856;(_tmp856=0,Cyc_Tcutil_terr(bound->loc,((_tmp857="body is not constant",_tag_dyneither(_tmp857,sizeof(char),21))),_tag_dyneither(_tmp856,sizeof(void*),0)));}}
# 1925
if(_tmp3CB != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3CB)){
# 1928
struct Cyc_Absyn_Exp*_tmp3E0=Cyc_Absyn_uint_exp(1,0);_tmp3E0->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp3E0,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1932
struct _RegionHandle _tmp3E1=_new_region("r");struct _RegionHandle*r=& _tmp3E1;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body)){
const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_Tcutil_terr(body->loc,((_tmp85A="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp85A,sizeof(char),49))),_tag_dyneither(_tmp859,sizeof(void*),0)));}{
# 1938
void*_tmp3E4=Cyc_Absyn_array_typ(t,_tmp3CA == 0?Cyc_Absyn_empty_tqual(0):*_tmp3CA,bound,
_tmp3CB == 0?Cyc_Absyn_false_conref:*_tmp3CB,0);
void*_tmp3E5=_tmp3E4;_npop_handler(2);return _tmp3E5;};
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
{void*_tmp3E6=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp3E7=_tmp3E6;void*_tmp3E8;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E7)->tag == 19){_LL221: _tmp3E8=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E7)->f1;_LL222:
# 1953
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp3E9=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3E8,0),0,Cyc_Absyn_No_coercion,0);
_tmp3E9->topt=bound->topt;
# 1958
bound=_tmp3E9;}
# 1960
goto _LL220;}else{_LL223: _LL224:
# 1962
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp85E;void*_tmp85D[1];struct Cyc_String_pa_PrintArg_struct _tmp85C;(_tmp85C.tag=0,((_tmp85C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp85D[0]=& _tmp85C,Cyc_Tcutil_terr(bound->loc,((_tmp85E="expecting unsigned int, found %s",_tag_dyneither(_tmp85E,sizeof(char),33))),_tag_dyneither(_tmp85D,sizeof(void*),1)))))));}}_LL220:;}{
# 1967
struct _RegionHandle _tmp3ED=_new_region("r");struct _RegionHandle*r=& _tmp3ED;_push_region(r);
{void**_tmp3EE=0;
struct Cyc_Absyn_Tqual*_tmp3EF=0;
union Cyc_Absyn_Constraint**_tmp3F0=0;
# 1972
if(topt != 0){
void*_tmp3F1=Cyc_Tcutil_compress(*topt);void*_tmp3F2=_tmp3F1;void*_tmp3F3;struct Cyc_Absyn_Tqual _tmp3F4;struct Cyc_Absyn_Exp*_tmp3F5;union Cyc_Absyn_Constraint*_tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F7;switch(*((int*)_tmp3F2)){case 5: _LL226: _tmp3F7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F2)->f1;_LL227:
# 1975
{void**_tmp85F;_tmp3EE=((_tmp85F=_region_malloc(r,sizeof(*_tmp85F)),((_tmp85F[0]=_tmp3F7.elt_typ,_tmp85F))));}
{struct Cyc_Absyn_Tqual*_tmp860;_tmp3EF=((_tmp860=_region_malloc(r,sizeof(*_tmp860)),((_tmp860[0]=_tmp3F7.elt_tq,_tmp860))));}
{union Cyc_Absyn_Constraint**_tmp861;_tmp3F0=((_tmp861=_region_malloc(r,sizeof(*_tmp861)),((_tmp861[0]=(_tmp3F7.ptr_atts).zero_term,_tmp861))));}
goto _LL225;case 8: _LL228: _tmp3F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F2)->f1).elt_type;_tmp3F4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F2)->f1).tq;_tmp3F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F2)->f1).num_elts;_tmp3F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F2)->f1).zero_term;_LL229:
# 1980
{void**_tmp862;_tmp3EE=((_tmp862=_region_malloc(r,sizeof(*_tmp862)),((_tmp862[0]=_tmp3F3,_tmp862))));}
{struct Cyc_Absyn_Tqual*_tmp863;_tmp3EF=((_tmp863=_region_malloc(r,sizeof(*_tmp863)),((_tmp863[0]=_tmp3F4,_tmp863))));}
{union Cyc_Absyn_Constraint**_tmp864;_tmp3F0=((_tmp864=_region_malloc(r,sizeof(*_tmp864)),((_tmp864[0]=_tmp3F6,_tmp864))));}
goto _LL225;default: _LL22A: _LL22B:
# 1985
 goto _LL225;}_LL225:;}
# 1988
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,1,t);
if(_tmp3EE != 0)Cyc_Tcutil_unify(*_tmp3EE,t);
# 1991
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp867;void*_tmp866;(_tmp866=0,Cyc_Tcutil_terr(bound->loc,((_tmp867="bound is not constant",_tag_dyneither(_tmp867,sizeof(char),22))),_tag_dyneither(_tmp866,sizeof(void*),0)));}}
# 1995
if(_tmp3F0 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3F0)){
# 1998
struct Cyc_Absyn_Exp*_tmp400=Cyc_Absyn_uint_exp(1,0);_tmp400->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp400,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 2002
const char*_tmp86A;void*_tmp869;(_tmp869=0,Cyc_Tcutil_terr(loc,((_tmp86A="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp86A,sizeof(char),74))),_tag_dyneither(_tmp869,sizeof(void*),0)));};}{
# 2005
void*_tmp403=Cyc_Absyn_array_typ(t,_tmp3EF == 0?Cyc_Absyn_empty_tqual(0):*_tmp3EF,bound,
_tmp3F0 == 0?Cyc_Absyn_false_conref:*_tmp3F0,0);
void*_tmp404=_tmp403;_npop_handler(0);return _tmp404;};}
# 1968
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2021 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2025
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp86B;adptr=((_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B[0]=ad,_tmp86B))));};}else{
# 2031
{struct _handler_cons _tmp406;_push_handler(& _tmp406);{int _tmp408=0;if(setjmp(_tmp406.handler))_tmp408=1;if(!_tmp408){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2031
;_pop_handler();}else{void*_tmp407=(void*)_exn_thrown;void*_tmp409=_tmp407;void*_tmp40A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp409)->tag == Cyc_Dict_Absent){_LL22D: _LL22E:
# 2034
{const char*_tmp86F;void*_tmp86E[1];struct Cyc_String_pa_PrintArg_struct _tmp86D;(_tmp86D.tag=0,((_tmp86D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp86E[0]=& _tmp86D,Cyc_Tcutil_terr(loc,((_tmp86F="unbound struct/union name %s",_tag_dyneither(_tmp86F,sizeof(char),29))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL22F: _tmp40A=_tmp409;_LL230:(int)_rethrow(_tmp40A);}_LL22C:;}};}
# 2037
*ad_opt=ad;
*tn=ad->name;}
# 2040
if(ad->impl == 0){
{const char*_tmp877;void*_tmp876[1];const char*_tmp875;const char*_tmp874;struct Cyc_String_pa_PrintArg_struct _tmp873;(_tmp873.tag=0,((_tmp873.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp874="struct",_tag_dyneither(_tmp874,sizeof(char),7)):((_tmp875="union",_tag_dyneither(_tmp875,sizeof(char),6)))),((_tmp876[0]=& _tmp873,Cyc_Tcutil_terr(loc,((_tmp877="can't construct abstract %s",_tag_dyneither(_tmp877,sizeof(char),28))),_tag_dyneither(_tmp876,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 2044
struct _RegionHandle _tmp413=_new_region("rgn");struct _RegionHandle*rgn=& _tmp413;_push_region(rgn);
# 2049
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp414=Cyc_Tcenv_new_status(te);
if(_tmp414 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 2058
te2=Cyc_Tcenv_set_new_status(rgn,_tmp414,te);}{
# 2060
struct _tuple12 _tmp878;struct _tuple12 _tmp415=(_tmp878.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp878.f2=rgn,_tmp878)));
struct Cyc_List_List*_tmp416=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp415,ad->tvs);
struct Cyc_List_List*_tmp417=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp415,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp418=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp416);
struct Cyc_List_List*_tmp419=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp417);
struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp416,_tmp417);
void*res_typ;
# 2071
if(topt != 0){
void*_tmp41B=Cyc_Tcutil_compress(*topt);void*_tmp41C=_tmp41B;struct Cyc_Absyn_Aggrdecl**_tmp41D;struct Cyc_List_List*_tmp41E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41C)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41C)->f1).aggr_info).KnownAggr).tag == 2){_LL232: _tmp41D=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41C)->f1).aggr_info).KnownAggr).val;_tmp41E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41C)->f1).targs;_LL233:
# 2074
 if(*_tmp41D == *adptr){
{struct Cyc_List_List*_tmp41F=_tmp418;for(0;_tmp41F != 0  && _tmp41E != 0;
(_tmp41F=_tmp41F->tl,_tmp41E=_tmp41E->tl)){
Cyc_Tcutil_unify((void*)_tmp41F->hd,(void*)_tmp41E->hd);}}
# 2079
res_typ=*topt;
goto _LL231;}
# 2082
goto _LL235;}else{goto _LL234;}}else{_LL234: _LL235: {
# 2084
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp87E;struct Cyc_Absyn_AggrInfo _tmp87D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp87C;res_typ=(void*)((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=((_tmp87E.tag=11,((_tmp87E.f1=((_tmp87D.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp87D.targs=_tmp418,_tmp87D)))),_tmp87E)))),_tmp87C))));}}_LL231:;}else{
# 2087
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp884;struct Cyc_Absyn_AggrInfo _tmp883;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp882;res_typ=(void*)((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882[0]=((_tmp884.tag=11,((_tmp884.f1=((_tmp883.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp883.targs=_tmp418,_tmp883)))),_tmp884)))),_tmp882))));}{
# 2090
struct Cyc_List_List*_tmp426=*ts;
struct Cyc_List_List*_tmp427=_tmp419;
while(_tmp426 != 0  && _tmp427 != 0){
# 2094
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,0,(void*)_tmp426->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp426->hd);
Cyc_Tcutil_unify((void*)_tmp426->hd,(void*)_tmp427->hd);
_tmp426=_tmp426->tl;
_tmp427=_tmp427->tl;}
# 2100
if(_tmp426 != 0){
const char*_tmp887;void*_tmp886;(_tmp886=0,Cyc_Tcutil_terr(loc,((_tmp887="too many explicit witness types",_tag_dyneither(_tmp887,sizeof(char),32))),_tag_dyneither(_tmp886,sizeof(void*),0)));}
# 2103
*ts=_tmp419;{
# 2106
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp42A=(struct _tuple19*)fields->hd;struct _tuple19*_tmp42B=_tmp42A;struct Cyc_Absyn_Aggrfield*_tmp42C;struct Cyc_Absyn_Exp*_tmp42D;_LL237: _tmp42C=_tmp42B->f1;_tmp42D=_tmp42B->f2;_LL238:;{
void*_tmp42E=Cyc_Tcutil_rsubstitute(rgn,_tmp41A,_tmp42C->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp42E,_tmp42D);
# 2117
if(!Cyc_Tcutil_coerce_arg(te2,_tmp42D,_tmp42E,& bogus)){
{const char*_tmp893;void*_tmp892[5];struct Cyc_String_pa_PrintArg_struct _tmp891;const char*_tmp890;const char*_tmp88F;struct Cyc_String_pa_PrintArg_struct _tmp88E;struct Cyc_String_pa_PrintArg_struct _tmp88D;struct Cyc_String_pa_PrintArg_struct _tmp88C;struct Cyc_String_pa_PrintArg_struct _tmp88B;(_tmp88B.tag=0,((_tmp88B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2121
Cyc_Absynpp_typ2string((void*)_check_null(_tmp42D->topt))),((_tmp88C.tag=0,((_tmp88C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2120
Cyc_Absynpp_typ2string(_tmp42E)),((_tmp88D.tag=0,((_tmp88D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp88E.tag=0,((_tmp88E.f1=(struct _dyneither_ptr)(
# 2119
ad->kind == Cyc_Absyn_StructA?(_tmp88F="struct",_tag_dyneither(_tmp88F,sizeof(char),7)):((_tmp890="union",_tag_dyneither(_tmp890,sizeof(char),6)))),((_tmp891.tag=0,((_tmp891.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp42C->name),((_tmp892[0]=& _tmp891,((_tmp892[1]=& _tmp88E,((_tmp892[2]=& _tmp88D,((_tmp892[3]=& _tmp88C,((_tmp892[4]=& _tmp88B,Cyc_Tcutil_terr(_tmp42D->loc,((_tmp893="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp893,sizeof(char),40))),_tag_dyneither(_tmp892,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2122
Cyc_Tcutil_explain_failure();}};}{
# 2126
struct Cyc_List_List*_tmp438=0;
{struct Cyc_List_List*_tmp439=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp439 != 0;_tmp439=_tmp439->tl){
struct _tuple0*_tmp896;struct Cyc_List_List*_tmp895;_tmp438=((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895->hd=((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp41A,(*((struct _tuple0*)_tmp439->hd)).f1),((_tmp896->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp41A,(*((struct _tuple0*)_tmp439->hd)).f2),_tmp896)))))),((_tmp895->tl=_tmp438,_tmp895))))));}}
# 2131
_tmp438=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp438);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp438);{
void*_tmp43C=res_typ;_npop_handler(0);return _tmp43C;};};};};};}
# 2049
;_pop_region(rgn);};}
# 2138
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2140
struct _RegionHandle _tmp43E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp43E;_push_region(rgn);{
void*_tmp43F=Cyc_Tcutil_compress(ts);void*_tmp440=_tmp43F;enum Cyc_Absyn_AggrKind _tmp441;struct Cyc_List_List*_tmp442;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp440)->tag == 12){_LL23A: _tmp441=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp440)->f1;_tmp442=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp440)->f2;_LL23B:
# 2143
 if(_tmp441 == Cyc_Absyn_UnionA){
const char*_tmp899;void*_tmp898;(_tmp898=0,Cyc_Tcutil_terr(loc,((_tmp899="expecting struct but found union",_tag_dyneither(_tmp899,sizeof(char),33))),_tag_dyneither(_tmp898,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp442);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp445=(struct _tuple19*)fields->hd;struct _tuple19*_tmp446=_tmp445;struct Cyc_Absyn_Aggrfield*_tmp447;struct Cyc_Absyn_Exp*_tmp448;_LL23F: _tmp447=_tmp446->f1;_tmp448=_tmp446->f2;_LL240:;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp447->type,_tmp448);
# 2152
if(!Cyc_Tcutil_coerce_arg(te,_tmp448,_tmp447->type,& bogus)){
{const char*_tmp89F;void*_tmp89E[3];struct Cyc_String_pa_PrintArg_struct _tmp89D;struct Cyc_String_pa_PrintArg_struct _tmp89C;struct Cyc_String_pa_PrintArg_struct _tmp89B;(_tmp89B.tag=0,((_tmp89B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2155
Cyc_Absynpp_typ2string((void*)_check_null(_tmp448->topt))),((_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2154
Cyc_Absynpp_typ2string(_tmp447->type)),((_tmp89D.tag=0,((_tmp89D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp447->name),((_tmp89E[0]=& _tmp89D,((_tmp89E[1]=& _tmp89C,((_tmp89E[2]=& _tmp89B,Cyc_Tcutil_terr(_tmp448->loc,((_tmp89F="field %s of struct expects type %s != %s",_tag_dyneither(_tmp89F,sizeof(char),41))),_tag_dyneither(_tmp89E,sizeof(void*),3)))))))))))))))))));}
# 2156
Cyc_Tcutil_explain_failure();}}
# 2159
goto _LL239;};}else{_LL23C: _LL23D: {
const char*_tmp8A2;void*_tmp8A1;(_tmp8A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A2="tcAnonStruct: wrong type",_tag_dyneither(_tmp8A2,sizeof(char),25))),_tag_dyneither(_tmp8A1,sizeof(void*),0)));}}_LL239:;}{
# 2163
void*_tmp450=ts;_npop_handler(0);return _tmp450;};
# 2140
;_pop_region(rgn);}
# 2167
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2170
struct _RegionHandle _tmp451=_new_region("rgn");struct _RegionHandle*rgn=& _tmp451;_push_region(rgn);
{struct _tuple12 _tmp8A3;struct _tuple12 _tmp452=(_tmp8A3.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8A3.f2=rgn,_tmp8A3)));
struct Cyc_List_List*_tmp453=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp452,tud->tvs);
struct Cyc_List_List*_tmp454=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp453);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp8A9;struct Cyc_Absyn_DatatypeFieldInfo _tmp8A8;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp8A7;void*res=(void*)(
(_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=((_tmp8A9.tag=4,((_tmp8A9.f1=((_tmp8A8.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp8A8.targs=_tmp454,_tmp8A8)))),_tmp8A9)))),_tmp8A7))));
# 2178
if(topt != 0){
void*_tmp455=Cyc_Tcutil_compress(*topt);void*_tmp456=_tmp455;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp456)->tag == 4){_LL242: _LL243:
 Cyc_Tcutil_unify(*topt,res);goto _LL241;}else{_LL244: _LL245:
 goto _LL241;}_LL241:;}{
# 2184
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple18*)ts->hd)).f2;
if(_tmp453 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp453,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp8B1;void*_tmp8B0[3];struct Cyc_String_pa_PrintArg_struct _tmp8AF;struct Cyc_String_pa_PrintArg_struct _tmp8AE;const char*_tmp8AD;struct Cyc_String_pa_PrintArg_struct _tmp8AC;(_tmp8AC.tag=0,((_tmp8AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2195
e->topt == 0?(struct _dyneither_ptr)((_tmp8AD="?",_tag_dyneither(_tmp8AD,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp8AE.tag=0,((_tmp8AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2194
Cyc_Absynpp_typ2string(t)),((_tmp8AF.tag=0,((_tmp8AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8B0[0]=& _tmp8AF,((_tmp8B0[1]=& _tmp8AE,((_tmp8B0[2]=& _tmp8AC,Cyc_Tcutil_terr(e->loc,((_tmp8B1="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp8B1,sizeof(char),82))),_tag_dyneither(_tmp8B0,sizeof(void*),3)))))))))))))))))));}
# 2196
Cyc_Tcutil_explain_failure();}}
# 2199
if(es != 0){
const char*_tmp8B5;void*_tmp8B4[1];struct Cyc_String_pa_PrintArg_struct _tmp8B3;void*_tmp460=(_tmp8B3.tag=0,((_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2202
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8B4[0]=& _tmp8B3,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp8B5="too many arguments for datatype constructor %s",_tag_dyneither(_tmp8B5,sizeof(char),47))),_tag_dyneither(_tmp8B4,sizeof(void*),1)))))));_npop_handler(0);return _tmp460;}
if(ts != 0){
const char*_tmp8B9;void*_tmp8B8[1];struct Cyc_String_pa_PrintArg_struct _tmp8B7;void*_tmp464=(_tmp8B7.tag=0,((_tmp8B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8B8[0]=& _tmp8B7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B9="too few arguments for datatype constructor %s",_tag_dyneither(_tmp8B9,sizeof(char),46))),_tag_dyneither(_tmp8B8,sizeof(void*),1)))))));_npop_handler(0);return _tmp464;}{
void*_tmp465=res;_npop_handler(0);return _tmp465;};};}
# 2171
;_pop_region(rgn);}
# 2210
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2212
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2214
if(topt != 0){
void*_tmp46A=Cyc_Tcutil_compress(*topt);void*_tmp46B=_tmp46A;void*_tmp46C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46B)->tag == 5){_LL247: _tmp46C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46B)->f1).elt_typ;_LL248:
# 2217
 Cyc_Tcutil_unify(_tmp46C,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL246;}else{_LL249: _LL24A:
 goto _LL246;}_LL246:;}{
# 2223
const char*_tmp8C2;void*_tmp8C1[2];const char*_tmp8C0;const char*_tmp8BF;struct Cyc_String_pa_PrintArg_struct _tmp8BE;struct Cyc_String_pa_PrintArg_struct _tmp8BD;(_tmp8BD.tag=0,((_tmp8BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp8BE.tag=0,((_tmp8BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp8BF="calloc",_tag_dyneither(_tmp8BF,sizeof(char),7))):(struct _dyneither_ptr)((_tmp8C0="malloc",_tag_dyneither(_tmp8C0,sizeof(char),7))))),((_tmp8C1[0]=& _tmp8BE,((_tmp8C1[1]=& _tmp8BD,Cyc_Tcutil_terr(loc,((_tmp8C2="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp8C2,sizeof(char),60))),_tag_dyneither(_tmp8C1,sizeof(void*),2)))))))))))));};}
# 2227
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2229
enum Cyc_Absyn_AliasQual _tmp473=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp474=_tmp473;if(_tmp474 == Cyc_Absyn_Unique){_LL24C: _LL24D:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL24E: _LL24F:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL24B:;}
# 2235
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2240
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2243
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp8C5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8C4;void*expected_type=(void*)(
(_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4[0]=((_tmp8C5.tag=15,((_tmp8C5.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp8C5)))),_tmp8C4))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp475=Cyc_Tcutil_compress(handle_type);void*_tmp476=_tmp475;void*_tmp477;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp476)->tag == 15){_LL251: _tmp477=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp476)->f1;_LL252:
# 2248
 rgn=_tmp477;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL250;}else{_LL253: _LL254:
# 2252
{const char*_tmp8C9;void*_tmp8C8[1];struct Cyc_String_pa_PrintArg_struct _tmp8C7;(_tmp8C7.tag=0,((_tmp8C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp8C8[0]=& _tmp8C7,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp8C9="expecting region_t type but found %s",_tag_dyneither(_tmp8C9,sizeof(char),37))),_tag_dyneither(_tmp8C8,sizeof(void*),1)))))));}
goto _LL250;}_LL250:;}else{
# 2259
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2266
{struct _RegionHandle _tmp47D=_new_region("r");struct _RegionHandle*r=& _tmp47D;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2277 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp8CC;void*_tmp8CB;(_tmp8CB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8CC="calloc with empty type",_tag_dyneither(_tmp8CC,sizeof(char),23))),_tag_dyneither(_tmp8CB,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2289
void*er=(*e)->r;
retry_sizeof: {
void*_tmp480=er;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp482;void*_tmp483;switch(*((int*)_tmp480)){case 16: _LL256: _tmp483=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp480)->f1;_LL257:
# 2293
 elt_type=_tmp483;
{void**_tmp8CD;*t=((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=elt_type,_tmp8CD))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL255;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp480)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp480)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp480)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp480)->f2)->tl)->tl == 0){_LL258: _tmp481=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp480)->f2)->hd;_tmp482=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp480)->f2)->tl)->hd;_LL259:
# 2300
{struct _tuple0 _tmp8CE;struct _tuple0 _tmp485=(_tmp8CE.f1=_tmp481->r,((_tmp8CE.f2=_tmp482->r,_tmp8CE)));struct _tuple0 _tmp486=_tmp485;void*_tmp487;void*_tmp488;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp486.f1)->tag == 16){_LL25D: _tmp488=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp486.f1)->f1;_LL25E:
# 2302
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp488);
elt_type=_tmp488;
{void**_tmp8CF;*t=((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF[0]=elt_type,_tmp8CF))));}
num_elts=_tmp482;
one_elt=0;
goto _LL25C;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp486.f2)->tag == 16){_LL25F: _tmp487=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp486.f2)->f1;_LL260:
# 2309
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp487);
elt_type=_tmp487;
{void**_tmp8D0;*t=((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=elt_type,_tmp8D0))));}
num_elts=_tmp481;
one_elt=0;
goto _LL25C;}else{_LL261: _LL262:
 goto No_sizeof;}}_LL25C:;}
# 2317
goto _LL255;}else{goto _LL25A;}}else{goto _LL25A;}}else{goto _LL25A;}}else{goto _LL25A;}default: _LL25A: _LL25B:
# 2319
 No_sizeof: {
# 2322
struct Cyc_Absyn_Exp*_tmp48C=*e;
{void*_tmp48D=_tmp48C->r;void*_tmp48E=_tmp48D;struct Cyc_Absyn_Exp*_tmp48F;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp48E)->tag == 13){_LL264: _tmp48F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp48E)->f2;_LL265:
 _tmp48C=_tmp48F;goto _LL263;}else{_LL266: _LL267:
 goto _LL263;}_LL263:;}
# 2327
{void*_tmp490=Cyc_Tcutil_compress((void*)_check_null(_tmp48C->topt));void*_tmp491=_tmp490;void*_tmp492;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp491)->tag == 19){_LL269: _tmp492=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp491)->f1;_LL26A:
# 2329
{void*_tmp493=Cyc_Tcutil_compress(_tmp492);void*_tmp494=_tmp493;struct Cyc_Absyn_Exp*_tmp495;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp494)->tag == 18){_LL26E: _tmp495=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp494)->f1;_LL26F:
# 2331
 er=_tmp495->r;goto retry_sizeof;}else{_LL270: _LL271:
 goto _LL26D;}_LL26D:;}
# 2334
goto _LL268;}else{_LL26B: _LL26C:
 goto _LL268;}_LL268:;}
# 2337
elt_type=Cyc_Absyn_char_typ;
{void**_tmp8D1;*t=((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1[0]=elt_type,_tmp8D1))));}
num_elts=*e;
one_elt=0;}
# 2342
goto _LL255;}_LL255:;}}
# 2346
*is_fat=!one_elt;
# 2349
{void*_tmp497=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp498;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp497)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp497)->f1).aggr_info).KnownAggr).tag == 2){_LL273: _tmp498=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp497)->f1).aggr_info).KnownAggr).val;_LL274:
# 2351
 if(_tmp498->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp498->impl))->exist_vars != 0){
const char*_tmp8D4;void*_tmp8D3;(_tmp8D3=0,Cyc_Tcutil_terr(loc,((_tmp8D4="malloc with existential types not yet implemented",_tag_dyneither(_tmp8D4,sizeof(char),50))),_tag_dyneither(_tmp8D3,sizeof(void*),0)));}
goto _LL272;}else{goto _LL275;}}else{_LL275: _LL276:
 goto _LL272;}_LL272:;}{
# 2358
void*(*_tmp49B)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp49C=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp49D=Cyc_Tcutil_compress(*topt);void*_tmp49E=_tmp49D;union Cyc_Absyn_Constraint*_tmp49F;union Cyc_Absyn_Constraint*_tmp4A0;union Cyc_Absyn_Constraint*_tmp4A1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->tag == 5){_LL278: _tmp49F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).ptr_atts).nullable;_tmp4A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).ptr_atts).bounds;_tmp4A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).ptr_atts).zero_term;_LL279:
# 2363
 _tmp49C=_tmp4A1;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp49F))
_tmp49B=Cyc_Absyn_star_typ;
# 2368
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A1) && !(*is_calloc)){
{const char*_tmp8D7;void*_tmp8D6;(_tmp8D6=0,Cyc_Tcutil_warn(loc,((_tmp8D7="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp8D7,sizeof(char),55))),_tag_dyneither(_tmp8D6,sizeof(void*),0)));}
*is_calloc=1;}
# 2374
{void*_tmp4A4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4A0);void*_tmp4A5=_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4A5)->tag == 0){_LL27D: _LL27E:
 goto _LL27C;}else{_LL27F: _tmp4A6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4A5)->f1;if(!one_elt){_LL280: {
# 2377
int _tmp4A7=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp4A7  && Cyc_Evexp_same_const_exp(_tmp4A6,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4A0),_tmp49C);}
# 2383
{void*_tmp4A8=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp4A9=_tmp4A8;void*_tmp4AA;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4A9)->tag == 19){_LL284: _tmp4AA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4A9)->f1;_LL285: {
# 2385
struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4AA,0),0,Cyc_Absyn_No_coercion,0);
# 2387
if(Cyc_Evexp_same_const_exp(_tmp4AB,_tmp4A6)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4A0),_tmp49C);}
# 2392
goto _LL283;}}else{_LL286: _LL287:
 goto _LL283;}_LL283:;}
# 2395
goto _LL27C;}}else{_LL281: _LL282:
 goto _LL27C;}}_LL27C:;}
# 2398
goto _LL277;}else{_LL27A: _LL27B:
 goto _LL277;}_LL277:;}
# 2401
if(!one_elt)_tmp49B=Cyc_Absyn_dyneither_typ;
return _tmp49B(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp49C);};};}
# 2406
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2412
struct _RegionHandle _tmp4AC=_new_region("r");struct _RegionHandle*r=& _tmp4AC;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp4AD=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExpNoPromote(_tmp4AD,0,e1);{
void*_tmp4AE=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp4AD,& _tmp4AE,e2);};}{
# 2418
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2421
{void*_tmp4AF=Cyc_Tcutil_compress(t1);void*_tmp4B0=_tmp4AF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B0)->tag == 8){_LL289: _LL28A:
{const char*_tmp8DA;void*_tmp8D9;(_tmp8D9=0,Cyc_Tcutil_terr(loc,((_tmp8DA="cannot assign to an array",_tag_dyneither(_tmp8DA,sizeof(char),26))),_tag_dyneither(_tmp8D9,sizeof(void*),0)));}goto _LL288;}else{_LL28B: _LL28C:
 goto _LL288;}_LL288:;}
# 2426
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1)){
const char*_tmp8DD;void*_tmp8DC;(_tmp8DC=0,Cyc_Tcutil_terr(loc,((_tmp8DD="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp8DD,sizeof(char),58))),_tag_dyneither(_tmp8DC,sizeof(void*),0)));}
# 2430
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp8E0;void*_tmp8DF;void*_tmp4B7=(_tmp8DF=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp8E0="swap non-lvalue",_tag_dyneither(_tmp8E0,sizeof(char),16))),_tag_dyneither(_tmp8DF,sizeof(void*),0)));_npop_handler(0);return _tmp4B7;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp8E3;void*_tmp8E2;void*_tmp4BA=(_tmp8E2=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp8E3="swap non-lvalue",_tag_dyneither(_tmp8E3,sizeof(char),16))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));_npop_handler(0);return _tmp4BA;}{
# 2435
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp8E6;void*_tmp8E5;void*_tmp4BD=(_tmp8E5=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp8E6="swap value in zeroterm array",_tag_dyneither(_tmp8E6,sizeof(char),29))),_tag_dyneither(_tmp8E5,sizeof(void*),0)));_npop_handler(0);return _tmp4BD;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp8E9;void*_tmp8E8;void*_tmp4C0=(_tmp8E8=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp8E9="swap value in zeroterm array",_tag_dyneither(_tmp8E9,sizeof(char),29))),_tag_dyneither(_tmp8E8,sizeof(void*),0)));_npop_handler(0);return _tmp4C0;}
# 2442
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp8EE;void*_tmp8ED[2];struct Cyc_String_pa_PrintArg_struct _tmp8EC;struct Cyc_String_pa_PrintArg_struct _tmp8EB;void*_tmp4C1=(_tmp8EB.tag=0,((_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp8EC.tag=0,((_tmp8EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8ED[0]=& _tmp8EC,((_tmp8ED[1]=& _tmp8EB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8EE="type mismatch: %s != %s",_tag_dyneither(_tmp8EE,sizeof(char),24))),_tag_dyneither(_tmp8ED,sizeof(void*),2)))))))))))));
void*_tmp4C2=_tmp4C1;_npop_handler(0);return _tmp4C2;}{
# 2449
void*_tmp4C7=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp4C7;};};};
# 2412
;_pop_region(r);}
# 2453
int Cyc_Tcexp_in_stmt_exp=0;
# 2455
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp4C8=_new_region("r");struct _RegionHandle*r=& _tmp4C8;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2462
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2464
while(1){
void*_tmp4C9=s->r;void*_tmp4CA=_tmp4C9;struct Cyc_Absyn_Decl*_tmp4CB;struct Cyc_Absyn_Stmt*_tmp4CC;struct Cyc_Absyn_Stmt*_tmp4CD;struct Cyc_Absyn_Stmt*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CF;switch(*((int*)_tmp4CA)){case 1: _LL28E: _tmp4CF=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4CA)->f1;_LL28F: {
# 2468
void*_tmp4D0=(void*)_check_null(_tmp4CF->topt);
if(!Cyc_Tcutil_unify(_tmp4D0,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp8F2;void*_tmp8F1[1];struct Cyc_String_pa_PrintArg_struct _tmp8F0;(_tmp8F0.tag=0,((_tmp8F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp4D0)),((_tmp8F1[0]=& _tmp8F0,Cyc_Tcutil_terr(loc,((_tmp8F2="statement expression returns type %s",_tag_dyneither(_tmp8F2,sizeof(char),37))),_tag_dyneither(_tmp8F1,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2474
void*_tmp4D4=_tmp4D0;_npop_handler(0);return _tmp4D4;};}case 2: _LL290: _tmp4CD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4CA)->f1;_tmp4CE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4CA)->f2;_LL291:
 s=_tmp4CE;continue;case 12: _LL292: _tmp4CB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4CA)->f1;_tmp4CC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4CA)->f2;_LL293:
 s=_tmp4CC;continue;default: _LL294: _LL295: {
# 2478
const char*_tmp8F5;void*_tmp8F4;void*_tmp4D7=(_tmp8F4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F5="statement expression must end with expression",_tag_dyneither(_tmp8F5,sizeof(char),46))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));_npop_handler(0);return _tmp4D7;}}_LL28D:;}
# 2456
;_pop_region(r);}
# 2483
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp4D8=_new_region("r");struct _RegionHandle*r=& _tmp4D8;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2488
{void*_tmp4D9=t;struct Cyc_Absyn_Aggrdecl*_tmp4DA;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D9)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D9)->f1).aggr_info).KnownAggr).tag == 2){_LL297: _tmp4DA=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D9)->f1).aggr_info).KnownAggr).val;_LL298:
# 2490
 if((_tmp4DA->kind == Cyc_Absyn_UnionA  && _tmp4DA->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DA->impl))->tagged)goto _LL296;
goto _LL29A;}else{goto _LL299;}}else{_LL299: _LL29A:
# 2493
{const char*_tmp8F9;void*_tmp8F8[1];struct Cyc_String_pa_PrintArg_struct _tmp8F7;(_tmp8F7.tag=0,((_tmp8F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8F8[0]=& _tmp8F7,Cyc_Tcutil_terr(loc,((_tmp8F9="expecting @tagged union but found %s",_tag_dyneither(_tmp8F9,sizeof(char),37))),_tag_dyneither(_tmp8F8,sizeof(void*),1)))))));}
goto _LL296;}_LL296:;}{
# 2496
void*_tmp4DE=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp4DE;};
# 2485
;_pop_region(r);}
# 2500
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2504
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp4DF=_new_region("r");struct _RegionHandle*r=& _tmp4DF;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp4E0=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2509
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp8FC;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8FB;void*expected_type=(void*)(
(_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=15,((_tmp8FC.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp4E0)),_tmp8FC)))),_tmp8FB))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp4E0,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp4E1=Cyc_Tcutil_compress(handle_type);void*_tmp4E2=_tmp4E1;void*_tmp4E3;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E2)->tag == 15){_LL29C: _tmp4E3=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E2)->f1;_LL29D:
# 2514
 rgn=_tmp4E3;
Cyc_Tcenv_check_rgn_accessible(_tmp4E0,loc,rgn);
goto _LL29B;}else{_LL29E: _LL29F:
# 2518
{const char*_tmp900;void*_tmp8FF[1];struct Cyc_String_pa_PrintArg_struct _tmp8FE;(_tmp8FE.tag=0,((_tmp8FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp8FF[0]=& _tmp8FE,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp900="expecting region_t type but found %s",_tag_dyneither(_tmp900,sizeof(char),37))),_tag_dyneither(_tmp8FF,sizeof(void*),1)))))));}
goto _LL29B;}_LL29B:;}else{
# 2525
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2533
void*_tmp4E9=e1->r;void*_tmp4EA=_tmp4E9;struct Cyc_List_List*_tmp4EB;struct Cyc_Core_Opt*_tmp4EC;struct Cyc_List_List*_tmp4ED;switch(*((int*)_tmp4EA)){case 26: _LL2A1: _LL2A2:
 goto _LL2A4;case 27: _LL2A3: _LL2A4: {
# 2538
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp4E0,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmp903;void*_tmp902;(_tmp902=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp903="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp903,sizeof(char),45))),_tag_dyneither(_tmp902,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp4E0,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp4E0,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2549
void*_tmp4F0=res_typ;_npop_handler(0);return _tmp4F0;};}case 35: _LL2A5: _tmp4EC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4EA)->f1;_tmp4ED=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4EA)->f2;_LL2A6:
# 2551
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp906;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp905;e1->r=(void*)((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905[0]=((_tmp906.tag=25,((_tmp906.f1=_tmp4ED,_tmp906)))),_tmp905))));}
_tmp4EB=_tmp4ED;goto _LL2A8;case 25: _LL2A7: _tmp4EB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4EA)->f1;_LL2A8: {
# 2554
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4F3=Cyc_Tcutil_compress(*topt);void*_tmp4F4=_tmp4F3;void**_tmp4F5;struct Cyc_Absyn_Tqual _tmp4F6;union Cyc_Absyn_Constraint*_tmp4F7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F4)->tag == 5){_LL2B0: _tmp4F5=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F4)->f1).elt_typ;_tmp4F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F4)->f1).elt_tq;_tmp4F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F4)->f1).ptr_atts).zero_term;_LL2B1:
# 2560
 elt_typ_opt=_tmp4F5;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F7);
goto _LL2AF;}else{_LL2B2: _LL2B3:
 goto _LL2AF;}_LL2AF:;}{
# 2566
void*res_typ=Cyc_Tcexp_tcArray(_tmp4E0,e1->loc,elt_typ_opt,0,zero_term,_tmp4EB);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmp909;void*_tmp908;(_tmp908=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp909="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp909,sizeof(char),50))),_tag_dyneither(_tmp908,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2575
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp4E0,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp4E0,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2581
void*_tmp4FA=res_typ;_npop_handler(0);return _tmp4FA;};};}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4EA)->f1).Wstring_c).tag){case 8: _LL2A9: _LL2AA: {
# 2586
void*_tmp4FB=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2588
void*_tmp4FC=Cyc_Tcexp_tcExp(_tmp4E0,& _tmp4FB,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp90C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp90B;void*_tmp4FF=Cyc_Absyn_atb_typ(_tmp4FC,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90C.tag=1,((_tmp90C.f1=Cyc_Absyn_uint_exp(1,0),_tmp90C)))),_tmp90B)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp4FF;}case 9: _LL2AB: _LL2AC: {
# 2593
void*_tmp500=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2595
void*_tmp501=Cyc_Tcexp_tcExp(_tmp4E0,& _tmp500,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp90F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp90E;void*_tmp504=Cyc_Absyn_atb_typ(_tmp501,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=1,((_tmp90F.f1=Cyc_Absyn_uint_exp(1,0),_tmp90F)))),_tmp90E)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp504;}default: goto _LL2AD;}default: _LL2AD: _LL2AE:
# 2602
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp505=Cyc_Tcutil_compress(*topt);void*_tmp506=_tmp505;void**_tmp507;struct Cyc_Absyn_Tqual _tmp508;switch(*((int*)_tmp506)){case 5: _LL2B5: _tmp507=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp506)->f1).elt_typ;_tmp508=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp506)->f1).elt_tq;_LL2B6:
# 2608
 topt2=_tmp507;goto _LL2B4;case 3: _LL2B7: _LL2B8:
# 2612
 bogus=*topt;
topt2=& bogus;
goto _LL2B4;default: _LL2B9: _LL2BA:
 goto _LL2B4;}_LL2B4:;}{
# 2618
void*telt=Cyc_Tcexp_tcExp(_tmp4E0,topt2,e1);
# 2620
struct _RegionHandle _tmp509=_new_region("temp");struct _RegionHandle*temp=& _tmp509;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1)){
const char*_tmp912;void*_tmp911;(_tmp911=0,Cyc_Tcutil_terr(e1->loc,((_tmp912="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp912,sizeof(char),49))),_tag_dyneither(_tmp911,sizeof(void*),0)));}{
# 2625
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp918;struct Cyc_Absyn_PtrInfo _tmp917;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp916;void*res_typ=(void*)(
(_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916[0]=((_tmp918.tag=5,((_tmp918.f1=((_tmp917.elt_typ=telt,((_tmp917.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp917.ptr_atts=(
((_tmp917.ptr_atts).rgn=rgn,(((_tmp917.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp917.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp917.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp917.ptr_atts).ptrloc=0,_tmp917.ptr_atts)))))))))),_tmp917)))))),_tmp918)))),_tmp916))));
# 2630
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp4E0,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp4E0,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2637
void*_tmp50C=res_typ;_npop_handler(1);return _tmp50C;};};
# 2620
;_pop_region(temp);};}}_LL2A0:;};}
# 2506
;_pop_region(r);}
# 2644
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
e->topt=(t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0));
return t;}
# 2654
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2658
struct _RegionHandle _tmp510=_new_region("temp");struct _RegionHandle*temp=& _tmp510;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2664
const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_Tcutil_terr(e->loc,((_tmp91B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp91B,sizeof(char),49))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}{
# 2667
void*_tmp513=e->r;void*_tmp514=_tmp513;switch(*((int*)_tmp514)){case 25: _LL2BC: _LL2BD:
 goto _LL2BF;case 26: _LL2BE: _LL2BF:
 goto _LL2C1;case 27: _LL2C0: _LL2C1:
 goto _LL2C3;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp514)->f1).String_c).tag){case 9: _LL2C2: _LL2C3:
 goto _LL2C5;case 8: _LL2C4: _LL2C5: {
void*_tmp515=t;_npop_handler(0);return _tmp515;}default: goto _LL2C6;}default: _LL2C6: _LL2C7:
# 2674
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2677
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}{
# 2680
void*_tmp516=t;_npop_handler(0);return _tmp516;};}_LL2BB:;};
# 2658
;_pop_region(temp);}
# 2691 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp517=e->r;void*_tmp518=_tmp517;struct Cyc_Absyn_Exp*_tmp519;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp518)->tag == 11){_LL2C9: _tmp519=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LL2CA:
# 2695
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp519);
_tmp519->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp519->topt),0);
e->topt=_tmp519->topt;
goto _LL2C8;}else{_LL2CB: _LL2CC:
# 2701
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2704
{void*_tmp51A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp51B=_tmp51A;void*_tmp51C;struct Cyc_Absyn_Tqual _tmp51D;void*_tmp51E;union Cyc_Absyn_Constraint*_tmp51F;union Cyc_Absyn_Constraint*_tmp520;union Cyc_Absyn_Constraint*_tmp521;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51B)->tag == 5){_LL2CE: _tmp51C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51B)->f1).elt_typ;_tmp51D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51B)->f1).elt_tq;_tmp51E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51B)->f1).ptr_atts).rgn;_tmp51F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51B)->f1).ptr_atts).nullable;_tmp520=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51B)->f1).ptr_atts).bounds;_tmp521=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51B)->f1).ptr_atts).zero_term;_LL2CF:
# 2706
{void*_tmp522=Cyc_Tcutil_compress(_tmp51C);void*_tmp523=_tmp522;struct Cyc_List_List*_tmp524;void*_tmp525;struct Cyc_Absyn_Tqual _tmp526;void*_tmp527;struct Cyc_List_List*_tmp528;int _tmp529;struct Cyc_Absyn_VarargInfo*_tmp52A;struct Cyc_List_List*_tmp52B;struct Cyc_List_List*_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_List_List*_tmp52E;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_List_List*_tmp530;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->tag == 9){_LL2D3: _tmp524=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).tvars;_tmp525=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).effect;_tmp526=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).ret_tqual;_tmp527=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).ret_typ;_tmp528=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).args;_tmp529=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).c_varargs;_tmp52A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).cyc_varargs;_tmp52B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).rgn_po;_tmp52C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).attributes;_tmp52D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).requires_clause;_tmp52E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).requires_relns;_tmp52F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).ensures_clause;_tmp530=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp523)->f1).ensures_relns;_LL2D4:
# 2708
 if(_tmp524 != 0){
struct _RegionHandle _tmp531=_new_region("rgn");struct _RegionHandle*rgn=& _tmp531;_push_region(rgn);
{struct _tuple12 _tmp91C;struct _tuple12 _tmp532=(_tmp91C.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp91C.f2=rgn,_tmp91C)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp532,_tmp524);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2717
_tmp52B=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp52B);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp52B);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp922;struct Cyc_Absyn_FnInfo _tmp921;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp920;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp920=_cycalloc(sizeof(*_tmp920)),((_tmp920[0]=((_tmp922.tag=9,((_tmp922.f1=((_tmp921.tvars=0,((_tmp921.effect=_tmp525,((_tmp921.ret_tqual=_tmp526,((_tmp921.ret_typ=_tmp527,((_tmp921.args=_tmp528,((_tmp921.c_varargs=_tmp529,((_tmp921.cyc_varargs=_tmp52A,((_tmp921.rgn_po=0,((_tmp921.attributes=_tmp52C,((_tmp921.requires_clause=_tmp52D,((_tmp921.requires_relns=_tmp52E,((_tmp921.ensures_clause=_tmp52F,((_tmp921.ensures_relns=_tmp530,_tmp921)))))))))))))))))))))))))),_tmp922)))),_tmp920)))));
# 2724
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp928;struct Cyc_Absyn_PtrInfo _tmp927;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp926;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp533=(_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926[0]=((_tmp928.tag=5,((_tmp928.f1=((_tmp927.elt_typ=ftyp,((_tmp927.elt_tq=_tmp51D,((_tmp927.ptr_atts=(((_tmp927.ptr_atts).rgn=_tmp51E,(((_tmp927.ptr_atts).nullable=_tmp51F,(((_tmp927.ptr_atts).bounds=_tmp520,(((_tmp927.ptr_atts).zero_term=_tmp521,(((_tmp927.ptr_atts).ptrloc=0,_tmp927.ptr_atts)))))))))),_tmp927)))))),_tmp928)))),_tmp926)));
# 2726
struct Cyc_Absyn_Exp*_tmp534=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp92B;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp92A;e->r=(void*)((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92B.tag=12,((_tmp92B.f1=_tmp534,((_tmp92B.f2=ts,_tmp92B)))))),_tmp92A))));}
e->topt=(void*)_tmp533;};}
# 2710
;_pop_region(rgn);}
# 2731
goto _LL2D2;}else{_LL2D5: _LL2D6:
 goto _LL2D2;}_LL2D2:;}
# 2734
goto _LL2CD;}else{_LL2D0: _LL2D1:
 goto _LL2CD;}_LL2CD:;}
# 2737
goto _LL2C8;}_LL2C8:;}
# 2739
return(void*)_check_null(e->topt);}
# 2747
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2749
struct _RegionHandle _tmp53E=_new_region("r");struct _RegionHandle*r=& _tmp53E;_push_region(r);
{struct Cyc_List_List*_tmp53F=0;
# 2752
{void*_tmp540=fn_exp->r;void*_tmp541=_tmp540;struct Cyc_List_List*_tmp542;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp541)->tag == 9){_LL2D8: _tmp542=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_LL2D9:
# 2754
{void*_tmp543=e->r;void*_tmp544=_tmp543;struct Cyc_List_List*_tmp545;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp544)->tag == 9){_LL2DD: _tmp545=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL2DE: {
# 2756
struct Cyc_List_List*_tmp546=alias_arg_exps;
int _tmp547=0;
while(_tmp546 != 0){
while(_tmp547 != (int)_tmp546->hd){
if(_tmp542 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmp935;struct Cyc_Int_pa_PrintArg_struct _tmp934;void*_tmp933[2];const char*_tmp932;void*_tmp931;(_tmp931=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp935.tag=1,((_tmp935.f1=(unsigned long)((int)_tmp546->hd),((_tmp934.tag=1,((_tmp934.f1=(unsigned long)_tmp547,((_tmp933[0]=& _tmp934,((_tmp933[1]=& _tmp935,Cyc_aprintf(((_tmp932="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp932,sizeof(char),36))),_tag_dyneither(_tmp933,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp931,sizeof(void*),0)));}
# 2763
++ _tmp547;
_tmp542=_tmp542->tl;
_tmp545=((struct Cyc_List_List*)_check_null(_tmp545))->tl;}{
# 2768
struct _tuple11 _tmp54D=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp542))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp545))->hd)->topt)));struct _tuple11 _tmp54E=_tmp54D;struct Cyc_Absyn_Decl*_tmp54F;struct Cyc_Absyn_Exp*_tmp550;_LL2E2: _tmp54F=_tmp54E.f1;_tmp550=_tmp54E.f2;_LL2E3:;
_tmp542->hd=(void*)_tmp550;
{struct Cyc_List_List*_tmp936;_tmp53F=((_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->hd=_tmp54F,((_tmp936->tl=_tmp53F,_tmp936))))));}
_tmp546=_tmp546->tl;};}
# 2773
goto _LL2DC;}}else{_LL2DF: _LL2E0: {
const char*_tmp939;void*_tmp938;(_tmp938=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp939="not a function call!",_tag_dyneither(_tmp939,sizeof(char),21))),_tag_dyneither(_tmp938,sizeof(void*),0)));}}_LL2DC:;}
# 2776
goto _LL2D7;}else{_LL2DA: _LL2DB: {
const char*_tmp93C;void*_tmp93B;(_tmp93B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp93C="not a function call!",_tag_dyneither(_tmp93C,sizeof(char),21))),_tag_dyneither(_tmp93B,sizeof(void*),0)));}}_LL2D7:;}
# 2781
while(_tmp53F != 0){
struct Cyc_Absyn_Decl*_tmp556=(struct Cyc_Absyn_Decl*)_tmp53F->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp556,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp53F=_tmp53F->tl;}
# 2788
e->topt=0;
e->r=fn_exp->r;}
# 2750
;_pop_region(r);}
# 2795
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2799
{void*_tmp557=e->r;void*_tmp558=_tmp557;void*_tmp559;struct Cyc_Absyn_Exp*_tmp55A;struct _dyneither_ptr*_tmp55B;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55D;int*_tmp55E;struct Cyc_Absyn_Exp**_tmp55F;void***_tmp560;struct Cyc_Absyn_Exp**_tmp561;int*_tmp562;struct _tuple1**_tmp563;void*_tmp564;struct Cyc_Absyn_Enumfield*_tmp565;struct _tuple1**_tmp566;struct Cyc_Absyn_Enumdecl*_tmp567;struct Cyc_Absyn_Enumfield*_tmp568;struct Cyc_List_List*_tmp569;struct Cyc_Absyn_Datatypedecl*_tmp56A;struct Cyc_Absyn_Datatypefield*_tmp56B;void*_tmp56C;struct Cyc_List_List*_tmp56D;struct _tuple1**_tmp56E;struct Cyc_List_List**_tmp56F;struct Cyc_List_List*_tmp570;struct Cyc_Absyn_Aggrdecl**_tmp571;struct Cyc_Absyn_Exp*_tmp572;void*_tmp573;int*_tmp574;struct Cyc_Absyn_Vardecl*_tmp575;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp577;int*_tmp578;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_List_List*_tmp57A;struct _tuple9*_tmp57B;struct Cyc_List_List*_tmp57C;struct Cyc_List_List*_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Exp*_tmp57F;struct Cyc_Absyn_Exp*_tmp580;struct _dyneither_ptr*_tmp581;int*_tmp582;int*_tmp583;struct Cyc_Absyn_Exp*_tmp584;struct _dyneither_ptr*_tmp585;int*_tmp586;int*_tmp587;struct Cyc_Absyn_Exp*_tmp588;void*_tmp589;struct Cyc_List_List*_tmp58A;void*_tmp58B;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Exp**_tmp58D;struct Cyc_Absyn_Exp*_tmp58E;struct Cyc_Absyn_Exp*_tmp58F;void*_tmp590;struct Cyc_Absyn_Exp*_tmp591;enum Cyc_Absyn_Coercion*_tmp592;struct Cyc_Absyn_Exp*_tmp593;struct Cyc_List_List*_tmp594;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_Absyn_Exp*_tmp596;struct Cyc_Absyn_Exp*_tmp597;struct Cyc_Absyn_Exp*_tmp598;struct Cyc_Absyn_Exp*_tmp599;struct Cyc_Absyn_Exp*_tmp59A;struct Cyc_Absyn_Exp*_tmp59B;struct Cyc_Absyn_Exp*_tmp59C;struct Cyc_Absyn_Exp*_tmp59D;struct Cyc_Absyn_Exp*_tmp59E;struct Cyc_Absyn_Exp*_tmp59F;struct Cyc_Core_Opt*_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A2;enum Cyc_Absyn_Incrementor _tmp5A3;enum Cyc_Absyn_Primop _tmp5A4;struct Cyc_List_List*_tmp5A5;struct _tuple1*_tmp5A6;void**_tmp5A7;union Cyc_Absyn_Cnst*_tmp5A8;struct Cyc_Core_Opt*_tmp5A9;struct Cyc_List_List*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_List_List*_tmp5AC;struct Cyc_Absyn_VarargCallInfo**_tmp5AD;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_Absyn_VarargCallInfo**_tmp5B0;int*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B2;switch(*((int*)_tmp558)){case 11: _LL2E5: _tmp5B2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL2E6:
# 2804
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5B2);
return;case 9: _LL2E7: _tmp5AE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp5AF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp5B0=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_tmp5B1=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp558)->f4;if(!(*_tmp5B1)){_LL2E8:
# 2810
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5AE,_tmp5AF)){
*_tmp5B1=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2814
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2817
return;}else{_LL2FD: _tmp5AB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp5AC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp5AD=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_LL2FE: {
# 2848
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp5B3;_push_handler(& _tmp5B3);{int _tmp5B5=0;if(setjmp(_tmp5B3.handler))_tmp5B5=1;if(!_tmp5B5){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp5B4=(void*)_exn_thrown;void*_tmp5B6=_tmp5B4;void*_tmp5B7;struct _dyneither_ptr _tmp5B8;if(((struct Cyc_Core_Failure_exn_struct*)_tmp5B6)->tag == Cyc_Core_Failure){_LL338: _tmp5B8=((struct Cyc_Core_Failure_exn_struct*)_tmp5B6)->f1;_LL339:
# 2855
 ok=0;
fn_exp=e;
goto _LL337;}else{_LL33A: _tmp5B7=_tmp5B6;_LL33B:(int)_rethrow(_tmp5B7);}_LL337:;}};}
# 2859
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5AB,_tmp5AC,_tmp5AD,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2866
goto _LL2E4;}}case 35: _LL2E9: _tmp5A9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp5AA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL2EA:
# 2821
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5AA);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0: _LL2EB: _tmp5A8=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL2EC:
# 2826
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5A8,e);goto _LL2E4;case 1: _LL2ED: _tmp5A6=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp5A7=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL2EE:
# 2828
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5A6,_tmp5A7);goto _LL2E4;case 2: _LL2EF: _tmp5A4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp5A5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL2F0:
# 2830
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5A4,_tmp5A5);goto _LL2E4;case 4: _LL2F1: _tmp5A2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp5A3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL2F2:
# 2832
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5A2,_tmp5A3);goto _LL2E4;case 3: _LL2F3: _tmp59F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp5A0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp5A1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_LL2F4:
# 2834
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp59F,_tmp5A0,_tmp5A1);goto _LL2E4;case 5: _LL2F5: _tmp59C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp59D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp59E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_LL2F6:
# 2836
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp59C,_tmp59D,_tmp59E);goto _LL2E4;case 6: _LL2F7: _tmp59A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp59B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL2F8:
# 2838
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp59A,_tmp59B);goto _LL2E4;case 7: _LL2F9: _tmp598=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp599=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL2FA:
# 2840
 t=Cyc_Tcexp_tcOr(te,loc,_tmp598,_tmp599);goto _LL2E4;case 8: _LL2FB: _tmp596=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp597=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL2FC:
# 2842
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp596,_tmp597);goto _LL2E4;case 10: _LL2FF: _tmp595=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL300:
# 2868
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp595);goto _LL2E4;case 12: _LL301: _tmp593=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp594=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL302:
# 2870
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp593,_tmp594);goto _LL2E4;case 13: _LL303: _tmp590=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp591=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp592=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp558)->f4;_LL304:
# 2872
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp590,_tmp591,_tmp592);goto _LL2E4;case 14: _LL305: _tmp58F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL306:
# 2874
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp58F);goto _LL2E4;case 15: _LL307: _tmp58D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp58E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL308:
# 2876
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp58D,e,_tmp58E);goto _LL2E4;case 17: _LL309: _tmp58C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL30A: {
# 2878
void*_tmp5B9=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp58C);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5B9);goto _LL2E4;}case 16: _LL30B: _tmp58B=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL30C:
# 2881
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp58B);goto _LL2E4;case 18: _LL30D: _tmp589=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp58A=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL30E:
# 2883
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp589,_tmp58A);goto _LL2E4;case 19: _LL30F: _tmp588=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL310:
# 2885
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp588);goto _LL2E4;case 20: _LL311: _tmp584=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp585=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp586=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_tmp587=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp558)->f4;_LL312:
# 2887
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp584,_tmp585,_tmp586,_tmp587);goto _LL2E4;case 21: _LL313: _tmp580=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp581=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp582=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_tmp583=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp558)->f4;_LL314:
# 2889
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp580,_tmp581,_tmp582,_tmp583);goto _LL2E4;case 22: _LL315: _tmp57E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp57F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL316:
# 2891
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp57E,_tmp57F);goto _LL2E4;case 23: _LL317: _tmp57D=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL318:
# 2893
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp57D);goto _LL2E4;case 24: _LL319: _tmp57B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp57C=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL31A:
# 2895
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp57B,_tmp57C);goto _LL2E4;case 25: _LL31B: _tmp57A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL31C: {
# 2899
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp5BA=Cyc_Tcutil_compress(*topt);void*_tmp5BB=_tmp5BA;void**_tmp5BC;struct Cyc_Absyn_Tqual*_tmp5BD;union Cyc_Absyn_Constraint*_tmp5BE;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5BB)->tag == 8){_LL33D: _tmp5BC=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5BB)->f1).elt_type;_tmp5BD=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5BB)->f1).tq;_tmp5BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5BB)->f1).zero_term;_LL33E:
# 2905
 elt_topt=_tmp5BC;
elt_tqopt=_tmp5BD;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5BE);
goto _LL33C;}else{_LL33F: _LL340:
 goto _LL33C;}_LL33C:;}
# 2912
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp57A);goto _LL2E4;}case 36: _LL31D: _tmp579=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL31E:
# 2914
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp579);goto _LL2E4;case 26: _LL31F: _tmp575=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp576=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp577=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_tmp578=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp558)->f4;_LL320:
# 2916
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp575,_tmp576,_tmp577,_tmp578);goto _LL2E4;case 27: _LL321: _tmp572=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp573=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp574=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_LL322:
# 2918
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp572,_tmp573,_tmp574);goto _LL2E4;case 28: _LL323: _tmp56E=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp56F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp570=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_tmp571=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp558)->f4;_LL324:
# 2920
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp56E,_tmp56F,_tmp570,_tmp571);goto _LL2E4;case 29: _LL325: _tmp56C=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp56D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL326:
# 2922
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp56C,_tmp56D);goto _LL2E4;case 30: _LL327: _tmp569=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp56A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp56B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_LL328:
# 2924
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp569,_tmp56A,_tmp56B);goto _LL2E4;case 31: _LL329: _tmp566=(struct _tuple1**)&((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp567=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp568=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_LL32A:
# 2926
*_tmp566=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp568))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp93F;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp93E;t=(void*)((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=13,((_tmp93F.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp567))->name,((_tmp93F.f2=_tmp567,_tmp93F)))))),_tmp93E))));}goto _LL2E4;case 32: _LL32B: _tmp563=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp564=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_tmp565=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp558)->f3;_LL32C:
# 2929
*_tmp563=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp565))->name;
t=_tmp564;goto _LL2E4;case 33: _LL32D: _tmp55E=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp558)->f1).is_calloc;_tmp55F=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp558)->f1).rgn;_tmp560=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp558)->f1).elt_type;_tmp561=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp558)->f1).num_elts;_tmp562=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp558)->f1).fat_result;_LL32E:
# 2932
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp55F,_tmp560,_tmp561,_tmp55E,_tmp562);goto _LL2E4;case 34: _LL32F: _tmp55C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp55D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL330:
# 2934
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp55C,_tmp55D);goto _LL2E4;case 37: _LL331: _tmp55A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_tmp55B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp558)->f2;_LL332:
# 2936
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp55A,_tmp55B);goto _LL2E4;case 38: _LL333: _tmp559=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp558)->f1;_LL334:
# 2938
 if(!te->allow_valueof){
const char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_Tcutil_terr(e->loc,((_tmp942="valueof(-) can only occur within types",_tag_dyneither(_tmp942,sizeof(char),39))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
# 2946
t=Cyc_Absyn_sint_typ;
goto _LL2E4;default: _LL335: _LL336:
# 2949
{const char*_tmp945;void*_tmp944;(_tmp944=0,Cyc_Tcutil_terr(e->loc,((_tmp945="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmp945,sizeof(char),50))),_tag_dyneither(_tmp944,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}_LL2E4:;}
# 2952
e->topt=t;}
