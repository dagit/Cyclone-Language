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
{const char*_tmp425;void*_tmp424;(_tmp424=0,Cyc_Tcutil_terr(p->loc,((_tmp425="struct tag used without arguments in pattern",_tag_dyneither(_tmp425,sizeof(char),45))),_tag_dyneither(_tmp424,sizeof(void*),0)));}
_npop_handler(1);return;case 2: _LL10: _tmp1A=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp15)->f1;_tmp1B=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp15)->f2;_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp428;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp427;p->r=(void*)((_tmp427=_cycalloc(sizeof(*_tmp427)),((_tmp427[0]=((_tmp428.tag=8,((_tmp428.f1=_tmp1A,((_tmp428.f2=_tmp1B,((_tmp428.f3=0,((_tmp428.f4=0,_tmp428)))))))))),_tmp427))));}_npop_handler(1);return;case 3: _LL12: _tmp18=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp15)->f1;_tmp19=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp15)->f2;_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp42B;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp42A;p->r=(void*)((_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A[0]=((_tmp42B.tag=13,((_tmp42B.f1=_tmp18,((_tmp42B.f2=_tmp19,_tmp42B)))))),_tmp42A))));}
_npop_handler(1);return;case 4: _LL14: _tmp16=(void*)((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp15)->f2;_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp42E;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp42D;p->r=(void*)((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp42E.tag=14,((_tmp42E.f1=_tmp16,((_tmp42E.f2=_tmp17,_tmp42E)))))),_tmp42D))));}
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
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp431;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp430;p->r=(void*)((_tmp430=_cycalloc(sizeof(*_tmp430)),((_tmp430[0]=((_tmp431.tag=1,((_tmp431.f1=Cyc_Absyn_new_vardecl(_tmpF,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp431.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp431)))))),_tmp430))));}
return;}else{goto _LL22;}default: _LL22: _LL23:
{const char*_tmp434;void*_tmp433;(_tmp433=0,Cyc_Tcutil_terr(p->loc,((_tmp434="qualified variable in pattern",_tag_dyneither(_tmp434,sizeof(char),30))),_tag_dyneither(_tmp433,sizeof(void*),0)));}return;}_LL1D:;};case 16: _LL3: _tmpC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_tmpD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmpE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_LL4:
# 85
{struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){
{struct _RegionHandle _tmp2F=_new_region("r");struct _RegionHandle*r=& _tmp2F;_push_region(r);
{void*_tmp30=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmpC,0);void*_tmp31=_tmp30;struct Cyc_Absyn_Datatypedecl*_tmp32;struct Cyc_Absyn_Datatypefield*_tmp33;struct Cyc_Absyn_Aggrdecl*_tmp34;switch(*((int*)_tmp31)){case 1: _LL25: _tmp34=((struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp31)->f1;_LL26: {
# 89
struct Cyc_List_List*_tmp35=0;
for(0;_tmpD != 0;_tmpD=_tmpD->tl){
struct _tuple15*_tmp437;struct Cyc_List_List*_tmp436;_tmp35=((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436->hd=((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437->f1=0,((_tmp437->f2=(struct Cyc_Absyn_Pat*)_tmpD->hd,_tmp437)))))),((_tmp436->tl=_tmp35,_tmp436))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp441;struct Cyc_Absyn_Aggrdecl**_tmp440;struct Cyc_Absyn_AggrInfo*_tmp43F;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp43E;p->r=(void*)((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E[0]=((_tmp441.tag=7,((_tmp441.f1=((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F->aggr_info=Cyc_Absyn_KnownAggr(((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440[0]=_tmp34,_tmp440))))),((_tmp43F->targs=0,_tmp43F)))))),((_tmp441.f2=0,((_tmp441.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp35),((_tmp441.f4=_tmpE,_tmp441)))))))))),_tmp43E))));}
_npop_handler(1);return;}case 2: _LL27: _tmp32=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp31)->f1;_tmp33=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp31)->f2;_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp444;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp443;p->r=(void*)((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443[0]=((_tmp444.tag=8,((_tmp444.f1=_tmp32,((_tmp444.f2=_tmp33,((_tmp444.f3=_tmpD,((_tmp444.f4=_tmpE,_tmp444)))))))))),_tmp443))));}_npop_handler(1);return;case 3: _LL29: _LL2A:
 goto _LL2C;case 4: _LL2B: _LL2C:
# 98
{const char*_tmp447;void*_tmp446;(_tmp446=0,Cyc_Tcutil_terr(p->loc,((_tmp447="enum tag used with arguments in pattern",_tag_dyneither(_tmp447,sizeof(char),40))),_tag_dyneither(_tmp446,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;default: _LL2D: _LL2E:
 goto _LL24;}_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp40=_tmp2D;void*_tmp41;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp40)->tag == Cyc_Dict_Absent){_LL30: _LL31:
# 103
 goto _LL2F;}else{_LL32: _tmp41=_tmp40;_LL33:(int)_rethrow(_tmp41);}_LL2F:;}};}
{const char*_tmp44B;void*_tmp44A[1];struct Cyc_String_pa_PrintArg_struct _tmp449;(_tmp449.tag=0,((_tmp449.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC)),((_tmp44A[0]=& _tmp449,Cyc_Tcutil_terr(p->loc,((_tmp44B="%s is not a constructor in pattern",_tag_dyneither(_tmp44B,sizeof(char),35))),_tag_dyneither(_tmp44A,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL5: _tmp9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmpA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmpB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL6:
# 109
 if(topt == 0){
const char*_tmp44E;void*_tmp44D;(_tmp44D=0,Cyc_Tcutil_terr(p->loc,((_tmp44E="cannot determine pattern type",_tag_dyneither(_tmp44E,sizeof(char),30))),_tag_dyneither(_tmp44D,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp47=t;struct Cyc_Absyn_AggrInfo _tmp48;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->tag == 11){_LL35: _tmp48=((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1;_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp454;struct Cyc_Absyn_AggrInfo*_tmp453;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp452;p->r=(void*)((_tmp452=_cycalloc(sizeof(*_tmp452)),((_tmp452[0]=((_tmp454.tag=7,((_tmp454.f1=((_tmp453=_cycalloc(sizeof(*_tmp453)),((_tmp453[0]=_tmp48,_tmp453)))),((_tmp454.f2=_tmp9,((_tmp454.f3=_tmpA,((_tmp454.f4=_tmpB,_tmp454)))))))))),_tmp452))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;}else{_LL37: _LL38:
# 118
{const char*_tmp458;void*_tmp457[1];struct Cyc_String_pa_PrintArg_struct _tmp456;(_tmp456.tag=0,((_tmp456.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp457[0]=& _tmp456,Cyc_Tcutil_terr(p->loc,((_tmp458="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp458,sizeof(char),45))),_tag_dyneither(_tmp457,sizeof(void*),1)))))));}
# 121
goto _LL34;}_LL34:;}
# 123
return;};}else{if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).tag == 1){_LL7: _tmp3=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).val).f1;_tmp4=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).val).f2;_tmp5=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->targs;_tmp6=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL8:
# 126
{struct _handler_cons _tmp4F;_push_handler(& _tmp4F);{int _tmp51=0;if(setjmp(_tmp4F.handler))_tmp51=1;if(!_tmp51){
{struct Cyc_Absyn_Aggrdecl**_tmp52=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp4);
struct Cyc_Absyn_Aggrdecl*_tmp53=*_tmp52;
if(_tmp53->impl == 0){
{const char*_tmp460;void*_tmp45F[1];const char*_tmp45E;const char*_tmp45D;struct Cyc_String_pa_PrintArg_struct _tmp45C;(_tmp45C.tag=0,((_tmp45C.f1=(struct _dyneither_ptr)(
_tmp53->kind == Cyc_Absyn_StructA?(_tmp45D="struct",_tag_dyneither(_tmp45D,sizeof(char),7)):((_tmp45E="union",_tag_dyneither(_tmp45E,sizeof(char),6)))),((_tmp45F[0]=& _tmp45C,Cyc_Tcutil_terr(p->loc,((_tmp460="can't destructure an abstract %s",_tag_dyneither(_tmp460,sizeof(char),33))),_tag_dyneither(_tmp45F,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp6);
if(more_exists < 0){
{const char*_tmp463;void*_tmp462;(_tmp462=0,Cyc_Tcutil_terr(p->loc,((_tmp463="too many existentially bound type variables in pattern",_tag_dyneither(_tmp463,sizeof(char),55))),_tag_dyneither(_tmp462,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp5B=_tmp6;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53->impl))->exist_vars);for(0;n != 0;-- n){
_tmp5B=&((struct Cyc_List_List*)_check_null(*_tmp5B))->tl;}}
*_tmp5B=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp5C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp469;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp468;struct Cyc_List_List*_tmp467;_tmp5C=((_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467->hd=Cyc_Tcutil_new_tvar((void*)((_tmp469=_cycalloc(sizeof(*_tmp469)),((_tmp469[0]=((_tmp468.tag=1,((_tmp468.f1=0,_tmp468)))),_tmp469))))),((_tmp467->tl=_tmp5C,_tmp467))))));}
*_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp6,_tmp5C);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp46F;struct Cyc_Absyn_AggrInfo*_tmp46E;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp46D;p->r=(void*)((_tmp46D=_cycalloc(sizeof(*_tmp46D)),((_tmp46D[0]=((_tmp46F.tag=7,((_tmp46F.f1=((_tmp46E=_cycalloc(sizeof(*_tmp46E)),((_tmp46E->aggr_info=Cyc_Absyn_KnownAggr(_tmp52),((_tmp46E->targs=_tmp5,_tmp46E)))))),((_tmp46F.f2=*_tmp6,((_tmp46F.f3=_tmp7,((_tmp46F.f4=_tmp8,_tmp46F)))))))))),_tmp46D))));};};}
# 127
;_pop_handler();}else{void*_tmp50=(void*)_exn_thrown;void*_tmp63=_tmp50;void*_tmp64;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp63)->tag == Cyc_Dict_Absent){_LL3A: _LL3B:
# 152
{const char*_tmp472;void*_tmp471;(_tmp471=0,Cyc_Tcutil_terr(p->loc,((_tmp472="Non-aggregate name has designator patterns",_tag_dyneither(_tmp472,sizeof(char),43))),_tag_dyneither(_tmp471,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;}else{_LL3C: _tmp64=_tmp63;_LL3D:(int)_rethrow(_tmp64);}_LL39:;}};}
# 156
return;}else{goto _LLB;}}case 17: _LL9: _tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp2);
if(!Cyc_Tcutil_is_const_exp(_tmp2)){
{const char*_tmp475;void*_tmp474;(_tmp474=0,Cyc_Tcutil_terr(p->loc,((_tmp475="non-constant expression in case pattern",_tag_dyneither(_tmp475,sizeof(char),40))),_tag_dyneither(_tmp474,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _tmp69=Cyc_Evexp_eval_const_uint_exp(_tmp2);struct _tuple14 _tmp6A=_tmp69;unsigned int _tmp6B;int _tmp6C;_LL3F: _tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;_LL40:
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp478;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp477;p->r=(void*)((_tmp477=_cycalloc_atomic(sizeof(*_tmp477)),((_tmp477[0]=((_tmp478.tag=10,((_tmp478.f1=Cyc_Absyn_None,((_tmp478.f2=(int)_tmp6B,_tmp478)))))),_tmp477))));}
return;_LL3E:;};default: _LLB: _LLC:
 return;}_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp479;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479->v=s,_tmp479)))));};}
# 178
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 182
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 185
{void*_tmp70=Cyc_Tcutil_compress(numt);void*_tmp71=_tmp70;switch(*((int*)_tmp71)){case 13: _LL44: _LL45:
 goto _LL47;case 14: _LL46: _LL47:
 if(topt != 0)return*topt;goto _LL43;default: _LL48: _LL49:
 goto _LL43;}_LL43:;}
# 190
return numt;}struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 193
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 195
if(vd != 0){
(*vd)->type=t;
(*vd)->tq=Cyc_Absyn_empty_tqual(0);}{
# 199
struct _tuple16*_tmp47C;struct Cyc_List_List*_tmp47B;*v_result_ptr=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B->hd=((_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->f1=vd,((_tmp47C->f2=e,_tmp47C)))))),((_tmp47B->tl=*v_result_ptr,_tmp47B))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct Cyc_Tcpat_TcPatResult _tmp74=res1;struct _tuple1*_tmp75;struct Cyc_List_List*_tmp76;_LL4B: _tmp75=_tmp74.tvars_and_bounds_opt;_tmp76=_tmp74.patvars;_LL4C: {
struct Cyc_Tcpat_TcPatResult _tmp77=res2;struct _tuple1*_tmp78;struct Cyc_List_List*_tmp79;_LL50: _tmp78=_tmp77.tvars_and_bounds_opt;_tmp79=_tmp77.patvars;_LL51:
 if(_tmp75 != 0  || _tmp78 != 0){
if(_tmp75 == 0){
struct _tuple1*_tmp47D;_tmp75=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->f1=0,((_tmp47D->f2=0,_tmp47D))))));}
if(_tmp78 == 0){
struct _tuple1*_tmp47E;_tmp78=((_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->f1=0,((_tmp47E->f2=0,_tmp47E))))));}{
struct _tuple1*_tmp481;struct Cyc_Tcpat_TcPatResult _tmp480;return(_tmp480.tvars_and_bounds_opt=((_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp75).f1,(*_tmp78).f1),((_tmp481->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp75).f2,(*_tmp78).f2),_tmp481)))))),((_tmp480.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp76,_tmp79),_tmp480)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp482;return(_tmp482.tvars_and_bounds_opt=0,((_tmp482.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp76,_tmp79),_tmp482)));};_LL4F:;}_LL4A:;}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp483;return(_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483->loc=loc,((_tmp483->topt=0,((_tmp483->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp483)))))));}
# 222
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
struct Cyc_Core_Opt*_tmp484;return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484->v=Cyc_Tcenv_lookup_type_vars(te),_tmp484))))): rgn_opt,0): t;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};
# 229
static struct _tuple17*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
struct _tuple17*_tmp485;return(_tmp485=_cycalloc(sizeof(*_tmp485)),((_tmp485->f1=tv,((_tmp485->f2=0,_tmp485)))));}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 233
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 236
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp486;struct Cyc_Tcpat_TcPatResult res=(_tmp486.tvars_and_bounds_opt=0,((_tmp486.patvars=0,_tmp486)));
# 241
{void*_tmp82=p->r;void*_tmp83=_tmp82;struct Cyc_Absyn_Datatypedecl*_tmp84;struct Cyc_Absyn_Datatypefield*_tmp85;struct Cyc_List_List**_tmp86;int _tmp87;struct Cyc_Absyn_Aggrdecl*_tmp88;struct Cyc_List_List**_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List**_tmp8B;int _tmp8C;struct Cyc_List_List**_tmp8D;int _tmp8E;struct Cyc_Absyn_Pat*_tmp8F;void*_tmp90;struct Cyc_Absyn_Enumdecl*_tmp91;int _tmp92;struct Cyc_Absyn_Tvar*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*_tmp95;struct Cyc_Absyn_Pat*_tmp96;struct Cyc_Absyn_Tvar*_tmp97;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Vardecl*_tmp99;struct Cyc_Absyn_Pat*_tmp9A;switch(*((int*)_tmp83)){case 0: _LL55: _LL56:
# 244
 if(topt != 0)
t=*topt;else{
# 247
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL54;case 1: _LL57: _tmp99=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp9A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL58: {
# 251
struct _tuple2*_tmp9B=_tmp99->name;struct _tuple2*_tmp9C=_tmp9B;struct _dyneither_ptr _tmp9D;_LL82: _tmp9D=*_tmp9C->f2;_LL83:
{const char*_tmp488;const char*_tmp487;if(Cyc_strcmp((struct _dyneither_ptr)_tmp9D,((_tmp488="true",_tag_dyneither(_tmp488,sizeof(char),5))))== 0  || Cyc_strcmp((struct _dyneither_ptr)_tmp9D,((_tmp487="false",_tag_dyneither(_tmp487,sizeof(char),6))))== 0){
const char*_tmp48C;void*_tmp48B[1];struct Cyc_String_pa_PrintArg_struct _tmp48A;(_tmp48A.tag=0,((_tmp48A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9D),((_tmp48B[0]=& _tmp48A,Cyc_Tcutil_warn(p->loc,((_tmp48C="you probably do not want to use %s as a local variable...",_tag_dyneither(_tmp48C,sizeof(char),58))),_tag_dyneither(_tmp48B,sizeof(void*),1)))))));}}
# 255
res=Cyc_Tcpat_tcPatRec(te,_tmp9A,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp9A->topt);
# 259
{void*_tmpA3=Cyc_Tcutil_compress(t);void*_tmpA4=_tmpA3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA4)->tag == 8){_LL87: _LL88:
# 261
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp48F;void*_tmp48E;(_tmp48E=0,Cyc_Tcutil_terr(p->loc,((_tmp48F="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp48F,sizeof(char),58))),_tag_dyneither(_tmp48E,sizeof(void*),0)));}
goto _LL86;}else{_LL89: _LL8A:
# 265
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp492;void*_tmp491;(_tmp491=0,Cyc_Tcutil_terr(p->loc,((_tmp492="pattern would point to an abstract member",_tag_dyneither(_tmp492,sizeof(char),42))),_tag_dyneither(_tmp491,sizeof(void*),0)));}
goto _LL86;}_LL86:;}
# 269
{struct Cyc_Absyn_Vardecl**_tmp493;Cyc_Tcpat_set_vd(((_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493[0]=_tmp99,_tmp493)))),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));}
goto _LL54;_LL81:;}case 2: _LL59: _tmp97=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp98=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL5A: {
# 272
struct _RegionHandle _tmpAA=_new_region("r2");struct _RegionHandle*r2=& _tmpAA;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp494[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp494[0]=_tmp97,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r2,_tag_dyneither(_tmp494,sizeof(struct Cyc_Absyn_Tvar*),1)))));
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp495;res.tvars_and_bounds_opt=((_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495->f1=0,((_tmp495->f2=0,_tmp495))))));}
{struct _tuple17*_tmp498;struct Cyc_List_List*_tmp497;(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((_tmp497=_cycalloc(sizeof(*_tmp497)),((_tmp497->hd=((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498->f1=_tmp97,((_tmp498->f2=1,_tmp498)))))),((_tmp497->tl=0,_tmp497)))))));}
Cyc_Tcutil_check_type(p->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,0,_tmp98->type);}
# 282
if(topt != 0)t=*topt;else{
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
{void*_tmpAF=Cyc_Tcutil_compress(t);void*_tmpB0=_tmpAF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0)->tag == 8){_LL8C: _LL8D:
# 286
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp49B;void*_tmp49A;(_tmp49A=0,Cyc_Tcutil_terr(p->loc,((_tmp49B="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp49B,sizeof(char),58))),_tag_dyneither(_tmp49A,sizeof(void*),0)));}
goto _LL8B;}else{_LL8E: _LL8F:
# 290
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp49E;void*_tmp49D;(_tmp49D=0,Cyc_Tcutil_terr(p->loc,((_tmp49E="pattern would point to an abstract member",_tag_dyneither(_tmp49E,sizeof(char),42))),_tag_dyneither(_tmp49D,sizeof(void*),0)));}
goto _LL8B;}_LL8B:;}
# 294
{struct Cyc_Absyn_Vardecl**_tmp49F;Cyc_Tcpat_set_vd(((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F[0]=_tmp98,_tmp49F)))),access_exp,& res.patvars,_tmp98->type);}
_npop_handler(0);goto _LL54;
# 272
;_pop_region(r2);}case 3: _LL5B: _tmp95=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp96=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL5C:
# 298
 res=Cyc_Tcpat_tcPatRec(te,_tmp96,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp96->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp4A2;void*_tmp4A1;(_tmp4A1=0,Cyc_Tcutil_terr(p->loc,((_tmp4A2="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp4A2,sizeof(char),55))),_tag_dyneither(_tmp4A1,sizeof(void*),0)));}
goto _LL54;}else{
# 305
struct _RegionHandle _tmpB8=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB8;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp4A5;void*_tmp4A4;(_tmp4A4=0,Cyc_Tcutil_terr(p->loc,((_tmp4A5="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp4A5,sizeof(char),56))),_tag_dyneither(_tmp4A4,sizeof(void*),0)));}
# 306
;_pop_region(rgn);}{
# 311
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4AB;struct Cyc_Absyn_PtrInfo _tmp4AA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A9;void*t2=(void*)((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4AB.tag=5,((_tmp4AB.f1=((_tmp4AA.elt_typ=t,((_tmp4AA.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp4AA.ptr_atts=(
((_tmp4AA.ptr_atts).rgn=rgn_pat,(((_tmp4AA.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp4AA.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4AA.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4AA.ptr_atts).ptrloc=0,_tmp4AA.ptr_atts)))))))))),_tmp4AA)))))),_tmp4AB)))),_tmp4A9))));
# 316
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 320
{struct Cyc_Absyn_Vardecl**_tmp4AC;Cyc_Tcpat_set_vd(((_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC[0]=_tmp95,_tmp4AC)))),new_access_exp,& res.patvars,t2);}
goto _LL54;};case 4: _LL5D: _tmp93=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp94=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL5E:
# 326
{struct Cyc_Absyn_Vardecl**_tmp4AD;Cyc_Tcpat_set_vd(((_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD[0]=_tmp94,_tmp4AD)))),access_exp,& res.patvars,_tmp94->type);}{
# 330
struct _RegionHandle _tmpC0=_new_region("r2");struct _RegionHandle*r2=& _tmpC0;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp4AE[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp4AE[0]=_tmp93,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4AE,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 333
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp4AF;res.tvars_and_bounds_opt=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF->f1=0,((_tmp4AF->f2=0,_tmp4AF))))));}
{struct _tuple17*_tmp4B2;struct Cyc_List_List*_tmp4B1;(*res.tvars_and_bounds_opt).f1=(
(_tmp4B1=_cycalloc(sizeof(*_tmp4B1)),((_tmp4B1->hd=((_tmp4B2=_cycalloc(sizeof(*_tmp4B2)),((_tmp4B2->f1=_tmp93,((_tmp4B2->f2=0,_tmp4B2)))))),((_tmp4B1->tl=(*res.tvars_and_bounds_opt).f1,_tmp4B1))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL54;
# 330
;_pop_region(r2);};case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp83)->f1){case Cyc_Absyn_Unsigned: _LL5F: _LL60:
# 340
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL54;case Cyc_Absyn_None: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL54;}case 11: _LL65: _LL66:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL54;case 12: _LL67: _tmp92=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL68:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmp92));goto _LL54;case 13: _LL69: _tmp91=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL6A:
# 346
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp4B5;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4B4;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((_tmp4B4[0]=((_tmp4B5.tag=13,((_tmp4B5.f1=_tmp91->name,((_tmp4B5.f2=_tmp91,_tmp4B5)))))),_tmp4B4)))));}
goto _LL54;case 14: _LL6B: _tmp90=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL6C:
 t=Cyc_Tcpat_num_type(topt,_tmp90);goto _LL54;case 9: _LL6D: _LL6E:
# 350
 if(topt != 0){
void*_tmpC7=Cyc_Tcutil_compress(*topt);void*_tmpC8=_tmpC7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8)->tag == 5){_LL91: _LL92:
 t=*topt;goto tcpat_end;}else{_LL93: _LL94:
 goto _LL90;}_LL90:;}{
# 355
struct Cyc_Core_Opt*_tmpC9=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4BB;struct Cyc_Absyn_PtrInfo _tmp4BA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B9;t=(void*)((_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9[0]=((_tmp4BB.tag=5,((_tmp4BB.f1=((_tmp4BA.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpC9),((_tmp4BA.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp4BA.ptr_atts=(
((_tmp4BA.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpC9),(((_tmp4BA.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp4BA.ptr_atts).bounds=
# 360
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4BA.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4BA.ptr_atts).ptrloc=0,_tmp4BA.ptr_atts)))))))))),_tmp4BA)))))),_tmp4BB)))),_tmp4B9))));}
# 363
goto _LL54;};case 6: _LL6F: _tmp8F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL70: {
# 368
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpCD=0;
int elt_const=0;
if(topt != 0){
void*_tmpCE=Cyc_Tcutil_compress(*topt);void*_tmpCF=_tmpCE;void*_tmpD0;struct Cyc_Absyn_Tqual _tmpD1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCF)->tag == 5){_LL96: _tmpD0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCF)->f1).elt_typ;_tmpD1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCF)->f1).elt_tq;_LL97:
# 374
 inner_typ=_tmpD0;
_tmpCD=& inner_typ;
elt_const=_tmpD1.real_const;
goto _LL95;}else{_LL98: _LL99:
 goto _LL95;}_LL95:;}{
# 383
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp8F,_tmpCD,ptr_rgn,1,new_access_exp));
# 392
{void*_tmpD2=Cyc_Tcutil_compress((void*)_check_null(_tmp8F->topt));void*_tmpD3=_tmpD2;struct Cyc_Absyn_Datatypedecl*_tmpD4;struct Cyc_Absyn_Datatypefield*_tmpD5;struct Cyc_List_List*_tmpD6;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->tag == 4){if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).field_info).KnownDatatypefield).tag == 2){_LL9B: _tmpD4=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).field_info).KnownDatatypefield).val).f1;_tmpD5=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).field_info).KnownDatatypefield).val).f2;_tmpD6=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD3)->f1).targs;_LL9C:
# 396
{void*_tmpD7=Cyc_Tcutil_compress(inner_typ);void*_tmpD8=_tmpD7;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD8)->tag == 4){_LLA0: _LLA1:
# 398
 goto DONT_PROMOTE;}else{_LLA2: _LLA3:
 goto _LL9F;}_LL9F:;}{
# 402
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp4C5;struct Cyc_Absyn_Datatypedecl**_tmp4C4;struct Cyc_Absyn_DatatypeInfo _tmp4C3;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4C2;void*new_type=(void*)((_tmp4C2=_cycalloc(sizeof(*_tmp4C2)),((_tmp4C2[0]=((_tmp4C5.tag=3,((_tmp4C5.f1=((_tmp4C3.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4[0]=_tmpD4,_tmp4C4))))),((_tmp4C3.targs=_tmpD6,_tmp4C3)))),_tmp4C5)))),_tmp4C2))));
# 405
_tmp8F->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4CB;struct Cyc_Absyn_PtrInfo _tmp4CA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C9;t=(void*)((_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((_tmp4C9[0]=((_tmp4CB.tag=5,((_tmp4CB.f1=((_tmp4CA.elt_typ=new_type,((_tmp4CA.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp4CA.ptr_atts=(
((_tmp4CA.ptr_atts).rgn=ptr_rgn,(((_tmp4CA.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4CA.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp4CA.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp4CA.ptr_atts).ptrloc=0,_tmp4CA.ptr_atts)))))))))),_tmp4CA)))))),_tmp4CB)))),_tmp4C9))));}
# 412
goto _LL9A;};}else{goto _LL9D;}}else{_LL9D: _LL9E:
# 414
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4D1;struct Cyc_Absyn_PtrInfo _tmp4D0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CF;t=(void*)((_tmp4CF=_cycalloc(sizeof(*_tmp4CF)),((_tmp4CF[0]=((_tmp4D1.tag=5,((_tmp4D1.f1=((_tmp4D0.elt_typ=(void*)_check_null(_tmp8F->topt),((_tmp4D0.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp4D0.ptr_atts=(
((_tmp4D0.ptr_atts).rgn=ptr_rgn,(((_tmp4D0.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4D0.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4D0.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp4D0.ptr_atts).ptrloc=0,_tmp4D0.ptr_atts)))))))))),_tmp4D0)))))),_tmp4D1)))),_tmp4CF))));}}_LL9A:;}
# 422
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp8F->topt;
goto _LL54;};}case 5: _LL71: _tmp8D=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp8E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL72: {
# 426
struct Cyc_List_List*_tmpE3=*_tmp8D;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpE4=Cyc_Tcutil_compress(*topt);void*_tmpE5=_tmpE4;struct Cyc_List_List*_tmpE6;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE5)->tag == 10){_LLA5: _tmpE6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE5)->f1;_LLA6:
# 432
 topt_ts=_tmpE6;
if(_tmp8E){
# 435
int _tmpE7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE3);
int _tmpE8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE6);
if(_tmpE7 < _tmpE8){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpE8 - _tmpE7;++ i){
struct Cyc_List_List*_tmp4D2;wild_ps=((_tmp4D2=_cycalloc(sizeof(*_tmp4D2)),((_tmp4D2->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp4D2->tl=wild_ps,_tmp4D2))))));}}
*_tmp8D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE3,wild_ps);
_tmpE3=*_tmp8D;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE3)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE6)){
const char*_tmp4D5;void*_tmp4D4;(_tmp4D4=0,Cyc_Tcutil_warn(p->loc,((_tmp4D5="unnecessary ... in tuple pattern",_tag_dyneither(_tmp4D5,sizeof(char),33))),_tag_dyneither(_tmp4D4,sizeof(void*),0)));}}}
# 447
goto _LLA4;}else{_LLA7: _LLA8:
# 449
 goto _LLA4;}_LLA4:;}else{
# 451
if(_tmp8E){
const char*_tmp4D8;void*_tmp4D7;(_tmp4D7=0,Cyc_Tcutil_terr(p->loc,((_tmp4D8="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp4D8,sizeof(char),57))),_tag_dyneither(_tmp4D7,sizeof(void*),0)));}}
{int i=0;for(0;_tmpE3 != 0;(_tmpE3=_tmpE3->tl,i ++)){
void**_tmpEE=0;
if(topt_ts != 0){
_tmpEE=&(*((struct _tuple18*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 459
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp4DB;union Cyc_Absyn_Cnst _tmp4DA;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp4DA.Int_c).val=((_tmp4DB.f1=Cyc_Absyn_Unsigned,((_tmp4DB.f2=i,_tmp4DB)))),(((_tmp4DA.Int_c).tag=5,_tmp4DA)))),0),0);}
# 464
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpE3->hd,_tmpEE,rgn_pat,allow_ref_pat,new_access_exp));
# 467
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpE3->hd)->topt;{
struct _tuple18*_tmp4DE;struct Cyc_List_List*_tmp4DD;pat_ts=((_tmp4DD=_cycalloc(sizeof(*_tmp4DD)),((_tmp4DD->hd=((_tmp4DE=_cycalloc(sizeof(*_tmp4DE)),((_tmp4DE->f1=Cyc_Absyn_empty_tqual(0),((_tmp4DE->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpE3->hd)->topt),_tmp4DE)))))),((_tmp4DD->tl=pat_ts,_tmp4DD))))));};};}}
# 470
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp4E1;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4E0;t=(void*)((_tmp4E0=_cycalloc(sizeof(*_tmp4E0)),((_tmp4E0[0]=((_tmp4E1.tag=10,((_tmp4E1.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp4E1)))),_tmp4E0))));}
goto _LL54;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->aggr_info).KnownAggr).tag == 2){_LL73: _tmp88=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->aggr_info).KnownAggr).val;_tmp89=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->targs;_tmp8A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_tmp8B=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f3;_tmp8C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f4;_LL74: {
# 474
struct Cyc_List_List*_tmpF5=*_tmp8B;
const char*_tmp4E3;const char*_tmp4E2;struct _dyneither_ptr aggr_str=_tmp88->kind == Cyc_Absyn_StructA?(_tmp4E3="struct",_tag_dyneither(_tmp4E3,sizeof(char),7)):((_tmp4E2="union",_tag_dyneither(_tmp4E2,sizeof(char),6)));
if(_tmp88->impl == 0){
{const char*_tmp4E7;void*_tmp4E6[1];struct Cyc_String_pa_PrintArg_struct _tmp4E5;(_tmp4E5.tag=0,((_tmp4E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp4E6[0]=& _tmp4E5,Cyc_Tcutil_terr(p->loc,((_tmp4E7="can't destructure an abstract %s",_tag_dyneither(_tmp4E7,sizeof(char),33))),_tag_dyneither(_tmp4E6,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL54;}
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
{void*_tmpFE=_tmpFD;struct Cyc_Absyn_Kind*_tmpFF;struct Cyc_Absyn_Kind*_tmp100;switch(*((int*)_tmpFE)){case 2: _LLAA: _tmp100=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFE)->f2;_LLAB:
 _tmpFF=_tmp100;goto _LLAD;case 0: _LLAC: _tmpFF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFE)->f1;_LLAD:
 k2=_tmpFF;goto _LLA9;default: _LLAE: _LLAF: {
const char*_tmp4EA;void*_tmp4E9;(_tmp4E9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4EA="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp4EA,sizeof(char),53))),_tag_dyneither(_tmp4E9,sizeof(void*),0)));}}_LLA9:;}
# 510
{void*_tmp103=_tmpFC;struct Cyc_Core_Opt**_tmp104;struct Cyc_Core_Opt**_tmp105;struct Cyc_Absyn_Kind*_tmp106;struct Cyc_Absyn_Kind*_tmp107;switch(*((int*)_tmp103)){case 0: _LLB1: _tmp107=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp103)->f1;_LLB2:
# 513
 if(!Cyc_Tcutil_kind_leq(k2,_tmp107))
error=1;
goto _LLB0;case 2: _LLB3: _tmp105=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp103)->f1;_tmp106=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp103)->f2;_LLB4:
 _tmp104=_tmp105;goto _LLB6;default: _LLB5: _tmp104=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp103)->f1;_LLB6:
{struct Cyc_Core_Opt*_tmp4EB;*_tmp104=((_tmp4EB=_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB->v=_tmpFD,_tmp4EB))));}goto _LLB0;}_LLB0:;}
# 519
if(error){
const char*_tmp4F1;void*_tmp4F0[3];struct Cyc_String_pa_PrintArg_struct _tmp4EF;struct Cyc_String_pa_PrintArg_struct _tmp4EE;struct Cyc_String_pa_PrintArg_struct _tmp4ED;(_tmp4ED.tag=0,((_tmp4ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 523
Cyc_Absynpp_kind2string(k2)),((_tmp4EE.tag=0,((_tmp4EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpFC)),((_tmp4EF.tag=0,((_tmp4EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp4F0[0]=& _tmp4EF,((_tmp4F0[1]=& _tmp4EE,((_tmp4F0[2]=& _tmp4ED,Cyc_Tcutil_terr(p->loc,((_tmp4F1="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp4F1,sizeof(char),60))),_tag_dyneither(_tmp4F0,sizeof(void*),3)))))))))))))))))));}{
# 525
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4F4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F3;void*vartype=(void*)((_tmp4F3=_cycalloc(sizeof(*_tmp4F3)),((_tmp4F3[0]=((_tmp4F4.tag=2,((_tmp4F4.f1=tv,_tmp4F4)))),_tmp4F3))));
{struct Cyc_List_List*_tmp4F5;_tmpFA=((_tmp4F5=_region_malloc(rgn,sizeof(*_tmp4F5)),((_tmp4F5->hd=vartype,((_tmp4F5->tl=_tmpFA,_tmp4F5))))));}
# 529
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp4F8;struct Cyc_List_List*_tmp4F7;outlives_constraints=(
(_tmp4F7=_cycalloc(sizeof(*_tmp4F7)),((_tmp4F7->hd=((_tmp4F8=_cycalloc(sizeof(*_tmp4F8)),((_tmp4F8->f1=Cyc_Absyn_empty_effect,((_tmp4F8->f2=vartype,_tmp4F8)))))),((_tmp4F7->tl=outlives_constraints,_tmp4F7))))));}else{
# 534
const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FB="opened existential had unique or top region kind",_tag_dyneither(_tmp4FB,sizeof(char),49))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}}};};}}
# 538
_tmpFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpFA);{
# 540
struct _RegionHandle _tmp115=_new_region("r2");struct _RegionHandle*r2=& _tmp115;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmp8A);
# 543
struct Cyc_List_List*_tmp116=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp4FC;struct _tuple12 _tmp117=(_tmp4FC.f1=_tmp116,((_tmp4FC.f2=rgn,_tmp4FC)));
struct Cyc_List_List*_tmp118=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp117,_tmp88->tvs);
struct Cyc_List_List*_tmp119=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->exist_vars,_tmpFA);
struct Cyc_List_List*_tmp11A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp118);
struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp119);
struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp118,_tmp119);
# 551
if(_tmp8A != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp4FD;res.tvars_and_bounds_opt=((_tmp4FD=_cycalloc(sizeof(*_tmp4FD)),((_tmp4FD->f1=0,((_tmp4FD->f2=0,_tmp4FD))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmp8A));
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp11E=0;
{struct Cyc_List_List*_tmp11F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->rgn_po;for(0;_tmp11F != 0;_tmp11F=_tmp11F->tl){
struct _tuple0*_tmp500;struct Cyc_List_List*_tmp4FF;_tmp11E=((_tmp4FF=_cycalloc(sizeof(*_tmp4FF)),((_tmp4FF->hd=((_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp11C,(*((struct _tuple0*)_tmp11F->hd)).f1),((_tmp500->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp11C,(*((struct _tuple0*)_tmp11F->hd)).f2),_tmp500)))))),((_tmp4FF->tl=_tmp11E,_tmp4FF))))));}}
# 563
_tmp11E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11E);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp11E);};}
# 568
*_tmp89=_tmp11A;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp50A;struct Cyc_Absyn_Aggrdecl**_tmp509;struct Cyc_Absyn_AggrInfo _tmp508;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp507;t=(void*)((_tmp507=_cycalloc(sizeof(*_tmp507)),((_tmp507[0]=((_tmp50A.tag=11,((_tmp50A.f1=((_tmp508.aggr_info=Cyc_Absyn_KnownAggr(((_tmp509=_cycalloc(sizeof(*_tmp509)),((_tmp509[0]=_tmp88,_tmp509))))),((_tmp508.targs=*_tmp89,_tmp508)))),_tmp50A)))),_tmp507))));}
if(_tmp8C  && _tmp88->kind == Cyc_Absyn_UnionA){
const char*_tmp50D;void*_tmp50C;(_tmp50C=0,Cyc_Tcutil_warn(p->loc,((_tmp50D="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp50D,sizeof(char),43))),_tag_dyneither(_tmp50C,sizeof(void*),0)));}else{
if(_tmp8C){
# 574
int _tmp128=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF5);
int _tmp129=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->fields);
if(_tmp128 < _tmp129){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp129 - _tmp128;++ i){
struct _tuple15*_tmp510;struct Cyc_List_List*_tmp50F;wild_dps=((_tmp50F=_cycalloc(sizeof(*_tmp50F)),((_tmp50F->hd=((_tmp510=_cycalloc(sizeof(*_tmp510)),((_tmp510->f1=0,((_tmp510->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp510)))))),((_tmp50F->tl=wild_dps,_tmp50F))))));}}
*_tmp8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpF5,wild_dps);
_tmpF5=*_tmp8B;}else{
if(_tmp128 == _tmp129){
const char*_tmp513;void*_tmp512;(_tmp512=0,Cyc_Tcutil_warn(p->loc,((_tmp513="unnecessary ... in struct pattern",_tag_dyneither(_tmp513,sizeof(char),34))),_tag_dyneither(_tmp512,sizeof(void*),0)));}}}}{
# 585
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpF5,_tmp88->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp88->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmp12E=(struct _tuple19*)fields->hd;struct _tuple19*_tmp12F=_tmp12E;struct Cyc_Absyn_Aggrfield*_tmp130;struct Cyc_Absyn_Pat*_tmp131;_LLB8: _tmp130=_tmp12F->f1;_tmp131=_tmp12F->f2;_LLB9: {
void*_tmp132=Cyc_Tcutil_rsubstitute(rgn,_tmp11C,_tmp130->type);
# 591
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp130->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp131,& _tmp132,rgn_pat,allow_ref_pat,new_access_exp));
# 599
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp131->topt),_tmp132)){
const char*_tmp51A;void*_tmp519[4];struct Cyc_String_pa_PrintArg_struct _tmp518;struct Cyc_String_pa_PrintArg_struct _tmp517;struct Cyc_String_pa_PrintArg_struct _tmp516;struct Cyc_String_pa_PrintArg_struct _tmp515;(_tmp515.tag=0,((_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 602
Cyc_Absynpp_typ2string((void*)_check_null(_tmp131->topt))),((_tmp516.tag=0,((_tmp516.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 601
Cyc_Absynpp_typ2string(_tmp132)),((_tmp517.tag=0,((_tmp517.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp518.tag=0,((_tmp518.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp130->name),((_tmp519[0]=& _tmp518,((_tmp519[1]=& _tmp517,((_tmp519[2]=& _tmp516,((_tmp519[3]=& _tmp515,Cyc_Tcutil_terr(p->loc,((_tmp51A="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp51A,sizeof(char),45))),_tag_dyneither(_tmp519,sizeof(void*),4)))))))))))))))))))))))));}
# 603
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp131->topt;}_LLB7:;}};}
# 541
;_pop_region(r2);};}
# 607
_npop_handler(0);goto _LL54;
# 492
;_pop_region(rgn);};}}else{_LL79: _LL7A:
# 663
 goto _LL7C;}}else{_LL77: _LL78:
# 662
 goto _LL7A;}case 8: _LL75: _tmp84=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp85=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_tmp86=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f3;_tmp87=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f4;_LL76: {
# 610
struct Cyc_List_List*_tmp13C=*_tmp86;
struct _RegionHandle _tmp13D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp13D;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp85->typs;
# 614
struct Cyc_List_List*_tmp13E=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp51B;struct _tuple12 _tmp13F=(_tmp51B.f1=_tmp13E,((_tmp51B.f2=rgn,_tmp51B)));
struct Cyc_List_List*_tmp140=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp13F,_tmp84->tvs);
struct Cyc_List_List*_tmp141=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp140);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp521;struct Cyc_Absyn_DatatypeFieldInfo _tmp520;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51F;t=(void*)((_tmp51F=_cycalloc(sizeof(*_tmp51F)),((_tmp51F[0]=((_tmp521.tag=4,((_tmp521.f1=((_tmp520.field_info=Cyc_Absyn_KnownDatatypefield(_tmp84,_tmp85),((_tmp520.targs=_tmp141,_tmp520)))),_tmp521)))),_tmp51F))));}
# 620
if(_tmp87){
# 622
int _tmp145=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp13C);
int _tmp146=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp145 < _tmp146){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp146 - _tmp145;++ i){
struct Cyc_List_List*_tmp522;wild_ps=((_tmp522=_cycalloc(sizeof(*_tmp522)),((_tmp522->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp522->tl=wild_ps,_tmp522))))));}}
*_tmp86=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp13C,wild_ps);
_tmp13C=*_tmp86;}else{
if(_tmp145 == _tmp146){
const char*_tmp526;void*_tmp525[1];struct Cyc_String_pa_PrintArg_struct _tmp524;(_tmp524.tag=0,((_tmp524.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp84->name)),((_tmp525[0]=& _tmp524,Cyc_Tcutil_warn(p->loc,((_tmp526="unnecessary ... in datatype field %s",_tag_dyneither(_tmp526,sizeof(char),37))),_tag_dyneither(_tmp525,sizeof(void*),1)))))));}}}
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
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp14B->topt),_tmp14C)){
const char*_tmp52C;void*_tmp52B[3];struct Cyc_String_pa_PrintArg_struct _tmp52A;struct Cyc_String_pa_PrintArg_struct _tmp529;struct Cyc_String_pa_PrintArg_struct _tmp528;(_tmp528.tag=0,((_tmp528.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 651
Cyc_Absynpp_typ2string((void*)_check_null(_tmp14B->topt))),((_tmp529.tag=0,((_tmp529.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 650
Cyc_Absynpp_typ2string(_tmp14C)),((_tmp52A.tag=0,((_tmp52A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp85->name)),((_tmp52B[0]=& _tmp52A,((_tmp52B[1]=& _tmp529,((_tmp52B[2]=& _tmp528,Cyc_Tcutil_terr(_tmp14B->loc,((_tmp52C="%s expects argument type %s, not %s",_tag_dyneither(_tmp52C,sizeof(char),36))),_tag_dyneither(_tmp52B,sizeof(void*),3)))))))))))))))))));}}
# 653
if(_tmp13C != 0){
const char*_tmp530;void*_tmp52F[1];struct Cyc_String_pa_PrintArg_struct _tmp52E;(_tmp52E.tag=0,((_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp85->name)),((_tmp52F[0]=& _tmp52E,Cyc_Tcutil_terr(p->loc,((_tmp530="too many arguments for datatype constructor %s",_tag_dyneither(_tmp530,sizeof(char),47))),_tag_dyneither(_tmp52F,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp534;void*_tmp533[1];struct Cyc_String_pa_PrintArg_struct _tmp532;(_tmp532.tag=0,((_tmp532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp85->name)),((_tmp533[0]=& _tmp532,Cyc_Tcutil_terr(p->loc,((_tmp534="too few arguments for datatype constructor %s",_tag_dyneither(_tmp534,sizeof(char),46))),_tag_dyneither(_tmp533,sizeof(void*),1)))))));}}
# 660
_npop_handler(0);goto _LL54;
# 611
;_pop_region(rgn);}case 15: _LL7B: _LL7C:
# 664
 goto _LL7E;case 17: _LL7D: _LL7E:
 goto _LL80;default: _LL7F: _LL80:
# 667
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL54;}_LL54:;}
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
struct _tuple1 _tmp15C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp15A.patvars);struct _tuple1 _tmp15D=_tmp15C;struct Cyc_List_List*_tmp15E;_LLBD: _tmp15E=_tmp15D.f1;_LLBE: {
struct Cyc_List_List*_tmp15F=0;
{struct Cyc_List_List*x=_tmp15E;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp535;_tmp15F=((_tmp535=_region_malloc(r,sizeof(*_tmp535)),((_tmp535->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp535->tl=_tmp15F,_tmp535))))));}}}
{const char*_tmp536;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp15F),p->loc,(
(_tmp536="pattern contains a repeated variable",_tag_dyneither(_tmp536,sizeof(char),37))));}{
# 688
struct Cyc_List_List*x=_tmp15A.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp162=(struct _tuple16*)x->hd;struct _tuple16*_tmp163=_tmp162;struct Cyc_Absyn_Vardecl**_tmp164;struct Cyc_Absyn_Exp**_tmp165;_LLC2: _tmp164=_tmp163->f1;_tmp165=(struct Cyc_Absyn_Exp**)& _tmp163->f2;_LLC3:
 if(*_tmp165 != 0  && *_tmp165 != pat_var_exp)
*_tmp165=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp165));_LLC1:;}};}_LLBC:;}{
# 694
struct Cyc_Tcpat_TcPatResult _tmp166=_tmp15A;_npop_handler(0);return _tmp166;};
# 678
;_pop_region(r);}
# 700
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 703
struct _tuple0 _tmp537;struct _tuple0 _tmp167=(_tmp537.f1=Cyc_Tcutil_compress(old_type),((_tmp537.f2=Cyc_Tcutil_compress(new_type),_tmp537)));struct _tuple0 _tmp168=_tmp167;struct Cyc_Absyn_PtrInfo _tmp169;struct Cyc_Absyn_PtrInfo _tmp16A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f2)->tag == 5){_LLC7: _tmp169=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f1)->f1;_tmp16A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f2)->f1;_LLC8: {
# 705
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp53D;struct Cyc_Absyn_PtrInfo _tmp53C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp53B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16B=(_tmp53B=_cycalloc(sizeof(*_tmp53B)),((_tmp53B[0]=((_tmp53D.tag=5,((_tmp53D.f1=((_tmp53C.elt_typ=_tmp169.elt_typ,((_tmp53C.elt_tq=_tmp16A.elt_tq,((_tmp53C.ptr_atts=(
((_tmp53C.ptr_atts).rgn=(_tmp169.ptr_atts).rgn,(((_tmp53C.ptr_atts).nullable=(_tmp16A.ptr_atts).nullable,(((_tmp53C.ptr_atts).bounds=(_tmp16A.ptr_atts).bounds,(((_tmp53C.ptr_atts).zero_term=(_tmp16A.ptr_atts).zero_term,(((_tmp53C.ptr_atts).ptrloc=(_tmp169.ptr_atts).ptrloc,_tmp53C.ptr_atts)))))))))),_tmp53C)))))),_tmp53D)))),_tmp53B)));
# 711
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp16B,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp16B);}}else{goto _LLC9;}}else{_LLC9: _LLCA:
 return 0;}_LLC6:;}
# 720
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 723
struct _tuple0*_tmp547;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp546;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp545;struct Cyc_List_List*_tmp544;struct Cyc_List_List*assump=(_tmp544=_cycalloc(sizeof(*_tmp544)),((_tmp544->hd=((_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547->f1=(void*)& Cyc_Absyn_UniqueRgn_val,((_tmp547->f2=(void*)((_tmp545=_cycalloc(sizeof(*_tmp545)),((_tmp545[0]=((_tmp546.tag=2,((_tmp546.f1=tv,_tmp546)))),_tmp545)))),_tmp547)))))),((_tmp544->tl=0,_tmp544)))));
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 741 "tcpat.cyc"
struct _tuple0 _tmp548;struct _tuple0 _tmp170=(_tmp548.f1=Cyc_Tcutil_compress(old_type),((_tmp548.f2=Cyc_Tcutil_compress(new_type),_tmp548)));struct _tuple0 _tmp171=_tmp170;struct Cyc_Absyn_PtrInfo _tmp172;struct Cyc_Absyn_PtrInfo _tmp173;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f2)->tag == 5){_LLCC: _tmp172=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f1)->f1;_tmp173=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f2)->f1;_LLCD:
 goto _LLCB;}else{goto _LLCE;}}else{_LLCE: _LLCF:
{const char*_tmp54B;void*_tmp54A;(_tmp54A=0,Cyc_Tcutil_terr(loc,((_tmp54B="alias requires pointer type",_tag_dyneither(_tmp54B,sizeof(char),28))),_tag_dyneither(_tmp54A,sizeof(void*),0)));}goto _LLCB;}_LLCB:;}else{
# 746
const char*_tmp550;void*_tmp54F[2];struct Cyc_String_pa_PrintArg_struct _tmp54E;struct Cyc_String_pa_PrintArg_struct _tmp54D;(_tmp54D.tag=0,((_tmp54D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type)),((_tmp54E.tag=0,((_tmp54E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type)),((_tmp54F[0]=& _tmp54E,((_tmp54F[1]=& _tmp54D,Cyc_Tcutil_terr(loc,((_tmp550="cannot alias value of type %s to type %s",_tag_dyneither(_tmp550,sizeof(char),41))),_tag_dyneither(_tmp54F,sizeof(void*),2)))))))))))));}}
# 753
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 757
void*_tmp17F=p->r;void*_tmp180=_tmp17F;struct Cyc_Absyn_Tvar*_tmp181;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Pat*_tmp184;struct Cyc_Absyn_Vardecl*_tmp185;struct Cyc_Absyn_Pat*_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_Absyn_AggrInfo*_tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Pat*_tmp18C;switch(*((int*)_tmp180)){case 6: _LLD1: _tmp18C=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_LLD2: {
# 759
void*_tmp18D=(void*)_check_null(p->topt);void*_tmp18E=_tmp18D;void*_tmp18F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18E)->tag == 5){_LLE2: _tmp18F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18E)->f1).ptr_atts).rgn;_LLE3:
# 761
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp18F);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp18C,Cyc_Tcutil_is_noalias_region(_tmp18F,0),patvars);
return;}else{_LLE4: _LLE5: {
const char*_tmp553;void*_tmp552;(_tmp552=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp553="check_pat_regions: bad pointer type",_tag_dyneither(_tmp553,sizeof(char),36))),_tag_dyneither(_tmp552,sizeof(void*),0)));}}_LLE1:;}case 7: _LLD3: _tmp189=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp18A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_tmp18B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f3;_LLD4:
# 767
 for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp18B->hd)).f2,did_noalias_deref,patvars);}
return;case 8: _LLD5: _tmp188=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp180)->f3;_LLD6:
# 771
 did_noalias_deref=0;_tmp187=_tmp188;goto _LLD8;case 5: _LLD7: _tmp187=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_LLD8:
# 773
 for(0;_tmp187 != 0;_tmp187=_tmp187->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp187->hd,did_noalias_deref,patvars);}
return;case 3: _LLD9: _tmp185=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp186=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLDA:
# 777
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp192=(struct _tuple16*)x->hd;struct _tuple16*_tmp193=_tmp192;struct Cyc_Absyn_Vardecl**_tmp194;struct Cyc_Absyn_Exp*_tmp195;_LLE7: _tmp194=_tmp193->f1;_tmp195=_tmp193->f2;_LLE8:
# 783
 if((_tmp194 != 0  && *_tmp194 == _tmp185) && _tmp195 != 0){
{void*_tmp196=_tmp195->r;void*_tmp197=_tmp196;struct Cyc_Absyn_Exp*_tmp198;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp197)->tag == 14){_LLEC: _tmp198=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp197)->f1;_LLED:
# 786
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp198->topt))){
# 788
const char*_tmp556;void*_tmp555;(_tmp555=0,Cyc_Tcutil_terr(p->loc,((_tmp556="reference pattern not allowed on alias-free pointers",_tag_dyneither(_tmp556,sizeof(char),53))),_tag_dyneither(_tmp555,sizeof(void*),0)));}
goto _LLEB;}else{_LLEE: _LLEF: {
# 791
const char*_tmp559;void*_tmp558;(_tmp558=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp559="check_pat_regions: bad reference access expression",_tag_dyneither(_tmp559,sizeof(char),51))),_tag_dyneither(_tmp558,sizeof(void*),0)));}}_LLEB:;}
# 793
break;}_LLE6:;}}
# 796
Cyc_Tcpat_check_pat_regions_rec(te,_tmp186,did_noalias_deref,patvars);
return;case 1: _LLDB: _tmp183=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp184=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLDC:
# 799
{void*_tmp19D=p->topt;void*_tmp19E=_tmp19D;if(_tmp19E != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp19E)->tag == 8){_LLF1: _LLF2:
# 801
 if(did_noalias_deref){
{const char*_tmp55C;void*_tmp55B;(_tmp55B=0,Cyc_Tcutil_terr(p->loc,((_tmp55C="pattern to array would create alias of no-alias pointer",_tag_dyneither(_tmp55C,sizeof(char),56))),_tag_dyneither(_tmp55B,sizeof(void*),0)));}
return;}
# 805
goto _LLF0;}else{goto _LLF3;}}else{_LLF3: _LLF4:
 goto _LLF0;}_LLF0:;}
# 808
Cyc_Tcpat_check_pat_regions_rec(te,_tmp184,did_noalias_deref,patvars);
return;case 2: _LLDD: _tmp181=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp182=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLDE:
# 811
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1A1=(struct _tuple16*)x->hd;struct _tuple16*_tmp1A2=_tmp1A1;struct Cyc_Absyn_Vardecl**_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;_LLF6: _tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;_LLF7:
# 815
 if(_tmp1A3 != 0  && *_tmp1A3 == _tmp182){
if(_tmp1A4 == 0){
const char*_tmp55F;void*_tmp55E;(_tmp55E=0,Cyc_Tcutil_terr(p->loc,((_tmp55F="cannot alias pattern expression in datatype",_tag_dyneither(_tmp55F,sizeof(char),44))),_tag_dyneither(_tmp55E,sizeof(void*),0)));}else{
# 819
struct _RegionHandle _tmp1A7=_new_region("r");struct _RegionHandle*r=& _tmp1A7;_push_region(r);
{struct Cyc_Absyn_Tvar*_tmp560[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,p->loc,te,((_tmp560[0]=_tmp181,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp560,sizeof(struct Cyc_Absyn_Tvar*),1)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp563;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp562;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp562=_cycalloc(sizeof(*_tmp562)),((_tmp562[0]=((_tmp563.tag=2,((_tmp563.f1=_tmp181,_tmp563)))),_tmp562)))),0,1);}
# 823
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp1A4->topt),_tmp181,_tmp182->type,_tmp1A4);}
# 820
;_pop_region(r);}
# 827
break;}_LLF5:;}}
# 830
goto _LLD0;default: _LLDF: _LLE0:
 return;}_LLD0:;}
# 846 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 848
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp1AB=_new_region("r");struct _RegionHandle*r=& _tmp1AB;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1AC=(struct _tuple16*)x->hd;struct _tuple16*_tmp1AD=_tmp1AC;struct Cyc_Absyn_Vardecl**_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AF;_LLFB: _tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;_LLFC:
 if(_tmp1AF != 0){
struct Cyc_Absyn_Exp*_tmp1B0=_tmp1AF;
# 856
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1B0->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1B0)){
# 859
const char*_tmp570;void*_tmp56F[1];const char*_tmp56E;struct Cyc_String_pa_PrintArg_struct _tmp56D;void*_tmp56C[1];const char*_tmp56B;struct Cyc_String_pa_PrintArg_struct _tmp56A;(_tmp56A.tag=0,((_tmp56A.f1=(struct _dyneither_ptr)(
# 861
_tmp1AE != 0?(struct _dyneither_ptr)(
(_tmp56D.tag=0,((_tmp56D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1AE)->name)),((_tmp56C[0]=& _tmp56D,Cyc_aprintf(((_tmp56B="for variable %s",_tag_dyneither(_tmp56B,sizeof(char),16))),_tag_dyneither(_tmp56C,sizeof(void*),1)))))))):(
(_tmp56E="",_tag_dyneither(_tmp56E,sizeof(char),1)))),((_tmp56F[0]=& _tmp56A,Cyc_Tcutil_terr(p->loc,((_tmp570="pattern %s dereferences a alias-free pointer from a non-unique path",_tag_dyneither(_tmp570,sizeof(char),68))),_tag_dyneither(_tmp56F,sizeof(void*),1)))))));}}_LLFA:;}}
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
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp571;return((_tmp571.Name_v).val=s,(((_tmp571.Name_v).tag=1,_tmp571)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp572;return((_tmp572.Int_v).val=i,(((_tmp572.Int_v).tag=2,_tmp572)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};
# 951
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 956
typedef void*Cyc_Tcpat_simple_pat_t;
# 959
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp1BE=c1->name;union Cyc_Tcpat_Name_value _tmp1BF=_tmp1BE;int _tmp1C0;struct _dyneither_ptr _tmp1C1;if((_tmp1BF.Name_v).tag == 1){_LL100: _tmp1C1=(_tmp1BF.Name_v).val;_LL101: {
# 962
union Cyc_Tcpat_Name_value _tmp1C2=c2->name;union Cyc_Tcpat_Name_value _tmp1C3=_tmp1C2;struct _dyneither_ptr _tmp1C4;if((_tmp1C3.Name_v).tag == 1){_LL105: _tmp1C4=(_tmp1C3.Name_v).val;_LL106:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1C1,(struct _dyneither_ptr)_tmp1C4);}else{_LL107: _LL108:
 return - 1;}_LL104:;}}else{_LL102: _tmp1C0=(_tmp1BF.Int_v).val;_LL103: {
# 967
union Cyc_Tcpat_Name_value _tmp1C5=c2->name;union Cyc_Tcpat_Name_value _tmp1C6=_tmp1C5;int _tmp1C7;if((_tmp1C6.Name_v).tag == 1){_LL10A: _LL10B:
 return 1;}else{_LL10C: _tmp1C7=(_tmp1C6.Int_v).val;_LL10D:
 return _tmp1C0 - _tmp1C7;}_LL109:;}}_LLFF:;}
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
++ i;}}{
# 1000
struct Cyc_String_pa_PrintArg_struct _tmp57A;void*_tmp579[1];const char*_tmp578;void*_tmp577;(_tmp577=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp57A.tag=0,((_tmp57A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp579[0]=& _tmp57A,Cyc_aprintf(((_tmp578="get_member_offset %s failed",_tag_dyneither(_tmp578,sizeof(char),28))),_tag_dyneither(_tmp579,sizeof(void*),1)))))))),_tag_dyneither(_tmp577,sizeof(void*),0)));};}
# 1003
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp1CF=pw;struct Cyc_Absyn_Pat*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;if((_tmp1CF.where_clause).tag == 2){_LL10F: _tmp1D1=(_tmp1CF.where_clause).val;_LL110: {
struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp57D;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp57C;return(void*)((_tmp57C=_cycalloc(sizeof(*_tmp57C)),((_tmp57C[0]=((_tmp57D.tag=0,((_tmp57D.f1=_tmp1D1,_tmp57D)))),_tmp57C))));}}else{_LL111: _tmp1D0=(_tmp1CF.pattern).val;_LL112: {
# 1007
void*_tmp1D4=_tmp1D0->r;void*_tmp1D5=_tmp1D4;union Cyc_Absyn_AggrInfoU _tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_Datatypedecl*_tmp1D8;struct Cyc_Absyn_Datatypefield*_tmp1D9;void*_tmp1DA;struct Cyc_Absyn_Enumfield*_tmp1DB;struct Cyc_Absyn_Enumdecl*_tmp1DC;struct Cyc_Absyn_Enumfield*_tmp1DD;struct _dyneither_ptr _tmp1DE;int _tmp1DF;char _tmp1E0;enum Cyc_Absyn_Sign _tmp1E1;int _tmp1E2;struct Cyc_Absyn_Pat*_tmp1E3;struct Cyc_Absyn_Pat*_tmp1E4;switch(*((int*)_tmp1D5)){case 1: _LL114: _tmp1E4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL115:
 _tmp1E3=_tmp1E4;goto _LL117;case 3: _LL116: _tmp1E3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL117: {
union Cyc_Tcpat_PatOrWhere _tmp57E;return Cyc_Tcpat_get_pat_test((((_tmp57E.pattern).val=_tmp1E3,(((_tmp57E.pattern).tag=1,_tmp57E)))));}case 9: _LL118: _LL119:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _LL11A: _tmp1E1=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1E2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL11B: {
struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp581;struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp580;return(void*)((_tmp580=_cycalloc_atomic(sizeof(*_tmp580)),((_tmp580[0]=((_tmp581.tag=6,((_tmp581.f1=(unsigned int)_tmp1E2,_tmp581)))),_tmp580))));}case 11: _LL11C: _tmp1E0=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_LL11D: {
struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp584;struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp583;return(void*)((_tmp583=_cycalloc_atomic(sizeof(*_tmp583)),((_tmp583[0]=((_tmp584.tag=6,((_tmp584.f1=(unsigned int)_tmp1E0,_tmp584)))),_tmp583))));}case 12: _LL11E: _tmp1DE=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1DF=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL11F: {
struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp587;struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp586;return(void*)((_tmp586=_cycalloc(sizeof(*_tmp586)),((_tmp586[0]=((_tmp587.tag=5,((_tmp587.f1=_tmp1DE,((_tmp587.f2=_tmp1DF,_tmp587)))))),_tmp586))));}case 13: _LL120: _tmp1DC=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1DD=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL121: {
struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp58A;struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp589;return(void*)((_tmp589=_cycalloc(sizeof(*_tmp589)),((_tmp589[0]=((_tmp58A.tag=3,((_tmp58A.f1=_tmp1DC,((_tmp58A.f2=_tmp1DD,_tmp58A)))))),_tmp589))));}case 14: _LL122: _tmp1DA=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1DB=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL123: {
struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp58D;struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp58C;return(void*)((_tmp58C=_cycalloc(sizeof(*_tmp58C)),((_tmp58C[0]=((_tmp58D.tag=4,((_tmp58D.f1=_tmp1DA,((_tmp58D.f2=_tmp1DB,_tmp58D)))))),_tmp58C))));}case 6: _LL124: _LL125:
# 1017
{void*_tmp1F0=Cyc_Tcutil_compress((void*)_check_null(_tmp1D0->topt));void*_tmp1F1=_tmp1F0;union Cyc_Absyn_Constraint*_tmp1F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1)->tag == 5){_LL12D: _tmp1F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1)->f1).ptr_atts).nullable;_LL12E:
# 1019
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp1F2))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL12C;}else{_LL12F: _LL130:
 goto _LL12C;}_LL12C:;}{
# 1024
const char*_tmp590;void*_tmp58F;(_tmp58F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp590="non-null pointer type or non-pointer type in pointer pattern",_tag_dyneither(_tmp590,sizeof(char),61))),_tag_dyneither(_tmp58F,sizeof(void*),0)));};case 8: _LL126: _tmp1D8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1;_tmp1D9=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D5)->f2;_LL127:
# 1026
 if(_tmp1D8->is_extensible){
struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp593;struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp592;return(void*)((_tmp592=_cycalloc(sizeof(*_tmp592)),((_tmp592[0]=((_tmp593.tag=9,((_tmp593.f1=_tmp1D8,((_tmp593.f2=_tmp1D9,_tmp593)))))),_tmp592))));}else{
# 1029
struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp596;struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp595;return(void*)((_tmp595=_cycalloc(sizeof(*_tmp595)),((_tmp595[0]=((_tmp596.tag=7,((_tmp596.f1=(int)Cyc_Tcpat_datatype_tag_number(_tmp1D8,_tmp1D9->name),((_tmp596.f2=_tmp1D8,((_tmp596.f3=_tmp1D9,_tmp596)))))))),_tmp595))));}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1 != 0){_LL128: _tmp1D6=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f1)->aggr_info;_tmp1D7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D5)->f3;_LL129: {
# 1031
struct Cyc_Absyn_Aggrdecl*_tmp1F9=Cyc_Absyn_get_known_aggrdecl(_tmp1D6);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F9->impl))->tagged){
struct _tuple15*_tmp1FA=(struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp1D7))->hd;struct _tuple15*_tmp1FB=_tmp1FA;struct Cyc_List_List*_tmp1FC;struct Cyc_Absyn_Pat*_tmp1FD;_LL132: _tmp1FC=_tmp1FB->f1;_tmp1FD=_tmp1FB->f2;_LL133: {
struct _dyneither_ptr*f;
{void*_tmp1FE=(void*)((struct Cyc_List_List*)_check_null(_tmp1FC))->hd;void*_tmp1FF=_tmp1FE;struct _dyneither_ptr*_tmp200;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1FF)->tag == 1){_LL137: _tmp200=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1FF)->f1;_LL138:
 f=_tmp200;goto _LL136;}else{_LL139: _LL13A: {
const char*_tmp599;void*_tmp598;(_tmp598=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp599="no field name in tagged union pattern",_tag_dyneither(_tmp599,sizeof(char),38))),_tag_dyneither(_tmp598,sizeof(void*),0)));}}_LL136:;}{
# 1039
struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp59C;struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp59B;return(void*)((_tmp59B=_cycalloc(sizeof(*_tmp59B)),((_tmp59B[0]=((_tmp59C.tag=8,((_tmp59C.f1=f,((_tmp59C.f2=Cyc_Tcpat_get_member_offset(_tmp1F9,f),_tmp59C)))))),_tmp59B))));};}_LL131:;}else{
# 1041
const char*_tmp59F;void*_tmp59E;(_tmp59E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp59F="non-tagged aggregate in pattern test",_tag_dyneither(_tmp59F,sizeof(char),37))),_tag_dyneither(_tmp59E,sizeof(void*),0)));}}}else{goto _LL12A;}default: _LL12A: _LL12B: {
const char*_tmp5A2;void*_tmp5A1;(_tmp5A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5A2="non-test pattern in pattern test",_tag_dyneither(_tmp5A2,sizeof(char),33))),_tag_dyneither(_tmp5A1,sizeof(void*),0)));}}_LL113:;}}_LL10E:;}
# 1047
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
union Cyc_Tcpat_PatOrWhere _tmp5A3;return((_tmp5A3.pattern).val=p,(((_tmp5A3.pattern).tag=1,_tmp5A3)));}
# 1051
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
const char*_tmp5A6;struct Cyc_Tcpat_Con_s*_tmp5A5;return(_tmp5A5=_cycalloc(sizeof(*_tmp5A5)),((_tmp5A5->name=Cyc_Tcpat_Name_v(((_tmp5A6="NULL",_tag_dyneither(_tmp5A6,sizeof(char),5)))),((_tmp5A5->arity=0,((_tmp5A5->span=& Cyc_Tcpat_two_opt,((_tmp5A5->orig_pat=Cyc_Tcpat_pw(p),_tmp5A5)))))))));}
# 1054
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
const char*_tmp5A9;struct Cyc_Tcpat_Con_s*_tmp5A8;return(_tmp5A8=_cycalloc(sizeof(*_tmp5A8)),((_tmp5A8->name=Cyc_Tcpat_Name_v(((_tmp5A9="&",_tag_dyneither(_tmp5A9,sizeof(char),2)))),((_tmp5A8->arity=1,((_tmp5A8->span=& Cyc_Tcpat_two_opt,((_tmp5A8->orig_pat=Cyc_Tcpat_pw(p),_tmp5A8)))))))));}
# 1057
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
const char*_tmp5AC;struct Cyc_Tcpat_Con_s*_tmp5AB;return(_tmp5AB=_cycalloc(sizeof(*_tmp5AB)),((_tmp5AB->name=Cyc_Tcpat_Name_v(((_tmp5AC="&",_tag_dyneither(_tmp5AC,sizeof(char),2)))),((_tmp5AB->arity=1,((_tmp5AB->span=& Cyc_Tcpat_one_opt,((_tmp5AB->orig_pat=Cyc_Tcpat_pw(p),_tmp5AB)))))))));}
# 1060
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
struct Cyc_Tcpat_Con_s*_tmp5AD;return(_tmp5AD=_cycalloc(sizeof(*_tmp5AD)),((_tmp5AD->name=Cyc_Tcpat_Int_v(i),((_tmp5AD->arity=0,((_tmp5AD->span=0,((_tmp5AD->orig_pat=p,_tmp5AD)))))))));}
# 1063
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp5AE;return(_tmp5AE=_cycalloc(sizeof(*_tmp5AE)),((_tmp5AE->name=Cyc_Tcpat_Name_v(f),((_tmp5AE->arity=0,((_tmp5AE->span=0,((_tmp5AE->orig_pat=Cyc_Tcpat_pw(p),_tmp5AE)))))))));}
# 1066
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp5AF;return(_tmp5AF=_cycalloc(sizeof(*_tmp5AF)),((_tmp5AF->name=Cyc_Tcpat_Int_v((int)c),((_tmp5AF->arity=0,((_tmp5AF->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp5AF->orig_pat=Cyc_Tcpat_pw(p),_tmp5AF)))))))));}
# 1069
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
const char*_tmp5B2;struct Cyc_Tcpat_Con_s*_tmp5B1;return(_tmp5B1=_cycalloc(sizeof(*_tmp5B1)),((_tmp5B1->name=Cyc_Tcpat_Name_v(((_tmp5B2="$",_tag_dyneither(_tmp5B2,sizeof(char),2)))),((_tmp5B1->arity=i,((_tmp5B1->span=& Cyc_Tcpat_one_opt,((_tmp5B1->orig_pat=p,_tmp5B1)))))))));}
# 1074
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5B5;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5B4;return(void*)((_tmp5B4=_cycalloc(sizeof(*_tmp5B4)),((_tmp5B4[0]=((_tmp5B5.tag=1,((_tmp5B5.f1=Cyc_Tcpat_null_con(p),((_tmp5B5.f2=0,_tmp5B5)))))),_tmp5B4))));}
# 1077
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5B8;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5B7;return(void*)((_tmp5B7=_cycalloc(sizeof(*_tmp5B7)),((_tmp5B7[0]=((_tmp5B8.tag=1,((_tmp5B8.f1=Cyc_Tcpat_int_con(i,p),((_tmp5B8.f2=0,_tmp5B8)))))),_tmp5B7))));}
# 1080
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5BB;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5BA;return(void*)((_tmp5BA=_cycalloc(sizeof(*_tmp5BA)),((_tmp5BA[0]=((_tmp5BB.tag=1,((_tmp5BB.f1=Cyc_Tcpat_char_con(c,p),((_tmp5BB.f2=0,_tmp5BB)))))),_tmp5BA))));}
# 1083
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5BE;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5BD;return(void*)((_tmp5BD=_cycalloc(sizeof(*_tmp5BD)),((_tmp5BD[0]=((_tmp5BE.tag=1,((_tmp5BE.f1=Cyc_Tcpat_float_con(f,p),((_tmp5BE.f2=0,_tmp5BE)))))),_tmp5BD))));}
# 1086
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5C4;struct Cyc_List_List*_tmp5C3;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5C2;return(void*)((_tmp5C2=_cycalloc(sizeof(*_tmp5C2)),((_tmp5C2[0]=((_tmp5C4.tag=1,((_tmp5C4.f1=Cyc_Tcpat_null_ptr_con(p0),((_tmp5C4.f2=((_tmp5C3=_cycalloc(sizeof(*_tmp5C3)),((_tmp5C3->hd=p,((_tmp5C3->tl=0,_tmp5C3)))))),_tmp5C4)))))),_tmp5C2))));}
# 1089
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5CA;struct Cyc_List_List*_tmp5C9;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5C8;return(void*)((_tmp5C8=_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8[0]=((_tmp5CA.tag=1,((_tmp5CA.f1=Cyc_Tcpat_ptr_con(p0),((_tmp5CA.f2=((_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9->hd=p,((_tmp5C9->tl=0,_tmp5C9)))))),_tmp5CA)))))),_tmp5C8))));}
# 1092
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5CD;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5CC;return(void*)((_tmp5CC=_cycalloc(sizeof(*_tmp5CC)),((_tmp5CC[0]=((_tmp5CD.tag=1,((_tmp5CD.f1=Cyc_Tcpat_tuple_con(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp5CD.f2=ss,_tmp5CD)))))),_tmp5CC))));}
# 1095
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1097
struct Cyc_Tcpat_Con_s*_tmp5CE;struct Cyc_Tcpat_Con_s*c=(_tmp5CE=_cycalloc(sizeof(*_tmp5CE)),((_tmp5CE->name=Cyc_Tcpat_Name_v(con_name),((_tmp5CE->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp5CE->span=span,((_tmp5CE->orig_pat=Cyc_Tcpat_pw(p),_tmp5CE)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp5D1;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp5D0;return(void*)((_tmp5D0=_cycalloc(sizeof(*_tmp5D0)),((_tmp5D0[0]=((_tmp5D1.tag=1,((_tmp5D1.f1=c,((_tmp5D1.f2=ps,_tmp5D1)))))),_tmp5D0))));}
# 1102
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp228=p->r;void*_tmp229=_tmp228;void*_tmp22A;struct Cyc_Absyn_Enumfield*_tmp22B;struct Cyc_Absyn_Enumdecl*_tmp22C;struct Cyc_Absyn_Enumfield*_tmp22D;struct Cyc_Absyn_Aggrdecl*_tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_Absyn_Datatypedecl*_tmp231;struct Cyc_Absyn_Datatypefield*_tmp232;struct Cyc_List_List*_tmp233;struct Cyc_Absyn_Pat*_tmp234;struct Cyc_Absyn_Pat*_tmp235;struct Cyc_Absyn_Pat*_tmp236;struct _dyneither_ptr _tmp237;char _tmp238;enum Cyc_Absyn_Sign _tmp239;int _tmp23A;switch(*((int*)_tmp229)){case 0: _LL13C: _LL13D:
 goto _LL13F;case 2: _LL13E: _LL13F:
 goto _LL141;case 4: _LL140: _LL141:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp5D4;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp5D3;s=(void*)((_tmp5D3=_cycalloc_atomic(sizeof(*_tmp5D3)),((_tmp5D3[0]=((_tmp5D4.tag=0,_tmp5D4)),_tmp5D3))));}goto _LL13B;case 9: _LL142: _LL143:
 s=Cyc_Tcpat_null_pat(p);goto _LL13B;case 10: _LL144: _tmp239=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp23A=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL145:
 s=Cyc_Tcpat_int_pat(_tmp23A,Cyc_Tcpat_pw(p));goto _LL13B;case 11: _LL146: _tmp238=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL147:
 s=Cyc_Tcpat_char_pat(_tmp238,p);goto _LL13B;case 12: _LL148: _tmp237=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL149:
 s=Cyc_Tcpat_float_pat(_tmp237,p);goto _LL13B;case 1: _LL14A: _tmp236=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL14B:
 s=Cyc_Tcpat_compile_pat(_tmp236);goto _LL13B;case 3: _LL14C: _tmp235=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL14D:
 s=Cyc_Tcpat_compile_pat(_tmp235);goto _LL13B;case 6: _LL14E: _tmp234=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL14F:
# 1115
{void*_tmp23D=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp23E=_tmp23D;union Cyc_Absyn_Constraint*_tmp23F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E)->tag == 5){_LL163: _tmp23F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E)->f1).ptr_atts).nullable;_LL164: {
# 1117
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp240=_tmp23F;int _tmp241;union Cyc_Absyn_Constraint*_tmp242;switch((((union Cyc_Absyn_Constraint*)_tmp240)->No_constr).tag){case 2: _LL168: _tmp242=(_tmp240->Forward_constr).val;_LL169:
# 1122
*_tmp23F=*_tmp242;
continue;case 3: _LL16A: _LL16B:
# 1125
{struct _union_Constraint_Eq_constr*_tmp5D5;(_tmp5D5=& _tmp23F->Eq_constr,((_tmp5D5->tag=1,_tmp5D5->val=0)));}
is_nullable=0;
still_working=0;
goto _LL167;default: _LL16C: _tmp241=(int)(_tmp240->Eq_constr).val;_LL16D:
# 1130
 is_nullable=_tmp241;
still_working=0;
goto _LL167;}_LL167:;}{
# 1135
void*ss=Cyc_Tcpat_compile_pat(_tmp234);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(ss,p);else{
# 1139
s=Cyc_Tcpat_ptr_pat(ss,p);}
goto _LL162;};}}else{_LL165: _LL166: {
const char*_tmp5D8;void*_tmp5D7;(_tmp5D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5D8="expecting pointertype for pattern!",_tag_dyneither(_tmp5D8,sizeof(char),35))),_tag_dyneither(_tmp5D7,sizeof(void*),0)));}}_LL162:;}
# 1143
goto _LL13B;case 8: _LL150: _tmp231=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp232=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_tmp233=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f3;_LL151: {
# 1145
int*span;
{void*_tmp246=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp247=_tmp246;switch(*((int*)_tmp247)){case 3: _LL16F: _LL170:
# 1148
 if(_tmp231->is_extensible)
span=0;else{
# 1151
int*_tmp5D9;span=((_tmp5D9=_cycalloc_atomic(sizeof(*_tmp5D9)),((_tmp5D9[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp231->fields))->v),_tmp5D9))));}
goto _LL16E;case 4: _LL171: _LL172:
 span=& Cyc_Tcpat_one_opt;goto _LL16E;default: _LL173: _LL174:
{const char*_tmp5DC;void*_tmp5DB;span=((_tmp5DB=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5DC="void datatype pattern has bad type",_tag_dyneither(_tmp5DC,sizeof(char),35))),_tag_dyneither(_tmp5DB,sizeof(void*),0))));}goto _LL16E;}_LL16E:;}
# 1156
s=Cyc_Tcpat_con_pat(*(*_tmp232->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp233),p);
goto _LL13B;}case 5: _LL152: _tmp230=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL153:
# 1160
 s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp230),Cyc_Tcpat_pw(p));goto _LL13B;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1)->aggr_info).KnownAggr).tag == 2){_LL154: _tmp22E=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1)->aggr_info).KnownAggr).val;_tmp22F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f3;_LL155:
# 1165
 if(_tmp22E->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1169
const char*_tmp5DD;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp5DD="",_tag_dyneither(_tmp5DD,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp22F;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_tmp24B=(struct _tuple15*)dlps0->hd;struct _tuple15*_tmp24C=_tmp24B;struct Cyc_List_List*_tmp24D;struct Cyc_Absyn_Pat*_tmp24E;_LL176: _tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;_LL177: {
struct Cyc_List_List*_tmp24F=_tmp24D;struct _dyneither_ptr*_tmp250;if(_tmp24F != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp24F)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp24F)->tl == 0){_LL17B: _tmp250=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp24F->hd)->f1;_LL17C:
# 1174
 if(Cyc_strptrcmp(_tmp250,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp5DE;ps=((_tmp5DE=_cycalloc(sizeof(*_tmp5DE)),((_tmp5DE->hd=Cyc_Tcpat_compile_pat(_tmp24E),((_tmp5DE->tl=ps,_tmp5DE))))));}
found=1;}
# 1178
goto _LL17A;}else{goto _LL17D;}}else{goto _LL17D;}}else{_LL17D: _LL17E: {
const char*_tmp5E1;void*_tmp5E0;(_tmp5E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5E1="bad designator(s)",_tag_dyneither(_tmp5E1,sizeof(char),18))),_tag_dyneither(_tmp5E0,sizeof(void*),0)));}}_LL17A:;}_LL175:;}}
# 1182
if(!found){
const char*_tmp5E4;void*_tmp5E3;(_tmp5E3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5E4="bad designator",_tag_dyneither(_tmp5E4,sizeof(char),15))),_tag_dyneither(_tmp5E3,sizeof(void*),0)));}}}
# 1185
s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps),Cyc_Tcpat_pw(p));}else{
# 1188
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->tagged){
const char*_tmp5E7;void*_tmp5E6;(_tmp5E6=0,Cyc_Tcutil_terr(p->loc,((_tmp5E7="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp5E7,sizeof(char),47))),_tag_dyneither(_tmp5E6,sizeof(void*),0)));}{
int*_tmp5E8;int*span=(_tmp5E8=_cycalloc_atomic(sizeof(*_tmp5E8)),((_tmp5E8[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22E->impl))->fields),_tmp5E8)));
struct Cyc_List_List*_tmp259=_tmp22F;struct _dyneither_ptr*_tmp25A;struct Cyc_Absyn_Pat*_tmp25B;if(_tmp259 != 0){if(((struct _tuple15*)((struct Cyc_List_List*)_tmp259)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp259)->hd)->f1)->hd)->tag == 1){if(((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp259)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp259)->tl == 0){_LL180: _tmp25A=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple15*)_tmp259->hd)->f1)->hd)->f1;_tmp25B=((struct _tuple15*)_tmp259->hd)->f2;_LL181:
# 1193
{struct Cyc_List_List*_tmp5E9;s=Cyc_Tcpat_con_pat(*_tmp25A,span,((_tmp5E9=_cycalloc(sizeof(*_tmp5E9)),((_tmp5E9->hd=Cyc_Tcpat_compile_pat(_tmp25B),((_tmp5E9->tl=0,_tmp5E9)))))),p);}
goto _LL17F;}else{goto _LL182;}}else{goto _LL182;}}else{goto _LL182;}}else{goto _LL182;}}else{_LL182: _LL183: {
const char*_tmp5EC;void*_tmp5EB;(_tmp5EB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5EC="bad union pattern",_tag_dyneither(_tmp5EC,sizeof(char),18))),_tag_dyneither(_tmp5EB,sizeof(void*),0)));}}_LL17F:;};}
# 1198
goto _LL13B;}else{goto _LL15E;}}else{_LL15E: _LL15F:
# 1240
 goto _LL161;}case 13: _LL156: _tmp22C=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp22D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL157:
# 1203
{void*_tmp260=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp261=_tmp260;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp261)->tag == 6){_LL185: _LL186:
# 1207
 s=Cyc_Tcpat_con_pat(*(*_tmp22D->name).f2,0,0,p);
goto _LL184;}else{_LL187: _LL188: {
# 1210
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22C->fields))->v);
{int*_tmp5ED;s=Cyc_Tcpat_con_pat(*(*_tmp22D->name).f2,((_tmp5ED=_cycalloc_atomic(sizeof(*_tmp5ED)),((_tmp5ED[0]=span,_tmp5ED)))),0,p);}
goto _LL184;}}_LL184:;}
# 1214
goto _LL13B;case 14: _LL158: _tmp22A=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp22B=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL159: {
# 1219
struct Cyc_List_List*fields;
{void*_tmp263=Cyc_Tcutil_compress(_tmp22A);void*_tmp264=_tmp263;struct Cyc_List_List*_tmp265;if(((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp264)->tag == 14){_LL18A: _tmp265=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp264)->f1;_LL18B:
 fields=_tmp265;goto _LL189;}else{_LL18C: _LL18D: {
const char*_tmp5F0;void*_tmp5EF;(_tmp5EF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5F0="bad type in AnonEnum_p",_tag_dyneither(_tmp5F0,sizeof(char),23))),_tag_dyneither(_tmp5EF,sizeof(void*),0)));}}_LL189:;}
# 1226
{void*_tmp268=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp269=_tmp268;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp269)->tag == 6){_LL18F: _LL190:
# 1230
 s=Cyc_Tcpat_con_pat(*(*_tmp22B->name).f2,0,0,p);
goto _LL18E;}else{_LL191: _LL192: {
# 1233
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp5F1;s=Cyc_Tcpat_con_pat(*(*_tmp22B->name).f2,((_tmp5F1=_cycalloc_atomic(sizeof(*_tmp5F1)),((_tmp5F1[0]=span,_tmp5F1)))),0,p);}
goto _LL18E;}}_LL18E:;}
# 1237
goto _LL13B;}case 15: _LL15A: _LL15B:
 goto _LL15D;case 16: _LL15C: _LL15D:
 goto _LL15F;default: _LL160: _LL161: {
# 1241
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp5F4;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp5F3;s=(void*)((_tmp5F3=_cycalloc_atomic(sizeof(*_tmp5F3)),((_tmp5F3[0]=((_tmp5F4.tag=0,_tmp5F4)),_tmp5F3))));}}_LL13B:;}
# 1243
return s;}
# 1251
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1265
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1272
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp5F5;struct _tuple0 _tmp26D=(_tmp5F5.f1=a1,((_tmp5F5.f2=a2,_tmp5F5)));struct _tuple0 _tmp26E=_tmp26D;int _tmp26F;struct _dyneither_ptr*_tmp270;int _tmp271;struct _dyneither_ptr*_tmp272;struct Cyc_Absyn_Datatypefield*_tmp273;unsigned int _tmp274;struct Cyc_Absyn_Datatypefield*_tmp275;unsigned int _tmp276;unsigned int _tmp277;unsigned int _tmp278;switch(*((int*)_tmp26E.f1)){case 0: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp26E.f2)->tag == 0){_LL194: _LL195:
 return 1;}else{goto _LL19E;}case 1: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp26E.f2)->tag == 1){_LL196: _LL197:
 return 1;}else{goto _LL19E;}case 2: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 2){_LL198: _tmp277=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f1)->f1;_tmp278=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f2)->f1;_LL199:
 return _tmp277 == _tmp278;}else{goto _LL19E;}case 3: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 3){_LL19A: _tmp273=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f1)->f2;_tmp274=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f1)->f3;_tmp275=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->f2;_tmp276=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->f3;_LL19B:
# 1278
 return _tmp273 == _tmp275  && _tmp274 == _tmp276;}else{goto _LL19E;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 4){_LL19C: _tmp26F=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f1)->f1;_tmp270=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f1)->f2;_tmp271=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->f1;_tmp272=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->f2;_LL19D:
# 1280
 return _tmp26F == _tmp271  && Cyc_strptrcmp(_tmp270,_tmp272)== 0;}else{_LL19E: _LL19F:
 return 0;}}_LL193:;}
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
{void*_tmp27B=d2;struct Cyc_List_List*_tmp27C;struct Cyc_List_List*_tmp27D;void*_tmp27E;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->tag == 2){_LL1A1: _tmp27C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f1;_tmp27D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f2;_tmp27E=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f3;_LL1A2:
# 1299
 if(Cyc_Tcpat_same_path(access,_tmp27C) && (int)(((con->orig_pat).pattern).tag == 1)){
struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp5FF;struct _tuple0*_tmp5FE;struct Cyc_List_List*_tmp5FD;struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp5FC;return(void*)((_tmp5FC=_cycalloc(sizeof(*_tmp5FC)),((_tmp5FC[0]=((_tmp5FF.tag=2,((_tmp5FF.f1=_tmp27C,((_tmp5FF.f2=(
(_tmp5FD=_cycalloc(sizeof(*_tmp5FD)),((_tmp5FD->hd=((_tmp5FE=_cycalloc(sizeof(*_tmp5FE)),((_tmp5FE->f1=_tmp27A,((_tmp5FE->f2=d1,_tmp5FE)))))),((_tmp5FD->tl=_tmp27D,_tmp5FD)))))),((_tmp5FF.f3=_tmp27E,_tmp5FF)))))))),_tmp5FC))));}else{
# 1303
goto _LL1A0;}}else{_LL1A3: _LL1A4:
 goto _LL1A0;}_LL1A0:;}{
# 1306
struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp609;struct _tuple0*_tmp608;struct Cyc_List_List*_tmp607;struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp606;return(void*)((_tmp606=_cycalloc(sizeof(*_tmp606)),((_tmp606[0]=((_tmp609.tag=2,((_tmp609.f1=access,((_tmp609.f2=((_tmp607=_cycalloc(sizeof(*_tmp607)),((_tmp607->hd=((_tmp608=_cycalloc(sizeof(*_tmp608)),((_tmp608->f1=_tmp27A,((_tmp608->f2=d1,_tmp608)))))),((_tmp607->tl=0,_tmp607)))))),((_tmp609.f3=d2,_tmp609)))))))),_tmp606))));};}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
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
const char*_tmp60C;void*_tmp60B;(_tmp60B=0,Cyc_fprintf(Cyc_stderr,((_tmp60C=" ",_tag_dyneither(_tmp60C,sizeof(char),2))),_tag_dyneither(_tmp60B,sizeof(void*),0)));}}
# 1326
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp289=c->name;
union Cyc_Tcpat_Name_value _tmp28A=_tmp289;int _tmp28B;struct _dyneither_ptr _tmp28C;if((_tmp28A.Name_v).tag == 1){_LL1A6: _tmp28C=(_tmp28A.Name_v).val;_LL1A7:
{const char*_tmp610;void*_tmp60F[1];struct Cyc_String_pa_PrintArg_struct _tmp60E;(_tmp60E.tag=0,((_tmp60E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp28C),((_tmp60F[0]=& _tmp60E,Cyc_fprintf(Cyc_stderr,((_tmp610="%s",_tag_dyneither(_tmp610,sizeof(char),3))),_tag_dyneither(_tmp60F,sizeof(void*),1)))))));}goto _LL1A5;}else{_LL1A8: _tmp28B=(_tmp28A.Int_v).val;_LL1A9:
{const char*_tmp614;void*_tmp613[1];struct Cyc_Int_pa_PrintArg_struct _tmp612;(_tmp612.tag=1,((_tmp612.f1=(unsigned long)_tmp28B,((_tmp613[0]=& _tmp612,Cyc_fprintf(Cyc_stderr,((_tmp614="%d",_tag_dyneither(_tmp614,sizeof(char),3))),_tag_dyneither(_tmp613,sizeof(void*),1)))))));}goto _LL1A5;}_LL1A5:;}
# 1334
static void Cyc_Tcpat_print_access(void*a){
void*_tmp293=a;int _tmp294;struct _dyneither_ptr*_tmp295;struct Cyc_Absyn_Datatypefield*_tmp296;unsigned int _tmp297;unsigned int _tmp298;switch(*((int*)_tmp293)){case 0: _LL1AB: _LL1AC:
{const char*_tmp617;void*_tmp616;(_tmp616=0,Cyc_fprintf(Cyc_stderr,((_tmp617="DUMMY",_tag_dyneither(_tmp617,sizeof(char),6))),_tag_dyneither(_tmp616,sizeof(void*),0)));}goto _LL1AA;case 1: _LL1AD: _LL1AE:
{const char*_tmp61A;void*_tmp619;(_tmp619=0,Cyc_fprintf(Cyc_stderr,((_tmp61A="*",_tag_dyneither(_tmp61A,sizeof(char),2))),_tag_dyneither(_tmp619,sizeof(void*),0)));}goto _LL1AA;case 2: _LL1AF: _tmp298=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp293)->f1;_LL1B0:
{const char*_tmp61E;void*_tmp61D[1];struct Cyc_Int_pa_PrintArg_struct _tmp61C;(_tmp61C.tag=1,((_tmp61C.f1=(unsigned long)((int)_tmp298),((_tmp61D[0]=& _tmp61C,Cyc_fprintf(Cyc_stderr,((_tmp61E="[%d]",_tag_dyneither(_tmp61E,sizeof(char),5))),_tag_dyneither(_tmp61D,sizeof(void*),1)))))));}goto _LL1AA;case 3: _LL1B1: _tmp296=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp293)->f2;_tmp297=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp293)->f3;_LL1B2:
# 1340
{const char*_tmp623;void*_tmp622[2];struct Cyc_String_pa_PrintArg_struct _tmp621;struct Cyc_Int_pa_PrintArg_struct _tmp620;(_tmp620.tag=1,((_tmp620.f1=(unsigned long)((int)_tmp297),((_tmp621.tag=0,((_tmp621.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp296->name)),((_tmp622[0]=& _tmp621,((_tmp622[1]=& _tmp620,Cyc_fprintf(Cyc_stderr,((_tmp623="%s[%d]",_tag_dyneither(_tmp623,sizeof(char),7))),_tag_dyneither(_tmp622,sizeof(void*),2)))))))))))));}goto _LL1AA;default: _LL1B3: _tmp294=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp293)->f1;_tmp295=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp293)->f2;_LL1B4:
# 1342
 if(_tmp294){
const char*_tmp627;void*_tmp626[1];struct Cyc_String_pa_PrintArg_struct _tmp625;(_tmp625.tag=0,((_tmp625.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp295),((_tmp626[0]=& _tmp625,Cyc_fprintf(Cyc_stderr,((_tmp627=".tagunion.%s",_tag_dyneither(_tmp627,sizeof(char),13))),_tag_dyneither(_tmp626,sizeof(void*),1)))))));}else{
# 1345
const char*_tmp62B;void*_tmp62A[1];struct Cyc_String_pa_PrintArg_struct _tmp629;(_tmp629.tag=0,((_tmp629.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp295),((_tmp62A[0]=& _tmp629,Cyc_fprintf(Cyc_stderr,((_tmp62B=".%s",_tag_dyneither(_tmp62B,sizeof(char),4))),_tag_dyneither(_tmp62A,sizeof(void*),1)))))));}
goto _LL1AA;}_LL1AA:;}
# 1350
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp2AA=p;struct Cyc_Absyn_Datatypefield*_tmp2AB;struct _dyneither_ptr*_tmp2AC;int _tmp2AD;int _tmp2AE;unsigned int _tmp2AF;struct _dyneither_ptr _tmp2B0;struct Cyc_Absyn_Enumfield*_tmp2B1;struct Cyc_Absyn_Enumfield*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B3;switch(*((int*)_tmp2AA)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp2AA)->f1 == 0){_LL1B6: _LL1B7:
{const char*_tmp62E;void*_tmp62D;(_tmp62D=0,Cyc_fprintf(Cyc_stderr,((_tmp62E="where(NULL)",_tag_dyneither(_tmp62E,sizeof(char),12))),_tag_dyneither(_tmp62D,sizeof(void*),0)));}goto _LL1B5;}else{_LL1B8: _tmp2B3=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1B9:
{const char*_tmp632;void*_tmp631[1];struct Cyc_String_pa_PrintArg_struct _tmp630;(_tmp630.tag=0,((_tmp630.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp2B3))),((_tmp631[0]=& _tmp630,Cyc_fprintf(Cyc_stderr,((_tmp632="where(%s)",_tag_dyneither(_tmp632,sizeof(char),10))),_tag_dyneither(_tmp631,sizeof(void*),1)))))));}goto _LL1B5;}case 1: _LL1BA: _LL1BB:
{const char*_tmp635;void*_tmp634;(_tmp634=0,Cyc_fprintf(Cyc_stderr,((_tmp635="NULL",_tag_dyneither(_tmp635,sizeof(char),5))),_tag_dyneither(_tmp634,sizeof(void*),0)));}goto _LL1B5;case 2: _LL1BC: _LL1BD:
{const char*_tmp638;void*_tmp637;(_tmp637=0,Cyc_fprintf(Cyc_stderr,((_tmp638="NOT-NULL:",_tag_dyneither(_tmp638,sizeof(char),10))),_tag_dyneither(_tmp637,sizeof(void*),0)));}goto _LL1B5;case 4: _LL1BE: _tmp2B2=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1BF:
 _tmp2B1=_tmp2B2;goto _LL1C1;case 3: _LL1C0: _tmp2B1=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1C1:
{const char*_tmp63C;void*_tmp63B[1];struct Cyc_String_pa_PrintArg_struct _tmp63A;(_tmp63A.tag=0,((_tmp63A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B1->name)),((_tmp63B[0]=& _tmp63A,Cyc_fprintf(Cyc_stderr,((_tmp63C="%s",_tag_dyneither(_tmp63C,sizeof(char),3))),_tag_dyneither(_tmp63B,sizeof(void*),1)))))));}goto _LL1B5;case 5: _LL1C2: _tmp2B0=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1C3:
{const char*_tmp640;void*_tmp63F[1];struct Cyc_String_pa_PrintArg_struct _tmp63E;(_tmp63E.tag=0,((_tmp63E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2B0),((_tmp63F[0]=& _tmp63E,Cyc_fprintf(Cyc_stderr,((_tmp640="%s",_tag_dyneither(_tmp640,sizeof(char),3))),_tag_dyneither(_tmp63F,sizeof(void*),1)))))));}goto _LL1B5;case 6: _LL1C4: _tmp2AF=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1C5:
{const char*_tmp644;void*_tmp643[1];struct Cyc_Int_pa_PrintArg_struct _tmp642;(_tmp642.tag=1,((_tmp642.f1=(unsigned long)((int)_tmp2AF),((_tmp643[0]=& _tmp642,Cyc_fprintf(Cyc_stderr,((_tmp644="%d",_tag_dyneither(_tmp644,sizeof(char),3))),_tag_dyneither(_tmp643,sizeof(void*),1)))))));}goto _LL1B5;case 7: _LL1C6: _tmp2AE=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1C7:
{const char*_tmp648;void*_tmp647[1];struct Cyc_Int_pa_PrintArg_struct _tmp646;(_tmp646.tag=1,((_tmp646.f1=(unsigned long)_tmp2AE,((_tmp647[0]=& _tmp646,Cyc_fprintf(Cyc_stderr,((_tmp648="datatypetag(%d)",_tag_dyneither(_tmp648,sizeof(char),16))),_tag_dyneither(_tmp647,sizeof(void*),1)))))));}goto _LL1B5;case 8: _LL1C8: _tmp2AC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp2AA)->f1;_tmp2AD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1C9:
{const char*_tmp64D;void*_tmp64C[2];struct Cyc_String_pa_PrintArg_struct _tmp64B;struct Cyc_Int_pa_PrintArg_struct _tmp64A;(_tmp64A.tag=1,((_tmp64A.f1=(unsigned long)_tmp2AD,((_tmp64B.tag=0,((_tmp64B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2AC),((_tmp64C[0]=& _tmp64B,((_tmp64C[1]=& _tmp64A,Cyc_fprintf(Cyc_stderr,((_tmp64D="uniontag[%s](%d)",_tag_dyneither(_tmp64D,sizeof(char),17))),_tag_dyneither(_tmp64C,sizeof(void*),2)))))))))))));}goto _LL1B5;default: _LL1CA: _tmp2AB=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1CB: {
# 1363
const char*_tmp651;void*_tmp650[1];struct Cyc_String_pa_PrintArg_struct _tmp64F;(_tmp64F.tag=0,((_tmp64F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2AB->name)),((_tmp650[0]=& _tmp64F,Cyc_fprintf(Cyc_stderr,((_tmp651="datatypefield(%s)",_tag_dyneither(_tmp651,sizeof(char),18))),_tag_dyneither(_tmp650,sizeof(void*),1)))))));}}_LL1B5:;}
# 1367
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
const char*_tmp655;void*_tmp654[1];struct Cyc_String_pa_PrintArg_struct _tmp653;(_tmp653.tag=0,((_tmp653.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs)),((_tmp654[0]=& _tmp653,Cyc_fprintf(Cyc_stderr,((_tmp655="%s",_tag_dyneither(_tmp655,sizeof(char),3))),_tag_dyneither(_tmp654,sizeof(void*),1)))))));}
# 1371
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp2D3=d;struct Cyc_List_List*_tmp2D4;struct Cyc_List_List*_tmp2D5;void*_tmp2D6;struct Cyc_Tcpat_Rhs*_tmp2D7;switch(*((int*)_tmp2D3)){case 1: _LL1CD: _tmp2D7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2D3)->f1;_LL1CE:
# 1374
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp658;void*_tmp657;(_tmp657=0,Cyc_fprintf(Cyc_stderr,((_tmp658="Success(",_tag_dyneither(_tmp658,sizeof(char),9))),_tag_dyneither(_tmp657,sizeof(void*),0)));}Cyc_Tcpat_print_rhs(_tmp2D7);{const char*_tmp65B;void*_tmp65A;(_tmp65A=0,Cyc_fprintf(Cyc_stderr,((_tmp65B=")\n",_tag_dyneither(_tmp65B,sizeof(char),3))),_tag_dyneither(_tmp65A,sizeof(void*),0)));}
goto _LL1CC;case 0: _LL1CF: _LL1D0:
{const char*_tmp65E;void*_tmp65D;(_tmp65D=0,Cyc_fprintf(Cyc_stderr,((_tmp65E="Failure\n",_tag_dyneither(_tmp65E,sizeof(char),9))),_tag_dyneither(_tmp65D,sizeof(void*),0)));}goto _LL1CC;default: _LL1D1: _tmp2D4=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f1;_tmp2D5=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f2;_tmp2D6=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f3;_LL1D2:
# 1379
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp661;void*_tmp660;(_tmp660=0,Cyc_fprintf(Cyc_stderr,((_tmp661="Switch[",_tag_dyneither(_tmp661,sizeof(char),8))),_tag_dyneither(_tmp660,sizeof(void*),0)));}
for(0;_tmp2D4 != 0;_tmp2D4=_tmp2D4->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp2D4->hd)->access);
if(_tmp2D4->tl != 0){const char*_tmp664;void*_tmp663;(_tmp663=0,Cyc_fprintf(Cyc_stderr,((_tmp664=",",_tag_dyneither(_tmp664,sizeof(char),2))),_tag_dyneither(_tmp663,sizeof(void*),0)));}}
# 1385
{const char*_tmp667;void*_tmp666;(_tmp666=0,Cyc_fprintf(Cyc_stderr,((_tmp667="] {\n",_tag_dyneither(_tmp667,sizeof(char),5))),_tag_dyneither(_tmp666,sizeof(void*),0)));}
for(0;_tmp2D5 != 0;_tmp2D5=_tmp2D5->tl){
struct _tuple0 _tmp2E4=*((struct _tuple0*)_tmp2D5->hd);struct _tuple0 _tmp2E5=_tmp2E4;void*_tmp2E6;void*_tmp2E7;_LL1D4: _tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;_LL1D5:
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp66A;void*_tmp669;(_tmp669=0,Cyc_fprintf(Cyc_stderr,((_tmp66A="case ",_tag_dyneither(_tmp66A,sizeof(char),6))),_tag_dyneither(_tmp669,sizeof(void*),0)));}
Cyc_Tcpat_print_pat_test(_tmp2E6);
{const char*_tmp66D;void*_tmp66C;(_tmp66C=0,Cyc_fprintf(Cyc_stderr,((_tmp66D=":\n",_tag_dyneither(_tmp66D,sizeof(char),3))),_tag_dyneither(_tmp66C,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp2E7,tab + 7);_LL1D3:;}
# 1394
Cyc_Tcpat_print_tab(tab);
{const char*_tmp670;void*_tmp66F;(_tmp66F=0,Cyc_fprintf(Cyc_stderr,((_tmp670="default:\n",_tag_dyneither(_tmp670,sizeof(char),10))),_tag_dyneither(_tmp66F,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp2D6,tab + 7);
Cyc_Tcpat_print_tab(tab);{
const char*_tmp673;void*_tmp672;(_tmp672=0,Cyc_fprintf(Cyc_stderr,((_tmp673="}\n",_tag_dyneither(_tmp673,sizeof(char),3))),_tag_dyneither(_tmp672,sizeof(void*),0)));};}_LL1CC:;}
# 1402
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1410
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2F0=td;struct Cyc_Set_Set*_tmp2F1;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F0)->tag == 1){_LL1D9: _tmp2F1=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F0)->f1;_LL1DA: {
# 1420
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp676;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp675;return(void*)((_tmp675=_cycalloc(sizeof(*_tmp675)),((_tmp675[0]=((_tmp676.tag=1,((_tmp676.f1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp2F1,c),_tmp676)))),_tmp675))));}}else{_LL1DB: _LL1DC: {
const char*_tmp679;void*_tmp678;(_tmp678=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp679="add_neg called when td is Positive",_tag_dyneither(_tmp679,sizeof(char),35))),_tag_dyneither(_tmp678,sizeof(void*),0)));}}_LL1D8:;}
# 1427
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2F6=td;struct Cyc_Set_Set*_tmp2F7;struct Cyc_Tcpat_Con_s*_tmp2F8;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F6)->tag == 0){_LL1DE: _tmp2F8=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F6)->f1;_LL1DF:
# 1431
 if(Cyc_Tcpat_compare_con(c,_tmp2F8)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL1E0: _tmp2F7=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F6)->f1;_LL1E1:
# 1435
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F7,c))return Cyc_Tcpat_No;else{
# 1438
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2F7)+ 1)
return Cyc_Tcpat_Yes;else{
# 1441
return Cyc_Tcpat_Maybe;}}}_LL1DD:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1449
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp2F9=ctxt;struct Cyc_Tcpat_Con_s*_tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_List_List*_tmp2FC;if(_tmp2F9 == 0){_LL1E3: _LL1E4:
 return 0;}else{_LL1E5: _tmp2FA=((struct _tuple21*)_tmp2F9->hd)->f1;_tmp2FB=((struct _tuple21*)_tmp2F9->hd)->f2;_tmp2FC=_tmp2F9->tl;_LL1E6: {
# 1453
struct _tuple21*_tmp67F;struct Cyc_List_List*_tmp67E;struct Cyc_List_List*_tmp67D;return(_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D->hd=(
(_tmp67F=_cycalloc(sizeof(*_tmp67F)),((_tmp67F->f1=_tmp2FA,((_tmp67F->f2=((_tmp67E=_cycalloc(sizeof(*_tmp67E)),((_tmp67E->hd=dsc,((_tmp67E->tl=_tmp2FB,_tmp67E)))))),_tmp67F)))))),((_tmp67D->tl=_tmp2FC,_tmp67D)))));}}_LL1E2:;}
# 1461
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp300=ctxt;struct Cyc_Tcpat_Con_s*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp303;if(_tmp300 == 0){_LL1E8: _LL1E9: {
const char*_tmp682;void*_tmp681;(_tmp681=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp682="norm_context: empty context",_tag_dyneither(_tmp682,sizeof(char),28))),_tag_dyneither(_tmp681,sizeof(void*),0)));}}else{_LL1EA: _tmp301=((struct _tuple21*)_tmp300->hd)->f1;_tmp302=((struct _tuple21*)_tmp300->hd)->f2;_tmp303=_tmp300->tl;_LL1EB: {
# 1465
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp685;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp684;return Cyc_Tcpat_augment(_tmp303,(void*)((_tmp684=_cycalloc(sizeof(*_tmp684)),((_tmp684[0]=((_tmp685.tag=0,((_tmp685.f1=_tmp301,((_tmp685.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp302),_tmp685)))))),_tmp684)))));}}_LL1E7:;}
# 1474
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1476
struct _tuple1 _tmp686;struct _tuple1 _tmp308=(_tmp686.f1=ctxt,((_tmp686.f2=work,_tmp686)));struct _tuple1 _tmp309=_tmp308;struct Cyc_Tcpat_Con_s*_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_List_List*_tmp30D;struct Cyc_List_List*_tmp30E;if(_tmp309.f1 == 0){if(_tmp309.f2 == 0){_LL1ED: _LL1EE:
 return dsc;}else{_LL1EF: _LL1F0:
 goto _LL1F2;}}else{if(_tmp309.f2 == 0){_LL1F1: _LL1F2: {
const char*_tmp689;void*_tmp688;(_tmp688=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp689="build_desc: ctxt and work don't match",_tag_dyneither(_tmp689,sizeof(char),38))),_tag_dyneither(_tmp688,sizeof(void*),0)));}}else{_LL1F3: _tmp30A=((struct _tuple21*)(_tmp309.f1)->hd)->f1;_tmp30B=((struct _tuple21*)(_tmp309.f1)->hd)->f2;_tmp30C=(_tmp309.f1)->tl;_tmp30D=((struct _tuple20*)(_tmp309.f2)->hd)->f3;_tmp30E=(_tmp309.f2)->tl;_LL1F4: {
# 1481
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp68F;struct Cyc_List_List*_tmp68E;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp68D;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp311=(_tmp68D=_cycalloc(sizeof(*_tmp68D)),((_tmp68D[0]=((_tmp68F.tag=0,((_tmp68F.f1=_tmp30A,((_tmp68F.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp30B),((_tmp68E=_cycalloc(sizeof(*_tmp68E)),((_tmp68E->hd=dsc,((_tmp68E->tl=_tmp30D,_tmp68E))))))),_tmp68F)))))),_tmp68D)));
return Cyc_Tcpat_build_desc(_tmp30C,(void*)_tmp311,_tmp30E);}}}_LL1EC:;}
# 1486
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1493
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp316=allmrules;void*_tmp317;struct Cyc_Tcpat_Rhs*_tmp318;struct Cyc_List_List*_tmp319;if(_tmp316 == 0){_LL1F6: _LL1F7: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp692;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp691;return(void*)((_tmp691=_cycalloc(sizeof(*_tmp691)),((_tmp691[0]=((_tmp692.tag=0,((_tmp692.f1=dsc,_tmp692)))),_tmp691))));}}else{_LL1F8: _tmp317=((struct _tuple22*)_tmp316->hd)->f1;_tmp318=((struct _tuple22*)_tmp316->hd)->f2;_tmp319=_tmp316->tl;_LL1F9:
# 1497
 return Cyc_Tcpat_match(_tmp317,0,dsc,0,0,_tmp318,_tmp319);}_LL1F5:;}
# 1502
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp695;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp694;return Cyc_Tcpat_or_match((void*)((_tmp694=_cycalloc(sizeof(*_tmp694)),((_tmp694[0]=((_tmp695.tag=1,((_tmp695.f1=Cyc_Tcpat_empty_con_set(),_tmp695)))),_tmp694)))),allmrules);}
# 1509
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1512
struct Cyc_List_List*_tmp31E=work;struct Cyc_List_List*_tmp31F;struct Cyc_List_List*_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List*_tmp322;struct Cyc_List_List*_tmp323;if(_tmp31E == 0){_LL1FB: _LL1FC: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp698;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp697;return(void*)((_tmp697=_cycalloc(sizeof(*_tmp697)),((_tmp697[0]=((_tmp698.tag=1,((_tmp698.f1=right_hand_side,_tmp698)))),_tmp697))));}}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f3 == 0){_LL1FD: _tmp323=_tmp31E->tl;_LL1FE:
# 1515
 return Cyc_Tcpat_and_match(Cyc_Tcpat_norm_context(ctx),_tmp323,right_hand_side,rules);}else{goto _LL1FF;}}else{goto _LL1FF;}}else{_LL1FF: _tmp31F=((struct _tuple20*)_tmp31E->hd)->f1;_tmp320=((struct _tuple20*)_tmp31E->hd)->f2;_tmp321=((struct _tuple20*)_tmp31E->hd)->f3;_tmp322=_tmp31E->tl;_LL200:
# 1517
 if((_tmp31F == 0  || _tmp320 == 0) || _tmp321 == 0){
const char*_tmp69B;void*_tmp69A;(_tmp69A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp69B="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp69B,sizeof(char),38))),_tag_dyneither(_tmp69A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp328=_tmp31F;struct Cyc_List_List*_tmp329=_tmp328;void*_tmp32A;struct Cyc_List_List*_tmp32B;_LL202: _tmp32A=(void*)_tmp329->hd;_tmp32B=_tmp329->tl;_LL203: {
struct Cyc_List_List*_tmp32C=_tmp320;struct Cyc_List_List*_tmp32D=_tmp32C;struct Cyc_List_List*_tmp32E;struct Cyc_List_List*_tmp32F;_LL207: _tmp32E=(struct Cyc_List_List*)_tmp32D->hd;_tmp32F=_tmp32D->tl;_LL208: {
struct Cyc_List_List*_tmp330=_tmp321;struct Cyc_List_List*_tmp331=_tmp330;void*_tmp332;struct Cyc_List_List*_tmp333;_LL20C: _tmp332=(void*)_tmp331->hd;_tmp333=_tmp331->tl;_LL20D: {
struct _tuple20*_tmp69C;struct _tuple20*_tmp334=(_tmp69C=_cycalloc(sizeof(*_tmp69C)),((_tmp69C->f1=_tmp32B,((_tmp69C->f2=_tmp32F,((_tmp69C->f3=_tmp333,_tmp69C)))))));
struct Cyc_List_List*_tmp69D;return Cyc_Tcpat_match(_tmp32A,_tmp32E,_tmp332,ctx,((_tmp69D=_cycalloc(sizeof(*_tmp69D)),((_tmp69D->hd=_tmp334,((_tmp69D->tl=_tmp322,_tmp69D)))))),right_hand_side,rules);}_LL20B:;}_LL206:;}_LL201:;};}}_LL1FA:;}
# 1528
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp337=dsc;struct Cyc_List_List*_tmp338;struct Cyc_Set_Set*_tmp339;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp337)->tag == 1){_LL211: _tmp339=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp337)->f1;_LL212: {
# 1534
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp6A0;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp69F;void*any=(void*)((_tmp69F=_cycalloc(sizeof(*_tmp69F)),((_tmp69F[0]=((_tmp6A0.tag=1,((_tmp6A0.f1=Cyc_Tcpat_empty_con_set(),_tmp6A0)))),_tmp69F))));
struct Cyc_List_List*_tmp33A=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp6A1;_tmp33A=((_tmp6A1=_cycalloc(sizeof(*_tmp6A1)),((_tmp6A1->hd=any,((_tmp6A1->tl=_tmp33A,_tmp6A1))))));}}
return _tmp33A;}}else{_LL213: _tmp338=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp337)->f2;_LL214:
 return _tmp338;}_LL210:;}
# 1543
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp33E=pw;struct Cyc_Absyn_Pat*_tmp33F;if((_tmp33E.where_clause).tag == 2){_LL216: _LL217:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL218: _tmp33F=(_tmp33E.pattern).val;_LL219: {
# 1547
void*_tmp340=_tmp33F->r;void*_tmp341=_tmp340;union Cyc_Absyn_AggrInfoU _tmp342;struct Cyc_List_List*_tmp343;struct Cyc_Absyn_Datatypedecl*_tmp344;struct Cyc_Absyn_Datatypefield*_tmp345;switch(*((int*)_tmp341)){case 6: _LL21B: _LL21C:
# 1549
 if(i != 0){
struct Cyc_Int_pa_PrintArg_struct _tmp6A9;void*_tmp6A8[1];const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp6A9.tag=1,((_tmp6A9.f1=(unsigned long)i,((_tmp6A8[0]=& _tmp6A9,Cyc_aprintf(((_tmp6A7="get_access on pointer pattern with offset %d\n",_tag_dyneither(_tmp6A7,sizeof(char),46))),_tag_dyneither(_tmp6A8,sizeof(void*),1)))))))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}
return(void*)& Cyc_Tcpat_Deref_val;case 5: _LL21D: _LL21E: {
struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp6AC;struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp6AB;return(void*)((_tmp6AB=_cycalloc_atomic(sizeof(*_tmp6AB)),((_tmp6AB[0]=((_tmp6AC.tag=2,((_tmp6AC.f1=(unsigned int)i,_tmp6AC)))),_tmp6AB))));}case 8: _LL21F: _tmp344=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp341)->f1;_tmp345=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp341)->f2;_LL220: {
struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp6AF;struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp6AE;return(void*)((_tmp6AE=_cycalloc(sizeof(*_tmp6AE)),((_tmp6AE[0]=((_tmp6AF.tag=3,((_tmp6AF.f1=_tmp344,((_tmp6AF.f2=_tmp345,((_tmp6AF.f3=(unsigned int)i,_tmp6AF)))))))),_tmp6AE))));}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp341)->f1 != 0){_LL221: _tmp342=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp341)->f1)->aggr_info;_tmp343=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp341)->f3;_LL222: {
# 1555
struct Cyc_Absyn_Aggrdecl*_tmp34E=Cyc_Absyn_get_known_aggrdecl(_tmp342);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->tagged){
struct Cyc_List_List*_tmp34F=(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp343))->hd)).f1;struct Cyc_List_List*_tmp350=_tmp34F;struct _dyneither_ptr*_tmp351;if(_tmp350 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp350)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp350)->tl == 0){_LL226: _tmp351=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp350->hd)->f1;_LL227: {
# 1559
struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp6B2;struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp6B1;return(void*)((_tmp6B1=_cycalloc(sizeof(*_tmp6B1)),((_tmp6B1[0]=((_tmp6B2.tag=4,((_tmp6B2.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->tagged,((_tmp6B2.f2=_tmp351,_tmp6B2)))))),_tmp6B1))));}}else{goto _LL228;}}else{goto _LL228;}}else{_LL228: _LL229: {
struct Cyc_String_pa_PrintArg_struct _tmp6BA;void*_tmp6B9[1];const char*_tmp6B8;void*_tmp6B7;(_tmp6B7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp6BA.tag=0,((_tmp6BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp33F)),((_tmp6B9[0]=& _tmp6BA,Cyc_aprintf(((_tmp6B8="get_access on bad aggr pattern: %s",_tag_dyneither(_tmp6B8,sizeof(char),35))),_tag_dyneither(_tmp6B9,sizeof(void*),1)))))))),_tag_dyneither(_tmp6B7,sizeof(void*),0)));}}_LL225:;}{
# 1563
struct Cyc_List_List*_tmp358=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->fields;
int _tmp359=i;
for(0;i != 0;-- i){_tmp358=((struct Cyc_List_List*)_check_null(_tmp358))->tl;}{
struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp6BD;struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp6BC;return(void*)((_tmp6BC=_cycalloc(sizeof(*_tmp6BC)),((_tmp6BC[0]=((_tmp6BD.tag=4,((_tmp6BD.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34E->impl))->tagged,((_tmp6BD.f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp358))->hd)->name,_tmp6BD)))))),_tmp6BC))));};};}}else{goto _LL223;}default: _LL223: _LL224: {
struct Cyc_String_pa_PrintArg_struct _tmp6C5;void*_tmp6C4[1];const char*_tmp6C3;void*_tmp6C2;(_tmp6C2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp6C5.tag=0,((_tmp6C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp33F)),((_tmp6C4[0]=& _tmp6C5,Cyc_aprintf(((_tmp6C3="get_access on bad pattern: %s",_tag_dyneither(_tmp6C3,sizeof(char),30))),_tag_dyneither(_tmp6C4,sizeof(void*),1)))))))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));}}_LL21A:;}}_LL215:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1573
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp360=env;struct Cyc_List_List*_tmp361;struct Cyc_Tcpat_Con_s*_tmp362;_LL22B: _tmp361=_tmp360->f1;_tmp362=_tmp360->f2;_LL22C: {
void*acc=Cyc_Tcpat_get_access(_tmp362->orig_pat,i);
struct Cyc_Tcpat_PathNode*_tmp6C8;struct Cyc_List_List*_tmp6C7;return(_tmp6C7=_cycalloc(sizeof(*_tmp6C7)),((_tmp6C7->hd=((_tmp6C8=_cycalloc(sizeof(*_tmp6C8)),((_tmp6C8->orig_pat=_tmp362->orig_pat,((_tmp6C8->access=acc,_tmp6C8)))))),((_tmp6C7->tl=_tmp361,_tmp6C7)))));}_LL22A:;}
# 1578
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp6C9;struct _tuple23 _tmp365=(_tmp6C9.f1=obj,((_tmp6C9.f2=pcon,_tmp6C9)));
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp365);}
# 1586
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1590
void*_tmp367=p;struct Cyc_Tcpat_Con_s*_tmp368;struct Cyc_List_List*_tmp369;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp367)->tag == 0){_LL230: _LL231:
# 1592
 return Cyc_Tcpat_and_match(Cyc_Tcpat_augment(ctx,dsc),work,right_hand_side,rules);}else{_LL232: _tmp368=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp367)->f1;_tmp369=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp367)->f2;_LL233: {
# 1594
enum Cyc_Tcpat_Answer _tmp36A=Cyc_Tcpat_static_match(_tmp368,dsc);enum Cyc_Tcpat_Answer _tmp36B=_tmp36A;switch(_tmp36B){case Cyc_Tcpat_Yes: _LL235: _LL236: {
# 1596
struct _tuple21*_tmp6CC;struct Cyc_List_List*_tmp6CB;struct Cyc_List_List*ctx2=(_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB->hd=((_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC->f1=_tmp368,((_tmp6CC->f2=0,_tmp6CC)))))),((_tmp6CB->tl=ctx,_tmp6CB)))));
struct _tuple20*_tmp6CD;struct _tuple20*work_frame=(_tmp6CD=_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD->f1=_tmp369,((_tmp6CD->f2=Cyc_Tcpat_getoargs(_tmp368,obj),((_tmp6CD->f3=
Cyc_Tcpat_getdargs(_tmp368,dsc),_tmp6CD)))))));
struct Cyc_List_List*_tmp6CE;struct Cyc_List_List*work2=(_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE->hd=work_frame,((_tmp6CE->tl=work,_tmp6CE)))));
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL237: _LL238:
# 1602
 return Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,dsc,work),rules);default: _LL239: _LL23A: {
# 1604
struct _tuple21*_tmp6D1;struct Cyc_List_List*_tmp6D0;struct Cyc_List_List*ctx2=(_tmp6D0=_cycalloc(sizeof(*_tmp6D0)),((_tmp6D0->hd=((_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1->f1=_tmp368,((_tmp6D1->f2=0,_tmp6D1)))))),((_tmp6D0->tl=ctx,_tmp6D0)))));
struct _tuple20*_tmp6D2;struct _tuple20*work_frame=(_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2->f1=_tmp369,((_tmp6D2->f2=Cyc_Tcpat_getoargs(_tmp368,obj),((_tmp6D2->f3=
Cyc_Tcpat_getdargs(_tmp368,dsc),_tmp6D2)))))));
struct Cyc_List_List*_tmp6D3;struct Cyc_List_List*work2=(_tmp6D3=_cycalloc(sizeof(*_tmp6D3)),((_tmp6D3->hd=work_frame,((_tmp6D3->tl=work,_tmp6D3)))));
void*_tmp370=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp371=Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,Cyc_Tcpat_add_neg(dsc,_tmp368),work),rules);
# 1611
return Cyc_Tcpat_ifeq(obj,_tmp368,_tmp370,_tmp371);}}_LL234:;}}_LL22F:;}
# 1621
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1626
void*_tmp376=d;struct Cyc_List_List*_tmp377;void*_tmp378;struct Cyc_Tcpat_Rhs*_tmp379;void*_tmp37A;switch(*((int*)_tmp376)){case 0: _LL23C: _tmp37A=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp376)->f1;_LL23D:
# 1628
 if(!(*exhaust_done)){not_exhaust(env1,_tmp37A);*exhaust_done=1;}
goto _LL23B;case 1: _LL23E: _tmp379=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp376)->f1;_LL23F:
 rhs_appears(env2,_tmp379);goto _LL23B;default: _LL240: _tmp377=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp376)->f2;_tmp378=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp376)->f3;_LL241:
# 1632
 for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
struct _tuple0 _tmp37B=*((struct _tuple0*)_tmp377->hd);struct _tuple0 _tmp37C=_tmp37B;void*_tmp37D;_LL243: _tmp37D=_tmp37C.f2;_LL244:
 Cyc_Tcpat_check_exhaust_overlap(_tmp37D,not_exhaust,env1,rhs_appears,env2,exhaust_done);_LL242:;}
# 1637
Cyc_Tcpat_check_exhaust_overlap(_tmp378,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1639
goto _LL23B;}_LL23B:;}
# 1647
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1649
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*_tmp6D4;struct Cyc_Tcpat_Rhs*rhs=(_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4->used=0,((_tmp6D4->pat_loc=(swc->pattern)->loc,((_tmp6D4->rhs=swc->body,_tmp6D4)))))));
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp6D5;union Cyc_Tcpat_PatOrWhere _tmp37E=((_tmp6D5.where_clause).val=swc->where_clause,(((_tmp6D5.where_clause).tag=2,_tmp6D5)));
{void*_tmp6D6[2];sp=Cyc_Tcpat_tuple_pat(((_tmp6D6[1]=Cyc_Tcpat_int_pat(*ctr,_tmp37E),((_tmp6D6[0]=sp0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6D6,sizeof(void*),2)))))),_tmp37E);}
*ctr=*ctr + 1;}else{
# 1657
union Cyc_Tcpat_PatOrWhere _tmp6DD;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp6DC;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp6DB;void*_tmp6DA[2];sp=Cyc_Tcpat_tuple_pat(((_tmp6DA[1]=(void*)((_tmp6DC=_cycalloc_atomic(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DB.tag=0,_tmp6DB)),_tmp6DC)))),((_tmp6DA[0]=sp0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6DA,sizeof(void*),2)))))),(((_tmp6DD.where_clause).val=0,(((_tmp6DD.where_clause).tag=2,_tmp6DD)))));}{
struct _tuple22*_tmp6DE;return(_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE->f1=sp,((_tmp6DE->f2=rhs,_tmp6DE)))));};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1663
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1665
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp388=d;struct Cyc_Set_Set*_tmp389;struct Cyc_Tcpat_Con_s*_tmp38A;struct Cyc_List_List*_tmp38B;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->tag == 0){_LL248: _tmp38A=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->f1;_tmp38B=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->f2;_LL249: {
# 1669
union Cyc_Tcpat_Name_value _tmp38C=_tmp38A->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp38D=_tmp38A->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp38E=_tmp38D;struct Cyc_Absyn_Pat*_tmp38F;if((_tmp38E.where_clause).tag == 2){_LL24D: _LL24E:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp38B))->hd);}else{_LL24F: _tmp38F=(_tmp38E.pattern).val;_LL250:
 p=_tmp38F;goto _LL24C;}_LL24C:;}{
# 1675
void*_tmp390=p->r;void*_tmp391=_tmp390;struct Cyc_Absyn_Exp*_tmp392;struct Cyc_Absyn_Enumfield*_tmp393;struct Cyc_Absyn_Enumfield*_tmp394;struct _dyneither_ptr _tmp395;int _tmp396;char _tmp397;int _tmp398;struct Cyc_Absyn_Datatypefield*_tmp399;struct Cyc_Absyn_Aggrdecl*_tmp39A;struct Cyc_List_List*_tmp39B;struct Cyc_Absyn_Tvar*_tmp39C;struct Cyc_Absyn_Vardecl*_tmp39D;struct Cyc_Absyn_Vardecl*_tmp39E;struct Cyc_Absyn_Vardecl*_tmp39F;switch(*((int*)_tmp391)){case 0: _LL252: _LL253: {
const char*_tmp6DF;return(_tmp6DF="_",_tag_dyneither(_tmp6DF,sizeof(char),2));}case 1: _LL254: _tmp39F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL255:
 return Cyc_Absynpp_qvar2string(_tmp39F->name);case 3: _LL256: _tmp39E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL257: {
const char*_tmp6E3;void*_tmp6E2[1];struct Cyc_String_pa_PrintArg_struct _tmp6E1;return(struct _dyneither_ptr)((_tmp6E1.tag=0,((_tmp6E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp39E->name)),((_tmp6E2[0]=& _tmp6E1,Cyc_aprintf(((_tmp6E3="*%s",_tag_dyneither(_tmp6E3,sizeof(char),4))),_tag_dyneither(_tmp6E2,sizeof(void*),1))))))));}case 4: _LL258: _tmp39C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_tmp39D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL259: {
const char*_tmp6E8;void*_tmp6E7[2];struct Cyc_String_pa_PrintArg_struct _tmp6E6;struct Cyc_String_pa_PrintArg_struct _tmp6E5;return(struct _dyneither_ptr)((_tmp6E5.tag=0,((_tmp6E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39C->name),((_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp39D->name)),((_tmp6E7[0]=& _tmp6E6,((_tmp6E7[1]=& _tmp6E5,Cyc_aprintf(((_tmp6E8="%s<`%s>",_tag_dyneither(_tmp6E8,sizeof(char),8))),_tag_dyneither(_tmp6E7,sizeof(void*),2))))))))))))));}case 5: _LL25A: _LL25B: {
# 1681
const char*_tmp6EC;void*_tmp6EB[1];struct Cyc_String_pa_PrintArg_struct _tmp6EA;return(struct _dyneither_ptr)((_tmp6EA.tag=0,((_tmp6EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B)),((_tmp6EB[0]=& _tmp6EA,Cyc_aprintf(((_tmp6EC="$(%s)",_tag_dyneither(_tmp6EC,sizeof(char),6))),_tag_dyneither(_tmp6EB,sizeof(void*),1))))))));}case 6: _LL25C: _LL25D: {
# 1683
const char*_tmp6F0;void*_tmp6EF[1];struct Cyc_String_pa_PrintArg_struct _tmp6EE;return(struct _dyneither_ptr)((_tmp6EE.tag=0,((_tmp6EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B)),((_tmp6EF[0]=& _tmp6EE,Cyc_aprintf(((_tmp6F0="&%s",_tag_dyneither(_tmp6F0,sizeof(char),4))),_tag_dyneither(_tmp6EF,sizeof(void*),1))))))));}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f1)->aggr_info).KnownAggr).tag == 2){_LL25E: _tmp39A=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f1)->aggr_info).KnownAggr).val;_tmp39B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp391)->f3;_LL25F:
# 1685
 if(_tmp39A->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp3AE=_tmp39B;struct _dyneither_ptr*_tmp3AF;if(_tmp3AE != 0){if(((struct _tuple15*)((struct Cyc_List_List*)_tmp3AE)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp3AE)->hd)->f1)->hd)->tag == 1){_LL273: _tmp3AF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple15*)_tmp3AE->hd)->f1)->hd)->f1;_LL274: {
# 1688
const char*_tmp6F6;void*_tmp6F5[3];struct Cyc_String_pa_PrintArg_struct _tmp6F4;struct Cyc_String_pa_PrintArg_struct _tmp6F3;struct Cyc_String_pa_PrintArg_struct _tmp6F2;return(struct _dyneither_ptr)((_tmp6F2.tag=0,((_tmp6F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(_tmp38B)),((_tmp6F3.tag=0,((_tmp6F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AF),((_tmp6F4.tag=0,((_tmp6F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1688
Cyc_Absynpp_qvar2string(_tmp39A->name)),((_tmp6F5[0]=& _tmp6F4,((_tmp6F5[1]=& _tmp6F3,((_tmp6F5[2]=& _tmp6F2,Cyc_aprintf(((_tmp6F6="%s{.%s = %s}",_tag_dyneither(_tmp6F6,sizeof(char),13))),_tag_dyneither(_tmp6F5,sizeof(void*),3))))))))))))))))))));}}else{goto _LL275;}}else{goto _LL275;}}else{_LL275: _LL276:
# 1690
 goto _LL272;}_LL272:;}{
# 1693
const char*_tmp6FB;void*_tmp6FA[2];struct Cyc_String_pa_PrintArg_struct _tmp6F9;struct Cyc_String_pa_PrintArg_struct _tmp6F8;return(struct _dyneither_ptr)((_tmp6F8.tag=0,((_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B)),((_tmp6F9.tag=0,((_tmp6F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp39A->name)),((_tmp6FA[0]=& _tmp6F9,((_tmp6FA[1]=& _tmp6F8,Cyc_aprintf(((_tmp6FB="%s{%s}",_tag_dyneither(_tmp6FB,sizeof(char),7))),_tag_dyneither(_tmp6FA,sizeof(void*),2))))))))))))));};}else{goto _LL270;}}else{goto _LL270;}case 8: _LL260: _tmp399=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL261:
# 1695
 if(_tmp38B == 0){
const char*_tmp6FF;void*_tmp6FE[1];struct Cyc_String_pa_PrintArg_struct _tmp6FD;return(struct _dyneither_ptr)((_tmp6FD.tag=0,((_tmp6FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp399->name)),((_tmp6FE[0]=& _tmp6FD,Cyc_aprintf(((_tmp6FF="%s",_tag_dyneither(_tmp6FF,sizeof(char),3))),_tag_dyneither(_tmp6FE,sizeof(void*),1))))))));}else{
# 1698
const char*_tmp704;void*_tmp703[2];struct Cyc_String_pa_PrintArg_struct _tmp702;struct Cyc_String_pa_PrintArg_struct _tmp701;return(struct _dyneither_ptr)((_tmp701.tag=0,((_tmp701.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp38B)),((_tmp702.tag=0,((_tmp702.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp399->name)),((_tmp703[0]=& _tmp702,((_tmp703[1]=& _tmp701,Cyc_aprintf(((_tmp704="%s(%s)",_tag_dyneither(_tmp704,sizeof(char),7))),_tag_dyneither(_tmp703,sizeof(void*),2))))))))))))));}case 9: _LL262: _LL263: {
const char*_tmp705;return(_tmp705="NULL",_tag_dyneither(_tmp705,sizeof(char),5));}case 10: _LL264: _tmp398=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL265: {
const char*_tmp709;void*_tmp708[1];struct Cyc_Int_pa_PrintArg_struct _tmp707;return(struct _dyneither_ptr)((_tmp707.tag=1,((_tmp707.f1=(unsigned long)_tmp398,((_tmp708[0]=& _tmp707,Cyc_aprintf(((_tmp709="%d",_tag_dyneither(_tmp709,sizeof(char),3))),_tag_dyneither(_tmp708,sizeof(void*),1))))))));}case 11: _LL266: _tmp397=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL267: {
const char*_tmp70D;void*_tmp70C[1];struct Cyc_Int_pa_PrintArg_struct _tmp70B;return(struct _dyneither_ptr)((_tmp70B.tag=1,((_tmp70B.f1=(unsigned long)((int)_tmp397),((_tmp70C[0]=& _tmp70B,Cyc_aprintf(((_tmp70D="%d",_tag_dyneither(_tmp70D,sizeof(char),3))),_tag_dyneither(_tmp70C,sizeof(void*),1))))))));}case 12: _LL268: _tmp395=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_tmp396=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL269:
 return _tmp395;case 13: _LL26A: _tmp394=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL26B:
 _tmp393=_tmp394;goto _LL26D;case 14: _LL26C: _tmp393=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp391)->f2;_LL26D:
 return Cyc_Absynpp_qvar2string(_tmp393->name);case 17: _LL26E: _tmp392=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp391)->f1;_LL26F:
 return Cyc_Absynpp_exp2string(_tmp392);default: _LL270: _LL271:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL251:;};}}else{_LL24A: _tmp389=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp388)->f1;_LL24B:
# 1709
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp389)){const char*_tmp70E;return(_tmp70E="_",_tag_dyneither(_tmp70E,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp3C8=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp389);
# 1713
if((int)(((_tmp3C8->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
union Cyc_Tcpat_PatOrWhere _tmp70F;struct Cyc_Absyn_Pat*_tmp3C9=(_tmp70F=_tmp3C8->orig_pat,(((_tmp70F.pattern).tag != 1?_throw_match(): 1,(_tmp70F.pattern).val)));
void*_tmp3CA=Cyc_Tcutil_compress((void*)_check_null(_tmp3C9->topt));void*_tmp3CB=_tmp3CA;struct Cyc_Absyn_Aggrdecl*_tmp3CC;struct Cyc_Absyn_Datatypedecl*_tmp3CD;struct Cyc_Absyn_PtrAtts _tmp3CE;switch(*((int*)_tmp3CB)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3CB)->f2 == Cyc_Absyn_Char_sz){_LL278: _LL279:
# 1718
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp3CF=Cyc_Tcpat_char_con((char)i,_tmp3C9);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,_tmp3CF)){
const char*_tmp713;void*_tmp712[1];struct Cyc_Int_pa_PrintArg_struct _tmp711;return(struct _dyneither_ptr)((_tmp711.tag=1,((_tmp711.f1=(unsigned long)i,((_tmp712[0]=& _tmp711,Cyc_aprintf(((_tmp713="%d",_tag_dyneither(_tmp713,sizeof(char),3))),_tag_dyneither(_tmp712,sizeof(void*),1))))))));}}}
# 1723
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL27A: _LL27B:
# 1726
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp3D3=Cyc_Tcpat_int_con((int)i,_tmp3C8->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,_tmp3D3)){
const char*_tmp717;void*_tmp716[1];struct Cyc_Int_pa_PrintArg_struct _tmp715;return(struct _dyneither_ptr)((_tmp715.tag=1,((_tmp715.f1=(unsigned long)((int)i),((_tmp716[0]=& _tmp715,Cyc_aprintf(((_tmp717="%d",_tag_dyneither(_tmp717,sizeof(char),3))),_tag_dyneither(_tmp716,sizeof(void*),1))))))));}}}
# 1731
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 5: _LL27C: _tmp3CE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).ptr_atts;_LL27D: {
# 1733
union Cyc_Absyn_Constraint*_tmp3D7=_tmp3CE.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3D7);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,Cyc_Tcpat_null_con(_tmp3C9))){
const char*_tmp718;return(_tmp718="NULL",_tag_dyneither(_tmp718,sizeof(char),5));}}{
# 1739
const char*_tmp719;return(_tmp719="&_",_tag_dyneither(_tmp719,sizeof(char),3));};}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3CB)->f1).datatype_info).KnownDatatype).tag == 2){_LL27E: _tmp3CD=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3CB)->f1).datatype_info).KnownDatatype).val;_LL27F:
# 1741
 if(_tmp3CD->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp3DA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3CD->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3DA);
for(0;(unsigned int)_tmp3DA;_tmp3DA=_tmp3DA->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp3DA->hd)->name).f2;
struct Cyc_List_List*_tmp3DB=((struct Cyc_Absyn_Datatypefield*)_tmp3DA->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp71A;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,((_tmp71A=_cycalloc(sizeof(*_tmp71A)),((_tmp71A->name=Cyc_Tcpat_Name_v(n),((_tmp71A->arity=0,((_tmp71A->span=0,((_tmp71A->orig_pat=_tmp3C8->orig_pat,_tmp71A)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3DB)== 0)
return n;else{
# 1751
const char*_tmp71E;void*_tmp71D[1];struct Cyc_String_pa_PrintArg_struct _tmp71C;return(struct _dyneither_ptr)((_tmp71C.tag=0,((_tmp71C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp71D[0]=& _tmp71C,Cyc_aprintf(((_tmp71E="%s(...)",_tag_dyneither(_tmp71E,sizeof(char),8))),_tag_dyneither(_tmp71D,sizeof(void*),1))))))));}}}
# 1754
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL282;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CB)->f1).aggr_info).KnownAggr).tag == 2){_LL280: _tmp3CC=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CB)->f1).aggr_info).KnownAggr).val;if(_tmp3CC->kind == Cyc_Absyn_UnionA){_LL281: {
# 1756
struct Cyc_List_List*_tmp3E0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3CC->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3E0);
struct _tuple2*_tmp3E1=_tmp3CC->name;struct _tuple2*_tmp3E2=_tmp3E1;struct _dyneither_ptr _tmp3E3;_LL285: _tmp3E3=*_tmp3E2->f2;_LL286:
 for(0;(unsigned int)_tmp3E0;_tmp3E0=_tmp3E0->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp3E0->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp71F;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp389,((_tmp71F=_cycalloc(sizeof(*_tmp71F)),((_tmp71F->name=Cyc_Tcpat_Name_v(n),((_tmp71F->arity=0,((_tmp71F->span=0,((_tmp71F->orig_pat=_tmp3C8->orig_pat,_tmp71F)))))))))))){
const char*_tmp724;void*_tmp723[2];struct Cyc_String_pa_PrintArg_struct _tmp722;struct Cyc_String_pa_PrintArg_struct _tmp721;return(struct _dyneither_ptr)((_tmp721.tag=0,((_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp722.tag=0,((_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E3),((_tmp723[0]=& _tmp722,((_tmp723[1]=& _tmp721,Cyc_aprintf(((_tmp724="%s{.%s = _}",_tag_dyneither(_tmp724,sizeof(char),12))),_tag_dyneither(_tmp723,sizeof(void*),2))))))))))))));}}
# 1764
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL284:;}}else{goto _LL282;}}else{goto _LL282;}default: _LL282: _LL283:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL277:;};};}_LL247:;}
# 1769
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
struct _dyneither_ptr*_tmp725;return(_tmp725=_cycalloc(sizeof(*_tmp725)),((_tmp725[0]=Cyc_Tcpat_desc2string(d),_tmp725)));}
# 1772
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp3EC=_tmp3EB;for(0;_tmp3EC != 0;_tmp3EC=((struct Cyc_List_List*)_check_null(_tmp3EC))->tl){
if(_tmp3EC->tl != 0){
{struct Cyc_List_List*_tmp726;_tmp3EC->tl=((_tmp726=_cycalloc(sizeof(*_tmp726)),((_tmp726->hd=comma,((_tmp726->tl=_tmp3EC->tl,_tmp726))))));}
_tmp3EC=_tmp3EC->tl;}}}
# 1781
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp3EB);}
# 1784
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp3EF;_push_handler(& _tmp3EF);{int _tmp3F1=0;if(setjmp(_tmp3EF.handler))_tmp3F1=1;if(!_tmp3F1){
{struct _dyneither_ptr _tmp3F2=Cyc_Tcpat_desc2string(desc);
const char*_tmp72A;void*_tmp729[1];struct Cyc_String_pa_PrintArg_struct _tmp728;(_tmp728.tag=0,((_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F2),((_tmp729[0]=& _tmp728,Cyc_Tcutil_terr(loc,((_tmp72A="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp72A,sizeof(char),53))),_tag_dyneither(_tmp729,sizeof(void*),1)))))));}
# 1786
;_pop_handler();}else{void*_tmp3F0=(void*)_exn_thrown;void*_tmp3F6=_tmp3F0;void*_tmp3F7;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3F6)->tag == Cyc_Tcpat_Desc2string){_LL28A: _LL28B:
# 1790
{const char*_tmp72D;void*_tmp72C;(_tmp72C=0,Cyc_Tcutil_terr(loc,((_tmp72D="patterns may not be exhaustive.",_tag_dyneither(_tmp72D,sizeof(char),32))),_tag_dyneither(_tmp72C,sizeof(void*),0)));}
goto _LL289;}else{_LL28C: _tmp3F7=_tmp3F6;_LL28D:(int)_rethrow(_tmp3F7);}_LL289:;}};}
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
struct _tuple22*_tmp3FF=(struct _tuple22*)_tmp3FC->hd;struct _tuple22*_tmp400=_tmp3FF;int _tmp401;unsigned int _tmp402;_LL28F: _tmp401=(_tmp400->f2)->used;_tmp402=(_tmp400->f2)->pat_loc;_LL290:
 if(!_tmp401){
{const char*_tmp730;void*_tmp72F;(_tmp72F=0,Cyc_Tcutil_terr(_tmp402,((_tmp730="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp730,sizeof(char),74))),_tag_dyneither(_tmp72F,sizeof(void*),0)));}
# 1818
break;}_LL28E:;}};}
# 1827
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp405;_push_handler(& _tmp405);{int _tmp407=0;if(setjmp(_tmp405.handler))_tmp407=1;if(!_tmp407){
{struct _dyneither_ptr _tmp408=Cyc_Tcpat_desc2string(desc);
const char*_tmp734;void*_tmp733[1];struct Cyc_String_pa_PrintArg_struct _tmp732;(_tmp732.tag=0,((_tmp732.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp408),((_tmp733[0]=& _tmp732,Cyc_Tcutil_warn((*pr).f1,((_tmp734="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp734,sizeof(char),45))),_tag_dyneither(_tmp733,sizeof(void*),1)))))));}
# 1830
;_pop_handler();}else{void*_tmp406=(void*)_exn_thrown;void*_tmp40C=_tmp406;void*_tmp40D;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp40C)->tag == Cyc_Tcpat_Desc2string){_LL294: _LL295:
# 1834
{const char*_tmp737;void*_tmp736;(_tmp736=0,Cyc_Tcutil_warn((*pr).f1,((_tmp737="pattern not exhaustive.",_tag_dyneither(_tmp737,sizeof(char),24))),_tag_dyneither(_tmp736,sizeof(void*),0)));}
goto _LL293;}else{_LL296: _tmp40D=_tmp40C;_LL297:(int)_rethrow(_tmp40D);}_LL293:;}};};}
# 1838
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1842
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*_tmp738;struct Cyc_Tcpat_Rhs*rhs=(_tmp738=_cycalloc(sizeof(*_tmp738)),((_tmp738->used=0,((_tmp738->pat_loc=p->loc,((_tmp738->rhs=Cyc_Absyn_skip_stmt(0),_tmp738)))))));
struct _tuple22*_tmp73B;struct Cyc_List_List*_tmp73A;struct Cyc_List_List*_tmp410=(_tmp73A=_cycalloc(sizeof(*_tmp73A)),((_tmp73A->hd=((_tmp73B=_cycalloc(sizeof(*_tmp73B)),((_tmp73B->f1=Cyc_Tcpat_compile_pat(p),((_tmp73B->f2=rhs,_tmp73B)))))),((_tmp73A->tl=0,_tmp73A)))));
void*_tmp411=Cyc_Tcpat_match_compile(_tmp410);
struct _tuple14 _tmp73C;struct _tuple14 _tmp412=(_tmp73C.f1=loc,((_tmp73C.f2=1,_tmp73C)));
int _tmp413=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp411,Cyc_Tcpat_not_exhaust_warn,& _tmp412,Cyc_Tcpat_dummy_fn,0,& _tmp413);
# 1850
*dopt=_tmp411;
return _tmp412.f2;}
# 1858
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*_tmp73D;struct Cyc_Tcpat_Rhs*rhs=(_tmp73D=_cycalloc(sizeof(*_tmp73D)),((_tmp73D->used=0,((_tmp73D->pat_loc=(swc->pattern)->loc,((_tmp73D->rhs=swc->body,_tmp73D)))))));
struct _tuple22*_tmp73E;return(_tmp73E=_cycalloc(sizeof(*_tmp73E)),((_tmp73E->f1=sp0,((_tmp73E->f2=rhs,_tmp73E)))));}
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
struct _tuple22*_tmp41D=(struct _tuple22*)_tmp41A->hd;struct _tuple22*_tmp41E=_tmp41D;int _tmp41F;unsigned int _tmp420;_LL299: _tmp41F=(_tmp41E->f2)->used;_tmp420=(_tmp41E->f2)->pat_loc;_LL29A:
 if(!_tmp41F){
{const char*_tmp741;void*_tmp740;(_tmp740=0,Cyc_Tcutil_terr(_tmp420,((_tmp741="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp741,sizeof(char),74))),_tag_dyneither(_tmp740,sizeof(void*),0)));}
break;}_LL298:;};}};}
