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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 379
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 110
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 339
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 357
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 370
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 451 "absyn.h"
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 453
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 750 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 944
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 952
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 958
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 964
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 966
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 972
void*Cyc_Absyn_compress_kb(void*);
# 977
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 979
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 982
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 984
extern void*Cyc_Absyn_sint_typ;
# 986
void*Cyc_Absyn_float_typ(int);
# 988
extern void*Cyc_Absyn_empty_effect;
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1088
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1092
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1095
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1110
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1133
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1142
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1202
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 75
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct Cyc_RgnOrder_RgnPO;
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
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 139
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 156
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 213
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 215
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 69
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 80
int Cyc_Tcutil_coerceable(void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 130
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 193
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 238 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 241
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 58
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 60
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
# 75
typedef void*Cyc_Tcpat_pat_test_t;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};
# 83
typedef void*Cyc_Tcpat_access_t;struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};
# 93
typedef struct Cyc_Tcpat_PathNode*Cyc_Tcpat_path_node_t;
# 95
typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_path_t;struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};
# 103
typedef struct Cyc_Tcpat_Rhs*Cyc_Tcpat_rhs_t;
# 105
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 112
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 114
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 116
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 118
void Cyc_Tcpat_print_decision_tree(void*);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2;enum Cyc_Absyn_AggrKind _tmp3;struct _tuple2*_tmp4;struct Cyc_List_List*_tmp5;struct Cyc_List_List**_tmp6;struct Cyc_List_List*_tmp7;int _tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;int _tmpB;struct _tuple2*_tmpC;struct Cyc_List_List*_tmpD;int _tmpE;struct _tuple2*_tmpF;switch(*((int*)_tmp1)){case 15: _LL1: _tmpF=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL2:
# 55
{struct _handler_cons _tmp10;_push_handler(& _tmp10);{int _tmp12=0;if(setjmp(_tmp10.handler))_tmp12=1;if(!_tmp12){
{struct _RegionHandle _tmp13=_new_region("r");struct _RegionHandle*r=& _tmp13;_push_region(r);
{void*_tmp14=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmpF,0);void*_tmp15=_tmp14;void*_tmp16;struct Cyc_Absyn_Enumfield*_tmp17;struct Cyc_Absyn_Enumdecl*_tmp18;struct Cyc_Absyn_Enumfield*_tmp19;struct Cyc_Absyn_Datatypedecl*_tmp1A;struct Cyc_Absyn_Datatypefield*_tmp1B;switch(*((int*)_tmp15)){case 1: _LLE: _LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
({void*_tmp1C=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp1D="struct tag used without arguments in pattern";_tag_dyneither(_tmp1D,sizeof(char),45);}),_tag_dyneither(_tmp1C,sizeof(void*),0));});
_npop_handler(1);return;case 2: _LL10: _tmp1A=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp15)->f1;_tmp1B=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp15)->f2;_LL11:
# 63
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp1F;_tmp1F.tag=8;_tmp1F.f1=_tmp1A;_tmp1F.f2=_tmp1B;_tmp1F.f3=0;_tmp1F.f4=0;_tmp1F;});_tmp1E;});_npop_handler(1);return;case 3: _LL12: _tmp18=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp15)->f1;_tmp19=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp15)->f2;_LL13:
# 65
 p->r=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp21;_tmp21.tag=13;_tmp21.f1=_tmp18;_tmp21.f2=_tmp19;_tmp21;});_tmp20;});
_npop_handler(1);return;case 4: _LL14: _tmp16=(void*)((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp15)->f2;_LL15:
# 68
 p->r=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp23;_tmp23.tag=14;_tmp23.f1=_tmp16;_tmp23.f2=_tmp17;_tmp23;});_tmp22;});
_npop_handler(1);return;default: _LL16: _LL17:
 goto _LLD;}_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp11=(void*)_exn_thrown;void*_tmp24=_tmp11;void*_tmp25;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp24)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
# 73
 goto _LL18;}else{_LL1B: _tmp25=_tmp24;_LL1C:(int)_rethrow(_tmp25);}_LL18:;}};}{
# 75
union Cyc_Absyn_Nmspace _tmp26=(*_tmpF).f1;union Cyc_Absyn_Nmspace _tmp27=_tmp26;switch((_tmp27.Rel_n).tag){case 4: _LL1E: _LL1F:
 goto _LL21;case 1: if((_tmp27.Rel_n).val == 0){_LL20: _LL21:
# 78
(*_tmpF).f1=Cyc_Absyn_Loc_n;
p->r=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp29;_tmp29.tag=1;_tmp29.f1=Cyc_Absyn_new_vardecl(_tmpF,(void*)& Cyc_Absyn_VoidType_val,0);_tmp29.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp29;});_tmp28;});
return;}else{goto _LL22;}default: _LL22: _LL23:
({void*_tmp2A=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp2B="qualified variable in pattern";_tag_dyneither(_tmp2B,sizeof(char),30);}),_tag_dyneither(_tmp2A,sizeof(void*),0));});return;}_LL1D:;};case 16: _LL3: _tmpC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_tmpD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmpE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_LL4:
# 85
{struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct _RegionHandle _tmp2F=_new_region("r");struct _RegionHandle*r=& _tmp2F;_push_region(r);
{void*_tmp30=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmpC,0);void*_tmp31=_tmp30;struct Cyc_Absyn_Datatypedecl*_tmp32;struct Cyc_Absyn_Datatypefield*_tmp33;struct Cyc_Absyn_Aggrdecl*_tmp34;switch(*((int*)_tmp31)){case 1: _LL25: _tmp34=((struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp31)->f1;_LL26: {
# 89
struct Cyc_List_List*_tmp35=0;
for(0;_tmpD != 0;_tmpD=_tmpD->tl){
_tmp35=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->hd=({struct _tuple15*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=0;_tmp37->f2=(struct Cyc_Absyn_Pat*)_tmpD->hd;_tmp37;});_tmp36->tl=_tmp35;_tmp36;});}
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp39;_tmp39.tag=7;_tmp39.f1=({struct Cyc_Absyn_AggrInfo*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=_tmp34;_tmp3B;}));_tmp3A->targs=0;_tmp3A;});_tmp39.f2=0;_tmp39.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp35);_tmp39.f4=_tmpE;_tmp39;});_tmp38;});
_npop_handler(1);return;}case 2: _LL27: _tmp32=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp31)->f1;_tmp33=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp31)->f2;_LL28:
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3D;_tmp3D.tag=8;_tmp3D.f1=_tmp32;_tmp3D.f2=_tmp33;_tmp3D.f3=_tmpD;_tmp3D.f4=_tmpE;_tmp3D;});_tmp3C;});_npop_handler(1);return;case 3: _LL29: _LL2A:
 goto _LL2C;case 4: _LL2B: _LL2C:
# 98
({void*_tmp3E=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp3F="enum tag used with arguments in pattern";_tag_dyneither(_tmp3F,sizeof(char),40);}),_tag_dyneither(_tmp3E,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;default: _LL2D: _LL2E:
 goto _LL24;}_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp40=_tmp2D;void*_tmp41;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp40)->tag == Cyc_Dict_Absent){_LL30: _LL31:
# 103
 goto _LL2F;}else{_LL32: _tmp41=_tmp40;_LL33:(int)_rethrow(_tmp41);}_LL2F:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC));({void*_tmp42[1]={& _tmp44};Cyc_Tcutil_terr(p->loc,({const char*_tmp43="%s is not a constructor in pattern";_tag_dyneither(_tmp43,sizeof(char),35);}),_tag_dyneither(_tmp42,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL5: _tmp9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmpA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmpB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL6:
# 109
 if(topt == 0)
({void*_tmp45=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp46="cannot determine pattern type";_tag_dyneither(_tmp46,sizeof(char),30);}),_tag_dyneither(_tmp45,sizeof(void*),0));});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp47=t;struct Cyc_Absyn_AggrInfo _tmp48;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->tag == 11){_LL35: _tmp48=((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1;_LL36:
# 114
 p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp4A;_tmp4A.tag=7;_tmp4A.f1=({struct Cyc_Absyn_AggrInfo*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=_tmp48;_tmp4B;});_tmp4A.f2=_tmp9;_tmp4A.f3=_tmpA;_tmp4A.f4=_tmpB;_tmp4A;});_tmp49;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;}else{_LL37: _LL38:
# 118
({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmp4C[1]={& _tmp4E};Cyc_Tcutil_terr(p->loc,({const char*_tmp4D="pattern expects aggregate type instead of %s";_tag_dyneither(_tmp4D,sizeof(char),45);}),_tag_dyneither(_tmp4C,sizeof(void*),1));});});
# 121
goto _LL34;}_LL34:;}
# 123
return;};}else{if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).tag == 1){_LL7: _tmp3=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).val).f1;_tmp4=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).val).f2;_tmp5=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->targs;_tmp6=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL8:
# 126
{struct _handler_cons _tmp4F;_push_handler(& _tmp4F);{int _tmp51=0;if(setjmp(_tmp4F.handler))_tmp51=1;if(!_tmp51){
{struct Cyc_Absyn_Aggrdecl**_tmp52=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp4);
struct Cyc_Absyn_Aggrdecl*_tmp53=*_tmp52;
if(_tmp53->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dyneither_ptr)(
_tmp53->kind == Cyc_Absyn_StructA?({const char*_tmp57="struct";_tag_dyneither(_tmp57,sizeof(char),7);}):({const char*_tmp58="union";_tag_dyneither(_tmp58,sizeof(char),6);}));({void*_tmp54[1]={& _tmp56};Cyc_Tcutil_terr(p->loc,({const char*_tmp55="can't destructure an abstract %s";_tag_dyneither(_tmp55,sizeof(char),33);}),_tag_dyneither(_tmp54,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp6);
if(more_exists < 0){
({void*_tmp59=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5A="too many existentially bound type variables in pattern";_tag_dyneither(_tmp5A,sizeof(char),55);}),_tag_dyneither(_tmp59,sizeof(void*),0));});{
struct Cyc_List_List**_tmp5B=_tmp6;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53->impl))->exist_vars);for(0;n != 0;-- n){
_tmp5B=&((struct Cyc_List_List*)_check_null(*_tmp5B))->tl;}}
*_tmp5B=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp5C=0;
for(0;more_exists != 0;-- more_exists){
_tmp5C=({struct Cyc_List_List*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->hd=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp5F;_tmp5F.tag=1;_tmp5F.f1=0;_tmp5F;});_tmp5E;}));_tmp5D->tl=_tmp5C;_tmp5D;});}
*_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp6,_tmp5C);}}
# 149
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp61;_tmp61.tag=7;_tmp61.f1=({struct Cyc_Absyn_AggrInfo*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->aggr_info=Cyc_Absyn_KnownAggr(_tmp52);_tmp62->targs=_tmp5;_tmp62;});_tmp61.f2=*_tmp6;_tmp61.f3=_tmp7;_tmp61.f4=_tmp8;_tmp61;});_tmp60;});};}
# 127
;_pop_handler();}else{void*_tmp50=(void*)_exn_thrown;void*_tmp63=_tmp50;void*_tmp64;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp63)->tag == Cyc_Dict_Absent){_LL3A: _LL3B:
# 152
({void*_tmp65=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp66="Non-aggregate name has designator patterns";_tag_dyneither(_tmp66,sizeof(char),43);}),_tag_dyneither(_tmp65,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;}else{_LL3C: _tmp64=_tmp63;_LL3D:(int)_rethrow(_tmp64);}_LL39:;}};}
# 156
return;}else{goto _LLB;}}case 17: _LL9: _tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp2);
if(!Cyc_Tcutil_is_const_exp(_tmp2)){
({void*_tmp67=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp68="non-constant expression in case pattern";_tag_dyneither(_tmp68,sizeof(char),40);}),_tag_dyneither(_tmp67,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _tmp69=Cyc_Evexp_eval_const_uint_exp(_tmp2);struct _tuple14 _tmp6A=_tmp69;unsigned int _tmp6B;int _tmp6C;_LL3F: _tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;_LL40:;
p->r=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp6D=_cycalloc_atomic(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp6E;_tmp6E.tag=10;_tmp6E.f1=Cyc_Absyn_None;_tmp6E.f2=(int)_tmp6B;_tmp6E;});_tmp6D;});
return;};default: _LLB: _LLC:
 return;}_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->v=s;_tmp6F;}));}
# 178
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 182
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 185
{void*_tmp70=Cyc_Tcutil_compress(numt);void*_tmp71=_tmp70;switch(*((int*)_tmp71)){case 13: _LL42: _LL43:
 goto _LL45;case 14: _LL44: _LL45:
 if(topt != 0)return*topt;goto _LL41;default: _LL46: _LL47:
 goto _LL41;}_LL41:;}
# 190
return numt;}struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 193
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 195
if(vd != 0){
(*vd)->type=t;
(*vd)->tq=Cyc_Absyn_empty_tqual(0);}
# 199
*v_result_ptr=({struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->hd=({struct _tuple16*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->f1=vd;_tmp73->f2=e;_tmp73;});_tmp72->tl=*v_result_ptr;_tmp72;});}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct Cyc_Tcpat_TcPatResult _tmp74=res1;struct _tuple1*_tmp75;struct Cyc_List_List*_tmp76;_LL49: _tmp75=_tmp74.tvars_and_bounds_opt;_tmp76=_tmp74.patvars;_LL4A:;{
struct Cyc_Tcpat_TcPatResult _tmp77=res2;struct _tuple1*_tmp78;struct Cyc_List_List*_tmp79;_LL4C: _tmp78=_tmp77.tvars_and_bounds_opt;_tmp79=_tmp77.patvars;_LL4D:;
if(_tmp75 != 0  || _tmp78 != 0){
if(_tmp75 == 0)
_tmp75=({struct _tuple1*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=0;_tmp7A->f2=0;_tmp7A;});
if(_tmp78 == 0)
_tmp78=({struct _tuple1*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->f1=0;_tmp7B->f2=0;_tmp7B;});
return({struct Cyc_Tcpat_TcPatResult _tmp7C;_tmp7C.tvars_and_bounds_opt=({struct _tuple1*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp75).f1,(*_tmp78).f1);_tmp7D->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp75).f2,(*_tmp78).f2);_tmp7D;});_tmp7C.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp76,_tmp79);_tmp7C;});}
# 214
return({struct Cyc_Tcpat_TcPatResult _tmp7E;_tmp7E.tvars_and_bounds_opt=0;_tmp7E.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp76,_tmp79);_tmp7E;});};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
return({struct Cyc_Absyn_Pat*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->loc=loc;_tmp7F->topt=0;_tmp7F->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp7F;});}
# 222
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->v=Cyc_Tcenv_lookup_type_vars(te);_tmp80;})): rgn_opt,0): t;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};
# 229
static struct _tuple17*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple17*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=tv;_tmp81->f2=0;_tmp81;});}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 233
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 236
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp159;_tmp159.tvars_and_bounds_opt=0;_tmp159.patvars=0;_tmp159;});
# 241
{void*_tmp82=p->r;void*_tmp83=_tmp82;struct Cyc_Absyn_Datatypedecl*_tmp84;struct Cyc_Absyn_Datatypefield*_tmp85;struct Cyc_List_List**_tmp86;int _tmp87;struct Cyc_Absyn_Aggrdecl*_tmp88;struct Cyc_List_List**_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List**_tmp8B;int _tmp8C;struct Cyc_List_List**_tmp8D;int _tmp8E;struct Cyc_Absyn_Pat*_tmp8F;void*_tmp90;struct Cyc_Absyn_Enumdecl*_tmp91;int _tmp92;struct Cyc_Absyn_Tvar*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*_tmp95;struct Cyc_Absyn_Pat*_tmp96;struct Cyc_Absyn_Tvar*_tmp97;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Vardecl*_tmp99;struct Cyc_Absyn_Pat*_tmp9A;switch(*((int*)_tmp83)){case 0: _LL4F: _LL50:
# 244
 if(topt != 0)
t=*topt;else{
# 247
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL4E;case 1: _LL51: _tmp99=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp9A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL52: {
# 251
struct _tuple2*_tmp9B=_tmp99->name;struct _tuple2*_tmp9C=_tmp9B;struct _dyneither_ptr _tmp9D;_LL7C: _tmp9D=*_tmp9C->f2;_LL7D:;
if(Cyc_strcmp((struct _dyneither_ptr)_tmp9D,({const char*_tmp9E="true";_tag_dyneither(_tmp9E,sizeof(char),5);}))== 0  || Cyc_strcmp((struct _dyneither_ptr)_tmp9D,({const char*_tmp9F="false";_tag_dyneither(_tmp9F,sizeof(char),6);}))== 0)
({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9D);({void*_tmpA0[1]={& _tmpA2};Cyc_Tcutil_warn(p->loc,({const char*_tmpA1="you probably do not want to use %s as a local variable...";_tag_dyneither(_tmpA1,sizeof(char),58);}),_tag_dyneither(_tmpA0,sizeof(void*),1));});});
# 255
res=Cyc_Tcpat_tcPatRec(te,_tmp9A,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp9A->topt);
# 259
{void*_tmpA3=Cyc_Tcutil_compress(t);void*_tmpA4=_tmpA3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA4)->tag == 8){_LL7F: _LL80:
# 261
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmpA5=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpA6="array reference would point into unknown/unallowed region";_tag_dyneither(_tmpA6,sizeof(char),58);}),_tag_dyneither(_tmpA5,sizeof(void*),0));});
goto _LL7E;}else{_LL81: _LL82:
# 265
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmpA7=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpA8="pattern would point to an abstract member";_tag_dyneither(_tmpA8,sizeof(char),42);}),_tag_dyneither(_tmpA7,sizeof(void*),0));});
goto _LL7E;}_LL7E:;}
# 269
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=_tmp99;_tmpA9;}),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));
goto _LL4E;}case 2: _LL53: _tmp97=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp98=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL54: {
# 272
struct _RegionHandle _tmpAA=_new_region("r2");struct _RegionHandle*r2=& _tmpAA;_push_region(r2);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,({struct Cyc_Absyn_Tvar*_tmpAE[1];_tmpAE[0]=_tmp97;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r2,_tag_dyneither(_tmpAE,sizeof(struct Cyc_Absyn_Tvar*),1));}));
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=0;_tmpAB->f2=0;_tmpAB;});
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=({struct _tuple17*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->f1=_tmp97;_tmpAD->f2=1;_tmpAD;});_tmpAC->tl=0;_tmpAC;}));
Cyc_Tcutil_check_type(p->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,0,_tmp98->type);}
# 282
if(topt != 0)t=*topt;else{
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
{void*_tmpAF=Cyc_Tcutil_compress(t);void*_tmpB0=_tmpAF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0)->tag == 8){_LL84: _LL85:
# 286
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmpB1=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpB2="array reference would point into unknown/unallowed region";_tag_dyneither(_tmpB2,sizeof(char),58);}),_tag_dyneither(_tmpB1,sizeof(void*),0));});
goto _LL83;}else{_LL86: _LL87:
# 290
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmpB3=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpB4="pattern would point to an abstract member";_tag_dyneither(_tmpB4,sizeof(char),42);}),_tag_dyneither(_tmpB3,sizeof(void*),0));});
goto _LL83;}_LL83:;}
# 294
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=_tmp98;_tmpB5;}),access_exp,& res.patvars,_tmp98->type);
_npop_handler(0);goto _LL4E;
# 272
;_pop_region(r2);}case 3: _LL55: _tmp95=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp96=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL56:
# 298
 res=Cyc_Tcpat_tcPatRec(te,_tmp96,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp96->topt);
if(!allow_ref_pat  || rgn_pat == 0){
({void*_tmpB6=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpB7="* pattern would point into an unknown/unallowed region";_tag_dyneither(_tmpB7,sizeof(char),55);}),_tag_dyneither(_tmpB6,sizeof(void*),0));});
goto _LL4E;}else{
# 305
struct _RegionHandle _tmpB8=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB8;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))
({void*_tmpB9=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpBA="* pattern cannot take the address of an alias-free path";_tag_dyneither(_tmpBA,sizeof(char),56);}),_tag_dyneither(_tmpB9,sizeof(void*),0));});
# 306
;_pop_region(rgn);}{
# 311
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpBD;_tmpBD.tag=5;_tmpBD.f1=({struct Cyc_Absyn_PtrInfo _tmpBE;_tmpBE.elt_typ=t;_tmpBE.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpBE.ptr_atts=({(_tmpBE.ptr_atts).rgn=rgn_pat;(_tmpBE.ptr_atts).nullable=Cyc_Absyn_false_conref;(_tmpBE.ptr_atts).bounds=
# 314
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpBE.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpBE.ptr_atts).ptrloc=0;_tmpBE.ptr_atts;});_tmpBE;});_tmpBD;});_tmpBC;});
# 316
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 320
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=_tmp95;_tmpBB;}),new_access_exp,& res.patvars,t2);
goto _LL4E;};case 4: _LL57: _tmp93=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp94=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL58:
# 326
 Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=_tmp94;_tmpBF;}),access_exp,& res.patvars,_tmp94->type);{
# 330
struct _RegionHandle _tmpC0=_new_region("r2");struct _RegionHandle*r2=& _tmpC0;_push_region(r2);
Cyc_Tcenv_add_type_vars(r2,p->loc,te,({struct Cyc_Absyn_Tvar*_tmpC1[1];_tmpC1[0]=_tmp93;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1,sizeof(struct Cyc_Absyn_Tvar*),1));}));
# 333
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=0;_tmpC2->f2=0;_tmpC2;});
(*res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->hd=({struct _tuple17*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->f1=_tmp93;_tmpC4->f2=0;_tmpC4;});_tmpC3->tl=(*res.tvars_and_bounds_opt).f1;_tmpC3;});
# 337
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL4E;
# 330
;_pop_region(r2);};case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp83)->f1){case Cyc_Absyn_Unsigned: _LL59: _LL5A:
# 340
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL4E;case Cyc_Absyn_None: _LL5B: _LL5C:
 goto _LL5E;default: _LL5D: _LL5E:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL4E;}case 11: _LL5F: _LL60:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL4E;case 12: _LL61: _tmp92=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL62:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmp92));goto _LL4E;case 13: _LL63: _tmp91=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL64:
# 346
 t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpC6;_tmpC6.tag=13;_tmpC6.f1=_tmp91->name;_tmpC6.f2=_tmp91;_tmpC6;});_tmpC5;}));
goto _LL4E;case 14: _LL65: _tmp90=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL66:
 t=Cyc_Tcpat_num_type(topt,_tmp90);goto _LL4E;case 9: _LL67: _LL68:
# 350
 if(topt != 0){
void*_tmpC7=Cyc_Tcutil_compress(*topt);void*_tmpC8=_tmpC7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8)->tag == 5){_LL89: _LL8A:
 t=*topt;goto tcpat_end;}else{_LL8B: _LL8C:
 goto _LL88;}_LL88:;}{
# 355
struct Cyc_Core_Opt*_tmpC9=Cyc_Tcenv_lookup_opt_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpCB;_tmpCB.tag=5;_tmpCB.f1=({struct Cyc_Absyn_PtrInfo _tmpCC;_tmpCC.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpC9);_tmpCC.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmpCC.ptr_atts=({(_tmpCC.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpC9);(_tmpCC.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpCC.ptr_atts).bounds=
# 360
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpCC.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpCC.ptr_atts).ptrloc=0;_tmpCC.ptr_atts;});_tmpCC;});_tmpCB;});_tmpCA;});
# 363
goto _LL4E;};case 6: _LL69: _tmp8F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL6A: {
# 368
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpCD=0;
int elt_const=0;
if(topt != 0){
void*_tmpCE=Cyc_Tcutil_compress(*topt);void*_tmpCF=_tmpCE;void*_tmpD0;struct Cyc_Absyn_Tqual _tmpD1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCF)->tag == 5){_LL8E: _tmpD0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCF)->f1).elt_typ;_tmpD1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCF)->f1).elt_tq;_LL8F:
# 374
 inner_typ=_tmpD0;
_tmpCD=& inner_typ;
elt_const=_tmpD1.real_const;
goto _LL8D;}else{_LL90: _LL91:
 goto _LL8D;}_LL8D:;}{
# 383
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp8F,_tmpCD,ptr_rgn,1,new_access_exp));
# 392
{void*_tmpD2=Cyc_Tcutil_compress((void*)_check_null(_tmp8F->topt));void*_tmpD3=_tmpD2;struct Cyc_Absyn_Datatypedecl*_tmpD4;struct Cyc_Absyn_Datatypefield*_tmpD5;struct Cyc_List_List*_tmpD6;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->tag == 4){if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).field_info).KnownDatatypefield).tag == 2){_LL93: _tmpD4=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).field_info).KnownDatatypefield).val).f1;_tmpD5=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).field_info).KnownDatatypefield).val).f2;_tmpD6=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).targs;_LL94:
# 396
{void*_tmpD7=Cyc_Tcutil_compress(inner_typ);void*_tmpD8=_tmpD7;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD8)->tag == 4){_LL98: _LL99:
# 398
 goto DONT_PROMOTE;}else{_LL9A: _LL9B:
 goto _LL97;}_LL97:;}{
# 402
void*new_type=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDD;_tmpDD.tag=3;_tmpDD.f1=({struct Cyc_Absyn_DatatypeInfo _tmpDE;_tmpDE.datatype_info=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=_tmpD4;_tmpDF;}));_tmpDE.targs=_tmpD6;_tmpDE;});_tmpDD;});_tmpDC;});
# 405
_tmp8F->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDA;_tmpDA.tag=5;_tmpDA.f1=({struct Cyc_Absyn_PtrInfo _tmpDB;_tmpDB.elt_typ=new_type;_tmpDB.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmpDB.ptr_atts=({(_tmpDB.ptr_atts).rgn=ptr_rgn;(_tmpDB.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpDB.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmpDB.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmpDB.ptr_atts).ptrloc=0;_tmpDB.ptr_atts;});_tmpDB;});_tmpDA;});_tmpD9;});
# 412
goto _LL92;};}else{goto _LL95;}}else{_LL95: _LL96:
# 414
 DONT_PROMOTE:
 t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpE1;_tmpE1.tag=5;_tmpE1.f1=({struct Cyc_Absyn_PtrInfo _tmpE2;_tmpE2.elt_typ=(void*)_check_null(_tmp8F->topt);_tmpE2.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmpE2.ptr_atts=({(_tmpE2.ptr_atts).rgn=ptr_rgn;(_tmpE2.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpE2.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpE2.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpE2.ptr_atts).ptrloc=0;_tmpE2.ptr_atts;});_tmpE2;});_tmpE1;});_tmpE0;});}_LL92:;}
# 422
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp8F->topt;
goto _LL4E;};}case 5: _LL6B: _tmp8D=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp8E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL6C: {
# 426
struct Cyc_List_List*_tmpE3=*_tmp8D;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpE4=Cyc_Tcutil_compress(*topt);void*_tmpE5=_tmpE4;struct Cyc_List_List*_tmpE6;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE5)->tag == 10){_LL9D: _tmpE6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE5)->f1;_LL9E:
# 432
 topt_ts=_tmpE6;
if(_tmp8E){
# 435
int _tmpE7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE3);
int _tmpE8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE6);
if(_tmpE7 < _tmpE8){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpE8 - _tmpE7;++ i){
wild_ps=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpE9->tl=wild_ps;_tmpE9;});}}
*_tmp8D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE3,wild_ps);
_tmpE3=*_tmp8D;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE3)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE6))
({void*_tmpEA=0;Cyc_Tcutil_warn(p->loc,({const char*_tmpEB="unnecessary ... in tuple pattern";_tag_dyneither(_tmpEB,sizeof(char),33);}),_tag_dyneither(_tmpEA,sizeof(void*),0));});}}
# 447
goto _LL9C;}else{_LL9F: _LLA0:
# 449
 goto _LL9C;}_LL9C:;}else{
# 451
if(_tmp8E)
({void*_tmpEC=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpED="cannot determine missing fields for ... in tuple pattern";_tag_dyneither(_tmpED,sizeof(char),57);}),_tag_dyneither(_tmpEC,sizeof(void*),0));});}
{int i=0;for(0;_tmpE3 != 0;(_tmpE3=_tmpE3->tl,i ++)){
void**_tmpEE=0;
if(topt_ts != 0){
_tmpEE=&(*((struct _tuple18*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 459
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmpEF;(_tmpEF.Int_c).val=({struct _tuple7 _tmpF0;_tmpF0.f1=Cyc_Absyn_Unsigned;_tmpF0.f2=i;_tmpF0;});(_tmpEF.Int_c).tag=5;_tmpEF;}),0),0);
# 464
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpE3->hd,_tmpEE,rgn_pat,allow_ref_pat,new_access_exp));
# 467
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpE3->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->hd=({struct _tuple18*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=Cyc_Absyn_empty_tqual(0);_tmpF2->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpE3->hd)->topt);_tmpF2;});_tmpF1->tl=pat_ts;_tmpF1;});};}}
# 470
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpF4;_tmpF4.tag=10;_tmpF4.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpF4;});_tmpF3;});
goto _LL4E;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->aggr_info).KnownAggr).tag == 2){_LL6D: _tmp88=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->aggr_info).KnownAggr).val;_tmp89=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->targs;_tmp8A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_tmp8B=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f3;_tmp8C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f4;_LL6E: {
# 474
struct Cyc_List_List*_tmpF5=*_tmp8B;
struct _dyneither_ptr aggr_str=_tmp88->kind == Cyc_Absyn_StructA?({const char*_tmp13A="struct";_tag_dyneither(_tmp13A,sizeof(char),7);}):({const char*_tmp13B="union";_tag_dyneither(_tmp13B,sizeof(char),6);});
if(_tmp88->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmpF6[1]={& _tmpF8};Cyc_Tcutil_terr(p->loc,({const char*_tmpF7="can't destructure an abstract %s";_tag_dyneither(_tmpF7,sizeof(char),33);}),_tag_dyneither(_tmpF6,sizeof(void*),1));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL4E;}
# 483
if(_tmp88->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->tagged)
allow_ref_pat=0;
if(_tmp8A != 0){
# 489
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 492
struct _RegionHandle _tmpF9=_new_region("rgn");struct _RegionHandle*rgn=& _tmpF9;_push_region(rgn);{
# 494
struct Cyc_List_List*_tmpFA=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpFB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->exist_vars;
{struct Cyc_List_List*t=_tmp8A;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpFB))->hd;
_tmpFB=_tmpFB->tl;{
void*_tmpFC=Cyc_Absyn_compress_kb(tv->kind);
void*_tmpFD=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmpFE=_tmpFD;struct Cyc_Absyn_Kind*_tmpFF;struct Cyc_Absyn_Kind*_tmp100;switch(*((int*)_tmpFE)){case 2: _LLA2: _tmp100=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFE)->f2;_LLA3:
 _tmpFF=_tmp100;goto _LLA5;case 0: _LLA4: _tmpFF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFE)->f1;_LLA5:
 k2=_tmpFF;goto _LLA1;default: _LLA6: _LLA7:
({void*_tmp101=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp102="unconstrained existential type variable in aggregate";_tag_dyneither(_tmp102,sizeof(char),53);}),_tag_dyneither(_tmp101,sizeof(void*),0));});}_LLA1:;}
# 510
{void*_tmp103=_tmpFC;struct Cyc_Core_Opt**_tmp104;struct Cyc_Core_Opt**_tmp105;struct Cyc_Absyn_Kind*_tmp106;struct Cyc_Absyn_Kind*_tmp107;switch(*((int*)_tmp103)){case 0: _LLA9: _tmp107=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp103)->f1;_LLAA:
# 513
 if(!Cyc_Tcutil_kind_leq(k2,_tmp107))
error=1;
goto _LLA8;case 2: _LLAB: _tmp105=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp103)->f1;_tmp106=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp103)->f2;_LLAC:
 _tmp104=_tmp105;goto _LLAE;default: _LLAD: _tmp104=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp103)->f1;_LLAE:
*_tmp104=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->v=_tmpFD;_tmp108;});goto _LLA8;}_LLA8:;}
# 519
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 523
Cyc_Absynpp_kind2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpFC));({struct Cyc_String_pa_PrintArg_struct _tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp109[3]={& _tmp10B,& _tmp10C,& _tmp10D};Cyc_Tcutil_terr(p->loc,({const char*_tmp10A="type variable %s has kind %s but must have at least kind %s";_tag_dyneither(_tmp10A,sizeof(char),60);}),_tag_dyneither(_tmp109,sizeof(void*),3));});});});});{
# 525
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp114;_tmp114.tag=2;_tmp114.f1=tv;_tmp114;});_tmp113;});
_tmpFA=({struct Cyc_List_List*_tmp10E=_region_malloc(rgn,sizeof(*_tmp10E));_tmp10E->hd=vartype;_tmp10E->tl=_tmpFA;_tmp10E;});
# 529
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable)
outlives_constraints=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=({struct _tuple0*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->f1=Cyc_Absyn_empty_effect;_tmp110->f2=vartype;_tmp110;});_tmp10F->tl=outlives_constraints;_tmp10F;});else{
# 534
({void*_tmp111=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp112="opened existential had unique or top region kind";_tag_dyneither(_tmp112,sizeof(char),49);}),_tag_dyneither(_tmp111,sizeof(void*),0));});}}};};}}
# 538
_tmpFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpFA);{
# 540
struct _RegionHandle _tmp115=_new_region("r2");struct _RegionHandle*r2=& _tmp115;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmp8A);
# 543
struct Cyc_List_List*_tmp116=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp117=({struct _tuple12 _tmp139;_tmp139.f1=_tmp116;_tmp139.f2=rgn;_tmp139;});
struct Cyc_List_List*_tmp118=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp117,_tmp88->tvs);
struct Cyc_List_List*_tmp119=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->exist_vars,_tmpFA);
struct Cyc_List_List*_tmp11A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp118);
struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp119);
struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp118,_tmp119);
# 551
if(_tmp8A != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->f1=0;_tmp11D->f2=0;_tmp11D;});
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmp8A));
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp11E=0;
{struct Cyc_List_List*_tmp11F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->rgn_po;for(0;_tmp11F != 0;_tmp11F=_tmp11F->tl){
_tmp11E=({struct Cyc_List_List*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->hd=({struct _tuple0*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp11C,(*((struct _tuple0*)_tmp11F->hd)).f1);_tmp121->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp11C,(*((struct _tuple0*)_tmp11F->hd)).f2);_tmp121;});_tmp120->tl=_tmp11E;_tmp120;});}}
# 563
_tmp11E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11E);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp11E);};}
# 568
*_tmp89=_tmp11A;
t=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp123;_tmp123.tag=11;_tmp123.f1=({struct Cyc_Absyn_AggrInfo _tmp124;_tmp124.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=_tmp88;_tmp125;}));_tmp124.targs=*_tmp89;_tmp124;});_tmp123;});_tmp122;});
if(_tmp8C  && _tmp88->kind == Cyc_Absyn_UnionA)
({void*_tmp126=0;Cyc_Tcutil_warn(p->loc,({const char*_tmp127="`...' pattern not allowed in union pattern";_tag_dyneither(_tmp127,sizeof(char),43);}),_tag_dyneither(_tmp126,sizeof(void*),0));});else{
if(_tmp8C){
# 574
int _tmp128=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF5);
int _tmp129=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->fields);
if(_tmp128 < _tmp129){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp129 - _tmp128;++ i){
wild_dps=({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->hd=({struct _tuple15*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=0;_tmp12B->f2=Cyc_Tcpat_wild_pat(p->loc);_tmp12B;});_tmp12A->tl=wild_dps;_tmp12A;});}}
*_tmp8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpF5,wild_dps);
_tmpF5=*_tmp8B;}else{
if(_tmp128 == _tmp129)
({void*_tmp12C=0;Cyc_Tcutil_warn(p->loc,({const char*_tmp12D="unnecessary ... in struct pattern";_tag_dyneither(_tmp12D,sizeof(char),34);}),_tag_dyneither(_tmp12C,sizeof(void*),0));});}}}{
# 585
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpF5,_tmp88->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmp12E=(struct _tuple19*)fields->hd;struct _tuple19*_tmp12F=_tmp12E;struct Cyc_Absyn_Aggrfield*_tmp130;struct Cyc_Absyn_Pat*_tmp131;_LLB0: _tmp130=_tmp12F->f1;_tmp131=_tmp12F->f2;_LLB1:;{
void*_tmp132=Cyc_Tcutil_rsubstitute(rgn,_tmp11C,_tmp130->type);
# 591
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp130->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp131,& _tmp132,rgn_pat,allow_ref_pat,new_access_exp));
# 599
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp131->topt),_tmp132))
({struct Cyc_String_pa_PrintArg_struct _tmp138;_tmp138.tag=0;_tmp138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 602
Cyc_Absynpp_typ2string((void*)_check_null(_tmp131->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 601
Cyc_Absynpp_typ2string(_tmp132));({struct Cyc_String_pa_PrintArg_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({struct Cyc_String_pa_PrintArg_struct _tmp135;_tmp135.tag=0;_tmp135.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp130->name);({void*_tmp133[4]={& _tmp135,& _tmp136,& _tmp137,& _tmp138};Cyc_Tcutil_terr(p->loc,({const char*_tmp134="field %s of %s pattern expects type %s != %s";_tag_dyneither(_tmp134,sizeof(char),45);}),_tag_dyneither(_tmp133,sizeof(void*),4));});});});});});
# 603
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp131->topt;};}};}
# 541
;_pop_region(r2);};}
# 607
_npop_handler(0);goto _LL4E;
# 492
;_pop_region(rgn);};}}else{_LL73: _LL74:
# 663
 goto _LL76;}}else{_LL71: _LL72:
# 662
 goto _LL74;}case 8: _LL6F: _tmp84=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp85=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_tmp86=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f3;_tmp87=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f4;_LL70: {
# 610
struct Cyc_List_List*_tmp13C=*_tmp86;
struct _RegionHandle _tmp13D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp13D;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp85->typs;
# 614
struct Cyc_List_List*_tmp13E=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp13F=({struct _tuple12 _tmp158;_tmp158.f1=_tmp13E;_tmp158.f2=rgn;_tmp158;});
struct Cyc_List_List*_tmp140=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp13F,_tmp84->tvs);
struct Cyc_List_List*_tmp141=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp140);
t=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp143;_tmp143.tag=4;_tmp143.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp144;_tmp144.field_info=Cyc_Absyn_KnownDatatypefield(_tmp84,_tmp85);_tmp144.targs=_tmp141;_tmp144;});_tmp143;});_tmp142;});
# 620
if(_tmp87){
# 622
int _tmp145=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp13C);
int _tmp146=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp145 < _tmp146){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp146 - _tmp145;++ i){
wild_ps=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->hd=Cyc_Tcpat_wild_pat(p->loc);_tmp147->tl=wild_ps;_tmp147;});}}
*_tmp86=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp13C,wild_ps);
_tmp13C=*_tmp86;}else{
if(_tmp145 == _tmp146)
({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp84->name));({void*_tmp148[1]={& _tmp14A};Cyc_Tcutil_warn(p->loc,({const char*_tmp149="unnecessary ... in datatype field %s";_tag_dyneither(_tmp149,sizeof(char),37);}),_tag_dyneither(_tmp148,sizeof(void*),1));});});}}
# 634
for(0;_tmp13C != 0  && tqts != 0;(_tmp13C=_tmp13C->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp14B=(struct Cyc_Absyn_Pat*)_tmp13C->hd;
# 638
void*_tmp14C=Cyc_Tcutil_rsubstitute(rgn,_tmp140,(*((struct _tuple18*)tqts->hd)).f2);
# 641
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp14B,& _tmp14C,rgn_pat,allow_ref_pat,0));
# 648
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp14B->topt),_tmp14C))
({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0;_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 651
Cyc_Absynpp_typ2string((void*)_check_null(_tmp14B->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 650
Cyc_Absynpp_typ2string(_tmp14C));({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp85->name));({void*_tmp14D[3]={& _tmp14F,& _tmp150,& _tmp151};Cyc_Tcutil_terr(_tmp14B->loc,({const char*_tmp14E="%s expects argument type %s, not %s";_tag_dyneither(_tmp14E,sizeof(char),36);}),_tag_dyneither(_tmp14D,sizeof(void*),3));});});});});}
# 653
if(_tmp13C != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp85->name));({void*_tmp152[1]={& _tmp154};Cyc_Tcutil_terr(p->loc,({const char*_tmp153="too many arguments for datatype constructor %s";_tag_dyneither(_tmp153,sizeof(char),47);}),_tag_dyneither(_tmp152,sizeof(void*),1));});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0;_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp85->name));({void*_tmp155[1]={& _tmp157};Cyc_Tcutil_terr(p->loc,({const char*_tmp156="too few arguments for datatype constructor %s";_tag_dyneither(_tmp156,sizeof(char),46);}),_tag_dyneither(_tmp155,sizeof(void*),1));});});}
# 660
_npop_handler(0);goto _LL4E;
# 611
;_pop_region(rgn);}case 15: _LL75: _LL76:
# 664
 goto _LL78;case 17: _LL77: _LL78:
 goto _LL7A;default: _LL79: _LL7A:
# 667
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL4E;}_LL4E:;}
# 669
tcpat_end:
 p->topt=t;
return res;};}
# 674
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 676
struct Cyc_Tcpat_TcPatResult _tmp15A=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 678
struct _RegionHandle _tmp15B=_new_region("r");struct _RegionHandle*r=& _tmp15B;_push_region(r);{
struct _tuple1 _tmp15C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp15A.patvars);struct _tuple1 _tmp15D=_tmp15C;struct Cyc_List_List*_tmp15E;_LLB3: _tmp15E=_tmp15D.f1;_LLB4:;{
struct Cyc_List_List*_tmp15F=0;
{struct Cyc_List_List*x=_tmp15E;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)_tmp15F=({struct Cyc_List_List*_tmp160=_region_malloc(r,sizeof(*_tmp160));_tmp160->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd));_tmp160->tl=_tmp15F;_tmp160;});}}
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp15F),p->loc,({const char*_tmp161="pattern contains a repeated variable";_tag_dyneither(_tmp161,sizeof(char),37);}));{
# 688
struct Cyc_List_List*x=_tmp15A.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp162=(struct _tuple16*)x->hd;struct _tuple16*_tmp163=_tmp162;struct Cyc_Absyn_Vardecl**_tmp164;struct Cyc_Absyn_Exp**_tmp165;_LLB6: _tmp164=_tmp163->f1;_tmp165=(struct Cyc_Absyn_Exp**)& _tmp163->f2;_LLB7:;
if(*_tmp165 != 0  && *_tmp165 != pat_var_exp)
*_tmp165=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp165));}};};}{
# 694
struct Cyc_Tcpat_TcPatResult _tmp166=_tmp15A;_npop_handler(0);return _tmp166;};
# 678
;_pop_region(r);}
# 700
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 703
struct _tuple0 _tmp167=({struct _tuple0 _tmp16F;_tmp16F.f1=Cyc_Tcutil_compress(old_type);_tmp16F.f2=Cyc_Tcutil_compress(new_type);_tmp16F;});struct _tuple0 _tmp168=_tmp167;struct Cyc_Absyn_PtrInfo _tmp169;struct Cyc_Absyn_PtrInfo _tmp16A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f2)->tag == 5){_LLB9: _tmp169=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f1)->f1;_tmp16A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f2)->f1;_LLBA: {
# 705
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16B=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp16D;_tmp16D.tag=5;_tmp16D.f1=({struct Cyc_Absyn_PtrInfo _tmp16E;_tmp16E.elt_typ=_tmp169.elt_typ;_tmp16E.elt_tq=_tmp16A.elt_tq;_tmp16E.ptr_atts=({(_tmp16E.ptr_atts).rgn=(_tmp169.ptr_atts).rgn;(_tmp16E.ptr_atts).nullable=(_tmp16A.ptr_atts).nullable;(_tmp16E.ptr_atts).bounds=(_tmp16A.ptr_atts).bounds;(_tmp16E.ptr_atts).zero_term=(_tmp16A.ptr_atts).zero_term;(_tmp16E.ptr_atts).ptrloc=(_tmp169.ptr_atts).ptrloc;_tmp16E.ptr_atts;});_tmp16E;});_tmp16D;});_tmp16C;});
# 711
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp16B,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp16B);}}else{goto _LLBB;}}else{_LLBB: _LLBC:
 return 0;}_LLB8:;}
# 720
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 723
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->hd=({struct _tuple0*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=(void*)& Cyc_Absyn_UniqueRgn_val;_tmp17C->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp17E;_tmp17E.tag=2;_tmp17E.f1=tv;_tmp17E;});_tmp17D;});_tmp17C;});_tmp17B->tl=0;_tmp17B;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 741 "tcpat.cyc"
struct _tuple0 _tmp170=({struct _tuple0 _tmp176;_tmp176.f1=Cyc_Tcutil_compress(old_type);_tmp176.f2=Cyc_Tcutil_compress(new_type);_tmp176;});struct _tuple0 _tmp171=_tmp170;struct Cyc_Absyn_PtrInfo _tmp172;struct Cyc_Absyn_PtrInfo _tmp173;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f2)->tag == 5){_LLBE: _tmp172=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f1)->f1;_tmp173=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f2)->f1;_LLBF:
 goto _LLBD;}else{goto _LLC0;}}else{_LLC0: _LLC1:
({void*_tmp174=0;Cyc_Tcutil_terr(loc,({const char*_tmp175="alias requires pointer type";_tag_dyneither(_tmp175,sizeof(char),28);}),_tag_dyneither(_tmp174,sizeof(void*),0));});goto _LLBD;}_LLBD:;}else{
# 746
({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type));({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type));({void*_tmp177[2]={& _tmp179,& _tmp17A};Cyc_Tcutil_terr(loc,({const char*_tmp178="cannot alias value of type %s to type %s";_tag_dyneither(_tmp178,sizeof(char),41);}),_tag_dyneither(_tmp177,sizeof(void*),2));});});});}}
# 753
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 757
void*_tmp17F=p->r;void*_tmp180=_tmp17F;struct Cyc_Absyn_Tvar*_tmp181;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Pat*_tmp184;struct Cyc_Absyn_Vardecl*_tmp185;struct Cyc_Absyn_Pat*_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Absyn_AggrInfo*_tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Pat*_tmp18C;switch(*((int*)_tmp180)){case 6: _LLC3: _tmp18C=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_LLC4: {
# 759
void*_tmp18D=(void*)_check_null(p->topt);void*_tmp18E=_tmp18D;void*_tmp18F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18E)->tag == 5){_LLD4: _tmp18F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18E)->f1).ptr_atts).rgn;_LLD5:
# 761
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp18F);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp18C,Cyc_Tcutil_is_noalias_region(_tmp18F,0),patvars);
return;}else{_LLD6: _LLD7:
({void*_tmp190=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp191="check_pat_regions: bad pointer type";_tag_dyneither(_tmp191,sizeof(char),36);}),_tag_dyneither(_tmp190,sizeof(void*),0));});}_LLD3:;}case 7: _LLC5: _tmp189=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp18A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_tmp18B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f3;_LLC6:
# 767
 for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp18B->hd)).f2,did_noalias_deref,patvars);}
return;case 8: _LLC7: _tmp188=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp180)->f3;_LLC8:
# 771
 did_noalias_deref=0;_tmp187=_tmp188;goto _LLCA;case 5: _LLC9: _tmp187=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_LLCA:
# 773
 for(0;_tmp187 != 0;_tmp187=_tmp187->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp187->hd,did_noalias_deref,patvars);}
return;case 3: _LLCB: _tmp185=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp186=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLCC:
# 777
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp192=(struct _tuple16*)x->hd;struct _tuple16*_tmp193=_tmp192;struct Cyc_Absyn_Vardecl**_tmp194;struct Cyc_Absyn_Exp*_tmp195;_LLD9: _tmp194=_tmp193->f1;_tmp195=_tmp193->f2;_LLDA:;
# 783
if((_tmp194 != 0  && *_tmp194 == _tmp185) && _tmp195 != 0){
{void*_tmp196=_tmp195->r;void*_tmp197=_tmp196;struct Cyc_Absyn_Exp*_tmp198;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp197)->tag == 14){_LLDC: _tmp198=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp197)->f1;_LLDD:
# 786
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp198->topt)))
# 788
({void*_tmp199=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp19A="reference pattern not allowed on alias-free pointers";_tag_dyneither(_tmp19A,sizeof(char),53);}),_tag_dyneither(_tmp199,sizeof(void*),0));});
goto _LLDB;}else{_LLDE: _LLDF:
# 791
({void*_tmp19B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp19C="check_pat_regions: bad reference access expression";_tag_dyneither(_tmp19C,sizeof(char),51);}),_tag_dyneither(_tmp19B,sizeof(void*),0));});}_LLDB:;}
# 793
break;}}}
# 796
Cyc_Tcpat_check_pat_regions_rec(te,_tmp186,did_noalias_deref,patvars);
return;case 1: _LLCD: _tmp183=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp184=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLCE:
# 799
{void*_tmp19D=p->topt;void*_tmp19E=_tmp19D;if(_tmp19E != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp19E)->tag == 8){_LLE1: _LLE2:
# 801
 if(did_noalias_deref){
({void*_tmp19F=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp1A0="pattern to array would create alias of no-alias pointer";_tag_dyneither(_tmp1A0,sizeof(char),56);}),_tag_dyneither(_tmp19F,sizeof(void*),0));});
return;}
# 805
goto _LLE0;}else{goto _LLE3;}}else{_LLE3: _LLE4:
 goto _LLE0;}_LLE0:;}
# 808
Cyc_Tcpat_check_pat_regions_rec(te,_tmp184,did_noalias_deref,patvars);
return;case 2: _LLCF: _tmp181=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp182=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLD0:
# 811
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1A1=(struct _tuple16*)x->hd;struct _tuple16*_tmp1A2=_tmp1A1;struct Cyc_Absyn_Vardecl**_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;_LLE6: _tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;_LLE7:;
# 815
if(_tmp1A3 != 0  && *_tmp1A3 == _tmp182){
if(_tmp1A4 == 0)
({void*_tmp1A5=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp1A6="cannot alias pattern expression in datatype";_tag_dyneither(_tmp1A6,sizeof(char),44);}),_tag_dyneither(_tmp1A5,sizeof(void*),0));});else{
# 819
struct _RegionHandle _tmp1A7=_new_region("r");struct _RegionHandle*r=& _tmp1A7;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,p->loc,te,({struct Cyc_Absyn_Tvar*_tmp1AA[1];_tmp1AA[0]=_tmp181;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp1AA,sizeof(struct Cyc_Absyn_Tvar*),1));}));
te2=Cyc_Tcenv_add_region(r,te2,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1A9;_tmp1A9.tag=2;_tmp1A9.f1=_tmp181;_tmp1A9;});_tmp1A8;}),0,1);
# 823
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp1A4->topt),_tmp181,_tmp182->type,_tmp1A4);}
# 820
;_pop_region(r);}
# 827
break;}}}
# 830
goto _LLC2;default: _LLD1: _LLD2:
 return;}_LLC2:;}
# 846 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 848
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp1AB=_new_region("r");struct _RegionHandle*r=& _tmp1AB;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1AC=(struct _tuple16*)x->hd;struct _tuple16*_tmp1AD=_tmp1AC;struct Cyc_Absyn_Vardecl**_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AF;_LLE9: _tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;_LLEA:;
if(_tmp1AF != 0){
struct Cyc_Absyn_Exp*_tmp1B0=_tmp1AF;
# 856
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1B0->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1B0))
# 859
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(struct _dyneither_ptr)(
# 861
_tmp1AE != 0?(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B6;_tmp1B6.tag=0;_tmp1B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 863
Cyc_Absynpp_qvar2string((*_tmp1AE)->name));({void*_tmp1B4[1]={& _tmp1B6};Cyc_aprintf(({const char*_tmp1B5="for variable %s";_tag_dyneither(_tmp1B5,sizeof(char),16);}),_tag_dyneither(_tmp1B4,sizeof(void*),1));});}):({const char*_tmp1B7="";_tag_dyneither(_tmp1B7,sizeof(char),1);}));({void*_tmp1B1[1]={& _tmp1B3};Cyc_Tcutil_terr(p->loc,({const char*_tmp1B2="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_dyneither(_tmp1B2,sizeof(char),68);}),_tag_dyneither(_tmp1B1,sizeof(void*),1));});});}}}
# 850
;_pop_region(r);};}
# 918 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 928
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};union Cyc_Tcpat_PatOrWhere;struct Cyc_Tcpat_PathNode;struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 941
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp1BC;(_tmp1BC.Name_v).val=s;(_tmp1BC.Name_v).tag=1;_tmp1BC;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp1BD;(_tmp1BD.Int_v).val=i;(_tmp1BD.Int_v).tag=2;_tmp1BD;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};
# 951
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 956
typedef void*Cyc_Tcpat_simple_pat_t;
# 959
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp1BE=c1->name;union Cyc_Tcpat_Name_value _tmp1BF=_tmp1BE;int _tmp1C0;struct _dyneither_ptr _tmp1C1;if((_tmp1BF.Name_v).tag == 1){_LLEC: _tmp1C1=(_tmp1BF.Name_v).val;_LLED: {
# 962
union Cyc_Tcpat_Name_value _tmp1C2=c2->name;union Cyc_Tcpat_Name_value _tmp1C3=_tmp1C2;struct _dyneither_ptr _tmp1C4;if((_tmp1C3.Name_v).tag == 1){_LLF1: _tmp1C4=(_tmp1C3.Name_v).val;_LLF2:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1C1,(struct _dyneither_ptr)_tmp1C4);}else{_LLF3: _LLF4:
 return - 1;}_LLF0:;}}else{_LLEE: _tmp1C0=(_tmp1BF.Int_v).val;_LLEF: {
# 967
union Cyc_Tcpat_Name_value _tmp1C5=c2->name;union Cyc_Tcpat_Name_value _tmp1C6=_tmp1C5;int _tmp1C7;if((_tmp1C6.Name_v).tag == 1){_LLF6: _LLF7:
 return 1;}else{_LLF8: _tmp1C7=(_tmp1C6.Int_v).val;_LLF9:
 return _tmp1C0 - _tmp1C7;}_LLF5:;}}_LLEB:;}
# 975
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 979
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 983
static unsigned int Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned int ans=0;
struct Cyc_List_List*_tmp1C8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1C8))->hd)->name)!= 0){
++ ans;
_tmp1C8=_tmp1C8->tl;}
# 990
return ans;}
# 993
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp1C9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp1C9 != 0;_tmp1C9=_tmp1C9->tl){
struct Cyc_Absyn_Aggrfield*_tmp1CA=(struct Cyc_Absyn_Aggrfield*)_tmp1C9->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp1CA->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1000
({void*_tmp1CB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1CE;_tmp1CE.tag=0;_tmp1CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp1CC[1]={& _tmp1CE};Cyc_aprintf(({const char*_tmp1CD="get_member_offset %s failed";_tag_dyneither(_tmp1CD,sizeof(char),28);}),_tag_dyneither(_tmp1CC,sizeof(void*),1));});}),_tag_dyneither(_tmp1CB,sizeof(void*),0));});}
# 1003
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp1CF=pw;struct Cyc_Absyn_Pat*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;if((_tmp1CF.where_clause).tag == 2){_LLFB: _tmp1D1=(_tmp1CF.where_clause).val;_LLFC:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp1D3;_tmp1D3.tag=0;_tmp1D3.f1=_tmp1D1;_tmp1D3;});_tmp1D2;});}else{_LLFD: _tmp1D0=(_tmp1CF.pattern).val;_LLFE: {
# 1007
void*_tmp1D4=_tmp1D0->r;void*_tmp1D5=_tmp1D4;union Cyc_Absyn_AggrInfoU _tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_Datatypedecl*_tmp1D8;struct Cyc_Absyn_Datatypefield*_tmp1D9;void*_tmp1DA;struct Cyc_Absyn_Enumfield*_tmp1DB;struct Cyc_Absyn_Enumdecl*_tmp1DC;struct Cyc_Absyn_Enumfield*_tmp1DD;struct _dyneither_ptr _tmp1DE;int _tmp1DF;char _tmp1E0;enum Cyc_Absyn_Sign _tmp1E1;int _tmp1E2;struct Cyc_Absyn_Pat*_tmp1E3;struct Cyc_Absyn_Pat*_tmp1E4;switch(*((int*)_tmp1D5)){case 1: _LL100: _tmp1E4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL101:
 _tmp1E3=_tmp1E4;goto _LL103;case 3: _LL102: _tmp1E3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL103:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp1E5;(_tmp1E5.pattern).val=_tmp1E3;(_tmp1E5.pattern).tag=1;_tmp1E5;}));case 9: _LL104: _LL105:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _LL106: _tmp1E1=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1E2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL107:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp1E6=_cycalloc_atomic(sizeof(*_tmp1E6));_tmp1E6[0]=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp1E7;_tmp1E7.tag=6;_tmp1E7.f1=(unsigned int)_tmp1E2;_tmp1E7;});_tmp1E6;});case 11: _LL108: _tmp1E0=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL109:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp1E8=_cycalloc_atomic(sizeof(*_tmp1E8));_tmp1E8[0]=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp1E9;_tmp1E9.tag=6;_tmp1E9.f1=(unsigned int)_tmp1E0;_tmp1E9;});_tmp1E8;});case 12: _LL10A: _tmp1DE=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1DF=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL10B:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp1EB;_tmp1EB.tag=5;_tmp1EB.f1=_tmp1DE;_tmp1EB.f2=_tmp1DF;_tmp1EB;});_tmp1EA;});case 13: _LL10C: _tmp1DC=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1DD=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL10D:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC[0]=({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp1ED;_tmp1ED.tag=3;_tmp1ED.f1=_tmp1DC;_tmp1ED.f2=_tmp1DD;_tmp1ED;});_tmp1EC;});case 14: _LL10E: _tmp1DA=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1DB=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL10F:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp1EF;_tmp1EF.tag=4;_tmp1EF.f1=_tmp1DA;_tmp1EF.f2=_tmp1DB;_tmp1EF;});_tmp1EE;});case 6: _LL110: _LL111:
# 1017
{void*_tmp1F0=Cyc_Tcutil_compress((void*)_check_null(_tmp1D0->topt));void*_tmp1F1=_tmp1F0;union Cyc_Absyn_Constraint*_tmp1F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1)->tag == 5){_LL119: _tmp1F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1)->f1).ptr_atts).nullable;_LL11A:
# 1019
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp1F2))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL118;}else{_LL11B: _LL11C:
 goto _LL118;}_LL118:;}
# 1024
({void*_tmp1F3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F4="non-null pointer type or non-pointer type in pointer pattern";_tag_dyneither(_tmp1F4,sizeof(char),61);}),_tag_dyneither(_tmp1F3,sizeof(void*),0));});case 8: _LL112: _tmp1D8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1D9=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL113:
# 1026
 if(_tmp1D8->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp1F6;_tmp1F6.tag=9;_tmp1F6.f1=_tmp1D8;_tmp1F6.f2=_tmp1D9;_tmp1F6;});_tmp1F5;});else{
# 1029
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7[0]=({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp1F8;_tmp1F8.tag=7;_tmp1F8.f1=(int)Cyc_Tcpat_datatype_tag_number(_tmp1D8,_tmp1D9->name);_tmp1F8.f2=_tmp1D8;_tmp1F8.f3=_tmp1D9;_tmp1F8;});_tmp1F7;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1 != 0){_LL114: _tmp1D6=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1)->aggr_info;_tmp1D7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_LL115: {
# 1031
struct Cyc_Absyn_Aggrdecl*_tmp1F9=Cyc_Absyn_get_known_aggrdecl(_tmp1D6);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F9->impl))->tagged){
struct _tuple15*_tmp1FA=(struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp1D7))->hd;struct _tuple15*_tmp1FB=_tmp1FA;struct Cyc_List_List*_tmp1FC;struct Cyc_Absyn_Pat*_tmp1FD;_LL11E: _tmp1FC=_tmp1FB->f1;_tmp1FD=_tmp1FB->f2;_LL11F:;{
struct _dyneither_ptr*f;
{void*_tmp1FE=(void*)((struct Cyc_List_List*)_check_null(_tmp1FC))->hd;void*_tmp1FF=_tmp1FE;struct _dyneither_ptr*_tmp200;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1FF)->tag == 1){_LL121: _tmp200=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1FF)->f1;_LL122:
 f=_tmp200;goto _LL120;}else{_LL123: _LL124:
({void*_tmp201=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp202="no field name in tagged union pattern";_tag_dyneither(_tmp202,sizeof(char),38);}),_tag_dyneither(_tmp201,sizeof(void*),0));});}_LL120:;}
# 1039
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203[0]=({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp204;_tmp204.tag=8;_tmp204.f1=f;_tmp204.f2=Cyc_Tcpat_get_member_offset(_tmp1F9,f);_tmp204;});_tmp203;});};}else{
# 1041
({void*_tmp205=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp206="non-tagged aggregate in pattern test";_tag_dyneither(_tmp206,sizeof(char),37);}),_tag_dyneither(_tmp205,sizeof(void*),0));});}}}else{goto _LL116;}default: _LL116: _LL117:
({void*_tmp207=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp208="non-test pattern in pattern test";_tag_dyneither(_tmp208,sizeof(char),33);}),_tag_dyneither(_tmp207,sizeof(void*),0));});}_LLFF:;}}_LLFA:;}
# 1047
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp209;(_tmp209.pattern).val=p;(_tmp209.pattern).tag=1;_tmp209;});}
# 1051
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->name=Cyc_Tcpat_Name_v(({const char*_tmp20B="NULL";_tag_dyneither(_tmp20B,sizeof(char),5);}));_tmp20A->arity=0;_tmp20A->span=& Cyc_Tcpat_two_opt;_tmp20A->orig_pat=Cyc_Tcpat_pw(p);_tmp20A;});}
# 1054
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->name=Cyc_Tcpat_Name_v(({const char*_tmp20D="&";_tag_dyneither(_tmp20D,sizeof(char),2);}));_tmp20C->arity=1;_tmp20C->span=& Cyc_Tcpat_two_opt;_tmp20C->orig_pat=Cyc_Tcpat_pw(p);_tmp20C;});}
# 1057
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->name=Cyc_Tcpat_Name_v(({const char*_tmp20F="&";_tag_dyneither(_tmp20F,sizeof(char),2);}));_tmp20E->arity=1;_tmp20E->span=& Cyc_Tcpat_one_opt;_tmp20E->orig_pat=Cyc_Tcpat_pw(p);_tmp20E;});}
# 1060
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->name=Cyc_Tcpat_Int_v(i);_tmp210->arity=0;_tmp210->span=0;_tmp210->orig_pat=p;_tmp210;});}
# 1063
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->name=Cyc_Tcpat_Name_v(f);_tmp211->arity=0;_tmp211->span=0;_tmp211->orig_pat=Cyc_Tcpat_pw(p);_tmp211;});}
# 1066
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->name=Cyc_Tcpat_Int_v((int)c);_tmp212->arity=0;_tmp212->span=& Cyc_Tcpat_twofiftysix_opt;_tmp212->orig_pat=Cyc_Tcpat_pw(p);_tmp212;});}
# 1069
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->name=Cyc_Tcpat_Name_v(({const char*_tmp214="$";_tag_dyneither(_tmp214,sizeof(char),2);}));_tmp213->arity=i;_tmp213->span=& Cyc_Tcpat_one_opt;_tmp213->orig_pat=p;_tmp213;});}
# 1074
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp216;_tmp216.tag=1;_tmp216.f1=Cyc_Tcpat_null_con(p);_tmp216.f2=0;_tmp216;});_tmp215;});}
# 1077
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp218;_tmp218.tag=1;_tmp218.f1=Cyc_Tcpat_int_con(i,p);_tmp218.f2=0;_tmp218;});_tmp217;});}
# 1080
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp21A;_tmp21A.tag=1;_tmp21A.f1=Cyc_Tcpat_char_con(c,p);_tmp21A.f2=0;_tmp21A;});_tmp219;});}
# 1083
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp21C;_tmp21C.tag=1;_tmp21C.f1=Cyc_Tcpat_float_con(f,p);_tmp21C.f2=0;_tmp21C;});_tmp21B;});}
# 1086
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp21E;_tmp21E.tag=1;_tmp21E.f1=Cyc_Tcpat_null_ptr_con(p0);_tmp21E.f2=({struct Cyc_List_List*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=p;_tmp21F->tl=0;_tmp21F;});_tmp21E;});_tmp21D;});}
# 1089
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp221;_tmp221.tag=1;_tmp221.f1=Cyc_Tcpat_ptr_con(p0);_tmp221.f2=({struct Cyc_List_List*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->hd=p;_tmp222->tl=0;_tmp222;});_tmp221;});_tmp220;});}
# 1092
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp224;_tmp224.tag=1;_tmp224.f1=Cyc_Tcpat_tuple_con(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p);_tmp224.f2=ss;_tmp224;});_tmp223;});}
# 1095
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1097
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->name=Cyc_Tcpat_Name_v(con_name);_tmp227->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp227->span=span;_tmp227->orig_pat=Cyc_Tcpat_pw(p);_tmp227;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp226;_tmp226.tag=1;_tmp226.f1=c;_tmp226.f2=ps;_tmp226;});_tmp225;});}
# 1102
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp228=p->r;void*_tmp229=_tmp228;void*_tmp22A;struct Cyc_Absyn_Enumfield*_tmp22B;struct Cyc_Absyn_Enumdecl*_tmp22C;struct Cyc_Absyn_Enumfield*_tmp22D;struct Cyc_Absyn_Aggrdecl*_tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_Absyn_Datatypedecl*_tmp231;struct Cyc_Absyn_Datatypefield*_tmp232;struct Cyc_List_List*_tmp233;struct Cyc_Absyn_Pat*_tmp234;struct Cyc_Absyn_Pat*_tmp235;struct Cyc_Absyn_Pat*_tmp236;struct _dyneither_ptr _tmp237;char _tmp238;enum Cyc_Absyn_Sign _tmp239;int _tmp23A;switch(*((int*)_tmp229)){case 0: _LL126: _LL127:
 goto _LL129;case 2: _LL128: _LL129:
 goto _LL12B;case 4: _LL12A: _LL12B:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp23B=_cycalloc_atomic(sizeof(*_tmp23B));_tmp23B[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp23C;_tmp23C.tag=0;_tmp23C;});_tmp23B;});goto _LL125;case 9: _LL12C: _LL12D:
 s=Cyc_Tcpat_null_pat(p);goto _LL125;case 10: _LL12E: _tmp239=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp23A=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL12F:
 s=Cyc_Tcpat_int_pat(_tmp23A,Cyc_Tcpat_pw(p));goto _LL125;case 11: _LL130: _tmp238=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL131:
 s=Cyc_Tcpat_char_pat(_tmp238,p);goto _LL125;case 12: _LL132: _tmp237=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL133:
 s=Cyc_Tcpat_float_pat(_tmp237,p);goto _LL125;case 1: _LL134: _tmp236=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL135:
 s=Cyc_Tcpat_compile_pat(_tmp236);goto _LL125;case 3: _LL136: _tmp235=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL137:
 s=Cyc_Tcpat_compile_pat(_tmp235);goto _LL125;case 6: _LL138: _tmp234=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL139:
# 1115
{void*_tmp23D=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp23E=_tmp23D;union Cyc_Absyn_Constraint*_tmp23F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E)->tag == 5){_LL14D: _tmp23F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E)->f1).ptr_atts).nullable;_LL14E: {
# 1117
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp240=_tmp23F;int _tmp241;union Cyc_Absyn_Constraint*_tmp242;switch((((union Cyc_Absyn_Constraint*)_tmp240)->No_constr).tag){case 2: _LL152: _tmp242=(_tmp240->Forward_constr).val;_LL153:
# 1122
*_tmp23F=*_tmp242;
continue;case 3: _LL154: _LL155:
# 1125
({struct _union_Constraint_Eq_constr*_tmp243=& _tmp23F->Eq_constr;_tmp243->tag=1;_tmp243->val=0;});
is_nullable=0;
still_working=0;
goto _LL151;default: _LL156: _tmp241=(int)(_tmp240->Eq_constr).val;_LL157:
# 1130
 is_nullable=_tmp241;
still_working=0;
goto _LL151;}_LL151:;}{
# 1135
void*ss=Cyc_Tcpat_compile_pat(_tmp234);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(ss,p);else{
# 1139
s=Cyc_Tcpat_ptr_pat(ss,p);}
goto _LL14C;};}}else{_LL14F: _LL150:
({void*_tmp244=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp245="expecting pointertype for pattern!";_tag_dyneither(_tmp245,sizeof(char),35);}),_tag_dyneither(_tmp244,sizeof(void*),0));});}_LL14C:;}
# 1143
goto _LL125;case 8: _LL13A: _tmp231=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp232=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_tmp233=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f3;_LL13B: {
# 1145
int*span;
{void*_tmp246=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp247=_tmp246;switch(*((int*)_tmp247)){case 3: _LL159: _LL15A:
# 1148
 if(_tmp231->is_extensible)
span=0;else{
# 1151
span=({int*_tmp248=_cycalloc_atomic(sizeof(*_tmp248));_tmp248[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp231->fields))->v);_tmp248;});}
goto _LL158;case 4: _LL15B: _LL15C:
 span=& Cyc_Tcpat_one_opt;goto _LL158;default: _LL15D: _LL15E:
 span=({void*_tmp249=0;((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24A="void datatype pattern has bad type";_tag_dyneither(_tmp24A,sizeof(char),35);}),_tag_dyneither(_tmp249,sizeof(void*),0));});goto _LL158;}_LL158:;}
# 1156
s=Cyc_Tcpat_con_pat(*(*_tmp232->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp233),p);
goto _LL125;}case 5: _LL13C: _tmp230=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL13D:
# 1160
 s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp230),Cyc_Tcpat_pw(p));goto _LL125;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1)->aggr_info).KnownAggr).tag == 2){_LL13E: _tmp22E=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1)->aggr_info).KnownAggr).val;_tmp22F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f3;_LL13F:
# 1165
 if(_tmp22E->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1169
int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*_tmp256="";_tag_dyneither(_tmp256,sizeof(char),1);}))== 0;
{struct Cyc_List_List*dlps0=_tmp22F;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_tmp24B=(struct _tuple15*)dlps0->hd;struct _tuple15*_tmp24C=_tmp24B;struct Cyc_List_List*_tmp24D;struct Cyc_Absyn_Pat*_tmp24E;_LL160: _tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;_LL161:;{
struct Cyc_List_List*_tmp24F=_tmp24D;struct _dyneither_ptr*_tmp250;if(_tmp24F != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp24F)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp24F)->tl == 0){_LL163: _tmp250=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp24F->hd)->f1;_LL164:
# 1174
 if(Cyc_strptrcmp(_tmp250,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251->hd=Cyc_Tcpat_compile_pat(_tmp24E);_tmp251->tl=ps;_tmp251;});
found=1;}
# 1178
goto _LL162;}else{goto _LL165;}}else{goto _LL165;}}else{_LL165: _LL166:
({void*_tmp252=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp253="bad designator(s)";_tag_dyneither(_tmp253,sizeof(char),18);}),_tag_dyneither(_tmp252,sizeof(void*),0));});}_LL162:;};}}
# 1182
if(!found)
({void*_tmp254=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp255="bad designator";_tag_dyneither(_tmp255,sizeof(char),15);}),_tag_dyneither(_tmp254,sizeof(void*),0));});}}
# 1185
s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps),Cyc_Tcpat_pw(p));}else{
# 1188
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->tagged)
({void*_tmp257=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp258="patterns on untagged unions not yet supported.";_tag_dyneither(_tmp258,sizeof(char),47);}),_tag_dyneither(_tmp257,sizeof(void*),0));});{
int*span=({int*_tmp25F=_cycalloc_atomic(sizeof(*_tmp25F));_tmp25F[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->fields);_tmp25F;});
struct Cyc_List_List*_tmp259=_tmp22F;struct _dyneither_ptr*_tmp25A;struct Cyc_Absyn_Pat*_tmp25B;if(_tmp259 != 0){if(((struct _tuple15*)((struct Cyc_List_List*)_tmp259)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp259)->hd)->f1)->hd)->tag == 1){if(((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp259)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp259)->tl == 0){_LL168: _tmp25A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple15*)_tmp259->hd)->f1)->hd)->f1;_tmp25B=((struct _tuple15*)_tmp259->hd)->f2;_LL169:
# 1193
 s=Cyc_Tcpat_con_pat(*_tmp25A,span,({struct Cyc_List_List*_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C->hd=Cyc_Tcpat_compile_pat(_tmp25B);_tmp25C->tl=0;_tmp25C;}),p);
goto _LL167;}else{goto _LL16A;}}else{goto _LL16A;}}else{goto _LL16A;}}else{goto _LL16A;}}else{_LL16A: _LL16B:
({void*_tmp25D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp25E="bad union pattern";_tag_dyneither(_tmp25E,sizeof(char),18);}),_tag_dyneither(_tmp25D,sizeof(void*),0));});}_LL167:;};}
# 1198
goto _LL125;}else{goto _LL148;}}else{_LL148: _LL149:
# 1240
 goto _LL14B;}case 13: _LL140: _tmp22C=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp22D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL141:
# 1203
{void*_tmp260=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp261=_tmp260;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp261)->tag == 6){_LL16D: _LL16E:
# 1207
 s=Cyc_Tcpat_con_pat(*(*_tmp22D->name).f2,0,0,p);
goto _LL16C;}else{_LL16F: _LL170: {
# 1210
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22C->fields))->v);
s=Cyc_Tcpat_con_pat(*(*_tmp22D->name).f2,({int*_tmp262=_cycalloc_atomic(sizeof(*_tmp262));_tmp262[0]=span;_tmp262;}),0,p);
goto _LL16C;}}_LL16C:;}
# 1214
goto _LL125;case 14: _LL142: _tmp22A=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp22B=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL143: {
# 1219
struct Cyc_List_List*fields;
{void*_tmp263=Cyc_Tcutil_compress(_tmp22A);void*_tmp264=_tmp263;struct Cyc_List_List*_tmp265;if(((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp264)->tag == 14){_LL172: _tmp265=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp264)->f1;_LL173:
 fields=_tmp265;goto _LL171;}else{_LL174: _LL175:
({void*_tmp266=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp267="bad type in AnonEnum_p";_tag_dyneither(_tmp267,sizeof(char),23);}),_tag_dyneither(_tmp266,sizeof(void*),0));});}_LL171:;}
# 1226
{void*_tmp268=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp269=_tmp268;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp269)->tag == 6){_LL177: _LL178:
# 1230
 s=Cyc_Tcpat_con_pat(*(*_tmp22B->name).f2,0,0,p);
goto _LL176;}else{_LL179: _LL17A: {
# 1233
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
s=Cyc_Tcpat_con_pat(*(*_tmp22B->name).f2,({int*_tmp26A=_cycalloc_atomic(sizeof(*_tmp26A));_tmp26A[0]=span;_tmp26A;}),0,p);
goto _LL176;}}_LL176:;}
# 1237
goto _LL125;}case 15: _LL144: _LL145:
 goto _LL147;case 16: _LL146: _LL147:
 goto _LL149;default: _LL14A: _LL14B:
# 1241
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp26B=_cycalloc_atomic(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp26C;_tmp26C.tag=0;_tmp26C;});_tmp26B;});}_LL125:;}
# 1243
return s;}
# 1251
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1265
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1272
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp26D=({struct _tuple0 _tmp279;_tmp279.f1=a1;_tmp279.f2=a2;_tmp279;});struct _tuple0 _tmp26E=_tmp26D;int _tmp26F;struct _dyneither_ptr*_tmp270;int _tmp271;struct _dyneither_ptr*_tmp272;struct Cyc_Absyn_Datatypefield*_tmp273;unsigned int _tmp274;struct Cyc_Absyn_Datatypefield*_tmp275;unsigned int _tmp276;unsigned int _tmp277;unsigned int _tmp278;switch(*((int*)_tmp26E.f1)){case 0: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp26E.f2)->tag == 0){_LL17C: _LL17D:
 return 1;}else{goto _LL186;}case 1: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp26E.f2)->tag == 1){_LL17E: _LL17F:
 return 1;}else{goto _LL186;}case 2: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 2){_LL180: _tmp277=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f1)->f1;_tmp278=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f2)->f1;_LL181:
 return _tmp277 == _tmp278;}else{goto _LL186;}case 3: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 3){_LL182: _tmp273=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f1)->f2;_tmp274=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f1)->f3;_tmp275=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->f2;_tmp276=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->f3;_LL183:
# 1278
 return _tmp273 == _tmp275  && _tmp274 == _tmp276;}else{goto _LL186;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 4){_LL184: _tmp26F=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f1)->f1;_tmp270=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f1)->f2;_tmp271=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->f1;_tmp272=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->f2;_LL185:
# 1280
 return _tmp26F == _tmp271  && Cyc_strptrcmp(_tmp270,_tmp272)== 0;}else{_LL186: _LL187:
 return 0;}}_LL17B:;}
# 1285
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0  && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1291
if(p1 != p2)return 0;
return 1;}
# 1295
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp27A=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp27B=d2;struct Cyc_List_List*_tmp27C;struct Cyc_List_List*_tmp27D;void*_tmp27E;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->tag == 2){_LL189: _tmp27C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f1;_tmp27D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f2;_tmp27E=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f3;_LL18A:
# 1299
 if(Cyc_Tcpat_same_path(access,_tmp27C) && (int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F[0]=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp280;_tmp280.tag=2;_tmp280.f1=_tmp27C;_tmp280.f2=({struct Cyc_List_List*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281->hd=({struct _tuple0*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->f1=_tmp27A;_tmp282->f2=d1;_tmp282;});_tmp281->tl=_tmp27D;_tmp281;});_tmp280.f3=_tmp27E;_tmp280;});_tmp27F;});else{
# 1303
goto _LL188;}}else{_LL18B: _LL18C:
 goto _LL188;}_LL188:;}
# 1306
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp284;_tmp284.tag=2;_tmp284.f1=access;_tmp284.f2=({struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=({struct _tuple0*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->f1=_tmp27A;_tmp286->f2=d1;_tmp286;});_tmp285->tl=0;_tmp285;});_tmp284.f3=d2;_tmp284;});_tmp283;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1312
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1315
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1317
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1320
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp287=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp288=" ";_tag_dyneither(_tmp288,sizeof(char),2);}),_tag_dyneither(_tmp287,sizeof(void*),0));});}}
# 1326
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp289=c->name;
union Cyc_Tcpat_Name_value _tmp28A=_tmp289;int _tmp28B;struct _dyneither_ptr _tmp28C;if((_tmp28A.Name_v).tag == 1){_LL18E: _tmp28C=(_tmp28A.Name_v).val;_LL18F:
({struct Cyc_String_pa_PrintArg_struct _tmp28F;_tmp28F.tag=0;_tmp28F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp28C);({void*_tmp28D[1]={& _tmp28F};Cyc_fprintf(Cyc_stderr,({const char*_tmp28E="%s";_tag_dyneither(_tmp28E,sizeof(char),3);}),_tag_dyneither(_tmp28D,sizeof(void*),1));});});goto _LL18D;}else{_LL190: _tmp28B=(_tmp28A.Int_v).val;_LL191:
({struct Cyc_Int_pa_PrintArg_struct _tmp292;_tmp292.tag=1;_tmp292.f1=(unsigned long)_tmp28B;({void*_tmp290[1]={& _tmp292};Cyc_fprintf(Cyc_stderr,({const char*_tmp291="%d";_tag_dyneither(_tmp291,sizeof(char),3);}),_tag_dyneither(_tmp290,sizeof(void*),1));});});goto _LL18D;}_LL18D:;}
# 1334
static void Cyc_Tcpat_print_access(void*a){
void*_tmp293=a;int _tmp294;struct _dyneither_ptr*_tmp295;struct Cyc_Absyn_Datatypefield*_tmp296;unsigned int _tmp297;unsigned int _tmp298;switch(*((int*)_tmp293)){case 0: _LL193: _LL194:
({void*_tmp299=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29A="DUMMY";_tag_dyneither(_tmp29A,sizeof(char),6);}),_tag_dyneither(_tmp299,sizeof(void*),0));});goto _LL192;case 1: _LL195: _LL196:
({void*_tmp29B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29C="*";_tag_dyneither(_tmp29C,sizeof(char),2);}),_tag_dyneither(_tmp29B,sizeof(void*),0));});goto _LL192;case 2: _LL197: _tmp298=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp293)->f1;_LL198:
({struct Cyc_Int_pa_PrintArg_struct _tmp29F;_tmp29F.tag=1;_tmp29F.f1=(unsigned long)((int)_tmp298);({void*_tmp29D[1]={& _tmp29F};Cyc_fprintf(Cyc_stderr,({const char*_tmp29E="[%d]";_tag_dyneither(_tmp29E,sizeof(char),5);}),_tag_dyneither(_tmp29D,sizeof(void*),1));});});goto _LL192;case 3: _LL199: _tmp296=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp293)->f2;_tmp297=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp293)->f3;_LL19A:
# 1340
({struct Cyc_Int_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=1;_tmp2A3.f1=(unsigned long)((int)_tmp297);({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp296->name));({void*_tmp2A0[2]={& _tmp2A2,& _tmp2A3};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A1="%s[%d]";_tag_dyneither(_tmp2A1,sizeof(char),7);}),_tag_dyneither(_tmp2A0,sizeof(void*),2));});});});goto _LL192;default: _LL19B: _tmp294=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp293)->f1;_tmp295=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp293)->f2;_LL19C:
# 1342
 if(_tmp294)
({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0;_tmp2A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp295);({void*_tmp2A4[1]={& _tmp2A6};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A5=".tagunion.%s";_tag_dyneither(_tmp2A5,sizeof(char),13);}),_tag_dyneither(_tmp2A4,sizeof(void*),1));});});else{
# 1345
({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp295);({void*_tmp2A7[1]={& _tmp2A9};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A8=".%s";_tag_dyneither(_tmp2A8,sizeof(char),4);}),_tag_dyneither(_tmp2A7,sizeof(void*),1));});});}
goto _LL192;}_LL192:;}
# 1350
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp2AA=p;struct Cyc_Absyn_Datatypefield*_tmp2AB;struct _dyneither_ptr*_tmp2AC;int _tmp2AD;int _tmp2AE;unsigned int _tmp2AF;struct _dyneither_ptr _tmp2B0;struct Cyc_Absyn_Enumfield*_tmp2B1;struct Cyc_Absyn_Enumfield*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B3;switch(*((int*)_tmp2AA)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp2AA)->f1 == 0){_LL19E: _LL19F:
({void*_tmp2B4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2B5="where(NULL)";_tag_dyneither(_tmp2B5,sizeof(char),12);}),_tag_dyneither(_tmp2B4,sizeof(void*),0));});goto _LL19D;}else{_LL1A0: _tmp2B3=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1A1:
({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp2B3)));({void*_tmp2B6[1]={& _tmp2B8};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B7="where(%s)";_tag_dyneither(_tmp2B7,sizeof(char),10);}),_tag_dyneither(_tmp2B6,sizeof(void*),1));});});goto _LL19D;}case 1: _LL1A2: _LL1A3:
({void*_tmp2B9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2BA="NULL";_tag_dyneither(_tmp2BA,sizeof(char),5);}),_tag_dyneither(_tmp2B9,sizeof(void*),0));});goto _LL19D;case 2: _LL1A4: _LL1A5:
({void*_tmp2BB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2BC="NOT-NULL:";_tag_dyneither(_tmp2BC,sizeof(char),10);}),_tag_dyneither(_tmp2BB,sizeof(void*),0));});goto _LL19D;case 4: _LL1A6: _tmp2B2=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1A7:
 _tmp2B1=_tmp2B2;goto _LL1A9;case 3: _LL1A8: _tmp2B1=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1A9:
({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B1->name));({void*_tmp2BD[1]={& _tmp2BF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BE="%s";_tag_dyneither(_tmp2BE,sizeof(char),3);}),_tag_dyneither(_tmp2BD,sizeof(void*),1));});});goto _LL19D;case 5: _LL1AA: _tmp2B0=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1AB:
({struct Cyc_String_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2B0);({void*_tmp2C0[1]={& _tmp2C2};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C1="%s";_tag_dyneither(_tmp2C1,sizeof(char),3);}),_tag_dyneither(_tmp2C0,sizeof(void*),1));});});goto _LL19D;case 6: _LL1AC: _tmp2AF=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1AD:
({struct Cyc_Int_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=1;_tmp2C5.f1=(unsigned long)((int)_tmp2AF);({void*_tmp2C3[1]={& _tmp2C5};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C4="%d";_tag_dyneither(_tmp2C4,sizeof(char),3);}),_tag_dyneither(_tmp2C3,sizeof(void*),1));});});goto _LL19D;case 7: _LL1AE: _tmp2AE=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1AF:
({struct Cyc_Int_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=1;_tmp2C8.f1=(unsigned long)_tmp2AE;({void*_tmp2C6[1]={& _tmp2C8};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C7="datatypetag(%d)";_tag_dyneither(_tmp2C7,sizeof(char),16);}),_tag_dyneither(_tmp2C6,sizeof(void*),1));});});goto _LL19D;case 8: _LL1B0: _tmp2AC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp2AA)->f1;_tmp2AD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1B1:
({struct Cyc_Int_pa_PrintArg_struct _tmp2CC;_tmp2CC.tag=1;_tmp2CC.f1=(unsigned long)_tmp2AD;({struct Cyc_String_pa_PrintArg_struct _tmp2CB;_tmp2CB.tag=0;_tmp2CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2AC);({void*_tmp2C9[2]={& _tmp2CB,& _tmp2CC};Cyc_fprintf(Cyc_stderr,({const char*_tmp2CA="uniontag[%s](%d)";_tag_dyneither(_tmp2CA,sizeof(char),17);}),_tag_dyneither(_tmp2C9,sizeof(void*),2));});});});goto _LL19D;default: _LL1B2: _tmp2AB=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1B3:
# 1363
({struct Cyc_String_pa_PrintArg_struct _tmp2CF;_tmp2CF.tag=0;_tmp2CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2AB->name));({void*_tmp2CD[1]={& _tmp2CF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2CE="datatypefield(%s)";_tag_dyneither(_tmp2CE,sizeof(char),18);}),_tag_dyneither(_tmp2CD,sizeof(void*),1));});});}_LL19D:;}
# 1367
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs));({void*_tmp2D0[1]={& _tmp2D2};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D1="%s";_tag_dyneither(_tmp2D1,sizeof(char),3);}),_tag_dyneither(_tmp2D0,sizeof(void*),1));});});}
# 1371
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp2D3=d;struct Cyc_List_List*_tmp2D4;struct Cyc_List_List*_tmp2D5;void*_tmp2D6;struct Cyc_Tcpat_Rhs*_tmp2D7;switch(*((int*)_tmp2D3)){case 1: _LL1B5: _tmp2D7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2D3)->f1;_LL1B6:
# 1374
 Cyc_Tcpat_print_tab(tab);
({void*_tmp2D8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D9="Success(";_tag_dyneither(_tmp2D9,sizeof(char),9);}),_tag_dyneither(_tmp2D8,sizeof(void*),0));});Cyc_Tcpat_print_rhs(_tmp2D7);({void*_tmp2DA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DB=")\n";_tag_dyneither(_tmp2DB,sizeof(char),3);}),_tag_dyneither(_tmp2DA,sizeof(void*),0));});
goto _LL1B4;case 0: _LL1B7: _LL1B8:
({void*_tmp2DC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DD="Failure\n";_tag_dyneither(_tmp2DD,sizeof(char),9);}),_tag_dyneither(_tmp2DC,sizeof(void*),0));});goto _LL1B4;default: _LL1B9: _tmp2D4=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f1;_tmp2D5=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f2;_tmp2D6=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f3;_LL1BA:
# 1379
 Cyc_Tcpat_print_tab(tab);
({void*_tmp2DE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DF="Switch[";_tag_dyneither(_tmp2DF,sizeof(char),8);}),_tag_dyneither(_tmp2DE,sizeof(void*),0));});
for(0;_tmp2D4 != 0;_tmp2D4=_tmp2D4->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp2D4->hd)->access);
if(_tmp2D4->tl != 0)({void*_tmp2E0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E1=",";_tag_dyneither(_tmp2E1,sizeof(char),2);}),_tag_dyneither(_tmp2E0,sizeof(void*),0));});}
# 1385
({void*_tmp2E2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E3="] {\n";_tag_dyneither(_tmp2E3,sizeof(char),5);}),_tag_dyneither(_tmp2E2,sizeof(void*),0));});
for(0;_tmp2D5 != 0;_tmp2D5=_tmp2D5->tl){
struct _tuple0 _tmp2E4=*((struct _tuple0*)_tmp2D5->hd);struct _tuple0 _tmp2E5=_tmp2E4;void*_tmp2E6;void*_tmp2E7;_LL1BC: _tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;_LL1BD:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp2E8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E9="case ";_tag_dyneither(_tmp2E9,sizeof(char),6);}),_tag_dyneither(_tmp2E8,sizeof(void*),0));});
Cyc_Tcpat_print_pat_test(_tmp2E6);
({void*_tmp2EA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2EB=":\n";_tag_dyneither(_tmp2EB,sizeof(char),3);}),_tag_dyneither(_tmp2EA,sizeof(void*),0));});
Cyc_Tcpat_print_dec_tree(_tmp2E7,tab + 7);}
# 1394
Cyc_Tcpat_print_tab(tab);
({void*_tmp2EC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2ED="default:\n";_tag_dyneither(_tmp2ED,sizeof(char),10);}),_tag_dyneither(_tmp2EC,sizeof(void*),0));});
Cyc_Tcpat_print_dec_tree(_tmp2D6,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp2EE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2EF="}\n";_tag_dyneither(_tmp2EF,sizeof(char),3);}),_tag_dyneither(_tmp2EE,sizeof(void*),0));});}_LL1B4:;}
# 1402
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1410
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2F0=td;struct Cyc_Set_Set*_tmp2F1;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F0)->tag == 1){_LL1BF: _tmp2F1=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F0)->f1;_LL1C0:
# 1420
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2F3;_tmp2F3.tag=1;_tmp2F3.f1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp2F1,c);_tmp2F3;});_tmp2F2;});}else{_LL1C1: _LL1C2:
({void*_tmp2F4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2F5="add_neg called when td is Positive";_tag_dyneither(_tmp2F5,sizeof(char),35);}),_tag_dyneither(_tmp2F4,sizeof(void*),0));});}_LL1BE:;}
# 1427
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2F6=td;struct Cyc_Set_Set*_tmp2F7;struct Cyc_Tcpat_Con_s*_tmp2F8;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F6)->tag == 0){_LL1C4: _tmp2F8=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F6)->f1;_LL1C5:
# 1431
 if(Cyc_Tcpat_compare_con(c,_tmp2F8)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL1C6: _tmp2F7=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F6)->f1;_LL1C7:
# 1435
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F7,c))return Cyc_Tcpat_No;else{
# 1438
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2F7)+ 1)
return Cyc_Tcpat_Yes;else{
# 1441
return Cyc_Tcpat_Maybe;}}}_LL1C3:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1449
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp2F9=ctxt;struct Cyc_Tcpat_Con_s*_tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_List_List*_tmp2FC;if(_tmp2F9 == 0){_LL1C9: _LL1CA:
 return 0;}else{_LL1CB: _tmp2FA=((struct _tuple21*)_tmp2F9->hd)->f1;_tmp2FB=((struct _tuple21*)_tmp2F9->hd)->f2;_tmp2FC=_tmp2F9->tl;_LL1CC:
# 1453
 return({struct Cyc_List_List*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->hd=({struct _tuple21*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE->f1=_tmp2FA;_tmp2FE->f2=({struct Cyc_List_List*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->hd=dsc;_tmp2FF->tl=_tmp2FB;_tmp2FF;});_tmp2FE;});_tmp2FD->tl=_tmp2FC;_tmp2FD;});}_LL1C8:;}
# 1461
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp300=ctxt;struct Cyc_Tcpat_Con_s*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp303;if(_tmp300 == 0){_LL1CE: _LL1CF:
({void*_tmp304=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp305="norm_context: empty context";_tag_dyneither(_tmp305,sizeof(char),28);}),_tag_dyneither(_tmp304,sizeof(void*),0));});}else{_LL1D0: _tmp301=((struct _tuple21*)_tmp300->hd)->f1;_tmp302=((struct _tuple21*)_tmp300->hd)->f2;_tmp303=_tmp300->tl;_LL1D1:
# 1465
 return Cyc_Tcpat_augment(_tmp303,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306[0]=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp307;_tmp307.tag=0;_tmp307.f1=_tmp301;_tmp307.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp302);_tmp307;});_tmp306;}));}_LL1CD:;}
# 1474
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1476
struct _tuple1 _tmp308=({struct _tuple1 _tmp315;_tmp315.f1=ctxt;_tmp315.f2=work;_tmp315;});struct _tuple1 _tmp309=_tmp308;struct Cyc_Tcpat_Con_s*_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_List_List*_tmp30D;struct Cyc_List_List*_tmp30E;if(_tmp309.f1 == 0){if(_tmp309.f2 == 0){_LL1D3: _LL1D4:
 return dsc;}else{_LL1D5: _LL1D6:
 goto _LL1D8;}}else{if(_tmp309.f2 == 0){_LL1D7: _LL1D8:
({void*_tmp30F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp310="build_desc: ctxt and work don't match";_tag_dyneither(_tmp310,sizeof(char),38);}),_tag_dyneither(_tmp30F,sizeof(void*),0));});}else{_LL1D9: _tmp30A=((struct _tuple21*)(_tmp309.f1)->hd)->f1;_tmp30B=((struct _tuple21*)(_tmp309.f1)->hd)->f2;_tmp30C=(_tmp309.f1)->tl;_tmp30D=((struct _tuple20*)(_tmp309.f2)->hd)->f3;_tmp30E=(_tmp309.f2)->tl;_LL1DA: {
# 1481
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp311=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312[0]=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp313;_tmp313.tag=0;_tmp313.f1=_tmp30A;_tmp313.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp30B),({struct Cyc_List_List*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->hd=dsc;_tmp314->tl=_tmp30D;_tmp314;}));_tmp313;});_tmp312;});
return Cyc_Tcpat_build_desc(_tmp30C,(void*)_tmp311,_tmp30E);}}}_LL1D2:;}
# 1486
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1493
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp316=allmrules;void*_tmp317;struct Cyc_Tcpat_Rhs*_tmp318;struct Cyc_List_List*_tmp319;if(_tmp316 == 0){_LL1DC: _LL1DD:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A[0]=({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp31B;_tmp31B.tag=0;_tmp31B.f1=dsc;_tmp31B;});_tmp31A;});}else{_LL1DE: _tmp317=((struct _tuple22*)_tmp316->hd)->f1;_tmp318=((struct _tuple22*)_tmp316->hd)->f2;_tmp319=_tmp316->tl;_LL1DF:
# 1497
 return Cyc_Tcpat_match(_tmp317,0,dsc,0,0,_tmp318,_tmp319);}_LL1DB:;}
# 1502
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return Cyc_Tcpat_or_match((void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp31D;_tmp31D.tag=1;_tmp31D.f1=Cyc_Tcpat_empty_con_set();_tmp31D;});_tmp31C;}),allmrules);}
# 1509
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1512
struct Cyc_List_List*_tmp31E=work;struct Cyc_List_List*_tmp31F;struct Cyc_List_List*_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List*_tmp322;struct Cyc_List_List*_tmp323;if(_tmp31E == 0){_LL1E1: _LL1E2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp325;_tmp325.tag=1;_tmp325.f1=right_hand_side;_tmp325;});_tmp324;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f3 == 0){_LL1E3: _tmp323=_tmp31E->tl;_LL1E4:
# 1515
 return Cyc_Tcpat_and_match(Cyc_Tcpat_norm_context(ctx),_tmp323,right_hand_side,rules);}else{goto _LL1E5;}}else{goto _LL1E5;}}else{_LL1E5: _tmp31F=((struct _tuple20*)_tmp31E->hd)->f1;_tmp320=((struct _tuple20*)_tmp31E->hd)->f2;_tmp321=((struct _tuple20*)_tmp31E->hd)->f3;_tmp322=_tmp31E->tl;_LL1E6:
# 1517
 if((_tmp31F == 0  || _tmp320 == 0) || _tmp321 == 0)
({void*_tmp326=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp327="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp327,sizeof(char),38);}),_tag_dyneither(_tmp326,sizeof(void*),0));});{
struct Cyc_List_List*_tmp328=_tmp31F;struct Cyc_List_List*_tmp329=_tmp328;void*_tmp32A;struct Cyc_List_List*_tmp32B;_LL1E8: _tmp32A=(void*)_tmp329->hd;_tmp32B=_tmp329->tl;_LL1E9:;{
struct Cyc_List_List*_tmp32C=_tmp320;struct Cyc_List_List*_tmp32D=_tmp32C;struct Cyc_List_List*_tmp32E;struct Cyc_List_List*_tmp32F;_LL1EB: _tmp32E=(struct Cyc_List_List*)_tmp32D->hd;_tmp32F=_tmp32D->tl;_LL1EC:;{
struct Cyc_List_List*_tmp330=_tmp321;struct Cyc_List_List*_tmp331=_tmp330;void*_tmp332;struct Cyc_List_List*_tmp333;_LL1EE: _tmp332=(void*)_tmp331->hd;_tmp333=_tmp331->tl;_LL1EF:;{
struct _tuple20*_tmp334=({struct _tuple20*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->f1=_tmp32B;_tmp336->f2=_tmp32F;_tmp336->f3=_tmp333;_tmp336;});
return Cyc_Tcpat_match(_tmp32A,_tmp32E,_tmp332,ctx,({struct Cyc_List_List*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335->hd=_tmp334;_tmp335->tl=_tmp322;_tmp335;}),right_hand_side,rules);};};};};}}_LL1E0:;}
# 1528
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp337=dsc;struct Cyc_List_List*_tmp338;struct Cyc_Set_Set*_tmp339;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp337)->tag == 1){_LL1F1: _tmp339=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp337)->f1;_LL1F2: {
# 1534
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp33D;_tmp33D.tag=1;_tmp33D.f1=Cyc_Tcpat_empty_con_set();_tmp33D;});_tmp33C;});
struct Cyc_List_List*_tmp33A=0;
{int i=0;for(0;i < pcon->arity;++ i){
_tmp33A=({struct Cyc_List_List*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->hd=any;_tmp33B->tl=_tmp33A;_tmp33B;});}}
return _tmp33A;}}else{_LL1F3: _tmp338=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp337)->f2;_LL1F4:
 return _tmp338;}_LL1F0:;}
# 1543
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp33E=pw;struct Cyc_Absyn_Pat*_tmp33F;if((_tmp33E.where_clause).tag == 2){_LL1F6: _LL1F7:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL1F8: _tmp33F=(_tmp33E.pattern).val;_LL1F9: {
# 1547
void*_tmp340=_tmp33F->r;void*_tmp341=_tmp340;union Cyc_Absyn_AggrInfoU _tmp342;struct Cyc_List_List*_tmp343;struct Cyc_Absyn_Datatypedecl*_tmp344;struct Cyc_Absyn_Datatypefield*_tmp345;switch(*((int*)_tmp341)){case 6: _LL1FB: _LL1FC:
# 1549
 if(i != 0)
({void*_tmp346=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp349;_tmp349.tag=1;_tmp349.f1=(unsigned long)i;({void*_tmp347[1]={& _tmp349};Cyc_aprintf(({const char*_tmp348="get_access on pointer pattern with offset %d\n";_tag_dyneither(_tmp348,sizeof(char),46);}),_tag_dyneither(_tmp347,sizeof(void*),1));});}),_tag_dyneither(_tmp346,sizeof(void*),0));});
return(void*)& Cyc_Tcpat_Deref_val;case 5: _LL1FD: _LL1FE:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp34A=_cycalloc_atomic(sizeof(*_tmp34A));_tmp34A[0]=({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp34B;_tmp34B.tag=2;_tmp34B.f1=(unsigned int)i;_tmp34B;});_tmp34A;});case 8: _LL1FF: _tmp344=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp341)->f1;_tmp345=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp341)->f2;_LL200:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp34D;_tmp34D.tag=3;_tmp34D.f1=_tmp344;_tmp34D.f2=_tmp345;_tmp34D.f3=(unsigned int)i;_tmp34D;});_tmp34C;});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp341)->f1 != 0){_LL201: _tmp342=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp341)->f1)->aggr_info;_tmp343=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp341)->f3;_LL202: {
# 1555
struct Cyc_Absyn_Aggrdecl*_tmp34E=Cyc_Absyn_get_known_aggrdecl(_tmp342);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->tagged){
struct Cyc_List_List*_tmp34F=(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp343))->hd)).f1;struct Cyc_List_List*_tmp350=_tmp34F;struct _dyneither_ptr*_tmp351;if(_tmp350 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp350)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp350)->tl == 0){_LL206: _tmp351=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp350->hd)->f1;_LL207:
# 1559
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp353;_tmp353.tag=4;_tmp353.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->tagged;_tmp353.f2=_tmp351;_tmp353;});_tmp352;});}else{goto _LL208;}}else{goto _LL208;}}else{_LL208: _LL209:
({void*_tmp354=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp357;_tmp357.tag=0;_tmp357.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp33F));({void*_tmp355[1]={& _tmp357};Cyc_aprintf(({const char*_tmp356="get_access on bad aggr pattern: %s";_tag_dyneither(_tmp356,sizeof(char),35);}),_tag_dyneither(_tmp355,sizeof(void*),1));});}),_tag_dyneither(_tmp354,sizeof(void*),0));});}_LL205:;}{
# 1563
struct Cyc_List_List*_tmp358=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->fields;
int _tmp359=i;
for(0;i != 0;-- i){_tmp358=((struct Cyc_List_List*)_check_null(_tmp358))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A[0]=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp35B;_tmp35B.tag=4;_tmp35B.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->tagged;_tmp35B.f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp358))->hd)->name;_tmp35B;});_tmp35A;});};}}else{goto _LL203;}default: _LL203: _LL204:
({void*_tmp35C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35F;_tmp35F.tag=0;_tmp35F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp33F));({void*_tmp35D[1]={& _tmp35F};Cyc_aprintf(({const char*_tmp35E="get_access on bad pattern: %s";_tag_dyneither(_tmp35E,sizeof(char),30);}),_tag_dyneither(_tmp35D,sizeof(void*),1));});}),_tag_dyneither(_tmp35C,sizeof(void*),0));});}_LL1FA:;}}_LL1F5:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1573
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp360=env;struct Cyc_List_List*_tmp361;struct Cyc_Tcpat_Con_s*_tmp362;_LL20B: _tmp361=_tmp360->f1;_tmp362=_tmp360->f2;_LL20C:;{
void*acc=Cyc_Tcpat_get_access(_tmp362->orig_pat,i);
return({struct Cyc_List_List*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363->hd=({struct Cyc_Tcpat_PathNode*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364->orig_pat=_tmp362->orig_pat;_tmp364->access=acc;_tmp364;});_tmp363->tl=_tmp361;_tmp363;});};}
# 1578
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp365=({struct _tuple23 _tmp366;_tmp366.f1=obj;_tmp366.f2=pcon;_tmp366;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp365);}
# 1586
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1590
void*_tmp367=p;struct Cyc_Tcpat_Con_s*_tmp368;struct Cyc_List_List*_tmp369;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp367)->tag == 0){_LL20E: _LL20F:
# 1592
 return Cyc_Tcpat_and_match(Cyc_Tcpat_augment(ctx,dsc),work,right_hand_side,rules);}else{_LL210: _tmp368=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp367)->f1;_tmp369=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp367)->f2;_LL211: {
# 1594
enum Cyc_Tcpat_Answer _tmp36A=Cyc_Tcpat_static_match(_tmp368,dsc);enum Cyc_Tcpat_Answer _tmp36B=_tmp36A;switch(_tmp36B){case Cyc_Tcpat_Yes: _LL213: _LL214: {
# 1596
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->hd=({struct _tuple21*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->f1=_tmp368;_tmp36F->f2=0;_tmp36F;});_tmp36E->tl=ctx;_tmp36E;});
struct _tuple20*work_frame=({struct _tuple20*_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D->f1=_tmp369;_tmp36D->f2=Cyc_Tcpat_getoargs(_tmp368,obj);_tmp36D->f3=
Cyc_Tcpat_getdargs(_tmp368,dsc);_tmp36D;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->hd=work_frame;_tmp36C->tl=work;_tmp36C;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL215: _LL216:
# 1602
 return Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,dsc,work),rules);default: _LL217: _LL218: {
# 1604
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->hd=({struct _tuple21*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->f1=_tmp368;_tmp375->f2=0;_tmp375;});_tmp374->tl=ctx;_tmp374;});
struct _tuple20*work_frame=({struct _tuple20*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->f1=_tmp369;_tmp373->f2=Cyc_Tcpat_getoargs(_tmp368,obj);_tmp373->f3=
Cyc_Tcpat_getdargs(_tmp368,dsc);_tmp373;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->hd=work_frame;_tmp372->tl=work;_tmp372;});
void*_tmp370=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp371=Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,Cyc_Tcpat_add_neg(dsc,_tmp368),work),rules);
# 1611
return Cyc_Tcpat_ifeq(obj,_tmp368,_tmp370,_tmp371);}}_LL212:;}}_LL20D:;}
# 1621
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1626
void*_tmp376=d;struct Cyc_List_List*_tmp377;void*_tmp378;struct Cyc_Tcpat_Rhs*_tmp379;void*_tmp37A;switch(*((int*)_tmp376)){case 0: _LL21A: _tmp37A=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp376)->f1;_LL21B:
# 1628
 if(!(*exhaust_done)){not_exhaust(env1,_tmp37A);*exhaust_done=1;}
goto _LL219;case 1: _LL21C: _tmp379=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp376)->f1;_LL21D:
 rhs_appears(env2,_tmp379);goto _LL219;default: _LL21E: _tmp377=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp376)->f2;_tmp378=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp376)->f3;_LL21F:
# 1632
 for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
struct _tuple0 _tmp37B=*((struct _tuple0*)_tmp377->hd);struct _tuple0 _tmp37C=_tmp37B;void*_tmp37D;_LL221: _tmp37D=_tmp37C.f2;_LL222:;
Cyc_Tcpat_check_exhaust_overlap(_tmp37D,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1637
Cyc_Tcpat_check_exhaust_overlap(_tmp378,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1639
goto _LL219;}_LL219:;}
# 1647
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1649
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->used=0;_tmp386->pat_loc=(swc->pattern)->loc;_tmp386->rhs=swc->body;_tmp386;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp37E=({union Cyc_Tcpat_PatOrWhere _tmp380;(_tmp380.where_clause).val=swc->where_clause;(_tmp380.where_clause).tag=2;_tmp380;});
sp=Cyc_Tcpat_tuple_pat(({void*_tmp37F[2];_tmp37F[1]=Cyc_Tcpat_int_pat(*ctr,_tmp37E);_tmp37F[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp37F,sizeof(void*),2));}),_tmp37E);
*ctr=*ctr + 1;}else{
# 1657
sp=Cyc_Tcpat_tuple_pat(({void*_tmp381[2];_tmp381[1]=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp382=_cycalloc_atomic(sizeof(*_tmp382));_tmp382[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp383;_tmp383.tag=0;_tmp383;});_tmp382;});_tmp381[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp381,sizeof(void*),2));}),({union Cyc_Tcpat_PatOrWhere _tmp384;(_tmp384.where_clause).val=0;(_tmp384.where_clause).tag=2;_tmp384;}));}
return({struct _tuple22*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->f1=sp;_tmp385->f2=rhs;_tmp385;});}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1663
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1665
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp388=d;struct Cyc_Set_Set*_tmp389;struct Cyc_Tcpat_Con_s*_tmp38A;struct Cyc_List_List*_tmp38B;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->tag == 0){_LL224: _tmp38A=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->f1;_tmp38B=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->f2;_LL225: {
# 1669
union Cyc_Tcpat_Name_value _tmp38C=_tmp38A->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp38D=_tmp38A->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp38E=_tmp38D;struct Cyc_Absyn_Pat*_tmp38F;if((_tmp38E.where_clause).tag == 2){_LL229: _LL22A:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp38B))->hd);}else{_LL22B: _tmp38F=(_tmp38E.pattern).val;_LL22C:
 p=_tmp38F;goto _LL228;}_LL228:;}{
# 1675
void*_tmp390=p->r;void*_tmp391=_tmp390;struct Cyc_Absyn_Exp*_tmp392;struct Cyc_Absyn_Enumfield*_tmp393;struct Cyc_Absyn_Enumfield*_tmp394;struct _dyneither_ptr _tmp395;int _tmp396;char _tmp397;int _tmp398;struct Cyc_Absyn_Datatypefield*_tmp399;struct Cyc_Absyn_Aggrdecl*_tmp39A;struct Cyc_List_List*_tmp39B;struct Cyc_Absyn_Tvar*_tmp39C;struct Cyc_Absyn_Vardecl*_tmp39D;struct Cyc_Absyn_Vardecl*_tmp39E;struct Cyc_Absyn_Vardecl*_tmp39F;switch(*((int*)_tmp391)){case 0: _LL22E: _LL22F:
 return({const char*_tmp3A0="_";_tag_dyneither(_tmp3A0,sizeof(char),2);});case 1: _LL230: _tmp39F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL231:
 return Cyc_Absynpp_qvar2string(_tmp39F->name);case 3: _LL232: _tmp39E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL233:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A3;_tmp3A3.tag=0;_tmp3A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp39E->name));({void*_tmp3A1[1]={& _tmp3A3};Cyc_aprintf(({const char*_tmp3A2="*%s";_tag_dyneither(_tmp3A2,sizeof(char),4);}),_tag_dyneither(_tmp3A1,sizeof(void*),1));});});case 4: _LL234: _tmp39C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_tmp39D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL235:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39C->name);({struct Cyc_String_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=0;_tmp3A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp39D->name));({void*_tmp3A4[2]={& _tmp3A6,& _tmp3A7};Cyc_aprintf(({const char*_tmp3A5="%s<`%s>";_tag_dyneither(_tmp3A5,sizeof(char),8);}),_tag_dyneither(_tmp3A4,sizeof(void*),2));});});});case 5: _LL236: _LL237:
# 1681
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B));({void*_tmp3A8[1]={& _tmp3AA};Cyc_aprintf(({const char*_tmp3A9="$(%s)";_tag_dyneither(_tmp3A9,sizeof(char),6);}),_tag_dyneither(_tmp3A8,sizeof(void*),1));});});case 6: _LL238: _LL239:
# 1683
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0;_tmp3AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B));({void*_tmp3AB[1]={& _tmp3AD};Cyc_aprintf(({const char*_tmp3AC="&%s";_tag_dyneither(_tmp3AC,sizeof(char),4);}),_tag_dyneither(_tmp3AB,sizeof(void*),1));});});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f1)->aggr_info).KnownAggr).tag == 2){_LL23A: _tmp39A=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f1)->aggr_info).KnownAggr).val;_tmp39B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f3;_LL23B:
# 1685
 if(_tmp39A->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp3AE=_tmp39B;struct _dyneither_ptr*_tmp3AF;if(_tmp3AE != 0){if(((struct _tuple15*)((struct Cyc_List_List*)_tmp3AE)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp3AE)->hd)->f1)->hd)->tag == 1){_LL24F: _tmp3AF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple15*)_tmp3AE->hd)->f1)->hd)->f1;_LL250:
# 1688
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0;_tmp3B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(_tmp38B));({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0;_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AF);({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0;_tmp3B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1688
Cyc_Absynpp_qvar2string(_tmp39A->name));({void*_tmp3B0[3]={& _tmp3B2,& _tmp3B3,& _tmp3B4};Cyc_aprintf(({const char*_tmp3B1="%s{.%s = %s}";_tag_dyneither(_tmp3B1,sizeof(char),13);}),_tag_dyneither(_tmp3B0,sizeof(void*),3));});});});});}else{goto _LL251;}}else{goto _LL251;}}else{_LL251: _LL252:
# 1690
 goto _LL24E;}_LL24E:;}
# 1693
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0;_tmp3B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B));({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0;_tmp3B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp39A->name));({void*_tmp3B5[2]={& _tmp3B7,& _tmp3B8};Cyc_aprintf(({const char*_tmp3B6="%s{%s}";_tag_dyneither(_tmp3B6,sizeof(char),7);}),_tag_dyneither(_tmp3B5,sizeof(void*),2));});});});}else{goto _LL24C;}}else{goto _LL24C;}case 8: _LL23C: _tmp399=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL23D:
# 1695
 if(_tmp38B == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0;_tmp3BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp399->name));({void*_tmp3B9[1]={& _tmp3BB};Cyc_aprintf(({const char*_tmp3BA="%s";_tag_dyneither(_tmp3BA,sizeof(char),3);}),_tag_dyneither(_tmp3B9,sizeof(void*),1));});});else{
# 1698
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0;_tmp3BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B));({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0;_tmp3BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp399->name));({void*_tmp3BC[2]={& _tmp3BE,& _tmp3BF};Cyc_aprintf(({const char*_tmp3BD="%s(%s)";_tag_dyneither(_tmp3BD,sizeof(char),7);}),_tag_dyneither(_tmp3BC,sizeof(void*),2));});});});}case 9: _LL23E: _LL23F:
 return({const char*_tmp3C0="NULL";_tag_dyneither(_tmp3C0,sizeof(char),5);});case 10: _LL240: _tmp398=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL241:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=1;_tmp3C3.f1=(unsigned long)_tmp398;({void*_tmp3C1[1]={& _tmp3C3};Cyc_aprintf(({const char*_tmp3C2="%d";_tag_dyneither(_tmp3C2,sizeof(char),3);}),_tag_dyneither(_tmp3C1,sizeof(void*),1));});});case 11: _LL242: _tmp397=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL243:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=1;_tmp3C6.f1=(unsigned long)((int)_tmp397);({void*_tmp3C4[1]={& _tmp3C6};Cyc_aprintf(({const char*_tmp3C5="%d";_tag_dyneither(_tmp3C5,sizeof(char),3);}),_tag_dyneither(_tmp3C4,sizeof(void*),1));});});case 12: _LL244: _tmp395=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_tmp396=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL245:
 return _tmp395;case 13: _LL246: _tmp394=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL247:
 _tmp393=_tmp394;goto _LL249;case 14: _LL248: _tmp393=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL249:
 return Cyc_Absynpp_qvar2string(_tmp393->name);case 17: _LL24A: _tmp392=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL24B:
 return Cyc_Absynpp_exp2string(_tmp392);default: _LL24C: _LL24D:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL22D:;};}}else{_LL226: _tmp389=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp388)->f1;_LL227:
# 1709
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp389))return({const char*_tmp3C7="_";_tag_dyneither(_tmp3C7,sizeof(char),2);});{
struct Cyc_Tcpat_Con_s*_tmp3C8=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp389);
# 1713
if((int)(((_tmp3C8->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp3C9=({union Cyc_Tcpat_PatOrWhere _tmp3E9=_tmp3C8->orig_pat;if((_tmp3E9.pattern).tag != 1)_throw_match();(_tmp3E9.pattern).val;});
void*_tmp3CA=Cyc_Tcutil_compress((void*)_check_null(_tmp3C9->topt));void*_tmp3CB=_tmp3CA;struct Cyc_Absyn_Aggrdecl*_tmp3CC;struct Cyc_Absyn_Datatypedecl*_tmp3CD;struct Cyc_Absyn_PtrAtts _tmp3CE;switch(*((int*)_tmp3CB)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3CB)->f2 == Cyc_Absyn_Char_sz){_LL254: _LL255:
# 1718
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp3CF=Cyc_Tcpat_char_con((char)i,_tmp3C9);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,_tmp3CF))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=1;_tmp3D2.f1=(unsigned long)i;({void*_tmp3D0[1]={& _tmp3D2};Cyc_aprintf(({const char*_tmp3D1="%d";_tag_dyneither(_tmp3D1,sizeof(char),3);}),_tag_dyneither(_tmp3D0,sizeof(void*),1));});});}}
# 1723
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL256: _LL257:
# 1726
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp3D3=Cyc_Tcpat_int_con((int)i,_tmp3C8->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,_tmp3D3))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=1;_tmp3D6.f1=(unsigned long)((int)i);({void*_tmp3D4[1]={& _tmp3D6};Cyc_aprintf(({const char*_tmp3D5="%d";_tag_dyneither(_tmp3D5,sizeof(char),3);}),_tag_dyneither(_tmp3D4,sizeof(void*),1));});});}}
# 1731
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 5: _LL258: _tmp3CE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).ptr_atts;_LL259: {
# 1733
union Cyc_Absyn_Constraint*_tmp3D7=_tmp3CE.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3D7);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,Cyc_Tcpat_null_con(_tmp3C9)))
return({const char*_tmp3D8="NULL";_tag_dyneither(_tmp3D8,sizeof(char),5);});}
# 1739
return({const char*_tmp3D9="&_";_tag_dyneither(_tmp3D9,sizeof(char),3);});}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3CB)->f1).datatype_info).KnownDatatype).tag == 2){_LL25A: _tmp3CD=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3CB)->f1).datatype_info).KnownDatatype).val;_LL25B:
# 1741
 if(_tmp3CD->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp3DA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3CD->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3DA);
for(0;(unsigned int)_tmp3DA;_tmp3DA=_tmp3DA->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp3DA->hd)->name).f2;
struct Cyc_List_List*_tmp3DB=((struct Cyc_Absyn_Datatypefield*)_tmp3DA->hd)->typs;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,({struct Cyc_Tcpat_Con_s*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->name=Cyc_Tcpat_Name_v(n);_tmp3DC->arity=0;_tmp3DC->span=0;_tmp3DC->orig_pat=_tmp3C8->orig_pat;_tmp3DC;}))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3DB)== 0)
return n;else{
# 1751
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0;_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({void*_tmp3DD[1]={& _tmp3DF};Cyc_aprintf(({const char*_tmp3DE="%s(...)";_tag_dyneither(_tmp3DE,sizeof(char),8);}),_tag_dyneither(_tmp3DD,sizeof(void*),1));});});}}}
# 1754
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL25E;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CB)->f1).aggr_info).KnownAggr).tag == 2){_LL25C: _tmp3CC=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CB)->f1).aggr_info).KnownAggr).val;if(_tmp3CC->kind == Cyc_Absyn_UnionA){_LL25D: {
# 1756
struct Cyc_List_List*_tmp3E0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3CC->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3E0);
struct _tuple2*_tmp3E1=_tmp3CC->name;struct _tuple2*_tmp3E2=_tmp3E1;struct _dyneither_ptr _tmp3E3;_LL261: _tmp3E3=*_tmp3E2->f2;_LL262:;
for(0;(unsigned int)_tmp3E0;_tmp3E0=_tmp3E0->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp3E0->hd)->name;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,({struct Cyc_Tcpat_Con_s*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->name=Cyc_Tcpat_Name_v(n);_tmp3E4->arity=0;_tmp3E4->span=0;_tmp3E4->orig_pat=_tmp3C8->orig_pat;_tmp3E4;})))
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3E8;_tmp3E8.tag=0;_tmp3E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({struct Cyc_String_pa_PrintArg_struct _tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E3);({void*_tmp3E5[2]={& _tmp3E7,& _tmp3E8};Cyc_aprintf(({const char*_tmp3E6="%s{.%s = _}";_tag_dyneither(_tmp3E6,sizeof(char),12);}),_tag_dyneither(_tmp3E5,sizeof(void*),2));});});});}
# 1764
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL25E;}}else{goto _LL25E;}default: _LL25E: _LL25F:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL253:;};};}_LL223:;}
# 1769
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dyneither_ptr*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA[0]=Cyc_Tcpat_desc2string(d);_tmp3EA;});}
# 1772
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp3EC=_tmp3EB;for(0;_tmp3EC != 0;_tmp3EC=((struct Cyc_List_List*)_check_null(_tmp3EC))->tl){
if(_tmp3EC->tl != 0){
_tmp3EC->tl=({struct Cyc_List_List*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->hd=comma;_tmp3ED->tl=_tmp3EC->tl;_tmp3ED;});
_tmp3EC=_tmp3EC->tl;}}}
# 1781
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp3EB);}
# 1784
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp3EF;_push_handler(& _tmp3EF);{int _tmp3F1=0;if(setjmp(_tmp3EF.handler))_tmp3F1=1;if(!_tmp3F1){
{struct _dyneither_ptr _tmp3F2=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3F5;_tmp3F5.tag=0;_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F2);({void*_tmp3F3[1]={& _tmp3F5};Cyc_Tcutil_terr(loc,({const char*_tmp3F4="patterns may not be exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3F4,sizeof(char),53);}),_tag_dyneither(_tmp3F3,sizeof(void*),1));});});}
# 1786
;_pop_handler();}else{void*_tmp3F0=(void*)_exn_thrown;void*_tmp3F6=_tmp3F0;void*_tmp3F7;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3F6)->tag == Cyc_Tcpat_Desc2string){_LL264: _LL265:
# 1790
({void*_tmp3F8=0;Cyc_Tcutil_terr(loc,({const char*_tmp3F9="patterns may not be exhaustive.";_tag_dyneither(_tmp3F9,sizeof(char),32);}),_tag_dyneither(_tmp3F8,sizeof(void*),0));});
goto _LL263;}else{_LL266: _tmp3F7=_tmp3F6;_LL267:(int)_rethrow(_tmp3F7);}_LL263:;}};}
# 1794
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1798
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1804
int _tmp3FA=0;
int*_tmp3FB=& _tmp3FA;
struct Cyc_List_List*_tmp3FC=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp3FB,swcs);
void*_tmp3FD=Cyc_Tcpat_match_compile(_tmp3FC);
*dopt=_tmp3FD;{
# 1810
int _tmp3FE=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3FD,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3FE);
# 1813
for(0;_tmp3FC != 0;_tmp3FC=_tmp3FC->tl){
struct _tuple22*_tmp3FF=(struct _tuple22*)_tmp3FC->hd;struct _tuple22*_tmp400=_tmp3FF;int _tmp401;unsigned int _tmp402;_LL269: _tmp401=(_tmp400->f2)->used;_tmp402=(_tmp400->f2)->pat_loc;_LL26A:;
if(!_tmp401){
({void*_tmp403=0;Cyc_Tcutil_terr(_tmp402,({const char*_tmp404="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp404,sizeof(char),74);}),_tag_dyneither(_tmp403,sizeof(void*),0));});
# 1818
break;}}};}
# 1827
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp405;_push_handler(& _tmp405);{int _tmp407=0;if(setjmp(_tmp405.handler))_tmp407=1;if(!_tmp407){
{struct _dyneither_ptr _tmp408=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0;_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp408);({void*_tmp409[1]={& _tmp40B};Cyc_Tcutil_warn((*pr).f1,({const char*_tmp40A="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp40A,sizeof(char),45);}),_tag_dyneither(_tmp409,sizeof(void*),1));});});}
# 1830
;_pop_handler();}else{void*_tmp406=(void*)_exn_thrown;void*_tmp40C=_tmp406;void*_tmp40D;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp40C)->tag == Cyc_Tcpat_Desc2string){_LL26C: _LL26D:
# 1834
({void*_tmp40E=0;Cyc_Tcutil_warn((*pr).f1,({const char*_tmp40F="pattern not exhaustive.";_tag_dyneither(_tmp40F,sizeof(char),24);}),_tag_dyneither(_tmp40E,sizeof(void*),0));});
goto _LL26B;}else{_LL26E: _tmp40D=_tmp40C;_LL26F:(int)_rethrow(_tmp40D);}_LL26B:;}};};}
# 1838
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1842
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->used=0;_tmp417->pat_loc=p->loc;_tmp417->rhs=Cyc_Absyn_skip_stmt(0);_tmp417;});
struct Cyc_List_List*_tmp410=({struct Cyc_List_List*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->hd=({struct _tuple22*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->f1=Cyc_Tcpat_compile_pat(p);_tmp416->f2=rhs;_tmp416;});_tmp415->tl=0;_tmp415;});
void*_tmp411=Cyc_Tcpat_match_compile(_tmp410);
struct _tuple14 _tmp412=({struct _tuple14 _tmp414;_tmp414.f1=loc;_tmp414.f2=1;_tmp414;});
int _tmp413=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp411,Cyc_Tcpat_not_exhaust_warn,& _tmp412,Cyc_Tcpat_dummy_fn,0,& _tmp413);
# 1850
*dopt=_tmp411;
return _tmp412.f2;}
# 1858
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->used=0;_tmp419->pat_loc=(swc->pattern)->loc;_tmp419->rhs=swc->body;_tmp419;});
return({struct _tuple22*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->f1=sp0;_tmp418->f2=rhs;_tmp418;});}
# 1863
static void Cyc_Tcpat_not_exhaust_err2(unsigned int loc,void*d){;}
# 1866
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1870
struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp41B=Cyc_Tcpat_match_compile(_tmp41A);
*dopt=_tmp41B;{
int _tmp41C=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp41B,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp41C);
# 1876
for(0;_tmp41A != 0;_tmp41A=_tmp41A->tl){
# 1878
if(_tmp41A->tl == 0)break;{
struct _tuple22*_tmp41D=(struct _tuple22*)_tmp41A->hd;struct _tuple22*_tmp41E=_tmp41D;int _tmp41F;unsigned int _tmp420;_LL271: _tmp41F=(_tmp41E->f2)->used;_tmp420=(_tmp41E->f2)->pat_loc;_LL272:;
if(!_tmp41F){
({void*_tmp421=0;Cyc_Tcutil_terr(_tmp420,({const char*_tmp422="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp422,sizeof(char),74);}),_tag_dyneither(_tmp421,sizeof(void*),0));});
break;}};}};}
