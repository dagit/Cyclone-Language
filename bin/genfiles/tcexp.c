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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 445 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 448
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 461
typedef void*Cyc_Absyn_funcparams_t;
# 464
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 528
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 544
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 546
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 554
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 561
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 579
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 946 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 948
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 952
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 955
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 958
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 960
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 971
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 973
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 976
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 978
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 980
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 990
void*Cyc_Absyn_exn_typ();
# 1001
extern void*Cyc_Absyn_bounds_one;
# 1006
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1009
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1012
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1017
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1028
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1081
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1083
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1091
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1095
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1101
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1105
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1116
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1177
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1179
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1182
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1184
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
{void*_tmp6A5;(_tmp6A5=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6A5,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp6A6;return(_tmp6A6=_cycalloc(sizeof(*_tmp6A6)),((_tmp6A6->f1=0,((_tmp6A6->f2=e,_tmp6A6)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;void*_tmp3=_tmp2;struct _tuple1*_tmp5;void**_tmp6;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3;if(_tmp4->tag != 1)goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(void**)((void**)& _tmp4->f2);}}_LL2: {
# 79
struct _handler_cons _tmp7;_push_handler(& _tmp7);{int _tmp9=0;if(setjmp(_tmp7.handler))_tmp9=1;if(!_tmp9){
{struct _RegionHandle _tmpA=_new_region("r");struct _RegionHandle*r=& _tmpA;_push_region(r);
{void*_tmpB=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp5,1);void*_tmpC=_tmpB;void*_tmp10;struct Cyc_Absyn_Datatypedecl*_tmp12;struct Cyc_Absyn_Datatypefield*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp15;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpD->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpD->f1);if(_tmpE->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp6AA;void*_tmp6A9[1];struct Cyc_String_pa_PrintArg_struct _tmp6A8;(_tmp6A8.tag=0,((_tmp6A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6A9[0]=& _tmp6A8,Cyc_Tcutil_terr(e1->loc,((_tmp6AA="undeclared identifier %s",_tag_dyneither(_tmp6AA,sizeof(char),25))),_tag_dyneither(_tmp6A9,sizeof(void*),1)))))));}{
int _tmp1B=0;_npop_handler(1);return _tmp1B;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpF->tag != 0)goto _LLA;else{_tmp10=(void*)_tmpF->f1;}}_LL9:
# 86
*_tmp6=_tmp10;{
int _tmp1C=1;_npop_handler(1);return _tmp1C;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp11=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp11->tag != 2)goto _LLC;else{_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;}}_LLB:
# 89
 if(_tmp13->typs == 0){
# 91
{const char*_tmp6AE;void*_tmp6AD[1];struct Cyc_String_pa_PrintArg_struct _tmp6AC;(_tmp6AC.tag=0,((_tmp6AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp6AD[0]=& _tmp6AC,Cyc_Tcutil_terr(e->loc,((_tmp6AE="%s is a constant, not a function",_tag_dyneither(_tmp6AE,sizeof(char),33))),_tag_dyneither(_tmp6AD,sizeof(void*),1)))))));}{
int _tmp20=0;_npop_handler(1);return _tmp20;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6B1;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6B0;e->r=(void*)((_tmp6B0=_cycalloc(sizeof(*_tmp6B0)),((_tmp6B0[0]=((_tmp6B1.tag=30,((_tmp6B1.f1=es,((_tmp6B1.f2=_tmp12,((_tmp6B1.f3=_tmp13,_tmp6B1)))))))),_tmp6B0))));}{
int _tmp23=1;_npop_handler(1);return _tmp23;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp14->tag != 1)goto _LLE;else{_tmp15=_tmp14->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp24=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6B4;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6B3;e->r=(void*)((_tmp6B3=_cycalloc(sizeof(*_tmp6B3)),((_tmp6B3[0]=((_tmp6B4.tag=28,((_tmp6B4.f1=_tmp15->name,((_tmp6B4.f2=0,((_tmp6B4.f3=_tmp24,((_tmp6B4.f4=_tmp15,_tmp6B4)))))))))),_tmp6B3))));}{
int _tmp27=1;_npop_handler(1);return _tmp27;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp16=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp16->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp17=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp17->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp6B8;void*_tmp6B7[1];struct Cyc_String_pa_PrintArg_struct _tmp6B6;(_tmp6B6.tag=0,((_tmp6B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6B7[0]=& _tmp6B6,Cyc_Tcutil_terr(e->loc,((_tmp6B8="%s is an enum constructor, not a function",_tag_dyneither(_tmp6B8,sizeof(char),42))),_tag_dyneither(_tmp6B7,sizeof(void*),1)))))));}{
int _tmp2B=0;_npop_handler(1);return _tmp2B;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp8=(void*)_exn_thrown;void*_tmp2D=_tmp8;void*_tmp2F;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp6BC;void*_tmp6BB[1];struct Cyc_String_pa_PrintArg_struct _tmp6BA;(_tmp6BA.tag=0,((_tmp6BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6BB[0]=& _tmp6BA,Cyc_Tcutil_terr(e1->loc,((_tmp6BC="undeclared identifier %s",_tag_dyneither(_tmp6BC,sizeof(char),25))),_tag_dyneither(_tmp6BB,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2F=_tmp2D;_LL16:(void)_rethrow(_tmp2F);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6BF;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6BE;e->r=(void*)((_tmp6BE=_cycalloc(sizeof(*_tmp6BE)),((_tmp6BE[0]=((_tmp6BF.tag=25,((_tmp6BF.f1=des,_tmp6BF)))),_tmp6BE))));}
return;}{
# 125
void*t=*topt;
void*_tmp35=Cyc_Tcutil_compress(t);void*_tmp36=_tmp35;union Cyc_Absyn_AggrInfoU _tmp38;void*_tmp3A;struct Cyc_Absyn_Tqual _tmp3B;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36;if(_tmp37->tag != 11)goto _LL1A;else{_tmp38=(_tmp37->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3D=_tmp38;struct Cyc_Absyn_Aggrdecl*_tmp3E;_LL21: if((_tmp3D.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp6C2;void*_tmp6C1;(_tmp6C1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6C2="struct type not properly set",_tag_dyneither(_tmp6C2,sizeof(char),29))),_tag_dyneither(_tmp6C1,sizeof(void*),0)));}_LL23: if((_tmp3D.KnownAggr).tag != 2)goto _LL20;_tmp3E=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3D.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6C5;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6C4;e->r=(void*)((_tmp6C4=_cycalloc(sizeof(*_tmp6C4)),((_tmp6C4[0]=((_tmp6C5.tag=28,((_tmp6C5.f1=_tmp3E->name,((_tmp6C5.f2=0,((_tmp6C5.f3=des,((_tmp6C5.f4=_tmp3E,_tmp6C5)))))))))),_tmp6C4))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36;if(_tmp39->tag != 8)goto _LL1C;else{_tmp3A=(_tmp39->f1).elt_type;_tmp3B=(_tmp39->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6C8;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6C7;e->r=(void*)((_tmp6C7=_cycalloc(sizeof(*_tmp6C7)),((_tmp6C7[0]=((_tmp6C8.tag=25,((_tmp6C8.f1=des,_tmp6C8)))),_tmp6C7))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36;if(_tmp3C->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6CB;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6CA;e->r=(void*)((_tmp6CA=_cycalloc(sizeof(*_tmp6CA)),((_tmp6CA[0]=((_tmp6CB.tag=29,((_tmp6CB.f1=t,((_tmp6CB.f2=des,_tmp6CB)))))),_tmp6CA))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6CE;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6CD;e->r=(void*)((_tmp6CD=_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CE.tag=25,((_tmp6CE.f1=des,_tmp6CE)))),_tmp6CD))));}goto _LL17;_LL17:;};}
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
const char*_tmp6D3;void*_tmp6D2[2];struct Cyc_String_pa_PrintArg_struct _tmp6D1;struct Cyc_String_pa_PrintArg_struct _tmp6D0;(_tmp6D0.tag=0,((_tmp6D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6D1.tag=0,((_tmp6D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6D2[0]=& _tmp6D1,((_tmp6D2[1]=& _tmp6D0,Cyc_Tcutil_terr(e->loc,((_tmp6D3="test of %s has type %s instead of integral or pointer type",_tag_dyneither(_tmp6D3,sizeof(char),59))),_tag_dyneither(_tmp6D2,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4D=e->r;void*_tmp4E=_tmp4D;enum Cyc_Absyn_Primop _tmp50;struct Cyc_List_List*_tmp51;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E;if(_tmp4F->tag != 2)goto _LL28;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL27:
# 163
 if(_tmp50 == Cyc_Absyn_Eq  || _tmp50 == Cyc_Absyn_Neq){
struct _tuple0 _tmp6D4;struct _tuple0 _tmp52=(_tmp6D4.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->topt),((_tmp6D4.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp51))->tl))->hd)->topt),_tmp6D4)));struct _tuple0 _tmp53=_tmp52;void*_tmp55;void*_tmp57;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp54=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f1;if(_tmp54->tag != 15)goto _LL2D;else{_tmp55=(void*)_tmp54->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp56=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f2;if(_tmp56->tag != 15)goto _LL2D;else{_tmp57=(void*)_tmp56->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp6D7;struct Cyc_Tcexp_TestEnv _tmp6D6;return(_tmp6D6.eq=((_tmp6D7=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6D7)),((_tmp6D7->f1=_tmp55,((_tmp6D7->f2=_tmp57,_tmp6D7)))))),((_tmp6D6.isTrue=_tmp50 == Cyc_Absyn_Eq,_tmp6D6)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp6D8;return(_tmp6D8.eq=0,((_tmp6D8.isTrue=0,_tmp6D8)));};}
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6DE;struct Cyc_Absyn_PtrInfo _tmp6DD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6DC;t=(void*)((_tmp6DC=_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DE.tag=5,((_tmp6DE.f1=((_tmp6DD.elt_typ=_tmp70,((_tmp6DD.elt_tq=_tmp71,((_tmp6DD.ptr_atts=(
((_tmp6DD.ptr_atts).rgn=_tmp72,(((_tmp6DD.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6DD.ptr_atts).bounds=_tmp74,(((_tmp6DD.ptr_atts).zero_term=_tmp75,(((_tmp6DD.ptr_atts).ptrloc=0,_tmp6DD.ptr_atts)))))))))),_tmp6DD)))))),_tmp6DE)))),_tmp6DC))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66;if(_tmp76->tag != 19)goto _LL53;else{_tmp77=(void*)_tmp76->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6E1;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6E0;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7D=(_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0[0]=((_tmp6E1.tag=18,((_tmp6E1.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp62,0),_tmp6E1)))),_tmp6E0)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6E4;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6E3;t=(void*)((_tmp6E3=_cycalloc(sizeof(*_tmp6E3)),((_tmp6E3[0]=((_tmp6E4.tag=19,((_tmp6E4.f1=(void*)_tmp7D,_tmp6E4)))),_tmp6E3))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6E7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp82=(_tmp6E6=_cycalloc(sizeof(*_tmp6E6)),((_tmp6E6[0]=((_tmp6E7.tag=1,((_tmp6E7.f1=elen,_tmp6E7)))),_tmp6E6)));
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6ED;struct Cyc_Absyn_PtrInfo _tmp6EC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6EB;return(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6ED.tag=5,((_tmp6ED.f1=((_tmp6EC.elt_typ=_tmp8D,((_tmp6EC.elt_tq=_tmp8E,((_tmp6EC.ptr_atts=(((_tmp6EC.ptr_atts).rgn=_tmp8F,(((_tmp6EC.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6EC.ptr_atts).bounds=_tmp91,(((_tmp6EC.ptr_atts).zero_term=_tmp92,(((_tmp6EC.ptr_atts).ptrloc=0,_tmp6EC.ptr_atts)))))))))),_tmp6EC)))))),_tmp6ED)))),_tmp6EB))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp96=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6FC;struct Cyc_Core_Opt*_tmp6FB;struct Cyc_Core_Opt*_tmp6FA;struct Cyc_Absyn_PtrInfo _tmp6F9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6F8;t=(void*)((_tmp6F8=_cycalloc(sizeof(*_tmp6F8)),((_tmp6F8[0]=((_tmp6FC.tag=5,((_tmp6FC.f1=((_tmp6F9.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6FA=_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA->v=_tmp96,_tmp6FA))))),((_tmp6F9.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6F9.ptr_atts=(
((_tmp6F9.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6FB=_cycalloc(sizeof(*_tmp6FB)),((_tmp6FB->v=_tmp96,_tmp6FB))))),(((_tmp6F9.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6F9.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6F9.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6F9.ptr_atts).ptrloc=0,_tmp6F9.ptr_atts)))))))))),_tmp6F9)))))),_tmp6FC)))),_tmp6F8))));}
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
const char*_tmp700;void*_tmp6FF[1];struct Cyc_String_pa_PrintArg_struct _tmp6FE;void*_tmpC0=(_tmp6FE.tag=0,((_tmp6FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6FF[0]=& _tmp6FE,Cyc_Tcexp_expr_err(te,loc,0,((_tmp700="undeclared identifier %s",_tag_dyneither(_tmp700,sizeof(char),25))),_tag_dyneither(_tmp6FF,sizeof(void*),1)))))));_npop_handler(1);return _tmpC0;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB1->tag != 0)goto _LL77;else{_tmpB2=(void*)_tmpB1->f1;}}_LL76:
# 348
*b=_tmpB2;{
void*_tmpC1=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpC1;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpB3=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB3->tag != 3)goto _LL79;else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp703;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp702;e->r=(void*)((_tmp702=_cycalloc(sizeof(*_tmp702)),((_tmp702[0]=((_tmp703.tag=31,((_tmp703.f1=_tmpB5->name,((_tmp703.f2=_tmpB4,((_tmp703.f3=_tmpB5,_tmp703)))))))),_tmp702))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp706;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp705;void*_tmpC6=(void*)((_tmp705=_cycalloc(sizeof(*_tmp705)),((_tmp705[0]=((_tmp706.tag=13,((_tmp706.f1=_tmpB4->name,((_tmp706.f2=_tmpB4,_tmp706)))))),_tmp705))));_npop_handler(1);return _tmpC6;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpB6=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB6->tag != 4)goto _LL7B;else{_tmpB7=(void*)_tmpB6->f1;_tmpB8=_tmpB6->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp709;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp708;e->r=(void*)((_tmp708=_cycalloc(sizeof(*_tmp708)),((_tmp708[0]=((_tmp709.tag=32,((_tmp709.f1=_tmpB8->name,((_tmp709.f2=_tmpB7,((_tmp709.f3=_tmpB8,_tmp709)))))))),_tmp708))));}{
void*_tmpC9=_tmpB7;_npop_handler(1);return _tmpC9;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB9=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB9->tag != 2)goto _LL7D;else{_tmpBA=_tmpB9->f1;_tmpBB=_tmpB9->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp70C;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp70B;e->r=(void*)((_tmp70B=_cycalloc(sizeof(*_tmp70B)),((_tmp70B[0]=((_tmp70C.tag=30,((_tmp70C.f1=0,((_tmp70C.f2=_tmpBA,((_tmp70C.f3=_tmpBB,_tmp70C)))))))),_tmp70B))));}{
void*_tmpCC=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpBA,_tmpBB);_npop_handler(1);return _tmpCC;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpBC=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpBC->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp710;void*_tmp70F[1];struct Cyc_String_pa_PrintArg_struct _tmp70E;void*_tmpD0=(_tmp70E.tag=0,((_tmp70E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp70F[0]=& _tmp70E,Cyc_Tcexp_expr_err(te,loc,0,((_tmp710="bad occurrence of type name %s",_tag_dyneither(_tmp710,sizeof(char),31))),_tag_dyneither(_tmp70F,sizeof(void*),1)))))));_npop_handler(1);return _tmpD0;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpAA=(void*)_exn_thrown;void*_tmpD2=_tmpAA;void*_tmpD4;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpD3=(struct Cyc_Dict_Absent_exn_struct*)_tmpD2;if(_tmpD3->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp714;void*_tmp713[1];struct Cyc_String_pa_PrintArg_struct _tmp712;return(_tmp712.tag=0,((_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp713[0]=& _tmp712,Cyc_Tcexp_expr_err(te,loc,0,((_tmp714="undeclared identifier %s",_tag_dyneither(_tmp714,sizeof(char),25))),_tag_dyneither(_tmp713,sizeof(void*),1)))))));}_LL82: _tmpD4=_tmpD2;_LL83:(void)_rethrow(_tmpD4);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp9D;if(_tmp9F->tag != 1)goto _LL6A;else{_tmpA0=_tmp9F->f1;}}_LL69:
# 368
*q=*_tmpA0->name;
return _tmpA0->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA1=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA1->tag != 2)goto _LL6C;else{_tmpA2=_tmpA1->f1;}}_LL6B:
*q=*_tmpA2->name;return Cyc_Tcutil_fndecl2typ(_tmpA2);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpA3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA3->tag != 5)goto _LL6E;else{_tmpA4=_tmpA3->f1;}}_LL6D:
 _tmpA6=_tmpA4;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA5->tag != 4)goto _LL70;else{_tmpA6=_tmpA5->f1;}}_LL6F:
 _tmpA8=_tmpA6;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpA7=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA7->tag != 3)goto _LL65;else{_tmpA8=_tmpA7->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp715;(*q).f1=(((_tmp715.Loc_n).val=0,(((_tmp715.Loc_n).tag=4,_tmp715))));}
if(te->allow_valueof){
void*_tmpD9=Cyc_Tcutil_compress(_tmpA8->type);void*_tmpDA=_tmpD9;void*_tmpDC;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpDA;if(_tmpDB->tag != 19)goto _LL87;else{_tmpDC=(void*)_tmpDB->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp718;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp717;e->r=(void*)((_tmp717=_cycalloc(sizeof(*_tmp717)),((_tmp717[0]=((_tmp718.tag=38,((_tmp718.f1=_tmpDC,_tmp718)))),_tmp717))));}
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
const char*_tmp71B;void*_tmp71A;(_tmp71A=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE6->hd)->loc,((_tmp71B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp71B,sizeof(char),49))),_tag_dyneither(_tmp71A,sizeof(void*),0)));}
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
{const char*_tmp720;void*_tmp71F[2];struct Cyc_String_pa_PrintArg_struct _tmp71E;struct Cyc_String_pa_PrintArg_struct _tmp71D;(_tmp71D.tag=0,((_tmp71D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp71E.tag=0,((_tmp71E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp71F[0]=& _tmp71E,((_tmp71F[1]=& _tmp71D,Cyc_Tcutil_terr(e->loc,((_tmp720="descriptor has type %s but argument has type %s",_tag_dyneither(_tmp720,sizeof(char),48))),_tag_dyneither(_tmp71F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp721;*alias_arg_exps=((_tmp721=_cycalloc(sizeof(*_tmp721)),((_tmp721->hd=(void*)arg_cnt,((_tmp721->tl=*alias_arg_exps,_tmp721))))));}{
struct _RegionHandle _tmpF0=_new_region("temp");struct _RegionHandle*temp=& _tmpF0;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp724;void*_tmp723;(_tmp723=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp724="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp724,sizeof(char),49))),_tag_dyneither(_tmp723,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp727;void*_tmp726;(_tmp726=0,Cyc_Tcutil_terr(fmt->loc,((_tmp727="too few arguments",_tag_dyneither(_tmp727,sizeof(char),18))),_tag_dyneither(_tmp726,sizeof(void*),0)));}
if(_tmpEA != 0){
{const char*_tmp72A;void*_tmp729;(_tmp729=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp72A="too many arguments",_tag_dyneither(_tmp72A,sizeof(char),19))),_tag_dyneither(_tmp729,sizeof(void*),0)));}{
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp72D;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp72C;(int)_throw((void*)((_tmp72C=_cycalloc_atomic(sizeof(*_tmp72C)),((_tmp72C[0]=((_tmp72D.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp72D)),_tmp72C)))));};}}}
# 447
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 449
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 453
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp731;void*_tmp730[1];struct Cyc_String_pa_PrintArg_struct _tmp72F;(_tmp72F.tag=0,((_tmp72F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp730[0]=& _tmp72F,Cyc_Tcutil_terr(loc,((_tmp731="expecting numeric type but found %s",_tag_dyneither(_tmp731,sizeof(char),36))),_tag_dyneither(_tmp730,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 457
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp735;void*_tmp734[1];struct Cyc_String_pa_PrintArg_struct _tmp733;(_tmp733.tag=0,((_tmp733.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp734[0]=& _tmp733,Cyc_Tcutil_terr(loc,((_tmp735="expecting integral or * type but found %s",_tag_dyneither(_tmp735,sizeof(char),42))),_tag_dyneither(_tmp734,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 462
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp739;void*_tmp738[1];struct Cyc_String_pa_PrintArg_struct _tmp737;(_tmp737.tag=0,((_tmp737.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp738[0]=& _tmp737,Cyc_Tcutil_terr(loc,((_tmp739="expecting integral type but found %s",_tag_dyneither(_tmp739,sizeof(char),37))),_tag_dyneither(_tmp738,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 466
{void*_tmp102=t;union Cyc_Absyn_Constraint*_tmp105;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp103=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102;if(_tmp103->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp104=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102;if(_tmp104->tag != 5)goto _LL9B;else{_tmp105=((_tmp104->f1).ptr_atts).bounds;}}_LL9A:
# 469
{void*_tmp106=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp105);void*_tmp107=_tmp106;struct Cyc_Absyn_Exp*_tmp10A;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp108=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp107;if(_tmp108->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp109=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp107;if(_tmp109->tag != 1)goto _LL9D;else{_tmp10A=_tmp109->f1;}}_LLA1:
# 472
 if(!Cyc_Evexp_c_can_eval(_tmp10A) && !((unsigned int)Cyc_Tcenv_allow_valueof)){
const char*_tmp73C;void*_tmp73B;(_tmp73B=0,Cyc_Tcutil_terr(loc,((_tmp73C="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp73C,sizeof(char),55))),_tag_dyneither(_tmp73B,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 476
goto _LL96;_LL9B:;_LL9C: {
# 478
const char*_tmp740;void*_tmp73F[1];struct Cyc_String_pa_PrintArg_struct _tmp73E;(_tmp73E.tag=0,((_tmp73E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp73F[0]=& _tmp73E,Cyc_Tcutil_terr(loc,((_tmp740="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp740,sizeof(char),47))),_tag_dyneither(_tmp73F,sizeof(void*),1)))))));}_LL96:;}
# 480
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp743;void*_tmp742;(_tmp742=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp743="Non-unary primop",_tag_dyneither(_tmp743,sizeof(char),17))),_tag_dyneither(_tmp742,sizeof(void*),0)));}}}
# 486
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 489
if(!checker(e1)){
{const char*_tmp747;void*_tmp746[1];struct Cyc_String_pa_PrintArg_struct _tmp745;(_tmp745.tag=0,((_tmp745.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp746[0]=& _tmp745,Cyc_Tcutil_terr(e1->loc,((_tmp747="type %s cannot be used here",_tag_dyneither(_tmp747,sizeof(char),28))),_tag_dyneither(_tmp746,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 493
if(!checker(e2)){
{const char*_tmp74B;void*_tmp74A[1];struct Cyc_String_pa_PrintArg_struct _tmp749;(_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp74A[0]=& _tmp749,Cyc_Tcutil_terr(e2->loc,((_tmp74B="type %s cannot be used here",_tag_dyneither(_tmp74B,sizeof(char),28))),_tag_dyneither(_tmp74A,sizeof(void*),1)))))));}
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
const char*_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_Tcutil_terr(e1->loc,((_tmp74E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp74E,sizeof(char),50))),_tag_dyneither(_tmp74D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp751;void*_tmp750;(_tmp750=0,Cyc_Tcutil_terr(e1->loc,((_tmp751="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp751,sizeof(char),54))),_tag_dyneither(_tmp750,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp755;void*_tmp754[1];struct Cyc_String_pa_PrintArg_struct _tmp753;(_tmp753.tag=0,((_tmp753.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp754[0]=& _tmp753,Cyc_Tcutil_terr(e2->loc,((_tmp755="expecting int but found %s",_tag_dyneither(_tmp755,sizeof(char),27))),_tag_dyneither(_tmp754,sizeof(void*),1)))))));}{
void*_tmp127=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp11E);void*_tmp128=_tmp127;struct Cyc_Absyn_Exp*_tmp12B;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp129=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp128;if(_tmp129->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp128;if(_tmp12A->tag != 1)goto _LLA8;else{_tmp12B=_tmp12A->f1;}}_LLAC:
# 518
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11F)){
const char*_tmp758;void*_tmp757;(_tmp757=0,Cyc_Tcutil_warn(e1->loc,((_tmp758="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp758,sizeof(char),70))),_tag_dyneither(_tmp757,sizeof(void*),0)));}{
# 526
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp75E;struct Cyc_Absyn_PtrInfo _tmp75D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp75C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12E=(_tmp75C=_cycalloc(sizeof(*_tmp75C)),((_tmp75C[0]=((_tmp75E.tag=5,((_tmp75E.f1=((_tmp75D.elt_typ=_tmp11A,((_tmp75D.elt_tq=_tmp11B,((_tmp75D.ptr_atts=(
((_tmp75D.ptr_atts).rgn=_tmp11C,(((_tmp75D.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp75D.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp75D.ptr_atts).zero_term=_tmp11F,(((_tmp75D.ptr_atts).ptrloc=0,_tmp75D.ptr_atts)))))))))),_tmp75D)))))),_tmp75E)))),_tmp75C)));
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
{const char*_tmp763;void*_tmp762[2];struct Cyc_String_pa_PrintArg_struct _tmp761;struct Cyc_String_pa_PrintArg_struct _tmp760;(_tmp760.tag=0,((_tmp760.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_typ2string(t2)),((_tmp761.tag=0,((_tmp761.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp762[0]=& _tmp761,((_tmp762[1]=& _tmp760,Cyc_Tcutil_terr(e1->loc,((_tmp763="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp763,sizeof(char),59))),_tag_dyneither(_tmp762,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 551
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp768;void*_tmp767[2];struct Cyc_String_pa_PrintArg_struct _tmp766;struct Cyc_String_pa_PrintArg_struct _tmp765;(_tmp765.tag=0,((_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp766.tag=0,((_tmp766.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp767[0]=& _tmp766,((_tmp767[1]=& _tmp765,Cyc_Tcutil_terr(e1->loc,((_tmp768="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp768,sizeof(char),59))),_tag_dyneither(_tmp767,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 560
{const char*_tmp76B;void*_tmp76A;(_tmp76A=0,Cyc_Tcutil_warn(e1->loc,((_tmp76B="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp76B,sizeof(char),60))),_tag_dyneither(_tmp76A,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 564
return Cyc_Absyn_sint_typ;}else{
# 566
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp76E;void*_tmp76D;(_tmp76D=0,Cyc_Tcutil_terr(e1->loc,((_tmp76E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp76E,sizeof(char),50))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_Tcutil_terr(e1->loc,((_tmp771="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp771,sizeof(char),54))),_tag_dyneither(_tmp770,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp776;void*_tmp775[2];struct Cyc_String_pa_PrintArg_struct _tmp774;struct Cyc_String_pa_PrintArg_struct _tmp773;(_tmp773.tag=0,((_tmp773.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp774.tag=0,((_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp775[0]=& _tmp774,((_tmp775[1]=& _tmp773,Cyc_Tcutil_terr(e2->loc,((_tmp776="expecting either %s or int but found %s",_tag_dyneither(_tmp776,sizeof(char),40))),_tag_dyneither(_tmp775,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 575
return t1;}}}
# 579
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp779;void*_tmp778;(_tmp778=0,Cyc_Tcutil_warn(e1->loc,((_tmp779="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp779,sizeof(char),60))),_tag_dyneither(_tmp778,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 589
{const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_warn(e1->loc,((_tmp77C="thin pointer subtraction!",_tag_dyneither(_tmp77C,sizeof(char),26))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 592
{const char*_tmp77F;void*_tmp77E;(_tmp77E=0,Cyc_Tcutil_warn(e1->loc,((_tmp77F="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp77F,sizeof(char),56))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 595
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp782;void*_tmp781;(_tmp781=0,Cyc_Tcutil_warn(e1->loc,((_tmp782="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp782,sizeof(char),51))),_tag_dyneither(_tmp781,sizeof(void*),0)));}
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
struct _tuple0 _tmp783;struct _tuple0 _tmp14C=(_tmp783.f1=Cyc_Tcutil_compress(t1),((_tmp783.f2=Cyc_Tcutil_compress(t2),_tmp783)));struct _tuple0 _tmp14D=_tmp14C;void*_tmp14F;void*_tmp151;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp14E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp14E->tag != 5)goto _LLB0;else{_tmp14F=(_tmp14E->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp150->tag != 5)goto _LLB0;else{_tmp151=(_tmp150->f1).elt_typ;}};_LLAF:
# 633
 if(Cyc_Tcutil_unify(_tmp14F,_tmp151))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp152->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp153=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp153->tag != 15)goto _LLB2;};_LLB1:
# 635
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 638
{const char*_tmp788;void*_tmp787[2];struct Cyc_String_pa_PrintArg_struct _tmp786;struct Cyc_String_pa_PrintArg_struct _tmp785;(_tmp785.tag=0,((_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp786.tag=0,((_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp787[0]=& _tmp786,((_tmp787[1]=& _tmp785,Cyc_Tcutil_terr(loc,((_tmp788="comparison not allowed between %s and %s",_tag_dyneither(_tmp788,sizeof(char),41))),_tag_dyneither(_tmp787,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp789;struct _tuple0 _tmp159=(_tmp789.f1=t1,((_tmp789.f2=t2,_tmp789)));struct _tuple0 _tmp15A=_tmp159;void*_tmp15C;void*_tmp15E;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f1;if(_tmp15B->tag != 15)goto _LLB7;else{_tmp15C=(void*)_tmp15B->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f2;if(_tmp15D->tag != 15)goto _LLB7;else{_tmp15E=(void*)_tmp15D->f1;}};_LLB6:
# 658
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 661
{const char*_tmp78E;void*_tmp78D[2];struct Cyc_String_pa_PrintArg_struct _tmp78C;struct Cyc_String_pa_PrintArg_struct _tmp78B;(_tmp78B.tag=0,((_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp78C.tag=0,((_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp78D[0]=& _tmp78C,((_tmp78D[1]=& _tmp78B,Cyc_Tcutil_terr(loc,((_tmp78E="comparison not allowed between %s and %s",_tag_dyneither(_tmp78E,sizeof(char),41))),_tag_dyneither(_tmp78D,sizeof(void*),2)))))))))))));}
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
const char*_tmp791;void*_tmp790;(_tmp790=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp791="bad binary primop",_tag_dyneither(_tmp791,sizeof(char),18))),_tag_dyneither(_tmp790,sizeof(void*),0)));}}}
# 700
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 708
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp166=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmp167=Cyc_Tcexp_tcExp(te,topt,_tmp166);
if(!Cyc_Tcutil_is_numeric(_tmp166)){
const char*_tmp795;void*_tmp794[1];struct Cyc_String_pa_PrintArg_struct _tmp793;(_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp167)),((_tmp794[0]=& _tmp793,Cyc_Tcutil_terr(_tmp166->loc,((_tmp795="expecting numeric type but found %s",_tag_dyneither(_tmp795,sizeof(char),36))),_tag_dyneither(_tmp794,sizeof(void*),1)))))));}
return _tmp167;}
# 715
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmp16B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_tmp16B){case 0: _LLCC: {
const char*_tmp798;void*_tmp797;return(_tmp797=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp798="primitive operator has 0 arguments",_tag_dyneither(_tmp798,sizeof(char),35))),_tag_dyneither(_tmp797,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp79B;void*_tmp79A;return(_tmp79A=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp79B="primitive operator has > 2 arguments",_tag_dyneither(_tmp79B,sizeof(char),37))),_tag_dyneither(_tmp79A,sizeof(void*),0)));}}}
# 723
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 726
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp170=Cyc_Tcutil_compress(t);
void*_tmp171=_tmp170;struct Cyc_Absyn_Aggrdecl*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Datatypefield*_tmp177;void*_tmp179;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_List_List*_tmp17C;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp171;if(_tmp172->tag != 11)goto _LLD4;else{if((((_tmp172->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp173=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp172->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 730
 if(_tmp173->impl == 0){
{const char*_tmp79E;void*_tmp79D;(_tmp79D=0,Cyc_Tcutil_terr(loc,((_tmp79E="attempt to write an abstract aggregate",_tag_dyneither(_tmp79E,sizeof(char),39))),_tag_dyneither(_tmp79D,sizeof(void*),0)));}
return 0;}else{
# 734
_tmp175=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp173->impl))->fields;goto _LLD5;}_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp171;if(_tmp174->tag != 12)goto _LLD6;else{_tmp175=_tmp174->f2;}}_LLD5:
# 736
 for(0;_tmp175 != 0;_tmp175=_tmp175->tl){
struct Cyc_Absyn_Aggrfield*_tmp17F=(struct Cyc_Absyn_Aggrfield*)_tmp175->hd;
if((_tmp17F->tq).real_const){
{const char*_tmp7A2;void*_tmp7A1[1];struct Cyc_String_pa_PrintArg_struct _tmp7A0;(_tmp7A0.tag=0,((_tmp7A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp17F->name),((_tmp7A1[0]=& _tmp7A0,Cyc_Tcutil_terr(loc,((_tmp7A2="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp7A2,sizeof(char),56))),_tag_dyneither(_tmp7A1,sizeof(void*),1)))))));}
return 0;}
# 742
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp17F->type))return 0;}
# 744
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp171;if(_tmp176->tag != 4)goto _LLD8;else{if((((_tmp176->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp177=((struct _tuple2)(((_tmp176->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 746
{struct Cyc_List_List*_tmp183=_tmp177->typs;for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
struct _tuple18*_tmp184=(struct _tuple18*)_tmp183->hd;struct Cyc_Absyn_Tqual _tmp186;void*_tmp187;struct _tuple18*_tmp185=_tmp184;_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;
if(_tmp186.real_const){
{const char*_tmp7A6;void*_tmp7A5[1];struct Cyc_String_pa_PrintArg_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp177->name)),((_tmp7A5[0]=& _tmp7A4,Cyc_Tcutil_terr(loc,((_tmp7A6="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp7A6,sizeof(char),64))),_tag_dyneither(_tmp7A5,sizeof(void*),1)))))));}
return 0;}
# 752
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp187))return 0;}}
# 754
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp171;if(_tmp178->tag != 8)goto _LLDA;else{_tmp179=(_tmp178->f1).elt_type;_tmp17A=(_tmp178->f1).tq;}}_LLD9:
# 756
 if(_tmp17A.real_const){
{const char*_tmp7A9;void*_tmp7A8;(_tmp7A8=0,Cyc_Tcutil_terr(loc,((_tmp7A9="attempt to over-write a const array",_tag_dyneither(_tmp7A9,sizeof(char),36))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}
return 0;}
# 760
return Cyc_Tcexp_check_writable_aggr(loc,_tmp179);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp171;if(_tmp17B->tag != 10)goto _LLDC;else{_tmp17C=_tmp17B->f1;}}_LLDB:
# 762
 for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
struct _tuple18*_tmp18D=(struct _tuple18*)_tmp17C->hd;struct Cyc_Absyn_Tqual _tmp18F;void*_tmp190;struct _tuple18*_tmp18E=_tmp18D;_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;
if(_tmp18F.real_const){
{const char*_tmp7AC;void*_tmp7AB;(_tmp7AB=0,Cyc_Tcutil_terr(loc,((_tmp7AC="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp7AC,sizeof(char),56))),_tag_dyneither(_tmp7AB,sizeof(void*),0)));}
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
{const char*_tmp7AF;void*_tmp7AE;(_tmp7AE=0,Cyc_Tcutil_terr(e->loc,((_tmp7AF="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp7AF,sizeof(char),47))),_tag_dyneither(_tmp7AE,sizeof(void*),0)));}
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
const char*_tmp7B3;void*_tmp7B2[1];struct Cyc_String_pa_PrintArg_struct _tmp7B1;(_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7B2[0]=& _tmp7B1,Cyc_Tcutil_terr(e->loc,((_tmp7B3="attempt to write a const location: %s",_tag_dyneither(_tmp7B3,sizeof(char),38))),_tag_dyneither(_tmp7B2,sizeof(void*),1)))))));};}
# 852
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 855
struct _RegionHandle _tmp1E1=_new_region("temp");struct _RegionHandle*temp=& _tmp1E1;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 858
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp7B6;void*_tmp7B5;void*_tmp1E4=(_tmp7B5=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B6="increment/decrement of non-lvalue",_tag_dyneither(_tmp7B6,sizeof(char),34))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));_npop_handler(0);return _tmp1E4;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 863
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp7B9;void*_tmp7B8;(_tmp7B8=0,Cyc_Tcutil_terr(e->loc,((_tmp7B9="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7B9,sizeof(char),50))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp7BC;void*_tmp7BB;(_tmp7BB=0,Cyc_Tcutil_terr(e->loc,((_tmp7BC="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7BC,sizeof(char),54))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}}else{
# 872
const char*_tmp7C0;void*_tmp7BF[1];struct Cyc_String_pa_PrintArg_struct _tmp7BE;(_tmp7BE.tag=0,((_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7BF[0]=& _tmp7BE,Cyc_Tcutil_terr(e->loc,((_tmp7C0="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7C0,sizeof(char),44))),_tag_dyneither(_tmp7BF,sizeof(void*),1)))))));}}{
# 874
void*_tmp1EC=t;_npop_handler(0);return _tmp1EC;};};
# 855
;_pop_region(temp);}
# 878
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 880
struct _RegionHandle _tmp1ED=_new_region("r");struct _RegionHandle*r=& _tmp1ED;_push_region(r);{
const char*_tmp7C1;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp7C1="conditional expression",_tag_dyneither(_tmp7C1,sizeof(char),23))));}
# 883
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 889
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp7C2;struct Cyc_List_List _tmp1EF=(_tmp7C2.hd=e3,((_tmp7C2.tl=0,_tmp7C2)));
struct Cyc_List_List _tmp7C3;struct Cyc_List_List _tmp1F0=(_tmp7C3.hd=e2,((_tmp7C3.tl=& _tmp1EF,_tmp7C3)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1F0)){
{const char*_tmp7C8;void*_tmp7C7[2];struct Cyc_String_pa_PrintArg_struct _tmp7C6;struct Cyc_String_pa_PrintArg_struct _tmp7C5;(_tmp7C5.tag=0,((_tmp7C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7C6.tag=0,((_tmp7C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7C7[0]=& _tmp7C6,((_tmp7C7[1]=& _tmp7C5,Cyc_Tcutil_terr(loc,((_tmp7C8="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7C8,sizeof(char),48))),_tag_dyneither(_tmp7C7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 897
void*_tmp1F5=t;_npop_handler(0);return _tmp1F5;};};};
# 880
;_pop_region(r);}
# 901
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 903
{const char*_tmp7C9;Cyc_Tcexp_tcTest(te,e1,((_tmp7C9="logical-and expression",_tag_dyneither(_tmp7C9,sizeof(char),23))));}
{const char*_tmp7CA;Cyc_Tcexp_tcTest(te,e2,((_tmp7CA="logical-and expression",_tag_dyneither(_tmp7CA,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 909
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 911
{const char*_tmp7CB;Cyc_Tcexp_tcTest(te,e1,((_tmp7CB="logical-or expression",_tag_dyneither(_tmp7CB,sizeof(char),22))));}
{const char*_tmp7CC;Cyc_Tcexp_tcTest(te,e2,((_tmp7CC="logical-or expression",_tag_dyneither(_tmp7CC,sizeof(char),22))));}
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
{const char*_tmp7CF;void*_tmp7CE;(_tmp7CE=0,Cyc_Tcutil_terr(loc,((_tmp7CF="cannot assign to an array",_tag_dyneither(_tmp7CF,sizeof(char),26))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 936
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp7D2;void*_tmp7D1;(_tmp7D1=0,Cyc_Tcutil_terr(loc,((_tmp7D2="type is abstract (can't determine size).",_tag_dyneither(_tmp7D2,sizeof(char),41))),_tag_dyneither(_tmp7D1,sizeof(void*),0)));}
# 940
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp7D5;void*_tmp7D4;void*_tmp206=(_tmp7D4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D5="assignment to non-lvalue",_tag_dyneither(_tmp7D5,sizeof(char),25))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));_npop_handler(0);return _tmp206;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp207=_new_region("temp");struct _RegionHandle*temp=& _tmp207;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp7D8;void*_tmp7D7;(_tmp7D7=0,Cyc_Tcutil_terr(e2->loc,((_tmp7D8="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7D8,sizeof(char),49))),_tag_dyneither(_tmp7D7,sizeof(void*),0)));}
# 948
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp7DD;void*_tmp7DC[2];struct Cyc_String_pa_PrintArg_struct _tmp7DB;struct Cyc_String_pa_PrintArg_struct _tmp7DA;void*_tmp20A=(_tmp7DA.tag=0,((_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7DB.tag=0,((_tmp7DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7DC[0]=& _tmp7DB,((_tmp7DC[1]=& _tmp7DA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DD="type mismatch: %s != %s",_tag_dyneither(_tmp7DD,sizeof(char),24))),_tag_dyneither(_tmp7DC,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp20B=_tmp20A;_npop_handler(1);return _tmp20B;};}
# 944
;_pop_region(temp);}else{
# 956
enum Cyc_Absyn_Primop _tmp210=(enum Cyc_Absyn_Primop)po->v;
void*_tmp211=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp210,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp211,t1) || Cyc_Tcutil_coerceable(_tmp211) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7E2;void*_tmp7E1[2];struct Cyc_String_pa_PrintArg_struct _tmp7E0;struct Cyc_String_pa_PrintArg_struct _tmp7DF;void*_tmp212=
(_tmp7DF.tag=0,((_tmp7DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 962
Cyc_Absynpp_typ2string(t2)),((_tmp7E0.tag=0,((_tmp7E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 961
Cyc_Absynpp_typ2string(t1)),((_tmp7E1[0]=& _tmp7E0,((_tmp7E1[1]=& _tmp7DF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E2="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp7E2,sizeof(char),82))),_tag_dyneither(_tmp7E1,sizeof(void*),2)))))))))))));
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
{const char*_tmp7E7;void*_tmp7E6[2];struct Cyc_String_pa_PrintArg_struct _tmp7E5;struct Cyc_String_pa_PrintArg_struct _tmp7E4;(_tmp7E4.tag=0,((_tmp7E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7E5.tag=0,((_tmp7E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7E6[0]=& _tmp7E5,((_tmp7E6[1]=& _tmp7E4,Cyc_Tcutil_terr(e->loc,((_tmp7E7="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp7E7,sizeof(char),54))),_tag_dyneither(_tmp7E6,sizeof(void*),2)))))))))))));}
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
const char*_tmp7EA;void*_tmp7E9;(_tmp7E9=0,Cyc_Tcutil_terr(e->loc,((_tmp7EA="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp7EA,sizeof(char),79))),_tag_dyneither(_tmp7E9,sizeof(void*),0)));}
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
const char*_tmp7EB;struct _dyneither_ptr s0=(_tmp7EB="actual argument has type ",_tag_dyneither(_tmp7EB,sizeof(char),26));
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
const char*_tmp7EC;struct _dyneither_ptr s2=(_tmp7EC=" but formal has type ",_tag_dyneither(_tmp7EC,sizeof(char),22));
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70){
struct Cyc_String_pa_PrintArg_struct _tmp7FA;struct Cyc_String_pa_PrintArg_struct _tmp7F9;struct Cyc_String_pa_PrintArg_struct _tmp7F8;struct Cyc_String_pa_PrintArg_struct _tmp7F7;void*_tmp7F6[4];const char*_tmp7F5;void*_tmp7F4;(_tmp7F4=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp7F9.tag=0,((_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp7F8.tag=0,((_tmp7F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp7F7.tag=0,((_tmp7F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp7F6[0]=& _tmp7F7,((_tmp7F6[1]=& _tmp7F8,((_tmp7F6[2]=& _tmp7F9,((_tmp7F6[3]=& _tmp7FA,Cyc_aprintf(((_tmp7F5="%s\n\t%s\n%s\n\t%s.",_tag_dyneither(_tmp7F5,sizeof(char),15))),_tag_dyneither(_tmp7F6,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp7F4,sizeof(void*),0)));}else{
# 1073
struct Cyc_String_pa_PrintArg_struct _tmp808;struct Cyc_String_pa_PrintArg_struct _tmp807;struct Cyc_String_pa_PrintArg_struct _tmp806;struct Cyc_String_pa_PrintArg_struct _tmp805;void*_tmp804[4];const char*_tmp803;void*_tmp802;(_tmp802=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp808.tag=0,((_tmp808.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp807.tag=0,((_tmp807.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp806.tag=0,((_tmp806.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp805.tag=0,((_tmp805.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp804[0]=& _tmp805,((_tmp804[1]=& _tmp806,((_tmp804[2]=& _tmp807,((_tmp804[3]=& _tmp808,Cyc_aprintf(((_tmp803="%s%s%s%s.",_tag_dyneither(_tmp803,sizeof(char),10))),_tag_dyneither(_tmp804,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp802,sizeof(void*),0)));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1078
if(alias_coercion){
struct Cyc_List_List*_tmp809;*alias_arg_exps=((_tmp809=_cycalloc(sizeof(*_tmp809)),((_tmp809->hd=(void*)_tmp232,((_tmp809->tl=*alias_arg_exps,_tmp809))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp80C;void*_tmp80B;(_tmp80B=0,Cyc_Tcutil_terr(e1->loc,((_tmp80C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp80C,sizeof(char),49))),_tag_dyneither(_tmp80B,sizeof(void*),0)));}
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
struct Cyc_Core_Opt*_tmp80D;fmt_args=((_tmp80D=_cycalloc(sizeof(*_tmp80D)),((_tmp80D->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp268 - 1),_tmp80D))));}
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
{const char*_tmp810;void*_tmp80F;(_tmp80F=0,Cyc_Tcutil_terr(loc,((_tmp810="bad format arguments",_tag_dyneither(_tmp810,sizeof(char),21))),_tag_dyneither(_tmp80F,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp272=_tmp270;_LL141:(void)_rethrow(_tmp272);_LL13D:;}};}
# 1120
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1124
if(_tmp245 != 0){const char*_tmp813;void*_tmp812;(_tmp812=0,Cyc_Tcutil_terr(loc,((_tmp813="too few arguments for function",_tag_dyneither(_tmp813,sizeof(char),31))),_tag_dyneither(_tmp812,sizeof(void*),0)));}else{
# 1126
if((_tmp231 != 0  || _tmp246) || _tmp247 != 0){
if(_tmp246)
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
Cyc_Tcexp_tcExp(_tmp235,0,(struct Cyc_Absyn_Exp*)_tmp231->hd);}else{
if(_tmp247 == 0){
const char*_tmp816;void*_tmp815;(_tmp815=0,Cyc_Tcutil_terr(loc,((_tmp816="too many arguments for function",_tag_dyneither(_tmp816,sizeof(char),32))),_tag_dyneither(_tmp815,sizeof(void*),0)));}else{
# 1133
struct Cyc_Absyn_VarargInfo _tmp279=*_tmp247;void*_tmp27B;int _tmp27C;struct Cyc_Absyn_VarargInfo _tmp27A=_tmp279;_tmp27B=_tmp27A.type;_tmp27C=_tmp27A.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp817;struct Cyc_Absyn_VarargCallInfo*_tmp27D=(_tmp817=_cycalloc(sizeof(*_tmp817)),((_tmp817->num_varargs=0,((_tmp817->injectors=0,((_tmp817->vai=_tmp247,_tmp817)))))));
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
{const char*_tmp81C;void*_tmp81B[2];struct Cyc_String_pa_PrintArg_struct _tmp81A;struct Cyc_String_pa_PrintArg_struct _tmp819;(_tmp819.tag=0,((_tmp819.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp27B)),((_tmp81B[0]=& _tmp81A,((_tmp81B[1]=& _tmp819,Cyc_Tcutil_terr(loc,((_tmp81C="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp81C,sizeof(char),49))),_tag_dyneither(_tmp81B,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1151
if(alias_coercion){
struct Cyc_List_List*_tmp81D;*alias_arg_exps=((_tmp81D=_cycalloc(sizeof(*_tmp81D)),((_tmp81D->hd=(void*)_tmp232,((_tmp81D->tl=*alias_arg_exps,_tmp81D))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp27B) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp820;void*_tmp81F;(_tmp81F=0,Cyc_Tcutil_terr(e1->loc,((_tmp820="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp820,sizeof(char),49))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}}else{
# 1160
void*_tmp285=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp27B));void*_tmp286=_tmp285;struct Cyc_Absyn_Datatypedecl*_tmp288;struct Cyc_List_List*_tmp289;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp287=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp286;if(_tmp287->tag != 3)goto _LL145;else{if((((_tmp287->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp288=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp287->f1).datatype_info).KnownDatatype).val);_tmp289=(_tmp287->f1).targs;}}_LL144: {
# 1164
struct Cyc_Absyn_Datatypedecl*_tmp28A=*Cyc_Tcenv_lookup_datatypedecl(_tmp235,loc,_tmp288->name);
struct Cyc_List_List*fields=0;
if(_tmp28A->fields == 0){
const char*_tmp824;void*_tmp823[1];struct Cyc_String_pa_PrintArg_struct _tmp822;(_tmp822.tag=0,((_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp27B)),((_tmp823[0]=& _tmp822,Cyc_Tcutil_terr(loc,((_tmp824="can't inject into abstract datatype %s",_tag_dyneither(_tmp824,sizeof(char),39))),_tag_dyneither(_tmp823,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp28A->fields))->v;}
# 1176
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp27B),Cyc_Tcenv_curr_rgn(_tmp235))){
const char*_tmp829;void*_tmp828[2];struct Cyc_String_pa_PrintArg_struct _tmp827;struct Cyc_String_pa_PrintArg_struct _tmp826;(_tmp826.tag=0,((_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp235))),((_tmp827.tag=0,((_tmp827.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp27B)),((_tmp828[0]=& _tmp827,((_tmp828[1]=& _tmp826,Cyc_Tcutil_terr(loc,((_tmp829="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp829,sizeof(char),49))),_tag_dyneither(_tmp828,sizeof(void*),2)))))))))))));}{
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
const char*_tmp82C;void*_tmp82B;(_tmp82B=0,Cyc_Tcutil_terr(e1->loc,((_tmp82C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp82C,sizeof(char),49))),_tag_dyneither(_tmp82B,sizeof(void*),0)));}}{
# 1192
struct Cyc_Absyn_Datatypefield*_tmp296=Cyc_Tcexp_tcInjection(_tmp235,e1,Cyc_Tcutil_pointer_elt_type(_tmp27B),rgn,_tmp293,fields);
# 1194
if(_tmp296 != 0){
struct Cyc_List_List*_tmp82D;_tmp27D->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp27D->injectors,(
(_tmp82D=_cycalloc(sizeof(*_tmp82D)),((_tmp82D->hd=_tmp296,((_tmp82D->tl=0,_tmp82D)))))));}};};}}
# 1200
_npop_handler(0);goto _LL142;
# 1180
;_pop_region(rgn);};}_LL145:;_LL146:
# 1201
{const char*_tmp830;void*_tmp82F;(_tmp82F=0,Cyc_Tcutil_terr(loc,((_tmp830="bad inject vararg type",_tag_dyneither(_tmp830,sizeof(char),23))),_tag_dyneither(_tmp82F,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1206
if(*alias_arg_exps == 0)
# 1215 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp235,loc,(void*)_check_null(_tmp242));{
# 1217
void*_tmp29B=_tmp244;_npop_handler(0);return _tmp29B;};};_LL133:;_LL134: {
const char*_tmp834;void*_tmp833[1];struct Cyc_String_pa_PrintArg_struct _tmp832;void*_tmp29F=(_tmp832.tag=0,((_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp833[0]=& _tmp832,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp834="expected pointer to function but found %s",_tag_dyneither(_tmp834,sizeof(char),42))),_tag_dyneither(_tmp833,sizeof(void*),1)))))));_npop_handler(0);return _tmp29F;}_LL130:;};_LL12E:;_LL12F: {
# 1220
const char*_tmp838;void*_tmp837[1];struct Cyc_String_pa_PrintArg_struct _tmp836;void*_tmp2A3=(_tmp836.tag=0,((_tmp836.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp837[0]=& _tmp836,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp838="expected pointer to function but found %s",_tag_dyneither(_tmp838,sizeof(char),42))),_tag_dyneither(_tmp837,sizeof(void*),1)))))));_npop_handler(0);return _tmp2A3;}_LL12B:;};}
# 1034 "tcexp.cyc"
;_pop_region(ter);}
# 1226 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp2A4=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp2A4,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp2A4,& bogus)){
const char*_tmp83D;void*_tmp83C[2];struct Cyc_String_pa_PrintArg_struct _tmp83B;struct Cyc_String_pa_PrintArg_struct _tmp83A;(_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp83B.tag=0,((_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1231
Cyc_Absynpp_typ2string(_tmp2A4)),((_tmp83C[0]=& _tmp83B,((_tmp83C[1]=& _tmp83A,Cyc_Tcutil_terr(loc,((_tmp83D="expected %s but found %s",_tag_dyneither(_tmp83D,sizeof(char),25))),_tag_dyneither(_tmp83C,sizeof(void*),2)))))))))))));}
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
struct _tuple13*_tmp840;struct Cyc_List_List*_tmp83F;instantiation=(
(_tmp83F=_region_malloc(temp,sizeof(*_tmp83F)),((_tmp83F->hd=((_tmp840=_region_malloc(temp,sizeof(*_tmp840)),((_tmp840->f1=(struct Cyc_Absyn_Tvar*)_tmp2B4->hd,((_tmp840->f2=(void*)ts->hd,_tmp840)))))),((_tmp83F->tl=instantiation,_tmp83F))))));};}
# 1258
if(ts != 0){
const char*_tmp843;void*_tmp842;void*_tmp2C6=
(_tmp842=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp843="too many type variables in instantiation",_tag_dyneither(_tmp843,sizeof(char),41))),_tag_dyneither(_tmp842,sizeof(void*),0)));_npop_handler(0);return _tmp2C6;}
# 1264
if(_tmp2B4 == 0){
_tmp2BB=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp2BB);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp2BB);
_tmp2BB=0;}{
# 1269
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp849;struct Cyc_Absyn_FnInfo _tmp848;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp847;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp847=_cycalloc(sizeof(*_tmp847)),((_tmp847[0]=((_tmp849.tag=9,((_tmp849.f1=((_tmp848.tvars=_tmp2B4,((_tmp848.effect=_tmp2B5,((_tmp848.ret_tqual=_tmp2B6,((_tmp848.ret_typ=_tmp2B7,((_tmp848.args=_tmp2B8,((_tmp848.c_varargs=_tmp2B9,((_tmp848.cyc_varargs=_tmp2BA,((_tmp848.rgn_po=_tmp2BB,((_tmp848.attributes=_tmp2BC,((_tmp848.requires_clause=_tmp2BD,((_tmp848.requires_relns=_tmp2BE,((_tmp848.ensures_clause=_tmp2BF,((_tmp848.ensures_relns=_tmp2C0,_tmp848)))))))))))))))))))))))))),_tmp849)))),_tmp847)))));
# 1274
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp84F;struct Cyc_Absyn_PtrInfo _tmp84E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp84D;void*_tmp2CA=(void*)((_tmp84D=_cycalloc(sizeof(*_tmp84D)),((_tmp84D[0]=((_tmp84F.tag=5,((_tmp84F.f1=((_tmp84E.elt_typ=new_fn_typ,((_tmp84E.elt_tq=_tmp2AC,((_tmp84E.ptr_atts=(((_tmp84E.ptr_atts).rgn=_tmp2AD,(((_tmp84E.ptr_atts).nullable=_tmp2AE,(((_tmp84E.ptr_atts).bounds=_tmp2AF,(((_tmp84E.ptr_atts).zero_term=_tmp2B0,(((_tmp84E.ptr_atts).ptrloc=0,_tmp84E.ptr_atts)))))))))),_tmp84E)))))),_tmp84F)))),_tmp84D))));_npop_handler(0);return _tmp2CA;};}
# 1249
;_pop_region(temp);}_LL14F:;_LL150:
# 1276
 goto _LL14C;_LL14C:;}
# 1278
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1281
const char*_tmp853;void*_tmp852[1];struct Cyc_String_pa_PrintArg_struct _tmp851;return(_tmp851.tag=0,((_tmp851.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp852[0]=& _tmp851,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp853="expecting polymorphic type but found %s",_tag_dyneither(_tmp853,sizeof(char),40))),_tag_dyneither(_tmp852,sizeof(void*),1)))))));};};}
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
const char*_tmp858;void*_tmp857[2];struct Cyc_String_pa_PrintArg_struct _tmp856;struct Cyc_String_pa_PrintArg_struct _tmp855;void*_tmp2D1=(_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp856.tag=0,((_tmp856.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp857[0]=& _tmp856,((_tmp857[1]=& _tmp855,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp858="cannot cast %s to %s",_tag_dyneither(_tmp858,sizeof(char),21))),_tag_dyneither(_tmp857,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp2D1;};}}}
# 1314
{struct _tuple0 _tmp859;struct _tuple0 _tmp2D6=(_tmp859.f1=e->r,((_tmp859.f2=Cyc_Tcutil_compress(t),_tmp859)));struct _tuple0 _tmp2D7=_tmp2D6;int _tmp2D9;union Cyc_Absyn_Constraint*_tmp2DB;union Cyc_Absyn_Constraint*_tmp2DC;union Cyc_Absyn_Constraint*_tmp2DD;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2D8=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D7.f1;if(_tmp2D8->tag != 33)goto _LL154;else{_tmp2D9=(_tmp2D8->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7.f2;if(_tmp2DA->tag != 5)goto _LL154;else{_tmp2DB=((_tmp2DA->f1).ptr_atts).nullable;_tmp2DC=((_tmp2DA->f1).ptr_atts).bounds;_tmp2DD=((_tmp2DA->f1).ptr_atts).zero_term;}};_LL153:
# 1318
 if((_tmp2D9  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DD)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DB)){
void*_tmp2DE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2DC);void*_tmp2DF=_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E1;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2E0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2DF;if(_tmp2E0->tag != 1)goto _LL159;else{_tmp2E1=_tmp2E0->f1;}}_LL158:
# 1321
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2E1)).f1 == 1){
const char*_tmp85C;void*_tmp85B;(_tmp85B=0,Cyc_Tcutil_warn(loc,((_tmp85C="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp85C,sizeof(char),60))),_tag_dyneither(_tmp85B,sizeof(void*),0)));}
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
const char*_tmp85F;void*_tmp85E;(_tmp85E=0,Cyc_Tcutil_terr(e->loc,((_tmp85F="Cannot take the address of an alias-free path",_tag_dyneither(_tmp85F,sizeof(char),46))),_tag_dyneither(_tmp85E,sizeof(void*),0)));}
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
{const char*_tmp862;void*_tmp861;(_tmp861=0,Cyc_Tcutil_terr(e->loc,((_tmp862="cannot take the address of a @tagged union member",_tag_dyneither(_tmp862,sizeof(char),50))),_tag_dyneither(_tmp861,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1391
struct _tuple14 _tmp300=Cyc_Tcutil_addressof_props(te,e);int _tmp302;void*_tmp303;struct _tuple14 _tmp301=_tmp300;_tmp302=_tmp301.f1;_tmp303=_tmp301.f2;
# 1393
if(Cyc_Tcutil_is_noalias_region(_tmp303,0)){
const char*_tmp865;void*_tmp864;(_tmp864=0,Cyc_Tcutil_terr(e->loc,((_tmp865="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp865,sizeof(char),60))),_tag_dyneither(_tmp864,sizeof(void*),0)));}{
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
const char*_tmp869;void*_tmp868[1];struct Cyc_String_pa_PrintArg_struct _tmp867;(_tmp867.tag=0,((_tmp867.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp868[0]=& _tmp867,Cyc_Tcutil_terr(loc,((_tmp869="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp869,sizeof(char),55))),_tag_dyneither(_tmp868,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp30A=Cyc_Tcutil_compress(*topt);void*_tmp30B=_tmp30A;void*_tmp30C;void*_tmp30E;_LL172: _tmp30C=_tmp30B;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp30D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp30C;if(_tmp30D->tag != 19)goto _LL174;else{_tmp30E=(void*)_tmp30D->f1;}};_LL173: {
# 1424
struct Cyc_Absyn_Exp*_tmp30F=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp86C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp86B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp310=(_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B[0]=((_tmp86C.tag=18,((_tmp86C.f1=_tmp30F,_tmp86C)))),_tmp86B)));
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
const char*_tmp870;void*_tmp86F[1];struct Cyc_String_pa_PrintArg_struct _tmp86E;(_tmp86E.tag=0,((_tmp86E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp318),((_tmp86F[0]=& _tmp86E,Cyc_Tcutil_terr(loc,((_tmp870="no field of struct/union has name %s",_tag_dyneither(_tmp870,sizeof(char),37))),_tag_dyneither(_tmp86F,sizeof(void*),1)))))));}else{
# 1458
_tmp314=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31C;if(_tmp31F->tag != 12)goto _LL180;else{_tmp320=_tmp31F->f2;}}_LL17F: {
# 1462
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp318,_tmp320);
if(!((unsigned int)t2)){
const char*_tmp874;void*_tmp873[1];struct Cyc_String_pa_PrintArg_struct _tmp872;(_tmp872.tag=0,((_tmp872.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp318),((_tmp873[0]=& _tmp872,Cyc_Tcutil_terr(loc,((_tmp874="no field of struct/union has name %s",_tag_dyneither(_tmp874,sizeof(char),37))),_tag_dyneither(_tmp873,sizeof(void*),1)))))));}else{
# 1466
_tmp314=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1471
if(bad_type){
if(_tmp313 == fs){
const char*_tmp878;void*_tmp877[1];struct Cyc_String_pa_PrintArg_struct _tmp876;(_tmp876.tag=0,((_tmp876.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp877[0]=& _tmp876,Cyc_Tcutil_terr(loc,((_tmp878="%s is not a known struct/union type",_tag_dyneither(_tmp878,sizeof(char),36))),_tag_dyneither(_tmp877,sizeof(void*),1)))))));}else{
# 1475
const char*_tmp87C;void*_tmp87B[1];struct Cyc_String_pa_PrintArg_struct _tmp87A;struct _dyneither_ptr _tmp32A=(_tmp87A.tag=0,((_tmp87A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp87B[0]=& _tmp87A,Cyc_aprintf(((_tmp87C="(%s)",_tag_dyneither(_tmp87C,sizeof(char),5))),_tag_dyneither(_tmp87B,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp313;x=x->tl){
void*_tmp32B=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp32C=_tmp32B;struct _dyneither_ptr*_tmp32E;unsigned int _tmp330;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp32D=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp32C;if(_tmp32D->tag != 0)goto _LL185;else{_tmp32E=_tmp32D->f1;}}_LL184:
# 1480
{const char*_tmp881;void*_tmp880[2];struct Cyc_String_pa_PrintArg_struct _tmp87F;struct Cyc_String_pa_PrintArg_struct _tmp87E;_tmp32A=((_tmp87E.tag=0,((_tmp87E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp32E),((_tmp87F.tag=0,((_tmp87F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp880[0]=& _tmp87F,((_tmp880[1]=& _tmp87E,Cyc_aprintf(((_tmp881="%s.%s",_tag_dyneither(_tmp881,sizeof(char),6))),_tag_dyneither(_tmp880,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp32F=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp32C;if(_tmp32F->tag != 1)goto _LL182;else{_tmp330=_tmp32F->f1;}}_LL186:
# 1482
{const char*_tmp886;void*_tmp885[2];struct Cyc_String_pa_PrintArg_struct _tmp884;struct Cyc_Int_pa_PrintArg_struct _tmp883;_tmp32A=((_tmp883.tag=1,((_tmp883.f1=(unsigned long)((int)_tmp330),((_tmp884.tag=0,((_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp885[0]=& _tmp884,((_tmp885[1]=& _tmp883,Cyc_aprintf(((_tmp886="%s.%d",_tag_dyneither(_tmp886,sizeof(char),6))),_tag_dyneither(_tmp885,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1484
const char*_tmp88B;void*_tmp88A[2];struct Cyc_String_pa_PrintArg_struct _tmp889;struct Cyc_String_pa_PrintArg_struct _tmp888;(_tmp888.tag=0,((_tmp888.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp889.tag=0,((_tmp889.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp88A[0]=& _tmp889,((_tmp88A[1]=& _tmp888,Cyc_Tcutil_terr(loc,((_tmp88B="%s == %s is not a struct/union type",_tag_dyneither(_tmp88B,sizeof(char),36))),_tag_dyneither(_tmp88A,sizeof(void*),2)))))))))))));};}}
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
const char*_tmp890;void*_tmp88F[2];struct Cyc_Int_pa_PrintArg_struct _tmp88E;struct Cyc_Int_pa_PrintArg_struct _tmp88D;(_tmp88D.tag=1,((_tmp88D.f1=(unsigned long)((int)_tmp31A),((_tmp88E.tag=1,((_tmp88E.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp345),((_tmp88F[0]=& _tmp88E,((_tmp88F[1]=& _tmp88D,Cyc_Tcutil_terr(loc,((_tmp890="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp890,sizeof(char),46))),_tag_dyneither(_tmp88F,sizeof(void*),2)))))))))))));}else{
# 1500
_tmp314=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp345,(int)_tmp31A))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp341;if(_tmp346->tag != 10)goto _LL18E;else{_tmp347=_tmp346->f1;}}_LL18D:
# 1504
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp347)<= _tmp31A){
const char*_tmp895;void*_tmp894[2];struct Cyc_Int_pa_PrintArg_struct _tmp893;struct Cyc_Int_pa_PrintArg_struct _tmp892;(_tmp892.tag=1,((_tmp892.f1=(unsigned long)((int)_tmp31A),((_tmp893.tag=1,((_tmp893.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp347),((_tmp894[0]=& _tmp893,((_tmp894[1]=& _tmp892,Cyc_Tcutil_terr(loc,((_tmp895="tuple has too few components: %d <= %d",_tag_dyneither(_tmp895,sizeof(char),39))),_tag_dyneither(_tmp894,sizeof(void*),2)))))))))))));}else{
# 1508
_tmp314=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp347,(int)_tmp31A)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp341;if(_tmp348->tag != 4)goto _LL190;else{if((((_tmp348->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp349=((struct _tuple2)(((_tmp348->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1512
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp349->typs)< _tmp31A){
const char*_tmp89A;void*_tmp899[2];struct Cyc_Int_pa_PrintArg_struct _tmp898;struct Cyc_Int_pa_PrintArg_struct _tmp897;(_tmp897.tag=1,((_tmp897.f1=(unsigned long)((int)_tmp31A),((_tmp898.tag=1,((_tmp898.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp349->typs),((_tmp899[0]=& _tmp898,((_tmp899[1]=& _tmp897,Cyc_Tcutil_terr(loc,((_tmp89A="datatype field has too few components: %d < %d",_tag_dyneither(_tmp89A,sizeof(char),47))),_tag_dyneither(_tmp899,sizeof(void*),2)))))))))))));}else{
# 1516
if(_tmp31A != 0)
_tmp314=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp349->typs,(int)(_tmp31A - 1))).f2;else{
if(_tmp313->tl != 0){
const char*_tmp89D;void*_tmp89C;(_tmp89C=0,Cyc_Tcutil_terr(loc,((_tmp89D="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp89D,sizeof(char),68))),_tag_dyneither(_tmp89C,sizeof(void*),0)));}}}
# 1521
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1525
if(bad_type){
const char*_tmp8A1;void*_tmp8A0[1];struct Cyc_String_pa_PrintArg_struct _tmp89F;(_tmp89F.tag=0,((_tmp89F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp8A0[0]=& _tmp89F,Cyc_Tcutil_terr(loc,((_tmp8A1="%s is not a known type",_tag_dyneither(_tmp8A1,sizeof(char),23))),_tag_dyneither(_tmp8A0,sizeof(void*),1)))))));}
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
struct Cyc_Core_Opt*_tmp8A2;void*_tmp365=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((_tmp8A2->v=_tmp364,_tmp8A2)))));
struct Cyc_Core_Opt*_tmp8A3;void*_tmp366=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3->v=_tmp364,_tmp8A3)))));
union Cyc_Absyn_Constraint*_tmp367=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp368=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp8A4;struct Cyc_Absyn_PtrAtts _tmp369=(_tmp8A4.rgn=_tmp366,((_tmp8A4.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp8A4.bounds=_tmp367,((_tmp8A4.zero_term=_tmp368,((_tmp8A4.ptrloc=0,_tmp8A4)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8AA;struct Cyc_Absyn_PtrInfo _tmp8A9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8A8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36A=(_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8AA.tag=5,((_tmp8AA.f1=((_tmp8A9.elt_typ=_tmp365,((_tmp8A9.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp8A9.ptr_atts=_tmp369,_tmp8A9)))))),_tmp8AA)))),_tmp8A8)));
Cyc_Tcutil_unify(t,(void*)_tmp36A);
_tmp360=_tmp365;_tmp361=_tmp366;_tmp362=_tmp367;_tmp363=_tmp368;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D;if(_tmp35F->tag != 5)goto _LL197;else{_tmp360=(_tmp35F->f1).elt_typ;_tmp361=((_tmp35F->f1).ptr_atts).rgn;_tmp362=((_tmp35F->f1).ptr_atts).bounds;_tmp363=((_tmp35F->f1).ptr_atts).zero_term;}}_LL196:
# 1551
 Cyc_Tcenv_check_rgn_accessible(_tmp35C,loc,_tmp361);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp362);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp360),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp35C)){
const char*_tmp8AD;void*_tmp8AC;(_tmp8AC=0,Cyc_Tcutil_terr(loc,((_tmp8AD="can't dereference abstract pointer type",_tag_dyneither(_tmp8AD,sizeof(char),40))),_tag_dyneither(_tmp8AC,sizeof(void*),0)));}{
void*_tmp373=_tmp360;_npop_handler(0);return _tmp373;};_LL197:;_LL198: {
# 1557
const char*_tmp8B1;void*_tmp8B0[1];struct Cyc_String_pa_PrintArg_struct _tmp8AF;void*_tmp377=(_tmp8AF.tag=0,((_tmp8AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8B0[0]=& _tmp8AF,Cyc_Tcexp_expr_err(_tmp35C,loc,topt,((_tmp8B1="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp8B1,sizeof(char),39))),_tag_dyneither(_tmp8B0,sizeof(void*),1)))))));_npop_handler(0);return _tmp377;}_LL192:;};}
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
const char*_tmp8B6;void*_tmp8B5[2];struct Cyc_String_pa_PrintArg_struct _tmp8B4;struct Cyc_String_pa_PrintArg_struct _tmp8B3;void*_tmp386=(_tmp8B3.tag=0,((_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp37C->name)),((_tmp8B5[0]=& _tmp8B4,((_tmp8B5[1]=& _tmp8B3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B6="%s has no %s member",_tag_dyneither(_tmp8B6,sizeof(char),20))),_tag_dyneither(_tmp8B5,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp386;}
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
const char*_tmp8BA;void*_tmp8B9[1];struct Cyc_String_pa_PrintArg_struct _tmp8B8;void*_tmp38C=(_tmp8B8.tag=0,((_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B9[0]=& _tmp8B8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8BA="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8BA,sizeof(char),56))),_tag_dyneither(_tmp8B9,sizeof(void*),1)))))));_npop_handler(0);return _tmp38C;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37C->impl))->exist_vars != 0){
# 1594
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8BE;void*_tmp8BD[1];struct Cyc_String_pa_PrintArg_struct _tmp8BC;void*_tmp390=(_tmp8BC.tag=0,((_tmp8BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8BD[0]=& _tmp8BC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8BE="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp8BE,sizeof(char),81))),_tag_dyneither(_tmp8BD,sizeof(void*),1)))))));_npop_handler(0);return _tmp390;}}{
# 1597
void*_tmp391=t2;_npop_handler(0);return _tmp391;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp37E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp37A;if(_tmp37E->tag != 12)goto _LL19E;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37E->f2;}}_LL19D: {
# 1599
struct Cyc_Absyn_Aggrfield*_tmp392=Cyc_Absyn_lookup_field(_tmp380,f);
if(_tmp392 == 0){
const char*_tmp8C2;void*_tmp8C1[1];struct Cyc_String_pa_PrintArg_struct _tmp8C0;void*_tmp396=(_tmp8C0.tag=0,((_tmp8C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8C1[0]=& _tmp8C0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C2="type has no %s member",_tag_dyneither(_tmp8C2,sizeof(char),22))),_tag_dyneither(_tmp8C1,sizeof(void*),1)))))));_npop_handler(0);return _tmp396;}
# 1604
if(((_tmp37F == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp392->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp392->requires_clause == 0){
# 1606
const char*_tmp8C6;void*_tmp8C5[1];struct Cyc_String_pa_PrintArg_struct _tmp8C4;void*_tmp39A=(_tmp8C4.tag=0,((_tmp8C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8C5[0]=& _tmp8C4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8C6,sizeof(char),56))),_tag_dyneither(_tmp8C5,sizeof(void*),1)))))));_npop_handler(0);return _tmp39A;}{
void*_tmp39B=_tmp392->type;_npop_handler(0);return _tmp39B;};}_LL19E:;_LL19F: {
# 1609
const char*_tmp8CA;void*_tmp8C9[1];struct Cyc_String_pa_PrintArg_struct _tmp8C8;void*_tmp39F=(_tmp8C8.tag=0,((_tmp8C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8C9[0]=& _tmp8C8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8CA="expecting struct or union, found %s",_tag_dyneither(_tmp8CA,sizeof(char),36))),_tag_dyneither(_tmp8C9,sizeof(void*),1)))))));_npop_handler(0);return _tmp39F;}_LL199:;};
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
const char*_tmp8CF;void*_tmp8CE[2];struct Cyc_String_pa_PrintArg_struct _tmp8CD;struct Cyc_String_pa_PrintArg_struct _tmp8CC;void*_tmp3B5=(_tmp8CC.tag=0,((_tmp8CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8CD.tag=0,((_tmp8CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3AB->name)),((_tmp8CE[0]=& _tmp8CD,((_tmp8CE[1]=& _tmp8CC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8CF="type %s has no %s member",_tag_dyneither(_tmp8CF,sizeof(char),25))),_tag_dyneither(_tmp8CE,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp3B5;}
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
const char*_tmp8D3;void*_tmp8D2[1];struct Cyc_String_pa_PrintArg_struct _tmp8D1;void*_tmp3BF=(_tmp8D1.tag=0,((_tmp8D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D2[0]=& _tmp8D1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D3="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8D3,sizeof(char),48))),_tag_dyneither(_tmp8D2,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BF;}_LL1AC:;}
# 1654
if((((_tmp3AB->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp3B0->requires_clause == 0){
# 1657
const char*_tmp8D7;void*_tmp8D6[1];struct Cyc_String_pa_PrintArg_struct _tmp8D5;void*_tmp3C3=(_tmp8D5.tag=0,((_tmp8D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D6[0]=& _tmp8D5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D7="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8D7,sizeof(char),56))),_tag_dyneither(_tmp8D6,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C3;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8DB;void*_tmp8DA[1];struct Cyc_String_pa_PrintArg_struct _tmp8D9;void*_tmp3C7=(_tmp8D9.tag=0,((_tmp8D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8DA[0]=& _tmp8D9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DB="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp8DB,sizeof(char),72))),_tag_dyneither(_tmp8DA,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C7;}}{
# 1664
void*_tmp3C8=t3;_npop_handler(0);return _tmp3C8;};};};}_LL1A8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AD->tag != 12)goto _LL1AA;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;}}_LL1A9: {
# 1666
struct Cyc_Absyn_Aggrfield*_tmp3C9=Cyc_Absyn_lookup_field(_tmp3AF,f);
if(_tmp3C9 == 0){
const char*_tmp8DF;void*_tmp8DE[1];struct Cyc_String_pa_PrintArg_struct _tmp8DD;void*_tmp3CD=(_tmp8DD.tag=0,((_tmp8DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8DE[0]=& _tmp8DD,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DF="type has no %s field",_tag_dyneither(_tmp8DF,sizeof(char),21))),_tag_dyneither(_tmp8DE,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CD;}
# 1671
if((_tmp3AE == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp3C9->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp8E3;void*_tmp8E2[1];struct Cyc_String_pa_PrintArg_struct _tmp8E1;void*_tmp3D1=(_tmp8E1.tag=0,((_tmp8E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E2[0]=& _tmp8E1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E3="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8E3,sizeof(char),56))),_tag_dyneither(_tmp8E2,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D1;}{
void*_tmp3D2=_tmp3C9->type;_npop_handler(0);return _tmp3D2;};}_LL1AA:;_LL1AB:
 goto _LL1A5;_LL1A5:;}
# 1676
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1679
const char*_tmp8E7;void*_tmp8E6[1];struct Cyc_String_pa_PrintArg_struct _tmp8E5;void*_tmp3D6=(_tmp8E5.tag=0,((_tmp8E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8E6[0]=& _tmp8E5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E7="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8E7,sizeof(char),44))),_tag_dyneither(_tmp8E6,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D6;};
# 1618
;_pop_region(r);}
# 1684
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1686
struct _tuple15 _tmp3D7=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp3D9;int _tmp3DA;struct _tuple15 _tmp3D8=_tmp3D7;_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;
if(!_tmp3DA){
const char*_tmp8EA;void*_tmp8E9;return(_tmp8E9=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8EA="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8EA,sizeof(char),47))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}{
# 1690
struct _handler_cons _tmp3DD;_push_handler(& _tmp3DD);{int _tmp3DF=0;if(setjmp(_tmp3DD.handler))_tmp3DF=1;if(!_tmp3DF){
{void*_tmp3E0=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3D9)).f2;_npop_handler(0);return _tmp3E0;};_pop_handler();}else{void*_tmp3DE=(void*)_exn_thrown;void*_tmp3E2=_tmp3DE;void*_tmp3E4;_LL1B2: {struct Cyc_List_Nth_exn_struct*_tmp3E3=(struct Cyc_List_Nth_exn_struct*)_tmp3E2;if(_tmp3E3->tag != Cyc_List_Nth)goto _LL1B4;}_LL1B3: {
# 1693
const char*_tmp8EF;void*_tmp8EE[2];struct Cyc_Int_pa_PrintArg_struct _tmp8ED;struct Cyc_Int_pa_PrintArg_struct _tmp8EC;return(_tmp8EC.tag=1,((_tmp8EC.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8ED.tag=1,((_tmp8ED.f1=(unsigned long)((int)_tmp3D9),((_tmp8EE[0]=& _tmp8ED,((_tmp8EE[1]=& _tmp8EC,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8EF="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8EF,sizeof(char),41))),_tag_dyneither(_tmp8EE,sizeof(void*),2)))))))))))));}_LL1B4: _tmp3E4=_tmp3E2;_LL1B5:(void)_rethrow(_tmp3E4);_LL1B1:;}};};}
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
const char*_tmp8F3;void*_tmp8F2[1];struct Cyc_String_pa_PrintArg_struct _tmp8F1;void*_tmp3EE=(_tmp8F1.tag=0,((_tmp8F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp8F2[0]=& _tmp8F1,Cyc_Tcexp_expr_err(_tmp3EA,e2->loc,topt,((_tmp8F3="expecting int subscript, found %s",_tag_dyneither(_tmp8F3,sizeof(char),34))),_tag_dyneither(_tmp8F2,sizeof(void*),1)))))));_npop_handler(0);return _tmp3EE;}{
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
const char*_tmp8F6;void*_tmp8F5;(_tmp8F5=0,Cyc_Tcutil_warn(e2->loc,((_tmp8F6="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp8F6,sizeof(char),63))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}}else{
# 1733
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp407=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp409;int _tmp40A;struct _tuple15 _tmp408=_tmp407;_tmp409=_tmp408.f1;_tmp40A=_tmp408.f2;
if(_tmp40A)
Cyc_Tcutil_check_bound(loc,_tmp409,_tmp3F4);}else{
# 1739
if(Cyc_Tcutil_is_bound_one(_tmp3F4) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F5)){
const char*_tmp8F9;void*_tmp8F8;(_tmp8F8=0,Cyc_Tcutil_warn(e1->loc,((_tmp8F9="subscript applied to pointer to one object",_tag_dyneither(_tmp8F9,sizeof(char),43))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3F4);}}
# 1744
Cyc_Tcenv_check_rgn_accessible(_tmp3EA,loc,_tmp3F3);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3F1),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3EA)){
const char*_tmp8FC;void*_tmp8FB;(_tmp8FB=0,Cyc_Tcutil_terr(e1->loc,((_tmp8FC="can't subscript an abstract pointer",_tag_dyneither(_tmp8FC,sizeof(char),36))),_tag_dyneither(_tmp8FB,sizeof(void*),0)));}{
void*_tmp40F=_tmp3F1;_npop_handler(0);return _tmp40F;};_LL1B9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F6->tag != 10)goto _LL1BB;else{_tmp3F7=_tmp3F6->f1;}}_LL1BA: {
void*_tmp410=Cyc_Tcexp_ithTupleType(_tmp3EA,loc,_tmp3F7,e2);_npop_handler(0);return _tmp410;}_LL1BB:;_LL1BC: {
const char*_tmp900;void*_tmp8FF[1];struct Cyc_String_pa_PrintArg_struct _tmp8FE;void*_tmp414=(_tmp8FE.tag=0,((_tmp8FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8FF[0]=& _tmp8FE,Cyc_Tcexp_expr_err(_tmp3EA,loc,topt,((_tmp900="subscript applied to %s",_tag_dyneither(_tmp900,sizeof(char),24))),_tag_dyneither(_tmp8FF,sizeof(void*),1)))))));_npop_handler(0);return _tmp414;}_LL1B6:;};};}
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
struct _tuple18*_tmp903;struct Cyc_List_List*_tmp902;fields=((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902->hd=((_tmp903=_cycalloc(sizeof(*_tmp903)),((_tmp903->f1=(*((struct _tuple18*)_tmp418->hd)).f1,((_tmp903->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp903)))))),((_tmp902->tl=fields,_tmp902))))));};}
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
struct _tuple18*_tmp906;struct Cyc_List_List*_tmp905;fields=((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905->hd=((_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906->f1=Cyc_Absyn_empty_tqual(0),((_tmp906->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp906)))))),((_tmp905->tl=fields,_tmp905))))));};
# 1783
;_pop_region(r);}{
# 1787
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp909;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp908;return(void*)((_tmp908=_cycalloc(sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=10,((_tmp909.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp909)))),_tmp908))));};}
# 1791
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1794
const char*_tmp90C;void*_tmp90B;return(_tmp90B=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp90C="tcCompoundLit",_tag_dyneither(_tmp90C,sizeof(char),14))),_tag_dyneither(_tmp90B,sizeof(void*),0)));}
# 1804 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1807
void*res_t2;
struct _RegionHandle _tmp424=_new_region("r");struct _RegionHandle*r=& _tmp424;_push_region(r);{
int _tmp425=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp90F;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp90E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp426=(_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=0,((_tmp90F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp425),_tmp90F)))),_tmp90E)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp426,loc);
# 1816
if(zero_term){
struct Cyc_Absyn_Exp*_tmp427=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp425 - 1);
if(!Cyc_Tcutil_is_zero(_tmp427)){
const char*_tmp912;void*_tmp911;(_tmp911=0,Cyc_Tcutil_terr(_tmp427->loc,((_tmp912="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp912,sizeof(char),45))),_tag_dyneither(_tmp911,sizeof(void*),0)));}}
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
const char*_tmp916;void*_tmp915[1];struct Cyc_String_pa_PrintArg_struct _tmp914;(_tmp914.tag=0,((_tmp914.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp915[0]=& _tmp914,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp916="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp916,sizeof(char),53))),_tag_dyneither(_tmp915,sizeof(void*),1)))))));}}
# 1835
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1840
void*_tmp42F=(void*)ds->hd;void*_tmp430=_tmp42F;struct Cyc_Absyn_Exp*_tmp433;_LL1C8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp431=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp430;if(_tmp431->tag != 1)goto _LL1CA;}_LL1C9:
# 1842
{const char*_tmp919;void*_tmp918;(_tmp918=0,Cyc_Tcutil_terr(loc,((_tmp919="only array index designators are supported",_tag_dyneither(_tmp919,sizeof(char),43))),_tag_dyneither(_tmp918,sizeof(void*),0)));}
goto _LL1C7;_LL1CA: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp432=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp430;if(_tmp432->tag != 0)goto _LL1C7;else{_tmp433=_tmp432->f1;}}_LL1CB:
# 1845
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp433);{
struct _tuple15 _tmp436=Cyc_Evexp_eval_const_uint_exp(_tmp433);unsigned int _tmp438;int _tmp439;struct _tuple15 _tmp437=_tmp436;_tmp438=_tmp437.f1;_tmp439=_tmp437.f2;
if(!_tmp439){
const char*_tmp91C;void*_tmp91B;(_tmp91B=0,Cyc_Tcutil_terr(_tmp433->loc,((_tmp91C="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp91C,sizeof(char),47))),_tag_dyneither(_tmp91B,sizeof(void*),0)));}else{
if(_tmp438 != offset){
const char*_tmp921;void*_tmp920[2];struct Cyc_Int_pa_PrintArg_struct _tmp91F;struct Cyc_Int_pa_PrintArg_struct _tmp91E;(_tmp91E.tag=1,((_tmp91E.f1=(unsigned long)((int)_tmp438),((_tmp91F.tag=1,((_tmp91F.f1=(unsigned long)offset,((_tmp920[0]=& _tmp91F,((_tmp920[1]=& _tmp91E,Cyc_Tcutil_terr(_tmp433->loc,((_tmp921="expecting index designator %d but found %d",_tag_dyneither(_tmp921,sizeof(char),43))),_tag_dyneither(_tmp920,sizeof(void*),2)))))))))))));}}
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
const char*_tmp925;void*_tmp924[1];struct Cyc_String_pa_PrintArg_struct _tmp923;(_tmp923.tag=0,((_tmp923.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp924[0]=& _tmp923,Cyc_Tcutil_terr(bound->loc,((_tmp925="expecting unsigned int, found %s",_tag_dyneither(_tmp925,sizeof(char),33))),_tag_dyneither(_tmp924,sizeof(void*),1)))))));}_LL1CC:;}
# 1882
if(!(vd->tq).real_const){
const char*_tmp928;void*_tmp927;(_tmp927=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp928="comprehension index variable is not declared const!",_tag_dyneither(_tmp928,sizeof(char),52))),_tag_dyneither(_tmp927,sizeof(void*),0)));}{
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
{void**_tmp929;_tmp44D=((_tmp929=_region_malloc(r,sizeof(*_tmp929)),((_tmp929[0]=_tmp453.elt_typ,_tmp929))));}
{struct Cyc_Absyn_Tqual*_tmp92A;_tmp44E=((_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A[0]=_tmp453.elt_tq,_tmp92A))));}
{union Cyc_Absyn_Constraint**_tmp92B;_tmp44F=((_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B[0]=(_tmp453.ptr_atts).zero_term,_tmp92B))));}
goto _LL1D1;_LL1D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp451;if(_tmp454->tag != 8)goto _LL1D6;else{_tmp455=(_tmp454->f1).elt_type;_tmp456=(_tmp454->f1).tq;_tmp457=(_tmp454->f1).num_elts;_tmp458=(_tmp454->f1).zero_term;}}_LL1D5:
# 1904
{void**_tmp92C;_tmp44D=((_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=_tmp455,_tmp92C))));}
{struct Cyc_Absyn_Tqual*_tmp92D;_tmp44E=((_tmp92D=_region_malloc(r,sizeof(*_tmp92D)),((_tmp92D[0]=_tmp456,_tmp92D))));}
{union Cyc_Absyn_Constraint**_tmp92E;_tmp44F=((_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E[0]=_tmp458,_tmp92E))));}
goto _LL1D1;_LL1D6:;_LL1D7:
# 1909
 goto _LL1D1;_LL1D1:;}{
# 1912
void*t=Cyc_Tcexp_tcExp(_tmp44C,_tmp44D,body);
# 1914
struct _RegionHandle _tmp45F=_new_region("temp");struct _RegionHandle*temp=& _tmp45F;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body)){
const char*_tmp931;void*_tmp930;(_tmp930=0,Cyc_Tcutil_terr(body->loc,((_tmp931="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp931,sizeof(char),49))),_tag_dyneither(_tmp930,sizeof(void*),0)));}
# 1918
if(_tmp44C->le == 0){
# 1920
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp934;void*_tmp933;(_tmp933=0,Cyc_Tcutil_terr(bound->loc,((_tmp934="bound is not constant",_tag_dyneither(_tmp934,sizeof(char),22))),_tag_dyneither(_tmp933,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp937;void*_tmp936;(_tmp936=0,Cyc_Tcutil_terr(bound->loc,((_tmp937="body is not constant",_tag_dyneither(_tmp937,sizeof(char),21))),_tag_dyneither(_tmp936,sizeof(void*),0)));}}
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
const char*_tmp93A;void*_tmp939;(_tmp939=0,Cyc_Tcutil_terr(body->loc,((_tmp93A="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp93A,sizeof(char),49))),_tag_dyneither(_tmp939,sizeof(void*),0)));}{
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
const char*_tmp93E;void*_tmp93D[1];struct Cyc_String_pa_PrintArg_struct _tmp93C;(_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp93D[0]=& _tmp93C,Cyc_Tcutil_terr(bound->loc,((_tmp93E="expecting unsigned int, found %s",_tag_dyneither(_tmp93E,sizeof(char),33))),_tag_dyneither(_tmp93D,sizeof(void*),1)))))));}_LL1D8:;}{
# 1967
struct _RegionHandle _tmp474=_new_region("r");struct _RegionHandle*r=& _tmp474;_push_region(r);
{void**_tmp475=0;
struct Cyc_Absyn_Tqual*_tmp476=0;
union Cyc_Absyn_Constraint**_tmp477=0;
# 1972
if(topt != 0){
void*_tmp478=Cyc_Tcutil_compress(*topt);void*_tmp479=_tmp478;struct Cyc_Absyn_PtrInfo _tmp47B;void*_tmp47D;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Exp*_tmp47F;union Cyc_Absyn_Constraint*_tmp480;_LL1DE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479;if(_tmp47A->tag != 5)goto _LL1E0;else{_tmp47B=_tmp47A->f1;}}_LL1DF:
# 1975
{void**_tmp93F;_tmp475=((_tmp93F=_region_malloc(r,sizeof(*_tmp93F)),((_tmp93F[0]=_tmp47B.elt_typ,_tmp93F))));}
{struct Cyc_Absyn_Tqual*_tmp940;_tmp476=((_tmp940=_region_malloc(r,sizeof(*_tmp940)),((_tmp940[0]=_tmp47B.elt_tq,_tmp940))));}
{union Cyc_Absyn_Constraint**_tmp941;_tmp477=((_tmp941=_region_malloc(r,sizeof(*_tmp941)),((_tmp941[0]=(_tmp47B.ptr_atts).zero_term,_tmp941))));}
goto _LL1DD;_LL1E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp479;if(_tmp47C->tag != 8)goto _LL1E2;else{_tmp47D=(_tmp47C->f1).elt_type;_tmp47E=(_tmp47C->f1).tq;_tmp47F=(_tmp47C->f1).num_elts;_tmp480=(_tmp47C->f1).zero_term;}}_LL1E1:
# 1980
{void**_tmp942;_tmp475=((_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942[0]=_tmp47D,_tmp942))));}
{struct Cyc_Absyn_Tqual*_tmp943;_tmp476=((_tmp943=_region_malloc(r,sizeof(*_tmp943)),((_tmp943[0]=_tmp47E,_tmp943))));}
{union Cyc_Absyn_Constraint**_tmp944;_tmp477=((_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=_tmp480,_tmp944))));}
goto _LL1DD;_LL1E2:;_LL1E3:
# 1985
 goto _LL1DD;_LL1DD:;}
# 1988
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,1,t);
if(_tmp475 != 0)Cyc_Tcutil_unify(*_tmp475,t);
# 1991
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp947;void*_tmp946;(_tmp946=0,Cyc_Tcutil_terr(bound->loc,((_tmp947="bound is not constant",_tag_dyneither(_tmp947,sizeof(char),22))),_tag_dyneither(_tmp946,sizeof(void*),0)));}}
# 1995
if(_tmp477 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp477)){
# 1998
struct Cyc_Absyn_Exp*_tmp489=Cyc_Absyn_uint_exp(1,0);_tmp489->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp489,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 2002
const char*_tmp94A;void*_tmp949;(_tmp949=0,Cyc_Tcutil_terr(loc,((_tmp94A="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp94A,sizeof(char),74))),_tag_dyneither(_tmp949,sizeof(void*),0)));};}{
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
struct Cyc_Absyn_Aggrdecl**_tmp94B;adptr=((_tmp94B=_cycalloc(sizeof(*_tmp94B)),((_tmp94B[0]=ad,_tmp94B))));};}else{
# 2031
{struct _handler_cons _tmp48F;_push_handler(& _tmp48F);{int _tmp491=0;if(setjmp(_tmp48F.handler))_tmp491=1;if(!_tmp491){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2031
;_pop_handler();}else{void*_tmp490=(void*)_exn_thrown;void*_tmp493=_tmp490;void*_tmp495;_LL1E5: {struct Cyc_Dict_Absent_exn_struct*_tmp494=(struct Cyc_Dict_Absent_exn_struct*)_tmp493;if(_tmp494->tag != Cyc_Dict_Absent)goto _LL1E7;}_LL1E6:
# 2034
{const char*_tmp94F;void*_tmp94E[1];struct Cyc_String_pa_PrintArg_struct _tmp94D;(_tmp94D.tag=0,((_tmp94D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp94E[0]=& _tmp94D,Cyc_Tcutil_terr(loc,((_tmp94F="unbound struct/union name %s",_tag_dyneither(_tmp94F,sizeof(char),29))),_tag_dyneither(_tmp94E,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1E7: _tmp495=_tmp493;_LL1E8:(void)_rethrow(_tmp495);_LL1E4:;}};}
# 2037
*ad_opt=ad;
*tn=ad->name;}
# 2040
if(ad->impl == 0){
{const char*_tmp957;void*_tmp956[1];const char*_tmp955;const char*_tmp954;struct Cyc_String_pa_PrintArg_struct _tmp953;(_tmp953.tag=0,((_tmp953.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp954="struct",_tag_dyneither(_tmp954,sizeof(char),7)):((_tmp955="union",_tag_dyneither(_tmp955,sizeof(char),6)))),((_tmp956[0]=& _tmp953,Cyc_Tcutil_terr(loc,((_tmp957="can't construct abstract %s",_tag_dyneither(_tmp957,sizeof(char),28))),_tag_dyneither(_tmp956,sizeof(void*),1)))))));}
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
struct _tuple12 _tmp958;struct _tuple12 _tmp4A0=(_tmp958.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp958.f2=rgn,_tmp958)));
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
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp95E;struct Cyc_Absyn_AggrInfo _tmp95D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp95C;res_typ=(void*)((_tmp95C=_cycalloc(sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95E.tag=11,((_tmp95E.f1=((_tmp95D.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp95D.targs=_tmp4A3,_tmp95D)))),_tmp95E)))),_tmp95C))));}_LL1E9:;}else{
# 2087
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp964;struct Cyc_Absyn_AggrInfo _tmp963;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp962;res_typ=(void*)((_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962[0]=((_tmp964.tag=11,((_tmp964.f1=((_tmp963.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp963.targs=_tmp4A3,_tmp963)))),_tmp964)))),_tmp962))));}{
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
const char*_tmp967;void*_tmp966;(_tmp966=0,Cyc_Tcutil_terr(loc,((_tmp967="too many explicit witness types",_tag_dyneither(_tmp967,sizeof(char),32))),_tag_dyneither(_tmp966,sizeof(void*),0)));}
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
{const char*_tmp973;void*_tmp972[5];struct Cyc_String_pa_PrintArg_struct _tmp971;const char*_tmp970;const char*_tmp96F;struct Cyc_String_pa_PrintArg_struct _tmp96E;struct Cyc_String_pa_PrintArg_struct _tmp96D;struct Cyc_String_pa_PrintArg_struct _tmp96C;struct Cyc_String_pa_PrintArg_struct _tmp96B;(_tmp96B.tag=0,((_tmp96B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2121
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4B9->topt))),((_tmp96C.tag=0,((_tmp96C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2120
Cyc_Absynpp_typ2string(_tmp4BA)),((_tmp96D.tag=0,((_tmp96D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp96E.tag=0,((_tmp96E.f1=(struct _dyneither_ptr)(
# 2119
ad->kind == Cyc_Absyn_StructA?(_tmp96F="struct",_tag_dyneither(_tmp96F,sizeof(char),7)):((_tmp970="union",_tag_dyneither(_tmp970,sizeof(char),6)))),((_tmp971.tag=0,((_tmp971.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B8->name),((_tmp972[0]=& _tmp971,((_tmp972[1]=& _tmp96E,((_tmp972[2]=& _tmp96D,((_tmp972[3]=& _tmp96C,((_tmp972[4]=& _tmp96B,Cyc_Tcutil_terr(_tmp4B9->loc,((_tmp973="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp973,sizeof(char),40))),_tag_dyneither(_tmp972,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2122
Cyc_Tcutil_explain_failure();}};}{
# 2126
struct Cyc_List_List*_tmp4C4=0;
{struct Cyc_List_List*_tmp4C5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp4C5 != 0;_tmp4C5=_tmp4C5->tl){
struct _tuple0*_tmp976;struct Cyc_List_List*_tmp975;_tmp4C4=((_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975->hd=((_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,(*((struct _tuple0*)_tmp4C5->hd)).f1),((_tmp976->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,(*((struct _tuple0*)_tmp4C5->hd)).f2),_tmp976)))))),((_tmp975->tl=_tmp4C4,_tmp975))))));}}
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
const char*_tmp979;void*_tmp978;(_tmp978=0,Cyc_Tcutil_terr(loc,((_tmp979="expecting struct but found union",_tag_dyneither(_tmp979,sizeof(char),33))),_tag_dyneither(_tmp978,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp4CF);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp4D2=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct _tuple19*_tmp4D3=_tmp4D2;_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp4D4->type,_tmp4D5);
# 2152
if(!Cyc_Tcutil_coerce_arg(te,_tmp4D5,_tmp4D4->type,& bogus)){
{const char*_tmp97F;void*_tmp97E[3];struct Cyc_String_pa_PrintArg_struct _tmp97D;struct Cyc_String_pa_PrintArg_struct _tmp97C;struct Cyc_String_pa_PrintArg_struct _tmp97B;(_tmp97B.tag=0,((_tmp97B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2155
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4D5->topt))),((_tmp97C.tag=0,((_tmp97C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2154
Cyc_Absynpp_typ2string(_tmp4D4->type)),((_tmp97D.tag=0,((_tmp97D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D4->name),((_tmp97E[0]=& _tmp97D,((_tmp97E[1]=& _tmp97C,((_tmp97E[2]=& _tmp97B,Cyc_Tcutil_terr(_tmp4D5->loc,((_tmp97F="field %s of struct expects type %s != %s",_tag_dyneither(_tmp97F,sizeof(char),41))),_tag_dyneither(_tmp97E,sizeof(void*),3)))))))))))))))))));}
# 2156
Cyc_Tcutil_explain_failure();}}
# 2159
goto _LL1EE;};_LL1F1:;_LL1F2: {
const char*_tmp982;void*_tmp981;(_tmp981=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp982="tcAnonStruct: wrong type",_tag_dyneither(_tmp982,sizeof(char),25))),_tag_dyneither(_tmp981,sizeof(void*),0)));}_LL1EE:;}{
# 2163
void*_tmp4DD=ts;_npop_handler(0);return _tmp4DD;};
# 2140
;_pop_region(rgn);}
# 2167
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2170
struct _RegionHandle _tmp4DE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4DE;_push_region(rgn);
{struct _tuple12 _tmp983;struct _tuple12 _tmp4DF=(_tmp983.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp983.f2=rgn,_tmp983)));
struct Cyc_List_List*_tmp4E0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4DF,tud->tvs);
struct Cyc_List_List*_tmp4E1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4E0);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp989;struct Cyc_Absyn_DatatypeFieldInfo _tmp988;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp987;void*res=(void*)(
(_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987[0]=((_tmp989.tag=4,((_tmp989.f1=((_tmp988.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp988.targs=_tmp4E1,_tmp988)))),_tmp989)))),_tmp987))));
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
{const char*_tmp991;void*_tmp990[3];struct Cyc_String_pa_PrintArg_struct _tmp98F;struct Cyc_String_pa_PrintArg_struct _tmp98E;const char*_tmp98D;struct Cyc_String_pa_PrintArg_struct _tmp98C;(_tmp98C.tag=0,((_tmp98C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2195
e->topt == 0?(struct _dyneither_ptr)((_tmp98D="?",_tag_dyneither(_tmp98D,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp98E.tag=0,((_tmp98E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2194
Cyc_Absynpp_typ2string(t)),((_tmp98F.tag=0,((_tmp98F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp990[0]=& _tmp98F,((_tmp990[1]=& _tmp98E,((_tmp990[2]=& _tmp98C,Cyc_Tcutil_terr(e->loc,((_tmp991="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp991,sizeof(char),82))),_tag_dyneither(_tmp990,sizeof(void*),3)))))))))))))))))));}
# 2196
Cyc_Tcutil_explain_failure();}}
# 2199
if(es != 0){
const char*_tmp995;void*_tmp994[1];struct Cyc_String_pa_PrintArg_struct _tmp993;void*_tmp4EE=(_tmp993.tag=0,((_tmp993.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2202
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp994[0]=& _tmp993,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp995="too many arguments for datatype constructor %s",_tag_dyneither(_tmp995,sizeof(char),47))),_tag_dyneither(_tmp994,sizeof(void*),1)))))));_npop_handler(0);return _tmp4EE;}
if(ts != 0){
const char*_tmp999;void*_tmp998[1];struct Cyc_String_pa_PrintArg_struct _tmp997;void*_tmp4F2=(_tmp997.tag=0,((_tmp997.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp998[0]=& _tmp997,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp999="too few arguments for datatype constructor %s",_tag_dyneither(_tmp999,sizeof(char),46))),_tag_dyneither(_tmp998,sizeof(void*),1)))))));_npop_handler(0);return _tmp4F2;}{
void*_tmp4F3=res;_npop_handler(0);return _tmp4F3;};};}
# 2171
;_pop_region(rgn);}
# 2210
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2212
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2214
if(topt != 0){
void*_tmp4F8=Cyc_Tcutil_compress(*topt);void*_tmp4F9=_tmp4F8;void*_tmp4FB;_LL1F9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FA->tag != 5)goto _LL1FB;else{_tmp4FB=(_tmp4FA->f1).elt_typ;}}_LL1FA:
# 2217
 Cyc_Tcutil_unify(_tmp4FB,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL1F8;_LL1FB:;_LL1FC:
 goto _LL1F8;_LL1F8:;}{
# 2223
const char*_tmp9A2;void*_tmp9A1[2];const char*_tmp9A0;const char*_tmp99F;struct Cyc_String_pa_PrintArg_struct _tmp99E;struct Cyc_String_pa_PrintArg_struct _tmp99D;(_tmp99D.tag=0,((_tmp99D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp99E.tag=0,((_tmp99E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp99F="calloc",_tag_dyneither(_tmp99F,sizeof(char),7))):(struct _dyneither_ptr)((_tmp9A0="malloc",_tag_dyneither(_tmp9A0,sizeof(char),7))))),((_tmp9A1[0]=& _tmp99E,((_tmp9A1[1]=& _tmp99D,Cyc_Tcutil_terr(loc,((_tmp9A2="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp9A2,sizeof(char),60))),_tag_dyneither(_tmp9A1,sizeof(void*),2)))))))))))));};}
# 2227
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2229
enum Cyc_Absyn_AliasQual _tmp502=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;switch(_tmp502){case Cyc_Absyn_Unique: _LL1FD:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL1FE:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2235
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2240
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2243
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9A5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9A4;void*expected_type=(void*)(
(_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=15,((_tmp9A5.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp9A5)))),_tmp9A4))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp503=Cyc_Tcutil_compress(handle_type);void*_tmp504=_tmp503;void*_tmp506;_LL201: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp504;if(_tmp505->tag != 15)goto _LL203;else{_tmp506=(void*)_tmp505->f1;}}_LL202:
# 2248
 rgn=_tmp506;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL200;_LL203:;_LL204:
# 2252
{const char*_tmp9A9;void*_tmp9A8[1];struct Cyc_String_pa_PrintArg_struct _tmp9A7;(_tmp9A7.tag=0,((_tmp9A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9A8[0]=& _tmp9A7,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp9A9="expecting region_t type but found %s",_tag_dyneither(_tmp9A9,sizeof(char),37))),_tag_dyneither(_tmp9A8,sizeof(void*),1)))))));}
goto _LL200;_LL200:;}else{
# 2259
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2266
{struct _RegionHandle _tmp50C=_new_region("r");struct _RegionHandle*r=& _tmp50C;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2277 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp9AC;void*_tmp9AB;(_tmp9AB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AC="calloc with empty type",_tag_dyneither(_tmp9AC,sizeof(char),23))),_tag_dyneither(_tmp9AB,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2289
void*er=(*e)->r;
retry_sizeof: {
void*_tmp50F=er;void*_tmp511;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp514;_LL206: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp510=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp50F;if(_tmp510->tag != 16)goto _LL208;else{_tmp511=(void*)_tmp510->f1;}}_LL207:
# 2293
 elt_type=_tmp511;
{void**_tmp9AD;*t=((_tmp9AD=_cycalloc(sizeof(*_tmp9AD)),((_tmp9AD[0]=elt_type,_tmp9AD))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL205;_LL208: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp50F;if(_tmp512->tag != 2)goto _LL20A;else{if(_tmp512->f1 != Cyc_Absyn_Times)goto _LL20A;if(_tmp512->f2 == 0)goto _LL20A;_tmp513=(struct Cyc_Absyn_Exp*)(_tmp512->f2)->hd;if((_tmp512->f2)->tl == 0)goto _LL20A;_tmp514=(struct Cyc_Absyn_Exp*)((_tmp512->f2)->tl)->hd;if(((_tmp512->f2)->tl)->tl != 0)goto _LL20A;}}_LL209:
# 2300
{struct _tuple0 _tmp9AE;struct _tuple0 _tmp516=(_tmp9AE.f1=_tmp513->r,((_tmp9AE.f2=_tmp514->r,_tmp9AE)));struct _tuple0 _tmp517=_tmp516;void*_tmp519;void*_tmp51B;_LL20D: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp518=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp517.f1;if(_tmp518->tag != 16)goto _LL20F;else{_tmp519=(void*)_tmp518->f1;}}_LL20E:
# 2302
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp519);
elt_type=_tmp519;
{void**_tmp9AF;*t=((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF[0]=elt_type,_tmp9AF))));}
num_elts=_tmp514;
one_elt=0;
goto _LL20C;_LL20F: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp517.f2;if(_tmp51A->tag != 16)goto _LL211;else{_tmp51B=(void*)_tmp51A->f1;}}_LL210:
# 2309
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp51B);
elt_type=_tmp51B;
{void**_tmp9B0;*t=((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=elt_type,_tmp9B0))));}
num_elts=_tmp513;
one_elt=0;
goto _LL20C;_LL211:;_LL212:
 goto No_sizeof;_LL20C:;}
# 2317
goto _LL205;_LL20A:;_LL20B:
# 2319
 No_sizeof: {
# 2322
struct Cyc_Absyn_Exp*_tmp51F=*e;
{void*_tmp520=_tmp51F->r;void*_tmp521=_tmp520;struct Cyc_Absyn_Exp*_tmp523;_LL214: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp521;if(_tmp522->tag != 13)goto _LL216;else{_tmp523=_tmp522->f2;}}_LL215:
 _tmp51F=_tmp523;goto _LL213;_LL216:;_LL217:
 goto _LL213;_LL213:;}
# 2327
{void*_tmp524=Cyc_Tcutil_compress((void*)_check_null(_tmp51F->topt));void*_tmp525=_tmp524;void*_tmp527;_LL219: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp526=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp525;if(_tmp526->tag != 19)goto _LL21B;else{_tmp527=(void*)_tmp526->f1;}}_LL21A:
# 2329
{void*_tmp528=Cyc_Tcutil_compress(_tmp527);void*_tmp529=_tmp528;struct Cyc_Absyn_Exp*_tmp52B;_LL21E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp52A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp529;if(_tmp52A->tag != 18)goto _LL220;else{_tmp52B=_tmp52A->f1;}}_LL21F:
# 2331
 er=_tmp52B->r;goto retry_sizeof;_LL220:;_LL221:
 goto _LL21D;_LL21D:;}
# 2334
goto _LL218;_LL21B:;_LL21C:
 goto _LL218;_LL218:;}
# 2337
elt_type=Cyc_Absyn_char_typ;
{void**_tmp9B1;*t=((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=elt_type,_tmp9B1))));}
num_elts=*e;
one_elt=0;}
# 2342
goto _LL205;_LL205:;}}
# 2346
*is_fat=!one_elt;
# 2349
{void*_tmp52D=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp52F;_LL223: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp52E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52D;if(_tmp52E->tag != 11)goto _LL225;else{if((((_tmp52E->f1).aggr_info).KnownAggr).tag != 2)goto _LL225;_tmp52F=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp52E->f1).aggr_info).KnownAggr).val);}}_LL224:
# 2351
 if(_tmp52F->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp52F->impl))->exist_vars != 0){
const char*_tmp9B4;void*_tmp9B3;(_tmp9B3=0,Cyc_Tcutil_terr(loc,((_tmp9B4="malloc with existential types not yet implemented",_tag_dyneither(_tmp9B4,sizeof(char),50))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));}
goto _LL222;_LL225:;_LL226:
 goto _LL222;_LL222:;}{
# 2358
void*(*_tmp532)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp533=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp534=Cyc_Tcutil_compress(*topt);void*_tmp535=_tmp534;union Cyc_Absyn_Constraint*_tmp537;union Cyc_Absyn_Constraint*_tmp538;union Cyc_Absyn_Constraint*_tmp539;_LL228: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp536=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp535;if(_tmp536->tag != 5)goto _LL22A;else{_tmp537=((_tmp536->f1).ptr_atts).nullable;_tmp538=((_tmp536->f1).ptr_atts).bounds;_tmp539=((_tmp536->f1).ptr_atts).zero_term;}}_LL229:
# 2363
 _tmp533=_tmp539;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp537))
_tmp532=Cyc_Absyn_star_typ;
# 2368
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp539) && !(*is_calloc)){
{const char*_tmp9B7;void*_tmp9B6;(_tmp9B6=0,Cyc_Tcutil_warn(loc,((_tmp9B7="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp9B7,sizeof(char),55))),_tag_dyneither(_tmp9B6,sizeof(void*),0)));}
*is_calloc=1;}
# 2374
{void*_tmp53C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp538);void*_tmp53D=_tmp53C;struct Cyc_Absyn_Exp*_tmp540;_LL22D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp53E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp53D;if(_tmp53E->tag != 0)goto _LL22F;}_LL22E:
 goto _LL22C;_LL22F:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp53F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp53D;if(_tmp53F->tag != 1)goto _LL231;else{_tmp540=_tmp53F->f1;}}if(!(!one_elt))goto _LL231;_LL230: {
# 2377
int _tmp541=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp541  && Cyc_Evexp_same_const_exp(_tmp540,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp538),_tmp533);}
# 2383
{void*_tmp542=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp543=_tmp542;void*_tmp545;_LL234: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp543;if(_tmp544->tag != 19)goto _LL236;else{_tmp545=(void*)_tmp544->f1;}}_LL235: {
# 2385
struct Cyc_Absyn_Exp*_tmp546=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp545,0),0,Cyc_Absyn_No_coercion,0);
# 2387
if(Cyc_Evexp_same_const_exp(_tmp546,_tmp540)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp538),_tmp533);}
# 2392
goto _LL233;}_LL236:;_LL237:
 goto _LL233;_LL233:;}
# 2395
goto _LL22C;}_LL231:;_LL232:
 goto _LL22C;_LL22C:;}
# 2398
goto _LL227;_LL22A:;_LL22B:
 goto _LL227;_LL227:;}
# 2401
if(!one_elt)_tmp532=Cyc_Absyn_dyneither_typ;
return _tmp532(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp533);};};}
# 2406
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2412
struct _RegionHandle _tmp547=_new_region("r");struct _RegionHandle*r=& _tmp547;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp548=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExpNoPromote(_tmp548,0,e1);{
void*_tmp549=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp548,& _tmp549,e2);};}{
# 2418
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2421
{void*_tmp54A=Cyc_Tcutil_compress(t1);void*_tmp54B=_tmp54A;_LL239: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp54C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54B;if(_tmp54C->tag != 8)goto _LL23B;}_LL23A:
{const char*_tmp9BA;void*_tmp9B9;(_tmp9B9=0,Cyc_Tcutil_terr(loc,((_tmp9BA="cannot assign to an array",_tag_dyneither(_tmp9BA,sizeof(char),26))),_tag_dyneither(_tmp9B9,sizeof(void*),0)));}goto _LL238;_LL23B:;_LL23C:
 goto _LL238;_LL238:;}
# 2426
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1)){
const char*_tmp9BD;void*_tmp9BC;(_tmp9BC=0,Cyc_Tcutil_terr(loc,((_tmp9BD="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp9BD,sizeof(char),58))),_tag_dyneither(_tmp9BC,sizeof(void*),0)));}
# 2430
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp9C0;void*_tmp9BF;void*_tmp553=(_tmp9BF=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9C0="swap non-lvalue",_tag_dyneither(_tmp9C0,sizeof(char),16))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));_npop_handler(0);return _tmp553;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp9C3;void*_tmp9C2;void*_tmp556=(_tmp9C2=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9C3="swap non-lvalue",_tag_dyneither(_tmp9C3,sizeof(char),16))),_tag_dyneither(_tmp9C2,sizeof(void*),0)));_npop_handler(0);return _tmp556;}{
# 2435
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9C6;void*_tmp9C5;void*_tmp559=(_tmp9C5=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9C6="swap value in zeroterm array",_tag_dyneither(_tmp9C6,sizeof(char),29))),_tag_dyneither(_tmp9C5,sizeof(void*),0)));_npop_handler(0);return _tmp559;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9C9;void*_tmp9C8;void*_tmp55C=(_tmp9C8=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9C9="swap value in zeroterm array",_tag_dyneither(_tmp9C9,sizeof(char),29))),_tag_dyneither(_tmp9C8,sizeof(void*),0)));_npop_handler(0);return _tmp55C;}
# 2442
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp9CE;void*_tmp9CD[2];struct Cyc_String_pa_PrintArg_struct _tmp9CC;struct Cyc_String_pa_PrintArg_struct _tmp9CB;void*_tmp55D=(_tmp9CB.tag=0,((_tmp9CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp9CC.tag=0,((_tmp9CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9CD[0]=& _tmp9CC,((_tmp9CD[1]=& _tmp9CB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9CE="type mismatch: %s != %s",_tag_dyneither(_tmp9CE,sizeof(char),24))),_tag_dyneither(_tmp9CD,sizeof(void*),2)))))))))))));
void*_tmp55E=_tmp55D;_npop_handler(0);return _tmp55E;}{
# 2449
void*_tmp563=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp563;};};};
# 2412
;_pop_region(r);}
# 2453
int Cyc_Tcexp_in_stmt_exp=0;
# 2455
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp564=_new_region("r");struct _RegionHandle*r=& _tmp564;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2462
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2464
while(1){
void*_tmp565=s->r;void*_tmp566=_tmp565;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Stmt*_tmp56A;struct Cyc_Absyn_Stmt*_tmp56B;struct Cyc_Absyn_Decl*_tmp56D;struct Cyc_Absyn_Stmt*_tmp56E;_LL23E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp567=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp567->tag != 1)goto _LL240;else{_tmp568=_tmp567->f1;}}_LL23F: {
# 2468
void*_tmp56F=(void*)_check_null(_tmp568->topt);
if(!Cyc_Tcutil_unify(_tmp56F,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp9D2;void*_tmp9D1[1];struct Cyc_String_pa_PrintArg_struct _tmp9D0;(_tmp9D0.tag=0,((_tmp9D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp56F)),((_tmp9D1[0]=& _tmp9D0,Cyc_Tcutil_terr(loc,((_tmp9D2="statement expression returns type %s",_tag_dyneither(_tmp9D2,sizeof(char),37))),_tag_dyneither(_tmp9D1,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2474
void*_tmp573=_tmp56F;_npop_handler(0);return _tmp573;};}_LL240: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp569=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp569->tag != 2)goto _LL242;else{_tmp56A=_tmp569->f1;_tmp56B=_tmp569->f2;}}_LL241:
 s=_tmp56B;continue;_LL242: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp56C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp566;if(_tmp56C->tag != 12)goto _LL244;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;}}_LL243:
 s=_tmp56E;continue;_LL244:;_LL245: {
# 2478
const char*_tmp9D5;void*_tmp9D4;void*_tmp576=(_tmp9D4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9D5="statement expression must end with expression",_tag_dyneither(_tmp9D5,sizeof(char),46))),_tag_dyneither(_tmp9D4,sizeof(void*),0)));_npop_handler(0);return _tmp576;}_LL23D:;}
# 2456
;_pop_region(r);}
# 2483
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp577=_new_region("r");struct _RegionHandle*r=& _tmp577;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2488
{void*_tmp578=t;struct Cyc_Absyn_Aggrdecl*_tmp57A;_LL247: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp578;if(_tmp579->tag != 11)goto _LL249;else{if((((_tmp579->f1).aggr_info).KnownAggr).tag != 2)goto _LL249;_tmp57A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp579->f1).aggr_info).KnownAggr).val);}}_LL248:
# 2490
 if((_tmp57A->kind == Cyc_Absyn_UnionA  && _tmp57A->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57A->impl))->tagged)goto _LL246;
goto _LL24A;_LL249:;_LL24A:
# 2493
{const char*_tmp9D9;void*_tmp9D8[1];struct Cyc_String_pa_PrintArg_struct _tmp9D7;(_tmp9D7.tag=0,((_tmp9D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9D8[0]=& _tmp9D7,Cyc_Tcutil_terr(loc,((_tmp9D9="expecting @tagged union but found %s",_tag_dyneither(_tmp9D9,sizeof(char),37))),_tag_dyneither(_tmp9D8,sizeof(void*),1)))))));}
goto _LL246;_LL246:;}{
# 2496
void*_tmp57E=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp57E;};
# 2485
;_pop_region(r);}
# 2500
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2504
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp57F=_new_region("r");struct _RegionHandle*r=& _tmp57F;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp580=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2509
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9DC;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9DB;void*expected_type=(void*)(
(_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DC.tag=15,((_tmp9DC.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp580)),_tmp9DC)))),_tmp9DB))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp580,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp581=Cyc_Tcutil_compress(handle_type);void*_tmp582=_tmp581;void*_tmp584;_LL24C: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp583=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp582;if(_tmp583->tag != 15)goto _LL24E;else{_tmp584=(void*)_tmp583->f1;}}_LL24D:
# 2514
 rgn=_tmp584;
Cyc_Tcenv_check_rgn_accessible(_tmp580,loc,rgn);
goto _LL24B;_LL24E:;_LL24F:
# 2518
{const char*_tmp9E0;void*_tmp9DF[1];struct Cyc_String_pa_PrintArg_struct _tmp9DE;(_tmp9DE.tag=0,((_tmp9DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9DF[0]=& _tmp9DE,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp9E0="expecting region_t type but found %s",_tag_dyneither(_tmp9E0,sizeof(char),37))),_tag_dyneither(_tmp9DF,sizeof(void*),1)))))));}
goto _LL24B;_LL24B:;}else{
# 2525
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2533
void*_tmp58A=e1->r;void*_tmp58B=_tmp58A;struct Cyc_Core_Opt*_tmp58F;struct Cyc_List_List*_tmp590;struct Cyc_List_List*_tmp592;_LL251: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp58C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp58B;if(_tmp58C->tag != 26)goto _LL253;}_LL252:
 goto _LL254;_LL253: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp58D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp58B;if(_tmp58D->tag != 27)goto _LL255;}_LL254: {
# 2538
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp580,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmp9E3;void*_tmp9E2;(_tmp9E2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9E3="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9E3,sizeof(char),45))),_tag_dyneither(_tmp9E2,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp580,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp580,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2549
void*_tmp597=res_typ;_npop_handler(0);return _tmp597;};}_LL255: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp58B;if(_tmp58E->tag != 35)goto _LL257;else{_tmp58F=_tmp58E->f1;_tmp590=_tmp58E->f2;}}_LL256:
# 2551
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9E6;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9E5;e1->r=(void*)((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=((_tmp9E6.tag=25,((_tmp9E6.f1=_tmp590,_tmp9E6)))),_tmp9E5))));}
_tmp592=_tmp590;goto _LL258;_LL257: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp591=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp58B;if(_tmp591->tag != 25)goto _LL259;else{_tmp592=_tmp591->f1;}}_LL258: {
# 2554
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp59A=Cyc_Tcutil_compress(*topt);void*_tmp59B=_tmp59A;void**_tmp59D;struct Cyc_Absyn_Tqual _tmp59E;union Cyc_Absyn_Constraint*_tmp59F;_LL260: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59B;if(_tmp59C->tag != 5)goto _LL262;else{_tmp59D=(void**)&(_tmp59C->f1).elt_typ;_tmp59E=(_tmp59C->f1).elt_tq;_tmp59F=((_tmp59C->f1).ptr_atts).zero_term;}}_LL261:
# 2560
 elt_typ_opt=_tmp59D;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59F);
goto _LL25F;_LL262:;_LL263:
 goto _LL25F;_LL25F:;}{
# 2566
void*res_typ=Cyc_Tcexp_tcArray(_tmp580,e1->loc,elt_typ_opt,0,zero_term,_tmp592);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmp9E9;void*_tmp9E8;(_tmp9E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9E9="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp9E9,sizeof(char),50))),_tag_dyneither(_tmp9E8,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2575
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp580,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp580,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2581
void*_tmp5A2=res_typ;_npop_handler(0);return _tmp5A2;};};}_LL259: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp593=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp58B;if(_tmp593->tag != 0)goto _LL25B;else{if(((_tmp593->f1).String_c).tag != 8)goto _LL25B;}}_LL25A: {
# 2586
void*_tmp5A3=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2588
void*_tmp5A4=Cyc_Tcexp_tcExp(_tmp580,& _tmp5A3,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9EC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9EB;void*_tmp5A7=Cyc_Absyn_atb_typ(_tmp5A4,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB[0]=((_tmp9EC.tag=1,((_tmp9EC.f1=Cyc_Absyn_uint_exp(1,0),_tmp9EC)))),_tmp9EB)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5A7;}_LL25B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp594=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp58B;if(_tmp594->tag != 0)goto _LL25D;else{if(((_tmp594->f1).Wstring_c).tag != 9)goto _LL25D;}}_LL25C: {
# 2593
void*_tmp5A8=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2595
void*_tmp5A9=Cyc_Tcexp_tcExp(_tmp580,& _tmp5A8,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9EF;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9EE;void*_tmp5AC=Cyc_Absyn_atb_typ(_tmp5A9,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=1,((_tmp9EF.f1=Cyc_Absyn_uint_exp(1,0),_tmp9EF)))),_tmp9EE)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5AC;}_LL25D:;_LL25E:
# 2602
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp5AD=Cyc_Tcutil_compress(*topt);void*_tmp5AE=_tmp5AD;void**_tmp5B0;struct Cyc_Absyn_Tqual _tmp5B1;_LL265: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AE;if(_tmp5AF->tag != 5)goto _LL267;else{_tmp5B0=(void**)&(_tmp5AF->f1).elt_typ;_tmp5B1=(_tmp5AF->f1).elt_tq;}}_LL266:
# 2608
 topt2=_tmp5B0;goto _LL264;_LL267: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5AE;if(_tmp5B2->tag != 3)goto _LL269;}_LL268:
# 2612
 bogus=*topt;
topt2=& bogus;
goto _LL264;_LL269:;_LL26A:
 goto _LL264;_LL264:;}{
# 2618
void*telt=Cyc_Tcexp_tcExp(_tmp580,topt2,e1);
# 2620
struct _RegionHandle _tmp5B3=_new_region("temp");struct _RegionHandle*temp=& _tmp5B3;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1)){
const char*_tmp9F2;void*_tmp9F1;(_tmp9F1=0,Cyc_Tcutil_terr(e1->loc,((_tmp9F2="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9F2,sizeof(char),49))),_tag_dyneither(_tmp9F1,sizeof(void*),0)));}{
# 2625
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9F8;struct Cyc_Absyn_PtrInfo _tmp9F7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F6;void*res_typ=(void*)(
(_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6[0]=((_tmp9F8.tag=5,((_tmp9F8.f1=((_tmp9F7.elt_typ=telt,((_tmp9F7.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp9F7.ptr_atts=(
((_tmp9F7.ptr_atts).rgn=rgn,(((_tmp9F7.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9F7.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp9F7.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp9F7.ptr_atts).ptrloc=0,_tmp9F7.ptr_atts)))))))))),_tmp9F7)))))),_tmp9F8)))),_tmp9F6))));
# 2630
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp580,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp580,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2637
void*_tmp5B6=res_typ;_npop_handler(1);return _tmp5B6;};};
# 2620
;_pop_region(temp);};}_LL250:;};}
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
struct _RegionHandle _tmp5BA=_new_region("temp");struct _RegionHandle*temp=& _tmp5BA;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2664
const char*_tmp9FB;void*_tmp9FA;(_tmp9FA=0,Cyc_Tcutil_terr(e->loc,((_tmp9FB="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9FB,sizeof(char),49))),_tag_dyneither(_tmp9FA,sizeof(void*),0)));}{
# 2667
void*_tmp5BD=e->r;void*_tmp5BE=_tmp5BD;_LL26C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5BF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5BE;if(_tmp5BF->tag != 25)goto _LL26E;}_LL26D:
 goto _LL26F;_LL26E: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5C0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BE;if(_tmp5C0->tag != 26)goto _LL270;}_LL26F:
 goto _LL271;_LL270: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5C1=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5BE;if(_tmp5C1->tag != 27)goto _LL272;}_LL271:
 goto _LL273;_LL272: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5C2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BE;if(_tmp5C2->tag != 0)goto _LL274;else{if(((_tmp5C2->f1).Wstring_c).tag != 9)goto _LL274;}}_LL273:
 goto _LL275;_LL274: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5C3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BE;if(_tmp5C3->tag != 0)goto _LL276;else{if(((_tmp5C3->f1).String_c).tag != 8)goto _LL276;}}_LL275: {
void*_tmp5C4=t;_npop_handler(0);return _tmp5C4;}_LL276:;_LL277:
# 2674
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2677
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}{
# 2680
void*_tmp5C5=t;_npop_handler(0);return _tmp5C5;};_LL26B:;};
# 2658
;_pop_region(temp);}
# 2691 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp5C6=e->r;void*_tmp5C7=_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C9;_LL279: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5C8=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5C8->tag != 11)goto _LL27B;else{_tmp5C9=_tmp5C8->f1;}}_LL27A:
# 2695
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5C9);
_tmp5C9->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5C9->topt),0);
e->topt=_tmp5C9->topt;
goto _LL278;_LL27B:;_LL27C:
# 2701
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2704
{void*_tmp5CA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5CB=_tmp5CA;void*_tmp5CD;struct Cyc_Absyn_Tqual _tmp5CE;void*_tmp5CF;union Cyc_Absyn_Constraint*_tmp5D0;union Cyc_Absyn_Constraint*_tmp5D1;union Cyc_Absyn_Constraint*_tmp5D2;_LL27E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CB;if(_tmp5CC->tag != 5)goto _LL280;else{_tmp5CD=(_tmp5CC->f1).elt_typ;_tmp5CE=(_tmp5CC->f1).elt_tq;_tmp5CF=((_tmp5CC->f1).ptr_atts).rgn;_tmp5D0=((_tmp5CC->f1).ptr_atts).nullable;_tmp5D1=((_tmp5CC->f1).ptr_atts).bounds;_tmp5D2=((_tmp5CC->f1).ptr_atts).zero_term;}}_LL27F:
# 2706
{void*_tmp5D3=Cyc_Tcutil_compress(_tmp5CD);void*_tmp5D4=_tmp5D3;struct Cyc_List_List*_tmp5D6;void*_tmp5D7;struct Cyc_Absyn_Tqual _tmp5D8;void*_tmp5D9;struct Cyc_List_List*_tmp5DA;int _tmp5DB;struct Cyc_Absyn_VarargInfo*_tmp5DC;struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*_tmp5DE;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E1;struct Cyc_List_List*_tmp5E2;_LL283: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D4;if(_tmp5D5->tag != 9)goto _LL285;else{_tmp5D6=(_tmp5D5->f1).tvars;_tmp5D7=(_tmp5D5->f1).effect;_tmp5D8=(_tmp5D5->f1).ret_tqual;_tmp5D9=(_tmp5D5->f1).ret_typ;_tmp5DA=(_tmp5D5->f1).args;_tmp5DB=(_tmp5D5->f1).c_varargs;_tmp5DC=(_tmp5D5->f1).cyc_varargs;_tmp5DD=(_tmp5D5->f1).rgn_po;_tmp5DE=(_tmp5D5->f1).attributes;_tmp5DF=(_tmp5D5->f1).requires_clause;_tmp5E0=(_tmp5D5->f1).requires_relns;_tmp5E1=(_tmp5D5->f1).ensures_clause;_tmp5E2=(_tmp5D5->f1).ensures_relns;}}_LL284:
# 2708
 if(_tmp5D6 != 0){
struct _RegionHandle _tmp5E3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5E3;_push_region(rgn);
{struct _tuple12 _tmp9FC;struct _tuple12 _tmp5E4=(_tmp9FC.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9FC.f2=rgn,_tmp9FC)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5E4,_tmp5D6);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2717
_tmp5DD=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5DD);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp5DD);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA02;struct Cyc_Absyn_FnInfo _tmpA01;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA00;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA02.tag=9,((_tmpA02.f1=((_tmpA01.tvars=0,((_tmpA01.effect=_tmp5D7,((_tmpA01.ret_tqual=_tmp5D8,((_tmpA01.ret_typ=_tmp5D9,((_tmpA01.args=_tmp5DA,((_tmpA01.c_varargs=_tmp5DB,((_tmpA01.cyc_varargs=_tmp5DC,((_tmpA01.rgn_po=0,((_tmpA01.attributes=_tmp5DE,((_tmpA01.requires_clause=_tmp5DF,((_tmpA01.requires_relns=_tmp5E0,((_tmpA01.ensures_clause=_tmp5E1,((_tmpA01.ensures_relns=_tmp5E2,_tmpA01)))))))))))))))))))))))))),_tmpA02)))),_tmpA00)))));
# 2724
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA08;struct Cyc_Absyn_PtrInfo _tmpA07;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA06;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E5=(_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06[0]=((_tmpA08.tag=5,((_tmpA08.f1=((_tmpA07.elt_typ=ftyp,((_tmpA07.elt_tq=_tmp5CE,((_tmpA07.ptr_atts=(((_tmpA07.ptr_atts).rgn=_tmp5CF,(((_tmpA07.ptr_atts).nullable=_tmp5D0,(((_tmpA07.ptr_atts).bounds=_tmp5D1,(((_tmpA07.ptr_atts).zero_term=_tmp5D2,(((_tmpA07.ptr_atts).ptrloc=0,_tmpA07.ptr_atts)))))))))),_tmpA07)))))),_tmpA08)))),_tmpA06)));
# 2726
struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpA0B;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA0A;e->r=(void*)((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=12,((_tmpA0B.f1=_tmp5E6,((_tmpA0B.f2=ts,_tmpA0B)))))),_tmpA0A))));}
e->topt=(void*)_tmp5E5;};}
# 2710
;_pop_region(rgn);}
# 2731
goto _LL282;_LL285:;_LL286:
 goto _LL282;_LL282:;}
# 2734
goto _LL27D;_LL280:;_LL281:
 goto _LL27D;_LL27D:;}
# 2737
goto _LL278;_LL278:;}
# 2739
return(void*)_check_null(e->topt);}
# 2747
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2749
struct _RegionHandle _tmp5F0=_new_region("r");struct _RegionHandle*r=& _tmp5F0;_push_region(r);
{struct Cyc_List_List*_tmp5F1=0;
# 2752
{void*_tmp5F2=fn_exp->r;void*_tmp5F3=_tmp5F2;struct Cyc_List_List*_tmp5F5;_LL288: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5F4=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp5F4->tag != 9)goto _LL28A;else{_tmp5F5=_tmp5F4->f2;}}_LL289:
# 2754
{void*_tmp5F6=e->r;void*_tmp5F7=_tmp5F6;struct Cyc_List_List*_tmp5F9;_LL28D: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5F8=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F7;if(_tmp5F8->tag != 9)goto _LL28F;else{_tmp5F9=_tmp5F8->f2;}}_LL28E: {
# 2756
struct Cyc_List_List*_tmp5FA=alias_arg_exps;
int _tmp5FB=0;
while(_tmp5FA != 0){
while(_tmp5FB != (int)_tmp5FA->hd){
if(_tmp5F5 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmpA15;struct Cyc_Int_pa_PrintArg_struct _tmpA14;void*_tmpA13[2];const char*_tmpA12;void*_tmpA11;(_tmpA11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA15.tag=1,((_tmpA15.f1=(unsigned long)((int)_tmp5FA->hd),((_tmpA14.tag=1,((_tmpA14.f1=(unsigned long)_tmp5FB,((_tmpA13[0]=& _tmpA14,((_tmpA13[1]=& _tmpA15,Cyc_aprintf(((_tmpA12="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA12,sizeof(char),36))),_tag_dyneither(_tmpA13,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA11,sizeof(void*),0)));}
# 2763
++ _tmp5FB;
_tmp5F5=_tmp5F5->tl;
_tmp5F9=((struct Cyc_List_List*)_check_null(_tmp5F9))->tl;}{
# 2768
struct _tuple11 _tmp601=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5F5))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5F9))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp603;struct Cyc_Absyn_Exp*_tmp604;struct _tuple11 _tmp602=_tmp601;_tmp603=_tmp602.f1;_tmp604=_tmp602.f2;
_tmp5F5->hd=(void*)_tmp604;
{struct Cyc_List_List*_tmpA16;_tmp5F1=((_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16->hd=_tmp603,((_tmpA16->tl=_tmp5F1,_tmpA16))))));}
_tmp5FA=_tmp5FA->tl;};}
# 2773
goto _LL28C;}_LL28F:;_LL290: {
const char*_tmpA19;void*_tmpA18;(_tmpA18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA19="not a function call!",_tag_dyneither(_tmpA19,sizeof(char),21))),_tag_dyneither(_tmpA18,sizeof(void*),0)));}_LL28C:;}
# 2776
goto _LL287;_LL28A:;_LL28B: {
const char*_tmpA1C;void*_tmpA1B;(_tmpA1B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA1C="not a function call!",_tag_dyneither(_tmpA1C,sizeof(char),21))),_tag_dyneither(_tmpA1B,sizeof(void*),0)));}_LL287:;}
# 2781
while(_tmp5F1 != 0){
struct Cyc_Absyn_Decl*_tmp60A=(struct Cyc_Absyn_Decl*)_tmp5F1->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp60A,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5F1=_tmp5F1->tl;}
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
{void*_tmp60B=e->r;void*_tmp60C=_tmp60B;struct Cyc_Absyn_Exp*_tmp60E;struct Cyc_Absyn_Exp*_tmp610;struct Cyc_List_List*_tmp611;struct Cyc_Absyn_VarargCallInfo**_tmp612;int*_tmp613;struct Cyc_Core_Opt*_tmp615;struct Cyc_List_List*_tmp616;union Cyc_Absyn_Cnst*_tmp618;struct _tuple1*_tmp61A;void**_tmp61B;enum Cyc_Absyn_Primop _tmp61D;struct Cyc_List_List*_tmp61E;struct Cyc_Absyn_Exp*_tmp620;enum Cyc_Absyn_Incrementor _tmp621;struct Cyc_Absyn_Exp*_tmp623;struct Cyc_Core_Opt*_tmp624;struct Cyc_Absyn_Exp*_tmp625;struct Cyc_Absyn_Exp*_tmp627;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_Absyn_Exp*_tmp629;struct Cyc_Absyn_Exp*_tmp62B;struct Cyc_Absyn_Exp*_tmp62C;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Exp*_tmp62F;struct Cyc_Absyn_Exp*_tmp631;struct Cyc_Absyn_Exp*_tmp632;struct Cyc_Absyn_Exp*_tmp634;struct Cyc_List_List*_tmp635;struct Cyc_Absyn_VarargCallInfo**_tmp636;struct Cyc_Absyn_Exp*_tmp638;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_List_List*_tmp63B;void*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;enum Cyc_Absyn_Coercion*_tmp63F;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp**_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp646;void*_tmp648;void*_tmp64A;struct Cyc_List_List*_tmp64B;struct Cyc_Absyn_Exp*_tmp64D;struct Cyc_Absyn_Exp*_tmp64F;struct _dyneither_ptr*_tmp650;int*_tmp651;int*_tmp652;struct Cyc_Absyn_Exp*_tmp654;struct _dyneither_ptr*_tmp655;int*_tmp656;int*_tmp657;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_List_List*_tmp65C;struct _tuple9*_tmp65E;struct Cyc_List_List*_tmp65F;struct Cyc_List_List*_tmp661;struct Cyc_Absyn_Stmt*_tmp663;struct Cyc_Absyn_Vardecl*_tmp665;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Exp*_tmp667;int*_tmp668;struct Cyc_Absyn_Exp*_tmp66A;void*_tmp66B;int*_tmp66C;struct _tuple1**_tmp66E;struct Cyc_List_List**_tmp66F;struct Cyc_List_List*_tmp670;struct Cyc_Absyn_Aggrdecl**_tmp671;void*_tmp673;struct Cyc_List_List*_tmp674;struct Cyc_List_List*_tmp676;struct Cyc_Absyn_Datatypedecl*_tmp677;struct Cyc_Absyn_Datatypefield*_tmp678;struct _tuple1**_tmp67A;struct Cyc_Absyn_Enumdecl*_tmp67B;struct Cyc_Absyn_Enumfield*_tmp67C;struct _tuple1**_tmp67E;void*_tmp67F;struct Cyc_Absyn_Enumfield*_tmp680;int*_tmp682;struct Cyc_Absyn_Exp**_tmp683;void***_tmp684;struct Cyc_Absyn_Exp**_tmp685;int*_tmp686;struct Cyc_Absyn_Exp*_tmp688;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp68B;struct _dyneither_ptr*_tmp68C;void*_tmp68E;_LL292: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp60D->tag != 11)goto _LL294;else{_tmp60E=_tmp60D->f1;}}_LL293:
# 2804
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp60E);
return;_LL294:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp60F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp60F->tag != 9)goto _LL296;else{_tmp610=_tmp60F->f1;_tmp611=_tmp60F->f2;_tmp612=(struct Cyc_Absyn_VarargCallInfo**)& _tmp60F->f3;_tmp613=(int*)& _tmp60F->f4;}}if(!(!(*_tmp613)))goto _LL296;_LL295:
# 2810
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp610,_tmp611)){
*_tmp613=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2814
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2817
return;_LL296: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp614->tag != 35)goto _LL298;else{_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;}}_LL297:
# 2821
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp616);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL298: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp617->tag != 0)goto _LL29A;else{_tmp618=(union Cyc_Absyn_Cnst*)& _tmp617->f1;}}_LL299:
# 2826
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp618,e);goto _LL291;_LL29A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp619=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp619->tag != 1)goto _LL29C;else{_tmp61A=_tmp619->f1;_tmp61B=(void**)((void**)& _tmp619->f2);}}_LL29B:
# 2828
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp61A,_tmp61B);goto _LL291;_LL29C: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp61C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp61C->tag != 2)goto _LL29E;else{_tmp61D=_tmp61C->f1;_tmp61E=_tmp61C->f2;}}_LL29D:
# 2830
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp61D,_tmp61E);goto _LL291;_LL29E: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp61F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp61F->tag != 4)goto _LL2A0;else{_tmp620=_tmp61F->f1;_tmp621=_tmp61F->f2;}}_LL29F:
# 2832
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp620,_tmp621);goto _LL291;_LL2A0: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp622=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp622->tag != 3)goto _LL2A2;else{_tmp623=_tmp622->f1;_tmp624=_tmp622->f2;_tmp625=_tmp622->f3;}}_LL2A1:
# 2834
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp623,_tmp624,_tmp625);goto _LL291;_LL2A2: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp626=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp626->tag != 5)goto _LL2A4;else{_tmp627=_tmp626->f1;_tmp628=_tmp626->f2;_tmp629=_tmp626->f3;}}_LL2A3:
# 2836
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp627,_tmp628,_tmp629);goto _LL291;_LL2A4: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp62A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp62A->tag != 6)goto _LL2A6;else{_tmp62B=_tmp62A->f1;_tmp62C=_tmp62A->f2;}}_LL2A5:
# 2838
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp62B,_tmp62C);goto _LL291;_LL2A6: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp62D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp62D->tag != 7)goto _LL2A8;else{_tmp62E=_tmp62D->f1;_tmp62F=_tmp62D->f2;}}_LL2A7:
# 2840
 t=Cyc_Tcexp_tcOr(te,loc,_tmp62E,_tmp62F);goto _LL291;_LL2A8: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp630->tag != 8)goto _LL2AA;else{_tmp631=_tmp630->f1;_tmp632=_tmp630->f2;}}_LL2A9:
# 2842
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp631,_tmp632);goto _LL291;_LL2AA: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp633=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp633->tag != 9)goto _LL2AC;else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;_tmp636=(struct Cyc_Absyn_VarargCallInfo**)& _tmp633->f3;}}_LL2AB: {
# 2848
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp690;_push_handler(& _tmp690);{int _tmp692=0;if(setjmp(_tmp690.handler))_tmp692=1;if(!_tmp692){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp691=(void*)_exn_thrown;void*_tmp694=_tmp691;struct _dyneither_ptr _tmp696;void*_tmp697;_LL2E5: {struct Cyc_Core_Failure_exn_struct*_tmp695=(struct Cyc_Core_Failure_exn_struct*)_tmp694;if(_tmp695->tag != Cyc_Core_Failure)goto _LL2E7;else{_tmp696=_tmp695->f1;}}_LL2E6:
# 2855
 ok=0;
fn_exp=e;
goto _LL2E4;_LL2E7: _tmp697=_tmp694;_LL2E8:(void)_rethrow(_tmp697);_LL2E4:;}};}
# 2859
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp634,_tmp635,_tmp636,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2866
goto _LL291;}_LL2AC: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp637=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp637->tag != 10)goto _LL2AE;else{_tmp638=_tmp637->f1;}}_LL2AD:
# 2868
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp638);goto _LL291;_LL2AE: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp639=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp639->tag != 12)goto _LL2B0;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp639->f2;}}_LL2AF:
# 2870
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp63A,_tmp63B);goto _LL291;_LL2B0: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp63C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp63C->tag != 13)goto _LL2B2;else{_tmp63D=(void*)_tmp63C->f1;_tmp63E=_tmp63C->f2;_tmp63F=(enum Cyc_Absyn_Coercion*)& _tmp63C->f4;}}_LL2B1:
# 2872
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp63D,_tmp63E,_tmp63F);goto _LL291;_LL2B2: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp640=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp640->tag != 14)goto _LL2B4;else{_tmp641=_tmp640->f1;}}_LL2B3:
# 2874
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp641);goto _LL291;_LL2B4: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp642->tag != 15)goto _LL2B6;else{_tmp643=(struct Cyc_Absyn_Exp**)& _tmp642->f1;_tmp644=_tmp642->f2;}}_LL2B5:
# 2876
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp643,e,_tmp644);goto _LL291;_LL2B6: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp645=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp645->tag != 17)goto _LL2B8;else{_tmp646=_tmp645->f1;}}_LL2B7: {
# 2878
void*_tmp698=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp646);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp698);goto _LL291;}_LL2B8: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp647=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp647->tag != 16)goto _LL2BA;else{_tmp648=(void*)_tmp647->f1;}}_LL2B9:
# 2881
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp648);goto _LL291;_LL2BA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp649=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp649->tag != 18)goto _LL2BC;else{_tmp64A=(void*)_tmp649->f1;_tmp64B=_tmp649->f2;}}_LL2BB:
# 2883
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp64A,_tmp64B);goto _LL291;_LL2BC: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp64C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp64C->tag != 19)goto _LL2BE;else{_tmp64D=_tmp64C->f1;}}_LL2BD:
# 2885
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp64D);goto _LL291;_LL2BE: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp64E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp64E->tag != 20)goto _LL2C0;else{_tmp64F=_tmp64E->f1;_tmp650=_tmp64E->f2;_tmp651=(int*)& _tmp64E->f3;_tmp652=(int*)& _tmp64E->f4;}}_LL2BF:
# 2887
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp64F,_tmp650,_tmp651,_tmp652);goto _LL291;_LL2C0: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp653=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp653->tag != 21)goto _LL2C2;else{_tmp654=_tmp653->f1;_tmp655=_tmp653->f2;_tmp656=(int*)& _tmp653->f3;_tmp657=(int*)& _tmp653->f4;}}_LL2C1:
# 2889
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp654,_tmp655,_tmp656,_tmp657);goto _LL291;_LL2C2: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp658=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp658->tag != 22)goto _LL2C4;else{_tmp659=_tmp658->f1;_tmp65A=_tmp658->f2;}}_LL2C3:
# 2891
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp659,_tmp65A);goto _LL291;_LL2C4: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp65B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp65B->tag != 23)goto _LL2C6;else{_tmp65C=_tmp65B->f1;}}_LL2C5:
# 2893
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp65C);goto _LL291;_LL2C6: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp65D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp65D->tag != 24)goto _LL2C8;else{_tmp65E=_tmp65D->f1;_tmp65F=_tmp65D->f2;}}_LL2C7:
# 2895
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp65E,_tmp65F);goto _LL291;_LL2C8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp660=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp660->tag != 25)goto _LL2CA;else{_tmp661=_tmp660->f1;}}_LL2C9: {
# 2899
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp699=Cyc_Tcutil_compress(*topt);void*_tmp69A=_tmp699;void**_tmp69C;struct Cyc_Absyn_Tqual*_tmp69D;union Cyc_Absyn_Constraint*_tmp69E;_LL2EA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp69B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp69A;if(_tmp69B->tag != 8)goto _LL2EC;else{_tmp69C=(void**)&(_tmp69B->f1).elt_type;_tmp69D=(struct Cyc_Absyn_Tqual*)&(_tmp69B->f1).tq;_tmp69E=(_tmp69B->f1).zero_term;}}_LL2EB:
# 2905
 elt_topt=_tmp69C;
elt_tqopt=_tmp69D;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp69E);
goto _LL2E9;_LL2EC:;_LL2ED:
 goto _LL2E9;_LL2E9:;}
# 2912
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp661);goto _LL291;}_LL2CA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp662=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp662->tag != 36)goto _LL2CC;else{_tmp663=_tmp662->f1;}}_LL2CB:
# 2914
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp663);goto _LL291;_LL2CC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp664=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp664->tag != 26)goto _LL2CE;else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;_tmp667=_tmp664->f3;_tmp668=(int*)& _tmp664->f4;}}_LL2CD:
# 2916
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp665,_tmp666,_tmp667,_tmp668);goto _LL291;_LL2CE: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp669=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp669->tag != 27)goto _LL2D0;else{_tmp66A=_tmp669->f1;_tmp66B=(void*)_tmp669->f2;_tmp66C=(int*)& _tmp669->f3;}}_LL2CF:
# 2918
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp66A,_tmp66B,_tmp66C);goto _LL291;_LL2D0: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp66D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp66D->tag != 28)goto _LL2D2;else{_tmp66E=(struct _tuple1**)& _tmp66D->f1;_tmp66F=(struct Cyc_List_List**)& _tmp66D->f2;_tmp670=_tmp66D->f3;_tmp671=(struct Cyc_Absyn_Aggrdecl**)& _tmp66D->f4;}}_LL2D1:
# 2920
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp66E,_tmp66F,_tmp670,_tmp671);goto _LL291;_LL2D2: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp672=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp672->tag != 29)goto _LL2D4;else{_tmp673=(void*)_tmp672->f1;_tmp674=_tmp672->f2;}}_LL2D3:
# 2922
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp673,_tmp674);goto _LL291;_LL2D4: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp675=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp675->tag != 30)goto _LL2D6;else{_tmp676=_tmp675->f1;_tmp677=_tmp675->f2;_tmp678=_tmp675->f3;}}_LL2D5:
# 2924
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp676,_tmp677,_tmp678);goto _LL291;_LL2D6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp679=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp679->tag != 31)goto _LL2D8;else{_tmp67A=(struct _tuple1**)& _tmp679->f1;_tmp67B=_tmp679->f2;_tmp67C=_tmp679->f3;}}_LL2D7:
# 2926
*_tmp67A=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp67C))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA1F;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA1E;t=(void*)((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=((_tmpA1F.tag=13,((_tmpA1F.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp67B))->name,((_tmpA1F.f2=_tmp67B,_tmpA1F)))))),_tmpA1E))));}goto _LL291;_LL2D8: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp67D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp67D->tag != 32)goto _LL2DA;else{_tmp67E=(struct _tuple1**)& _tmp67D->f1;_tmp67F=(void*)_tmp67D->f2;_tmp680=_tmp67D->f3;}}_LL2D9:
# 2929
*_tmp67E=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp680))->name;
t=_tmp67F;goto _LL291;_LL2DA: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp681=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp681->tag != 33)goto _LL2DC;else{_tmp682=(int*)&(_tmp681->f1).is_calloc;_tmp683=(struct Cyc_Absyn_Exp**)&(_tmp681->f1).rgn;_tmp684=(void***)&(_tmp681->f1).elt_type;_tmp685=(struct Cyc_Absyn_Exp**)&(_tmp681->f1).num_elts;_tmp686=(int*)&(_tmp681->f1).fat_result;}}_LL2DB:
# 2932
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp683,_tmp684,_tmp685,_tmp682,_tmp686);goto _LL291;_LL2DC: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp687=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp687->tag != 34)goto _LL2DE;else{_tmp688=_tmp687->f1;_tmp689=_tmp687->f2;}}_LL2DD:
# 2934
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp688,_tmp689);goto _LL291;_LL2DE: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp68A=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp68A->tag != 37)goto _LL2E0;else{_tmp68B=_tmp68A->f1;_tmp68C=_tmp68A->f2;}}_LL2DF:
# 2936
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp68B,_tmp68C);goto _LL291;_LL2E0: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp68D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp68D->tag != 38)goto _LL2E2;else{_tmp68E=(void*)_tmp68D->f1;}}_LL2E1:
# 2938
 if(!te->allow_valueof){
const char*_tmpA22;void*_tmpA21;(_tmpA21=0,Cyc_Tcutil_terr(e->loc,((_tmpA22="valueof(-) can only occur within types",_tag_dyneither(_tmpA22,sizeof(char),39))),_tag_dyneither(_tmpA21,sizeof(void*),0)));}
# 2946
t=Cyc_Absyn_sint_typ;
goto _LL291;_LL2E2: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp68F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp60C;if(_tmp68F->tag != 39)goto _LL291;}_LL2E3:
# 2949
{const char*_tmpA25;void*_tmpA24;(_tmpA24=0,Cyc_Tcutil_terr(e->loc,((_tmpA25="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA25,sizeof(char),50))),_tag_dyneither(_tmpA24,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL291:;}
# 2952
e->topt=t;}
